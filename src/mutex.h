/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2025 Vincent ROGIER <vince.rogier@ocilib.net>
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

#ifndef OCILIB_MUTEX_H_INCLUDED
#define OCILIB_MUTEX_H_INCLUDED

#include "types.h"

OCI_SYM_LOCAL OCI_Mutex * OcilibMutexCreateInternal
(
    void
);

OCI_SYM_LOCAL OCI_Mutex * OcilibMutexCreate
(
    void
);

OCI_SYM_LOCAL boolean OcilibMutexFree
(
    OCI_Mutex *mutex
);

OCI_SYM_LOCAL boolean OcilibMutexAcquire
(
    OCI_Mutex *mutex
);

OCI_SYM_LOCAL boolean OcilibMutexRelease
(
    OCI_Mutex *mutex
);

#endif /* OCILIB_MUTEX_H_INCLUDED */
