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

#ifndef OCILIB_OCILIBC_TYPES_H_INCLUDED
#define OCILIB_OCILIBC_TYPES_H_INCLUDED

#include "ocilibc/defines.h"

/**
 * @addtogroup OcilibCApi
 * @{
 *
 */

/**
 * @addtogroup OcilibCApiDatatypes
 * @{
 *
 * OCILIB implements:
 *
 * - Oracle Scalar data types through scalar C data types
 * - Oracle opaque/complex objects though opaque library handles
 * - Library objects for manipulating the database: connections, transactions, statements...
 *
 * @par Supported Oracle data types
 *
 * - All Database types are supported excluding REFs.
 *
 * Here is a summary of the supported data types:
 *
 * - Scalar types CHAR/NCHAR, VARCHAR2/NVARCHAR2, NUMBER, FLOAT, REAL, RAW, ...
 * - Binary types: RAW, LONG RAW, VARRAW, ..
 * - Larges Objects (Lobs and Files) : BLOB, CLOB, NCLOB, BFILE
 * - LONG types: LONG, VAR LONG
 * - Date, Timestamps et Intervals: DATE, TIMESTAMP, INTERVAL
 * - PL/SQL types: Ref cursors, PL/SQL Tables
 * - Named Types (by value): Built-in system objects and User defined objects
 * - VARRAYs and Nested Tables
 * - ROWIDs
 *
 * @par OCILIB library objects
 *
 * The public OCILIB library interface implements encapsulation for
 * representing database objects (such as connections, statements ...) through
 * opaque structures (pointers to structures whose definition is kept private)
 *
 * Instead of directly manipulating the structures and their members, the library
 * has functions to access the underlying members.
 *
 * It's designed to make the user code as more independent as possible of
 * the library details.
 *
**/

/**
 * @typedef OCI_Environment
 *
 * @brief
 * Environment object 
 *
 * Represents a runtime OCI executing environment
 *
 */

typedef struct OCI_Environment OCI_Environment;

/**
 * @typedef OCI_Pool
 *
 * @brief
 * Pool object (session or connection)
 *
 * A pool is a set of pooled objects
 *
 */

typedef struct OCI_Pool OCI_Pool;

/**
 * @typedef OCI_Connection
 *
 * @brief
 * Oracle physical connection.
 *
 * It holds all information about a connection such as error handling, associated statements, ...
 * Error handling and transactions are embedded within a connection object.
 *
 * @warning
 * Multi threaded applications that use multiple connections should use one connection per thread
 * as all statements associated with a connection share the same context.
 *
 */

typedef struct OCI_Connection OCI_Connection;

/**
 * @typedef OCI_Statement
 *
 * @brief
 * Oracle SQL or PL/SQL statement.
 *
 * A Statement object allows users to prepare, execute SQL orders or PL/SQL blocks
 *
 */

typedef struct OCI_Statement OCI_Statement;

/**
 * @typedef OCI_Bind
 *
 * @brief
 * Internal bind representation.
 *
 * A bind object is an object that holds all information about an Oracle statement binding operation
 *
 */

typedef struct OCI_Bind OCI_Bind;

/**
 * @typedef OCI_Resultset
 *
 * @brief
 * Collection of output columns from a select statement.
 *
 * A resultset object is the result of 'select' SQL Statement.
 *
 * It's a set of data (ordered in columns) that can be fetched row by row
 * to get data returned by the SQL statement
 *
 */

typedef struct OCI_Resultset OCI_Resultset;

/**
 * @typedef OCI_Column
 *
 * @brief
 * Oracle SQL Column and Type member representation.
 *
 * A column object represents an output column from a select statement
 *
 */

typedef struct OCI_Column OCI_Column;

/**
 * @typedef OCI_Lob
 *
 * @brief
 * Oracle Internal Large objects:
 *
 * The following internal Larges Objects are supported:
 *
 * - BLOBs           : Binary large objects
 * - CLOBs / NCLOBs  : Character large objects
 *
 * LOBs were introduced by OCI8 to replace Long data types.
 *
 * It's designed to store really larges objects (buffer, files) inside the database
 *
 * Oracle encourages programmers to use those objects instead of LONG, LONG RAW, ...
 *
 * OCILIB supports both LOBs and LONGs
 *
 */

typedef struct OCI_Lob OCI_Lob;

/**
 * @typedef OCI_File
 *
 * @brief
 * Oracle External Large objects:
 *
 * The following external Larges Objects are supported:
 *
 * - BFILEs : Binary files
 * - CFILEs : Character files
 *
 * FILEs were introduced by OCI8 in order to store references to files located outside the database.
 *
 * @warning
 * Only Read-only access is allowed on BFILEs
 *
 * Two way to use FILEs :
 *
 * - within statement context (query, binding)
 * - without statement context (server files reading) through OCI_File properties functions
 *
 */

typedef struct OCI_File OCI_File;

/**
 * @typedef OCI_Transaction
 *
 * @brief
 * Oracle Transaction.
 *
 * A transaction can be:
 *
 * - Local: it's implicitly created by OCILIB
 * - Global: it's explicitly created by the program
 *
 */

typedef struct OCI_Transaction OCI_Transaction;

/**
 * @typedef OCI_Long
 *
 * @brief Oracle Long data type.
 *
 * The following long Objects are supported:
 *
 * - LONG RAW : Binary long objects
 * - LONG     : Character long objects
 *
 * Those types were used in older versions of Oracle (before Oracle 8i) to store
 * large chunks of data in the database.
 *
 * It's now depreciated by Oracle that recommends using LOBs
 *
 * Many databases and applications are still designed to use LONGs that's why
 * OCILIB supports Long Objects and piecewise operations
 *
 */

typedef struct OCI_Long OCI_Long;

/**
* @typedef OCI_Number
*
* @brief
* Oracle NUMBER representation.
*
*/
typedef struct OCI_Number OCI_Number;

/**
 * @typedef OCI_Date
 *
 * @brief
 * Oracle internal date representation.
 *
 */

typedef struct OCI_Date OCI_Date;

/**
 * @typedef OCI_Timestamp
 *
 * @brief
 * Oracle internal timestamp representation.
 *
 */

typedef struct OCI_Timestamp OCI_Timestamp;

/**
 * @typedef OCI_Interval
 *
 * @brief
 * Oracle internal interval representation.
 *
 */

typedef struct OCI_Interval OCI_Interval;

/**
 * @typedef OCI_Object
 *
 * @brief
 * Oracle Named types representation.
 *
 */

typedef struct OCI_Object OCI_Object;

/**
 * @typedef OCI_Coll
 *
 * @brief
 * Oracle Collections (VARRAYs and Nested Tables) representation.
 *
 */

typedef struct OCI_Coll OCI_Coll;

/**
 * @typedef OCI_Elem
 *
 * @brief
 * Oracle Collection item representation.
 *
 */

typedef struct OCI_Elem OCI_Elem;

/**
 * @typedef OCI_Iter
 *
 * @brief
 * Oracle Collection iterator representation.
 *
 */
typedef struct OCI_Iter OCI_Iter;

/**
 * @typedef OCI_TypeInfo
 *
 * @brief
 * Type info metadata handle.
 *
 */

/**
 * @typedef OCI_Ref
 *
 * @brief
 * Oracle REF type representation.
 *
 */

typedef struct OCI_Ref OCI_Ref;

/**
 * @typedef OCI_TypeInfo
 *
 * @brief
 * Type info meta data handle.
 *
 */

typedef struct OCI_TypeInfo OCI_TypeInfo;

/**
 * @typedef OCI_HashTable
 *
 * @brief
 * OCILIB implementation of hash tables.
 *
 */

typedef struct OCI_HashTable OCI_HashTable;

/**
 * @typedef OCI_Error
 *
 * @brief
 * Encapsulates an Oracle or OCILIB exception.
 *
 * The error object is used to raise internal or oracle errors.
 * When an error occurs, if the application has provided an error handler, an
 * error object is constructed and passed to the handler
 *
 */

typedef struct OCI_Error OCI_Error;

/**
 * @typedef OCI_Mutex
 *
 * @brief
 * OCILIB encapsulation of OCI mutexes.
 *
 */

typedef struct OCI_Mutex OCI_Mutex;

/**
 * @typedef OCI_Thread
 *
 * @brief
 * OCILIB encapsulation of OCI Threads.
 *
 */

typedef struct OCI_Thread OCI_Thread;

/**
 * @typedef OCI_DirPath
 *
 * @brief
 * OCILIB encapsulation of OCI Direct Path handle.
 *
 */

typedef struct OCI_DirPath OCI_DirPath;

/**
 * @typedef OCI_Subscription
 *
 * @brief
 * OCILIB encapsulation of Oracle DCN notification
 *
 */

typedef struct OCI_Subscription OCI_Subscription;

/**
 * @typedef OCI_Event
 *
 * @brief
 * OCILIB encapsulation of Oracle DCN event
 *
 */

typedef struct OCI_Event OCI_Event;

/**
 * @typedef OCI_Msg
 *
 * @brief
 * OCILIB encapsulation of A/Q message
 *
 */

typedef struct OCI_Msg OCI_Msg;

/**
 * @typedef OCI_Agent
 *
 * @brief
 * OCILIB encapsulation of A/Q Agent
 *
 */

typedef struct OCI_Agent OCI_Agent;

/**
 * @typedef OCI_Dequeue
 *
 * @brief
 * OCILIB encapsulation of A/Q dequeuing operations
 *
 */

typedef struct OCI_Dequeue OCI_Dequeue;

/**
 * @typedef OCI_Enqueue
 *
 * @brief
 * OCILIB encapsulation of A/Q enqueuing operations
 *
 */

typedef struct OCI_Enqueue OCI_Enqueue;

/**
 * @var POCI_ERROR
 *
 * @brief
 * Error procedure prototype
 *
 * @param err - Error handle
 *
 */

typedef void (*POCI_ERROR)
(
    OCI_Error *err
);

/**
 * @var POCI_THREAD
 *
 * @brief
 * Thread procedure prototype
 *
 * @param thread - Thread handle
 * @param arg    - Pointer passed to OCI_ThreadRun()
 *
 */

typedef void (*POCI_THREAD)
(
    OCI_Thread *thread,
    void *      arg
);

/**
 * @var POCI_THREADKEYDEST
 *
 * @brief
 * Thread key destructor prototype.
 *
 * @param data - Thread Key current pointer value
 *
 */

typedef void (*POCI_THREADKEYDEST)
(
    void *data
);

/**
 * @var POCI_NOTIFY
 *
 * @brief
 * Database Change Notification User callback prototype.
 *
 * @param event - Event handle
 *
 */

typedef void (*POCI_NOTIFY)
(
    OCI_Event *event
);

/**
 * @var POCI_NOTIFY_AQ
 *
 * @brief
 * AQ notification callback prototype.
 *
 * @param dequeue - dequeue handle
 *
 */

typedef void (*POCI_NOTIFY_AQ)
(
    OCI_Dequeue *dequeue
);

/**
 * @var POCI_TAF_HANDLER
 *
 * @brief
 * Failover Notification User callback prototype.
 *
 * @param con   - Connection handle related to the event
 * @param type  - Event type
 * @param event - Event code
 *
 * @note
 * Possible values for parameter 'type' :
 *  - OCI_FOT_NONE
 *  - OCI_FOT_SESSION
 *  - OCI_FOT_SELECT
 *
 * @note
 * Possible values for parameter 'event' :
 *  - OCI_FOE_END
 *  - OCI_FOE_ABORT
 *  - OCI_FOE_REAUTH
 *  - OCI_FOE_BEGIN
 *  - OCI_FOE_ERROR
 *
 * @return
 * User callback should return one of the following value :
 *  - OCI_FOC_OK
 *  - OCI_FOC_RETRY
 *
 */

typedef unsigned int (*POCI_TAF_HANDLER)
(
    OCI_Connection *con,
    unsigned int    type,
    unsigned int    event
);

/**
 * @var POCI_HA_HANDLER
 *
 * @brief
 * HA (High Availability) events Notification User callback prototype.
 *
 * @param con    - Connection handle related to the event
 * @param source - source of the event
 * @param event  - type of the event
 * @param time   - Timestamp of the event
 *
 * @note
 * Currently, Oracle only send HA down events
 *
 * @note
 * Possible values for parameter 'source' :
 *  - OCI_HES_INSTANCE
 *  - OCI_HES_DATABASE
 *  - OCI_HES_NODE
 *  - OCI_HES_SERVICE
 *  - OCI_HES_SERVICE_MEMBER
 *  - OCI_HES_ASM_INSTANCE
 *  - OCI_HES_PRECONNECT
 *
 * @note
 * Possible values for parameter 'event' :
 *  - OCI_HET_DOWN : HA event type down
 *  - OCI_HET_UP   : HA event type up
 *
 */

typedef void (*POCI_HA_HANDLER)
(
    OCI_Connection *con,
    unsigned int    source,
    unsigned int    event,
    OCI_Timestamp * time
);

/* public structures */

/**
 * @typedef OCI_XID
 *
 * @brief
 * Global transaction identifier
 *
 */

typedef struct OCI_XID
{
    long formatID;
    long gtrid_length;
    long bqual_length;
    char data[128];
} OCI_XID;

/**
 * @typedef OCI_Variant
 *
 * @brief
 * Internal Variant type based on union C type.
 *
 * @note
 * Helpful for generic buffer, it reduces the amount of casts
 *
 */

typedef union OCI_Variant
{
    /* integers */
    int num;

    /* raw data */
    unsigned char *p_bytes;

    /* pointer to c natives types */
    void *  p_void;
    int *   p_int;
    float * p_float;
    double *p_double;
    otext * p_text;

    /* ocilib object types */
    OCI_Date *     p_date;
    OCI_Interval * p_interval;
    OCI_Timestamp *p_timestamp;
    OCI_Long *     p_long;
    OCI_Lob *      p_lob;
    OCI_File *     p_file;
    OCI_Statement *p_stmt;
    OCI_Column *   p_col;
    OCI_Object *   p_obj;
    OCI_Coll *     p_coll;
    OCI_Iter *     p_iter;
    OCI_Elem *     p_elem;
} OCI_Variant;

/**
* @typedef OCI_HashValue
*
* @brief
* Hash table entry value
*
* OCILIB implementation of hash tables uses chaining method for dealing with collisions
*
*/

typedef struct OCI_HashValue
{
    OCI_Variant           value;
    struct OCI_HashValue *next;
} OCI_HashValue;

/**
 * @typedef OCI_HashEntry
 *
 * @brief
 * Hash table entry
 *
 */

typedef struct OCI_HashEntry
{
    otext *               key;
    struct OCI_HashValue *values;
    struct OCI_HashEntry *next;
} OCI_HashEntry;

/**
 * @} OcilibCApiDatatypes
 */
 
/**
 * @} OcilibCApi
 */

#endif    /* OCILIB_OCILIBC_TYPES_H_INCLUDED */
