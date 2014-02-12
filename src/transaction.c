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
 * $Id: transaction.c, Vincent Rogier $
 * --------------------------------------------------------------------------------------------- */

#include "ocilib_internal.h"

/* ********************************************************************************************* *
 *                            PUBLIC FUNCTIONS
 * ********************************************************************************************* */

/* --------------------------------------------------------------------------------------------- *
 * OCI_TransactionCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Transaction * OCI_API OCI_TransactionCreate
(
    OCI_Connection *con,
    unsigned int    timeout,
    unsigned int    mode,
    OCI_XID        *pxid
)
{
    OCI_Item *item         = NULL;
    OCI_Transaction *trans = NULL;
    boolean res            = FALSE;

    OCI_CHECK_INITIALIZED(NULL);

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, NULL);

    /* create transaction object */

    item = OCI_ListAppend(con->trsns, sizeof(*trans));

    if (item)
    {
        trans = (OCI_Transaction *) item->data;

        trans->con     = con;
        trans->mode    = mode;
        trans->timeout = timeout;
        trans->local   = (pxid == NULL);

        /* allocate transaction handle */

        res = OCI_SUCCESSFUL(OCI_HandleAlloc((dvoid *) trans->con->env,
                                             (dvoid **) (void *) &trans->htr,
                                             (ub4) OCI_HTYPE_TRANS,
                                             (size_t) 0, (dvoid **) NULL));

        /* set XID attribute for global transaction */

        if (pxid)
        {
            memcpy(&trans->xid, pxid, sizeof(trans->xid));

            OCI_CALL2
            (
                res, con,

                OCIAttrSet((dvoid *) trans->htr, (ub4) OCI_HTYPE_TRANS,
                           (dvoid *) &trans->xid, (ub4) sizeof(trans->xid),
                           (ub4) OCI_ATTR_XID, trans->con->err)
            )
        }
    }

    /* handle errors */

    if (!res)
    {
        OCI_TransactionFree(trans);
        trans = NULL;
    }

    OCI_RESULT(res);

    return trans;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_TransactionClose
 * --------------------------------------------------------------------------------------------- */

boolean OCI_TransactionClose
(
    OCI_Transaction * trans
)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_TRANSACTION, trans, FALSE);

    res = OCI_TransactionStop(trans);

    /* close transaction handle */

    if (trans->htr)
    {
        OCI_HandleFree((dvoid *) trans->htr, (ub4) OCI_HTYPE_TRANS);
    }

    if (trans->con->trs == trans)
    {
        trans->con->trs = NULL;
    }

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_TransactionFree
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_TransactionFree
(
    OCI_Transaction * trans
)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_TRANSACTION, trans, FALSE);

    res = OCI_TransactionClose(trans);

    /* remove transaction from internal list */

    OCI_ListRemove(trans->con->trsns, trans);

    OCI_FREE(trans);

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_TransactionStart
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_TransactionStart
(
    OCI_Transaction * trans
)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_TRANSACTION, trans, FALSE);

    OCI_CALL2
    (
        res, trans->con,

        OCITransStart(trans->con->cxt, trans->con->err, (uword) trans->timeout,
                      (ub4) trans->mode)
    )

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_TransactionStop
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_TransactionStop
(
    OCI_Transaction * trans
)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_TRANSACTION, trans, FALSE);

    /* commit or rollback upon auto commit mode */

    res = trans->con->autocom ? OCI_Commit(trans->con) : OCI_Rollback(trans->con);

    /* detach global transaction */

    if (!trans->local)
    {
        OCI_CALL2
        (
            res, trans->con,

            OCITransDetach(trans->con->cxt, trans->con->err, (ub4) OCI_DEFAULT)
        )
    }

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_TransactionResume
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_TransactionResume
(
    OCI_Transaction * trans
)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_TRANSACTION, trans, FALSE);

    OCI_CALL2
    (
        res, trans->con,

        OCITransStart(trans->con->cxt, trans->con->err,
                      (uword) trans->timeout, (ub4) OCI_TRANS_RESUME)
    )

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_TransactionPrepare
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_TransactionPrepare
(
    OCI_Transaction * trans
)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_TRANSACTION, trans, FALSE);

    OCI_CALL2
    (
        res, trans->con,

        OCITransPrepare(trans->con->cxt, trans->con->err, (ub4) OCI_DEFAULT)
    )

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_TransactionForget
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_TransactionForget
(
    OCI_Transaction * trans
)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_TRANSACTION, trans, FALSE);

    OCI_CALL2
    (
        res, trans->con,

        OCITransForget(trans->con->cxt, trans->con->err, (ub4) OCI_DEFAULT)
    )

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_TransactionGetMode
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_TransactionGetMode
(
    OCI_Transaction * trans
)
{
    OCI_CHECK_PTR(OCI_IPC_TRANSACTION, trans, OCI_UNKNOWN);

    OCI_RESULT(TRUE);

    return trans->mode;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_TransactionGetTimeout
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_TransactionGetTimeout
(
    OCI_Transaction * trans
)
{
    OCI_CHECK_PTR(OCI_IPC_TRANSACTION, trans, 0);

    OCI_RESULT(TRUE);

    return trans->timeout;
}
