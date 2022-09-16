/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2022 Vincent ROGIER <vince.rogier@ocilib.net>
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
* OcilibTransactionDispose
* --------------------------------------------------------------------------------------------- */

boolean OcilibTransactionDispose
(
    OCI_Transaction * trans
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_TRANSACTION, trans
    )

    CHECK_PTR(OCI_IPC_TRANSACTION, trans)

    CHECK(OcilibTransactionStop(trans))

    /* close transaction handle */

    if (trans->htr)
    {
        OcilibMemoryFreeHandle((dvoid*)trans->htr, OCI_HTYPE_TRANS);
    }

    if (trans->con->trs == trans)
    {
        trans->con->trs = NULL;
    }

    OcilibErrorResetSource(NULL, trans);

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibTransactionCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Transaction * OcilibTransactionCreate
(
    OCI_Connection *con,
    unsigned int    timeout,
    unsigned int    mode,
    OCI_XID        *pxid
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Transaction*, NULL,
        /* context */ OCI_IPC_CONNECTION, con
    )

    OCI_Transaction* trans = NULL;

    CHECK_PTR(OCI_IPC_CONNECTION, con)

    /* create transaction object */

    LIST_ATOMIC_ADD(con->trsns, trans)

    CHECK_NULL(trans)

    trans->con     = con;
    trans->mode    = mode;
    trans->timeout = timeout;
    trans->local   = (NULL == pxid);

    /* allocate transaction handle */

    CHECK(OcilibMemoryAllocHandle((dvoid *)trans->con->env, (dvoid **)&trans->htr, OCI_HTYPE_TRANS))

    /* set XID attribute for global transaction */

    if (pxid)
    {
        memcpy(&trans->xid, pxid, sizeof(trans->xid));

        CHECK_ATTRIB_SET
        (
            OCI_HTYPE_TRANS, OCI_ATTR_XID,
            trans->htr, &trans->xid, sizeof(trans->xid),
            trans->con->err
        )
    }

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE)
        {
            OcilibTransactionFree(trans);
            trans = NULL;
        }

        SET_RETVAL(trans)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibTransactionFree
 * --------------------------------------------------------------------------------------------- */

boolean OcilibTransactionFree
(
    OCI_Transaction * trans
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_TRANSACTION, trans
    )

    CHECK_PTR(OCI_IPC_TRANSACTION, trans)

    /* remove transaction from internal list */

    LIST_ATOMIC_REMOVE(trans->con->trsns, trans, OcilibTransactionDispose)

    FREE(trans)

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibTransactionStart
 * --------------------------------------------------------------------------------------------- */

boolean OcilibTransactionStart
(
    OCI_Transaction * trans
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_TRANSACTION, trans
    )

    CHECK_PTR(OCI_IPC_TRANSACTION, trans)

    CHECK_OCI
    (
        trans->con->err,
        OCITransStart,
        trans->con->cxt, trans->con->err,
        (uword) trans->timeout,  (ub4) trans->mode
    )

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibTransactionStop
 * --------------------------------------------------------------------------------------------- */

boolean OcilibTransactionStop
(
    OCI_Transaction * trans
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_TRANSACTION, trans
    )

    CHECK_PTR(OCI_IPC_TRANSACTION, trans)

    /* commit or rollback upon auto commit mode */

    if (trans->con->autocom)
    {
        OcilibConnectionCommit(trans->con);
    }
    else
    {
        OcilibConnectionRollback(trans->con);
    }

    /* detach global transaction */

    if (!trans->local)
    {
        CHECK_OCI
        (
            trans->con->err,
            OCITransDetach,
            trans->con->cxt, trans->con->err,
            (ub4) OCI_DEFAULT
        )
    }

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibTransactionResume
 * --------------------------------------------------------------------------------------------- */

boolean OcilibTransactionResume
(
    OCI_Transaction * trans
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_TRANSACTION, trans
    )

    CHECK_PTR(OCI_IPC_TRANSACTION, trans)

    CHECK_OCI
    (
        trans->con->err,
        OCITransStart,
        trans->con->cxt, trans->con->err,
        (uword) trans->timeout, (ub4) OCI_TRANS_RESUME
    )

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibTransactionPrepare
 * --------------------------------------------------------------------------------------------- */

boolean OcilibTransactionPrepare
(
    OCI_Transaction * trans
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_TRANSACTION, trans
    )

    CHECK_PTR(OCI_IPC_TRANSACTION, trans)

    CHECK_OCI
    (
        trans->con->err,
        OCITransPrepare,
        trans->con->cxt, trans->con->err,
        (ub4) OCI_DEFAULT
    )

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibTransactionForget
 * --------------------------------------------------------------------------------------------- */

boolean OcilibTransactionForget
(
    OCI_Transaction * trans
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_TRANSACTION, trans
    )

    CHECK_PTR(OCI_IPC_TRANSACTION, trans)

    CHECK_OCI
    (
        trans->con->err,
        OCITransForget,
        trans->con->cxt, trans->con->err,
        (ub4) OCI_DEFAULT
    )

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibTransactionGetMode
 * --------------------------------------------------------------------------------------------- */

unsigned int OcilibTransactionGetMode
(
    OCI_Transaction * trans
)
{
    GET_PROP
    (
        unsigned int, OCI_UNKNOWN,
        OCI_IPC_TRANSACTION, trans,
        mode
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibTransactionGetTimeout
 * --------------------------------------------------------------------------------------------- */

unsigned int OcilibTransactionGetTimeout
(
    OCI_Transaction * trans
)
{
    GET_PROP
    (
        unsigned int, 0,
        OCI_IPC_TRANSACTION, trans,
        timeout
    )
}
