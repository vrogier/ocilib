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

#include "ocilib_internal.h"

/* ********************************************************************************************* *
 *                             PRIVATE FUNCTIONS
 * ********************************************************************************************* */

#define COMPUTE_LENTGH(type, ptr, size)     \
    const type *s = (const type *) (ptr);   \
    const type *e = (const type *) (ptr);   \
    while (*e++) ;                          \
    (size) = (int) (e - s - (size_t) 1);    \

/* --------------------------------------------------------------------------------------------- *
 * OCI_StringLength
 * --------------------------------------------------------------------------------------------- */

size_t OCI_StringLength
(
    const void  *ptr,
    size_t size_elem
)
{
    int size = 0;

    OCI_CHECK(NULL == ptr, 0);

    if (OCILib.nls_utf8)
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
 * OCI_StringBinaryToString
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_StringBinaryToString
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
 * OCI_StringRequestBuffer
 * --------------------------------------------------------------------------------------------- */

boolean OCI_StringRequestBuffer
(
    otext         **buffer,
    unsigned int   *buffer_size,
    unsigned int    request_size
)
{
    boolean res = FALSE;

    if (!buffer || !buffer_size)
    {
        return FALSE;
    }

    request_size++;

    request_size *= OCILib.nls_utf8 ? OCI_UTF8_BYTES_PER_CHAR :  sizeof(otext);

    if (!*buffer)
    {
       *buffer = (otext *) OCI_MemAlloc(OCI_IPC_STRING, (size_t) request_size, (size_t) 1, TRUE);
    }
    else if (*buffer_size < request_size)
    {
        *buffer = (otext *) OCI_MemRealloc(*buffer, OCI_IPC_STRING, (size_t) request_size, (size_t) 1, TRUE);
    }

    if (*buffer)
    {
        *buffer_size = request_size;
        res = TRUE;
    }

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_StringTranslate
 * --------------------------------------------------------------------------------------------- */

void OCI_StringTranslate
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
            unsigned int *str2   = (unsigned int   *) dst;

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

            unsigned char *str1  = (unsigned char  *) src;
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
            unsigned int *str2  = (unsigned int  *) dst;

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

            unsigned int *str1   = (unsigned int   *) src;
            unsigned short *str2 = (unsigned short *) dst;
            int i                = 0;

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
            unsigned char *str2  = (unsigned char  *) dst;
            int i                = 0;

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

            unsigned int *str1  = (unsigned int  *) src;
            unsigned char *str2 = (unsigned char *) dst;
            int i               = 0;

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
        memcpy(dst, src,  len * size_char_out);
    }

    memset(((char*) dst) + len * size_char_out, 0, size_char_out);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_StringGetOracleString
 * --------------------------------------------------------------------------------------------- */

dbtext * OCI_StringGetOracleString
(
    const otext  *src,
    int          *size
)
{
    dbtext  *dst = NULL;
    int      len = 0;

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
        len = (*size) / sizeof(otext);
    }

    if (OCILib.use_wide_char_conv)
    {
        dst = (dbtext *) OCI_MemAlloc(OCI_IPC_STRING, sizeof(dbtext), len + 1, FALSE);

        if (dst)
        {
            OCI_StringUTF32ToUTF16( src, dst, len );   
        }
    }
    else
    {
        dst = (dbtext *) src;
    }

    *size = len * sizeof(dbtext);

    return dst;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_StringReleaseOracleString
 * --------------------------------------------------------------------------------------------- */

void OCI_StringReleaseOracleString
(
    dbtext *str
)
{
    if (OCILib.use_wide_char_conv && str)
    {
        OCI_MemFree(str);
    }
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_StringCopyOracleStringToNativeString
 * --------------------------------------------------------------------------------------------- */

int OCI_StringCopyOracleStringToNativeString
(
    const dbtext  *src,
    otext         *dst,
    int            len
)
{
    if (OCILib.use_wide_char_conv)
    {
        OCI_StringUTF16ToUTF32((void *) src, (void *) dst, len);   
    }

    return len;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_StringDuplicateFromOracleString
 * --------------------------------------------------------------------------------------------- */

otext* OCI_StringDuplicateFromOracleString
(
    const dbtext  *src,
    int            len
)
{
    otext *dst = (otext *) OCI_MemAlloc(OCI_IPC_STRING, sizeof(otext), len + 1, FALSE);

    if (dst)
    {
        if (OCILib.use_wide_char_conv)
        {
            OCI_StringUTF16ToUTF32((void *) src, (void *) dst, len);   
        }
        else
        {
            OCI_StringRawCopy((void *) src, (void *) dst, len);       
        }
    }

    return dst;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_StringFromStringPtr
 * --------------------------------------------------------------------------------------------- */

otext * OCI_StringFromStringPtr
(
    OCIEnv           *env,
    OCIString        *str,
    otext          **buffer,
    unsigned int     *buffer_size
)
{
    dbtext *tmp = NULL;

    OCI_CHECK(NULL == buffer, NULL);

    tmp = (dbtext *) OCIStringPtr(env, str);

    if (tmp)
    {
        const size_t length = OCIStringSize(OCILib.env, str) / sizeof(dbtext);

        if (!(*buffer))
        {
            *buffer = OCI_MemAlloc(OCI_IPC_STRING, sizeof(otext), length + 1, FALSE);
        }
        else if ((*buffer_size) < ((length + 1) * sizeof(otext)))
        {
            *buffer = OCI_MemRealloc((void*)  *buffer, OCI_IPC_STRING, sizeof(otext), length + 1, FALSE);
        }

        if (*buffer)
        {
            *buffer_size = (unsigned int) ( (length + 1) * sizeof(otext) );

            if (OCILib.use_wide_char_conv)
            {
                OCI_StringUTF16ToUTF32((void *)tmp, (void *)*buffer, (int) length);
            }
            else
            {
                OCI_StringRawCopy((void *)tmp, (void *)*buffer, (int) length);
            }
        }
    }

    return *buffer;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_StringToStringPtr
 * --------------------------------------------------------------------------------------------- */

boolean OCI_StringToStringPtr
(
    OCIEnv      *env,
    OCIString  **str,
    OCIError    *err,
    const otext *value
)
{
    dbtext *dbstr = NULL;
    int     dbsize = -1;

    OCI_CALL_DECLARE_CONTEXT(TRUE)

    OCI_CHECK(NULL == str, FALSE);

    OCI_CALL_CONTEXT_SET_FROM_ERR(err)

    dbstr  = OCI_StringGetOracleString(value, &dbsize);

    OCI_EXEC(OCIStringAssignText(env, err, (oratext *) dbstr, (ub4) dbsize, str))

    OCI_StringReleaseOracleString(dbstr);

    return OCI_STATUS;
}


/* --------------------------------------------------------------------------------------------- *
* OCI_StringFreeStringPtr
* --------------------------------------------------------------------------------------------- */

boolean OCI_StringFreeStringPtr
(
    OCIEnv      *env,
    OCIString  **str,
    OCIError    *err
)
{
    OCI_CALL_DECLARE_CONTEXT(TRUE)    
    OCI_CHECK(NULL == str, FALSE);    
    OCI_CALL_CONTEXT_SET_FROM_ERR(err)

    OCI_EXEC(OCIStringResize(env, err, (ub4)0, str))

    return OCI_STATUS;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetStringAttribute
 * --------------------------------------------------------------------------------------------- */

boolean OCI_GetStringAttribute
(
    OCI_Connection *con,
    void           *handle,
    unsigned int    type,
    unsigned int    attr,
    otext         **str,
    unsigned int   *size

)
{
    dbtext *dbstr  = NULL;
    int     dbsize = -1;
    int     len    = 0;

    OCI_CALL_DECLARE_CONTEXT(TRUE)
        
    OCI_CHECK(NULL == str,  FALSE)
    OCI_CHECK(NULL == size, FALSE)

    OCI_CALL_CONTEXT_SET_FROM_CONN(con)

    OCI_GET_ATTRIB(type, attr, handle, &dbstr, &dbsize)

    if (OCI_STATUS && dbstr)
    {
        boolean is_ansi = FALSE;

        /*  Oracle BUG using OCI in Unicode mode (once again...) 
            Some attributes are returned as ANSI buffer even when OCI is initialized in UTF16 mode
            Some we check if the first character slot has any zero bytes set 
            to detect this defect ! */
    
        if ((OCI_CHAR_WIDE == OCILib.charset) && dbsize > 1)
        {
            char *ptr = (char*) dbstr;

            if (ptr[0] != 0 && ptr[1] != 0)
            {
                /* ANSI buffer returned instead of an UTF16 one ! */
                is_ansi = TRUE;
                len = dbsize;
            }
        }

        // if the input buffer was not ANSI while using an UTF16 environment, compute string length from returned buffer size
        if (len == 0)
        {
            len = dbcharcount(dbsize);
        }

        if (OCI_StringRequestBuffer(str, size, len))
        {
            OCI_StringTranslate(dbstr, *str, len, is_ansi ? sizeof(char) : sizeof(dbtext), sizeof(otext));
        }

        OCI_STATUS = (NULL != *str);
    }

    return OCI_STATUS;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_SetStringAttribute
 * --------------------------------------------------------------------------------------------- */

boolean OCI_SetStringAttribute
(
    OCI_Connection *con,
    void           *handle,
    unsigned int    type,
    unsigned int    attr,
    otext         **str,
    const otext    *value
)
{
    dbtext *dbstr = NULL;
    int     dbsize = -1;

    OCI_CALL_DECLARE_CONTEXT(TRUE)
    OCI_CALL_CONTEXT_SET_FROM_CONN(con)

    dbstr = OCI_StringGetOracleString(value, &dbsize);

    if (dbsize == -1)
    {
        dbsize = 0;
    }

    OCI_SET_ATTRIB(type, attr, handle, dbstr, dbsize)

    OCI_StringReleaseOracleString(dbstr);

    if (OCI_STATUS && str)
    {
        OCI_FREE(*str)

        if (value)
        {
            *str = ostrdup(value);
        }
    }

    return OCI_STATUS;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_StringGetFromType
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_StringGetFromType
(
    OCI_Connection   *con,
    OCI_Column       *col,
    void             *data,
    unsigned int      data_size,
    otext            *buffer,
    unsigned int      buffer_size,
    boolean           quote
)
{
    boolean      res = TRUE;
    unsigned int len = 0;

    /* if buffer is not null, then buffer_size must the > 0 */

    otext *ptr = buffer;

    if (quote && data)
    {
        len = OCI_StringAddToBuffer(ptr, len, OTEXT("'"), FALSE); 
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
            len += OCI_StringAddToBuffer(buffer, len, (otext *) data, quote); 
            break;
        }
        case OCI_CDT_BOOLEAN:
        {
            if (ptr)
            {
                if (data)
                {
                    len += OCI_StringAddToBuffer(buffer, len, (*(boolean*)data) ? OCI_STRING_TRUE : OCI_STRING_FALSE, quote);
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
            if (ptr)
            {
                res = OCI_NumberToString(con, data, col->subtype, ptr, buffer_size, NULL);
            }    
            else
            {
                len = OCI_SIZE_TMP_CVT;
            }
            break;
        }
        case OCI_CDT_DATETIME:
        {
            if (ptr)
            {           
                OCI_Date    *date = (OCI_Date*) data;
                const otext *fmt  = OCI_GetFormat(con, OCI_FMT_DATE);

                res = date ? OCI_DateToText(date, fmt, buffer_size, ptr) : FALSE;
            }
            else
            {
                len = OCI_SIZE_TMP_CVT;
            }
            break;
        }
        case OCI_CDT_TIMESTAMP:
        {
            if (ptr)
            {
                OCI_Timestamp *tmsp = (OCI_Timestamp *) data;
                const otext   *fmt = OCI_GetFormat(con, tmsp && tmsp->type == OCI_TIMESTAMP_TZ ? OCI_FMT_TIMESTAMP_TZ : OCI_FMT_TIMESTAMP);

                 res = tmsp ? OCI_TimestampToText(tmsp, fmt, buffer_size, ptr, 0) : FALSE;
            }
            else
            {
                len = OCI_SIZE_TMP_CVT;
            }
            break;
        }
        case OCI_CDT_INTERVAL:
        {
            if (ptr)
            {
                OCI_Interval *itv = (OCI_Interval * ) data;

                res = itv ? OCI_IntervalToText(itv, OCI_STRING_DEFAULT_PREC, OCI_STRING_DEFAULT_PREC, buffer_size, ptr) : FALSE;
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

            if (lg)
            {
                if (OCI_CLONG == col->subtype)
                {
                    len = OCI_StringAddToBuffer(buffer, len, (otext*) OCI_LongGetBuffer(lg), quote);
                }
                else
                {
                    len = OCI_StringBinaryToString((unsigned char *) data, buffer_size, ptr); 
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
            if (data)
            {
                len = OCI_StringBinaryToString((unsigned char *) data, data_size, ptr);
            }
            break;
        }
        case OCI_CDT_LOB:
        {
            OCI_Lob *lob = (OCI_Lob *) data;

            if (lob)
            {
                if (ptr)
                {
                    unsigned char lob_buf[(OCI_SIZE_BUFFER + 1) * OCI_UTF8_BYTES_PER_CHAR];

                    while (res)
                    {
                        unsigned int bytes_count = OCI_SIZE_BUFFER * OCI_UTF8_BYTES_PER_CHAR;
                        unsigned int char_count = 0;

                        res = OCI_LobRead2(lob, lob_buf, &char_count, &bytes_count);

                        if (bytes_count == 0)
                        {
                            // lob eof reached
                            break;
                        }

                        if (OCI_CLOB == lob->type)
                        {
                            len += OCI_StringAddToBuffer(buffer, len, (otext*)lob_buf, quote);
                        }
                        else
                        {
                            len += OCI_StringBinaryToString(lob_buf, bytes_count, ptr + len);
                        }
                    }

                    OCI_LobSeek(lob, 0, OCI_SEEK_SET);
                }
                else
                {
                    len = (unsigned int)OCI_LobGetLength(lob);

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

            if (file)
            {
                const otext * dir  = OCI_FileGetDirectory(file);
                const otext * name =  OCI_FileGetName(file);

                len += OCI_StringAddToBuffer(buffer, len, dir, TRUE); 
                len += OCI_StringAddToBuffer(buffer, len, OTEXT("/"), TRUE); 
                len += OCI_StringAddToBuffer(buffer, len, name, TRUE); 
            }
            else
            {
                res = FALSE;
            }

            break;

        }
        case OCI_CDT_REF:
        {
            if (ptr)
            {
                OCI_Ref *ref = (OCI_Ref *) data;
                res = ref ? OCI_RefToText(ref, buffer_size, ptr) : FALSE;
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
            res = obj ? OCI_ObjectToText(obj, &real_size, ptr) : FALSE;
            len = real_size;
            break;
        }
        case OCI_CDT_COLLECTION:
        {
            OCI_Coll *coll = (OCI_Coll *) data;
            unsigned int real_size = buffer_size;
            quote = FALSE;
            res = coll ? OCI_CollToText(coll, &real_size, ptr) : FALSE;
            len = real_size;
            break;
        }
        case OCI_CDT_CURSOR:
        {
            OCI_Statement *stmt = (OCI_Statement *)data;
            quote = TRUE;
            if (stmt)
            {
                len = OCI_StringAddToBuffer(buffer, len, stmt->sql, quote);
            }
            else
            {
                res = FALSE;
            }
            break;
        }
        default:
        {
            res = FALSE;
            quote = FALSE;
        }
    }

    if (res)
    {
        if (buffer && buffer_size > len)
        {
            /* the resulting string shall be computed as it was not known in advanded (e.g. numeric types) */
            len = (unsigned int) ostrlen(buffer);
        }

        if (quote && data)
        {
            len += OCI_StringAddToBuffer(buffer, len, OTEXT("'"), FALSE); 
        }
    }
    else
    {
        len = 0;
    }

    /* make sure it is null terminated */
    if (buffer)
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
 * OCI_StringAddToBuffer
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_StringAddToBuffer
(
    otext           *buffer,
    unsigned int     offset,
    const otext     *str,
    boolean          check_quote
)
{
    unsigned int  len_in   = 0;
    unsigned int  len_out  = 0;

    if (!str)
    {
        return 0;
    }

    len_in = (unsigned int) ostrlen(str);
  
    if (check_quote)
    {
        if (buffer)
        {
            const otext *src = str;
            otext       *dst = buffer + offset;
            unsigned int src_len = len_in;

            while (src && *src && src_len)
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

            while (s && *s)
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

        if (buffer)
        {
            ostrcpy(buffer + offset, str);
        }
    }

    return len_out;
}

/* --------------------------------------------------------------------------------------------- *
* OCI_StringGetTypeName
* --------------------------------------------------------------------------------------------- */

unsigned int OCI_StringGetTypeName
(
    const otext *source,
    otext *dest,
    unsigned int length
)
{
    boolean      quote  = FALSE;
    unsigned int offset = 0;

    if (!OCI_STRING_VALID(source) || !dest)
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
* OCI_StringGetFullTypeName
* --------------------------------------------------------------------------------------------- */

unsigned int OCI_StringGetFullTypeName
(
    const otext  *schema,
    const otext  *package,
    const otext  *type,
    const otext  *link,
    otext        *name,
    unsigned int  length
    )
{
    unsigned int offset = 0;

    if (OCI_STRING_VALID(schema))
    {
        offset += OCI_StringGetTypeName(schema, name + offset, length - offset);
        
        if (offset)
        {
            ostrncpy(name + offset, OTEXT("."), length - offset);
            offset++;
        }
    }

    if (OCI_STRING_VALID(package))
    {
        offset += OCI_StringGetTypeName(package, name + offset, length - offset);

        if (offset)
        {
            ostrncpy(name + offset, OTEXT("."), length - offset);
            offset++;
        }
    }

    if (OCI_STRING_VALID(type))
    {
        offset += OCI_StringGetTypeName(type, name + offset, length - offset);
    }

    if (OCI_STRING_VALID(link))
    {
        ostrncpy(name + offset, OTEXT("@"), length - offset);
        offset++;
        offset += OCI_StringGetTypeName(link, name + offset, length - offset);
    }

    return offset;
}

/* ********************************************************************************************* *
 *                            PUBLIC FUNCTIONS
 * ********************************************************************************************* */

/* --------------------------------------------------------------------------------------------- *
 * ocistrdup
 * --------------------------------------------------------------------------------------------- */

char * ocistrdup
(
    const char * src
)
{
    OCI_CHECK(NULL == src, NULL)

    char *dst = (char *) OCI_MemAlloc(OCI_IPC_STRING, 1, strlen(src) + 1, 0);

    if (dst)
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
    if (!str1 && !str2)
    {
        return 0;
    }

    if (!str1)
    {
        return 1;
    }

    if (!str2)
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
    OCI_CHECK(NULL == src, NULL)

    wchar_t *dst = (wchar_t *) OCI_MemAlloc(OCI_IPC_STRING, sizeof(wchar_t), wcslen(src) + 1, 0);

    if (dst)
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
    if (!str1 && !str2)
    {
        return 0;
    }

    if (!str1)
    {
        return 1;
    }

    if (!str2)
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

