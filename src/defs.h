/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2024 Vincent ROGIER <vince.rogier@ocilib.net>
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

#ifndef OCILIB_DEFS_H_INCLUDED
#define OCILIB_DEFS_H_INCLUDED

#include "ocilibc/defines.h"

#include "import.h"

/* ********************************************************************************************* *
                           ORACLE VERSION DETECTION
 * ********************************************************************************************* */

#ifdef OCI_IMPORT_RUNTIME

  /* for runtime loading, set compile time version to the highest known OCI version */

  #define OCI_VERSION_COMPILE OCI_23_6

  /* set runtime version to unknown, it will be guessed from symbols loading */

  #define OCI_VERSION_RUNTIME OCI_UNKNOWN

#else

  #if defined(OCI_MAJOR_VERSION)

    #define OCI_VERSION_COMPILE OCI_VER_MAKE(OCI_MAJOR_VERSION, OCI_MINOR_VERSION, 0)

    #if OCI_VERSION_COMPILE == OCI_VER_MAKE(18, 0, 0)

      /* Oracle headers not updated for Windows 18.3 release ! */

      #undef  OCI_VERSION_COMPILE

      #define OCI_VERSION_COMPILE OCI_18_3

    #endif

  #elif defined(OCI_FNCODE_SODAOPERKEYSSET)     /* = OCI_18_3 */

    #define OCI_VERSION_COMPILE OCI_18_3

  #elif defined(OCI_FNCODE_SERVERRELEASE2)      /* = OCI_18_1 */

    #define OCI_VERSION_COMPILE OCI_18_1

  #elif defined(OCI_FNCODE_LOBGETCHUNKSIZE)     /* = OCI_12_2 */

    #define OCI_VERSION_COMPILE OCI_12_2

  #elif defined(OCI_FNCODE_BINDBYPOS2)          /* = OCI_12_1 */

    #define OCI_VERSION_COMPILE OCI_12_1

  #elif defined(OCI_FNCODE_LOBGETCONTENTTYPE)   /* = OCI_11_2 */

    #define OCI_VERSION_COMPILE OCI_11_2
    
  #elif defined(OCI_FNCODE_LOBGETOPT)           /* = OCI_11_1 */

    #define OCI_VERSION_COMPILE OCI_11_1

  #elif defined(OCI_FNCODE_DBSHUTDOWN)          /* = OCI_10_2 */

    #define OCI_VERSION_COMPILE OCI_10_2

  #elif defined(OCI_FNCODE_LOBREAD2)            /* = OCI_10_1 */

    #define OCI_VERSION_COMPILE OCI_10_1

  #elif defined(OCI_FNCODE_STMTPREPARE2)        /* = OCI_9_2 */

    #define OCI_VERSION_COMPILE OCI_9_2

  #elif defined(OCI_FNCODE_CPOOLCREATE)         /* = OCI_9_0 */

    #define OCI_VERSION_COMPILE OCI_9_0

  #elif defined(OCIThreadHandle)                /* = OCI_8_1 */

    #define OCI_VERSION_COMPILE OCI_8_1

  #elif defined(OCIEnv)                         /* = OCI_8_0 */

    #define OCI_VERSION_COMPILE OCI_8_0

  #else                                         /* OCI_UNKNOWN */

    #define OCI_VERSION_COMPILE OCI_UNKNOWN

  #endif

  #define OCI_VERSION_RUNTIME OCI_VERSION_COMPILE

#endif

/* tries to enable Oracle 10g support for lobs > 4Go */

#if defined(OCI_BIG_UINT_ENABLED) && defined(ORAXB8_DEFINED)

  #define OCI_LOB2_API_ENABLED

#endif

/* ********************************************************************************************* *
                     CHARSET AND STRING TYPES DETECTION
 * ********************************************************************************************* */

/* otext is public character types for all string types
   We need to handle as well an internal oracle string type because :

   - wchar_t is not the same type on all platforms (that is such a pain !),
   - OCI, in Unicode mode, uses Fixed length UTF16 encoding (2 bytes)

   So, dbtext was added to represent internal oracle string type.

   The following checks find out the real types and size of dbtext
*/

#if  defined (OCI_CHARSET_ANSI)

typedef otext dbtext;

#else

typedef unsigned short dbtext;

#endif

/* ********************************************************************************************* *
                            INTERNAL  CONSTANTS
 * ********************************************************************************************* */

/* --------------------------------------------------------------------------------------------- *
 * Default environment mode
 * --------------------------------------------------------------------------------------------- */

#ifdef OCI_CHARSET_WIDE

#define OCI_ENV_MODE    OCI_UTF16

#else

#define OCI_ENV_MODE    OCI_DEFAULT

#endif

/* --------------------------------------------------------------------------------------------- *
 * Internal Pointer Codes
 * --------------------------------------------------------------------------------------------- */

/* -- external C pointers ---- */

#define OCI_IPC_ORACLE          (-1)
#define OCI_IPC_BOOLEAN          0
#define OCI_IPC_VOID             1
#define OCI_IPC_SHORT            2
#define OCI_IPC_INT              3
#define OCI_IPC_BIGINT           4
#define OCI_IPC_DOUBLE           5
#define OCI_IPC_FLOAT            6
#define OCI_IPC_STRING           7
#define OCI_IPC_PROC             8

/* internal OCILIB handles - */

#define OCI_IPC_ERROR            9

/* ---- Internal pointers ----- */

#define OCI_IPC_LIST             42
#define OCI_IPC_LIST_ITEM        43
#define OCI_IPC_BIND_ARRAY       44
#define OCI_IPC_DEFINE           45
#define OCI_IPC_DEFINE_ARRAY     46
#define OCI_IPC_HASHENTRY        47
#define OCI_IPC_HASHENTRY_ARRAY  48
#define OCI_IPC_HASHVALUE        49
#define OCI_IPC_THREADKEY        50
#define OCI_IPC_OCIDATE          51
#define OCI_IPC_TM               52
#define OCI_IPC_RESULTSET_ARRAY  53
#define OCI_IPC_PLS_SIZE_ARRAY   54
#define OCI_IPC_PLS_RCODE_ARRAY  55
#define OCI_IPC_SERVER_OUPUT     56
#define OCI_IPC_INDICATOR_ARRAY  57
#define OCI_IPC_LEN_ARRAY        58
#define OCI_IPC_BUFF_ARRAY       59
#define OCI_IPC_LONG_BUFFER      60
#define OCI_IPC_TRACE_INFO       61
#define OCI_IPC_DP_COL_ARRAY     62
#define OCI_IPC_BATCH_ERRORS     63
#define OCI_IPC_STATEMENT_ARRAY  64

#define OCI_IPC_COUNT            (OCI_IPC_STATEMENT_ARRAY + 2)

/* --------------------------------------------------------------------------------------------- *
 * Oracle conditional features
 * --------------------------------------------------------------------------------------------- */

#define OCI_FEATURE_WIDE_USERDATA        1
#define OCI_FEATURE_TIMESTAMP            2
#define OCI_FEATURE_DIRPATH_DATE_CACHE   3
#define OCI_FEATURE_STATEMENT_CACHING    4
#define OCI_FEATURE_SCROLLABLE_CURSOR    5
#define OCI_FEATURE_DATABASE_NOTIFY      6
#define OCI_FEATURE_REMOTE_DBS_CONTROL   7
#define OCI_FEATURE_HIGH_AVAILABILITY    8
#define OCI_FEATURE_XA                   9
#define OCI_FEATURE_EXTENDED_PLSQLTYPES 10
#define OCI_FEATURE_MULTITHREADING      11

#define OCI_FEATURE_COUNT               OCI_FEATURE_MULTITHREADING

/* --------------------------------------------------------------------------------------------- *
 * handle types
 * --------------------------------------------------------------------------------------------- */

#define OCI_HDLE_HANDLE                 1
#define OCI_HDLE_DESCRIPTOR             2
#define OCI_HDLE_OBJECT                 3

#define OCI_HDLE_COUNT                  OCI_HDLE_OBJECT

/* --------------------------------------------------------------------------------------------- *
 * statement status
 * --------------------------------------------------------------------------------------------- */

#define OCI_STMT_CLOSED                 1
#define OCI_STMT_PARSED                 2
#define OCI_STMT_PREPARED               4
#define OCI_STMT_DESCRIBED              8
#define OCI_STMT_EXECUTED               16

#define OCI_STMT_STATES_COUNT           5

/* --------------------------------------------------------------------------------------------- *
 * connection states
 * --------------------------------------------------------------------------------------------- */

#define OCI_CONN_ALLOCATED              1
#define OCI_CONN_ATTACHED               2
#define OCI_CONN_LOGGED                 3

/* --------------------------------------------------------------------------------------------- *
 * objects status
 * --------------------------------------------------------------------------------------------- */

#define OCI_OBJECT_ALLOCATED            1
#define OCI_OBJECT_FETCHED_CLEAN        2
#define OCI_OBJECT_FETCHED_DIRTY        3
#define OCI_OBJECT_FETCHED_INVALIDATED  4
#define OCI_OBJECT_ALLOCATED_ARRAY      5
#define OCI_OBJECT_ALLOCATED_BIND_STMT  6

/* --------------------------------------------------------------------------------------------- *
 * bind type
 * --------------------------------------------------------------------------------------------- */

#define OCI_BIND_INPUT                  1
#define OCI_BIND_OUTPUT                 2

/* --------------------------------------------------------------------------------------------- *
 * Type of schema describing
 * --------------------------------------------------------------------------------------------- */

#define OCI_DESC_RESULTSET              1
#define OCI_DESC_TABLE                  2
#define OCI_DESC_TYPE                   3
#define OCI_DESC_COLLECTION             4

/* --------------------------------------------------------------------------------------------- *
 * Direct path object status
 * --------------------------------------------------------------------------------------------- */

#define OCI_DPS_NOT_PREPARED            1
#define OCI_DPS_PREPARED                2
#define OCI_DPS_CONVERTED               3
#define OCI_DPS_TERMINATED              4

#define OCI_DPS_COUNT                   OCI_DPS_TERMINATED

/* --------------------------------------------------------------------------------------------- *
 * internal statement fetch direction
 * --------------------------------------------------------------------------------------------- */

#define OCI_SFD_NEXT                    0x02
#define OCI_SFD_FIRST                   0x04
#define OCI_SFD_LAST                    0x08
#define OCI_SFD_PREV                    0x10

/* --------------------------------------------------------------------------------------------- *
 * internal direct path column types
 * --------------------------------------------------------------------------------------------- */

#define OCI_DDT_TEXT                    1
#define OCI_DDT_BINARY                  2
#define OCI_DDT_NUMBER                  3
#define OCI_DDT_OTHERS                  4

/* --------------------------------------------------------------------------------------------- *
 *  output buffer server line size
 * --------------------------------------------------------------------------------------------- */

#define OCI_OUPUT_LSIZE                 255
#define OCI_OUPUT_LSIZE_10G             32767

/* --------------------------------------------------------------------------------------------- *
*  Undocumented OCI SQL TYPES
* --------------------------------------------------------------------------------------------- */

#define SQLT_UNDOCUMENTED_REC           245
#define SQLT_UNDOCUMENTED_BIN_INTEGER   265

/* --------------------------------------------------------------------------------------------- *
 *  Unicode constants
 * --------------------------------------------------------------------------------------------- */

/* OCI unicode flag */

#ifndef OCI_UTF16ID

#define OCI_UTF16ID OCI_UCS2ID

#endif

#define OCI_UTF8_BYTES_PER_CHAR 4
#define OCI_SIZE_TMP_CVT        128

#ifdef _WINDOWS

#define OCI_CVT_CHAR                  1

#else

#define OCI_CVT_CHAR                  0

#endif

#define OCI_SQLCMD_COUNT                241

#define OCI_FMT_COUNT                   6

#define OCI_BIND_ARRAY_GROWTH_FACTOR    128

#define OCI_DEF_ALIGN                   sizeof(void *)

#define ROUNDUP(amount, align) \
                               \
    (((unsigned long)(amount)+((align)-1))&~((align)-1))

#define osizeof(s) (sizeof(s) / sizeof(otext))

#define ocharcount(l)  ((l) / sizeof(otext))
#define dbcharcount(l) ((l) / sizeof(dbtext))

#define OCI_ERR_AQ_LISTEN_TIMEOUT      25254
#define OCI_ERR_AQ_DEQUEUE_TIMEOUT     25228
#define OCI_ERR_SUB_BUG_OCI_UTF16      24915
#define OCI_ERR_AQ_QUEUE_NAME_INVALID  25200
#define OCI_ERR_AQ_QUEUE_NOT_EXIST     24010

#define OCI_DEFAUT_STMT_CACHE_SIZE     20

#define WCHAR_2_BYTES   0xFFFF
#define WCHAR_4_BYTES   0x7FFFFFFF

#define SCALE_FLOAT     (-127)

#define OCI_VARS_COUNT 1

#define OCI_VARS_TRUE_VALUE "TRUE"

#define OCI_VARS_WORKAROUND_UTF16_COLUMN_NAME  0

#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif

/* check OCI status */

#define OCI_FAILURE(res)                ((res) != OCI_SUCCESS)
#define OCI_SUCCESSFUL(res)             ((res) == OCI_SUCCESS)

/* indicator and nullity handlers */

#define OCI_IND(exp)                    (sb2) ((exp) ? 0 : -1)

#define OCI_NOT_USED(p)                 (p) = (p);

#define NB_ARG_VERSION                  3

#define LIB_THREADED                    ((Env.env_mode & OCI_ENV_THREADED) && (Env.version_runtime> OCI_8_0))

#define IS_PLSQL_STMT(type)         \
                                    \
    ((OCI_CST_BEGIN   == (type)) || \
     (OCI_CST_DECLARE == (type)) || \
     (OCI_CST_CALL    == (type)))

#define IS_OCI_NUMBER(type, subtype) \
                                     \
    (OCI_CDT_NUMERIC == (type) && OCI_NUM_NUMBER == (subtype))

#define IS_ROWID_COL(col) \
                                     \
    (OCI_CDT_TEXT == (col)->datatype && SQLT_RDD == (col)->libcode)

#define IS_OCILIB_OBJECT(type, subtype) \
                                        \
    ( (IS_OCI_NUMBER(type, subtype)) || \
      (OCI_CDT_TEXT    != (type) &&     \
       OCI_CDT_NUMERIC != (type) &&     \
       OCI_CDT_RAW     != (type) &&     \
       OCI_CDT_BOOLEAN != (type)))

#define IS_XMLTYPE(typinf)                                  \
      ( NULL != (typinf) &&                                 \
        NULL != (typinf)->name &&                           \
        SQLT_OPAQUE_TYPE == (typinf)->typecode &&           \
        0 == ostrcmp((typinf)->name, OTEXT("XMLTYPE")) )    \

#define IS_XMLTYPE_COL(col)                                 \
     ( NULL != (col) &&                                     \
       (col)->sqlcode ==  SQLT_OPAQUE_TYPE &&               \
       IS_XMLTYPE((col)->typinf) )

#define IS_DYNAMIC_FETCH_COLUMN(col)                        \
    ( OCI_CDT_LONG    == (col)->datatype ||                 \
      OCI_CDT_XMLTYPE == (col)->datatype )


#define SQLT_OPAQUE_TYPE 58   /*  SQL/OTS Opaque Types */

/* helpers mapping macros */

#ifdef OCI_CHARSET_ANSI

  #define ostrcpy            strcpy
  #define ostrncpy           strncpy
  #define ostrcat            strcat
  #define ostrncat           strncat
  #define ostrlen            strlen
  #define ostrcmp            strcmp
  #define ostrtol            strtol
  #define osscanf            sscanf
  #define otoupper           toupper
  #define oisdigit           isdigit

  #ifdef _WINDOWS
    #define ovsprintf       _vsnprintf
  #else
    #define ovsprintf       vsnprintf

  #endif

#else

  #define ostrcpy           wcscpy
  #define ostrncpy          wcsncpy
  #define ostrcat           wcscat
  #define ostrncat          wcsncat
  #define ostrlen           wcslen
  #define ostrcmp           wcscmp
  #define ostrtol           wcstol
  #define osscanf           swscanf
  #define otoupper          towupper
  #define oisdigit          iswdigit
  #define ovsprintf         vswprintf

#endif

/* string size macros */

#define otextsize(s) (ostrlen(s) * sizeof(otext))

#endif    /* OCILIB_DEFS_H_INCLUDED */
