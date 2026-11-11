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

#include "stringutils.h"

#include "collection.h"
#include "date.h"
#include "environment.h"
#include "error.h"
#include "json.h"
#include "file.h"
#include "interval.h"
#include "lob.h"
#include "long.h"
#include "macros.h"
#include "memory.h"
#include "number.h"
#include "object.h"
#include "reference.h"
#include "timestamp.h"
#include "vector.h"
#include "xmltype.h"

#include <limits.h>

#define COMPUTE_LENGTH(type, ptr, size)       \
        const type *s = (const type *) (ptr); \
        const type *e = (const type *) (ptr); \
        while (*e++);                         \
        (size) = (OCI_Int32)(e - s) - 1;      \


/* --------------------------------------------------------------------------------------------- *
  * OcilibStringLength
  * --------------------------------------------------------------------------------------------- */

OCI_Size OcilibStringLength
(
    const void* ptr,
    OCI_Size    size_elem
)
{
    OCI_Int32 size = 0;

    if (NULL == ptr) return (0);

    if (FALSE)
    {
        const char* s = (char*)ptr;
        while (*s)
        {
            if ((*s & 0xc0) != 0x80) size++;
            s++;
        }
    }
    else if (sizeof(char) == size_elem)
    {
        COMPUTE_LENGTH(char, ptr, size);
    }
    else if (sizeof(OCI_Int16) == size_elem)
    {
        COMPUTE_LENGTH(OCI_Int16, ptr, size);
    }
    else if (sizeof(OCI_Int32) == size_elem)
    {
        COMPUTE_LENGTH(OCI_Int32, ptr, size);
    }

    return (OCI_Size) size;
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStringBinaryToString
 * --------------------------------------------------------------------------------------------- */

OCI_UInt32 OcilibStringBinaryToString
(
    const unsigned char* binary,
    OCI_UInt32           binary_size,
    otext              * buffer
)
{
    char hex_str[] = "0123456789ABCDEF";

    /* Each input byte produces two hex characters, so anything above
       UINT_MAX / 2 would wrap and make `buffer[len] = 0' write out of bounds. */

    if (binary_size > (UINT_MAX - 1) / 2)
    {
        binary_size = (UINT_MAX - 1) / 2;
    }

    const OCI_UInt32 len = binary_size * 2;

    if (buffer)
    {
        for (OCI_UInt32 i = 0; i < binary_size; i++)
        {
            buffer[i * 2 + 0] = hex_str[binary[i] >> 4];
            buffer[i * 2 + 1] = hex_str[binary[i] & 0x0F];
        }

        buffer[len] = 0;
    }

    return len;
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStringRequestBuffer
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibStringRequestBuffer
(
    OCI_Environment* env,
    otext         ** buffer,
    OCI_UInt32     * buffer_size,
    OCI_UInt32       request_size
)
{
    OCI_Boolean res = FALSE;

    if (!buffer || !buffer_size)
    {
        return FALSE;
    }

    const OCI_Size char_size = (env && (TRUE == env->charset))
        ? (OCI_Size)OCI_UTF8_BYTES_PER_CHAR
        : sizeof(otext);

    /* Computed in OCI_Size: doing this in OCI_UInt32 wrapped for large requests
       and produced a buffer far smaller than *buffer_size claimed, which the
       callers then overran. */

    const OCI_Size needed = ((OCI_Size)request_size + 1) * char_size;

    if (needed > (OCI_Size)UINT_MAX)
    {
        return FALSE;
    }

    request_size = (OCI_UInt32)needed;

    if (!*buffer)
    {
        *buffer = (otext*)OcilibMemoryAlloc(env, OCI_IPC_STRING, (OCI_Size)request_size,
                                            (OCI_Size)1, TRUE);
    }
    else if (*buffer_size < request_size)
    {
        *buffer = (otext*)OcilibMemoryRealloc(env, *buffer, OCI_IPC_STRING, (OCI_Size)request_size,
                                              (OCI_Size)1, TRUE);
    }

    if (*buffer)
    {
        *buffer_size = request_size;

        res = TRUE;
    }

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStringTranslate
 * --------------------------------------------------------------------------------------------- */

#define STRING_CVT_EXPAND(type_src, type_dst, char_count)           \
                                                                    \
        const OCI_Int32 len = char_count;                           \
        unsigned type_src* str1 = (unsigned type_src*)src;          \
        unsigned type_dst* str2 = (unsigned type_dst*)dst;          \
                                                                    \
        if (*str1 == 0)                                             \
        {                                                           \
            return;                                                 \
        }                                                           \
                                                                    \
        while (char_count--)                                        \
        {                                                           \
            str2[char_count] = (unsigned type_dst)str1[char_count]; \
        }                                                           \
                                                                    \
        str2[len] = 0;                                              \

#define STRING_CVT_PACK(type_src, type_dst, char_count)    \
                                                           \
        const OCI_Int32 len = char_count;                  \
        unsigned type_src* str1 = (unsigned type_src*)src; \
        unsigned type_dst* str2 = (unsigned type_dst*)dst; \
        OCI_Int32          i    = 0;                       \
                                                           \
        if (*str1 == 0)                                    \
        {                                                  \
            return;                                        \
        }                                                  \
                                                           \
        while (i < char_count)                             \
        {                                                  \
            str2[i] = (unsigned type_dst)str1[i];          \
            i++;                                           \
        }                                                  \
                                                           \
        str2[len] = 0;

void OcilibStringTranslate
(
    void    * src,
    void    * dst,
    OCI_Int32 char_count,
    OCI_Size  size_char_in,
    OCI_Size  size_char_out
)
{
    if (!src || !dst)
    {
        return;
    }

    /* raw string packing/expansion without charset conversion */

    if (size_char_out > size_char_in)
    {
        /* expand string */

        if ((size_char_in == sizeof(OCI_Int16)) && (size_char_out == sizeof(OCI_Int32)))
        {
            /* 2 => 4 bytes */

            STRING_CVT_EXPAND(short, int, char_count);
        }
        else if ((size_char_in == sizeof(char)) && (size_char_out == sizeof(OCI_Int16)))
        {
            /* 1 => 2 bytes */

            STRING_CVT_EXPAND(char, short, char_count);
        }
        else if ((size_char_in == sizeof(char)) && (size_char_out == sizeof(OCI_Int32)))
        {
            /* 1 => 4 bytes */

            STRING_CVT_EXPAND(char, int, char_count);
        }
    }
    else if (size_char_out < size_char_in)
    {
        /* pack string */

        if ((size_char_in == sizeof(OCI_Int32)) && (size_char_out == sizeof(OCI_Int16)))
        {
            /* 4 => 2 bytes */

            STRING_CVT_PACK(int, short, char_count);
        }
        else if ((size_char_in == sizeof(OCI_Int16)) && (size_char_out == sizeof(char)))
        {
            /* 2 => 1 bytes */

            STRING_CVT_PACK(short, char, char_count);

        }
        else if ((size_char_in == sizeof(OCI_Int32)) && (size_char_out == sizeof(char)))
        {
            /* 4 => 1 bytes */

            STRING_CVT_PACK(int, char, char_count);
        }
    }
    else if (src != dst)
    {
        memcpy(dst, src, (OCI_Size) char_count * size_char_out);

        if (size_char_out == sizeof(char))
        {
            ((char*)dst)[char_count] = 0;
        }
        else if (size_char_out == sizeof(OCI_Int16))
        {
            ((OCI_Int16*)dst)[char_count] = 0;
        }
        else if (size_char_out == sizeof(OCI_Int32))
        {
            ((OCI_Int32*)dst)[char_count] = 0;
        }
    }
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStringGetDBString
 * --------------------------------------------------------------------------------------------- */

dbtext* OcilibStringGetDBString
(
    OCI_Environment* env,
    const otext    * src,
    OCI_Int32      * size
)
{
    dbtext  * dst = NULL;
    OCI_Int32 len = 0;

    if (!src)
    {
        *size = 0;
        return dst;
    }

    if (*size == -1)
    {
        COMPUTE_LENGTH(otext, src, len);
    }
    else
    {
        len = (OCI_Int32)((OCI_Int32)(*size) / (OCI_Int32)sizeof(otext));
    }

    if (env && env->use_wide_char_conv)
    {
        dst = (dbtext*)OcilibMemoryAlloc(env, OCI_IPC_STRING, sizeof(dbtext),
                                         (OCI_Size)(len + 1), FALSE);

        if (NULL != dst)
        {
            OcilibStringUTF32ToUTF16(src, dst, len);
        }
    }
    else
    {
        dst = (dbtext*)src;
    }

    *size = (OCI_Size)len * sizeof(dbtext);

    return dst;
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStringReleaseDBString
 * --------------------------------------------------------------------------------------------- */

void OcilibStringReleaseDBString
(
    OCI_Environment* env,
    dbtext         * str
)
{
    if (env && env->use_wide_char_conv && NULL != str)
    {
        OcilibMemoryFree(env, str);
    }
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStringGetDBAnsiString
 * --------------------------------------------------------------------------------------------- */

dbtext* OcilibStringGetDBAnsiString
(
    OCI_Environment* env,
    const otext    * src,
    OCI_Int32      * size
)
{
    dbtext  * dst = NULL;
    OCI_Int32 len = 0;

    if (!src)
    {
        *size = 0;
        return dst;
    }

    if (*size == -1)
    {
        COMPUTE_LENGTH(otext, src, len);
    }
    else
    {
        len = (OCI_Int32)((OCI_Int32)(*size) / (OCI_Int32)sizeof(otext));
    }

    if (env && env->charset == OCI_CHAR_WIDE)
    {
        dst = (dbtext*)OcilibMemoryAlloc(env, OCI_IPC_STRING, sizeof(dbtext),
                                         (OCI_Size)(len + 1), FALSE);

        if (NULL != dst)
        {
            OcilibStringNativeToAnsi(src, dst, len);
        }
    }
    else
    {
        dst = (dbtext*)src;
    }

    *size = (OCI_Size)len * sizeof(dbtext);

    return dst;
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStringReleaseDBAnsiString
 * --------------------------------------------------------------------------------------------- */

void OcilibStringReleaseDBAnsiString
(
    OCI_Environment* env,
    dbtext         * str
)
{
    if (env && env->charset == OCI_CHAR_WIDE && NULL != str)
    {
        OcilibMemoryFree(env, str);
    }
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStringCopyDBStringToNativeString
 * --------------------------------------------------------------------------------------------- */

OCI_Int32 OcilibStringCopyDBStringToNativeString
(
    OCI_Environment* env,
    const dbtext   * src,
    otext          * dst,
    OCI_Int32        len
)
{
    if (env && env->use_wide_char_conv)
    {
        OcilibStringUTF16ToUTF32((void*)src, (void*)dst, len);
    }

    return len;
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStringDuplicateFromDBString
 * --------------------------------------------------------------------------------------------- */

otext* OcilibStringDuplicateFromDBString
(
    OCI_Environment* env,
    const dbtext   * src,
    OCI_Int32        len
)
{
    otext* dst = (otext*)OcilibMemoryAlloc(env, OCI_IPC_STRING, sizeof(otext), (OCI_Size)(len + 1), FALSE);

    if (NULL != dst)
    {
        if (env->use_wide_char_conv)
        {
            OcilibStringUTF16ToUTF32((void*)src, (void*)dst, len);
        }
        else
        {
            OcilibStringRawCopy((void*)src, (void*)dst, len);
        }
    }

    dst[len] = 0;

    return dst;
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStringFromStringPtr
 * --------------------------------------------------------------------------------------------- */

otext* OcilibStringFromStringPtr
(
    OCI_Environment* libenv,
    OCIEnv         * envh,
    OCIString      * str,
    otext         ** buffer,
    OCI_UInt32     * buffer_size
)
{
    dbtext* tmp = NULL;

    if (NULL == buffer) return (NULL);

    tmp = (dbtext*)libenv->api->OCIStringPtr(envh, str);

    if (tmp)
    {
        const OCI_Size length = libenv->api->OCIStringSize(envh, str) / sizeof(dbtext);

        if (NULL == *buffer)
        {
            *buffer = OcilibMemoryAlloc(libenv, OCI_IPC_STRING, sizeof(otext),
                                        length + 1, FALSE);
        }
        else if ((*buffer_size) < ((length + 1) * sizeof(otext)))
        {
            *buffer = OcilibMemoryRealloc(libenv, (void*)*buffer, OCI_IPC_STRING, sizeof(otext),
                                          length + 1, FALSE);
        }

        if (NULL != *buffer)
        {
            *buffer_size = (OCI_UInt32)((length + 1) * sizeof(otext));

            if (libenv && libenv->use_wide_char_conv)
            {
                OcilibStringUTF16ToUTF32((void*)tmp, (void*)*buffer, (OCI_Int32)length);
            }
            else
            {
                OcilibStringRawCopy((void*)tmp, (void*)*buffer, (OCI_Int32)length);
            }
        }
    }

    return *buffer;
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStringToStringPtr
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibStringToStringPtr
(
    OCI_Environment* libenv,
    OCIEnv         * envh,
    OCIString     ** str,
    OCIError       * err,
    const otext    * value
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_ENVIRONMENT, libenv
    );

    dbtext  * dbstr  = NULL;
    OCI_Int32 dbsize = -1;

    CHECK_PTR(OCI_IPC_STRING, value);

    dbstr = OcilibStringGetDBString(libenv, value, &dbsize);

    CHECK_OCI
    (
        err,
        OCIStringAssignText,
        envh, err, (oratext*)dbstr, (ub4)dbsize, str
    );

    SET_SUCCESS();

    CLEANUP_AND_EXIT_FUNC
    (
        OcilibStringReleaseDBString(libenv, dbstr);
    );
}

/* --------------------------------------------------------------------------------------------- *
* OcilibStringFreeStringPtr
* --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibStringFreeStringPtr
(
    OCI_Environment* libenv,
    OCIEnv         * envh,
    OCIString     ** str,
    OCIError       * err
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_ENVIRONMENT, libenv
    );

    CHECK_PTR(OCI_IPC_STRING, str);

    CHECK_OCI
    (
        err,
        OCIStringResize,
        envh, err, (ub4)0, str
    );

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStringGetAttribute
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibStringGetAttribute
(
    OCI_Connection* con,
    void          * handle,
    OCI_UInt32      type,
    OCI_UInt32      attr,
    otext        ** str,
    OCI_UInt32    * size
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_CONNECTION, con
    );

    CHECK_PTR(OCI_IPC_CONNECTION, con);
    CHECK_PTR(OCI_IPC_STRING, str);
    CHECK_PTR(OCI_IPC_INT, size);

    dbtext  * dbstr  = NULL;
    OCI_Int32 dbsize = -1;
    OCI_Int32 len    = 0;

    CHECK_ATTRIB_GET
    (
        type, attr, handle, &dbstr, &dbsize, con->err
    );

    if (NULL != dbstr)
    {
        OCI_Boolean is_ansi = FALSE;

        /*  Oracle BUG using OCI in Unicode mode (once again...)
            Some attributes are returned as MBCS buffer even when OCI is initialized in UTF16 mode
            Some we check if the first character slot has any zero bytes set
            to detect this defect ! */

        if (IS_WIDE_ENV() && dbsize > 1)
        {
            char* ptr = (char*)dbstr;

            if (ptr[0] != 0 && ptr[1] != 0)
            {
                /* MBCS buffer returned instead of an UTF16 one ! */
                is_ansi = TRUE;
                len     = dbsize;
            }
        }

        /* if the input buffer was not MBCS while using an UTF16 environment, compute string length
         * from returned buffer size */
        if (len == 0)
        {
            len = (OCI_Int32) dbcharcount(dbsize);
        }

        if (OcilibStringRequestBuffer(con->env, str, size, (OCI_UInt32) len))
        {
            OcilibStringTranslate(dbstr, *str, (OCI_Int32) len,
                                  is_ansi ? sizeof(char) : sizeof(dbtext),
                                  sizeof(otext));
        }

        CHECK_NULL(*str);

        /* Ensure that the string value will be NULL terminated */

        (*str)[len] = 0;
    }

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStringSetAttribute
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibStringSetAttribute
(
    OCI_Connection* con,
    void          * handle,
    OCI_UInt32      type,
    OCI_UInt32      attr,
    otext        ** str,
    const otext   * value
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_CONNECTION, con
    );

    dbtext  * dbstr  = NULL;
    OCI_Int32 dbsize = -1;

    CHECK_PTR(OCI_IPC_CONNECTION, con);

    dbstr = OcilibStringGetDBString(con->env, value, &dbsize);

    if (dbsize == -1)
    {
        dbsize = 0;
    }

    CHECK_ATTRIB_SET
    (
        type, attr, handle, dbstr, dbsize, con->err
    );

    if (NULL != str)
    {
        FREE(*str);

        if (NULL != value)
        {
            *str = OcilibStringDuplicate(con->env, value);
        }
    }

    SET_SUCCESS();

    CLEANUP_AND_EXIT_FUNC
    (
        OcilibStringReleaseDBString(con->env, dbstr);
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStringGetFromType
 * --------------------------------------------------------------------------------------------- */

OCI_UInt32 OcilibStringGetFromType
(
    OCI_Connection* con,
    OCI_Column    * col,
    void          * data,
    OCI_UInt32      data_size,
    otext         * buffer,
    OCI_UInt32      buffer_size,
    OCI_Boolean     quote
)
{
    OCI_Boolean res   = TRUE;
    OCI_Boolean check = FALSE;
    OCI_UInt32  len   = 0;

    /* if buffer is not null, then buffer_size must the > 0 */

    otext* ptr = buffer;

    if (quote && NULL != data)
    {
        len = OcilibStringAddToBuffer(ptr, len, OTEXT("'"), 1, FALSE);
        if (ptr)
        {
            ptr++;
            buffer_size--;
        }
    }

    switch (col->datatype)
    {
        case OCI_CDT_TEXT:
        {
            len += OcilibStringAddToBuffer(buffer, len, (otext*)data, (OCI_UInt32) dbcharcount(data_size), quote);
            break;
        }
        case OCI_CDT_BOOLEAN:
        {
            if (NULL != ptr)
            {
                if (data)
                {
                    const otext* str_value = *(OCI_Boolean*)data ? OCI_STRING_TRUE : OCI_STRING_FALSE;

                    const OCI_UInt32 str_length = (OCI_UInt32)((*(OCI_Boolean*)data) ? OCI_STRING_TRUE_SIZE : OCI_STRING_FALSE_SIZE);

                    len += (OCI_UInt32) OcilibStringAddToBuffer(buffer, len, str_value, str_length, quote);
                }
                else
                {
                    res = FALSE;
                }
            }
            else
            {
                len = OCI_STRING_FALSE_SIZE;
            }

            break;
        }
        case OCI_CDT_NUMERIC:
        {
            check = TRUE;

            if (NULL != ptr)
            {
                res = OcilibNumberToStringInternal(con->env, data, col->subtype, ptr, (OCI_Int32)buffer_size, NULL);
            }
            else
            {
                len = OCI_SIZE_TMP_CVT;
            }
            break;
        }
        case OCI_CDT_DATETIME:
        {
            check = TRUE;

            if (ptr)
            {
                OCI_Date   * date = (OCI_Date*)data;
                const otext* fmt  = date ? OcilibEnvironmentGetFormat(date->env, OCI_FMT_DATE) : NULL;

                res = date ? OcilibDateToString(date, fmt, (OCI_Int32)buffer_size, ptr) : FALSE;
            }
            else
            {
                len = OCI_SIZE_TMP_CVT;
            }
            break;
        }
        case OCI_CDT_TIMESTAMP:
        {
            check = TRUE;

            if (ptr)
            {
                OCI_Timestamp * tmsp     = (OCI_Timestamp*)data;
                const OCI_Int32 fmt_type = tmsp && tmsp->type == OCI_TIMESTAMP_TZ ? OCI_FMT_TIMESTAMP_TZ : OCI_FMT_TIMESTAMP;

                const otext* fmt = tmsp ? OcilibEnvironmentGetFormat(tmsp->env, (OCI_FormatType) fmt_type) : NULL;

                res = tmsp ? OcilibTimestampToString(tmsp, fmt, (OCI_Int32)buffer_size, ptr, 0) : FALSE;
            }
            else
            {
                len = OCI_SIZE_TMP_CVT;
            }
            break;
        }
        case OCI_CDT_INTERVAL:
        {
            check = TRUE;

            if (NULL != ptr)
            {
                OCI_Interval* itv = (OCI_Interval*)data;

                res = itv ? OcilibIntervalToString(itv, OCI_STRING_DEFAULT_PREC,
                                                   OCI_STRING_DEFAULT_PREC,
                                                   (OCI_Int32)buffer_size, ptr) : FALSE;
            }
            else
            {
                len = OCI_SIZE_TMP_CVT;
            }
            break;
        }
        case OCI_CDT_LONG:
        {
            OCI_Long* lg = (OCI_Long*)data;

            if (NULL != lg)
            {
                if (OCI_CLONG == col->subtype)
                {
                    len = OcilibStringAddToBuffer(buffer, len,
                                                  (otext*)OcilibLongGetBuffer(lg),
                                                  OcilibLongGetSize(lg), quote);
                }
                else
                {
                    /* must hex-encode the LONG payload, not the OCI_Long handle
                       itself, and must be bounded by the payload size */

                    len = OcilibStringBinaryToString((unsigned char*)OcilibLongGetBuffer(lg),
                                                     OcilibLongGetSize(lg), ptr);
                }
            }
            else
            {
                res = FALSE;
            }

            break;
        }
        case OCI_CDT_RAW:
        {
            if (NULL != data)
            {
                len = OcilibStringBinaryToString((unsigned char*)data, data_size, ptr);
            }
            break;
        }
        case OCI_CDT_LOB:
        {
            OCI_Lob* lob = (OCI_Lob*)data;

            if (NULL != lob)
            {
                if (NULL != ptr)
                {
                    if (0 == OcilibLobGetLength(lob))
                    {
                        /* empty lobs (e.g. EMPTY_CLOB() collection elements) report
                           ORA-1403 in OcilibLobGetLength() - render them as empty */

                        OcilibErrorReset(OcilibErrorGet(con->env, FALSE, FALSE));
                    }
                    else
                    {
                        unsigned char lob_buf[OCI_SIZE_LARGE_BUFFER];

                        while (res)
                        {
                            const OCI_UInt32 bytes_requested = sizeof(lob_buf) - sizeof(otext);

                            OCI_UInt32 bytes_count = bytes_requested;
                            OCI_UInt32 char_count  = (OCI_UInt32) (bytes_requested / sizeof(dbtext));

                            res = OcilibLobRead(lob, lob_buf, &char_count, &bytes_count);

                            if (!res || bytes_count == 0)
                            {
                                break;
                            }

                            if (OCI_BLOB == lob->type)
                            {
                                len += OcilibStringBinaryToString(lob_buf, bytes_count, ptr + len);
                            }
                            else
                            {
                                len += OcilibStringAddToBuffer(buffer, len, (otext*)lob_buf,
                                                               (OCI_UInt32) ocharcount(bytes_count), quote);
                            }
                        }

                        OcilibLobSeek(lob, 0, OCI_SEEK_SET);
                    }
                }
                else
                {
                    len = (OCI_UInt32)OcilibLobGetLength(lob);

                    if (0 == len)
                    {
                        OcilibErrorReset(OcilibErrorGet(con->env, FALSE, FALSE));
                    }

                    if (OCI_BLOB == lob->type)
                    {
                        /* here we have binary blob, it will be output in hexadecimal */
                        len *= 2;
                    }
                }
            }
            else
            {
                res = FALSE;
            }

            break;

        }
        case OCI_CDT_FILE:
        {
            OCI_File* file = (OCI_File*)data;

            if (NULL != file)
            {
                const otext* dir  = OcilibFileGetDirectory(file);
                const otext* name = OcilibFileGetName(file);

                len += OcilibStringAddToBuffer(buffer, len, dir, (OCI_UInt32)ostrlen(dir), TRUE);
                len += OcilibStringAddToBuffer(buffer, len, OTEXT("/"), 1, TRUE);
                len += OcilibStringAddToBuffer(buffer, len, name, (OCI_UInt32)ostrlen(name), TRUE);
            }
            else
            {
                res = FALSE;
            }

            break;

        }
        case OCI_CDT_REF:
        {
            check = TRUE;

            if (NULL != ptr)
            {
                OCI_Reference* ref = (OCI_Reference*)data;
                res = ref ? OcilibReferenceToText(ref, buffer_size, ptr) : FALSE;
            }
            else
            {
                len = OCI_SIZE_TMP_CVT;
            }
            break;
        }
        case OCI_CDT_OBJECT:
        {
            OCI_Object* obj = (OCI_Object*)data;

            OCI_UInt32 real_size = buffer_size;

            quote = FALSE;
            res   = obj ? OcilibObjectToString(obj, &real_size, ptr) : FALSE;
            len   = real_size;
            break;
        }
        case OCI_CDT_VECTOR:
        {
            OCI_Vector* vect = (OCI_Vector*)data;

            OCI_UInt32 real_size = buffer_size;

            quote = FALSE;
            res   = vect ? OcilibVectorToString(vect, &real_size, ptr) : FALSE;
            len   = real_size;
            break;
        }
        case OCI_CDT_COLLECTION:
        {
            OCI_Collection* coll = (OCI_Collection*)data;

            OCI_UInt32 real_size = buffer_size;

            quote = FALSE;
            res   = coll ? OcilibCollectionToText(coll, &real_size, ptr) : FALSE;
            len   = real_size;
            break;
        }
        case OCI_CDT_CURSOR:
        {
            OCI_Statement* stmt = (OCI_Statement*)data;

            quote = TRUE;

            if (NULL != stmt)
            {
                len = OcilibStringAddToBuffer(buffer, len, stmt->sql,
                                              (OCI_UInt32)ostrlen(stmt->sql), quote);
            }
            else
            {
                res = FALSE;
            }
            break;
        }
        case OCI_CDT_JSON:
        {
            OCI_Json* json = (OCI_Json*)data;

            OCI_UInt32 real_size = (OCI_UInt32)(buffer_size / sizeof(otext));

            quote = FALSE;
            res   = json ? OcilibJsonToText(json, ptr, &real_size, 0) : FALSE;
            len   = real_size;
            break;
        }
        default:
        {
            res   = FALSE;
            quote = FALSE;
        }
    }

    if (res)
    {
        if (NULL != buffer && check && buffer_size > len)
        {
            /* the resulting string shall be computed as it was not known in advance */
            len = (OCI_UInt32)ostrlen(buffer);
        }

        if (quote && NULL != data)
        {
            len += OcilibStringAddToBuffer(buffer, len, OTEXT("'"), 1, FALSE);
        }
    }
    else
    {
        len = 0;
    }

    /* make sure it is null terminated */
    if (NULL != buffer)
    {
        /* if computed length > buffer_size, something went wrong, consider this as failure */
        if (len > buffer_size)
        {
            len = 0;
        }

        buffer[len] = 0;
    }

    return len;
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStringAddToBuffer
 * --------------------------------------------------------------------------------------------- */

OCI_UInt32 OcilibStringAddToBuffer
(
    otext      * buffer,
    OCI_UInt32   offset,
    const otext* str,
    OCI_UInt32   length,
    OCI_Boolean  check_quote
)
{
    const OCI_UInt32 len_in = length;

    OCI_UInt32 len_out = 0;

    if (NULL == str)
    {
        return 0;
    }

    if (check_quote)
    {
        if (NULL != buffer)
        {
            const otext* src = str;

            otext* dst = buffer + offset;

            OCI_UInt32 src_len = len_in;

            while (NULL != src && *src && src_len)
            {
                *dst = *src;

                if (*src == OTEXT('\''))
                {
                    *(++dst) = OTEXT('\'');
                    len_out++;
                }

                dst++;
                src++;
                src_len--;
            }

            len_out += len_in;

            *dst = 0;
        }
        else
        {
            const otext* s = str;
            len_out = len_in;

            while (NULL != s && *s)
            {
                if (*s++ == OTEXT('\''))
                {
                    len_out++;
                }
            }
        }
    }
    else
    {
        len_out = len_in;

        if (NULL != buffer)
        {
            ostrcpy(buffer + offset, str);
        }
    }

    return len_out;
}

/* --------------------------------------------------------------------------------------------- *
* OcilibStringAppendChecked
* --------------------------------------------------------------------------------------------- */

/* Appends at most 'count' characters of 'src' to 'dest' at 'offset'.
 *
 * 'limit' is the maximum number of characters that may be written to 'dest',
 * excluding the terminating NUL: 'dest' must therefore have room for
 * 'limit' + 1 elements, which is the convention used by every caller of
 * OcilibStringGetTypeName() / OcilibStringGetFullTypeName().
 *
 * The result is always NUL terminated, the write is always clamped to 'limit',
 * and the returned offset is always <= 'limit'. Used instead of ostrncpy()
 * because ostrncpy() is strncpy()/wcsncpy(), which does not terminate when the
 * source is longer than the bound.
 */

static OCI_UInt32 OcilibStringAppendChecked
(
    otext      * dest,
    OCI_UInt32   limit,
    OCI_UInt32   offset,
    const otext* src,
    OCI_Size     count
)
{
    if (NULL == dest)
    {
        return offset;
    }

    if (offset > limit)
    {
        offset = limit;
    }

    {
        const OCI_Size avail = (OCI_Size)(limit - offset);

        if (count > avail)
        {
            count = avail;
        }

        if (NULL != src && count > 0)
        {
            memcpy(dest + offset, src, count * sizeof(otext));
            offset += (OCI_UInt32)count;
        }
    }

    dest[offset] = 0;

    return offset;
}

/* --------------------------------------------------------------------------------------------- *
* OcilibStringGetTypeName
* --------------------------------------------------------------------------------------------- */

OCI_UInt32 OcilibStringGetTypeName
(
    const otext* source,
    otext      * dest,
    OCI_UInt32   length
)
{
    OCI_Boolean quote  = FALSE;
    OCI_UInt32  offset = 0;

    if (!IS_STRING_VALID(source) || NULL == dest)
    {
        return 0;
    }

    /* For types created WITH case sensitivity, OCI may not return quoted names... */

    if (source[0] != OTEXT('"'))
    {
        const otext* str = NULL;

        for (str = source; *str; str++)
        {
            if ((*str) != (otext)otoupper(*str))
            {
                quote = TRUE;
                break;
            }
        }
    }

    /* Fill destination string, clamped to 'length' characters */

    if (quote)
    {
        offset = OcilibStringAppendChecked(dest, length, offset, OTEXT("\""), 1);
    }

    offset = OcilibStringAppendChecked(dest, length, offset, source, ostrlen(source));

    if (quote)
    {
        offset = OcilibStringAppendChecked(dest, length, offset, OTEXT("\""), 1);
    }

    return offset;
}

/* --------------------------------------------------------------------------------------------- *
* OcilibStringGetFullTypeName
* --------------------------------------------------------------------------------------------- */

OCI_UInt32 OcilibStringGetFullTypeName
(
    const otext* schema,
    const otext* package,
    const otext* type,
    const otext* link,
    otext      * name,
    OCI_UInt32   length
)
{
    OCI_UInt32 offset = 0;

    if (NULL == name)
    {
        return 0;
    }

    name[0] = 0;

    /* Every step is guarded on 'offset < length' so that the sub-calls never
       receive an underflowed (length - offset) as their own capacity. */

    if (IS_STRING_VALID(schema) && offset < length)
    {
        offset += OcilibStringGetTypeName(schema, name + offset, length - offset);

        if (offset)
        {
            offset = OcilibStringAppendChecked(name, length, offset, OTEXT("."), 1);
        }
    }

    if (IS_STRING_VALID(package) && offset < length)
    {
        offset += OcilibStringGetTypeName(package, name + offset, length - offset);

        if (offset)
        {
            offset = OcilibStringAppendChecked(name, length, offset, OTEXT("."), 1);
        }
    }

    if (IS_STRING_VALID(type) && offset < length)
    {
        offset += OcilibStringGetTypeName(type, name + offset, length - offset);
    }

    if (IS_STRING_VALID(link) && offset < length)
    {
        offset = OcilibStringAppendChecked(name, length, offset, OTEXT("@"), 1);

        if (offset < length)
        {
            offset += OcilibStringGetTypeName(link, name + offset, length - offset);
        }
    }

    return offset;
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStringDuplicate
 * --------------------------------------------------------------------------------------------- */

otext* OcilibStringDuplicate
(
    OCI_Environment* env,
    const otext    * src
)
{
    if (NULL == src) return (NULL);

    otext* dst = (otext*)OcilibMemoryAlloc
                 (
                     env,
                     OCI_IPC_STRING,
                     sizeof(otext),
                     ostrlen(src) + 1,
                     0);

    if (NULL != dst)
    {
        ostrcpy(dst, src);
    }

    return dst;
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStringMbcsDuplicate
 * --------------------------------------------------------------------------------------------- */

char* OcilibStringMbcsDuplicate
(
    OCI_Environment* env,
    const char     * src
)
{
    if (NULL == src) return (NULL);

    char* dst = (char*)OcilibMemoryAlloc
                (
                    env,
                    OCI_IPC_STRING,
                    sizeof(char),
                    strlen(src) + 1,
                    0);

    if (NULL != dst)
    {
        strcpy(dst, src);
    }

    return dst;
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStringCaseCompare
 * --------------------------------------------------------------------------------------------- */

OCI_Int32 OcilibStringCaseCompare
(
    const otext* str1,
    const otext* str2
)
{
    if (NULL == str1 && NULL == str2)
    {
        return 0;
    }

    if (NULL == str1)
    {
        return 1;
    }

    if (NULL == str2)
    {
        return -1;
    }

    while (((*str1) != 0) && ((*str2) != 0) && (otoupper(*str1) == otoupper(*str2)))
    {
        str1++;
        str2++;
    }

    return (otoupper(*str1) - otoupper(*str2));
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStringMbcsCaseCompare
 * --------------------------------------------------------------------------------------------- */

OCI_Int32 OcilibStringMbcsCaseCompare
(
    const char* str1,
    const char* str2
)
{
    if (NULL == str1 && NULL == str2)
    {
        return 0;
    }

    if (NULL == str1)
    {
        return 1;
    }

    if (NULL == str2)
    {
        return -1;
    }

    while (((*str1) != 0) && ((*str2) != 0) && (toupper(*str1) == toupper(*str2)))
    {
        str1++;
        str2++;
    }

    return (toupper(*str1) - toupper(*str2));
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibStringFormat
 * --------------------------------------------------------------------------------------------- */

OCI_Int32 OcilibStringFormat
(
    otext      * str,
    OCI_Int32    size,
    const otext* format,
    ...
)
{
    /* size is the number of character that can fit in str variable without including last null char
    */

    va_list args;

    va_start(args, format);

    const OCI_Int32 n = (OCI_Int32)ovsprintf(str, (OCI_Size)(size + 1), format, args);

    va_end(args);

    return n;
}