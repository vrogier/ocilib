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
 *                             PRIVATE FUNCTIONS
 * ********************************************************************************************* */

/* --------------------------------------------------------------------------------------------- *
 * OCI_ThreadProc
 * --------------------------------------------------------------------------------------------- */

void OCI_ThreadProc
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

/* ********************************************************************************************* *
 *                            PUBLIC FUNCTIONS
 * ********************************************************************************************* */

/* --------------------------------------------------------------------------------------------- *
 * OCI_ThreadCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Thread * OCI_API OCI_ThreadCreate
(
    void
)
{
    OCI_Thread *thread = NULL;

    OCI_CALL_ENTER(OCI_Thread*, NULL)
    OCI_CALL_CHECK_INITIALIZED()
    OCI_CALL_CHECK_THREAD_ENABLED()

    /* allocate thread structure */

    OCI_ALLOCATE_DATA(OCI_IPC_THREAD, thread, 1)

    if (OCI_STATUS)
    {
        /* allocate error handle */

        OCI_STATUS = OCI_HandleAlloc(OCILib.env, (dvoid **)(void *)&thread->err, OCI_HTYPE_ERROR);

        /* allocate thread handle */

        OCI_EXEC(OCIThreadHndInit(OCILib.env, thread->err, &thread->handle))

        /* allocate thread ID */

        OCI_EXEC(OCIThreadIdInit(OCILib.env, thread->err, &thread->id))
    }

    if (OCI_STATUS)
    {
        OCI_RETVAL = thread;
    }
    else if (thread)
    {
        OCI_ThreadFree(thread);
    }

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ThreadFree
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ThreadFree
(
    OCI_Thread *thread
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_THREAD_ENABLED()
    OCI_CALL_CHECK_PTR(OCI_IPC_THREAD, thread)
    OCI_CALL_CONTEXT_SET_FROM_ERR(thread->err)

    /* close thread handle */

    if (thread->handle)
    {
        OCI_EXEC(OCIThreadClose(OCILib.env, thread->err, thread->handle))
        OCI_EXEC(OCIThreadHndDestroy(OCILib.env, thread->err, &thread->handle))
    }

    /* close thread id */

    if (thread->id)
    {
        OCI_EXEC(OCIThreadIdDestroy(OCILib.env, thread->err, &thread->id))
    }

    /* close error handle */

    if (thread->err)
    {
        OCI_HandleFree(thread->err, OCI_HTYPE_ERROR);
    }

    /* free thread structure */

    OCI_FREE(thread)

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ThreadRun
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ThreadRun
(
    OCI_Thread *thread,
    POCI_THREAD proc,
    void       *arg
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_THREAD, thread)
    OCI_CALL_CHECK_PTR(OCI_IPC_PROC, proc)
    OCI_CALL_CONTEXT_SET_FROM_ERR(thread->err)

    thread->proc = proc;
    thread->arg  = arg;

    OCI_EXEC(OCIThreadCreate(OCILib.env, thread->err, OCI_ThreadProc, thread, thread->id, thread->handle))

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ThreadJoin
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ThreadJoin
(
    OCI_Thread *thread
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_THREAD, thread)
    OCI_CALL_CONTEXT_SET_FROM_ERR(thread->err)

    OCI_EXEC(OCIThreadJoin(OCILib.env, thread->err, thread->handle))

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}
