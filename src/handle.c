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
    GET_LIB_PROP(const void*, NULL, Env.env)
}

/* --------------------------------------------------------------------------------------------- *
 * HandleGetContext
 * --------------------------------------------------------------------------------------------- */

const void * HandleGetContext
(
    OCI_Connection *con
)
{
    GET_PROP(const void*, NULL, OCI_IPC_CONNECTION, con, cxt)
}

/* --------------------------------------------------------------------------------------------- *
 * HandleGetServer
 * --------------------------------------------------------------------------------------------- */

const void * HandleGetServer
(
    OCI_Connection *con
)
{
    GET_PROP(const void *, NULL, OCI_IPC_CONNECTION, con, svr)
}

/* --------------------------------------------------------------------------------------------- *
 * HandleGetError
 * --------------------------------------------------------------------------------------------- */

const void * HandleGetError
(
    OCI_Connection *con
)
{
    GET_PROP(const void *, NULL, OCI_IPC_CONNECTION, con, err)
}

/* --------------------------------------------------------------------------------------------- *
 * HandleGetSession
 * --------------------------------------------------------------------------------------------- */

const void * HandleGetSession
(
    OCI_Connection *con
)
{
    GET_PROP(const void *, NULL, OCI_IPC_CONNECTION, con, ses)
}

/* --------------------------------------------------------------------------------------------- *
 * HandleGetTransaction
 * --------------------------------------------------------------------------------------------- */

const void * HandleGetTransaction
(
    OCI_Transaction *trans
)
{
    GET_PROP(const void *, NULL, OCI_IPC_TRANSACTION, trans, htr)
}

/* --------------------------------------------------------------------------------------------- *
 * HandleGetStatement
 * --------------------------------------------------------------------------------------------- */

const void * HandleGetStatement
(
    OCI_Statement *stmt
)
{
    GET_PROP(const void *, NULL, OCI_IPC_STATEMENT, stmt, stmt)
}

/* --------------------------------------------------------------------------------------------- *
 * HandleGetLob
 * --------------------------------------------------------------------------------------------- */

const void * HandleGetLob
(
    OCI_Lob *lob
)
{
    GET_PROP(const void *, NULL, OCI_IPC_LOB, lob, handle)
}

/* --------------------------------------------------------------------------------------------- *
 * HandleGetFile
 * --------------------------------------------------------------------------------------------- */

const void * HandleGetFile
(
    OCI_File *file
)
{
    GET_PROP(const void *, NULL, OCI_IPC_FILE, file, handle)
}

/* --------------------------------------------------------------------------------------------- *
 * HandleGetDate
 * --------------------------------------------------------------------------------------------- */

const void * HandleGetDate
(
    OCI_Date *date
)
{
    GET_PROP(const void *, NULL, OCI_IPC_DATE, date, handle)
}

/* --------------------------------------------------------------------------------------------- *
 * HandleGetTimestamp
 * --------------------------------------------------------------------------------------------- */

const void * HandleGetTimestamp
(
    OCI_Timestamp *tmsp
)
{
    GET_PROP(const void *, NULL, OCI_IPC_TIMESTAMP, tmsp, handle)
}

/* --------------------------------------------------------------------------------------------- *
 * HandleGetInterval
 * --------------------------------------------------------------------------------------------- */

const void * HandleGetInterval
(
    OCI_Interval *itv
)
{
    GET_PROP(const void *, NULL, OCI_IPC_INTERVAL, itv, handle)
}

/* --------------------------------------------------------------------------------------------- *
 * HandleGetObject
 * --------------------------------------------------------------------------------------------- */

const void * HandleGetObject
(
    OCI_Object *obj
)
{
    GET_PROP(const void *, NULL, OCI_IPC_OBJECT, obj, handle)
}

/* --------------------------------------------------------------------------------------------- *
 * HandleGetCollection
 * --------------------------------------------------------------------------------------------- */

const void * HandleGetColl
(
    OCI_Coll *coll
)
{
    GET_PROP(const void *, NULL, OCI_IPC_COLLECTION, coll, handle)
}

/* --------------------------------------------------------------------------------------------- *
 * HandleGetReference
 * --------------------------------------------------------------------------------------------- */

const void * HandleGetReference
(
    OCI_Ref *ref
)
{
    GET_PROP(const void *, NULL, OCI_IPC_REF, ref, handle)
}

/* --------------------------------------------------------------------------------------------- *
 * HandleGetMutex
 * --------------------------------------------------------------------------------------------- */

const void * HandleGetMutex
(
    OCI_Mutex *mutex
)
{
    GET_PROP(const void *, NULL, OCI_IPC_MUTEX, mutex, handle)
}

/* --------------------------------------------------------------------------------------------- *
 * HandleGetThreadID
 * --------------------------------------------------------------------------------------------- */

const void * HandleGetThreadID
(
    OCI_Thread *thread
)
{
    GET_PROP(const void *, NULL, OCI_IPC_THREAD, thread, id)
}

/* --------------------------------------------------------------------------------------------- *
 * HandleGetThread
 * --------------------------------------------------------------------------------------------- */

const void * HandleGetThread
(
    OCI_Thread *thread
)
{
    GET_PROP(const void *, NULL, OCI_IPC_THREAD, thread, handle)
}

/* --------------------------------------------------------------------------------------------- *
 * HandleGetDirPathCtx
 * --------------------------------------------------------------------------------------------- */

const void * HandleGetDirPathCtx
(
    OCI_DirPath *dp
)
{
    GET_PROP(const void *, NULL, OCI_IPC_DIRPATH, dp, ctx)
}

/* --------------------------------------------------------------------------------------------- *
 * HandleGetDirPathColArray
 * --------------------------------------------------------------------------------------------- */

const void * HandleGetDirPathColArray
(
    OCI_DirPath *dp
)
{
    GET_PROP(const void *, NULL, OCI_IPC_DIRPATH, dp, arr)
}

/* --------------------------------------------------------------------------------------------- *
 * HandleGetDirPathStream
 * --------------------------------------------------------------------------------------------- */

const void * HandleGetDirPathStream
(
    OCI_DirPath *dp
)
{
    GET_PROP(const void *, NULL, OCI_IPC_DIRPATH, dp, strm)
}

/* --------------------------------------------------------------------------------------------- *
 * HandleGetSubscription
 * --------------------------------------------------------------------------------------------- */

const void * HandleGetSubscription
(
    OCI_Subscription *sub
)
{
    GET_PROP(const void *, NULL, OCI_IPC_NOTIFY, sub, subhp)
}
