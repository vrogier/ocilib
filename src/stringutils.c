/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2020 Vincent ROGIER <vince.rogier@ocilib.net>
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

#define COMPUTE_LENTGH(type, ptr, size)   \
    const type *s = (const type *) (ptr); \
    const type *e = (const type *) (ptr); \
    while (*e++);                         \
    (size) = (int) (e - s - (size_t) 1);  \


/* --------------------------------------------------------------------------------------------- *
 * StringLength
 * --------------------------------------------------------------------------------------------- */

size_t StringLength
(
    const void *ptr,
    size_t      size_elem
)
{
    int size = 0;

    CHECK_FALSE(NULL == ptr, 0);

    if (Env.nls_utf8)
    {
        const char *s = (char *) ptr;
        while (*s)
        {
            if ((*s & 0xc0) != 0x80) size++;
            s++;
        }
    }
    else if (sizeof(char) == size_elem)
    {
        COMPUTE_LENTGH(char, ptr, size)
    }
    else if (sizeof(short) == size_elem)
    {
        COMPUTE_LENTGH(short, ptr, size)
    }
    else if (sizeof(int) == size_elem)
    {
        COMPUTE_LENTGH(int, ptr, size)
    }

    return size;
}

/* --------------------------------------------------------------------------------------------- *
 * StringBinaryToString
 * --------------------------------------------------------------------------------------------- */

unsigned int StringBinaryToString
(
    const unsigned char *binary,
    unsigned int         binary_size,
    otext               *buffer
)
{
    char hex_str[] = "0123456789ABCDEF";

    const unsigned int len = binary_size * 2;

    if (buffer)
    {
        for (unsigned int i = 0; i < binary_size; i++)
        {
            buffer[i * 2 + 0] = hex_str[binary[i] >> 4  ];
            buffer[i * 2 + 1] = hex_str[binary[i] & 0x0F];
        }

        buffer[len] = 0;
    }

    return len;
}

/* --------------------------------------------------------------------------------------------- *
 * StringRequestBuffer
 * --------------------------------------------------------------------------------------------- */

boolean StringRequestBuffer
(
    otext       **buffer,
    unsigned int *buffer_size,
    unsigned int  request_size
)
{
    boolean res = FALSE;

    if (!buffer || !buffer_size)
    {
        return FALSE;
    }

    request_size++;

    request_size *= Env.nls_utf8 ? OCI_UTF8_BYTES_PER_CHAR :  sizeof(otext);

    if (!*buffer)
    {
        *buffer = (otext *) MemoryAlloc(OCI_IPC_STRING, (size_t) request_size, 
                                        (size_t) 1, TRUE);
    }
    else if (*buffer_size < request_size)
    {
        *buffer = (otext *) MemoryRealloc(*buffer, OCI_IPC_STRING, (size_t) request_size,
                                          (size_t) 1, TRUE);
    }

    if (*buffer)
    {
        *buffer_size = request_size;

        res = TRUE;
    }

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * StringTranslate
 * --------------------------------------------------------------------------------------------- */

void StringTranslate
(
    void  *src,
    void  *dst,
    int    char_count,
    size_t size_char_in,
    size_t size_char_out
)
{
    const int len = char_count;

    if (!src || !dst)
    {
        return;
    }

    /* raw string packing/expansion without charset conversion */

    if (size_char_out > size_char_in)
    {
        /* expand string */

        if ((size_char_in == sizeof(short)) && (size_char_out == sizeof(int)))
        {
            /* 2 => 4 bytes */

            unsigned short *str1 = (unsigned short *) src;
            unsigned int   *str2 = (unsigned int   *) dst;

            if (*str1 == 0)
            {
                return;
            }

            while (char_count--)
            {
                str2[char_count] = (unsigned int) str1[char_count];
            }
        }

        else if ((size_char_in == sizeof(char)) && (size_char_out == sizeof(short)))
        {
            /* 1 => 2 bytes */

            unsigned char  *str1 = (unsigned char  *) src;
            unsigned short *str2 = (unsigned short *) dst;

            if (*str1 == 0)
            {
                return;
            }

            while (char_count--)
            {
                str2[char_count] = (unsigned short) str1[char_count];
            }
        }
        else if ((size_char_in == sizeof(char)) && (size_char_out == sizeof(int)))
        {
            /* 1 => 4 bytes */

            unsigned char *str1 = (unsigned char *) src;
            unsigned int  *str2 = (unsigned int  *) dst;

            if (*str1 == 0)
            {
                return;
            }

            while (char_count--)
            {
                str2[char_count] = (unsigned int) str1[char_count];
            }
        }
    }
    else if (size_char_out < size_char_in)
    {
        /* pack string */

        if ((size_char_in == sizeof(int)) && (size_char_out == sizeof(short)))
        {
            /* 4 => 2 bytes */

            unsigned int   *str1 = (unsigned int   *) src;
            unsigned short *str2 = (unsigned short *) dst;

            int i = 0;

            if (*str1 == 0)
            {
                return;
            }

            while (i < char_count)
            {
                str2[i] = (unsigned short) str1[i];
                i++;
            }
        }
        else if ((size_char_in == sizeof(short)) && (size_char_out == sizeof(char)))
        {
            /* 2 => 1 bytes */

            unsigned short *str1 = (unsigned short *) src;
            unsigned char  *str2 = (unsigned char  *) dst;

            int i= 0;

            if (*str1 == 0)
            {
                return;
            }

            while (i < char_count)
            {
                str2[i] = (unsigned char) str1[i];
                i++;
            }
        }
        else if ((size_char_in == sizeof(int)) &&  (size_char_out == sizeof(char)))
        {
            /* 4 => 1 bytes */

            unsigned int  *str1 = (unsigned int  *) src;
            unsigned char *str2 = (unsigned char *) dst;

            int i = 0;

            if (*str1 == 0)
            {
                return;
            }

            while (i < char_count)
            {
                str2[i] = (unsigned char) str1[i];
                i++;
            }
        }
    }
    else
    {
        memcpy(dst, src, len * size_char_out);
    }

    memset(((char*) dst) + len * size_char_out, 0, size_char_out);
}

/* --------------------------------------------------------------------------------------------- *
 * StringGetDBString
 * --------------------------------------------------------------------------------------------- */

dbtext * StringGetDBString
(
    const otext *src,
    int         *size
)
{
    dbtext *dst = NULL;
    int     len = 0;

    if (!src)
    {
        *size = 0;
        return dst;
    }

    if (*size == -1)
    {
        COMPUTE_LENTGH(otext, src, len)
    }
    else
    {
        len = (int) ((*size) / sizeof(otext));
    }

    if (Env.use_wide_char_conv)
    {
        dst = (dbtext *) MemoryAlloc(OCI_IPC_STRING, sizeof(dbtext), len + 1, FALSE);

        if (NULL != dst)
        {
            StringUTF32ToUTF16( src, dst, len );
        }
    }
    else
    {
        dst = (dbtext *) src;
    }

    *size = (int) (len * sizeof(dbtext));

    return dst;
}

/* --------------------------------------------------------------------------------------------- *
 * StringReleaseDBString
 * --------------------------------------------------------------------------------------------- */

void StringReleaseDBString
(
    dbtext *str
)
{
    if (Env.use_wide_char_conv && NULL != str)
    {
        MemoryFree(str);
    }
}

/* --------------------------------------------------------------------------------------------- *
 * StringCopyDBStringToNativeString
 * --------------------------------------------------------------------------------------------- */

int StringCopyDBStringToNativeString
(
    const dbtext *src,
    otext        *dst,
    int           len
)
{
    if (Env.use_wide_char_conv)
    {
        StringUTF16ToUTF32((void *) src, (void *) dst, len);
    }

    return len;
}

/* --------------------------------------------------------------------------------------------- *
 * StringDuplicateFromDBString
 * --------------------------------------------------------------------------------------------- */

otext* StringDuplicateFromDBString
(
    const dbtext *src,
    int           len
)
{
    otext *dst = (otext *) MemoryAlloc(OCI_IPC_STRING, sizeof(otext), len + 1, FALSE);

    if (NULL != dst)
    {
        if (Env.use_wide_char_conv)
        {
            StringUTF16ToUTF32((void *) src, (void *) dst, len);
        }
        else
        {
            StringRawCopy((void *) src, (void *) dst, len);
        }
    }

    return dst;
}

/* --------------------------------------------------------------------------------------------- *
 * StringFromStringPtr
 * --------------------------------------------------------------------------------------------- */

otext * StringFromStringPtr
(
    OCIEnv       *env,
    OCIString    *str,
    otext       **buffer,
    unsigned int *buffer_size
)
{
    dbtext *tmp = NULL;

    CHECK_FALSE(NULL == buffer, NULL);

    tmp = (dbtext *) OCIStringPtr(env, str);

    if (tmp)
    {
        const size_t length = OCIStringSize(Env.env, str) / sizeof(dbtext);

        if (NULL == *buffer)
        {
            *buffer = MemoryAlloc(OCI_IPC_STRING, sizeof(otext), 
                                  length + 1, FALSE);
        }
        else if ((*buffer_size) < ((length + 1) * sizeof(otext)))
        {
            *buffer = MemoryRealloc((void*)  *buffer, OCI_IPC_STRING, sizeof(otext),
                                    length + 1, FALSE);
        }

        if (NULL != *buffer)
        {
            *buffer_size = (unsigned int) ( (length + 1) * sizeof(otext));

            if (Env.use_wide_char_conv)
            {
                StringUTF16ToUTF32((void *)tmp, (void *)*buffer, (int) length);
            }
            else
            {
                StringRawCopy((void *)tmp, (void *)*buffer, (int) length);
            }
        }
    }

    return *buffer;
}

/* --------------------------------------------------------------------------------------------- *
 * StringToStringPtr
 * --------------------------------------------------------------------------------------------- */

boolean StringToStringPtr
(
    OCIEnv      *env,
    OCIString  **str,
    OCIError    *err,
    const otext *value
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_VOID, &Env
    )

    dbtext *dbstr = NULL;
    int dbsize = -1;

    CHECK_PTR(OCI_IPC_STRING, value)

    dbstr = StringGetDBString(value, &dbsize);

    CHECK_OCI
    (
        err,
        OCIStringAssignText,
        env, err, (oratext*)dbstr, (ub4)dbsize, str
    )

    SET_SUCCESS()

    CLEANUP_AND_EXIT_FUNC
    (
        StringReleaseDBString(dbstr);
    )
}

/* --------------------------------------------------------------------------------------------- *
* StringFreeStringPtr
* --------------------------------------------------------------------------------------------- */

boolean StringFreeStringPtr
(
    OCIEnv     *env,
    OCIString **str,
    OCIError   *err
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_VOID, &Env
    )

    CHECK_PTR(OCI_IPC_STRING, str)

    CHECK_OCI
    (
        err,
        OCIStringResize,
        env, err, (ub4)0, str
    )

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * StringGetAttribute
 * --------------------------------------------------------------------------------------------- */

boolean StringGetAttribute
(
    OCI_Connection *con,
    void           *handle,
    unsigned int    type,
    unsigned int    attr,
    otext         **str,
    unsigned int   *size
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_CONNECTION, con
    )

    CHECK_PTR(OCI_IPC_CONNECTION, con)
    CHECK_PTR(OCI_IPC_STRING,     str)
    CHECK_PTR(OCI_IPC_INT,       size)

    dbtext *dbstr = NULL;
    int dbsize = -1;
    int len    = 0;

    CHECK_ATTRIB_GET
    (
        type, attr, handle, &dbstr, &dbsize, con->err
    )

    if (NULL !=  dbstr)
    {
        boolean is_ansi = FALSE;

        /*  Oracle BUG using OCI in Unicode mode (once again...)
            Some attributes are returned as ANSI buffer even when OCI is initialized in UTF16 mode
            Some we check if the first character slot has any zero bytes set
            to detect this defect ! */

        if ((OCI_CHAR_WIDE == Env.charset) && dbsize > 1)
        {
            char *ptr = (char*) dbstr;

            if (ptr[0] != 0 && ptr[1] != 0)
            {
                /* ANSI buffer returned instead of an UTF16 one ! */
                is_ansi = TRUE;
                len     = dbsize;
            }
        }

        /* if the input buffer was not ANSI while using an UTF16 environment, compute string length
         * from returned buffer size */
        if (len == 0)
        {
            len = dbcharcount(dbsize);
        }

        if (StringRequestBuffer(str, size, len))
        {
            StringTranslate(dbstr, *str, len, is_ansi ? sizeof(char) : sizeof(dbtext),
                            sizeof(otext));
        }

        CHECK_NULL(*str)
    }

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * StringSetAttribute
 * --------------------------------------------------------------------------------------------- */

boolean StringSetAttribute
(
    OCI_Connection *con,
    void           *handle,
    unsigned int    type,
    unsigned int    attr,
    otext         **str,
    const otext    *value
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_CONNECTION, con
    )

    dbtext *dbstr = NULL;
    int dbsize = -1;

    CHECK_PTR(OCI_IPC_CONNECTION, con)

    dbstr = StringGetDBString(value, &dbsize);

    if (dbsize == -1)
    {
        dbsize = 0;
    }

    CHECK_ATTRIB_SET
    (
        type, attr, handle, dbstr, dbsize, con->err
    )

    if (NULL != str)
    {
        FREE(*str)

        if (NULL != value)
        {
            *str = ostrdup(value);
        }
    }

    SET_SUCCESS()

    CLEANUP_AND_EXIT_FUNC
    (
        StringReleaseDBString(dbstr);
    )
}

/* --------------------------------------------------------------------------------------------- *
 * StringGetFromType
 * --------------------------------------------------------------------------------------------- */

unsigned int StringGetFromType
(
    OCI_Connection *con,
    OCI_Column     *col,
    void           *data,
    unsigned int    data_size,
    otext          *buffer,
    unsigned int    buffer_size,
    boolean         quote
)
{
    boolean      res   = TRUE;
    boolean      check = FALSE;
    unsigned int len   = 0;

    /* if buffer is not null, then buffer_size must the > 0 */

    otext *ptr = buffer;

    if (quote && NULL != data)
    {
        len = StringAddToBuffer(ptr, len, OTEXT("'"), 1, FALSE);
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
            len += StringAddToBuffer(buffer, len, (otext *) data, dbcharcount(data_size), quote);
            break;
        }
        case OCI_CDT_BOOLEAN:
        {
            if (NULL != ptr)
            {
                if (data)
                {
                    otext * str_value = *(boolean*)data ? OCI_STRING_TRUE : OCI_STRING_FALSE;

                    const unsigned int str_length = (*(boolean*)data) ? OCI_STRING_TRUE_SIZE : OCI_STRING_FALSE_SIZE;

                    len += StringAddToBuffer(buffer, len, str_value, str_length, quote);
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
                res = NumberToStringInternal(con, data, col->subtype, ptr, (int) buffer_size, NULL);
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
                OCI_Date    *date = (OCI_Date*) data;
                const otext *fmt  = EnvironmentGetFormat(con, OCI_FMT_DATE);

                res = date ? DateToString(date, fmt, (int) buffer_size, ptr) : FALSE;
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
                OCI_Timestamp *tmsp = (OCI_Timestamp *) data;
                const int fmt_type = tmsp && tmsp->type == OCI_TIMESTAMP_TZ ? OCI_FMT_TIMESTAMP_TZ : OCI_FMT_TIMESTAMP;

                const otext *fmt = EnvironmentGetFormat(con, fmt_type);

                res = tmsp ? TimestampToString(tmsp, fmt, (int) buffer_size, ptr, 0) : FALSE;
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
                OCI_Interval *itv = (OCI_Interval * ) data;

                res = itv ? IntervalToString(itv, OCI_STRING_DEFAULT_PREC,
                                 OCI_STRING_DEFAULT_PREC, (int) buffer_size, ptr) : FALSE;
            }
            else
            {
                len = OCI_SIZE_TMP_CVT;
            }
            break;
        }
        case OCI_CDT_LONG:
        {
            OCI_Long *lg = (OCI_Long *) data;

            if (NULL != lg)
            {
                if (OCI_CLONG == col->subtype)
                {
                    len = StringAddToBuffer(buffer, len, (otext*) LongGetBuffer(lg),
                                      LongGetSize(lg), quote);
                }
                else
                {
                    len = StringBinaryToString((unsigned char *) data, buffer_size, ptr);
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
                len = StringBinaryToString((unsigned char *) data, data_size, ptr);
            }
            break;
        }
        case OCI_CDT_LOB:
        {
            OCI_Lob *lob = (OCI_Lob *) data;

            if (NULL != lob)
            {
                if (NULL != ptr)
                {
                    unsigned char lob_buf[OCI_SIZE_LARGE_BUFFER];

                    while (res)
                    {
                        const unsigned int bytes_requested = sizeof(lob_buf) - sizeof(otext);

                        unsigned int bytes_count = bytes_requested;
                        unsigned int char_count  = 0;

                        res = LobRead2(lob, lob_buf, &char_count, &bytes_count);

                        if (bytes_count == 0)
                        {
                            break;
                        }

                        if (OCI_BLOB == lob->type)
                        {
                            len += StringBinaryToString(lob_buf, bytes_count, ptr + len);
                        }
                        else
                        {
                            len += StringAddToBuffer(buffer, len, (otext*)lob_buf,
                                                     ocharcount(bytes_count), quote);
                        }
                    }

                    LobSeek(lob, 0, OCI_SEEK_SET);
                }
                else
                {
                    len = (unsigned int)LobGetLength(lob);

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
            OCI_File *file = (OCI_File *) data;

            if (NULL != file)
            {
                const otext * dir  = FileGetDirectory(file);
                const otext * name =  FileGetName(file);

                len += StringAddToBuffer(buffer, len, dir, (unsigned int) ostrlen(dir), TRUE);
                len += StringAddToBuffer(buffer, len, OTEXT("/"), 1, TRUE);
                len += StringAddToBuffer(buffer, len, name, (unsigned int) ostrlen(name), TRUE);
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
                OCI_Ref *ref = (OCI_Ref *) data;
                res = ref ? ReferenceToString(ref, buffer_size, ptr) : FALSE;
            }
            else
            {
                len = OCI_SIZE_TMP_CVT;
            }
            break;
        }
        case OCI_CDT_OBJECT:
        {
            OCI_Object *obj = (OCI_Object *) data;

            unsigned int real_size = buffer_size;

            quote = FALSE;
            res   = obj ? ObjectToString(obj, &real_size, ptr) : FALSE;
            len   = real_size;
            break;
        }
        case OCI_CDT_COLLECTION:
        {
            OCI_Coll *coll = (OCI_Coll *) data;

            unsigned int real_size = buffer_size;

            quote = FALSE;
            res   = coll ? CollectionToString(coll, &real_size, ptr) : FALSE;
            len   = real_size;
            break;
        }
        case OCI_CDT_CURSOR:
        {
            OCI_Statement *stmt = (OCI_Statement *)data;

            quote = TRUE;

            if (NULL != stmt)
            {
                len = StringAddToBuffer(buffer, len, stmt->sql,
                                        (unsigned int) ostrlen(stmt->sql), quote);
            }
            else
            {
                res = FALSE;
            }
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
            len = (unsigned int) ostrlen(buffer);
        }

        if (quote && NULL != data)
        {
            len += StringAddToBuffer(buffer, len, OTEXT("'"), 1, FALSE);
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
 * StringAddToBuffer
 * --------------------------------------------------------------------------------------------- */

unsigned int StringAddToBuffer
(
    otext       *buffer,
    unsigned int offset,
    const otext *str,
    unsigned int length,
    boolean      check_quote
)
{
    const unsigned int len_in = length;

    unsigned int len_out = 0;

    if (NULL == str)
    {
        return 0;
    }

    if (check_quote)
    {
        if (NULL != buffer)
        {
            const otext *src = str;

            otext *dst = buffer + offset;

            unsigned int src_len = len_in;

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
            const otext *s = str;
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
* StringGetTypeName
* --------------------------------------------------------------------------------------------- */

unsigned int StringGetTypeName
(
    const otext *source,
    otext       *dest,
    unsigned int length
)
{
    boolean      quote  = FALSE;
    unsigned int offset = 0;

    if (!IS_STRING_VALID(source) || NULL == dest)
    {
        return 0;
    }

    /* For types created WITH case sensitivity, OCI may not return quoted names... */

    if (source[0] != OTEXT('"'))
    {
        const otext *str = NULL;

        for (str = source; *str; str++)
        {
            if ((*str) != otoupper(*str))
            {
                quote = TRUE;
                break;
            }
        }
    }

    /* Fill destination string */

    if (quote)
    {
        ostrncpy(dest + offset, OTEXT("\""), length - offset);
        offset++;
    }

    ostrncpy(dest + offset, source, length - offset);
    offset += (unsigned int)ostrlen(source);

    if (quote)
    {
        ostrncpy(dest + offset, OTEXT("\""), length - offset);
        offset++;
    }

    return offset;
}

/* --------------------------------------------------------------------------------------------- *
* StringGetFullTypeName
* --------------------------------------------------------------------------------------------- */

unsigned int StringGetFullTypeName
(
    const otext *schema,
    const otext *package,
    const otext *type,
    const otext *link,
    otext       *name,
    unsigned int length
)
{
    unsigned int offset = 0;

    if (IS_STRING_VALID(schema))
    {
        offset += StringGetTypeName(schema, name + offset, length - offset);

        if (offset)
        {
            ostrncpy(name + offset, OTEXT("."), length - offset);
            offset++;
        }
    }

    if (IS_STRING_VALID(package))
    {
        offset += StringGetTypeName(package, name + offset, length - offset);

        if (offset)
        {
            ostrncpy(name + offset, OTEXT("."), length - offset);
            offset++;
        }
    }

    if (IS_STRING_VALID(type))
    {
        offset += StringGetTypeName(type, name + offset, length - offset);
    }

    if (IS_STRING_VALID(link))
    {
        ostrncpy(name + offset, OTEXT("@"), length - offset);
        offset++;
        offset += StringGetTypeName(link, name + offset, length - offset);
    }

    return offset;
}

/* --------------------------------------------------------------------------------------------- *
 * ocistrdup
 * --------------------------------------------------------------------------------------------- */

char * ocistrdup
(
    const char * src
)
{
    CHECK_FALSE(NULL == src, NULL)

    char *dst = (char *) MemoryAlloc(OCI_IPC_STRING, 1, strlen(src) + 1, 0);

    if (NULL != dst)
    {
        strcpy(dst, src);
    }

    return dst;
}

/* --------------------------------------------------------------------------------------------- *
 * ocistrcasecmp
 * --------------------------------------------------------------------------------------------- */

int ocistrcasecmp
(
    const char *str1,
    const char *str2
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

    while (((*str1) != 0) && ((*str2) != 0) && (tolower((int)(*str1)) == tolower((int)(*str2))))
    {
        str1++;
        str2++;
    }

    return (tolower((int) (*str1)) - tolower((int) (*str2)));
}

/* --------------------------------------------------------------------------------------------- *
 * ocisprintf
 * --------------------------------------------------------------------------------------------- */

int ocisprintf
(
    char       *str,
    int         size,
    const char *format,
    ...
)
{
    va_list args;

    va_start(args, format);

    const int n = (int) vsnprintf(str, (size_t) size, format, args);

    va_end(args);

    return n;
}

/* --------------------------------------------------------------------------------------------- *
 * ociwcsdup
 * --------------------------------------------------------------------------------------------- */

wchar_t * ociwcsdup
(
    const wchar_t * src
)
{
    CHECK_FALSE(NULL == src, NULL)

    wchar_t *dst = (wchar_t *) MemoryAlloc(OCI_IPC_STRING, sizeof(wchar_t),
                                           wcslen(src) + 1, 0);

    if (NULL != dst)
    {
        wcscpy(dst, src);
    }

    return dst;
}

/* --------------------------------------------------------------------------------------------- *
 * ociwcscasecmp
 * --------------------------------------------------------------------------------------------- */

int ociwcscasecmp
(
    const wchar_t *str1,
    const wchar_t *str2
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

    while (((*str1) != 0) && ((*str2) != 0) && (towlower((wint_t)*str1) == towlower((wint_t)*str2)))
    {
        str1++;
        str2++;
    }

    return (towlower((wint_t) *str1) - towlower((wint_t) *str2));
}
