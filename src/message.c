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

#include "message.h"

#include "agent.h"
#include "date.h"
#include "macros.h"
#include "memory.h"
#include "object.h"
#include "strings.h"

/* --------------------------------------------------------------------------------------------- *
 * MessageCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Msg * MessageCreate
(
    OCI_TypeInfo *typinf
)
{
    OCI_Msg *msg = NULL;

    CALL_ENTER(OCI_Msg*, NULL)
    CALL_CHECK_PTR(OCI_IPC_TYPE_INFO, typinf)
    CALL_CONTEXT_FROM_CON(typinf->con)

    /* allocate message structure */

    ALLOC_DATA(OCI_IPC_MSG, msg, 1)

    if (STATUS)
    {
        msg->typinf = typinf;
        msg->ind    = OCI_IND_NULL;

        /* allocate message properties descriptor */

        STATUS = MemoryAllocDescriptor((dvoid *)msg->typinf->con->env, (dvoid **)&msg->proph, OCI_DTYPE_AQMSG_PROPERTIES);

        if (STATUS)
        {
            /* allocate internal OCI_Object handle if payload is not RAW */

            if (OCI_UNKNOWN != msg->typinf->typecode)
            {
                msg->obj = ObjectCreate(typinf->con, typinf);

                STATUS = (NULL != msg->obj);
            }
        }
    }

    /* check for failure */

    if (STATUS)
    {
        RETVAL = msg;
    }
    else if (msg)
    {
        MessageFree(msg);
    }

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * MessageFree
 * --------------------------------------------------------------------------------------------- */

boolean MessageFree
(
    OCI_Msg *msg
)
{
    CALL_ENTER(boolean, TRUE)
    CALL_CHECK_PTR(OCI_IPC_MSG, msg)
    CALL_CONTEXT_FROM_CON(msg->typinf->con)

    /* free local OCI_Agent object */

    if (msg->sender)
    {
        AgentFree(msg->sender);
    }

    /* free internal OCI_Object handle if payload is not RAW */

    if (msg->obj)
    {
        msg->obj->hstate =  OCI_OBJECT_ALLOCATED;

        ObjectFree(msg->obj);

        msg->obj = NULL;
    }

    /* free message RAW payload if necessary */

    if (msg->id && (OCI_UNKNOWN == msg->typinf->typecode))
    {
        EXEC(OCIRawResize(msg->typinf->con->env, msg->typinf->con->err, 0, (OCIRaw **) &msg->payload))
    }

    /* free message ID */

    if (msg->id)
    {

        EXEC(OCIRawResize(msg->typinf->con->env, msg->typinf->con->err, 0, (OCIRaw **) &msg->id))
    }

    msg->id = NULL;

    /* free OCI descriptor */
    
    MemoryFreeDescriptor((dvoid *) msg->proph, OCI_DTYPE_AQMSG_PROPERTIES);

    FREE(msg)

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * MessageReset
 * --------------------------------------------------------------------------------------------- */

boolean MessageReset
(
    OCI_Msg *msg
)
{
    const unsigned int  len = 0;

    boolean res = 
    (
        MessageSetExpiration(msg, -1)            &&
        MessageSetEnqueueDelay(msg, 0)           &&
        MessageSetPriority(msg,0)                &&
        MessageSetOriginalID(msg, NULL, len)     &&
        MessageSetSender(msg, NULL)              &&
        MessageSetConsumers(msg, NULL, len)      &&
        MessageSetCorrelation(msg, NULL)         &&
        MessageSetExceptionQueue(msg, NULL)
    );

    if (res)
    {
        if (OCI_UNKNOWN == msg->typinf->typecode)
        {
            res = MessageSetRaw(msg, NULL, len);
        }
        else
        {
            res = MessageSetObject(msg, NULL);
        }
    }

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * MessageGetObject
 * --------------------------------------------------------------------------------------------- */

OCI_Object * MessageGetObject
(
    OCI_Msg *msg
)
{
    CALL_ENTER(OCI_Object *, NULL)
    CALL_CHECK_PTR(OCI_IPC_MSG, msg)
    CALL_CHECK_COMPAT(msg->typinf->con, msg->typinf->typecode != OCI_UNKNOWN)
    CALL_CONTEXT_FROM_CON(msg->typinf->con)

    if (OCI_IND_NULL != msg->ind)
    {
        RETVAL = (OCI_Object *) msg->obj;
    }

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * MessageGetObject
 * --------------------------------------------------------------------------------------------- */

boolean MessageSetObject
(
    OCI_Msg    *msg,
    OCI_Object *obj
)
{
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_MSG, msg)
    CALL_CHECK_COMPAT(msg->typinf->con, msg->typinf->typecode != OCI_UNKNOWN)
    CALL_CONTEXT_FROM_CON(msg->typinf->con)

    if (obj)
    {
        /* assign the given object to the message internal object */

        STATUS = ObjectAssign((OCI_Object *) msg->obj, obj);

        if (STATUS)
        {
            msg->ind = OCI_IND_NOTNULL;
        }
    }
    else
    {
        STATUS = TRUE;
        msg->ind    = OCI_IND_NULL;
    }

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * MessageGetRaw
 * --------------------------------------------------------------------------------------------- */

boolean MessageGetRaw
(
    OCI_Msg      *msg,
    void         *raw,
    unsigned int *size
)
{
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_MSG, msg)
    CALL_CHECK_PTR(OCI_IPC_VOID, raw)
    CALL_CHECK_PTR(OCI_IPC_VOID, size)
    CALL_CHECK_COMPAT(msg->typinf->con, OCI_UNKNOWN == msg->typinf->typecode)
    CALL_CONTEXT_FROM_CON(msg->typinf->con)

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

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * MessageSetRaw
 * --------------------------------------------------------------------------------------------- */

boolean MessageSetRaw
(
    OCI_Msg      *msg,
    const void   *raw,
    unsigned int  size
)
{
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_MSG, msg)
    CALL_CONTEXT_FROM_CON(msg->typinf->con)

    EXEC(OCIRawAssignBytes(msg->typinf->con->env, msg->typinf->con->err, (ub1*) raw, (ub4) size, (OCIRaw **) &msg->payload))

    if (STATUS && msg->payload && (size > 0))
    {
        msg->ind = OCI_IND_NOTNULL;
    }
    else
    {
        msg->ind = OCI_IND_NULL;
    }

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * MessageGetAttemptCount
 * --------------------------------------------------------------------------------------------- */

int MessageGetAttemptCount
(
    OCI_Msg *msg
)
{
    sb4 value = 0;

    CALL_ENTER(int, value)
    CALL_CHECK_PTR(OCI_IPC_MSG, msg)
    CALL_CONTEXT_FROM_CON(msg->typinf->con)

    ATTRIB_GET(OCI_DTYPE_AQMSG_PROPERTIES, OCI_ATTR_ATTEMPTS, msg->proph, &value, NULL)

    RETVAL = value;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * MessageGetEnqueueDelay
 * --------------------------------------------------------------------------------------------- */

int MessageGetEnqueueDelay
(
    OCI_Msg *msg
)
{
    sb4 value = 0;

    CALL_ENTER(int, 0)
    CALL_CHECK_PTR(OCI_IPC_MSG, msg)
    CALL_CONTEXT_FROM_CON(msg->typinf->con)

    ATTRIB_GET(OCI_DTYPE_AQMSG_PROPERTIES, OCI_ATTR_DELAY, msg->proph, &value, NULL)

    RETVAL = value;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * MessageSetEnqueueDelay
 * --------------------------------------------------------------------------------------------- */

boolean MessageSetEnqueueDelay
(
    OCI_Msg *msg,
    int      value
)
{
    sb4 sval = (sb4) value;

    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_MSG, msg)
    CALL_CONTEXT_FROM_CON(msg->typinf->con)

    ATTRIB_SET(OCI_DTYPE_AQMSG_PROPERTIES, OCI_ATTR_DELAY, msg->proph, &sval, sizeof(sval))

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * MessageGetEnqueueTime
 * --------------------------------------------------------------------------------------------- */

OCI_Date * MessageGetEnqueueTime
(
    OCI_Msg *msg
)
{
    OCIDate date;

    CALL_ENTER(OCI_Date*, NULL)
    CALL_CHECK_PTR(OCI_IPC_MSG, msg)
    CALL_CONTEXT_FROM_CON(msg->typinf->con)

    ATTRIB_GET(OCI_DTYPE_AQMSG_PROPERTIES, OCI_ATTR_ENQ_TIME, msg->proph, &date, NULL)

    if (STATUS)
    {
        RETVAL = msg->date = DateInitialize(msg->typinf->con, msg->date, &date, FALSE, FALSE);
        STATUS = (NULL != RETVAL);
    }

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * MessageGetExpiration
 * --------------------------------------------------------------------------------------------- */

int MessageGetExpiration
(
    OCI_Msg *msg
)
{
    sb4 value = 0;

    CALL_ENTER(int, 0)
    CALL_CHECK_PTR(OCI_IPC_MSG, msg)
    CALL_CONTEXT_FROM_CON(msg->typinf->con)

    ATTRIB_GET(OCI_DTYPE_AQMSG_PROPERTIES, OCI_ATTR_EXPIRATION, msg->proph, &value, NULL)

    RETVAL = value;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * MessageSetExpiration
 * --------------------------------------------------------------------------------------------- */

boolean MessageSetExpiration
(
    OCI_Msg *msg,
    int      value
)
{
    sb4 sval = (sb4) value;

    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_MSG, msg)
    CALL_CONTEXT_FROM_CON(msg->typinf->con)

    ATTRIB_SET(OCI_DTYPE_AQMSG_PROPERTIES, OCI_ATTR_EXPIRATION, msg->proph, &sval, sizeof(sval))

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * MessageGetState
 * --------------------------------------------------------------------------------------------- */

unsigned int MessageGetState
(
    OCI_Msg *msg
)
{
    sb4 value = 0;

    CALL_ENTER(unsigned int, OCI_UNKNOWN)
    CALL_CHECK_PTR(OCI_IPC_MSG, msg)
    CALL_CONTEXT_FROM_CON(msg->typinf->con)

    ATTRIB_GET(OCI_DTYPE_AQMSG_PROPERTIES, OCI_ATTR_MSG_STATE, msg->proph, &value, NULL)

    /* increment value to handle return code OCI_UNKNOWN on failure */

    if (STATUS)
    {
        RETVAL = ++value;
    }

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * MessageGetPriority
 * --------------------------------------------------------------------------------------------- */

int MessageGetPriority
(
    OCI_Msg *msg
)
{
    sb4 value = 0;

    CALL_ENTER(int, FALSE)
    CALL_CHECK_PTR(OCI_IPC_MSG, msg)
    CALL_CONTEXT_FROM_CON(msg->typinf->con)

    ATTRIB_GET(OCI_DTYPE_AQMSG_PROPERTIES, OCI_ATTR_PRIORITY, msg->proph, &value, NULL)

    RETVAL = value;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * MessageSetPriority
 * --------------------------------------------------------------------------------------------- */

boolean MessageSetPriority
(
    OCI_Msg *msg,
    int      value
)
{
    sb4 sval = (sb4) value;

    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_MSG, msg)
    CALL_CONTEXT_FROM_CON(msg->typinf->con)

    ATTRIB_SET(OCI_DTYPE_AQMSG_PROPERTIES, OCI_ATTR_PRIORITY, msg->proph, &sval, sizeof(sval))

    RETVAL = STATUS;

    CALL_EXIT()
}


/* --------------------------------------------------------------------------------------------- *
 * MessageGetID
 * --------------------------------------------------------------------------------------------- */

boolean MessageGetID
(
    OCI_Msg      *msg,
    void         *id,
    unsigned int *len
)
{
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_MSG, msg)
    CALL_CHECK_PTR(OCI_IPC_VOID, id)
    CALL_CHECK_PTR(OCI_IPC_VOID, len)
    CALL_CONTEXT_FROM_CON(msg->typinf->con)

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

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * MessageGetOriginalID
 * --------------------------------------------------------------------------------------------- */

boolean MessageGetOriginalID
(
    OCI_Msg      *msg,
    void         *id,
    unsigned int *len
)
{
    OCIRaw *value = NULL;

    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_MSG, msg)
    CALL_CHECK_PTR(OCI_IPC_VOID, id)
    CALL_CHECK_PTR(OCI_IPC_VOID, len)
    CALL_CONTEXT_FROM_CON(msg->typinf->con)

    *len = 0;

    ATTRIB_GET(OCI_DTYPE_AQMSG_PROPERTIES, OCI_ATTR_ORIGINAL_MSGID, msg->proph, &value, NULL)

    if (STATUS && value)
    {
        const ub4 raw_len = OCIRawSize(msg->typinf->con->env, value);

        if (*len > raw_len)
        {
            *len = raw_len;
        }

        memcpy(id, OCIRawPtr(msg->typinf->con->env, value), (size_t) (*len));
    }

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * MessageSetOriginalID
 * --------------------------------------------------------------------------------------------- */

boolean MessageSetOriginalID
(
    OCI_Msg      *msg,
    const void   *id,
    unsigned int len
)
{
    OCIRaw *value = NULL;

    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_MSG, msg)
    CALL_CONTEXT_FROM_CON(msg->typinf->con)

    EXEC(OCIRawAssignBytes(msg->typinf->con->env, msg->typinf->con->err, (ub1*) id, (ub4) len, (OCIRaw **) &value))
    ATTRIB_SET(OCI_DTYPE_AQMSG_PROPERTIES, OCI_ATTR_ORIGINAL_MSGID, msg->proph, &value, 0)
 
    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * MessageGetCorrelation
 * --------------------------------------------------------------------------------------------- */

const otext * MessageGetCorrelation
(
    OCI_Msg *msg
)
{
    CALL_ENTER(const otext *, NULL)
    CALL_CHECK_PTR(OCI_IPC_MSG, msg)
    CALL_CONTEXT_FROM_CON(msg->typinf->con)

    if (!msg->correlation)
    {
        unsigned int size = 0;
        
        STATUS = StringGetAttribute(msg->typinf->con,
                                        msg->proph,
                                        OCI_DTYPE_AQMSG_PROPERTIES,
                                        OCI_ATTR_CORRELATION,
                                        &msg->correlation,
                                        &size);
    }

    RETVAL = msg->correlation;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * MessageSetCorrelation
 * --------------------------------------------------------------------------------------------- */

boolean MessageSetCorrelation
(
    OCI_Msg     *msg,
    const otext *correlation
)
{
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_MSG, msg)
    CALL_CONTEXT_FROM_CON(msg->typinf->con)

    STATUS = StringSetAttribute(msg->typinf->con,
                                    msg->proph,
                                    OCI_DTYPE_AQMSG_PROPERTIES,
                                    OCI_ATTR_CORRELATION,
                                    &msg->correlation,
                                    correlation);

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * MessageGetExceptionQueue
 * --------------------------------------------------------------------------------------------- */

const otext * MessageGetExceptionQueue
(
    OCI_Msg *msg
)
{
    CALL_ENTER(const otext *, NULL)
    CALL_CHECK_PTR(OCI_IPC_MSG, msg)
    CALL_CONTEXT_FROM_CON(msg->typinf->con)

    if (!msg->except_queue)
    {
        unsigned int size = 0;
        
        STATUS = StringGetAttribute
                        (
                            msg->typinf->con,
                            msg->proph,
                            OCI_DTYPE_AQMSG_PROPERTIES,
                            OCI_ATTR_EXCEPTION_QUEUE,
                            &msg->except_queue,
                            &size
                        );
    }

    RETVAL = msg->except_queue;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * MessageSetExceptionQueue
 * --------------------------------------------------------------------------------------------- */

boolean MessageSetExceptionQueue
(
    OCI_Msg     *msg,
    const otext *queue
)
{
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_MSG, msg)
    CALL_CONTEXT_FROM_CON(msg->typinf->con)

    STATUS = StringSetAttribute(msg->typinf->con,
                                    msg->proph,
                                    OCI_DTYPE_AQMSG_PROPERTIES,
                                    OCI_ATTR_EXCEPTION_QUEUE,
                                    &msg->except_queue,
                                    queue);

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * MessageGetSender
 * --------------------------------------------------------------------------------------------- */

OCI_Agent * MessageGetSender
(
    OCI_Msg   *msg
)
{
    OCIAQAgent *handle = NULL;

    CALL_ENTER(OCI_Agent *, NULL)
    CALL_CHECK_PTR(OCI_IPC_MSG, msg)
    CALL_CONTEXT_FROM_CON(msg->typinf->con)

    ATTRIB_GET(OCI_DTYPE_AQMSG_PROPERTIES, OCI_ATTR_SENDER_ID, msg->proph, &handle, NULL)

    if (STATUS && handle)
    {
        RETVAL = msg->sender = AgentInitialize(msg->typinf->con, msg->sender, handle, NULL, NULL);
        STATUS = (NULL != RETVAL);
    }

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * MessageSetSender
 * --------------------------------------------------------------------------------------------- */

boolean MessageSetSender
(
    OCI_Msg   *msg,
    OCI_Agent *sender
)
{
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_MSG, msg)
    CALL_CONTEXT_FROM_CON(msg->typinf->con)

    ATTRIB_SET(OCI_DTYPE_AQMSG_PROPERTIES, OCI_ATTR_SENDER_ID, msg->proph, (sender ? sender->handle : NULL), 0)

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * MessageSetConsumers
 * --------------------------------------------------------------------------------------------- */

boolean MessageSetConsumers
(
    OCI_Msg     *msg,
    OCI_Agent  **consumers,
    unsigned int count
)
{
    OCIAQAgent **handles = NULL;

    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_MSG, msg)
    CALL_CONTEXT_FROM_CON(msg->typinf->con)

    /* allocate local array of OCIAQAgent handles if needed */

    if (consumers && (count > 0))
    {
        ALLOC_DATA(OCI_IPC_ARRAY, handles, count)

        if (STATUS)
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

    ATTRIB_SET(OCI_DTYPE_AQMSG_PROPERTIES, OCI_ATTR_RECIPIENT_LIST, msg->proph, handles, count)

    /* free local array of OCIAQAgent handles if needed */

    if (handles)
    {
        FREE(handles)
    }

    RETVAL = STATUS;

    CALL_EXIT()
}
