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
 *                             PRIVATE VARIABLES
 * ********************************************************************************************* */

static unsigned int SeekModeValues[] = { OCI_SFD_ABSOLUTE, OCI_SFD_RELATIVE };

/* ********************************************************************************************* *
 *                             PRIVATE FUNCTIONS
 * ********************************************************************************************* */

#define OCI_MATCHING_TYPE(def, type) \
    (def && OCI_DefineIsDataNotNull(def) && (type == def->col.datatype))


#define OCI_GET_BY_NAME(rs, name, func, type, res)                                              \
    int index = -1;                                                                             \
    OCI_CALL_ENTER(type, res)                                                                   \
    OCI_CALL_CHECK_PTR(OCI_IPC_RESULTSET, rs)                                                   \
    OCI_CALL_CHECK_PTR(OCI_IPC_STRING, name)                                                    \
    OCI_CALL_CONTEXT_SET_FROM_STMT(rs->stmt)                                                    \
    OCI_STATUS = FALSE;                                                                         \
    index = OCI_GetDefineIndex(rs, name);                                                       \
    if (index > 0)                                                                              \
    {                                                                                           \
        if (!ctx->call_err) ctx->call_err = OCI_ErrorGet(FALSE);                                \
        OCI_RETVAL = func(rs, (unsigned int) index);                                            \
        OCI_STATUS = !ctx->call_err->raise;                                                     \
    }                                                                                           \
    OCI_CALL_EXIT()


#define OCI_GET_NUMBER(rs, index, num_type, type, res)                                          \
    OCI_CALL_ENTER(type, res)                                                                   \
    OCI_CALL_CHECK_PTR(OCI_IPC_RESULTSET, rs)                                                   \
    OCI_CALL_CHECK_BOUND(rs->stmt->con, index, 1, rs->nb_defs)                                  \
    OCI_CALL_CONTEXT_SET_FROM_STMT(rs->stmt)                                                    \
    OCI_STATUS = OCI_DefineGetNumber(rs, index, &OCI_RETVAL, num_type, sizeof(OCI_RETVAL));     \
    OCI_CALL_EXIT()


#define OCI_GET_HANDLE(rs, index, type, res, lib_type, func)                                    \
    OCI_Define *def = NULL;                                                                     \
    OCI_CALL_ENTER(type, res)                                                                   \
    OCI_CALL_CHECK_PTR(OCI_IPC_RESULTSET, rs)                                                   \
    OCI_CALL_CHECK_BOUND(rs->stmt->con, index, 1, rs->nb_defs)                                  \
    OCI_CALL_CONTEXT_SET_FROM_STMT(rs->stmt)                                                    \
    OCI_STATUS = FALSE;                                                                         \
    def = OCI_GetDefine(rs, index);                                                             \
    if (OCI_MATCHING_TYPE(def, lib_type))                                                       \
    {                                                                                           \
        if (!ctx->call_err) ctx->call_err = OCI_ErrorGet(FALSE);                                \
        def->obj = OCI_RETVAL = func;                                                                      \
        OCI_STATUS = (NULL != OCI_RETVAL);                                                      \
    }                                                                                           \
    OCI_CALL_EXIT()

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

    OCI_CALL_DECLARE_CONTEXT(TRUE)
        
    OCI_CHECK(stmt == NULL, NULL)

    OCI_CALL_CONTEXT_SET_FROM_STMT(stmt)

    /* allocate resultset structure */

    OCI_ALLOCATE_DATA(OCI_IPC_RESULTSET, rs, 1)

    /* set attributes */

    if (OCI_STATUS)
    {
        ub4     nb  = 0;
        ub4     i;

        rs->stmt         = stmt;
        rs->bof          = TRUE;
        rs->eof          = FALSE;
        rs->fetch_size   = size;
        rs->fetch_status = OCI_SUCCESS;
        rs->row_count    = 0;
        rs->row_cur      = 0;
        rs->row_abs      = 0;

        /* is the resultset created from a SQL select statement ? */

        if (OCI_CST_SELECT == stmt->type)
        {
            OCI_GET_ATTRIB(OCI_HTYPE_STMT, OCI_ATTR_PARAM_COUNT, stmt->stmt, &nb, NULL)
        }
        else
        {
            nb = stmt->nb_rbinds;
        }

        /* allocate columns array */

        OCI_ALLOCATE_DATA(OCI_IPC_DEFINE, rs->defs, nb)

        /* describe select list */

        if (OCI_STATUS && (OCI_CST_SELECT == stmt->type))
        {
            /* Compute columns information */

            for (i = 0; (i < nb) && OCI_STATUS; i++)
            {
                OCI_Define *def = &rs->defs[i];

                def->buf.count   = size;
                def->buf.sizelen = sizeof(ub2);

                def->rs = rs;

                rs->nb_defs++;

                /* get column description */

                OCI_STATUS = OCI_ColumnDescribe(&def->col, rs->stmt->con,
                                                 rs->stmt, rs->stmt->stmt,
                                                 i + 1, OCI_DESC_RESULTSET);

                /* mapping to OCILIB internal types */

                OCI_STATUS = OCI_STATUS && OCI_ColumnMap(&def->col, rs->stmt);

             #if defined(OCI_STMT_SCROLLABLE_READONLY)

                if (OCI_SFM_SCROLLABLE == rs->stmt->exec_mode)
                {
                    if (OCI_CDT_CURSOR == def->col.datatype)
                    {
                        rs->fetch_size = 1;
                    }
                }

            #endif

            }

            /* allocation internal buffers if needed */

            if (OCI_STATUS && !(rs->stmt->exec_mode & OCI_DESCRIBE_ONLY) && !(rs->stmt->exec_mode & OCI_PARSE_ONLY))
            {
                for (i = 0; (i < nb) && OCI_STATUS; i++)
                {
                    OCI_Define *def = &rs->defs[i];

                    /* allocation of internal buffers for resultset content and
                       register OCILIB result buffers to OCI */

                    OCI_STATUS = OCI_DefineAlloc(def) && OCI_DefineDef(def, i + 1);
                }
            }
        }
        else if (rs->defs)
        {
            /* get info from register binds */

            for (i=0; i < stmt->nb_rbinds; i++)
            {
                OCI_Bind   *bnd  = stmt->rbinds[i];
                OCI_Define *def  = &rs->defs[bnd->dynpos];

                def->buf.count   = size;
                def->buf.sizelen = sizeof(ub4);

                def->rs = rs;

                rs->nb_defs++;

                /* columns info */

                def->col.sqlcode    = bnd->code;
                def->col.libcode    = bnd->code;
                def->col.name       = ostrdup(bnd->name);
                def->col.size       = (ub2) bnd->size;
                def->col.datatype   = bnd->type;
                def->col.subtype    = bnd->subtype;

                switch (def->col.datatype)
                {
                    case OCI_CDT_LOB:
                    {
                        /* check national attribute for national character large objects */
                        if (OCI_NCLOB == def->col.subtype)
                        {
                            def->col.csfrm = SQLCS_NCHAR;
                        }
                        break;
                    }
                    case OCI_CDT_TEXT:
                    {
                        /* adjust column size from bind attributes */
                        def->col.size = (ub2) (def->col.size / ((ub2) sizeof(otext)) - 1);
                        break;
                    }
                    case OCI_CDT_NUMERIC:
                    {
                        /* for integer types, set the buffer size here in order to
                           retrieve later the integer type (short, integer, big_int)
                           depending on the integer size */
                        def->col.bufsize = def->col.size;
                        break;
                    }
                    case OCI_CDT_DATETIME:
                    {
                        /* preset buffer size here to let OCI_ColumnMap() know we don't
                           want the column to mapped to SQLT_ODT */
                        if (SQLT_DAT == def->col.sqlcode)
                        {
                            def->col.bufsize = def->col.size;
                        }
                        break;
                    }
                }

                /* map column and allocation of internal buffers for resultset content **/

                OCI_STATUS = OCI_ColumnMap(&def->col, rs->stmt) && OCI_DefineAlloc(def);
            }
        }
    }

    if (!OCI_STATUS && rs)
    {
        OCI_ResultsetFree(rs);
        rs = NULL;
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
    ub4 type, iter, dx;
    ub1 in_out, piece;
    void *handle;
    ub4 i, j;

    OCI_CALL_DECLARE_CONTEXT(TRUE)

    OCI_CHECK(NULL == rs, FALSE)

    OCI_CALL_CONTEXT_SET_FROM_STMT(rs->stmt);

    /* reset long objects */

    for (i = 0; i < rs->nb_defs; i++)
    {
        OCI_Define *def = &rs->defs[i];

        if (OCI_CDT_LONG == def->col.datatype)
        {
            for (j = 0; j < def->buf.count; j++)
            {
                def->buf.data[j] = OCI_LongInit(rs->stmt, (OCI_Long *)def->buf.data[j], def, def->col.subtype);
            }
        }
    }

    /* dynamic fetch */

    while (OCI_STATUS && (OCI_NEED_DATA == rs->fetch_status))
    {
        piece  = OCI_NEXT_PIECE;
        iter   = 0;
        handle = NULL;

        /* get piece information */

        OCI_EXEC(OCIStmtGetPieceInfo(rs->stmt->stmt, rs->stmt->con->err, &handle,  &type, &in_out, &iter, &dx, &piece))

        /* search for the given column */

        for (i = 0; OCI_STATUS && (i < rs->nb_defs); i++)
        {
            OCI_Define *def = &(rs->defs[i]);

            if ((OCI_CDT_LONG == def->col.datatype) && (def->buf.handle == handle))
            {
                /* get the long object for the given internal row */

                OCI_Long *lg = (OCI_Long *) def->buf.data[iter];

                unsigned int nb_alloc      = 0;
                unsigned int trailing_size = 0;
                unsigned int char_fact     = sizeof(otext) / sizeof(dbtext);

                /* setup up piece size */

                ub4 bufsize = rs->stmt->long_size;

                if (char_fact == 0)
                {
                    char_fact = 1;
                }

                if (OCI_CLONG == lg->type)
                {
                    bufsize += (ub4) sizeof(dbtext);
                }

                nb_alloc = (lg->maxsize / bufsize);

                if (OCI_CLONG == lg->type)
                {
                    trailing_size = sizeof(dbtext) * nb_alloc;
                }

                /* check buffer */

                if (!lg->buffer)
                {
                    lg->maxsize = bufsize;

                    OCI_ALLOCATE_DATA(OCI_IPC_LONG_BUFFER, lg->buffer, lg->maxsize)
                }
                else if ((lg->size*char_fact) >= (lg->maxsize - trailing_size))
                {
                    lg->maxsize = (lg->size + trailing_size + bufsize) * char_fact;

                    lg->buffer = (ub1 *) OCI_MemRealloc(lg->buffer, (size_t) OCI_IPC_LONG_BUFFER, (size_t) lg->maxsize, 1);
                }

                /* update piece info */

                if (OCI_STATUS)
                {
                    lg->piecesize = bufsize;

                    if (OCI_CLONG == lg->type)
                    {
                        lg->piecesize /= sizeof(otext);
                        lg->piecesize *= sizeof(dbtext);
                        lg->piecesize -= (ub4) sizeof(dbtext);
                    }

                    OCI_EXEC
                    (
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

        if (OCILib.use_scrollable_cursors)
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

        if (OCI_ERROR == rs->fetch_status)
        {
            OCI_ExceptionOCI(rs->stmt->con->err, rs->stmt->con, rs->stmt, FALSE);
            OCI_STATUS = FALSE;
        }
        else if (OCI_SUCCESS_WITH_INFO == rs->fetch_status)
        {
            OCI_ExceptionOCI(rs->stmt->con->err, rs->stmt->con, rs->stmt, TRUE);
            OCI_STATUS = TRUE;
        }
        else
        {
            /* search for the given column */

            for (i = 0; i < rs->nb_defs; i++)
            {
                OCI_Define *def = &rs->defs[i];

                if ((OCI_CDT_LONG == def->col.datatype) && (def->buf.handle == handle))
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

        if ((OCI_CDT_LONG == def->col.datatype) && (OCI_CLONG == def->col.subtype))
        {
            for (j = 0; j < def->buf.count; j++)
            {
                OCI_Long *lg = (OCI_Long *) def->buf.data[j];

                if (lg->buffer)
                {
                    int len  = (int) ( lg->size / sizeof(dbtext) );

                    ((dbtext *)lg->buffer)[len] = 0;

                    if (OCILib.use_wide_char_conv)
                    {
                        OCI_StringUTF16ToUTF32(lg->buffer, lg->buffer, len);
                    }
                }
            }
        }
    }

    return OCI_STATUS;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_FetchData
 * --------------------------------------------------------------------------------------------- */

boolean OCI_FetchData
(
    OCI_Resultset *rs,
    int            mode,
    int            offset,
    boolean       *success
)
{
    OCI_CALL_DECLARE_CONTEXT(TRUE)

    OCI_CHECK(NULL == rs, FALSE)

    OCI_CALL_CONTEXT_SET_FROM_STMT(rs->stmt);

    /* let's initialize the success flag to FALSE until the process completes */

    *success = FALSE;

    OCI_ClearFetchedObjectInstances(rs);

    /* internal fetch */

 #if defined(OCI_STMT_SCROLLABLE_READONLY)

    if (OCILib.use_scrollable_cursors)
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


    if (OCI_ERROR == rs->fetch_status)
    {
        /* failure */
        OCI_ExceptionOCI(rs->stmt->con->err, rs->stmt->con, rs->stmt, FALSE);
        OCI_STATUS = FALSE;
    }
    else if (OCI_SUCCESS_WITH_INFO == rs->fetch_status)
    {
        OCI_ExceptionOCI(rs->stmt->con->err, rs->stmt->con, rs->stmt, TRUE);
        OCI_STATUS = TRUE;
    }
    else if (OCI_NEED_DATA == rs->fetch_status)
    {
        /* need to do a piecewise fetch */
        OCI_STATUS = OCI_FetchPieces(rs);
    }

    /* check string buffer for Unicode builds that need buffer expansion */

    if (OCILib.use_wide_char_conv)
    {
        OCI_ResultsetExpandStrings(rs);
    }

    /* check for success */

    OCI_STATUS = (OCI_STATUS && ((OCI_SUCCESS           == rs->fetch_status) ||
                                 (OCI_NO_DATA           == rs->fetch_status) ||
                                 (OCI_SUCCESS_WITH_INFO == rs->fetch_status)));

    /* update internal fetch status and variables */

    if (OCI_STATUS)
    {
        ub4 row_count   = 0;
        ub4 row_fetched = 0;

    #if defined(OCI_STMT_SCROLLABLE_READONLY)

        if (OCI_SFM_SCROLLABLE == rs->stmt->exec_mode)
        {
            OCI_GET_ATTRIB(OCI_HTYPE_STMT, OCI_ATTR_CURRENT_POSITION, rs->stmt->stmt, &row_count, NULL)
            OCI_GET_ATTRIB(OCI_HTYPE_STMT, OCI_ATTR_ROWS_FETCHED, rs->stmt->stmt, &row_fetched, NULL)
        }
        else

    #endif

        {
            row_count   = OCI_GetAffectedRows(rs->stmt);
            row_fetched = row_count - rs->row_count;
        }

        if (rs->row_count < row_count)
        {
            rs->row_count = row_count;
        }

        if (row_fetched > 0)
        {
            rs->row_fetched = row_fetched;
        }

        /* so far, no OCI error occurred, let's clear the error flag */

        *success = TRUE;

        /* check if internal fetch was successful */

        if ((OCI_NO_DATA == rs->fetch_status) && (row_fetched == 0))
        {
            if ((OCI_SFD_NEXT == mode) || (offset > 0))
            {
                rs->eof = TRUE;
            }

            if (offset < 0)
            {
                rs->bof = TRUE;
            }

            OCI_STATUS = FALSE;
        }
    }

    return OCI_STATUS;
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
            if (((offset > 0) && (rs->eof)) ||
                ((offset < 0) && (rs->bof)) ||
                (offset == 0))
            {
                res = FALSE;
            }
            else
            {
                int offset_save = offset;

                offset      = offset - rs->row_fetched + rs->row_cur;
                rs->row_cur = 1;

                res = OCI_FetchData(rs, mode, offset, err);

                if (res)
                {
                    rs->row_abs += offset_save;
                }
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

                if (res)
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
            break;
        }
    }

    return res;
}

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

    OCI_CHECK(NULL == rs, FALSE)

    for (i = 0; i < rs->nb_defs; i++)
    {
        OCI_Define *def = &rs->defs[i];

        if (OCI_CDT_TEXT == def->col.datatype)
        {
            for (j = (int) (def->buf.count-1); j >= 0; j--)
            {
                ub1 * tmpbuf = ((ub1*) def->buf.data) + (def->col.bufsize * j);

                OCI_StringUTF16ToUTF32(tmpbuf, tmpbuf, (def->col.bufsize / sizeof(otext) ) -1);
            }
        }
    }

    return TRUE;
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

    OCI_CHECK(NULL == rs, FALSE)

    for (i = 0; i < rs->nb_defs; i++)
    {
        OCI_Define *def = &(rs->defs[i]);

        /* free buffer objects */

        if (def->obj)
        {
            /* handy cast to set object state flag */

            ((OCI_Datatype *)def->obj)->hstate = OCI_OBJECT_FETCHED_DIRTY;

            if (OCI_CDT_CURSOR == def->col.datatype)
            {
                OCI_StatementClose((OCI_Statement *)def->obj);
                OCI_FREE(def->obj)
            }
            else
            {
                OCI_FreeObjectFromType(def->obj, def->col.datatype);
            }

            def->obj = NULL;
        }

        /* free OCI handles */

        if (OCI_UNKNOWN != def->col.handletype)
        {
            if (OCI_CDT_CURSOR == def->col.datatype)
            {
                if (def->buf.data)
                {
                    for (j = 0; j < def->buf.count; j++)
                    {
                        OCI_HandleFree((dvoid *)def->buf.data[j], (ub4)def->col.handletype);
                    }
                }
            }
            else
            {
                OCI_DescriptorArrayFree((dvoid *)def->buf.data,  (ub4)def->col.handletype, (ub4)def->buf.count);
            }
        }

        /* free OCI long buffers */

        if ((OCI_CDT_LONG) == def->col.datatype && def->buf.data)
        {
            for (j = 0; j < def->buf.count; j++)
            {
                if (def->buf.data[j])
                {
                    ((OCI_Datatype *)def->buf.data[j])->hstate = OCI_OBJECT_FETCHED_DIRTY;

                    OCI_LongFree((OCI_Long *)def->buf.data[j]);
                }
            }
        }

        /* free object instances from object cache */

        OCI_ClearFetchedObjectInstances(rs);

        /* free column pointers */

        OCI_FREE(def->col.name)

        /* free buffer pointers */

        OCI_FREE(def->buf.data)
        OCI_FREE(def->buf.inds)
        OCI_FREE(def->buf.obj_inds)
        OCI_FREE(def->buf.lens)
        OCI_FREE(def->buf.tmpbuf)
    }

    /* free column map */

    if (rs->map)
    {
        OCI_HashFree(rs->map);
    }

    /* free defines (column array) */

    OCI_FREE(rs->defs)

    OCI_FREE(rs)

    return res;
}

/* --------------------------------------------------------------------------------------------- *
* OCI_ClearFetchedObjectInstances
* --------------------------------------------------------------------------------------------- */

boolean OCI_ClearFetchedObjectInstances(OCI_Resultset *rs)
{
    boolean res = TRUE;
    ub4 i, j;

    OCI_CHECK(NULL == rs, FALSE)

    for (i = 0; i < rs->nb_defs; i++)
    {
        OCI_Define *def = &(rs->defs[i]);

        if (SQLT_NTY == def->col.sqlcode && def->buf.data)
        {
            for (j = 0; j < def->buf.count; j++)
            {
                if (def->buf.data[j] != NULL)
                {
                    OCIObjectFree(rs->stmt->con->env, rs->stmt->con->err, def->buf.data[j], OCI_DEFAULT);
                    def->buf.data[j] = NULL;
                }
            }
        }
    }

    return res;
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
    OCI_CALL_ENTER(OCI_Resultset*, NULL)
    OCI_CALL_CHECK_PTR(OCI_IPC_STATEMENT, stmt)
    OCI_CALL_CHECK_STMT_STATUS(stmt, OCI_STMT_DESCRIBED)
    OCI_CALL_CONTEXT_SET_FROM_STMT(stmt)

    /* if the sql statement does not return a result, we just return NULL and not
       throwing any exception
       statements that can return a resultset are "SELECT..." and "... RETURNING INTO..."
    */

    if ((OCI_CST_SELECT == stmt->type) || (stmt->nb_rbinds > 0) || (stmt->nb_stmt > 0))
    {
        /* if the resultset exists, let's use it */

        if (stmt->rsts && stmt->rsts[0])
        {
            OCI_RETVAL = stmt->rsts[0];
        }

        /* allocate resultset for select statements only */

        if (!OCI_RETVAL && (OCI_CST_SELECT == stmt->type))
        {
            /* allocate memory for one resultset handle */

            OCI_ALLOCATE_DATA(OCI_IPC_RESULTSET_ARRAY, stmt->rsts, 1)
           
            if (OCI_STATUS)
            {
                stmt->nb_rs  = 1;
                stmt->cur_rs = 0;

                /* create resultset object */

                OCI_RETVAL = stmt->rsts[0] = OCI_ResultsetCreate(stmt, stmt->fetch_size);
            }

        }

        OCI_STATUS = (NULL != OCI_RETVAL);
    }

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetNextResultset
 * --------------------------------------------------------------------------------------------- */

OCI_Resultset * OCI_API OCI_GetNextResultset
(
    OCI_Statement *stmt
)
{
    OCI_CALL_ENTER(OCI_Resultset*, NULL)
    OCI_CALL_CHECK_PTR(OCI_IPC_STATEMENT, stmt)
    OCI_CALL_CHECK_STMT_STATUS(stmt, OCI_STMT_DESCRIBED)
    OCI_CALL_CONTEXT_SET_FROM_STMT(stmt)

    if (stmt->cur_rs < (stmt->nb_rs-1))
    {
        OCI_RETVAL = stmt->rsts[++stmt->cur_rs];
    }

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_FetchPrev
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_FetchPrev
(
    OCI_Resultset *rs
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_RESULTSET, rs)
    OCI_CALL_CHECK_STMT_STATUS(rs->stmt, OCI_STMT_EXECUTED)
    OCI_CALL_CHECK_SCROLLABLE_CURSOR_ENABLED(rs->stmt->con)

#if OCI_VERSION_COMPILE >= OCI_9_0

    OCI_CALL_CHECK_SCROLLABLE_CURSOR_ACTIVATED(rs->stmt)
    OCI_CALL_CONTEXT_SET_FROM_STMT(rs->stmt)

    if (!rs->bof)
    {
        OCI_RETVAL = TRUE;

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
                {
                    offset = 1 - rs->row_abs;
                }
                else
                {
                    offset = 1 - (rs->fetch_size + rs->row_fetched);
                }

                OCI_RETVAL = OCI_FetchData(rs, OCI_SFD_RELATIVE, offset, &OCI_STATUS);

                if (OCI_RETVAL)
                {
                    if (rs->fetch_size > rs->row_abs)
                    {
                        rs->row_cur = rs->row_abs-1;
                    }
                    else
                    {
                        rs->row_cur = rs->fetch_size;
                    }

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

        OCI_RETVAL = OCI_RETVAL && !rs->bof;
    }

#endif

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_FetchNext
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_FetchNext
(
    OCI_Resultset *rs
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_RESULTSET, rs)
    OCI_CALL_CHECK_STMT_STATUS(rs->stmt, OCI_STMT_EXECUTED)
    OCI_CALL_CONTEXT_SET_FROM_STMT(rs->stmt)

    if (!rs->eof)
    {
        OCI_RETVAL = TRUE;

        if (rs->stmt->nb_rbinds == 0)
        {
            /* for regular resultsets */

            if (rs->row_cur == rs->row_fetched)
            {
                if (OCI_NO_DATA == rs->fetch_status)
                {
                    rs->eof = TRUE;
                }
                else
                {
                    OCI_RETVAL = OCI_FetchData(rs, OCI_SFD_NEXT, 0, &OCI_STATUS);

                    if (OCI_RETVAL)
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

                if (OCILib.use_wide_char_conv)
                {
                    OCI_ResultsetExpandStrings(rs);
                }
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

        OCI_RETVAL = OCI_RETVAL && !rs->eof;
    }

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_FetchFirst
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_FetchFirst
(
    OCI_Resultset *rs
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_RESULTSET, rs)
    OCI_CALL_CHECK_STMT_STATUS(rs->stmt, OCI_STMT_EXECUTED)
    OCI_CALL_CHECK_SCROLLABLE_CURSOR_ENABLED(rs->stmt->con)

#if OCI_VERSION_COMPILE >= OCI_9_0

    OCI_CALL_CHECK_SCROLLABLE_CURSOR_ACTIVATED(rs->stmt)
    OCI_CALL_CONTEXT_SET_FROM_STMT(rs->stmt)

    rs->bof = FALSE;
    rs->eof = FALSE;

    rs->row_abs = 1;
    rs->row_cur = 1;

    OCI_RETVAL = (OCI_FetchData(rs, OCI_SFD_FIRST, 0, &OCI_STATUS) && !rs->bof);

#endif

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_FetchLast
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_FetchLast
(
    OCI_Resultset *rs
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_RESULTSET, rs)
    OCI_CALL_CHECK_STMT_STATUS(rs->stmt, OCI_STMT_EXECUTED)
    OCI_CALL_CHECK_SCROLLABLE_CURSOR_ENABLED(rs->stmt->con)

#if OCI_VERSION_COMPILE >= OCI_9_0

    OCI_CALL_CHECK_SCROLLABLE_CURSOR_ACTIVATED(rs->stmt)
    OCI_CALL_CONTEXT_SET_FROM_STMT(rs->stmt)

    rs->bof = FALSE;
    rs->eof = FALSE;

    rs->row_abs = 0;
    rs->row_cur = 1;

    OCI_RETVAL = (OCI_FetchData(rs, OCI_SFD_LAST, 0, &OCI_STATUS) && !rs->eof);

    rs->row_abs = rs->row_count;

#endif

    OCI_CALL_EXIT()
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
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_RESULTSET, rs)
    OCI_CALL_CHECK_STMT_STATUS(rs->stmt, OCI_STMT_EXECUTED)
    OCI_CALL_CHECK_SCROLLABLE_CURSOR_ENABLED(rs->stmt->con)

#if OCI_VERSION_COMPILE >= OCI_9_0

    OCI_CALL_CHECK_SCROLLABLE_CURSOR_ACTIVATED(rs->stmt)
    OCI_CALL_CHECK_ENUM_VALUE(rs->stmt->con, rs->stmt, mode, SeekModeValues, OTEXT("Fetch Seek Mode"))
    OCI_CALL_CONTEXT_SET_FROM_STMT(rs->stmt)

    OCI_RETVAL = OCI_FetchCustom(rs, mode, offset, &OCI_STATUS);

#else

    OCI_NOT_USED(mode)
    OCI_NOT_USED(offset)

#endif

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetRowCount
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_GetRowCount
(
    OCI_Resultset *rs
)
{
    OCI_GET_PROP(unsigned int, 0, OCI_IPC_RESULTSET, rs, row_count, rs->stmt->con, rs->stmt, rs->stmt->con->err)
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetRowCount
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_GetCurrentRow
(
    OCI_Resultset *rs
)
{
    OCI_GET_PROP(unsigned int, 0, OCI_IPC_RESULTSET, rs, row_abs, rs->stmt->con, rs->stmt, rs->stmt->con->err)
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetColumnCount
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_GetColumnCount
(
    OCI_Resultset *rs
)
{
    OCI_GET_PROP(unsigned int, 0, OCI_IPC_RESULTSET, rs, nb_defs, rs->stmt->con, rs->stmt, rs->stmt->con->err)
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
    OCI_CALL_ENTER(OCI_Column*, NULL)
    OCI_CALL_CHECK_PTR(OCI_IPC_RESULTSET, rs)
    OCI_CALL_CHECK_BOUND(rs->stmt->con, index, 1, rs->nb_defs)
    OCI_CALL_CONTEXT_SET_FROM_STMT(rs->stmt)

    OCI_RETVAL = &rs->defs[index - 1].col;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetColumn2
 * --------------------------------------------------------------------------------------------- */

OCI_Column * OCI_API OCI_GetColumn2
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    int index = -1;

    OCI_CALL_ENTER(OCI_Column*, NULL)
    OCI_CALL_CHECK_PTR(OCI_IPC_RESULTSET, rs)
    OCI_CALL_CHECK_PTR(OCI_IPC_STRING, name)
    OCI_CALL_CONTEXT_SET_FROM_STMT(rs->stmt)

    OCI_STATUS = FALSE;

    index = OCI_GetDefineIndex(rs, name);

    if (index >= 0)
    {
        OCI_RETVAL = &rs->defs[index - 1].col;
        OCI_STATUS = TRUE;
    }

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetColumnIndex
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_GetColumnIndex
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    int index = -1;

    OCI_CALL_ENTER(unsigned int, 0)
    OCI_CALL_CHECK_PTR(OCI_IPC_RESULTSET, rs)
    OCI_CALL_CHECK_PTR(OCI_IPC_STRING, name)
    OCI_CALL_CONTEXT_SET_FROM_STMT(rs->stmt)

    OCI_STATUS = FALSE;

    index = OCI_GetDefineIndex(rs, name);

    if (index >= 0)
    {
        OCI_RETVAL = (unsigned int) index;
        OCI_STATUS = TRUE;
    }

    OCI_CALL_EXIT()
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
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_RESULTSET, rs)
    OCI_CALL_CHECK_BOUND(rs->stmt->con, index,  1,  rs->nb_defs)
    OCI_CALL_CHECK_COMPAT(rs->stmt->con, OCI_CDT_NUMERIC == rs->defs[index - 1].col.datatype);
    OCI_CALL_CONTEXT_SET_FROM_STMT(rs->stmt)

    rs->defs[index-1].col.subtype = type;

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_SetStructNumericType2
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_SetStructNumericType2
(
    OCI_Resultset *rs,
    const otext   *name,
    unsigned int   type
)
{
    int index = -1;

    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_RESULTSET, rs)
    OCI_CALL_CHECK_PTR(OCI_IPC_STRING, name)
    OCI_CALL_CONTEXT_SET_FROM_STMT(rs->stmt)

    OCI_STATUS = FALSE;

    index = OCI_GetDefineIndex(rs, name);

    if (index >= 0)
    {
        OCI_RETVAL = OCI_STATUS = OCI_SetStructNumericType(rs, (unsigned int)index, type);
    }

    OCI_CALL_EXIT()
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

    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_RESULTSET, rs)
    OCI_CALL_CHECK_PTR(OCI_IPC_VOID, row_struct)
    OCI_CALL_CONTEXT_SET_FROM_STMT(rs->stmt)

    ptr  = (char    *) row_struct;
    inds = (boolean *) row_struct_ind;

    if (ptr)
    {
        size_t size1 = 0;
        size_t size2 = 0;
        size_t align = 0;
        ub2 i;

        size_t size = 0;

        for (i = 1; i <= rs->nb_defs; i++)
        {
            OCI_Define *def = &rs->defs[i - 1];

            boolean is_not_null = OCI_DefineIsDataNotNull(def);

            if (i == 1)
            {
                OCI_ColumnGetAttrInfo(&def->col, rs->nb_defs, i - 1, &size1, &align);
            }
            else
            {
                size1 = size2;
            }         

            OCI_ColumnGetAttrInfo(&rs->defs[i].col, rs->nb_defs, i, &size2, &align);

            size += size1;

            size = ROUNDUP(size, align);
                
            memset(ptr, 0, size1);

            if (is_not_null)
            {
                switch (def->col.datatype)
                {
                    case OCI_CDT_NUMERIC:
                    {
                        OCI_DefineGetNumber(rs, i, ptr, def->col.subtype, (uword)size1);

                        break;
                    }
                    case OCI_CDT_TEXT:
                    {
                        *((otext **) ptr) =  (otext * ) OCI_GetString(rs, i);
                        break;
                    }
                    case OCI_CDT_RAW:
                    {
                        *((void **) ptr) = OCI_DefineGetData(def);
                        break;
                    }
                    case OCI_CDT_LONG:
                    {
                        *((OCI_Long **) ptr) = OCI_GetLong(rs, i);
                        break;
                    }
                    case OCI_CDT_DATETIME:
                    {
                        *((OCI_Date **) ptr) = OCI_GetDate(rs, i);
                        break;
                    }
                    case OCI_CDT_CURSOR:
                    {
                        *((OCI_Statement **) ptr) = OCI_GetStatement(rs, i);
                        break;
                    }
                    case OCI_CDT_LOB:
                    {
                        *((OCI_Lob **) ptr) = OCI_GetLob(rs, i);
                        break;
                    }
                    case OCI_CDT_FILE:
                    {
                        *((OCI_File **) ptr) = OCI_GetFile(rs, i);
                        break;
                    }
                    case OCI_CDT_TIMESTAMP:
                    {
                        *((OCI_Timestamp **) ptr) = OCI_GetTimestamp(rs, i);

                        break;
                    }
                    case OCI_CDT_INTERVAL:
                    {
                        *((OCI_Interval **) ptr) = OCI_GetInterval(rs, i);
                        break;
                    }
                    case OCI_CDT_OBJECT:
                    {
                        *((OCI_Object **) ptr) = OCI_GetObject(rs, i);
                        break;
                    }
                    case OCI_CDT_COLLECTION:
                    {
                        *((OCI_Coll **) ptr) = OCI_GetColl(rs, i);
                        break;
                    }
                    case OCI_CDT_REF:
                    {
                        *((OCI_Ref **) ptr) = OCI_GetRef(rs, i);
                        break;
                    }
                }
            }

            ptr = ((char *) row_struct) + size;

            if (inds)
            {
                *inds = is_not_null;

                inds++;
            }
        }
    }

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
* OCI_GetNumber
* --------------------------------------------------------------------------------------------- */

OCI_Number * OCI_API OCI_GetNumber
(
    OCI_Resultset *rs,
    unsigned int   index
)
{
    OCI_GET_HANDLE
    (
        rs, index, OCI_Number *, NULL, OCI_CDT_NUMERIC,

        OCI_NumberInit(rs->stmt->con, (OCI_Number *) def->obj, (OCINumber *) OCI_DefineGetData(def))
    )
}

/* --------------------------------------------------------------------------------------------- * 
 * OCI_GetNumber2
 * --------------------------------------------------------------------------------------------- */

OCI_Number * OCI_API OCI_GetNumber2
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    OCI_GET_BY_NAME(rs, name, OCI_GetNumber, OCI_Number *, NULL)
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
    OCI_GET_NUMBER(rs, index, OCI_NUM_SHORT, short, 0)
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetShort2
 * --------------------------------------------------------------------------------------------- */

short OCI_API OCI_GetShort2
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    OCI_GET_BY_NAME(rs, name, OCI_GetShort, short, 0)
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
    OCI_GET_NUMBER(rs, index, OCI_NUM_USHORT, unsigned short, 0)
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetUnsignedShort2
 * --------------------------------------------------------------------------------------------- */

unsigned short OCI_API OCI_GetUnsignedShort2
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    OCI_GET_BY_NAME(rs, name, OCI_GetUnsignedShort, unsigned short, 0)
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
    OCI_GET_NUMBER(rs, index, OCI_NUM_INT, int, 0)
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetInt2
 * --------------------------------------------------------------------------------------------- */

int OCI_API OCI_GetInt2
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    OCI_GET_BY_NAME(rs, name, OCI_GetInt, int, 0)
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
    OCI_GET_NUMBER(rs, index, OCI_NUM_UINT, unsigned int, 0)
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetUnsignedInt2
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_GetUnsignedInt2
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    OCI_GET_BY_NAME(rs, name, OCI_GetUnsignedInt, unsigned int, 0)
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
    OCI_GET_NUMBER(rs, index, OCI_NUM_BIGINT, big_int, 0)
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetBigInt2
 * --------------------------------------------------------------------------------------------- */

big_int OCI_API OCI_GetBigInt2
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    OCI_GET_BY_NAME(rs, name, OCI_GetBigInt, big_int, 0)
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
    OCI_GET_NUMBER(rs, index, OCI_NUM_BIGUINT, big_uint, 0)
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetUnsignedInt2
 * --------------------------------------------------------------------------------------------- */

big_uint OCI_API OCI_GetUnsignedBigInt2
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    OCI_GET_BY_NAME(rs, name, OCI_GetUnsignedBigInt, big_uint, 0)
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetString
 * --------------------------------------------------------------------------------------------- */

const otext * OCI_API OCI_GetString
(
    OCI_Resultset *rs,
    unsigned int   index
)
{
    OCI_Define *def = NULL;

    OCI_CALL_ENTER(otext *, NULL)
    OCI_CALL_CHECK_PTR(OCI_IPC_RESULTSET, rs)
    OCI_CALL_CHECK_BOUND(rs->stmt->con, index, 1, rs->nb_defs)
    OCI_CALL_CONTEXT_SET_FROM_STMT(rs->stmt)

    OCI_STATUS = FALSE;

    def = OCI_GetDefine(rs, index);

    if (def && OCI_DefineIsDataNotNull(def))
    {
        void *data = NULL;

        OCI_STATUS = TRUE;

        if (OCI_CDT_TEXT == def->col.datatype)
        {
            OCI_RETVAL = (otext *)OCI_DefineGetData(def);

            /* for long mapped to string, the zero terminal character is not
                always added by Oracle ? or OCILIB issue ? Anyway we check the
                length returned by Oracle and set it properly */

            if (OCI_CLONG == def->col.subtype)
            {
                ub2* lens = (ub2 *)def->buf.lens;

                OCI_RETVAL[lens[rs->row_cur - 1]] = 0;
            }
        }
        else if ((OCI_CDT_LONG == def->col.datatype) && (OCI_CLONG == def->col.subtype))
        {
            OCI_Long *lg = OCI_GetLong(rs, index);

            if (lg)
            {
                OCI_RETVAL = (otext *)OCI_LongGetBuffer(lg);
            }
        }
        else
        {
            unsigned int bufsize = OCI_SIZE_BUFFER;
            unsigned int data_size = 0;

            switch (def->col.datatype)
            {
            case OCI_CDT_NUMERIC:
            {
                data = OCI_DefineGetData(def);
                break;
            }
            case OCI_CDT_DATETIME:
            {
                data = OCI_GetDate(rs, index);
                break;
            }
            case OCI_CDT_TIMESTAMP:
            {
                data = OCI_GetTimestamp(rs, index);
                break;
            }
            case OCI_CDT_INTERVAL:
            {
                data = OCI_GetInterval(rs, index);
                break;
            }
            case OCI_CDT_RAW:
            {
                data = OCI_DefineGetData(def);
                data_size = ((ub2*)def->buf.lens)[def->rs->row_cur - 1];
                break;
            }
            case OCI_CDT_REF:
            {
                data = OCI_GetRef(rs, index);
                break;
            }
            case OCI_CDT_LONG:
            {
                OCI_Long *lg = OCI_GetLong(rs, index);

                if (lg)
                {
                    /* here we have binary long, it will be output in hexadecimal */
                    bufsize = OCI_LongGetSize(lg) * 2;
                }

                data = lg;
                break;
            }

            case OCI_CDT_LOB:
            {
                OCI_Lob *lob = OCI_GetLob(rs, index);

                if (lob)
                {
                    bufsize = (unsigned int)OCI_LobGetLength(lob);;

                    if (OCI_BLONG == def->col.subtype)
                    {
                        /* here we have binary blob, it will be output in hexadecimal */
                        bufsize *= 2;
                    }
                }

                data = lob;
                break;
            }
            case OCI_CDT_FILE:
            {
                /* directory / name will be output */
                data = OCI_GetFile(rs, index);
                break;
            }
            case OCI_CDT_OBJECT:
            {
                OCI_Object *obj = OCI_GetObject(rs, index);

                if (obj)
                {
                    OCI_STATUS = OCI_ObjectToText(obj, &bufsize, NULL);
                }

                data = obj;
                break;
            }
            case OCI_CDT_COLLECTION:
            {
                OCI_Coll *coll = OCI_GetColl(rs, index);

                if (coll)
                {
                    OCI_STATUS = OCI_CollToText(coll, &bufsize, NULL);
                }

                data = coll;
                break;
            }
            default:
            {
                break;
            }
            }

            if (OCI_STATUS)
            {
                OCI_STATUS = OCI_StringRequestBuffer(&def->buf.tmpbuf, &def->buf.tmpsize, bufsize);

                if (OCI_STATUS)
                {
                    bufsize = OCI_StringGetFromType(rs->stmt->con, &def->col, data, data_size, def->buf.tmpbuf, FALSE);

                    if (bufsize > 0)
                    {
                        OCI_RETVAL = def->buf.tmpbuf;
                    }
                }
            }
        }
    }

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetString2
 * --------------------------------------------------------------------------------------------- */

const otext * OCI_API OCI_GetString2
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    OCI_GET_BY_NAME(rs, name, OCI_GetString, const otext *, NULL)
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
    OCI_Define *def = NULL;

    OCI_CALL_ENTER(unsigned int, 0)
    OCI_CALL_CHECK_PTR(OCI_IPC_RESULTSET, rs)
    OCI_CALL_CHECK_PTR(OCI_IPC_VOID, buffer);
    OCI_CALL_CHECK_BOUND(rs->stmt->con, index, 1, rs->nb_defs)
    OCI_CALL_CONTEXT_SET_FROM_STMT(rs->stmt)

    OCI_STATUS = TRUE;

    def = OCI_GetDefine(rs, index);

    if (OCI_MATCHING_TYPE(def, OCI_CDT_RAW))
    {
        unsigned int size = (unsigned int)(ub2)((ub2*)def->buf.lens)[def->rs->row_cur - 1];

        OCI_RETVAL = size < len ? size : len;
        OCI_STATUS = TRUE;

        /* for RAWs, we copy the data in the destination buffer instead of
        returning internal buffer as we do for strings */

        memcpy(buffer, OCI_DefineGetData(def), (size_t)OCI_RETVAL);
    }

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetRaw2
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_GetRaw2
(
    OCI_Resultset *rs,
    const otext   *name,
    void          *buffer,
    unsigned int   len
)
{
    OCI_Define *def = NULL;
    int index = -1;

    OCI_CALL_ENTER(unsigned int, 0)
    OCI_CALL_CHECK_PTR(OCI_IPC_RESULTSET, rs)
    OCI_CALL_CHECK_PTR(OCI_IPC_VOID, buffer)
    OCI_CALL_CHECK_PTR(OCI_IPC_STRING, name)
    OCI_CALL_CONTEXT_SET_FROM_STMT(rs->stmt)

    OCI_STATUS = FALSE;

    index = OCI_GetDefineIndex(rs, name);
    if (index > 0)
    {
        def = OCI_GetDefine(rs, index);

        if (OCI_MATCHING_TYPE(def, OCI_CDT_RAW))
        {
            unsigned int size = (unsigned int)(ub2)((ub2*)def->buf.lens)[def->rs->row_cur - 1];

            OCI_RETVAL = size < len ? size : len;
            OCI_STATUS = TRUE;

            /* for RAWs, we copy the data in the destination buffer instead of
            returning internal buffer as we do for strings */

            memcpy(buffer, OCI_DefineGetData(def), (size_t)OCI_RETVAL);
        }
    }

    OCI_CALL_EXIT()
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
    OCI_GET_NUMBER(rs, index, OCI_NUM_DOUBLE, double, 0.0)
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetDouble2
 * --------------------------------------------------------------------------------------------- */

double OCI_API OCI_GetDouble2
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    OCI_GET_BY_NAME(rs, name, OCI_GetDouble, double, 0.0)
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetFloat
 * --------------------------------------------------------------------------------------------- */

float OCI_API OCI_GetFloat
(
    OCI_Resultset *rs,
    unsigned int   index
)
{
    OCI_GET_NUMBER(rs, index, OCI_NUM_FLOAT, float, 0.0f)
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetFloat2
 * --------------------------------------------------------------------------------------------- */

float OCI_API OCI_GetFloat2
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    OCI_GET_BY_NAME(rs, name, OCI_GetFloat, float, 0.0f)
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
    OCI_GET_HANDLE
    (
        rs, index, OCI_Date *, NULL, OCI_CDT_DATETIME,

        OCI_DateInit(rs->stmt->con, (OCI_Date *) def->obj,
                     (OCIDate *) OCI_DefineGetData(def), FALSE,
                     (SQLT_DAT == def->col.libcode))
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetDate2
 * --------------------------------------------------------------------------------------------- */

OCI_Date * OCI_API OCI_GetDate2
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    OCI_GET_BY_NAME(rs, name, OCI_GetDate, OCI_Date *, NULL)
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
    OCI_GET_HANDLE
    (
        rs, index, OCI_Timestamp *, NULL, OCI_CDT_TIMESTAMP,

        OCI_TimestampInit(rs->stmt->con, (OCI_Timestamp *) def->obj,
                         (OCIDateTime *) OCI_DefineGetData(def), def->col.subtype)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetTimestamp2
 * --------------------------------------------------------------------------------------------- */

OCI_Timestamp * OCI_API OCI_GetTimestamp2
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    OCI_GET_BY_NAME(rs, name, OCI_GetTimestamp, OCI_Timestamp*, NULL)
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
    OCI_GET_HANDLE
    (
       rs, index, OCI_Interval *, NULL, OCI_CDT_INTERVAL,

       OCI_IntervalInit(rs->stmt->con, (OCI_Interval *) def->obj,
                        (OCIInterval *) OCI_DefineGetData(def), def->col.subtype)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetInterval2
 * --------------------------------------------------------------------------------------------- */

OCI_Interval * OCI_API OCI_GetInterval2
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    OCI_GET_BY_NAME(rs, name, OCI_GetInterval, OCI_Interval*, NULL)
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
    OCI_GET_HANDLE
    (
       rs, index, OCI_Object *, NULL, OCI_CDT_OBJECT,

       OCI_ObjectInit(rs->stmt->con, (OCI_Object *) def->obj,
                      OCI_DefineGetData(def), def->col.typinf,
                      NULL, -1, TRUE)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetObject2
 * --------------------------------------------------------------------------------------------- */

OCI_Object * OCI_API OCI_GetObject2
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    OCI_GET_BY_NAME(rs, name, OCI_GetObject, OCI_Object*, NULL)
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
    OCI_GET_HANDLE
    (
       rs, index, OCI_Coll *, NULL, OCI_CDT_COLLECTION,

       OCI_CollInit(rs->stmt->con, (OCI_Coll *) def->obj, OCI_DefineGetData(def), def->col.typinf)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetColl2
 * --------------------------------------------------------------------------------------------- */

OCI_Coll * OCI_API OCI_GetColl2
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    OCI_GET_BY_NAME(rs, name, OCI_GetColl, OCI_Coll*, NULL)
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
    OCI_GET_HANDLE
    (
       rs, index, OCI_Ref *, NULL, OCI_CDT_REF,

       OCI_RefInit(rs->stmt->con, def->col.typinf, (OCI_Ref *) def->obj, OCI_DefineGetData(def))
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetRef2
 * --------------------------------------------------------------------------------------------- */

OCI_Ref * OCI_API OCI_GetRef2
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    OCI_GET_BY_NAME(rs, name, OCI_GetRef, OCI_Ref*, NULL)
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
    OCI_GET_HANDLE
    (
       rs, index, OCI_Statement *, NULL, OCI_CDT_CURSOR,

       OCI_StatementInit(rs->stmt->con,(OCI_Statement *) def->obj,
                         (OCIStmt *) OCI_DefineGetData(def), TRUE, def->col.name)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetStatement2
 * --------------------------------------------------------------------------------------------- */

OCI_Statement * OCI_API OCI_GetStatement2
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    OCI_GET_BY_NAME(rs, name, OCI_GetStatement, OCI_Statement*, NULL)
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
    OCI_GET_HANDLE
    (
       rs, index, OCI_Lob *, NULL, OCI_CDT_LOB,

       OCI_LobInit(rs->stmt->con,(OCI_Lob *) def->obj,
                   (OCILobLocator *) OCI_DefineGetData(def),
                   def->col.subtype)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetLob2
 * --------------------------------------------------------------------------------------------- */

OCI_Lob * OCI_API OCI_GetLob2
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    OCI_GET_BY_NAME(rs, name, OCI_GetLob, OCI_Lob*, NULL)
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
    OCI_GET_HANDLE
    (
       rs, index, OCI_File *, NULL, OCI_CDT_FILE,

       OCI_FileInit(rs->stmt->con, (OCI_File *) def->obj,
                   (OCILobLocator *) OCI_DefineGetData(def),
                   def->col.subtype)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetFile2
 * --------------------------------------------------------------------------------------------- */

OCI_File * OCI_API OCI_GetFile2
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    OCI_GET_BY_NAME(rs, name, OCI_GetFile, OCI_File*, NULL)
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
    OCI_GET_HANDLE
    (
       rs, index, OCI_Long *, NULL, OCI_CDT_LONG,

       (OCI_Long *) OCI_DefineGetData(def)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetLong2
 * --------------------------------------------------------------------------------------------- */

OCI_Long * OCI_API OCI_GetLong2
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    OCI_GET_BY_NAME(rs, name, OCI_GetLong, OCI_Long*, NULL)
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetDataSize
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_GetDataSize
(
    OCI_Resultset *rs,
    unsigned int   index
)
{
    OCI_Define *def = NULL;

    OCI_CALL_ENTER(unsigned int, 0)
    OCI_CALL_CHECK_PTR(OCI_IPC_RESULTSET, rs)
    OCI_CALL_CHECK_BOUND(rs->stmt->con, index, 1, rs->nb_defs)
    OCI_CALL_CONTEXT_SET_FROM_STMT(rs->stmt)
    
    def = OCI_GetDefine(rs, index);

    if (def && OCI_DefineIsDataNotNull(def))
    {
        ub2* lens = (ub2 *)def->buf.lens;

        if (lens)
        {
            OCI_RETVAL = lens[rs->row_cur - 1];

            if (OCI_CDT_TEXT == def->col.datatype)
            {
                OCI_RETVAL /= sizeof(otext);
            }
        }
    }

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetDataSize2
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_GetDataSize2
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    int index = -1;

    OCI_CALL_ENTER(unsigned int, 0)        
    OCI_CALL_CHECK_PTR(OCI_IPC_RESULTSET, rs)
    OCI_CALL_CHECK_PTR(OCI_IPC_STRING, name)
    OCI_CALL_CONTEXT_SET_FROM_STMT(rs->stmt)

    OCI_STATUS = FALSE;

    index = OCI_GetDefineIndex(rs, name);

    if (index >= 0)
    {
        OCI_RETVAL = OCI_GetDataSize(rs, (unsigned int)index);
        OCI_STATUS = TRUE;
    }

    OCI_CALL_EXIT()
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
    OCI_Define *def = NULL;

    OCI_CALL_ENTER(boolean, TRUE)
    OCI_CALL_CHECK_PTR(OCI_IPC_RESULTSET, rs)
    OCI_CALL_CHECK_BOUND(rs->stmt->con, index, 1, rs->nb_defs)
    OCI_CALL_CONTEXT_SET_FROM_STMT(rs->stmt)

    def = OCI_GetDefine(rs, index);

    OCI_STATUS = (NULL != def);
    OCI_RETVAL = !OCI_DefineIsDataNotNull(def);

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_IsNull2
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_IsNull2
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    int index = -1;

    OCI_CALL_ENTER(boolean, TRUE)
    OCI_CALL_CHECK_PTR(OCI_IPC_RESULTSET, rs)
    OCI_CALL_CHECK_PTR(OCI_IPC_STRING, name)
    OCI_CALL_CONTEXT_SET_FROM_STMT(rs->stmt)

    OCI_STATUS = FALSE;

    index = OCI_GetDefineIndex(rs, name);

    if (index >= 0)
    {
        OCI_RETVAL = OCI_IsNull(rs, (unsigned int)index);
        OCI_STATUS = TRUE;
    }

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ResultsetGetStatment
 * --------------------------------------------------------------------------------------------- */

OCI_Statement * OCI_API OCI_ResultsetGetStatement
(
    OCI_Resultset *rs
)
{
    OCI_GET_PROP(OCI_Statement *, NULL, OCI_IPC_RESULTSET, rs, stmt, rs->stmt->con, rs->stmt, rs->stmt->con->err)
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
    OCI_Define *def = NULL;

    OCI_CALL_ENTER(unsigned int, 0)
    OCI_CALL_CHECK_PTR(OCI_IPC_RESULTSET, rs)
    OCI_CALL_CHECK_BOUND(rs->stmt->con, index, 1, rs->nb_defs)
    OCI_CALL_CONTEXT_SET_FROM_STMT(rs->stmt)

    OCI_STATUS = FALSE;

    def = OCI_GetDefine(rs, index);

    if (def && (rs->row_cur > 0))
    {
        OCI_RETVAL = (unsigned int)((ub2 *)def->buf.lens)[rs->row_cur - 1];
        OCI_STATUS = TRUE;
    }

    OCI_CALL_EXIT()
}
