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

    OCI_CALL_DECLARE_CONTEXT(TRUE)        

    /* allocate mutex structure */

    OCI_ALLOCATE_DATA(OCI_IPC_MUTEX, mutex, 1)

    if (OCI_STATUS)
    {
        /* allocate error handle */

        OCI_STATUS = OCI_HandleAlloc(OCILib.env, (dvoid **)(void *)&mutex->err, OCI_HTYPE_ERROR);

        /* allocate mutex handle */

        OCI_EXEC(OCIThreadMutexInit(OCILib.env, mutex->err, &mutex->handle))
    }

    if (!OCI_STATUS && mutex)
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
    OCI_CALL_ENTER(OCI_Mutex*, NULL)
    OCI_CALL_CHECK_INITIALIZED()

    OCI_RETVAL = OCI_MutexCreateInternal();
    OCI_STATUS = (NULL != OCI_RETVAL);

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_MutexFree
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_MutexFree
(
    OCI_Mutex *mutex
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_MUTEX, mutex)
    OCI_CALL_CONTEXT_SET_FROM_ERR(mutex->err)

    /* close mutex handle */

    if (mutex->handle)
    {
        OCI_EXEC(OCIThreadMutexDestroy(OCILib.env, mutex->err, &mutex->handle))
    }

    /* close error handle */

    if (mutex->err)
    {
        OCI_HandleFree(mutex->err, OCI_HTYPE_ERROR);
    }

    /* free mutex structure */

    OCI_FREE(mutex)

    OCI_RETVAL = OCI_STATUS;
    
    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_MutexAcquire
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_MutexAcquire
(
    OCI_Mutex *mutex
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_MUTEX, mutex)
    OCI_CALL_CONTEXT_SET_FROM_ERR(mutex->err)

    OCI_EXEC(OCIThreadMutexAcquire(OCILib.env, mutex->err, mutex->handle))

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_MutexRelease
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_MutexRelease
(
    OCI_Mutex *mutex
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_MUTEX, mutex)
    OCI_CALL_CONTEXT_SET_FROM_ERR(mutex->err)

    OCI_EXEC(OCIThreadMutexRelease(OCILib.env, mutex->err, mutex->handle))
 
    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}
