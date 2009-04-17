/*
   +----------------------------------------------------------------------+   
   |                                                                      |
   |                     OCILIB - C Driver for Oracle                     |
   |                                                                      |
   |                      (C Wrapper for Oracle OCI)                      |
   |                                                                      |
   +----------------------------------------------------------------------+
   |                      Website : http://ocilib.net                     |
   +----------------------------------------------------------------------+
   |               Copyright (c) 2007-2009 Vincent ROGIER                 |
   +----------------------------------------------------------------------+
   | This library is free software; you can redistribute it and/or        |
   | modify it under the terms of the GNU Library General Public          |
   | License as published by the Free Software Foundation; either         |
   | version 2 of the License, or (at your option) any later version.     |
   |                                                                      |
   | This library is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU    |
   | Library General Public License for more details.                     |
   |                                                                      |
   | You should have received a copy of the GNU Library General Public    |
   | License along with this library; if not, write to the Free           |
   | Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.   |
   +----------------------------------------------------------------------+
   |          Author: Vincent ROGIER <vince.rogier@gmail.com>             |
   +----------------------------------------------------------------------+ 
*/

/* ------------------------------------------------------------------------ *
 * $Id: iterator.c, v 3.2.0 2009/04/20 00:00 Vince $
 * ------------------------------------------------------------------------ */

#include "ocilib_internal.h"

/* ************************************************************************ *
 *                            PUBLIC FUNCTIONS
 * ************************************************************************ */

/* ------------------------------------------------------------------------ *
 * OCI_IterCreate
 * ------------------------------------------------------------------------ */

OCI_Iter * OCI_API OCI_IterCreate(OCI_Coll *coll)
{
    boolean res    = TRUE;
    OCI_Iter *iter = NULL;

    OCI_CHECK_INITIALIZED(NULL);

    OCI_CHECK_PTR(OCI_IPC_COLLECTION, coll, NULL);

    /* allocate iterator structure */

    iter = (OCI_Iter *) OCI_MemAlloc(OCI_IPC_ITERATOR, sizeof(*iter), 1, TRUE);

    if (iter != NULL)
    {
        iter->coll      = coll;
        iter->eoc       = FALSE;
        iter->boc       = TRUE;

        /* create iterator */

        OCI_CALL2
        (
            res, iter->coll->con,  
            
            OCIIterCreate(OCILib.env, iter->coll->con->err, coll->handle, 
                          &iter->handle)
        )

        /* create data element accessor */

       if (res == TRUE)
           iter->elem = OCI_ElemInit(coll->con, &iter->elem, NULL, 
                                     (OCIInd *) NULL, coll->typinf);
    
       if (res == TRUE)
           res = (iter->elem != NULL);
    }
    else
        res = FALSE;

    /* check for success */

    if (res == FALSE)
    {
        OCI_IterFree(iter);
        iter = NULL;
    }

    OCI_RESULT(res);

    return iter;
}

/* ------------------------------------------------------------------------ *
 * OCI_IterFree
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_IterFree(OCI_Iter *iter)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_ITERATOR, iter, FALSE);
    
    /* close iterator handle */

    if (iter->handle != NULL)
    {
        OCI_CALL2
        (
            res, iter->coll->con,  

            OCIIterDelete(OCILib.env, iter->coll->con->err, &iter->handle)
        )
    }

    /* free data element accessor */
    
    if (iter->elem != NULL)
    {
        res = OCI_ElemFree(iter->elem);
        iter->elem = NULL;
    }
   
    /* free iterator structure */

    OCI_FREE(iter);

    OCI_RESULT(res);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_IterGetNext
 * ------------------------------------------------------------------------ */

OCI_Elem * OCI_API OCI_IterGetNext(OCI_Iter *iter)
{
    boolean res    = TRUE;
    OCI_Elem *elem = NULL;

    OCI_CHECK_PTR(OCI_IPC_ITERATOR, iter, FALSE);
    
    OCI_CHECK(iter->eoc == TRUE, FALSE);

    iter->elem->init = FALSE;

    OCI_CALL2
    (
        res, iter->coll->con,  

        OCIIterNext(OCILib.env, iter->coll->con->err, iter->handle,
                    &iter->elem->handle, (dvoid **) &iter->elem->ind,
                    &iter->eoc)
    )

    if ((res == TRUE) && (iter->eoc == FALSE))
        elem = iter->elem;
       
    OCI_RESULT(elem != NULL);

    return elem;
}

/* ------------------------------------------------------------------------ *
 * OCI_IterGetPrev
 * ------------------------------------------------------------------------ */

OCI_Elem * OCI_API OCI_IterGetPrev(OCI_Iter *iter)
{
    boolean res    = TRUE;
    OCI_Elem *elem = NULL;

    OCI_CHECK_PTR(OCI_IPC_ITERATOR, iter, FALSE);
    
    OCI_CHECK(iter->boc == TRUE, FALSE);

    iter->elem->init = FALSE;

    OCI_CALL2
    (
        res, iter->coll->con,  

        OCIIterPrev(OCILib.env, iter->coll->con->err, iter->handle, 
                    &iter->elem->handle, (dvoid **) &iter->elem->ind, 
                    &iter->boc)
    )

    if ((res == TRUE) && (iter->boc == FALSE))
        elem = iter->elem;

    OCI_RESULT(elem != NULL);

    return elem;

}
