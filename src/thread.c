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

#include "thread.h"

#include "macros.h"
#include "memory.h"

/* --------------------------------------------------------------------------------------------- *
 * ThreadProc
 * --------------------------------------------------------------------------------------------- */

void ThreadProc
(
    dvoid *arg
)
{
    OCI_Thread *thread = (OCI_Thread *) arg;

    if (thread)
    {
        thread->proc(thread, thread->arg);
    }
}

/* --------------------------------------------------------------------------------------------- *
 * ThreadCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Thread * ThreadCreate
(
    void
)
{
    OCI_Thread *thread = NULL;

    CALL_ENTER(OCI_Thread*, NULL)
    CALL_CHECK_INITIALIZED()
    CALL_CHECK_THREAD_ENABLED()

    /* allocate thread structure */

    ALLOC_DATA(OCI_IPC_THREAD, thread, 1)

    if (STATUS)
    {
        /* allocate error handle */

        STATUS = MemoryAllocHandle(Env.env, (dvoid **)(void *)&thread->err, OCI_HTYPE_ERROR);

        /* allocate thread handle */

        EXEC(OCIThreadHndInit(Env.env, thread->err, &thread->handle))

        /* allocate thread ID */

        EXEC(OCIThreadIdInit(Env.env, thread->err, &thread->id))
    }

    if (STATUS)
    {
        RETVAL = thread;
    }
    else if (thread)
    {
        ThreadFree(thread);
    }

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * ThreadFree
 * --------------------------------------------------------------------------------------------- */

boolean ThreadFree
(
    OCI_Thread *thread
)
{
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_THREAD_ENABLED()
    CALL_CHECK_PTR(OCI_IPC_THREAD, thread)
    CALL_CONTEXT_FROM_ERR(thread->err)

    /* close thread handle */

    if (thread->handle)
    {
        EXEC(OCIThreadClose(Env.env, thread->err, thread->handle))
        EXEC(OCIThreadHndDestroy(Env.env, thread->err, &thread->handle))
    }

    /* close thread id */

    if (thread->id)
    {
        EXEC(OCIThreadIdDestroy(Env.env, thread->err, &thread->id))
    }

    /* close error handle */

    if (thread->err)
    {
        MemoryFreeHandle(thread->err, OCI_HTYPE_ERROR);
    }

    /* free thread structure */

    FREE(thread)

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * ThreadRun
 * --------------------------------------------------------------------------------------------- */

boolean ThreadRun
(
    OCI_Thread *thread,
    POCI_THREAD proc,
    void       *arg
)
{
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_THREAD, thread)
    CALL_CHECK_PTR(OCI_IPC_PROC, proc)
    CALL_CONTEXT_FROM_ERR(thread->err)

    thread->proc = proc;
    thread->arg  = arg;

    EXEC(OCIThreadCreate(Env.env, thread->err, ThreadProc, thread, thread->id, thread->handle))

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * ThreadJoin
 * --------------------------------------------------------------------------------------------- */

boolean ThreadJoin
(
    OCI_Thread *thread
)
{
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_THREAD, thread)
    CALL_CONTEXT_FROM_ERR(thread->err)

    EXEC(OCIThreadJoin(Env.env, thread->err, thread->handle))

    RETVAL = STATUS;

    CALL_EXIT()
}
