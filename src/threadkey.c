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

#include "threadkey.h"

#include "hash.h"
#include "macros.h"

/* --------------------------------------------------------------------------------------------- *
 * ThreadKeyCreateInternal
 * --------------------------------------------------------------------------------------------- */

OCI_ThreadKey * ThreadKeyCreateInternal
(
    POCI_THREADKEYDEST destfunc
)
{
    OCI_ThreadKey *key = NULL;

    DECLARE_CTX(TRUE)
        
    /* allocate key structure */

    ALLOC_DATA(OCI_IPC_THREADKEY, key, 1)

    if (STATUS)
    {
        /* allocate error handle */

        STATUS = MemoryAllocHandle(OCILib.env, (dvoid **)(void *)&key->err, OCI_HTYPE_ERROR);

        /* key initialization */

        EXEC(OCIThreadKeyInit(OCILib.env, key->err, &key->handle, destfunc))

        /* check errors */

        if (!STATUS)
        {
            ThreadKeyFree(key);
            key = NULL;
        }
    } 

    return key;
}

/* --------------------------------------------------------------------------------------------- *
 * ThreadKeyFree
 * --------------------------------------------------------------------------------------------- */

boolean ThreadKeyFree
(
    OCI_ThreadKey *key
)
{
    DECLARE_CTX(TRUE)

    CHECK(NULL == key, FALSE);

    /* close key handle */

    if (key->handle)
    {
        EXEC(OCIThreadKeyDestroy(OCILib.env, key->err, &key->handle))
    }

    /* close error handle */

    if (key->err)
    {
        MemoryFreeHandle(key->err, OCI_HTYPE_ERROR);
    }

    /* free key structure */

    FREE(key)

    return STATUS;
}

/* --------------------------------------------------------------------------------------------- *
 * ThreadKeySet
 * --------------------------------------------------------------------------------------------- */

boolean ThreadKeySet
(
    OCI_ThreadKey *key,
    void          *value
)
{
    DECLARE_CTX(TRUE)

    CHECK(NULL == key, FALSE);

    EXEC(OCIThreadKeySet(OCILib.env, key->err, key->handle, value))

    return STATUS;
}

/* --------------------------------------------------------------------------------------------- *
 * ThreadKeyGet
 * --------------------------------------------------------------------------------------------- */

boolean ThreadKeyGet
(
    OCI_ThreadKey* key,
    void         **value
)
{
    DECLARE_CTX(TRUE)

    CHECK(NULL == key, FALSE);

    EXEC(OCIThreadKeyGet(OCILib.env, key->err, key->handle, value))

    return STATUS;
}

/* --------------------------------------------------------------------------------------------- *
 * ThreadKeyCreate
 * --------------------------------------------------------------------------------------------- */

boolean ThreadKeyCreate
(
    const otext       *name,
    POCI_THREADKEYDEST destfunc
)
{
    OCI_ThreadKey *key = NULL;

    CALL_ENTER(boolean, FALSE)
    CHECK_PTR(OCI_IPC_STRING, name)
    CHECK_INITIALIZED()

    if (!OCILib.key_map)
    {
        /* create the map at the first call to ThreadKeyCreate to save
           time and memory when it's not needed */

        OCILib.key_map = HashCreate(OCI_HASH_DEFAULT_SIZE, OCI_HASH_POINTER);
        STATUS = (NULL != OCILib.key_map);
    }

    /* create key */

    if (STATUS)
    {
        key = ThreadKeyCreateInternal(destfunc);
        STATUS = (NULL != key);
       
        /* add key to internal key hash table */

        STATUS = STATUS && HashAddPointer(OCILib.key_map, name, key);
    }

    /* check errors */

    if (!STATUS && key)
    {
        ThreadKeyFree(key);
    }

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * ThreadKeySetValue
 * --------------------------------------------------------------------------------------------- */

boolean ThreadKeySetValue
(
    const otext *name,
    void        *value
)
{
    OCI_ThreadKey *key = NULL;

    CALL_ENTER(boolean, FALSE)
    CHECK_PTR(OCI_IPC_STRING, name)

    key = (OCI_ThreadKey *) HashGetPointer(OCILib.key_map, name);

    RETVAL = STATUS = ThreadKeySet(key, value);

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * ThreadKeyGetValue
 * --------------------------------------------------------------------------------------------- */

void * ThreadKeyGetValue
(
    const otext *name
)
{
    OCI_ThreadKey* key = NULL;

    CALL_ENTER(void*, NULL)
    CHECK_PTR(OCI_IPC_STRING, name)

    key = (OCI_ThreadKey *)HashGetPointer(OCILib.key_map, name);

    STATUS = ThreadKeyGet(key, &RETVAL);

    CALL_EXIT()
}
