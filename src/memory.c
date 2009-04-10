/*
   +----------------------------------------------------------------------+   
   |                                                                      |
   |                     OCILIB - C Driver for Oracle                     |
   |                                                                      |
   |                      (C Wrapper for Oracle OCI)                      |
   |                                                                      |
   +----------------------------------------------------------------------+
   |                      Website : http://ocilib.net                     |
   +----------------------------------------------------------------------+
   |               Copyright (c) 2007-2009 Vincent ROGIER                 |
   +----------------------------------------------------------------------+
   | This library is free software; you can redistribute it and/or        |
   | modify it under the terms of the GNU Library General Public          |
   | License as published by the Free Software Foundation; either         |
   | version 2 of the License, or (at your option) any later version.     |
   |                                                                      |
   | This library is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU    |
   | Library General Public License for more details.                     |
   |                                                                      |
   | You should have received a copy of the GNU Library General Public    |
   | License along with this library; if not, write to the Free           |
   | Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.   |
   +----------------------------------------------------------------------+
   |          Author: Vincent ROGIER <vince.rogier@gmail.com>             |
   +----------------------------------------------------------------------+ 
*/

/* ------------------------------------------------------------------------ *
 * $Id: memory.c, v 3.2.0 2009/04/20 00:00 Vince $
 * ------------------------------------------------------------------------ */

#include "ocilib_internal.h"

/* ************************************************************************ *
 *                             PRIVATE FUNCTIONS
 * ************************************************************************ */

/* ------------------------------------------------------------------------ *
 * OCI_MemAlloc
 * ------------------------------------------------------------------------ */

void * OCI_MemAlloc(int ptr_type, int block_size, int block_count, 
                    boolean zero_fill)
{
    void * ptr  = NULL;
    size_t size = (size_t) (block_size * block_count);

    ptr = (void *) malloc(size);
     
    if (ptr != NULL)                                        
    {
        if (zero_fill == TRUE)
            memset(ptr, 0, size);
    }
    else
        OCI_ExceptionMemory(ptr_type, size, NULL, NULL);

    return ptr;
}

/* ------------------------------------------------------------------------ *
 * OCI_MemRealloc
 * ------------------------------------------------------------------------ */

void * OCI_MemRealloc(void * ptr_mem, int ptr_type, int block_size, 
                      int block_count)
{
    void * ptr  = NULL;
    size_t size = (size_t) (block_size * block_count);

    ptr = (void *) realloc(ptr_mem, size);
     
    if (ptr == NULL)
    {
        OCI_MemFree(ptr_mem);

        OCI_ExceptionMemory(ptr_type, size, NULL, NULL);
    }

    return ptr;
}

/* ------------------------------------------------------------------------ *
 * OCI_MemFree
 * ------------------------------------------------------------------------ */

void OCI_MemFree(void * ptr_mem)
{
    if (ptr_mem != NULL)
        free(ptr_mem);
}

/* ------------------------------------------------------------------------ *
 * OCI_HandleAlloc
 * ------------------------------------------------------------------------ */

sword OCI_HandleAlloc(CONST dvoid *parenth, dvoid **hndlpp, CONST ub4 type,
                      CONST size_t xtramem_sz, dvoid **usrmempp)
{     
    sword ret = OCI_SUCCESS;

    ret = OCIHandleAlloc(parenth, hndlpp, type, xtramem_sz, usrmempp);

    if (ret == OCI_SUCCESS)
    {
        OCILib.nb_hndlp++;   
    }

    return ret;
}

/* ------------------------------------------------------------------------ *
 * OCI_HandleFree
 * ------------------------------------------------------------------------ */

sword OCI_HandleFree(dvoid *hndlp, CONST ub4 type)
{                
    sword ret = OCI_SUCCESS;

    if (hndlp != NULL)
    {
        OCILib.nb_hndlp--;  

        ret = OCIHandleFree(hndlp, type);
    }

    return ret;
}

/* ------------------------------------------------------------------------ *
 * OCI_DescriptorAlloc
 * ------------------------------------------------------------------------ */

sword OCI_DescriptorAlloc(CONST dvoid *parenth, dvoid **descpp, CONST ub4 type,
                          CONST size_t xtramem_sz,  dvoid **usrmempp)
{
    sword ret = OCI_SUCCESS;

    ret = OCIDescriptorAlloc(parenth, descpp, type, xtramem_sz, usrmempp);

    if (ret == OCI_SUCCESS)
    {
        OCILib.nb_descp++;   
    }

    return ret;
}

/* ------------------------------------------------------------------------ *
 * OCI_DescriptorFree
 * ------------------------------------------------------------------------ */

sword OCI_DescriptorFree(dvoid *descp, CONST ub4 type)
{                                                    
    sword ret = OCI_SUCCESS;

    if (descp != NULL)
    {
        OCILib.nb_descp--;  

        ret = OCIDescriptorFree(descp, type);
    }

    return ret;
}

/* ------------------------------------------------------------------------ *
 * OCI_fob
 * ------------------------------------------------------------------------ */

sword OCI_ObjectNew(OCIEnv *env, OCIError *err, CONST OCISvcCtx *svc,
                    OCITypeCode typecode, OCIType *tdo, dvoid *table, 
                    OCIDuration duration, boolean value, 
                    dvoid **instance)
{
    sword ret = OCI_SUCCESS;

    ret = OCIObjectNew(env, err, svc, typecode, tdo, table, duration, value, instance);

    if (ret == OCI_SUCCESS)
    {
        OCILib.nb_objinst++;  
    }

    return ret;
}

/* ------------------------------------------------------------------------ *
 * OCI_OCIObjectFree
 * ------------------------------------------------------------------------ */

sword OCI_OCIObjectFree(OCIEnv *env, OCIError *err, dvoid *instance, ub2 flags)
{
    sword ret = OCI_SUCCESS;

    if (instance != NULL)
    {
        OCILib.nb_objinst--;  

        ret = OCIObjectFree(env, err, instance, flags);
    }

    return ret;
}


