/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2026 Vincent ROGIER <vince.rogier@ocilib.net>
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

#include "subscription.h"

#include "callback.h"
#include "connection.h"
#include "list.h"
#include "macros.h"
#include "memory.h"
#include "statement.h"
#include "stringutils.h"

/* --------------------------------------------------------------------------------------------- *
 * OcilibSubscriptionDispose
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibSubscriptionDispose
(
    OCI_Subscription *sub
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_NOTIFY, sub
    );

    OCI_Boolean alloc     = FALSE;
    sword       unreg_res = OCI_SUCCESS;

    CHECK_PTR(OCI_IPC_NOTIFY, sub);

#if OCI_VERSION_COMPILE >= OCI_10_2

    /* unregister the subscription if connection still alive */

    if (NULL != sub->handle)
    {
        if (NULL == sub->con)
        {
            sub->con = OcilibConnectionCreate(sub->env, sub->saved_db, sub->saved_user,
                                              sub->saved_pwd, OCI_SESSION_DEFAULT);

            CHECK_NULL(sub->con);

            alloc = TRUE;
        }

        if (NULL != sub->con)
        {
            /* OCISubscriptionUnRegister() seems to partially fail when OCI is initialized in UTF16
             * mode as it returns ORA-24915 */

            /* Thus, if using OCI in Unicode mode, discard  error ORA-24915 */

            /* a failure to unregister is not fatal: it is reported after all
               resources are freed, otherwise every subsequent free would be
               skipped and leaked */

  #if defined(OCI_CHARSET_WIDE)

            sword res;

            OCI_CALL(res, OCISubscriptionUnRegister, sub->con->handle, sub->handle, sub->err, (ub4)sub->mode);
            if (OCI_FAILURE(res))
            {
                sb4 code = 0;

                OCI_ERROR_GET(sub->err, &code, NULL, 0);

                if (OCI_ERR_SUB_BUG_OCI_UTF16 != code)
                {
                    unreg_res = res;
                }
            }
  #else

            OCI_CALL(unreg_res, OCISubscriptionUnRegister, sub->con->handle, sub->handle, sub->err, (ub4)sub->mode);

  #endif

            if (alloc && NULL != sub->con)
            {
                OcilibConnectionFree(sub->con);
            }
        }
    }

    /* free OCI handle */

    OcilibMemoryFreeHandle(sub->env, (dvoid*)sub->handle, OCI_HTYPE_SUBSCRIPTION);

    /* report the deferred unregister error while the error handle is
       still valid */

    if (OCI_FAILURE(unreg_res))
    {
        OcilibExceptionOCI(&ocilib_call_context, sub->err, unreg_res);
    }

    /* close error handle */

    if (NULL != sub->err)
    {
        OcilibMemoryFreeHandle(sub->env, sub->err, OCI_HTYPE_ERROR);
    }

#endif

    /* free event data */

    FREE(sub->event.dbname);
    FREE(sub->event.objname);
    FREE(sub->event.rowid);

    /* free strings */

    FREE(sub->saved_db);
    FREE(sub->saved_user);
    FREE(sub->saved_pwd);
    FREE(sub->name);

    CHECK(!OCI_FAILURE(unreg_res));

    SET_SUCCESS();

    CLEANUP_AND_EXIT_FUNC
    (
        OcilibErrorResetSource(NULL, sub);
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibSubscriptionCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Subscription * OcilibSubscriptionCreate
(
    OCI_Connection      *con,
    const otext         *name,
    OCI_SubscriptionType type,
    POCI_NOTIFY          handler,
    OCI_SubscriptionMode mode,
    OCI_UInt32           timeout
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Subscription*, NULL,
        /* context */ OCI_IPC_CONNECTION, con
    );

    ub4       attr   = 0;
    OCI_Int32 dbsize = -1;
    dbtext   *dbstr  = NULL;

    OCI_Subscription* sub = NULL;

    CHECK_PTR(OCI_IPC_CONNECTION, con);
    CHECK_DATABASE_NOTIFY_ENABLED();
    CHECK_PTR(OCI_IPC_PROC,       handler);
    CHECK_PTR(OCI_IPC_STRING,     name);

#if OCI_VERSION_COMPILE >= OCI_10_2

    /* OCI_SRM_SECURE_NOTIFICATION requires an Oracle client and database 19.4 or above */

    if (mode & OCI_SRM_SECURE_NOTIFICATION && !OcilibConnectionIsVersionSupported(con, OCI_19_4))
    {
        THROW(OcilibExceptionNotAvailable, OCI_FEATURE_SECURE_NOTIFICATION);
    }

    /* create subscription object */

    LIST_ATOMIC_ADD(con->env->subs, sub);

    CHECK_NULL(sub);

    /* allocate error handle */

    CHECK(OcilibMemoryAllocHandle(con->env, con->env->handle, (dvoid **)(void *)&sub->err, OCI_HTYPE_ERROR));

    /* allocate subscription handle */

    CHECK(OcilibMemoryAllocHandle(con->env, con->env->handle, (dvoid **)(void *)&sub->handle, OCI_HTYPE_SUBSCRIPTION));

    sub->con       = con;
    sub->env       = con->env;
    sub->htype     = OCI_IPC_NOTIFY;
    sub->timeout   = (ub4)timeout;
    sub->handler   = handler;
    sub->type      = type;
    sub->mode      = mode;
    sub->name      = OcilibStringDuplicate(sub->con->env, name);
    sub->event.sub = sub;

    /* set timeout */

    if (sub->timeout > 0)
    {
        CHECK_ATTRIB_SET
        (
            OCI_HTYPE_SUBSCRIPTION, OCI_ATTR_SUBSCR_TIMEOUT,
            sub->handle, &sub->timeout,
            sizeof(sub->timeout),
            sub->con->err
        );
    }

    /* name  */

    dbstr = OcilibStringGetDBString(sub->con->env, sub->name, &dbsize);

    CHECK_ATTRIB_SET
    (
        OCI_HTYPE_SUBSCRIPTION, OCI_ATTR_SUBSCR_NAME,
        sub->handle, dbstr, dbsize,
        sub->con->err
    );

    /* namespace for CDN */

    attr =  OCI_SUBSCR_NAMESPACE_DBCHANGE;
    CHECK_ATTRIB_SET
    (
        OCI_HTYPE_SUBSCRIPTION, OCI_ATTR_SUBSCR_NAMESPACE,
        sub->handle, &attr, sizeof(attr),
        sub->con->err
    );

    /* protocol for CDN */

    attr =  OCI_SUBSCR_PROTO_OCI;
    CHECK_ATTRIB_SET
    (
        OCI_HTYPE_SUBSCRIPTION, OCI_ATTR_SUBSCR_RECPTPROTO,
        sub->handle, &attr, sizeof(attr),
        sub->con->err
    );

    /* On MSVC, casting a function pointer to a data pointer generates a warning.
       As there is no other way to do regarding the OCI API, let's disable this
       warning just the time to set the callback attribute to the subscription handle */

    WARNING_DISABLE_CAST_FUNC_TYPE
    WARNING_DISABLE_PEDANTIC

    /* internal callback handler */

    CHECK_ATTRIB_SET
    (
        OCI_HTYPE_SUBSCRIPTION,
        OCI_ATTR_SUBSCR_CALLBACK,
        sub->handle,
        OcilibCallbackNotifyChanges,
        0,
        sub->con->err
    );

    WARNING_RESTORE_CAST_FUNC_TYPE
        WARNING_RESTORE_PEDANTIC

    /* RowIds handling */

    if (sub->type & OCI_CNT_ROWS)
    {
        attr = TRUE;
        CHECK_ATTRIB_SET
        (
            OCI_HTYPE_SUBSCRIPTION, OCI_ATTR_CHNF_ROWIDS,
            sub->handle, &attr, sizeof(attr),
            sub->con->err
        );
    }

    /* set subscription context pointer to our subscription structure */

    CHECK_ATTRIB_SET
    (
        OCI_HTYPE_SUBSCRIPTION, OCI_ATTR_SUBSCR_CTX,
        sub->handle, sub, 0,
        sub->con->err
    );

    /* all attributes set, let's register the subscription ! */

    CHECK_OCI
    (
        sub->con->err,
        OCISubscriptionRegister,
        sub->con->handle, &sub->handle,
        (ub2)1, sub->con->err, (ub4)mode
    );

#else

    OCI_NOT_USED(name);
    OCI_NOT_USED(type);
    OCI_NOT_USED(handler);
    OCI_NOT_USED(timeout);
    OCI_NOT_USED(mode);
    OCI_NOT_USED(con);

#endif

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE)
        {
            OcilibStringReleaseDBString(sub->con->env, dbstr);
            OcilibSubscriptionUnregister(sub);
            sub = NULL;
        }

        SET_RETVAL(sub);
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibSubscriptionUnregister
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibSubscriptionUnregister
(
    OCI_Subscription *sub
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_NOTIFY, sub
    );

    CHECK_PTR(OCI_IPC_NOTIFY, sub);

    LIST_ATOMIC_REMOVE(sub->env->subs, sub, OcilibSubscriptionDispose);

    FREE(sub);

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibSubscriptionAddStatement
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibSubscriptionAddStatement
(
    OCI_Subscription *sub,
    OCI_Statement    *stmt
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_NOTIFY, sub
    );

    CHECK_PTR(OCI_IPC_NOTIFY,    sub);
    CHECK_PTR(OCI_IPC_STATEMENT, stmt);
    CHECK_STMT_STATUS(stmt, OCI_STMT_PREPARED);

#if OCI_VERSION_COMPILE >= OCI_10_2

    /* register the statement query if provided */

    if (sub->type & (OCI_CNT_OBJECTS | OCI_CNT_ROWS))
    {
        CHECK_ATTRIB_SET
        (
            OCI_HTYPE_STMT, OCI_ATTR_CHNF_REGHANDLE,
            stmt->handle, sub->handle, 0,
            stmt->con->err
        );

        CHECK(OcilibStatementExecutePrepared(stmt));
        CHECK_NULL(OcilibStatementGetResultset(stmt));
    }

#endif

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibSubscriptionGetName
 * --------------------------------------------------------------------------------------------- */

const otext * OcilibSubscriptionGetName
(
    OCI_Subscription *sub
)
{
    GET_PROP
    (
        const otext*, NULL,
        OCI_IPC_NOTIFY, sub,
        name
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibSubscriptionGetTimeout
 * --------------------------------------------------------------------------------------------- */

OCI_UInt32 OcilibSubscriptionGetTimeout
(
    OCI_Subscription *sub
)
{
    GET_PROP
    (
        OCI_UInt32, 0,
        OCI_IPC_NOTIFY, sub,
        timeout
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibSubscriptionGetConnection
 * --------------------------------------------------------------------------------------------- */

OCI_Connection * OcilibSubscriptionGetConnection
(
    OCI_Subscription *sub
)
{
    GET_PROP
    (
        OCI_Connection*, NULL,
        OCI_IPC_NOTIFY, sub,
        con
    );
}
