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
#include "macro.h"
#include "strings.h"

static unsigned int PoolTypeValues[] = { OCI_POOL_CONNECTION, OCI_POOL_SESSION };

/* --------------------------------------------------------------------------------------------- *
 * PoolClose
 * --------------------------------------------------------------------------------------------- */

boolean PoolClose
(
    OCI_Pool *pool
)
{
    OCI_CALL_DECLARE_CONTEXT(TRUE)

    OCI_CHECK(NULL == pool, FALSE)

    OCI_CALL_CONTEXT_SET_FROM_ERR(pool->err);

 #if OCI_VERSION_COMPILE >= OCI_9_0

    if (OCILib.version_runtime >= OCI_9_0)
    {
        /* close pool handle */

        if (pool->handle)
        {
            if (OCI_HTYPE_CPOOL == pool->htype)
            {
               OCI_EXEC(OCIConnectionPoolDestroy((OCICPool *) pool->handle, pool->err, (ub4) OCI_DEFAULT))
            }

        #if OCI_VERSION_COMPILE >= OCI_9_2

            else
            {
                OCI_EXEC(OCISessionPoolDestroy((OCISPool *) pool->handle, pool->err, (ub4) OCI_SPD_FORCE))
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

    OCI_FREE(pool->name)
    OCI_FREE(pool->db)
    OCI_FREE(pool->user)
    OCI_FREE(pool->pwd)

    return OCI_STATUS;
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

    OCI_CALL_ENTER(OCI_Pool*, NULL)
    OCI_CALL_CHECK_INITIALIZED()
    OCI_CALL_CHECK_ENUM_VALUE(NULL, NULL, type, PoolTypeValues, OTEXT("Pool Type"))
    OCI_CALL_CHECK_MIN(NULL, NULL, max_con, 1)

    OCI_STATUS = FALSE;

    /* make sure that we do not have a XA session flag */
    
    mode &= ~OCI_SESSION_XA;
    
    /* create pool object */

    pool = ListAppend(OCILib.pools, sizeof(*pool));
    OCI_STATUS = (NULL != pool);

    if (OCI_STATUS)
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

    if (OCI_STATUS)
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

        OCI_STATUS = OCI_STATUS && MemoryAllocHandle((dvoid *)OCILib.env, (dvoid **)(void *)&pool->err, OCI_HTYPE_ERROR);

        /* allocate pool handle */

        OCI_STATUS = OCI_STATUS && MemoryAllocHandle((dvoid *)OCILib.env, (dvoid **)(void *)&pool->handle, (ub4)pool->htype);

        /* allocate authentication handle only if needed */

   #if OCI_VERSION_COMPILE >= OCI_11_1

        if (OCI_STATUS)
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

                dbstr = StringGetOracleString(driver_version, &dbsize);

                /* allocate authentication handle */

                OCI_STATUS = MemoryAllocHandle((dvoid *)OCILib.env, (dvoid **)(void *)&pool->authp, OCI_HTYPE_AUTHINFO);

                /* set OCILIB driver layer name attribute only for session pools here
                    For standalone connections and connection pool this attribute is set
                    in OCI_ConnectionLogon() */

                OCI_SET_ATTRIB(OCI_HTYPE_AUTHINFO, OCI_ATTR_DRIVER_NAME, pool->authp, dbstr, dbsize)
                
                StringReleaseOracleString(dbstr);

                /* set authentication handle on the session pool */

                OCI_SET_ATTRIB(OCI_HTYPE_SPOOL, OCI_ATTR_SPOOL_AUTH, pool->handle, pool->authp, sizeof(pool->authp))
            }
        }

    #endif

        /* create the pool */

        if (OCI_STATUS)
        {
            dbtext *dbstr_user  = NULL;
            dbtext *dbstr_pwd   = NULL;
            int     dbsize_user = -1;
            int     dbsize_pwd  = -1;
      
            dbstr_db   = StringGetOracleString(pool->db,   &dbsize_db);
            dbstr_user = StringGetOracleString(pool->user, &dbsize_user);
            dbstr_pwd  = StringGetOracleString(pool->pwd,  &dbsize_pwd);

            if (OCI_HTYPE_CPOOL == pool->htype)
            {
                OCI_EXEC
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
                
                if (!(pool->mode & OCI_SESSION_SYSDBA) && OCI_STRING_VALID(pool->user) && OCI_STRING_VALID(pool->pwd))
                {
                    sess_mode |= OCI_SPC_HOMOGENEOUS;
                }

                OCI_EXEC
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

            StringReleaseOracleString(dbstr_db);
            StringReleaseOracleString(dbstr_user);
            StringReleaseOracleString(dbstr_pwd);
        }

        if (OCI_STATUS && dbstr_name)
        {
            pool->name = StringDuplicateFromOracleString(dbstr_name, dbcharcount(dbsize_name));

            OCI_STATUS = (NULL != pool->name);
        }
    }

#endif

    /* on success, we allocate internal OCI connection objects for pool
       minimum size */

    if (OCI_STATUS)
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

    if (OCI_STATUS)
    {
        OCI_RETVAL = pool;
    }
    else if (pool)
    {
        PoolFree(pool);
    }

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * PoolFree
 * --------------------------------------------------------------------------------------------- */

boolean PoolFree
(
    OCI_Pool *pool
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_POOL, pool)
    OCI_CALL_CONTEXT_SET_FROM_ERR(pool->err)

    OCI_STATUS = PoolClose(pool);

    ListRemove(OCILib.pools, pool);

    OCI_FREE(pool)

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
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
    OCI_CALL_ENTER(OCI_Connection*, NULL)
    OCI_CALL_CHECK_PTR(OCI_IPC_POOL, pool)
    OCI_CALL_CONTEXT_SET_FROM_ERR(pool->err)

    OCI_RETVAL = ConnectionCreateInternal(pool, pool->db, pool->user, pool->pwd, pool->mode, tag);

    /* for regular connection pool, set the statement cache size to 
       retrieved connection */

 #if OCI_VERSION_COMPILE >= OCI_10_1

    if (OCI_RETVAL)
    {
        const unsigned int cache_size = PoolGetStatementCacheSize(pool);

        ConnectionSetStatementCacheSize(OCI_RETVAL, cache_size);
    }

#endif

    OCI_STATUS = (NULL != OCI_RETVAL);
    
    OCI_CALL_EXIT()
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

    OCI_CALL_ENTER(unsigned int, value)
    OCI_CALL_CHECK_PTR(OCI_IPC_POOL, pool)
    OCI_CALL_CONTEXT_SET_FROM_ERR(pool->err)

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

        OCI_GET_ATTRIB(pool->htype, attr, pool->handle, &value, NULL)
    }

#endif

    OCI_RETVAL = (unsigned int)value;

    OCI_CALL_EXIT()
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
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_POOL, pool)
    OCI_CALL_CONTEXT_SET_FROM_ERR(pool->err)

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

        OCI_SET_ATTRIB(pool->htype, attr, pool->handle, &timeout, sizeof(timeout))
    }

#endif

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
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

    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_POOL, pool)
    OCI_CALL_CONTEXT_SET_FROM_ERR(pool->err)

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

        OCI_GET_ATTRIB(pool->htype, attr, pool->handle, &value, NULL)
    }

#endif

    OCI_RETVAL = (boolean)value;

    OCI_CALL_EXIT()
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
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_POOL, pool)
    OCI_CALL_CONTEXT_SET_FROM_ERR(pool->err)

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

        OCI_SET_ATTRIB(pool->htype, attr, pool->handle, &nowait, sizeof(nowait))
    }

#endif

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
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

    OCI_CALL_ENTER(unsigned int, value)
    OCI_CALL_CHECK_PTR(OCI_IPC_POOL, pool)
    OCI_CALL_CONTEXT_SET_FROM_ERR(pool->err)

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

        OCI_GET_ATTRIB(pool->htype, attr, pool->handle, &value, NULL)
    }

#endif

    OCI_RETVAL = value;

    OCI_CALL_EXIT()
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

    OCI_CALL_ENTER(unsigned int, value)
    OCI_CALL_CHECK_PTR(OCI_IPC_POOL, pool)
    OCI_CALL_CONTEXT_SET_FROM_ERR(pool->err)

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

        OCI_GET_ATTRIB(pool->htype, attr, pool->handle, &value, NULL)
    }

#endif

    OCI_RETVAL = value;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * PoolGetMin
 * --------------------------------------------------------------------------------------------- */

unsigned int PoolGetMin
(
    OCI_Pool *pool
)
{
    OCI_GET_PROP(unsigned int, 0, OCI_IPC_POOL, pool, min, NULL, NULL, pool->err)
}

/* --------------------------------------------------------------------------------------------- *
 * PoolGetMax
 * --------------------------------------------------------------------------------------------- */

unsigned int PoolGetMax
(
    OCI_Pool *pool
)
{
    OCI_GET_PROP(unsigned int, 0, OCI_IPC_POOL, pool, max, NULL, NULL, pool->err)
}

/* --------------------------------------------------------------------------------------------- *
 * PoolGetIncrement
 * --------------------------------------------------------------------------------------------- */

unsigned int PoolGetIncrement
(
    OCI_Pool *pool
)
{
    OCI_GET_PROP(unsigned int, 0, OCI_IPC_POOL, pool, incr, NULL, NULL, pool->err)
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

    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_POOL, pool)
    OCI_CALL_CONTEXT_SET_FROM_ERR(pool->err)

 #if OCI_VERSION_COMPILE >= OCI_10_1

    if (OCILib.version_runtime >= OCI_10_1)
    {
        if (OCI_HTYPE_SPOOL == pool->htype)
        {
            OCI_SET_ATTRIB(pool->htype, OCI_ATTR_SPOOL_STMTCACHESIZE, pool->handle, &cache_size, sizeof(cache_size))
        }
    }

#endif

    if (OCI_STATUS)
    {
        pool->cache_size = cache_size;
    }

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
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

    OCI_CALL_ENTER(unsigned int, cache_size)
    OCI_CALL_CHECK_PTR(OCI_IPC_POOL, pool)
    OCI_CALL_CONTEXT_SET_FROM_ERR(pool->err)

 #if OCI_VERSION_COMPILE >= OCI_10_1

    if (OCILib.version_runtime >= OCI_10_1)
    {
        if (OCI_HTYPE_SPOOL == pool->htype)
        {
            OCI_GET_ATTRIB(pool->htype, OCI_ATTR_SPOOL_STMTCACHESIZE, pool->handle, &cache_size, NULL)
        }
        else
        {
            cache_size = pool->cache_size;
        }
        
        if (OCI_STATUS)
        {
            pool->cache_size = cache_size;
        }
    }

#else

    OCI_NOT_USED(cache_size)

#endif

    if (OCI_STATUS)
    {
        OCI_RETVAL = pool->cache_size;
   } 

    OCI_CALL_EXIT()
}

