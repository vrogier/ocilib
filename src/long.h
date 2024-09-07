/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2024 Vincent ROGIER <vince.rogier@ocilib.net>
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

#ifndef OCILIB_LONG_H_INCLUDED
#define OCILIB_LONG_H_INCLUDED

#include "types.h"

OCI_SYM_LOCAL OCI_Long * OcilibLongInitialize
(
    OCI_Statement *stmt,
    OCI_Long      *lg,
    OCI_Define    *def,
    unsigned int   type
);

OCI_SYM_LOCAL OCI_Long * OcilibLongCreate
(
    OCI_Statement *stmt,
    unsigned int   type
);

OCI_SYM_LOCAL boolean OcilibLongFree
(
    OCI_Long *lg
);

OCI_SYM_LOCAL unsigned int OcilibLongGetType
(
    OCI_Long *lg
);

OCI_SYM_LOCAL unsigned int OcilibLongRead
(
    OCI_Long    *lg,
    void        *buffer,
    unsigned int len
);

OCI_SYM_LOCAL unsigned int OcilibLongWrite
(
    OCI_Long    *lg,
    void        *buffer,
    unsigned int len
);

OCI_SYM_LOCAL unsigned int OcilibLongGetSize
(
    OCI_Long *lg
);

OCI_SYM_LOCAL void * OcilibLongGetBuffer
(
    OCI_Long *lg
);

OCI_SYM_LOCAL boolean OcilibLongFinalizeDynamicFetch
(
    OCI_Long *lg
);

#endif /* OCILIB_LONG_H_INCLUDED */
