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
#include "strings.h"

static unsigned int PoolTypeValues[] = { OCI_POOL_CONNECTION, OCI_POOL_SESSION };

/* --------------------------------------------------------------------------------------------- *
 * PoolDispose
 * --------------------------------------------------------------------------------------------- */

boolean PoolDispose
(
    OCI_Pool *pool
)
{
    DECLARE_CTX(TRUE)

    CHECK(NULL == pool, FALSE)

    CTX_SET_FROM_ERR(pool->err);

 #if OCI_VERSION_COMPILE >= OCI_9_0

    if (OCILib.version_runtime >= OCI_9_0)
    {
        /* close pool handle */

        if (pool->handle)
        {
            if (OCI_HTYPE_CPOOL == pool->htype)
            {
               EXEC(OCIConnectionPoolDestroy((OCICPool *) pool->handle, pool->err, (ub4) OCI_DEFAULT))
            }

        #if OCI_VERSION_COMPILE >= OCI_9_2

            else
            {
                EXEC(OCISessionPoolDestroy((OCISPool *) pool->handle, pool->err, (ub4) OCI_SPD_FORCE))
            }

        #endif

            MemoryFreeHandle((void *) pool->handle, (ub4) pool->htype);
        }

    #if OCI_VERSION_COMPILE >= OCI_11_2

        /* close authentication handle */

        if (pool->authp)
        {
            MemoryFreeHandle((void *) pool->authp, OCI_HTYPE_AUTHINFO);
        }

    #endif

        /* close error handle */

        if (pool->err)
        {
            MemoryFreeHandle((void *) pool->err, OCI_HTYPE_ERROR);
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

    return STATUS;
}

/* --------------------------------------------------------------------------------------------- *
 * PoolCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Pool * PoolCreate
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
    OCI_Pool *pool = NULL;

    CALL_ENTER(OCI_Pool*, NULL)
    CHECK_INITIALIZED()
    CHECK_ENUM_VALUE(NULL, NULL, type, PoolTypeValues, OTEXT("Pool Type"))
    CHECK_MIN(NULL, NULL, max_con, 1)

    STATUS = FALSE;

    /* make sure that we do not have a XA session flag */
    
    mode &= ~OCI_SESSION_XA;
    
    /* create pool object */

    pool = ListAppend(OCILib.pools, sizeof(*pool));
    STATUS = (NULL != pool);

    if (STATUS)
    {
        pool->mode = mode;
        pool->min  = min_con;
        pool->max  = max_con;
        pool->incr = incr_con;

        pool->db   = ostrdup(db   ? db   : OTEXT(""));
        pool->user = ostrdup(user ? user : OTEXT(""));
        pool->pwd  = ostrdup(pwd  ? pwd  : OTEXT(""));
    }

#if OCI_VERSION_COMPILE < OCI_9_2

    type = OCI_POOL_CONNECTION;

#endif

#if OCI_VERSION_COMPILE >= OCI_9_0

    if (STATUS)
    {
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

    }

    if (OCILib.version_runtime >= OCI_9_0)
    {
        int dbsize_name = -1;
        int dbsize_db   = -1;

        dbtext *dbstr_name = NULL;
        dbtext *dbstr_db   = NULL;

        /* allocate error handle */

        STATUS = STATUS && MemoryAllocHandle((dvoid *)OCILib.env, (dvoid **)(void *)&pool->err, OCI_HTYPE_ERROR);

        /* allocate pool handle */

        STATUS = STATUS && MemoryAllocHandle((dvoid *)OCILib.env, (dvoid **)(void *)&pool->handle, (ub4)pool->htype);

        /* allocate authentication handle only if needed */

   #if OCI_VERSION_COMPILE >= OCI_11_1

        if (STATUS)
        {       
            if ((OCI_HTYPE_SPOOL == pool->htype) && (OCILib.version_runtime >= OCI_11_1))
            {
                int     dbsize = -1;
                dbtext *dbstr  = NULL;

                otext driver_version[OCI_SIZE_FORMAT];

                osprintf(driver_version,
                         osizeof(driver_version) - (size_t)1,
                         OTEXT("%s : %d.%d.%d"),
                         OCILIB_DRIVER_NAME,
                         OCILIB_MAJOR_VERSION,
                         OCILIB_MINOR_VERSION,
                         OCILIB_REVISION_VERSION);

                dbstr = StringGetDBString(driver_version, &dbsize);

                /* allocate authentication handle */

                STATUS = MemoryAllocHandle((dvoid *)OCILib.env, (dvoid **)(void *)&pool->authp, OCI_HTYPE_AUTHINFO);

                /* set OCILIB driver layer name attribute only for session pools here
                    For standalone connections and connection pool this attribute is set
                    in OCI_ConnectionLogon() */

                ATTRIB_SET(OCI_HTYPE_AUTHINFO, OCI_ATTR_DRIVER_NAME, pool->authp, dbstr, dbsize)
                
                StringReleaseDBString(dbstr);

                /* set authentication handle on the session pool */

                ATTRIB_SET(OCI_HTYPE_SPOOL, OCI_ATTR_SPOOL_AUTH, pool->handle, pool->authp, sizeof(pool->authp))
            }
        }

    #endif

        /* create the pool */

        if (STATUS)
        {
            dbtext *dbstr_user  = NULL;
            dbtext *dbstr_pwd   = NULL;
            int     dbsize_user = -1;
            int     dbsize_pwd  = -1;
      
            dbstr_db   = StringGetDBString(pool->db,   &dbsize_db);
            dbstr_user = StringGetDBString(pool->user, &dbsize_user);
            dbstr_pwd  = StringGetDBString(pool->pwd,  &dbsize_pwd);

            if (OCI_HTYPE_CPOOL == pool->htype)
            {
                EXEC
                (
                    OCIConnectionPoolCreate(OCILib.env, pool->err, (OCICPool *) pool->handle,
                                            (OraText **) (dvoid *) &dbstr_name,
                                            (sb4*) &dbsize_name,
                                            (OraText *) dbstr_db, (sb4) dbsize_db,
                                            (ub4) pool->min, (ub4) pool->max,
                                            (ub4) pool->incr, (OraText *) dbstr_user,
                                            (sb4) dbsize_user, (OraText *) dbstr_pwd,
                                            (sb4) dbsize_pwd,  (ub4) OCI_DEFAULT)
                )
            }

        #if OCI_VERSION_COMPILE >= OCI_9_2

            else
            {
                ub4 sess_mode = OCI_DEFAULT;
                
                if (!(pool->mode & OCI_SESSION_SYSDBA) && IS_STRING_VALID(pool->user) && IS_STRING_VALID(pool->pwd))
                {
                    sess_mode |= OCI_SPC_HOMOGENEOUS;
                }

                EXEC
                (
                    OCISessionPoolCreate(OCILib.env, pool->err, (OCISPool *) pool->handle,
                                         (OraText **) (dvoid *) &dbstr_name,
                                         (ub4*) &dbsize_name,
                                         (OraText *) dbstr_db, (sb4) dbsize_db,
                                         (ub4) pool->min, (ub4) pool->max,
                                         (ub4) pool->incr, (OraText *) dbstr_user,
                                         (sb4) dbsize_user, (OraText *) dbstr_pwd,
                                         (sb4) dbsize_pwd,  (ub4) sess_mode)
                )
            }

        #endif

            StringReleaseDBString(dbstr_db);
            StringReleaseDBString(dbstr_user);
            StringReleaseDBString(dbstr_pwd);
        }

        if (STATUS && dbstr_name)
        {
            pool->name = StringDuplicateFromDBString(dbstr_name, dbcharcount(dbsize_name));

            STATUS = (NULL != pool->name);
        }
    }

#endif

    /* on success, we allocate internal OCI connection objects for pool
       minimum size */

    if (STATUS)
    {     

    #if OCI_VERSION_COMPILE >= OCI_9_0

        /* retrieve statement cache size */

        PoolGetStatementCacheSize(pool);

        /* for connection pools that do not handle the statement cache
           attribute, let's set the value with documented default cache size */

        if (pool->cache_size == 0)
        {
            PoolSetStatementCacheSize(pool, OCI_DEFAUT_STMT_CACHE_SIZE);
        }

    #endif

    }

    if (STATUS)
    {
        RETVAL = pool;
    }
    else if (pool)
    {
        PoolFree(pool);
    }

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * PoolFree
 * --------------------------------------------------------------------------------------------- */

boolean PoolFree
(
    OCI_Pool *pool
)
{
    CALL_ENTER(boolean, FALSE)
    CHECK_PTR(OCI_IPC_POOL, pool)
    CTX_SET_FROM_ERR(pool->err)

    STATUS = PoolDispose(pool);

    ListRemove(OCILib.pools, pool);

    FREE(pool)

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * PoolGetConnection
 * --------------------------------------------------------------------------------------------- */

OCI_Connection * PoolGetConnection
(
    OCI_Pool    *pool,
    const otext *tag
)
{
    CALL_ENTER(OCI_Connection*, NULL)
    CHECK_PTR(OCI_IPC_POOL, pool)
    CTX_SET_FROM_ERR(pool->err)

    RETVAL = ConnectionCreateInternal(pool, pool->db, pool->user, pool->pwd, pool->mode, tag);

    /* for regular connection pool, set the statement cache size to 
       retrieved connection */

 #if OCI_VERSION_COMPILE >= OCI_10_1

    if (RETVAL)
    {
        const unsigned int cache_size = PoolGetStatementCacheSize(pool);

        ConnectionSetStatementCacheSize(RETVAL, cache_size);
    }

#endif

    STATUS = (NULL != RETVAL);
    
    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * PoolGetTimeout
 * --------------------------------------------------------------------------------------------- */

unsigned int PoolGetTimeout
(
    OCI_Pool *pool
)
{
    ub4 value = 0;

    CALL_ENTER(unsigned int, value)
    CHECK_PTR(OCI_IPC_POOL, pool)
    CTX_SET_FROM_ERR(pool->err)

#if OCI_VERSION_COMPILE >= OCI_9_0

    if (OCILib.version_runtime >= OCI_9_0)
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

        ATTRIB_GET(pool->htype, attr, pool->handle, &value, NULL)
    }

#endif

    RETVAL = (unsigned int)value;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * PoolSetTimeout
 * --------------------------------------------------------------------------------------------- */

boolean PoolSetTimeout
(
    OCI_Pool    *pool,
    unsigned int value
)
{
    CALL_ENTER(boolean, FALSE)
    CHECK_PTR(OCI_IPC_POOL, pool)
    CTX_SET_FROM_ERR(pool->err)

#if OCI_VERSION_COMPILE >= OCI_9_0

    if (OCILib.version_runtime >= OCI_9_0)
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

        ATTRIB_SET(pool->htype, attr, pool->handle, &timeout, sizeof(timeout))
    }

#endif

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * PoolGetNoWait
 * --------------------------------------------------------------------------------------------- */

boolean PoolGetNoWait
(
    OCI_Pool *pool
)
{
    ub1 value = 0;

    CALL_ENTER(boolean, FALSE)
    CHECK_PTR(OCI_IPC_POOL, pool)
    CTX_SET_FROM_ERR(pool->err)

#if OCI_VERSION_COMPILE >= OCI_9_0

    if (OCILib.version_runtime >= OCI_9_0)
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

        ATTRIB_GET(pool->htype, attr, pool->handle, &value, NULL)
    }

#endif

    RETVAL = (boolean)value;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * PoolSetNoWait
 * --------------------------------------------------------------------------------------------- */

boolean PoolSetNoWait
(
    OCI_Pool *pool,
    boolean   value
)
{
    CALL_ENTER(boolean, FALSE)
    CHECK_PTR(OCI_IPC_POOL, pool)
    CTX_SET_FROM_ERR(pool->err)

 #if OCI_VERSION_COMPILE >= OCI_9_0

    if (OCILib.version_runtime >= OCI_9_0)
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

        ATTRIB_SET(pool->htype, attr, pool->handle, &nowait, sizeof(nowait))
    }

#endif

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * PoolGetBusyCount
 * --------------------------------------------------------------------------------------------- */

unsigned int PoolGetBusyCount
(
    OCI_Pool *pool
)
{
    ub4 value = 0;

    CALL_ENTER(unsigned int, value)
    CHECK_PTR(OCI_IPC_POOL, pool)
    CTX_SET_FROM_ERR(pool->err)

#if OCI_VERSION_COMPILE >= OCI_9_0

    if (OCILib.version_runtime >= OCI_9_0)
    {
        ub4 attr  = 0;

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

        ATTRIB_GET(pool->htype, attr, pool->handle, &value, NULL)
    }

#endif

    RETVAL = value;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * PoolGetOpenedCount
 * --------------------------------------------------------------------------------------------- */

unsigned int PoolGetOpenedCount
(
    OCI_Pool *pool
)
{
    ub4 value = 0;

    CALL_ENTER(unsigned int, value)
    CHECK_PTR(OCI_IPC_POOL, pool)
    CTX_SET_FROM_ERR(pool->err)

#if OCI_VERSION_COMPILE >= OCI_9_0

    if (OCILib.version_runtime >= OCI_9_0)
    {
        ub4 attr  = 0;

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

        ATTRIB_GET(pool->htype, attr, pool->handle, &value, NULL)
    }

#endif

    RETVAL = value;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * PoolGetMin
 * --------------------------------------------------------------------------------------------- */

unsigned int PoolGetMin
(
    OCI_Pool *pool
)
{
    GET_PROP(unsigned int, 0, OCI_IPC_POOL, pool, min, NULL, NULL, pool->err)
}

/* --------------------------------------------------------------------------------------------- *
 * PoolGetMax
 * --------------------------------------------------------------------------------------------- */

unsigned int PoolGetMax
(
    OCI_Pool *pool
)
{
    GET_PROP(unsigned int, 0, OCI_IPC_POOL, pool, max, NULL, NULL, pool->err)
}

/* --------------------------------------------------------------------------------------------- *
 * PoolGetIncrement
 * --------------------------------------------------------------------------------------------- */

unsigned int PoolGetIncrement
(
    OCI_Pool *pool
)
{
    GET_PROP(unsigned int, 0, OCI_IPC_POOL, pool, incr, NULL, NULL, pool->err)
}

/* --------------------------------------------------------------------------------------------- *
 * PoolSetStatementCacheSize
 * --------------------------------------------------------------------------------------------- */

boolean PoolSetStatementCacheSize
(
    OCI_Pool     *pool,
    unsigned int  value
)
{
    ub4 cache_size = value;

    CALL_ENTER(boolean, FALSE)
    CHECK_PTR(OCI_IPC_POOL, pool)
    CTX_SET_FROM_ERR(pool->err)

 #if OCI_VERSION_COMPILE >= OCI_10_1

    if (OCILib.version_runtime >= OCI_10_1)
    {
        if (OCI_HTYPE_SPOOL == pool->htype)
        {
            ATTRIB_SET(pool->htype, OCI_ATTR_SPOOL_STMTCACHESIZE, pool->handle, &cache_size, sizeof(cache_size))
        }
    }

#endif

    if (STATUS)
    {
        pool->cache_size = cache_size;
    }

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * PoolGetStatementCacheSize
 * --------------------------------------------------------------------------------------------- */

unsigned int PoolGetStatementCacheSize
(
    OCI_Pool *pool
)
{
    ub4 cache_size = 0;

    CALL_ENTER(unsigned int, cache_size)
    CHECK_PTR(OCI_IPC_POOL, pool)
    CTX_SET_FROM_ERR(pool->err)

 #if OCI_VERSION_COMPILE >= OCI_10_1

    if (OCILib.version_runtime >= OCI_10_1)
    {
        if (OCI_HTYPE_SPOOL == pool->htype)
        {
            ATTRIB_GET(pool->htype, OCI_ATTR_SPOOL_STMTCACHESIZE, pool->handle, &cache_size, NULL)
        }
        else
        {
            cache_size = pool->cache_size;
        }
        
        if (STATUS)
        {
            pool->cache_size = cache_size;
        }
    }

#else

    OCI_NOT_USED(cache_size)

#endif

    if (STATUS)
    {
        RETVAL = pool->cache_size;
   } 

    CALL_EXIT()
}

