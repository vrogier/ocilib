/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2018 Vincent ROGIER <vince.rogier@ocilib.net>
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

static const unsigned int TraceTypeValues[] = 
{ 
    OCI_TRC_IDENTITY, 
    OCI_TRC_MODULE,
    OCI_TRC_ACTION, 
    OCI_TRC_DETAIL, 
    OCI_TRC_OPERATION 
};

/* ********************************************************************************************* *
 *                             PRIVATE FUNCTIONS
 * ********************************************************************************************* */

#define OCI_SET_TRACE(prop)                                              \
    con->trace->prop[0] = 0;                                             \
    if (value)                                                           \
    {                                                                    \
        ostrncat(con->trace->prop, value, osizeof(con->trace->prop)-1);  \
        str = con->trace->prop;                                          \
    }

#define OCI_GET_TRACE(prop)                                              \
    OCI_RETVAL = con->trace->prop[0] ? con->trace->prop : NULL;

/* --------------------------------------------------------------------------------------------- *
 * OCI_ConnectionDetachSubscriptions
 * --------------------------------------------------------------------------------------------- */

void OCI_ConnectionDetachSubscriptions(OCI_Subscription *sub, OCI_Connection *con)
{
    if (sub && (sub->con == con))
    {
        sub->con = NULL;

        sub->saved_db   = ostrdup(con->db);
        sub->saved_user = ostrdup(con->user);
        sub->saved_pwd  = ostrdup(con->pwd);
    }
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

    OCI_CALL_DECLARE_CONTEXT(TRUE)

    /* create connection object */

    con = OCI_ListAppend(OCILib.cons, sizeof(*con));
    OCI_STATUS = (NULL != con);

    if (OCI_STATUS)
    {
        con->alloc_handles = (0 == (mode & OCI_SESSION_XA));

        /* create internal lists */

        con->stmts = OCI_ListCreate(OCI_IPC_STATEMENT);

        if (OCI_STATUS)
        {
            con->tinfs = OCI_ListCreate(OCI_IPC_TYPE_INFO);
            OCI_STATUS = (NULL != con->tinfs);
        }

        if (OCI_STATUS)
        {
            con->trsns = OCI_ListCreate(OCI_IPC_TRANSACTION);
            OCI_STATUS = (NULL != con->trsns);
        }

        /* set attributes */

        if (OCI_STATUS)
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

                if (OCI_STRING_VALID(con->db))
                {
                    OCI_StringNativeToAnsi(con->db, dbname, (int) ostrlen(con->db));
                }

                con->env = xaoEnv((OraText *) (dbname[0] ? dbname : NULL ));

                if (NULL == con->env)
                {
                    OCI_ExceptionEnvFromXaString(con->db);
                }
            }
            else

        #endif

            {
                con->env = OCILib.env;
            }

            OCI_STATUS = (NULL != con->env);
        }

        /*  allocate error handle */

        OCI_STATUS = OCI_STATUS && OCI_HandleAlloc((dvoid *)con->env, (dvoid **)(void *)&con->err, OCI_HTYPE_ERROR);
    }

    /* update internal status */

    if (OCI_STATUS && con)
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
        OCI_HandleFree((dvoid *) con->err, OCI_HTYPE_ERROR);
    }

    /* close server handle (if it had been allocated) in case of login error */

    if ((con->svr) && con->alloc_handles)
    {
        OCI_HandleFree((dvoid *) con->svr, OCI_HTYPE_SERVER);
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
    OCI_CALL_DECLARE_CONTEXT(TRUE)

    OCI_CHECK(NULL == con, FALSE)
    OCI_CHECK(con->cstate != OCI_CONN_ALLOCATED, FALSE)

    OCI_CALL_CONTEXT_SET_FROM_CONN(con);

    /* allocate server handle for non session pooled connection */

    if (con->alloc_handles)
    {
        ub4     cmode  = OCI_DEFAULT;
        dbtext *dbstr  = NULL;
        int     dbsize = -1;

        OCI_STATUS = OCI_HandleAlloc((dvoid *) con->env, (dvoid **) (void *) &con->svr, OCI_HTYPE_SERVER);

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

        OCI_EXEC(OCIServerAttach(con->svr, con->err, (OraText *)dbstr, (sb4)dbsize, cmode));

        OCI_StringReleaseOracleString(dbstr);
    }

    /* handle errors */

    if (OCI_STATUS)
    {
        con->cstate = OCI_CONN_ATTACHED;
    }

    return OCI_STATUS;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ConnectionDetach
 * --------------------------------------------------------------------------------------------- */

boolean OCI_ConnectionDetach
(
    OCI_Connection *con
)
{
    OCI_CALL_DECLARE_CONTEXT(TRUE)

    OCI_CHECK(NULL == con, FALSE)
    OCI_CHECK(con->cstate != OCI_CONN_ATTACHED, FALSE)

    OCI_CALL_CONTEXT_SET_FROM_CONN(con);

    if (con->alloc_handles && con->svr)
    {
        /* detach from the oracle server */

        OCI_EXEC(OCIServerDetach(con->svr, con->err, OCI_DEFAULT));

        /* close server handle */

        OCI_HandleFree((dvoid *) con->svr, OCI_HTYPE_SERVER);

        con->svr = NULL;
    }

    /* update internal status */

    if (OCI_STATUS)
    {
        con->cstate = OCI_CONN_ALLOCATED;
    }

    return OCI_STATUS;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ConnectionLogonXA
 * --------------------------------------------------------------------------------------------- */

void OCI_ConnectionLogonXA
(
    OCI_Context    *ctx,
    OCI_Connection *con
)
{
    dbtext *dbstr_user  = NULL;
    int     dbsize_user = 0;
    char    dbname[OCI_SIZE_BUFFER + 1];

    memset(dbname, 0, sizeof(dbname));

    if (OCI_STRING_VALID(con->db))
    {
        OCI_StringNativeToAnsi(con->db, dbname, (int) ostrlen(con->db));
    }

    con->cxt = xaoSvcCtx((OraText *) (dbname[0] ? dbname : NULL ));
    OCI_STATUS = (NULL != con->cxt);

    OCI_GET_ATTRIB(OCI_HTYPE_SVCCTX, OCI_ATTR_SERVER, con->cxt, &con->svr, NULL)
    OCI_GET_ATTRIB(OCI_HTYPE_SVCCTX, OCI_ATTR_SESSION, con->cxt, &con->ses, NULL)
    OCI_GET_ATTRIB(OCI_HTYPE_SESSION, OCI_ATTR_USERNAME, con->ses, &dbstr_user, &dbsize_user)

    if (NULL == con->ses)
    {
        OCI_STATUS = FALSE;
        OCI_ExceptionConnFromXaString(con->db);
    }

    if (OCI_STATUS && dbstr_user)
    {
        OCI_FREE(con->user)

        con->user = OCI_StringDuplicateFromOracleString(dbstr_user, dbcharcount(dbsize_user));
    }
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ConnectionLogonRegular
 * --------------------------------------------------------------------------------------------- */

void OCI_ConnectionLogonRegular
(
    OCI_Context    *ctx,
    OCI_Connection *con,
    const otext    *new_pwd
)
{
    /* allocate session handle */

    OCI_HandleAlloc((dvoid *)con->env, (dvoid **)(void *)&con->ses, OCI_HTYPE_SESSION);

    /* allocate context handle */

    OCI_HandleAlloc((dvoid *)con->env, (dvoid **)(void *)&con->cxt, OCI_HTYPE_SVCCTX);

    /* set context server attribute */

    OCI_SET_ATTRIB(OCI_HTYPE_SVCCTX, OCI_ATTR_SERVER, con->cxt, con->svr, sizeof(con->svr));

    /* modify user password if needed */

    if (OCI_STRING_VALID(new_pwd))
    {
        int dbsize1 = -1;
        int dbsize2 = -1;
        int dbsize3 = -1;

        dbtext *dbstr1 = NULL;
        dbtext *dbstr2 = NULL;
        dbtext *dbstr3 = NULL;

        dbstr1 = OCI_StringGetOracleString(con->user, &dbsize1);
        dbstr2 = OCI_StringGetOracleString(con->pwd, &dbsize2);
        dbstr3 = OCI_StringGetOracleString(new_pwd, &dbsize3);

        OCI_SET_ATTRIB(OCI_HTYPE_SVCCTX, OCI_ATTR_SESSION, con->cxt, con->ses, sizeof(con->ses));

        OCI_EXEC
        (
            OCIPasswordChange(con->cxt, con->err, (OraText *)dbstr1, (ub4)dbsize1,
                              (OraText *)dbstr2, (ub4)dbsize2,
                              (OraText *)dbstr3, (ub4)dbsize3, OCI_AUTH)
        )

        OCI_StringReleaseOracleString(dbstr1);
        OCI_StringReleaseOracleString(dbstr2);
        OCI_StringReleaseOracleString(dbstr3);

        if (OCI_STATUS)
        {
            OCI_FREE(con->pwd)
            con->pwd = ostrdup(new_pwd);
        }
    }
    else
    {
        int     dbsize = -1;
        dbtext *dbstr  = NULL;

        /* set session login attribute */

        if (OCI_STATUS && OCI_STRING_VALID(con->user))
        {
            dbsize = -1;
            dbstr  = OCI_StringGetOracleString(con->user, &dbsize);

            OCI_SET_ATTRIB(OCI_HTYPE_SESSION, OCI_ATTR_USERNAME, con->ses, dbstr, dbsize);

            OCI_StringReleaseOracleString(dbstr);
        }

        /* set session password attribute */

        if (OCI_STATUS && OCI_STRING_VALID(con->pwd))
        {
            dbsize = -1;
            dbstr  = OCI_StringGetOracleString(con->pwd, &dbsize);

            OCI_SET_ATTRIB(OCI_HTYPE_SESSION, OCI_ATTR_PASSWORD, con->ses, dbstr, dbsize);

            OCI_StringReleaseOracleString(dbstr);
        }

        /* set OCILIB's driver layer name attribute */

#if OCI_VERSION_COMPILE >= OCI_11_1

        if (OCI_STATUS && (OCILib.version_runtime >= OCI_11_1))
        {
            otext driver_version[OCI_SIZE_FORMAT];

            osprintf(driver_version,
                     osizeof(driver_version) - (size_t)1,
                     OTEXT("%s : %d.%d.%d"),
                     OCILIB_DRIVER_NAME,
                     OCILIB_MAJOR_VERSION,
                     OCILIB_MINOR_VERSION,
                     OCILIB_REVISION_VERSION);

            dbsize = -1;
            dbstr = OCI_StringGetOracleString(driver_version, &dbsize);

            OCI_SET_ATTRIB(OCI_HTYPE_SESSION, OCI_ATTR_DRIVER_NAME, con->ses, dbstr, dbsize);

            OCI_StringReleaseOracleString(dbstr);
        }

#endif

        /* start session */

        if (OCI_STATUS)
        {
            ub4 credt = OCI_CRED_RDBMS;
            ub4 mode  = con->mode;

            if (!OCI_STRING_VALID(con->user) && !OCI_STRING_VALID(con->pwd))
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

            OCI_EXEC(OCISessionBegin(con->cxt, con->err, con->ses, credt, mode));

            OCI_SET_ATTRIB(OCI_HTYPE_SVCCTX, OCI_ATTR_SESSION, con->cxt, con->ses, sizeof(con->ses));

            if (OCI_STATUS)
            {
                if (!(con->mode & OCI_PRELIM_AUTH))
                {
                    /* create default transaction object */

                    OCI_Transaction *trs = OCI_TransactionCreate(con, 1, OCI_TRANS_READWRITE, NULL);

                    if (trs && OCI_SetTransaction(con, trs))
                    {
                        /* start transaction */

                        OCI_STATUS = OCI_TransactionStart(trs);
                    }
                }
            }
            else
            {
                /* could not start session, must free the session and context handles */

                OCI_HandleFree((dvoid *)con->ses, OCI_HTYPE_SESSION);
                OCI_HandleFree((dvoid *)con->cxt, OCI_HTYPE_SVCCTX);

                con->ses = NULL;
                con->cxt = NULL;
            }
        }
    }
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ConnectionLogonSessionPool
 * --------------------------------------------------------------------------------------------- */

void OCI_ConnectionLogonSessionPool
(
    OCI_Context    *ctx,
    OCI_Connection *con,
    const otext    *tag
)
{
    ub4      sess_mode  = OCI_SESSGET_SPOOL;
    boolean  found      = FALSE;
    int      dbsize     = -1;
    dbtext  *dbstr      = NULL;
    dbtext  *dbstr_tag  = NULL;
    int      dbsize_tag = 0;
    OraText *dbstr_ret  = NULL;
    ub4      dbsize_ret = 0;

    if (!OCI_STRING_VALID(con->pool->user) && !OCI_STRING_VALID(con->pool->pwd))
    {
        sess_mode |= OCI_SESSGET_CREDEXT;
    }

    if (OCI_STRING_VALID(con->pool->name))
    {
        dbsize = -1;
        dbstr  = OCI_StringGetOracleString(con->pool->name, &dbsize);
    }

    if (OCI_STRING_VALID(tag))
    {
        dbsize_tag = -1;
        dbstr_tag  = OCI_StringGetOracleString(tag, &dbsize_tag);
    }

    OCI_EXEC
    (
        OCISessionGet(con->env, con->err, &con->cxt, NULL,
                      (OraText  *)dbstr, (ub4)dbsize, (OraText *)dbstr_tag, dbsize_tag,
                      (OraText **)&dbstr_ret, &dbsize_ret, &found, sess_mode)
    )

    OCI_StringReleaseOracleString(dbstr);
    OCI_StringReleaseOracleString(dbstr_tag);

    OCI_GET_ATTRIB(OCI_HTYPE_SVCCTX, OCI_ATTR_SERVER, con->cxt, &con->svr, NULL)
    OCI_GET_ATTRIB(OCI_HTYPE_SVCCTX, OCI_ATTR_SESSION, con->cxt, &con->ses, NULL)

    if (OCI_STATUS && found)
    {
        OCI_SetSessionTag(con, tag);
    }
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
    OCI_CALL_DECLARE_CONTEXT(TRUE)

    OCI_CHECK(NULL == con, FALSE)

    OCI_CALL_CONTEXT_SET_FROM_CONN(con);

#if OCI_VERSION_COMPILE < OCI_9_2

    OCI_NOT_USED(tag)

#endif

    /* 1 - XA connection */

#if OCI_VERSION_COMPILE >= OCI_10_1

    if (con->mode & OCI_SESSION_XA)
    {
        OCI_ConnectionLogonXA(ctx, con);
    }
    else

#endif

    /* 2 - regular connection and connection from connection pool */

    if (con->alloc_handles)
    {
        OCI_ConnectionLogonRegular(ctx, con, new_pwd);
    }

#if OCI_VERSION_COMPILE >= OCI_9_2

   /* 3 - connection from session pool */

    else if (OCILib.version_runtime >= OCI_9_2)
    {
        OCI_ConnectionLogonSessionPool(ctx, con, tag);
    }

#endif

    /* check for success */

    if (OCI_STATUS)
    {
        /* get server version */

        OCI_GetVersionServer(con);

        con->cstate = OCI_CONN_LOGGED;
    }

    return OCI_STATUS;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ConnectionLogoffRegular
 * --------------------------------------------------------------------------------------------- */

void OCI_ConnectionLogoffRegular
(
    OCI_Context    *ctx,
    OCI_Connection *con
)
{
    /* close any server files not explicitly closed - no check of return code */

    if  (con->cxt && con->err && con->ses)
    {
        OCI_EXEC(OCISessionEnd(con->cxt, con->err, con->ses, (ub4)OCI_DEFAULT));

        /* close session handle */

        if (con->ses)
        {
            OCI_HandleFree((dvoid *) con->ses, OCI_HTYPE_SESSION);

            con->ses = NULL;
        }

        /* close context handle */

        if (con->cxt)
        {
            OCI_HandleFree((dvoid *) con->cxt, OCI_HTYPE_SVCCTX);

            con->cxt = NULL;
        }
    }    
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ConnectionLogoffSessionPool
 * --------------------------------------------------------------------------------------------- */

void OCI_ConnectionLogoffSessionPool
(
    OCI_Context    *ctx,
    OCI_Connection *con
)
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

    if (OCILib.version_runtime >= OCI_9_2)
    {
        dbtext *dbstr  = NULL;
        int     dbsize = 0;
        ub4     mode   = OCI_DEFAULT;

        /* Clear session tag if connection was retrieved from session pool */

        if (con->pool && con->sess_tag && ( OCI_HTYPE_SPOOL == con->pool->htype))
        {
            dbsize = -1;
            dbstr  = OCI_StringGetOracleString(con->sess_tag, &dbsize);
            mode   = OCI_SESSRLS_RETAG;
        }

        OCI_EXEC(OCISessionRelease(con->cxt, con->err, (OraText*)dbstr, (ub4)dbsize, mode));
 
        OCI_StringReleaseOracleString(dbstr);

        con->cxt = NULL;
        con->ses = NULL;
        con->svr = NULL;
    }

    #endif
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ConnectionLogOff
 * --------------------------------------------------------------------------------------------- */

boolean OCI_ConnectionLogOff
(
    OCI_Connection *con
)
{
    OCI_CALL_DECLARE_CONTEXT(TRUE)

    OCI_CHECK(NULL == con, FALSE)
    OCI_CHECK(con->cstate != OCI_CONN_LOGGED, FALSE)

    OCI_CALL_CONTEXT_SET_FROM_CONN(con);

    /* close opened files */

    if (con->nb_files > 0)
    {
        OCILobFileCloseAll(con->cxt, con->err);
    }

    /* dissociate connection from existing subscriptions */

    OCI_ListForEachWithParam(OCILib.subs, con, (POCI_LIST_FOR_EACH_WITH_PARAM) OCI_ConnectionDetachSubscriptions);

    /* free all statements */

    OCI_ListForEach(con->stmts, (POCI_LIST_FOR_EACH) OCI_StatementClose);
    OCI_ListClear(con->stmts);

    /* free all type info objects */

    OCI_ListForEach(con->tinfs, (POCI_LIST_FOR_EACH) OCI_TypeInfoClose);
    OCI_ListClear(con->tinfs);

    /* free all transactions */

    OCI_ListForEach(con->trsns, (POCI_LIST_FOR_EACH) OCI_TransactionClose);
    OCI_ListClear(con->trsns);

    /* 1 - XA connection */

#if OCI_VERSION_COMPILE >= OCI_10_1

    if (con->mode & OCI_SESSION_XA)
    {
        /* nothing to do */
    }
    else

#endif

    /* 2 - regular connection and connection from connection pool */

    if (con->alloc_handles)
    {
        OCI_ConnectionLogoffRegular(ctx, con);
    }

#if OCI_VERSION_COMPILE >= OCI_9_2

   /* 3 - connection from session pool */

    else if (OCILib.version_runtime >= OCI_9_0)
    {
        OCI_ConnectionLogoffSessionPool(ctx, con);
    }

#endif

    /* update internal status */

    if (OCI_STATUS)
    {
        con->cstate = OCI_CONN_ATTACHED;
    }

    return OCI_STATUS;
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
    OCI_FREE(con->trace)

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
    OCI_CALL_ENTER(OCI_Connection *, NULL)
    OCI_CALL_CHECK_INITIALIZED()
    OCI_CALL_CHECK_XA_ENABLED(mode)

    OCI_RETVAL = OCI_ConnectionCreateInternal(NULL, db, user, pwd, mode, NULL);
    OCI_STATUS = (NULL != OCI_RETVAL);

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ConnectionFree
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ConnectionFree
(
    OCI_Connection *con
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_CONNECTION, con)
    OCI_CALL_CONTEXT_SET_FROM_CONN(con)

    OCI_RETVAL = OCI_STATUS = OCI_ConnectionClose(con);

    OCI_ListRemove(OCILib.cons, con);
    OCI_FREE(con)

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_Commit
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_Commit
(
    OCI_Connection *con
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_CONNECTION, con)
    OCI_CALL_CONTEXT_SET_FROM_CONN(con)

    OCI_EXEC(OCITransCommit(con->cxt, con->err, (ub4)OCI_DEFAULT));

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
 }

/* --------------------------------------------------------------------------------------------- *
 * OCI_Rollback
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_Rollback
(
    OCI_Connection *con
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_CONNECTION, con)
    OCI_CALL_CONTEXT_SET_FROM_CONN(con)

    OCI_EXEC(OCITransRollback(con->cxt, con->err, (ub4)OCI_DEFAULT));

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
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
    OCI_SET_PROP(boolean, OCI_IPC_CONNECTION, con, autocom, enable, con, NULL, con->err)
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetAutoCommit
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_GetAutoCommit
(
    OCI_Connection *con
)
{
    OCI_GET_PROP(boolean, FALSE, OCI_IPC_CONNECTION, con, autocom, con, NULL, con->err)
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

    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_CONNECTION, con)
    OCI_CALL_CONTEXT_SET_FROM_CONN(con)

    OCI_GET_ATTRIB(OCI_HTYPE_SERVER, OCI_ATTR_SERVER_STATUS, con->svr, &status, &size)

    OCI_RETVAL = (status == OCI_SERVER_NORMAL);

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetUserData
 * --------------------------------------------------------------------------------------------- */

void * OCI_API OCI_GetUserData
(
    OCI_Connection *con
)
{
    OCI_GET_PROP(void*, NULL, OCI_IPC_CONNECTION, con, usrdata, con, NULL, con->err)
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
    OCI_SET_PROP(void*, OCI_IPC_CONNECTION, con, usrdata, data, con, NULL, con->err)
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
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_CONNECTION, con)
    OCI_CALL_CONTEXT_SET_FROM_CONN(con)

    OCI_FREE(con->sess_tag)

#if OCI_VERSION_COMPILE >= OCI_9_2

    if (tag && con->pool && (OCI_HTYPE_SPOOL == con->pool->htype))
    {
        con->sess_tag = ostrdup(tag);
        OCI_STATUS = (NULL != con->sess_tag);
    }

#else

    OCI_NOT_USED(tag)

#endif

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetSessionTag
 * --------------------------------------------------------------------------------------------- */

const otext * OCI_API OCI_GetSessionTag
(
    OCI_Connection *con
)
{
    OCI_GET_PROP(const otext*, NULL, OCI_IPC_CONNECTION, con, sess_tag, con, NULL, con->err)
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetDatabase
 * --------------------------------------------------------------------------------------------- */

const otext * OCI_API OCI_GetDatabase
(
    OCI_Connection *con
)
{
    OCI_GET_PROP(const otext*, NULL, OCI_IPC_CONNECTION, con, db, con, NULL, con->err)
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetUserName
 * --------------------------------------------------------------------------------------------- */

const otext * OCI_API OCI_GetUserName
(
    OCI_Connection *con
)
{
    OCI_GET_PROP(const otext*, NULL, OCI_IPC_CONNECTION, con, user, con, NULL, con->err)
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetPassword
 * --------------------------------------------------------------------------------------------- */

const otext * OCI_API OCI_GetPassword
(
    OCI_Connection *con
)
{
    OCI_GET_PROP(const otext*, NULL, OCI_IPC_CONNECTION, con, pwd, con, NULL, con->err)
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
    OCI_CALL_ENTER(boolean, FALSE)

    OCI_CALL_CHECK_PTR(OCI_IPC_CONNECTION, con)
    OCI_CALL_CHECK_PTR(OCI_IPC_STRING, password)
    OCI_CALL_CONTEXT_SET_FROM_CONN(con)

    if (OCI_CONN_LOGGED != con->cstate)
    {
        OCI_STATUS = OCI_ConnectionLogon(con, password, NULL);
    }
    else
    {
        int dbsize1 = -1;
        int dbsize2 = -1;
        int dbsize3 = -1;

        dbtext *dbstr1 = OCI_StringGetOracleString(con->user, &dbsize1);
        dbtext *dbstr2 = OCI_StringGetOracleString(con->pwd, &dbsize2);
        dbtext *dbstr3 = OCI_StringGetOracleString(password, &dbsize3);

        OCI_EXEC
        (
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

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
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

    OCI_CALL_ENTER(boolean, FALSE)

    /* let's be sure OCI_Initialize() has been called */

    OCI_CALL_CHECK_INITIALIZED()
    OCI_CALL_CHECK_PTR(OCI_IPC_STRING, pwd)
    OCI_CALL_CHECK_PTR(OCI_IPC_STRING, new_pwd)

    con = OCI_ConnectionAllocate(NULL, db, user, pwd, OCI_AUTH);

    if (con)
    {
        if (!OCI_ConnectionAttach(con) || !OCI_ConnectionLogon(con, new_pwd, NULL))
        {
            OCI_ConnectionFree(con);
            con = NULL;
        }
    }

    OCI_STATUS = (con != NULL);

    if (OCI_STATUS)
    {
        OCI_ConnectionFree(con);
    }

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetSessionMode
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_GetSessionMode
(
    OCI_Connection *con
)
{
    OCI_GET_PROP(unsigned int, OCI_UNKNOWN, OCI_IPC_CONNECTION, con, mode, con, NULL, con->err)
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetVersionServer
 * --------------------------------------------------------------------------------------------- */

const otext * OCI_API OCI_GetVersionServer
(
    OCI_Connection *con
)
{
    OCI_CALL_ENTER(const otext *, NULL)
    OCI_CALL_CHECK_PTR(OCI_IPC_CONNECTION, con)
    OCI_CALL_CONTEXT_SET_FROM_CONN(con)

    /* no version available in preliminary authentication mode */

    if (!con->ver_str && (!(con->mode & OCI_PRELIM_AUTH)))
    {
        int     dbsize = OCI_SIZE_BUFFER * (int) sizeof(dbtext);
        dbtext *dbstr  = NULL;

        OCI_ALLOCATE_DATA(OCI_IPC_STRING, con->ver_str, OCI_SIZE_BUFFER + 1)

        if (OCI_STATUS)
        {
            dbstr = OCI_StringGetOracleString(con->ver_str, &dbsize);

            OCI_EXEC(OCIServerVersion((dvoid *)con->cxt, con->err, (OraText *)dbstr, (ub4)dbsize, (ub1)OCI_HTYPE_SVCCTX))

            OCI_StringCopyOracleStringToNativeString(dbstr, con->ver_str, dbcharcount(dbsize));
            OCI_StringReleaseOracleString(dbstr);
        }

        if (OCI_STATUS)
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

    OCI_RETVAL = (const otext*)con->ver_str;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetServerMajorVersion
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_GetServerMajorVersion
(
    OCI_Connection *con
)
{
    OCI_CALL_ENTER(unsigned int, OCI_UNKNOWN)
    OCI_CALL_CHECK_PTR(OCI_IPC_CONNECTION, con)
    OCI_CALL_CONTEXT_SET_FROM_CONN(con)

    if (OCI_UNKNOWN == con->ver_num)
    {
        OCI_GetVersionServer(con);
    }

    OCI_RETVAL = (unsigned int) OCI_VER_MAJ(con->ver_num);

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetServerMinorVersion
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_GetServerMinorVersion
(
    OCI_Connection *con
)
{
    OCI_CALL_ENTER(unsigned int, OCI_UNKNOWN)
    OCI_CALL_CHECK_PTR(OCI_IPC_CONNECTION, con)
    OCI_CALL_CONTEXT_SET_FROM_CONN(con)

    if (OCI_UNKNOWN == con->ver_num)
    {
        OCI_GetVersionServer(con);
    }

    OCI_RETVAL = (unsigned int) OCI_VER_MIN(con->ver_num);

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetServerRevisionVersion
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_GetServerRevisionVersion
(
    OCI_Connection *con
)
{
    OCI_CALL_ENTER(unsigned int, OCI_UNKNOWN)
    OCI_CALL_CHECK_PTR(OCI_IPC_CONNECTION, con)
    OCI_CALL_CONTEXT_SET_FROM_CONN(con)

    if (OCI_UNKNOWN == con->ver_num)
    {
        OCI_GetVersionServer(con);
    }

    OCI_RETVAL = (unsigned int) OCI_VER_REV(con->ver_num);

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetTransaction
 * --------------------------------------------------------------------------------------------- */

OCI_Transaction * OCI_API OCI_GetTransaction
(
    OCI_Connection *con
)
{
    OCI_GET_PROP(OCI_Transaction*, NULL, OCI_IPC_CONNECTION, con, trs, con, NULL, con->err)
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
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_CONNECTION, con)
    OCI_CALL_CHECK_PTR(OCI_IPC_TRANSACTION, trans)
    OCI_CALL_CONTEXT_SET_FROM_CONN(con)

    if (con->trs)
    {
        OCI_STATUS = OCI_TransactionStop(con->trs);
    }

    OCI_SET_ATTRIB(OCI_HTYPE_SVCCTX, OCI_ATTR_TRANS, con->cxt, trans->htr, sizeof(trans->htr));

    if (OCI_STATUS)
    {
        con->trs = trans;
    }

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetVersionConnection
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_GetVersionConnection
(
    OCI_Connection *con
)
{
    OCI_CALL_ENTER(unsigned int, OCI_UNKNOWN)
    OCI_CALL_CHECK_PTR(OCI_IPC_CONNECTION, con)
    OCI_CALL_CONTEXT_SET_FROM_CONN(con)

    /* return the minimum supported version */

    OCI_RETVAL = (OCILib.version_runtime > con->ver_num) ? con->ver_num : OCILib.version_runtime;
    
    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_Break
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_Break
(
    OCI_Connection *con
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_CONNECTION, con)
    OCI_CALL_CONTEXT_SET_FROM_CONN(con)

    OCI_EXEC(OCIBreak((dvoid *) con->cxt, con->err))

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
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
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_CONNECTION, con)
    OCI_CALL_CONTEXT_SET_FROM_CONN(con)

    /* initialize the output buffer on server side */

    OCI_ALLOCATE_DATA(OCI_IPC_SERVER_OUPUT, con->svopt, 1)

    /* allocation internal buffer if needed */

    if (OCI_STATUS && !con->svopt->arrbuf)
    {
        const unsigned int charsize = sizeof(otext);

        /* check parameter ranges ( Oracle 10g increased the size of output line */

        if (con->ver_num >= OCI_10_2 && lnsize > OCI_OUPUT_LSIZE_10G)
        {
            lnsize = OCI_OUPUT_LSIZE_10G;
        }
        else  if (lnsize > OCI_OUPUT_LSIZE)
        {
            lnsize = OCI_OUPUT_LSIZE;
        }

        con->svopt->arrsize = arrsize;
        con->svopt->lnsize  = lnsize;

        /* allocate internal string (line) array */

        OCI_ALLOCATE_BUFFER(OCI_IPC_STRING, con->svopt->arrbuf, (con->svopt->lnsize + 1) * charsize, con->svopt->arrsize)
    }

    if (OCI_STATUS)
    {
        if (!con->svopt->stmt)
        {
            con->svopt->stmt = OCI_StatementCreate(con);
            OCI_STATUS = (NULL != con->svopt->stmt);
        }

        if (con->svopt->stmt)
        {
            /* enable server output */

            OCI_STATUS = OCI_Prepare(con->svopt->stmt, OTEXT("BEGIN DBMS_OUTPUT.ENABLE(:n); END;"));

            OCI_STATUS = OCI_STATUS && OCI_BindUnsignedInt(con->svopt->stmt, OTEXT(":n"), &bufsize);

            if (0 == bufsize)
            {
                OCI_STATUS = OCI_STATUS && OCI_BindSetNull(OCI_GetBind(con->svopt->stmt, 1));
            }

            OCI_STATUS = OCI_STATUS && OCI_Execute(con->svopt->stmt);

            /* prepare the retrieval statement call */

            if (OCI_STATUS)
            {
                con->svopt->cursize = con->svopt->arrsize;
            }

            OCI_STATUS = OCI_STATUS && OCI_Prepare(con->svopt->stmt, OTEXT("BEGIN DBMS_OUTPUT.GET_LINES(:s, :i); END;"));

            OCI_STATUS = OCI_STATUS && OCI_BindArrayOfStrings(con->svopt->stmt, OTEXT(":s"),
                                                              (otext *) con->svopt->arrbuf,
                                                              con->svopt->lnsize,
                                                              con->svopt->arrsize);

            OCI_STATUS = OCI_STATUS && OCI_BindUnsignedInt(con->svopt->stmt, OTEXT(":i"), &con->svopt->cursize);
        }
    }

    if (!OCI_STATUS)
    {
        OCI_ServerDisableOutput(con);
    }

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ServerDisableOutput
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ServerDisableOutput
(
    OCI_Connection *con
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_CONNECTION, con)
    OCI_CALL_CONTEXT_SET_FROM_CONN(con)

    if (con->svopt)
    {
        OCI_STATUS = OCI_ExecuteStmt(con->svopt->stmt, OTEXT("BEGIN DBMS_OUTPUT.DISABLE(); END;"));

        if (con->svopt->stmt)
        {
            OCI_StatementFree(con->svopt->stmt);
            con->svopt->stmt = NULL;
        }

        OCI_FREE(con->svopt->arrbuf)
        OCI_FREE(con->svopt)
    }

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ServerGetOutput
 * --------------------------------------------------------------------------------------------- */

const otext * OCI_API OCI_ServerGetOutput
(
    OCI_Connection *con
)
{
    OCI_CALL_ENTER(const otext *, NULL)
    OCI_CALL_CHECK_PTR(OCI_IPC_CONNECTION, con)
    OCI_CALL_CONTEXT_SET_FROM_CONN(con)

    if (con->svopt)
    {
        if (0 == con->svopt->curpos || con->svopt->curpos >= con->svopt->cursize)
        {
            con->svopt->cursize = con->svopt->arrsize;
            OCI_STATUS = OCI_Execute(con->svopt->stmt);
            con->svopt->curpos = 0;
        }

        if (OCI_STATUS && (con->svopt->cursize > 0))
        {
            const unsigned int charsize = sizeof(otext);

            OCI_RETVAL = (const otext*)(con->svopt->arrbuf + (size_t)(((con->svopt->lnsize + 1) * charsize) * con->svopt->curpos++));
        }
    }

    OCI_CALL_EXIT()
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
    ub4 attrib = OCI_UNKNOWN;

    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_CONNECTION, con)
    OCI_CALL_CHECK_ENUM_VALUE(con, NULL, trace, TraceTypeValues, OTEXT("Trace Type"))
    OCI_CALL_CONTEXT_SET_FROM_CONN(con)

    /* allocate trace info structure only if trace functions are used */

    OCI_ALLOCATE_DATA(OCI_IPC_TRACE_INFO, con->trace, 1)

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
                OCI_SET_TRACE(identifier)
                break;
            }
            case OCI_TRC_MODULE:
            {
            #if OCI_VERSION_COMPILE >= OCI_10_1
                attrib = OCI_ATTR_MODULE;
            #endif
                OCI_SET_TRACE(module)
                break;
            }
            case OCI_TRC_ACTION:
            {
            #if OCI_VERSION_COMPILE >= OCI_10_1
                attrib = OCI_ATTR_ACTION;
            #endif
                OCI_SET_TRACE(action)
                break;
            }
            case OCI_TRC_DETAIL:
            {
            #if OCI_VERSION_COMPILE >= OCI_10_1
                attrib = OCI_ATTR_CLIENT_INFO;
            #endif
                OCI_SET_TRACE(info)
                break;
            }
            case OCI_TRC_OPERATION:
            {
#if OCI_VERSION_COMPILE >= OCI_12_1
                attrib = OCI_ATTR_DBOP;
#endif
                OCI_SET_TRACE(operation)
           break;
            }
        }
    }

#if OCI_VERSION_COMPILE >= OCI_10_1

    /* On success, we give the value to Oracle to record it in system views */

    if (OCI_STATUS && attrib != OCI_UNKNOWN)
    {
        dbtext *dbstr  = NULL;
        int     dbsize = 0;

        if (str)
        {
            dbsize = -1;
            dbstr  = OCI_StringGetOracleString(str, &dbsize);
        }

        OCI_SET_ATTRIB(OCI_HTYPE_SESSION, attrib, con->ses, dbstr, dbsize);

        OCI_StringReleaseOracleString(dbstr);
    }

#endif

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
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
    OCI_CALL_ENTER(const otext *, NULL)
    OCI_CALL_CHECK_PTR(OCI_IPC_CONNECTION, con)
    OCI_CALL_CHECK_ENUM_VALUE(con, NULL, trace, TraceTypeValues, OTEXT("Trace Type"))
    OCI_CALL_CONTEXT_SET_FROM_CONN(con)

    if (con->trace)
    {
        switch (trace)
        {
            case OCI_TRC_IDENTITY:
            {
                OCI_GET_TRACE(identifier)
                break;
            }
            case OCI_TRC_MODULE:
            {
                OCI_GET_TRACE(module)
                break;
            }
            case OCI_TRC_ACTION:
            {
                OCI_GET_TRACE(action)
                break;
            }
            case OCI_TRC_DETAIL:
            {
                OCI_GET_TRACE(info)
                break;
            }
            case OCI_TRC_OPERATION:
            {
                OCI_GET_TRACE(operation)
                break;
            }
        }
    }

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_Ping
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_Ping
(
    OCI_Connection *con
)
{
    OCI_CALL_ENTER(boolean , FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_CONNECTION, con)
    OCI_CALL_CONTEXT_SET_FROM_CONN(con)

#if OCI_VERSION_COMPILE >= OCI_10_2

    if (OCILib.version_runtime >= OCI_10_2)
    {
        OCI_EXEC(OCIPing(con->cxt, con->err, (ub4)OCI_DEFAULT))

        OCI_RETVAL = OCI_STATUS;
    }

#endif

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetDBName
 * --------------------------------------------------------------------------------------------- */

const otext * OCI_API OCI_GetDBName
(
    OCI_Connection *con
)
{
    OCI_CALL_ENTER(const otext *, NULL)
    OCI_CALL_CHECK_PTR(OCI_IPC_CONNECTION, con)
    OCI_CALL_CONTEXT_SET_FROM_CONN(con)

#if OCI_VERSION_COMPILE >= OCI_10_2

    if (!con->db_name)
    {
        unsigned int size = 0;
        
        OCI_STATUS = OCI_GetStringAttribute(con, con->svr, OCI_HTYPE_SERVER, OCI_ATTR_DBNAME, &con->db_name, &size);
    }

#endif

    OCI_RETVAL = (const otext *) con->db_name;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetInstanceName
 * --------------------------------------------------------------------------------------------- */

const otext * OCI_API OCI_GetInstanceName
(
    OCI_Connection *con
)
{
    OCI_CALL_ENTER(const otext *, NULL)
    OCI_CALL_CHECK_PTR(OCI_IPC_CONNECTION, con)
    OCI_CALL_CONTEXT_SET_FROM_CONN(con)

#if OCI_VERSION_COMPILE >= OCI_10_2

    if (!con->inst_name)
    {
        unsigned int size = 0;
        
        OCI_STATUS = OCI_GetStringAttribute(con, con->svr, OCI_HTYPE_SERVER, OCI_ATTR_INSTNAME, &con->inst_name, &size);
    }

#endif

    OCI_RETVAL = (const otext *)con->inst_name;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetServiceName
 * --------------------------------------------------------------------------------------------- */

const otext * OCI_API OCI_GetServiceName
(
    OCI_Connection *con
)
{
    OCI_CALL_ENTER(const otext *, NULL)
    OCI_CALL_CHECK_PTR(OCI_IPC_CONNECTION, con)
    OCI_CALL_CONTEXT_SET_FROM_CONN(con)

#if OCI_VERSION_COMPILE >= OCI_10_2

    if (!con->service_name)
    {
        unsigned int size = 0;
        
        OCI_STATUS = OCI_GetStringAttribute(con, con->svr, OCI_HTYPE_SERVER, OCI_ATTR_SERVICENAME, &con->service_name, &size);
    }

#endif

    OCI_RETVAL = (const otext *)con->service_name;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetServerName
 * --------------------------------------------------------------------------------------------- */

const otext * OCI_API OCI_GetServerName
(
    OCI_Connection *con
)
{
    OCI_CALL_ENTER(const otext *, NULL)
    OCI_CALL_CHECK_PTR(OCI_IPC_CONNECTION, con)
    OCI_CALL_CONTEXT_SET_FROM_CONN(con)

#if OCI_VERSION_COMPILE >= OCI_10_2

    if (!con->server_name)
    {
        unsigned int size = 0;
        
        OCI_STATUS = OCI_GetStringAttribute(con, con->svr, OCI_HTYPE_SERVER, OCI_ATTR_HOSTNAME, &con->server_name, &size);
    }

#endif

    OCI_RETVAL = (const otext *)con->server_name;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetDomainName
 * --------------------------------------------------------------------------------------------- */

const otext * OCI_API OCI_GetDomainName
(
    OCI_Connection *con
)
{
    OCI_CALL_ENTER(const otext *, NULL)
    OCI_CALL_CHECK_PTR(OCI_IPC_CONNECTION, con)
    OCI_CALL_CONTEXT_SET_FROM_CONN(con)

#if OCI_VERSION_COMPILE >= OCI_10_2

    if (!con->domain_name)
    {
        unsigned int size = 0;
        
        OCI_STATUS = OCI_GetStringAttribute(con, con->svr, OCI_HTYPE_SERVER, OCI_ATTR_DBDOMAIN, &con->domain_name, &size);
    }

#endif

    OCI_RETVAL = (const otext *)con->domain_name;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetInstanceStartTime
 * --------------------------------------------------------------------------------------------- */

OCI_Timestamp * OCI_API OCI_GetInstanceStartTime
(
    OCI_Connection *con
)
{
    OCI_CALL_ENTER(OCI_Timestamp*, NULL)
    OCI_CALL_CHECK_PTR(OCI_IPC_CONNECTION, con)
    OCI_CALL_CONTEXT_SET_FROM_CONN(con)

#if OCI_VERSION_COMPILE >= OCI_10_2

    if (!con->inst_startup)
    {
        OCIDateTime *handle = NULL;

        OCI_GET_ATTRIB(OCI_HTYPE_SERVER, OCI_ATTR_INSTSTARTTIME, con->svr, &handle, NULL);
        con->inst_startup = OCI_TimestampInit(con, NULL, handle, OCI_TIMESTAMP);
        OCI_STATUS = OCI_STATUS && (NULL != con->inst_startup);
    }

#endif

    OCI_RETVAL = con->inst_startup;

    OCI_CALL_EXIT()
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

    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_CONNECTION, con)
    OCI_CALL_CONTEXT_SET_FROM_CONN(con)

#if OCI_VERSION_COMPILE >= OCI_10_2

    if (OCILib.version_runtime >= OCI_10_2)
    {
        OCI_GET_ATTRIB(OCI_HTYPE_SERVER, OCI_ATTR_TAF_ENABLED, con->svr, &value, NULL);
    }

#endif

    OCI_RETVAL = value;

    OCI_CALL_EXIT()
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
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_CONNECTION, con)
    OCI_CALL_CONTEXT_SET_FROM_CONN(con)

    OCI_RETVAL = OCI_IsTAFCapable(con);

#if OCI_VERSION_COMPILE >= OCI_10_2

    if (OCI_RETVAL && OCILib.version_runtime >= OCI_10_2)
    {
        OCIFocbkStruct fo_struct;

        memset(&fo_struct, 0, sizeof(fo_struct));

        con->taf_handler = handler;

        if (con->taf_handler)
        {
            fo_struct.callback_function = (OCICallbackFailover) OCI_ProcFailOver;
            fo_struct.fo_ctx            = (dvoid *) con;
        }

        OCI_SET_ATTRIB(OCI_HTYPE_SERVER, OCI_ATTR_FOCBK, con->svr, &fo_struct, 0);
    }

#endif

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
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

    OCI_CALL_ENTER(unsigned int, 0)
    OCI_CALL_CHECK_PTR(OCI_IPC_CONNECTION, con)
    OCI_CALL_CONTEXT_SET_FROM_CONN(con)

#if OCI_VERSION_COMPILE >= OCI_9_2

    if (OCILib.version_runtime >= OCI_9_2)
    {
        OCI_GET_ATTRIB(OCI_HTYPE_SVCCTX, OCI_ATTR_STMTCACHESIZE, con->cxt, &cache_size, NULL);
    }

#endif

    OCI_RETVAL = cache_size;

    OCI_CALL_EXIT()
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

    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_CONNECTION, con)
    OCI_CALL_CONTEXT_SET_FROM_CONN(con)

#if OCI_VERSION_COMPILE >= OCI_9_2

    if (OCILib.version_runtime >= OCI_9_2)
    {
        OCI_SET_ATTRIB(OCI_HTYPE_SVCCTX, OCI_ATTR_STMTCACHESIZE, con->cxt, &cache_size, sizeof(cache_size));
    }

#else

    OCI_NOT_USED(cache_size)

#endif

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
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

    OCI_CALL_ENTER(unsigned int, 0)
    OCI_CALL_CHECK_PTR(OCI_IPC_CONNECTION, con)
    OCI_CALL_CONTEXT_SET_FROM_CONN(con)

#if OCI_VERSION_COMPILE >= OCI_11_1

    if (con->ver_num >= OCI_11_1)
    {
        OCI_GET_ATTRIB(OCI_HTYPE_SESSION, OCI_ATTR_DEFAULT_LOBPREFETCH_SIZE, con->ses, &prefetch_size, NULL);
    }

#endif

    OCI_RETVAL = prefetch_size;

    OCI_CALL_EXIT()
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

    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_CONNECTION, con)
    OCI_CALL_CONTEXT_SET_FROM_CONN(con)

#if OCI_VERSION_COMPILE >= OCI_11_1

    if (con->ver_num >= OCI_11_1)
    {
        OCI_SET_ATTRIB(OCI_HTYPE_SESSION, OCI_ATTR_DEFAULT_LOBPREFETCH_SIZE, con->ses, &prefetch_size, sizeof(prefetch_size));
    }

#else

    OCI_NOT_USED(prefetch_size)

#endif

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
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

    OCI_CALL_ENTER(unsigned int, 0)
    OCI_CALL_CHECK_PTR(OCI_IPC_CONNECTION, con)
    OCI_CALL_CONTEXT_SET_FROM_CONN(con)

#if OCI_VERSION_COMPILE >= OCI_12_1

    if (con->ver_num >= OCI_12_1)
    {
        OCI_GET_ATTRIB(OCI_HTYPE_SESSION, OCI_ATTR_MAX_OPEN_CURSORS, con->ses, &max_cursors, NULL);
    }

#endif

    OCI_RETVAL = max_cursors;

    OCI_CALL_EXIT()
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

    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_CONNECTION, con)
    OCI_CALL_CHECK_PTR(OCI_IPC_STRING, sql)
    OCI_CALL_CONTEXT_SET_FROM_CONN(con)

    /* First, execute SQL */

    stmt = OCI_StatementCreate(con);
    OCI_STATUS = (NULL != stmt);
    
    if (OCI_STATUS)
    {
        OCI_STATUS = OCI_ExecuteStmt(stmt, sql);

        /* get resultset and set up variables */

        if (OCI_STATUS && (OCI_CST_SELECT == OCI_GetStatementType(stmt)))
        {
            va_start(args, sql);

            OCI_STATUS = OCI_FetchIntoUserVariables(stmt, args);

            va_end(args);
        }

        OCI_StatementFree(stmt);
    }

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
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

    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_CONNECTION, con)
    OCI_CALL_CHECK_PTR(OCI_IPC_STRING, sql)
    OCI_CALL_CONTEXT_SET_FROM_CONN(con)

    stmt = OCI_StatementCreate(con);
    OCI_STATUS = (NULL != stmt);

    if (OCI_STATUS)
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

            otext  *sql_fmt = NULL;

            OCI_ALLOCATE_DATA(OCI_IPC_STRING, sql_fmt, size + 1)
    
            if (OCI_STATUS)
            {
                /* format buffer */

                va_start(args, sql);

                if (OCI_ParseSqlFmt(stmt, sql_fmt, sql, &args) > 0)
                {
                    /* prepare and execute SQL buffer */

                    OCI_STATUS = OCI_PrepareInternal(stmt, sql_fmt) && OCI_ExecuteInternal(stmt, OCI_DEFAULT);

                    /* get resultset and set up variables */

                    if (OCI_STATUS && (OCI_CST_SELECT == OCI_GetStatementType(stmt)))
                    {
                        OCI_STATUS = OCI_FetchIntoUserVariables(stmt, args);
                    }
                }

                va_end(args);

                OCI_FREE(sql_fmt)
            }
        }

        OCI_StatementFree(stmt);
    }

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

