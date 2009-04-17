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
 * $Id: threadkey.c, v 3.2.0 2009/04/20 00:00 Vince $
 * ------------------------------------------------------------------------ */

#include "ocilib_internal.h"

/* ************************************************************************ *
 *                             PRIVATE FUNCTIONS
 * ************************************************************************ */

/* ------------------------------------------------------------------------ *
 * OCI_ThreadKeyCreateInternal
 * ------------------------------------------------------------------------ */

OCI_ThreadKey * OCI_ThreadKeyCreateInternal(POCI_THREADKEYDEST destfunc)
{
    boolean  res       = TRUE;
    OCI_ThreadKey *key = NULL;

    /* allocate key structure */

    key = (OCI_ThreadKey *) OCI_MemAlloc(OCI_IPC_THREADKEY, sizeof(*key), 1, TRUE);

    if (key != NULL)
    {
        /* allocate error handle */

        res = (OCI_SUCCESS == OCI_HandleAlloc(OCILib.env, 
                                              (dvoid **) (void *) &key->err,
                                              OCI_HTYPE_ERROR, 0,
                                              (dvoid **) NULL));

        /* key initialization */

        OCI_CALL3
        (
            res, key->err,
            
            OCIThreadKeyInit(OCILib.env, key->err, &key->handle, destfunc)
        )
    }
    else
        res = FALSE;

    /* check errors */

    if (res == FALSE)
    {
        OCI_ThreadKeyFree(key);
        key = NULL;
    }

    return key;
}
/* ------------------------------------------------------------------------ *
 * OCI_ThreadKeyFree
 * ------------------------------------------------------------------------ */

boolean OCI_ThreadKeyFree(OCI_ThreadKey *key)
{
    boolean res = TRUE;

    OCI_CHECK(key == NULL, FALSE);

    /* close key handle */

    if (key->handle != NULL)
    {
        OCI_CALL0
        (
            res, key->err, 
            
            OCIThreadKeyDestroy(OCILib.env, key->err, &key->handle)
        )
    }

    /* close error handle */

    if (key->err != NULL)
    {
        OCI_HandleFree(key->err, OCI_HTYPE_ERROR);
    }

    /* free key structure */

    OCI_FREE(key);

    OCI_RESULT(res);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_ThreadKeySet
 * ------------------------------------------------------------------------ */

boolean OCI_ThreadKeySet(OCI_ThreadKey *key, void *value)
{
    boolean res = TRUE;

    OCI_CHECK(key == NULL, FALSE);

    OCI_CALL3
    (
        res, key->err, 
        
        OCIThreadKeySet(OCILib.env, key->err, key->handle, value)
     )

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_ThreadKeyGet
 * ------------------------------------------------------------------------ */

boolean OCI_ThreadKeyGet(OCI_ThreadKey* key, void **value)
{
    boolean res  = TRUE;

    OCI_CHECK(key == NULL, FALSE);
 
    OCI_CALL3
    (
        res, key->err, 
        
        OCIThreadKeyGet(OCILib.env, key->err, key->handle, value)
     )
  
    return res;
}

/* ************************************************************************ *
 *                            PUBLIC FUNCTIONS
 * ************************************************************************ */

/* ------------------------------------------------------------------------ *
 * OCI_ThreadKeyCreate
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_ThreadKeyCreate(const mtext *name, POCI_THREADKEYDEST destfunc)
{
    OCI_ThreadKey *key = NULL;
    boolean res        = TRUE;

    OCI_CHECK_PTR(OCI_IPC_STRING, name, FALSE);

    OCI_CHECK_INITIALIZED(FALSE);

    if (OCILib.key_map == NULL)
    {
        /* create the map at the first call to OCI_ThreadKeyCreate to save
           time and memory when it's not needed */

        OCILib.key_map = OCI_HashCreate(OCI_HASH_DEFAULT_SIZE, OCI_HASH_POINTER);

    }

    res = (OCILib.key_map != NULL);

    /* create key */

    if (res == TRUE)
    {
        key = OCI_ThreadKeyCreateInternal(destfunc);

        /* add key to internal key hash table */

        if (key != NULL)
            res = OCI_HashAddPointer(OCILib.key_map, name, key);
        else
            res = FALSE;
    }

    /* check errors */

    if (res == FALSE)
        OCI_ThreadKeyFree(key);

    OCI_RESULT(res);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_ThreadKeySetValue
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_ThreadKeySetValue(const mtext *name, void *value)
{
    boolean res        = TRUE;
    OCI_ThreadKey *key = NULL;

    OCI_CHECK_PTR(OCI_IPC_STRING, name, FALSE);

    key = (OCI_ThreadKey *) OCI_HashGetPointer(OCILib.key_map, name);

    res = OCI_ThreadKeySet(key, value);

    OCI_RESULT(res);

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_ThreadKeyGetValue
 * ------------------------------------------------------------------------ */

void * OCI_API OCI_ThreadKeyGetValue(const mtext *name)
{
    boolean res        = TRUE;
    void * value       = NULL;
    OCI_ThreadKey* key = NULL;

    OCI_CHECK_PTR(OCI_IPC_STRING, name, FALSE);

    key = (OCI_ThreadKey *) OCI_HashGetPointer(OCILib.key_map, name);

    res = OCI_ThreadKeyGet(key, &value);

    OCI_RESULT(res);
    
    return value;
}
