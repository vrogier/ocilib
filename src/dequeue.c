/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2018 Vincent ROGIER <vince.rogier@ocilib.net>
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

#include "ocilib_internal.h"

/* ********************************************************************************************* *
 *                             PRIVATE VARIABLES
 * ********************************************************************************************* */

static const unsigned int NavigationModeValues[] = { OCI_ADN_FIRST_MSG, OCI_ADN_NEXT_MSG, OCI_ADN_NEXT_TRANSACTION };
static const unsigned int VisibilityModeValues[] = { OCI_AMV_IMMEDIATE, OCI_AMV_ON_COMMIT };
static const unsigned int DequeueModeValues[]    = { OCI_ADM_BROWSE, OCI_ADM_LOCKED, OCI_ADM_REMOVE, OCI_ADM_REMOVE_NODATA };

/* ********************************************************************************************* *
 *                            PUBLIC FUNCTIONS
 * ********************************************************************************************* */

/* --------------------------------------------------------------------------------------------- *
 * OCI_DequeueCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Dequeue * OCI_API OCI_DequeueCreate
(
    OCI_TypeInfo *typinf,
    const otext  *name
)
{
    OCI_Dequeue *dequeue = NULL;

    OCI_CALL_ENTER(OCI_Dequeue*, dequeue)
    OCI_CALL_CHECK_PTR(OCI_IPC_TYPE_INFO, typinf)
    OCI_CALL_CHECK_PTR(OCI_IPC_STRING, name)
    OCI_CALL_CONTEXT_SET_FROM_CONN(typinf->con)

    /* allocate dequeue structure */

    OCI_ALLOCATE_DATA(OCI_IPC_DEQUEUE, dequeue, 1)

    if (OCI_STATUS)
    {
        dequeue->typinf = typinf;
        dequeue->name   = ostrdup(name);

        /* allocate dequeue options descriptor */

        OCI_STATUS = OCI_DescriptorAlloc((dvoid *)dequeue->typinf->con->env, (dvoid **)&dequeue->opth, OCI_DTYPE_AQDEQ_OPTIONS);

        /* create local message for OCI_DequeueGet() */

        if (OCI_STATUS)
        {
            dequeue->msg = OCI_MsgCreate(dequeue->typinf);
            OCI_STATUS = (NULL != dequeue->msg);
        }
    }

    /* check for failure */

    if (OCI_STATUS)
    {
        OCI_RETVAL = dequeue;
    }
    else if (dequeue)
    {
        OCI_DequeueFree(dequeue);
    }

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DequeueFree
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_DequeueFree
(
    OCI_Dequeue *dequeue
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_DEQUEUE, dequeue)
    OCI_CALL_CONTEXT_SET_FROM_CONN(dequeue->typinf->con)

    /* Unsubscribe notification if needed */

    if (dequeue->subhp)
    {
        OCI_DequeueUnsubscribe(dequeue);
    }

    /* free local message  */

    if (dequeue->msg)
    {
        OCI_MsgFree(dequeue->msg);
    }

    /* free local agent  */

    if (dequeue->agent)
    {
        OCI_AgentFree(dequeue->agent);
    }

    /* free OCI descriptor */

    OCI_DescriptorFree((dvoid *) dequeue->opth, OCI_DTYPE_AQDEQ_OPTIONS);

    /* free data  */

    OCI_FREE(dequeue->name)
    OCI_FREE(dequeue->pattern)
    OCI_FREE(dequeue->consumer)
    OCI_FREE(dequeue->agent_list)
    OCI_FREE(dequeue)

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DequeueListen
 * --------------------------------------------------------------------------------------------- */

OCI_Agent * OCI_API OCI_DequeueListen
(
    OCI_Dequeue *dequeue,
    int          timeout
)
{
    OCIAQAgent *handle = NULL;

    OCI_CALL_ENTER(OCI_Agent*, NULL)
    OCI_CALL_CHECK_PTR(OCI_IPC_DEQUEUE, dequeue)
    OCI_CALL_CONTEXT_SET_FROM_CONN(dequeue->typinf->con)

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
                OCI_RAISE_EXCEPTION(OCI_ExceptionOCI(dequeue->typinf->con->err, dequeue->typinf->con, NULL, FALSE))
            }
        }

        /* initialize local agent object */

        if (OCI_STATUS && handle && OCI_SUCCESSFUL(ret))
        {
            OCI_RETVAL = dequeue->agent = OCI_AgentInit(dequeue->typinf->con, dequeue->agent, handle, NULL, NULL);
        }
    }

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DequeueGet
 * --------------------------------------------------------------------------------------------- */

OCI_Msg * OCI_API OCI_DequeueGet
(
    OCI_Dequeue *dequeue
)
{
    sword  ret   = OCI_SUCCESS;
    void  *p_ind = NULL;

    OCI_CALL_ENTER(OCI_Msg*, NULL)
    OCI_CALL_CHECK_PTR(OCI_IPC_DEQUEUE, dequeue)
    OCI_CALL_CONTEXT_SET_FROM_CONN(dequeue->typinf->con)

    /* reset message */

    OCI_STATUS = OCI_MsgReset(dequeue->msg);

    if (OCI_STATUS)
    {
        int     dbsize = -1;
        dbtext *dbstr  = OCI_StringGetOracleString(dequeue->name, &dbsize);

        if (OCI_UNKNOWN == dequeue->typinf->typecode)
        {
            p_ind = &dequeue->msg->ind;
        }

        /* dequeue message */

        ret = OCIAQDeq(dequeue->typinf->con->cxt, dequeue->typinf->con->err,
                       (OraText *) dbstr, dequeue->opth, dequeue->msg->proph,
                       dequeue->typinf->tdo, &dequeue->msg->payload,
                       (void **) &p_ind, &dequeue->msg->id, OCI_DEFAULT);

        OCI_StringReleaseOracleString(dbstr);

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
                OCI_RAISE_EXCEPTION(OCI_ExceptionOCI(dequeue->typinf->con->err, dequeue->typinf->con, NULL, FALSE))
            }
        }
    }

    /* reset message */

    if (OCI_STATUS && OCI_SUCCESSFUL(ret))
    {
        /* get payload */

        if (OCI_UNKNOWN != dequeue->typinf->typecode)
        {
            if (p_ind && (OCI_IND_NULL != (*(OCIInd *) p_ind)))
            {
                dequeue->msg->ind = *(OCIInd *) p_ind;

                dequeue->msg->obj = OCI_ObjectInit(dequeue->typinf->con,
                                                   (OCI_Object *) dequeue->msg->obj,
                                                   dequeue->msg->payload, dequeue->typinf,
                                                   NULL, -1, TRUE);

                OCI_STATUS = (NULL != dequeue->msg->obj);
            }
        }
    }

    /* on success return internal message handle */

    if (OCI_STATUS && OCI_SUCCESSFUL(ret))
    {
        OCI_RETVAL = dequeue->msg;
    }

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DequeueGetConsumerName
 * --------------------------------------------------------------------------------------------- */

const otext * OCI_API OCI_DequeueGetConsumer
(
    OCI_Dequeue *dequeue
)
{
    OCI_CALL_ENTER(const otext*, NULL)
    OCI_CALL_CHECK_PTR(OCI_IPC_DEQUEUE, dequeue)
    OCI_CALL_CONTEXT_SET_FROM_CONN(dequeue->typinf->con)

    if (!dequeue->consumer)
    {
        unsigned int size = 0;
        
        OCI_STATUS = OCI_GetStringAttribute(dequeue->typinf->con, dequeue->opth, OCI_DTYPE_AQDEQ_OPTIONS,
                                            OCI_ATTR_CONSUMER_NAME, &dequeue->consumer, &size);
    }
 
    OCI_RETVAL = dequeue->consumer;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DequeueSetConsumerName
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_DequeueSetConsumer
(
    OCI_Dequeue *dequeue,
    const otext *consumer
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_DEQUEUE, dequeue)
    OCI_CALL_CONTEXT_SET_FROM_CONN(dequeue->typinf->con)

    OCI_RETVAL = OCI_STATUS = OCI_SetStringAttribute(dequeue->typinf->con, dequeue->opth, OCI_DTYPE_AQDEQ_OPTIONS,
                                                     OCI_ATTR_CONSUMER_NAME, &dequeue->consumer, consumer);
    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DequeueGetCorrelation
 * --------------------------------------------------------------------------------------------- */

const otext * OCI_API OCI_DequeueGetCorrelation
(
    OCI_Dequeue *dequeue
)
{
    OCI_CALL_ENTER(const otext*, NULL)

    OCI_CALL_CHECK_PTR(OCI_IPC_DEQUEUE, dequeue)
    OCI_CALL_CONTEXT_SET_FROM_CONN(dequeue->typinf->con)

    if (!dequeue->pattern)
    {
        unsigned int size = 0;
        
        OCI_STATUS = OCI_GetStringAttribute(dequeue->typinf->con, dequeue->opth, OCI_DTYPE_AQDEQ_OPTIONS,
                                            OCI_ATTR_CORRELATION, &dequeue->pattern, &size);
    }
 
    OCI_RETVAL = dequeue->pattern;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DequeueSetCorrelation
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_DequeueSetCorrelation
(
    OCI_Dequeue *dequeue,
    const otext *pattern
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_DEQUEUE, dequeue)
    OCI_CALL_CONTEXT_SET_FROM_CONN(dequeue->typinf->con)

    OCI_RETVAL = OCI_STATUS = OCI_SetStringAttribute(dequeue->typinf->con, dequeue->opth, OCI_DTYPE_AQDEQ_OPTIONS,
                                                     OCI_ATTR_CORRELATION, &dequeue->pattern, pattern);

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DequeueGetRelativeMsgID
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_DequeueGetRelativeMsgID
(
    OCI_Dequeue  *dequeue,
    void         *id,
    unsigned int *len
)
{
    OCIRaw *value = NULL;

    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_DEQUEUE, dequeue)
    OCI_CALL_CHECK_PTR(OCI_IPC_VOID, id)
    OCI_CALL_CHECK_PTR(OCI_IPC_VOID, len)
    OCI_CALL_CONTEXT_SET_FROM_CONN(dequeue->typinf->con)

    *len = 0;

    OCI_GET_ATTRIB(OCI_DTYPE_AQDEQ_OPTIONS, OCI_ATTR_DEQ_MSGID, dequeue->opth, &value, NULL)

    if (OCI_STATUS && value)
    {
        const ub4 raw_len = OCIRawSize(dequeue->typinf->con->env, value);

        if (*len > raw_len)
        {
            *len = raw_len;
        }

        memcpy(id, OCIRawPtr(dequeue->typinf->con->env, value), (size_t) (*len));
    }

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DequeueSetRelativeMsgID
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_DequeueSetRelativeMsgID
(
    OCI_Dequeue  *dequeue,
    const void   *id,
    unsigned int  len
)
{
    OCIRaw *value = NULL;

    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_DEQUEUE, dequeue)
    OCI_CALL_CONTEXT_SET_FROM_CONN(dequeue->typinf->con)

    OCI_EXEC(OCIRawAssignBytes(dequeue->typinf->con->env, dequeue->typinf->con->err, (ub1*) id, (ub4) len, (OCIRaw **) &value))

    OCI_SET_ATTRIB(OCI_DTYPE_AQDEQ_OPTIONS, OCI_ATTR_DEQ_MSGID, dequeue->opth, &value, 0)

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DequeueGetVisibility
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_DequeueGetVisibility
(
    OCI_Dequeue *dequeue
)
{
    ub4 value = OCI_UNKNOWN;

    OCI_CALL_ENTER(unsigned int, OCI_UNKNOWN)
    OCI_CALL_CHECK_PTR(OCI_IPC_DEQUEUE, dequeue)
    OCI_CALL_CONTEXT_SET_FROM_CONN(dequeue->typinf->con)

    OCI_GET_ATTRIB(OCI_DTYPE_AQDEQ_OPTIONS, OCI_ATTR_VISIBILITY, dequeue->opth, &value, NULL)

    OCI_RETVAL = (unsigned int)value;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DequeueSetVisibility
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_DequeueSetVisibility
(
    OCI_Dequeue *dequeue,
    unsigned int visibility
)
{
    ub4 value = (ub4) visibility;

    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_DEQUEUE, dequeue)
    OCI_CALL_CHECK_ENUM_VALUE(NULL, NULL, visibility, VisibilityModeValues, OTEXT("Visibility Mode"))
    OCI_CALL_CONTEXT_SET_FROM_CONN(dequeue->typinf->con)

    OCI_SET_ATTRIB(OCI_DTYPE_AQDEQ_OPTIONS, OCI_ATTR_VISIBILITY, dequeue->opth, &value, 0)

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DequeueGetMode
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_DequeueGetMode
(
    OCI_Dequeue *dequeue
)
{
    ub4 value = OCI_UNKNOWN;

    OCI_CALL_ENTER(unsigned int, OCI_UNKNOWN)
    OCI_CALL_CHECK_PTR(OCI_IPC_DEQUEUE, dequeue)
    OCI_CALL_CONTEXT_SET_FROM_CONN(dequeue->typinf->con)

    OCI_GET_ATTRIB(OCI_DTYPE_AQDEQ_OPTIONS, OCI_ATTR_DEQ_MODE, dequeue->opth, &value, NULL)

    OCI_RETVAL = value;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DequeueSetMode
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_DequeueSetMode
(
    OCI_Dequeue *dequeue,
    unsigned int mode
)
{
    ub4 value = (ub4) mode;

    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_DEQUEUE, dequeue)
    OCI_CALL_CHECK_ENUM_VALUE(NULL, NULL, mode, DequeueModeValues, OTEXT("Dequeue Mode"))
    OCI_CALL_CONTEXT_SET_FROM_CONN(dequeue->typinf->con)

    OCI_SET_ATTRIB(OCI_DTYPE_AQDEQ_OPTIONS, OCI_ATTR_DEQ_MODE, dequeue->opth, &value, 0)

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DequeueGetNavigation
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_DequeueGetNavigation
(
    OCI_Dequeue *dequeue
)
{
    ub4 value = OCI_UNKNOWN;

    OCI_CALL_ENTER(unsigned int, value)
    OCI_CALL_CHECK_PTR(OCI_IPC_DEQUEUE, dequeue)
    OCI_CALL_CONTEXT_SET_FROM_CONN(dequeue->typinf->con)

    OCI_GET_ATTRIB(OCI_DTYPE_AQDEQ_OPTIONS, OCI_ATTR_NAVIGATION, dequeue->opth, &value, NULL)

    OCI_RETVAL = value;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DequeueSetNavigation
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_DequeueSetNavigation
(
    OCI_Dequeue *dequeue,
    unsigned int position
)
{
    ub4 value = (ub4) position;

    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_DEQUEUE, dequeue)
    OCI_CALL_CHECK_ENUM_VALUE(NULL, NULL, position, NavigationModeValues, OTEXT("Navigation Mode"))
    OCI_CALL_CONTEXT_SET_FROM_CONN(dequeue->typinf->con)

    OCI_SET_ATTRIB(OCI_DTYPE_AQDEQ_OPTIONS, OCI_ATTR_NAVIGATION, dequeue->opth, &value, 0)

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DequeueGetWaitTime
 * --------------------------------------------------------------------------------------------- */

int OCI_API OCI_DequeueGetWaitTime
(
    OCI_Dequeue *dequeue
)
{
    sb4 value = 0;

    OCI_CALL_ENTER(unsigned int, value)
    OCI_CALL_CHECK_PTR(OCI_IPC_DEQUEUE, dequeue)
    OCI_CALL_CONTEXT_SET_FROM_CONN(dequeue->typinf->con)

    OCI_GET_ATTRIB(OCI_DTYPE_AQDEQ_OPTIONS, OCI_ATTR_WAIT, dequeue->opth, &value, NULL)

    OCI_RETVAL = value;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DequeueSetWaitTime
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_DequeueSetWaitTime
(
    OCI_Dequeue *dequeue,
    int          timeout
)
{
    sb4 value = (ub4) timeout;

    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_DEQUEUE, dequeue)
    OCI_CALL_CONTEXT_SET_FROM_CONN(dequeue->typinf->con)

    OCI_SET_ATTRIB(OCI_DTYPE_AQDEQ_OPTIONS, OCI_ATTR_WAIT, dequeue->opth, &value, 0)

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DequeueSetAgentList
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_DequeueSetAgentList
(
    OCI_Dequeue *dequeue,
    OCI_Agent  **consumers,
    unsigned int count
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_ENQUEUE, dequeue)
    OCI_CALL_CONTEXT_SET_FROM_CONN(dequeue->typinf->con)

    OCI_FREE(dequeue->agent_list)

    if (consumers && (count > 0))
    {
        OCI_ALLOCATE_DATA(OCI_IPC_ARRAY, dequeue->agent_list, count)

        if (OCI_STATUS)
        {
            for (unsigned int i = 0; i < count; i++)
            {
                dequeue->agent_list[i] = consumers[i]->handle;
            }

            dequeue->agent_count = (ub4)count;
        }
    }

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DequeueSubscribe
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API  OCI_DequeueSubscribe
(
    OCI_Dequeue    *dequeue,
    unsigned int    port,
    unsigned int    timeout,
    POCI_NOTIFY_AQ  callback
)
{
    ub4     oci_namespace   = OCI_SUBSCR_NAMESPACE_AQ;

#if OCI_VERSION_COMPILE >= OCI_10_2

    ub4     oci_port        = (ub4) port;
    ub4     oci_timeout     = (ub4) timeout;
    ub4     oci_protocol    = OCI_SUBSCR_PROTO_OCI;
    ub4     oci_msgpres     = OCI_SUBSCR_PRES_DEFAULT;

#endif

    OCI_Connection *con = NULL;

    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_DATABASE_NOTIFY_ENABLED()
    OCI_CALL_CHECK_PTR(OCI_IPC_DEQUEUE, dequeue)
    OCI_CALL_CONTEXT_SET_FROM_CONN(dequeue->typinf->con)

    con = dequeue->typinf->con;

    /* clear any previous subscription */

    OCI_DequeueUnsubscribe(dequeue);

    /* allocate subscription handle */

    OCI_STATUS = OCI_HandleAlloc(con->env, (dvoid **) (void *) &dequeue->subhp, OCI_HTYPE_SUBSCRIPTION);

#if OCI_VERSION_COMPILE >= OCI_10_2

    /* set port number */

    if (oci_port > 0)
    {
        OCI_GET_ATTRIB(OCI_HTYPE_SUBSCRIPTION, OCI_ATTR_SUBSCR_PORTNO, dequeue->subhp, &oci_port, sizeof(oci_port))
    }

    /* set timeout */

    if (oci_timeout > 0)
    {
        OCI_GET_ATTRIB(OCI_HTYPE_SUBSCRIPTION, OCI_ATTR_SUBSCR_TIMEOUT, dequeue->subhp, &oci_timeout, sizeof(oci_timeout))
    }

    /* set protocol  */

    OCI_SET_ATTRIB(OCI_HTYPE_SUBSCRIPTION, OCI_ATTR_SUBSCR_RECPTPROTO, dequeue->subhp, &oci_protocol, sizeof(oci_protocol))

    /* set presentation  */

    OCI_SET_ATTRIB(OCI_HTYPE_SUBSCRIPTION, OCI_ATTR_SUBSCR_RECPTPRES, dequeue->subhp, &oci_msgpres, sizeof(oci_msgpres))

#else

    OCI_NOT_USED(port)
    OCI_NOT_USED(timeout)

#endif

    /* set name  */

    if (dequeue->name)
    {
        /* for AQ subscription, the name should be "[schema.]queue[:consumer]" */

        otext buffer[OCI_SIZE_BUFFER] = OTEXT("");

        otext *str  = NULL;
        size_t max_chars = sizeof(buffer) / sizeof(otext) - 1;

        dbtext *dbstr    = NULL;
        int     dbsize   = -1;

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

        dbstr = OCI_StringGetOracleString(buffer, &dbsize);

        OCI_SET_ATTRIB(OCI_HTYPE_SUBSCRIPTION, OCI_ATTR_SUBSCR_NAME, dequeue->subhp, dbstr, dbsize)

        OCI_StringReleaseOracleString(dbstr);
    }

    /* set namespace  */

    OCI_SET_ATTRIB(OCI_HTYPE_SUBSCRIPTION, OCI_ATTR_SUBSCR_NAMESPACE, dequeue->subhp, &oci_namespace, oci_namespace)

    /* set context pointer to dequeue structure */

    OCI_SET_ATTRIB(OCI_HTYPE_SUBSCRIPTION, OCI_ATTR_SUBSCR_CTX, dequeue->subhp, dequeue, 0)

    /* On MSVC, casting a function pointer to a data pointer generates a warning.
        As there is no other to way to do regarding the OCI API, let's disable this
        warning just the time to set the callback attribute to the dequeue handle */

    #ifdef _MSC_VER
    #pragma warning(disable: 4054)
    #endif

    /* internal callback handler */

    OCI_SET_ATTRIB(OCI_HTYPE_SUBSCRIPTION, OCI_ATTR_SUBSCR_CALLBACK, dequeue->subhp, OCI_ProcNotifyMessages, 0)

    #ifdef _MSC_VER
    #pragma warning(default: 4054)
    #endif

    /* set callback */

    dequeue->callback = callback;

    /* all attributes set, let's register the subscription ! */

    OCI_EXEC(OCISubscriptionRegister(con->cxt, &dequeue->subhp, (ub2) 1, con->err,(ub4) OCI_DEFAULT))

    if (!OCI_STATUS)
    {
        /* clear subscription on failure */

        OCI_DequeueUnsubscribe(dequeue);
    }

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DequeueUnsubscribe
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_DequeueUnsubscribe
(
    OCI_Dequeue *dequeue
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_DATABASE_NOTIFY_ENABLED()
    OCI_CALL_CHECK_PTR(OCI_IPC_DEQUEUE, dequeue)
    OCI_CALL_CONTEXT_SET_FROM_CONN(dequeue->typinf->con)

    dequeue->callback = NULL;

    if (dequeue->subhp)
    {
        /* unregister the subscription */

        OCI_EXEC
        (
            OCISubscriptionUnRegister(dequeue->typinf->con->cxt, dequeue->subhp,
                                      dequeue->typinf->con->err,(ub4) OCI_DEFAULT)
        )

        /* free OCI handle */

        OCI_HandleFree((dvoid *) dequeue->subhp, OCI_HTYPE_SUBSCRIPTION);

        dequeue->subhp = NULL;
    }

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}
