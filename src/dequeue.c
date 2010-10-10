/*
   +----------------------------------------------------------------------+
   |                                                                      |
   |                     OCILIB - C Driver for Oracle                     |
   |                                                                      |
   |                      (C Wrapper for Oracle OCI)                      |
   |                                                                      |
   +----------------------------------------------------------------------+
   |                      Website : http://www.ocilib.net                 |
   +----------------------------------------------------------------------+
   |               Copyright (c) 2007-2010 Vincent ROGIER                 |
   +----------------------------------------------------------------------+
   | This library is free software; you can redistribute it and/or        |
   | modify it under the terms of the GNU Lesser General Public           |
   | License as published by the Free Software Foundation; either         |
   | version 2 of the License, or (at your option) any later version.     |
   |                                                                      |
   | This library is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU    |
   | Lesser General Public License for more details.                      |
   |                                                                      |
   | You should have received a copy of the GNU Lesser General Public     |
   | License along with this library; if not, write to the Free           |
   | Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.   |
   +----------------------------------------------------------------------+
   |          Author: Vincent ROGIER <vince.rogier@ocilib.net>            |
   +----------------------------------------------------------------------+
*/

/* ------------------------------------------------------------------------ *
 * $Id: dequeue.c, v 3.8.0 2010-10-09 19:30 Vincent Rogier $
 * ------------------------------------------------------------------------ */

#include "ocilib_internal.h"

/* ************************************************************************ *
 *                             PRIVATE FUNCTIONS
 * ************************************************************************ */

/* ************************************************************************ *
 *                            PUBLIC FUNCTIONS
 * ************************************************************************ */

/* ------------------------------------------------------------------------ *
 * OCI_DequeueCreate
 * ------------------------------------------------------------------------ */

OCI_Dequeue * OCI_API OCI_DequeueCreate(OCI_TypeInfo *typinf, const mtext *name)
{
    OCI_Dequeue *dequeue = NULL;
    boolean res  = TRUE;

    OCI_CHECK_INITIALIZED(NULL);

    OCI_CHECK_PTR(OCI_IPC_TYPE_INFO, typinf, NULL);
    OCI_CHECK_PTR(OCI_IPC_STRING, name, NULL);

    /* allocate message structure */

    dequeue = (OCI_Dequeue *) OCI_MemAlloc(OCI_IPC_ENQUEUE, sizeof(*dequeue),
                                       (size_t) 1, TRUE);

    if (dequeue != NULL)
    {
        dequeue->typinf = typinf;
        dequeue->name   = mtsdup(name);

        /* allocate enqueue options handle */

        res = (OCI_SUCCESS == OCI_DescriptorAlloc((dvoid * ) OCILib.env,
                                                  (dvoid **) &dequeue->opth,
                                                  OCI_DTYPE_AQDEQ_OPTIONS,
                                                  (size_t) 0, (dvoid **) NULL));

        dequeue->msg = OCI_MsgCreate(dequeue->typinf);

        res = (dequeue->msg != NULL);
    }
    else
        res = FALSE;

    if (res == FALSE)
    {
        OCI_DequeueFree(dequeue);
        dequeue = NULL;
    }

    return dequeue;
}

/* ------------------------------------------------------------------------ *
 * OCI_DequeueFree
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_DequeueFree(OCI_Dequeue *dequeue)
{
    OCI_CHECK_PTR(OCI_IPC_ENQUEUE, dequeue, FALSE);

    OCI_DescriptorFree((dvoid *) dequeue->opth, OCI_DTYPE_AQDEQ_OPTIONS);

    if (dequeue->msg != NULL)
    {
        OCI_MsgFree(dequeue->msg);
    }

    OCI_FREE(dequeue->name);
    OCI_FREE(dequeue->pattern);

    if (dequeue->agent != NULL)
    {
        OCI_AgentFree(dequeue->agent);
    }

    OCI_FREE(dequeue->agent_list);

    OCI_FREE(dequeue);

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_DequeueListen
 * ------------------------------------------------------------------------ */

OCI_Agent * OCI_API OCI_DequeueListen(OCI_Dequeue *dequeue, int timeout)
{
    boolean res = TRUE;
    OCI_Agent *agent = NULL;
    OCIAQAgent *handle = NULL;

    OCI_CHECK_PTR(OCI_IPC_ENQUEUE, dequeue, NULL);

    if (dequeue->agent_list != NULL)
    {
        sword ret;
        sb4 code;

        ret =  OCIAQListen(dequeue->typinf->con->cxt, dequeue->typinf->con->err,
                        dequeue->agent_list, (ub4) dequeue->agent_count,
                        (sb4) timeout, &handle, OCI_DEFAULT);

        if (ret == OCI_ERROR)
        {
            OCIErrorGet((dvoid *) dequeue->typinf->con->err, (ub4) 1,
                        (OraText *) NULL, &code, (OraText *) NULL, (ub4) 0,
                        (ub4) OCI_HTYPE_ERROR);

            if (code != 25254)
            {
                OCI_ExceptionOCI(dequeue->typinf->con->err,
                                 dequeue->typinf->con,
                                 NULL, FALSE);

                res = FALSE;
            }
        }

        if ((res == TRUE) && (ret == OCI_SUCCESS))
        {
            agent = OCI_AgentInit(dequeue->typinf->con, &dequeue->agent,
                                  handle, NULL, NULL);
        }
    }

    OCI_RESULT(res);

    return agent;
}

/* ------------------------------------------------------------------------ *
 * OCI_DequeueGet
 * ------------------------------------------------------------------------ */

OCI_Msg * OCI_API OCI_DequeueGet(OCI_Dequeue *dequeue)
{
    boolean res   = TRUE;

    void  *ostr   = NULL;
    int    osize  = -1;

    OCI_Msg *msg  = NULL;

    void *payload = NULL;
    void *ind     = NULL;

    OCI_CHECK_PTR(OCI_IPC_ENQUEUE, dequeue, FALSE);

    ostr = OCI_GetInputMetaString(dequeue->name, &osize);

    if (mtscmp(dequeue->typinf->name, OCI_RAW_OBJECT_TYPE) != 0)
    {
        payload = &dequeue->msg->obj->handle;
        ind     = &dequeue->msg->obj->tab_ind;
    }
    else
    {
        payload = &dequeue->msg->raw;
        ind     = NULL;
    }

    OCI_CALL2
    (
        res, dequeue->typinf->con,

        OCIAQDeq(dequeue->typinf->con->cxt, dequeue->typinf->con->err,
                 ostr, dequeue->opth, dequeue->msg->proph, dequeue->typinf->tdo,
                 payload, ind, &dequeue->msg->id, OCI_DEFAULT);
    )

    if (res == TRUE)
    {
        msg = dequeue->msg;
    }

    OCI_RESULT(res);

    return msg;
}

/* ------------------------------------------------------------------------ *
 * OCI_DequeueSetConsumerName
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_DequeueSetConsumer(OCI_Dequeue *dequeue, const mtext *consumer)
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

/* ------------------------------------------------------------------------ *
 * OCI_DequeueGetConsumerName
 * ------------------------------------------------------------------------ */

const mtext * OCI_API OCI_DequeueGetConsumer(OCI_Dequeue *dequeue)
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

/* ------------------------------------------------------------------------ *
 * OCI_DequeueSetCorrelation
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_DequeueSetCorrelation(OCI_Dequeue *dequeue, const mtext *pattern)
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

/* ------------------------------------------------------------------------ *
 * OCI_DequeueGetCorrelation
 * ------------------------------------------------------------------------ */

const mtext * OCI_API OCI_DequeueGetCorrelation(OCI_Dequeue *dequeue)
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

/* ------------------------------------------------------------------------ *
 * OCI_DequeueSetMsgID
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_DequeueSetMsgID(OCI_Dequeue *dequeue,
                                    const void *msg_id,
                                    unsigned int len)
{
    boolean res    = TRUE;
    OCIRaw  *value = NULL;

    OCI_CHECK_PTR(OCI_IPC_DEQUEUE, dequeue, FALSE);

    OCI_CALL2
    (
        res, dequeue->typinf->con,

        OCIRawAssignBytes(OCILib.env, dequeue->typinf->con->err,
                          (ub1*) msg_id, (ub4) len, (OCIRaw **) &value)
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

/* ------------------------------------------------------------------------ *
 * OCI_DequeueSetMsgID
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_DequeueGetMsgID(OCI_Dequeue *dequeue,
                                    void *msg_id,
                                    unsigned int len)
{
    boolean res    = TRUE;
    OCIRaw  *value = NULL;

    OCI_CHECK_PTR(OCI_IPC_DEQUEUE, dequeue, FALSE);

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

        if (len > raw_len)
            len = raw_len;

        memcpy(msg_id, OCIRawPtr(OCILib.env, value), (size_t) len);
    }

    OCI_RESULT(res);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_DequeueSetVisibility
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_DequeueSetVisibility(OCI_Dequeue *dequeue, 
                                         unsigned int visibility)
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

/* ------------------------------------------------------------------------ *
 * OCI_DequeueGetVisibility
 * ------------------------------------------------------------------------ */

unsigned int OCI_API OCI_DequeueGetVisibility(OCI_Dequeue *dequeue)
{
    boolean res = TRUE;
    ub4 ret = 0;

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

/* ------------------------------------------------------------------------ *
 * OCI_DequeueSetMode
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_DequeueSetMode(OCI_Dequeue *dequeue, unsigned int mode)
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

/* ------------------------------------------------------------------------ *
 * OCI_DequeueGetMode
 * ------------------------------------------------------------------------ */

unsigned int OCI_API OCI_DequeueGetMode(OCI_Dequeue *dequeue)
{
    boolean res = TRUE;
    ub4 ret = 0;

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

/* ------------------------------------------------------------------------ *
 * OCI_DequeueSetNavigation
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_DequeueSetNavigation(OCI_Dequeue *dequeue, 
                                         unsigned int position)
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
                   (ub4    )  OCI_ATTR_NAVIGATION,
                   dequeue->typinf->con->err)
    )

    OCI_RESULT(res);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_DequeueGetNavigation
 * ------------------------------------------------------------------------ */

unsigned int OCI_API OCI_DequeueGetNavigation(OCI_Dequeue *dequeue)
{
    boolean res = TRUE;
    ub4 ret = 0;

    OCI_CHECK_PTR(OCI_IPC_DEQUEUE, dequeue, FALSE);

    OCI_CALL2
    (
        res, dequeue->typinf->con,

        OCIAttrGet((dvoid *) dequeue->opth,
                   (ub4    ) OCI_DTYPE_AQDEQ_OPTIONS,
                   (dvoid *) &ret,
                   (ub4   *) NULL,
                   (ub4    )  OCI_ATTR_NAVIGATION,
                   dequeue->typinf->con->err)
    )


    OCI_RESULT(res);

    return (int) ret;
}

/* ------------------------------------------------------------------------ *
 * OCI_DequeueSetWaitTime
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_DequeueSetWaitTime(OCI_Dequeue *dequeue, int timeout)
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

/* ------------------------------------------------------------------------ *
 * OCI_DequeueGetWaitTime
 * ------------------------------------------------------------------------ */

int OCI_API OCI_DequeueGetWaitTime(OCI_Dequeue *dequeue)
{
    boolean res = TRUE;
    sb4 ret = 0;

    OCI_CHECK_PTR(OCI_IPC_DEQUEUE, dequeue, FALSE);

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

/* ------------------------------------------------------------------------ *
 * OCI_DequeueSetAgentList
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_DequeueSetAgentList(OCI_Dequeue *dequeue,
                                        OCI_Agent **consumers,
                                        unsigned int count)
{
    boolean res  = TRUE;

    OCI_CHECK_PTR(OCI_IPC_ENQUEUE, dequeue, FALSE);
    OCI_CHECK_PTR(OCI_IPC_AGENT, consumers, FALSE);

    OCI_CHECK_MIN(dequeue->typinf->con, NULL, count, 1, FALSE);

    OCI_FREE(dequeue->agent_list);

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

    OCI_RESULT(res);

    return res;
}
