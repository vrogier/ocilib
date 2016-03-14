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
 *                            PRIVATE FUNCTIONS
 * ********************************************************************************************* */

/* --------------------------------------------------------------------------------------------- *
 * OCI_MutexCreateInternal
 * --------------------------------------------------------------------------------------------- */

OCI_Mutex * OCI_MutexCreateInternal
(
    void
)
{
    OCI_Mutex *mutex = NULL;
    boolean    res   = FALSE;

    /* allocate mutex structure */

    mutex = (OCI_Mutex *) OCI_MemAlloc(OCI_IPC_MUTEX, sizeof(*mutex), (size_t) 1, TRUE);

    if (mutex)
    {
        /* allocate error handle */

        res = OCI_SUCCESSFUL(OCI_HandleAlloc(OCILib.env, (dvoid **) (void *) &mutex->err,
                                             OCI_HTYPE_ERROR, (size_t) 0, (dvoid **) NULL));

        /* allocate mutex handle */

        OCI_CALL3
        (
            res, mutex->err,

            OCIThreadMutexInit(OCILib.env, mutex->err, &mutex->handle)
        )
    }

    if (!res && mutex)
    {
        OCI_MutexFree(mutex);
        mutex = NULL;
    }

    return mutex;
}

/* ********************************************************************************************* *
 *                            PUBLIC FUNCTIONS
 * ********************************************************************************************* */

/* --------------------------------------------------------------------------------------------- *
 * OCI_MutexCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Mutex * OCI_API OCI_MutexCreate
(
    void
)
{
    OCI_LIB_CALL_ENTER(OCI_Mutex*, NULL)

    OCI_CHECK_INITIALIZED()

    call_retval = OCI_MutexCreateInternal();
    call_status = (NULL != call_retval);

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_MutexFree
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_MutexFree
(
    OCI_Mutex *mutex
)
{
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_MUTEX, mutex)

    /* close mutex handle */

    if (mutex->handle)
    {
        OCI_CALL0
        (
            call_status, mutex->err,

            OCIThreadMutexDestroy(OCILib.env, mutex->err, &mutex->handle)
        )
    }

    /* close error handle */

    if (mutex->err)
    {
        OCI_HandleFree(mutex->err, OCI_HTYPE_ERROR);
    }

    /* free mutex structure */

    OCI_FREE(mutex)

    call_retval = call_status;
    
    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_MutexAcquire
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_MutexAcquire
(
    OCI_Mutex *mutex
)
{
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_MUTEX, mutex)

    OCI_CALL0
    (
        call_status, mutex->err,

        OCIThreadMutexAcquire(OCILib.env, mutex->err, mutex->handle)
    )

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_MutexRelease
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_MutexRelease
(
    OCI_Mutex *mutex
)
{
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_MUTEX, mutex)

    OCI_CALL0
    (
        call_status, mutex->err,

        OCIThreadMutexRelease(OCILib.env, mutex->err, mutex->handle)
    )

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
}
