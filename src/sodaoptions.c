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

#include "sodaoptions.h"

#include "macros.h"
#include "memory.h"
#include "stringutils.h"

/* --------------------------------------------------------------------------------------------- *
 * OcilibSodaOptionsCreate
 * --------------------------------------------------------------------------------------------- */

OCI_SodaOptions * OcilibSodaOptionsCreate
(
    OCI_SodaDb *sdb
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_SodaOptions*, NULL,
        /* context */ OCI_IPC_SODA_DB, sdb
    );

    OCI_SodaOptions *options = NULL;

    CHECK_PTR(OCI_IPC_SODA_DB, sdb);
    CHECK_SODA_ENABLED(sdb->con);

    ALLOC_HANDLE(OCI_IPC_SODA_OPTIONS, options);
    options->sdb = sdb;

    CHECK
    (
        OcilibMemoryAllocHandle
        (
            sdb->env,
            (dvoid *)sdb->env->handle,
            (dvoid **)&options->handle,
            OCI_HTYPE_SODA_OPER_OPTIONS
        )
    );

    SET_RETVAL(options);

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE && NULL != options)
        {
            if (NULL != options->handle)
            {
                OcilibMemoryFreeHandle(sdb->env, (dvoid*)options->handle, OCI_HTYPE_SODA_OPER_OPTIONS);
            }
            OcilibErrorResetSource(NULL, options);
            FREE(options);
        }
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibSodaOptionsClearKeys
 * --------------------------------------------------------------------------------------------- */

static void OcilibSodaOptionsClearKeys
(
    OCI_SodaOptions *options
)
{
    OCI_UInt32 i;

    if (NULL == options)
    {
        return;
    }

    for (i = 0; i < options->key_count; i++)
    {
        if (NULL != options->keys[i])
        {
            OcilibMemoryFree(NULL != options->sdb ? options->sdb->env : NULL, options->keys[i]);
        }
    }

    if (NULL != options->keys)
    {
        OcilibMemoryFree(NULL != options->sdb ? options->sdb->env : NULL, options->keys);
    }

    options->keys      = NULL;
    options->key_count = 0;
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibSodaOptionsFree
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibSodaOptionsFree
(
    OCI_SodaOptions *options
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_SODA_OPTIONS, options
    );

    CHECK_PTR(OCI_IPC_SODA_OPTIONS, options);

    OcilibSodaOptionsClearKeys(options);

    if (NULL != options->handle)
    {
        OcilibMemoryFreeHandle(options->sdb->env, (dvoid*)options->handle, OCI_HTYPE_SODA_OPER_OPTIONS);
    }

    OcilibErrorResetSource(NULL, options);
    FREE(options);

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibSodaOptionsFreeKeysArray
 * --------------------------------------------------------------------------------------------- */

static void OcilibSodaOptionsFreeKeysArray
(
    OCI_Environment *env,
    OraText        **keys,
    ub4             *lengths,
    OCI_UInt32       count
)
{
    OCI_UInt32 i;

    if (NULL != keys)
    {
        for (i = 0; i < count; i++)
        {
            OcilibStringReleaseDBString(env, (dbtext*)keys[i]);
        }

        OcilibMemoryFree(env, keys);
    }

    if (NULL != lengths)
    {
        OcilibMemoryFree(env, lengths);
    }
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibSodaOptionsSetKeyAttr
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibSodaOptionsSetKeyAttr
(
    OCI_SodaOptions *options,
    const otext     *key
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_SODA_OPTIONS, options
    );

    dbtext   *dbstr  = NULL;
    OCI_Int32 dbsize = -1;

    CHECK_PTR(OCI_IPC_SODA_OPTIONS, options);
    CHECK_PTR(OCI_IPC_STRING, key);
    CHECK_PTR(OCI_IPC_SODA_DB, options->sdb);
    CHECK_SODA_ENABLED(options->sdb->con);

    dbstr = OcilibStringGetDBString(options->sdb->env, key, &dbsize);

    CHECK_ATTRIB_SET
    (
        OCI_HTYPE_SODA_OPER_OPTIONS,
        OCI_ATTR_SODA_KEY,
        options->handle,
        (void*)dbstr,
        (ub4)dbsize,
        options->sdb->con->err
    );

    SET_SUCCESS();

    CLEANUP_AND_EXIT_FUNC
    (
        OcilibStringReleaseDBString(NULL != options && NULL != options->sdb ? options->sdb->env : NULL, dbstr);
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibSodaOptionsSetKey
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibSodaOptionsSetKey
(
    OCI_SodaOptions *options,
    const otext     *key
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_SODA_OPTIONS, options
    );

    /* single key setting overrides any previously set key list */

    OcilibSodaOptionsClearKeys(options);

    CHECK(OcilibSodaOptionsSetKeyAttr(options, key));

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibSodaOptionsSetKeys
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibSodaOptionsSetKeys
(
    OCI_SodaOptions *options,
    const otext    **keys,
    OCI_UInt32       count
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_SODA_OPTIONS, options
    );

    OCI_UInt32 i         = 0;
    OraText  **dbkeys    = NULL;
    ub4       *dblengths = NULL;

    CHECK_PTR(OCI_IPC_SODA_OPTIONS, options);
    CHECK_PTR(OCI_IPC_VOID, keys);
    CHECK_PTR(OCI_IPC_SODA_DB, options->sdb);
    CHECK_SODA_ENABLED(options->sdb->con);

#ifdef OCI_CHARSET_WIDE

    /* OCISodaOperKeysSet() crashes under an OCI environment created with a
     * UTF-16 client charset (observed with Oracle client 23.6): the function
     * raises an access violation before processing its arguments. Multiple
     * key operations are therefore emulated in OCILIB by caching the keys and
     * iterating over them in the consuming calls (find, count, remove,
     * replace), which use the OCI_ATTR_SODA_KEY attribute that works fine
     * in UTF-16 environments.
     */

    OcilibSodaOptionsClearKeys(options);

    if (count > 0)
    {
        ALLOC_DATA(OCI_IPC_VOID, options->keys, count);

        for (i = 0; i < count; i++)
        {
            CHECK_PTR(OCI_IPC_STRING, keys[i]);

            options->keys[i] = OcilibStringDuplicate(options->sdb->env, keys[i]);
            CHECK(NULL != options->keys[i]);
        }

        options->key_count = count;
    }

#else

    ALLOC_DATA(OCI_IPC_VOID, dbkeys, count);
    ALLOC_DATA(OCI_IPC_VOID, dblengths, count);

    for (i = 0; i < count; i++)
    {
        OCI_Int32 dbsize = -1;

        CHECK_PTR(OCI_IPC_STRING, keys[i]);

        dbkeys[i]    = (OraText*)OcilibStringGetDBString(options->sdb->env, keys[i], &dbsize);
        dblengths[i] = (ub4)dbsize;
    }

    CHECK_OCI
    (
        options->sdb->con->err,
        OCISodaOperKeysSet,
        options->handle,
        dbkeys,
        dblengths,
        (ub4)count,
        options->sdb->con->err,
        OCI_DEFAULT
    );

#endif

    SET_SUCCESS();

    CLEANUP_AND_EXIT_FUNC
    (
        OcilibSodaOptionsFreeKeysArray
        (
            NULL != options && NULL != options->sdb ? options->sdb->env : NULL,
            dbkeys,
            dblengths,
            count
        );

        if (FAILURE)
        {
            OcilibSodaOptionsClearKeys(options);
        }
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibSodaOptionsSetVersion
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibSodaOptionsSetVersion
(
    OCI_SodaOptions *options,
    const otext     *version
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_SODA_OPTIONS, options
    );

    dbtext   *dbstr  = NULL;
    OCI_Int32 dbsize = -1;

    CHECK_PTR(OCI_IPC_SODA_OPTIONS, options);
    CHECK_PTR(OCI_IPC_STRING, version);
    CHECK_PTR(OCI_IPC_SODA_DB, options->sdb);
    CHECK_SODA_ENABLED(options->sdb->con);

    dbstr = OcilibStringGetDBString(options->sdb->env, version, &dbsize);

    CHECK_ATTRIB_SET
    (
        OCI_HTYPE_SODA_OPER_OPTIONS,
        OCI_ATTR_SODA_VERSION,
        options->handle,
        (void*)dbstr,
        (ub4)dbsize,
        options->sdb->con->err
    );

    SET_SUCCESS();

    CLEANUP_AND_EXIT_FUNC
    (
        OcilibStringReleaseDBString(NULL != options && NULL != options->sdb ? options->sdb->env : NULL, dbstr);
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibSodaOptionsSetFilter
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibSodaOptionsSetFilter
(
    OCI_SodaOptions *options,
    const otext     *filterSpec
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_SODA_OPTIONS, options
    );

    dbtext   *dbstr  = NULL;
    OCI_Int32 dbsize = -1;

    CHECK_PTR(OCI_IPC_SODA_OPTIONS, options);
    CHECK_PTR(OCI_IPC_STRING, filterSpec);
    CHECK_PTR(OCI_IPC_SODA_DB, options->sdb);
    CHECK_SODA_ENABLED(options->sdb->con);

    dbstr = OcilibStringGetDBString(options->sdb->env, filterSpec, &dbsize);

    CHECK_ATTRIB_SET
    (
        OCI_HTYPE_SODA_OPER_OPTIONS,
        OCI_ATTR_SODA_FILTER,
        options->handle,
        (void*)dbstr,
        (ub4)dbsize,
        options->sdb->con->err
    );

    SET_SUCCESS();

    CLEANUP_AND_EXIT_FUNC
    (
        OcilibStringReleaseDBString(NULL != options && NULL != options->sdb ? options->sdb->env : NULL, dbstr);
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibSodaOptionsSetSkip
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibSodaOptionsSetSkip
(
    OCI_SodaOptions *options,
    OCI_UInt32       count
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_SODA_OPTIONS, options
    );

    ub4 val = (ub4)count;

    CHECK_PTR(OCI_IPC_SODA_OPTIONS, options);
    CHECK_PTR(OCI_IPC_SODA_DB, options->sdb);
    CHECK_SODA_ENABLED(options->sdb->con);

    CHECK_ATTRIB_SET
    (
        OCI_HTYPE_SODA_OPER_OPTIONS,
        OCI_ATTR_SODA_SKIP,
        options->handle,
        &val,
        sizeof(val),
        options->sdb->con->err
    );

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibSodaOptionsSetLimit
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibSodaOptionsSetLimit
(
    OCI_SodaOptions *options,
    OCI_UInt32       count
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_SODA_OPTIONS, options
    );

    ub4 val = (ub4)count;

    CHECK_PTR(OCI_IPC_SODA_OPTIONS, options);
    CHECK_PTR(OCI_IPC_SODA_DB, options->sdb);
    CHECK_SODA_ENABLED(options->sdb->con);

    CHECK_ATTRIB_SET
    (
        OCI_HTYPE_SODA_OPER_OPTIONS,
        OCI_ATTR_SODA_LIMIT,
        options->handle,
        &val,
        sizeof(val),
        options->sdb->con->err
    );

    SET_SUCCESS();

    EXIT_FUNC();
}
