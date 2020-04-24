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
 
#ifndef OCILIB_MEMORY_H_INCLUDED
#define OCILIB_MEMORY_H_INCLUDED

#include "types.h"

void * MemoryAlloc
(
    int      ptr_type,
    size_t   block_size,
    size_t   block_count,
    boolean  zero_fill
);

void * MemoryRealloc
(
    void   * ptr_mem,
    int      ptr_type,
    size_t   block_size,
    size_t   block_count,
    boolean  zero_fill
);

void MemoryFree
(
    void   * ptr_mem
);

boolean MemoryAllocHandle
(
    CONST dvoid *parenth,
    dvoid      **hndlpp,
    ub4          type
);

boolean MemoryFreeHandle
(
    dvoid* hndlp,
    ub4    type
);

boolean MemoryAllocDescriptor
(
    CONST dvoid *parenth,
    dvoid      **descpp,
    ub4          type
);

boolean MemoryAllocDescriptorArray
(
    CONST dvoid *parenth,
    dvoid      **descpp,
    ub4          type,
    ub4          nb_elem
);

boolean MemoryFreeDescriptor
(
    dvoid *descp,
    ub4   type
);

boolean MemoryFreeDescriptorArray
(
    dvoid   **descp,
    ub4       type,
    ub4       nb_elem
);

sword MemoryAllocateObject
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

sword MemoryFreeObject
(
    OCIEnv* env,
    OCIError* err,
    dvoid* instance,
    ub2       flags
);

void * MemoryAllocOracleCallback
(
    void  *ctxp,
    size_t size
);

void * MemoryReallocOracleCallback
(
    void  *ctxp,
    void  *memptr,
    size_t newsize
);

void MemoryFreeOracleCallback
(
    void *ctxp,
    void *memptr
);


#endif /* OCILIB_MEMORY_H_INCLUDED */
