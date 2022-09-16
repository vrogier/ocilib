/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2022 Vincent ROGIER <vince.rogier@ocilib.net>
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
#include "threadkey.h"

/* --------------------------------------------------------------------------------------------- *
 * OcilibErrorCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Error * OcilibErrorCreate
(
    void
)
{
    return (OCI_Error *) calloc(1, sizeof(OCI_Error));
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibErrorFree
 * --------------------------------------------------------------------------------------------- */

void OcilibErrorFree
(
    OCI_Error *err
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
    OCI_Error *err
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

void OcilibErrorSet
(
    OCI_Error   *err,
    unsigned int type,
    int          code,
    void        *source_ptr,
    unsigned int source_type,
    const char  *location,
    otext       *message,
    unsigned int row
)
{
    err->type        = type;
    err->code        = code;
    err->source_ptr  = source_ptr;
    err->source_type = source_type;
    err->row         = row;

#if defined(OCI_CHARSET_WIDE) && !defined(_WINDOWS)
    const otext* format = OTEXT("Error occurred at %ls: %ls");
#else
    const otext* format = OTEXT("Error occurred at %s: %s");
#endif 

    const size_t format_len = ostrlen(format);
    const size_t message_len = message ? ostrlen(message) : 0;
    const size_t location_len = location ? strlen(location) : 0;
    const size_t total_len = format_len + message_len + location_len;

    /* allocate storage for location */
    if (err->location_len < location_len)
    {
        err->location = realloc(err->location, (location_len + 1) * sizeof(otext));
    }

    /* convert location if needed */

    OcilibStringAnsiToNative(location, err->location, (unsigned int) location_len);
    err->location_len = max(err->location_len, (unsigned int) location_len);

    /* allocate storage for message */

    if (err->message_len < total_len)
    {
        err->message = realloc(err->message, (total_len + 1) * sizeof(otext));
    }

    /* format message */

    OcilibStringFormat(err->message, (int)total_len, format, err->location, message);

    err->message_len = max(err->message_len, (unsigned int)total_len);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibErrorResetSource
 * --------------------------------------------------------------------------------------------- */

void OcilibErrorResetSource
(
    OCI_Error* err,
    void* source_ptr
)
{
    if (err == NULL)
    {
        err = OcilibErrorGet(FALSE, FALSE);
    }

    if (err != NULL && err->source_ptr == source_ptr)
    {
        err->source_ptr  = NULL;
        err->source_type = OCI_UNKNOWN;
    }
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibErrorGet
 * --------------------------------------------------------------------------------------------- */

OCI_Error * OcilibErrorGet
(
    boolean check_state,
    boolean reset_err
)
{
    OCI_Error *err = Env.lib_err;

    if (Env.loaded && LIB_THREADED && Env.key_errs != NULL)
    {
        if (OcilibThreadKeyGet(Env.key_errs, (void **)(dvoid *)&err))
        {
            if (NULL == err)
            {
                err = OcilibErrorCreate();

                if (NULL != err)
                {
                    OcilibThreadKeySet(Env.key_errs, err);
                }
            }
        }
    }

    if (err != NULL && check_state && err->active)
    {
        err = NULL;
    }

    if (err != NULL && reset_err)
    {
        OcilibErrorReset(err);
    }

    return err;
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibErrorGetString
 * --------------------------------------------------------------------------------------------- */

const otext * OcilibErrorGetString
(
    OCI_Error *err
)
{
    CHECK_FALSE(NULL == err, NULL);

    return err->message;
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibErrorGetType
 * --------------------------------------------------------------------------------------------- */

unsigned int OcilibErrorGetType
(
    OCI_Error *err
)
{
    CHECK_FALSE(NULL == err, OCI_UNKNOWN);

    return err->type;
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibErrorGetOCICode
 * --------------------------------------------------------------------------------------------- */

int OcilibErrorGetOCICode
(
    OCI_Error *err
)
{
    CHECK_FALSE(NULL == err, OCI_UNKNOWN);

    return err->type == OCI_ERR_OCILIB ? 0 : err->code;
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibErrorGetInternalCode
 * --------------------------------------------------------------------------------------------- */

int OcilibErrorGetInternalCode
(
    OCI_Error *err
)
{
    CHECK_FALSE(NULL == err, 0);

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
    CHECK_FALSE(NULL == err, NULL);

    return err->source_ptr;

}

/* --------------------------------------------------------------------------------------------- *
 * OcilibErrorGetSourceType
 * --------------------------------------------------------------------------------------------- */

unsigned int OcilibErrorGetSourceType
(
    OCI_Error* err
)
{
    CHECK_FALSE(NULL == err, OCI_UNKNOWN);

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
    CHECK_FALSE(NULL == err, NULL);

    return err->location;

}

/* --------------------------------------------------------------------------------------------- *
 * OcilibErrorGetConnection
 * --------------------------------------------------------------------------------------------- */

OCI_Connection * OcilibErrorGetConnection
(
    OCI_Error *err
)
{
    CHECK_FALSE(NULL == err, NULL);

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
            return ((OCI_Date*)err->source_ptr)->con;
        case OCI_IPC_TIMESTAMP:
            return ((OCI_Timestamp*)err->source_ptr)->con;
        case OCI_IPC_INTERVAL:
            return ((OCI_Interval*)err->source_ptr)->con;
        case OCI_IPC_LOB:
            return ((OCI_Lob*)err->source_ptr)->con;
        case OCI_IPC_FILE:
            return ((OCI_File*)err->source_ptr)->con;
        case OCI_IPC_LONG:
            return ((OCI_Long*)err->source_ptr)->stmt->con;
        case OCI_IPC_OBJECT:
            return ((OCI_Statement*)err->source_ptr)->con;
        case OCI_IPC_COLLECTION:
            return ((OCI_Coll*)err->source_ptr)->con;
        case OCI_IPC_ITERATOR:
            return ((OCI_Iter*)err->source_ptr)->coll->con;
        case OCI_IPC_ELEMENT:
            return ((OCI_Elem*)err->source_ptr)->con;
        case OCI_IPC_NUMBER:
            return ((OCI_Number*)err->source_ptr)->con;
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
    }

    return NULL;
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibErrorGetStatement
 * --------------------------------------------------------------------------------------------- */

OCI_Statement * OcilibErrorGetStatement
(
    OCI_Error *err
)
{
    CHECK_FALSE(NULL == err, NULL);

    if (NULL == err->source_ptr)
    {
        return NULL;
    }

    switch (err->source_type)
    {
        case OCI_IPC_STATEMENT:
            return (OCI_Statement*) err->source_ptr;
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

unsigned int OcilibErrorGetRow
(
    OCI_Error *err
)
{
    CHECK_FALSE(NULL == err, 0);

    return err->row;
}
