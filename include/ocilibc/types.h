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
 * @typedef OCI_XmlType
 *
 * @brief
 * Oracle XMLTYPE type representation.
 *
 */

typedef struct OCI_XmlType OCI_XmlType;

/**
 * @typedef OCI_Vector
 *
 * @brief
 * Oracle VECTOR type representation.
 *
 */

typedef struct OCI_Vector OCI_Vector;

/**
 * @typedef OCI_Json
 *
 * @brief
 * Oracle JSON type representation.
 *
 */

typedef struct OCI_Json OCI_Json;

/**
 * @typedef OCI_JsonNode
 *
 * @brief
 * Oracle JSON DOM node representation.
 *
 */

typedef struct OCI_JsonNode OCI_JsonNode;

/**
 * @typedef OCI_SodaDb
 *
 * @brief
 * SODA database handle.
 *
 */

typedef struct OCI_SodaDb OCI_SodaDb;

/**
 * @typedef OCI_SodaColl
 *
 * @brief
 * SODA collection handle.
 *
 */

typedef struct OCI_SodaColl OCI_SodaColl;

/**
 * @typedef OCI_SodaDoc
 *
 * @brief
 * SODA document handle.
 *
 */

typedef struct OCI_SodaDoc OCI_SodaDoc;

/**
 * @typedef OCI_SodaOptions
 *
 * @brief
 * SODA operation options handle.
 *
 */

typedef struct OCI_SodaOptions OCI_SodaOptions;

/**
 * @typedef OCI_SodaCollCursor
 *
 * @brief
 * SODA collection cursor handle.
 *
 */

typedef struct OCI_SodaCollCursor OCI_SodaCollCursor;

/**
 * @typedef OCI_SodaDocCursor
 *
 * @brief
 * SODA document cursor handle.
 *
 */

typedef struct OCI_SodaDocCursor OCI_SodaDocCursor;

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

/* enum-like parameter and return types
 *
 * These are distinct typedefs over OCI_UInt32: they document which OCI_*
 * constant family a parameter or return value expects. They are deliberately
 * typedefs and not enums: several parameters accept combinable flags, and C
 * enums would not be checked anyway. Runtime validation via CHECK_ENUM_VALUE
 * still applies.
 */

typedef OCI_UInt32 OCI_EnvMode;               /* OCI_ENV_* flags, combinable          */
typedef OCI_UInt32 OCI_SessionMode;           /* OCI_SESSION_* flags, combinable      */
typedef OCI_UInt32 OCI_FormatType;            /* OCI_FMT_*                            */
typedef OCI_UInt32 OCI_LocaleCode;            /* OCI_LSI_*                            */
typedef OCI_UInt32 OCI_TraceType;             /* OCI_TRC_*                            */
typedef OCI_UInt32 OCI_TimeoutType;           /* OCI_NTO_*                            */
typedef OCI_UInt32 OCI_PoolType;              /* OCI_POOL_*                           */
typedef OCI_UInt32 OCI_TransactionMode;       /* OCI_TRS_*                            */
typedef OCI_UInt32 OCI_FetchSeekMode;         /* OCI_SFD_*                            */
typedef OCI_UInt32 OCI_FetchMode;             /* OCI_SFM_*                            */
typedef OCI_UInt32 OCI_BindMode;              /* OCI_BIND_BY_*                        */
typedef OCI_UInt32 OCI_BindAllocationMode;    /* OCI_BAM_*                            */
typedef OCI_UInt32 OCI_BindDirection;         /* OCI_BDM_*                            */
typedef OCI_UInt32 OCI_CharsetForm;           /* OCI_CSF_*                            */
typedef OCI_UInt32 OCI_LongMode;              /* OCI_LONG_*                           */
typedef OCI_UInt32 OCI_SeekMode;              /* OCI_SEEK_*                           */
typedef OCI_UInt32 OCI_LobMode;               /* OCI_LOB_*                            */
typedef OCI_UInt32 OCI_DirPathConvertMode;    /* OCI_DCM_*                            */
typedef OCI_UInt32 OCI_QueueSeqDeviation;     /* OCI_ASD_*                            */
typedef OCI_UInt32 OCI_QueueVisibility;       /* OCI_AMV_*                            */
typedef OCI_UInt32 OCI_DequeueMode;           /* OCI_ADM_*                            */
typedef OCI_UInt32 OCI_DequeueNav;            /* OCI_ADN_*                            */
typedef OCI_UInt32 OCI_QueueType;             /* OCI_AQT_*                            */
typedef OCI_UInt32 OCI_QueueGrouping;         /* OCI_AGM_*                            */
typedef OCI_UInt32 OCI_QueuePurgeMode;        /* OCI_APM_*                            */
typedef OCI_UInt32 OCI_SubscriptionType;      /* OCI_CNT_*                            */
typedef OCI_UInt32 OCI_SubscriptionMode;      /* OCI_SRM_*                            */
typedef OCI_UInt32 OCI_DatabaseStartMode;     /* OCI_DB_SPM_*                         */
typedef OCI_UInt32 OCI_DatabaseStartFlag;     /* OCI_DB_SPF_*                         */
typedef OCI_UInt32 OCI_DatabaseShutdownMode;  /* OCI_DB_SDM_*                         */
typedef OCI_UInt32 OCI_DatabaseShutdownFlag;  /* OCI_DB_SDF_*                         */
typedef OCI_UInt32 OCI_LobType;               /* OCI_CLOB, OCI_BLOB, OCI_NCLOB        */
typedef OCI_UInt32 OCI_FileType;              /* OCI_CFILE, OCI_BFILE                 */
typedef OCI_UInt32 OCI_LongType;              /* OCI_CLONG, OCI_BLONG                 */
typedef OCI_UInt32 OCI_NumericType;           /* OCI_NUM_*                            */
typedef OCI_UInt32 OCI_TimestampType;         /* OCI_TIMESTAMP, OCI_TIMESTAMP_TZ/LTZ  */
typedef OCI_UInt32 OCI_IntervalType;          /* OCI_INTERVAL_*                       */
typedef OCI_UInt32 OCI_HashType;              /* OCI_HASH_*                           */
typedef OCI_UInt32 OCI_TypeInfoType;          /* OCI_TIF_*                            */
typedef OCI_UInt32 OCI_VectorFormat;          /* OCI_VEC_*                            */
typedef OCI_UInt32 OCI_JsonType;              /* OCI_JNT_*                            */
typedef OCI_UInt32 OCI_JsonValidation;        /* OCI_JTV_**/
typedef OCI_UInt32 OCI_JsonEncoding;          /* OCI_JTE_**/
typedef OCI_UInt32 OCI_JsonPrint;             /* OCI_JTP_**/
typedef OCI_UInt32 OCI_Stat;                  /* OCI_UST_*                            */
typedef OCI_UInt32 OCI_ErrorType;             /* OCI_ERR_TYPE_ERROR/TYPE_WARNING      */
typedef OCI_UInt32 OCI_ErrorCategory;         /* OCI_ERR_CAT_LIB/CAT_OCI/CAT_JZN      */
typedef OCI_UInt32 OCI_ImportMode;            /* OCI_IMPORT_MODE_*                    */
typedef OCI_UInt32 OCI_Charset;               /* OCI_CHAR_*                           */
typedef OCI_UInt32 OCI_SQLCommand;            /* OCI_SFC_*                            */
typedef OCI_UInt32 OCI_StatementType;         /* OCI_CST_*                            */
typedef OCI_UInt32 OCI_SqlType;               /* OCI_CDT_*                            */
typedef OCI_UInt32 OCI_ColumnPropFlags;       /* OCI_CPF_* flags, combinable          */
typedef OCI_UInt32 OCI_CollationID;           /* OCI_CCI_*                            */
typedef OCI_UInt32 OCI_SubType;               /* OCI_NUM_*, OCI_LOB_*, OCI_FILE_*,
                                                 * OCI_TIMESTAMP_*, OCI_INTERVAL_* subtypes */
typedef OCI_UInt32 OCI_ObjectType;            /* OCI_OBJ_*                            */
typedef OCI_UInt32 OCI_CollType;              /* OCI_COLL_*                           */
typedef OCI_UInt32 OCI_MsgState;              /* OCI_AMS_*                            */
typedef OCI_UInt32 OCI_DirPathResult;         /* OCI_DPR_*                            */
typedef OCI_UInt32 OCI_EventType;             /* OCI_ENT_*                            */
typedef OCI_UInt32 OCI_EventOperation;        /* OCI_ONT_*                            */
typedef OCI_UInt32 OCI_FailoverCode;          /* OCI_FOC_*                            */
typedef OCI_UInt32 OCI_FailoverType;          /* OCI_FOT_*                            */
typedef OCI_UInt32 OCI_FailoverEvent;         /* OCI_FOE_*                            */
typedef OCI_UInt32 OCI_HAEventSource;         /* OCI_HES_*                            */
typedef OCI_UInt32 OCI_HAEventType;           /* OCI_HET_*                            */
typedef OCI_UInt32 OCI_PointerType;           /* OCI_IPC_*                            */
typedef OCI_UInt32 OCI_LogLevel;              /* OCI_LOG_*                            */
typedef OCI_UInt32 OCI_LogCategory;           /* OCI_LOG_CAT_*                        */
typedef OCI_UInt32 OCI_ErrorCode;             /* error code in the category's namespace
                                               * (OCI/OCILIB/JZN) */

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

typedef OCI_FailoverCode (*POCI_TAF_HANDLER)
(
    OCI_Connection   *con,
    OCI_FailoverType  type,
    OCI_FailoverEvent event
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
    OCI_Connection   *con,
    OCI_HAEventSource source,
    OCI_HAEventType   event,
    OCI_Timestamp   * time
);

/**
 * @var POCI_LOG_HANDLER
 *
 * @brief
 * Logging callback prototype.
 *
 * @param env      - Environment handle
 * @param level    - Log level
 * @param category - Log category
 * @param message  - Formatted log line as an ANSI/UTF-8 string
 *
 */

typedef void (*POCI_LOG_HANDLER)
(
    OCI_Environment *env,
    OCI_LogLevel     level,
    OCI_LogCategory  category,
    const char      *message
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
    OCI_Int32 num;

    /* raw data */
    unsigned char *p_bytes;

    /* pointer to c natives types */
    void      * p_void;
    OCI_Int32 * p_int;
    OCI_Float * p_float;
    OCI_Double *p_double;
    otext     * p_text;

    /* ocilib object types */
    OCI_Date     * p_date;
    OCI_Interval * p_interval;
    OCI_Timestamp *p_timestamp;
    OCI_Long     * p_long;
    OCI_Lob      * p_lob;
    OCI_File     * p_file;
    OCI_Statement *p_stmt;
    OCI_Column   * p_col;
    OCI_Object   * p_obj;
    OCI_Coll     * p_coll;
    OCI_Iter     * p_iter;
    OCI_Elem     * p_elem;
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
    otext               * key;
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
