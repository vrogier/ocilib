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

#ifndef OCILIB_MACROS_H_INCLUDED
#define OCILIB_MACROS_H_INCLUDED

#include "environment.h"
#include "error.h"
#include "exception.h"
#include "log.h"
#include "memory.h"
#include "types.h"

/* Global warning macro specific warnings macro */

#if defined(_MSC_VER)

#define WARNING_DISABLE(w)              __pragma(warning( disable : w ))
#define WARNING_RESTORE(w)              __pragma(warning( default : w ))

#define WARNING_DISABLE_CAST_FUNC_TYPE  WARNING_DISABLE(4054)
#define WARNING_RESTORE_CAST_FUNC_TYPE  WARNING_RESTORE(4054)
#define WARNING_DISABLE_PEDANTIC
#define WARNING_RESTORE_PEDANTIC
#define WARNING_DISABLE_UNSAFE_CONVERT  WARNING_DISABLE(4191)
#define WARNING_RESTORE_UNSAFE_CONVERT  WARNING_RESTORE(4191)

#elif defined(__GNUC__)

#define DO_PRAGMA(exp) _Pragma(#exp)

#define WARNING_DISABLE(w)             \
        DO_PRAGMA(GCC diagnostic push) \
        DO_PRAGMA(GCC diagnostic ignored #w)

#define WARNING_RESTORE(w)            \
        DO_PRAGMA(GCC diagnostic pop) \

  #if __GNUC__ > 7
#define WARNING_DISABLE_CAST_FUNC_TYPE      WARNING_DISABLE(-Wcast-function-type)
#define WARNING_RESTORE_CAST_FUNC_TYPE      WARNING_RESTORE(-Wcast-function-type)
  #else
#define WARNING_DISABLE_CAST_FUNC_TYPE
#define WARNING_RESTORE_CAST_FUNC_TYPE
  #endif

#define WARNING_DISABLE_PEDANTIC            WARNING_DISABLE(-Wpedantic)
#define WARNING_RESTORE_PEDANTIC            WARNING_RESTORE(-Wpedantic)

#define WARNING_DISABLE_UNSAFE_CONVERT
#define WARNING_RESTORE_UNSAFE_CONVERT

#endif

/* declare a call context
 *
 * The framework's hidden locals are prefixed with ocilib_ so that a user
 * variable or macro can never collide with them.
 */

#define CONTEXT(src_type, src_ptr)                                      \
                                                                        \
        OCI_Context ocilib_call_context;                                \
        ocilib_call_context.source_ptr  = src_ptr;                      \
        ocilib_call_context.source_type = src_type;                     \
        ocilib_call_context.location    = __func__;                     \
        ocilib_call_context.env         = (src_ptr)                     \
                                      ? ((OCI_Handle *) (src_ptr))->env \
                                      : NULL;

/* enter in a call */

#define ENTER_FUNC(ret_type, ret_value, src_type, src_ptr) \
                                                           \
        ret_type ocilib_call_retval = ret_value;           \
        OCI_Boolean ocilib_call_status = TRUE;             \
        CONTEXT(src_type, src_ptr)                         \
        OCILIB_LOG_API_ENTRY();                            \

#define ENTER_FUNC_NO_CONTEXT(ret_type, ret_value) \
                                                   \
        ret_type ocilib_call_retval = ret_value;   \
        OCI_Boolean ocilib_call_status = TRUE;     \

#define ENTER_VOID(src_type, src_ptr)          \
                                               \
        OCI_Boolean ocilib_call_status = TRUE; \
        CONTEXT(src_type, src_ptr)             \
        OCILIB_LOG_API_ENTRY();

/* jumps */

#define JUMP_CLEANUP()                 \
                                       \
        do                             \
        {                              \
            goto ocilib_cleanup_label; \
        }                              \
        while (0)

/* JUMP_EXIT_NO_CLEANUP skips the cleanup block entirely. Prefer JUMP_CLEANUP:
   bypassing cleanup silently drops resource releases and failure bookkeeping. */

#define JUMP_EXIT_NO_CLEANUP()      \
                                    \
        do                          \
        {                           \
            goto ocilib_exit_label; \
        }                           \
        while (0)

/* labels */

#define LABEL_CLEANUP(exp)                                      \
                                                                \
ocilib_cleanup_label:                                           \
        if (NULL != ocilib_call_context.source_ptr)             \
        {                                                       \
            exp;                                                \
        }                                                       \
        goto ocilib_exit_label;                                 \

/* return to the caller */

#define LABEL_EXIT_VOID()           \
                                    \
ocilib_exit_label:                  \
        OCILIB_LOG_API_EXIT_VOID(); \
        return;

#define LABEL_EXIT_FUNC()           \
                                    \
ocilib_exit_label:                  \
        OCILIB_LOG_API_EXIT_FUNC(); \
        return ocilib_call_retval;

/* status management */

#define CHECK(exp)                      \
                                        \
        do                              \
        {                               \
            ocilib_call_status = (exp); \
            if (!ocilib_call_status)    \
            {                           \
                JUMP_CLEANUP();         \
            }                           \
        }                               \
        while (0)

/* invoke exception and jump to cleanup */

#define THROW_NO_ARGS(func)             \
                                        \
        do                              \
        {                               \
            func(&ocilib_call_context); \
            CHECK(FALSE);               \
        }                               \
        while (0)

#define THROW(func, ...)                             \
                                                     \
        do                                           \
        {                                            \
            func(&ocilib_call_context, __VA_ARGS__); \
            CHECK(FALSE);                            \
        }                                            \
        while (0)

/* exit a call */

#define EXIT_FUNC()       \
                          \
        LABEL_CLEANUP(; ) \
        LABEL_EXIT_FUNC()

#define CLEANUP_AND_EXIT_FUNC(exp) \
                                   \
        LABEL_CLEANUP(exp)         \
        LABEL_EXIT_FUNC()

#define EXIT_VOID()       \
                          \
        LABEL_CLEANUP(; ) \
        LABEL_EXIT_VOID() \


/* logging helpers */

#define LOG_ENABLED(cat, lvl) \
        OcilibLogIsEnabled((cat), (lvl))

#define OCILIB_LOG_STEP_WITH_ENV(env, cat, lvl, step, func, ...) \
        do { if (LOG_ENABLED(cat, lvl)) { OcilibLogWrite((cat), (lvl), (step), (func), __VA_ARGS__); } } while(0)

#define OCILIB_LOG_WITH_ENV(env, cat, lvl, func, ...) \
        OCILIB_LOG_STEP_WITH_ENV((env), (cat), (lvl), NULL, (func), __VA_ARGS__)

#define OCILIB_LOG(cat, lvl, ...) \
        OCILIB_LOG_WITH_ENV(ocilib_call_context.env, (cat), (lvl), __func__, __VA_ARGS__)

#define OCILIB_LOG_STEP(cat, lvl, step, ...) \
        OCILIB_LOG_STEP_WITH_ENV(ocilib_call_context.env, (cat), (lvl), (step), __func__, __VA_ARGS__)

#define OCILIB_LOG_API_ENTRY() \
        OCILIB_LOG_STEP(OCI_LOG_CAT_LIB, OCI_LOG_TRACE, "ENTER", OTEXT("%s"), OTEXT(""))

#define OCILIB_LOG_API_EXIT_VOID() \
        OCILIB_LOG_STEP(OCI_LOG_CAT_LIB, OCI_LOG_TRACE, "LEAVE", OTEXT("%s"), OTEXT(""))

#define OCILIB_LOG_API_EXIT_FUNC() \
        OCILIB_LOG_STEP(OCI_LOG_CAT_LIB, OCI_LOG_TRACE, "LEAVE", OTEXT("(ret=%p)"), (void *) (OCI_Size) (ocilib_call_retval))

#define FAILURE (ocilib_call_status == FALSE)

/* return value management */

#define SET_RETVAL(exp)                 \
                                        \
        do                              \
        {                               \
            ocilib_call_retval = (exp); \
        }                               \
        while (0)

#define SET_SUCCESS()  SET_RETVAL(TRUE)

/* checking macros */

#define CHECK_NULL(ptr)        \
                               \
        do                     \
        {                      \
            if (NULL == (ptr)) \
            {                  \
                CHECK(FALSE);  \
            }                  \
        }                      \
        while (0)

#define CHECK_ERROR(err) CHECK(NULL == (err) || OCI_UNKNOWN == (err)->type)

#define CHECK_PTR(type, ptr)                               \
                                                           \
        do                                                 \
        {                                                  \
            if (!(ptr))                                    \
            {                                              \
                THROW(OcilibExceptionNullPointer, (type)); \
            }                                              \
        }                                                  \
        while (0)

#define CHECK_BOUND(v, b1, b2)                                     \
                                                                   \
        do                                                         \
        {                                                          \
            if (((v) < (b1)) || ((v) > (b2)))                      \
            {                                                      \
                THROW(OcilibExceptionOutOfBounds, (OCI_Int32)(v)); \
            }                                                      \
        }                                                          \
        while (0)

#define CHECK_INDEX(v, n)                                                                                                                do                                                                 {                                                                      if ((v) >= (n))                                                    {                                                                      THROW(OcilibExceptionOutOfBounds, (OCI_Int32)(v));             }                                                              }                                                                  while (0)

#define CHECK_MIN(v, m)                                  \
                                                         \
        do                                               \
        {                                                \
            if ((v) < (m))                               \
            {                                            \
                THROW(OcilibExceptionMinimumValue, (m)); \
            }                                            \
        }                                                \
        while (0)

#define CHECK_COMPAT(exp)                                        \
                                                                 \
        do                                                       \
        {                                                        \
            if (!(exp))                                          \
            {                                                    \
                THROW_NO_ARGS(OcilibExceptionTypeNotCompatible); \
            }                                                    \
        }                                                        \
        while (0)

#define CHECK_STMT_STATUS(st, v)                           \
                                                           \
        do                                                 \
        {                                                  \
            if ((((st)->status) & (v)) == 0)               \
            {                                              \
                THROW(OcilibExceptionStatementState, (v)); \
            }                                              \
        }                                                  \
        while (0)

#define CHECK_OBJECT_FETCHED(obj)                          \
                                                           \
        do                                                 \
        {                                                  \
            if (OCI_OBJECT_FETCHED_CLEAN == (obj)->hstate) \
            {                                              \
                SET_SUCCESS();                             \
                JUMP_EXIT_NO_CLEANUP();                    \
            }                                              \
        }                                                  \
        while (0)

#define CHECK_CON_STATUS(cn, v)         \
                                        \
        do                              \
        {                               \
            if ((cn)->cstate != (v))    \
            {                           \
                SET_SUCCESS();          \
                JUMP_EXIT_NO_CLEANUP(); \
            }                           \
        }                               \
        while (0)

#define CHECK_SCROLLABLE_CURSOR_ACTIVATED(st)                         \
                                                                      \
        do                                                            \
        {                                                             \
            if (((st)->nb_rbinds > 0) ||                              \
                ((st)->exec_mode != OCI_STMT_SCROLLABLE_READONLY))    \
            {                                                         \
                THROW_NO_ARGS(OcilibExceptionStatementNotScrollable); \
            }                                                         \
        }                                                             \
        while (0)

#define CHECK_DIRPATH_STATUS(dp, v)                                            \
                                                                               \
        do                                                                     \
        {                                                                      \
            if ((dp)->status != (v))                                           \
            {                                                                  \
                THROW(OcilibExceptionDirPathState, (OCI_Int32)((dp)->status)); \
            }                                                                  \
        }                                                                      \
        while (0)

#define CHECK_ENV_FEATURE(feat, ver)                              \
                                                                  \
        do                                                        \
        {                                                         \
            if (!ocilib_call_context.env ||                       \
                ocilib_call_context.env->version_runtime < (ver)) \
            {                                                     \
                THROW(OcilibExceptionNotAvailable, (feat));       \
            }                                                     \
        }                                                         \
        while (0)

#define CHECK_FEATURE(con, feat, ver)                                 \
                                                                      \
        do                                                            \
        {                                                             \
            if (!ocilib_call_context.env ||                           \
                (ocilib_call_context.env->version_runtime < (ver)) || \
                ((con) && (con)->ver_num < (ver)))                    \
            {                                                         \
                THROW(OcilibExceptionNotAvailable, (feat));           \
            }                                                         \
        }                                                             \
        while (0)

#define CHECK_THREAD_ENABLED()                                  \
                                                                \
        do                                                      \
        {                                                       \
            if (!LIB_THREADED(ocilib_call_context.env))         \
            {                                                   \
                THROW_NO_ARGS(OcilibExceptionNotMultithreaded); \
            }                                                   \
        }                                                       \
        while (0)

#define CHECK_TIMESTAMP_ENABLED() \
                                  \
        CHECK_ENV_FEATURE(OCI_FEATURE_TIMESTAMP, OCI_9_0)

#define CHECK_INTERVAL_ENABLED() \
                                 \
        CHECK_ENV_FEATURE(OCI_FEATURE_TIMESTAMP, OCI_9_0)

#define CHECK_SCROLLABLE_CURSOR_ENABLED(con) \
                                             \
        CHECK_FEATURE(con, OCI_FEATURE_SCROLLABLE_CURSOR, OCI_9_0)

#define CHECK_EXTENDED_PLSQLTYPES_ENABLED(con) \
                                               \
        CHECK_FEATURE(con, OCI_FEATURE_EXTENDED_PLSQLTYPES, OCI_12_1)

#define CHECK_STATEMENT_CACHING_ENABLED()                                          \
                                                                                   \
        do                                                                         \
        {                                                                          \
            if (!ocilib_call_context.env ||                                        \
                ocilib_call_context.env->version_runtime < OCI_9_2)                \
            {                                                                      \
                THROW(OcilibExceptionNotAvailable, OCI_FEATURE_STATEMENT_CACHING); \
            }                                                                      \
        }                                                                          \
        while (0)

#define CHECK_DIRPATH_DATE_CACHE_ENABLED(dp)                                        \
                                                                                    \
        do                                                                          \
        {                                                                           \
            if (!ocilib_call_context.env ||                                         \
                ocilib_call_context.env->version_runtime < OCI_9_2)                 \
            {                                                                       \
                THROW(OcilibExceptionNotAvailable, OCI_FEATURE_DIRPATH_DATE_CACHE); \
            }                                                                       \
        }                                                                           \
        while (0)

#define CHECK_REMOTE_DBS_CONTROL_ENABLED()                                          \
                                                                                    \
        do                                                                          \
        {                                                                           \
            if (!ocilib_call_context.env ||                                         \
                ocilib_call_context.env->version_runtime < OCI_10_2)                \
            {                                                                       \
                THROW(OcilibExceptionNotAvailable, OCI_FEATURE_REMOTE_DBS_CONTROL); \
            }                                                                       \
        }                                                                           \
        while (0)

#define CHECK_DATABASE_NOTIFY_ENABLED()                                          \
                                                                                 \
        do                                                                       \
        {                                                                        \
            if (!ocilib_call_context.env ||                                      \
                ocilib_call_context.env->version_runtime < OCI_10_2)             \
            {                                                                    \
                THROW(OcilibExceptionNotAvailable, OCI_FEATURE_DATABASE_NOTIFY); \
            }                                                                    \
        }                                                                        \
        while (0)

#define CHECK_HIGH_AVAILABILITY_ENABLED()                                          \
                                                                                   \
        do                                                                         \
        {                                                                          \
            if (!ocilib_call_context.env ||                                        \
                ocilib_call_context.env->version_runtime < OCI_10_2)               \
            {                                                                      \
                THROW(OcilibExceptionNotAvailable, OCI_FEATURE_HIGH_AVAILABILITY); \
            }                                                                      \
        }                                                                          \
        while (0)

#define CHECK_VECTOR_ENABLED(con) \
                                  \
        CHECK_FEATURE(con, OCI_FEATURE_VECTOR, OCI_23_4)

#define CHECK_SODA_ENABLED(con) \
                                \
        CHECK_FEATURE(con, OCI_FEATURE_SODA, OCI_18_1)

#define CHECK_ENUM_VALUE(mode, values, name)                            \
                                                                        \
        do                                                              \
        {                                                               \
            OCI_Size ii = 0, nn = sizeof(values) / sizeof((values)[0]); \
            for (; ii < nn; ii++)                                       \
            {                                                           \
                if ((mode) == (values)[ii]) break;                      \
            }                                                           \
            if (ii >= nn)                                               \
            {                                                           \
                THROW(OcilibExceptionArgInvalidValue, (name), (mode));  \
            }                                                           \
        }                                                               \
        while (0)

/* memory management helpers */

#define FREE(ptr)                                             \
                                                              \
        do                                                    \
        {                                                     \
            OcilibMemoryFree(ocilib_call_context.env, (ptr)); \
            (ptr) = NULL;                                     \
        }                                                     \
        while (0)

/* helpers macros */

#define IS_WIDE_CHAR_ENV()   (ocilib_call_context.env && (OCI_CHAR_WIDE == ocilib_call_context.env->charset))
#define IS_ANSI_ENV()        (ocilib_call_context.env && (OCI_CHAR_ANSI == ocilib_call_context.env->charset))
#define IS_UTF8_ENV()        (ocilib_call_context.env && (TRUE == ocilib_call_context.env->charset))

#define GET_PROP(rtype, rvalue, stype, sptr, prop) \
                                                   \
        ENTER_FUNC(rtype, rvalue, stype, sptr)     \
                                                   \
        CHECK_PTR(stype, sptr);                    \
                                                   \
        SET_RETVAL((rtype) ((sptr)->prop));        \
                                                   \
        EXIT_FUNC()

#define SET_PROP(stype, sptr, prop, ptype, val)    \
                                                   \
        ENTER_FUNC(OCI_Boolean, TRUE, stype, sptr) \
                                                   \
        CHECK_PTR(stype, sptr);                    \
                                                   \
        (sptr)->prop = (ptype) (val);              \
                                                   \
        SET_SUCCESS();                             \
                                                   \
        EXIT_FUNC()

#define SET_PROP_ENUM(stype, sptr, prop, ptype, val, enms, msg) \
                                                                \
        ENTER_FUNC(OCI_Boolean, TRUE, stype, sptr)              \
                                                                \
        CHECK_PTR(stype, sptr);                                 \
        CHECK_ENUM_VALUE((val), (enms), (msg));                 \
                                                                \
        (sptr)->prop = (ptype) (val);                           \
                                                                \
        SET_SUCCESS();                                          \
                                                                \
        EXIT_FUNC()

#define GET_LIB_PROP(env, type, value, prop)              \
                                                          \
        ENTER_FUNC(type, value, OCI_IPC_ENVIRONMENT, env) \
                                                          \
        CHECK_PTR(OCI_IPC_ENVIRONMENT, env);              \
                                                          \
        SET_RETVAL((type) (prop));                        \
                                                          \
        EXIT_FUNC()                                       \

#define SET_LIB_PROP(env, prop, value)                           \
                                                                 \
        ENTER_FUNC(OCI_Boolean, FALSE, OCI_IPC_ENVIRONMENT, env) \
                                                                 \
        CHECK_PTR(OCI_IPC_ENVIRONMENT, env);                     \
                                                                 \
        (prop) = (value);                                        \
                                                                 \
        SET_SUCCESS();                                           \
                                                                 \
        EXIT_FUNC()                                              \

#define ALLOC_BUFFER(type, ptr, size, count)                                   \
                                                                               \
        do                                                                     \
        {                                                                      \
            if (!(ptr))                                                        \
            {                                                                  \
                (ptr) = OcilibMemoryAlloc(ocilib_call_context.env, type, size, \
                                          (OCI_Size) (count), TRUE);           \
                CHECK_NULL((ptr));                                             \
            }                                                                  \
        }                                                                      \
        while (0)

#define REALLOC_BUFFER(type, ptr, size, current, allocated, requested)                \
                                                                                      \
        do                                                                            \
        {                                                                             \
            if (!(ptr))                                                               \
            {                                                                         \
                (ptr) = OcilibMemoryAlloc(ocilib_call_context.env, type, size,        \
                                          (OCI_Size) (requested), TRUE);              \
                if (ptr) (allocated) = (requested);                                   \
            }                                                                         \
            else if ((current) >= (allocated))                                        \
            {                                                                         \
                (ptr) = OcilibMemoryRealloc(ocilib_call_context.env, ptr, type, size, \
                                            (OCI_Size) (requested), TRUE);            \
                if (ptr) (allocated) = (requested);                                   \
            }                                                                         \
                                                                                      \
            CHECK_NULL((ptr));                                                        \
        }                                                                             \
        while (0)

#define ALLOC_DATA(type, ptr, count) \
                                     \
        ALLOC_BUFFER(type, ptr, sizeof(*(ptr)), count)

#define ALLOC_HANDLE(type, ptr)                     \
                                                    \
        do                                          \
        {                                           \
            ALLOC_DATA(type, ptr, 1);               \
            (ptr)->env   = ocilib_call_context.env; \
            (ptr)->htype = (type);                  \
        }                                           \
        while (0)

#define REALLOC_DATA(type, ptr, cur, alloc, inc) \
                                                 \
        REALLOC_BUFFER(type, ptr, sizeof(*(ptr)), cur, alloc, inc)

#define ARRAY_GET_AT(ptr, size, offset) \
                                        \
        (((ub1 *) (ptr)) + (OCI_Size)((size)*offset))

#define ARRAY_SET_AT(ptr, type, offset, value)                                  \
                                                                                \
        do                                                                      \
        {                                                                       \
            *(type*)(ARRAY_GET_AT(ptr, sizeof(type), offset)) = (type) (value); \
        }                                                                       \
        while (0)

#define IS_STRING_VALID(s) ((s) && ((s)[0]))

#define OCI_CALL(ret, func, ...)                                                \
                                                                                \
        do                                                                      \
        {                                                                       \
            CHECK_NULL(ocilib_call_context.env);                                \
            CHECK_NULL(ocilib_call_context.env->api);                           \
            OCILIB_LOG_STEP_WITH_ENV(ocilib_call_context.env, OCI_LOG_CAT_OCI,  \
                                     OCI_LOG_TRACE, "ENTER", #func, OTEXT("%s"), OTEXT("")); \
            (ret) = ocilib_call_context.env->api->func(__VA_ARGS__);            \
            OCILIB_LOG_STEP_WITH_ENV(ocilib_call_context.env, OCI_LOG_CAT_OCI,  \
                                     OCI_LOG_TRACE, "LEAVE", #func,             \
                                     OTEXT("(ret=%p)"), (void *)(size_t)(ret)); \
        }                                                                       \
        while (0)

#define OCI_ERROR_GET(errhp, code, buf, buflen)                                    \
        (ocilib_call_context.env->api->OCIErrorGet((dvoid *) (errhp), (ub4) 1,     \
            (OraText *) NULL, (sb4 *) (code), (OraText *) (buf), (ub4) (buflen),   \
            (ub4) OCI_HTYPE_ERROR))

#define CHECK_OCI(oci_err, func, ...)                                           \
                                                                                \
        do                                                                      \
        {                                                                       \
            sword oci_retcall;                                                  \
            OCI_CALL(oci_retcall, func, __VA_ARGS__);                           \
            if (OCI_FAILURE(oci_retcall))                                       \
            {                                                                   \
                OcilibExceptionOCI(&ocilib_call_context, oci_err, oci_retcall); \
                CHECK(OCI_SUCCESS_WITH_INFO == oci_retcall);                    \
            }                                                                   \
        }                                                                       \
        while (0)

#define CHECK_ATTRIB_GET(htype, atype, handle, value, size, err)       \
                                                                       \
        CHECK_OCI                                                      \
        (                                                              \
    err, OCIAttrGet, (void*) (handle), (ub4) (htype), (void*) (value), \
    (ub4*) (size), (ub4) (atype), err                                  \
        )                                                              \

#define CHECK_ATTRIB_SET(htype, atype, handle, value, size, err)       \
                                                                       \
        CHECK_OCI                                                      \
        (                                                              \
    err, OCIAttrSet, (void*) (handle), (ub4) (htype), (void*) (value), \
    (ub4) (size), (ub4) (atype), err                                   \
        )                                                              \

#define CHECK_JSON(jdoc, expr)                                   \
                                                                 \
        do                                                       \
        {                                                        \
            ocilib_call_status = (expr);                         \
            if (!ocilib_call_status)                             \
            {                                                    \
                OcilibExceptionJson(&ocilib_call_context, jdoc); \
                JUMP_CLEANUP();                                  \
            }                                                    \
        } while (0)

#define LIST_ATOMIC_OPERATION(list, exp)       \
                                               \
        do                                     \
        {                                      \
            if (OcilibListLock(list))          \
            {                                  \
                WARNING_DISABLE_UNSAFE_CONVERT \
                exp                            \
                WARNING_RESTORE_UNSAFE_CONVERT \
                OcilibListUnlock(list);        \
            }                                  \
        }                                      \
        while (0)

#define LIST_ATOMIC_FOREACH(list, cb)                    \
                                                         \
        LIST_ATOMIC_OPERATION                            \
        (                                                \
    list,                                                \
    {                                                    \
        OcilibListForEach(list, (POCI_LIST_FOR_EACH)cb); \
    }                                                    \
        )

#define LIST_ATOMIC_FOREACH_WITH_PARAM(list, param, cb) \
                                                        \
        LIST_ATOMIC_OPERATION                           \
        (                                               \
    list,                                               \
    {                                                   \
        OcilibListForEachWithParam                      \
        (                                               \
    list,(void *) param,                                \
    (POCI_LIST_FOR_EACH_WITH_PARAM)cb                   \
        );                                              \
    }                                                   \
        )

/* Dispose every list item through cb. Items whose dispose fails are kept
   in the list (and logged by OcilibListDisposeAll) instead of being silently
   unlinked, so a failed teardown no longer loses the only object reference. */

#define LIST_ATOMIC_REMOVE_ALL(list, cb) \
                                         \
        LIST_ATOMIC_OPERATION            \
        (                                \
    list,                                \
    {                                    \
        OcilibListDisposeAll             \
        (                                \
    list, (POCI_LIST_DISPOSE)cb          \
        );                               \
    }                                    \
        )

#define LIST_ATOMIC_FIND(list, cb, data, item) \
                                               \
        LIST_ATOMIC_OPERATION                  \
        (                                      \
    list,                                      \
    {                                          \
        item = OcilibListFind                  \
               (                               \
    list, (POCI_LIST_FIND) cb,                 \
    (void *) data                              \
               );                              \
    }                                          \
        )

#define LIST_ATOMIC_ADD(list, item)                   \
                                                      \
        LIST_ATOMIC_OPERATION                         \
        (                                             \
    list,                                             \
    {                                                 \
        item = OcilibListAppend(list, sizeof(*item)); \
    }                                                 \
        )

#define LIST_ATOMIC_REMOVE(list, item, cb)                      \
                                                                \
        do                                                      \
        {                                                       \
            OCI_Boolean ocilib_dispose_ok = TRUE;               \
            LIST_ATOMIC_OPERATION                               \
            (                                                   \
    list,                                                       \
    {                                                           \
        OcilibListRemove(list, item);                           \
    }                                                           \
            );                                                  \
            ocilib_dispose_ok = (cb(item));                     \
            if (!ocilib_dispose_ok)                             \
            {                                                   \
                OCILIB_LOG(OCI_LOG_CAT_LIB, OCI_LOG_WARN,       \
                           OTEXT("dispose failed for item %p"), \
                           (void*) item);                       \
            }                                                   \
        }                                                       \
        while (0)

#define LIST_ATOMIC_FIND_AND_REMOVE(list, cb, data, item) \
                                                          \
        do                                                \
        {                                                 \
            LIST_ATOMIC_OPERATION                         \
            (                                             \
    list,                                                 \
    {                                                     \
        item = OcilibListFind                             \
               (                                          \
    list, (POCI_LIST_FIND) cb,                            \
    (void *) data                                         \
               );                                         \
        if (NULL != item)                                 \
        {                                                 \
            OcilibListRemove(list, item);                 \
        }                                                 \
    }                                                     \
            );                                            \
            if (NULL != item)                             \
            {                                             \
                cb(item);                                 \
            }                                             \
        }                                                 \
        while (0)

#endif /* OCILIB_MACROS_H_INCLUDED */
