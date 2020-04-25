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
static unsigned int TimestampTypeValues[] = { OCI_TIMESTAMP, OCI_TIMESTAMP_TZ, OCI_TIMESTAMP_LTZ };
static unsigned int IntervalTypeValues[]  = { OCI_INTERVAL_YM, OCI_INTERVAL_DS };
#endif

static unsigned int LobTypeValues[]  = { OCI_CLOB, OCI_NCLOB, OCI_BLOB };
static unsigned int FileTypeValues[] = { OCI_CFILE, OCI_BFILE };

static unsigned int FetchModeValues[]      = { OCI_SFM_DEFAULT, OCI_SFM_SCROLLABLE };
static unsigned int BindModeValues[]       = { OCI_BIND_BY_POS, OCI_BIND_BY_NAME };
static unsigned int BindAllocationValues[] = { OCI_BAM_EXTERNAL, OCI_BAM_INTERNAL };
static unsigned int LongModeValues[]       = { OCI_LONG_EXPLICIT, OCI_LONG_IMPLICIT };

#define SET_ARG_NUM(type, func)                                     \
    type src = func(rs, i), *dst = ( type *) va_arg(args, type *);  \
    if (dst)                                                        \
    {                                                               \
        *dst = src;                                                 \
    }                                                               \

#define SET_ARG_HANDLE(type, func, assign)                          \
    type *src = func(rs, i), *dst = (type *) va_arg(args, type *);  \
    if (src && dst)                                                 \
    {                                                               \
        res = assign(dst, src);                                     \
    }                                                               \

#define BIND_DATA(...)                                              \
                                                                    \
    STATUS = BindCreate(ctx, stmt, data, name,                      \
                        OCI_BIND_INPUT, __VA_ARGS__) != NULL;       \

#define REGISTER_DATA(...)                                          \
                                                                    \
    STATUS = BindCreate(ctx, stmt, NULL, name,                      \
                        OCI_BIND_OUTPUT, __VA_ARGS__) != NULL;      \


#define OCI_BIND_CALL(type, check, ...)                             \
                                                                    \
    CALL_ENTER(boolean, FALSE)                                      \
    CALL_CHECK_BIND(stmt, name, data, type, check)                  \
    CALL_CONTEXT_FROM_STMT(stmt)                                    \
    BIND_DATA(__VA_ARGS__)                                          \
    RETVAL = STATUS;                                                \
    CALL_EXIT()                                                     \

#define BIND_CALL_NULL_ALLOWED(type, ...)                           \
                                                                    \
    OCI_BIND_CALL(type, FALSE, __VA_ARGS__)

#define BIND_CALL_NULL_FORBIDDEN(type, ...)                         \
                                                                    \
    OCI_BIND_CALL(type, TRUE, __VA_ARGS__)

#define REGISTER_CALL(...)                                          \
                                                                    \
    CALL_ENTER(boolean, FALSE)                                      \
    CALL_CHECK_REGISTER(stmt, name)                                 \
    CALL_CONTEXT_FROM_STMT(stmt)                                    \
    REGISTER_DATA(__VA_ARGS__)                                      \
    RETVAL = STATUS;                                                \
    CALL_EXIT()                                                     \

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
    if (stmt->batch)
    {
        /* free internal array of OCI_Errors */

        FREE(stmt->batch->errs)

        /* free batch structure */

        FREE(stmt->batch)
    }

    return TRUE;
}

/* --------------------------------------------------------------------------------------------- *
 * StatementFreeAllBinds
 * --------------------------------------------------------------------------------------------- */

boolean StatementFreeAllBinds
(
    OCI_Statement *stmt
)
{
    int i;

    CHECK(NULL == stmt, FALSE);

    /* free user binds */

    if (stmt->ubinds)
    {
        for(i = 0; i < stmt->nb_ubinds; i++)
        {
            BindFree(stmt->ubinds[i]);
        }

        FREE(stmt->ubinds)
    }

    /* free register binds */

    if (stmt->rbinds)
    {
        for(i = 0; i < stmt->nb_rbinds; i++)
        {
            BindFree(stmt->rbinds[i]);
        }

        FREE(stmt->rbinds)
    }

    stmt->nb_ubinds = 0;
    stmt->nb_rbinds = 0;

    return TRUE;
}

/* --------------------------------------------------------------------------------------------- *
 * StatementReset
 * --------------------------------------------------------------------------------------------- */

boolean StatementReset
(
    OCI_Statement *stmt
)
{
    ub4 mode = OCI_DEFAULT;

    DECLARE_CTX(TRUE)

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

    STATUS = StatementBatchErrorClear(stmt);

    /* free resultsets */

    STATUS = STATUS && StatementReleaseResultsets(stmt);

    /* free in/out binds */

    STATUS = STATUS && StatementFreeAllBinds(stmt);

    /* free bind map */

    if (stmt->map)
    {
        STATUS = STATUS && HashFree(stmt->map);
    }

    /* free handle if needed */

    if (STATUS && stmt->stmt)
    {
        if (OCI_OBJECT_ALLOCATED == stmt->hstate)
        {

#if OCI_VERSION_COMPILE >= OCI_9_2

            if (Env.version_runtime >= OCI_9_2)
            {
                EXEC(OCIStmtRelease(stmt->stmt, stmt->con->err, NULL, 0, mode))
            }
            else

#endif

            {
                STATUS = MemoryFreeHandle((dvoid *)stmt->stmt, OCI_HTYPE_STMT);
            }

            stmt->stmt = NULL;
        }
        else if (OCI_OBJECT_ALLOCATED_BIND_STMT == stmt->hstate)
        {
            STATUS = MemoryFreeHandle((dvoid *) stmt->stmt, OCI_HTYPE_STMT);

            stmt->stmt = NULL;
        }
    }

    if (STATUS)
    {
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
    }

    return STATUS;
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
    DECLARE_CTX(TRUE)

    if (!bnd || !dst)
    {
        return FALSE;
    }

    CALL_CONTEXT_FROM_STMT(bnd->stmt)

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
                        EXEC(OCINumberAssign(bnd->stmt->con->err, src_num->handle, dst_num))
                    }
                }
            }
            else if (OCI_NUM_BIGINT & bnd->subtype)
            {
                big_int   *src_bint = OCI_BIND_GET_SCALAR(src, big_int, index);
                OCINumber *dst_num  = OCI_BIND_GET_BUFFER(dst, OCINumber, index);

                STATUS = NumberTranslateValue(bnd->stmt->con, src_bint, bnd->subtype, dst_num, OCI_NUM_NUMBER);
            }
        }

        /* OCI_Date binds */
        else if (OCI_CDT_DATETIME == bnd->type)
        {
            OCI_Date *src_date = OCI_BIND_GET_HANDLE(src, OCI_Date, index);
            OCIDate  *dst_date = OCI_BIND_GET_BUFFER(dst, OCIDate, index);

            if (src_date)
            {
                EXEC(OCIDateAssign(bnd->stmt->con->err, src_date->handle, dst_date))
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

    return STATUS;
}

/* --------------------------------------------------------------------------------------------- *
* StatementBindUpdate
* --------------------------------------------------------------------------------------------- */

boolean StatementBindUpdate
(
    OCI_Bind    *bnd,
    ub1         *src,
    ub1         *dst,
    unsigned int index)
{
    DECLARE_CTX(TRUE)

    if (!bnd || !src || !dst)
    {
        return FALSE;
    }

    CALL_CONTEXT_FROM_STMT(bnd->stmt)

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
                    EXEC(OCINumberAssign(bnd->stmt->con->err, src_num, dst_num->handle))
                }
            }
        }
        else if (OCI_NUM_BIGINT & bnd->subtype)
        {
            OCINumber *src_number = OCI_BIND_GET_BUFFER(src, OCINumber, index);
            big_int   *dst_bint   = OCI_BIND_GET_SCALAR(dst, big_int, index);

            if (dst_bint)
            {
                STATUS = NumberTranslateValue(bnd->stmt->con, src_number, OCI_NUM_NUMBER, dst_bint, bnd->subtype);
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
            EXEC(OCIDateAssign(bnd->stmt->con->err, src_date, dst_date->handle))
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

    return STATUS;
}

/* --------------------------------------------------------------------------------------------- *
 * StatementBindCheckAll
 * --------------------------------------------------------------------------------------------- */

boolean StatementBindCheckAll
(
    OCI_Statement *stmt
)
{
    ub4 j;

    DECLARE_CTX(TRUE)

    CHECK(NULL == stmt, FALSE)
    CHECK(NULL == stmt->ubinds, TRUE);

    for (ub4 i = 0; i < stmt->nb_ubinds && STATUS; i++)
    {
        OCI_Bind *bnd = stmt->ubinds[i];

        if (OCI_CDT_CURSOR == bnd->type)
        {
            OCI_Statement *bnd_stmt = (OCI_Statement *) bnd->buffer.data;

            StatementReset(bnd_stmt);

            bnd_stmt->hstate = OCI_OBJECT_ALLOCATED_BIND_STMT;

            /* allocate statement handle */

            STATUS = MemoryAllocHandle((dvoid *)bnd_stmt->con->env, (dvoid **)(void *)&bnd_stmt->stmt, OCI_HTYPE_STMT);

            STATUS = STATUS && StatementSetPrefetchSize(stmt, stmt->prefetch_size);
            STATUS = STATUS && StatementSetFetchSize(stmt, stmt->fetch_size);
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

                    for (j = 0; j < count && STATUS; j++)
                    {
                        STATUS = StatementBindCheck(bnd, (ub1*)bnd->input, (ub1*)bnd->buffer.data, j);
                    }
                }
                else
                {
                    STATUS = StatementBindCheck(bnd, (ub1*)bnd->input, (ub1*)bnd->buffer.data, 0);
                }
            }
        }
    }

    return STATUS;
}

/* --------------------------------------------------------------------------------------------- *
 * StatementBindUpdateAll
 * --------------------------------------------------------------------------------------------- */

boolean StatementBindUpdateAll
(
    OCI_Statement *stmt
)
{
    DECLARE_CTX(TRUE)

    CHECK(NULL == stmt, FALSE)
    CHECK(NULL == stmt->ubinds, FALSE);

    for (ub4 i = 0; i < stmt->nb_ubinds && STATUS; i++)
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

                    for (ub4 j = 0; j < count && STATUS; j++)
                    {
                        STATUS = StatementBindUpdate(bnd, (ub1*)bnd->buffer.data, (ub1*)bnd->input, j);
                    }
                }
                else
                {
                    STATUS = StatementBindUpdate(bnd, (ub1*)bnd->buffer.data, (ub1*)bnd->input, 0);
                }
            }
        }
    }

    return STATUS;
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
    OCI_Resultset *rs  = NULL;
    boolean        res = FALSE;

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
                    ExceptionMappingArgument(stmt->con, stmt, type);

                    res = FALSE;

                    break;
                }
            }
        }
    }

    return res;
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
    DECLARE_CTX(TRUE)
    CALL_CONTEXT_FROM_CON(con)

    ALLOC_DATA(OCI_IPC_STATEMENT, stmt, 1);

    if (STATUS)
    {
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

        STATUS = StatementReset(stmt);

        if (is_desc)
        {
            stmt->hstate = OCI_OBJECT_FETCHED_CLEAN;
            stmt->status = OCI_STMT_PREPARED  | OCI_STMT_PARSED |
                           OCI_STMT_DESCRIBED | OCI_STMT_EXECUTED;
            stmt->type = OCI_CST_SELECT;

            if (sql)
            {
                stmt->sql = ostrdup(sql);
            }
            else
            {
                dbtext *dbstr  = NULL;
                int     dbsize = 0;

                ATTRIB_GET(OCI_HTYPE_STMT, OCI_ATTR_STATEMENT, stmt->stmt, &dbstr, &dbsize)

                if (STATUS && dbstr)
                {
                    stmt->sql = StringDuplicateFromDBString(dbstr, dbcharcount(dbsize));
                    STATUS    = (NULL != stmt->sql);
                }
            }

            /* Setting fetch attributes here as the statement is already prepared */

            STATUS = STATUS && StatementSetPrefetchSize(stmt, stmt->prefetch_size);
            STATUS = STATUS && StatementSetFetchSize(stmt, stmt->fetch_size);
        }
        else
        {
            /* allocate handle for non fetched cursor */

            stmt->hstate = OCI_OBJECT_ALLOCATED;
        }
    }

    /* check for failure */

    if (!STATUS && stmt)
    {
        StatementFree(stmt);
        stmt = NULL;
    }

    return stmt;
}

/* --------------------------------------------------------------------------------------------- *
 * StatementDispose
 * --------------------------------------------------------------------------------------------- */

boolean StatementDispose
(
    OCI_Statement *stmt
)
{
    OCI_Error *err = NULL;

    CHECK(NULL == stmt, FALSE);

    /* clear statement reference from current error object */

    err = ErrorGet(FALSE, FALSE);

    if (err && err->stmt == stmt)
    {
        err->stmt = NULL;
    }

    /* reset data */

    return StatementReset(stmt);
}

/* --------------------------------------------------------------------------------------------- *
 * StatementCheckImplicitResultsets
 * --------------------------------------------------------------------------------------------- */

boolean StatementCheckImplicitResultsets
(
    OCI_Statement *stmt
)
{
    DECLARE_CTX(TRUE)
    CALL_CONTEXT_FROM_STMT(stmt)

#if OCI_VERSION_COMPILE >= OCI_12_1

    if (Env.version_runtime >= OCI_12_1)
    {
        ATTRIB_GET(OCI_HTYPE_STMT, OCI_ATTR_IMPLICIT_RESULT_COUNT, stmt->stmt, &stmt->nb_stmt, NULL)

        if (STATUS && stmt->nb_stmt > 0)
        {
            OCIStmt *result  = NULL;
            ub4      rs_type = OCI_UNKNOWN;
            ub4      i       = 0;

            /* allocate resultset handles array */

            ALLOC_DATA(OCI_IPC_STATEMENT_ARRAY, stmt->stmts, stmt->nb_stmt)
            ALLOC_DATA(OCI_IPC_RESULTSET_ARRAY, stmt->rsts, stmt->nb_stmt)

            while (STATUS && OCI_SUCCESS == OCIStmtGetNextResult(stmt->stmt, stmt->con->err, (dvoid  **)&result, &rs_type, OCI_DEFAULT))
            {
                if (OCI_RESULT_TYPE_SELECT == rs_type)
                {
                    stmt->stmts[i] = StatementInitialize(stmt->con, NULL, result, TRUE, NULL);

                    STATUS = (NULL != stmt->stmts[i]);

                    if (STATUS)
                    {
                        stmt->rsts[i] = ResultsetCreate(stmt->stmts[i], stmt->stmts[i]->fetch_size);

                        STATUS = (NULL != stmt->rsts[i]);

                        if (STATUS)
                        {
                            i++;
                            stmt->nb_rs++;
                        }
                    }
                }
            }
        }
    }

#endif

    return STATUS;
}

/* --------------------------------------------------------------------------------------------- *
 * StatementBatchErrorsInit
 * --------------------------------------------------------------------------------------------- */

boolean StatementBatchErrorInit
(
    OCI_Statement *stmt
)
{
    ub4 err_count = 0;

    DECLARE_CTX(TRUE)
    CALL_CONTEXT_FROM_STMT(stmt)

    StatementBatchErrorClear(stmt);

    /* all OCI call here are not checked for errors as we already dealing
       with an array DML error */

    ATTRIB_GET(OCI_HTYPE_STMT, OCI_ATTR_NUM_DML_ERRORS, stmt->stmt, &err_count, NULL)

    if (err_count > 0)
    {
        OCIError *hndl = NULL;

        /* allocate batch error structure */

        ALLOC_DATA(OCI_IPC_BATCH_ERRORS, stmt->batch, 1)

        /* allocate array of error objects */

        ALLOC_DATA(OCI_IPC_ERROR, stmt->batch->errs, err_count)

        if (STATUS)
        {
            /* allocate OCI error handle */

            STATUS = MemoryAllocHandle((dvoid  *)stmt->con->env, (dvoid **)(void *)&hndl, OCI_HTYPE_ERROR);
        }

        /* loop on the OCI errors to fill OCILIB error objects */

        if (STATUS)
        {
            stmt->batch->count = err_count;

            for (ub4 i = 0; i < stmt->batch->count; i++)
            {
                int     dbsize = -1;
                dbtext *dbstr  = NULL;

                OCI_Error *err = &stmt->batch->errs[i];

                OCIParamGet((dvoid *) stmt->con->err, OCI_HTYPE_ERROR,
                            stmt->con->err, (dvoid **) (void *) &hndl, i);

                /* get row offset */

                OCIAttrGet((dvoid *) hndl, (ub4) OCI_HTYPE_ERROR,
                           (void *) &err->row, (ub4 *) NULL,
                           (ub4) OCI_ATTR_DML_ROW_OFFSET, stmt->con->err);

                /* fill error attributes */

                err->type = OCI_ERR_ORACLE;
                err->con  = stmt->con;
                err->stmt = stmt;

                /* OCILIB indexes start at 1 */

                err->row++;

                /* get error string */

                dbsize = (int) osizeof(err->str) - 1;

                dbstr = StringGetDBString(err->str, &dbsize);

                OCIErrorGet((dvoid *) hndl,
                            (ub4) 1,
                            (OraText *) NULL, &err->sqlcode,
                            (OraText *) dbstr,
                            (ub4) dbsize,
                            (ub4) OCI_HTYPE_ERROR);

                StringCopyDBStringToNativeString(dbstr, err->str, dbcharcount(dbsize));
                StringReleaseDBString(dbstr);
            }
        }

        /* release error handle */

        if (hndl)
        {
            MemoryFreeHandle(hndl, OCI_HTYPE_ERROR);
        }
    }

    return STATUS;
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
    dbtext *dbstr  = NULL;
    int     dbsize = -1;

    DECLARE_CTX(TRUE)
    CALL_CONTEXT_FROM_STMT(stmt)

    /* reset statement */

    STATUS = StatementReset(stmt);

    if (STATUS)
    {
        /* store SQL */

        stmt->sql = ostrdup(sql);

        dbstr = StringGetDBString(stmt->sql, &dbsize);

        if (Env.version_runtime < OCI_9_2)
        {
            /* allocate handle */

            STATUS = MemoryAllocHandle((dvoid *)stmt->con->env, (dvoid **)(void *)&stmt->stmt, OCI_HTYPE_STMT);
        }
    }

    if (STATUS)
    {
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

            EXEC
            (
                OCIStmtPrepare2
                (
                    stmt->con->cxt, &stmt->stmt, stmt->con->err, (OraText *) dbstr,
                    (ub4) dbsize, NULL, 0, (ub4) OCI_NTV_SYNTAX, (ub4) mode
                )
            )
        }
        else

#endif

        {
            EXEC
            (
                OCIStmtPrepare
                (
                    stmt->stmt,stmt->con->err, (OraText *) dbstr, (ub4) dbsize,
                    (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT
                )
            )
        }

        /* get statement type */

        ATTRIB_GET(OCI_HTYPE_STMT, OCI_ATTR_STMT_TYPE, stmt->stmt, &stmt->type, NULL)
    }

    StringReleaseDBString(dbstr);

    /* update statement status */

    if (STATUS)
    {
        stmt->status = OCI_STMT_PREPARED;

        STATUS = STATUS && StatementSetPrefetchSize(stmt, stmt->prefetch_size);
        STATUS = STATUS && StatementSetFetchSize(stmt, stmt->fetch_size);
    }

    return STATUS;
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
    sword status = OCI_SUCCESS;
    ub4   iters  = 0;

    DECLARE_CTX(TRUE)
    CALL_CONTEXT_FROM_STMT(stmt)

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

    StatementBatchErrorClear(stmt);

    /* check bind objects for updating their null indicator status */

    STATUS = StatementBindCheckAll(stmt);

    /* check current resultsets */

    if (STATUS && stmt->rsts)
    {
        /* resultsets are freed before any prepare operations.
           So, if we got ones here, it means the same SQL order
           is re-executed */

        if (OCI_CST_SELECT == stmt->type)
        {
            /* just reinitialize the current resultset */

            STATUS = ResultsetInitialize(stmt->rsts[0]);
        }
        else
        {
            /* Must free previous resultsets for 'returning into'
               SQL orders that can produce multiple resultsets */

            STATUS = StatementReleaseResultsets(stmt);
        }
    }

    /* Oracle execute call */

    if (STATUS)
    {

        status = OCIStmtExecute(stmt->con->cxt, stmt->stmt, stmt->con->err, iters,
                                (ub4)0, (OCISnapshot *)NULL, (OCISnapshot *)NULL, mode);
    }

    /* check result */

    STATUS = ((OCI_SUCCESS   == status) || (OCI_SUCCESS_WITH_INFO == status) ||
              (OCI_NEED_DATA == status) || (OCI_NO_DATA == status));

    if (OCI_SUCCESS_WITH_INFO == status)
    {
        ExceptionOCI(stmt->con->err, stmt->con, stmt, TRUE);
    }

    /* on batch mode, check if any error occurred */

    if (mode & OCI_BATCH_ERRORS)
    {
        /* build batch error list if the statement is array DML */

        StatementBatchErrorInit(stmt);

        if (stmt->batch)
        {
            STATUS = (stmt->batch->count == 0);
        }
    }

    /* update status on success */

    if (STATUS)
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

            StatementBindUpdateAll(stmt);

            /* commit if necessary */

            if (stmt->con->autocom)
            {
                ConnectionCommit(stmt->con);
            }

            /* check if any implicit results are available */

            STATUS = StatementCheckImplicitResultsets(stmt);

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

        ExceptionOCI(stmt->con->err, stmt->con, stmt, FALSE);
    }

    return STATUS;
}

/* --------------------------------------------------------------------------------------------- *
 * StatementCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Statement * StatementCreate
(
    OCI_Connection *con
)
{
    CALL_ENTER(OCI_Statement *, NULL)
    CALL_CHECK_PTR(OCI_IPC_CONNECTION, con)
    CALL_CONTEXT_FROM_CON(con)

    /* create statement object */

    RETVAL = ListAppend(con->stmts, sizeof(*RETVAL));
    STATUS = (NULL != RETVAL);

    if (STATUS)
    {
        RETVAL = StatementInitialize(con, (OCI_Statement *) RETVAL, NULL, FALSE, NULL);
        STATUS = (NULL != RETVAL);
    }

    if (!STATUS && RETVAL)
    {
        StatementFree(RETVAL);
        RETVAL = NULL;
    }

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * StatementFree
 * --------------------------------------------------------------------------------------------- */

boolean StatementFree
(
    OCI_Statement *stmt
)
{
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_STATEMENT, stmt)
    CALL_CHECK_OBJECT_FETCHED(stmt)
    CALL_CONTEXT_FROM_STMT(stmt)

    StatementDispose(stmt);
    ListRemove(stmt->con->stmts, stmt);

    FREE(stmt)

    RETVAL = TRUE;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * StatementGetResultset
 * --------------------------------------------------------------------------------------------- */

OCI_Resultset * StatementGetResultset
(
    OCI_Statement *stmt
)
{
    CALL_ENTER(OCI_Resultset*, NULL)
    CALL_CHECK_PTR(OCI_IPC_STATEMENT, stmt)
    CALL_CHECK_STMT_STATUS(stmt, OCI_STMT_DESCRIBED)
    CALL_CONTEXT_FROM_STMT(stmt)

    /* if the sql statement does not return a result, we just return NULL and not
       throwing any exception
       statements that can return a resultset are "SELECT..." and "... RETURNING INTO..."
    */

    if ((OCI_CST_SELECT == stmt->type) || (stmt->nb_rbinds > 0) || (stmt->nb_stmt > 0))
    {
        /* if the resultset exists, let's use it */

        if (stmt->rsts && stmt->rsts[0])
        {
            RETVAL = stmt->rsts[0];
        }

        /* allocate resultset for select statements only */

        if (!RETVAL && (OCI_CST_SELECT == stmt->type))
        {
            /* allocate memory for one resultset handle */

            ALLOC_DATA(OCI_IPC_RESULTSET_ARRAY, stmt->rsts, 1)

            if (STATUS)
            {
                stmt->nb_rs  = 1;
                stmt->cur_rs = 0;

                /* create resultset object */

                RETVAL = stmt->rsts[0] = ResultsetCreate(stmt, stmt->fetch_size);
            }

        }

        STATUS = (NULL != RETVAL);
    }

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * StatementGetNextResultset
 * --------------------------------------------------------------------------------------------- */

OCI_Resultset * StatementGetNextResultset
(
    OCI_Statement *stmt
)
{
    CALL_ENTER(OCI_Resultset*, NULL)
    CALL_CHECK_PTR(OCI_IPC_STATEMENT, stmt)
    CALL_CHECK_STMT_STATUS(stmt, OCI_STMT_DESCRIBED)
    CALL_CONTEXT_FROM_STMT(stmt)

    if (stmt->cur_rs < (stmt->nb_rs-1))
    {
        RETVAL = stmt->rsts[++stmt->cur_rs];
    }

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * StatementReleaseResultsets
 * --------------------------------------------------------------------------------------------- */

boolean StatementReleaseResultsets
(
    OCI_Statement *stmt
)
{
    ub4 i;

    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_STATEMENT, stmt)
    CALL_CONTEXT_FROM_STMT(stmt)

    /* Release statements for implicit resultsets */
    if (stmt->stmts)
    {
        for (i = 0; i  < stmt->nb_stmt; i++)
        {
            if (stmt->rsts[i])
            {
                StatementDispose(stmt->stmts[i]);
            }
        }

        FREE(stmt->rsts)
    }

    /* release resultsets */
    if (stmt->rsts)
    {
        for (i = 0; i  < stmt->nb_rs; i++)
        {
            if (stmt->rsts[i])
            {
                ResultsetFree(stmt->rsts[i]);
            }
        }

        FREE(stmt->rsts)
    }

    RETVAL = TRUE;

    CALL_EXIT()
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
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_STATEMENT, stmt)
    CALL_CHECK_PTR(OCI_IPC_STRING, sql)
    CALL_CONTEXT_FROM_STMT(stmt)

    RETVAL = STATUS = StatementPrepareInternal(stmt, sql);

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * StatementExecute
 * --------------------------------------------------------------------------------------------- */

boolean StatementExecute
(
    OCI_Statement *stmt
)
{
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_STATEMENT, stmt)
    CALL_CHECK_STMT_STATUS(stmt, OCI_STMT_PREPARED)
    CALL_CONTEXT_FROM_STMT(stmt)

    RETVAL = STATUS = StatementExecuteInternal(stmt, OCI_DEFAULT);

    CALL_EXIT()
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
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_STATEMENT, stmt)
    CALL_CHECK_PTR(OCI_IPC_STRING, sql)
    CALL_CONTEXT_FROM_STMT(stmt)

    RETVAL = STATUS = StatementPrepareInternal(stmt, sql) && StatementExecuteInternal(stmt, OCI_DEFAULT);

    CALL_EXIT()
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
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_STATEMENT, stmt)
    CALL_CHECK_PTR(OCI_IPC_STRING, sql)
    CALL_CONTEXT_FROM_STMT(stmt)

    RETVAL = STATUS = StatementPrepareInternal(stmt, sql) && StatementExecuteInternal(stmt, OCI_PARSE_ONLY);

    CALL_EXIT()
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
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_STATEMENT, stmt)
    CALL_CHECK_PTR(OCI_IPC_STRING, sql)
    CALL_CONTEXT_FROM_STMT(stmt)

    STATUS = StatementPrepareInternal(stmt, sql);

    if (STATUS && OCI_CST_SELECT == stmt->type)
    {
        STATUS = StatementExecuteInternal(stmt, OCI_DESCRIBE_ONLY);
    }

    RETVAL = STATUS;

    CALL_EXIT()
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
    va_list first_pass_args;
    va_list second_pass_args;

    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_STATEMENT, stmt)
    CALL_CHECK_PTR(OCI_IPC_STRING, sql)
    CALL_CONTEXT_FROM_STMT(stmt)

    va_copy(first_pass_args, args);
    va_copy(second_pass_args, args);

    /* first, get buffer size */

    const int size = FormatParseSql(stmt, NULL, sql, &first_pass_args);

    if (size > 0)
    {
        otext *sql_fmt = NULL;

        /* allocate buffer */

        ALLOC_DATA(OCI_IPC_STRING, sql_fmt, size + 1)

        if (STATUS)
        {
            /* format buffer */

            if (FormatParseSql(stmt, sql_fmt, sql, &second_pass_args) > 0)
            {
                /* parse buffer */

                STATUS = StatementPrepareInternal(stmt, sql_fmt);
            }

            FREE(sql_fmt)
        }
    }

    RETVAL = STATUS;

    va_end(first_pass_args);
    va_end(second_pass_args);

    CALL_EXIT()
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
    va_list first_pass_args;
    va_list second_pass_args;

    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_STATEMENT, stmt)
    CALL_CHECK_PTR(OCI_IPC_STRING, sql)
    CALL_CONTEXT_FROM_STMT(stmt)

    va_copy(first_pass_args, args);
    va_copy(second_pass_args, args);

    /* first, get buffer size */

    const int size = FormatParseSql(stmt, NULL, sql, &first_pass_args);

    if (size > 0)
    {
        otext *sql_fmt = NULL;

        /* allocate buffer */

        ALLOC_DATA(OCI_IPC_STRING, sql_fmt, size + 1)

        if (STATUS)
        {
            /* format buffer */

            if (FormatParseSql(stmt, sql_fmt, sql, &second_pass_args) > 0)
            {
                /* prepare and execute SQL buffer */

                STATUS = StatementPrepareInternal(stmt, sql_fmt) && StatementExecuteInternal(stmt, OCI_DEFAULT);
            }

            FREE(sql_fmt)
        }
    }

    RETVAL = STATUS;

    va_end(first_pass_args);
    va_end(second_pass_args);

    CALL_EXIT()
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
    va_list first_pass_args;
    va_list second_pass_args;

    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_STATEMENT, stmt)
    CALL_CHECK_PTR(OCI_IPC_STRING, sql)
    CALL_CONTEXT_FROM_STMT(stmt)

    va_copy(first_pass_args, args);
    va_copy(second_pass_args, args);

    /* first, get buffer size */

    const int size = FormatParseSql(stmt, NULL, sql, &first_pass_args);

    if (size > 0)
    {
        otext *sql_fmt = NULL;

        /* allocate buffer */

        ALLOC_DATA(OCI_IPC_STRING, sql_fmt, size + 1)

        if (STATUS)
        {
            /* format buffer */

            if (FormatParseSql(stmt, sql_fmt, sql, &second_pass_args) > 0)
            {
                /* prepare and execute SQL buffer */

                STATUS = StatementPrepareInternal(stmt, sql_fmt) && StatementExecuteInternal(stmt, OCI_PARSE_ONLY);
            }

            FREE(sql_fmt)
        }
    }

    RETVAL = STATUS;

    va_end(first_pass_args);
    va_end(second_pass_args);

    CALL_EXIT()
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
    va_list first_pass_args;
    va_list second_pass_args;

    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_STATEMENT, stmt)
    CALL_CHECK_PTR(OCI_IPC_STRING, sql)
    CALL_CONTEXT_FROM_STMT(stmt)

    va_copy(first_pass_args, args);
    va_copy(second_pass_args, args);

    /* first, get buffer size */

    const int size = FormatParseSql(stmt, NULL, sql, &first_pass_args);

    if (size > 0)
    {
        otext *sql_fmt = NULL;

        /* allocate buffer */

        ALLOC_DATA(OCI_IPC_STRING, sql_fmt, size + 1)

        if (STATUS)
        {
            /* format buffer */

            if (FormatParseSql(stmt, sql_fmt, sql, &second_pass_args) > 0)
            {
                /* prepare and execute SQL buffer */

                STATUS = StatementPrepareInternal(stmt, sql_fmt) && StatementExecuteInternal(stmt, OCI_DESCRIBE_ONLY);
            }

            FREE(sql_fmt)
        }
    }

    RETVAL = STATUS;

    va_end(first_pass_args);
    va_end(second_pass_args);

    CALL_EXIT()
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
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_STATEMENT, stmt)
    CALL_CHECK_MIN(stmt->con, stmt, size, 1)
    CALL_CHECK_STMT_STATUS(stmt, OCI_STMT_PREPARED)
    CALL_CONTEXT_FROM_STMT(stmt)

    /* if the statements already has binds, we need to check if the new size is
       not greater than the initial size
    */

    if ((stmt->nb_ubinds > 0) && (stmt->nb_iters_init < size))
    {
        THROW(ExceptionBindArraySize(stmt, stmt->nb_iters_init, stmt->nb_iters, size))
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

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * StatementGetBindArraySize
 * --------------------------------------------------------------------------------------------- */

unsigned int StatementGetBindArraySize
(
    OCI_Statement *stmt
)
{
    GET_PROP(unsigned int, 0, OCI_IPC_STATEMENT, stmt, nb_iters, stmt->con, stmt, stmt->con->err)
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
    SET_PROP(boolean, OCI_IPC_STATEMENT, stmt, bind_reuse, value, stmt->con, stmt, stmt->con->err)
}

/* --------------------------------------------------------------------------------------------- *
 * StatementIsRebindingAllowed
 * --------------------------------------------------------------------------------------------- */

boolean StatementIsRebindingAllowed
(
    OCI_Statement *stmt
)
{
    GET_PROP(boolean, FALSE, OCI_IPC_STATEMENT, stmt, bind_reuse, stmt->con, stmt, stmt->con->err)
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
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_BIND(stmt, name, data, OCI_IPC_BOOLEAN, TRUE)
    CALL_CHECK_EXTENDED_PLSQLTYPES_ENABLED(stmt->con)
    CALL_CONTEXT_FROM_STMT(stmt)

#if OCI_VERSION_COMPILE >= OCI_12_1

    BIND_DATA(sizeof(boolean), OCI_CDT_BOOLEAN, SQLT_BOL, 0, NULL, 0)

#else

    OCI_NOT_USED(name)
    OCI_NOT_USED(data)

#endif

    RETVAL = STATUS;

    CALL_EXIT()
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
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_BIND(stmt, name, data, OCI_IPC_STRING, FALSE)
    CALL_CONTEXT_FROM_STMT(stmt)

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

            THROW(ExceptionMinimumValue(stmt->con, stmt, 1))
        }
    }

    const unsigned int size = (len + 1) * (ub4) sizeof(dbtext);

    BIND_DATA(size, OCI_CDT_TEXT, SQLT_STR, 0, NULL, 0)

    RETVAL = STATUS;

    CALL_EXIT()
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
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_BIND(stmt, name, data, OCI_IPC_STRING, FALSE)
    CALL_CHECK_MIN(stmt->con, stmt, len, 1)
    CALL_CONTEXT_FROM_STMT(stmt)

    const unsigned int size = (len + 1) * (ub4) sizeof(dbtext);

    BIND_DATA(size, OCI_CDT_TEXT, SQLT_STR, 0, NULL, nbelem)

    RETVAL = STATUS;

    CALL_EXIT()
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
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_BIND(stmt, name, data, OCI_IPC_VOID, FALSE)
    CALL_CONTEXT_FROM_STMT(stmt)

    if (len == 0 && !data)
    {
        /* if data is NULL, it means that binding mode is OCI_BAM_INTERNAL.
        An invalid length passed to the function, we do not have a valid length to
        allocate internal array, thus we need to raise an exception */

        THROW(ExceptionMinimumValue(stmt->con, stmt, 1))
    }

    BIND_DATA(len, OCI_CDT_RAW, SQLT_BIN, 0, NULL, 0)

    RETVAL = STATUS;

    CALL_EXIT()
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
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_BIND(stmt, name, data, OCI_IPC_VOID, FALSE)
    CALL_CHECK_MIN(stmt->con, stmt, len, 1)
    CALL_CONTEXT_FROM_STMT(stmt)

    BIND_DATA(len, OCI_CDT_RAW, SQLT_BIN, 0, NULL, nbelem)

    RETVAL = STATUS;

    CALL_EXIT()
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
    unsigned int code = SQLT_FLT;

    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_BIND(stmt, name, data, OCI_IPC_DOUBLE, FALSE)
    CALL_CONTEXT_FROM_STMT(stmt)

#if OCI_VERSION_COMPILE >= OCI_10_1

    if (ConnectionIsVersionSupported(stmt->con, OCI_10_1))
    {
        code = SQLT_BDOUBLE;
    }

#endif

    BIND_DATA(sizeof(double), OCI_CDT_NUMERIC, code, OCI_NUM_DOUBLE, NULL, 0)

    RETVAL = STATUS;

    CALL_EXIT()
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
    unsigned int code = SQLT_FLT;

    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_BIND(stmt, name, data, OCI_IPC_DOUBLE, FALSE)
    CALL_CONTEXT_FROM_STMT(stmt)

#if OCI_VERSION_COMPILE >= OCI_10_1

    if (ConnectionIsVersionSupported(stmt->con, OCI_10_1))
    {
        code = SQLT_BDOUBLE;
    }

#endif

    BIND_DATA(sizeof(double), OCI_CDT_NUMERIC, code, OCI_NUM_DOUBLE, NULL, nbelem)

    RETVAL = STATUS;

    CALL_EXIT()
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
    unsigned int code = SQLT_FLT;

    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_BIND(stmt, name, data, OCI_IPC_FLOAT, FALSE);
    CALL_CONTEXT_FROM_STMT(stmt)

#if OCI_VERSION_COMPILE >= OCI_10_1

    if (ConnectionIsVersionSupported(stmt->con, OCI_10_1))
    {
        code = SQLT_BFLOAT;
    }

#endif

    BIND_DATA(sizeof(float), OCI_CDT_NUMERIC, code, OCI_NUM_FLOAT, NULL, 0)

    RETVAL = STATUS;

    CALL_EXIT()
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
    unsigned int code = SQLT_FLT;

    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_BIND(stmt, name, data, OCI_IPC_FLOAT, FALSE)
    CALL_CONTEXT_FROM_STMT(stmt)

#if OCI_VERSION_COMPILE >= OCI_10_1

    if (ConnectionIsVersionSupported(stmt->con, OCI_10_1))
    {
        code = SQLT_BFLOAT;
    }

#endif

    BIND_DATA(sizeof(float), OCI_CDT_NUMERIC, code, OCI_NUM_FLOAT, NULL, nbelem)

    RETVAL = STATUS;

    CALL_EXIT()
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
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_BIND(stmt, name, data, OCI_IPC_TIMESTAMP, TRUE)
    CALL_CHECK_TIMESTAMP_ENABLED(stmt->con)
    CALL_CONTEXT_FROM_STMT(stmt)

#if OCI_VERSION_COMPILE >= OCI_9_0

    BIND_DATA(sizeof(OCIDateTime *), OCI_CDT_TIMESTAMP,
              ExternalSubTypeToSQLType(OCI_CDT_TIMESTAMP, data->type),
              data->type, NULL, 0)

#endif

    RETVAL = STATUS;

    CALL_EXIT()
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
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_BIND(stmt, name, data, OCI_IPC_TIMESTAMP, FALSE)
    CALL_CHECK_TIMESTAMP_ENABLED(stmt->con)

#if OCI_VERSION_COMPILE >= OCI_9_0

    CALL_CHECK_ENUM_VALUE(stmt->con, stmt, type, TimestampTypeValues, OTEXT("Timestamp type"))
    CALL_CONTEXT_FROM_STMT(stmt)

    BIND_DATA(sizeof(OCIDateTime *), OCI_CDT_TIMESTAMP,
              ExternalSubTypeToSQLType(OCI_CDT_TIMESTAMP, type),
              type, NULL, nbelem)

#else

    OCI_NOT_USED(name)
    OCI_NOT_USED(type)
    OCI_NOT_USED(nbelem)

#endif

    RETVAL = STATUS;

    CALL_EXIT()
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
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_BIND(stmt, name, data, OCI_IPC_INTERVAL, TRUE)
    CALL_CHECK_INTERVAL_ENABLED(stmt->con)
    CALL_CONTEXT_FROM_STMT(stmt)

#if OCI_VERSION_COMPILE >= OCI_9_0

    BIND_DATA(sizeof(OCIInterval *), OCI_CDT_INTERVAL,
              ExternalSubTypeToSQLType(OCI_CDT_INTERVAL, data->type),
              data->type, NULL, 0)

#else

    OCI_NOT_USED(name)

#endif

    RETVAL = STATUS;

    CALL_EXIT()
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
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_BIND(stmt, name, data, OCI_IPC_INTERVAL, FALSE)
    CALL_CHECK_INTERVAL_ENABLED(stmt->con)

#if OCI_VERSION_COMPILE >= OCI_9_0

    CALL_CHECK_ENUM_VALUE(stmt->con, stmt, type, IntervalTypeValues, OTEXT("Interval type"))
    CALL_CONTEXT_FROM_STMT(stmt)

    BIND_DATA(sizeof(OCIInterval *), OCI_CDT_INTERVAL,
              ExternalSubTypeToSQLType(OCI_CDT_INTERVAL, type),
              type, NULL, nbelem)

#else

    OCI_NOT_USED(name)
    OCI_NOT_USED(type)
    OCI_NOT_USED(nbelem)

#endif

    RETVAL = STATUS;

    CALL_EXIT()
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
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_BIND(stmt, name, data, OCI_IPC_OBJECT, FALSE)
    CALL_CHECK_PTR(OCI_IPC_TYPE_INFO, typinf)
    CALL_CONTEXT_FROM_STMT(stmt)

    BIND_DATA(sizeof(void *), OCI_CDT_OBJECT, SQLT_NTY, 0, typinf, nbelem)

    RETVAL = STATUS;

    CALL_EXIT()
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
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_BIND(stmt, name, data, OCI_IPC_LOB, FALSE)
    CALL_CONTEXT_FROM_STMT(stmt)
    CALL_CHECK_ENUM_VALUE(stmt->con, stmt, type, LobTypeValues, OTEXT("Lob type"))

    BIND_DATA(sizeof(OCILobLocator*), OCI_CDT_LOB,
              ExternalSubTypeToSQLType(OCI_CDT_LOB, type),
              type, NULL, nbelem)

    RETVAL = STATUS;

    CALL_EXIT()
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
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_BIND(stmt, name, data, OCI_IPC_LOB, OCI_IPC_FILE)
    CALL_CHECK_ENUM_VALUE(stmt->con, stmt, type, FileTypeValues, OTEXT("File type"))
    CALL_CONTEXT_FROM_STMT(stmt)

    BIND_DATA(sizeof(OCILobLocator*), OCI_CDT_FILE,
              ExternalSubTypeToSQLType(OCI_CDT_FILE, type),
              type, NULL, nbelem)

    RETVAL = STATUS;

    CALL_EXIT()
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
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_BIND(stmt, name, data, OCI_IPC_STATEMENT, TRUE)
    CALL_CONTEXT_FROM_STMT(stmt)

    BIND_DATA(sizeof(OCIStmt*), OCI_CDT_CURSOR, SQLT_RSET, 0, NULL, 0)

    RETVAL = STATUS;
    CALL_EXIT()
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
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_REGISTER(stmt, name)
    CALL_CHECK_MIN(stmt->con, stmt, len, 1)
    CALL_CONTEXT_FROM_STMT(stmt)

    const int size = (len + 1) * (ub4) sizeof(dbtext);

    REGISTER_DATA(size, OCI_CDT_TEXT, SQLT_STR, 0, NULL, 0)

    RETVAL = STATUS;

    CALL_EXIT()
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
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_REGISTER(stmt, name)
    CALL_CHECK_MIN(stmt->con, stmt, len, 1)
    CALL_CONTEXT_FROM_STMT(stmt)

    REGISTER_DATA(len, OCI_CDT_RAW, SQLT_BIN, 0, NULL, 0)

    RETVAL = STATUS;

    CALL_EXIT()
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
    unsigned int code = SQLT_ODT;
    unsigned int size = sizeof(OCIDate);

    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_REGISTER(stmt, name)
    CALL_CONTEXT_FROM_STMT(stmt)

    /* versions of OCI (< 10.2) crashes if SQLT_ODT is passed for output
       data with returning clause.
       It's an Oracle known bug #3269146 */

    if (ConnectionGetVersion(stmt->con) < OCI_10_2)
    {
        code = SQLT_DAT;
        size = 7;
    }

    REGISTER_DATA(size, OCI_CDT_DATETIME, code, 0, NULL, 0)

    RETVAL = STATUS;

    CALL_EXIT()
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
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_REGISTER(stmt, name)
    CALL_CHECK_TIMESTAMP_ENABLED(stmt->con)

#if OCI_VERSION_COMPILE >= OCI_9_0

    CALL_CHECK_ENUM_VALUE(stmt->con, stmt, type, TimestampTypeValues, OTEXT("Timestamp type"))
    CALL_CONTEXT_FROM_STMT(stmt)

    REGISTER_DATA(sizeof(OCIDateTime *), OCI_CDT_TIMESTAMP,
                  ExternalSubTypeToSQLType(OCI_CDT_TIMESTAMP, type),
                  type, NULL, 0)

#endif

    RETVAL = STATUS;

    CALL_EXIT()
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
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_REGISTER(stmt, name)
    CALL_CHECK_INTERVAL_ENABLED(stmt->con)

#if OCI_VERSION_COMPILE >= OCI_9_0

    CALL_CHECK_ENUM_VALUE(stmt->con, stmt, type, IntervalTypeValues, OTEXT("Interval type"))
    CALL_CONTEXT_FROM_STMT(stmt)

    REGISTER_DATA(sizeof(OCIInterval *), OCI_CDT_INTERVAL,
                  ExternalSubTypeToSQLType(OCI_CDT_INTERVAL, type),
                  type, NULL, 0)

#endif

    RETVAL = STATUS;

    CALL_EXIT()
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
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_REGISTER(stmt, name)
    CALL_CHECK_PTR(OCI_IPC_TYPE_INFO, typinf)
    CALL_CONTEXT_FROM_STMT(stmt)

    REGISTER_DATA(sizeof(void *), OCI_CDT_OBJECT, SQLT_NTY, 0, typinf, 0)

    RETVAL = STATUS;

    CALL_EXIT()
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
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_REGISTER(stmt, name)
    CALL_CHECK_ENUM_VALUE(stmt->con, stmt, type, LobTypeValues, OTEXT("Lob type"))
    CALL_CONTEXT_FROM_STMT(stmt)

    REGISTER_DATA(sizeof(OCILobLocator*), OCI_CDT_LOB,
                  ExternalSubTypeToSQLType(OCI_CDT_LOB, type),
                  type, NULL, 0)

    RETVAL = STATUS;

    CALL_EXIT()
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
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_REGISTER(stmt, name)
    CALL_CHECK_ENUM_VALUE(stmt->con, stmt, type, FileTypeValues, OTEXT("File type"))
    CALL_CONTEXT_FROM_STMT(stmt)

    REGISTER_DATA(sizeof(OCILobLocator*), OCI_CDT_FILE,
                  ExternalSubTypeToSQLType(OCI_CDT_FILE, type),
                  type, NULL, 0)

    RETVAL = STATUS;

    CALL_EXIT()
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
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_REGISTER(stmt, name)
    CALL_CHECK_PTR(OCI_IPC_TYPE_INFO, typinf)
    CALL_CONTEXT_FROM_STMT(stmt)

    REGISTER_DATA(sizeof(OCIRef *), OCI_CDT_REF, SQLT_REF, 0, typinf, 0)

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * StatementGetStatementType
 * --------------------------------------------------------------------------------------------- */

unsigned int StatementGetStatementType
(
    OCI_Statement *stmt
)
{
    GET_PROP(unsigned int, OCI_UNKNOWN, OCI_IPC_STATEMENT, stmt, type, stmt->con, stmt, stmt->con->err)
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
    unsigned int old_exec_mode = OCI_UNKNOWN;

    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_STATEMENT, stmt)
    CALL_CHECK_SCROLLABLE_CURSOR_ENABLED(stmt->con)
    CALL_CHECK_ENUM_VALUE(stmt->con, stmt, mode, FetchModeValues, OTEXT("Fetch mode"))
    CALL_CONTEXT_FROM_STMT(stmt)

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

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * StatementGetFetchMode
 * --------------------------------------------------------------------------------------------- */

unsigned int StatementGetFetchMode
(
    OCI_Statement *stmt
)
{
    GET_PROP(unsigned int, OCI_UNKNOWN, OCI_IPC_STATEMENT, stmt, exec_mode, stmt->con, stmt, stmt->con->err)
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
    SET_PROP_ENUM(ub1, OCI_IPC_STATEMENT, stmt, bind_mode, mode, BindModeValues, OTEXT("Bind mode"), stmt->con, stmt, stmt->con->err)
}

/* --------------------------------------------------------------------------------------------- *
 * StatementGetBindMode
 * --------------------------------------------------------------------------------------------- */

unsigned int StatementGetBindMode
(
    OCI_Statement *stmt
)
{
    GET_PROP(unsigned int, OCI_UNKNOWN, OCI_IPC_STATEMENT, stmt, bind_mode, stmt->con, stmt, stmt->con->err)
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
    SET_PROP_ENUM(ub1, OCI_IPC_STATEMENT, stmt, bind_alloc_mode, mode, BindAllocationValues, OTEXT(
                      "Bind Allocation"), stmt->con, stmt, stmt->con->err)
}

/* --------------------------------------------------------------------------------------------- *
 * StatementGetBindAllocation
 * --------------------------------------------------------------------------------------------- */

unsigned int StatementGetBindAllocation
(
    OCI_Statement *stmt
)
{
    GET_PROP(unsigned int, OCI_UNKNOWN, OCI_IPC_STATEMENT, stmt, bind_alloc_mode, stmt->con, stmt, stmt->con->err)
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
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_STATEMENT, stmt)
    CALL_CHECK_MIN(stmt->con, stmt, size, 1)
    CALL_CONTEXT_FROM_STMT(stmt)

    stmt->fetch_size = size;

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * StatementGetFetchSize
 * --------------------------------------------------------------------------------------------- */

unsigned int StatementGetFetchSize
(
    OCI_Statement *stmt
)
{
    GET_PROP(unsigned int, 0, OCI_IPC_STATEMENT, stmt, fetch_size, stmt->con, stmt, stmt->con->err)
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
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_STATEMENT, stmt)
    CALL_CONTEXT_FROM_STMT(stmt)

    stmt->prefetch_size = size;

    /* Prefetch is not working with scrollable cursors in Oracle 9iR1, thus disable it */

    if (stmt->exec_mode == OCI_SFM_SCROLLABLE && stmt->con->ver_num == OCI_9_0)
    {
        stmt->prefetch_size = 0;
    }

    if (stmt->stmt)
    {
        ATTRIB_SET(OCI_HTYPE_STMT, OCI_ATTR_PREFETCH_ROWS, stmt->stmt, &stmt->prefetch_size, sizeof(stmt->prefetch_size))
    }

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * StatementGetPrefetchSize
 * --------------------------------------------------------------------------------------------- */

unsigned int StatementGetPrefetchSize
(
    OCI_Statement *stmt
)
{
    GET_PROP(unsigned int, 0, OCI_IPC_STATEMENT, stmt, prefetch_size, stmt->con, stmt, stmt->con->err)
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
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_STATEMENT, stmt)
    CALL_CONTEXT_FROM_STMT(stmt)

    stmt->prefetch_mem = size;

    if (stmt->stmt)
    {
        ATTRIB_SET(OCI_HTYPE_STMT, OCI_ATTR_PREFETCH_MEMORY, stmt->stmt, &stmt->prefetch_mem, sizeof(stmt->prefetch_mem))
    }

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * StatementGetPrefetchMemory
 * --------------------------------------------------------------------------------------------- */

unsigned int StatementGetPrefetchMemory
(
    OCI_Statement *stmt
)
{
    GET_PROP(unsigned int, 0, OCI_IPC_STATEMENT, stmt, prefetch_mem, stmt->con, stmt, stmt->con->err)
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
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_STATEMENT, stmt)
    CALL_CHECK_MIN(stmt->con, stmt, size, 1)
    CALL_CONTEXT_FROM_STMT(stmt)

    stmt->long_size = size;

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * StatementGetLongMaxSize
 * --------------------------------------------------------------------------------------------- */

unsigned int StatementGetLongMaxSize
(
    OCI_Statement *stmt
)
{
    GET_PROP(unsigned int, 0, OCI_IPC_STATEMENT, stmt, long_size, stmt->con, stmt, stmt->con->err)
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
    SET_PROP_ENUM(ub1, OCI_IPC_STATEMENT, stmt, long_mode, mode, LongModeValues, OTEXT("Long Mode"), stmt->con, stmt, stmt->con->err)
}

/* --------------------------------------------------------------------------------------------- *
 * StatementGetLongMode
 * --------------------------------------------------------------------------------------------- */

unsigned int StatementGetLongMode
(
    OCI_Statement *stmt
)
{
    GET_PROP(unsigned int, OCI_UNKNOWN, OCI_IPC_STATEMENT, stmt, long_mode, stmt->con, stmt, stmt->con->err)
}

/* --------------------------------------------------------------------------------------------- *
 * StatementGetConnection
 * --------------------------------------------------------------------------------------------- */

OCI_Connection * StatementGetConnection
(
    OCI_Statement *stmt
)
{
    GET_PROP(OCI_Connection*, NULL, OCI_IPC_STATEMENT, stmt, con, stmt->con, stmt, stmt->con->err)
}

/* --------------------------------------------------------------------------------------------- *
 * StatementGetSql
 * --------------------------------------------------------------------------------------------- */

const otext * StatementGetSql
(
    OCI_Statement *stmt
)
{
    GET_PROP(const otext*, NULL, OCI_IPC_STATEMENT, stmt, sql, stmt->con, stmt, stmt->con->err)
}

/* --------------------------------------------------------------------------------------------- *
 * StatementGetSqlIdentifier
 * --------------------------------------------------------------------------------------------- */

const otext* StatementGetSqlIdentifier
(
    OCI_Statement *stmt
)
{
    GET_PROP(const otext*, NULL, OCI_IPC_STATEMENT, stmt, sql_id, stmt->con, stmt, stmt->con->err)
}

/* --------------------------------------------------------------------------------------------- *
 * StatementGetSqlErrorPos
 * --------------------------------------------------------------------------------------------- */

unsigned int StatementGetSqlErrorPos
(
    OCI_Statement *stmt
)
{
    GET_PROP(unsigned int, 0, OCI_IPC_STATEMENT, stmt, err_pos, stmt->con, stmt, stmt->con->err)
}

/* --------------------------------------------------------------------------------------------- *
 * StatementGetAffectedRows
 * --------------------------------------------------------------------------------------------- */

unsigned int StatementGetAffectedRows
(
    OCI_Statement *stmt
)
{
    ub4 count = 0;

    CALL_ENTER(unsigned int, count)
    CALL_CHECK_PTR(OCI_IPC_STATEMENT, stmt)
    CALL_CONTEXT_FROM_STMT(stmt)

    ATTRIB_GET(OCI_HTYPE_STMT, OCI_ATTR_ROW_COUNT, stmt->stmt, &count, NULL)

    RETVAL = count;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * StatementGetBindCount
 * --------------------------------------------------------------------------------------------- */

unsigned int StatementGetBindCount
(
    OCI_Statement *stmt
)
{
    GET_PROP(unsigned int, 0, OCI_IPC_STATEMENT, stmt, nb_ubinds, stmt->con, stmt, stmt->con->err)
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
    CALL_ENTER(OCI_Bind*, NULL)
    CALL_CHECK_PTR(OCI_IPC_STATEMENT, stmt)
    CALL_CHECK_BOUND(stmt->con, index, 1, stmt->nb_ubinds)
    CALL_CONTEXT_FROM_STMT(stmt)

    RETVAL = stmt->ubinds[index - 1];

    CALL_EXIT()
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
    int index = -1;

    CALL_ENTER(OCI_Bind*, NULL)
    CALL_CHECK_PTR(OCI_IPC_STATEMENT, stmt)
    CALL_CHECK_PTR(OCI_IPC_STRING, name)
    CALL_CONTEXT_FROM_STMT(stmt)

    index = BindGetIndex(stmt, name);

    if (index > 0)
    {
        RETVAL = stmt->ubinds[index - 1];
    }
    else
    {
        THROW(ExceptionItemNotFound(stmt->con, stmt, name, OCI_IPC_BIND))
    }

    CALL_EXIT()
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
    int index = -1;

    CALL_ENTER(unsigned int, 0)
    CALL_CHECK_PTR(OCI_IPC_STATEMENT, stmt)
    CALL_CHECK_PTR(OCI_IPC_STRING, name)
    CALL_CONTEXT_FROM_STMT(stmt)

    STATUS = FALSE;

    index = BindGetIndex(stmt, name);

    if (index >= 0)
    {
        RETVAL = index;
        STATUS = TRUE;
    }

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * StatementGetSqlCommand
 * --------------------------------------------------------------------------------------------- */

unsigned int StatementGetSqlCommand
(
    OCI_Statement *stmt
)
{
    ub2 code = OCI_UNKNOWN;

    CALL_ENTER(unsigned int, code)
    CALL_CHECK_PTR(OCI_IPC_STATEMENT, stmt)
    CALL_CHECK_STMT_STATUS(stmt, OCI_STMT_EXECUTED)
    CALL_CONTEXT_FROM_STMT(stmt)

    ATTRIB_GET(OCI_HTYPE_STMT, OCI_ATTR_SQLFNCODE, stmt->stmt, &code, NULL)

    RETVAL = code;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * StatementGetSqlVerb
 * --------------------------------------------------------------------------------------------- */

const otext * StatementGetSqlVerb
(
    OCI_Statement *stmt
)
{
    unsigned int code = OCI_UNKNOWN;

    CALL_ENTER(const otext *, NULL)
    CALL_CHECK_PTR(OCI_IPC_STATEMENT, stmt)
    CALL_CONTEXT_FROM_STMT(stmt)

    code = StatementGetSqlCommand(stmt);

    if (OCI_UNKNOWN != code)
    {
        for (int i = 0; i < OCI_SQLCMD_COUNT; i++)
        {
            if (code == SQLCmds[i].code)
            {
                RETVAL = SQLCmds[i].verb;
                break;
            }
        }
    }

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * StatementGetBatchError
 * --------------------------------------------------------------------------------------------- */

OCI_Error * StatementGetBatchError
(
    OCI_Statement *stmt
)
{
    CALL_ENTER(OCI_Error*, NULL)
    CALL_CHECK_PTR(OCI_IPC_STATEMENT, stmt)
    CALL_CONTEXT_FROM_STMT(stmt)

    if (stmt->batch && (stmt->batch->cur < stmt->batch->count))
    {
        RETVAL = &stmt->batch->errs[stmt->batch->cur++];
    }

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * StatementGetBatchErrorCount
 * --------------------------------------------------------------------------------------------- */

unsigned int StatementGetBatchErrorCount
(
    OCI_Statement *stmt
)
{
    CALL_ENTER(unsigned int, 0)
    CALL_CHECK_PTR(OCI_IPC_STATEMENT, stmt)
    CALL_CONTEXT_FROM_STMT(stmt)

    if (stmt->batch)
    {
        RETVAL = stmt->batch->count;
    }

    CALL_EXIT()
}
