/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2026 Vincent ROGIER <vince.rogier@ocilib.net>
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
#include "stringutils.h"

static const OCI_UInt32 NavigationModeValues[] =
{
    OCI_ADN_FIRST_MSG,
    OCI_ADN_NEXT_MSG,
    OCI_ADN_NEXT_TRANSACTION
};

static const OCI_UInt32 VisibilityModeValues[] =
{
    OCI_AMV_IMMEDIATE,
    OCI_AMV_ON_COMMIT
};

static const OCI_UInt32 OcilibDequeueModeValues[]=
{
    OCI_ADM_BROWSE,
    OCI_ADM_LOCKED,
    OCI_ADM_REMOVE,
    OCI_ADM_REMOVE_NODATA
};

/* --------------------------------------------------------------------------------------------- *
 * OcilibDequeueCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Dequeue * OcilibDequeueCreate
(
    OCI_TypeInfo *typinf,
    const otext  *name
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Dequeue*, NULL,
        /* context */ OCI_IPC_TYPE_INFO, typinf
    );

    OCI_Dequeue* dequeue = NULL;

    CHECK_PTR(OCI_IPC_TYPE_INFO, typinf);
    CHECK_PTR(OCI_IPC_STRING,    name);

    /* allocate dequeue structure */

    ALLOC_HANDLE(OCI_IPC_DEQUEUE, dequeue);

    dequeue->typinf = typinf;
    dequeue->name   = OcilibStringDuplicate(dequeue->typinf->con->env, name);

    /* allocate dequeue options descriptor */

    CHECK(OcilibMemoryAllocDescriptor(dequeue->typinf->con->env,
                                      (dvoid *)dequeue->typinf->con->env->handle,
                                      (dvoid **)&dequeue->handle,
                                      OCI_DTYPE_AQDEQ_OPTIONS));

    /* create local message for OCI_DequeueGet() */

    dequeue->msg = OcilibMessageCreate(dequeue->typinf);
    CHECK_NULL(dequeue->msg);

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE)
        {
            OcilibDequeueFree(dequeue);
            dequeue = NULL;
        }

        SET_RETVAL(dequeue);
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDequeueFree
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibDequeueFree
(
    OCI_Dequeue *dequeue
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_DEQUEUE, dequeue
    );

    CHECK_PTR(OCI_IPC_DEQUEUE, dequeue);

    /* Unsubscribe notification if needed */

    if (NULL != dequeue->subhp)
    {
        OcilibDequeueUnsubscribe(dequeue);
    }

    /* free local message  */

    if (NULL != dequeue->msg)
    {
        OcilibMessageFree(dequeue->msg);
    }

    /* free local agent  */

    if (NULL != dequeue->agent)
    {
        OcilibAgentFree(dequeue->agent);
    }

    /* free OCI descriptor */

    OcilibMemoryFreeDescriptor(dequeue->typinf->con->env, (dvoid*)dequeue->handle, OCI_DTYPE_AQDEQ_OPTIONS);

    /* free data  */

    OcilibErrorResetSource(NULL, dequeue);

    FREE(dequeue->name);
    FREE(dequeue->pattern);
    FREE(dequeue->consumer);
    FREE(dequeue->agent_list);
    FREE(dequeue);

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDequeueListen
 * --------------------------------------------------------------------------------------------- */

OCI_Agent * OcilibDequeueListen
(
    OCI_Dequeue *dequeue,
    OCI_Int32    timeout
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Agent*, NULL,
        /* context */ OCI_IPC_DEQUEUE, dequeue
    );

    CHECK_PTR(OCI_IPC_DEQUEUE, dequeue);

    /* listen only if OCI_DequeueSetAgentList has been called */

    CHECK_NULL(dequeue->agent_list);

    sword ret  = OCI_SUCCESS;
    sb4   code = OCI_SUCCESS;

    OCIAQAgent *handle = NULL;

    OCI_CALL(ret, OCIAQListen, dequeue->typinf->con->handle, dequeue->typinf->con->err,
             dequeue->agent_list, (ub4) dequeue->agent_count,
             (sb4) timeout, &handle, OCI_DEFAULT);

    /* check returned error code */

    if (OCI_ERROR == ret)
    {
        OCI_ERROR_GET(dequeue->typinf->con->err, &code, NULL, 0);

        /* raise error only if the call has not been timed out */

        if (OCI_ERR_AQ_LISTEN_TIMEOUT != code)
        {
            THROW(OcilibExceptionOCI, dequeue->typinf->con->err, ret);
        }
    }

    /* initialize local agent object */

    OCI_Agent* agent = NULL;

    if (NULL != handle && OCI_SUCCESSFUL(ret))
    {
        dequeue->agent= OcilibAgentInitialize(dequeue->typinf->con,
                                              dequeue->agent, handle,
                                              NULL, NULL);

        CHECK_NULL(dequeue->agent);

        agent = dequeue->agent;
    }

    SET_RETVAL(agent);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDequeueGetMessage
 * --------------------------------------------------------------------------------------------- */

OCI_Message * OcilibDequeueGetMessage
(
    OCI_Dequeue *dequeue
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Message*, NULL,
        /* context */ OCI_IPC_DEQUEUE, dequeue
    );

    sword ret   = OCI_ERROR;
    void *p_ind = NULL;

    dbtext * dbstr = NULL;

    char* mbcs_queue_name = NULL;

    CHECK_PTR(OCI_IPC_DEQUEUE, dequeue);

    /* reset message */

    CHECK(OcilibMessageReset(dequeue->msg));

    /* dequeue message */

    dbstr = (dbtext *) (dequeue->name);

    if (OCI_UNKNOWN != dequeue->typinf->typecode && NULL != dequeue->msg->obj)
    {
        p_ind = dequeue->msg->obj->tab_ind;
    }
    else
    {
        p_ind = &dequeue->msg->ind;
    }

    /* OCIAQDeq() parameter  'queue_name' is supposed to be either MBCS or UTF16 depending on the
    * environment. It appears that whatever mode is used, OCIAQDeq() only takes MBCS strings !
    * Oracle might fix this issue at some point.
    * Thus let's try to handle oracle future version fixing this issue.
    * Make a first attempt using the correct way.
    * If an error is reported let's try again using an MBCS string.
    */

#ifdef OCI_CHARSET_WIDE
    const OCI_Size attempt_max = 2;
#else
    const OCI_Size attempt_max = 1;
#endif

    OCI_Size attempt_count = 0;

    while (OCI_ERROR == ret && ++attempt_count <= attempt_max)
    {
        void* name = attempt_count == 1 ? (void*)dbstr : (void*) mbcs_queue_name;

        OCI_CALL(ret, OCIAQDeq, dequeue->typinf->con->handle, dequeue->typinf->con->err,
                 (OraText *)name, dequeue->handle, dequeue->msg->handle,
                 dequeue->typinf->handle, &dequeue->msg->payload,
                 (void **) &p_ind, &dequeue->msg->id, OCI_DEFAULT);

        /* check returned error code */

        if (OCI_ERROR == ret)
        {
            sb4 code = 0;

            OCI_ERROR_GET(dequeue->typinf->con->err, &code, NULL, 0);

#ifdef OCI_CHARSET_WIDE

            if (dbstr == name && (OCI_ERR_AQ_QUEUE_NAME_INVALID == code ||
                                  OCI_ERR_AQ_QUEUE_NOT_EXIST == code))
            {
                /* non valid queue name in UTF16 mode, we have an Oracle client with OCIAQDeq()
                 * accepting only MBCS strings.* Let's try again with an MBCS string for queue name
                 */

                const OCI_Size len = ostrlen(dequeue->name);
                mbcs_queue_name = OcilibMemoryAlloc(dequeue->typinf->con->env, OCI_IPC_STRING, sizeof(char), len + 1, FALSE);
                OcilibStringNativeToMbcs(dequeue->name, mbcs_queue_name, len);
            }
            else

#endif
            /* raise error only if the call has not been timed out */

            if (OCI_ERR_AQ_DEQUEUE_TIMEOUT != code)
            {
                THROW(OcilibExceptionOCI, dequeue->typinf->con->err, ret);
            }
        }
    }

    /* reset message */

    OCI_Message* msg = NULL;

    if (OCI_SUCCESSFUL(ret))
    {
        msg = dequeue->msg;

        /* get payload */

        if (p_ind && (OCI_IND_NULL != (*(OCIInd *) p_ind)))
        {
            dequeue->msg->ind = *(OCIInd *) p_ind;

            if (OCI_UNKNOWN != dequeue->typinf->typecode)
            {
                dequeue->msg->obj = OcilibObjectInitialize
                                    (
                                        dequeue->typinf->con,
                                        (OCI_Object*)dequeue->msg->obj,
                                        dequeue->msg->payload,
                                        dequeue->typinf,
                                        NULL, -1, TRUE
                                    );

                CHECK_NULL(dequeue->msg->obj);
            }
        }
    }

    SET_RETVAL(msg);

    CLEANUP_AND_EXIT_FUNC
    (

        FREE(mbcs_queue_name);
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDequeueGetConsumerName
 * --------------------------------------------------------------------------------------------- */

const otext * OcilibDequeueGetConsumer
(
    OCI_Dequeue *dequeue
)
{
    ENTER_FUNC
    (
        /* returns */ const otext*, NULL,
        /* context */ OCI_IPC_DEQUEUE, dequeue
    );

    CHECK_PTR(OCI_IPC_DEQUEUE, dequeue);

    if (NULL == dequeue->consumer)
    {
        OCI_UInt32 size = 0;

        CHECK(OcilibStringGetAttribute(dequeue->typinf->con, dequeue->handle,
                                       OCI_DTYPE_AQDEQ_OPTIONS,
                                       OCI_ATTR_CONSUMER_NAME,
                                       &dequeue->consumer, &size));
    }

    SET_RETVAL(dequeue->consumer);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDequeueSetConsumerName
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibDequeueSetConsumer
(
    OCI_Dequeue *dequeue,
    const otext *consumer
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_DEQUEUE, dequeue
    );

    CHECK_PTR(OCI_IPC_DEQUEUE, dequeue);

    CHECK(OcilibStringSetAttribute(dequeue->typinf->con, dequeue->handle,
                                   OCI_DTYPE_AQDEQ_OPTIONS,
                                   OCI_ATTR_CONSUMER_NAME,
                                   &dequeue->consumer, consumer));

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDequeueGetCorrelation
 * --------------------------------------------------------------------------------------------- */

const otext * OcilibDequeueGetCorrelation
(
    OCI_Dequeue *dequeue
)
{
    ENTER_FUNC
    (
        /* returns */ const otext*, NULL,
        /* context */ OCI_IPC_DEQUEUE, dequeue
    );

    CHECK_PTR(OCI_IPC_DEQUEUE, dequeue);

    if (NULL == dequeue->pattern)
    {
        OCI_UInt32 size = 0;

        CHECK(OcilibStringGetAttribute(dequeue->typinf->con, dequeue->handle,
                                       OCI_DTYPE_AQDEQ_OPTIONS,
                                       OCI_ATTR_CORRELATION,
                                       &dequeue->pattern, &size));
    }

    SET_RETVAL(dequeue->pattern);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDequeueSetCorrelation
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibDequeueSetCorrelation
(
    OCI_Dequeue *dequeue,
    const otext *pattern
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_DEQUEUE, dequeue
    );

    CHECK_PTR(OCI_IPC_DEQUEUE, dequeue);

    CHECK(OcilibStringSetAttribute(dequeue->typinf->con, dequeue->handle,
                                   OCI_DTYPE_AQDEQ_OPTIONS,
                                   OCI_ATTR_CORRELATION,
                                   &dequeue->pattern, pattern));

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDequeueGetRelativeMsgID
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibDequeueGetRelativeMsgID
(
    OCI_Dequeue *dequeue,
    void        *id,
    OCI_UInt32  *len
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_DEQUEUE, dequeue
    );

    OCIRaw *value = NULL;

    CHECK_PTR(OCI_IPC_DEQUEUE, dequeue);
    CHECK_PTR(OCI_IPC_VOID,    id);
    CHECK_PTR(OCI_IPC_VOID,    len);

    CHECK_ATTRIB_GET
    (
        OCI_DTYPE_AQDEQ_OPTIONS, OCI_ATTR_DEQ_MSGID,
        dequeue->handle, &value, NULL,
        dequeue->typinf->con->err
    );

    if (NULL != value)
    {
        ub4 raw_len;

        OCI_CALL(raw_len, OCIRawSize, dequeue->typinf->con->env->handle, value);

        if (*len > raw_len)
        {
            *len = raw_len;
        }

        oratext *rawptr;

        OCI_CALL(rawptr, OCIRawPtr, dequeue->typinf->con->env->handle, value);

        CHECK_NULL(rawptr);

        memcpy(id, rawptr, (OCI_Size) (*len));
    }
    else
    {
        *len = 0;
    }

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDequeueSetRelativeMsgID
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibDequeueSetRelativeMsgID
(
    OCI_Dequeue *dequeue,
    const void  *id,
    OCI_UInt32   len
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_DEQUEUE, dequeue
    );

    OCIRaw *value = NULL;

    CHECK_PTR(OCI_IPC_DEQUEUE, dequeue);

    CHECK_OCI
    (
        dequeue->typinf->con->err,
        OCIRawAssignBytes,
        dequeue->typinf->con->env->handle, dequeue->typinf->con->err,
        (ub1*) id, (ub4) len, (OCIRaw **) &value
    );

    CHECK_ATTRIB_SET
    (
        OCI_DTYPE_AQDEQ_OPTIONS, OCI_ATTR_DEQ_MSGID,
        dequeue->handle, &value, 0,
        dequeue->typinf->con->err
    );

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDequeueGetVisibility
 * --------------------------------------------------------------------------------------------- */

OCI_QueueVisibility OcilibDequeueGetVisibility
(
    OCI_Dequeue *dequeue
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_QueueVisibility, OCI_UNKNOWN,
        /* context */ OCI_IPC_DEQUEUE, dequeue
    );

    ub4 value = OCI_UNKNOWN;

    CHECK_PTR(OCI_IPC_DEQUEUE, dequeue);

    CHECK_ATTRIB_GET
    (
        OCI_DTYPE_AQDEQ_OPTIONS, OCI_ATTR_VISIBILITY,
        dequeue->handle, &value, NULL,
        dequeue->typinf->con->err
    );

    SET_RETVAL((OCI_UInt32)value);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDequeueSetVisibility
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibDequeueSetVisibility
(
    OCI_Dequeue        *dequeue,
    OCI_QueueVisibility visibility
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_DEQUEUE, dequeue
    );

    ub4 value = (ub4) visibility;

    CHECK_PTR(OCI_IPC_DEQUEUE, dequeue);
    CHECK_ENUM_VALUE(visibility, VisibilityModeValues, OTEXT("Visibility Mode"));

    CHECK_ATTRIB_SET
    (
        OCI_DTYPE_AQDEQ_OPTIONS, OCI_ATTR_VISIBILITY,
        dequeue->handle, &value, 0,
        dequeue->typinf->con->err
    );

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDequeueGetMode
 * --------------------------------------------------------------------------------------------- */

OCI_DequeueMode OcilibDequeueGetMode
(
    OCI_Dequeue *dequeue
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_DequeueMode, OCI_UNKNOWN,
        /* context */ OCI_IPC_DEQUEUE, dequeue
    );

    ub4 value = OCI_UNKNOWN;

    CHECK_PTR(OCI_IPC_DEQUEUE, dequeue);

    CHECK_ATTRIB_GET
    (
        OCI_DTYPE_AQDEQ_OPTIONS, OCI_ATTR_DEQ_MODE,
        dequeue->handle, &value, NULL,
        dequeue->typinf->con->err
    );

    SET_RETVAL((OCI_DequeueMode) value);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDequeueSetMode
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibDequeueSetMode
(
    OCI_Dequeue    *dequeue,
    OCI_DequeueMode mode
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_DEQUEUE, dequeue
    );

    ub4 value = (ub4) mode;

    CHECK_PTR(OCI_IPC_DEQUEUE, dequeue);
    CHECK_ENUM_VALUE(mode, OcilibDequeueModeValues, OTEXT("Dequeue Mode"));

    CHECK_ATTRIB_SET
    (
        OCI_DTYPE_AQDEQ_OPTIONS, OCI_ATTR_DEQ_MODE,
        dequeue->handle, &value, 0,
        dequeue->typinf->con->err
    );

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDequeueGetNavigation
 * --------------------------------------------------------------------------------------------- */

OCI_DequeueNav OcilibDequeueGetNavigation
(
    OCI_Dequeue *dequeue
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_DequeueNav, OCI_UNKNOWN,
        /* context */ OCI_IPC_DEQUEUE, dequeue
    );

    ub4 value = OCI_UNKNOWN;

    CHECK_PTR(OCI_IPC_DEQUEUE, dequeue);

    CHECK_ATTRIB_GET
    (
        OCI_DTYPE_AQDEQ_OPTIONS, OCI_ATTR_NAVIGATION,
        dequeue->handle, &value, NULL,
        dequeue->typinf->con->err
    );

    SET_RETVAL((OCI_DequeueNav) value);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDequeueSetNavigation
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibDequeueSetNavigation
(
    OCI_Dequeue   *dequeue,
    OCI_DequeueNav position
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_DEQUEUE, dequeue
    );

    ub4 value = (ub4) position;

    CHECK_PTR(OCI_IPC_DEQUEUE, dequeue);
    CHECK_ENUM_VALUE(position, NavigationModeValues, OTEXT("Navigation Mode"));

    CHECK_ATTRIB_SET
    (
        OCI_DTYPE_AQDEQ_OPTIONS, OCI_ATTR_NAVIGATION,
        dequeue->handle, &value, 0,
        dequeue->typinf->con->err
    );

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDequeueGetWaitTime
 * --------------------------------------------------------------------------------------------- */

OCI_Int32 OcilibDequeueGetWaitTime
(
    OCI_Dequeue *dequeue
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Int32, 0,
        /* context */ OCI_IPC_DEQUEUE, dequeue
    );

    sb4 value = 0;

    CHECK_PTR(OCI_IPC_DEQUEUE, dequeue);

    CHECK_ATTRIB_GET
    (
        OCI_DTYPE_AQDEQ_OPTIONS, OCI_ATTR_WAIT,
        dequeue->handle, &value, NULL,
        dequeue->typinf->con->err
    );

    SET_RETVAL((OCI_Int32) value);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDequeueSetWaitTime
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibDequeueSetWaitTime
(
    OCI_Dequeue *dequeue,
    OCI_Int32    timeout
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_DEQUEUE, dequeue
    );

    sb4 value = (sb4) timeout;

    CHECK_PTR(OCI_IPC_DEQUEUE, dequeue);

    CHECK_ATTRIB_SET
    (
        OCI_DTYPE_AQDEQ_OPTIONS, OCI_ATTR_WAIT,
        dequeue->handle, &value, 0,
        dequeue->typinf->con->err
    );

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDequeueSetAgentList
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibDequeueSetAgentList
(
    OCI_Dequeue *dequeue,
    OCI_Agent  **consumers,
    OCI_UInt32   count
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_DEQUEUE, dequeue
    );

    CHECK_PTR(OCI_IPC_ENQUEUE, dequeue);

    FREE(dequeue->agent_list);

    if (NULL != consumers && count > 0)
    {
        ALLOC_DATA(OCI_IPC_ARRAY, dequeue->agent_list, count);

        for (OCI_UInt32 i = 0; i < count; i++)
        {
            dequeue->agent_list[i] = consumers[i]->handle;
        }

        dequeue->agent_count = (ub4)count;
    }

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDequeueSubscribe
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibDequeueSubscribe
(
    OCI_Connection* con,
    OCI_Dequeue  ** dequeues,
    OCI_UInt32      count,
    OCI_UInt32      timeout,
    POCI_NOTIFY_AQ  callback
)
{
    OCISubscription** subs   = NULL;
    dbtext          * dbstr  = NULL;
    OCI_Int32         dbsize = -1;

    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_CONNECTION, con
    );

    CHECK_PTR(OCI_IPC_CONNECTION, con);
    CHECK_PTR(OCI_IPC_ARRAY, dequeues);
    CHECK_DATABASE_NOTIFY_ENABLED();
    CHECK_MIN(count, 1);

#if OCI_VERSION_COMPILE >= OCI_10_2

    ub4 oci_timeout  = (ub4) timeout;
    ub4 oci_protocol = OCI_SUBSCR_PROTO_OCI;
    ub4 oci_msgpres  = OCI_SUBSCR_PRES_DEFAULT;

#endif

    ub4 oci_namespace = OCI_SUBSCR_NAMESPACE_AQ;

    ALLOC_DATA(OCI_IPC_VOID, subs, count);

    for (OCI_UInt32 i = 0; i < count; i++)
    {
        OCI_Dequeue* dequeue = dequeues[i];

        /* clear any previous subscription */

        CHECK(OcilibDequeueUnsubscribe(dequeue));

        /* allocate subscription handle */

        CHECK(OcilibMemoryAllocHandle(con->env,
                                      con->env->handle,
                                      (dvoid**)(void*)&dequeue->subhp,
                                      OCI_HTYPE_SUBSCRIPTION));

        subs[i] = dequeue->subhp;

#if OCI_VERSION_COMPILE >= OCI_10_2

        /* set timeout */

        if (oci_timeout > 0)
        {
            CHECK_ATTRIB_SET
            (
                OCI_HTYPE_SUBSCRIPTION, OCI_ATTR_SUBSCR_TIMEOUT,
                dequeue->subhp, &oci_timeout, sizeof(oci_timeout),
                dequeue->typinf->con->err
            );
        }

        /* set protocol  */

        CHECK_ATTRIB_SET
        (
            OCI_HTYPE_SUBSCRIPTION, OCI_ATTR_SUBSCR_RECPTPROTO,
            dequeue->subhp, &oci_protocol, sizeof(oci_protocol),
            dequeue->typinf->con->err
        );

        /* set presentation  */

        CHECK_ATTRIB_SET
        (
            OCI_HTYPE_SUBSCRIPTION, OCI_ATTR_SUBSCR_RECPTPRES,
            dequeue->subhp, &oci_msgpres, sizeof(oci_msgpres),
            dequeue->typinf->con->err
        );

#else

        OCI_NOT_USED(timeout);

#endif

        /* set name  */

        if (NULL != dequeue->name)
        {
            /* for AQ subscription, the name should be "[schema.]queue[:consumer]" */

            otext buffer[OCI_SIZE_BUFFER] = OTEXT("");

            otext *str = NULL;

            OCI_Size max_chars = sizeof(buffer) / sizeof(otext) - 1;

            ostrncat(buffer, dequeue->name, max_chars);

            if (dequeue->consumer)
            {
                max_chars -= ostrlen(dequeue->name);
                ostrncat(buffer, OTEXT(":"),        max_chars);
                max_chars -= (OCI_Size)1;

                ostrncat(buffer, dequeue->consumer, max_chars);
            }

            /* queue name must be uppercase */

            for (str = buffer; *str != 0; str++)
            {
                *str = (otext) otoupper(*str);
            }

            dbstr  = (dbtext *) (buffer);
            dbsize = OcilibStringDBSize(buffer);

            CHECK_ATTRIB_SET
            (
                OCI_HTYPE_SUBSCRIPTION, OCI_ATTR_SUBSCR_NAME,
                dequeue->subhp, dbstr, dbsize,
                dequeue->typinf->con->err
            );

            dbstr = NULL;
        }

        /* set namespace  */

        CHECK_ATTRIB_SET
        (
            OCI_HTYPE_SUBSCRIPTION, OCI_ATTR_SUBSCR_NAMESPACE,
            dequeue->subhp, &oci_namespace, sizeof(oci_namespace),
            dequeue->typinf->con->err
        );

        /* set context pointer to dequeue structure */

        CHECK_ATTRIB_SET
        (
            OCI_HTYPE_SUBSCRIPTION, OCI_ATTR_SUBSCR_CTX,
            dequeue->subhp, dequeue, 0,
            dequeue->typinf->con->err
        );

        /* On MSVC, casting a function pointer to a data pointer generates a warning.
            As there is no other way to do regarding the OCI API, let's disable this
            warning just the time to set the callback attribute to the dequeue handle */

        WARNING_DISABLE_CAST_FUNC_TYPE
        WARNING_DISABLE_PEDANTIC

        /* internal callback handler */

        CHECK_ATTRIB_SET
        (
            OCI_HTYPE_SUBSCRIPTION,
            OCI_ATTR_SUBSCR_CALLBACK,
            dequeue->subhp,
            OcilibCallbackNotifyMessages,
            0,
            dequeue->typinf->con->err
        );

        WARNING_RESTORE_CAST_FUNC_TYPE
        WARNING_RESTORE_PEDANTIC

        /* set callback */

        dequeue->callback = callback;

        /* all attributes set, let's register the subscription ! */
    }

    CHECK_OCI
    (
        con->err,
        OCISubscriptionRegister,
        con->handle, subs, (ub2) count,
        con->err,(ub4) OCI_DEFAULT
    );

    SET_SUCCESS();

    CLEANUP_AND_EXIT_FUNC
    (
        FREE(subs);

        if (con != NULL)
        {
        }

        if (FAILURE && NULL != dequeues)
        {
            for (OCI_UInt32 i = 0; i < count; i++)
            {
                OcilibDequeueUnsubscribe(dequeues[i]);
            }
        }
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDequeueUnsubscribe
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibDequeueUnsubscribe
(
    OCI_Dequeue *dequeue
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_DEQUEUE, dequeue
    );

    CHECK_PTR(OCI_IPC_DEQUEUE, dequeue);
    CHECK_DATABASE_NOTIFY_ENABLED();

    dequeue->callback = NULL;

    if (NULL != dequeue->subhp)
    {
        /* unregister the subscription */

        CHECK_OCI
        (
            dequeue->typinf->con->err,
            OCISubscriptionUnRegister,
            dequeue->typinf->con->handle, dequeue->subhp,
            dequeue->typinf->con->err,(ub4) OCI_DEFAULT
        );

        /* free OCI handle */

        OcilibMemoryFreeHandle(dequeue->typinf->con->env, (dvoid *) dequeue->subhp, OCI_HTYPE_SUBSCRIPTION);

        dequeue->subhp = NULL;
    }

    SET_SUCCESS();

    EXIT_FUNC();
}
