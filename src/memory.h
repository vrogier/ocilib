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

void * MemAlloc
(
    int      ptr_type,
    size_t   block_size,
    size_t   block_count,
    boolean  zero_fill
);

void * MemRealloc
(
    void   * ptr_mem,
    int      ptr_type,
    size_t   block_size,
    size_t   block_count,
    boolean  zero_fill
);

void MemFree
(
    void   * ptr_mem
);

boolean MemHandleAlloc
(
    CONST dvoid *parenth,
    dvoid      **hndlpp,
    ub4          type
);

boolean MemHandleFree
(
    dvoid* hndlp,
    ub4    type
);

boolean MemDescriptorAlloc
(
    CONST dvoid *parenth,
    dvoid      **descpp,
    ub4          type
);

boolean MemDescriptorArrayAlloc
(
    CONST dvoid *parenth,
    dvoid      **descpp,
    ub4          type,
    ub4          nb_elem
);

boolean MemDescriptorFree
(
    dvoid *descp,
    ub4   type
);

boolean MemDescriptorArrayFree
(
    dvoid   **descp,
    ub4       type,
    ub4       nb_elem
);

sword MemObjectNew
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

sword MemObjectFree
(
    OCIEnv* env,
    OCIError* err,
    dvoid* instance,
    ub2       flags
);

void * MemAllocOracleClient
(
    void  *ctxp,
    size_t size
);

void * MemReallocOracleClient
(
    void  *ctxp,
    void  *memptr,
    size_t newsize
);

void MemFreeOracleClient
(
    void *ctxp,
    void *memptr
);

void MemUpdateBytes
(
    int      type,
    big_int  size
);

#endif /* OCILIB_MEMORY_H_INCLUDED */
