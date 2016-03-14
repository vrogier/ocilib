/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2016 Vincent ROGIER <vince.rogier@ocilib.net>
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

#ifndef OCILIB_OCILIB_CHECKS_H_INCLUDED
#define OCILIB_OCILIB_CHECKS_H_INCLUDED

/* ********************************************************************************************* *
                        MACROS FOR CHECKING OCI CALLS
 * ********************************************************************************************* */

/**
 * @brief
 * Direct OCI call with return value checking
 *
 * @param res   - OCI call result
 * @param err   - OCI error handle
 * @param fct   - OCI function
 *
 * @note
 * Throws an exception on failure
 *
 */

#define OCI_CALL0(res, err, fct)                                               \
                                                                               \
    {                                                                          \
        (res) = (boolean) fct;                                                 \
        if (OCI_FAILURE((res)))                                                \
        {                                                                      \
            (res) = OCI_SUCCESS_WITH_INFO  == (res);                           \
            OCI_ExceptionOCI((err), NULL, NULL, res);                          \
        }                                                                      \
        else                                                                   \
            (res) = TRUE;                                                      \
    }

/**
 * @brief
 * Conditional OCI call with return value checking
 *
 * @param res   - OCI call result
 * @param con   - OCILIB connection objet
 * @param stmt  - OCILIB statement object
 * @param fct   - OCI function
 *
 * @note
 * Calls the OCI function only if the 'res' variable is TRUE
 * Throws an exception on failure.
 * Uses the OCI error handle of the connection object
 *
 */

#define OCI_CALL1(res, con, stmt, fct)                                         \
                                                                               \
    {                                                                          \
        if (res)                                                               \
        {                                                                      \
            (res) = (boolean) fct;                                             \
            if (OCI_FAILURE((res)))                                            \
            {                                                                  \
                (res) = OCI_SUCCESS_WITH_INFO  == (res);                       \
                OCI_ExceptionOCI((con)->err, (con), (stmt), res);              \
            }                                                                  \
            else                                                               \
                (res) = TRUE;                                                  \
        }                                                                      \
    }

/**
 * @brief
 * Conditional OCI call with return value checking
 *
 * @param res   - OCI call result
 * @param con   - OCILIB connection object
 * @param fct   - OCI function
 *
 * @note
 * Calls the OCI function only if the 'res' variable is TRUE
 * Throws an exception on failure.
 * Uses the OCI error handle of the connection object
 *
 */

#define OCI_CALL2(res, con, fct)                                               \
                                                                               \
    {                                                                          \
        if (res)                                                               \
        {                                                                      \
            (res) = (boolean) fct;                                             \
            if (OCI_FAILURE((res)))                                            \
            {                                                                  \
               (res) = OCI_SUCCESS_WITH_INFO  == (res);                        \
                OCI_ExceptionOCI((con)->err, (con), NULL, res);                \
            }                                                                  \
            else                                                               \
                (res) = TRUE;                                                  \
        }                                                                      \
    }

/**
 * @brief
 * Conditional OCI call with return value checking
 *
 * @param res   - OCI call result
 * @param err   - OCI error handle
 * @param fct   - OCI function
 *
 * @note
 * Throws an exception on failure
 *
 */

#define OCI_CALL3(res, err, fct)                                               \
                                                                               \
    {                                                                          \
        if (res)                                                               \
        {                                                                      \
            (res) = (boolean) fct;                                             \
            if (OCI_FAILURE((res)))                                            \
            {                                                                  \
                (res) = OCI_SUCCESS_WITH_INFO  == (res);                       \
                OCI_ExceptionOCI((err), NULL, NULL, res);                      \
            }                                                                  \
            else                                                               \
                (res) = TRUE;                                                  \
        }                                                                      \
    }

/**
 * @brief
 * Conditional OCI call with return value checking
 *
 * @param res   - OCI call result
 * @param err   - OCI error handle
 * @param con   - OCILIB connection objet
 * @param fct   - OCI function
 *
 * @note
 * Calls the OCI function only if the 'res' variable is TRUE
 * Throws an exception on failure.
 *
 */

#define OCI_CALL4(res, err, con, fct)                                          \
                                                                               \
    {                                                                          \
        if (res)                                                               \
        {                                                                      \
            (res) = (boolean) fct;                                             \
            if (OCI_FAILURE((res)))                                            \
            {                                                                  \
                (res) = OCI_SUCCESS_WITH_INFO  == (res);                       \
                OCI_ExceptionOCI((err), (con), NULL, res);                     \
            }                                                                  \
            else                                                               \
                (res) = TRUE;                                                  \
        }                                                                      \
    }

/**
 * @brief
 * Direct OCI call with return value checking

 * @param res   - OCI call result
 * @param con   - OCILIB connection objet
 * @param stmt  - OCILIB statement object
 * @param fct   - OCI function
 *
 * @note
 * Calls the OCI function only if the 'res' variable is TRUE
 * Throws an exception on failure.
 * Uses the OCI error handle of the connection object
 *
 */

#define OCI_CALL5(res, con, stmt, fct)                                         \
                                                                               \
    {                                                                          \
        (res) = (boolean) fct;                                                 \
        if (OCI_FAILURE((res)))                                                \
        {                                                                      \
            (res) = OCI_SUCCESS_WITH_INFO  == (res);                           \
            OCI_WarningOCI((con)->err, (con), (stmt), res);                    \
        }                                                                      \
        else                                                                   \
            (res) = TRUE;                                                      \
    }

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

#define OCI_CHECK_PTR(type, ptr)                                               \
                                                                               \
    if (!(ptr))                                                                \
    {                                                                          \
        OCI_ExceptionNullPointer(type);                                        \
        OCI_LIB_JUMP_EXIT()                                                    \
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

#define OCI_CHECK_BIND_CALL(stmt, name, data, type, ext_only)               \
                                                                            \
    OCI_CHECK_PTR(OCI_IPC_STATEMENT, stmt);                                 \
    OCI_CHECK_PTR(OCI_IPC_STRING, name);                                    \
    OCI_CHECK_STMT_STATUS(stmt, OCI_STMT_PREPARED);                         \
    {                                                                       \
        boolean ext_only_value = ext_only;                                  \
        if (ext_only_value || OCI_BAM_EXTERNAL == stmt->bind_alloc_mode)    \
        OCI_CHECK_PTR(type, data);                                          \
    }                                                                       \

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
#define OCI_CHECK_REGISTER_CALL(stmt, name)                                    \
                                                                               \
    OCI_CHECK_PTR(OCI_IPC_STATEMENT, stmt);                                    \
    OCI_CHECK_PTR(OCI_IPC_STRING, name);                                       \


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

#define OCI_CHECK_BOUND(con, v, b1, b2)                                        \
                                                                               \
    if ((v < (b1)) || (v > (b2)))                                              \
    {                                                                          \
        OCI_ExceptionOutOfBounds((con), (v));                                  \
        OCI_LIB_JUMP_EXIT()                                                    \
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

#define OCI_CHECK_MIN(con, stmt, v, m)                                         \
                                                                               \
    if ((v) < (m))                                                             \
    {                                                                          \
        OCI_ExceptionMinimumValue((con), (stmt), m);                           \
        OCI_LIB_JUMP_EXIT()                                                    \
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

#define OCI_CHECK_COMPAT(con, exp)                                             \
                                                                               \
    if (!(exp))                                                                \
    {                                                                          \
        OCI_ExceptionTypeNotCompatible((con));                                 \
        OCI_LIB_JUMP_EXIT()                                                    \
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

#define OCI_CHECK_OBJECT_FETCHED(obj)                                         \
                                                                              \
    if (OCI_OBJECT_FETCHED_CLEAN == (obj)->hstate)                            \
        OCI_LIB_JUMP_EXIT()                                                   


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

#define OCI_CHECK_STMT_STATUS(st, v)                                           \
                                                                               \
    if ((((st)->status) & (v)) == 0)                                           \
    {                                                                          \
        OCI_ExceptionStatementState((st), v);                                  \
        OCI_LIB_JUMP_EXIT()                                                    \
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

#define OCI_CHECK_SCROLLABLE_CURSOR_ACTIVATED(st)                              \
                                                                               \
    if (((st)->nb_rbinds > 0) ||                                               \
        ((st)->exec_mode != OCI_STMT_SCROLLABLE_READONLY))                     \
    {                                                                          \
        OCI_ExceptionStatementNotScrollable(st);                               \
        OCI_LIB_JUMP_EXIT()                                                    \
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
#define OCI_CHECK_DIRPATH_STATUS(dp, v)                                        \
                                                                               \
    if ((dp)->status != (v))                                                   \
    {                                                                          \
        OCI_ExceptionDirPathState((dp), (dp)->status);                         \
        OCI_LIB_JUMP_EXIT()                                                    \
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

#define OCI_CHECK_INITIALIZED()                                                \
                                                                               \
    if (!OCILib.loaded)                                                        \
    {                                                                          \
        OCI_ExceptionNotInitialized();                                         \
        OCI_LIB_JUMP_EXIT()                                                    \
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

#define OCI_CHECK_FEATURE(con, feat, ver)                                          \
                                                                                   \
    if (OCILib.version_runtime < ver || ((con) && (con)->ver_num < ver))           \
    {                                                                              \
        OCI_ExceptionNotAvailable(con, feat);                                      \
        OCI_LIB_JUMP_EXIT()                                                        \
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

#define OCI_CHECK_THREAD_ENABLED()                                         \
                                                                           \
    if (!(OCI_LIB_THREADED))                                               \
    {                                                                      \
        OCI_ExceptionNotMultithreaded();                                   \
        OCI_LIB_JUMP_EXIT()                                                \
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

#define OCI_CHECK_TIMESTAMP_ENABLED(con)                                       \
                                                                               \
    OCI_CHECK_FEATURE(con, OCI_FEATURE_TIMESTAMP, OCI_9_0)  

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

#define OCI_CHECK_INTERVAL_ENABLED OCI_CHECK_TIMESTAMP_ENABLED

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

#define OCI_CHECK_SCROLLABLE_CURSOR_ENABLED(con)                               \
                                                                               \
    OCI_CHECK_FEATURE(con, OCI_FEATURE_SCROLLABLE_CURSOR, OCI_9_0)


/**
* @brief
* Checks if the runtime OCI version supports PL SQL Boolean, Records and Index Tables
*
* @note
* Throws an exception if the connection (client and server versions) does not support it
*
*/

#define OCI_CHECK_EXTENDED_PLSQLTYPES_ENABLED(con)                             \
                                                                               \
    OCI_CHECK_FEATURE(con, OCI_FEATURE_EXTENDED_PLSQLTYPES, OCI_12_1)

/**
 * @brief
 * Checks if the runtime OCI client supports statement caching
 *
 * @note
 * Throws an exception if the OCI client does not
 * support statement caching
 *
 */

#define OCI_CHECK_STATEMENT_CACHING_ENABLED()                                  \
                                                                               \
    if (OCILib.version_runtime < OCI_9_2)                                      \
    {                                                                          \
        OCI_ExceptionNotAvailable((dp)->con, OCI_FEATURE_STATEMENT_CACHING);   \
        OCI_LIB_JUMP_EXIT()                                                    \
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

#define OCI_CHECK_DIRPATH_DATE_CACHE_ENABLED(dp)                               \
                                                                               \
    if (OCILib.version_runtime < OCI_9_2)                                      \
    {                                                                          \
        OCI_ExceptionNotAvailable((dp)->con, OCI_FEATURE_DIRPATH_DATE_CACHE);  \
        OCI_LIB_JUMP_EXIT()                                                    \
    }

/**
 * @brief
 * Checks if the current OCI client supports remote database startup/shutdown
 *
 * @note
 * Throws an exception if the Oracle client is < 10g R2
 *
 */

#define OCI_CHECK_REMOTE_DBS_CONTROL_ENABLED()                                 \
                                                                               \
    if (OCILib.version_runtime < OCI_10_2)                                     \
    {                                                                          \
        OCI_ExceptionNotAvailable(NULL, OCI_FEATURE_REMOTE_DBS_CONTROL);       \
        OCI_LIB_JUMP_EXIT()                                                    \
    }

/**
 * @brief
 * Checks if the current OCI client supports database notifications
 *
 * @note
 * Throws an exception if the Oracle client is < 10g R2
 *
 */

#define OCI_CHECK_DATABASE_NOTIFY_ENABLED()                                    \
                                                                               \
    if (OCILib.version_runtime < OCI_10_2)                                     \
    {                                                                          \
        OCI_ExceptionNotAvailable(NULL, OCI_FEATURE_DATABASE_NOTIFY);          \
        OCI_LIB_JUMP_EXIT()                                                    \
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

#define OCI_CHECK_HIGH_AVAILABILITY_ENABLED()                                  \
                                                                               \
    if (OCILib.version_runtime < OCI_10_2)                                     \
    {                                                                          \
        OCI_ExceptionNotAvailable(NULL, OCI_FEATURE_HIGH_AVAILABILITY);        \
        OCI_LIB_JUMP_EXIT()                                                    \
    }

/**
 * @brief
 * Checks if the OCILIB runtime version supports Oracle XA
 *
 * @note
 * Throws an exception if the OCILIB that not support XA connections
 *
 */

#define OCI_CHECK_XA_ENABLED(mode)                                             \
                                                                               \
    if ( (mode & OCI_SESSION_XA) && (!OCILib.use_xa) )                         \
    {                                                                          \
        OCI_ExceptionNotAvailable(NULL, OCI_FEATURE_XA);                       \
        OCI_LIB_JUMP_EXIT()                                                    \
    }

#define OCI_CHECK_ENUM_VALUE(con, stmt, mode, values, name)                    \
    {                                                                          \
        size_t ii = 0, nn = sizeof(values) / sizeof(values[0]);                \
        for (; ii < nn; ii++) { if (mode == values[ii]) break; }               \
        if (ii >= nn)                                                          \
        {                                                                      \
            OCI_ExceptionArgInvalidValue(con, stmt, name, mode);               \
            OCI_LIB_JUMP_EXIT()                                                \
        }                                                                      \
    }



#endif    /* OCILIB_OCILIB_CHECKS_H_INCLUDED */


