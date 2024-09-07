/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2024 Vincent ROGIER <vince.rogier@ocilib.net>
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

#include "thread.h"

#include "macros.h"
#include "memory.h"

/* --------------------------------------------------------------------------------------------- *
 * OcilibThreadProc
 * --------------------------------------------------------------------------------------------- */

static void OcilibThreadProc
(
    dvoid *arg
)
{
    OCI_Thread *thread = (OCI_Thread *) arg;

    if (NULL != thread)
    {
        thread->proc(thread, thread->arg);
    }
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibThreadCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Thread * OcilibThreadCreate
(
    void
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Thread*, NULL,
        /* context */ OCI_IPC_VOID, &Env
    )

    OCI_Thread* thread = NULL;

    CHECK_INITIALIZED()
    CHECK_THREAD_ENABLED()

    /* allocate thread structure */

    ALLOC_DATA(OCI_IPC_THREAD, thread, 1)

    /* allocate error handle */

    CHECK(OcilibMemoryAllocHandle(Env.env, (dvoid **)(void *)&thread->err, OCI_HTYPE_ERROR))

    /* allocate thread handle */

    CHECK_OCI
    (
        thread->err,
        OCIThreadHndInit,
        Env.env, thread->err,
        &thread->handle
    )

    /* allocate thread ID */

    CHECK_OCI
    (
        thread->err,
        OCIThreadIdInit,
        Env.env, thread->err,
        &thread->id
    )

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE)
        {
            OcilibThreadFree(thread);
            thread = NULL;
        }

        SET_RETVAL(thread)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibThreadFree
 * --------------------------------------------------------------------------------------------- */

boolean OcilibThreadFree
(
    OCI_Thread *thread
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_THREAD, thread
    )

    CHECK_THREAD_ENABLED()
    CHECK_PTR(OCI_IPC_THREAD, thread)

    /* close thread handle */

    if (NULL != thread->handle)
    {
        CHECK_OCI
        (
            thread->err,
            OCIThreadClose,
            Env.env, thread->err,
            thread->handle
        )

        CHECK_OCI
        (
            thread->err,
            OCIThreadHndDestroy,
            Env.env, thread->err,
            &thread->handle
        )
    }

    /* close thread id */

    if (NULL != thread->id)
    {
        CHECK_OCI
        (
            thread->err,
            OCIThreadIdDestroy,
            Env.env, thread->err,
            &thread->id
        )
    }

    /* close error handle */

    if (NULL != thread->err)
    {
        OcilibMemoryFreeHandle(thread->err, OCI_HTYPE_ERROR);
    }
    
    /* free thread structure */

    OcilibErrorResetSource(NULL, thread);

    FREE(thread)

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibThreadRun
 * --------------------------------------------------------------------------------------------- */

boolean OcilibThreadRun
(
    OCI_Thread *thread,
    POCI_THREAD proc,
    void       *arg
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_THREAD, thread
    )

    CHECK_PTR(OCI_IPC_THREAD, thread)
    CHECK_PTR(OCI_IPC_PROC,   proc)

    thread->proc = proc;
    thread->arg  = arg;

    CHECK_OCI
    (
        thread->err,
        OCIThreadCreate,
        Env.env, thread->err, OcilibThreadProc,
        thread, thread->id, thread->handle
    )

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibThreadJoin
 * --------------------------------------------------------------------------------------------- */

boolean OcilibThreadJoin
(
    OCI_Thread *thread
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_THREAD, thread
    )

    CHECK_PTR(OCI_IPC_THREAD, thread)

    CHECK_OCI
    (
        thread->err,
        OCIThreadJoin,
        Env.env, thread->err,
        thread->handle
    )

    SET_SUCCESS()

    EXIT_FUNC()
}
