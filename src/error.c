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

#include "error.h"

#include "macros.h"
#include "stringutils.h"
#include <threads.h>

/* Upper bound applied to the location and message parts of an error string.
    Prevents overflow of the length arithmetic in OcilibErrorSet() and of the
    OCI_UInt32 capacities cached on OCI_Error. */

#define OCI_ERR_TEXT_MAX_LEN ((OCI_Size) 65536)

static tss_t     error_tss;
static once_flag error_tss_once      = ONCE_FLAG_INIT;
static OCI_Int32 error_tss_status    = thrd_error;
static OCI_Int32 error_tss_ref_count = 0;

/* --------------------------------------------------------------------------------------------- *
 * OcilibErrorTssDtor
 * --------------------------------------------------------------------------------------------- */

static void OcilibErrorTssDtor
(
    void* value
)
{
    if (value != NULL)
    {
        OcilibErrorFree((OCI_Error*)value);
    }
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibErrorTssCreate
 * --------------------------------------------------------------------------------------------- */

static void OcilibErrorTssCreate
(
    void
)
{
    error_tss_status = tss_create(&error_tss, OcilibErrorTssDtor);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibErrorInitialize
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibErrorInitialize
(
    void
)
{
    call_once(&error_tss_once, OcilibErrorTssCreate);

    error_tss_ref_count++;

    return error_tss_status == thrd_success;
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibErrorCleanup
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibErrorCleanup
(
    void
)
{
    if (--error_tss_ref_count == 0)
    {
        OCI_Error* err = (OCI_Error*)tss_get(error_tss);

        if (err != NULL)
        {
            tss_set(error_tss, NULL);
            OcilibErrorFree(err);
        }
    }

    return TRUE;
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibErrorCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Error* OcilibErrorCreate
(
    OCI_Environment* env
)
{
    OCI_Error* err = (OCI_Error*)calloc(1, sizeof(OCI_Error));

    if (err != NULL)
    {
        err->env   = env;
        err->htype = OCI_IPC_ERROR;
    }

    return err;
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibErrorFree
 * --------------------------------------------------------------------------------------------- */

void OcilibErrorFree
(
    OCI_Error* err
)
{
    if (NULL != err)
    {
        if (NULL != err->message)
        {
            free(err->message);
        }

        if (NULL != err->location)
        {
            free(err->location);
        }

        free(err);
    }
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibErrorReset
 * --------------------------------------------------------------------------------------------- */

void OcilibErrorReset
(
    OCI_Error* err
)
{
    if (err)
    {
        err->active      = FALSE;
        err->source_ptr  = NULL;
        err->source_type = OCI_UNKNOWN;
        err->type        = OCI_UNKNOWN;
        err->code        = 0;
        err->row         = 0;

        if (NULL != err->message)
        {
            err->message[0] = 0;
        }

        if (NULL != err->location)
        {
            err->location[0] = 0;
        }
    }
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibErrorGet
 * --------------------------------------------------------------------------------------------- */

#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
#endif

void OcilibErrorSet
(
    OCI_Error     * err,
    OCI_ErrorType   type,
    OCI_ErrorCode   code,
    void          * source_ptr,
    OCI_PointerType source_type,
    const char    * location,
    otext         * message,
    OCI_UInt32      row
)
{
    err->type        = type;
    err->code        = code;
    err->source_ptr  = source_ptr;
    err->source_type = source_type;
    err->row         = row;
    err->warnings_on = (NULL != err->env) ? err->env->warnings_on : FALSE;

#if defined(OCI_CHARSET_WIDE) && !defined(_WINDOWS)
    const otext* format = OTEXT("Error occurred at %ls: %ls");
#else
    const otext* format = OTEXT("Error occurred at %s: %s");
#endif

    const OCI_Size format_len = ostrlen(format);

    OCI_Size message_len  = message ? ostrlen(message) : 0;
    OCI_Size location_len = location ? strlen(location) : 0;

    /* Errors are diagnostics, not payloads: bounding both inputs keeps the
       sum below from overflowing and keeps the cached capacities (which are
       OCI_UInt32) from being truncated. */

    if (message_len > OCI_ERR_TEXT_MAX_LEN)
    {
        message_len = OCI_ERR_TEXT_MAX_LEN;
    }

    if (location_len > OCI_ERR_TEXT_MAX_LEN)
    {
        location_len = OCI_ERR_TEXT_MAX_LEN;
    }

    /* Capacities actually available, in otext elements, excluding the
       terminating NUL. These must never be assumed to have grown: when
       realloc() fails the previous, smaller buffer is still what we hold. */

    OCI_Size location_capacity = err->location_len;
    OCI_Size message_capacity  = err->message_len;

    /* allocate storage for location */

    if (location_capacity < location_len)
    {
        otext* buffer = realloc(err->location, (location_len + 1) * sizeof(otext));

        if (buffer != NULL)
        {
            err->location     = buffer;
            location_capacity = location_len;
        }
    }

    /* convert location if needed, never beyond the real capacity */

    if (err->location != NULL)
    {
        if (location_len > location_capacity)
        {
            location_len = location_capacity;
        }

        if (location_len > 0)
        {
            OcilibStringAnsiToNative(location, err->location, (OCI_Int32)location_len);
        }
        else
        {
            err->location[0] = 0;
        }
    }
    else
    {
        location_len = 0;
    }

    err->location_len = (OCI_UInt32)location_capacity;

    /* allocate storage for message */

    const OCI_Size total_len = format_len + message_len + location_len;

    if (message_capacity < total_len)
    {
        otext* buffer = realloc(err->message, (total_len + 1) * sizeof(otext));

        if (buffer != NULL)
        {
            err->message     = buffer;
            message_capacity = total_len;
        }
    }

    /* format message, bounded by the capacity we actually have rather than by
       the capacity we asked for */

    if (err->message != NULL && message_capacity > 0)
    {
        OcilibStringFormat(err->message, (OCI_Int32)message_capacity, format,
                           err->location != NULL ? err->location : OTEXT(""),
                           message != NULL ? message : OTEXT(""));
    }

    err->message_len = (OCI_UInt32)message_capacity;
}

#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif


/* --------------------------------------------------------------------------------------------- *
 * OcilibErrorResetSource
 * --------------------------------------------------------------------------------------------- */

void OcilibErrorResetSource
(
    OCI_Error* err,
    void     * source_ptr
)
{
    if (err != NULL && err->source_ptr == source_ptr)
    {
        err->source_ptr  = NULL;
        err->source_type = OCI_UNKNOWN;
    }
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibErrorGet
 * --------------------------------------------------------------------------------------------- */

OCI_Error* OcilibErrorGet
(
    OCI_Environment* env,
    OCI_Boolean      check_state,
    OCI_Boolean      reset_err
)
{
    OCI_Error* err = NULL;

    call_once(&error_tss_once, OcilibErrorTssCreate);

    err = (OCI_Error*)tss_get(error_tss);

    if (err == NULL)
    {
        err = OcilibErrorCreate(env);

        if (err != NULL)
        {
            tss_set(error_tss, err);
        }
    }

    if (err != NULL && check_state && err->active)
    {
        err = NULL;
    }

    if (err != NULL && reset_err)
    {
        OcilibErrorReset(err);
        err->env = env;
    }

    return err;
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibErrorGetLastError
 * --------------------------------------------------------------------------------------------- */

OCI_Error* OcilibErrorGetLastError
(
    void
)
{
    OCI_Error* err = OcilibErrorGet(NULL, TRUE, FALSE);

    if (NULL != err && OCI_UNKNOWN == err->code)
    {
        err = NULL;
    }

    return err;
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibErrorGetString
 * --------------------------------------------------------------------------------------------- */

const otext* OcilibErrorGetString
(
    OCI_Error* err
)
{
    if (NULL == err) return (NULL);

    return err->message;
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibErrorGetType
 * --------------------------------------------------------------------------------------------- */

OCI_ErrorType OcilibErrorGetType
(
    OCI_Error* err
)
{
    if (NULL == err) return (OCI_UNKNOWN);

    /* warnings are reported only when they were enabled on the environment
       that raised the error (snapshotted at raise time -- the environment may
       have been freed since) */

    if (OCI_ERR_WARNING == err->type && !err->warnings_on)
    {
        return OCI_UNKNOWN;
    }

    return err->type;
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibErrorGetOCICode
 * --------------------------------------------------------------------------------------------- */

OCI_ErrorCode OcilibErrorGetOCICode
(
    OCI_Error* err
)
{
    if (NULL == err) return (OCI_UNKNOWN);

    return err->type == OCI_ERR_OCILIB ? 0 : err->code;
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibErrorGetInternalCode
 * --------------------------------------------------------------------------------------------- */

OCI_ErrorCode OcilibErrorGetInternalCode
(
    OCI_Error* err
)
{
    if (NULL == err) return (0);

    return err->type == OCI_ERR_OCILIB ? err->code : 0;
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibErrorGetSource
 * --------------------------------------------------------------------------------------------- */

void* OcilibErrorGetSource
(
    OCI_Error* err
)
{
    if (NULL == err) return (NULL);

    return err->source_ptr;

}

/* --------------------------------------------------------------------------------------------- *
 * OcilibErrorGetSourceType
 * --------------------------------------------------------------------------------------------- */

OCI_PointerType OcilibErrorGetSourceType
(
    OCI_Error* err
)
{
    if (NULL == err) return (OCI_UNKNOWN);

    return err->source_type;
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibErrorGetLocation
 * --------------------------------------------------------------------------------------------- */

const otext* OcilibErrorGetLocation
(
    OCI_Error* err
)
{
    if (NULL == err) return (NULL);

    return err->location;

}

/* --------------------------------------------------------------------------------------------- *
 * OcilibErrorGetConnection
 * --------------------------------------------------------------------------------------------- */

OCI_Connection* OcilibErrorGetConnection
(
    OCI_Error* err
)
{
    if (NULL == err) return (NULL);

    if (NULL == err->source_ptr)
    {
        return NULL;
    }

    switch (err->source_type)
    {
        case OCI_IPC_TYPE_INFO:
            return ((OCI_TypeInfo*)err->source_ptr)->con;
        case OCI_IPC_CONNECTION:
            return (OCI_Connection*)err->source_ptr;
        case OCI_IPC_TRANSACTION:
            return ((OCI_Transaction*)err->source_ptr)->con;
        case OCI_IPC_STATEMENT:
            return ((OCI_Statement*)err->source_ptr)->con;
        case OCI_IPC_RESULTSET:
            return ((OCI_Resultset*)err->source_ptr)->stmt->con;
        case OCI_IPC_DATE:
            return NULL;
        case OCI_IPC_TIMESTAMP:
            return NULL;
        case OCI_IPC_INTERVAL:
            return NULL;
        case OCI_IPC_LOB:
            return ((OCI_Lob*)err->source_ptr)->con;
        case OCI_IPC_FILE:
            return ((OCI_File*)err->source_ptr)->con;
        case OCI_IPC_LONG:
            return ((OCI_Long*)err->source_ptr)->handle->con;
        case OCI_IPC_OBJECT:
            return ((OCI_Object*)err->source_ptr)->con;
        case OCI_IPC_COLLECTION:
            return ((OCI_Coll*)err->source_ptr)->con;
        case OCI_IPC_ITERATOR:
            return ((OCI_Iter*)err->source_ptr)->coll->con;
        case OCI_IPC_ELEMENT:
            return ((OCI_Elem*)err->source_ptr)->typinf->con;
        case OCI_IPC_NUMBER:
            return NULL;
        case OCI_IPC_BIND:
            return ((OCI_Bind*)err->source_ptr)->stmt->con;
        case OCI_IPC_REF:
            return ((OCI_Ref*)err->source_ptr)->con;
        case OCI_IPC_DIRPATH:
            return ((OCI_DirPath*)err->source_ptr)->con;
        case OCI_IPC_MSG:
            return ((OCI_Msg*)err->source_ptr)->typinf->con;
        case OCI_IPC_ENQUEUE:
            return ((OCI_Enqueue*)err->source_ptr)->typinf->con;
        case OCI_IPC_DEQUEUE:
            return ((OCI_Dequeue*)err->source_ptr)->typinf->con;
        case OCI_IPC_AGENT:
            return ((OCI_Agent*)err->source_ptr)->con;
        case OCI_IPC_XMLTYPE:
            return ((OCI_XmlType*)err->source_ptr)->con;
        case OCI_IPC_VECTOR:
            return ((OCI_Vector*)err->source_ptr)->con;
    }

    return NULL;
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibErrorGetStatement
 * --------------------------------------------------------------------------------------------- */

OCI_Statement* OcilibErrorGetStatement
(
    OCI_Error* err
)
{
    if (NULL == err) return (NULL);

    if (NULL == err->source_ptr)
    {
        return NULL;
    }

    switch (err->source_type)
    {
        case OCI_IPC_STATEMENT:
            return (OCI_Statement*)err->source_ptr;
        case OCI_IPC_RESULTSET:
            return ((OCI_Resultset*)err->source_ptr)->stmt;
        case OCI_IPC_BIND:
            return ((OCI_Bind*)err->source_ptr)->stmt;
    }

    return NULL;
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibErrorGetRow
 * --------------------------------------------------------------------------------------------- */

OCI_UInt32 OcilibErrorGetRow
(
    OCI_Error* err
)
{
    if (NULL == err) return (0);

    return err->row;
}
