/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2026 Vincent ROGIER <vince.rogier@ocilib.net>
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

#include <stdint.h>

#define MUTEXED_CALL(env, exp)         \
                                       \
        if (LIB_THREADED(env))         \
        {                              \
            mtx_lock(&(env)->mutex);   \
        }                              \
                                       \
        exp;                           \
                                       \
        if (LIB_THREADED(env))         \
        {                              \
            mtx_unlock(&(env)->mutex); \
        }

/* --------------------------------------------------------------------------------------------- *
 * OcilibMemoryUpdateBytes
 * --------------------------------------------------------------------------------------------- */

static void OcilibMemoryUpdateBytes
(
    OCI_Environment *env,
    OCI_PointerType  type,
    OCI_Int64        size
)
{
    (void) type;

    MUTEXED_CALL(env, env->mem_bytes_lib += (OCI_UInt64) size);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibMemoryComputeSize
 * --------------------------------------------------------------------------------------------- */

/* Computes sizeof(OCI_MemoryBlock) + block_size * block_count with overflow
 * detection, returning FALSE if the result is not representable in a OCI_Size.
 *
 * Without this check a wrapped product makes malloc() hand back a block far
 * smaller than the caller believes it obtained, turning every subsequent write
 * into a heap overflow. Callers pass sizes derived from column widths, string
 * lengths and bind array counts, so the inputs are not all trusted.
 */

static OCI_Boolean OcilibMemoryComputeSize
(
    OCI_Size  block_size,
    OCI_Size  block_count,
    OCI_Size *size
)
{
    *size = 0;

    if (block_size != 0 && block_count > (SIZE_MAX / block_size))
    {
        return FALSE;
    }

    {
        const OCI_Size requested = block_size * block_count;

        if (requested > (SIZE_MAX - sizeof(OCI_MemoryBlock)))
        {
            return FALSE;
        }

        *size = sizeof(OCI_MemoryBlock) + requested;
    }

    return TRUE;
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibMemoryAlloc
 * --------------------------------------------------------------------------------------------- */

void * OcilibMemoryAlloc
(
    OCI_Environment *env,
    OCI_PointerType  ptr_type,
    OCI_Size         block_size,
    OCI_Size         block_count,
    OCI_Boolean      zero_fill
)
{
    ENTER_FUNC
    (
        /* returns */ void*, NULL,
        /* context */ OCI_IPC_ENVIRONMENT, env
    );

    OCI_Size size = 0;

    if (!OcilibMemoryComputeSize(block_size, block_count, &size))
    {
        THROW(OcilibExceptionMemory, ptr_type, SIZE_MAX);
    }

    if (size == sizeof(OCI_MemoryBlock))
    {
        OCILIB_LOG(OCI_LOG_CAT_LIB, OCI_LOG_WARN,
                   OTEXT("zero-size allocation requested for type %d"),
                   (OCI_Int32) ptr_type);
    }

    OCI_MemoryBlock* block = (OCI_MemoryBlock *)malloc(size);

    if (NULL == block)
    {
        THROW(OcilibExceptionMemory, ptr_type, size);
    }

    if (zero_fill)
    {
        memset(block, 0, size);
    }

    block->type = ptr_type;
    block->size = size;

    OcilibMemoryUpdateBytes(env, block->type, (OCI_Int64) block->size);

    OCILIB_LOG(OCI_LOG_CAT_LIB, OCI_LOG_DEBUG, OTEXT("allocated %zu bytes for type %d"), size, (OCI_Int32) block->type);

    SET_RETVAL(((unsigned char*)block) + sizeof(*block));

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibMemoryRealloc
 * --------------------------------------------------------------------------------------------- */

void * OcilibMemoryRealloc
(
    OCI_Environment *env,
    void            *ptr_mem,
    OCI_PointerType  ptr_type,
    OCI_Size         block_size,
    OCI_Size         block_count,
    OCI_Boolean      zero_fill
)
{
    ENTER_FUNC
    (
        /* returns */ void*, NULL,
        /* context */ OCI_IPC_ENVIRONMENT, env
    );

    if (ptr_mem != NULL)
    {
        OCI_MemoryBlock* block = (OCI_MemoryBlock*)(((unsigned char*)ptr_mem) - sizeof(*block));

        OCI_Size size = 0;

        if (!OcilibMemoryComputeSize(block_size, block_count, &size))
        {
            OcilibMemoryFree(env, ptr_mem);
            THROW(OcilibExceptionMemory, ptr_type, SIZE_MAX);
        }

        if (block->size < size)
        {
            const OCI_Size old_size = block->size;

            void* ptr_new = realloc(block, size);

            if (NULL == ptr_new)
            {
                /* Contract: on failure the original block is released and NULL
                   is returned, so the caller must not keep using its previous
                   pointer. REALLOC_BUFFER() overwrites it with the NULL return,
                   so releasing here is what avoids a guaranteed leak. */

                OcilibMemoryFree(env, ptr_mem);
                THROW(OcilibExceptionMemory, ptr_type, size);
            }

            block = (OCI_MemoryBlock*)ptr_new;

            block->type = ptr_type;
            block->size = size;

            if (zero_fill)
            {
                memset(((unsigned char*)block) + old_size, 0, size - old_size);
            }

            OcilibMemoryUpdateBytes(env, block->type, (OCI_Int64) (size - old_size));
        }

        ptr_mem = ((unsigned char*)block) + sizeof(*block);
    }
    else
    {
        ptr_mem = OcilibMemoryAlloc(env, ptr_type, block_size, block_count, zero_fill);
    }

    SET_RETVAL(ptr_mem);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibMemoryFree
 * --------------------------------------------------------------------------------------------- */

void OcilibMemoryFree
(
    OCI_Environment *env,
    void            *ptr_mem
)
{
    if (ptr_mem && env)
    {
        OCI_MemoryBlock *block = (OCI_MemoryBlock *)(((unsigned char*)ptr_mem) - sizeof(*block));

        if (block)
        {
            /* Must negate after the cast: block->size is a OCI_Size, so
               `(OCI_Int64) 0 - block->size' would promote both operands to
               unsigned and add a huge positive value to the counter. */

            OcilibMemoryUpdateBytes(env, block->type, -(OCI_Int64) block->size);

            OCILIB_LOG_WITH_ENV(env, OCI_LOG_CAT_LIB, OCI_LOG_DEBUG, __func__,
                                OTEXT("freed %zu bytes of type %d"),
                                block->size, (OCI_Int32) block->type);

            free(block);
        }
    }
    else if (ptr_mem)
    {
        free(ptr_mem);
    }
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibMemoryAllocHandle
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibMemoryAllocHandle
(
    OCI_Environment *env,
    CONST dvoid     *parenth,
    dvoid          **hndlpp,
    ub4              type
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_ENVIRONMENT, env
    );

    CHECK_NULL(hndlpp);

    sword ret;

    OCI_CALL(ret, OCIHandleAlloc, parenth, hndlpp, type, 0, NULL);

    if (OCI_SUCCESSFUL(ret))
    {
        MUTEXED_CALL(env, env->nb_hndlp++);
        OCILIB_LOG(OCI_LOG_CAT_LIB, OCI_LOG_DEBUG, OTEXT("OCI handle allocated (type=%u)"), (OCI_UInt32) type);
        SET_SUCCESS();
    }

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibMemoryFreeHandle
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibMemoryFreeHandle
(
    OCI_Environment *env,
    dvoid           *hndlp,
    ub4              type
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_ENVIRONMENT, env
    );

    CHECK_NULL(hndlp);
   
    sword ret;

    OCI_CALL(ret, OCIHandleFree, hndlp, type);

    if (OCI_SUCCESSFUL(ret))
    {
        MUTEXED_CALL(env, { if (env->nb_hndlp > 0) env->nb_hndlp--;});
        OCILIB_LOG(OCI_LOG_CAT_LIB, OCI_LOG_DEBUG, OTEXT("OCI handle freed (type=%u)"), (OCI_UInt32) type);
        SET_SUCCESS();
    }

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibMemoryAllocDescriptor
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibMemoryAllocDescriptor
(
    OCI_Environment *env,
    CONST dvoid     *parenth,
    dvoid          **descpp,
    ub4              type
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_ENVIRONMENT, env
    );

    CHECK_NULL(descpp);

    sword ret;

    OCI_CALL(ret, OCIDescriptorAlloc, parenth, descpp, type, 0, NULL);

    if (OCI_SUCCESSFUL(ret))
    {
        MUTEXED_CALL(env, {env->nb_descp++;});
        SET_SUCCESS();
    }

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibMemoryAllocDescriptorArray
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibMemoryAllocDescriptorArray
(
    OCI_Environment *env,
    CONST dvoid     *parenth,
    dvoid          **descpp,
    ub4              type,
    ub4              nb_elem
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_ENVIRONMENT, env
    );

    CHECK_NULL(descpp);

    sword ret = OCI_SUCCESS;

#if OCI_VERSION_COMPILE >= OCI_11_1

    if (env->version_runtime >= OCI_11_1)
    {
        OCI_CALL(ret, OCIArrayDescriptorAlloc, parenth, descpp, type, nb_elem, 0, NULL);

    }
    else

#endif

    {
        for (ub4 i = 0; (i < nb_elem) && (OCI_SUCCESS == ret); i++)
        {
            OCI_CALL(ret, OCIDescriptorAlloc, parenth, &descpp[i], type, 0, NULL);
        }
    }

    if (OCI_SUCCESSFUL(ret))
    {
        MUTEXED_CALL(env, env->nb_descp += nb_elem);
        SET_SUCCESS();
    }

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibMemoryFreeDescriptor
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibMemoryFreeDescriptor
(
    OCI_Environment *env,
    dvoid           *descp,
    ub4              type
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_ENVIRONMENT, env
    );

    CHECK_NULL(descp);

    sword ret;

    OCI_CALL(ret, OCIDescriptorFree, descp, type);

    if (OCI_SUCCESSFUL(ret))
    {
        MUTEXED_CALL(env, { if (env->nb_descp > 0) env->nb_descp--;});
        SET_SUCCESS();
    }

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibMemoryFreeDescriptorArray
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibMemoryFreeDescriptorArray
(
    OCI_Environment *env,
    dvoid          **descp,
    ub4              type,
    ub4              nb_elem
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_ENVIRONMENT, env
    );

    CHECK_NULL(descp);

    sword ret = OCI_SUCCESS;

    if (descp)
    {

#if OCI_VERSION_COMPILE >= OCI_11_1

        if (env->version_runtime >= OCI_11_1)
        {
            OCI_CALL(ret, OCIArrayDescriptorFree, descp, type);
        }
        else

#endif

        {
            for (ub4 i = 0; (i < nb_elem) && (OCI_SUCCESS == ret); i++)
            {
                OCI_CALL(ret, OCIDescriptorFree, descp[i], type);
            }
        }
    }

    if (OCI_SUCCESSFUL(ret))
    {
        MUTEXED_CALL(env, { if (env->nb_descp >= nb_elem) env->nb_descp -= nb_elem;});
        SET_SUCCESS();
    }

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibMemoryAllocateObject
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibMemoryAllocateObject
(
    OCI_Environment *libenv,
    OCIEnv          *envh,
    OCIError        *err,
    CONST OCISvcCtx *svc,
    OCITypeCode      typecode,
    OCIType         *tdo,
    dvoid           *table,
    OCIDuration      duration,
    OCI_Boolean      value,
    dvoid          **instance
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_ENVIRONMENT, libenv
    );

    CHECK_OCI
    (
        err,
        OCIObjectNew,
        envh, err, svc, typecode, tdo, table,
        duration, (boolean) value, instance
    );

    SET_SUCCESS();

    MUTEXED_CALL(libenv, libenv->nb_objinst++);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibMemoryFreeObject
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibMemoryFreeObject
(
    OCI_Environment *libenv,
    OCIEnv          *envh,
    OCIError        *err,
    dvoid           *instance,
    ub2              flags
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_ENVIRONMENT, libenv
    );

    CHECK_PTR(OCI_IPC_VOID, instance);

    CHECK_OCI
    (
        err,
        OCIObjectFree,
        envh, err, instance, flags
    );

    SET_SUCCESS();

    MUTEXED_CALL(libenv, libenv->nb_objinst--);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
* OcilibMemoryAllocOracleCallback
* --------------------------------------------------------------------------------------------- */

void * OcilibMemoryAllocOracleCallback
(
    void    *ctxp,
    OCI_Size size
)
{
    OCI_NOT_USED(ctxp);

    return malloc(size);
}

/* --------------------------------------------------------------------------------------------- *
* OcilibMemoryReallocOracleCallback
* --------------------------------------------------------------------------------------------- */

void * OcilibMemoryReallocOracleCallback
(
    void    *ctxp,
    void    *memptr,
    OCI_Size newsize
)
{
    OCI_NOT_USED(ctxp);

    return realloc(memptr, newsize);
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
    OCI_NOT_USED(ctxp);

    free(memptr);
}
