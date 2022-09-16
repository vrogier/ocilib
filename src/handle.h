/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2022 Vincent ROGIER <vince.rogier@ocilib.net>
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

OCI_SYM_LOCAL const void * OcilibHandleGetEnvironment
(
    void
);

OCI_SYM_LOCAL const void * OcilibHandleGetContext
(
    OCI_Connection *con
);

OCI_SYM_LOCAL const void * OcilibHandleGetServer
(
    OCI_Connection *con
);

OCI_SYM_LOCAL const void * OcilibHandleGetError
(
    OCI_Connection *con
);

OCI_SYM_LOCAL const void * OcilibHandleGetSession
(
    OCI_Connection *con
);

OCI_SYM_LOCAL const void * OcilibHandleGetTransaction
(
    OCI_Transaction *trans
);

OCI_SYM_LOCAL const void * OcilibHandleGetStatement
(
    OCI_Statement *stmt
);

OCI_SYM_LOCAL const void * OcilibHandleGetLob
(
    OCI_Lob *lob
);

OCI_SYM_LOCAL const void * OcilibHandleGetFile
(
    OCI_File *file
);

OCI_SYM_LOCAL const void * OcilibHandleGetDate
(
    OCI_Date *date
);

OCI_SYM_LOCAL const void * OcilibHandleGetTimestamp
(
    OCI_Timestamp *tmsp
);

OCI_SYM_LOCAL const void * OcilibHandleGetInterval
(
    OCI_Interval *itv
);

OCI_SYM_LOCAL const void * OcilibHandleGetObject
(
    OCI_Object *obj
);

OCI_SYM_LOCAL const void * OcilibHandleGetColl
(
    OCI_Coll *coll
);

OCI_SYM_LOCAL const void * OcilibHandleGetReference
(
    OCI_Ref *ref
);

OCI_SYM_LOCAL const void * OcilibHandleGetMutex
(
    OCI_Mutex *mutex
);

OCI_SYM_LOCAL const void * OcilibHandleGetThreadID
(
    OCI_Thread *thread
);

OCI_SYM_LOCAL const void * OcilibHandleGetThread
(
    OCI_Thread *thread
);

OCI_SYM_LOCAL const void * OcilibHandleGetDirPathCtx
(
    OCI_DirPath *dp
);

OCI_SYM_LOCAL const void * OcilibHandleGetDirPathColArray
(
    OCI_DirPath *dp
);

OCI_SYM_LOCAL const void * OcilibHandleGetDirPathStream
(
    OCI_DirPath *dp
);

OCI_SYM_LOCAL const void * OcilibHandleGetSubscription
(
    OCI_Subscription *sub
);

#endif /* OCILIB_HANDLE_H_INCLUDED */
