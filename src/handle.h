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

const void * HandleGetEnvironment
(
);

const void * HandleGetContext
(
    OCI_Connection *con
);

const void * HandleGetServer
(
    OCI_Connection *con
);

const void * HandleGetError
(
    OCI_Connection *con
);

const void * HandleGetSession
(
    OCI_Connection *con
);

const void * HandleGetTransaction
(
    OCI_Transaction *trans
);

const void * HandleGetStatement
(
    OCI_Statement *stmt
);

const void * HandleGetLob
(
    OCI_Lob     *lob
);

const void * HandleGetFile
(
    OCI_File    *file
);

const void * HandleGetDate
(
    OCI_Date    *date
);

const void * HandleGetTimestamp
(
    OCI_Timestamp *tmsp
);

const void * HandleGetInterval
(
    OCI_Interval *itv
);

const void * HandleGetObject
(
    OCI_Object  *obj
);

const void * HandleGetColl
(
    OCI_Coll *coll
);

const void * HandleGetRef
(
    OCI_Ref *ref
);

const void * HandleGetMutex
(
    OCI_Mutex   *mutex
);

const void * HandleGetThreadID
(
    OCI_Thread  *thread
);

const void * HandleGetThread
(
    OCI_Thread  *thread
);

const void * HandleGetDirPathCtx
(
    OCI_DirPath *dp
);

const void * HandleGetDirPathColArray
(
    OCI_DirPath *dp
);

const void * HandleGetDirPathStream
(
    OCI_DirPath *dp
);

const void * HandleGetSubscription
(
    OCI_Subscription *sub
);

#endif /* OCILIB_HANDLE_H_INCLUDED */
