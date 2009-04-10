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
 * $Id: callback.c, v 3.2.0 2009/04/20 00:00 Vince $
 * ------------------------------------------------------------------------ */

#include "ocilib_internal.h"

/* ************************************************************************ *
 *                             PRIVATE FUNCTIONS
 * ************************************************************************ */

/* ------------------------------------------------------------------------ *
 * OCI_ProcInBind
 * ------------------------------------------------------------------------ */

sb4 OCI_ProcInBind(dvoid *ictxp, OCIBind *bindp, ub4 iter, ub4 index,
                   dvoid **bufpp, ub4 *alenp, ub1 *piecep, dvoid **indp)
{
    OCI_Bind * bnd = (OCI_Bind *) ictxp;
    sb2 *ind       = (sb2 *) bnd->buf.inds;
    ub4 i          = 0;

    /* those checks may be not necessary but they keep away compilers warning
       away if the warning level is set to maximum !
    */

    OCI_NOT_USED(index);
    OCI_NOT_USED(bindp);

    /* check objects and bounds */

    OCI_CHECK(bnd  == NULL, OCI_ERROR);
    OCI_CHECK(iter >= bnd->buf.count, OCI_ERROR);

    /* indicators must be set to -1 depending on datatype,
       so let's do it for all */

    for (i = 0; i < bnd->buf.count; i++, ind++)
        *ind = -1;

    /* setup bind index because OCI_RegisterXXX() might not have been called
       in the same order than the variables in the returning clause */

    if (iter == 0)
        bnd->dynpos = bnd->stmt->dynidx++;

    /* we do not need to do anything here except setting indicators */

    *bufpp  = (dvoid *) 0;
    *alenp  = (ub4    ) 0;
    *indp   = (dvoid *) bnd->buf.inds;
    *piecep = (ub1    ) OCI_ONE_PIECE;

    return OCI_CONTINUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_GetDefine
 * ------------------------------------------------------------------------ */

sb4 OCI_ProcOutBind(dvoid *octxp, OCIBind *bindp, ub4 iter, ub4 index,
                    dvoid **bufpp, ub4 **alenp, ub1 *piecep, dvoid **indp,
                    ub2 **rcodep)
{
    OCI_Bind * bnd    = (OCI_Bind *) octxp;
    OCI_Define *def   = NULL;
    OCI_Resultset *rs = NULL;
    boolean res       = TRUE;
    ub4 rows          = 0;

    /* those checks may be not necessary but they keep away compilers warning
       away if the warning level is set to maximum !
    */

    OCI_NOT_USED(bindp);

    /* check objects and bounds */

    OCI_CHECK(bnd  == NULL, OCI_ERROR);
    OCI_CHECK(iter >= bnd->buf.count, OCI_ERROR);

    /* update statmement status */

    bnd->stmt->status = OCI_STMT_EXECUTED;

    /* create resultset on the first row processed for each iteration */

    if (index == 0)
    {
        bnd->stmt->nb_rs   = bnd->stmt->nb_iters;
        bnd->stmt->cur_rs  = 0;

        /* allocate resultset handles array */

        if (bnd->stmt->rsts == NULL)
        {
            bnd->stmt->rsts = (OCI_Resultset **) OCI_MemAlloc(OCI_IPC_RESULTSET_ARRAY, 
                                                              sizeof(*bnd->stmt->rsts),
                                                              bnd->stmt->nb_rs, TRUE);
 
            if (bnd->stmt->rsts == NULL)
                res = FALSE;
        }
        
        /* create resultset as needed */

        if (bnd->stmt->rsts[iter] == NULL)
        {
            OCI_CALL1
            (   
                res, bnd->stmt->con, bnd->stmt,
                
                OCIAttrGet(bnd->buf.handle, (ub4) OCI_HTYPE_BIND, (void *) &rows, 
                           (ub4 *) NULL, (ub4) OCI_ATTR_ROWS_RETURNED,
                           bnd->stmt->con->err)
            )

            if (res == TRUE)
            {
                bnd->stmt->rsts[iter] = OCI_ResultsetCreate(bnd->stmt, rows);

                if (bnd->stmt->rsts[iter] != NULL)
                    bnd->stmt->rsts[iter]->row_count = rows;
            }
        }
    }

    OCI_CHECK(bnd->stmt->rsts == NULL, OCI_ERROR);
    
    rs = bnd->stmt->rsts[iter];

    OCI_CHECK(rs == NULL, OCI_ERROR);
 
    /* ok.. let's Oracle update its buffers */

    if (res == TRUE)
    {
        /* update pointers contents */

        def = &rs->defs[bnd->dynpos];

        switch (def->col.type)
        {

            case OCI_CDT_CURSOR:
            case OCI_CDT_TIMESTAMP:
            case OCI_CDT_INTERVAL:
            case OCI_CDT_LOB:
            case OCI_CDT_FILE:

                *bufpp = def->buf.data[index];
                break;

            default:

                *bufpp = (((ub1*)def->buf.data) + (def->col.bufsize * index));
        }

        *alenp  = (ub4   *) (((ub1 *) def->buf.lens) + (def->buf.sizelen * index));
        *indp   = (dvoid *) (((ub1 *) def->buf.inds) + (sizeof(sb2) * index));
        *piecep = (ub1    ) OCI_ONE_PIECE;
        *rcodep = (ub2   *) NULL;                
    }

    return ((res == TRUE) ? OCI_CONTINUE : OCI_ERROR);
}
