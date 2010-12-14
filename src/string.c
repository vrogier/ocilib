/*
    +-----------------------------------------------------------------------------------------+
    |                                                                                         |
    |                               OCILIB - C Driver for Oracle                              |
    |                                                                                         |
    |                                (C Wrapper for Oracle OCI)                               |
    |                                                                                         |
    |                              Website : http://www.ocilib.net                            |
    |                                                                                         |
    |             Copyright (c) 2007-2010 Vincent ROGIER <vince.rogier@ocilib.net>            |
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
 * $Id: string.c, v 3.8.1 2010-12-13 00:00 Vincent Rogier $
 * --------------------------------------------------------------------------------------------- */

#include "ocilib_internal.h"

/* ********************************************************************************************* *
 *                             PRIVATE FUNCTIONS
 * ********************************************************************************************* */

/* --------------------------------------------------------------------------------------------- *
 *                    Widechar/Multibytes String functions
 *
 * StringCopy4to2bytes() and StringCopy2to4bytes() are based on the
 * ConvertUTF source file by Mark E. Davis - Copyright 2001 Unicode, Inc.
 *
 * --------------------------------------------------------------------------------------------- */

/* --------------------------------------------------------------------------------------------- *
 * OCI_StringCopy4to2bytes
 * --------------------------------------------------------------------------------------------- */

int OCI_StringCopy4to2bytes
(
    const unsigned int* src,
    size_t              src_size,
    unsigned short    * dst,
    size_t              dst_size
)
{
    int cp_size = 0;

    const unsigned int *src_end   = NULL;
    const unsigned short *dst_end = NULL;

    OCI_CHECK(src == NULL, 0);
    OCI_CHECK(dst == NULL, 0);

    src_end = src + src_size;
    dst_end = dst + dst_size;

    while (src < src_end)
    {
        unsigned int c;

        if (dst >= dst_end) return -1;

        c = *src++;

        if (c <= UNI_MAX_BMP)
        {
            if ((c >= UNI_SUR_HIGH_START) && (c < UNI_SUR_LOW_END))
                *dst++ = UNI_REPLACEMENT_CHAR;
            else
                *dst++ = (unsigned short) c;

            cp_size++;
        }
        else if (c > UNI_MAX_LEGAL_UTF32)
        {
            *dst++ = UNI_REPLACEMENT_CHAR;

            cp_size++;
        }
        else
        {
            if ((dst +  (size_t) 1) >= dst_end) return -2;

            c -= UNI_BASE;

            if (dst)
            {
                *dst++ = (unsigned short)((c >> UNI_SHIFT) + UNI_SUR_HIGH_START);
                *dst++ = (unsigned short)((c &  UNI_MASK ) + UNI_SUR_LOW_START );
            }

            cp_size++;
            cp_size++;
        }
    }

    return cp_size;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_StringCopy2to4bytes
 * --------------------------------------------------------------------------------------------- */

int OCI_StringCopy2to4bytes
(
    const unsigned short* src,
    size_t                src_size,
    unsigned int        * dst,
    size_t                dst_size
)
{
    int cp_size = 0;

    const unsigned short *src_end = NULL;
    const unsigned int *dst_end   = NULL;

    unsigned int c1, c2;

    OCI_CHECK(src == NULL, 0);
    OCI_CHECK(dst == NULL, 0);

    src_end = src + src_size;
    dst_end = dst + dst_size;

    while (src < src_end)
    {
        c1 = *src++;

        if ((c1 >= UNI_SUR_HIGH_START) && (c1 <= UNI_SUR_HIGH_END))
        {
            if (src < src_end)
            {
                c2 = *src;

                if ((c2 >= UNI_SUR_LOW_START) && (c2 <= UNI_SUR_LOW_END))
                {
                    c1 = ((c1 - UNI_SUR_HIGH_START) << UNI_SHIFT) +
                         (c2 - UNI_SUR_LOW_START )  + UNI_BASE;

                    ++src;
                }
            }
            else
                return -1;
        }

        if (dst >= dst_end) return -2;

        *dst++ = c1;

        cp_size++;
    }

    return cp_size;
}

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

    if (size_elem == sizeof(char))
    {
        const char *s = (const char *) ptr;
        const char *e = (const char *) ptr;

        while (*e++) ;

        size = (int) (e - s - (size_t) 1);
    }
    else if (size_elem == sizeof(short))
    {
        const short *s = (const short *) ptr;
        const short *e = (const short *) ptr;

        while (*e++) ;

        size = (int) (e - s - (size_t) 1);
    }
    else if (size_elem == sizeof(int))
    {
        const int *s = (const int *) ptr;
        const int *e = (const int *) ptr;

        while (*e++) ;

        size = (int) (e - s - (size_t) 1);
    }

    return size;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_StringUTF8Length
 * --------------------------------------------------------------------------------------------- */

int OCI_StringUTF8Length
(
    const char *str
)
{
    int size = 0;

    while (*str)
    {
        if ((*str & 0xc0) != 0x80)
            size++;
        
        str++;
    }

    return size;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetInputString
 * --------------------------------------------------------------------------------------------- */

void * OCI_GetInputString
(
    void  *src,
    int   *size,
    size_t size_char_in,
    size_t size_char_out
)
{
    OCI_CHECK(src  == NULL, NULL);
    OCI_CHECK(size == NULL, NULL);

    if (size_char_in == size_char_out)
    {
        /* in/out type sizes are equal, so no conversion ! */

        if (*size == -1)
            *size = (int) (OCI_StringLength(src, size_char_in) * size_char_in);

        return src;
    }
    else
    {
        /* in/out type size are not equal, so conversion needed ! */

        int char_count = 0;
        void *dest     = NULL;

        if (*size == -1)
            char_count = (int) OCI_StringLength(src, size_char_in);
        else
            char_count = *size / (int) size_char_in;

        *size = 0;

        dest = OCI_MemAlloc(OCI_IPC_STRING, size_char_out, char_count + 1, 0);

        if (dest != NULL)
        {
            unsigned int null_char = 0;

            if (memcmp(src, &null_char, (size_t) size_char_in) != 0)
            {
                if (size_char_in > size_char_out)
                {
                    if ((size_char_in  == sizeof(int  )) &&
                        (size_char_out == sizeof(short)))
                    {
                        /* UTF32 to UTF16 */

                        char_count = OCI_StringCopy4to2bytes
                                     (
                            (unsigned int   *)  src, char_count,
                            (unsigned short *) dest, char_count
                                     );
                    }
                    else
                    {
                        /* widechar to multibytes */

                        char_count = (int) wcstombs(dest, src, (size_t) (char_count+1));
                    }
                }
                else
                {
                    if ((size_char_in  == sizeof(short)) &&
                        (size_char_out == sizeof(int  )))
                    {
                        /* UTF16 to UTF32 */

                        char_count = OCI_StringCopy2to4bytes
                                     (
                            (unsigned short *) src,  char_count,
                            (unsigned int   *) dest, char_count
                                     );
                    }
                    else
                    {
                        /* multibytes to widechar */

                        char_count = (int) mbstowcs(dest, src, (size_t) (char_count+1));
                    }
                }
            }

            *size = char_count * (int) size_char_out;

            memset( (void*) (((char*) dest) + (size_t) (*size)), 0, size_char_out);
        }

        return dest;
    }
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetOutputString
 * --------------------------------------------------------------------------------------------- */

void OCI_GetOutputString
(
    void  *src,
    void  *dest,
    int   *size,
    size_t size_char_in,
    size_t size_char_out
)
{
    if ((src == NULL) || (dest == NULL) || (size == NULL))
        return;

    /* do something only if in/out type sizes are not equal ! */

    if (size_char_in != size_char_out)
    {
        int char_count = 0;

        if (*size == -1)
            char_count = (int) OCI_StringLength(src, size_char_in);
        else
            char_count = *size / (int) size_char_in;

        if (size_char_in > size_char_out)
        {
            if ((size_char_in  == sizeof(int  )) &&
                (size_char_out == sizeof(short)))
            {
                /* UTF32 to UTF16 */

                char_count = OCI_StringCopy4to2bytes
                             (
                    (unsigned int   *)  src, char_count,
                    (unsigned short *) dest, char_count
                             );
            }
            else
            {
                /* widechar to multibytes */

                char_count = (int) wcstombs(dest, src, (size_t) (char_count+1));
            }
        }
        else
        {
            if ((size_char_in  == sizeof(short)) &&
                (size_char_out == sizeof(int  )))
            {
                /* UTF16 to UTF32 */

                char_count = OCI_StringCopy2to4bytes
                             (
                    (unsigned short *) src,  char_count,
                    (unsigned int   *) dest, char_count
                             );
            }
            else
            {
                /* multibytes to widechar */

                char_count = (int) mbstowcs(dest, src, (size_t) (char_count+1));
            }
        }

        *size = char_count * (int) size_char_out;
    }
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_MoveString
 * --------------------------------------------------------------------------------------------- */

void OCI_MoveString
(
    void  *src,
    void  *dst,
    int    char_count,
    size_t size_char_in,
    size_t size_char_out
)
{
    if ((src == NULL) || (dst == NULL))
        return;

    /* raw string packing/expansion without charset conversion */

    if (size_char_out > size_char_in)
    {
        /* expand string */

        if ((size_char_in  == sizeof(short)) &&
            (size_char_out == sizeof(int)))
        {
            /* 2 => 4 bytes */

            unsigned short *str1 = (unsigned short *) src;
            unsigned int *str2   = (unsigned int   *) dst;

            if (*str1 == 0)
                return;

            while (char_count--)
                str2[char_count] = (unsigned int) str1[char_count];
        }

        else if ((size_char_in  == sizeof(char)) &&
                 (size_char_out == sizeof(short)))
        {
            /* 1 => 2 bytes */

            unsigned char *str1  = (unsigned char  *) src;
            unsigned short *str2 = (unsigned short *) dst;

            if (*str1 == 0)
                return;

            while (char_count--)
                str2[char_count] = (unsigned short) str1[char_count];
        }
        else if ((size_char_in  == sizeof(char)) &&
                 (size_char_out == sizeof(int)))
        {
            /* 1 => 4 bytes */

            unsigned char *str1 = (unsigned char *) src;
            unsigned int *str2  = (unsigned int  *) dst;

            if (*str1 == 0)
                return;

            while (char_count--)
                str2[char_count] = (unsigned int) str1[char_count];
        }
    }
    else if (size_char_out < size_char_in)
    {
        /* pack string */

        if ((size_char_in  == sizeof(int)) &&
            (size_char_out == sizeof(short)))
        {
            /* 4 => 2 bytes */

            unsigned int *str1   = (unsigned int   *) src;
            unsigned short *str2 = (unsigned short *) dst;
            int i                = 0;

            if (*str1 == 0)
                return;

            while (++i < char_count)
                str2[i] = (unsigned short) str1[i];
        }
        else if ((size_char_in  == sizeof(short)) &&
                 (size_char_out == sizeof(char)))
        {
            /* 2 => 1 bytes */

            unsigned short *str1 = (unsigned short *) src;
            unsigned char *str2  = (unsigned char  *) dst;
            int i                = 0;

            if (*str1 == 0)
                return;

            while (++i < char_count)
                str2[i] = (unsigned char) str1[i];
        }
        else if ((size_char_in  == sizeof(int)) &&
                 (size_char_out == sizeof(char)))
        {
            /* 4 => 1 bytes */

            unsigned int *str1  = (unsigned int  *) src;
            unsigned char *str2 = (unsigned char *) dst;
            int i               = 0;

            if (*str1 == 0)
                return;

            while (++i < char_count)
                str2[i] = (unsigned char) str1[i];
        }
    }
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ConvertString
 * --------------------------------------------------------------------------------------------- */

void OCI_ConvertString
(
    void  *str,
    int    char_count,
    size_t size_char_in,
    size_t size_char_out
)
{
    /* inplace string packing / expansion */

    OCI_MoveString(str, str, char_count, size_char_in, size_char_out);
}

/* --------------------------------------------------------------------------------------------- *
 *
 * --------------------------------------------------------------------------------------------- */

void OCI_CopyString
(
    void  *src,
    void  *dest,
    int   *size,
    size_t size_char_in,
    size_t size_char_out
)
{
    if ((src == NULL) || (dest == NULL) || (size == NULL))
        return;

    if (size_char_out == size_char_in)
    {
        memcpy(dest, src, (size_t) *size);
        memset((void*) (((char*) dest) + (size_t)    (*size)), 0, (size_t) size_char_out);
    }
    else
        OCI_GetOutputString(src, dest, size, size_char_in, size_char_out);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ReleaseMetaString
 * --------------------------------------------------------------------------------------------- */

void OCI_ReleaseMetaString
(
    void *str
)
{
    if (str == NULL)
        return;

    #ifdef OCI_CHECK_METASTRINGS

    OCI_MemFree(str);

    #endif
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ReleaseDataString
 * --------------------------------------------------------------------------------------------- */

void OCI_ReleaseDataString
(
    void *str
)
{
    if (str == NULL)
        return;

    #ifdef OCI_CHECK_DATASTRINGS

    OCI_MemFree(str);

    #endif
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_StringFromStringPtr
 * --------------------------------------------------------------------------------------------- */

void * OCI_StringFromStringPtr
(
    OCIString *str,
    void     **buf,
    int       *buflen
)
{
    void *tmp = NULL;
    void *ret = NULL;

    int olen  = 0;
    int osize = 0;
    int esize = 0;
    int msize = 0;

    OCI_CHECK(buf    == NULL, NULL);
    OCI_CHECK(buflen == NULL, NULL);

    tmp = OCIStringPtr(OCILib.env, str);

    if (tmp != NULL)
    {

        #if defined(OCI_CHARSET_MIXED)

        /* tmp is ANSI and must be converted to UTF16 */

        esize = 1;
        msize = (int) sizeof(dtext);
        olen  = (int) strlen((char* ) tmp);
        osize = olen * esize;

        #elif defined(OCI_CHECK_DATASTRINGS)

        /* tmp is UTF16 and might be converted to UTF32 on unixes */

        esize = (int) sizeof(odtext);
        msize = (int) sizeof(dtext);
        olen  = (int) OCI_StringLength(tmp, sizeof(odtext));
        osize = olen * esize;

        #else

        OCI_NOT_USED(esize);

        #endif

        /* do we need to use a buffer */

        if (olen > 0)
        {
            /* do we need to allocate/reallocate the buffer */

            if ((*buf) == NULL)
            {
                *buflen = (olen+1) * msize;
                *buf    = OCI_MemAlloc(OCI_IPC_STRING, (size_t) msize,
                                       (size_t) (olen+1), FALSE);
            }
            else if ((*buflen) < ((olen+1) * msize))
            {
                *buflen = (olen+1) * msize;
                *buf    = OCI_MemRealloc(*buf, OCI_IPC_STRING, (size_t) msize,
                                         (size_t) (olen+1));
            }
        }

        #if defined(OCI_CHARSET_MIXED)

        mbstowcs(*buf, tmp, olen + OCI_CVT_CHAR);

        memset( (void*) (((char*) *buf) + (olen*msize)), 0, msize);

        ret = *buf;

        #elif defined(OCI_CHECK_DATASTRINGS)

        OCI_GetOutputDataString(tmp, *buf, &osize);

        memset( (void*) (((char*) *buf) + (osize)), 0, msize);

        ret = *buf;

        #else

        osize = 0;
        ret   = tmp;

        #endif

    }
    else
    {
        ret = tmp;
    }

    return ret;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_StringFromStringPtr
 * --------------------------------------------------------------------------------------------- */

boolean OCI_StringToStringPtr
(
    OCIString **str,
    OCIError   *err,
    void       *value,
    void      **buf,
    int        *buflen
)
{
    boolean res = TRUE;
    void *ostr  = NULL;
    int osize   = 0;

    #ifdef OCI_CHARSET_MIXED

    int olen  = 0;
    int esize = 0;

    #endif

    OCI_CHECK(str    == NULL, FALSE);
    OCI_CHECK(buf    == NULL, FALSE);
    OCI_CHECK(buflen == NULL, FALSE);

    #ifdef OCI_CHARSET_MIXED

    /* value is wchar_t and must be converted to ANSI */

    esize = (int) 1;
    olen  = (int) dtslen((dtext*) value);
    osize = olen;

    /* do we need to use a buffer */

    if (olen > 0)
    {
        /* do we need to allocate/reallocate the buffer */

        if ((*buf) == NULL)
        {
            *buflen = (olen+1) * esize;
            *buf    = OCI_MemAlloc(OCI_IPC_STRING, esize, olen+1, FALSE);
        }
        else if ((*buflen) < ((olen+1) * esize))
        {
            *buflen = (olen+1) * esize;
            *buf    = OCI_MemRealloc(*buf, OCI_IPC_STRING, esize, olen+1);
        }

    }

    wcstombs((char *) *buf, (dtext *) value, olen + OCI_CVT_CHAR);

    ostr = *buf;

    #else

    osize = -1;
    ostr  = OCI_GetInputDataString(value, &osize);

    #endif

    OCI_CALL3
    (
        res, err,

        OCIStringAssignText(OCILib.env, err, (oratext *) ostr, (ub4) osize, str)
    )

    #ifndef OCI_CHARSET_MIXED

    OCI_ReleaseDataString(ostr);

    #endif
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
    mtext         **str
)
{
    boolean res = TRUE;
    void *ostr  = NULL;
    int osize   = -1;

    OCI_CHECK(str == NULL, FALSE);

    OCI_CALL2
    (
        res, con,

        OCIAttrGet((dvoid *) handle,
                   (ub4    ) type,
                   (dvoid *) &ostr,
                   (ub4   *) &osize,
                   (ub4    ) attr,
                   con->err)
    )

    if ((res == TRUE) && (ostr != NULL))
    {
        *str = (void *) OCI_MemAlloc(OCI_IPC_STRING,  sizeof(mtext),
                                     (size_t) ((osize / (int) sizeof(omtext)) + 1),
                                     TRUE);

        if (*str != NULL)
        {
            OCI_CopyString(ostr, *str, &osize, sizeof(omtext), sizeof(mtext));
        }
        else
            res = FALSE;
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
    mtext         **str,
    const mtext    *value
)
{
    boolean res = TRUE;
    void *ostr  = NULL;
    int osize   = -1;

    ostr = OCI_GetInputMetaString(value, &osize);

    if (osize == -1)
        osize = 0;

    OCI_CALL2
    (
        res, con,

        OCIAttrSet((dvoid *) handle,
                   (ub4    ) type,
                   (dvoid *) ostr,
                   (ub4    ) osize,
                   (ub4    ) attr,
                   con->err)
    )

    OCI_ReleaseMetaString(ostr);

    if ((res == TRUE) && (str != NULL))
    {
        OCI_FREE(*str);

        if (value != NULL)
        {
            *str = mtsdup(value);
        }
    }

    return res;
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

    if (dst != NULL)
        strcpy(dst, src);

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
    if (str1 == NULL && str2 == NULL)
        return 0;

    if (str1 == NULL)
        return 1;

    if (str2 == NULL)
        return -1;

    while (((*str1) != 0) &&
           ((*str2) != 0) &&
           (tolower((int)(*str1)) == tolower((int)(*str2))))
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

#ifdef OCI_INCLUDE_WCHAR

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

    if (dst != NULL)
        wcscpy(dst, src);

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
    if (str1 == NULL && str2 == NULL)
        return 0;

    if (str1 == NULL)
        return 1;

    if (str2 == NULL)
        return -1;

    while ((*str1 != 0) && (*str2 != 0) &&
           (towlower((wint_t)*str1) == towlower((wint_t)*str2)))
    {
        str1++;
        str2++;
    }

    return (towlower((wint_t) *str1) - towlower((wint_t) *str2));
}

#endif
