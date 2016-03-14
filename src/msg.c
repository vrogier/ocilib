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

    OCI_LIB_CALL_ENTER(OCI_Msg*, NULL)

    OCI_CHECK_PTR(OCI_IPC_TYPE_INFO, typinf)

    /* allocate message structure */

    msg = (OCI_Msg *) OCI_MemAlloc(OCI_IPC_MSG, sizeof(*msg), (size_t) 1, TRUE);

    if (msg)
    {
        msg->typinf = typinf;
        msg->ind    = OCI_IND_NULL;

        /* allocate message properties descriptor */

        call_status = OCI_SUCCESSFUL(OCI_DescriptorAlloc((dvoid * ) msg->typinf->con->env,
                                                                    (dvoid **) &msg->proph,
                                                                    OCI_DTYPE_AQMSG_PROPERTIES,
                                                                    (size_t) 0, (dvoid **) NULL));

        if (call_status)
        {
            /* allocate internal OCI_Object handle if payload is not RAW */

            if (OCI_UNKNOWN != msg->typinf->typecode)
            {
                msg->obj = OCI_ObjectCreate(typinf->con, typinf);

                call_status = (NULL != msg->obj);
            }
        }
    }

    /* check for failure */

    if (call_status)
    {
        call_retval = msg;
    }
    else if (msg)
    {
        OCI_MsgFree(msg);
        msg = NULL;
    }

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_MsgFree
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_MsgFree
(
    OCI_Msg *msg
)
{
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_MSG, msg)

    call_retval = call_status = TRUE;

    /* free local OCI_Agent object */

    if (msg->sender)
    {
        OCI_AgentFree(msg->sender);
    }

    /* free internal OCI_Object handle if payload is not RAW */

    if (msg->obj)
    {
        msg->obj->hstate =  OCI_OBJECT_ALLOCATED;

        OCI_ObjectFree(msg->obj);

        msg->obj = NULL;
    }

    /* free message RAW payload if necessary */

    if (msg->id && (OCI_UNKNOWN == msg->typinf->typecode))
    {
        OCI_CALL2
        (
            call_status, msg->typinf->con,

            OCIRawResize(msg->typinf->con->env, msg->typinf->con->err, 0, (OCIRaw **) &msg->payload)
        )
    }

    /* free message ID */

    if (msg->id)
    {

        OCI_CALL2
        (
            call_status, msg->typinf->con,

            OCIRawResize(msg->typinf->con->env, msg->typinf->con->err, 0, (OCIRaw **) &msg->id)
        )
    }

    msg->id = NULL;

    /* free OCI descriptor */

    OCI_DescriptorFree((dvoid *) msg->proph, OCI_DTYPE_AQMSG_PROPERTIES);

    OCI_FREE(msg)

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_MsgReset
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_MsgReset
(
    OCI_Msg *msg
)
{
    unsigned int  len = 0;

    boolean res = 
    (
        OCI_MsgSetExpiration(msg, -1)            &&
        OCI_MsgSetEnqueueDelay(msg, 0)           &&
        OCI_MsgSetPriority(msg,0)                &&
        OCI_MsgSetOriginalID(msg, NULL, len)     &&
        OCI_MsgSetSender(msg, NULL)              &&
        OCI_MsgSetConsumers(msg, NULL, len)      &&
        OCI_MsgSetCorrelation(msg, NULL)         &&
        OCI_MsgSetExceptionQueue(msg, NULL)
    );

    if (res)
    {
        if (OCI_UNKNOWN == msg->typinf->typecode)
        {
            res = OCI_MsgSetRaw(msg, NULL, len);
        }
        else
        {
            res = OCI_MsgSetObject(msg, NULL);
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
    OCI_LIB_CALL_ENTER(OCI_Object *, NULL)

    OCI_CHECK_PTR(OCI_IPC_MSG, msg)
    OCI_CHECK_COMPAT(msg->typinf->con, msg->typinf->typecode != OCI_UNKNOWN)

    if (OCI_IND_NULL != msg->ind)
    {
        call_retval = (OCI_Object *) msg->obj;
    }

    call_status = TRUE;

    OCI_LIB_CALL_EXIT()
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
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_MSG, msg)
    OCI_CHECK_COMPAT(msg->typinf->con, msg->typinf->typecode != OCI_UNKNOWN)

    if (obj)
    {
        /* assign the given object to the message internal object */

        call_status = OCI_ObjectAssign((OCI_Object *) msg->obj, obj);

        if (call_status)
        {
            msg->ind = OCI_IND_NOTNULL;
        }
    }
    else
    {
        call_status = TRUE;
        msg->ind    = OCI_IND_NULL;
    }

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
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
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_MSG, msg)
    OCI_CHECK_PTR(OCI_IPC_VOID, raw)
    OCI_CHECK_PTR(OCI_IPC_VOID, size)

    OCI_CHECK_COMPAT(msg->typinf->con, OCI_UNKNOWN == msg->typinf->typecode)

    if ((msg->payload) && (OCI_IND_NULL != msg->ind))
    {
        unsigned int raw_size = OCIRawSize(msg->typinf->con->env, (OCIRaw *) msg->payload);

        if (*size > raw_size)
        {
            *size = raw_size;
        }

        memcpy(raw, OCIRawPtr(msg->typinf->con->env, msg->payload), (size_t) (*size));
    }
    else
    {
        *size = 0;
    }

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
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
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_MSG, msg)
    OCI_CHECK_COMPAT(msg->typinf->con, OCI_UNKNOWN ==  msg->typinf->typecode)

    call_status = TRUE;

    OCI_CALL2
    (
        call_status, msg->typinf->con,

        OCIRawAssignBytes(msg->typinf->con->env, msg->typinf->con->err,
                          (ub1*) raw, (ub4) size, (OCIRaw **) &msg->payload)
    )

    if (call_status && msg->payload && (size > 0))
    {
        msg->ind = OCI_IND_NOTNULL;
    }
    else
    {
        msg->ind = OCI_IND_NULL;
    }

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_MsgGetAttemptCount
 * --------------------------------------------------------------------------------------------- */

int OCI_API OCI_MsgGetAttemptCount
(
    OCI_Msg *msg
)
{
    sb4 value = 0;

    OCI_LIB_CALL_ENTER(int, value)

    OCI_CHECK_PTR(OCI_IPC_MSG, msg)

    call_status = TRUE;

    OCI_CALL2
    (
        call_status, msg->typinf->con,

        OCIAttrGet((dvoid *) msg->proph,
                   (ub4    ) OCI_DTYPE_AQMSG_PROPERTIES,
                   (dvoid *) &value,
                   (ub4   *) NULL,
                   (ub4    ) OCI_ATTR_ATTEMPTS,
                   msg->typinf->con->err)
    )

    call_retval = value;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_MsgGetEnqueueDelay
 * --------------------------------------------------------------------------------------------- */

int OCI_API OCI_MsgGetEnqueueDelay
(
    OCI_Msg *msg
)
{
    sb4 value = 0;

    OCI_LIB_CALL_ENTER(int, 0)

    OCI_CHECK_PTR(OCI_IPC_MSG, msg)

    call_status = TRUE;

    OCI_CALL2
    (
        call_status, msg->typinf->con,

        OCIAttrGet((dvoid *) msg->proph,
                   (ub4    ) OCI_DTYPE_AQMSG_PROPERTIES,
                   (dvoid *) &value,
                   (ub4   *) NULL,
                   (ub4    ) OCI_ATTR_DELAY,
                   msg->typinf->con->err)
    )

    call_retval = value;

    OCI_LIB_CALL_EXIT()
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
    sb4 sval = (sb4) value;

    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_MSG, msg)

    call_status = TRUE;

    OCI_CALL2
    (
        call_status, msg->typinf->con,

        OCIAttrSet((dvoid *) msg->proph,
                   (ub4    ) OCI_DTYPE_AQMSG_PROPERTIES,
                   (dvoid *) &sval,
                   (ub4    ) sizeof(sval),
                   (ub4    ) OCI_ATTR_DELAY,
                   msg->typinf->con->err)
    )

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_MsgGetEnqueueTime
 * --------------------------------------------------------------------------------------------- */

OCI_Date * OCI_API OCI_MsgGetEnqueueTime
(
    OCI_Msg *msg
)
{
    OCIDate date;

    OCI_LIB_CALL_ENTER(OCI_Date*, NULL)

    OCI_CHECK_PTR(OCI_IPC_MSG, msg)

    call_status = TRUE;

    OCI_CALL2
    (
        call_status, msg->typinf->con,

        OCIAttrGet((dvoid *) msg->proph,
                   (ub4    ) OCI_DTYPE_AQMSG_PROPERTIES,
                   (dvoid *) &date,
                   (ub4   *) NULL,
                   (ub4    ) OCI_ATTR_ENQ_TIME,
                   msg->typinf->con->err)
    )

    if (call_status)
    {
        call_retval = OCI_DateInit(msg->typinf->con, &msg->date, &date, FALSE, FALSE);

        call_status = (NULL != call_retval);
    }

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_MsgGetExpiration
 * --------------------------------------------------------------------------------------------- */

int OCI_API OCI_MsgGetExpiration
(
    OCI_Msg *msg
)
{
    sb4 value = 0;

    OCI_LIB_CALL_ENTER(int, 0)

    OCI_CHECK_PTR(OCI_IPC_MSG, msg)

    call_status = TRUE;

    OCI_CALL2
    (
        call_status, msg->typinf->con,

        OCIAttrGet((dvoid *) msg->proph,
                   (ub4    ) OCI_DTYPE_AQMSG_PROPERTIES,
                   (dvoid *) &value,
                   (ub4   *) NULL,
                   (ub4    ) OCI_ATTR_EXPIRATION,
                   msg->typinf->con->err)
    )

    call_retval = value;

    OCI_LIB_CALL_EXIT()
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
    sb4 sval = (sb4) value;

    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_MSG, msg)

    call_status = TRUE;

    OCI_CALL2
    (
        call_status, msg->typinf->con,

        OCIAttrSet((dvoid *) msg->proph,
                   (ub4    ) OCI_DTYPE_AQMSG_PROPERTIES,
                   (dvoid *) &sval,
                   (ub4    ) sizeof(sval),
                   (ub4    ) OCI_ATTR_EXPIRATION,
                   msg->typinf->con->err)
    )

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_MsgGetState
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_MsgGetState
(
    OCI_Msg *msg
)
{
    sb4 value = 0;

    OCI_LIB_CALL_ENTER(unsigned int, OCI_UNKNOWN)

    OCI_CHECK_PTR(OCI_IPC_MSG, msg)

    call_status = TRUE;

    OCI_CALL2
    (
        call_status, msg->typinf->con,

        OCIAttrGet((dvoid *) msg->proph,
                   (ub4    ) OCI_DTYPE_AQMSG_PROPERTIES,
                   (dvoid *) &value,
                   (ub4   *) NULL,
                   (ub4    ) OCI_ATTR_MSG_STATE,
                   msg->typinf->con->err)
    )

    /* increment value to handle return code OCI_UNKNOWN on failure */

    if (call_status)
    {
        call_retval = ++value;
    }

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_MsgGetPriority
 * --------------------------------------------------------------------------------------------- */

int OCI_API OCI_MsgGetPriority
(
    OCI_Msg *msg
)
{
    sb4 value = 0;

    OCI_LIB_CALL_ENTER(int, FALSE)

    OCI_CHECK_PTR(OCI_IPC_MSG, msg)

    call_status = TRUE;

    OCI_CALL2
    (
        call_status, msg->typinf->con,

        OCIAttrGet((dvoid *) msg->proph,
                   (ub4    ) OCI_DTYPE_AQMSG_PROPERTIES,
                   (dvoid *) &value,
                   (ub4   *) NULL,
                   (ub4    ) OCI_ATTR_PRIORITY,
                   msg->typinf->con->err)
    )

    call_retval = value;

    OCI_LIB_CALL_EXIT()
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
    sb4 sval = (sb4) value;

    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_MSG, msg)

    call_status = TRUE;

    OCI_CALL2
    (
        call_status, msg->typinf->con,

        OCIAttrSet((dvoid *) msg->proph,
                   (ub4    ) OCI_DTYPE_AQMSG_PROPERTIES,
                   (dvoid *) &sval,
                   (ub4    ) sizeof(sval),
                   (ub4    ) OCI_ATTR_PRIORITY,
                   msg->typinf->con->err)
    )

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
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
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_MSG, msg)
    OCI_CHECK_PTR(OCI_IPC_VOID, id)
    OCI_CHECK_PTR(OCI_IPC_VOID, len)

    if (msg->id)
    {
        ub4 raw_len = OCIRawSize(msg->typinf->con->env, msg->id);

        if (*len > raw_len)
        {
            *len = raw_len;
        }

        memcpy(id, OCIRawPtr(msg->typinf->con->env, msg->id), (size_t) (*len));
    }
    else
    {
        *len = 0;
    }

    call_retval = call_status = TRUE;

    OCI_LIB_CALL_EXIT()
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
    OCIRaw *value = NULL;

    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_MSG, msg)
    OCI_CHECK_PTR(OCI_IPC_VOID, id)
    OCI_CHECK_PTR(OCI_IPC_VOID, len)

    *len = 0;

    call_status = TRUE;

    OCI_CALL2
    (
        call_status, msg->typinf->con,

        OCIAttrGet((dvoid *) msg->proph,
                   (ub4    ) OCI_DTYPE_AQMSG_PROPERTIES,
                   (dvoid *) &value,
                   (ub4   *) NULL,
                   (ub4    ) OCI_ATTR_ORIGINAL_MSGID,
                   msg->typinf->con->err)
    )

    if (call_status && value)
    {
        ub4 raw_len = OCIRawSize(msg->typinf->con->env, value);

        if (*len > raw_len)
        {
            *len = raw_len;
        }

        memcpy(id, OCIRawPtr(msg->typinf->con->env, value), (size_t) (*len));
    }

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
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
    OCIRaw *value = NULL;

    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_MSG, msg)

    call_status = TRUE;

    OCI_CALL2
    (
        call_status, msg->typinf->con,

        OCIRawAssignBytes(msg->typinf->con->env, msg->typinf->con->err,
                          (ub1*) id, (ub4) len, (OCIRaw **) &value)
    )

    OCI_CALL2
    (
        call_status, msg->typinf->con,

        OCIAttrSet((dvoid *) msg->proph,
                   (ub4    ) OCI_DTYPE_AQMSG_PROPERTIES,
                   (dvoid *) &value,
                   (ub4    ) 0,
                   (ub4    ) OCI_ATTR_ORIGINAL_MSGID,
                   msg->typinf->con->err)
    )

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_MsgGetCorrelation
 * --------------------------------------------------------------------------------------------- */

const otext * OCI_API OCI_MsgGetCorrelation
(
    OCI_Msg *msg
)
{
    OCI_LIB_CALL_ENTER(const otext *, NULL)

    OCI_CHECK_PTR(OCI_IPC_MSG, msg)

    call_status = TRUE;

    if (!msg->correlation)
    {
        call_status = OCI_GetStringAttribute(msg->typinf->con,
                                          msg->proph,
                                          OCI_DTYPE_AQMSG_PROPERTIES,
                                          OCI_ATTR_CORRELATION,
                                          &msg->correlation);
    }

    call_retval = msg->correlation;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_MsgSetCorrelation
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_MsgSetCorrelation
(
    OCI_Msg     *msg,
    const otext *correlation
)
{
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_MSG, msg)

    call_status =  OCI_SetStringAttribute(msg->typinf->con,
                                     msg->proph,
                                     OCI_DTYPE_AQMSG_PROPERTIES,
                                     OCI_ATTR_CORRELATION,
                                     &msg->correlation,
                                     correlation);

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_MsgGetExceptionQueue
 * --------------------------------------------------------------------------------------------- */

const otext * OCI_API OCI_MsgGetExceptionQueue
(
    OCI_Msg *msg
)
{
    OCI_LIB_CALL_ENTER(const otext *, NULL)

    OCI_CHECK_PTR(OCI_IPC_MSG, msg)

    call_status = TRUE;

    if (!msg->except_queue)
    {
        call_status = OCI_GetStringAttribute
                        (
                            msg->typinf->con,
                            msg->proph,
                            OCI_DTYPE_AQMSG_PROPERTIES,
                            OCI_ATTR_EXCEPTION_QUEUE,
                            &msg->except_queue
                        );
    }

    call_retval = msg->except_queue;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_MsgSetExceptionQueue
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_MsgSetExceptionQueue
(
    OCI_Msg     *msg,
    const otext *queue
)
{
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_MSG, msg)

    call_status =  OCI_SetStringAttribute(msg->typinf->con,
                                     msg->proph,
                                     OCI_DTYPE_AQMSG_PROPERTIES,
                                     OCI_ATTR_EXCEPTION_QUEUE,
                                     &msg->except_queue,
                                     queue);

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_MsgGetSender
 * --------------------------------------------------------------------------------------------- */

OCI_Agent * OCI_API OCI_MsgGetSender
(
    OCI_Msg   *msg
)
{
    OCIAQAgent *handle = NULL;

    OCI_LIB_CALL_ENTER(OCI_Agent *, NULL)

    OCI_CHECK_PTR(OCI_IPC_MSG, msg)

    call_status = TRUE;

    OCI_CALL2
    (
        call_status, msg->typinf->con,

        OCIAttrGet((dvoid *) msg->proph,
                   (ub4    ) OCI_DTYPE_AQMSG_PROPERTIES,
                   (dvoid *) &handle,
                   (ub4  * ) NULL,
                   (ub4    ) OCI_ATTR_SENDER_ID,
                   msg->typinf->con->err)
    )

    if (call_status && handle)
    {
        call_retval = OCI_AgentInit(msg->typinf->con, &msg->sender, handle, NULL, NULL);
        call_status = (NULL != call_retval);
    }

    OCI_LIB_CALL_EXIT()
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
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_MSG, msg)

    call_status = TRUE;

    OCI_CALL2
    (
        call_status, msg->typinf->con,

        OCIAttrSet((dvoid *) msg->proph,
                   (ub4    ) OCI_DTYPE_AQMSG_PROPERTIES,
                   (dvoid *) (sender ? sender->handle : NULL),
                   (ub4    ) 0,
                   (ub4    ) OCI_ATTR_SENDER_ID,
                   msg->typinf->con->err)
    )

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
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
    OCIAQAgent **handles = NULL;

    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_MSG, msg)

    /* allocate local array of OCIAQAgent handles if needed */

    if (consumers && (count > 0))
    {
        handles = (OCIAQAgent **)OCI_MemAlloc(OCI_IPC_ARRAY, sizeof(OCIAQAgent *), count, FALSE);

        if (handles)
        {
            unsigned int i;

            for(i = 0; i < count; i++)
            {
                handles[i] = consumers[i]->handle;
            }

            call_status = TRUE;
        }
    }
    else
    {
        call_status = TRUE;
        count = 0;
    }

    OCI_CALL2
    (
        call_status, msg->typinf->con,

        OCIAttrSet((dvoid *) msg->proph,
                   (ub4    ) OCI_DTYPE_AQMSG_PROPERTIES,
                   (dvoid *) handles,
                   (ub4    ) count,
                   (ub4    ) OCI_ATTR_RECIPIENT_LIST,
                   msg->typinf->con->err)
    )


    /* free local array of OCIAQAgent handles if needed */

    if (handles)
    {
        OCI_FREE(handles)
    }

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
}
