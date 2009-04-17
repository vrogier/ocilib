/*
   +----------------------------------------------------------------------+   
   |                                                                      |
   |                     OCILIB - C Driver for Oracle                     |
   |                                                                      |
   |                      (C Wrapper for Oracle OCI)                      |
   |                                                                      |
   +----------------------------------------------------------------------+
   |                      Website : http://ocilib.net                     |
   +----------------------------------------------------------------------+
   |               Copyright (c) 2007-2009 Vincent ROGIER                 |
   +----------------------------------------------------------------------+
   | This library is free software; you can redistribute it and/or        |
   | modify it under the terms of the GNU Library General Public          |
   | License as published by the Free Software Foundation; either         |
   | version 2 of the License, or (at your option) any later version.     |
   |                                                                      |
   | This library is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU    |
   | Library General Public License for more details.                     |
   |                                                                      |
   | You should have received a copy of the GNU Library General Public    |
   | License along with this library; if not, write to the Free           |
   | Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.   |
   +----------------------------------------------------------------------+
   |          Author: Vincent ROGIER <vince.rogier@gmail.com>             |
   +----------------------------------------------------------------------+ 
*/

/* ------------------------------------------------------------------------ *
 * $Id: thread.c, v 3.2.0 2009/04/20 00:00 Vince $
 * ------------------------------------------------------------------------ */

#include "ocilib_internal.h"

/* ************************************************************************ *
 *                             PRIVATE FUNCTIONS
 * ************************************************************************ */

/* ------------------------------------------------------------------------ *
 * OCI_ThreadProc
 * ------------------------------------------------------------------------ */

void OCI_ThreadProc(dvoid *arg)
{
    OCI_Thread *thread = (OCI_Thread *) arg;

    if (thread != NULL)
        thread->proc(thread, thread->arg);
}

/* ************************************************************************ *
 *                            PUBLIC FUNCTIONS
 * ************************************************************************ */

/* ------------------------------------------------------------------------ *
 * OCI_ThreadCreate
 * ------------------------------------------------------------------------ */

OCI_Thread * OCI_API OCI_ThreadCreate(void)
{
    OCI_Thread *thread = NULL;
    boolean res        = TRUE;

    OCI_CHECK_INITIALIZED(NULL);

    OCI_CHECK_THREAD_ENABLED(FALSE);

    /* allocate thread structure */

    thread = (OCI_Thread *) OCI_MemAlloc(OCI_IPC_THREAD, sizeof(*thread), 1, TRUE);

    if (thread != NULL)
    {
        /* allocate error handle */

        res = (OCI_SUCCESS == OCI_HandleAlloc(OCILib.env, 
                                              (dvoid **) (void *) &thread->err,
                                              OCI_HTYPE_ERROR, 0,
                                              (dvoid **) NULL));

        /* allocate thread handle */

        OCI_CALL3
        (
            res, thread->err,
            
            OCIThreadHndInit(OCILib.env, thread->err, &thread->handle)
        )

        /* allocate thread ID */

        OCI_CALL3
        (
            res, thread->err,
            
            OCIThreadIdInit(OCILib.env, thread->err, &thread->id)
        )    
    }
    else
        res = FALSE;

    if (res == FALSE)
    {
        OCI_ThreadFree(thread);
        thread = NULL;
    }

    OCI_RESULT(res);

    return thread;
}

/* ------------------------------------------------------------------------ *
 * OCI_ThreadFree
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_ThreadFree(OCI_Thread *thread)
{
    boolean res = TRUE;

    OCI_CHECK_THREAD_ENABLED(FALSE);

    OCI_CHECK_PTR(OCI_IPC_THREAD, thread, FALSE);

    /* close thread handle */

    if (thread->handle != NULL)
    {
        OCI_CALL0
        (
            res, thread->err, 
            
            OCIThreadClose(OCILib.env, thread->err, thread->handle)
        )

        OCI_CALL0
        (
            res, thread->err, 
            
            OCIThreadHndDestroy(OCILib.env, thread->err, &thread->handle)
        )
    }

    /* close thread id */

    if (thread->id != NULL)
    {
        OCI_CALL0
        (
            res, thread->err, 
            
            OCIThreadIdDestroy(OCILib.env, thread->err, &thread->id)
         )    
    }

    /* close error handle */

    if (thread->err != NULL)
    {
        OCI_HandleFree(thread->err, OCI_HTYPE_ERROR);
    }

    /* free mutex structure */

    OCI_FREE(thread);

    OCI_RESULT(res);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_ThreadRun
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_ThreadRun(OCI_Thread *thread, POCI_THREAD proc, void *arg)
{
    boolean res = TRUE;

    OCI_CHECK_THREAD_ENABLED(FALSE);

    OCI_CHECK_PTR(OCI_IPC_THREAD, thread, FALSE);
    OCI_CHECK_PTR(OCI_IPC_PROC, proc, FALSE);

    thread->proc = proc;
    thread->arg  = arg;

    OCI_CALL3
    (
        res, thread->err, 
        
        OCIThreadCreate(OCILib.env, thread->err, OCI_ThreadProc,
                        thread, thread->id, thread->handle)
    )

    OCI_RESULT(res);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_ThreadJoin
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_ThreadJoin(OCI_Thread *thread)
{
    boolean res = TRUE;

    OCI_CHECK_THREAD_ENABLED(FALSE);

    OCI_CHECK_PTR(OCI_IPC_THREAD, thread, FALSE);

    OCI_CALL3
    (
        res, thread->err, 
        
        OCIThreadJoin(OCILib.env, thread->err, thread->handle)
    )

    OCI_RESULT(res);

    return res;
}
