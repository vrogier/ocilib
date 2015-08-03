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
 * $Id: threadkey.c, Vincent Rogier $
 * --------------------------------------------------------------------------------------------- */

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

    /* allocate key structure */

    key = (OCI_ThreadKey *) OCI_MemAlloc(OCI_IPC_THREADKEY, sizeof(*key), (size_t) 1, TRUE);

    if (key)
    {
        /* allocate error handle */

        boolean res = OCI_SUCCESSFUL(OCI_HandleAlloc(OCILib.env,(dvoid **) (void *) &key->err,
                                              OCI_HTYPE_ERROR, (size_t) 0, (dvoid **) NULL));

        /* key initialization */

        OCI_CALL3
        (
            res, key->err,

            OCIThreadKeyInit(OCILib.env, key->err, &key->handle, destfunc)
        )

        /* check errors */

        if (!res)
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
    boolean res = TRUE;

    OCI_CHECK(NULL == key, FALSE);

    /* close key handle */

    if (key->handle)
    {
        OCI_CALL0
        (
            res, key->err,

            OCIThreadKeyDestroy(OCILib.env, key->err, &key->handle)
        )
    }

    /* close error handle */

    if (key->err)
    {
        OCI_HandleFree(key->err, OCI_HTYPE_ERROR);
    }

    /* free key structure */

    OCI_FREE(key)

    return res;
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
    boolean res = TRUE;

    OCI_CHECK(NULL == key, FALSE);

    OCI_CALL3
    (
        res, key->err,

        OCIThreadKeySet(OCILib.env, key->err, key->handle, value)
    )

    return res;
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
    boolean res = TRUE;

    OCI_CHECK(NULL == key, FALSE);

    OCI_CALL3
    (
        res, key->err,

        OCIThreadKeyGet(OCILib.env, key->err, key->handle, value)
    )

    return res;
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

    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_STRING, name)
    OCI_CHECK_INITIALIZED()

    if (!OCILib.key_map)
    {
        /* create the map at the first call to OCI_ThreadKeyCreate to save
           time and memory when it's not needed */

        OCILib.key_map = OCI_HashCreate(OCI_HASH_DEFAULT_SIZE, OCI_HASH_POINTER);

    }

    call_status = (NULL != OCILib.key_map);

    /* create key */

    if (call_status)
    {
        key = OCI_ThreadKeyCreateInternal(destfunc);

        /* add key to internal key hash table */

        if (key)
        {
            call_status = OCI_HashAddPointer(OCILib.key_map, name, key);
        }
        else
        {
            call_status = FALSE;
        }
    }

    /* check errors */

    if (!call_status && key)
    {
        OCI_ThreadKeyFree(key);
    }

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
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

    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_STRING, name)

    key = (OCI_ThreadKey *) OCI_HashGetPointer(OCILib.key_map, name);

    call_retval = call_status = OCI_ThreadKeySet(key, value);

    OCI_LIB_CALL_EXIT()
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

    OCI_LIB_CALL_ENTER(void*, NULL)

    OCI_CHECK_PTR(OCI_IPC_STRING, name)

    key = (OCI_ThreadKey *) OCI_HashGetPointer(OCILib.key_map, name);

    call_status = OCI_ThreadKeyGet(key, &call_retval);

    OCI_LIB_CALL_EXIT()
}
