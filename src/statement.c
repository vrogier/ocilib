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

#include "statement.h"

#include "bind.h"
#include "collection.h"
#include "connection.h"
#include "date.h"
#include "error.h"
#include "exception.h"
#include "file.h"
#include "format.h"
#include "hash.h"
#include "helpers.h"
#include "interval.h"
#include "list.h"
#include "lob.h"
#include "macros.h"
#include "memory.h"
#include "number.h"
#include "object.h"
#include "reference.h"
#include "resultset.h"
#include "strings.h"
#include "timestamp.h"

#if OCI_VERSION_COMPILE >= OCI_9_0

static unsigned int TimestampTypeValues[] =
{
    OCI_TIMESTAMP,
    OCI_TIMESTAMP_TZ,
    OCI_TIMESTAMP_LTZ
};

static unsigned int IntervalTypeValues[] =
{
    OCI_INTERVAL_YM,
    OCI_INTERVAL_DS
};

#endif

static unsigned int LobTypeValues[] =
{
    OCI_CLOB,
    OCI_NCLOB,
    OCI_BLOB
};

static unsigned int FileTypeValues[] =
{
    OCI_CFILE,
    OCI_BFILE
};

static unsigned int FetchModeValues[] =
{
    OCI_SFM_DEFAULT,
    OCI_SFM_SCROLLABLE
};

static unsigned int BindModeValues[] =
{
    OCI_BIND_BY_POS,
    OCI_BIND_BY_NAME
};

static unsigned int BindAllocationValues[] =
{
    OCI_BAM_EXTERNAL,
    OCI_BAM_INTERNAL
};

static unsigned int LongModeValues[] =
{
    OCI_LONG_EXPLICIT,
    OCI_LONG_IMPLICIT
};

#define CHECK_BIND(stmt, name, data, type, ext_only)                         \
                                                                             \
    CHECK_PTR(OCI_IPC_STATEMENT, stmt)                                       \
    CHECK_PTR(OCI_IPC_STRING,    name)                                       \
    CHECK_STMT_STATUS(stmt, OCI_STMT_PREPARED)                               \
    {                                                                        \
        const boolean ext_only_value = (ext_only);                           \
        if ((ext_only_value) &&                                              \
            (OCI_BAM_INTERNAL == (stmt)->bind_alloc_mode) &&                 \
            ((data) != NULL))                                                \
        {                                                                    \
            THROW(ExceptionExternalBindingNotAllowed, (name))                \
        }                                                                    \
                                                                             \
        if ((ext_only_value) || OCI_BAM_EXTERNAL == (stmt)->bind_alloc_mode) \
        {                                                                    \
            CHECK_PTR(type, data)                                            \
        }                                                                    \
    }                                                                        \

#define CHECK_REGISTER(stmt, name)     \
                                       \
    CHECK_PTR(OCI_IPC_STATEMENT, stmt) \
    CHECK_PTR(OCI_IPC_STRING,    name) \


#define SET_ARG_NUM(type, func)                                    \
                                                                   \
    type src = func(rs, i), *dst = ( type *) va_arg(args, type *); \
    if (dst)                                                       \
    {                                                              \
        *dst = src;                                                \
    }                                                              \

#define SET_ARG_HANDLE(type, func, assign)                         \
                                                                   \
    type *src = func(rs, i), *dst = (type *) va_arg(args, type *); \
    if (src && dst)                                                \
    {                                                              \
        res = assign(dst, src);                                    \
    }                                                              \

#define BIND_DATA(...)                                  \
                                                        \
    CHECK_NULL(BindCreate(stmt, data, name,             \
                          OCI_BIND_INPUT, __VA_ARGS__)) \

#define REGISTER_DATA(...)                               \
                                                         \
    CHECK_NULL(BindCreate(stmt, NULL, name,              \
                          OCI_BIND_OUTPUT, __VA_ARGS__)) \


#define OCI_BIND_CALL(type, check, ...)                 \
                                                        \
    ENTER_FUNC(boolean, FALSE, OCI_IPC_STATEMENT, stmt) \
                                                        \
    CHECK_BIND(stmt, name, data, type, check)           \
                                                        \
    BIND_DATA(__VA_ARGS__)                              \
                                                        \
    SET_SUCCESS()                                       \
                                                        \
    EXIT_FUNC()                                         \

#define BIND_CALL_NULL_ALLOWED(type, ...) \
                                          \
    OCI_BIND_CALL(type, FALSE, __VA_ARGS__)

#define BIND_CALL_NULL_FORBIDDEN(type, ...) \
                                            \
    OCI_BIND_CALL(type, TRUE, __VA_ARGS__)

#define REGISTER_CALL(...)                              \
                                                        \
    ENTER_FUNC(boolean, FALSE, OCI_IPC_STATEMENT, stmt) \
                                                        \
    CHECK_REGISTER(stmt, name)                          \
                                                        \
    REGISTER_DATA(__VA_ARGS__)                          \
                                                        \
    SET_SUCCESS()                                       \
                                                        \
    EXIT_FUNC()                                         \

#define OCI_BIND_GET_SCALAR(s, t, i) (bnd->is_array ? ((t *) (s)) + (i) : (t *) (s))
#define OCI_BIND_GET_HANDLE(s, t, i) (bnd->is_array ? ((t **) (s))[i] : (t *) (s))
#define OCI_BIND_GET_BUFFER(d, t, i) ((t *)((d) + (i) * sizeof(t)))

/* --------------------------------------------------------------------------------------------- *
 * StatementBatchErrorClear
 * --------------------------------------------------------------------------------------------- */

boolean StatementBatchErrorClear
(
    OCI_Statement *stmt
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    )

    CHECK_PTR(OCI_IPC_STATEMENT, stmt)

    if (NULL != stmt->batch)
    {
        /* free internal array of OCI_Errors */

        FREE(stmt->batch->errs)

        /* free batch structure */

        FREE(stmt->batch)
    }

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * StatementFreeAllBinds
 * --------------------------------------------------------------------------------------------- */

boolean StatementFreeAllBinds
(
    OCI_Statement *stmt
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    )

    CHECK_PTR(OCI_IPC_STATEMENT, stmt)

    int i;

    /* free user binds */

    if (NULL != stmt->ubinds)
    {
        for(i = 0; i < stmt->nb_ubinds; i++)
        {
            BindFree(stmt->ubinds[i]);
        }

        FREE(stmt->ubinds)
    }

    /* free register binds */

    if (NULL != stmt->rbinds)
    {
        for(i = 0; i < stmt->nb_rbinds; i++)
        {
            BindFree(stmt->rbinds[i]);
        }

        FREE(stmt->rbinds)
    }

    stmt->nb_ubinds = 0;
    stmt->nb_rbinds = 0;

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * StatementReset
 * --------------------------------------------------------------------------------------------- */

boolean StatementReset
(
    OCI_Statement *stmt
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    )

    CHECK_PTR(OCI_IPC_STATEMENT, stmt)

    ub4 mode = OCI_DEFAULT;

#if OCI_VERSION_COMPILE >= OCI_9_2

    if ((Env.version_runtime >= OCI_9_2) && (stmt->nb_rbinds > 0))
    {
        /*  if we had registered binds, we must delete the statement from the cache.
            Because, if we execute another sql with "returning into clause",
            OCI_ProcInBind won't be called by OCI. Nice Oracle bug ! */

        const unsigned int cache_size = ConnectionGetStatementCacheSize(stmt->con);

        if (cache_size > 0)
        {
            mode = OCI_STRLS_CACHE_DELETE;
        }
    }

#else

    OCI_NOT_USED(mode)

#endif

    /* reset batch errors */

    CHECK(StatementBatchErrorClear(stmt))

    /* free resultsets */

    CHECK(StatementReleaseResultsets(stmt))

    /* free in/out binds */

    CHECK(StatementFreeAllBinds(stmt))

    /* free bind map */

    if (NULL != stmt->map)
    {
        CHECK(HashFree(stmt->map))
    }

    /* free handle if needed */

    if (NULL != stmt->stmt)
    {
        if (OCI_OBJECT_ALLOCATED == stmt->hstate)
        {

#if OCI_VERSION_COMPILE >= OCI_9_2

            if (Env.version_runtime >= OCI_9_2)
            {
                CHECK_OCI
                (
                    stmt->con->err,
                    OCIStmtRelease,
                    stmt->stmt, stmt->con->err, NULL, 0, mode
                )
            }
            else

#endif

            {
                MemoryFreeHandle((dvoid*)stmt->stmt, OCI_HTYPE_STMT);
            }

            stmt->stmt = NULL;
        }
        else if (OCI_OBJECT_ALLOCATED_BIND_STMT == stmt->hstate)
        {
            MemoryFreeHandle((dvoid*)stmt->stmt, OCI_HTYPE_STMT);

            stmt->stmt = NULL;
        }
    }

    /* free sql statement */

    FREE(stmt->sql)
    FREE(stmt->sql_id)

    stmt->rsts   = NULL;
    stmt->stmts  = NULL;
    stmt->sql    = NULL;
    stmt->sql_id = NULL;
    stmt->map    = NULL;
    stmt->batch  = NULL;

    stmt->nb_rs   = 0;
    stmt->nb_stmt = 0;

    stmt->status     = OCI_STMT_CLOSED;
    stmt->type       = OCI_UNKNOWN;
    stmt->bind_array = FALSE;

    stmt->nb_iters      = 1;
    stmt->nb_iters_init = 1;
    stmt->dynidx        = 0;
    stmt->err_pos       = 0;

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
* StatementBindCheck
* --------------------------------------------------------------------------------------------- */

boolean StatementBindCheck
(
    OCI_Bind    *bnd,
    ub1         *src,
    ub1         *dst,
    unsigned int index
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_BIND, bnd
    )

    CHECK_PTR(OCI_IPC_BIND, bnd)
    CHECK_PTR(OCI_IPC_VOID, dst)

    /* Non-scalar type binds */
    if (bnd->alloc && src)
    {
        /* OCI_Number binds */
        if ((OCI_CDT_NUMERIC == bnd->type) && (SQLT_VNU == bnd->code))
        {
            if (OCI_NUM_NUMBER & bnd->subtype)
            {
                if (bnd->buffer.inds[index] != OCI_IND_NULL)
                {
                    OCI_Number *src_num = OCI_BIND_GET_HANDLE(src, OCI_Number, index);
                    OCINumber  *dst_num = OCI_BIND_GET_BUFFER(dst, OCINumber, index);

                    if (src_num)
                    {
                        CHECK_OCI
                        (
                            bnd->stmt->con->err,
                            OCINumberAssign,
                            bnd->stmt->con->err,
                            src_num->handle, dst_num
                        )
                    }
                }
            }
            else if (OCI_NUM_BIGINT & bnd->subtype)
            {
                big_int   *src_bint = OCI_BIND_GET_SCALAR(src, big_int, index);
                OCINumber *dst_num  = OCI_BIND_GET_BUFFER(dst, OCINumber, index);

                CHECK(NumberTranslateValue(bnd->stmt->con, src_bint,
                                           bnd->subtype, dst_num, OCI_NUM_NUMBER))
            }
        }

        /* OCI_Date binds */
        else if (OCI_CDT_DATETIME == bnd->type)
        {
            OCI_Date *src_date = OCI_BIND_GET_HANDLE(src, OCI_Date, index);
            OCIDate  *dst_date = OCI_BIND_GET_BUFFER(dst, OCIDate, index);

            if (src_date)
            {
                CHECK_OCI
                (
                    bnd->stmt->con->err,
                    OCIDateAssign,
                    bnd->stmt->con->err,
                    src_date->handle, dst_date
                )
            }
        }

        /* String binds that may required conversion on systems where wchar_t is UTF32 */
        else if (OCI_CDT_TEXT == bnd->type)
        {
            if (Env.use_wide_char_conv)
            {
                const int    max_chars  = (int) (bnd->size / sizeof(dbtext));
                const size_t src_offset = index * max_chars * sizeof(otext);
                const size_t dst_offset = index * max_chars * sizeof(dbtext);

                StringUTF32ToUTF16(src + src_offset, dst + dst_offset, max_chars - 1);
            }
        }

        /* otherwise we have an ocilib handle based type */
        else
        {
            OCI_Datatype *src_handle = OCI_BIND_GET_HANDLE(src, OCI_Datatype, index);

            if (src_handle)
            {
                ((void**)dst)[index] = src_handle->handle;
            }
        }
    }

    /* for handles, check anyway the value for null data */

    if (IS_OCILIB_OBJECT(bnd->type, bnd->subtype) && OCI_CDT_OBJECT != bnd->type)
    {
        if (bnd->buffer.inds[index] != OCI_IND_NULL)
        {
            bnd->buffer.inds[index] = OCI_IND(dst);
        }
    }

    /* update bind object indicator pointer with object indicator */

    if (SQLT_NTY == bnd->code)
    {
        if (OCI_CDT_OBJECT == bnd->type && bnd->buffer.inds[index] != OCI_IND_NULL && src)
        {
            OCI_Object *obj = OCI_BIND_GET_HANDLE(src, OCI_Object, index);

            if (obj)
            {
                bnd->buffer.obj_inds[index] = obj->tab_ind;
            }
        }
        else
        {
            bnd->buffer.obj_inds[index] = &bnd->buffer.inds[index];
        }
    }

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
* StatementBindUpdate
* --------------------------------------------------------------------------------------------- */

boolean StatementBindUpdate
(
    OCI_Bind    *bnd,
    ub1         *src,
    ub1         *dst,
    unsigned int index
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_BIND, bnd
    )

    CHECK_PTR(OCI_IPC_BIND, bnd)
    CHECK_PTR(OCI_IPC_VOID, src)
    CHECK_PTR(OCI_IPC_VOID, dst)

    /* OCI_Number binds */
    if ((OCI_CDT_NUMERIC == bnd->type) && (SQLT_VNU == bnd->code))
    {
        if (OCI_NUM_NUMBER & bnd->subtype)
        {
            if (bnd->buffer.inds[index] != OCI_IND_NULL)
            {
                OCINumber  *src_num = OCI_BIND_GET_BUFFER(src, OCINumber, index);
                OCI_Number *dst_num = OCI_BIND_GET_HANDLE(dst, OCI_Number, index);

                if (dst_num)
                {
                    CHECK_OCI
                    (
                        bnd->stmt->con->err,
                        OCINumberAssign,
                        bnd->stmt->con->err,
                        src_num, dst_num->handle
                    )
                }
            }
        }
        else if (OCI_NUM_BIGINT & bnd->subtype)
        {
            OCINumber *src_number = OCI_BIND_GET_BUFFER(src, OCINumber, index);
            big_int   *dst_bint   = OCI_BIND_GET_SCALAR(dst, big_int, index);

            if (dst_bint)
            {
                CHECK(NumberTranslateValue(bnd->stmt->con, src_number,
                                           OCI_NUM_NUMBER, dst_bint, bnd->subtype))
            }
        }
    }

    /* OCI_Date binds */
    else if (OCI_CDT_DATETIME == bnd->type)
    {
        OCIDate  *src_date = OCI_BIND_GET_BUFFER(src, OCIDate, index);
        OCI_Date *dst_date = OCI_BIND_GET_HANDLE(dst, OCI_Date, index);

        if (dst_date)
        {
            CHECK_OCI
            (
                bnd->stmt->con->err,
                OCIDateAssign,
                bnd->stmt->con->err, src_date,
                dst_date->handle
            )
        }
    }

    /* String binds that may required conversion on systems where wchar_t is UTF32 */
    else if (OCI_CDT_TEXT == bnd->type)
    {
        if (Env.use_wide_char_conv)
        {
            const int    max_chars  = (int) (bnd->size / sizeof(dbtext));
            const size_t src_offset = index * max_chars * sizeof(dbtext);
            const size_t dst_offset = index * max_chars * sizeof(otext);

            StringUTF16ToUTF32(src + src_offset, dst + dst_offset, max_chars - 1);
        }
    }
    else if (OCI_CDT_OBJECT == bnd->type)
    {
        /* update object indicator with bind object indicator pointer */

        OCI_Object *obj = OCI_BIND_GET_HANDLE(dst, OCI_Object, index);

        if (obj && bnd->buffer.inds[index] != OCI_IND_NULL)
        {
            obj->tab_ind = (sb2*)bnd->buffer.obj_inds[index];
        }
    }

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * StatementBindCheckAll
 * --------------------------------------------------------------------------------------------- */

boolean StatementBindCheckAll
(
    OCI_Statement *stmt
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    )

    ub4 j;

    CHECK_PTR(OCI_IPC_STATEMENT, stmt)

    for (ub4 i = 0; i < stmt->nb_ubinds; i++)
    {
        OCI_Bind *bnd = stmt->ubinds[i];

        if (OCI_CDT_CURSOR == bnd->type)
        {
            OCI_Statement *bnd_stmt = (OCI_Statement *) bnd->buffer.data;

            StatementReset(bnd_stmt);

            bnd_stmt->hstate = OCI_OBJECT_ALLOCATED_BIND_STMT;

            /* allocate statement handle */

            CHECK
            (
                MemoryAllocHandle
                (
                    (dvoid *)bnd_stmt->con->env,
                    (dvoid **)(void *)&bnd_stmt->stmt,
                    OCI_HTYPE_STMT
                )
            )

            CHECK(StatementSetPrefetchSize(stmt, stmt->prefetch_size))
            CHECK(StatementSetFetchSize(stmt, stmt->fetch_size))
        }

        if ((bnd->direction & OCI_BDM_IN) ||
            (bnd->alloc &&
             (OCI_CDT_DATETIME != bnd->type) &&
             (OCI_CDT_TEXT != bnd->type) &&
             (OCI_CDT_NUMERIC != bnd->type || SQLT_VNU == bnd->code)))
        {
            /* for strings, re-initialize length array with buffer default size */

            if (OCI_CDT_TEXT == bnd->type)
            {
                for (j = 0; j < bnd->buffer.count; j++)
                {
                    *(ub2*)(((ub1 *)bnd->buffer.lens) + (sizeof(ub2) * (size_t) j)) = (ub2) bnd->size;
                }
            }

            /* extra work for internal allocated binds buffers */

            if (bnd->alloc)
            {
                if (bnd->is_array)
                {
                    const ub4 count = IS_PLSQL_STMT(stmt->type) ? bnd->nbelem : stmt->nb_iters;

                    for (j = 0; j < count; j++)
                    {
                        CHECK(StatementBindCheck(bnd, (ub1*)bnd->input, (ub1*)bnd->buffer.data, j))
                    }
                }
                else
                {
                    CHECK(StatementBindCheck(bnd, (ub1*)bnd->input, (ub1*)bnd->buffer.data, 0))
                }
            }
        }
    }

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * StatementBindUpdateAll
 * --------------------------------------------------------------------------------------------- */

boolean StatementBindUpdateAll
(
    OCI_Statement *stmt
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    )

    CHECK_PTR(OCI_IPC_STATEMENT, stmt)

    for (ub4 i = 0; i < stmt->nb_ubinds; i++)
    {
        OCI_Bind *bnd = stmt->ubinds[i];

        if (OCI_CDT_CURSOR == bnd->type)
        {
            OCI_Statement *bnd_stmt = (OCI_Statement *) bnd->buffer.data;

            bnd_stmt->status = OCI_STMT_PREPARED  | OCI_STMT_PARSED |
                               OCI_STMT_DESCRIBED | OCI_STMT_EXECUTED;

            bnd_stmt->type = OCI_CST_SELECT;
        }

        if ((bnd->direction & OCI_BDM_OUT) && (bnd->input) && (bnd->buffer.data))
        {
            if (bnd->alloc)
            {
                if (bnd->is_array)
                {
                    const ub4 count = IS_PLSQL_STMT(stmt->type) ? bnd->nbelem : stmt->nb_iters;

                    for (ub4 j = 0; j < count; j++)
                    {
                        CHECK(StatementBindUpdate(bnd, (ub1*)bnd->buffer.data, (ub1*)bnd->input, j))
                    }
                }
                else
                {
                    CHECK(StatementBindUpdate(bnd, (ub1*)bnd->buffer.data, (ub1*)bnd->input, 0))
                }
            }
        }
    }

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * StatementFetchIntoUserVariables
 * --------------------------------------------------------------------------------------------- */

boolean StatementFetchIntoUserVariables
(
    OCI_Statement *stmt,
    va_list        args
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    )

    CHECK_PTR(OCI_IPC_STATEMENT, stmt)

    OCI_Resultset *rs = NULL;
    boolean res = FALSE;

    /* get resultset */

    rs = StatementGetResultset(stmt);

    /* fetch data */

    if (rs)
    {
        res = ResultsetFetchNext(rs);
    }

    if (res)
    {
        unsigned int i, n;

        /* loop on column list for updating user given placeholders */

        for (i = 1, n = ResultsetGetColumnCount(rs); (i <= n) && res; i++)
        {
            OCI_Column *col = ResultsetGetColumn(rs, i);

            const int type = va_arg(args, int);

            switch (type)
            {
                case OCI_ARG_TEXT:
                {
                    const otext *src = ResultsetGetString(rs, i);
                    otext       *dst = va_arg(args, otext *);

                    if (dst)
                    {
                        dst[0] = 0;
                    }

                    if (dst && src)
                    {
                        ostrcat(dst, src);
                    }

                    break;
                }
                case OCI_ARG_SHORT:
                {
                    SET_ARG_NUM(short, ResultsetGetShort);
                    break;
                }
                case OCI_ARG_USHORT:
                {
                    SET_ARG_NUM(unsigned short, ResultsetGetUnsignedShort);
                    break;
                }
                case OCI_ARG_INT:
                {
                    SET_ARG_NUM(int, ResultsetGetInt);
                    break;
                }
                case OCI_ARG_UINT:
                {
                    SET_ARG_NUM(unsigned int, ResultsetGetUnsignedInt);
                    break;
                }
                case OCI_ARG_BIGINT:
                {
                    SET_ARG_NUM(big_int, ResultsetGetBigInt);
                    break;
                }
                case OCI_ARG_BIGUINT:
                {
                    SET_ARG_NUM(big_uint, ResultsetGetUnsignedBigInt);
                    break;
                }
                case OCI_ARG_DOUBLE:
                {
                    SET_ARG_NUM(double, ResultsetGetDouble);
                    break;
                }
                case OCI_ARG_FLOAT:
                {
                    SET_ARG_NUM(float, ResultsetGetFloat);
                    break;
                }
                case OCI_ARG_NUMBER:
                {
                    SET_ARG_HANDLE(OCI_Number, ResultsetGetNumber, NumberAssign);
                    break;
                }
                case OCI_ARG_DATETIME:
                {
                    SET_ARG_HANDLE(OCI_Date, ResultsetGetDate, DateAssign);
                    break;
                }
                case OCI_ARG_RAW:
                {
                    ResultsetGetRaw(rs, i, va_arg(args, otext *), col->bufsize);
                    break;
                }
                case OCI_ARG_LOB:
                {
                    SET_ARG_HANDLE(OCI_Lob, ResultsetGetLob, LobAssign);
                    break;
                }
                case OCI_ARG_FILE:
                {
                    SET_ARG_HANDLE(OCI_File, ResultsetGetFile, FileAssign);
                    break;
                }
                case OCI_ARG_TIMESTAMP:
                {
                    SET_ARG_HANDLE(OCI_Timestamp, ResultsetGetTimestamp, TimestampAssign);
                    break;
                }
                case OCI_ARG_INTERVAL:
                {
                    SET_ARG_HANDLE(OCI_Interval, ResultsetGetInterval, IntervalAssign);
                    break;
                }
                case OCI_ARG_OBJECT:
                {
                    SET_ARG_HANDLE(OCI_Object, ResultsetGetObject, ObjectAssign);
                    break;
                }
                case OCI_ARG_COLLECTION:
                {
                    SET_ARG_HANDLE(OCI_Coll, ResultsetGetColl, CollectionAssign);
                    break;
                }
                case OCI_ARG_REF:
                {
                    SET_ARG_HANDLE(OCI_Ref, ResultsetGetReference, ReferenceAssign);
                    break;
                }
                default:
                {
                    THROW(ExceptionMappingArgument, type);
                    break;
                }
            }
        }
    }

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * StatementInitialize
 * --------------------------------------------------------------------------------------------- */

OCI_Statement * StatementInitialize
(
    OCI_Connection *con,
    OCI_Statement  *stmt,
    OCIStmt        *handle,
    boolean         is_desc,
    const otext    *sql
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Statement*, stmt,
        /* context */ OCI_IPC_CONNECTION, con
    )

    CHECK_PTR(OCI_IPC_CONNECTION, con)

    ALLOC_DATA(OCI_IPC_STATEMENT, stmt, 1);

    stmt->con  = con;
    stmt->stmt = handle;

    stmt->exec_mode       = OCI_DEFAULT;
    stmt->long_size       = OCI_SIZE_LONG;
    stmt->bind_reuse      = FALSE;
    stmt->bind_mode       = OCI_BIND_BY_NAME;
    stmt->long_mode       = OCI_LONG_EXPLICIT;
    stmt->bind_alloc_mode = OCI_BAM_EXTERNAL;
    stmt->fetch_size      = OCI_FETCH_SIZE;
    stmt->prefetch_size   = OCI_PREFETCH_SIZE;

    /* reset statement */

    CHECK(StatementReset(stmt))

    if (is_desc)
    {
        stmt->hstate = OCI_OBJECT_FETCHED_CLEAN;
        stmt->status = OCI_STMT_PREPARED  | OCI_STMT_PARSED |
                       OCI_STMT_DESCRIBED | OCI_STMT_EXECUTED;
        stmt->type = OCI_CST_SELECT;

        if (NULL != sql)
        {
            stmt->sql = ostrdup(sql);
        }
        else
        {
            dbtext *dbstr  = NULL;
            int     dbsize = 0;

            CHECK_ATTRIB_GET
            (
                OCI_HTYPE_STMT, OCI_ATTR_STATEMENT,
                stmt->stmt, &dbstr, &dbsize,
                stmt->con->err
            )

            if (NULL !=  dbstr)
            {
                stmt->sql = StringDuplicateFromDBString(dbstr, dbcharcount(dbsize));
                CHECK_NULL(stmt->sql)
            }
        }

        /* Setting fetch attributes here as the statement is already prepared */

        CHECK(StatementSetPrefetchSize(stmt, stmt->prefetch_size))
        CHECK(StatementSetFetchSize(stmt, stmt->fetch_size))
    }
    else
    {
        /* allocate handle for non fetched cursor */

        stmt->hstate = OCI_OBJECT_ALLOCATED;
    }

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE)
        {
            StatementFree(stmt);
            stmt = NULL;
        }

        SET_RETVAL(stmt)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * StatementDispose
 * --------------------------------------------------------------------------------------------- */

boolean StatementDispose
(
    OCI_Statement *stmt
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    )

    CHECK_PTR(OCI_IPC_STATEMENT, stmt)

    /* reset data */

    StatementReset(stmt);

    ErrorResetSource(NULL, stmt);

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * StatementCheckImplicitResultsets
 * --------------------------------------------------------------------------------------------- */

boolean StatementCheckImplicitResultsets
(
    OCI_Statement *stmt
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    )

    CHECK_PTR(OCI_IPC_STATEMENT, stmt)

#if OCI_VERSION_COMPILE >= OCI_12_1

    if (Env.version_runtime >= OCI_12_1)
    {
        CHECK_ATTRIB_GET
        (
            OCI_HTYPE_STMT, OCI_ATTR_IMPLICIT_RESULT_COUNT,
            stmt->stmt, &stmt->nb_stmt, NULL,
            stmt->con->err
        )

        if (stmt->nb_stmt > 0)
        {
            OCIStmt *result  = NULL;
            ub4      rs_type = OCI_UNKNOWN;
            ub4      i       = 0;

            /* allocate resultset handles array */

            ALLOC_DATA(OCI_IPC_STATEMENT_ARRAY, stmt->stmts, stmt->nb_stmt)
            ALLOC_DATA(OCI_IPC_RESULTSET_ARRAY, stmt->rsts,  stmt->nb_stmt)

            while (OCI_SUCCESS == OCIStmtGetNextResult(stmt->stmt, stmt->con->err, (dvoid  **)&result, &rs_type, OCI_DEFAULT))
            {
                if (OCI_RESULT_TYPE_SELECT == rs_type)
                {
                    stmt->stmts[i] = StatementInitialize(stmt->con, NULL, result, TRUE, NULL);

                    CHECK_NULL(stmt->stmts[i])

                    stmt->rsts[i] = ResultsetCreate(stmt->stmts[i], stmt->stmts[i]->fetch_size);

                    CHECK_NULL(stmt->rsts[i])

                    i++;
                    stmt->nb_rs++;
                }
            }
        }
    }

#endif

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * StatementBatchErrorsInit
 * --------------------------------------------------------------------------------------------- */

boolean StatementBatchErrorInit
(
    OCI_Statement *stmt
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    )

    OCIError* hndl = NULL;

    ub4 err_count = 0;

    CHECK_PTR(OCI_IPC_STATEMENT, stmt)

    CHECK(StatementBatchErrorClear(stmt))

    /* all OCI call here are not checked for errors as we already dealing
       with an array DML error */

    CHECK_ATTRIB_GET
    (
        OCI_HTYPE_STMT, OCI_ATTR_NUM_DML_ERRORS,
        stmt->stmt, &err_count, NULL,
        stmt->con->err
    )

    if (err_count > 0)
    {
        /* allocate batch error structure */

        ALLOC_DATA(OCI_IPC_BATCH_ERRORS, stmt->batch, 1)

        /* allocate array of error objects */

        ALLOC_DATA(OCI_IPC_ERROR, stmt->batch->errs, err_count)

        /* allocate OCI error handle */

        CHECK(MemoryAllocHandle((dvoid  *)stmt->con->env, (dvoid **)(void *)&hndl, OCI_HTYPE_ERROR))

        /* loop on the OCI errors to fill OCILIB error objects */

        stmt->batch->count = err_count;

        for (ub4 i = 0; i < stmt->batch->count; i++)
        {
            sb4 row = 0;

            OCI_Error *err = &stmt->batch->errs[i];

            OCIParamGet((dvoid *) stmt->con->err, OCI_HTYPE_ERROR,
                        stmt->con->err, (dvoid **) (void *) &hndl, i);

            if (NULL != hndl)
            {
                sb4   err_code = 0;
                otext buffer[512];
                int   err_size = osizeof(buffer);

                dbtext * err_msg = StringGetDBString(buffer, &err_size);

                OCIAttrGet((dvoid *)hndl, (ub4)OCI_HTYPE_ERROR,
                           (void *)&row, (ub4 *)NULL,
                           (ub4)OCI_ATTR_DML_ROW_OFFSET, stmt->con->err);

                OCIErrorGet((dvoid *)hndl, (ub4)1, (OraText *)NULL, &err_code,
                            (OraText *)err_msg, (ub4)err_size, (ub4)OCI_HTYPE_ERROR);

                ErrorSet
                (
                    err,
                    OCI_ERR_ORACLE,
                    (int)err_code,
                    call_context.source_ptr,
                    call_context.source_type,
                    call_context.location,
                    buffer,
                    row + 1
                );

                StringReleaseDBString(err_msg);
            }
        }
    }

    SET_SUCCESS()

    CLEANUP_AND_EXIT_FUNC
    (
        if (NULL != hndl)
        {
            MemoryFreeHandle(hndl, OCI_HTYPE_ERROR);
        }
    )
}

/* --------------------------------------------------------------------------------------------- *
 * StatementPrepareInternal
 * --------------------------------------------------------------------------------------------- */

boolean StatementPrepareInternal
(
    OCI_Statement *stmt,
    const otext   *sql
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    )

    dbtext *dbstr = NULL;
    int dbsize = -1;

    CHECK_PTR(OCI_IPC_STATEMENT, stmt)

    /* reset statement */

    CHECK(StatementReset(stmt))

    /* store SQL */

    stmt->sql = ostrdup(sql);

    dbstr = StringGetDBString(stmt->sql, &dbsize);

    if (Env.version_runtime < OCI_9_2)
    {
        /* allocate handle */

        CHECK(MemoryAllocHandle((dvoid *)stmt->con->env, (dvoid **)(void *)&stmt->stmt, OCI_HTYPE_STMT))
    }

    /* prepare SQL */

#if OCI_VERSION_COMPILE >= OCI_9_2

    if (Env.version_runtime >= OCI_9_2)
    {
        ub4 mode = OCI_DEFAULT;

  #if OCI_VERSION_COMPILE >= OCI_12_2

        if (ConnectionIsVersionSupported(stmt->con, OCI_12_2))
        {
            mode |= OCI_PREP2_GET_SQL_ID;
        }

  #endif

        CHECK_OCI
        (
            stmt->con->err,
            OCIStmtPrepare2,
            stmt->con->cxt, &stmt->stmt, stmt->con->err, (OraText *) dbstr,
            (ub4) dbsize, NULL, 0, (ub4) OCI_NTV_SYNTAX, (ub4) mode
        )
    }
    else

#endif

    {
        CHECK_OCI
        (
            stmt->con->err,
            OCIStmtPrepare,
            stmt->stmt,stmt->con->err, (OraText *) dbstr, (ub4) dbsize,
            (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT
        )
    }

    /* get statement type */

    CHECK_ATTRIB_GET
    (
        OCI_HTYPE_STMT, OCI_ATTR_STMT_TYPE,
        stmt->stmt, &stmt->type, NULL,
        stmt->con->err
    )

    /* update statement status */

    stmt->status = OCI_STMT_PREPARED;

    CHECK(StatementSetPrefetchSize(stmt, stmt->prefetch_size))
    CHECK(StatementSetFetchSize(stmt, stmt->fetch_size))

    SET_SUCCESS()

    CLEANUP_AND_EXIT_FUNC
    (
        StringReleaseDBString(dbstr);
    )
}

/* --------------------------------------------------------------------------------------------- *
 * StatementExecuteInternal
 * --------------------------------------------------------------------------------------------- */

boolean StatementExecuteInternal
(
    OCI_Statement *stmt,
    ub4            mode
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    )

    CHECK_PTR(OCI_IPC_STATEMENT, stmt)

    ub4 iters = 0;

    /* set up iterations and mode values for execution */

    if (OCI_CST_SELECT == stmt->type)
    {
        mode |= stmt->exec_mode;
    }
    else
    {
        iters = stmt->nb_iters;

        /* for array DML, use batch error mode */

        if (iters > 1)
        {
            mode = mode | OCI_BATCH_ERRORS;
        }
    }

    /* reset batch errors */

    CHECK(StatementBatchErrorClear(stmt))

    /* check bind objects for updating their null indicator status */

    CHECK(StatementBindCheckAll(stmt))

    /* check current resultsets */

    if (stmt->rsts)
    {
        /* resultsets are freed before any prepare operations.
           So, if we got ones here, it means the same SQL order
           is re-executed */

        if (OCI_CST_SELECT == stmt->type)
        {
            /* just reinitialize the current resultset */

            CHECK(ResultsetInitialize(stmt->rsts[0]))
        }
        else
        {
            /* Must free previous resultsets for 'returning into'
               SQL orders that can produce multiple resultsets */

            CHECK(StatementReleaseResultsets(stmt))
        }
    }

    /* Oracle execute call */

    const sword ret = OCIStmtExecute(stmt->con->cxt, stmt->stmt, stmt->con->err, 
                                     iters, (ub4)0, (OCISnapshot *)NULL, 
                                     (OCISnapshot *)NULL, mode);

    /* check result */

    boolean success = ((OCI_SUCCESS   == ret) || (OCI_SUCCESS_WITH_INFO == ret) ||
                       (OCI_NEED_DATA == ret) || (OCI_NO_DATA == ret));

    if (OCI_SUCCESS_WITH_INFO == ret)
    {
        ExceptionOCI(&call_context, stmt->con->err, ret);
    }

    /* on batch mode, check if any error occurred */

    if (mode & OCI_BATCH_ERRORS)
    {
        /* build batch error list if the statement is array DML */

        StatementBatchErrorInit(stmt);

        if (stmt->batch)
        {
            success = (stmt->batch->count == 0);
        }
    }

    /* update status on success */

    if (success)
    {
        if (mode & OCI_PARSE_ONLY)
        {
            stmt->status |= OCI_STMT_PARSED;
        }
        else if (mode & OCI_DESCRIBE_ONLY)
        {
            stmt->status |= OCI_STMT_PARSED;
            stmt->status |= OCI_STMT_DESCRIBED;
        }
        else
        {
            stmt->status |= OCI_STMT_PARSED;
            stmt->status |= OCI_STMT_DESCRIBED;
            stmt->status |= OCI_STMT_EXECUTED;

#if OCI_VERSION_COMPILE >= OCI_12_2

            if (ConnectionIsVersionSupported(stmt->con, OCI_12_2))
            {
                unsigned int size_id = 0;

                StringGetAttribute(stmt->con, stmt->stmt, OCI_HTYPE_STMT, OCI_ATTR_SQL_ID, &stmt->sql_id, &size_id);
            }

#endif

            /* reset binds indicators */

            CHECK(StatementBindUpdateAll(stmt))

            /* commit if necessary */

            if (stmt->con->autocom)
            {
                CHECK(ConnectionCommit(stmt->con))
            }

            /* check if any implicit results are available */

            CHECK(StatementCheckImplicitResultsets(stmt))

        }
    }
    else
    {
        /* get parse error position type */

        /* (one of the rare OCI call not enclosed with a OCI_CALL macro ...) */

        OCIAttrGet((dvoid *)stmt->stmt, (ub4)OCI_HTYPE_STMT,
                   (dvoid *)&stmt->err_pos, (ub4 *)NULL,
                   (ub4)OCI_ATTR_PARSE_ERROR_OFFSET, stmt->con->err);

        /* raise exception */

        THROW(ExceptionOCI, stmt->con->err, ret)
    }

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * StatementCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Statement * StatementCreate
(
    OCI_Connection *con
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Statement*, NULL,
        /* context */ OCI_IPC_CONNECTION, con
    )

    CHECK_PTR(OCI_IPC_CONNECTION, con)

    /* create statement object */

    OCI_Statement *stmt = ListAppend(con->stmts, sizeof(*stmt));
    CHECK_NULL(stmt)

    SET_RETVAL(StatementInitialize(con, (OCI_Statement*)stmt, NULL, FALSE, NULL))

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * StatementFree
 * --------------------------------------------------------------------------------------------- */

boolean StatementFree
(
    OCI_Statement *stmt
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    )

    CHECK_PTR(OCI_IPC_STATEMENT, stmt)
    CHECK_OBJECT_FETCHED(stmt)

    StatementDispose(stmt);
    ListRemove(stmt->con->stmts, stmt);

    FREE(stmt)

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * StatementGetResultset
 * --------------------------------------------------------------------------------------------- */

OCI_Resultset * StatementGetResultset
(
    OCI_Statement *stmt
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Resultset*, NULL,
        /* context */ OCI_IPC_STATEMENT, stmt
    )

    CHECK_PTR(OCI_IPC_STATEMENT, stmt)
    CHECK_STMT_STATUS(stmt, OCI_STMT_DESCRIBED)

    OCI_Resultset *rs = NULL;

    /* if the sql statement does not return a result, we just return NULL and not
       throwing any exception
       statements that can return a resultset are "SELECT..." and "... RETURNING INTO..."
    */

    if ((OCI_CST_SELECT == stmt->type) || (stmt->nb_rbinds > 0) || (stmt->nb_stmt > 0))
    {
        /* if the resultset exists, let's use it */

        if (stmt->rsts && stmt->rsts[0])
        {
            rs = stmt->rsts[0];
        }

        /* allocate resultset for select statements only */

        if (NULL == rs && (OCI_CST_SELECT == stmt->type))
        {
            /* allocate memory for one resultset handle */

            ALLOC_DATA(OCI_IPC_RESULTSET_ARRAY, stmt->rsts, 1)

            stmt->nb_rs  = 1;
            stmt->cur_rs = 0;

            /* create resultset object */

            rs = stmt->rsts[0] = ResultsetCreate(stmt, stmt->fetch_size);
        }

        CHECK_NULL(rs)
    }

    SET_RETVAL(rs)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * StatementGetNextResultset
 * --------------------------------------------------------------------------------------------- */

OCI_Resultset * StatementGetNextResultset
(
    OCI_Statement *stmt
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Resultset*, NULL,
        /* context */ OCI_IPC_STATEMENT, stmt
    )

    CHECK_PTR(OCI_IPC_STATEMENT, stmt)
    CHECK_STMT_STATUS(stmt, OCI_STMT_DESCRIBED)
    CHECK(stmt->cur_rs < stmt->nb_rs-1)

    SET_RETVAL(stmt->rsts[++stmt->cur_rs])

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * StatementReleaseResultsets
 * --------------------------------------------------------------------------------------------- */

boolean StatementReleaseResultsets
(
    OCI_Statement *stmt
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    )

    ub4 i;

    CHECK_PTR(OCI_IPC_STATEMENT, stmt)

    /* release statements for implicit resultsets */
    if (NULL != stmt->stmts)
    {
        for (i = 0; i < stmt->nb_stmt; i++)
        {
            if (stmt->stmts[i] != NULL)
            {
                StatementDispose(stmt->stmts[i]);
                FREE(stmt->stmts[i])
            }
        }

        FREE(stmt->stmts)
    }

    /* release resultsets */
    if (NULL != stmt->rsts)
    {
        for (i = 0; i < stmt->nb_rs; i++)
        {
            if (stmt->rsts[i] != NULL)
            {
                ResultsetFree(stmt->rsts[i]);
            }
        }

        FREE(stmt->rsts)
    }

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * StatementPrepare
 * --------------------------------------------------------------------------------------------- */

boolean StatementPrepare
(
    OCI_Statement *stmt,
    const otext   *sql
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    )

    CHECK_PTR(OCI_IPC_STATEMENT, stmt)
    CHECK_PTR(OCI_IPC_STRING,    sql)

    CHECK(StatementPrepareInternal(stmt, sql))

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * StatementExecute
 * --------------------------------------------------------------------------------------------- */

boolean StatementExecute
(
    OCI_Statement *stmt
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    )

    CHECK_PTR(OCI_IPC_STATEMENT, stmt)
    CHECK_STMT_STATUS(stmt, OCI_STMT_PREPARED)

    CHECK(StatementExecuteInternal(stmt, OCI_DEFAULT))

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * StatementExecuteStmt
 * --------------------------------------------------------------------------------------------- */

boolean StatementExecuteStmt
(
    OCI_Statement *stmt,
    const otext   *sql
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    )

    CHECK_PTR(OCI_IPC_STATEMENT, stmt)
    CHECK_PTR(OCI_IPC_STRING,    sql)

    CHECK(StatementPrepareInternal(stmt, sql))
    CHECK(StatementExecuteInternal(stmt, OCI_DEFAULT))

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * StatementParse
 * --------------------------------------------------------------------------------------------- */

boolean StatementParse
(
    OCI_Statement *stmt,
    const otext   *sql
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    )

    CHECK_PTR(OCI_IPC_STATEMENT, stmt)
    CHECK_PTR(OCI_IPC_STRING,    sql)

    CHECK(StatementPrepareInternal(stmt, sql))
    CHECK(StatementExecuteInternal(stmt, OCI_PARSE_ONLY))

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * StatementDescribe
 * --------------------------------------------------------------------------------------------- */

boolean StatementDescribe
(
    OCI_Statement *stmt,
    const otext   *sql
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    )

    CHECK_PTR(OCI_IPC_STATEMENT, stmt)
    CHECK_PTR(OCI_IPC_STRING,    sql)

    CHECK(StatementPrepareInternal(stmt, sql))

    if (OCI_CST_SELECT == stmt->type)
    {
        CHECK(StatementExecuteInternal(stmt, OCI_DESCRIBE_ONLY))
    }

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * StatementPrepareFmt
 * --------------------------------------------------------------------------------------------- */

boolean StatementPrepareFmt
(
    OCI_Statement *stmt,
    const otext   *sql,
    va_list        args
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    )

    otext* sql_fmt = NULL;

    va_list first_pass_args;
    va_list second_pass_args;

    CHECK_PTR(OCI_IPC_STATEMENT, stmt)
    CHECK_PTR(OCI_IPC_STRING,    sql)

    va_copy(first_pass_args,  args);
    va_copy(second_pass_args, args);

    /* first, get buffer size */

    const int size = FormatParseSql(stmt, NULL, sql, &first_pass_args);

    if (size > 0)
    {
        /* allocate buffer */

        ALLOC_DATA(OCI_IPC_STRING, sql_fmt, size + 1)

        /* format buffer */

        if (FormatParseSql(stmt, sql_fmt, sql, &second_pass_args) > 0)
        {
            /* parse buffer */

            CHECK(StatementPrepareInternal(stmt, sql_fmt))
        }
    }

    SET_SUCCESS()

    CLEANUP_AND_EXIT_FUNC
    (
        va_end(first_pass_args);
        va_end(second_pass_args);

        FREE(sql_fmt)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * StatementExecuteStmtFmt
 * --------------------------------------------------------------------------------------------- */

boolean StatementExecuteStmtFmt
(
    OCI_Statement *stmt,
    const otext   *sql,
    va_list        args
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    )

    va_list first_pass_args;
    va_list second_pass_args;

    otext* sql_fmt = NULL;

    CHECK_PTR(OCI_IPC_STATEMENT, stmt)
    CHECK_PTR(OCI_IPC_STRING,    sql)

    va_copy(first_pass_args,  args);
    va_copy(second_pass_args, args);

    /* first, get buffer size */

    const int size = FormatParseSql(stmt, NULL, sql, &first_pass_args);

    if (size > 0)
    {

        /* allocate buffer */

        ALLOC_DATA(OCI_IPC_STRING, sql_fmt, size + 1)

        /* format buffer */

        if (FormatParseSql(stmt, sql_fmt, sql, &second_pass_args) > 0)
        {
            /* prepare and execute SQL buffer */

            CHECK(StatementPrepareInternal(stmt, sql_fmt))
            CHECK(StatementExecuteInternal(stmt, OCI_DEFAULT))
        }
    }

    SET_SUCCESS()

    CLEANUP_AND_EXIT_FUNC
    (
        va_end(first_pass_args);
        va_end(second_pass_args);

        FREE(sql_fmt)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * StatementParseFmt
 * --------------------------------------------------------------------------------------------- */

boolean StatementParseFmt
(
    OCI_Statement *stmt,
    const otext   *sql,
    va_list        args
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    )

    va_list first_pass_args;
    va_list second_pass_args;

    otext* sql_fmt = NULL;

    CHECK_PTR(OCI_IPC_STATEMENT, stmt)
    CHECK_PTR(OCI_IPC_STRING,    sql)

    va_copy(first_pass_args,  args);
    va_copy(second_pass_args, args);

    /* first, get buffer size */

    const int size = FormatParseSql(stmt, NULL, sql, &first_pass_args);

    if (size > 0)
    {
        /* allocate buffer */

        ALLOC_DATA(OCI_IPC_STRING, sql_fmt, size + 1)

        /* format buffer */

        if (FormatParseSql(stmt, sql_fmt, sql, &second_pass_args) > 0)
        {
            /* prepare and execute SQL buffer */

            CHECK(StatementPrepareInternal(stmt, sql_fmt))
            CHECK(StatementExecuteInternal(stmt, OCI_PARSE_ONLY))
        }
    }

    SET_SUCCESS()

    CLEANUP_AND_EXIT_FUNC
    (
        va_end(first_pass_args);
        va_end(second_pass_args);

        FREE(sql_fmt)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * StatementDescribeFmt
 * --------------------------------------------------------------------------------------------- */

boolean StatementDescribeFmt
(
    OCI_Statement *stmt,
    const otext   *sql,
    va_list        args
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    )

    va_list first_pass_args;
    va_list second_pass_args;

    otext* sql_fmt = NULL;

    CHECK_PTR(OCI_IPC_STATEMENT, stmt)
    CHECK_PTR(OCI_IPC_STRING,    sql)

    va_copy(first_pass_args,  args);
    va_copy(second_pass_args, args);

    /* first, get buffer size */

    const int size = FormatParseSql(stmt, NULL, sql, &first_pass_args);

    if (size > 0)
    {
        /* allocate buffer */

        ALLOC_DATA(OCI_IPC_STRING, sql_fmt, size + 1)

        /* format buffer */

        if (FormatParseSql(stmt, sql_fmt, sql, &second_pass_args) > 0)
        {
            /* prepare and execute SQL buffer */

            CHECK(StatementPrepareInternal(stmt, sql_fmt))
            CHECK(StatementExecuteInternal(stmt, OCI_DESCRIBE_ONLY))
        }
    }

    SET_SUCCESS()

    CLEANUP_AND_EXIT_FUNC
    (
        va_end(first_pass_args);
        va_end(second_pass_args);

        FREE(sql_fmt)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * StatementSetBindArraySize
 * --------------------------------------------------------------------------------------------- */

boolean StatementSetBindArraySize
(
    OCI_Statement *stmt,
    unsigned int   size
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    )

    CHECK_PTR(OCI_IPC_STATEMENT, stmt)
    CHECK_MIN(size, 1)
    CHECK_STMT_STATUS(stmt, OCI_STMT_PREPARED)

    /* if the statements already has binds, we need to check if the new size is
       not greater than the initial size
    */

    if ((stmt->nb_ubinds > 0) && (stmt->nb_iters_init < size))
    {
        THROW(ExceptionBindArraySize, stmt->nb_iters_init, stmt->nb_iters, size)
    }
    else
    {
        stmt->nb_iters   = size;
        stmt->bind_array = TRUE;

        if (stmt->nb_ubinds == 0)
        {
            stmt->nb_iters_init = stmt->nb_iters;
        }
    }

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * StatementGetBindArraySize
 * --------------------------------------------------------------------------------------------- */

unsigned int StatementGetBindArraySize
(
    OCI_Statement *stmt
)
{
    GET_PROP
    (
        unsigned int, 0,
        OCI_IPC_STATEMENT, stmt,
        nb_iters
    )
}

/* --------------------------------------------------------------------------------------------- *
 * StatementAllowRebinding
 * --------------------------------------------------------------------------------------------- */

boolean StatementAllowRebinding
(
    OCI_Statement *stmt,
    boolean        value
)
{
    SET_PROP
    (
        /* handle */ OCI_IPC_STATEMENT, stmt,
        /* member */ bind_reuse, boolean,
        /* value  */ value
    )
}

/* --------------------------------------------------------------------------------------------- *
 * StatementIsRebindingAllowed
 * --------------------------------------------------------------------------------------------- */

boolean StatementIsRebindingAllowed
(
    OCI_Statement *stmt
)
{
    GET_PROP
    (
        boolean, FALSE,
        OCI_IPC_STATEMENT, stmt,
        bind_reuse
    )
}

/* --------------------------------------------------------------------------------------------- *
* OCI_BindBoolean
* --------------------------------------------------------------------------------------------- */

boolean StatementBindBoolean
(
    OCI_Statement *stmt,
    const otext   *name,
    boolean       *data
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    )

    CHECK_BIND(stmt, name, data, OCI_IPC_BOOLEAN, TRUE)
    CHECK_EXTENDED_PLSQLTYPES_ENABLED(stmt->con)

#if OCI_VERSION_COMPILE >= OCI_12_1

    BIND_DATA(sizeof(boolean), OCI_CDT_BOOLEAN, SQLT_BOL, 0, NULL, 0)

#else

    OCI_NOT_USED(name)
    OCI_NOT_USED(data)

#endif

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * StatementBindNumber
 * --------------------------------------------------------------------------------------------- */

boolean StatementBindNumber
(
    OCI_Statement *stmt,
    const otext   *name,
    OCI_Number    *data
)
{
    BIND_CALL_NULL_ALLOWED
    (
        OCI_IPC_SHORT, sizeof(OCINumber), OCI_CDT_NUMERIC, SQLT_VNU, OCI_NUM_NUMBER, NULL, 0
    )
}

/* --------------------------------------------------------------------------------------------- *
* OCI_BindArrayOfNumbers
* --------------------------------------------------------------------------------------------- */

boolean StatementBindArrayOfNumbers
(
    OCI_Statement *stmt,
    const otext   *name,
    OCI_Number   **data,
    unsigned int   nbelem
)
{
    BIND_CALL_NULL_ALLOWED
    (
        OCI_IPC_NUMBER, sizeof(OCINumber), OCI_CDT_NUMERIC, SQLT_VNU, OCI_NUM_NUMBER, NULL, nbelem
    )
}

/* --------------------------------------------------------------------------------------------- *
 * StatementBindShort
 * --------------------------------------------------------------------------------------------- */

boolean StatementBindShort
(
    OCI_Statement *stmt,
    const otext   *name,
    short         *data
)
{
    BIND_CALL_NULL_ALLOWED
    (
        OCI_IPC_SHORT, sizeof(short), OCI_CDT_NUMERIC, SQLT_INT, OCI_NUM_SHORT, NULL, 0
    )
}

/* --------------------------------------------------------------------------------------------- *
 * StatementBindArrayOfShorts
 * --------------------------------------------------------------------------------------------- */

boolean StatementBindArrayOfShorts
(
    OCI_Statement *stmt,
    const otext   *name,
    short         *data,
    unsigned int   nbelem
)
{
    BIND_CALL_NULL_ALLOWED
    (
        OCI_IPC_SHORT, sizeof(short), OCI_CDT_NUMERIC, SQLT_INT, OCI_NUM_SHORT, NULL, nbelem
    )
}

/* --------------------------------------------------------------------------------------------- *
 * StatementBindUnsignedShort
 * --------------------------------------------------------------------------------------------- */

boolean StatementBindUnsignedShort
(
    OCI_Statement  *stmt,
    const otext    *name,
    unsigned short *data
)
{
    BIND_CALL_NULL_ALLOWED
    (
        OCI_IPC_SHORT, sizeof(unsigned short), OCI_CDT_NUMERIC, SQLT_UIN, OCI_NUM_USHORT, NULL, 0
    )
}

/* --------------------------------------------------------------------------------------------- *
 * StatementBindArrayOfUnsignedShorts
 * --------------------------------------------------------------------------------------------- */

boolean StatementBindArrayOfUnsignedShorts
(
    OCI_Statement  *stmt,
    const otext    *name,
    unsigned short *data,
    unsigned int    nbelem
)
{
    BIND_CALL_NULL_ALLOWED
    (
        OCI_IPC_SHORT, sizeof(unsigned short), OCI_CDT_NUMERIC, SQLT_UIN, OCI_NUM_USHORT, NULL, nbelem
    )
}

/* --------------------------------------------------------------------------------------------- *
 * StatementBindInt
 * --------------------------------------------------------------------------------------------- */

boolean StatementBindInt
(
    OCI_Statement *stmt,
    const otext   *name,
    int           *data
)
{
    BIND_CALL_NULL_ALLOWED
    (
        OCI_IPC_INT, sizeof(int), OCI_CDT_NUMERIC, SQLT_INT, OCI_NUM_INT, NULL, 0
    )
}

/* --------------------------------------------------------------------------------------------- *
 * StatementBindArrayOfInts
 * --------------------------------------------------------------------------------------------- */

boolean StatementBindArrayOfInts
(
    OCI_Statement *stmt,
    const otext   *name,
    int           *data,
    unsigned int   nbelem
)
{
    BIND_CALL_NULL_ALLOWED
    (
        OCI_IPC_INT, sizeof(int), OCI_CDT_NUMERIC, SQLT_INT, OCI_NUM_INT, NULL, nbelem
    )
}

/* --------------------------------------------------------------------------------------------- *
 * StatementBindUnsignedInt
 * --------------------------------------------------------------------------------------------- */

boolean StatementBindUnsignedInt
(
    OCI_Statement *stmt,
    const otext   *name,
    unsigned int  *data
)
{
    BIND_CALL_NULL_ALLOWED
    (
        OCI_IPC_INT, sizeof(unsigned int), OCI_CDT_NUMERIC, SQLT_UIN, OCI_NUM_UINT, NULL, 0
    )
}

/* --------------------------------------------------------------------------------------------- *
 * StatementBindArrayOfUnsignedInts
 * --------------------------------------------------------------------------------------------- */

boolean StatementBindArrayOfUnsignedInts
(
    OCI_Statement *stmt,
    const otext   *name,
    unsigned int  *data,
    unsigned int   nbelem
)
{
    BIND_CALL_NULL_ALLOWED
    (
        OCI_IPC_INT, sizeof(unsigned int), OCI_CDT_NUMERIC, SQLT_UIN, OCI_NUM_UINT, NULL, nbelem
    )
}

/* --------------------------------------------------------------------------------------------- *
 * StatementBindBigInt
 * --------------------------------------------------------------------------------------------- */

boolean StatementBindBigInt
(
    OCI_Statement *stmt,
    const otext   *name,
    big_int       *data
)
{
    BIND_CALL_NULL_ALLOWED
    (
        OCI_IPC_BIGINT, sizeof(OCINumber), OCI_CDT_NUMERIC, SQLT_VNU, OCI_NUM_BIGINT, NULL, 0
    )
}

/* --------------------------------------------------------------------------------------------- *
 * StatementBindArrayOfBigInts
 * --------------------------------------------------------------------------------------------- */

boolean StatementBindArrayOfBigInts
(
    OCI_Statement *stmt,
    const otext   *name,
    big_int       *data,
    unsigned int   nbelem
)
{
    BIND_CALL_NULL_ALLOWED
    (
        OCI_IPC_BIGINT, sizeof(OCINumber), OCI_CDT_NUMERIC, SQLT_VNU, OCI_NUM_BIGINT, NULL, nbelem
    )
}

/* --------------------------------------------------------------------------------------------- *
 * StatementBindUnsignedBigInt
 * --------------------------------------------------------------------------------------------- */

boolean StatementBindUnsignedBigInt
(
    OCI_Statement *stmt,
    const otext   *name,
    big_uint      *data
)
{
    BIND_CALL_NULL_ALLOWED
    (
        OCI_IPC_BIGINT, sizeof(OCINumber), OCI_CDT_NUMERIC, SQLT_VNU, OCI_NUM_BIGUINT, NULL, 0
    )
}

/* --------------------------------------------------------------------------------------------- *
 * StatementBindArrayOfUnsignedInts
 * --------------------------------------------------------------------------------------------- */

boolean StatementBindArrayOfUnsignedBigInts
(
    OCI_Statement *stmt,
    const otext   *name,
    big_uint      *data,
    unsigned int   nbelem
)
{
    BIND_CALL_NULL_ALLOWED
    (
        OCI_IPC_BIGINT, sizeof(OCINumber), OCI_CDT_NUMERIC, SQLT_VNU, OCI_NUM_BIGUINT, NULL, nbelem
    )
}

/* --------------------------------------------------------------------------------------------- *
 * StatementBindString
 * --------------------------------------------------------------------------------------------- */

boolean StatementBindString
(
    OCI_Statement *stmt,
    const otext   *name,
    otext         *data,
    unsigned int   len
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    )

    CHECK_BIND(stmt, name, data, OCI_IPC_STRING, FALSE)

    if ((len == 0) || len == (UINT_MAX))
    {
        if (data)
        {
            /* only compute length for external bind if no valid length has been provided */

            len = (unsigned int) ostrlen(data);
        }
        else
        {
            /* if data is NULL, it means that binding mode is OCI_BAM_INTERNAL.
               An invalid length passed to the function, we do not have a valid length to
               allocate internal array, thus we need to raise an exception */

            THROW(ExceptionMinimumValue, 1)
        }
    }

    const unsigned int size = (len + 1) * (ub4) sizeof(dbtext);

    BIND_DATA(size, OCI_CDT_TEXT, SQLT_STR, 0, NULL, 0)

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * StatementBindArrayOfStrings
 * --------------------------------------------------------------------------------------------- */

boolean StatementBindArrayOfStrings
(
    OCI_Statement *stmt,
    const otext   *name,
    otext         *data,
    unsigned int   len,
    unsigned int   nbelem
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    )

    CHECK_BIND(stmt, name, data, OCI_IPC_STRING, FALSE)
    CHECK_MIN(len, 1)

    const unsigned int size = (len + 1) * (ub4) sizeof(dbtext);

    BIND_DATA(size, OCI_CDT_TEXT, SQLT_STR, 0, NULL, nbelem)

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * StatementBindRaw
 * --------------------------------------------------------------------------------------------- */

boolean StatementBindRaw
(
    OCI_Statement *stmt,
    const otext   *name,
    void          *data,
    unsigned int   len
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    )

    CHECK_BIND(stmt, name, data, OCI_IPC_VOID, FALSE)

    if (len == 0 && !data)
    {
        /* if data is NULL, it means that binding mode is OCI_BAM_INTERNAL.
        An invalid length passed to the function, we do not have a valid length to
        allocate internal array, thus we need to raise an exception */

        THROW(ExceptionMinimumValue, 1)
    }

    BIND_DATA(len, OCI_CDT_RAW, SQLT_BIN, 0, NULL, 0)

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * StatementBindArrayOfRaws
 * --------------------------------------------------------------------------------------------- */

boolean StatementBindArrayOfRaws
(
    OCI_Statement *stmt,
    const otext   *name,
    void          *data,
    unsigned int   len,
    unsigned int   nbelem
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    )

    CHECK_BIND(stmt, name, data, OCI_IPC_VOID, FALSE)
    CHECK_MIN(len, 1)

    BIND_DATA(len, OCI_CDT_RAW, SQLT_BIN, 0, NULL, nbelem)

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * StatementBindDouble
 * --------------------------------------------------------------------------------------------- */

boolean StatementBindDouble
(
    OCI_Statement *stmt,
    const otext   *name,
    double        *data
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    )

    unsigned int code = SQLT_FLT;

    CHECK_BIND(stmt, name, data, OCI_IPC_DOUBLE, FALSE)

#if OCI_VERSION_COMPILE >= OCI_10_1

    if (ConnectionIsVersionSupported(stmt->con, OCI_10_1))
    {
        code = SQLT_BDOUBLE;
    }

#endif

    BIND_DATA(sizeof(double), OCI_CDT_NUMERIC, code, OCI_NUM_DOUBLE, NULL, 0)

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * StatementBindArrayOfDoubles
 * --------------------------------------------------------------------------------------------- */

boolean StatementBindArrayOfDoubles
(
    OCI_Statement *stmt,
    const otext   *name,
    double        *data,
    unsigned int   nbelem
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    )

    unsigned int code = SQLT_FLT;

    CHECK_BIND(stmt, name, data, OCI_IPC_DOUBLE, FALSE)

#if OCI_VERSION_COMPILE >= OCI_10_1

    if (ConnectionIsVersionSupported(stmt->con, OCI_10_1))
    {
        code = SQLT_BDOUBLE;
    }

#endif

    BIND_DATA(sizeof(double), OCI_CDT_NUMERIC, code, OCI_NUM_DOUBLE, NULL, nbelem)

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * StatementBindFloat
 * --------------------------------------------------------------------------------------------- */

boolean StatementBindFloat
(
    OCI_Statement *stmt,
    const otext   *name,
    float         *data
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    )

    unsigned int code = SQLT_FLT;

    CHECK_BIND(stmt, name, data, OCI_IPC_FLOAT, FALSE);

#if OCI_VERSION_COMPILE >= OCI_10_1

    if (ConnectionIsVersionSupported(stmt->con, OCI_10_1))
    {
        code = SQLT_BFLOAT;
    }

#endif

    BIND_DATA(sizeof(float), OCI_CDT_NUMERIC, code, OCI_NUM_FLOAT, NULL, 0)

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * StatementBindArrayOfFloats
 * --------------------------------------------------------------------------------------------- */

boolean StatementBindArrayOfFloats
(
    OCI_Statement *stmt,
    const otext   *name,
    float         *data,
    unsigned int   nbelem
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    )

    unsigned int code = SQLT_FLT;

    CHECK_BIND(stmt, name, data, OCI_IPC_FLOAT, FALSE)

#if OCI_VERSION_COMPILE >= OCI_10_1

    if (ConnectionIsVersionSupported(stmt->con, OCI_10_1))
    {
        code = SQLT_BFLOAT;
    }

#endif

    BIND_DATA(sizeof(float), OCI_CDT_NUMERIC, code, OCI_NUM_FLOAT, NULL, nbelem)

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * StatementBindDate
 * --------------------------------------------------------------------------------------------- */

boolean StatementBindDate
(
    OCI_Statement *stmt,
    const otext   *name,
    OCI_Date      *data
)
{
    BIND_CALL_NULL_ALLOWED
    (
        OCI_IPC_DATE, sizeof(OCIDate), OCI_CDT_DATETIME, SQLT_ODT, 0, NULL, 0
    )
}

/* --------------------------------------------------------------------------------------------- *
 * StatementBindArrayOfDates
 * --------------------------------------------------------------------------------------------- */

boolean StatementBindArrayOfDates
(
    OCI_Statement *stmt,
    const otext   *name,
    OCI_Date     **data,
    unsigned int   nbelem
)
{
    BIND_CALL_NULL_ALLOWED
    (
        OCI_IPC_DATE, sizeof(OCIDate), OCI_CDT_DATETIME, SQLT_ODT, 0, NULL, nbelem
    )
}

/* --------------------------------------------------------------------------------------------- *
 * StatementBindTimestamp
 * --------------------------------------------------------------------------------------------- */

boolean StatementBindTimestamp
(
    OCI_Statement *stmt,
    const otext   *name,
    OCI_Timestamp *data
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    )

    CHECK_BIND(stmt, name, data, OCI_IPC_TIMESTAMP, TRUE)
    CHECK_TIMESTAMP_ENABLED(stmt->con)

#if OCI_VERSION_COMPILE >= OCI_9_0

    BIND_DATA(sizeof(OCIDateTime *), OCI_CDT_TIMESTAMP,
              ExternalSubTypeToSQLType(OCI_CDT_TIMESTAMP, data->type),
              data->type, NULL, 0)

#endif

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * StatementBindArrayOfTimestamps
 * --------------------------------------------------------------------------------------------- */

boolean StatementBindArrayOfTimestamps
(
    OCI_Statement  *stmt,
    const otext    *name,
    OCI_Timestamp **data,
    unsigned int    type,
    unsigned int    nbelem
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    )

    CHECK_BIND(stmt, name, data, OCI_IPC_TIMESTAMP, FALSE)
    CHECK_TIMESTAMP_ENABLED(stmt->con)

#if OCI_VERSION_COMPILE >= OCI_9_0

    CHECK_ENUM_VALUE(type, TimestampTypeValues, OTEXT("Timestamp type"))

    BIND_DATA(sizeof(OCIDateTime *), OCI_CDT_TIMESTAMP,
              ExternalSubTypeToSQLType(OCI_CDT_TIMESTAMP, type),
              type, NULL, nbelem)

#else

    OCI_NOT_USED(name)
    OCI_NOT_USED(type)
    OCI_NOT_USED(nbelem)

#endif

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * StatementBindInterval
 * --------------------------------------------------------------------------------------------- */

boolean StatementBindInterval
(
    OCI_Statement *stmt,
    const otext   *name,
    OCI_Interval  *data
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    )

    CHECK_BIND(stmt, name, data, OCI_IPC_INTERVAL, TRUE)
    CHECK_INTERVAL_ENABLED(stmt->con)

#if OCI_VERSION_COMPILE >= OCI_9_0

    BIND_DATA(sizeof(OCIInterval *), OCI_CDT_INTERVAL,
              ExternalSubTypeToSQLType(OCI_CDT_INTERVAL, data->type),
              data->type, NULL, 0)

#else

    OCI_NOT_USED(name)

#endif

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * StatementBindArrayOfIntervals
 * --------------------------------------------------------------------------------------------- */

boolean StatementBindArrayOfIntervals
(
    OCI_Statement *stmt,
    const otext   *name,
    OCI_Interval **data,
    unsigned int   type,
    unsigned int   nbelem
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    )

    CHECK_BIND(stmt, name, data, OCI_IPC_INTERVAL, FALSE)
    CHECK_INTERVAL_ENABLED(stmt->con)

#if OCI_VERSION_COMPILE >= OCI_9_0

    CHECK_ENUM_VALUE(type, IntervalTypeValues, OTEXT("Interval type"))

    BIND_DATA(sizeof(OCIInterval *), OCI_CDT_INTERVAL,
              ExternalSubTypeToSQLType(OCI_CDT_INTERVAL, type),
              type, NULL, nbelem)

#else

    OCI_NOT_USED(name)
    OCI_NOT_USED(type)
    OCI_NOT_USED(nbelem)

#endif

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * StatementBindObject
 * --------------------------------------------------------------------------------------------- */

boolean StatementBindObject
(
    OCI_Statement *stmt,
    const otext   *name,
    OCI_Object    *data
)
{
    BIND_CALL_NULL_FORBIDDEN
    (
        OCI_IPC_OBJECT, sizeof(void *), OCI_CDT_OBJECT, SQLT_NTY, 0, data->typinf, 0
    )
}

/* --------------------------------------------------------------------------------------------- *
 * StatementBindArrayOfObjects
 * --------------------------------------------------------------------------------------------- */

boolean StatementBindArrayOfObjects
(
    OCI_Statement *stmt,
    const otext   *name,
    OCI_Object   **data,
    OCI_TypeInfo  *typinf,
    unsigned int   nbelem
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    )

    CHECK_BIND(stmt, name, data, OCI_IPC_OBJECT, FALSE)
    CHECK_PTR(OCI_IPC_TYPE_INFO, typinf)

    BIND_DATA(sizeof(void *), OCI_CDT_OBJECT, SQLT_NTY, 0, typinf, nbelem)

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * StatementBindLob
 * --------------------------------------------------------------------------------------------- */

boolean StatementBindLob
(
    OCI_Statement *stmt,
    const otext   *name,
    OCI_Lob       *data
)
{
    BIND_CALL_NULL_FORBIDDEN
    (
        OCI_IPC_LOB,
        sizeof(OCILobLocator*), OCI_CDT_LOB,
        ExternalSubTypeToSQLType(OCI_CDT_LOB, data->type),
        data->type, NULL, 0
    )
}

/* --------------------------------------------------------------------------------------------- *
 * StatementBindArrayOfLobs
 * --------------------------------------------------------------------------------------------- */

boolean StatementBindArrayOfLobs
(
    OCI_Statement *stmt,
    const otext   *name,
    OCI_Lob      **data,
    unsigned int   type,
    unsigned int   nbelem
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    )

    CHECK_BIND(stmt, name, data, OCI_IPC_LOB, FALSE)

    CHECK_ENUM_VALUE(type, LobTypeValues, OTEXT("Lob type"))

    BIND_DATA(sizeof(OCILobLocator*), OCI_CDT_LOB,
              ExternalSubTypeToSQLType(OCI_CDT_LOB, type),
              type, NULL, nbelem)

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * StatementBindFile
 * --------------------------------------------------------------------------------------------- */

boolean StatementBindFile
(
    OCI_Statement *stmt,
    const otext   *name,
    OCI_File      *data
)
{
    BIND_CALL_NULL_FORBIDDEN
    (
        OCI_IPC_FILE,
        sizeof(OCILobLocator*), OCI_CDT_FILE,
        ExternalSubTypeToSQLType(OCI_CDT_FILE, data->type),
        data->type, NULL, 0
    )
}

/* --------------------------------------------------------------------------------------------- *
 * StatementBindArrayOfFiles
 * --------------------------------------------------------------------------------------------- */

boolean StatementBindArrayOfFiles
(
    OCI_Statement *stmt,
    const otext   *name,
    OCI_File     **data,
    unsigned int   type,
    unsigned int   nbelem
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    )

    CHECK_BIND(stmt, name, data, OCI_IPC_LOB, OCI_IPC_FILE)
    CHECK_ENUM_VALUE(type, FileTypeValues, OTEXT("File type"))

    BIND_DATA(sizeof(OCILobLocator*), OCI_CDT_FILE,
              ExternalSubTypeToSQLType(OCI_CDT_FILE, type),
              type, NULL, nbelem)

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * StatementBindReference
 * --------------------------------------------------------------------------------------------- */

boolean StatementBindReference
(
    OCI_Statement *stmt,
    const otext   *name,
    OCI_Ref       *data
)
{
    BIND_CALL_NULL_FORBIDDEN
    (
        OCI_IPC_REF, sizeof(OCIRef *), OCI_CDT_REF, SQLT_REF, 0, data->typinf, 0
    )
}

/* --------------------------------------------------------------------------------------------- *
 * StatementBindArrayOfReferences
 * --------------------------------------------------------------------------------------------- */

boolean StatementBindArrayOfReferences
(
    OCI_Statement *stmt,
    const otext   *name,
    OCI_Ref      **data,
    OCI_TypeInfo  *typinf,
    unsigned int   nbelem
)
{
    BIND_CALL_NULL_FORBIDDEN
    (
        OCI_IPC_REF, sizeof(OCIRef *), OCI_CDT_REF, SQLT_REF, 0, typinf, nbelem
    )
}

/* --------------------------------------------------------------------------------------------- *
 * StatementBindCollection
 * --------------------------------------------------------------------------------------------- */

boolean StatementBindCollection
(
    OCI_Statement *stmt,
    const otext   *name,
    OCI_Coll      *data
)
{
    BIND_CALL_NULL_FORBIDDEN
    (
        OCI_IPC_COLLECTION, sizeof(OCIColl*), OCI_CDT_COLLECTION, SQLT_NTY, 0, data->typinf, 0
    )
}

/* --------------------------------------------------------------------------------------------- *
 * StatementBindArrayOfCollections
 * --------------------------------------------------------------------------------------------- */

boolean StatementBindArrayOfCollections
(
    OCI_Statement *stmt,
    const otext   *name,
    OCI_Coll     **data,
    OCI_TypeInfo  *typinf,
    unsigned int   nbelem
)
{
    BIND_CALL_NULL_FORBIDDEN
    (
        OCI_IPC_COLLECTION, sizeof(OCIColl*), OCI_CDT_COLLECTION, SQLT_NTY, 0, typinf, nbelem
    )
}

/* --------------------------------------------------------------------------------------------- *
 * StatementBindStatement
 * --------------------------------------------------------------------------------------------- */

boolean StatementBindStatement
(
    OCI_Statement *stmt,
    const otext   *name,
    OCI_Statement *data
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    )

    CHECK_BIND(stmt, name, data, OCI_IPC_STATEMENT, TRUE)

    BIND_DATA(sizeof(OCIStmt*), OCI_CDT_CURSOR, SQLT_RSET, 0, NULL, 0)

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * StatementBindLong
 * --------------------------------------------------------------------------------------------- */

boolean StatementBindLong
(
    OCI_Statement *stmt,
    const otext   *name,
    OCI_Long      *data,
    unsigned int   size
)
{
    BIND_CALL_NULL_FORBIDDEN
    (
        OCI_IPC_LONG,
        size, OCI_CDT_LONG,
        ExternalSubTypeToSQLType(OCI_CDT_LONG, data->type),
        data->type, NULL, 0
    )
}

/* --------------------------------------------------------------------------------------------- *
* OCI_RegisterNumber
* --------------------------------------------------------------------------------------------- */

boolean StatementRegisterNumber
(
    OCI_Statement *stmt,
    const otext   *name
)
{
    REGISTER_CALL(sizeof(OCINumber), OCI_CDT_NUMERIC, SQLT_VNU, OCI_NUM_NUMBER, NULL, 0)
}

/* --------------------------------------------------------------------------------------------- *
 * StatementRegisterShort
 * --------------------------------------------------------------------------------------------- */

boolean StatementRegisterShort
(
    OCI_Statement *stmt,
    const otext   *name
)
{
    REGISTER_CALL(sizeof(OCINumber), OCI_CDT_NUMERIC, SQLT_VNU, OCI_NUM_SHORT, NULL, 0)
}

/* --------------------------------------------------------------------------------------------- *
 * StatementRegisterUnsignedShort
 * --------------------------------------------------------------------------------------------- */

boolean StatementRegisterUnsignedShort
(
    OCI_Statement *stmt,
    const otext   *name
)
{
    REGISTER_CALL(sizeof(OCINumber), OCI_CDT_NUMERIC, SQLT_VNU, OCI_NUM_USHORT, NULL, 0)
}

/* --------------------------------------------------------------------------------------------- *
 * StatementRegisterInt
 * --------------------------------------------------------------------------------------------- */

boolean StatementRegisterInt
(
    OCI_Statement *stmt,
    const otext   *name
)
{
    REGISTER_CALL(sizeof(OCINumber), OCI_CDT_NUMERIC, SQLT_VNU, OCI_NUM_INT, NULL, 0)
}

/* --------------------------------------------------------------------------------------------- *
 * StatementRegisterUnsignedInt
 * --------------------------------------------------------------------------------------------- */

boolean StatementRegisterUnsignedInt
(
    OCI_Statement *stmt,
    const otext   *name
)
{
    REGISTER_CALL(sizeof(OCINumber), OCI_CDT_NUMERIC, SQLT_VNU, OCI_NUM_UINT, NULL, 0)
}

/* --------------------------------------------------------------------------------------------- *
 * StatementRegisterBigInt
 * --------------------------------------------------------------------------------------------- */

boolean StatementRegisterBigInt
(
    OCI_Statement *stmt,
    const otext   *name
)
{
    REGISTER_CALL(sizeof(OCINumber), OCI_CDT_NUMERIC, SQLT_VNU, OCI_NUM_BIGINT, NULL, 0)
}

/* --------------------------------------------------------------------------------------------- *
 * StatementRegisterUnsignedBigInt
 * --------------------------------------------------------------------------------------------- */

boolean StatementRegisterUnsignedBigInt
(
    OCI_Statement *stmt,
    const otext   *name
)
{
    REGISTER_CALL(sizeof(OCINumber), OCI_CDT_NUMERIC, SQLT_VNU, OCI_NUM_BIGUINT, NULL, 0)
}

/* --------------------------------------------------------------------------------------------- *
 * StatementRegisterString
 * --------------------------------------------------------------------------------------------- */

boolean StatementRegisterString
(
    OCI_Statement *stmt,
    const otext   *name,
    unsigned int   len
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    )

    CHECK_REGISTER(stmt, name)
    CHECK_MIN(len, 1)

    const int size = (len + 1) * (ub4) sizeof(dbtext);

    REGISTER_DATA(size, OCI_CDT_TEXT, SQLT_STR, 0, NULL, 0)

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * StatementRegisterRaw
 * --------------------------------------------------------------------------------------------- */

boolean StatementRegisterRaw
(
    OCI_Statement *stmt,
    const otext   *name,
    unsigned int   len
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    )

    CHECK_REGISTER(stmt, name)
    CHECK_MIN(len, 1)

    REGISTER_DATA(len, OCI_CDT_RAW, SQLT_BIN, 0, NULL, 0)

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * StatementRegisterDouble
 * --------------------------------------------------------------------------------------------- */

boolean StatementRegisterDouble
(
    OCI_Statement *stmt,
    const otext   *name
)
{
    REGISTER_CALL(sizeof(OCINumber), OCI_CDT_NUMERIC, SQLT_VNU, OCI_NUM_DOUBLE, NULL, 0)
}

/* --------------------------------------------------------------------------------------------- *
 * StatementRegisterFloat
 * --------------------------------------------------------------------------------------------- */

boolean StatementRegisterFloat
(
    OCI_Statement *stmt,
    const otext   *name
)
{
    REGISTER_CALL(sizeof(OCINumber), OCI_CDT_NUMERIC, SQLT_VNU, OCI_NUM_FLOAT, NULL, 0)
}

/* --------------------------------------------------------------------------------------------- *
 * StatementRegisterDate
 * --------------------------------------------------------------------------------------------- */

boolean StatementRegisterDate
(
    OCI_Statement *stmt,
    const otext   *name
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    )

    unsigned int code = SQLT_ODT;
    unsigned int size = sizeof(OCIDate);

    CHECK_REGISTER(stmt, name)

    /* versions of OCI (< 10.2) crashes if SQLT_ODT is passed for output
       data with returning clause.
       It's an Oracle known bug #3269146 */

    if (ConnectionGetVersion(stmt->con) < OCI_10_2)
    {
        code = SQLT_DAT;
        size = 7;
    }

    REGISTER_DATA(size, OCI_CDT_DATETIME, code, 0, NULL, 0)

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * StatementRegisterTimestamp
 * --------------------------------------------------------------------------------------------- */

boolean StatementRegisterTimestamp
(
    OCI_Statement *stmt,
    const otext   *name,
    unsigned int   type
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    )

    CHECK_REGISTER(stmt, name)
    CHECK_TIMESTAMP_ENABLED(stmt->con)

#if OCI_VERSION_COMPILE >= OCI_9_0

    CHECK_ENUM_VALUE(type, TimestampTypeValues, OTEXT("Timestamp type"))

    REGISTER_DATA(sizeof(OCIDateTime *), OCI_CDT_TIMESTAMP,
                  ExternalSubTypeToSQLType(OCI_CDT_TIMESTAMP, type),
                  type, NULL, 0)

#endif

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * StatementRegisterInterval
 * --------------------------------------------------------------------------------------------- */

boolean StatementRegisterInterval
(
    OCI_Statement *stmt,
    const otext   *name,
    unsigned int   type
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    )

    CHECK_REGISTER(stmt, name)
    CHECK_INTERVAL_ENABLED(stmt->con)

#if OCI_VERSION_COMPILE >= OCI_9_0

    CHECK_ENUM_VALUE(type, IntervalTypeValues, OTEXT("Interval type"))

    REGISTER_DATA(sizeof(OCIInterval *), OCI_CDT_INTERVAL,
                  ExternalSubTypeToSQLType(OCI_CDT_INTERVAL, type),
                  type, NULL, 0)

#endif

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * StatementRegisterObject
 * --------------------------------------------------------------------------------------------- */

boolean StatementRegisterObject
(
    OCI_Statement *stmt,
    const otext   *name,
    OCI_TypeInfo  *typinf
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    )

    CHECK_REGISTER(stmt, name)
    CHECK_PTR(OCI_IPC_TYPE_INFO, typinf)

    REGISTER_DATA(sizeof(void *), OCI_CDT_OBJECT, SQLT_NTY, 0, typinf, 0)

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * StatementRegisterLob
 * --------------------------------------------------------------------------------------------- */

boolean StatementRegisterLob
(
    OCI_Statement *stmt,
    const otext   *name,
    unsigned int   type
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    )

    CHECK_REGISTER(stmt, name)
    CHECK_ENUM_VALUE(type, LobTypeValues, OTEXT("Lob type"))

    REGISTER_DATA(sizeof(OCILobLocator*), OCI_CDT_LOB,
                  ExternalSubTypeToSQLType(OCI_CDT_LOB, type),
                  type, NULL, 0)

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * StatementRegisterFile
 * --------------------------------------------------------------------------------------------- */

boolean StatementRegisterFile
(
    OCI_Statement *stmt,
    const otext   *name,
    unsigned int   type
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    )

    CHECK_REGISTER(stmt, name)
    CHECK_ENUM_VALUE(type, FileTypeValues, OTEXT("File type"))

    REGISTER_DATA(sizeof(OCILobLocator*), OCI_CDT_FILE,
                  ExternalSubTypeToSQLType(OCI_CDT_FILE, type),
                  type, NULL, 0)

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * StatementRegisterReference
 * --------------------------------------------------------------------------------------------- */

boolean StatementRegisterReference
(
    OCI_Statement *stmt,
    const otext   *name,
    OCI_TypeInfo  *typinf
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    )

    CHECK_REGISTER(stmt, name)
    CHECK_PTR(OCI_IPC_TYPE_INFO, typinf)

    REGISTER_DATA(sizeof(OCIRef *), OCI_CDT_REF, SQLT_REF, 0, typinf, 0)

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * StatementGetStatementType
 * --------------------------------------------------------------------------------------------- */

unsigned int StatementGetStatementType
(
    OCI_Statement *stmt
)
{
    GET_PROP
    (
        unsigned int, OCI_UNKNOWN,
        OCI_IPC_STATEMENT, stmt,
        type
    )
}

/* --------------------------------------------------------------------------------------------- *
 * StatementSetFetchMode
 * --------------------------------------------------------------------------------------------- */

boolean StatementSetFetchMode
(
    OCI_Statement *stmt,
    unsigned int   mode
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    )
    unsigned int old_exec_mode = OCI_UNKNOWN;

    CHECK_PTR(OCI_IPC_STATEMENT, stmt)
    CHECK_SCROLLABLE_CURSOR_ENABLED(stmt->con)
    CHECK_ENUM_VALUE(mode, FetchModeValues, OTEXT("Fetch mode"))

    old_exec_mode   = stmt->exec_mode;
    stmt->exec_mode = mode;

    if (stmt->con->ver_num == OCI_9_0)
    {
        if (old_exec_mode == OCI_SFM_DEFAULT && stmt->exec_mode == OCI_SFM_SCROLLABLE)
        {
            /* Disabling prefetch that causes bugs for 9iR1 for scrollable cursors */
            StatementSetPrefetchSize(stmt, 0);
        }
        else if (old_exec_mode == OCI_SFM_SCROLLABLE && stmt->exec_mode == OCI_SFM_DEFAULT)
        {
            /* Re-enable prefetch previously disabled */
            StatementSetPrefetchSize(stmt, OCI_PREFETCH_SIZE);
        }
    }

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * StatementGetFetchMode
 * --------------------------------------------------------------------------------------------- */

unsigned int StatementGetFetchMode
(
    OCI_Statement *stmt
)
{
    GET_PROP
    (
        unsigned int, OCI_UNKNOWN,
        OCI_IPC_STATEMENT, stmt,
        exec_mode
    )
}

/* --------------------------------------------------------------------------------------------- *
 * StatementSetBindMode
 * --------------------------------------------------------------------------------------------- */

boolean StatementSetBindMode
(
    OCI_Statement *stmt,
    unsigned int   mode
)
{
    SET_PROP_ENUM
    (
        /* handle */ OCI_IPC_STATEMENT, stmt,
        /* member */ bind_mode, ub1,
        /* value  */ mode, BindModeValues, OTEXT("Bind Mode")
    )
}

/* --------------------------------------------------------------------------------------------- *
 * StatementGetBindMode
 * --------------------------------------------------------------------------------------------- */

unsigned int StatementGetBindMode
(
    OCI_Statement *stmt
)
{
    GET_PROP
    (
        unsigned int, OCI_UNKNOWN,
        OCI_IPC_STATEMENT, stmt,
        bind_mode
    )
}

/* --------------------------------------------------------------------------------------------- *
 * StatementSetBindAllocation
 * --------------------------------------------------------------------------------------------- */

boolean StatementSetBindAllocation
(
    OCI_Statement *stmt,
    unsigned int   mode
)
{
    SET_PROP_ENUM
    (
        /* handle */ OCI_IPC_STATEMENT, stmt,
        /* member */ bind_alloc_mode, ub1,
        /* value  */ mode, BindAllocationValues, OTEXT("Bind Allocation")
    )
}

/* --------------------------------------------------------------------------------------------- *
 * StatementGetBindAllocation
 * --------------------------------------------------------------------------------------------- */

unsigned int StatementGetBindAllocation
(
    OCI_Statement *stmt
)
{
    GET_PROP
    (
        unsigned int, OCI_UNKNOWN,
        OCI_IPC_STATEMENT, stmt,
        bind_alloc_mode
    )
}

/* --------------------------------------------------------------------------------------------- *
 * StatementSetFetchSize
 * --------------------------------------------------------------------------------------------- */

boolean StatementSetFetchSize
(
    OCI_Statement *stmt,
    unsigned int   size
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    )

    CHECK_PTR(OCI_IPC_STATEMENT, stmt)
    CHECK_MIN(size, 1)

    stmt->fetch_size = size;

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * StatementGetFetchSize
 * --------------------------------------------------------------------------------------------- */

unsigned int StatementGetFetchSize
(
    OCI_Statement *stmt
)
{
    GET_PROP
    (
        unsigned int, 0,
        OCI_IPC_STATEMENT, stmt,
        fetch_size
    )
}

/* --------------------------------------------------------------------------------------------- *
 * StatementPrefetchSize
 * --------------------------------------------------------------------------------------------- */

boolean StatementSetPrefetchSize
(
    OCI_Statement *stmt,
    unsigned int   size
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    )

    CHECK_PTR(OCI_IPC_STATEMENT, stmt)

    stmt->prefetch_size = size;

    /* Prefetch is not working with scrollable cursors in Oracle 9iR1, thus disable it */

    if (stmt->exec_mode == OCI_SFM_SCROLLABLE && stmt->con->ver_num == OCI_9_0)
    {
        stmt->prefetch_size = 0;
    }

    if (stmt->stmt)
    {
        CHECK_ATTRIB_SET
        (
            OCI_HTYPE_STMT, OCI_ATTR_PREFETCH_ROWS,
            stmt->stmt, &stmt->prefetch_size, sizeof(stmt->prefetch_size),
            stmt->con->err
        )
    }

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * StatementGetPrefetchSize
 * --------------------------------------------------------------------------------------------- */

unsigned int StatementGetPrefetchSize
(
    OCI_Statement *stmt
)
{
    GET_PROP
    (
        unsigned int, 0,
        OCI_IPC_STATEMENT, stmt,
        prefetch_size
    )
}

/* --------------------------------------------------------------------------------------------- *
 * StatementSetPrefetchMemory
 * --------------------------------------------------------------------------------------------- */

boolean StatementSetPrefetchMemory
(
    OCI_Statement *stmt,
    unsigned int   size
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    )

    CHECK_PTR(OCI_IPC_STATEMENT, stmt)

    stmt->prefetch_mem = size;

    if (stmt->stmt)
    {
        CHECK_ATTRIB_SET
        (
            OCI_HTYPE_STMT, OCI_ATTR_PREFETCH_MEMORY,
            stmt->stmt, &stmt->prefetch_mem, sizeof(stmt->prefetch_mem),
            stmt->con->err
        )
    }

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * StatementGetPrefetchMemory
 * --------------------------------------------------------------------------------------------- */

unsigned int StatementGetPrefetchMemory
(
    OCI_Statement *stmt
)
{
    GET_PROP
    (
        unsigned int, 0,
        OCI_IPC_STATEMENT, stmt,
        prefetch_mem
    )
}

/* --------------------------------------------------------------------------------------------- *
 * StatementSetLongMaxSize
 * --------------------------------------------------------------------------------------------- */

boolean StatementSetLongMaxSize
(
    OCI_Statement *stmt,
    unsigned int   size
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    )

    CHECK_PTR(OCI_IPC_STATEMENT, stmt)
    CHECK_MIN(size, 1)

    stmt->long_size = size;

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * StatementGetLongMaxSize
 * --------------------------------------------------------------------------------------------- */

unsigned int StatementGetLongMaxSize
(
    OCI_Statement *stmt
)
{
    GET_PROP
    (
        unsigned int, 0,
        OCI_IPC_STATEMENT, stmt,
        long_size
    )
}

/* --------------------------------------------------------------------------------------------- *
 * StatementSetLongMode
 * --------------------------------------------------------------------------------------------- */

boolean StatementSetLongMode
(
    OCI_Statement *stmt,
    unsigned int   mode
)
{
    SET_PROP_ENUM
    (
        /* handle */ OCI_IPC_STATEMENT, stmt,
        /* member */ long_mode, ub1,
        /* value  */ mode, LongModeValues, OTEXT("Long Mode")
    )
}

/* --------------------------------------------------------------------------------------------- *
 * StatementGetLongMode
 * --------------------------------------------------------------------------------------------- */

unsigned int StatementGetLongMode
(
    OCI_Statement *stmt
)
{
    GET_PROP
    (
        unsigned int, OCI_UNKNOWN,
        OCI_IPC_STATEMENT, stmt,
        long_mode
    )
}

/* --------------------------------------------------------------------------------------------- *
 * StatementGetConnection
 * --------------------------------------------------------------------------------------------- */

OCI_Connection * StatementGetConnection
(
    OCI_Statement *stmt
)
{
    GET_PROP
    (
        OCI_Connection*, NULL,
        OCI_IPC_STATEMENT, stmt,
        con
    )
}

/* --------------------------------------------------------------------------------------------- *
 * StatementGetSql
 * --------------------------------------------------------------------------------------------- */

const otext * StatementGetSql
(
    OCI_Statement *stmt
)
{
    GET_PROP
    (
        const otext*, NULL,
        OCI_IPC_STATEMENT, stmt,
        sql
    )
}

/* --------------------------------------------------------------------------------------------- *
 * StatementGetSqlIdentifier
 * --------------------------------------------------------------------------------------------- */

const otext* StatementGetSqlIdentifier
(
    OCI_Statement *stmt
)
{
    GET_PROP
    (
        const otext*, NULL,
        OCI_IPC_STATEMENT, stmt,
        sql_id
    )
}

/* --------------------------------------------------------------------------------------------- *
 * StatementGetSqlErrorPos
 * --------------------------------------------------------------------------------------------- */

unsigned int StatementGetSqlErrorPos
(
    OCI_Statement *stmt
)
{
    GET_PROP
    (
        unsigned int, 0,
        OCI_IPC_STATEMENT, stmt,
        err_pos
    )
}

/* --------------------------------------------------------------------------------------------- *
 * StatementGetAffectedRows
 * --------------------------------------------------------------------------------------------- */

unsigned int StatementGetAffectedRows
(
    OCI_Statement *stmt
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    )

    ub4 count = 0;

    CHECK_PTR(OCI_IPC_STATEMENT, stmt)

    CHECK_ATTRIB_GET
    (
        OCI_HTYPE_STMT, OCI_ATTR_ROW_COUNT,
        stmt->stmt, &count, NULL,
        stmt->con->err
    )

    SET_RETVAL(count)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * StatementGetBindCount
 * --------------------------------------------------------------------------------------------- */

unsigned int StatementGetBindCount
(
    OCI_Statement *stmt
)
{
    GET_PROP
    (
        unsigned int, 0,
        OCI_IPC_STATEMENT, stmt,
        nb_ubinds
    )
}

/* --------------------------------------------------------------------------------------------- *
 * StatementGetBind
 * --------------------------------------------------------------------------------------------- */

OCI_Bind * StatementGetBind
(
    OCI_Statement *stmt,
    unsigned int   index
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Bind*, NULL,
        /* context */ OCI_IPC_STATEMENT, stmt
    )

    CHECK_PTR(OCI_IPC_STATEMENT, stmt)
    CHECK_BOUND(index, 1, stmt->nb_ubinds)

    SET_RETVAL(stmt->ubinds[index - 1])

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * StatementGetBind2
 * --------------------------------------------------------------------------------------------- */

OCI_Bind * StatementGetBind2
(
    OCI_Statement *stmt,
    const otext   *name
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Bind*, NULL,
        /* context */ OCI_IPC_STATEMENT, stmt
    )

    int index = -1;

    CHECK_PTR(OCI_IPC_STATEMENT, stmt)
    CHECK_PTR(OCI_IPC_STRING,    name)

    index = BindGetIndex(stmt, name);
    if (index <= 0)
    {
        THROW(ExceptionItemNotFound, name, OCI_IPC_BIND)
    }

    SET_RETVAL(stmt->ubinds[index - 1])

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
* GetBindIndex
* --------------------------------------------------------------------------------------------- */

unsigned int StatementGetBindIndex
(
    OCI_Statement *stmt,
    const otext   *name
)
{
    ENTER_FUNC
    (
        /* returns */ unsigned int, 0,
        /* context */ OCI_IPC_STATEMENT, stmt
    )

    int index = -1;

    CHECK_PTR(OCI_IPC_STATEMENT, stmt)
    CHECK_PTR(OCI_IPC_STRING,    name)

    index = BindGetIndex(stmt, name);
    CHECK(index >= 0)

    SET_RETVAL(index)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * StatementGetSqlCommand
 * --------------------------------------------------------------------------------------------- */

unsigned int StatementGetSqlCommand
(
    OCI_Statement *stmt
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    )

    ub2 code = OCI_UNKNOWN;

    CHECK_PTR(OCI_IPC_STATEMENT, stmt)
    CHECK_STMT_STATUS(stmt, OCI_STMT_EXECUTED)

    CHECK_ATTRIB_GET
    (
        OCI_HTYPE_STMT, OCI_ATTR_SQLFNCODE,
        stmt->stmt, &code, NULL,
        stmt->con->err
    )

    SET_RETVAL(code)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * StatementGetSqlVerb
 * --------------------------------------------------------------------------------------------- */

const otext * StatementGetSqlVerb
(
    OCI_Statement *stmt
)
{
    ENTER_FUNC
    (
        /* returns */ const otext*, NULL,
        /* context */ OCI_IPC_STATEMENT, stmt
    )

    unsigned int code = OCI_UNKNOWN;

    CHECK_PTR(OCI_IPC_STATEMENT, stmt)

    code = StatementGetSqlCommand(stmt);

    const otext* verb = NULL;

    if (OCI_UNKNOWN != code)
    {
        for (int i = 0; i < OCI_SQLCMD_COUNT; i++)
        {
            if (code == SQLCmds[i].code)
            {
                verb = SQLCmds[i].verb;
                break;
            }
        }
    }

    SET_RETVAL(verb)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * StatementGetBatchError
 * --------------------------------------------------------------------------------------------- */

OCI_Error * StatementGetBatchError
(
    OCI_Statement *stmt
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Error*, NULL,
        /* context */ OCI_IPC_STATEMENT, stmt
    )

    CHECK_PTR(OCI_IPC_STATEMENT, stmt)

    CHECK(NULL != stmt->batch && stmt->batch->cur < stmt->batch->count)

    SET_RETVAL(&stmt->batch->errs[stmt->batch->cur++])

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * StatementGetBatchErrorCount
 * --------------------------------------------------------------------------------------------- */

unsigned int StatementGetBatchErrorCount
(
    OCI_Statement *stmt
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    )

    CHECK_PTR(OCI_IPC_STATEMENT, stmt)

    CHECK_NULL(stmt->batch)

    SET_RETVAL(stmt->batch->count)

    EXIT_FUNC()
}
