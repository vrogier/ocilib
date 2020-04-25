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

#include "mutex.h"

#include "macros.h"
#include "memory.h"

 /* --------------------------------------------------------------------------------------------- *
  * MutexCreateInternal
  * --------------------------------------------------------------------------------------------- */

OCI_Mutex * MutexCreateInternal
(
    void
)
{
    OCI_Mutex *mutex = NULL;

    DECLARE_CTX(TRUE)        

    /* allocate mutex structure */

    ALLOC_DATA(OCI_IPC_MUTEX, mutex, 1)

    if (STATUS)
    {
        /* allocate error handle */

        STATUS = MemoryAllocHandle(Env.env, (dvoid **)(void *)&mutex->err, OCI_HTYPE_ERROR);

        /* allocate mutex handle */

        EXEC(OCIThreadMutexInit(Env.env, mutex->err, &mutex->handle))
    }

    if (!STATUS && mutex)
    {
        MutexFree(mutex);
        mutex = NULL;
    }

    return mutex;
}

/* --------------------------------------------------------------------------------------------- *
 * MutexCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Mutex * MutexCreate
(
    void
)
{
    CALL_ENTER(OCI_Mutex*, NULL)
    CALL_CHECK_INITIALIZED()

    RETVAL = MutexCreateInternal();
    STATUS = (NULL != RETVAL);

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * MutexFree
 * --------------------------------------------------------------------------------------------- */

boolean MutexFree
(
    OCI_Mutex *mutex
)
{
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_MUTEX, mutex)
    CALL_CONTEXT_FROM_ERR(mutex->err)

    /* close mutex handle */

    if (mutex->handle)
    {
        EXEC(OCIThreadMutexDestroy(Env.env, mutex->err, &mutex->handle))
    }

    /* close error handle */

    if (mutex->err)
    {
        MemoryFreeHandle(mutex->err, OCI_HTYPE_ERROR);
    }

    /* free mutex structure */

    FREE(mutex)

    RETVAL = STATUS;
    
    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * MutexAcquire
 * --------------------------------------------------------------------------------------------- */

boolean MutexAcquire
(
    OCI_Mutex *mutex
)
{
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_MUTEX, mutex)
    CALL_CONTEXT_FROM_ERR(mutex->err)

    EXEC(OCIThreadMutexAcquire(Env.env, mutex->err, mutex->handle))

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * MutexRelease
 * --------------------------------------------------------------------------------------------- */

boolean MutexRelease
(
    OCI_Mutex *mutex
)
{
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_MUTEX, mutex)
    CALL_CONTEXT_FROM_ERR(mutex->err)

    EXEC(OCIThreadMutexRelease(Env.env, mutex->err, mutex->handle))
 
    RETVAL = STATUS;

    CALL_EXIT()
}
