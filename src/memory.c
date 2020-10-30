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

#define MUTEXED_CALL(exp)                   \
                                            \
    if (Env.mem_mutex)                      \
    {                                       \
        OcilibMutexAcquire(Env.mem_mutex);  \
    }                                       \
                                            \
    (exp);                                  \
                                            \
    if (Env.mem_mutex)                      \
    {                                       \
        OcilibMutexRelease(Env.mem_mutex);  \
    }                                       \


/* --------------------------------------------------------------------------------------------- *
 * OcilibMemoryUpdateBytes
 * --------------------------------------------------------------------------------------------- */

static void OcilibMemoryUpdateBytes
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
 * OcilibMemoryAlloc
 * --------------------------------------------------------------------------------------------- */

void * OcilibMemoryAlloc
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
        THROW(OcilibExceptionMemory, ptr_type, size)
    }

    if (zero_fill)
    {
        memset(block, 0, size);
    }

    block->type = ptr_type;
    block->size = (unsigned int) size;

    OcilibMemoryUpdateBytes(block->type, block->size);

    SET_RETVAL(((unsigned char*)block) + sizeof(*block))

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibMemoryRealloc
 * --------------------------------------------------------------------------------------------- */

void * OcilibMemoryRealloc
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
                OcilibMemoryFree(ptr_mem);
                THROW(OcilibExceptionMemory, ptr_type, size);
            }

            block = (OCI_MemoryBlock*)ptr_new;

            const big_int size_diff = (big_int)size - block->size;

            block->type = ptr_type;
            block->size = (unsigned int)size;

            if (zero_fill)
            {
                memset(((unsigned char*)block) + block->size, 0, size - block->size);
            }

            OcilibMemoryUpdateBytes(block->type, size_diff);
        }

        ptr_mem = ((unsigned char*)block) + sizeof(*block);
    }
    else
    {
        ptr_mem = OcilibMemoryAlloc(ptr_type, block_size, block_count, zero_fill);
    }

    SET_RETVAL(ptr_mem)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibMemoryFree
 * --------------------------------------------------------------------------------------------- */

void OcilibMemoryFree
(
    void * ptr_mem
)
{
    if (ptr_mem)
    {
        OCI_MemoryBlock *block = (OCI_MemoryBlock *)(((unsigned char*)ptr_mem) - sizeof(*block));

        if (block)
        {
            OcilibMemoryUpdateBytes(block->type, (big_int) 0 - block->size);

            free(block);
        }
    }
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibMemoryAllocHandle
 * --------------------------------------------------------------------------------------------- */

boolean OcilibMemoryAllocHandle
(
    CONST dvoid *parenth,
    dvoid      **hndlpp,
    ub4          type
)
{
    ENTER_FUNC_NO_CONTEXT
    (
        /* returns */ boolean, FALSE
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
 * OcilibMemoryFreeHandle
 * --------------------------------------------------------------------------------------------- */

boolean OcilibMemoryFreeHandle
(
    dvoid *hndlp,
    ub4    type
)
{
    ENTER_FUNC_NO_CONTEXT
    (
        /* returns */ boolean, FALSE
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
 * OcilibMemoryAllocDescriptor
 * --------------------------------------------------------------------------------------------- */

boolean OcilibMemoryAllocDescriptor
(
    CONST dvoid *parenth,
    dvoid      **descpp,
    ub4          type
)
{
    ENTER_FUNC_NO_CONTEXT
    (
        /* returns */ boolean, FALSE
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
 * OcilibMemoryAllocDescriptorArray
 * --------------------------------------------------------------------------------------------- */

boolean OcilibMemoryAllocDescriptorArray
(
    CONST dvoid *parenth,
    dvoid      **descpp,
    ub4          type,
    ub4          nb_elem
)
{
    ENTER_FUNC_NO_CONTEXT
    (
        /* returns */ boolean, FALSE
    )

    CHECK_NULL(descpp)

    sword ret = OCI_SUCCESS;

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
 * OcilibMemoryFreeDescriptor
 * --------------------------------------------------------------------------------------------- */

boolean OcilibMemoryFreeDescriptor
(
    dvoid *descp,
    ub4    type
)
{
    ENTER_FUNC_NO_CONTEXT
    (
        /* returns */ boolean, FALSE
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
 * OcilibMemoryFreeDescriptor
 * --------------------------------------------------------------------------------------------- */

boolean OcilibMemoryFreeDescriptorArray
(
    dvoid **descp,
    ub4     type,
    ub4     nb_elem
)
{
    ENTER_FUNC_NO_CONTEXT
    (
        /* returns */ boolean, FALSE
    )

    CHECK_NULL(descp)

    sword ret = OCI_SUCCESS;

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
 * OcilibMemoryAllocateObject
 * --------------------------------------------------------------------------------------------- */

boolean OcilibMemoryAllocateObject
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
 * OcilibMemoryFreeObject
 * --------------------------------------------------------------------------------------------- */

boolean OcilibMemoryFreeObject
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
* OcilibMemoryAllocOracleCallback
* --------------------------------------------------------------------------------------------- */

void * OcilibMemoryAllocOracleCallback
(
    void  *ctxp,
    size_t size
)
{
    OCI_NOT_USED(ctxp)

    return OcilibMemoryAlloc(OCI_IPC_ORACLE, size, 1, FALSE);
}

/* --------------------------------------------------------------------------------------------- *
* OcilibMemoryReallocOracleCallback
* --------------------------------------------------------------------------------------------- */

void * OcilibMemoryReallocOracleCallback
(
    void  *ctxp,
    void  *memptr,
    size_t newsize
)
{
    OCI_NOT_USED(ctxp)

    return OcilibMemoryRealloc(memptr, OCI_IPC_ORACLE, newsize, 1, FALSE);
}

/* --------------------------------------------------------------------------------------------- *
* OcilibMemoryFreeOracleCallback
* --------------------------------------------------------------------------------------------- */

void OcilibMemoryFreeOracleCallback
(
    void *ctxp,
    void *memptr
)
{
    OCI_NOT_USED(ctxp)

        OcilibMemoryFree(memptr);
}
