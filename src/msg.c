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
 * $Id: msg.c, v 3.8.1 2010-12-13 00:00 Vincent Rogier $
 * --------------------------------------------------------------------------------------------- */

#include "ocilib_internal.h"

/* ********************************************************************************************* *
 *                            PUBLIC FUNCTIONS
 * ********************************************************************************************* */

/* --------------------------------------------------------------------------------------------- *
 * OCI_MsgCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Msg * OCI_API OCI_MsgCreate
(
    OCI_TypeInfo *typinf
)
{
    OCI_Msg *msg = NULL;
    boolean res  = TRUE;

    OCI_CHECK_INITIALIZED(NULL);

    OCI_CHECK_PTR(OCI_IPC_TYPE_INFO, typinf, NULL);

    /* allocate message structure */

    msg = (OCI_Msg *) OCI_MemAlloc(OCI_IPC_MSG, sizeof(*msg), (size_t) 1, TRUE);

    if (msg != NULL)
    {
        msg->typinf = typinf;
        msg->ind    = OCI_IND_NULL;

        /* allocate message properties descriptor */

        res = (OCI_SUCCESS == OCI_DescriptorAlloc((dvoid * ) OCILib.env,
                                                  (dvoid **) &msg->proph,
                                                  OCI_DTYPE_AQMSG_PROPERTIES,
                                                  (size_t) 0, (dvoid **) NULL));

        if (res == TRUE)
        {
            /* allocate internal OCI_Object handle if payload is not RAW */

            if (msg->typinf->tcode != OCI_UNKNOWN)
            {
                msg->obj = OCI_ObjectCreate(typinf->con, typinf);

                res = (msg->obj != NULL);
            }
        }
    }
    else
        res = FALSE;

    /* check for failure */

    if (res == FALSE)
    {
        OCI_MsgFree(msg);
        msg = NULL;
    }

    return msg;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_MsgFree
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_MsgFree
(
    OCI_Msg *msg
)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_MSG, msg, FALSE);

    /* free local OCI_Agent object */

    if (msg->sender != NULL)
    {
        OCI_AgentFree(msg->sender);
    }

    /* free internal OCI_Object handle if payload is not RAW */

    if (msg->obj != NULL)
    {
        msg->obj->hstate =  OCI_OBJECT_ALLOCATED;

        OCI_ObjectFree(msg->obj);

        msg->obj = NULL;
    }

    /* free message RAW payload if necessary */

    if ((msg->typinf->tcode == OCI_UNKNOWN)&& ( msg->id != NULL))
    {
        OCI_CALL2
        (
            res, msg->typinf->con,

            OCIRawResize(OCILib.env, msg->typinf->con->err, 0, (OCIRaw **) &msg->payload)
        )
    }	

    /* free message ID */

    if (msg->id != NULL)
    {
        
        OCI_CALL2
        (
            res, msg->typinf->con,

            OCIRawResize(OCILib.env, msg->typinf->con->err, 0, (OCIRaw **) &msg->id)
        )
    }		

    msg->id = NULL;

    /* free OCI descriptor */

    OCI_DescriptorFree((dvoid *) msg->proph, OCI_DTYPE_AQMSG_PROPERTIES);
  
    OCI_FREE(msg);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_MsgReset
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_MsgReset
(
    OCI_Msg *msg
)
{
    boolean res = TRUE;

    unsigned int len = 0;
    void  *null_ptr  = NULL;

    res =   (
                OCI_MsgSetExpiration(msg, -1)            &&
                OCI_MsgSetEnqueueDelay(msg, 0)           &&
                OCI_MsgSetPriority(msg,0)                &&
                OCI_MsgSetOriginalID(msg, null_ptr, len) &&
                OCI_MsgSetSender(msg, NULL)              &&
                OCI_MsgSetConsumers(msg, null_ptr, len)  &&
                OCI_MsgSetCorrelation(msg, NULL)         &&
                OCI_MsgSetExceptionQueue(msg, NULL)
            );

    if (res == TRUE)
    {
        if (msg->typinf->tcode == OCI_UNKNOWN)
        {
            res = OCI_MsgSetRaw(msg, null_ptr, len);
        }
        else
        {
            res = OCI_MsgSetObject(msg, null_ptr);
        }
    }

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_MsgGetObject
 * --------------------------------------------------------------------------------------------- */

OCI_Object * OCI_API OCI_MsgGetObject
(
    OCI_Msg *msg
)
{
    OCI_Object *obj = NULL;

    OCI_CHECK_PTR(OCI_IPC_MSG, msg, NULL);
    OCI_CHECK_COMPAT(msg->typinf->con, msg->typinf->tcode != OCI_UNKNOWN, NULL);

    if (msg->ind != OCI_IND_NULL)
    {
        obj = (OCI_Object *) msg->obj;
    }

    OCI_RESULT(TRUE);

    return obj;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_MsgGetObject
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_MsgSetObject
(
    OCI_Msg    *msg,
    OCI_Object *obj
)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_MSG, msg, FALSE);

    OCI_CHECK_COMPAT(msg->typinf->con, msg->typinf->tcode != OCI_UNKNOWN, FALSE);

    if (obj != NULL)
    {
        /* assign the given object to the message internal object */

        res = OCI_ObjectAssign((OCI_Object *) msg->obj, obj);

        if (res == TRUE)
        {
            msg->ind = OCI_IND_NOTNULL;
        }
    }
    else
    {
        msg->ind = OCI_IND_NULL;
    }

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_MsgGetRaw
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_MsgGetRaw
(
    OCI_Msg      *msg,
    void         *raw,
    unsigned int *size
)
{
    unsigned int raw_size = 0;

    OCI_CHECK_PTR(OCI_IPC_MSG, msg, FALSE);
    OCI_CHECK_PTR(OCI_IPC_VOID, raw, FALSE);
    OCI_CHECK_PTR(OCI_IPC_VOID, size, FALSE);

    OCI_CHECK_COMPAT(msg->typinf->con, msg->typinf->tcode == OCI_UNKNOWN, FALSE);

    if ((msg->payload != NULL) && (msg->ind != OCI_IND_NULL))
    {
        raw_size = OCIRawSize(OCILib.env, (OCIRaw *) msg->payload);

        if (*size > raw_size)
            *size = raw_size;

        memcpy(raw, OCIRawPtr(OCILib.env, msg->payload), (size_t) (*size));
    }
    else
    {
        *size = 0;
    }

    OCI_RESULT(TRUE);

    return TRUE;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_MsgSetRaw
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_MsgSetRaw
(
    OCI_Msg      *msg,
    const void   *raw,
    unsigned int  size
)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_MSG, msg, FALSE);
 
    OCI_CHECK_COMPAT(msg->typinf->con, msg->typinf->tcode == OCI_UNKNOWN, FALSE);

    OCI_CALL2
    (
        res, msg->typinf->con,

        OCIRawAssignBytes(OCILib.env, msg->typinf->con->err, (ub1*) raw,
                            (ub4) size, (OCIRaw **) &msg->payload)
    )
 
    if ((res == TRUE) && (msg->payload != NULL) && (size > 0))
    {
        msg->ind = OCI_IND_NOTNULL;
    }
    else
    {
        msg->ind = OCI_IND_NULL;
    }

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_MsgGetAttemptCount
 * --------------------------------------------------------------------------------------------- */

int OCI_API OCI_MsgGetAttemptCount
(
    OCI_Msg *msg
)
{
    boolean res = TRUE;
    sb4 ret     = 0;

    OCI_CHECK_PTR(OCI_IPC_MSG, msg, 0);

    OCI_CALL2
    (
        res, msg->typinf->con,

        OCIAttrGet((dvoid *) msg->proph,
                   (ub4    ) OCI_DTYPE_AQMSG_PROPERTIES,
                   (dvoid *) &ret,
                   (ub4   *) NULL,
                   (ub4    ) OCI_ATTR_ATTEMPTS,
                   msg->typinf->con->err)
    )

    OCI_RESULT(res);

    return (int) ret;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_MsgGetEnqueueDelay
 * --------------------------------------------------------------------------------------------- */

int OCI_API OCI_MsgGetEnqueueDelay
(
    OCI_Msg *msg
)
{
    boolean res = TRUE;
    sb4 ret     = 0;

    OCI_CHECK_PTR(OCI_IPC_MSG, msg, 0);

    OCI_CALL2
    (
        res, msg->typinf->con,

        OCIAttrGet((dvoid *) msg->proph,
                   (ub4    ) OCI_DTYPE_AQMSG_PROPERTIES,
                   (dvoid *) &ret,
                   (ub4   *) NULL,
                   (ub4    ) OCI_ATTR_DELAY,
                   msg->typinf->con->err)
    )

    OCI_RESULT(res);

    return (int) ret;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_MsgSetEnqueueDelay
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_MsgSetEnqueueDelay
(
    OCI_Msg *msg,
    int      value
)
{
    boolean res = TRUE;
    sb4 sval    = (sb4) value;

    OCI_CHECK_PTR(OCI_IPC_MSG, msg, 0);

    OCI_CALL2
    (
        res, msg->typinf->con,

        OCIAttrSet((dvoid *) msg->proph,
                   (ub4    ) OCI_DTYPE_AQMSG_PROPERTIES,
                   (dvoid *) &sval,
                   (ub4    ) sizeof(sval),
                   (ub4    ) OCI_ATTR_DELAY,
                   msg->typinf->con->err)
    )

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_MsgGetEnqueueTime
 * --------------------------------------------------------------------------------------------- */

OCI_Date * OCI_API OCI_MsgGetEnqueueTime
(
    OCI_Msg *msg
)
{
    boolean res    = TRUE;
    OCI_Date *date = NULL;
    OCIDate oci_date;

    OCI_CHECK_PTR(OCI_IPC_MSG, msg, NULL);

    OCI_CALL2
    (
        res, msg->typinf->con,

        OCIAttrGet((dvoid *) msg->proph,
                   (ub4    ) OCI_DTYPE_AQMSG_PROPERTIES,
                   (dvoid *) &oci_date,
                   (ub4   *) NULL,
                   (ub4    ) OCI_ATTR_ENQ_TIME,
                   msg->typinf->con->err)
    )

    if (res == TRUE)
    {
        date = OCI_DateInit(msg->typinf->con, &msg->date, &oci_date,
                            FALSE, FALSE);

        res = (date != NULL);
    }

    OCI_RESULT(res);

    return date;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_MsgGetExpiration
 * --------------------------------------------------------------------------------------------- */

int OCI_API OCI_MsgGetExpiration
(
    OCI_Msg *msg
)
{
    boolean res = TRUE;
    sb4 ret     = 0;

    OCI_CHECK_PTR(OCI_IPC_MSG, msg, 0);

    OCI_CALL2
    (
        res, msg->typinf->con,

        OCIAttrGet((dvoid *) msg->proph,
                   (ub4    ) OCI_DTYPE_AQMSG_PROPERTIES,
                   (dvoid *) &ret,
                   (ub4   *) NULL,
                   (ub4    ) OCI_ATTR_EXPIRATION,
                   msg->typinf->con->err)
    )

    OCI_RESULT(res);

    return (int) ret;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_MsgSetExpiration
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_MsgSetExpiration
(
    OCI_Msg *msg,
    int      value
)
{
    boolean res = TRUE;
    sb4 sval    = (sb4) value;

    OCI_CHECK_PTR(OCI_IPC_MSG, msg, FALSE);

    OCI_CALL2
    (
        res, msg->typinf->con,

        OCIAttrSet((dvoid *) msg->proph,
                   (ub4    ) OCI_DTYPE_AQMSG_PROPERTIES,
                   (dvoid *) &sval,
                   (ub4    ) sizeof(sval),
                   (ub4    ) OCI_ATTR_EXPIRATION,
                   msg->typinf->con->err)
    )

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_MsgGetState
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_MsgGetState
(
    OCI_Msg *msg
)
{
    boolean res = TRUE;
    sb4 ret     = 0;

    OCI_CHECK_PTR(OCI_IPC_MSG, msg, OCI_UNKNOWN);

    OCI_CALL2
    (
        res, msg->typinf->con,

        OCIAttrGet((dvoid *) msg->proph,
                   (ub4    ) OCI_DTYPE_AQMSG_PROPERTIES,
                   (dvoid *) &ret,
                   (ub4   *) NULL,
                   (ub4    ) OCI_ATTR_MSG_STATE,
                   msg->typinf->con->err)
    )

    /* increment value to handle return code OCI_UNKNOWN on failure */

    if (res == TRUE)
    {
        ret++;
    }
    else
    {
        ret = OCI_UNKNOWN;
    }
    
    OCI_RESULT(res);
    
    return (int) ret;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_MsgGetPriority
 * --------------------------------------------------------------------------------------------- */

int OCI_API OCI_MsgGetPriority
(
    OCI_Msg *msg
)
{
    boolean res = TRUE;
    sb4 ret     = 0;

    OCI_CHECK_PTR(OCI_IPC_MSG, msg, 0);

    OCI_CALL2
    (
        res, msg->typinf->con,

        OCIAttrGet((dvoid *) msg->proph,
                   (ub4    ) OCI_DTYPE_AQMSG_PROPERTIES,
                   (dvoid *) &ret,
                   (ub4   *) NULL,
                   (ub4    ) OCI_ATTR_PRIORITY,
                   msg->typinf->con->err)
    )

    OCI_RESULT(res);

    return (int) ret;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_MsgSetPriority
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_MsgSetPriority
(
    OCI_Msg *msg,
    int      value
)
{
    boolean res = TRUE;
    sb4 sval    = (sb4) value;

    OCI_CHECK_PTR(OCI_IPC_MSG, msg, FALSE);

    OCI_CALL2
    (
        res, msg->typinf->con,

        OCIAttrSet((dvoid *) msg->proph,
                   (ub4    ) OCI_DTYPE_AQMSG_PROPERTIES,
                   (dvoid *) &sval,
                   (ub4    ) sizeof(sval),
                   (ub4    ) OCI_ATTR_PRIORITY,
                   msg->typinf->con->err)
    )

    OCI_RESULT(res);

    return res;
}


/* --------------------------------------------------------------------------------------------- *
 * OCI_MsgGetID
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_MsgGetID
(
    OCI_Msg      *msg,
    void         *id,
    unsigned int *len
)
{
    boolean res   = TRUE;

    OCI_CHECK_PTR(OCI_IPC_MSG,  msg, FALSE);
    OCI_CHECK_PTR(OCI_IPC_VOID, id,  FALSE);
    OCI_CHECK_PTR(OCI_IPC_VOID, len, FALSE);

    if (msg->id != NULL)
    {
        ub4 raw_len = 0;

        raw_len = OCIRawSize(OCILib.env, msg->id);

        if (*len > raw_len)
            *len = raw_len;

        memcpy(id, OCIRawPtr(OCILib.env, msg->id), (size_t) (*len));
    }
    else
    {
        *len = 0;
    }

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_MsgGetOriginalID
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_MsgGetOriginalID
(
    OCI_Msg      *msg,
    void         *id,
    unsigned int *len
)
{
    boolean res   = TRUE;
    OCIRaw *value = NULL;

    OCI_CHECK_PTR(OCI_IPC_MSG,  msg, FALSE);
    OCI_CHECK_PTR(OCI_IPC_VOID, id,  FALSE);
    OCI_CHECK_PTR(OCI_IPC_VOID, len, FALSE);

    OCI_CALL2
    (
        res, msg->typinf->con,

        OCIAttrGet((dvoid *) msg->proph,
                   (ub4    ) OCI_DTYPE_AQMSG_PROPERTIES,
                   (dvoid *) &value,
                   (ub4   *) NULL,
                   (ub4    ) OCI_ATTR_ORIGINAL_MSGID,
                   msg->typinf->con->err)
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
 * OCI_MsgSetOriginalID
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_MsgSetOriginalID
(
    OCI_Msg      *msg,
    const void   *id,
    unsigned int len
)
{
    boolean res   = TRUE;
    OCIRaw *value = NULL;

    OCI_CHECK_PTR(OCI_IPC_MSG,  msg, FALSE);

    OCI_CALL2
    (
        res, msg->typinf->con,

        OCIRawAssignBytes(OCILib.env, msg->typinf->con->err,
                          (ub1*) id, (ub4) len, (OCIRaw **) &value)
    )

    OCI_CALL2
    (
        res, msg->typinf->con,

        OCIAttrSet((dvoid *) msg->proph,
                   (ub4    ) OCI_DTYPE_AQMSG_PROPERTIES,
                   (dvoid *) &value,
                   (ub4    ) 0,
                   (ub4    ) OCI_ATTR_ORIGINAL_MSGID,
                   msg->typinf->con->err)
    )

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_MsgGetCorrelation
 * --------------------------------------------------------------------------------------------- */

const mtext * OCI_API OCI_MsgGetCorrelation
(
    OCI_Msg *msg
)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_MSG, msg, NULL);

    if (msg->correlation == NULL)
    {
        res = OCI_StringGetFromAttrHandle(msg->typinf->con,
                                          msg->proph,
                                          OCI_DTYPE_AQMSG_PROPERTIES,
                                          OCI_ATTR_CORRELATION,
                                          &msg->correlation);
    }

    OCI_RESULT(res);

    return msg->correlation;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_MsgSetCorrelation
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_MsgSetCorrelation
(
    OCI_Msg     *msg,
    const mtext *correlation
)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_MSG, msg, FALSE);

    res =  OCI_StringSetToAttrHandle(msg->typinf->con,
                                     msg->proph,
                                     OCI_DTYPE_AQMSG_PROPERTIES,
                                     OCI_ATTR_CORRELATION,
                                     &msg->correlation,
                                     correlation);

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_MsgGetExceptionQueue
 * --------------------------------------------------------------------------------------------- */

const mtext * OCI_API OCI_MsgGetExceptionQueue
(
    OCI_Msg *msg
)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_MSG, msg, NULL);

    if (msg->except_queue == NULL)
    {
        res = OCI_StringGetFromAttrHandle(msg->typinf->con,
                                          msg->proph,
                                          OCI_DTYPE_AQMSG_PROPERTIES,
                                          OCI_ATTR_EXCEPTION_QUEUE,
                                          &msg->except_queue);
    }

    OCI_RESULT(res);

    return msg->except_queue;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_MsgSetExceptionQueue
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_MsgSetExceptionQueue
(
    OCI_Msg     *msg,
    const mtext *queue
)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_MSG, msg, FALSE);

    res =  OCI_StringSetToAttrHandle(msg->typinf->con,
                                     msg->proph,
                                     OCI_DTYPE_AQMSG_PROPERTIES,
                                     OCI_ATTR_EXCEPTION_QUEUE,
                                     &msg->except_queue,
                                     queue);

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_MsgGetSender
 * --------------------------------------------------------------------------------------------- */

OCI_Agent * OCI_API OCI_MsgGetSender
(
    OCI_Msg   *msg
)
{
    boolean res = TRUE;
    OCIAQAgent *handle = NULL;
    OCI_Agent  *sender = NULL;

    OCI_CHECK_PTR(OCI_IPC_MSG, msg, NULL);

    OCI_CALL2
    (
        res, msg->typinf->con,

        OCIAttrGet((dvoid *) msg->proph,
                   (ub4    ) OCI_DTYPE_AQMSG_PROPERTIES,
                   (dvoid *) &handle,
                   (ub4    ) 0,
                   (ub4    ) OCI_ATTR_SENDER_ID,
                   msg->typinf->con->err)
    )

    if ((res == TRUE) && (handle != NULL))
    {
        sender = OCI_AgentInit(msg->typinf->con, &msg->sender, handle, NULL, NULL);
    }

    OCI_RESULT(res);

    return sender;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_MsgSetSender
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_MsgSetSender
(
    OCI_Msg   *msg,
    OCI_Agent *sender
)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_MSG, msg, FALSE);

    OCI_CALL2
    (
        res, msg->typinf->con,

        OCIAttrSet((dvoid *) msg->proph,
                   (ub4    ) OCI_DTYPE_AQMSG_PROPERTIES,
                   (dvoid *) (sender ? sender->handle : NULL),
                   (ub4    ) 0,
                   (ub4    ) OCI_ATTR_SENDER_ID,
                   msg->typinf->con->err)
    )

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_MsgSetConsumers
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_MsgSetConsumers
(
    OCI_Msg     *msg,
    OCI_Agent  **consumers,
    unsigned int count
)
{
    boolean res          = TRUE;
    OCIAQAgent **handles = NULL;

    OCI_CHECK_PTR(OCI_IPC_MSG, msg, FALSE);

    /* allocate local array of OCIAQAgent handles if needed */

    if ((consumers != NULL) && (count > 0))
    {
        handles = (OCIAQAgent **) OCI_MemAlloc(OCI_IPC_ARRAY,sizeof(OCIAQAgent *),
                                               count, FALSE);

        if (handles != NULL)
        {
            unsigned int i;

            for(i = 0; i < count; i++)
            {
                handles[i] = consumers[i]->handle;
            }
        }
    }
    else
    {
        count = 0;
    }

    OCI_CALL2
    (
        res, msg->typinf->con,

        OCIAttrSet((dvoid *) msg->proph,
                   (ub4    ) OCI_DTYPE_AQMSG_PROPERTIES,
                   (dvoid *) handles,
                   (ub4    ) count,
                   (ub4    ) OCI_ATTR_RECIPIENT_LIST,
                   msg->typinf->con->err)
    )


    /* free local array of OCIAQAgent handles if needed */

    if (handles != NULL)
    {
        OCI_FREE(handles);
    }

    OCI_RESULT(res);

    return res;
}
