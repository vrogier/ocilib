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
 *                             PRIVATE VARIABLES
 * ********************************************************************************************* */

static const unsigned int VisibilityModeValues[] = { OCI_AMV_IMMEDIATE, OCI_AMV_ON_COMMIT };
static const unsigned int EnqueueModeValues[]    = { OCI_ASD_BEFORE, OCI_ASD_TOP };

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

    OCI_CALL_ENTER(OCI_Enqueue*, enqueue)
    OCI_CALL_CHECK_PTR(OCI_IPC_TYPE_INFO, typinf)
    OCI_CALL_CHECK_PTR(OCI_IPC_STRING, name)
    OCI_CALL_CONTEXT_SET_FROM_CONN(typinf->con)

    /* allocate enqueue structure */

    OCI_ALLOCATE_DATA(OCI_IPC_ENQUEUE, enqueue, 1)

    if (OCI_STATUS)
    {
        enqueue->typinf = typinf;
        enqueue->name = ostrdup(name);

        /* allocate enqueue options descriptor */

        OCI_STATUS = OCI_DescriptorAlloc((dvoid * ) enqueue->typinf->con->env, (dvoid **) &enqueue->opth, OCI_DTYPE_AQENQ_OPTIONS);
    }

    /* check for failure */

    if (OCI_STATUS)
    {
        OCI_RETVAL = enqueue;
    }
    else if (enqueue)
    {
        OCI_EnqueueFree(enqueue);
   }

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_EnqueueFree
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_EnqueueFree
(
    OCI_Enqueue *enqueue
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_ENQUEUE, enqueue)
    OCI_CALL_CONTEXT_SET_FROM_CONN(enqueue->typinf->con)

    /* free OCI descriptor */

    OCI_DescriptorFree((dvoid *) enqueue->opth, OCI_DTYPE_AQENQ_OPTIONS);

    OCI_FREE(enqueue->name)
    OCI_FREE(enqueue)

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
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
    dbtext *dbstr    = NULL;
    int     dbsize   = -1;
    void   *payload  = NULL;
    void   *ind      = NULL;

    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_ENQUEUE, enqueue)
    OCI_CALL_CHECK_PTR(OCI_IPC_MSG, msg)
    OCI_CALL_CHECK_COMPAT(enqueue->typinf->con, enqueue->typinf->tdo == msg->typinf->tdo)
    OCI_CALL_CONTEXT_SET_FROM_CONN(enqueue->typinf->con)

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

    OCI_EXEC
    (
        OCIAQEnq(enqueue->typinf->con->cxt, enqueue->typinf->con->err,
                 (OraText *) dbstr, enqueue->opth, msg->proph, enqueue->typinf->tdo,
                 &payload, &ind, NULL, OCI_DEFAULT);
    )

    OCI_StringReleaseOracleString(dbstr);

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
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

    OCI_CALL_ENTER(unsigned int, ret)
    OCI_CALL_CHECK_PTR(OCI_IPC_ENQUEUE, enqueue)
    OCI_CALL_CONTEXT_SET_FROM_CONN(enqueue->typinf->con)

    OCI_GET_ATTRIB(OCI_DTYPE_AQENQ_OPTIONS, OCI_ATTR_VISIBILITY, enqueue->opth, &ret, NULL)
    
    OCI_RETVAL = ret;
 
    OCI_CALL_EXIT()
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

    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_ENQUEUE, enqueue)
    OCI_CALL_CHECK_ENUM_VALUE(NULL, NULL, visibility, VisibilityModeValues, OTEXT("Visibility Mode"))
    OCI_CALL_CONTEXT_SET_FROM_CONN(enqueue->typinf->con)

    OCI_SET_ATTRIB(OCI_DTYPE_AQENQ_OPTIONS, OCI_ATTR_VISIBILITY, enqueue->opth, &value, 0)

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
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

    OCI_CALL_ENTER(unsigned int, ret)
    OCI_CALL_CHECK_PTR(OCI_IPC_ENQUEUE, enqueue)
    OCI_CALL_CONTEXT_SET_FROM_CONN(enqueue->typinf->con)

    OCI_GET_ATTRIB(OCI_DTYPE_AQENQ_OPTIONS, OCI_ATTR_SEQUENCE_DEVIATION, enqueue->opth, &ret, NULL)

    OCI_RETVAL = (unsigned int) ret;

    OCI_CALL_EXIT();
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

    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_ENQUEUE, enqueue)
    OCI_CALL_CHECK_ENUM_VALUE(NULL, NULL, sequence, EnqueueModeValues, OTEXT("Sequence Deviation"))
    OCI_CALL_CONTEXT_SET_FROM_CONN(enqueue->typinf->con)

    OCI_SET_ATTRIB(OCI_DTYPE_AQENQ_OPTIONS, OCI_ATTR_SEQUENCE_DEVIATION, enqueue->opth, &value, 0)

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
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

    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_ENQUEUE, enqueue)
    OCI_CALL_CHECK_PTR(OCI_IPC_VOID, id);
    OCI_CALL_CHECK_PTR(OCI_IPC_VOID, len);
    OCI_CALL_CONTEXT_SET_FROM_CONN(enqueue->typinf->con)

    OCI_GET_ATTRIB(OCI_DTYPE_AQENQ_OPTIONS, OCI_ATTR_RELATIVE_MSGID, enqueue->opth, &value, NULL)
    
    if (OCI_STATUS && value)
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

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
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

    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_ENQUEUE, enqueue)
    OCI_CALL_CONTEXT_SET_FROM_CONN(enqueue->typinf->con)

    OCI_EXEC(OCIRawAssignBytes(enqueue->typinf->con->env, enqueue->typinf->con->err,  (ub1*) id, (ub4) len, (OCIRaw **) &value))
    OCI_SET_ATTRIB(OCI_DTYPE_AQENQ_OPTIONS, OCI_ATTR_RELATIVE_MSGID, enqueue->opth, &value, 0)
    
    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}
