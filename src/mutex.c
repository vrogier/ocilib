/*
    +-----------------------------------------------------------------------------------------+
    |                                                                                         |
    |                               OCILIB - C Driver for Oracle                              |
    |                                                                                         |
    |                                (C Wrapper for Oracle OCI)                               |
    |                                                                                         |
    |                              Website : http://www.ocilib.net                            |
    |                                                                                         |
    |             Copyright (c) 2007-2015 Vincent ROGIER <vince.rogier@ocilib.net>            |
    |                                                                                         |
    +-----------------------------------------------------------------------------------------+
    |                                                                                         |
    |             This library is free software; you can redistribute it and/or               |
    |             modify it under the terms of the GNU Lesser General Public                  |
    |             License as published by the Free Software Foundation; either                |
    |             version 2 of the License, or (at your option) any later version.            |
    |                                                                                         |
    |             This library is distributed in the hope that it will be useful,             |
    |             but WITHOUT ANY WARRANTY; without even the implied warranty of              |
    |             MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU           |
    |             Lesser General Public License for more details.                             |
    |                                                                                         |
    |             You should have received a copy of the GNU Lesser General Public            |
    |             License along with this library; if not, write to the Free                  |
    |             Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.          |
    |                                                                                         |
    +-----------------------------------------------------------------------------------------+
*/

/* --------------------------------------------------------------------------------------------- *
 * $Id: mutex.c, Vincent Rogier $
 * --------------------------------------------------------------------------------------------- */

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
