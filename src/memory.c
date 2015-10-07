/*
    +-----------------------------------------------------------------------------------------+
    |                                                                                         |
    |                               OCILIB - C Driver for Oracle                              |
    |                                                                                         |
    |                                (C Wrapper for Oracle OCI)                               |
    |                                                                                         |
    |                              Website : http://www.ocilib.net                            |
    |                                                                                         |
    |             Copyright (c) 2007-2015 Vincent ROGIER <vince.rogier@ocilib.net>            |
    |                                                                                         |
    +-----------------------------------------------------------------------------------------+
    |                                                                                         |
    |             This library is free software; you can redistribute it and/or               |
    |             modify it under the terms of the GNU Lesser General Public                  |
    |             License as published by the Free Software Foundation; either                |
    |             version 2 of the License, or (at your option) any later version.            |
    |                                                                                         |
    |             This library is distributed in the hope that it will be useful,             |
    |             but WITHOUT ANY WARRANTY; without even the implied warranty of              |
    |             MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU           |
    |             Lesser General Public License for more details.                             |
    |                                                                                         |
    |             You should have received a copy of the GNU Lesser General Public            |
    |             License along with this library; if not, write to the Free                  |
    |             Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.          |
    |                                                                                         |
    +-----------------------------------------------------------------------------------------+
*/

/* --------------------------------------------------------------------------------------------- *
 * $Id: memory.c, Vincent Rogier $
 * --------------------------------------------------------------------------------------------- */

#include "ocilib_internal.h"

/* ********************************************************************************************* *
 *                             PRIVATE FUNCTIONS
 * ********************************************************************************************* */

#define OCI_MUTEXED_CALL(exp)               \
                                            \
    if (OCILib.mem_mutex)                   \
    {                                       \
       OCI_MutexAcquire(OCILib.mem_mutex);  \
    }                                       \
                                            \
    exp;                                    \
                                            \
    if (OCILib.mem_mutex)                   \
    {                                       \
       OCI_MutexRelease(OCILib.mem_mutex);  \
    }                                       \

/* --------------------------------------------------------------------------------------------- *
 * OCI_MemAlloc
 * --------------------------------------------------------------------------------------------- */

void * OCI_MemAlloc
(
    int     ptr_type,
    size_t  block_size,
    size_t  block_count,
    boolean zero_fill
)
{
    OCI_MemoryBlock * mem_block = NULL;
    size_t size = sizeof(OCI_MemoryBlock) + (block_size * block_count);

    mem_block = (OCI_MemoryBlock *)malloc(size);

    if (mem_block)
    {
        if (zero_fill)
        {
            memset(mem_block, 0, size);
        }

        mem_block->type = ptr_type;
        mem_block->size = (unsigned int) size;

        OCI_MemUpdateBytes(mem_block->type, mem_block->size);
    }
    else
    {
        OCI_ExceptionMemory(ptr_type, size, NULL, NULL);
    }

    return ((unsigned char *) mem_block) + sizeof(*mem_block);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_MemRealloc
 * --------------------------------------------------------------------------------------------- */

void * OCI_MemRealloc
(
    void * ptr_mem,
    int    ptr_type,
    size_t block_size,
    size_t block_count
)
{
    OCI_MemoryBlock * mem_block = NULL;
	size_t size = 0;

    if (ptr_mem)
    {
        mem_block = (OCI_MemoryBlock *) (((unsigned char*)ptr_mem) - sizeof(*mem_block));
    }

    size = sizeof(OCI_MemoryBlock) + (block_size * block_count);

    if (!mem_block || mem_block->size < size)
    {
        void *ptr_new = realloc(mem_block, size);

        if (!ptr_new && ptr_mem)
        {
            OCI_MemFree(ptr_mem);

            OCI_ExceptionMemory(ptr_type, size, NULL, NULL);
        }
        else
        {
            mem_block = (OCI_MemoryBlock *) ptr_new;

            big_int size_diff = (big_int) size - mem_block->size;

            mem_block->type = ptr_type;
            mem_block->size = (unsigned int) size;

            OCI_MemUpdateBytes(mem_block->type, size_diff);
        }
    }

    return mem_block ? ((unsigned char *)mem_block) + sizeof(*mem_block) : NULL;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_MemFree
 * --------------------------------------------------------------------------------------------- */

void OCI_MemFree
(
    void * ptr_mem
)
{
    if (ptr_mem)
    {
        OCI_MemoryBlock *mem_block = (OCI_MemoryBlock *)(((unsigned char*)ptr_mem) - sizeof(*mem_block));
       
        if (mem_block)
        {
            OCI_MemUpdateBytes(mem_block->type, (big_int) 0 - mem_block->size);

            free(mem_block);
        }
    }
}

/* --------------------------------------------------------------------------------------------- *
* OCI_MemUpdateBytes
* --------------------------------------------------------------------------------------------- */

void OCI_MemUpdateBytes
(
    int type,
    big_int size
)
{
    if (OCI_IPC_ORACLE == type)
    {
        OCI_MUTEXED_CALL(OCILib.mem_bytes_oci += size)
    }
    else
    {
        OCI_MUTEXED_CALL(OCILib.mem_bytes_lib += size)
    }
}


/* --------------------------------------------------------------------------------------------- *
 * OCI_HandleAlloc
 * --------------------------------------------------------------------------------------------- */

sword OCI_HandleAlloc
(
    CONST dvoid *parenth,
    dvoid      **hndlpp,
    CONST ub4    type,
    CONST size_t xtramem_sz,
    dvoid      **usrmempp
)
{
    sword ret = OCIHandleAlloc(parenth, hndlpp, type, xtramem_sz, usrmempp);

    if (OCI_SUCCESSFUL(ret))
    {
        OCI_MUTEXED_CALL(OCILib.nb_hndlp++)
    }

    return ret;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_HandleFree
 * --------------------------------------------------------------------------------------------- */

sword OCI_HandleFree
(
    dvoid    *hndlp,
    CONST ub4 type
)
{
    sword ret = OCI_SUCCESS;

    if (hndlp)
    {
        OCI_MUTEXED_CALL(OCILib.nb_hndlp--)

        ret = OCIHandleFree(hndlp, type);
    }

    return ret;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DescriptorAlloc
 * --------------------------------------------------------------------------------------------- */

sword OCI_DescriptorAlloc
(
    CONST dvoid *parenth,
    dvoid      **descpp,
    CONST ub4    type,
    CONST size_t xtramem_sz,
    dvoid      **usrmempp
)
{
    sword ret = OCIDescriptorAlloc(parenth, descpp, type, xtramem_sz, usrmempp);

    if (OCI_SUCCESSFUL(ret))
    {
        OCI_MUTEXED_CALL(OCILib.nb_descp++)
    }

    return ret;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DescriptorArrayAlloc
 * --------------------------------------------------------------------------------------------- */

sword OCI_DescriptorArrayAlloc
(
    CONST dvoid *parenth,
    dvoid      **descpp,
    CONST ub4    type,
    ub4          nb_elem,
    CONST size_t xtramem_sz,
    dvoid      **usrmempp
)
{
    sword ret = OCI_SUCCESS;

#if OCI_VERSION_COMPILE >= OCI_11_1

    if (OCILib.version_runtime >= OCI_11_1)
    {
        ret = OCIArrayDescriptorAlloc(parenth, descpp, type, nb_elem, xtramem_sz, usrmempp);

    }
    else

#endif

    {
        ub4 i;

        for (i = 0; (i < nb_elem) && (OCI_SUCCESS == ret); i++)
        {
            ret = OCIDescriptorAlloc(parenth, &descpp[i], type, xtramem_sz, usrmempp);
        }
    }

    if (OCI_SUCCESSFUL(ret))
    {
        OCI_MUTEXED_CALL(OCILib.nb_descp += nb_elem)
    }

    return ret;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DescriptorFree
 * --------------------------------------------------------------------------------------------- */

sword OCI_DescriptorFree
(
    dvoid    *descp,
    CONST ub4 type
)
{
    sword ret = OCI_SUCCESS;

    if (descp)
    {
        OCI_MUTEXED_CALL(OCILib.nb_descp--)

        ret = OCIDescriptorFree(descp, type);
    }

    return ret;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DescriptorFree
 * --------------------------------------------------------------------------------------------- */

sword OCI_DescriptorArrayFree
(
    dvoid   **descp,
    CONST ub4 type,
    ub4       nb_elem
)
{
    sword ret = OCI_SUCCESS;

    if (descp)
    {

    #if OCI_VERSION_COMPILE >= OCI_11_1

        if (OCILib.version_runtime >= OCI_11_1)
        {
            ret = OCIArrayDescriptorFree(descp, type);

        }
        else

    #endif

        {
            ub4 i;

            for (i = 0; (i < nb_elem) && (OCI_SUCCESS == ret); i++)
            {
                ret = OCIDescriptorFree(descp[i], type);
            }
        }

        OCI_MUTEXED_CALL(OCILib.nb_descp -= nb_elem)
    }

    return ret;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectNew
 * --------------------------------------------------------------------------------------------- */

sword OCI_ObjectNew
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
    sword ret = OCIObjectNew(env, err, svc, typecode, tdo, table, duration, value, instance);

    if (OCI_SUCCESSFUL(ret))
    {
        OCI_MUTEXED_CALL(OCILib.nb_objinst++)
    }

    return ret;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_OCIObjectFree
 * --------------------------------------------------------------------------------------------- */

sword OCI_OCIObjectFree
(
    OCIEnv   *env,
    OCIError *err,
    dvoid    *instance,
    ub2       flags
)
{
    sword ret = OCI_SUCCESS;

    if (instance)
    {
        OCI_MUTEXED_CALL(OCILib.nb_objinst--)

        ret = OCIObjectFree(env, err, instance, flags);
    }

    return ret;
}

/* --------------------------------------------------------------------------------------------- *
* OCI_MemAllocOracleClient
* --------------------------------------------------------------------------------------------- */

void * OCI_MemAllocOracleClient
(
    void *ctxp, 
    size_t size
)
{
    OCI_NOT_USED(ctxp)
        
    return OCI_MemAlloc(OCI_IPC_ORACLE, size, 1, FALSE);
}

/* --------------------------------------------------------------------------------------------- *
* OCI_MemReallocOracleClient
* --------------------------------------------------------------------------------------------- */

void * OCI_MemReallocOracleClient
(
    void *ctxp, 
    void *memptr, 
    size_t newsize
)
{
    OCI_NOT_USED(ctxp)
        
    return OCI_MemRealloc(memptr, OCI_IPC_ORACLE, newsize, 1);
}

/* --------------------------------------------------------------------------------------------- *
* OCI_MemFreeOracleClient
* --------------------------------------------------------------------------------------------- */

void OCI_MemFreeOracleClient
(
    void *ctxp,
    void *memptr
)
{
    OCI_NOT_USED(ctxp)

    OCI_MemFree(memptr);
}
