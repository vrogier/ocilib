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

#include "transaction.h"

#include "connection.h"
#include "list.h"
#include "macros.h"

/* --------------------------------------------------------------------------------------------- *
* TransactionDispose
* --------------------------------------------------------------------------------------------- */

boolean TransactionDispose
(
OCI_Transaction * trans
)
{
    const boolean res = TransactionStop(trans);

    /* close transaction handle */

    if (trans->htr)
    {
        MemoryFreeHandle((dvoid *)trans->htr, OCI_HTYPE_TRANS);
    }

    if (trans->con->trs == trans)
    {
        trans->con->trs = NULL;
    }

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * TransactionCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Transaction * TransactionCreate
(
    OCI_Connection *con,
    unsigned int    timeout,
    unsigned int    mode,
    OCI_XID        *pxid
)
{
    OCI_Transaction *trans = NULL;

    CALL_ENTER(OCI_Transaction *, NULL)
    CALL_CHECK_PTR(OCI_IPC_CONNECTION, con)
    CALL_CONTEXT_FROM_CON(con)

    /* create transaction object */

    trans = ListAppend(con->trsns, sizeof(*trans));
    STATUS = (NULL != trans);
    
    if (STATUS)
    {
        trans->con = con;
        trans->mode = mode;
        trans->timeout = timeout;
        trans->local = (NULL == pxid);

        /* allocate transaction handle */

        STATUS = MemoryAllocHandle((dvoid *)trans->con->env, (dvoid **)&trans->htr, OCI_HTYPE_TRANS);

        /* set XID attribute for global transaction */

        if (STATUS && pxid)
        {
            memcpy(&trans->xid, pxid, sizeof(trans->xid));

            ATTRIB_SET(OCI_HTYPE_TRANS, OCI_ATTR_XID, trans->htr, &trans->xid, sizeof(trans->xid))
        }
    }

    /* handle errors */

    if (STATUS)
    {
        RETVAL = trans;
    }
    else if (trans)
    {
        TransactionFree(trans);
    }

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * TransactionFree
 * --------------------------------------------------------------------------------------------- */

boolean TransactionFree
(
    OCI_Transaction * trans
)
{
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_TRANSACTION, trans)
    CALL_CONTEXT_FROM_CON(trans->con)

    STATUS = TransactionDispose(trans);

    /* remove transaction from internal list */

    ListRemove(trans->con->trsns, trans);

    FREE(trans)

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * TransactionStart
 * --------------------------------------------------------------------------------------------- */

boolean TransactionStart
(
    OCI_Transaction * trans
)
{
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_TRANSACTION, trans)
    CALL_CONTEXT_FROM_CON(trans->con)

    EXEC(OCITransStart(trans->con->cxt, trans->con->err, (uword) trans->timeout,  (ub4) trans->mode))

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * TransactionStop
 * --------------------------------------------------------------------------------------------- */

boolean TransactionStop
(
    OCI_Transaction * trans
)
{
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_TRANSACTION, trans)
    CALL_CONTEXT_FROM_CON(trans->con)

    /* commit or rollback upon auto commit mode */

    STATUS = trans->con->autocom ? ConnectionCommit(trans->con) : ConnectionRollback(trans->con);

    /* detach global transaction */

    if (STATUS && !trans->local)
    {
        EXEC(OCITransDetach(trans->con->cxt, trans->con->err, (ub4) OCI_DEFAULT))
    }

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * TransactionResume
 * --------------------------------------------------------------------------------------------- */

boolean TransactionResume
(
    OCI_Transaction * trans
)
{
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_TRANSACTION, trans)
    CALL_CONTEXT_FROM_CON(trans->con)

    EXEC(OCITransStart(trans->con->cxt, trans->con->err, (uword) trans->timeout, (ub4) OCI_TRANS_RESUME))

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * TransactionPrepare
 * --------------------------------------------------------------------------------------------- */

boolean TransactionPrepare
(
    OCI_Transaction * trans
)
{
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_TRANSACTION, trans)
    CALL_CONTEXT_FROM_CON(trans->con)

    EXEC(OCITransPrepare(trans->con->cxt, trans->con->err, (ub4) OCI_DEFAULT))

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * TransactionForget
 * --------------------------------------------------------------------------------------------- */

boolean TransactionForget
(
    OCI_Transaction * trans
)
{
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_TRANSACTION, trans)
    CALL_CONTEXT_FROM_CON(trans->con)

    EXEC(OCITransForget(trans->con->cxt, trans->con->err, (ub4) OCI_DEFAULT))

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * TransactionGetMode
 * --------------------------------------------------------------------------------------------- */

unsigned int TransactionGetMode
(
    OCI_Transaction * trans
)
{
    GET_PROP(unsigned int, OCI_UNKNOWN, OCI_IPC_TRANSACTION, trans, mode, trans->con, NULL, trans->con->err);
}

/* --------------------------------------------------------------------------------------------- *
 * TransactionGetTimeout
 * --------------------------------------------------------------------------------------------- */

unsigned int TransactionGetTimeout
(
    OCI_Transaction * trans
)
{
    GET_PROP(unsigned int, 0, OCI_IPC_TRANSACTION, trans, timeout, trans->con, NULL, trans->con->err);
}
