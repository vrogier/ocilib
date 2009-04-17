/*
   +----------------------------------------------------------------------+
   |                                                                      |
   |                     OCILIB - C Driver for Oracle                     |
   |                                                                      |
   |                      (C Wrapper for Oracle OCI)                      |
   |                                                                      |
   +----------------------------------------------------------------------+
   |                      Website : http://ocilib.net                     |
   +----------------------------------------------------------------------+
   |               Copyright (c) 2007-2009 Vincent ROGIER                 |
   +----------------------------------------------------------------------+
   | This library is free software; you can redistribute it and/or        |
   | modify it under the terms of the GNU Library General Public          |
   | License as published by the Free Software Foundation; either         |
   | version 2 of the License, or (at your option) any later version.     |
   |                                                                      |
   | This library is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU    |
   | Library General Public License for more details.                     |
   |                                                                      |
   | You should have received a copy of the GNU Library General Public    |
   | License along with this library; if not, write to the Free           |
   | Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.   |
   +----------------------------------------------------------------------+
   |          Author: Vincent ROGIER <vince.rogier@gmail.com>             |
   +----------------------------------------------------------------------+
*/

/* ------------------------------------------------------------------------ *
 * $Id: connpool.c, v 3.2.0 2009/04/20 00:00 Vince $
 * ------------------------------------------------------------------------ */

#include "ocilib_internal.h"

/* ************************************************************************ *
 *                             PRIVATE FUNCTIONS
 * ************************************************************************ */

/* ------------------------------------------------------------------------ *
 * OCI_ConnPoolClose
 * ------------------------------------------------------------------------ */

boolean OCI_ConnPoolClose(OCI_ConnPool *pool)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_CONNPOOL, pool, FALSE);

    /* free all connections */

    OCI_ListForEach(pool->cons, (boolean (*)(void *)) OCI_ConnectionClose);
    OCI_ListClear(pool->cons);
    OCI_ListFree(pool->cons);

    pool->cons = NULL;

    if (OCI_LIB_THREADED)
        OCI_MutexFree(pool->mutex);

#if OCI_VERSION_COMPILE >= OCI_9

    if (OCILib.ver_runtime >= OCI_9)
    {
        /* close connection pool handle */

        if (pool->handle != NULL)
        {
            OCI_CALL0
            (
                res, pool->err,

                OCIConnectionPoolDestroy(pool->handle, pool->err,
                                          (ub4) OCI_DEFAULT)
            )

            OCI_HandleFree((void *) pool->handle, (ub4) OCI_HTYPE_CPOOL);
        }

        /* close error handle */

        if (pool->err != NULL)
            OCI_HandleFree((void *) pool->err, (ub4) OCI_HTYPE_ERROR);
    }

#endif

    pool->err    = NULL;
    pool->handle = NULL;

    /* free strings */

    OCI_FREE(pool->name);
    OCI_FREE(pool->db);
    OCI_FREE(pool->user);
    OCI_FREE(pool->pwd);

    return res;
}

/* ************************************************************************ *
 *                             PUBLIC FUNCTIONS
 * ************************************************************************ */

/* ------------------------------------------------------------------------ *
 * OCI_ConnPoolCreate
 * ------------------------------------------------------------------------ */

OCI_ConnPool * OCI_API OCI_ConnPoolCreate(const mtext *db, const mtext *user,
                                          const mtext *pwd,
                                          unsigned int mode,
                                          unsigned int min_con,
                                          unsigned int max_con,
                                          unsigned int incr_con)
{
    OCI_ConnPool *pool = NULL;
    OCI_Item     *item = NULL;
    boolean res        = TRUE;

    OCI_CHECK_MIN(NULL, NULL, max_con, 1, FALSE);

    /* let's be sure OCI_Initialize() has been called */

    OCI_CHECK_INITIALIZED(NULL);

    /* create connection pool object */

    item = OCI_ListAppend(OCILib.pools, sizeof(*pool));

    if (item != NULL)
    {
        pool = (OCI_ConnPool *) item->data;

        /* create internal lists */

        pool->cons = OCI_ListCreate(OCI_IPC_CONNECTION);

        if (OCI_LIB_THREADED)
        {
            /* create mutex for OCI_ConnPoolGetConnection() */

            pool->mutex = OCI_MutexCreateInternal();

            res = (pool->mutex != NULL);
        }
    }
    else
        res = FALSE;

    /* set attributes */

    if (res == TRUE)
    {

        pool->mode = mode;
        pool->min  = min_con;
        pool->max  = max_con;
        pool->incr = incr_con;

        pool->db   = mtsdup(db   != NULL ? db   : MT(""));
        pool->user = mtsdup(user != NULL ? user : MT(""));
        pool->pwd  = mtsdup(pwd  != NULL ? pwd  : MT(""));
    }


#if OCI_VERSION_COMPILE >= OCI_9

    if (OCILib.ver_runtime >= OCI_9)
    {
        int osize_name  = -1;
        int osize_db    = -1;
        int osize_user  = -1;
        int osize_pwd   = -1;

        void *ostr_name = NULL;
        void *ostr_db   = NULL;
        void *ostr_user = NULL;
        void *ostr_pwd  = NULL;

        /* allocate error handle */

        if (res == TRUE)
            res = (OCI_SUCCESS == OCI_HandleAlloc((dvoid *) OCILib.env,
                                                  (dvoid **) (void *) &pool->err,
                                                  (ub4) OCI_HTYPE_ERROR,
                                                  (size_t) 0,
                                                  (dvoid **) NULL));

        /* allocate connection pool handle */

        if (res == TRUE)
            res = (OCI_SUCCESS == OCI_HandleAlloc((dvoid *) OCILib.env,
                                                  (dvoid **) (void *) &pool->handle,
                                                  (ub4) OCI_HTYPE_CPOOL,
                                                  (size_t) 0,
                                                  (dvoid **) NULL));

        /* create the pool */

        if (res == TRUE)
        {
            ostr_db    = OCI_GetInputMetaString(pool->db,   &osize_db);
            ostr_user  = OCI_GetInputMetaString(pool->user, &osize_user);
            ostr_pwd   = OCI_GetInputMetaString(pool->pwd,  &osize_pwd);

            OCI_CALL3
            (
                res, pool->err,

                OCIConnectionPoolCreate(OCILib.env, pool->err, pool->handle,
                                        (OraText **) (dvoid *) &ostr_name,
                                        (sb4*) &osize_name,
                                        (OraText *) ostr_db, (sb4) osize_db,
                                        (ub4) pool->min, (ub4) pool->max,
                                        (ub4) pool->incr, (OraText *) ostr_user,
                                        (sb4) osize_user, (OraText *) ostr_pwd,
                                        (sb4) osize_pwd,  (ub4) OCI_DEFAULT)
            )

            OCI_ReleaseMetaString(ostr_db);
            OCI_ReleaseMetaString(ostr_user);
            OCI_ReleaseMetaString(ostr_pwd);
        }

        if ((res == TRUE) && (ostr_name != NULL))
        {
            pool->name = (mtext *) OCI_MemAlloc(OCI_IPC_STRING, sizeof(mtext),
                                                (osize_name/sizeof(omtext)) + 1,
                                                FALSE);

            if (pool->name != NULL)
            {
                OCI_CopyString(ostr_name, pool->name, &osize_name,
                               sizeof(omtext), sizeof(mtext));
            }
            else
                res = FALSE;
        }
    }

#endif

   /* on success, we allocate internal OCI connection objects for pool
      minimum size */

    if (res == TRUE)
    {
        OCI_Connection *cn;

        while ((min_con--) > 0)
        {
            cn = OCI_ConnectionAllocate(pool, pool->db, pool->user,
                                        pool->pwd, pool->mode);
        }
    }
    else
    {
        OCI_ConnPoolFree(pool);
        pool = NULL;
    }

    OCI_RESULT(res);

    return pool;
}

/* ------------------------------------------------------------------------ *
 * OCI_ConnPoolFree
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_ConnPoolFree(OCI_ConnPool *pool)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_CONNPOOL, pool, FALSE);

    res = OCI_ConnPoolClose(pool);

    OCI_ListRemove(OCILib.pools, pool);

    OCI_FREE(pool);

    OCI_RESULT(res);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_ConnPoolGetConnection
 * ------------------------------------------------------------------------ */

OCI_Connection * OCI_API OCI_ConnPoolGetConnection(OCI_ConnPool *pool)
{
    OCI_Connection *con  = NULL;
    OCI_Item       *item = NULL;
    boolean res          = FALSE;
    boolean found        = FALSE;

    OCI_CHECK_PTR(OCI_IPC_CONNPOOL, pool, NULL);

    if (OCI_LIB_THREADED)
        OCI_MutexAcquire(pool->mutex);

    /* fist, try to find an unused OCI_Connection in list */

    item = pool->cons->head;

    while (item != NULL)
    {
        con = (OCI_Connection *) item->data;

        if (con->cstate == OCI_CONN_ALLOCATED)
        {
            found = TRUE;
            break;
        }

        item = item->next;
    }

    if (found == FALSE)
    {
        /* no available connection found ! Try to allocate a new one... */

        if (OCILib.ver_runtime >= OCI_9 || pool->cons->count < pool->max)
        {
            ub4 i, nb;
            OCI_Connection *c = NULL;

            nb = pool->nb_opened + pool->incr;

            if (nb > pool->max)
                nb = pool->max;

            for (i = 0; i < nb; i++)
            {
                c = OCI_ConnectionAllocate(pool, pool->db, pool->user,
                                           pool->pwd, pool->mode);

                if (i == 0 && c != NULL)
                   con = c;
            }
        }
    }

    if (con != NULL)
    {
        res = TRUE;

        if (con->cstate == OCI_CONN_ALLOCATED)
            res = res && OCI_ConnectionAttach(con);

        res  = res &&  OCI_ConnectionLogon(con);

        if (res == FALSE)
        {
            OCI_ConnectionFree(con);
            con = NULL;
        }
    }

    if (OCI_LIB_THREADED)
        OCI_MutexRelease(pool->mutex);

    OCI_RESULT(res);

    return con;
}

/* ------------------------------------------------------------------------ *
 * OCI_ConnPoolGetTimeout
 * ------------------------------------------------------------------------ */

unsigned int OCI_API OCI_ConnPoolGetTimeout(OCI_ConnPool *pool)
{
    OCI_CHECK_PTR(OCI_IPC_CONNPOOL, pool, 0);

    OCI_RESULT(TRUE);

    return pool->timeout;
}

/* ------------------------------------------------------------------------ *
 * OCI_ConnPoolSetTimeout
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_ConnPoolSetTimeout(OCI_ConnPool *pool, unsigned int value)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_CONNPOOL, pool, FALSE);

#if OCI_VERSION_COMPILE >= OCI_9

    if (OCILib.ver_runtime >= OCI_9)
    {
        ub4 timeout = value;

        OCI_CALL3
        (
            res, pool->err,

            OCIAttrSet((dvoid *) pool->handle, (ub4) OCI_HTYPE_CPOOL,
                       (dvoid *) &timeout,(ub4) sizeof(timeout),
                       (ub4) OCI_ATTR_CONN_TIMEOUT, pool->err)
        )
    }

#endif

    if (res == TRUE)
        pool->timeout = value;

    OCI_RESULT(res);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_ConnPoolGetlGetNoWait
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_ConnPoolGetlGetNoWait(OCI_ConnPool *pool)
{
    OCI_CHECK_PTR(OCI_IPC_CONNPOOL, pool, FALSE);

    OCI_RESULT(TRUE);

    return pool->nowait;
}

/* ------------------------------------------------------------------------ *
 * OCI_ConnPoolSetNoWait
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_ConnPoolSetNoWait(OCI_ConnPool *pool, boolean value)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_CONNPOOL, pool, 0);

#if OCI_VERSION_COMPILE >= OCI_9

    if (OCILib.ver_runtime >= OCI_9)
    {
        ub1 nowait = (ub1) value;

        OCI_CALL3
        (
            res, pool->err,

            OCIAttrSet((dvoid *) pool->handle, (ub4) OCI_HTYPE_CPOOL,
                       (dvoid *) &nowait, (ub4) sizeof(nowait),
                       (ub4) OCI_ATTR_CONN_NOWAIT, pool->err)
        )
    }

#endif

    if (res == TRUE)
        pool->nowait = value;

    OCI_RESULT(res);

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_ConnPoolGetBusyCount
 * ------------------------------------------------------------------------ */

unsigned int OCI_API OCI_ConnPoolGetBusyCount(OCI_ConnPool *pool)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_CONNPOOL, pool, 0);

#if OCI_VERSION_COMPILE >= OCI_9

    if (OCILib.ver_runtime >= OCI_9)
    {
        ub4 value = 0;

        OCI_CALL3
        (
            res, pool->err,

            OCIAttrGet((dvoid *) pool->handle,(ub4) OCI_HTYPE_CPOOL,
                       (dvoid *) &value, (ub4 *) NULL,
                       (ub4) OCI_ATTR_CONN_BUSY_COUNT, pool->err)
        )

        if (res == TRUE)
            pool->nb_busy = value;
    }

#endif

    OCI_RESULT(res);

    return pool->nb_busy;
}

/* ------------------------------------------------------------------------ *
 * OCI_ConnPoolGetOpenedCount
 * ------------------------------------------------------------------------ */

unsigned int OCI_API OCI_ConnPoolGetOpenedCount(OCI_ConnPool *pool)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_CONNPOOL, pool, 0);

#if OCI_VERSION_COMPILE >= OCI_9

    if (OCILib.ver_runtime >= OCI_9)
    {
        ub4 value = 0;

        OCI_CALL3
        (
            res, pool->err,

            OCIAttrGet((dvoid *) pool->handle, (ub4) OCI_HTYPE_CPOOL,
                       (dvoid *) &value, (ub4 *) NULL,
                       (ub4) OCI_ATTR_CONN_OPEN_COUNT, pool->err)
        )

        if (res == TRUE)
            pool->nb_opened = value;
    }

#endif

     OCI_RESULT(res);

     return pool->nb_opened;
}

/* ------------------------------------------------------------------------ *
 * OCI_ConnPoolGetMin
 * ------------------------------------------------------------------------ */

unsigned int OCI_API OCI_ConnPoolGetMin(OCI_ConnPool *pool)
{
    OCI_CHECK_PTR(OCI_IPC_CONNPOOL, pool, 0);

    OCI_RESULT(TRUE);

    return pool->min;
}

/* ------------------------------------------------------------------------ *
 * OCI_ConnPoolGetMax
 * ------------------------------------------------------------------------ */

unsigned int OCI_API OCI_ConnPoolGetMax(OCI_ConnPool *pool)
{
    OCI_CHECK_PTR(OCI_IPC_CONNPOOL, pool, 0);

    OCI_RESULT(TRUE);

    return pool->max;
}

/* ------------------------------------------------------------------------ *
 * OCI_ConnPoolGetIncrement
 * ------------------------------------------------------------------------ */

unsigned int OCI_API OCI_ConnPoolGetIncrement(OCI_ConnPool *pool)
{
    OCI_CHECK_PTR(OCI_IPC_CONNPOOL, pool, 0);

    OCI_RESULT(TRUE);

    return pool->incr;
}
