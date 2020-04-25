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

static const unsigned int NavigationModeValues[] = { OCI_ADN_FIRST_MSG, OCI_ADN_NEXT_MSG, OCI_ADN_NEXT_TRANSACTION };
static const unsigned int VisibilityModeValues[] = { OCI_AMV_IMMEDIATE, OCI_AMV_ON_COMMIT };
static const unsigned int DequeueModeValues[]    = { OCI_ADM_BROWSE, OCI_ADM_LOCKED, OCI_ADM_REMOVE, OCI_ADM_REMOVE_NODATA };

/* --------------------------------------------------------------------------------------------- *
 * DequeueCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Dequeue * DequeueCreate
(
    OCI_TypeInfo *typinf,
    const otext  *name
)
{
    OCI_Dequeue *dequeue = NULL;

    CALL_ENTER(OCI_Dequeue*, dequeue)
    CALL_CHECK_PTR(OCI_IPC_TYPE_INFO, typinf)
    CALL_CHECK_PTR(OCI_IPC_STRING, name)
    CALL_CONTEXT_FROM_CON(typinf->con)

    /* allocate dequeue structure */

    ALLOC_DATA(OCI_IPC_DEQUEUE, dequeue, 1)

    if (STATUS)
    {
        dequeue->typinf = typinf;
        dequeue->name   = ostrdup(name);

        /* allocate dequeue options descriptor */

        STATUS = MemoryAllocDescriptor((dvoid *)dequeue->typinf->con->env, (dvoid **)&dequeue->opth, OCI_DTYPE_AQDEQ_OPTIONS);

        /* create local message for OCI_DequeueGet() */

        if (STATUS)
        {
            dequeue->msg = MessageCreate(dequeue->typinf);

            STATUS = (NULL != dequeue->msg);
        }
    }

    /* check for failure */

    if (STATUS)
    {
        RETVAL = dequeue;
    }
    else if (dequeue)
    {
        DequeueFree(dequeue);
    }

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * DequeueFree
 * --------------------------------------------------------------------------------------------- */

boolean DequeueFree
(
    OCI_Dequeue *dequeue
)
{
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_DEQUEUE, dequeue)
    CALL_CONTEXT_FROM_CON(dequeue->typinf->con)

    /* Unsubscribe notification if needed */

    if (dequeue->subhp)
    {
        DequeueUnsubscribe(dequeue);
    }

    /* free local message  */

    if (dequeue->msg)
    {
        MessageFree(dequeue->msg);
    }

    /* free local agent  */

    if (dequeue->agent)
    {
        AgentFree(dequeue->agent);
    }

    /* free OCI descriptor */

    MemoryFreeDescriptor((dvoid *) dequeue->opth, OCI_DTYPE_AQDEQ_OPTIONS);

    /* free data  */

    FREE(dequeue->name)
    FREE(dequeue->pattern)
    FREE(dequeue->consumer)
    FREE(dequeue->agent_list)
    FREE(dequeue)

    RETVAL = STATUS;

    CALL_EXIT()
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
    OCIAQAgent *handle = NULL;

    CALL_ENTER(OCI_Agent*, NULL)
    CALL_CHECK_PTR(OCI_IPC_DEQUEUE, dequeue)
    CALL_CONTEXT_FROM_CON(dequeue->typinf->con)

    /* listen only if OCI_DequeueSetAgentList has been called */

    if (dequeue->agent_list)
    {
        sword ret  = OCI_SUCCESS;
        sb4   code = OCI_SUCCESS;

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
                THROW(ExceptionOCI(dequeue->typinf->con->err, dequeue->typinf->con, NULL, FALSE))
            }
        }

        /* initialize local agent object */

        if (STATUS && handle && OCI_SUCCESSFUL(ret))
        {
            RETVAL = dequeue->agent = AgentInitialize(dequeue->typinf->con, dequeue->agent, handle, NULL, NULL);
        }
    }

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * DequeueGetMessage
 * --------------------------------------------------------------------------------------------- */

OCI_Msg * DequeueGetMessage
(
    OCI_Dequeue *dequeue
)
{
    sword ret   = OCI_SUCCESS;
    void *p_ind = NULL;

    CALL_ENTER(OCI_Msg*, NULL)
    CALL_CHECK_PTR(OCI_IPC_DEQUEUE, dequeue)
    CALL_CONTEXT_FROM_CON(dequeue->typinf->con)

    /* reset message */

    STATUS = MessageReset(dequeue->msg);

    if (STATUS)
    {
        int     dbsize = -1;
        dbtext *dbstr  = StringGetDBString(dequeue->name, &dbsize);

        if (OCI_UNKNOWN == dequeue->typinf->typecode)
        {
            p_ind = &dequeue->msg->ind;
        }

        /* dequeue message */

        ret = OCIAQDeq(dequeue->typinf->con->cxt, dequeue->typinf->con->err,
                       (OraText *) dbstr, dequeue->opth, dequeue->msg->proph,
                       dequeue->typinf->tdo, &dequeue->msg->payload,
                       (void **) &p_ind, &dequeue->msg->id, OCI_DEFAULT);

        StringReleaseDBString(dbstr);

        /* check returned error code */

        if (OCI_ERROR == ret)
        {
            sb4 code = 0;

            OCIErrorGet((dvoid *) dequeue->typinf->con->err, (ub4) 1,
                        (OraText *) NULL, &code, (OraText *) NULL, (ub4) 0,
                        (ub4) OCI_HTYPE_ERROR);

            /* raise error only if the call has not been timed out */

            if (OCI_ERR_AQ_DEQUEUE_TIMEOUT != code)
            {
                THROW(ExceptionOCI(dequeue->typinf->con->err, dequeue->typinf->con, NULL, FALSE))
            }
        }
    }

    /* reset message */

    if (STATUS && OCI_SUCCESSFUL(ret))
    {
        /* get payload */

        if (OCI_UNKNOWN != dequeue->typinf->typecode)
        {
            if (p_ind && (OCI_IND_NULL != (*(OCIInd *) p_ind)))
            {
                dequeue->msg->ind = *(OCIInd *) p_ind;

                dequeue->msg->obj = ObjectInitialize(dequeue->typinf->con,
                                                     (OCI_Object *) dequeue->msg->obj,
                                                     dequeue->msg->payload, dequeue->typinf,
                                                     NULL, -1, TRUE);

                STATUS = (NULL != dequeue->msg->obj);
            }
        }
    }

    /* on success return internal message handle */

    if (STATUS && OCI_SUCCESSFUL(ret))
    {
        RETVAL = dequeue->msg;
    }

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * DequeueGetConsumerName
 * --------------------------------------------------------------------------------------------- */

const otext * DequeueGetConsumer
(
    OCI_Dequeue *dequeue
)
{
    CALL_ENTER(const otext*, NULL)
    CALL_CHECK_PTR(OCI_IPC_DEQUEUE, dequeue)
    CALL_CONTEXT_FROM_CON(dequeue->typinf->con)

    if (!dequeue->consumer)
    {
        unsigned int size = 0;

        STATUS = StringGetAttribute(dequeue->typinf->con, dequeue->opth, OCI_DTYPE_AQDEQ_OPTIONS,
                                    OCI_ATTR_CONSUMER_NAME, &dequeue->consumer, &size);
    }

    RETVAL = dequeue->consumer;

    CALL_EXIT()
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
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_DEQUEUE, dequeue)
    CALL_CONTEXT_FROM_CON(dequeue->typinf->con)

    RETVAL = STATUS = StringSetAttribute(dequeue->typinf->con, dequeue->opth, OCI_DTYPE_AQDEQ_OPTIONS,
                                         OCI_ATTR_CONSUMER_NAME, &dequeue->consumer, consumer);
    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * DequeueGetCorrelation
 * --------------------------------------------------------------------------------------------- */

const otext * DequeueGetCorrelation
(
    OCI_Dequeue *dequeue
)
{
    CALL_ENTER(const otext*, NULL)

    CALL_CHECK_PTR(OCI_IPC_DEQUEUE, dequeue)
    CALL_CONTEXT_FROM_CON(dequeue->typinf->con)

    if (!dequeue->pattern)
    {
        unsigned int size = 0;

        STATUS = StringGetAttribute(dequeue->typinf->con, dequeue->opth, OCI_DTYPE_AQDEQ_OPTIONS,
                                    OCI_ATTR_CORRELATION, &dequeue->pattern, &size);
    }

    RETVAL = dequeue->pattern;

    CALL_EXIT()
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
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_DEQUEUE, dequeue)
    CALL_CONTEXT_FROM_CON(dequeue->typinf->con)

    RETVAL = STATUS = StringSetAttribute(dequeue->typinf->con, dequeue->opth, OCI_DTYPE_AQDEQ_OPTIONS,
                                         OCI_ATTR_CORRELATION, &dequeue->pattern, pattern);

    CALL_EXIT()
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
    OCIRaw *value = NULL;

    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_DEQUEUE, dequeue)
    CALL_CHECK_PTR(OCI_IPC_VOID, id)
    CALL_CHECK_PTR(OCI_IPC_VOID, len)
    CALL_CONTEXT_FROM_CON(dequeue->typinf->con)

    *len = 0;

    ATTRIB_GET(OCI_DTYPE_AQDEQ_OPTIONS, OCI_ATTR_DEQ_MSGID, dequeue->opth, &value, NULL)

    if (STATUS && value)
    {
        const ub4 raw_len = OCIRawSize(dequeue->typinf->con->env, value);

        if (*len > raw_len)
        {
            *len = raw_len;
        }

        memcpy(id, OCIRawPtr(dequeue->typinf->con->env, value), (size_t) (*len));
    }

    RETVAL = STATUS;

    CALL_EXIT()
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
    OCIRaw *value = NULL;

    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_DEQUEUE, dequeue)
    CALL_CONTEXT_FROM_CON(dequeue->typinf->con)

    EXEC(OCIRawAssignBytes(dequeue->typinf->con->env, dequeue->typinf->con->err, (ub1*) id, (ub4) len, (OCIRaw **) &value))

    ATTRIB_SET(OCI_DTYPE_AQDEQ_OPTIONS, OCI_ATTR_DEQ_MSGID, dequeue->opth, &value, 0)

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * DequeueGetVisibility
 * --------------------------------------------------------------------------------------------- */

unsigned int DequeueGetVisibility
(
    OCI_Dequeue *dequeue
)
{
    ub4 value = OCI_UNKNOWN;

    CALL_ENTER(unsigned int, OCI_UNKNOWN)
    CALL_CHECK_PTR(OCI_IPC_DEQUEUE, dequeue)
    CALL_CONTEXT_FROM_CON(dequeue->typinf->con)

    ATTRIB_GET(OCI_DTYPE_AQDEQ_OPTIONS, OCI_ATTR_VISIBILITY, dequeue->opth, &value, NULL)

    RETVAL = (unsigned int)value;

    CALL_EXIT()
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
    ub4 value = (ub4) visibility;

    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_DEQUEUE, dequeue)
    CALL_CHECK_ENUM_VALUE(NULL, NULL, visibility, VisibilityModeValues, OTEXT("Visibility Mode"))
    CALL_CONTEXT_FROM_CON(dequeue->typinf->con)

    ATTRIB_SET(OCI_DTYPE_AQDEQ_OPTIONS, OCI_ATTR_VISIBILITY, dequeue->opth, &value, 0)

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * DequeueGetMode
 * --------------------------------------------------------------------------------------------- */

unsigned int DequeueGetMode
(
    OCI_Dequeue *dequeue
)
{
    ub4 value = OCI_UNKNOWN;

    CALL_ENTER(unsigned int, OCI_UNKNOWN)
    CALL_CHECK_PTR(OCI_IPC_DEQUEUE, dequeue)
    CALL_CONTEXT_FROM_CON(dequeue->typinf->con)

    ATTRIB_GET(OCI_DTYPE_AQDEQ_OPTIONS, OCI_ATTR_DEQ_MODE, dequeue->opth, &value, NULL)

    RETVAL = value;

    CALL_EXIT()
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
    ub4 value = (ub4) mode;

    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_DEQUEUE, dequeue)
    CALL_CHECK_ENUM_VALUE(NULL, NULL, mode, DequeueModeValues, OTEXT("Dequeue Mode"))
    CALL_CONTEXT_FROM_CON(dequeue->typinf->con)

    ATTRIB_SET(OCI_DTYPE_AQDEQ_OPTIONS, OCI_ATTR_DEQ_MODE, dequeue->opth, &value, 0)

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * DequeueGetNavigation
 * --------------------------------------------------------------------------------------------- */

unsigned int DequeueGetNavigation
(
    OCI_Dequeue *dequeue
)
{
    ub4 value = OCI_UNKNOWN;

    CALL_ENTER(unsigned int, value)
    CALL_CHECK_PTR(OCI_IPC_DEQUEUE, dequeue)
    CALL_CONTEXT_FROM_CON(dequeue->typinf->con)

    ATTRIB_GET(OCI_DTYPE_AQDEQ_OPTIONS, OCI_ATTR_NAVIGATION, dequeue->opth, &value, NULL)

    RETVAL = value;

    CALL_EXIT()
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
    ub4 value = (ub4) position;

    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_DEQUEUE, dequeue)
    CALL_CHECK_ENUM_VALUE(NULL, NULL, position, NavigationModeValues, OTEXT("Navigation Mode"))
    CALL_CONTEXT_FROM_CON(dequeue->typinf->con)

    ATTRIB_SET(OCI_DTYPE_AQDEQ_OPTIONS, OCI_ATTR_NAVIGATION, dequeue->opth, &value, 0)

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * DequeueGetWaitTime
 * --------------------------------------------------------------------------------------------- */

int DequeueGetWaitTime
(
    OCI_Dequeue *dequeue
)
{
    sb4 value = 0;

    CALL_ENTER(unsigned int, value)
    CALL_CHECK_PTR(OCI_IPC_DEQUEUE, dequeue)
    CALL_CONTEXT_FROM_CON(dequeue->typinf->con)

    ATTRIB_GET(OCI_DTYPE_AQDEQ_OPTIONS, OCI_ATTR_WAIT, dequeue->opth, &value, NULL)

    RETVAL = value;

    CALL_EXIT()
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
    sb4 value = (ub4) timeout;

    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_DEQUEUE, dequeue)
    CALL_CONTEXT_FROM_CON(dequeue->typinf->con)

    ATTRIB_SET(OCI_DTYPE_AQDEQ_OPTIONS, OCI_ATTR_WAIT, dequeue->opth, &value, 0)

    RETVAL = STATUS;

    CALL_EXIT()
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
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_ENQUEUE, dequeue)
    CALL_CONTEXT_FROM_CON(dequeue->typinf->con)

    FREE(dequeue->agent_list)

    if (consumers && (count > 0))
    {
        ALLOC_DATA(OCI_IPC_ARRAY, dequeue->agent_list, count)

        if (STATUS)
        {
            for (unsigned int i = 0; i < count; i++)
            {
                dequeue->agent_list[i] = consumers[i]->handle;
            }

            dequeue->agent_count = (ub4)count;
        }
    }

    RETVAL = STATUS;

    CALL_EXIT()
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
    ub4 oci_namespace = OCI_SUBSCR_NAMESPACE_AQ;

#if OCI_VERSION_COMPILE >= OCI_10_2

    ub4 oci_port     = (ub4) port;
    ub4 oci_timeout  = (ub4) timeout;
    ub4 oci_protocol = OCI_SUBSCR_PROTO_OCI;
    ub4 oci_msgpres  = OCI_SUBSCR_PRES_DEFAULT;

#endif

    OCI_Connection *con = NULL;

    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_DATABASE_NOTIFY_ENABLED()
    CALL_CHECK_PTR(OCI_IPC_DEQUEUE, dequeue)
    CALL_CONTEXT_FROM_CON(dequeue->typinf->con)

    con = dequeue->typinf->con;

    /* clear any previous subscription */

    DequeueUnsubscribe(dequeue);

    /* allocate subscription handle */

    STATUS = MemoryAllocHandle(con->env, (dvoid **) (void *) &dequeue->subhp, OCI_HTYPE_SUBSCRIPTION);

#if OCI_VERSION_COMPILE >= OCI_10_2

    /* set port number */

    if (oci_port > 0)
    {
        ATTRIB_GET(OCI_HTYPE_SUBSCRIPTION, OCI_ATTR_SUBSCR_PORTNO, dequeue->subhp, &oci_port, sizeof(oci_port))
    }

    /* set timeout */

    if (oci_timeout > 0)
    {
        ATTRIB_GET(OCI_HTYPE_SUBSCRIPTION, OCI_ATTR_SUBSCR_TIMEOUT, dequeue->subhp, &oci_timeout, sizeof(oci_timeout))
    }

    /* set protocol  */

    ATTRIB_SET(OCI_HTYPE_SUBSCRIPTION, OCI_ATTR_SUBSCR_RECPTPROTO, dequeue->subhp, &oci_protocol, sizeof(oci_protocol))

    /* set presentation  */

    ATTRIB_SET(OCI_HTYPE_SUBSCRIPTION, OCI_ATTR_SUBSCR_RECPTPRES, dequeue->subhp, &oci_msgpres, sizeof(oci_msgpres))

#else

    OCI_NOT_USED(port)
    OCI_NOT_USED(timeout)

#endif

    /* set name  */

    if (dequeue->name)
    {
        /* for AQ subscription, the name should be "[schema.]queue[:consumer]" */

        otext buffer[OCI_SIZE_BUFFER] = OTEXT("");

        otext *str = NULL;

        size_t max_chars = sizeof(buffer) / sizeof(otext) - 1;

        dbtext *dbstr  = NULL;
        int     dbsize = -1;

        ostrncat(buffer, dequeue->name, max_chars);

        if (dequeue->consumer)
        {
            max_chars -= ostrlen(dequeue->name);
            ostrncat(buffer, OTEXT(":"), max_chars);
            max_chars -= (size_t)1;

            ostrncat(buffer, dequeue->consumer, max_chars);
        }

        /* queue name must be uppercase */

        for (str = buffer; *str != 0; str++)
        {
            *str = (otext) otoupper(*str);
        }

        dbstr = StringGetDBString(buffer, &dbsize);

        ATTRIB_SET(OCI_HTYPE_SUBSCRIPTION, OCI_ATTR_SUBSCR_NAME, dequeue->subhp, dbstr, dbsize)

        StringReleaseDBString(dbstr);
    }

    /* set namespace  */

    ATTRIB_SET(OCI_HTYPE_SUBSCRIPTION, OCI_ATTR_SUBSCR_NAMESPACE, dequeue->subhp, &oci_namespace, oci_namespace)

    /* set context pointer to dequeue structure */

    ATTRIB_SET(OCI_HTYPE_SUBSCRIPTION, OCI_ATTR_SUBSCR_CTX, dequeue->subhp, dequeue, 0)

    /* On MSVC, casting a function pointer to a data pointer generates a warning.
        As there is no other to way to do regarding the OCI API, let's disable this
        warning just the time to set the callback attribute to the dequeue handle */

#ifdef _MSC_VER
  #pragma warning(disable: 4054)
#endif

    /* internal callback handler */

    ATTRIB_SET(OCI_HTYPE_SUBSCRIPTION, OCI_ATTR_SUBSCR_CALLBACK, dequeue->subhp, CallbackNotifyMessages, 0)

#ifdef _MSC_VER
  #pragma warning(default: 4054)
#endif

    /* set callback */

    dequeue->callback = callback;

    /* all attributes set, let's register the subscription ! */

    EXEC(OCISubscriptionRegister(con->cxt, &dequeue->subhp, (ub2) 1, con->err,(ub4) OCI_DEFAULT))

    if (!STATUS)
    {
        /* clear subscription on failure */

        DequeueUnsubscribe(dequeue);
    }

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * DequeueUnsubscribe
 * --------------------------------------------------------------------------------------------- */

boolean DequeueUnsubscribe
(
    OCI_Dequeue *dequeue
)
{
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_DATABASE_NOTIFY_ENABLED()
    CALL_CHECK_PTR(OCI_IPC_DEQUEUE, dequeue)
    CALL_CONTEXT_FROM_CON(dequeue->typinf->con)

    dequeue->callback = NULL;

    if (dequeue->subhp)
    {
        /* unregister the subscription */

        EXEC
        (
            OCISubscriptionUnRegister(dequeue->typinf->con->cxt, dequeue->subhp,
                                      dequeue->typinf->con->err,(ub4) OCI_DEFAULT)
        )

        /* free OCI handle */

        MemoryFreeHandle((dvoid *) dequeue->subhp, OCI_HTYPE_SUBSCRIPTION);

        dequeue->subhp = NULL;
    }

    RETVAL = STATUS;

    CALL_EXIT()
}
