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
#include "stringutils.h"
#include "timestamp.h"

static unsigned int SeekModeValues[] =
{
    OCI_SFD_ABSOLUTE,
    OCI_SFD_RELATIVE
};

#define MATCHING_TYPE(def, type) \
                                 \
    ((def) && OcilibDefineIsDataNotNull(def) && ((type) == (def)->col.datatype))

#define GET_BY_NAME(rs, name, func, type, res)   \
                                                 \
    ENTER_FUNC(type, res, OCI_IPC_RESULTSET, rs) \
                                                 \
    CHECK_PTR(OCI_IPC_RESULTSET, rs)             \
    CHECK_PTR(OCI_IPC_STRING,    name)           \
                                                 \
    int index = OcilibDefineGetIndex(rs, name);  \
    CHECK(index >= 0)                            \
                                                 \
    OCI_Error *err = OcilibErrorGet(TRUE, TRUE); \
                                                 \
    type tmp = func(rs, (unsigned int) index);   \
                                                 \
    CHECK_ERROR(err)                             \
                                                 \
    SET_RETVAL(tmp)                              \
                                                 \
    EXIT_FUNC()

#define GET_NUMBER(rs, index, num_type, type, res)          \
                                                            \
    ENTER_FUNC(type, res, OCI_IPC_RESULTSET, rs)            \
                                                            \
    CHECK_PTR(OCI_IPC_RESULTSET, rs)                        \
    CHECK_BOUND(index, 1, (rs)->nb_defs)                    \
                                                            \
    type tmp = res;                                         \
                                                            \
    CHECK(OcilibDefineGetNumber(rs, index, &tmp, num_type)) \
                                                            \
    SET_RETVAL(tmp)                                         \
                                                            \
    EXIT_FUNC()

#define GET_HANDLE(rs, index, type, res, lib_type, func) \
                                                         \
    ENTER_FUNC(type, res, OCI_IPC_RESULTSET, rs)         \
                                                         \
    OCI_Define *def = NULL;                              \
                                                         \
    CHECK_PTR(OCI_IPC_RESULTSET, rs)                     \
    CHECK_BOUND(index, 1, (rs)->nb_defs)                 \
                                                         \
    def = OcilibDefineGet(rs, index);                    \
    CHECK_NULL(def)                                      \
                                                         \
    type tmp = res;                                      \
                                                         \
    if (MATCHING_TYPE(def, lib_type))                    \
    {                                                    \
        def->obj = func;                                 \
        CHECK_NULL(def->obj)                             \
        tmp = def->obj;                                  \
    }                                                    \
                                                         \
    SET_RETVAL(tmp)                                      \
                                                         \
    EXIT_FUNC()

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Resultset * OcilibResultsetCreate
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

            CHECK(OcilibColumnRetrieveInfo(&def->col, rs->stmt->con,
                                           rs->stmt, rs->stmt->stmt,
                                           i + 1, OCI_DESC_RESULTSET))

            /* mapping to OCILIB internal types */

            CHECK(OcilibColumnMapInfo(&def->col, rs->stmt))

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

                CHECK(OcilibDefineAlloc( def))
                CHECK(OcilibDefineDef(def, i + 1))
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
            def->col.name     = OcilibStringDuplicate(bnd->name);
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

            CHECK(OcilibColumnMapInfo(&def->col, rs->stmt))
            CHECK(OcilibDefineAlloc(def))
        }
    }

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE)
        {
            OcilibResultsetFree(rs);
            rs = NULL;
        }

        SET_RETVAL(rs)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetExpandStrings
 * --------------------------------------------------------------------------------------------- */

static boolean OcilibResultsetExpandStrings
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

                OcilibStringUTF16ToUTF32(tmpbuf, tmpbuf, (def->col.bufsize / sizeof(otext) ) -1);
            }
        }
    }

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetFetchPieces
 * --------------------------------------------------------------------------------------------- */

static boolean OcilibResultsetFetchPieces
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
                def->buf.data[j] = OcilibLongInitialize(rs->stmt,
                                                        (OCI_Long *)def->buf.data[j],
                                                        def, def->col.subtype);
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

                    lg->buffer = (ub1 *)OcilibMemoryRealloc(lg->buffer, (size_t) OCI_IPC_LONG_BUFFER,
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
            THROW(OcilibExceptionOCI, rs->stmt->con->err, rs->fetch_status)
        }
        else if (OCI_SUCCESS_WITH_INFO == rs->fetch_status)
        {
            OcilibExceptionOCI(&call_context, rs->stmt->con->err, rs->fetch_status);
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
                        OcilibStringUTF16ToUTF32(lg->buffer, lg->buffer, len);
                    }
                }
            }
        }
    }

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetClearFetchedObjectInstances
 * --------------------------------------------------------------------------------------------- */

static boolean OcilibResultsetClearFetchedObjectInstances(OCI_Resultset *rs)
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
 * OcilibResultsetFetchData
 * --------------------------------------------------------------------------------------------- */

static boolean OcilibResultsetFetchData
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

    CHECK(OcilibResultsetClearFetchedObjectInstances(rs))

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
        THROW(OcilibExceptionOCI, rs->stmt->con->err, rs->fetch_status)
    }
    else if (OCI_SUCCESS_WITH_INFO == rs->fetch_status)
    {
        OcilibExceptionOCI(&call_context, rs->stmt->con->err, rs->fetch_status);
    }
    else if (OCI_NEED_DATA == rs->fetch_status)
    {
        /* need to do a piecewise fetch */
        CHECK(OcilibResultsetFetchPieces(rs))
    }

    /* check string buffer for Unicode builds that need buffer expansion */

    if (Env.use_wide_char_conv)
    {
        CHECK(OcilibResultsetExpandStrings(rs))
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
        row_count   = OcilibStatementGetAffectedRows(rs->stmt);
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
 * OcilibResultsetFetchCustom
 * --------------------------------------------------------------------------------------------- */

static boolean OcilibResultsetFetchCustom
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

    switch (mode)
    {
        case OCI_SFD_RELATIVE:
        {
            const boolean isOutOfBound = ((offset > 0) && (rs->eof)) ||
                                         ((offset < 0) && (rs->bof)) ||
                                         ((offset == 0));

            CHECK(isOutOfBound == FALSE)

            offset += rs->row_abs;
            break;
        }
        case OCI_SFD_ABSOLUTE:
        {
            CHECK(offset != 0)
            break;
        }
        default:
        {
            CHECK(FALSE)
            break;
        }
    }

    rs->row_abs = 1;
    rs->row_cur = 1;

    CHECK(OcilibResultsetFetchData(rs, OCI_SFD_ABSOLUTE, offset))

    rs->row_abs = offset;

    rs->bof = FALSE;
    rs->eof = FALSE;

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetInitialize
 * --------------------------------------------------------------------------------------------- */

boolean OcilibResultsetInitialize
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
 * OcilibResultsetFree
 * --------------------------------------------------------------------------------------------- */

boolean OcilibResultsetFree
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
                OcilibStatementDispose((OCI_Statement *)def->obj);
                FREE(def->obj)
            }
            else
            {
                OcilibFreeObjectFromType(def->obj, def->col.datatype);
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
                        OcilibMemoryFreeHandle((dvoid *)def->buf.data[j], (ub4)def->col.handletype);
                    }
                }
            }
            else
            {
                OcilibMemoryFreeDescriptorArray((dvoid *)def->buf.data,
                                                (ub4)def->col.handletype,
                                                (ub4)def->buf.count);
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

                    OcilibLongFree((OCI_Long *)def->buf.data[j]);
                }
            }
        }

        /* free object instances from object cache */

        CHECK(OcilibResultsetClearFetchedObjectInstances(rs))

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
        OcilibHashFree(rs->map);
    }

    /* free defines (column array) */

    FREE(rs->defs)

    OcilibErrorResetSource(NULL, rs);

    FREE(rs)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetFetchPrev
 * --------------------------------------------------------------------------------------------- */

boolean OcilibResultsetFetchPrev
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

            CHECK(OcilibResultsetFetchData(rs, OCI_SFD_RELATIVE, offset))

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
 * OcilibResultsetFetchNext
 * --------------------------------------------------------------------------------------------- */

boolean OcilibResultsetFetchNext
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
                CHECK(OcilibResultsetFetchData(rs, OCI_SFD_NEXT, 0))

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
                OcilibResultsetExpandStrings(rs);
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
 * OcilibResultsetFetchFirstg
 * --------------------------------------------------------------------------------------------- */

boolean OcilibResultsetFetchFirst
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

    CHECK(OcilibResultsetFetchData(rs, OCI_SFD_FIRST, 0))

    CHECK(!rs->bof)

    SET_SUCCESS()

#endif

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetFetchLast
 * --------------------------------------------------------------------------------------------- */

boolean OcilibResultsetFetchLast
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

    result = OcilibResultsetFetchData(rs, OCI_SFD_LAST, 0);

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
 * OcilibResultsetFetchSeek
 * --------------------------------------------------------------------------------------------- */

boolean OcilibResultsetFetchSeek
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

    CHECK(OcilibResultsetFetchCustom(rs, (int)mode, offset))

    SET_SUCCESS()

#else

    OCI_NOT_USED(mode)
    OCI_NOT_USED(offset)

#endif

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetRowCount
 * --------------------------------------------------------------------------------------------- */

unsigned int OcilibResultsetGetRowCount
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
 * OcilibResultsetGetCurrentRow
 * --------------------------------------------------------------------------------------------- */

unsigned int OcilibResultsetGetCurrentRow
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
 * OcilibResultsetGetColumnCount
 * --------------------------------------------------------------------------------------------- */

unsigned int OcilibResultsetGetColumnCount
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
 * OcilibResultsetGetColumn
 * --------------------------------------------------------------------------------------------- */

OCI_Column * OcilibResultsetGetColumn
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
 * OcilibResultsetGetColumn2
 * --------------------------------------------------------------------------------------------- */

OCI_Column * OcilibResultsetGetColumn2
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
    CHECK_PTR(OCI_IPC_STRING,    name)

    const int index = OcilibDefineGetIndex(rs, name);
    CHECK(index >= 0)

    SET_RETVAL(&rs->defs[index - 1].col)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetColumnIndex
 * --------------------------------------------------------------------------------------------- */

unsigned int OcilibResultsetGetColumnIndex
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
    CHECK_PTR(OCI_IPC_STRING,    name)

    const int index = OcilibDefineGetIndex(rs, name);
    CHECK(index >= 0)

    SET_RETVAL((unsigned int) index)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 *Ocilib ResultsetSetStructNumericType
 * --------------------------------------------------------------------------------------------- */

boolean OcilibResultsetSetStructNumericType
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
    CHECK_BOUND(index, 1, rs->nb_defs)
    CHECK_COMPAT(OCI_CDT_NUMERIC == rs->defs[index - 1].col.datatype);

    rs->defs[index-1].col.struct_subtype = (ub2) type;

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetSetStructNumericType2
 * --------------------------------------------------------------------------------------------- */

boolean OcilibResultsetSetStructNumericType2
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
    CHECK_PTR(OCI_IPC_STRING,    name)

    const int index = OcilibDefineGetIndex(rs, name);
    CHECK(index >= 0)

    CHECK(OcilibResultsetSetStructNumericType(rs, (unsigned int)index, type))

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetStruct
 * --------------------------------------------------------------------------------------------- */

boolean OcilibResultsetGetStruct
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

    char    *ptr = NULL;
    boolean *inds = NULL;

    CHECK_PTR(OCI_IPC_RESULTSET, rs)
    CHECK_PTR(OCI_IPC_VOID,      row_struct)

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

            const boolean is_not_null = OcilibDefineIsDataNotNull(def);

            if (i == 1)
            {
                OcilibColumnGetAttributeInfo(&def->col, rs->nb_defs, i - 1, &size1, &align);
            }
            else
            {
                size1 = size2;
            }

            OcilibColumnGetAttributeInfo(&rs->defs[i].col, rs->nb_defs, i, &size2, &align);

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

                        OcilibDefineGetNumber(rs, i, ptr, type);

                        break;
                    }
                    case OCI_CDT_TEXT:
                    {
                        *((otext **) ptr) =  (otext * )OcilibResultsetGetString(rs, i);
                        break;
                    }
                    case OCI_CDT_RAW:
                    {
                        *((void **) ptr) = OcilibDefineGetData(def);
                        break;
                    }
                    case OCI_CDT_LONG:
                    {
                        *((OCI_Long **) ptr) = OcilibResultsetGetLong(rs, i);
                        break;
                    }
                    case OCI_CDT_DATETIME:
                    {
                        *((OCI_Date **) ptr) = OcilibResultsetGetDate(rs, i);
                        break;
                    }
                    case OCI_CDT_CURSOR:
                    {
                        *((OCI_Statement **) ptr) = OcilibResultsetGetStatement(rs, i);
                        break;
                    }
                    case OCI_CDT_LOB:
                    {
                        *((OCI_Lob **) ptr) = OcilibResultsetGetLob(rs, i);
                        break;
                    }
                    case OCI_CDT_FILE:
                    {
                        *((OCI_File **) ptr) = OcilibResultsetGetFile(rs, i);
                        break;
                    }
                    case OCI_CDT_TIMESTAMP:
                    {
                        *((OCI_Timestamp **) ptr) = OcilibResultsetGetTimestamp(rs, i);

                        break;
                    }
                    case OCI_CDT_INTERVAL:
                    {
                        *((OCI_Interval **) ptr) = OcilibResultsetGetInterval(rs, i);
                        break;
                    }
                    case OCI_CDT_OBJECT:
                    {
                        *((OCI_Object **) ptr) = OcilibResultsetGetObject(rs, i);
                        break;
                    }
                    case OCI_CDT_COLLECTION:
                    {
                        *((OCI_Coll **) ptr) = OcilibResultsetGetColl(rs, i);
                        break;
                    }
                    case OCI_CDT_REF:
                    {
                        *((OCI_Ref **) ptr) = OcilibResultsetGetReference(rs, i);
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
* OcilibResultsetGetNumber
* --------------------------------------------------------------------------------------------- */

OCI_Number * OcilibResultsetGetNumber
(
    OCI_Resultset *rs,
    unsigned int   index
)
{
    GET_HANDLE
    (
        rs, index, OCI_Number *, NULL, OCI_CDT_NUMERIC,

        OcilibNumberInitialize
        (
            rs->stmt->con,
            (OCI_Number *) def->obj,
            (OCINumber *)OcilibDefineGetData(def)
        )
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetNumber2
 * --------------------------------------------------------------------------------------------- */

OCI_Number * OcilibResultsetGetNumber2
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    GET_BY_NAME(rs, name, OcilibResultsetGetNumber, OCI_Number *, NULL)
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetShort
 * --------------------------------------------------------------------------------------------- */

short OcilibResultsetGetShort
(
    OCI_Resultset *rs,
    unsigned int   index
)
{
    GET_NUMBER(rs, index, OCI_NUM_SHORT, short, 0)
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetShort2
 * --------------------------------------------------------------------------------------------- */

short OcilibResultsetGetShort2
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    GET_BY_NAME(rs, name, OcilibResultsetGetShort, short, 0)
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetUnsignedShort
 * --------------------------------------------------------------------------------------------- */

unsigned short OcilibResultsetGetUnsignedShort
(
    OCI_Resultset *rs,
    unsigned int   index
)
{
    GET_NUMBER(rs, index, OCI_NUM_USHORT, unsigned short, 0)
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetUnsignedShort2
 * --------------------------------------------------------------------------------------------- */

unsigned short OcilibResultsetGetUnsignedShort2
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    GET_BY_NAME(rs, name, OcilibResultsetGetUnsignedShort, unsigned short, 0)
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetInt
 * --------------------------------------------------------------------------------------------- */

int OcilibResultsetGetInt
(
    OCI_Resultset *rs,
    unsigned int   index
)
{
    GET_NUMBER(rs, index, OCI_NUM_INT, int, 0)
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetInt2
 * --------------------------------------------------------------------------------------------- */

int OcilibResultsetGetInt2
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    GET_BY_NAME(rs, name, OcilibResultsetGetInt, int, 0)
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetUnsignedInt
 * --------------------------------------------------------------------------------------------- */

unsigned int OcilibResultsetGetUnsignedInt
(
    OCI_Resultset *rs,
    unsigned int   index
)
{
    GET_NUMBER(rs, index, OCI_NUM_UINT, unsigned int, 0)
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetUnsignedInt2
 * --------------------------------------------------------------------------------------------- */

unsigned int OcilibResultsetGetUnsignedInt2
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    GET_BY_NAME(rs, name, OcilibResultsetGetUnsignedInt, unsigned int, 0)
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetBigInt
 * --------------------------------------------------------------------------------------------- */

big_int OcilibResultsetGetBigInt
(
    OCI_Resultset *rs,
    unsigned int   index
)
{
    GET_NUMBER(rs, index, OCI_NUM_BIGINT, big_int, 0)
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetBigInt2
 * --------------------------------------------------------------------------------------------- */

big_int OcilibResultsetGetBigInt2
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    GET_BY_NAME(rs, name, OcilibResultsetGetBigInt, big_int, 0)
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetUnsignedBigInt
 * --------------------------------------------------------------------------------------------- */

big_uint OcilibResultsetGetUnsignedBigInt
(
    OCI_Resultset *rs,
    unsigned int   index
)
{
    GET_NUMBER(rs, index, OCI_NUM_BIGUINT, big_uint, 0)
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetUnsignedInt2
 * --------------------------------------------------------------------------------------------- */

big_uint OcilibResultsetGetUnsignedBigInt2
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    GET_BY_NAME(rs, name, OcilibResultsetGetUnsignedBigInt, big_uint, 0)
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetString
 * --------------------------------------------------------------------------------------------- */

const otext * OcilibResultsetGetString
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

    def = OcilibDefineGet(rs, index);
    CHECK_NULL(def)

    otext *result = NULL;

    if (OcilibDefineIsDataNotNull(def))
    {
        void *data = NULL;

        if (OCI_CDT_TEXT == def->col.datatype)
        {
            result = (otext *)OcilibDefineGetData(def);

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
            OCI_Long *lg = OcilibResultsetGetLong(rs, index);

            if (lg)
            {
                result = (otext *)OcilibLongGetBuffer(lg);
            }
        }
        else
        {
            OCI_Error *err = OcilibErrorGet(TRUE, TRUE);

            unsigned int bufsize   = OCI_SIZE_TMP_CVT;
            unsigned int data_size = 0;

            switch (def->col.datatype)
            {
                case OCI_CDT_NUMERIC:
                {
                    data = OcilibDefineGetData(def);
                    break;
                }
                case OCI_CDT_DATETIME:
                {
                    data = OcilibResultsetGetDate(rs, index);
                    break;
                }
                case OCI_CDT_TIMESTAMP:
                {
                    data = OcilibResultsetGetTimestamp(rs, index);
                    break;
                }
                case OCI_CDT_INTERVAL:
                {
                    data = OcilibResultsetGetInterval(rs, index);
                    break;
                }
                case OCI_CDT_RAW:
                {
                    data      = OcilibDefineGetData(def);
                    data_size = ((ub2*)def->buf.lens)[def->rs->row_cur - 1];
                    break;
                }
                case OCI_CDT_REF:
                {
                    data = OcilibResultsetGetReference(rs, index);
                    break;
                }
                case OCI_CDT_LONG:
                {
                    OCI_Long *lg = OcilibResultsetGetLong(rs, index);

                    if (lg)
                    {
                        bufsize = OcilibLongGetSize(lg);

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
                    OCI_Lob *lob = OcilibResultsetGetLob(rs, index);

                    if (lob)
                    {
                        bufsize = (unsigned int)OcilibLobGetLength(lob);

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

                    OCI_File * file = OcilibResultsetGetFile(rs, index);

                    if (file)
                    {
                        bufsize = (unsigned int) ostrlen(OTEXT("/"));

                        OcilibFileGetInfo(file);

                        bufsize += (unsigned int) (file->dir ? ostrlen(file->dir) : 0);
                        bufsize += (unsigned int) (file->name ? ostrlen(file->name) : 0);
                    }

                    data = file;
                    break;
                }
                case OCI_CDT_OBJECT:
                {
                    OCI_Object *obj = OcilibResultsetGetObject(rs, index);

                    if (obj)
                    {
                        CHECK(OcilibObjectToString(obj, &bufsize, NULL))
                    }

                    data = obj;
                    break;
                }
                case OCI_CDT_COLLECTION:
                {
                    OCI_Coll *coll = OcilibResultsetGetColl(rs, index);

                    if (coll)
                    {
                        CHECK(OcilibCollectionToString(coll, &bufsize, NULL))
                    }

                    data = coll;
                    break;
                }
                case OCI_CDT_CURSOR:
                {
                    OCI_Statement *stmt = OcilibResultsetGetStatement(rs, index);

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

            CHECK(OcilibStringRequestBuffer(&def->buf.tmpbuf, &def->buf.tmpsize, bufsize))

            CHECK(OcilibStringGetFromType(rs->stmt->con, &def->col, data, data_size,
                                          def->buf.tmpbuf, def->buf.tmpsize, FALSE))

            CHECK(NULL == err || OCI_UNKNOWN == err->type)

            result = def->buf.tmpbuf;
        }
    }

    SET_RETVAL(result)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetString2
 * --------------------------------------------------------------------------------------------- */

const otext * OcilibResultsetGetString2
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    GET_BY_NAME(rs, name, OcilibResultsetGetString, const otext *, NULL)
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetRaw
 * --------------------------------------------------------------------------------------------- */

unsigned int OcilibResultsetGetRaw
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
    CHECK_PTR(OCI_IPC_VOID,      buffer);
    CHECK_BOUND(index, 1, rs->nb_defs)

    OCI_Define* def = OcilibDefineGet(rs, index);
    CHECK_NULL(def)

    unsigned int result = 0;

    if (MATCHING_TYPE(def, OCI_CDT_RAW))
    {
        unsigned int size = (unsigned int)(ub2)((ub2*)def->buf.lens)[def->rs->row_cur - 1];

        result = size < len ? size : len;

        /* for RAWs, we copy the data in the destination buffer instead of
        returning internal buffer as we do for strings */

        memcpy(buffer, OcilibDefineGetData(def), (size_t)result);
    }

    SET_RETVAL(result)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetRaw2
 * --------------------------------------------------------------------------------------------- */

unsigned int OcilibResultsetGetRaw2
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
    CHECK_PTR(OCI_IPC_VOID,      buffer)
    CHECK_PTR(OCI_IPC_STRING,    name)

    const int index = OcilibDefineGetIndex(rs, name);
    CHECK(index >= 0)

    OCI_Define* def = OcilibDefineGet(rs, index);
    CHECK_NULL(def)

    unsigned int result = 0;

    if (MATCHING_TYPE(def, OCI_CDT_RAW))
    {
        unsigned int size = (unsigned int)(ub2)((ub2*)def->buf.lens)[def->rs->row_cur - 1];

        result = size < len ? size : len;

        /* for RAWs, we copy the data in the destination buffer instead of
        returning internal buffer as we do for strings */

        memcpy(buffer, OcilibDefineGetData(def), (size_t)result);
    }

    SET_RETVAL(result)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetDouble
 * --------------------------------------------------------------------------------------------- */

double OcilibResultsetGetDouble
(
    OCI_Resultset *rs,
    unsigned int   index
)
{
    GET_NUMBER(rs, index, OCI_NUM_DOUBLE, double, 0.0)
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetDouble2
 * --------------------------------------------------------------------------------------------- */

double OcilibResultsetGetDouble2
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    GET_BY_NAME(rs, name, OcilibResultsetGetDouble, double, 0.0)
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetFloat
 * --------------------------------------------------------------------------------------------- */

float OcilibResultsetGetFloat
(
    OCI_Resultset *rs,
    unsigned int   index
)
{
    GET_NUMBER(rs, index, OCI_NUM_FLOAT, float, 0.0f)
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetFloat2
 * --------------------------------------------------------------------------------------------- */

float OcilibResultsetGetFloat2
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    GET_BY_NAME(rs, name, OcilibResultsetGetFloat, float, 0.0f)
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetDate
 * --------------------------------------------------------------------------------------------- */

OCI_Date * OcilibResultsetGetDate
(
    OCI_Resultset *rs,
    unsigned int   index
)
{
    GET_HANDLE
    (
        rs, index, OCI_Date *, NULL, OCI_CDT_DATETIME,

        OcilibDateInitialize(rs->stmt->con, (OCI_Date *) def->obj,
                             (OCIDate *)OcilibDefineGetData(def), FALSE,
                             (SQLT_DAT == def->col.libcode))
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetDate2
 * --------------------------------------------------------------------------------------------- */

OCI_Date * OcilibResultsetGetDate2
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    GET_BY_NAME(rs, name, OcilibResultsetGetDate, OCI_Date *, NULL)
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetTimestamp
 * --------------------------------------------------------------------------------------------- */

OCI_Timestamp * OcilibResultsetGetTimestamp
(
    OCI_Resultset *rs,
    unsigned int   index
)
{
    GET_HANDLE
    (
        rs, index, OCI_Timestamp *, NULL, OCI_CDT_TIMESTAMP,

        OcilibTimestampInitialize(rs->stmt->con, (OCI_Timestamp *) def->obj,
                                  (OCIDateTime *)OcilibDefineGetData(def), def->col.subtype)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetTimestamp2
 * --------------------------------------------------------------------------------------------- */

OCI_Timestamp * OcilibResultsetGetTimestamp2
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    GET_BY_NAME(rs, name, OcilibResultsetGetTimestamp, OCI_Timestamp*, NULL)
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetInterval
 * --------------------------------------------------------------------------------------------- */

OCI_Interval * OcilibResultsetGetInterval
(
    OCI_Resultset *rs,
    unsigned int   index
)
{
    GET_HANDLE
    (
        rs, index, OCI_Interval *, NULL, OCI_CDT_INTERVAL,

        OcilibIntervalInitialize(rs->stmt->con, (OCI_Interval *) def->obj,
                                 (OCIInterval *)OcilibDefineGetData(def), def->col.subtype)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetInterval2
 * --------------------------------------------------------------------------------------------- */

OCI_Interval * OcilibResultsetGetInterval2
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    GET_BY_NAME(rs, name, OcilibResultsetGetInterval, OCI_Interval*, NULL)
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetObject
 * --------------------------------------------------------------------------------------------- */

OCI_Object * OcilibResultsetGetObject
(
    OCI_Resultset *rs,
    unsigned int   index
)
{
    GET_HANDLE
    (
        rs, index, OCI_Object *, NULL, OCI_CDT_OBJECT,

        OcilibObjectInitialize(rs->stmt->con, (OCI_Object *) def->obj,
                               OcilibDefineGetData(def), def->col.typinf,
                               NULL, -1, TRUE)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetObject2
 * --------------------------------------------------------------------------------------------- */

OCI_Object * OcilibResultsetGetObject2
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    GET_BY_NAME(rs, name, OcilibResultsetGetObject, OCI_Object*, NULL)
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetColl
 * --------------------------------------------------------------------------------------------- */

OCI_Coll * OcilibResultsetGetColl
(
    OCI_Resultset *rs,
    unsigned int   index
)
{
    GET_HANDLE
    (
        rs, index, OCI_Coll *, NULL, OCI_CDT_COLLECTION,

        OcilibCollectionInitialize(rs->stmt->con, (OCI_Coll *) def->obj,
                                   OcilibDefineGetData(def), def->col.typinf)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetColl2
 * --------------------------------------------------------------------------------------------- */

OCI_Coll * OcilibResultsetGetColl2
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    GET_BY_NAME(rs, name, OcilibResultsetGetColl, OCI_Coll*, NULL)
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetReference
 * --------------------------------------------------------------------------------------------- */

OCI_Ref * OcilibResultsetGetReference
(
    OCI_Resultset *rs,
    unsigned int   index
)
{
    GET_HANDLE
    (
        rs, index, OCI_Ref *, NULL, OCI_CDT_REF,

        OcilibReferenceInitialize(rs->stmt->con, def->col.typinf, (OCI_Ref *) def->obj,
                                  OcilibDefineGetData(def))
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetReference2
 * --------------------------------------------------------------------------------------------- */

OCI_Ref * OcilibResultsetGetReference2
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    GET_BY_NAME(rs, name, OcilibResultsetGetReference, OCI_Ref*, NULL)
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetStatement
 * --------------------------------------------------------------------------------------------- */

OCI_Statement * OcilibResultsetGetStatement
(
    OCI_Resultset *rs,
    unsigned int   index
)
{
    GET_HANDLE
    (
        rs, index, OCI_Statement *, NULL, OCI_CDT_CURSOR,

        OcilibStatementInitialize
        (
            rs->stmt->con,
            (OCI_Statement *) def->obj,
            (OCIStmt *)OcilibDefineGetData(def),
            TRUE,
            def->col.name
        )
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetStatement2
 * --------------------------------------------------------------------------------------------- */

OCI_Statement * OcilibResultsetGetStatement2
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    GET_BY_NAME(rs, name, OcilibResultsetGetStatement, OCI_Statement*, NULL)
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetLob
 * --------------------------------------------------------------------------------------------- */

OCI_Lob * OcilibResultsetGetLob
(
    OCI_Resultset *rs,
    unsigned int   index
)
{
    GET_HANDLE
    (
        rs, index, OCI_Lob *, NULL, OCI_CDT_LOB,

        OcilibLobInitialize
        (
            rs->stmt->con,
            (OCI_Lob *) def->obj,
            (OCILobLocator *)OcilibDefineGetData(def),
            def->col.subtype
        )
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetLob2
 * --------------------------------------------------------------------------------------------- */

OCI_Lob * OcilibResultsetGetLob2
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    GET_BY_NAME(rs, name, OcilibResultsetGetLob, OCI_Lob*, NULL)
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetFile
 * --------------------------------------------------------------------------------------------- */

OCI_File * OcilibResultsetGetFile
(
    OCI_Resultset *rs,
    unsigned int   index
)
{
    GET_HANDLE
    (
        rs, index, OCI_File *, NULL, OCI_CDT_FILE,

        OcilibFileInitialize
        (
            rs->stmt->con,
            (OCI_File *) def->obj,
            (OCILobLocator *) OcilibDefineGetData(def),
            def->col.subtype
        )
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetFile2
 * --------------------------------------------------------------------------------------------- */

OCI_File * OcilibResultsetGetFile2
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    GET_BY_NAME(rs, name, OcilibResultsetGetFile, OCI_File*, NULL)
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetLong
 * --------------------------------------------------------------------------------------------- */

OCI_Long * OcilibResultsetGetLong
(
    OCI_Resultset *rs,
    unsigned int   index
)
{
    GET_HANDLE
    (
        rs, index, OCI_Long *, NULL, OCI_CDT_LONG,

        (OCI_Long *)OcilibDefineGetData(def)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetLong2
 * --------------------------------------------------------------------------------------------- */

OCI_Long * OcilibResultsetGetLong2
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    GET_BY_NAME(rs, name, OcilibResultsetGetLong, OCI_Long*, NULL)
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetDataSize
 * --------------------------------------------------------------------------------------------- */

unsigned int OcilibResultsetGetDataSize
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

    OCI_Define* def = OcilibDefineGet(rs, index);
    CHECK_NULL(def)

    unsigned int size = 0;

    if (OcilibDefineIsDataNotNull(def))
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
 * OcilibResultsetGetDataSize2
 * --------------------------------------------------------------------------------------------- */

unsigned int OcilibResultsetGetDataSize2
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
    CHECK_PTR(OCI_IPC_STRING,    name)

    const int index = OcilibDefineGetIndex(rs, name);
    CHECK(index >= 0)

    SET_RETVAL(OcilibResultsetGetDataSize(rs, (unsigned int)index))

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetIsNull
 * --------------------------------------------------------------------------------------------- */

boolean OcilibResultsetIsNull
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

    OCI_Define *def = OcilibDefineGet(rs, index);
    CHECK_NULL(def)

    SET_RETVAL(!OcilibDefineIsDataNotNull(def))

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetIsNull2
 * --------------------------------------------------------------------------------------------- */

boolean OcilibResultsetIsNull2
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
    CHECK_PTR(OCI_IPC_STRING,    name)

    const int index = OcilibDefineGetIndex(rs, name);
    CHECK(index >= 0)

    SET_RETVAL(OcilibResultsetIsNull(rs, (unsigned int)index))

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetResultsetGetStatement
 * --------------------------------------------------------------------------------------------- */

OCI_Statement * OcilibResultsetResultsetGetStatement
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
 * OcilibResultsetGetDataLength
 * --------------------------------------------------------------------------------------------- */

unsigned int OcilibResultsetGetDataLength
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

    OCI_Define *def = OcilibDefineGet(rs, index);
    CHECK_NULL(def)
    CHECK(rs->row_cur > 0)

    SET_RETVAL((unsigned int)((ub2 *)def->buf.lens)[rs->row_cur - 1])

    EXIT_FUNC()
}
