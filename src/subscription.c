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
 *                             PRIVATE FUNCTIONS
 * ********************************************************************************************* */

/* --------------------------------------------------------------------------------------------- *
 * OCI_SubscriptionClose
 * --------------------------------------------------------------------------------------------- */

boolean OCI_SubscriptionClose
(
    OCI_Subscription *sub
)
{
    boolean res   = TRUE;
    boolean alloc = FALSE;

    OCI_CHECK(NULL == sub, FALSE);

#if OCI_VERSION_COMPILE >= OCI_10_2

    /* unregister the subscription if connection still alive */

    if (sub->subhp)
    {
        if (!sub->con)
        {
            sub->con = OCI_ConnectionCreate(sub->saved_db, sub->saved_user,
                                            sub->saved_pwd, OCI_SESSION_DEFAULT);

            alloc = TRUE;
        }

        if (sub->con)
        {
            OCI_CALL3
            (
                res, sub->err,

                OCISubscriptionUnRegister(sub->con->cxt, sub->subhp,
                                          sub->err,(ub4) OCI_DEFAULT)
            )

            if (alloc)
            {
                OCI_ConnectionFree(sub->con);
            }
        }
    }

    /* free OCI handle */

    OCI_HandleFree((dvoid *) sub->subhp, OCI_HTYPE_SUBSCRIPTION);

    /* close error handle */

    if (sub->err)
    {
        OCI_HandleFree(sub->err, OCI_HTYPE_ERROR);
    }

#endif

    /* free event data */

    OCI_FREE(sub->event.dbname)
    OCI_FREE(sub->event.objname)
    OCI_FREE(sub->event.rowid)

    /* free strings */

    OCI_FREE(sub->saved_db)
    OCI_FREE(sub->saved_user)
    OCI_FREE(sub->saved_pwd)
    OCI_FREE(sub->name)

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_SubscriptionDetachConnection
 * --------------------------------------------------------------------------------------------- */

boolean OCI_SubscriptionDetachConnection
(
    OCI_Connection *con
)
{
    OCI_List *list = OCILib.subs;
    OCI_Item *item = NULL;

    OCI_CHECK(NULL == list, FALSE);

    if (list->mutex)
    {
        OCI_MutexAcquire(list->mutex);
    }

    item = list->head;

    /* for each item in the list, check the connection */

    while (item)
    {
        OCI_Subscription * sub = (OCI_Subscription *) item->data;

        if (sub && (sub->con == con))
        {
            sub->con = NULL;

            sub->saved_db   = ostrdup(con->db);
            sub->saved_user = ostrdup(con->user);
            sub->saved_pwd  = ostrdup(con->pwd);
        }

        item = item->next;
    }

    if (list->mutex)
    {
        OCI_MutexRelease(list->mutex);
    }

    return TRUE;
}

/* ********************************************************************************************* *
 *                            PUBLIC FUNCTIONS
 * ********************************************************************************************* */

/* --------------------------------------------------------------------------------------------- *
 * OCI_SubscriptionRegister
 * --------------------------------------------------------------------------------------------- */

OCI_Subscription * OCI_API OCI_SubscriptionRegister
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
    OCI_Item *item        = NULL;

    OCI_LIB_CALL_ENTER(OCI_Subscription*, NULL)

    OCI_CHECK_DATABASE_NOTIFY_ENABLED()
    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con)
    OCI_CHECK_PTR(OCI_IPC_PROC, handler)
    OCI_CHECK_PTR(OCI_IPC_STRING, name)

#if OCI_VERSION_COMPILE >= OCI_10_2

    /* create subscription object */

    item = OCI_ListAppend(OCILib.subs, sizeof(*sub));

    if (item)
    {
        sub = (OCI_Subscription *) item->data;

        /* allocate error handle */

        call_status = OCI_SUCCESSFUL(OCI_HandleAlloc(con->env,
                                                     (dvoid **) (void *) &sub->err,
                                                     OCI_HTYPE_ERROR, (size_t) 0,
                                                     (dvoid **) NULL));

        if (call_status)
        {
            /* allocate subscription handle */

            call_status = OCI_SUCCESSFUL(OCI_HandleAlloc(con->env,
                                                         (dvoid **) (void *) &sub->subhp,
                                                         OCI_HTYPE_SUBSCRIPTION, (size_t) 0,
                                                         (dvoid **) NULL));
        }

        if (call_status)
        {
            ub4     attr   = 0;
            int     dbsize = -1;
            dbtext *dbstr  = NULL;

            sub->con       = con;
            sub->env       = con->env;
            sub->port      = (ub4) port;
            sub->timeout   = (ub4) timeout;
            sub->handler   = handler;
            sub->type      = type;
            sub->name      = ostrdup(name);
            sub->event.sub = sub;

            /* set/get port number */

            if (sub->port > 0)
            {
                OCI_CALL3
                (
                    call_status, sub->err,

                    OCIAttrSet((dvoid *) sub->subhp, (ub4)  OCI_HTYPE_SUBSCRIPTION,
                               (dvoid *) &sub->port, (ub4) sizeof (sub->port),
                               (ub4) OCI_ATTR_SUBSCR_PORTNO, sub->err)
                )
            }
            else
            {
                OCI_CALL3
                (
                    call_status, sub->err,

                    OCIAttrGet((dvoid *) sub->subhp, (ub4) OCI_HTYPE_SUBSCRIPTION,
                               (dvoid *) &sub->port, (ub4*) NULL,
                               (ub4) OCI_ATTR_SUBSCR_PORTNO, sub->err)
                )
            }

            /* set timeout */

            if(sub->timeout > 0)
            {
                OCI_CALL3
                (
                    call_status, sub->err,

                    OCIAttrSet((dvoid *) sub->subhp, (ub4) OCI_HTYPE_SUBSCRIPTION,
                               (dvoid *) &sub->timeout, (ub4) sizeof (sub->timeout),
                               (ub4) OCI_ATTR_SUBSCR_TIMEOUT, sub->err)
                )
            }

            /* name  */

            dbstr = OCI_StringGetOracleString(sub->name, &dbsize);

            OCI_CALL3
            (
                call_status, sub->err,

                OCIAttrSet((dvoid *) sub->subhp, (ub4) OCI_HTYPE_SUBSCRIPTION,
                           (dvoid *) dbstr, (ub4) dbsize,
                           (ub4) OCI_ATTR_SUBSCR_NAME, sub->err)
            )

            OCI_StringReleaseOracleString(dbstr);

            /* namespace for CDN */

            attr =  OCI_SUBSCR_NAMESPACE_DBCHANGE;

            OCI_CALL3
            (
                call_status, sub->err,

                OCIAttrSet((dvoid *) sub->subhp, (ub4) OCI_HTYPE_SUBSCRIPTION,
                           (dvoid *) &attr, (ub4) sizeof(attr),
                           (ub4) OCI_ATTR_SUBSCR_NAMESPACE, sub->err)
            )

            /* protocol for CDN */

            attr =  OCI_SUBSCR_PROTO_OCI;

            OCI_CALL3
            (
                call_status, sub->err,

                OCIAttrSet((dvoid *) sub->subhp, (ub4) OCI_HTYPE_SUBSCRIPTION,
                           (dvoid *) &attr, (ub4) sizeof(attr),
                           (ub4) OCI_ATTR_SUBSCR_RECPTPROTO, sub->err)
            )

           /* On MSVC, casting a function pointer to a data pointer generates a warning.
              As there is no other to way to do regarding the OCI API, let's disable this
              warning just the time to set the callback attribute to the subscription handle */

            #ifdef _MSC_VER
            #pragma warning(disable: 4054)
            #endif

            /* internal callback handler */

            OCI_CALL3
            (
                call_status, sub->err,

                OCIAttrSet((dvoid *) sub->subhp, (ub4) OCI_HTYPE_SUBSCRIPTION,
                           (dvoid *) OCI_ProcNotifyChanges, (ub4) 0,
                           (ub4) OCI_ATTR_SUBSCR_CALLBACK, sub->err)
            )

            #ifdef _MSC_VER
            #pragma warning(default: 4054)
            #endif

            /* RowIds handling */

            if (sub->type & OCI_CNT_ROWS)
            {
                attr = TRUE;

                OCI_CALL3
                (
                    call_status, sub->err,

                    OCIAttrSet((dvoid *) sub->subhp, (ub4) OCI_HTYPE_SUBSCRIPTION,
                               (dvoid *) &attr, (ub4) sizeof(attr),
                               (ub4) OCI_ATTR_CHNF_ROWIDS, sub->err)
                )
            }

            /* set subscription context pointer to our subscription structure */

            OCI_CALL3
            (
                call_status, sub->err,

                OCIAttrSet((dvoid *) sub->subhp, (ub4) OCI_HTYPE_SUBSCRIPTION,
                           (dvoid *) sub, (ub4) 0,
                           (ub4) OCI_ATTR_SUBSCR_CTX, sub->err)
            )

            /* all attributes set, let's register the subscription ! */

            OCI_CALL3
            (
                call_status, sub->err,

                OCISubscriptionRegister(sub->con->cxt, &sub->subhp, (ub2) 1,
                                        sub->err,(ub4) OCI_DEFAULT)

            )
        }
    }

    if (call_status)
    {
        call_retval = sub;
    } 
    else if (sub)
    {
        OCI_SubscriptionClose(sub);
        OCI_ListRemove(OCILib.subs, sub);
        OCI_FREE(sub)
    }

#else

    OCI_NOT_USED(name)
    OCI_NOT_USED(type)
    OCI_NOT_USED(handler)
    OCI_NOT_USED(port)
    OCI_NOT_USED(timeout)
    OCI_NOT_USED(con)
    OCI_NOT_USED(item)

#endif

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_SubscriptionUnregister
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_SubscriptionUnregister
(
    OCI_Subscription *sub
)
{
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_NOTIFY, sub)

    call_retval = call_status = OCI_SubscriptionClose(sub);

    OCI_ListRemove(OCILib.subs, sub);

    OCI_FREE(sub);

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_SubscriptionAddStatement
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_SubscriptionAddStatement
(
    OCI_Subscription *sub,
    OCI_Statement    *stmt
)
{
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_NOTIFY, sub)
    OCI_CHECK_PTR(OCI_IPC_STATEMENT, stmt)
    OCI_CHECK_STMT_STATUS(stmt, OCI_STMT_PREPARED)

    call_status = TRUE;

#if OCI_VERSION_COMPILE >= OCI_10_2

    /* register the statement query if provided */

    if (sub->type & OCI_CNT_OBJECTS)
    {
        OCI_CALL3
        (
            call_status, sub->err,

            OCIAttrSet((dvoid *) stmt->stmt, (ub4) OCI_HTYPE_STMT,
                       (dvoid *) sub->subhp, (ub4) 0,
                       (ub4) OCI_ATTR_CHNF_REGHANDLE, sub->err)
        )

        call_status = call_status && OCI_Execute(stmt) && (NULL != OCI_GetResultset(stmt));
    }

#endif

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_SubscriptionGetName
 * --------------------------------------------------------------------------------------------- */

const otext * OCI_API OCI_SubscriptionGetName
(
    OCI_Subscription *sub
)
{
    OCI_LIB_CALL_ENTER(const otext*, NULL)

    OCI_CHECK_PTR(OCI_IPC_NOTIFY, sub)

    call_retval = sub->name;
    call_status = TRUE;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_SubscriptionGetPort
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_SubscriptionGetPort
(
    OCI_Subscription *sub
)
{
    OCI_LIB_CALL_ENTER(unsigned int, 0)

    OCI_CHECK_PTR(OCI_IPC_NOTIFY, sub)

    call_retval = sub->port;
    call_status = TRUE;

    OCI_LIB_CALL_EXIT()}

/* --------------------------------------------------------------------------------------------- *
 * OCI_SubscriptionGetTimeout
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_SubscriptionGetTimeout
(
    OCI_Subscription *sub
)
{
    OCI_LIB_CALL_ENTER(unsigned int, 0)

    OCI_CHECK_PTR(OCI_IPC_NOTIFY, sub)

    call_retval = sub->timeout;
    call_status = TRUE;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_SubscriptionGetConnection
 * --------------------------------------------------------------------------------------------- */

OCI_Connection * OCI_API OCI_SubscriptionGetConnection
(
    OCI_Subscription *sub
)
{
    OCI_LIB_CALL_ENTER(OCI_Connection*, NULL)

    OCI_CHECK_PTR(OCI_IPC_NOTIFY, sub)

    call_retval = sub->con;
    call_status = TRUE;

    OCI_LIB_CALL_EXIT()}


