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
 
#ifndef OCILIB_MACROS_H_INCLUDED
#define OCILIB_MACROS_H_INCLUDED

#include "types.h"
#include "exception.h"
#include "memory.h"
#include "error.h"
#include "environment.h"

/* status management  */

#define RETVAL  call_retval
#define STATUS  ((ctx)->call_status)

/* memory management helpers */

#define FREE(ptr)   { MemoryFree(ptr), (ptr) = NULL; }

/* miscellaneous */

#define NB_ARG_VERSION              3

#define LIB_THREADED                (Env.env_mode & OCI_ENV_THREADED)

#define DECLARE_CTX(status)                                                     \
                                                                                \
    OCI_Context ctx_obj = { NULL, NULL, NULL, NULL, FALSE };                    \
    OCI_Context* ctx = &ctx_obj;                                                \
    ctx_obj.oci_err    = Env.err;                                               \
    ctx_obj.call_status = (status);                                             \

#define DECLARE_VARS(type, value, status)                                       \
                                                                                \
    type call_retval = (type) (value);                                          \
    DECLARE_CTX(status);                                                        \

#define CTX_ENTER(mode)                                                         \
                                                                                \
    if ((mode) & OCI_ENV_CONTEXT)                                               \
    {                                                                           \
        ctx->call_err = ErrorGet(FALSE, FALSE);                                 \
        CallEnter(ctx);                                                         \
    }

#define CTX_EXIT(mode)                                                          \
                                                                                \
    if ((mode) & OCI_ENV_CONTEXT)                                               \
    {                                                                           \
        CallExit(ctx);                                                          \
    }

#define CALL_ENTER(type, value)                                                 \
                                                                                \
    DECLARE_VARS(type, value, TRUE)                                             \
    CTX_ENTER(Env.env_mode)                                                     \

#define CALL_EXIT()                                                             \
                                                                                \
    ExitCall:                                                                   \
    CTX_EXIT(Env.env_mode)                                                      \
    return call_retval;

#define JUMP_EXIT()                                                             \
                                                                                \
    goto ExitCall;                                                              \

#define CTX_SET(c, s, e)                                                        \
                                                                                \
    ctx->lib_con      = (c);                                                    \
    ctx->lib_stmt     = (s);                                                    \
    ctx->oci_err      = (e) ? (e) : Env.err;                                    \

#define CALL_CONTEXT_FROM_CON(con)                                              \
                                                                                \
   CTX_SET((con), NULL, ((con) ? (con)->err : Env.err))

#define CALL_CONTEXT_FROM_OBJ(obj)                                              \
                                                                                \
    CTX_SET(((obj)->con), NULL, ((obj)->err))

#define CALL_CONTEXT_FROM_ERR(err)                                              \
                                                                                \
    CTX_SET(NULL, NULL, (err))

#define CALL_CONTEXT_FROM_STMT(stmt)                                            \
                                                                                \
    CTX_SET(((stmt)->con), (stmt), ((stmt)->con->err))

#define THROW(exp)                                                              \
                                                                                \
    exp;                                                                        \
    STATUS = FALSE;                                                             \
    JUMP_EXIT()                                                                 \

#define IS_PLSQL_STMT(type)                                                     \
                                                                                \
    ((OCI_CST_BEGIN   == (type)) ||                                             \
     (OCI_CST_DECLARE == (type)) ||                                             \
     (OCI_CST_CALL    == (type)))

#define IS_OCI_NUMBER(type, subtype)                                            \
                                                                                \
   (OCI_CDT_NUMERIC == (type) && OCI_NUM_NUMBER == (subtype))

#define IS_OCILIB_OBJECT(type, subtype)                                         \
                                                                                \
   ( (IS_OCI_NUMBER(type, subtype)) ||                                          \
      (OCI_CDT_TEXT    != (type) &&                                             \
       OCI_CDT_RAW     != (type) &&                                             \
       OCI_CDT_BOOLEAN != (type)))

#define GET_PROP(type, val, objtype, obj, prop, con, stmt, err)                 \
                                                                                \
    CALL_ENTER(type, val)                                                       \
    CALL_CHECK_PTR(objtype, obj)                                                \
    CTX_SET((con),(stmt), (err))                                                \
    RETVAL = (type) ((obj)->prop);                                              \
    CALL_EXIT()                                                                 \

#define SET_PROP(type, objtype, obj, prop, val, con, stmt, err)                 \
                                                                                \
    CALL_ENTER(boolean, FALSE)                                                  \
    CALL_CHECK_PTR(objtype, obj)                                                \
    CTX_SET((con),(stmt), (err))                                                \
    (obj)->prop = (type) (val);                                                 \
    RETVAL = STATUS;                                                            \
    CALL_EXIT()                                                                 \

#define SET_PROP_ENUM(type, objtype, obj, prop, val, enms, msg, con, stmt, err) \
                                                                                \
    CALL_ENTER(boolean, FALSE)                                                  \
    CALL_CHECK_PTR(objtype, obj)                                                \
    CTX_SET((con),(stmt), (err))                                                \
    CALL_CHECK_ENUM_VALUE((con),(stmt), (val), (enms), (msg))                   \
    (obj)->prop = (type) (val);                                                 \
    RETVAL = STATUS;                                                            \
    CALL_EXIT()                                                                 \

#define GET_LIB_PROP(type, value, prop)                                         \
                                                                                \
    CALL_ENTER(type, value)                                                     \
    CALL_CHECK_INITIALIZED()                                                    \
    RETVAL = (type) (prop);                                                     \
    CALL_EXIT()                                                                 \

#define SET_LIB_PROP(prop, value)                                               \
                                                                                \
    CALL_ENTER(boolean, FALSE)                                                  \
    CALL_CHECK_INITIALIZED()                                                    \
    (prop) = (value);                                                           \
    RETVAL = STATUS;                                                            \
    CALL_EXIT()                                                                 \

#define ALLOC_BUFFER(type, ptr, size, count)                                    \
                                                                                \
    if (STATUS && !(ptr))                                                       \
    {                                                                           \
        (ptr) = MemoryAlloc(type, size, (size_t) (count), TRUE);                \
                                                                                \
        STATUS = (NULL != (ptr));                                               \
    }                                                                           \

#define REALLOC_BUFFER(type, ptr, size, current, allocated, requested)          \
                                                                                \
    if (STATUS)                                                                 \
    {                                                                           \
        if (!(ptr))                                                             \
        {                                                                       \
            (ptr) = MemoryAlloc(type, size, (size_t) (requested), TRUE);        \
            if (ptr) (allocated) = (requested);                                 \
        }                                                                       \
        else if ((current) >= (allocated))                                      \
        {                                                                       \
            (ptr) = MemoryRealloc(ptr, type, size, (size_t) (requested), TRUE); \
            if (ptr) (allocated) = (requested);                                 \
        }                                                                       \
                                                                                \
        STATUS = (NULL != (ptr));                                               \
    }                                                                           \

#define ALLOC_DATA(type, ptr, count)                                            \
                                                                                \
    ALLOC_BUFFER(type, ptr, sizeof(*(ptr)), count)

#define REALLOC_DATA(type, ptr, cur, alloc, inc)                                \
                                                                                \
    REALLOC_BUFFER(type, ptr, sizeof(*(ptr)), cur, alloc, inc)

#define ARRAY_GET_AT(ptr, size, offset)                                         \
                                                                                \
    (((ub1 *) (ptr)) + (size_t)((size)*i))                                      

#define ARRAY_SET_AT(ptr, type, offset, value)                                  \
                                                                                \
    *(type*)(ARRAY_GET_AT(ptr, sizeof(type), i)) = (type) (value);

#define IS_STRING_VALID(s) ((s) && ((s)[0]))

#define EXEC(fct)                                                               \
                                                                                \
    if (STATUS)                                                                 \
    {                                                                           \
        STATUS = fct;                                                           \
        if (OCI_FAILURE(STATUS))                                                \
        {                                                                       \
            STATUS = (OCI_SUCCESS_WITH_INFO == STATUS);                         \
            ExceptionOCI(ctx->oci_err, ctx->lib_con, ctx->lib_stmt, STATUS);    \
        }                                                                       \
        else                                                                    \
        {                                                                       \
           STATUS = TRUE;                                                       \
        }                                                                       \
    }

#define ATTRIB_GET(htype, atype, handle, value, size)                           \
                                                                                \
    EXEC                                                                        \
    (                                                                           \
        OCIAttrGet((void*) (handle), (ub4) (htype), (void*) (value),            \
                   (ub4*) (size), (ub4) (atype), (ctx)->oci_err)                \
    )                                                                           \

#define ATTRIB_SET(htype, atype, handle, value, size)                           \
                                                                                \
    EXEC                                                                        \
    (                                                                           \
        OCIAttrSet((void*) (handle), (ub4) (htype), (void*) (value),            \
                   (ub4) (size), (ub4) (atype), (ctx)->oci_err)                 \
    )                                                                           \


 /* --------------------------------------------------------------------------------------------- *
  * CHECKING MACROS
  * --------------------------------------------------------------------------------------------- */


#define CHECK(exp, ret) if (exp) return (ret);

#define CALL_CHECK_PTR(type, ptr)                                               \
                                                                                \
    if (!(ptr))                                                                 \
    {                                                                           \
        THROW(ExceptionNullPointer(type))                                       \
    }

#define CALL_CHECK_BIND(stmt, name, data, type, ext_only)                       \
                                                                                \
    CALL_CHECK_PTR(OCI_IPC_STATEMENT, stmt)                                     \
    CALL_CHECK_PTR(OCI_IPC_STRING, name)                                        \
    CALL_CHECK_STMT_STATUS(stmt, OCI_STMT_PREPARED)                             \
    {                                                                           \
        const boolean ext_only_value = (ext_only);                              \
        if ((ext_only_value) &&                                                 \
            (OCI_BAM_INTERNAL == (stmt)->bind_alloc_mode) &&                    \
            ((data) != NULL))                                                   \
        {                                                                       \
            THROW(ExceptionExternalBindingNotAllowed(stmt, name))               \
        }                                                                       \
                                                                                \
        if ((ext_only_value) || OCI_BAM_EXTERNAL == (stmt)->bind_alloc_mode)    \
        {                                                                       \
            CALL_CHECK_PTR(type, data)                                          \
        }                                                                       \
    }                                                                           \

#define CALL_CHECK_REGISTER(stmt, name)                                         \
                                                                                \
    CALL_CHECK_PTR(OCI_IPC_STATEMENT, stmt)                                     \
    CALL_CHECK_PTR(OCI_IPC_STRING, name)                                        \

#define CALL_CHECK_BOUND(con, v, b1, b2)                                        \
                                                                                \
    if (((v) < (b1)) || ((v) > (b2)))                                           \
    {                                                                           \
        THROW(ExceptionOutOfBounds((con), (v)))                                 \
    }

#define CALL_CHECK_MIN(con, stmt, v, m)                                         \
                                                                                \
    if ((v) < (m))                                                              \
    {                                                                           \
        THROW(ExceptionMinimumValue((con), (stmt), (m)))                        \
    }

#define CALL_CHECK_COMPAT(con, exp)                                             \
                                                                                \
    if (!(exp))                                                                 \
    {                                                                           \
        THROW(ExceptionTypeNotCompatible((con)))                                \
    }

#define CALL_CHECK_OBJECT_FETCHED(obj)                                          \
                                                                                \
    if (OCI_OBJECT_FETCHED_CLEAN == (obj)->hstate)                              \
    {                                                                           \
        JUMP_EXIT()                                                             \
    }                                                                           \

#define CALL_CHECK_STMT_STATUS(st, v)                                           \
                                                                                \
    if ((((st)->status) & (v)) == 0)                                            \
    {                                                                           \
        THROW(ExceptionStatementState((st), v))                                 \
    }                                                                           \

#define CALL_CHECK_SCROLLABLE_CURSOR_ACTIVATED(st)                              \
                                                                                \
    if (((st)->nb_rbinds > 0) ||                                                \
        ((st)->exec_mode != OCI_STMT_SCROLLABLE_READONLY))                      \
    {                                                                           \
        THROW(ExceptionStatementNotScrollable(st))                              \
    }

#define CALL_CHECK_DIRPATH_STATUS(dp, v)                                        \
                                                                                \
    if ((dp)->status != (v))                                                    \
    {                                                                           \
        THROW(ExceptionDirPathState((dp), (dp)->status))                        \
    }

#define CALL_CHECK_INITIALIZED()                                                \
                                                                                \
    if (!Env.loaded)                                                            \
    {                                                                           \
        THROW(ExceptionNotInitialized())                                        \
    }

#define CALL_CHECK_FEATURE(con, feat, ver)                                      \
                                                                                \
    if (Env.version_runtime < (ver) || ((con) && (con)->ver_num < (ver)))       \
    {                                                                           \
        THROW(ExceptionNotAvailable(con, feat))                                 \
    }

#define CALL_CHECK_THREAD_ENABLED()                                             \
                                                                                \
    if (!(LIB_THREADED))                                                        \
    {                                                                           \
        THROW(ExceptionNotMultithreaded())                                      \
    }

#define CALL_CHECK_TIMESTAMP_ENABLED(con)                                       \
                                                                                \
    CALL_CHECK_FEATURE(con, OCI_FEATURE_TIMESTAMP, OCI_9_0)  

#define CALL_CHECK_INTERVAL_ENABLED(con)                                        \
                                                                                \
    CALL_CHECK_FEATURE(con, OCI_FEATURE_TIMESTAMP, OCI_9_0)  

#define CALL_CHECK_SCROLLABLE_CURSOR_ENABLED(con)                               \
                                                                                \
    CALL_CHECK_FEATURE(con, OCI_FEATURE_SCROLLABLE_CURSOR, OCI_9_0)

#define CALL_CHECK_EXTENDED_PLSQLTYPES_ENABLED(con)                             \
                                                                                \
    CALL_CHECK_FEATURE(con, OCI_FEATURE_EXTENDED_PLSQLTYPES, OCI_12_1)

#define CALL_CHECK_STATEMENT_CACHING_ENABLED()                                  \
                                                                                \
    if (Env.version_runtime < OCI_9_2)                                          \
    {                                                                           \
        THROW(ExceptionNotAvailable((dp)->con, OCI_FEATURE_STATEMENT_CACHING))  \
    }

#define CALL_CHECK_DIRPATH_DATE_CACHE_ENABLED(dp)                               \
                                                                                \
    if (Env.version_runtime < OCI_9_2)                                          \
    {                                                                           \
        THROW(ExceptionNotAvailable((dp)->con, OCI_FEATURE_DIRPATH_DATE_CACHE)  \
    }

#define CALL_CHECK_REMOTE_DBS_CONTROL_ENABLED()                                 \
                                                                                \
    if (Env.version_runtime < OCI_10_2)                                         \
    {                                                                           \
        THROW(ExceptionNotAvailable(NULL, OCI_FEATURE_REMOTE_DBS_CONTROL))      \
    }

#define CALL_CHECK_DATABASE_NOTIFY_ENABLED()                                    \
                                                                                \
    if (Env.version_runtime < OCI_10_2)                                         \
    {                                                                           \
        THROW(ExceptionNotAvailable(NULL, OCI_FEATURE_DATABASE_NOTIFY))         \
    }

#define CALL_CHECK_HIGH_AVAILABILITY_ENABLED()                                  \
                                                                                \
    if (Env.version_runtime < OCI_10_2)                                         \
    {                                                                           \
        THROW(ExceptionNotAvailable(NULL, OCI_FEATURE_HIGH_AVAILABILITY))       \
    }

#define CALL_CHECK_XA_ENABLED(mode)                                             \
                                                                                \
    if ( ((mode) & OCI_SESSION_XA) && (!Env.use_xa) )                           \
    {                                                                           \
        THROW(ExceptionNotAvailable(NULL, OCI_FEATURE_XA))                      \
    }

#define CALL_CHECK_ENUM_VALUE(con, stmt, mode, values, name)                    \
                                                                                \
    {                                                                           \
        size_t ii = 0, nn = sizeof(values) / sizeof((values)[0]);               \
        for (; ii < nn; ii++) { if ((mode) == (values)[ii]) break; }            \
        if (ii >= nn)                                                           \
        {                                                                       \
            THROW(ExceptionArgInvalidValue(con, stmt, name, mode))              \
        }                                                                       \
    }



#endif /* OCILIB_MACROS_H_INCLUDED */
