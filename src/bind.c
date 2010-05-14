/*
   +----------------------------------------------------------------------+   
   |                                                                      |
   |                     OCILIB - C Driver for Oracle                     |
   |                                                                      |
   |                      (C Wrapper for Oracle OCI)                      |
   |                                                                      |
   +----------------------------------------------------------------------+
   |                      Website : http://www.ocilib.net                 |
   +----------------------------------------------------------------------+
   |               Copyright (c) 2007-2010 Vincent ROGIER                 |
   +----------------------------------------------------------------------+
   | This library is free software; you can redistribute it and/or        |
   | modify it under the terms of the GNU Lesser General Public           |
   | License as published by the Free Software Foundation; either         |
   | version 2 of the License, or (at your option) any later version.     |
   |                                                                      |
   | This library is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU    |
   | Lesser General Public License for more details.                      |
   |                                                                      |
   | You should have received a copy of the GNU Lesser General Public     |
   | License along with this library; if not, write to the Free           |
   | Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.   |
   +----------------------------------------------------------------------+
   |          Author: Vincent ROGIER <vince.rogier@ocilib.net>            |
   +----------------------------------------------------------------------+ 
*/

/* ------------------------------------------------------------------------ *
 * $Id: bind.c, v 3.6.0 2010-05-14 20:21 Vincent Rogier $
 * ------------------------------------------------------------------------ */

#include "ocilib_internal.h"

/* ************************************************************************ *
 *                             PRIVATE FUNCTIONS
 * ************************************************************************ */

/* ------------------------------------------------------------------------ *
 * OCI_BindFree
 * ------------------------------------------------------------------------ */

boolean OCI_BindFree(OCI_Bind *bnd)
{
    if (bnd->alloc == TRUE)
        OCI_FREE(bnd->buf.data);

    OCI_FREE(bnd->buf.inds);
    OCI_FREE(bnd->buf.lens);
    OCI_FREE(bnd->buf.tmpbuf);

    OCI_FREE(bnd->plrcds);

    OCI_FREE(bnd->name);
    OCI_FREE(bnd);

    return TRUE;
}

/* ************************************************************************ *
 *                            PUBLIC FUNCTIONS
 * ************************************************************************ */

/* ------------------------------------------------------------------------ *
 * OCI_BindGetName
 * ------------------------------------------------------------------------ */

const mtext * OCI_API OCI_BindGetName(OCI_Bind *bnd)
{
    OCI_CHECK_PTR(OCI_IPC_BIND, bnd, NULL);

    OCI_RESULT(TRUE);

    return (const mtext *) bnd->name; 
}

/* ------------------------------------------------------------------------ *
 * OCI_BindGetType
 * ------------------------------------------------------------------------ */

unsigned int OCI_API OCI_BindGetType(OCI_Bind *bnd)
{
    OCI_CHECK_PTR(OCI_IPC_BIND, bnd, OCI_UNKNOWN);

    OCI_RESULT(TRUE);

    return (unsigned int) bnd->type; 
}

/* ------------------------------------------------------------------------ *
 * OCI_BindGetSubtype
 * ------------------------------------------------------------------------ */

unsigned int OCI_API OCI_BindGetSubtype(OCI_Bind *bnd)
{
    unsigned int type = OCI_UNKNOWN;

    OCI_CHECK_PTR(OCI_IPC_BIND, bnd, OCI_UNKNOWN);

    OCI_RESULT(TRUE);

    if (bnd->type == OCI_CDT_LONG      || 
        bnd->type == OCI_CDT_LOB       ||
        bnd->type == OCI_CDT_FILE      ||
        bnd->type == OCI_CDT_TIMESTAMP ||
        bnd->type == OCI_CDT_INTERVAL)
    {
        type = bnd->subtype;
    }

    return type;
}

/* ------------------------------------------------------------------------ *
 * OCI_BindGetDataCount
 * ------------------------------------------------------------------------ */

unsigned int OCI_API OCI_BindGetDataCount(OCI_Bind *bnd)
{
    OCI_CHECK_PTR(OCI_IPC_BIND, bnd, 0);

    OCI_RESULT(TRUE);

    return (unsigned int) bnd->buf.count; 
}

/* ------------------------------------------------------------------------ *
 * OCI_BindGetData
 * ------------------------------------------------------------------------ */

void * OCI_API OCI_BindGetData(OCI_Bind *bnd)
{
    OCI_CHECK_PTR(OCI_IPC_BIND, bnd, NULL);

    OCI_RESULT(TRUE);

    return (void *) bnd->input; 
}

/* ------------------------------------------------------------------------ *
 * OCI_BindGetStatement
 * ------------------------------------------------------------------------ */

OCI_EXPORT OCI_Statement * OCI_API OCI_BindGetStatement(OCI_Bind *bnd)
{
    OCI_CHECK_PTR(OCI_IPC_BIND, bnd, NULL);

    OCI_RESULT(TRUE);

    return bnd->stmt; 
}

/* ------------------------------------------------------------------------ *
 * OCI_BindSetDataSize
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_BindSetDataSize(OCI_Bind *bnd, unsigned int size)
{
    return OCI_BindSetDataSizeAtPos(bnd, 1, size);
}

/* ------------------------------------------------------------------------ *
 * OCI_BindSetDataSizeAtPos
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_BindSetDataSizeAtPos(OCI_Bind *bnd, unsigned int position, 
                                         unsigned int size)
{
    boolean res   = FALSE;

    OCI_CHECK_PTR(OCI_IPC_BIND, bnd, FALSE);
    OCI_CHECK_BOUND(bnd->stmt->con, position, 1, bnd->buf.count, FALSE);
    OCI_CHECK_MIN(bnd->stmt->con, bnd->stmt, size, 1, FALSE);

    if (bnd->buf.lens != NULL)
    {

        if (bnd->type == OCI_CDT_TEXT)
        {
            if (bnd->size == (sb4) size)
                size += (unsigned int) (size_t) sizeof(odtext);

            size *= (unsigned int) sizeof(odtext);
        }

        ((ub2 *) bnd->buf.lens)[position-1] = (ub2) size; 

        res = TRUE;
    }

    OCI_RESULT(TRUE);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_BindGetDataSize
 * ------------------------------------------------------------------------ */

unsigned int OCI_API OCI_BindGetDataSize(OCI_Bind *bnd)
{
    return OCI_BindGetDataSizeAtPos(bnd, 1);
}

/* ------------------------------------------------------------------------ *
 * OCI_BindGetDataSizeAtPos
 * ------------------------------------------------------------------------ */

unsigned int OCI_API OCI_BindGetDataSizeAtPos(OCI_Bind *bnd, unsigned int position)
{
    unsigned int size = 0;

    OCI_CHECK_PTR(OCI_IPC_BIND, bnd, 0);
    OCI_CHECK_BOUND(bnd->stmt->con, position, 1, bnd->buf.count, 0);
 
    if (bnd->buf.lens != NULL)
    {
        size = (unsigned int) ((ub2 *) bnd->buf.lens)[position-1];

        if (bnd->type == OCI_CDT_TEXT)
        {
            if (bnd->size == (sb4) size)
                size -= (unsigned int) sizeof(odtext);

            size /= (unsigned int) sizeof(odtext);
        }
    }

    OCI_RESULT(TRUE);

    return size;
}


/* ------------------------------------------------------------------------ *
 * OCI_BindSetNullAtPos
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_BindSetNullAtPos(OCI_Bind *bnd, unsigned int position)
{
    OCI_CHECK_PTR(OCI_IPC_BIND, bnd, FALSE);
    OCI_CHECK_BOUND(bnd->stmt->con, position, 1, bnd->buf.count, FALSE);

    if (bnd->buf.inds != NULL)
        ((sb2*) bnd->buf.inds)[position-1] = -1;

    OCI_RESULT(TRUE);

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_BindSetNull
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_BindSetNull(OCI_Bind *bnd)
{
  return OCI_BindSetNullAtPos(bnd, 1);
}

/* ------------------------------------------------------------------------ *
 * OCI_BindIsNullAtPos
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_BindIsNullAtPos(OCI_Bind *bnd, unsigned int position)
{
    boolean ret = TRUE;

    OCI_CHECK_PTR(OCI_IPC_BIND, bnd, FALSE);
    OCI_CHECK_BOUND(bnd->stmt->con, position, 1, bnd->buf.count, FALSE);

    if (bnd->buf.inds != NULL)
        ret = (((sb2*) bnd->buf.inds)[position-1] == -1);

    OCI_RESULT(TRUE);

    return ret;
}

/* ------------------------------------------------------------------------ *
 * OCI_BindIsNull
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_BindIsNull(OCI_Bind *bnd)
{
  return OCI_BindIsNullAtPos(bnd, 1);
}


/* ------------------------------------------------------------------------ *
 * OCI_BindSetCharsetForm
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_BindSetCharsetForm(OCI_Bind *bnd, unsigned int csfrm)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_BIND, bnd, FALSE);

    if ((bnd->type == OCI_CDT_TEXT) || (bnd->type == OCI_CDT_LONG))
    {
        if (csfrm == OCI_CSF_NATIONAL)
            bnd->csfrm = SQLCS_NCHAR;
        else if (csfrm == OCI_CSF_CHARSET)
            bnd->csfrm = SQLCS_IMPLICIT;

        OCI_CALL1
        (
            res, bnd->stmt->con, bnd->stmt,

            OCIAttrSet((dvoid *) bnd->buf.handle,
                       (ub4    ) OCI_HTYPE_BIND,
                       (dvoid *) &bnd->csfrm,
                       (ub4    ) sizeof(bnd->csfrm),
                       (ub4    ) OCI_ATTR_CHARSET_FORM,
                       bnd->stmt->con->err)
        )
    }
    
    OCI_RESULT(res);
    
    return res;
}
