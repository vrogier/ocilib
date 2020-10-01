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
#include "stringutils.h"
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

#define SET_TRACE(prop)                                                 \
                                                                        \
    con->trace->prop[0] = 0;                                            \
    if (value)                                                          \
    {                                                                   \
        ostrncat(con->trace->prop, value, osizeof(con->trace->prop)-1); \
        str = con->trace->prop;                                         \
    }

#define GET_TRACE(prop) \
                        \
    value = con->trace->prop[0] ? con->trace->prop : NULL;

/* --------------------------------------------------------------------------------------------- *
 * ConnectionDetachSubscriptions
 * --------------------------------------------------------------------------------------------- */

void ConnectionDetachSubscriptions
(
    OCI_Subscription *sub,
    OCI_Connection   *con
)
{
    ENTER_VOID
    (
        /* context */ OCI_IPC_CONNECTION, con
    )

    CHECK_PTR(OCI_IPC_NOTIFY,     sub)
    CHECK_PTR(OCI_IPC_CONNECTION, con)

    if (NULL != sub && (sub->con == con))
    {
        sub->con = NULL;

        sub->saved_db   = ostrdup(con->db);
        sub->saved_user = ostrdup(con->user);
        sub->saved_pwd  = ostrdup(con->pwd);
    }

    EXIT_VOID()
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
    ENTER_FUNC
    (
        /* returns */ OCI_Connection*, NULL,
        /* context */ (pool ? OCI_IPC_POOL : OCI_IPC_VOID), (pool ? (void*)pool : (void*)&Env)
    )

    /* create connection object */

    OCI_Connection *con = ListAppend(Env.cons, sizeof(*con));
    CHECK_NULL(con)

    con->alloc_handles = (0 == (mode & OCI_SESSION_XA));

    /* create internal lists */

    con->stmts = ListCreate(OCI_IPC_STATEMENT);
    CHECK_NULL(con->stmts)

    con->tinfs = ListCreate(OCI_IPC_TYPE_INFO);
    CHECK_NULL(con->tinfs)

    con->trsns = ListCreate(OCI_IPC_TRANSACTION);
    CHECK_NULL(con->trsns)

    /* set attributes */

    con->mode     = mode;
    con->pool     = pool;
    con->sess_tag = NULL;

    if (NULL != con->pool)
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
            THROW(ExceptionEnvFromXaString, con->db)
        }
    }
    else

#endif

    {
        con->env = Env.env;
    }

    /*  allocate error handle */

    CHECK
    (
        MemoryAllocHandle
        (
            (dvoid *)con->env,
            (dvoid **)(void *)&con->err,
            OCI_HTYPE_ERROR
        )
    )

    /* update internal status */

    con->cstate = OCI_CONN_ALLOCATED;

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE)
        {
            ConnectionFree(con);
            con = NULL;
        }

        SET_RETVAL(con)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionDeallocate
 * --------------------------------------------------------------------------------------------- */

boolean ConnectionDeallocate
(
    OCI_Connection *con
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_CONNECTION, con
    )

    CHECK_PTR(OCI_IPC_CONNECTION, con)
    CHECK_CON_STATUS(con, OCI_CONN_ALLOCATED)

    /* close error handle */

    if (NULL != con->err)
    {
        MemoryFreeHandle((dvoid*)con->err, OCI_HTYPE_ERROR);
    }

    /* close server handle (if it had been allocated) in case of login error */

    if (NULL != con->svr && con->alloc_handles)
    {
        MemoryFreeHandle((dvoid*)con->svr, OCI_HTYPE_SERVER);
    }

    con->cxt = NULL;
    con->ses = NULL;
    con->svr = NULL;
    con->err = NULL;

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionAttach
 * --------------------------------------------------------------------------------------------- */

boolean ConnectionAttach
(
    OCI_Connection *con
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_CONNECTION, con
    )

    dbtext* dbstr = NULL;
    int dbsize = -1;

    CHECK_PTR(OCI_IPC_CONNECTION, con)
    CHECK_CON_STATUS(con, OCI_CONN_ALLOCATED)

    /* allocate server handle for non session pooled connection */

    if (con->alloc_handles)
    {
        ub4 cmode = OCI_DEFAULT;

        CHECK(MemoryAllocHandle((dvoid *) con->env, (dvoid **) (void *) &con->svr, OCI_HTYPE_SERVER))

        /* attach server handle to service name */

#if OCI_VERSION_COMPILE >= OCI_9_0

        if (Env.version_runtime >= OCI_9_0 && con->pool)
        {
            dbstr = StringGetDBString(con->pool->name, &dbsize);
            cmode = OCI_CPOOL;
        }
        else

#endif

        {
            dbstr = StringGetDBString(con->db, &dbsize);
        }

        CHECK_OCI
        (
            con->err,
            OCIServerAttach,
            con->svr, con->err,
            (OraText *)dbstr, (sb4)dbsize, cmode
        )
    }

    /* update internal status */

    con->cstate = OCI_CONN_ATTACHED;

    SET_SUCCESS()

    CLEANUP_AND_EXIT_FUNC
    (
        StringReleaseDBString(dbstr);
    )
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionDetach
 * --------------------------------------------------------------------------------------------- */

boolean ConnectionDetach
(
    OCI_Connection *con
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_CONNECTION, con
    )

    CHECK_PTR(OCI_IPC_CONNECTION, con)
    CHECK_CON_STATUS(con, OCI_CONN_ATTACHED)

    if (con->alloc_handles && NULL != con->svr)
    {
        /* detach from the oracle server */

        CHECK_OCI
        (
            con->err,
            OCIServerDetach,
            con->svr, con->err, OCI_DEFAULT
        )

        /* close server handle */

        MemoryFreeHandle((dvoid*)con->svr, OCI_HTYPE_SERVER);

        con->svr = NULL;
    }

    /* update internal status */

    con->cstate = OCI_CONN_ALLOCATED;

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionLogonXA
 * --------------------------------------------------------------------------------------------- */

boolean ConnectionLogonXA
(
    OCI_Connection *con
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_CONNECTION, con
    )

    dbtext *dbstr_user = NULL;
    int  dbsize_user = 0;
    char dbname[OCI_SIZE_BUFFER + 1];

    CHECK_PTR(OCI_IPC_CONNECTION, con)

    memset(dbname, 0, sizeof(dbname));

    if (IS_STRING_VALID(con->db))
    {
        StringNativeToAnsi(con->db, dbname, (int) ostrlen(con->db));
    }

    con->cxt = xaoSvcCtx((OraText *) (dbname[0] ? dbname : NULL ));
    CHECK_NULL(con->cxt)

    CHECK_ATTRIB_GET
    (
        OCI_HTYPE_SVCCTX, OCI_ATTR_SERVER,
        con->cxt, &con->svr, NULL,
        con->err
    )

    CHECK_ATTRIB_GET
    (
        OCI_HTYPE_SVCCTX, OCI_ATTR_SESSION,
        con->cxt, &con->ses, NULL,
        con->err
    )

    CHECK_ATTRIB_GET
    (
        OCI_HTYPE_SESSION, OCI_ATTR_USERNAME,
        con->ses, &dbstr_user, &dbsize_user,
        con->err
    )

    if (NULL == con->ses)
    {
        THROW(ExceptionConnFromXaString, con->db)
    }

    if (dbstr_user)
    {
        FREE(con->user)

        con->user = StringDuplicateFromDBString(dbstr_user, dbcharcount(dbsize_user));
    }

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionLogonRegular
 * --------------------------------------------------------------------------------------------- */

boolean ConnectionLogonRegular
(
    OCI_Connection *con,
    const otext    *new_pwd
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_CONNECTION, con
    )

    int dbsize1 = -1;
    int dbsize2 = -1;
    int dbsize3 = -1;

    dbtext* dbstr1 = NULL;
    dbtext* dbstr2 = NULL;
    dbtext* dbstr3 = NULL;

    CHECK_PTR(OCI_IPC_CONNECTION, con)

    /* allocate session handle */

    CHECK(MemoryAllocHandle((dvoid *)con->env, (dvoid **)(void *)&con->ses, OCI_HTYPE_SESSION))

    /* allocate context handle */

    CHECK(MemoryAllocHandle((dvoid *)con->env, (dvoid **)(void *)&con->cxt, OCI_HTYPE_SVCCTX))

    /* set context server attribute */

    CHECK_ATTRIB_SET
    (
        OCI_HTYPE_SVCCTX, OCI_ATTR_SERVER,
        con->cxt, con->svr, sizeof(con->svr),
        con->err
    )

    /* modify user password if needed */

    if (IS_STRING_VALID(new_pwd))
    {
        dbstr1 = StringGetDBString(con->user, &dbsize1);
        dbstr2 = StringGetDBString(con->pwd, &dbsize2);
        dbstr3 = StringGetDBString(new_pwd, &dbsize3);

        CHECK_ATTRIB_SET
        (
            OCI_HTYPE_SVCCTX, OCI_ATTR_SESSION,
            con->cxt, con->ses,
            sizeof(con->ses),
            con->err
        )

        CHECK_OCI
        (
            con->err,
            OCIPasswordChange,
            con->cxt, con->err,
            (OraText *)dbstr1, (ub4)dbsize1,
            (OraText *)dbstr2, (ub4)dbsize2,
            (OraText *)dbstr3, (ub4)dbsize3,
            OCI_AUTH
        )

        /* replace connection password */

        FREE(con->pwd)
        con->pwd = ostrdup(new_pwd);
    }
    else
    {
        ub4 credt = OCI_CRED_RDBMS;
        ub4 mode  = con->mode;

        /* set session login attribute */

        if (IS_STRING_VALID(con->user))
        {
            dbsize1 = -1;
            dbstr1  = StringGetDBString(con->user, &dbsize1);

            CHECK_ATTRIB_SET
            (
                OCI_HTYPE_SESSION, OCI_ATTR_USERNAME,
                con->ses, dbstr1, dbsize1,
                con->err
            )
        }

        /* set session password attribute */

        if (IS_STRING_VALID(con->pwd))
        {
            dbsize2 = -1;
            dbstr2  = StringGetDBString(con->pwd, &dbsize2);

            CHECK_ATTRIB_SET
            (
                OCI_HTYPE_SESSION, OCI_ATTR_PASSWORD,
                con->ses, dbstr2, dbsize2,
                con->err
            )
        }

        /* set OCILIB driver layer name attribute */

#if OCI_VERSION_COMPILE >= OCI_11_1

        if (Env.version_runtime >= OCI_11_1)
        {
            otext driver_version[OCI_SIZE_FORMAT];

            osprintf(driver_version,
                     osizeof(driver_version) - (size_t)1,
                     OTEXT("%s : %d.%d.%d"),
                     OCILIB_DRIVER_NAME,
                     OCILIB_MAJOR_VERSION,
                     OCILIB_MINOR_VERSION,
                     OCILIB_REVISION_VERSION);

            dbsize3 = -1;
            dbstr3  = StringGetDBString(driver_version, &dbsize3);

            CHECK_ATTRIB_SET
            (
                OCI_HTYPE_SESSION, OCI_ATTR_DRIVER_NAME,
                con->ses, dbstr3, dbsize3,
                con->err
            )
        }

#endif

        /* start session */

        if (!IS_STRING_VALID(con->user) && !IS_STRING_VALID(con->pwd))
        {
            credt = OCI_CRED_EXT;
        }

#if OCI_VERSION_COMPILE >= OCI_9_2

        /* activate statement cache is the OCI version supports it */

        if (Env.version_runtime >= OCI_9_2)
        {
            mode |= OCI_STMT_CACHE;
        }

#endif

        /* start session */

        CHECK_OCI
        (
            con->err,
            OCISessionBegin,
            con->cxt, con->err, con->ses,
            credt, mode
        )

        CHECK_ATTRIB_SET
        (
            OCI_HTYPE_SVCCTX, OCI_ATTR_SESSION,
            con->cxt, con->ses, sizeof(con->ses),
            con->err
        )

        if (!(con->mode & OCI_PRELIM_AUTH))
        {
            /* create default transaction object */

            OCI_Transaction* trs = TransactionCreate(con, 1, OCI_TRANS_READWRITE, NULL);
            CHECK_NULL(trs)

            /* start transaction */

            CHECK(ConnectionSetTransaction(con, trs))
            CHECK(TransactionStart( trs))
        }
    }

    SET_SUCCESS()

    CLEANUP_AND_EXIT_FUNC
    (
        StringReleaseDBString(dbstr1);
        StringReleaseDBString(dbstr2);
        StringReleaseDBString(dbstr3);

        if (FAILURE && NULL != con)
        {
            /* could not start session, must free the session and context handles */

            MemoryFreeHandle((dvoid*)con->ses, OCI_HTYPE_SESSION);
            MemoryFreeHandle((dvoid*)con->cxt, OCI_HTYPE_SVCCTX);

            con->ses = NULL;
            con->cxt = NULL;
        }
    )
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionLogonSessionPool
 * --------------------------------------------------------------------------------------------- */

boolean ConnectionLogonSessionPool
(
    OCI_Connection *con,
    const otext    *tag
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_CONNECTION, con
    )

    ub4 sess_mode = OCI_SESSGET_SPOOL;
    boolean  found      = FALSE;
    int      dbsize     = -1;
    dbtext  *dbstr      = NULL;
    dbtext  *dbstr_tag  = NULL;
    int      dbsize_tag = 0;
    OraText *dbstr_ret  = NULL;
    ub4      dbsize_ret = 0;

#if OCI_VERSION_COMPILE >= OCI_10_2

    if (con->mode & OCI_SESSION_SYSDBA)
    {
        sess_mode |= OCI_SESSGET_SYSDBA;
    }

#endif

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

    CHECK_OCI
    (
        con->err,
        OCISessionGet,
        con->env, con->err, &con->cxt, NULL,
        (OraText  *)dbstr, (ub4)dbsize,
        (OraText *)dbstr_tag, dbsize_tag,
        (OraText **)&dbstr_ret, &dbsize_ret,
        &found, sess_mode
    )

    CHECK_ATTRIB_GET
    (
        OCI_HTYPE_SVCCTX, OCI_ATTR_SERVER,
        con->cxt, &con->svr, NULL,
        con->err
    )

    CHECK_ATTRIB_GET
    (
        OCI_HTYPE_SVCCTX, OCI_ATTR_SESSION,
        con->cxt, &con->ses, NULL,
        con->err
    )

    if (found)
    {
        CHECK(ConnectionSetSessionTag(con, tag))
    }

    SET_SUCCESS()

    CLEANUP_AND_EXIT_FUNC
    (
        StringReleaseDBString(dbstr);
        StringReleaseDBString(dbstr_tag);
    )
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
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_CONNECTION, con
    )

    CHECK_PTR(OCI_IPC_CONNECTION, con)

#if OCI_VERSION_COMPILE < OCI_9_2

    OCI_NOT_USED(tag)

#endif

    /* 1 - XA connection */

#if OCI_VERSION_COMPILE >= OCI_10_1

    if (con->mode & OCI_SESSION_XA)
    {
        CHECK(ConnectionLogonXA(con))
    }
    else

#endif

    /* 2 - regular connection and connection from connection pool */

    if (con->alloc_handles)
    {
        CHECK(ConnectionLogonRegular(con, new_pwd))
    }

#if OCI_VERSION_COMPILE >= OCI_9_2

    /* 3 - connection from session pool */

    else if (Env.version_runtime >= OCI_9_2)
    {
        CHECK(ConnectionLogonSessionPool(con, tag))
    }

#endif

    /* get server version */

    ConnectionGetServerVersion(con);

    /* update internal status */

    con->cstate = OCI_CONN_LOGGED;

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionLogoffRegular
 * --------------------------------------------------------------------------------------------- */

boolean ConnectionLogoffRegular
(
    OCI_Connection *con
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_CONNECTION, con
    )

    CHECK_PTR(OCI_IPC_CONNECTION, con)

    /* close any server files not explicitly closed - no check of return code */

    if (NULL != con->cxt && NULL != con->err && NULL != con->ses)
    {
        CHECK_OCI
        (
            con->err,
            OCISessionEnd,
            con->cxt, con->err, con->ses,
            (ub4)OCI_DEFAULT
        )

        /* close session handle */

        if (con->ses)
        {
            MemoryFreeHandle((dvoid*)con->ses, OCI_HTYPE_SESSION);
            con->ses = NULL;
        }

        /* close context handle */

        if (con->cxt)
        {
            MemoryFreeHandle((dvoid*)con->cxt, OCI_HTYPE_SVCCTX);
            con->cxt = NULL;
        }
    }

    SET_SUCCESS()

    EXIT_FUNC()

}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionLogoffSessionPool
 * --------------------------------------------------------------------------------------------- */

boolean ConnectionLogoffSessionPool
(
    OCI_Connection *con
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_CONNECTION, con
    )

    dbtext* dbstr = NULL;
    int dbsize = 0;

    CHECK_PTR(OCI_IPC_CONNECTION, con)

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

    if (Env.version_runtime >= OCI_9_2)
    {
        ub4 mode = OCI_DEFAULT;

        /* Clear session tag if connection was retrieved from session pool */

        if (NULL != con->pool && NULL != con->sess_tag && ( OCI_HTYPE_SPOOL == con->pool->htype))
        {
            dbsize = -1;
            dbstr  = StringGetDBString(con->sess_tag, &dbsize);
            mode   = OCI_SESSRLS_RETAG;
        }

        CHECK_OCI
        (
            con->err,
            OCISessionRelease,
            con->cxt, con->err,
            (OraText*)dbstr, (ub4)dbsize,
            mode
        )

        con->cxt = NULL;
        con->ses = NULL;
        con->svr = NULL;
    }

#endif

    SET_SUCCESS()

    CLEANUP_AND_EXIT_FUNC
    (
        StringReleaseDBString(dbstr);
    )
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionLogOff
 * --------------------------------------------------------------------------------------------- */

boolean ConnectionLogOff
(
    OCI_Connection *con
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_CONNECTION, con
    )

    CHECK_PTR(OCI_IPC_CONNECTION, con)
    CHECK_CON_STATUS(con, OCI_CONN_LOGGED)

    /* close opened files */

    if (con->nb_files > 0)
    {
        OCILobFileCloseAll(con->cxt, con->err);
    }

    /* dissociate connection from existing subscriptions */

    ListForEachWithParam(Env.subs, con, (POCI_LIST_FOR_EACH_WITH_PARAM) ConnectionDetachSubscriptions);

    /* free all statements */

    ListForEach(con->stmts, (POCI_LIST_FOR_EACH)StatementDispose);
    ListClear(con->stmts);

    /* free all type info objects */

    ListForEach(con->tinfs, (POCI_LIST_FOR_EACH)TypeInfoDispose);
    ListClear(con->tinfs);

    /* free all transactions */

    ListForEach(con->trsns, (POCI_LIST_FOR_EACH)TransactionDispose);
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
        ConnectionLogoffRegular(con);
    }

#if OCI_VERSION_COMPILE >= OCI_9_2

    /* 3 - connection from session pool */

    else if (Env.version_runtime >= OCI_9_0)
    {
        ConnectionLogoffSessionPool(con);
    }

#endif

    /* update internal status */

    con->cstate = OCI_CONN_ATTACHED;

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionClose
 * --------------------------------------------------------------------------------------------- */

boolean ConnectionDispose
(
    OCI_Connection *con
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_CONNECTION, con
    )

    unsigned int i = 0;

    CHECK_PTR(OCI_IPC_CONNECTION, con)

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

    if (NULL != con->inst_startup)
    {
        TimestampFree(con->inst_startup);
    }

    con->stmts = NULL;
    con->trsns = NULL;
    con->tinfs = NULL;

    ErrorResetSource(NULL, con);

    SET_SUCCESS()

    EXIT_FUNC()
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
    ENTER_FUNC
    (
        /* returns */ OCI_Connection*, NULL,
        /* context */ (pool ? OCI_IPC_POOL : OCI_IPC_VOID), (pool ? (void*)pool : (void*)&Env)
    )

    /* create connection */

    OCI_Connection *con = ConnectionAllocate(pool, db, user, pwd, mode);
    CHECK_NULL(con)

    CHECK(ConnectionAttach(con))
    CHECK(ConnectionLogon(con, NULL, tag))

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE)
        {
            ConnectionFree(con);
            con = NULL;
        }

        SET_RETVAL(con)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionGetMinSupportedVersion
 * --------------------------------------------------------------------------------------------- */

unsigned int ConnectionGetMinSupportedVersion
(
    OCI_Connection *con
)
{
    ENTER_FUNC
    (
        /* returns */ unsigned int, OCI_UNKNOWN,
        /* context */ OCI_IPC_CONNECTION, con
    )

    CHECK_PTR(OCI_IPC_CONNECTION, con)

    SET_RETVAL((Env.version_runtime > con->ver_num) ? con->ver_num : Env.version_runtime)

    EXIT_FUNC()
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
    ENTER_FUNC
    (
        /* returns */ OCI_Connection*, NULL,
        /* context */ OCI_IPC_VOID, &Env
    )

    CHECK_INITIALIZED()
    CHECK_XA_ENABLED(mode)

    SET_RETVAL(ConnectionCreateInternal(NULL, db, user, pwd, mode, NULL))

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionFree
 * --------------------------------------------------------------------------------------------- */

boolean ConnectionFree
(
    OCI_Connection *con
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_CONNECTION, con
    )

    CHECK_PTR(OCI_IPC_CONNECTION, con)

    ConnectionDispose(con);
    ListRemove(Env.cons, con);

    FREE(con)

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionCommit
 * --------------------------------------------------------------------------------------------- */

boolean ConnectionCommit
(
    OCI_Connection *con
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_CONNECTION, con
    )

    CHECK_PTR(OCI_IPC_CONNECTION, con)

    CHECK_OCI
    (
        con->err,
        OCITransCommit,
        con->cxt, con->err, (ub4)OCI_DEFAULT
    )

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionRollback
 * --------------------------------------------------------------------------------------------- */

boolean ConnectionRollback
(
    OCI_Connection *con
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_CONNECTION, con
    )

    CHECK_PTR(OCI_IPC_CONNECTION, con)

    CHECK_OCI
    (
        con->err,
        OCITransRollback,
        con->cxt, con->err, (ub4)OCI_DEFAULT
    )

    SET_SUCCESS()

    EXIT_FUNC()
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
    SET_PROP
    (
        /* handle */ OCI_IPC_CONNECTION, con,
        /* member */ autocom, boolean,
        /* value  */ enable
    )
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionGetAutoCommit
 * --------------------------------------------------------------------------------------------- */

boolean ConnectionGetAutoCommit
(
    OCI_Connection *con
)
{
    GET_PROP
    (
        /* result */ boolean, FALSE,
        /* handle */ OCI_IPC_CONNECTION, con,
        /* member */ autocom
    )
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionIsConnected
 * --------------------------------------------------------------------------------------------- */

boolean ConnectionIsConnected
(
    OCI_Connection *con
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_CONNECTION, con
    )

    ub4 status = 0;
    ub4 size = (ub4) sizeof(status);

    CHECK_PTR(OCI_IPC_CONNECTION, con)

    CHECK_ATTRIB_GET
    (
        OCI_HTYPE_SERVER, OCI_ATTR_SERVER_STATUS,
        con->svr, &status, &size,
        con->err
    )

    SET_RETVAL(status == OCI_SERVER_NORMAL)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionGetUserData
 * --------------------------------------------------------------------------------------------- */

void * ConnectionGetUserData
(
    OCI_Connection *con
)
{
    ENTER_FUNC
    (
        /* returns */ void*, NULL,
        /* context */ OCI_IPC_CONNECTION, con
    )

    CHECK_INITIALIZED()

    SET_RETVAL((void*) (con ? con->usrdata : Env.usrdata))

    EXIT_FUNC()
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
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_CONNECTION, con
    )

    CHECK_INITIALIZED()

    if (NULL != con)
    {
        con->usrdata = data;
    }
    else
    {
        Env.usrdata = data;
    }

    SET_SUCCESS()

    EXIT_FUNC()
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
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_CONNECTION, con
    )

    CHECK_PTR(OCI_IPC_CONNECTION, con)

    FREE(con->sess_tag)

#if OCI_VERSION_COMPILE >= OCI_9_2

    if (tag && con->pool && (OCI_HTYPE_SPOOL == con->pool->htype))
    {
        con->sess_tag = ostrdup(tag);
        CHECK_NULL(con->sess_tag)
    }

#else

    OCI_NOT_USED(tag)

#endif

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionGetSessionTag
 * --------------------------------------------------------------------------------------------- */

const otext * ConnectionGetSessionTag
(
    OCI_Connection *con
)
{
    GET_PROP
    (
        /* result */ const otext*, NULL,
        /* handle */ OCI_IPC_CONNECTION, con,
        /* member */ sess_tag
    )
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionGetDatabase
 * --------------------------------------------------------------------------------------------- */

const otext * ConnectionGetConnectionString
(
    OCI_Connection *con
)
{
    GET_PROP
    (
        /* result */ const otext*, NULL,
        /* handle */ OCI_IPC_CONNECTION, con,
        /* member */ db
    )
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionGetUserName
 * --------------------------------------------------------------------------------------------- */

const otext * ConnectionGetUserName
(
    OCI_Connection *con
)
{
    GET_PROP
    (
        /* result */ const otext*, NULL,
        /* handle */ OCI_IPC_CONNECTION, con,
        /* member */ user
    )
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionGetPassword
 * --------------------------------------------------------------------------------------------- */

const otext * ConnectionGetPassword
(
    OCI_Connection *con
)
{
    GET_PROP
    (
        /* result */ const otext*, NULL,
        /* handle */ OCI_IPC_CONNECTION, con,
        /* member */ pwd
    )
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
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_CONNECTION, con
    )

    int dbsize1 = -1;
    int dbsize2 = -1;
    int dbsize3 = -1;

    dbtext* dbstr1 = StringGetDBString(con->user, &dbsize1);
    dbtext* dbstr2 = StringGetDBString(con->pwd, &dbsize2);
    dbtext* dbstr3 = StringGetDBString(password, &dbsize3);

    CHECK_PTR(OCI_IPC_CONNECTION, con)
    CHECK_PTR(OCI_IPC_STRING,     password)

    if (OCI_CONN_LOGGED != con->cstate)
    {
        CHECK(ConnectionLogon(con, password, NULL))
    }
    else
    {
        CHECK_OCI
        (
            con->err,
            OCIPasswordChange,
            con->cxt, con->err,
            (OraText *) dbstr1, (ub4) dbsize1,
            (OraText *) dbstr2, (ub4) dbsize2,
            (OraText *) dbstr3, (ub4) dbsize3,
            OCI_DEFAULT
        )
    }

    SET_SUCCESS()

    CLEANUP_AND_EXIT_FUNC
    (
        StringReleaseDBString(dbstr1);
        StringReleaseDBString(dbstr2);
        StringReleaseDBString(dbstr3);
    )
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionGetSessionMode
 * --------------------------------------------------------------------------------------------- */

unsigned int ConnectionGetSessionMode
(
    OCI_Connection *con
)
{
    GET_PROP
    (
        /* result */ unsigned int, OCI_UNKNOWN,
        /* handle */ OCI_IPC_CONNECTION, con,
        /* member */ mode
    )
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionGetVersionServer
 * --------------------------------------------------------------------------------------------- */

const otext * ConnectionGetServerVersion
(
    OCI_Connection *con
)
{
    ENTER_FUNC
    (
        /* returns */ const otext*, NULL,
        /* context */ OCI_IPC_CONNECTION, con
    )

    int dbsize = OCI_SIZE_BUFFER * (int)sizeof(dbtext);
    dbtext* dbstr = NULL;

    CHECK_PTR(OCI_IPC_CONNECTION, con)

    /* no version available in preliminary authentication mode */

    if (!con->ver_str && (!(con->mode & OCI_PRELIM_AUTH)))
    {
        int ver_maj = 0;
        int ver_min = 0;
        int ver_rev = 0;

        ALLOC_DATA(OCI_IPC_STRING, con->ver_str, OCI_SIZE_BUFFER + 1)

        dbstr = StringGetDBString(con->ver_str, &dbsize);

#if OCI_VERSION_COMPILE >= OCI_18_1

        ub4 version = 0;

        if (Env.version_runtime >= OCI_18_1)
        {
            CHECK_OCI
            (
                con->err,
                OCIServerRelease2,
                (dvoid *)con->cxt, con->err, (OraText *)dbstr, (ub4)dbsize,
                (ub1)OCI_HTYPE_SVCCTX, &version, OCI_DEFAULT
            )
        }
        else

#endif

        {
            CHECK_OCI
            (
                con->err,
                OCIServerVersion,
                (dvoid *)con->cxt, con->err,
                (OraText *)dbstr, (ub4)dbsize,
                (ub1)OCI_HTYPE_SVCCTX
            )
        }

        StringCopyDBStringToNativeString(dbstr, con->ver_str, dbcharcount(dbsize));

#if OCI_VERSION_COMPILE >= OCI_18_1

        if (Env.version_runtime >= OCI_18_1)
        {
            ver_maj = OCI_SERVER_RELEASE_REL(version);
            ver_min = OCI_SERVER_RELEASE_REL_UPD(version);
            ver_rev = OCI_SERVER_RELEASE_REL_UPD_REV(version);

            con->ver_num = ver_maj * 100 + ver_min * 10 + ver_rev;
        }
        else

#endif

        {
            otext* p = NULL;

            con->ver_str[ocharcount(dbsize)] = 0;

            /* parse server version string to find the version information
                **/

            for (p = con->ver_str; p && *p; p++)
            {
                if (oisdigit((unsigned char)*p) &&
                    (*(p + (size_t)1) != 0) &&
                    (*(p + (size_t)1) == OTEXT('.') || (*(p + (size_t)2) == OTEXT('.'))))
                {
                    if (NB_ARG_VERSION == osscanf(p, OTEXT("%d.%d.%d"),
                                                  (int*)&ver_maj,
                                                  (int*)&ver_min,
                                                  (int*)&ver_rev))
                    {
                        con->ver_num = ver_maj * 100 + ver_min * 10 + ver_rev;
                    }

                    break;
                }
            }
        }
    }

    SET_RETVAL((const otext*)con->ver_str)

    CLEANUP_AND_EXIT_FUNC
    (
        StringReleaseDBString(dbstr);

        if (FAILURE)
        {
            FREE(con->ver_str)
        }
    )
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionGetServerMajorVersion
 * --------------------------------------------------------------------------------------------- */

unsigned int ConnectionGetServerMajorVersion
(
    OCI_Connection *con
)
{
    ENTER_FUNC
    (
        /* returns */ unsigned int, OCI_UNKNOWN,
        /* context */ OCI_IPC_CONNECTION, con
    )

    CHECK_PTR(OCI_IPC_CONNECTION, con)

    if (OCI_UNKNOWN == con->ver_num)
    {
        ConnectionGetServerVersion(con);
    }

    SET_RETVAL((unsigned int) OCI_VER_MAJ(con->ver_num))

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionGetServerMinorVersion
 * --------------------------------------------------------------------------------------------- */

unsigned int ConnectionGetServerMinorVersion
(
    OCI_Connection *con
)
{
    ENTER_FUNC
    (
        /* returns */ unsigned int, OCI_UNKNOWN,
        /* context */ OCI_IPC_CONNECTION, con
    )

    CHECK_PTR(OCI_IPC_CONNECTION, con)

    if (OCI_UNKNOWN == con->ver_num)
    {
        ConnectionGetServerVersion(con);
    }

    SET_RETVAL((unsigned int) OCI_VER_MIN(con->ver_num))

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionGetServerRevisionVersion
 * --------------------------------------------------------------------------------------------- */

unsigned int ConnectionGetServerRevisionVersion
(
    OCI_Connection *con
)
{
    ENTER_FUNC
    (
        /* returns */ unsigned int, OCI_UNKNOWN,
        /* context */ OCI_IPC_CONNECTION, con
    )

    CHECK_PTR(OCI_IPC_CONNECTION, con)

    if (OCI_UNKNOWN == con->ver_num)
    {
        ConnectionGetServerVersion(con);
    }

    SET_RETVAL((unsigned int) OCI_VER_REV(con->ver_num))

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionGetTransaction
 * --------------------------------------------------------------------------------------------- */

OCI_Transaction * ConnectionGetTransaction
(
    OCI_Connection *con
)
{
    GET_PROP
    (
        /* result */ OCI_Transaction*, NULL,
        /* handle */ OCI_IPC_CONNECTION, con,
        /* member */ trs
    )
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
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_CONNECTION, con
    )

    CHECK_PTR(OCI_IPC_CONNECTION,  con)
    CHECK_PTR(OCI_IPC_TRANSACTION, trans)

    if (NULL != con->trs)
    {
        CHECK(TransactionStop( con->trs))
    }

    CHECK_ATTRIB_SET
    (
        OCI_HTYPE_SVCCTX, OCI_ATTR_TRANS,
        con->cxt, trans->htr, sizeof(trans->htr),
        con->err
    )

    con->trs = trans;

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionGetVersion
 * --------------------------------------------------------------------------------------------- */

unsigned int ConnectionGetVersion
(
    OCI_Connection *con
)
{
    ENTER_FUNC
    (
        /* returns */ unsigned int, OCI_UNKNOWN,
        /* context */ OCI_IPC_CONNECTION, con
    )

    CHECK_PTR(OCI_IPC_CONNECTION, con)

    /* return the minimum supported version */

    SET_RETVAL(ConnectionGetMinSupportedVersion(con))

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionBreak
 * --------------------------------------------------------------------------------------------- */

boolean ConnectionBreak
(
    OCI_Connection *con
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_CONNECTION, con
    )

    CHECK_PTR(OCI_IPC_CONNECTION, con)

    CHECK_OCI
    (
        con->err,
        OCIBreak,
        (dvoid*)con->cxt, con->err
    )

    SET_SUCCESS()

    EXIT_FUNC()
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
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_CONNECTION, con
    )

    CHECK_PTR(OCI_IPC_CONNECTION, con)

    /* initialize the output buffer on server side */

    ALLOC_DATA(OCI_IPC_SERVER_OUPUT, con->svopt, 1)

    /* allocation internal buffer if needed */

    if (NULL == con->svopt->arrbuf)
    {
        const unsigned int charsize = sizeof(otext);

        /* check parameter ranges ( Oracle 10g increased the size of output line */

        if (con->ver_num >= OCI_10_2 && lnsize > OCI_OUPUT_LSIZE_10G)
        {
            lnsize = OCI_OUPUT_LSIZE_10G;
        }
        else if (lnsize > OCI_OUPUT_LSIZE)
        {
            lnsize = OCI_OUPUT_LSIZE;
        }

        con->svopt->arrsize = arrsize;
        con->svopt->lnsize  = lnsize;

        /* allocate internal string (line) array */

        ALLOC_BUFFER(OCI_IPC_STRING, con->svopt->arrbuf, (con->svopt->lnsize + 1) * charsize, con->svopt->arrsize)
    }

    if (NULL == con->svopt->stmt)
    {
        con->svopt->stmt = StatementCreate(con);
        CHECK_NULL(con->svopt->stmt)
    }

    /* enable server output */

    CHECK(StatementPrepare(con->svopt->stmt, OTEXT("BEGIN DBMS_OUTPUT.ENABLE(:n); END;")))
    CHECK(StatementBindUnsignedInt(con->svopt->stmt, OTEXT(":n"), &bufsize))

    if (0 == bufsize)
    {
        OCI_Bind* bnd = StatementGetBind(con->svopt->stmt, 1);
        CHECK_NULL(bnd);

        CHECK(OcilibBindSetNull(bnd))
    }

    CHECK(StatementExecute(con->svopt->stmt))

    /* prepare the retrieval statement call */

    con->svopt->cursize = con->svopt->arrsize;

    CHECK(StatementPrepare(con->svopt->stmt, OTEXT("BEGIN DBMS_OUTPUT.GET_LINES(:s, :i); END;")))

    CHECK
    (
        StatementBindArrayOfStrings
        (
            con->svopt->stmt, OTEXT(":s"),
            (otext *) con->svopt->arrbuf,
            con->svopt->lnsize,
            con->svopt->arrsize
        )
    )

    CHECK
    (
        StatementBindUnsignedInt
        (
            con->svopt->stmt, OTEXT(":i"),
            &con->svopt->cursize
        )
    )

    SET_SUCCESS()

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE)
        {
            ConnectionDisableServerOutput(con);
        }
    )
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionServerDisableOutput
 * --------------------------------------------------------------------------------------------- */

boolean ConnectionDisableServerOutput
(
    OCI_Connection *con
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_CONNECTION, con
    )

    CHECK_PTR(OCI_IPC_CONNECTION, con)

    if (NULL != con->svopt)
    {
        StatementExecuteStmt(con->svopt->stmt, OTEXT("BEGIN DBMS_OUTPUT.DISABLE(); END;"));

        if (NULL != con->svopt->stmt)
        {
            StatementFree(con->svopt->stmt);
            con->svopt->stmt = NULL;
        }

        FREE(con->svopt->arrbuf)
        FREE(con->svopt)
    }

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionServerGetOutput
 * --------------------------------------------------------------------------------------------- */

const otext * ConnectionGetServerOutput
(
    OCI_Connection *con
)
{
    ENTER_FUNC
    (
        /* returns */ const otext*, NULL,
        /* context */ OCI_IPC_CONNECTION, con
    )

    CHECK_PTR(OCI_IPC_CONNECTION, con)

    const otext *line = NULL;

    if (NULL != con->svopt)
    {
        if (0 == con->svopt->curpos || con->svopt->curpos >= con->svopt->cursize)
        {
            con->svopt->cursize = con->svopt->arrsize;

            CHECK(StatementExecute(con->svopt->stmt))

            con->svopt->curpos = 0;
        }

        if (con->svopt->cursize > 0)
        {
            const unsigned int charsize = sizeof(otext);

            line = (const otext*)(con->svopt->arrbuf + (size_t)(((con->svopt->lnsize + 1) * charsize) * con->svopt->curpos++));
        }
    }

    SET_RETVAL(line)

    EXIT_FUNC()
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
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_CONNECTION, con
    )

    dbtext* dbstr = NULL;
    int dbsize = 0;

    ub4 attrib = OCI_UNKNOWN;

    const otext* str = NULL;

    CHECK_PTR(OCI_IPC_CONNECTION, con)
    CHECK_ENUM_VALUE(trace, TraceTypeValues, OTEXT("Trace Type"))

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

    if (attrib != OCI_UNKNOWN)
    {
        if (NULL != str)
        {
            dbsize = -1;
            dbstr  = StringGetDBString(str, &dbsize);
        }

        CHECK_ATTRIB_SET
        (
            OCI_HTYPE_SESSION, attrib,
            con->ses, dbstr, dbsize,
            con->err
        )
    }

#endif

    SET_SUCCESS()

    CLEANUP_AND_EXIT_FUNC
    (
        StringReleaseDBString(dbstr);
    )
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
    ENTER_FUNC
    (
        /* returns */ const otext*, NULL,
        /* context */ OCI_IPC_CONNECTION, con
    )

    CHECK_PTR(OCI_IPC_CONNECTION, con)
    CHECK_ENUM_VALUE(trace, TraceTypeValues, OTEXT("Trace Type"))

    const otext *value = NULL;

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

    SET_RETVAL(value)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionPing
 * --------------------------------------------------------------------------------------------- */

boolean ConnectionPing
(
    OCI_Connection *con
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_CONNECTION, con
    )

    CHECK_PTR(OCI_IPC_CONNECTION, con)

#if OCI_VERSION_COMPILE >= OCI_10_2

    if (Env.version_runtime >= OCI_10_2)
    {
        CHECK_OCI
        (
            con->err,
            OCIPing, con->cxt, con->err,
            (ub4)OCI_DEFAULT
        )
    }

#endif

    SET_SUCCESS()

    EXIT_FUNC()
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
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_CONNECTION, con
    )

    CHECK_PTR(OCI_IPC_CONNECTION, con)
    CHECK_ENUM_VALUE(type, TimeoutTypeValues, OTEXT("timeout type"))

    boolean success = FALSE;

#if OCI_VERSION_COMPILE >= OCI_12_1

    ub4 timeout = value;

    if (Env.version_runtime >= OCI_12_1)
    {
        switch (type)
        {
            case OCI_NTO_SEND:
            {
                CHECK_ATTRIB_SET
                (
                    OCI_HTYPE_SERVER, OCI_ATTR_SEND_TIMEOUT,
                    con->svr, &timeout, sizeof(timeout),
                    con->err
                )
                success = TRUE;
                break;
            }
            case OCI_NTO_RECEIVE:
            {
                CHECK_ATTRIB_SET
                (
                    OCI_HTYPE_SERVER, OCI_ATTR_RECEIVE_TIMEOUT,
                    con->svr, &timeout, sizeof(timeout),
                    con->err
                )
                success = TRUE;
                break;
            }

  #if OCI_VERSION_COMPILE >= OCI_18_1

            case OCI_NTO_CALL:
            {
                if (Env.version_runtime >= OCI_18_1)
                {
                    CHECK_ATTRIB_SET
                    (
                        OCI_HTYPE_SVCCTX, OCI_ATTR_CALL_TIMEOUT,
                        con->cxt, &timeout, sizeof(timeout),
                        con->err
                    )
                    success = TRUE;
                }
                break;
            }

  #endif

        }
    }

#endif

    SET_RETVAL(success)

    EXIT_FUNC()
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
    ENTER_FUNC
    (
        /* returns */ unsigned int, 0,
        /* context */ OCI_IPC_CONNECTION, con
    )

    ub4 timeout = 0;

    CHECK_PTR(OCI_IPC_CONNECTION, con)
    CHECK_ENUM_VALUE(type, TimeoutTypeValues, OTEXT("timeout type"))

#if OCI_VERSION_COMPILE >= OCI_12_1

    if (Env.version_runtime >= OCI_12_1)
    {
        switch (type)
        {
            case OCI_NTO_SEND:
            {
                CHECK_ATTRIB_SET
                (
                    OCI_HTYPE_SERVER, OCI_ATTR_SEND_TIMEOUT,
                    con->svr, &timeout, sizeof(timeout),
                    con->err
                )
                break;
            }
            case OCI_NTO_RECEIVE:
            {
                CHECK_ATTRIB_SET
                (
                    OCI_HTYPE_SERVER, OCI_ATTR_RECEIVE_TIMEOUT,
                    con->svr, &timeout, sizeof(timeout),
                    con->err
                )
                break;
            }

  #if OCI_VERSION_COMPILE >= OCI_18_1

            case OCI_NTO_CALL:
            {
                if (Env.version_runtime >= OCI_18_3)
                {
                    CHECK_ATTRIB_SET
                    (
                        OCI_HTYPE_SVCCTX, OCI_ATTR_CALL_TIMEOUT,
                        con->cxt, &timeout, sizeof(timeout),
                        con->err
                    )
                }
                break;
            }

  #endif

        }
    }

#endif

    SET_RETVAL(timeout)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionGetDBName
 * --------------------------------------------------------------------------------------------- */

const otext * ConnectionGetDatabaseName
(
    OCI_Connection *con
)
{
    ENTER_FUNC
    (
        /* returns */ const otext *, NULL,
        /* context */ OCI_IPC_CONNECTION, con
    )

    CHECK_PTR(OCI_IPC_CONNECTION, con)

#if OCI_VERSION_COMPILE >= OCI_10_2

    if (NULL == con->db_name)
    {
        unsigned int size = 0;

        CHECK(StringGetAttribute(con, con->svr, OCI_HTYPE_SERVER,
                                 OCI_ATTR_DBNAME, &con->db_name, &size))
    }

#endif

    SET_RETVAL((const otext *) con->db_name)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionGetInstanceName
 * --------------------------------------------------------------------------------------------- */

const otext * ConnectionGetInstanceName
(
    OCI_Connection *con
)
{
    ENTER_FUNC
    (
        /* returns */ const otext*, NULL,
        /* context */ OCI_IPC_CONNECTION, con
    )
    CHECK_PTR(OCI_IPC_CONNECTION, con)

#if OCI_VERSION_COMPILE >= OCI_10_2

    if (NULL == con->inst_name)
    {
        unsigned int size = 0;

        CHECK(StringGetAttribute(con, con->svr, OCI_HTYPE_SERVER,
                                 OCI_ATTR_INSTNAME, &con->inst_name, &size))
    }

#endif

    SET_RETVAL((const otext *)con->inst_name)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionGetServiceName
 * --------------------------------------------------------------------------------------------- */

const otext * ConnectionGetServiceName
(
    OCI_Connection *con
)
{
    ENTER_FUNC
    (
        /* returns */ const otext*, NULL,
        /* context */ OCI_IPC_CONNECTION, con
    )
    CHECK_PTR(OCI_IPC_CONNECTION, con)

#if OCI_VERSION_COMPILE >= OCI_10_2

    if (NULL == con->service_name)
    {
        unsigned int size = 0;

        CHECK(StringGetAttribute(con, con->svr, OCI_HTYPE_SERVER,
                                 OCI_ATTR_SERVICENAME, &con->service_name, &size))
    }

#endif

    SET_RETVAL((const otext *)con->service_name)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionGetServerName
 * --------------------------------------------------------------------------------------------- */

const otext * ConnectionGetServerName
(
    OCI_Connection *con
)
{
    ENTER_FUNC
    (
        /* returns */ const otext*, NULL,
        /* context */ OCI_IPC_CONNECTION, con
    )

    CHECK_PTR(OCI_IPC_CONNECTION, con)

#if OCI_VERSION_COMPILE >= OCI_10_2

    if (NULL == con->server_name)
    {
        unsigned int size = 0;

        CHECK(StringGetAttribute(con, con->svr, OCI_HTYPE_SERVER,
                                 OCI_ATTR_HOSTNAME, &con->server_name, &size))
    }

#endif

    SET_RETVAL((const otext *)con->server_name)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionGetDomainName
 * --------------------------------------------------------------------------------------------- */

const otext * ConnectionGetDomainName
(
    OCI_Connection *con
)
{
    ENTER_FUNC
    (
        /* returns */ const otext*, NULL,
        /* context */ OCI_IPC_CONNECTION, con
    )
    CHECK_PTR(OCI_IPC_CONNECTION, con)

#if OCI_VERSION_COMPILE >= OCI_10_2

    if (NULL == con->domain_name)
    {
        unsigned int size = 0;

        CHECK(StringGetAttribute(con, con->svr, OCI_HTYPE_SERVER,
                                 OCI_ATTR_DBDOMAIN, &con->domain_name, &size))
    }

#endif

    SET_RETVAL((const otext *)con->domain_name)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionGetInstanceStartTime
 * --------------------------------------------------------------------------------------------- */

OCI_Timestamp * ConnectionGetInstanceStartTime
(
    OCI_Connection *con
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Timestamp*, NULL,
        /* context */ OCI_IPC_CONNECTION, con
    )

    CHECK_PTR(OCI_IPC_CONNECTION, con)

#if OCI_VERSION_COMPILE >= OCI_10_2

    if (NULL == con->inst_startup)
    {
        OCIDateTime *handle = NULL;

        CHECK_ATTRIB_GET
        (
            OCI_HTYPE_SERVER, OCI_ATTR_INSTSTARTTIME,
            con->svr, &handle, NULL,
            con->err
        )

        con->inst_startup = TimestampInitialize(con, NULL, handle, OCI_TIMESTAMP);
        CHECK_NULL(con->inst_startup)
    }

#endif

    SET_RETVAL(con->inst_startup)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionIsTAFCapable
 * --------------------------------------------------------------------------------------------- */

boolean ConnectionIsTAFCapable
(
    OCI_Connection *con
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_CONNECTION, con
    )

    boolean value = FALSE;

    CHECK_PTR(OCI_IPC_CONNECTION, con)

#if OCI_VERSION_COMPILE >= OCI_10_2

    if (Env.version_runtime >= OCI_10_2)
    {
        CHECK_ATTRIB_GET
        (
            OCI_HTYPE_SERVER, OCI_ATTR_TAF_ENABLED,
            con->svr, &value, NULL,
            con->err
        )
    }

#endif

    SET_RETVAL(value)

    EXIT_FUNC()
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
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_CONNECTION, con
    )

    CHECK_PTR(OCI_IPC_CONNECTION, con)

    CHECK(ConnectionIsTAFCapable(con))

#if OCI_VERSION_COMPILE >= OCI_10_2

    if (Env.version_runtime >= OCI_10_2)
    {
        OCIFocbkStruct fo_struct;

        memset(&fo_struct, 0, sizeof(fo_struct));

        con->taf_handler = handler;

        if (con->taf_handler)
        {
            fo_struct.callback_function = (OCICallbackFailover) CallbackFailOver;
            fo_struct.fo_ctx            = (dvoid *) con;
        }

        CHECK_ATTRIB_SET
        (
            OCI_HTYPE_SERVER, OCI_ATTR_FOCBK,
            con->svr, &fo_struct, 0,
            con->err
        )
    }

#endif

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionGetStatementCacheSize
 * --------------------------------------------------------------------------------------------- */

unsigned int ConnectionGetStatementCacheSize
(
    OCI_Connection *con
)
{
    ENTER_FUNC
    (
        /* returns */ unsigned int, 0,
        /* context */ OCI_IPC_CONNECTION, con
    )

    ub4 cache_size = 0;

    CHECK_PTR(OCI_IPC_CONNECTION, con)

#if OCI_VERSION_COMPILE >= OCI_9_2

    if (Env.version_runtime >= OCI_9_2)
    {
        CHECK_ATTRIB_GET
        (
            OCI_HTYPE_SVCCTX, OCI_ATTR_STMTCACHESIZE,
            con->cxt, &cache_size, NULL,
            con->err
        )
    }

#endif

    SET_RETVAL(cache_size)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionSetStatementCacheSize
 * --------------------------------------------------------------------------------------------- */

boolean ConnectionSetStatementCacheSize
(
    OCI_Connection *con,
    unsigned int    value
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_CONNECTION, con
    )

    ub4 cache_size = value;

    CHECK_PTR(OCI_IPC_CONNECTION, con)

#if OCI_VERSION_COMPILE >= OCI_9_2

    if (Env.version_runtime >= OCI_9_2)
    {
        CHECK_ATTRIB_SET
        (
            OCI_HTYPE_SVCCTX, OCI_ATTR_STMTCACHESIZE,
            con->cxt, &cache_size, sizeof(cache_size),
            con->err
        )
    }

#else

    OCI_NOT_USED(cache_size)

#endif

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionGetDefaultLobPrefetchSize
 * --------------------------------------------------------------------------------------------- */

unsigned int ConnectionGetDefaultLobPrefetchSize
(
    OCI_Connection *con
)
{
    ENTER_FUNC
    (
        /* returns */ unsigned int, 0,
        /* context */ OCI_IPC_CONNECTION, con
    )

    ub4 prefetch_size = 0;

    CHECK_PTR(OCI_IPC_CONNECTION, con)

#if OCI_VERSION_COMPILE >= OCI_11_1

    if (ConnectionIsVersionSupported(con, OCI_11_1))
    {
        CHECK_ATTRIB_GET
        (
            OCI_HTYPE_SESSION, OCI_ATTR_DEFAULT_LOBPREFETCH_SIZE,
            con->ses, &prefetch_size, NULL,
            con->err
        )
    }

#endif

    SET_RETVAL(prefetch_size)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionSetDefaultLobPrefetchSize
 * --------------------------------------------------------------------------------------------- */

boolean ConnectionSetDefaultLobPrefetchSize
(
    OCI_Connection *con,
    unsigned int    value
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_CONNECTION, con
    )

    ub4 prefetch_size = value;

    boolean success = FALSE;

    CHECK_PTR(OCI_IPC_CONNECTION, con)

#if OCI_VERSION_COMPILE >= OCI_11_1

    if (ConnectionIsVersionSupported(con, OCI_11_1))
    {
        CHECK_ATTRIB_SET
        (
            OCI_HTYPE_SESSION, OCI_ATTR_DEFAULT_LOBPREFETCH_SIZE,
            con->ses, &prefetch_size, sizeof(prefetch_size),
            con->err
        )

        success = TRUE;
    }

#else

    OCI_NOT_USED(prefetch_size)

#endif

    SET_RETVAL(success)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * ConnectionGetMaxCursors
 * --------------------------------------------------------------------------------------------- */

unsigned int ConnectionGetMaxCursors
(
    OCI_Connection *con
)
{
    ENTER_FUNC
    (
        /* returns */ unsigned int, 0,
        /* context */ OCI_IPC_CONNECTION, con
    )

    ub4 max_cursors = 0;

    CHECK_PTR(OCI_IPC_CONNECTION, con)

#if OCI_VERSION_COMPILE >= OCI_12_1

    if (ConnectionIsVersionSupported(con, OCI_11_1))
    {
        CHECK_ATTRIB_GET
        (
            OCI_HTYPE_SESSION, OCI_ATTR_MAX_OPEN_CURSORS,
            con->ses, &max_cursors, NULL,
            con->err
        )
    }

#endif

    SET_RETVAL(max_cursors)

    EXIT_FUNC()
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
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_CONNECTION, con
    )

    OCI_Statement *stmt = NULL;

    CHECK_PTR(OCI_IPC_CONNECTION, con)
    CHECK_PTR(OCI_IPC_STRING,     sql)

    /* create statement */

    stmt = StatementCreate(con);
    CHECK_NULL(stmt)

    /* First, execute SQL */

    CHECK(StatementExecuteStmt(stmt, sql))

    /* get resultset and set up variables */

    if (OCI_CST_SELECT == StatementGetStatementType(stmt))
    {
        CHECK(StatementFetchIntoUserVariables(stmt, args))
    }

    SET_SUCCESS()

    CLEANUP_AND_EXIT_FUNC
    (
        if (NULL != stmt)
        {
            StatementFree(stmt);
        }
    )
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
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_CONNECTION, con
    )

    OCI_Statement *stmt = NULL;
    otext* sql_fmt = NULL;

    va_list first_pass_args;
    va_list second_pass_args;

    va_copy(first_pass_args,  args);
    va_copy(second_pass_args, args);

    CHECK_PTR(OCI_IPC_CONNECTION, con)
    CHECK_PTR(OCI_IPC_STRING,     sql)

    /* create statement */

    stmt = StatementCreate(con);
    CHECK_NULL(stmt)

    /* first, get buffer size */

    int size = FormatParseSql(stmt, NULL, sql, &first_pass_args);
    CHECK(size > 0)

    /* allocate buffer */

    ALLOC_DATA(OCI_IPC_STRING, sql_fmt, size + 1)

    /* format buffer */

    size = FormatParseSql(stmt, sql_fmt, sql, &second_pass_args);
    CHECK(size > 0)

    /* prepare and execute SQL buffer */

    CHECK(StatementPrepareInternal(stmt, sql_fmt))
    CHECK(StatementExecuteInternal(stmt, OCI_DEFAULT))

    /* get resultset and set up variables */

    if (OCI_CST_SELECT == StatementGetStatementType(stmt))
    {
        CHECK(StatementFetchIntoUserVariables(stmt, second_pass_args))
    }

    SET_SUCCESS()

    CLEANUP_AND_EXIT_FUNC
    (
        va_end(first_pass_args);
        va_end(second_pass_args);

        if (NULL != stmt)
        {
            StatementFree(stmt);
        }

        FREE(sql_fmt)
    )
}
