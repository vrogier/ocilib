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

#include "enqueue.h"

#include "macros.h"
#include "memory.h"
#include "strings.h"

static const unsigned int VisibilityModeValues[] = { OCI_AMV_IMMEDIATE, OCI_AMV_ON_COMMIT };
static const unsigned int EnqueueModeValues[]    = { OCI_ASD_BEFORE, OCI_ASD_TOP };

/* --------------------------------------------------------------------------------------------- *
 * EnqueueCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Enqueue * EnqueueCreate
(
    OCI_TypeInfo *typinf,
    const otext  *name
)
{
    OCI_Enqueue *enqueue = NULL;

    CALL_ENTER(OCI_Enqueue*, enqueue)
    CALL_CHECK_PTR(OCI_IPC_TYPE_INFO, typinf)
    CALL_CHECK_PTR(OCI_IPC_STRING, name)
    CALL_CONTEXT_FROM_CON(typinf->con)

    /* allocate enqueue structure */

    ALLOC_DATA(OCI_IPC_ENQUEUE, enqueue, 1)

    if (STATUS)
    {
        enqueue->typinf = typinf;
        enqueue->name = ostrdup(name);

        /* allocate enqueue options descriptor */

        STATUS = MemoryAllocDescriptor((dvoid * ) enqueue->typinf->con->env, (dvoid **) &enqueue->opth, OCI_DTYPE_AQENQ_OPTIONS);
    }

    /* check for failure */

    if (STATUS)
    {
        RETVAL = enqueue;
    }
    else if (enqueue)
    {
        EnqueueFree(enqueue);
   }

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * EnqueueFree
 * --------------------------------------------------------------------------------------------- */

boolean EnqueueFree
(
    OCI_Enqueue *enqueue
)
{
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_ENQUEUE, enqueue)
    CALL_CONTEXT_FROM_CON(enqueue->typinf->con)

    /* free OCI descriptor */

    MemoryFreeDescriptor((dvoid *) enqueue->opth, OCI_DTYPE_AQENQ_OPTIONS);

    FREE(enqueue->name)
    FREE(enqueue)

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * EnqueuePut
 * --------------------------------------------------------------------------------------------- */

boolean EnqueuePut
(
    OCI_Enqueue *enqueue,
    OCI_Msg     *msg
)
{
    dbtext *dbstr    = NULL;
    int     dbsize   = -1;
    void   *payload  = NULL;
    void   *ind      = NULL;

    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_ENQUEUE, enqueue)
    CALL_CHECK_PTR(OCI_IPC_MSG, msg)
    CALL_CHECK_COMPAT(enqueue->typinf->con, enqueue->typinf->tdo == msg->typinf->tdo)
    CALL_CONTEXT_FROM_CON(enqueue->typinf->con)

    dbstr = StringGetDBString(enqueue->name, &dbsize);

    /* get payload */

    if (OCI_UNKNOWN != enqueue->typinf->typecode)
    {
        if (OCI_IND_NULL != msg->ind)
        {
            payload = msg->obj->handle;
            ind     = msg->obj->tab_ind;
        }
    }
    else
    {
        payload =  msg->payload;
        ind     = &msg->ind;
    }

    /* enqueue message */

    EXEC
    (
        OCIAQEnq(enqueue->typinf->con->cxt, enqueue->typinf->con->err,
                 (OraText *) dbstr, enqueue->opth, msg->proph, enqueue->typinf->tdo,
                 &payload, &ind, NULL, OCI_DEFAULT);
    )

    StringReleaseDBString(dbstr);

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * EnqueueGetVisibility
 * --------------------------------------------------------------------------------------------- */

unsigned int EnqueueGetVisibility
(
    OCI_Enqueue *enqueue
)
{
    ub4 ret = OCI_UNKNOWN;

    CALL_ENTER(unsigned int, ret)
    CALL_CHECK_PTR(OCI_IPC_ENQUEUE, enqueue)
    CALL_CONTEXT_FROM_CON(enqueue->typinf->con)

    ATTRIB_GET(OCI_DTYPE_AQENQ_OPTIONS, OCI_ATTR_VISIBILITY, enqueue->opth, &ret, NULL)
    
    RETVAL = ret;
 
    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * EnqueueSetVisibility
 * --------------------------------------------------------------------------------------------- */

boolean EnqueueSetVisibility
(
    OCI_Enqueue *enqueue,
    unsigned int visibility
)
{
    ub4 value = (ub4) visibility;

    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_ENQUEUE, enqueue)
    CALL_CHECK_ENUM_VALUE(NULL, NULL, visibility, VisibilityModeValues, OTEXT("Visibility Mode"))
    CALL_CONTEXT_FROM_CON(enqueue->typinf->con)

    ATTRIB_SET(OCI_DTYPE_AQENQ_OPTIONS, OCI_ATTR_VISIBILITY, enqueue->opth, &value, 0)

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * EnqueueGetSequenceDeviation
 * --------------------------------------------------------------------------------------------- */

unsigned int EnqueueGetSequenceDeviation
(
    OCI_Enqueue *enqueue
)
{
    ub4 ret = OCI_UNKNOWN;

    CALL_ENTER(unsigned int, ret)
    CALL_CHECK_PTR(OCI_IPC_ENQUEUE, enqueue)
    CALL_CONTEXT_FROM_CON(enqueue->typinf->con)

    ATTRIB_GET(OCI_DTYPE_AQENQ_OPTIONS, OCI_ATTR_SEQUENCE_DEVIATION, enqueue->opth, &ret, NULL)

    RETVAL = (unsigned int) ret;

    CALL_EXIT();
}

/* --------------------------------------------------------------------------------------------- *
 * EnqueueSetDeviation
 * --------------------------------------------------------------------------------------------- */

boolean EnqueueSetSequenceDeviation
(
    OCI_Enqueue *enqueue,
    unsigned int sequence
)
{
    ub4 value = (ub4) sequence;

    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_ENQUEUE, enqueue)
    CALL_CHECK_ENUM_VALUE(NULL, NULL, sequence, EnqueueModeValues, OTEXT("Sequence Deviation"))
    CALL_CONTEXT_FROM_CON(enqueue->typinf->con)

    ATTRIB_SET(OCI_DTYPE_AQENQ_OPTIONS, OCI_ATTR_SEQUENCE_DEVIATION, enqueue->opth, &value, 0)

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * EnqueueSetRelativeMsgID
 * --------------------------------------------------------------------------------------------- */

boolean EnqueueGetRelativeMsgID
(
    OCI_Enqueue  *enqueue,
    void         *id,
    unsigned int *len
)
{
    OCIRaw *value = NULL;

    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_ENQUEUE, enqueue)
    CALL_CHECK_PTR(OCI_IPC_VOID, id);
    CALL_CHECK_PTR(OCI_IPC_VOID, len);
    CALL_CONTEXT_FROM_CON(enqueue->typinf->con)

    ATTRIB_GET(OCI_DTYPE_AQENQ_OPTIONS, OCI_ATTR_RELATIVE_MSGID, enqueue->opth, &value, NULL)
    
    if (STATUS && value)
    {
        const ub4 raw_len = OCIRawSize(enqueue->typinf->con->env, value);

        if (*len > raw_len)
        {
            *len = raw_len;
        }

        memcpy(id, OCIRawPtr(enqueue->typinf->con->env, value), (size_t) (*len));
    }
    else
    {
        *len = 0;
    }

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * EnqueueSetRelativeMsgID
 * --------------------------------------------------------------------------------------------- */

boolean EnqueueSetRelativeMsgID
(
    OCI_Enqueue  *enqueue,
    const void   *id,
    unsigned int  len
)
{
    OCIRaw *value = NULL;

    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_ENQUEUE, enqueue)
    CALL_CONTEXT_FROM_CON(enqueue->typinf->con)

    EXEC(OCIRawAssignBytes(enqueue->typinf->con->env, enqueue->typinf->con->err,  (ub1*) id, (ub4) len, (OCIRaw **) &value))
    ATTRIB_SET(OCI_DTYPE_AQENQ_OPTIONS, OCI_ATTR_RELATIVE_MSGID, enqueue->opth, &value, 0)
    
    RETVAL = STATUS;

    CALL_EXIT()
}
