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

#ifndef OCILIB_MEMORY_H_INCLUDED
#define OCILIB_MEMORY_H_INCLUDED

#include "types.h"

OCI_SYM_LOCAL void * OcilibMemoryAlloc
(
    int     ptr_type,
    size_t  block_size,
    size_t  block_count,
    boolean zero_fill
);

OCI_SYM_LOCAL void * OcilibMemoryRealloc
(
    void  * ptr_mem,
    int     ptr_type,
    size_t  block_size,
    size_t  block_count,
    boolean zero_fill
);

OCI_SYM_LOCAL void OcilibMemoryFree
(
    void * ptr_mem
);

OCI_SYM_LOCAL boolean OcilibMemoryAllocHandle
(
    CONST dvoid *parenth,
    dvoid      **hndlpp,
    ub4          type
);

OCI_SYM_LOCAL boolean OcilibMemoryFreeHandle
(
    dvoid* hndlp,
    ub4    type
);

OCI_SYM_LOCAL boolean OcilibMemoryAllocDescriptor
(
    CONST dvoid *parenth,
    dvoid      **descpp,
    ub4          type
);

OCI_SYM_LOCAL boolean OcilibMemoryAllocDescriptorArray
(
    CONST dvoid *parenth,
    dvoid      **descpp,
    ub4          type,
    ub4          nb_elem
);

OCI_SYM_LOCAL boolean OcilibMemoryFreeDescriptor
(
    dvoid *descp,
    ub4    type
);

OCI_SYM_LOCAL boolean OcilibMemoryFreeDescriptorArray
(
    dvoid **descp,
    ub4     type,
    ub4     nb_elem
);

OCI_SYM_LOCAL sword OcilibMemoryAllocateObject
(
    OCIEnv          *env,
    OCIError        *err,
    CONST OCISvcCtx *svc,
    OCITypeCode      typecode,
    OCIType         *tdo,
    dvoid           *table,
    OCIDuration      duration,
    boolean          value,
    dvoid          **instance
);

OCI_SYM_LOCAL sword OcilibMemoryFreeObject
(
    OCIEnv  * env,
    OCIError* err,
    dvoid   * instance,
    ub2       flags
);

OCI_SYM_LOCAL void * OcilibMemoryAllocOracleCallback
(
    void  *ctxp,
    size_t size
);

OCI_SYM_LOCAL void * OcilibMemoryReallocOracleCallback
(
    void  *ctxp,
    void  *memptr,
    size_t newsize
);

OCI_SYM_LOCAL void OcilibMemoryFreeOracleCallback
(
    void *ctxp,
    void *memptr
);

#endif /* OCILIB_MEMORY_H_INCLUDED */
