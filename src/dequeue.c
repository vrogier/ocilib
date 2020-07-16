/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2020 Vincent ROGIER <vince.rogier@ocilib.net>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "dequeue.h"

#include "agent.h"
#include "callback.h"
#include "macros.h"
#include "memory.h"
#include "message.h"
#include "object.h"
#include "strings.h"

static const unsigned int NavigationModeValues[] =
{
    OCI_ADN_FIRST_MSG,
    OCI_ADN_NEXT_MSG,
    OCI_ADN_NEXT_TRANSACTION
};

static const unsigned int VisibilityModeValues[] =
{
    OCI_AMV_IMMEDIATE,
    OCI_AMV_ON_COMMIT
};

static const unsigned int DequeueModeValues[]=
{
    OCI_ADM_BROWSE,
    OCI_ADM_LOCKED,
    OCI_ADM_REMOVE,
    OCI_ADM_REMOVE_NODATA
};

/* --------------------------------------------------------------------------------------------- *
 * DequeueCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Dequeue * DequeueCreate
(
    OCI_TypeInfo *typinf,
    const otext  *name
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Dequeue*, NULL,
        /* context */ OCI_IPC_TYPE_INFO, typinf
    )

    OCI_Dequeue* dequeue = NULL;

    CHECK_PTR(OCI_IPC_TYPE_INFO, typinf)
    CHECK_PTR(OCI_IPC_STRING,    name)

    /* allocate dequeue structure */

    ALLOC_DATA(OCI_IPC_DEQUEUE, dequeue, 1)

    dequeue->typinf = typinf;
    dequeue->name   = ostrdup(name);

    /* allocate dequeue options descriptor */

    CHECK(MemoryAllocDescriptor((dvoid *)dequeue->typinf->con->env,
                                (dvoid **)&dequeue->opth,
                                OCI_DTYPE_AQDEQ_OPTIONS))

    /* create local message for OCI_DequeueGet() */

    dequeue->msg = MessageCreate(dequeue->typinf);
    CHECK_NULL(dequeue->msg)

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE)
        {
            DequeueFree(dequeue);
            dequeue = NULL;
        }

        SET_RETVAL(dequeue)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * DequeueFree
 * --------------------------------------------------------------------------------------------- */

boolean DequeueFree
(
    OCI_Dequeue *dequeue
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_DEQUEUE, dequeue
    )

    CHECK_PTR(OCI_IPC_DEQUEUE, dequeue)

    /* Unsubscribe notification if needed */

    if (NULL != dequeue->subhp)
    {
        DequeueUnsubscribe(dequeue);
    }

    /* free local message  */

    if (NULL != dequeue->msg)
    {
        MessageFree(dequeue->msg);
    }

    /* free local agent  */

    if (NULL != dequeue->agent)
    {
        AgentFree(dequeue->agent);
    }

    /* free OCI descriptor */

    MemoryFreeDescriptor((dvoid*)dequeue->opth, OCI_DTYPE_AQDEQ_OPTIONS);

    /* free data  */

    ErrorResetSource(NULL, dequeue);

    FREE(dequeue->name)
    FREE(dequeue->pattern)
    FREE(dequeue->consumer)
    FREE(dequeue->agent_list)
    FREE(dequeue)

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * DequeueListen
 * --------------------------------------------------------------------------------------------- */

OCI_Agent * DequeueListen
(
    OCI_Dequeue *dequeue,
    int          timeout
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Agent*, NULL,
        /* context */ OCI_IPC_DEQUEUE, dequeue
    )

    CHECK_PTR(OCI_IPC_DEQUEUE, dequeue)

    /* listen only if OCI_DequeueSetAgentList has been called */

    CHECK_NULL(dequeue->agent_list)

    sword ret = OCI_SUCCESS;
    sb4 code = OCI_SUCCESS;

    OCIAQAgent *handle = NULL;

    ret =  OCIAQListen(dequeue->typinf->con->cxt, dequeue->typinf->con->err,
                       dequeue->agent_list, (ub4) dequeue->agent_count,
                       (sb4) timeout, &handle, OCI_DEFAULT);

    /* check returned error code */

    if (OCI_ERROR == ret)
    {
        OCIErrorGet((dvoid *) dequeue->typinf->con->err, (ub4) 1,
                    (OraText *) NULL, &code, (OraText *) NULL, (ub4) 0,
                    (ub4) OCI_HTYPE_ERROR);

        /* raise error only if the call has not been timed out */

        if (OCI_ERR_AQ_LISTEN_TIMEOUT != code)
        {
            THROW(ExceptionOCI, dequeue->typinf->con->err, ret)
        }
    }

    /* initialize local agent object */

    OCI_Agent* agent = NULL;

    if (NULL != handle && OCI_SUCCESSFUL(ret))
    {
        dequeue->agent= AgentInitialize(dequeue->typinf->con,
                                        dequeue->agent, handle,
                                        NULL, NULL);

        CHECK_NULL(dequeue->agent)

        agent = dequeue->agent;
    }

    SET_RETVAL(agent)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * DequeueGetMessage
 * --------------------------------------------------------------------------------------------- */

OCI_Msg * DequeueGetMessage
(
    OCI_Dequeue *dequeue
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Msg*, NULL,
        /* context */ OCI_IPC_DEQUEUE, dequeue
    )

    sword ret = OCI_ERROR;
    void *p_ind = NULL;

    int     dbsize = -1;
    dbtext* dbstr  = NULL;

    char* ansi_queue_name = NULL;

    CHECK_PTR(OCI_IPC_DEQUEUE, dequeue)

    /* reset message */

    CHECK(MessageReset(dequeue->msg))

    /* dequeue message */

    dbstr = StringGetDBString(dequeue->name, &dbsize);

    if (OCI_UNKNOWN == dequeue->typinf->typecode)
    {
        p_ind = &dequeue->msg->ind;
    }

    /* OCIAQDeq() parameter  'queue_name' is supposed to be either ANSI or UTF16 depending on the
    * environment. It appears that whatever mode is used, OCIAQDeq() only takes ANSI strings !
    * Oracle might fix this issue at some point.
    * Thus let's try to handle oracle future version fixing this issue.
    * Make a first attempt using the correct way.
    * If an error is reported let's try again using an ANSI string.
    */

#ifdef OCI_CHARSET_WIDE
    const size_t attempt_max = 2;
#else
    const size_t attempt_max = 1;
#endif

    size_t attempt_count = 0;

    while (OCI_ERROR == ret && ++attempt_count <= attempt_max)
    {
        void* name = attempt_count == 1 ? (void*)dbstr : (void*) ansi_queue_name;

        ret = OCIAQDeq(dequeue->typinf->con->cxt, dequeue->typinf->con->err,
                       (OraText *)name, dequeue->opth, dequeue->msg->proph,
                       dequeue->typinf->tdo, &dequeue->msg->payload,
                       (void **) &p_ind, &dequeue->msg->id, OCI_DEFAULT);

        /* check returned error code */

        if (OCI_ERROR == ret)
        {
            sb4 code = 0;

            OCIErrorGet((dvoid *) dequeue->typinf->con->err, (ub4) 1,
                        (OraText *) NULL, &code, (OraText *) NULL, (ub4) 0,
                        (ub4) OCI_HTYPE_ERROR);

        #ifdef OCI_CHARSET_WIDE

            if (dbstr == name && (OCI_ERR_AQ_QUEUE_NAME_INVALID == code ||
                                  OCI_ERR_AQ_QUEUE_NOT_EXIST == code))
            {
                /* non valid queue name in UTF16 mode, we have an Oracle client with OCIAQDeq()
                 * accepting only ANSI strings.* Let's try again with an ANSI string for queue name
                 */

                const int len = (int)ostrlen(dequeue->name);
                ansi_queue_name = MemoryAlloc(OCI_IPC_STRING, sizeof(char), len + 1, FALSE);
                StringNativeToAnsi(dequeue->name, ansi_queue_name, len);
            }
            else

#endif
            /* raise error only if the call has not been timed out */

            if (OCI_ERR_AQ_DEQUEUE_TIMEOUT != code)
            {
                THROW(ExceptionOCI, dequeue->typinf->con->err, ret)
            }
        }
    }

    /* reset message */

    OCI_Msg* msg = NULL;

    if (OCI_SUCCESSFUL(ret))
    {
        /* get payload */

        if (OCI_UNKNOWN != dequeue->typinf->typecode)
        {
            if (p_ind && (OCI_IND_NULL != (*(OCIInd *) p_ind)))
            {
                dequeue->msg->ind = *(OCIInd *) p_ind;

                dequeue->msg->obj = ObjectInitialize
                                    (
                                        dequeue->typinf->con, 
                                        (OCI_Object*)dequeue->msg->obj,
                                        dequeue->msg->payload, 
                                        dequeue->typinf,
                                        NULL, -1, TRUE
                                    );

                CHECK_NULL(dequeue->msg->obj)

                msg = dequeue->msg;
            }
        }
    }

    SET_RETVAL(msg)

    CLEANUP_AND_EXIT_FUNC
    (
        StringReleaseDBString(dbstr);
        
        FREE(ansi_queue_name)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * DequeueGetConsumerName
 * --------------------------------------------------------------------------------------------- */

const otext * DequeueGetConsumer
(
    OCI_Dequeue *dequeue
)
{
    ENTER_FUNC
    (
        /* returns */ const otext*, NULL,
        /* context */ OCI_IPC_DEQUEUE, dequeue
    )

    CHECK_PTR(OCI_IPC_DEQUEUE, dequeue)

    if (NULL == dequeue->consumer)
    {
        unsigned int size = 0;

        CHECK(StringGetAttribute(dequeue->typinf->con, dequeue->opth,
                                 OCI_DTYPE_AQDEQ_OPTIONS,
                                 OCI_ATTR_CONSUMER_NAME,
                                 &dequeue->consumer, &size))
    }

    SET_RETVAL(dequeue->consumer)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * DequeueSetConsumerName
 * --------------------------------------------------------------------------------------------- */

boolean DequeueSetConsumer
(
    OCI_Dequeue *dequeue,
    const otext *consumer
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_DEQUEUE, dequeue
    )

    CHECK_PTR(OCI_IPC_DEQUEUE, dequeue)

    CHECK(StringSetAttribute(dequeue->typinf->con, dequeue->opth,
                             OCI_DTYPE_AQDEQ_OPTIONS,
                             OCI_ATTR_CONSUMER_NAME,
                             &dequeue->consumer, consumer))

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * DequeueGetCorrelation
 * --------------------------------------------------------------------------------------------- */

const otext * DequeueGetCorrelation
(
    OCI_Dequeue *dequeue
)
{
    ENTER_FUNC
    (
        /* returns */ const otext*, NULL,
        /* context */ OCI_IPC_DEQUEUE, dequeue
    )

    CHECK_PTR(OCI_IPC_DEQUEUE, dequeue)

    if (NULL == dequeue->pattern)
    {
        unsigned int size = 0;

        CHECK(StringGetAttribute(dequeue->typinf->con, dequeue->opth,
                                 OCI_DTYPE_AQDEQ_OPTIONS,
                                 OCI_ATTR_CORRELATION,
                                 &dequeue->pattern, &size))
    }

    SET_RETVAL(dequeue->pattern)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * DequeueSetCorrelation
 * --------------------------------------------------------------------------------------------- */

boolean DequeueSetCorrelation
(
    OCI_Dequeue *dequeue,
    const otext *pattern
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_DEQUEUE, dequeue
    )

    CHECK_PTR(OCI_IPC_DEQUEUE, dequeue)

    CHECK(StringSetAttribute(dequeue->typinf->con, dequeue->opth,
                             OCI_DTYPE_AQDEQ_OPTIONS,
                             OCI_ATTR_CORRELATION,
                             &dequeue->pattern, pattern))

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * DequeueGetRelativeMsgID
 * --------------------------------------------------------------------------------------------- */

boolean DequeueGetRelativeMsgID
(
    OCI_Dequeue  *dequeue,
    void         *id,
    unsigned int *len
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_DEQUEUE, dequeue
    )

    OCIRaw *value = NULL;

    CHECK_PTR(OCI_IPC_DEQUEUE, dequeue)
    CHECK_PTR(OCI_IPC_VOID,    id)
    CHECK_PTR(OCI_IPC_VOID,    len)

    *len = 0;

    CHECK_ATTRIB_GET
    (
        OCI_DTYPE_AQDEQ_OPTIONS, OCI_ATTR_DEQ_MSGID,
        dequeue->opth, &value, NULL,
        dequeue->typinf->con->err
    )

    if (NULL != value)
    {
        const ub4 raw_len = OCIRawSize(dequeue->typinf->con->env, value);

        if (*len > raw_len)
        {
            *len = raw_len;
        }

        memcpy(id, OCIRawPtr(dequeue->typinf->con->env, value), (size_t) (*len));
    }

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * DequeueSetRelativeMsgID
 * --------------------------------------------------------------------------------------------- */

boolean DequeueSetRelativeMsgID
(
    OCI_Dequeue *dequeue,
    const void  *id,
    unsigned int len
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_DEQUEUE, dequeue
    )

    OCIRaw *value = NULL;

    CHECK_PTR(OCI_IPC_DEQUEUE, dequeue)

    CHECK_OCI
    (
        dequeue->typinf->con->err,
        OCIRawAssignBytes,
        dequeue->typinf->con->env, dequeue->typinf->con->err,
        (ub1*) id, (ub4) len, (OCIRaw **) &value
    )

    CHECK_ATTRIB_SET
    (
        OCI_DTYPE_AQDEQ_OPTIONS, OCI_ATTR_DEQ_MSGID,
        dequeue->opth, &value, 0,
        dequeue->typinf->con->err
    )

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * DequeueGetVisibility
 * --------------------------------------------------------------------------------------------- */

unsigned int DequeueGetVisibility
(
    OCI_Dequeue *dequeue
)
{
    ENTER_FUNC
    (
        /* returns */ unsigned int, OCI_UNKNOWN,
        /* context */ OCI_IPC_DEQUEUE, dequeue
    )

    ub4 value = OCI_UNKNOWN;

    CHECK_PTR(OCI_IPC_DEQUEUE, dequeue)

    CHECK_ATTRIB_GET
    (
        OCI_DTYPE_AQDEQ_OPTIONS, OCI_ATTR_VISIBILITY,
        dequeue->opth, &value, NULL,
        dequeue->typinf->con->err
    )

    SET_RETVAL((unsigned int)value)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * DequeueSetVisibility
 * --------------------------------------------------------------------------------------------- */

boolean DequeueSetVisibility
(
    OCI_Dequeue *dequeue,
    unsigned int visibility
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_DEQUEUE, dequeue
    )

    ub4 value = (ub4) visibility;

    CHECK_PTR(OCI_IPC_DEQUEUE, dequeue)
    CHECK_ENUM_VALUE(visibility, VisibilityModeValues, OTEXT("Visibility Mode"))

    CHECK_ATTRIB_SET
    (
        OCI_DTYPE_AQDEQ_OPTIONS, OCI_ATTR_VISIBILITY,
        dequeue->opth, &value, 0,
        dequeue->typinf->con->err
    )

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * DequeueGetMode
 * --------------------------------------------------------------------------------------------- */

unsigned int DequeueGetMode
(
    OCI_Dequeue *dequeue
)
{
    ENTER_FUNC
    (
        /* returns */ unsigned int, OCI_UNKNOWN,
        /* context */ OCI_IPC_DEQUEUE, dequeue
    )

    ub4 value = OCI_UNKNOWN;

    CHECK_PTR(OCI_IPC_DEQUEUE, dequeue)

    CHECK_ATTRIB_GET
    (
        OCI_DTYPE_AQDEQ_OPTIONS, OCI_ATTR_DEQ_MODE,
        dequeue->opth, &value, NULL,
        dequeue->typinf->con->err
    )

    SET_RETVAL(value)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * DequeueSetMode
 * --------------------------------------------------------------------------------------------- */

boolean DequeueSetMode
(
    OCI_Dequeue *dequeue,
    unsigned int mode
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_DEQUEUE, dequeue
    )

    ub4 value = (ub4) mode;

    CHECK_PTR(OCI_IPC_DEQUEUE, dequeue)
    CHECK_ENUM_VALUE(mode, DequeueModeValues, OTEXT("Dequeue Mode"))

    CHECK_ATTRIB_SET
    (
        OCI_DTYPE_AQDEQ_OPTIONS, OCI_ATTR_DEQ_MODE,
        dequeue->opth, &value, 0,
        dequeue->typinf->con->err
    )

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * DequeueGetNavigation
 * --------------------------------------------------------------------------------------------- */

unsigned int DequeueGetNavigation
(
    OCI_Dequeue *dequeue
)
{
    ENTER_FUNC
    (
        /* returns */ unsigned int, OCI_UNKNOWN,
        /* context */ OCI_IPC_DEQUEUE, dequeue
    )

    ub4 value = OCI_UNKNOWN;

    CHECK_PTR(OCI_IPC_DEQUEUE, dequeue)

    CHECK_ATTRIB_GET
    (
        OCI_DTYPE_AQDEQ_OPTIONS, OCI_ATTR_NAVIGATION,
        dequeue->opth, &value, NULL,
        dequeue->typinf->con->err
    )

    SET_RETVAL(value)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * DequeueSetNavigation
 * --------------------------------------------------------------------------------------------- */

boolean DequeueSetNavigation
(
    OCI_Dequeue *dequeue,
    unsigned int position
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_DEQUEUE, dequeue
    )

    ub4 value = (ub4) position;

    CHECK_PTR(OCI_IPC_DEQUEUE, dequeue)
    CHECK_ENUM_VALUE(position, NavigationModeValues, OTEXT("Navigation Mode"))

    CHECK_ATTRIB_SET
    (
        OCI_DTYPE_AQDEQ_OPTIONS, OCI_ATTR_NAVIGATION,
        dequeue->opth, &value, 0,
        dequeue->typinf->con->err
    )

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * DequeueGetWaitTime
 * --------------------------------------------------------------------------------------------- */

int DequeueGetWaitTime
(
    OCI_Dequeue *dequeue
)
{
    ENTER_FUNC
    (
        /* returns */ unsigned int, 0,
        /* context */ OCI_IPC_DEQUEUE, dequeue
    )

    sb4 value = 0;

    CHECK_PTR(OCI_IPC_DEQUEUE, dequeue)

    CHECK_ATTRIB_GET
    (
        OCI_DTYPE_AQDEQ_OPTIONS, OCI_ATTR_WAIT,
        dequeue->opth, &value, NULL,
        dequeue->typinf->con->err
    )

    SET_RETVAL(value)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * DequeueSetWaitTime
 * --------------------------------------------------------------------------------------------- */

boolean DequeueSetWaitTime
(
    OCI_Dequeue *dequeue,
    int          timeout
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_DEQUEUE, dequeue
    )

    sb4 value = (ub4) timeout;

    CHECK_PTR(OCI_IPC_DEQUEUE, dequeue)

    CHECK_ATTRIB_SET
    (
        OCI_DTYPE_AQDEQ_OPTIONS, OCI_ATTR_WAIT,
        dequeue->opth, &value, 0,
        dequeue->typinf->con->err
    )

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * DequeueSetAgentList
 * --------------------------------------------------------------------------------------------- */

boolean DequeueSetAgentList
(
    OCI_Dequeue *dequeue,
    OCI_Agent  **consumers,
    unsigned int count
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_DEQUEUE, dequeue
    )

    CHECK_PTR(OCI_IPC_ENQUEUE, dequeue)

    FREE(dequeue->agent_list)

    if (NULL != consumers && count > 0)
    {
        ALLOC_DATA(OCI_IPC_ARRAY, dequeue->agent_list, count)

        for (unsigned int i = 0; i < count; i++)
        {
            dequeue->agent_list[i] = consumers[i]->handle;
        }

        dequeue->agent_count = (ub4)count;
    }

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * DequeueSubscribe
 * --------------------------------------------------------------------------------------------- */

boolean DequeueSubscribe
(
    OCI_Dequeue   *dequeue,
    unsigned int   port,
    unsigned int   timeout,
    POCI_NOTIFY_AQ callback
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_DEQUEUE, dequeue
    )

#if OCI_VERSION_COMPILE >= OCI_10_2

    ub4 oci_port = (ub4) port;
    ub4 oci_timeout  = (ub4) timeout;
    ub4 oci_protocol = OCI_SUBSCR_PROTO_OCI;
    ub4 oci_msgpres  = OCI_SUBSCR_PRES_DEFAULT;

#endif

    ub4 oci_namespace = OCI_SUBSCR_NAMESPACE_AQ;

    OCI_Connection *con = NULL;

    dbtext* dbstr  = NULL;
    int     dbsize = -1;

    CHECK_DATABASE_NOTIFY_ENABLED()
    CHECK_PTR(OCI_IPC_DEQUEUE, dequeue)

    con = dequeue->typinf->con;

    /* clear any previous subscription */

    CHECK(DequeueUnsubscribe(dequeue))

    /* allocate subscription handle */

    CHECK(MemoryAllocHandle(con->env,
                            (dvoid **) (void *) &dequeue->subhp,
                            OCI_HTYPE_SUBSCRIPTION))

#if OCI_VERSION_COMPILE >= OCI_10_2

    /* set port number */

    if (oci_port > 0)
    {
        CHECK_ATTRIB_GET
        (
            OCI_HTYPE_SUBSCRIPTION, OCI_ATTR_SUBSCR_PORTNO,
            dequeue->subhp, &oci_port, sizeof(oci_port),
            dequeue->typinf->con->err
        )
    }

    /* set timeout */

    if (oci_timeout > 0)
    {
        CHECK_ATTRIB_GET
        (
            OCI_HTYPE_SUBSCRIPTION, OCI_ATTR_SUBSCR_TIMEOUT,
            dequeue->subhp, &oci_timeout, sizeof(oci_timeout),
            dequeue->typinf->con->err
        )
    }

    /* set protocol  */

    CHECK_ATTRIB_SET
    (
        OCI_HTYPE_SUBSCRIPTION, OCI_ATTR_SUBSCR_RECPTPROTO,
        dequeue->subhp, &oci_protocol, sizeof(oci_protocol),
        dequeue->typinf->con->err
    )

    /* set presentation  */

    CHECK_ATTRIB_SET
    (
        OCI_HTYPE_SUBSCRIPTION, OCI_ATTR_SUBSCR_RECPTPRES,
        dequeue->subhp, &oci_msgpres, sizeof(oci_msgpres),
        dequeue->typinf->con->err
    )

#else

    OCI_NOT_USED(port)
    OCI_NOT_USED(timeout)

#endif

    /* set name  */

    if (NULL != dequeue->name)
    {
        /* for AQ subscription, the name should be "[schema.]queue[:consumer]" */

        otext buffer[OCI_SIZE_BUFFER] = OTEXT("");

        otext *str = NULL;

        size_t max_chars = sizeof(buffer) / sizeof(otext) - 1;

        ostrncat(buffer, dequeue->name, max_chars);

        if (dequeue->consumer)
        {
            max_chars -= ostrlen(dequeue->name);
            ostrncat(buffer, OTEXT(":"),        max_chars);
            max_chars -= (size_t)1;

            ostrncat(buffer, dequeue->consumer, max_chars);
        }

        /* queue name must be uppercase */

        for (str = buffer; *str != 0; str++)
        {
            *str = (otext) otoupper(*str);
        }

        dbstr = StringGetDBString(buffer, &dbsize);

        CHECK_ATTRIB_SET
        (
            OCI_HTYPE_SUBSCRIPTION, OCI_ATTR_SUBSCR_NAME,
            dequeue->subhp, dbstr, dbsize,
            dequeue->typinf->con->err
        )
    }

    /* set namespace  */

    CHECK_ATTRIB_SET
    (
        OCI_HTYPE_SUBSCRIPTION, OCI_ATTR_SUBSCR_NAMESPACE,
        dequeue->subhp, &oci_namespace, oci_namespace,
        dequeue->typinf->con->err
    )

    /* set context pointer to dequeue structure */

    CHECK_ATTRIB_SET
    (
        OCI_HTYPE_SUBSCRIPTION, OCI_ATTR_SUBSCR_CTX,
        dequeue->subhp, dequeue, 0,
        dequeue->typinf->con->err
    )

    /* On MSVC, casting a function pointer to a data pointer generates a warning.
        As there is no other to way to do regarding the OCI API, let's disable this
        warning just the time to set the callback attribute to the dequeue handle */

#ifdef _MSC_VER
  #pragma warning(disable: 4054)
#endif

    /* internal callback handler */

    CHECK_ATTRIB_SET
    (
        OCI_HTYPE_SUBSCRIPTION, OCI_ATTR_SUBSCR_CALLBACK,
        dequeue->subhp, CallbackNotifyMessages, 0,
        dequeue->typinf->con->err
    )

#ifdef _MSC_VER
  #pragma warning(default: 4054)
#endif

    /* set callback */

    dequeue->callback = callback;

    /* all attributes set, let's register the subscription ! */

    CHECK_OCI
    (
        dequeue->typinf->con->err,
        OCISubscriptionRegister,
        con->cxt, &dequeue->subhp, (ub2) 1,
        con->err,(ub4) OCI_DEFAULT
    )

    SET_SUCCESS()

    CLEANUP_AND_EXIT_FUNC
    (
        StringReleaseDBString(dbstr);

        if (FAILURE && NULL != dequeue)
        {
            DequeueUnsubscribe(dequeue);
        }
    )
}

/* --------------------------------------------------------------------------------------------- *
 * DequeueUnsubscribe
 * --------------------------------------------------------------------------------------------- */

boolean DequeueUnsubscribe
(
    OCI_Dequeue *dequeue
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_DEQUEUE, dequeue
    )

    CHECK_DATABASE_NOTIFY_ENABLED()
    CHECK_PTR(OCI_IPC_DEQUEUE, dequeue)

    dequeue->callback = NULL;

    if (NULL != dequeue->subhp)
    {
        /* unregister the subscription */

        CHECK_OCI
        (
            dequeue->typinf->con->err,
            OCISubscriptionUnRegister,
            dequeue->typinf->con->cxt, dequeue->subhp,
            dequeue->typinf->con->err,(ub4) OCI_DEFAULT
        )

        /* free OCI handle */

        MemoryFreeHandle((dvoid *) dequeue->subhp, OCI_HTYPE_SUBSCRIPTION);

        dequeue->subhp = NULL;
    }

    SET_SUCCESS()

    EXIT_FUNC()
}
