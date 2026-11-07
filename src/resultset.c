/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2026 Vincent ROGIER <vince.rogier@ocilib.net>
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
#include "json.h"
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
#include "vector.h"
#include "xmltype.h"

static OCI_UInt32 SeekModeValues[] =
{
    OCI_SFD_ABSOLUTE,
    OCI_SFD_RELATIVE
};

#define MATCHING_TYPE(def, type) \
                                 \
        ((def) && OcilibDefineIsDataNotNull(def) && ((type) == (def)->col.datatype))

#define GET_BY_NAME(rs, name, func, type, res)                                \
                                                                              \
        ENTER_FUNC(type, res, OCI_IPC_RESULTSET, rs);                         \
                                                                              \
        CHECK_PTR(OCI_IPC_RESULTSET, rs);                                     \
        CHECK_PTR(OCI_IPC_STRING,    name);                                   \
                                                                              \
        OCI_Int32 index = OcilibDefineGetIndex(rs, name);                     \
        CHECK(index >= 0);                                                    \
                                                                              \
        OCI_Error *err = OcilibErrorGet(ocilib_call_context.env, TRUE, TRUE); \
                                                                              \
        type tmp = func(rs, (OCI_UInt32) index);                              \
                                                                              \
        CHECK_ERROR(err);                                                     \
                                                                              \
        SET_RETVAL(tmp);                                                      \
                                                                              \
        EXIT_FUNC();

#define GET_NUMBER(rs, index, num_type, type, res)               \
                                                                 \
        ENTER_FUNC(type, res, OCI_IPC_RESULTSET, rs);            \
                                                                 \
        CHECK_PTR(OCI_IPC_RESULTSET, rs);                        \
        CHECK_INDEX(index, (rs)->nb_defs);                       \
                                                                 \
        type tmp = res;                                          \
                                                                 \
        CHECK(OcilibDefineGetNumber(rs, index, &tmp, num_type)); \
                                                                 \
        SET_RETVAL(tmp);                                         \
                                                                 \
        EXIT_FUNC();

#define GET_HANDLE(rs, index, type, res, lib_type, func) \
                                                         \
        ENTER_FUNC(type, res, OCI_IPC_RESULTSET, rs);    \
                                                         \
        OCI_Define *def = NULL;                          \
                                                         \
        CHECK_PTR(OCI_IPC_RESULTSET, rs);                \
        CHECK_INDEX(index, (rs)->nb_defs);               \
                                                         \
        def = OcilibDefineGet(rs, index);                \
        CHECK_NULL(def);                                 \
                                                         \
        type tmp = res;                                  \
                                                         \
        if (MATCHING_TYPE(def, lib_type))                \
        {                                                \
            def->obj = func;                             \
            CHECK_NULL(def->obj);                        \
            tmp = def->obj;                              \
        }                                                \
                                                         \
        SET_RETVAL(tmp);                                 \
                                                         \
        EXIT_FUNC()

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Resultset * OcilibResultsetCreate
(
    OCI_Statement *stmt,
    OCI_Int32      size
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Resultset*, NULL,
        /* context */ OCI_IPC_STATEMENT, stmt
    );

    OCI_Resultset* rs = NULL;

    CHECK_PTR(OCI_IPC_STATEMENT, stmt);

    /* allocate resultset structure */

    ALLOC_HANDLE(OCI_IPC_RESULTSET, rs);

    /* set attributes */

    ub4 nb = 0;
    ub4 i;

    rs->stmt         = stmt;
    rs->bof          = TRUE;
    rs->eof          = FALSE;
    rs->fetch_size   = (ub4) size;
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
            stmt->handle, &nb, NULL,
            rs->stmt->con->err
        );
    }
    else
    {
        nb = stmt->nb_rbinds;
    }

    /* allocate columns array */

    ALLOC_DATA(OCI_IPC_DEFINE, rs->defs, nb);

    /* describe select list */

    if (OCI_CST_SELECT == stmt->type)
    {
        /* Compute columns information */

        for (i = 0; i < nb; i++)
        {
            OCI_Define *def = &rs->defs[i];

            def->env   = stmt->con->env;
            def->htype = OCI_IPC_DEFINE;

            def->buf.count   = (OCI_UInt32) size;
            def->buf.sizelen = sizeof(ub2);

            def->rs = rs;

            rs->nb_defs++;

            /* get column description */

            CHECK(OcilibColumnRetrieveInfo(&def->col, rs->stmt->con,
                                           rs->stmt, rs->stmt->handle,
                                           (OCI_Int32)(i + 1), OCI_DESC_RESULTSET));

            /* mapping to OCILIB internal types */

            CHECK(OcilibColumnMapInfo(&def->col, rs->stmt->con, rs->stmt));

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

                CHECK_NULL(def);

                /* allocation of internal buffers for resultset content and
                   register OCILIB result buffers to OCI */

                CHECK(OcilibDefineAlloc( def));
                CHECK(OcilibDefineDef(def, i + 1));
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

            def->env   = stmt->con->env;
            def->htype = OCI_IPC_DEFINE;

            def->buf.count   = (OCI_UInt32) size;
            def->buf.sizelen = sizeof(ub4);

            def->rs = rs;

            rs->nb_defs++;

            /* columns info */

            def->col.sqlcode  = bnd->code;
            def->col.libcode  = bnd->code;
            def->col.name     = OcilibStringDuplicate(stmt->con->env, bnd->name);
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
                       retrieve later the integer type (OCI_Int16, integer, OCI_Int64)
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

            CHECK(OcilibColumnMapInfo(&def->col, rs->stmt->con, rs->stmt));
            CHECK(OcilibDefineAlloc(def));
        }
    }

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE)
        {
            OcilibResultsetFree(rs);
            rs = NULL;
        }

        SET_RETVAL(rs);
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetExpandStrings
 * --------------------------------------------------------------------------------------------- */

static OCI_Boolean OcilibResultsetExpandStrings
(
    OCI_Resultset *rs
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_RESULTSET, rs
    );

    CHECK_PTR(OCI_IPC_RESULTSET, rs);

    for (ub4 i = 0; i < rs->nb_defs; i++)
    {
        OCI_Define *def = &rs->defs[i];

        if (OCI_CDT_TEXT == def->col.datatype)
        {
            for (OCI_Int32 j = (OCI_Int32) (def->buf.count-1); j >= 0; j--)
            {
                ub1 * tmpbuf = ((ub1*) def->buf.data) + ((OCI_Size) def->col.bufsize * (OCI_Size) j);

                OcilibStringUTF16ToUTF32(tmpbuf, tmpbuf, (OCI_Int32)(def->col.bufsize / sizeof(otext)) - 1);
            }
        }
    }

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultFinalizeDynamicFetch
 * --------------------------------------------------------------------------------------------- */

static OCI_Boolean OcilibResultFinalizeDynamicFetch
(
    OCI_Resultset *rs
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_RESULTSET, rs
    );

    ub4 i, j;

    CHECK_PTR(OCI_IPC_RESULTSET, rs);

    /* for LONG columns, set the zero terminal string */

    for (i = 0; i < rs->nb_defs; i++)
    {
        OCI_Define *def = &rs->defs[i];

        if (IS_DYNAMIC_FETCH_COLUMN(&def->col))
        {
            for (j = 0; j < def->buf.count; j++)
            {
                OCI_Long* lg = OcilibGetLongObjectFromDefine(def, j);

                if (lg && lg->buffer)
                {
                    OcilibLongFinalizeDynamicFetch(lg);
                }
            }
        }
    }

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetClearFetchedObjectInstances
 * --------------------------------------------------------------------------------------------- */

static OCI_Boolean OcilibResultsetClearFetchedObjectInstances(OCI_Resultset *rs)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_RESULTSET, rs
    );

    CHECK_PTR(OCI_IPC_RESULTSET, rs);

    for (ub4 i = 0; i < rs->nb_defs; i++)
    {
        OCI_Define *def = &(rs->defs[i]);

        if (SQLT_NTY == def->col.sqlcode && def->buf.data)
        {
            for (ub4 j = 0; j < def->buf.count; j++)
            {
                if (def->buf.data[j] != NULL)
                {
                    sword ret;

                    OCI_CALL(ret, OCIObjectFree, rs->stmt->con->env->handle, rs->stmt->con->err, def->buf.data[j], OCI_DEFAULT);

                    def->buf.data[j] = NULL;
                }
            }
        }

        if (NULL != def->obj)
        {
            /* previously assigned handle is invalided
               We shall not try manipulate its oci handle when calling the destructor method */

            ((OCI_DataType*)def->obj)->hstate = OCI_OBJECT_FETCHED_INVALIDATED;
        }
    }

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetFetchData
 * --------------------------------------------------------------------------------------------- */

static OCI_Boolean OcilibResultsetFetchData
(
    OCI_Resultset    *rs,
    OCI_FetchSeekMode direction,
    OCI_Int32         offset
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_RESULTSET, rs
    );

    CHECK_PTR(OCI_IPC_RESULTSET, rs);

    /* let's initialize the success flag to FALSE until the process completes */

    CHECK(OcilibResultsetClearFetchedObjectInstances(rs));

    /* internal fetch */

#if defined(OCI_STMT_SCROLLABLE_READONLY)

    if (rs->stmt->con->env->use_scrollable_cursors)
    {
        rs->fetch_status = rs->stmt->env->api->OCIStmtFetch2(rs->stmt->handle, rs->stmt->con->err,
                                                             rs->fetch_size, (ub2) direction, (sb4) offset,
                                                             (ub4) OCI_DEFAULT);
    }
    else

#endif

    {
        OCI_CALL(rs->fetch_status, OCIStmtFetch, rs->stmt->handle, rs->stmt->con->err,
                 rs->fetch_size, (ub2) OCI_FETCH_NEXT,
                 (ub4) OCI_DEFAULT);
    }

    if (OCI_ERROR == rs->fetch_status)
    {
        /* failure */
        THROW(OcilibExceptionOCI, rs->stmt->con->err, rs->fetch_status);
    }
    else if (OCI_SUCCESS_WITH_INFO == rs->fetch_status)
    {
        OcilibExceptionOCI(&ocilib_call_context, rs->stmt->con->err, rs->fetch_status);
    }

    /* finalize dynamic fetch */
    CHECK(OcilibResultFinalizeDynamicFetch(rs));

    /* check string buffer for Unicode builds that need buffer expansion */

    if (rs->stmt->con->env->use_wide_char_conv)
    {
        CHECK(OcilibResultsetExpandStrings(rs));
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
            rs->stmt->handle, &row_count, NULL,
            rs->stmt->con->err
        );

        CHECK_ATTRIB_GET
        (
            OCI_HTYPE_STMT, OCI_ATTR_ROWS_FETCHED,
            rs->stmt->handle, &row_fetched, NULL,
            rs->stmt->con->err
        );
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
        if ((OCI_SFD_NEXT == direction) || (offset > 0))
        {
            rs->eof = TRUE;
        }

        if (offset < 0)
        {
            rs->bof = TRUE;
        }

        CHECK(FALSE);
    }

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetFetchCustom
 * --------------------------------------------------------------------------------------------- */

static OCI_Boolean OcilibResultsetFetchCustom
(
    OCI_Resultset    *rs,
    OCI_FetchSeekMode mode,
    OCI_Int32         offset
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_RESULTSET, rs
    );

    CHECK_PTR(OCI_IPC_RESULTSET, rs);

    switch (mode)
    {
        case OCI_SFD_RELATIVE:
        {
            const OCI_Boolean isOutOfBound = ((offset > 0) && (rs->eof)) ||
                                             ((offset < 0) && (rs->bof)) ||
                                             ((offset == 0));

            CHECK(isOutOfBound == FALSE);

            offset += (OCI_Int32) rs->row_abs;
            break;
        }
        case OCI_SFD_ABSOLUTE:
        {
            CHECK(offset >= 0);
            offset += 1;
            break;
        }
        default:
        {
            CHECK(FALSE);
            break;
        }
    }

    rs->row_abs = 1;
    rs->row_cur = 1;

    CHECK(OcilibResultsetFetchData(rs, OCI_SFD_ABSOLUTE, offset));

    rs->row_abs = (ub4) offset;

    rs->bof = FALSE;
    rs->eof = FALSE;

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetInitialize
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibResultsetInitialize
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

OCI_Boolean OcilibResultsetFree
(
    OCI_Resultset *rs
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_RESULTSET, rs
    );

    ub4 j;

    CHECK_PTR(OCI_IPC_RESULTSET, rs);

    for (ub4 i = 0; i < rs->nb_defs; i++)
    {
        OCI_Define *def = &(rs->defs[i]);

        /* free buffer objects */

        if (NULL != def->obj)
        {
            /* handy cast to set object state flag */

            OCI_DataType* data_type = (OCI_DataType*)def->obj;

            /* mark it as dirty only is it not already invalidated */

            if (data_type->hstate != OCI_OBJECT_FETCHED_INVALIDATED)
            {
                data_type->hstate = OCI_OBJECT_FETCHED_DIRTY;
            }

            if (OCI_CDT_CURSOR == def->col.datatype)
            {
                OcilibStatementDispose((OCI_Statement *)def->obj);
                FREE(def->obj);
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
                        OcilibMemoryFreeHandle(rs->stmt->con->env, (dvoid *)def->buf.data[j], (ub4)def->col.handletype);
                    }
                }
            }
            else
            {
                OcilibMemoryFreeDescriptorArray(rs->stmt->con->env,
                                                (dvoid *)def->buf.data,
                                                (ub4)def->col.handletype,
                                                (ub4)def->buf.count);
            }
        }

        /* free OCI long buffers */

        if (IS_DYNAMIC_FETCH_COLUMN(&def->col) && def->buf.data)
        {
            for (j = 0; j < def->buf.count; j++)
            {
                if (def->buf.data[j])
                {
                    ((OCI_DataType*)def->buf.data[j])->hstate = OCI_OBJECT_FETCHED_DIRTY;

                    if (OCI_CDT_LONG == def->col.datatype)
                    {
                        OcilibLongFree((OCI_Long*)def->buf.data[j]);
                    }
                    else if (OCI_CDT_XMLTYPE == def->col.datatype)
                    {
                        OcilibXmlTypeFree((OCI_XmlType*)def->buf.data[j]);
                    }
                }
            }
        }

        /* free object instances from object cache */

        CHECK(OcilibResultsetClearFetchedObjectInstances(rs));

        /* free column pointers */

        FREE(def->col.name);

        /* free buffer pointers */

        FREE(def->buf.data);
        FREE(def->buf.inds);
        FREE(def->buf.obj_inds);
        FREE(def->buf.lens);
        FREE(def->buf.tmpbuf);
    }

    /* free column map */

    if (NULL != rs->map)
    {
        OcilibHashFree(rs->map);
    }

    /* free defines (column array) */

    FREE(rs->defs);

    OcilibErrorResetSource(NULL, rs);

    FREE(rs);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetFetchPrev
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibResultsetFetchPrev
(
    OCI_Resultset *rs
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_RESULTSET, rs
    );

    CHECK_PTR(OCI_IPC_RESULTSET, rs);
    CHECK_STMT_STATUS(rs->stmt, OCI_STMT_EXECUTED);
    CHECK_SCROLLABLE_CURSOR_ENABLED(rs->stmt->con);

#if OCI_VERSION_COMPILE >= OCI_9_0

    CHECK_SCROLLABLE_CURSOR_ACTIVATED(rs->stmt);

    CHECK(!rs->bof);

    if (rs->row_cur == 1)
    {
        if (rs->row_abs == 1)
        {
            rs->bof = TRUE;
        }
        else
        {
            OCI_Int32 offset = 0;

            if (rs->fetch_size > rs->row_abs)
            {
                offset = 1 - (OCI_Int32) rs->row_abs;
            }
            else
            {
                offset = 1 - (OCI_Int32) (rs->fetch_size + rs->row_fetched);
            }

            CHECK(OcilibResultsetFetchData(rs, OCI_SFD_RELATIVE, offset));

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

    CHECK(!rs->bof);

    SET_SUCCESS();

#endif

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetFetchNext
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibResultsetFetchNext
(
    OCI_Resultset *rs
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_RESULTSET, rs
    );

    CHECK_PTR(OCI_IPC_RESULTSET, rs);
    CHECK_STMT_STATUS(rs->stmt, OCI_STMT_EXECUTED);

    CHECK(!rs->eof);

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
                CHECK(OcilibResultsetFetchData(rs, OCI_SFD_NEXT, 0));

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

            if (rs->stmt->con->env->use_wide_char_conv)
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

    CHECK(!rs->eof);

    SET_SUCCESS();

    OCILIB_LOG(OCI_LOG_CAT_LIB, OCI_LOG_DEBUG,
               OTEXT("handle=[%p] row fetched (abs=%lld, count=%lld)"),
               rs->stmt,
               (long long) rs->row_abs,
               (long long) rs->row_count);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetFetchFirstg
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibResultsetFetchFirst
(
    OCI_Resultset *rs
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_RESULTSET, rs
    );

    CHECK_PTR(OCI_IPC_RESULTSET, rs);
    CHECK_STMT_STATUS(rs->stmt, OCI_STMT_EXECUTED);
    CHECK_SCROLLABLE_CURSOR_ENABLED(rs->stmt->con);

#if OCI_VERSION_COMPILE >= OCI_9_0

    CHECK_SCROLLABLE_CURSOR_ACTIVATED(rs->stmt);

    rs->bof = FALSE;
    rs->eof = FALSE;

    rs->row_abs = 1;
    rs->row_cur = 1;

    CHECK(OcilibResultsetFetchData(rs, OCI_SFD_FIRST, 0));

    CHECK(!rs->bof);

    SET_SUCCESS();

#endif

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetFetchLast
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibResultsetFetchLast
(
    OCI_Resultset *rs
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_RESULTSET, rs
    );

    CHECK_PTR(OCI_IPC_RESULTSET, rs);
    CHECK_STMT_STATUS(rs->stmt, OCI_STMT_EXECUTED);
    CHECK_SCROLLABLE_CURSOR_ENABLED(rs->stmt->con);

    OCI_Boolean result = FALSE;

#if OCI_VERSION_COMPILE >= OCI_9_0

    CHECK_SCROLLABLE_CURSOR_ACTIVATED(rs->stmt);

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

    SET_RETVAL(result);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetFetchSeek
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibResultsetFetchSeek
(
    OCI_Resultset    *rs,
    OCI_FetchSeekMode mode,
    OCI_Int32         offset
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_RESULTSET, rs
    );

    CHECK_PTR(OCI_IPC_RESULTSET, rs);
    CHECK_STMT_STATUS(rs->stmt, OCI_STMT_EXECUTED);
    CHECK_SCROLLABLE_CURSOR_ENABLED(rs->stmt->con);

#if OCI_VERSION_COMPILE >= OCI_9_0

    CHECK_SCROLLABLE_CURSOR_ACTIVATED(rs->stmt);
    CHECK_ENUM_VALUE(mode, SeekModeValues, OTEXT("Fetch Seek Mode"));

    CHECK(OcilibResultsetFetchCustom(rs, mode, offset));

    SET_SUCCESS();

#else

    OCI_NOT_USED(mode);
    OCI_NOT_USED(offset);

#endif

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetRowCount
 * --------------------------------------------------------------------------------------------- */

OCI_UInt32 OcilibResultsetGetRowCount
(
    OCI_Resultset *rs
)
{
    GET_PROP
    (
        OCI_UInt32, 0,
        OCI_IPC_RESULTSET, rs,
        row_count
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetCurrentRow
 * --------------------------------------------------------------------------------------------- */

OCI_Int32 OcilibResultsetGetCurrentRow
(
    OCI_Resultset *rs
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Int32, -1,
        /* context */ OCI_IPC_RESULTSET, rs
    );

    CHECK_PTR(OCI_IPC_RESULTSET, rs);

    SET_RETVAL((OCI_Int32) rs->row_abs - 1);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetColumnCount
 * --------------------------------------------------------------------------------------------- */

OCI_UInt32 OcilibResultsetGetColumnCount
(
    OCI_Resultset *rs
)
{
    GET_PROP
    (
        OCI_UInt32, 0,
        OCI_IPC_RESULTSET, rs,
        nb_defs
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetColumn
 * --------------------------------------------------------------------------------------------- */

OCI_Column * OcilibResultsetGetColumn
(
    OCI_Resultset *rs,
    OCI_UInt32     index
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Column*, NULL,
        /* context */ OCI_IPC_RESULTSET, rs
    );

    CHECK_PTR(OCI_IPC_RESULTSET, rs);
    CHECK_INDEX(index, rs->nb_defs);

    SET_RETVAL(&rs->defs[index].col);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetColumnByName
 * --------------------------------------------------------------------------------------------- */

OCI_Column * OcilibResultsetGetColumnByName
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Column*, NULL,
        /* context */ OCI_IPC_RESULTSET, rs
    );

    CHECK_PTR(OCI_IPC_RESULTSET, rs);
    CHECK_PTR(OCI_IPC_STRING,    name);

    const OCI_Int32 index = OcilibDefineGetIndex(rs, name);
    CHECK(index >= 0);

    SET_RETVAL(&rs->defs[index].col);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetColumnIndex
 * --------------------------------------------------------------------------------------------- */

OCI_Int32 OcilibResultsetGetColumnIndex
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Int32, -1,
        /* context */ OCI_IPC_RESULTSET, rs
    );

    CHECK_PTR(OCI_IPC_RESULTSET, rs);
    CHECK_PTR(OCI_IPC_STRING,    name);

    SET_RETVAL(OcilibDefineGetIndex(rs, name));

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 *Ocilib ResultsetSetStructNumericType
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibResultsetSetStructNumericType
(
    OCI_Resultset  *rs,
    OCI_UInt32      index,
    OCI_NumericType type
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_RESULTSET, rs
    );

    CHECK_PTR(OCI_IPC_RESULTSET, rs);
    CHECK_INDEX(index, rs->nb_defs);
    CHECK_COMPAT(OCI_CDT_NUMERIC == rs->defs[index].col.datatype);

    rs->defs[index].col.struct_subtype = type;

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetSetStructNumericTypeByName
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibResultsetSetStructNumericTypeByName
(
    OCI_Resultset  *rs,
    const otext    *name,
    OCI_NumericType type
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_RESULTSET, rs
    );

    CHECK_PTR(OCI_IPC_RESULTSET, rs);
    CHECK_PTR(OCI_IPC_STRING,    name);

    const OCI_Int32 index = OcilibDefineGetIndex(rs, name);
    CHECK(index >= 0);

    CHECK(OcilibResultsetSetStructNumericType(rs, (OCI_UInt32)index, type));

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetStruct
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibResultsetGetStruct
(
    OCI_Resultset *rs,
    void          *row_struct,
    void          *row_struct_ind
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_RESULTSET, rs
    );

    char        *ptr  = NULL;
    OCI_Boolean *inds = NULL;

    CHECK_PTR(OCI_IPC_RESULTSET, rs);
    CHECK_PTR(OCI_IPC_VOID,      row_struct);

    ptr  = (char    *) row_struct;
    inds = (OCI_Boolean *) row_struct_ind;

    if (ptr)
    {
        OCI_Size size1 = 0;
        OCI_Size size2 = 0;
        OCI_Size align = 0;

        OCI_Size size = 0;

        for (ub4 i = 0; i < rs->nb_defs; i++)
        {
            OCI_Define *def = &rs->defs[i];

            const OCI_Boolean is_not_null = OcilibDefineIsDataNotNull(def);

            if (i == 0)
            {
                OcilibColumnGetAttributeInfo(&def->col, rs->nb_defs, i, &size1, &align);
            }
            else
            {
                size1 = size2;
            }

            if (i + 1 < rs->nb_defs)
            {
                OcilibColumnGetAttributeInfo(&rs->defs[i + 1].col, rs->nb_defs, i + 1, &size2, &align);
            }
            else
            {
                size2 = 0;
            }

            size += size1;

            size = ROUNDUP(size, align);

            memset(ptr, 0, size1);

            if (is_not_null)
            {
                switch (def->col.datatype)
                {
                    case OCI_CDT_NUMERIC:
                    {
                        OCI_NumericType type = def->col.struct_subtype;
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
                    case OCI_CDT_XMLTYPE:
                    {
                        *((OCI_XmlType **) ptr) = OcilibResultsetGetXmlType(rs, i);
                        break;
                    }
                    case OCI_CDT_JSON:
                    {
                        *((OCI_Json **) ptr) = OcilibResultsetGetJson(rs, i);
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

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
* OcilibResultsetGetNumber
* --------------------------------------------------------------------------------------------- */

OCI_Number * OcilibResultsetGetNumber
(
    OCI_Resultset *rs,
    OCI_UInt32     index
)
{
    GET_HANDLE
    (
        rs, index, OCI_Number *, NULL, OCI_CDT_NUMERIC,

        OcilibNumberInitialize
        (
            rs->stmt->con->env,
            (OCI_Number *) def->obj,
            (OCINumber *)OcilibDefineGetData(def)
        )
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetNumberByName
 * --------------------------------------------------------------------------------------------- */

OCI_Number * OcilibResultsetGetNumberByName
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    GET_BY_NAME(rs, name, OcilibResultsetGetNumber, OCI_Number *, NULL);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetShort
 * --------------------------------------------------------------------------------------------- */

OCI_Int16 OcilibResultsetGetShort
(
    OCI_Resultset *rs,
    OCI_UInt32     index
)
{
    GET_NUMBER(rs, index, OCI_NUM_SHORT, OCI_Int16, 0);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetShortByName
 * --------------------------------------------------------------------------------------------- */

OCI_Int16 OcilibResultsetGetShortByName
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    GET_BY_NAME(rs, name, OcilibResultsetGetShort, OCI_Int16, 0);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetUnsignedShort
 * --------------------------------------------------------------------------------------------- */

OCI_UInt16 OcilibResultsetGetUnsignedShort
(
    OCI_Resultset *rs,
    OCI_UInt32     index
)
{
    GET_NUMBER(rs, index, OCI_NUM_USHORT, OCI_UInt16, 0);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetUnsignedShortByName
 * --------------------------------------------------------------------------------------------- */

OCI_UInt16 OcilibResultsetGetUnsignedShortByName
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    GET_BY_NAME(rs, name, OcilibResultsetGetUnsignedShort, OCI_UInt16, 0);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetInt
 * --------------------------------------------------------------------------------------------- */

OCI_Int32 OcilibResultsetGetInt
(
    OCI_Resultset *rs,
    OCI_UInt32     index
)
{
    GET_NUMBER(rs, index, OCI_NUM_INT, OCI_Int32, 0);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetIntByName
 * --------------------------------------------------------------------------------------------- */

OCI_Int32 OcilibResultsetGetIntByName
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    GET_BY_NAME(rs, name, OcilibResultsetGetInt, OCI_Int32, 0);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetUnsignedInt
 * --------------------------------------------------------------------------------------------- */

OCI_UInt32 OcilibResultsetGetUnsignedInt
(
    OCI_Resultset *rs,
    OCI_UInt32     index
)
{
    GET_NUMBER(rs, index, OCI_NUM_UINT, OCI_UInt32, 0);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetUnsignedIntByName
 * --------------------------------------------------------------------------------------------- */

OCI_UInt32 OcilibResultsetGetUnsignedIntByName
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    GET_BY_NAME(rs, name, OcilibResultsetGetUnsignedInt, OCI_UInt32, 0);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetBigInt
 * --------------------------------------------------------------------------------------------- */

OCI_Int64 OcilibResultsetGetBigInt
(
    OCI_Resultset *rs,
    OCI_UInt32     index
)
{
    GET_NUMBER(rs, index, OCI_NUM_BIGINT, OCI_Int64, 0);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetBigIntByName
 * --------------------------------------------------------------------------------------------- */

OCI_Int64 OcilibResultsetGetBigIntByName
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    GET_BY_NAME(rs, name, OcilibResultsetGetBigInt, OCI_Int64, 0);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetUnsignedBigInt
 * --------------------------------------------------------------------------------------------- */

OCI_UInt64 OcilibResultsetGetUnsignedBigInt
(
    OCI_Resultset *rs,
    OCI_UInt32     index
)
{
    GET_NUMBER(rs, index, OCI_NUM_BIGUINT, OCI_UInt64, 0);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetUnsignedIntByName
 * --------------------------------------------------------------------------------------------- */

OCI_UInt64 OcilibResultsetGetUnsignedBigIntByName
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    GET_BY_NAME(rs, name, OcilibResultsetGetUnsignedBigInt, OCI_UInt64, 0);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetString
 * --------------------------------------------------------------------------------------------- */

const otext * OcilibResultsetGetString
(
    OCI_Resultset *rs,
    OCI_UInt32     index
)
{
    ENTER_FUNC
    (
        /* returns */ otext *, NULL,
        /* context */ OCI_IPC_RESULTSET, rs
    );

    OCI_Define *def = NULL;

    CHECK_PTR(OCI_IPC_RESULTSET, rs);
    CHECK_INDEX(index, rs->nb_defs);

    def = OcilibDefineGet(rs, index);
    CHECK_NULL(def);

    otext *result = NULL;

    if (OcilibDefineIsDataNotNull(def))
    {
        void *data = NULL;

        if (OCI_CDT_TEXT == def->col.datatype)
        {
            /* special case for ROWIDs exposed as strings but internally handled as rowid
             * descriptors */

            if (IS_ROWID_COL(&def->col))
            {
                OCIRowid* rowid        = (OCIRowid*)OcilibDefineGetData(def);
                ub2       rowid_length = 0;

                /* retrieve the size - no error checking */

                sword ret;

                OCI_CALL(ret, OCIRowidToChar, rowid, NULL, &rowid_length, def->rs->stmt->con->err);

                if (rowid_length > 0)
                {
                    /* allocate column temporary buffer if needed */

                    CHECK(OcilibStringRequestBuffer(rs->stmt->con->env, &def->buf.tmpbuf, &def->buf.tmpsize, rowid_length));

                    /* Retrieve ROWID as hex string */

                    CHECK_OCI
                    (
                        rs->stmt->con->err,
                        OCIRowidToChar,
                        rowid, (OraText*) def->buf.tmpbuf, &rowid_length, rs->stmt->con->err
                    );

                    if (IS_WIDE_CHAR_ENV())
                    {
                        /* ROWIDs hex strings are always returned by OCI client as ANSI strings,
                           we need to expand them to native wide character strings */

                        OcilibStringAnsiToNative(def->buf.tmpbuf, def->buf.tmpbuf, rowid_length);
                    }

                    result = def->buf.tmpbuf;
                }
            }
            else
            {
                result = (otext*)OcilibDefineGetData(def);

                /* for long mapped to string, the zero terminal character is not
                    always added by Oracle ? or OCILIB issue ? Anyway we check the
                    length returned by Oracle and set it properly */

                if (OCI_CLONG == def->col.subtype)
                {
                    ub2* lens = (ub2 *)def->buf.lens;

                    result[lens[rs->row_cur - 1]] = 0;
                }
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
        else if (OCI_CDT_XMLTYPE == def->col.datatype)
        {
            OCI_XmlType *xmlType = OcilibResultsetGetXmlType(rs, index);

            if (xmlType)
            {
                result = (otext *)OcilibXmlTypeGetContent(xmlType);
            }
        }
        else
        {
            OCI_Error *err = OcilibErrorGet(ocilib_call_context.env, TRUE, TRUE);

            OCI_UInt32 buffer_size = OCI_SIZE_TMP_CVT;
            OCI_UInt32 data_size   = 0;

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
                    data        = OcilibDefineGetData(def);
                    data_size   = ((ub2*)def->buf.lens)[def->rs->row_cur - 1];
                    buffer_size = data_size * 2;
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
                        buffer_size = OcilibLongGetSize(lg);

                        if (OCI_BLONG == def->col.subtype)
                        {
                            /* here we have binary long, it will be output in hexadecimal */
                            buffer_size *= 2;
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
                        buffer_size = (OCI_UInt32)OcilibLobGetLength(lob);

                        if (OCI_BLOB == def->col.subtype)
                        {
                            /* here we have binary blob, it will be output in hexadecimal */
                            buffer_size *= 2;
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
                        buffer_size = (OCI_UInt32) ostrlen(OTEXT("/"));

                        OcilibFileGetInfo(file);

                        buffer_size += (OCI_UInt32) (file->dir ? ostrlen(file->dir) : 0);
                        buffer_size += (OCI_UInt32) (file->name ? ostrlen(file->name) : 0);
                    }

                    data = file;
                    break;
                }
                case OCI_CDT_OBJECT:
                {
                    OCI_Object *obj = OcilibResultsetGetObject(rs, index);

                    if (obj)
                    {
                        CHECK(OcilibObjectToString(obj, &buffer_size, NULL));
                    }

                    data = obj;
                    break;
                }
                case OCI_CDT_VECTOR:
                {
                    OCI_Vector *vect = OcilibResultsetGetVector(rs, index);

                    if (vect)
                    {
                        CHECK(OcilibVectorToString(vect, &buffer_size, NULL));
                    }

                    data = vect;
                    break;
                }
                case OCI_CDT_COLLECTION:
                {
                    OCI_Coll *coll = OcilibResultsetGetColl(rs, index);

                    if (coll)
                    {
                        CHECK(OcilibCollectionToString(coll, &buffer_size, NULL));
                    }

                    data = coll;
                    break;
                }
                case OCI_CDT_CURSOR:
                {
                    OCI_Statement *stmt = OcilibResultsetGetStatement(rs, index);

                    if (stmt && stmt->sql)
                    {
                        buffer_size = (OCI_UInt32) ostrlen(stmt->sql);
                    }

                    data = stmt;
                    break;
                }
                case OCI_CDT_JSON:
                {
                    OCI_Json *json = OcilibResultsetGetJson(rs, index);

                    if (json)
                    {
                        CHECK(OcilibJsonToText(json, NULL, &buffer_size, 0));
                    }

                    data = json;
                    break;
                }
                default:
                {
                    break;
                }
            }

            CHECK(NULL == err || OCI_UNKNOWN == err->type);

            CHECK(OcilibStringRequestBuffer(rs->stmt->con->env, &def->buf.tmpbuf, &def->buf.tmpsize, buffer_size));

            OcilibStringGetFromType(rs->stmt->con, &def->col, data, data_size, def->buf.tmpbuf, def->buf.tmpsize, FALSE);

            CHECK(NULL == err || OCI_UNKNOWN == err->type);

            result = def->buf.tmpbuf;
        }
    }

    SET_RETVAL(result);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetStringByName
 * --------------------------------------------------------------------------------------------- */

const otext * OcilibResultsetGetStringByName
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    GET_BY_NAME(rs, name, OcilibResultsetGetString, const otext *, NULL);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetJson
 * --------------------------------------------------------------------------------------------- */

OCI_Json * OcilibResultsetGetJson
(
    OCI_Resultset *rs,
    OCI_UInt32     index
)
{
    GET_HANDLE
    (
        rs, index, OCI_Json *, NULL, OCI_CDT_JSON,

        OcilibJsonInitialize
        (
            rs->stmt->con,
            (OCI_Json *) def->obj,
            (OCIJson *) OcilibDefineGetData(def)
        )
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetJsonByName
 * --------------------------------------------------------------------------------------------- */

OCI_Json * OcilibResultsetGetJsonByName
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    GET_BY_NAME(rs, name, OcilibResultsetGetJson, OCI_Json*, NULL);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetRaw
 * --------------------------------------------------------------------------------------------- */

OCI_UInt32 OcilibResultsetGetRaw
(
    OCI_Resultset *rs,
    OCI_UInt32     index,
    void          *buffer,
    OCI_UInt32     len
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_UInt32, 0,
        /* context */ OCI_IPC_RESULTSET, rs
    );

    CHECK_PTR(OCI_IPC_RESULTSET, rs);
    CHECK_PTR(OCI_IPC_VOID,      buffer);
    CHECK_INDEX(index, rs->nb_defs);

    OCI_Define* def = OcilibDefineGet(rs, (OCI_UInt32) index);
    CHECK_NULL(def);

    OCI_UInt32 result = 0;

    if (MATCHING_TYPE(def, OCI_CDT_RAW))
    {
        OCI_UInt32 size = (OCI_UInt32)(ub2)((ub2*)def->buf.lens)[def->rs->row_cur - 1];

        result = size < len ? size : len;

        /* for RAWs, we copy the data in the destination buffer instead of
        returning internal buffer as we do for strings */

        memcpy(buffer, OcilibDefineGetData(def), (OCI_Size)result);
    }

    SET_RETVAL(result);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetRawByName
 * --------------------------------------------------------------------------------------------- */

OCI_UInt32 OcilibResultsetGetRawByName
(
    OCI_Resultset *rs,
    const otext   *name,
    void          *buffer,
    OCI_UInt32     len
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_UInt32, 0,
        /* context */ OCI_IPC_RESULTSET, rs
    );

    CHECK_PTR(OCI_IPC_RESULTSET, rs);
    CHECK_PTR(OCI_IPC_VOID,      buffer);
    CHECK_PTR(OCI_IPC_STRING,    name);

    const OCI_Int32 index = OcilibDefineGetIndex(rs, name);
    CHECK(index >= 0);

    OCI_Define* def = OcilibDefineGet(rs, (OCI_UInt32) index);
    CHECK_NULL(def);

    OCI_UInt32 result = 0;

    if (MATCHING_TYPE(def, OCI_CDT_RAW))
    {
        OCI_UInt32 size = (OCI_UInt32)(ub2)((ub2*)def->buf.lens)[def->rs->row_cur - 1];

        result = size < len ? size : len;

        /* for RAWs, we copy the data in the destination buffer instead of
        returning internal buffer as we do for strings */

        memcpy(buffer, OcilibDefineGetData(def), (OCI_Size)result);
    }

    SET_RETVAL(result);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetDouble
 * --------------------------------------------------------------------------------------------- */

OCI_Double OcilibResultsetGetDouble
(
    OCI_Resultset *rs,
    OCI_UInt32     index
)
{
    GET_NUMBER(rs, index, OCI_NUM_DOUBLE, OCI_Double, 0.0);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetDoubleByName
 * --------------------------------------------------------------------------------------------- */

OCI_Double OcilibResultsetGetDoubleByName
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    GET_BY_NAME(rs, name, OcilibResultsetGetDouble, OCI_Double, 0.0);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetFloat
 * --------------------------------------------------------------------------------------------- */

OCI_Float OcilibResultsetGetFloat
(
    OCI_Resultset *rs,
    OCI_UInt32     index
)
{
    GET_NUMBER(rs, index, OCI_NUM_FLOAT, OCI_Float, 0.0f);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetFloatByName
 * --------------------------------------------------------------------------------------------- */

OCI_Float OcilibResultsetGetFloatByName
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    GET_BY_NAME(rs, name, OcilibResultsetGetFloat, OCI_Float, 0.0f);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetDate
 * --------------------------------------------------------------------------------------------- */

OCI_Date * OcilibResultsetGetDate
(
    OCI_Resultset *rs,
    OCI_UInt32     index
)
{
    GET_HANDLE
    (
        rs, index, OCI_Date *, NULL, OCI_CDT_DATETIME,

        OcilibDateInitialize(rs->stmt->con->env, (OCI_Date *) def->obj,
                             (OCIDate *)OcilibDefineGetData(def), FALSE,
                             (SQLT_DAT == def->col.libcode))
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetDateByName
 * --------------------------------------------------------------------------------------------- */

OCI_Date * OcilibResultsetGetDateByName
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    GET_BY_NAME(rs, name, OcilibResultsetGetDate, OCI_Date *, NULL);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetTimestamp
 * --------------------------------------------------------------------------------------------- */

OCI_Timestamp * OcilibResultsetGetTimestamp
(
    OCI_Resultset *rs,
    OCI_UInt32     index
)
{
    GET_HANDLE
    (
        rs, index, OCI_Timestamp *, NULL, OCI_CDT_TIMESTAMP,

        OcilibTimestampInitialize(rs->stmt->con->env, rs->stmt->con, (OCI_Timestamp *) def->obj,
                                  (OCIDateTime *)OcilibDefineGetData(def), def->col.subtype)
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetTimestampByName
 * --------------------------------------------------------------------------------------------- */

OCI_Timestamp * OcilibResultsetGetTimestampByName
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    GET_BY_NAME(rs, name, OcilibResultsetGetTimestamp, OCI_Timestamp*, NULL);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetInterval
 * --------------------------------------------------------------------------------------------- */

OCI_Interval * OcilibResultsetGetInterval
(
    OCI_Resultset *rs,
    OCI_UInt32     index
)
{
    GET_HANDLE
    (
        rs, index, OCI_Interval *, NULL, OCI_CDT_INTERVAL,

        OcilibIntervalInitialize(rs->stmt->con->env, rs->stmt->con, (OCI_Interval *) def->obj,
                                 (OCIInterval *)OcilibDefineGetData(def), def->col.subtype)
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetIntervalByName
 * --------------------------------------------------------------------------------------------- */

OCI_Interval * OcilibResultsetGetIntervalByName
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    GET_BY_NAME(rs, name, OcilibResultsetGetInterval, OCI_Interval*, NULL);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetObject
 * --------------------------------------------------------------------------------------------- */

OCI_Object * OcilibResultsetGetObject
(
    OCI_Resultset *rs,
    OCI_UInt32     index
)
{
    GET_HANDLE
    (
        rs, index, OCI_Object *, NULL, OCI_CDT_OBJECT,

        OcilibObjectInitialize(rs->stmt->con, (OCI_Object *) def->obj,
                               OcilibDefineGetData(def), def->col.typinf,
                               NULL, -1, TRUE)
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetObjectByName
 * --------------------------------------------------------------------------------------------- */

OCI_Object * OcilibResultsetGetObjectByName
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    GET_BY_NAME(rs, name, OcilibResultsetGetObject, OCI_Object*, NULL);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetColl
 * --------------------------------------------------------------------------------------------- */

OCI_Coll * OcilibResultsetGetColl
(
    OCI_Resultset *rs,
    OCI_UInt32     index
)
{
    GET_HANDLE
    (
        rs, index, OCI_Coll *, NULL, OCI_CDT_COLLECTION,

        OcilibCollectionInitialize(rs->stmt->con, (OCI_Coll *) def->obj,
                                   OcilibDefineGetData(def), def->col.typinf)
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetCollByName
 * --------------------------------------------------------------------------------------------- */

OCI_Coll * OcilibResultsetGetCollByName
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    GET_BY_NAME(rs, name, OcilibResultsetGetColl, OCI_Coll*, NULL);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetReference
 * --------------------------------------------------------------------------------------------- */

OCI_Ref * OcilibResultsetGetReference
(
    OCI_Resultset *rs,
    OCI_UInt32     index
)
{
    GET_HANDLE
    (
        rs, index, OCI_Ref *, NULL, OCI_CDT_REF,

        OcilibReferenceInitialize(rs->stmt->con, def->col.typinf, (OCI_Ref *) def->obj,
                                  OcilibDefineGetData(def))
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetReferenceByName
 * --------------------------------------------------------------------------------------------- */

OCI_Ref * OcilibResultsetGetReferenceByName
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    GET_BY_NAME(rs, name, OcilibResultsetGetReference, OCI_Ref*, NULL);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetXmlType
 * --------------------------------------------------------------------------------------------- */

OCI_XmlType * OcilibResultsetGetXmlType
(
    OCI_Resultset *rs,
    OCI_UInt32     index
)
{
    GET_HANDLE
    (
        rs, index, OCI_XmlType *, NULL, OCI_CDT_XMLTYPE,

        (OCI_XmlType *) OcilibDefineGetData(def)
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetXmlTypeByName
 * --------------------------------------------------------------------------------------------- */

OCI_XmlType * OcilibResultsetGetXmlTypeByName
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    GET_BY_NAME(rs, name, OcilibResultsetGetXmlType, OCI_XmlType*, NULL);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetVector
 * --------------------------------------------------------------------------------------------- */

OCI_Vector * OcilibResultsetGetVector
(
    OCI_Resultset* rs,
    OCI_UInt32     index
)
{
    GET_HANDLE
    (
        rs, index, OCI_Vector *, NULL, OCI_CDT_VECTOR,

        OcilibVectorInitialize(rs->stmt->con, (OCI_Vector *) def->obj, OcilibDefineGetData(def))

    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetVectorByName
 * --------------------------------------------------------------------------------------------- */

OCI_Vector * OcilibResultsetGetVectorByName
(
    OCI_Resultset* rs,
    const otext  * name
)
{
    GET_BY_NAME(rs, name, OcilibResultsetGetVector, OCI_Vector*, NULL);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetStatement
 * --------------------------------------------------------------------------------------------- */

OCI_Statement * OcilibResultsetGetStatement
(
    OCI_Resultset *rs,
    OCI_UInt32     index
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
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetStatementByName
 * --------------------------------------------------------------------------------------------- */

OCI_Statement * OcilibResultsetGetStatementByName
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    GET_BY_NAME(rs, name, OcilibResultsetGetStatement, OCI_Statement*, NULL);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetLob
 * --------------------------------------------------------------------------------------------- */

OCI_Lob * OcilibResultsetGetLob
(
    OCI_Resultset *rs,
    OCI_UInt32     index
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
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetLobByName
 * --------------------------------------------------------------------------------------------- */

OCI_Lob * OcilibResultsetGetLobByName
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    GET_BY_NAME(rs, name, OcilibResultsetGetLob, OCI_Lob*, NULL);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetFile
 * --------------------------------------------------------------------------------------------- */

OCI_File * OcilibResultsetGetFile
(
    OCI_Resultset *rs,
    OCI_UInt32     index
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
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetFileByName
 * --------------------------------------------------------------------------------------------- */

OCI_File * OcilibResultsetGetFileByName
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    GET_BY_NAME(rs, name, OcilibResultsetGetFile, OCI_File*, NULL);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetLong
 * --------------------------------------------------------------------------------------------- */

OCI_Long * OcilibResultsetGetLong
(
    OCI_Resultset *rs,
    OCI_UInt32     index
)
{
    GET_HANDLE
    (
        rs, index, OCI_Long *, NULL, OCI_CDT_LONG,

        (OCI_Long *)OcilibDefineGetData(def)
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetLongByName
 * --------------------------------------------------------------------------------------------- */

OCI_Long * OcilibResultsetGetLongByName
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    GET_BY_NAME(rs, name, OcilibResultsetGetLong, OCI_Long*, NULL);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetDataSize
 * --------------------------------------------------------------------------------------------- */

OCI_UInt32 OcilibResultsetGetDataSize
(
    OCI_Resultset *rs,
    OCI_UInt32     index
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_UInt32, 0,
        /* context */ OCI_IPC_RESULTSET, rs
    );

    CHECK_PTR(OCI_IPC_RESULTSET, rs);
    CHECK_INDEX(index, rs->nb_defs);

    OCI_Define* def = OcilibDefineGet(rs, (OCI_UInt32) index);
    CHECK_NULL(def);

    OCI_UInt32 size = 0;

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

            /* for XMLTYPE, lens array contains the len in characters, not in bytes */
        }
    }

    SET_RETVAL(size);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetDataSizeByName
 * --------------------------------------------------------------------------------------------- */

OCI_UInt32 OcilibResultsetGetDataSizeByName
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_UInt32, 0,
        /* context */ OCI_IPC_RESULTSET, rs
    );

    CHECK_PTR(OCI_IPC_RESULTSET, rs);
    CHECK_PTR(OCI_IPC_STRING,    name);

    const OCI_Int32 index = OcilibDefineGetIndex(rs, name);
    CHECK(index >= 0);

    SET_RETVAL(OcilibResultsetGetDataSize(rs, (OCI_UInt32)index));

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetIsNull
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibResultsetIsNull
(
    OCI_Resultset *rs,
    OCI_UInt32     index
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_RESULTSET, rs
    );

    CHECK_PTR(OCI_IPC_RESULTSET, rs);
    CHECK_INDEX(index, rs->nb_defs);

    OCI_Define *def = OcilibDefineGet(rs, index);
    CHECK_NULL(def);

    SET_RETVAL(!OcilibDefineIsDataNotNull(def));

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetIsNullByName
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibResultsetIsNullByName
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, TRUE,
        /* context */ OCI_IPC_RESULTSET, rs
    );

    CHECK_PTR(OCI_IPC_RESULTSET, rs);
    CHECK_PTR(OCI_IPC_STRING,    name);

    const OCI_Int32 index = OcilibDefineGetIndex(rs, name);
    CHECK(index >= 0);

    SET_RETVAL(OcilibResultsetIsNull(rs, (OCI_UInt32)index));

    EXIT_FUNC();
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
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibResultsetGetDataLength
 * --------------------------------------------------------------------------------------------- */

OCI_UInt32 OcilibResultsetGetDataLength
(
    OCI_Resultset *rs,
    OCI_UInt32     index
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_UInt32, 0,
        /* context */ OCI_IPC_RESULTSET, rs
    );

    CHECK_PTR(OCI_IPC_RESULTSET, rs);
    CHECK_INDEX(index, rs->nb_defs);

    OCI_Define *def = OcilibDefineGet(rs, index);
    CHECK_NULL(def);
    CHECK(rs->row_cur > 0);

    SET_RETVAL((OCI_UInt32)((ub2 *)def->buf.lens)[rs->row_cur - 1]);

    EXIT_FUNC();
}
