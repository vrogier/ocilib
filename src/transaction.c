/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2018 Vincent ROGIER <vince.rogier@ocilib.net>
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
 *                            PRIVATE FUNCTIONS
 * ********************************************************************************************* */

/* --------------------------------------------------------------------------------------------- *
* OCI_TransactionClose
* --------------------------------------------------------------------------------------------- */

boolean OCI_TransactionClose
(
OCI_Transaction * trans
)
{
    const boolean res = OCI_TransactionStop(trans);

    /* close transaction handle */

    if (trans->htr)
    {
        OCI_HandleFree((dvoid *)trans->htr, OCI_HTYPE_TRANS);
    }

    if (trans->con->trs == trans)
    {
        trans->con->trs = NULL;
    }

    return res;
}

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
    OCI_Transaction *trans = NULL;

    OCI_CALL_ENTER(OCI_Transaction *, NULL)
    OCI_CALL_CHECK_PTR(OCI_IPC_CONNECTION, con)
    OCI_CALL_CONTEXT_SET_FROM_CONN(con)

    /* create transaction object */

    trans = OCI_ListAppend(con->trsns, sizeof(*trans));
    OCI_STATUS = (NULL != trans);
    
    if (OCI_STATUS)
    {
        trans->con = con;
        trans->mode = mode;
        trans->timeout = timeout;
        trans->local = (NULL == pxid);

        /* allocate transaction handle */

        OCI_STATUS = OCI_HandleAlloc((dvoid *)trans->con->env, (dvoid **)&trans->htr, OCI_HTYPE_TRANS);

        /* set XID attribute for global transaction */

        if (OCI_STATUS && pxid)
        {
            memcpy(&trans->xid, pxid, sizeof(trans->xid));

            OCI_SET_ATTRIB(OCI_HTYPE_TRANS, OCI_ATTR_XID, trans->htr, &trans->xid, sizeof(trans->xid))
        }
    }

    /* handle errors */

    if (OCI_STATUS)
    {
        OCI_RETVAL = trans;
    }
    else if (trans)
    {
        OCI_TransactionFree(trans);
    }

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_TransactionFree
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_TransactionFree
(
    OCI_Transaction * trans
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_TRANSACTION, trans)
    OCI_CALL_CONTEXT_SET_FROM_CONN(trans->con)

    OCI_STATUS = OCI_TransactionClose(trans);

    /* remove transaction from internal list */

    OCI_ListRemove(trans->con->trsns, trans);

    OCI_FREE(trans)

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_TransactionStart
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_TransactionStart
(
    OCI_Transaction * trans
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_TRANSACTION, trans)
    OCI_CALL_CONTEXT_SET_FROM_CONN(trans->con)

    OCI_EXEC(OCITransStart(trans->con->cxt, trans->con->err, (uword) trans->timeout,  (ub4) trans->mode))

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_TransactionStop
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_TransactionStop
(
    OCI_Transaction * trans
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_TRANSACTION, trans)
    OCI_CALL_CONTEXT_SET_FROM_CONN(trans->con)

    /* commit or rollback upon auto commit mode */

    OCI_STATUS = trans->con->autocom ? OCI_Commit(trans->con) : OCI_Rollback(trans->con);

    /* detach global transaction */

    if (OCI_STATUS && !trans->local)
    {
        OCI_EXEC(OCITransDetach(trans->con->cxt, trans->con->err, (ub4) OCI_DEFAULT))
    }

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_TransactionResume
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_TransactionResume
(
    OCI_Transaction * trans
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_TRANSACTION, trans)
    OCI_CALL_CONTEXT_SET_FROM_CONN(trans->con)

    OCI_EXEC(OCITransStart(trans->con->cxt, trans->con->err, (uword) trans->timeout, (ub4) OCI_TRANS_RESUME))

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_TransactionPrepare
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_TransactionPrepare
(
    OCI_Transaction * trans
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_TRANSACTION, trans)
    OCI_CALL_CONTEXT_SET_FROM_CONN(trans->con)

    OCI_EXEC(OCITransPrepare(trans->con->cxt, trans->con->err, (ub4) OCI_DEFAULT))

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_TransactionForget
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_TransactionForget
(
    OCI_Transaction * trans
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_TRANSACTION, trans)
    OCI_CALL_CONTEXT_SET_FROM_CONN(trans->con)

    OCI_EXEC(OCITransForget(trans->con->cxt, trans->con->err, (ub4) OCI_DEFAULT))

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_TransactionGetMode
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_TransactionGetMode
(
    OCI_Transaction * trans
)
{
    OCI_GET_PROP(unsigned int, OCI_UNKNOWN, OCI_IPC_TRANSACTION, trans, mode, trans->con, NULL, trans->con->err);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_TransactionGetTimeout
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_TransactionGetTimeout
(
    OCI_Transaction * trans
)
{
    OCI_GET_PROP(unsigned int, 0, OCI_IPC_TRANSACTION, trans, timeout, trans->con, NULL, trans->con->err);
}
