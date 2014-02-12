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
 * $Id: subscriptions.c, Vincent Rogier $
 * --------------------------------------------------------------------------------------------- */

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
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_NOTIFY, sub, FALSE);

#if OCI_VERSION_COMPILE >= OCI_10_2

    /* deregister the subscription if connection still alive */

    if (sub->subhp)
    {
        if (!sub->con)
        {
            sub->con = OCI_ConnectionCreate(sub->saved_db, sub->saved_user,
                                            sub->saved_pwd, OCI_SESSION_DEFAULT);
        }

        if (sub->con)
        {
            OCI_CALL3
            (
                res, sub->err,

                OCISubscriptionUnRegister(sub->con->cxt, sub->subhp,
                                          sub->err,(ub4) OCI_DEFAULT)
            )

            OCI_ConnectionFree(sub->con);
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

    OCI_FREE(sub->event.dbname);
    OCI_FREE(sub->event.objname);
    OCI_FREE(sub->event.rowid);

    /* free strings */

    OCI_FREE(sub->saved_db);
    OCI_FREE(sub->saved_user);
    OCI_FREE(sub->saved_pwd);
    OCI_FREE(sub->name);

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

    OCI_CHECK(list == NULL, FALSE);

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
 * OCI_SubscriptionCreate
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
    boolean res           = FALSE;

    OCI_CHECK_INITIALIZED(NULL);
    OCI_CHECK_DATABASE_NOTIFY_ENABLED(NULL);

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, NULL);
    OCI_CHECK_PTR(OCI_IPC_PROC, handler, NULL);
    OCI_CHECK_PTR(OCI_IPC_STRING, name, NULL);

#if OCI_VERSION_COMPILE >= OCI_10_2

    /* create subscription object */

    item = OCI_ListAppend(OCILib.subs, sizeof(*sub));

    if (item)
    {
        sub = (OCI_Subscription *) item->data;

        /* allocate error handle */

        res = OCI_SUCCESSFUL(OCI_HandleAlloc(con->env,
                                             (dvoid **) (void *) &sub->err,
                                             OCI_HTYPE_ERROR, (size_t) 0,
                                             (dvoid **) NULL));

        if (res)
        {
            /* allocate subcription handle */

            res = OCI_SUCCESSFUL(OCI_HandleAlloc(con->env,
                                                 (dvoid **) (void *) &sub->subhp,
                                                 OCI_HTYPE_SUBSCRIPTION, (size_t) 0,
                                                 (dvoid **) NULL));
        }

        if (res)
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
                    res, sub->err,

                    OCIAttrSet((dvoid *) sub->subhp, (ub4)  OCI_HTYPE_SUBSCRIPTION,
                               (dvoid *) &sub->port, (ub4) sizeof (sub->port),
                               (ub4) OCI_ATTR_SUBSCR_PORTNO, sub->err)
                )
            }
            else
            {
                OCI_CALL3
                (
                    res, sub->err,

                    OCIAttrGet((dvoid *) sub->subhp, (ub4) OCI_HTYPE_SUBSCRIPTION,
                               (dvoid *) &sub->port, (ub4) 0,
                               (ub4) OCI_ATTR_SUBSCR_PORTNO, sub->err)
                )
            }

            /* set timeout */

            if(sub->timeout > 0)
            {
                OCI_CALL3
                (
                    res, sub->err,

                    OCIAttrSet((dvoid *) sub->subhp, (ub4) OCI_HTYPE_SUBSCRIPTION,
                               (dvoid *) &sub->timeout, (ub4) sizeof (sub->timeout),
                               (ub4) OCI_ATTR_SUBSCR_TIMEOUT, sub->err)
                )
            }

            /* name  */

            dbstr = OCI_StringGetOracleString(sub->name, &dbsize);

            OCI_CALL3
            (
                res, sub->err,

                OCIAttrSet((dvoid *) sub->subhp, (ub4) OCI_HTYPE_SUBSCRIPTION,
                           (dvoid *) dbstr, (ub4) dbsize,
                           (ub4) OCI_ATTR_SUBSCR_NAME, sub->err)
            )

            OCI_StringReleaseOracleString(dbstr);

            /* namespace for CDN */

            attr =  OCI_SUBSCR_NAMESPACE_DBCHANGE;

            OCI_CALL3
            (
                res, sub->err,

                OCIAttrSet((dvoid *) sub->subhp, (ub4) OCI_HTYPE_SUBSCRIPTION,
                           (dvoid *) &attr, (ub4) sizeof(attr),
                           (ub4) OCI_ATTR_SUBSCR_NAMESPACE, sub->err)
            )

            /* protocol for CDN */

            attr =  OCI_SUBSCR_PROTO_OCI;

            OCI_CALL3
            (
                res, sub->err,

                OCIAttrSet((dvoid *) sub->subhp, (ub4) OCI_HTYPE_SUBSCRIPTION,
                           (dvoid *) &attr, (ub4) sizeof(attr),
                           (ub4) OCI_ATTR_SUBSCR_RECPTPROTO, sub->err)
            )

            /* internal callback handler */

            OCI_CALL3
            (
                res, sub->err,

                OCIAttrSet((dvoid *) sub->subhp, (ub4) OCI_HTYPE_SUBSCRIPTION,
                           (dvoid *) OCI_ProcNotifyChanges, (ub4) 0,
                           (ub4) OCI_ATTR_SUBSCR_CALLBACK, sub->err)
            )

            /* RowIds handling */

            if (sub->type & OCI_CNT_ROWS)
            {
                attr = TRUE;

                OCI_CALL3
                (
                    res, sub->err,

                    OCIAttrSet((dvoid *) sub->subhp, (ub4) OCI_HTYPE_SUBSCRIPTION,
                               (dvoid *) &attr, (ub4) sizeof(attr),
                               (ub4) OCI_ATTR_CHNF_ROWIDS, sub->err)
                )
            }

            /* set subsription context pointer to our subscription structure */

            OCI_CALL3
            (
                res, sub->err,

                OCIAttrSet((dvoid *) sub->subhp, (ub4) OCI_HTYPE_SUBSCRIPTION,
                           (dvoid *) sub, (ub4) 0,
                           (ub4) OCI_ATTR_SUBSCR_CTX, sub->err)
            )

            /* all attributes set, let's register the subscription ! */

            OCI_CALL3
            (
                res, sub->err,

                OCISubscriptionRegister(sub->con->cxt, &sub->subhp, (ub2) 1,
                                        sub->err,(ub4) OCI_DEFAULT)

            )
        }
    }

    if (!res)
    {
        OCI_SubscriptionClose(sub);
        OCI_ListRemove(OCILib.subs, sub);
        OCI_FREE(sub);
    }

#else

    OCI_NOT_USED(name);
    OCI_NOT_USED(type);
    OCI_NOT_USED(handler);
    OCI_NOT_USED(port);
    OCI_NOT_USED(timeout);
    OCI_NOT_USED(con);
    OCI_NOT_USED(item);

#endif

    OCI_RESULT(res);

    return sub;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_SubscriptionUnregister
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_SubscriptionUnregister
(
    OCI_Subscription *sub
)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_NOTIFY, sub, FALSE);

    res = OCI_SubscriptionClose(sub);

    OCI_ListRemove(OCILib.subs, sub);

    OCI_FREE(sub);

    OCI_RESULT(res);

    return res;
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
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_NOTIFY, sub, FALSE);
    OCI_CHECK_PTR(OCI_IPC_STATEMENT, stmt, FALSE);

    OCI_CHECK_STMT_STATUS(stmt, OCI_STMT_PREPARED, FALSE);

#if OCI_VERSION_COMPILE >= OCI_10_2

    /* register the statement query if provided */

    if (sub->type & OCI_CNT_OBJECTS)
    {
        OCI_CALL3
        (
            res, sub->err,

            OCIAttrSet((dvoid *) stmt->stmt, (ub4) OCI_HTYPE_STMT,
                       (dvoid *) sub->subhp, (ub4) 0,
                       (ub4) OCI_ATTR_CHNF_REGHANDLE, sub->err)
        )

        res = res && OCI_Execute(stmt) && (OCI_GetResultset(stmt) != NULL);
    }

#endif

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_SubscriptionGetName
 * --------------------------------------------------------------------------------------------- */

const otext * OCI_API OCI_SubscriptionGetName
(
    OCI_Subscription *sub
)
{
    OCI_CHECK_PTR(OCI_IPC_NOTIFY, sub, NULL);

    OCI_RESULT(TRUE);

    return sub->name;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_SubscriptionGetPort
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_SubscriptionGetPort
(
    OCI_Subscription *sub
)
{
    OCI_CHECK_PTR(OCI_IPC_NOTIFY, sub, 0);

    OCI_RESULT(TRUE);

    return sub->port;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_SubscriptionGetTimeout
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_SubscriptionGetTimeout
(
    OCI_Subscription *sub
)
{
    OCI_CHECK_PTR(OCI_IPC_NOTIFY, sub, 0);

    OCI_RESULT(TRUE);

    return sub->timeout;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_SubscriptionGetConnection
 * --------------------------------------------------------------------------------------------- */

OCI_Connection * OCI_API OCI_SubscriptionGetConnection
(
    OCI_Subscription *sub
)
{
    OCI_CHECK_PTR(OCI_IPC_NOTIFY, sub, NULL);

    OCI_RESULT(TRUE);

    return sub->con;
}


