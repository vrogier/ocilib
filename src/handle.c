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
 * OcilibHandleGetEnvironment
 * --------------------------------------------------------------------------------------------- */

const void * OcilibHandleGetEnvironment
(
    void
)
{
    GET_LIB_PROP(const void*, NULL, Env.env)
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibHandleGetContext
 * --------------------------------------------------------------------------------------------- */

const void * OcilibHandleGetContext
(
    OCI_Connection *con
)
{
    GET_PROP(const void*, NULL, OCI_IPC_CONNECTION, con, cxt)
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibHandleGetServer
 * --------------------------------------------------------------------------------------------- */

const void * OcilibHandleGetServer
(
    OCI_Connection *con
)
{
    GET_PROP(const void *, NULL, OCI_IPC_CONNECTION, con, svr)
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibHandleGetError
 * --------------------------------------------------------------------------------------------- */

const void * OcilibHandleGetError
(
    OCI_Connection *con
)
{
    GET_PROP(const void *, NULL, OCI_IPC_CONNECTION, con, err)
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibHandleGetSession
 * --------------------------------------------------------------------------------------------- */

const void * OcilibHandleGetSession
(
    OCI_Connection *con
)
{
    GET_PROP(const void *, NULL, OCI_IPC_CONNECTION, con, ses)
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibHandleGetTransaction
 * --------------------------------------------------------------------------------------------- */

const void * OcilibHandleGetTransaction
(
    OCI_Transaction *trans
)
{
    GET_PROP(const void *, NULL, OCI_IPC_TRANSACTION, trans, htr)
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibHandleGetStatement
 * --------------------------------------------------------------------------------------------- */

const void * OcilibHandleGetStatement
(
    OCI_Statement *stmt
)
{
    GET_PROP(const void *, NULL, OCI_IPC_STATEMENT, stmt, stmt)
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibHandleGetLob
 * --------------------------------------------------------------------------------------------- */

const void * OcilibHandleGetLob
(
    OCI_Lob *lob
)
{
    GET_PROP(const void *, NULL, OCI_IPC_LOB, lob, handle)
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibHandleGetFile
 * --------------------------------------------------------------------------------------------- */

const void * OcilibHandleGetFile
(
    OCI_File *file
)
{
    GET_PROP(const void *, NULL, OCI_IPC_FILE, file, handle)
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibHandleGetDate
 * --------------------------------------------------------------------------------------------- */

const void * OcilibHandleGetDate
(
    OCI_Date *date
)
{
    GET_PROP(const void *, NULL, OCI_IPC_DATE, date, handle)
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibHandleGetTimestamp
 * --------------------------------------------------------------------------------------------- */

const void * OcilibHandleGetTimestamp
(
    OCI_Timestamp *tmsp
)
{
    GET_PROP(const void *, NULL, OCI_IPC_TIMESTAMP, tmsp, handle)
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibHandleGetInterval
 * --------------------------------------------------------------------------------------------- */

const void * OcilibHandleGetInterval
(
    OCI_Interval *itv
)
{
    GET_PROP(const void *, NULL, OCI_IPC_INTERVAL, itv, handle)
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibHandleGetObject
 * --------------------------------------------------------------------------------------------- */

const void * OcilibHandleGetObject
(
    OCI_Object *obj
)
{
    GET_PROP(const void *, NULL, OCI_IPC_OBJECT, obj, handle)
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibHandleGetCollection
 * --------------------------------------------------------------------------------------------- */

const void * OcilibHandleGetColl
(
    OCI_Coll *coll
)
{
    GET_PROP(const void *, NULL, OCI_IPC_COLLECTION, coll, handle)
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibHandleGetReference
 * --------------------------------------------------------------------------------------------- */

const void * OcilibHandleGetReference
(
    OCI_Ref *ref
)
{
    GET_PROP(const void *, NULL, OCI_IPC_REF, ref, handle)
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibHandleGetMutex
 * --------------------------------------------------------------------------------------------- */

const void * OcilibHandleGetMutex
(
    OCI_Mutex *mutex
)
{
    GET_PROP(const void *, NULL, OCI_IPC_MUTEX, mutex, handle)
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibHandleGetThreadID
 * --------------------------------------------------------------------------------------------- */

const void * OcilibHandleGetThreadID
(
    OCI_Thread *thread
)
{
    GET_PROP(const void *, NULL, OCI_IPC_THREAD, thread, id)
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibHandleGetThread
 * --------------------------------------------------------------------------------------------- */

const void * OcilibHandleGetThread
(
    OCI_Thread *thread
)
{
    GET_PROP(const void *, NULL, OCI_IPC_THREAD, thread, handle)
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibHandleGetDirPathCtx
 * --------------------------------------------------------------------------------------------- */

const void * OcilibHandleGetDirPathCtx
(
    OCI_DirPath *dp
)
{
    GET_PROP(const void *, NULL, OCI_IPC_DIRPATH, dp, ctx)
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibHandleGetDirPathColArray
 * --------------------------------------------------------------------------------------------- */

const void * OcilibHandleGetDirPathColArray
(
    OCI_DirPath *dp
)
{
    GET_PROP(const void *, NULL, OCI_IPC_DIRPATH, dp, arr)
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibHandleGetDirPathStream
 * --------------------------------------------------------------------------------------------- */

const void * OcilibHandleGetDirPathStream
(
    OCI_DirPath *dp
)
{
    GET_PROP(const void *, NULL, OCI_IPC_DIRPATH, dp, strm)
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibHandleGetSubscription
 * --------------------------------------------------------------------------------------------- */

const void * OcilibHandleGetSubscription
(
    OCI_Subscription *sub
)
{
    GET_PROP(const void *, NULL, OCI_IPC_NOTIFY, sub, subhp)
}
