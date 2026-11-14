/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2026 Vincent ROGIER <vince.rogier@gmail.com>
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
#include "shardingkey.h"
#include "statement.h"
#include "stringutils.h"
#include "timestamp.h"
#include "transaction.h"
#include "typeinfo.h"

static const OCI_UInt32 TraceTypeValues[] =
{
    OCI_TRC_IDENTITY,
    OCI_TRC_MODULE,
    OCI_TRC_ACTION,
    OCI_TRC_DETAIL,
    OCI_TRC_OPERATION
};

static const OCI_UInt32 TimeoutTypeValues[] =
{
    OCI_NTO_SEND,
    OCI_NTO_RECEIVE,
    OCI_NTO_CALL
};

#define SET_TRACE(prop)                                                     \
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

/* run a fallible teardown step: accumulate the status and log a warning on
   failure, but keep disposing the remaining resources */

#define CONN_DISPOSE_STEP(expr, name)                             \
                                                                  \
        do                                                        \
        {                                                         \
            if (!(expr))                                          \
            {                                                     \
                res = FALSE;                                      \
                OCILIB_LOG(OCI_LOG_CAT_LIB, OCI_LOG_WARN,         \
                           OTEXT("OCI_Connection %p: %s failed"), \
                           (void *)con, name);                    \
            }                                                     \
        }                                                         \
        while (0)

/* --------------------------------------------------------------------------------------------- *
 * OcilibConnectionDetachSubscriptions
 * --------------------------------------------------------------------------------------------- */

static void OcilibConnectionDetachSubscriptions
(
    OCI_Subscription *sub,
    OCI_Connection   *con
)
{
    ENTER_VOID
    (
        /* context */ OCI_IPC_CONNECTION, con
    );

    CHECK_PTR(OCI_IPC_NOTIFY,     sub);
    CHECK_PTR(OCI_IPC_CONNECTION, con);

    if (NULL != sub && (sub->con == con))
    {
        sub->con = NULL;

        sub->saved_db   = OcilibStringDuplicate(con->env, con->db);
        sub->saved_user = OcilibStringDuplicate(con->env, con->user);
        sub->saved_pwd  = OcilibStringDuplicate(con->env, con->pwd);
    }

    EXIT_VOID();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibConnectionAllocate
 * --------------------------------------------------------------------------------------------- */

OCI_Connection * OcilibConnectionAllocate
(
    OCI_Environment *env,
    OCI_Pool        *pool,
    const otext     *db,
    const otext     *user,
    const otext     *pwd,
    OCI_ShardingKey* skey,
    OCI_ShardingKey* sskey,
    OCI_SessionMode  mode
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Connection*, NULL,
        /* context */ OCI_IPC_ENVIRONMENT, env
    )

    /* create connection object */

    OCI_Connection* con = NULL;

    LIST_ATOMIC_ADD(env->cons, con);

    CHECK_NULL(con);

    con->env           = env;
    con->htype         = OCI_IPC_CONNECTION;
    con->alloc_handles = (NULL == skey && NULL == sskey);

    /* create internal lists */

    con->stmts = OcilibListCreate(env, OCI_IPC_STATEMENT);
    CHECK_NULL(con->stmts);

    con->tinfs = OcilibListCreate(env, OCI_IPC_TYPE_INFO);
    CHECK_NULL(con->tinfs);

    con->trsns = OcilibListCreate(env, OCI_IPC_TRANSACTION);
    CHECK_NULL(con->trsns);

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

        if (OCI_HTYPE_SPOOL == con->pool->kind)
        {
            con->alloc_handles = FALSE;
        }

#endif

    }
    else
    {
        con->db   = OcilibStringDuplicate(env, db   ? db   : OTEXT(""));
        con->user = OcilibStringDuplicate(env, user ? user : OTEXT(""));
        con->pwd  = OcilibStringDuplicate(env, pwd  ? pwd  : OTEXT(""));
    }

    con->env->handle = env->handle;
    con->env         = env;

    /*  allocate error handle */

    CHECK
    (
        OcilibMemoryAllocHandle
        (
            env,
            con->env->handle,
            (dvoid **)(void *)&con->err,
            OCI_HTYPE_ERROR
        )
    );

    /* update internal status */

    con->cstate = OCI_CONN_ALLOCATED;

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE)
        {
            OcilibConnectionFree(con);
            con = NULL;
        }

        SET_RETVAL(con);
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibConnectionDeallocate
 * --------------------------------------------------------------------------------------------- */

static OCI_Boolean OcilibConnectionDeallocate
(
    OCI_Connection *con
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_CONNECTION, con
    );

    CHECK_PTR(OCI_IPC_CONNECTION, con);

    /* free all OCI child handles that may still be alive, regardless of the
       connection state. This covers cases where an earlier dispose step failed
       or where login failed after allocating session/context handles. */

    if (con->alloc_handles)
    {
        if (NULL != con->handle)
        {
            OcilibMemoryFreeHandle(con->env, (dvoid*)con->handle, OCI_HTYPE_SVCCTX);
            con->handle = NULL;
        }

        if (NULL != con->ses)
        {
            OcilibMemoryFreeHandle(con->env, (dvoid*)con->ses, OCI_HTYPE_SESSION);
            con->ses = NULL;
        }

        if (NULL != con->svr)
        {
            OcilibMemoryFreeHandle(con->env, (dvoid*)con->svr, OCI_HTYPE_SERVER);
            con->svr = NULL;
        }
    }

    if (NULL != con->err)
    {
        OcilibMemoryFreeHandle(con->env, (dvoid*)con->err, OCI_HTYPE_ERROR);
        con->err = NULL;
    }

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibConnectionAttach
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibConnectionAttach
(
    OCI_Connection *con
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_CONNECTION, con
    );

    dbtext  * dbstr  = NULL;
    OCI_Int32 dbsize = -1;

    CHECK_PTR(OCI_IPC_CONNECTION, con);
    CHECK_CON_STATUS(con, OCI_CONN_ALLOCATED);

    /* allocate server handle for non session pooled connection */

    if (con->alloc_handles)
    {
        ub4 cmode = OCI_DEFAULT;

        CHECK(OcilibMemoryAllocHandle(con->env, con->env->handle, (dvoid **) (void *) &con->svr, OCI_HTYPE_SERVER));

        /* attach server handle to service name */

#if OCI_VERSION_COMPILE >= OCI_9_0

        if (con->env->version_runtime >= OCI_9_0 && con->pool)
        {
            dbstr  = (dbtext *) (con->pool->name);
            dbsize = OcilibStringDBSize(con->pool->name);
            cmode  = OCI_CPOOL;
        }
        else

#endif

        {
            dbstr  = (dbtext *) (con->db);
            dbsize = OcilibStringDBSize(con->db);
        }

        CHECK_OCI
        (
            con->err,
            OCIServerAttach,
            con->svr, con->err,
            (OraText *)dbstr, (sb4)dbsize, cmode
        );
    }

    /* update internal status */

    con->cstate = OCI_CONN_ATTACHED;

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibConnectionDetach
 * --------------------------------------------------------------------------------------------- */

static OCI_Boolean OcilibConnectionDetach
(
    OCI_Connection *con
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_CONNECTION, con
    );

    CHECK_PTR(OCI_IPC_CONNECTION, con);

    if (con->alloc_handles && NULL != con->svr)
    {
        /* detach from the oracle server */

        CHECK_OCI
        (
            con->err,
            OCIServerDetach,
            con->svr, con->err, OCI_DEFAULT
        );

        /* close server handle */

        OcilibMemoryFreeHandle(con->env, (dvoid*)con->svr, OCI_HTYPE_SERVER);

        con->svr = NULL;
    }

    /* update internal status */

    con->cstate = OCI_CONN_ALLOCATED;

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibConnectionLogonRegular
 * --------------------------------------------------------------------------------------------- */

static OCI_Boolean OcilibConnectionLogonRegular
(
    OCI_Connection *con,
    const otext    *new_pwd
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_CONNECTION, con
    );

    OCI_Int32 dbsize1 = -1;
    OCI_Int32 dbsize2 = -1;
    OCI_Int32 dbsize3 = -1;

    dbtext* dbstr1 = NULL;
    dbtext* dbstr2 = NULL;
    dbtext* dbstr3 = NULL;

    CHECK_PTR(OCI_IPC_CONNECTION, con);

    /* allocate session handle */

    CHECK(OcilibMemoryAllocHandle(con->env, con->env->handle, (dvoid **)(void *)&con->ses, OCI_HTYPE_SESSION));

    /* allocate context handle */

    CHECK(OcilibMemoryAllocHandle(con->env, con->env->handle, (dvoid **)(void *)&con->handle, OCI_HTYPE_SVCCTX));

    /* set context server attribute */

    CHECK_ATTRIB_SET
    (
        OCI_HTYPE_SVCCTX, OCI_ATTR_SERVER,
        con->handle, con->svr, sizeof(con->svr),
        con->err
    );

    /* modify user password if needed */

    if (IS_STRING_VALID(new_pwd))
    {
        dbstr1  = (dbtext *) (con->user);
        dbsize1 = OcilibStringDBSize(con->user);
        dbstr2  = (dbtext *) (con->pwd);
        dbsize2 = OcilibStringDBSize(con->pwd);
        dbstr3  = (dbtext *) (new_pwd);
        dbsize3 = OcilibStringDBSize(new_pwd);

        CHECK_ATTRIB_SET
        (
            OCI_HTYPE_SVCCTX, OCI_ATTR_SESSION,
            con->handle, con->ses,
            sizeof(con->ses),
            con->err
        );

        CHECK_OCI
        (
            con->err,
            OCIPasswordChange,
            con->handle, con->err,
            (OraText *)dbstr1, (ub4)dbsize1,
            (OraText *)dbstr2, (ub4)dbsize2,
            (OraText *)dbstr3, (ub4)dbsize3,
            OCI_AUTH
        );

        /* replace connection password */

        FREE(con->pwd);
        con->pwd = OcilibStringDuplicate(con->env, new_pwd);
    }
    else
    {
        ub4 credt = OCI_CRED_RDBMS;
        ub4 mode  = con->mode;

        /* set session login attribute */

        if (IS_STRING_VALID(con->user))
        {
            dbstr1  = (dbtext *) (con->user);
            dbsize1 = OcilibStringDBSize(con->user);

            CHECK_ATTRIB_SET
            (
                OCI_HTYPE_SESSION, OCI_ATTR_USERNAME,
                con->ses, dbstr1, dbsize1,
                con->err
            );
        }

        /* set session password attribute */

        if (IS_STRING_VALID(con->pwd))
        {
            dbstr2  = (dbtext *) (con->pwd);
            dbsize2 = OcilibStringDBSize(con->pwd);

            CHECK_ATTRIB_SET
            (
                OCI_HTYPE_SESSION, OCI_ATTR_PASSWORD,
                con->ses, dbstr2, dbsize2,
                con->err
            );
        }

        /* set OCILIB driver layer name attribute */

#if OCI_VERSION_COMPILE >= OCI_11_1

        if (con->env->version_runtime >= OCI_11_1)
        {
            otext driver_version[OCI_SIZE_FORMAT];

            OcilibStringFormat
            (
                driver_version,
                osizeof(driver_version) - (OCI_Size)1,
                OTEXT("%s : %d.%d.%d"),
                OCILIB_DRIVER_NAME,
                OCILIB_MAJOR_VERSION,
                OCILIB_MINOR_VERSION,
                OCILIB_REVISION_VERSION
            );

            dbstr3  = (dbtext *) (driver_version);
            dbsize3 = OcilibStringDBSize(driver_version);

            CHECK_ATTRIB_SET
            (
                OCI_HTYPE_SESSION, OCI_ATTR_DRIVER_NAME,
                con->ses, dbstr3, dbsize3,
                con->err
            );
        }

#endif

        /* start session */

        if (!IS_STRING_VALID(con->user) && !IS_STRING_VALID(con->pwd))
        {
            credt = OCI_CRED_EXT;
        }

#if OCI_VERSION_COMPILE >= OCI_9_2

        /* activate statement cache if the OCI version supports it */

        if (con->env->version_runtime >= OCI_9_2)
        {
            mode |= OCI_STMT_CACHE;
        }

#endif

        /* start session */

        CHECK_OCI
        (
            con->err,
            OCISessionBegin,
            con->handle, con->err, con->ses,
            credt, mode
        );

        CHECK_ATTRIB_SET
        (
            OCI_HTYPE_SVCCTX, OCI_ATTR_SESSION,
            con->handle, con->ses, sizeof(con->ses),
            con->err
        );

        if (!(con->mode & OCI_PRELIM_AUTH))
        {
            /* create default transaction object */

            OCI_Transaction* trs = OcilibTransactionCreate(con, 1, OCI_TRANS_READWRITE, NULL);
            CHECK_NULL(trs);

            /* start transaction */

            CHECK(OcilibConnectionSetTransaction(con, trs));
            CHECK(OcilibTransactionStart( trs));
        }
    }

    SET_SUCCESS();

    CLEANUP_AND_EXIT_FUNC
    (

        if (FAILURE && NULL != con)
        {
            /* could not start session, must free the session and context handles.
               Free the service context first as it holds references to the session */

            OcilibMemoryFreeHandle(con->env, (dvoid*)con->handle, OCI_HTYPE_SVCCTX);
            OcilibMemoryFreeHandle(con->env, (dvoid*)con->ses, OCI_HTYPE_SESSION);

            con->handle = NULL;
            con->ses    = NULL;
        }
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibConnectionLogonSessionGet
 *
 * Establish a connection through OCISessionGet().
 *
 * Applies to all kinds of connections:
 *
 *  - standalone connections   : OCI_SESSGET_CUSTOM_POOL + connect string
 *  - session pool connections : OCI_SESSGET_SPOOL + pool name
 *  - connection pool entries  : OCI_SESSGET_CPOOL  + pool name
 *
 * When sharding keys are provided (sharded connections), an OCIAuthInfo
 * handle is allocated to carry them (keys must be set on the authp before
 * OCISessionGet()); otherwise authp stays NULL and the call is the regular
 * session pool logon.
 *
 * Sessions obtained through OCISessionGet() are released through
 * OCISessionRelease(), hence con->alloc_handles is set to FALSE so that
 * the regular session pool logoff path is used.
 * --------------------------------------------------------------------------------------------- */

static OCI_Boolean OcilibConnectionLogonSessionGet
(
    OCI_Connection  *con,
    OCI_ShardingKey *skey,
    OCI_ShardingKey *sskey,
    const otext     *tag
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_CONNECTION, con
    );

    void     *authp     = NULL;
    ub4       sess_mode = OCI_DEFAULT;
    OCI_Int32 dbsize    = -1;
    dbtext   *dbstr     = NULL;
    OCI_Int32 dbtagsize = 0;
    dbtext   *dbtag     = NULL;
    OraText  *dbret     = NULL;
    ub4       dbretsize = 0;
    boolean   found     = FALSE;

    CHECK_PTR(OCI_IPC_CONNECTION, con);

#if OCI_VERSION_COMPILE >= OCI_12_2

    /* allocate authentication handle only when needed (sharding keys or
       standalone credentials); NULL authp is the regular pool logon     */

    if (NULL != skey || NULL != sskey || NULL == con->pool)
    {
        CHECK(OcilibMemoryAllocHandle(con->env, con->env->handle, (dvoid **)(void *)&authp, OCI_HTYPE_AUTHINFO));
    }

    if (NULL != skey)
    {
        CHECK_ATTRIB_SET
        (
            OCI_HTYPE_AUTHINFO, OCI_ATTR_SHARDING_KEY,
            authp, skey->handle, 0,
            con->err
        );
    }

    if (NULL != sskey)
    {
        CHECK_ATTRIB_SET
        (
            OCI_HTYPE_AUTHINFO, OCI_ATTR_SUPER_SHARDING_KEY,
            authp, sskey->handle, 0,
            con->err
        );
    }

    /* set credentials on the authentication handle for standalone
       connections (session/connection pools supply their own) */

    if (NULL == con->pool)
    {
        if (IS_STRING_VALID(con->user))
        {

            CHECK_ATTRIB_SET
            (
                OCI_HTYPE_AUTHINFO, OCI_ATTR_USERNAME,
                authp, (con->user), OcilibStringDBSize(con->user),
                con->err
            );
        }

        if (IS_STRING_VALID(con->pwd))
        {

            CHECK_ATTRIB_SET
            (
                OCI_HTYPE_AUTHINFO, OCI_ATTR_PASSWORD,
                authp, (con->pwd), OcilibStringDBSize(con->pwd),
                con->err
            );
        }
    }

    /* session get mode and connect string */

    if (con->mode & OCI_SESSION_SYSDBA)
    {
        sess_mode |= OCI_SESSGET_SYSDBA;
    }

    if (!IS_STRING_VALID(con->user) && !IS_STRING_VALID(con->pwd))
    {
        sess_mode |= OCI_SESSGET_CREDEXT;
    }

    if (NULL != con->pool)
    {
        dbstr  = (dbtext *) (con->pool->name);
        dbsize = OcilibStringDBSize(con->pool->name);

        if (OCI_HTYPE_SPOOL == con->pool->kind)
        {
            sess_mode |= OCI_SESSGET_SPOOL;

            if (IS_STRING_VALID(tag))
            {
                dbtag     = (dbtext *) (tag);
                dbtagsize = OcilibStringDBSize(tag);
            }
        }
        else
        {
            sess_mode |= OCI_SESSGET_CPOOL;
        }
    }
    else
    {
        sess_mode |= OCI_SESSGET_CUSTOM_POOL;

        if (con->env->version_runtime >= OCI_9_2)
        {
            sess_mode |= OCI_SESSGET_STMTCACHE;
        }

        dbstr  = (dbtext *) (con->db);
        dbsize = OcilibStringDBSize(con->db);
    }

    CHECK_OCI
    (
        con->err,
        OCISessionGet,
        con->env->handle, con->err, &con->handle,
        authp,
        (OraText  *)dbstr, (ub4)dbsize,
        (OraText  *)dbtag, (ub4)dbtagsize,
        (OraText **)&dbret, &dbretsize,
        (boolean*) &found, sess_mode
    );

    CHECK_ATTRIB_GET
    (
        OCI_HTYPE_SVCCTX, OCI_ATTR_SERVER,
        con->handle, &con->svr, NULL,
        con->err
    );

    CHECK_ATTRIB_GET
    (
        OCI_HTYPE_SVCCTX, OCI_ATTR_SESSION,
        con->handle, &con->ses, NULL,
        con->err
    );

    if (found)
    {
        CHECK(OcilibConnectionSetSessionTag(con, tag));
    }

    SET_SUCCESS();

#else

    OCI_NOT_USED(skey);
    OCI_NOT_USED(sskey);
    OCI_NOT_USED(tag);

    CHECK(FALSE);

#endif

    CLEANUP_AND_EXIT_FUNC
    (

        if (NULL != authp)
        {
            OcilibMemoryFreeHandle(con->env, (dvoid *)authp, OCI_HTYPE_AUTHINFO);
        }
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibConnectionLogon
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibConnectionLogon
(
    OCI_Connection  *con,
    const otext     *new_pwd,
    const otext     *tag,
    OCI_ShardingKey *skey,
    OCI_ShardingKey *sskey
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_CONNECTION, con
    );

    CHECK_PTR(OCI_IPC_CONNECTION, con);

#if OCI_VERSION_COMPILE < OCI_9_2

    OCI_NOT_USED(skey);
    OCI_NOT_USED(sskey);
    OCI_NOT_USED(tag);

#endif

    /* 1 - regular connections and connections from pool */

    if (con->alloc_handles)
    {
        CHECK(OcilibConnectionLogonRegular(con, new_pwd));
    }

#if OCI_VERSION_COMPILE >= OCI_9_2

    /* 2 - connection from session pool or sharded connection
           (sharding keys provided: con->alloc_handles is FALSE)          */

    else if (con->env->version_runtime >= OCI_9_2)
    {
        CHECK(OcilibConnectionLogonSessionGet(con, skey, sskey, tag));
    }

#endif

    /* get server version */

    OcilibConnectionGetServerVersionString(con);

    /* update internal status */

    con->cstate = OCI_CONN_LOGGED;

    /* if a warning/error was raised during logon and it must be reported,
       fail now. The CLEANUP block will dispose the partially created con. */
    OCI_Error *err = OcilibErrorGet(con->env, FALSE, FALSE);

    if (NULL != err &&  OCI_UNKNOWN != err->code &&
        (OCI_ERR_TYPE_WARNING != err->type || err->warnings_on))
    {
        CHECK(FALSE);
    }

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibConnectionLogoffRegular
 * --------------------------------------------------------------------------------------------- */

static OCI_Boolean OcilibConnectionLogoffRegular
(
    OCI_Connection *con
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_CONNECTION, con
    );

    CHECK_PTR(OCI_IPC_CONNECTION, con);

    /* close any server files not explicitly closed - no check of return code */

    if (NULL != con->handle && NULL != con->err && NULL != con->ses)
    {
        CHECK_OCI
        (
            con->err,
            OCISessionEnd,
            con->handle, con->err, con->ses,
            (ub4)OCI_DEFAULT
        );

        /* close session handle */

        if (con->ses)
        {
            OcilibMemoryFreeHandle(con->env, (dvoid*)con->ses, OCI_HTYPE_SESSION);
            con->ses = NULL;
        }

        /* close context handle */

        if (con->handle)
        {
            OcilibMemoryFreeHandle(con->env, (dvoid*)con->handle, OCI_HTYPE_SVCCTX);
            con->handle = NULL;
        }
    }

    SET_SUCCESS();

    EXIT_FUNC();

}

/* --------------------------------------------------------------------------------------------- *
 * OcilibConnectionLogoffSessionPool
 * --------------------------------------------------------------------------------------------- */

static OCI_Boolean OcilibConnectionLogoffSessionPool
(
    OCI_Connection *con
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_CONNECTION, con
    );

    dbtext  * dbstr  = NULL;
    OCI_Int32 dbsize = 0;

    CHECK_PTR(OCI_IPC_CONNECTION, con);

    /* No explicit transaction object => commit if needed otherwise rollback changes */

    if (con->autocom)
    {
        OcilibConnectionCommit(con);
    }
    else
    {
        OcilibConnectionRollback(con);
    }

#if OCI_VERSION_COMPILE >= OCI_9_2

    if (con->env->version_runtime >= OCI_9_2)
    {
        ub4 mode = OCI_DEFAULT;

        /* Clear session tag if connection was retrieved from session pool */

        if (NULL != con->pool && NULL != con->sess_tag && ( OCI_HTYPE_SPOOL == con->pool->kind))
        {
            dbstr  = (dbtext *) (con->sess_tag);
            dbsize = OcilibStringDBSize(con->sess_tag);
            mode   = OCI_SESSRLS_RETAG;
        }

        CHECK_OCI
        (
            con->err,
            OCISessionRelease,
            con->handle, con->err,
            (OraText*)dbstr, (ub4)dbsize,
            mode
        );

        con->handle = NULL;
        con->ses    = NULL;
        con->svr    = NULL;
    }

#endif

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibConnectionLogOff
 * --------------------------------------------------------------------------------------------- */

static OCI_Boolean OcilibConnectionLogOff
(
    OCI_Connection *con
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_CONNECTION, con
    );

    OCI_Boolean res = TRUE;

    CHECK_PTR(OCI_IPC_CONNECTION, con);
    CHECK_CON_STATUS(con, OCI_CONN_LOGGED);

    /* close opened files */

    if (con->nb_files > 0)
    {
        sword ret;

        OCI_CALL(ret, OCILobFileCloseAll, con->handle, con->err);
    }

    /* dissociate connection from existing subscriptions */

    LIST_ATOMIC_FOREACH_WITH_PARAM(con->env->subs, con, OcilibConnectionDetachSubscriptions);

    /* free all child lists */

    LIST_ATOMIC_REMOVE_ALL(con->stmts, OcilibStatementDispose);
    LIST_ATOMIC_REMOVE_ALL(con->tinfs, OcilibTypeInfoDispose);
    LIST_ATOMIC_REMOVE_ALL(con->trsns, OcilibTransactionDispose);

    /* 1 - regular Connection and Connection from Connection pool */

    if (con->alloc_handles)
    {
        CONN_DISPOSE_STEP(OcilibConnectionLogoffRegular(con), OTEXT("Regular"));
    }

#if OCI_VERSION_COMPILE >= OCI_9_2

    /* 3 - Connection from session pool */

    else if (con->env->version_runtime >= OCI_9_0)
    {
        CONN_DISPOSE_STEP(OcilibConnectionLogoffSessionPool(con), OTEXT("SessionPool"));
    }

#endif

    /* update internal status */

    con->cstate = OCI_CONN_ATTACHED;

    OCILIB_LOG(OCI_LOG_CAT_LIB, OCI_LOG_INFO, OTEXT("handle=[%p] disconnected"), con);

    SET_RETVAL(res);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibConnectionDispose
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibConnectionDispose
(
    OCI_Connection *con
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_CONNECTION, con
    );

    OCI_Boolean res = TRUE;

    CHECK_PTR(OCI_IPC_CONNECTION, con);

    /* clear server output resources */

    CONN_DISPOSE_STEP(OcilibConnectionDisableServerOutput(con), OTEXT("DisableServerOutput"));

    /* log off and detach form server */

    CONN_DISPOSE_STEP(OcilibConnectionLogOff(con),     OTEXT("LogOff"));
    CONN_DISPOSE_STEP(OcilibConnectionDetach(con),     OTEXT("Detach"));
    CONN_DISPOSE_STEP(OcilibConnectionDeallocate(con), OTEXT("Deallocate"));

    /* free internal lists */

    OcilibListFree(con->stmts);
    OcilibListFree(con->trsns);
    OcilibListFree(con->tinfs);

    /* free strings */

    FREE(con->ver_str);
    FREE(con->sess_tag);
    FREE(con->db_name);
    FREE(con->inst_name);
    FREE(con->service_name);
    FREE(con->server_name);
    FREE(con->db_name);
    FREE(con->domain_name);
    FREE(con->trace);

    if (!con->pool)
    {
        FREE(con->db);
        FREE(con->user);
        FREE(con->pwd);
    }

    if (NULL != con->inst_startup)
    {
        /* con->inst_startup wraps an OCI descriptor that is owned by the server
           handle (OCI_ATTR_INSTSTARTTIME). Free the OCILIB wrapper only. */
        FREE(con->inst_startup);
    }

    con->stmts = NULL;
    con->trsns = NULL;
    con->tinfs = NULL;

    OcilibErrorResetSource(NULL, con);

    SET_RETVAL(res);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibConnectionCreateInternal
 * --------------------------------------------------------------------------------------------- */

OCI_Connection * OcilibConnectionCreateInternal
(
    OCI_Environment *env,
    OCI_Pool        *pool,
    const otext     *db,
    const otext     *user,
    const otext     *pwd,
    OCI_SessionMode  mode,
    const otext     *tag,
    OCI_ShardingKey *skey,
    OCI_ShardingKey *sskey
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Connection*, NULL,
        /* context */ OCI_IPC_ENVIRONMENT, env
    )

    /* create Connection */

    OCI_Connection *con = OcilibConnectionAllocate(env, pool, db, user, pwd, skey, sskey, mode);
    CHECK_NULL(con);

    CHECK(OcilibConnectionAttach(con));
    CHECK(OcilibConnectionLogon(con, NULL, tag, skey, sskey));

    OCILIB_LOG(OCI_LOG_CAT_LIB, OCI_LOG_INFO,
               OTEXT("handle=[%p] connected db=[%s] user=[%s] tag =[%s]"),
               con,
               db ? db : OTEXT(""),
               user ? user : OTEXT(""),
               tag ? tag : OTEXT(""));

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE)
        {
            OcilibConnectionFree(con);
            con = NULL;
        }

        SET_RETVAL(con);
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibConnectionGetMinSupportedVersion
 * --------------------------------------------------------------------------------------------- */

OCI_UInt32 OcilibConnectionGetMinSupportedVersion
(
    OCI_Connection *con
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_UInt32, OCI_UNKNOWN,
        /* context */ OCI_IPC_CONNECTION, con
    );

    CHECK_PTR(OCI_IPC_CONNECTION, con);

    SET_RETVAL((con->env->version_runtime > con->ver_num) ? con->ver_num : con->env->version_runtime);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibConnectionIsVersionSupported
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibConnectionIsVersionSupported
(
    OCI_Connection *con,
    OCI_UInt32      version
)
{
    return OcilibConnectionGetMinSupportedVersion(con) >= version;
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibConnectionCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Connection * OcilibConnectionCreate
(
    OCI_Environment *env,
    const otext     *db,
    const otext     *user,
    const otext     *pwd,
    OCI_SessionMode  mode
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Connection*, NULL,
        /* context */ OCI_IPC_ENVIRONMENT, env
    );

    CHECK_PTR(OCI_IPC_ENVIRONMENT, env);

    SET_RETVAL(OcilibConnectionCreateInternal(env, NULL, db, user, pwd, mode, NULL, NULL, NULL));

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibConnectionCreateSharded
 * --------------------------------------------------------------------------------------------- */

OCI_Connection * OcilibConnectionCreateSharded
(
    OCI_Environment *env,
    const otext     *db,
    const otext     *user,
    const otext     *pwd,
    OCI_SessionMode  mode,
    OCI_ShardingKey *skey,
    OCI_ShardingKey *sskey
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Connection*, NULL,
        /* context */ OCI_IPC_ENVIRONMENT, env
    );

    CHECK_PTR(OCI_IPC_ENVIRONMENT, env);

    SET_RETVAL(OcilibConnectionCreateInternal(env, NULL, db, user, pwd, mode, NULL, skey, sskey));

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibConnectionFree
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibConnectionFree
(
    OCI_Connection *con
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_CONNECTION, con
    );

    CHECK_PTR(OCI_IPC_CONNECTION, con);

    LIST_ATOMIC_REMOVE(con->env->cons, con, OcilibConnectionDispose);

    FREE(con);

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibConnectionCommit
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibConnectionCommit
(
    OCI_Connection *con
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_CONNECTION, con
    );

    CHECK_PTR(OCI_IPC_CONNECTION, con);

    CHECK_OCI
    (
        con->err,
        OCITransCommit,
        con->handle, con->err, (ub4)OCI_DEFAULT
    );

    OCILIB_LOG(OCI_LOG_CAT_LIB, OCI_LOG_DEBUG, OTEXT("handle=[%p] Commit"), con);

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibConnectionRollback
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibConnectionRollback
(
    OCI_Connection *con
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_CONNECTION, con
    );

    CHECK_PTR(OCI_IPC_CONNECTION, con);

    CHECK_OCI
    (
        con->err,
        OCITransRollback,
        con->handle, con->err, (ub4)OCI_DEFAULT
    );

    OCILIB_LOG(OCI_LOG_CAT_LIB, OCI_LOG_DEBUG, OTEXT("handle=[%p] Rollback"), con);

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibConnectionSetAutoCommit
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibConnectionSetAutoCommit
(
    OCI_Connection *con,
    OCI_Boolean     enable
)
{
    SET_PROP
    (
        /* handle */ OCI_IPC_CONNECTION, con,
        /* member */ autocom, OCI_Boolean,
        /* value  */ enable
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibConnectionGetAutoCommit
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibConnectionGetAutoCommit
(
    OCI_Connection *con
)
{
    GET_PROP
    (
        /* result */ OCI_Boolean, FALSE,
        /* handle */ OCI_IPC_CONNECTION, con,
        /* member */ autocom
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibConnectionIsAlive
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibConnectionIsAlive
(
    OCI_Connection *con
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_CONNECTION, con
    );

    ub4 status = 0;
    ub4 size   = (ub4) sizeof(status);

    CHECK_PTR(OCI_IPC_CONNECTION, con);

    CHECK_ATTRIB_GET
    (
        OCI_HTYPE_SERVER, OCI_ATTR_SERVER_STATUS,
        con->svr, &status, &size,
        con->err
    );

    SET_RETVAL(status == OCI_SERVER_NORMAL);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibConnectionSetSessionTag
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibConnectionSetSessionTag
(
    OCI_Connection *con,
    const otext    *tag
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_CONNECTION, con
    );

    CHECK_PTR(OCI_IPC_CONNECTION, con);

    FREE(con->sess_tag);

#if OCI_VERSION_COMPILE >= OCI_9_2

    if (tag && con->pool && (OCI_HTYPE_SPOOL == con->pool->kind))
    {
        con->sess_tag = OcilibStringDuplicate(con->env, tag);
        CHECK_NULL(con->sess_tag);
    }

#else

    OCI_NOT_USED(tag);

#endif

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibConnectionGetSessionTag
 * --------------------------------------------------------------------------------------------- */

const otext * OcilibConnectionGetSessionTag
(
    OCI_Connection *con
)
{
    GET_PROP
    (
        /* result */ const otext*, NULL,
        /* handle */ OCI_IPC_CONNECTION, con,
        /* member */ sess_tag
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibConnectionGetDatabase
 * --------------------------------------------------------------------------------------------- */

const otext * OcilibConnectionGetConnectionString
(
    OCI_Connection *con
)
{
    GET_PROP
    (
        /* result */ const otext*, NULL,
        /* handle */ OCI_IPC_CONNECTION, con,
        /* member */ db
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibConnectionGetUserName
 * --------------------------------------------------------------------------------------------- */

const otext * OcilibConnectionGetUserName
(
    OCI_Connection *con
)
{
    GET_PROP
    (
        /* result */ const otext*, NULL,
        /* handle */ OCI_IPC_CONNECTION, con,
        /* member */ user
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibConnectionGetPassword
 * --------------------------------------------------------------------------------------------- */

const otext * OcilibConnectionGetPassword
(
    OCI_Connection *con
)
{
    GET_PROP
    (
        /* result */ const otext*, NULL,
        /* handle */ OCI_IPC_CONNECTION, con,
        /* member */ pwd
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibConnectionSetPassword
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibConnectionSetPassword
(
    OCI_Connection *con,
    const otext    *password
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_CONNECTION, con
    );

    CHECK_PTR(OCI_IPC_CONNECTION, con);
    CHECK_PTR(OCI_IPC_STRING,     password);

    if (OCI_CONN_LOGGED != con->cstate)
    {
        CHECK(OcilibConnectionLogon(con, password, NULL, NULL, NULL));
    }
    else
    {
        CHECK_OCI
        (
            con->err,
            OCIPasswordChange,
            con->handle, con->err,
            (OraText *) (con->user), (ub4) OcilibStringDBSize(con->user),
            (OraText *) (con->pwd), (ub4) OcilibStringDBSize(con->pwd),
            (OraText *) (password), (ub4) OcilibStringDBSize(password),
            OCI_DEFAULT
        );

        /* replace connection password */

        FREE(con->pwd);
        con->pwd = OcilibStringDuplicate(con->env, password);
    }

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibConnectionGetSessionMode
 * --------------------------------------------------------------------------------------------- */

OCI_SessionMode OcilibConnectionGetSessionMode
(
    OCI_Connection *con
)
{
    GET_PROP
    (
        /* result */ OCI_SessionMode, OCI_UNKNOWN,
        /* handle */ OCI_IPC_CONNECTION, con,
        /* member */ mode
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibConnectionGetServerVersionString
 * --------------------------------------------------------------------------------------------- */

const otext * OcilibConnectionGetServerVersionString
(
    OCI_Connection *con
)
{
    ENTER_FUNC
    (
        /* returns */ const otext*, NULL,
        /* context */ OCI_IPC_CONNECTION, con
    );

    OCI_Int32 dbsize = OCI_SIZE_BUFFER * (OCI_Int32)sizeof(dbtext);
    dbtext  * dbstr  = NULL;

    CHECK_PTR(OCI_IPC_CONNECTION, con);

    /* no version available in preliminary authentication mode */

    if (!con->ver_str && (!(con->mode & OCI_PRELIM_AUTH)))
    {
        OCI_Int32 ver_maj = 0;
        OCI_Int32 ver_min = 0;
        OCI_Int32 ver_rev = 0;

        ALLOC_DATA(OCI_IPC_STRING, con->ver_str, OCI_SIZE_BUFFER + 1);

        dbstr = (dbtext *) (con->ver_str);

#if OCI_VERSION_COMPILE >= OCI_18_1

        ub4 version = 0;

        if (con->env->version_runtime >= OCI_18_1)
        {
            CHECK_OCI
            (
                con->err,
                OCIServerRelease2,
                (dvoid *)con->handle, con->err, (OraText *)dbstr, (ub4)dbsize,
                (ub1)OCI_HTYPE_SVCCTX, &version, OCI_DEFAULT
            );
        }
        else

#endif

        {
            CHECK_OCI
            (
                con->err,
                OCIServerVersion,
                (dvoid *)con->handle, con->err,
                (OraText *)dbstr, (ub4)dbsize,
                (ub1)OCI_HTYPE_SVCCTX
            );
        }

#if OCI_VERSION_COMPILE >= OCI_18_1

        if (con->env->version_runtime >= OCI_18_1)
        {
            ver_maj = OCI_SERVER_RELEASE_REL(version);
            ver_min = OCI_SERVER_RELEASE_REL_UPD(version);
            ver_rev = OCI_SERVER_RELEASE_REL_UPD_REV(version);

            con->ver_num = (OCI_UInt32) OCI_VER_MAKE(ver_maj, ver_min, ver_rev);
        }
        else

#endif

        {
            otext* p = NULL;

            con->ver_str[(OCI_Size) ocharcount(dbsize)] = 0;

            /* parse server version string to find the version information
                **/

            for (p = con->ver_str; p && *p; p++)
            {
                if (oisdigit((unsigned char)*p) &&
                    (*(p + (OCI_Size)1) != 0) &&
                    (*(p + (OCI_Size)1) == OTEXT('.') || (*(p + (OCI_Size)2) == OTEXT('.'))))
                {
                    if (NB_ARG_VERSION == osscanf(p, OTEXT("%d.%d.%d"),
                                                  (OCI_Int32*)&ver_maj,
                                                  (OCI_Int32*)&ver_min,
                                                  (OCI_Int32*)&ver_rev))
                    {
                        con->ver_num = (OCI_UInt32) OCI_VER_MAKE(ver_maj, ver_min, ver_rev);
                    }

                    break;
                }
            }
        }
    }

    SET_RETVAL((const otext*)con->ver_str);

    CLEANUP_AND_EXIT_FUNC
    (

        if (FAILURE)
        {
            FREE(con->ver_str);
        }
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibConnectionGetServerMajorVersion
 * --------------------------------------------------------------------------------------------- */

OCI_UInt32 OcilibConnectionGetServerMajorVersion
(
    OCI_Connection *con
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_UInt32, OCI_UNKNOWN,
        /* context */ OCI_IPC_CONNECTION, con
    );

    CHECK_PTR(OCI_IPC_CONNECTION, con);

    if (OCI_UNKNOWN == con->ver_num)
    {
        OcilibConnectionGetServerVersionString(con);
    }

    SET_RETVAL((OCI_UInt32) OCI_VER_MAJ(con->ver_num));

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibConnectionGetServerMinorVersion
 * --------------------------------------------------------------------------------------------- */

OCI_UInt32 OcilibConnectionGetServerMinorVersion
(
    OCI_Connection *con
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_UInt32, OCI_UNKNOWN,
        /* context */ OCI_IPC_CONNECTION, con
    );

    CHECK_PTR(OCI_IPC_CONNECTION, con);

    if (OCI_UNKNOWN == con->ver_num)
    {
        OcilibConnectionGetServerVersionString(con);
    }

    SET_RETVAL((OCI_UInt32) OCI_VER_MIN(con->ver_num));

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibConnectionGetServerRevisionVersion
 * --------------------------------------------------------------------------------------------- */

OCI_UInt32 OcilibConnectionGetServerRevisionVersion
(
    OCI_Connection *con
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_UInt32, OCI_UNKNOWN,
        /* context */ OCI_IPC_CONNECTION, con
    );

    CHECK_PTR(OCI_IPC_CONNECTION, con);

    if (OCI_UNKNOWN == con->ver_num)
    {
        OcilibConnectionGetServerVersionString(con);
    }

    SET_RETVAL((OCI_UInt32) OCI_VER_REV(con->ver_num));

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibConnectionGetTransaction
 * --------------------------------------------------------------------------------------------- */

OCI_Transaction * OcilibConnectionGetTransaction
(
    OCI_Connection *con
)
{
    GET_PROP
    (
        /* result */ OCI_Transaction*, NULL,
        /* handle */ OCI_IPC_CONNECTION, con,
        /* member */ trs
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibConnectionSetTransaction
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibConnectionSetTransaction
(
    OCI_Connection  *con,
    OCI_Transaction *trans
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_CONNECTION, con
    );

    CHECK_PTR(OCI_IPC_CONNECTION,  con);
    CHECK_PTR(OCI_IPC_TRANSACTION, trans);

    if (NULL != con->trs)
    {
        CHECK(OcilibTransactionStop( con->trs));
    }

    CHECK_ATTRIB_SET
    (
        OCI_HTYPE_SVCCTX, OCI_ATTR_TRANS,
        con->handle, trans->handle, sizeof(trans->handle),
        con->err
    );

    con->trs = trans;

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibConnectionGetVersion
 * --------------------------------------------------------------------------------------------- */

OCI_UInt32 OcilibConnectionGetVersion
(
    OCI_Connection *con
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_UInt32, OCI_UNKNOWN,
        /* context */ OCI_IPC_CONNECTION, con
    );

    CHECK_PTR(OCI_IPC_CONNECTION, con);

    /* return the minimum supported version */

    SET_RETVAL(OcilibConnectionGetMinSupportedVersion(con));

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibConnectionBreak
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibConnectionBreak
(
    OCI_Connection *con
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_CONNECTION, con
    );

    CHECK_PTR(OCI_IPC_CONNECTION, con);

    CHECK_OCI
    (
        con->err,
        OCIBreak,
        (dvoid*)con->handle, con->err
    );

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibConnectionEnableServerOutput
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibConnectionEnableServerOutput
(
    OCI_Connection *con,
    OCI_UInt32      bufsize,
    OCI_UInt32      arrsize,
    OCI_UInt32      lnsize
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_CONNECTION, con
    );

    CHECK_PTR(OCI_IPC_CONNECTION, con);

    /* initialize the output buffer on server side */

    ALLOC_DATA(OCI_IPC_SERVER_OUPUT, con->svopt, 1);

    /* allocation internal buffer if needed */

    if (NULL == con->svopt->arrbuf)
    {
        const OCI_UInt32 charsize = sizeof(otext);

        const OCI_UInt32 max_lnsize = (con->ver_num >= OCI_10_2) ? OCI_OUPUT_LSIZE_10G : OCI_OUPUT_LSIZE;

        /* check parameter ranges (Oracle 10g increased the size of output line) */

        if (lnsize > max_lnsize)
        {
            lnsize = max_lnsize;
        }

        con->svopt->arrsize = arrsize;
        con->svopt->lnsize  = lnsize;

        /* allocate internal string (line) array */

        ALLOC_BUFFER(OCI_IPC_STRING, con->svopt->arrbuf, (con->svopt->lnsize + 1) * charsize, con->svopt->arrsize);
    }

    if (NULL == con->svopt->stmt)
    {
        con->svopt->stmt = OcilibStatementCreate(con);
        CHECK_NULL(con->svopt->stmt);
    }

    /* enable server output */

    CHECK(OcilibStatementPrepare(con->svopt->stmt, OTEXT("BEGIN DBMS_OUTPUT.ENABLE(:n); END;")));
    CHECK_NULL(OcilibStatementBindUInt32(con->svopt->stmt, OTEXT(":n"), &bufsize));

    if (0 == bufsize)
    {
        OCI_Bind* bnd = OcilibStatementGetBind(con->svopt->stmt, 0);
        CHECK_NULL(bnd);

        CHECK(OcilibBindSetNull(bnd));
    }

    CHECK(OcilibStatementExecutePrepared(con->svopt->stmt));

    /* prepare the retrieval statement call */

    con->svopt->cursize = con->svopt->arrsize;

    CHECK(OcilibStatementPrepare(con->svopt->stmt, OTEXT("BEGIN DBMS_OUTPUT.GET_LINES(:s, :i); END;")));

    CHECK_NULL(OcilibStatementBindStringArray
               (
                   con->svopt->stmt, OTEXT(":s"),
                   (otext *) con->svopt->arrbuf,
                   con->svopt->lnsize,
                   con->svopt->arrsize
               )
               );

    CHECK_NULL(OcilibStatementBindUInt32
               (
                   con->svopt->stmt, OTEXT(":i"),
                   &con->svopt->cursize
               )
               );

    SET_SUCCESS();

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE)
        {
            OcilibConnectionDisableServerOutput(con);
        }
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibConnectionDisableServerOutput
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibConnectionDisableServerOutput
(
    OCI_Connection *con
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_CONNECTION, con
    );

    CHECK_PTR(OCI_IPC_CONNECTION, con);

    if (NULL != con->svopt)
    {
        OcilibStatementExecute(con->svopt->stmt, OTEXT("BEGIN DBMS_OUTPUT.DISABLE(); END;"));

        if (NULL != con->svopt->stmt)
        {
            OcilibStatementFree(con->svopt->stmt);
            con->svopt->stmt = NULL;
        }

        FREE(con->svopt->arrbuf);
        FREE(con->svopt);
    }

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibConnectionGetServerOutput
 * --------------------------------------------------------------------------------------------- */

const otext * OcilibConnectionGetServerOutput
(
    OCI_Connection *con
)
{
    ENTER_FUNC
    (
        /* returns */ const otext*, NULL,
        /* context */ OCI_IPC_CONNECTION, con
    );

    CHECK_PTR(OCI_IPC_CONNECTION, con);

    const otext *line = NULL;

    if (NULL != con->svopt)
    {
        if (0 == con->svopt->curpos || con->svopt->curpos >= con->svopt->cursize)
        {
            con->svopt->cursize = con->svopt->arrsize;

            CHECK(OcilibStatementExecutePrepared(con->svopt->stmt));

            con->svopt->curpos = 0;
        }

        if (con->svopt->cursize > 0)
        {
            const OCI_UInt32 charsize = sizeof(otext);

            line = (const otext*)(con->svopt->arrbuf + (OCI_Size)(((con->svopt->lnsize + 1) * charsize) * con->svopt->curpos++));
        }
    }

    SET_RETVAL(line);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibConnectionSetTrace
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibConnectionSetTrace
(
    OCI_Connection *con,
    OCI_TraceType   trace,
    const otext    *value
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_CONNECTION, con
    );

    dbtext  * dbstr  = NULL;
    OCI_Int32 dbsize = 0;

    ub4 attrib = OCI_UNKNOWN;

    const otext* str = NULL;

    CHECK_PTR(OCI_IPC_CONNECTION, con);
    CHECK_ENUM_VALUE(trace, TraceTypeValues, OTEXT("Trace Type"));

    /* allocate trace info structure only if trace functions are used */

    ALLOC_DATA(OCI_IPC_TRACE_INFO, con->trace, 1);

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
                SET_TRACE(identifier);
                break;
            }
            case OCI_TRC_MODULE:
            {
#if OCI_VERSION_COMPILE >= OCI_10_1
                attrib = OCI_ATTR_MODULE;
#endif
                SET_TRACE(module);
                break;
            }
            case OCI_TRC_ACTION:
            {
#if OCI_VERSION_COMPILE >= OCI_10_1
                attrib = OCI_ATTR_ACTION;
#endif
                SET_TRACE(action);
                break;
            }
            case OCI_TRC_DETAIL:
            {
#if OCI_VERSION_COMPILE >= OCI_10_1
                attrib = OCI_ATTR_CLIENT_INFO;
#endif
                SET_TRACE(info);
                break;
            }
            case OCI_TRC_OPERATION:
            {
#if OCI_VERSION_COMPILE >= OCI_12_1
                attrib = OCI_ATTR_DBOP;
#endif
                SET_TRACE(operation);
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
            dbstr  = (dbtext *) (str);
            dbsize = OcilibStringDBSize(str);
        }

        CHECK_ATTRIB_SET
        (
            OCI_HTYPE_SESSION, attrib,
            con->ses, dbstr, dbsize,
            con->err
        );
    }

#endif

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibConnectionGetTrace
 * --------------------------------------------------------------------------------------------- */

const otext * OcilibConnectionGetTrace
(
    OCI_Connection *con,
    OCI_TraceType   trace
)
{
    ENTER_FUNC
    (
        /* returns */ const otext*, NULL,
        /* context */ OCI_IPC_CONNECTION, con
    );

    CHECK_PTR(OCI_IPC_CONNECTION, con);
    CHECK_ENUM_VALUE(trace, TraceTypeValues, OTEXT("Trace Type"));

    const otext *value = NULL;

    if (con->trace)
    {
        switch (trace)
        {
            case OCI_TRC_IDENTITY:
            {
                GET_TRACE(identifier);
                break;
            }
            case OCI_TRC_MODULE:
            {
                GET_TRACE(module);
                break;
            }
            case OCI_TRC_ACTION:
            {
                GET_TRACE(action);
                break;
            }
            case OCI_TRC_DETAIL:
            {
                GET_TRACE(info);
                break;
            }
            case OCI_TRC_OPERATION:
            {
                GET_TRACE(operation);
                break;
            }
        }
    }

    SET_RETVAL(value);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibConnectionPing
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibConnectionPing
(
    OCI_Connection *con
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_CONNECTION, con
    );

    CHECK_PTR(OCI_IPC_CONNECTION, con);

#if OCI_VERSION_COMPILE >= OCI_10_2

    if (con->env->version_runtime >= OCI_10_2)
    {
        CHECK_OCI
        (
            con->err,
            OCIPing, con->handle, con->err,
            (ub4)OCI_DEFAULT
        );
    }

#endif

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibConnectionSetTimeout
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibConnectionSetTimeout
(
    OCI_Connection *con,
    OCI_TimeoutType type,
    OCI_UInt32      value
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_CONNECTION, con
    );

    CHECK_PTR(OCI_IPC_CONNECTION, con);
    CHECK_ENUM_VALUE(type, TimeoutTypeValues, OTEXT("timeout type"));

    OCI_Boolean success = FALSE;

#if OCI_VERSION_COMPILE >= OCI_12_1

    ub4 timeout = value;

    if (con->env->version_runtime >= OCI_12_1)
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
                );
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
                );
                success = TRUE;
                break;
            }

  #if OCI_VERSION_COMPILE >= OCI_18_1

            case OCI_NTO_CALL:
            {
                if (con->env->version_runtime >= OCI_18_1)
                {
                    CHECK_ATTRIB_SET
                    (
                        OCI_HTYPE_SVCCTX, OCI_ATTR_CALL_TIMEOUT,
                        con->handle, &timeout, sizeof(timeout),
                        con->err
                    );
                    success = TRUE;
                }
                break;
            }

  #endif

        }
    }

#endif

    SET_RETVAL(success);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibConnectionGetTimeout
 * --------------------------------------------------------------------------------------------- */

OCI_UInt32 OcilibConnectionGetTimeout
(
    OCI_Connection *con,
    OCI_TimeoutType type
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_UInt32, 0,
        /* context */ OCI_IPC_CONNECTION, con
    );

    ub4 timeout = 0;

    CHECK_PTR(OCI_IPC_CONNECTION, con);
    CHECK_ENUM_VALUE(type, TimeoutTypeValues, OTEXT("timeout type"));

#if OCI_VERSION_COMPILE >= OCI_12_1

    if (con->env->version_runtime >= OCI_12_1)
    {
        switch (type)
        {
            case OCI_NTO_SEND:
            {
                CHECK_ATTRIB_GET
                (
                    OCI_HTYPE_SERVER, OCI_ATTR_SEND_TIMEOUT,
                    con->svr, &timeout, NULL,
                    con->err
                );
                break;
            }
            case OCI_NTO_RECEIVE:
            {
                CHECK_ATTRIB_GET
                (
                    OCI_HTYPE_SERVER, OCI_ATTR_RECEIVE_TIMEOUT,
                    con->svr, &timeout, NULL,
                    con->err
                );
                break;
            }

  #if OCI_VERSION_COMPILE >= OCI_18_1

            case OCI_NTO_CALL:
            {
                if (con->env->version_runtime >= OCI_18_3)
                {
                    CHECK_ATTRIB_GET
                    (
                        OCI_HTYPE_SVCCTX, OCI_ATTR_CALL_TIMEOUT,
                        con->handle, &timeout, NULL,
                        con->err
                    );
                }
                break;
            }

  #endif

        }
    }

#endif

    SET_RETVAL(timeout);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibConnectionGetDatabaseName
 * --------------------------------------------------------------------------------------------- */

const otext * OcilibConnectionGetDatabaseName
(
    OCI_Connection *con
)
{
    ENTER_FUNC
    (
        /* returns */ const otext *, NULL,
        /* context */ OCI_IPC_CONNECTION, con
    );

    CHECK_PTR(OCI_IPC_CONNECTION, con);

#if OCI_VERSION_COMPILE >= OCI_10_2

    if (NULL == con->db_name)
    {
        OCI_UInt32 size = 0;

        CHECK(OcilibStringGetAttribute(con, con->svr, OCI_HTYPE_SERVER,
                                       OCI_ATTR_DBNAME, &con->db_name, &size));
    }

#endif

    SET_RETVAL((const otext *) con->db_name);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibConnectionGetInstanceName
 * --------------------------------------------------------------------------------------------- */

const otext * OcilibConnectionGetInstanceName
(
    OCI_Connection *con
)
{
    ENTER_FUNC
    (
        /* returns */ const otext*, NULL,
        /* context */ OCI_IPC_CONNECTION, con
    );
    CHECK_PTR(OCI_IPC_CONNECTION, con);

#if OCI_VERSION_COMPILE >= OCI_10_2

    if (NULL == con->inst_name)
    {
        OCI_UInt32 size = 0;

        CHECK(OcilibStringGetAttribute(con, con->svr, OCI_HTYPE_SERVER,
                                       OCI_ATTR_INSTNAME, &con->inst_name, &size));
    }

#endif

    SET_RETVAL((const otext *)con->inst_name);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibConnectionGetServiceName
 * --------------------------------------------------------------------------------------------- */

const otext * OcilibConnectionGetServiceName
(
    OCI_Connection *con
)
{
    ENTER_FUNC
    (
        /* returns */ const otext*, NULL,
        /* context */ OCI_IPC_CONNECTION, con
    );
    CHECK_PTR(OCI_IPC_CONNECTION, con);

#if OCI_VERSION_COMPILE >= OCI_10_2

    if (NULL == con->service_name)
    {
        OCI_UInt32 size = 0;

        CHECK(OcilibStringGetAttribute(con, con->svr, OCI_HTYPE_SERVER,
                                       OCI_ATTR_SERVICENAME, &con->service_name, &size));
    }

#endif

    SET_RETVAL((const otext *)con->service_name);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibConnectionGetServerName
 * --------------------------------------------------------------------------------------------- */

const otext * OcilibConnectionGetServerName
(
    OCI_Connection *con
)
{
    ENTER_FUNC
    (
        /* returns */ const otext*, NULL,
        /* context */ OCI_IPC_CONNECTION, con
    );

    CHECK_PTR(OCI_IPC_CONNECTION, con);

#if OCI_VERSION_COMPILE >= OCI_10_2

    if (NULL == con->server_name)
    {
        OCI_UInt32 size = 0;

        CHECK(OcilibStringGetAttribute(con, con->svr, OCI_HTYPE_SERVER,
                                       OCI_ATTR_HOSTNAME, &con->server_name, &size));
    }

#endif

    SET_RETVAL((const otext *)con->server_name);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibConnectionGetDomainName
 * --------------------------------------------------------------------------------------------- */

const otext * OcilibConnectionGetDomainName
(
    OCI_Connection *con
)
{
    ENTER_FUNC
    (
        /* returns */ const otext*, NULL,
        /* context */ OCI_IPC_CONNECTION, con
    );
    CHECK_PTR(OCI_IPC_CONNECTION, con);

#if OCI_VERSION_COMPILE >= OCI_10_2

    if (NULL == con->domain_name)
    {
        OCI_UInt32 size = 0;

        CHECK(OcilibStringGetAttribute(con, con->svr, OCI_HTYPE_SERVER,
                                       OCI_ATTR_DBDOMAIN, &con->domain_name, &size));
    }

#endif

    SET_RETVAL((const otext *)con->domain_name);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibConnectionGetInstanceStartTime
 * --------------------------------------------------------------------------------------------- */

OCI_Timestamp * OcilibConnectionGetInstanceStartTime
(
    OCI_Connection *con
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Timestamp*, NULL,
        /* context */ OCI_IPC_CONNECTION, con
    );

    CHECK_PTR(OCI_IPC_CONNECTION, con);

#if OCI_VERSION_COMPILE >= OCI_10_2

    if (NULL == con->inst_startup)
    {
        OCIDateTime *handle = NULL;

        CHECK_ATTRIB_GET
        (
            OCI_HTYPE_SERVER, OCI_ATTR_INSTSTARTTIME,
            con->svr, &handle, NULL,
            con->err
        );

        con->inst_startup = OcilibTimestampInitialize(con->env, con, NULL, handle, OCI_TIMESTAMP);
        CHECK_NULL(con->inst_startup);
    }

#endif

    SET_RETVAL(con->inst_startup);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibConnectionIsTAFCapable
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibConnectionIsTAFCapable
(
    OCI_Connection *con
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_CONNECTION, con
    );

    OCI_Boolean value = FALSE;

    CHECK_PTR(OCI_IPC_CONNECTION, con);

#if OCI_VERSION_COMPILE >= OCI_10_2

    if (con->env->version_runtime >= OCI_10_2)
    {
        CHECK_ATTRIB_GET
        (
            OCI_HTYPE_SERVER, OCI_ATTR_TAF_ENABLED,
            con->svr, &value, NULL,
            con->err
        );
    }

#endif

    SET_RETVAL(value);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibConnectionSetTAFHandler
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibConnectionSetTAFHandler
(
    OCI_Connection  *con,
    POCI_TAF_HANDLER handler
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_CONNECTION, con
    );

    CHECK_PTR(OCI_IPC_CONNECTION, con);

    CHECK(OcilibConnectionIsTAFCapable(con));

#if OCI_VERSION_COMPILE >= OCI_10_2

    if (con->env->version_runtime >= OCI_10_2)
    {
        OCIFocbkStruct fo_struct;

        memset(&fo_struct, 0, sizeof(fo_struct));

        con->taf_handler = handler;

        if (con->taf_handler)
        {
            fo_struct.callback_function = (OCICallbackFailover)OcilibCallbackFailOver;
            fo_struct.fo_ctx            = (dvoid *) con;
        }

        CHECK_ATTRIB_SET
        (
            OCI_HTYPE_SERVER, OCI_ATTR_FOCBK,
            con->svr, &fo_struct, 0,
            con->err
        );
    }

#endif

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibConnectionGetStatementCacheSize
 * --------------------------------------------------------------------------------------------- */

OCI_UInt32 OcilibConnectionGetStatementCacheSize
(
    OCI_Connection *con
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_UInt32, 0,
        /* context */ OCI_IPC_CONNECTION, con
    );

    ub4 cache_size = 0;

    CHECK_PTR(OCI_IPC_CONNECTION, con);

#if OCI_VERSION_COMPILE >= OCI_9_2

    if (con->env->version_runtime >= OCI_9_2)
    {
        CHECK_ATTRIB_GET
        (
            OCI_HTYPE_SVCCTX, OCI_ATTR_STMTCACHESIZE,
            con->handle, &cache_size, NULL,
            con->err
        );
    }

#endif

    SET_RETVAL(cache_size);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibConnectionSetStatementCacheSize
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibConnectionSetStatementCacheSize
(
    OCI_Connection *con,
    OCI_UInt32      value
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_CONNECTION, con
    );

    ub4 cache_size = value;

    CHECK_PTR(OCI_IPC_CONNECTION, con);

#if OCI_VERSION_COMPILE >= OCI_9_2

    if (con->env->version_runtime >= OCI_9_2)
    {
        CHECK_ATTRIB_SET
        (
            OCI_HTYPE_SVCCTX, OCI_ATTR_STMTCACHESIZE,
            con->handle, &cache_size, sizeof(cache_size),
            con->err
        );
    }

#else

    OCI_NOT_USED(cache_size);

#endif

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibConnectionGetDefaultLobPrefetchSize
 * --------------------------------------------------------------------------------------------- */

OCI_UInt32 OcilibConnectionGetDefaultLobPrefetchSize
(
    OCI_Connection *con
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_UInt32, 0,
        /* context */ OCI_IPC_CONNECTION, con
    );

    ub4 prefetch_size = 0;

    CHECK_PTR(OCI_IPC_CONNECTION, con);

#if OCI_VERSION_COMPILE >= OCI_11_1

    if (OcilibConnectionIsVersionSupported(con, OCI_11_1))
    {
        CHECK_ATTRIB_GET
        (
            OCI_HTYPE_SESSION, OCI_ATTR_DEFAULT_LOBPREFETCH_SIZE,
            con->ses, &prefetch_size, NULL,
            con->err
        );
    }

#endif

    SET_RETVAL(prefetch_size);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibConnectionSetDefaultLobPrefetchSize
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibConnectionSetDefaultLobPrefetchSize
(
    OCI_Connection *con,
    OCI_UInt32      value
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_CONNECTION, con
    );

    ub4 prefetch_size = value;

    OCI_Boolean success = FALSE;

    CHECK_PTR(OCI_IPC_CONNECTION, con);

#if OCI_VERSION_COMPILE >= OCI_11_1

    if (OcilibConnectionIsVersionSupported(con, OCI_11_1))
    {
        CHECK_ATTRIB_SET
        (
            OCI_HTYPE_SESSION, OCI_ATTR_DEFAULT_LOBPREFETCH_SIZE,
            con->ses, &prefetch_size, sizeof(prefetch_size),
            con->err
        );

        success = TRUE;
    }

#else

    OCI_NOT_USED(prefetch_size);

#endif

    SET_RETVAL(success);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibConnectionGetMaxCursors
 * --------------------------------------------------------------------------------------------- */

OCI_UInt32 OcilibConnectionGetMaxCursors
(
    OCI_Connection *con
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_UInt32, 0,
        /* context */ OCI_IPC_CONNECTION, con
    );

    ub4 max_cursors = 0;

    CHECK_PTR(OCI_IPC_CONNECTION, con);

#if OCI_VERSION_COMPILE >= OCI_12_1

    if (OcilibConnectionIsVersionSupported(con, OCI_11_1))
    {
        CHECK_ATTRIB_GET
        (
            OCI_HTYPE_SESSION, OCI_ATTR_MAX_OPEN_CURSORS,
            con->ses, &max_cursors, NULL,
            con->err
        );
    }

#endif

    SET_RETVAL(max_cursors);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibConnectionImmediate
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibConnectionImmediate
(
    OCI_Connection *con,
    const otext    *sql,
    va_list         args
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_CONNECTION, con
    );

    OCI_Statement *stmt = NULL;

    CHECK_PTR(OCI_IPC_CONNECTION, con);
    CHECK_PTR(OCI_IPC_STRING,     sql);

    /* create statement */

    stmt = OcilibStatementCreate(con);
    CHECK_NULL(stmt);

    /* First, execute SQL */

    CHECK(OcilibStatementExecute(stmt, sql));

    /* get resultset and set up variables */

    if (OCI_CST_SELECT == OcilibStatementGetType(stmt))
    {
        CHECK(OcilibStatementFetchIntoUserVariables(stmt, args));
    }

    SET_SUCCESS();

    CLEANUP_AND_EXIT_FUNC
    (
        if (NULL != stmt)
        {
            OcilibStatementFree(stmt);
        }
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibConnectionImmediateFormat
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibConnectionImmediateFormat
(
    OCI_Connection *con,
    const otext    *sql,
    va_list         args
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_CONNECTION, con
    );

    OCI_Statement *stmt    = NULL;
    otext        * sql_fmt = NULL;

    va_list first_pass_args;
    va_list second_pass_args;

    va_copy(first_pass_args,  args);
    va_copy(second_pass_args, args);

    CHECK_PTR(OCI_IPC_CONNECTION, con);
    CHECK_PTR(OCI_IPC_STRING,     sql);

    /* create statement */

    stmt = OcilibStatementCreate(con);
    CHECK_NULL(stmt);

    /* first, get buffer size */

    OCI_Int32 size = OcilibFormatParseSql(stmt, NULL, sql, &first_pass_args);
    CHECK(size > 0);

    /* allocate buffer */

    ALLOC_DATA(OCI_IPC_STRING, sql_fmt, size + 1);

    /* format buffer */

    size = OcilibFormatParseSql(stmt, sql_fmt, sql, &second_pass_args);
    CHECK(size > 0);

    /* prepare and execute SQL buffer */

    CHECK(OcilibStatementPrepareInternal(stmt, sql_fmt));
    CHECK(OcilibStatementExecuteInternal(stmt, OCI_DEFAULT));

    /* get resultset and set up variables */

    if (OCI_CST_SELECT == OcilibStatementGetType(stmt))
    {
        CHECK(OcilibStatementFetchIntoUserVariables(stmt, second_pass_args));
    }

    SET_SUCCESS();

    CLEANUP_AND_EXIT_FUNC
    (
        va_end(first_pass_args);
        va_end(second_pass_args);

        if (NULL != stmt)
        {
            OcilibStatementFree(stmt);
        }

        FREE(sql_fmt);
    );
}
