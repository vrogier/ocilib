/*
    +-----------------------------------------------------------------------------------------+
    |                                                                                         |
    |                               OCILIB - C Driver for Oracle                              |
    |                                                                                         |
    |                                (C Wrapper for Oracle OCI)                               |
    |                                                                                         |
    |                              Website : http://www.ocilib.net                            |
    |                                                                                         |
    |             Copyright (c) 2007-2014 Vincent ROGIER <vince.rogier@ocilib.net>            |
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
 * $Id: ocilib_defs.h, Vincent Rogier $
 * --------------------------------------------------------------------------------------------- */

#ifndef OCILIB_OCILIB_DEFS_H_INCLUDED
#define OCILIB_OCILIB_DEFS_H_INCLUDED

#include "ocilib.h"
#include "oci_import.h"

/* ********************************************************************************************* *
                           ORACLE VERSION DETECTION
 * ********************************************************************************************* */

#ifdef OCI_IMPORT_RUNTIME

    /* for runtime loading, set compile time version to the highest minimum
       version needed by OCILIB encapsulation of OCI */

    #define OCI_VERSION_COMPILE OCI_12_1

    /* set runtime version to unknown, it will be guessed from symbols loading */

    #define OCI_VERSION_RUNTIME OCI_UNKNOWN

#else

    #if defined(OCI_FNCODE_BINDBYPOS2)  /* = OCI_12_1 */

        #define OCI_VERSION_COMPILE OCI_12_1
        #define OCI_VERSION_RUNTIME OCI_12_1
  
    #elif defined(OCI_FNCODE_LOBGETCONTENTTYPE)  /* = OCI_11_2 */

        #define OCI_VERSION_COMPILE OCI_11_2
        #define OCI_VERSION_RUNTIME OCI_11_2

    #elif defined(OCI_FNCODE_LOBGETOPT)        /* = OCI_11_1 */

        #define OCI_VERSION_COMPILE OCI_11_1
        #define OCI_VERSION_RUNTIME OCI_11_1

    #elif defined(OCI_FNCODE_DBSHUTDOWN)       /* = OCI_10_2 */

        #define OCI_VERSION_COMPILE OCI_10_2
        #define OCI_VERSION_RUNTIME OCI_10_2

    #elif defined(OCI_FNCODE_LOBREAD2)         /* = OCI_10_1 */

        #define OCI_VERSION_COMPILE OCI_10_1
        #define OCI_VERSION_RUNTIME OCI_10_1

    #elif defined(OCI_FNCODE_STMTPREPARE2)     /* = OCI_9_2 */

        #define OCI_VERSION_COMPILE OCI_9_2
        #define OCI_VERSION_RUNTIME OCI_9_2

    #elif defined(OCI_FNCODE_CPOOLCREATE)      /* = OCI_9_0 */

        #define OCI_VERSION_COMPILE OCI_9_0
        #define OCI_VERSION_RUNTIME OCI_9_0

    #elif defined(OCIThreadHandle)             /* = OCI_8_1 */

        #define OCI_VERSION_COMPILE OCI_8_1
        #define OCI_VERSION_RUNTIME OCI_8_1

    #elif defined(OCIEnv)                      /* = OCI_8_0 */

        #define OCI_VERSION_COMPILE OCI_8_0
        #define OCI_VERSION_RUNTIME OCI_8_0

    #else                                      /* OCI_UNKNOWN */

        #define OCI_VERSION_COMPILE OCI_UNKNOWN
        #define OCI_VERSION_RUNTIME OCI_UNKNOWN

    #endif

#endif

/* tries to enable Oracle 10g support for lobs > 4Go with OCILobxxx2() calls */

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

    typedef otext           dbtext;         
  
#else
 
    typedef unsigned short  dbtext;   
    
#endif

/* ********************************************************************************************* *
                            INTERNAL  CONSTANTS
 * ********************************************************************************************* */

/* --------------------------------------------------------------------------------------------- *
 * DEfault environnement mode
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

#define OCI_IPC_VOID             1
#define OCI_IPC_SHORT            2
#define OCI_IPC_INT              3
#define OCI_IPC_BIGINT           4
#define OCI_IPC_DOUBLE           5
#define OCI_IPC_FLOAT            6
#define OCI_IPC_STRING           7
#define OCI_IPC_PROC             8

/* -- external OCILIB handles - */

#define OCI_IPC_ERROR            9
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
#define OCI_IPC_HASHTABLE        27
#define OCI_IPC_THREAD           28
#define OCI_IPC_MUTEX            29
#define OCI_IPC_BIND             30
#define OCI_IPC_REF              31
#define OCI_IPC_DIRPATH          32
#define OCI_IPC_NOTIFY           33
#define OCI_IPC_EVENT            34
#define OCI_IPC_ARRAY            35
#define OCI_IPC_MSG              36
#define OCI_IPC_ENQUEUE          37
#define OCI_IPC_DEQUEUE          38
#define OCI_IPC_AGENT            39

/* ---- Internal pointers ----- */

#define OCI_IPC_LIST             40
#define OCI_IPC_LIST_ITEM        41
#define OCI_IPC_BIND_ARRAY       42
#define OCI_IPC_DEFINE           43
#define OCI_IPC_DEFINE_ARRAY     44
#define OCI_IPC_HASHENTRY        45
#define OCI_IPC_HASHENTRY_ARRAY  46
#define OCI_IPC_HASHVALUE        47
#define OCI_IPC_THREADKEY        48
#define OCI_IPC_OCIDATE          49
#define OCI_IPC_TM               50
#define OCI_IPC_RESULTSET_ARRAY  51
#define OCI_IPC_PLS_SIZE_ARRAY   52
#define OCI_IPC_PLS_RCODE_ARRAY  53
#define OCI_IPC_SERVER_OUPUT     54
#define OCI_IPC_INDICATOR_ARRAY  55
#define OCI_IPC_LEN_ARRAY        56
#define OCI_IPC_BUFF_ARRAY       57
#define OCI_IPC_LONG_BUFFER      58
#define OCI_IPC_TRACE_INFO       59
#define OCI_IPC_DP_COL_ARRAY     60
#define OCI_IPC_BATCH_ERRORS     61

#define OCI_IPC_COUNT            OCI_IPC_BATCH_ERRORS

/* --------------------------------------------------------------------------------------------- *
 * Oracle conditionnal features
 * --------------------------------------------------------------------------------------------- */

#define OCI_FEATURE_WIDE_USERDATA       1
#define OCI_FEATURE_TIMESTAMP           2
#define OCI_FEATURE_DIRPATH_DATE_CACHE  3
#define OCI_FEATURE_STATEMENT_CACHING   4
#define OCI_FEATURE_SCROLLABLE_CURSOR   5
#define OCI_FEATURE_DATABASE_NOTIFY     6
#define OCI_FEATURE_REMOTE_DBS_CONTROL  7
#define OCI_FEATURE_HIGH_AVAILABILITY   8
#define OCI_FEATURE_XA                  9

#define OCI_FEATURE_COUNT               OCI_FEATURE_XA

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
#define OCI_OBJECT_ALLOCATED_ARRAY      4
#define OCI_OBJECT_ALLOCATED_BIND_STMT  5

/* --------------------------------------------------------------------------------------------- *
 * bind type
 * --------------------------------------------------------------------------------------------- */

#define OCI_BIND_INPUT                  1
#define OCI_BIND_OUTPUT                 2

/* --------------------------------------------------------------------------------------------- *
 * Type of schema describing
 * --------------------------------------------------------------------------------------------- */

#define OCI_DESC_RESULTSET              1
#define OCI_DESC_COLUMN                 2
#define OCI_DESC_TABLE                  3
#define OCI_DESC_TYPE                   4
#define OCI_DESC_COLLECTION             5

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
 *  offset for alignment computation
 * --------------------------------------------------------------------------------------------- */

#define OCI_OFT_POINTER                 1
#define OCI_OFT_NUMBER                  2
#define OCI_OFT_DATE                    4
#define OCI_OFT_OBJECT                  8
#define OCI_OFT_SHORT                   16
#define OCI_OFT_INT                     32
#define OCI_OFT_BIGINT                  64
#define OCI_OFT_DOUBLE                  128
#define OCI_OFT_FLOAT                   256
#define OCI_OFT_TEXT                    512
#define OCI_OFT_STRUCT                  1024

#define OCI_OFFSET_PAIR(a, b)           (a + (b << 16))

/* --------------------------------------------------------------------------------------------- *
 *  Internal integer types
 * --------------------------------------------------------------------------------------------- */

#define OCI_NUM_NUMBER        0

/* --------------------------------------------------------------------------------------------- *
 *  Unicode constants
 * --------------------------------------------------------------------------------------------- */

/* OCI unicode flag */

#ifndef OCI_UTF16ID
  
    #define OCI_UTF16ID                   OCI_UCS2ID

#endif

/* unicode constants */

#define UNI_SHIFT             ((int) 10 )
#define UNI_BASE              ((unsigned int) 0x0010000UL)
#define UNI_MASK              ((unsigned int) 0x3FFUL)
#define UNI_REPLACEMENT_CHAR  ((unsigned int) 0x0000FFFD)
#define UNI_MAX_BMP           ((unsigned int) 0x0000FFFF)
#define UNI_MAX_UTF16         ((unsigned int) 0x0010FFFF)
#define UNI_MAX_UTF32         ((unsigned int) 0x7FFFFFFF)
#define UNI_MAX_LEGAL_UTF32   ((unsigned int) 0x0010FFFF)
#define UNI_SUR_HIGH_START    ((unsigned int) 0xD800)
#define UNI_SUR_HIGH_END      ((unsigned int) 0xDBFF)
#define UNI_SUR_LOW_START     ((unsigned int) 0xDC00)
#define UNI_SUR_LOW_END       ((unsigned int) 0xDFFF)

#define CVT_SRC_ILLEGAL         0
#define CVT_SRC_EXHAUSTED      -1
#define CVT_DST_EXHAUSTED      -2

#define CVT_STRICT              0
#define CVT_LENIENT             1

#define UTF8_BYTES_PER_CHAR     4

/* --------------------------------------------------------------------------------------------- *
 * Local helper macros
 * --------------------------------------------------------------------------------------------- */

/* check OCI status */

#define OCI_FAILURE(res)                (OCI_SUCCESS != (res))
#define OCI_SUCCESSFUL(res)             (OCI_SUCCESS == (res))

/* memory management helpers */

#define OCI_FREE(ptr)                   OCI_MemFree(ptr), ptr = NULL;

/* indicator and nullity handlers */

#define OCI_IND(exp)                    (sb2) ((exp) ? 0 : -1)

#define OCI_NOT_USED(p)                 (p) = (p);

/* miscellaneaous */

#define OCI_NB_ARG_VERSION              3

#define OCI_LIB_THREADED                (OCILib.env_mode & OCI_ENV_THREADED)

#define OCI_LIB_CONTEXT                 (OCILib.env_mode & OCI_ENV_CONTEXT)

#define OCI_RESULT(res)                                                        \
                                                                               \
    if (OCI_LIB_CONTEXT)                                                       \
        OCI_SetStatus(res);                                                    \

#ifdef _WINDOWS

    #define OCI_CVT_CHAR                  1

#else

    #define OCI_CVT_CHAR                  0

#endif

#define OCI_SQLCMD_COUNT                126

#define OCI_ERR_MSG_SIZE                512

#define OCI_DEF_ALIGN                   sizeof(void *)

#define ROUNDUP(amount, align)                                                 \
                                                                               \
    (((unsigned long)(amount)+((align)-1))&~((align)-1))

#define OCI_SIZEOF_NUMBER               22
#define OCI_SIZEOF_DATE                 7

#define osizeof(s) (sizeof(s) / sizeof(otext))

#define ocharcount(l) (l / sizeof(otext))
#define dbcharcount(l) (l / sizeof(dbtext))

#define OCI_ERR_AQ_LISTEN_TIMEOUT      25254
#define OCI_ERR_AQ_DEQUEUE_TIMEOUT     25228

#define OCI_DEFAUT_STMT_CACHE_SIZE     20

#define WCHAR_2_BYTES   0xFFFF
#define WCHAR_4_BYTES   0x7FFFFFFF


#endif    /* OCILIB_OCILIB_DEFS_H_INCLUDED */

