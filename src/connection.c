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

#include "connection.h"

#include "bind.h"
#include "callback.h"
#include "error.h"
#include "format.h"
#include "list.h"
#include "macros.h"
#include "statement.h"
#include "strings.h"
#include "timestamp.h"
#include "transaction.h"
#include "typeinfo.h"

static const unsigned int TraceTypeValues[] = 
{ 
    OCI_TRC_IDENTITY, 
    OCI_TRC_MODULE,
    OCI_TRC_ACTION, 
    OCI_TRC_DETAIL, 
    OCI_TRC_OPERATION 
};

static const unsigned int TimeoutTypeValues[] =
{
    OCI_NTO_SEND,
    OCI_NTO_RECEIVE,
    OCI_NTO_CALL
};

#define SET_TRACE(prop)                                                  \
    con->trace->prop[0] = 0;                                             \
    if (value)                                                           \
    {                                                                    \
        ostrncat(con->trace->prop, value, osizeof(con->trace->prop)-1);  \
        str = con->trace->prop;                                          \
    }

#define GET_TRACE(prop)                                                 \
    RETVAL = con->trace->prop[0] ? con->trace->prop : NULL;

/* --------------------------------------------------------------------------------------------- *
 * ConnectionDetachSubscriptions
 * --------------------------------------------------------------------------------------------- */

void ConnectionDetachSubscriptions(OCI_Subscription *sub, OCI_Connection *con)
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
 * ConnectionAllocate
 * --------------------------------------------------------------------------------------------- */

OCI_Connection * ConnectionAllocate
(
    OCI_Pool    *pool,
    const otext *db,
    const otext *user,
    const otext *pwd,
    unsigned int mode
)
{
    OCI_Connection *con  = NULL;

    DECLARE_CTX(TRUE)

    /* create connection object */

    con = ListAppend(OCILib.cons, sizeof(*con));
    STATUS = (NULL != con);

    if (STATUS)
    {
        con->alloc_handles = (0 == (mode & OCI_SESSION_XA));

        /* create internal lists */

        con->stmts = ListCreate(OCI_IPC_STATEMENT);

        if (STATUS)
        {
            con->tinfs = ListCreate(OCI_IPC_TYPE_INFO);
            STATUS = (NULL != con->tinfs);
        }

        if (STATUS)
        {
            con->trsns = ListCreate(OCI_IPC_TRANSACTION);
            STATUS = (NULL != con->trsns);
        }

        /* set attributes */

        if (STATUS)
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

                if (IS_STRING_VALID(con->db))
                {
                    StringNativeToAnsi(con->db, dbname, (int) ostrlen(con->db));
                }

                con->env = xaoEnv((OraText *) (dbname[0] ? dbname : NULL ));

                if (NULL == con->env)
                {
                    ExceptionEnvFromXaString(con->db);
                }
            }
            else

        #endif

            {
                con->env = OCILib.env;
            }

            STATUS = (NULL != con->env);
        }

        /*  allocate error handle */

        STATUS = STATUS && MemoryAllocHandle((dvoid *)con->env, (dvoid **)(void *)&con->err, OCI_HTYPE_ERROR);
    }

    /* update internal status */

    if (STATUS && con)
    {
        con->cstate = OCI_CONN_ALLOCATED;
    }
    else if (con)
    {
        ConnectionFree(con);
        con = NULL;
    }

    return con;
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionDeallocate
 * --------------------------------------------------------------------------------------------- */

boolean ConnectionDeallocate
(
    OCI_Connection *con
)
{
    CHECK(NULL == con, FALSE)
    CHECK(con->cstate != OCI_CONN_ALLOCATED, FALSE)

    /* close error handle */

    if (con->err)
    {
        MemoryFreeHandle((dvoid *) con->err, OCI_HTYPE_ERROR);
    }

    /* close server handle (if it had been allocated) in case of login error */

    if ((con->svr) && con->alloc_handles)
    {
        MemoryFreeHandle((dvoid *) con->svr, OCI_HTYPE_SERVER);
    }

    con->cxt = NULL;
    con->ses = NULL;
    con->svr = NULL;
    con->err = NULL;

    return TRUE;
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionAttach
 * --------------------------------------------------------------------------------------------- */

boolean ConnectionAttach
(
    OCI_Connection *con
)
{
    DECLARE_CTX(TRUE)

    CHECK(NULL == con, FALSE)
    CHECK(con->cstate != OCI_CONN_ALLOCATED, FALSE)

    CTX_SET_FROM_CON(con);

    /* allocate server handle for non session pooled connection */

    if (con->alloc_handles)
    {
        ub4     cmode  = OCI_DEFAULT;
        dbtext *dbstr  = NULL;
        int     dbsize = -1;

        STATUS = MemoryAllocHandle((dvoid *) con->env, (dvoid **) (void *) &con->svr, OCI_HTYPE_SERVER);

        /* attach server handle to service name */

    #if OCI_VERSION_COMPILE >= OCI_9_0

        if (OCILib.version_runtime >= OCI_9_0 && con->pool)
        {
            dbstr = StringGetDBString(con->pool->name, &dbsize);
            cmode = OCI_CPOOL;
        }
        else

    #endif

        {
            dbstr = StringGetDBString(con->db, &dbsize);
        }

        EXEC(OCIServerAttach(con->svr, con->err, (OraText *)dbstr, (sb4)dbsize, cmode));

        StringReleaseDBString(dbstr);
    }

    /* handle errors */

    if (STATUS)
    {
        con->cstate = OCI_CONN_ATTACHED;
    }

    return STATUS;
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionDetach
 * --------------------------------------------------------------------------------------------- */

boolean ConnectionDetach
(
    OCI_Connection *con
)
{
    DECLARE_CTX(TRUE)

    CHECK(NULL == con, FALSE)
    CHECK(con->cstate != OCI_CONN_ATTACHED, FALSE)

    CTX_SET_FROM_CON(con);

    if (con->alloc_handles && con->svr)
    {
        /* detach from the oracle server */

        EXEC(OCIServerDetach(con->svr, con->err, OCI_DEFAULT));

        /* close server handle */

        MemoryFreeHandle((dvoid *) con->svr, OCI_HTYPE_SERVER);

        con->svr = NULL;
    }

    /* update internal status */

    if (STATUS)
    {
        con->cstate = OCI_CONN_ALLOCATED;
    }

    return STATUS;
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionLogonXA
 * --------------------------------------------------------------------------------------------- */

void ConnectionLogonXA
(
    OCI_Context    *ctx,
    OCI_Connection *con
)
{
    dbtext *dbstr_user  = NULL;
    int     dbsize_user = 0;
    char    dbname[OCI_SIZE_BUFFER + 1];

    memset(dbname, 0, sizeof(dbname));

    if (IS_STRING_VALID(con->db))
    {
        StringNativeToAnsi(con->db, dbname, (int) ostrlen(con->db));
    }

    con->cxt = xaoSvcCtx((OraText *) (dbname[0] ? dbname : NULL ));
    STATUS = (NULL != con->cxt);

    ATTRIB_GET(OCI_HTYPE_SVCCTX, OCI_ATTR_SERVER, con->cxt, &con->svr, NULL)
    ATTRIB_GET(OCI_HTYPE_SVCCTX, OCI_ATTR_SESSION, con->cxt, &con->ses, NULL)
    ATTRIB_GET(OCI_HTYPE_SESSION, OCI_ATTR_USERNAME, con->ses, &dbstr_user, &dbsize_user)

    if (NULL == con->ses)
    {
        STATUS = FALSE;
        ExceptionConnFromXaString(con->db);
    }

    if (STATUS && dbstr_user)
    {
        FREE(con->user)

        con->user = StringDuplicateFromDBString(dbstr_user, dbcharcount(dbsize_user));
    }
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionLogonRegular
 * --------------------------------------------------------------------------------------------- */

void ConnectionLogonRegular
(
    OCI_Context    *ctx,
    OCI_Connection *con,
    const otext    *new_pwd
)
{
    /* allocate session handle */

    MemoryAllocHandle((dvoid *)con->env, (dvoid **)(void *)&con->ses, OCI_HTYPE_SESSION);

    /* allocate context handle */

    MemoryAllocHandle((dvoid *)con->env, (dvoid **)(void *)&con->cxt, OCI_HTYPE_SVCCTX);

    /* set context server attribute */

    ATTRIB_SET(OCI_HTYPE_SVCCTX, OCI_ATTR_SERVER, con->cxt, con->svr, sizeof(con->svr));

    /* modify user password if needed */

    if (IS_STRING_VALID(new_pwd))
    {
        int dbsize1 = -1;
        int dbsize2 = -1;
        int dbsize3 = -1;

        dbtext *dbstr1 = NULL;
        dbtext *dbstr2 = NULL;
        dbtext *dbstr3 = NULL;

        dbstr1 = StringGetDBString(con->user, &dbsize1);
        dbstr2 = StringGetDBString(con->pwd, &dbsize2);
        dbstr3 = StringGetDBString(new_pwd, &dbsize3);

        ATTRIB_SET(OCI_HTYPE_SVCCTX, OCI_ATTR_SESSION, con->cxt, con->ses, sizeof(con->ses));

        EXEC
        (
            OCIPasswordChange(con->cxt, con->err, (OraText *)dbstr1, (ub4)dbsize1,
                              (OraText *)dbstr2, (ub4)dbsize2,
                              (OraText *)dbstr3, (ub4)dbsize3, OCI_AUTH)
        )

        StringReleaseDBString(dbstr1);
        StringReleaseDBString(dbstr2);
        StringReleaseDBString(dbstr3);

        if (STATUS)
        {
            FREE(con->pwd)
            con->pwd = ostrdup(new_pwd);
        }
    }
    else
    {
        int     dbsize = -1;
        dbtext *dbstr  = NULL;

        /* set session login attribute */

        if (STATUS && IS_STRING_VALID(con->user))
        {
            dbsize = -1;
            dbstr  = StringGetDBString(con->user, &dbsize);

            ATTRIB_SET(OCI_HTYPE_SESSION, OCI_ATTR_USERNAME, con->ses, dbstr, dbsize);

            StringReleaseDBString(dbstr);
        }

        /* set session password attribute */

        if (STATUS && IS_STRING_VALID(con->pwd))
        {
            dbsize = -1;
            dbstr  = StringGetDBString(con->pwd, &dbsize);

            ATTRIB_SET(OCI_HTYPE_SESSION, OCI_ATTR_PASSWORD, con->ses, dbstr, dbsize);

            StringReleaseDBString(dbstr);
        }

        /* set OCILIB driver layer name attribute */

#if OCI_VERSION_COMPILE >= OCI_11_1

        if (STATUS && (OCILib.version_runtime >= OCI_11_1))
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
            dbstr = StringGetDBString(driver_version, &dbsize);

            ATTRIB_SET(OCI_HTYPE_SESSION, OCI_ATTR_DRIVER_NAME, con->ses, dbstr, dbsize);

            StringReleaseDBString(dbstr);
        }

#endif

        /* start session */

        if (STATUS)
        {
            ub4 credt = OCI_CRED_RDBMS;
            ub4 mode  = con->mode;

            if (!IS_STRING_VALID(con->user) && !IS_STRING_VALID(con->pwd))
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

            EXEC(OCISessionBegin(con->cxt, con->err, con->ses, credt, mode));

            ATTRIB_SET(OCI_HTYPE_SVCCTX, OCI_ATTR_SESSION, con->cxt, con->ses, sizeof(con->ses));

            if (STATUS)
            {
                if (!(con->mode & OCI_PRELIM_AUTH))
                {
                    /* create default transaction object */

                    OCI_Transaction *trs = TransactionCreate(con, 1, OCI_TRANS_READWRITE, NULL);

                    if (trs && ConnectionSetTransaction(con, trs))
                    {
                        /* start transaction */

                        STATUS = TransactionStart(trs);
                    }
                }
            }
            else
            {
                /* could not start session, must free the session and context handles */

                MemoryFreeHandle((dvoid *)con->ses, OCI_HTYPE_SESSION);
                MemoryFreeHandle((dvoid *)con->cxt, OCI_HTYPE_SVCCTX);

                con->ses = NULL;
                con->cxt = NULL;
            }
        }
    }
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionLogonSessionPool
 * --------------------------------------------------------------------------------------------- */

void ConnectionLogonSessionPool
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

    if (con->mode & OCI_SESSION_SYSDBA)
    {
        sess_mode |= OCI_SESSGET_SYSDBA;
    }

    if (!IS_STRING_VALID(con->pool->user) && !IS_STRING_VALID(con->pool->pwd))
    {
        sess_mode |= OCI_SESSGET_CREDEXT;
    }

    if (IS_STRING_VALID(con->pool->name))
    {
        dbsize = -1;
        dbstr  = StringGetDBString(con->pool->name, &dbsize);
    }

    if (IS_STRING_VALID(tag))
    {
        dbsize_tag = -1;
        dbstr_tag  = StringGetDBString(tag, &dbsize_tag);
    }

    EXEC
    (
        OCISessionGet(con->env, con->err, &con->cxt, NULL,
                      (OraText  *)dbstr, (ub4)dbsize, (OraText *)dbstr_tag, dbsize_tag,
                      (OraText **)&dbstr_ret, &dbsize_ret, &found, sess_mode)
    )

    StringReleaseDBString(dbstr);
    StringReleaseDBString(dbstr_tag);

    ATTRIB_GET(OCI_HTYPE_SVCCTX, OCI_ATTR_SERVER, con->cxt, &con->svr, NULL)
    ATTRIB_GET(OCI_HTYPE_SVCCTX, OCI_ATTR_SESSION, con->cxt, &con->ses, NULL)

    if (STATUS && found)
    {
        ConnectionSetSessionTag(con, tag);
    }
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionLogon
 * --------------------------------------------------------------------------------------------- */

boolean ConnectionLogon
(
    OCI_Connection *con,
    const otext    *new_pwd,
    const otext    *tag
)
{
    DECLARE_CTX(TRUE)

    CHECK(NULL == con, FALSE)

    CTX_SET_FROM_CON(con);

#if OCI_VERSION_COMPILE < OCI_9_2

    OCI_NOT_USED(tag)

#endif

    /* 1 - XA connection */

#if OCI_VERSION_COMPILE >= OCI_10_1

    if (con->mode & OCI_SESSION_XA)
    {
        ConnectionLogonXA(ctx, con);
    }
    else

#endif

    /* 2 - regular connection and connection from connection pool */

    if (con->alloc_handles)
    {
        ConnectionLogonRegular(ctx, con, new_pwd);
    }

#if OCI_VERSION_COMPILE >= OCI_9_2

   /* 3 - connection from session pool */

    else if (OCILib.version_runtime >= OCI_9_2)
    {
        ConnectionLogonSessionPool(ctx, con, tag);
    }

#endif

    /* check for success */

    if (STATUS)
    {
        /* get server version */

        ConnectionGetServerVersion(con);

        con->cstate = OCI_CONN_LOGGED;
    }

    return STATUS;
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionLogoffRegular
 * --------------------------------------------------------------------------------------------- */

void ConnectionLogoffRegular
(
    OCI_Context    *ctx,
    OCI_Connection *con
)
{
    /* close any server files not explicitly closed - no check of return code */

    if  (con->cxt && con->err && con->ses)
    {
        EXEC(OCISessionEnd(con->cxt, con->err, con->ses, (ub4)OCI_DEFAULT));

        /* close session handle */

        if (con->ses)
        {
            MemoryFreeHandle((dvoid *) con->ses, OCI_HTYPE_SESSION);

            con->ses = NULL;
        }

        /* close context handle */

        if (con->cxt)
        {
            MemoryFreeHandle((dvoid *) con->cxt, OCI_HTYPE_SVCCTX);

            con->cxt = NULL;
        }
    }    
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionLogoffSessionPool
 * --------------------------------------------------------------------------------------------- */

void ConnectionLogoffSessionPool
(
    OCI_Context    *ctx,
    OCI_Connection *con
)
{
    /* No explicit transaction object => commit if needed otherwise rollback changes */

    if (con->autocom)
    {
        ConnectionCommit(con);
    }
    else
    {
        ConnectionRollback(con);
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
            dbstr  = StringGetDBString(con->sess_tag, &dbsize);
            mode   = OCI_SESSRLS_RETAG;
        }

        EXEC(OCISessionRelease(con->cxt, con->err, (OraText*)dbstr, (ub4)dbsize, mode));
 
        StringReleaseDBString(dbstr);

        con->cxt = NULL;
        con->ses = NULL;
        con->svr = NULL;
    }

    #endif
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionLogOff
 * --------------------------------------------------------------------------------------------- */

boolean ConnectionLogOff
(
    OCI_Connection *con
)
{
    DECLARE_CTX(TRUE)

    CHECK(NULL == con, FALSE)
    CHECK(con->cstate != OCI_CONN_LOGGED, FALSE)

    CTX_SET_FROM_CON(con);

    /* close opened files */

    if (con->nb_files > 0)
    {
        OCILobFileCloseAll(con->cxt, con->err);
    }

    /* dissociate connection from existing subscriptions */

    ListForEachWithParam(OCILib.subs, con, (POCI_LIST_FOR_EACH_WITH_PARAM) ConnectionDetachSubscriptions);

    /* free all statements */

    ListForEach(con->stmts, (POCI_LIST_FOR_EACH) StatementDispose);
    ListClear(con->stmts);

    /* free all type info objects */

    ListForEach(con->tinfs, (POCI_LIST_FOR_EACH) TypeInfoDispose);
    ListClear(con->tinfs);

    /* free all transactions */

    ListForEach(con->trsns, (POCI_LIST_FOR_EACH) TransactionDispose);
    ListClear(con->trsns);

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
        ConnectionLogoffRegular(ctx, con);
    }

#if OCI_VERSION_COMPILE >= OCI_9_2

   /* 3 - connection from session pool */

    else if (OCILib.version_runtime >= OCI_9_0)
    {
        ConnectionLogoffSessionPool(ctx, con);
    }

#endif

    /* update internal status */

    if (STATUS)
    {
        con->cstate = OCI_CONN_ATTACHED;
    }

    return STATUS;
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionClose
 * --------------------------------------------------------------------------------------------- */

boolean ConnectionDispose
(
    OCI_Connection *con
)
{
    OCI_Error   *err = NULL;
    unsigned int i   = 0;

    CHECK(NULL == con, FALSE)

    /* clear connection reference from current error object */

    err = ErrorGet(FALSE, FALSE);

    if (err && err->con == con)
    {
        err->con = NULL;
    }

    /* clear server output resources */

    ConnectionDisableServerOutput(con);

    /* log off and detach form server */

    ConnectionLogOff(con);
    ConnectionDetach(con);
    ConnectionDeallocate(con);

    /* free internal lists */

    ListFree(con->stmts);
    ListFree(con->trsns);
    ListFree(con->tinfs);

    /* free strings */

    for (i = 0; i < OCI_FMT_COUNT; i++)
    {
        FREE(con->formats[i])
    }

    FREE(con->ver_str)
    FREE(con->sess_tag)
    FREE(con->db_name)
    FREE(con->inst_name)
    FREE(con->service_name)
    FREE(con->server_name)
    FREE(con->db_name)
    FREE(con->domain_name)
    FREE(con->trace)

    if (!con->pool)
    {
        FREE(con->db)
        FREE(con->user)
        FREE(con->pwd)
    }

    if (con->inst_startup)
    {
        TimestampFree(con->inst_startup);
    }

    con->stmts = NULL;
    con->trsns = NULL;
    con->tinfs = NULL;

    return TRUE;
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionCreateInternal
 * --------------------------------------------------------------------------------------------- */

OCI_Connection * ConnectionCreateInternal
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

    OCI_Connection *con = ConnectionAllocate(pool, db, user, pwd, mode);

    if (con)
    {
        if (!ConnectionAttach(con) || !ConnectionLogon(con, NULL, tag))
        {
            ConnectionFree(con);
            con = NULL;
        }
    }

    return con;
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionGetMinSupportedVersion
 * --------------------------------------------------------------------------------------------- */

unsigned int ConnectionGetMinSupportedVersion
(
    OCI_Connection *con
)
{
    return (OCILib.version_runtime > con->ver_num) ? con->ver_num : OCILib.version_runtime;
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionIsVersionSupported
 * --------------------------------------------------------------------------------------------- */

boolean ConnectionIsVersionSupported
(
    OCI_Connection *con,
    unsigned int    version
)
{
    return ConnectionGetMinSupportedVersion(con) >= version;
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Connection * ConnectionCreate
(
    const otext *db,
    const otext *user,
    const otext *pwd,
    unsigned int mode
)
{
    CALL_ENTER(OCI_Connection *, NULL)
    CHECK_INITIALIZED()
    CHECK_XA_ENABLED(mode)

    RETVAL = ConnectionCreateInternal(NULL, db, user, pwd, mode, NULL);
    STATUS = (NULL != RETVAL);

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionFree
 * --------------------------------------------------------------------------------------------- */

boolean ConnectionFree
(
    OCI_Connection *con
)
{
    CALL_ENTER(boolean, FALSE)
    CHECK_PTR(OCI_IPC_CONNECTION, con)
    CTX_SET_FROM_CON(con)

    RETVAL = STATUS = ConnectionDispose(con);

    ListRemove(OCILib.cons, con);
    FREE(con)

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionCommit
 * --------------------------------------------------------------------------------------------- */

boolean ConnectionCommit
(
    OCI_Connection *con
)
{
    CALL_ENTER(boolean, FALSE)
    CHECK_PTR(OCI_IPC_CONNECTION, con)
    CTX_SET_FROM_CON(con)

    EXEC(OCITransCommit(con->cxt, con->err, (ub4)OCI_DEFAULT));

    RETVAL = STATUS;

    CALL_EXIT()
 }

/* --------------------------------------------------------------------------------------------- *
 * ConnectionRollback
 * --------------------------------------------------------------------------------------------- */

boolean ConnectionRollback
(
    OCI_Connection *con
)
{
    CALL_ENTER(boolean, FALSE)
    CHECK_PTR(OCI_IPC_CONNECTION, con)
    CTX_SET_FROM_CON(con)

    EXEC(OCITransRollback(con->cxt, con->err, (ub4)OCI_DEFAULT));

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionSetAutoCommit
 * --------------------------------------------------------------------------------------------- */

boolean ConnectionSetAutoCommit
(
    OCI_Connection *con,
    boolean         enable
)
{
    SET_PROP(boolean, OCI_IPC_CONNECTION, con, autocom, enable, con, NULL, con->err)
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionGetAutoCommit
 * --------------------------------------------------------------------------------------------- */

boolean ConnectionGetAutoCommit
(
    OCI_Connection *con
)
{
    GET_PROP(boolean, FALSE, OCI_IPC_CONNECTION, con, autocom, con, NULL, con->err)
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionIsConnected
 * --------------------------------------------------------------------------------------------- */

boolean ConnectionIsConnected
(
    OCI_Connection *con
)
{
    ub4 status  = 0;
    ub4 size    = (ub4) sizeof(status);

    CALL_ENTER(boolean, FALSE)
    CHECK_PTR(OCI_IPC_CONNECTION, con)
    CTX_SET_FROM_CON(con)

    ATTRIB_GET(OCI_HTYPE_SERVER, OCI_ATTR_SERVER_STATUS, con->svr, &status, &size)

    RETVAL = (status == OCI_SERVER_NORMAL);

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionGetUserData
 * --------------------------------------------------------------------------------------------- */

void * ConnectionGetUserData
(
    OCI_Connection *con
)
{
    CALL_ENTER(void*, NULL)
    CHECK_INITIALIZED()

    RETVAL = (void*) (con ? con->usrdata : OCILib.usrdata);
    
    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionSetSetData
 * --------------------------------------------------------------------------------------------- */

boolean ConnectionSetUserData
(
    OCI_Connection *con,
    void           *data
)
{
    CALL_ENTER(boolean, FALSE)
    CHECK_INITIALIZED()

    if (con)
    {
        con->usrdata = data;
    }
    else
    {
        OCILib.usrdata = data;
    }

    RETVAL = TRUE;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionSetSessionTag
 * --------------------------------------------------------------------------------------------- */

boolean ConnectionSetSessionTag
(
    OCI_Connection *con,
    const otext    *tag
)
{
    CALL_ENTER(boolean, FALSE)
    CHECK_PTR(OCI_IPC_CONNECTION, con)
    CTX_SET_FROM_CON(con)

    FREE(con->sess_tag)

#if OCI_VERSION_COMPILE >= OCI_9_2

    if (tag && con->pool && (OCI_HTYPE_SPOOL == con->pool->htype))
    {
        con->sess_tag = ostrdup(tag);
        STATUS = (NULL != con->sess_tag);
    }

#else

    OCI_NOT_USED(tag)

#endif

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionGetSessionTag
 * --------------------------------------------------------------------------------------------- */

const otext * ConnectionGetSessionTag
(
    OCI_Connection *con
)
{
    GET_PROP(const otext*, NULL, OCI_IPC_CONNECTION, con, sess_tag, con, NULL, con->err)
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionGetDatabase
 * --------------------------------------------------------------------------------------------- */

const otext * ConnectionGetConnectionString
(
    OCI_Connection *con
)
{
    GET_PROP(const otext*, NULL, OCI_IPC_CONNECTION, con, db, con, NULL, con->err)
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionGetUserName
 * --------------------------------------------------------------------------------------------- */

const otext * ConnectionGetUserName
(
    OCI_Connection *con
)
{
    GET_PROP(const otext*, NULL, OCI_IPC_CONNECTION, con, user, con, NULL, con->err)
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionGetPassword
 * --------------------------------------------------------------------------------------------- */

const otext * ConnectionGetPassword
(
    OCI_Connection *con
)
{
    GET_PROP(const otext*, NULL, OCI_IPC_CONNECTION, con, pwd, con, NULL, con->err)
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionSetPassword
 * --------------------------------------------------------------------------------------------- */

boolean ConnectionSetPassword
(
    OCI_Connection *con,
    const otext    *password
)
{
    CALL_ENTER(boolean, FALSE)

    CHECK_PTR(OCI_IPC_CONNECTION, con)
    CHECK_PTR(OCI_IPC_STRING, password)
    CTX_SET_FROM_CON(con)

    if (OCI_CONN_LOGGED != con->cstate)
    {
        STATUS = ConnectionLogon(con, password, NULL);
    }
    else
    {
        int dbsize1 = -1;
        int dbsize2 = -1;
        int dbsize3 = -1;

        dbtext *dbstr1 = StringGetDBString(con->user, &dbsize1);
        dbtext *dbstr2 = StringGetDBString(con->pwd, &dbsize2);
        dbtext *dbstr3 = StringGetDBString(password, &dbsize3);

        EXEC
        (
            OCIPasswordChange(con->cxt, con->err,
                              (OraText *) dbstr1, (ub4) dbsize1,
                              (OraText *) dbstr2, (ub4) dbsize2,
                              (OraText *) dbstr3, (ub4) dbsize3,
                              OCI_DEFAULT)
        )

        StringReleaseDBString(dbstr1);
        StringReleaseDBString(dbstr2);
        StringReleaseDBString(dbstr3);
    }

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionGetSessionMode
 * --------------------------------------------------------------------------------------------- */

unsigned int ConnectionGetSessionMode
(
    OCI_Connection *con
)
{
    GET_PROP(unsigned int, OCI_UNKNOWN, OCI_IPC_CONNECTION, con, mode, con, NULL, con->err)
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionGetVersionServer
 * --------------------------------------------------------------------------------------------- */

const otext * ConnectionGetServerVersion
(
    OCI_Connection *con
)
{
    CALL_ENTER(const otext *, NULL)
    CHECK_PTR(OCI_IPC_CONNECTION, con)
    CTX_SET_FROM_CON(con)

    /* no version available in preliminary authentication mode */

    if (!con->ver_str && (!(con->mode & OCI_PRELIM_AUTH)))
    {
        int     dbsize  = OCI_SIZE_BUFFER * (int) sizeof(dbtext);
        dbtext *dbstr   = NULL;
        ub4     version = 0;

        ALLOC_DATA(OCI_IPC_STRING, con->ver_str, OCI_SIZE_BUFFER + 1)

        if (STATUS)
        {    
            dbstr = StringGetDBString(con->ver_str, &dbsize);

         #if OCI_VERSION_COMPILE >= OCI_18_1
   
            if (OCILib.version_runtime >= OCI_18_1)
            {
                EXEC(OCIServerRelease2((dvoid *)con->cxt, con->err, (OraText *)dbstr, (ub4)dbsize, (ub1)OCI_HTYPE_SVCCTX, &version, OCI_DEFAULT))
            }
            else

        #endif
            
            {
                EXEC(OCIServerVersion((dvoid *)con->cxt, con->err, (OraText *)dbstr, (ub4)dbsize, (ub1)OCI_HTYPE_SVCCTX))
            }

            StringCopyDBStringToNativeString(dbstr, con->ver_str, dbcharcount(dbsize));
            StringReleaseDBString(dbstr);            
        }

        if (STATUS)
        {
            int ver_maj = 0, ver_min = 0, ver_rev = 0;

        #if OCI_VERSION_COMPILE >= OCI_18_1

            if (OCILib.version_runtime >= OCI_18_1)
            {
                ver_maj = OCI_SERVER_RELEASE_REL(version);
                ver_min = OCI_SERVER_RELEASE_REL_UPD(version);
                ver_rev = OCI_SERVER_RELEASE_REL_UPD_REV(version);

                con->ver_num = ver_maj * 100 + ver_min * 10 + ver_rev;
            }
            else

        #endif

            {
                otext *p = NULL;

                con->ver_str[ocharcount(dbsize)] = 0;

                /* parse server version string to find the version information
                    **/

                for (p = con->ver_str; p && *p; p++)
                {
                    if (oisdigit((unsigned char) *p) &&
                        (*(p + (size_t) 1) != 0) &&
                        (*(p + (size_t) 1) == OTEXT('.') || (*(p + (size_t) 2) == OTEXT('.') )))
                    {
                        if (NB_ARG_VERSION == osscanf(p, OTEXT("%d.%d.%d"),
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
        }
        else
        {
            FREE(con->ver_str)
        }
    }

    RETVAL = (const otext*)con->ver_str;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionGetServerMajorVersion
 * --------------------------------------------------------------------------------------------- */

unsigned int ConnectionGetServerMajorVersion
(
    OCI_Connection *con
)
{
    CALL_ENTER(unsigned int, OCI_UNKNOWN)
    CHECK_PTR(OCI_IPC_CONNECTION, con)
    CTX_SET_FROM_CON(con)

    if (OCI_UNKNOWN == con->ver_num)
    {
        ConnectionGetServerVersion(con);
    }

    RETVAL = (unsigned int) OCI_VER_MAJ(con->ver_num);

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionGetServerMinorVersion
 * --------------------------------------------------------------------------------------------- */

unsigned int ConnectionGetServerMinorVersion
(
    OCI_Connection *con
)
{
    CALL_ENTER(unsigned int, OCI_UNKNOWN)
    CHECK_PTR(OCI_IPC_CONNECTION, con)
    CTX_SET_FROM_CON(con)

    if (OCI_UNKNOWN == con->ver_num)
    {
        ConnectionGetServerVersion(con);
    }

    RETVAL = (unsigned int) OCI_VER_MIN(con->ver_num);

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionGetServerRevisionVersion
 * --------------------------------------------------------------------------------------------- */

unsigned int ConnectionGetServerRevisionVersion
(
    OCI_Connection *con
)
{
    CALL_ENTER(unsigned int, OCI_UNKNOWN)
    CHECK_PTR(OCI_IPC_CONNECTION, con)
    CTX_SET_FROM_CON(con)

    if (OCI_UNKNOWN == con->ver_num)
    {
        ConnectionGetServerVersion(con);
    }

    RETVAL = (unsigned int) OCI_VER_REV(con->ver_num);

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionGetTransaction
 * --------------------------------------------------------------------------------------------- */

OCI_Transaction * ConnectionGetTransaction
(
    OCI_Connection *con
)
{
    GET_PROP(OCI_Transaction*, NULL, OCI_IPC_CONNECTION, con, trs, con, NULL, con->err)
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionSetTransaction
 * --------------------------------------------------------------------------------------------- */

boolean ConnectionSetTransaction
(
    OCI_Connection  *con,
    OCI_Transaction *trans
)
{
    CALL_ENTER(boolean, FALSE)
    CHECK_PTR(OCI_IPC_CONNECTION, con)
    CHECK_PTR(OCI_IPC_TRANSACTION, trans)
    CTX_SET_FROM_CON(con)

    if (con->trs)
    {
        STATUS = TransactionStop(con->trs);
    }

    ATTRIB_SET(OCI_HTYPE_SVCCTX, OCI_ATTR_TRANS, con->cxt, trans->htr, sizeof(trans->htr));

    if (STATUS)
    {
        con->trs = trans;
    }

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionGetVersion
 * --------------------------------------------------------------------------------------------- */

unsigned int ConnectionGetVersion
(
    OCI_Connection *con
)
{
    CALL_ENTER(unsigned int, OCI_UNKNOWN)
    CHECK_PTR(OCI_IPC_CONNECTION, con)
    CTX_SET_FROM_CON(con)

    /* return the minimum supported version */
  
    RETVAL = ConnectionGetMinSupportedVersion(con);

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionBreak
 * --------------------------------------------------------------------------------------------- */

boolean ConnectionBreak
(
    OCI_Connection *con
)
{
    CALL_ENTER(boolean, FALSE)
    CHECK_PTR(OCI_IPC_CONNECTION, con)
    CTX_SET_FROM_CON(con)

    EXEC(OCIBreak((dvoid *) con->cxt, con->err))

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionServerEnableOutput
 * --------------------------------------------------------------------------------------------- */

boolean ConnectionEnableServerOutput
(
    OCI_Connection *con,
    unsigned int    bufsize,
    unsigned int    arrsize,
    unsigned int    lnsize
)
{
    CALL_ENTER(boolean, FALSE)
    CHECK_PTR(OCI_IPC_CONNECTION, con)
    CTX_SET_FROM_CON(con)

    /* initialize the output buffer on server side */

    ALLOC_DATA(OCI_IPC_SERVER_OUPUT, con->svopt, 1)

    /* allocation internal buffer if needed */

    if (STATUS && !con->svopt->arrbuf)
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

        ALLOC_BUFFER(OCI_IPC_STRING, con->svopt->arrbuf, (con->svopt->lnsize + 1) * charsize, con->svopt->arrsize)
    }

    if (STATUS)
    {
        if (!con->svopt->stmt)
        {
            con->svopt->stmt = StatementCreate(con);
            STATUS = (NULL != con->svopt->stmt);
        }

        if (con->svopt->stmt)
        {
            /* enable server output */

            STATUS = StatementPrepare(con->svopt->stmt, OTEXT("BEGIN DBMS_OUTPUT.ENABLE(:n); END;"));

            STATUS = STATUS && StatementBindUnsignedInt(con->svopt->stmt, OTEXT(":n"), &bufsize);

            if (0 == bufsize)
            {
                STATUS = STATUS && BindSetNull(StatementGetBind(con->svopt->stmt, 1));
            }

            STATUS = STATUS && StatementExecute(con->svopt->stmt);

            /* prepare the retrieval statement call */

            if (STATUS)
            {
                con->svopt->cursize = con->svopt->arrsize;
            }

            STATUS = STATUS && StatementPrepare(con->svopt->stmt, OTEXT("BEGIN DBMS_OUTPUT.GET_LINES(:s, :i); END;"));

            STATUS = STATUS && StatementBindArrayOfStrings(con->svopt->stmt, OTEXT(":s"),
                                                                  (otext *) con->svopt->arrbuf,
                                                                  con->svopt->lnsize,
                                                                  con->svopt->arrsize);

            STATUS = STATUS && StatementBindUnsignedInt(con->svopt->stmt, OTEXT(":i"), &con->svopt->cursize);
        }
    }

    if (!STATUS)
    {
       ConnectionDisableServerOutput(con);
    }

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionServerDisableOutput
 * --------------------------------------------------------------------------------------------- */

boolean ConnectionDisableServerOutput
(
    OCI_Connection *con
)
{
    CALL_ENTER(boolean, FALSE)
    CHECK_PTR(OCI_IPC_CONNECTION, con)
    CTX_SET_FROM_CON(con)

    if (con->svopt)
    {
        STATUS = StatementExecuteStmt(con->svopt->stmt, OTEXT("BEGIN DBMS_OUTPUT.DISABLE(); END;"));

        if (con->svopt->stmt)
        {
            StatementFree(con->svopt->stmt);
            con->svopt->stmt = NULL;
        }

        FREE(con->svopt->arrbuf)
        FREE(con->svopt)
    }

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionServerGetOutput
 * --------------------------------------------------------------------------------------------- */

const otext * ConnectionGetServerOutput
(
    OCI_Connection *con
)
{
    CALL_ENTER(const otext *, NULL)
    CHECK_PTR(OCI_IPC_CONNECTION, con)
    CTX_SET_FROM_CON(con)

    if (con->svopt)
    {
        if (0 == con->svopt->curpos || con->svopt->curpos >= con->svopt->cursize)
        {
            con->svopt->cursize = con->svopt->arrsize;
            STATUS = StatementExecute(con->svopt->stmt);
            con->svopt->curpos = 0;
        }

        if (STATUS && (con->svopt->cursize > 0))
        {
            const unsigned int charsize = sizeof(otext);

            RETVAL = (const otext*)(con->svopt->arrbuf + (size_t)(((con->svopt->lnsize + 1) * charsize) * con->svopt->curpos++));
        }
    }

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionSetTrace
 * --------------------------------------------------------------------------------------------- */

boolean ConnectionSetTrace
(
    OCI_Connection *con,
    unsigned int    trace,
    const otext    *value
)
{
    const otext *str = NULL;
    ub4 attrib = OCI_UNKNOWN;

    CALL_ENTER(boolean, FALSE)
    CHECK_PTR(OCI_IPC_CONNECTION, con)
    CHECK_ENUM_VALUE(con, NULL, trace, TraceTypeValues, OTEXT("Trace Type"))
    CTX_SET_FROM_CON(con)

    /* allocate trace info structure only if trace functions are used */

    ALLOC_DATA(OCI_IPC_TRACE_INFO, con->trace, 1)

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
                SET_TRACE(identifier)
                break;
            }
            case OCI_TRC_MODULE:
            {
            #if OCI_VERSION_COMPILE >= OCI_10_1
                attrib = OCI_ATTR_MODULE;
            #endif
                SET_TRACE(module)
                break;
            }
            case OCI_TRC_ACTION:
            {
            #if OCI_VERSION_COMPILE >= OCI_10_1
                attrib = OCI_ATTR_ACTION;
            #endif
                SET_TRACE(action)
                break;
            }
            case OCI_TRC_DETAIL:
            {
            #if OCI_VERSION_COMPILE >= OCI_10_1
                attrib = OCI_ATTR_CLIENT_INFO;
            #endif
                SET_TRACE(info)
                break;
            }
            case OCI_TRC_OPERATION:
            {
#if OCI_VERSION_COMPILE >= OCI_12_1
                attrib = OCI_ATTR_DBOP;
#endif
                SET_TRACE(operation)
           break;
            }
        }
    }

#if OCI_VERSION_COMPILE >= OCI_10_1

    /* On success, we give the value to Oracle to record it in system views */

    if (STATUS && attrib != OCI_UNKNOWN)
    {
        dbtext *dbstr  = NULL;
        int     dbsize = 0;

        if (str)
        {
            dbsize = -1;
            dbstr  = StringGetDBString(str, &dbsize);
        }

        ATTRIB_SET(OCI_HTYPE_SESSION, attrib, con->ses, dbstr, dbsize);

        StringReleaseDBString(dbstr);
    }

#endif

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionTraceGet
 * --------------------------------------------------------------------------------------------- */

const otext * ConnectionGetTrace
(
    OCI_Connection *con,
    unsigned int    trace
)
{
    CALL_ENTER(const otext *, NULL)
    CHECK_PTR(OCI_IPC_CONNECTION, con)
    CHECK_ENUM_VALUE(con, NULL, trace, TraceTypeValues, OTEXT("Trace Type"))
    CTX_SET_FROM_CON(con)

    if (con->trace)
    {
        switch (trace)
        {
            case OCI_TRC_IDENTITY:
            {
                GET_TRACE(identifier)
                break;
            }
            case OCI_TRC_MODULE:
            {
                GET_TRACE(module)
                break;
            }
            case OCI_TRC_ACTION:
            {
                GET_TRACE(action)
                break;
            }
            case OCI_TRC_DETAIL:
            {
                GET_TRACE(info)
                break;
            }
            case OCI_TRC_OPERATION:
            {
                GET_TRACE(operation)
                break;
            }
        }
    }

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionPing
 * --------------------------------------------------------------------------------------------- */

boolean ConnectionPing
(
    OCI_Connection *con
)
{
    CALL_ENTER(boolean , FALSE)
    CHECK_PTR(OCI_IPC_CONNECTION, con)
    CTX_SET_FROM_CON(con)

#if OCI_VERSION_COMPILE >= OCI_10_2

    if (OCILib.version_runtime >= OCI_10_2)
    {
        EXEC(OCIPing(con->cxt, con->err, (ub4)OCI_DEFAULT))

        RETVAL = STATUS;
    }

#endif

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionSetTimeout
 * --------------------------------------------------------------------------------------------- */

boolean ConnectionSetTimeout
(
    OCI_Connection *con,
    unsigned int    type,
    unsigned int    value
)
{
    ub4 timeout = value;

    CALL_ENTER(boolean , FALSE)
    CHECK_PTR(OCI_IPC_CONNECTION, con)
    CHECK_ENUM_VALUE(con, FALSE, type, TimeoutTypeValues, OTEXT("timeout type"))
    CTX_SET_FROM_CON(con)

#if OCI_VERSION_COMPILE >= OCI_12_1

    if (OCILib.version_runtime >= OCI_12_1)
    {
        switch (type)
        {
            case OCI_NTO_SEND:
            {
                ATTRIB_SET(OCI_HTYPE_SERVER, OCI_ATTR_SEND_TIMEOUT, con->svr, &timeout, sizeof(timeout))
                RETVAL = STATUS;
                break;
            }
            case OCI_NTO_RECEIVE:
            {
                ATTRIB_SET(OCI_HTYPE_SERVER, OCI_ATTR_RECEIVE_TIMEOUT, con->svr, &timeout, sizeof(timeout))
                RETVAL = STATUS;
                break;
            }

         #if OCI_VERSION_COMPILE >= OCI_18_1

            case OCI_NTO_CALL:
            {
                if (OCILib.version_runtime >= OCI_18_1)
                {
                    ATTRIB_SET(OCI_HTYPE_SVCCTX, OCI_ATTR_CALL_TIMEOUT, con->cxt, &timeout, sizeof(timeout))
                    RETVAL = STATUS;
                }
                break;
            }

        #endif
        
        }
    }

#endif

    CALL_EXIT()    
}


/* --------------------------------------------------------------------------------------------- *
 * ConnectionGetTimeout
 * --------------------------------------------------------------------------------------------- */

unsigned int ConnectionGetTimeout
(
    OCI_Connection *con,
    unsigned int    type
)
{
    ub4 timeout = 0;

    CALL_ENTER(unsigned int, 0)
    CHECK_PTR(OCI_IPC_CONNECTION, con)
    CHECK_ENUM_VALUE(con, 0, type, TimeoutTypeValues, OTEXT("timeout type"))
    CTX_SET_FROM_CON(con)

#if OCI_VERSION_COMPILE >= OCI_12_1

    if (OCILib.version_runtime >= OCI_12_1)
    {
        switch (type)
        {
            case OCI_NTO_SEND:
            {
                ATTRIB_GET(OCI_HTYPE_SERVER, OCI_ATTR_SEND_TIMEOUT, con->svr, &timeout, sizeof(timeout))
                break;
            }
            case OCI_NTO_RECEIVE:
            {
                ATTRIB_GET(OCI_HTYPE_SERVER, OCI_ATTR_RECEIVE_TIMEOUT, con->svr, &timeout, sizeof(timeout))
                 break;
            }

        #if OCI_VERSION_COMPILE >= OCI_18_1

            case OCI_NTO_CALL:
            {
                if (OCILib.version_runtime >= OCI_18_3)
                {
                    ATTRIB_GET(OCI_HTYPE_SVCCTX, OCI_ATTR_CALL_TIMEOUT, con->cxt, &timeout, sizeof(timeout))
                }
                break;
            }
            
         #endif

        }
    }

#endif

    RETVAL = timeout;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionGetDBName
 * --------------------------------------------------------------------------------------------- */

const otext * ConnectionGetDatabaseName
(
    OCI_Connection *con
)
{
    CALL_ENTER(const otext *, NULL)
    CHECK_PTR(OCI_IPC_CONNECTION, con)
    CTX_SET_FROM_CON(con)

#if OCI_VERSION_COMPILE >= OCI_10_2

    if (!con->db_name)
    {
        unsigned int size = 0;
        
        STATUS = StringGetAttribute(con, con->svr, OCI_HTYPE_SERVER, OCI_ATTR_DBNAME, &con->db_name, &size);
    }

#endif

    RETVAL = (const otext *) con->db_name;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionGetInstanceName
 * --------------------------------------------------------------------------------------------- */

const otext * ConnectionGetInstanceName
(
    OCI_Connection *con
)
{
    CALL_ENTER(const otext *, NULL)
    CHECK_PTR(OCI_IPC_CONNECTION, con)
    CTX_SET_FROM_CON(con)

#if OCI_VERSION_COMPILE >= OCI_10_2

    if (!con->inst_name)
    {
        unsigned int size = 0;
        
        STATUS = StringGetAttribute(con, con->svr, OCI_HTYPE_SERVER, OCI_ATTR_INSTNAME, &con->inst_name, &size);
    }

#endif

    RETVAL = (const otext *)con->inst_name;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionGetServiceName
 * --------------------------------------------------------------------------------------------- */

const otext * ConnectionGetServiceName
(
    OCI_Connection *con
)
{
    CALL_ENTER(const otext *, NULL)
    CHECK_PTR(OCI_IPC_CONNECTION, con)
    CTX_SET_FROM_CON(con)

#if OCI_VERSION_COMPILE >= OCI_10_2

    if (!con->service_name)
    {
        unsigned int size = 0;
        
        STATUS = StringGetAttribute(con, con->svr, OCI_HTYPE_SERVER, OCI_ATTR_SERVICENAME, &con->service_name, &size);
    }

#endif

    RETVAL = (const otext *)con->service_name;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionGetServerName
 * --------------------------------------------------------------------------------------------- */

const otext * ConnectionGetServerName
(
    OCI_Connection *con
)
{
    CALL_ENTER(const otext *, NULL)
    CHECK_PTR(OCI_IPC_CONNECTION, con)
    CTX_SET_FROM_CON(con)

#if OCI_VERSION_COMPILE >= OCI_10_2

    if (!con->server_name)
    {
        unsigned int size = 0;
        
        STATUS = StringGetAttribute(con, con->svr, OCI_HTYPE_SERVER, OCI_ATTR_HOSTNAME, &con->server_name, &size);
    }

#endif

    RETVAL = (const otext *)con->server_name;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionGetDomainName
 * --------------------------------------------------------------------------------------------- */

const otext * ConnectionGetDomainName
(
    OCI_Connection *con
)
{
    CALL_ENTER(const otext *, NULL)
    CHECK_PTR(OCI_IPC_CONNECTION, con)
    CTX_SET_FROM_CON(con)

#if OCI_VERSION_COMPILE >= OCI_10_2

    if (!con->domain_name)
    {
        unsigned int size = 0;
        
        STATUS = StringGetAttribute(con, con->svr, OCI_HTYPE_SERVER, OCI_ATTR_DBDOMAIN, &con->domain_name, &size);
    }

#endif

    RETVAL = (const otext *)con->domain_name;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionGetInstanceStartTime
 * --------------------------------------------------------------------------------------------- */

OCI_Timestamp * ConnectionGetInstanceStartTime
(
    OCI_Connection *con
)
{
    CALL_ENTER(OCI_Timestamp*, NULL)
    CHECK_PTR(OCI_IPC_CONNECTION, con)
    CTX_SET_FROM_CON(con)

#if OCI_VERSION_COMPILE >= OCI_10_2

    if (!con->inst_startup)
    {
        OCIDateTime *handle = NULL;

        ATTRIB_GET(OCI_HTYPE_SERVER, OCI_ATTR_INSTSTARTTIME, con->svr, &handle, NULL);
        con->inst_startup = TimestampInitialize(con, NULL, handle, OCI_TIMESTAMP);
        STATUS = STATUS && (NULL != con->inst_startup);
    }

#endif

    RETVAL = con->inst_startup;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionIsTAFCapable
 * --------------------------------------------------------------------------------------------- */

boolean ConnectionIsTAFCapable
(
    OCI_Connection *con
)
{
    boolean value = FALSE;

    CALL_ENTER(boolean, FALSE)
    CHECK_PTR(OCI_IPC_CONNECTION, con)
    CTX_SET_FROM_CON(con)

#if OCI_VERSION_COMPILE >= OCI_10_2

    if (OCILib.version_runtime >= OCI_10_2)
    {
        ATTRIB_GET(OCI_HTYPE_SERVER, OCI_ATTR_TAF_ENABLED, con->svr, &value, NULL);
    }

#endif

    RETVAL = value;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionSetTAFHandler
 * --------------------------------------------------------------------------------------------- */

boolean ConnectionSetTAFHandler
(
    OCI_Connection  *con,
    POCI_TAF_HANDLER handler
)
{
    CALL_ENTER(boolean, FALSE)
    CHECK_PTR(OCI_IPC_CONNECTION, con)
    CTX_SET_FROM_CON(con)

    RETVAL = ConnectionIsTAFCapable(con);

#if OCI_VERSION_COMPILE >= OCI_10_2

    if (RETVAL && OCILib.version_runtime >= OCI_10_2)
    {
        OCIFocbkStruct fo_struct;

        memset(&fo_struct, 0, sizeof(fo_struct));

        con->taf_handler = handler;

        if (con->taf_handler)
        {
            fo_struct.callback_function = (OCICallbackFailover) CallbackFailOver;
            fo_struct.fo_ctx            = (dvoid *) con;
        }

        ATTRIB_SET(OCI_HTYPE_SERVER, OCI_ATTR_FOCBK, con->svr, &fo_struct, 0);
    }

#endif

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionGetStatementCacheSize
 * --------------------------------------------------------------------------------------------- */

unsigned int ConnectionGetStatementCacheSize
(
    OCI_Connection  *con
)
{
    ub4 cache_size = 0;

    CALL_ENTER(unsigned int, 0)
    CHECK_PTR(OCI_IPC_CONNECTION, con)
    CTX_SET_FROM_CON(con)

#if OCI_VERSION_COMPILE >= OCI_9_2

    if (OCILib.version_runtime >= OCI_9_2)
    {
        ATTRIB_GET(OCI_HTYPE_SVCCTX, OCI_ATTR_STMTCACHESIZE, con->cxt, &cache_size, NULL);
    }

#endif

    RETVAL = cache_size;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionSetStatementCacheSize
 * --------------------------------------------------------------------------------------------- */

boolean ConnectionSetStatementCacheSize
(
    OCI_Connection  *con,
    unsigned int     value
)
{
    ub4 cache_size = value;

    CALL_ENTER(boolean, FALSE)
    CHECK_PTR(OCI_IPC_CONNECTION, con)
    CTX_SET_FROM_CON(con)

#if OCI_VERSION_COMPILE >= OCI_9_2

    if (OCILib.version_runtime >= OCI_9_2)
    {
        ATTRIB_SET(OCI_HTYPE_SVCCTX, OCI_ATTR_STMTCACHESIZE, con->cxt, &cache_size, sizeof(cache_size));
    }

#else

    OCI_NOT_USED(cache_size)

#endif

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionGetDefaultLobPrefetchSize
 * --------------------------------------------------------------------------------------------- */

unsigned int ConnectionGetDefaultLobPrefetchSize
(
    OCI_Connection *con
)
{
    ub4 prefetch_size = 0;

    CALL_ENTER(unsigned int, 0)
    CHECK_PTR(OCI_IPC_CONNECTION, con)
    CTX_SET_FROM_CON(con)

#if OCI_VERSION_COMPILE >= OCI_11_1

    if (ConnectionIsVersionSupported(con, OCI_11_1))
    {
        ATTRIB_GET(OCI_HTYPE_SESSION, OCI_ATTR_DEFAULT_LOBPREFETCH_SIZE, con->ses, &prefetch_size, NULL);
    }

#endif

    RETVAL = prefetch_size;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionSetDefaultLobPrefetchSize
 * --------------------------------------------------------------------------------------------- */

boolean ConnectionSetDefaultLobPrefetchSize
(
    OCI_Connection *con,
    unsigned int     value
)
{
    ub4 prefetch_size = value;

    CALL_ENTER(boolean, FALSE)
    CHECK_PTR(OCI_IPC_CONNECTION, con)
    CTX_SET_FROM_CON(con)

#if OCI_VERSION_COMPILE >= OCI_11_1

    if (ConnectionIsVersionSupported(con, OCI_11_1))
    {
        ATTRIB_SET(OCI_HTYPE_SESSION, OCI_ATTR_DEFAULT_LOBPREFETCH_SIZE, con->ses, &prefetch_size, sizeof(prefetch_size));
    }

#else

    OCI_NOT_USED(prefetch_size)

#endif

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionGetMaxCursors
 * --------------------------------------------------------------------------------------------- */

unsigned int ConnectionGetMaxCursors
(
    OCI_Connection *con
)
{
    ub4 max_cursors = 0;

    CALL_ENTER(unsigned int, 0)
    CHECK_PTR(OCI_IPC_CONNECTION, con)
    CTX_SET_FROM_CON(con)

#if OCI_VERSION_COMPILE >= OCI_12_1

    if (ConnectionIsVersionSupported(con, OCI_12_1))
    {
        ATTRIB_GET(OCI_HTYPE_SESSION, OCI_ATTR_MAX_OPEN_CURSORS, con->ses, &max_cursors, NULL);
    }

#endif

    RETVAL = max_cursors;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionExecuteImmediate
 * --------------------------------------------------------------------------------------------- */

boolean ConnectionExecuteImmediate
(
    OCI_Connection *con,
    const otext    *sql,
    va_list         args
)
{
    OCI_Statement *stmt = NULL;

    CALL_ENTER(boolean, FALSE)
    CHECK_PTR(OCI_IPC_CONNECTION, con)
    CHECK_PTR(OCI_IPC_STRING, sql)
    CTX_SET_FROM_CON(con)

    /* First, execute SQL */

    stmt = StatementCreate(con);
    STATUS = (NULL != stmt);
    
    if (STATUS)
    {
        STATUS = StatementExecuteStmt(stmt, sql);

        /* get resultset and set up variables */

        if (STATUS && (OCI_CST_SELECT == StatementGetStatementType(stmt)))
        {
            STATUS = StatementFetchIntoUserVariables(stmt, args);
        }

        StatementFree(stmt);
    }

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionExecuteImmediateFmt
 * --------------------------------------------------------------------------------------------- */

boolean ConnectionExecuteImmediateFmt
(
    OCI_Connection *con,
    const otext    *sql,
    va_list         args
)
{
    OCI_Statement  *stmt = NULL;

    va_list first_pass_args;
    va_list second_pass_args;

    CALL_ENTER(boolean, FALSE)
    CHECK_PTR(OCI_IPC_CONNECTION, con)
    CHECK_PTR(OCI_IPC_STRING, sql)
    CTX_SET_FROM_CON(con)

    va_copy(first_pass_args, args);
    va_copy(second_pass_args, args);

    stmt = StatementCreate(con);
    STATUS = (NULL != stmt);

    if (STATUS)
    {
        int     size = 0;

        /* first, get buffer size */

        size = FormatParseSql(stmt, NULL, sql, &first_pass_args);

        if (size > 0)
        {
            /* allocate buffer */

            otext  *sql_fmt = NULL;

            ALLOC_DATA(OCI_IPC_STRING, sql_fmt, size + 1)
    
            if (STATUS)
            {
                /* format buffer */

                if (FormatParseSql(stmt, sql_fmt, sql, &second_pass_args) > 0)
                {
                    /* prepare and execute SQL buffer */

                    STATUS = StatementPrepareInternal(stmt, sql_fmt) && StatementExecuteInternal(stmt, OCI_DEFAULT);

                    /* get resultset and set up variables */

                    if (STATUS && (OCI_CST_SELECT == StatementGetStatementType(stmt)))
                    {
                        STATUS = StatementFetchIntoUserVariables(stmt, second_pass_args);
                    }
                }

                FREE(sql_fmt)
            }
        }

        StatementFree(stmt);
    }

    RETVAL = STATUS;

    va_end(first_pass_args);
    va_end(second_pass_args);

    CALL_EXIT()
}

