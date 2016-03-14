/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2016 Vincent ROGIER <vince.rogier@ocilib.net>
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

static unsigned int NavigationModeValues[] = { OCI_ADN_FIRST_MSG, OCI_ADN_NEXT_MSG, OCI_ADN_NEXT_TRANSACTION };
static unsigned int VisibilityModeValues[] = { OCI_AMV_IMMEDIATE, OCI_AMV_ON_COMMIT };
static unsigned int DequeueModeValues[]    = { OCI_ADM_BROWSE, OCI_ADM_LOCKED, OCI_ADM_REMOVE, OCI_ADM_REMOVE_NODATA };

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

    OCI_LIB_CALL_ENTER(OCI_Dequeue*, dequeue)

    OCI_CHECK_PTR(OCI_IPC_TYPE_INFO, typinf)
    OCI_CHECK_PTR(OCI_IPC_STRING, name)

    /* allocate dequeue structure */

    dequeue = (OCI_Dequeue *)OCI_MemAlloc(OCI_IPC_DEQUEUE, sizeof(*dequeue), (size_t)1, TRUE);

    if (dequeue)
    {
        dequeue->typinf = typinf;
        dequeue->name   = ostrdup(name);

        /* allocate dequeue options descriptor */

        call_status = OCI_SUCCESSFUL(OCI_DescriptorAlloc((dvoid *)dequeue->typinf->con->env,
                                                         (dvoid **) &dequeue->opth,
                                                         OCI_DTYPE_AQDEQ_OPTIONS,
                                                         (size_t) 0, (dvoid **) NULL));

        /* create local message for OCI_DequeueGet() */

        if (call_status)
        {
            dequeue->msg = OCI_MsgCreate(dequeue->typinf);
        }

        call_status = (NULL !=  dequeue->msg);
    }

    /* check for failure */

    if (call_status)
    {
        call_retval = dequeue;
    }
    else if (dequeue)
    {
        OCI_DequeueFree(dequeue);
    }

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DequeueFree
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_DequeueFree
(
    OCI_Dequeue *dequeue
)
{
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_DEQUEUE, dequeue)

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

    /* free strings  */

    OCI_FREE(dequeue->name)
    OCI_FREE(dequeue->pattern)
    OCI_FREE(dequeue->consumer)

    /* free misc. */

    OCI_FREE(dequeue->agent_list)

    OCI_FREE(dequeue)

    call_retval = call_status = TRUE;

    OCI_LIB_CALL_EXIT()
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

    OCI_LIB_CALL_ENTER(OCI_Agent*, NULL)

    OCI_CHECK_PTR(OCI_IPC_DEQUEUE, dequeue)

    /* listen only if OCI_DequeueSetAgentList has been called */

    if (dequeue->agent_list)
    {
        sword ret  = OCI_SUCCESS;
        sb4   code = OCI_SUCCESS;

        call_status = TRUE;

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
                OCI_ExceptionOCI(dequeue->typinf->con->err, dequeue->typinf->con, NULL, FALSE);

                call_status = FALSE;
            }
        }

        /* initialize local agent object */

        if (call_status && handle && OCI_SUCCESSFUL(ret))
        {
            call_retval = OCI_AgentInit(dequeue->typinf->con, &dequeue->agent, handle, NULL, NULL);
        }
    }

    OCI_LIB_CALL_EXIT()
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

    OCI_LIB_CALL_ENTER(OCI_Msg*, NULL)

    OCI_CHECK_PTR(OCI_IPC_DEQUEUE, dequeue)

    /* reset message */

    call_status = OCI_MsgReset(dequeue->msg);

    if (call_status)
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
                OCI_ExceptionOCI(dequeue->typinf->con->err, dequeue->typinf->con, NULL, FALSE);

                call_status = FALSE;
            }
        }
    }

    /* reset message */

    if (call_status && OCI_SUCCESSFUL(ret))
    {
        /* get payload */

        if (OCI_UNKNOWN != dequeue->typinf->typecode)
        {
            if (p_ind && (OCI_IND_NULL != (*(OCIInd *) p_ind)))
            {
                dequeue->msg->ind = *(OCIInd *) p_ind;

                dequeue->msg->obj = OCI_ObjectInit(dequeue->typinf->con,
                                                   (OCI_Object **) &dequeue->msg->obj,
                                                   dequeue->msg->payload, dequeue->typinf,
                                                   NULL, -1, TRUE);

                call_status = (NULL != dequeue->msg->obj);
            }
        }
    }

    /* on success return internal message handle */

    if (call_status && OCI_SUCCESSFUL(ret))
    {
        call_retval = dequeue->msg;
    }

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DequeueGetConsumerName
 * --------------------------------------------------------------------------------------------- */

const otext * OCI_API OCI_DequeueGetConsumer
(
    OCI_Dequeue *dequeue
)
{
    OCI_LIB_CALL_ENTER(const otext*, NULL)

    OCI_CHECK_PTR(OCI_IPC_DEQUEUE, dequeue)

    call_status = TRUE;

    if (!dequeue->consumer)
    {
        call_status = OCI_GetStringAttribute(dequeue->typinf->con,
                                             dequeue->opth,
                                             OCI_DTYPE_AQDEQ_OPTIONS,
                                             OCI_ATTR_CONSUMER_NAME,
                                             &dequeue->consumer);
    }
 
    call_retval = dequeue->consumer;

    OCI_LIB_CALL_EXIT()
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
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_DEQUEUE, dequeue)

    call_retval = call_status = OCI_SetStringAttribute( dequeue->typinf->con,
                                                        dequeue->opth,
                                                        OCI_DTYPE_AQDEQ_OPTIONS,
                                                        OCI_ATTR_CONSUMER_NAME,
                                                        &dequeue->consumer,
                                                        consumer);
    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DequeueGetCorrelation
 * --------------------------------------------------------------------------------------------- */

const otext * OCI_API OCI_DequeueGetCorrelation
(
    OCI_Dequeue *dequeue
)
{
    OCI_LIB_CALL_ENTER(const otext*, NULL)

    OCI_CHECK_PTR(OCI_IPC_DEQUEUE, dequeue)

    call_status = TRUE;

    if (!dequeue->pattern)
    {
        call_status = OCI_GetStringAttribute(dequeue->typinf->con,
                                             dequeue->opth,
                                             OCI_DTYPE_AQDEQ_OPTIONS,
                                             OCI_ATTR_CORRELATION,
                                             &dequeue->pattern);
    }
 
    call_retval = dequeue->pattern;

    OCI_LIB_CALL_EXIT()
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
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_DEQUEUE, dequeue)

    call_retval = call_status = OCI_SetStringAttribute( dequeue->typinf->con,
                                                        dequeue->opth,
                                                        OCI_DTYPE_AQDEQ_OPTIONS,
                                                        OCI_ATTR_CORRELATION,
                                                        &dequeue->pattern,
                                                        pattern);

    OCI_LIB_CALL_EXIT()
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

    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_DEQUEUE, dequeue)
    OCI_CHECK_PTR(OCI_IPC_VOID, id)
    OCI_CHECK_PTR(OCI_IPC_VOID, len)

    call_status = TRUE;

    OCI_CALL2
    (
        call_status, dequeue->typinf->con,

        OCIAttrGet((dvoid *) dequeue->opth,
                   (ub4    ) OCI_DTYPE_AQDEQ_OPTIONS,
                   (dvoid *) &value,
                   (ub4   *) NULL,
                   (ub4    ) OCI_ATTR_DEQ_MSGID,
                   dequeue->typinf->con->err)
    )

    if (call_status && value)
    {
        ub4 raw_len = OCIRawSize(dequeue->typinf->con->env, value);

        if (*len > raw_len)
        {
            *len = raw_len;
        }

        memcpy(id, OCIRawPtr(dequeue->typinf->con->env, value), (size_t) (*len));
    }
    else
    {
        *len = 0;
    }

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
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

    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_DEQUEUE, dequeue)

    call_status = TRUE;

    OCI_CALL2
    (
        call_status, dequeue->typinf->con,

        OCIRawAssignBytes(dequeue->typinf->con->env, dequeue->typinf->con->err,
                          (ub1*) id, (ub4) len, (OCIRaw **) &value)
    )

    OCI_CALL2
    (
        call_status, dequeue->typinf->con,

        OCIAttrSet((dvoid *) dequeue->opth,
                   (ub4    ) OCI_DTYPE_AQDEQ_OPTIONS,
                   (dvoid *) &value,
                   (ub4    ) 0,
                   (ub4    ) OCI_ATTR_DEQ_MSGID,
                   dequeue->typinf->con->err)
    )

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
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

    OCI_LIB_CALL_ENTER(unsigned int, value)

    OCI_CHECK_PTR(OCI_IPC_DEQUEUE, dequeue)

    call_status = TRUE;

    OCI_CALL2
    (
        call_status, dequeue->typinf->con,

        OCIAttrGet((dvoid *) dequeue->opth,
                   (ub4    ) OCI_DTYPE_AQDEQ_OPTIONS,
                   (dvoid *) &value,
                   (ub4   *) NULL,
                   (ub4    ) OCI_ATTR_VISIBILITY,
                   dequeue->typinf->con->err)
    )

    call_retval = (unsigned int) value;

    OCI_LIB_CALL_EXIT()
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

    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_DEQUEUE, dequeue)
    OCI_CHECK_ENUM_VALUE(NULL, NULL, visibility, VisibilityModeValues, OTEXT("Visibility Mode"))

    call_status = TRUE;

    OCI_CALL2
    (
        call_status, dequeue->typinf->con,

        OCIAttrSet((dvoid *) dequeue->opth,
                   (ub4    ) OCI_DTYPE_AQDEQ_OPTIONS,
                   (dvoid *) &value,
                   (ub4    ) 0,
                   (ub4    ) OCI_ATTR_VISIBILITY,
                   dequeue->typinf->con->err)
    )

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
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

    OCI_LIB_CALL_ENTER(unsigned int, OCI_UNKNOWN)

    OCI_CHECK_PTR(OCI_IPC_DEQUEUE, dequeue)

    call_status = TRUE;

    OCI_CALL2
    (
        call_status, dequeue->typinf->con,

        OCIAttrGet((dvoid *) dequeue->opth,
                   (ub4    ) OCI_DTYPE_AQDEQ_OPTIONS,
                   (dvoid *) &value,
                   (ub4   *) NULL,
                   (ub4    ) OCI_ATTR_DEQ_MODE,
                   dequeue->typinf->con->err)
    )

    call_retval = value;

    OCI_LIB_CALL_EXIT()
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

    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_DEQUEUE, dequeue)
    OCI_CHECK_ENUM_VALUE(NULL, NULL, mode, DequeueModeValues, OTEXT("Dequeue Mode"))

    call_status = TRUE;

    OCI_CALL2
    (
        call_status, dequeue->typinf->con,

        OCIAttrSet((dvoid *) dequeue->opth,
                   (ub4    ) OCI_DTYPE_AQDEQ_OPTIONS,
                   (dvoid *) &value,
                   (ub4    ) 0,
                   (ub4    ) OCI_ATTR_DEQ_MODE,
                   dequeue->typinf->con->err)
    )

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
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

    OCI_LIB_CALL_ENTER(unsigned int, value)

    OCI_CHECK_PTR(OCI_IPC_DEQUEUE, dequeue)

    call_status = TRUE;

    OCI_CALL2
    (
        call_status, dequeue->typinf->con,

        OCIAttrGet((dvoid *) dequeue->opth,
                   (ub4    ) OCI_DTYPE_AQDEQ_OPTIONS,
                   (dvoid *) &value,
                   (ub4   *) NULL,
                   (ub4    ) OCI_ATTR_NAVIGATION,
                   dequeue->typinf->con->err)
    )

    call_retval = value;

    OCI_LIB_CALL_EXIT()
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

    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_DEQUEUE, dequeue)
    OCI_CHECK_ENUM_VALUE(NULL, NULL, position, NavigationModeValues, OTEXT("Navigation Mode"))

    call_status = TRUE;

    OCI_CALL2
    (
        call_status, dequeue->typinf->con,

        OCIAttrSet((dvoid *) dequeue->opth,
                   (ub4    ) OCI_DTYPE_AQDEQ_OPTIONS,
                   (dvoid *) &value,
                   (ub4    ) 0,
                   (ub4    ) OCI_ATTR_NAVIGATION,
                   dequeue->typinf->con->err)
    )

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
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

    OCI_LIB_CALL_ENTER(unsigned int, value)

    OCI_CHECK_PTR(OCI_IPC_DEQUEUE, dequeue)

    call_status = TRUE;

    OCI_CALL2
    (
        call_status, dequeue->typinf->con,

        OCIAttrGet((dvoid *) dequeue->opth,
                   (ub4    ) OCI_DTYPE_AQDEQ_OPTIONS,
                   (dvoid *) &value,
                   (ub4   *) NULL,
                   (ub4    )  OCI_ATTR_WAIT,
                   dequeue->typinf->con->err)
    )

    call_retval = value;
    
    OCI_LIB_CALL_EXIT()
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

    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_DEQUEUE, dequeue)

    call_status = TRUE;

    OCI_CALL2
    (
        call_status, dequeue->typinf->con,

        OCIAttrSet((dvoid *) dequeue->opth,
                   (ub4    ) OCI_DTYPE_AQDEQ_OPTIONS,
                   (dvoid *) &value,
                   (ub4    ) 0,
                   (ub4    )  OCI_ATTR_WAIT,
                   dequeue->typinf->con->err)
    )

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
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
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_ENQUEUE, dequeue)

    OCI_FREE(dequeue->agent_list)

    call_status = TRUE;

    if (consumers && (count > 0))
    {
        dequeue->agent_list = (OCIAQAgent **) OCI_MemAlloc(OCI_IPC_ARRAY,  sizeof(OCIAQAgent *),
                                                           count, FALSE);

        if (dequeue->agent_list)
        {
            unsigned int i;

            for(i = 0; i < count; i++)
            {
                dequeue->agent_list[i] = consumers[i]->handle;
            }

            dequeue->agent_count = (ub4) count;
        }
        else
        {
            call_status = FALSE;
        }
    }

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
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

    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_DATABASE_NOTIFY_ENABLED()
    OCI_CHECK_PTR(OCI_IPC_DEQUEUE, dequeue)

    con = dequeue->typinf->con;

    /* clear any previous subscription */

    OCI_DequeueUnsubscribe(dequeue);

    /* allocate subscription handle */

    call_status = OCI_SUCCESSFUL(OCI_HandleAlloc(con->env, (dvoid **) (void *) &dequeue->subhp,
                                                 OCI_HTYPE_SUBSCRIPTION, (size_t) 0, (dvoid **) NULL));

#if OCI_VERSION_COMPILE >= OCI_10_2

    /* set port number */

    if (oci_port > 0)
    {
        OCI_CALL3
        (
            call_status, con->err,

            OCIAttrSet((dvoid *) dequeue->subhp, (ub4) OCI_HTYPE_SUBSCRIPTION,
                        (dvoid *) &oci_port, (ub4) sizeof (oci_port),
                        (ub4) OCI_ATTR_SUBSCR_PORTNO, con->err)
        )
    }

    /* set timeout */

    if (oci_timeout > 0)
    {
        OCI_CALL3
        (
            call_status, con->err,

            OCIAttrSet((dvoid *) dequeue->subhp, (ub4) OCI_HTYPE_SUBSCRIPTION,
                        (dvoid *) &oci_timeout, (ub4) sizeof (oci_timeout),
                        (ub4) OCI_ATTR_SUBSCR_TIMEOUT, con->err)
        )
    }

    /* set protocol  */

    OCI_CALL3
    (
        call_status, con->err,

        OCIAttrSet((dvoid *) dequeue->subhp, (ub4) OCI_HTYPE_SUBSCRIPTION,
                    (dvoid *) &oci_protocol, (ub4) sizeof(oci_protocol),
                    (ub4) OCI_ATTR_SUBSCR_RECPTPROTO, con->err)
    )

    /* set presentation  */

    OCI_CALL3
    (
        call_status, con->err,

        OCIAttrSet((dvoid *) dequeue->subhp, (ub4) OCI_HTYPE_SUBSCRIPTION,
                    (dvoid *) &oci_msgpres, (ub4) sizeof(oci_msgpres),
                    (ub4) OCI_ATTR_SUBSCR_RECPTPRES, con->err)
    )

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

        OCI_CALL3
        (
            call_status, con->err,

            OCIAttrSet((dvoid *) dequeue->subhp, (ub4) OCI_HTYPE_SUBSCRIPTION,
                        (dvoid *) dbstr, (ub4) dbsize,
                        (ub4) OCI_ATTR_SUBSCR_NAME, con->err)
        )

        OCI_StringReleaseOracleString(dbstr);
    }

    /* set namespace  */

    OCI_CALL3
    (
        call_status, con->err,

        OCIAttrSet((dvoid *) dequeue->subhp, (ub4) OCI_HTYPE_SUBSCRIPTION,
                    (dvoid *) &oci_namespace, (ub4) sizeof(oci_namespace),
                    (ub4) OCI_ATTR_SUBSCR_NAMESPACE, con->err)
    )

    /* set context pointer to dequeue structure */

    OCI_CALL3
    (
        call_status, con->err,

        OCIAttrSet((dvoid *) dequeue->subhp, (ub4) OCI_HTYPE_SUBSCRIPTION,
                    (dvoid *) dequeue, (ub4) 0,
                    (ub4) OCI_ATTR_SUBSCR_CTX, con->err)
    )

    /* On MSVC, casting a function pointer to a data pointer generates a warning.
        As there is no other to way to do regarding the OCI API, let's disable this
        warning just the time to set the callback attribute to the dequeue handle */

    #ifdef _MSC_VER
    #pragma warning(disable: 4054)
    #endif

    /* internal callback handler */

    OCI_CALL3
    (
        call_status, con->err,

        OCIAttrSet((dvoid *) dequeue->subhp, (ub4) OCI_HTYPE_SUBSCRIPTION,
                    (dvoid *) OCI_ProcNotifyMessages, (ub4) 0,
                    (ub4) OCI_ATTR_SUBSCR_CALLBACK, con->err)
    )

    #ifdef _MSC_VER
    #pragma warning(default: 4054)
    #endif

    /* set callback */

    dequeue->callback = callback;

    /* all attributes set, let's register the subscription ! */

    OCI_CALL3
    (
        call_status, con->err,

        OCISubscriptionRegister(con->cxt, &dequeue->subhp, (ub2) 1, con->err,(ub4) OCI_DEFAULT)
    )

    if (!call_status)
    {
        /* clear subscription on failure */

        OCI_DequeueUnsubscribe(dequeue);
    }

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DequeueUnsubscribe
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_DequeueUnsubscribe
(
    OCI_Dequeue *dequeue
)
{
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_DATABASE_NOTIFY_ENABLED()
    OCI_CHECK_PTR(OCI_IPC_DEQUEUE, dequeue)

    dequeue->callback = NULL;

    call_status = TRUE;

    if (dequeue->subhp)
    {
        /* unregister the subscription */

        OCI_CALL3
        (
            call_status, dequeue->typinf->con->err,

            OCISubscriptionUnRegister(dequeue->typinf->con->cxt, dequeue->subhp,
                                      dequeue->typinf->con->err,(ub4) OCI_DEFAULT)
        )

        /* free OCI handle */

        OCI_HandleFree((dvoid *) dequeue->subhp, OCI_HTYPE_SUBSCRIPTION);

        dequeue->subhp = NULL;
    }

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
}
