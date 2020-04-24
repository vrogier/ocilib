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

#include "iterator.h"

#include "element.h"
#include "macros.h"

/* --------------------------------------------------------------------------------------------- *
 * IterCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Iter * IteratorCreate
(
OCI_Coll *coll
)
{
    OCI_Iter *iter = NULL;

    CALL_ENTER(OCI_Iter*, iter);
    CHECK_PTR(OCI_IPC_COLLECTION, coll)
    CTX_SET_FROM_CON(coll->con)

    /* allocate iterator structure */

    ALLOC_DATA(OCI_IPC_ITERATOR, iter, 1)

    if (STATUS)
    {
        iter->coll  = coll;
        iter->eoc   = FALSE;
        iter->boc   = TRUE;
        iter->dirty = TRUE;

        /* create iterator */

        EXEC(OCIIterCreate(iter->coll->con->env, iter->coll->con->err, coll->handle, &iter->handle))

        /* create data element */

        if (STATUS)
        {
            iter->elem = ElementInitialize(coll->con, iter->elem, NULL, (OCIInd *)NULL, coll->typinf);
            STATUS = (NULL != iter->elem);
        }
    }

    /* check for success */

    if (STATUS)
    {
        RETVAL = iter;
    }
    else if (iter)
    {
        IteratorFree(iter);
   }

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * IterFree
 * --------------------------------------------------------------------------------------------- */

boolean IteratorFree
(
    OCI_Iter *iter
)
{
    CALL_ENTER(boolean, FALSE)
    CHECK_PTR(OCI_IPC_ITERATOR, iter)
    CTX_SET_FROM_CON(iter->coll->con)

    /* close iterator handle */

    if (iter->handle)
    {
        EXEC(OCIIterDelete(iter->coll->con->env, iter->coll->con->err, &iter->handle))
    }

    /* free data element */

    if (iter->elem)
    {
        iter->elem->hstate = OCI_OBJECT_FETCHED_DIRTY;
        ElementFree(iter->elem);
        iter->elem = NULL;
    }

    /* free iterator structure */

    FREE(iter)

    RETVAL = STATUS;
    
    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * IterGetNext
 * --------------------------------------------------------------------------------------------- */

OCI_Elem * IteratorGetNext
(
    OCI_Iter *iter
)
{
    void     *data  = NULL;
    OCIInd   *p_ind = NULL;

    CALL_ENTER(OCI_Elem *, NULL)
    CHECK_PTR(OCI_IPC_ITERATOR, iter)
    CTX_SET_FROM_CON(iter->coll->con)

    if (!iter->eoc)
    {
        EXEC(OCIIterNext(iter->coll->con->env, iter->coll->con->err, iter->handle, &data, (dvoid **) &p_ind, &iter->eoc))

        if (STATUS && !iter->eoc)
        {
            RETVAL = iter->elem = ElementInitialize(iter->coll->con, iter->elem, data, p_ind, iter->coll->typinf);

            iter->dirty = FALSE;
            iter->boc   = FALSE;
        }
    }

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * IterGetPrev
 * --------------------------------------------------------------------------------------------- */

OCI_Elem * IteratorGetPrev
(
    OCI_Iter *iter
)
{
    void     *data  = NULL;
    OCIInd   *p_ind = NULL;

    CALL_ENTER(OCI_Elem *, NULL)
    CHECK_PTR(OCI_IPC_ITERATOR, iter)
    CTX_SET_FROM_CON(iter->coll->con)

    if (!iter->boc)
    {
        EXEC(OCIIterPrev(iter->coll->con->env, iter->coll->con->err, iter->handle, &data, (dvoid **) &p_ind, &iter->boc))

        if (STATUS && !iter->boc)
        {
            RETVAL = iter->elem = ElementInitialize(iter->coll->con, iter->elem, data, p_ind, iter->coll->typinf);

            iter->dirty = FALSE;
            iter->eoc   = FALSE;
        }
    }

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * IterGetCurrent
 * --------------------------------------------------------------------------------------------- */

OCI_Elem * IteratorGetCurrent
(
    OCI_Iter *iter
)
{
    CALL_ENTER(OCI_Elem*, NULL)
    CHECK_PTR(OCI_IPC_ITERATOR, iter)
    CTX_SET_FROM_CON(iter->coll->con)

    if (iter->elem && !iter->boc && !iter->eoc && !iter->dirty)
    {
        RETVAL = iter->elem;
    }

    CALL_EXIT()
}
