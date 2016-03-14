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
 *                             PRIVATE VARIABLES
 * ********************************************************************************************* */

static unsigned int VisibilityModeValues[] = { OCI_AMV_IMMEDIATE, OCI_AMV_ON_COMMIT };
static unsigned int EnqueueModeValues[] = { OCI_ASD_BEFORE, OCI_ASD_TOP };

/* ********************************************************************************************* *
 *                            PUBLIC FUNCTIONS
 * ********************************************************************************************* */

/* --------------------------------------------------------------------------------------------- *
 * OCI_EnqueueCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Enqueue * OCI_API OCI_EnqueueCreate
(
    OCI_TypeInfo *typinf,
    const otext  *name
)
{

    OCI_Enqueue *enqueue = NULL;

    OCI_LIB_CALL_ENTER(OCI_Enqueue*, enqueue)

    OCI_CHECK_PTR(OCI_IPC_TYPE_INFO, typinf)
    OCI_CHECK_PTR(OCI_IPC_STRING, name)

    /* allocate enqueue structure */

    enqueue = (OCI_Enqueue *) OCI_MemAlloc(OCI_IPC_ENQUEUE, sizeof(*enqueue), (size_t) 1, TRUE);

    if (enqueue)
    {
        enqueue->typinf = typinf;
        enqueue->name = ostrdup(name);

        /* allocate enqueue options descriptor */

        call_status = OCI_SUCCESSFUL(OCI_DescriptorAlloc((dvoid * ) enqueue->typinf->con->env,
                                                         (dvoid **) &enqueue->opth,
                                                         OCI_DTYPE_AQENQ_OPTIONS,
                                                         (size_t) 0, (dvoid **) NULL));
    }

    /* check for failure */

    if (call_status)
    {
        call_retval = enqueue;
    }
    else if (enqueue)
    {
        OCI_EnqueueFree(enqueue);
   }

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_EnqueueFree
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_EnqueueFree
(
    OCI_Enqueue *enqueue
)
{
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_ENQUEUE, enqueue)

    /* free OCI descriptor */

    OCI_DescriptorFree((dvoid *) enqueue->opth, OCI_DTYPE_AQENQ_OPTIONS);

    OCI_FREE(enqueue->name)
    OCI_FREE(enqueue)

    call_retval = call_status = TRUE;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_EnqueuePut
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_EnqueuePut
(
    OCI_Enqueue *enqueue,
    OCI_Msg     *msg
)
{
    dbtext *dbstr   = NULL;
    int     dbsize  = -1;

    void *payload  = NULL;
    void *ind      = NULL;

    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_ENQUEUE, enqueue)
    OCI_CHECK_PTR(OCI_IPC_MSG, msg)
    OCI_CHECK_COMPAT(enqueue->typinf->con, enqueue->typinf->tdo == msg->typinf->tdo)

    call_status = TRUE;

    dbstr = OCI_StringGetOracleString(enqueue->name, &dbsize);

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

    OCI_CALL2
    (
        call_status, enqueue->typinf->con,

        OCIAQEnq(enqueue->typinf->con->cxt, enqueue->typinf->con->err,
                 (OraText *) dbstr, enqueue->opth, msg->proph, enqueue->typinf->tdo,
                 &payload, &ind, NULL, OCI_DEFAULT);
    )

    OCI_StringReleaseOracleString(dbstr);

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_EnqueueGetVisibility
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_EnqueueGetVisibility
(
    OCI_Enqueue *enqueue
)
{
    ub4 ret = OCI_UNKNOWN;

    OCI_LIB_CALL_ENTER(unsigned int, ret)

    OCI_CHECK_PTR(OCI_IPC_ENQUEUE, enqueue)

    call_status = TRUE;

    OCI_CALL2
    (
        call_status, enqueue->typinf->con,

        OCIAttrGet((dvoid *) enqueue->opth,
                   (ub4    ) OCI_DTYPE_AQENQ_OPTIONS,
                   (dvoid *) &ret,
                   (ub4   *) NULL,
                   (ub4    ) OCI_ATTR_VISIBILITY,
                   enqueue->typinf->con->err)
    )
    
    call_retval = ret;
 
    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_EnqueueSetVisibility
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_EnqueueSetVisibility
(
    OCI_Enqueue *enqueue,
    unsigned int visibility
)
{
    ub4 value = (ub4) visibility;

    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_ENQUEUE, enqueue)
    OCI_CHECK_ENUM_VALUE(NULL, NULL, visibility, VisibilityModeValues, OTEXT("Visibility Mode"))

    call_status = TRUE;

    OCI_CALL2
    (
        call_status, enqueue->typinf->con,

        OCIAttrSet((dvoid *) enqueue->opth,
                   (ub4    ) OCI_DTYPE_AQENQ_OPTIONS,
                   (dvoid *) &value,
                   (ub4    ) 0,
                   (ub4    ) OCI_ATTR_VISIBILITY,
                   enqueue->typinf->con->err)
    )

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_EnqueueGetSequenceDeviation
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_EnqueueGetSequenceDeviation
(
    OCI_Enqueue *enqueue
)
{
    ub4 ret = OCI_UNKNOWN;

    OCI_LIB_CALL_ENTER(unsigned int, ret)

    OCI_CHECK_PTR(OCI_IPC_ENQUEUE, enqueue)

    call_status = TRUE;

    OCI_CALL2
    (
        call_status, enqueue->typinf->con,

        OCIAttrGet((dvoid *) enqueue->opth,
                   (ub4    ) OCI_DTYPE_AQENQ_OPTIONS,
                   (dvoid *) &ret,
                   (ub4   *) NULL,
                   (ub4    ) OCI_ATTR_SEQUENCE_DEVIATION,
                   enqueue->typinf->con->err)
    )

    call_retval = (unsigned int) ret;

    OCI_LIB_CALL_EXIT();
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_EnqueueSetDeviation
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_EnqueueSetSequenceDeviation
(
    OCI_Enqueue *enqueue,
    unsigned int sequence
)
{
    ub4 value = (ub4) sequence;

    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_ENQUEUE, enqueue)
    OCI_CHECK_ENUM_VALUE(NULL, NULL, sequence, EnqueueModeValues, OTEXT("Sequence Deviation"))

    call_status = TRUE;

    OCI_CALL2
    (
        call_status, enqueue->typinf->con,

        OCIAttrSet((dvoid *) enqueue->opth,
                   (ub4    ) OCI_DTYPE_AQENQ_OPTIONS,
                   (dvoid *) &value,
                   (ub4    ) 0,
                   (ub4    ) OCI_ATTR_SEQUENCE_DEVIATION,
                   enqueue->typinf->con->err)
    )

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_EnqueueSetRelativeMsgID
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_EnqueueGetRelativeMsgID
(
    OCI_Enqueue  *enqueue,
    void         *id,
    unsigned int *len
)
{
    OCIRaw *value = NULL;

    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_ENQUEUE, enqueue)
    OCI_CHECK_PTR(OCI_IPC_VOID,    id);
    OCI_CHECK_PTR(OCI_IPC_VOID,    len);

    call_status = TRUE;

    OCI_CALL2
    (
        call_status, enqueue->typinf->con,

        OCIAttrGet((dvoid *) enqueue->opth,
                   (ub4    ) OCI_DTYPE_AQENQ_OPTIONS,
                   (dvoid *) &value,
                   (ub4   *) NULL,
                   (ub4    ) OCI_ATTR_RELATIVE_MSGID,
                   enqueue->typinf->con->err)
    )

    if (call_status && value)
    {
        ub4 raw_len = OCIRawSize(enqueue->typinf->con->env, value);

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

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_EnqueueSetRelativeMsgID
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_EnqueueSetRelativeMsgID
(
    OCI_Enqueue  *enqueue,
    const void   *id,
    unsigned int  len
)
{
    OCIRaw *value = NULL;

    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_ENQUEUE, enqueue)

    call_status = TRUE;

    OCI_CALL2
    (
        call_status, enqueue->typinf->con,

        OCIRawAssignBytes(enqueue->typinf->con->env, enqueue->typinf->con->err,
                          (ub1*) id, (ub4) len, (OCIRaw **) &value)
    )

    OCI_CALL2
    (
        call_status, enqueue->typinf->con,

        OCIAttrSet((dvoid *) enqueue->opth,
                   (ub4    ) OCI_DTYPE_AQENQ_OPTIONS,
                   (dvoid *) &value,
                   (ub4    ) 0,
                   (ub4    ) OCI_ATTR_RELATIVE_MSGID,
                   enqueue->typinf->con->err)
    )

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
}

