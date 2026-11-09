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
#include "json.h"
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
#include "vector.h"

#if OCI_VERSION_COMPILE >= OCI_9_0

static OCI_UInt32 TimestampTypeValues[] =
{
    OCI_TIMESTAMP,
    OCI_TIMESTAMP_TZ,
    OCI_TIMESTAMP_LTZ
};

static OCI_UInt32 IntervalTypeValues[] =
{
    OCI_INTERVAL_YM,
    OCI_INTERVAL_DS
};

#endif

static OCI_UInt32 LobTypeValues[] =
{
    OCI_CLOB,
    OCI_NCLOB,
    OCI_BLOB
};

static OCI_UInt32 FileTypeValues[] =
{
    OCI_CFILE,
    OCI_BFILE
};

static OCI_UInt32 FetchModeValues[] =
{
    OCI_SFM_DEFAULT,
    OCI_SFM_SCROLLABLE
};

static OCI_UInt32 BindModeValues[] =
{
    OCI_BIND_BY_POS,
    OCI_BIND_BY_NAME
};

static OCI_UInt32 BindAllocationValues[] =
{
    OCI_BAM_EXTERNAL,
    OCI_BAM_INTERNAL
};

static OCI_UInt32 LongModeValues[] =
{
    OCI_LONG_EXPLICIT,
    OCI_LONG_IMPLICIT
};

#define CHECK_BIND(stmt, name, data, type, ext_only)                             \
                                                                                 \
        CHECK_PTR(OCI_IPC_STATEMENT, stmt);                                      \
        CHECK_PTR(OCI_IPC_STRING,    name);                                      \
        CHECK_STMT_STATUS(stmt, OCI_STMT_PREPARED);                              \
        {                                                                        \
            const OCI_Boolean ext_only_value = (ext_only);                       \
            if ((OCI_BAM_INTERNAL == (stmt)->bind_alloc_mode) &&                 \
                ((data) != NULL))                                                \
            {                                                                    \
                THROW(OcilibExceptionExternalBindingNotAllowed, (name));         \
            }                                                                    \
                                                                                 \
            if ((ext_only_value) || OCI_BAM_EXTERNAL == (stmt)->bind_alloc_mode) \
            {                                                                    \
                CHECK_PTR(type, data);                                           \
            }                                                                    \
        }                                                                        \

#define CHECK_REGISTER(stmt, name)          \
                                            \
        CHECK_PTR(OCI_IPC_STATEMENT, stmt); \
        CHECK_PTR(OCI_IPC_STRING,    name); \


#define SET_ARG_NUM(type, func)                                        \
                                                                       \
        type src = func(rs, i), *dst = ( type *) va_arg(args, type *); \
        if (dst)                                                       \
        {                                                              \
            *dst = src;                                                \
        }                                                              \

#define SET_ARG_HANDLE(type, func, assign)                             \
                                                                       \
        type *src = func(rs, i), *dst = (type *) va_arg(args, type *); \
        if (src && dst)                                                \
        {                                                              \
            res = assign(dst, src);                                    \
        }                                                              \

#define BIND_DATA(...)                                              \
                                                                    \
        OCI_Bind *bind_created = OcilibBindCreate(stmt, data, name, \
                                                  OCI_BIND_INPUT,   \
                                                  __VA_ARGS__);     \
        SET_RETVAL(bind_created);

#define REGISTER_DATA(...)                                          \
                                                                    \
        OCI_Bind *bind_created = OcilibBindCreate(stmt, NULL, name, \
                                                  OCI_BIND_OUTPUT,  \
                                                  __VA_ARGS__);     \
        SET_RETVAL(NULL != bind_created);

#define OCI_BIND_CALL(type, check, ...)                       \
                                                              \
        ENTER_FUNC(OCI_Bind*, NULL, OCI_IPC_STATEMENT, stmt); \
                                                              \
        CHECK_BIND(stmt, name, data, type, check);            \
                                                              \
        BIND_DATA(__VA_ARGS__);                               \
                                                              \
        EXIT_FUNC();                                          \

#define BIND_CALL_NULL_ALLOWED(type, ...) \
                                          \
        OCI_BIND_CALL(type, FALSE, __VA_ARGS__);

#define BIND_CALL_NULL_FORBIDDEN(type, ...) \
                                            \
        OCI_BIND_CALL(type, TRUE, __VA_ARGS__);

#define REGISTER_CALL(...)                                       \
                                                                 \
        ENTER_FUNC(OCI_Boolean, FALSE, OCI_IPC_STATEMENT, stmt); \
                                                                 \
        CHECK_REGISTER(stmt, name);                              \
                                                                 \
        REGISTER_DATA(__VA_ARGS__);                              \
                                                                 \
        EXIT_FUNC();                                             \

#define OCI_BIND_GET_SCALAR(s, t, i) (bnd->is_array ? ((t *) (s)) + (i) : (t *) (s))
#define OCI_BIND_GET_HANDLE(s, t, i) (bnd->is_array ? ((t **) (s))[i] : (t *) (s))
#define OCI_BIND_GET_BUFFER(d, t, i) ((t *)((d) + (i) * sizeof(t)))

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementBatchErrorClear
 * --------------------------------------------------------------------------------------------- */

static OCI_Boolean OcilibStatementBatchErrorClear
(
    OCI_Statement *stmt
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    );

    CHECK_PTR(OCI_IPC_STATEMENT, stmt);

    if (NULL != stmt->batch)
    {
        /* free internal array of OCI_Errors */

        FREE(stmt->batch->errs);

        /* free batch structure */

        FREE(stmt->batch);
    }

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementFreeAllBinds
 * --------------------------------------------------------------------------------------------- */

static OCI_Boolean OcilibStatementFreeAllBinds
(
    OCI_Statement *stmt
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    );

    CHECK_PTR(OCI_IPC_STATEMENT, stmt);

    OCI_Int32 i;

    /* free user binds */

    if (NULL != stmt->ubinds)
    {
        for(i = 0; i < stmt->nb_ubinds; i++)
        {
            OcilibBindFree(stmt->ubinds[i]);
        }

        FREE(stmt->ubinds);
    }

    /* free register binds */

    if (NULL != stmt->rbinds)
    {
        for(i = 0; i < stmt->nb_rbinds; i++)
        {
            OcilibBindFree(stmt->rbinds[i]);
        }

        FREE(stmt->rbinds);
    }

    stmt->nb_ubinds = 0;
    stmt->nb_rbinds = 0;

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementFreeParseBinds
 * --------------------------------------------------------------------------------------------- */

static OCI_Boolean OcilibStatementFreeParseBinds
(
    OCI_Statement* stmt
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    );

    CHECK_PTR(OCI_IPC_STATEMENT, stmt);

    if (NULL != stmt->parse_binds)
    {
        for (OCI_UInt32 i = 0; i < stmt->parse_binds->count; i++)
        {
            FREE(stmt->parse_binds->names[i]);
        }

        FREE(stmt->parse_binds->names);
        FREE(stmt->parse_binds);
    }

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementGetParseBinds
 * --------------------------------------------------------------------------------------------- */

static OCI_Boolean OcilibStatementGetParseBinds
(
    OCI_Statement* stmt
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    );

    OraText** bvnp = NULL;
    ub1     * bvnl = NULL;
    OraText** invp = NULL;
    ub1     * inpl = NULL;
    ub1     * dupl = NULL;
    OCIBind * hndl = NULL;

    CHECK_PTR(OCI_IPC_STATEMENT, stmt);

    if (NULL == stmt->parse_binds)
    {
        ub4 count = 0;

        CHECK_ATTRIB_GET
        (
            OCI_HTYPE_STMT, OCI_ATTR_BIND_COUNT,
            stmt->handle, &count, NULL,
            stmt->con->err
        );

        stmt->parse_binds = (OCI_ParseBinds*)OcilibMemoryAlloc(stmt->con->env,OCI_IPC_VOID, sizeof(OCI_ParseBinds), 1, TRUE);
        CHECK_NULL(stmt->parse_binds);

        stmt->parse_binds->names = (otext**)OcilibMemoryAlloc(stmt->con->env,OCI_IPC_VOID, sizeof(otext*), count, TRUE);
        CHECK_NULL(stmt->parse_binds->names);

        sb4 found = 0;
        bvnp = (OraText**)OcilibMemoryAlloc(stmt->con->env,OCI_IPC_VOID, sizeof(OraText*), count, TRUE);
        CHECK_NULL(bvnp);

        bvnl = (ub1*)OcilibMemoryAlloc(stmt->con->env,OCI_IPC_VOID, sizeof(ub1), count, TRUE);
        CHECK_NULL(bvnl);

        invp = (OraText**)OcilibMemoryAlloc(stmt->con->env,OCI_IPC_VOID, sizeof(OraText*), count, TRUE);
        CHECK_NULL(invp);

        inpl = (ub1*)OcilibMemoryAlloc(stmt->con->env,OCI_IPC_VOID, sizeof(ub1), count, TRUE);
        CHECK_NULL(inpl);

        dupl = (ub1*)OcilibMemoryAlloc(stmt->con->env,OCI_IPC_VOID, sizeof(ub1), count, TRUE);
        CHECK_NULL(dupl);

        hndl = (OCIBind*) OcilibMemoryAlloc(stmt->con->env,OCI_IPC_VOID, sizeof(OCIBind*), count, TRUE);
        CHECK_NULL(hndl);

        CHECK_OCI
        (
            stmt->con->err,
            OCIStmtGetBindInfo,
            stmt->handle,
            stmt->con->err,
            count, 1, &found,
            bvnp, bvnl,
            invp, inpl,
            dupl, NULL
        );

        /* names[] is packed: duplicates and positional binds are skipped,
           so names[0 .. count-1] never contains NULL holes */

        for (sb4 i = 0; i < found && stmt->parse_binds->count < count; i++)
        {
            if (stmt->bind_mode == OCI_BIND_BY_POS || 0 == dupl[i])
            {
                otext *name = OcilibStringDuplicateFromDBString
                              (
                                  stmt->con->env,
                                  (const dbtext*)bvnp[i],
                                  (OCI_Int32)dbcharcount(bvnl[i])
                              );

                CHECK_NULL(name);

                stmt->parse_binds->names[stmt->parse_binds->count++] = name;
            }
        }
    }

    CLEANUP_AND_EXIT_FUNC
    (
        FREE(bvnp);
        FREE(bvnl);
        FREE(invp);
        FREE(inpl);
        FREE(dupl);
        FREE(hndl);

        if (FAILURE)
        {
            OcilibStatementFreeParseBinds(stmt);
        }

        SET_RETVAL(NULL != stmt->parse_binds);
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementReset
 * --------------------------------------------------------------------------------------------- */

static OCI_Boolean OcilibStatementReset
(
    OCI_Statement *stmt
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    );

    CHECK_PTR(OCI_IPC_STATEMENT, stmt);

    ub4 mode = OCI_DEFAULT;

#if OCI_VERSION_COMPILE >= OCI_9_2

    if ((stmt->con->env->version_runtime >= OCI_9_2) && (stmt->nb_rbinds > 0))
    {
        /*  if we had registered binds, we must delete the statement from the cache.
            Because, if we execute another sql with "returning into clause",
            OCI_ProcInBind won't be called by OCI. Nice Oracle bug ! */

        const OCI_UInt32 cache_size = OcilibConnectionGetStatementCacheSize(stmt->con);

        if (cache_size > 0)
        {
            mode = OCI_STRLS_CACHE_DELETE;
        }
    }

#else

    OCI_NOT_USED(mode);

#endif

    /* reset batch errors */

    CHECK(OcilibStatementBatchErrorClear(stmt));

    /* free resultsets */

    CHECK(OcilibStatementReleaseResultsets(stmt));

    /* free in/out binds */

    CHECK(OcilibStatementFreeAllBinds(stmt));

    /* free parsed binds */

    CHECK(OcilibStatementFreeParseBinds(stmt));

    /* free bind map */

    if (NULL != stmt->map)
    {
        CHECK(OcilibHashFree(stmt->map));
    }

    /* free handle if needed */

    if (NULL != stmt->handle)
    {
        if (OCI_OBJECT_ALLOCATED == stmt->hstate)
        {

#if OCI_VERSION_COMPILE >= OCI_9_2

            if (stmt->con->env->version_runtime >= OCI_9_2)
            {
                CHECK_OCI
                (
                    stmt->con->err,
                    OCIStmtRelease,
                    stmt->handle, stmt->con->err, NULL, 0, mode
                );
            }
            else

#endif

            {
                OcilibMemoryFreeHandle(stmt->con->env, (dvoid*)stmt->handle, OCI_HTYPE_STMT);
            }

            stmt->handle = NULL;
        }
        else if (OCI_OBJECT_ALLOCATED_BIND_STMT == stmt->hstate)
        {
            OcilibMemoryFreeHandle(stmt->con->env, (dvoid*)stmt->handle, OCI_HTYPE_STMT);

            stmt->handle = NULL;
        }
    }

    /* free sql statement */

    FREE(stmt->sql);
    FREE(stmt->sql_id);

    stmt->rsts        = NULL;
    stmt->stmts       = NULL;
    stmt->sql         = NULL;
    stmt->sql_id      = NULL;
    stmt->map         = NULL;
    stmt->batch       = NULL;
    stmt->parse_binds = NULL;

    stmt->nb_rs   = 0;
    stmt->nb_stmt = 0;

    stmt->status     = OCI_STMT_CLOSED;
    stmt->type       = OCI_UNKNOWN;
    stmt->bind_array = FALSE;

    stmt->nb_iters      = 1;
    stmt->nb_iters_init = 1;
    stmt->dynidx        = 0;
    stmt->err_pos       = 0;

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
* OcilibStatementBindCheck
* --------------------------------------------------------------------------------------------- */

static OCI_Boolean OcilibStatementBindCheck
(
    OCI_Bind  *bnd,
    ub1       *src,
    ub1       *dst,
    OCI_UInt32 index
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_BIND, bnd
    );

    CHECK_PTR(OCI_IPC_BIND, bnd);
    CHECK_PTR(OCI_IPC_VOID, dst);

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
                        );
                    }
                }
            }
            else if (OCI_NUM_BIGINT & bnd->subtype)
            {
                OCI_Int64 *src_bint = OCI_BIND_GET_SCALAR(src, OCI_Int64, index);
                OCINumber *dst_num  = OCI_BIND_GET_BUFFER(dst, OCINumber, index);

                CHECK(OcilibNumberTranslateValue(bnd->stmt->con->env, src_bint,
                                                 bnd->subtype, dst_num, OCI_NUM_NUMBER));
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
                );
            }
        }

        /* String binds that may required conversion on systems where wchar_t is UTF32 */
        else if (OCI_CDT_TEXT == bnd->type)
        {
            if (bnd->stmt->con->env->use_wide_char_conv)
            {
                const OCI_Int32 max_chars  = dbcharcount(bnd->size);
                const OCI_Size  src_offset = (OCI_Size) index * (OCI_Size) max_chars * sizeof(otext);
                const OCI_Size  dst_offset = (OCI_Size) index * (OCI_Size) max_chars * sizeof(dbtext);

                OcilibStringUTF32ToUTF16(src + src_offset, dst + dst_offset, max_chars - 1);
            }
        }

        /* otherwise we have an ocilib handle based type */
        else
        {
            OCI_DataType*src_handle = OCI_BIND_GET_HANDLE(src, OCI_DataType, index);

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

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
* OcilibStatementBindUpdate
* --------------------------------------------------------------------------------------------- */

static OCI_Boolean OcilibStatementBindUpdate
(
    OCI_Bind  *bnd,
    ub1       *src,
    ub1       *dst,
    OCI_UInt32 index
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_BIND, bnd
    );

    CHECK_PTR(OCI_IPC_BIND, bnd);
    CHECK_PTR(OCI_IPC_VOID, src);
    CHECK_PTR(OCI_IPC_VOID, dst);

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
                    );
                }
            }
        }
        else if (OCI_NUM_BIGINT & bnd->subtype)
        {
            OCINumber *src_number = OCI_BIND_GET_BUFFER(src, OCINumber, index);
            OCI_Int64 *dst_bint   = OCI_BIND_GET_SCALAR(dst, OCI_Int64, index);

            if (dst_bint)
            {
                if (src_number && bnd->buffer.inds[index] != OCI_IND_NULL)
                {
                    CHECK
                    (
                        OcilibNumberTranslateValue
                        (
                            bnd->stmt->con->env,
                            src_number,
                            OCI_NUM_NUMBER,
                            dst_bint,
                            bnd->subtype
                        )
                    );
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
            );
        }
    }

    /* String binds that may required conversion on systems where wchar_t is UTF32 */
    else if (OCI_CDT_TEXT == bnd->type)
    {
        if (bnd->stmt->con->env->use_wide_char_conv)
        {
            const OCI_Int32 max_chars  = dbcharcount(bnd->size);
            const OCI_Size  src_offset = (OCI_Size) index * (OCI_Size) max_chars * sizeof(dbtext);
            const OCI_Size  dst_offset = (OCI_Size) index * (OCI_Size) max_chars * sizeof(otext);

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

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementBindCheckAll
 * --------------------------------------------------------------------------------------------- */

static OCI_Boolean OcilibStatementBindCheckAll
(
    OCI_Statement *stmt
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    );

    ub4 j;

    CHECK_PTR(OCI_IPC_STATEMENT, stmt);

    for (ub4 i = 0; i < stmt->nb_ubinds; i++)
    {
        OCI_Bind *bnd = stmt->ubinds[i];

        if (OCI_CDT_CURSOR == bnd->type)
        {
            OCI_Statement *bnd_stmt = (OCI_Statement *) bnd->input;

            OcilibStatementReset(bnd_stmt);

            bnd_stmt->hstate = OCI_OBJECT_ALLOCATED_BIND_STMT;

            /* allocate statement handle */

            CHECK
            (
                OcilibMemoryAllocHandle
                (
                    bnd_stmt->con->env,
                    bnd_stmt->con->env->handle,
                    (dvoid **)(void *)&bnd_stmt->handle,
                    OCI_HTYPE_STMT
                )
            );

            CHECK(OcilibStatementSetPrefetchSize(stmt, stmt->prefetch_size));
            CHECK(OcilibStatementSetFetchSize(stmt, stmt->fetch_size));
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
                    *(ub2*)(((ub1 *)bnd->buffer.lens) + (sizeof(ub2) * (OCI_Size) j)) = (ub2) bnd->size;
                }
            }

            /* extra work for internal allocated binds buffers */

            if (bnd->alloc)
            {
                if (bnd->is_array)
                {
                    const ub4 count = OCI_MAX(bnd->nbelem, stmt->nb_iters);

                    for (j = 0; j < count; j++)
                    {
                        CHECK(OcilibStatementBindCheck(bnd, (ub1*)bnd->input, (ub1*)bnd->buffer.data, j));
                    }
                }
                else
                {
                    CHECK(OcilibStatementBindCheck(bnd, (ub1*)bnd->input, (ub1*)bnd->buffer.data, 0));
                }
            }
        }
    }

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementBindUpdateAll
 * --------------------------------------------------------------------------------------------- */

static OCI_Boolean OcilibStatementBindUpdateAll
(
    OCI_Statement *stmt
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    );

    CHECK_PTR(OCI_IPC_STATEMENT, stmt);

    for (ub4 i = 0; i < stmt->nb_ubinds; i++)
    {
        OCI_Bind *bnd = stmt->ubinds[i];

        if (OCI_CDT_CURSOR == bnd->type)
        {
            OCI_Statement *bnd_stmt = (OCI_Statement *) bnd->input;

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
                    const ub4 count = OCI_MAX(bnd->nbelem, stmt->nb_iters);

                    for (ub4 j = 0; j < count; j++)
                    {
                        CHECK(OcilibStatementBindUpdate(bnd, (ub1*)bnd->buffer.data, (ub1*)bnd->input, j));
                    }
                }
                else
                {
                    CHECK(OcilibStatementBindUpdate(bnd, (ub1*)bnd->buffer.data, (ub1*)bnd->input, 0));
                }
            }
        }
    }

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementFetchIntoUserVariables
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibStatementFetchIntoUserVariables
(
    OCI_Statement *stmt,
    va_list        args
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    );

    CHECK_PTR(OCI_IPC_STATEMENT, stmt);

    OCI_Resultset *rs  = NULL;
    OCI_Boolean    res = FALSE;

    /* get resultset */

    rs = OcilibStatementGetResultset(stmt);

    /* fetch data */

    if (rs)
    {
        res = OcilibResultsetFetchNext(rs);
    }

    if (res)
    {
        OCI_UInt32 i, n;

        /* loop on column list for updating user given placeholders */

        for (i = 0, n = OcilibResultsetGetColumnCount(rs); (i < n) && res; i++)
        {
            OCI_Column *col = OcilibResultsetGetColumn(rs, i);

            const OCI_Int32 type = va_arg(args, OCI_Int32);

            switch (type)
            {
                case OCI_ARG_TEXT:
                {
                    const otext *src = OcilibResultsetGetString(rs, i);
                    otext       *dst = va_arg(args, otext *);

                    if (dst)
                    {
                        dst[0] = 0;

                        if (src && col)
                        {
                            /* OCI_ARG_TEXT carries no destination size, so the
                               only bound available is the column's declared
                               character width, which the caller is documented
                               to have allocated for. For non-character columns
                               (ROWID, numbers, ...) charsize is 0 and bufsize
                               is the raw descriptor size -- not a character
                               count -- so the bound is the produced string's
                               own length (already bounded internally by the
                               conversion buffer). Callers wanting a real bound
                               must use OCI_ARG_TEXT_N. */

                            OCI_Size max_chars = (OCI_Size) col->charsize;

                            if (0 == max_chars)
                            {
                                max_chars = ostrlen(src);
                            }

                            if (max_chars > 0)
                            {
                                ostrncat(dst, src, max_chars);
                            }
                        }
                    }

                    break;
                }
                case OCI_ARG_TEXT_N:
                {
                    const otext     *src   = OcilibResultsetGetString(rs, i);
                    otext           *dst   = va_arg(args, otext *);
                    const OCI_UInt32 count = va_arg(args, OCI_UInt32);

                    if (dst && count > 0)
                    {
                        dst[0] = 0;

                        if (src)
                        {
                            /* count includes the terminating null */

                            ostrncat(dst, src, (OCI_Size) count - 1);
                            dst[count - 1] = 0;
                        }
                    }

                    break;
                }
                case OCI_ARG_SHORT:
                {
                    SET_ARG_NUM(OCI_Int16, OcilibResultsetGetShort);
                    break;
                }
                case OCI_ARG_USHORT:
                {
                    SET_ARG_NUM(OCI_UInt16, OcilibResultsetGetUnsignedShort);
                    break;
                }
                case OCI_ARG_INT:
                {
                    SET_ARG_NUM(OCI_Int32, OcilibResultsetGetInt);
                    break;
                }
                case OCI_ARG_UINT:
                {
                    SET_ARG_NUM(OCI_UInt32, OcilibResultsetGetUnsignedInt);
                    break;
                }
                case OCI_ARG_BIGINT:
                {
                    SET_ARG_NUM(OCI_Int64, OcilibResultsetGetBigInt);
                    break;
                }
                case OCI_ARG_BIGUINT:
                {
                    SET_ARG_NUM(OCI_UInt64, OcilibResultsetGetUnsignedBigInt);
                    break;
                }
                case OCI_ARG_DOUBLE:
                {
                    SET_ARG_NUM(OCI_Double, OcilibResultsetGetDouble);
                    break;
                }
                case OCI_ARG_FLOAT:
                {
                    SET_ARG_NUM(OCI_Float, OcilibResultsetGetFloat);
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
                case OCI_ARG_VECTOR:
                {
                    SET_ARG_HANDLE(OCI_Vector, OcilibResultsetGetVector, OcilibVectorAssign);
                    break;
                }
                case OCI_ARG_JSON:
                {
                    SET_ARG_HANDLE(OCI_Json, OcilibResultsetGetJson, OcilibJsonAssign);
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

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementInitialize
 * --------------------------------------------------------------------------------------------- */

OCI_Statement * OcilibStatementInitialize
(
    OCI_Connection *con,
    OCI_Statement  *stmt,
    OCIStmt        *handle,
    OCI_Boolean     is_desc,
    const otext    *sql
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Statement*, stmt,
        /* context */ OCI_IPC_CONNECTION, con
    );

    CHECK_PTR(OCI_IPC_CONNECTION, con);

    ALLOC_HANDLE(OCI_IPC_STATEMENT, stmt);
    stmt->con    = con;
    stmt->handle = handle;

    stmt->exec_mode       = OCI_DEFAULT;
    stmt->piece_size      = OCI_SIZE_PIECE_DYNAMIC_FETCH;
    stmt->bind_reuse      = FALSE;
    stmt->bind_mode       = OCI_BIND_BY_NAME;
    stmt->long_mode       = OCI_LONG_EXPLICIT;
    stmt->bind_alloc_mode = OCI_BAM_EXTERNAL;
    stmt->fetch_size      = OCI_FETCH_SIZE;
    stmt->prefetch_size   = OCI_PREFETCH_SIZE;

    /* reset statement */

    CHECK(OcilibStatementReset(stmt));

    if (is_desc)
    {
        stmt->hstate = OCI_OBJECT_FETCHED_CLEAN;
        stmt->status = OCI_STMT_PREPARED  | OCI_STMT_PARSED |
                       OCI_STMT_DESCRIBED | OCI_STMT_EXECUTED;
        stmt->type = OCI_CST_SELECT;

        if (NULL != sql)
        {
            stmt->sql = OcilibStringDuplicate(stmt->con->env, sql);
        }
        else
        {
            dbtext   *dbstr  = NULL;
            OCI_Int32 dbsize = 0;

            CHECK_ATTRIB_GET
            (
                OCI_HTYPE_STMT, OCI_ATTR_STATEMENT,
                stmt->handle, &dbstr, &dbsize,
                stmt->con->err
            );

            if (NULL !=  dbstr)
            {
                stmt->sql = OcilibStringDuplicateFromDBString(stmt->con->env, dbstr, (OCI_Size) dbcharcount(dbsize));
                CHECK_NULL(stmt->sql);
            }
        }

        /* Setting fetch attributes here as the statement is already prepared */

        CHECK(OcilibStatementSetPrefetchSize(stmt, stmt->prefetch_size));
        CHECK(OcilibStatementSetFetchSize(stmt, stmt->fetch_size));
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

        SET_RETVAL(stmt);
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementDispose
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibStatementDispose
(
    OCI_Statement *stmt
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    );

    CHECK_PTR(OCI_IPC_STATEMENT, stmt);

    /* reset data */

    OcilibStatementReset(stmt);

    OcilibErrorResetSource(NULL, stmt);

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementCheckImplicitResultsets
 * --------------------------------------------------------------------------------------------- */

static OCI_Boolean OcilibStatementCheckImplicitResultsets
(
    OCI_Statement *stmt
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    );

    CHECK_PTR(OCI_IPC_STATEMENT, stmt);

#if OCI_VERSION_COMPILE >= OCI_12_1

    if (stmt->con->env->version_runtime >= OCI_12_1)
    {
        CHECK_ATTRIB_GET
        (
            OCI_HTYPE_STMT, OCI_ATTR_IMPLICIT_RESULT_COUNT,
            stmt->handle, &stmt->nb_stmt, NULL,
            stmt->con->err
        );

        if (stmt->nb_stmt > 0)
        {
            OCIStmt *result  = NULL;
            ub4      rs_type = OCI_UNKNOWN;
            ub4      i       = 0;

            /* allocate resultset handles array */

            ALLOC_DATA(OCI_IPC_STATEMENT_ARRAY, stmt->stmts, stmt->nb_stmt);
            ALLOC_DATA(OCI_IPC_RESULTSET_ARRAY, stmt->rsts,  stmt->nb_stmt);

            for (;;)
            {
                sword rs_next;

                OCI_CALL(rs_next, OCIStmtGetNextResult, stmt->handle, stmt->con->err, (dvoid  **)&result, &rs_type, OCI_DEFAULT);

                if (OCI_SUCCESS != rs_next)
                {
                    break;
                }

                if (OCI_RESULT_TYPE_SELECT == rs_type)
                {
                    stmt->stmts[i] = OcilibStatementInitialize(stmt->con, NULL, result, TRUE, NULL);

                    CHECK_NULL(stmt->stmts[i]);

                    stmt->rsts[i] = OcilibResultsetCreate(stmt->stmts[i], (OCI_Int32) stmt->stmts[i]->fetch_size);

                    CHECK_NULL(stmt->rsts[i]);

                    i++;
                    stmt->nb_rs++;
                }
            }
        }
    }

#endif

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementBatchErrorsInit
 * --------------------------------------------------------------------------------------------- */

static OCI_Boolean OcilibStatementBatchErrorInit
(
    OCI_Statement *stmt
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    );

    OCIError* hndl = NULL;

    ub4 err_count = 0;

    CHECK_PTR(OCI_IPC_STATEMENT, stmt);

    CHECK(OcilibStatementBatchErrorClear(stmt));

    /* all OCI call here are not checked for errors as we already dealing
       with an array DML error */

    CHECK_ATTRIB_GET
    (
        OCI_HTYPE_STMT, OCI_ATTR_NUM_DML_ERRORS,
        stmt->handle, &err_count, NULL,
        stmt->con->err
    );

    if (err_count > 0)
    {
        /* allocate batch error structure */

        ALLOC_DATA(OCI_IPC_BATCH_ERRORS, stmt->batch, 1);

        /* allocate array of error objects */

        ALLOC_DATA(OCI_IPC_ERROR, stmt->batch->errs, err_count);

        /* allocate OCI error handle */

        CHECK(OcilibMemoryAllocHandle(stmt->con->env, stmt->con->env->handle, (dvoid **)(void *)&hndl, OCI_HTYPE_ERROR));

        /* loop on the OCI errors to fill OCILIB error objects */

        stmt->batch->count = err_count;

        for (ub4 i = 0; i < stmt->batch->count; i++)
        {
            sb4 row = 0;

            OCI_Error *err = &stmt->batch->errs[i];

            err->env   = stmt->con->env;
            err->htype = OCI_IPC_ERROR;

            sword ret;

            OCI_CALL(ret, OCIParamGet, (dvoid *) stmt->con->err, OCI_HTYPE_ERROR,
                     stmt->con->err, (dvoid **) (void *) &hndl, i);

            if (NULL != hndl)
            {
                sb4       err_code = 0;
                otext     buffer[512];
                OCI_Int32 err_size = osizeof(buffer);

                dbtext * err_msg = OcilibStringGetDBString(stmt->con->env, buffer, &err_size);

                sword rethndl;

                OCI_CALL(rethndl, OCIAttrGet, (dvoid *)hndl, (ub4)OCI_HTYPE_ERROR,
                         (void *)&row, (ub4 *)NULL,
                         (ub4)OCI_ATTR_DML_ROW_OFFSET, stmt->con->err);

                OCI_ERROR_GET(hndl, &err_code, err_msg, err_size);

                OcilibErrorSet
                (
                    err,
                    OCI_ERR_TYPE_ERROR,
                    OCI_ERR_CAT_OCI,
                    (OCI_UInt32)err_code,
                    ocilib_call_context.source_ptr,
                    ocilib_call_context.source_type,
                    ocilib_call_context.location,
                    buffer,
                    (OCI_UInt32)(row + 1)
                );

                OcilibStringReleaseDBString(stmt->con->env, err_msg);
            }
        }
    }

    SET_SUCCESS();

    CLEANUP_AND_EXIT_FUNC
    (
        if (NULL != hndl)
        {
            OcilibMemoryFreeHandle(stmt->con->env, hndl, OCI_HTYPE_ERROR);
        }
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementPrepareInternal
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibStatementPrepareInternal
(
    OCI_Statement *stmt,
    const otext   *sql
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    );

    dbtext   *dbstr  = NULL;
    OCI_Int32 dbsize = -1;

    CHECK_PTR(OCI_IPC_STATEMENT, stmt);

    /* reset statement */

    CHECK(OcilibStatementReset(stmt));

    /* store SQL */

    stmt->sql = OcilibStringDuplicate(stmt->con->env, sql);

    OCILIB_LOG(OCI_LOG_CAT_LIB, OCI_LOG_INFO, OTEXT("handle=[%p]sql=[%s]"), stmt, sql ? sql : OTEXT(""));

    dbstr = OcilibStringGetDBString(stmt->con->env, stmt->sql, &dbsize);

    if (stmt->con->env->version_runtime < OCI_9_2)
    {
        /* allocate handle */

        CHECK(OcilibMemoryAllocHandle(stmt->con->env, stmt->con->env->handle, (dvoid **)(void *)&stmt->handle, OCI_HTYPE_STMT));
    }

    /* prepare SQL */

#if OCI_VERSION_COMPILE >= OCI_9_2

    if (stmt->con->env->version_runtime >= OCI_9_2)
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
            stmt->con->handle, &stmt->handle, stmt->con->err, (OraText *) dbstr,
            (ub4) dbsize, NULL, 0, (ub4) OCI_NTV_SYNTAX, (ub4) mode
        );
    }
    else

#endif

    {
        CHECK_OCI
        (
            stmt->con->err,
            OCIStmtPrepare,
            stmt->handle,stmt->con->err, (OraText *) dbstr, (ub4) dbsize,
            (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT
        );
    }

    /* get statement type */

    CHECK_ATTRIB_GET
    (
        OCI_HTYPE_STMT, OCI_ATTR_STMT_TYPE,
        stmt->handle, &stmt->type, NULL,
        stmt->con->err
    );

    /* update statement status */

    stmt->status = OCI_STMT_PREPARED;

    CHECK(OcilibStatementSetPrefetchSize(stmt, stmt->prefetch_size));
    CHECK(OcilibStatementSetFetchSize(stmt, stmt->fetch_size));

    SET_SUCCESS();

    CLEANUP_AND_EXIT_FUNC
    (
        OcilibStringReleaseDBString(stmt->con->env, dbstr);
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementExecuteInternal
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibStatementExecuteInternal
(
    OCI_Statement *stmt,
    ub4            mode
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    );

    CHECK_PTR(OCI_IPC_STATEMENT, stmt);

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

        if (iters > 1 && !IS_PLSQL_STMT(stmt->type))
        {
            mode = mode | OCI_BATCH_ERRORS;
        }
    }

    /* reset batch errors */

    CHECK(OcilibStatementBatchErrorClear(stmt));

    /* check bind objects for updating their null indicator status */

    CHECK(OcilibStatementBindCheckAll(stmt));

    /* check current resultsets */

    if (stmt->rsts)
    {
        /* Release any previous resultset before re-executing the statement;
           the SQL may have changed and the cached define objects/descriptors
           must not be reused or leaked. */

        CHECK(OcilibStatementReleaseResultsets(stmt));
    }

    /* Oracle execute call */

    sword ret;

    OCI_CALL(ret, OCIStmtExecute, stmt->con->handle, stmt->handle, stmt->con->err,
             iters, (ub4)0, (OCISnapshot *)NULL,
             (OCISnapshot *)NULL, mode);

    /* check result */

    OCI_Boolean success = ((OCI_SUCCESS           == ret) ||
        (OCI_SUCCESS_WITH_INFO == ret) ||
        (OCI_NEED_DATA         == ret));

    /* handle no data as error only for PL/SQL statements as it results from NO_DATA_FOUND exception
     * in PL/SQL */

    if (!success && !IS_PLSQL_STMT(stmt->type))
    {
        success = (OCI_NO_DATA == ret);
    }

    if (OCI_SUCCESS_WITH_INFO == ret)
    {
        OcilibExceptionOCI(&ocilib_call_context, stmt->con->err, ret);
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
                OCI_UInt32 size_id = 0;

                OcilibStringGetAttribute(stmt->con, stmt->handle, OCI_HTYPE_STMT,
                                         OCI_ATTR_SQL_ID, &stmt->sql_id, &size_id);
            }

#endif

            /* reset binds indicators */

            CHECK(OcilibStatementBindUpdateAll(stmt));

            /* commit if necessary */

            if (stmt->con->autocom)
            {
                CHECK(OcilibConnectionCommit(stmt->con));
            }

            /* check if any implicit results are available */

            CHECK(OcilibStatementCheckImplicitResultsets(stmt));
        }
    }
    else
    {
        /* get parse error position type */

        /* (one of the rare OCI call not enclosed with a OCI_CALL macro ...) */

        sword ret2;

        OCI_CALL(ret2, OCIAttrGet, (dvoid *)stmt->handle, (ub4)OCI_HTYPE_STMT,
                 (dvoid *)&stmt->err_pos, (ub4 *)NULL,
                 (ub4)OCI_ATTR_PARSE_ERROR_OFFSET, stmt->con->err);

        /* raise exception */

        THROW(OcilibExceptionOCI, stmt->con->err, ret);
    }

    OCILIB_LOG(OCI_LOG_CAT_LIB, OCI_LOG_DEBUG, OTEXT("handle=[%p] type=[%d]"), stmt, (OCI_Int32) stmt->type);

    SET_SUCCESS();

    EXIT_FUNC();
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
    );

    CHECK_PTR(OCI_IPC_CONNECTION, con);

    /* create statement object */

    OCI_Statement *stmt = NULL;

    LIST_ATOMIC_ADD(con->stmts, stmt);

    CHECK_NULL(stmt);

    SET_RETVAL(OcilibStatementInitialize(con, (OCI_Statement*)stmt, NULL, FALSE, NULL));

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementFree
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibStatementFree
(
    OCI_Statement *stmt
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    );

    CHECK_PTR(OCI_IPC_STATEMENT, stmt);
    CHECK_OBJECT_FETCHED(stmt);

    OcilibErrorResetSource(NULL, stmt);

    LIST_ATOMIC_REMOVE(stmt->con->stmts, stmt, OcilibStatementDispose);

    FREE(stmt);

    SET_SUCCESS();

    EXIT_FUNC();
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
    );

    CHECK_PTR(OCI_IPC_STATEMENT, stmt);
    CHECK_STMT_STATUS(stmt, OCI_STMT_DESCRIBED);

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

            ALLOC_DATA(OCI_IPC_RESULTSET_ARRAY, stmt->rsts, 1);

            stmt->nb_rs  = 1;
            stmt->cur_rs = 0;

            /* create resultset object */

            rs = stmt->rsts[0] = OcilibResultsetCreate(stmt, (OCI_Int32) stmt->fetch_size);
        }

        CHECK_NULL(rs);
    }

    SET_RETVAL(rs);

    EXIT_FUNC();
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
    );

    CHECK_PTR(OCI_IPC_STATEMENT, stmt);
    CHECK_STMT_STATUS(stmt, OCI_STMT_DESCRIBED);
    CHECK(stmt->cur_rs < stmt->nb_rs-1);

    SET_RETVAL(stmt->rsts[++stmt->cur_rs]);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementReleaseResultsets
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibStatementReleaseResultsets
(
    OCI_Statement *stmt
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    );

    ub4 i;

    CHECK_PTR(OCI_IPC_STATEMENT, stmt);

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

        FREE(stmt->rsts);
    }

    /* release statements for implicit resultsets */
    if (NULL != stmt->stmts)
    {
        for (i = 0; i < stmt->nb_stmt; i++)
        {
            if (stmt->stmts[i] != NULL)
            {
                OcilibStatementDispose(stmt->stmts[i]);
                FREE(stmt->stmts[i]);
            }
        }

        FREE(stmt->stmts);
    }

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementPrepare
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibStatementPrepare
(
    OCI_Statement *stmt,
    const otext   *sql
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    );

    CHECK_PTR(OCI_IPC_STATEMENT, stmt);
    CHECK_PTR(OCI_IPC_STRING,    sql);

    CHECK(OcilibStatementPrepareInternal(stmt, sql));

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementExecute
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibStatementExecute
(
    OCI_Statement *stmt
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    );

    CHECK_PTR(OCI_IPC_STATEMENT, stmt);
    CHECK_STMT_STATUS(stmt, OCI_STMT_PREPARED);

    CHECK(OcilibStatementExecuteInternal(stmt, OCI_DEFAULT));

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementExecuteStmt
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibStatementExecuteStmt
(
    OCI_Statement *stmt,
    const otext   *sql
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    );

    CHECK_PTR(OCI_IPC_STATEMENT, stmt);
    CHECK_PTR(OCI_IPC_STRING,    sql);

    CHECK(OcilibStatementPrepareInternal(stmt, sql));
    CHECK(OcilibStatementExecuteInternal(stmt, OCI_DEFAULT));

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementExecuteAndGetResultset
 * --------------------------------------------------------------------------------------------- */

OCI_Resultset * OcilibStatementExecuteAndGetResultset
(
    OCI_Statement *stmt
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Resultset*, NULL,
        /* context */ OCI_IPC_STATEMENT, stmt
    );

    CHECK_PTR(OCI_IPC_STATEMENT, stmt);

    CHECK(OcilibStatementExecute(stmt));

    SET_RETVAL(OcilibStatementGetResultset(stmt));

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementExecuteStmtAndGetResultset
 * --------------------------------------------------------------------------------------------- */

OCI_Resultset * OcilibStatementExecuteStmtAndGetResultset
(
    OCI_Statement *stmt,
    const otext   *sql
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Resultset*, NULL,
        /* context */ OCI_IPC_STATEMENT, stmt
    );

    CHECK_PTR(OCI_IPC_STATEMENT, stmt);
    CHECK_PTR(OCI_IPC_STRING,    sql);

    CHECK(OcilibStatementExecuteStmt(stmt, sql));

    SET_RETVAL(OcilibStatementGetResultset(stmt));

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementParse
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibStatementParse
(
    OCI_Statement *stmt,
    const otext   *sql
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    );

    CHECK_PTR(OCI_IPC_STATEMENT, stmt);
    CHECK_PTR(OCI_IPC_STRING,    sql);

    CHECK(OcilibStatementPrepareInternal(stmt, sql));
    CHECK(OcilibStatementExecuteInternal(stmt, OCI_PARSE_ONLY));

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementDescribe
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibStatementDescribe
(
    OCI_Statement *stmt,
    const otext   *sql
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    );

    CHECK_PTR(OCI_IPC_STATEMENT, stmt);
    CHECK_PTR(OCI_IPC_STRING,    sql);

    CHECK(OcilibStatementPrepareInternal(stmt, sql));

    if (OCI_CST_SELECT == stmt->type)
    {
        CHECK(OcilibStatementExecuteInternal(stmt, OCI_DESCRIBE_ONLY));
    }

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementPrepareFmt
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibStatementPrepareFmt
(
    OCI_Statement *stmt,
    const otext   *sql,
    va_list        args
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    );

    otext* sql_fmt = NULL;

    va_list first_pass_args;
    va_list second_pass_args;

    CHECK_PTR(OCI_IPC_STATEMENT, stmt);
    CHECK_PTR(OCI_IPC_STRING,    sql);

    va_copy(first_pass_args,  args);
    va_copy(second_pass_args, args);

    /* first, get buffer size */

    const OCI_Int32 size = OcilibFormatParseSql(stmt, NULL, sql, &first_pass_args);
    CHECK(size > 0);

    /* allocate buffer */

    ALLOC_DATA(OCI_IPC_STRING, sql_fmt, size + 1);

    /* format buffer */

    CHECK(OcilibFormatParseSql(stmt, sql_fmt, sql, &second_pass_args) > 0);

    /* parse buffer */

    CHECK(OcilibStatementPrepareInternal(stmt, sql_fmt));

    SET_SUCCESS();

    CLEANUP_AND_EXIT_FUNC
    (
        va_end(first_pass_args);
        va_end(second_pass_args);

        FREE(sql_fmt);
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementExecuteStmtFmt
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibStatementExecuteStmtFmt
(
    OCI_Statement *stmt,
    const otext   *sql,
    va_list        args
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    );

    va_list first_pass_args;
    va_list second_pass_args;

    otext* sql_fmt = NULL;

    CHECK_PTR(OCI_IPC_STATEMENT, stmt);
    CHECK_PTR(OCI_IPC_STRING,    sql);

    va_copy(first_pass_args,  args);
    va_copy(second_pass_args, args);

    /* first, get buffer size */

    const OCI_Int32 size = OcilibFormatParseSql(stmt, NULL, sql, &first_pass_args);
    CHECK(size > 0);

    /* allocate buffer */

    ALLOC_DATA(OCI_IPC_STRING, sql_fmt, size + 1);

    /* format buffer */

    CHECK(OcilibFormatParseSql(stmt, sql_fmt, sql, &second_pass_args) > 0);

    /* prepare and execute SQL buffer */

    CHECK(OcilibStatementPrepareInternal(stmt, sql_fmt));
    CHECK(OcilibStatementExecuteInternal(stmt, OCI_DEFAULT));

    SET_SUCCESS();

    CLEANUP_AND_EXIT_FUNC
    (
        va_end(first_pass_args);
        va_end(second_pass_args);

        FREE(sql_fmt);
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementParseFmt
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibStatementParseFmt
(
    OCI_Statement *stmt,
    const otext   *sql,
    va_list        args
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    );

    va_list first_pass_args;
    va_list second_pass_args;

    otext* sql_fmt = NULL;

    CHECK_PTR(OCI_IPC_STATEMENT, stmt);
    CHECK_PTR(OCI_IPC_STRING,    sql);

    va_copy(first_pass_args,  args);
    va_copy(second_pass_args, args);

    /* first, get buffer size */

    const OCI_Int32 size = OcilibFormatParseSql(stmt, NULL, sql, &first_pass_args);
    CHECK(size > 0);

    /* allocate buffer */

    ALLOC_DATA(OCI_IPC_STRING, sql_fmt, size + 1);

    /* format buffer */

    CHECK(OcilibFormatParseSql(stmt, sql_fmt, sql, &second_pass_args) > 0);

    /* prepare and execute SQL buffer */

    CHECK(OcilibStatementPrepareInternal(stmt, sql_fmt));
    CHECK(OcilibStatementExecuteInternal(stmt, OCI_PARSE_ONLY));

    SET_SUCCESS();

    CLEANUP_AND_EXIT_FUNC
    (
        va_end(first_pass_args);
        va_end(second_pass_args);

        FREE(sql_fmt);
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementDescribeFmt
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibStatementDescribeFmt
(
    OCI_Statement *stmt,
    const otext   *sql,
    va_list        args
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    );

    va_list first_pass_args;
    va_list second_pass_args;

    otext* sql_fmt = NULL;

    CHECK_PTR(OCI_IPC_STATEMENT, stmt);
    CHECK_PTR(OCI_IPC_STRING,    sql);

    va_copy(first_pass_args,  args);
    va_copy(second_pass_args, args);

    /* first, get buffer size */

    const OCI_Int32 size = OcilibFormatParseSql(stmt, NULL, sql, &first_pass_args);
    CHECK(size > 0);

    /* allocate buffer */

    ALLOC_DATA(OCI_IPC_STRING, sql_fmt, size + 1);

    /* format buffer */

    CHECK(OcilibFormatParseSql(stmt, sql_fmt, sql, &second_pass_args) > 0);

    /* prepare and execute SQL buffer */

    CHECK(OcilibStatementPrepareInternal(stmt, sql_fmt));
    CHECK(OcilibStatementExecuteInternal(stmt, OCI_DESCRIBE_ONLY));

    SET_SUCCESS();

    CLEANUP_AND_EXIT_FUNC
    (
        va_end(first_pass_args);
        va_end(second_pass_args);

        FREE(sql_fmt);
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementSetBindArraySize
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibStatementSetBindArraySize
(
    OCI_Statement *stmt,
    OCI_UInt32     size
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    );

    CHECK_PTR(OCI_IPC_STATEMENT, stmt);
    CHECK_MIN(size, 1);
    CHECK_STMT_STATUS(stmt, OCI_STMT_PREPARED);

    /* if the statements already has binds, we need to check if the new size is
       not greater than the initial size
    */

    if ((stmt->nb_ubinds > 0) && (stmt->nb_iters_init < size))
    {
        THROW(OcilibExceptionBindArraySize, stmt->nb_iters_init, stmt->nb_iters, size);
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

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementGetBindArraySize
 * --------------------------------------------------------------------------------------------- */

OCI_UInt32 OcilibStatementGetBindArraySize
(
    OCI_Statement *stmt
)
{
    GET_PROP
    (
        OCI_UInt32, 0,
        OCI_IPC_STATEMENT, stmt,
        nb_iters
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementAllowRebinding
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibStatementAllowRebinding
(
    OCI_Statement *stmt,
    OCI_Boolean    value
)
{
    SET_PROP
    (
        /* handle */ OCI_IPC_STATEMENT, stmt,
        /* member */ bind_reuse, OCI_Boolean,
        /* value  */ value
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementIsRebindingAllowed
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibStatementIsRebindingAllowed
(
    OCI_Statement *stmt
)
{
    GET_PROP
    (
        OCI_Boolean, FALSE,
        OCI_IPC_STATEMENT, stmt,
        bind_reuse
    );
}

/* --------------------------------------------------------------------------------------------- *
* OcilibStatementBindBoolean
* --------------------------------------------------------------------------------------------- */

OCI_Bind* OcilibStatementBindBoolean
(
    OCI_Statement *stmt,
    const otext   *name,
    OCI_Boolean   *data
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Bind*, NULL,
        /* context */ OCI_IPC_STATEMENT, stmt
    );

    CHECK_BIND(stmt, name, data, OCI_IPC_BOOLEAN, TRUE);
    CHECK_EXTENDED_PLSQLTYPES_ENABLED(stmt->con);

#if OCI_VERSION_COMPILE >= OCI_12_1

    BIND_DATA(sizeof(OCI_Boolean), OCI_CDT_BOOLEAN, SQLT_BOL, 0, NULL, 0);

#else

    OCI_NOT_USED(name);
    OCI_NOT_USED(data);

#endif

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementBindNumber
 * --------------------------------------------------------------------------------------------- */

OCI_Bind* OcilibStatementBindNumber
(
    OCI_Statement *stmt,
    const otext   *name,
    OCI_Number    *data
)
{
    BIND_CALL_NULL_ALLOWED
    (
        OCI_IPC_SHORT, sizeof(OCINumber), OCI_CDT_NUMERIC, SQLT_VNU, OCI_NUM_NUMBER, NULL, 0
    );
}

/* --------------------------------------------------------------------------------------------- *
* OcilibStatementBindArrayOfNumbers
* --------------------------------------------------------------------------------------------- */

OCI_Bind* OcilibStatementBindArrayOfNumbers
(
    OCI_Statement *stmt,
    const otext   *name,
    OCI_Number   **data,
    OCI_UInt32     nbelem
)
{
    BIND_CALL_NULL_ALLOWED
    (
        OCI_IPC_NUMBER, sizeof(OCINumber), OCI_CDT_NUMERIC, SQLT_VNU, OCI_NUM_NUMBER, NULL, nbelem
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementBindShort
 * --------------------------------------------------------------------------------------------- */

OCI_Bind* OcilibStatementBindShort
(
    OCI_Statement *stmt,
    const otext   *name,
    OCI_Int16     *data
)
{
    BIND_CALL_NULL_ALLOWED
    (
        OCI_IPC_SHORT, sizeof(OCI_Int16), OCI_CDT_NUMERIC, SQLT_INT, OCI_NUM_SHORT, NULL, 0
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementBindArrayOfShorts
 * --------------------------------------------------------------------------------------------- */

OCI_Bind* OcilibStatementBindArrayOfShorts
(
    OCI_Statement *stmt,
    const otext   *name,
    OCI_Int16     *data,
    OCI_UInt32     nbelem
)
{
    BIND_CALL_NULL_ALLOWED
    (
        OCI_IPC_SHORT, sizeof(OCI_Int16), OCI_CDT_NUMERIC, SQLT_INT, OCI_NUM_SHORT, NULL, nbelem
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementBindUnsignedShort
 * --------------------------------------------------------------------------------------------- */

OCI_Bind* OcilibStatementBindUnsignedShort
(
    OCI_Statement *stmt,
    const otext   *name,
    OCI_UInt16    *data
)
{
    BIND_CALL_NULL_ALLOWED
    (
        OCI_IPC_SHORT, sizeof(OCI_UInt16), OCI_CDT_NUMERIC, SQLT_UIN, OCI_NUM_USHORT, NULL, 0
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementBindArrayOfUnsignedShorts
 * --------------------------------------------------------------------------------------------- */

OCI_Bind* OcilibStatementBindArrayOfUnsignedShorts
(
    OCI_Statement *stmt,
    const otext   *name,
    OCI_UInt16    *data,
    OCI_UInt32     nbelem
)
{
    BIND_CALL_NULL_ALLOWED
    (
        OCI_IPC_SHORT, sizeof(OCI_UInt16), OCI_CDT_NUMERIC, SQLT_UIN, OCI_NUM_USHORT, NULL, nbelem
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementBindInt
 * --------------------------------------------------------------------------------------------- */

OCI_Bind* OcilibStatementBindInt
(
    OCI_Statement *stmt,
    const otext   *name,
    OCI_Int32     *data
)
{
    BIND_CALL_NULL_ALLOWED
    (
        OCI_IPC_INT, sizeof(OCI_Int32), OCI_CDT_NUMERIC, SQLT_INT, OCI_NUM_INT, NULL, 0
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementBindArrayOfInts
 * --------------------------------------------------------------------------------------------- */

OCI_Bind* OcilibStatementBindArrayOfInts
(
    OCI_Statement *stmt,
    const otext   *name,
    OCI_Int32     *data,
    OCI_UInt32     nbelem
)
{
    BIND_CALL_NULL_ALLOWED
    (
        OCI_IPC_INT, sizeof(OCI_Int32), OCI_CDT_NUMERIC, SQLT_INT, OCI_NUM_INT, NULL, nbelem
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementBindUnsignedInt
 * --------------------------------------------------------------------------------------------- */

OCI_Bind* OcilibStatementBindUnsignedInt
(
    OCI_Statement *stmt,
    const otext   *name,
    OCI_UInt32    *data
)
{
    BIND_CALL_NULL_ALLOWED
    (
        OCI_IPC_INT, sizeof(OCI_UInt32), OCI_CDT_NUMERIC, SQLT_UIN, OCI_NUM_UINT, NULL, 0
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementBindArrayOfUnsignedInts
 * --------------------------------------------------------------------------------------------- */

OCI_Bind* OcilibStatementBindArrayOfUnsignedInts
(
    OCI_Statement *stmt,
    const otext   *name,
    OCI_UInt32    *data,
    OCI_UInt32     nbelem
)
{
    BIND_CALL_NULL_ALLOWED
    (
        OCI_IPC_INT, sizeof(OCI_UInt32), OCI_CDT_NUMERIC, SQLT_UIN, OCI_NUM_UINT, NULL, nbelem
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementBindBigInt
 * --------------------------------------------------------------------------------------------- */

OCI_Bind* OcilibStatementBindBigInt
(
    OCI_Statement *stmt,
    const otext   *name,
    OCI_Int64     *data
)
{
    BIND_CALL_NULL_ALLOWED
    (
        OCI_IPC_BIGINT, sizeof(OCINumber), OCI_CDT_NUMERIC, SQLT_VNU, OCI_NUM_BIGINT, NULL, 0
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementBindArrayOfBigInts
 * --------------------------------------------------------------------------------------------- */

OCI_Bind* OcilibStatementBindArrayOfBigInts
(
    OCI_Statement *stmt,
    const otext   *name,
    OCI_Int64     *data,
    OCI_UInt32     nbelem
)
{
    BIND_CALL_NULL_ALLOWED
    (
        OCI_IPC_BIGINT, sizeof(OCINumber), OCI_CDT_NUMERIC, SQLT_VNU, OCI_NUM_BIGINT, NULL, nbelem
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementBindUnsignedBigInt
 * --------------------------------------------------------------------------------------------- */

OCI_Bind* OcilibStatementBindUnsignedBigInt
(
    OCI_Statement *stmt,
    const otext   *name,
    OCI_UInt64    *data
)
{
    BIND_CALL_NULL_ALLOWED
    (
        OCI_IPC_BIGINT, sizeof(OCINumber), OCI_CDT_NUMERIC, SQLT_VNU, OCI_NUM_BIGUINT, NULL, 0
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementBindArrayOfUnsignedInts
 * --------------------------------------------------------------------------------------------- */

OCI_Bind* OcilibStatementBindArrayOfUnsignedBigInts
(
    OCI_Statement *stmt,
    const otext   *name,
    OCI_UInt64    *data,
    OCI_UInt32     nbelem
)
{
    BIND_CALL_NULL_ALLOWED
    (
        OCI_IPC_BIGINT, sizeof(OCINumber), OCI_CDT_NUMERIC, SQLT_VNU, OCI_NUM_BIGUINT, NULL, nbelem
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementBindString
 * --------------------------------------------------------------------------------------------- */

OCI_Bind* OcilibStatementBindString
(
    OCI_Statement *stmt,
    const otext   *name,
    otext         *data,
    OCI_UInt32     len
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Bind*, NULL,
        /* context */ OCI_IPC_STATEMENT, stmt
    );

    CHECK_BIND(stmt, name, data, OCI_IPC_STRING, FALSE);

    if ((len == 0) || len == (UINT_MAX))
    {
        if (data)
        {
            /* only compute length for external bind if no valid length has been provided */

            len = (OCI_UInt32) ostrlen(data);
        }
        else
        {
            /* if data is NULL, it means that binding mode is OCI_BAM_INTERNAL.
               An invalid length passed to the function, we do not have a valid length to
               allocate internal array, thus we need to raise an exception */

            THROW(OcilibExceptionMinimumValue, 1);
        }
    }

    const OCI_UInt32 size = (len + 1) * (ub4) sizeof(dbtext);

    BIND_DATA(size, OCI_CDT_TEXT, SQLT_STR, 0, NULL, 0);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementBindArrayOfStrings
 * --------------------------------------------------------------------------------------------- */

OCI_Bind* OcilibStatementBindArrayOfStrings
(
    OCI_Statement *stmt,
    const otext   *name,
    otext         *data,
    OCI_UInt32     len,
    OCI_UInt32     nbelem
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Bind*, NULL,
        /* context */ OCI_IPC_STATEMENT, stmt
    );

    CHECK_BIND(stmt, name, data, OCI_IPC_STRING, FALSE);
    CHECK_MIN(len, 1);

    const OCI_UInt32 size = (len + 1) * (ub4) sizeof(dbtext);

    BIND_DATA(size, OCI_CDT_TEXT, SQLT_STR, 0, NULL, nbelem);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementBindRaw
 * --------------------------------------------------------------------------------------------- */

OCI_Bind* OcilibStatementBindRaw
(
    OCI_Statement *stmt,
    const otext   *name,
    void          *data,
    OCI_UInt32     len
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Bind*, NULL,
        /* context */ OCI_IPC_STATEMENT, stmt
    );

    CHECK_BIND(stmt, name, data, OCI_IPC_VOID, FALSE);

    if (len == 0 && !data)
    {
        /* if data is NULL, it means that binding mode is OCI_BAM_INTERNAL.
        An invalid length passed to the function, we do not have a valid length to
        allocate internal array, thus we need to raise an exception */

        THROW(OcilibExceptionMinimumValue, 1);
    }

    BIND_DATA(len, OCI_CDT_RAW, SQLT_BIN, 0, NULL, 0);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementBindArrayOfRaws
 * --------------------------------------------------------------------------------------------- */

OCI_Bind* OcilibStatementBindArrayOfRaws
(
    OCI_Statement *stmt,
    const otext   *name,
    void          *data,
    OCI_UInt32     len,
    OCI_UInt32     nbelem
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Bind*, NULL,
        /* context */ OCI_IPC_STATEMENT, stmt
    );

    CHECK_BIND(stmt, name, data, OCI_IPC_VOID, FALSE);
    CHECK_MIN(len, 1);

    BIND_DATA(len, OCI_CDT_RAW, SQLT_BIN, 0, NULL, nbelem);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementBindDouble
 * --------------------------------------------------------------------------------------------- */

OCI_Bind* OcilibStatementBindDouble
(
    OCI_Statement *stmt,
    const otext   *name,
    OCI_Double    *data
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Bind*, NULL,
        /* context */ OCI_IPC_STATEMENT, stmt
    );

    OCI_UInt32 code = SQLT_FLT;

    CHECK_BIND(stmt, name, data, OCI_IPC_DOUBLE, FALSE);

#if OCI_VERSION_COMPILE >= OCI_10_1

    if (OcilibConnectionIsVersionSupported(stmt->con, OCI_10_1))
    {
        code = SQLT_BDOUBLE;
    }

#endif

    BIND_DATA(sizeof(OCI_Double), OCI_CDT_NUMERIC, code, OCI_NUM_DOUBLE, NULL, 0);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementBindArrayOfDoubles
 * --------------------------------------------------------------------------------------------- */

OCI_Bind* OcilibStatementBindArrayOfDoubles
(
    OCI_Statement *stmt,
    const otext   *name,
    OCI_Double    *data,
    OCI_UInt32     nbelem
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Bind*, NULL,
        /* context */ OCI_IPC_STATEMENT, stmt
    );

    OCI_UInt32 code = SQLT_FLT;

    CHECK_BIND(stmt, name, data, OCI_IPC_DOUBLE, FALSE);

#if OCI_VERSION_COMPILE >= OCI_10_1

    if (OcilibConnectionIsVersionSupported(stmt->con, OCI_10_1))
    {
        code = SQLT_BDOUBLE;
    }

#endif

    BIND_DATA(sizeof(OCI_Double), OCI_CDT_NUMERIC, code, OCI_NUM_DOUBLE, NULL, nbelem);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementBindFloat
 * --------------------------------------------------------------------------------------------- */

OCI_Bind* OcilibStatementBindFloat
(
    OCI_Statement *stmt,
    const otext   *name,
    OCI_Float     *data
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Bind*, NULL,
        /* context */ OCI_IPC_STATEMENT, stmt
    );

    OCI_UInt32 code = SQLT_FLT;

    CHECK_BIND(stmt, name, data, OCI_IPC_FLOAT, FALSE);

#if OCI_VERSION_COMPILE >= OCI_10_1

    if (OcilibConnectionIsVersionSupported(stmt->con, OCI_10_1))
    {
        code = SQLT_BFLOAT;
    }

#endif

    BIND_DATA(sizeof(OCI_Float), OCI_CDT_NUMERIC, code, OCI_NUM_FLOAT, NULL, 0);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementBindArrayOfFloats
 * --------------------------------------------------------------------------------------------- */

OCI_Bind* OcilibStatementBindArrayOfFloats
(
    OCI_Statement *stmt,
    const otext   *name,
    OCI_Float     *data,
    OCI_UInt32     nbelem
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Bind*, NULL,
        /* context */ OCI_IPC_STATEMENT, stmt
    );

    OCI_UInt32 code = SQLT_FLT;

    CHECK_BIND(stmt, name, data, OCI_IPC_FLOAT, FALSE);

#if OCI_VERSION_COMPILE >= OCI_10_1

    if (OcilibConnectionIsVersionSupported(stmt->con, OCI_10_1))
    {
        code = SQLT_BFLOAT;
    }

#endif

    BIND_DATA(sizeof(OCI_Float), OCI_CDT_NUMERIC, code, OCI_NUM_FLOAT, NULL, nbelem);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementBindDate
 * --------------------------------------------------------------------------------------------- */

OCI_Bind* OcilibStatementBindDate
(
    OCI_Statement *stmt,
    const otext   *name,
    OCI_Date      *data
)
{
    BIND_CALL_NULL_ALLOWED
    (
        OCI_IPC_DATE, sizeof(OCIDate), OCI_CDT_DATETIME, SQLT_ODT, 0, NULL, 0
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementBindArrayOfDates
 * --------------------------------------------------------------------------------------------- */

OCI_Bind* OcilibStatementBindArrayOfDates
(
    OCI_Statement *stmt,
    const otext   *name,
    OCI_Date     **data,
    OCI_UInt32     nbelem
)
{
    BIND_CALL_NULL_ALLOWED
    (
        OCI_IPC_DATE, sizeof(OCIDate), OCI_CDT_DATETIME, SQLT_ODT, 0, NULL, nbelem
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementBindTimestamp
 * --------------------------------------------------------------------------------------------- */

OCI_Bind* OcilibStatementBindTimestamp
(
    OCI_Statement *stmt,
    const otext   *name,
    OCI_Timestamp *data
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Bind*, NULL,
        /* context */ OCI_IPC_STATEMENT, stmt
    );

    CHECK_BIND(stmt, name, data, OCI_IPC_TIMESTAMP, TRUE);
    CHECK_TIMESTAMP_ENABLED();

#if OCI_VERSION_COMPILE >= OCI_9_0

    BIND_DATA(sizeof(OCIDateTime *), OCI_CDT_TIMESTAMP,
              OcilibExternalSubTypeToSQLType(OCI_CDT_TIMESTAMP, data->type),
              data->type, NULL, 0);

#endif

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementBindArrayOfTimestamps
 * --------------------------------------------------------------------------------------------- */

OCI_Bind* OcilibStatementBindArrayOfTimestamps
(
    OCI_Statement    *stmt,
    const otext      *name,
    OCI_Timestamp   **data,
    OCI_TimestampType type,
    OCI_UInt32        nbelem
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Bind*, NULL,
        /* context */ OCI_IPC_STATEMENT, stmt
    );

    CHECK_BIND(stmt, name, data, OCI_IPC_TIMESTAMP, FALSE);
    CHECK_TIMESTAMP_ENABLED();

#if OCI_VERSION_COMPILE >= OCI_9_0

    CHECK_ENUM_VALUE(type, TimestampTypeValues, OTEXT("Timestamp type"));

    BIND_DATA(sizeof(OCIDateTime *), OCI_CDT_TIMESTAMP,
              OcilibExternalSubTypeToSQLType(OCI_CDT_TIMESTAMP, type),
              type, NULL, nbelem);

#else

    OCI_NOT_USED(name);
    OCI_NOT_USED(type);
    OCI_NOT_USED(nbelem);

#endif

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementBindInterval
 * --------------------------------------------------------------------------------------------- */

OCI_Bind* OcilibStatementBindInterval
(
    OCI_Statement *stmt,
    const otext   *name,
    OCI_Interval  *data
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Bind*, NULL,
        /* context */ OCI_IPC_STATEMENT, stmt
    );

    CHECK_BIND(stmt, name, data, OCI_IPC_INTERVAL, TRUE);
    CHECK_INTERVAL_ENABLED();

#if OCI_VERSION_COMPILE >= OCI_9_0

    BIND_DATA(sizeof(OCIInterval *), OCI_CDT_INTERVAL,
              OcilibExternalSubTypeToSQLType(OCI_CDT_INTERVAL, data->type),
              data->type, NULL, 0);

#else

    OCI_NOT_USED(name);

#endif

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementBindArrayOfIntervals
 * --------------------------------------------------------------------------------------------- */

OCI_Bind* OcilibStatementBindArrayOfIntervals
(
    OCI_Statement   *stmt,
    const otext     *name,
    OCI_Interval   **data,
    OCI_IntervalType type,
    OCI_UInt32       nbelem
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Bind*, NULL,
        /* context */ OCI_IPC_STATEMENT, stmt
    );

    CHECK_BIND(stmt, name, data, OCI_IPC_INTERVAL, FALSE);
    CHECK_INTERVAL_ENABLED();

#if OCI_VERSION_COMPILE >= OCI_9_0

    CHECK_ENUM_VALUE(type, IntervalTypeValues, OTEXT("Interval type"));

    BIND_DATA(sizeof(OCIInterval *), OCI_CDT_INTERVAL,
              OcilibExternalSubTypeToSQLType(OCI_CDT_INTERVAL, type),
              type, NULL, nbelem);

#else

    OCI_NOT_USED(name);
    OCI_NOT_USED(type);
    OCI_NOT_USED(nbelem);

#endif

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementBindObject
 * --------------------------------------------------------------------------------------------- */

OCI_Bind* OcilibStatementBindObject
(
    OCI_Statement *stmt,
    const otext   *name,
    OCI_Object    *data
)
{
    BIND_CALL_NULL_FORBIDDEN
    (
        OCI_IPC_OBJECT, sizeof(void *), OCI_CDT_OBJECT, SQLT_NTY, 0, data->typinf, 0
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementBindArrayOfObjects
 * --------------------------------------------------------------------------------------------- */

OCI_Bind* OcilibStatementBindArrayOfObjects
(
    OCI_Statement *stmt,
    const otext   *name,
    OCI_Object   **data,
    OCI_TypeInfo  *typinf,
    OCI_UInt32     nbelem
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Bind*, NULL,
        /* context */ OCI_IPC_STATEMENT, stmt
    );

    CHECK_BIND(stmt, name, data, OCI_IPC_OBJECT, FALSE);
    CHECK_PTR(OCI_IPC_TYPE_INFO, typinf);

    BIND_DATA(sizeof(void *), OCI_CDT_OBJECT, SQLT_NTY, 0, typinf, nbelem);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementBindVector
 * --------------------------------------------------------------------------------------------- */

OCI_Bind* OcilibStatementBindVector
(
    OCI_Statement *stmt,
    const otext   *name,
    OCI_Vector    *data
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Bind*, NULL,
        /* context */ OCI_IPC_STATEMENT, stmt
    );

    CHECK_BIND(stmt, name, data, OCI_IPC_VECTOR, TRUE);
    CHECK_VECTOR_ENABLED(stmt->con);

#if OCI_VERSION_COMPILE >= OCI_23_4

    BIND_DATA(sizeof(OCIVector*), OCI_CDT_VECTOR, SQLT_VEC, 0, NULL, 0);

#else

    OCI_NOT_USED(name);
    OCI_NOT_USED(data);

#endif

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementBindArrayOfVectors
 * --------------------------------------------------------------------------------------------- */

OCI_Bind* OcilibStatementBindArrayOfVectors
(
    OCI_Statement *stmt,
    const otext   *name,
    OCI_Vector   **data,
    OCI_UInt32     nbelem
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Bind*, NULL,
        /* context */ OCI_IPC_STATEMENT, stmt
    );

    CHECK_BIND(stmt, name, data, OCI_IPC_VECTOR, TRUE);
    CHECK_VECTOR_ENABLED(stmt->con);

#if OCI_VERSION_COMPILE >= OCI_23_4

    BIND_DATA(sizeof(OCIVector*), OCI_CDT_VECTOR, SQLT_VEC, 0, NULL, nbelem);

#else

    OCI_NOT_USED(name);
    OCI_NOT_USED(data);
    OCI_NOT_USED(nbelem);

#endif

    EXIT_FUNC();

}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementBindLob
 * --------------------------------------------------------------------------------------------- */

OCI_Bind* OcilibStatementBindLob
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
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementBindJson
 * --------------------------------------------------------------------------------------------- */

OCI_Bind* OcilibStatementBindJson
(
    OCI_Statement *stmt,
    const otext   *name,
    OCI_Json      *data
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Bind*, NULL,
        /* context */ OCI_IPC_STATEMENT, stmt
    );

    CHECK_BIND(stmt, name, data, OCI_IPC_JSON, TRUE);
    CHECK_FEATURE(stmt->con, OCI_FEATURE_JSON, OCI_21_1);

#if OCI_VERSION_COMPILE >= OCI_21_1

    BIND_DATA(sizeof(OCIJson*), OCI_CDT_JSON, SQLT_JSON, 0, NULL, 0);

#endif

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementBindArrayOfJsons
 * --------------------------------------------------------------------------------------------- */

OCI_Bind* OcilibStatementBindArrayOfJsons
(
    OCI_Statement *stmt,
    const otext   *name,
    OCI_Json     **data,
    OCI_UInt32     nbelem
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Bind*, NULL,
        /* context */ OCI_IPC_STATEMENT, stmt
    );

    CHECK_BIND(stmt, name, data, OCI_IPC_JSON, FALSE);
    CHECK_FEATURE(stmt->con, OCI_FEATURE_JSON, OCI_21_1);

#if OCI_VERSION_COMPILE >= OCI_21_1

    BIND_DATA(sizeof(OCIJson*), OCI_CDT_JSON, SQLT_JSON, 0, NULL, nbelem);

#else

    OCI_NOT_USED(nbelem);

#endif

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementBindArrayOfLobs
 * --------------------------------------------------------------------------------------------- */

OCI_Bind* OcilibStatementBindArrayOfLobs
(
    OCI_Statement *stmt,
    const otext   *name,
    OCI_Lob      **data,
    OCI_LobType    type,
    OCI_UInt32     nbelem
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Bind*, NULL,
        /* context */ OCI_IPC_STATEMENT, stmt
    );

    CHECK_BIND(stmt, name, data, OCI_IPC_LOB, FALSE);

    CHECK_ENUM_VALUE(type, LobTypeValues, OTEXT("Lob type"));

    BIND_DATA(sizeof(OCILobLocator*), OCI_CDT_LOB,
              OcilibExternalSubTypeToSQLType(OCI_CDT_LOB, type),
              type, NULL, nbelem);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementBindFile
 * --------------------------------------------------------------------------------------------- */

OCI_Bind* OcilibStatementBindFile
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
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementBindArrayOfFiles
 * --------------------------------------------------------------------------------------------- */

OCI_Bind* OcilibStatementBindArrayOfFiles
(
    OCI_Statement *stmt,
    const otext   *name,
    OCI_File     **data,
    OCI_FileType   type,
    OCI_UInt32     nbelem
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Bind*, NULL,
        /* context */ OCI_IPC_STATEMENT, stmt
    );

    CHECK_BIND(stmt, name, data, OCI_IPC_LOB, OCI_IPC_FILE);
    CHECK_ENUM_VALUE(type, FileTypeValues, OTEXT("File type"));

    BIND_DATA(sizeof(OCILobLocator*), OCI_CDT_FILE,
              OcilibExternalSubTypeToSQLType(OCI_CDT_FILE, type),
              type, NULL, nbelem);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementBindReference
 * --------------------------------------------------------------------------------------------- */

OCI_Bind* OcilibStatementBindReference
(
    OCI_Statement *stmt,
    const otext   *name,
    OCI_Ref       *data
)
{
    BIND_CALL_NULL_FORBIDDEN
    (
        OCI_IPC_REF, sizeof(OCIRef *), OCI_CDT_REF, SQLT_REF, 0, data->typinf, 0
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementBindArrayOfReferences
 * --------------------------------------------------------------------------------------------- */

OCI_Bind* OcilibStatementBindArrayOfReferences
(
    OCI_Statement *stmt,
    const otext   *name,
    OCI_Ref      **data,
    OCI_TypeInfo  *typinf,
    OCI_UInt32     nbelem
)
{
    BIND_CALL_NULL_FORBIDDEN
    (
        OCI_IPC_REF, sizeof(OCIRef *), OCI_CDT_REF, SQLT_REF, 0, typinf, nbelem
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementBindCollection
 * --------------------------------------------------------------------------------------------- */

OCI_Bind* OcilibStatementBindCollection
(
    OCI_Statement *stmt,
    const otext   *name,
    OCI_Coll      *data
)
{
    BIND_CALL_NULL_FORBIDDEN
    (
        OCI_IPC_COLLECTION, sizeof(OCIColl*), OCI_CDT_COLLECTION, SQLT_NTY, 0, data->typinf, 0
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementBindArrayOfCollections
 * --------------------------------------------------------------------------------------------- */

OCI_Bind* OcilibStatementBindArrayOfCollections
(
    OCI_Statement *stmt,
    const otext   *name,
    OCI_Coll     **data,
    OCI_TypeInfo  *typinf,
    OCI_UInt32     nbelem
)
{
    BIND_CALL_NULL_FORBIDDEN
    (
        OCI_IPC_COLLECTION, sizeof(OCIColl*), OCI_CDT_COLLECTION, SQLT_NTY, 0, typinf, nbelem
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementBindStatement
 * --------------------------------------------------------------------------------------------- */

OCI_Bind* OcilibStatementBindStatement
(
    OCI_Statement *stmt,
    const otext   *name,
    OCI_Statement *data
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Bind*, NULL,
        /* context */ OCI_IPC_STATEMENT, stmt
    );

    CHECK_BIND(stmt, name, data, OCI_IPC_STATEMENT, TRUE);

    BIND_DATA(sizeof(OCIStmt*), OCI_CDT_CURSOR, SQLT_RSET, 0, NULL, 0);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementBindLong
 * --------------------------------------------------------------------------------------------- */

OCI_Bind* OcilibStatementBindLong
(
    OCI_Statement *stmt,
    const otext   *name,
    OCI_Long      *data,
    OCI_UInt32     size
)
{
    BIND_CALL_NULL_FORBIDDEN
    (
        OCI_IPC_LONG,
        size, OCI_CDT_LONG,
        OcilibExternalSubTypeToSQLType(OCI_CDT_LONG, data->type),
        data->type, NULL, 0
    );
}

/* --------------------------------------------------------------------------------------------- *
* OcilibStatementRegisterNumber
* --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibStatementRegisterNumber
(
    OCI_Statement *stmt,
    const otext   *name
)
{
    REGISTER_CALL(sizeof(OCINumber), OCI_CDT_NUMERIC, SQLT_VNU, OCI_NUM_NUMBER, NULL, 0);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementRegisterShort
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibStatementRegisterShort
(
    OCI_Statement *stmt,
    const otext   *name
)
{
    REGISTER_CALL(sizeof(OCINumber), OCI_CDT_NUMERIC, SQLT_VNU, OCI_NUM_SHORT, NULL, 0);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementRegisterUnsignedShort
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibStatementRegisterUnsignedShort
(
    OCI_Statement *stmt,
    const otext   *name
)
{
    REGISTER_CALL(sizeof(OCINumber), OCI_CDT_NUMERIC, SQLT_VNU, OCI_NUM_USHORT, NULL, 0);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementRegisterInt
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibStatementRegisterInt
(
    OCI_Statement *stmt,
    const otext   *name
)
{
    REGISTER_CALL(sizeof(OCINumber), OCI_CDT_NUMERIC, SQLT_VNU, OCI_NUM_INT, NULL, 0);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementRegisterUnsignedInt
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibStatementRegisterUnsignedInt
(
    OCI_Statement *stmt,
    const otext   *name
)
{
    REGISTER_CALL(sizeof(OCINumber), OCI_CDT_NUMERIC, SQLT_VNU, OCI_NUM_UINT, NULL, 0);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementRegisterBigInt
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibStatementRegisterBigInt
(
    OCI_Statement *stmt,
    const otext   *name
)
{
    REGISTER_CALL(sizeof(OCINumber), OCI_CDT_NUMERIC, SQLT_VNU, OCI_NUM_BIGINT, NULL, 0);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementRegisterUnsignedBigInt
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibStatementRegisterUnsignedBigInt
(
    OCI_Statement *stmt,
    const otext   *name
)
{
    REGISTER_CALL(sizeof(OCINumber), OCI_CDT_NUMERIC, SQLT_VNU, OCI_NUM_BIGUINT, NULL, 0);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementRegisterString
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibStatementRegisterString
(
    OCI_Statement *stmt,
    const otext   *name,
    OCI_UInt32     len
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    );

    CHECK_REGISTER(stmt, name);
    CHECK_MIN(len, 1);

    const OCI_Int32 size = (OCI_Int32)((len + 1) * sizeof(dbtext));

    REGISTER_DATA((OCI_UInt32) size, OCI_CDT_TEXT, SQLT_STR, 0, NULL, 0);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementRegisterRaw
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibStatementRegisterRaw
(
    OCI_Statement *stmt,
    const otext   *name,
    OCI_UInt32     len
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    );

    CHECK_REGISTER(stmt, name);
    CHECK_MIN(len, 1);

    REGISTER_DATA(len, OCI_CDT_RAW, SQLT_BIN, 0, NULL, 0);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementRegisterDouble
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibStatementRegisterDouble
(
    OCI_Statement *stmt,
    const otext   *name
)
{
    REGISTER_CALL(sizeof(OCINumber), OCI_CDT_NUMERIC, SQLT_VNU, OCI_NUM_DOUBLE, NULL, 0);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementRegisterFloat
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibStatementRegisterFloat
(
    OCI_Statement *stmt,
    const otext   *name
)
{
    REGISTER_CALL(sizeof(OCINumber), OCI_CDT_NUMERIC, SQLT_VNU, OCI_NUM_FLOAT, NULL, 0);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementRegisterDate
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibStatementRegisterDate
(
    OCI_Statement *stmt,
    const otext   *name
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    );

    OCI_UInt32 code = SQLT_ODT;
    OCI_UInt32 size = sizeof(OCIDate);

    CHECK_REGISTER(stmt, name);

    /* versions of OCI (< 10.2) crashes if SQLT_ODT is passed for output
       data with returning clause.
       It's an Oracle known bug #3269146 */

    if (OcilibConnectionGetVersion(stmt->con) < OCI_10_2)
    {
        code = SQLT_DAT;
        size = 7;
    }

    REGISTER_DATA(size, OCI_CDT_DATETIME, code, 0, NULL, 0);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementRegisterTimestamp
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibStatementRegisterTimestamp
(
    OCI_Statement    *stmt,
    const otext      *name,
    OCI_TimestampType type
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    );

    CHECK_REGISTER(stmt, name);
    CHECK_TIMESTAMP_ENABLED();

#if OCI_VERSION_COMPILE >= OCI_9_0

    CHECK_ENUM_VALUE(type, TimestampTypeValues, OTEXT("Timestamp type"));

    REGISTER_DATA(sizeof(OCIDateTime *), OCI_CDT_TIMESTAMP,
                  OcilibExternalSubTypeToSQLType(OCI_CDT_TIMESTAMP, type),
                  type, NULL, 0);

#endif

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementRegisterInterval
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibStatementRegisterInterval
(
    OCI_Statement   *stmt,
    const otext     *name,
    OCI_IntervalType type
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    );

    CHECK_REGISTER(stmt, name);
    CHECK_INTERVAL_ENABLED();

#if OCI_VERSION_COMPILE >= OCI_9_0

    CHECK_ENUM_VALUE(type, IntervalTypeValues, OTEXT("Interval type"));

    REGISTER_DATA(sizeof(OCIInterval *), OCI_CDT_INTERVAL,
                  OcilibExternalSubTypeToSQLType(OCI_CDT_INTERVAL, type),
                  type, NULL, 0);

#endif

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementRegisterObject
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibStatementRegisterObject
(
    OCI_Statement *stmt,
    const otext   *name,
    OCI_TypeInfo  *typinf
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    );

    CHECK_REGISTER(stmt, name);
    CHECK_PTR(OCI_IPC_TYPE_INFO, typinf);

    REGISTER_DATA(sizeof(void *), OCI_CDT_OBJECT, SQLT_NTY, 0, typinf, 0);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementRegisterLob
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibStatementRegisterLob
(
    OCI_Statement *stmt,
    const otext   *name,
    OCI_LobType    type
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    );

    CHECK_REGISTER(stmt, name);
    CHECK_ENUM_VALUE(type, LobTypeValues, OTEXT("Lob type"));

    REGISTER_DATA(sizeof(OCILobLocator*), OCI_CDT_LOB,
                  OcilibExternalSubTypeToSQLType(OCI_CDT_LOB, type),
                  type, NULL, 0);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementRegisterFile
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibStatementRegisterFile
(
    OCI_Statement *stmt,
    const otext   *name,
    OCI_FileType   type
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    );

    CHECK_REGISTER(stmt, name);
    CHECK_ENUM_VALUE(type, FileTypeValues, OTEXT("File type"));

    REGISTER_DATA(sizeof(OCILobLocator*), OCI_CDT_FILE,
                  OcilibExternalSubTypeToSQLType(OCI_CDT_FILE, type),
                  type, NULL, 0);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementRegisterReference
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibStatementRegisterReference
(
    OCI_Statement *stmt,
    const otext   *name,
    OCI_TypeInfo  *typinf
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    );

    CHECK_REGISTER(stmt, name);
    CHECK_PTR(OCI_IPC_TYPE_INFO, typinf);

    REGISTER_DATA(sizeof(OCIRef *), OCI_CDT_REF, SQLT_REF, 0, typinf, 0);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementRegisterVector
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibStatementRegisterVector
(
    OCI_Statement* stmt,
    const otext  * name
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    );

    CHECK_REGISTER(stmt, name);
    CHECK_VECTOR_ENABLED(stmt->con);

#if OCI_VERSION_COMPILE >= OCI_23_4

    REGISTER_DATA(sizeof(OCIVector *), OCI_CDT_VECTOR, SQLT_VEC, 0, NULL, 0);

#endif

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementRegisterJson
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibStatementRegisterJson
(
    OCI_Statement *stmt,
    const otext   *name
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    );

    CHECK_REGISTER(stmt, name);
    CHECK_FEATURE(stmt->con, OCI_FEATURE_JSON, OCI_21_1);

#if OCI_VERSION_COMPILE >= OCI_21_1

    REGISTER_DATA(sizeof(OCIJson *), OCI_CDT_JSON, SQLT_JSON, 0, NULL, 0);

#endif

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementGetStatementType
 * --------------------------------------------------------------------------------------------- */

OCI_StatementType OcilibStatementGetStatementType
(
    OCI_Statement *stmt
)
{
    GET_PROP
    (
        OCI_StatementType, OCI_UNKNOWN,
        OCI_IPC_STATEMENT, stmt,
        type
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementSetFetchMode
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibStatementSetFetchMode
(
    OCI_Statement *stmt,
    OCI_FetchMode  mode
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    );
    OCI_FetchMode old_exec_mode = OCI_UNKNOWN;

    CHECK_PTR(OCI_IPC_STATEMENT, stmt);
    CHECK_SCROLLABLE_CURSOR_ENABLED(stmt->con);
    CHECK_ENUM_VALUE(mode, FetchModeValues, OTEXT("Fetch mode"));

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

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementGetFetchMode
 * --------------------------------------------------------------------------------------------- */

OCI_FetchMode OcilibStatementGetFetchMode
(
    OCI_Statement *stmt
)
{
    GET_PROP
    (
        OCI_FetchMode, OCI_UNKNOWN,
        OCI_IPC_STATEMENT, stmt,
        exec_mode
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementSetBindMode
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibStatementSetBindMode
(
    OCI_Statement *stmt,
    OCI_BindMode   mode
)
{
    SET_PROP_ENUM
    (
        /* handle */ OCI_IPC_STATEMENT, stmt,
        /* member */ bind_mode, ub1,
        /* value  */ mode, BindModeValues, OTEXT("Bind Mode")
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementGetBindMode
 * --------------------------------------------------------------------------------------------- */

OCI_BindMode OcilibStatementGetBindMode
(
    OCI_Statement *stmt
)
{
    GET_PROP
    (
        OCI_BindMode, OCI_UNKNOWN,
        OCI_IPC_STATEMENT, stmt,
        bind_mode
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementSetBindAllocation
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibStatementSetBindAllocation
(
    OCI_Statement         *stmt,
    OCI_BindAllocationMode mode
)
{
    SET_PROP_ENUM
    (
        /* handle */ OCI_IPC_STATEMENT, stmt,
        /* member */ bind_alloc_mode, ub1,
        /* value  */ mode, BindAllocationValues, OTEXT("Bind Allocation")
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementGetBindAllocation
 * --------------------------------------------------------------------------------------------- */

OCI_BindAllocationMode OcilibStatementGetBindAllocation
(
    OCI_Statement *stmt
)
{
    GET_PROP
    (
        OCI_BindAllocationMode, OCI_UNKNOWN,
        OCI_IPC_STATEMENT, stmt,
        bind_alloc_mode
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementSetFetchSize
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibStatementSetFetchSize
(
    OCI_Statement *stmt,
    OCI_UInt32     size
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    );

    CHECK_PTR(OCI_IPC_STATEMENT, stmt);
    CHECK_MIN(size, 1);

    stmt->fetch_size = size;

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementGetFetchSize
 * --------------------------------------------------------------------------------------------- */

OCI_UInt32 OcilibStatementGetFetchSize
(
    OCI_Statement *stmt
)
{
    GET_PROP
    (
        OCI_UInt32, 0,
        OCI_IPC_STATEMENT, stmt,
        fetch_size
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementPrefetchSize
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibStatementSetPrefetchSize
(
    OCI_Statement *stmt,
    OCI_UInt32     size
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    );

    CHECK_PTR(OCI_IPC_STATEMENT, stmt);

    stmt->prefetch_size = size;

    /* Prefetch is not working with scrollable cursors in Oracle 9iR1, thus disable it */

    if (stmt->exec_mode == OCI_SFM_SCROLLABLE && stmt->con->ver_num == OCI_9_0)
    {
        stmt->prefetch_size = 0;
    }

    if (stmt->handle)
    {
        CHECK_ATTRIB_SET
        (
            OCI_HTYPE_STMT, OCI_ATTR_PREFETCH_ROWS,
            stmt->handle, &stmt->prefetch_size, sizeof(stmt->prefetch_size),
            stmt->con->err
        );
    }

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementGetPrefetchSize
 * --------------------------------------------------------------------------------------------- */

OCI_UInt32 OcilibStatementGetPrefetchSize
(
    OCI_Statement *stmt
)
{
    GET_PROP
    (
        OCI_UInt32, 0,
        OCI_IPC_STATEMENT, stmt,
        prefetch_size
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementSetPrefetchMemory
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibStatementSetPrefetchMemory
(
    OCI_Statement *stmt,
    OCI_UInt32     size
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    );

    CHECK_PTR(OCI_IPC_STATEMENT, stmt);

    stmt->prefetch_mem = size;

    if (stmt->handle)
    {
        CHECK_ATTRIB_SET
        (
            OCI_HTYPE_STMT, OCI_ATTR_PREFETCH_MEMORY,
            stmt->handle, &stmt->prefetch_mem, sizeof(stmt->prefetch_mem),
            stmt->con->err
        );
    }

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementGetPrefetchMemory
 * --------------------------------------------------------------------------------------------- */

OCI_UInt32 OcilibStatementGetPrefetchMemory
(
    OCI_Statement *stmt
)
{
    GET_PROP
    (
        OCI_UInt32, 0,
        OCI_IPC_STATEMENT, stmt,
        prefetch_mem
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementSetPieceSize
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibStatementSetPieceSize
(
    OCI_Statement *stmt,
    OCI_UInt32     size
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    );

    CHECK_PTR(OCI_IPC_STATEMENT, stmt);
    CHECK_MIN(size, 1);

    stmt->piece_size = size;

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementGetPieceSize
 * --------------------------------------------------------------------------------------------- */

OCI_UInt32 OcilibStatementGetPieceSize
(
    OCI_Statement *stmt
)
{
    GET_PROP
    (
        OCI_UInt32, 0,
        OCI_IPC_STATEMENT, stmt,
        piece_size
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementSetLongMode
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibStatementSetLongMode
(
    OCI_Statement *stmt,
    OCI_LongMode   mode
)
{
    SET_PROP_ENUM
    (
        /* handle */ OCI_IPC_STATEMENT, stmt,
        /* member */ long_mode, ub1,
        /* value  */ mode, LongModeValues, OTEXT("Long Mode")
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementGetLongMode
 * --------------------------------------------------------------------------------------------- */

OCI_LongMode OcilibStatementGetLongMode
(
    OCI_Statement *stmt
)
{
    GET_PROP
    (
        OCI_LongMode, OCI_UNKNOWN,
        OCI_IPC_STATEMENT, stmt,
        long_mode
    );
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
    );
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
    );
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
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementGetSqlErrorPos
 * --------------------------------------------------------------------------------------------- */

OCI_UInt32 OcilibStatementGetSqlErrorPos
(
    OCI_Statement *stmt
)
{
    GET_PROP
    (
        OCI_UInt32, 0,
        OCI_IPC_STATEMENT, stmt,
        err_pos
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementGetAffectedRows
 * --------------------------------------------------------------------------------------------- */

OCI_UInt32 OcilibStatementGetAffectedRows
(
    OCI_Statement *stmt
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_UInt32, 0,
        /* context */ OCI_IPC_STATEMENT, stmt
    );

    ub4 count = 0;

    CHECK_PTR(OCI_IPC_STATEMENT, stmt);

    CHECK_ATTRIB_GET
    (
        OCI_HTYPE_STMT, OCI_ATTR_ROW_COUNT,
        stmt->handle, &count, NULL,
        stmt->con->err
    );

    SET_RETVAL((OCI_UInt32) count);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementGetBindCount
 * --------------------------------------------------------------------------------------------- */

OCI_UInt32 OcilibStatementGetBindCount
(
    OCI_Statement *stmt
)
{
    GET_PROP
    (
        OCI_UInt32, 0,
        OCI_IPC_STATEMENT, stmt,
        nb_ubinds
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementGetBind
 * --------------------------------------------------------------------------------------------- */

OCI_Bind * OcilibStatementGetBind
(
    OCI_Statement *stmt,
    OCI_UInt32     index
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Bind*, NULL,
        /* context */ OCI_IPC_STATEMENT, stmt
    );

    CHECK_PTR(OCI_IPC_STATEMENT, stmt);
    CHECK_INDEX(index, stmt->nb_ubinds);

    SET_RETVAL(stmt->ubinds[index]);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementGetBindByName
 * --------------------------------------------------------------------------------------------- */

OCI_Bind * OcilibStatementGetBindByName
(
    OCI_Statement *stmt,
    const otext   *name
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Bind*, NULL,
        /* context */ OCI_IPC_STATEMENT, stmt
    );

    OCI_Int32 index = -1;

    CHECK_PTR(OCI_IPC_STATEMENT, stmt);
    CHECK_PTR(OCI_IPC_STRING,    name);

    index = OcilibBindGetIndex(stmt, name);
    if (index < 0)
    {
        THROW(OcilibExceptionItemNotFound, name, OCI_IPC_BIND);
    }

    SET_RETVAL(stmt->ubinds[index]);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
* OcilibStatementGetBindIndex
* --------------------------------------------------------------------------------------------- */

OCI_Int32 OcilibStatementGetBindIndex
(
    OCI_Statement *stmt,
    const otext   *name
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Int32, -1,
        /* context */ OCI_IPC_STATEMENT, stmt
    );

    OCI_Int32 index = -1;

    CHECK_PTR(OCI_IPC_STATEMENT, stmt);
    CHECK_PTR(OCI_IPC_STRING,    name);

    index = OcilibBindGetIndex(stmt, name);

    SET_RETVAL(index);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementGetSqlCommand
 * --------------------------------------------------------------------------------------------- */

/* OCI_ATTR_SQLFNCODE returns Oracle OCI SQL function codes, whose numbering
   differs from the OCI_SFC_* public constants (based on V$SESSION.COMMAND).
   This table maps each OCI function code (index) to its OCI_SFC_* value.
   Entries without an OCI_SFC_* equivalent are set to OCI_UNKNOWN.           */

static const ub2 OcilibSqlFunctionCodeMap[100] =
{
    /*   0             */ OCI_UNKNOWN,
    /*   1             */ OCI_SFC_CREATE_TABLE,
    /*   2 SET ROLE    */ OCI_SFC_SET_ROLE,
    /*   3             */ OCI_SFC_INSERT,
    /*   4             */ OCI_SFC_SELECT,
    /*   5             */ OCI_SFC_UPDATE,
    /*   6             */ OCI_SFC_DROP_ROLE,
    /*   7             */ OCI_SFC_DROP_VIEW,
    /*   8             */ OCI_SFC_DROP_TABLE,
    /*   9             */ OCI_SFC_DELETE,
    /*  10             */ OCI_SFC_CREATE_VIEW,
    /*  11             */ OCI_SFC_DROP_USER,
    /*  12             */ OCI_SFC_CREATE_ROLE,
    /*  13             */ OCI_SFC_CREATE_SEQUENCE,
    /*  14             */ OCI_SFC_ALTER_SEQUENCE,
    /*  15 (not used)  */ OCI_UNKNOWN,
    /*  16             */ OCI_SFC_DROP_SEQUENCE,
    /*  17             */ OCI_SFC_CREATE_SCHEMA,
    /*  18             */ OCI_SFC_CREATE_CLUSTER,
    /*  19             */ OCI_SFC_CREATE_USER,
    /*  20             */ OCI_SFC_CREATE_INDEX,
    /*  21             */ OCI_SFC_DROP_INDEX,
    /*  22             */ OCI_SFC_DROP_CLUSTER,
    /*  23             */ OCI_SFC_VALIDATE_INDEX,
    /*  24             */ OCI_SFC_CREATE_PROCEDURE,
    /*  25             */ OCI_SFC_ALTER_PROCEDURE,
    /*  26             */ OCI_SFC_ALTER_TABLE,
    /*  27             */ OCI_SFC_EXPLAIN,
    /*  28             */ OCI_SFC_GRANT_OBJECT,
    /*  29             */ OCI_SFC_REVOKE_OBJECT,
    /*  30             */ OCI_SFC_CREATE_SYNONYM,
    /*  31             */ OCI_SFC_DROP_SYNONYM,
    /*  32             */ OCI_SFC_ALTER_SYSTEM,
    /*  33             */ OCI_SFC_SET_TRANSACTION,
    /*  34             */ OCI_SFC_PL_SQL_EXECUTE,
    /*  35             */ OCI_SFC_LOCK,
    /*  36             */ OCI_SFC_NO_OP,
    /*  37             */ OCI_SFC_RENAME,
    /*  38             */ OCI_SFC_COMMENT,
    /*  39             */ OCI_SFC_AUDIT_OBJECT,
    /*  40             */ OCI_SFC_NOAUDIT_OBJECT,
    /*  41             */ OCI_SFC_ALTER_INDEX,
    /*  42 CREATE EXT. DATABASE */ OCI_UNKNOWN,
    /*  43 DROP EXT. DATABASE   */ OCI_UNKNOWN,
    /*  44             */ OCI_SFC_CREATE_DATABASE,
    /*  45             */ OCI_SFC_ALTER_DATABASE,
    /*  46             */ OCI_SFC_CREATE_ROLLBACK_SEG,
    /*  47             */ OCI_SFC_ALTER_ROLLBACK_SEG,
    /*  48             */ OCI_SFC_DROP_ROLLBACK_SEG,
    /*  49             */ OCI_SFC_CREATE_TABLESPACE,
    /*  50             */ OCI_SFC_ALTER_TABLESPACE,
    /*  51             */ OCI_SFC_DROP_TABLESPACE,
    /*  52             */ OCI_SFC_ALTER_SESSION,
    /*  53             */ OCI_SFC_ALTER_USER,
    /*  54             */ OCI_SFC_COMMIT,
    /*  55             */ OCI_SFC_ROLLBACK,
    /*  56             */ OCI_SFC_SAVEPOINT,
    /*  57             */ OCI_SFC_CREATE_CONTROL_FILE,
    /*  58             */ OCI_SFC_ALTER_TRACING,
    /*  59             */ OCI_SFC_CREATE_TRIGGER,
    /*  60             */ OCI_SFC_ALTER_TRIGGER,
    /*  61             */ OCI_SFC_DROP_TRIGGER,
    /*  62             */ OCI_SFC_ANALYZE_TABLE,
    /*  63             */ OCI_SFC_ANALYZE_INDEX,
    /*  64             */ OCI_SFC_ANALYZE_CLUSTER,
    /*  65             */ OCI_SFC_CREATE_PROFILE,
    /*  66             */ OCI_SFC_DROP_PROFILE,
    /*  67             */ OCI_SFC_ALTER_PROFILE,
    /*  68             */ OCI_SFC_DROP_PROCEDURE,
    /*  69 (not used)  */ OCI_UNKNOWN,
    /*  70             */ OCI_SFC_ALTER_RESOURCE_COST,
    /*  71             */ OCI_SFC_CREATE_MATERIALIZED_VIEW_LOG,
    /*  72             */ OCI_SFC_ALTER_MATERIALIZED_VIEW_LOG,
    /*  73             */ OCI_SFC_DROP_MATERIALIZED_VIEW_LOG,
    /*  74             */ OCI_SFC_CREATE_MATERIALIZED_VIEW,
    /*  75             */ OCI_SFC_ALTER_MATERIALIZED_VIEW,
    /*  76             */ OCI_SFC_DROP_MATERIALIZED_VIEW,
    /*  77             */ OCI_SFC_CREATE_TYPE,
    /*  78             */ OCI_SFC_DROP_TYPE,
    /*  79             */ OCI_SFC_ALTER_ROLE,
    /*  80             */ OCI_SFC_ALTER_TYPE,
    /*  81             */ OCI_SFC_CREATE_TYPE_BODY,
    /*  82             */ OCI_SFC_ALTER_TYPE_BODY,
    /*  83             */ OCI_SFC_DROP_TYPE_BODY,
    /*  84             */ OCI_SFC_DROP_LIBRARY,
    /*  85             */ OCI_SFC_TRUNCATE_TABLE,
    /*  86             */ OCI_SFC_TRUNCATE_CLUSTER,
    /*  87 CREATE BITMAPFILE    */ OCI_UNKNOWN,
    /*  88             */ OCI_SFC_ALTER_VIEW,
    /*  89 DROP BITMAPFILE      */ OCI_UNKNOWN,
    /*  90             */ OCI_SFC_SET_CONSTRAINTS,
    /*  91             */ OCI_SFC_CREATE_FUNCTION,
    /*  92             */ OCI_SFC_ALTER_FUNCTION,
    /*  93             */ OCI_SFC_DROP_FUNCTION,
    /*  94             */ OCI_SFC_CREATE_PACKAGE,
    /*  95             */ OCI_SFC_ALTER_PACKAGE,
    /*  96             */ OCI_SFC_DROP_PACKAGE,
    /*  97             */ OCI_SFC_CREATE_PACKAGE_BODY,
    /*  98             */ OCI_SFC_ALTER_PACKAGE_BODY,
    /*  99             */ OCI_SFC_DROP_PACKAGE_BODY
};

OCI_SQLCommand OcilibStatementGetSqlCommand
(
    OCI_Statement *stmt
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_SQLCommand, OCI_UNKNOWN,
        /* context */ OCI_IPC_STATEMENT, stmt
    );

    ub2 code = OCI_UNKNOWN;

    CHECK_PTR(OCI_IPC_STATEMENT, stmt);
    CHECK_STMT_STATUS(stmt, OCI_STMT_EXECUTED);

    CHECK_ATTRIB_GET
    (
        OCI_HTYPE_STMT, OCI_ATTR_SQLFNCODE,
        stmt->handle, &code, NULL,
        stmt->con->err
    );

    if (code < sizeof(OcilibSqlFunctionCodeMap) / sizeof(OcilibSqlFunctionCodeMap[0]))
    {
        code = OcilibSqlFunctionCodeMap[code];
    }
    else
    {
        code = OCI_UNKNOWN;
    }

    SET_RETVAL(code);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementGetParseBindNames
 * --------------------------------------------------------------------------------------------- */

OCI_SYM_LOCAL OCI_Boolean OcilibStatementGetParseBindNames
(
    OCI_Statement* stmt,
    OCI_UInt32   * count,
    const otext*** names
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    );

    CHECK_PTR(OCI_IPC_STATEMENT, stmt);
    CHECK_PTR(OCI_IPC_VOID, count);
    CHECK_PTR(OCI_IPC_VOID, names);

    *count = 0;
    *names = NULL;

    CHECK(OcilibStatementGetParseBinds(stmt));

    *count = stmt->parse_binds->count;
    *names = (const otext**)stmt->parse_binds->names;

    SET_SUCCESS();

    EXIT_FUNC();
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
    );

    OCI_SQLCommand code = OCI_UNKNOWN;

    CHECK_PTR(OCI_IPC_STATEMENT, stmt);

    code = OcilibStatementGetSqlCommand(stmt);

    const otext* verb = NULL;

    if (OCI_UNKNOWN != code)
    {
        for (OCI_Int32 i = 0; i < OCI_SQLCMD_COUNT; i++)
        {
            if (code == SQLCmds[i].code)
            {
                verb = SQLCmds[i].verb;
                break;
            }
        }
    }

    SET_RETVAL(verb);

    EXIT_FUNC();
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
    );

    CHECK_PTR(OCI_IPC_STATEMENT, stmt);

    CHECK(NULL != stmt->batch && stmt->batch->cur < stmt->batch->count);

    SET_RETVAL(&stmt->batch->errs[stmt->batch->cur++]);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStatementGetBatchErrorCount
 * --------------------------------------------------------------------------------------------- */

OCI_UInt32 OcilibStatementGetBatchErrorCount
(
    OCI_Statement *stmt
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_UInt32, 0,
        /* context */ OCI_IPC_STATEMENT, stmt
    );

    CHECK_PTR(OCI_IPC_STATEMENT, stmt);

    CHECK_NULL(stmt->batch);

    SET_RETVAL(stmt->batch->count);

    EXIT_FUNC();
}
