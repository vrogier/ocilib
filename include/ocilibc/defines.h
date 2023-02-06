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

#ifndef OCILIB_OCILIBC_DEFINES_H_INCLUDED
#define OCILIB_OCILIBC_DEFINES_H_INCLUDED

#include "ocilibc/platform.h"

/**
 * @addtogroup OcilibCApi
 * @{
 *
 */

/* versions extract macros */

#define OCI_VER_MAJ(v)                      (unsigned int) ((v)/10000)
#define OCI_VER_MIN(v)                      (unsigned int) (((v)/100) - (((v)/10000)*100))
#define OCI_VER_REV(v)                      (unsigned int) ((v) - (((v)/100)*100))

#define OCI_VER_MAKE(x, y, z)               ((x)*10000 + (y)*100 + (z))

/* oracle OCI well known major versions (non exhaustive) */

#define OCI_8_0                             OCI_VER_MAKE( 8, 0, 0)
#define OCI_8_1                             OCI_VER_MAKE( 8, 1, 0)
#define OCI_9_0                             OCI_VER_MAKE( 9, 0, 0)
#define OCI_9_2                             OCI_VER_MAKE( 9, 2, 0)
#define OCI_10_1                            OCI_VER_MAKE(10, 1, 0)
#define OCI_10_2                            OCI_VER_MAKE(10, 2, 0)
#define OCI_11_1                            OCI_VER_MAKE(11, 1, 0)
#define OCI_11_2                            OCI_VER_MAKE(11, 2, 0)
#define OCI_12_1                            OCI_VER_MAKE(12, 1, 0)
#define OCI_12_2                            OCI_VER_MAKE(12, 2, 0)
#define OCI_18_1                            OCI_VER_MAKE(18, 1, 0)
#define OCI_18_2                            OCI_VER_MAKE(18, 2, 0)
#define OCI_18_3                            OCI_VER_MAKE(18, 3, 0)
#define OCI_18_4                            OCI_VER_MAKE(18, 4, 0)
#define OCI_18_5                            OCI_VER_MAKE(18, 5, 0)
#define OCI_19_3                            OCI_VER_MAKE(19, 3, 0)
#define OCI_19_5                            OCI_VER_MAKE(19, 5, 0)
#define OCI_21_3                            OCI_VER_MAKE(21, 3, 0)

/* OCILIB Error types */

#define OCI_ERR_ORACLE                      1
#define OCI_ERR_OCILIB                      2
#define OCI_ERR_WARNING                     3

/* OCILIB Error codes */

#define OCI_ERR_NONE                        0
#define OCI_ERR_NOT_INITIALIZED             1
#define OCI_ERR_LOADING_SHARED_LIB          2
#define OCI_ERR_LOADING_SYMBOLS             3
#define OCI_ERR_MULTITHREADED               4
#define OCI_ERR_MEMORY                      5
#define OCI_ERR_NOT_AVAILABLE               6
#define OCI_ERR_NULL_POINTER                7
#define OCI_ERR_DATATYPE_NOT_SUPPORTED      8
#define OCI_ERR_PARSE_TOKEN                 9
#define OCI_ERR_MAP_ARGUMENT                10
#define OCI_ERR_OUT_OF_BOUNDS               11
#define OCI_ERR_UNFREED_DATA                12
#define OCI_ERR_MAX_BIND                    13
#define OCI_ERR_ATTR_NOT_FOUND              14
#define OCI_ERR_MIN_VALUE                   15
#define OCI_ERR_NOT_COMPATIBLE              16
#define OCI_ERR_STMT_STATE                  17
#define OCI_ERR_STMT_NOT_SCROLLABLE         18
#define OCI_ERR_BIND_ALREADY_USED           19
#define OCI_ERR_BIND_ARRAY_SIZE             20
#define OCI_ERR_COLUMN_NOT_FOUND            21
#define OCI_ERR_DIRPATH_STATE               22
#define OCI_ERR_CREATE_OCI_ENVIRONMENT      23
#define OCI_ERR_REBIND_BAD_DATATYPE         24
#define OCI_ERR_TYPEINFO_DATATYPE           25
#define OCI_ERR_ITEM_NOT_FOUND              26
#define OCI_ERR_ARG_INVALID_VALUE           27
#define OCI_ERR_XA_ENV_FROM_STRING          28
#define OCI_ERR_XA_CONN_FROM_STRING         29
#define OCI_ERR_BIND_EXTERNAL_NOT_ALLOWED   30
#define OCI_ERR_UNFREED_BYTES               31

#define OCI_ERR_COUNT                       32

/* Public OCILIB handles */

#define OCI_IPC_TYPE_INFO        10
#define OCI_IPC_CONNECTION       11
#define OCI_IPC_POOL             12
#define OCI_IPC_TRANSACTION      13
#define OCI_IPC_STATEMENT        14
#define OCI_IPC_RESULTSET        15
#define OCI_IPC_COLUMN           16
#define OCI_IPC_DATE             17
#define OCI_IPC_TIMESTAMP        18
#define OCI_IPC_INTERVAL         19
#define OCI_IPC_LOB              20
#define OCI_IPC_FILE             21
#define OCI_IPC_LONG             22
#define OCI_IPC_OBJECT           23
#define OCI_IPC_COLLECTION       24
#define OCI_IPC_ITERATOR         25
#define OCI_IPC_ELEMENT          26
#define OCI_IPC_NUMBER           27
#define OCI_IPC_HASHTABLE        28
#define OCI_IPC_THREAD           29
#define OCI_IPC_MUTEX            30
#define OCI_IPC_BIND             31
#define OCI_IPC_REF              32
#define OCI_IPC_DIRPATH          33
#define OCI_IPC_NOTIFY           34
#define OCI_IPC_EVENT            35
#define OCI_IPC_ARRAY            36
#define OCI_IPC_MSG              37
#define OCI_IPC_ENQUEUE          38
#define OCI_IPC_DEQUEUE          39
#define OCI_IPC_AGENT            40

/* allocated bytes types */

#define OCI_MEM_ORACLE                      1
#define OCI_MEM_OCILIB                      2
#define OCI_MEM_ALL                         (OCI_MEM_ORACLE | OCI_MEM_OCILIB)

/* binding */

#define OCI_BIND_BY_POS                     0
#define OCI_BIND_BY_NAME                    1
#define OCI_BIND_SIZE                       6
#define OCI_BIND_MAX                        65535

/* fetching */

#define OCI_FETCH_SIZE                      20
#define OCI_PREFETCH_SIZE                   20
#define OCI_LONG_EXPLICIT                   1
#define OCI_LONG_IMPLICIT                   2

/* unknown value */

#define OCI_UNKNOWN                         0

/* C Data Type mapping */

#define OCI_CDT_NUMERIC                     1
#define OCI_CDT_DATETIME                    3
#define OCI_CDT_TEXT                        4
#define OCI_CDT_LONG                        5
#define OCI_CDT_CURSOR                      6
#define OCI_CDT_LOB                         7
#define OCI_CDT_FILE                        8
#define OCI_CDT_TIMESTAMP                   9
#define OCI_CDT_INTERVAL                    10
#define OCI_CDT_RAW                         11
#define OCI_CDT_OBJECT                      12
#define OCI_CDT_COLLECTION                  13
#define OCI_CDT_REF                         14
#define OCI_CDT_BOOLEAN                     15

/* Data Type codes for OCI_ImmediateXXX() calls */

#define OCI_ARG_SHORT                       1
#define OCI_ARG_USHORT                      2
#define OCI_ARG_INT                         3
#define OCI_ARG_UINT                        4
#define OCI_ARG_BIGINT                      5
#define OCI_ARG_BIGUINT                     6
#define OCI_ARG_DOUBLE                      7
#define OCI_ARG_DATETIME                    8
#define OCI_ARG_TEXT                        9
#define OCI_ARG_LOB                         10
#define OCI_ARG_FILE                        11
#define OCI_ARG_TIMESTAMP                   12
#define OCI_ARG_INTERVAL                    13
#define OCI_ARG_RAW                         14
#define OCI_ARG_OBJECT                      15
#define OCI_ARG_COLLECTION                  16
#define OCI_ARG_REF                         17
#define OCI_ARG_FLOAT                       18
#define OCI_ARG_NUMBER                      19

/* statement types */

#define OCI_CST_SELECT                      1
#define OCI_CST_UPDATE                      2
#define OCI_CST_DELETE                      3
#define OCI_CST_INSERT                      4
#define OCI_CST_CREATE                      5
#define OCI_CST_DROP                        6
#define OCI_CST_ALTER                       7
#define OCI_CST_BEGIN                       8
#define OCI_CST_DECLARE                     9
#define OCI_CST_CALL                        10
#define OCI_CST_MERGE                       16

/* environment modes */

#define OCI_ENV_DEFAULT                     0
#define OCI_ENV_THREADED                    1
#define OCI_ENV_CONTEXT                     2
#define OCI_ENV_EVENTS                      4

/* sessions modes */

#define OCI_SESSION_DEFAULT                 0x00000000  /* any version */
#define OCI_SESSION_SYSDBA                  0x00000002  /* any version */
#define OCI_SESSION_SYSOPER                 0x00000004  /* any version */
#define OCI_SESSION_SYSASM                  0x00008000  /* From 11gR1  */
#define OCI_SESSION_SYSBKP                  0x00020000  /* From 12cR1  */
#define OCI_SESSION_SYSDGD                  0x00040000  /* From 12cR1  */
#define OCI_SESSION_SYSKMT                  0x00080000  /* From 12cR1  */
#define OCI_SESSION_SYSRAC                  0x00100000  /* From 12cR2  */

#define OCI_SESSION_XA                      0x00000001
#define OCI_SESSION_PRELIM_AUTH             0x00000008

/* change notification types */

#define OCI_CNT_OBJECTS                     1
#define OCI_CNT_ROWS                        2
#define OCI_CNT_DATABASES                   4
#define OCI_CNT_ALL                         (OCI_CNT_OBJECTS | OCI_CNT_ROWS | OCI_CNT_DATABASES)

/* event notification types */

#define OCI_ENT_STARTUP                     1
#define OCI_ENT_SHUTDOWN                    2
#define OCI_ENT_SHUTDOWN_ANY                3
#define OCI_ENT_DROP_DATABASE               4
#define OCI_ENT_DEREGISTER                  5
#define OCI_ENT_OBJECT_CHANGED              6

/* event object notification types */

#define OCI_ONT_INSERT                      0x2
#define OCI_ONT_UPDATE                      0x4
#define OCI_ONT_DELETE                      0x8
#define OCI_ONT_ALTER                       0x10
#define OCI_ONT_DROP                        0x20
#define OCI_ONT_GENERIC                     0x40

/* database startup modes */

#define OCI_DB_SPM_START                    1
#define OCI_DB_SPM_MOUNT                    2
#define OCI_DB_SPM_OPEN                     4
#define OCI_DB_SPM_FULL                     (OCI_DB_SPM_START | OCI_DB_SPM_MOUNT | OCI_DB_SPM_OPEN)

/* database startup flags */

#define OCI_DB_SPF_DEFAULT                  0
#define OCI_DB_SPF_FORCE                    1
#define OCI_DB_SPF_RESTRICT                 2

/* database shutdown modes */

#define OCI_DB_SDM_SHUTDOWN                 1
#define OCI_DB_SDM_CLOSE                    2
#define OCI_DB_SDM_DISMOUNT                 4
#define OCI_DB_SDM_FULL                     (OCI_DB_SDM_SHUTDOWN | OCI_DB_SDM_CLOSE | OCI_DB_SDM_DISMOUNT)

/* database shutdown flags */

#define OCI_DB_SDF_DEFAULT                  0
#define OCI_DB_SDF_TRANS                    1
#define OCI_DB_SDF_TRANS_LOCAL              2
#define OCI_DB_SDF_IMMEDIATE                3
#define OCI_DB_SDF_ABORT                    4

/* charset form types */

#define OCI_CSF_NONE                        0
#define OCI_CSF_DEFAULT                     1
#define OCI_CSF_NATIONAL                    2

/* statement fetch mode */

#define OCI_SFM_DEFAULT                     0
#define OCI_SFM_SCROLLABLE                  0x08

/* statement fetch direction */

#define OCI_SFD_ABSOLUTE                    0x20
#define OCI_SFD_RELATIVE                    0x40

/* bind allocation mode */

#define OCI_BAM_EXTERNAL                    1
#define OCI_BAM_INTERNAL                    2

/* bind direction mode */

#define OCI_BDM_IN                          1
#define OCI_BDM_OUT                         2
#define OCI_BDM_IN_OUT                      (OCI_BDM_IN | OCI_BDM_OUT)

/* Column property flags     */

#define OCI_CPF_NONE                        0
#define OCI_CPF_IS_IDENTITY                 1
#define OCI_CPF_IS_GEN_ALWAYS               2
#define OCI_CPF_IS_GEN_BY_DEFAULT_ON_NULL   4
#define OCI_CPF_IS_LPART                    8
#define OCI_CPF_IS_CONID                   16

/* Column collation IDs     */

#define OCI_CCI_NONE                        0x00000000
#define OCI_CCI_NLS_COMP                    0x00003FFE
#define OCI_CCI_NLS_SORT                    0x00003FFD
#define OCI_CCI_NLS_SORT_CI                 0x00003FFC
#define OCI_CCI_NLS_SORT_AI                 0x00003FFB
#define OCI_CCI_NLS_SORT_CS                 0x00003FFA
#define OCI_CCI_NLS_SORT_VAR1               0x00003FF9
#define OCI_CCI_NLS_SORT_VAR1_CI            0x00003FF8
#define OCI_CCI_NLS_SORT_VAR1_AI            0x00003FF7
#define OCI_CCI_NLS_SORT_VAR1_CS            0x00003FF6
#define OCI_CCI_BINARY                      0x00003FFF
#define OCI_CCI_BINARY_CI                   0x00023FFF
#define OCI_CCI_BINARY_AI                   0x00013FFF

/* Integer sign flag */

#define OCI_NUM_UNSIGNED                    2

/* External Integer types */

#define OCI_NUM_SHORT                       4
#define OCI_NUM_INT                         8
#define OCI_NUM_BIGINT                      16
#define OCI_NUM_FLOAT                       32
#define OCI_NUM_DOUBLE                      64
#define OCI_NUM_NUMBER                      128

#define OCI_NUM_USHORT                      (OCI_NUM_SHORT  | OCI_NUM_UNSIGNED)
#define OCI_NUM_UINT                        (OCI_NUM_INT    | OCI_NUM_UNSIGNED)
#define OCI_NUM_BIGUINT                     (OCI_NUM_BIGINT | OCI_NUM_UNSIGNED)

/* timestamp types */

#define OCI_TIMESTAMP                       1
#define OCI_TIMESTAMP_TZ                    2
#define OCI_TIMESTAMP_LTZ                   3

/* interval types */

#define OCI_INTERVAL_YM                     1
#define OCI_INTERVAL_DS                     2

/* long types */

#define OCI_BLONG                           1
#define OCI_CLONG                           2

/* lob types */

#define OCI_BLOB                            1
#define OCI_CLOB                            2
#define OCI_NCLOB                           3

/* lob opening mode */

#define OCI_LOB_READONLY                    1
#define OCI_LOB_READWRITE                   2

/* file types */

#define OCI_BFILE                           1
#define OCI_CFILE                           2

/* lob browsing mode */

#define OCI_SEEK_SET                        1
#define OCI_SEEK_END                        2
#define OCI_SEEK_CUR                        3

/* type info types */

#define OCI_TIF_TABLE                       1
#define OCI_TIF_VIEW                        2
#define OCI_TIF_TYPE                        3

/* object type */

#define OCI_OBJ_PERSISTENT                  1
#define OCI_OBJ_TRANSIENT                   2
#define OCI_OBJ_VALUE                       3

/* collection types */

#define OCI_COLL_VARRAY                     1
#define OCI_COLL_NESTED_TABLE               2
#define OCI_COLL_INDEXED_TABLE              3

/* pool types */

#define OCI_POOL_CONNECTION                 1
#define OCI_POOL_SESSION                    2

/* AQ message state */

#define OCI_AMS_READY                       1
#define OCI_AMS_WAITING                     2
#define OCI_AMS_PROCESSED                   3
#define OCI_AMS_EXPIRED                     4

/* AQ sequence deviation    */

#define OCI_ASD_BEFORE                      2
#define OCI_ASD_TOP                         3

/* AQ message visibility    */

#define OCI_AMV_IMMEDIATE                   1
#define OCI_AMV_ON_COMMIT                   2

/* AQ dequeue mode    */

#define OCI_ADM_BROWSE                      1
#define OCI_ADM_LOCKED                      2
#define OCI_ADM_REMOVE                      3
#define OCI_ADM_REMOVE_NODATA               4

/* AQ dequeue navigation */

#define OCI_ADN_FIRST_MSG                   1
#define OCI_ADN_NEXT_TRANSACTION            2
#define OCI_ADN_NEXT_MSG                    3

/* AQ queue table purge mode */

#define OCI_APM_BUFFERED                    1
#define OCI_APM_PERSISTENT                  2
#define OCI_APM_ALL                         (OCI_APM_BUFFERED | OCI_APM_PERSISTENT)

/* AQ queue table grouping mode */

#define OCI_AGM_NONE                        0
#define OCI_AGM_TRANSACTIONNAL              1

/* AQ queue table type */

#define OCI_AQT_NORMAL                      0
#define OCI_AQT_EXCEPTION                   1
#define OCI_AQT_NON_PERSISTENT              2

/* direct path processing return status */

#define OCI_DPR_COMPLETE                    1
#define OCI_DPR_ERROR                       2
#define OCI_DPR_FULL                        3
#define OCI_DPR_PARTIAL                     4
#define OCI_DPR_EMPTY                       5

/* direct path conversion modes */

#define OCI_DCM_DEFAULT                     1
#define OCI_DCM_FORCE                       2

/* trace size constants */

#define OCI_SIZE_TRACE_ID                   64
#define OCI_SIZE_TRACE_MODULE               48
#define OCI_SIZE_TRACE_ACTION               32
#define OCI_SIZE_TRACE_INFO                 64
#define OCI_SIZE_TRACE_OPERATION            32

/* trace types */

#define OCI_TRC_IDENTITY                    1
#define OCI_TRC_MODULE                      2
#define OCI_TRC_ACTION                      3
#define OCI_TRC_DETAIL                      4
#define OCI_TRC_OPERATION                   5

/* Network timeout type */

#define OCI_NTO_SEND                        1
#define OCI_NTO_RECEIVE                     2
#define OCI_NTO_CALL                        3

/* HA event type */

#define OCI_HET_DOWN                        0
#define OCI_HET_UP                          1

/* HA event source */
#define OCI_HES_INSTANCE                    0
#define OCI_HES_DATABASE                    1
#define OCI_HES_NODE                        2
#define OCI_HES_SERVICE                     3
#define OCI_HES_SERVICE_MEMBER              4
#define OCI_HES_ASM_INSTANCE                5
#define OCI_HES_PRECONNECT                  6

/* Fail over types */

#define OCI_FOT_NONE                        1
#define OCI_FOT_SESSION                     2
#define OCI_FOT_SELECT                      4

/* fail over notifications */

#define OCI_FOE_END                         1
#define OCI_FOE_ABORT                       2
#define OCI_FOE_REAUTH                      4
#define OCI_FOE_BEGIN                       8
#define OCI_FOE_ERROR                       16

/* fail over callback return code */

#define OCI_FOC_OK                          0
#define OCI_FOC_RETRY                       25410

/* hash tables support */

#define OCI_HASH_STRING                     1
#define OCI_HASH_INTEGER                    2
#define OCI_HASH_POINTER                    3

/* transaction types */

#define OCI_TRS_NEW                         0x00000001
#define OCI_TRS_READONLY                    0x00000100
#define OCI_TRS_READWRITE                   0x00000200
#define OCI_TRS_SERIALIZABLE                0x00000400
#define OCI_TRS_LOOSE                       0x00010000
#define OCI_TRS_TIGHT                       0x00020000

/* format types */

#define OCI_FMT_DATE                        1
#define OCI_FMT_TIMESTAMP                   2
#define OCI_FMT_NUMERIC                     3
#define OCI_FMT_BINARY_DOUBLE               4
#define OCI_FMT_BINARY_FLOAT                5
#define OCI_FMT_TIMESTAMP_TZ                6

/* sql function codes */

#define OCI_SFC_CREATE_TABLE                    1
#define OCI_SFC_INSERT                          2
#define OCI_SFC_SELECT                          3
#define OCI_SFC_CREATE_CLUSTER                  4
#define OCI_SFC_ALTER_CLUSTER                   5
#define OCI_SFC_UPDATE                          6
#define OCI_SFC_DELETE                          7
#define OCI_SFC_DROP_CLUSTER                    8
#define OCI_SFC_CREATE_INDEX                    9
#define OCI_SFC_DROP_INDEX                      10
#define OCI_SFC_ALTER_INDEX                     11
#define OCI_SFC_DROP_TABLE                      12
#define OCI_SFC_CREATE_SEQUENCE                 13
#define OCI_SFC_ALTER_SEQUENCE                  14
#define OCI_SFC_ALTER_TABLE                     15
#define OCI_SFC_DROP_SEQUENCE                   16
#define OCI_SFC_GRANT_OBJECT                    17
#define OCI_SFC_REVOKE_OBJECT                   18
#define OCI_SFC_CREATE_SYNONYM                  19
#define OCI_SFC_DROP_SYNONYM                    20
#define OCI_SFC_CREATE_VIEW                     21
#define OCI_SFC_DROP_VIEW                       22
#define OCI_SFC_VALIDATE_INDEX                  23
#define OCI_SFC_CREATE_PROCEDURE                24
#define OCI_SFC_ALTER_PROCEDURE                 25
#define OCI_SFC_LOCK                            26
#define OCI_SFC_NO_OP                           27
#define OCI_SFC_RENAME                          28
#define OCI_SFC_COMMENT                         29
#define OCI_SFC_AUDIT_OBJECT                    30
#define OCI_SFC_NOAUDIT_OBJECT                  31
#define OCI_SFC_CREATE_DATABASE_LINK            32
#define OCI_SFC_DROP_DATABASE_LINK              33
#define OCI_SFC_CREATE_DATABASE                 34
#define OCI_SFC_ALTER_DATABASE                  35
#define OCI_SFC_CREATE_ROLLBACK_SEG             36
#define OCI_SFC_ALTER_ROLLBACK_SEG              37
#define OCI_SFC_DROP_ROLLBACK_SEG               38
#define OCI_SFC_CREATE_TABLESPACE               39
#define OCI_SFC_ALTER_TABLESPACE                40
#define OCI_SFC_DROP_TABLESPACE                 41
#define OCI_SFC_ALTER_SESSION                   42
#define OCI_SFC_ALTER_USER                      43
#define OCI_SFC_COMMIT                          44
#define OCI_SFC_ROLLBACK                        45
#define OCI_SFC_SAVEPOINT                       46
#define OCI_SFC_PL_SQL_EXECUTE                  47
#define OCI_SFC_SET_TRANSACTION                 48
#define OCI_SFC_ALTER_SYSTEM                    49
#define OCI_SFC_EXPLAIN                         50
#define OCI_SFC_CREATE_USER                     51
#define OCI_SFC_CREATE_ROLE                     52
#define OCI_SFC_DROP_USER                       53
#define OCI_SFC_DROP_ROLE                       54
#define OCI_SFC_SET_ROLE                        55
#define OCI_SFC_CREATE_SCHEMA                   56
#define OCI_SFC_CREATE_CONTROL_FILE             57
#define OCI_SFC_ALTER_TRACING                   58
#define OCI_SFC_CREATE_TRIGGER                  59
#define OCI_SFC_ALTER_TRIGGER                   60
#define OCI_SFC_DROP_TRIGGER                    61
#define OCI_SFC_ANALYZE_TABLE                   62
#define OCI_SFC_ANALYZE_INDEX                   63
#define OCI_SFC_ANALYZE_CLUSTER                 64
#define OCI_SFC_CREATE_PROFILE                  65
#define OCI_SFC_DROP_PROFILE                    66
#define OCI_SFC_ALTER_PROFILE                   67
#define OCI_SFC_DROP_PROCEDURE                  68
#define OCI_SFC_ALTER_RESOURCE_COST             70
#define OCI_SFC_CREATE_MATERIALIZED_VIEW_LOG    71
#define OCI_SFC_ALTER_MATERIALIZED_VIEW_LOG     72
#define OCI_SFC_DROP_MATERIALIZED_VIEW_LOG      73
#define OCI_SFC_CREATE_MATERIALIZED_VIEW        74
#define OCI_SFC_ALTER_MATERIALIZED_VIEW         75
#define OCI_SFC_DROP_MATERIALIZED_VIEW          76
#define OCI_SFC_CREATE_TYPE                     77
#define OCI_SFC_DROP_TYPE                       78
#define OCI_SFC_ALTER_ROLE                      79
#define OCI_SFC_ALTER_TYPE                      80
#define OCI_SFC_CREATE_TYPE_BODY                81
#define OCI_SFC_ALTER_TYPE_BODY                 82
#define OCI_SFC_DROP_TYPE_BODY                  83
#define OCI_SFC_DROP_LIBRARY                    84
#define OCI_SFC_TRUNCATE_TABLE                  85
#define OCI_SFC_TRUNCATE_CLUSTER                86
#define OCI_SFC_ALTER_VIEW                      88
#define OCI_SFC_SET_CONSTRAINTS                 90
#define OCI_SFC_CREATE_FUNCTION                 91
#define OCI_SFC_ALTER_FUNCTION                  92
#define OCI_SFC_DROP_FUNCTION                   93
#define OCI_SFC_CREATE_PACKAGE                  94
#define OCI_SFC_ALTER_PACKAGE                   95
#define OCI_SFC_DROP_PACKAGE                    96
#define OCI_SFC_CREATE_PACKAGE_BODY             97
#define OCI_SFC_ALTER_PACKAGE_BODY              98
#define OCI_SFC_DROP_PACKAGE_BODY               99
#define OCI_SFC_LOGON                           100
#define OCI_SFC_LOGOFF                          101
#define OCI_SFC_LOGOFF_BY_CLEANUP               102
#define OCI_SFC_SESSION_REC                     103
#define OCI_SFC_SYSTEM_AUDIT                    104
#define OCI_SFC_SYSTEM_NOAUDIT                  105
#define OCI_SFC_AUDIT_DEFAULT                   106
#define OCI_SFC_NOAUDIT_DEFAULT                 107
#define OCI_SFC_SYSTEM_GRANT                    108
#define OCI_SFC_SYSTEM_REVOKE                   109
#define OCI_SFC_CREATE_PUBLIC_SYNONYM           110
#define OCI_SFC_DROP_PUBLIC_SYNONYM             111
#define OCI_SFC_CREATE_PUBLIC_DATABASE_LINK     112
#define OCI_SFC_DROP_PUBLIC_DATABASE_LINK       113
#define OCI_SFC_GRANT_ROLE                      114
#define OCI_SFC_REVOKE_ROLE                     115
#define OCI_SFC_EXECUTE_PROCEDURE               116
#define OCI_SFC_USER_COMMENT                    117
#define OCI_SFC_ENABLE_TRIGGER                  118
#define OCI_SFC_DISABLE_TRIGGER                 119
#define OCI_SFC_ENABLE_ALL_TRIGGERS             120
#define OCI_SFC_DISABLE_ALL_TRIGGERS            121
#define OCI_SFC_NETWORK_ERROR                   122
#define OCI_SFC_EXECUTE_TYPE                    123
#define OCI_SFC_READ_DIRECTORY                  125
#define OCI_SFC_WRITE_DIRECTORY                 126
#define OCI_SFC_FLASHBACK                       128
#define OCI_SFC_BECOME_USER                     129
#define OCI_SFC_ALTER_MINING_MODEL              130
#define OCI_SFC_SELECT_MINING_MODEL             131
#define OCI_SFC_CREATE_MINING_MODEL             133
#define OCI_SFC_ALTER_PUBLIC_SYNONYM            134
#define OCI_SFC_EXECUTE_DIRECTORY               135
#define OCI_SFC_SQL_LOADER_DIRECT_PATH_LOAD     136
#define OCI_SFC_DATAPUMP_DIRECT_PATH_UNLOAD     137
#define OCI_SFC_DATABASE_STARTUP                138
#define OCI_SFC_DATABASE_SHUTDOWN               139
#define OCI_SFC_CREATE_SQL_TXLN_PROFILE         140
#define OCI_SFC_ALTER_SQL_TXLN_PROFILE          141
#define OCI_SFC_USE_SQL_TXLN_PROFILE            142
#define OCI_SFC_DROP_SQL_TXLN_PROFILE           143
#define OCI_SFC_CREATE_MEASURE_FOLDER           144
#define OCI_SFC_ALTER_MEASURE_FOLDER            145
#define OCI_SFC_DROP_MEASURE_FOLDER             146
#define OCI_SFC_CREATE_CUBE_BUILD_PROCESS       147
#define OCI_SFC_ALTER_CUBE_BUILD_PROCESS        148
#define OCI_SFC_DROP_CUBE_BUILD_PROCESS         149
#define OCI_SFC_CREATE_CUBE                     150
#define OCI_SFC_ALTER_CUBE                      151
#define OCI_SFC_DROP_CUBE                       152
#define OCI_SFC_CREATE_CUBE_DIMENSION           153
#define OCI_SFC_ALTER_CUBE_DIMENSION            154
#define OCI_SFC_DROP_CUBE_DIMENSION             155
#define OCI_SFC_CREATE_DIRECTORY                157
#define OCI_SFC_DROP_DIRECTORY                  158
#define OCI_SFC_CREATE_LIBRARY                  159
#define OCI_SFC_CREATE_JAVA                     160
#define OCI_SFC_ALTER_JAVA                      161
#define OCI_SFC_DROP_JAVA                       162
#define OCI_SFC_CREATE_OPERATOR                 163
#define OCI_SFC_CREATE_INDEXTYPE                164
#define OCI_SFC_DROP_INDEXTYPE                  165
#define OCI_SFC_ALTER_INDEXTYPE                 166
#define OCI_SFC_DROP_OPERATOR                   167
#define OCI_SFC_ASSOCIATE_STATISTICS            168
#define OCI_SFC_DISASSOCIATE_STATISTICS         169
#define OCI_SFC_CALL_METHOD                     170
#define OCI_SFC_CREATE_SUMMARY                  171
#define OCI_SFC_ALTER_SUMMARY                   172
#define OCI_SFC_DROP_SUMMARY                    173
#define OCI_SFC_CREATE_DIMENSION                174
#define OCI_SFC_ALTER_DIMENSION                 175
#define OCI_SFC_DROP_DIMENSION                  176
#define OCI_SFC_CREATE_CONTEXT                  177
#define OCI_SFC_DROP_CONTEXT                    178
#define OCI_SFC_ALTER_OUTLINE                   179
#define OCI_SFC_CREATE_OUTLINE                  180
#define OCI_SFC_DROP_OUTLINE                    181
#define OCI_SFC_UPDATE_INDEXES                  182
#define OCI_SFC_ALTER_OPERATOR                  183
#define OCI_SFC_CREATE_SPFILE                   187
#define OCI_SFC_CREATE_PFILE                    188
#define OCI_SFC_MERGE                           189
#define OCI_SFC_PASSWORD_CHANGE                 190
#define OCI_SFC_ALTER_SYNONYM                   192
#define OCI_SFC_ALTER_DISKGROUP                 193
#define OCI_SFC_CREATE_DISKGROUP                194
#define OCI_SFC_DROP_DISKGROUP                  195
#define OCI_SFC_PURGE_RECYCLEBIN                197
#define OCI_SFC_PURGE_DBA_RECYCLEBIN            198
#define OCI_SFC_PURGE_TABLESPACE                199
#define OCI_SFC_PURGE_TABLE                     200
#define OCI_SFC_PURGE_INDEX                     201
#define OCI_SFC_UNDROP_OBJECT                   202
#define OCI_SFC_DROP_DATABASE                   203
#define OCI_SFC_FLASHBACK_DATABASE              204
#define OCI_SFC_FLASHBACK_TABLE                 205
#define OCI_SFC_CREATE_RESTORE_POINT            206
#define OCI_SFC_DROP_RESTORE_POINT              207
#define OCI_SFC_PROXY_AUTHENTICATION_ONLY       208
#define OCI_SFC_DECLARE_REWRITE_EQUIVALENCE     209
#define OCI_SFC_ALTER_REWRITE_EQUIVALENCE       210
#define OCI_SFC_DROP_REWRITE_EQUIVALENCE        211
#define OCI_SFC_CREATE_EDITION                  212
#define OCI_SFC_ALTER_EDITION                   213
#define OCI_SFC_DROP_EDITION                    214
#define OCI_SFC_DROP_ASSEMBLY                   215
#define OCI_SFC_CREATE_ASSEMBLY                 216
#define OCI_SFC_ALTER_ASSEMBLY                  217
#define OCI_SFC_CREATE_FLASHBACK_ARCHIVE        218
#define OCI_SFC_ALTER_FLASHBACK_ARCHIVE         219
#define OCI_SFC_DROP_FLASHBACK_ARCHIVE          220
#define OCI_SFC_DEBUG_CONNECT                   221
#define OCI_SFC_DEBUG_PROCEDURE                 223
#define OCI_SFC_ALTER_DATABASE_LINK             225
#define OCI_SFC_CREATE_PLUGGABLE_DATABASE       226
#define OCI_SFC_ALTER_PLUGGABLE_DATABASE        227
#define OCI_SFC_DROP_PLUGGABLE_DATABASE         228
#define OCI_SFC_CREATE_AUDIT_POLICY             229
#define OCI_SFC_ALTER_AUDIT_POLICY              230
#define OCI_SFC_DROP_AUDIT_POLICY               231
#define OCI_SFC_CODE_BASED_GRANT                232
#define OCI_SFC_CODE_BASED_REVOKE               233
#define OCI_SFC_CREATE_LOCKDOWN_PROFILE         234
#define OCI_SFC_DROP_LOCKDOWN_PROFILE           235
#define OCI_SFC_ALTER_LOCKDOWN_PROFILE          236
#define OCI_SFC_TRANSLATE_SQL                   237
#define OCI_SFC_ADMINISTER_KEY_MANAGEMENT       238
#define OCI_SFC_CREATE_MATERIALIZED_ZONEMAP     239
#define OCI_SFC_ALTER_MATERIALIZED_ZONEMAP      240
#define OCI_SFC_DROP_MATERIALIZED_ZONEMAP       241
#define OCI_SFC_DROP_MINING_MODEL               242
#define OCI_SFC_CREATE_ATTRIBUTE_DIMENSION      243
#define OCI_SFC_ALTER_ATTRIBUTE_DIMENSION       244
#define OCI_SFC_DROP_ATTRIBUTE_DIMENSION        245
#define OCI_SFC_CREATE_HIERARCHY                246
#define OCI_SFC_ALTER_HIERARCHY                 247
#define OCI_SFC_DROP_HIERARCHY                  248
#define OCI_SFC_CREATE_ANALYTIC_VIEW            249
#define OCI_SFC_ALTER_ANALYTIC_VIEW             250
#define OCI_SFC_DROP_ANALYTIC_VIEW              251
#define OCI_SFC_CREATE_LANGUAGE                 270
#define OCI_SFC_DROP_LANGUAGE                   271
#define OCI_SFC_ALTER_PUBLIC_DATABASE_LINK      305

/* size constants */

#define OCI_SIZE_FORMAT                     64
#define OCI_SIZE_BUFFER                     512
#define OCI_SIZE_LARGE_BUFFER               ((64*1024)-1)
#define OCI_SIZE_LONG                       ((64*1024)-1)
#define OCI_SIZE_DATE                       45
#define OCI_SIZE_TIMESTAMP                  54
#define OCI_SIZE_FORMAT_TODATE              14
#define OCI_SIZE_NULL                       4
#define OCI_SIZE_PRECISION                  10
#define OCI_SIZE_ROWID                      23 /* deprecated */
#define OCI_SIZE_DIRECTORY                  30
#define OCI_SIZE_FILENAME                   255
#define OCI_SIZE_FORMAT_NUMS                40
#define OCI_SIZE_FORMAT_NUML                65
#define OCI_SIZE_OBJ_NAME                   128

#define OCI_HASH_DEFAULT_SIZE               256

/* string constants */

#define OCILIB_DRIVER_NAME                  OTEXT("OCILIB")
#define OCI_STRING_NULL                     OTEXT("NULL")
#define OCI_STRING_EMPTY                    OTEXT("")
#define OCI_STRING_FORMAT_DATE              OTEXT("YYYY-MM-DD")
#define OCI_STRING_FORMAT_TIME              OTEXT("HH24:MI:SS")
#define OCI_STRING_FORMAT_DATETIME          OTEXT("YYYY-MM-DD HH24:MI:SS")
#define OCI_STRING_FORMAT_TIMESTAMP         OTEXT("YYYY-MM-DD HH24:MI:SS.FF")
#define OCI_STRING_FORMAT_TIMESTAMP_TZ      OTEXT("YYYY-MM-DD HH24:MI:SS.FF TZR")
#define OCI_STRING_DEFAULT_PREC             3
#define OCI_STRING_FORMAT_NUM   \
    OTEXT("FM99999999999999999999999999999999999990.999999999999999999999999")
#define OCI_STRING_FORMAT_NUM_BDOUBLE       OTEXT("%lf")
#define OCI_STRING_FORMAT_NUM_BFLOAT        OTEXT("%f")
#define OCI_STRING_FORMAT_NUM_SHORT         OTEXT("%hd")
#define OCI_STRING_FORMAT_NUM_INT           OTEXT("%d")
#define OCI_STRING_TRUE                     OTEXT("TRUE")
#define OCI_STRING_FALSE                    OTEXT("FALSE")
#define OCI_STRING_TRUE_SIZE                4
#define OCI_STRING_FALSE_SIZE               5
#define OCI_STRING_NULL_SIZE                4

#ifdef _WINDOWS
#define OCI_CHAR_SLASH                    '\\'
#else
  #define OCI_CHAR_SLASH                    '/'
#endif

#define OCI_STRING_FORMAT_NUM_BIN               OCI_STRING_FORMAT_NUM_BDOUBLE

/**
* @addtogroup OcilibCApiEnvironmentVariables
* @{
*
* Some environment variables can be defined in order to activate specific features
* They must have one of the following values (case insensitive) for being enabled : "TRUE", "1"
*
* - "OCILIB_WORKAROUND_UTF16_COLUMN_NAME": This variable enables an experimental workaround for the Oracle bug 9838993:
*    - When executing select statements, OCILIB queries column names against the Oracle Client
*    - Unicode builds of OCILIB initialize Oracle client into UTF16 mode
*    - In such environments, a memory leak occurs when statements are re-executed multiple times followed by retrieving the resultset
*    - This workaround retrieves column names using direct access to undocumented Oracle structures instead of using buggy Oracle calls
*    - As Oracle undocumented structures may change upon versions, this workaround is provided as-is in case the Oracle bug represents an real issue for applications
*    - This workaround has been tested with 32bit and 64bit Oracle 12g clients and Unicode OCILIB builds
*/

#define VAR_OCILIB_WORKAROUND_UTF16_COLUMN_NAME "OCILIB_WORKAROUND_UTF16_COLUMN_NAME"

/**
* @} OcilibCApiEnvironmentVariables
*/

/**
 * @} OcilibCApi
 */

#endif /* OCILIB_OCILIBC_DEFINES_H_INCLUDED */
