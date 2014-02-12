/*
    +-----------------------------------------------------------------------------------------+
    |                                                                                         |
    |                               OCILIB - C Driver for Oracle                              |
    |                                                                                         |
    |                                (C Wrapper for Oracle OCI)                               |
    |                                                                                         |
    |                              Website : http://www.ocilib.net                            |
    |                                                                                         |
    |             Copyright (c) 2007-2014 Vincent ROGIER <vince.rogier@ocilib.net>            |
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
 * $Id: Pool.c, Vincent Rogier $
 * --------------------------------------------------------------------------------------------- */

#include "ocilib_internal.h"

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

    OCI_CHECK_PTR(OCI_IPC_POOL, pool, FALSE);

    /* free all connections */

    OCI_ListForEach(pool->cons, (POCI_LIST_FOR_EACH) OCI_ConnectionClose);
    OCI_ListClear(pool->cons);
    OCI_ListFree(pool->cons);

    pool->cons = NULL;

    if (OCI_LIB_THREADED)
    {
        OCI_MutexFree(pool->mutex);
    }

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

    #if OCI_VERSION_COMPILE >= OCI_9_2

        /* close authentification handle */

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

    OCI_FREE(pool->name);
    OCI_FREE(pool->db);
    OCI_FREE(pool->user);
    OCI_FREE(pool->pwd);

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
    boolean res    = TRUE;

    OCI_CHECK_MIN(NULL, NULL, max_con, 1, NULL);

    /* let's be sure OCI_Initialize() has been called */

    OCI_CHECK_INITIALIZED(NULL);

    /* make sure that we do not have a XA session flag */
    
    mode &= ~OCI_SESSION_XA;
    
    /* create pool object */

    item = OCI_ListAppend(OCILib.pools, sizeof(*pool));

    if (item)
    {
        pool = (OCI_Pool *) item->data;

        /* create internal lists */

        pool->cons = OCI_ListCreate(OCI_IPC_CONNECTION);

        if (OCI_LIB_THREADED)
        {
            /* create mutex for OCI_PoolGetConnection() */

            pool->mutex = OCI_MutexCreateInternal();

            res = (pool->mutex != NULL);
        }
    }
    else
    {
        res = FALSE;
    }

    /* set attributes */

    if (res)
    {
        pool->mode = mode;
        pool->min  = min_con;
        pool->max  = max_con;
        pool->incr = incr_con;

        pool->db   = ostrdup(db   != NULL ? db   : OTEXT(""));
        pool->user = ostrdup(user != NULL ? user : OTEXT(""));
        pool->pwd  = ostrdup(pwd  != NULL ? pwd  : OTEXT(""));
    }

#if OCI_VERSION_COMPILE < OCI_9_2

    type = OCI_POOL_CONNECTION;

#endif

#if OCI_VERSION_COMPILE >= OCI_9_0

    if (res)
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

        if (res)
        {
            res = OCI_SUCCESSFUL(OCI_HandleAlloc((dvoid *) OCILib.env,
                                                 (dvoid **) (void *) &pool->err,
                                                 (ub4) OCI_HTYPE_ERROR,
                                                 (size_t) 0,
                                                 (dvoid **) NULL));
        }

        /* allocate pool handle */

        if (res)
        {
            res = OCI_SUCCESSFUL(OCI_HandleAlloc((dvoid *) OCILib.env,
                                                 (dvoid **) (void *) &pool->handle,
                                                 (ub4) pool->htype,
                                                 (size_t) 0,
                                                 (dvoid **) NULL));
        }

        /* allocate authentification handle only if needed */

   #if OCI_VERSION_COMPILE >= OCI_11_1

        if (res)
        {       
            if ((OCI_HTYPE_SPOOL == pool->htype) && (OCILib.version_runtime >= OCI_11_1))
            {
                int     dbsize = -1;
                dbtext *dbstr  = OCI_StringGetOracleString(OCILIB_DRIVER_NAME, &dbsize);
                    
                /* allocate authentification handle */

                res = OCI_SUCCESSFUL(OCI_HandleAlloc((dvoid *) OCILib.env,
                                                     (dvoid **) (void *) &pool->authp,
                                                     (ub4) OCI_HTYPE_AUTHINFO,
                                                     (size_t) 0,
                                                     (dvoid **) NULL));


                /* set OCILIB's driver layer name attribute only for session pools here
                    For standalone connections and connection pool this attribute is set
                    in OCI_ConnectionLogon() */

                OCI_CALL3
                (
                    res, pool->err,

                    OCIAttrSet((dvoid *) pool->authp, (ub4) OCI_HTYPE_AUTHINFO,
                                (dvoid *) dbstr, (ub4) dbsize,
                                (ub4) OCI_ATTR_DRIVER_NAME, pool->err)
                )
                
                OCI_StringReleaseOracleString(dbstr);

                /* set auth handle on the session pool */

                OCI_CALL3
                (
                    res, pool->err,

                    OCIAttrSet((dvoid *) pool->handle, (ub4) OCI_HTYPE_SPOOL,
                                (dvoid *) pool->authp, (ub4) sizeof(pool->authp),
                                (ub4) OCI_ATTR_SPOOL_AUTH, pool->err)
                ) 
            }
        }

    #endif

        /* create the pool */

        if (res)
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
                    res, pool->err,

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
                    res, pool->err,

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

        if (res && dbstr_name)
        {
            pool->name = OCI_StringDuplicateFromOracleString(dbstr_name, dbcharcount(dbsize_name));

            res = (pool->name != NULL);
        }
    }

#endif

    /* on success, we allocate internal OCI connection objects for pool
       minimum size */

    if (res)
    {     

    #if OCI_VERSION_COMPILE >= OCI_9_0

        /* retrieve statement cache size */

        OCI_PoolGetStatementCacheSize(pool);

        /* for connection pools that do not handle the statement cache
           atribute, let's set the value with documented default cache size */

        if (pool->cache_size == 0)
        {
            OCI_PoolSetStatementCacheSize(pool, OCI_DEFAUT_STMT_CACHE_SIZE);
        }

    #endif

        while (res && (min_con--) > 0)
        {
            res = (NULL != OCI_ConnectionAllocate(pool, pool->db, pool->user, pool->pwd, pool->mode));
        }
    }
    else
    {
        OCI_PoolFree(pool);
        pool = NULL;
    }

    OCI_RESULT(res);

    return pool;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_PoolFree
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_PoolFree
(
    OCI_Pool *pool
)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_POOL, pool, FALSE);

    res = OCI_PoolClose(pool);

    OCI_ListRemove(OCILib.pools, pool);

    OCI_FREE(pool);

    OCI_RESULT(res);

    return res;
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
    OCI_Connection *con = NULL;
    OCI_Item *item      = NULL;
    boolean res         = FALSE;
    boolean found       = FALSE;

    OCI_CHECK_PTR(OCI_IPC_POOL, pool, NULL);

    if (OCI_LIB_THREADED)
    {
        OCI_MutexAcquire(pool->mutex);
    }

    /* first, try to find an unused OCI_Connection in list */

    item = pool->cons->head;

    while (item)
    {
        con = (OCI_Connection *) item->data;

        if (((OCILib.version_runtime >= OCI_9_0) && (con->cstate == OCI_CONN_ALLOCATED)) ||
            ((OCILib.version_runtime <  OCI_9_0) && (con->cstate == OCI_CONN_ATTACHED )))
        {
            found = TRUE;
            break;
        }

        item = item->next;
    }

    if (!found)
    {
        con = NULL;

        /* no available connection found ! Try to allocate a new one... */

        if (OCILib.version_runtime >= OCI_9_0 || pool->cons->count < pool->max)
        {
            ub4 i, nb;
            OCI_Connection *c = NULL;

            nb = pool->nb_opened + pool->incr;

            if (nb > pool->max)
            {
                nb = pool->max;
            }

            for (i = pool->nb_opened; i < nb; i++)
            {
                c = OCI_ConnectionAllocate(pool, pool->db, pool->user,
                                           pool->pwd, pool->mode);

                if (i == pool->nb_opened && c != NULL)
                {
                    con = c;
                }
            }
        }
    }

    if (con)
    {
        res = TRUE;

        if (OCI_CONN_ALLOCATED == con->cstate)
        {
            res = res && OCI_ConnectionAttach(con);
        }

        res = res &&  OCI_ConnectionLogon(con, NULL, tag);

        if (!res)
        {
            OCI_ConnectionFree(con);
            con = NULL;
        }
    }
    else
    {
        con = NULL;
    }

    if (OCI_LIB_THREADED)
    {
        OCI_MutexRelease(pool->mutex);
    }

    /* for regular connection pool, set the statement cache size to 
       retrieved connection */

 #if OCI_VERSION_COMPILE >= OCI_10_1

    if (con && (OCI_HTYPE_CPOOL == pool->htype))
    {
        unsigned int cache_size = OCI_PoolGetStatementCacheSize(pool);

        OCI_SetStatementCacheSize(con, cache_size);
    }

#endif

    OCI_RESULT(res);

    return con;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_PoolGetTimeout
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_PoolGetTimeout
(
    OCI_Pool *pool
)
{
    OCI_CHECK_PTR(OCI_IPC_POOL, pool, 0);

    OCI_RESULT(TRUE);

    return pool->timeout;
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
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_POOL, pool, FALSE);

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
            res, pool->err,

            OCIAttrSet((dvoid *) pool->handle, (ub4) pool->htype,
                       (dvoid *) &timeout,(ub4) sizeof(timeout),
                       (ub4) attr, pool->err)
        )
    }

#endif

    if (res)
    {
        pool->timeout = value;
    }

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_PoolGetNoWait
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_PoolGetNoWait
(
    OCI_Pool *pool
)
{
    OCI_CHECK_PTR(OCI_IPC_POOL, pool, FALSE);

    OCI_RESULT(TRUE);

    return pool->nowait;
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
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_POOL, pool, 0);

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
            res, pool->err,

            OCIAttrSet((dvoid *) pool->handle, (ub4) pool->htype,
                       (dvoid *) &nowait, (ub4) sizeof(nowait),
                       (ub4) attr, pool->err)
        )
    }

#endif

    if (res)
    {
        pool->nowait = value;
    }

    OCI_RESULT(res);

    return TRUE;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_PoolGetBusyCount
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_PoolGetBusyCount
(
    OCI_Pool *pool
)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_POOL, pool, 0);

#if OCI_VERSION_COMPILE >= OCI_9_0

    if (OCILib.version_runtime >= OCI_9_0)
    {
        ub4 value = 0;
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
            res, pool->err,

            OCIAttrGet((dvoid *) pool->handle,(ub4) pool->htype,
                       (dvoid *) &value, (ub4 *) NULL,
                       (ub4) attr, pool->err)
        )

        if (res)
        {
            pool->nb_busy = value;
        }
    }

#endif

    OCI_RESULT(res);

    return pool->nb_busy;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_PoolGetOpenedCount
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_PoolGetOpenedCount
(
    OCI_Pool *pool
)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_POOL, pool, 0);

#if OCI_VERSION_COMPILE >= OCI_9_0

    if (OCILib.version_runtime >= OCI_9_0)
    {
        ub4 value = 0;
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
            res, pool->err,

            OCIAttrGet((dvoid *) pool->handle, (ub4) pool->htype,
                       (dvoid *) &value, (ub4 *) NULL,
                       (ub4) attr, pool->err)
        )

        if (res)
        {
            pool->nb_opened = value;
        }
    }

#endif

    OCI_RESULT(res);

    return pool->nb_opened;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_PoolGetMin
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_PoolGetMin
(
    OCI_Pool *pool
)
{
    OCI_CHECK_PTR(OCI_IPC_POOL, pool, 0);

    OCI_RESULT(TRUE);

    return pool->min;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_PoolGetMax
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_PoolGetMax
(
    OCI_Pool *pool
)
{
    OCI_CHECK_PTR(OCI_IPC_POOL, pool, 0);

    OCI_RESULT(TRUE);

    return pool->max;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_PoolGetIncrement
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_PoolGetIncrement
(
    OCI_Pool *pool
)
{
    OCI_CHECK_PTR(OCI_IPC_POOL, pool, 0);

    OCI_RESULT(TRUE);

    return pool->incr;
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
    boolean res         = TRUE;
    ub4     cache_size  = value;

    OCI_CHECK_PTR(OCI_IPC_POOL, pool, FALSE);

 #if OCI_VERSION_COMPILE >= OCI_10_1

    if (OCILib.version_runtime >= OCI_10_1)
    {
        if (OCI_HTYPE_SPOOL == pool->htype)
        {
            OCI_CALL3
            (
                res, pool->err,

                OCIAttrSet((dvoid *) pool->handle, (ub4) pool->htype,
                           (dvoid *) &cache_size, (ub4) sizeof(cache_size),
                           (ub4) OCI_ATTR_SPOOL_STMTCACHESIZE, pool->err)
            )
        }
    }

#endif

    if (res)
    {
        pool->cache_size = cache_size;
    }

    OCI_RESULT(res);

    return TRUE;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_PoolGetStatementCacheSize
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_PoolGetStatementCacheSize
(
    OCI_Pool *pool
)
{
    boolean res = TRUE;
    ub4     cache_size = 0;

    OCI_CHECK_PTR(OCI_IPC_POOL, pool, 0);

 #if OCI_VERSION_COMPILE >= OCI_10_1

    if (OCILib.version_runtime >= OCI_10_1)
    {
        if (OCI_HTYPE_SPOOL == pool->htype)
        {
            OCI_CALL3
            (
                res, pool->err,

                OCIAttrGet((dvoid **) pool->handle, (ub4) pool->htype,
                           (dvoid *) &cache_size, (ub4 *) NULL,  
                           (ub4) OCI_ATTR_SPOOL_STMTCACHESIZE, pool->err)
            )
        }
        else
        {
            cache_size = pool->cache_size;
        }
        
        if (res)
        {
            pool->cache_size = cache_size;
        }
    }

#else

    OCI_NOT_USED(res);
    OCI_NOT_USED(cache_size);

#endif

    OCI_RESULT(res);

    return pool->cache_size;
}

