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

#include "error.h"
#include "exception.h"
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

#elif defined(__GNUC__)

    #define DO_PRAGMA(X) _Pragma(#X)

    #define WARNING_DISABLE(w)                  \
        DO_PRAGMA(GCC diagnostic push)          \
        DO_PRAGMA(GCC diagnostic ignored #w)

    #define WARNING_RESTORE(w)                  \
        DO_PRAGMA(GCC diagnostic pop)           \

    #define WARNING_DISABLE_CAST_FUNC_TYPE      WARNING_DISABLE(-Wcast-function-type)
    #define WARNING_RESTORE_CAST_FUNC_TYPE      WARNING_RESTORE(-Wcast-function-type)
    #define WARNING_DISABLE_PEDANTIC            WARNING_DISABLE(-Wpedantic)
    #define WARNING_RESTORE_PEDANTIC            WARNING_RESTORE(-Wpedantic)

#endif

/* declare a call context */

#define CONTEXT(src_type, src_ptr)       \
                                         \
    OCI_Context call_context;            \
    call_context.source_ptr  = src_ptr;  \
    call_context.source_type = src_type; \
    call_context.location    = __func__; \

/* enter in a call */

#define ENTER_FUNC(ret_type, ret_value, src_type, src_ptr) \
                                                           \
    ret_type call_retval = ret_value;                      \
    boolean call_status = TRUE;                            \
    CONTEXT(src_type, src_ptr)                             \

#define ENTER_FUNC_NO_CONTEXT(ret_type, ret_value)         \
                                                           \
    ret_type call_retval = ret_value;                      \
    boolean call_status = TRUE;                            \

#define ENTER_VOID(src_type, src_ptr) \
                                      \
    boolean call_status = TRUE;       \
    CONTEXT(src_type, src_ptr)        \

/* jumps */

#define JUMP_CLEANUP() \
                       \
    goto CleanupLabel; \

#define JUMP_EXIT() \
                    \
    goto ExitLabel; \

/* labels */

#define LABEL_CLEANUP(exp) \
                           \
CleanupLabel:              \
    {exp;}                 \
    goto ExitLabel;        \

/* return to the caller */

#define LABEL_EXIT_VOID()           \
                                    \
ExitLabel:                          \
    return;

#define LABEL_EXIT_FUNC()           \
                                    \
ExitLabel:                          \
    return call_retval;

/* status management */

#define CHECK(exp)           \
                             \
    {                        \
        call_status = (exp); \
        if (!call_status)    \
        {                    \
            JUMP_CLEANUP()   \
        }                    \
    }

/* invoke exception and jump to cleanup */

#define THROW_NO_ARGS(func)           \
                                      \
    func(&call_context);              \
    CHECK(FALSE)                      \

#define THROW(func, ...)              \
                                      \
    func(&call_context, __VA_ARGS__); \
    CHECK(FALSE)                      \


/* exit a call */

#define EXIT_FUNC()   \
                      \
    LABEL_CLEANUP(; ) \
    LABEL_EXIT_FUNC()

#define CLEANUP_AND_EXIT_FUNC(exp) \
                                   \
    LABEL_CLEANUP(exp)             \
    LABEL_EXIT_FUNC()

#define EXIT_VOID()   \
                      \
    LABEL_CLEANUP(; ) \
    LABEL_EXIT_VOID() \


#define FAILURE (call_status == FALSE)

/* return value management */

#define SET_RETVAL(exp)  { call_retval = exp; }

#define SET_SUCCESS()  SET_RETVAL(TRUE)

/* error management when calling or assigning */

#define CHECK_NULL(ptr) \
                        \
    if (NULL == (ptr))  \
    {                   \
        CHECK(FALSE)    \
    }

/* checking macros */

#define CHECK_FALSE(exp, ret) if (exp) return (ret);

#define CHECK_ERROR(err) CHECK(NULL == (err) || OCI_UNKNOWN == (err)->type)

#define CHECK_PTR(type, ptr)                        \
                                                    \
    if (!(ptr))                                     \
    {                                               \
        THROW(OcilibExceptionNullPointer, (type))   \
    }

#define CHECK_BOUND(v, b1, b2)                  \
                                                \
    if (((v) < (b1)) || ((v) > (b2)))           \
    {                                           \
        THROW(OcilibExceptionOutOfBounds, (v))  \
    }

#define CHECK_MIN(v, m)                         \
                                                \
    if ((v) < (m))                              \
    {                                           \
        THROW(OcilibExceptionMinimumValue, (m)) \
    }

#define CHECK_COMPAT(exp)                               \
                                                        \
    if (!(exp))                                         \
    {                                                   \
        THROW_NO_ARGS(OcilibExceptionTypeNotCompatible) \
    }

#define CHECK_STMT_STATUS(st, v)                    \
                                                    \
    if ((((st)->status) & (v)) == 0)                \
    {                                               \
        THROW(OcilibExceptionStatementState, (v))   \
    }

#define CHECK_OBJECT_FETCHED(obj)                  \
                                                   \
    if (OCI_OBJECT_FETCHED_CLEAN == (obj)->hstate) \
    {                                              \
        JUMP_EXIT()                                \
    }                                              \

#define CHECK_CON_STATUS(cn, v) \
                                \
    if ((cn)->cstate != (v))    \
    {                           \
        JUMP_EXIT()             \
    }                           \

#define CHECK_SCROLLABLE_CURSOR_ACTIVATED(st)                   \
                                                                \
    if (((st)->nb_rbinds > 0) ||                                \
        ((st)->exec_mode != OCI_STMT_SCROLLABLE_READONLY))      \
    {                                                           \
        THROW_NO_ARGS(OcilibExceptionStatementNotScrollable)    \
    }

#define CHECK_DIRPATH_STATUS(dp, v)                         \
                                                            \
    if ((dp)->status != (v))                                \
    {                                                       \
        THROW(OcilibExceptionDirPathState, (dp)->status)    \
    }

#define CHECK_INITIALIZED()                             \
                                                        \
    if (!Env.loaded)                                    \
    {                                                   \
        THROW_NO_ARGS(OcilibExceptionNotInitialized)    \
    }

#define CHECK_FEATURE(con, feat, ver)                                     \
                                                                          \
    if (Env.version_runtime < (ver) || ((con) && (con)->ver_num < (ver))) \
    {                                                                     \
        THROW(OcilibExceptionNotAvailable, (feat))                        \
    }

#define CHECK_THREAD_ENABLED()                          \
                                                        \
    if (!(LIB_THREADED))                                \
    {                                                   \
        THROW_NO_ARGS(OcilibExceptionNotMultithreaded)  \
    }

#define CHECK_TIMESTAMP_ENABLED(con) \
                                     \
    CHECK_FEATURE(con, OCI_FEATURE_TIMESTAMP, OCI_9_0)

#define CHECK_INTERVAL_ENABLED(con) \
                                    \
    CHECK_FEATURE(con, OCI_FEATURE_TIMESTAMP, OCI_9_0)

#define CHECK_SCROLLABLE_CURSOR_ENABLED(con) \
                                             \
    CHECK_FEATURE(con, OCI_FEATURE_SCROLLABLE_CURSOR, OCI_9_0)

#define CHECK_EXTENDED_PLSQLTYPES_ENABLED(con) \
                                               \
    CHECK_FEATURE(con, OCI_FEATURE_EXTENDED_PLSQLTYPES, OCI_12_1)

#define CHECK_STATEMENT_CACHING_ENABLED()                                   \
                                                                            \
    if (Env.version_runtime < OCI_9_2)                                      \
    {                                                                       \
        THROW(OcilibExceptionNotAvailable, OCI_FEATURE_STATEMENT_CACHING)   \
    }

#define CHECK_DIRPATH_DATE_CACHE_ENABLED(dp)                                \
                                                                            \
    if (Env.version_runtime < OCI_9_2)                                      \
    {                                                                       \
        THROW(OcilibExceptionNotAvailable, OCI_FEATURE_DIRPATH_DATE_CACHE)  \
    }

#define CHECK_REMOTE_DBS_CONTROL_ENABLED()                                  \
                                                                            \
    if (Env.version_runtime < OCI_10_2)                                     \
    {                                                                       \
        THROW(OcilibExceptionNotAvailable, OCI_FEATURE_REMOTE_DBS_CONTROL)  \
    }

#define CHECK_DATABASE_NOTIFY_ENABLED()                                     \
                                                                            \
    if (Env.version_runtime < OCI_10_2)                                     \
    {                                                                       \
        THROW(OcilibExceptionNotAvailable, OCI_FEATURE_DATABASE_NOTIFY)     \
    }

#define CHECK_HIGH_AVAILABILITY_ENABLED()                                   \
                                                                            \
    if (Env.version_runtime < OCI_10_2)                                     \
    {                                                                       \
        THROW(OcilibExceptionNotAvailable, OCI_FEATURE_HIGH_AVAILABILITY)   \
    }

#define CHECK_XA_ENABLED(mode)                              \
                                                            \
    if ( ((mode) & OCI_SESSION_XA) && (!Env.use_xa) )       \
    {                                                       \
        THROW(OcilibExceptionNotAvailable, OCI_FEATURE_XA)  \
    }

#define CHECK_ENUM_VALUE(mode, values, name)                            \
                                                                        \
    {                                                                   \
        size_t ii = 0, nn = sizeof(values) / sizeof((values)[0]);       \
        for (; ii < nn; ii++)                                           \
        {                                                               \
            if ((mode) == (values)[ii]) break;                          \
            }                                                           \
            if (ii >= nn)                                               \
            {                                                           \
                THROW(OcilibExceptionArgInvalidValue, (name), (mode))   \
            }                                                           \
        }

/* memory management helpers */

#define FREE(ptr)   { OcilibMemoryFree(ptr); (ptr) = NULL; }

/* helpers macros */

#define GET_PROP(rtype, rvalue, stype, sptr, prop) \
                                                   \
    ENTER_FUNC(rtype, rvalue, stype, sptr)         \
                                                   \
    CHECK_PTR(stype, sptr)                         \
                                                   \
    SET_RETVAL((rtype) ((sptr)->prop))             \
                                                   \
    EXIT_FUNC()

#define SET_PROP(stype, sptr, prop, ptype, val) \
                                                \
    ENTER_FUNC(boolean, TRUE, stype, sptr)      \
                                                \
    CHECK_PTR(stype, sptr)                      \
                                                \
    (sptr)->prop = (ptype) (val);               \
                                                \
    SET_SUCCESS()                               \
                                                \
    EXIT_FUNC()

#define SET_PROP_ENUM(stype, sptr, prop, ptype, val, enms, msg) \
                                                                \
    ENTER_FUNC(boolean, TRUE, stype, sptr)                      \
                                                                \
    CHECK_PTR(stype, sptr)                                      \
    CHECK_ENUM_VALUE((val), (enms), (msg))                      \
                                                                \
    (sptr)->prop = (ptype) (val);                               \
                                                                \
    SET_SUCCESS()                                               \
                                                                \
    EXIT_FUNC()

#define GET_LIB_PROP(type, value, prop)         \
                                                \
    ENTER_FUNC(type, value, OCI_IPC_VOID, &Env) \
                                                \
    CHECK_INITIALIZED()                         \
                                                \
    SET_RETVAL((type) (prop))                   \
                                                \
    EXIT_FUNC()                                 \

#define SET_LIB_PROP(prop, value)                  \
                                                   \
    ENTER_FUNC(boolean, FALSE, OCI_IPC_VOID, &Env) \
                                                   \
    CHECK_INITIALIZED()                            \
                                                   \
    (prop) = (value);                              \
                                                   \
    SET_SUCCESS()                                  \
                                                   \
    EXIT_FUNC()                                    \

#define ALLOC_BUFFER(type, ptr, size, count)                            \
                                                                        \
    if (!(ptr))                                                         \
    {                                                                   \
        (ptr) = OcilibMemoryAlloc(type, size, (size_t) (count), TRUE);  \
                                                                        \
        CHECK_NULL((ptr))                                               \
    }                                                                   \

#define REALLOC_BUFFER(type, ptr, size, current, allocated, requested)                          \
                                                                                                \
    if (!(ptr))                                                                                 \
    {                                                                                           \
        (ptr)                = OcilibMemoryAlloc(type, size,  (size_t) (requested), TRUE);      \
        if (ptr) (allocated) = (requested);                                                     \
    }                                                                                           \
    else if ((current) >= (allocated))                                                          \
    {                                                                                           \
        (ptr)                = OcilibMemoryRealloc(ptr, type, size, (size_t) (requested), TRUE);\
        if (ptr) (allocated) = (requested);                                                     \
    }                                                                                           \
                                                                                                \
    CHECK_NULL((ptr))                                                                           \

#define ALLOC_DATA(type, ptr, count) \
                                     \
    ALLOC_BUFFER(type, ptr, sizeof(*(ptr)), count)

#define REALLOC_DATA(type, ptr, cur, alloc, inc) \
                                                 \
    REALLOC_BUFFER(type, ptr, sizeof(*(ptr)), cur, alloc, inc)

#define ARRAY_GET_AT(ptr, size, offset) \
                                        \
    (((ub1 *) (ptr)) + (size_t)((size)*i))

#define ARRAY_SET_AT(ptr, type, offset, value) \
                                               \
    *(type*)(ARRAY_GET_AT(ptr, sizeof(type), i)) = (type) (value);

#define IS_STRING_VALID(s) ((s) && ((s)[0]))

#define CHECK_OCI(oci_err, func, ...)                                   \
                                                                        \
    {                                                                   \
        sword oci_retcall = func(__VA_ARGS__);                          \
        if (OCI_FAILURE(oci_retcall))                                   \
        {                                                               \
            OcilibExceptionOCI(&call_context, oci_err, oci_retcall);    \
            CHECK(OCI_SUCCESS_WITH_INFO == oci_retcall)                 \
        }                                                               \
    }                                                                   \

#define CHECK_ATTRIB_GET(htype, atype, handle, value, size, err)           \
                                                                           \
    CHECK_OCI                                                              \
    (                                                                      \
        err, OCIAttrGet, (void*) (handle), (ub4) (htype), (void*) (value), \
        (ub4*) (size), (ub4) (atype), err                                  \
    )                                                                      \

#define CHECK_ATTRIB_SET(htype, atype, handle, value, size, err)           \
                                                                           \
    CHECK_OCI                                                              \
    (                                                                      \
        err, OCIAttrSet, (void*) (handle), (ub4) (htype), (void*) (value), \
        (ub4) (size), (ub4) (atype), err                                   \
    )                                                                      \

#endif /* OCILIB_MACROS_H_INCLUDED */
