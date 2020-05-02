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
    ENTER_FUNC
    (
        /* returns */ OCI_ThreadKey*, NULL,
        /* context */ OCI_IPC_VOID, &Env
    )

    /* allocate key structure */

    OCI_ThreadKey *key = NULL;

    ALLOC_DATA(OCI_IPC_THREADKEY, key, 1)

    /* allocate error handle */

    CHECK(MemoryAllocHandle(Env.env, (dvoid **)(void *)&key->err, OCI_HTYPE_ERROR))

    /* key initialization */

    CHECK_OCI
    (
        key->err,
        OCIThreadKeyInit, 
        Env.env, key->err, &key->handle, destfunc
    )

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE)
        {
            ThreadKeyFree(key);
            key = NULL;
        }

        SET_RETVAL(key)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * ThreadKeyFree
 * --------------------------------------------------------------------------------------------- */

boolean ThreadKeyFree
(
    OCI_ThreadKey *key
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_THREADKEY, key
    )

    CHECK_PTR(OCI_IPC_THREADKEY, key)

    /* close key handle */

    if (NULL != key->handle)
    {
        CHECK_OCI
        (
            key->err, 
            OCIThreadKeyDestroy, 
            Env.env, key->err,
            &key->handle
        )
    }

    /* close error handle */

    if (NULL !=  key->err)
    {
        MemoryFreeHandle(key->err, OCI_HTYPE_ERROR);
    }

    /* free key structure */

    FREE(key)

    EXIT_FUNC()
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
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_THREADKEY, key
    )

    CHECK_PTR(OCI_IPC_THREADKEY, key)

    CHECK_OCI
    (
        key->err, 
        OCIThreadKeySet,
        Env.env, key->err, 
        key->handle, value
    )

    SET_SUCCESS()

    EXIT_FUNC()
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
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_THREADKEY, key
    )

    CHECK_PTR(OCI_IPC_THREADKEY, key)
    CHECK_PTR(OCI_IPC_VOID, value)

    CHECK_OCI
    (
        key->err, 
        OCIThreadKeyGet, 
        Env.env, key->err,
        key->handle, value
    )

    SET_SUCCESS()

    EXIT_FUNC()
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
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_VOID, &Env
    )

    OCI_ThreadKey *key = NULL;

    CHECK_PTR(OCI_IPC_STRING, name)
    CHECK_INITIALIZED()

    if (NULL == Env.key_map)
    {
        /* create the map at the first call to ThreadKeyCreate to save
           time and memory when it's not needed */

        Env.key_map = HashCreate(OCI_HASH_DEFAULT_SIZE, OCI_HASH_POINTER);
        CHECK_NULL(Env.key_map)
    }

    /* create key */

    key = ThreadKeyCreateInternal(destfunc);
    CHECK_NULL(key)

    /* add key to internal key hash table */

    CHECK(HashAddPointer(Env.key_map, name, key))

    SET_SUCCESS()

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE && NULL != key)
        {
            ThreadKeyFree(key);
        }
    )
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
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_VOID, &Env
    )

    OCI_ThreadKey *key = NULL;

    CHECK_PTR(OCI_IPC_STRING, name)

    key = (OCI_ThreadKey*) HashGetPointer(Env.key_map, name);
    CHECK_NULL(key)

    CHECK(ThreadKeySet(key, value))

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * ThreadKeyGetValue
 * --------------------------------------------------------------------------------------------- */

void * ThreadKeyGetValue
(
    const otext *name
)
{
    ENTER_FUNC
    (
        /* returns */ void*, NULL,
        /* context */ OCI_IPC_VOID, &Env
    )

    OCI_ThreadKey* key = NULL;
    CHECK_PTR(OCI_IPC_STRING, name)

    key = (OCI_ThreadKey*)HashGetPointer(Env.key_map, name);
    CHECK_NULL(key)

    void *data = NULL;
    CHECK(ThreadKeyGet(key, &data))

    SET_RETVAL(data)

    EXIT_FUNC()
}
