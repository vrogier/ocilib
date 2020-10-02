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

#include "pool.h"

#include "connection.h"
#include "list.h"
#include "macros.h"
#include "stringutils.h"

static unsigned int PoolTypeValues[] =
{
    OCI_POOL_CONNECTION, OCI_POOL_SESSION
};

/* --------------------------------------------------------------------------------------------- *
 * OcilibPoolDispose
 * --------------------------------------------------------------------------------------------- */

boolean OcilibPoolDispose
(
    OCI_Pool *pool
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_POOL, pool
    )

#if OCI_VERSION_COMPILE >= OCI_9_0

    if (Env.version_runtime >= OCI_9_0)
    {
        /* close pool handle */

        if (pool->handle)
        {
            if (OCI_HTYPE_CPOOL == pool->htype)
            {
                CHECK_OCI
                (
                    pool->err,
                    OCIConnectionPoolDestroy,
                    (OCICPool *) pool->handle,
                    pool->err, (ub4) OCI_DEFAULT
                )
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
                )
            }

  #endif

            OcilibMemoryFreeHandle((void *) pool->handle, (ub4) pool->htype);
        }

  #if OCI_VERSION_COMPILE >= OCI_11_2

        /* close authentication handle */

        if (NULL != pool->authp)
        {
            OcilibMemoryFreeHandle((void *) pool->authp, OCI_HTYPE_AUTHINFO);
        }

  #endif

        /* close error handle */

        if (NULL != pool->err)
        {
            OcilibMemoryFreeHandle((void *) pool->err, OCI_HTYPE_ERROR);
        }
    }

#endif

    pool->err    = NULL;
    pool->handle = NULL;
    pool->authp  = NULL;

    /* free strings */

    FREE(pool->name)
    FREE(pool->db)
    FREE(pool->user)
    FREE(pool->pwd)

    OcilibErrorResetSource(NULL, pool);

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibPoolCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Pool * OcilibPoolCreate
(
    const otext *db,
    const otext *user,
    const otext *pwd,
    unsigned int type,
    unsigned int mode,
    unsigned int min_con,
    unsigned int max_con,
    unsigned int incr_con
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Pool*, NULL,
        /* context */ OCI_IPC_VOID, &Env
    )

    dbtext *dbstr = NULL;
    dbtext* dbstr_name = NULL;
    dbtext* dbstr_db   = NULL;
    dbtext* dbstr_user = NULL;
    dbtext* dbstr_pwd  = NULL;

    int dbsize      = -1;
    int dbsize_name = -1;
    int dbsize_db   = -1;
    int dbsize_user = -1;
    int dbsize_pwd  = -1;

    OCI_Pool* pool = NULL;

    CHECK_INITIALIZED()
    CHECK_ENUM_VALUE( type, PoolTypeValues, OTEXT("Pool Type"))
    CHECK_MIN(max_con, 1)

    /* make sure that we do not have a XA session flag */

    mode &= ~OCI_SESSION_XA;

    /* create pool object */

    pool = OcilibListAppend(Env.pools, sizeof(*pool));
    CHECK_NULL(pool)

    pool->mode = mode;
    pool->min  = min_con;
    pool->max  = max_con;
    pool->incr = incr_con;

    pool->db   = ostrdup(db   ? db   : OTEXT(""));
    pool->user = ostrdup(user ? user : OTEXT(""));
    pool->pwd  = ostrdup(pwd  ? pwd  : OTEXT(""));

#if OCI_VERSION_COMPILE < OCI_9_2

    type = OCI_POOL_CONNECTION;

#endif

#if OCI_VERSION_COMPILE >= OCI_9_0

    if (OCI_POOL_CONNECTION == type)
    {
        pool->htype = OCI_HTYPE_CPOOL;
    }

  #if OCI_VERSION_COMPILE >= OCI_9_2

    else
    {
        pool->htype = OCI_HTYPE_SPOOL;
    }

  #endif

    if (Env.version_runtime >= OCI_9_0)
    {
        /* allocate error handle */

        CHECK(OcilibMemoryAllocHandle((dvoid *)Env.env,
                                      (dvoid **)(void *)&pool->err,
                                      OCI_HTYPE_ERROR))

        /* allocate pool handle */

        CHECK(OcilibMemoryAllocHandle((dvoid *)Env.env,
                                      (dvoid **)(void *)&pool->handle,
                                      (ub4)pool->htype))

        /* allocate authentication handle only if needed */

  #if OCI_VERSION_COMPILE >= OCI_11_1

        if ((OCI_HTYPE_SPOOL == pool->htype) && (Env.version_runtime >= OCI_11_1))
        {
            otext driver_version[OCI_SIZE_FORMAT];

            osprintf(driver_version,
                     osizeof(driver_version) - (size_t)1,
                     OTEXT("%s : %d.%d.%d"),
                     OCILIB_DRIVER_NAME,
                     OCILIB_MAJOR_VERSION,
                     OCILIB_MINOR_VERSION,
                     OCILIB_REVISION_VERSION);

            dbstr = OcilibStringGetDBString(driver_version, &dbsize);

            /* allocate authentication handle */

            CHECK(OcilibMemoryAllocHandle((dvoid *)Env.env,
                                          (dvoid **)(void *)&pool->authp,
                                          OCI_HTYPE_AUTHINFO))

            /* set OCILIB driver layer name attribute only for session pools here
                For standalone connections and connection pool this attribute is set
                in OCI_ConnectionLogon() */

            CHECK_ATTRIB_SET
            (
                OCI_HTYPE_AUTHINFO, OCI_ATTR_DRIVER_NAME,
                pool->authp, dbstr, dbsize,
                pool->err
            )

            /* set authentication handle on the session pool */

            CHECK_ATTRIB_SET
            (
                OCI_HTYPE_SPOOL, OCI_ATTR_SPOOL_AUTH,
                pool->handle, pool->authp,
                sizeof(pool->authp),
                pool->err
            )
        }

  #endif

        /* create the pool */

        dbstr_db   = OcilibStringGetDBString(pool->db,   &dbsize_db);
        dbstr_user = OcilibStringGetDBString(pool->user, &dbsize_user);
        dbstr_pwd  = OcilibStringGetDBString(pool->pwd,  &dbsize_pwd);

        if (OCI_HTYPE_CPOOL == pool->htype)
        {
            CHECK_OCI
            (
                pool->err,
                OCIConnectionPoolCreate,
                Env.env, pool->err, (OCICPool *)pool->handle,
                (OraText **) (dvoid *) &dbstr_name,
                (sb4*) &dbsize_name,
                (OraText *) dbstr_db, (sb4) dbsize_db,
                (ub4)pool->min, (ub4)pool->max,
                (ub4)pool->incr, (OraText *) dbstr_user,
                (sb4) dbsize_user, (OraText *) dbstr_pwd,
                (sb4) dbsize_pwd,  (ub4) OCI_DEFAULT
            )
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
                Env.env, pool->err, (OCISPool *)pool->handle,
                (OraText **) (dvoid *) &dbstr_name,
                (ub4*) &dbsize_name,
                (OraText *) dbstr_db, (sb4) dbsize_db,
                (ub4)pool->min, (ub4)pool->max,
                (ub4)pool->incr, (OraText *) dbstr_user,
                (sb4) dbsize_user, (OraText *) dbstr_pwd,
                (sb4) dbsize_pwd,  (ub4) sess_mode
            )
        }

  #endif

        if (NULL != dbstr_name)
        {
            pool->name = OcilibStringDuplicateFromDBString(dbstr_name, dbcharcount(dbsize_name));

            CHECK(NULL != pool->name)
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
        CHECK(OcilibPoolSetStatementCacheSize(pool, OCI_DEFAUT_STMT_CACHE_SIZE))
    }

#endif

    CLEANUP_AND_EXIT_FUNC
    (
        OcilibStringReleaseDBString(dbstr);
        OcilibStringReleaseDBString(dbstr_db);
        OcilibStringReleaseDBString(dbstr_user);
        OcilibStringReleaseDBString(dbstr_pwd);

        if (FAILURE)
        {
            OcilibPoolFree(pool);
            pool = NULL;
        }

        SET_RETVAL(pool)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibPoolFree
 * --------------------------------------------------------------------------------------------- */

boolean OcilibPoolFree
(
    OCI_Pool *pool
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_POOL, pool
    )

    CHECK_PTR(OCI_IPC_POOL, pool)

    OcilibPoolDispose(pool);
    OcilibListRemove(Env.pools, pool);

    FREE(pool)

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibPoolGetConnection
 * --------------------------------------------------------------------------------------------- */

OCI_Connection * OcilibPoolGetConnection
(
    OCI_Pool    *pool,
    const otext *tag
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Connection*, NULL,
        /* context */ OCI_IPC_POOL, pool
    )

    CHECK_PTR(OCI_IPC_POOL, pool)

    OCI_Connection *con = OcilibConnectionCreateInternal(pool, pool->db, pool->user,
                                                         pool->pwd, pool->mode, tag);
    CHECK_NULL(con)

    /* for regular connection pool, set the statement cache size to
       retrieved connection */

#if OCI_VERSION_COMPILE >= OCI_10_1

    const unsigned int cache_size = OcilibPoolGetStatementCacheSize(pool);

    CHECK(OcilibConnectionSetStatementCacheSize(con, cache_size))

#endif

    SET_RETVAL(con)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibPoolGetTimeout
 * --------------------------------------------------------------------------------------------- */

unsigned int OcilibPoolGetTimeout
(
    OCI_Pool *pool
)
{
    ENTER_FUNC
    (
        /* returns */ unsigned int, 0,
        /* context */ OCI_IPC_POOL, pool
    )

    ub4 value = 0;

    CHECK_PTR(OCI_IPC_POOL, pool)

#if OCI_VERSION_COMPILE >= OCI_9_0

    if (Env.version_runtime >= OCI_9_0)
    {
        ub4 attr = 0;

        if (OCI_HTYPE_CPOOL == pool->htype)
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
            pool->htype, attr,
            pool->handle, &value, NULL,
            pool->err
        )
    }

#endif

    SET_RETVAL((unsigned int)value)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibPoolSetTimeout
 * --------------------------------------------------------------------------------------------- */

boolean OcilibPoolSetTimeout
(
    OCI_Pool    *pool,
    unsigned int value
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_POOL, pool
    )

    CHECK_PTR(OCI_IPC_POOL, pool)

#if OCI_VERSION_COMPILE >= OCI_9_0

    if (Env.version_runtime >= OCI_9_0)
    {
        ub4 timeout = value;
        ub4 attr    = 0;

        if (OCI_HTYPE_CPOOL == pool->htype)
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
            pool->htype, attr, pool->handle,
            &timeout, sizeof(timeout),
            pool->err
        )
    }

#endif

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibPoolGetNoWait
 * --------------------------------------------------------------------------------------------- */

boolean OcilibPoolGetNoWait
(
    OCI_Pool *pool
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_POOL, pool
    )

    ub1 value = 0;

    CHECK_PTR(OCI_IPC_POOL, pool)

#if OCI_VERSION_COMPILE >= OCI_9_0

    if (Env.version_runtime >= OCI_9_0)
    {
        ub4 attr = 0;

        if (OCI_HTYPE_CPOOL == pool->htype)
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
            pool->htype, attr,
            pool->handle, &value, NULL,
            pool->err
        )
    }

#endif

    SET_RETVAL((boolean)value)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibPoolSetNoWait
 * --------------------------------------------------------------------------------------------- */

boolean OcilibPoolSetNoWait
(
    OCI_Pool *pool,
    boolean   value
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_POOL, pool
    )

    CHECK_PTR(OCI_IPC_POOL, pool)

#if OCI_VERSION_COMPILE >= OCI_9_0

    if (Env.version_runtime >= OCI_9_0)
    {
        ub1 nowait = (ub1) value;
        ub4 attr   = 0;

        if (OCI_HTYPE_CPOOL == pool->htype)
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
            pool->htype, attr,
            pool->handle, &nowait, sizeof(nowait),
            pool->err
        )
    }

#endif

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibPoolGetBusyCount
 * --------------------------------------------------------------------------------------------- */

unsigned int OcilibPoolGetBusyCount
(
    OCI_Pool *pool
)
{
    ENTER_FUNC
    (
        /* returns */ unsigned int, 0,
        /* context */ OCI_IPC_POOL, pool
    )

    ub4 value = 0;

    CHECK_PTR(OCI_IPC_POOL, pool)

#if OCI_VERSION_COMPILE >= OCI_9_0

    if (Env.version_runtime >= OCI_9_0)
    {
        ub4 attr = 0;

        if (OCI_HTYPE_CPOOL == pool->htype)
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
            pool->htype, attr,
            pool->handle, &value, NULL,
            pool->err
        )
    }

#endif

    SET_RETVAL(value)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibPoolGetOpenedCount
 * --------------------------------------------------------------------------------------------- */

unsigned int OcilibPoolGetOpenedCount
(
    OCI_Pool *pool
)
{
    ENTER_FUNC
    (
        /* returns */ unsigned int, 0,
        /* context */ OCI_IPC_POOL, pool
    )

    ub4 value = 0;

    CHECK_PTR(OCI_IPC_POOL, pool)

#if OCI_VERSION_COMPILE >= OCI_9_0

    if (Env.version_runtime >= OCI_9_0)
    {
        ub4 attr = 0;

        if (OCI_HTYPE_CPOOL == pool->htype)
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
            pool->htype, attr,
            pool->handle, &value, NULL,
            pool->err
        )
    }

#endif

    SET_RETVAL(value)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibPoolGetMin
 * --------------------------------------------------------------------------------------------- */

unsigned int OcilibPoolGetMin
(
    OCI_Pool *pool
)
{
    GET_PROP
    (
        unsigned int, 0,
        OCI_IPC_POOL, pool,
        min
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibPoolGetMax
 * --------------------------------------------------------------------------------------------- */

unsigned int OcilibPoolGetMax
(
    OCI_Pool *pool
)
{
    GET_PROP
    (
        unsigned int, 0,
        OCI_IPC_POOL, pool,
        max
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibPoolGetIncrement
 * --------------------------------------------------------------------------------------------- */

unsigned int OcilibPoolGetIncrement
(
    OCI_Pool *pool
)
{
    GET_PROP
    (
        unsigned int, 0,
        OCI_IPC_POOL, pool,
        incr
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibPoolSetStatementCacheSize
 * --------------------------------------------------------------------------------------------- */

boolean OcilibPoolSetStatementCacheSize
(
    OCI_Pool    *pool,
    unsigned int value
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_POOL, pool
    )

    ub4 cache_size = value;

    CHECK_PTR(OCI_IPC_POOL, pool)

#if OCI_VERSION_COMPILE >= OCI_10_1

    if (Env.version_runtime >= OCI_10_1)
    {
        if (OCI_HTYPE_SPOOL == pool->htype)
        {
            CHECK_ATTRIB_SET
            (
                pool->htype, OCI_ATTR_SPOOL_STMTCACHESIZE,
                pool->handle, &cache_size, sizeof(cache_size),
                pool->err
            )
        }
    }

#endif

    pool->cache_size = cache_size;

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibPoolGetStatementCacheSize
 * --------------------------------------------------------------------------------------------- */

unsigned int OcilibPoolGetStatementCacheSize
(
    OCI_Pool *pool
)
{
    ENTER_FUNC
    (
        /* returns */ unsigned int, 0,
        /* context */ OCI_IPC_POOL, pool
    )
    ub4 cache_size = 0;

    CHECK_PTR(OCI_IPC_POOL, pool)

#if OCI_VERSION_COMPILE >= OCI_10_1

    if (Env.version_runtime >= OCI_10_1)
    {
        if (OCI_HTYPE_SPOOL == pool->htype)
        {
            CHECK_ATTRIB_GET
            (
                pool->htype, OCI_ATTR_SPOOL_STMTCACHESIZE,
                pool->handle, &cache_size, NULL,
                pool->err
            )
        }
        else
        {
            cache_size = pool->cache_size;
        }

        pool->cache_size = cache_size;
    }

#else

    OCI_NOT_USED(cache_size)

#endif

    SET_RETVAL(pool->cache_size)

    EXIT_FUNC()
}
