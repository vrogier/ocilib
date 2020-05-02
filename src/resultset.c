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

#include "resultset.h"

#include "collection.h"
#include "column.h"
#include "date.h"
#include "define.h"
#include "error.h"
#include "exception.h"
#include "file.h"
#include "hash.h"
#include "helpers.h"
#include "interval.h"
#include "lob.h"
#include "long.h"
#include "macros.h"
#include "memory.h"
#include "number.h"
#include "object.h"
#include "reference.h"
#include "statement.h"
#include "strings.h"
#include "timestamp.h"

static unsigned int SeekModeValues[] =
{
    OCI_SFD_ABSOLUTE,
    OCI_SFD_RELATIVE
};

#define MATCHING_TYPE(def, type)                                                    \
                                                                                    \
    ((def) && DefineIsDataNotNull(def) && ((type) == (def)->col.datatype))  

#define GET_BY_NAME(rs, name, func, type, res)                                      \
                                                                                    \
    ENTER_FUNC(type, res, OCI_IPC_RESULTSET, rs)                                    \
                                                                                    \
    CHECK_PTR(OCI_IPC_RESULTSET, rs)                                                \
    CHECK_PTR(OCI_IPC_STRING, name)                                                 \
                                                                                    \
    int index = DefineGetIndex(rs, name);                                           \
    CHECK(index >= 0)                                                               \
                                                                                    \
    OCI_Error *err = ErrorGet(TRUE, TRUE);                                          \
                                                                                    \
    type tmp = func(rs, (unsigned int) index);                                      \
                                                                                    \
    CHECK_ERROR(err)                                                                \
                                                                                    \
    SET_RETVAL(tmp)                                                                 \
                                                                                    \
    EXIT_FUNC()

#define GET_NUMBER(rs, index, num_type, type, res)                                  \
                                                                                    \
    ENTER_FUNC(type, res, OCI_IPC_RESULTSET, rs)                                    \
                                                                                    \
    CHECK_PTR(OCI_IPC_RESULTSET, rs)                                                \
    CHECK_BOUND(index, 1, (rs)->nb_defs)                                            \
                                                                                    \
    type tmp = res;                                                                 \
                                                                                    \
    CHECK(DefineGetNumber(rs, index, &tmp, num_type))                               \
                                                                                    \
    SET_RETVAL(tmp)                                                                 \
                                                                                    \
    EXIT_FUNC()

#define GET_HANDLE(rs, index, type, res, lib_type, func)                            \
                                                                                    \
    ENTER_FUNC(type, res, OCI_IPC_RESULTSET, rs)                                    \
                                                                                    \
    OCI_Define *def = NULL;                                                         \
                                                                                    \
    CHECK_PTR(OCI_IPC_RESULTSET, rs)                                                \
    CHECK_BOUND(index, 1, (rs)->nb_defs)                                            \
                                                                                    \
    def = DefineGet(rs, index);                                                     \
    CHECK_NULL(def)                                                                 \
                                                                                    \
    type tmp = res;                                                                 \
                                                                                    \
    if (MATCHING_TYPE(def, lib_type))                                               \
    {                                                                               \
        def->obj = func;                                                            \
        CHECK_NULL(def->obj)                                                        \
        tmp = def->obj;                                                             \
    }                                                                               \
                                                                                    \
    SET_RETVAL(tmp)                                                                 \
                                                                                    \
    EXIT_FUNC()

/* --------------------------------------------------------------------------------------------- *
 * ResultsetCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Resultset * ResultsetCreate
(
    OCI_Statement *stmt,
    int            size
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Resultset*, NULL,
        /* context */ OCI_IPC_STATEMENT, stmt
    )

    OCI_Resultset* rs = NULL;

    CHECK_PTR(OCI_IPC_STATEMENT, stmt)

    /* allocate resultset structure */

    ALLOC_DATA(OCI_IPC_RESULTSET, rs, 1)

    /* set attributes */

    ub4 nb = 0;
    ub4 i;

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
        CHECK_ATTRIB_GET
        (
            OCI_HTYPE_STMT, OCI_ATTR_PARAM_COUNT,
            stmt->stmt, &nb, NULL,
            rs->stmt->con->err
        )
    }
    else
    {
        nb = stmt->nb_rbinds;
    }

    /* allocate columns array */

    ALLOC_DATA(OCI_IPC_DEFINE, rs->defs, nb)

    /* describe select list */

    if (OCI_CST_SELECT == stmt->type)
    {
        /* Compute columns information */

        for (i = 0; i < nb; i++)
        {
            OCI_Define *def = &rs->defs[i];

            def->buf.count   = size;
            def->buf.sizelen = sizeof(ub2);

            def->rs = rs;

            rs->nb_defs++;

            /* get column description */

            CHECK(ColumnRetrieveInfo(&def->col, rs->stmt->con,
                                     rs->stmt, rs->stmt->stmt,
                                     i + 1, OCI_DESC_RESULTSET))

            /* mapping to OCILIB internal types */

            CHECK(ColumnMapInfo(&def->col, rs->stmt))

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

        if (!(rs->stmt->exec_mode & OCI_DESCRIBE_ONLY) && !(rs->stmt->exec_mode & OCI_PARSE_ONLY))
        {
            for (i = 0; i < nb; i++)
            {
                OCI_Define *def = &rs->defs[i];

                CHECK_NULL(def)

                /* allocation of internal buffers for resultset content and
                   register OCILIB result buffers to OCI */

                CHECK(DefineAlloc( def))
                CHECK(DefineDef(def, i + 1))
            }
        }
    }
    else if (NULL != rs->defs)
    {
        /* get info from register binds */

        for (i = 0; i < stmt->nb_rbinds; i++)
        {
            OCI_Bind   *bnd = stmt->rbinds[i];
            OCI_Define *def = &rs->defs[bnd->dynpos];

            def->buf.count   = size;
            def->buf.sizelen = sizeof(ub4);

            def->rs = rs;

            rs->nb_defs++;

            /* columns info */

            def->col.sqlcode  = bnd->code;
            def->col.libcode  = bnd->code;
            def->col.name     = ostrdup(bnd->name);
            def->col.size     = (ub2) bnd->size;
            def->col.datatype = bnd->type;
            def->col.subtype  = bnd->subtype;
            def->col.typinf   = bnd->typinf;

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

            CHECK(ColumnMapInfo(&def->col, rs->stmt))
            CHECK(DefineAlloc(def))
        }
    }

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE)
        {
            ResultsetFree(rs);
            rs = NULL;
        }

        SET_RETVAL(rs)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * ResultsetExpandStrings
 * --------------------------------------------------------------------------------------------- */

boolean ResultsetExpandStrings
(
    OCI_Resultset *rs
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_RESULTSET, rs
    )

    CHECK_PTR(OCI_IPC_RESULTSET, rs)

    for (ub4 i = 0; i < rs->nb_defs; i++)
    {
        OCI_Define *def = &rs->defs[i];

        if (OCI_CDT_TEXT == def->col.datatype)
        {
            for (int j = (int) (def->buf.count-1); j >= 0; j--)
            {
                ub1 * tmpbuf = ((ub1*) def->buf.data) + (def->col.bufsize * j);

                StringUTF16ToUTF32(tmpbuf, tmpbuf, (def->col.bufsize / sizeof(otext) ) -1);
            }
        }
    }

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * FetchPieces
 * --------------------------------------------------------------------------------------------- */

boolean FetchPieces
(
    OCI_Resultset *rs
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_RESULTSET, rs
    )

    ub4 type, dx;
    ub1 in_out;
    ub4 i, j;

    CHECK_PTR(OCI_IPC_RESULTSET, rs)

    /* reset long objects */

    for (i = 0; i < rs->nb_defs; i++)
    {
        OCI_Define *def = &rs->defs[i];

        if (OCI_CDT_LONG == def->col.datatype)
        {
            for (j = 0; j < def->buf.count; j++)
            {
                def->buf.data[j] = LongInitialize(rs->stmt, (OCI_Long *)def->buf.data[j], def, def->col.subtype);
            }
        }
    }

    /* dynamic fetch */

    while (OCI_NEED_DATA == rs->fetch_status)
    {
        ub1   piece  = OCI_NEXT_PIECE;
        ub4   iter   = 0;
        void *handle = NULL;

        /* get piece information */

        CHECK_OCI
        (
            rs->stmt->con->err,
            OCIStmtGetPieceInfo, 
            rs->stmt->stmt, rs->stmt->con->err,
            &handle,  &type, &in_out, 
            &iter, &dx, &piece
        )

        /* search for the given column */

        for (i = 0; i < rs->nb_defs; i++)
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

                    ALLOC_DATA(OCI_IPC_LONG_BUFFER, lg->buffer, lg->maxsize)
                }
                else if ((lg->size*char_fact) >= (lg->maxsize - trailing_size))
                {
                    lg->maxsize = (lg->size + trailing_size + bufsize) * char_fact;

                    lg->buffer = (ub1 *) MemoryRealloc(lg->buffer, (size_t) OCI_IPC_LONG_BUFFER,
                                             (size_t) lg->maxsize, 1, TRUE);
                }

                /* update piece info */

                lg->piecesize = bufsize;

                if (OCI_CLONG == lg->type)
                {
                    lg->piecesize /= sizeof(otext);
                    lg->piecesize *= sizeof(dbtext);
                    lg->piecesize -= (ub4) sizeof(dbtext);
                }

                CHECK_OCI
                (
                    rs->stmt->con->err,
                    OCIStmtSetPieceInfo,
                    (dvoid *) handle,
                    (ub4) OCI_HTYPE_DEFINE,
                    lg->stmt->con->err,
                    (dvoid *) (lg->buffer + (size_t) lg->size),
                    &lg->piecesize, piece,
                    lg->def->buf.inds, (ub2 *) NULL
                )

                break;
            }
        }

        /* fetch data */

#if defined(OCI_STMT_SCROLLABLE_READONLY)

        if (Env.use_scrollable_cursors)
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
            THROW(ExceptionOCI, rs->stmt->con->err, rs->fetch_status)
        }
        else if (OCI_SUCCESS_WITH_INFO == rs->fetch_status)
        {
            ExceptionOCI(&call_context, rs->stmt->con->err, rs->fetch_status);
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
                    const int len = (int) ( lg->size / sizeof(dbtext) );

                    ((dbtext *)lg->buffer)[len] = 0;

                    if (Env.use_wide_char_conv)
                    {
                        StringUTF16ToUTF32(lg->buffer, lg->buffer, len);
                    }
                }
            }
        }
    }

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * ClearFetchedObjectInstances
 * --------------------------------------------------------------------------------------------- */

boolean ClearFetchedObjectInstances(OCI_Resultset *rs)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_RESULTSET, rs
    )

    CHECK_PTR(OCI_IPC_RESULTSET, rs)

    for (ub4 i = 0; i < rs->nb_defs; i++)
    {
        OCI_Define *def = &(rs->defs[i]);

        if (SQLT_NTY == def->col.sqlcode && def->buf.data)
        {
            for (ub4 j = 0; j < def->buf.count; j++)
            {
                if (def->buf.data[j] != NULL)
                {
                    OCIObjectFree(rs->stmt->con->env, rs->stmt->con->err, def->buf.data[j], OCI_DEFAULT);
                    def->buf.data[j] = NULL;
                }
            }
        }
    }

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * FetchData
 * --------------------------------------------------------------------------------------------- */

boolean FetchData
(
    OCI_Resultset *rs,
    int            mode,
    int            offset
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_RESULTSET, rs
    )

    CHECK_PTR(OCI_IPC_RESULTSET, rs)

    /* let's initialize the success flag to FALSE until the process completes */

   CHECK(ClearFetchedObjectInstances(rs))

    /* internal fetch */

#if defined(OCI_STMT_SCROLLABLE_READONLY)

    if (Env.use_scrollable_cursors)
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
        THROW(ExceptionOCI, rs->stmt->con->err, rs->fetch_status)
    }
    else if (OCI_SUCCESS_WITH_INFO == rs->fetch_status)
    {
        ExceptionOCI(&call_context, rs->stmt->con->err, rs->fetch_status);
    }
    else if (OCI_NEED_DATA == rs->fetch_status)
    {
        /* need to do a piecewise fetch */
        CHECK(FetchPieces(rs))
    }

    /* check string buffer for Unicode builds that need buffer expansion */

    if (Env.use_wide_char_conv)
    {
       CHECK(ResultsetExpandStrings(rs))
    }

    /* update internal fetch status and variables */

    ub4 row_count   = 0;
    ub4 row_fetched = 0;

#if defined(OCI_STMT_SCROLLABLE_READONLY)

    if (OCI_SFM_SCROLLABLE == rs->stmt->exec_mode)
    {
        CHECK_ATTRIB_GET
        (
            OCI_HTYPE_STMT, OCI_ATTR_CURRENT_POSITION, 
            rs->stmt->stmt, &row_count, NULL,
            rs->stmt->con->err
        )

        CHECK_ATTRIB_GET
        (
            OCI_HTYPE_STMT, OCI_ATTR_ROWS_FETCHED,
            rs->stmt->stmt, &row_fetched, NULL,
            rs->stmt->con->err
        )
    }
    else

#endif

    {
        row_count   = StatementGetAffectedRows(rs->stmt);
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

        CHECK(FALSE)
    }

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * FetchCustom
 * --------------------------------------------------------------------------------------------- */

boolean FetchCustom
(
    OCI_Resultset *rs,
    int            mode,
    int            offset
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_RESULTSET, rs
    )

    CHECK_PTR(OCI_IPC_RESULTSET, rs)

    boolean result = FALSE;

    switch (mode)
    {
        case OCI_SFD_RELATIVE:
        {
            if (((offset > 0) && (rs->eof)) ||
                ((offset < 0) && (rs->bof)) ||
                (offset == 0))
            {
                result = FALSE;
            }
            else
            {
                const int offset_save = offset;

                offset      = offset - (int) (rs->row_fetched + rs->row_cur);
                rs->row_cur = 1;

                result = FetchData(rs, mode, offset);

                if (result)
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
                result = FALSE;
            }
            else
            {
                rs->row_abs = 1;
                rs->row_cur = 1;

                result = FetchData(rs, mode, offset);

                if (result)
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
            result = FALSE;
            break;
        }
    }

    SET_RETVAL(result)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * ResultsetInitialize
 * --------------------------------------------------------------------------------------------- */

boolean ResultsetInitialize
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
 * ResultsetFree
 * --------------------------------------------------------------------------------------------- */

boolean ResultsetFree
(
    OCI_Resultset *rs
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_RESULTSET, rs
    )

    ub4 j;

    CHECK_PTR(OCI_IPC_RESULTSET, rs)

    for (ub4 i = 0; i < rs->nb_defs; i++)
    {
        OCI_Define *def = &(rs->defs[i]);

        /* free buffer objects */

        if (def->obj)
        {
            /* handy cast to set object state flag */

            ((OCI_Datatype *)def->obj)->hstate = OCI_OBJECT_FETCHED_DIRTY;

            if (OCI_CDT_CURSOR == def->col.datatype)
            {
                StatementDispose((OCI_Statement *)def->obj);
                FREE(def->obj)
            }
            else
            {
                FreeObjectFromType(def->obj, def->col.datatype);
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
                        MemoryFreeHandle((dvoid *)def->buf.data[j], (ub4)def->col.handletype);
                    }
                }
            }
            else
            {
                MemoryFreeDescriptorArray((dvoid *)def->buf.data,  (ub4)def->col.handletype, (ub4)def->buf.count);
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

                    LongFree((OCI_Long *)def->buf.data[j]);
                }
            }
        }

        /* free object instances from object cache */

        ClearFetchedObjectInstances(rs);

        /* free column pointers */

        FREE(def->col.name)

        /* free buffer pointers */

        FREE(def->buf.data)
        FREE(def->buf.inds)
        FREE(def->buf.obj_inds)
        FREE(def->buf.lens)
        FREE(def->buf.tmpbuf)
    }

    /* free column map */

    if (NULL != rs->map)
    {
        HashFree(rs->map);
    }

    /* free defines (column array) */

    FREE(rs->defs)

    FREE(rs)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * ResultsetFetchPrev
 * --------------------------------------------------------------------------------------------- */

boolean ResultsetFetchPrev
(
    OCI_Resultset *rs
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_RESULTSET, rs
    )

    CHECK_PTR(OCI_IPC_RESULTSET, rs)
    CHECK_STMT_STATUS(rs->stmt, OCI_STMT_EXECUTED)
    CHECK_SCROLLABLE_CURSOR_ENABLED(rs->stmt->con)

#if OCI_VERSION_COMPILE >= OCI_9_0

    CHECK_SCROLLABLE_CURSOR_ACTIVATED(rs->stmt)

    CHECK(!rs->bof)

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
                offset = 1 - (int) rs->row_abs;
            }
            else
            {
                offset = 1 - (int) (rs->fetch_size + rs->row_fetched);
            }

            CHECK(FetchData(rs, OCI_SFD_RELATIVE, offset))

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
    else
    {
        rs->row_cur--;
        rs->row_abs--;
    }

    rs->eof = FALSE;

    CHECK(!rs->bof)

    SET_SUCCESS()

#endif


    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * ResultsetFetchNext
 * --------------------------------------------------------------------------------------------- */

boolean ResultsetFetchNext
(
    OCI_Resultset *rs
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_RESULTSET, rs
    )

    CHECK_PTR(OCI_IPC_RESULTSET, rs)
    CHECK_STMT_STATUS(rs->stmt, OCI_STMT_EXECUTED)

    CHECK(!rs->eof)

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
                CHECK(FetchData(rs, OCI_SFD_NEXT, 0))

                rs->bof     = FALSE;
                rs->row_cur = 1;

                rs->row_abs++;
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

            if (Env.use_wide_char_conv)
            {
                ResultsetExpandStrings(rs);
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

    CHECK(!rs->eof)

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * ResultsetFetchFirst
 * --------------------------------------------------------------------------------------------- */

boolean ResultsetFetchFirst
(
    OCI_Resultset *rs
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_RESULTSET, rs
    )

    CHECK_PTR(OCI_IPC_RESULTSET, rs)
    CHECK_STMT_STATUS(rs->stmt, OCI_STMT_EXECUTED)
    CHECK_SCROLLABLE_CURSOR_ENABLED(rs->stmt->con)

#if OCI_VERSION_COMPILE >= OCI_9_0

    CHECK_SCROLLABLE_CURSOR_ACTIVATED(rs->stmt)

    rs->bof = FALSE;
    rs->eof = FALSE;

    rs->row_abs = 1;
    rs->row_cur = 1;

    CHECK(FetchData(rs, OCI_SFD_FIRST, 0))

    CHECK(!rs->bof)

    SET_SUCCESS()

#endif

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * ResultsetFetchLast
 * --------------------------------------------------------------------------------------------- */

boolean ResultsetFetchLast
(
    OCI_Resultset *rs
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_RESULTSET, rs
    )

    CHECK_PTR(OCI_IPC_RESULTSET, rs)
    CHECK_STMT_STATUS(rs->stmt, OCI_STMT_EXECUTED)
    CHECK_SCROLLABLE_CURSOR_ENABLED(rs->stmt->con)

    boolean result = FALSE;

#if OCI_VERSION_COMPILE >= OCI_9_0

    CHECK_SCROLLABLE_CURSOR_ACTIVATED(rs->stmt)

    rs->bof = FALSE;
    rs->eof = FALSE;

    rs->row_abs = 0;
    rs->row_cur = 1;

    result = FetchData(rs, OCI_SFD_LAST, 0);

    if (result)
    {
        result = !rs->eof;
    }

    rs->row_abs = rs->row_count;

#endif


    SET_RETVAL(result)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * ResultsetFetchSeek
 * --------------------------------------------------------------------------------------------- */

boolean ResultsetFetchSeek
(
    OCI_Resultset *rs,
    unsigned int   mode,
    int            offset
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_RESULTSET, rs
    )

    CHECK_PTR(OCI_IPC_RESULTSET, rs)
    CHECK_STMT_STATUS(rs->stmt, OCI_STMT_EXECUTED)
    CHECK_SCROLLABLE_CURSOR_ENABLED(rs->stmt->con)

#if OCI_VERSION_COMPILE >= OCI_9_0

    CHECK_SCROLLABLE_CURSOR_ACTIVATED(rs->stmt)
    CHECK_ENUM_VALUE(mode, SeekModeValues, OTEXT("Fetch Seek Mode"))

    CHECK(FetchCustom(rs, (int)mode, offset))

    SET_SUCCESS()

#else

    OCI_NOT_USED(mode)
    OCI_NOT_USED(offset)

#endif

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * ResultsetGetRowCount
 * --------------------------------------------------------------------------------------------- */

unsigned int ResultsetGetRowCount
(
    OCI_Resultset *rs
)
{
    GET_PROP
    (
        unsigned int, 0, 
        OCI_IPC_RESULTSET, rs,
        row_count
    )
}

/* --------------------------------------------------------------------------------------------- *
 * ResultsetGetRowCount
 * --------------------------------------------------------------------------------------------- */

unsigned int ResultsetGetCurrentRow
(
    OCI_Resultset *rs
)
{
    GET_PROP
    (
        unsigned int, 0,
        OCI_IPC_RESULTSET, rs,
        row_abs
    )
}

/* --------------------------------------------------------------------------------------------- *
 * ResultsetGetColumnCount
 * --------------------------------------------------------------------------------------------- */

unsigned int ResultsetGetColumnCount
(
    OCI_Resultset *rs
)
{
    GET_PROP
    (
        unsigned int, 0, 
        OCI_IPC_RESULTSET, rs,
        nb_defs
    )
}

/* --------------------------------------------------------------------------------------------- *
 * ResultsetGetColumn
 * --------------------------------------------------------------------------------------------- */

OCI_Column * ResultsetGetColumn
(
    OCI_Resultset *rs,
    unsigned int   index
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Column*, NULL,
        /* context */ OCI_IPC_RESULTSET, rs
    )

    CHECK_PTR(OCI_IPC_RESULTSET, rs)
    CHECK_BOUND(index, 1, rs->nb_defs)

    SET_RETVAL(&rs->defs[index - 1].col)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * ResultsetGetColumn2
 * --------------------------------------------------------------------------------------------- */

OCI_Column * ResultsetGetColumn2
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Column*, NULL,
        /* context */ OCI_IPC_RESULTSET, rs
    )

    CHECK_PTR(OCI_IPC_RESULTSET, rs)
    CHECK_PTR(OCI_IPC_STRING, name)

    const int index = DefineGetIndex(rs, name);
    CHECK(index >= 0)

    SET_RETVAL(&rs->defs[index - 1].col)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * ResultsetGetColumnIndex
 * --------------------------------------------------------------------------------------------- */

unsigned int ResultsetGetColumnIndex
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    ENTER_FUNC
    (
        /* returns */ unsigned int, 0,
        /* context */ OCI_IPC_RESULTSET, rs
    )

    CHECK_PTR(OCI_IPC_RESULTSET, rs)
    CHECK_PTR(OCI_IPC_STRING, name)

    const int index = DefineGetIndex(rs, name);
    CHECK(index >= 0)

    SET_RETVAL((unsigned int) index)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * ResultsetSetStructNumericType
 * --------------------------------------------------------------------------------------------- */

boolean ResultsetSetStructNumericType
(
    OCI_Resultset *rs,
    unsigned int   index,
    unsigned int   type
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_RESULTSET, rs
    )

    CHECK_PTR(OCI_IPC_RESULTSET, rs)
    CHECK_BOUND(index,  1,  rs->nb_defs)
    CHECK_COMPAT(OCI_CDT_NUMERIC == rs->defs[index - 1].col.datatype);

    rs->defs[index-1].col.struct_subtype = (ub2) type;

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * ResultsetSetStructNumericType2
 * --------------------------------------------------------------------------------------------- */

boolean ResultsetSetStructNumericType2
(
    OCI_Resultset *rs,
    const otext   *name,
    unsigned int   type
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_RESULTSET, rs
    )

    CHECK_PTR(OCI_IPC_RESULTSET, rs)
    CHECK_PTR(OCI_IPC_STRING, name)

    const int index = DefineGetIndex(rs, name);
    CHECK(index >= 0)

    CHECK(ResultsetSetStructNumericType(rs, (unsigned int)index, type))

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * ResultsetGetStruct
 * --------------------------------------------------------------------------------------------- */

boolean ResultsetGetStruct
(
    OCI_Resultset *rs,
    void          *row_struct,
    void          *row_struct_ind
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_RESULTSET, rs
    )

    char    *ptr  = NULL;
    boolean *inds = NULL;

    CHECK_PTR(OCI_IPC_RESULTSET, rs)
    CHECK_PTR(OCI_IPC_VOID, row_struct)

    ptr  = (char    *) row_struct;
    inds = (boolean *) row_struct_ind;

    if (ptr)
    {
        size_t size1 = 0;
        size_t size2 = 0;
        size_t align = 0;

        size_t size = 0;

        for (ub4 i = 1; i <= rs->nb_defs; i++)
        {
            OCI_Define *def = &rs->defs[i - 1];

            const boolean is_not_null = DefineIsDataNotNull(def);

            if (i == 1)
            {
                ColumnGetAttributeInfo(&def->col, rs->nb_defs, i - 1, &size1, &align);
            }
            else
            {
                size1 = size2;
            }

            ColumnGetAttributeInfo(&rs->defs[i].col, rs->nb_defs, i, &size2, &align);

            size += size1;

            size = ROUNDUP(size, align);

            memset(ptr, 0, size1);

            if (is_not_null)
            {
                switch (def->col.datatype)
                {
                    case OCI_CDT_NUMERIC:
                    {
                        ub2 type = def->col.struct_subtype;
                        if (type == OCI_UNKNOWN)
                        {
                            type = def->col.subtype;
                        }

                        DefineGetNumber(rs, i, ptr, type);

                        break;
                    }
                    case OCI_CDT_TEXT:
                    {
                        *((otext **) ptr) =  (otext * ) ResultsetGetString(rs, i);
                        break;
                    }
                    case OCI_CDT_RAW:
                    {
                        *((void **) ptr) = DefineGetData(def);
                        break;
                    }
                    case OCI_CDT_LONG:
                    {
                        *((OCI_Long **) ptr) = ResultsetGetLong(rs, i);
                        break;
                    }
                    case OCI_CDT_DATETIME:
                    {
                        *((OCI_Date **) ptr) = ResultsetGetDate(rs, i);
                        break;
                    }
                    case OCI_CDT_CURSOR:
                    {
                        *((OCI_Statement **) ptr) = ResultsetGetStatement(rs, i);
                        break;
                    }
                    case OCI_CDT_LOB:
                    {
                        *((OCI_Lob **) ptr) = ResultsetGetLob(rs, i);
                        break;
                    }
                    case OCI_CDT_FILE:
                    {
                        *((OCI_File **) ptr) = ResultsetGetFile(rs, i);
                        break;
                    }
                    case OCI_CDT_TIMESTAMP:
                    {
                        *((OCI_Timestamp **) ptr) = ResultsetGetTimestamp(rs, i);

                        break;
                    }
                    case OCI_CDT_INTERVAL:
                    {
                        *((OCI_Interval **) ptr) = ResultsetGetInterval(rs, i);
                        break;
                    }
                    case OCI_CDT_OBJECT:
                    {
                        *((OCI_Object **) ptr) = ResultsetGetObject(rs, i);
                        break;
                    }
                    case OCI_CDT_COLLECTION:
                    {
                        *((OCI_Coll **) ptr) = ResultsetGetColl(rs, i);
                        break;
                    }
                    case OCI_CDT_REF:
                    {
                        *((OCI_Ref **) ptr) = ResultsetGetReference(rs, i);
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

    SET_SUCCESS()

   EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
* ResultsetGetNumber
* --------------------------------------------------------------------------------------------- */

OCI_Number * ResultsetGetNumber
(
    OCI_Resultset *rs,
    unsigned int   index
)
{
    GET_HANDLE
    (
        rs, index, OCI_Number *, NULL, OCI_CDT_NUMERIC,

        NumberInitialize(rs->stmt->con, (OCI_Number *) def->obj, (OCINumber *) DefineGetData(def))
    )
}

/* --------------------------------------------------------------------------------------------- *
 * ResultsetGetNumber2
 * --------------------------------------------------------------------------------------------- */

OCI_Number * ResultsetGetNumber2
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    GET_BY_NAME(rs, name, ResultsetGetNumber, OCI_Number *, NULL)
}

/* --------------------------------------------------------------------------------------------- *
 * ResultsetGetShort
 * --------------------------------------------------------------------------------------------- */

short ResultsetGetShort
(
    OCI_Resultset *rs,
    unsigned int   index
)
{
    GET_NUMBER(rs, index, OCI_NUM_SHORT, short, 0)
}

/* --------------------------------------------------------------------------------------------- *
 * ResultsetGetShort2
 * --------------------------------------------------------------------------------------------- */

short ResultsetGetShort2
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    GET_BY_NAME(rs, name, ResultsetGetShort, short, 0)
}

/* --------------------------------------------------------------------------------------------- *
 * ResultsetGetUnsignedShort
 * --------------------------------------------------------------------------------------------- */

unsigned short ResultsetGetUnsignedShort
(
    OCI_Resultset *rs,
    unsigned int   index
)
{
    GET_NUMBER(rs, index, OCI_NUM_USHORT, unsigned short, 0)
}

/* --------------------------------------------------------------------------------------------- *
 * ResultsetGetUnsignedShort2
 * --------------------------------------------------------------------------------------------- */

unsigned short ResultsetGetUnsignedShort2
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    GET_BY_NAME(rs, name, ResultsetGetUnsignedShort, unsigned short, 0)
}

/* --------------------------------------------------------------------------------------------- *
 * ResultsetGetInt
 * --------------------------------------------------------------------------------------------- */

int ResultsetGetInt
(
    OCI_Resultset *rs,
    unsigned int   index
)
{
    GET_NUMBER(rs, index, OCI_NUM_INT, int, 0)
}

/* --------------------------------------------------------------------------------------------- *
 * ResultsetGetInt2
 * --------------------------------------------------------------------------------------------- */

int ResultsetGetInt2
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    GET_BY_NAME(rs, name, ResultsetGetInt, int, 0)
}

/* --------------------------------------------------------------------------------------------- *
 * ResultsetGetUnsignedInt
 * --------------------------------------------------------------------------------------------- */

unsigned int ResultsetGetUnsignedInt
(
    OCI_Resultset *rs,
    unsigned int   index
)
{
    GET_NUMBER(rs, index, OCI_NUM_UINT, unsigned int, 0)
}

/* --------------------------------------------------------------------------------------------- *
 * ResultsetGetUnsignedInt2
 * --------------------------------------------------------------------------------------------- */

unsigned int ResultsetGetUnsignedInt2
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    GET_BY_NAME(rs, name, ResultsetGetUnsignedInt, unsigned int, 0)
}

/* --------------------------------------------------------------------------------------------- *
 * ResultsetGetBigInt
 * --------------------------------------------------------------------------------------------- */

big_int ResultsetGetBigInt
(
    OCI_Resultset *rs,
    unsigned int   index
)
{
    GET_NUMBER(rs, index, OCI_NUM_BIGINT, big_int, 0)
}

/* --------------------------------------------------------------------------------------------- *
 * ResultsetGetBigInt2
 * --------------------------------------------------------------------------------------------- */

big_int ResultsetGetBigInt2
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    GET_BY_NAME(rs, name, ResultsetGetBigInt, big_int, 0)
}

/* --------------------------------------------------------------------------------------------- *
 * ResultsetGetUnsignedBigInt
 * --------------------------------------------------------------------------------------------- */

big_uint ResultsetGetUnsignedBigInt
(
    OCI_Resultset *rs,
    unsigned int   index
)
{
    GET_NUMBER(rs, index, OCI_NUM_BIGUINT, big_uint, 0)
}

/* --------------------------------------------------------------------------------------------- *
 * ResultsetGetUnsignedInt2
 * --------------------------------------------------------------------------------------------- */

big_uint ResultsetGetUnsignedBigInt2
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    GET_BY_NAME(rs, name, ResultsetGetUnsignedBigInt, big_uint, 0)
}

/* --------------------------------------------------------------------------------------------- *
 * ResultsetGetString
 * --------------------------------------------------------------------------------------------- */

const otext * ResultsetGetString
(
    OCI_Resultset *rs,
    unsigned int   index
)
{
    ENTER_FUNC
    (
        /* returns */ otext *, NULL,
        /* context */ OCI_IPC_RESULTSET, rs
    )

    OCI_Define *def = NULL;

    CHECK_PTR(OCI_IPC_RESULTSET, rs)
    CHECK_BOUND(index, 1, rs->nb_defs)

    def = DefineGet(rs, index);
    CHECK_NULL(def)

    otext *result = NULL;

    if (DefineIsDataNotNull(def))
    {
        void *data = NULL;

        if (OCI_CDT_TEXT == def->col.datatype)
        {
            result = (otext *)DefineGetData(def);

            /* for long mapped to string, the zero terminal character is not
                always added by Oracle ? or OCILIB issue ? Anyway we check the
                length returned by Oracle and set it properly */

            if (OCI_CLONG == def->col.subtype)
            {
                ub2* lens = (ub2 *)def->buf.lens;

                result[lens[rs->row_cur - 1]] = 0;
            }
        }
        else if ((OCI_CDT_LONG == def->col.datatype) && (OCI_CLONG == def->col.subtype))
        {
            OCI_Long *lg = ResultsetGetLong(rs, index);

            if (lg)
            {
                result = (otext *)LongGetBuffer(lg);
            }
        }
        else
        {
            OCI_Error *err = ErrorGet(TRUE, TRUE);

            unsigned int bufsize   = OCI_SIZE_TMP_CVT;
            unsigned int data_size = 0;

            switch (def->col.datatype)
            {
                case OCI_CDT_NUMERIC:
                {
                    data = DefineGetData(def);
                    break;
                }
                case OCI_CDT_DATETIME:
                {
                    data = ResultsetGetDate(rs, index);
                    break;
                }
                case OCI_CDT_TIMESTAMP:
                {
                    data = ResultsetGetTimestamp(rs, index);
                    break;
                }
                case OCI_CDT_INTERVAL:
                {
                    data = ResultsetGetInterval(rs, index);
                    break;
                }
                case OCI_CDT_RAW:
                {
                    data      = DefineGetData(def);
                    data_size = ((ub2*)def->buf.lens)[def->rs->row_cur - 1];
                    break;
                }
                case OCI_CDT_REF:
                {
                    data = ResultsetGetReference(rs, index);
                    break;
                }
                case OCI_CDT_LONG:
                {
                    OCI_Long *lg = ResultsetGetLong(rs, index);

                    if (lg)
                    {
                        bufsize = LongGetSize(lg);

                        if (OCI_BLONG == def->col.subtype)
                        {
                            /* here we have binary long, it will be output in hexadecimal */
                            bufsize *= 2;
                        }
                    }

                    data = lg;
                    break;
                }

                case OCI_CDT_LOB:
                {
                    OCI_Lob *lob = ResultsetGetLob(rs, index);

                    if (lob)
                    {
                        bufsize = (unsigned int)LobGetLength(lob);

                        if (OCI_BLOB == def->col.subtype)
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

                    OCI_File * file = ResultsetGetFile(rs, index);

                    if (file)
                    {
                        bufsize = (unsigned int) ostrlen(OTEXT("/"));

                        FileGetInfo(file);

                        bufsize += (unsigned int) (file->dir ? ostrlen(file->dir) : 0);
                        bufsize += (unsigned int) (file->name ? ostrlen(file->name) : 0);
                    }

                    data = file;
                    break;
                }
                case OCI_CDT_OBJECT:
                {
                    OCI_Object *obj = ResultsetGetObject(rs, index);

                    if (obj)
                    {
                        CHECK(ObjectToString(obj, &bufsize, NULL))
                    }

                    data = obj;
                    break;
                }
                case OCI_CDT_COLLECTION:
                {
                    OCI_Coll *coll = ResultsetGetColl(rs, index);

                    if (coll)
                    {
                        CHECK(CollectionToString(coll, &bufsize, NULL))
                    }

                    data = coll;
                    break;
                }
                case OCI_CDT_CURSOR:
                {
                    OCI_Statement *stmt = ResultsetGetStatement(rs, index);

                    if (stmt && stmt->sql)
                    {
                        bufsize = (unsigned int) ostrlen(stmt->sql);
                    }

                    data = stmt;
                    break;
                }
                default:
                {
                    break;
                }
            }

            CHECK(NULL == err || OCI_UNKNOWN == err->type)

            CHECK(StringRequestBuffer(&def->buf.tmpbuf, &def->buf.tmpsize, bufsize))

            CHECK(StringGetFromType(rs->stmt->con, &def->col, data, data_size,
                                    def->buf.tmpbuf, def->buf.tmpsize, FALSE))

            CHECK(NULL == err || OCI_UNKNOWN == err->type)

            result = def->buf.tmpbuf;
        }
    }

    SET_RETVAL(result)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * ResultsetGetString2
 * --------------------------------------------------------------------------------------------- */

const otext * ResultsetGetString2
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    GET_BY_NAME(rs, name, ResultsetGetString, const otext *, NULL)
}

/* --------------------------------------------------------------------------------------------- *
 * ResultsetGetRaw
 * --------------------------------------------------------------------------------------------- */

unsigned int ResultsetGetRaw
(
    OCI_Resultset *rs,
    unsigned int   index,
    void          *buffer,
    unsigned int   len
)
{
    ENTER_FUNC
    (
        /* returns */ unsigned int, 0,
        /* context */ OCI_IPC_RESULTSET, rs
    )

    CHECK_PTR(OCI_IPC_RESULTSET, rs)
    CHECK_PTR(OCI_IPC_VOID, buffer);
    CHECK_BOUND(index, 1, rs->nb_defs)

    OCI_Define*def = DefineGet(rs, index);
    CHECK_NULL(def)

    unsigned int result = 0;

    if (MATCHING_TYPE(def, OCI_CDT_RAW))
    {
        unsigned int size = (unsigned int)(ub2)((ub2*)def->buf.lens)[def->rs->row_cur - 1];

        result = size < len ? size : len;

        /* for RAWs, we copy the data in the destination buffer instead of
        returning internal buffer as we do for strings */

        memcpy(buffer, DefineGetData(def), (size_t)result);
    }

    SET_RETVAL(result)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * ResultsetGetRaw2
 * --------------------------------------------------------------------------------------------- */

unsigned int ResultsetGetRaw2
(
    OCI_Resultset *rs,
    const otext   *name,
    void          *buffer,
    unsigned int   len
)
{
    ENTER_FUNC
    (
        /* returns */ unsigned int, 0,
        /* context */ OCI_IPC_RESULTSET, rs
    )

    CHECK_PTR(OCI_IPC_RESULTSET, rs)
    CHECK_PTR(OCI_IPC_VOID, buffer)
    CHECK_PTR(OCI_IPC_STRING, name)

    const int index = DefineGetIndex(rs, name);
    CHECK(index >= 0)

    OCI_Define*def = DefineGet(rs, index);
    CHECK_NULL(def)

    unsigned int result = 0;

    if (MATCHING_TYPE(def, OCI_CDT_RAW))
    {
        unsigned int size = (unsigned int)(ub2)((ub2*)def->buf.lens)[def->rs->row_cur - 1];

        result = size < len ? size : len;

        /* for RAWs, we copy the data in the destination buffer instead of
        returning internal buffer as we do for strings */

        memcpy(buffer, DefineGetData(def), (size_t)result);
    }

    SET_RETVAL(result)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * ResultsetGetDouble
 * --------------------------------------------------------------------------------------------- */

double ResultsetGetDouble
(
    OCI_Resultset *rs,
    unsigned int   index
)
{
    GET_NUMBER(rs, index, OCI_NUM_DOUBLE, double, 0.0)
}

/* --------------------------------------------------------------------------------------------- *
 * ResultsetGetDouble2
 * --------------------------------------------------------------------------------------------- */

double ResultsetGetDouble2
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    GET_BY_NAME(rs, name, ResultsetGetDouble, double, 0.0)
}

/* --------------------------------------------------------------------------------------------- *
 * ResultsetGetFloat
 * --------------------------------------------------------------------------------------------- */

float ResultsetGetFloat
(
    OCI_Resultset *rs,
    unsigned int   index
)
{
    GET_NUMBER(rs, index, OCI_NUM_FLOAT, float, 0.0f)
}

/* --------------------------------------------------------------------------------------------- *
 * ResultsetGetFloat2
 * --------------------------------------------------------------------------------------------- */

float ResultsetGetFloat2
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    GET_BY_NAME(rs, name, ResultsetGetFloat, float, 0.0f)
}

/* --------------------------------------------------------------------------------------------- *
 * ResultsetGetDate
 * --------------------------------------------------------------------------------------------- */

OCI_Date * ResultsetGetDate
(
    OCI_Resultset *rs,
    unsigned int   index
)
{
    GET_HANDLE
    (
        rs, index, OCI_Date *, NULL, OCI_CDT_DATETIME,

        DateInitialize(rs->stmt->con, (OCI_Date *) def->obj,
                       (OCIDate *)DefineGetData(def), FALSE,
                       (SQLT_DAT == def->col.libcode))
    )
}

/* --------------------------------------------------------------------------------------------- *
 * ResultsetGetDate2
 * --------------------------------------------------------------------------------------------- */

OCI_Date * ResultsetGetDate2
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    GET_BY_NAME(rs, name, ResultsetGetDate, OCI_Date *, NULL)
}

/* --------------------------------------------------------------------------------------------- *
 * ResultsetGetTimestamp
 * --------------------------------------------------------------------------------------------- */

OCI_Timestamp * ResultsetGetTimestamp
(
    OCI_Resultset *rs,
    unsigned int   index
)
{
    GET_HANDLE
    (
        rs, index, OCI_Timestamp *, NULL, OCI_CDT_TIMESTAMP,

        TimestampInitialize(rs->stmt->con, (OCI_Timestamp *) def->obj,
                            (OCIDateTime *) DefineGetData(def), def->col.subtype)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * ResultsetGetTimestamp2
 * --------------------------------------------------------------------------------------------- */

OCI_Timestamp * ResultsetGetTimestamp2
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    GET_BY_NAME(rs, name, ResultsetGetTimestamp, OCI_Timestamp*, NULL)
}

/* --------------------------------------------------------------------------------------------- *
 * ResultsetGetInterval
 * --------------------------------------------------------------------------------------------- */

OCI_Interval * ResultsetGetInterval
(
    OCI_Resultset *rs,
    unsigned int   index
)
{
    GET_HANDLE
    (
        rs, index, OCI_Interval *, NULL, OCI_CDT_INTERVAL,

        IntervalInitialize(rs->stmt->con, (OCI_Interval *) def->obj,
                           (OCIInterval *) DefineGetData(def), def->col.subtype)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * ResultsetGetInterval2
 * --------------------------------------------------------------------------------------------- */

OCI_Interval * ResultsetGetInterval2
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    GET_BY_NAME(rs, name, ResultsetGetInterval, OCI_Interval*, NULL)
}

/* --------------------------------------------------------------------------------------------- *
 * ResultsetObject
 * --------------------------------------------------------------------------------------------- */

OCI_Object * ResultsetGetObject
(
    OCI_Resultset *rs,
    unsigned int   index
)
{
    GET_HANDLE
    (
        rs, index, OCI_Object *, NULL, OCI_CDT_OBJECT,

        ObjectInitialize(rs->stmt->con, (OCI_Object *) def->obj,
                         DefineGetData(def), def->col.typinf,
                         NULL, -1, TRUE)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * ResultsetGetObject2
 * --------------------------------------------------------------------------------------------- */

OCI_Object * ResultsetGetObject2
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    GET_BY_NAME(rs, name, ResultsetGetObject, OCI_Object*, NULL)
}

/* --------------------------------------------------------------------------------------------- *
 * ResultsetGetColl
 * --------------------------------------------------------------------------------------------- */

OCI_Coll * ResultsetGetColl
(
    OCI_Resultset *rs,
    unsigned int   index
)
{
    GET_HANDLE
    (
        rs, index, OCI_Coll *, NULL, OCI_CDT_COLLECTION,

        CollectionInitialize(rs->stmt->con, (OCI_Coll *) def->obj, DefineGetData(def), def->col.typinf)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * ResultsetGetColl2
 * --------------------------------------------------------------------------------------------- */

OCI_Coll * ResultsetGetColl2
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    GET_BY_NAME(rs, name, ResultsetGetColl, OCI_Coll*, NULL)
}

/* --------------------------------------------------------------------------------------------- *
 * ResultsetGetReference
 * --------------------------------------------------------------------------------------------- */

OCI_Ref * ResultsetGetReference
(
    OCI_Resultset *rs,
    unsigned int   index
)
{
    GET_HANDLE
    (
        rs, index, OCI_Ref *, NULL, OCI_CDT_REF,

        ReferenceInitialize(rs->stmt->con, def->col.typinf, (OCI_Ref *) def->obj, DefineGetData(def))
    )
}

/* --------------------------------------------------------------------------------------------- *
 * ResultsetGetReference2
 * --------------------------------------------------------------------------------------------- */

OCI_Ref * ResultsetGetReference2
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    GET_BY_NAME(rs, name, ResultsetGetReference, OCI_Ref*, NULL)
}

/* --------------------------------------------------------------------------------------------- *
 * ResultsetGetStatement
 * --------------------------------------------------------------------------------------------- */

OCI_Statement * ResultsetGetStatement
(
    OCI_Resultset *rs,
    unsigned int   index
)
{
    GET_HANDLE
    (
        rs, index, OCI_Statement *, NULL, OCI_CDT_CURSOR,

        StatementInitialize(rs->stmt->con,(OCI_Statement *) def->obj,
                            (OCIStmt *) DefineGetData(def), TRUE, def->col.name)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * ResultsetGetStatement2
 * --------------------------------------------------------------------------------------------- */

OCI_Statement * ResultsetGetStatement2
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    GET_BY_NAME(rs, name, ResultsetGetStatement, OCI_Statement*, NULL)
}

/* --------------------------------------------------------------------------------------------- *
 * ResultsetGetLob
 * --------------------------------------------------------------------------------------------- */

OCI_Lob * ResultsetGetLob
(
    OCI_Resultset *rs,
    unsigned int   index
)
{
    GET_HANDLE
    (
        rs, index, OCI_Lob *, NULL, OCI_CDT_LOB,

        LobInitialize(rs->stmt->con,(OCI_Lob *) def->obj,
                      (OCILobLocator *) DefineGetData(def),
                      def->col.subtype)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * ResultsetGetLob2
 * --------------------------------------------------------------------------------------------- */

OCI_Lob * ResultsetGetLob2
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    GET_BY_NAME(rs, name, ResultsetGetLob, OCI_Lob*, NULL)
}

/* --------------------------------------------------------------------------------------------- *
 * ResultsetGetFile
 * --------------------------------------------------------------------------------------------- */

OCI_File * ResultsetGetFile
(
    OCI_Resultset *rs,
    unsigned int   index
)
{
    GET_HANDLE
    (
        rs, index, OCI_File *, NULL, OCI_CDT_FILE,

        FileInitialize(rs->stmt->con, (OCI_File *) def->obj,
                       (OCILobLocator *) DefineGetData(def),
                       def->col.subtype)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * ResultsetGetFile2
 * --------------------------------------------------------------------------------------------- */

OCI_File * ResultsetGetFile2
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    GET_BY_NAME(rs, name, ResultsetGetFile, OCI_File*, NULL)
}

/* --------------------------------------------------------------------------------------------- *
 * ResultsetGetLong
 * --------------------------------------------------------------------------------------------- */

OCI_Long * ResultsetGetLong
(
    OCI_Resultset *rs,
    unsigned int   index
)
{
    GET_HANDLE
    (
        rs, index, OCI_Long *, NULL, OCI_CDT_LONG,

        (OCI_Long *) DefineGetData(def)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * ResultsetGetLong2
 * --------------------------------------------------------------------------------------------- */

OCI_Long * ResultsetGetLong2
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    GET_BY_NAME(rs, name, ResultsetGetLong, OCI_Long*, NULL)
}

/* --------------------------------------------------------------------------------------------- *
 * ResultsetGetDataSize
 * --------------------------------------------------------------------------------------------- */

unsigned int ResultsetGetDataSize
(
    OCI_Resultset *rs,
    unsigned int   index
)
{
    ENTER_FUNC
    (
        /* returns */ unsigned int, 0,
        /* context */ OCI_IPC_RESULTSET, rs
    )

    CHECK_PTR(OCI_IPC_RESULTSET, rs)
    CHECK_BOUND(index, 1, rs->nb_defs)

    OCI_Define* def = DefineGet(rs, index);
    CHECK_NULL(def)

    unsigned int size = 0;

    if (DefineIsDataNotNull(def))
    {
        ub2* lens = (ub2 *)def->buf.lens;

        if (lens)
        {
            size = lens[rs->row_cur - 1];

            if (OCI_CDT_TEXT == def->col.datatype)
            {
                size /= sizeof(otext);
            }
        }
    }

    SET_RETVAL(size)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * ResultsetGetDataSize2
 * --------------------------------------------------------------------------------------------- */

unsigned int ResultsetGetDataSize2
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    ENTER_FUNC
    (
        /* returns */ unsigned int, 0,
        /* context */ OCI_IPC_RESULTSET, rs
    )

    CHECK_PTR(OCI_IPC_RESULTSET, rs)
    CHECK_PTR(OCI_IPC_STRING, name)

    const int index = DefineGetIndex(rs, name);
    CHECK(index >= 0)

    SET_RETVAL(ResultsetGetDataSize(rs, (unsigned int)index))
 
    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * ResultsetIsNull
 * --------------------------------------------------------------------------------------------- */

boolean ResultsetIsNull
(
    OCI_Resultset *rs,
    unsigned int   index
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_RESULTSET, rs
    )

    CHECK_PTR(OCI_IPC_RESULTSET, rs)
    CHECK_BOUND(index, 1, rs->nb_defs)

    OCI_Define *def = DefineGet(rs, index);
    CHECK_NULL(def)

    SET_RETVAL(!DefineIsDataNotNull(def))

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * ResultsetIsNull2
 * --------------------------------------------------------------------------------------------- */

boolean ResultsetIsNull2
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, TRUE,
        /* context */ OCI_IPC_RESULTSET, rs
    )

    CHECK_PTR(OCI_IPC_RESULTSET, rs)
    CHECK_PTR(OCI_IPC_STRING, name)

    const int index = DefineGetIndex(rs, name);
    CHECK(index >= 0)

    SET_RETVAL(ResultsetIsNull(rs, (unsigned int)index))

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * ResultsetResultsetGetStatement
 * --------------------------------------------------------------------------------------------- */

OCI_Statement * ResultsetResultsetGetStatement
(
    OCI_Resultset *rs
)
{
    GET_PROP
    (
        OCI_Statement *, NULL,
        OCI_IPC_RESULTSET, rs,
        stmt
    )
}

/* --------------------------------------------------------------------------------------------- *
 * ResultsetGetDataLength
 * --------------------------------------------------------------------------------------------- */

unsigned int ResultsetGetDataLength
(
    OCI_Resultset *rs,
    unsigned int   index
)
{
    ENTER_FUNC
    (
        /* returns */ unsigned int, 0,
        /* context */ OCI_IPC_RESULTSET, rs
    )

    CHECK_PTR(OCI_IPC_RESULTSET, rs)
    CHECK_BOUND(index, 1, rs->nb_defs)

    OCI_Define *def = DefineGet(rs, index);
    CHECK_NULL(def)
    CHECK(rs->row_cur > 0)
   
    SET_RETVAL((unsigned int)((ub2 *)def->buf.lens)[rs->row_cur - 1])

    EXIT_FUNC()
}
