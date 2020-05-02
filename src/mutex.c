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
    ENTER_FUNC
    (
        /* returns */ OCI_Mutex*, NULL,
        /* context */ OCI_IPC_VOID, &Env
    )

    /* allocate mutex structure */

    OCI_Mutex *mutex = NULL;

    ALLOC_DATA(OCI_IPC_MUTEX, mutex, 1)

    /* allocate error handle */

    CHECK(MemoryAllocHandle(Env.env, (dvoid **)(void *)&mutex->err, OCI_HTYPE_ERROR))

    /* allocate mutex handle */

    CHECK_OCI
    (
        mutex->err,
        OCIThreadMutexInit, 
        Env.env, mutex->err, 
        &mutex->handle
    )

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE)
        {
            MutexFree(mutex);
            mutex = NULL;
        }

        SET_RETVAL(mutex)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * MutexCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Mutex * MutexCreate
(
    void
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Mutex*, NULL,
        /* context */ OCI_IPC_VOID, &Env
    )

    CHECK_INITIALIZED()

    SET_RETVAL(MutexCreateInternal())

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * MutexFree
 * --------------------------------------------------------------------------------------------- */

boolean MutexFree
(
    OCI_Mutex *mutex
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_MUTEX, mutex
    )

    CHECK_PTR(OCI_IPC_MUTEX, mutex)

    /* close mutex handle */

    if (NULL != mutex->handle)
    {
        CHECK_OCI
        (
            mutex->err,
            OCIThreadMutexDestroy,
            Env.env, mutex->err, &mutex->handle
        )
    }

    /* close error handle */

    if (NULL != mutex->err)
    {
        MemoryFreeHandle(mutex->err, OCI_HTYPE_ERROR);
    }

    /* free mutex structure */

    FREE(mutex)

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * MutexAcquire
 * --------------------------------------------------------------------------------------------- */

boolean MutexAcquire
(
    OCI_Mutex *mutex
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_MUTEX, mutex
    )

    CHECK_PTR(OCI_IPC_MUTEX, mutex)

    CHECK_OCI
    (
        mutex->err,
        OCIThreadMutexAcquire, 
        Env.env, mutex->err, mutex->handle
    )

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * MutexRelease
 * --------------------------------------------------------------------------------------------- */

boolean MutexRelease
(
    OCI_Mutex *mutex
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_MUTEX, mutex
    )

    CHECK_PTR(OCI_IPC_MUTEX, mutex)

    CHECK_OCI
    (
        mutex->err,
        OCIThreadMutexRelease, 
        Env.env, mutex->err, mutex->handle
    )

    SET_SUCCESS()

    EXIT_FUNC()
}
