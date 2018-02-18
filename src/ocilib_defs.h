/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2018 Vincent ROGIER <vince.rogier@ocilib.net>
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

/* ---- Internal pointers ----- */

#define OCI_IPC_LIST             41
#define OCI_IPC_LIST_ITEM        42
#define OCI_IPC_BIND_ARRAY       43
#define OCI_IPC_DEFINE           44
#define OCI_IPC_DEFINE_ARRAY     45
#define OCI_IPC_HASHENTRY        46
#define OCI_IPC_HASHENTRY_ARRAY  47
#define OCI_IPC_HASHVALUE        48
#define OCI_IPC_THREADKEY        49
#define OCI_IPC_OCIDATE          50
#define OCI_IPC_TM               51
#define OCI_IPC_RESULTSET_ARRAY  52
#define OCI_IPC_PLS_SIZE_ARRAY   53
#define OCI_IPC_PLS_RCODE_ARRAY  54
#define OCI_IPC_SERVER_OUPUT     55
#define OCI_IPC_INDICATOR_ARRAY  56
#define OCI_IPC_LEN_ARRAY        57
#define OCI_IPC_BUFF_ARRAY       58
#define OCI_IPC_LONG_BUFFER      59
#define OCI_IPC_TRACE_INFO       60
#define OCI_IPC_DP_COL_ARRAY     61
#define OCI_IPC_BATCH_ERRORS     62
#define OCI_IPC_STATEMENT_ARRAY  63

#define OCI_IPC_COUNT            (OCI_IPC_STATEMENT_ARRAY + 2)

/* --------------------------------------------------------------------------------------------- *
 * Oracle conditionnal features
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

#define OCI_FEATURE_COUNT               OCI_FEATURE_EXTENDED_PLSQLTYPES

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

/* --------------------------------------------------------------------------------------------- *
 * Local helper macros
 * --------------------------------------------------------------------------------------------- */

/* check OCI status */

#define OCI_FAILURE(res)                ((res) != OCI_SUCCESS)
#define OCI_SUCCESSFUL(res)             ((res) == OCI_SUCCESS)

/* memory management helpers */

#define OCI_FREE(ptr)                   { OCI_MemFree(ptr), (ptr) = NULL; }

/* indicator and nullity handlers */

#define OCI_IND(exp)                    (sb2) ((exp) ? 0 : -1)

#define OCI_NOT_USED(p)                 (p) = (p);

/* miscellaneaous */

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
        ctx->call_err = OCI_ErrorGet(FALSE);                                    \
        OCI_CallEnter(ctx);                                                     \
    }

#define OCI_CALL_CONTEXT_EXIT(mode)                                             \
                                                                                \
    if ((mode) & OCI_ENV_CONTEXT)                                               \
    {                                                                           \
        OCI_CallExit(ctx);                                                      \
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

#define OCI_RAISE_EXCEPTION(exp)                                                \
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
        (ptr) = OCI_MemAlloc(type, size, (size_t) (count), TRUE);               \
                                                                                \
        OCI_STATUS = (NULL != (ptr));                                           \
    }                                                                           \

#define OCI_REALLOCATE_BUFFER(type, ptr, size, current, allocated, requested)   \
                                                                                \
    if (OCI_STATUS)                                                             \
    {                                                                           \
        if (!(ptr))                                                             \
        {                                                                       \
            (ptr) = OCI_MemAlloc(type, size, (size_t) (requested), TRUE);       \
            if (ptr) (allocated) = (requested);                                 \
        }                                                                       \
        else if ((current) >= (allocated))                                      \
        {                                                                       \
            (ptr) = OCI_MemRealloc(ptr, type, size, (size_t) (requested), TRUE);\
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

#ifdef _WINDOWS

    #define OCI_CVT_CHAR                  1

#else

    #define OCI_CVT_CHAR                  0

#endif

#define OCI_SQLCMD_COUNT                126

#define OCI_FMT_COUNT                   6

#define OCI_ERR_MSG_SIZE                512

#define OCI_BIND_ARRAY_GROWTH_FACTOR    128

#define OCI_DEF_ALIGN                   sizeof(void *)

#define ROUNDUP(amount, align)                                                 \
                                                                               \
    (((unsigned long)(amount)+((align)-1))&~((align)-1))

#define osizeof(s) (sizeof(s) / sizeof(otext))

#define ocharcount(l)  ((l) / sizeof(otext))
#define dbcharcount(l) ((l) / sizeof(dbtext))

#define OCI_ERR_AQ_LISTEN_TIMEOUT      25254
#define OCI_ERR_AQ_DEQUEUE_TIMEOUT     25228
#define OCI_ERR_SUB_BUG_OCI_UTF16      24915

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

#endif    /* OCILIB_OCILIB_DEFS_H_INCLUDED */

