/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2025 Vincent ROGIER <vince.rogier@ocilib.net>
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
#include "stringutils.h"
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
            THROW(OcilibExceptionExternalBindingNotAllowed, (name))          \
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

#define BIND_DATA(...)                                              \
                                                                    \
    CHECK_NULL(OcilibBindCreate(stmt, data, name,                   \
                                OCI_BIND_INPUT, __VA_ARGS__))       \

#define REGISTER_DATA(...)                                          \
                                                                    \
    CHECK_NULL(OcilibBindCreate(stmt, NULL, name,                   \
                                OCI_BIND_OUTPUT, __VA_ARGS__))      \


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
 * OcilibStatementBatchErrorClear
 * --------------------------------------------------------------------------------------------- */

static boolean OcilibStatementBatchErrorClear
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
 * OcilibStatementFreeAllBinds
 * --------------------------------------------------------------------------------------------- */

static boolean OcilibStatementFreeAllBinds
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
            OcilibBindFree(stmt->ubinds[i]);
        }

        FREE(stmt->ubinds)
    }

    /* free register binds */

    if (NULL != stmt->rbinds)
    {
        for(i = 0; i < stmt->nb_rbinds; i++)
        {
            OcilibBindFree(stmt->rbinds[i]);
        }

        FREE(stmt->rbinds)
    }

    stmt->nb_ubinds = 0;
    stmt->nb_rbinds = 0;

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementReset
 * --------------------------------------------------------------------------------------------- */

static boolean OcilibStatementReset
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

        const unsigned int cache_size = OcilibConnectionGetStatementCacheSize(stmt->con);

        if (cache_size > 0)
        {
            mode = OCI_STRLS_CACHE_DELETE;
        }
    }

#else

    OCI_NOT_USED(mode)

#endif

    /* reset batch errors */

    CHECK(OcilibStatementBatchErrorClear(stmt))

    /* free resultsets */

    CHECK(OcilibStatementReleaseResultsets(stmt))

    /* free in/out binds */

    CHECK(OcilibStatementFreeAllBinds(stmt))

    /* free bind map */

    if (NULL != stmt->map)
    {
        CHECK(OcilibHashFree(stmt->map))
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
                OcilibMemoryFreeHandle((dvoid*)stmt->stmt, OCI_HTYPE_STMT);
            }

            stmt->stmt = NULL;
        }
        else if (OCI_OBJECT_ALLOCATED_BIND_STMT == stmt->hstate)
        {
            OcilibMemoryFreeHandle((dvoid*)stmt->stmt, OCI_HTYPE_STMT);

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
* OcilibStatementBindCheck
* --------------------------------------------------------------------------------------------- */

static boolean OcilibStatementBindCheck
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

                CHECK(OcilibNumberTranslateValue(bnd->stmt->con, src_bint,
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

                OcilibStringUTF32ToUTF16(src + src_offset, dst + dst_offset, max_chars - 1);
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
* OcilibStatementBindUpdate
* --------------------------------------------------------------------------------------------- */

static boolean OcilibStatementBindUpdate
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
                if (src_number && bnd->buffer.inds[index] != OCI_IND_NULL)
                {
                    CHECK
                    (
                        OcilibNumberTranslateValue
                        (
                            bnd->stmt->con, 
                            src_number,
                            OCI_NUM_NUMBER,
                            dst_bint, 
                            bnd->subtype
                        )
                    )
                }
                else
                {
                    *dst_bint = 0;
                }
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

            OcilibStringUTF16ToUTF32(src + src_offset, dst + dst_offset, max_chars - 1);
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
 * OcilibStatementBindCheckAll
 * --------------------------------------------------------------------------------------------- */

static boolean OcilibStatementBindCheckAll
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

            OcilibStatementReset(bnd_stmt);

            bnd_stmt->hstate = OCI_OBJECT_ALLOCATED_BIND_STMT;

            /* allocate statement handle */

            CHECK
            (
                OcilibMemoryAllocHandle
                (
                    (dvoid *)bnd_stmt->con->env,
                    (dvoid **)(void *)&bnd_stmt->stmt,
                    OCI_HTYPE_STMT
                )
            )

            CHECK(OcilibStatementSetPrefetchSize(stmt, stmt->prefetch_size))
            CHECK(OcilibStatementSetFetchSize(stmt, stmt->fetch_size))
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
                        CHECK(OcilibStatementBindCheck(bnd, (ub1*)bnd->input, (ub1*)bnd->buffer.data, j))
                    }
                }
                else
                {
                    CHECK(OcilibStatementBindCheck(bnd, (ub1*)bnd->input, (ub1*)bnd->buffer.data, 0))
                }
            }
        }
    }

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementBindUpdateAll
 * --------------------------------------------------------------------------------------------- */

static boolean OcilibStatementBindUpdateAll
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
                        CHECK(OcilibStatementBindUpdate(bnd, (ub1*)bnd->buffer.data, (ub1*)bnd->input, j))
                    }
                }
                else
                {
                    CHECK(OcilibStatementBindUpdate(bnd, (ub1*)bnd->buffer.data, (ub1*)bnd->input, 0))
                }
            }
        }
    }

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementFetchIntoUserVariables
 * --------------------------------------------------------------------------------------------- */

boolean OcilibStatementFetchIntoUserVariables
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

    rs = OcilibStatementGetResultset(stmt);

    /* fetch data */

    if (rs)
    {
        res = OcilibResultsetFetchNext(rs);
    }

    if (res)
    {
        unsigned int i, n;

        /* loop on column list for updating user given placeholders */

        for (i = 1, n = OcilibResultsetGetColumnCount(rs); (i <= n) && res; i++)
        {
            OCI_Column *col = OcilibResultsetGetColumn(rs, i);

            const int type = va_arg(args, int);

            switch (type)
            {
                case OCI_ARG_TEXT:
                {
                    const otext *src = OcilibResultsetGetString(rs, i);
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
                    SET_ARG_NUM(short, OcilibResultsetGetShort);
                    break;
                }
                case OCI_ARG_USHORT:
                {
                    SET_ARG_NUM(unsigned short, OcilibResultsetGetUnsignedShort);
                    break;
                }
                case OCI_ARG_INT:
                {
                    SET_ARG_NUM(int, OcilibResultsetGetInt);
                    break;
                }
                case OCI_ARG_UINT:
                {
                    SET_ARG_NUM(unsigned int, OcilibResultsetGetUnsignedInt);
                    break;
                }
                case OCI_ARG_BIGINT:
                {
                    SET_ARG_NUM(big_int, OcilibResultsetGetBigInt);
                    break;
                }
                case OCI_ARG_BIGUINT:
                {
                    SET_ARG_NUM(big_uint, OcilibResultsetGetUnsignedBigInt);
                    break;
                }
                case OCI_ARG_DOUBLE:
                {
                    SET_ARG_NUM(double, OcilibResultsetGetDouble);
                    break;
                }
                case OCI_ARG_FLOAT:
                {
                    SET_ARG_NUM(float, OcilibResultsetGetFloat);
                    break;
                }
                case OCI_ARG_NUMBER:
                {
                    SET_ARG_HANDLE(OCI_Number, OcilibResultsetGetNumber, OcilibNumberAssign);
                    break;
                }
                case OCI_ARG_DATETIME:
                {
                    SET_ARG_HANDLE(OCI_Date, OcilibResultsetGetDate, OcilibDateAssign);
                    break;
                }
                case OCI_ARG_RAW:
                {
                    OcilibResultsetGetRaw(rs, i, va_arg(args, otext *), col->bufsize);
                    break;
                }
                case OCI_ARG_LOB:
                {
                    SET_ARG_HANDLE(OCI_Lob, OcilibResultsetGetLob, OcilibLobAssign);
                    break;
                }
                case OCI_ARG_FILE:
                {
                    SET_ARG_HANDLE(OCI_File, OcilibResultsetGetFile, OcilibFileAssign);
                    break;
                }
                case OCI_ARG_TIMESTAMP:
                {
                    SET_ARG_HANDLE(OCI_Timestamp, OcilibResultsetGetTimestamp, OcilibTimestampAssign);
                    break;
                }
                case OCI_ARG_INTERVAL:
                {
                    SET_ARG_HANDLE(OCI_Interval, OcilibResultsetGetInterval, OcilibIntervalAssign);
                    break;
                }
                case OCI_ARG_OBJECT:
                {
                    SET_ARG_HANDLE(OCI_Object, OcilibResultsetGetObject, OcilibObjectAssign);
                    break;
                }
                case OCI_ARG_COLLECTION:
                {
                    SET_ARG_HANDLE(OCI_Coll, OcilibResultsetGetColl, OcilibCollectionAssign);
                    break;
                }
                case OCI_ARG_REF:
                {
                    SET_ARG_HANDLE(OCI_Ref, OcilibResultsetGetReference, OcilibReferenceAssign);
                    break;
                }
                default:
                {
                    THROW(OcilibExceptionMappingArgument, type);
                    break;
                }
            }
        }
    }

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementInitialize
 * --------------------------------------------------------------------------------------------- */

OCI_Statement * OcilibStatementInitialize
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
    stmt->piece_size      = OCI_SIZE_PIECE_DYNAMIC_FETCH;
    stmt->bind_reuse      = FALSE;
    stmt->bind_mode       = OCI_BIND_BY_NAME;
    stmt->long_mode       = OCI_LONG_EXPLICIT;
    stmt->bind_alloc_mode = OCI_BAM_EXTERNAL;
    stmt->fetch_size      = OCI_FETCH_SIZE;
    stmt->prefetch_size   = OCI_PREFETCH_SIZE;

    /* reset statement */

    CHECK(OcilibStatementReset(stmt))

    if (is_desc)
    {
        stmt->hstate = OCI_OBJECT_FETCHED_CLEAN;
        stmt->status = OCI_STMT_PREPARED  | OCI_STMT_PARSED |
                       OCI_STMT_DESCRIBED | OCI_STMT_EXECUTED;
        stmt->type = OCI_CST_SELECT;

        if (NULL != sql)
        {
            stmt->sql = OcilibStringDuplicate(sql);
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
                stmt->sql = OcilibStringDuplicateFromDBString(dbstr, dbcharcount(dbsize));
                CHECK_NULL(stmt->sql)
            }
        }

        /* Setting fetch attributes here as the statement is already prepared */

        CHECK(OcilibStatementSetPrefetchSize(stmt, stmt->prefetch_size))
        CHECK(OcilibStatementSetFetchSize(stmt, stmt->fetch_size))
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
            OcilibStatementFree(stmt);
            stmt = NULL;
        }

        SET_RETVAL(stmt)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementDispose
 * --------------------------------------------------------------------------------------------- */

boolean OcilibStatementDispose
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

    OcilibStatementReset(stmt);

    OcilibErrorResetSource(NULL, stmt);

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementCheckImplicitResultsets
 * --------------------------------------------------------------------------------------------- */

static boolean OcilibStatementCheckImplicitResultsets
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
                    stmt->stmts[i] = OcilibStatementInitialize(stmt->con, NULL, result, TRUE, NULL);

                    CHECK_NULL(stmt->stmts[i])

                    stmt->rsts[i] = OcilibResultsetCreate(stmt->stmts[i], stmt->stmts[i]->fetch_size);

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
 * OcilibStatementBatchErrorsInit
 * --------------------------------------------------------------------------------------------- */

static boolean OcilibStatementBatchErrorInit
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

    CHECK(OcilibStatementBatchErrorClear(stmt))

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

        CHECK(OcilibMemoryAllocHandle((dvoid  *)stmt->con->env, (dvoid **)(void *)&hndl, OCI_HTYPE_ERROR))

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

                dbtext * err_msg = OcilibStringGetDBString(buffer, &err_size);

                OCIAttrGet((dvoid *)hndl, (ub4)OCI_HTYPE_ERROR,
                           (void *)&row, (ub4 *)NULL,
                           (ub4)OCI_ATTR_DML_ROW_OFFSET, stmt->con->err);

                OCIErrorGet((dvoid *)hndl, (ub4)1, (OraText *)NULL, &err_code,
                            (OraText *)err_msg, (ub4)err_size, (ub4)OCI_HTYPE_ERROR);

                OcilibErrorSet
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

                OcilibStringReleaseDBString(err_msg);
            }
        }
    }

    SET_SUCCESS()

    CLEANUP_AND_EXIT_FUNC
    (
        if (NULL != hndl)
        {
            OcilibMemoryFreeHandle(hndl, OCI_HTYPE_ERROR);
        }
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementPrepareInternal
 * --------------------------------------------------------------------------------------------- */

boolean OcilibStatementPrepareInternal
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

    CHECK(OcilibStatementReset(stmt))

    /* store SQL */

    stmt->sql = OcilibStringDuplicate(sql);

    dbstr = OcilibStringGetDBString(stmt->sql, &dbsize);

    if (Env.version_runtime < OCI_9_2)
    {
        /* allocate handle */

        CHECK(OcilibMemoryAllocHandle((dvoid *)stmt->con->env, (dvoid **)(void *)&stmt->stmt, OCI_HTYPE_STMT))
    }

    /* prepare SQL */

#if OCI_VERSION_COMPILE >= OCI_9_2

    if (Env.version_runtime >= OCI_9_2)
    {
        ub4 mode = OCI_DEFAULT;

  #if OCI_VERSION_COMPILE >= OCI_12_2

        if (OcilibConnectionIsVersionSupported(stmt->con, OCI_12_2))
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

    CHECK(OcilibStatementSetPrefetchSize(stmt, stmt->prefetch_size))
    CHECK(OcilibStatementSetFetchSize(stmt, stmt->fetch_size))

    SET_SUCCESS()

    CLEANUP_AND_EXIT_FUNC
    (
        OcilibStringReleaseDBString(dbstr);
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementExecuteInternal
 * --------------------------------------------------------------------------------------------- */

boolean OcilibStatementExecuteInternal
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

    CHECK(OcilibStatementBatchErrorClear(stmt))

    /* check bind objects for updating their null indicator status */

    CHECK(OcilibStatementBindCheckAll(stmt))

    /* check current resultsets */

    if (stmt->rsts)
    {
        /* resultsets are freed before any prepare operations.
           So, if we got ones here, it means the same SQL order
           is re-executed */

        if (OCI_CST_SELECT == stmt->type)
        {
            /* just reinitialize the current resultset */

            CHECK(OcilibResultsetInitialize(stmt->rsts[0]))
        }
        else
        {
            /* Must free previous resultsets for 'returning into'
               SQL orders that can produce multiple resultsets */

            CHECK(OcilibStatementReleaseResultsets(stmt))
        }
    }

    /* Oracle execute call */

    const sword ret = OCIStmtExecute(stmt->con->cxt, stmt->stmt, stmt->con->err, 
                                     iters, (ub4)0, (OCISnapshot *)NULL, 
                                     (OCISnapshot *)NULL, mode);

    /* check result */

    boolean success = ((OCI_SUCCESS           == ret) ||
                       (OCI_SUCCESS_WITH_INFO == ret) ||
                       (OCI_NEED_DATA         == ret));
    
    /* handle no data as error only for PL/SQL statements as it results from NO_DATA_FOUND exception in PL/SQL */

    if (!success && !IS_PLSQL_STMT(stmt->type))
    {
        success = (OCI_NO_DATA == ret);
    }

    if (OCI_SUCCESS_WITH_INFO == ret)
    {
        OcilibExceptionOCI(&call_context, stmt->con->err, ret);
    }

    /* on batch mode, check if any error occurred */

    if (mode & OCI_BATCH_ERRORS)
    {
        /* build batch error list if the statement is array DML */

        OcilibStatementBatchErrorInit(stmt);

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

            if (OcilibConnectionIsVersionSupported(stmt->con, OCI_12_2) && stmt->sql_id == NULL)
            {
                unsigned int size_id = 0;

                OcilibStringGetAttribute(stmt->con, stmt->stmt, OCI_HTYPE_STMT, 
                                         OCI_ATTR_SQL_ID, &stmt->sql_id, &size_id);
            }

#endif

            /* reset binds indicators */

            CHECK(OcilibStatementBindUpdateAll(stmt))

            /* commit if necessary */

            if (stmt->con->autocom)
            {
                CHECK(OcilibConnectionCommit(stmt->con))
            }

            /* check if any implicit results are available */

            CHECK(OcilibStatementCheckImplicitResultsets(stmt))

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

        THROW(OcilibExceptionOCI, stmt->con->err, ret)
    }

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Statement * OcilibStatementCreate
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

    OCI_Statement *stmt = NULL;
    
    LIST_ATOMIC_ADD(con->stmts, stmt)

    CHECK_NULL(stmt)

    SET_RETVAL(OcilibStatementInitialize(con, (OCI_Statement*)stmt, NULL, FALSE, NULL))

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementFree
 * --------------------------------------------------------------------------------------------- */

boolean OcilibStatementFree
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

    LIST_ATOMIC_REMOVE(stmt->con->stmts, stmt, OcilibStatementDispose)

    FREE(stmt)

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementGetResultset
 * --------------------------------------------------------------------------------------------- */

OCI_Resultset * OcilibStatementGetResultset
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

            rs = stmt->rsts[0] = OcilibResultsetCreate(stmt, stmt->fetch_size);
        }

        CHECK_NULL(rs)
    }

    SET_RETVAL(rs)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementGetNextResultset
 * --------------------------------------------------------------------------------------------- */

OCI_Resultset * OcilibStatementGetNextResultset
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
 * OcilibStatementReleaseResultsets
 * --------------------------------------------------------------------------------------------- */

boolean OcilibStatementReleaseResultsets
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
                OcilibStatementDispose(stmt->stmts[i]);
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
                OcilibResultsetFree(stmt->rsts[i]);
            }
        }

        FREE(stmt->rsts)
    }

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementPrepare
 * --------------------------------------------------------------------------------------------- */

boolean OcilibStatementPrepare
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

    CHECK(OcilibStatementPrepareInternal(stmt, sql))

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementExecute
 * --------------------------------------------------------------------------------------------- */

boolean OcilibStatementExecute
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

    CHECK(OcilibStatementExecuteInternal(stmt, OCI_DEFAULT))

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementExecuteStmt
 * --------------------------------------------------------------------------------------------- */

boolean OcilibStatementExecuteStmt
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

    CHECK(OcilibStatementPrepareInternal(stmt, sql))
    CHECK(OcilibStatementExecuteInternal(stmt, OCI_DEFAULT))

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementParse
 * --------------------------------------------------------------------------------------------- */

boolean OcilibStatementParse
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

    CHECK(OcilibStatementPrepareInternal(stmt, sql))
    CHECK(OcilibStatementExecuteInternal(stmt, OCI_PARSE_ONLY))

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementDescribe
 * --------------------------------------------------------------------------------------------- */

boolean OcilibStatementDescribe
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

    CHECK(OcilibStatementPrepareInternal(stmt, sql))

    if (OCI_CST_SELECT == stmt->type)
    {
        CHECK(OcilibStatementExecuteInternal(stmt, OCI_DESCRIBE_ONLY))
    }

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementPrepareFmt
 * --------------------------------------------------------------------------------------------- */

boolean OcilibStatementPrepareFmt
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

    const int size = OcilibFormatParseSql(stmt, NULL, sql, &first_pass_args);

    if (size > 0)
    {
        /* allocate buffer */

        ALLOC_DATA(OCI_IPC_STRING, sql_fmt, size + 1)

        /* format buffer */

        if (OcilibFormatParseSql(stmt, sql_fmt, sql, &second_pass_args) > 0)
        {
            /* parse buffer */

            CHECK(OcilibStatementPrepareInternal(stmt, sql_fmt))
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
 * OcilibStatementExecuteStmtFmt
 * --------------------------------------------------------------------------------------------- */

boolean OcilibStatementExecuteStmtFmt
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

    const int size = OcilibFormatParseSql(stmt, NULL, sql, &first_pass_args);

    if (size > 0)
    {

        /* allocate buffer */

        ALLOC_DATA(OCI_IPC_STRING, sql_fmt, size + 1)

        /* format buffer */

        if (OcilibFormatParseSql(stmt, sql_fmt, sql, &second_pass_args) > 0)
        {
            /* prepare and execute SQL buffer */

            CHECK(OcilibStatementPrepareInternal(stmt, sql_fmt))
            CHECK(OcilibStatementExecuteInternal(stmt, OCI_DEFAULT))
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
 * OcilibStatementParseFmt
 * --------------------------------------------------------------------------------------------- */

boolean OcilibStatementParseFmt
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

    const int size = OcilibFormatParseSql(stmt, NULL, sql, &first_pass_args);

    if (size > 0)
    {
        /* allocate buffer */

        ALLOC_DATA(OCI_IPC_STRING, sql_fmt, size + 1)

        /* format buffer */

        if (OcilibFormatParseSql(stmt, sql_fmt, sql, &second_pass_args) > 0)
        {
            /* prepare and execute SQL buffer */

            CHECK(OcilibStatementPrepareInternal(stmt, sql_fmt))
            CHECK(OcilibStatementExecuteInternal(stmt, OCI_PARSE_ONLY))
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
 * OcilibStatementDescribeFmt
 * --------------------------------------------------------------------------------------------- */

boolean OcilibStatementDescribeFmt
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

    const int size = OcilibFormatParseSql(stmt, NULL, sql, &first_pass_args);

    if (size > 0)
    {
        /* allocate buffer */

        ALLOC_DATA(OCI_IPC_STRING, sql_fmt, size + 1)

        /* format buffer */

        if (OcilibFormatParseSql(stmt, sql_fmt, sql, &second_pass_args) > 0)
        {
            /* prepare and execute SQL buffer */

            CHECK(OcilibStatementPrepareInternal(stmt, sql_fmt))
            CHECK(OcilibStatementExecuteInternal(stmt, OCI_DESCRIBE_ONLY))
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
 * OcilibStatementSetBindArraySize
 * --------------------------------------------------------------------------------------------- */

boolean OcilibStatementSetBindArraySize
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
        THROW(OcilibExceptionBindArraySize, stmt->nb_iters_init, stmt->nb_iters, size)
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
 * OcilibStatementGetBindArraySize
 * --------------------------------------------------------------------------------------------- */

unsigned int OcilibStatementGetBindArraySize
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
 * OcilibStatementAllowRebinding
 * --------------------------------------------------------------------------------------------- */

boolean OcilibStatementAllowRebinding
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
 * OcilibStatementIsRebindingAllowed
 * --------------------------------------------------------------------------------------------- */

boolean OcilibStatementIsRebindingAllowed
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
* OcilibStatementBindBoolean
* --------------------------------------------------------------------------------------------- */

boolean OcilibStatementBindBoolean
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
 * OcilibStatementBindNumber
 * --------------------------------------------------------------------------------------------- */

boolean OcilibStatementBindNumber
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
* OcilibStatementBindArrayOfNumbers
* --------------------------------------------------------------------------------------------- */

boolean OcilibStatementBindArrayOfNumbers
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
 * OcilibStatementBindShort
 * --------------------------------------------------------------------------------------------- */

boolean OcilibStatementBindShort
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
 * OcilibStatementBindArrayOfShorts
 * --------------------------------------------------------------------------------------------- */

boolean OcilibStatementBindArrayOfShorts
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
 * OcilibStatementBindUnsignedShort
 * --------------------------------------------------------------------------------------------- */

boolean OcilibStatementBindUnsignedShort
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
 * OcilibStatementBindArrayOfUnsignedShorts
 * --------------------------------------------------------------------------------------------- */

boolean OcilibStatementBindArrayOfUnsignedShorts
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
 * OcilibStatementBindInt
 * --------------------------------------------------------------------------------------------- */

boolean OcilibStatementBindInt
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
 * OcilibStatementBindArrayOfInts
 * --------------------------------------------------------------------------------------------- */

boolean OcilibStatementBindArrayOfInts
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
 * OcilibStatementBindUnsignedInt
 * --------------------------------------------------------------------------------------------- */

boolean OcilibStatementBindUnsignedInt
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
 * OcilibStatementBindArrayOfUnsignedInts
 * --------------------------------------------------------------------------------------------- */

boolean OcilibStatementBindArrayOfUnsignedInts
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
 * OcilibStatementBindBigInt
 * --------------------------------------------------------------------------------------------- */

boolean OcilibStatementBindBigInt
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
 * OcilibStatementBindArrayOfBigInts
 * --------------------------------------------------------------------------------------------- */

boolean OcilibStatementBindArrayOfBigInts
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
 * OcilibStatementBindUnsignedBigInt
 * --------------------------------------------------------------------------------------------- */

boolean OcilibStatementBindUnsignedBigInt
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
 * OcilibStatementBindArrayOfUnsignedInts
 * --------------------------------------------------------------------------------------------- */

boolean OcilibStatementBindArrayOfUnsignedBigInts
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
 * OcilibStatementBindString
 * --------------------------------------------------------------------------------------------- */

boolean OcilibStatementBindString
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

            THROW(OcilibExceptionMinimumValue, 1)
        }
    }

    const unsigned int size = (len + 1) * (ub4) sizeof(dbtext);

    BIND_DATA(size, OCI_CDT_TEXT, SQLT_STR, 0, NULL, 0)

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementBindArrayOfStrings
 * --------------------------------------------------------------------------------------------- */

boolean OcilibStatementBindArrayOfStrings
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
 * OcilibStatementBindRaw
 * --------------------------------------------------------------------------------------------- */

boolean OcilibStatementBindRaw
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

        THROW(OcilibExceptionMinimumValue, 1)
    }

    BIND_DATA(len, OCI_CDT_RAW, SQLT_BIN, 0, NULL, 0)

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementBindArrayOfRaws
 * --------------------------------------------------------------------------------------------- */

boolean OcilibStatementBindArrayOfRaws
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
 * OcilibStatementBindDouble
 * --------------------------------------------------------------------------------------------- */

boolean OcilibStatementBindDouble
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

    if (OcilibConnectionIsVersionSupported(stmt->con, OCI_10_1))
    {
        code = SQLT_BDOUBLE;
    }

#endif

    BIND_DATA(sizeof(double), OCI_CDT_NUMERIC, code, OCI_NUM_DOUBLE, NULL, 0)

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementBindArrayOfDoubles
 * --------------------------------------------------------------------------------------------- */

boolean OcilibStatementBindArrayOfDoubles
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

    if (OcilibConnectionIsVersionSupported(stmt->con, OCI_10_1))
    {
        code = SQLT_BDOUBLE;
    }

#endif

    BIND_DATA(sizeof(double), OCI_CDT_NUMERIC, code, OCI_NUM_DOUBLE, NULL, nbelem)

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementBindFloat
 * --------------------------------------------------------------------------------------------- */

boolean OcilibStatementBindFloat
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

    if (OcilibConnectionIsVersionSupported(stmt->con, OCI_10_1))
    {
        code = SQLT_BFLOAT;
    }

#endif

    BIND_DATA(sizeof(float), OCI_CDT_NUMERIC, code, OCI_NUM_FLOAT, NULL, 0)

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementBindArrayOfFloats
 * --------------------------------------------------------------------------------------------- */

boolean OcilibStatementBindArrayOfFloats
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

    if (OcilibConnectionIsVersionSupported(stmt->con, OCI_10_1))
    {
        code = SQLT_BFLOAT;
    }

#endif

    BIND_DATA(sizeof(float), OCI_CDT_NUMERIC, code, OCI_NUM_FLOAT, NULL, nbelem)

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementBindDate
 * --------------------------------------------------------------------------------------------- */

boolean OcilibStatementBindDate
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
 * OcilibStatementBindArrayOfDates
 * --------------------------------------------------------------------------------------------- */

boolean OcilibStatementBindArrayOfDates
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
 * OcilibStatementBindTimestamp
 * --------------------------------------------------------------------------------------------- */

boolean OcilibStatementBindTimestamp
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
              OcilibExternalSubTypeToSQLType(OCI_CDT_TIMESTAMP, data->type),
              data->type, NULL, 0)

#endif

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementBindArrayOfTimestamps
 * --------------------------------------------------------------------------------------------- */

boolean OcilibStatementBindArrayOfTimestamps
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
              OcilibExternalSubTypeToSQLType(OCI_CDT_TIMESTAMP, type),
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
 * OcilibStatementBindInterval
 * --------------------------------------------------------------------------------------------- */

boolean OcilibStatementBindInterval
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
              OcilibExternalSubTypeToSQLType(OCI_CDT_INTERVAL, data->type),
              data->type, NULL, 0)

#else

    OCI_NOT_USED(name)

#endif

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementBindArrayOfIntervals
 * --------------------------------------------------------------------------------------------- */

boolean OcilibStatementBindArrayOfIntervals
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
              OcilibExternalSubTypeToSQLType(OCI_CDT_INTERVAL, type),
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
 * OcilibStatementBindObject
 * --------------------------------------------------------------------------------------------- */

boolean OcilibStatementBindObject
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
 * OcilibStatementBindArrayOfObjects
 * --------------------------------------------------------------------------------------------- */

boolean OcilibStatementBindArrayOfObjects
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
 * OcilibStatementBindLob
 * --------------------------------------------------------------------------------------------- */

boolean OcilibStatementBindLob
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
        OcilibExternalSubTypeToSQLType(OCI_CDT_LOB, data->type),
        data->type, NULL, 0
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementBindArrayOfLobs
 * --------------------------------------------------------------------------------------------- */

boolean OcilibStatementBindArrayOfLobs
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
              OcilibExternalSubTypeToSQLType(OCI_CDT_LOB, type),
              type, NULL, nbelem)

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementBindFile
 * --------------------------------------------------------------------------------------------- */

boolean OcilibStatementBindFile
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
        OcilibExternalSubTypeToSQLType(OCI_CDT_FILE, data->type),
        data->type, NULL, 0
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementBindArrayOfFiles
 * --------------------------------------------------------------------------------------------- */

boolean OcilibStatementBindArrayOfFiles
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
              OcilibExternalSubTypeToSQLType(OCI_CDT_FILE, type),
              type, NULL, nbelem)

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementBindReference
 * --------------------------------------------------------------------------------------------- */

boolean OcilibStatementBindReference
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
 * OcilibStatementBindArrayOfReferences
 * --------------------------------------------------------------------------------------------- */

boolean OcilibStatementBindArrayOfReferences
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
 * OcilibStatementBindCollection
 * --------------------------------------------------------------------------------------------- */

boolean OcilibStatementBindCollection
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
 * OcilibStatementBindArrayOfCollections
 * --------------------------------------------------------------------------------------------- */

boolean OcilibStatementBindArrayOfCollections
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
 * OcilibStatementBindStatement
 * --------------------------------------------------------------------------------------------- */

boolean OcilibStatementBindStatement
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
 * OcilibStatementBindLong
 * --------------------------------------------------------------------------------------------- */

boolean OcilibStatementBindLong
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
        OcilibExternalSubTypeToSQLType(OCI_CDT_LONG, data->type),
        data->type, NULL, 0
    )
}

/* --------------------------------------------------------------------------------------------- *
* OcilibStatementRegisterNumber
* --------------------------------------------------------------------------------------------- */

boolean OcilibStatementRegisterNumber
(
    OCI_Statement *stmt,
    const otext   *name
)
{
    REGISTER_CALL(sizeof(OCINumber), OCI_CDT_NUMERIC, SQLT_VNU, OCI_NUM_NUMBER, NULL, 0)
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementRegisterShort
 * --------------------------------------------------------------------------------------------- */

boolean OcilibStatementRegisterShort
(
    OCI_Statement *stmt,
    const otext   *name
)
{
    REGISTER_CALL(sizeof(OCINumber), OCI_CDT_NUMERIC, SQLT_VNU, OCI_NUM_SHORT, NULL, 0)
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementRegisterUnsignedShort
 * --------------------------------------------------------------------------------------------- */

boolean OcilibStatementRegisterUnsignedShort
(
    OCI_Statement *stmt,
    const otext   *name
)
{
    REGISTER_CALL(sizeof(OCINumber), OCI_CDT_NUMERIC, SQLT_VNU, OCI_NUM_USHORT, NULL, 0)
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementRegisterInt
 * --------------------------------------------------------------------------------------------- */

boolean OcilibStatementRegisterInt
(
    OCI_Statement *stmt,
    const otext   *name
)
{
    REGISTER_CALL(sizeof(OCINumber), OCI_CDT_NUMERIC, SQLT_VNU, OCI_NUM_INT, NULL, 0)
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementRegisterUnsignedInt
 * --------------------------------------------------------------------------------------------- */

boolean OcilibStatementRegisterUnsignedInt
(
    OCI_Statement *stmt,
    const otext   *name
)
{
    REGISTER_CALL(sizeof(OCINumber), OCI_CDT_NUMERIC, SQLT_VNU, OCI_NUM_UINT, NULL, 0)
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementRegisterBigInt
 * --------------------------------------------------------------------------------------------- */

boolean OcilibStatementRegisterBigInt
(
    OCI_Statement *stmt,
    const otext   *name
)
{
    REGISTER_CALL(sizeof(OCINumber), OCI_CDT_NUMERIC, SQLT_VNU, OCI_NUM_BIGINT, NULL, 0)
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementRegisterUnsignedBigInt
 * --------------------------------------------------------------------------------------------- */

boolean OcilibStatementRegisterUnsignedBigInt
(
    OCI_Statement *stmt,
    const otext   *name
)
{
    REGISTER_CALL(sizeof(OCINumber), OCI_CDT_NUMERIC, SQLT_VNU, OCI_NUM_BIGUINT, NULL, 0)
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementRegisterString
 * --------------------------------------------------------------------------------------------- */

boolean OcilibStatementRegisterString
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
 * OcilibStatementRegisterRaw
 * --------------------------------------------------------------------------------------------- */

boolean OcilibStatementRegisterRaw
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
 * OcilibStatementRegisterDouble
 * --------------------------------------------------------------------------------------------- */

boolean OcilibStatementRegisterDouble
(
    OCI_Statement *stmt,
    const otext   *name
)
{
    REGISTER_CALL(sizeof(OCINumber), OCI_CDT_NUMERIC, SQLT_VNU, OCI_NUM_DOUBLE, NULL, 0)
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementRegisterFloat
 * --------------------------------------------------------------------------------------------- */

boolean OcilibStatementRegisterFloat
(
    OCI_Statement *stmt,
    const otext   *name
)
{
    REGISTER_CALL(sizeof(OCINumber), OCI_CDT_NUMERIC, SQLT_VNU, OCI_NUM_FLOAT, NULL, 0)
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementRegisterDate
 * --------------------------------------------------------------------------------------------- */

boolean OcilibStatementRegisterDate
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

    if (OcilibConnectionGetVersion(stmt->con) < OCI_10_2)
    {
        code = SQLT_DAT;
        size = 7;
    }

    REGISTER_DATA(size, OCI_CDT_DATETIME, code, 0, NULL, 0)

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementRegisterTimestamp
 * --------------------------------------------------------------------------------------------- */

boolean OcilibStatementRegisterTimestamp
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
                  OcilibExternalSubTypeToSQLType(OCI_CDT_TIMESTAMP, type),
                  type, NULL, 0)

#endif

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementRegisterInterval
 * --------------------------------------------------------------------------------------------- */

boolean OcilibStatementRegisterInterval
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
                  OcilibExternalSubTypeToSQLType(OCI_CDT_INTERVAL, type),
                  type, NULL, 0)

#endif

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementRegisterObject
 * --------------------------------------------------------------------------------------------- */

boolean OcilibStatementRegisterObject
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
 * OcilibStatementRegisterLob
 * --------------------------------------------------------------------------------------------- */

boolean OcilibStatementRegisterLob
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
                  OcilibExternalSubTypeToSQLType(OCI_CDT_LOB, type),
                  type, NULL, 0)

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementRegisterFile
 * --------------------------------------------------------------------------------------------- */

boolean OcilibStatementRegisterFile
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
                  OcilibExternalSubTypeToSQLType(OCI_CDT_FILE, type),
                  type, NULL, 0)

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementRegisterReference
 * --------------------------------------------------------------------------------------------- */

boolean OcilibStatementRegisterReference
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
 * OcilibStatementGetStatementType
 * --------------------------------------------------------------------------------------------- */

unsigned int OcilibStatementGetStatementType
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
 * OcilibStatementSetFetchMode
 * --------------------------------------------------------------------------------------------- */

boolean OcilibStatementSetFetchMode
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
            OcilibStatementSetPrefetchSize(stmt, 0);
        }
        else if (old_exec_mode == OCI_SFM_SCROLLABLE && stmt->exec_mode == OCI_SFM_DEFAULT)
        {
            /* Re-enable prefetch previously disabled */
            OcilibStatementSetPrefetchSize(stmt, OCI_PREFETCH_SIZE);
        }
    }

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementGetFetchMode
 * --------------------------------------------------------------------------------------------- */

unsigned int OcilibStatementGetFetchMode
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
 * OcilibStatementSetBindMode
 * --------------------------------------------------------------------------------------------- */

boolean OcilibStatementSetBindMode
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
 * OcilibStatementGetBindMode
 * --------------------------------------------------------------------------------------------- */

unsigned int OcilibStatementGetBindMode
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
 * OcilibStatementSetBindAllocation
 * --------------------------------------------------------------------------------------------- */

boolean OcilibStatementSetBindAllocation
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
 * OcilibStatementGetBindAllocation
 * --------------------------------------------------------------------------------------------- */

unsigned int OcilibStatementGetBindAllocation
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
 * OcilibStatementSetFetchSize
 * --------------------------------------------------------------------------------------------- */

boolean OcilibStatementSetFetchSize
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
 * OcilibStatementGetFetchSize
 * --------------------------------------------------------------------------------------------- */

unsigned int OcilibStatementGetFetchSize
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
 * OcilibStatementPrefetchSize
 * --------------------------------------------------------------------------------------------- */

boolean OcilibStatementSetPrefetchSize
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
 * OcilibStatementGetPrefetchSize
 * --------------------------------------------------------------------------------------------- */

unsigned int OcilibStatementGetPrefetchSize
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
 * OcilibStatementSetPrefetchMemory
 * --------------------------------------------------------------------------------------------- */

boolean OcilibStatementSetPrefetchMemory
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
 * OcilibStatementGetPrefetchMemory
 * --------------------------------------------------------------------------------------------- */

unsigned int OcilibStatementGetPrefetchMemory
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
 * OcilibStatementSetPieceSize
 * --------------------------------------------------------------------------------------------- */

boolean OcilibStatementSetPieceSize
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

    stmt->piece_size = size;

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementGetPieceSize
 * --------------------------------------------------------------------------------------------- */

unsigned int OcilibStatementGetPieceSize
(
    OCI_Statement *stmt
)
{
    GET_PROP
    (
        unsigned int, 0,
        OCI_IPC_STATEMENT, stmt,
        piece_size
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementSetLongMode
 * --------------------------------------------------------------------------------------------- */

boolean OcilibStatementSetLongMode
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
 * OcilibStatementGetLongMode
 * --------------------------------------------------------------------------------------------- */

unsigned int OcilibStatementGetLongMode
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
 * OcilibStatementGetConnection
 * --------------------------------------------------------------------------------------------- */

OCI_Connection * OcilibStatementGetConnection
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
 * OcilibStatementGetSql
 * --------------------------------------------------------------------------------------------- */

const otext * OcilibStatementGetSql
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
 * OcilibStatementGetSqlIdentifier
 * --------------------------------------------------------------------------------------------- */

const otext* OcilibStatementGetSqlIdentifier
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
 * OcilibStatementGetSqlErrorPos
 * --------------------------------------------------------------------------------------------- */

unsigned int OcilibStatementGetSqlErrorPos
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
 * OcilibStatementGetAffectedRows
 * --------------------------------------------------------------------------------------------- */

unsigned int OcilibStatementGetAffectedRows
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
 * OcilibStatementGetBindCount
 * --------------------------------------------------------------------------------------------- */

unsigned int OcilibStatementGetBindCount
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
 * OcilibStatementGetBind
 * --------------------------------------------------------------------------------------------- */

OCI_Bind * OcilibStatementGetBind
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
 * OcilibStatementGetBind2
 * --------------------------------------------------------------------------------------------- */

OCI_Bind * OcilibStatementGetBind2
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

    index = OcilibBindGetIndex(stmt, name);
    if (index <= 0)
    {
        THROW(OcilibExceptionItemNotFound, name, OCI_IPC_BIND)
    }

    SET_RETVAL(stmt->ubinds[index - 1])

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
* OcilibStatementGetBindIndex
* --------------------------------------------------------------------------------------------- */

unsigned int OcilibStatementGetBindIndex
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

    index = OcilibBindGetIndex(stmt, name);
    CHECK(index >= 0)

    SET_RETVAL(index)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementGetSqlCommand
 * --------------------------------------------------------------------------------------------- */

unsigned int OcilibStatementGetSqlCommand
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
 * OcilibStatementGetSqlVerb
 * --------------------------------------------------------------------------------------------- */

const otext * OcilibStatementGetSqlVerb
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

    code = OcilibStatementGetSqlCommand(stmt);

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
 * OcilibStatementGetBatchError
 * --------------------------------------------------------------------------------------------- */

OCI_Error * OcilibStatementGetBatchError
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
 * OcilibStatementGetBatchErrorCount
 * --------------------------------------------------------------------------------------------- */

unsigned int OcilibStatementGetBatchErrorCount
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
