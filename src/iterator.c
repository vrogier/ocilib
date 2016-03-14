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
 *                            PUBLIC FUNCTIONS
 * ********************************************************************************************* */

/* --------------------------------------------------------------------------------------------- *
 * OCI_IterCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Iter * OCI_API OCI_IterCreate
(
OCI_Coll *coll
)
{

    OCI_Iter *iter = NULL;

    OCI_LIB_CALL_ENTER(OCI_Iter*, iter);

    OCI_CHECK_PTR(OCI_IPC_COLLECTION, coll)

    /* allocate iterator structure */

    iter = (OCI_Iter *)OCI_MemAlloc(OCI_IPC_ITERATOR, sizeof(*iter), (size_t)1, TRUE);

    if (iter)
    {
        call_status = TRUE;

        iter->coll  = coll;
        iter->eoc   = FALSE;
        iter->boc   = TRUE;
        iter->dirty = TRUE;

        /* create iterator */

        OCI_CALL2
        (
            call_status, iter->coll->con,

            OCIIterCreate(iter->coll->con->env, iter->coll->con->err, coll->handle, &iter->handle)
        )

        /* create data element */

        if (call_status)
        {
            iter->elem = OCI_ElemInit(coll->con, &iter->elem, NULL, (OCIInd *)NULL, coll->typinf);

            call_status = (NULL != iter->elem);
        }
    }

    /* check for success */

    if (call_status)
    {
        call_retval = iter;
    }
    else if (iter)
    {
        OCI_IterFree(iter);
   }

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_IterFree
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_IterFree
(
    OCI_Iter *iter
)
{
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_ITERATOR, iter)

    /* close iterator handle */

    call_status = TRUE;

    if (iter->handle)
    {
        OCI_CALL2
        (
            call_status, iter->coll->con,

            OCIIterDelete(iter->coll->con->env, iter->coll->con->err, &iter->handle)
        )
    }

    /* free data element */

    if (iter->elem)
    {
        iter->elem->hstate = OCI_OBJECT_FETCHED_DIRTY;
        OCI_ElemFree(iter->elem);
        iter->elem = NULL;
    }

    /* free iterator structure */

    OCI_FREE(iter)

    call_retval = call_status = TRUE;
    
    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_IterGetNext
 * --------------------------------------------------------------------------------------------- */

OCI_Elem * OCI_API OCI_IterGetNext
(
    OCI_Iter *iter
)
{
    void     *data  = NULL;
    OCIInd   *p_ind = NULL;

    OCI_LIB_CALL_ENTER(OCI_Elem *, NULL)

    OCI_CHECK_PTR(OCI_IPC_ITERATOR, iter)

    call_status = TRUE;

    if (!iter->eoc)
    {
        OCI_CALL2
        (
            call_status, iter->coll->con,

            OCIIterNext(iter->coll->con->env, iter->coll->con->err, iter->handle,
                        &data, (dvoid **) &p_ind, &iter->eoc)
        )

        if (call_status && !iter->eoc)
        {
            call_retval = OCI_ElemInit(iter->coll->con, &iter->elem, data, p_ind, iter->coll->typinf);

            iter->dirty = FALSE;
            iter->boc   = FALSE;
        }
    }

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_IterGetPrev
 * --------------------------------------------------------------------------------------------- */

OCI_Elem * OCI_API OCI_IterGetPrev
(
    OCI_Iter *iter
)
{
    void     *data  = NULL;
    OCIInd   *p_ind = NULL;

    OCI_LIB_CALL_ENTER(OCI_Elem *, NULL)

    OCI_CHECK_PTR(OCI_IPC_ITERATOR, iter)

    call_status = TRUE;

    if (!iter->boc)
    {
        OCI_CALL2
        (
            call_status, iter->coll->con,

            OCIIterPrev(iter->coll->con->env, iter->coll->con->err, iter->handle,
                        &data, (dvoid **) &p_ind, &iter->boc)
        )

        if (call_status && !iter->boc)
        {
            call_retval = OCI_ElemInit(iter->coll->con, &iter->elem, data, p_ind, iter->coll->typinf);

            iter->dirty = FALSE;
            iter->eoc   = FALSE;
        }
    }

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_IterGetCurrent
 * --------------------------------------------------------------------------------------------- */

OCI_Elem * OCI_API OCI_IterGetCurrent
(
    OCI_Iter *iter
)
{
    OCI_LIB_CALL_ENTER(OCI_Elem*, NULL)

    OCI_CHECK_PTR(OCI_IPC_ITERATOR, iter)

    call_status = TRUE;

    if (iter->elem && !iter->boc && !iter->eoc && !iter->dirty)
    {
        call_retval = iter->elem;
    }

    OCI_LIB_CALL_EXIT()
}
