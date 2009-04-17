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
 * $Id: mutex.c, v 3.2.0 2009/04/20 00:00 Vince $
 * ------------------------------------------------------------------------ */

#include "ocilib_internal.h"

/* ************************************************************************ *
 *                            PRIVATE FUNCTIONS
 * ************************************************************************ */

/* ------------------------------------------------------------------------ *
 * OCI_MutexCreateInternal
 * ------------------------------------------------------------------------ */

OCI_Mutex * OCI_MutexCreateInternal(void)
{
    OCI_Mutex *mutex = NULL;
    boolean res      = TRUE;

    /* allocate mutex structure */

    mutex = (OCI_Mutex *) OCI_MemAlloc(OCI_IPC_MUTEX, sizeof(*mutex), 1, TRUE);

    if (mutex != NULL)
    {
        /* allocate error handle */

        res = (OCI_SUCCESS == OCI_HandleAlloc(OCILib.env, 
                                              (dvoid **) (void *) &mutex->err,
                                              OCI_HTYPE_ERROR, 0,
                                              (dvoid **) NULL));

        /* allocate mutex handle */

        OCI_CALL3
        (
            res, mutex->err,
            
            OCIThreadMutexInit(OCILib.env, mutex->err, &mutex->handle)
        )
    }
    else
        res = FALSE;

    if (res == FALSE)
    {
        OCI_MutexFree(mutex);
        mutex = NULL;
    }

    return mutex;
}

/* ************************************************************************ *
 *                            PUBLIC FUNCTIONS
 * ************************************************************************ */

/* ------------------------------------------------------------------------ *
 * OCI_MutexCreate
 * ------------------------------------------------------------------------ */

OCI_Mutex * OCI_API OCI_MutexCreate(void)
{
    OCI_Mutex *mutex = NULL;

    OCI_CHECK_INITIALIZED(NULL);

    mutex = OCI_MutexCreateInternal();

    OCI_RESULT(mutex != NULL);

    return mutex;
}

/* ------------------------------------------------------------------------ *
 * OCI_MutexFree
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_MutexFree(OCI_Mutex *mutex)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_MUTEX, mutex, FALSE);

    /* close mutex handle */

    if (mutex->handle != NULL)
    {
        OCI_CALL0
        (
            res, mutex->err, 
            
            OCIThreadMutexDestroy(OCILib.env, mutex->err, &mutex->handle)
        )
    }

    /* close error handle */

    if (mutex->err != NULL)
    {
        OCI_HandleFree(mutex->err, OCI_HTYPE_ERROR);
    }

    /* free mutex structure */

    OCI_FREE(mutex);

    OCI_RESULT(res);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_MutexAcquire
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_MutexAcquire(OCI_Mutex *mutex)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_MUTEX, mutex, FALSE);

    OCI_CALL3
    (
        res, mutex->err, 
        
        OCIThreadMutexAcquire(OCILib.env, mutex->err, mutex->handle)
    )
    
    OCI_RESULT(res);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_MutexRelease
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_MutexRelease(OCI_Mutex *mutex)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_MUTEX, mutex, FALSE);

    OCI_CALL3
    (
        res, mutex->err, 
        
        OCIThreadMutexRelease(OCILib.env, mutex->err, mutex->handle)
    )
  
    OCI_RESULT(res);

    return TRUE;
}
