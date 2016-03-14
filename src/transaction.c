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
    boolean res = OCI_TransactionStop(trans);

    /* close transaction handle */

    if (trans->htr)
    {
        OCI_HandleFree((dvoid *)trans->htr, (ub4)OCI_HTYPE_TRANS);
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
    OCI_Item *item = NULL;

    OCI_LIB_CALL_ENTER(OCI_Transaction *, NULL)

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con)

    /* create transaction object */

    item = OCI_ListAppend(con->trsns, sizeof(*call_retval));

    if (item)
    {
        call_retval = (OCI_Transaction *) item->data;

        call_retval->con     = con;
        call_retval->mode    = mode;
        call_retval->timeout = timeout;
        call_retval->local   = (NULL == pxid);

        /* allocate transaction handle */

        call_status = OCI_SUCCESSFUL(OCI_HandleAlloc((dvoid *) call_retval->con->env,
                                                     (dvoid **) &call_retval->htr,
                                                     (ub4) OCI_HTYPE_TRANS,
                                                     (size_t) 0, (dvoid **) NULL));

        /* set XID attribute for global transaction */

        if (call_status && pxid)
        {
            memcpy(&call_retval->xid, pxid, sizeof(call_retval->xid));

            OCI_CALL2
            (
                call_status, con,

                OCIAttrSet((dvoid *) call_retval->htr, (ub4) OCI_HTYPE_TRANS,
                           (dvoid *) &call_retval->xid, (ub4) sizeof(call_retval->xid),
                           (ub4) OCI_ATTR_XID, call_retval->con->err)
            )
        }
    }

    /* handle errors */

    if (!call_status)
    {
        OCI_TransactionFree(call_retval);
        call_retval = NULL;
    }

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_TransactionFree
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_TransactionFree
(
    OCI_Transaction * trans
)
{
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_TRANSACTION, trans)

    call_status = OCI_TransactionClose(trans);

    /* remove transaction from internal list */

    OCI_ListRemove(trans->con->trsns, trans);

    OCI_FREE(trans)

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_TransactionStart
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_TransactionStart
(
    OCI_Transaction * trans
)
{
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_TRANSACTION, trans)

    call_status = TRUE;

    OCI_CALL2
    (
        call_status, trans->con,

        OCITransStart(trans->con->cxt, trans->con->err, (uword) trans->timeout,
                      (ub4) trans->mode)
    )

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_TransactionStop
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_TransactionStop
(
    OCI_Transaction * trans
)
{
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_TRANSACTION, trans)

    /* commit or rollback upon auto commit mode */

    call_status = trans->con->autocom ? OCI_Commit(trans->con) : OCI_Rollback(trans->con);

    /* detach global transaction */

    if (call_status && !trans->local)
    {
        OCI_CALL2
        (
            call_status, trans->con,

            OCITransDetach(trans->con->cxt, trans->con->err, (ub4) OCI_DEFAULT)
        )
    }

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_TransactionResume
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_TransactionResume
(
    OCI_Transaction * trans
)
{
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_TRANSACTION, trans)

    call_status = TRUE;

    OCI_CALL2
    (
        call_status, trans->con,

        OCITransStart(trans->con->cxt, trans->con->err,
                      (uword) trans->timeout, (ub4) OCI_TRANS_RESUME)
    )

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_TransactionPrepare
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_TransactionPrepare
(
    OCI_Transaction * trans
)
{
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_TRANSACTION, trans)

    call_status = TRUE;

    OCI_CALL2
    (
        call_status, trans->con,

        OCITransPrepare(trans->con->cxt, trans->con->err, (ub4) OCI_DEFAULT)
    )

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_TransactionForget
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_TransactionForget
(
    OCI_Transaction * trans
)
{
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_TRANSACTION, trans)

    call_status = TRUE;

    OCI_CALL2
    (
        call_status, trans->con,

        OCITransForget(trans->con->cxt, trans->con->err, (ub4) OCI_DEFAULT)
    )

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_TransactionGetMode
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_TransactionGetMode
(
    OCI_Transaction * trans
)
{
    OCI_LIB_CALL_ENTER(unsigned int, OCI_UNKNOWN)

    OCI_CHECK_PTR(OCI_IPC_TRANSACTION, trans)

    call_retval = trans->mode;
    call_status = TRUE;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_TransactionGetTimeout
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_TransactionGetTimeout
(
    OCI_Transaction * trans
)
{
    OCI_LIB_CALL_ENTER(unsigned int, 0)

    OCI_CHECK_PTR(OCI_IPC_TRANSACTION, trans)

    call_retval = trans->timeout;
    call_status = TRUE;

    OCI_LIB_CALL_EXIT()
}
