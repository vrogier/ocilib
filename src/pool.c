/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2016 Vincent ROGIER <vince.rogier@ocilib.net>
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
 *                             PRIVATE VARIABLES
 * ********************************************************************************************* */

static unsigned int PoolTypeValues[] = { OCI_POOL_CONNECTION, OCI_POOL_SESSION };

/* ********************************************************************************************* *
 *                             PRIVATE FUNCTIONS
 * ********************************************************************************************* */

/* --------------------------------------------------------------------------------------------- *
 * OCI_PoolClose
 * --------------------------------------------------------------------------------------------- */

boolean OCI_PoolClose
(
    OCI_Pool *pool
)
{
    boolean res = TRUE;

    OCI_CHECK(NULL == pool, FALSE)

 #if OCI_VERSION_COMPILE >= OCI_9_0

    if (OCILib.version_runtime >= OCI_9_0)
    {
        /* close pool handle */

        if (pool->handle)
        {
            if (OCI_HTYPE_CPOOL == pool->htype)
            {
                OCI_CALL0
                (
                    res, pool->err,

                    OCIConnectionPoolDestroy((OCICPool *) pool->handle, pool->err, (ub4) OCI_DEFAULT)
                )
            }

        #if OCI_VERSION_COMPILE >= OCI_9_2

            else
            {
                OCI_CALL0
                (
                    res, pool->err,

                    OCISessionPoolDestroy((OCISPool *) pool->handle, pool->err, (ub4) OCI_SPD_FORCE)
                )
            }

        #endif

            OCI_HandleFree((void *) pool->handle, (ub4) pool->htype);
        }

    #if OCI_VERSION_COMPILE >= OCI_11_2

        /* close authentication handle */

        if (pool->authp)
        {
            OCI_HandleFree((void *) pool->authp, (ub4) OCI_HTYPE_AUTHINFO);
        }

    #endif

        /* close error handle */

        if (pool->err)
        {
            OCI_HandleFree((void *) pool->err, (ub4) OCI_HTYPE_ERROR);
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

    return res;
}

/* ********************************************************************************************* *
 *                             PUBLIC FUNCTIONS
 * ********************************************************************************************* */

/* --------------------------------------------------------------------------------------------- *
 * OCI_PoolCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Pool * OCI_API OCI_PoolCreate
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
    OCI_Item *item = NULL;

    OCI_LIB_CALL_ENTER(OCI_Pool*, NULL)

    OCI_CHECK_INITIALIZED()
    OCI_CHECK_ENUM_VALUE(NULL, NULL, type, PoolTypeValues, OTEXT("Pool Type"))
    OCI_CHECK_MIN(NULL, NULL, max_con, 1)

    /* make sure that we do not have a XA session flag */
    
    mode &= ~OCI_SESSION_XA;
    
    /* create pool object */

    item = OCI_ListAppend(OCILib.pools, sizeof(*pool));

    if (item)
    {
        pool = (OCI_Pool *) item->data;
        pool->mode = mode;
        pool->min  = min_con;
        pool->max  = max_con;
        pool->incr = incr_con;

        pool->db   = ostrdup(db   ? db   : OTEXT(""));
        pool->user = ostrdup(user ? user : OTEXT(""));
        pool->pwd  = ostrdup(pwd  ? pwd  : OTEXT(""));

        call_status = TRUE;
    }

#if OCI_VERSION_COMPILE < OCI_9_2

    type = OCI_POOL_CONNECTION;

#endif

#if OCI_VERSION_COMPILE >= OCI_9_0

    if (call_status)
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

        if (call_status)
        {
            call_status = OCI_SUCCESSFUL(OCI_HandleAlloc((dvoid *)OCILib.env,
                                                         (dvoid **) (void *) &pool->err,
                                                         (ub4) OCI_HTYPE_ERROR,
                                                         (size_t) 0,
                                                         (dvoid **) NULL));
        }

        /* allocate pool handle */

        if (call_status)
        {
            call_status = OCI_SUCCESSFUL(OCI_HandleAlloc((dvoid *)OCILib.env,
                                                         (dvoid **) (void *) &pool->handle,
                                                         (ub4) pool->htype,
                                                         (size_t) 0,
                                                         (dvoid **) NULL));
        }

        /* allocate authentication handle only if needed */

   #if OCI_VERSION_COMPILE >= OCI_11_2

        if (call_status)
        {       
            if ((OCI_HTYPE_SPOOL == pool->htype) && (OCILib.version_runtime >= OCI_11_2))
            {
                int     dbsize = -1;
                dbtext *dbstr  = OCI_StringGetOracleString(OCILIB_DRIVER_NAME, &dbsize);
                    
                /* allocate authentication handle */

                call_status = OCI_SUCCESSFUL(OCI_HandleAlloc((dvoid *)OCILib.env,
                                                             (dvoid **) (void *) &pool->authp,
                                                             (ub4) OCI_HTYPE_AUTHINFO,
                                                             (size_t) 0,
                                                             (dvoid **) NULL));


                /* set OCILIB's driver layer name attribute only for session pools here
                    For standalone connections and connection pool this attribute is set
                    in OCI_ConnectionLogon() */

                OCI_CALL3
                (
                    call_status, pool->err,

                    OCIAttrSet((dvoid *) pool->authp, (ub4) OCI_HTYPE_AUTHINFO,
                                (dvoid *) dbstr, (ub4) dbsize,
                                (ub4) OCI_ATTR_DRIVER_NAME, pool->err)
                )
                
                OCI_StringReleaseOracleString(dbstr);

                /* set authentication handle on the session pool */

                OCI_CALL3
                (
                    call_status, pool->err,

                    OCIAttrSet((dvoid *) pool->handle, (ub4) OCI_HTYPE_SPOOL,
                                (dvoid *) pool->authp, (ub4) sizeof(pool->authp),
                                (ub4) OCI_ATTR_SPOOL_AUTH, pool->err)
                ) 
            }
        }

    #endif

        /* create the pool */

        if (call_status)
        {
            dbtext *dbstr_user  = NULL;
            dbtext *dbstr_pwd   = NULL;
            int     dbsize_user = -1;
            int     dbsize_pwd  = -1;
      
            dbstr_db   = OCI_StringGetOracleString(pool->db,   &dbsize_db);
            dbstr_user = OCI_StringGetOracleString(pool->user, &dbsize_user);
            dbstr_pwd  = OCI_StringGetOracleString(pool->pwd,  &dbsize_pwd);

            if (OCI_HTYPE_CPOOL == pool->htype)
            {
                OCI_CALL3
                (
                    call_status, pool->err,

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
                OCI_CALL3
                (
                    call_status, pool->err,

                    OCISessionPoolCreate(OCILib.env, pool->err, (OCISPool *) pool->handle,
                                         (OraText **) (dvoid *) &dbstr_name,
                                         (ub4*) &dbsize_name,
                                         (OraText *) dbstr_db, (sb4) dbsize_db,
                                         (ub4) pool->min, (ub4) pool->max,
                                         (ub4) pool->incr, (OraText *) dbstr_user,
                                         (sb4) dbsize_user, (OraText *) dbstr_pwd,
                                         (sb4) dbsize_pwd,  (ub4) OCI_SPC_HOMOGENEOUS)
                )
            }

        #endif

            OCI_StringReleaseOracleString(dbstr_db);
            OCI_StringReleaseOracleString(dbstr_user);
            OCI_StringReleaseOracleString(dbstr_pwd);
        }

        if (call_status && dbstr_name)
        {
            pool->name = OCI_StringDuplicateFromOracleString(dbstr_name, dbcharcount(dbsize_name));

            call_status = (NULL != pool->name);
        }
    }

#endif

    /* on success, we allocate internal OCI connection objects for pool
       minimum size */

    if (call_status)
    {     

    #if OCI_VERSION_COMPILE >= OCI_9_0

        /* retrieve statement cache size */

        OCI_PoolGetStatementCacheSize(pool);

        /* for connection pools that do not handle the statement cache
           attribute, let's set the value with documented default cache size */

        if (pool->cache_size == 0)
        {
            OCI_PoolSetStatementCacheSize(pool, OCI_DEFAUT_STMT_CACHE_SIZE);
        }

    #endif

    }

    if (call_status)
    {
        call_retval = pool;
    }
    else if (pool)
    {
        OCI_PoolFree(pool);
    }

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_PoolFree
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_PoolFree
(
    OCI_Pool *pool
)
{
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_POOL, pool)

    call_status = OCI_PoolClose(pool);

    OCI_ListRemove(OCILib.pools, pool);

    OCI_FREE(pool)

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_PoolGetConnection
 * --------------------------------------------------------------------------------------------- */

OCI_Connection * OCI_API OCI_PoolGetConnection
(
    OCI_Pool    *pool,
    const otext *tag
)
{
    OCI_LIB_CALL_ENTER(OCI_Connection*, NULL)
    
    OCI_CHECK_PTR(OCI_IPC_POOL, pool)

    call_retval = OCI_ConnectionCreateInternal(pool, pool->db, pool->user, pool->pwd, pool->mode, tag);

    /* for regular connection pool, set the statement cache size to 
       retrieved connection */

 #if OCI_VERSION_COMPILE >= OCI_10_1

    if (call_retval && (OCI_HTYPE_CPOOL == pool->htype))
    {
        unsigned int cache_size = OCI_PoolGetStatementCacheSize(pool);

        OCI_SetStatementCacheSize(call_retval, cache_size);
    }

#endif

    call_status = (NULL != call_retval);
    
    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_PoolGetTimeout
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_PoolGetTimeout
(
    OCI_Pool *pool
)
{
    ub4 value = 0;

    OCI_LIB_CALL_ENTER(unsigned int, value)

    OCI_CHECK_PTR(OCI_IPC_POOL, pool)

    call_status = TRUE;

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

        OCI_CALL3
        (
            call_status, pool->err,

            OCIAttrGet((dvoid *)pool->handle, (ub4)pool->htype,
                        (dvoid *)&value, (ub4 *)NULL,
                        (ub4)attr, pool->err)
        )
    }

#endif

    call_retval = (unsigned int)value;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_PoolSetTimeout
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_PoolSetTimeout
(
    OCI_Pool    *pool,
    unsigned int value
)
{
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_POOL, pool)

    call_status = TRUE;

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

        OCI_CALL3
        (
            call_status, pool->err,

            OCIAttrSet((dvoid *) pool->handle, (ub4) pool->htype,
                       (dvoid *) &timeout,(ub4) sizeof(timeout),
                       (ub4) attr, pool->err)
        )
    }

#endif

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_PoolGetNoWait
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_PoolGetNoWait
(
    OCI_Pool *pool
)
{
    ub1 value = 0;

    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_POOL, pool)

    call_status = TRUE;

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

        OCI_CALL3
        (
            call_status, pool->err,

            OCIAttrGet((dvoid *)pool->handle, (ub4)pool->htype,
                        (dvoid *)&value, (ub4 *)NULL,
                        (ub4)attr, pool->err)
        )
    }

#endif

    call_retval = (boolean)value;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_PoolSetNoWait
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_PoolSetNoWait
(
    OCI_Pool *pool,
    boolean   value
)
{
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_POOL, pool)

    call_status = TRUE;

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

        OCI_CALL3
        (
            call_status, pool->err,

            OCIAttrSet((dvoid *) pool->handle, (ub4) pool->htype,
                       (dvoid *) &nowait, (ub4) sizeof(nowait),
                       (ub4) attr, pool->err)
        )
    }

#endif

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_PoolGetBusyCount
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_PoolGetBusyCount
(
    OCI_Pool *pool
)
{
    ub4 value = 0;

    OCI_LIB_CALL_ENTER(unsigned int, value)

    OCI_CHECK_PTR(OCI_IPC_POOL, pool)

    call_status = TRUE;

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

        OCI_CALL3
        (
            call_status, pool->err,

            OCIAttrGet((dvoid *) pool->handle,(ub4) pool->htype,
                       (dvoid *) &value, (ub4 *) NULL,
                       (ub4) attr, pool->err)
        )
    }

#endif

    call_retval = value;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_PoolGetOpenedCount
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_PoolGetOpenedCount
(
    OCI_Pool *pool
)
{
    ub4 value = 0;

    OCI_LIB_CALL_ENTER(unsigned int, value)

    OCI_CHECK_PTR(OCI_IPC_POOL, pool)

    call_status = TRUE;

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

        OCI_CALL3
        (
            call_status, pool->err,

            OCIAttrGet((dvoid *) pool->handle, (ub4) pool->htype,
                       (dvoid *) &value, (ub4 *) NULL,
                       (ub4) attr, pool->err)
        )
    }

#endif

    call_retval = value;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_PoolGetMin
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_PoolGetMin
(
    OCI_Pool *pool
)
{
    OCI_LIB_CALL_ENTER(unsigned int, 0)

    OCI_CHECK_PTR(OCI_IPC_POOL, pool)

    call_retval = pool->min;
    call_status = TRUE;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_PoolGetMax
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_PoolGetMax
(
    OCI_Pool *pool
)
{
    OCI_LIB_CALL_ENTER(unsigned int, 0)

    OCI_CHECK_PTR(OCI_IPC_POOL, pool)

    call_retval = pool->max;
    call_status = TRUE;

    OCI_LIB_CALL_EXIT()}

/* --------------------------------------------------------------------------------------------- *
 * OCI_PoolGetIncrement
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_PoolGetIncrement
(
    OCI_Pool *pool
)
{
    OCI_LIB_CALL_ENTER(unsigned int, 0)

    OCI_CHECK_PTR(OCI_IPC_POOL, pool)

    call_retval = pool->incr;
    call_status = TRUE;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_PoolSetStatementCacheSize
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_PoolSetStatementCacheSize
(
    OCI_Pool     *pool,
    unsigned int  value
)
{
    ub4 cache_size = value;

    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_POOL, pool)

    call_status = TRUE;

 #if OCI_VERSION_COMPILE >= OCI_10_1

    if (OCILib.version_runtime >= OCI_10_1)
    {
        if (OCI_HTYPE_SPOOL == pool->htype)
        {
            OCI_CALL3
            (
                call_status, pool->err,

                OCIAttrSet((dvoid *) pool->handle, (ub4) pool->htype,
                           (dvoid *) &cache_size, (ub4) sizeof(cache_size),
                           (ub4) OCI_ATTR_SPOOL_STMTCACHESIZE, pool->err)
            )
        }
    }

#endif

    if (call_status)
    {
        pool->cache_size = cache_size;
    }

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_PoolGetStatementCacheSize
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_PoolGetStatementCacheSize
(
    OCI_Pool *pool
)
{
    ub4 cache_size = 0;

    OCI_LIB_CALL_ENTER(unsigned int, cache_size)

    OCI_CHECK_PTR(OCI_IPC_POOL, pool)

    call_status = TRUE;

 #if OCI_VERSION_COMPILE >= OCI_10_1

    if (OCILib.version_runtime >= OCI_10_1)
    {
        if (OCI_HTYPE_SPOOL == pool->htype)
        {
            OCI_CALL3
            (
                call_status, pool->err,

                OCIAttrGet((dvoid **) pool->handle, (ub4) pool->htype,
                           (dvoid *) &cache_size, (ub4 *) NULL,  
                           (ub4) OCI_ATTR_SPOOL_STMTCACHESIZE, pool->err)
            )
        }
        else
        {
            cache_size = pool->cache_size;
        }
        
        if (call_status)
        {
            pool->cache_size = cache_size;
        }
    }

#else

    OCI_NOT_USED(cache_size)

#endif

    if (call_status)
    {
        call_retval = pool->cache_size;
   } 

    OCI_LIB_CALL_EXIT()
}

