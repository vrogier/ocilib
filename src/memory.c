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
   |               Copyright (c) 2007-2008 Vincent ROGIER                 |
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
 * $Id: memory.c, v 3.1.0 2008/10/26 07:50 Vince $
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
    OCILib.nb_hndlp++;        
    
    return OCIHandleAlloc(parenth, hndlpp, type, xtramem_sz, usrmempp);           
}

/* ------------------------------------------------------------------------ *
 * OCI_HandleFree
 * ------------------------------------------------------------------------ */

sword OCI_HandleFree(dvoid *hndlp, CONST ub4 type)
{                
    OCILib.nb_hndlp--;  

    return OCIHandleFree(hndlp, type);
}

/* ------------------------------------------------------------------------ *
 * OCI_DescriptorAlloc
 * ------------------------------------------------------------------------ */

sword OCI_DescriptorAlloc(CONST dvoid *parenth, dvoid **descpp, CONST ub4 type,
                          CONST size_t xtramem_sz,  dvoid **usrmempp)
{
    OCILib.nb_descp++;   

    return OCIDescriptorAlloc(parenth, descpp, type, xtramem_sz, usrmempp);
}

/* ------------------------------------------------------------------------ *
 * OCI_DescriptorFree
 * ------------------------------------------------------------------------ */

sword OCI_DescriptorFree(dvoid *descp, CONST ub4 type)
{                                                    
    OCILib.nb_descp--;        

    return OCIDescriptorFree(descp, type);                  
}

/* ------------------------------------------------------------------------ *
 * OCI_fob
 * ------------------------------------------------------------------------ */

sword OCI_ObjectNew(OCIEnv *env, OCIError *err, CONST OCISvcCtx *svc,
                    OCITypeCode typecode, OCIType *tdo, dvoid *table, 
                    OCIDuration duration, boolean value, 
                    dvoid **instance)
{
    OCILib.nb_objinst++;        
    
    return OCIObjectNew(env, err, svc, typecode, tdo, table, duration, value, 
                        instance);
}

/* ------------------------------------------------------------------------ *
 * OCI_OCIObjectFree
 * ------------------------------------------------------------------------ */

sword OCI_OCIObjectFree(OCIEnv *env, OCIError *err, dvoid *instance, ub2 flags)
{
    OCILib.nb_objinst--;        

    return OCIObjectFree(env, err, instance, flags);
}


