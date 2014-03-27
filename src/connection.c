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
 * $Id: connection.c, Vincent Rogier $
 * --------------------------------------------------------------------------------------------- */

#include "ocilib_internal.h"

/* ********************************************************************************************* *
 *                             PRIVATE FUNCTIONS
 * ********************************************************************************************* */

/* --------------------------------------------------------------------------------------------- *
 * OCI_ConnectionAllocate
 * --------------------------------------------------------------------------------------------- */

OCI_Connection * OCI_ConnectionAllocate
(
    OCI_Pool    *pool,
    const otext *db,
    const otext *user,
    const otext *pwd,
    unsigned int mode
)
{
    OCI_Connection *con = NULL;
    OCI_List *list      = NULL;
    OCI_Item *item      = NULL;
    boolean res         = TRUE;

    /* create connection object */

    if (pool)
    {
        list = pool->cons;
    }
    else
    {
        list = OCILib.cons;
    }

    item = OCI_ListAppend(list, sizeof(*con));

    if (item)
    {
        con = (OCI_Connection *) item->data;

        con->alloc_handles = ((con->mode & OCI_SESSION_XA) == 0);

        /* create internal lists */

        con->stmts = OCI_ListCreate(OCI_IPC_STATEMENT);

        if (res)
        {
            con->tinfs = OCI_ListCreate(OCI_IPC_TYPE_INFO);
            res        = (con->tinfs != NULL);
        }

        if (res)
        {
            con->trsns = OCI_ListCreate(OCI_IPC_TRANSACTION);
            res        = (con->trsns != NULL);
        }

        /* set attributes */

        if (res)
        {
            con->mode     = mode;
            con->pool     = pool;
            con->sess_tag = NULL;

            if (con->pool)
            {
                con->db   = (otext *) db;
                con->user = (otext *) user;
                con->pwd  = (otext *) pwd;

            #if OCI_VERSION_COMPILE >= OCI_9_2

                if (OCI_HTYPE_SPOOL == con->pool->htype)
                {
                    con->alloc_handles = FALSE;
                }

            #endif

            }
            else
            {
                con->db   = ostrdup(db   ? db   : OTEXT(""));
                con->user = ostrdup(user ? user : OTEXT(""));
                con->pwd  = ostrdup(pwd  ? pwd  : OTEXT(""));
            }

        #if OCI_VERSION_COMPILE >= OCI_10_1

            if (con->mode & OCI_SESSION_XA)
            {
                char dbname[OCI_SIZE_BUFFER+1];

                memset(dbname, 0, sizeof(dbname));

                if (con->db && con->db[0])
                {
                    OCI_StringNativeToAnsi(con->db, dbname, (int) ostrlen(con->db));         
                }

                con->env = xaoEnv((OraText *) (dbname[0] ? dbname : NULL ));
            }
            else

        #endif

            {
                con->env = OCILib.env;
            }

            res = (con->env != NULL);
        }

        /*  allocate error handle */

        if (res)
        {
            res = OCI_SUCCESSFUL(OCI_HandleAlloc((dvoid *) con->env,
                                                 (dvoid **) (void *) &con->err,
                                                 (ub4) OCI_HTYPE_ERROR,
                                                 (size_t) 0, (dvoid **) NULL));
        }
    }
    else
    {
        res = FALSE;
    }

    /* update internal status */

    if (res)
    {
        con->cstate = OCI_CONN_ALLOCATED;
    }
    else
    {
        OCI_ConnectionFree(con);
        con = NULL;
    }

    return con;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ConnectionDeallocate
 * --------------------------------------------------------------------------------------------- */

boolean OCI_ConnectionDeallocate
(
    OCI_Connection *con
)
{
    OCI_CHECK(con == NULL, FALSE);
    OCI_CHECK(con->cstate != OCI_CONN_ALLOCATED, FALSE);

    /* close error handle */

    if (con->err)
    {
        OCI_HandleFree((dvoid *) con->err, (ub4) OCI_HTYPE_ERROR);
    }

    /* close server handle (if it had been allocated) in case of login error */

    if ((con->svr) && con->alloc_handles)
    {
        OCI_HandleFree((dvoid *) con->svr, (ub4) OCI_HTYPE_SERVER);
    }

    con->cxt = NULL;
    con->ses = NULL;
    con->svr = NULL;
    con->err = NULL;

    return TRUE;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ConnectionAttach
 * --------------------------------------------------------------------------------------------- */

boolean OCI_ConnectionAttach
(
    OCI_Connection *con
)
{
    boolean res = TRUE;

    OCI_CHECK(con == NULL, FALSE);
    OCI_CHECK(con->cstate != OCI_CONN_ALLOCATED, FALSE);

    /* allocate server handle for non session pooled connection */

    if (con->alloc_handles)
    {
        ub4     cmode  = OCI_DEFAULT;
        dbtext *dbstr  = NULL;
        int     dbsize = -1;
        
        res = OCI_SUCCESSFUL(OCI_HandleAlloc((dvoid *) con->env,
                                             (dvoid **) (void *) &con->svr,
                                             (ub4) OCI_HTYPE_SERVER,
                                             (size_t) 0, (dvoid **) NULL));

        /* attach server handle to service name */

    #if OCI_VERSION_COMPILE >= OCI_9_0

        if (OCILib.version_runtime >= OCI_9_0 && con->pool)
        {
            dbstr = OCI_StringGetOracleString(con->pool->name, &dbsize);
            cmode = OCI_CPOOL;
        }
        else

    #endif

        {
            dbstr = OCI_StringGetOracleString(con->db, &dbsize);
        }

        OCI_CALL2
        (
            res, con,

            OCIServerAttach(con->svr, con->err,(OraText *) dbstr, (sb4) dbsize, cmode)
        )

        OCI_StringReleaseOracleString(dbstr);
    }

    /* handle errors */

    if (res)
    {
        if (OCILib.version_runtime < OCI_9_0 && con->pool)
        {
            con->pool->nb_opened++;
        }

        con->cstate = OCI_CONN_ATTACHED;
    }

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ConnectionDetach
 * --------------------------------------------------------------------------------------------- */

boolean OCI_ConnectionDetach
(
    OCI_Connection *con
)
{
    boolean res = TRUE;

    OCI_CHECK(con == NULL, FALSE);
    OCI_CHECK(con->cstate != OCI_CONN_ATTACHED, FALSE);

    if (con->alloc_handles && con->svr)
    {
        /* detach from the oracle server */

        OCI_CALL2
        (
            res, con,

            OCIServerDetach(con->svr, con->err, (ub4) OCI_DEFAULT)
        )

        /* close server handle */

        OCI_HandleFree((dvoid *) con->svr, (ub4) OCI_HTYPE_SERVER);

        con->svr = NULL;
    }

    /* update internal status */

    if (res)
    {
        if (OCILib.version_runtime < OCI_9_0 && con->pool)
        {
            con->pool->nb_opened--;
        }

        con->cstate = OCI_CONN_ALLOCATED;
    }

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ConnectionLogon
 * --------------------------------------------------------------------------------------------- */

boolean OCI_ConnectionLogon
(
    OCI_Connection *con,
    const otext    *new_pwd,
    const otext    *tag
)
{
    dbtext *dbstr  = NULL;
    int dbsize   = -1;
    boolean res = TRUE;

    OCI_CHECK(con == NULL, FALSE);

#if OCI_VERSION_COMPILE < OCI_9_2

    OCI_NOT_USED(tag)

#endif

    /* 1 - XA connection */

#if OCI_VERSION_COMPILE >= OCI_10_1

    if (con->mode & OCI_SESSION_XA)
    {
        char dbname[OCI_SIZE_BUFFER+1];

        memset(dbname, 0, sizeof(dbname));

        if (con->db && con->db[0])
        {
            OCI_StringNativeToAnsi(con->db, dbname, (int) ostrlen(con->db));         
        }

        con->cxt = xaoSvcCtx((OraText *) (dbname[0] ? dbname : NULL ));

        res = (con->cxt != NULL);

        if (res)
        {
            OCI_CALL2
            (
                res, con,

                OCIAttrGet((dvoid **) con->cxt, (ub4) OCI_HTYPE_SVCCTX,
                           (dvoid *) &con->svr, (ub4 *) NULL,
                           (ub4) OCI_ATTR_SERVER, con->err)

            )

            OCI_CALL2
            (
                res, con,

                OCIAttrGet((dvoid **) con->cxt, (ub4) OCI_HTYPE_SVCCTX,
                           (dvoid *) &con->ses, (ub4 *) NULL,
                           (ub4) OCI_ATTR_SESSION, con->err)

            )

            OCI_CALL2
            (
                res, con,

                OCIAttrGet((dvoid **) con->ses, (ub4) OCI_HTYPE_SESSION,
                           (dvoid *) &con->user, (ub4 *) NULL,
                           (ub4) OCI_ATTR_USERNAME, con->err)

            )
        }
    }

#endif

    /* 2 - regular connection and connection from connection pool */

    if (con->alloc_handles)
    {
        /* allocate session handle */

        if (res)
        {
            res = OCI_SUCCESSFUL(OCI_HandleAlloc((dvoid *) con->env,
                                                 (dvoid **) (void *) &con->ses,
                                                 (ub4) OCI_HTYPE_SESSION,
                                                 (size_t) 0, (dvoid **) NULL));
        }

        /* allocate context handle */

        if (res)
        {
            res = OCI_SUCCESSFUL(OCI_HandleAlloc((dvoid *) con->env,
                                                 (dvoid **) (void *) &con->cxt,
                                                 (ub4) OCI_HTYPE_SVCCTX,
                                                 (size_t) 0, (dvoid **) NULL));
        }

        /* set context server attribute */

        OCI_CALL2
        (
            res, con,

            OCIAttrSet((dvoid *) con->cxt, (ub4) OCI_HTYPE_SVCCTX,
                       (dvoid *) con->svr, (ub4) sizeof (con->svr),
                       (ub4) OCI_ATTR_SERVER, con->err)
        )

        /* modifiy user password if needed */

        if (new_pwd && new_pwd[0])
        {
            int   dbsize1 = -1;
            int   dbsize2 = -1;
            int   dbsize3 = -1;
            dbtext *dbstr1  = NULL;
            dbtext *dbstr2  = NULL;
            dbtext *dbstr3  = NULL;

            dbstr1 = OCI_StringGetOracleString(con->user, &dbsize1);
            dbstr2 = OCI_StringGetOracleString(con->pwd,  &dbsize2);
            dbstr3 = OCI_StringGetOracleString(new_pwd,   &dbsize3);

            OCI_CALL2
            (
                res, con,

                OCIAttrSet((dvoid *) con->cxt, (ub4) OCI_HTYPE_SVCCTX,
                           (dvoid *) con->ses, (ub4) sizeof(con->ses),
                           (ub4) OCI_ATTR_SESSION, con->err)
            )

            OCI_CALL2
            (
                res, con,

                OCIPasswordChange(con->cxt, con->err,
                                  (OraText *) dbstr1, (ub4) dbsize1,
                                  (OraText *) dbstr2, (ub4) dbsize2,
                                  (OraText *) dbstr3, (ub4) dbsize3,
                                  OCI_AUTH)
            )

            OCI_StringReleaseOracleString(dbstr1);
            OCI_StringReleaseOracleString(dbstr2);
            OCI_StringReleaseOracleString(dbstr3);
            
            if (res)
            {
                OCI_FREE(con->pwd);

                con->pwd = ostrdup(new_pwd);
            }
        }
        else
        {
            /* set session login attribute */

            if (res && con->user && con->user[0])
            {
                dbsize = -1;
                dbstr  = OCI_StringGetOracleString(con->user, &dbsize);

                OCI_CALL2
                (
                    res, con,

                    OCIAttrSet((dvoid *) con->ses,(ub4)  OCI_HTYPE_SESSION, (dvoid *) dbstr,
                               (ub4) dbsize, (ub4) OCI_ATTR_USERNAME, con->err)
                )

                OCI_StringReleaseOracleString(dbstr);
            }

            /* set session password attribute */

            if (res && con->pwd && con->pwd[0])
            {
                dbsize = -1;
                dbstr  = OCI_StringGetOracleString(con->pwd, &dbsize);

                OCI_CALL2
                (
                    res, con,

                    OCIAttrSet((dvoid *) con->ses, (ub4) OCI_HTYPE_SESSION, (dvoid *) dbstr,
                               (ub4) dbsize, (ub4) OCI_ATTR_PASSWORD, con->err)
                )

                OCI_StringReleaseOracleString(dbstr);
            }


            /* set OCILIB's driver layer name attribute */

        #if OCI_VERSION_COMPILE >= OCI_11_1

            if (res  && (OCILib.version_runtime >= OCI_11_1))
            {
                dbsize = -1;
                dbstr  = OCI_StringGetOracleString(OCILIB_DRIVER_NAME, &dbsize);

                OCI_CALL2
                (
                    res, con,

                    OCIAttrSet((dvoid *) con->ses, (ub4) OCI_HTYPE_SESSION, (dvoid *) dbstr,
                                (ub4) dbsize, (ub4) OCI_ATTR_DRIVER_NAME, con->err)
                )

                OCI_StringReleaseOracleString(dbstr);
            }

        #endif

            /* start session */

            if (res)
            {
                ub4 credt = OCI_CRED_RDBMS;
                ub4 mode  = con->mode;

                if  ((!con->user || !con->user[0]) && (!con->pwd || !con->pwd[0]))
                {
                    credt = OCI_CRED_EXT;
                }

            #if OCI_VERSION_COMPILE >= OCI_9_2
       
                /* activate statement cache is the OCI version supports it */

                if (OCILib.version_runtime >= OCI_9_2)
                {
                    mode |= OCI_STMT_CACHE;
                }

            #endif

                /* start session */

                OCI_CALL2
                (
                    res, con,

                    OCISessionBegin(con->cxt, con->err, con->ses, credt, mode)
                )

                if (res)
                {
                    /* This call has moved after OCISessionBegin() call to
                       enable connection pooling (an error ORA-24324 was thrown if
                       the session handle was set to the service context handle
                       before OCISessionBegin()

                       note  : from v3.7.0, OCISessionBegin() is not used anymore
                               for OCI managed pools
                               From v3.9.4, OCISessionBegin() is not used anymore 
                               only for session pools
                       */

                    OCI_CALL2
                    (
                        res, con,

                        OCIAttrSet((dvoid *) con->cxt, (ub4) OCI_HTYPE_SVCCTX,
                                   (dvoid *) con->ses, (ub4) sizeof(con->ses),
                                   (ub4) OCI_ATTR_SESSION, con->err)
                    )
                }
                else
                {
                    /* could not start session, must free the session and context handles */

                    OCI_HandleFree((dvoid *) con->ses, (ub4) OCI_HTYPE_SESSION);
                    OCI_HandleFree((dvoid *) con->cxt, (ub4) OCI_HTYPE_SVCCTX);

                    con->ses = NULL;
                    con->cxt = NULL;
                }

                if (res)
                {                    
                    if (!(con->mode & OCI_PRELIM_AUTH))
                    {
                        /* create default transaction object */

                       OCI_Transaction *trs = OCI_TransactionCreate(con, 1, OCI_TRANS_READWRITE, NULL);

                       if (trs)
                       {
                           if (OCI_SetTransaction(con, trs))
                           {
                                /* start transaction */

                                res = OCI_TransactionStart(trs);
                           }
                       }
                    }
                }
            }
        }
    }

#if OCI_VERSION_COMPILE >= OCI_9_2

   /* 3 - connection from session pool */

    else 
    {
        if (OCILib.version_runtime >= OCI_9_0)
        {
            ub4     mode       = OCI_DEFAULT;
            boolean found      = FALSE;
            dbtext *dbstr_tag  = NULL;
            int     dbsize_tag = 0;

            OraText *dbstr_ret = NULL;
            ub4 dbsize_ret     = 0;

            dbsize = -1;
            dbstr  = OCI_StringGetOracleString(con->pool->name, &dbsize);

            mode = OCI_SESSGET_SPOOL;

            if (tag && tag[0])
            {
                dbsize_tag = -1;
                dbstr_tag  = OCI_StringGetOracleString(tag, &dbsize_tag);
            }

            OCI_CALL2
            (
                res, con,

                OCISessionGet(con->env, con->err, &con->cxt, NULL,
                              (OraText  *) dbstr, (ub4) dbsize, (OraText *) dbstr_tag, dbsize_tag,
                              (OraText **) &dbstr_ret, &dbsize_ret, &found, mode)
            )

            OCI_StringReleaseOracleString(dbstr);
            OCI_StringReleaseOracleString(dbstr_tag);

            OCI_CALL2
            (
                res, con,

                OCIAttrGet((dvoid **) con->cxt, (ub4) OCI_HTYPE_SVCCTX,
                            (dvoid *) &con->svr, (ub4 *) NULL,
                            (ub4) OCI_ATTR_SERVER, con->err)

            )

            OCI_CALL2
            (
                res, con,

                OCIAttrGet((dvoid **) con->cxt, (ub4) OCI_HTYPE_SVCCTX,
                            (dvoid *) &con->ses, (ub4 *) NULL,
                            (ub4) OCI_ATTR_SESSION, con->err)

            )

            if (res && found)
            {
                OCI_SetSessionTag(con, tag);
            }
        }
    }

#endif

    /* check for success */

    if (res)
    {
        /* get server version */

        OCI_GetVersionServer(con);

        /* update internal status */

        if (OCILib.version_runtime < OCI_9_0 && con->pool)
        {
            con->pool->nb_busy++;
        }

        con->cstate = OCI_CONN_LOGGED;
    }

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ConnectionLogOff
 * --------------------------------------------------------------------------------------------- */

boolean OCI_ConnectionLogOff
(
    OCI_Connection *con
)
{
    boolean res = TRUE;

    OCI_CHECK(con == NULL, FALSE);
    OCI_CHECK(con->cstate != OCI_CONN_LOGGED, FALSE);

    /* close opened files */

    if (con->nb_files > 0)
    {
        OCILobFileCloseAll(con->cxt, con->err);
    }

    /* deassociate connection from existing subscriptions */

    OCI_SubscriptionDetachConnection(con);

    /* free all statements */

    OCI_ListForEach(con->stmts, (POCI_LIST_FOR_EACH) OCI_StatementClose);
    OCI_ListClear(con->stmts);

    /* free all type info objects */

    OCI_ListForEach(con->tinfs, (POCI_LIST_FOR_EACH) OCI_TypeInfoClose);
    OCI_ListClear(con->tinfs);

    /* free all transactions */

    OCI_ListForEach(con->trsns, (POCI_LIST_FOR_EACH) OCI_TransactionClose);
    OCI_ListClear(con->trsns);

    /* close session */

    if (con->alloc_handles)
    {
        /* close any server files not explicitly closed - no check of return code */

        if  (con->cxt && con->err && con->ses)
        {
            OCI_CALL2
            (
                res, con,

                OCISessionEnd(con->cxt, con->err, con->ses, (ub4) OCI_DEFAULT)
            )

            /* close session handle */

            if (con->ses)
            {
                OCI_HandleFree((dvoid *) con->ses, (ub4) OCI_HTYPE_SESSION);

                con->ses = NULL;
            }

            /* close context handle */

            if (con->cxt)
            {
                OCI_HandleFree((dvoid *) con->cxt, (ub4) OCI_HTYPE_SVCCTX);

                con->cxt = NULL;
            }
        }
    }
    else
    {
        /* No explicit transaction object => commit if needed otherwise rollback changes */

        if (con->autocom)
        {
             OCI_Commit(con);
        }
        else
        {
             OCI_Rollback(con);
        }

    #if OCI_VERSION_COMPILE >= OCI_9_2

        if (OCILib.version_runtime >= OCI_9_0)
        {
            dbtext *dbstr  = NULL;
            int     dbsize = 0;
            ub4     mode   = OCI_DEFAULT;

            /* Clear session tag if connection was retrieved from session pool */

            if (con->pool && con->sess_tag && ( (ub4) OCI_HTYPE_SPOOL == con->pool->htype))
            {
                dbsize = -1;
                dbstr  = OCI_StringGetOracleString(con->sess_tag, &dbsize);
                mode   = OCI_SESSRLS_RETAG;
            }

            OCI_CALL2
            (
                res, con,

                OCISessionRelease(con->cxt, con->err, (OraText*) dbstr, (ub4) dbsize, mode)
            )

            OCI_StringReleaseOracleString(dbstr);

            con->cxt = NULL;
            con->ses = NULL;
            con->svr = NULL;
        }

        #endif
    }

    /* update internal status */

    if (res)
    {
        con->cstate = OCI_CONN_ATTACHED;

        if (OCILib.version_runtime < OCI_9_0 && con->pool)
        {
            con->pool->nb_busy--;
        }
    }

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ConnectionClose
 * --------------------------------------------------------------------------------------------- */

boolean OCI_ConnectionClose
(
    OCI_Connection *con
)
{
    OCI_CHECK(con == NULL, FALSE);

    /* clear server output resources */

    OCI_ServerDisableOutput(con);

    /* lofoff and detatch form server */

    OCI_ConnectionLogOff(con);
    OCI_ConnectionDetach(con);
    OCI_ConnectionDeallocate(con);

    /* free internal lists */

    OCI_ListFree(con->stmts);
    OCI_ListFree(con->trsns);
    OCI_ListFree(con->tinfs);

    /* free strings */

    OCI_FREE(con->fmt_date);
    OCI_FREE(con->fmt_num);
    OCI_FREE(con->ver_str);
    OCI_FREE(con->sess_tag);
    OCI_FREE(con->db_name);
    OCI_FREE(con->inst_name);
    OCI_FREE(con->service_name);
    OCI_FREE(con->server_name);
    OCI_FREE(con->db_name);
    OCI_FREE(con->domain_name);

    if (!con->pool)
    {
        OCI_FREE(con->db);
        OCI_FREE(con->user);
        OCI_FREE(con->pwd);
    }

    if (con->inst_startup)
    {
        OCI_TimestampFree(con->inst_startup);
    }

    con->stmts = NULL;
    con->trsns = NULL;
    con->tinfs = NULL;

    return TRUE;
}

/* ********************************************************************************************* *
 *                             PUBLIC FUNCTIONS
 * ********************************************************************************************* */

/* --------------------------------------------------------------------------------------------- *
 * OCI_ConnectionCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Connection * OCI_API OCI_ConnectionCreate
(
    const otext *db,
    const otext *user,
    const otext *pwd,
    unsigned int mode
)
{
    OCI_Connection * con;

    /* let's be sure OCI_Initialize() has been called */

    OCI_CHECK_INITIALIZED(NULL);

    /* check for XA connections support */

    OCI_CHECK_XA_ENABLED(mode, NULL);

    /* create connection */

    con = OCI_ConnectionAllocate(NULL, db, user, pwd, mode);

    if (con)
    {
        if (!OCI_ConnectionAttach(con) || !OCI_ConnectionLogon(con, NULL, NULL))
        {
            OCI_ConnectionFree(con);
            con = NULL;
        }
    }

    OCI_RESULT(con != NULL);

    return con;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ConnectionFree
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ConnectionFree
(
    OCI_Connection *con
)
{
    boolean res    = TRUE;
    OCI_Error *err = NULL;

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, FALSE);

    /* clear connection reference from current error object */

    err = OCI_ErrorGet(FALSE, FALSE);

    if (err && err->con == con)
    {
        err->con = NULL;
    }

    OCI_FREE(con->sess_tag);

    if (con->pool)
    {
        res = OCI_ConnectionLogOff(con);

        if (OCILib.version_runtime >= OCI_9_0)
        {
            OCI_ConnectionDetach(con);
        }
    }
    else
    {
        res = OCI_ConnectionClose(con);
        OCI_ListRemove(OCILib.cons, con);
        OCI_FREE(con);
    }

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_Commit
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_Commit
(
    OCI_Connection *con
)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, FALSE);

    OCI_CALL2
    (
        res, con,

        OCITransCommit(con->cxt, con->err, (ub4) OCI_DEFAULT)
    )

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_Rollback
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_Rollback
(
    OCI_Connection *con
)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, FALSE);

    OCI_CALL2
    (
        res, con,

        OCITransRollback(con->cxt, con->err, (ub4) OCI_DEFAULT)
    )

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_SetAutoCommit
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_SetAutoCommit
(
    OCI_Connection *con,
    boolean         enable
)
{
    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, FALSE);

    OCI_RESULT(TRUE);

    con->autocom = enable;

    return TRUE;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetAutoCommit
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_GetAutoCommit
(
    OCI_Connection *con
)
{
    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, FALSE);

    OCI_RESULT(TRUE);

    return con->autocom;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_IsConnected
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_IsConnected
(
    OCI_Connection *con
)
{
    boolean res = TRUE;
    ub4 status  = 0;
    ub4 size    = (ub4) sizeof(status);

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, FALSE);

    OCI_CALL2
    (
        res, con,

        OCIAttrGet((dvoid **) con->svr, (ub4) OCI_HTYPE_SERVER, (dvoid *) &status,
                   (ub4 *) &size, (ub4) OCI_ATTR_SERVER_STATUS, con->err)
    )

    OCI_RESULT(res);

    return (status == OCI_SERVER_NORMAL);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetUserData
 * --------------------------------------------------------------------------------------------- */

void * OCI_API OCI_GetUserData
(
    OCI_Connection *con
)
{
    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, NULL);

    OCI_RESULT(TRUE);

    return con->usrdata;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_SetSetData
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_SetUserData
(
    OCI_Connection *con,
    void           *data
)
{
    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, FALSE);

    OCI_RESULT(TRUE);

    con->usrdata = data;

    return TRUE;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_SetSessionTag
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_SetSessionTag
(
    OCI_Connection *con,
    const otext    *tag
)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, FALSE);

    OCI_RESULT(TRUE);

    OCI_FREE(con->sess_tag);

#if OCI_VERSION_COMPILE >= OCI_9_2

    if (tag && con->pool && ((ub4) OCI_HTYPE_SPOOL == con->pool->htype))
    {
        con->sess_tag = ostrdup(tag);

        res = (con->sess_tag != NULL);
    }

#else

    OCI_NOT_USED(tag)

#endif

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetSessionTag
 * --------------------------------------------------------------------------------------------- */

const otext * OCI_API OCI_GetSessionTag
(
    OCI_Connection *con
)
{
    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, NULL);

    OCI_RESULT(TRUE);

    return con->sess_tag;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetDatabase
 * --------------------------------------------------------------------------------------------- */

const otext * OCI_API OCI_GetDatabase
(
    OCI_Connection *con
)
{
    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, NULL);

    OCI_RESULT(TRUE);

    return (const otext *) con->db;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetUserName
 * --------------------------------------------------------------------------------------------- */

const otext * OCI_API OCI_GetUserName
(
    OCI_Connection *con
)
{
    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, NULL);

    OCI_RESULT(TRUE);

    return (const otext *) con->user;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetPassword
 * --------------------------------------------------------------------------------------------- */

const otext * OCI_API OCI_GetPassword
(
    OCI_Connection *con
)
{
    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, NULL);

    OCI_RESULT(TRUE);

    return (const otext *) con->pwd;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_SetPassword
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_SetPassword
(
    OCI_Connection *con,
    const otext    *password
)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, FALSE);
    OCI_CHECK_PTR(OCI_IPC_STRING, password, FALSE);

    if (OCI_CONN_LOGGED != con->cstate)
    {
        OCI_ConnectionLogon(con, password, NULL);
    }
    else
    {
        int   dbsize1 = -1;
        int   dbsize2 = -1;
        int   dbsize3 = -1;
        dbtext *dbstr1  = NULL;
        dbtext *dbstr2  = NULL;
        dbtext *dbstr3  = NULL;

        dbstr1 = OCI_StringGetOracleString(con->user,  &dbsize1);
        dbstr2 = OCI_StringGetOracleString(con->pwd,   &dbsize2);
        dbstr3 = OCI_StringGetOracleString(password,   &dbsize3);

        OCI_CALL2
        (
            res, con,

            OCIPasswordChange(con->cxt, con->err,
                              (OraText *) dbstr1, (ub4) dbsize1,
                              (OraText *) dbstr2, (ub4) dbsize2,
                              (OraText *) dbstr3, (ub4) dbsize3,
                              OCI_DEFAULT)
        )

        OCI_StringReleaseOracleString(dbstr1);
        OCI_StringReleaseOracleString(dbstr2);
        OCI_StringReleaseOracleString(dbstr3);
    }

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_SetUserPassword
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_SetUserPassword
(
    const otext *db,
    const otext *user,
    const otext *pwd,
    const otext *new_pwd
)
{
    OCI_Connection * con = NULL;
    boolean res = FALSE;

    /* let's be sure OCI_Initialize() has been called */

    OCI_CHECK_INITIALIZED(FALSE);

    con = OCI_ConnectionAllocate(NULL, db, user, pwd, OCI_AUTH);

    if (con)
    {
        if (!OCI_ConnectionAttach(con) || !OCI_ConnectionLogon(con, new_pwd, NULL))
        {
            OCI_ConnectionFree(con);
            con = NULL;
        }
    }

    if (con)
    {
        res = TRUE;
        OCI_ConnectionFree(con);
    }
    else
    {
        res = FALSE;
    }

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetSessionMode
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_GetSessionMode
(
    OCI_Connection *con
)
{
    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, OCI_UNKNOWN);

    OCI_RESULT(TRUE);

    return con->mode;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetVersionServer
 * --------------------------------------------------------------------------------------------- */

const otext * OCI_API OCI_GetVersionServer
(
    OCI_Connection *con
)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, NULL);

    /* no version available in prelim mode */

    if (!con->ver_str && (!(con->mode & OCI_PRELIM_AUTH)))
    {
        int     dbsize = OCI_SIZE_BUFFER * (int) sizeof(dbtext);
        dbtext *dbstr  = NULL;
       
        con->ver_str = (otext *) OCI_MemAlloc(OCI_IPC_STRING, sizeof(otext), OCI_SIZE_BUFFER + 1, TRUE); 

        dbstr = OCI_StringGetOracleString(con->ver_str, &dbsize);

        OCI_CALL2
        (
            res, con,

            OCIServerVersion((dvoid *) con->cxt, con->err, (OraText *) dbstr,
                                (ub4) dbsize, (ub1) OCI_HTYPE_SVCCTX)
        )

        OCI_StringCopyOracleStringToNativeString(dbstr, con->ver_str, dbcharcount(dbsize));
        OCI_StringReleaseOracleString(dbstr);

        if (res)
        {
            otext *p = NULL;
                
            int ver_maj, ver_min, ver_rev;

            ver_maj = ver_min = ver_rev = 0;

            con->ver_str[ocharcount(dbsize)] = 0;

            /* parse server version string to find the version information
                **/

            for (p = con->ver_str; (p != NULL) && (*p != 0); p++)
            {
                if (oisdigit((unsigned char) *p) &&
                    (*(p + (size_t) 1) != 0) &&
                    (*(p + (size_t) 1) == OTEXT('.') || (*(p + (size_t) 2) == OTEXT('.') )))
                {
                    if (OCI_NB_ARG_VERSION == osscanf(p, OTEXT("%d.%d.%d"),
                                                        (int *) &ver_maj,
                                                        (int *) &ver_min,
                                                        (int *) &ver_rev))
                    {
                        con->ver_num = ver_maj*100 + ver_min*10 + ver_rev;
                    }

                    break;
                }
            }
        }
        else
        {
            OCI_FREE(con->ver_str);
        }
    }

    OCI_RESULT(res);

    return con->ver_str;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetServerMajorVersion
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_GetServerMajorVersion
(
    OCI_Connection *con
)
{
    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, OCI_UNKNOWN);

    if (OCI_UNKNOWN == con->ver_num)
    {
        OCI_GetVersionServer(con);
    }

    OCI_RESULT(OCI_UNKNOWN != con->ver_num);

    return (unsigned int) OCI_VER_MAJ(con->ver_num);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetServerMinorVersion
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_GetServerMinorVersion
(
    OCI_Connection *con
)
{
    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, OCI_UNKNOWN);

    if (OCI_UNKNOWN == con->ver_num)
    {
        OCI_GetVersionServer(con);
    }

    OCI_RESULT(OCI_UNKNOWN != con->ver_num);

    return OCI_VER_MIN(con->ver_num);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetServerRevisionVersion
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_GetServerRevisionVersion
(
    OCI_Connection *con
)
{
    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, OCI_UNKNOWN);

    if (OCI_UNKNOWN == con->ver_num)
    {
        OCI_GetVersionServer(con);
    }

    OCI_RESULT(OCI_UNKNOWN != con->ver_num);

    return (unsigned int) OCI_VER_REV(con->ver_num);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetTransaction
 * --------------------------------------------------------------------------------------------- */

OCI_Transaction * OCI_API OCI_GetTransaction
(
    OCI_Connection *con
)
{
    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, NULL);

    OCI_RESULT(TRUE);

    return con->trs;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_SetTransaction
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_SetTransaction
(
    OCI_Connection  *con,
    OCI_Transaction *trans
)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, FALSE);
    OCI_CHECK_PTR(OCI_IPC_TRANSACTION, trans, FALSE);

    if (con->trs)
    {
        res = OCI_TransactionStop(con->trs);
    }

    if (res)
    {
        con->trs = trans;

        /* set context transaction attribute */

        OCI_CALL2
        (
            res, con,

            OCIAttrSet((dvoid *) con->cxt, (ub4) OCI_HTYPE_SVCCTX,
                       (dvoid *) con->trs->htr, (ub4) sizeof(con->trs->htr),
                       (ub4) OCI_ATTR_TRANS, con->err)
        )
    }

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetVersionConnection
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_GetVersionConnection
(
    OCI_Connection *con
)
{
    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, OCI_UNKNOWN);

    OCI_RESULT(TRUE);

    /* return the minimum supported version */

    return (OCILib.version_runtime > con->ver_num) ? con->ver_num : OCILib.version_runtime;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_SetDefaultFormatDate
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_SetDefaultFormatDate
(
    OCI_Connection *con,
    const otext    *format
)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, FALSE);

    OCI_FREE(con->fmt_date);

    con->fmt_date = ostrdup(format ? format : OCI_STRING_FORMAT_DATE);

    res = (con->fmt_date != NULL);

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetDefaultFormatDate
 * --------------------------------------------------------------------------------------------- */

const otext * OCI_API OCI_GetDefaultFormatDate
(
    OCI_Connection *con
)
{
    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, NULL);

    OCI_RESULT(TRUE);

    if (!con->fmt_date)
    {
        OCI_SetDefaultFormatDate(con, NULL);
    }

    return (con->fmt_date);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_SetDefaultFormatNumeric
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_SetDefaultFormatNumeric
(
    OCI_Connection *con,
    const otext    *format
)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, FALSE);

    OCI_FREE(con->fmt_num);

    con->fmt_num = ostrdup(format ? format : OCI_STRING_FORMAT_NUM);

    res = (con->fmt_num != NULL);

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetDefaultFormatNumeric
 * --------------------------------------------------------------------------------------------- */

const otext * OCI_API OCI_GetDefaultFormatNumeric
(
    OCI_Connection *con
)
{
    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, NULL);

    OCI_RESULT(TRUE);

    if (!con->fmt_num)
    {
        OCI_SetDefaultFormatNumeric(con, NULL);
    }

    return (con->fmt_num);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_Break
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_Break
(
    OCI_Connection *con
)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, FALSE);

    OCI_CALL2
    (
        res, con,

        OCIBreak((dvoid *) con->cxt, con->err)
    )

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ServerEnableOutput
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ServerEnableOutput
(
    OCI_Connection *con,
    unsigned int    bufsize,
    unsigned int    arrsize,
    unsigned int    lnsize
)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, FALSE);

    /* initialize the output buffer on server side */

    if (!con->svopt)
    {
        con->svopt = (OCI_ServerOutput *) OCI_MemAlloc(OCI_IPC_SERVER_OUPUT, sizeof(*con->svopt),
                                                       (size_t) 1, TRUE);

        res = (con->svopt != NULL);
    }

    /* allocation internal buffer if needed */

    if (res && !con->svopt->arrbuf)
    {
        unsigned int charsize = sizeof(otext);

        /* check params ranges ( Oracle 10g increased the size of output line */

        if (con->ver_num >= OCI_10_2)
        {
            if (lnsize < OCI_OUPUT_LSIZE)
            {
                lnsize = OCI_OUPUT_LSIZE;
            }
            else if (lnsize > OCI_OUPUT_LSIZE_10G)
            {
                lnsize = OCI_OUPUT_LSIZE_10G;
            }
        }
        else
        {
            if (lnsize > OCI_OUPUT_LSIZE)
            {
                lnsize = OCI_OUPUT_LSIZE;
            }
        }

        con->svopt->arrsize = arrsize;
        con->svopt->lnsize  = lnsize;

        /* allocate internal string (line) array */

        con->svopt->arrbuf = (ub1 *) OCI_MemAlloc(OCI_IPC_STRING,
                                                  ((size_t)(con->svopt->lnsize + 1)) * charsize,
                                                  (size_t) con->svopt->arrsize, TRUE);

        res = (con->svopt->arrbuf != NULL);
    }

    if (res)
    {
        if (!con->svopt->stmt)
        {
            con->svopt->stmt = OCI_StatementCreate(con);
        }

        if (con->svopt->stmt)
        {
            /* enable server ouput */

            res = OCI_Prepare(con->svopt->stmt, OTEXT("BEGIN DBMS_OUTPUT.ENABLE(:n); END;"));

            res = res && OCI_BindUnsignedInt(con->svopt->stmt, OTEXT(":n"), &bufsize);

            if (bufsize == 0)
            {
                res = OCI_BindSetNull(OCI_GetBind(con->svopt->stmt, 1));
            }

            res = res && OCI_Execute(con->svopt->stmt);

            /* prepare the retreival statement call */

            con->svopt->cursize = con->svopt->arrsize;

            res = res && OCI_Prepare(con->svopt->stmt, OTEXT("BEGIN DBMS_OUTPUT.GET_LINES(:s, :i); END;"));

            res = res && OCI_BindArrayOfStrings(con->svopt->stmt,  OTEXT(":s"),
                                                (otext *) con->svopt->arrbuf,
                                                con->svopt->lnsize,
                                                con->svopt->arrsize);

            res = res && OCI_BindUnsignedInt(con->svopt->stmt, OTEXT(":i"), &con->svopt->cursize);
        }
    }

    if (!res)
    {
        OCI_ServerDisableOutput(con);
    }

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ServerDisableOutput
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ServerDisableOutput
(
    OCI_Connection *con
)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, FALSE);

    if (con->svopt)
    {
        res = res && OCI_ExecuteStmt(con->svopt->stmt, OTEXT("BEGIN DBMS_OUTPUT.DISABLE(); END;"));

        res = res && OCI_StatementFree(con->svopt->stmt);

        OCI_FREE(con->svopt->arrbuf);
        OCI_FREE(con->svopt);
    }

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ServerGetOutput
 * --------------------------------------------------------------------------------------------- */

const otext * OCI_API OCI_ServerGetOutput
(
    OCI_Connection *con
)
{
    boolean res = TRUE;
    otext *str  = NULL;

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, FALSE);
    OCI_CHECK(con->svopt == NULL, FALSE);

    if (con->svopt->curpos == 0 || con->svopt->curpos >= con->svopt->cursize)
    {
        con->svopt->cursize = con->svopt->arrsize;
        res                 = OCI_Execute(con->svopt->stmt);
        con->svopt->curpos  = 0;
    }

    if (res && (con->svopt->cursize > 0))
    {
        unsigned int charsize = sizeof(otext);

        str = (otext*) (con->svopt->arrbuf + (size_t) (((con->svopt->lnsize + 1) * charsize) * con->svopt->curpos++));
    }

    OCI_RESULT(res);

    return (const otext *) str;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_SetTrace
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_SetTrace
(
    OCI_Connection *con,
    unsigned int    trace,
    const otext    *value
)
{
    boolean res = TRUE;
    otext *str  = NULL;

#if OCI_VERSION_COMPILE >= OCI_10_1

    ub4 attrib = 0;

#endif

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, FALSE);

    /* allocate trace info structure only if trace functions are used */

    if (con->trace == NULL)
    {
        con->trace = (OCI_TraceInfo *) OCI_MemAlloc(OCI_IPC_TRACE_INFO, sizeof(*con->trace),
                                                    (size_t) 1, TRUE);
        res = (con->trace != NULL);
    }

    /* set trace properties */

    if (con->trace)
    {
        switch (trace)
        {
            case OCI_TRC_IDENTITY:
            {

            #if OCI_VERSION_COMPILE >= OCI_10_1

                attrib = OCI_ATTR_CLIENT_IDENTIFIER;

            #endif

                con->trace->identifier[0] = 0;

                ostrncat(con->trace->identifier, value, OCI_SIZE_TRACE_ID);

                str = con->trace->identifier;

                break;
            }
            case OCI_TRC_MODULE:
            {

            #if OCI_VERSION_COMPILE >= OCI_10_1

                attrib = OCI_ATTR_MODULE;

            #endif

                con->trace->module[0] = 0;

                ostrncat(con->trace->module, value, OCI_SIZE_TRACE_MODULE);

                str = con->trace->module;

                break;
            }
            case OCI_TRC_ACTION:
            {

            #if OCI_VERSION_COMPILE >= OCI_10_1

                attrib = OCI_ATTR_ACTION;

            #endif

                con->trace->action[0] = 0;

                ostrncat(con->trace->action, value, OCI_SIZE_TRACE_ACTION);

                str = con->trace->action;

                break;
            }
            case OCI_TRC_DETAIL:
            {

            #if OCI_VERSION_COMPILE >= OCI_10_1

                attrib = OCI_ATTR_CLIENT_INFO;

            #endif

                con->trace->info[0] = 0;

                ostrncat(con->trace->info, value,  OCI_SIZE_TRACE_INF0);

                str = con->trace->info;

                break;
            }
            default:
            {
                res = FALSE;

                break;
            }
        }
    }

#if OCI_VERSION_COMPILE >= OCI_10_1

    /* On success, we give the value to Oracle to record it in system session view */

    if (res)
    {
        dbtext *dbstr  = NULL;
        int     dbsize = -1;

        dbstr = OCI_StringGetOracleString(str, &dbsize);

        if (!str)
        {
            dbsize = 0;
        }

        OCI_CALL2
        (
            res, con,

            OCIAttrSet((dvoid *) con->ses, (ub4) OCI_HTYPE_SESSION,
                       (dvoid *) dbstr, (ub4) dbsize, attrib, con->err)
        )

        OCI_StringReleaseOracleString(dbstr);
    }

#endif

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_TraceGet
 * --------------------------------------------------------------------------------------------- */

const otext * OCI_API OCI_GetTrace
(
    OCI_Connection *con,
    unsigned int    trace
)
{
    const otext *str = NULL;
    boolean res      = TRUE;

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, NULL);

    if (con->trace)
    {
        switch (trace)
        {
            case OCI_TRC_IDENTITY:
            {
                str = con->trace->identifier;
                break;
            }
            case OCI_TRC_MODULE:
            {
                str = con->trace->module;
                break;
            }
            case OCI_TRC_ACTION:
            {
                str = con->trace->action;
                break;
            }
            case OCI_TRC_DETAIL:
            {
                str = con->trace->info;
                break;
            }
            default:
            {
                res = FALSE;
                break;
            }
        }
    }

    OCI_RESULT(res);

    return str;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_Ping
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_Ping
(
    OCI_Connection *con
)
{
    boolean res = TRUE;
    boolean ret = FALSE;

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, FALSE);

#if OCI_VERSION_COMPILE >= OCI_10_2

    if (OCILib.version_runtime >= OCI_10_2)
    {
        OCI_CALL2
        (
            res, con,

            OCIPing(con->cxt, con->err, (ub4) OCI_DEFAULT)

        )

        ret = res;
    }

#endif

    OCI_RESULT(res);

    return ret;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetDBName
 * --------------------------------------------------------------------------------------------- */

const otext * OCI_API OCI_GetDBName
(
    OCI_Connection *con
)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, NULL);

#if OCI_VERSION_COMPILE >= OCI_10_2

    if (!con->db_name)
    {
        res = OCI_StringGetFromAttrHandle(con, con->svr, OCI_HTYPE_SERVER, 
                                          OCI_ATTR_DBNAME, &con->db_name);
    }

#endif

    OCI_RESULT(res);

    return con->db_name;
} 

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetInstanceName
 * --------------------------------------------------------------------------------------------- */

const otext * OCI_API OCI_GetInstanceName
(
    OCI_Connection *con
)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, NULL);

#if OCI_VERSION_COMPILE >= OCI_10_2

    if (!con->inst_name)
    {
        res = OCI_StringGetFromAttrHandle(con, con->svr, OCI_HTYPE_SERVER, 
                                          OCI_ATTR_INSTNAME, &con->inst_name);
    }

#endif

    OCI_RESULT(res);

    return con->inst_name;
} 

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetServiceName
 * --------------------------------------------------------------------------------------------- */

const otext * OCI_API OCI_GetServiceName
(
    OCI_Connection *con
)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, NULL);

#if OCI_VERSION_COMPILE >= OCI_10_2

    if (!con->service_name)
    {
        res = OCI_StringGetFromAttrHandle(con, con->svr, OCI_HTYPE_SERVER, 
                                          OCI_ATTR_SERVICENAME, &con->service_name);
    }

#endif

    OCI_RESULT(res);

    return con->service_name;
} 

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetServerName
 * --------------------------------------------------------------------------------------------- */

const otext * OCI_API OCI_GetServerName
(
    OCI_Connection *con
)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, NULL);

#if OCI_VERSION_COMPILE >= OCI_10_2

    if (!con->server_name)
    {
        res = OCI_StringGetFromAttrHandle(con, con->svr, OCI_HTYPE_SERVER, 
                                          OCI_ATTR_HOSTNAME, &con->server_name);
    }

#endif

    OCI_RESULT(res);

    return con->server_name;
} 

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetDomainName
 * --------------------------------------------------------------------------------------------- */

const otext * OCI_API OCI_GetDomainName
(
    OCI_Connection *con
)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, NULL);

#if OCI_VERSION_COMPILE >= OCI_10_2

    if (!con->domain_name)
    {
        res = OCI_StringGetFromAttrHandle(con, con->svr, OCI_HTYPE_SERVER, 
                                          OCI_ATTR_DBDOMAIN, &con->domain_name);
    }

#endif

    OCI_RESULT(res);

    return con->domain_name;
} 

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetInstanceStartTime
 * --------------------------------------------------------------------------------------------- */

OCI_Timestamp * OCI_API OCI_GetInstanceStartTime
(
    OCI_Connection *con
)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, NULL);

#if OCI_VERSION_COMPILE >= OCI_10_2

    if (!con->inst_startup)
    {
        OCIDateTime *handle = NULL;

        OCI_CALL2
        (
            res, con,

            OCIAttrGet((dvoid **) con->svr, (ub4) OCI_HTYPE_SERVER, (dvoid *) &handle,
                       (ub4 *) NULL,  (ub4) OCI_ATTR_INSTSTARTTIME, con->err)

        )

        if (res)
        {
            res = (OCI_TimestampInit(con, &con->inst_startup, handle, OCI_TIMESTAMP) != NULL);
        }
    }

#endif

    OCI_RESULT(res);

    return con->inst_startup;
} 

/* --------------------------------------------------------------------------------------------- *
 * OCI_IsTAFCapable
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_IsTAFCapable
(
    OCI_Connection *con
)
{
    boolean res = TRUE;
    boolean ret = FALSE;

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, FALSE);
    
#if OCI_VERSION_COMPILE >= OCI_10_2

    if (OCILib.version_runtime >= OCI_10_2)
    {
        OCI_CALL2
        (
            res, con,

            OCIAttrGet((dvoid **) con->svr, (ub4) OCI_HTYPE_SERVER, (dvoid *) &ret,
                       (ub4 *) NULL,  (ub4) OCI_ATTR_TAF_ENABLED, con->err)

        )
    }

#endif

    OCI_RESULT(res);

    return ret;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_SetTAFHandler
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_SetTAFHandler
(
    OCI_Connection  *con,
    POCI_TAF_HANDLER handler
)
{
    boolean res = TRUE;
    boolean ret = OCI_IsTAFCapable(con);
    
#if OCI_VERSION_COMPILE >= OCI_10_2

    if (OCILib.version_runtime >= OCI_10_2)
    {
        if (ret)
        {
            OCIFocbkStruct fo_struct;
        
            memset(&fo_struct, 0, sizeof(fo_struct));

            con->taf_handler = handler;

            if (con->taf_handler != NULL)
            {
                fo_struct.callback_function = (OCICallbackFailover) OCI_ProcFailOver;
                fo_struct.fo_ctx            = (dvoid *) con;
            }

            OCI_CALL2
            (
                res, con,

                OCIAttrSet((dvoid *) con->svr, (ub4) OCI_HTYPE_SERVER, (dvoid *) &fo_struct, (ub4) 0,
                           (ub4) OCI_ATTR_FOCBK, con->err)
            )
        }
    }
    
#endif
    
    OCI_RESULT(res);

    return (ret);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetStatementCacheSize
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_GetStatementCacheSize
(
    OCI_Connection  *con
)
{
    boolean res        = TRUE;
    ub4     cache_size = 0;

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, 0);
    
#if OCI_VERSION_COMPILE >= OCI_9_2

    if (OCILib.version_runtime >= OCI_9_2)
    {

        OCI_CALL2
        (
            res, con,

            OCIAttrGet((dvoid **) con->cxt, (ub4) OCI_HTYPE_SVCCTX, (dvoid *) &cache_size,
                        (ub4 *) NULL,  (ub4) OCI_ATTR_STMTCACHESIZE, con->err)
        )
    }

#endif

    OCI_RESULT(res);

    return cache_size;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_SetStatementCacheSize
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_SetStatementCacheSize
(
    OCI_Connection  *con,
    unsigned int     value
)
{
    boolean res        = TRUE;
    ub4     cache_size = value;

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, FALSE);
    
#if OCI_VERSION_COMPILE >= OCI_9_2

    if (OCILib.version_runtime >= OCI_9_2)
    {
        OCI_CALL2
        (
            res, con,

            OCIAttrSet((dvoid *) con->cxt, (ub4) OCI_HTYPE_SVCCTX,
                        (dvoid *) &cache_size, (ub4) sizeof (cache_size),
                        (ub4) OCI_ATTR_STMTCACHESIZE, con->err)
        )
    }

#else

    OCI_NOT_USED(cache_size);

#endif

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetDefaultLobPrefetchSize
 * --------------------------------------------------------------------------------------------- */

OCI_EXPORT unsigned int OCI_API OCI_GetDefaultLobPrefetchSize
(
    OCI_Connection *con
)
{
    boolean res           = TRUE;
    ub4     prefetch_size = 0;

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, 0);
    
#if OCI_VERSION_COMPILE >= OCI_11_1

    if (con->ver_num >= OCI_11_1)
    {

        OCI_CALL2
        (
            res, con,

            OCIAttrGet((dvoid **) con->ses, (ub4) OCI_HTYPE_SESSION, (dvoid *) &prefetch_size,
                        (ub4 *) NULL,  (ub4) OCI_ATTR_DEFAULT_LOBPREFETCH_SIZE, con->err)
        )
    }

#endif

    OCI_RESULT(res);

    return prefetch_size;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_SetDefaultLobPrefetchSize
 * --------------------------------------------------------------------------------------------- */

OCI_EXPORT boolean OCI_API OCI_SetDefaultLobPrefetchSize
(
    OCI_Connection *con,
    unsigned int     value
)
{
    boolean res           = TRUE;
    ub4     prefetch_size = value;

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, FALSE);
    
#if OCI_VERSION_COMPILE >= OCI_11_1

    if (con->ver_num >= OCI_11_1)
    {
        OCI_CALL2
        (
            res, con,

            OCIAttrSet((dvoid *) con->ses, (ub4) OCI_HTYPE_SESSION,
                        (dvoid *) &prefetch_size, (ub4) sizeof (prefetch_size),
                        (ub4) OCI_ATTR_DEFAULT_LOBPREFETCH_SIZE, con->err)
        )
    }

#else

    OCI_NOT_USED(prefetch_size);

#endif

    OCI_RESULT(res);

    return res;
}

