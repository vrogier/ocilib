/*
    +-----------------------------------------------------------------------------------------+
    |                                                                                         |
    |                               OCILIB - C Driver for Oracle                              |
    |                                                                                         |
    |                                (C Wrapper for Oracle OCI)                               |
    |                                                                                         |
    |                              Website : http://www.ocilib.net                            |
    |                                                                                         |
    |             Copyright (c) 2007-2010 Vincent ROGIER <vince.rogier@ocilib.net>            |
    |                                                                                         |
    +-----------------------------------------------------------------------------------------+
    |                                                                                         |
    |             This library is free software; you can redistribute it and/or               |
    |             modify it under the terms of the GNU Lesser General Public                  |
    |             License as published by the Free Software Foundation; either                |
    |             version 2 of the License, or (at your option) any later version.            |
    |                                                                                         |
    |             This library is distributed in the hope that it will be useful,             |
    |             but WITHOUT ANY WARRANTY; without even the implied warranty of              |
    |             MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU           |
    |             Lesser General Public License for more details.                             |
    |                                                                                         |
    |             You should have received a copy of the GNU Lesser General Public            |
    |             License along with this library; if not, write to the Free                  |
    |             Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.          |
    |                                                                                         |
    +-----------------------------------------------------------------------------------------+
*/

/* --------------------------------------------------------------------------------------------- *
 * $Id: dequeue.c, v 3.8.1 2010-12-13 00:00 Vincent Rogier $
 * --------------------------------------------------------------------------------------------- */

#include "ocilib_internal.h"

/* ********************************************************************************************* *
 *                            PUBLIC FUNCTIONS
 * ********************************************************************************************* */

/* --------------------------------------------------------------------------------------------- *
 * OCI_DequeueCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Dequeue * OCI_API OCI_DequeueCreate
(
    OCI_TypeInfo *typinf,
    const mtext  *name
)
{
    OCI_Dequeue *dequeue = NULL;
    boolean res          = TRUE;

    OCI_CHECK_INITIALIZED(NULL);

    OCI_CHECK_PTR(OCI_IPC_TYPE_INFO, typinf, NULL);
    OCI_CHECK_PTR(OCI_IPC_STRING, name, NULL);

    /* allocate dequeue structure */

    dequeue = (OCI_Dequeue *) OCI_MemAlloc(OCI_IPC_DEQUEUE, sizeof(*dequeue), (size_t) 1, TRUE);

    if (dequeue != NULL)
    {
        dequeue->typinf = typinf;
        dequeue->name   = mtsdup(name);

        /* allocate dequeue options descriptor */

        res = (OCI_SUCCESS == OCI_DescriptorAlloc((dvoid * ) OCILib.env,
                                                  (dvoid **) &dequeue->opth,
                                                  OCI_DTYPE_AQDEQ_OPTIONS,
                                                  (size_t) 0, (dvoid **) NULL));

        /* create local message for OCI_DequeueGet() */

        if (res == TRUE)
        {
            dequeue->msg = OCI_MsgCreate(dequeue->typinf);
        }

        res = (dequeue->msg != NULL);
    }
    else
        res = FALSE;

    /* check for failure */

    if (res == FALSE)
    {
        OCI_DequeueFree(dequeue);
        dequeue = NULL;
    }

    return dequeue;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DequeueFree
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_DequeueFree
(
    OCI_Dequeue *dequeue
)
{
    OCI_CHECK_PTR(OCI_IPC_DEQUEUE, dequeue, FALSE);

    /* free local message  */

    if (dequeue->msg != NULL)
    {
        OCI_MsgFree(dequeue->msg);
    }

    /* free local agent  */

    if (dequeue->agent != NULL)
    {
        OCI_AgentFree(dequeue->agent);
    }

    /* free OCI descriptor */

    OCI_DescriptorFree((dvoid *) dequeue->opth, OCI_DTYPE_AQDEQ_OPTIONS);

    /* free strings  */

    OCI_FREE(dequeue->name);
    OCI_FREE(dequeue->pattern);
    OCI_FREE(dequeue->consumer);

    /* free misc. */

    OCI_FREE(dequeue->agent_list);

    OCI_FREE(dequeue);

    return TRUE;
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
    boolean res        = TRUE;
    OCI_Agent *agent   = NULL;
    OCIAQAgent *handle = NULL;

    OCI_CHECK_PTR(OCI_IPC_DEQUEUE, dequeue, NULL);

    /* listen only if OCI_DequeueSetAgentList has been called */

    if (dequeue->agent_list != NULL)
    {
        sword ret;
        sb4 code;

        ret =  OCIAQListen(dequeue->typinf->con->cxt, dequeue->typinf->con->err,
                           dequeue->agent_list, (ub4) dequeue->agent_count,
                           (sb4) timeout, &handle, OCI_DEFAULT);

        /* check returned error code */

        if (ret == OCI_ERROR)
        {
            OCIErrorGet((dvoid *) dequeue->typinf->con->err, (ub4) 1,
                        (OraText *) NULL, &code, (OraText *) NULL, (ub4) 0,
                        (ub4) OCI_HTYPE_ERROR);

            /* raise error only if the call has not been timed out */

            if (code != OCI_ERR_AQ_LISTEN_TIMEOUT)
            {
                OCI_ExceptionOCI(dequeue->typinf->con->err, dequeue->typinf->con, NULL, FALSE);

                res = FALSE;
            }
        }

        /* init local agent object */

        if ((res == TRUE) && (ret == OCI_SUCCESS) && (handle != NULL))
        {
            agent = OCI_AgentInit(dequeue->typinf->con, &dequeue->agent, handle, NULL, NULL);
        }
    }

    OCI_RESULT(res);

    return agent;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DequeueGet
 * --------------------------------------------------------------------------------------------- */

OCI_Msg * OCI_API OCI_DequeueGet
(
    OCI_Dequeue *dequeue
)
{
    boolean   res      = TRUE;
    void     *ostr     = NULL;
    int       osize    = -1;
    OCI_Msg  *msg      = NULL;
    sword     ret      = OCI_SUCCESS;
    void     *p_ind    = NULL;
 
    OCI_CHECK_PTR(OCI_IPC_DEQUEUE, dequeue, NULL);

    /* reset message */

    res = OCI_MsgReset(dequeue->msg);

    if (res == TRUE)
    {
        ostr  = OCI_GetInputMetaString(dequeue->name, &osize);

        if (dequeue->typinf->tcode == OCI_UNKNOWN)
        {
            p_ind = &dequeue->msg->ind;
        }

        /* dequeue message */
        
        ret = OCIAQDeq(dequeue->typinf->con->cxt, dequeue->typinf->con->err,
                       ostr, dequeue->opth, dequeue->msg->proph, dequeue->typinf->tdo,
                       &dequeue->msg->payload, (void **) &p_ind, &dequeue->msg->id, OCI_DEFAULT);
        
        OCI_ReleaseMetaString(ostr);
        
        /* check returned error code */

        if (ret == OCI_ERROR)
        {
            sb4 code = 0;

            OCIErrorGet((dvoid *) dequeue->typinf->con->err, (ub4) 1,
                        (OraText *) NULL, &code, (OraText *) NULL, (ub4) 0,
                        (ub4) OCI_HTYPE_ERROR);

            /* raise error only if the call has not been timed out */

            if (code != OCI_ERR_AQ_DEQUEUE_TIMEOUT)
            {
                OCI_ExceptionOCI(dequeue->typinf->con->err, dequeue->typinf->con, NULL, FALSE);

                res = FALSE;
            }
        }
    }

    /* reset message */

    if ((res == TRUE) && (ret == OCI_SUCCESS))
    {
        /* get payload */

        if (dequeue->typinf->tcode != OCI_UNKNOWN)
        {
            if ((p_ind != NULL) && ((*(OCIInd *) p_ind) != OCI_IND_NULL))
            {
                dequeue->msg->ind = *(OCIInd *) p_ind;

                dequeue->msg->obj = OCI_ObjectInit(dequeue->typinf->con, 
                                                   (OCI_Object **) &dequeue->msg->obj,
                                                   dequeue->msg->payload, dequeue->typinf,
                                                   NULL, -1, TRUE); 

                res = dequeue->msg->obj != NULL;
            }
        }
    }

    /* on success return internla message handle */

    if ((res == TRUE) && (ret == OCI_SUCCESS))
    {
        msg = dequeue->msg;
    }

    OCI_RESULT(res);

    return msg;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DequeueGetConsumerName
 * --------------------------------------------------------------------------------------------- */

const mtext * OCI_API OCI_DequeueGetConsumer
(
    OCI_Dequeue *dequeue
)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_DEQUEUE, dequeue, NULL);

    if (dequeue->consumer == NULL)
    {
        res = OCI_StringGetFromAttrHandle(dequeue->typinf->con,
                                          dequeue->opth,
                                          OCI_DTYPE_AQDEQ_OPTIONS,
                                          OCI_ATTR_CONSUMER_NAME,
                                          &dequeue->consumer);
    }

    OCI_RESULT(res);

    return dequeue->consumer;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DequeueSetConsumerName
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_DequeueSetConsumer
(
    OCI_Dequeue *dequeue,
    const mtext *consumer
)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_DEQUEUE, dequeue, FALSE);

    res =  OCI_StringSetToAttrHandle(dequeue->typinf->con,
                                     dequeue->opth,
                                     OCI_DTYPE_AQDEQ_OPTIONS,
                                     OCI_ATTR_CONSUMER_NAME,
                                     &dequeue->consumer,
                                     consumer);

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DequeueGetCorrelation
 * --------------------------------------------------------------------------------------------- */

const mtext * OCI_API OCI_DequeueGetCorrelation
(
    OCI_Dequeue *dequeue
)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_DEQUEUE, dequeue, NULL);

    if (dequeue->pattern == NULL)
    {
        res = OCI_StringGetFromAttrHandle(dequeue->typinf->con,
                                          dequeue->opth,
                                          OCI_DTYPE_AQDEQ_OPTIONS,
                                          OCI_ATTR_CORRELATION,
                                          &dequeue->pattern);
    }

    OCI_RESULT(res);

    return dequeue->pattern;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DequeueSetCorrelation
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_DequeueSetCorrelation
(
    OCI_Dequeue *dequeue,
    const mtext *pattern
)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_DEQUEUE, dequeue, FALSE);

    res =  OCI_StringSetToAttrHandle(dequeue->typinf->con,
                                     dequeue->opth,
                                     OCI_DTYPE_AQDEQ_OPTIONS,
                                     OCI_ATTR_CORRELATION,
                                     &dequeue->pattern,
                                     pattern);

    OCI_RESULT(res);

    return res;
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
    boolean res   = TRUE;
    OCIRaw *value = NULL;

    OCI_CHECK_PTR(OCI_IPC_DEQUEUE, dequeue, FALSE);
    OCI_CHECK_PTR(OCI_IPC_VOID,    id,      FALSE);
    OCI_CHECK_PTR(OCI_IPC_VOID,    len,     FALSE);

    OCI_CALL2
    (
        res, dequeue->typinf->con,

        OCIAttrGet((dvoid *) dequeue->opth,
                   (ub4    ) OCI_DTYPE_AQDEQ_OPTIONS,
                   (dvoid *) &value,
                   (ub4   *) NULL,
                   (ub4    ) OCI_ATTR_DEQ_MSGID,
                   dequeue->typinf->con->err)
    )

    if (value != NULL)
    {
        ub4 raw_len = 0;

        raw_len = OCIRawSize(OCILib.env, value);

        if (*len > raw_len)
            *len = raw_len;

        memcpy(id, OCIRawPtr(OCILib.env, value), (size_t) (*len));
    }
    else
    {
        *len = 0;
    }

    OCI_RESULT(res);

    return res;
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
    boolean res   = TRUE;
    OCIRaw *value = NULL;

    OCI_CHECK_PTR(OCI_IPC_DEQUEUE, dequeue, FALSE);

    OCI_CALL2
    (
        res, dequeue->typinf->con,

        OCIRawAssignBytes(OCILib.env, dequeue->typinf->con->err,
                          (ub1*) id, (ub4) len, (OCIRaw **) &value)
    )

    OCI_CALL2
    (
        res, dequeue->typinf->con,

        OCIAttrSet((dvoid *) dequeue->opth,
                   (ub4    ) OCI_DTYPE_AQDEQ_OPTIONS,
                   (dvoid *) &value,
                   (ub4    ) 0,
                   (ub4    ) OCI_ATTR_DEQ_MSGID,
                   dequeue->typinf->con->err)
    )

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DequeueGetVisibility
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_DequeueGetVisibility
(
    OCI_Dequeue *dequeue
)
{
    boolean res = TRUE;
    ub4 ret     = 0;

    OCI_CHECK_PTR(OCI_IPC_DEQUEUE, dequeue, 0);

    OCI_CALL2
    (
        res, dequeue->typinf->con,

        OCIAttrGet((dvoid *) dequeue->opth,
                   (ub4    ) OCI_DTYPE_AQDEQ_OPTIONS,
                   (dvoid *) &ret,
                   (ub4   *) NULL,
                   (ub4    ) OCI_ATTR_VISIBILITY,
                   dequeue->typinf->con->err)
    )

    OCI_RESULT(res);

    return (int) ret;
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
    boolean res = TRUE;
    ub4 value   = (ub4) visibility;

    OCI_CHECK_PTR(OCI_IPC_DEQUEUE, dequeue, FALSE);

    OCI_CALL2
    (
        res, dequeue->typinf->con,

        OCIAttrSet((dvoid *) dequeue->opth,
                   (ub4    ) OCI_DTYPE_AQDEQ_OPTIONS,
                   (dvoid *) &value,
                   (ub4    ) 0,
                   (ub4    ) OCI_ATTR_VISIBILITY,
                   dequeue->typinf->con->err)
    )

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DequeueGetMode
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_DequeueGetMode
(
    OCI_Dequeue *dequeue
)
{
    boolean res = TRUE;
    ub4 ret     = 0;

    OCI_CHECK_PTR(OCI_IPC_DEQUEUE, dequeue, 0);

    OCI_CALL2
    (
        res, dequeue->typinf->con,

        OCIAttrGet((dvoid *) dequeue->opth,
                   (ub4    ) OCI_DTYPE_AQDEQ_OPTIONS,
                   (dvoid *) &ret,
                   (ub4   *) NULL,
                   (ub4    ) OCI_ATTR_DEQ_MODE,
                   dequeue->typinf->con->err)
    )

    OCI_RESULT(res);

    return (int) ret;
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
    boolean res = TRUE;
    ub4 value   = (ub4) mode;

    OCI_CHECK_PTR(OCI_IPC_DEQUEUE, dequeue, FALSE);

    OCI_CALL2
    (
        res, dequeue->typinf->con,

        OCIAttrSet((dvoid *) dequeue->opth,
                   (ub4    ) OCI_DTYPE_AQDEQ_OPTIONS,
                   (dvoid *) &value,
                   (ub4    ) 0,
                   (ub4    ) OCI_ATTR_DEQ_MODE,
                   dequeue->typinf->con->err)
    )

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DequeueGetNavigation
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_DequeueGetNavigation
(
    OCI_Dequeue *dequeue
)
{
    boolean res = TRUE;
    ub4 ret     = 0;

    OCI_CHECK_PTR(OCI_IPC_DEQUEUE, dequeue, 0);

    OCI_CALL2
    (
        res, dequeue->typinf->con,

        OCIAttrGet((dvoid *) dequeue->opth,
                   (ub4    ) OCI_DTYPE_AQDEQ_OPTIONS,
                   (dvoid *) &ret,
                   (ub4   *) NULL,
                   (ub4    ) OCI_ATTR_NAVIGATION,
                   dequeue->typinf->con->err)
    )

    OCI_RESULT(res);

    return (int) ret;
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
    boolean res = TRUE;
    ub4 value   = (ub4) position;

    OCI_CHECK_PTR(OCI_IPC_DEQUEUE, dequeue, FALSE);

    OCI_CALL2
    (
        res, dequeue->typinf->con,

        OCIAttrSet((dvoid *) dequeue->opth,
                   (ub4    ) OCI_DTYPE_AQDEQ_OPTIONS,
                   (dvoid *) &value,
                   (ub4    ) 0,
                   (ub4    ) OCI_ATTR_NAVIGATION,
                   dequeue->typinf->con->err)
    )

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DequeueGetWaitTime
 * --------------------------------------------------------------------------------------------- */

int OCI_API OCI_DequeueGetWaitTime
(
    OCI_Dequeue *dequeue
)
{
    boolean res = TRUE;
    sb4 ret     = 0;

    OCI_CHECK_PTR(OCI_IPC_DEQUEUE, dequeue, 0);

    OCI_CALL2
    (
        res, dequeue->typinf->con,

        OCIAttrGet((dvoid *) dequeue->opth,
                   (ub4    ) OCI_DTYPE_AQDEQ_OPTIONS,
                   (dvoid *) &ret,
                   (ub4   *) NULL,
                   (ub4    )  OCI_ATTR_WAIT,
                   dequeue->typinf->con->err)
    )

    OCI_RESULT(res);

    return (int) ret;
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
    boolean res = TRUE;
    sb4 value   = (ub4) timeout;

    OCI_CHECK_PTR(OCI_IPC_DEQUEUE, dequeue, FALSE);

    OCI_CALL2
    (
        res, dequeue->typinf->con,

        OCIAttrSet((dvoid *) dequeue->opth,
                   (ub4    ) OCI_DTYPE_AQDEQ_OPTIONS,
                   (dvoid *) &value,
                   (ub4    ) 0,
                   (ub4    )  OCI_ATTR_WAIT,
                   dequeue->typinf->con->err)
    )

    OCI_RESULT(res);

    return res;
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
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_ENQUEUE, dequeue, FALSE);

    OCI_FREE(dequeue->agent_list);

    if ((consumers != NULL) && (count > 0))
    {
        dequeue->agent_list = (OCIAQAgent **) OCI_MemAlloc(OCI_IPC_ARRAY,
                                                           sizeof(OCIAQAgent *),
                                                           count, FALSE);

        if (dequeue->agent_list != NULL)
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
            res = FALSE;
        }
    }

    OCI_RESULT(res);

    return res;
}
