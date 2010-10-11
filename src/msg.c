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
 * $Id: msg.c, v 3.8.0 2010-10-09 19:30 Vincent Rogier $
 * ------------------------------------------------------------------------ */

#include "ocilib_internal.h"

/* ************************************************************************ *
 *                             PRIVATE FUNCTIONS
 * ************************************************************************ */


/* ************************************************************************ *
 *                            PUBLIC FUNCTIONS
 * ************************************************************************ */

/* ------------------------------------------------------------------------ *
 * OCI_MsgCreate
 * ------------------------------------------------------------------------ */

OCI_Msg * OCI_API OCI_MsgCreate(OCI_TypeInfo *typinf)
{
    OCI_Msg *msg = NULL;
    boolean res  = TRUE;

    OCI_CHECK_INITIALIZED(NULL);

    OCI_CHECK_PTR(OCI_IPC_TYPE_INFO, typinf, NULL);

    /* allocate message structure */

    msg = (OCI_Msg *) OCI_MemAlloc(OCI_IPC_MSG, sizeof(*msg), 
                                       (size_t) 1, TRUE);

    if (msg != NULL)
    {
        msg->typinf = typinf;

        /* allocate message properties handle */

        res = (OCI_SUCCESS == OCI_DescriptorAlloc((dvoid * ) OCILib.env, 
                                                  (dvoid **) &msg->proph,
                                                  OCI_DTYPE_AQMSG_PROPERTIES, 
                                                  (size_t) 0, (dvoid **) NULL));

        if (res == TRUE)
        {
            if (mtscmp(msg->typinf->name, OCI_RAW_OBJECT_TYPE) != 0)
            {
                msg->obj = OCI_ObjectCreate(typinf->con, typinf);

                res = (msg->obj != NULL);
            }
        }
    }
    else
        res = FALSE;

    if (res == FALSE)
    {
        OCI_MsgFree(msg);
        msg = NULL;
    }

    return msg;
}
/* ------------------------------------------------------------------------ *
 * OCI_MsgFree
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_MsgFree(OCI_Msg *msg)
{
    OCI_CHECK_PTR(OCI_IPC_MSG, msg, FALSE);

    OCI_DescriptorFree((dvoid *) msg->proph, OCI_DTYPE_AQMSG_PROPERTIES);

    if (msg->obj != NULL)
    {
        OCI_ObjectFree(msg->obj);
    }

    OCI_FREE(msg);

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_MsgGetObject
 * ------------------------------------------------------------------------ */

OCI_Object * OCI_API OCI_MsgGetObject(OCI_Msg *msg)
{
    OCI_Object *obj = NULL;

    OCI_CHECK_PTR(OCI_IPC_MSG, msg, NULL);

    if (mtscmp(msg->typinf->name, OCI_RAW_OBJECT_TYPE) != 0)
    {
        obj = msg->obj;
    }

    OCI_RESULT(TRUE);

    return obj;
}

/* ------------------------------------------------------------------------ *
 * OCI_MsgGetObject
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_MsgSetObject(OCI_Msg *msg, OCI_Object *obj)
{
    boolean res = FALSE;

    OCI_CHECK_PTR(OCI_IPC_MSG, msg, FALSE);

    if (mtscmp(msg->typinf->name, OCI_RAW_OBJECT_TYPE) != 0)
    {
        res = OCI_ObjectAssign(msg->obj, obj);
    }

    OCI_RESULT(res);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_MsgGetRaw
 * ------------------------------------------------------------------------ */

void * OCI_API OCI_MsgGetRaw(OCI_Msg *msg)
{
    void *raw = NULL;

    OCI_CHECK_PTR(OCI_IPC_MSG, msg, FALSE);

    if (mtscmp(msg->typinf->name, OCI_RAW_OBJECT_TYPE) == 0)
    {
        raw = OCIRawPtr(OCILib.env, msg->raw);
    }
    
    OCI_RESULT(TRUE);

    return raw;
}

/* ------------------------------------------------------------------------ *
 * OCI_MsgSetRaw
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_MsgSetRaw(OCI_Msg *msg, void *raw, unsigned int size)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_MSG, msg, FALSE);
    OCI_CHECK_PTR(OCI_IPC_VOID, raw, FALSE);

    if (mtscmp(msg->typinf->name, OCI_RAW_OBJECT_TYPE) == 0)
    {
        OCI_CALL2
        (
            res, msg->typinf->con, 
            
            OCIRawAssignBytes(OCILib.env, msg->typinf->con->err, (ub1*) raw,
                              (ub4) size, (OCIRaw **) msg->raw)
        )    
    }
    
    OCI_RESULT(res);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_MsgGetAttemptCount
 * ------------------------------------------------------------------------ */

int OCI_API OCI_MsgGetAttemptCount(OCI_Msg *msg)
{
    boolean res = TRUE;
    sb4 ret = 0;

    OCI_CHECK_PTR(OCI_IPC_MSG, msg, FALSE);

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

/* ------------------------------------------------------------------------ *
 * OCI_MsgGetEnqueueDelay
 * ------------------------------------------------------------------------ */

int OCI_API OCI_MsgGetEnqueueDelay(OCI_Msg *msg)
{
    boolean res = TRUE;
    sb4 ret = 0;

    OCI_CHECK_PTR(OCI_IPC_MSG, msg, FALSE);

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

/* ------------------------------------------------------------------------ *
 * OCI_MsgSetEnqueueDelay
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_MsgSetEnqueueDelay(OCI_Msg *msg, int value)
{
    boolean res = TRUE;
    sb4 sval = (sb4) value;

    OCI_CHECK_PTR(OCI_IPC_MSG, msg, FALSE);

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

/* ------------------------------------------------------------------------ *
 * OCI_MsgGetEnqueueTime
 * ------------------------------------------------------------------------ */

OCI_Date * OCI_API OCI_MsgGetEnqueueTime(OCI_Msg *msg)
{
    boolean   res  = TRUE;
    OCI_Date *date = NULL;
    OCIDate oci_date;

    OCI_CHECK_PTR(OCI_IPC_MSG, msg, FALSE);

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

/* ------------------------------------------------------------------------ *
 * OCI_MsgGetExpiration
 * ------------------------------------------------------------------------ */

int OCI_API OCI_MsgGetExpiration(OCI_Msg *msg)
{
    boolean res = TRUE;
    sb4 ret = 0;

    OCI_CHECK_PTR(OCI_IPC_MSG, msg, FALSE);

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

/* ------------------------------------------------------------------------ *
 * OCI_MsgSetExpiration
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_MsgSetExpiration(OCI_Msg *msg, int value)
{
    boolean res = TRUE;
    sb4 sval = (sb4) value;

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

/* ------------------------------------------------------------------------ *
 * OCI_MsgGetState
 * ------------------------------------------------------------------------ */

unsigned int OCI_API OCI_MsgGetState(OCI_Msg *msg)
{
    boolean res = TRUE;
    sb4 ret = 0;

    OCI_CHECK_PTR(OCI_IPC_MSG, msg, 0);

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


    OCI_RESULT(res);

    return (int) ret;
}

/* ------------------------------------------------------------------------ *
 * OCI_MsgGetPriority
 * ------------------------------------------------------------------------ */

int OCI_API OCI_MsgGetPriority(OCI_Msg *msg)
{
    boolean res = TRUE;
    sb4 ret = 0;

    OCI_CHECK_PTR(OCI_IPC_MSG, msg, FALSE);

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

/* ------------------------------------------------------------------------ *
 * OCI_MsgSetPriority
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_MsgSetPriority(OCI_Msg *msg, int value)
{
    boolean res = TRUE;
    sb4 sval = (sb4) value;

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

/* ------------------------------------------------------------------------ *
 * OCI_MsgGetOriginalID
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_MsgGetOriginalID(OCI_Msg *msg,
                                     void *msg_id, 
                                     unsigned int len)
{
    boolean res    = TRUE;
    OCIRaw  *value = NULL;

    OCI_CHECK_PTR(OCI_IPC_MSG, msg, FALSE);

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

        if (len > raw_len)
            len = raw_len;

        memcpy(msg_id, OCIRawPtr(OCILib.env, value), (size_t) len);
    }

    OCI_RESULT(res);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_MsgSetOriginalID
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_MsgSetOriginalID(OCI_Msg *msg,
                                     const void *msg_id, 
                                     unsigned int len)
{
    boolean res    = TRUE;
    OCIRaw  *value = NULL;

    OCI_CHECK_PTR(OCI_IPC_MSG, msg, FALSE);

    OCI_CALL2
    (
        res, msg->typinf->con,
        
        OCIRawAssignBytes(OCILib.env, msg->typinf->con->err, 
                          (ub1*) msg_id, (ub4) len, (OCIRaw **) &value)
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

/* ------------------------------------------------------------------------ *
 * OCI_MsgGetCorrelation
 * ------------------------------------------------------------------------ */

const mtext * OCI_API OCI_MsgGetCorrelation(OCI_Msg *msg)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_MSG, msg, FALSE);

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

/* ------------------------------------------------------------------------ *
 * OCI_MsgSetCorrelation
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_MsgSetCorrelation(OCI_Msg *msg, const mtext *correlation)
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

/* ------------------------------------------------------------------------ *
 * OCI_MsGSetExceptionQueue
 * ------------------------------------------------------------------------ */

const mtext * OCI_API OCI_MsGSetExceptionQueue(OCI_Msg *msg)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_MSG, msg, NULL);

    if (msg->except_queue == NULL)
    {
        res = OCI_StringGetFromAttrHandle(msg->typinf->con, 
                                          msg->proph,
                                          OCI_DTYPE_AQMSG_PROPERTIES,
                                          OCI_ATTR_CORRELATION, 
                                          &msg->except_queue);
    }

    OCI_RESULT(res);

    return msg->except_queue;
}

/* ------------------------------------------------------------------------ *
 * OCI_MsgSetExceptionQueue
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_MsgSetExceptionQueue(OCI_Msg *msg, const mtext *queue)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_MSG, msg, FALSE);

    res =  OCI_StringSetToAttrHandle(msg->typinf->con, 
                                     msg->proph,
                                     OCI_DTYPE_AQMSG_PROPERTIES,
                                     OCI_ATTR_CORRELATION, 
                                     &msg->except_queue,
                                     queue);

    OCI_RESULT(res);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_MsgSetSender
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_MsgSetSender(OCI_Msg *msg, OCI_Agent *sender)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_MSG, msg, FALSE);
    OCI_CHECK_PTR(OCI_IPC_AGENT, sender, FALSE);

    OCI_CALL2
    (
        res, msg->typinf->con,

        OCIAttrSet((dvoid *) msg->proph, 
                   (ub4    ) OCI_DTYPE_AQMSG_PROPERTIES,
                   (dvoid *) sender->handle, 
                   (ub4    ) 0,
                   (ub4    ) OCI_ATTR_SENDER_ID, 
                   msg->typinf->con->err)
    )

    OCI_RESULT(res);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_MsgSetConsumers
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_MsgSetConsumers(OCI_Msg *msg, OCI_Agent **consumers, 
                                    unsigned int count)
{
    boolean res = TRUE;
    OCIAQAgent **handles = NULL;

    OCI_CHECK_PTR(OCI_IPC_MSG, msg, FALSE);
    OCI_CHECK_PTR(OCI_IPC_AGENT, consumers, FALSE);

    OCI_CHECK_MIN(msg->typinf->con, NULL, count, 1, FALSE);

    handles = (OCIAQAgent **) OCI_MemAlloc(OCI_IPC_ARRAY,sizeof(OCIAQAgent *), 
                                           count, FALSE);

    if (handles != NULL)
    {
        unsigned int i;

        for(i = 0; i < count; i++)
        {
            handles[i] = consumers[i]->handle;
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

        OCI_FREE(handles);
    }
    else
    {
        res = FALSE;
    }

    OCI_RESULT(res);

    return res;
}
