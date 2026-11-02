/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2026 Vincent ROGIER <vince.rogier@ocilib.net>
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
#include "stringutils.h"

static const OCI_UInt32 VisibilityModeValues[] =
{
    OCI_AMV_IMMEDIATE,
    OCI_AMV_ON_COMMIT
};

static const OCI_UInt32 EnqueueModeValues[] =
{
    OCI_ASD_BEFORE,
    OCI_ASD_TOP
};

/* --------------------------------------------------------------------------------------------- *
 * OcilibEnqueueCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Enqueue * OcilibEnqueueCreate
(
    OCI_TypeInfo *typinf,
    const otext  *name
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Enqueue*, NULL,
        /* context */ OCI_IPC_TYPE_INFO, typinf
    );

    OCI_Enqueue* enqueue = NULL;

    CHECK_PTR(OCI_IPC_TYPE_INFO, typinf);
    CHECK_PTR(OCI_IPC_STRING,    name);

    /* allocate enqueue structure */

    ALLOC_HANDLE(OCI_IPC_ENQUEUE, enqueue);

    enqueue->typinf = typinf;
    enqueue->name   = OcilibStringDuplicate(enqueue->typinf->con->env, name);

    /* allocate enqueue options descriptor */

    CHECK(OcilibMemoryAllocDescriptor(enqueue->typinf->con->env,
                                      (dvoid *)enqueue->typinf->con->env->handle,
                                      (dvoid **) &enqueue->handle,
                                      OCI_DTYPE_AQENQ_OPTIONS));

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE)
        {
            OcilibEnqueueFree(enqueue);
            enqueue = NULL;
        }

        SET_RETVAL(enqueue);
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibEnqueueFree
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibEnqueueFree
(
    OCI_Enqueue *enqueue
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_ENQUEUE, enqueue
    );

    CHECK_PTR(OCI_IPC_ENQUEUE, enqueue);

    /* free OCI descriptor */

    OcilibMemoryFreeDescriptor(enqueue->typinf->con->env, (dvoid *) enqueue->handle, OCI_DTYPE_AQENQ_OPTIONS);

    OcilibErrorResetSource(NULL, enqueue);

    FREE(enqueue->name);
    FREE(enqueue);

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibEnqueuePut
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibEnqueuePut
(
    OCI_Enqueue *enqueue,
    OCI_Msg     *msg
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_ENQUEUE, enqueue
    );

    void *payload = NULL;
    void *ind     = NULL;

    sword ret = OCI_ERROR;

    OCI_Int32 dbsize = -1;
    dbtext  * dbstr  = NULL;

    char* ansi_queue_name = NULL;

    CHECK_PTR(OCI_IPC_ENQUEUE, enqueue);
    CHECK_PTR(OCI_IPC_MSG,     msg);
    CHECK_COMPAT(enqueue->typinf->handle == msg->typinf->handle);

    /* get payload */

    if (OCI_UNKNOWN != enqueue->typinf->typecode && OCI_IND_NULL != msg->ind)
    {
        payload = msg->obj->handle;
        ind     = msg->obj->tab_ind;
    }
    else
    {
        payload =  msg->payload;
        ind     = &msg->ind;
    }

    /* enqueue message */

    dbstr = OcilibStringGetDBString(enqueue->typinf->con->env,enqueue->name, &dbsize);

    /* OCIAQEnq() parameter  'queue_name' is supposed to be either ANSI or UTF16 depending on the
    * environment. It appears that whatever mode is used, OCIAQEnq() only takes ANSI strings !
    * Oracle might fix this issue at some point.
    * Thus let's try to handle oracle future version fixing this issue.
    * Make a first attempt using the correct way.
    * If an error is reported let's try again using an ANSI string.
    */

#ifdef OCI_CHARSET_WIDE
    const OCI_Size attempt_max = 2;
#else
    const OCI_Size attempt_max = 1;
#endif

    OCI_Size attempt_count = 0;

    while (OCI_ERROR == ret && ++attempt_count <= attempt_max)
    {
        void* name = attempt_count == 1 ? (void*)dbstr : (void*) ansi_queue_name;

        ret = enqueue->typinf->con->env->api->OCIAQEnq(enqueue->typinf->con->handle, enqueue->typinf->con->err,
                                                       (OraText*)name, enqueue->handle, msg->handle,
                                                       enqueue->typinf->handle, &payload, &ind, &msg->id, OCI_DEFAULT);

        /* check returned error code */

        if (OCI_ERROR == ret)
        {
            sb4 code = 0;

            enqueue->typinf->con->env->api->OCIErrorGet((dvoid *)enqueue->typinf->con->err, (ub4) 1,
                                                        (OraText *) NULL, &code, (OraText *) NULL, (ub4) 0,
                                                        (ub4) OCI_HTYPE_ERROR);

#ifdef OCI_CHARSET_WIDE

            if (dbstr == name && (OCI_ERR_AQ_QUEUE_NAME_INVALID == code ||
                                  OCI_ERR_AQ_QUEUE_NOT_EXIST == code))
            {
                /* non valid queue name in UTF16 mode, we have an Oracle client with OCIAQDeq()
                 * accepting only ANSI strings.* Let's try again with an ANSI string for queue name
                 */

                const OCI_Int32 len = (OCI_Int32)ostrlen(enqueue->name);
                ansi_queue_name = OcilibMemoryAlloc(enqueue->typinf->con->env, OCI_IPC_STRING, sizeof(char), len + 1, FALSE);
                OcilibStringNativeToAnsi(enqueue->name, ansi_queue_name, len);
            }
            else

#endif
            /* raise error only if the call has not been timed out */

            {
                THROW(OcilibExceptionOCI, enqueue->typinf->con->err, ret);
            }
        }
    }

    SET_SUCCESS();

    CLEANUP_AND_EXIT_FUNC
    (
        OcilibStringReleaseDBString(enqueue->typinf->con->env, dbstr);

        FREE(ansi_queue_name);
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibEnqueueGetVisibility
 * --------------------------------------------------------------------------------------------- */

OCI_QueueVisibility OcilibEnqueueGetVisibility
(
    OCI_Enqueue *enqueue
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_QueueVisibility, OCI_UNKNOWN,
        /* context */ OCI_IPC_ENQUEUE, enqueue
    );

    ub4 ret = OCI_UNKNOWN;

    CHECK_PTR(OCI_IPC_ENQUEUE, enqueue);

    CHECK_ATTRIB_GET
    (
        OCI_DTYPE_AQENQ_OPTIONS, OCI_ATTR_VISIBILITY,
        enqueue->handle, &ret, NULL,
        enqueue->typinf->con->err
    );

    SET_RETVAL(ret);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibEnqueueSetVisibility
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibEnqueueSetVisibility
(
    OCI_Enqueue        *enqueue,
    OCI_QueueVisibility visibility
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_ENQUEUE, enqueue
    );

    ub4 value = (ub4) visibility;

    CHECK_PTR(OCI_IPC_ENQUEUE, enqueue);
    CHECK_ENUM_VALUE(visibility, VisibilityModeValues, OTEXT("Visibility Mode"));

    CHECK_ATTRIB_SET
    (
        OCI_DTYPE_AQENQ_OPTIONS, OCI_ATTR_VISIBILITY,
        enqueue->handle, &value, 0,
        enqueue->typinf->con->err
    );

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibEnqueueGetSequenceDeviation
 * --------------------------------------------------------------------------------------------- */

OCI_QueueSeqDeviation OcilibEnqueueGetSequenceDeviation
(
    OCI_Enqueue *enqueue
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_QueueSeqDeviation, OCI_UNKNOWN,
        /* context */ OCI_IPC_ENQUEUE, enqueue
    );

    ub4 ret = OCI_UNKNOWN;

    CHECK_PTR(OCI_IPC_ENQUEUE, enqueue);

    CHECK_ATTRIB_GET
    (
        OCI_DTYPE_AQENQ_OPTIONS, OCI_ATTR_SEQUENCE_DEVIATION,
        enqueue->handle, &ret, NULL, enqueue->typinf->con->err
    );

    SET_RETVAL((OCI_UInt32) ret);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibEnqueueSetDeviation
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibEnqueueSetSequenceDeviation
(
    OCI_Enqueue          *enqueue,
    OCI_QueueSeqDeviation sequence
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_ENQUEUE, enqueue
    );

    ub4 value = (ub4) sequence;

    CHECK_PTR(OCI_IPC_ENQUEUE, enqueue);
    CHECK_ENUM_VALUE(sequence, EnqueueModeValues, OTEXT("Sequence Deviation"));

    CHECK_ATTRIB_SET
    (
        OCI_DTYPE_AQENQ_OPTIONS, OCI_ATTR_SEQUENCE_DEVIATION,
        enqueue->handle, &value, 0, enqueue->typinf->con->err
    );

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibEnqueueSetRelativeMsgID
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibEnqueueGetRelativeMsgID
(
    OCI_Enqueue *enqueue,
    void        *id,
    OCI_UInt32  *len
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_ENQUEUE, enqueue
    );

    OCIRaw *value = NULL;

    CHECK_PTR(OCI_IPC_ENQUEUE, enqueue);
    CHECK_PTR(OCI_IPC_VOID,    id);
    CHECK_PTR(OCI_IPC_VOID,    len);

    CHECK_ATTRIB_GET
    (
        OCI_DTYPE_AQENQ_OPTIONS, OCI_ATTR_RELATIVE_MSGID,
        enqueue->handle, &value, NULL,
        enqueue->typinf->con->err
    );

    if (NULL != value)
    {
        const ub4 raw_len = enqueue->typinf->con->env->api->OCIRawSize(enqueue->typinf->con->env->handle, value);

        if (*len > raw_len)
        {
            *len = raw_len;
        }

        memcpy(id, enqueue->typinf->con->env->api->OCIRawPtr(enqueue->typinf->con->env->handle, value), (OCI_Size) (*len));
    }
    else
    {
        *len = 0;
    }

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibEnqueueSetRelativeMsgID
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibEnqueueSetRelativeMsgID
(
    OCI_Enqueue *enqueue,
    const void  *id,
    OCI_UInt32   len
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_ENQUEUE, enqueue
    );

    OCIRaw *value = NULL;

    CHECK_PTR(OCI_IPC_ENQUEUE, enqueue);

    CHECK_OCI
    (
        enqueue->typinf->con->err,
        OCIRawAssignBytes,
        enqueue->typinf->con->env->handle, enqueue->typinf->con->err,
        (ub1*) id, (ub4) len, (OCIRaw **) &value
    );

    CHECK_ATTRIB_SET
    (
        OCI_DTYPE_AQENQ_OPTIONS, OCI_ATTR_RELATIVE_MSGID,
        enqueue->handle, &value, 0,
        enqueue->typinf->con->err
    );

    SET_SUCCESS();

    EXIT_FUNC();
}
