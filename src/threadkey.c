/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2018 Vincent ROGIER <vince.rogier@ocilib.net>
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

#include "ocilib_internal.h"

/* ********************************************************************************************* *
 *                             PRIVATE FUNCTIONS
 * ********************************************************************************************* */

/* --------------------------------------------------------------------------------------------- *
 * OCI_ThreadKeyCreateInternal
 * --------------------------------------------------------------------------------------------- */

OCI_ThreadKey * OCI_ThreadKeyCreateInternal
(
    POCI_THREADKEYDEST destfunc
)
{
    OCI_ThreadKey *key = NULL;

    OCI_CALL_DECLARE_CONTEXT(TRUE)
        
    /* allocate key structure */

    OCI_ALLOCATE_DATA(OCI_IPC_THREADKEY, key, 1)

    if (OCI_STATUS)
    {
        /* allocate error handle */

        OCI_STATUS = OCI_HandleAlloc(OCILib.env, (dvoid **)(void *)&key->err, OCI_HTYPE_ERROR);

        /* key initialization */

        OCI_EXEC(OCIThreadKeyInit(OCILib.env, key->err, &key->handle, destfunc))

        /* check errors */

        if (!OCI_STATUS)
        {
            OCI_ThreadKeyFree(key);
            key = NULL;
        }
    } 

    return key;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ThreadKeyFree
 * --------------------------------------------------------------------------------------------- */

boolean OCI_ThreadKeyFree
(
    OCI_ThreadKey *key
)
{
    OCI_CALL_DECLARE_CONTEXT(TRUE)

    OCI_CHECK(NULL == key, FALSE);

    /* close key handle */

    if (key->handle)
    {
        OCI_EXEC(OCIThreadKeyDestroy(OCILib.env, key->err, &key->handle))
    }

    /* close error handle */

    if (key->err)
    {
        OCI_HandleFree(key->err, OCI_HTYPE_ERROR);
    }

    /* free key structure */

    OCI_FREE(key)

    return OCI_STATUS;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ThreadKeySet
 * --------------------------------------------------------------------------------------------- */

boolean OCI_ThreadKeySet
(
    OCI_ThreadKey *key,
    void          *value
)
{
    OCI_CALL_DECLARE_CONTEXT(TRUE)

    OCI_CHECK(NULL == key, FALSE);

    OCI_EXEC(OCIThreadKeySet(OCILib.env, key->err, key->handle, value))

    return OCI_STATUS;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ThreadKeyGet
 * --------------------------------------------------------------------------------------------- */

boolean OCI_ThreadKeyGet
(
    OCI_ThreadKey* key,
    void         **value
)
{
    OCI_CALL_DECLARE_CONTEXT(TRUE)

    OCI_CHECK(NULL == key, FALSE);

    OCI_EXEC(OCIThreadKeyGet(OCILib.env, key->err, key->handle, value))

    return OCI_STATUS;
}

/* ********************************************************************************************* *
 *                            PUBLIC FUNCTIONS
 * ********************************************************************************************* */

/* --------------------------------------------------------------------------------------------- *
 * OCI_ThreadKeyCreate
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ThreadKeyCreate
(
    const otext       *name,
    POCI_THREADKEYDEST destfunc
)
{
    OCI_ThreadKey *key = NULL;

    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_STRING, name)
    OCI_CALL_CHECK_INITIALIZED()

    if (!OCILib.key_map)
    {
        /* create the map at the first call to OCI_ThreadKeyCreate to save
           time and memory when it's not needed */

        OCILib.key_map = OCI_HashCreate(OCI_HASH_DEFAULT_SIZE, OCI_HASH_POINTER);
        OCI_STATUS = (NULL != OCILib.key_map);
    }

    /* create key */

    if (OCI_STATUS)
    {
        key = OCI_ThreadKeyCreateInternal(destfunc);
        OCI_STATUS = (NULL != key);
       
        /* add key to internal key hash table */

        OCI_STATUS = OCI_STATUS && OCI_HashAddPointer(OCILib.key_map, name, key);
    }

    /* check errors */

    if (!OCI_STATUS && key)
    {
        OCI_ThreadKeyFree(key);
    }

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ThreadKeySetValue
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ThreadKeySetValue
(
    const otext *name,
    void        *value
)
{
    OCI_ThreadKey *key = NULL;

    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_STRING, name)

    key = (OCI_ThreadKey *) OCI_HashGetPointer(OCILib.key_map, name);

    OCI_RETVAL = OCI_STATUS = OCI_ThreadKeySet(key, value);

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ThreadKeyGetValue
 * --------------------------------------------------------------------------------------------- */

void * OCI_API OCI_ThreadKeyGetValue
(
    const otext *name
)
{
    OCI_ThreadKey* key = NULL;

    OCI_CALL_ENTER(void*, NULL)
    OCI_CALL_CHECK_PTR(OCI_IPC_STRING, name)

    key = (OCI_ThreadKey *)OCI_HashGetPointer(OCILib.key_map, name);

    OCI_STATUS = OCI_ThreadKeyGet(key, &OCI_RETVAL);

    OCI_CALL_EXIT()
}
