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
 * $Id: handle.c, v 3.2.0 2009/04/20 00:00 Vince $
 * ------------------------------------------------------------------------ */

#include "ocilib_internal.h"

/* ************************************************************************ *
 *                            PUBLIC FUNCTIONS
 * ************************************************************************ */

/* ------------------------------------------------------------------------ *
 * OCI_HandleGetEnvironment
 * ------------------------------------------------------------------------ */

const void * OCI_API OCI_HandleGetEnvironment(void)
{
    return OCILib.env;
}

/* ------------------------------------------------------------------------ *
 * OCI_HandleGetContext
 * ------------------------------------------------------------------------ */

const void * OCI_API OCI_HandleGetContext(OCI_Connection *con)
{
    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, NULL);

    OCI_RESULT(TRUE);

    return con->cxt;
}

/* ------------------------------------------------------------------------ *
 * OCI_HandleGetServer
 * ------------------------------------------------------------------------ */

const void * OCI_API OCI_HandleGetServer(OCI_Connection *con)
{
    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, NULL);

    OCI_RESULT(TRUE);

    return con->svr;
}

/* ------------------------------------------------------------------------ *
 * OCI_HandleGetError
 * ------------------------------------------------------------------------ */

const void * OCI_API OCI_HandleGetError(OCI_Connection *con)
{
    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, NULL);

    OCI_RESULT(TRUE);

    return con->err;
}

/* ------------------------------------------------------------------------ *
 * OCI_HandleGetSession
 * ------------------------------------------------------------------------ */

const void * OCI_API OCI_HandleGetSession(OCI_Connection *con)
{
    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, NULL);

    OCI_RESULT(TRUE);

    return con->ses;
}

/* ------------------------------------------------------------------------ *
 * OCI_HandleGetTransaction
 * ------------------------------------------------------------------------ */

const void * OCI_API OCI_HandleGetTransaction(OCI_Transaction *trans)
{
    OCI_CHECK_PTR(OCI_IPC_TRANSACTION, trans, NULL);

    OCI_RESULT(TRUE);

    return trans->htr;
}

/* ------------------------------------------------------------------------ *
 * OCI_HandleGetStatement
 * ------------------------------------------------------------------------ */

const void * OCI_API OCI_HandleGetStatement(OCI_Statement *stmt)
{
    OCI_CHECK_PTR(OCI_IPC_STATEMENT, stmt, NULL);

    OCI_RESULT(TRUE);

    return stmt->stmt;
}

/* ------------------------------------------------------------------------ *
 * OCI_HandleGetLob
 * ------------------------------------------------------------------------ */

const void * OCI_API OCI_HandleGetLob(OCI_Lob *lob)
{
    OCI_CHECK_PTR(OCI_IPC_LOB, lob,NULL);

    OCI_RESULT(TRUE);

    return lob->handle;
}

/* ------------------------------------------------------------------------ *
 * OCI_HandleGetFile
 * ------------------------------------------------------------------------ */

const void * OCI_API OCI_HandleGetFile(OCI_File *file)
{
    OCI_CHECK_PTR(OCI_IPC_FILE, file, NULL);

    OCI_RESULT(TRUE);

    return file->handle;
}

/* ------------------------------------------------------------------------ *
 * OCI_HandleGetDate
 * ------------------------------------------------------------------------ */

const void * OCI_API OCI_HandleGetDate(OCI_Date *date)
{
    OCI_CHECK_PTR(OCI_IPC_DATE, date, NULL);

    OCI_RESULT(TRUE);

    return date->handle;
}

/* ------------------------------------------------------------------------ *
 * OCI_HandleGetTimestamp
 * ------------------------------------------------------------------------ */

const void * OCI_API OCI_HandleGetTimestamp(OCI_Timestamp *tmsp)
{
    OCI_CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp, NULL);

    OCI_RESULT(TRUE);

    return tmsp->handle;
}

/* ------------------------------------------------------------------------ *
 * OCI_HandleGetInterval
 * ------------------------------------------------------------------------ */

const void * OCI_API OCI_HandleGetInterval(OCI_Interval *itv)
{
    OCI_CHECK_PTR(OCI_IPC_INTERVAL, itv, NULL);

    OCI_RESULT(TRUE);

    return itv->handle;
}

/* ------------------------------------------------------------------------ *
 * OCI_HandleGetObject
 * ------------------------------------------------------------------------ */

const void * OCI_API OCI_HandleGetObject(OCI_Object *obj)
{
    OCI_CHECK_PTR(OCI_IPC_OBJECT, obj, NULL);

    OCI_RESULT(TRUE);

    return obj->handle;
}

/* ------------------------------------------------------------------------ *
 * OCI_HandleGetColl
 * ------------------------------------------------------------------------ */

const void * OCI_API OCI_HandleGetColl(OCI_Coll *coll)
{
    OCI_CHECK_PTR(OCI_IPC_COLLECTION, coll, NULL);

    OCI_RESULT(TRUE);

    return coll->handle;
}

/* ------------------------------------------------------------------------ *
 * OCI_HandleGetRef
 * ------------------------------------------------------------------------ */

const void * OCI_API OCI_HandleGetRef(OCI_Ref *ref)
{
    OCI_CHECK_PTR(OCI_IPC_REF, ref, NULL);

    OCI_RESULT(TRUE);

    return ref->handle;
}

/* ------------------------------------------------------------------------ *
 * OCI_HandleGetMutex
 * ------------------------------------------------------------------------ */

const void * OCI_API OCI_HandleGetMutex(OCI_Mutex *mutex)
{
    OCI_CHECK_PTR(OCI_IPC_MUTEX, mutex, NULL);

    OCI_RESULT(TRUE);

    return mutex->handle;
}

/* ------------------------------------------------------------------------ *
 * OCI_HandleGetThreadID
 * ------------------------------------------------------------------------ */

const void * OCI_API OCI_HandleGetThreadID(OCI_Thread *thread)
{
    OCI_CHECK_PTR(OCI_IPC_THREAD, thread, NULL);

    OCI_RESULT(TRUE);

    return thread->id;
}

/* ------------------------------------------------------------------------ *
 * OCI_HandleGetThread
 * ------------------------------------------------------------------------ */

const void * OCI_API OCI_HandleGetThread(OCI_Thread *thread)
{
    OCI_CHECK_PTR(OCI_IPC_THREAD, thread, NULL);

    OCI_RESULT(TRUE);

    return thread->handle;
}

/* ------------------------------------------------------------------------ *
 * OCI_HandleGetDirPathCtx
 * ------------------------------------------------------------------------ */

OCI_EXPORT const void * OCI_API OCI_HandleGetDirPathCtx(OCI_DirPath *dp)
{
    OCI_CHECK_PTR(OCI_IPC_DIRPATH, dp, NULL);

    OCI_RESULT(TRUE);

    return dp->ctx;
}

/* ------------------------------------------------------------------------ *
 * OCI_HandleGetDirPathColArray
 * ------------------------------------------------------------------------ */

OCI_EXPORT const void * OCI_API OCI_HandleGetDirPathColArray(OCI_DirPath *dp)
{
    OCI_CHECK_PTR(OCI_IPC_DIRPATH, dp, NULL);

    OCI_RESULT(TRUE);

    return dp->arr;
}

/* ------------------------------------------------------------------------ *
 * OCI_HandleGetDirPathStream
 * ------------------------------------------------------------------------ */

OCI_EXPORT const void * OCI_API OCI_HandleGetDirPathStream(OCI_DirPath *dp)
{
    OCI_CHECK_PTR(OCI_IPC_DIRPATH, dp, NULL);

    OCI_RESULT(TRUE);

    return dp->strm;
}
