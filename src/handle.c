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

#include "handle.h"

#include "macros.h"

/* --------------------------------------------------------------------------------------------- *
 * HandleGetEnvironment
 * --------------------------------------------------------------------------------------------- */

const void * HandleGetEnvironment
(
    void
)
{
    CALL_ENTER(const void *, NULL)
    CHECK_INITIALIZED()

    RETVAL = OCILib.env;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * HandleGetContext
 * --------------------------------------------------------------------------------------------- */

const void * HandleGetContext
(
    OCI_Connection *con
)
{
    GET_PROP(const void*, NULL, OCI_IPC_CONNECTION, con, cxt, NULL, NULL, NULL)
}

/* --------------------------------------------------------------------------------------------- *
 * HandleGetServer
 * --------------------------------------------------------------------------------------------- */

const void * HandleGetServer
(
    OCI_Connection *con
)
{
    GET_PROP(const void *, NULL, OCI_IPC_CONNECTION, con, svr, NULL, NULL, NULL)
}

/* --------------------------------------------------------------------------------------------- *
 * HandleGetError
 * --------------------------------------------------------------------------------------------- */

const void * HandleGetError
(
    OCI_Connection *con
)
{
    GET_PROP(const void *, NULL, OCI_IPC_CONNECTION, con, err, NULL, NULL, NULL)
}

/* --------------------------------------------------------------------------------------------- *
 * HandleGetSession
 * --------------------------------------------------------------------------------------------- */

const void * HandleGetSession
(
    OCI_Connection *con
)
{
    GET_PROP(const void *, NULL, OCI_IPC_CONNECTION, con, ses, NULL, NULL, NULL)
}

/* --------------------------------------------------------------------------------------------- *
 * HandleGetTransaction
 * --------------------------------------------------------------------------------------------- */

const void * HandleGetTransaction
(
    OCI_Transaction *trans
)
{
    GET_PROP(const void *, NULL, OCI_IPC_TRANSACTION, trans, htr, NULL, NULL, NULL)
}

/* --------------------------------------------------------------------------------------------- *
 * HandleGetStatement
 * --------------------------------------------------------------------------------------------- */

const void * HandleGetStatement
(
    OCI_Statement *stmt
)
{
    GET_PROP(const void *, NULL, OCI_IPC_STATEMENT, stmt, stmt, NULL, NULL, NULL)
}

/* --------------------------------------------------------------------------------------------- *
 * HandleGetLob
 * --------------------------------------------------------------------------------------------- */

const void * HandleGetLob
(
    OCI_Lob *lob
)
{
    GET_PROP(const void *, NULL, OCI_IPC_LOB, lob, handle, NULL, NULL, NULL)
}

/* --------------------------------------------------------------------------------------------- *
 * HandleGetFile
 * --------------------------------------------------------------------------------------------- */

const void * HandleGetFile
(
    OCI_File *file
)
{
    GET_PROP(const void *, NULL, OCI_IPC_FILE, file, handle, NULL, NULL, NULL)
}

/* --------------------------------------------------------------------------------------------- *
 * HandleGetDate
 * --------------------------------------------------------------------------------------------- */

const void * HandleGetDate
(
    OCI_Date *date
)
{
    GET_PROP(const void *, NULL, OCI_IPC_DATE, date, handle, NULL, NULL, NULL)
}

/* --------------------------------------------------------------------------------------------- *
 * HandleGetTimestamp
 * --------------------------------------------------------------------------------------------- */

const void * HandleGetTimestamp
(
    OCI_Timestamp *tmsp
)
{
    GET_PROP(const void *, NULL, OCI_IPC_TIMESTAMP, tmsp, handle, NULL, NULL, NULL)
}

/* --------------------------------------------------------------------------------------------- *
 * HandleGetInterval
 * --------------------------------------------------------------------------------------------- */

const void * HandleGetInterval
(
    OCI_Interval *itv
)
{
    GET_PROP(const void *, NULL, OCI_IPC_INTERVAL, itv, handle, NULL, NULL, NULL)
}

/* --------------------------------------------------------------------------------------------- *
 * HandleGetObject
 * --------------------------------------------------------------------------------------------- */

const void * HandleGetObject
(
    OCI_Object *obj
)
{
    GET_PROP(const void *, NULL, OCI_IPC_OBJECT, obj, handle, NULL, NULL, NULL)
}

/* --------------------------------------------------------------------------------------------- *
 * HandleGetCollection
 * --------------------------------------------------------------------------------------------- */

const void * HandleGetColl
(
    OCI_Coll *coll
)
{
    GET_PROP(const void *, NULL, OCI_IPC_COLLECTION, coll, handle, NULL, NULL, NULL)
}

/* --------------------------------------------------------------------------------------------- *
 * HandleGetReference
 * --------------------------------------------------------------------------------------------- */

const void * HandleGetReference
(
    OCI_Ref *ref
)
{
    GET_PROP(const void *, NULL, OCI_IPC_REF, ref, handle, NULL, NULL, NULL)
}

/* --------------------------------------------------------------------------------------------- *
 * HandleGetMutex
 * --------------------------------------------------------------------------------------------- */

const void * HandleGetMutex
(
    OCI_Mutex *mutex
)
{
    GET_PROP(const void *, NULL, OCI_IPC_MUTEX, mutex, handle, NULL, NULL, NULL)
}

/* --------------------------------------------------------------------------------------------- *
 * HandleGetThreadID
 * --------------------------------------------------------------------------------------------- */

const void * HandleGetThreadID
(
    OCI_Thread *thread
)
{
    GET_PROP(const void *, NULL, OCI_IPC_THREAD, thread, id, NULL, NULL, NULL)
}

/* --------------------------------------------------------------------------------------------- *
 * HandleGetThread
 * --------------------------------------------------------------------------------------------- */

const void * HandleGetThread
(
    OCI_Thread *thread
)
{
    GET_PROP(const void *, NULL, OCI_IPC_THREAD, thread, handle, NULL, NULL, NULL)
}

/* --------------------------------------------------------------------------------------------- *
 * HandleGetDirPathCtx
 * --------------------------------------------------------------------------------------------- */

const void * HandleGetDirPathCtx
(
    OCI_DirPath *dp
)
{
    GET_PROP(const void *, NULL, OCI_IPC_DIRPATH, dp, ctx, NULL, NULL, NULL)
}

/* --------------------------------------------------------------------------------------------- *
 * HandleGetDirPathColArray
 * --------------------------------------------------------------------------------------------- */

const void * HandleGetDirPathColArray
(
    OCI_DirPath *dp
)
{
    GET_PROP(const void *, NULL, OCI_IPC_DIRPATH, dp, arr, NULL, NULL, NULL)
}

/* --------------------------------------------------------------------------------------------- *
 * HandleGetDirPathStream
 * --------------------------------------------------------------------------------------------- */

const void * HandleGetDirPathStream
(
    OCI_DirPath *dp
)
{
    GET_PROP(const void *, NULL, OCI_IPC_DIRPATH, dp, strm, NULL, NULL, NULL)
}

/* --------------------------------------------------------------------------------------------- *
 * HandleGetSubscription
 * --------------------------------------------------------------------------------------------- */

const void * HandleGetSubscription
(
    OCI_Subscription *sub
)
{
    GET_PROP(const void *, NULL, OCI_IPC_NOTIFY, sub, subhp, NULL, NULL, NULL)
}
