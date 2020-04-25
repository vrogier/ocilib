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
#include "strings.h"

/* --------------------------------------------------------------------------------------------- *
 * SubscriptionDispose
 * --------------------------------------------------------------------------------------------- */

boolean SubscriptionDispose
(
    OCI_Subscription *sub
)
{
    boolean alloc = FALSE;

    DECLARE_CTX(TRUE)

    CHECK(NULL == sub, FALSE);

    CTX_SET(sub->con, NULL, sub->err)

#if OCI_VERSION_COMPILE >= OCI_10_2

    /* unregister the subscription if connection still alive */

    if (sub->subhp)
    {
        if (!sub->con)
        {
            sub->con = ConnectionCreate(sub->saved_db, sub->saved_user,
                                        sub->saved_pwd, OCI_SESSION_DEFAULT);

            alloc = TRUE;
        }

        if (sub->con)
        {
            /* OCISubscriptionUnRegister() seems to partially fail when OCI is initialized in UTF16
             * mode as it returns ORA-24915 */

            /* Thus, if using OCI in Unicode mode, discard  error ORA-24915 */

            const sword res = OCISubscriptionUnRegister(sub->con->cxt, sub->subhp, sub->err, (ub4) OCI_DEFAULT);

  #if defined(OCI_CHARSET_WIDE)

            if (OCI_FAILURE(res))
            {
                sb4 code = 0;

                OCIErrorGet((dvoid *)sub->err, (ub4)1, (OraText *)NULL, &code, (OraText *)NULL, (ub4)0, (ub4)OCI_HTYPE_ERROR);

                if (OCI_ERR_SUB_BUG_OCI_UTF16 != code)
                {
                    ExceptionOCI(sub->err, sub->con, NULL, FALSE);
                    STATUS = FALSE;
                }
            }
  #else

            EXEC(res);
  #endif

            if (alloc)
            {
                ConnectionFree(sub->con);
            }
        }
    }

    /* free OCI handle */

    MemoryFreeHandle((dvoid *) sub->subhp, OCI_HTYPE_SUBSCRIPTION);

    /* close error handle */

    if (sub->err)
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

    return STATUS;
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
    OCI_Subscription *sub = NULL;

    CALL_ENTER(OCI_Subscription*, NULL)
    CALL_CHECK_DATABASE_NOTIFY_ENABLED()
    CALL_CHECK_PTR(OCI_IPC_CONNECTION, con)
    CALL_CHECK_PTR(OCI_IPC_PROC, handler)
    CALL_CHECK_PTR(OCI_IPC_STRING, name)
    CALL_CONTEXT_FROM_CON(con)

#if OCI_VERSION_COMPILE >= OCI_10_2

    /* create subscription object */

    sub = ListAppend(Env.subs, sizeof(*sub));

    STATUS = (NULL != sub);

    if (sub)
    {
        /* allocate error handle */

        STATUS = MemoryAllocHandle(con->env, (dvoid **)(void *)&sub->err, OCI_HTYPE_ERROR);

        /* allocate subscription handle */

        STATUS = STATUS && MemoryAllocHandle(con->env, (dvoid **)(void *)&sub->subhp, OCI_HTYPE_SUBSCRIPTION);

        if (STATUS)
        {
            ub4     attr   = 0;
            int     dbsize = -1;
            dbtext *dbstr  = NULL;

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
                ATTRIB_SET(OCI_HTYPE_ENV, OCI_ATTR_SUBSCR_PORTNO, sub->env, &port, sizeof(port))
            }

            /* set timeout */

            if (sub->timeout > 0)
            {
                ATTRIB_SET(OCI_HTYPE_SUBSCRIPTION, OCI_ATTR_SUBSCR_TIMEOUT, sub->subhp, &sub->timeout, sizeof(sub->timeout))
            }

            /* name  */

            dbstr = StringGetDBString(sub->name, &dbsize);

            ATTRIB_SET(OCI_HTYPE_SUBSCRIPTION, OCI_ATTR_SUBSCR_NAME, sub->subhp, dbstr, dbsize)

            StringReleaseDBString(dbstr);

            /* namespace for CDN */

            attr =  OCI_SUBSCR_NAMESPACE_DBCHANGE;
            ATTRIB_SET(OCI_HTYPE_SUBSCRIPTION, OCI_ATTR_SUBSCR_NAMESPACE, sub->subhp, &attr, sizeof(attr))

            /* protocol for CDN */

            attr =  OCI_SUBSCR_PROTO_OCI;
            ATTRIB_SET(OCI_HTYPE_SUBSCRIPTION, OCI_ATTR_SUBSCR_RECPTPROTO, sub->subhp, &attr, sizeof(attr))

            /* On MSVC, casting a function pointer to a data pointer generates a warning.
               As there is no other to way to do regarding the OCI API, let's disable this
               warning just the time to set the callback attribute to the subscription handle */

  #ifdef _MSC_VER
    #pragma warning(disable: 4054)
  #endif

            /* internal callback handler */

            ATTRIB_SET(OCI_HTYPE_SUBSCRIPTION, OCI_ATTR_SUBSCR_CALLBACK, sub->subhp, CallbackNotifyChanges, 0)

  #ifdef _MSC_VER
    #pragma warning(default: 4054)
  #endif

            /* RowIds handling */

            if (sub->type & OCI_CNT_ROWS)
            {
                attr = TRUE;
                ATTRIB_SET(OCI_HTYPE_SUBSCRIPTION, OCI_ATTR_CHNF_ROWIDS, sub->subhp, &attr, sizeof(attr))
            }

            /* set subscription context pointer to our subscription structure */

            ATTRIB_SET(OCI_HTYPE_SUBSCRIPTION, OCI_ATTR_SUBSCR_CTX, sub->subhp, sub, 0)

            /* all attributes set, let's register the subscription ! */

            EXEC(OCISubscriptionRegister(sub->con->cxt, &sub->subhp, (ub2) 1, sub->err,(ub4) OCI_DEFAULT))

            /* get real port number */

            ATTRIB_GET(OCI_HTYPE_ENV, OCI_ATTR_SUBSCR_PORTNO, sub->env, &sub->port, sizeof(sub->port))
        }
    }

    if (STATUS)
    {
        RETVAL = sub;
    }
    else if (sub)
    {
        SubscriptionDispose(sub);
        ListRemove(Env.subs, sub);
        FREE(sub)
    }

#else

    OCI_NOT_USED(name)
    OCI_NOT_USED(type)
    OCI_NOT_USED(handler)
    OCI_NOT_USED(port)
    OCI_NOT_USED(timeout)
    OCI_NOT_USED(con)

#endif

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * SubscriptionUnregister
 * --------------------------------------------------------------------------------------------- */

boolean SubscriptionUnregister
(
    OCI_Subscription *sub
)
{
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_NOTIFY, sub)
    CTX_SET(sub->con, NULL, sub->err)

    RETVAL = STATUS = SubscriptionDispose(sub);

    ListRemove(Env.subs, sub);

    FREE(sub);

    CALL_EXIT()
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
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_NOTIFY, sub)
    CALL_CHECK_PTR(OCI_IPC_STATEMENT, stmt)
    CALL_CHECK_STMT_STATUS(stmt, OCI_STMT_PREPARED)

    CTX_SET(sub->con, stmt, sub->err)

#if OCI_VERSION_COMPILE >= OCI_10_2

    /* register the statement query if provided */

    if (sub->type & OCI_CNT_OBJECTS)
    {
        ATTRIB_SET(OCI_HTYPE_STMT, OCI_ATTR_CHNF_REGHANDLE, stmt->stmt, sub->subhp, 0)

        STATUS = STATUS && StatementExecute(stmt) && (NULL != StatementGetResultset(stmt));
    }

#endif

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * SubscriptionGetName
 * --------------------------------------------------------------------------------------------- */

const otext * SubscriptionGetName
(
    OCI_Subscription *sub
)
{
    GET_PROP(const otext*, NULL, OCI_IPC_NOTIFY, sub, name, sub->con, NULL, sub->err)
}

/* --------------------------------------------------------------------------------------------- *
 * SubscriptionGetPort
 * --------------------------------------------------------------------------------------------- */

unsigned int SubscriptionGetPort
(
    OCI_Subscription *sub
)
{
    GET_PROP(unsigned int, 0, OCI_IPC_NOTIFY, sub, port, sub->con, NULL, sub->err)
}

/* --------------------------------------------------------------------------------------------- *
 * SubscriptionGetTimeout
 * --------------------------------------------------------------------------------------------- */

unsigned int SubscriptionGetTimeout
(
    OCI_Subscription *sub
)
{
    GET_PROP(unsigned int, 0, OCI_IPC_NOTIFY, sub, timeout, sub->con, NULL, sub->err)
}

/* --------------------------------------------------------------------------------------------- *
 * SubscriptionGetConnection
 * --------------------------------------------------------------------------------------------- */

OCI_Connection * SubscriptionGetConnection
(
    OCI_Subscription *sub
)
{
    GET_PROP(OCI_Connection*, NULL, OCI_IPC_NOTIFY, sub, con, sub->con, NULL, sub->err)
}
