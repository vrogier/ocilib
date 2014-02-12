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
 * $Id: string.c, Vincent Rogier $
 * --------------------------------------------------------------------------------------------- */

#include "ocilib_internal.h"

/* ********************************************************************************************* *
 *                             PRIVATE FUNCTIONS
 * ********************************************************************************************* */

#define COMPUTE_LENTGH(type, ptr, size)     \
    const type *s = (const type *) ptr;     \
    const type *e = (const type *) ptr;     \
    while (*e++) ;                          \
    size = (int) (e - s - (size_t) 1);      \

/* --------------------------------------------------------------------------------------------- *
 * OCI_StringLength
 * --------------------------------------------------------------------------------------------- */

size_t OCI_StringLength
(
    void  *ptr,
    size_t size_elem
)
{
    int size = 0;

    OCI_CHECK(ptr == NULL, 0);

    if (OCILib.nls_utf8 == TRUE)
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
    unsigned char *binary,
    unsigned int   binary_size,
    otext         *buffer
 )
{
    char          hex_str[] = "0123456789ABCDEF";
    unsigned int  i;

    if (buffer)
    {
        for (i = 0; i < binary_size; i++)
        {
            buffer[i * 2 + 0] = hex_str[binary[i] >> 4  ];
            buffer[i * 2 + 1] = hex_str[binary[i] & 0x0F];
        }  
        
        buffer[binary_size * 2] = 0;
    }

    return (binary_size * 2 + 1);
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

    if (!buffer || buffer_size == 0)
    {
        return FALSE;
    }

    request_size++;

    request_size *= OCILib.nls_utf8 ? UTF8_BYTES_PER_CHAR :  sizeof(otext);

    if (!*buffer)
    {
       *buffer = (otext *) OCI_MemAlloc(OCI_IPC_STRING, (size_t) request_size, (size_t) 1, TRUE);
    }
    else if (*buffer_size < request_size)
    {
        *buffer = (otext *) OCI_MemRealloc(*buffer, OCI_IPC_STRING, (size_t) request_size, (size_t) 1);
    }

    if (*buffer)
    {
        *buffer_size = request_size;
        memset(*buffer, 0, *buffer_size);
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
    int len = char_count;

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

    if (*size == -1)
    {
        len  = (int) OCI_StringLength( (void *) src, sizeof(otext));
    }
    else
    {
        len = (*size) / sizeof(otext);
    }

    if (OCILib.use_wide_char_conv)
    {
        dbtext *dst = (dbtext *) OCI_MemAlloc(OCI_IPC_STRING, sizeof(dbtext), len + 1, FALSE);

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
    if (OCILib.use_wide_char_conv && str != NULL)
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

    OCI_CHECK(buffer == NULL, NULL);

    tmp = (dbtext *) OCIStringPtr(env, str);

    if (tmp)
    {
        int len = OCIStringSize(OCILib.env, str) / sizeof(dbtext);
        *buffer = OCI_StringDuplicateFromOracleString(tmp, len);

        if (*buffer)
        {
            *buffer_size = ( unsigned int ) len;
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
    boolean res    = TRUE;
    dbtext *dbstr  = NULL;
    int     dbsize = 0;

    OCI_CHECK(str == NULL, FALSE);

    dbsize = -1;
    dbstr  = OCI_StringGetOracleString(value, &dbsize);

    OCI_CALL3
    (
        res, err,

        OCIStringAssignText(env, err, (oratext *) dbstr, (ub4) dbsize, str)
    )

    OCI_StringReleaseOracleString(dbstr);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_StringGetFromAttrHandle
 * --------------------------------------------------------------------------------------------- */

boolean OCI_StringGetFromAttrHandle
(
    OCI_Connection *con,
    void           *handle,
    unsigned int    type,
    unsigned int    attr,
    otext         **str
)
{
    boolean res     = TRUE;
    dbtext *dbstr   = NULL;
    int     dbsize  = -1;
    boolean is_ansi = FALSE;

    OCI_CHECK(str == NULL, FALSE);

    OCI_CALL2
    (
        res, con,

        OCIAttrGet((dvoid *) handle,
                   (ub4    ) type,
                   (dvoid *) &dbstr,
                   (ub4   *) &dbsize,
                   (ub4    ) attr,
                   con->err)
    )

    if (res && dbstr)
    {
        /*  Oracle BUG using OCI in Unicode mode (once again...) 
            Some connection server handle attributes are returned 
            as ANSI buffer even when OCI is initialized in UTF16 mode
            Some we check if the first character slot has any zero bytes set 
            to detect this defect ! */
    
        if ((OCI_CHAR_WIDE == OCILib.charset) && dbsize > 1)
        {
            char *ptr = (char*) dbstr;

            if (ptr[0] != 0 && ptr[1] != 0)
            {
               /* ansi buffer returned instead of an UTF16 one ! */
               is_ansi = TRUE;
               dbsize  = (dbsize / sizeof(char)) * sizeof(dbtext);
            }
        }

        *str = (otext *) OCI_MemAlloc(OCI_IPC_STRING, sizeof(otext), dbcharcount(dbsize) + 1, TRUE);

        OCI_StringTranslate(dbstr, *str, dbcharcount(dbsize),( is_ansi == TRUE) ? sizeof(char) : sizeof(dbtext), sizeof(otext));

        res = (*str != NULL);
    }

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_StringSetToAttrHandle
 * --------------------------------------------------------------------------------------------- */

boolean OCI_StringSetToAttrHandle
(
    OCI_Connection *con,
    void           *handle,
    unsigned int    type,
    unsigned int    attr,
    otext         **str,
    const otext    *value
)
{
    boolean res    = TRUE;
    dbtext *dbstr  = NULL;
    int     dbsize = -1;

    dbstr = OCI_StringGetOracleString(value, &dbsize);

    if (dbsize == -1)
    {
        dbsize = 0;
    }

    OCI_CALL2
    (
        res, con,

        OCIAttrSet((dvoid *) handle,
                   (ub4    ) type,
                   (dvoid *) dbstr,
                   (ub4    ) dbsize,
                   (ub4    ) attr,
                   con->err)
    )

    OCI_StringReleaseOracleString(dbstr);

    if (res && str)
    {
        OCI_FREE(*str);

        if (value)
        {
            *str = ostrdup(value);
        }
    }

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_StringGetFromType
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_StringGetFromType
(
    OCI_Connection   *con,
    OCI_Column       *col,
    void             *data,
    unsigned int      size,
    otext            *buffer,
    boolean           quote
)
{
    boolean      res = TRUE;
    unsigned int len = 0;

    otext *ptr = buffer;

    if (quote)
    {
        len = OCI_StringAddToBuffer(ptr, len, OTEXT("'"), FALSE); 
        if (ptr) ptr++;
    }
  
    switch (col->datatype)
    {
        case OCI_CDT_TEXT:
        {
            len += OCI_StringAddToBuffer(buffer, len, (otext *) data, quote); 
            break;
        }
        case OCI_CDT_NUMERIC:
        {
            len = OCI_SIZE_BUFFER;
            if (ptr)
            {
                res = OCI_NumberToString(con, data, col->subtype, col->sqlcode, ptr, len, NULL);
            }            
            break;
        }
        case OCI_CDT_DATETIME:
        {
            len = OCI_SIZE_BUFFER;

            if (ptr)
            {           
                OCI_Date    *date = (OCI_Date*) data;
                const otext *fmt  = OCI_GetDefaultFormatDate(con);

                if (date)
                {
                    res = OCI_DateToText(date, fmt, len, ptr); 
                }
                else
                {
                    res = FALSE;
                }
            }
            break;
        }
        case OCI_CDT_TIMESTAMP:
        {
            len = OCI_SIZE_BUFFER;
            if (ptr)
            {
                OCI_Timestamp *tmsp = (OCI_Timestamp *) data;
                const otext   *fmt  = OCI_GetDefaultFormatDate(con);

                if (tmsp)
                {
                    res = OCI_TimestampToText(tmsp, fmt, len, ptr, 0);
                }
                else
                {
                    res = FALSE;
                }
            }
            break;
        }
        case OCI_CDT_INTERVAL:
        {
            len = OCI_SIZE_BUFFER;
            if (ptr)
            {
                OCI_Interval *itv = (OCI_Interval * ) data;

                if (itv)
                {
                    res = OCI_IntervalToText(  itv,
                                               OCI_STRING_DEFAULT_PREC,
                                               OCI_STRING_DEFAULT_PREC,
                                               len,  ptr);
                }
                else
                {
                    res = FALSE;
                }
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
                    len = OCI_StringBinaryToString((unsigned char *) data, len, ptr);  
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
                len = OCI_StringBinaryToString((unsigned char *) data, size, ptr);
            }
            break;
        }
        case OCI_CDT_LOB:
        {
            OCI_Lob *lob = (OCI_Lob *) data;

            if (lob)
            {
                otext lob_buf[OCI_SIZE_BUFFER + 1 ];

                unsigned int read = 0;    

                do
                {                        
                    read = OCI_LobRead(lob, lob_buf, OCI_SIZE_BUFFER);

                    if (OCI_CLOB == lob->type)
                    {
                        lob_buf[read] = 0;
                        len += OCI_StringAddToBuffer(buffer, len, lob_buf, quote);
                    }
                    else
                    {
                        len += OCI_StringBinaryToString((unsigned char *) lob_buf, read, ptr ? ptr + len : ptr);  
                    }  
                }
                while (read >= OCI_SIZE_BUFFER);

                OCI_LobSeek(lob, 0, OCI_SEEK_SET);
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
            len = OCI_SIZE_BUFFER;
            if (ptr)
            {
                OCI_Ref *ref = (OCI_Ref *) data;
                if (ref)
                {
                    res = OCI_RefToText(ref, OCI_SIZE_BUFFER, ptr);
                }
                else
                {
                    res = FALSE;
                }
            }

            break;
        }
        case OCI_CDT_OBJECT:
        {
            OCI_Object *obj = (OCI_Object *) data;
            quote = FALSE;
            if (obj != NULL)
            {
               res = OCI_ObjectToText(obj, &len, ptr);           
            }
            else
            {
                res = FALSE;
            }

            break;
        }
        case OCI_CDT_COLLECTION:
        {
            OCI_Coll *coll = (OCI_Coll *) data;
            quote = FALSE;
            if (coll != NULL)
            {
                OCI_CollToText(coll, &len, ptr);    
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
        if (buffer)
        {
            len = (unsigned int) ostrlen(buffer);
        }

        if (quote )
        {
            len += OCI_StringAddToBuffer(buffer, len, OTEXT("'"), FALSE); 
        }
    }
    else
    {
        len = 0;
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
    char *dst;

    OCI_CHECK(src == NULL, NULL)

    dst = (char *) OCI_MemAlloc(OCI_IPC_STRING, 1, strlen(src) + 1, 0);

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
    int n;

    va_start(args, format);

    n = (int) vsnprintf(str, (size_t) size, format, args);

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
    wchar_t *dst;

    OCI_CHECK(src == NULL, NULL)

    dst = (wchar_t *) OCI_MemAlloc(OCI_IPC_STRING, sizeof(wchar_t), wcslen(src) + 1, 0);

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

