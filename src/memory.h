/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2026 Vincent ROGIER <vince.rogier@gmail.com>
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

#ifndef OCILIB_MEMORY_H_INCLUDED
#define OCILIB_MEMORY_H_INCLUDED

#include "types.h"

OCI_SYM_LOCAL void * OcilibMemoryAlloc
(
    OCI_Environment *env,
    OCI_PointerType  ptr_type,
    OCI_Size         block_size,
    OCI_Size         block_count,
    OCI_Boolean      zero_fill
);

OCI_SYM_LOCAL void * OcilibMemoryRealloc
(
    OCI_Environment *env,
    void           * ptr_mem,
    OCI_PointerType  ptr_type,
    OCI_Size         block_size,
    OCI_Size         block_count,
    OCI_Boolean      zero_fill
);

OCI_SYM_LOCAL void OcilibMemoryFree
(
    OCI_Environment *env,
    void           * ptr_mem
);

OCI_SYM_LOCAL OCI_Boolean OcilibMemoryAllocHandle
(
    OCI_Environment *env,
    CONST dvoid     *parenth,
    dvoid          **hndlpp,
    ub4              type
);

OCI_SYM_LOCAL OCI_Boolean OcilibMemoryFreeHandle
(
    OCI_Environment *env,
    dvoid           *hndlp,
    ub4              type
);

OCI_SYM_LOCAL OCI_Boolean OcilibMemoryAllocDescriptor
(
    OCI_Environment *env,
    CONST dvoid     *parenth,
    dvoid          **descpp,
    ub4              type
);

OCI_SYM_LOCAL OCI_Boolean OcilibMemoryAllocDescriptorArray
(
    OCI_Environment *env,
    CONST dvoid     *parenth,
    dvoid          **descpp,
    ub4              type,
    ub4              nb_elem
);

OCI_SYM_LOCAL OCI_Boolean OcilibMemoryFreeDescriptor
(
    OCI_Environment *env,
    dvoid           *descp,
    ub4              type
);

OCI_SYM_LOCAL OCI_Boolean OcilibMemoryFreeDescriptorArray
(
    OCI_Environment *env,
    dvoid          **descp,
    ub4              type,
    ub4              nb_elem
);

OCI_SYM_LOCAL sword OcilibMemoryAllocateObject
(
    OCI_Environment *libenv,
    OCIEnv          *env,
    OCIError        *err,
    CONST OCISvcCtx *svc,
    OCITypeCode      typecode,
    OCIType         *tdo,
    dvoid           *table,
    OCIDuration      duration,
    OCI_Boolean      value,
    dvoid          **instance
);

OCI_SYM_LOCAL sword OcilibMemoryFreeObject
(
    OCI_Environment *libenv,
    OCIEnv          *env,
    OCIError        *err,
    dvoid           *instance,
    ub2              flags
);

OCI_SYM_LOCAL void * OcilibMemoryAllocOracleCallback
(
    void    *ctxp,
    OCI_Size size
);

OCI_SYM_LOCAL void * OcilibMemoryReallocOracleCallback
(
    void    *ctxp,
    void    *memptr,
    OCI_Size newsize
);

OCI_SYM_LOCAL void OcilibMemoryFreeOracleCallback
(
    void *ctxp,
    void *memptr
);

#endif /* OCILIB_MEMORY_H_INCLUDED */
