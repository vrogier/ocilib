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

#include "memory.h"

#include "defs.h"
#include "exception.h"
#include "macros.h"
#include "mutex.h"

#define MUTEXED_CALL(exp)            \
                                     \
    if (Env.mem_mutex)               \
    {                                \
        MutexAcquire(Env.mem_mutex); \
    }                                \
                                     \
    (exp);                           \
                                     \
    if (Env.mem_mutex)               \
    {                                \
        MutexRelease(Env.mem_mutex); \
    }                                \

void MemoryUpdateBytes
(
    int     type,
    big_int size
);

/* --------------------------------------------------------------------------------------------- *
 * MemoryUpdateBytes
 * --------------------------------------------------------------------------------------------- */

void MemoryUpdateBytes
(
    int     type,
    big_int size
)
{
    if (OCI_IPC_ORACLE == type)
    {
        MUTEXED_CALL(Env.mem_bytes_oci += size)
    }
    else
    {
        MUTEXED_CALL(Env.mem_bytes_lib += size)
    }
}

/* --------------------------------------------------------------------------------------------- *
 * MemoryAlloc
 * --------------------------------------------------------------------------------------------- */

void * MemoryAlloc
(
    int     ptr_type,
    size_t  block_size,
    size_t  block_count,
    boolean zero_fill
)
{
    ENTER_FUNC
    (
        /* returns */ void*, NULL,
        /* context */ OCI_IPC_VOID, &Env
    )

    const size_t size = sizeof(OCI_MemoryBlock) + (block_size * block_count);

    OCI_MemoryBlock* block = (OCI_MemoryBlock *)malloc(size);

    if (NULL == block)
    {
        THROW(ExceptionMemory, ptr_type, size)
    }

    if (zero_fill)
    {
        memset(block, 0, size);
    }

    block->type = ptr_type;
    block->size = (unsigned int) size;

    MemoryUpdateBytes(block->type, block->size);

    SET_RETVAL(((unsigned char*)block) + sizeof(*block))

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * MemoryRealloc
 * --------------------------------------------------------------------------------------------- */

void * MemoryRealloc
(
    void  * ptr_mem,
    int     ptr_type,
    size_t  block_size,
    size_t  block_count,
    boolean zero_fill
)
{
    ENTER_FUNC
    (
        /* returns */ void*, NULL,
        /* context */ OCI_IPC_VOID, &Env
    )

    if (ptr_mem != NULL)
    {
        OCI_MemoryBlock* block = (OCI_MemoryBlock*)(((unsigned char*)ptr_mem) - sizeof(*block));

        const size_t size = sizeof(OCI_MemoryBlock) + (block_size * block_count);

        if (block->size < size)
        {
            void* ptr_new = realloc(block, size);

            if (NULL == ptr_new)
            {
                MemoryFree(ptr_mem);
                THROW(ExceptionMemory, ptr_type, size);
            }

            block = (OCI_MemoryBlock*)ptr_new;

            const big_int size_diff = (big_int)size - block->size;

            block->type = ptr_type;
            block->size = (unsigned int)size;

            if (zero_fill)
            {
                memset(((unsigned char*)block) + block->size, 0, size - block->size);
            }

            MemoryUpdateBytes(block->type, size_diff);
        }

        ptr_mem = ((unsigned char*)block) + sizeof(*block);
    }
    else
    {
        ptr_mem = MemoryAlloc(ptr_type, block_size, block_count, zero_fill);
    }

    SET_RETVAL(ptr_mem)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * MemoryFree
 * --------------------------------------------------------------------------------------------- */

void MemoryFree
(
    void * ptr_mem
)
{
    if (ptr_mem)
    {
        OCI_MemoryBlock *block = (OCI_MemoryBlock *)(((unsigned char*)ptr_mem) - sizeof(*block));

        if (block)
        {
            MemoryUpdateBytes(block->type, (big_int) 0 - block->size);

            free(block);
        }
    }
}

/* --------------------------------------------------------------------------------------------- *
 * MemoryAllocHandle
 * --------------------------------------------------------------------------------------------- */

boolean MemoryAllocHandle
(
    CONST dvoid *parenth,
    dvoid      **hndlpp,
    ub4          type
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_VOID, &Env
    )

    CHECK_NULL(hndlpp)

    const sword ret = OCIHandleAlloc(parenth, hndlpp, type, 0, NULL);

    if (OCI_SUCCESSFUL(ret))
    {
        MUTEXED_CALL(Env.nb_hndlp++)
        SET_SUCCESS()
    }

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * MemoryFreeHandle
 * --------------------------------------------------------------------------------------------- */

boolean MemoryFreeHandle
(
    dvoid *hndlp,
    ub4    type
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_VOID, &Env
    )

    CHECK_NULL(hndlp)
    const sword ret = OCIHandleFree(hndlp, type);

    if (OCI_SUCCESSFUL(ret))
    {
        MUTEXED_CALL(Env.nb_hndlp--)
        SET_SUCCESS()
    }

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * MemoryAllocDescriptor
 * --------------------------------------------------------------------------------------------- */

boolean MemoryAllocDescriptor
(
    CONST dvoid *parenth,
    dvoid      **descpp,
    ub4          type
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_VOID, &Env
    )

    CHECK_NULL(descpp)

    const sword ret = OCIDescriptorAlloc(parenth, descpp, type, 0, NULL);

    if (OCI_SUCCESSFUL(ret))
    {
        MUTEXED_CALL(Env.nb_descp++)
        SET_SUCCESS()
    }

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * MemoryAllocDescriptorArray
 * --------------------------------------------------------------------------------------------- */

boolean MemoryAllocDescriptorArray
(
    CONST dvoid *parenth,
    dvoid      **descpp,
    ub4          type,
    ub4          nb_elem
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_VOID, &Env
    )

    CHECK_NULL(descpp)

    sword ret = OCI_ERROR;

#if OCI_VERSION_COMPILE >= OCI_11_1

    if (Env.version_runtime >= OCI_11_1)
    {
        ret = OCIArrayDescriptorAlloc(parenth, descpp, type, nb_elem, 0, NULL);

    }
    else

#endif

    {
        for (ub4 i = 0; (i < nb_elem) && (OCI_SUCCESS == ret); i++)
        {
            ret = OCIDescriptorAlloc(parenth, &descpp[i], type, 0, NULL);
        }
    }

    if (OCI_SUCCESSFUL(ret))
    {
        MUTEXED_CALL(Env.nb_descp += nb_elem)
        SET_SUCCESS()
    }

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * MemoryFreeDescriptor
 * --------------------------------------------------------------------------------------------- */

boolean MemoryFreeDescriptor
(
    dvoid *descp,
    ub4    type
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_VOID, &Env
    )

    CHECK_NULL(descp)

    const sword ret = OCIDescriptorFree(descp, type);

    if (OCI_SUCCESSFUL(ret))
    {
        MUTEXED_CALL(Env.nb_descp--)
        SET_SUCCESS()
    }

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * MemoryFreeDescriptor
 * --------------------------------------------------------------------------------------------- */

boolean MemoryFreeDescriptorArray
(
    dvoid **descp,
    ub4     type,
    ub4     nb_elem
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_VOID, &Env
    )

    CHECK_NULL(descp)

    sword ret = OCI_ERROR;

    if (descp)
    {

#if OCI_VERSION_COMPILE >= OCI_11_1

        if (Env.version_runtime >= OCI_11_1)
        {
            ret = OCIArrayDescriptorFree(descp, type);
        }
        else

#endif

        {
            for (ub4 i = 0; (i < nb_elem) && (OCI_SUCCESS == ret); i++)
            {
                ret = OCIDescriptorFree(descp[i], type);
            }
        }
    }

    if (OCI_SUCCESSFUL(ret))
    {
        MUTEXED_CALL(Env.nb_descp -= nb_elem)
        SET_SUCCESS()
    }

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * MemoryAllocateObject
 * --------------------------------------------------------------------------------------------- */

boolean MemoryAllocateObject
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
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_VOID, &Env
    )

    CHECK_OCI
    (
        err,
        OCIObjectNew,
        env, err, svc, typecode, tdo, table,
        duration, value, instance
    )

    SET_SUCCESS()

    MUTEXED_CALL(Env.nb_objinst++)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * MemoryFreeObject
 * --------------------------------------------------------------------------------------------- */

boolean MemoryFreeObject
(
    OCIEnv   *env,
    OCIError *err,
    dvoid    *instance,
    ub2       flags
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_VOID, &Env
    )

    CHECK_PTR(OCI_IPC_VOID, instance)

    CHECK_OCI
    (
        err,
        OCIObjectFree,
        env, err, instance, flags
    )

    SET_SUCCESS()

    MUTEXED_CALL(Env.nb_objinst--)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
* MemoryAllocOracleCallback
* --------------------------------------------------------------------------------------------- */

void * MemoryAllocOracleCallback
(
    void  *ctxp,
    size_t size
)
{
    OCI_NOT_USED(ctxp)

    return MemoryAlloc(OCI_IPC_ORACLE, size, 1, FALSE);
}

/* --------------------------------------------------------------------------------------------- *
* MemoryReallocOracleCallback
* --------------------------------------------------------------------------------------------- */

void * MemoryReallocOracleCallback
(
    void  *ctxp,
    void  *memptr,
    size_t newsize
)
{
    OCI_NOT_USED(ctxp)

    return MemoryRealloc(memptr, OCI_IPC_ORACLE, newsize, 1, FALSE);
}

/* --------------------------------------------------------------------------------------------- *
* MemoryFreeOracleCallback
* --------------------------------------------------------------------------------------------- */

void MemoryFreeOracleCallback
(
    void *ctxp,
    void *memptr
)
{
    OCI_NOT_USED(ctxp)

    MemoryFree(memptr);
}
