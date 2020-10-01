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
#include "stringutils.h"

/* --------------------------------------------------------------------------------------------- *
 * MessageCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Msg * MessageCreate
(
    OCI_TypeInfo *typinf
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Msg*, NULL,
        /* context */ OCI_IPC_TYPE_INFO, typinf
    )

    OCI_Msg* msg = NULL;

    CHECK_PTR(OCI_IPC_TYPE_INFO, typinf)

    /* allocate message structure */

    ALLOC_DATA(OCI_IPC_MSG, msg, 1)

    msg->typinf = typinf;
    msg->ind    = OCI_IND_NULL;

    /* allocate message properties descriptor */

    CHECK
    (
        MemoryAllocDescriptor
        (
            (dvoid *)msg->typinf->con->env,
            (dvoid **)&msg->proph,
            OCI_DTYPE_AQMSG_PROPERTIES
        )
    )

    /* allocate internal OCI_Object handle if payload is not RAW */

    if (OCI_UNKNOWN != msg->typinf->typecode)
    {
        msg->obj = ObjectCreate(typinf->con, typinf);
        CHECK_NULL(msg->obj)
    }

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE)
        {
            MessageFree(msg);
            msg = NULL;
        }

        SET_RETVAL(msg)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * MessageFree
 * --------------------------------------------------------------------------------------------- */

boolean MessageFree
(
    OCI_Msg *msg
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_MSG, msg
    )

    CHECK_PTR(OCI_IPC_MSG, msg)

    /* free local OCI_Agent object */

    if (NULL != msg->sender)
    {
        OcilibAgentFree(msg->sender);
    }

    /* free internal OCI_Object handle if payload is not RAW */

    if (NULL != msg->obj)
    {
        msg->obj->hstate =  OCI_OBJECT_ALLOCATED;

        ObjectFree(msg->obj);

        msg->obj = NULL;
    }

    /* free message RAW payload if necessary */

    if (NULL != msg->id && (OCI_UNKNOWN == msg->typinf->typecode))
    {
        CHECK_OCI
        (
            msg->typinf->con->err,
            OCIRawResize,
            msg->typinf->con->env, msg->typinf->con->err, 0,
            (OCIRaw **) &msg->payload
        )
    }

    /* free message ID */

    if (NULL != msg->id)
    {
        CHECK_OCI
        (
            msg->typinf->con->err,
            OCIRawResize,
            msg->typinf->con->env, msg->typinf->con->err,
            0, (OCIRaw **) &msg->id
        )
    }

    msg->id = NULL;

    /* free OCI descriptor */

    MemoryFreeDescriptor((dvoid*)msg->proph, OCI_DTYPE_AQMSG_PROPERTIES);

    ErrorResetSource(NULL, msg);

    FREE(msg)

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * MessageReset
 * --------------------------------------------------------------------------------------------- */

boolean MessageReset
(
    OCI_Msg *msg
)
{
    const unsigned int len = 0;

    boolean res =
        (
            MessageSetExpiration(msg, -1)       &&
            MessageSetEnqueueDelay(msg, 0)      &&
            MessageSetPriority(msg,0)           &&
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
    ENTER_FUNC
    (
        /* returns */ OCI_Object*, NULL,
        /* context */ OCI_IPC_MSG, msg
    )

    CHECK_PTR(OCI_IPC_MSG, msg)
    CHECK_COMPAT(msg->typinf->typecode != OCI_UNKNOWN)

    CHECK(OCI_IND_NULL != msg->ind)

    SET_RETVAL((OCI_Object *) msg->obj)

    EXIT_FUNC()
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
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_MSG, msg
    )

    CHECK_PTR(OCI_IPC_MSG, msg)
    CHECK_COMPAT(msg->typinf->typecode != OCI_UNKNOWN)

    if (NULL != obj)
    {
        /* assign the given object to the message internal object */

        CHECK(ObjectAssign((OCI_Object *) msg->obj, obj))

        msg->ind = OCI_IND_NOTNULL;
    }
    else
    {
        msg->ind = OCI_IND_NULL;
    }

    SET_SUCCESS()

    EXIT_FUNC()
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
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_MSG, msg
    )

    CHECK_PTR(OCI_IPC_MSG,  msg)
    CHECK_PTR(OCI_IPC_VOID, raw)
    CHECK_PTR(OCI_IPC_VOID, size)
    CHECK_COMPAT(OCI_UNKNOWN == msg->typinf->typecode)

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

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * MessageSetRaw
 * --------------------------------------------------------------------------------------------- */

boolean MessageSetRaw
(
    OCI_Msg     *msg,
    const void  *raw,
    unsigned int size
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_MSG, msg
    )

    CHECK_PTR(OCI_IPC_MSG, msg)

    CHECK_OCI
    (
        msg->typinf->con->err,
        OCIRawAssignBytes,
        msg->typinf->con->env, msg->typinf->con->err,
        (ub1*) raw, (ub4) size, (OCIRaw **) &msg->payload
    )

    if (NULL != msg->payload && (size > 0))
    {
        msg->ind = OCI_IND_NOTNULL;
    }
    else
    {
        msg->ind = OCI_IND_NULL;
    }

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * MessageGetAttemptCount
 * --------------------------------------------------------------------------------------------- */

int MessageGetAttemptCount
(
    OCI_Msg *msg
)
{
    ENTER_FUNC
    (
        /* returns */ int, 0,
        /* context */ OCI_IPC_MSG, msg
    )

    sb4 value = 0;

    CHECK_PTR(OCI_IPC_MSG, msg)

    CHECK_ATTRIB_GET
    (
        OCI_DTYPE_AQMSG_PROPERTIES, OCI_ATTR_ATTEMPTS,
        msg->proph, &value, NULL,
        msg->typinf->con->err
    )

    SET_RETVAL(value)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * MessageGetEnqueueDelay
 * --------------------------------------------------------------------------------------------- */

int MessageGetEnqueueDelay
(
    OCI_Msg *msg
)
{
    ENTER_FUNC
    (
        /* returns */ int, 0,
        /* context */ OCI_IPC_MSG, msg
    )

    sb4 value = 0;

    CHECK_PTR(OCI_IPC_MSG, msg)

    CHECK_ATTRIB_GET
    (
        OCI_DTYPE_AQMSG_PROPERTIES, OCI_ATTR_DELAY,
        msg->proph, &value, NULL,
        msg->typinf->con->err
    )

    SET_RETVAL(value)

    EXIT_FUNC()
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
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_MSG, msg
    )

    sb4 sval = (sb4) value;

    CHECK_PTR(OCI_IPC_MSG, msg)

    CHECK_ATTRIB_SET
    (
        OCI_DTYPE_AQMSG_PROPERTIES, OCI_ATTR_DELAY,
        msg->proph, &sval, sizeof(sval),
        msg->typinf->con->err
    )

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * MessageGetEnqueueTime
 * --------------------------------------------------------------------------------------------- */

OCI_Date * MessageGetEnqueueTime
(
    OCI_Msg *msg
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Date*, NULL,
        /* context */ OCI_IPC_MSG, msg
    )

    OCIDate date;

    CHECK_PTR(OCI_IPC_MSG, msg)

    CHECK_ATTRIB_GET
    (
        OCI_DTYPE_AQMSG_PROPERTIES, OCI_ATTR_ENQ_TIME,
        msg->proph, &date, NULL,
        msg->typinf->con->err
    )

    msg->date = DateInitialize(msg->typinf->con, msg->date, &date, FALSE, FALSE);
    CHECK_NULL(msg->date)

    SET_RETVAL(msg->date)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * MessageGetExpiration
 * --------------------------------------------------------------------------------------------- */

int MessageGetExpiration
(
    OCI_Msg *msg
)
{
    ENTER_FUNC
    (
        /* returns */ int, 0,
        /* context */ OCI_IPC_MSG, msg
    )

    sb4 value = 0;

    CHECK_PTR(OCI_IPC_MSG, msg)

    CHECK_ATTRIB_GET
    (
        OCI_DTYPE_AQMSG_PROPERTIES, OCI_ATTR_EXPIRATION,
        msg->proph, &value, NULL,
        msg->typinf->con->err
    )

    SET_RETVAL(value)

    EXIT_FUNC()
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
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_MSG, msg
    )

    sb4 sval = (sb4) value;

    CHECK_PTR(OCI_IPC_MSG, msg)

    CHECK_ATTRIB_SET
    (
        OCI_DTYPE_AQMSG_PROPERTIES, OCI_ATTR_EXPIRATION,
        msg->proph, &sval, sizeof(sval),
        msg->typinf->con->err
    )

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * MessageGetState
 * --------------------------------------------------------------------------------------------- */

unsigned int MessageGetState
(
    OCI_Msg *msg
)
{
    ENTER_FUNC
    (
        /* returns */ unsigned int, OCI_UNKNOWN,
        /* context */ OCI_IPC_MSG, msg
    )

    sb4 value = 0;

    CHECK_PTR(OCI_IPC_MSG, msg)

    CHECK_ATTRIB_GET
    (
        OCI_DTYPE_AQMSG_PROPERTIES, OCI_ATTR_MSG_STATE,
        msg->proph, &value, NULL,
        msg->typinf->con->err
    )

    /* increment value to handle return code OCI_UNKNOWN on failure */

    SET_RETVAL(++value)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * MessageGetPriority
 * --------------------------------------------------------------------------------------------- */

int MessageGetPriority
(
    OCI_Msg *msg
)
{
    ENTER_FUNC
    (
        /* returns */ int, 0,
        /* context */ OCI_IPC_MSG, msg
    )

    sb4 value = 0;

    CHECK_PTR(OCI_IPC_MSG, msg)

    CHECK_ATTRIB_GET
    (
        OCI_DTYPE_AQMSG_PROPERTIES, OCI_ATTR_PRIORITY,
        msg->proph, &value, NULL,
        msg->typinf->con->err
    )

    SET_RETVAL(value)

    EXIT_FUNC()
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
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_MSG, msg
    )

    sb4 sval = (sb4) value;

    CHECK_PTR(OCI_IPC_MSG, msg)

    CHECK_ATTRIB_SET
    (
        OCI_DTYPE_AQMSG_PROPERTIES, OCI_ATTR_PRIORITY,
        msg->proph, &sval, sizeof(sval),
        msg->typinf->con->err
    )

    SET_SUCCESS()

    EXIT_FUNC()
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
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_MSG, msg
    )

    CHECK_PTR(OCI_IPC_MSG,  msg)
    CHECK_PTR(OCI_IPC_VOID, id)
    CHECK_PTR(OCI_IPC_VOID, len)

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

    SET_SUCCESS()

    EXIT_FUNC()
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
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_MSG, msg
    )

    OCIRaw *value = NULL;

    CHECK_PTR(OCI_IPC_MSG,  msg)
    CHECK_PTR(OCI_IPC_VOID, id)
    CHECK_PTR(OCI_IPC_VOID, len)

    *len = 0;

    CHECK_ATTRIB_GET
    (
        OCI_DTYPE_AQMSG_PROPERTIES, OCI_ATTR_ORIGINAL_MSGID,
        msg->proph, &value, NULL,
        msg->typinf->con->err
    )

    if (NULL != value)
    {
        const ub4 raw_len = OCIRawSize(msg->typinf->con->env, value);

        if (*len > raw_len)
        {
            *len = raw_len;
        }

        memcpy(id, OCIRawPtr(msg->typinf->con->env, value), (size_t) (*len));
    }

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * MessageSetOriginalID
 * --------------------------------------------------------------------------------------------- */

boolean MessageSetOriginalID
(
    OCI_Msg     *msg,
    const void  *id,
    unsigned int len
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_MSG, msg
    )

    OCIRaw *value = NULL;

    CHECK_PTR(OCI_IPC_MSG, msg)

    CHECK_OCI
    (
        msg->typinf->con->err,
        OCIRawAssignBytes,
        msg->typinf->con->env, msg->typinf->con->err,
        (ub1*) id, (ub4) len, (OCIRaw **) &value
    )

    CHECK_ATTRIB_SET
    (
        OCI_DTYPE_AQMSG_PROPERTIES, OCI_ATTR_ORIGINAL_MSGID,
        msg->proph, &value, 0,
        msg->typinf->con->err
    )

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * MessageGetCorrelation
 * --------------------------------------------------------------------------------------------- */

const otext * MessageGetCorrelation
(
    OCI_Msg *msg
)
{
    ENTER_FUNC
    (
        /* returns */ const otext*, NULL,
        /* context */ OCI_IPC_MSG, msg
    )

    CHECK_PTR(OCI_IPC_MSG, msg)

    if (!msg->correlation)
    {
        unsigned int size = 0;

        CHECK(StringGetAttribute(msg->typinf->con,
                                 msg->proph,
                                 OCI_DTYPE_AQMSG_PROPERTIES,
                                 OCI_ATTR_CORRELATION,
                                 &msg->correlation,
                                 &size))
    }

    SET_RETVAL(msg->correlation)

    EXIT_FUNC()
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
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_MSG, msg
    )

    CHECK_PTR(OCI_IPC_MSG, msg)

    CHECK(StringSetAttribute(msg->typinf->con,
                             msg->proph,
                             OCI_DTYPE_AQMSG_PROPERTIES,
                             OCI_ATTR_CORRELATION,
                             &msg->correlation,
                             correlation))

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * MessageGetExceptionQueue
 * --------------------------------------------------------------------------------------------- */

const otext * MessageGetExceptionQueue
(
    OCI_Msg *msg
)
{
    ENTER_FUNC
    (
        /* returns */ const otext*, NULL,
        /* context */ OCI_IPC_MSG, msg
    )

    CHECK_PTR(OCI_IPC_MSG, msg)

    if (!msg->except_queue)
    {
        unsigned int size = 0;

        CHECK(StringGetAttribute(msg->typinf->con,
                                 msg->proph,
                                 OCI_DTYPE_AQMSG_PROPERTIES,
                                 OCI_ATTR_EXCEPTION_QUEUE,
                                 &msg->except_queue,
                                 &size))
    }

    SET_RETVAL(msg->except_queue)

    EXIT_FUNC()
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
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_MSG, msg
    )

    CHECK_PTR(OCI_IPC_MSG, msg)

    CHECK(StringSetAttribute(msg->typinf->con,
                             msg->proph,
                             OCI_DTYPE_AQMSG_PROPERTIES,
                             OCI_ATTR_EXCEPTION_QUEUE,
                             &msg->except_queue,
                             queue))

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * MessageGetSender
 * --------------------------------------------------------------------------------------------- */

OCI_Agent * MessageGetSender
(
    OCI_Msg *msg
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Agent*, NULL,
        /* context */ OCI_IPC_MSG, msg
    )

    OCIAQAgent *handle = NULL;

    CHECK_PTR(OCI_IPC_MSG, msg)

    CHECK_ATTRIB_GET
    (
        OCI_DTYPE_AQMSG_PROPERTIES, OCI_ATTR_SENDER_ID,
        msg->proph, &handle, NULL,
        msg->typinf->con->err
    )

    if (NULL != handle)
    {
        msg->sender = OcilibAgentInitialize(msg->typinf->con, msg->sender, handle, NULL, NULL);
        CHECK_NULL(msg->sender)
    }

    SET_RETVAL(msg->sender)

    EXIT_FUNC()
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
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_MSG, msg
    )

    CHECK_PTR(OCI_IPC_MSG, msg)

    CHECK_ATTRIB_SET
    (
        OCI_DTYPE_AQMSG_PROPERTIES, OCI_ATTR_SENDER_ID,
        msg->proph, (sender ? sender->handle : NULL), 0,
        msg->typinf->con->err
    )

    SET_SUCCESS()

    EXIT_FUNC()
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
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_MSG, msg
    )

    OCIAQAgent **handles = NULL;

    CHECK_PTR(OCI_IPC_MSG, msg)

    /* allocate local array of OCIAQAgent handles if needed */

    if (NULL != consumers && (count > 0))
    {
        ALLOC_DATA(OCI_IPC_ARRAY, handles, count)

        for(unsigned int i = 0; i < count; i++)
        {
            handles[i] = consumers[i]->handle;
        }
    }
    else
    {
        count = 0;
    }

    CHECK_ATTRIB_SET
    (
        OCI_DTYPE_AQMSG_PROPERTIES, OCI_ATTR_RECIPIENT_LIST,
        msg->proph, handles, count,
        msg->typinf->con->err
    )

    /* free local array of OCIAQAgent handles if needed */

    if (NULL != handles)
    {
        FREE(handles)
    }

    SET_SUCCESS()

    EXIT_FUNC()
}
