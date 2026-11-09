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

#include "exception.h"

#include <stdio.h>

#include "error.h"
#include "macros.h"
#include "stringutils.h"
#include "oci/jsondom.h"

static const otext * TypeNames[OCI_IPC_COUNT] =
{
    OTEXT("OCI_Boolean pointer"),
    OTEXT("generic pointer"),
    OTEXT("OCI_Int16 pointer"),
    OTEXT("OCI_Int32 pointer"),
    OTEXT("OCI_Int64 pointer"),
    OTEXT("OCI_Double pointer"),
    OTEXT("OCI_Float pointer"),
    OTEXT("string pointer"),
    OTEXT("function callback"),

    OTEXT("Error handle"),
    OTEXT("Environment handle"),
    OTEXT("TypeInfo handle"),
    OTEXT("Connection handle"),
    OTEXT("Pool handle"),
    OTEXT("Transaction handle"),
    OTEXT("Statement handle"),
    OTEXT("Resultset handle"),
    OTEXT("Column handle"),
    OTEXT("Date handle"),
    OTEXT("Timestamp handle"),
    OTEXT("Interval handle"),
    OTEXT("Lob handle"),
    OTEXT("File handle"),
    OTEXT("Long handle"),
    OTEXT("Object handle"),
    OTEXT("Collection handle"),
    OTEXT("Collection iterator handle"),
    OTEXT("Collection element handle"),
    OTEXT("Number handle"),
    OTEXT("Hash Table handle"),
    OTEXT("Bind handle"),
    OTEXT("Ref handle"),
    OTEXT("Direct Path handle"),
    OTEXT("Subscription handle"),
    OTEXT("Event handle"),
    OTEXT("Array handle"),
    OTEXT("Message handle"),
    OTEXT("Enqueue handle"),
    OTEXT("Dequeue handle"),
    OTEXT("Agent handle"),
    OTEXT("XmlType handle"),
    OTEXT("Vector handle"),
    OTEXT("SODA database handle"),
    OTEXT("SODA collection handle"),
    OTEXT("SODA document handle"),
    OTEXT("SODA operation options handle"),
    OTEXT("SODA collection cursor handle"),
    OTEXT("SODA document cursor handle"),

    OTEXT("Internal list handle"),
    OTEXT("Internal list item handle"),
    OTEXT("Internal array of bind handles"),
    OTEXT("Internal define handle"),
    OTEXT("Internal array of define handles"),
    OTEXT("Internal hash entry handle"),
    OTEXT("Internal array of hash entry handles"),
    OTEXT("Internal hash value handle"),
    OTEXT("Internal thread key handle"),
    OTEXT("Internal Oracle date handle"),
    OTEXT("Internal C time structure"),
    OTEXT("Internal array of resultset handles"),
    OTEXT("Internal array of PL/SQL sizes integers"),
    OTEXT("Internal array of PL/SQL return codes integers"),
    OTEXT("Internal server output handle"),
    OTEXT("Internal array of indicator integers"),
    OTEXT("Internal array of buffer length integers"),
    OTEXT("Internal array of data buffers"),
    OTEXT("Internal Long handle data buffer"),
    OTEXT("Internal trace info structure"),
    OTEXT("Internal array of direct path columns"),
    OTEXT("Internal array of batch error objects"),
    OTEXT("Internal statement array"),
    OTEXT("JSON handle"),
    OTEXT("JSON node handle"),
    OTEXT("Sharding key handle")
};

#if defined(OCI_CHARSET_WIDE) && !defined(_MSC_VER)

static const otext * ErrorMessages[OCI_ERR_COUNT] =
{
    OTEXT("No error"),
    OTEXT("OCILIB has not been initialized"),
    OTEXT("Cannot load OCI shared library (%ls)"),
    OTEXT("Cannot load OCI symbols from shared library"),
    OTEXT("OCILIB has not been initialized in multi threaded mode"),
    OTEXT("Memory allocation failure (type %ls, size : %zu)"),
    OTEXT("Feature not available (%ls) "),
    OTEXT("A null %ls has been provided"),
    OTEXT("Oracle data type (sql code %d) not supported for this operation "),
    OTEXT("Unknown identifier %c while parsing SQL"),
    OTEXT("Unknown argument %d while retrieving data"),
    OTEXT("Index %d out of bounds"),
    OTEXT("Found %d non freed %ls"),
    OTEXT("Maximum number of binds (%d) already reached"),
    OTEXT("Object attribute '%ls' not found"),
    OTEXT("The integer parameter value must be at least %d"),
    OTEXT("Elements are not compatible"),
    OTEXT("The statement must be %ls to perform this operation"),
    OTEXT("The statement is not scrollable"),
    OTEXT("Name or position '%ls' already binded to the statement"),
    OTEXT("Invalid new size for bind arrays (initial %d, current %d, new %d)"),
    OTEXT("Column '%ls' not find in table '%ls'"),
    OTEXT("Unable to perform this operation on a %ls direct path process"),
    OTEXT("Cannot create OCI environment"),
    OTEXT("Name or position '%ls' previously binded with different data type"),
    OTEXT("Object '%ls' type does not match the requested object type"),
    OTEXT("Item '%ls' (type %d)  not found"),
    OTEXT("Argument '%ls' : Invalid value %d"),
    OTEXT("Binding '%ls': Passing non NULL host variable is not allowed when bind allocation mode is internal"),
    OTEXT("Found %d non freed allocated bytes"),
    OTEXT("Mutex failure")
};

#else

static const otext * ErrorMessages[OCI_ERR_COUNT] =
{
    OTEXT("No error"),
    OTEXT("OCILIB has not been initialized"),
    OTEXT("Cannot load OCI shared library (%s)"),
    OTEXT("Cannot load OCI symbols from shared library"),
    OTEXT("OCILIB has not been initialized in multi threaded mode"),
    OTEXT("Memory allocation failure (type %s, size : %zu)"),
    OTEXT("Feature not available (%s) "),
    OTEXT("A null %s has been provided"),
    OTEXT("Oracle data type (sql code %d) not supported for this operation "),
    OTEXT("Unknown identifier %c while parsing SQL : "),
    OTEXT("Unknown argument %d while retrieving data"),
    OTEXT("Index %d out of bounds"),
    OTEXT("Found %d non freed %s"),
    OTEXT("Maximum number of binds (%d) already reached"),
    OTEXT("Object attribute '%s' not found"),
    OTEXT("The integer parameter value must be at least %d"),
    OTEXT("Elements are not compatible"),
    OTEXT("The statement must be %s to perform this operation"),
    OTEXT("The statement is not scrollable"),
    OTEXT("Name or position '%s' already binded to the statement"),
    OTEXT("Invalid new size for bind arrays (initial %d, current %d, new %d)"),
    OTEXT("Column '%s' not find in table '%s'"),
    OTEXT("Unable to perform this operation on a %s direct path process"),
    OTEXT("Cannot create OCI environment"),
    OTEXT("Name or position '%s' previously binded with different datatype"),
    OTEXT("Object '%s' type does not match the requested object type"),
    OTEXT("Item '%s' (type %d)  not found"),
    OTEXT("Argument '%s' : Invalid value %d"),
    OTEXT("Binding '%s': Passing non NULL host variable is not allowed when bind allocation mode is internal"),
    OTEXT("Found %d non freed allocated bytes"),
    OTEXT("Mutex failure")
};

#endif

static const otext * OracleFeatures[OCI_FEATURE_COUNT] =
{
    OTEXT("Oracle 9.0 support for Unicode data"),
    OTEXT("Oracle 9.0 Timestamps and Intervals"),
    OTEXT("Oracle 9.2 Direct path date caching"),
    OTEXT("Oracle 9.2 Statement caching"),
    OTEXT("Oracle 10g R1 LOBs size extensions"),
    OTEXT("Oracle 10g R2 Database change notification"),
    OTEXT("Oracle 10g R2 remote database startup/shutdown"),
    OTEXT("Oracle 10g R2 High Availability"),
    OTEXT("Oracle 12c R1 PL/SQL extended support"),
    OTEXT("Oracle 8.1 multihreading support"),
    OTEXT("Oracle 23ai Vectors support"),
    OTEXT("Oracle 18.1 SODA support"),
    OTEXT("Oracle 21.1 JSON support"),
    OTEXT("Oracle 19.4 client-side secure notification mode"),
    OTEXT("Oracle 12c R2 sharding support")
};

typedef struct StatementState
{
    OCI_Int32    state;
    const otext *name;
} StatementState;

static const StatementState StatementStates[OCI_STMT_STATES_COUNT] =
{
    { OCI_STMT_CLOSED,    OTEXT("closed")         },
    { OCI_STMT_PARSED,    OTEXT("parsed")         },
    { OCI_STMT_PREPARED,  OTEXT("prepared")       },
    { OCI_STMT_DESCRIBED, OTEXT("described")      },
    { OCI_STMT_EXECUTED,  OTEXT("executed")       }
};

static const otext * DirPathStates[OCI_DPS_COUNT] =
{
    OTEXT("non prepared"),
    OTEXT("prepared"),
    OTEXT("converted"),
    OTEXT("terminated")
};

static const otext * HandleNames[OCI_HDLE_COUNT] =
{
    OTEXT("OCI handle"),
    OTEXT("OCI descriptors"),
    OTEXT("OCI Object handles")
};

#define EXCEPTION_IMPL(err_code, ...)                       \
                                                            \
        OCI_Error *err = OcilibExceptionGetError(ctx->env); \
        if (err)                                            \
        {                                                   \
            otext message[512];                             \
            OcilibStringFormat                              \
            (                                               \
    message, osizeof(message) - (OCI_Size)1,                \
    ErrorMessages[err_code],                                \
    __VA_ARGS__                                             \
            );                                              \
                                                            \
            OcilibErrorSet                                  \
            (                                               \
    err,                                                    \
    OCI_ERR_TYPE_ERROR,                                     \
    OCI_ERR_CAT_LIB,                                        \
    (OCI_Int32)(err_code),                                  \
    ctx->source_ptr,                                        \
    ctx->source_type,                                       \
    ctx->location,                                          \
    message,                                                \
    0                                                       \
            );                                              \
                                                            \
            OcilibExceptionCallHandler(err);                \
        }                                                   \


#define EXCEPTION_IMPL_NO_ARGS(err_code)                    \
                                                            \
        OCI_Error *err = OcilibExceptionGetError(ctx->env); \
        if (err)                                            \
        {                                                   \
            otext message[512];                             \
            OcilibStringFormat                              \
            (                                               \
    message,                                                \
    osizeof(message) - (OCI_Size)1,                         \
    OTEXT("%s"),                                            \
    ErrorMessages[err_code]                                 \
            );                                              \
                                                            \
            OcilibErrorSet                                  \
            (                                               \
    err,                                                    \
    OCI_ERR_TYPE_ERROR,                                     \
    OCI_ERR_CAT_LIB,                                        \
    (OCI_Int32)(err_code),                                  \
    ctx->source_ptr,                                        \
    ctx->source_type,                                       \
    ctx->location,                                          \
    message,                                                \
    0                                                       \
            );                                              \
                                                            \
            OcilibExceptionCallHandler(err);                \
        }                                                   \


/* --------------------------------------------------------------------------------------------- *
 * OcilibExceptionGetError
 * --------------------------------------------------------------------------------------------- */

OCI_Error * OcilibExceptionGetError
(
    OCI_Environment *env
)
{
    return OcilibErrorGet(env, TRUE, TRUE);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibExceptionCallHandler
 * --------------------------------------------------------------------------------------------- */

static void OcilibExceptionCallHandler
(
    OCI_Error *err
)
{
    if (err)
    {
        OCI_LogCategory log_cat = OCI_LOG_CAT_LIB;
        OCI_LogLevel    level   = OCI_LOG_ERROR;
        const otext    *label   = OTEXT("OCILIB");
        const otext    *type    = OTEXT("error");

        switch (err->category)
        {
            case OCI_ERR_CAT_OCI:
            {
                log_cat = OCI_LOG_CAT_LIB;
                label   = OTEXT("OCI");
                break;
            }
            case OCI_ERR_CAT_JZN:
            {
                log_cat = OCI_LOG_CAT_LIB;
                label   = OTEXT("JZN");
                break;
            }
            case OCI_ERR_CAT_LIB:
            default:
            {
                log_cat = OCI_LOG_CAT_LIB;
                label   = OTEXT("OCILIB");
                break;
            }
        }

        if (OCI_ERR_TYPE_WARNING == err->type)
        {
            level = OCI_LOG_WARN;
            type  = OTEXT("warning");
        }

        OCILIB_LOG_WITH_ENV(err->env, log_cat, level,
                            __func__,
                            OTEXT("%s: %s %s %d: %s"),
                            err->location ? err->location : OTEXT(""),
                            label, type,
                            (OCI_Int32) err->code,
                            err->message ? err->message : OTEXT(""));

        err->active = TRUE;

        if (err->env != NULL && err->env->error_handler != NULL)
        {
            err->env->error_handler(err);
        }

        err->active = FALSE;
    }
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibExceptionOCI
 * --------------------------------------------------------------------------------------------- */

void OcilibExceptionJson
(
    OCI_Context *ctx,
    JsonDomDoc  *jdoc
)
{
    OCI_Error *err = OcilibExceptionGetError(ctx->env);
    if (err && jdoc)
    {
        sword jerr                    = jdoc->errcode_jznDomDoc;
        char *msg                     = NULL;
        otext buffer[OCI_SIZE_BUFFER] = OTEXT("");

        if (jdoc->mthds_jznDomDoc && jdoc->mthds_jznDomDoc->jznDomGetError)
        {
            msg = (char*) jdoc->mthds_jznDomDoc->jznDomGetError(jdoc, &jerr, TRUE);
        }

        if (NULL != msg && 0 != *msg)
        {
            OCI_Int32 len = min((OCI_Int32)strlen(msg), (OCI_Int32)OCI_SIZE_BUFFER - 1);
            OcilibStringAnsiToNative((const char *)msg, buffer, len);
        }
        else
        {
            OcilibStringFormat(buffer, osizeof(buffer) - (OCI_Size)1,
                               OTEXT("JSON DOM error [%d]"), (OCI_Int32)jerr);
        }

        OcilibErrorSet(err, OCI_ERR_TYPE_ERROR, OCI_ERR_CAT_JZN, (OCI_ErrorCode)jerr,
                       ctx->source_ptr, ctx->source_type, ctx->location,
                       buffer, (OCI_UInt32)0);

        OcilibExceptionCallHandler(err);
    }
}

void OcilibExceptionOCI
(
    OCI_Context *ctx,
    OCIError   * oci_err,
    sword        call_ret
)
{
    OCI_Error *err = OcilibExceptionGetError(ctx->env);
    if (err)
    {
        sb4               err_code                = 0;
        otext             buffer[OCI_SIZE_BUFFER] = OTEXT("");
        OCI_Int32         err_size                = (OCI_Int32)OCI_SIZE_BUFFER* (OCI_Int32)sizeof(otext);
        const OCI_Boolean warning                 = OCI_SUCCESS_WITH_INFO == call_ret;

        dbtext * err_msg = OcilibStringGetDBString(ctx->env, buffer, &err_size);

        buffer[0] = 0;

        ctx->env->api->OCIErrorGet((dvoid *)oci_err, (ub4)1, (OraText *)NULL, &err_code,
                                   (OraText *)err_msg, (ub4)err_size, (ub4)OCI_HTYPE_ERROR);

        OcilibStringCopyDBStringToNativeString(ctx->env, err_msg, buffer, dbcharcount(err_size));

        if (err_code == 0 && err_msg[0] == 0)
        {
            /* for some OCI call might return an error but OCIErrorGet() not giving more
             * information. thus let's provide a message in this case known OCI errors */

            switch (call_ret)
            {
                case OCI_SUCCESS:
                    ostrcpy(buffer, OTEXT("Oracle Client error: OCI_SUCCESS"));
                    break;
                case OCI_SUCCESS_WITH_INFO:
                    ostrcpy(buffer, OTEXT("Oracle Client error: OCI_SUCCESS_WITH_INFO"));
                    break;
                case OCI_ERROR:
                    ostrcpy(buffer, OTEXT("Oracle Client error: OCI_ERROR"));
                    break;
                case OCI_INVALID_HANDLE:
                    ostrcpy(buffer, OTEXT("Oracle Client error: OCI_INVALID_HANDLE"));
                    break;
                case OCI_NEED_DATA:
                    ostrcpy(buffer, OTEXT("Oracle Client error: OCI_NEED_DATA"));
                    break;
                case OCI_STILL_EXECUTING:
                    ostrcpy(buffer, OTEXT("Oracle Client error: OCI_STILL_EXECUTING"));
                    break;
                default:
                    OcilibStringFormat(buffer, osizeof(buffer) - (OCI_Size)1,
                                       OTEXT("Oracle Client error: OCI error code [%d]"), call_ret);
                    break;
            }
        }

        OcilibErrorSet
        (
            err,
            (warning ? OCI_ERR_TYPE_WARNING : OCI_ERR_TYPE_ERROR),
            OCI_ERR_CAT_OCI,
            (OCI_ErrorCode)err_code,
            ctx->source_ptr,
            ctx->source_type,
            ctx->location,
            buffer,
            0
        );

        OcilibStringReleaseDBString(ctx->env, err_msg);

        OcilibExceptionCallHandler(err);
    }
}

#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
#endif

/* --------------------------------------------------------------------------------------------- *
 * OcilibExceptionNotInitialized
 * --------------------------------------------------------------------------------------------- */

void OcilibExceptionNotInitialized
(
    OCI_Context* ctx
)
{
    EXCEPTION_IMPL_NO_ARGS(OCI_ERR_NOT_INITIALIZED);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibExceptionLoadingShareLib
 * --------------------------------------------------------------------------------------------- */

void OcilibExceptionLoadingSharedLib
(
    OCI_Context* ctx
)
{
#ifdef OCI_IMPORT_RUNTIME

    EXCEPTION_IMPL(OCI_ERR_LOADING_SHARED_LIB, OCI_DL_NAME);
#else

    OCI_NOT_USED(ctx);

#endif
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibExceptionLoadingSymbols
 * --------------------------------------------------------------------------------------------- */

void OcilibExceptionLoadingSymbols
(
    OCI_Context* ctx
)
{
    EXCEPTION_IMPL_NO_ARGS(OCI_ERR_LOADING_SYMBOLS);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibExceptionNotMultithreaded
 * --------------------------------------------------------------------------------------------- */

void OcilibExceptionNotMultithreaded
(
    OCI_Context* ctx
)
{
    EXCEPTION_IMPL_NO_ARGS(OCI_ERR_MULTITHREADED);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibExceptionMutex
 * --------------------------------------------------------------------------------------------- */

void OcilibExceptionMutex
(
    OCI_Context* ctx
)
{
    EXCEPTION_IMPL_NO_ARGS(OCI_ERR_MUTEX);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibExceptionNullPointer
 * --------------------------------------------------------------------------------------------- */

void OcilibExceptionNullPointer
(
    OCI_Context   * ctx,
    OCI_PointerType type
)
{
    EXCEPTION_IMPL(OCI_ERR_NULL_POINTER, TypeNames[type]);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibExceptionMemory
 * --------------------------------------------------------------------------------------------- */

void OcilibExceptionMemory
(
    OCI_Context   * ctx,
    OCI_PointerType type,
    OCI_Size        nb_bytes
)
{
    EXCEPTION_IMPL(OCI_ERR_MEMORY, TypeNames[type], nb_bytes);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibExceptionNotAvailable
 * --------------------------------------------------------------------------------------------- */

void OcilibExceptionNotAvailable
(
    OCI_Context* ctx,
    OCI_Int32    feature
)
{
    EXCEPTION_IMPL(OCI_ERR_NOT_AVAILABLE, OracleFeatures[feature - 1]);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibExceptionDatatypeNotSupported
 * --------------------------------------------------------------------------------------------- */

void OcilibExceptionDatatypeNotSupported
(
    OCI_Context* ctx,
    OCI_Int32    code
)
{
    EXCEPTION_IMPL(OCI_ERR_DATATYPE_NOT_SUPPORTED, code);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibExceptionParsingError
 * --------------------------------------------------------------------------------------------- */

void OcilibExceptionParsingToken
(
    OCI_Context* ctx,
    otext        token
)
{
    EXCEPTION_IMPL(OCI_ERR_PARSE_TOKEN, token);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibExceptionMappingArgument
 * --------------------------------------------------------------------------------------------- */

void OcilibExceptionMappingArgument
(
    OCI_Context* ctx,
    OCI_Int32    arg
)
{
    EXCEPTION_IMPL(OCI_ERR_MAP_ARGUMENT, arg);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibExceptionOutOfBounds
 * --------------------------------------------------------------------------------------------- */

void OcilibExceptionOutOfBounds
(
    OCI_Context* ctx,
    OCI_Int32    value
)
{
    EXCEPTION_IMPL(OCI_ERR_OUT_OF_BOUNDS, value);
}

/* --------------------------------------------------------------------------------------------- *
* OcilibExceptionUnfreedData
* --------------------------------------------------------------------------------------------- */

void OcilibExceptionUnfreedData
(
    OCI_Context* ctx,
    OCI_Int32    type_elem,
    OCI_Int32    nb_elem
)
{
    EXCEPTION_IMPL(OCI_ERR_UNFREED_DATA, nb_elem, HandleNames[type_elem - 1]);
}

/* --------------------------------------------------------------------------------------------- *
* OcilibExceptionUnfreedBytes
* --------------------------------------------------------------------------------------------- */

void OcilibExceptionUnfreedBytes
(
    OCI_Context* ctx,
    OCI_UInt64   nb_bytes
)
{
    EXCEPTION_IMPL(OCI_ERR_UNFREED_BYTES, nb_bytes);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibExceptionRuntimeLoading
 * --------------------------------------------------------------------------------------------- */

void OcilibExceptionMaxBind
(
    OCI_Context* ctx
)
{
    EXCEPTION_IMPL(OCI_ERR_MAX_BIND, OCI_BIND_MAX);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibExceptionAttributeNotFound
 * --------------------------------------------------------------------------------------------- */

void OcilibExceptionAttributeNotFound
(
    OCI_Context* ctx,
    const otext *attr
)
{
    EXCEPTION_IMPL(OCI_ERR_ATTR_NOT_FOUND, attr);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibExceptionMinimumValue
 * --------------------------------------------------------------------------------------------- */

void OcilibExceptionMinimumValue
(
    OCI_Context* ctx,
    OCI_Int32    min
)
{
    EXCEPTION_IMPL(OCI_ERR_MIN_VALUE, min);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibExceptionTypeNotCompatible
 * --------------------------------------------------------------------------------------------- */

void OcilibExceptionTypeNotCompatible
(
    OCI_Context* ctx
)
{
    EXCEPTION_IMPL_NO_ARGS(OCI_ERR_NOT_COMPATIBLE);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibExceptionStatementState
 * --------------------------------------------------------------------------------------------- */

void OcilibExceptionStatementState
(
    OCI_Context* ctx,
    OCI_Int32    state
)
{
    OCI_Int32 i = 0, index = 0;

    for (; i < OCI_STMT_STATES_COUNT; i++)
    {
        if (state == StatementStates[i].state)
        {
            index = i;
            break;
        }
    }

    EXCEPTION_IMPL(OCI_ERR_STMT_STATE, StatementStates[index].name);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibExceptionStatementNotScrollable
 * --------------------------------------------------------------------------------------------- */

void OcilibExceptionStatementNotScrollable
(
    OCI_Context* ctx
)
{
    EXCEPTION_IMPL_NO_ARGS(OCI_ERR_STMT_NOT_SCROLLABLE);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibExceptionBindAlreadyUsed
 * --------------------------------------------------------------------------------------------- */

void OcilibExceptionBindAlreadyUsed
(
    OCI_Context * ctx,
    const otext * bind
)
{
    EXCEPTION_IMPL(OCI_ERR_BIND_ALREADY_USED, bind);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibExceptionBindArraySize
 * --------------------------------------------------------------------------------------------- */

void OcilibExceptionBindArraySize
(
    OCI_Context* ctx,
    OCI_UInt32   maxsize,
    OCI_UInt32   cursize,
    OCI_UInt32   newsize
)
{
    EXCEPTION_IMPL(OCI_ERR_BIND_ARRAY_SIZE, maxsize, cursize, newsize);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibExceptionDirPathColNotFound
 * --------------------------------------------------------------------------------------------- */

void OcilibExceptionDirPathColNotFound
(
    OCI_Context * ctx,
    const otext * column,
    const otext  *table
)
{
    EXCEPTION_IMPL(OCI_ERR_COLUMN_NOT_FOUND, column, table);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibExceptionDirPathState
 * --------------------------------------------------------------------------------------------- */

void OcilibExceptionDirPathState
(
    OCI_Context* ctx,
    OCI_Int32    state
)
{
    EXCEPTION_IMPL(OCI_ERR_DIRPATH_STATE, DirPathStates[state - 1]);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibExceptionOCIEnvironment
 * --------------------------------------------------------------------------------------------- */

void OcilibExceptionOCIEnvironment
(
    OCI_Context* ctx
)
{
    EXCEPTION_IMPL_NO_ARGS(OCI_ERR_CREATE_OCI_ENVIRONMENT);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibExceptionRebindBadDatatype
 * --------------------------------------------------------------------------------------------- */

void OcilibExceptionRebindBadDatatype
(
    OCI_Context * ctx,
    const otext * bind
)
{
    EXCEPTION_IMPL(OCI_ERR_REBIND_BAD_DATATYPE, bind);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibExceptionTypeInfoWrongType
 * --------------------------------------------------------------------------------------------- */

void OcilibExceptionTypeInfoWrongType
(
    OCI_Context * ctx,
    const otext * name
)
{
    EXCEPTION_IMPL(OCI_ERR_TYPEINFO_DATATYPE, name);
}

/* --------------------------------------------------------------------------------------------- *
* OcilibExceptionItemNotFound
* --------------------------------------------------------------------------------------------- */

void OcilibExceptionItemNotFound
(
    OCI_Context   * ctx,
    const otext    *name,
    OCI_PointerType type
)
{
    EXCEPTION_IMPL(OCI_ERR_ITEM_NOT_FOUND, name, type);
}

/* --------------------------------------------------------------------------------------------- *
* OcilibExceptionArgInvalidValue
* --------------------------------------------------------------------------------------------- */

void OcilibExceptionArgInvalidValue
(
    OCI_Context* ctx,
    const otext *name,
    OCI_UInt32   value
)
{
    EXCEPTION_IMPL(OCI_ERR_ARG_INVALID_VALUE, name, value);
}

/* --------------------------------------------------------------------------------------------- *
* OcilibExceptionExternalBindingNotAllowed
* --------------------------------------------------------------------------------------------- */

void OcilibExceptionExternalBindingNotAllowed
(
    OCI_Context* ctx,
    const otext *bind
)
{
    EXCEPTION_IMPL(OCI_ERR_BIND_EXTERNAL_NOT_ALLOWED, bind);
}

#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif
