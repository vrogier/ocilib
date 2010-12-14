/*
    +-----------------------------------------------------------------------------------------+
    |                                                                                         |
    |                               OCILIB - C Driver for Oracle                              |
    |                                                                                         |
    |                                (C Wrapper for Oracle OCI)                               |
    |                                                                                         |
    |                              Website : http://www.ocilib.net                            |
    |                                                                                         |
    |             Copyright (c) 2007-2010 Vincent ROGIER <vince.rogier@ocilib.net>            |
    |                                                                                         |
    +-----------------------------------------------------------------------------------------+
    |                                                                                         |
    |             This library is free software; you can redistribute it and/or               |
    |             modify it under the terms of the GNU Lesser General Public                  |
    |             License as published by the Free Software Foundation; either                |
    |             version 2 of the License, or (at your option) any later version.            |
    |                                                                                         |
    |             This library is distributed in the hope that it will be useful,             |
    |             but WITHOUT ANY WARRANTY; without even the implied warranty of              |
    |             MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU           |
    |             Lesser General Public License for more details.                             |
    |                                                                                         |
    |             You should have received a copy of the GNU Lesser General Public            |
    |             License along with this library; if not, write to the Free                  |
    |             Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.          |
    |                                                                                         |
    +-----------------------------------------------------------------------------------------+
*/

/* --------------------------------------------------------------------------------------------- *
 * $Id: long.c, v 3.8.1 2010-12-13 00:00 Vincent Rogier $
 * --------------------------------------------------------------------------------------------- */

#include "ocilib_internal.h"

/* ********************************************************************************************* *
 *                             PRIVATE FUNCTIONS
 * ********************************************************************************************* */

/* --------------------------------------------------------------------------------------------- *
 * OCI_LongInit
 * --------------------------------------------------------------------------------------------- */

OCI_Long * OCI_LongInit
(
    OCI_Statement *stmt,
    OCI_Long     **plg,
    OCI_Define    *def,
    unsigned int   type
)
{
    boolean res  = TRUE;
    OCI_Long *lg = NULL;

    OCI_CHECK(plg == NULL, NULL);

    if (*plg == NULL)
        *plg = (OCI_Long *) OCI_MemAlloc(OCI_IPC_LONG, sizeof(*lg),
                                         (size_t) 1, TRUE);

    if (*plg != NULL)
    {
        lg = *plg;

        lg->size   = 0;
        lg->stmt   = stmt;
        lg->def    = def;
        lg->type   = type;
        lg->offset = 0;

        if (def != NULL)
            lg->hstate = OCI_OBJECT_FETCHED_CLEAN;
        else
            lg->hstate = OCI_OBJECT_ALLOCATED;
    }
    else
        res = FALSE;

    OCI_RESULT(res);

    return lg;
}

/* ********************************************************************************************* *
 *                            PUBLIC FUNCTIONS
 * ********************************************************************************************* */

/* --------------------------------------------------------------------------------------------- *
 * OCI_LongCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Long * OCI_API OCI_LongCreate
(
    OCI_Statement *stmt,
    unsigned int   type
)
{
    OCI_Long *lg = NULL;

    OCI_CHECK_INITIALIZED(NULL);

    OCI_CHECK_PTR(OCI_IPC_STATEMENT, stmt, NULL);

    lg = OCI_LongInit(stmt, &lg, NULL, type);

    OCI_RESULT(lg != NULL);

    return lg;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_LongFree
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_LongFree
(
    OCI_Long *lg
)
{
    OCI_CHECK_PTR(OCI_IPC_LONG, lg, FALSE);

    OCI_CHECK_OBJECT_FETCHED(lg, FALSE);

    OCI_FREE(lg->buffer);
    OCI_FREE(lg);

    OCI_RESULT(TRUE);

    return TRUE;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_LongGetType
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_LongGetType
(
    OCI_Long *lg
)
{
    OCI_CHECK_PTR(OCI_IPC_LONG, lg, OCI_UNKNOWN);

    OCI_RESULT(TRUE);

    return lg->type;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_LongRead
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_LongRead
(
    OCI_Long    *lg,
    void        *buffer,
    unsigned int len
)
{
    unsigned int size = len;
    unsigned int fact = 1;

    OCI_CHECK_PTR(OCI_IPC_LONG, lg, 0);
    OCI_CHECK_PTR(OCI_IPC_VOID, buffer, 0);

    OCI_CHECK_MIN(lg->stmt->con, lg->stmt, size, 1, 0);

    OCI_CHECK(lg->offset >= lg->size, 0);

    /* lg->size and lg offset are still expressed in odtext units even
       if the buffer had already been expanded to dtext *
    */

    if (lg->type == OCI_CLONG)
        len *= (unsigned int) sizeof(odtext);

    /* check buffer size to read */

    if ((size + lg->offset) > lg->size)
    {
        size = lg->size - lg->offset;
    }

    /* copy buffer */

    memcpy(buffer, lg->buffer + (size_t) lg->offset*fact, (size_t) (size*fact));

    lg->offset += size;

    if (lg->type == OCI_CLONG)
    {
        ((dtext *) buffer)[size] = 0;
        size                    /= (unsigned int) sizeof(dtext);
    }

    OCI_RESULT(TRUE);

    return size;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_LongWrite
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_LongWrite
(
    OCI_Long    *lg,
    void        *buffer,
    unsigned int len
)
{
    boolean res  = TRUE;
    sword code   = OCI_SUCCESS;
    void *obuf   = NULL;
    void *handle = NULL;
    ub1 in_out   = OCI_PARAM_IN;
    ub1 piece    = OCI_ONE_PIECE;
    ub4 type     = 0;
    ub4 iter     = 0;
    ub4 dx       = 0;
    ub4 count    = 0;

    OCI_CHECK_PTR(OCI_IPC_VOID, buffer, 0);
    OCI_CHECK_PTR(OCI_IPC_LONG, lg, 0);

    OCI_CHECK_MIN(lg->stmt->con, lg->stmt, len, 1, 0);

    if (lg->type == OCI_CLONG)
        len *= (unsigned int) sizeof(dtext);

    if (lg->type == OCI_CLONG)
        obuf = OCI_GetInputDataString(buffer, (int *) &len);
    else
        obuf = buffer;

    /* get piece info */

    OCI_CALL1
    (
        res, lg->stmt->con, lg->stmt,

        OCIStmtGetPieceInfo(lg->stmt->stmt, lg->stmt->con->err, &handle,
                            &type, &in_out, &iter, &dx, &piece)
    )

    /* set up piece type */

    if (len > 0)
        piece = (ub1) ((lg->size > 0) ? OCI_NEXT_PIECE : OCI_FIRST_PIECE);
    else
        piece = (ub1) OCI_LAST_PIECE;

    /* correct size to read for last piece */

    if ((lg->size + len) >= lg->stmt->long_size)
    {
        piece = OCI_LAST_PIECE;
        count = lg->stmt->long_size - lg->size;
    }
    else
    {
        count = len;
    }

    /* set up info for writing */

    OCI_CALL1
    (
        res, lg->stmt->con, lg->stmt,

        OCIStmtSetPieceInfo(handle, type, lg->stmt->con->err, (dvoid *) obuf,
                            &count,  piece, (dvoid *) NULL, (ub2 *) NULL)
    )

    /* perform write call */

    if (res == TRUE)
    {
        code = OCIStmtExecute(lg->stmt->con->cxt, lg->stmt->stmt,
                              lg->stmt->con->err, (ub4) 1, (ub4) 0,
                              (OCISnapshot *) NULL, (OCISnapshot *) NULL,
                              (ub4) 0);
    }

    if ((code != OCI_SUCCESS) && (code != OCI_NEED_DATA))
    {
        if (code == OCI_SUCCESS_WITH_INFO)
        {
            OCI_ExceptionOCI(lg->stmt->con->err, lg->stmt->con, lg->stmt, TRUE);
        }
        else
        {
            OCI_ExceptionOCI(lg->stmt->con->err, lg->stmt->con, lg->stmt, FALSE);
            res = FALSE;
        }
    }

    if (lg->type == OCI_CLONG)
        OCI_ReleaseDataString(obuf);

    /* update size */

    if (res == TRUE)
    {
        lg->size += count;

        /* at this point, count is expressed in odtext bytes for character LONGs
         **/

        if (lg->type == OCI_CLONG)
            count /= (unsigned int) sizeof(odtext);

    }

    OCI_RESULT(res);

    return count;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_LongGetSize
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_LongGetSize
(
    OCI_Long *lg
)
{
    unsigned int size = 0;

    OCI_CHECK_PTR(OCI_IPC_LONG, lg, 0);

    size = lg->size;

    if (lg->type == OCI_CLONG)
        size /= (unsigned int) sizeof(odtext);

    OCI_RESULT(TRUE);

    return size;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_LongGetBuffer
 * --------------------------------------------------------------------------------------------- */

void * OCI_API OCI_LongGetBuffer
(
    OCI_Long *lg
)
{
    OCI_CHECK_PTR(OCI_IPC_LONG, lg, NULL);

    OCI_RESULT(TRUE);

    return (void *) lg->buffer;
}
