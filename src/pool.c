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

#include "pool.h"

#include "connection.h"
#include "list.h"
#include "macros.h"
#include "stringutils.h"

static OCI_UInt32 PoolTypeValues[] =
{
    OCI_POOL_CONNECTION, OCI_POOL_SESSION
};

/* --------------------------------------------------------------------------------------------- *
 * OcilibPoolDispose
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibPoolDispose
(
    OCI_Pool *pool
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_POOL, pool
    );

    CHECK_PTR(OCI_IPC_POOL, pool);

#if OCI_VERSION_COMPILE >= OCI_9_0

    if (pool->env->version_runtime >= OCI_9_0)
    {
        /* close pool handle */

        if (pool->handle)
        {
            if (OCI_HTYPE_CPOOL == pool->kind)
            {
                CHECK_OCI
                (
                    pool->err,
                    OCIConnectionPoolDestroy,
                    (OCICPool *) pool->handle,
                    pool->err, (ub4) OCI_DEFAULT
                );
            }

  #if OCI_VERSION_COMPILE >= OCI_9_2

            else
            {
                CHECK_OCI
                (
                    pool->err,
                    OCISessionPoolDestroy,
                    (OCISPool*)pool->handle,
                    pool->err, (ub4)OCI_SPD_FORCE
                );
            }

  #endif

            OcilibMemoryFreeHandle(pool->env, (void *) pool->handle, (ub4) pool->kind);
        }

  #if OCI_VERSION_COMPILE >= OCI_11_2

        /* close authentication handle */

        if (NULL != pool->authp)
        {
            OcilibMemoryFreeHandle(pool->env, (void *) pool->authp, OCI_HTYPE_AUTHINFO);
        }

  #endif

        /* close error handle */

        if (NULL != pool->err)
        {
            OcilibMemoryFreeHandle(pool->env, (void *) pool->err, OCI_HTYPE_ERROR);
        }
    }

#endif

    pool->err    = NULL;
    pool->handle = NULL;
    pool->authp  = NULL;

    /* free strings */

    FREE(pool->name);
    FREE(pool->db);
    FREE(pool->user);
    FREE(pool->pwd);

    OcilibErrorResetSource(NULL, pool);

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibPoolCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Pool * OcilibPoolCreate
(
    OCI_Environment *env,
    const otext     *db,
    const otext     *user,
    const otext     *pwd,
    OCI_PoolType     type,
    OCI_SessionMode  mode,
    OCI_UInt32       min_con,
    OCI_UInt32       max_con,
    OCI_UInt32       incr_con
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Pool*, NULL,
        /* context */ OCI_IPC_ENVIRONMENT, env
    );

    dbtext* dbstr_name = NULL;
    dbtext* dbstr_db   = NULL;
    dbtext* dbstr_user = NULL;
    dbtext* dbstr_pwd  = NULL;

    OCI_Int32 dbsize_name = -1;
    OCI_Int32 dbsize_db   = -1;
    OCI_Int32 dbsize_user = -1;
    OCI_Int32 dbsize_pwd  = -1;

    OCI_Pool* pool = NULL;

    CHECK_PTR(OCI_IPC_ENVIRONMENT, env);
    CHECK_ENUM_VALUE( type, PoolTypeValues, OTEXT("Pool Type"));
    CHECK_MIN(max_con, 1);

    /* create pool object */

    LIST_ATOMIC_ADD(env->pools, pool);

    CHECK_NULL(pool);

    pool->env   = env;
    pool->htype = OCI_IPC_POOL;

    pool->mode = mode;
    pool->min  = min_con;
    pool->max  = max_con;
    pool->incr = incr_con;

    pool->db   = OcilibStringDuplicate(pool->env, db   ? db   : OTEXT(""));
    pool->user = OcilibStringDuplicate(pool->env, user ? user : OTEXT(""));
    pool->pwd  = OcilibStringDuplicate(pool->env, pwd  ? pwd  : OTEXT(""));

#if OCI_VERSION_COMPILE < OCI_9_2

    type = OCI_POOL_CONNECTION;

#endif

#if OCI_VERSION_COMPILE >= OCI_9_0

    if (OCI_POOL_CONNECTION == type)
    {
        pool->kind = OCI_HTYPE_CPOOL;
    }

  #if OCI_VERSION_COMPILE >= OCI_9_2

    else
    {
        pool->kind = OCI_HTYPE_SPOOL;
    }

  #endif

    if (env->version_runtime >= OCI_9_0)
    {
        /* allocate error handle */

        CHECK(OcilibMemoryAllocHandle(env, env->handle,
                                      (dvoid **)(void *)&pool->err,
                                      OCI_HTYPE_ERROR));

        /* allocate pool handle */

        CHECK(OcilibMemoryAllocHandle(env, env->handle,
                                      (dvoid **)(void *)&pool->handle,
                                      (ub4)pool->kind));

        /* allocate authentication handle only if needed */

  #if OCI_VERSION_COMPILE >= OCI_11_1

        if ((OCI_HTYPE_SPOOL == pool->kind) && (env->version_runtime >= OCI_11_1))
        {
            otext driver_version[OCI_SIZE_FORMAT];

            OcilibStringFormat
            (
                driver_version,
                osizeof(driver_version) - (OCI_Size)1,
                OTEXT("%s : %d.%d.%d"),
                OCILIB_DRIVER_NAME,
                OCILIB_MAJOR_VERSION,
                OCILIB_MINOR_VERSION,
                OCILIB_REVISION_VERSION
            );

            /* allocate authentication handle */

            CHECK(OcilibMemoryAllocHandle(env, env->handle,
                                          (dvoid **)(void *)&pool->authp,
                                          OCI_HTYPE_AUTHINFO));

            /* set OCILIB driver layer name attribute only for session pools here
                For standalone connections and connection pool this attribute is set
                in OCI_ConnectionLogon() */

            CHECK_ATTRIB_SET
            (
                OCI_HTYPE_AUTHINFO, OCI_ATTR_DRIVER_NAME,
                pool->authp, (driver_version), OcilibStringDBSize(driver_version),
                pool->err
            );

            /* set authentication handle on the session pool */

            CHECK_ATTRIB_SET
            (
                OCI_HTYPE_SPOOL, OCI_ATTR_SPOOL_AUTH,
                pool->handle, pool->authp,
                sizeof(pool->authp),
                pool->err
            );
        }

  #endif

        /* create the pool */

        dbstr_db = (dbtext *) (pool->db);
        dbsize_db = OcilibStringDBSize(pool->db);
        dbstr_user = (dbtext *) (pool->user);
        dbsize_user = OcilibStringDBSize(pool->user);
        dbstr_pwd = (dbtext *) (pool->pwd);
        dbsize_pwd = OcilibStringDBSize(pool->pwd);

        if (OCI_HTYPE_CPOOL == pool->kind)
        {
            CHECK_OCI
            (
                pool->err,
                OCIConnectionPoolCreate,
                pool->env->handle, pool->err, (OCICPool *)pool->handle,
                (OraText **) (dvoid *) &dbstr_name,
                (sb4*) &dbsize_name,
                (OraText *) dbstr_db, (sb4) dbsize_db,
                (ub4)pool->min, (ub4)pool->max,
                (ub4)pool->incr, (OraText *) dbstr_user,
                (sb4) dbsize_user, (OraText *) dbstr_pwd,
                (sb4) dbsize_pwd,  (ub4) OCI_DEFAULT
            );
        }

  #if OCI_VERSION_COMPILE >= OCI_9_2

        else
        {
            ub4 sess_mode = OCI_DEFAULT;

            if (!(pool->mode & OCI_SESSION_SYSDBA) &&
                IS_STRING_VALID(pool->user) &&
                IS_STRING_VALID(pool->pwd))
            {
                sess_mode |= OCI_SPC_HOMOGENEOUS;
            }

            CHECK_OCI
            (
                pool->err,
                OCISessionPoolCreate,
                pool->env->handle, pool->err, (OCISPool *)pool->handle,
                (OraText **) (dvoid *) &dbstr_name,
                (ub4*) &dbsize_name,
                (OraText *) dbstr_db, (ub4) dbsize_db,
                (ub4)pool->min, (ub4)pool->max,
                (ub4)pool->incr, (OraText *) dbstr_user,
                (ub4) dbsize_user, (OraText *) dbstr_pwd,
                (ub4) dbsize_pwd,  (ub4) sess_mode
            );
        }

  #endif

        if (NULL != dbstr_name)
        {
            pool->name = OcilibStringDuplicateFromDBString(pool->env, dbstr_name, dbcharcount(dbsize_name));

            CHECK(NULL != pool->name);
        }
    }

#endif

    /* on success, we allocate internal OCI connection objects for pool
       minimum size */

#if OCI_VERSION_COMPILE >= OCI_9_0

    /* retrieve statement cache size */

    OcilibPoolGetStatementCacheSize(pool);

    /* for connection pools that do not handle the statement cache
       attribute, let's set the value with documented default cache size */

    if (pool->cache_size == 0)
    {
        CHECK(OcilibPoolSetStatementCacheSize(pool, OCI_DEFAUT_STMT_CACHE_SIZE));
    }

#endif

    CLEANUP_AND_EXIT_FUNC
    (

        if (FAILURE)
        {
            OcilibPoolFree(env, pool);
            pool = NULL;
        }

        if (pool)
        {
            OCILIB_LOG(OCI_LOG_CAT_LIB, OCI_LOG_INFO,
                       OTEXT("pool created (min=%u, max=%u, incr=%u)"),
                       (OCI_UInt32) pool->min,
                       (OCI_UInt32) pool->max,
                       (OCI_UInt32) pool->incr);
        }

        SET_RETVAL(pool);
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibPoolFree
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibPoolFree
(
    OCI_Environment *env,
    OCI_Pool        *pool
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_POOL, pool
    );

    CHECK_PTR(OCI_IPC_POOL, pool);

    LIST_ATOMIC_REMOVE(env->pools, pool, OcilibPoolDispose);

    FREE(pool);

    OCILIB_LOG(OCI_LOG_CAT_LIB, OCI_LOG_INFO, OTEXT("pool freed"));

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibPoolGetConnection
 * --------------------------------------------------------------------------------------------- */

OCI_Connection * OcilibPoolGetConnection
(
    OCI_Environment *env,
    OCI_Pool        *pool,
    const otext     *tag,
    OCI_ShardingKey *skey,
    OCI_ShardingKey *sskey
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Connection*, NULL,
        /* context */ OCI_IPC_POOL, pool
    );

    CHECK_PTR(OCI_IPC_POOL, pool);

    (void) env;

    OCI_Connection *con = OcilibConnectionCreateInternal(pool->env, pool, pool->db, pool->user,
                                                         pool->pwd, pool->mode, tag, skey, sskey);
    CHECK_NULL(con);

    /* for regular connection pool, set the statement cache size to
       retrieved connection */

#if OCI_VERSION_COMPILE >= OCI_10_1

    const OCI_UInt32 cache_size = OcilibPoolGetStatementCacheSize(pool);

    CHECK(OcilibConnectionSetStatementCacheSize(con, cache_size));

#endif

    OCILIB_LOG(OCI_LOG_CAT_LIB, OCI_LOG_INFO,
               OTEXT("connection acquired from pool: %s/%s"),
               con->user ? con->user : OTEXT(""),
               con->db ? con->db : OTEXT(""));

    SET_RETVAL(con);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibPoolGetTimeout
 * --------------------------------------------------------------------------------------------- */

OCI_UInt32 OcilibPoolGetTimeout
(
    OCI_Pool *pool
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_UInt32, 0,
        /* context */ OCI_IPC_POOL, pool
    );

    ub4 value = 0;

    CHECK_PTR(OCI_IPC_POOL, pool);

#if OCI_VERSION_COMPILE >= OCI_9_0

    if (pool->env->version_runtime >= OCI_9_0)
    {
        ub4 attr = 0;

        if (OCI_HTYPE_CPOOL == pool->kind)
        {
            attr = OCI_ATTR_CONN_TIMEOUT;
        }

  #if OCI_VERSION_COMPILE >= OCI_9_2

        else
        {
            attr = OCI_ATTR_SPOOL_TIMEOUT;
        }

  #endif

        CHECK_ATTRIB_GET
        (
            pool->kind, attr,
            pool->handle, &value, NULL,
            pool->err
        );
    }

#endif

    SET_RETVAL((OCI_UInt32)value);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibPoolSetTimeout
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibPoolSetTimeout
(
    OCI_Pool  *pool,
    OCI_UInt32 value
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_POOL, pool
    );

    CHECK_PTR(OCI_IPC_POOL, pool);

#if OCI_VERSION_COMPILE >= OCI_9_0

    if (pool->env->version_runtime >= OCI_9_0)
    {
        ub4 timeout = value;
        ub4 attr    = 0;

        if (OCI_HTYPE_CPOOL == pool->kind)
        {
            attr = OCI_ATTR_CONN_TIMEOUT;
        }

  #if OCI_VERSION_COMPILE >= OCI_9_2

        else
        {
            attr = OCI_ATTR_SPOOL_TIMEOUT;
        }

  #endif

        CHECK_ATTRIB_SET
        (
            pool->kind, attr, pool->handle,
            &timeout, sizeof(timeout),
            pool->err
        );
    }

#endif

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibPoolGetNoWait
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibPoolGetNoWait
(
    OCI_Pool *pool
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_POOL, pool
    );

    ub1 value = 0;

    CHECK_PTR(OCI_IPC_POOL, pool);

#if OCI_VERSION_COMPILE >= OCI_9_0

    if (pool->env->version_runtime >= OCI_9_0)
    {
        ub4 attr = 0;

        if (OCI_HTYPE_CPOOL == pool->kind)
        {
            attr = OCI_ATTR_CONN_NOWAIT;
        }

  #if OCI_VERSION_COMPILE >= OCI_9_2

        else
        {
            attr = OCI_ATTR_SPOOL_GETMODE;

        }

  #endif

        CHECK_ATTRIB_GET
        (
            pool->kind, attr,
            pool->handle, &value, NULL,
            pool->err
        );
    }

#endif

    SET_RETVAL((OCI_Boolean)value);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibPoolSetNoWait
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibPoolSetNoWait
(
    OCI_Pool   *pool,
    OCI_Boolean value
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_POOL, pool
    );

    CHECK_PTR(OCI_IPC_POOL, pool);

#if OCI_VERSION_COMPILE >= OCI_9_0

    if (pool->env->version_runtime >= OCI_9_0)
    {
        ub1 nowait = (ub1) value;
        ub4 attr   = 0;

        if (OCI_HTYPE_CPOOL == pool->kind)
        {
            attr = OCI_ATTR_CONN_NOWAIT;
        }

  #if OCI_VERSION_COMPILE >= OCI_9_2

        else
        {
            attr = OCI_ATTR_SPOOL_GETMODE;

            nowait = (ub1) (value ? OCI_SPOOL_ATTRVAL_NOWAIT : OCI_SPOOL_ATTRVAL_WAIT);

        }

  #endif

        CHECK_ATTRIB_SET
        (
            pool->kind, attr,
            pool->handle, &nowait, sizeof(nowait),
            pool->err
        );
    }

#endif

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibPoolGetBusyCount
 * --------------------------------------------------------------------------------------------- */

OCI_UInt32 OcilibPoolGetBusyCount
(
    OCI_Pool *pool
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_UInt32, 0,
        /* context */ OCI_IPC_POOL, pool
    );

    ub4 value = 0;

    CHECK_PTR(OCI_IPC_POOL, pool);

#if OCI_VERSION_COMPILE >= OCI_9_0

    if (pool->env->version_runtime >= OCI_9_0)
    {
        ub4 attr = 0;

        if (OCI_HTYPE_CPOOL == pool->kind)
        {
            attr = (ub4) OCI_ATTR_CONN_BUSY_COUNT;
        }

  #if OCI_VERSION_COMPILE >= OCI_9_2

        else
        {
            attr = (ub4) OCI_ATTR_SPOOL_BUSY_COUNT;
        }

  #endif

        CHECK_ATTRIB_GET
        (
            pool->kind, attr,
            pool->handle, &value, NULL,
            pool->err
        );
    }

#endif

    SET_RETVAL(value);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibPoolGetOpenedCount
 * --------------------------------------------------------------------------------------------- */

OCI_UInt32 OcilibPoolGetOpenedCount
(
    OCI_Pool *pool
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_UInt32, 0,
        /* context */ OCI_IPC_POOL, pool
    );

    ub4 value = 0;

    CHECK_PTR(OCI_IPC_POOL, pool);

#if OCI_VERSION_COMPILE >= OCI_9_0

    if (pool->env->version_runtime >= OCI_9_0)
    {
        ub4 attr = 0;

        if (OCI_HTYPE_CPOOL == pool->kind)
        {
            attr = OCI_ATTR_CONN_OPEN_COUNT;
        }

  #if OCI_VERSION_COMPILE >= OCI_9_2

        else
        {
            attr = OCI_ATTR_SPOOL_OPEN_COUNT;
        }

  #endif

        CHECK_ATTRIB_GET
        (
            pool->kind, attr,
            pool->handle, &value, NULL,
            pool->err
        );
    }

#endif

    SET_RETVAL(value);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibPoolGetMin
 * --------------------------------------------------------------------------------------------- */

OCI_UInt32 OcilibPoolGetMin
(
    OCI_Pool *pool
)
{
    GET_PROP
    (
        OCI_UInt32, 0,
        OCI_IPC_POOL, pool,
        min
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibPoolGetMax
 * --------------------------------------------------------------------------------------------- */

OCI_UInt32 OcilibPoolGetMax
(
    OCI_Pool *pool
)
{
    GET_PROP
    (
        OCI_UInt32, 0,
        OCI_IPC_POOL, pool,
        max
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibPoolGetIncrement
 * --------------------------------------------------------------------------------------------- */

OCI_UInt32 OcilibPoolGetIncrement
(
    OCI_Pool *pool
)
{
    GET_PROP
    (
        OCI_UInt32, 0,
        OCI_IPC_POOL, pool,
        incr
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibPoolSetStatementCacheSize
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibPoolSetStatementCacheSize
(
    OCI_Pool  *pool,
    OCI_UInt32 value
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_POOL, pool
    );

    ub4 cache_size = value;

    CHECK_PTR(OCI_IPC_POOL, pool);

#if OCI_VERSION_COMPILE >= OCI_10_1

    if (pool->env->version_runtime >= OCI_10_1)
    {
        if (OCI_HTYPE_SPOOL == pool->kind)
        {
            CHECK_ATTRIB_SET
            (
                pool->kind, OCI_ATTR_SPOOL_STMTCACHESIZE,
                pool->handle, &cache_size, sizeof(cache_size),
                pool->err
            );
        }
    }

#endif

    pool->cache_size = cache_size;

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibPoolGetStatementCacheSize
 * --------------------------------------------------------------------------------------------- */

OCI_UInt32 OcilibPoolGetStatementCacheSize
(
    OCI_Pool *pool
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_UInt32, 0,
        /* context */ OCI_IPC_POOL, pool
    );
    ub4 cache_size = 0;

    CHECK_PTR(OCI_IPC_POOL, pool);

#if OCI_VERSION_COMPILE >= OCI_10_1

    if (pool->env->version_runtime >= OCI_10_1)
    {
        if (OCI_HTYPE_SPOOL == pool->kind)
        {
            CHECK_ATTRIB_GET
            (
                pool->kind, OCI_ATTR_SPOOL_STMTCACHESIZE,
                pool->handle, &cache_size, NULL,
                pool->err
            );
        }
        else
        {
            cache_size = pool->cache_size;
        }

        pool->cache_size = cache_size;
    }

#else

    OCI_NOT_USED(cache_size);

#endif

    SET_RETVAL(pool->cache_size);

    EXIT_FUNC();
}
