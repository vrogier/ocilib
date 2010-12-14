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
 * $Id: resultset.c, v 3.8.1 2010-12-13 00:00 Vincent Rogier $
 * --------------------------------------------------------------------------------------------- */

#include "ocilib_internal.h"

/* ********************************************************************************************* *
 *                             PRIVATE FUNCTIONS
 * ********************************************************************************************* */

/* --------------------------------------------------------------------------------------------- *
 * OCI_ResultsetCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Resultset * OCI_ResultsetCreate
(
    OCI_Statement *stmt,
    int            size
)
{
    OCI_Resultset* rs = NULL;
    boolean res       = TRUE;
    ub4 nb            = 0;
    ub4 i;

    /* allocate resultset structure */

    rs = (OCI_Resultset *) OCI_MemAlloc(OCI_IPC_RESULTSET, sizeof(*rs),
                                        (size_t) 1, TRUE);

    /* set attributes */

    if (rs != NULL)
    {
        rs->stmt         = stmt;
        rs->bof          = TRUE;
        rs->eof          = FALSE;
        rs->fetch_size   = size;
        rs->fetch_status = OCI_SUCCESS;
        rs->row_count    = 0;
        rs->row_cur      = 0;
        rs->row_abs      = 0;

        /* is the resultset created from a SQL select statement ? */

        if (stmt->type == OCI_CST_SELECT)
        {
            OCI_CALL1
            (
                res, stmt->con, stmt,

                OCIAttrGet((void *) stmt->stmt, (ub4) OCI_HTYPE_STMT,
                           (void *) &nb, (ub4 *) NULL,
                           (ub4) OCI_ATTR_PARAM_COUNT, stmt->con->err)
            )
        }
        else
        {
            nb = stmt->nb_rbinds;
        }

        /* allocate columns array */

        if (res == TRUE)
        {
            rs->defs = (OCI_Define *) OCI_MemAlloc(OCI_IPC_DEFINE,
                                                   sizeof(*rs->defs),
                                                   (size_t) nb, TRUE);

            res = (rs->defs != NULL);
        }

        /* describe select list */

        if ((res == TRUE) && (stmt->type == OCI_CST_SELECT))
        {
            for (i=0; i < nb; i++)
            {
                OCI_Define *def = &rs->defs[i];

                def->buf.count   = size;
                def->buf.sizelen = sizeof(ub2);

                def->rs = rs;

                rs->nb_defs++;

                /* get column description */

                if (res == TRUE)
                    res = OCI_ColumnDescribe(&def->col, rs->stmt->con,
                                             rs->stmt, rs->stmt->stmt,
                                             rs->nb_defs, OCI_DESC_RESULTSET);

                /* mapping to OCILIB internal types */

                if (res == TRUE)
                    res = OCI_ColumnMap(&def->col, rs->stmt);

                /* allocation of internal buffers for resultset content */

                if (res == TRUE)
                    res = OCI_DefineAlloc(def);

                /* register OCILIB result buffers to OCI */

                if (res == TRUE)
                    res = OCI_DefineDef(def);

                if (res == FALSE)
                    break;
            }
        }
        else
        {
            /* get info from register binds */

            for (i=0; i < stmt->nb_rbinds; i++)
            {
                OCI_Bind *bnd   = stmt->rbinds[i];
                OCI_Define *def = &rs->defs[bnd->dynpos];

                def->buf.count   = size;
                def->buf.sizelen = sizeof(ub4);

                def->rs = rs;

                rs->nb_defs++;

                /* columns info */

                def->col.ocode   = bnd->code;
                def->col.name    = mtsdup(bnd->name);
                def->col.size    = (ub2) bnd->size;
                def->col.type    = bnd->type;
                def->col.subtype = bnd->subtype;

                /* check national attribute for nclobs */

                if (bnd->type == OCI_CDT_LOB && bnd->subtype == OCI_NCLOB)
                    def->col.csfrm = SQLCS_NCHAR;

                /* adjust column size from bind attributes */

                if (def->col.type == OCI_CDT_TEXT)
                    def->col.size = (ub2) (def->col.size / ((ub2) sizeof(dtext)) - 1);

                /* for integer types, set the bufsize here in order to
                   retrieve later the integer type (short, integer, big_int)
                   depending on the integer size */

                if (def->col.type == OCI_CDT_NUMERIC)
                    def->col.bufsize = def->col.size;

                /* preset bufsize here to let OCI_ColumnMap() know we don't
                   want the column to mapped to SQLT_ODT */

                if (def->col.ocode == SQLT_DAT)
                    def->col.bufsize = def->col.size;

                /* map column : failure means unsupported type */

                if (res == TRUE)
                    res = OCI_ColumnMap(&def->col, rs->stmt);

                /* allocation of internal buffers for resultset content */

                if (res == TRUE)
                    res = OCI_DefineAlloc(def);

                if (res == FALSE)
                    break;
            }
        }
    }

    return rs;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_FetchPieces
 * --------------------------------------------------------------------------------------------- */

boolean OCI_FetchPieces
(
    OCI_Resultset *rs
)
{
    boolean res = TRUE;

    ub4 type, iter, dx;
    ub1 in_out, piece;
    void *handle;
    ub4 i, j;

    OCI_CHECK(rs == NULL, FALSE);

    /* reset long objects */

    for (i = 0; i < rs->nb_defs; i++)
    {
        OCI_Define *def = &rs->defs[i];

        if (def->col.type == OCI_CDT_LONG)
        {
            for (j = 0; j < def->buf.count; j++)
            {
                OCI_LongInit(rs->stmt, (OCI_Long **) &def->buf.data[j],
                             def, def->col.subtype);
            }
        }
    }

    /* dynamic fetch */

    while ((res == TRUE) && (rs->fetch_status == OCI_NEED_DATA))
    {
        piece  = OCI_NEXT_PIECE;
        iter   = 0;
        handle = NULL;

        /* get piece information */

        OCI_CALL1
        (
            res, rs->stmt->con, rs->stmt,

            OCIStmtGetPieceInfo(rs->stmt->stmt, rs->stmt->con->err,
                                &handle,  &type, &in_out, &iter, &dx, &piece)
        )

        /* search for the given column */

        for (i = 0; (res == TRUE) && (i < rs->nb_defs); i++)
        {
            OCI_Define *def = &(rs->defs[i]);

            if (def->col.type == OCI_CDT_LONG && def->buf.handle == handle)
            {
                /* get the long object for the given internal row */

                OCI_Long *lg = (OCI_Long *) def->buf.data[iter];

                unsigned int nb_alloc      = 0;
                unsigned int trailing_size = 0;
                unsigned int char_fact     = sizeof(dtext)/sizeof(odtext);

                /* setup up piece size */

                ub4 bufsize = rs->stmt->long_size;

                if (char_fact == 0)
                    char_fact = 1;

                if (lg->type == OCI_CLONG)
                    bufsize += (ub4) sizeof(odtext);

                nb_alloc = (lg->maxsize / bufsize);

                if (lg->type == OCI_CLONG)
                    trailing_size = sizeof(odtext) * nb_alloc;

                /* check buffer */

                if (lg->buffer == NULL)
                {
                    lg->maxsize = bufsize;

                    lg->buffer = (ub1 *) OCI_MemAlloc(OCI_IPC_LONG_BUFFER,
                                                      (size_t) lg->maxsize,
                                                      (size_t) 1, FALSE);

                    lg->buffer[0] = 0;
                }
                else if ((lg->size*char_fact) >= (lg->maxsize - trailing_size))
                {
                    lg->maxsize = (lg->size*char_fact) + trailing_size + bufsize;

                    lg->buffer = (ub1 *) OCI_MemRealloc(lg->buffer,
                                                        (size_t) OCI_IPC_LONG_BUFFER,
                                                        (size_t) lg->maxsize, 1);
                }

                res = (lg->buffer != NULL);

                /* update piece info */

                if (res == TRUE)
                {
                    if (lg->type == OCI_CLONG)
                        lg->piecesize -= (ub4) sizeof(odtext);

                    lg->piecesize = (bufsize / sizeof(dtext)) * sizeof(odtext);

                    OCI_CALL1
                    (
                        res, rs->stmt->con, rs->stmt,

                        OCIStmtSetPieceInfo((dvoid *) handle,
                                            (ub4) OCI_HTYPE_DEFINE,
                                            lg->stmt->con->err,
                                            (dvoid *) (lg->buffer + (size_t) lg->size),
                                            &lg->piecesize, piece,
                                            lg->def->buf.inds, (ub2 *) NULL)
                    )

                }

                break;
            }
        }

        /* fetch data */

        #if defined(OCI_STMT_SCROLLABLE_READONLY)

        if (OCILib.use_scrollable_cursors == TRUE)
        {
            rs->fetch_status = OCIStmtFetch2(rs->stmt->stmt, rs->stmt->con->err,
                                             rs->fetch_size, (ub2) OCI_FETCH_NEXT,
                                             (sb4) 0, (ub4) OCI_DEFAULT);
        }
        else
        #endif
        {
            rs->fetch_status = OCIStmtFetch(rs->stmt->stmt, rs->stmt->con->err,
                                            rs->fetch_size, (ub2) OCI_FETCH_NEXT,
                                            (ub4) OCI_DEFAULT);
        }

        /* check for return value of fetch call */

        if (rs->fetch_status == OCI_ERROR)
        {
            OCI_ExceptionOCI(rs->stmt->con->err, rs->stmt->con, rs->stmt, FALSE);
            res = FALSE;
        }
        else if (rs->fetch_status == OCI_SUCCESS_WITH_INFO)
        {
            OCI_ExceptionOCI(rs->stmt->con->err, rs->stmt->con, rs->stmt, TRUE);
            res = TRUE;
        }
        else
        {
            /* search for the given column */

            for (i = 0; i < rs->nb_defs; i++)
            {
                OCI_Define *def = &rs->defs[i];

                if (def->col.type == OCI_CDT_LONG && def->buf.handle == handle)
                {
                    /* get the long object for the given internal row */

                    OCI_Long *lg = (OCI_Long *) def->buf.data[iter];

                    lg->size += lg->piecesize;

                    break;
                }
            }
        }
    }

    /* for LONG columns, set the zero terminal string */

    for (i = 0; i < rs->nb_defs; i++)
    {
        OCI_Define *def = &rs->defs[i];

        if (def->col.type == OCI_CDT_LONG && def->col.subtype == OCI_CLONG)
        {
            for (j = 0; j < def->buf.count; j++)
            {
                OCI_Long *lg = (OCI_Long *) def->buf.data[j];

                if (lg->buffer != NULL)
                    ((odtext *)lg->buffer)[lg->size/sizeof(odtext)] = 0;

                OCI_ConvertString(lg->buffer, (lg->size / sizeof(odtext))+1, sizeof(odtext),
                                  sizeof(dtext));
            }
        }
    }

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_FetchData
 * --------------------------------------------------------------------------------------------- */

boolean OCI_FetchData
(
    OCI_Resultset *rs,
    int            mode,
    int            offset,
    boolean       *err
)
{
    boolean res = TRUE;

    /* let's initialize the error flag to TRUE until the process completes */

    *err = TRUE;

    /* internal fetch */

    #if defined(OCI_STMT_SCROLLABLE_READONLY)

    if (OCILib.use_scrollable_cursors == TRUE)
    {
        rs->fetch_status = OCIStmtFetch2(rs->stmt->stmt, rs->stmt->con->err,
                                         rs->fetch_size, (ub2) mode, (sb4) offset,
                                         (ub4) OCI_DEFAULT);
    }
    else
    #endif
    {
        rs->fetch_status = OCIStmtFetch(rs->stmt->stmt, rs->stmt->con->err,
                                        rs->fetch_size, (ub2) OCI_FETCH_NEXT,
                                        (ub4) OCI_DEFAULT);
    }

    /* check failure */

    if (rs->fetch_status == OCI_ERROR)
    {
        OCI_ExceptionOCI(rs->stmt->con->err, rs->stmt->con, rs->stmt, FALSE);
        res = FALSE;
    }
    else if (rs->fetch_status == OCI_SUCCESS_WITH_INFO)
    {
        OCI_ExceptionOCI(rs->stmt->con->err, rs->stmt->con, rs->stmt, TRUE);
        res = TRUE;
    }

    /* do we need to do a piecewise fetch */

    if (rs->fetch_status == OCI_NEED_DATA)
        res = OCI_FetchPieces(rs);

    /* check string buffer for Unicode builds that need buffer expansion */

    #ifdef OCI_CHECK_DATASTRINGS

    OCI_ResultsetExpandStrings(rs);

    #endif

    /* check for success */

    res = ((res == TRUE) && ((rs->fetch_status == OCI_SUCCESS) ||
                             (rs->fetch_status == OCI_NO_DATA) ||
                             (rs->fetch_status == OCI_SUCCESS_WITH_INFO)));

    /* update internal fetch status and variables */

    if (res == TRUE)
    {
        ub4 row_count   = 0;
        ub4 row_fetched = 0;

        if (rs->fetch_status == OCI_SUCCESS_WITH_INFO)
        {
            OCI_ExceptionOCI(rs->stmt->con->err, rs->stmt->con, rs->stmt, TRUE);
        }

        #if defined(OCI_STMT_SCROLLABLE_READONLY)

        if (rs->stmt->exec_mode  == OCI_SFM_SCROLLABLE)
        {
            OCI_CALL1
            (
                res, rs->stmt->con, rs->stmt,

                OCIAttrGet((dvoid *) rs->stmt->stmt, (ub4) OCI_HTYPE_STMT,
                           (dvoid *) &row_count, (ub4 *) NULL,
                           (ub4) OCI_ATTR_CURRENT_POSITION, rs->stmt->con->err)
            )

            OCI_CALL1
            (
                res, rs->stmt->con, rs->stmt,

                OCIAttrGet((dvoid *) rs->stmt->stmt, (ub4) OCI_HTYPE_STMT,
                           (dvoid *) &row_fetched, (ub4 *) NULL,
                           (ub4) OCI_ATTR_ROWS_FETCHED, rs->stmt->con->err)
            )
        }
        else
        #endif
        {
            row_count   = OCI_GetAffectedRows(rs->stmt);
            row_fetched = row_count - rs->row_count;

        }

        if (rs->row_count < row_count)
            rs->row_count = row_count;

        if (row_fetched > 0)
            rs->row_fetched = row_fetched;

        /* so far, no OCI error occurred, let's clear the error flag */

        *err = FALSE;

        /* check if internal fetch was successful */

        if ((rs->fetch_status == OCI_NO_DATA) && (row_fetched == 0))
        {
            if ((mode == OCI_SFD_NEXT) || (offset > 0))
                rs->eof = TRUE;

            if (offset < 0)
                rs->bof = TRUE;

            res = FALSE;
        }
    }

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_FetchCustom
 * --------------------------------------------------------------------------------------------- */

boolean OCI_FetchCustom
(
    OCI_Resultset *rs,
    int            mode,
    int            offset,
    boolean       *err
)
{
    boolean res = TRUE;

    switch (mode)
    {
        case OCI_SFD_RELATIVE:
        {
            int offset_save = 0;

            if (((offset > 0) && (rs->eof == TRUE)) ||
                ((offset < 0) && (rs->bof == TRUE)) ||
                (offset == 0))
            {
                res = FALSE;
            }
            else
            {
                offset_save = offset;
                offset      = offset - rs->row_fetched + rs->row_cur;
                rs->row_cur = 1;

                res = OCI_FetchData(rs, mode, offset, err);

                if (res == TRUE)
                    rs->row_abs += offset_save;
            }

            break;
        }
        case OCI_SFD_ABSOLUTE:
        {
            if (offset == 0)
            {
                res = FALSE;
            }
            else
            {
                rs->row_abs = 1;
                rs->row_cur = 1;

                res = OCI_FetchData(rs, mode, offset, err);

                if (res == TRUE)
                {
                    rs->row_abs = offset;

                    rs->bof = FALSE;
                    rs->eof = FALSE;
                }
            }

            break;
        }
        default:
        {
            res = FALSE;
        }
    }

    return res;
}

#ifdef OCI_CHECK_DATASTRINGS

/* --------------------------------------------------------------------------------------------- *
 * OCI_ResultsetExpandStrings
 * --------------------------------------------------------------------------------------------- */

boolean OCI_ResultsetExpandStrings
(
    OCI_Resultset *rs
)
{
    ub4 i;
    int j;

    OCI_CHECK(rs == NULL, FALSE)

    for (i = 0; i < rs->nb_defs; i++)
    {
        OCI_Define *def = &rs->defs[i];

        if (def->col.type == OCI_CDT_TEXT)
        {
            for (j = (int) (def->buf.count-1); j >= 0; j--)
            {
                OCI_ConvertString(((ub1*) def->buf.data) + (def->col.bufsize * j),
                                  def->col.bufsize / sizeof(dtext), sizeof(odtext),
                                  sizeof(dtext));
            }
        }
    }

    return TRUE;
}

#endif

/* --------------------------------------------------------------------------------------------- *
 * OCI_ResultsetGetAttrInfo
 * --------------------------------------------------------------------------------------------- */

boolean OCI_ResultsetGetAttrInfo
(
    OCI_Resultset *rs,
    unsigned int   index,
    size_t        *p_size,
    int           *p_type
)
{
    if (index >= rs->nb_defs)
    {
        *p_size = 0;
        *p_type = 0;

        return FALSE;
    }

    switch (rs->defs[index].col.type)
    {
        case OCI_CDT_NUMERIC:
        {
            int type = rs->defs[index].col.subtype;

            if (type & OCI_NUM_SHORT)
            {
                *p_type = OCI_OFT_SHORT;
                *p_size = sizeof(short);
            }
            else if (type & OCI_NUM_INT)
            {
                *p_type = OCI_OFT_INT;
                *p_size = sizeof(int);
            }
            else if (type & OCI_NUM_BIGUINT)
            {
                *p_type = OCI_OFT_BIGINT;
                 *p_size = sizeof(big_int);
            }
            else if (type & OCI_NUM_DOUBLE)
            {
                *p_type = OCI_OFT_DOUBLE;
                *p_size = sizeof(double);
            }

            break;
        }
        default:
        {
            *p_size = sizeof(void *);
            *p_type = OCI_OFT_POINTER;
        }
    }

    return TRUE;
}

/* ********************************************************************************************* *
 *                            PUBLIC FUNCTIONS
 * ********************************************************************************************* */

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetResultset
 * --------------------------------------------------------------------------------------------- */

OCI_Resultset * OCI_API OCI_GetResultset
(
    OCI_Statement *stmt
)
{
    OCI_Resultset *rs = NULL;
    boolean res       = TRUE;

    OCI_CHECK_PTR(OCI_IPC_STATEMENT, stmt, NULL);

    OCI_CHECK_STMT_STATUS(stmt, OCI_STMT_CLOSED, NULL);
    OCI_CHECK_STMT_STATUS(stmt, OCI_STMT_PREPARED, NULL);

    /* if the sql statement does not return a result, we just return NULL and not
       throwing any exception */

    if ((stmt->type != OCI_CST_SELECT) || (stmt->nb_rbinds == 0))
    {
        /* if the resultset exists, let's use it */

        if ((stmt->rsts != NULL) && (stmt->rsts[0] != NULL))
        {
            rs = stmt->rsts[0];
        }

        /* allocate resultset for select statements */

        if ((rs == NULL)  && (stmt->type == OCI_CST_SELECT))
        {
            /* allocate memory for one resultset handle */

            stmt->rsts = (OCI_Resultset **) OCI_MemAlloc(OCI_IPC_RESULTSET_ARRAY,
                                                         sizeof(*stmt->rsts),
                                                         (size_t) 1, TRUE);
            if (stmt->rsts != NULL)
            {
                stmt->nb_rs  = 1;
                stmt->cur_rs = 0;

                /* create resultset object */

                rs = OCI_ResultsetCreate(stmt, stmt->fetch_size);

                if (rs != NULL)
                    stmt->rsts[0] = rs;
                else
                    res = FALSE;
            }
            else
                res = FALSE;
        }
    }

    OCI_RESULT(res);

    return rs;
}

/* ------------------------------------------------------------------------ v*
 * OCI_GetNextResultset
 * --------------------------------------------------------------------------------------------- */

OCI_Resultset * OCI_API OCI_GetNextResultset
(
    OCI_Statement *stmt
)
{
    OCI_Resultset *rs = NULL;

    OCI_CHECK_PTR(OCI_IPC_STATEMENT, stmt, NULL);

    if (stmt->cur_rs < (stmt->nb_rs-1))
        rs = stmt->rsts[++stmt->cur_rs];

    OCI_RESULT(TRUE);

    return rs;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ResultsetInit
 * --------------------------------------------------------------------------------------------- */

boolean OCI_ResultsetInit
(
    OCI_Resultset *rs
)
{
    rs->bof          = TRUE;
    rs->eof          = FALSE;
    rs->fetch_status = OCI_SUCCESS;
    rs->row_count    = 0;
    rs->row_cur      = 0;
    rs->row_abs      = 0;
    rs->row_fetched  = 0;

    return TRUE;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ResultsetFree
 * --------------------------------------------------------------------------------------------- */

boolean OCI_ResultsetFree
(
    OCI_Resultset *rs
)
{
    boolean res = TRUE;
    ub4 i, j;

    OCI_CHECK_PTR(OCI_IPC_RESULTSET, rs, FALSE);

    for (i = 0; i < rs->nb_defs; i++)
    {
        OCI_Define *def = &(rs->defs[i]);

        /* free buffer objects */

        if (def->obj != NULL)
        {
            /* handy cast to set object state flag */

            ((OCI_Datatype *) def->obj)->hstate = OCI_OBJECT_FETCHED_DIRTY;

            switch (def->col.type)
            {
                case OCI_CDT_DATETIME:

                    OCI_DateFree((OCI_Date *) def->obj);
                    break;

                case OCI_CDT_LOB:

                    OCI_LobFree((OCI_Lob *) def->obj);
                    break;

                case OCI_CDT_FILE:

                    OCI_FileFree((OCI_File *) def->obj);
                    break;

                case OCI_CDT_CURSOR:

                    OCI_StatementClose((OCI_Statement *) def->obj);
                    OCI_FREE(def->obj);
                    break;

                case OCI_CDT_OBJECT:

                    OCI_ObjectFree((OCI_Object *) def->obj);
                    break;

                case OCI_CDT_COLLECTION:

                    OCI_CollFree((OCI_Coll *) def->obj);
                    break;

                case OCI_CDT_REF:

                    OCI_RefFree((OCI_Ref *) def->obj);
                    break;

                case OCI_CDT_TIMESTAMP:

                    OCI_TimestampFree((OCI_Timestamp *) def->obj);
                    break;

                case OCI_CDT_INTERVAL:

                    OCI_IntervalFree((OCI_Interval *) def->obj);
                    break;
            }

            def->obj = NULL;
        }

        /* free OCI handles */

        if (def->col.dtype != 0)
        {
            if (def->col.type == OCI_CDT_CURSOR)
            {
                for(j=0; j < def->buf.count; j++)
                {
                    OCI_HandleFree((dvoid *) def->buf.data[j],
                                   (ub4    ) def->col.dtype);
                }
            }
            else
            {
                OCI_DescriptorArrayFree((dvoid *) def->buf.data,
                                        (ub4    ) def->col.dtype,
                                        (ub4    ) def->buf.count);
            }
        }

        /* free OCI long buffers */

        if (def->col.type == OCI_CDT_LONG && def->buf.data != NULL)
        {
            for(j=0; j < def->buf.count; j++)
            {
                if (def->buf.data[j] != NULL)
                {
                    ((OCI_Datatype *) def->buf.data[j])->hstate = OCI_OBJECT_FETCHED_DIRTY;

                    OCI_LongFree((OCI_Long *) def->buf.data[j]);
                }
            }
        }

        /* free column pointers */

        OCI_FREE(def->col.name);

        /* free buffer pointers */

        OCI_FREE(def->buf.data);
        OCI_FREE(def->buf.inds);
        OCI_FREE(def->buf.obj_inds);
        OCI_FREE(def->buf.lens);
        OCI_FREE(def->buf.tmpbuf);
    }

    /* free column map */

    if (rs->map != NULL)
    {
        OCI_HashFree(rs->map);
    }

    /* free defines (column array) */

    OCI_FREE(rs->defs);

    OCI_FREE(rs);

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_FetchPrev
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_FetchPrev
(
    OCI_Resultset *rs
)
{
    boolean res = TRUE;
    boolean err = FALSE;

    OCI_CHECK_PTR(OCI_IPC_RESULTSET, rs, FALSE);

    OCI_CHECK_SCROLLABLE_CURSOR_ENABLED(rs->stmt->con, FALSE);

    #if OCI_VERSION_COMPILE >= OCI_9_0

    OCI_CHECK_SCROLLABLE_CURSOR_ACTIVATED(rs->stmt, FALSE);

    if (rs->bof == FALSE)
    {
        if (rs->row_cur == 1)
        {
            if (rs->row_abs == 1)
            {
                rs->bof = TRUE;
            }
            else
            {
                int offset = 0;

                if (rs->fetch_size > rs->row_abs)
                    offset = 1 - rs->row_abs;
                else
                    offset = 1 - (rs->fetch_size + rs->row_fetched);

                res = OCI_FetchData(rs, OCI_SFD_RELATIVE, offset, &err);

                if (res == TRUE)
                {
                    if (rs->fetch_size > rs->row_abs)
                        rs->row_cur = rs->row_abs-1;
                    else
                        rs->row_cur = rs->fetch_size;

                    rs->row_abs--;
                }
            }
        }
        else
        {
            rs->row_cur--;
            rs->row_abs--;
        }

        rs->eof = FALSE;

        res = ((res == TRUE) && (rs->bof == FALSE));
    }
    else
        res = FALSE;

    #else

    res = FALSE;

    #endif

    OCI_RESULT(err == FALSE);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_FetchNext
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_FetchNext
(
    OCI_Resultset *rs
)
{
    boolean res = TRUE;
    boolean err = FALSE;

    OCI_CHECK_PTR(OCI_IPC_RESULTSET, rs, FALSE);

    if (rs->eof == FALSE)
    {
        if (rs->stmt->nb_rbinds == 0)
        {
            /* for regular resultsets */

            if ((rs->row_cur == rs->row_fetched))
            {
                if (rs->fetch_status == OCI_NO_DATA)
                {
                    rs->eof = TRUE;
                }
                else
                {
                    res = OCI_FetchData(rs, OCI_SFD_NEXT, 0, &err);

                    if (res == TRUE)
                    {
                        rs->bof     = FALSE;
                        rs->row_cur = 1;

                        rs->row_abs++;
                    }
                }
            }
            else
            {
                rs->row_cur++;
                rs->row_abs++;
            }
        }
        else
        {
            /* for resultset from returning into clause */

            if (rs->row_abs == 0)
            {
                /* check string buffer once for Unicode build */

            #ifdef OCI_CHECK_DATASTRINGS

                OCI_ResultsetExpandStrings(rs);

            #endif

            }

            if (rs->row_abs >= rs->row_count)
            {
                rs->eof = TRUE;
            }
            else
            {
                rs->row_cur++;
                rs->row_abs++;
            }
        }
    }
    else
        res = FALSE;

    OCI_RESULT(err == FALSE);

    return ((res == TRUE) && (rs->eof == FALSE));
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_FetchFirst
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_FetchFirst
(
    OCI_Resultset *rs
)
{
    boolean res = TRUE;
    boolean err = FALSE;

    OCI_CHECK_PTR(OCI_IPC_RESULTSET, rs, FALSE);

    OCI_CHECK_SCROLLABLE_CURSOR_ENABLED(rs->stmt->con, FALSE);

    #if OCI_VERSION_COMPILE >= OCI_9_0

    OCI_CHECK_SCROLLABLE_CURSOR_ACTIVATED(rs->stmt, FALSE);

    rs->bof = FALSE;
    rs->eof = FALSE;

    rs->row_abs = 1;
    rs->row_cur = 1;

    res = OCI_FetchData(rs, OCI_SFD_FIRST, 0, &err);

    #else

    res = FALSE;
    err = TRUE;

    #endif

    OCI_RESULT(err == FALSE);

    return ((res == TRUE) && (rs->bof == FALSE));
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_FetchLast
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_FetchLast
(
    OCI_Resultset *rs
)
{
    boolean res = TRUE;
    boolean err = FALSE;

    OCI_CHECK_PTR(OCI_IPC_RESULTSET, rs, FALSE);

    OCI_CHECK_SCROLLABLE_CURSOR_ENABLED(rs->stmt->con, FALSE);

    #if OCI_VERSION_COMPILE >= OCI_9_0

    OCI_CHECK_SCROLLABLE_CURSOR_ACTIVATED(rs->stmt, FALSE);

    rs->bof = FALSE;
    rs->eof = FALSE;

    rs->row_abs = 0;
    rs->row_cur = 1;

    res = OCI_FetchData(rs, OCI_SFD_LAST, 0, &err);

    rs->row_abs = rs->row_count;

    #else

    res = FALSE;
    err = TRUE;

    #endif

    OCI_RESULT(err == FALSE);

    return ((res == TRUE) && (rs->eof != TRUE));
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_FetchSeek
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_FetchSeek
(
    OCI_Resultset *rs,
    unsigned int   mode,
    int            offset
)
{
    boolean res = TRUE;
    boolean err = FALSE;

    OCI_CHECK_PTR(OCI_IPC_RESULTSET, rs, FALSE);

    OCI_CHECK_SCROLLABLE_CURSOR_ENABLED(rs->stmt->con, FALSE);

    #if OCI_VERSION_COMPILE >= OCI_9_0

    OCI_CHECK_SCROLLABLE_CURSOR_ACTIVATED(rs->stmt, FALSE);

    res = OCI_FetchCustom(rs, mode, offset, &err);

    #else

    OCI_NOT_USED(mode);
    OCI_NOT_USED(offset);

    res = FALSE;
    err = TRUE;

    #endif

    OCI_RESULT(err == FALSE);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetRowCount
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_GetRowCount
(
    OCI_Resultset *rs
)
{
    OCI_CHECK_PTR(OCI_IPC_RESULTSET, rs, 0);

    OCI_RESULT(TRUE);

    return rs->row_count;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetRowCount
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_GetCurrentRow
(
    OCI_Resultset *rs
)
{
    OCI_CHECK_PTR(OCI_IPC_RESULTSET, rs, 0);

    OCI_RESULT(TRUE);

    return rs->row_abs;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetColumnCount
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_GetColumnCount
(
    OCI_Resultset *rs
)
{
    OCI_CHECK_PTR(OCI_IPC_RESULTSET, rs, 0);

    OCI_RESULT(TRUE);

    return rs->nb_defs;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetColumn
 * --------------------------------------------------------------------------------------------- */

OCI_Column * OCI_API OCI_GetColumn
(
    OCI_Resultset *rs,
    unsigned int   index
)
{
    OCI_Define *def = NULL;
    OCI_Column *col = NULL;

    def = OCI_GetDefine(rs, index);

    if (def != NULL)
        col = &def->col;

    OCI_RESULT(col != NULL);

    return col;

}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetColumn2
 * --------------------------------------------------------------------------------------------- */

OCI_Column * OCI_API OCI_GetColumn2
(
    OCI_Resultset *rs,
    const mtext   *name
)
{
    OCI_Define *def = NULL;
    OCI_Column *col = NULL;

    def = OCI_GetDefine(rs, OCI_GetDefineIndex(rs, name));

    if (def != NULL)
        col = &def->col;

    OCI_RESULT(col != NULL);

    return col;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetColumnIndex
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_GetColumnIndex
(
    OCI_Resultset *rs,
    const mtext   *name
)
{
    int index = OCI_GetDefineIndex(rs, name);

    OCI_RESULT(index >= 1);

    if (index <= 0)
        index = 0;

    return (unsigned int) index;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_SetStructNumericType
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_SetStructNumericType
(
    OCI_Resultset *rs,
    unsigned int   index,
    unsigned int   type
)
{
    OCI_CHECK_PTR(OCI_IPC_RESULTSET, rs, FALSE);

    OCI_CHECK_BOUND(rs->stmt->con, index,  1,  rs->nb_defs, FALSE);

    OCI_CHECK_COMPAT(rs->stmt->con, rs->defs[index-1].col.type == OCI_CDT_NUMERIC, FALSE);

    rs->defs[index-1].col.subtype = type;

    OCI_RESULT(TRUE);

    return TRUE;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_SetStructNumericType2
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_SetStructNumericType2
(
    OCI_Resultset *rs,
    const mtext   *name,
    unsigned int   type
)
{
    return OCI_SetStructNumericType(rs, OCI_GetDefineIndex(rs, name), type);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetStruct
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_GetStruct
(
    OCI_Resultset *rs,
    void          *row_struct,
    void          *row_struct_ind
)
{
    char *ptr     = NULL;
    boolean *inds = NULL;
    boolean res   = TRUE;
    size_t size   = 0;
    size_t size1  = 0;
    size_t size2  = 0;
    int type1     = 0;
    int type2     = 0;
 
    ub4 i;

    OCI_CHECK_PTR(OCI_IPC_RESULTSET, rs, FALSE);
    OCI_CHECK_PTR(OCI_IPC_VOID, row_struct, FALSE);

    ptr  = row_struct;
    inds = row_struct_ind;

    if (ptr != NULL)
    {
        for (i = 1; i <= rs->nb_defs; i++)
        {
            OCI_ResultsetGetAttrInfo(rs, i-1, &size1, &type1);
            OCI_ResultsetGetAttrInfo(rs, i,   &size2, &type2);

            switch (rs->defs[i-1].col.type)
            {
                case OCI_CDT_NUMERIC:
                {
                    OCI_DefineGetNumber(rs, i, ptr,
                                        rs->defs[i-1].col.subtype,
                                        (uword) size1);

                    break;
                }
                case OCI_CDT_TEXT:
                {
                    dtext **str = (dtext **) ptr;

                    *str =  (dtext * ) OCI_GetString(rs, i);

                    break;
                }
                case OCI_CDT_RAW:
                {
                    void **buf = (void **) ptr;

                    *buf = OCI_DefineGetData(&rs->defs[i]);

                    break;
                }
                case OCI_CDT_LONG:
                {
                    OCI_Long **lg = (OCI_Long **) ptr;

                    *lg = OCI_GetLong(rs, i);

                    break;
                }
                case OCI_CDT_DATETIME:
                {
                    OCI_Date **date = (OCI_Date **) ptr;

                    *date = OCI_GetDate(rs, i);

                    break;
                }
                case OCI_CDT_CURSOR:
                {
                    OCI_Statement **stmt = (OCI_Statement **) ptr;

                    *stmt = OCI_GetStatement(rs, i);

                    break;
                }
                case OCI_CDT_LOB:
                {
                    OCI_Lob **lob = (OCI_Lob **) ptr;

                    *lob = OCI_GetLob(rs, i);

                    break;
                }
                case OCI_CDT_FILE:
                {
                    OCI_File **file = (OCI_File **) ptr;

                    *file = OCI_GetFile(rs, i);

                    break;
                }
                case OCI_CDT_TIMESTAMP:
                {
                    OCI_Timestamp **tmsp = (OCI_Timestamp **) ptr;

                    *tmsp = OCI_GetTimestamp(rs, i);

                    break;
                }
                case OCI_CDT_INTERVAL:
                {
                    OCI_Interval **itv = (OCI_Interval **) ptr;

                    *itv = OCI_GetInterval(rs, i);

                    break;
                }
                case OCI_CDT_OBJECT:
                {
                    OCI_Object **obj = (OCI_Object **) ptr;

                    *obj = OCI_GetObject(rs, i);

                    break;
                }
                case OCI_CDT_COLLECTION:
                {
                    OCI_Coll **coll = (OCI_Coll **) ptr;

                    *coll = OCI_GetColl(rs, i);

                    break;
                }
                case OCI_CDT_REF:
                {
                    OCI_Ref **ref = (OCI_Ref **) ptr;

                    *ref = OCI_GetRef(rs, i);

                    break;
                }
            }

            if ((size2 > 0)  && ((size + size1) % size2))
            {
                size1 = ROUNDUP(size1, size2);
            }
         
            size += size1;

            ptr = ((char *) row_struct) + size;


            if (inds != NULL)
            {
                *inds = OCI_NOT_NULL(&rs->defs[i-1]);

                inds++;
            }
        }
    }

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetShort
 * --------------------------------------------------------------------------------------------- */

short OCI_API OCI_GetShort
(
    OCI_Resultset *rs,
    unsigned int   index
)
{
    short value = 0;

    OCI_DefineGetNumber(rs, index, &value, OCI_NUM_SHORT, sizeof(value));

    return value;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetShort2
 * --------------------------------------------------------------------------------------------- */

short OCI_API OCI_GetShort2
(
    OCI_Resultset *rs,
    const mtext   *name
)
{
    return OCI_GetShort(rs, OCI_GetDefineIndex(rs, name));
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetUnsignedShort
 * --------------------------------------------------------------------------------------------- */

unsigned short OCI_API OCI_GetUnsignedShort
(
    OCI_Resultset *rs,
    unsigned int   index
)
{
    unsigned short value = 0;

    OCI_DefineGetNumber(rs, index, &value, OCI_NUM_USHORT, sizeof(value));

    return value;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetUnsignedShort2
 * --------------------------------------------------------------------------------------------- */

unsigned short OCI_API OCI_GetUnsignedShort2
(
    OCI_Resultset *rs,
    const mtext   *name
)
{
    return OCI_GetUnsignedShort(rs, OCI_GetDefineIndex(rs, name));
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetInt
 * --------------------------------------------------------------------------------------------- */

int OCI_API OCI_GetInt
(
    OCI_Resultset *rs,
    unsigned int   index
)
{
    int value = 0;

    OCI_DefineGetNumber(rs, index, &value, OCI_NUM_INT, sizeof(value));

    return value;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetInt2
 * --------------------------------------------------------------------------------------------- */

int OCI_API OCI_GetInt2
(
    OCI_Resultset *rs,
    const mtext   *name
)
{
    return OCI_GetInt(rs, OCI_GetDefineIndex(rs, name));
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetUnsignedInt
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_GetUnsignedInt
(
    OCI_Resultset *rs,
    unsigned int   index
)
{
    unsigned int value = 0;

    OCI_DefineGetNumber(rs, index, &value, OCI_NUM_UINT, sizeof(value));

    return value;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetUnsignedInt2
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_GetUnsignedInt2
(
    OCI_Resultset *rs,
    const mtext   *name
)
{
    return OCI_GetUnsignedInt(rs, OCI_GetDefineIndex(rs, name));
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetBigInt
 * --------------------------------------------------------------------------------------------- */

big_int OCI_API OCI_GetBigInt
(
    OCI_Resultset *rs,
    unsigned int   index
)
{
    big_int value = 0;

    OCI_DefineGetNumber(rs, index, &value, OCI_NUM_BIGINT, sizeof(value));

    return value;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetBigInt2
 * --------------------------------------------------------------------------------------------- */

big_int OCI_API OCI_GetBigInt2
(
    OCI_Resultset *rs,
    const mtext   *name
)
{
    return OCI_GetBigInt(rs, OCI_GetDefineIndex(rs, name));
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetUnsignedBigInt
 * --------------------------------------------------------------------------------------------- */

big_uint OCI_API OCI_GetUnsignedBigInt
(
    OCI_Resultset *rs,
    unsigned int   index
)
{
    big_uint value = 0;

    OCI_DefineGetNumber(rs, index, &value, OCI_NUM_BIGUINT, sizeof(value));

    return value;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetUnsignedInt2
 * --------------------------------------------------------------------------------------------- */

big_uint OCI_API OCI_GetUnsignedBigInt2
(
    OCI_Resultset *rs,
    const mtext   *name
)
{
    return OCI_GetUnsignedBigInt(rs, OCI_GetDefineIndex(rs, name));
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetString
 * --------------------------------------------------------------------------------------------- */

const dtext * OCI_API OCI_GetString
(
    OCI_Resultset *rs,
    unsigned int   index
)
{
    OCI_Define *def = OCI_GetDefine(rs, index);
    dtext *str      = NULL;
    boolean res     = TRUE;

    res = (def != NULL);

    if ((res == TRUE) && (OCI_NOT_NULL(def) == TRUE))
    {
        void *data = OCI_DefineGetData(def);

        if (def->col.type == OCI_CDT_TEXT)
        {
            str = (dtext *) data;

            /* for long mapped to string, the zero terminal character is not
               always added by Oracle ? or OCILIB issue ? Anyway we check the
               length returned by Oracle and set it properly */

            if (def->col.subtype == OCI_CLONG)
            {
                ub2* lens = (ub2 *) def->buf.lens;

                str[lens[rs->row_cur-1]] = 0;
            }
        }
        else
        {
            /* tries an implicit conversion if possible */

            if (res == TRUE)
            {
                switch (def->col.type)
                {
                case OCI_CDT_NUMERIC:
                {
                    void *ostr1 = NULL;
                    void *ostr2 = NULL;
                    int osize1  = OCI_SIZE_FORMAT_NUML * (int) sizeof(mtext);
                    int osize2  = OCI_SIZE_BUFFER      * (int) sizeof(dtext);
                    const mtext *fmt;
                    int pos;

                    res = OCI_DefineRequestBuffer(def, OCI_SIZE_BUFFER);

                    if (res == TRUE)
                    {
                        /* init output buffer in case of OCI failure */

                        fmt   = OCI_GetDefaultFormatNumeric(rs->stmt->con);
                        ostr1 = OCI_GetInputMetaString(fmt, &osize1);

                        #ifndef OCI_CHARSET_MIXED

                        ostr2 = OCI_GetInputString(def->buf.tmpbuf, &osize2,
                                                   sizeof(dtext), sizeof(omtext));
                        #else

                        ostr2 = (void* ) def->buf.tmpbuf;

                        #endif

                        /* check for decimal character */

                        OCI_CALL1
                        (
                            res, rs->stmt->con, rs->stmt,

                            OCINumberToText(rs->stmt->con->err,
                                            (OCINumber *) data,
                                            (oratext *) ostr1,
                                            (ub4) osize1,
                                            (oratext *) NULL,
                                            (ub4) 0,
                                            (ub4 *) &osize2,
                                            (oratext *) ostr2)
                        )

                        #ifndef OCI_CHARSET_MIXED

                        OCI_GetOutputString(ostr2, def->buf.tmpbuf, &osize2,
                                            sizeof(omtext), sizeof(dtext));

                        #else

                        OCI_ConvertString(ostr2, osize2/sizeof(omtext)+1,
                                          sizeof(omtext), sizeof(dtext));

                        osize2 = (osize2/sizeof(omtext)) * sizeof(dtext);

                        #endif

                        /* do we need to suppress last '.' or ',' from integers
                         **/

                        pos = (osize2 / (int) sizeof(dtext)) - 1;

                        if (pos >= 0)
                        {
                            if ((def->buf.tmpbuf[pos] == DT('.')) ||
                                (def->buf.tmpbuf[pos] == DT(',')))
                            {
                                def->buf.tmpbuf[pos] = 0;
                            }
                        }

                        OCI_ReleaseMetaString(ostr1);

                        #ifndef OCI_CHARSET_MIXED

                        OCI_ReleaseDataString(ostr2);

                        #endif

                        str = def->buf.tmpbuf;
                    }

                    break;
                }
                case OCI_CDT_DATETIME:
                {
                    OCI_Date *date   = OCI_GetDate(rs, index);
                    const mtext *fmt = OCI_GetDefaultFormatDate(rs->stmt->con);

                    if (date != NULL)
                    {
                        res = OCI_DefineRequestBuffer(def, OCI_SIZE_BUFFER);

                        if (res == TRUE)
                        {
                            res = OCI_DateToText(date, fmt, OCI_SIZE_BUFFER,
                                                 (mtext *) def->buf.tmpbuf);

                            #ifdef OCI_CHARSET_MIXED

                            OCI_ConvertString(def->buf.tmpbuf,
                                              (int) mtslen((mtext*) def->buf.tmpbuf),
                                              sizeof(mtext),
                                              sizeof(dtext));

                            #endif

                            str = def->buf.tmpbuf;
                        }
                    }

                    break;
                }
                case OCI_CDT_TIMESTAMP:
                {
                    OCI_Timestamp *tmsp = OCI_GetTimestamp(rs, index);
                    const mtext *fmt    = OCI_GetDefaultFormatDate(rs->stmt->con);

                    if (tmsp != NULL)
                    {
                        res = OCI_DefineRequestBuffer(def, OCI_SIZE_BUFFER);

                        if (res == TRUE)
                        {
                            OCI_TimestampToText(tmsp, fmt, OCI_SIZE_BUFFER,
                                                (mtext *) def->buf.tmpbuf,  0);

                            #ifdef OCI_CHARSET_MIXED

                            OCI_ConvertString(def->buf.tmpbuf,
                                              (int) mtslen((mtext*) def->buf.tmpbuf),
                                              sizeof(mtext),
                                              sizeof(dtext));

                            #endif

                            str = def->buf.tmpbuf;
                        }
                    }

                    break;
                }
                case OCI_CDT_INTERVAL:
                {
                    OCI_Interval *itv = OCI_GetInterval(rs, index);

                    if (itv != NULL)
                    {
                        res = OCI_DefineRequestBuffer(def, OCI_SIZE_BUFFER);

                        if (res == TRUE)
                        {
                            OCI_IntervalToText(OCI_GetInterval(rs, index),
                                               OCI_STRING_DEFAULT_PREC,
                                               OCI_STRING_DEFAULT_PREC,
                                               OCI_SIZE_BUFFER,
                                               (mtext *) def->buf.tmpbuf);

                            #ifdef OCI_CHARSET_MIXED

                            OCI_ConvertString(def->buf.tmpbuf,
                                              (int) mtslen((mtext*) def->buf.tmpbuf),
                                              sizeof(mtext),
                                              sizeof(dtext));
                            #endif

                            str = def->buf.tmpbuf;
                        }
                    }

                    break;
                }
                case OCI_CDT_LONG:
                {
                    OCI_Long *lg = OCI_GetLong(rs, index);

                    if (lg != NULL)
                    {
                        str = OCI_LongGetBuffer(lg);
                    }

                    break;

                }
                case OCI_CDT_RAW:
                {
                    str = (dtext *) data;

                    break;

                }
                case OCI_CDT_LOB:
                {
                    OCI_Lob *lob = OCI_GetLob(rs, index);

                    if (lob != NULL)
                    {
                        big_uint len = OCI_LobGetLength(lob);

                        if (lob->type == OCI_BLOB)
                        {
                            len /= sizeof(dtext);
                        }

                        res = OCI_DefineRequestBuffer(def, (unsigned int) len);

                        if (res == TRUE)
                        {
                            res = OCI_LobRead(lob, def->buf.tmpbuf, (unsigned int) len) &&
                                  OCI_LobSeek(lob, 0, OCI_SEEK_SET);

                            str = def->buf.tmpbuf;
                        }
                    }

                    break;

                }
                case OCI_CDT_FILE:
                {
                    OCI_File *file = OCI_GetFile(rs, index);

                    if (file != NULL)
                    {
                        big_uint len = OCI_FileGetSize(file) / sizeof(dtext);

                        res = OCI_DefineRequestBuffer(def, (unsigned int) len);

                        if (res == TRUE)
                        {
                            res = OCI_FileRead(file, def->buf.tmpbuf, (unsigned int) len) &&
                                  OCI_FileSeek(file, 0, OCI_SEEK_SET);

                            memset(((char*) def->buf.tmpbuf) + len, 0, sizeof(dtext));

                            str = def->buf.tmpbuf;
                        }
                    }

                    break;

                }
                case OCI_CDT_REF:
                {
                    OCI_Ref *ref = OCI_GetRef(rs, index);

                    if (ref != NULL)
                    {
                        res = OCI_DefineRequestBuffer(def, OCI_SIZE_BUFFER);

                        if (res == TRUE)
                        {
                            OCI_RefToText(ref, OCI_SIZE_BUFFER, (mtext *) def->buf.tmpbuf);

                            #ifdef OCI_CHARSET_MIXED

                            OCI_ConvertString(def->buf.tmpbuf,
                                              (int) mtslen((mtext*) def->buf.tmpbuf),
                                              sizeof(mtext),
                                              sizeof(dtext));

                            #endif

                            str = def->buf.tmpbuf;
                        }
                    }

                    break;
                }
                default:
                {
                    res = FALSE;
                }
                }
            }
        }
    }

    OCI_RESULT(res);

    return str;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetString2
 * --------------------------------------------------------------------------------------------- */

const dtext * OCI_API OCI_GetString2
(
    OCI_Resultset *rs,
    const mtext   *name
)
{
    return OCI_GetString(rs, OCI_GetDefineIndex(rs, name));
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetRaw
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_GetRaw
(
    OCI_Resultset *rs,
    unsigned int   index,
    void          *buffer,
    unsigned int   len
)
{
    OCI_Define *def = OCI_GetDefine(rs, index);
    boolean res     = TRUE;
    ub2 count       = (ub2) len;

    OCI_CHECK_PTR(OCI_IPC_VOID, buffer, 0);

    res = (def != NULL);

    if ((OCI_NOT_NULL(def) == TRUE) && (def->col.type == OCI_CDT_RAW))
    {
        ub2 size = ((ub2*)def->buf.lens)[def->rs->row_cur-1];

        if (count > size)
            count = size;

        /* for RAWs, we copy the data in the destination buffer instead of
           returning internal buffer as we do for strings */

        memcpy(buffer, OCI_DefineGetData(def), (size_t) count);

    }

    OCI_RESULT(res);

    if (res == FALSE)
        count = 0;

    return (unsigned int) count;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetRaw2
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_GetRaw2
(
    OCI_Resultset *rs,
    const mtext   *name,
    void          *buffer,
    unsigned int   len
)
{
    return OCI_GetRaw(rs, OCI_GetDefineIndex(rs, name), buffer, len);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetDouble
 * --------------------------------------------------------------------------------------------- */

double OCI_API OCI_GetDouble
(
    OCI_Resultset *rs,
    unsigned int   index
)
{
    double value = 0.0;

    OCI_DefineGetNumber(rs, index, &value, OCI_NUM_DOUBLE, sizeof(value));

    return value;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetDouble2
 * --------------------------------------------------------------------------------------------- */

double OCI_API OCI_GetDouble2
(
    OCI_Resultset *rs,
    const mtext   *name
)
{
    return OCI_GetDouble(rs, OCI_GetDefineIndex(rs, name));
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetDate
 * --------------------------------------------------------------------------------------------- */

OCI_Date * OCI_API OCI_GetDate
(
    OCI_Resultset *rs,
    unsigned int   index
)
{
    OCI_Define *def = OCI_GetDefine(rs, index);
    OCI_Date *date  = NULL;

    if ((OCI_NOT_NULL(def) == TRUE) && (def->col.type == OCI_CDT_DATETIME))
    {
        date =  OCI_DateInit(rs->stmt->con,
                             (OCI_Date **) &def->obj,
                             (OCIDate *) OCI_DefineGetData(def), FALSE,
                             (def->col.icode == SQLT_DAT));
    }

    OCI_RESULT(date != NULL);

    return date;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetDate2
 * --------------------------------------------------------------------------------------------- */

OCI_Date * OCI_API OCI_GetDate2
(
    OCI_Resultset *rs,
    const mtext   *name
)
{
    return OCI_GetDate(rs, OCI_GetDefineIndex(rs, name));
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetTimestamp
 * --------------------------------------------------------------------------------------------- */

OCI_Timestamp * OCI_API OCI_GetTimestamp
(
    OCI_Resultset *rs,
    unsigned int   index
)
{
    OCI_Define *def     = OCI_GetDefine(rs, index);
    OCI_Timestamp *tmsp = NULL;

    if ((OCI_NOT_NULL(def) == TRUE) && (def->col.type == OCI_CDT_TIMESTAMP))
    {
        tmsp =  OCI_TimestampInit(rs->stmt->con,
                                  (OCI_Timestamp **) &def->obj,
                                  (OCIDateTime *) OCI_DefineGetData(def),
                                  def->col.subtype);
    }

    OCI_RESULT(tmsp != NULL);

    return tmsp;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetTimestamp2
 * --------------------------------------------------------------------------------------------- */

OCI_Timestamp * OCI_API OCI_GetTimestamp2
(
    OCI_Resultset *rs,
    const mtext   *name
)
{
    return OCI_GetTimestamp(rs, OCI_GetDefineIndex(rs, name));
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetInterval
 * --------------------------------------------------------------------------------------------- */

OCI_Interval * OCI_API OCI_GetInterval
(
    OCI_Resultset *rs,
    unsigned int   index
)
{
    OCI_Define *def   = OCI_GetDefine(rs, index);
    OCI_Interval *itv = NULL;

    if ((OCI_NOT_NULL(def) == TRUE) && (def->col.type == OCI_CDT_INTERVAL))
    {
        itv = OCI_IntervalInit(rs->stmt->con,
                               (OCI_Interval **) &def->obj,
                               (OCIInterval *) OCI_DefineGetData(def),
                               def->col.subtype);
    }

    OCI_RESULT(itv != NULL);

    return itv;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetInterval2
 * --------------------------------------------------------------------------------------------- */

OCI_Interval * OCI_API OCI_GetInterval2
(
    OCI_Resultset *rs,
    const mtext   *name
)
{
    return OCI_GetInterval(rs, OCI_GetDefineIndex(rs, name));
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_Object
 * --------------------------------------------------------------------------------------------- */

OCI_Object * OCI_API OCI_GetObject
(
    OCI_Resultset *rs,
    unsigned int   index
)
{
    OCI_Define *def = OCI_GetDefine(rs, index);
    OCI_Object *obj =  NULL;

    if ((OCI_NOT_NULL(def) == TRUE) && (def->col.type == OCI_CDT_OBJECT))
    {
        obj =  OCI_ObjectInit(rs->stmt->con,
                              (OCI_Object **) &def->obj,
                              OCI_DefineGetData(def), def->col.typinf, NULL, -1,
                              TRUE);
    }

    OCI_RESULT(obj != NULL);

    return obj;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetObject2
 * --------------------------------------------------------------------------------------------- */

OCI_Object * OCI_API OCI_GetObject2
(
    OCI_Resultset *rs,
    const mtext   *name
)
{
    return OCI_GetObject(rs, OCI_GetDefineIndex(rs, name));
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetColl
 * --------------------------------------------------------------------------------------------- */

OCI_Coll * OCI_API OCI_GetColl
(
    OCI_Resultset *rs,
    unsigned int   index
)
{
    OCI_Define *def = OCI_GetDefine(rs, index);
    OCI_Coll *coll  = NULL;

    if ((OCI_NOT_NULL(def) == TRUE) && (def->col.type == OCI_CDT_COLLECTION))
    {
        coll = OCI_CollInit(rs->stmt->con, (OCI_Coll **) &def->obj,
                            OCI_DefineGetData(def), def->col.typinf);
    }

    OCI_RESULT(coll != NULL);

    return coll;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetColl2
 * --------------------------------------------------------------------------------------------- */

OCI_Coll * OCI_API OCI_GetColl2
(
    OCI_Resultset *rs,
    const mtext   *name
)
{
    return OCI_GetColl(rs, OCI_GetDefineIndex(rs, name));
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetRef
 * --------------------------------------------------------------------------------------------- */

OCI_Ref * OCI_API OCI_GetRef
(
    OCI_Resultset *rs,
    unsigned int   index
)
{
    OCI_Define *def = OCI_GetDefine(rs, index);
    OCI_Ref *ref    = NULL;

    if ((OCI_NOT_NULL(def) == TRUE) && (def->col.type == OCI_CDT_REF))
    {
        ref = OCI_RefInit(rs->stmt->con, def->col.typinf,
                          (OCI_Ref **) &def->obj, OCI_DefineGetData(def));

        /* if the ref object is retrieved from a register bind that has
           no type info object associated, the type info object is retrieved
           at the first fetch call by pinning the ref and we affect the type
           info object handle to the define object */

        if ((def->col.typinf == NULL) && (ref != NULL) && (ref->typinf != NULL))
        {
            def->col.typinf = ref->typinf;
        }
    }

    OCI_RESULT(ref != NULL);

    return ref;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetRef2
 * --------------------------------------------------------------------------------------------- */

OCI_Ref * OCI_API OCI_GetRef2
(
    OCI_Resultset *rs,
    const mtext   *name
)
{
    return OCI_GetRef(rs, OCI_GetDefineIndex(rs, name));
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetStatement
 * --------------------------------------------------------------------------------------------- */

OCI_Statement * OCI_API OCI_GetStatement
(
    OCI_Resultset *rs,
    unsigned int   index
)
{
    OCI_Define *def   = OCI_GetDefine(rs, index);
    OCI_Statement *st = NULL;

    if ((OCI_NOT_NULL(def) == TRUE) && (def->col.type == OCI_CDT_CURSOR))
    {
        st = OCI_StatementInit(rs->stmt->con,
                               (OCI_Statement **) &def->obj,
                               (OCIStmt *) OCI_DefineGetData(def), def);
    }

    OCI_RESULT(st != NULL);

    return st;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetStatement2
 * --------------------------------------------------------------------------------------------- */

OCI_Statement * OCI_API OCI_GetStatement2
(
    OCI_Resultset *rs,
    const mtext   *name
)
{
    return OCI_GetStatement(rs, OCI_GetDefineIndex(rs, name));
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetLob
 * --------------------------------------------------------------------------------------------- */

OCI_Lob * OCI_API OCI_GetLob
(
    OCI_Resultset *rs,
    unsigned int   index
)
{
    OCI_Define *def = OCI_GetDefine(rs, index);
    OCI_Lob * lob   = NULL;

    if ((OCI_NOT_NULL(def) == TRUE) && (def->col.type == OCI_CDT_LOB))
    {
        lob =  OCI_LobInit(rs->stmt->con,(OCI_Lob **) &def->obj,
                           (OCILobLocator *) OCI_DefineGetData(def),
                           def->col.subtype);
    }

    OCI_RESULT(lob != NULL);

    return lob;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetLob2
 * --------------------------------------------------------------------------------------------- */

OCI_Lob * OCI_API OCI_GetLob2
(
    OCI_Resultset *rs,
    const mtext   *name
)
{
    return OCI_GetLob(rs, OCI_GetDefineIndex(rs, name));
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetFile
 * --------------------------------------------------------------------------------------------- */

OCI_File * OCI_API OCI_GetFile
(
    OCI_Resultset *rs,
    unsigned int   index
)
{
    OCI_Define *def = OCI_GetDefine(rs, index);
    OCI_File *file  = NULL;

    if ((OCI_NOT_NULL(def) == TRUE) && (def->col.type == OCI_CDT_FILE))
    {
        file = OCI_FileInit(rs->stmt->con,(OCI_File **) &def->obj,
                            (OCILobLocator *) OCI_DefineGetData(def),
                            def->col.subtype);
    }

    OCI_RESULT(file != NULL);

    return file;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetFile2
 * --------------------------------------------------------------------------------------------- */

OCI_File * OCI_API OCI_GetFile2
(
    OCI_Resultset *rs,
    const mtext   *name
)
{
    return OCI_GetFile(rs, OCI_GetDefineIndex(rs, name));
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetLong
 * --------------------------------------------------------------------------------------------- */

OCI_Long * OCI_API OCI_GetLong
(
    OCI_Resultset *rs,
    unsigned int   index
)
{
    OCI_Define *def = OCI_GetDefine(rs, index);
    OCI_Long *lg    = NULL;

    if ((OCI_NOT_NULL(def) == TRUE) && (def->col.type == OCI_CDT_LONG))
    {
        lg = (OCI_Long *) OCI_DefineGetData(def);
    }

    OCI_RESULT(lg != NULL);

    return lg;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetLong2
 * --------------------------------------------------------------------------------------------- */

OCI_Long * OCI_API OCI_GetLong2
(
    OCI_Resultset *rs,
    const mtext   *name
)
{
    return OCI_GetLong(rs, OCI_GetDefineIndex(rs, name));
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_IsNull
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_IsNull
(
    OCI_Resultset *rs,
    unsigned int   index
)
{
    OCI_Define *def = OCI_GetDefine(rs, index);

    OCI_RESULT(def != NULL);

    return (OCI_NOT_NULL(def) == FALSE);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_IsNull2
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_IsNull2
(
    OCI_Resultset *rs,
    const mtext   *name
)
{
    return OCI_IsNull(rs, OCI_GetDefineIndex(rs, name));
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ResultsetGetStatment
 * --------------------------------------------------------------------------------------------- */

OCI_Statement * OCI_API OCI_ResultsetGetStatement
(
    OCI_Resultset *rs
)
{
    OCI_CHECK_PTR(OCI_IPC_RESULTSET, rs, FALSE);

    OCI_RESULT(TRUE);

    return rs->stmt;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetDataLength
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_GetDataLength
(
    OCI_Resultset *rs,
    unsigned int   index
)
{
    OCI_Define *def     = OCI_GetDefine(rs, index);
    unsigned int length = 0;
    boolean res         = FALSE;

    if ((def != NULL) && (rs->row_cur > 0))
    {
        length = (unsigned int) ((ub2 *) def->buf.lens)[rs->row_cur-1];
        res    = TRUE;
    }

    OCI_RESULT(res);

    return length;
}

