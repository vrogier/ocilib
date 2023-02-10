/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2023 Vincent ROGIER <vince.rogier@ocilib.net>
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

/* IMPORTANT NOTICE
 *
 * This file contains explanations about Oracle and OCI technologies. 
 * OCILIB is a wrapper around OCI and thus exposes OCI features. 
 * The OCILIB documentation intends to explain Oracle / OCI concepts
 * and is naturally based on the official Oracle OCI documentation. 
 * 
 * Some parts of OCILIB documentation may include some information 
 * taken and adapted from the following Oracle documentations :
 *  - Oracle Call Interface Programmer's Guide
 *  - Oracle Streams - Advanced Queuing User's Guide
 */

#ifndef OCILIB_OCILIBC_API_H_INCLUDED
#define OCILIB_OCILIBC_API_H_INCLUDED

#include "ocilibc/types.h"

/**
 * @addtogroup OcilibCApi
 * @{
 *
 */
 
/**
 * @defgroup OcilibCApiInitialization Initializing the library
 * @{
 *
 * To use OCILIB, it first needs to be initialized through a call to OCI_Initialize().
 *
 * Then, the application connects to server, executes queries...
 *
 * Finally, OCILIB resources must be released by OCI_Cleanup()
 *
 * @note
 *
 * The following objects are automatically freed by the library:
 * - Connections
 * - pools
 * - Statements
 * - Type info objects
 * - Thread keys
 *
 * @warning
 *
 * All other standalone object instances (mutexes, threads, dates, lobs, ...) <b>ARE NOT</b> freed.
 *
 */

/**
 * @brief
 * Initialize the library
 *
 * @param err_handler  - Pointer to error handler procedure (optional)
 * @param lib_path     - Oracle shared library path (optional)
 * @param mode         - Environment mode
 *
 * Possible values for parameter mode:
 * - OCI_ENV_DEFAULT  : default mode
 * - OCI_ENV_THREADED : multi-threading support
 * - OCI_ENV_CONTEXT  : thread contextual error handling
 * - OCI_ENV_EVENTS   : enables events for subscription, HA Events, AQ notifications
 *
 * @note
 * This function must be called before any OCILIB library function.
 *
 * @warning
 * - The parameter 'libpath' is only used if OCILIB has been built with the option OCI_IMPORT_RUNTIME
 * - If the parameter 'lib_path' is NULL, the Oracle library is loaded from system environment variables
 *
 * @warning
 * OCI_Initialize() should be called <b>ONCE</b> per application
 *
 * @return
 * TRUE on success otherwise FALSE.
 * Possible reasons for failures:
 *  - when OCI_ErrorGetType() return OCI_ERR_ORACLE, OCI_ErrorGetOCICode() returns:
 *    - any ORA-XXXXXX error code. Refer to Oracle documentation
 *  - when OCI_ErrorGetType() return OCI_ERR_OCILIB, possible error code returned by OCI_ErrorGetInternalCode() 
 *    - OCI_ERR_LOADING_SHARED_LIB : OCILIB could not load oracle shared libraries at runtime (32/64bits mismatch, wrong \p lib_path, missing MSVC runtime required by oci.dll (MS Windows)
 *    - OCI_ERR_LOADING_SYMBOLS : the loaded shared library does not contain OCI symbols
 *    - OCI_ERR_NOT_AVAILABLE : OCILIb was built with OCI_CHARSET_WIDE and the oracle shared library dos not supports UTF16 (Oracle 8i)
 *    - OCI_ERR_CREATE_OCI_ENVIRONMENT: Oracle OCI environment initialization failed (in such cases, it is impossible to get the reason)
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_Initialize
(
    POCI_ERROR   err_handler,
    const otext *lib_path,
    unsigned int mode
);

/**
 * @brief
 * Clean up all resources allocated by the library
 *
 * @note
 * * This function must be the last OCILIB library function call.
 * - It deallocates objects not explicitly freed by the program (connections, statements, ...)
 * - It unloads the Oracle shared library if it has been dynamically loaded
 *
 * @warning
 * OCI_Cleanup() should be called <b>ONCE</b> per application
 *
 * @return TRUE
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_Cleanup
(
    void
);

/**
 * @brief
 * Return the version of OCI used for compilation
 *
 * @note
 * - with linkage build option, the version is determined from the oci.h header through different ways
 * - with runtime loading build option, the version is set to the highest version
 *   of OCI that is required by OCILIB to support all library features. Thus this version is just an indicator 
 *   as OCILIB is compatible with all Oracle 8+ OCI versions and adapt itself runtime loaded Oracle libraries
 *
 * @note
 *  Retun value is an integer using the following pattern 'MMmmRR'
 * - 'MM' digits are the Oracle OCI major version
 * - 'mm' digits are the Oracle OCI minor version
 * - 'RR' digits are the Oracle OCI revision version
 * 
 * @note 
 *  Manipulating direclty this value is not recommended.
 *  - For retrieving OCI major version from it, use the maro OCI_VER_MAJ()
 *  - For retrieving OCI minor version from it, use the maro OCI_VER_MIN()
 *  - For retrieving OCI revision version from it, use the maro OCI_VER_REV()
 * For example: if (OCI_VER_MAJ(OCI_GetOCICompileVersion()) >= 21) {...}
 * 
 * @note 
 * For testing this return value agains a specific specific version, use OCI_VER_MAKE()
 * For example, if OCI_GetOCICompileVersion() > OCI_VER_MAKE(21, 3, 0) {...}
 * 
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_GetOCICompileVersion
(
    void
);

/**
 * @brief
 * Return the version of OCI used at runtime
 *
 * @note
 * - with linkage build option, the version is determined from the oci.h header through different ways
 * - with runtime loading build option, the version determined from the symbols dynamically loaded. This
 *   is a best effort guess as a given Oracle release may not introduce new OCI symbols
 *
 * @note
 *  Retun value is an integer using the following pattern 'MMmmRR'
 * - 'MM' digits are the Oracle OCI major version
 * - 'mm' digits are the Oracle OCI minor version
 * - 'RR' digits are the Oracle OCI revision version
 * 
 * @note 
 *  Manipulating direclty this value is not recommended.
 *  - For retrieving OCI major version from it, use the maro OCI_VER_MAJ()
 *  - For retrieving OCI minor version from it, use the maro OCI_VER_MIN()
 *  - For retrieving OCI revision version from it, use the maro OCI_VER_REV()
 * For example: if (OCI_VER_MAJ(OCI_GetOCIRuntimeVersion()) >= 21) {...}
 * 
 * @note 
 * For testing this return value agains a specific specific version, use OCI_VER_MAKE()
 * For example, if OCI_GetOCIRuntimeVersion() > OCI_VER_MAKE(21, 3, 0) {...}
 * 
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_GetOCIRuntimeVersion
(
    void
);

/**
 * @brief
 * Return the Oracle shared library import mode
 *
 * @note
 * Possible values are:
 * - OCI_IMPORT_MODE_LINKAGE
 * - OCI_IMPORT_MODE_RUNTIME
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_GetImportMode
(
    void
);

/**
 * @brief
 * Return the OCILIB charset type
 *
 * @note
 * Possible values are:
 * - OCI_CHAR_ANSI
 * - OCI_CHAR_WIDE
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_GetCharset
(
    void
);

/**
* @brief
* Return the current number of bytes allocated internally in the library
*
* @param mem_type : type of memory to request
* 
* @note
* Possible values are:
* - OCI_MEM_ORACLE : bytes allocated by Oracle client library
* - OCI_MEM_OCILIB : bytes allocated by OCILIB library
* - OCI_MEM_ORACLE : bytes allocated by all libraries
*
*/

OCI_SYM_PUBLIC big_uint OCI_API OCI_GetAllocatedBytes
(
    unsigned int mem_type
);

/**
 * @brief
 * Enable or disable Oracle warning notifications
 *
 * @param value  - enable/disable warnings
 *
 * @note
 * Default value is FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_EnableWarnings
(
    boolean value
);

/**
 * @brief
 * Set the High availability (HA) user handler
 *
 * @param handler - Pointer to HA handler procedure
 *
 * @note
 * See POCI_HA_HANDLER documentation for more details
 *
 * @note
 * OCI_ENV_EVENTS flag must be passed to OCI_Initialize() to be able to use
 * HA events
 *
 * @warning
 * This call is supported from Oracle 10gR2.
 * For previous versions, it returns FALSE without throwing any exception.
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_SetHAHandler
(
    POCI_HA_HANDLER handler
);

/**
 * @} OcilibCApiInitialization
 */

/**
 * @defgroup OcilibCApiErrorHandling Error handling
 * @{
 *
 * OCILIB provides two mechanisms for error handling:
 *
 * - Global error handling through callbacks.
 * - Contextual thread error handling
 *
 * Exceptions are raised:
 *
 * - On Oracle OCI API call error
 * - On Oracle SQL statement error
 * - On Internal OCILIB error (type checking, memory allocations ...)
 * - On Oracle warnings (OCI API or SQL)
 *
 * If an error handler was provided to OCI_Initialize(), when an error occurs, the
 * library generates an OCI_Error handle and pass it to the error handler.
 *
 * In order to use the thread contextual error handling, you must call
 * OCI_Initialize() with the flag OCI_ENV_CONTEXT for the mode parameter. When
 * activated, error handles are stored per thread and the last error within a
 * thread can be retrieved with OCI_GetLastError()
 *
 * Exception properties are accessible through a set of functions
 *
 * @note
 * The two ways to handle errors are not exclusive and can be mixed.
 *
 * @note
 * Thread contextual error is also available for single thread based applications
 *
 * @par Oracle Warnings
 *
 * Oracle warnings are raised through OCI_Error API.
 * Such error handles have their error type property (OCI_ErrorGetType()) set to OCI_ERR_WARNING.
 * Warning handing is disabled by default. To activate/deactivate it, use OCI_EnableWarnings()
 *
 * @par Example with callbacks
 * @include err.c
 *
 * @par Example with thread context
 * @include err_ctx.c
 *
 * @par Example of warning handling
 * @include err_warning.c
 *
 */

/**
 * @brief
 * Set the global error user handler
 *
 * @param handler  - Pointer to error handler procedure
 *
 * @note
 * Use this call to change or remove the user callback error handler installed by OCI_Initialize()
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_SetErrorHandler
(
    POCI_ERROR handler
);

/**
 * @brief
 * Retrieve the last error or warning occurred within the last OCILIB call
 *
 * @note
 * OCI_GetLastError() is based on thread context and thus OCILIB must be
 * initialized with the flag OCI_ENV_CONTEXT
 *
 * @warning
 * OCILIB functions that returns a boolean value to indicate their success :
 * - return TRUE if no error occurred OR if a warning occurred
 * - return FALSE if an error occurred
 *
 */

OCI_SYM_PUBLIC OCI_Error * OCI_API OCI_GetLastError
(
    void
);

/**
 * @brief
 * Retrieve error message from error handle
 *
 * @param err - Error handle
 *
 */

OCI_SYM_PUBLIC const otext * OCI_API OCI_ErrorGetString
(
    OCI_Error *err
);

/**
 * @brief
 * Retrieve the type of error from error handle
 *
 * @param err - Error handle
 *
 * @note
 * Returns one of the following values:
 *
 * - OCI_ERR_ORACLE
 * - OCI_ERR_OCILIB
 * - OCI_ERR_WARNING
 *
 * @return
 * Object type or OCI_UNKNOWN the input handle is NULL
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_ErrorGetType
(
    OCI_Error *err
);

/**
 * @brief
 * Retrieve Oracle Error code from error handle
 *
 * @param err - Error handle
 *
 */

OCI_SYM_PUBLIC int OCI_API OCI_ErrorGetOCICode
(
    OCI_Error *err
);

/**
 * @brief
 * Retrieve Internal Error code from error handle
 *
 * @param err - Error handle
 *
 */

OCI_SYM_PUBLIC int OCI_API OCI_ErrorGetInternalCode
(
    OCI_Error *err
);

/**
 * @brief
 * Retrieve connection handle within the error occurred
 *
 * @param err - Error handle
 *
 */

OCI_SYM_PUBLIC OCI_Connection * OCI_API OCI_ErrorGetConnection
(
    OCI_Error *err
);

/**
 * @brief
 * Retrieve statement handle within the error occurred
 *
 * @param err - Error handle
 *
 * @note
 * If the error occurred outside of a statement context, it returns NULL
 *
 */

OCI_SYM_PUBLIC OCI_Statement * OCI_API OCI_ErrorGetStatement
(
    OCI_Error *err
);

/**
 * @brief
 * Return the row index which caused an error during statement execution
 *
 * @param err - Error handle
 *
 * @warning
 * Row index start at 1.
 *
 * @return
 * 0 is the error is not related to array DML otherwise the index of the given
 * row which caused the error
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_ErrorGetRow
(
    OCI_Error *err
);

/**
 * @brief
 * Return the location where the error occured
 *
 * @param err - Error handle
 *
 * @return
 * The method name that has generated the error
 *
 */

OCI_SYM_PUBLIC const otext * OCI_API OCI_ErrorGetLocation
(
    OCI_Error* err
);

/**
 * @} OcilibCApiErrorHandling
 */

/**
 * @defgroup OcilibCApiConnections Connecting to Database
 * @{
 *
 * Connecting to a database server is done with one call to OCI_ConnectionCreate().
 *
 * OCI_ConnectionFree() closes the established connection.
 *
 * Connection properties are accessible through a set of functions
 *
 * @par Example
 * @include conn.c
 *
 */

/**
 * @brief
 * Create a physical connection to an Oracle database server
 *
 * @param db   - Oracle Service Name
 * @param user - Oracle User name
 * @param pwd  - Oracle User password
 * @param mode - Session mode
 *
 * Possible values for parameter mode :
 * - OCI_SESSION_DEFAULT
 * - OCI_SESSION_SYSDBA
 * - OCI_SESSION_SYSOPER
 * - OCI_SESSION_XA
 *
 * @note
 * External credentials are supported by supplying a null value for the
 * 'user' and 'pwd' parameters.
 * If the param 'db' is NULL then a connection to the default local DB is done
 *
 * @note
 * For parameter 'mode', the possible values are exclusive and cannot be combined
 *
 * @par Oracle XA support
 *
 * OCILIB supports Oracle XA connectivity. In order to get a connection using
 * the XA interface :
 *  - For parameter 'db' : pass the value of the 'DB' parameter of the given
 *   XA connection string passed to the Transaction Processing Monitor (TPM)
 *  - Pass NULL to the 'user' and 'pwd' parameters
 *  - Pass the value OCI_SESSION_XA to parameter 'mode'
 *
 * @par Oracle XA Connection String
 *
 * The XA connection string used in a transaction monitor to connect to Oracle must
 * be compatible with OCILIB :
 *
 *   - the XA parameter 'Objects' MUST be set to 'true'
 *   - If OCI_ENV_THREADED is passed to OCI_Initialize(), the XA parameter 'Threads' must
 *     be set to 'true', otherwise to 'false'
 *   - If OCI_ENV_EVENTS is passed to OCI_Initialize(), the XA parameter 'Events' must
 *     be set to 'true', otherwise to 'false'
 *   - As Oracle does not support Unicode UTF16 character set through the XA interface,
 *     Only OCI_CHARSET_ANSI builds of OCILIB can be used
 *   - You still can use UTF8 if the NLS_LANG environment variable is set with a valid
 *     UTF8 NLS value
 *   - DO NOT USE OCI_CHARSET_WIDE OCILIB builds with XA connections
 *
 * @note
 * On success, a local transaction is automatically created and started ONLY for regular
 * standalone connections and connections retrieved from connection pools.
 * No transaction is created for a XA connection or q connection retrieved from session pools.
 *
 * @return
 * Connection handle on success or NULL on failure
 *
 */

OCI_SYM_PUBLIC OCI_Connection * OCI_API OCI_ConnectionCreate
(
    const otext *db,
    const otext *user,
    const otext *pwd,
    unsigned int mode
);

/**
 * @brief
 * Close a physical connection to an Oracle database server
 *
 * @param con - Connection handle
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_ConnectionFree
(
    OCI_Connection *con
);

/**
 * @brief
 * Returns TRUE is the given connection is still connected otherwise FALSE
 *
 * @param con - Connection handle
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_IsConnected
(
    OCI_Connection *con
);

/**
 * @brief
 * Return the pointer to user data previously associated with the connection
 *
 * @param con - Connection handle
 *
 * @note
 * Value NULL for parameter \p con is a valid value.
 * It returns then previously stored data with global scope (program wide)
 *
 */

OCI_SYM_PUBLIC void * OCI_API OCI_GetUserData
(
    OCI_Connection *con
);

/**
 * @brief
 * Associate a pointer to user data to the given connection
 *
 * @param con  - Connection handle
 * @param data - User data pointer
 *
 * @note
 * Value NULL for parameter \p con is a valid value.
 * It allows users to associate a pointer to user data with global scope (program wide)
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_SetUserData
(
    OCI_Connection *con,
    void *          data
);

/**
 * @brief
 * Associate a tag to the given connection/session
 *
 * @param con - Connection handle
 * @param tag - user tag string
 *
 * @note
 * Use this call only for connections retrieved from a session pool
 * See OCI_PoolGetConnection() for more details
 *
 * @note
 * To untag a session, call OCI_SetSessionTag() with 'tag' parameter set to NULL
 *
 * @warning
 * No error is raised if the connection is a standalone connection or retrieved from a connection
 * pool
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_SetSessionTag
(
    OCI_Connection *con,
    const otext *   tag
);

/**
 * @brief
 * Return the tag associated the given connection
 *
 * @param con - Connection handle
 *
 */

OCI_SYM_PUBLIC const otext * OCI_API OCI_GetSessionTag
(
    OCI_Connection *con
);

/**
 * @brief
 * Return the name of the connected database/service name
 *
 * @param con - Connection handle
 *
 */

OCI_SYM_PUBLIC const otext * OCI_API OCI_GetDatabase
(
    OCI_Connection *con
);

/**
 * @brief
 * Return the current logged user name
 *
 * @param con - Connection handle
 *
 */

OCI_SYM_PUBLIC const otext * OCI_API OCI_GetUserName
(
    OCI_Connection *con
);

/**
 * @brief
 * Return the current logged user password
 *
 * @param con - Connection handle
 *
 */

OCI_SYM_PUBLIC const otext * OCI_API OCI_GetPassword
(
    OCI_Connection *con
);

/**
 * @brief
 * Change the password of the logged user
 *
 * @param con      - Connection handle
 * @param password - New password
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_SetPassword
(
    OCI_Connection *con,
    const otext *   password
);

/**
 * @brief
 * Change the password of the given user on the given database
 *
 * @param db      - Oracle Service Name
 * @param user    - Oracle User name
 * @param pwd     - Oracle User password
 * @param new_pwd - Oracle User New password
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_SetUserPassword
(
    const otext *db,
    const otext *user,
    const otext *pwd,
    const otext *new_pwd
);

/**
 * @brief
 * Return the current session mode
 *
 * @param con - Connection handle
 *
 * @note
 * See OCI_ConnectionCreate() for possible values
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_GetSessionMode
(
    OCI_Connection *con
);

/**
 * @brief
 * Return the connected database server version string (aka server banner version)
 *  as reported by SQL*Plus
 *
 * @param con - Connection handle
 *
 */

OCI_SYM_PUBLIC const otext * OCI_API OCI_GetVersionServer
(
    OCI_Connection *con
);

/**
 * @brief
 * Return the major version number of the connected database server
 *
 * @param con - Connection handle
 *
 * @return
 * Version number or 0 on failure
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_GetServerMajorVersion
(
    OCI_Connection *con
);

/**
 * @brief
 * Return the minor version number of the connected database server
 *
 * @param con - Connection handle
 *
 * @return
 * Version number or 0 on failure
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_GetServerMinorVersion
(
    OCI_Connection *con
);

/**
 * @brief
 * Return the revision version number of the connected database server
 *
 * @param con - Connection handle
 *
 * @return
 * Version number or 0 on failure
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_GetServerRevisionVersion
(
    OCI_Connection *con
);

/**
 * @brief
 * Set the format string for implicit string conversions of the given type
 *
 * @param con    - Connection handle (optional)
 * @param type   - Type of format
 * @param format - Format string
 *
 * Formats can set at 2 levels:
 * - Library level: by passing a NULL Connection handle
 * - Connection level: by passing a valid Connection handle
 *
 * When the library needs to perform a string conversion, it search for a valid format using the
 * following order:
 * - Connection format
 * - Library format
 * - Default format
 *
 * @note
 * Possible values of parameter 'type' :
 *
 * - OCI_FMT_DATE          : format used to convert DATE to string
 * - OCI_FMT_TIMESTAMP     : format used to convert TIMESTAMP and TIMESTAMP WITH LOCAL TIMEZONE to string
 * - OCI_FMT_TIMESTAMP_TZ  : format used to convert TIMESTAMP WITH TIME ZONE to string
 * - OCI_FMT_NUMERIC       : format used to convert numeric types to string
 * - OCI_FMT_BINARY_DOUBLE : format used to convert BINARY_DOUBLE to string
 * - OCI_FMT_BINARY FLOAT  : format used to convert BINARY_FLOAT to string
 *
 * @note
 * Default format values are :
 * - OCI_FMT_DATE          : constant OCI_STRING_FORMAT_DATE
 * - OCI_FMT_TIMESTAMP     : constant OCI_STRING_FORMAT_TIMESTAMP
 * - OCI_FMT_TIMESTAMP_TZ  : constant OCI_STRING_FORMAT_TIMESTAMP_TZ
 * - OCI_FMT_NUMERIC       : constant OCI_STRING_FORMAT_NUMERIC
 * - OCI_FMT_BINARY_DOUBLE : constant OCI_STRING_FORMAT_BINARY_DOUBLE
 * - OCI_FMT_BINARY FLOAT  : constant OCI_STRING_FORMAT_BINARY_FLOAT
 *
 * @note
 * Conversions are performed by Oracle built-in functions whenever possible.
 * For DATE, TIMESTAMP and numeric types, see documentation of Oracle SQL to_char() function for more details
 * For BINARY_DOUBLE and BINARY_FLOAT, refer to the C Standard Library printf() family documentation
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_SetFormat
(
    OCI_Connection *con,
    unsigned int    type,
    const otext *   format
);

/**
 * @brief
 * Return the format string for implicit string conversions of the given type
 *
 * @param con    - Connection handle
 * @param type   - Type of format
 *
 * @note
 *  See OCI_SetFormat() for possible values
 *
 */

OCI_SYM_PUBLIC const otext * OCI_API OCI_GetFormat
(
    OCI_Connection *con,
    unsigned int    type
);

/**
 * @brief
 * Return the current transaction of the connection
 *
 * @param con - Connection handle
 *
 * @note
 * From v3.9.4, no more default transaction object is created for a new connection
 *
 */

OCI_SYM_PUBLIC OCI_Transaction * OCI_API OCI_GetTransaction
(
    OCI_Connection *con
);

/**
 * @brief
 * Set a transaction to a connection
 *
 * @param con   - Connection handle
 * @param trans - Transaction handle to assign
 *
 * @note
 * The current transaction (if any) is automatically stopped but the newly assigned is not
 * started or resumed
 *
 * @warning
 * Do not set transaction object to XA connection or connection retrieved from a session pool
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_SetTransaction
(
    OCI_Connection * con,
    OCI_Transaction *trans
);

/**
 * @brief
 * Return the highest Oracle version is supported by the connection
 *
 * @param con - connection handle
 *
 * @note
 * The highest supported version is the lower version between client and server:
 *
 * @note
 * Returns one of the following values:
 *
 * - OCI_UNKNOWN
 * - OCI_8_0
 * - OCI_8_1
 * - OCI_9_0
 * - OCI_9_2
 * - OCI_10_1
 * - OCI_10_2
 * - OCI_11_1
 * - OCI_11_2
 * - OCI_12_1
 * - OCI_18_3
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_GetVersionConnection
(
    OCI_Connection *con
);

/**
 * @brief
 * Set tracing information to the session of the given connection
 *
 * @param con   - connection handle
 * @param trace - trace type
 * @param value - trace content
 *
 * Store current trace information to the given connection handle.
 * These information:
 *
 * - is stored in system view V$SESSION and/or V$SQL_MONITOR
 * - can be retrieved from the connection property of an OCI_Error handle
 *
 * @note
 * Possible values of parameter 'trace' :
 *
 * - OCI_TRC_IDENTITY : Specifies the user defined identifier in the session.
 *                      It's recorded in the column CLIENT_IDENTIFIER of the
 *                      system view V$SESSION
 * - OCI_TRC_MODULE   : name of the current module in the client application.
 *                      It's recorded in the column MODULE of the
 *                      system view V$SESSION
 * - OCI_TRC_ACTION   : name of the current action within the current module.
 *                      It's recorded in the column ACTION of the
 *                      system view V$SESSION
 * - OCI_TRC_DETAIL   : Client application additional information.
 *                      It's recorded in the column CLIENT_INFO of the
 *                      system view V$SESSION
 * - OCI_TRC_OPERATION: Client application database operation.
 *                      It's recorded in the column DBOP_NAME of the
 *                      system view V$SQL_MONITOR
 * @warning
 * The system view V$SESSION is updated on Oracle versions >= 10gR1
 * The system view V$SQL_MONITOR is updated on Oracle versions >= 12cR1
 *
 * @warning
 * Oracle limits the size of these traces content: 
 *
 * - OCI_TRC_IDENTITY : 64 bytes
 * - OCI_TRC_MODULE   : 48 bytes
 * - OCI_TRC_ACTION   : 32 bytes
 * - OCI_TRC_DETAIL   : 64 bytes
 * - OCI_TRC_OPERATION: 32 bytes
 *
 * OCILIB truncates input values to match theses limits
 * 
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_SetTrace
(
    OCI_Connection *con,
    unsigned int    trace,
    const otext *   value
);

/**
 * @brief
 * Get the current trace for the trace type from the given connection.
 *
 * @param con   - connection handle
 * @param trace - trace type
 *
 * @note
 * See OCI_SetTrace() for more details.
 *
 */

OCI_SYM_PUBLIC const otext * OCI_API OCI_GetTrace
(
    OCI_Connection *con,
    unsigned int    trace
);

/**
 * @brief
 * Makes a round trip call to the server to confirm that the connection and the server are active.
 *
 * @param con - Connection handle
 *
 * @note
 * Returns TRUE is the connection is still alive otherwise FALSE
 *
 * @warning
 * This call is supported from Oracle 10g.
 * For previous versions, it returns FALSE without throwing any exception.
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_Ping
(
    OCI_Connection *con
);

/**
 * @brief
 * Set a given timeout for OCI calls that require server round-trips to the given database
 *
 * @param con   - Connection handle
 * @param type  - Type of timeout to set
 * @param value - Timeout in milliseconds
 *
 * Possible values for parameter 'type':
 * - OCI_NTO_SEND
 *   - Time to wait for send operations completion to the database server
 *   - Requires Oracle 12cR1 client
 * - OCI_NTO_RECEIVE
 *   - Time to wait for read operations completion from the database server
 *   - Requires Oracle 12cR1 client
 * - OCI_NTO_CALL
 *   - Time to wait for a database round-trip to complete ( Client processing is not taken into account)
 *   - Requires Oracle 18c client
 *
 * OCI client raises an timeout type related error when a given timeout is reached.
 * 
 * @note
 * To disable a given timeout, pass the value 0
 *
 * @warning
 * OCI client is using the following precedence rules when applying timeouts:
 *   - 1 - Timeout set using OCI_NTO_CALL (all other timeouts are discarded)
 *   - 2 - Timeouts set using OCI_NTO_SEND and/or OCI_NTO_RECEIVE
 *   - 3 - Timeouts set in sqlnet.ora file
 * 
 * Here is a summary:
 * 
 * FLAG            | Min. Version | OCI Error raised | OCI Error description          | sqlnet.ora equivalent |
 * --------------- | ------------ | ---------------- | ------------------------------ | --------------------- |
 * OCI_NTO_SEND    | OCI_12_1     | ORA-12608        | TNS: Send timeout occurred     | SQLNET.SEND_TIMEOUT   |
 * OCI_NTO_RECEIVE | OCI_12_1     | ORA-12609        | TNS: Receive timeout occurred  | SQLNET.RECV_TIMEOUT   | 
 * OCI_NTO_CALL    | OCI_18_1     | ORA-03136        | inbound connection timed out   |         ---           |  
 *
 * @warning
 * Returns FALSE without throwing any exception if the Oracle client does not support the given flag
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_SetTimeout
(
    OCI_Connection *con,
    unsigned int    type,
    unsigned int    value
);

/**
 * @brief
 * Returns the requested timeout value for OCI calls that require server round-trips to the given database
 *
 * @param con   - Connection handle
 * @param type  - Type of timeout
 *
 * @note:
 * See OCI_SetTimeout() for more information
 *
 * @return
 * The given timeout value if supported, otherwise 0
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_GetTimeout
(
    OCI_Connection *con,
    unsigned int    type
);

/**
 * @brief
 * Return the Oracle server database name of the connected database/service name
 *
 * @param con - Connection handle
 *
 * @warning
 * This call is supported from Oracle 10gR2.
 * For previous versions, it returns NULL without throwing any exception.
 *
 */

OCI_SYM_PUBLIC const otext * OCI_API OCI_GetDBName
(
    OCI_Connection *con
);

/**
 * @brief
 * Return the Oracle server Instance name of the connected database/service name
 *
 * @param con - Connection handle
 *
 * @warning
 * This call is supported from Oracle 10gR2.
 * For previous versions, it returns NULL without throwing any exception.
 *
 */

OCI_SYM_PUBLIC const otext * OCI_API OCI_GetInstanceName
(
    OCI_Connection *con
);

/**
 * @brief
 * Return the Oracle server service name of the connected database/service name
 *
 * @param con - Connection handle
 *
 * @warning
 * This call is supported from Oracle 10gR2.
 * For previous versions, it returns NULL without throwing any exception.
 *
 */

OCI_SYM_PUBLIC const otext * OCI_API OCI_GetServiceName
(
    OCI_Connection *con
);

/**
 * @brief
 * Return the Oracle server machine name of the connected database/service name
 *
 * @param con - Connection handle
 *
 * @warning
 * This call is supported from Oracle 10gR2.
 * For previous versions, it returns NULL without throwing any exception.
 *
 */

OCI_SYM_PUBLIC const otext * OCI_API OCI_GetServerName
(
    OCI_Connection *con
);

/**
 * @brief
 * Return the Oracle server domain name of the connected database/service name
 *
 * @param con - Connection handle
 *
 * @warning
 * This call is supported from Oracle 10gR2.
 * For previous versions, it returns NULL without throwing any exception.
 *
 */

OCI_SYM_PUBLIC const otext * OCI_API OCI_GetDomainName
(
    OCI_Connection *con
);

/**
 * @brief
 * Return the date and time (Timestamp) server instance start of the
 * connected database/service name
 *
 * @param con - Connection handle
 *
 * @warning
 * This call is supported from Oracle 10gR2.
 * For previous versions, it returns NULL without throwing any exception.
 *
 */

OCI_SYM_PUBLIC OCI_Timestamp * OCI_API OCI_GetInstanceStartTime
(
    OCI_Connection *con
);

/**
 * @brief
 * Verify if the given connection support TAF events
 *
 * @param con - Connection handle
 *
 * @note
 * Returns TRUE is the connection supports TAF event otherwise FALSE
 *
 * @warning
 * This call is supported from Oracle 10gR2.
 * For previous versions, it returns FALSE without throwing any exception.
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_IsTAFCapable
(
    OCI_Connection *con
);

/**
 * @brief
 * Set the Transparent Application Failover (TAF) user handler
 *
 * @param con     - Connection handle
 * @param handler - Pointer to TAF handler procedure
 *
 * @note
 * See POCI_TAF_HANDLER documentation for more details
 *
* @warning
 * This call is supported from Oracle 10gR2.
 * For previous versions, it returns FALSE without throwing any exception.
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_SetTAFHandler
(
    OCI_Connection * con,
    POCI_TAF_HANDLER handler
);

/**
 * @brief
 * Return the maximum number of statements to keep in the statement cache
 *
 * @param con  - Connection handle
 *
 * @note
 * Default value is 20 (value from Oracle Documentation)
 *
 * @warning
 * Requires Oracle Client 9.2 or above
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_GetStatementCacheSize
(
    OCI_Connection *con
);

/**
 * @brief
 * Set the maximum number of statements to keep in the statement cache
 *
 * @param con   - Connection handle
 * @param value - maximum number of statements in the cache
 *
 * @warning
 * Requires Oracle Client 9.2 or above
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_SetStatementCacheSize
(
    OCI_Connection *con,
    unsigned int    value
);

/**
 * @brief
 * Return the default LOB prefetch buffer size for the connection
 *
 * @param con  - Connection handle
 *
 * @warning
 * Requires Oracle Client AND Server 11gR1 or above
 *
 * @note
 * Prefetch size is:
 * - number of bytes for BLOBs and BFILEs
 * - number of characters for CLOBs.
 *
 * @note
 * Default is 0 (prefetching disabled)
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_GetDefaultLobPrefetchSize
(
    OCI_Connection *con
);

/**
 * @brief
 * Enable or disable prefetching for all LOBs fetched in the connection
 *
 * @param con   - Connection handle
 * @param value - default prefetch buffer size
 *
 * @note
 * If parameter 'value':
 * - is == 0, it disables prefetching for all LOBs fetched in the connection.
 * - is >  0, it enables prefetching for all LOBs fetched in the connection
 * and the given buffer size is used for prefetching LOBs
 *
 * @note
 * LOBs prefetching is disabled by default
 *
 * @warning
 * Requires Oracle Client AND Server 11gR1 or above.
 *
 * @note
 * Prefetch size is:
 * - number of bytes for BLOBs and BFILEs
 * - number of characters for CLOBs.
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_SetDefaultLobPrefetchSize
(
    OCI_Connection *con,
    unsigned int    value
);

/**
* @brief
* Return the maximum number of SQL statements that can be opened in one session
*
* @param con  - Connection handle
*
* @warning
* Requires Oracle Client AND Server 12cR1 or above
*
* @note
* the returned value is the same as the db parameter 'open_cursors' from server's parameter file
*
* @note
* Return 0 if the client and server version are < 12cR1
*
*/

OCI_SYM_PUBLIC unsigned int OCI_API OCI_GetMaxCursors
(
    OCI_Connection *con
);

/**
 * @} OcilibCApiConnections
 */

/**
 * @defgroup OcilibCApiPools Oracle Pools
 * @{
 *
 * OCILIB support the connections and sessions pooling features introduced
 * in Oracle 9i.
 *
 * Let's Oracle talk about this features !
 *
 * @par Connection pools (from Oracle Call Interface Programmer's Guide)
 *
 * Connection pooling is the use of a group (the pool) of reusable physical connections
 * by several sessions, in order to balance loads. The management of the pool is done
 * by OCI, not the application. Applications that can use connection pooling include
 * middle-tier applications for Web application servers and e-mail servers.
 *
 * @par Session Pools (from Oracle Call Interface Programmer's Guide)
 *
 * Session pooling means that the application will create and maintain a group of stateless
 * sessions to the database. These sessions will be handed over to thin clients as requested.
 * If no sessions are available, a new one may be created. When the client is done with
 * the session, the client will release it to the pool. Thus, the number of sessions in
 * the pool can increase dynamically.
 *
 * @note
 * OCILIB implements homogeneous session pools only.
 *
 * @par When using Pools (from Oracle Call Interface Programmer's Guide)
 *
 * If database sessions are not reusable by mid-tier threads (that is, they are stateful)
 * and the number of back-end server processes may cause scaling problems on the database,
 * use OCI connection pooling.
 *
 * If database sessions are reusable by mid-tier threads (that is, they are stateless)
 * and the number of back-end server processes may cause scaling problems on the database,
 * use OCI session pooling.
 *
 * If database sessions are not reusable by mid-tier threads (that is, they are stateful)
 * and the number of back-end server processes will never be large enough to potentially
 * cause any scaling issue on the database, there is no need to use any pooling mechanism.
 *
 * @par Example
 * @include pool.c
 *
 */

/**
 * @brief
 * Create an Oracle pool of connections or sessions
 *
 * @param db       - Oracle Service Name
 * @param user     - Oracle User name
 * @param pwd      - Oracle User password
 * @param type     - Type of pool
 * @param mode     - Session mode
 * @param min_con  - minimum number of  connections/sessions that can be opened.
 * @param max_con  - maximum number of  connections/sessions that can be opened.
 * @param incr_con - next increment for connections/sessions to be opened
 *
 * Possible values for parameter 'type':
 * - OCI_POOL_CONNECTION
 * - OCI_POOL_SESSION
 *
 * Possible values for parameter 'mode':
 * - OCI_SESSION_DEFAULT
 * - OCI_SESSION_SYSDBA (session pools only)
 *
 * @note
 * External credentials are supported by supplying a null value for the 'user'
 * and 'pwd' parameters
 * If the param 'db' is NULL then a connection to the default local DB is done
 *
 * @return
 * Connection or session pool handle on success or NULL on failure
 *
 */

OCI_SYM_PUBLIC OCI_Pool * OCI_API OCI_PoolCreate
(
    const otext *db,
    const otext *user,
    const otext *pwd,
    unsigned int type,
    unsigned int mode,
    unsigned int min_con,
    unsigned int max_con,
    unsigned int incr_con
);

/**
 * @brief
 * Destroy a pool object
 *
 * @param pool - Pool handle
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_PoolFree
(
    OCI_Pool *pool
);

/**
 * @brief
 * Get a connection from the pool
 *
 * @param pool - Pool handle
 * @param tag  - user tag string
 *
 * @par Session tagging
 *
 * Session pools have a nice feature that is 'session tagging'
 * It's possible to tag a session with a string identifier
 * when the session is returned to the pool, it keeps its tags.
 * When requesting a connection from the session pool, it's
 * possible to request a session that has the given 'tag' parameter
 * If one exists, it is returned. If not and if an untagged session
 * is available, it is then returned. So check the connection tag
 * property with OCI_GetSessionTag() to find out if the returned
 * connection is tagged or not.
 *
 * This features is described in the OCI developer guide as the following :
 *
 *  "The tags provide a way for users to customize sessions in the pool.
 *   A client may get a default or untagged session from a pool, set certain
 *   attributes on the session (such as NLS settings), and return the session
 *   to the pool, labeling it with an appropriate tag.
 *   The user may request a session with the same tags in order to have a
 *   session with the same attributes"
 *
 * @return
 * Connection handle otherwise NULL on failure
 */

OCI_SYM_PUBLIC OCI_Connection * OCI_API OCI_PoolGetConnection
(
    OCI_Pool *   pool,
    const otext *tag
);

/**
 * @brief
 * Get the idle timeout for connections/sessions in the pool
 *
 * @param pool - Pool handle
 *
 * @note
 * Connections/sessions idle for more than this time value (in seconds) is terminated
 *
 * @note
 * Timeout is not available for internal pooling implementation (client < 9i)
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_PoolGetTimeout
(
    OCI_Pool *pool
);

/**
 * @brief
 * Set the connections/sessions idle timeout
 *
 * @param pool  - Pool handle
 * @param value - Timeout value
 *
 * @note
 * connections/sessions idle for more than this time value (in seconds) is terminated
 *
 * @note
 * This call has no effect if pooling is internally implemented (client < 9i)
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_PoolSetTimeout
(
    OCI_Pool *   pool,
    unsigned int value
);

/**
 * @brief
 * Get the waiting mode used when no more connections/sessions are available
 * from the pool
 *
 * @param pool - Pool handle
 *
 * @return
 * - FALSE to wait for an available object if the pool is saturated
 * - TRUE to not wait for an available object
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_PoolGetNoWait
(
    OCI_Pool *pool
);

/**
 * @brief
 * Set the waiting mode used when no more connections/sessions are available
 * from the pool
 *
 * @param pool  - Pool handle
 * @param value - wait for object
 *
 * @note
 * Pass :
 * - FALSE to wait for an available object if the pool is saturated
 * - TRUE to not wait for an available object
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_PoolSetNoWait
(
    OCI_Pool *pool,
    boolean   value
);

/**
 * @brief
 * Return the current number of busy connections/sessions
 *
 * @param pool - Pool handle
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_PoolGetBusyCount
(
    OCI_Pool *pool
);

/**
 * @brief
 * Return the current number of opened connections/sessions
 *
 * @param pool - Pool handle
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_PoolGetOpenedCount
(
    OCI_Pool *pool
);

/**
 * @brief
 * Return the minimum number of connections/sessions that can be opened to the database
 *
 * @param pool - Pool handle
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_PoolGetMin
(
    OCI_Pool *pool
);

/**
 * @brief
 * Return the maximum number of connections/sessions that can be opened to the database
 *
 * @param pool - Pool handle
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_PoolGetMax
(
    OCI_Pool *pool
);

/**
 * @brief
 * Return the increment for connections/sessions to be opened to the database when the pool is
 * not full
 *
 * @param pool - Pool handle
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_PoolGetIncrement
(
    OCI_Pool *pool
);

/**
 * @brief
 * Return the maximum number of statements to keep in the pool statement cache
 *
 * @param pool - Pool handle
 *
 * @note
 * Default value is 20 (value from Oracle Documentation)
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_PoolGetStatementCacheSize
(
    OCI_Pool *pool
);

/**
 * @brief
 * Set the maximum number of statements to keep in the pool statement cache
 *
 * @param pool  - Pool handle
 * @param value - maximum number of statements in the cache
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_PoolSetStatementCacheSize
(
    OCI_Pool *   pool,
    unsigned int value
);

/**
 * @} OcilibCApiPools
 */

/**
 * @defgroup OcilibCApiTransactions Managing transactions
 * @{
 *
 * OCILIB supports local and global transactions.
 *
 * Local transactions are implicit within connection objects and there is no
 * specific call or programming step for using it.
 *
 * In order to control changes made in the database:
 *
 * - OCI_Commit() validates current pending modifications
 * - OCI_Rollback() discards current pending modifications
 *
 * OCILIB supports a feature called 'Auto Commit' that performs an implicit and
 * automatic commit call after every execute call
 *
 * @note
 * Those actions are executed within a connection context and not directly to a transaction.
 *
 * @warning
 * Global transactions are optional and are designed for distributed or global
 * transaction environments.
 *
 * OCILIB supports them by :
 *
 * - Creating/Destroying explicitly a transaction object
 * - Starting/Stopping/Resuming explicitly the transaction
 * - Preparing the transaction for specific calls
 *
 */

/**
 * @brief
 * Commit current pending changes
 *
 * @param con - Connection handle
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_Commit
(
    OCI_Connection *con
);

/**
 * @brief
 * Cancel current pending changes
 *
 * @param con - Connection handle
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_Rollback
(
    OCI_Connection *con
);

/**
 * @brief
 * Enable / disable auto commit mode
 *
 * The auto commit mode allows commit changes after every executed SQL order
 *
 * @param con    - Connection handle
 * @param enable - Enable (TRUE) or disable (FALSE)
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_SetAutoCommit
(
    OCI_Connection *con,
    boolean         enable
);

/**
 * @brief
 * Get current auto commit mode status
 *
 * @param con - Connection handle
 *
 * @return
 * TRUE if auto commit mode is activated otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_GetAutoCommit
(
    OCI_Connection *con
);

/**
 * @brief
 * Create a new global transaction or a serializable/read-only local transaction
 *
 * @param con     - Connection handle
 * @param timeout - Time that a transaction stays inactive after being stopped
 * @param mode    - Transaction mode
 * @param pxid    - pointer to a global transaction identifier structure
 *
 *
 * @note
 * The parameter 'mode' can be one of the following values :
 *
 * - Global transactions:
 *      - OCI_TRS_NEW : By default starts a new, tightly coupled and
 *                      migratable branch.
 *      - OCI_TRS_TIGHT : explicitly specifies a tightly coupled branch
 *      - OCI_TRS_LOOSE : specifies a loosely coupled branch
 *
 * - Global and local transactions :
 *      - OCI_TRS_READONLY - start a read-only transaction
 *      - OCI_TRS_READWRITE - start a read-write transaction
 *      - OCI_TRS_SERIALIZABLE : start a serializable transaction
 *
 * @note
 * For local transaction:
 * - pass a NULL value for pxid
 *
 */

OCI_SYM_PUBLIC OCI_Transaction * OCI_API OCI_TransactionCreate
(
    OCI_Connection *con,
    unsigned int    timeout,
    unsigned int    mode,
    OCI_XID *       pxid
);

/**
 * @brief
 * Free current transaction
 *
 * @param trans - Connection handle
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_TransactionFree
(
    OCI_Transaction *trans
);

/**
 * @brief
 * Start global transaction
 *
 * @param trans - Connection handle
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_TransactionStart
(
    OCI_Transaction *trans
);

/**
 * @brief
 * Stop current global transaction
 *
 * @param trans - Connection handle
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_TransactionStop
(
    OCI_Transaction *trans
);

/**
 * @brief
 * Resume a stopped global transaction
 *
 * @param trans - Global transaction handle
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_TransactionResume
(
    OCI_Transaction *trans
);

/**
 * @brief
 * Prepare a global transaction validation
 *
 * @param trans - Global transaction handle
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_TransactionPrepare
(
    OCI_Transaction *trans
);

/**
 * @brief
 * Cancel the prepared global transaction validation
 *
 * @param trans - Global transaction handle
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_TransactionForget
(
    OCI_Transaction *trans
);

/**
 * @brief
 * Return global transaction mode.
 *
 * @note:
 * see OCI_TransactionCreate() for possible values
 *
 * @param trans - Global transaction handle
 *
 * @return
 * Transaction mode or OCI_UNKNOW if trans is NULL
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_TransactionGetMode
(
    OCI_Transaction *trans
);

/**
 * @brief
 * Return global transaction Timeout
 *
 * @param trans - Global transaction handle
 *
 * @return
 * Transaction timeout or 0 if trans is NULL
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_TransactionGetTimeout
(
    OCI_Transaction *trans
);

/**
 * @} OcilibCApiTransactions
 */

/**
 * @defgroup OcilibCApiStatements Executing statements
 * @{
 *
 * Executing SQL statements or PL/SQL blocks is really simple with OCILIB.
 *
 * First, call OCI_StatementCreate() to allocate a statement handle. Then :
 *
 * - Prepare the SQL with OCI_Prepare()
 * - Parse and execute it with OCI_Execute()
 *
 * These two steps can be done together by calling OCI_ExecuteStmt() that
 * prepares and executes in one go.
 *
 * To find out if the statement has affected any rows, call OCI_GetAffectedRows()
 *
 * Finally, release the statement and its resources with OCI_StatementFree()
 *
 * @note
 * A statement can be prepared once and executed as many times as needed (see
 * Binding variables section)
 *
 * @note
 * An OCI_Statement can be used to prepare and/or execute different SQL and PL/SQL
 * statements as many times as needed.
 * For example, if the SQL processing of an application is sequential, only
 * one statement handle is required
 *
 * @note
 * OCILIB supports nested levels of SQL statement processing.
 * An application can loop through the resultset of the statement handle A,
 * executing statement B and fetching statement C at every loop, and so on ...
 *
 * @par Example
 * @include exec.c
 *
 */

/**
 * @brief
 * Create a statement object and return its handle
 *
 * @param con - Connection handle
 *
 * @return
 * A statement handle on success otherwise NULL
 *
 */

OCI_SYM_PUBLIC OCI_Statement * OCI_API OCI_StatementCreate
(
    OCI_Connection *con
);

/**
 * @brief
 * Free a statement and all resources associated to it (resultsets ...)
 *
 * @param stmt - Connection handle
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_StatementFree
(
    OCI_Statement *stmt
);

/**
 * @brief
 * Prepare a SQL statement or PL/SQL block.
 *
 * @param stmt - Statement handle
 * @param sql  - SQL order or PL/SQL block
 *
 * @note
 * Do not call this function for fetched statements (REF cursors)
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_Prepare
(
    OCI_Statement *stmt,
    const otext *  sql
);

/**
 * @brief
 * Execute a prepared SQL statement or PL/SQL block.
 *
 * @param stmt - Statement handle
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 * @warning
 * If a SQL warning occurs:
 * - the function returns TRUE
 * - the SQL warning triggers the global error handler with an OCI_Error having its OCI_ErrorGetType()
 *   attribute set to OCI_ERR_WARNING
 * - If OCILIB is initialized with the OCI_ENV_CONTEXT mode, OCI_GetLastError() will return the OCI_Error
 *   object corresponding to the warning
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_Execute
(
    OCI_Statement *stmt
);

/**
 * @brief
 * Prepare and Execute a SQL statement or PL/SQL block.
 *
 * @param stmt - Statement handle
 * @param sql  - SQL order - PL/SQL block
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 * @warning
 * If a SQL warning occurs:
 * - the function returns TRUE
 * - the SQL warning triggers the global error handler with an OCI_Error having its OCI_ErrorGetType()
 *   attribute set to OCI_ERR_WARNING
 * - If OCILIB is initialized with the OCI_ENV_CONTEXT mode, OCI_GetLastError() will return the OCI_Error
 *   object corresponding to the warning
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_ExecuteStmt
(
    OCI_Statement *stmt,
    const otext *  sql
);

/**
 * @brief
 * Parse a SQL statement or PL/SQL block.
 *
 * @param stmt - Statement handle
 * @param sql  - SQL order - PL/SQL block
 *
 * @note
 * This call sends the SQL or PL/SQL command to the server for parsing only.
 * The command is not executed.
 * This call is only useful to check is a command is valid or not.
 *
 * @note
 * This call prepares the statement (internal call to OCI_Prepare()) and ask
 * the Oracle server to parse its SQL or PL/SQL command.
 * OCI_Execute() can be call after OCI_Parse() in order to execute the
 * statement, which means that the server will re-parse again the command.
 *
 * @warning
 * Do not use OCI_Parse() unless you're only interested in the parsing result
 * because the statement will be parsed again when executed and thus leading to
 * unnecessary server round-trips and less performance
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_Parse
(
    OCI_Statement *stmt,
    const otext *  sql
);

/**
 * @brief
 * Describe the select list of a SQL select statement.
 *
 * @param stmt - Statement handle
 * @param sql  - SELECT sql statement
 *
 * @note
 * This call sends the SELECT SQL order to the server for retrieving the
 * description of the select order only.
 * The command is not executed.
 * This call is only useful to retrieve information on the associated resultset
 * Call OCI_GetResultet() after OCI_Describe() to access to SELECT list
 * information
 *
 * @note
 * This call prepares the statement (internal call to OCI_Prepare()) and ask
 * the Oracle server to describe the output SELECT list.
 * OCI_Execute() can be called after OCI_Describe() in order to execute the
 * statement, which means that the server will parse, and describe again the SQL
 * order.
 *
 * @warning
 * Do not use OCI_Describe() unless you're only interested in the resultset
 * information because the statement will be parsed again when executed and thus
 * leading to unnecessary server round-trips and less performance
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_Describe
(
    OCI_Statement *stmt,
    const otext *  sql
);

/**
 * @brief
 * Return the last SQL or PL/SQL statement prepared or executed by the statement
 *
 * @param stmt - Statement handle
 *
 */

OCI_SYM_PUBLIC const otext * OCI_API OCI_GetSql
(
    OCI_Statement *stmt
);

/**
* @brief
* Returns the statement SQL_ID from the server
*
* @param stmt - Statement handle
*
* @note
* The statement must be executed first
*
* @warning
* Requires Oracle 12cR2 (both client and server side), otherwise it returns NULL
* 
*/

OCI_SYM_PUBLIC const otext * OCI_API OCI_GetSqlIdentifier
(
    OCI_Statement *stmt
);

/**
 * @brief
 * Return the error position (in terms of characters) in the SQL statement
 * where the error occurred in case of SQL parsing error
 *
 * @param stmt - Statement handle
 *
 * @note
 * Positions start at 1.
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_GetSqlErrorPos
(
    OCI_Statement *stmt
);

/**
 * @brief
 * Return the number of rows affected by the SQL statement
 *
 * @param stmt - Statement handle
 *
 * The returned value is :
 *  - For UPDATEs : number of rows updated
 *  - For INSERTs : number of rows inserted
 *  - For DELETEs : number of rows deleted
 *
 * @note
 * For SELECTs statements, use OCI_GetRowCount() instead
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_GetAffectedRows
(
    OCI_Statement *stmt
);

/**
 * @brief
 * Return the Oracle SQL code the command held by the statement handle
 *
 * @param stmt - Statement handle
 *
 * @warning
 * OCI_GetSQLCommand() must be called after the statement has be executed
 * because that's the server engine that computes the SQL command code
 *
 * @return
 * The SQL command code of the statement otherwise OCI_UNKOWN
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_GetSQLCommand
(
    OCI_Statement *stmt
);

/**
 * @brief
 * Return the verb of the SQL command held by the statement handle
 *
 * @param stmt - Statement handle
 *
 * @warning
 * OCI_GetSQLVerb() must be called after the statement has been executed
 * because that's the server engine that computes the SQL verb
 *
 * @note
 * The SQL verb list is available in Oracle documentations and guides
 *
 * @return
 * The SQL command verb of the statement otherwise NULL
 */

OCI_SYM_PUBLIC const otext * OCI_API OCI_GetSQLVerb
(
    OCI_Statement *stmt
);

/**
 * @} OcilibCApiStatements
 */

/**
 * @defgroup OcilibCApiBinding Binding variables and arrays
 * @{
 *
 * OCILIB supports OCI data binding APIs
 *
 * Programs variables can be binded to an Oracle SQL PL/SQL statement in order to :
 *
 * - Provide input data for SQL statement
 * - Provide input/output data for PL/SQL blocks
 *
 * OCILIB provides a set of binding functions to use with:
 *
 * - Basic data types: string (char/wchar_t *), int, float, double, raw
 * - Object data types: lobs, files,longs, dates, cursors, statements,
 *                      timestamps, intervals, objects
 *  
 * To use binding:
 *
 * - Prepare a statement with OCI_Prepare() (see Executing statements)
 * - Bind variables by calling one if the OCI_Bindxxxxx() function for every
 *   input variable referenced by the SQL statement
 * - Setup up values of the program variables
 * - Call OCI_Execute() as many times as needed
 * - Each OCI_Execute() call may be preceded by an update of the program
 *   variables (for INSERTs for example)
 *
 * @warning
 * Between re-execution of same SQL statetement, use OCI_BindSetNullAtPos() / OCI_BindSetNull() and OCI_BindSetNullAtPos() / OCI_BindSetNotNullAtPos()
 * to update the NULL / NOT NULL status of host variables
 *
 * Bindings can be:
 *  - IN (host variable are not used anymore after statement execution)
 *  - OUT (host variable are set during statement execution)
 *  - IN/OUT (default)
 * Use OCI_BindSetDirectionTo() to change a host variable bind direction mode after the binding call but before statement execution. 
 * Note that each direction mode may have a little overhead depending on the SQL type as OCILIB may have to do checks/conversions/mappings between host variable and buffers.
 * Thus, to maximize performances:
 *  - set direction mode to OCI_BDM_IN  if host variable is not updated by statement execution
 *  - set direction mode to OCI_BDM_OUT if host variable value does not matter prior to statement execution
 *  - set direction mode to OCI_BDM_IN_OUT when host variable value is used for execution and updated by statement execution
 *  
 * OCILIB supports the OCI array Interface by binding arrays of C scalar types
 * and OCILIB object types.
 *
 * - all types supported the library can be used for array binding except
 *   OCI_Statement and OCI_Long
 * - Array binding is really fast for massive DML operations
 * - For string/RAW arrays, the input array MUST BE a contiguous block of data
 *   and not an array of pointers. So to bind an array of 10 elements for a
 *   varchar2(30) column, binded variable must be a like array[10][31]
 *
 * OCILIB does not pre-parse statements (like other frameworks such as JDBC, ...)
 * and lets Oracle recognize input variables embedded within the SQL statements.
 *
 * Bind variables must be preceded in the SQL code by the character ':'.
 *
 * Oracle and OCILIB supports two ways of binding:
 *
 * - by name (default mode in OCILIB): Oracle looks for variables in the SQL
 *   statement by searching their names provided to the binding function.
 *   So a variable can be binded once and used many times in the statement
 * - by position: Oracle binds variables by position, so every variable is
 *   binded with a position number
 *
 * OCILIB Default binding mode is OCI_BIND_BY_NAME.
 *
 * When using binding by position, provide the position to OCI_BindXXXX() call
 * through the name parameter. Within this mode the bind name must be the
 * position preceded by a semicolon like ':1', ':2', ....
 *
 * @par Internal Bind allocation mode
 *
 * Bind variables or arrays can be internally allocated by OCILIB. 
 * That means that instead of allocating variables or arrays on the stack/heap
 * in the user program, bind contents can be allocated internally and thus :
 * - minimize the amount of program variables
 * - optimize internal memory management for arrays
 *
 * To do so :
 * - Call OCI_SetBindAllocation() with the mode OCI_BAM_INTERNAL
 * - pass a NULL variable or array to OCI_BindXXX() calls
 * - Retrieve the bind content allocated by OCILIB with OCI_BindGetData()
 *
 * Internal Bind allocation mode IS compatible with ALL array binding OCI_BindArrayOfxxx() methods.
 *
 * Internal Bind allocation mode IS NOT compatible with some single variable bind calls :
 * - OCI_BindTimestamp()
 * - OCI_BindInterval()
 * - OCI_BindLob()
 * - OCI_BindFile()
 * - OCI_BindObject()
 * - OCI_BindColl()
 * - OCI_BindRef()
 * - OCI_BindStatement()
 * - OCI_BindLong()
 *
 * These methods need to know the data sub type (like OCI_CLOB/OCI_BLOB for lobs) in order
 * to internally create variables. As these methods prototypes are not passing the sub type,
 * calling them with the statement bind mode set to OCI_BAM_INTERNAL will raise
 * an OCILIB error of type OCI_ERR_NULL_POINTER
 *
 * @note
 * Rebinding is disabled by default (see OCI_AllowRebinding())
 * When using rebinding feature, host variable re-binded to a previously allocated
 * bind MUST be of the SAME data type !
 *
 * @par Basic input bind Example
 * @include bind.c
 *
 * @par Array interface Example
 * @include array.c
 *
 * @par Internal Array interface Example
 * @include array_internal.c
 *
 * */

/**
 * @brief
 * Set the input array size for bulk operations
 *
 * @param stmt - Statement handle
 * @param size - Array size
 *
 * @warning
 * Do not use OCI_BindArraySetSize() for PL/SQL tables binding
 *
 * @note
 * OCI_BindArraySetSize() is used to set the size of input bind array when using
 * arrays for DML statements.
 * OCI_BindArraySetSize() MUST be called to set the maximum size of the arrays
 * to bind to the statement before any of its execution. This initial call must
 * be bone AFTER OCI_Prepare() and BEFORE any OCI_BindArrayOfxxx() call.
 *
 * @note
 * OCI_BindArraySetSize() can optionally be called before any later OCI_Execute()
 * call in order to notify the statement of the exact number of elements
 * populating the input arrays for the next execution. The array size passed to
 * later OCI_BindArraySetSize() calls cannot be greater than the initial size
 * otherwise an exception will be thrown.
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_BindArraySetSize
(
    OCI_Statement *stmt,
    unsigned int   size
);

/**
 * @brief
 * Return the current input array size for bulk operations
 *
 * @param stmt - Statement handle
 *
 * @return
 * Array size value or 0 if OCI_BindArraySetSize() has not been called
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_BindArrayGetSize
(
    OCI_Statement *stmt
);

/**
 * @brief
 * Allow different host variables to be binded using the same bind name or
 * position between executions of a prepared statement
 *
 * @param stmt  - Statement handle
 * @param value - Rebinding mode allowed
 *
 * @note
 * Default value is FALSE
 *
 * @warning
 * When using rebinding feature, host variable re-binded to a previously allocated
 * bind MUST be of the same data type !
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_AllowRebinding
(
    OCI_Statement *stmt,
    boolean        value
);

/**
 * @brief
 * Indicate if rebinding is allowed on the given statement
 *
 * @param stmt - Statement handle
 *
 * @note
 * See OCI_AllowRebinding() for more details
 *
 * @return
 * TRUE if allowed otherwise FALSE
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_IsRebindingAllowed
(
    OCI_Statement *stmt
);

/**
* @brief
* Bind a boolean variable (PL/SQL ONLY)
*
* @param stmt - Statement handle
* @param name - Variable name
* @param data - Pointer to boolean variable
*
* @note
* parameter 'data' can NULL if the statement bind allocation mode
* has been set to OCI_BAM_INTERNAL
*
* @warning
* - OCI_BindBoolean() CAN ONLY BE USED for PL/SQL boolean type when calling PL/SQL procedures/function
* - ONLY supported by Oracle 12c and above !
*
* @return
* TRUE on success otherwise FALSE
*/
OCI_SYM_PUBLIC boolean OCI_API OCI_BindBoolean
(
    OCI_Statement *stmt,
    const otext *  name,
    boolean *      data
);

/**
* @brief
* Bind an Number variable
*
* @param stmt - Statement handle
* @param name - Variable name
* @param data - Pointer to short variable
*
* @note
* parameter 'data' can NULL if the statement bind allocation mode
* has been set to OCI_BAM_INTERNAL
*
* @return
* TRUE on success otherwise FALSE
*/

OCI_SYM_PUBLIC boolean OCI_API OCI_BindNumber
(
    OCI_Statement *stmt,
    const otext *  name,
    OCI_Number *   data
);

/**
* @brief
* Bind an array of Number
*
* @param stmt   - Statement handle
* @param name   - Variable name
* @param data   - Array of numbers
* @param nbelem - Number of element in the array (PL/SQL table only)
*
* @warning
* Parameter 'nbelem' SHOULD ONLY be USED for PL/SQL tables.
* For regular DML array operations, pass the value 0.
*
* @note
* parameter 'data' can NULL if the statement bind allocation mode
* has been set to OCI_BAM_INTERNAL
*
* @return
* TRUE on success otherwise FALSE
*/

OCI_SYM_PUBLIC boolean OCI_API OCI_BindArrayOfNumbers
(
    OCI_Statement *stmt,
    const otext *  name,
    OCI_Number **  data,
    unsigned int   nbelem
);

/**
 * @brief
 * Bind an short variable
 *
 * @param stmt - Statement handle
 * @param name - Variable name
 * @param data - Pointer to short variable
 *
 * @note
 * parameter 'data' can NULL if the statement bind allocation mode
 * has been set to OCI_BAM_INTERNAL
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_BindShort
(
    OCI_Statement *stmt,
    const otext *  name,
    short *        data
);

/**
 * @brief
 * Bind an array of shorts
 *
 * @param stmt   - Statement handle
 * @param name   - Variable name
 * @param data   - Array of shorts
 * @param nbelem - Number of element in the array (PL/SQL table only)
 *
 * @warning
 * Parameter 'nbelem' SHOULD ONLY be USED for PL/SQL tables.
 * For regular DML array operations, pass the value 0.
 *
 * @note
 * parameter 'data' can NULL if the statement bind allocation mode
 * has been set to OCI_BAM_INTERNAL
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_BindArrayOfShorts
(
    OCI_Statement *stmt,
    const otext *  name,
    short *        data,
    unsigned int   nbelem
);

/**
 * @brief
 * Bind an unsigned short variable
 *
 * @param stmt - Statement handle
 * @param name - Variable name
 * @param data - Pointer to unsigned short variable
 *
 * @note
 * parameter 'data' can NULL if the statement bind allocation mode
 * has been set to OCI_BAM_INTERNAL
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_BindUnsignedShort
(
    OCI_Statement * stmt,
    const otext *   name,
    unsigned short *data
);

/**
 * @brief
 * Bind an array of unsigned shorts
 *
 * @param stmt   - Statement handle
 * @param name   - Variable name
 * @param data   - Array of unsigned shorts
 * @param nbelem - Number of element in the array (PL/SQL table only)
 *
 * @warning
 * Parameter 'nbelem' SHOULD ONLY be USED for PL/SQL tables.
 * For regular DML array operations, pass the value 0.
 *
 * @note
 * parameter 'data' can NULL if the statement bind allocation mode
 * has been set to OCI_BAM_INTERNAL
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_BindArrayOfUnsignedShorts
(
    OCI_Statement * stmt,
    const otext *   name,
    unsigned short *data,
    unsigned int    nbelem
);

/**
 * @brief
 * Bind an integer variable
 *
 * @param stmt - Statement handle
 * @param name - Variable name
 * @param data - Pointer to int variable
 *
 * @note
 * parameter 'data' can NULL if the statement bind allocation mode
 * has been set to OCI_BAM_INTERNAL
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_BindInt
(
    OCI_Statement *stmt,
    const otext *  name,
    int *          data
);

/**
 * @brief
 * Bind an array of integers
 *
 * @param stmt   - Statement handle
 * @param name   - Variable name
 * @param data   - Array of int
 * @param nbelem - Number of element in the array (PL/SQL table only)
 *
 * @warning
 * Parameter 'nbelem' SHOULD ONLY be USED for PL/SQL tables.
 * For regular DML array operations, pass the value 0.
 *
 * @note
 * parameter 'data' can NULL if the statement bind allocation mode
 * has been set to OCI_BAM_INTERNAL
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_BindArrayOfInts
(
    OCI_Statement *stmt,
    const otext *  name,
    int *          data,
    unsigned int   nbelem
);

/**
 * @brief
 * Bind an unsigned integer variable
 *
 * @param stmt - Statement handle
 * @param name - Variable name
 * @param data - Pointer to unsigned int variable
 *
 * @note
 * parameter 'data' can NULL if the statement bind allocation mode
 * has been set to OCI_BAM_INTERNAL
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_BindUnsignedInt
(
    OCI_Statement *stmt,
    const otext *  name,
    unsigned int * data
);

/**
 * @brief
 * Bind an array of unsigned integers
 *
 * @param stmt   - Statement handle
 * @param name   - Variable name
 * @param data   - Array of unsigned int
 * @param nbelem - Number of element in the array (PL/SQL table only)
 *
 * @warning
 * Parameter 'nbelem' SHOULD ONLY be USED for PL/SQL tables.
 * For regular DML array operations, pass the value 0.
 *
 * @note
 * parameter 'data' can NULL if the statement bind allocation mode
 * has been set to OCI_BAM_INTERNAL
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_BindArrayOfUnsignedInts
(
    OCI_Statement *stmt,
    const otext *  name,
    unsigned int * data,
    unsigned int   nbelem
);

/**
 * @brief
 * Bind a big integer variable
 *
 * @param stmt - Statement handle
 * @param name - Variable name
 * @param data - Pointer to big int variable
 *
 * @note
 * parameter 'data' can NULL if the statement bind allocation mode
 * has been set to OCI_BAM_INTERNAL
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_BindBigInt
(
    OCI_Statement *stmt,
    const otext *  name,
    big_int *      data
);

/**
 * @brief
 * Bind an array of big integers
 *
 * @param stmt   - Statement handle
 * @param name   - Variable name
 * @param data   - Array of big int
 * @param nbelem - Number of element in the array (PL/SQL table only)
 *
 * @warning
 * Parameter 'nbelem' SHOULD ONLY be USED for PL/SQL tables.
 * For regular DML array operations, pass the value 0.
 *
 * @note
 * parameter 'data' can NULL if the statement bind allocation mode
 * has been set to OCI_BAM_INTERNAL
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_BindArrayOfBigInts
(
    OCI_Statement *stmt,
    const otext *  name,
    big_int *      data,
    unsigned int   nbelem
);

/**
 * @brief
 * Bind an unsigned big integer variable
 *
 * @param stmt - Statement handle
 * @param name - Variable name
 * @param data - Pointer to unsigned big int variable
 *
 * @note
 * parameter 'data' can NULL if the statement bind allocation mode
 * has been set to OCI_BAM_INTERNAL
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_BindUnsignedBigInt
(
    OCI_Statement *stmt,
    const otext *  name,
    big_uint *     data
);

/**
 * @brief
 * Bind an array of unsigned big integers
 *
 * @param stmt - Statement handle
 * @param name - Variable name
 * @param data - Array of unsigned big int
 * @param nbelem - Number of element in the array (PL/SQL table only)
 *
 * @warning
 * Parameter 'nbelem' SHOULD ONLY be USED for PL/SQL tables.
 * For regular DML array operations, pass the value 0.
 *
 * @note
 * parameter 'data' can NULL if the statement bind allocation mode
 * has been set to OCI_BAM_INTERNAL
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_BindArrayOfUnsignedBigInts
(
    OCI_Statement *stmt,
    const otext *  name,
    big_uint *     data,
    unsigned int   nbelem
);

/**
 * @brief
 * Bind a string variable
 *
 * @param stmt - Statement handle
 * @param name - Variable name
 * @param data - String to bind
 * @param len  - Max length of the string (in character without
 *               the zero null terminal character)
 *
 * @note
 * if len == 0, len is set to the string size
 *
 * @note
 * parameter 'data' can NULL if the statement bind allocation mode
 * has been set to OCI_BAM_INTERNAL
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_BindString
(
    OCI_Statement *stmt,
    const otext *  name,
    otext *        data,
    unsigned int   len
);

/**
 * @brief
 * Bind an array of strings
 *
 * @param stmt   - Statement handle
 * @param name   - Variable name
 * @param data   - Array of string
 * @param len    - Max length of a single string element (in character without
 *                 the zero null terminal character)
 * @param nbelem - Number of element in the array (PL/SQL table only)
 *
 * @warning
 * Parameter 'nbelem' SHOULD ONLY be USED for PL/SQL tables.
 * For regular DML array operations, pass the value 0.
 *
 * @note
 * parameter 'data' can NULL if the statement bind allocation mode
 * has been set to OCI_BAM_INTERNAL
 *
 * @warning
 * if len <= 0, it returns FALSE
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_BindArrayOfStrings
(
    OCI_Statement *stmt,
    const otext *  name,
    otext *        data,
    unsigned int   len,
    unsigned int   nbelem
);

/**
 * @brief
 * Bind a raw buffer
 *
 * @param stmt - Statement handle
 * @param name - Variable name
 * @param data - buffer to bind
 * @param len  - Max length of the buffer
 *
 * @note
 * if len <= 0, it returns false
 *
 * @note
 * parameter 'data' can NULL if the statement bind allocation mode
 * has been set to OCI_BAM_INTERNAL
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_BindRaw
(
    OCI_Statement *stmt,
    const otext *  name,
    void *         data,
    unsigned int   len
);

/**
 * @brief
 * Bind an array of raw buffers
 *
 * @param stmt   - Statement handle
 * @param name   - Variable name
 * @param data   - Array of buffers
 * @param len    - Size in bytes on a single RAW array element
 * @param nbelem - Number of element in the array (PL/SQL table only)
 *
 * @warning
 * Parameter 'nbelem' SHOULD ONLY be USED for PL/SQL tables.
 * For regular DML array operations, pass the value 0.
 *
 * @note
 * The buffer must be a contiguous block of data elements
 *
 * @note
 * If len <= 0, it returns FALSE
 *
 * @note
 * parameter 'data' can NULL if the statement bind allocation mode
 * has been set to OCI_BAM_INTERNAL
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_BindArrayOfRaws
(
    OCI_Statement *stmt,
    const otext *  name,
    void *         data,
    unsigned int   len,
    unsigned int   nbelem
);

/**
 * @brief
 * Bind a double variable
 *
 * @param stmt - Statement handle
 * @param name - Variable name
 * @param data - Pointer to double variable
 *
 * @note
 * parameter 'data' can NULL if the statement bind allocation mode
 * has been set to OCI_BAM_INTERNAL
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_BindDouble
(
    OCI_Statement *stmt,
    const otext *  name,
    double *       data
);

/**
 * @brief
 * Bind an array of doubles
 *
 * @param stmt   - Statement handle
 * @param name   - Variable name
 * @param data   - Array of double
 * @param nbelem - Number of element in the array (PL/SQL table only)
 *
 * @warning
 * Parameter 'nbelem' SHOULD ONLY be USED for PL/SQL tables.
 * For regular DML array operations, pass the value 0.
 *
 * @note
 * parameter 'data' can NULL if the statement bind allocation mode
 * has been set to OCI_BAM_INTERNAL
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_BindArrayOfDoubles
(
    OCI_Statement *stmt,
    const otext *  name,
    double *       data,
    unsigned int   nbelem
);

/**
 * @brief
 * Bind a float variable
 *
 * @param stmt - Statement handle
 * @param name - Variable name
 * @param data - Pointer to float variable
 *
 * @note
 * parameter 'data' can NULL if the statement bind allocation mode
 * has been set to OCI_BAM_INTERNAL
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_BindFloat
(
    OCI_Statement *stmt,
    const otext *  name,
    float *        data
);

/**
 * @brief
 * Bind an array of floats
 *
 * @param stmt   - Statement handle
 * @param name   - Variable name
 * @param data   - Array of float
 * @param nbelem - Number of element in the array (PL/SQL table only)
 *
 * @warning
 * Parameter 'nbelem' SHOULD ONLY be USED for PL/SQL tables.
 * For regular DML array operations, pass the value 0.
 *
 * @note
 * parameter 'data' can NULL if the statement bind allocation mode
 * has been set to OCI_BAM_INTERNAL
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_BindArrayOfFloats
(
    OCI_Statement *stmt,
    const otext *  name,
    float *        data,
    unsigned int   nbelem
);

/**
 * @brief
 * Bind a date variable
 *
 * @param stmt - Statement handle
 * @param name - Variable name
 * @param data - Date handle
 *
 * @note
 * parameter 'data' can NULL if the statement bind allocation mode
 * has been set to OCI_BAM_INTERNAL
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_BindDate
(
    OCI_Statement *stmt,
    const otext *  name,
    OCI_Date *     data
);

/**
 * @brief
 * Bind an array of dates
 *
 * @param stmt   - Statement handle
 * @param name   - Variable name
 * @param data   - Array of date handle
 * @param nbelem - Number of element in the array (PL/SQL table only)
 *
 * @warning
 * Parameter 'nbelem' SHOULD ONLY be USED for PL/SQL tables.
 * For regular DML array operations, pass the value 0.
 *
 * @note
 * parameter 'data' can NULL if the statement bind allocation mode
 * has been set to OCI_BAM_INTERNAL
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_BindArrayOfDates
(
    OCI_Statement *stmt,
    const otext *  name,
    OCI_Date **    data,
    unsigned int   nbelem
);

/**
 * @brief
 * Bind a timestamp variable
 *
 * @param stmt - Statement handle
 * @param name - Variable name
 * @param data - Timestamp handle
 *
 * @note
 * parameter 'data' CANNOT be NULL resulting OCI_BAM_INTERNAL bind allocation mode being NOT supported
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_BindTimestamp
(
    OCI_Statement *stmt,
    const otext *  name,
    OCI_Timestamp *data
);

/**
 * @brief
 * Bind an array of timestamp handles
 *
 * @param stmt   - Statement handle
 * @param name   - Variable name
 * @param data   - Array of Timestamp handle
 * @param type   - Timestamp type
 * @param nbelem - Number of element in the array (PL/SQL table only)
 *
 * @warning
 * Parameter 'nbelem' SHOULD ONLY be USED for PL/SQL tables.
 * For regular DML array operations, pass the value 0.
 *
 * @note
 * See OCI_TimestampCreate() for possible values of parameter 'type'
 *
 * @note
 * parameter 'data' can NULL if the statement bind allocation mode
 * has been set to OCI_BAM_INTERNAL
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_BindArrayOfTimestamps
(
    OCI_Statement * stmt,
    const otext *   name,
    OCI_Timestamp **data,
    unsigned int    type,
    unsigned int    nbelem
);

/**
 * @brief
 * Bind an interval variable
 *
 * @param stmt - Statement handle
 * @param name - Variable name
 * @param data - Interval handle
 *
 * @note
 * parameter 'data' CANNOT be NULL resulting OCI_BAM_INTERNAL bind allocation mode being NOT supported
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_BindInterval
(
    OCI_Statement *stmt,
    const otext *  name,
    OCI_Interval * data
);

/**
 * @brief
 * Bind an array of interval handles
 *
 * @param stmt   - Statement handle
 * @param name   - Variable name
 * @param data   - Array of Interval handle
 * @param type   - Interval type
 * @param nbelem - Number of element in the array (PL/SQL table only)
 *
 * @warning
 * Parameter 'nbelem' SHOULD ONLY be USED for PL/SQL tables.
 * For regular DML array operations, pass the value 0.
 *
 * @note
 * See OCI_IntervalCreate() for possible values of parameter 'type'
 *
 * @note
 * parameter 'data' can NULL if the statement bind allocation mode
 * has been set to OCI_BAM_INTERNAL
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_BindArrayOfIntervals
(
    OCI_Statement *stmt,
    const otext *  name,
    OCI_Interval **data,
    unsigned int   type,
    unsigned int   nbelem
);

/**
 * @brief
 * Bind a Lob variable
 *
 * @param stmt - Statement handle
 * @param name - Variable name
 * @param data - Lob handle
 *
 * @note
 * parameter 'data' CANNOT be NULL resulting OCI_BAM_INTERNAL bind allocation mode being NOT supported
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_BindLob
(
    OCI_Statement *stmt,
    const otext *  name,
    OCI_Lob *      data
);

/**
 * @brief
 * Bind an array of Lob handles
 *
 * @param stmt   - Statement handle
 * @param name   - Variable name
 * @param data   - Array of Lob handle
 * @param type   - Lob type
 * @param nbelem - Number of element in the array (PL/SQL table only)
 *
 * @warning
 * Parameter 'nbelem' SHOULD ONLY be USED for PL/SQL tables.
 * For regular DML array operations, pass the value 0.
 *
 * @note
 * See OCI_LobCreate() for possible values of parameter 'type'
 *
 * @note
 * parameter 'data' can NULL if the statement bind allocation mode
 * has been set to OCI_BAM_INTERNAL
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_BindArrayOfLobs
(
    OCI_Statement *stmt,
    const otext *  name,
    OCI_Lob **     data,
    unsigned int   type,
    unsigned int   nbelem
);

/**
 * @brief
 * Bind a File variable
 *
 * @param stmt - Statement handle
 * @param name - Variable name
 * @param data - File handle
 *
 * @note
 * parameter 'data' CANNOT be NULL resulting OCI_BAM_INTERNAL bind allocation mode being NOT supported
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_BindFile
(
    OCI_Statement *stmt,
    const otext *  name,
    OCI_File *     data
);

/**
 * @brief
 * Bind an array of File handles
 *
 * @param stmt   - Statement handle
 * @param name   - Variable name
 * @param data   - Array of File handle
 * @param type   - File type
 * @param nbelem - Number of element in the array (PL/SQL table only)
 *
 * @warning
 * Parameter 'nbelem' SHOULD ONLY be USED for PL/SQL tables.
 * For regular DML array operations, pass the value 0.
 *
 * @note
 * See OCI_FileCreate() for possible values of parameter 'type'
 *
 * @note
 * parameter 'data' can NULL if the statement bind allocation mode
 * has been set to OCI_BAM_INTERNAL
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_BindArrayOfFiles
(
    OCI_Statement *stmt,
    const otext *  name,
    OCI_File **    data,
    unsigned int   type,
    unsigned int   nbelem
);

/**
 * @brief
 * Bind an object (named type) variable
 *
 * @param stmt - Statement handle
 * @param name - Variable name
 * @param data - Object handle
 *
 * @note
 * parameter 'data' CANNOT be NULL resulting OCI_BAM_INTERNAL bind allocation mode being NOT supported
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_BindObject
(
    OCI_Statement *stmt,
    const otext *  name,
    OCI_Object *   data
);

/**
 * @brief
 * Bind an array of object handles
 *
 * @param stmt   - Statement handle
 * @param name   - Variable name
 * @param data   - Array of object handle
 * @param typinf - type info handle
 * @param nbelem - Number of element in the array (PL/SQL table only)
 *
 * @warning
 * Parameter 'nbelem' SHOULD ONLY be USED for PL/SQL tables.
 * For regular DML array operations, pass the value 0.
 *
 * @note
 * parameter 'data' can NULL if the statement bind allocation mode
 * has been set to OCI_BAM_INTERNAL
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_BindArrayOfObjects
(
    OCI_Statement *stmt,
    const otext *  name,
    OCI_Object **  data,
    OCI_TypeInfo * typinf,
    unsigned int   nbelem
);

/**
 * @brief
 * Bind a Collection variable
 *
 * @param stmt - Statement handle
 * @param name - Variable name
 * @param data - Collection handle to bind
 *
 * @note
 * parameter 'data' CANNOT be NULL resulting OCI_BAM_INTERNAL bind allocation mode being NOT supported
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_BindColl
(
    OCI_Statement *stmt,
    const otext *  name,
    OCI_Coll *     data
);

/**
 * @brief
 * Bind an array of Collection handles
 *
 * @param stmt   - Statement handle
 * @param name   - Variable name
 * @param data   - Array of Collection handle
 * @param typinf - Type info handle
 * @param nbelem - Number of element in the array (PL/SQL table only)
 *
 * @warning
 * Parameter 'nbelem' SHOULD ONLY be USED for PL/SQL tables.
 * For regular DML array operations, pass the value 0.
 *
 * @note
 * See OCI_CollCreate() for possible values of parameter 'type'
 *
 * @note
 * parameter 'data' can NULL if the statement bind allocation mode
 * has been set to OCI_BAM_INTERNAL
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_BindArrayOfColls
(
    OCI_Statement *stmt,
    const otext *  name,
    OCI_Coll **    data,
    OCI_TypeInfo * typinf,
    unsigned int   nbelem
);

/**
 * @brief
 * Bind a Ref variable
 *
 * @param stmt - Statement handle
 * @param name - Variable name
 * @param data - Ref handle to bind
 *
 * @note
 * parameter 'data' CANNOT be NULL resulting OCI_BAM_INTERNAL bind allocation mode being NOT supported
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_BindRef
(
    OCI_Statement *stmt,
    const otext *  name,
    OCI_Ref *      data
);

/**
 * @brief
 * Bind an array of Ref handles
 *
 * @param stmt   - Statement handle
 * @param name   - Variable name
 * @param data   - Array of Ref handle
 * @param typinf - type info handle
 * @param nbelem - Number of element in the array (PL/SQL table only)
 *
 * @warning
 * Parameter 'nbelem' SHOULD ONLY be USED for PL/SQL tables.
 * For regular DML array operations, pass the value 0.
 *
 * @note
 * parameter 'data' can NULL if the statement bind allocation mode
 * has been set to OCI_BAM_INTERNAL
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_BindArrayOfRefs
(
    OCI_Statement *stmt,
    const otext *  name,
    OCI_Ref **     data,
    OCI_TypeInfo * typinf,
    unsigned int   nbelem
);

/**
 * @brief
 * Bind a Statement variable (PL/SQL Ref Cursor)
 *
 * @param stmt - Statement handle
 * @param name - Variable name
 * @param data - Statement handle to bind
 *
 * @note
 * parameter 'data' CANNOT be NULL resulting OCI_BAM_INTERNAL bind allocation mode being NOT supported
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_BindStatement
(
    OCI_Statement *stmt,
    const otext *  name,
    OCI_Statement *data
);

/**
 * @brief
 * Bind a Long variable
 *
 * @param stmt - Statement handle
 * @param name - Variable name
 * @param data - Long handle
 * @param size - Size of the long buffer in bytes or characters
 *
 * @note
 * Size is expressed in:
 * - Bytes for BLONGs
 * - Characters for CLONGs
 *
 * @note
 * parameter 'data' CANNOT be NULL whatever the statement bind allocation mode
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_BindLong
(
    OCI_Statement *stmt,
    const otext *  name,
    OCI_Long *     data,
    unsigned int   size
);

/**
 * @brief
 * Returns the first or next error that occurred within a DML array statement execution
 *
 * @param stmt - Statement handle
 *
 * @return
 * The first or next error handle otherwise NULL
 */

OCI_SYM_PUBLIC OCI_Error * OCI_API OCI_GetBatchError
(
    OCI_Statement *stmt
);

/**
 * @brief
 * Returns the number of errors that occurred within the last DML array statement
 *
 * @param stmt - Statement handle
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_GetBatchErrorCount
(
    OCI_Statement *stmt
);

/**
 * @brief
 * Return the number of binds currently associated to a statement
 *
 * @param stmt - Statement handle
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_GetBindCount
(
    OCI_Statement *stmt
);

/**
 * @brief
 * Return the bind handle at the given index in the internal array of bind
 * handle
 *
 * @param stmt  - Statement handle
 * @param index - Bind position
 *
 * @note
 * Index starts at 1.
 *
 * @note
 * Bind handle are created sequentially. For example, the third call to a
 * OCI_BindXXX() generates a bind handle of index 3.
 *
 * @return
 * The bind handle or NULL if index is out of bounds
 *
 */

OCI_SYM_PUBLIC OCI_Bind * OCI_API OCI_GetBind
(
    OCI_Statement *stmt,
    unsigned int   index
);

/**
 * @brief
 * Return a bind handle from its name
 *
 * @param stmt - Statement handle
 * @param name - Bind variable name
 *
 * @note
 * Bind names must include a semicolon at the beginning.
 *
 * @return
 * The bind handle or NULL if not found
 *
 */

OCI_SYM_PUBLIC OCI_Bind * OCI_API OCI_GetBind2
(
    OCI_Statement *stmt,
    const otext *  name
);

/**
* @brief
* Return the index of the bind  from its name belonging to the given statement
*
* @param stmt - Statement handle
* @param name - Bind variable name
*
* @warning
* The bind name is case insensitive
*
* @note
* Bind indexes start with 1 in OCILIB
*
* @return
* Bind index on success or zero if the bind does not exists or if statement is NULL
*
*/

OCI_SYM_PUBLIC unsigned int OCI_API OCI_GetBindIndex
(
    OCI_Statement *stmt,
    const otext *  name
);

/**
 * @brief
 * Return the name of the given bind
 *
 * @param bnd - Bind handle
 *
 */

OCI_SYM_PUBLIC const otext * OCI_API OCI_BindGetName
(
    OCI_Bind *bnd
);

/**
 * @brief
 * Set the direction mode of a bind handle
 *
 * @param bnd       - Bind handle
 * @param direction - direction mode
 *
 * @note
 * Possible values for parameter 'direction' :
 *   - OCI_BDM_IN      : input values  (not modified by the server)
 *   - OCI_BDM_OUT     : output values (modified by the server)
 *   - OCI_BDM_IN_OUT  : input and output values
 *
 * @note
 * Default value is OCI_BDM_IN_OUT
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_BindSetDirection
(
    OCI_Bind *   bnd,
    unsigned int direction
);

/**
 * @brief
 * Get the direction mode of a bind handle
 *
 * @param bnd - Bind handle
 *
 * @note
 * see OCI_BindSetDirection() for more details
 *
 * return the bind direction mode on success otherwise OCI_UNKNWON
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_BindGetDirection
(
    OCI_Bind *bnd
);

/**
 * @brief
 * Return the OCILIB type of the given bind
 *
 * @param bnd - Bind handle
 *
 * @note
 * Possible values are :
 *
 * - OCI_CDT_NUMERIC     : short, int, long long, float, double
 * - OCI_CDT_DATETIME    : OCI_Date *
 * - OCI_CDT_TEXT        : otext *
 * - OCI_CDT_LONG        : OCI_Long *
 * - OCI_CDT_CURSOR      : OCI_Statement *
 * - OCI_CDT_LOB         : OCI_Lob  *
 * - OCI_CDT_FILE        : OCI_File *
 * - OCI_CDT_TIMESTAMP   : OCI_Timestamp *
 * - OCI_CDT_INTERVAL    : OCI_Interval *
 * - OCI_CDT_RAW         : void *
 * - OCI_CDT_OBJECT      : OCI_Object *
 * - OCI_CDT_COLLECTION  : OCI_Coll *
 * - OCI_CDT_REF         : OCI_Ref *
 * - OCI_CDT_BOOLEAN     : boolean
 *
 * @return
 * The column type or OCI_CDT_UNKNOWN on error
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_BindGetType
(
    OCI_Bind *bnd
);

/**
 * @brief
 * Return the OCILIB object subtype of the given bind
 *
 * @param bnd - Bind handle
 *
 * @note
 * * This call is valid for the following OCILIB types:
 * - OCI_CDT_NUMERIC
 * - OCI_CDT_LONG
 * - OCI_CDT_LOB
 * - OCI_CDT_FILE
 * - OCI_CDT_TIMESTAMP
 * - OCI_CDT_INTERVAL
 *
 * For numeric binds the possible values are:
 * - OCI_NUM_SHORT
 * - OCI_NUM_INT
 * - OCI_NUM_BIGINT
 * - OCI_NUM_USHORT
 * - OCI_NUM_UINT
 * - OCI_NUM_BIGUINT
 * - OCI_NUM_DOUBLE
 * - OCI_NUM_FLOAT
 * - OCI_NUM_NUMBER
 *
 * For OCI_Long type the possible values are:
 * - OCI_BLONG
 * - OCI_CLONG
 *
 * For OCI_Lob type the possible values are:
 * - OCI_BLOB
 * - OCI_CLOB
 * - OCI_NCLOB
 *
 * For OCI_File type the possible values are:
 * - OCI_BFILE
 * - OCI_CFILE
 *
 * For OCI_Timestamp type the possible values are:
 * - OCI_TIMESTAMP
 * - OCI_TIMESTAMP_TZ
 * - OCI_TIMESTAMP_LTZ
 *
 * For OCI_Interval type the possible values are:
 * - OCI_INTERVAL_YM
 * - OCI_INTERVAL_DS
 *
 * @note
 * For all other OCILIB types, it returns OCI_UNKNOWN
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_BindGetSubtype
(
    OCI_Bind *bnd
);

/**
 * @brief
 * Return the number of elements of the bind handle
 *
 * @param bnd - Bind handle
 *
 * @return
 * - For single binds, it returns 1
 * - For array binds, it returns the number of element in the array
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_BindGetDataCount
(
    OCI_Bind *bnd
);

/**
 * @brief
 * Return the user defined data associated with a bind handle
 *
 * @param bnd - Bind handle
 *
 * @return
 * - The pointer to variable/array passed to an OCI_BindXXX() or
 *   OCI_BindArrayOfXXX() call
 *
 */

OCI_SYM_PUBLIC void * OCI_API OCI_BindGetData
(
    OCI_Bind *bnd
);

/**
 * @brief
 * Return the statement handle associated with a bind handle
 *
 * @param bnd - bind handle
 *
 */

OCI_SYM_PUBLIC OCI_Statement * OCI_API OCI_BindGetStatement
(
    OCI_Bind *bnd
);

/**
 * @brief
 * Set the actual size of the element held by the given bind handle
 *
 * @param bnd  - bind handle
 * @param size - data size
 *
 * @note
 * This call is not mandatory and should ONLY be called for RAWs binds to set
 * the real size of the given data if different from the expected column or
 * parameter size
 *
 * @note
 * It works as well with string based PL/SQL tables (in or in/out but NOT out)
 * even if it's not necessary.
 *
 * @warning
 * For binds of type OCI_CDT_TEXT (strings), the parameter 'size' is expressed in
 * number of characters.
 *
 * @return
 * Data size if the bind type is listed above otherwise 0.
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_BindSetDataSize
(
    OCI_Bind *   bnd,
    unsigned int size
);

/**
 * @brief
 * Set the size of the element at the given position in
 * the bind input array
 *
 * @param bnd      - bind handle
 * @param position - Position in the array
 * @param size     - data size
 *
 * @note
 * See OCI_BindSetDataSize() for supported data types
 *
 * @warning
 * Before execution, it returns the max default size for the bind and not the real
 * data size, unless a custom size has been set with OCI_BindSetDataSizeXXX()
 * After execution, it returns the real data size.
 *
 * @warning
 * For binds of type OCI_CDT_TEXT (strings), the parameter 'size' is expressed in
 * number of characters.
 *
 * @return
 * Data size if the bind type is listed above otherwise 0.
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_BindSetDataSizeAtPos
(
    OCI_Bind *   bnd,
    unsigned int position,
    unsigned int size
);

/**
 * @brief
 * Return the actual size of the element held by the given bind handle
 *
 * @param bnd - bind handle
 *
 * @note
 * See OCI_BindSetDataSize() for supported data types
 *
 * @warning
 * For binds of type OCI_CDT_TEXT (strings), the returned value is expressed in
 * number of characters.
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_BindGetDataSize
(
    OCI_Bind *bnd
);

/**
 * @brief
 * Return the actual size of the element at the given position in
 * the bind input array
 *
 * @param bnd      - bind handle
 * @param position - Position in the array
 *
 * @note
 * See OCI_BindSetDataSize() for supported data types
 *
 * @warning
 * For binds of type OCI_CDT_TEXT (strings), the returned value is expressed in
 * number of characters.
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_BindGetDataSizeAtPos
(
    OCI_Bind *   bnd,
    unsigned int position
);

/**
 * @brief
 * Set the bind variable to null
 *
 * @param bnd   - Bind handle
 *
 * @note
 * There is no notion of null value in C.
 * It's necessary to explicitly tell Oracle that the bind has a null value.
 * It must be done before an OCI_Execute() call
 *
 * @note
 * For handled based data types (non scalar types), OCILIB performs an extra
 * check on handles and set the bind status to null is the handle is null
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_BindSetNull
(
    OCI_Bind *bnd
);

/**
 * @brief
 * Set to null the entry in the bind variable input array
 *
 * @param bnd      - Bind handle
 * @param position - Position in the array
 *
 * @note
 * There is no notion of null value in C.
 * It's necessary to explicitly tell Oracle that the bind has a null value.
 * It must be done before an OCI_Execute() call
 *
 * @warning
 * Position starts with 1
 *
 * @note
 * For handled based data types (non scalar types), OCILIB performs an extra
 * check on handles and set the bind status to null is the handle is null
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_BindSetNullAtPos
(
    OCI_Bind *   bnd,
    unsigned int position
);

/**
 * @brief
 * Set the bind variable to NOT null
 *
 * @param bnd   - Bind handle
 *
 * @note
 * There is no notion of null value in C.
 * It's necessary to explicitly tell Oracle that the bind has a null value.
 * It must be done before an OCI_Execute() call
 *
 * @note
 * For handled based data types (non scalar types), OCILIB performs an extra
 * check on handles and set the bind status to null is the handle is null
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_BindSetNotNull
(
    OCI_Bind *bnd
);

/**
 * @brief
 * Set to NOT null the entry in the bind variable input array
 *
 * @param bnd      - Bind handle
 * @param position - Position in the array
 *
 * @note
 * There is no notion of null value in C.
 * It's necessary to explicitly tell Oracle that the bind has a null value.
 * It must be done before an OCI_Execute() call
 *
 * @warning
 * Position starts with 1
 *
 * @note
 * For handled based data  types (non scalar types), OCILIB performs an extra
 * check on handles and set the bind status to null is the handle is null
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_BindSetNotNullAtPos
(
    OCI_Bind *   bnd,
    unsigned int position
);

/**
 * @brief
 * Check if the current value of the binded variable is marked as NULL
 *
 * @param bnd - Bind handle
 *
 * @return
 * TRUE if it's null otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_BindIsNull
(
    OCI_Bind *bnd
);

/**
 * @brief
 * Check if the current entry value at the given index of the binded array
 * is marked as NULL
 *
 * @param bnd      - Bind handle
 * @param position - Position in the array
 *
 * @warning
 * Position starts with 1
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_BindIsNullAtPos
(
    OCI_Bind *   bnd,
    unsigned int position
);

/**
 * @brief
 * Set the charset form of the given character based bind variable
 *
 * @param bnd   - Bind handle
 * @param csfrm - charset form
 *
 * @note
 * Possible values are :
 *
 * - OCI_CSF_DEFAULT : the column has default charset
 * - OCI_CSF_NATIONAL: the column has national charset
 *
 * @note
 * This call has to be made after OCI_Prepare() but before OCI_Execute()
 *
 * @warning
 * This call does nothing :
 *  - if the csform is out of range
 *  - if the bind type is not OCI_CFT_TEXT or OCI_CDT_LONG
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_BindSetCharsetForm
(
    OCI_Bind *   bnd,
    unsigned int csfrm
);

/**
 * @brief
 * Get the allocation mode of a bind handle
 *
 * @param bnd - Bind handle
 *
 * @note
 * Possible values are :
 *  - OCI_BAM_EXTERNAL : bind variable is allocated by user code
 *  - OCI_BAM_INTERNAL : bind variable is allocated internally
 *
 * return the allocation mode on success otherwise OCI_UNKNOWN
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_BindGetAllocationMode
(
    OCI_Bind *bnd
);

/**
 * @} OcilibCApiBinding
 */

/**
 * @defgroup OcilibCApiFetching Fetching data
 * @{
 *
 * OCILIB offers a really easy and smart mechanism to fetch data from a SQL Statement.
 * It looks like what's found in JDBC and other object oriented databases frameworks.
 *
 * ONLY the following statements can return resultsets that can be fetched by host programs:
 * - Statements executing SQL SELECT
 * - Statements executing SQL INSERT/UPDATE/DELETE using a RETURNING INTO clause
 * - Statements binded to PL/SQL OPEN FOR argument
 * - Statements binded to PL/SQL procedure OUT variables
 * - Statements implicitly returned from PL/SQL procedure or blocks (new feature in Oracle 12cR1) using
 *   DBMS_SQL.RETURN_RESULT()
 *
 * These resultsets are encapsulated in OCILIB by OCI_Resultset objects.
 *
 * Thus, after any successful call to an OCI_Executexxx() function that executed
 * a fetchable statement or filled output bind variables, the resultset can be
 * retrieved by calling OCI_GetResultset()
 *
 * The creation of a OCI_Resultset object consists in :
 *
 * - Describing the output columns of the resultset
 * - Allocating memory to hold the content data
 *
 * OCILIB supports multi-row fetching for increasing performances. Instead of
 * fetching data row by row from the server (that induces lots of round-trips
 * between the client and the server), the library prefetches data chunk by
 * chunks (default is 20 rows).
 * So, less network traffic and better performances.
 * These mechanisms are completely hidden from the application which fetches the
 * resultset row by row.
 *
 * Once the Resultset handle is retrieved :
 *
 * - It can be fetched by calling OCI_FetchNext() as long as it returns TRUE.
 * - To retrieve the value of a column, call OCI_GetXXXX() where XXXX is the
 *   type of data you want to fetch.
 *
 * @note
 * In case of a statement that has executed PL/SQL calls or blocks returning implicit resultsets:
 * - OCI_GetResultset() return the first available resultset
 * - OCI_GetNextResultset() return the next available resultset until no more resultset available
 *
 * @par Scrollable Resultsets
 *
 * Oracle 9i introduced scrollable cursors (resultsets in OCILIB) that can be
 * fetched:
 *
 * - Sequentially in both directions: OCI_FetchPrev() and OCI_FetchNext()
 * - To a relative position in the resultset: OCI_FetchSeek() with OCI_SFD_RELATIVE
 * - To an absolute position in the resultset: OCI_FetchSeek() with OCI_SFD_ABOSLUTE
 * - To the first or last row in the resultset: OCI_FetchFirst() and OCI_FetchLast()
 *
 * Scrollable statements uses more server and client resources and should only
 * be used when necessary.
 *
 * Resultsets are 'forward only' by default. Call OCI_SetFetchMode() with
 * OCI_SFM_SCROLLABLE to enable scrollable resultsets for a given statement.
 *
 * @warning
 * Any use of scrollable fetching functions with a resultset that depends on a
 * statement with fetch mode set to OCI_SFM_DEFAULT will fail !
 *
 * @warning
 * If you intend to use OCI_FetchSeek() on a scrollable statement and if any of the
 * selected columns is a ref cursor or a nested table, OCILIB will internally set the
 * resultset internal array size to 1 and thus ignore any values set using OCI_SetFetchSize()
 * This is performed due to an Oracle bug.
 *
 * @note
 * If the column internal data does not match the requested type, OCILIB tries
 * to convert the data when it's possible and throws an error if not.
 *
 * The properties (columns names, types ...) of the resultset are accessible
 * through a set of APIs.
 *
 * @par Implicit conversion to string types
 *
 * OCI_GetString() performs an implicit conversion from ANY Oracle types:
 *
 * - Numerics (based on the current connection handle numeric format)
 * - Binary doubles and floats (using the standard C Library functions)
 * - OCI_Date      : uses OCI_DateToText() with current connection date format
 * - OCI_Timestamp : uses OCI_TimestampToText() with current connection date format
 * - OCI_Interval  : uses OCI_IntervalToText() with Oracle default format
 * - OCI_Coll      : uses OCI_CollToText()
 * - OCI_Object    : uses OCI_ObjectToText()
 * - OCI_Ref       : uses OCI_RefToText()
 * - OCI_File      : returns "$(folder)/$(filename)" - no content returned
 * - OCI_Lob       : see note above for binary types
 * - OCI_Long      : see note above for binary types
 * - RAWs          : see note above for binary types
 *
 * @note
 * For RAWs and BLOBs attributes, their binary values are converted to hexadecimal strings
 * For LONG and CLOBs/NCLOBSs attributes, the whole string content is returned
 *
 * @note
 * The following OCILIB types are not supported for implicit conversion:
 * - OCI_Statement
 *
 * @warning
 * For Dates and numerics types, OCILIB uses OCI client calls to perform
 * the conversion.
 * For binary double and binary floats data types, OCI client functions cannot
 * handle the full double range of values. Thus, OCILIB is using the
 * standard C library to convert theses data types to string
 *
 * @par Fetching rows into user structures
 *
 * It is possible to fetch a complete row into a user defined structure.
 * Each column of the resultset is mapped to a structure member.
 * The mapping rules are :
 *   - LOBs (CLOB, NCLOB, BLOB) : OCI_Lob *
 *   - DATE : OCI_Date *
 *   - TIMESTAMPS : OCI_Timestamp *
 *   - INTERVALS : OCI_Interval *
 *   - LONG, LONG RAW : OCI_Long *
 *   - REFs : OCI_Ref *
 *   - CURSOR, RESULSET : OCI_Statement *
 *   - OBJECTS, UDT : OCI_Object *
 *   - Character columns (CHAR,VARCHAR, etc..) : otext *
 *   - All NUMERIC types :
 *        - default : big_int
 *        - user defined (see OCI_SetStructNumericType())
 *
 * See OCI_GetStruct() and OCI_SetStructNumericType() for more details
 *
 * @par Fetch Example
 * @include fetch.c
 *
 * @par Fetch Rows into user structures Example
 * @include fetch_struct.c
 *
 * @par Meta data Example
 * @include meta.c
 *
 * @par Ref cursor Example
 * @include cursor.c
 *
 * @par Implicit resultset Example
 * @include implicit_resultset.c
 *
 * @par Scrollable resultset Example
 * @include scroll.c
 *
 */

/**
 * @brief
 * Retrieve the resultset handle from an executed statement
 *
 * @param stmt - Statement handle
 *
 * @note
 * See @ref OcilibCApiFetching for more details about what statements can return resultsets
 *
 * @warning
 * If the statement has not been prepared and executed, no resultset will be  returned
 *
 * @return
 * A resultset handle on success otherwise NULL
 *
 */

OCI_SYM_PUBLIC OCI_Resultset * OCI_API OCI_GetResultset
(
    OCI_Statement *stmt
);

/**
 * @brief
 * Free the statement resultsets
 *
 * @param stmt - Statement handle
 *
 * @note
 * This call is optional. Resultsets are automatically freed when the
 * statement is destroyed or when it's reused.
 *
 * @note
 * This function has been introduced for releasing big resultsets when the
 * application wants to keep the statement alive and does not know when it
 * will be destroyed.
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_ReleaseResultsets
(
    OCI_Statement *stmt
);

/**
 * @brief
 * Fetch the next row of the resultset
 *
 * @param rs - Resultset handle
 *
 * @note
 * OCI_FetchNext() works for normal and scrollable resultsets
 *
 * @return
 * TRUE on success otherwise FALSE if :
 * - Empty resultset
 * - Last row already fetched
 * - An error occurred
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_FetchNext
(
    OCI_Resultset *rs
);

/**
 * @brief
 * Fetch the previous row of the resultset
 *
 * @param rs - Resultset handle
 *
 * @note
 * OCI_FetchPrev() works ONLY for scrollable resultsets
 *
 * @return
 * TRUE on success otherwise FALSE if :
 * - Empty resultset
 * - First row already fetched
 * - An error occurred
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_FetchPrev
(
    OCI_Resultset *rs
);

/**
 * @brief
 * Fetch the first row of the resultset
 *
 * @param rs - Resultset handle
 *
 * @note
 * OCI_FetchFirst() works ONLY for scrollable resultsets
 *
 * @return
 * TRUE on success otherwise FALSE if :
 * - Empty resultset
 * - An error occurred
 *f
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_FetchFirst
(
    OCI_Resultset *rs
);

/**
 * @brief
 * Fetch the last row of the resultset
 *
 * @param rs - Resultset handle
 *
 * @note
 * OCI_FetchLast() works ONLY for scrollable resultsets
 *
 * @return
 * TRUE on success otherwise FALSE if:
 * - Empty resultset
 * - An error occurred
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_FetchLast
(
    OCI_Resultset *rs
);

/**
 * @brief
 * Custom Fetch of the resultset
 *
 * @param rs      - Resultset handle
 * @param mode    - Fetch direction
 * @param offset  - Fetch offset
 *
 * @note
 * Possible values for 'direction' parameter are:
 *  - OCI_SFD_ABSOLUTE
 *  - OCI_SFD_RELATIVE
 *
 * @note
 * OCI_FetchSeek() works ONLY for scrollable resultsets
 *
 * @warning
 * If you intend to use OCI_FetchSeek() on a scrollable statement and if any of the
 * selected columns is a ref cursor or a nested table, you must set the fetching size
 * to 1 using OCI_SetFetchSize() before calling OCI_GetResultset()
 * Otherwise OCI_FetchSeek() will fails with a OCI-10002 error
 *
 * @return
 * TRUE on success otherwise FALSE if:
 * - Empty resultset
 * - An error occurred
 * - OCI_SetFetchMode() has not been called with OCI_SFM_SCROLLABLE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_FetchSeek
(
    OCI_Resultset *rs,
    unsigned int   mode,
    int            offset
);

/**
 * @brief
 * Retrieve the number of rows fetched so far
 *
 * @param rs - Resultset handle
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_GetRowCount
(
    OCI_Resultset *rs
);

/**
 * @brief
 * Retrieve the current row number
 *
 * @param rs - Resultset handle
 *
 * @note
 * - OCI_GetCurrentRow() returns the current row number starting from 1
 * - If the resultset has not been fetched or if the resultset is empty, it returns 0
 * - If the resultset has been fully fetched, it returns the last fetched row number
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_GetCurrentRow
(
    OCI_Resultset *rs
);

/**
 * @brief
 * Return the number of columns in the resultset
 *
 * @param rs - Resultset handle
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_GetColumnCount
(
    OCI_Resultset *rs
);

/**
 * @brief
 * Return the column object handle at the given index in the resultset
 *
 * @param rs    - Resultset handle
 * @param index - Column position
 *
 * @return
 * - Column handle on success
 * - NULL if index is out of bounds or on error
 *
 */

OCI_SYM_PUBLIC OCI_Column * OCI_API OCI_GetColumn
(
    OCI_Resultset *rs,
    unsigned int   index
);

/**
 * @brief
 * Return the column object handle from its name in the resultset
 *
 * @param rs    - Resultset handle
 * @param name  - Column name
 *
 * @note
 * The column name is case insensitive
 *
 * @return
 * - Column handle on success or
 * - NULL if no column found with the given name or on error
 *
 */

OCI_SYM_PUBLIC OCI_Column * OCI_API OCI_GetColumn2
(
    OCI_Resultset *rs,
    const otext *  name
);

/**
 * @brief
 * Return the index of the column in the result from its name
 *
 * @param rs    - Resultset handle
 * @param name  - Column name
 *
 * @note
 * The column name is case insensitive
 *
 * @note
 * Column indexes start with 1 in OCILIB
 *
 * @return
 * Column index on success or zero on error
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_GetColumnIndex
(
    OCI_Resultset *rs,
    const otext *  name
);

/**
 * @brief
 * Return the name of the given column
 *
 * @param col   - Column handle
 *
 */

OCI_SYM_PUBLIC const otext * OCI_API OCI_ColumnGetName
(
    OCI_Column *col
);

/**
 * @brief
 * Return the type of the given column
 *
 * @param col   - Column handle
 *
 * @note
 * Possible values are :
 *
 * - OCI_CDT_NUMERIC     : short, int, long long, float, double
 * - OCI_CDT_DATETIME    : OCI_Date *
 * - OCI_CDT_TEXT        : otext *
 * - OCI_CDT_LONG        : OCI_Long *
 * - OCI_CDT_CURSOR      : OCI_Statement *
 * - OCI_CDT_LOB         : OCI_Lob  *
 * - OCI_CDT_FILE        : OCI_File *
 * - OCI_CDT_TIMESTAMP   : OCI_Timestamp *
 * - OCI_CDT_INTERVAL    : OCI_Interval *
 * - OCI_CDT_RAW         : void *
 * - OCI_CDT_OBJECT      : OCI_Object *
 * - OCI_CDT_COLLECTION  : OCI_Coll *
 * - OCI_CDT_REF         : OCI_Ref *
 * - OCI_CDT_BOOLEAN     : boolean
 *
 * @return
 * The column type or OCI_CDT_UNKNOWN if index is out of bounds
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_ColumnGetType
(
    OCI_Column *col
);

/**
 * @brief
 * Return the charset form of the given column
 *
 * @param col   - Column handle
 *
 * @note
 * Possible values are :
 * - OCI_CSF_NONE     : the column is not an character or lob column
 * - OCI_CSF_DEFAULT  : the column has server default charset
 * - OCI_CSF_NATIONAL : the column has national server charset
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_ColumnGetCharsetForm
(
    OCI_Column *col
);

/**
 * @brief
 * Return the Oracle SQL type name of the column data type
 *
 * @param col   - Column handle
 *
 * @note
 * For possible values, consults Oracle Documentation
 *
 */

OCI_SYM_PUBLIC const otext * OCI_API OCI_ColumnGetSQLType
(
    OCI_Column *col
);

/**
 * @brief
 * Return the Oracle SQL Full name including precision and size of the
 * column data type
 *
 * @param col    - Column handle
 * @param buffer - buffer to store the full column type name and size
 * @param len    - max size of the buffer in characters
 *
 * @note
 * This function returns a description that matches the one given by SQL*Plus
 *
 * @note
 * Return the number of characters written into the buffer
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_ColumnGetFullSQLType
(
    OCI_Column * col,
    otext *      buffer,
    unsigned int len
);

/**
 * @brief
 * Return the size of the column
 *
 * @note
 * For all types, the size is expressed is bytes, excepted for character
 * based columns that were created with a character based size or of type NCHAR/NVARCHAR
 *
 * @param col   - Column handle
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_ColumnGetSize
(
    OCI_Column *col
);

/**
 * @brief
 * Return the scale of the column for numeric columns
 *
 * @param col   - Column handle
 *
 */

OCI_SYM_PUBLIC int OCI_API OCI_ColumnGetScale
(
    OCI_Column *col
);

/**
 * @brief
 * Return the precision of the column for numeric columns
 *
 * @param col   - Column handle
 *
 */

OCI_SYM_PUBLIC int OCI_API OCI_ColumnGetPrecision
(
    OCI_Column *col
);

/**
 * @brief
 * Return the fractional precision of the column for timestamp and interval columns
 *
 * @param col   - Column handle
 *
 */

OCI_SYM_PUBLIC int OCI_API OCI_ColumnGetFractionalPrecision
(
    OCI_Column *col
);

/**
 * @brief
 * Return the leading precision of the column for interval columns
 *
 * @param col   - Column handle
 *
 */

OCI_SYM_PUBLIC int OCI_API OCI_ColumnGetLeadingPrecision
(
    OCI_Column *col
);

/**
 * @brief
 * Return the nullable attribute of the column
 *
 * @param col   - Column handle
 *
 * @return
 * Return TRUE if the column is nullable otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_ColumnGetNullable
(
    OCI_Column *col
);

/**
 * @brief
 * Return TRUE if the length of the column is character-length or  FALSE if
 * it is byte-length
 *
 * @param col - Column handle
 *
 * @note
 * This was introduced in Oracle 9i. So for version that are not supporting this
 * property, it always return FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_ColumnGetCharUsed
(
    OCI_Column *col
);

/**
 * @brief
 * Return the column property flags
 *
 * @param col - Column handle
 *
 * For flags are:
 * - OCI_CPF_NONE : The column has no flags or the OCI client does not support this call
 * - OCI_CPF_IS_IDENTITY:
 *    - If Set, the column is an IDENTITY column
 *    - Otherwise, it is not an IDENTITY column
 * - OCI_CPF_IS_GEN_ALWAYS:
 *    - If set, means that the value is "ALWAYS GENERATED"
 *    - Otherwise means that the value is "GENERATED BY"
 * - OCI_CPF_IS_GEN_BY_DEFAULT_ON_NULL:
 *    - If set, means that the value is generated by default on NULL
 * - OCI_CPF_IS_LPART:
 *    - If set, Column is an implicitly generated logical partitioning column for container_map enabled object
 * - OCI_CPF_IS_CONID:
 *    - If set, Column is a CON_ID column implicitly generated by CONTAINERS() or is an ORIGIN_CON_ID column implicitly generated for Extended Data Link
 *
 * @note
 * This was introduced in Oracle 12cR1.
 * For earlier versions, it always return OCI_CPF_NONE
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_ColumnGetPropertyFlags
(
    OCI_Column *col
);

/**
* @brief
* Return the column collation ID
*
* @param col - Column handle
*
* Possible values:
*  - OCI_CCI_NONE  
*  - OCI_CCI_NLS_COMP 
*  - OCI_CCI_NLS_SORT
*  - OCI_CCI_NLS_SORT_CI
*  - OCI_CCI_NLS_SORT_AI
*  - OCI_CCI_NLS_SORT_CS
*  - OCI_CCI_NLS_SORT_VAR1 
*  - OCI_CCI_NLS_SORT_VAR1_CI
*  - OCI_CCI_NLS_SORT_VAR1_AI
*  - OCI_CCI_NLS_SORT_VAR1_CS
*  - OCI_CCI_BINARY
*  - OCI_CCI_BINARY_CI
*  - OCI_CCI_BINARY_AI
*
* @note
* This was introduced in Oracle 12cR2.
* For earlier versions, it always return OCI_CCI_NONE
*
*/

OCI_SYM_PUBLIC unsigned int OCI_API OCI_ColumnGetCollationID
(
    OCI_Column *col
);

/**
 * @brief
 * Return the type information object associated to the column
 *
 * @param col - Column handle
 *
 * @note
 * This call is used only for Named Object typed and collection columns.
 * It returns NULL if the column is not a Named Object or a collection.
 *
 */

OCI_SYM_PUBLIC OCI_TypeInfo * OCI_API OCI_ColumnGetTypeInfo
(
    OCI_Column *col
);

/**
 * @brief
 * Return the OCILIB object subtype of a column
 *
 * @param col - Column handle
 *
 * @note
 * This call is valid for the following OCILIB types:
 *
 * - OCI_CDT_LONG
 * - OCI_CDT_LOB
 * - OCI_CDT_FILE
 * - OCI_CDT_TIMESTAMP
 * - OCI_CDT_INTERVAL
 * - OCI_CDT_NUMERIC
 *
 * For OCI_Long type the possible values are:
 * - OCI_BLONG
 * - OCI_CLONG
 *
 * For OCI_Lob type the possible values are:
 * - OCI_BLOB
 * - OCI_CLOB
 * - OCI_NCLOB
 *
 * For OCI_File type the possible values are:
 * - OCI_BFILE
 * - OCI_CFILE
 *
 * For OCI_Timestamp type the possible values are:
 * - OCI_TIMESTAMP
 * - OCI_TIMESTAMP_TZ
 * - OCI_TIMESTAMP_LTZ
 *
 * For OCI_Interval type the possible values are:
 * - OCI_INTERVAL_YM
 * - OCI_INTERVAL_DS
 *
 * For numeric columns the possible values are:
 * - OCI_NUM_SHORT
 * - OCI_NUM_INT
 * - OCI_NUM_BIGINT
 * - OCI_NUM_USHORT
 * - OCI_NUM_UINT
 * - OCI_NUM_BIGUINT
 * - OCI_NUM_DOUBLE
 * - OCI_NUM_FLOAT
 * - OCI_NUM_NUMBER
 *
 * @warning
 * For numeric columns, the value may be not accurate at all!
 * OCI does not allow to find out the real SQL precise type of an numeric column (int, real, ...). 
 * OCI based libraries can only 'guess' some types in some situations : float, binary_float, binary_float, number.
 * For example:
 * - with the statement 'select 101 from dual', OCI would report numeric type NUMBER.
 * - if a column is declared as "INT", OCI would report also NUMBER.
 *  
 *
 * @note
 * For all other OCILIB types, it returns OCI_UNKNOWN
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_ColumnGetSubType
(
    OCI_Column *col
);

/**
 * @brief
 * set the numeric data type of the given structure member (identified from position in the
 * resultset)  to retrieve when calling OCI_GetStruct()
 *
 * @param rs    - Resultset handle
 * @param index - Column position
 * @param type  - Numeric type
 *
 * @note
 * Possible values for parameter 'type' :
 * - OCI_NUM_SHORT
 * - OCI_NUM_USHORT
 * - OCI_NUM_INT
 * - OCI_NUM_UINT
 * - OCI_NUM_BIGINT
 * - OCI_NUM_BIGUINT
 * - OCI_NUM_DOUBLE
 * - OCI_NUM_FLOAT
 * - OCI_NUM_NUMBER
 *
 * @return
 * Return TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_SetStructNumericType
(
    OCI_Resultset *rs,
    unsigned int   index,
    unsigned int   type
);

/**
 * @brief
 * set the numeric data type of the given structure member (identified from column name in the
 * resultset)  to retrieve when calling OCI_GetStruct()
 *
 * @param rs    - Resultset handle
 * @param name  - Column name
 * @param type  - Numeric type
 *
 * @note
 * Possible values for parameter 'type' :
 *   - OCI_NUM_SHORT
 *   - OCI_NUM_USHORT
 *   - OCI_NUM_INT
 *   - OCI_NUM_UINT
 *   - OCI_NUM_BIGINT
 *   - OCI_NUM_BIGUINT
 *   - OCI_NUM_DOUBLE
 *   - OCI_NUM_FLOAT
 *   - OCI_NUM_NUMBER
 *
 * @return
 * Return TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_SetStructNumericType2
(
    OCI_Resultset *rs,
    const otext *  name,
    unsigned int   type
);

/**
 * @brief
 * Return the row columns values into a single structure
 *
 * @param rs              - Resultset handle
 * @param row_struct      - pointer to user row structure
 * @param row_struct_ind  - pointer to user indicator structure
 *
 * @note
 * Structure members values are contextual to the current row.
 * The returned values can get out of scope when the current row
 * changes when calling any OCI_FecthXXX() calls
 *
 * @par User row structure
 *
 * The user structure must have the same members than the resultset.
 * Each column in the resultset must have its equivalent in the structure.
 * Fields must be in the same order.
 *
 * The mapping rules are :
 *
 *   - LOBs (CLOB, NCLOB, BLOB) : OCI_Lob *
 *   - DATE : OCI_Date *
 *   - TIMESTAMPS : OCI_Timestamp *
 *   - INTERVALS : OCI_Interval *
 *   - LONG, LONG RAW : OCI_Long *
 *   - REFs : OCI_Ref *
 *   - CURSOR, RESULSET : OCI_Statement *
 *   - OBJECTS, UDT : OCI_Object *
 *   - Character columns (CHAR,VARCHAR, etc..) : otext *
 *   - All NUMERIC types :
 *        - default : big_int
 *        - user defined (see OCI_SetStructNumericType())
 *
 * The user structure pointer is not mandatory
 *
 * @par User row indicator structure

 * This structure must have one boolean field per column in
 * the resultset and respect in the same member order.
 *
 * If the value of the given member is TRUE, it means the value in
 * the user row structure is NOT NULL, otherwise its NULL
 *
 * The user indicator structure pointer is  mandatory
 *
 * @return
 * Return TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_GetStruct
(
    OCI_Resultset *rs,
    void *         row_struct,
    void *         row_struct_ind
);

/**
* @brief
* Return the current Number value of the column at the given index in the resultset
*
* @param rs    - Resultset handle
* @param index - Column position
*
* @note
* Column position starts at 1.
*
* @return
* The column current row value or 0 if index is out of bounds
*
*/
OCI_SYM_PUBLIC OCI_Number * OCI_API OCI_GetNumber
(
    OCI_Resultset *rs,
    unsigned int   index
);

/**
* @brief
* Return the current number value of the column from its name in the resultset
*
* @param rs    - Resultset handle
* @param name  - Column name
*
* @note
* The column name is case insensitive
*
* @return
* The column current row value or 0 if no column found with the given name
*
*/

OCI_SYM_PUBLIC OCI_Number * OCI_API OCI_GetNumber2
(
    OCI_Resultset *rs,
    const otext *  name
);

/**
 * @brief
 * Return the current short value of the column at the given index in the resultset
 *
 * @param rs    - Resultset handle
 * @param index - Column position
 *
 * @note
 * Column position starts at 1.
 *
 * @return
 * The column current row value or 0 if index is out of bounds
 *
 */

OCI_SYM_PUBLIC short OCI_API OCI_GetShort
(
    OCI_Resultset *rs,
    unsigned int   index
);

/**
 * @brief
 * Return the current short value of the column from its name in the resultset
 *
 * @param rs    - Resultset handle
 * @param name  - Column name
 *
 * @note
 * The column name is case insensitive
 *
 * @return
 * The column current row value or 0 if no column found with the given name
 *
 */

OCI_SYM_PUBLIC short OCI_API OCI_GetShort2
(
    OCI_Resultset *rs,
    const otext *  name
);

/**
 * @brief
 * Return the current unsigned short value of the column at the given index in the resultset
 *
 * @param rs    - Resultset handle
 * @param index - Column position
 *
 * @note
 * Column position starts at 1.
 *
 * @return
 * The column current row value or 0 if index is out of bounds
 *
 */

OCI_SYM_PUBLIC unsigned short OCI_API OCI_GetUnsignedShort
(
    OCI_Resultset *rs,
    unsigned int   index
);

/**
 * @brief
 * Return the current unsigned short value of the column from its name in the resultset
 *
 * @param rs    - Resultset handle
 * @param name  - Column name
 *
 * @note
 * The column name is case insensitive
 *
 * @return
 * The column current row value or 0 if no column found with the given name
 *
 */

OCI_SYM_PUBLIC unsigned short OCI_API OCI_GetUnsignedShort2
(
    OCI_Resultset *rs,
    const otext *  name
);

/**
 * @brief
 * Return the current integer value of the column at the given index in the resultset
 *
 * @param rs    - Resultset handle
 * @param index - Column position
 *
 * @note
 * Column position starts at 1.
 *
 * @return
 * The column current row value or 0 if index is out of bounds
 *
 */

OCI_SYM_PUBLIC int OCI_API OCI_GetInt
(
    OCI_Resultset *rs,
    unsigned int   index
);

/**
 * @brief
 * Return the current integer value of the column from its name in the resultset
 *
 * @param rs    - Resultset handle
 * @param name  - Column name
 *
 * @note
 * The column name is case insensitive
 *
 * @return
 * The column current row value or 0 if no column found with the given name
 *
 */

OCI_SYM_PUBLIC int OCI_API OCI_GetInt2
(
    OCI_Resultset *rs,
    const otext *  name
);

/**
 * @brief
 * Return the current unsigned integer value of the column at the given index in the resultset
 *
 * @param rs    - Resultset handle
 * @param index - Column position
 *
 * @note
 * Column position starts at 1.
 *
 * @return
 * The column current row value or 0 if index is out of bounds
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_GetUnsignedInt
(
    OCI_Resultset *rs,
    unsigned int   index
);

/**
 * @brief
 * Return the current unsigned integer value of the column from its name in the resultset
 *
 * @param rs    - Resultset handle
 * @param name  - Column name
 *
 * @note
 * The column name is case insensitive
 *
 * @return
 * The column current row value or 0 if no column found with the given name
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_GetUnsignedInt2
(
    OCI_Resultset *rs,
    const otext *  name
);

/**
 * @brief
 * Return the current big integer value of the column at the given index in the resultset
 *
 * @param rs    - Resultset handle
 * @param index - Column position
 *
 * @note
 * Column position starts at 1.
 *
 * @return
 * The column current row value or 0 if index is out of bounds
 *
 */

OCI_SYM_PUBLIC big_int OCI_API OCI_GetBigInt
(
    OCI_Resultset *rs,
    unsigned int   index
);

/**
 * @brief
 * Return the current big integer value of the column from its name in the resultset
 *
 * @param rs    - Resultset handle
 * @param name  - Column name
 *
 * @note
 * The column name is case insensitive
 *
 * @return
 * The column current row value or 0 if no column found with the given name
 *
 */

OCI_SYM_PUBLIC big_int OCI_API OCI_GetBigInt2
(
    OCI_Resultset *rs,
    const otext *  name
);

/**
 * @brief
 * Return the current unsigned big integer value of the column at the given index in the resultset
 *
 * @param rs    - Resultset handle
 * @param index - Column position
 *
 * @note
 * Column position starts at 1.
 *
 * @return
 * The column current row value or 0 if index is out of bounds
 *
 */

OCI_SYM_PUBLIC big_uint OCI_API OCI_GetUnsignedBigInt
(
    OCI_Resultset *rs,
    unsigned int   index
);

/**
 * @brief
 * Return the current unsigned big integer value of the column from its name in the resultset
 *
 * @param rs    - Resultset handle
 * @param name  - Column name
 *
 * @note
 * The column name is case insensitive
 *
 * @return
 * The column current row value or 0 if no column found with the given name
 *
 */

OCI_SYM_PUBLIC big_uint OCI_API OCI_GetUnsignedBigInt2
(
    OCI_Resultset *rs,
    const otext *  name
);

/**
 * @brief
 * Return the current string value of the column at the given index in the resultset
 *
 * @param rs    - Resultset handle
 * @param index - Column position
 *
 * @note
 * Column position starts at 1.
 *
 * @note
 * OCI_GetString() performs an implicit conversion from  the
 * following data types:
 *
 * - Numerics (based on the current connection handle numeric format)
 * - Binary doubles and floats (using the standard C Library functions)
 * - OCI_Number (based on the current connection handle numeric format)
 * - OCI_Date (based on the current connection handle date format)
 * - OCI_Timestamp (based on the current connection handle date format)
 * - OCI_Interval (based on Oracle default conversion)
 * - OCI_Lob (for BLOBs, output is expressed in hexadecimal)
 * - OCI_Long  (for BLONGs, output is expressed in hexadecimal)
 * - OCI_File ("[directory]/[name]" will be output)
 * - OCI_Object (Textual SQL string representation)
 * - OCI_Coll (Textual SQL string representation)
 * - RAW buffer (expressed in hexadecimal)
 * - OCI_Statement (SQL statement string or cursor name)
 *
 * @return
 * The column current row value or NULL if index is out of bounds
 *
 */

OCI_SYM_PUBLIC const otext * OCI_API OCI_GetString
(
    OCI_Resultset *rs,
    unsigned int   index
);

/**
 * @brief
 * Return the current string value of the column from its name in the resultset
 *
 * @param rs    - Resultset handle
 * @param name  - Column name
 *
 * @note
 * The column name is case insensitive
 *
 * @return
 * The column current row value or NULL if no column found with the given name
 *
 */

OCI_SYM_PUBLIC const otext * OCI_API OCI_GetString2
(
    OCI_Resultset *rs,
    const otext *  name
);

/**
 * @brief
 * Copy the current raw value of the column at the given index into the specified buffer
 *
 * @param rs     - Resultset handle
 * @param index  - Column position
 * @param buffer - Buffer that receive the raw value
 * @param len    - Max size of the input buffer in bytes
 *
 * @note
 * Column position starts at 1.
 *
 * @return
 * Number of bytes copied into the buffer on SUCCESS otherwise 0
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_GetRaw
(
    OCI_Resultset *rs,
    unsigned int   index,
    void *         buffer,
    unsigned int   len
);

/**
 * @brief
 * Copy the current raw value of the column from its name into the specified buffer
 *
 * @param rs     - Resultset handle
 * @param name   - Column name
 * @param buffer - Buffer that receive the raw value
 * @param len    - Max size of the input buffer
 *
 * @note
 * The column name is case insensitive
 *
 * @return
 * Number of bytes copied into the buffer on SUCCESS otherwise 0
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_GetRaw2
(
    OCI_Resultset *rs,
    const otext *  name,
    void *         buffer,
    unsigned int   len
);

/**
 * @brief
 * Return the current double value of the column at the given index in the resultset
 *
 * @param rs    - Resultset handle
 * @param index - Column position
 *
 * @note
 * Column position starts at 1.
 *
 * @return
 * The column current row value or 0.O if index is out of bounds
 *
 */

OCI_SYM_PUBLIC double OCI_API OCI_GetDouble
(
    OCI_Resultset *rs,
    unsigned int   index
);

/**
 * @brief
 * Return the current double value of the column from its name in the resultset
 *
 * @param rs    - Resultset handle
 * @param name  - Column name
 *
 * @note
 * The column name is case insensitive
 *
 * @return
 * The column current row value or 0.0 if no column found with the given name
 *
 */

OCI_SYM_PUBLIC double OCI_API OCI_GetDouble2
(
    OCI_Resultset *rs,
    const otext *  name
);

/**
 * @brief
 * Return the current float value of the column at the given index in the resultset
 *
 * @param rs    - Resultset handle
 * @param index - Column position
 *
 * @note
 * Column position starts at 1.
 *
 * @return
 * The column current row value or 0.O if index is out of bounds
 *
 */

OCI_SYM_PUBLIC float OCI_API OCI_GetFloat
(
    OCI_Resultset *rs,
    unsigned int   index
);

/**
 * @brief
 * Return the current float value of the column from its name in the resultset
 *
 * @param rs    - Resultset handle
 * @param name  - Column name
 *
 * @note
 * The column name is case insensitive
 *
 * @return
 * The column current row value or 0.0 if no column found with the given name
 *
 */

OCI_SYM_PUBLIC float OCI_API OCI_GetFloat2
(
    OCI_Resultset *rs,
    const otext *  name
);

/**
 * @brief
 * Return the current date value of the column at the given index in the resultset
 *
 * @param rs    - Resultset handle
 * @param index - Column position
 *
 * @note
 * Column position starts at 1.
 *
 * @return
 * The column current row value or NULL if index is out of bounds
 *
 */

OCI_SYM_PUBLIC OCI_Date * OCI_API OCI_GetDate
(
    OCI_Resultset *rs,
    unsigned int   index
);

/**
 * @brief
 * Return the current date value of the column from its name in the resultset
 *
 * @param rs    - Resultset handle
 * @param name  - Column name
 *
 * @return
 * The column current row value or NULL if no column found with the given name
 *
 */

OCI_SYM_PUBLIC OCI_Date * OCI_API OCI_GetDate2
(
    OCI_Resultset *rs,
    const otext *  name
);

/**
 * @brief
 * Return the current timestamp value of the column at the given index in the resultset
 *
 * @param rs    - Resultset handle
 * @param index - Column position
 *
 * @note
 * Column position starts at 1.
 *
 * @return
 * The column current row value or NULL if index is out of bounds
 *
 */

OCI_SYM_PUBLIC OCI_Timestamp * OCI_API OCI_GetTimestamp
(
    OCI_Resultset *rs,
    unsigned int   index
);

/**
 * @brief
 * Return the current timestamp value of the column from its name in the resultset
 *
 * @param rs    - Resultset handle
 * @param name  - Column name
 *
 * @return
 * The column current row value or NULL if no column found with the given name
 *
 */

OCI_SYM_PUBLIC OCI_Timestamp * OCI_API OCI_GetTimestamp2
(
    OCI_Resultset *rs,
    const otext *  name
);

/**
 * @brief
 * Return the current interval value of the column at the given index in the resultset
 *
 * @param rs    - Resultset handle
 * @param index - Column position
 *
 * @note
 * Column position starts at 1.
 *
 * @return
 * The column current row value or NULL if index is out of bounds
 *
 */

OCI_SYM_PUBLIC OCI_Interval * OCI_API OCI_GetInterval
(
    OCI_Resultset *rs,
    unsigned int   index
);

/**
 * @brief
 * Return the current interval value of the column from its name in the resultset
 *
 * @param rs    - Resultset handle
 * @param name  - Column name
 *
 * @return
 * The column current row value or NULL if no column found with the given name
 *
 */

OCI_SYM_PUBLIC OCI_Interval * OCI_API OCI_GetInterval2
(
    OCI_Resultset *rs,
    const otext *  name
);

/**
 * @brief
 * Return the current cursor value (Nested table) of the column at the given index in the resultset
 *
 * @param rs    - Resultset handle
 * @param index - Column position
 *
 * @note
 * Column position starts at 1.
 *
 * @return
 * The column current row value or NULL if index is out of bounds
 *
 */

OCI_SYM_PUBLIC OCI_Statement * OCI_API OCI_GetStatement
(
    OCI_Resultset *rs,
    unsigned int   index
);

/**
 * @brief
 * Return the current cursor value of the column from its name in the resultset
 *
 * @param rs    - Resultset handle
 * @param name  - Column name
 *
 * @return
 * The column current row value or NULL if no column found with the given name
 *
 */

OCI_SYM_PUBLIC OCI_Statement * OCI_API OCI_GetStatement2
(
    OCI_Resultset *rs,
    const otext *  name
);

/**
 * @brief
 * Return the current lob value of the column at the given index in the resultset
 *
 * @param rs    - Resultset handle
 * @param index - Column position
 *
 * @note
 * Column position starts at 1.
 *
 * @return
 * The column current row value or NULL if index is out of bounds
 *
 */

OCI_SYM_PUBLIC OCI_Lob * OCI_API OCI_GetLob
(
    OCI_Resultset *rs,
    unsigned int   index
);

/**
 * @brief
 * Return the current lob value of the column from its name in the resultset
 *
 * @param rs    - Resultset handle
 * @param name  - Column name
 *
 * @return
 * The column current row value or NULL if no column found with the given name
 *
 */

OCI_SYM_PUBLIC OCI_Lob * OCI_API OCI_GetLob2
(
    OCI_Resultset *rs,
    const otext *  name
);

/**
 * @brief
 * Return the current File value of the column at the given index in the resultset
 *
 * @param rs    - Resultset handle
 * @param index - Column position
 *
 * @note
 * Column position starts at 1.
 *
 * @return
 * The column current row value or NULL if index is out of bounds
 *
 */

OCI_SYM_PUBLIC OCI_File * OCI_API OCI_GetFile
(
    OCI_Resultset *rs,
    unsigned int   index
);

/**
 * @brief
 * Return the current File value of the column from its name in the resultset
 *
 * @param rs    - Resultset handle
 * @param name  - Column name
 *
 * @return
 * The column current row value or NULL if no column found with the given name
 *
 */

OCI_SYM_PUBLIC OCI_File * OCI_API OCI_GetFile2
(
    OCI_Resultset *rs,
    const otext *  name
);

/**
 * @brief
 * Return the current Object value of the column at the given index in the resultset
 *
 * @param rs    - Resultset handle
 * @param index - Column position
 *
 * @note
 * Column position starts at 1.
 *
 * @return
 * The column current row value or NULL if index is out of bounds
 *
 */

OCI_SYM_PUBLIC OCI_Object * OCI_API OCI_GetObject
(
    OCI_Resultset *rs,
    unsigned int   index
);

/**
 * @brief
 * Return the current Object value of the column from its name in the resultset
 *
 * @param rs    - Resultset handle
 * @param name  - Column name
 *
 * @return
 * The column current row value or NULL if no column found with the given name
 *
 */

OCI_SYM_PUBLIC OCI_Object * OCI_API OCI_GetObject2
(
    OCI_Resultset *rs,
    const otext *  name
);

/**
 * @brief
 * Return the current Collection value of the column at the given index in the resultset
 *
 * @param rs    - Resultset handle
 * @param index - Column position
 *
 * @note
 * Column position starts at 1.
 *
 * @return
 * The column current row value or NULL if index is out of bounds
 *
 */

OCI_SYM_PUBLIC OCI_Coll * OCI_API OCI_GetColl
(
    OCI_Resultset *rs,
    unsigned int   index
);

/**
 * @brief
 * Return the current Collection value of the column from its name in the resultset
 *
 * @param rs    - Resultset handle
 * @param name  - Column name
 *
 * @return
 * The column current row value or NULL if no column found with the given name
 *
 */

OCI_SYM_PUBLIC OCI_Coll * OCI_API OCI_GetColl2
(
    OCI_Resultset *rs,
    const otext *  name
);

/**
 * @brief
 * Return the current Ref value of the column at the given index in the resultset
 *
 * @param rs    - Resultset handle
 * @param index - Column position
 *
 * @note
 * Column position starts at 1.
 *
 * @return
 * The column current row value or NULL if index is out of bounds
 *
 */

OCI_SYM_PUBLIC OCI_Ref * OCI_API OCI_GetRef
(
    OCI_Resultset *rs,
    unsigned int   index
);

/**
 * @brief
 * Return the current Ref value of the column from its name in the resultset
 *
 * @param rs    - Resultset handle
 * @param name  - Column name
 *
 * @return
 * The column current row value or NULL if no column found with the given name
 *
 */

OCI_SYM_PUBLIC OCI_Ref * OCI_API OCI_GetRef2
(
    OCI_Resultset *rs,
    const otext *  name
);

/**
 * @brief
 * Return the current Long value of the column at the given index in the resultset
 *
 * @param rs    - Resultset handle
 * @param index - Column position
 *
 * @note
 * Column position starts at 1.
 *
 * @return
 * The column current row value or NULL if index is out of bounds
 *
 */

OCI_SYM_PUBLIC OCI_Long * OCI_API OCI_GetLong
(
    OCI_Resultset *rs,
    unsigned int   index
);

/**
 * @brief
 * Return the current Long value of the column from its name in the resultset
 *
 * @param rs    - Resultset handle
 * @param name  - Column name
 *
 * @return
 * The column current row value or NULL if no column found with the given name
 *
 */

OCI_SYM_PUBLIC OCI_Long * OCI_API OCI_GetLong2
(
    OCI_Resultset *rs,
    const otext *  name
);

/**
 * @brief
 * Check if the current row value is null for the column at the given index in the resultset
 *
 * @param rs    - Resultset handle
 * @param index - Column position
 *
 * @note
 * Column position starts at 1.
 *
 * @return
 * TRUE if it's null otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_IsNull
(
    OCI_Resultset *rs,
    unsigned int   index
);

/**
 * @brief
 * Return the size of the value of the column at the given index in the resultset
 *
 * @param rs    - Resultset handle
 * @param index - Column position
 *
 * @note
 * Column position starts at 1.
 *
 * @warning
 * For binds of type OCI_CDT_TEXT (strings), the returned value is expressed in
 * number of characters.
 *
 * @return value size of 0 if the value is NULL
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_GetDataSize
(
    OCI_Resultset *rs,
    unsigned int   index
);

/**
 * @brief
 * Return the size of the value of the column from its name in the resultset
 *
 * @param rs    - Resultset handle
 * @param name  - Column name
 *
 * @warning
 * For binds of type OCI_CDT_TEXT (strings), the returned value is expressed in
 * number of characters.
 *
 * @return value size of 0 if the value is NULL
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_GetDataSize2
(
    OCI_Resultset *rs,
    const otext *  name
);

/**
 * @brief
 * Check if the current row value is null for the column of the given name in the resultset
 *
 * @param rs    - Resultset handle
 * @param name  - Column name
 *
 * @return
 * TRUE if it's null otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_IsNull2
(
    OCI_Resultset *rs,
    const otext *  name
);

/**
 * @brief
 * Return the statement handle associated with a resultset handle
 *
 * @param rs - resultset handle
 *
 */

OCI_SYM_PUBLIC OCI_Statement * OCI_API OCI_ResultsetGetStatement
(
    OCI_Resultset *rs
);

/**
 * @brief
 * Return the current row data length of the column at the given index in the resultset
 *
 * @param rs    - Resultset handle
 * @param index - Column position
 *
 * @note
 * Column position starts at 1.
 *
 * @return
 * The column current row data length or 0 if index is out of bounds
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_GetDataLength
(
    OCI_Resultset *rs,
    unsigned int   index
);

/**
 * @} OcilibCApiFetching
 */

/**
 * @defgroup OcilibCApiPlSql PL/SQL Support
 * @{
 *
 * OCILIB has a strong PL/SQL support :
 *
 * - Blocks, procedures and function can be used with OCILIB statements.
 * - Ref cursors
 * - Nested tables
 * - Tables (indexed by integer types)
 * - Access to the server side output generated by the DBMS_OUTPUT package
 *
 * Stored procedures/functions calls, blocks declarations are done like regular
 * SQL calls using OCI_Prepare(), OCI_Execute(), OCI_ExecuteStmt() and
 * OCI_ExecuteStmtFmt() functions.
 *
 * All PL/SQL statements must:
 *
 * - start with a 'begin' or 'declare' keyword
 * - end with a 'end;' keyword
 *
 * Binding Host arrays to PL/SQL tables is done with OCI_BindArrayXXX() calls
 *
 * @par Using a PL/SQL block with OCILIB
 * @include plsql_block.c
 *
 * @par Binding host arrays to PL/SQL tables parameters of a stored procedure
 * @include plsql_table.c
 *
 * @par Retrieve the output generated by the dbms_output package on the server
 * @include output.c
 *
 */

/**
 * @brief
 * Enable the server output
 *
 * @param con     - Connection handle
 * @param bufsize - server buffer max size (server side)
 * @param arrsize - number of lines to retrieve per server round-trip
 * @param lnsize  - maximum size of one line
 *
 * @note
 * This call is equivalent to the command 'set serveroutput on' in SQL*PLUS
 *
 * @note
 * 'bufsize' minimum value is 2000, maximum 1000000 with Oracle < 10.2g and can be unlimited above
 *
 * @note
 * 'lnsize' maximum value is 255 with Oracle < 10g R2 and 32767 above
 *
 * @warning
 * If OCI_ServerEnableOutput() is not called, OCI_ServerGetOutput() will return NULL
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_ServerEnableOutput
(
    OCI_Connection *con,
    unsigned int    bufsize,
    unsigned int    arrsize,
    unsigned int    lnsize
);

/**
 * @brief
 * Disable the server output
 *
 * @param con - Connection handle
 *
 * @note
 * After this call, OCI_ServerGetOutput() will return NULL.
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_ServerDisableOutput
(
    OCI_Connection *con
);

/**
 * @brief
 * Retrieve one line of the server buffer
 *
 * @param con   - Connection handle
 *
 * @note
 * Internally, OCILIB gets the server buffer through an array of lines in
 * order to minimize round-trips with the server
 *
 * @return
 * return a server output buffer line or NULL if the server buffer is empty
 *
 */

OCI_SYM_PUBLIC const otext * OCI_API OCI_ServerGetOutput
(
    OCI_Connection *con
);

/**
 * @} OcilibCApiPlSql
 */

/**
 * @defgroup OcilibCApiCollections Oracle collections (VARRAYS and Nested Tables)
 * @{
 *
 * OCILIB supports all Oracle collections:
 *
 * - PL/SQL Tables: only available in PL/SQL, unbounded, sparse arrays of
                    homogeneous elements.
 * - VARRAYS      : available in SQL and PL/SQL, they are bounded arrays of
 *                  homogeneous elements
 * - Nested Tables: available in SQL and PL/SQL, they are unbounded arrays of
 *                  homogeneous elements and can become sparse through deletions
 *
 * PL/SQL tables are implemented by binding regular C arrays with the array
 * interface (using OCI_BindArrayOfXXX() calls)
 *
 * VARRAYS and Nested tables are implemented in OCILIB with the type OCI_Coll.
 * It's possible to bind and fetch VARRAYS and Nested tables using OCI_Coll handle.
 *
 * It's also possible to declare local collections based on some database type without using queries
 *
 * OCI (and thus OCILIB) offers the possibility to access collection elements :
 *
 * - directly by index (OCI_CollGetElem() and OCI_CollSetElem())
 * - using an iterator (OCI_Iter) to iterate through the collection
 *   (OCI_IterGetNext(), OCI_IterGetPrev())
 *
 * Collection Items are implemented through the type OCI_Elem and use the series
 * of calls OCI_ElemGetXXX() and OCI_ElemSetXXX() to manipulate elements
 * content values
 *
 * @par Example
 * @include coll.c
 *
 */

/**
 * @brief
 * Create a local collection instance
 *
 * @param typinf - Type info handle of the collection type descriptor
 *
 * @return
 * Return the collection object handle on success otherwise NULL on failure
 *
 */

OCI_SYM_PUBLIC OCI_Coll * OCI_API OCI_CollCreate
(
    OCI_TypeInfo *typinf
);

/**
 * @brief
 * Free a local collection
 *
 * @param coll - Collection handle
 *
 * @warning
 * Only collection created with OCI_CollCreate() should be freed
 * by OCI_CollFree()
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_CollFree
(
    OCI_Coll *coll
);

/**
 * @brief
 * Create an array of Collection object
 *
 * @param con    - Connection handle
 * @param typinf - Object type (type info handle)
 * @param nbelem - number of elements in the array
 *
 * @note
 * see OCI_ObjectCreate() for more details
 *
 * @return
 * Return the Collection handle array on success otherwise NULL on failure
 *
 */

OCI_SYM_PUBLIC OCI_Coll ** OCI_API OCI_CollArrayCreate
(
    OCI_Connection *con,
    OCI_TypeInfo *  typinf,
    unsigned int    nbelem
);

/**
 * @brief
 * Free an array of Collection objects
 *
 * @param colls - Array of Collection objects
 *
 * @warning
 * Only arrays of Collection created with OCI_CollArrayCreate()
 * should be freed by OCI_CollArrayFree()
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_CollArrayFree
(
    OCI_Coll **colls
);

/**
 * @brief
 * Assign a collection to another one
 *
 * @param coll     - Destination Collection handle
 * @param coll_src - Source Collection handle
 *
 * @note
 * Oracle proceeds to a deep copy of the collection content
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_CollAssign
(
    OCI_Coll *coll,
    OCI_Coll *coll_src
);

/**
 * @brief
 * Return the type info object associated to the collection
 *
 * @param coll - Collection handle
 *
 */

OCI_SYM_PUBLIC OCI_TypeInfo * OCI_API OCI_CollGetTypeInfo
(
    OCI_Coll *coll
);

/**
 * @brief
 * Return the collection type
 *
 * @param coll - Collection handle
 *
 * @note
 * Current collection types are:
 *
 * - OCI_COLL_VARRAY: Oracle VARRAY
 * - OCI_COLL_NESTED_TABLE: Oracle Nested Table
 *
 * @return
 * Collection type or OCI_UNKNOWN if the collection handle is null
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_CollGetType
(
    OCI_Coll *coll
);

/**
 * @brief
 * Returns the maximum number of elements of the given collection.
 *
 * @param coll - Collection handle
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_CollGetMax
(
    OCI_Coll *coll
);

/**
 * @brief
 * Returns the total number of elements of the given collection.
 *
 * @param coll - Collection handle
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_CollGetSize
(
    OCI_Coll *coll
);

/**
 * @brief
 * Returns the current number of elements of the given collection.
 *
 * @note
 * - For VARRAYs, it returns the same value than OCI_CollGetSize() as VARRAYs cannot contains holes
 * - For Nested Tables that are spare collections that can have holes, it returns the total number
 *   of elements minus the total of deleted elements
 *
 * @param coll - Collection handle
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_CollGetCount
(
    OCI_Coll *coll
);

/**
 * @brief
 * Trims the given number of elements from the end of the collection
 *
 * @param coll    - Collection handle
 * @param nb_elem - Number of elements to trim
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_CollTrim
(
    OCI_Coll *   coll,
    unsigned int nb_elem
);

/**
 * @brief
 * clear all items of the given collection
 *
 * @param coll - Collection handle
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_CollClear
(
    OCI_Coll *coll
);

/**
 * @brief
 * Return the element at the given position in the collection
 *
 * @param coll  - Collection handle
 * @param index - Index of the destination element
 *
 * @note
 * Collection indexes start at position 1.
 *
 * @return
 * Element handle on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC OCI_Elem * OCI_API OCI_CollGetElem
(
    OCI_Coll *   coll,
    unsigned int index
);

/**
 * @brief
 * Return the element at the given position in the collection
 *
 * @param coll  - Collection handle
 * @param index - Index of the destination element
 * @param elem  - Element handle to hold the collection item data
 *
 * @note
 * Collection indexes start at position 1.
 *
 * @return
 * Element handle on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_CollGetElem2
(
    OCI_Coll *   coll,
    unsigned int index,
    OCI_Elem *   elem
);

/**
 * @brief
 * Assign the given element value to the element at the given position in
 * the collection
 *
 * @param coll  - Collection handle
 * @param index - Index of the destination element
 * @param elem  - Source element handle to assign
 *
 * @note
 * Collection indexes start at position 1.
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_CollSetElem
(
    OCI_Coll *   coll,
    unsigned int index,
    OCI_Elem *   elem
);

/**
 * @brief
 * Append the given element at the end of the collection
 *
 * @param coll  - Collection handle
 * @param elem  - Element handle to add
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_CollAppend
(
    OCI_Coll *coll,
    OCI_Elem *elem
);

/**
 * @brief
 * Convert a collection handle value to a string
 *
 * @param coll - Collection handle
 * @param size - Destination string length pointer in characters
 * @param str  - Destination string
 *
 * @note
 * In order to compute the needed string length, call the method with a NULL string
 * Then call the method again with a valid buffer
 *
 * @note
 * The resulting string is similar to the SQL*PLUS output for collections
 * For RAWs and BLOBs attributes, their binary values are converted to hexadecimal strings
 *
 * @warning
 * This convenient method shall not be used when performance matters. It is usually called twice (buffer length
 * computation) and must also care about quotes within strings.
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_CollToText
(
    OCI_Coll *    coll,
    unsigned int *size,
    otext *       str
);

/**
 * @brief
 * Delete the element at the given position in the Nested Table Collection
 *
 * @param coll  - Collection handle
 * @param index - Index of the element to delete
 *
 * @note
 * Collection indexes start at position 1.
 *
 * @warning
 * OCI_CollDeleteElem() is only valid for nested tables.
 *
 * @return
 * - if the input collection is a nested table, it returns TRUE if the element
 *   is successfully deleted otherwise FALSE on error
 * - if the input collection is a VARRAY, it always returns FALSE without spawning an exception
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_CollDeleteElem
(
    OCI_Coll *   coll,
    unsigned int index
);

/**
 * @brief
 * Create an iterator handle to iterate through a collection
 *
 * @param coll  - Collection handle
 *
 * @return
 * Return the iterator handle on success otherwise NULL on failure
 *
 */

OCI_SYM_PUBLIC OCI_Iter * OCI_API OCI_IterCreate
(
    OCI_Coll *coll
);

/**
 * @brief
 * Free an iterator handle
 *
 * @param iter - Iterator handle
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_IterFree
(
    OCI_Iter *iter
);

/**
 * @brief
 * Get the next element in the collection
 *
 * @param iter - Iterator handle
 *
 * @return
 * Element handle on success otherwise NULL if:
 * - Empty collection
 * - Iterator already positioned on the last collection element
 * - An error occurred
 *
 */

OCI_SYM_PUBLIC OCI_Elem * OCI_API OCI_IterGetNext
(
    OCI_Iter *iter
);

/**
 * @brief
 * Get the previous element in the collection
 *
 * @param iter - Iterator handle
 *
 * @return
 * Element handle on success otherwise NULL if:
 * - Empty collection
 * - Iterator already positioned on the last collection element
 * - An error occurred
 *
 */

OCI_SYM_PUBLIC OCI_Elem * OCI_API OCI_IterGetPrev
(
    OCI_Iter *iter
);

/**
 * @brief
 * Get the current element in the collection
 *
 * @param iter - Iterator handle
 *
 * @return
 * Element handle on success otherwise NULL if:
 * - Empty collection
 * - Iterator already positioned on the last collection element
 * - An error occurred
 *
 */

OCI_SYM_PUBLIC OCI_Elem * OCI_API OCI_IterGetCurrent
(
    OCI_Iter *iter
);

/**
 * @brief
 * Create a local collection element instance based on a collection type
 * descriptor
 *
 * @param typinf  - Type info handle
 *
 * @return
 * Return the collection element handle on success otherwise NULL on failure
 *
 */

OCI_SYM_PUBLIC OCI_Elem * OCI_API OCI_ElemCreate
(
    OCI_TypeInfo *typinf
);

/**
 * @brief
 * Free a local collection element
 *
 * @param elem   - Element handle
 *
 * @warning
 * Only element created with OCI_ElemCreate() should be freed
 * by OCI_ElemFree()
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_ElemFree
(
    OCI_Elem *elem
);

/**
* @brief
* Return the boolean value of the given collection element
*
* @param elem   - Element handle
*
* @warning
* OCI_ElemGetBoolean() returns a valid value only for collection elements of PL/SQL boolean type
*
* @return
* boolean value or FALSE on failure
*
*/

OCI_SYM_PUBLIC boolean OCI_API OCI_ElemGetBoolean
(
    OCI_Elem *elem
);

/**
* @brief
* Return the number value of the given collection element
*
* @param elem   - Element handle
*
* @return
* number handle or NULL on failure
*
*/

OCI_SYM_PUBLIC OCI_Number * OCI_API OCI_ElemGetNumber
(
    OCI_Elem *elem
);

/**
 * @brief
 * Return the short value of the given collection element
 *
 * @param elem   - Element handle
 *
 * @return
 * Short value or 0 on failure
 *
 */

OCI_SYM_PUBLIC short OCI_API OCI_ElemGetShort
(
    OCI_Elem *elem
);

/**
 * @brief
 * Return the unsigned short value of the given collection element
 *
 * @param elem   - Element handle
 *
 * @return
 * Unsigned short value or 0 on failure
 *
 */

OCI_SYM_PUBLIC unsigned short OCI_API OCI_ElemGetUnsignedShort
(
    OCI_Elem *elem
);

/**
 * @brief
 * Return the int value of the given collection element
 *
 * @param elem   - Element handle
 *
 * @return
 * Int value or 0 on failure
 *
 */

OCI_SYM_PUBLIC int OCI_API OCI_ElemGetInt
(
    OCI_Elem *elem
);

/**
 * @brief
 * Return the unsigned int value of the given collection element
 *
 * @param elem   - Element handle
 *
 * @return
 * Unsigned int value or 0 on failure
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_ElemGetUnsignedInt
(
    OCI_Elem *elem
);

/**
 * @brief
 * Return the big int value of the given collection element
 *
 * @param elem   - Element handle
 *
 * @return
 * Big int value or 0 on failure
 *
 */

OCI_SYM_PUBLIC big_int OCI_API OCI_ElemGetBigInt
(
    OCI_Elem *elem
);

/**
 * @brief
 * Return the unsigned big int value of the given collection element
 *
 * @param elem   - Element handle
 *
 * @return
 * Unsigned big int value or 0 on failure
 *
 */

OCI_SYM_PUBLIC big_uint OCI_API OCI_ElemGetUnsignedBigInt
(
    OCI_Elem *elem
);

/**
 * @brief
 * Return the Double value of the given collection element
 *
 * @param elem   - Element handle
 *
 * @return
 * Double value or 0 on failure
 *
 */

OCI_SYM_PUBLIC double OCI_API OCI_ElemGetDouble
(
    OCI_Elem *elem
);

/**
 * @brief
 * Return the float value of the given collection element
 *
 * @param elem   - Element handle
 *
 * @return
 * Double value or 0 on failure
 *
 */

OCI_SYM_PUBLIC float OCI_API OCI_ElemGetFloat
(
    OCI_Elem *elem
);

/**
 * @brief
 * Return the String value of the given collection element
 *
 * @param elem   - Element handle
 *
 * @return
 * String value or NULL on failure
 *
 */

OCI_SYM_PUBLIC const otext * OCI_API OCI_ElemGetString
(
    OCI_Elem *elem
);

/**
 * @brief
 * Read the RAW value of the collection element into the given buffer
 *
 * @param elem   - Element handle
 * @param value  - Buffer to store the RAW value
 * @param len    - Size of the buffer
 *
 * @return
 * Number of bytes read from the RAW value or 0 on failure
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_ElemGetRaw
(
    OCI_Elem *   elem,
    void *       value,
    unsigned int len
);

/**
* @brief
* Return the raw attribute value size of the given element handle
*
* @param elem   - Element handle
*
* @return
* size in bytes of the RAW value or 0 on failure or wrong attribute type
*
*/

OCI_SYM_PUBLIC unsigned int OCI_API OCI_ElemGetRawSize
(
    OCI_Elem *elem
);

/**
 * @brief
 * Return the Date value of the given collection element
 *
 * @param elem   - Element handle
 *
 * @return
 * Date handle or NULL on failure
 *
 */

OCI_SYM_PUBLIC OCI_Date * OCI_API OCI_ElemGetDate
(
    OCI_Elem *elem
);

/**
 * @brief
 * Return the Timestamp value of the given collection element
 *
 * @param elem   - Element handle
 *
 * @return
 * Timestamp handle or NULL on failure
 *
 */

OCI_SYM_PUBLIC OCI_Timestamp * OCI_API OCI_ElemGetTimestamp
(
    OCI_Elem *elem
);

/**
 * @brief
 * Return the Interval value of the given collection element
 *
 * @param elem   - Element handle
 *
 * @return
 * Interval handle or NULL on failure
 *
 */

OCI_SYM_PUBLIC OCI_Interval * OCI_API OCI_ElemGetInterval
(
    OCI_Elem *elem
);

/**
 * @brief
 * Return the Lob value of the given collection element
 *
 * @param elem   - Element handle
 *
 * @return
 * Lob handle or NULL on failure
 *
 */

OCI_SYM_PUBLIC OCI_Lob * OCI_API OCI_ElemGetLob
(
    OCI_Elem *elem
);

/**
 * @brief
 * Return the File value of the given collection element
 *
 * @param elem   - Element handle
 *
 * @return
 * File handle or NULL on failure
 *
 */

OCI_SYM_PUBLIC OCI_File * OCI_API OCI_ElemGetFile
(
    OCI_Elem *elem
);

/**
 * @brief
 * Return the object value of the given collection element
 *
 * @param elem   - Element handle
 *
 * @return
 * Object handle or NULL on failure
 *
 */

OCI_SYM_PUBLIC OCI_Object * OCI_API OCI_ElemGetObject
(
    OCI_Elem *elem
);

/**
 * @brief
 * Return the collection value of the given collection element
 *
 * @param elem   - Element handle
 *
 * @return
 * Collection handle or NULL on failure
 *
 */

OCI_SYM_PUBLIC OCI_Coll * OCI_API OCI_ElemGetColl
(
    OCI_Elem *elem
);

/**
 * @brief
 * Return the Ref value of the given collection element
 *
 * @param elem   - Element handle
 *
 * @return
 * Ref handle or NULL on failure
 *
 */

OCI_SYM_PUBLIC OCI_Ref * OCI_API OCI_ElemGetRef
(
    OCI_Elem *elem
);

/**
* @brief
* Set a boolean value to a collection element
*
* @param elem   - Element handle
* @param value  - Short value
*
* @warning
* OCI_ElemSetBoolean() is only valid value only for collection elements of PL / SQL boolean type
*
* @return
* TRUE on success otherwise FALSE
*
*/

OCI_SYM_PUBLIC boolean OCI_API OCI_ElemSetBoolean
(
    OCI_Elem *elem,
    boolean   value
);

/**
* @brief
* Set a number value to a collection element
*
* @param elem   - Element handle
* @param value  - number value
*
* @return
* TRUE on success otherwise FALSE
*
*/

OCI_SYM_PUBLIC boolean OCI_API OCI_ElemSetNumber
(
    OCI_Elem *  elem,
    OCI_Number *value
);

/**
 * @brief
 * Set a short value to a collection element
 *
 * @param elem   - Element handle
 * @param value  - Short value
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_ElemSetShort
(
    OCI_Elem *elem,
    short     value
);

/**
 * @brief
 * Set a unsigned short value to a collection element
 *
 * @param elem   - Element handle
 * @param value  - Unsigned short value
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_ElemSetUnsignedShort
(
    OCI_Elem *     elem,
    unsigned short value
);

/**
 * @brief
 * Set a int value to a collection element
 *
 * @param elem   - Element handle
 * @param value  - Int value
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_ElemSetInt
(
    OCI_Elem *elem,
    int       value
);

/**
 * @brief
 * Set a unsigned int value to a collection element
 *
 * @param elem   - Element handle
 * @param value  - Unsigned int value
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_ElemSetUnsignedInt
(
    OCI_Elem *   elem,
    unsigned int value
);

/**
 * @brief
 * Set a big int value to a collection element
 *
 * @param elem   - Element handle
 * @param value  - big int value
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_ElemSetBigInt
(
    OCI_Elem *elem,
    big_int   value
);

/**
 * @brief
 * Set a unsigned big_int value to a collection element
 *
 * @param elem   - Element handle
 * @param value  - Unsigned big int value
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_ElemSetUnsignedBigInt
(
    OCI_Elem *elem,
    big_uint  value
);

/**
 * @brief
 * Set a double value to a collection element
 *
 * @param elem   - Element handle
 * @param value  - Double value
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_ElemSetDouble
(
    OCI_Elem *elem,
    double    value
);

/**
 * @brief
 * Set a float value to a collection element
 *
 * @param elem   - Element handle
 * @param value  - float value
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_ElemSetFloat
(
    OCI_Elem *elem,
    float     value
);

/**
 * @brief
 * Set a string value to a collection element
 *
 * @param elem   - Element handle
 * @param value  - String value
 *
 * @note
 * passing a null pointer for value calls OCI_ElemSetNull()
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_ElemSetString
(
    OCI_Elem *   elem,
    const otext *value
);

/**
 * @brief
 * Set a RAW value to a collection element
 *
 * @param elem   - Element handle
 * @param value  - Raw value
 * @param len    - Size of the raw value
 *
 * @note
 * passing a null pointer for value calls OCI_ElemSetNull()
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_ElemSetRaw
(
    OCI_Elem *   elem,
    void *       value,
    unsigned int len
);

/**
 * @brief
 * Assign a Date handle to a collection element
 *
 * @param elem   - Element handle
 * @param value  - Date Handle
 *
 * @note
 * passing a null pointer for value calls OCI_ElemSetNull()
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_ElemSetDate
(
    OCI_Elem *elem,
    OCI_Date *value
);

/**
 * @brief
 * Assign a Timestamp handle to a collection element
 *
 * @param elem   - Element handle
 * @param value  - Timestamp Handle
 *
 * @note
 * passing a null pointer for value calls OCI_ElemSetNull()
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_ElemSetTimestamp
(
    OCI_Elem *     elem,
    OCI_Timestamp *value
);

/**
 * @brief
 * Assign an Interval handle to a collection element
 *
 * @param elem   - Element handle
 * @param value  - Interval Handle
 *
 * @note
 * passing a null pointer for value calls OCI_ElemSetNull()
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_ElemSetInterval
(
    OCI_Elem *    elem,
    OCI_Interval *value
);

/**
 * @brief
 * Assign a Collection handle to a collection element
 *
 * @param elem   - Element handle
 * @param value  - Collection Handle
 *
 * @note
 * passing a null pointer for value calls OCI_ElemSetNull()
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_ElemSetColl
(
    OCI_Elem *elem,
    OCI_Coll *value
);

/**
 * @brief
 * Assign an Object handle to a collection element
 *
 * @param elem   - Element handle
 * @param value  - Object Handle
 *
 * @warning
 * This function assigns a copy of the object to the given attribute.
 * Any further modifications of the object passed as the parameter 'value'
 * will not be reflected to object 's attribute set with this call
 *
 * @note
 * passing a null pointer for value calls OCI_ElemSetNull()
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_ElemSetObject
(
    OCI_Elem *  elem,
    OCI_Object *value
);

/**
 * @brief
 * Assign a Lob handle to a collection element
 *
 * @param elem   - Element handle
 * @param value  - Lob Handle
 *
 * @note
 * passing a null pointer for value calls OCI_ElemSetNull()
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_ElemSetLob
(
    OCI_Elem *elem,
    OCI_Lob * value
);

/**
 * @brief
 * Assign a File handle to a collection element
 *
 * @param elem   - Element handle
 * @param value  - File Handle
 *
 * @note
 * passing a null pointer for value calls OCI_ElemSetNull()
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_ElemSetFile
(
    OCI_Elem *elem,
    OCI_File *value
);

/**
 * @brief
 * Assign a Ref handle to a collection element
 *
 * @param elem   - Element handle
 * @param value  - Ref Handle
 *
 * @note
 * passing a null pointer for value calls OCI_ElemSetNull()
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_ElemSetRef
(
    OCI_Elem *elem,
    OCI_Ref * value
);

/**
 * @brief
 * Check if the collection element value is null
 *
 * @param elem - Element handle
 *
 * @return
 * TRUE if it's null otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_ElemIsNull
(
    OCI_Elem *elem
);

/**
 * @brief
 * Set a collection element value to null
 *
 * @param elem - Element handle
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_ElemSetNull
(
    OCI_Elem *elem
);

/**
 * @} OcilibCApiCollections
 */

/**
 * @defgroup OcilibCApiFeatureReturningInto Oracle Returning feature
 * @{
 *
 * OCILIB supports the Oracle feature 'Returning into' for DML statements.
 *
 * Let's Oracle talk about this features:
 *
 * @par
 * 'Using the RETURNING clause with a DML statement allows you to essentially
 * combine two SQL statements into one, possibly saving you a server round-trip.
 * This is accomplished by adding an extra clause to the traditional UPDATE,
 * INSERT, and DELETE statements. The extra clause effectively adds a query to
 * the DML statement. In the OCI, the values are returned to the application
 * through the use of OUT bind variables.'
 *
 * OCILIB implements this features by providing a set of functions that allows
 * to register output placeholders for the returned values.
 * Once the DML is executed with OCI_Execute(), the output returned data is
 * available through a regular resultset object that can be fetched.
 *
 * @note
 * Array binding interface is also supported with 'returning into' DML statement.
 * Every iteration (or row of given arrays) generates an resultset object.
 * Once a resultset is fetched, the next on can be retrieved with OCI_GetNextResultset()
 *
 * @par
 *
 * @note
 * OCI_Long are not supported for 'returning into' clause .This is a limitation imposed by Oracle.
 *
 * @note
 * OCI_Column objects retrieved from output OCI_Resultset have the following
 * particularities:
 *
 * - their names are the provided bind names to the DML statement
 *   (by example, ':out1'). So any call to the functions OCI_GetXXX2()
 *   should be aware of it
 * - The columns detailed SQL attributes might be not all set or accurate. By
 *   example, the scale and precision are not set, the SQL type is the one
 *   chosen by OCILIB regarding the OCILIB object data type and might be
 *   slightly different from the real one.
 *
 * @par Example
 * @include returning.c
 *
 */

/**
 * @brief
 * Retrieve the next available resultset
 *
 * @param stmt - Statement handle
 *
 * @note
 * it is only valid for the following statements:
 * - Statements executing SQL UPDATE/DELETE using a RETURNING INTO clause
 * - Statements implicitly returned from PL/SQL procedure or blocks (new feature in Oracle 12cR1) using
 *   DBMS_SQL.RETURN_RESULT()
 *
 * @note
 * SQL statements with a 'returning' clause can return multiple resultsets.
 * When arrays of program variables are binded to the statement, Oracle will
 * execute the statement for every row (iteration).
 * Each iteration generates a resultset that can be fetched like regular ones.
 *
 * @note
 * Starting withOracle 12cR1, PL/SQ procedure and blocks ca return multiple implicit resultsets
 * Refer to  Oracle documentation for more information.
 *
 * @return
 * A resultset handle on success otherwise NULL
 *
 */

OCI_SYM_PUBLIC OCI_Resultset * OCI_API OCI_GetNextResultset
(
    OCI_Statement *stmt
);

/**
* @brief
* Register a register output bind placeholder
*
* @param stmt - Statement handle
* @param name - Output bind name
*
* @return
* TRUE on success otherwise FALSE
*
*/

OCI_SYM_PUBLIC boolean OCI_API OCI_RegisterNumber
(
    OCI_Statement *stmt,
    const otext *  name
);

/**
 * @brief
 * Register a short output bind placeholder
 *
 * @param stmt - Statement handle
 * @param name - Output bind name
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_RegisterShort
(
    OCI_Statement *stmt,
    const otext *  name
);

/**
 * @brief
 * Register an unsigned short output bind placeholder
 *
 * @param stmt - Statement handle
 * @param name - Output bind name
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_RegisterUnsignedShort
(
    OCI_Statement *stmt,
    const otext *  name
);

/**
 * @brief
 * Register an integer output bind placeholder
 *
 * @param stmt - Statement handle
 * @param name - Output bind name
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_RegisterInt
(
    OCI_Statement *stmt,
    const otext *  name
);

/**
 * @brief
 * Register an unsigned integer output bind placeholder
 *
 * @param stmt - Statement handle
 * @param name - Output bind name
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_RegisterUnsignedInt
(
    OCI_Statement *stmt,
    const otext *  name
);

/**
 * @brief
 * Register a big integer output bind placeholder
 *
 * @param stmt - Statement handle
 * @param name - Output bind name
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_RegisterBigInt
(
    OCI_Statement *stmt,
    const otext *  name
);

/**
 * @brief
 * Register an unsigned big integer output bind placeholder
 *
 * @param stmt - Statement handle
 * @param name - Output bind name
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_RegisterUnsignedBigInt
(
    OCI_Statement *stmt,
    const otext *  name
);

/**
 * @brief
 * Register a string output bind placeholder
 *
 * @param stmt - Statement handle
 * @param name - Output bind name
 * @param len  - Max length of single string (in characters)
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_RegisterString
(
    OCI_Statement *stmt,
    const otext *  name,
    unsigned int   len
);

/**
 * @brief
 * Register an raw output bind placeholder
 *
 * @param stmt - Statement handle
 * @param name - Output bind name
 * @param len  - Max length of the buffer (in bytes)
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_RegisterRaw
(
    OCI_Statement *stmt,
    const otext *  name,
    unsigned int   len
);

/**
 * @brief
 * Register a double output bind placeholder
 *
 * @param stmt - Statement handle
 * @param name - Output bind name
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_RegisterDouble
(
    OCI_Statement *stmt,
    const otext *  name
);

/**
 * @brief
 * Register a float output bind placeholder
 *
 * @param stmt - Statement handle
 * @param name - Output bind name
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_RegisterFloat
(
    OCI_Statement *stmt,
    const otext *  name
);

/**
 * @brief
 * Register a date output bind placeholder
 *
 * @param stmt - Statement handle
 * @param name - Output bind name
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_RegisterDate
(
    OCI_Statement *stmt,
    const otext *  name
);

/**
 * @brief
 * Register a timestamp output bind placeholder
 *
 * @param stmt - Statement handle
 * @param name - Output bind name
 * @param type - Timestamp type
 *
 * @note
 * See OCI_TimestampCreate() for possible values of parameter 'type'
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_RegisterTimestamp
(
    OCI_Statement *stmt,
    const otext *  name,
    unsigned int   type
);

/**
 * @brief
 * Register an interval output bind placeholder
 *
 * @param stmt - Statement handle
 * @param name - Output bind name
 * @param type - Interval type
 *
 * @note
 * See OCI_IntervalCreate() for possible values of parameter 'type'
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_RegisterInterval
(
    OCI_Statement *stmt,
    const otext *  name,
    unsigned int   type
);

/**
 * @brief
 * Register an object output bind placeholder
 *
 * @param stmt   - Statement handle
 * @param name   - Output bind name
 * @param typinf - Type info handle
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_RegisterObject
(
    OCI_Statement *stmt,
    const otext *  name,
    OCI_TypeInfo * typinf
);

/**
 * @brief
 * Register a lob output bind placeholder
 *
 * @param stmt - Statement handle
 * @param name - Output bind name
 * @param type - Lob type
 *
 * @note
 * See OCI_LobCreate() for possible values of parameter 'type'
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_RegisterLob
(
    OCI_Statement *stmt,
    const otext *  name,
    unsigned int   type
);

/**
 * @brief
 * Register a file output bind placeholder
 *
 * @param stmt - Statement handle
 * @param name - Output bind name
 * @param type - File type
 *
 * @note
 * See OCI_FileCreate() for possible values of parameter 'type'
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_RegisterFile
(
    OCI_Statement *stmt,
    const otext *  name,
    unsigned int   type
);

/**
 * @brief
 * Register a Ref output bind placeholder
 *
 * @param stmt   - Statement handle
 * @param name   - Output bind name
 * @param typinf - Type info handle
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_RegisterRef
(
    OCI_Statement *stmt,
    const otext *  name,
    OCI_TypeInfo * typinf
);

/**
 * @} OcilibCApiFeatureReturningInto
 */

/**
 * @defgroup OcilibCApiRowIds Oracle Rowids
 * @{
 *
 * OCILIB supports the Oracle ROWID type through C scalar string types (otext).
 *
 * - ROWIDs can be retrieved from resultset with OCI_GetString()
 * - ROWIDs can be binded to statements with OCI_BindString()
 *
 * The maximum size of an ROWID buffer is defined by the constant OCI_SIZE_ROWID
 *
 * @par Example
 * @include rowid.c
 *
 * @} OcilibCApiRowIds
 */

/**
 * @defgroup OcilibCApiStatementControl Statements control
 * @{
 *
 * Those functions give extra information about OCILIB statements and can modify their behavior.
 *
 */

/**
 * @brief
 * Return the type of a SQL statement
 *
 * @param stmt - Statement handle
 *
 * @note
 * Possible values are :
 *
 * - OCI_CST_SELECT  : select statement
 * - OCI_CST_UPDATE  : update statement
 * - OCI_CST_DELETE  : delete statement
 * - OCI_CST_INSERT  : insert statement
 * - OCI_CST_CREATE  : create statement
 * - OCI_CST_DROP    : drop statement
 * - OCI_CST_ALTER   : alter statement
 * - OCI_CST_BEGIN   : begin (pl/sql) statement
 * - OCI_CST_DECLARE : declare (pl/sql) statement
 * - OCI_CST_CALL    : kpu call
 * - OCI_CST_MERGE   : merge statement
 *
 * @return
 * The statement type on success or OCI_UNKOWN on error
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_GetStatementType
(
    OCI_Statement *stmt
);

/**
 * @brief
 * Set the fetch mode of a SQL statement
 *
 * @param stmt - Statement handle
 * @param mode - fetch mode value
 *
 * @warning
 * OCI_SetFetchMode() MUST be called before any OCI_ExecuteXXX() call
 *
 * @note
 * Possible values are :
 *  - OCI_SFM_DEFAULT
 *  - OCI_SFM_SCROLLABLE
 *
 * @warning
 * if Oracle Client is 9iR1:
 *  - when setting OCI_SFM_SCROLLABLE, OCI_SetPrefetch() is internally called with value 0 
 *    to disable prefetching (to avoid an oracle bug).
 *  - when re-setting OCI_SFM_DEFAULT after having set OCI_SFM_SCROLLABLE, OCI_SetPrefetch() 
 *    is internally called with value OCI_PREFETCH_SIZE
 * 
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_SetFetchMode
(
    OCI_Statement *stmt,
    unsigned int   mode
);

/**
 * @brief
 * Return the fetch mode of a SQL statement
 *
 * @param stmt - Statement handle
 *
 * @note
 * See OCI_SetFetchMode() for possible values
 * Default value is OCI_SFM_DEFAULT
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_GetFetchMode
(
    OCI_Statement *stmt
);

/**
 * @brief
 * Set the binding mode of a SQL statement
 *
 * @param stmt - Statement handle
 * @param mode - binding mode value
 *
 * @note
 * Possible values are :
 *  - OCI_BIND_BY_POS  : position binding
 *  - OCI_BIND_BY_NAME : name binding
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_SetBindMode
(
    OCI_Statement *stmt,
    unsigned int   mode
);

/**
 * @brief
 * Return the binding mode of a SQL statement
 *
 * @param stmt - Statement handle
 *
 * @note
 * See OCI_SetBindMode() for possible values
 * Default value is OCI_BIND_BY_NAME
 *
 * @note
 * if stmt is NULL, the return value is OCI_UNKNOWN
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_GetBindMode
(
    OCI_Statement *stmt
);

/**
 * @brief
 * Set the current bind allocation mode that will be used for subsequent binding calls
 *
 * @param stmt - Statement handle
 * @param mode - bind allocation mode value
 *
 * @note
 * Possible values are :
 *  - OCI_BAM_EXTERNAL : bind variable are allocated by user code
 *  - OCI_BAM_INTERNAL : bind variable are allocated internally
 *
 * @warning
 * This call has to be made after preparing a statement as OCI_Prepare() reset it by default to OCI_BAM_EXTERNAL.
 * When calling an OCI_BindXXXX() call, this value is used and stored in the OCI_Bind object created during the bind call.
 * Each bind can have is own allocation mode that is returned by OCI_BindGetAllocationMode()
 * OCI_SetBindAllocation() can be called before each binding call if needed, resulting having some bind allocated externally and other ones internally.
 *
 * @note
 * Refer to the section "Binding variables and arrays" of the documentation about allocation mode as OCI_BAM_INTERNAL is not compatible with all bind calls
 * 
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_SetBindAllocation
(
    OCI_Statement *stmt,
    unsigned int   mode
);

/**
 * @brief
 * Return the current bind allocation mode used for subsequent binding calls
 *
 * @param stmt - Statement handle
 *
 * @note
 * See OCI_SetBindAllocation() for possible values
 * Default value is OCI_BAM_EXTERNAL
 *
 * @warning
 * Each OCI_Bind object has its own allocation mode that may differ from the one returned by OCI_GetBindAllocation()
 * The return value of OCI_GetBindAllocation() is the mode that will be used for any subsequent OCI_BindXXXX() calls
 *
 * @note
 * if stmt is NULL, the return value is OCI_UNKNOWN
 *
 */
OCI_SYM_PUBLIC unsigned int OCI_API OCI_GetBindAllocation
(
    OCI_Statement *stmt
);

/**
 * @brief
 * Set the number of rows fetched per internal server fetch call
 *
 * @param stmt - Statement handle
 * @param size - number of rows to fetch
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_SetFetchSize
(
    OCI_Statement *stmt,
    unsigned int   size
);

/**
 * @brief
 * Return the number of rows fetched per internal server fetch call
 *
 * @param stmt - Statement handle
 *
 * @note
 * Default value is set to constant OCI_FETCH_SIZE
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_GetFetchSize
(
    OCI_Statement *stmt
);

/**
 * @brief
 * Set the number of rows pre-fetched by OCI Client
 *
 * @param stmt - Statement handle
 * @param size - number of rows to pre-fetch
 *
 * @note
 * To turn off pre-fetching, set both attributes (size and memory) to 0.
 *
 * @warning
 * Prefetch is not working with scrollable cursors in Oracle 9iR1
 * In that case, argument 'size' is not used and replace by 0.
 * 
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_SetPrefetchSize
(
    OCI_Statement *stmt,
    unsigned int   size
);

/**
 * @brief
 * Return the number of rows pre-fetched by OCI Client
 *
 * @param stmt - Statement handle
 *
 * @note
 * Default value is set to constant OCI_PREFETCH_SIZE
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_GetPrefetchSize
(
    OCI_Statement *stmt
);

/**
 * @brief
 * Set the amount of memory pre-fetched by OCI Client
 *
 * @param stmt - Statement handle
 * @param size - amount of memory to fetch
 *
 * @note
 * Default value is 0 and the pre-fetch size attribute is used instead.
 * When both attributes are set (pre-fetch size and memory) and pre-fetch memory
 * value can hold more rows than specified by pre-fetch size, OCI uses pre-fetch
 * size instead.
 *
 * @note
 * OCILIB set pre-fetch attribute to OCI_PREFETCH_SIZE when a statement is created.
 * To setup a big value for OCI_SetPrefetchMemory(), you must call
 * OCI_SetPrefetchSize() to 0 to make OCI consider this attribute.
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_SetPrefetchMemory
(
    OCI_Statement *stmt,
    unsigned int   size
);

/**
 * @brief
 * Return the amount of memory used to retrieve rows pre-fetched by OCI Client
 *
 * @param stmt - Statement handle
 *
 * @note
 * Default value is 0
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_GetPrefetchMemory
(
    OCI_Statement *stmt
);

/**
 * @brief
 * Set the LONG data type piece buffer size
 *
 * @param stmt - Statement handle
 * @param size - maximum size for long buffer
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_SetLongMaxSize
(
    OCI_Statement *stmt,
    unsigned int   size
);

/**
 * @brief
 * Return the LONG data type piece buffer size
 *
 * @param stmt - Statement handle
 *
 * @note
 * Default value is set to constant OCI_SIZE_LONG
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_GetLongMaxSize
(
    OCI_Statement *stmt
);

/**
 * @brief
 * Set the long data type handling mode of a SQL statement
 *
 * @param stmt - Statement handle
 * @param mode - long mode value
 *
 * @note
 * Possible values are :
 *
 * - OCI_LONG_EXPLICIT : LONGs are explicitly handled by OCI_Long type
 * - OCI_LONG_IMPLICIT : LONGs are implicitly mapped to string type in the
 *   limits of VARCHAR2 size capacity
 *
 *  LONG RAWs cannot be handled with OCI_LONG_IMPLICIT
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_SetLongMode
(
    OCI_Statement *stmt,
    unsigned int   mode
);

/**
 * @brief
 * Return the long data type handling mode of a SQL statement
 *
 * @param stmt - Statement handle
 *
 * @note
 *  See OCI_SetLongMode() for possible values
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_GetLongMode
(
    OCI_Statement *stmt
);

/**
 * @brief
 * Return the connection handle associated with a statement handle
 *
 * @param stmt - Statement handle
 *
 */

OCI_SYM_PUBLIC OCI_Connection * OCI_API OCI_StatementGetConnection
(
    OCI_Statement *stmt
);

/**
 * @} OcilibCApiStatementControl
 */

/**
 * @defgroup OcilibCApiLobs Internal Large Objects (LOBs)
 * @{
 *
 * Large Objects (LOBs) were introduced with Oracle 8i to replace LONGs
 *
 * Oracle OCI supplies a set APIs to manipulate this data type.
 *
 * OCILIB encapsulates this API by supplying:
 *
 * - An OCI_Lob C type
 * - A set of really easy APIs to manipulate OCI_Lob objects
 *
 * OCILIB currently supports 3 types of Lobs :
 *
 * - BLOB  : Binary LOBs (replacement for LONG RAW data type)
 * - CLOB  : Character LOBs (replacement for LONG data type)
 * - NCLOB : National Character LOBs
 *
 * OCI_Lob objects can be :
 *
 * - Created as standalone instances
 * - Used for in/out binding
 * - Retrieved from select statements
 * - Manipulated (copy, append, ...)
 *
 * @par Lobs > 4 Go
 *
 * Oracle 10g extended lobs by increasing maximum size from 4Go to 128 To.
 *
 * OCILIB, with version 2.1.0, supports now this new limit.
 * For handling sizes and offsets up to 128 To, 64 bit integers are requested.
 *
 * So, A new scalar integer type has been introduced: big_uint (elderly lobsize_t).
 * This type can be a 32 bits or 64 bits integer depending on :
 * - Compiler support for 64 bits integers (C99 compiler, MS compilers)
 * - Oracle client version
 *
 * big_uint will be a 64 bits integer :
 * - if the compiler supports it
 * - if OCILIB is build with option OCI_IMPORT_LINKAGE and the Oracle version is >= 10.1
 * - or OCILIB is build with option OCI_IMPORT_RUNTIME (oracle version is not known at
 *   compilation stage)
 *
 * @par Example
 * @include lob.c
 *
 */

/**
 * @brief
 * Create a local temporary Lob instance
 *
 * @param con  - Connection handle
 * @param type - Lob type
 *
 * Supported lob types :
 *
 * - OCI_BLOB  : Binary Lob
 * - OCI_CLOB  : Character Lob
 * - OCI_NCLOB ! National Character Lob
 *
 * @return
 * Return the lob handle on success otherwise NULL on failure
 *
 */

OCI_SYM_PUBLIC OCI_Lob * OCI_API OCI_LobCreate
(
    OCI_Connection *con,
    unsigned int    type
);

/**
 * @brief
 * Free a local temporary lob
 *
 * @param lob - Lob handle
 *
 * @warning
 * Only lobs created with OCI_LobCreate() should be freed by OCI_LobFree()
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_LobFree
(
    OCI_Lob *lob
);

/**
 * @brief
 * Create an array of lob object
 *
 * @param con    - Connection handle
 * @param type   - Lob type
 * @param nbelem - number of elements in the array
 *
 * @note
 * see OCI_LobCreate() for more details
 *
 * @return
 * Return the lob handle array on success otherwise NULL on failure
 *
 */

OCI_SYM_PUBLIC OCI_Lob ** OCI_API OCI_LobArrayCreate
(
    OCI_Connection *con,
    unsigned int    type,
    unsigned int    nbelem
);

/**
* @brief
* Free an array of lob objects
*
* @param lobs - Array of lob objects
*
* @warning
* Only arrays of lobs created with OCI_LobArrayCreate() should be freed
* by OCI_LobArrayFree()
*
* @return
* TRUE on success otherwise FALSE
*
*/

OCI_SYM_PUBLIC boolean OCI_API OCI_LobArrayFree
(
    OCI_Lob **lobs
);

/**
 * @brief
 * Return the type of the given Lob object
 *
 * @param lob - Lob handle
 *
 * @note
 * For possible values, see OCI_LobCreate()
 *
 * @return
 * Object type or OCI_UNKNOWN the input handle is NULL
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_LobGetType
(
    OCI_Lob *lob
);

/**
 * @brief
 * Perform a seek operation on the OCI_lob content buffer
 *
 * @param lob    - Lob handle
 * @param offset - Offset from current position (bytes or characters)
 * @param mode   - Seek mode
 *
 * Parameter 'mode' can be one of the following value :
 *
 * - OCI_SEEK_SET : set the lob current offset to the given absolute offset
 * - OCI_SEEK_END : set the lob current offset to the end of the lob
 * - OCI_SEEK_CUR : move the lob current offset to the number of bytes or
 *                  characters given by parameter 'offset'
 *
 * @note
 * - For CLOB and CLOB, offset in characters
 * - For BLOB and BFILE, offset is in bytes
 *
 * @note
 * Position in the Lob buffer starts at 0.
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_LobSeek
(
    OCI_Lob *    lob,
    big_uint     offset,
    unsigned int mode
);

/**
 * @brief
 * Return the current position in the Lob content buffer
 *
 * @param lob - Lob handle
 *
 * @return
 * Lob position (starting with 0) or 0 on failure
 */

OCI_SYM_PUBLIC big_uint OCI_API OCI_LobGetOffset
(
    OCI_Lob *lob
);

/**
 * @brief
 * [OBSOLETE] Read a portion of a lob into the given buffer
 *
 * @param lob    - Lob handle
 * @param buffer - Pointer to a buffer
 * @param len    - Length of the buffer (in bytes or characters)
 *
 * @note
 * Length is expressed in :
 * - Bytes for BLOBs
 * - Characters for CLOBs/NCLOBS
 *
 * @warning
 * This call is obsolete ! Use OCI_LobRead2() instead.
 *
 * @return
 * Number of bytes/characters read on success otherwise 0 on failure
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_LobRead
(
    OCI_Lob *    lob,
    void *       buffer,
    unsigned int len
);

/**
 * @brief
 * Read a portion of a lob into the given buffer
 *
 * @param lob        - Lob handle
 * @param buffer     - Pointer to a buffer
 * @param char_count - [in/out] Pointer to maximum number of characters
 * @param byte_count - [in/out] Pointer to maximum number of bytes
 *
 * @note
 * In input,  'char_count' and 'byte_count' are values to read into the buffer
 * In output, 'char_count' and 'byte_count' are values read into the buffer
 *
 * @note
 * For BLOBs, only the parameter 'byte_count' is used
 * For CLOBs, both parameters can be used :
 * In input :
 *  - if 'byte_count' is set to zero, it is computed from 'char_count'
 *  - if 'char_count' is set to zero, it is computed from 'byte_count'
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_LobRead2
(
    OCI_Lob *     lob,
    void *        buffer,
    unsigned int *char_count,
    unsigned int *byte_count
);

/**
 * @brief
 * [OBSOLETE] Write a buffer into a LOB
 *
 * @param lob    - Lob handle
 * @param buffer - Pointer to a buffer
 * @param len    - Length of the buffer (in bytes or characters)
 *
 * @note
 * Length is expressed in :
 * - Bytes for BLOBs
 * - Characters for CLOBs/NCLOBs
 *
 * @warning
 * This call is obsolete ! Use OCI_LobWrite2() instead.
 *
 * @return
 * Number of bytes / characters written on success otherwise 0 on failure
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_LobWrite
(
    OCI_Lob *    lob,
    void *       buffer,
    unsigned int len
);

/**
 * @brief
 * Write a buffer into a LOB
 *
 * @param lob        - Lob handle
 * @param buffer     - Pointer to a buffer
 * @param char_count - [in/out] Pointer to maximum number of characters
 * @param byte_count - [in/out] Pointer to maximum number of bytes
 *
 * @note
 * In input,  'char_count' and 'byte_count' are values to write from the buffer
 * In output, 'char_count' and 'byte_count' are values written from the buffer
 *
 * @note
 * For BLOBs, only the parameter 'byte_count' is used
 * For CLOBs, both parameters can be used :
 * In input :
 *  - if 'byte_count' is set to zero, it is computed from 'char_count'
 *  - if 'char_count' is set to zero, it is computed from 'byte_count'
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_LobWrite2
(
    OCI_Lob *     lob,
    void *        buffer,
    unsigned int *char_count,
    unsigned int *byte_count
);

/**
 * @brief
 * Truncate the given lob to a shorter length
 *
 * @param lob  - Lob handle
 * @param size - New length (in bytes or characters)
 *
 * @note
 * Length is expressed in :
 * - Bytes for BLOBs
 * - Characters for CLOBs/NCLOBs
 *
 * @note
 * If current offset was beyond the new size, it is then updated to an eof position in order for further write calls to append data
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_LobTruncate
(
    OCI_Lob *lob,
    big_uint size
);

/**
 * @brief
 * Return the actual length of a lob
 *
 * @param lob - Lob handle
 *
 * @note
 * The returned value is in bytes for BLOBS and characters for CLOBS/NCLOBs
 *
 */

OCI_SYM_PUBLIC big_uint OCI_API OCI_LobGetLength
(
    OCI_Lob *lob
);

/**
 * @brief
 * Returns the chunk size of a LOB
 *
 * @param lob - Lob handle
 *
 * @note
 * This chunk size corresponds to the chunk size used by the LOB data layer
 * when accessing and modifying the LOB value. According to Oracle
 * documentation, performance will be improved if the application issues
 * read or write requests using a multiple of this chunk size
 *
 * @note
 * The returned value is in bytes for BLOBS and characters for CLOBS/NCLOBs
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_LobGetChunkSize
(
    OCI_Lob *lob
);

/**
 * @brief
 * Erase a portion of the lob at a given position
 *
 * @param lob    - Lob handle
 * @param offset - Absolute position in source lob
 * @param len    - Number of bytes or characters to erase
 *
 * @note
 * Absolute position starts at 0.
 * Erasing means "overwriting" the range of values at the given offset with:
 *   - spaces for CLOB/NCLOB
 *   - 'zero' bytes for BLOB  
 *
 * @return
 * Number of bytes (BLOB) or characters (CLOB/NCLOB) erased on success
 * otherwise 0 on failure
 *
 */

OCI_SYM_PUBLIC big_uint OCI_API OCI_LobErase
(
    OCI_Lob *lob,
    big_uint offset,
    big_uint len
);

/**
 * @brief
 * Append a buffer at the end of a LOB
 *
 * @param lob    - Lob handle
 * @param buffer - Pointer to a buffer
 * @param len    - Length of the buffer (in bytes or characters)
 *
 * @note
 * Length is expressed in :
 * - Bytes for BLOBs
 * - Characters for CLOBs
 *
 * @return
 * Number of bytes / characters written on success otherwise 0 on failure
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_LobAppend
(
    OCI_Lob *    lob,
    void *       buffer,
    unsigned int len
);

/**
 * @brief
 * Append a buffer at the end of a LOB
 *
 * @param lob        - Lob handle
 * @param buffer     - Pointer to a buffer
 * @param char_count - [in/out] Pointer to maximum number of characters
 * @param byte_count - [in/out] Pointer to maximum number of bytes
 *
 * @note
 * In input,  'char_count' and 'byte_count' are values to write from the buffer
 * In output, 'char_count' and 'byte_count' are values written from the buffer
 *
 * @note
 * For BLOBs, only the parameter 'byte_count' is used
 * For CLOBs, both parameters can be used :
 * In input :
 *  - if 'byte_count' is set to zero, it is computed from 'char_count'
 *  - if 'char_count' is set to zero, it is computed from 'byte_count'
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_LobAppend2
(
    OCI_Lob *     lob,
    void *        buffer,
    unsigned int *char_count,
    unsigned int *byte_count
);

/**
 * @brief
 * Append a source LOB at the end of a destination LOB
 *
 * @param lob        - Destination Lob handle
 * @param lob_src    - Source Lob handle
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_LobAppendLob
(
    OCI_Lob *lob,
    OCI_Lob *lob_src
);

/**
 * @brief
 * Check if the given lob is a temporary lob
 *
 * @param lob - Lob handle
 *
 * @return
 * TRUE if it's a temporary lob otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_LobIsTemporary
(
    OCI_Lob *lob
);

/**
 * @brief
 * Copy a portion of a source LOB into a destination LOB
 *
 * @param lob        - Destination Lob handle
 * @param lob_src    - Source Lob handle
 * @param offset_dst - Absolute position in destination lob
 * @param offset_src - Absolute position in source lob
 * @param count      - Number of bytes or character to copy
 *
 * @note
 * For character LOB (CLOB/NCLOBS) the parameters count, offset_dst and
 * offset_src are expressed in characters and not in bytes.
 *
 * @note
 * Absolute position starts at 0.
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_LobCopy
(
    OCI_Lob *lob,
    OCI_Lob *lob_src,
    big_uint offset_dst,
    big_uint offset_src,
    big_uint count
);

/**
 * @brief
 * Copy a portion of a source FILE into a destination LOB
 *
 * @param lob        - Destination Lob handle
 * @param file       - Source File handle
 * @param offset_dst - Absolute position in destination lob
 * @param offset_src - Absolute position in source file
 * @param count      - Number of bytes to copy
 *
 * @note
 * - For character LOB (CLOB/NCLOB) the parameter offset_src are expressed in
 *   characters and not in bytes.
 * - Offset_src is always in bytes
 *
 * @note
 * Absolute position starts at 0.
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_LobCopyFromFile
(
    OCI_Lob * lob,
    OCI_File *file,
    big_uint  offset_dst,
    big_uint  offset_src,
    big_uint  count
);

/**
 * @brief
 * Open explicitly a Lob
 *
 * @param lob  - Lob handle
 * @param mode - open mode
 *
 * Possible values for mode are :
 *
 * - OCI_LOB_READONLY  : read only access
 * - OCI_LOB_READWRITE : read/write access
 *
 * @note
 * - A call to OCI_LobOpen is not necessary to manipulate a Lob.
 * - If a lob has not been opened explicitly, triggers are fired and
 *   indexes updated at every read/write/append operation
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_LobOpen
(
    OCI_Lob *    lob,
    unsigned int mode
);

/**
 * @brief
 * Close explicitly a Lob
 *
 * @param lob - Lob handle
 *
 * @note
 * - A call to OCI_LobClose is not necessary to manipulate a Lob.
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_LobClose
(
    OCI_Lob *lob
);

/**
 * @brief
 * Compare two lob handles for equality
 *
 * @param lob  - Lob handle
 * @param lob2 - Lob2 handle
 *
 * @return
 * TRUE is the lobs are not null and equal otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_LobIsEqual
(
    OCI_Lob *lob,
    OCI_Lob *lob2
);

/**
 * @brief
 * Assign a lob to another one
 *
 * @param lob     - Destination Lob handle
 * @param lob_src - Source Lob handle
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_LobAssign
(
    OCI_Lob *lob,
    OCI_Lob *lob_src
);

/**
 * @brief
 * Return the maximum size that the lob can contain
 *
 * @param lob - Lob handle
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC big_uint OCI_API OCI_LobGetMaxSize
(
    OCI_Lob *lob
);

/**
 * @brief
 * Flush Lob content to the server
 *
 * @param lob   - Lob handle
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_LobFlush
(
    OCI_Lob *lob
);

/**
 * @brief
 * Enable / disable buffering mode on the given lob handle
 *
 * @param lob    - Lob handle
 * @param value  - Enable/disable buffering mode
 *
 * @note
 * Oracle "LOB Buffering Subsystem" allows client applications
 * to speedup read/write of small buffers on Lobs Objects.
 * Check Oracle Documentation for more details on "LOB Buffering Subsystem".
 * This reduces the number of network round trips and LOB versions, thereby
 * improving LOB performance significantly.
 *
 * @warning
 * According to Oracle documentation the following operations are not permitted
 * on Lobs when buffering is on : OCI_LobCopy(), OCI_LobAppend, OCI_LobErase(),
 * OCI_LobGetLength(), OCI_LobTruncate()
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_LobEnableBuffering
(
    OCI_Lob *lob,
    boolean  value
);

/**
* @brief
* Retrieve connection handle from the lob handle
*
* @param lob - lob handle
*
*/

OCI_SYM_PUBLIC OCI_Connection * OCI_API OCI_LobGetConnection
(
    OCI_Lob *lob
);

/**
* @brief
* Indicates if the given lob belongs to a local or remote database table
*
* @param lob - lob handle
*
* @warning
* Requires Oracle 12cR2 (both client and server side), otherwise it returns FALSE
*
*/

OCI_SYM_PUBLIC boolean OCI_API OCI_LobIsRemote
(
    OCI_Lob *lob
);

/**
 * @} OcilibCApiLobs
 */

/**
 * @defgroup OcilibCApiFiles External Large Objects (FILEs)
 * @{
 *
 * External Large Objects (FILEs) were introduced with Oracle 8i
 *
 * Oracle OCI supplies a set APIs to manipulate this data type.
 *
 * OCILIB encapsulates this API by supplying:
 *
 * - An OCI_File C type
 * - A set of really easy APIs to manipulate OCI_File objects
 *
 * OCILIB currently supports 2 types of Lobs :
 *
 * - BFILE : Binary files
 * - CFILE : Character files
 *
 * @warning
 * FILEs are read-only.
 *
 * OCI_Lob objects can be :
 *
 * - Created as standalone instances
 * - Used for in/out binding
 * - Retrieved from select statements
 * - Used for reading server files content
 *
 * @par Files > 4 Go
 *
 * - New maximum file size limit (128 To) applies to OCI_Files objects.
 * - See Internal Large Objects (LOBs) section for Files > 4 Go information
 *
 * @par Example
 * @include file.c
 *
 */

/**
 * @brief
 * Create a file object instance
 *
 * @param con  - Connection handle
 * @param type - File type
 *
 * Supported file types :
 *
 * - OCI_BFILE : Binary file
 * - OCI_CFILE : Character file
 *
 * @return
 * Return the lob handle on success otherwise NULL on failure
 *
 */

OCI_SYM_PUBLIC OCI_File * OCI_API OCI_FileCreate
(
    OCI_Connection *con,
    unsigned int    type
);

/**
 * @brief
 * Free a local File object
 *
 * @param file - File handle
 *
 * @warning
 * Only Files created with OCI_FileCreate() should be freed by OCI_FileFree()
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_FileFree
(
    OCI_File *file
);

/**
 * @brief
 * Create an array of file object
 *
 * @param con    - Connection handle
 * @param type   - File type
 * @param nbelem - number of elements in the array
 *
 * @note
 * see OCI_FileCreate() for more details
 *
 * @return
 * Return the file handle array on success otherwise NULL on failure
 *
 */

OCI_SYM_PUBLIC OCI_File ** OCI_API OCI_FileArrayCreate
(
    OCI_Connection *con,
    unsigned int    type,
    unsigned int    nbelem
);

/**
* @brief
* Free an array of file objects
*
* @param files - Array of file objects
*
* @warning
* Only arrays of lobs created with OCI_FileArrayCreate() should be freed by OCI_FileArrayFree()
*
* @return
* TRUE on success otherwise FALSE
*
*/

OCI_SYM_PUBLIC boolean OCI_API OCI_FileArrayFree
(
    OCI_File **files
);

/**
 * @brief
 * Return the type of the given File object
 *
 * @param file - File handle
 *
 * @note
 * For possible values, see OCI_FileCreate()
 *
 * @return
 * Object type or OCI_UNKNOWN the input handle is NULL
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_FileGetType
(
    OCI_File *file
);

/**
 * @brief
 * Perform a seek operation on the OCI_File content buffer
 *
 * @param file    - File handle
 * @param offset  - Offset from current position
 * @param mode    - Seek mode
 *
 * Mode parameter can be one of the following value :
 *
 * - OCI_SEEK_SET : set the file current offset to the given absolute offset
 * - OCI_SEEK_END : set the file current offset to the end of the lob
 * - OCI_SEEK_CUR : move the file current offset to the number of bytes given by
 *                  parameter 'offset'
 *
 * @note
 * Position in the File buffer starts at 0.
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_FileSeek
(
    OCI_File *   file,
    big_uint     offset,
    unsigned int mode
);

/**
 * @brief
 * Return the current position in the file
 *
 * @param file - File handle
 *
 * @return
 * File position (starting with 0) or 0 on failure
 */

OCI_SYM_PUBLIC big_uint OCI_API OCI_FileGetOffset
(
    OCI_File *file
);

/**
 * @brief
 * Read a portion of a file into the given buffer
 *
 * @param file   - File handle
 * @param buffer - Pointer to a buffer
 * @param len    - Length of the buffer in bytes
 *
 * @return
 * Number of bytes read on success otherwise 0 on failure
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_FileRead
(
    OCI_File *   file,
    void *       buffer,
    unsigned int len
);

/**
 * @brief
 * Return the size in bytes of a file
 *
 * @param file - File handle
 *
 */

OCI_SYM_PUBLIC big_uint OCI_API OCI_FileGetSize
(
    OCI_File *file
);

/**
 * @brief
 * Check if the given file exists on server
 *
 * @param file - File handle
 *
 * @note
 * For local FILEs object, OCI_LobFileSetName() must be called before to set the filename to check
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_FileExists
(
    OCI_File *file
);

/**
 * @brief
 * Set the directory and file name of FILE handle
 *
 * @param file  - File handle
 * @param dir   - File directory
 * @param name  - File name
 *in
 * @note
 * - For local FILEs only
 * - Files fetched from resultset cannot be assigned a new directory and name
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_FileSetName
(
    OCI_File *   file,
    const otext *dir,
    const otext *name
);

/**
 * @brief
 * Return the directory of the given file
 *
 * @param file  - File handle
 *
 */

OCI_SYM_PUBLIC const otext * OCI_API OCI_FileGetDirectory
(
    OCI_File *file
);

/**
 * @brief
 * Return the name of the given file
 *
 * @param file  - File handle
 *
 */

OCI_SYM_PUBLIC const otext * OCI_API OCI_FileGetName
(
    OCI_File *file
);

/**
 * @brief
 * Open a file for reading
 *
 * @param file  - File handle
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_FileOpen
(
    OCI_File *file
);

/**
 * @brief
 * Check if the specified file is opened within the file handle
 *
 * @param file  - File handle
 *
 * @return
 * TRUE if the file was opened with this handle otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_FileIsOpen
(
    OCI_File *file
);

/**
 * @brief
 * Close a file
 *
 * @param file  - File handle
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_FileClose
(
    OCI_File *file
);

/**
 * @brief
 * Compare two file handle for equality
 *
 * @param file  - File handle
 * @param file2 - File2 handle
 *
 * @return
 * TRUE is the lobs are not null and equal otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_FileIsEqual
(
    OCI_File *file,
    OCI_File *file2
);

/**
 * @brief
 * Assign a file to another one
 *
 * @param file     - Destination File handle
 * @param file_src - Source File handle
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_FileAssign
(
    OCI_File *file,
    OCI_File *file_src
);

/**
* @brief
* Retrieve connection handle from the file handle
*
* @param file - file handle
*
*/

OCI_SYM_PUBLIC OCI_Connection * OCI_API OCI_FileGetConnection
(
    OCI_File *file
);

/**
 * @} OcilibCApiFiles
 */

/**
 * @defgroup OcilibCApiLongs Long objects
 * @{
 *
 * Long Objects encapsulate Oracle LONGs data types and were used to store large
 * buffers in Oracle database.
 *
 * They're still supported but are depreciated. Oracle now provides a
 * newer and better way to deal with data that needs large storage : LOBs
 *
 * OCILIB supports this data type because it was and still is widely used
 *
 * OCILIB provides a set of API for manipulating LONGs that is really close to
 * the one provided for LOBs.
 *
 * OCILIB currently supports 3 types of Long Objects:
 *
 * - OCI_BLONG : LONG RAW columns
 * - OCI_CLONG : LONG columns
 *
 * OCI_Lob objects can be :
 *
 * - Created as standalone instances
 * - Used for in/out binding
 * - Retrieved from select statement
 *
 * @par Example
 * @include long.c
 *
 */

/**
 * @brief
 * Create a local temporary Long instance
 *
 * @param stmt - Statement handle
 * @param type - Long type
 *
 * Supported lob types :
 *
 * - OCI_BLONG : Binary Long
 * - OCI_CLONG : Character Long
 *
 * @return
 * Return the long handle on success otherwise NULL on failure
 *
 */

OCI_SYM_PUBLIC OCI_Long * OCI_API OCI_LongCreate
(
    OCI_Statement *stmt,
    unsigned int   type
);

/**
 * @brief
 * Free a local temporary long
 *
 * @param lg - Long handle
 *
 * @warning
 * Only lobs created with OCI_LongCreate() should be freed by OCI_LongFree()
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_LongFree
(
    OCI_Long *lg
);

/**
 * @brief
 * Return the type of the given Long object
 *
 * @param lg - Long handle
 *
 * @note
 * For possible values, see OCI_LobCreate()
 *
 * @return
 * Object type or OCI_UNKNOWN the input handle is NULL
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_LongGetType
(
    OCI_Long *lg
);

/**
 * @brief
 * Read a portion of a long into the given buffer [Obsolete]
 *
 * @param lg     - Long handle
 * @param buffer - Pointer to a buffer
 * @param len    - Length of the buffer in bytes / characters
 *
 * @note
 * - From version 2.0.0, this function is obsolete because OCILIB fetches now
 *   all data during OCIFetchNext() call
 * - So, this call reads now the internal OCI_Long object allocated buffer
 * - The internal buffer can be directly accessed with OCI_LongGetBuffer()
 *
 * @note
 * - For OCI_CLONG, parameter 'len' and returned value are expressed in characters
 * - For OCI_BLONG, parameter 'len' and returned value are expressed in bytes
 *
 * @return
 * - Number of bytes/characters read on success
 * - 0 if there is nothing more to read
 * - 0 on failure
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_LongRead
(
    OCI_Long *   lg,
    void *       buffer,
    unsigned int len
);

/**
 * @brief
 * Write a buffer into a Long
 *
 * @param lg     - Long handle
 * @param buffer - the pointer to a buffer
 * @param len    - the length of the buffer in bytes (OCI_BLONG) or
 *                  character (OCI_CLONG)
 *
 * @return
 * Number of bytes (OCI_BLONG) / character (OCI_CLONG) written on success otherwise 0 on failure
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_LongWrite
(
    OCI_Long *   lg,
    void *       buffer,
    unsigned int len
);

/**
 * @brief
 * Return the buffer size of a long object in bytes (OCI_BLONG) or character (OCI_CLONG)
 *
 * @param lg - Long handle
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_LongGetSize
(
    OCI_Long *lg
);

/**
 * @brief
 * Return the internal buffer of an OCI_Long object read from a fetch sequence
 *
 * @param lg - Long handle
 *
 */

OCI_SYM_PUBLIC void * OCI_API OCI_LongGetBuffer
(
    OCI_Long *lg
);

/**
 * @} OcilibCApiLongs
 */

/**
* @defgroup OcilibCApiOracleNumber Oracle NUMBER manipulation (optional)
* @{
*
* OCILIB encapsulates Oracle SQL all Numeric types using C native data types.
* But it also provides an optional OCI_Number handle for manipulating and accessing Oracle NUMBER type.
* OCI_Number provides management for some special value that cannot be addressed in C such as positive 
* and negative infinity.
*
* @par Example
* @include number.c
*
*/

/**
* @brief
* Create a local number object
*
* @param con - Connection handle
*
* @note
* Parameter 'con' can be NULL in order to manipulate numbers
* independently from database connections
*
* @return
* Return the number handle on success otherwise NULL on failure
*
*/

OCI_SYM_PUBLIC OCI_Number * OCI_API OCI_NumberCreate
(
    OCI_Connection *con
);

/**
* @brief
* Free a number object
*
* @param number - Number handle
*
* @warning
* Only Numbers created with OCI_NumberCreate() should be freed by OCI_NumberFree()
*
* @return
* TRUE on success otherwise FALSE
*
*/

OCI_SYM_PUBLIC boolean OCI_API OCI_NumberFree
(
    OCI_Number *number
);

/**
* @brief
* Create an array of number object
*
* @param con    - Connection handle
* @param nbelem - number of elements in the array
*
* @note
* see OCI_NumberCreate() for more details
*
* @return
* Return the number handle array on success otherwise NULL on failure
*
*/

OCI_SYM_PUBLIC OCI_Number ** OCI_API OCI_NumberArrayCreate
(
    OCI_Connection *con,
    unsigned int    nbelem
);

/**
* @brief
* Free an array of number objects
*
* @param numbers - Array of number objects
*
* @warning
* Only arrays of numbers created with OCI_NumberArrayCreate() should be freed by OCI_NumberArrayFree()
*
* @return
* TRUE on success otherwise FALSE
*
*/

OCI_SYM_PUBLIC boolean OCI_API OCI_NumberArrayFree
(
    OCI_Number **numbers
);

/**
* @brief
* Assign the value of a number handle to another one
*
* @param number     - Destination number handle
* @param number_src - Source number handle
*
* @return
* TRUE on success otherwise FALSE
*
*/

OCI_SYM_PUBLIC int OCI_API OCI_NumberAssign
(
    OCI_Number *number,
    OCI_Number *number_src
);

/**
* @brief
* Convert a number value from the given number handle to a string
*
* @param number - source number handle
* @param fmt    - Number format
* @param size   - Destination string size in characters
* @param str    - Destination date string
*
* @note
* Output string can be one the following 'magic strings':
*   - '~'  for positive infinity
*   - '-~' for negative infinity
*
* @return
* TRUE on success otherwise FALSE
*
*/

OCI_SYM_PUBLIC boolean OCI_API OCI_NumberToText
(
    OCI_Number * number,
    const otext *fmt,
    int          size,
    otext *      str
);

/**
* @brief
* Convert a string to a number and store it in the given number handle
*
* @param number - Destination number handle
* @param str    - Source number string
* @param fmt    - Number format
*
* @note
* Input string can be one the following 'magic strings':
*   - '~'  for positive infinity
*   - '-~' for negative infinity
*
* @return
* TRUE on success otherwise FALSE
*
*/

OCI_SYM_PUBLIC boolean OCI_API OCI_NumberFromText
(
    OCI_Number * number,
    const otext *str,
    const otext *fmt
);

/**
* @brief
* Return the number value content
*
* @param number  - number handle
*
* @note
* Returned content is a buffer of 22 bytes corresponding to Oracle C native 
* representation of NUMBER values
* See oracle Documentation of its layout
*
* @return
* TRUE on success otherwise FALSE
*
*/

OCI_SYM_PUBLIC unsigned char * OCI_API OCI_NumberGetContent
(
    OCI_Number *number
);

/**
* @brief
* Assign the number value content
*
* @param number  - number handle
* @param content - raw number content
*
* @note
* See OCI_NumberSetContent() for more information
*
* @return
* TRUE on success otherwise FALSE
*
*/

OCI_SYM_PUBLIC boolean OCI_API OCI_NumberSetContent
(
    OCI_Number *   number,
    unsigned char *content
);

/**
* @brief
* Assign the number value with the value of a native C numeric type
*
* @param number - number handle
* @param type   - native C type to assign
* @param value  - pointer to value to set
*
* @note
* Argument \p param type can be :
*
* - OCI_NUM_SHORT     : value is a pointer to a signed short
* - OCI_NUM_USHORT    : value is a pointer to an unsigned short
* - OCI_NUM_INT       : value is a pointer to a signed int
* - OCI_NUM_UINT      : value is a pointer to an unsigned short
* - OCI_NUM_BIGINT    : value is a pointer to a signed big_int
* - OCI_NUM_BIGUINT   : value is a pointer to an unsigned big_uint
* - OCI_NUM_FLOAT     : value is a pointer to an float
* - OCI_NUM_DOUBLE    : value is a pointer to a double
* - OCI_NUM_NUMBER    : value is a pointer to the return value of OCI_NumberGetContent()
*
* @return
* TRUE on success otherwise FALSE
*
*/

OCI_SYM_PUBLIC boolean OCI_API OCI_NumberSetValue
(
    OCI_Number * number,
    unsigned int type,
    void *       value
);

/**
* @brief
* Assign the number value to a native C numeric type
*
* @param number - number handle
* @param type   - native C type to assign
* @param value  - pointer to a native C variable 
*
* @note
* See OCI_NumberSetValue() for more information
*
* @return
* TRUE on success otherwise FALSE
*
*/

OCI_SYM_PUBLIC boolean OCI_API OCI_NumberGetValue
(
    OCI_Number * number,
    unsigned int type,
    void *       value
);

/**
* @brief
* Add the value of a native C numeric type to the given number
*
* @param number - number handle
* @param type   - native C type of the variable
* @param value  - pointer to a native C variable to add
*
* @note
* See OCI_NumberSetValue() for more information
*
* @return
* TRUE on success otherwise FALSE
*
*/

OCI_SYM_PUBLIC boolean OCI_API OCI_NumberAdd
(
    OCI_Number * number,
    unsigned int type,
    void *       value
);

/**
* @brief
* Subtract the value of a native C numeric type to the given number
*
* @param number - number handle
* @param type   - native C type of the variable
* @param value  - pointer to a native C variable to subtract
*
* @note
* See OCI_NumberSetValue() for more information
*
* @return
* TRUE on success otherwise FALSE
*
*/

OCI_SYM_PUBLIC boolean OCI_API OCI_NumberSub
(
    OCI_Number * number,
    unsigned int type,
    void *       value
);

/**
* @brief
* Multiply the given number with the value of a native C numeric
*
* @param number - number handle
* @param type   - native C type of the variable
* @param value  - pointer to a native C variable to multiply by
*
* @note
* See OCI_NumberSetValue() for more information
*
* @return
* TRUE on success otherwise FALSE
*
*/

OCI_SYM_PUBLIC boolean OCI_API OCI_NumberMultiply
(
    OCI_Number * number,
    unsigned int type,
    void *       value
);

/**
* @brief
* Divide the given number with the value of a native C numeric
*
* @param number - number handle
* @param type   - native C type of the variable
* @param value  - pointer to a native C variable to divide by
*
* @note
* See OCI_NumberSetValue() for more information
*
* @return
* TRUE on success otherwise FALSE
*
*/

OCI_SYM_PUBLIC boolean OCI_API OCI_NumberDivide
(
    OCI_Number * number,
    unsigned int type,
    void *       value
);

/**
 * @brief
 * Compares two number handles
 *
 * @param number1 - number1 handle
 * @param number2 - number2 handle
 *
 * @return
 * - -1 if number1 is smaller than number2,
 * -  0 if they are equal
 * -  1 if number1 is greater than number2.
 *
 */

OCI_SYM_PUBLIC int OCI_API OCI_NumberCompare
(
    OCI_Number *number1,
    OCI_Number *number2
);

/**
 * @} OcilibCApiOracleNumber
 */


/**
 * @defgroup OcilibCApiDatetimes Date/time manipulation
 * @{
 *
 * OCILIB encapsulates Oracle SQL Date data type within OCI_Date structure
 *
 * Basically, the OCI_Date routines are wrappers around the Oracle OCIDate APIs
 *
 * @par Example
 * @include date.c
 *
 */

/**
 * @brief
 * Create a local date object
 *
 * @param con - Connection handle
 *
 * @note
 * From version 2.5.0, parameter 'con' can be NULL in order to manipulate dates
 * independently from database connections
 *
 * @return
 * Return the date handle on success otherwise NULL on failure
 *
 */

OCI_SYM_PUBLIC OCI_Date * OCI_API OCI_DateCreate
(
    OCI_Connection *con
);

/**
 * @brief
 * Free a date object
 *
 * @param date - Date handle
 *
 * @warning
 * Only dates created with OCI_DateCreate() should be freed by OCI_DateFree()
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_DateFree
(
    OCI_Date *date
);

/**
 * @brief
 * Create an array of date object
 *
 * @param con    - Connection handle
 * @param nbelem - number of elements in the array
 *
 * @note
 * see OCI_DateCreate() for more details
 *
 * @return
 * Return the date handle array on success otherwise NULL on failure
 *
 */

OCI_SYM_PUBLIC OCI_Date ** OCI_API OCI_DateArrayCreate
(
    OCI_Connection *con,
    unsigned int    nbelem
);

/**
 * @brief
 * Free an array of date objects
 *
 * @param dates - Array of date objects
 *
 * @warning
 * Only arrays of dates created with OCI_DateArrayCreate() should be freed by OCI_DateArrayFree()
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_DateArrayFree
(
    OCI_Date **dates
);

/**
 * @brief
 * Add or subtract days to a date handle
 *
 * @param date - Date handle
 * @param nb   - Number of days to add/remove
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_DateAddDays
(
    OCI_Date *date,
    int       nb
);

/**
 * @brief
 * Add or subtract months to a date handle
 *
 * @param date - Date handle
 * @param nb   - Number of months to add/remove
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_DateAddMonths
(
    OCI_Date *date,
    int       nb
);

/**
 * @brief
 * Assign the value of a date handle to another one
 *
 * @param date     - Destination Date handle
 * @param date_src - Source Date handle
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC int OCI_API OCI_DateAssign
(
    OCI_Date *date,
    OCI_Date *date_src
);

/**
 * @brief
 * Check if the given date is valid
 *
 * @param date - Date handle
 *
 * @return
 * - Zero if date is valid
 * - Any other value means the date is invalid
 *
 */

OCI_SYM_PUBLIC int OCI_API OCI_DateCheck
(
    OCI_Date *date
);

/**
 * @brief
 * Compares two date handles
 *
 * @param date  - Date1 handle
 * @param date2 - Date2 handle
 *
 * @return
 * - -1 if date1 is smaller than date2,
 * -  0 if they are equal
 * -  1 if date1 is greater than date2.
 *
 */

OCI_SYM_PUBLIC int OCI_API OCI_DateCompare
(
    OCI_Date *date,
    OCI_Date *date2
);

/**
 * @brief
 * Return the number of days betWeen two dates
 *
 * @param date  - Date1 handle
 * @param date2 - Date2 handle
 *
 * The number of days is the difference of (date2 - date)
 * 
 * @return
 * Number of days on success otherwise OCI_ERROR on failure
 *
 */

OCI_SYM_PUBLIC int OCI_API OCI_DateDaysBetween
(
    OCI_Date *date,
    OCI_Date *date2
);

/**
 * @brief
 * Convert a string to a date and store it in the given date handle
 *
 * @param date - Destination Date handle
 * @param str  - Source date string
 * @param fmt  - Date format
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_DateFromText
(
    OCI_Date *   date,
    const otext *str,
    const otext *fmt
);

/**
 * @brief
 * Convert a Date value from the given date handle to a string
 *
 * @param date - source Date handle
 * @param fmt  - Date format
 * @param size - Destination string size in characters
 * @param str  - Destination date string
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_DateToText
(
    OCI_Date *   date,
    const otext *fmt,
    int          size,
    otext *      str
);

/**
 * @brief
 * Extract the date part from a date handle
 *
 * @param date  - Date handle
 * @param year  - Place holder for year value
 * @param month - Place holder for month value
 * @param day   - Place holder for day value
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_DateGetDate
(
    OCI_Date *date,
    int *     year,
    int *     month,
    int *     day
);

/**
 * @brief
 * Extract the time part from a date handle
 *
 * @param date  - Date handle
 * @param hour  - Place holder for hour value
 * @param min   - Place holder for minute value
 * @param sec   - Place holder for second value
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_DateGetTime
(
    OCI_Date *date,
    int *     hour,
    int *     min,
    int *     sec
);

/**
 * @brief
 * Extract the date and time parts from a date handle
 *
 * @param date  - Date handle
 * @param year  - Place holder for year value
 * @param month - Place holder for month value
 * @param day   - Place holder for day value
 * @param hour  - Place holder for hour value
 * @param min   - Place holder for minute value
 * @param sec   - Place holder for second value
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_DateGetDateTime
(
    OCI_Date *date,
    int *     year,
    int *     month,
    int *     day,
    int *     hour,
    int *     min,
    int *     sec
);

/**
 * @brief
 * Set the date portion if the given date handle
 *
 * @param date  - Date handle
 * @param year  - Year value
 * @param month - Month value
 * @param day   - Day value
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_DateSetDate
(
    OCI_Date *date,
    int       year,
    int       month,
    int       day
);

/**
 * @brief
 * Set the time portion if the given date handle
 *
 * @param date  - Date handle
 * @param hour  - Hour value
 * @param min   - Minute value
 * @param sec   - Second value
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_DateSetTime
(
    OCI_Date *date,
    int       hour,
    int       min,
    int       sec
);

/**
 * @brief
 * Set the date and time portions if the given date handle
 *
 * @param date  - Date handle
 * @param year  - Year value
 * @param month - Month value
 * @param day   - Day value
 * @param hour  - Hour value
 * @param min   - Minute value
 * @param sec   - Second value
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_DateSetDateTime
(
    OCI_Date *date,
    int       year,
    int       month,
    int       day,
    int       hour,
    int       min,
    int       sec
);

/**
 * @brief
 * Place the last day of month (from the given date) into the given date
 *
 * @param date  - Date handle
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_DateLastDay
(
    OCI_Date *date
);

/**
 * @brief
 * Gets the date of next day of the week, after a given date
 *
 * @param date  - Date handle
 * @param day   - Day of the week
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_DateNextDay
(
    OCI_Date *   date,
    const otext *day
);

/**
 * @brief
 * Return the current system date/time into the date handle
 *
 * @param date  - Date handle
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_DateSysDate
(
    OCI_Date *date
);

/**
 * @brief
 * Convert a date from one zone to another zone
 *
 * @param date  - Date handle
 * @param zone1 - Source zone
 * @param zone2 - Destination zone
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_DateZoneToZone
(
    OCI_Date *   date,
    const otext *zone1,
    const otext *zone2
);

/**
 * @brief
 * Affect an OCI_Date handle value to ISO C time data types
 *
 * @param date  - Date handle
 * @param ptm   - Pointer to a structure tm to receive date/time values
 * @param pt    - Pointer to a time_t to hold the date/time in the time_t format
 *
 * @note
 * Both parameters 'ptm' and 'p' are optional but one of them has to be provided.
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_DateToCTime
(
    OCI_Date * date,
    struct tm *ptm,
    time_t *   pt
);

/**
 * @brief
 * Affect ISO C time data types values to an OCI_Date handle
 *
 * @param date  - Date handle
 * @param ptm   - Pointer to a structure tm that hold the date/time value
 * @param t     - Value (time_t) that hold the date/time in the time_t format
 *
 * @note
 * Both parameters 'ptm' and 'p' are optional but one of them has to be provided.
 * If 'ptm' is not null, its value is affected to the OCI_Timestamp handle,
 * otherwise the value of 't' is used.
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_DateFromCTime
(
    OCI_Date * date,
    struct tm *ptm,
    time_t     t
);

/**
 * @} OcilibCApiDatetimes
 */

/**
 * @defgroup OcilibCApiTimestamps Timestamps and intervals manipulation
 * @{
 *
 * OCILIB encapsulates Oracle  :
 *
 * - SQL timestamp data type within OCI_Timestamp structure
 * - SQL interval  data type within OCI_Interval  structure
 *
 * Basically, the OCI_Timestamp and OCI_Interval routines are wrappers around
 *  the Oracle OCIDatetime and OCIInterval APIs
 *
 * @par Examples
 * @include timestamp.c
 *
 */

/**
 * @brief
 * Create a local Timestamp instance
 *
 * @param con  - Connection handle
 * @param type - Timestamp type
 *
 * @note
 * From version 2.5.0, parameter 'con' can be NULL in order to manipulate
 * timestamps independently from database connections
 *
 * @note
 * Timestamp type can be :
 *
 * - OCI_TIMESTAMP     : timestamp
 * - OCI_TIMESTAMP_TZ  : timestamp with time zone
 * - OCI_TIMESTAMP_LTZ : timestamp with local time zone
 *
 * @return
 * Return the Timestamp handle on success otherwise NULL on failure
 *
 */

OCI_SYM_PUBLIC OCI_Timestamp * OCI_API OCI_TimestampCreate
(
    OCI_Connection *con,
    unsigned int    type
);

/**
 * @brief
 * Free an OCI_Timestamp handle
 *
 * @param tmsp - Timestamp handle
 *
 * @warning
 * Only Timestamp created with OCI_TimestampCreate() should be freed by OCI_IntervalFree()
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_TimestampFree
(
    OCI_Timestamp *tmsp
);

/**
 * @brief
 * Create an array of timestamp object
 *
 * @param con    - Connection handle
 * @param type   - Timestamp type
 * @param nbelem - number of elements in the array
 *
 * @note
 * see OCI_TimestampCreate() for more details
 *
 * @return
 * Return the timestamp handle array on success otherwise NULL on failure
 *
 */

OCI_SYM_PUBLIC OCI_Timestamp ** OCI_API OCI_TimestampArrayCreate
(
    OCI_Connection *con,
    unsigned int    type,
    unsigned int    nbelem
);

/**
 * @brief
 * Free an array of timestamp objects
 *
 * @param tmsps - Array of timestamp objects
 *
 * @warning
 * Only arrays of timestamp created with OCI_TimestampArrayCreate()
 * should be freed by OCI_TimestampArrayFree()
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_TimestampArrayFree
(
    OCI_Timestamp **tmsps
);

/**
 * @brief
 * Return the type of the given Timestamp object
 *
 * @param tmsp - Timestamp handle
 *
 * @note
 * For possible values, see OCI_TimestampCreate()
 *
 * @return
 * Object type or OCI_UNKNOWN the input handle is NULL
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_TimestampGetType
(
    OCI_Timestamp *tmsp
);

/**
 * @brief
 * Assign the value of a timestamp handle to another one
 *
 * @param tmsp     - Destination Timestamp handle
 * @param tmsp_src - Source Timestamp handle
 *
 * @note
 * Both timestamp handles must be of the same type
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_TimestampAssign
(
    OCI_Timestamp *tmsp,
    OCI_Timestamp *tmsp_src
);

/**
 * @brief
 * Check if the given timestamp is valid
 *
 * @param tmsp - Timestamp handle
 *
 * @return
 * - Zero if the timestamp value is valid
 * - Any other value means the timestamp value is invalid
 *
 */

OCI_SYM_PUBLIC int OCI_API OCI_TimestampCheck
(
    OCI_Timestamp *tmsp
);

/**
 * @brief
 * Compares two timestamp handles
 *
 * @param tmsp  - Timestamp1 handle
 * @param tmsp2 - Timestamp2 handle
 *
 * @return
 * - -1 if Timestamp1 is smaller than Timestamp2,
 * -  0 if they are equal
 * -  1 if Timestamp1 is greater than Timestamp2.
 *
 */

OCI_SYM_PUBLIC int OCI_API OCI_TimestampCompare
(
    OCI_Timestamp *tmsp,
    OCI_Timestamp *tmsp2
);

/**
 * @brief
 * Set a timestamp handle value
 *
 * @param tmsp      - Timestamp handle
 * @param year      - Year value
 * @param month     - Month value
 * @param day       - Day value
 * @param hour      - hour value
 * @param min       - minutes value
 * @param sec       - seconds value
 * @param fsec      - fractional part of seconds value
 * @param time_zone - name of a time zone to use [optional]
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_TimestampConstruct
(
    OCI_Timestamp *tmsp,
    int            year,
    int            month,
    int            day,
    int            hour,
    int            min,
    int            sec,
    int            fsec,
    const otext *  time_zone
);

/**
 * @brief
 * Convert one timestamp value from one type to another.
 *
 * @param tmsp     - Timestamp handle to convert
 * @param tmsp_src - Timestamp handle to use for the type conversion
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_TimestampConvert
(
    OCI_Timestamp *tmsp,
    OCI_Timestamp *tmsp_src
);

/**
 * @brief
 * Convert a string to a timestamp and store it in the given timestamp handle
 *
 * @param tmsp - Destination Timestamp handle
 * @param str  - Source date string
 * @param fmt  - Date format
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_TimestampFromText
(
    OCI_Timestamp *tmsp,
    const otext *  str,
    const otext *  fmt
);

/**
 * @brief
 * Convert a timestamp value from the given timestamp handle to a string
 *
 * @param tmsp      - source Timestamp handle
 * @param fmt       - Timestamp format
 * @param size      - Destination string size in characters
 * @param str       - Destination date string
 * @param precision - Precision for fractional part of the seconds
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_TimestampToText
(
    OCI_Timestamp *tmsp,
    const otext *  fmt,
    int            size,
    otext *        str,
    int            precision
);

/**
 * @brief
 * Extract the date part from a timestamp handle
 *
 * @param tmsp  - Timestamp handle
 * @param year  - Place holder for year value
 * @param month - Place holder for month value
 * @param day   - Place holder for day value
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_TimestampGetDate
(
    OCI_Timestamp *tmsp,
    int *          year,
    int *          month,
    int *          day
);

/**
 * @brief
 * Extract the time portion from a timestamp handle
 *
 * @param tmsp  - Timestamp handle
 * @param hour  - Place holder for hour value
 * @param min   - Place holder for minute value
 * @param sec   - Place holder for second value
 * @param fsec  - Place holder for fractional part of second value
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_TimestampGetTime
(
    OCI_Timestamp *tmsp,
    int *          hour,
    int *          min,
    int *          sec,
    int *          fsec
);

/**
 * @brief
 * Extract the date and time parts from a date handle
 *
 * @param tmsp  - Date handle
 * @param year  - Place holder for year value
 * @param month - Place holder for month value
 * @param day   - Place holder for day value
 * @param hour  - Place holder for hour value
 * @param min   - Place holder for minute value
 * @param sec   - Place holder for second value
 * @param fsec  - Place holder for fractional part of seconds value
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_TimestampGetDateTime
(
    OCI_Timestamp *tmsp,
    int *          year,
    int *          month,
    int *          day,
    int *          hour,
    int *          min,
    int *          sec,
    int *          fsec
);

/**
 * @brief
 * Return the time zone name of a timestamp handle
 *
 * @param tmsp - Timestamp handle
 * @param size - Destination string size in characters
 * @param str  - Destination zone name string
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_TimestampGetTimeZoneName
(
    OCI_Timestamp *tmsp,
    int            size,
    otext *        str
);

/**
 * @brief
 * Return the time zone (hour, minute) portion of a timestamp handle
 *
 * @param tmsp  - Timestamp handle
 * @param hour  - Place holder for hour value
 * @param min   - Place holder for min value
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_TimestampGetTimeZoneOffset
(
    OCI_Timestamp *tmsp,
    int *          hour,
    int *          min
);

/**
 * @brief
 * Add an interval value to a timestamp value of a timestamp handle
 *
 * @param tmsp  - Timestamp handle
 * @param itv   - Interval handle
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_TimestampIntervalAdd
(
    OCI_Timestamp *tmsp,
    OCI_Interval * itv
);

/**
 * @brief
 * Subtract an interval value from a timestamp value of a timestamp handle
 *
 * @param tmsp  - Timestamp handle
 * @param itv   - Interval handle
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_TimestampIntervalSub
(
    OCI_Timestamp *tmsp,
    OCI_Interval * itv
);

/**
 * @brief
 * Store the difference of two timestamp handles into an interval handle
 *
 * @param tmsp  - Timestamp handle (subtrahend)
 * @param tmsp2 - Timestamp2 handle (minuend)
 * @param itv   - Interval handle
 *
 * @note
 * The function acts like tmsp - tmsp2 = itv
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_TimestampSubtract
(
    OCI_Timestamp *tmsp,
    OCI_Timestamp *tmsp2,
    OCI_Interval * itv
);

/**
 * @brief
 * Stores the system current date and time as a timestamp value with time zone
 * into the timestamp handle.
 *
 * @param tmsp  - Timestamp handle
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_TimestampSysTimestamp
(
    OCI_Timestamp *tmsp
);

/**
 * @brief
 * Affect an OCI_Timestamp handle value to ISO C time data types
 *
 * @param tmsp  - Timestamp handle
 * @param ptm   - Pointer to a structure tm to receive date/time values
 * @param pt    - Pointer to a time_t to hold the date/time in the time_t format
 *
 * @note
 * Both parameters 'ptm' and 'p' are optional but one of them has to be provided.
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_TimestampToCTime
(
    OCI_Timestamp *tmsp,
    struct tm *    ptm,
    time_t *       pt
);

/**
 * @brief
 * Affect ISO C time data types values to an OCI_Timestamp handle
 *
 * @param tmsp  - Timestamp handle
 * @param ptm   - Pointer to a structure tm that hold the date/time value
 * @param t     - Value (time_t) that hold the date/time in the time_t format
 *
 * @note
 * Both parameters 'ptm' and 'p' are optional but one of them has to be provided.
 * If 'ptm' is not null, its value is affected to the OCI_Timestamp handle,
 * otherwise the value of 't' is used.
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_TimestampFromCTime
(
    OCI_Timestamp *tmsp,
    struct tm *    ptm,
    time_t         t
);

/**
 * @brief
 * Create a local interval object
 *
 * @param con  - Connection handle
 * @param type - Type of Interval
 *
 * @note
 * From version 2.5.0, parameter 'con' can be NULL in order to manipulate
 * intervals independently from database connections
 *
 * @note
 * Interval type can be :
 * - OCI_INTERVAL_YM : year / month interval
 * - OCI_INTERVAL_DS : date/ time interval
 *
 * @return
 * Return the Interval handle on success otherwise NULL on failure
 *
 */

OCI_SYM_PUBLIC OCI_Interval * OCI_API OCI_IntervalCreate
(
    OCI_Connection *con,
    unsigned int    type
);

/**
 * @brief
 * Free an OCI_Interval handle
 *
 * @param itv - Interval handle
 *
 * @warning
 * Only Intervals created with OCI_IntervalCreate() should be freed by
 * OCI_IntervalFree()
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_IntervalFree
(
    OCI_Interval *itv
);

/**
 * @brief
 * Create an array of Interval object
 *
 * @param con    - Connection handle
 * @param type   - Type of Interval
 * @param nbelem - number of elements in the array
 *
 * @note
 * see OCI_IntervalCreate() for more details
 *
 * @return
 * Return the Interval handle array on success otherwise NULL on failure
 *
 */

OCI_SYM_PUBLIC OCI_Interval ** OCI_API OCI_IntervalArrayCreate
(
    OCI_Connection *con,
    unsigned int    type,
    unsigned int    nbelem
);

/**
 * @brief
 * Free an array of Interval objects
 *
 * @param itvs - Array of Interval objects
 *
 * @warning
 * Only arrays of Interval created with OCI_IntervalArrayCreate() should be freed by
 * OCI_IntervalArrayFree()
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_IntervalArrayFree
(
    OCI_Interval **itvs
);

/**
 * @brief
 * Return the type of the given Interval object
 *
 * @param itv - Interval handle
 *
 * @note
 * For possible values, see OCI_IntervalCreate()
 *
 * @return
 * Object type or OCI_UNKNOWN the input handle is NULL
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_IntervalGetType
(
    OCI_Interval *itv
);

/**
 * @brief
 * Assign the value of a interval handle to another one
 *
 * @param itv     - Destination interval handle
 * @param itv_src - Source interval handle
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_IntervalAssign
(
    OCI_Interval *itv,
    OCI_Interval *itv_src
);

/**
 * @brief
 * Check if the given interval is valid
 *
 * @param itv - Interval handle
 *
 * @return
 * - Zero if the interval value is valid
 * - Any other value means the interval value is invalid
 *
 */

OCI_SYM_PUBLIC int OCI_API OCI_IntervalCheck
(
    OCI_Interval *itv
);

/**
 * @brief
 * Compares two interval handles
 *
 * @param itv  - Interval1 handle
 * @param itv2 - Interval2 handle
 *
 * @return
 * - -1 if interval1 is smaller than interval2,
 * -  0 if they are equal
 * -  1 if interval1 is greater than interval2.
 *
 */

OCI_SYM_PUBLIC int OCI_API OCI_IntervalCompare
(
    OCI_Interval *itv,
    OCI_Interval *itv2
);

/**
 * @brief
 * Convert a string to an interval and store it in the given interval handle
 *
 * @param itv  - Destination interval handle
 * @param str  - Source date string
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_IntervalFromText
(
    OCI_Interval *itv,
    const otext * str
);

/**
 * @brief
 * Convert an interval value from the given interval handle to a string
 *
 * @param itv           - source Interval handle
 * @param leading_prec  - Precision of the leading part
 * @param fraction_prec - Precision of the fractional part
 * @param size          - Destination string size in characters
 * @param str           - Destination date string
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_IntervalToText
(
    OCI_Interval *itv,
    int           leading_prec,
    int           fraction_prec,
    int           size,
    otext *       str
);

/**
 * @brief
 * Correct an interval handle value with the given time zone
 *
 * @param itv - Interval handle
 * @param str - Time zone name
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_IntervalFromTimeZone
(
    OCI_Interval *itv,
    const otext * str
);

/**
 * @brief
 * Return the day / time portion of an interval handle
 *
 * @param itv  - Interval handle
 * @param day  - Place holder for day value
 * @param hour - Place holder for hours value
 * @param min  - Place holder for minutes value
 * @param sec  - Place holder for seconds value
 * @param fsec - Place holder for fractional part of seconds value
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_IntervalGetDaySecond
(
    OCI_Interval *itv,
    int *         day,
    int *         hour,
    int *         min,
    int *         sec,
    int *         fsec
);

/**
 * @brief
 * Return the year / month portion of an interval handle
 *
 * @param itv   - Interval handle
 * @param year  - Place holder for year value
 * @param month - Place holder for month value
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_IntervalGetYearMonth
(
    OCI_Interval *itv,
    int *         year,
    int *         month
);

/**
 * @brief
 * Set the day / time portion if the given interval handle
 *
 * @param itv   - Interval handle
 * @param day   - day value
 * @param hour  - Hour value
 * @param min   - Minute value
 * @param sec   - Second value
 * @param fsec  - Fractional part of the seconds
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_IntervalSetDaySecond
(
    OCI_Interval *itv,
    int           day,
    int           hour,
    int           min,
    int           sec,
    int           fsec
);

/**
 * @brief
 * Set the year / month portion if the given Interval handle
 *
 * @param itv    - Interval handle
 * @param year   - Year value
 * @param month  - Month value
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_IntervalSetYearMonth
(
    OCI_Interval *itv,
    int           year,
    int           month
);

/**
 * @brief
 * Adds an interval handle value to another
 *
 * @param itv   - Interval handle from witch to add
 * @param itv2  - Interval handle to add
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_IntervalAdd
(
    OCI_Interval *itv,
    OCI_Interval *itv2
);

/**
 * @brief
 * Subtract an interval handle value from another
 *
 * @param itv   - Interval handle from witch to remove
 * @param itv2  - Interval handle to remove
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_IntervalSubtract
(
    OCI_Interval *itv,
    OCI_Interval *itv2
);

/**
 * @} OcilibCApiTimestamps
 */

/**
 * @defgroup OcilibCApiUserTypes Oracle Named Types (Oracle OBJECTs)
 * @{
 *
 * OCILIB implements Oracle Named types (user types and built-in types) through
 * the OCI_Object type.
 *
 * OTT and C structures are not required to use objects in OCILIB.
 *
 * In order to manipulate objects attributes, OCILIB proposes a set of functions
 * to get/set properties for various supported types.
 *
 * Objects can be:
 * - Created as standalone instances (transient objects)
 * - Used for binding (persistent / transient objects)
 * - Retrieved from select statements (persistent / embedded objects)
 *
 * References (Oracle type REF) are identifiers (smart pointers) to objects and
 * are implemented in OCILIB with the type OCI_Ref.
 *
 * OCILIB implements Oracle REFs as strong typed Reference (underlying OCI REFs
 * are weaker in terms of typing).
 * It means it's mandatory to provide type information to:
 * - create a local OCI_Ref handle.
 * - register an OCI_Ref handle for a 'returning into' clause.
 *
 * @note
 * See Oracle Database SQL Language Reference for more details about REF data type
 *
 * @warning
 * Prior to v3.5.0, OCILIB relied on some OCI routines to set/get objects
 * attributes. these OCI calls had known bugs in Unicode mode that has been fixed in Oracle 11gR2.
 * From v3.5.0, OCILIB directly sets objects attributes and thus OCILIB objects
 * can now be used in Unicode mode.
 *
 * @par Example : Inserting a local object into a table
 * @include object.c
 *
 * @par Example : Using Object References
 * @include ref.c
 *
 */

/**
 * @brief
 * Create a local object instance
 *
 * @param con    - Connection handle
 * @param typinf - Object type (type info handle)
 *
 * @return
 * Return the object handle on success otherwise NULL on failure
 *
 */

OCI_SYM_PUBLIC OCI_Object * OCI_API OCI_ObjectCreate
(
    OCI_Connection *con,
    OCI_TypeInfo *  typinf
);

/**
 * @brief
 * Free a local object
 *
 * @param obj - Object handle
 *
 * @warning
 * Only object created with OCI_ObjectCreate() should be freed
 * by OCI_ObjectFree()
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_ObjectFree
(
    OCI_Object *obj
);

/**
 * @brief
 * Create an array of Object objects
 *
 * @param con    - Connection handle
 * @param typinf - Object type (type info handle)
 * @param nbelem - number of elements in the array
 *
 * @note
 * see OCI_ObjectCreate() for more details
 *
 * @return
 * Return the Object handle array on success otherwise NULL on failure
 *
 */

OCI_SYM_PUBLIC OCI_Object ** OCI_API OCI_ObjectArrayCreate
(
    OCI_Connection *con,
    OCI_TypeInfo *  typinf,
    unsigned int    nbelem
);

/**
 * @brief
 * Free an array of Object objects
 *
 * @param objs - Array of Object objects
 *
 * @warning
 * Only arrays of Object created with OCI_ObjectArrayCreate()
 * should be freed by OCI_ObjectArrayFree()
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_ObjectArrayFree
(
    OCI_Object **objs
);

/**
 * @brief
 * Assign an object to another one
 *
 * @param obj     - Destination Object handle
 * @param obj_src - Source Object handle
 *
 * @note
 * Oracle proceeds to a deep copy of the object content
 *
 * @note
 * The two object handles must have the same type otherwise an exception is thrown
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_ObjectAssign
(
    OCI_Object *obj,
    OCI_Object *obj_src
);

/**
 * @brief
 * Return the type of an object instance
 *
 * @param obj - Object handle
 *
 * @note
 * Possibles values are :
 *
 * - OCI_OBJ_PERSISTENT: persistent object from the DB
 * - OCI_OBJ_TRANSIENT : local temporary object
 * - OCI_OBJ_VALUE     : embedded object
 *
 * @return
 * Instance type or OCI_UNKNOWN the input handle is NULL
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_ObjectGetType
(
    OCI_Object *obj
);

/**
 * @brief
 * Retrieve an Oracle Ref handle from an object and assign it to the given
 * OCILIB OCI_Ref handle
 *
 * @param obj - Object handle
 * @param ref - Ref handle
 *
 * @note
 * The type information of the object and the ref must be the same, otherwise
 * an exception is thrown
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_ObjectGetSelfRef
(
    OCI_Object *obj,
    OCI_Ref *   ref
);

/**
 * @brief
 * Return the type info object associated to the object
 *
 * @param obj - Object handle
 *
 */

OCI_SYM_PUBLIC OCI_TypeInfo * OCI_API OCI_ObjectGetTypeInfo
(
    OCI_Object *obj
);

/**
 * @brief
 * Return the boolean value of the given object attribute (ONLY for PL/SQL records)
 *
 * @param obj  - Object handle
 * @param attr - Attribute name
 *
 * @note
 * If the attribute is found in the object descriptor attributes list, then a
 * data type check is performed for integrity.
 * OCI_ObjectGetBoolean() returns a valid value only for PL/SQL boolean based attributes
 *
 * @warning
 * - ONLY supported by Oracle 12c and above !
 *
 * @return
 * Attribute value or 0 on failure or wrong attribute type
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_ObjectGetBoolean
(
    OCI_Object * obj,
    const otext *attr
);

/**
* @brief
* Return the number value of the given object attribute
*
* @param obj  - Object handle
* @param attr - Attribute name
*
* @note
* If the attribute is found in the object descriptor attributes list, then a
* data type check is performed for integrity.
* OCI_ObjectGetNumber() returns a valid value only for number based attributes
*
* @return
* Attribute value or NULL on failure or wrong attribute type
*
*/

OCI_SYM_PUBLIC OCI_Number * OCI_API OCI_ObjectGetNumber
(
    OCI_Object * obj,
    const otext *attr
);

/**
 * @brief
 * Return the short value of the given object attribute
 *
 * @param obj  - Object handle
 * @param attr - Attribute name
 *
 * @note
 * If the attribute is found in the object descriptor attributes list, then a
 * data type check is performed for integrity.
 * OCI_ObjectGetShort() returns a valid value only for integer and number based attributes
 *
 * @return
 * Attribute value or 0 on failure or wrong attribute type
 *
 */

OCI_SYM_PUBLIC short OCI_API OCI_ObjectGetShort
(
    OCI_Object * obj,
    const otext *attr
);

/**
 * @brief
 * Return the unsigned short value of the given object attribute
 *
 * @param obj  - Object handle
 * @param attr - Attribute name
 *
 * @note
 * If the attribute is found in the object descriptor attributes list, then a
 * data type check is performed for integrity.
 * OCI_ObjectGetUnsignedShort() returns a valid value only for integer and number based attributes
 *
 * @return
 * Attribute value or 0 on failure or wrong attribute type
 *
 */

OCI_SYM_PUBLIC unsigned short OCI_API OCI_ObjectGetUnsignedShort
(
    OCI_Object * obj,
    const otext *attr
);

/**
 * @brief
 * Return the integer value of the given object attribute
 *
 * @param obj  - Object handle
 * @param attr - Attribute name
 *
 * @note
 * If the attribute is found in the object descriptor attributes list, then a
 * data type check is performed for integrity.
 * OCI_ObjectGetInt() returns a valid value only for integer and number based attributes
 *
 * @return
 * Attribute value or 0 on failure or wrong attribute type
 *
 */

OCI_SYM_PUBLIC int OCI_API OCI_ObjectGetInt
(
    OCI_Object * obj,
    const otext *attr
);

/**
 * @brief
 * Return the unsigned integer value of the given object attribute
 *
 * @param obj  - Object handle
 * @param attr - Attribute name
 *
 * @note
 * If the attribute is found in the object descriptor attributes list, then a
 * data type check is performed for integrity.
 * OCI_ObjectGetUnsignedInt() returns a valid value only for integer and number based attributes
 *
 * @return
 * Attribute value or 0 on failure or wrong attribute type
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_ObjectGetUnsignedInt
(
    OCI_Object * obj,
    const otext *attr
);

/**
 * @brief
 * Return the big integer value of the given object attribute
 *
 * @param obj  - Object handle
 * @param attr - Attribute name
 *
 * @note
 * If the attribute is found in the object descriptor attributes list, then a
 * data type check is performed for integrity.
 * OCI_ObjectGetBigInt() returns a valid value only for integer and number based attributes
 *
 * @return
 * Attribute value or 0 on failure or wrong attribute type
 *
 */

OCI_SYM_PUBLIC big_int OCI_API OCI_ObjectGetBigInt
(
    OCI_Object * obj,
    const otext *attr
);

/**
 * @brief
 * Return the unsigned big integer value of the given object attribute
 *
 * @param obj  - Object handle
 * @param attr - Attribute name
 *
 * @note
 * If the attribute is found in the object descriptor attributes list, then a
 * data type check is performed for integrity.
 * OCI_ObjectGetUnsignedBigInt() returns a valid value only for integer and number based attributes
 *
 * @return
 * Attribute value or 0 on failure or wrong attribute type
 *
 */

OCI_SYM_PUBLIC big_uint OCI_API OCI_ObjectGetUnsignedBigInt
(
    OCI_Object * obj,
    const otext *attr
);

/**
 * @brief
 * Return the double value of the given object attribute
 *
 * @param obj  - Object handle
 * @param attr - Attribute name
 *
 * @note
 * If the attribute is found in the object descriptor attributes list, then a
 * data type check is performed for integrity.
 * OCI_ObjectGetDouble() returns a valid value only for integer and number based attributes
 *
 * @return
 * Attribute value or 0.0 on failure or wrong attribute type
 *
 */

OCI_SYM_PUBLIC double OCI_API OCI_ObjectGetDouble
(
    OCI_Object * obj,
    const otext *attr
);

/**
 * @brief
 * Return the float value of the given object attribute
 *
 * @param obj  - Object handle
 * @param attr - Attribute name
 *
 * @note
 * If the attribute is found in the object descriptor attributes list, then a
 * data type check is performed for integrity.
 * OCI_ObjectGetFloat() returns a valid value only for integer and number based attributes
 *
 * @return
 * Attribute value or 0.0 on failure or wrong attribute type
 *
 */

OCI_SYM_PUBLIC float OCI_API OCI_ObjectGetFloat
(
    OCI_Object * obj,
    const otext *attr
);

/**
 * @brief
 * Return the string value of the given object attribute
 *
 * @param obj  - Object handle
 * @param attr - Attribute name
 *
 * @note
 * The method can return a string value for any attributes types.
 * It performs implicit string conversions using the same
 * mechanisms than OCI_GetString(). See its documentation for more details.
 *
 * @return
 * Attribute value or NULL on failure
 *
 */

OCI_SYM_PUBLIC const otext * OCI_API OCI_ObjectGetString
(
    OCI_Object * obj,
    const otext *attr
);

/**
 * @brief
 * Return the raw attribute value of the given object attribute into the
 * given buffer
 *
 * @param obj   - Object handle
 * @param attr  - Attribute name
 * @param value - Destination buffer
 * @param len   - Max size to write into buffer

 * @note
 * If the attribute is found in the object descriptor attributes list, then a
 * data type check is performed for integrity.
 * OCI_ObjectGetRaw() copies data into the buffer only for raw based attributes
 *
 * @return
 * Number of bytes written to the buffer or 0 on failure or wrong attribute type
 *
 */

OCI_SYM_PUBLIC int OCI_API OCI_ObjectGetRaw
(
    OCI_Object * obj,
    const otext *attr,
    void *       value,
    unsigned int len
);

/**
* @brief
* Return the raw attribute value size of the given object attribute into the
* given buffer
*
* @param obj   - Object handle
* @param attr  - Attribute name
*
* @note
* If the attribute is found in the object descriptor attributes list, then a
* data type check is performed for integrity.
*
* @return
* size in bytes of the RAW value or 0 on failure or wrong attribute type
*
*/

OCI_SYM_PUBLIC unsigned int OCI_API OCI_ObjectGetRawSize
(
    OCI_Object * obj,
    const otext *attr
);

/**
 * @brief
 * Return the date value of the given object attribute
 *
 * @param obj  - Object handle
 * @param attr - Attribute name
 *
 * @note
 * If the attribute is found in the object descriptor attributes list, then a
 * data type check is performed for integrity.
 * OCI_ObjectGetDate() returns a valid value only for date based attributes
 *
 * @return
 * Attribute value or NULL on failure or wrong attribute type
 *
 */

OCI_SYM_PUBLIC OCI_Date * OCI_API OCI_ObjectGetDate
(
    OCI_Object * obj,
    const otext *attr
);

/**
 * @brief
 * Return the timestamp value of the given object attribute
 *
 * @param obj  - Object handle
 * @param attr - Attribute name
 *
 * @note
 * If the attribute is found in the object descriptor attributes list, then a
 * data type check is performed for integrity.
 * OCI_ObjectGetTimestamp() returns a valid value only for timestamps based attributes
 *
 * @return
 * Attribute value or NULL on failure or wrong attribute type
 *
 */

OCI_SYM_PUBLIC OCI_Timestamp * OCI_API OCI_ObjectGetTimestamp
(
    OCI_Object * obj,
    const otext *attr
);

/**
 * @brief
 * Return the interval value of the given object attribute
 *
 * @param obj  - Object handle
 * @param attr - Attribute name
 *
 * @note
 * If the attribute is found in the object descriptor attributes list, then a
 * data type check is performed for integrity.
 * OCI_ObjectGetInterval() returns a valid value only for intervals based attributes
 *
 * @return
 * Attribute value or NULL on failure or wrong attribute type
 *
 */

OCI_SYM_PUBLIC OCI_Interval * OCI_API OCI_ObjectGetInterval
(
    OCI_Object * obj,
    const otext *attr
);

/**
 * @brief
 * Return the collection value of the given object attribute
 *
 * @param obj  - Object handle
 * @param attr - Attribute name
 *
 * @note
 * If the attribute is found in the object descriptor attributes list, then a
 * data type check is performed for integrity.
 * OCI_ObjectGetColl() returns a valid value only for intervals based attributes
 *
 * @return
 * Attribute value or NULL on failure or wrong attribute type
 *
 */

OCI_SYM_PUBLIC OCI_Coll * OCI_API OCI_ObjectGetColl
(
    OCI_Object * obj,
    const otext *attr
);

/**
 * @brief
 * Return the Ref value of the given object attribute
 *
 * @param obj  - Object handle
 * @param attr - Attribute name
 *
 * @note
 * If the attribute is found in the object descriptor attributes list, then a
 * data type check is performed for integrity.
 * OCI_ObjectGetRef() returns a valid value only for Refs based attributes
 *
 * @return
 * Attribute value or NULL on failure or wrong attribute type
 *
 */

OCI_SYM_PUBLIC OCI_Ref * OCI_API OCI_ObjectGetRef
(
    OCI_Object * obj,
    const otext *attr
);

/**
 * @brief
 * Return the object value of the given object attribute
 *
 * @param obj  - Object handle
 * @param attr - Attribute name
 *
 * @note
 * If the attribute is found in the object descriptor attributes list, then a
 * data type check is performed for integrity.
 * OCI_ObjectGetObject() returns a valid value only for object based attributes
 *
 * @return
 * Attribute value or NULL on failure or wrong attribute type
 *
 */

OCI_SYM_PUBLIC OCI_Object * OCI_API OCI_ObjectGetObject
(
    OCI_Object * obj,
    const otext *attr
);

/**
 * @brief
 * Return the lob value of the given object attribute
 *
 * @param obj  - Object handle
 * @param attr - Attribute name
 *
 * @note
 * If the attribute is found in the object descriptor attributes list, then a
 * data type check is performed for integrity.
 * OCI_ObjectGetLob() returns a valid value only for lobs based attributes
 *
 * @return
 * Attribute value or NULL on failure or wrong attribute type
 *
 */

OCI_SYM_PUBLIC OCI_Lob * OCI_API OCI_ObjectGetLob
(
    OCI_Object * obj,
    const otext *attr
);

/**
 * @brief
 * Return the file value of the given object attribute
 *
 * @param obj  - Object handle
 * @param attr - Attribute name
 *
 * @note
 * If the attribute is found in the object descriptor attributes list, then a
 * data type check is performed for integrity.
 * OCI_ObjectGetFile() returns a valid value only for files based attributes
 *
 * @return
 * Attribute value or NULL on failure or wrong attribute type
 *
 */

OCI_SYM_PUBLIC OCI_File * OCI_API OCI_ObjectGetFile
(
    OCI_Object * obj,
    const otext *attr
);

/**
 * @brief
 * Set an object attribute of type boolean (ONLY for PL/SQL records)
 *
 * @param obj    - Object handle
 * @param attr   - Attribute name
 * @param value  - boolean value
 *
 * @warning
 * - ONLY supported by Oracle 12c and above !
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_ObjectSetBoolean
(
    OCI_Object * obj,
    const otext *attr,
    boolean      value
);

/**
* @brief
* Set an object attribute of type number
*
* @param obj    - Object handle
* @param attr   - Attribute name
* @param value  - number value
*
* @return
* TRUE on success otherwise FALSE
*
*/

OCI_SYM_PUBLIC boolean OCI_API OCI_ObjectSetNumber
(
    OCI_Object * obj,
    const otext *attr,
    OCI_Number * value
);

/**
 * @brief
 * Set an object attribute of type short
 *
 * @param obj    - Object handle
 * @param attr   - Attribute name
 * @param value  - Short value
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_ObjectSetShort
(
    OCI_Object * obj,
    const otext *attr,
    short        value
);

/**
 * @brief
 * Set an object attribute of type unsigned short
 *
 * @param obj    - Object handle
 * @param attr   - Attribute name
 * @param value  - Unsigned short value
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_ObjectSetUnsignedShort
(
    OCI_Object *   obj,
    const otext *  attr,
    unsigned short value
);

/**
 * @brief
 * Set an object attribute of type int
 *
 * @param obj    - Object handle
 * @param attr   - Attribute name
 * @param value  - Int value
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_ObjectSetInt
(
    OCI_Object * obj,
    const otext *attr,
    int          value
);

/**
 * @brief
 * Set an object attribute of type unsigned int
 *
 * @param obj    - Object handle
 * @param attr   - Attribute name
 * @param value  - Unsigned int value
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_ObjectSetUnsignedInt
(
    OCI_Object * obj,
    const otext *attr,
    unsigned int value
);

/**
 * @brief
 * Set an object attribute of type big int
 *
 * @param obj    - Object handle
 * @param attr   - Attribute name
 * @param value  - Big int value
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_ObjectSetBigInt
(
    OCI_Object * obj,
    const otext *attr,
    big_int      value
);

/**
 * @brief
 * Set an object attribute of type unsigned big int
 *
 * @param obj    - Object handle
 * @param attr   - Attribute name
 * @param value  - Unsigned big int value
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_ObjectSetUnsignedBigInt
(
    OCI_Object * obj,
    const otext *attr,
    big_uint     value
);

/**
 * @brief
 * Set an object attribute of type double
 *
 * @param obj    - Object handle
 * @param attr   - Attribute name
 * @param value  - Double value
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_ObjectSetDouble
(
    OCI_Object * obj,
    const otext *attr,
    double       value
);

/**
 * @brief
 * Set an object attribute of type float
 *
 * @param obj    - Object handle
 * @param attr   - Attribute name
 * @param value  - Float value
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_ObjectSetFloat
(
    OCI_Object * obj,
    const otext *attr,
    float        value
);

/**
 * @brief
 * Set an object attribute of type string
 *
 * @param obj    - Object handle
 * @param attr   - Attribute name
 * @param value  - String value
 *
 * @note
 * passing a null pointer for value calls OCI_ObjectSetNull()
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_ObjectSetString
(
    OCI_Object * obj,
    const otext *attr,
    const otext *value
);

/**
 * @brief
 * Set an object attribute of type RAW
 *
 * @param obj    - Object handle
 * @param attr   - Attribute name
 * @param value  - Raw value
 * @param len    - Size of the raw value
 *
 * @note
 * passing a null pointer for value calls OCI_ObjectSetNull()
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_ObjectSetRaw
(
    OCI_Object * obj,
    const otext *attr,
    void *       value,
    unsigned int len
);

/**
 * @brief
 * Set an object attribute of type Date
 *
 * @param obj    - Object handle
 * @param attr   - Attribute name
 * @param value  - Date Handle
 *
 * @note
 * passing a null pointer for value calls OCI_ObjectSetNull()
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_ObjectSetDate
(
    OCI_Object * obj,
    const otext *attr,
    OCI_Date *   value
);

/**
 * @brief
 * Set an object attribute of type Timestamp
 *
 * @param obj    - Object handle
 * @param attr   - Attribute name
 * @param value  - Timestamp Handle
 *
 * @note
 * passing a null pointer for value calls OCI_ObjectSetNull()
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_ObjectSetTimestamp
(
    OCI_Object *   obj,
    const otext *  attr,
    OCI_Timestamp *value
);

/**
 * @brief
 * Set an object attribute of type Interval
 *
 * @param obj    - Object handle
 * @param attr   - Attribute name
 * @param value  - Interval Handle
 *
 * @note
 * passing a null pointer for value calls OCI_ObjectSetNull()
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_ObjectSetInterval
(
    OCI_Object *  obj,
    const otext * attr,
    OCI_Interval *value
);

/**
 * @brief
 * Set an object attribute of type Collection
 *
 * @param obj    - Object handle
 * @param attr   - Attribute name
 * @param value  - Collection Handle
 *
 * @note
 * passing a null pointer for value calls OCI_ObjectSetNull()
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_ObjectSetColl
(
    OCI_Object * obj,
    const otext *attr,
    OCI_Coll *   value
);

/**
 * @brief
 * Set an object attribute of type Object
 *
 * @param obj    - Object handle
 * @param attr   - Attribute name
 * @param value  - Object Handle
 *
 * @warning
 * This function assigns a copy of the object to the given attribute.
 * Any further modifications of the object passed as the parameter 'value'
 * will not be reflected to object 's attribute set with this call
 *
 * @note
 * passing a null pointer for value calls OCI_ObjectSetNull()
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_ObjectSetObject
(
    OCI_Object * obj,
    const otext *attr,
    OCI_Object * value
);

/**
 * @brief
 * Set an object attribute of type Lob
 *
 * @param obj    - Object handle
 * @param attr   - Attribute name
 * @param value  - Lob Handle
 *
 * @note
 * passing a null pointer for value calls OCI_ObjectSetNull()
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_ObjectSetLob
(
    OCI_Object * obj,
    const otext *attr,
    OCI_Lob *    value
);

/**
 * @brief
 * Set an object attribute of type File
 *
 * @param obj    - Object handle
 * @param attr   - Attribute name
 * @param value  - File Handle
 *
 * @note
 * passing a null pointer for value calls OCI_ObjectSetNull()
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_ObjectSetFile
(
    OCI_Object * obj,
    const otext *attr,
    OCI_File *   value
);

/**
 * @brief
 * Set an object attribute of type Ref
 *
 * @param obj    - Object handle
 * @param attr   - Attribute name
 * @param value  - Ref Handle
 *
 * @note
 * passing a null pointer for value calls OCI_ObjectSetNull()
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_ObjectSetRef
(
    OCI_Object * obj,
    const otext *attr,
    OCI_Ref *    value
);

/**
 * @brief
 * Check if an object attribute is null
 *
 * @param obj    - Object handle
 * @param attr   - Attribute name
 *
 * @return
 * FALSE if the attribute is not null otherwise TRUE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_ObjectIsNull
(
    OCI_Object * obj,
    const otext *attr
);

/**
 * @brief
 * Set an object attribute to null
 *
 * @param obj    - Object handle
 * @param attr   - Attribute name
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_ObjectSetNull
(
    OCI_Object * obj,
    const otext *attr
);

/**
 * @brief
 * Retrieve the underlying C (OTT/OCI style) structure of an OCI_Object handle
 *
 * @param obj       - Object handle
 * @param pp_struct - Address of a pointer that retrieve the C structure of data
 * @param pp_ind    - Address of a pointer that retrieve the C structure of indicators
 *
 * @note
 * See Oracle OCI programming guide for more details about OTT structures.
 * The members of these structures are OCI data types like OCINumber, OCIString
 * that requires mixing OCILIB code and raw OCI code.
 * OCI Object API headers have to be included to handle this data types using OCI object functions
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_ObjectGetStruct
(
    OCI_Object *obj,
    void **     pp_struct,
    void **     pp_ind
);

/**
 * @brief
 * Convert an object handle value to a string
 *
 * @param obj  - Object handle
 * @param size - Destination string length pointer in characters
 * @param str  - Destination string
 *
 * @note
 * In order to compute the needed string length, call the method with a NULL string
 * Then call the method again with a valid buffer
 *
 * @note
 * The resulting string is similar to the SQL*PLUS output for UDTs (user types and objects)
 * For RAWs and BLOBs attributes, their binary values are converted to hexadecimal strings
 *
 * @warning
 * This convenient method shall not be used when performance matters. It is usually called twice (buffer length
 * computation) and must also care about quotes within strings.
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_ObjectToText
(
    OCI_Object *  obj,
    unsigned int *size,
    otext *       str
);

/**
 * @brief
 * Create a local Ref instance
 *
 * @param con    - Connection handle
 * @param typinf - Ref type
 *
 * @return
 * Return the Ref handle on success otherwise NULL on failure
 *
 */

OCI_SYM_PUBLIC OCI_Ref * OCI_API OCI_RefCreate
(
    OCI_Connection *con,
    OCI_TypeInfo *  typinf
);

/**
 * @brief
 * Free a local Ref
 *
 * @param ref - Ref handle
 *
 * @warning
 * Only Refs created with OCI_RefCreate() should be freed
 * by OCI_RefFree()
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_RefFree
(
    OCI_Ref *ref
);

/**
 * @brief
 * Create an array of Ref object
 *
 * @param con    - Connection handle
 * @param typinf - Object type (type info handle)
 * @param nbelem - number of elements in the array
 *
 * @note
 * see OCI_RefCreate() for more details
 *
 * @return
 * Return the Ref handle array on success otherwise NULL on failure
 *
 */

OCI_SYM_PUBLIC OCI_Ref ** OCI_API OCI_RefArrayCreate
(
    OCI_Connection *con,
    OCI_TypeInfo *  typinf,
    unsigned int    nbelem
);

/**
 * @brief
 * Free an array of Ref objects
 *
 * @param refs - Array of Ref objects
 *
 * @warning
 * Only arrays of Ref created with OCI_RefArrayCreate()
 * should be freed by OCI_RefArrayFree()
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_RefArrayFree
(
    OCI_Ref **refs
);

/**
 * @brief
 * Assign a Ref to another one
 *
 * @param ref     - Destination Ref handle
 * @param ref_src - Source Ref handle
 *
 * @note
 * The two Ref handles must have the same type otherwise an exception is thrown
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_RefAssign
(
    OCI_Ref *ref,
    OCI_Ref *ref_src
);

/**
 * @brief
 * Return the type info object associated to the Ref
 *
 * @param ref - Ref handle
 *
 */

OCI_SYM_PUBLIC OCI_TypeInfo * OCI_API OCI_RefGetTypeInfo
(
    OCI_Ref *ref
);

/**
 * @brief
 * Returns the object pointed by the Ref handle.
 *
 * @param ref   - Ref handle
 *
 * @return
 * The object handle is the ref is not null otherwise NULL
 *
 */

OCI_SYM_PUBLIC OCI_Object * OCI_API OCI_RefGetObject
(
    OCI_Ref *ref
);

/**
 * @brief
 * Check if the Ref points to an object or not.
 *
 * @param ref   - Ref handle
 *
 * @return
 * TRUE if it does not point to a valid object otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_RefIsNull
(
    OCI_Ref *ref
);

/**
 * @brief
 * Nullify the given Ref handle
 *
 * @param ref   - Ref handle
 *
 * @note
 * this call clears the reference to object pointed by the Ref handle.
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_RefSetNull
(
    OCI_Ref *ref
);

/**
 * @brief
 * Returns the size of the hex representation of the given Ref handle
 *
 * @param ref  - Ref handle
 *
 * @note
 * the returned size is the number of character needed to store the
 * hex representation of the Ref that can be retrieved with OCI_RefToText()
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_RefGetHexSize
(
    OCI_Ref *ref
);

/**
 * @brief
 * Converts a Ref handle value to a hexadecimal string.
 *
 * @param ref   - Ref handle
 * @param size - Destination string size in characters
 * @param str  - Destination string
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_RefToText
(
    OCI_Ref *    ref,
    unsigned int size,
    otext *      str
);

/**
 * @} OcilibCApiUserTypes
 */

/**
 * @defgroup OcilibCApiAbort Aborting long operations
 * @{
 *
 * The Oracle OCI provides the ability to establish a server connection in :
 *
 * - blocking mode: each call to an OCI function returns control to the
 *   application when the call completes
 * - non-blocking mode (based on polling paradigm) : the application have to
 *   call each function until its has completed its job
 *
 * OCILIB implements OCI in blocking mode. The application has to wait for OCI
 * calls to complete to continue.
 *
 * Some operations can be long to be processed by the server.
 *
 * In order to cancel the current pending call, OCILIB provides OCI_Break() that
 * cancel the last internal OCI Call and then raise an OCI abortion error code.
 *
 * @note
 * Any call to OCI_Break() has to be done from a separate thread because the
 * thread that has executed a long OCI call is waiting for its OCI call to complete.
 *
 * @par Example
 * @include abort.c
 *
 */

/**
 * @brief
 * Perform an immediate abort of any currently Oracle OCI call
 *
 * @param con - connection handle
 *
 * @note
 * The current call will abort and generate an error
 *
 * @return
 * Returns FALSE if connection handle is NULL otherwise TRUE
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_Break
(
    OCI_Connection *con
);

/**
 * @} OcilibCApiAbort
 */

/**
 * @defgroup OcilibCApiMetadata Describing Schema Meta data and Objects
 * @{
 *
 *
 * @par Example
 * @include desc.c
 *
 */

/**
 * @brief
 * Retrieve the available type info information
 *
 * @param con  - Connection handle
 * @param name - Table/view name to query for
 * @param type - Type of object
 *
 * @note
 * Possible values for parameter type are :
 *
 * - OCI_UNKNOWN
 * - OCI_TIF_TABLE
 * - OCI_TIF_VIEW
 * - OCI_TIF_TYPE
 *
 * @return
 * - Type info handle on success
 = - NULL if the object does not exist
 * - NULL on failure
 *
 */

OCI_SYM_PUBLIC OCI_TypeInfo * OCI_API OCI_TypeInfoGet
(
    OCI_Connection *con,
    const otext *   name,
    unsigned int    type
);

/**
 * @brief
 * Return the type of the type info object
 *
 * @param typinf - Type info handle
 *
 * @note
 * Possible values for parameter type are :
 *
 * - OCI_UNKNOWN
 * - OCI_TIF_TABLE
 * - OCI_TIF_VIEW
 * - OCI_TIF_TYPE
 *
 * @return
 * Object type or OCI_UNKNOWN the input handle is NULL
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_TypeInfoGetType
(
    OCI_TypeInfo *typinf
);

/**
 * @brief
 * Retrieve connection handle from the type info handle
 *
 * @param typinf - Type info handle
 *
 */

OCI_SYM_PUBLIC OCI_Connection * OCI_API OCI_TypeInfoGetConnection
(
    OCI_TypeInfo *typinf
);

/**
 * @brief
 * Free a type info object
 *
 * @param typinf  - Type info handle
 *
 * @note
 * this call is DEPRECATED.
 * OCI_TypeInfo object are internally tracked and
 * automatically freed when their related connection is freed
 * Thus this method does nothing
 *
 * @return
 * TRUE if input handle is valid otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_TypeInfoFree
(
    OCI_TypeInfo *typinf
);

/**
 * @brief
 * Return the number of columns of a table/view/object
 *
 * @param typinf - Type info handle
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_TypeInfoGetColumnCount
(
    OCI_TypeInfo *typinf
);

/**
 * @brief
 * Return the column object handle at the given index in the table
 *
 * @param typinf - Type info handle
 * @param index  - Column position
 *
 * @return
 * - Column handle on success
 * - NULL if index is out of bounds or on error
 *
 */

OCI_SYM_PUBLIC OCI_Column * OCI_API OCI_TypeInfoGetColumn
(
    OCI_TypeInfo *typinf,
    unsigned int  index
);

/**
 * @brief
 * Return the name described by the type info object
 *
 * @param typinf - Type info handle
 *
 */

OCI_SYM_PUBLIC const otext * OCI_API OCI_TypeInfoGetName
(
    OCI_TypeInfo *typinf
);

/**
* @brief
* Indicate if the given UDT type if final
*
* @note
* Non-final types are virtual UDT type that can be inherited
* 
* @param typinf - Type info handle
*
* @note
*  This property is only valid for UDTs (OCI_TIF_TYPE)
*  
* @return
*  - when OCI_TypeInfoGetType() returns OCI_UNKNOWN, OCI_TIF_TABLE, OCI_TIF_VIEW: returns TRUE
*  - when OCI_TypeInfoGetType() returns OCI_TIF_TYPE: returns TRUE if the type is final and FALSE if it is virtual
*
*/

OCI_SYM_PUBLIC boolean OCI_API OCI_TypeInfoIsFinalType
(
    OCI_TypeInfo *typinf
);

/**
* @brief
* Return the super type of the given type (e.g. parent type for a derived ORACLE UDT type)
*
* @param typinf - Type info handle
*
* @note
*  This property is only valid for UDTs (OCI_TIF_TYPE)
*  
* @return
*  - when OCI_TypeInfoGetType() returns OCI_UNKNOWN, OCI_TIF_TABLE, OCI_TIF_VIEW: 
*       - returns NULL
*  - when OCI_TypeInfoGetType() returns OCI_TIF_TYPE: 
*       - returns the TypeInfo handle wrapping the parent super type
*       - returns NULL if the given type is NOT deriving from a base type
*
*/

OCI_SYM_PUBLIC OCI_TypeInfo * OCI_API OCI_TypeInfoGetSuperType
(
    OCI_TypeInfo *typinf
);

/**
 * @} OcilibCApiMetadata
 */

/**
 * @defgroup OcilibCApiFormatting Formatted functions
 * @{
 *
 * OCILIB offers some smart routines that takes a variable number of arguments
 * in order to minimize OCILIB function calls and reduce the amount of code lines
 *
 * On Windows platforms, the target programming language must support the __cdecl
 * calling convention
 *
 * @note
 * OCI_Immediate() and OCI_ImmediateFmt() support all OCILIB supported types
 * for output result, except :
 * - OCI_Long
 * - OCI_Statement
 * If a query output result contains one of these unsupported types, the function returns FALSE
 *
 * @note
 * In the parameter list, every output placeholder MUST be preceded by
 * an integer parameter that indicates the type of the placeholder
 * in order to handle correctly the given pointer.
 *
 * Possible values for indicating placeholders type :
 *
 * - OCI_ARG_SHORT ------> short *
 * - OCI_ARG_USHORT -----> unsigned short *
 * - OCI_ARG_INT --------> int *
 * - OCI_ARG_UINT -------> unsigned int*
 * - OCI_ARG_BIGINT -----> big_int *
 * - OCI_ARG_BIGUINT ----> unsigned big_int *
 * - OCI_ARG_DOUBLE  ----> double *
 * - OCI_ARG_FLOAT ------> float *
 * - OCI_ARG_NUMBER -----> OCI_Number *
 * - OCI_ARG_TEXT -------> otext *
 * - OCI_ARG_RAW --------> void *
 * - OCI_ARG_DATETIME ---> OCI_Date *
 * - OCI_ARG_LOB --------> OCI_Lob *
 * - OCI_ARG_FILE -------> OCI_File *
 * - OCI_ARG_TIMESTAMP --> OCI_Timestamp *
 * - OCI_ARG_INTERVAL ---> OCI_Interval *
 * - OCI_ARG_OBJECT -----> OCI_Object *
 * - OCI_ARG_COLLECTION -> OCI_Coll *
 * - OCI_ARG_REF --------> OCI_Ref *
 *
 * @note
 * For output strings and Raws, returned data is copied to the given buffer
 * instead of returning a pointer the real data.
 * So these buffers must be big enough to hold the column content. No size check is performed.
 *
 * - For strings, only the real string is copied.
 * - For Raws, the number of bytes copied is the column size
 *
 * @warning
 * Input parameters for formatted function only support a restricted set of data types !
 *
 * Supported input identifiers :
 *
 * - '%s'  : (otext *) ----------> input string (quotes are added)
 * - '%m'  : (otext *) ----------> meta data string (no quotes added)
 * - '%t'  : (OCI_Date *) -------> Date
 * - '%p'  : (OCI_Timestamp *) --> timestamp
 * - '%v'  : (OCI_Interval *) ---> Interval
 * - '%i'  : (int) --------------> signed 32 bits integer
 * - '%u'  : (unsigned int) -----> unsigned 32 bits integer
 * - '%li' : (big_int) ----------> signed 64 bits integer
 * - '%lu' : (big_uint) ---------> unsigned 64 bits integer
 * - '%hi' : (short) ------------> signed 16 bits integer
 * - '%hu' : (unsigned short) ---> unsigned 16 bits integer
 * - '%g'  : (double, float ) ---> Numerics
 * - '%n'  : (OCI_Number *) -----> Number
 * - '%r'  : (OCI_Ref *) --------> Reference
 * - '%o'  : (OCI_Object *) -----> Object  (not implemented yet)
 * - '%c'  : (OCI_Coll *) -------> collection  (not implemented yet)
 *
 * @par Example
 * @include format.c
 *
 */

/**
 * @brief
 * Perform 3 calls (prepare+execute+fetch) in 1 call
 *
 * @param con  - Connection handle
 * @param sql  - SQL statement
 * @param ...  - List of program variables address to store the result of fetch operation
 *
 * @note
 * Every output parameter MUST be preceded by an integer parameter that indicates the type
 * of the placeholder in order to handle correctly the given pointer.
 *
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_Immediate
(
    OCI_Connection *con,
    const otext *   sql,
    ...
);

/**
 * @brief
 * Performs 4 call (prepare+bind+execute+fetch) in 1 call
 *
 * @param con  - Connection handle
 * @param sql  - SQL statement
 * @param ...  - List of program values to format the SQL followed by the
 *               output variables addresses for the fetch operation
 *
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_ImmediateFmt
(
    OCI_Connection *con,
    const otext *   sql,
    ...
);

/**
 * @brief
 * Prepare a formatted SQL statement or PL/SQL block.
 *
 * @param stmt - Statement handle
 * @param sql  - SQL statement
 * @param ...  - List of program values to format the SQL
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_SYM_PUBLIC boolean OCI_PrepareFmt
(
    OCI_Statement *stmt,
    const otext *  sql,
    ...
);

/**
 * @brief
 * Execute a formatted SQL statement or PL/SQL block.
 *
 * @param stmt - Statement handle
 * @param sql  - SQL statement
 * @param ...  - List of program values to format the SQL
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 * @warning
 * If a SQL warning occurs:
 * - the function returns TRUE
 * - the SQL warning triggers the global error handler with an OCI_Error having its OCI_ErrorGetType()
 *   attribute set to OCI_ERR_WARNING
 * - If OCILIB is initialized with the OCI_ENV_CONTEXT mode, OCI_GetLastError() will return the OCI_Error
 *   object corresponding to the warning
 *
 */

OCI_SYM_PUBLIC boolean OCI_ExecuteStmtFmt
(
    OCI_Statement *stmt,
    const otext *  sql,
    ...
);

/**
 * @brief
 * Parse a formatted SQL statement or PL/SQL block.
 *
 * @param stmt - Statement handle
 * @param sql  - SQL statement
 * @param ...  - List of program values to format the SQL
 *
 * @note
 * This call sends the SQL or PL/SQL command to the server for parsing only.
 * The command is not executed.
 * This call is only useful to check is a command is valid or not.
 *
 * @note
 * This call prepares the statement (internal call to OCI_Prepare()) and ask
 * the Oracle server to parse its SQL or PL/SQL command.
 * OCI_Execute() can be call after OCI_ParseFmt() in order to execute the
 * statement, which means that the server will re-parse again the command.
 *
 * @warning
 * Do not use OCI_ParseFmt() unless you're only interested in the parsing result
 * because the statement will be parsed again when executed and thus leading to
 * unnecessary server round-trips and less performance
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_ParseFmt
(
    OCI_Statement *stmt,
    const otext *  sql,
    ...
);

/**
 * @brief
 * Describe the select list of a formatted SQL select statement.
 *
 * @param stmt - Statement handle
 * @param sql  - SQL statement
 * @param ...  - List of program values to format the SQL
 *
 * @note
 * This call sends the SELECT SQL order to the server for retrieving the
 * description of the select order only.
 * The command is not executed.
 * This call is only useful to retrieve information on the associated resultset
 * Call OCI_GetResultset() after OCI_Describe() to access to SELECT list
 * information
 *
 * @note
 * This call prepares the statement (internal call to OCI_Prepare()) and ask
 * the Oracle server to describe the output SELECT list.
 * OCI_Execute() can be call after OCI_Describe() in order to execute the
 * statement, which means that the server will parse, and describe again the SQL
 * order.
 *
 * @warning
 * Do not use OCI_Describe() unless you're only interested in the resultset
 * information because the statement will be parsed again when executed and thus
 * leading to unnecessary server round-trips and less performance
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_SYM_PUBLIC boolean OCI_DescribeFmt
(
    OCI_Statement *stmt,
    const otext *  sql,
    ...
);

/**
 * @} OcilibCApiFormatting
 */

/**
 * @defgroup OcilibCApiHashTables Hash tables
 * @{
 *
 * OCILIB uses hash tables internally for index/name columns mapping.
 *
 * OCILIB makes public its hash tables implementation public for general purpose
 * uses.
 *
 * OCI_HashTable objects manage string keys / values that can be :
 *
 * - integers
 * - strings
 * - pointers
 *
 * This hash table implementation :
 *
 * - handle collisions
 * - allows multiple values per key
 *
 * @par Internal conception
 *
 * - The hash table is composed of an array of slots.
 * - Each slot can hold a linked list of entries (one per key)
 * - Each entry can hold a linked list of values
 *
 * @note
 * - The internal hash function computes the index in the array where the entry
 *   has to be inserted/looked up.
 *
 *
 * @note
 * Collisions are handled by chaining method.
 *
 * @include hash.c
 *
 */

/**
 * @brief
 * Create a hash table
 *
 * @param size     - size of the hash table
 * @param type     - type of the hash table
 *
 * @note
 * Parameter can be one of the following values :
 *
 * - OCI_HASH_STRING  : string values
 * - OCI_HASH_INTEGER : integer values
 * - OCI_HASH_POINTER : pointer values
 *
 * @return
 * Hash handle on success or NULL on failure
 *
 */

OCI_SYM_PUBLIC OCI_HashTable * OCI_API OCI_HashCreate
(
    unsigned int size,
    unsigned int type
);

/**
 * @brief
 * Destroy a hash table
 *
 * @param table - Table handle
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_HashFree
(
    OCI_HashTable *table
);

/**
 * @brief
 * Return the size of the hash table
 *
 * @param table  - Table handle
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_HashGetSize
(
    OCI_HashTable *table
);

/**
 * @brief
 * Return the type of the hash table
 *
 * @param table  - Table handle
 *
 * @note
 * the return value can be one of the following values :
 *
 * - OCI_HASH_STRING  : string values
 * - OCI_HASH_INTEGER : integer values
 * - OCI_HASH_POINTER : pointer values
 *
 * @return
 * Hash table data type or OCI_UNKNOWN the input handle is NULL
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_HashGetType
(
    OCI_HashTable *table
);

/**
 * @brief
 * Add a pair string key / string value to the hash table
 *
 * @param table  - Table handle
 * @param key    - String key
 * @param value  - string value
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_HashAddString
(
    OCI_HashTable *table,
    const otext *  key,
    const otext *  value
);

/**
 * @brief
 * Return the string value associated to the given key
 *
 * @param table  - Table handle
 * @param key    - String key
 *
 * @return
 * Stored string associated with the key otherwise NULL
 *
 */

OCI_SYM_PUBLIC const otext * OCI_API OCI_HashGetString
(
    OCI_HashTable *table,
    const otext *  key
);

/**
 * @brief
 * Adds a pair string key / integer value to the hash table
 *
 * @param table  - Table handle
 * @param key    - String key
 * @param value  - Integer value
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_HashAddInt
(
    OCI_HashTable *table,
    const otext *  key,
    int            value
);

/**
 * @brief
 * Return the integer value associated to the given key
 *
 * @param table  - Table handle
 * @param key    - String key
 *
 * @return
 * Stored integer associated with the key otherwise 0
 *
 */

OCI_SYM_PUBLIC int OCI_API OCI_HashGetInt
(
    OCI_HashTable *table,
    const otext *  key
);

/**
 * @brief
 * Adds a pair string key / pointer value to the hash table
 *
 * @param table  - Table handle
 * @param key    - String key
 * @param value  - Pointer value
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_HashAddPointer
(
    OCI_HashTable *table,
    const otext *  key,
    void *         value
);

/**
 * @brief
 * Return a pointer associated with the given key
 *
 * @param table  - Table handle
 * @param key    - String key
 *
 * @return
 * Stored pointer associated with the key otherwise NULL
 *
 */

OCI_SYM_PUBLIC void * OCI_API OCI_HashGetPointer
(
    OCI_HashTable *table,
    const otext *  key
);

/**
 * @brief
 * Lookup for an entry matching the key in the table
 *
 * @param table  - Table handle
 * @param key    - String key
 * @param create - Do create the entry if not exists
 *
 * @return
 * Entry handle if key found/added otherwise NULL
 *
 */

OCI_SYM_PUBLIC OCI_HashEntry * OCI_API OCI_HashLookup
(
    OCI_HashTable *table,
    const otext *  key,
    boolean        create
);

/**
 * @brief
 * Return the first hash slot that matches the key
 *
 * @param table - Table handle
 * @param key   - String key
 *
 * @return
 * Slot handle if key found otherwise NULL
 *
 */

OCI_SYM_PUBLIC OCI_HashValue * OCI_API OCI_HashGetValue
(
    OCI_HashTable *table,
    const otext *  key
);

/**
 * @brief
 * Return the entry slot of the hash table internal list at the given position
 *
 * @param table - Table handle
 * @param index - index
 *
 * @warning
 * Index start at at
 *
 * @return
 * Slot handle otherwise NULL
 *
 */

OCI_SYM_PUBLIC OCI_HashEntry * OCI_API OCI_HashGetEntry
(
    OCI_HashTable *table,
    unsigned int   index
);

/**
 * @} OcilibCApiHashTables
 */

/**
 * @defgroup OcilibCApiThreading Threads and mutexes
 * @{
 *
 * Oracle proposes a portable implementation of Mutex and Thread objects
 *
 * OCILIB implements these OCI features for portable multi-threading support.
 *
 * Mutexes are designed for mutual exclusion between thread in order to lock resources temporarily
 *
 * Thread keys can be seen as process-wide variables that have a thread-specific
 * values. It allows to create a unique key identified by a name (string) that
 * can store values specific to each thread.
 *
 * OCILIB exposes the types OCI_Mutex, OCI_Thread
 *
 * @warning
 * OCILIB MUST be initialized with OCI_ENV_THREADED to enable threads support
 *
 * @warning
 * OCI_Thread relies on Oracle API which uses natives threading capabilities of
 * the supported platform
 *
 * @warning
 * Using OCI_Mutex :
 * - On Microsoft Windows, a thread can call OCI_MutexAcquire() more than once
 *   without any blocking. Just be sure that there is an OCI_MutexRelease() for
 *   every OCI_MutexAcquire() call
 * - On Unix systems, a thread MUST call OCI_MutexRelease() after every call to
 *   OCI_MutexAcquire() in order to be able to call OCI_MutexAcquire() again. If
 *   not, it will be blocked...
 *
 * @par Example
 * @include thread.c
 *
 */

/**
 * @brief
 * Create a Mutex object
 *
 * @return
 * Mutex handle on success or NULL on failure
 *
 */

OCI_SYM_PUBLIC OCI_Mutex * OCI_API OCI_MutexCreate
(
    void
);

/**
 * @brief
 * Destroy a mutex object
 *
 * @param mutex - Mutex handle
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_MutexFree
(
    OCI_Mutex *mutex
);

/**
 * @brief
 * Acquire a mutex lock
 *
 * @param mutex - Mutex handle
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_MutexAcquire
(
    OCI_Mutex *mutex
);

/**
 * @brief
 * Release a mutex lock
 *
 * @param mutex - Mutex handle
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_MutexRelease
(
    OCI_Mutex *mutex
);

/**
 * @brief
 * Create a Thread object
 *
 * @return
 * Thread handle on success or NULL on failure
 *
 */

OCI_SYM_PUBLIC OCI_Thread * OCI_API OCI_ThreadCreate
(
    void
);

/**
 * @brief
 * Destroy a thread object
 *
 * @param thread - Thread handle
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_ThreadFree
(
    OCI_Thread *thread
);

/**
 * @brief
 * Execute the given routine within the given thread object
 *
 * @param thread - Thread handle
 * @param proc   - routine to execute
 * @param arg    - parameter to pass to the routine
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_ThreadRun
(
    OCI_Thread *thread,
    POCI_THREAD proc,
    void *      arg
);

/**
 * @brief
 * Join the given thread
 *
 * @param thread - Thread handle
 *
 * @note
 * This function waits for the given thread to finish
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_ThreadJoin
(
    OCI_Thread *thread
);

/**
 * @brief
 * Create a thread key object
 *
 * @param name     - Thread key name
 * @param destfunc - Thread key value destructor function
 *
 * @note
 * Parameter proc is optional. It's called when the thread terminates to allow
 * the program to deal with the thread specific value of the key
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_ThreadKeyCreate
(
    const otext *      name,
    POCI_THREADKEYDEST destfunc
);

/**
 * @brief
 * Set a thread key value
 *
 * @param name  - Thread key name
 * @param value - user value to set
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_ThreadKeySetValue
(
    const otext *name,
    void *       value
);

/**
 * @brief
 * Get a thread key value
 *
 * @param name - Thread key name
 *
 * @return
 * Thread key value on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC void * OCI_API OCI_ThreadKeyGetValue
(
    const otext *name
);

/**
 * @} OcilibCApiThreading
 */

/**
 * @defgroup OcilibCApiDirectPath Direct Path loading
 * @{
 *
 * OCILIB (from version 3.2.0) support the OCI direct Path API.
 *
 * Actual implementation of direct path API does not support the following
 * elements :
 * - Objects data types (User Defined Types and Object References)
 * - Object tables
 * - Nested tables
 * - SQL String functions
 *
 * All scalar data types (numerics, characters and date/time), including LOBs
 * and LONG types are supported
 *
 * @par Oracle direct API features (from Oracle Documentation)
 *
 * The direct path load interface allows an application to access the direct path
 * load engine of the Oracle database server to perform the functions of the
 * Oracle SQL*Loader utility.
 * This functionality provides the ability to load data from external files
 * into Oracle database objects, either a table or a partition of a partitioned
 * table.
 * The OCI direct path load interface has the ability to load multiple rows by
 * loading a direct path stream which contains data for multiple rows.
 *
 * @par Oracle direct API limitation (from Oracle Documentation)
 * The direct path load interface has the following limitations which are the
 * same as SQL*Loader:
 *   - triggers are not supported
 *   - check constraints are not supported
 *   - referential integrity constraints are not supported
 *   - clustered tables are not supported
 *   - loading of remote objects is not supported
 *   - user-defined types are not supported
 *   - LOBs must be specified after all scalar columns
 *   - LONGs must be specified last
 *
 * @warning
 *
 * Its recommended to use direct path interface with an Oracle client that is
 * the same version than the database. With version < 10g, it is mandatory
 * regarding that it causes segmentation faults and it's known from Oracle that
 * advices to use the same version for client and server (see metalink KB)
 *
 * @par How to use direct path
 *
 * - 1 : Create a direct path handle with OCI_DirPathCreate()
 * - 2 : Set (optional) some direct path load attributes
 * - 3 : Describe the columns to load with OCI_DirPathSetColumn()
 * - 4 : Populate data with OCI_DirPathSetEntry()
 * - 5 : Convert the data with OCI_DirPathConvert()
 * - 6 : Load the data into the database with OCI_DirPathLoad()
 * - 7 : Repeat step 4,5,6 + reset the stream with OCI_DirPathReset() until all
 *       rows has been loaded
 * - 8 : Commit the load with OCI_DirPathFinish()
 * - 9 : Free the direct path handle with OCI_DirPathFree()
 *
 * @par Example
 * @include dirpath.c
 *
 */

/**
 * @brief
 * Create a direct path object
 *
 * @param typinf    - Table type info handle
 * @param partition - Partition name
 * @param nb_cols   - Number of columns to load
 * @param nb_rows   - Maximum of rows to handle per load operation
 *
 * @note
 * Retrieve the table type info handle with OCI_TypeInfoGet().
 * The partition name is not mandatory
 *
 * @note
 * Parameter 'nb_rows' is ignored for Oracle 8i. Prior to Oracle 9i, it's the
 * OCI client that decides of the number of rows to process per convert/load calls.
 * From Oracle 9i, OCI allows application to specify this value. Note that, the
 * OCI client might not accept the input value. After OCI_DirPathPrepare() has
 * been successfully called, OCI_DirPathGetMaxRows() returns the final number
 * of rows used for the given direct path operation.
 *
 * @return
 * Return the direct path handle on success otherwise NULL on failure
 *
 */

OCI_SYM_PUBLIC OCI_DirPath * OCI_API OCI_DirPathCreate
(
    OCI_TypeInfo *typinf,
    const otext * partition,
    unsigned int  nb_cols,
    unsigned int  nb_rows
);

/**
 * @brief
 * Free an OCI_DirPath handle
 *
 * @param dp - Direct path Handle
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */
OCI_SYM_PUBLIC boolean OCI_API OCI_DirPathFree
(
    OCI_DirPath *dp
);

/**
 * @brief
 * Describe a column to load into the given table
 *
 * @param dp      - Direct path Handle
 * @param index   - Column index
 * @param name    - Column name
 * @param maxsize - Maximum input value size for a column entry
 * @param format  - Date or numeric format to use
 *
 * @note
 * An error is thrown if :
 * - If the column specified by the 'name' parameter is not found in the table
 *   referenced by the type info handle passed to OCI_DirPathCreate()
 * - the index is out of bounds (= 0 or >= number of columns)
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_DirPathSetColumn
(
    OCI_DirPath *dp,
    unsigned int index,
    const otext *name,
    unsigned int maxsize,
    const otext *format
);

/**
 * @brief
 * Prepares the OCI direct path load interface before any rows can be converted
 * or loaded
 *
 * @param dp - Direct path Handle
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_DirPathPrepare
(
    OCI_DirPath *dp
);

/**
 * @brief
 * Set the value of the given row/column array entry
 *
 * @param dp        - Direct path Handle
 * @param row       - Row index
 * @param index     - Column index
 * @param value     - Value to set
 * @param size      - Size of the input value
 * @param complete  - Is the entry content fully provided ?
 *
 * @note
 * Rows and columns indexes start at 1.
 *
 * @note
 * The 'size' parameter is expressed in number of :
 * - bytes for binary columns
 * - characters for other columns
 *
 * @note
 * Direct path support piece loading for LONGs and LOBs columns. When filling
 * these columns, it's possible to provide input buffer piece by piece. In order
 * to do so :
 * - set the 'complete' parameter to FALSE
 * - set the 'size' parameter to the piece size
 * - Repeat calls to OCI_DirPathSetEntry() until the data is totally provided
 * - The last call that set the last piece or an entry must specify the value
 *   TRUE for the 'complete' parameter
 *
 * @warning
 * Current Direct Path OCILIB implementation DOES NOT support setting entry
 * content piece by piece as mentioned above. It was planned in the original design
 * but not supported yet. So, always set the complete parameter to TRUE.
 * Setting entries content piece by piece may be supported in future releases
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_DirPathSetEntry
(
    OCI_DirPath *dp,
    unsigned int row,
    unsigned int index,
    void *       value,
    unsigned     size,
    boolean      complete
);

/**
 * @brief
 * Convert provided user data to the direct path stream format
 *
 * @param dp - Direct path Handle
 *
 * @return
 * Possible return values :
 * - OCI_DPR_COMPLETE : load has been successful
 * - OCI_DPR_ERROR    : an error happened while loading data
 * - OCI_DPR_FULL     : the internal stream is full
 * - OCI_DPR_PARTIAL  : a column has not been fully filled yet
 * - OCI_DPR_EMPTY    : no data was found to convert
 *
 * @note
 * - When using conversion mode OCI_DCM_DEFAULT, OCI_DirPathConvert() stops when
 *   any error is encountered and returns OCI_DPR_ERROR
 * - When using conversion mode OCI_DCM_FORCE, OCI_DirPathConvert() does not stop
 *   on errors. Instead it discards any erred rows and returns OCI_DPR_COMPLETE once
 *   all rows are processed.
 *
 * @note
 * List of faulted rows and columns can be retrieved using OCI_DirPathGetErrorRow() and
 * OCI_DirPathGetErrorColumn()
 *
 * @note
 * OCI_DirPathGetAffectedRows() returns the number of rows converted in the last call.
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_DirPathConvert
(
    OCI_DirPath *dp
);

/**
 * @brief
 * Loads the data converted to direct path stream format
 *
 * @param dp - Direct path Handle
 *
 * @return
 * Possible return values :
 * - OCI_DPR_COMPLETE : conversion has been successful
 * - OCI_DPR_ERROR    : an error happened while converting data
 * - OCI_DPR_FULL     : the internal stream is full
 * - OCI_DPR_PARTIAL  : a column has not been fully filled yet
 * - OCI_DPR_EMPTY    : no data was found to load
 *
 * @note
 * List of faulted rows can be retrieved using OCI_DirPathGetErrorRow()
 *
 * @note
 * OCI_DirPathGetAffectedRows() returns the number of rows successfully loaded in the last call.
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_DirPathLoad
(
    OCI_DirPath *dp
);

/**
 * @brief
 * Reset internal arrays and streams to prepare another load
 *
 * @param dp - Direct path Handle
 *
 * @note
 * Once some data have been converted or loaded, OCI_DirPathReset() resets
 * internal OCI structures in order to prepare another load operation
 * (set entries, convert and load)
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_DirPathReset
(
    OCI_DirPath *dp
);

/**
 * @brief
 * Terminate a direct path operation and commit changes into the database
 *
 * @param dp - Direct path Handle
 *
 * @warning
 * The direct path handle cannot be used anymore after this call for any more
 * loading operations and must be freed with OCI_DirPathFree().
 *
 * @note
 * Some properties functions of the direct path handle, such as
 * OCI_DirPathGetRowCount() can be called on a terminated direct path handle
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_DirPathFinish
(
    OCI_DirPath *dp
);

/**
 * @brief
 * Terminate a direct path operation without committing changes
 *
 * @param dp - Direct path Handle
 *
 * @note
 * Any pending loaded data are canceled.
 * Any load completion operations, such as index maintenance operations, are not performed.
 *
 * @warning
 * The direct path handle cannot be used anymore after this call for any more
 * loading operations and must be freed with OCI_DirPathFree().
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_DirPathAbort
(
    OCI_DirPath *dp
);

/**
 * @brief
 * Execute a data save-point (server side)
 *
 * @param dp - Direct path Handle
 *
 * @note
 * Executing a data save-point is not allowed for LOBs
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_DirPathSave
(
    OCI_DirPath *dp
);

/**
 * @brief
 * Flushes a partially loaded row from server
 *
 * @param dp - Direct path Handle
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_DirPathFlushRow
(
    OCI_DirPath *dp
);

/**
 * @brief
 * Set the current number of rows to convert and load
 *
 * @param dp      - Direct path Handle
 * @param nb_rows - Number of row to process
 *
 * @warning
 * An OCILIB error will be thrown if the value exceeds the maximum number of
 * rows in the internals arrays
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_DirPathSetCurrentRows
(
    OCI_DirPath *dp,
    unsigned int nb_rows
);

/**
 * @brief
 * Return the current number of rows used in the OCILIB internal
 * arrays of rows
 *
 * @param dp - Direct path Handle
 *
 * @return
 * Internal current array size on SUCCESS otherwise 0
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_DirPathGetCurrentRows
(
    OCI_DirPath *dp
);

/**
 * @brief
 * Return the maximum number of rows allocated in the OCI and OCILIB
 * internal arrays of rows
 *
 * @param dp - Direct path Handle
 *
 * @return
 * Internal maximum array size on SUCCESS otherwise 0
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_DirPathGetMaxRows
(
    OCI_DirPath *dp
);

/**
 * @brief
 * Set the default date format string for input conversion
 *
 * @param dp     - Direct path Handle
 * @param format - date format
 *
 * @note
 * For string to date conversion, Oracle uses :
 * - Column date format
 * - Default date format (modified by this call)
 * - Default global support environment setting
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_DirPathSetDateFormat
(
    OCI_DirPath *dp,
    const otext *format
);

/**
 * @brief
 * Set the parallel loading mode
 *
 * @param dp    - Direct path Handle
 * @param value - enable/disable parallel mode
 *
 * @note
 * Default value is FALSE.
 *
 * @note
 * Setting the value to TRUE allows multiple load sessions to load the same
 * segment concurrently
 *
 * @par Parallel loading mode (From Oracle documentation)
 *
 * A direct load operation requires that the object being loaded is locked to
 * prevent DML on the object.
 * Note that queries are lock-free and are allowed while the object is being loaded.
 * - For a table load, if the option is set to:
 *   - FALSE, then the table DML X-Lock is acquired.
 *   - TRUE, then the table DML S-Lock is acquired.
 * - For a partition load, if the option is set to:
 *   - FALSE, then the table DML SX-Lock and partition DML X-Lock is acquired.
 *   - TRUE, then the table DML SS-Lock and partition DML S-Lock is acquired.
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_DirPathSetParallel
(
    OCI_DirPath *dp,
    boolean      value
);

/**
 * @brief
 * Set the logging mode for the loading operation
 *
 * @param dp    - Direct path Handle
 * @param value - enable/disable logging
 *
 * @par Logging mode (from Oracle Documentation)
 *
 * The NOLOG attribute of each segment determines whether image redo or
 * invalidation redo is generated:
 * - FALSE : Use the attribute of the segment being loaded.
 * - TRUE  : No logging. Overrides DDL statement, if necessary.
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_DirPathSetNoLog
(
    OCI_DirPath *dp,
    boolean      value
);

/**
 * @brief
 * Set number of elements in the date cache
 *
 * @param dp   - Direct path Handle
 * @param size - Buffer size
 *
 * @note
 * Default value is 0.
 *
 * @note
 * Setting the value to 0 disables the cache
 *
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_DirPathSetCacheSize
(
    OCI_DirPath *dp,
    unsigned int size
);

/**
 * @brief
 * Set the size of the internal stream transfer buffer
 *
 * @param dp   - Direct path Handle
 * @param size - Buffer size
 *
 * @note
 * Default value is 64KB.
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_DirPathSetBufferSize
(
    OCI_DirPath *dp,
    unsigned int size
);

/**
 * @brief
 * Set the direct path conversion mode
 *
 * @param dp   - Direct path Handle
 * @param mode - Conversion mode
 *
 * @note
 * Possible values for parameter 'mode' :
 *   - OCI_DCM_DEFAULT : conversion fails on error
 *   - OCI_DCM_FORCE   : conversion does not fail on error
 *
 * @note
 * See OCI_DirPathConvert() for conversion mode details
 *
 * @note
 * Default value is OCI_DCM_DEFAULT
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_DirPathSetConvertMode
(
    OCI_DirPath *dp,
    unsigned int mode
);

/**
 * @brief
 * Return the number of rows successfully loaded into the database so far
 *
 * @param dp - Direct path Handle
 *
 * @note
 * Insertions are committed with OCI_DirPathFinish()
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_DirPathGetRowCount
(
    OCI_DirPath *dp
);

/**
 * @brief
 * return the number of rows successfully processed during in the last
 * conversion or loading call
 *
 * @param dp - Direct path Handle
 *
 * @note
 * This function called after :
 *
 * - OCI_DirPathConvert(), returns the number of converted rows
 * - OCI_DirPathLoad(), returns the number of loaded rows
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_DirPathGetAffectedRows
(
    OCI_DirPath *dp
);

/**
 * @brief
 * Return the index of a column which caused an error during data conversion
 *
 * @param dp - Direct path Handle
 *
 * @warning
 * Direct path column indexes start at 1.
 *
 * @note
 * Errors may happen while data is converted to direct path stream format
 * using OCI_DirPathConvert().
 * When using conversion mode OCI_DCM_DEFAULT, OCI_DirPathConvert() returns
 * OCI_DPR_ERROR on error. OCI_DirPathGetErrorColumn() returns the column index
 * that caused the error
 * When using conversion mode OCI_DCM_FORCE, OCI_DirPathConvert() returns
 * OCI_DPR_COMPLETE even on errors. In order to retrieve the list of all column
 * indexes that have erred, the application can call OCI_DirPathGetErrorColumn()
 * repeatedly until it returns 0.
 *
 * @note
 * The internal value is reset to 0 when calling OCI_DirPathConvert()
 *
 * @return
 * 0 is no error occurs otherwise the index of the given column which caused an
 * error
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_DirPathGetErrorColumn
(
    OCI_DirPath *dp
);

/**
 * @brief
 * Return the index of a row which caused an error during data conversion
 *
 * @param dp - Direct path Handle
 *
 * @warning
 * Direct path row indexes start at 1.
 *
 * @note
 * Errors may happen :
 * - while data is converted to direct path stream format using OCI_DirPathConvert()
 * - while data is loaded to database using OCI_DirPathLoad()
 *
 * @note
 * When using conversion mode OCI_DCM_DEFAULT, OCI_DirPathConvert() returns
 * OCI_DPR_ERROR on error. OCI_DirPathGetErrorRow() returns the row index that
 * caused the error.
 * When using conversion mode OCI_DCM_FORCE, OCI_DirPathConvert() returns
 * OCI_DPR_COMPLETE even on errors. In order to retrieve the list of all row
 * indexes that have erred, the application can call OCI_DirPathGetErrorRow()
 * repeatedly until it returns 0.
 *
 * @note
 * After a call to OCI_DirPathLoad(), in order to retrieve the list of all faulted rows
 * indexes, the application can call OCI_DirPathGetErrorRow() repeatedly until it returns 0.
 *
 * @note
 * The internal value is reset to 0 when calling OCI_DirPathConvert(),
 * OCI_DirPathReset() or OCI_DirPathLoad()
 *
 * @return
 * 0 is no error occurs otherwise the index of the given row which caused an
 * error
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_DirPathGetErrorRow
(
    OCI_DirPath *dp
);

/**
 * @} OcilibCApiDirectPath
 */

/**
 * @defgroup OcilibCApiAdvancedQueuing Oracle Advanced Queuing (A/Q)
 * @{
 *
 * OCILIB supports Oracle Advanced Queues features
 *
 * Let's Oracle talk about this features !
 *
 * @par Oracle Queues (from Oracle Streams - Advanced Queuing User's Guide)
 *
 * Oracle Streams AQ provides database-integrated message queuing functionality.
 * It is built on top of Oracle Streams and leverages the functions of Oracle
 * Database so that messages can be stored persistently, propagated between
 * queues on different computers and databases, and transmitted using Oracle
 * Net Services and HTTP(S).
 * Because Oracle Streams AQ is implemented in database tables, all operational
 * benefits of high availability, scalability, and reliability are also
 * applicable to queue data. Standard database features such as recovery,
 * restart, and security are supported by Oracle Streams AQ. You can use
 * database development and management tools such as Oracle Enterprise Manager
 * to monitor queues. Like other database tables, queue tables can be imported
 * and exported.
 *
 * @par OCILIB implementation
 *
 * OCILIB provides a (nearly) full C implementation of Advanced Queues available in
 * Oracle OCI and proposes the following data types :
 *  - OCI_Msg     : Implementation of message to enqueue/dequeue from/to queues
 *  - OCI_Enqueue : Implementation of enqueuing process
 *  - OCI_Dequeue : Implementation of dequeuing process
 *  - OCI_Agent   : Implementation of Advanced queues Agents
 *
 * OCILIB support AQ messages notification with Oracle Client 10gR2 or above
 *
 * Note that the only AQ features not supported yet by OCILIB are :
 *   - Payloads of type AnyData
 *   - Enqueuing/dequeuing arrays of messages
 *   - Optional delivery mode introduced in 10gR2
 *
 * OCILIB provides as well a C API to administrate queues and queue tables initially
 * reserved to PL/SQL and Java (wrappers around PL/SQL calls).
 * This API, based on internal PL/SQL calls wrapping the DBMS_AQADM packages procedures, allow the
 * following actions :
 *  - create, alter, drop and purge queue tables (OCI_QueueTableXXX calls)
 *  - create, alter, drop, start, stop queues (OCI_QueueXXX calls)
 *
 * Note that the user connected to the database needs particular privileges to manipulate or
 * administrate queues (See Oracle Streams - Advanced Queuing User's Guide for more information
 * on these privileges)
 *
 * @par Example
 * @include queue.c
 *
 */

/**
 * @brief
 * Create a message object based on the given payload type
 *
 * @param typinf - Type info handle
 *
 * @note
 * OCILIB supports 2 type of message payload :
 * - Oracle types (UDT)
 * - RAW data
 *
 * @note
 * Oracle Type AnyData is not supported in the current version of OCILIB
 *
 * @note
 * the parameter 'typinf' indicates the type of payload :
 * - For object payload, retrieve the object type information handle with
 *   OCI_TypeInfoGet() using the object type name
 * - For RAW payload, you MUST pass the object type information retrieved with
 *   OCI_TypeInfoGet() using "SYS.RAW" as object type name
 *
 * @warning
 * Newly created Message handles have NULL payloads.
 * For Message handling Objects payloads, OCI_MsgGetObject() returns NULL until an object handle is
 * assigned to the message.
 *
 * @note
 * When a local OCI_Msg handle is enqueued, it keeps its attributes. If it's enqueued again, another
 * identical message is posted into the queue.
 * To reset a message and empty all its properties, call OCI_MsgReset()
 * Note that OCI_MsgReset() clears the message payload.
 *
 * @return
 * Return the message handle on success otherwise NULL on failure
 *
 */

OCI_SYM_PUBLIC OCI_Msg * OCI_API OCI_MsgCreate
(
    OCI_TypeInfo *typinf
);

/**
 * @brief
 * Free a message object
 *
 * @param msg - Message handle
 *
 * @warning
 * Only message handles created with OCI_MsgCreate() should be freed by OCI_MsgFree()
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_MsgFree
(
    OCI_Msg *msg
);

/**
 * @brief
 * Reset all attributes of a message object
 *
 * @param msg - Message handle
 *
 * @note
 * This function calls OCI_MsgSetxxx() with default or NULL attributes
 *
 * @warning
 * OCI_MsgReset() clears the message payload and set it to NULL
 * For messages handling objects payloads, OCI_MsgSetObject() must be called again to assign a
 * payload.
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_MsgReset
(
    OCI_Msg *msg
);

/**
 * @brief
 * Get the object payload of the given message
 *
 * @param msg - Message handle
 *
 * @return
 * Return the object handle on success otherwise NULL on failure or if payload is NULL
 *
 */

OCI_SYM_PUBLIC OCI_Object * OCI_API OCI_MsgGetObject
(
    OCI_Msg *msg
);

/**
 * @brief
 * Set the object payload of the given message
 *
 * @param msg - Message handle
 * @param obj - Object handle
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_MsgSetObject
(
    OCI_Msg *   msg,
    OCI_Object *obj
);

/**
 * @brief
 * Get the RAW payload of the given message
 *
 * @param msg  - Message handle
 * @param raw  - Input buffer
 * @param size - Input buffer maximum size
 *
 * @note
 * On output, parameter 'size' holds the number of bytes copied into the given buffer
 *
 * @return
 * TRUE on success otherwise FALSE on failure or if payload is object based.
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_MsgGetRaw
(
    OCI_Msg *     msg,
    void *        raw,
    unsigned int *size
);

/**
 * @brief
 * Set the RAW payload of the given message
 *
 * @param msg  - Message handle
 * @param raw  - Raw data
 * @param size - Raw data size
 *
 * @return
 * TRUE on success otherwise FALSE on failure or if payload is object based.
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_MsgSetRaw
(
    OCI_Msg *    msg,
    const void * raw,
    unsigned int size
);

/**
 * @brief
 * Return the number of attempts that have been made to dequeue the message
 *
 * @param msg - Message handle
 *
 */

OCI_SYM_PUBLIC int OCI_API OCI_MsgGetAttemptCount
(
    OCI_Msg *msg
);

/**
 * @brief
 * Return the number of seconds that a message is delayed for dequeuing
 *
 * @param msg - Message handle
 *
 * @note
 * see OCI_MsgSetEnqueueDelay() for more details
 *
 */

OCI_SYM_PUBLIC int OCI_API OCI_MsgGetEnqueueDelay
(
    OCI_Msg *msg
);

/**
 * @brief
 * set the number of seconds to delay the enqueued message
 *
 * @param msg   - Message handle
 * @param value - Delay in seconds
 *
 * @note
 * The delay represents the number of seconds after which a message is available for dequeuing.
 * When the message is enqueued, its state is set to OCI_AMS_WAITING.
 * When the delay expires, its state is set to OCI_AMS_READY.
 *
 * @note
 * If parameter 'value' is set to zero (default value), the message will be immediately available
 * for dequeuing
 *
 * @warning
 * Dequeuing by Message ID overrides the delay specification.
 *
 * @warning
 * Delaying processing requires the queue monitor to be started.
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_MsgSetEnqueueDelay
(
    OCI_Msg *msg,
    int      value
);

/**
 * @brief
 * return the time the message was enqueued
 *
 * @param msg - Message handle
 *
 * @note
 * Only use this function for message dequeued from queues
 *
 */

OCI_SYM_PUBLIC OCI_Date * OCI_API OCI_MsgGetEnqueueTime
(
    OCI_Msg *msg
);

/**
 * @brief
 * Return the duration that the message is available for dequeuing
 *
 * @param msg - Message handle
 *
 * @note
 * see OCI_MsgSetExpiration() for more details
 *
 */

OCI_SYM_PUBLIC int OCI_API OCI_MsgGetExpiration
(
    OCI_Msg *msg
);

/**
 * @brief
 * set the duration that the message is available for dequeuing
 *
 * @param msg   - Message handle
 * @param value - duration in seconds
 *
 * @note
 * This parameter is an offset from the delay (see OCI_MsgSetEnqueueDelay())
 * While waiting for expiration, the message state is set to OCI_AMS_READY.
 * If the message is not dequeued before it expires, it will be moved to the exception queue
 * with the state OCI_AMS_EXPIRED.
 *
 * @note
 * If parameter 'value' is set to -1 (default value), the message will not expire
 *
 * @warning
 * Expiration processing requires the queue monitor to be started.
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_MsgSetExpiration
(
    OCI_Msg *msg,
    int      value
);

/**
 * @brief
 * Return the state of the message at the time of the dequeue
 *
 * @param msg - Message handle
 *
 * @return
 *  - OCI_UNKNOWN       :  the function has failed to get the message state
 *  - OCI_AMS_READY     :  the message is ready to be processed
 *  - OCI_AMS_WAITING   :  the message delay has not yet completed
 *  - OCI_AMS_PROCESSED :  the message has been processed
 *  - OCI_AMS_EXPIRED   :  the message has moved to exception queue
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_MsgGetState
(
    OCI_Msg *msg
);

/**
 * @brief
 * Return the priority of the message
 *
 * @param msg - Message handle
 *
 * @note
 * see OCI_MsgSetPriority() for more details
 *
 */

OCI_SYM_PUBLIC int OCI_API OCI_MsgGetPriority
(
    OCI_Msg *msg
);

/**
 * @brief
 * Set the priority of the message
 *
 * @param msg   - Message handle
 * @param value - Message priority
 *
 * @note
 *   - The priority can be any number, including negative numbers.
 *   - A smaller number indicates higher priority.
 *   - Default value is zero.
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_MsgSetPriority
(
    OCI_Msg *msg,
    int      value
);

/**
 * @brief
 * Return the ID of the message
 *
 * @param msg - Message handle
 * @param id  - Input buffer
 * @param len - Input buffer maximum size
 *
 * @note
 * The message ID is :
 *  - generated when the message is enqueued in the queue
 *  - retrieved when the message is dequeued from the queue
 *
 * @note
 * On output, parameter 'len' holds the number of bytes copied into the given buffer
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_MsgGetID
(
    OCI_Msg *     msg,
    void *        id,
    unsigned int *len
);

/**
 * @brief
 * Return the original ID of the message in the last queue that generated this message
 *
 * @param msg - Message handle
 * @param id  - Input buffer
 * @param len - Input buffer maximum size
 *
 * @warning
 * When a message is propagated from/to different queues, this ID is the one generated for the
 * message in the previous queue.
 *
 * @note
 * On output, parameter 'len' holds the number of bytes copied into the given buffer
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_MsgGetOriginalID
(
    OCI_Msg *     msg,
    void *        id,
    unsigned int *len
);

/**
 * @brief
 * Set the original ID of the message in the last queue that generated this message
 *
 * @param msg  - Message handle
 * @param id   - Message ID
 * @param len  - Message ID size
 *
 * @warning
 * When a message is propagated from/to different queues, this ID is the one generated for the
 * message in the previous queue.
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_MsgSetOriginalID
(
    OCI_Msg *    msg,
    const void * id,
    unsigned int len
);

/**
 * @brief
 * Return the original sender of a message
 *
 * @param msg    - Message handle
 *
 * @return
 * Sender Handle (OCI_Agent *) on success (if set at enqueue time) otherwise NULL
 *
 */

OCI_SYM_PUBLIC OCI_Agent * OCI_API OCI_MsgGetSender
(
    OCI_Msg *msg
);

/**
 * @brief
 * Set the original sender of a message
 *
 * @param msg    - Message handle
 * @param sender - Message sender
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_MsgSetSender
(
    OCI_Msg *  msg,
    OCI_Agent *sender
);

/**
 * @brief
 * Set the recipient list of a message to enqueue
 *
 * @param msg       - Message handle
 * @param consumers - Recipients list (array of agent handles)
 * @param count     - Number of recipients
 *
 * @warning
 * This function should only be used for queues which allow multiple consumers.
 * The default recipients are the queue subscribers.
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_MsgSetConsumers
(
    OCI_Msg *    msg,
    OCI_Agent ** consumers,
    unsigned int count
);

/**
 * @brief
 * Get the correlation identifier of the message
 *
 * @param msg - Message handle
 *
 * @note
 * see OCI_MsgSetCorrelation() for more details
 *
 */

OCI_SYM_PUBLIC const otext * OCI_API OCI_MsgGetCorrelation
(
    OCI_Msg *msg
);

/**
 * @brief
 * set the correlation identifier of the message
 *
 * @param msg         - Message handle
 * @param correlation - Message correlation text
 *
 * @note
 * see OCI_DequeueSetCorrelation()  for more details
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_MsgSetCorrelation
(
    OCI_Msg *    msg,
    const otext *correlation
);

/**
 * @brief
 * Get the Exception queue name of the message
 *
 * @param msg - Message handle
 *
 * @warning
 * When calling this function on a message retrieved with OCI_DequeueGet(), the returned value is
 * NULL if the default exception queue associated with the current queue is used (e.g. no user
 * defined specified at enqueue time for the message)
 *
 * @note
 * see OCI_MsgSetExceptionQueue() for more details
 *
 */
OCI_SYM_PUBLIC const otext * OCI_API OCI_MsgGetExceptionQueue
(
    OCI_Msg *msg
);

/**
 * @brief
 * Set the name of the queue to which the message is moved to if it cannot be
 * processed successfully
 *
 * @param msg   - Message handle
 * @param queue - Exception queue name
 *
 * @warning
 * From Oracle Documentation :
 *
 * "Messages are moved into exception queues in two cases :
 *  - If the number of unsuccessful dequeue attempts has exceeded the attribute 'max_retries' of
 *    given queue
 *  - if the message has expired.
 *
 * All messages in the exception queue are in the EXPIRED state.
 *
 * The default is the exception queue associated with the queue table.
 *
 * If the exception queue specified does not exist at the time of the move the message will be
 * moved to the default exception queue associated with the queue table and a warning will be
 * logged in the alert file.
 *
 * This attribute must refer to a valid queue name."
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_MsgSetExceptionQueue
(
    OCI_Msg *    msg,
    const otext *queue
);

/**
 * @brief
 * Create a Enqueue object for the given queue
 *
 * @param typinf - Type info handle
 * @param name   - Queue name
 *
 * @note
 * OCILIB supports 2 type of message payload :
 * - Oracle types (UDT)
 * - RAW data
 *
 * @note
 * Oracle Type AnyData is not supported in the current version of OCILIB
 *
 * @note
 * the parameter 'typinf' indicates the type of payload to enqueue to the given queue :
 * - For object payload, retrieve the object type information handle with
 *   OCI_TypeInfoGet() using the object type name
 * - For RAW payload, you MUST pass the object type information retrieved with
 *   OCI_TypeInfoGet() using "SYS.RAW" as object type name
 *
 * @return
 * Return the Enqueue handle on success otherwise NULL on failure
 *
 */

OCI_SYM_PUBLIC OCI_Enqueue * OCI_API OCI_EnqueueCreate
(
    OCI_TypeInfo *typinf,
    const otext * name
);

/**
 * @brief
 * Free a Enqueue object
 *
 * @param enqueue - Enqueue handle
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_EnqueueFree
(
    OCI_Enqueue *enqueue
);

/**
 * @brief
 * Enqueue a message on the queue associated to the Enqueue object
 *
 * @param enqueue - Enqueue handle
 * @param msg     - Message handle to enqueue
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_EnqueuePut
(
    OCI_Enqueue *enqueue,
    OCI_Msg *    msg
);

/**
* @brief
* Set the enqueuing sequence of messages to put in the queue
*
* @param enqueue  - Enqueue handle
* @param sequence - enqueuing sequence
*
* @note
* Possible values for parameter 'sequence' :
*   - OCI_ASD_BEFORE : enqueue message before another message
*   - OCI_ASD_TOP    : enqueue message before all messages
*
* @note
* Default value is OCI_ASD_TOP
*
* @note
* if the parameter 'sequence' is set to OCI_ASD_BEFORE, the application must
* call OCI_EnqueueSetRelativeMsgID() before enqueuing the next message in the queue.
*
* @note
* In order to stop enqueuing message using a sequence deviation, call
* OCI_EnqueueSetSequenceDeviation() with the value OCI_ASD_TOP
*
* @return
* TRUE on success otherwise FALSE
*
*/

OCI_SYM_PUBLIC boolean OCI_API OCI_EnqueueSetSequenceDeviation
(
    OCI_Enqueue *enqueue,
    unsigned int sequence
);

/**
 * @brief
 * Return the sequence deviation of messages to enqueue to the queue
 *
 * @param enqueue - Enqueue handle
 *
 * @note
 * see OCI_EnqueueSetSequenceDeviation() for more details
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_EnqueueGetSequenceDeviation
(
    OCI_Enqueue *enqueue
);

/**
 * @brief
 * Set whether the new message is enqueued as part of the current transaction
 *
 * @param enqueue    - Enqueue handle
 * @param visibility - Enqueuing visibility
 *
 * @note
 * Possible values for parameter 'visibility' :
 *   - OCI_AMV_IMMEDIATE : enqueue is an independent transaction
 *   - OCI_AMV_ON_COMMIT : enqueue is part of current transaction
 *
 * @note
 * Default value is OCI_AMV_ON_COMMIT
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_EnqueueSetVisibility
(
    OCI_Enqueue *enqueue,
    unsigned int visibility
);

/**
 * @brief
 * Get the enqueuing/locking behavior
 *
 * @param enqueue - Enqueue handle
 *
 * @note
 * see OCI_EnqueueSetVisibility() for more details
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_EnqueueGetVisibility
(
    OCI_Enqueue *enqueue
);

/**
 * @brief
 * Set a message identifier to use for enqueuing messages using a sequence deviation
 *
 * @param enqueue - Enqueue handle
 * @param id      - message identifier
 * @param len     - pointer to message identifier length
 *
 * @note
 * This call is only valid if OCI_EnqueueSetSequenceDeviation() has been called
 * with the value  OCI_ASD_BEFORE
 *
 * @warning
 * if the function cannot assign the message id, the content of the parameter 'len' is set to zero.
 *
 * @note
 * see OCI_EnqueueSetSequenceDeviation() for more details
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_EnqueueSetRelativeMsgID
(
    OCI_Enqueue *enqueue,
    const void * id,
    unsigned int len
);

/**
 * @brief
 * Get the current associated message identifier used for enqueuing messages
 * using a sequence deviation
 *
 * @param enqueue - Enqueue handle
 * @param id      - buffer to receive the message identifier
 * @param len     - pointer to buffer max length
 *
 * @warning
 * When the function returns, parameter 'len' hold the number of bytes assigned to parameter 'id'
 *
 * @note
 * see OCI_EnqueueGetRelativeMsgID() for more details
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_EnqueueGetRelativeMsgID
(
    OCI_Enqueue * enqueue,
    void *        id,
    unsigned int *len
);

/**
 * @brief
 * Create a Dequeue object for the given queue
 *
 * @param typinf - Type info handle
 * @param name   - Queue name
 *
 * @note
 * OCILIB supports 2 type of message payload :
 * - Oracle types (UDT)
 * - RAW data
 *
 * @note
 * Oracle Type AnyData is not supported in the current version of OCILIB
 *
 * @note
 * the parameter 'typinf' indicates the type of payload to dequeue from the given queue :
 * - For object payload, retrieve the object type information handle with
 *   OCI_TypeInfoGet() using the object type name
 * - For RAW payload, you MUST pass the object type information retrieved with
 *   OCI_TypeInfoGet() using "SYS.RAW" as object type name
 *
 * @return
 * Return the Dequeue handle on success otherwise NULL on failure
 *
 */

OCI_SYM_PUBLIC OCI_Dequeue * OCI_API OCI_DequeueCreate
(
    OCI_TypeInfo *typinf,
    const otext * name
);

/**
 * @brief
 * Free a Dequeue object
 *
 * @param dequeue - Dequeue handle
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_DequeueFree
(
    OCI_Dequeue *dequeue
);

/**
 * @brief
 * Dequeue messages from the given queue
 *
 * @param dequeue - Dequeue handle
 *
 * @warning
 * The returned message is handled by the dequeue object.
 * Do not release it with OCI_MsgFree()
 *
 * @warning
 * When dequeuing from a multiple consumer queue, you need
 * to set the navigation mode to OCI_ADN_FIRST_MSG using
 * OCI_DequeueSetNavigation()
 *
 * @return
 * Message handle on success otherwise NULL on failure or on timeout
 *
 */

OCI_SYM_PUBLIC OCI_Msg * OCI_API OCI_DequeueGet
(
    OCI_Dequeue *dequeue
);

/**
 * @brief
 * Subscribe for asynchronous messages notifications
 *
 * @param dequeue  - Dequeue handle
 * @param port     - Port to use for notifications
 * @param timeout  - notification timeout
 * @param callback - User handler callback fired when messages are ready to be dequeued
 *
 * @note
 * OCI_ENV_EVENTS flag must be passed to OCI_Initialize() to be able to use
 * asynchronous messages notifications
 *
 * @note
 * Requires Oracle Client 10gR2 or above
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_DequeueSubscribe
(
    OCI_Dequeue *  dequeue,
    unsigned int   port,
    unsigned int   timeout,
    POCI_NOTIFY_AQ callback
);

/**
 * @brief
 * Unsubscribe for asynchronous messages notifications
 *
 * @param dequeue - Dequeue handle
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_DequeueUnsubscribe
(
    OCI_Dequeue *dequeue
);

/**
 * @brief
 * Set the current consumer name to retrieve message for.
 *
 * @param dequeue  - Dequeue handle
 * @param consumer - consumer name
 *
 * @warning
 * If a queue is not set up for multiple consumers, OCI_DequeueSetConsumer()
 * should not be called or called with parameter 'consumer' set to NULL
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_DequeueSetConsumer
(
    OCI_Dequeue *dequeue,
    const otext *consumer
);

/**
 * @brief
 * Get the current consumer name associated with the dequeuing process.
 *
 * @param dequeue - Dequeue handle
 *
 * @note
 * see OCI_DequeueSetConsumer() for more details
 *
 */

OCI_SYM_PUBLIC const otext * OCI_API OCI_DequeueGetConsumer
(
    OCI_Dequeue *dequeue
);

/**
 * @brief
 * set the correlation identifier of the message to be dequeued
 *
 * @param dequeue - Dequeue handle
 * @param pattern - correlation identifier
 *
 * @note
 * Special pattern matching characters, such as "%" or "_" can be used.
 * If more than one message satisfies the pattern, the order of dequeuing is undetermined.
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_DequeueSetCorrelation
(
    OCI_Dequeue *dequeue,
    const otext *pattern
);

/**
 * @brief
 * Get the correlation identifier of the message to be dequeued
 *
 * @param dequeue - Dequeue handle
 *
 * @note
 * see OCI_DequeueSetCorrelation() for more details
 *
 */

OCI_SYM_PUBLIC const otext * OCI_API OCI_DequeueGetCorrelation
(
    OCI_Dequeue *dequeue
);

/**
 * @brief
 * Set the message identifier of the message to be dequeued
 *
 * @param dequeue - Dequeue handle
 * @param id      - message identifier
 * @param len     - size of the message identifier
 *
 * @warning
 * if the function cannot assign the message id, the content of the parameter 'len' is set to zero.
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_DequeueSetRelativeMsgID
(
    OCI_Dequeue *dequeue,
    const void * id,
    unsigned int len
);

/**
 * @brief
 * Get the message identifier of the message to be dequeued
 *
 * @param dequeue - Dequeue handle
 * @param id      - message identifier
 * @param len     - size of the message identifier
 *
 * @warning
 * When the function returns, parameter 'len' hold the number of bytes assigned to parameter 'id'
 *
 * @note
 * see OCI_DequeueSetRelativeMsgID() for more details
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_DequeueGetRelativeMsgID
(
    OCI_Dequeue * dequeue,
    void *        id,
    unsigned int *len
);

/**
 * @brief
 * Set whether the new message is dequeued as part of the current transaction
 *
 * @param dequeue    - Dequeue handle
 * @param visibility - dequeuing mode
 *
 * @warning
 * The visibility parameter is ignored when using the OCI_ADM_BROWSE dequeuing
 * mode (see OCI_DequeueSetMode())
 *
 * @note
 * Possible values for parameter 'mode' :
 *   - OCI_AMV_IMMEDIATE : dequeue is an independent transaction
 *   - OCI_AMV_ON_COMMIT : dequeue is part of current transaction
 *
 * @note
 * Default value is OCI_AMV_ON_COMMIT
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_DequeueSetVisibility
(
    OCI_Dequeue *dequeue,
    unsigned int visibility
);

/**
 * @brief
 * Get the dequeuing/locking behavior
 *
 * @param dequeue - Dequeue handle
 *
 * @note
 * see OCI_DequeueSetVisibility() for more details
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_DequeueGetVisibility
(
    OCI_Dequeue *dequeue
);

/**
 * @brief
 * Set the dequeuing/locking behavior
 *
 * @param dequeue - Dequeue handle
 * @param mode    - dequeuing mode
 *
 * @note
 * Possible values for parameter 'mode' :
 *   - OCI_ADM_BROWSE        : read message without acquiring a lock
 *   - OCI_ADM_LOCKED        : read and obtain write lock on message
 *   - OCI_ADM_REMOVE        : read the message and delete it
 *   - OCI_ADM_REMOVE_NODATA : confirm receipt of the message, but do not
 *                             deliver the actual message content
 *
 * @note
 * Default value is OCI_ADM_REMOVE
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_DequeueSetMode
(
    OCI_Dequeue *dequeue,
    unsigned int mode
);

/**
 * @brief
 * Get the dequeuing/locking behavior
 *
 * @param dequeue - Dequeue handle
 *
 * @note
 * see OCI_DequeueSetMode() for more details
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_DequeueGetMode
(
    OCI_Dequeue *dequeue
);

/**
 * @brief
 * Set the position of messages to be retrieved.
 *
 * @param dequeue  - Dequeue handle
 * @param position - navigation position
 *
 * @note
 * The dequeuing uses the following sequence :
 *   - find messages using the navigation position
 *   - apply search criteria (message correlation)
 *   - get message
 *
 * @note
 * Possible values for parameter 'position' :
 *   - OCI_ADN_FIRST_MSG        : retrieves the first message which is available
 *   - OCI_ADN_NEXT_MSG         : retrieves the next message which is available
 *   - OCI_ADN_NEXT_TRANSACTION : skips the remainder of the current transaction
 *                                group (if any) and retrieves the first message
 *                                of the next transaction group.
 *
 * @note
 * Default value is OCI_ADN_NEXT_MSG
 *
 * @warning
 * OCI_ADN_NEXT_TRANSACTION can only be used if message grouping is enabled for the given queue.
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_DequeueSetNavigation
(
    OCI_Dequeue *dequeue,
    unsigned int position
);

/**
 * @brief
 * Return the navigation position of messages to retrieve from the queue
 *
 * @param dequeue - Dequeue handle
 *
 * @note
 * see OCI_DequeueSetNavigation() for more details
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_DequeueGetNavigation
(
    OCI_Dequeue *dequeue
);

/**
 * @brief
 * set the time that OCIDequeueGet() waits for messages if no messages are
 * currently available
 *
 * @param dequeue - Dequeue handle
 * @param timeout - timeout in seconds
 *
 * @note
 * - Any positive values in seconds are valid.
 * - The value 0  is accepted and means OCIDequeueGet() does not wait for
 *   messages and returns immediately if no messages are available
 * - The value -1 is accepted and means OCIDequeueGet() waits for ever (until
 *   a message is available in the queue)
 *
 * @note
 * Default value is -1 (wait for ever)
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_DequeueSetWaitTime
(
    OCI_Dequeue *dequeue,
    int          timeout
);

/**
 * @brief
 * Return the time that OCIDequeueGet() waits for messages if no messages are currently available
 *
 * @param dequeue - Dequeue handle
 *
 * @note
 * see OCI_DequeueSetWaitTime() for more details
 *
 */

OCI_SYM_PUBLIC int OCI_API OCI_DequeueGetWaitTime
(
    OCI_Dequeue *dequeue
);

/**
 * @brief
 * Set the Agent list to listen to message for
 *
 * @param dequeue   - Dequeue handle
 * @param consumers - Agent handle array
 * @param count     - Number of agents the array
 *
 * @return
 * return TRUE on success otherwise FALSE
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_DequeueSetAgentList
(
    OCI_Dequeue *dequeue,
    OCI_Agent ** consumers,
    unsigned int count
);

/**
 * @brief
 * Listen for messages that match any recipient of the associated Agent list
 *
 * @param dequeue - Dequeue handle
 * @param timeout - Timeout in second
 *
 * @note
 * If an Agent handle is returned, messages are available for this agent.
 * In order to retrieve its messages :
 * - call OCI_DequeueSetConsumer() with the name of agent using OCI_AgentGetName()
 * - call OCI_DequeueGet() to dequeue it's pending messages
 *
 * @warning
 * The return value is valid only until:
 * - OCIDequeueListen() is called again
 * - OCI_DequeueFree() is called to free the Dequeue object
 * So Do not store the handle value across calls to OCIDequeueListen()
 *
 * @return
 * An Agent handle for who messages are available on success otherwise NULL
 */

OCI_SYM_PUBLIC OCI_Agent * OCI_API OCI_DequeueListen
(
    OCI_Dequeue *dequeue,
    int          timeout
);

/**
 * @brief
 * Create an AQ agent object
 *
 * @param con     - Connection handle
 * @param name    - Agent name
 * @param address - Agent address
 *
 * @note
 * An AQ agent object is :
 * - used as recipient information when enqueuing a message
 * - used as sender information when dequeuing a message
 * - used for listening message only from identified senders
 *
 * @note
 * the AQ agent address can be any Oracle identifier, up to 128 bytes.
 * the AQ agent name    can be any Oracle identifier, up to 30  bytes.
 *
 * @return
 * AQ agent handle on success otherwise NULL
 *
 */

OCI_SYM_PUBLIC OCI_Agent * OCI_API OCI_AgentCreate
(
    OCI_Connection *con,
    const otext *   name,
    const otext *   address
);

/**
 * @brief
 * Free an AQ agent object
 *
 * @param agent - AQ agent handle
 *
 * @warning
 * Only AQ agent handle created with OCI_AgentCreate() should be freed by OCI_AgentFree()
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_AgentFree
(
    OCI_Agent *agent
);

/**
 * @brief
 * Set the given AQ agent name
 *
 * @param agent - AQ agent handle
 * @param name  - AQ agent name
 *
 * @note
 * the AQ agent name is used to identified an message send or recipient when enqueuing/dequeuing
 * a message
 *
 * @note
 * the AQ agent name can be any Oracle identifier, up to 30 bytes.
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_AgentSetName
(
    OCI_Agent *  agent,
    const otext *name
);

/**
 * @brief
 * Get the given AQ agent name
 *
 * @param agent - AQ agent handle
 *
 * @return
 * AQ agent name on success otherwise NULL on failure
 *
 */

OCI_SYM_PUBLIC const otext * OCI_API OCI_AgentGetName
(
    OCI_Agent *agent
);

/**
 * @brief
 * Set the given AQ agent address
 *
 * @param agent   - AQ agent handle
 * @param address - AQ agent address
 *
 * @note
 * the parameter 'address' must be of the form : [schema.]queue_name[\@dblink]
 *
 * @note
 * the AQ agent address can be any Oracle identifier, up to 128 bytes.
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_AgentSetAddress
(
    OCI_Agent *  agent,
    const otext *address
);

/**
 * @brief
 * Get the given AQ agent address
 *
 * @param agent - AQ agent handle
 *
 * @note
 * See OCI_AgentSetAddress()
 *
 * @return
 * AQ agent address on success otherwise NULL on failure
 *
 */

OCI_SYM_PUBLIC const otext * OCI_API OCI_AgentGetAddress
(
    OCI_Agent *agent
);

/**
 * @brief
 * Create a queue
 *
 * @param con                   - Connection handle
 * @param queue_name            - Queue name
 * @param queue_table           - Queue table name
 * @param queue_type            - Queue type
 * @param max_retries           - Maximum number of attempts to dequeue a message
 * @param retry_delay           - Number of seconds between attempts to dequeue a message
 * @param retention_time        - number of seconds a message is retained in the queue table after
 *                                being dequeued from the queue
 * @param dependency_tracking   - Parameter reserved for future use by Oracle (MUST be set to FALSE)
 * @param comment               - Description of the queue
 *
 * @note
 * Parameter 'queue_name' can specify the schema where to create to queue ([schema.]queue_name)
 * Queue names cannot be longer than 24 characters (Oracle limit for user queues)
 *
 * @note
 * Possible values for parameter 'queue_type' :
 *  - OCI_AQT_NORMAL            : Normal queue
 *  - OCI_AQT_EXCEPTION         : Exception queue
 *  - OCI_AQT_NON_PERSISTENT    : Non persistent queue
 *
 * To set default values, pass :
 *  - queue_type     : OCI_AQT_NORMAL
 *  - max_retries    : 0
 *  - retry_delay    : 0
 *  - retention_time : 0
 *  - comment        : NULL
 *
 * @note
 * this call wraps the PL/SQL procedure DBMS_AQADM.CREATE_QUEUE().
 * Refer to Oracle Streams - Advanced Queuing User's Guide for more details
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_QueueCreate
(
    OCI_Connection *con,
    const otext *   queue_name,
    const otext *   queue_table,
    unsigned int    queue_type,
    unsigned int    max_retries,
    unsigned int    retry_delay,
    unsigned int    retention_time,
    boolean         dependency_tracking,
    const otext *   comment
);

/**
 * @brief
 * Alter the given queue
 *
 * @param con                   - Connection handle
 * @param queue_name            - Queue name
 * @param max_retries           - Maximum number of attempts to dequeue a message
 * @param retry_delay           - Number of seconds between attempts to dequeue a message
 * @param retention_time        - number of seconds a message is retained in the queue table after
 *                                being dequeued from the queue
 * @param comment               - Description of the queue
 *
 * @note
 * See OCI_QueueCreate() for more details
 *
 * @warning
 * This function updates all attributes handled in the parameter list !
 *
 * @note
 * this call wraps the PL/SQL procedure DBMS_AQADM.ALTER_QUEUE().
 * Refer to Oracle Streams - Advanced Queuing User's Guide for more details
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_QueueAlter
(
    OCI_Connection *con,
    const otext *   queue_name,
    unsigned int    max_retries,
    unsigned int    retry_delay,
    unsigned int    retention_time,
    const otext *   comment
);

/**
 * @brief
 * Drop the given queue
 *
 * @param con        - Connection handle
 * @param queue_name - Queue name
 *
 * @warning
 * A queue can be dropped only if it has been stopped before.
 *
 * @note
 * this call wraps the PL/SQL procedure DBMS_AQADM.DROP_QUEUE().
 * Refer to Oracle Streams - Advanced Queuing User's Guide for more details
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_QueueDrop
(
    OCI_Connection *con,
    const otext *   queue_name
);

/**
 * @brief
 * Start the given queue
 *
 * @param con        - Connection handle
 * @param queue_name - Queue name
 * @param enqueue    - Enable enqueue
 * @param dequeue    - Enable dequeue
 *
 * @warning
 * For exception queues, only enqueuing is allowed
 *
 * @note
 * this call wraps the PL/SQL procedure DBMS_AQADM.START_QUEUE().
 * Refer to Oracle Streams - Advanced Queuing User's Guide for more details
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_QueueStart
(
    OCI_Connection *con,
    const otext *   queue_name,
    boolean         enqueue,
    boolean         dequeue
);

/**
 * @brief
 * Stop enqueuing or dequeuing or both on the given queue
 *
 * @param con        - Connection handle
 * @param queue_name - Queue name
 * @param enqueue    - Disable enqueue
 * @param dequeue    - Disable dequeue
 * @param wait       - Wait for current pending enqueues/dequeues
 *
 * @warning
 * A queue cannot be stopped if there are pending transactions against the queue.
 *
 * @note
 * this call wraps the PL/SQL procedure DBMS_AQADM.STOP_QUEUE().
 * Refer to Oracle Streams - Advanced Queuing User's Guide for more details
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_QueueStop
(
    OCI_Connection *con,
    const otext *   queue_name,
    boolean         enqueue,
    boolean         dequeue,
    boolean         wait
);

/**
 * @brief
 * Create a queue table for messages of the given type
 *
 * @param con                   - Connection handle
 * @param queue_table           - Queue table name
 * @param queue_payload_type    - Message type name
 * @param storage_clause        - Additional clauses for the table storage
 * @param sort_list             - Additional columns name to use for sorting
 * @param multiple_consumers    - Enable multiple consumers for each messages
 * @param message_grouping      - Specifies if messages are grouped within a transaction
 * @param comment               - Description of the queue table
 * @param primary_instance      - primary owner (instance) of the queue table
 * @param secondary_instance    - Owner of the queue table if the primary instance is not available
 * @param compatible            - lowest database version with which the queue table is compatible
 *
 * @note
 * Parameter 'queue_table' can specify the schema where to create to queue table ([schema.]queue_table)
 * Queue table names cannot be longer than 24 characters (Oracle limit for user queue tables)
 *
 * @note
 * Possible values for parameter 'queue_payload_type' :
 * - For Oracle types (UDT) : use the type name ([schema.].type_name)
 * - For RAW data           : use "SYS.RAW" or "RAW" (depending on Oracle versions - For latest ones, use "RAW")
 *
 * @note
 * Possible values for parameter 'message_grouping' :
 *  - OCI_AGM_NONE            : each message is treated individually
 *  - OCI_AGM_TRANSACTIONNAL  : all messages enqueued in one transaction are considered part of
 *                              the same group and can be dequeued as a group of related messages.
 *
 * @note
 * Possible values for parameter 'compatible' :
 * - "8.0", "8.1", "10.0"
 *
 * To set default values, pass :
 *  - storage_clause    : NULL
 *  - sort_list         : NULL
 *  - message_grouping  : OCI_AGM_NONE
 *  - comment           : NULL
 *  - primary_instance  : 0
 *  - primary_instance  : 0
 *  - compatible        : NULL
 *
 * @note
 * this call wraps the PL/SQL procedure DBMS_AQADM.CREATE_QUEUE_TABLE().
 * Refer to Oracle Streams - Advanced Queuing User's Guide for more details
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_QueueTableCreate
(
    OCI_Connection *con,
    const otext *   queue_table,
    const otext *   queue_payload_type,
    const otext *   storage_clause,
    const otext *   sort_list,
    boolean         multiple_consumers,
    unsigned int    message_grouping,
    const otext *   comment,
    unsigned int    primary_instance,
    unsigned int    secondary_instance,
    const otext *   compatible
);

/**
 * @brief
 * Alter the given queue table
 *
 * @param con                   - Connection handle
 * @param queue_table           - Queue table name
 * @param comment               - Description of the queue table
 * @param primary_instance      - primary owner (instance) of the queue table
 * @param secondary_instance    - Owner of the queue table if the primary instance is not available
 *
 * @note
 * See OCI_QueueTableCreate() from more details
 *
 * @note
 * this call wraps the PL/SQL procedure DBMS_AQADM.ALTER_QUEUE_TABLE().
 * Refer to Oracle Streams - Advanced Queuing User's Guide for more details
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_QueueTableAlter
(
    OCI_Connection *con,
    const otext *   queue_table,
    const otext *   comment,
    unsigned int    primary_instance,
    unsigned int    secondary_instance
);

/**
 * @brief
 * Drop the given queue table
 *
 * @param con         - Connection handle
 * @param queue_table - Queue table name
 * @param force       - Force the deletion of objects related to the queue table
 *
 * @note
 * Possible values for 'force' :
 *  - TRUE  : all queues using the queue table and their associated propagation schedules are
 *            dropped automatically
 *  - FALSE : All the queues using the given queue table must be stopped and dropped before the
 *            queue table can be dropped.
 *
 * @note
 * this call wraps the PL/SQL procedure DBMS_AQADM.DROP_QUEUE_TABLE().
 * Refer to Oracle Streams - Advanced Queuing User's Guide for more details
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_QueueTableDrop
(
    OCI_Connection *con,
    const otext *   queue_table,
    boolean         force
);

/**
 * @brief
 * Purge messages from the given queue table
 *
 * @param con             - Connection handle
 * @param queue_table     - Queue table name
 * @param purge_condition - Optional SQL based conditions (see notes)
 * @param block           - Lock all queues using the queue table while doing the purge
 * @param delivery_mode   - Type of message to purge
 *
 * @note
 * Possible values for parameter 'delivery_mode' :
 *  - OCI_APM_BUFFERED      : purge only buffered   messages
 *  - OCI_APM_PERSISTENT    : purge only persistent messages
 *  - OCI_APM_ALL           : purge all messages
 *
 * @note
 * For more information about the SQL purge conditions, refer to
 *  Oracle Streams - Advanced Queuing User's Guide for more details
 *
 * @warning
 * This feature is only available from Oracle 10gR2.
 * This function does nothing and returns TRUE is the server version is < Oracle 10gR2
 *
 * @note
 * this call wraps the PL/SQL procedure DBMS_AQADM.PURGE_QUEUE_TABLE().
 * Refer to Oracle Streams - Advanced Queuing User's Guide for more details
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_QueueTablePurge
(
    OCI_Connection *con,
    const otext *   queue_table,
    const otext *   purge_condition,
    boolean         block,
    unsigned int    delivery_mode
);

/**
 * @brief
 * Migrate a queue table from one version to another
 *
 * @param con             - Connection handle
 * @param queue_table     - Queue table name
 * @param compatible      - Database version with witch the queue table has to migrate
 *
 * @note
 * Possible values for parameter 'compatible' :
 * - "8.0", "8.1", "10.0"
 *
 * @note
 * this call wraps the PL/SQL procedure DBMS_AQADM.MIGRATE_QUEUE_TABLE().
 * Refer to Oracle Streams - Advanced Queuing User's Guide for more details
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_QueueTableMigrate
(
    OCI_Connection *con,
    const otext *   queue_table,
    const otext *   compatible
);

/**
 * @} OcilibCApiAdvancedQueuing
 */

/**
 * @defgroup OcilibCApiSubscriptions Database Change notifications (DCN or CQN)
 * @{
 *
 * OCILIB supports Oracle 10gR2 feature Database Change Notifications (DCN)
 * also named Continuous Query Notifications (CQN)
 *
 * This features allows a client application to register notifications
 * when some changes are made in a database :
 * - Database status changes : startup and shutdown
 * - Database objects changes :
 *  - DDL changes : alter or drop actions
 *  - DML changes : insert, delete, update actions
 *
 * This feature can be really useful in applications that hold
 * a cache of data. Instead of refreshing data periodically by
 * connecting to the server, the application could only refresh
 * modified data when necessary or perform specific tasks depending on
 * the events. It saves application time, network traffic and can help
 * the design of the application logic.
 *
 * The database status change notification is also interesting to be
 * informed of instance startup / shutdown
 *
 * Check Oracle documentation for more details about this feature
 *
 * @note
 * No active database connection is required to receive the
 * notifications as they are handled by the Oracle client using a
 * dedicated socket connection to the server
 *
 * @par Database changes
 *
 * The client application can be notified of any database status
 * change (single DB or multiple DB in a RAC environment).
 *
 * @par Object changes
 *
 * The notifications of object changes are based on the registration
 * of a query ('select' SQL statement).
 *
 * Oracle server will notify of any changes of any object that is
 * part of the statement result set.
 *
 * Registering a statement will notify about any changes on its
 * result set rows performed after the registration of the query.
 *
 * The query can be a simple 'select * from table' or a complex
 * query involving many tables and/or criteria in the where clause.
 *
 * For Object changes, the notification can be at :
 *  - At Object level : only the object name (schema + object) is given
 *  - At row level : same that object level + RowID of the altered row
 *
 * @warning
 * Trying to use this features with a client/server version < 10gR2 will raise an error
 *
 * @par Example
 * @include notification.c
 *
 */

/**
 * @brief
 * Register a notification against the given database
 *
 * @param con      - Connection handle
 * @param name     - Notification name
 * @param type     - Subscription type
 * @param handler  - User handler callback
 * @param port     - Port to use for notifications
 * @param timeout  - notification timeout
 *
 * @note
 * Parameter 'type' can be one of the following values :
 *
 * - OCI_CNT_OBJECTS   : request for changes at objects (e.g. tables) level (DDL / DML)
 * - OCI_CNT_ROWS      : request for changes at rows level (DML)
 * - OCI_CNT_DATABASES : request for changes at database level (startup, shutdown)
 * - OCI_CNT_ALL       : request for all changes
 *
 * @note
 * Parameter 'port' and 'timeout' are optional (use value 0 a optional value)
 * 
 * @note
 * OCI_ENV_EVENTS flag must be passed to OCI_Initialize() to be able to use
 * subscriptions
 *
 * @ @warning Port usage
 * All notifications are using the same port. 
 * Port number can be either:
 *   - determined automatically by Oracle client once the first subscription had been created and can be retrieved using OCI_SubscriptionGetPort()
 *   - Set by the parameter 'port' during the first call to OCI_SubscriptionRegister(). In this case later calls can provide same port number or 0
 * 
 * @note
 * Requires Oracle Client 10gR2 or above
 *
 * @note
 * Subscription handles are automatically managed by the library
 *
 * @return
 * Subscription handle on success or NULL on failure
 *
 */

OCI_SYM_PUBLIC OCI_Subscription * OCI_API OCI_SubscriptionRegister
(
    OCI_Connection *con,
    const otext *   name,
    unsigned int    type,
    POCI_NOTIFY     handler,
    unsigned int    port,
    unsigned int    timeout
);

/**
 * @brief
 * Unregister a previously registered notification
 *
 * @param sub - Subscription handle
 *
 * @note
 * OCI_Cleanup() will automatically unregister any registered subscriptions
 *
 * @note
 * If the database connection passed to OCI_SubscriptionRegister()
 * has been closed by the time that the application calls
 * OCI_SubscriptionUnregister, the library internally reconnects
 * to the given database, performs the unregistration and then disconnects
 *
 * @note
 * OCI_ENV_EVENTS flag must be passed to OCI_Initialize() to be able to use
 * subscriptions
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_SubscriptionUnregister
(
    OCI_Subscription *sub
);

/**
 * @brief
 * Add a statement to the notification to monitor
 *
 * @param sub  - Subscription handle
 * @param stmt - Statement handle
 *
 * @note
 * The given statement must be prepared but not executed before being passed to this function.
 * OCI_SubscriptionAddStatement() executes the statement and register it for notifications
 *
 * @note
 * OCI_ENV_EVENTS flag must be passed to OCI_Initialize() to be able to use
 * subscriptions
 *
 * @note
 * The given statement must hold a 'SELECT' SQL statement
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_SubscriptionAddStatement
(
    OCI_Subscription *sub,
    OCI_Statement *   stmt
);

/**
 * @brief
 * Return the name of the given registered subscription
 *
 * @param sub - Subscription handle
 *
 * @note
 * OCI_ENV_EVENTS flag must be passed to OCI_Initialize() to be able to use
 * subscriptions
 *
 */

OCI_SYM_PUBLIC const otext * OCI_API OCI_SubscriptionGetName
(
    OCI_Subscription *sub
);

/**
 * @brief
 * Return the port used by the notification
 *
 * @param sub - Subscription handle
 *
 * @note
 * OCI_ENV_EVENTS flag must be passed to OCI_Initialize() to be able to use
 * subscriptions
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_SubscriptionGetPort
(
    OCI_Subscription *sub
);

/**
 * @brief
 * Return the timeout of the given registered subscription
 *
 * @param sub - Subscription handle
 *
 * @note
 * OCI_ENV_EVENTS flag must be passed to OCI_Initialize() to be able to use
 * subscriptions
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_SubscriptionGetTimeout
(
    OCI_Subscription *sub
);

/**
 * @brief
 * Return the connection handle associated with a subscription handle
 *
 * @param sub - Subscription handle
 *
 * @note
 * It may return a NULL handle if the connection used in OCI_SubscriptionRegister has been closed.
 *
 */

OCI_SYM_PUBLIC OCI_Connection * OCI_API OCI_SubscriptionGetConnection
(
    OCI_Subscription *sub
);

/**
 * @brief
 * Return the type of event reported by a notification
 *
 * @param event - Event handle
 *
 * @note
 * The returned value can be one of the following values :
 *
 * - OCI_ENT_STARTUP        : a database has been started up
 * - OCI_ENT_SHUTDOWN       : a database has been shut down
 * - OCI_ENT_SHUTDOWN_ANY   : a database has been shut down (RAC)
 * - OCI_ENT_DROP_DATABASE  : a database has been dropped
 * - OCI_ENT_DEREGISTER     : the notification is timed out
 * - OCI_ENT_OBJECT_CHANGED : a database object has been modified
 *
 * @note
 * OCI_EventGetDatabase() returns the affected database
 *
 * @note
 * OCI_ENV_EVENTS flag must be passed to OCI_Initialize() to be able to use
 * subscriptions
 *
 * @note
 * OCI_EventGetObject() returns the affected object
 * ('schema_name'.'object_name')
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_EventGetType
(
    OCI_Event *event
);

/**
 * @brief
 * Return the type of operation reported by a notification
 *
 * @param event - Event handle
 *
 * @note
 * This call is only valid when OCI_EventGetType() reports the
 * event type OCI_ENT_OBJECT_CHANGED
 *
 * @note
 * The returned value can be one of the following values :
 *
 * - OCI_ONT_INSERT  : an insert has been performed
 * - OCI_ONT_UPDATE  : an update has been performed
 * - OCI_ONT_DELETE  : a  delete has been performed
 * - OCI_ONT_ALTER   : an alter  has been performed
 * - OCI_ONT_DROP    : a  drop   has been performed
 * - OCI_ONT_GENERIC : generic undefined action
 *
 * @note
 * OCI_EventGetDatabase() returns the affected database
 *
 * @note
 * OCI_EventGetObject() returns the affected object ('schema_name'.'object_name')
 *
 * @note
 * OCI_ENV_EVENTS flag must be passed to OCI_Initialize() to be able to use
 * subscriptions
 *
 * @note
 * if OCI_CNT_ROWS is passed to OCI_SubscriptionRegister(),
 * the rowid of the altered row can be retrieved with OCI_EventGetRowid()
 *
 */

OCI_SYM_PUBLIC unsigned int OCI_API OCI_EventGetOperation
(
    OCI_Event *event
);

/**
 * @brief
 * Return the name of the database that generated the event
 *
 * @param event - Event handle
 *
 * @note
 * OCI_ENV_EVENTS flag must be passed to OCI_Initialize() to be able to use
 * subscriptions
 *
 * @warning
 * When using Oracle pluggable databases (PDBs), OCI_EventGetDatabase() returns
 * the container database name and NOT the pluggable database name (Oracle limitation).
 *
 */

OCI_SYM_PUBLIC const otext * OCI_API OCI_EventGetDatabase
(
    OCI_Event *event
);

/**
 * @brief
 * Return the name of the object that generated the event
 *
 * @param event - Event handle
 *
 * @note
 * OCI_ENV_EVENTS flag must be passed to OCI_Initialize() to be able to use
 * subscriptions
 *
 */

OCI_SYM_PUBLIC const otext * OCI_API OCI_EventGetObject
(
    OCI_Event *event
);

/**
 * @brief
 * Return the rowid of the altered database object row
 *
 * @param event - Event handle
 *
 * @note
 * OCI_ENV_EVENTS flag must be passed to OCI_Initialize() to be able to use
 * subscriptions
 *
 */

OCI_SYM_PUBLIC const otext * OCI_API OCI_EventGetRowid
(
    OCI_Event *event
);

/**
 * @brief
 * Return the subscription handle that generated this event
 *
 * @param event - Event handle
 *
 * @note
 * OCI_ENV_EVENTS flag must be passed to OCI_Initialize() to be able to use
 * subscriptions
 *
 */

OCI_SYM_PUBLIC OCI_Subscription * OCI_API OCI_EventGetSubscription
(
    OCI_Event *event
);

/**
 * @} OcilibCApiSubscriptions
 */

/**
 * @defgroup OcilibCApiInstancesManagement Remote Instance startup/shutdown
 * @{
 *
 * OCILIB supports Oracle 11g client features for manipulating remote Oracle instances.
 *
 * Oracle instances (on the same computer or on a remote server) can be :
 *
 * - started with OCI_DatabaseStartup()
 * - shutdown with OCI_DatabaseShutdown()
 *
 * Several options are handled for this actions
 *
 * @par Example
 * @include instance.c
 *
 */

/**
 * @brief
 * Start a database instance
 *
 * @param db         - Oracle Service Name
 * @param user       - Oracle User name
 * @param pwd        - Oracle User password
 * @param sess_mode  - Session mode
 * @param start_mode - Start mode
 * @param start_flag - Start flags
 * @param spfile     - Client-side spfile to start up the database (optional)
 *
 * Possible values for parameter session mode :
 * - OCI_SESSION_SYSDBA
 * - OCI_SESSION_SYSOPER
 *
 * @note
 * External credentials are supported by supplying a null value for the 'user' and 'pwd' parameters
 * If the param 'db' is NULL then a connection to the default local DB is done
 *
 * Possible (combined) values for parameter start_mode :
 * - OCI_DB_SPM_START : start the instance
 * - OCI_DB_SPM_MOUNT : mount the instance
 * - OCI_DB_SPM_OPEN  : open the instance
 * - OCI_DB_SPM_FULL  : start, mount and open the instance
 *
 * Possible (combined) values for parameter start_flag :
 * - OCI_DB_SPF_DEFAULT  : default startup
 * - OCI_DB_SPF_FORCE    : shuts down a running instance (if needed) using
 *                         ABORT command and starts a new instance
 * - OCI_DB_SPF_RESTRICT : allows database access only to users with both
 *                         CREATE SESSION and RESTRICTED SESSION privileges
 *
 * @note
 * If the client side spfile is not provided, the database is started with its server-side spfile
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_DatabaseStartup
(
    const otext *db,
    const otext *user,
    const otext *pwd,
    unsigned int sess_mode,
    unsigned int start_mode,
    unsigned int start_flag,
    const otext *spfile
);

/**
 * @brief
 * Shutdown a database instance
 *
 * @param db         - Oracle Service Name
 * @param user       - Oracle User name
 * @param pwd        - Oracle User password
 * @param sess_mode  - Session mode
 * @param shut_mode  - Shutdown mode
 * @param shut_flag  - Shutdown flag
 *
 *
 * @warning
 * Possible values for parameter session mode :
 * - OCI_SESSION_SYSDBA
 * - OCI_SESSION_SYSOPER
 *
 * @note
 * External credentials are supported by supplying a null value for the 'user' and 'pwd' parameters
 * If the param 'db' is NULL then a connection to the default local DB is done
 *
 * Possible (combined) values for parameter shut_mode :
 * - OCI_DB_SDM_SHUTDOWN : shutdown the instance
 * - OCI_DB_SDM_CLOSE    : close the instance
 * - OCI_DB_SDM_DISMOUNT : dismount the instance
 * - OCI_DB_SDM_FULL     : shutdown, close and dismount the instance
 *
 * Possible (exclusive) value for parameter shut_flag (from Oracle documentation) :
 * - OCI_DB_SDF_DEFAULT     :
 *   - Further connects are prohibited.
 *   - Waits for users to disconnect from the database
 * - OCI_DB_SDF_TRANS       :
 *   - Further connects are prohibited
 *   - No new transactions are allowed.
 *   - Waits for active transactions to complete
 * - OCI_DB_SDF_TRANS_LOCAL :
 *   - Further connects are prohibited
 *   - No new transactions are allowed.
 *   - Waits only for local transactions to complete
 * - OCI_DB_SDF_IMMEDIATE   :
 *   - Does not wait for current calls to complete or users to disconnect from the database.
 *   - All uncommitted transactions are terminated and rolled back
 * - OCI_DB_SDF_ABORT       :
 *   - Does not wait for current calls to complete or users to disconnect from the database.
 *   - All uncommitted transactions are terminated and are not rolled back.
 *   - This is the fastest possible way to shut down the database, but the next
 *     database startup may require instance recovery.
 *   - Therefore, this option should be used only in unusual circumstances
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_SYM_PUBLIC boolean OCI_API OCI_DatabaseShutdown
(
    const otext *db,
    const otext *user,
    const otext *pwd,
    unsigned int sess_mode,
    unsigned int shut_mode,
    unsigned int shut_flag
);

/**
 * @} OcilibCApiInstancesManagement
 */

/**
 * @defgroup OcilibCApiRawHandles Using OCI Handles directly
 * @{
 *
 * OCILIB conception was focused on a full but closed encapsulation of OCI.
 *
 * All OCI headers, data types, prototypes are imported internally
 * (linkage or runtime import).
 *
 * OCILIB public interface exposes only ISO C scalar types and OCILIB objects
 *
 * OCI is a wide and rich API that can deals with hundreds of options !
 *
 * OCILIB tries to implements most of it. But, sometimes in really specific
 * contexts, it might be necessary to directly call OCI APIs in order to use
 * uncovered OCI functionalities or options
 *
 * OCILIB proposes now a set of functions to retrieve its internal OCI handles
 *
 * @warning
 *
 * The OCILIB author strongly <b>advises</b> against the use of internal handles,
 * unless there is no other way to accomplish the task
 *
 * @warning
 *
 * Using these handles for direct application calls to OCI might lead
 * to OCILIB instability or crash if handles are incorrectly used !
 *
 */

/**
 * @brief
 * Return the OCI Environment Handle (OCIEnv *) of OCILIB library
 *
 * @return
 * OCI Environment handle otherwise NULL
 *
 */

OCI_SYM_PUBLIC const void * OCI_API OCI_HandleGetEnvironment
(
    void
);

/**
 * @brief
 * Return the OCI Context Handle (OCISvcCtx *) of an OCILIB OCI_Connection object
 *
 * @param con - Connection handle
 *
 * @return
 * OCI Context handle otherwise NULL
 *
 */

OCI_SYM_PUBLIC const void * OCI_API OCI_HandleGetContext
(
    OCI_Connection *con
);

/**
 * @brief
 * Return the OCI Server Handle (OCIServer *) of an OCILIB OCI_Connection object
 *
 * @param con - Connection handle
 *
 * @return
 * OCI Server handle otherwise NULL
 *
 */

OCI_SYM_PUBLIC const void * OCI_API OCI_HandleGetServer
(
    OCI_Connection *con
);

/**
 * @brief
 * Return the OCI Error Handle (OCIError *) of an OCILIB OCI_Connection object
 *
 * @param con - Connection handle
 *
 * @return
 * OCI Error handle otherwise NULL
 *
 */

OCI_SYM_PUBLIC const void * OCI_API OCI_HandleGetError
(
    OCI_Connection *con
);

/**
 * @brief
 * Return the OCI Session Handle (OCISession *) of an OCILIB OCI_Connection object
 *
 * @param con - Connection handle
 *
 * @return
 * OCI Session handle otherwise NULL
 *
 */

OCI_SYM_PUBLIC const void * OCI_API OCI_HandleGetSession
(
    OCI_Connection *con
);

/**
 * @brief
 * Return the OCI Transaction Handle (OCITrans *) of an OCILIB OCI_Transaction object
 *
 * @param trans - Transaction handle
 *
 * @return
 * OCI Transaction handle otherwise NULL
 *
 */

OCI_SYM_PUBLIC const void * OCI_API OCI_HandleGetTransaction
(
    OCI_Transaction *trans
);

/**
 * @brief
 * Return the OCI Statement Handle (OCIStmt *) of an OCILIB OCI_Statement object
 *
 * @param stmt - Statement handle
 *
 * @return
 * OCI Statement handle otherwise NULL
 *
 */

OCI_SYM_PUBLIC const void * OCI_API OCI_HandleGetStatement
(
    OCI_Statement *stmt
);

/**
 * @brief
 * Return the OCI LobLocator Handle (OCILobLocator *) of an OCILIB OCI_Lob object
 *
 * @param lob - Lob handle
 *
 * @return
 * OCI LobLocator handle otherwise NULL
 *
 */

OCI_SYM_PUBLIC const void * OCI_API OCI_HandleGetLob
(
    OCI_Lob *lob
);

/**
 * @brief
 * Return the OCI LobLocator Handle (OCILobLocator *) of an OCILIB OCI_File object
 *
 * @param file - File handle
 *
 * @return
 * OCI LobLocator handle otherwise NULL
 *
 */

OCI_SYM_PUBLIC const void * OCI_API OCI_HandleGetFile
(
    OCI_File *file
);

/**
 * @brief
 * Return the OCI Date Handle (OCIDate *) of an OCILIB OCI_Date object
 *
 * @param date - Date handle
 *
 * @return
 * OCI Date handle otherwise NULL
 *
 */

OCI_SYM_PUBLIC const void * OCI_API OCI_HandleGetDate
(
    OCI_Date *date
);

/**
 * @brief
 * Return the OCI Date time Handle (OCIDatetime *) of an OCILIB OCI_Timestamp  object
 *
 * @param tmsp - Timestamp handle
 *
 * @return
 * OCI Date time handle otherwise NULL
 *
 */

OCI_SYM_PUBLIC const void * OCI_API OCI_HandleGetTimestamp
(
    OCI_Timestamp *tmsp
);

/**
 * @brief
 * Return OCI Interval Handle (OCIInterval *) of an OCILIB OCI_Interval object
 *
 * @param itv - Interval handle
 *
 * @return
 * OCI Interval handle otherwise NULL
 *
 */

OCI_SYM_PUBLIC const void * OCI_API OCI_HandleGetInterval
(
    OCI_Interval *itv
);

/**
 * @brief
 * Return OCI Object Handle (void *) of an OCILIB OCI_Object object
 *
 * @param obj - Object handle
 *
 * @return
 * OCI Object handle otherwise NULL
 *
 */

OCI_SYM_PUBLIC const void * OCI_API OCI_HandleGetObject
(
    OCI_Object *obj
);

/**
 * @brief
 * Return OCI Collection Handle (OCIColl *) of an OCILIB OCI_Coll object
 *
 * @param coll - Collection handle
 *
 * @return
 * OCI Collection handle otherwise NULL
 *
 */

OCI_SYM_PUBLIC const void * OCI_API OCI_HandleGetColl
(
    OCI_Coll *coll
);

/**
 * @brief
 * Return OCI Ref Handle (OCIRef *) of an OCILIB OCI_Ref object
 *
 * @param ref - Ref handle
 *
 * @return
 * OCI Ref handle otherwise NULL
 *
 */

OCI_SYM_PUBLIC const void * OCI_API OCI_HandleGetRef
(
    OCI_Ref *ref
);

/**
 * @brief
 * Return OCI Mutex handle (OCIThreadMutex *) of an OCILIB OCI_Mutex object
 *
 * @param mutex - Mutex handle
 *
 * @return
 * OCI Mutex handle otherwise NULL
 *
 */

OCI_SYM_PUBLIC const void * OCI_API OCI_HandleGetMutex
(
    OCI_Mutex *mutex
);

/**
 * @brief
 * Return OCI Thread ID (OCIThreadId *) of an OCILIB OCI_Thread object
 *
 * @param thread - Thread handle
 *
 * @return
 * OCI Thread ID otherwise NULL
 *
 */

OCI_SYM_PUBLIC const void * OCI_API OCI_HandleGetThreadID
(
    OCI_Thread *thread
);

/**
 * @brief
 * Return OCI Thread handle (OCIThreadHandle *) of an OCILIB OCI_Thread object
 *
 * @param thread - Thread handle
 *
 * @return
 * OCI Thread handle otherwise NULL
 *
 */

OCI_SYM_PUBLIC const void * OCI_API OCI_HandleGetThread
(
    OCI_Thread *thread
);

/**
 * @brief
 * Return OCI DirectPath Context handle (OCIDirPathCtx *) of an OCILIB OCI_DirPath object
 *
 * @param dp - DirectPath handle
 *
 * @return
 * OCI DirectPath Context handle otherwise NULL
 *
 */

OCI_SYM_PUBLIC const void * OCI_API OCI_HandleGetDirPathCtx
(
    OCI_DirPath *dp
);

/**
 * @brief
 * Return OCI DirectPath Column array handle (OCIDirPathColArray *) of an OCILIB OCI_DirPath object
 *
 * @param dp - DirectPath handle
 *
 * @return
 * OCI DirectPath Column array handle otherwise NULL
 *
 */

OCI_SYM_PUBLIC const void * OCI_API OCI_HandleGetDirPathColArray
(
    OCI_DirPath *dp
);

/**
 * @brief
 * Return OCI DirectPath Stream handle (OCIDirPathStream *) of an OCILIB OCI_DirPath object
 *
 * @param dp - DirectPath handle
 *
 * @return
 * OCI DirectPath Stream handle otherwise NULL
 *
 */

OCI_SYM_PUBLIC const void * OCI_API OCI_HandleGetDirPathStream
(
    OCI_DirPath *dp
);

/**
 * @brief
 * Return OCI Subscription handle (OCISubscription *) of an OCILIB OCI_Subscription object
 *
 * @param sub - Subscription handle
 *
 * @return
 * OCI Subscription otherwise NULL
 *
 */

OCI_SYM_PUBLIC const void * OCI_API OCI_HandleGetSubscription
(
    OCI_Subscription *sub
);

/**
 * @} OcilibCApiRawHandles
 */

/**
 * @defgroup OcilibCApiDemoApplication OCILIB main demo application code
 * @{
 *
 * Portable Main demo application header
 * @include ocilib_demo.h
 *
 * Portable Main demo application source
 * @include ocilib_demo.c
 *
 * @} OcilibCApiDemoApplication
 */

/**
* @} OcilibCApi
*/

#endif /* OCILIB_OCILIBC_API_H_INCLUDED */
