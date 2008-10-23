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
   |               Copyright (c) 2007-2008 Vincent ROGIER                 |
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
 * $Id: bind.c, v 3.1.0 2008/10/23 21:00 Vince $
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
    OCI_FREE(bnd->buf.temp);

    OCI_FREE(bnd->plsizes);
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
 * OCI_BindGetElemCount
 * ------------------------------------------------------------------------ */

unsigned int OCI_API OCI_BindGetCount(OCI_Bind *bnd)
{
    OCI_CHECK_PTR(OCI_IPC_BIND, bnd, 0);

    OCI_RESULT(TRUE);

    return (unsigned int) bnd->nbelem; 
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

