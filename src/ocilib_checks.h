/*
    +-----------------------------------------------------------------------------------------+
    |                                                                                         |
    |                               OCILIB - C Driver for Oracle                              |
    |                                                                                         |
    |                                (C Wrapper for Oracle OCI)                               |
    |                                                                                         |
    |                              Website : http://www.ocilib.net                            |
    |                                                                                         |
    |             Copyright (c) 2007-2010 Vincent ROGIER <vince.rogier@ocilib.net>            |
    |                                                                                         |
    +-----------------------------------------------------------------------------------------+
    |                                                                                         |
    |             This library is free software; you can redistribute it and/or               |
    |             modify it under the terms of the GNU Lesser General Public                  |
    |             License as published by the Free Software Foundation; either                |
    |             version 2 of the License, or (at your option) any later version.            |
    |                                                                                         |
    |             This library is distributed in the hope that it will be useful,             |
    |             but WITHOUT ANY WARRANTY; without even the implied warranty of              |
    |             MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU           |
    |             Lesser General Public License for more details.                             |
    |                                                                                         |
    |             You should have received a copy of the GNU Lesser General Public            |
    |             License along with this library; if not, write to the Free                  |
    |             Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.          |
    |                                                                                         |
    +-----------------------------------------------------------------------------------------+
*/

/* --------------------------------------------------------------------------------------------- *
 * $Id: ocilib_checks.h, v 3.8.1 2010-12-13 00:00 Vincent Rogier $
 * --------------------------------------------------------------------------------------------- */

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
        if (OCI_NO_ERROR((res)) == FALSE)                                      \
        {                                                                      \
            (res) = ((res) == OCI_SUCCESS_WITH_INFO);                          \
            OCI_ExceptionOCI((err), NULL, NULL, res);                          \
        }                                                                      \
        else                                                                                             \
            (res) = TRUE;                                                                                                          \
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
        if ((res) == TRUE)                                                     \
        {                                                                      \
            (res) = (boolean) fct;                                             \
            if (OCI_NO_ERROR((res)) == FALSE)                                  \
            {                                                                  \
                (res) = ((res) == OCI_SUCCESS_WITH_INFO);                      \
                OCI_ExceptionOCI((con)->err, (con), (stmt), res);              \
            }                                                                  \
            else                                                                                                 \
                (res) = TRUE;                                                                                                              \
        }                                                                      \
    }

/**
 * @brief
 * Conditional OCI call with return value checking
 *
 * @param res   - OCI call result
 * @param con   - OCILIB connection objet
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
        if ((res) == TRUE)                                                     \
        {                                                                      \
            (res) = (boolean) fct;                                             \
            if (OCI_NO_ERROR((res)) == FALSE)                                  \
            {                                                                  \
                (res) = ((res) == OCI_SUCCESS_WITH_INFO);                      \
                OCI_ExceptionOCI((con)->err, (con), NULL, res);                \
            }                                                                  \
            else                                                                                                 \
                (res) = TRUE;                                                                                                              \
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
        if ((res) == TRUE)                                                     \
        {                                                                      \
            (res) = (boolean) fct;                                             \
            if (OCI_NO_ERROR((res)) == FALSE)                                  \
            {                                                                  \
                (res) = ((res) == OCI_SUCCESS_WITH_INFO);                      \
                OCI_ExceptionOCI((err), NULL, NULL, res);                      \
            }                                                                  \
            else                                                                                                 \
                (res) = TRUE;                                                                                                              \
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
        if ((res) == TRUE)                                                     \
        {                                                                      \
            (res) = (boolean) fct;                                             \
            if (OCI_NO_ERROR((res)) == FALSE)                                  \
            {                                                                  \
                (res) = ((res) == OCI_SUCCESS_WITH_INFO);                      \
                OCI_ExceptionOCI((err), (con), NULL, res);                     \
            }                                                                  \
            else                                                                                                 \
                (res) = TRUE;                                                                                                              \
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
        if (OCI_NO_ERROR((res)) == FALSE)                                      \
        {                                                                      \
            (res) = ((res) == OCI_SUCCESS_WITH_INFO);                          \
            OCI_WarningOCI((con)->err, (con), (stmt), res);                    \
        }                                                                      \
        else                                                                                             \
            (res) = TRUE;                                                                                                          \
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

#define OCI_CHECK(exp, ret) if ((exp) == TRUE) return (ret);

/**
 * @brief
 * Checks if a pointer passed to public function is NULL
 *
 * @param type  - Pointer type
 * @param ptr   - Pointer
 * @param ret   - Return value
 *
 * @note
 * Throws an exception if the pointer is null.
 *
 */

#define OCI_CHECK_PTR(type, ptr, ret)                                          \
                                                                               \
    if ((ptr) == NULL)                                                         \
    {                                                                          \
        OCI_ExceptionNullPointer(type);                                        \
                                                                               \
        return (ret);                                                          \
    }

/**
 * @brief
 * Checks if the parameters of a bind call are valid
 *
 * @param stmt  - Statement handle
 * @param name  - Bind name/literal position
 * @param data  - Input pointer to bind
 * @param type  - Input pointer type
 *
 * @note
 * Throws an exception if one of the parameters is invalid and then returns
 * FALSE.
 *
 */

#define OCI_CHECK_BIND_CALL(stmt, name, data, type)                            \
                                                                               \
    OCI_CHECK_PTR(OCI_IPC_STATEMENT, stmt, FALSE);                             \
    OCI_CHECK_PTR(OCI_IPC_STRING, name, FALSE);                                \
    if (stmt->bind_alloc_mode == OCI_BAM_EXTERNAL)                                                                                                                                                                                      \
        OCI_CHECK_PTR(type, data, FALSE);                                                                                                                                                                    \


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
    OCI_CHECK_PTR(OCI_IPC_STATEMENT, stmt, FALSE);                             \
    OCI_CHECK_PTR(OCI_IPC_STRING, name, FALSE);                                \


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
 * @param ret - Return value
 *
 * @note
 * Throws an exception if the input value is out of bounds.
 *
 */

#define OCI_CHECK_BOUND(con, v, b1, b2, ret)                                   \
                                                                               \
    if ((v < (b1)) || (v > (b2)))                                              \
    {                                                                          \
        OCI_ExceptionOutOfBounds((con), (v));                                  \
                                                                               \
        return (ret);                                                          \
    }

/**
 * @brief
 * Checks if an integer parameter value is >= minimum provided value
 *
 * @param con  - Connection handle
 * @param stmt - Statement handle
 * @param v    - Integer value
 * @param m    - Minimum value
 * @param ret  - Return value
 *
 * @note
 * Throws an exception if the input value is < 1.
 *
 */

#define OCI_CHECK_MIN(con, stmt, v, m, ret)                                    \
                                                                               \
    if ((v) < (m))                                                             \
    {                                                                          \
        OCI_ExceptionMinimumValue((con), (stmt), m);                           \
                                                                               \
        return (ret);                                                          \
    }

/**
 * @brief
 * Checks if two expressions are compatible
 *
 * @param con - Connection handle
 * @param exp - Equality expression
 * @param ret - Return value
 *
 * @note
 * Throws an exception if the 2 expressions are not compatible.
 *
 */

#define OCI_CHECK_COMPAT(con, exp, ret)                                        \
                                                                               \
    if ((exp) == FALSE)                                                        \
    {                                                                          \
        OCI_ExceptionTypeNotCompatible((con));                                 \
                                                                               \
        return (ret);                                                          \
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

#define OCI_CHECK_OBJECT_FETCHED(obj, ret)                                     \
                                                                               \
    if ((obj)->hstate == OCI_OBJECT_FETCHED_CLEAN)                                                                                                                                   \
        return (ret);

/**
 * @brief
 * Checks if the status of a OCILIB statement matches the provided one
 *
 * @param st  - Statement handle
 * @param v   - Status to compare
 * @param ret - Return value
 *
 * @note
 * Throws an exception if the status of the statement equals the provided one.
 *
 */

#define OCI_CHECK_STMT_STATUS(st, v, ret)                                      \
                                                                               \
    if ((st)->status == (v))                                                   \
    {                                                                          \
        OCI_ExceptionStatementState((st), v);                                  \
        return ret;                                                            \
    }                                                                          \


/**
 * @brief
 * Checks if the given statement is scrollable
 *
 * @param st  - Statement handle
 * @param ret - Return value
 *
 * @note
 * Throws an exception if the statement is not scrollable.
 *
 */

#define OCI_CHECK_SCROLLABLE_CURSOR_ACTIVATED(st, ret)                         \
                                                                               \
    if (((st)->nb_rbinds > 0) ||                                               \
        ((st)->exec_mode != OCI_STMT_SCROLLABLE_READONLY))                     \
    {                                                                          \
        OCI_ExceptionStatementNotScrollable(st);                               \
        return ret;                                                            \
    }

/**
 * @brief
 * Checks if the status of a OCILIB direct path handle is compatible with the
 * given one
 *
 * @param st  - Direct path handle
 * @param v   - Status to compare
 * @param ret - Return value
 *
 * @note
 * Throws an exception if the status of the direct path handle is different than
 * the provided one.
 *
 */
#define OCI_CHECK_DIRPATH_STATUS(dp, v, ret)                                   \
                                                                               \
    if ((dp)->status != (v))                                                   \
    {                                                                          \
        OCI_ExceptionDirPathState((dp), (dp)->status);                         \
        return ret;                                                            \
    }

/* ********************************************************************************************* *
                    INTERNAL FEATURES AVAILABILITY CHECKING MACROS
 * ********************************************************************************************* */

/**
 * @brief
 * Checks the library has been initialized
 *
 * @param ret - Return value
 *
 * @note
 * Returns 'ret' if the library has not been initialized
 *
 */

#define OCI_CHECK_INITIALIZED(ret)                                             \
                                                                               \
    if (OCILib.loaded == FALSE)                                                \
    {                                                                          \
        OCI_ExceptionNotInitialized();                                         \
        return ret;                                                            \
    }

/**
 * @brief
 * Internal check for various features
 *
 * @param con  - Connection handle
 * @param feat - Feature to check
 * @param ver  - OCI version that introduced the feature
 * @param ret  - Return value
*
 * @note
 * Throws an exception the given feature is not available
 *
 */

#define OCI_CHECK_FEATURE(con, feat, ver,  ret)                                    \
                                                                                   \
    if (OCILib.version_runtime < ver || (((con) != NULL) && (con)->ver_num < ver)) \
    {                                                                              \
        OCI_ExceptionNotAvailable(con, feat);                                      \
        return ret;                                                                \
    }

/**
 * @brief
 * Checks if multithreading mode is activated
 *
 * @param ret - Return value
 *
 * @note
 * Throws an exception the library has not been initialized with multithreading
 * mode
 *
 */

#define OCI_CHECK_THREAD_ENABLED(ret)                                      \
                                                                           \
    if ((OCI_LIB_THREADED) == FALSE)                                       \
    {                                                                      \
        OCI_ExceptionNotMultithreaded();                                   \
        return ret;                                                        \
    }

/**
 * @brief
 * Checks if the timestamp datatype is supported by the connection
 *
 * @param con - Connection handle
 * @param ret - Return value
 *
 * @note
 * Throws an exception if the connection (client and server versions) does not
 * support timestamps
 *
 */

#define OCI_CHECK_TIMESTAMP_ENABLED(con,  ret)                                 \
                                                                               \
    OCI_CHECK_FEATURE(con, OCI_FEATURE_TIMESTAMP, OCI_9_0, ret)

/**
 * @brief
 * Checks if the interval datatype is supported by the connection
 *
 * @param con - Connection handle
 * @param ret - Return value
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
 * @param ret - Return value
 *
 * @note
 * Throws an exception if the connection (client and server versions) does not
 * support scrollable cursors
 *
 */

#define OCI_CHECK_SCROLLABLE_CURSOR_ENABLED(con, ret)                          \
                                                                               \
    OCI_CHECK_FEATURE(con, OCI_FEATURE_SCROLLABLE_CURSOR, OCI_9_0, ret)

/**
 * @brief
 * Checks if the direct path date caching is available
 *
 * @param con - Connection handle
 * @param ret - Return value
 *
 * @note
 * Throws an exception if the Oracle client does not support date caching
 *
 */

#define OCI_CHECK_DIRPATH_DATE_CACHE_ENABLED(dp,  ret)                         \
                                                                               \
    if (OCILib.version_runtime < OCI_9_2)                                      \
    {                                                                          \
        OCI_ExceptionNotAvailable((dp)->con, OCI_FEATURE_DIRPATH_DATE_CACHE);  \
        return ret;                                                            \
    }

/**
 * @brief
 * Checks if the current OCI client supports remote database startup/shutdown
 *
 * @param ret - Return value
 *
 * @note
 * Throws an exception if the Oracle client is < 10g R2
 *
 */

#define OCI_CHECK_REMOTE_DBS_CONTROL_ENABLED(ret)                              \
                                                                               \
    if (OCILib.version_runtime < OCI_10_2)                                     \
    {                                                                          \
        OCI_ExceptionNotAvailable(NULL, OCI_FEATURE_DIRPATH_DATE_CACHE);       \
        return ret;                                                            \
    }

/**
 * @brief
 * Checks if the current OCI client supports database notifications
 *
 * @param ret - Return value
 *
 * @note
 * Throws an exception if the Oracle client is < 10g R2
 *
 */

#define OCI_CHECK_DATABASE_NOTIFY_ENABLED(ret)                                 \
                                                                               \
    if (OCILib.version_runtime < OCI_10_2)                                     \
    {                                                                          \
        OCI_ExceptionNotAvailable(NULL, OCI_FEATURE_DATABASE_NOTIFY);          \
        return ret;                                                            \
    }

#endif    /* OCILIB_OCILIB_CHECKS_H_INCLUDED */

