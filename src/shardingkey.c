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

#include "shardingkey.h"

#include "macros.h"
#include "memory.h"
#include "stringutils.h"

/* --------------------------------------------------------------------------------------------- *
 * OcilibShardingKeyCreate
 * --------------------------------------------------------------------------------------------- */

OCI_ShardingKey * OcilibShardingKeyCreate
(
    OCI_Environment *env
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_ShardingKey*, NULL,
        /* context */ OCI_IPC_ENVIRONMENT, env
    );

    OCI_ShardingKey *skey = NULL;

    CHECK_PTR(OCI_IPC_ENVIRONMENT, env);
    CHECK_ENV_FEATURE(OCI_FEATURE_SHARDING, OCI_12_2);

#if OCI_VERSION_COMPILE >= OCI_12_2

    ALLOC_HANDLE(OCI_IPC_SHARDING_KEY, skey);

    skey->env = env;

    /* allocate OCI sharding key descriptor */

    CHECK
    (
        OcilibMemoryAllocDescriptor
        (
            env,
            (dvoid *)env->handle,
            (dvoid **)(void *)&skey->handle,
            (ub4)OCI_DTYPE_SHARDING_KEY
        )
    );

#endif

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE)
        {
            OcilibShardingKeyFree(skey);
            skey = NULL;
        }

        SET_RETVAL(skey);
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibShardingKeyFree
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibShardingKeyFree
(
    OCI_ShardingKey *skey
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_SHARDING_KEY, skey
    );

    CHECK_PTR(OCI_IPC_SHARDING_KEY, skey);

#if OCI_VERSION_COMPILE >= OCI_12_2

    if (NULL != skey->handle)
    {
        OcilibMemoryFreeDescriptor(skey->env, (dvoid *)skey->handle, OCI_DTYPE_SHARDING_KEY);
    }

#endif

    OcilibErrorResetSource(NULL, skey);

    FREE(skey->str);
    FREE(skey);

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibShardingKeyReset
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibShardingKeyReset
(
    OCI_ShardingKey *skey
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_SHARDING_KEY, skey
    );

    CHECK_PTR(OCI_IPC_SHARDING_KEY, skey);
    CHECK_ENV_FEATURE(OCI_FEATURE_SHARDING, OCI_12_2);

#if OCI_VERSION_COMPILE >= OCI_12_2

    CHECK_OCI
    (
        skey->env->err,
        OCIShardingKeyReset,
        skey->handle,
        skey->env->err,
        OCI_DEFAULT
    );

    FREE(skey->str);

    SET_SUCCESS();

#else

    CHECK(FALSE);

#endif

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibShardingKeyAddString
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibShardingKeyAddString
(
    OCI_ShardingKey *skey,
    const otext     *value
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_SHARDING_KEY, skey
    );

    OCI_Int32 dbsize = -1;
    dbtext   *dbstr  = NULL;

    CHECK_PTR(OCI_IPC_SHARDING_KEY, skey);
    CHECK_PTR(OCI_IPC_STRING,       value);
    CHECK_ENV_FEATURE(OCI_FEATURE_SHARDING, OCI_12_2);

#if OCI_VERSION_COMPILE >= OCI_12_2

    dbstr = OcilibStringGetDBString(skey->env, value, &dbsize);

    CHECK_OCI
    (
        skey->env->err,
        OCIShardingKeyColumnAdd,
        skey->handle,
        skey->env->err,
        dbstr,
        (ub4)dbsize,
        (ub2)SQLT_STR,
        OCI_DEFAULT
    );

    SET_SUCCESS();

#else

    OCI_NOT_USED(value);

    CHECK(FALSE);

#endif

    CLEANUP_AND_EXIT_FUNC
    (
        OcilibStringReleaseDBString(skey->env, dbstr);
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibShardingKeyAddNumber
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibShardingKeyAddNumber
(
    OCI_ShardingKey *skey,
    OCI_Number      *value
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_SHARDING_KEY, skey
    );

    CHECK_PTR(OCI_IPC_SHARDING_KEY, skey);
    CHECK_PTR(OCI_IPC_NUMBER,       value);
    CHECK_ENV_FEATURE(OCI_FEATURE_SHARDING, OCI_12_2);

#if OCI_VERSION_COMPILE >= OCI_12_2

    CHECK_OCI
    (
        skey->env->err,
        OCIShardingKeyColumnAdd,
        skey->handle,
        skey->env->err,
        value->handle,
        (ub4)sizeof(OCINumber),
        (ub2)SQLT_VNU,
        OCI_DEFAULT
    );

    SET_SUCCESS();

#else

    OCI_NOT_USED(value);

    CHECK(FALSE);

#endif

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibShardingKeyAddDate
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibShardingKeyAddDate
(
    OCI_ShardingKey *skey,
    OCI_Date        *value
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_SHARDING_KEY, skey
    );

    CHECK_PTR(OCI_IPC_SHARDING_KEY, skey);
    CHECK_PTR(OCI_IPC_DATE,         value);
    CHECK_ENV_FEATURE(OCI_FEATURE_SHARDING, OCI_12_2);

#if OCI_VERSION_COMPILE >= OCI_12_2

    CHECK_OCI
    (
        skey->env->err,
        OCIShardingKeyColumnAdd,
        skey->handle,
        skey->env->err,
        value->handle,
        (ub4)sizeof(OCIDate),
        (ub2)SQLT_DAT,
        OCI_DEFAULT
    );

    SET_SUCCESS();

#else

    OCI_NOT_USED(value);

    CHECK(FALSE);

#endif

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibShardingKeyAddTimestamp
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibShardingKeyAddTimestamp
(
    OCI_ShardingKey *skey,
    OCI_Timestamp   *value
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_SHARDING_KEY, skey
    );

    CHECK_PTR(OCI_IPC_SHARDING_KEY, skey);
    CHECK_PTR(OCI_IPC_TIMESTAMP,    value);
    CHECK_ENV_FEATURE(OCI_FEATURE_SHARDING, OCI_12_2);

#if OCI_VERSION_COMPILE >= OCI_12_2

    CHECK_OCI
    (
        skey->env->err,
        OCIShardingKeyColumnAdd,
        skey->handle,
        skey->env->err,
        value->handle,
        (ub4)sizeof(value->handle),
        (ub2)SQLT_TIMESTAMP,
        OCI_DEFAULT
    );

    SET_SUCCESS();

#else

    OCI_NOT_USED(value);

    CHECK(FALSE);

#endif

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibShardingKeyAddRaw
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibShardingKeyAddRaw
(
    OCI_ShardingKey *skey,
    const void      *value,
    OCI_UInt32       size
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_SHARDING_KEY, skey
    );

    CHECK_PTR(OCI_IPC_SHARDING_KEY, skey);
    CHECK_PTR(OCI_IPC_VOID,         value);
    CHECK_ENV_FEATURE(OCI_FEATURE_SHARDING, OCI_12_2);

#if OCI_VERSION_COMPILE >= OCI_12_2

    CHECK_OCI
    (
        skey->env->err,
        OCIShardingKeyColumnAdd,
        skey->handle,
        skey->env->err,
        (void *)value,
        (ub4)size,
        (ub2)SQLT_BIN,
        OCI_DEFAULT
    );

    SET_SUCCESS();

#else

    OCI_NOT_USED(value);
    OCI_NOT_USED(size);

    CHECK(FALSE);

#endif

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibShardingKeyToText
 * --------------------------------------------------------------------------------------------- */

const otext * OcilibShardingKeyToText
(
    OCI_ShardingKey *skey
)
{
    ENTER_FUNC
    (
        /* returns */ const otext*, NULL,
        /* context */ OCI_IPC_SHARDING_KEY, skey
    );

    CHECK_PTR(OCI_IPC_SHARDING_KEY, skey);
    CHECK_ENV_FEATURE(OCI_FEATURE_SHARDING, OCI_12_2);

#if OCI_VERSION_COMPILE >= OCI_12_2

    oratext *b64     = NULL;
    ub4      b64size = 0;

    CHECK_ATTRIB_GET
    (
        OCI_DTYPE_SHARDING_KEY,
        OCI_ATTR_SHARDING_KEY_B64,
        skey->handle,
        &b64,
        &b64size,
        skey->env->err
    );

    FREE(skey->str);

    if (NULL != b64 && b64size > 0)
    {
        /* the base64 representation is a single byte oratext string,
           it requires a conversion to otext in wide charset builds */

        ALLOC_DATA(OCI_IPC_STRING, skey->str, (OCI_Size)b64size + 1);
        OcilibStringAnsiToNative((const char *)b64, skey->str, (OCI_Int32)b64size);
    }

    SET_RETVAL(skey->str);

#else

    CHECK(FALSE);

#endif

    EXIT_FUNC();
}
