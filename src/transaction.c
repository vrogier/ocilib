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

#include "transaction.h"

#include "connection.h"
#include "list.h"
#include "macros.h"

/* --------------------------------------------------------------------------------------------- *
* OcilibTransactionDispose
* --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibTransactionDispose
(
    OCI_Transaction * trans
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_TRANSACTION, trans
    );

    CHECK_PTR(OCI_IPC_TRANSACTION, trans);

    if (trans->started)
    {
        /* best effort: a global branch that was prepared or resolved
           cannot be detached again, but the handle must still be freed */

        OcilibTransactionStop(trans);
    }

    /* close transaction handle */

    if (trans->handle)
    {
        OcilibMemoryFreeHandle(trans->con->env, (dvoid*)trans->handle, OCI_HTYPE_TRANS);
        trans->handle = NULL;
    }

    if (trans->con->trs == trans)
    {
        trans->con->trs = NULL;
    }

    OcilibErrorResetSource(NULL, trans);

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibTransactionCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Transaction * OcilibTransactionCreate
(
    OCI_Connection     *con,
    OCI_UInt32        timeout,
    OCI_TransactionMode mode,
    OCI_XID            *pxid
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Transaction*, NULL,
        /* context */ OCI_IPC_CONNECTION, con
    );

    OCI_Transaction* trans = NULL;

    CHECK_PTR(OCI_IPC_CONNECTION, con);

    /* create transaction object */

    LIST_ATOMIC_ADD(con->trsns, trans);

    CHECK_NULL(trans);

    trans->env     = con->env;
    trans->htype   = OCI_IPC_TRANSACTION;
    trans->con     = con;
    trans->mode    = mode;
    trans->timeout = timeout;
    trans->local   = (NULL == pxid);
    trans->started = FALSE;

    /* allocate transaction handle */

    CHECK(OcilibMemoryAllocHandle(trans->con->env, trans->con->env->handle, (dvoid **)&trans->handle, OCI_HTYPE_TRANS));

    /* set XID attribute for global transaction */

    if (pxid)
    {
        memcpy(&trans->xid, pxid, sizeof(trans->xid));

        CHECK_ATTRIB_SET
        (
            OCI_HTYPE_TRANS, OCI_ATTR_XID,
            trans->handle, &trans->xid, sizeof(trans->xid),
            trans->con->err
        );
    }

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE)
        {
            OcilibTransactionFree(trans);
            trans = NULL;
        }

        SET_RETVAL(trans);
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibTransactionFree
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibTransactionFree
(
    OCI_Transaction * trans
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_TRANSACTION, trans
    );

    CHECK_PTR(OCI_IPC_TRANSACTION, trans);

    /* remove transaction from internal list */

    LIST_ATOMIC_REMOVE(trans->con->trsns, trans, OcilibTransactionDispose);

    FREE(trans);

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibTransactionStart
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibTransactionStart
(
    OCI_Transaction * trans
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_TRANSACTION, trans
    );

    CHECK_PTR(OCI_IPC_TRANSACTION, trans);

    /* attach the transaction handle to the service context so that
       OCITransStart() uses its XID for global transactions */

    if (!trans->local)
    {
        CHECK_ATTRIB_SET
        (
            OCI_HTYPE_SVCCTX, OCI_ATTR_TRANS,
            trans->con->handle, trans->handle, 0,
            trans->con->err
        );
    }

    CHECK_OCI
    (
        trans->con->err,
        OCITransStart,
        trans->con->handle, trans->con->err,
        (uword) trans->timeout,  (ub4) trans->mode
    );

    trans->started = TRUE;

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibTransactionStop
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibTransactionStop
(
    OCI_Transaction * trans
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_TRANSACTION, trans
    );

    CHECK_PTR(OCI_IPC_TRANSACTION, trans);

    if (trans->local)
    {
        /* commit or rollback upon auto commit mode */

        if (trans->con->autocom)
        {
            OcilibConnectionCommit(trans->con);
        }
        else
        {
            OcilibConnectionRollback(trans->con);
        }
    }
    else
    {
        /* a global transaction branch is suspended, not ended - the
           transaction manager decides its outcome. Committing or rolling
           back here would end and detach the branch, making the subsequent
           OCITransDetach() fail with ORA-25352 */

        CHECK_OCI
        (
            trans->con->err,
            OCITransDetach,
            trans->con->handle, trans->con->err,
            (ub4) OCI_DEFAULT
        );
    }

    trans->started = FALSE;

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibTransactionResume
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibTransactionResume
(
    OCI_Transaction * trans
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_TRANSACTION, trans
    );

    CHECK_PTR(OCI_IPC_TRANSACTION, trans);

    /* attach the transaction handle to the service context so that
       OCITransStart() resumes the branch identified by its XID */

    if (!trans->local)
    {
        CHECK_ATTRIB_SET
        (
            OCI_HTYPE_SVCCTX, OCI_ATTR_TRANS,
            trans->con->handle, trans->handle, 0,
            trans->con->err
        );
    }

    CHECK_OCI
    (
        trans->con->err,
        OCITransStart,
        trans->con->handle, trans->con->err,
        (uword) trans->timeout, (ub4) OCI_TRANS_RESUME
    );

    trans->started = TRUE;

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibTransactionPrepare
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibTransactionPrepare
(
    OCI_Transaction * trans
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_TRANSACTION, trans
    );

    CHECK_PTR(OCI_IPC_TRANSACTION, trans);

    CHECK_OCI
    (
        trans->con->err,
        OCITransPrepare,
        trans->con->handle, trans->con->err,
        (ub4) OCI_DEFAULT
    );

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibTransactionForget
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibTransactionForget
(
    OCI_Transaction * trans
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_TRANSACTION, trans
    );

    CHECK_PTR(OCI_IPC_TRANSACTION, trans);

    CHECK_OCI
    (
        trans->con->err,
        OCITransForget,
        trans->con->handle, trans->con->err,
        (ub4) OCI_DEFAULT
    );

    trans->started = FALSE;

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibTransactionGetMode
 * --------------------------------------------------------------------------------------------- */

OCI_TransactionMode OcilibTransactionGetMode
(
    OCI_Transaction * trans
)
{
    GET_PROP
    (
        OCI_TransactionMode, OCI_UNKNOWN,
        OCI_IPC_TRANSACTION, trans,
        mode
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibTransactionGetTimeout
 * --------------------------------------------------------------------------------------------- */

OCI_UInt32 OcilibTransactionGetTimeout
(
    OCI_Transaction * trans
)
{
    GET_PROP
    (
        OCI_UInt32, 0,
        OCI_IPC_TRANSACTION, trans,
        timeout
    );
}
