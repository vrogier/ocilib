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

static unsigned int TraceTypeValues[] = { OCI_TRC_IDENTITY, OCI_TRC_MODULE, OCI_TRC_ACTION, OCI_TRC_DETAIL };

/* ********************************************************************************************* *
 *                             PRIVATE FUNCTIONS
 * ********************************************************************************************* */

/* --------------------------------------------------------------------------------------------- *
* OCI_ConnectionCreateInternal
* --------------------------------------------------------------------------------------------- */

OCI_Connection * OCI_ConnectionCreateInternal
(
    OCI_Pool    *pool,
    const otext *db,
    const otext *user,
    const otext *pwd,
    unsigned int mode,
    const otext *tag
)
{
    /* create connection */

    OCI_Connection *con = OCI_ConnectionAllocate(pool, db, user, pwd, mode);

    if (con)
    {
        if (!OCI_ConnectionAttach(con) || !OCI_ConnectionLogon(con, NULL, tag))
        {
            OCI_ConnectionFree(con);
            con = NULL;
        }
    }

    return con;
}

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
    OCI_Connection *con  = NULL;
    OCI_Item       *item = NULL;
    boolean         res  = TRUE;

    /* create connection object */

    item = OCI_ListAppend(OCILib.cons, sizeof(*con));

    if (item)
    {
        con = (OCI_Connection *) item->data;

        con->alloc_handles = (0 == (con->mode & OCI_SESSION_XA));

        /* create internal lists */

        con->stmts = OCI_ListCreate(OCI_IPC_STATEMENT);

        if (res)
        {
            con->tinfs = OCI_ListCreate(OCI_IPC_TYPE_INFO);
            res        = (NULL != con->tinfs);
        }

        if (res)
        {
            con->trsns = OCI_ListCreate(OCI_IPC_TRANSACTION);
            res        = (NULL != con->trsns);
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

            res = (NULL != con->env);
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

    if (res && con)
    {
        con->cstate = OCI_CONN_ALLOCATED;
    }
    else if (con)
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
    OCI_CHECK(NULL == con, FALSE)
    OCI_CHECK(con->cstate != OCI_CONN_ALLOCATED, FALSE)

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

    OCI_CHECK(NULL == con, FALSE)
    OCI_CHECK(con->cstate != OCI_CONN_ALLOCATED, FALSE)

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

    OCI_CHECK(NULL == con, FALSE)
    OCI_CHECK(con->cstate != OCI_CONN_ATTACHED, FALSE)

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
    boolean res    = TRUE;
    dbtext *dbstr  = NULL;
    int     dbsize = -1;

    OCI_CHECK(NULL == con, FALSE)

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

        res = (NULL != con->cxt);

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

        /* modify user password if needed */

        if (new_pwd && new_pwd[0])
        {
            int dbsize1 = -1;
            int dbsize2 = -1;
            int dbsize3 = -1;

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
                OCI_FREE(con->pwd)

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
            ub4      mode       = OCI_DEFAULT;
            boolean  found      = FALSE;
            dbtext  *dbstr_tag  = NULL;
            int      dbsize_tag = 0;
            OraText *dbstr_ret  = NULL;
            ub4      dbsize_ret = 0;

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

    OCI_CHECK(NULL == con, FALSE)
    OCI_CHECK(con->cstate != OCI_CONN_LOGGED, FALSE)

    /* close opened files */

    if (con->nb_files > 0)
    {
        OCILobFileCloseAll(con->cxt, con->err);
    }

    /* dissociate connection from existing subscriptions */

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
    OCI_Error   *err = NULL;
    unsigned int i   = 0;

    OCI_CHECK(NULL == con, FALSE)

    /* clear connection reference from current error object */

    err = OCI_ErrorGet(FALSE);

    if (err && err->con == con)
    {
        err->con = NULL;
    }

    /* clear server output resources */

    OCI_ServerDisableOutput(con);

    /* log off and detach form server */

    OCI_ConnectionLogOff(con);
    OCI_ConnectionDetach(con);
    OCI_ConnectionDeallocate(con);

    /* free internal lists */

    OCI_ListFree(con->stmts);
    OCI_ListFree(con->trsns);
    OCI_ListFree(con->tinfs);

    /* free strings */

    for (i = 0; i < OCI_FMT_COUNT; i++)
    {
        OCI_FREE(con->formats[i])
    }

    OCI_FREE(con->ver_str)
    OCI_FREE(con->sess_tag)
    OCI_FREE(con->db_name)
    OCI_FREE(con->inst_name)
    OCI_FREE(con->service_name)
    OCI_FREE(con->server_name)
    OCI_FREE(con->db_name)
    OCI_FREE(con->domain_name)

    if (!con->pool)
    {
        OCI_FREE(con->db)
        OCI_FREE(con->user)
        OCI_FREE(con->pwd)
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
    OCI_LIB_CALL_ENTER(OCI_Connection *, NULL)

    /* let's be sure OCI_Initialize() has been called */

    OCI_CHECK_INITIALIZED()

    /* check for XA connections support */

    OCI_CHECK_XA_ENABLED(mode)

    call_retval = OCI_ConnectionCreateInternal(NULL, db, user, pwd, mode, NULL);
    call_status = (NULL != call_retval);

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ConnectionFree
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ConnectionFree
(
    OCI_Connection *con
)
{
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con)

    call_retval = call_status = OCI_ConnectionClose(con);

    OCI_ListRemove(OCILib.cons, con);
    OCI_FREE(con)

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_Commit
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_Commit
(
    OCI_Connection *con
)
{
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con)

    call_status = TRUE;

    OCI_CALL2
    (
        call_status, con,

        OCITransCommit(con->cxt, con->err, (ub4)OCI_DEFAULT)
    )

        call_retval = call_status;

    OCI_LIB_CALL_EXIT()
 }

/* --------------------------------------------------------------------------------------------- *
 * OCI_Rollback
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_Rollback
(
    OCI_Connection *con
)
{
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con)

    call_status = TRUE;

    OCI_CALL2
    (
        call_status, con,

        OCITransRollback(con->cxt, con->err, (ub4) OCI_DEFAULT)
    )

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
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
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con)

    con->autocom = enable;

    call_retval = call_status = TRUE;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetAutoCommit
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_GetAutoCommit
(
    OCI_Connection *con
)
{
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con)

    call_retval = con->autocom;
    call_status = TRUE;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_IsConnected
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_IsConnected
(
    OCI_Connection *con
)
{
    ub4 status  = 0;
    ub4 size    = (ub4) sizeof(status);

    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con)

    call_status = TRUE;

    OCI_CALL2
    (
        call_status, con,
        OCIAttrGet((dvoid **) con->svr, (ub4) OCI_HTYPE_SERVER, (dvoid *) &status,
                   (ub4 *) &size, (ub4) OCI_ATTR_SERVER_STATUS, con->err)
    )

    call_retval = (status == OCI_SERVER_NORMAL);

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetUserData
 * --------------------------------------------------------------------------------------------- */

void * OCI_API OCI_GetUserData
(
    OCI_Connection *con
)
{
    OCI_LIB_CALL_ENTER(void *, NULL)

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con)

    call_retval = con->usrdata;
    call_status = TRUE;

    OCI_LIB_CALL_EXIT()
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
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con)

    con->usrdata = data;

    call_retval = call_status = TRUE;

    OCI_LIB_CALL_EXIT()
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
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con)

    OCI_FREE(con->sess_tag)

    call_status = TRUE;

#if OCI_VERSION_COMPILE >= OCI_9_2

    if (tag && con->pool && ((ub4) OCI_HTYPE_SPOOL == con->pool->htype))
    {
        con->sess_tag = ostrdup(tag);

        call_status = (NULL != con->sess_tag);
    }

#else

    OCI_NOT_USED(tag)

#endif

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetSessionTag
 * --------------------------------------------------------------------------------------------- */

const otext * OCI_API OCI_GetSessionTag
(
    OCI_Connection *con
)
{
    OCI_LIB_CALL_ENTER(const otext *, NULL)

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con)

    call_retval = (const otext *) con->sess_tag;
    call_status = TRUE;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetDatabase
 * --------------------------------------------------------------------------------------------- */

const otext * OCI_API OCI_GetDatabase
(
    OCI_Connection *con
)
{
    OCI_LIB_CALL_ENTER(const otext *, NULL)

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con)

    call_retval = (const otext*) con->db;
    call_status = TRUE;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetUserName
 * --------------------------------------------------------------------------------------------- */

const otext * OCI_API OCI_GetUserName
(
    OCI_Connection *con
)
{
    OCI_LIB_CALL_ENTER(const otext *, NULL)

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con)

    call_retval = (const otext*) con->user;
    call_status = TRUE;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetPassword
 * --------------------------------------------------------------------------------------------- */

const otext * OCI_API OCI_GetPassword
(
    OCI_Connection *con
)
{
    OCI_LIB_CALL_ENTER(const otext *, NULL)

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con)

    call_retval = (const otext*) con->pwd;
    call_status = TRUE;

    OCI_LIB_CALL_EXIT()
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
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con)
    OCI_CHECK_PTR(OCI_IPC_STRING, password)

    call_status = TRUE;

    if (OCI_CONN_LOGGED != con->cstate)
    {
        OCI_ConnectionLogon(con, password, NULL);
    }
    else
    {
        int dbsize1 = -1;
        int dbsize2 = -1;
        int dbsize3 = -1;

        dbtext *dbstr1 = OCI_StringGetOracleString(con->user, &dbsize1);
        dbtext *dbstr2 = OCI_StringGetOracleString(con->pwd, &dbsize2);
        dbtext *dbstr3 = OCI_StringGetOracleString(password, &dbsize3);

        OCI_CALL2
        (
            call_status, con,

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

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
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

    OCI_LIB_CALL_ENTER(boolean, FALSE)

    /* let's be sure OCI_Initialize() has been called */

    OCI_CHECK_INITIALIZED();
    OCI_CHECK_PTR(OCI_IPC_STRING, pwd)
    OCI_CHECK_PTR(OCI_IPC_STRING, new_pwd)

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
        call_status = TRUE;
        OCI_ConnectionFree(con);
    }

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetSessionMode
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_GetSessionMode
(
    OCI_Connection *con
)
{
    OCI_LIB_CALL_ENTER(unsigned int, OCI_UNKNOWN)

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con)

    call_retval = con->mode;
    call_status = TRUE;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetVersionServer
 * --------------------------------------------------------------------------------------------- */

const otext * OCI_API OCI_GetVersionServer
(
    OCI_Connection *con
)
{
    OCI_LIB_CALL_ENTER(const otext *, NULL)

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con)

    call_status = TRUE;

    /* no version available in preliminary authentication mode */

    if (!con->ver_str && (!(con->mode & OCI_PRELIM_AUTH)))
    {
        int     dbsize = OCI_SIZE_BUFFER * (int) sizeof(dbtext);
        dbtext *dbstr  = NULL;

        con->ver_str = (otext *) OCI_MemAlloc(OCI_IPC_STRING, sizeof(otext), OCI_SIZE_BUFFER + 1, TRUE);

        dbstr = OCI_StringGetOracleString(con->ver_str, &dbsize);

        OCI_CALL2
        (
            call_status, con,

            OCIServerVersion((dvoid *) con->cxt, con->err, (OraText *) dbstr,
                                (ub4) dbsize, (ub1) OCI_HTYPE_SVCCTX)
        )

        OCI_StringCopyOracleStringToNativeString(dbstr, con->ver_str, dbcharcount(dbsize));
        OCI_StringReleaseOracleString(dbstr);

        if (call_status)
        {
            otext *p = NULL;

            int ver_maj = 0, ver_min = 0, ver_rev = 0;

            con->ver_str[ocharcount(dbsize)] = 0;

            /* parse server version string to find the version information
                **/

            for (p = con->ver_str; p && *p; p++)
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
            OCI_FREE(con->ver_str)
        }
    }

    call_retval = (const otext*)con->ver_str;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetServerMajorVersion
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_GetServerMajorVersion
(
    OCI_Connection *con
)
{
    OCI_LIB_CALL_ENTER(unsigned int, OCI_UNKNOWN)

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con)

    if (OCI_UNKNOWN == con->ver_num)
    {
        OCI_GetVersionServer(con);
    }

    call_retval = (unsigned int) OCI_VER_MAJ(con->ver_num);
    call_status = OCI_UNKNOWN != con->ver_num;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetServerMinorVersion
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_GetServerMinorVersion
(
    OCI_Connection *con
)
{
    OCI_LIB_CALL_ENTER(unsigned int, OCI_UNKNOWN)

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con)

    if (OCI_UNKNOWN == con->ver_num)
    {
        OCI_GetVersionServer(con);
    }

    call_retval = (unsigned int) OCI_VER_MIN(con->ver_num);
    call_status = OCI_UNKNOWN != con->ver_num;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetServerRevisionVersion
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_GetServerRevisionVersion
(
    OCI_Connection *con
)
{
    OCI_LIB_CALL_ENTER(unsigned int, OCI_UNKNOWN)

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con)

    if (OCI_UNKNOWN == con->ver_num)
    {
        OCI_GetVersionServer(con);
    }

    call_retval = (unsigned int) OCI_VER_REV(con->ver_num);
    call_status = OCI_UNKNOWN != con->ver_num;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetTransaction
 * --------------------------------------------------------------------------------------------- */

OCI_Transaction * OCI_API OCI_GetTransaction
(
    OCI_Connection *con
)
{
    OCI_LIB_CALL_ENTER(OCI_Transaction*, NULL)

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con)

    call_retval = con->trs;
    call_status = TRUE;

    OCI_LIB_CALL_EXIT()
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
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con)
    OCI_CHECK_PTR(OCI_IPC_TRANSACTION, trans)

    call_status = TRUE;

    if (con->trs)
    {
        call_status = OCI_TransactionStop(con->trs);
    }

    if (call_status)
    {
        con->trs = trans;

        /* set context transaction attribute */

        OCI_CALL2
        (
            call_status, con,

            OCIAttrSet((dvoid *) con->cxt, (ub4) OCI_HTYPE_SVCCTX,
                       (dvoid *) con->trs->htr, (ub4) sizeof(con->trs->htr),
                       (ub4) OCI_ATTR_TRANS, con->err)
        )
    }

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetVersionConnection
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_GetVersionConnection
(
    OCI_Connection *con
)
{
    OCI_LIB_CALL_ENTER(unsigned int, OCI_UNKNOWN)

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con)

    /* return the minimum supported version */

    call_retval =  (OCILib.version_runtime > con->ver_num) ? con->ver_num : OCILib.version_runtime;
    call_status = TRUE;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_Break
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_Break
(
    OCI_Connection *con
)
{
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con)

    call_status = TRUE;

    OCI_CALL2
    (
        call_status, con,

        OCIBreak((dvoid *) con->cxt, con->err)
    )

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
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
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con)

    call_status = TRUE;

    /* initialize the output buffer on server side */

    if (!con->svopt)
    {
        con->svopt = (OCI_ServerOutput *) OCI_MemAlloc(OCI_IPC_SERVER_OUPUT, sizeof(*con->svopt),
                                                       (size_t) 1, TRUE);

        call_status = (NULL != con->svopt);
    }

    /* allocation internal buffer if needed */

    if (call_status && !con->svopt->arrbuf)
    {
        unsigned int charsize = sizeof(otext);

        /* check parameter ranges ( Oracle 10g increased the size of output line */

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

        call_status = (NULL != con->svopt->arrbuf);
    }

    if (call_status)
    {
        if (!con->svopt->stmt)
        {
            con->svopt->stmt = OCI_StatementCreate(con);
        }

        if (con->svopt->stmt)
        {
            /* enable server output */

            call_status = OCI_Prepare(con->svopt->stmt, OTEXT("BEGIN DBMS_OUTPUT.ENABLE(:n); END;"));

            call_status = call_status && OCI_BindUnsignedInt(con->svopt->stmt, OTEXT(":n"), &bufsize);

            if (0 == bufsize)
            {
                call_status = OCI_BindSetNull(OCI_GetBind(con->svopt->stmt, 1));
            }

            call_status = call_status && OCI_Execute(con->svopt->stmt);

            /* prepare the retrieval statement call */

            con->svopt->cursize = con->svopt->arrsize;

            call_status = call_status && OCI_Prepare(con->svopt->stmt, OTEXT("BEGIN DBMS_OUTPUT.GET_LINES(:s, :i); END;"));

            call_status = call_status && OCI_BindArrayOfStrings(con->svopt->stmt, OTEXT(":s"),
                                                (otext *) con->svopt->arrbuf,
                                                con->svopt->lnsize,
                                                con->svopt->arrsize);

            call_status = call_status && OCI_BindUnsignedInt(con->svopt->stmt, OTEXT(":i"), &con->svopt->cursize);
        }
    }

    if (!call_status)
    {
        OCI_ServerDisableOutput(con);
    }

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ServerDisableOutput
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ServerDisableOutput
(
    OCI_Connection *con
)
{
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con)

    call_status = TRUE;

    if (con->svopt)
    {
        call_status = call_status && OCI_ExecuteStmt(con->svopt->stmt, OTEXT("BEGIN DBMS_OUTPUT.DISABLE(); END;"));

        if (con->svopt->stmt)
        {
            OCI_StatementFree(con->svopt->stmt);
            con->svopt->stmt = NULL;
        }

        OCI_FREE(con->svopt->arrbuf)
        OCI_FREE(con->svopt)
    }

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ServerGetOutput
 * --------------------------------------------------------------------------------------------- */

const otext * OCI_API OCI_ServerGetOutput
(
    OCI_Connection *con
)
{
    OCI_LIB_CALL_ENTER(const otext *, NULL)

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con)

    call_status = TRUE;

    if (con->svopt)
    {
        if (0 == con->svopt->curpos || con->svopt->curpos >= con->svopt->cursize)
        {
            con->svopt->cursize = con->svopt->arrsize;
            call_status = OCI_Execute(con->svopt->stmt);
            con->svopt->curpos = 0;
        }

        if (call_status && (con->svopt->cursize > 0))
        {
            unsigned int charsize = sizeof(otext);

            call_retval = (const otext*)(con->svopt->arrbuf + (size_t)(((con->svopt->lnsize + 1) * charsize) * con->svopt->curpos++));
        }
    }

    OCI_LIB_CALL_EXIT()
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
    const otext *str = NULL;

#if OCI_VERSION_COMPILE >= OCI_10_1

    ub4 attrib = 0;

#endif

    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con)
    OCI_CHECK_ENUM_VALUE(con, NULL, trace, TraceTypeValues, OTEXT("Trace Type"))

    call_status = TRUE;

    /* allocate trace info structure only if trace functions are used */

    if (!con->trace)
    {
        con->trace  = (OCI_TraceInfo *) OCI_MemAlloc(OCI_IPC_TRACE_INFO, sizeof(*con->trace), (size_t) 1, TRUE);
        call_status = (NULL != con->trace);
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
        }
    }

#if OCI_VERSION_COMPILE >= OCI_10_1

    /* On success, we give the value to Oracle to record it in system session view */

    if (call_status)
    {
        dbtext *dbstr  = NULL;
        int     dbsize = 0;

        if (str)
        {
            dbsize = -1;
            dbstr  = OCI_StringGetOracleString(str, &dbsize);
        }

        OCI_CALL2
        (
            call_status, con,

            OCIAttrSet((dvoid *) con->ses, (ub4) OCI_HTYPE_SESSION,
                       (dvoid *) dbstr, (ub4) dbsize, attrib, con->err)
        )

        OCI_StringReleaseOracleString(dbstr);
    }

#endif

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
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
    OCI_LIB_CALL_ENTER(const otext *, NULL)

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con)
    OCI_CHECK_ENUM_VALUE(con, NULL, trace, TraceTypeValues, OTEXT("Trace Type"))

    call_status = TRUE;

    if (con->trace)
    {
        switch (trace)
        {
            case OCI_TRC_IDENTITY:
            {
                call_retval = con->trace->identifier;
                break;
            }
            case OCI_TRC_MODULE:
            {
                call_retval = con->trace->module;
                break;
            }
            case OCI_TRC_ACTION:
            {
                call_retval = con->trace->action;
                break;
            }
            case OCI_TRC_DETAIL:
            {
                call_retval = con->trace->info;
                break;
            }
        }
    }

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_Ping
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_Ping
(
    OCI_Connection *con
)
{
    OCI_LIB_CALL_ENTER(boolean , FALSE)

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con)

    call_status = TRUE;

#if OCI_VERSION_COMPILE >= OCI_10_2

    if (OCILib.version_runtime >= OCI_10_2)
    {
        OCI_CALL2
        (
            call_status, con,

            OCIPing(con->cxt, con->err, (ub4) OCI_DEFAULT)

        )

        call_retval = call_status;
    }

#endif

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetDBName
 * --------------------------------------------------------------------------------------------- */

const otext * OCI_API OCI_GetDBName
(
    OCI_Connection *con
)
{
    OCI_LIB_CALL_ENTER(const otext *, NULL)

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con)

    call_status = TRUE;

#if OCI_VERSION_COMPILE >= OCI_10_2

    if (!con->db_name)
    {
        call_status = OCI_GetStringAttribute(con, con->svr, OCI_HTYPE_SERVER,
                                             OCI_ATTR_DBNAME, &con->db_name);
    }

#endif

    call_retval = (const otext *) con->db_name;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetInstanceName
 * --------------------------------------------------------------------------------------------- */

const otext * OCI_API OCI_GetInstanceName
(
    OCI_Connection *con
)
{
    OCI_LIB_CALL_ENTER(const otext *, NULL)

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con)

    call_status = TRUE;

#if OCI_VERSION_COMPILE >= OCI_10_2

    if (!con->inst_name)
    {
        call_status = OCI_GetStringAttribute(con, con->svr, OCI_HTYPE_SERVER,
                                             OCI_ATTR_INSTNAME, &con->inst_name);
    }

#endif

    call_retval = (const otext *)con->inst_name;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetServiceName
 * --------------------------------------------------------------------------------------------- */

const otext * OCI_API OCI_GetServiceName
(
    OCI_Connection *con
)
{
    OCI_LIB_CALL_ENTER(const otext *, NULL)

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con)

    call_status = TRUE;

#if OCI_VERSION_COMPILE >= OCI_10_2

    if (!con->service_name)
    {
        call_status = OCI_GetStringAttribute(con, con->svr, OCI_HTYPE_SERVER,
                                             OCI_ATTR_SERVICENAME, &con->service_name);
    }

#endif

    call_retval = (const otext *)con->service_name;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetServerName
 * --------------------------------------------------------------------------------------------- */

const otext * OCI_API OCI_GetServerName
(
    OCI_Connection *con
)
{
    OCI_LIB_CALL_ENTER(const otext *, NULL)

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con)

    call_status = TRUE;

#if OCI_VERSION_COMPILE >= OCI_10_2

    if (!con->server_name)
    {
        call_status = OCI_GetStringAttribute(con, con->svr, OCI_HTYPE_SERVER,
                                             OCI_ATTR_HOSTNAME, &con->server_name);
    }

#endif

    call_retval = (const otext *)con->server_name;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetDomainName
 * --------------------------------------------------------------------------------------------- */

const otext * OCI_API OCI_GetDomainName
(
    OCI_Connection *con
)
{
    OCI_LIB_CALL_ENTER(const otext *, NULL)

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con)

    call_status = TRUE;

#if OCI_VERSION_COMPILE >= OCI_10_2

    if (!con->domain_name)
    {
        call_status = OCI_GetStringAttribute(con, con->svr, OCI_HTYPE_SERVER,
                                             OCI_ATTR_DBDOMAIN, &con->domain_name);
    }

#endif

    call_retval = (const otext *)con->domain_name;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetInstanceStartTime
 * --------------------------------------------------------------------------------------------- */

OCI_Timestamp * OCI_API OCI_GetInstanceStartTime
(
    OCI_Connection *con
)
{
    OCI_LIB_CALL_ENTER(OCI_Timestamp*, NULL)

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con)

    call_status = TRUE;

#if OCI_VERSION_COMPILE >= OCI_10_2

    if (!con->inst_startup)
    {
        OCIDateTime *handle = NULL;

        OCI_CALL2
        (
            call_status, con,

            OCIAttrGet((dvoid **) con->svr, (ub4) OCI_HTYPE_SERVER, (dvoid *) &handle,
                       (ub4 *) NULL,  (ub4) OCI_ATTR_INSTSTARTTIME, con->err)

        )

        if (call_status)
        {
            call_status = (NULL != OCI_TimestampInit(con, &con->inst_startup, handle, OCI_TIMESTAMP));
        }
    }

#endif

    call_retval = con->inst_startup;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_IsTAFCapable
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_IsTAFCapable
(
    OCI_Connection *con
)
{
    boolean value = FALSE;

    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con)

    call_status = TRUE;

#if OCI_VERSION_COMPILE >= OCI_10_2

    if (OCILib.version_runtime >= OCI_10_2)
    {
        OCI_CALL2
        (
            call_status, con,

            OCIAttrGet((dvoid **)con->svr, (ub4)OCI_HTYPE_SERVER, (dvoid *)&value,
                       (ub4 *) NULL,  (ub4) OCI_ATTR_TAF_ENABLED, con->err)

        )
    }

#endif

    call_retval = value;

    OCI_LIB_CALL_EXIT()
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
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con)

    call_status = TRUE;
    call_retval = OCI_IsTAFCapable(con);

#if OCI_VERSION_COMPILE >= OCI_10_2

    if (OCILib.version_runtime >= OCI_10_2)
    {
        if (call_retval)
        {
            OCIFocbkStruct fo_struct;

            memset(&fo_struct, 0, sizeof(fo_struct));

            con->taf_handler = handler;

            if (con->taf_handler)
            {
                fo_struct.callback_function = (OCICallbackFailover) OCI_ProcFailOver;
                fo_struct.fo_ctx            = (dvoid *) con;
            }

            OCI_CALL2
            (
                call_status, con,

                OCIAttrSet((dvoid *) con->svr, (ub4) OCI_HTYPE_SERVER, (dvoid *) &fo_struct, (ub4) 0,
                           (ub4) OCI_ATTR_FOCBK, con->err)
            )
        }
    }

#endif

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetStatementCacheSize
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_GetStatementCacheSize
(
    OCI_Connection  *con
)
{
    ub4 cache_size = 0;

    OCI_LIB_CALL_ENTER(unsigned int, 0)

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con)

    call_status = TRUE;

#if OCI_VERSION_COMPILE >= OCI_9_2

    if (OCILib.version_runtime >= OCI_9_2)
    {
        OCI_CALL2
        (
            call_status, con,

            OCIAttrGet((dvoid **) con->cxt, (ub4) OCI_HTYPE_SVCCTX, (dvoid *) &cache_size,
                        (ub4 *) NULL,  (ub4) OCI_ATTR_STMTCACHESIZE, con->err)
        )
    }

#endif

    call_retval = cache_size;

    OCI_LIB_CALL_EXIT()
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
    ub4 cache_size = value;

    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con)

    call_status = TRUE;

#if OCI_VERSION_COMPILE >= OCI_9_2

    if (OCILib.version_runtime >= OCI_9_2)
    {
        OCI_CALL2
        (
            call_status, con,

            OCIAttrSet((dvoid *) con->cxt, (ub4) OCI_HTYPE_SVCCTX,
                        (dvoid *) &cache_size, (ub4) sizeof (cache_size),
                        (ub4) OCI_ATTR_STMTCACHESIZE, con->err)
        )
    }

#else

    OCI_NOT_USED(cache_size)

#endif

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetDefaultLobPrefetchSize
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_GetDefaultLobPrefetchSize
(
    OCI_Connection *con
)
{
    ub4 prefetch_size = 0;

    OCI_LIB_CALL_ENTER(unsigned int, 0)

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con)

    call_status = TRUE;

#if OCI_VERSION_COMPILE >= OCI_11_1

    if (con->ver_num >= OCI_11_1)
    {

        OCI_CALL2
        (
            call_status, con,

            OCIAttrGet((dvoid **) con->ses, (ub4) OCI_HTYPE_SESSION, (dvoid *) &prefetch_size,
                        (ub4 *) NULL,  (ub4) OCI_ATTR_DEFAULT_LOBPREFETCH_SIZE, con->err)
        )
    }

#endif

    call_retval = prefetch_size;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_SetDefaultLobPrefetchSize
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_SetDefaultLobPrefetchSize
(
    OCI_Connection *con,
    unsigned int     value
)
{
    ub4 prefetch_size = value;

    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con)

    call_status = TRUE;

#if OCI_VERSION_COMPILE >= OCI_11_1

    if (con->ver_num >= OCI_11_1)
    {
        OCI_CALL2
        (
            call_status, con,

            OCIAttrSet((dvoid *) con->ses, (ub4) OCI_HTYPE_SESSION,
                        (dvoid *) &prefetch_size, (ub4) sizeof (prefetch_size),
                        (ub4) OCI_ATTR_DEFAULT_LOBPREFETCH_SIZE, con->err)
        )
    }

#else

    OCI_NOT_USED(prefetch_size)

#endif

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetMaxCursors
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_GetMaxCursors
(
    OCI_Connection *con
)
{
    ub4 max_cursors = 0;

    OCI_LIB_CALL_ENTER(unsigned int, 0)

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con)

    call_status = TRUE;

#if OCI_VERSION_COMPILE >= OCI_12_1

    if (con->ver_num >= OCI_12_1)
    {

        OCI_CALL2
        (
            call_status, con,

            OCIAttrGet((dvoid **) con->ses, (ub4) OCI_HTYPE_SESSION, (dvoid *) &max_cursors,
                        (ub4 *) NULL,  (ub4) OCI_ATTR_MAX_OPEN_CURSORS, con->err)
        )
    }

#endif

    call_retval = max_cursors;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_Immediate
 * --------------------------------------------------------------------------------------------- */

boolean OCI_Immediate
(
    OCI_Connection *con,
    const otext    *sql,
    ...
)
{
    OCI_Statement *stmt = NULL;
    va_list args;

    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con)
    OCI_CHECK_PTR(OCI_IPC_STRING, sql)

    /* First, execute SQL */

    stmt = OCI_StatementCreate(con);

    if (stmt)
    {
        call_status = OCI_ExecuteStmt(stmt, sql);

        if (call_status)
        {
            /* get resultset and set up variables */

            if (OCI_GetStatementType(stmt) == OCI_CST_SELECT)
            {
                va_start(args, sql);

                call_status = OCI_FetchIntoUserVariables(stmt, args);

                va_end(args);
            }
        }

        OCI_StatementFree(stmt);
    }

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ImmediateFmt
 * --------------------------------------------------------------------------------------------- */

boolean OCI_ImmediateFmt
(
    OCI_Connection *con,
    const otext    *sql,
    ...
)
{
    OCI_Statement  *stmt = NULL;

    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con)
    OCI_CHECK_PTR(OCI_IPC_STRING, sql)

    stmt = OCI_StatementCreate(con);

    if (stmt)
    {
        int     size = 0;
        va_list args;

        /* first, get buffer size */

        va_start(args, sql);

        size = OCI_ParseSqlFmt(stmt, NULL, sql, &args);

        va_end(args);

        if (size > 0)
        {
            /* allocate buffer */

            otext  *sql_fmt = (otext *) OCI_MemAlloc(OCI_IPC_STRING, sizeof(otext), (size_t) (size+1), TRUE);

            if (sql_fmt)
            {
                /* format buffer */

                va_start(args, sql);

                if (OCI_ParseSqlFmt(stmt, sql_fmt, sql, &args) > 0)
                {
                    /* prepare and execute SQL buffer */

                    call_status = (OCI_PrepareInternal(stmt, sql_fmt) && OCI_ExecuteInternal(stmt, OCI_DEFAULT));

                    /* get resultset and set up variables */

                    if (call_status && (OCI_CST_SELECT == OCI_GetStatementType(stmt)))
                    {
                        call_status = OCI_FetchIntoUserVariables(stmt, args);
                    }
                }

                va_end(args);

                OCI_FREE(sql_fmt)
            }
        }

        OCI_StatementFree(stmt);
    }

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
}

