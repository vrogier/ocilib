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

static const unsigned int VisibilityModeValues[] =
{
    OCI_AMV_IMMEDIATE,
    OCI_AMV_ON_COMMIT
};

static const unsigned int EnqueueModeValues[] = 
{
    OCI_ASD_BEFORE,
    OCI_ASD_TOP
};

/* --------------------------------------------------------------------------------------------- *
 * EnqueueCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Enqueue * EnqueueCreate
(
    OCI_TypeInfo *typinf,
    const otext  *name
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Enqueue*, NULL,
        /* context */ OCI_IPC_TYPE_INFO, typinf
    )

    OCI_Enqueue* enqueue = NULL;

    CHECK_PTR(OCI_IPC_TYPE_INFO, typinf)
    CHECK_PTR(OCI_IPC_STRING, name)

    /* allocate enqueue structure */

    ALLOC_DATA(OCI_IPC_ENQUEUE, enqueue, 1)

    enqueue->typinf = typinf;
    enqueue->name   = ostrdup(name);

    /* allocate enqueue options descriptor */

    CHECK(MemoryAllocDescriptor((dvoid * )enqueue->typinf->con->env,
                                (dvoid **) &enqueue->opth, 
                                OCI_DTYPE_AQENQ_OPTIONS))

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE)
        {
            EnqueueFree(enqueue);
            enqueue = NULL;
        }

        SET_RETVAL(enqueue)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * EnqueueFree
 * --------------------------------------------------------------------------------------------- */

boolean EnqueueFree
(
    OCI_Enqueue *enqueue
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_ENQUEUE, enqueue
    )

    CHECK_PTR(OCI_IPC_ENQUEUE, enqueue)

    /* free OCI descriptor */

    MemoryFreeDescriptor((dvoid *) enqueue->opth, OCI_DTYPE_AQENQ_OPTIONS);

    FREE(enqueue->name)
    FREE(enqueue)

    SET_SUCCESS()

    EXIT_FUNC()
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
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_ENQUEUE, enqueue
    )

    dbtext *dbstr   = NULL;
    int     dbsize  = -1;
    void   *payload = NULL;
    void   *ind     = NULL;

    CHECK_PTR(OCI_IPC_ENQUEUE, enqueue)
    CHECK_PTR(OCI_IPC_MSG, msg)
    CHECK_COMPAT(enqueue->typinf->tdo == msg->typinf->tdo)

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

    CHECK_OCI
    (
        enqueue->typinf->con->err,
        OCIAQEnq,
        enqueue->typinf->con->cxt, enqueue->typinf->con->err,
        (OraText *) dbstr, enqueue->opth, msg->proph, 
        enqueue->typinf->tdo, &payload, &ind, NULL, OCI_DEFAULT
    )

    SET_SUCCESS()

    CLEANUP_AND_EXIT_FUNC
    (
        StringReleaseDBString(dbstr);
    )
}

/* --------------------------------------------------------------------------------------------- *
 * EnqueueGetVisibility
 * --------------------------------------------------------------------------------------------- */

unsigned int EnqueueGetVisibility
(
    OCI_Enqueue *enqueue
)
{
    ENTER_FUNC
    (
        /* returns */ unsigned int, OCI_UNKNOWN,
        /* context */ OCI_IPC_ENQUEUE, enqueue
    )

    ub4 ret = OCI_UNKNOWN;

    CHECK_PTR(OCI_IPC_ENQUEUE, enqueue)

    CHECK_ATTRIB_GET
    (
        OCI_DTYPE_AQENQ_OPTIONS, OCI_ATTR_VISIBILITY, 
        enqueue->opth, &ret, NULL,
        enqueue->typinf->con->err
    )

    SET_RETVAL(ret)

    EXIT_FUNC()
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
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_ENQUEUE, enqueue
    )

    ub4 value = (ub4) visibility;

    CHECK_PTR(OCI_IPC_ENQUEUE, enqueue)
    CHECK_ENUM_VALUE(visibility, VisibilityModeValues, OTEXT("Visibility Mode"))

    CHECK_ATTRIB_SET
    (
        OCI_DTYPE_AQENQ_OPTIONS, OCI_ATTR_VISIBILITY, 
        enqueue->opth, &value, 0,
        enqueue->typinf->con->err
    )

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * EnqueueGetSequenceDeviation
 * --------------------------------------------------------------------------------------------- */

unsigned int EnqueueGetSequenceDeviation
(
    OCI_Enqueue *enqueue
)
{
    ENTER_FUNC
    (
        /* returns */ unsigned int, OCI_UNKNOWN,
        /* context */ OCI_IPC_ENQUEUE, enqueue
    )

    ub4 ret = OCI_UNKNOWN;

    CHECK_PTR(OCI_IPC_ENQUEUE, enqueue)

    CHECK_ATTRIB_GET
    (
        OCI_DTYPE_AQENQ_OPTIONS, OCI_ATTR_SEQUENCE_DEVIATION, 
        enqueue->opth, &ret, NULL, enqueue->typinf->con->err
    )

    SET_RETVAL((unsigned int) ret)

    EXIT_FUNC()
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
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_ENQUEUE, enqueue
    )

    ub4 value = (ub4) sequence;

    CHECK_PTR(OCI_IPC_ENQUEUE, enqueue)
    CHECK_ENUM_VALUE(sequence, EnqueueModeValues, OTEXT("Sequence Deviation"))

    CHECK_ATTRIB_SET
    (
        OCI_DTYPE_AQENQ_OPTIONS, OCI_ATTR_SEQUENCE_DEVIATION,
        enqueue->opth, &value, 0, enqueue->typinf->con->err
    )

    SET_SUCCESS()

    EXIT_FUNC()
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
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_ENQUEUE, enqueue
    )

    OCIRaw *value = NULL;

    CHECK_PTR(OCI_IPC_ENQUEUE, enqueue)
    CHECK_PTR(OCI_IPC_VOID, id);
    CHECK_PTR(OCI_IPC_VOID, len);

    CHECK_ATTRIB_GET
    (
        OCI_DTYPE_AQENQ_OPTIONS, OCI_ATTR_RELATIVE_MSGID, 
        enqueue->opth, &value, NULL,
        enqueue->typinf->con->err
    )

    if (NULL != value)
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

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * EnqueueSetRelativeMsgID
 * --------------------------------------------------------------------------------------------- */

boolean EnqueueSetRelativeMsgID
(
    OCI_Enqueue *enqueue,
    const void  *id,
    unsigned int len
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_ENQUEUE, enqueue
    )

    OCIRaw *value = NULL;

    CHECK_PTR(OCI_IPC_ENQUEUE, enqueue)

    CHECK_OCI
    (
        enqueue->typinf->con->err,
        OCIRawAssignBytes,
        enqueue->typinf->con->env, enqueue->typinf->con->err,  
        (ub1*) id, (ub4) len, (OCIRaw **) &value
    )

    CHECK_ATTRIB_SET
    (
        OCI_DTYPE_AQENQ_OPTIONS, OCI_ATTR_RELATIVE_MSGID,
        enqueue->opth, &value, 0,
        enqueue->typinf->con->err
    )

    SET_SUCCESS()

    EXIT_FUNC()
}
