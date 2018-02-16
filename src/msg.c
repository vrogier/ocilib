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

    OCI_CALL_ENTER(OCI_Msg*, NULL)
    OCI_CALL_CHECK_PTR(OCI_IPC_TYPE_INFO, typinf)
    OCI_CALL_CONTEXT_SET_FROM_CONN(typinf->con)

    /* allocate message structure */

    OCI_ALLOCATE_DATA(OCI_IPC_MSG, msg, 1)

    if (OCI_STATUS)
    {
        msg->typinf = typinf;
        msg->ind    = OCI_IND_NULL;

        /* allocate message properties descriptor */

        OCI_STATUS = OCI_DescriptorAlloc((dvoid *)msg->typinf->con->env, (dvoid **)&msg->proph, OCI_DTYPE_AQMSG_PROPERTIES);

        if (OCI_STATUS)
        {
            /* allocate internal OCI_Object handle if payload is not RAW */

            if (OCI_UNKNOWN != msg->typinf->typecode)
            {
                msg->obj = OCI_ObjectCreate(typinf->con, typinf);

                OCI_STATUS = (NULL != msg->obj);
            }
        }
    }

    /* check for failure */

    if (OCI_STATUS)
    {
        OCI_RETVAL = msg;
    }
    else if (msg)
    {
        OCI_MsgFree(msg);
    }

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_MsgFree
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_MsgFree
(
    OCI_Msg *msg
)
{
    OCI_CALL_ENTER(boolean, TRUE)
    OCI_CALL_CHECK_PTR(OCI_IPC_MSG, msg)
    OCI_CALL_CONTEXT_SET_FROM_CONN(msg->typinf->con)

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
        OCI_EXEC(OCIRawResize(msg->typinf->con->env, msg->typinf->con->err, 0, (OCIRaw **) &msg->payload))
    }

    /* free message ID */

    if (msg->id)
    {

        OCI_EXEC(OCIRawResize(msg->typinf->con->env, msg->typinf->con->err, 0, (OCIRaw **) &msg->id))
    }

    msg->id = NULL;

    /* free OCI descriptor */

    OCI_DescriptorFree((dvoid *) msg->proph, OCI_DTYPE_AQMSG_PROPERTIES);

    OCI_FREE(msg)

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_MsgReset
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_MsgReset
(
    OCI_Msg *msg
)
{
    const unsigned int  len = 0;

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
    OCI_CALL_ENTER(OCI_Object *, NULL)
    OCI_CALL_CHECK_PTR(OCI_IPC_MSG, msg)
    OCI_CALL_CHECK_COMPAT(msg->typinf->con, msg->typinf->typecode != OCI_UNKNOWN)
    OCI_CALL_CONTEXT_SET_FROM_CONN(msg->typinf->con)

    if (OCI_IND_NULL != msg->ind)
    {
        OCI_RETVAL = (OCI_Object *) msg->obj;
    }

    OCI_CALL_EXIT()
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
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_MSG, msg)
    OCI_CALL_CHECK_COMPAT(msg->typinf->con, msg->typinf->typecode != OCI_UNKNOWN)
    OCI_CALL_CONTEXT_SET_FROM_CONN(msg->typinf->con)

    if (obj)
    {
        /* assign the given object to the message internal object */

        OCI_STATUS = OCI_ObjectAssign((OCI_Object *) msg->obj, obj);

        if (OCI_STATUS)
        {
            msg->ind = OCI_IND_NOTNULL;
        }
    }
    else
    {
        OCI_STATUS = TRUE;
        msg->ind    = OCI_IND_NULL;
    }

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
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
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_MSG, msg)
    OCI_CALL_CHECK_PTR(OCI_IPC_VOID, raw)
    OCI_CALL_CHECK_PTR(OCI_IPC_VOID, size)
    OCI_CALL_CHECK_COMPAT(msg->typinf->con, OCI_UNKNOWN == msg->typinf->typecode)
    OCI_CALL_CONTEXT_SET_FROM_CONN(msg->typinf->con)

    if ((msg->payload) && (OCI_IND_NULL != msg->ind))
    {
        const unsigned int raw_size = OCIRawSize(msg->typinf->con->env, (OCIRaw *) msg->payload);

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

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
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
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_MSG, msg)
    OCI_CALL_CONTEXT_SET_FROM_CONN(msg->typinf->con)

    OCI_EXEC(OCIRawAssignBytes(msg->typinf->con->env, msg->typinf->con->err, (ub1*) raw, (ub4) size, (OCIRaw **) &msg->payload))

    if (OCI_STATUS && msg->payload && (size > 0))
    {
        msg->ind = OCI_IND_NOTNULL;
    }
    else
    {
        msg->ind = OCI_IND_NULL;
    }

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
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

    OCI_CALL_ENTER(int, value)
    OCI_CALL_CHECK_PTR(OCI_IPC_MSG, msg)
    OCI_CALL_CONTEXT_SET_FROM_CONN(msg->typinf->con)

    OCI_GET_ATTRIB(OCI_DTYPE_AQMSG_PROPERTIES, OCI_ATTR_ATTEMPTS, msg->proph, &value, NULL)

    OCI_RETVAL = value;

    OCI_CALL_EXIT()
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

    OCI_CALL_ENTER(int, 0)
    OCI_CALL_CHECK_PTR(OCI_IPC_MSG, msg)
    OCI_CALL_CONTEXT_SET_FROM_CONN(msg->typinf->con)

    OCI_GET_ATTRIB(OCI_DTYPE_AQMSG_PROPERTIES, OCI_ATTR_DELAY, msg->proph, &value, NULL)

    OCI_RETVAL = value;

    OCI_CALL_EXIT()
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

    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_MSG, msg)
    OCI_CALL_CONTEXT_SET_FROM_CONN(msg->typinf->con)

    OCI_SET_ATTRIB(OCI_DTYPE_AQMSG_PROPERTIES, OCI_ATTR_DELAY, msg->proph, &sval, sizeof(sval))

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
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

    OCI_CALL_ENTER(OCI_Date*, NULL)
    OCI_CALL_CHECK_PTR(OCI_IPC_MSG, msg)
    OCI_CALL_CONTEXT_SET_FROM_CONN(msg->typinf->con)

    OCI_GET_ATTRIB(OCI_DTYPE_AQMSG_PROPERTIES, OCI_ATTR_ENQ_TIME, msg->proph, &date, NULL)

    if (OCI_STATUS)
    {
        OCI_RETVAL = msg->date = OCI_DateInit(msg->typinf->con, msg->date, &date, FALSE, FALSE);
        OCI_STATUS = (NULL != OCI_RETVAL);
    }

    OCI_CALL_EXIT()
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

    OCI_CALL_ENTER(int, 0)
    OCI_CALL_CHECK_PTR(OCI_IPC_MSG, msg)
    OCI_CALL_CONTEXT_SET_FROM_CONN(msg->typinf->con)

    OCI_GET_ATTRIB(OCI_DTYPE_AQMSG_PROPERTIES, OCI_ATTR_EXPIRATION, msg->proph, &value, NULL)

    OCI_RETVAL = value;

    OCI_CALL_EXIT()
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

    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_MSG, msg)
    OCI_CALL_CONTEXT_SET_FROM_CONN(msg->typinf->con)

    OCI_SET_ATTRIB(OCI_DTYPE_AQMSG_PROPERTIES, OCI_ATTR_EXPIRATION, msg->proph, &sval, sizeof(sval))

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
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

    OCI_CALL_ENTER(unsigned int, OCI_UNKNOWN)
    OCI_CALL_CHECK_PTR(OCI_IPC_MSG, msg)
    OCI_CALL_CONTEXT_SET_FROM_CONN(msg->typinf->con)

    OCI_GET_ATTRIB(OCI_DTYPE_AQMSG_PROPERTIES, OCI_ATTR_MSG_STATE, msg->proph, &value, NULL)

    /* increment value to handle return code OCI_UNKNOWN on failure */

    if (OCI_STATUS)
    {
        OCI_RETVAL = ++value;
    }

    OCI_CALL_EXIT()
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

    OCI_CALL_ENTER(int, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_MSG, msg)
    OCI_CALL_CONTEXT_SET_FROM_CONN(msg->typinf->con)

    OCI_GET_ATTRIB(OCI_DTYPE_AQMSG_PROPERTIES, OCI_ATTR_PRIORITY, msg->proph, &value, NULL)

    OCI_RETVAL = value;

    OCI_CALL_EXIT()
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

    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_MSG, msg)
    OCI_CALL_CONTEXT_SET_FROM_CONN(msg->typinf->con)

    OCI_SET_ATTRIB(OCI_DTYPE_AQMSG_PROPERTIES, OCI_ATTR_PRIORITY, msg->proph, &sval, sizeof(sval))

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
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
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_MSG, msg)
    OCI_CALL_CHECK_PTR(OCI_IPC_VOID, id)
    OCI_CALL_CHECK_PTR(OCI_IPC_VOID, len)
    OCI_CALL_CONTEXT_SET_FROM_CONN(msg->typinf->con)

    if (msg->id)
    {
        const ub4 raw_len = OCIRawSize(msg->typinf->con->env, msg->id);

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

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
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

    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_MSG, msg)
    OCI_CALL_CHECK_PTR(OCI_IPC_VOID, id)
    OCI_CALL_CHECK_PTR(OCI_IPC_VOID, len)
    OCI_CALL_CONTEXT_SET_FROM_CONN(msg->typinf->con)

    *len = 0;

    OCI_GET_ATTRIB(OCI_DTYPE_AQMSG_PROPERTIES, OCI_ATTR_ORIGINAL_MSGID, msg->proph, &value, NULL)

    if (OCI_STATUS && value)
    {
        const ub4 raw_len = OCIRawSize(msg->typinf->con->env, value);

        if (*len > raw_len)
        {
            *len = raw_len;
        }

        memcpy(id, OCIRawPtr(msg->typinf->con->env, value), (size_t) (*len));
    }

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
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

    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_MSG, msg)
    OCI_CALL_CONTEXT_SET_FROM_CONN(msg->typinf->con)

    OCI_EXEC(OCIRawAssignBytes(msg->typinf->con->env, msg->typinf->con->err, (ub1*) id, (ub4) len, (OCIRaw **) &value))
    OCI_SET_ATTRIB(OCI_DTYPE_AQMSG_PROPERTIES, OCI_ATTR_ORIGINAL_MSGID, msg->proph, &value, 0)
 
    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_MsgGetCorrelation
 * --------------------------------------------------------------------------------------------- */

const otext * OCI_API OCI_MsgGetCorrelation
(
    OCI_Msg *msg
)
{
    OCI_CALL_ENTER(const otext *, NULL)
    OCI_CALL_CHECK_PTR(OCI_IPC_MSG, msg)
    OCI_CALL_CONTEXT_SET_FROM_CONN(msg->typinf->con)

    if (!msg->correlation)
    {
        unsigned int size = 0;
        
        OCI_STATUS = OCI_GetStringAttribute(msg->typinf->con,
                                            msg->proph,
                                            OCI_DTYPE_AQMSG_PROPERTIES,
                                            OCI_ATTR_CORRELATION,
                                            &msg->correlation,
                                            &size);
    }

    OCI_RETVAL = msg->correlation;

    OCI_CALL_EXIT()
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
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_MSG, msg)
    OCI_CALL_CONTEXT_SET_FROM_CONN(msg->typinf->con)

    OCI_STATUS =  OCI_SetStringAttribute(msg->typinf->con,
                                         msg->proph,
                                         OCI_DTYPE_AQMSG_PROPERTIES,
                                         OCI_ATTR_CORRELATION,
                                         &msg->correlation,
                                         correlation);

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_MsgGetExceptionQueue
 * --------------------------------------------------------------------------------------------- */

const otext * OCI_API OCI_MsgGetExceptionQueue
(
    OCI_Msg *msg
)
{
    OCI_CALL_ENTER(const otext *, NULL)
    OCI_CALL_CHECK_PTR(OCI_IPC_MSG, msg)
    OCI_CALL_CONTEXT_SET_FROM_CONN(msg->typinf->con)

    if (!msg->except_queue)
    {
        unsigned int size = 0;
        
        OCI_STATUS = OCI_GetStringAttribute
                        (
                            msg->typinf->con,
                            msg->proph,
                            OCI_DTYPE_AQMSG_PROPERTIES,
                            OCI_ATTR_EXCEPTION_QUEUE,
                            &msg->except_queue,
                            &size
                        );
    }

    OCI_RETVAL = msg->except_queue;

    OCI_CALL_EXIT()
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
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_MSG, msg)
    OCI_CALL_CONTEXT_SET_FROM_CONN(msg->typinf->con)

    OCI_STATUS =  OCI_SetStringAttribute(msg->typinf->con,
                                         msg->proph,
                                         OCI_DTYPE_AQMSG_PROPERTIES,
                                         OCI_ATTR_EXCEPTION_QUEUE,
                                         &msg->except_queue,
                                         queue);

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
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

    OCI_CALL_ENTER(OCI_Agent *, NULL)
    OCI_CALL_CHECK_PTR(OCI_IPC_MSG, msg)
    OCI_CALL_CONTEXT_SET_FROM_CONN(msg->typinf->con)

    OCI_GET_ATTRIB(OCI_DTYPE_AQMSG_PROPERTIES, OCI_ATTR_SENDER_ID, msg->proph, &handle, NULL)

    if (OCI_STATUS && handle)
    {
        OCI_RETVAL = msg->sender = OCI_AgentInit(msg->typinf->con, msg->sender, handle, NULL, NULL);
        OCI_STATUS = (NULL != OCI_RETVAL);
    }

    OCI_CALL_EXIT()
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
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_MSG, msg)
    OCI_CALL_CONTEXT_SET_FROM_CONN(msg->typinf->con)

    OCI_SET_ATTRIB(OCI_DTYPE_AQMSG_PROPERTIES, OCI_ATTR_SENDER_ID, msg->proph, (sender ? sender->handle : NULL), 0)

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
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

    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_MSG, msg)
    OCI_CALL_CONTEXT_SET_FROM_CONN(msg->typinf->con)

    /* allocate local array of OCIAQAgent handles if needed */

    if (consumers && (count > 0))
    {
        OCI_ALLOCATE_DATA(OCI_IPC_ARRAY, handles, count)

        if (OCI_STATUS)
        {
            for(unsigned int i = 0; i < count; i++)
            {
                handles[i] = consumers[i]->handle;
            }
        }
    }
    else
    {
        count = 0;
    }

    OCI_SET_ATTRIB(OCI_DTYPE_AQMSG_PROPERTIES, OCI_ATTR_RECIPIENT_LIST, msg->proph, handles, count)

    /* free local array of OCIAQAgent handles if needed */

    if (handles)
    {
        OCI_FREE(handles)
    }

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}
