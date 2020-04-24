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

#ifndef OCILIB_CHECKS_H_INCLUDED
#define OCILIB_CHECKS_H_INCLUDED

/* ********************************************************************************************* *
                        MACROS FOR CHECKING OCI CALLS
 * ********************************************************************************************* */

#define OCI_RETVAL          call_retval
#define OCI_STATUS          ((ctx)->call_status)


 /* --------------------------------------------------------------------------------------------- *
  * Local helper macros
  * --------------------------------------------------------------------------------------------- */


/* memory management helpers */

#define OCI_FREE(ptr)                   { MemoryFree(ptr), (ptr) = NULL; }



/* miscellaneous */

#define OCI_NB_ARG_VERSION              3

#define OCI_LIB_THREADED                (OCILib.env_mode & OCI_ENV_THREADED)

#define OCI_CALL_DECLARE_CONTEXT(status)                                        \
                                                                                \
    OCI_Context ctx_obj = { NULL, NULL, NULL, NULL, FALSE };                    \
    OCI_Context* ctx = &ctx_obj;                                                \
    ctx_obj.oci_err    = OCILib.err;                                            \
    ctx_obj.call_status = (status);                                             \

#define OCI_CALL_DECLARE_VARIABLES(type, value, status)                         \
                                                                                \
    type call_retval = (type) (value);                                          \
    OCI_CALL_DECLARE_CONTEXT(status);                                           \

#define OCI_CALL_CONTEXT_ENTER(mode)                                            \
                                                                                \
    if ((mode) & OCI_ENV_CONTEXT)                                               \
    {                                                                           \
        ctx->call_err = ErrorGet(FALSE, FALSE);                                 \
        CallEnter(ctx);                                                         \
    }

#define OCI_CALL_CONTEXT_EXIT(mode)                                             \
                                                                                \
    if ((mode) & OCI_ENV_CONTEXT)                                               \
    {                                                                           \
        CallExit(ctx);                                                          \
    }

#define OCI_CALL_ENTER(type, value)                                             \
                                                                                \
    OCI_CALL_DECLARE_VARIABLES(type, value, TRUE)                               \
    OCI_CALL_CONTEXT_ENTER(OCILib.env_mode)                                     \

#define OCI_CALL_EXIT()                                                         \
                                                                                \
    ExitCall:                                                                   \
    OCI_CALL_CONTEXT_EXIT(OCILib.env_mode)                                      \
    return call_retval;

#define OCI_CALL_JUMP_EXIT()                                                    \
                                                                                \
    goto ExitCall;                                                              \

#define OCI_CALL_CONTEXT_SET(c, s, e)                                           \
    ctx->lib_con      = (c);                                                    \
    ctx->lib_stmt     = (s);                                                    \
    ctx->oci_err      = (e) ? (e) : OCILib.err;                                 \

#define OCI_CALL_CONTEXT_SET_FROM_CONN(con)   OCI_CALL_CONTEXT_SET((con), NULL, ((con) ? (con)->err : OCILib.err))
#define OCI_CALL_CONTEXT_SET_FROM_OBJ(obj)    OCI_CALL_CONTEXT_SET(((obj)->con), NULL, ((obj)->err))
#define OCI_CALL_CONTEXT_SET_FROM_ERR(err)    OCI_CALL_CONTEXT_SET(NULL, NULL, (err))
#define OCI_CALL_CONTEXT_SET_FROM_STMT(stmt)  OCI_CALL_CONTEXT_SET(((stmt)->con), (stmt), ((stmt)->con->err))

#define THROW(exp)                                                \
                                                                                \
    exp;                                                                        \
    OCI_STATUS = FALSE;                                                         \
    OCI_CALL_JUMP_EXIT()                                                        \

#define OCI_IS_PLSQL_STMT(type)                                                 \
    ((OCI_CST_BEGIN == (type)) || (OCI_CST_DECLARE == (type)) || (OCI_CST_CALL == (type)))

#define OCI_IS_OCI_NUMBER(type, subtype)                                        \
   (OCI_CDT_NUMERIC == (type) && OCI_NUM_NUMBER == (subtype))

#define OCI_IS_OCILIB_OBJECT(type, subtype)                                     \
    ( (OCI_IS_OCI_NUMBER(type, subtype)) ||                                     \
      (OCI_CDT_TEXT    != (type) &&                                             \
       OCI_CDT_RAW     != (type) &&                                             \
       OCI_CDT_BOOLEAN != (type)))

#define OCI_GET_PROP(type, value, obj_type, obj, prop, con, stmt, err)          \
                                                                                \
    OCI_CALL_ENTER(type, value)                                                 \
    OCI_CALL_CHECK_PTR(obj_type, obj)                                           \
    OCI_CALL_CONTEXT_SET((con),(stmt), (err))                                   \
    OCI_RETVAL = (type) ((obj)->prop);                                          \
    OCI_CALL_EXIT()                                                             \

#define OCI_SET_PROP(type, obj_type, obj, prop, value, con, stmt, err)          \
                                                                                \
    OCI_CALL_ENTER(boolean, FALSE)                                              \
    OCI_CALL_CHECK_PTR(obj_type, obj)                                           \
    OCI_CALL_CONTEXT_SET((con),(stmt), (err))                                   \
    (obj)->prop = (type) (value);                                               \
    OCI_RETVAL = OCI_STATUS;                                                    \
    OCI_CALL_EXIT()                                                             \

#define OCI_SET_PROP_ENUM(type, obj_type, obj, prop, value, enums, msg, con, stmt, err)\
                                                                                \
    OCI_CALL_ENTER(boolean, FALSE)                                              \
    OCI_CALL_CHECK_PTR(obj_type, obj)                                           \
    OCI_CALL_CONTEXT_SET((con),(stmt), (err))                                   \
    OCI_CALL_CHECK_ENUM_VALUE((con),(stmt), (value), (enums), (msg))            \
    (obj)->prop = (type) (value);                                               \
    OCI_RETVAL = OCI_STATUS;                                                    \
    OCI_CALL_EXIT()                                                             \

#define OCI_GET_LIB_PROP(type, value, prop)                                     \
                                                                                \
    OCI_CALL_ENTER(type, value)                                                 \
    OCI_CALL_CHECK_INITIALIZED()                                                \
    OCI_RETVAL = (type) (prop);                                                 \
    OCI_CALL_EXIT()                                                             \

#define OCI_SET_LIB_PROP(prop, value)                                           \
                                                                                \
    OCI_CALL_ENTER(boolean, FALSE)                                              \
    OCI_CALL_CHECK_INITIALIZED()                                                \
    (prop) = (value);                                                           \
    OCI_RETVAL = OCI_STATUS;                                                    \
    OCI_CALL_EXIT()                                                             \

#define OCI_ALLOCATE_BUFFER(type, ptr, size, count)                             \
                                                                                \
    if (OCI_STATUS && !(ptr))                                                   \
    {                                                                           \
        (ptr) = MemoryAlloc(type, size, (size_t) (count), TRUE);                \
                                                                                \
        OCI_STATUS = (NULL != (ptr));                                           \
    }                                                                           \

#define OCI_REALLOCATE_BUFFER(type, ptr, size, current, allocated, requested)   \
                                                                                \
    if (OCI_STATUS)                                                             \
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
        OCI_STATUS = (NULL != (ptr));                                           \
    }                                                                           \


#define OCI_ALLOCATE_DATA(type, ptr, count)                                     \
                                                                                \
    OCI_ALLOCATE_BUFFER(type, ptr, sizeof(*(ptr)), count)

#define OCI_REALLOCATE_DATA(type, ptr, cur, alloc, inc)                         \
                                                                                \
    OCI_REALLOCATE_BUFFER(type, ptr, sizeof(*(ptr)), cur, alloc, inc)

#define OCI_ARRAY_GET_AT(ptr, size, offset)  (((ub1 *) (ptr)) + (size_t)((size)*i))
#define OCI_ARRAY_SET_AT(ptr, type, offset, value)  *(type*)(OCI_ARRAY_GET_AT(ptr, sizeof(type), i)) = (type) (value);


#define OCI_STRING_VALID(s) ((s) && ((s)[0]))

#define OCI_EXEC(fct)                                                            \
    if (OCI_STATUS)                                                              \
    {                                                                            \
        OCI_STATUS = fct;                                                        \
        if (OCI_FAILURE(OCI_STATUS))                                             \
        {                                                                        \
            OCI_STATUS = (OCI_SUCCESS_WITH_INFO == OCI_STATUS);                  \
            ExceptionOCI(ctx->oci_err, ctx->lib_con, ctx->lib_stmt, OCI_STATUS); \
        }                                                                        \
        else                                                                     \
        {                                                                        \
           OCI_STATUS = TRUE;                                                    \
        }                                                                        \
    }


#define OCI_GET_ATTRIB(htype, atype, handle, value, size)                                           \
                                                                                                    \
    OCI_EXEC                                                                                        \
    (                                                                                               \
        OCIAttrGet((void*) (handle), (ub4) (htype), (void*) (value),                                \
                   (ub4*) (size), (ub4) (atype), (ctx)->oci_err)                                    \
    )                                                                                               \

#define OCI_SET_ATTRIB(htype, atype, handle, value, size)                                           \
                                                                                                    \
    OCI_EXEC                                                                                        \
    (                                                                                               \
        OCIAttrSet((void*) (handle), (ub4) (htype), (void*) (value),                                \
                   (ub4) (size), (ub4) (atype), (ctx)->oci_err)                                     \
    )                                                                                               \



/* ********************************************************************************************* *
                        PARAMETER CHECKING MACROS
 * ********************************************************************************************* */

/**
 * @brief
 * Internal general purpose expression checking
 *
 * @param exp   - Expression
 * @param ret   - Return value if 'exp' is true
 *
 * @note
 * Does not throw an exception.
 *
 */

#define OCI_CHECK(exp, ret) if (exp) return (ret);

/**
 * @brief
 * Checks if a pointer passed to public function is NULL
 *
 * @param type  - Pointer type
 * @param ptr   - Pointer
 *
 * @note
 * Throws an exception if the pointer is null.
 *
 */

#define OCI_CALL_CHECK_PTR(type, ptr)                                          \
                                                                               \
    if (!(ptr))                                                                \
    {                                                                          \
        THROW(ExceptionNullPointer(type))                        \
    }

/**
 * @brief
 * Checks if the parameters of a bind call are valid
 *
 * @param stmt      - Statement handle
 * @param name      - Bind name/literal position
 * @param data      - Input pointer to bind
 * @param type      - Input pointer type
 * @param ext_only  - is it an external  bind to check to input checking
 *
 * @note
 * Throws an exception if one of the parameters is invalid and then returns
 * FALSE.
 *
 */

#define OCI_CALL_CHECK_BIND(stmt, name, data, type, ext_only)                       \
                                                                                    \
    OCI_CALL_CHECK_PTR(OCI_IPC_STATEMENT, stmt)                                     \
    OCI_CALL_CHECK_PTR(OCI_IPC_STRING, name)                                        \
    OCI_CALL_CHECK_STMT_STATUS(stmt, OCI_STMT_PREPARED)                             \
    {                                                                               \
        const boolean ext_only_value = (ext_only);                                  \
        if ((ext_only_value) &&                                                     \
            (OCI_BAM_INTERNAL == (stmt)->bind_alloc_mode) &&                        \
            ((data) != NULL))                                                       \
        {                                                                           \
            THROW(ExceptionExternalBindingNotAllowed(stmt, name))     \
        }                                                                           \
                                                                                    \
        if ((ext_only_value) || OCI_BAM_EXTERNAL == (stmt)->bind_alloc_mode)        \
        {                                                                           \
            OCI_CALL_CHECK_PTR(type, data)                                          \
        }                                                                           \
    }                                                                               \

/**
 * @brief
 * Checks if the parameters of a register call are valid
 *
 * @param stmt  - Statement handle
 * @param name  - Bind name/literal position
 *
 * @note
 * Throws an exception if one of the parameters is invalid and returns FALSE.
 *
 */
#define OCI_CALL_CHECK_REGISTER(stmt, name)                                    \
                                                                               \
    OCI_CALL_CHECK_PTR(OCI_IPC_STATEMENT, stmt)                                \
    OCI_CALL_CHECK_PTR(OCI_IPC_STRING, name)                                   \

/* ********************************************************************************************* *
                        MISCELLANEOUS CHECKING MACROS
 * ********************************************************************************************* */

/**
 * @brief
 * Checks if an integer parameter value fits into the given bounds
 *
 * @param con - Connection handle
 * @param v   - Integer value
 * @param b1  - Lower bound
 * @param b2  - Upper bound
 *
 * @note
 * Throws an exception if the input value is out of bounds.
 *
 */

#define OCI_CALL_CHECK_BOUND(con, v, b1, b2)                                   \
                                                                               \
    if (((v) < (b1)) || ((v) > (b2)))                                          \
    {                                                                          \
        THROW(ExceptionOutOfBounds((con), (v)))                  \
    }

/**
 * @brief
 * Checks if an integer parameter value is >= minimum provided value
 *
 * @param con  - Connection handle
 * @param stmt - Statement handle
 * @param v    - Integer value
 * @param m    - Minimum value
 *
 * @note
 * Throws an exception if the input value is < < m.
 *
 */

#define OCI_CALL_CHECK_MIN(con, stmt, v, m)                                    \
                                                                               \
    if ((v) < (m))                                                             \
    {                                                                          \
        THROW(ExceptionMinimumValue((con), (stmt), (m)))         \
    }

/**
 * @brief
 * Checks if two expressions are compatible
 *
 * @param con - Connection handle
 * @param exp - Equality expression
 *
 * @note
 * Throws an exception if the 2 expressions are not compatible.
 *
 */

#define OCI_CALL_CHECK_COMPAT(con, exp)                                        \
                                                                               \
    if (!(exp))                                                                \
    {                                                                          \
        THROW(ExceptionTypeNotCompatible((con)))                 \
    }

/* ********************************************************************************************* *
                  INTERNAL STATES/ATTRIBUTES CHECKING MACROS
 * ********************************************************************************************* */

/**
 * @brief
 * Checks if the input OCILIB object was fetched and not discartable
 *
 * @param obj - OCILIB object handle
 * @param ret - Return value
 *
 * @note
 * Returns the value 'ret' if the object was fetched from a sql statement
 *
 */

#define OCI_CALL_CHECK_OBJECT_FETCHED(obj)                                    \
                                                                              \
    if (OCI_OBJECT_FETCHED_CLEAN == (obj)->hstate)                            \
        OCI_CALL_JUMP_EXIT()                                                   


/**
 * @brief
 * Checks if the status of a OCILIB statement matches the provided one
 *
 * @param st  - Statement handle
 * @param v   - Status to compare
 *
 * @note
 * Throws an exception if the status of the statement equals the provided one.
 *
 */

#define OCI_CALL_CHECK_STMT_STATUS(st, v)                                      \
                                                                               \
    if ((((st)->status) & (v)) == 0)                                           \
    {                                                                          \
        THROW(ExceptionStatementState((st), v))                  \
    }                                                                          \


/**
 * @brief
 * Checks if the given statement is scrollable
 *
 * @param st  - Statement handle
 *
 * @note
 * Throws an exception if the statement is not scrollable.
 *
 */

#define OCI_CALL_CHECK_SCROLLABLE_CURSOR_ACTIVATED(st)                         \
                                                                               \
    if (((st)->nb_rbinds > 0) ||                                               \
        ((st)->exec_mode != OCI_STMT_SCROLLABLE_READONLY))                     \
    {                                                                          \
        THROW(ExceptionStatementNotScrollable(st))               \
    }

/**
 * @brief
 * Checks if the status of a OCILIB direct path handle is compatible with the
 * given one
 *
 * @param dp  - Direct path handle
 * @param v   - Status to compare
 *
 * @note
 * Throws an exception if the status of the direct path handle is different than
 * the provided one.
 *
 */
#define OCI_CALL_CHECK_DIRPATH_STATUS(dp, v)                                   \
                                                                               \
    if ((dp)->status != (v))                                                   \
    {                                                                          \
        THROW(ExceptionDirPathState((dp), (dp)->status))         \
    }

/* ********************************************************************************************* *
                    INTERNAL FEATURES AVAILABILITY CHECKING MACROS
 * ********************************************************************************************* */

/**
 * @brief
 * Checks the library has been initialized
 *
 * @note
 * Returns 'ret' if the library has not been initialized
 *
 */

#define OCI_CALL_CHECK_INITIALIZED()                                           \
                                                                               \
    if (!OCILib.loaded)                                                        \
    {                                                                          \
        THROW(ExceptionNotInitialized())                         \
    }

/**
 * @brief
 * Internal check for various features
 *
 * @param con  - Connection handle
 * @param feat - Feature to check
 * @param ver  - OCI version that introduced the feature
 *
 * @note
 * Throws an exception the given feature is not available
 *
 */

#define OCI_CALL_CHECK_FEATURE(con, feat, ver)                                     \
                                                                                   \
    if (OCILib.version_runtime < (ver) || ((con) && (con)->ver_num < (ver)))       \
    {                                                                              \
        THROW(ExceptionNotAvailable(con, feat))                      \
    }

/**
 * @brief
 * Checks if multithreading mode is activated
 *
 * @note
 * Throws an exception the library has not been initialized with multithreading
 * mode
 *
 */

#define OCI_CALL_CHECK_THREAD_ENABLED()                                    \
                                                                           \
    if (!(OCI_LIB_THREADED))                                               \
    {                                                                      \
        THROW(ExceptionNotMultithreaded())                   \
    }

/**
 * @brief
 * Checks if the timestamp datatype is supported by the connection
 *
 * @param con - Connection handle
 *
 * @note
 * Throws an exception if the connection (client and server versions) does not
 * support timestamps
 *
 */

#define OCI_CALL_CHECK_TIMESTAMP_ENABLED(con)                                  \
                                                                               \
    OCI_CALL_CHECK_FEATURE(con, OCI_FEATURE_TIMESTAMP, OCI_9_0)  

/**
 * @brief
 * Checks if the interval datatype is supported by the connection
 *
 * @param con - Connection handle
 *
 * @note
 * Throws an exception if the connection (client and server versions) does not
 * support intervals
 *
 */

#define OCI_CALL_CHECK_INTERVAL_ENABLED OCI_CALL_CHECK_TIMESTAMP_ENABLED

/**
 * @brief
 * Checks if the connection supports scrollable cursors
 *
 * @param con - Connection handle
 *
 * @note
 * Throws an exception if the connection (client and server versions) does not
 * support scrollable cursors
 *
 */

#define OCI_CALL_CHECK_SCROLLABLE_CURSOR_ENABLED(con)                          \
                                                                               \
    OCI_CALL_CHECK_FEATURE(con, OCI_FEATURE_SCROLLABLE_CURSOR, OCI_9_0)


/**
* @brief
* Checks if the runtime OCI version supports PL SQL Boolean, Records and Index Tables
*
* @note
* Throws an exception if the connection (client and server versions) does not support it
*
*/

#define OCI_CALL_CHECK_EXTENDED_PLSQLTYPES_ENABLED(con)                        \
                                                                               \
    OCI_CALL_CHECK_FEATURE(con, OCI_FEATURE_EXTENDED_PLSQLTYPES, OCI_12_1)

/**
 * @brief
 * Checks if the runtime OCI client supports statement caching
 *
 * @note
 * Throws an exception if the OCI client does not
 * support statement caching
 *
 */

#define OCI_CALL_CHECK_STATEMENT_CACHING_ENABLED()                             \
                                                                               \
    if (OCILib.version_runtime < OCI_9_2)                                      \
    {                                                                          \
        THROW(ExceptionNotAvailable((dp)->con,                   \
                            OCI_FEATURE_STATEMENT_CACHING))                    \
    }

/**
 * @brief
 * Checks if the direct path date caching is available
 *
 * @param dp  - Direct path handle
 *
 * @note
 * Throws an exception if the Oracle client does not support date caching
 *
 */

#define OCI_CALL_CHECK_DIRPATH_DATE_CACHE_ENABLED(dp)                          \
                                                                               \
    if (OCILib.version_runtime < OCI_9_2)                                      \
    {                                                                          \
        THROW(ExceptionNotAvailable((dp)->con,                   \
                            OCI_FEATURE_DIRPATH_DATE_CACHE)                    \
    }

/**
 * @brief
 * Checks if the current OCI client supports remote database startup/shutdown
 *
 * @note
 * Throws an exception if the Oracle client is < 10g R2
 *
 */

#define OCI_CALL_CHECK_REMOTE_DBS_CONTROL_ENABLED()                            \
                                                                               \
    if (OCILib.version_runtime < OCI_10_2)                                     \
    {                                                                          \
        THROW(ExceptionNotAvailable(NULL,                        \
                            OCI_FEATURE_REMOTE_DBS_CONTROL))                   \
    }

/**
 * @brief
 * Checks if the current OCI client supports database notifications
 *
 * @note
 * Throws an exception if the Oracle client is < 10g R2
 *
 */

#define OCI_CALL_CHECK_DATABASE_NOTIFY_ENABLED()                               \
                                                                               \
    if (OCILib.version_runtime < OCI_10_2)                                     \
    {                                                                          \
        THROW(ExceptionNotAvailable(NULL,                        \
                            OCI_FEATURE_DATABASE_NOTIFY))                      \
    }

/**
 * @brief
 * Checks if the runtime OCI version supports Oracle high availability
 *
 * @note
 * Throws an exception if the client version does not
 * support Oracle high availability
 *
 */

#define OCI_CALL_CHECK_HIGH_AVAILABILITY_ENABLED()                             \
                                                                               \
    if (OCILib.version_runtime < OCI_10_2)                                     \
    {                                                                          \
        THROW(ExceptionNotAvailable(NULL,                        \
                            OCI_FEATURE_HIGH_AVAILABILITY))                    \
    }

/**
 * @brief
 * Checks if the OCILIB runtime version supports Oracle XA
 *
 * @note
 * Throws an exception if the OCILIB that not support XA connections
 *
 */

#define OCI_CALL_CHECK_XA_ENABLED(mode)                                        \
                                                                               \
    if ( ((mode) & OCI_SESSION_XA) && (!OCILib.use_xa) )                       \
    {                                                                          \
        THROW(ExceptionNotAvailable(NULL, OCI_FEATURE_XA))       \
    }

#define OCI_CALL_CHECK_ENUM_VALUE(con, stmt, mode, values, name)               \
    {                                                                          \
        size_t ii = 0, nn = sizeof(values) / sizeof((values)[0]);              \
        for (; ii < nn; ii++) { if ((mode) == (values)[ii]) break; }           \
        if (ii >= nn)                                                          \
        {                                                                      \
            THROW(ExceptionArgInvalidValue(con, stmt,            \
                                                         name, mode))          \
        }                                                                      \
    }



#endif    /* OCILIB_CHECKS_H_INCLUDED */


