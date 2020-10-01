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

#include "subscription.h"

#include "callback.h"
#include "connection.h"
#include "list.h"
#include "macros.h"
#include "memory.h"
#include "statement.h"
#include "stringutils.h"

/* --------------------------------------------------------------------------------------------- *
 * SubscriptionDispose
 * --------------------------------------------------------------------------------------------- */

boolean SubscriptionDispose
(
    OCI_Subscription *sub
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_NOTIFY, sub
    )

    boolean alloc = FALSE;

    CHECK_PTR(OCI_IPC_NOTIFY, sub)

#if OCI_VERSION_COMPILE >= OCI_10_2

    /* unregister the subscription if connection still alive */

    if (NULL != sub->subhp)
    {
        if (NULL == sub->con)
        {
            sub->con = ConnectionCreate(sub->saved_db, sub->saved_user,
                                        sub->saved_pwd, OCI_SESSION_DEFAULT);

            CHECK_NULL(sub->con)

            alloc = TRUE;
        }

        if (NULL != sub->con)
        {
            /* OCISubscriptionUnRegister() seems to partially fail when OCI is initialized in UTF16
             * mode as it returns ORA-24915 */

            /* Thus, if using OCI in Unicode mode, discard  error ORA-24915 */

  #if defined(OCI_CHARSET_WIDE)

            const sword res = OCISubscriptionUnRegister(sub->con->cxt, sub->subhp, sub->err, (ub4)OCI_DEFAULT);
            if (OCI_FAILURE(res))
            {
                sb4 code = 0;

                OCIErrorGet((dvoid *)sub->err, (ub4)1, (OraText *)NULL, &code, (OraText *)NULL, (ub4)0, (ub4)OCI_HTYPE_ERROR);

                if (OCI_ERR_SUB_BUG_OCI_UTF16 != code)
                {
                    THROW(ExceptionOCI, sub->err, res)
                }
            }
  #else

            CHECK_OCI
            (
                sub->err,
                OCISubscriptionUnRegister,
                sub->con->cxt, sub->subhp,
                sub->err, (ub4)OCI_DEFAULT
            )

  #endif

            if (alloc && NULL != sub->con)
            {
                ConnectionFree(sub->con);
            }
        }
    }

    /* free OCI handle */

    MemoryFreeHandle((dvoid*)sub->subhp, OCI_HTYPE_SUBSCRIPTION);

    /* close error handle */

    if (NULL != sub->err)
    {
        MemoryFreeHandle(sub->err, OCI_HTYPE_ERROR);
    }

#endif

    /* free event data */

    FREE(sub->event.dbname)
    FREE(sub->event.objname)
    FREE(sub->event.rowid)

    /* free strings */

    FREE(sub->saved_db)
    FREE(sub->saved_user)
    FREE(sub->saved_pwd)
    FREE(sub->name)

    SET_SUCCESS()

    CLEANUP_AND_EXIT_FUNC
    (
        ErrorResetSource(NULL, sub);
    )
}

/* --------------------------------------------------------------------------------------------- *
 * SubscriptionRegister
 * --------------------------------------------------------------------------------------------- */

OCI_Subscription * SubscriptionRegister
(
    OCI_Connection *con,
    const otext    *name,
    unsigned int    type,
    POCI_NOTIFY     handler,
    unsigned int    port,
    unsigned int    timeout
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Subscription*, NULL,
        /* context */ OCI_IPC_CONNECTION, con
    )

    ub4 attr = 0;
    int     dbsize = -1;
    dbtext *dbstr  = NULL;

    OCI_Subscription* sub = NULL;

    CHECK_DATABASE_NOTIFY_ENABLED()
    CHECK_PTR(OCI_IPC_CONNECTION, con)
    CHECK_PTR(OCI_IPC_PROC,       handler)
    CHECK_PTR(OCI_IPC_STRING,     name)

#if OCI_VERSION_COMPILE >= OCI_10_2

    /* create subscription object */

    sub = ListAppend(Env.subs, sizeof(*sub));
    CHECK_NULL(sub)

    /* allocate error handle */

    CHECK(MemoryAllocHandle(con->env, (dvoid **)(void *)&sub->err, OCI_HTYPE_ERROR))

    /* allocate subscription handle */

    CHECK(MemoryAllocHandle(con->env, (dvoid **)(void *)&sub->subhp, OCI_HTYPE_SUBSCRIPTION))

    sub->con       = con;
    sub->env       = con->env;
    sub->timeout   = (ub4)timeout;
    sub->handler   = handler;
    sub->type      = type;
    sub->name      = ostrdup(name);
    sub->event.sub = sub;

    /* set port number */

    if (port > 0)
    {
        CHECK_ATTRIB_SET
        (
            OCI_HTYPE_ENV, OCI_ATTR_SUBSCR_PORTNO,
            sub->env, &port, sizeof(port),
            sub->con->err
        )
    }

    /* set timeout */

    if (sub->timeout > 0)
    {
        CHECK_ATTRIB_SET
        (
            OCI_HTYPE_SUBSCRIPTION, OCI_ATTR_SUBSCR_TIMEOUT,
            sub->subhp, &sub->timeout,
            sizeof(sub->timeout),
            sub->con->err
        )
    }

    /* name  */

    dbstr = StringGetDBString(sub->name, &dbsize);

    CHECK_ATTRIB_SET
    (
        OCI_HTYPE_SUBSCRIPTION, OCI_ATTR_SUBSCR_NAME,
        sub->subhp, dbstr, dbsize,
        sub->con->err
    )

    /* namespace for CDN */

    attr =  OCI_SUBSCR_NAMESPACE_DBCHANGE;
    CHECK_ATTRIB_SET
    (
        OCI_HTYPE_SUBSCRIPTION, OCI_ATTR_SUBSCR_NAMESPACE,
        sub->subhp, &attr, sizeof(attr),
        sub->con->err
    )

    /* protocol for CDN */

    attr =  OCI_SUBSCR_PROTO_OCI;
    CHECK_ATTRIB_SET
    (
        OCI_HTYPE_SUBSCRIPTION, OCI_ATTR_SUBSCR_RECPTPROTO,
        sub->subhp, &attr, sizeof(attr),
        sub->con->err
    )

    /* On MSVC, casting a function pointer to a data pointer generates a warning.
       As there is no other to way to do regarding the OCI API, let's disable this
       warning just the time to set the callback attribute to the subscription handle */

  #ifdef _MSC_VER
    #pragma warning(disable: 4054)
  #endif

    /* internal callback handler */

    CHECK_ATTRIB_SET
    (
        OCI_HTYPE_SUBSCRIPTION, OCI_ATTR_SUBSCR_CALLBACK,
        sub->subhp, CallbackNotifyChanges, 0,
        sub->con->err
    )

  #ifdef _MSC_VER
    #pragma warning(default: 4054)
  #endif

    /* RowIds handling */

    if (sub->type & OCI_CNT_ROWS)
    {
        attr = TRUE;
        CHECK_ATTRIB_SET
        (
            OCI_HTYPE_SUBSCRIPTION, OCI_ATTR_CHNF_ROWIDS,
            sub->subhp, &attr, sizeof(attr),
            sub->con->err
        )
    }

    /* set subscription context pointer to our subscription structure */

    CHECK_ATTRIB_SET
    (
        OCI_HTYPE_SUBSCRIPTION, OCI_ATTR_SUBSCR_CTX,
        sub->subhp, sub, 0,
        sub->con->err
    )

    /* all attributes set, let's register the subscription ! */

    CHECK_OCI
    (
        sub->con->err,
        OCISubscriptionRegister,
        sub->con->cxt, &sub->subhp,
        (ub2)1, sub->err, (ub4)OCI_DEFAULT
    )

    /* get real port number */

    CHECK_ATTRIB_GET
    (
        OCI_HTYPE_ENV, OCI_ATTR_SUBSCR_PORTNO,
        sub->env, &sub->port, sizeof(sub->port),
        sub->con->err
    )

#else

    OCI_NOT_USED(name)
    OCI_NOT_USED(type)
    OCI_NOT_USED(handler)
    OCI_NOT_USED(port)
    OCI_NOT_USED(timeout)
    OCI_NOT_USED(con)

#endif

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE)
        {
            StringReleaseDBString(dbstr);

            SubscriptionDispose(sub);
            ListRemove(Env.subs, sub);
            FREE(sub)
        }
    )
}

/* --------------------------------------------------------------------------------------------- *
 * SubscriptionUnregister
 * --------------------------------------------------------------------------------------------- */

boolean SubscriptionUnregister
(
    OCI_Subscription *sub
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_NOTIFY, sub
    )

    CHECK_PTR(OCI_IPC_NOTIFY, sub)

    SubscriptionDispose(sub);
    ListRemove(Env.subs, sub);

    FREE(sub);

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * SubscriptionAddStatement
 * --------------------------------------------------------------------------------------------- */

boolean SubscriptionAddStatement
(
    OCI_Subscription *sub,
    OCI_Statement    *stmt
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_NOTIFY, sub
    )

    CHECK_PTR(OCI_IPC_NOTIFY,    sub)
    CHECK_PTR(OCI_IPC_STATEMENT, stmt)
    CHECK_STMT_STATUS(stmt, OCI_STMT_PREPARED)

#if OCI_VERSION_COMPILE >= OCI_10_2

    /* register the statement query if provided */

    if (sub->type & OCI_CNT_OBJECTS)
    {
        CHECK_ATTRIB_SET
        (
            OCI_HTYPE_STMT, OCI_ATTR_CHNF_REGHANDLE,
            stmt->stmt, sub->subhp, 0,
            stmt->con->err
        )

        CHECK(StatementExecute(stmt))
        CHECK_NULL(StatementGetResultset(stmt))
    }

#endif

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * SubscriptionGetName
 * --------------------------------------------------------------------------------------------- */

const otext * SubscriptionGetName
(
    OCI_Subscription *sub
)
{
    GET_PROP
    (
        const otext*, NULL,
        OCI_IPC_NOTIFY, sub,
        name
    )
}

/* --------------------------------------------------------------------------------------------- *
 * SubscriptionGetPort
 * --------------------------------------------------------------------------------------------- */

unsigned int SubscriptionGetPort
(
    OCI_Subscription *sub
)
{
    GET_PROP
    (
        unsigned int, 0,
        OCI_IPC_NOTIFY, sub,
        port
    )
}

/* --------------------------------------------------------------------------------------------- *
 * SubscriptionGetTimeout
 * --------------------------------------------------------------------------------------------- */

unsigned int SubscriptionGetTimeout
(
    OCI_Subscription *sub
)
{
    GET_PROP
    (
        unsigned int, 0,
        OCI_IPC_NOTIFY, sub,
        timeout
    )
}

/* --------------------------------------------------------------------------------------------- *
 * SubscriptionGetConnection
 * --------------------------------------------------------------------------------------------- */

OCI_Connection * SubscriptionGetConnection
(
    OCI_Subscription *sub
)
{
    GET_PROP
    (
        OCI_Connection*, NULL,
        OCI_IPC_NOTIFY, sub,
        con
    )
}
