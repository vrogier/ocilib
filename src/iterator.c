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

#include "iterator.h"

#include "element.h"
#include "macros.h"

/* --------------------------------------------------------------------------------------------- *
 * OcilibIteratorCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Iterator * OcilibIteratorCreate
(
    OCI_Collection *coll
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Iterator*, NULL,
        /* context */ OCI_IPC_COLLECTION, coll
    );

    OCI_Iterator* iter = NULL;

    CHECK_PTR(OCI_IPC_COLLECTION, coll);

    /* allocate iterator structure */

    ALLOC_HANDLE(OCI_IPC_ITERATOR, iter);
    iter->coll  = coll;
    iter->eoc   = FALSE;
    iter->boc   = TRUE;
    iter->dirty = TRUE;

    /* create iterator */

    CHECK_OCI
    (
        iter->coll->con->err,
        OCIIterCreate,
        iter->coll->con->env->handle,
        iter->coll->con->err,
        iter->coll->handle,
        &iter->handle
    );

    /* create data element */

    iter->elem = OcilibElementInitialize(iter->elem, NULL, (OCIInd*)NULL, coll->typinf);
    CHECK_NULL(iter->elem);

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE)
        {
            OcilibIteratorFree(iter);
            iter = NULL;
        }

        SET_RETVAL(iter);
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibIteratorFree
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibIteratorFree
(
    OCI_Iterator *iter
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_ITERATOR, iter
    );

    CHECK_PTR(OCI_IPC_ITERATOR, iter);

    /* close iterator handle */

    if (NULL != iter->handle)
    {
        CHECK_OCI
        (
            iter->coll->con->err,
            OCIIterDelete,
            iter->coll->con->env->handle,
            iter->coll->con->err,
            &iter->handle
        );
    }

    /* free data element */

    if (NULL != iter->elem)
    {
        iter->elem->hstate = OCI_OBJECT_FETCHED_DIRTY;
        OcilibElementFree(iter->elem);
        iter->elem = NULL;
    }

    /* free iterator structure */

    OcilibErrorResetSource(NULL, iter);

    FREE(iter);

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibIteratorGetNext
 * --------------------------------------------------------------------------------------------- */

OCI_Element * OcilibIteratorGetNext
(
    OCI_Iterator *iter
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Element*, NULL,
        /* context */ OCI_IPC_ITERATOR, iter
    );

    void   *data  = NULL;
    OCIInd *p_ind = NULL;

    CHECK_PTR(OCI_IPC_ITERATOR, iter);

    CHECK(!iter->eoc);

    CHECK_OCI
    (
        iter->coll->con->err,
        OCIIterNext,
        iter->coll->con->env->handle,
        iter->coll->con->err,
        iter->handle, &data,
        (dvoid **) &p_ind,
        (boolean*) &iter->eoc
    );

    CHECK(!iter->eoc);

    iter->elem = OcilibElementInitialize(iter->elem,
                                         data, p_ind, iter->coll->typinf);
    iter->dirty = FALSE;
    iter->boc   = FALSE;

    SET_RETVAL(iter->elem);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibIteratorGetPrev
 * --------------------------------------------------------------------------------------------- */

OCI_Element * OcilibIteratorGetPrev
(
    OCI_Iterator *iter
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Element*, NULL,
        /* context */ OCI_IPC_ITERATOR, iter
    );

    void   *data  = NULL;
    OCIInd *p_ind = NULL;

    CHECK_PTR(OCI_IPC_ITERATOR, iter);

    CHECK(!iter->boc);

    CHECK_OCI
    (
        iter->coll->con->err,
        OCIIterPrev,
        iter->coll->con->env->handle,
        iter->coll->con->err,
        iter->handle, &data,
        (dvoid **) &p_ind,
        (boolean*) &iter->boc
    );

    CHECK(!iter->boc);

    iter->elem  = OcilibElementInitialize(iter->elem, data, p_ind, iter->coll->typinf);
    iter->dirty = FALSE;
    iter->eoc   = FALSE;

    SET_RETVAL(iter->elem);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibIteratorGetCurrent
 * --------------------------------------------------------------------------------------------- */

OCI_Element * OcilibIteratorGetCurrent
(
    OCI_Iterator *iter
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Element*, NULL,
        /* context */ OCI_IPC_ITERATOR, iter
    );

    CHECK_PTR(OCI_IPC_ITERATOR, iter);

    CHECK(NULL != iter->elem && !iter->boc && !iter->eoc && !iter->dirty);

    SET_RETVAL(iter->elem);

    EXIT_FUNC();
}
