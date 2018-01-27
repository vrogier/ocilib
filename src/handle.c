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
 *                            PUBLIC FUNCTIONS
 * ********************************************************************************************* */

/* --------------------------------------------------------------------------------------------- *
 * OCI_HandleGetEnvironment
 * --------------------------------------------------------------------------------------------- */

const void * OCI_API OCI_HandleGetEnvironment
(
    void
)
{
    OCI_CALL_ENTER(const void *, NULL)
    OCI_CALL_CHECK_INITIALIZED()

    OCI_RETVAL = OCILib.env;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_HandleGetContext
 * --------------------------------------------------------------------------------------------- */

const void * OCI_API OCI_HandleGetContext
(
    OCI_Connection *con
)
{
    OCI_CALL_ENTER(const void *, NULL)
    OCI_CALL_CHECK_PTR(OCI_IPC_CONNECTION, con)

    OCI_RETVAL = con->cxt;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_HandleGetServer
 * --------------------------------------------------------------------------------------------- */

const void * OCI_API OCI_HandleGetServer
(
    OCI_Connection *con
)
{
    OCI_CALL_ENTER(const void *, NULL)
    OCI_CALL_CHECK_PTR(OCI_IPC_CONNECTION, con)

    OCI_RETVAL = con->svr;

    OCI_CALL_EXIT()

}

/* --------------------------------------------------------------------------------------------- *
 * OCI_HandleGetError
 * --------------------------------------------------------------------------------------------- */

const void * OCI_API OCI_HandleGetError
(
    OCI_Connection *con
)
{
    OCI_CALL_ENTER(const void *, NULL)
    OCI_CALL_CHECK_PTR(OCI_IPC_CONNECTION, con)

    OCI_RETVAL = con->err;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_HandleGetSession
 * --------------------------------------------------------------------------------------------- */

const void * OCI_API OCI_HandleGetSession
(
    OCI_Connection *con
)
{
    OCI_CALL_ENTER(const void *, NULL)
    OCI_CALL_CHECK_PTR(OCI_IPC_CONNECTION, con)

    OCI_RETVAL = con->ses;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_HandleGetTransaction
 * --------------------------------------------------------------------------------------------- */

const void * OCI_API OCI_HandleGetTransaction
(
    OCI_Transaction *trans
)
{
    OCI_CALL_ENTER(const void *, NULL)
    OCI_CALL_CHECK_PTR(OCI_IPC_TRANSACTION, trans)

    OCI_RETVAL = trans->htr;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_HandleGetStatement
 * --------------------------------------------------------------------------------------------- */

const void * OCI_API OCI_HandleGetStatement
(
    OCI_Statement *stmt
)
{
    OCI_CALL_ENTER(const void *, NULL)
    OCI_CALL_CHECK_PTR(OCI_IPC_STATEMENT, stmt)

    OCI_RETVAL = stmt->stmt;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_HandleGetLob
 * --------------------------------------------------------------------------------------------- */

const void * OCI_API OCI_HandleGetLob
(
    OCI_Lob *lob
)
{
    OCI_CALL_ENTER(const void *, NULL)
    OCI_CALL_CHECK_PTR(OCI_IPC_LOB, lob)

    OCI_RETVAL = lob->handle;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_HandleGetFile
 * --------------------------------------------------------------------------------------------- */

const void * OCI_API OCI_HandleGetFile
(
    OCI_File *file
)
{
    OCI_CALL_ENTER(const void *, NULL)
    OCI_CALL_CHECK_PTR(OCI_IPC_FILE, file)

    OCI_RETVAL = file->handle;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_HandleGetDate
 * --------------------------------------------------------------------------------------------- */

const void * OCI_API OCI_HandleGetDate
(
    OCI_Date *date
)
{
    OCI_CALL_ENTER(const void *, NULL)
    OCI_CALL_CHECK_PTR(OCI_IPC_DATE, date)

    OCI_RETVAL = date->handle;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_HandleGetTimestamp
 * --------------------------------------------------------------------------------------------- */

const void * OCI_API OCI_HandleGetTimestamp
(
    OCI_Timestamp *tmsp
)
{
    OCI_CALL_ENTER(const void *, NULL)
    OCI_CALL_CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp)

    OCI_RETVAL = tmsp->handle;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_HandleGetInterval
 * --------------------------------------------------------------------------------------------- */

const void * OCI_API OCI_HandleGetInterval
(
    OCI_Interval *itv
)
{
    OCI_CALL_ENTER(const void *, NULL)
    OCI_CALL_CHECK_PTR(OCI_IPC_INTERVAL, itv)

    OCI_RETVAL = itv->handle;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_HandleGetObject
 * --------------------------------------------------------------------------------------------- */

const void * OCI_API OCI_HandleGetObject
(
    OCI_Object *obj
)
{
    OCI_CALL_ENTER(const void *, NULL)
    OCI_CALL_CHECK_PTR(OCI_IPC_OBJECT, obj)

    OCI_RETVAL = obj->handle;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_HandleGetColl
 * --------------------------------------------------------------------------------------------- */

const void * OCI_API OCI_HandleGetColl
(
    OCI_Coll *coll
)
{
    OCI_CALL_ENTER(const void *, NULL)
    OCI_CALL_CHECK_PTR(OCI_IPC_COLLECTION, coll)

    OCI_RETVAL = coll->handle;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_HandleGetRef
 * --------------------------------------------------------------------------------------------- */

const void * OCI_API OCI_HandleGetRef
(
    OCI_Ref *ref
)
{
    OCI_CALL_ENTER(const void *, NULL)
    OCI_CALL_CHECK_PTR(OCI_IPC_REF, ref)

    OCI_RETVAL = ref->handle;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_HandleGetMutex
 * --------------------------------------------------------------------------------------------- */

const void * OCI_API OCI_HandleGetMutex
(
    OCI_Mutex *mutex
)
{
    OCI_CALL_ENTER(const void *, NULL)
    OCI_CALL_CHECK_PTR(OCI_IPC_MUTEX, mutex)

    OCI_RETVAL = mutex->handle;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_HandleGetThreadID
 * --------------------------------------------------------------------------------------------- */

const void * OCI_API OCI_HandleGetThreadID
(
    OCI_Thread *thread
)
{
    OCI_CALL_ENTER(const void *, NULL)
    OCI_CALL_CHECK_PTR(OCI_IPC_THREAD, thread)

    OCI_RETVAL = thread->id;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_HandleGetThread
 * --------------------------------------------------------------------------------------------- */

const void * OCI_API OCI_HandleGetThread
(
    OCI_Thread *thread
)
{
    OCI_CALL_ENTER(const void *, NULL)
    OCI_CALL_CHECK_PTR(OCI_IPC_THREAD, thread)

    OCI_RETVAL = thread->handle;

    OCI_CALL_EXIT()
 }

/* --------------------------------------------------------------------------------------------- *
 * OCI_HandleGetDirPathCtx
 * --------------------------------------------------------------------------------------------- */

const void * OCI_API OCI_HandleGetDirPathCtx
(
    OCI_DirPath *dp
)
{
    OCI_CALL_ENTER(const void *, NULL)
    OCI_CALL_CHECK_PTR(OCI_IPC_DIRPATH, dp)

    OCI_RETVAL = dp->ctx;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_HandleGetDirPathColArray
 * --------------------------------------------------------------------------------------------- */

const void * OCI_API OCI_HandleGetDirPathColArray
(
    OCI_DirPath *dp
)
{
    OCI_CALL_ENTER(const void *, NULL)
    OCI_CALL_CHECK_PTR(OCI_IPC_DIRPATH, dp)

    OCI_RETVAL = dp->arr;

    OCI_CALL_EXIT()

}

/* --------------------------------------------------------------------------------------------- *
 * OCI_HandleGetDirPathStream
 * --------------------------------------------------------------------------------------------- */

const void * OCI_API OCI_HandleGetDirPathStream
(
    OCI_DirPath *dp
)
{
    OCI_CALL_ENTER(const void *, NULL)
    OCI_CALL_CHECK_PTR(OCI_IPC_DIRPATH, dp)

    OCI_RETVAL = dp->strm;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_HandleGetSubscription
 * --------------------------------------------------------------------------------------------- */

const void * OCI_API OCI_HandleGetSubscription
(
    OCI_Subscription *sub
)
{
    OCI_CALL_ENTER(const void *, NULL)
    OCI_CALL_CHECK_PTR(OCI_IPC_NOTIFY, sub)

    OCI_RETVAL = sub->subhp;

    OCI_CALL_EXIT()
}

