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

#ifndef OCILIB_HANDLE_H_INCLUDED
#define OCILIB_HANDLE_H_INCLUDED

#include "types.h"

const void * OcilibHandleGetEnvironment
(
    void
);

const void * OcilibHandleGetContext
(
    OCI_Connection *con
);

const void * OcilibHandleGetServer
(
    OCI_Connection *con
);

const void * OcilibHandleGetError
(
    OCI_Connection *con
);

const void * OcilibHandleGetSession
(
    OCI_Connection *con
);

const void * OcilibHandleGetTransaction
(
    OCI_Transaction *trans
);

const void * OcilibHandleGetStatement
(
    OCI_Statement *stmt
);

const void * OcilibHandleGetLob
(
    OCI_Lob *lob
);

const void * OcilibHandleGetFile
(
    OCI_File *file
);

const void * OcilibHandleGetDate
(
    OCI_Date *date
);

const void * OcilibHandleGetTimestamp
(
    OCI_Timestamp *tmsp
);

const void * OcilibHandleGetInterval
(
    OCI_Interval *itv
);

const void * OcilibHandleGetObject
(
    OCI_Object *obj
);

const void * OcilibHandleGetColl
(
    OCI_Coll *coll
);

const void * OcilibHandleGetReference
(
    OCI_Ref *ref
);

const void * OcilibHandleGetMutex
(
    OCI_Mutex *mutex
);

const void * OcilibHandleGetThreadID
(
    OCI_Thread *thread
);

const void * OcilibHandleGetThread
(
    OCI_Thread *thread
);

const void * OcilibHandleGetDirPathCtx
(
    OCI_DirPath *dp
);

const void * OcilibHandleGetDirPathColArray
(
    OCI_DirPath *dp
);

const void * OcilibHandleGetDirPathStream
(
    OCI_DirPath *dp
);

const void * OcilibHandleGetSubscription
(
    OCI_Subscription *sub
);

#endif /* OCILIB_HANDLE_H_INCLUDED */
