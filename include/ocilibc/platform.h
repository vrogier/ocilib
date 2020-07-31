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

/* IMPORTANT NOTICE
 *
 * This file contains explanations about Oracle and OCI technologies. 
 * OCILIB is a wrapper around OCI and thus exposes OCI features. 
 * The OCILIB documentation intends to explain Oracle / OCI concepts
 * and is naturally based on the official Oracle OCI documentation. 
 * 
 * Some parts of OCILIB documentation may include some information 
 * taken and adapted from the following Oracle documentations :
 *  - Oracle Call Interface Programmer's Guide
 *  - Oracle Streams - Advanced Queuing User's Guide
 */

#ifndef OCILIB_OCILIBC_PLATFORM_H_INCLUDED
#define OCILIB_OCILIBC_PLATFORM_H_INCLUDED

/**
 * @addtogroup OcilibCApi
 * @{
 *
 */

/* --------------------------------------------------------------------------------------------- *
 * Platform configuration
 * --------------------------------------------------------------------------------------------- */

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

/* --------------------------------------------------------------------------------------------- *
 * C headers
 * --------------------------------------------------------------------------------------------- */

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <wctype.h>
#include <string.h>
#include <time.h>
#include <limits.h>

/* --------------------------------------------------------------------------------------------- *
 * MS Windows platform detection
 * --------------------------------------------------------------------------------------------- */

#ifndef _WINDOWS
#if defined(_WIN32)|| defined(WIN32) || defined(_WIN64) || defined(WIN64) || defined(_WIN32_WINNT)
    #define _WINDOWS
#endif
#endif

#ifdef _WINDOWS
#ifdef boolean
    #undef boolean
#endif
#include <Windows.h>
#ifdef boolean
    #undef boolean
#endif
#endif

/* --------------------------------------------------------------------------------------------- *
 * OCILIB version information
 * --------------------------------------------------------------------------------------------- */

#define OCILIB_MAJOR_VERSION     4
#define OCILIB_MINOR_VERSION     7
#define OCILIB_REVISION_VERSION  0

/* Import mode */

#define OCI_IMPORT_MODE_LINKAGE  1
#define OCI_IMPORT_MODE_RUNTIME  2

#ifdef OCI_IMPORT_RUNTIME
#undef OCI_IMPORT_LINKAGE
#endif

#ifdef OCI_IMPORT_LINKAGE
  #undef OCI_IMPORT_RUNTIME
#endif

#if !defined(OCI_IMPORT_RUNTIME) && !defined(OCI_IMPORT_LINKAGE)
  #define OCI_IMPORT_LINKAGE
#endif

#ifdef OCI_IMPORT_RUNTIME
#define OCI_IMPORT_MODE OCI_IMPORT_MODE_RUNTIME
#else
  #define OCI_IMPORT_MODE OCI_IMPORT_MODE_LINKAGE
#endif

/* Charset modes */

#ifdef  OCI_CHARSET_UNICODE
  #define OCI_CHARSET_WIDE
#endif

#ifdef OCI_CHARSET_WIDE
#undef OCI_CHARSET_ANSI
#endif

#ifdef OCI_CHARSET_ANSI
  #undef OCI_CHARSET_ANSI
#endif

#if !defined(OCI_CHARSET_ANSI)  && !defined(OCI_CHARSET_WIDE)
  #define OCI_CHARSET_ANSI
#endif

/* Calling convention */

#ifndef OCI_API
#ifdef _MSC_VER
    #define OCI_API __stdcall
#else
    #define OCI_API
#endif
#endif

/* Build mode */

#ifndef OCI_EXPORT
  #define OCI_EXPORT
#endif

/**
 * @addtogroup OcilibCApiSupportedCharsets
 * @{
 *
 * OCILIB supports both ANSI and Unicode.
 *
 * Oracle started a real Unicode support with Oracle8i but only for bind and fetch data.
 * All SQL and PL/SQ/ statements, database objects names were still only supported in ANSI.
 *
 * With Oracle 9i, Oracle provides a full Unicode support.
 *
 * So depending on the compile time Oracle library or the runtime loaded library, Unicode support differs.
 *
 * OCILIB supports:
 *
 * - ANSI (char)
 * - Unicode (wchar_t)
 * - UTF8 strings
 *
 * OCILIB uses the character type 'otext' that is a define around char and wchar_t  depending on the charset mode.
 *
 * @par Option OCI_CHARSET_ANSI
 *
 *  - otext    -->    char
 *  - OTEXT(x) -->    x
 *
 * @par Option OCI_CHARSET_WIDE
 *
 *  - otext    -->    wchar_t
 *  - OTEXT(x) -->    L ## x
 *
 * @par Unicode and ISO C
 *
 * Well, ISO C:
 * - does not know anything about Unicode.
 * - makes wide characters support tricky because wide character size is not defined and is freely adaptable by implementations.
 *
 * OCILIB uses char/wchar_t strings for both public interface and internal storage.
 *
 * Unicode builds of OCILIB initialize OCI in UTF16 Unicode mode.
 * Oracle implements this mode with a 2 bytes (fixed length) UTF16 encoding.
 *
 * @warning
 * When using Unicode builds of OCILIB, make sure that the target
 * Database charset is also using an Unicode charset or is a superset of UTF16.
 * If not, strings may be converted with substitution characters by the Oracle client !
 *
 * So, on systems implementing wchar_t as 2 bytes based UTF16 (e.g. Ms Windows),
 * strings are directly passed to Oracle and taken back from it.
 *
 * On other systems (most of the Unix systems) that use UTF32 as encoding, (4 bytes based wchar_t), OCILIB uses:
 * - temporary buffers for statements and object names
 * - buffer expansion from UTF16 to UTF32 for fetch and bind string:
 *     - allocation based on sizeof(wchar_t)
 *     - data filling based on sizeof(short) -> (UTF16 2 bytes)
 *     - data expansion to sizeof(wchar_t).
 *
 * Buffer expansion is done in place and has the advantage of not requiring extra buffer.
 * That reduces the cost of the Unicode/ISO C handling overhead on Unix systems.
 *
 * @par UTF8 strings
 *
 * OCILIB fully supports UTF8 strings :
 * - Within OCI_CHARSET_ANSI builds
 * - NLS_LANG environment variable must be set to any valid UTF8 Oracle charset string
 *
 * @par Charset mapping macros
 *
 * OCILIB main header file provides macro around most common string functions of
 * the C standard library.
 *
 * these macros are based on the model: ostr[libc function name]()
 *
 * xxx is the standard C library string function name without the character type prefix (str/wcs).
 *
 * List of available macros:
 * - ostrdup
 * - ostrcpy
 * - ostrncpy
 * - ostrcat
 * - ostrncat
 * - ostrlen
 * - ostrcmp
 * - ostrcasecmp
 * - osprintf
 * - ostol
 *
**/

#if defined(__cplusplus) && defined(_MSC_VER) && (_MSC_VER < 1300)
extern "C++" {
#endif

#include <wchar.h>

#if defined(__cplusplus) && defined(_MSC_VER) && (_MSC_VER < 1300)
}
#endif

/* Charset macros */

#define OCI_CHAR_ANSI       1
#define OCI_CHAR_WIDE       2

#ifdef OCI_CHARSET_ANSI
typedef char              otext;
#define OTEXT(x)          x
#define OCI_CHAR_TEXT     OCI_CHAR_ANSI
#else
typedef wchar_t           otext;
#define OTEXT(x)          L ## x
#define OCI_CHAR_TEXT     OCI_CHAR_WIDE
#endif

/*
   For ISO conformance, strdup/wcsdup/stricmp/strncasecmp are not used.
   All wide char routines are part of the 1995 Normative Addendum 1 to the ISO C90 standard.
   OCILIB also needs an ANSI equivalent to swprintf => ocisprintf
   Thus OCILIB exports the following helper functions

*/

OCI_EXPORT int ocisprintf
(
    char *      str,
    int         size,
    const char *format,
    ...
);

OCI_EXPORT char * ocistrdup
(
    const char *src
);

OCI_EXPORT int ocistrcasecmp
(
    const char *str1,
    const char *str2
);

OCI_EXPORT wchar_t * ociwcsdup
(
    const wchar_t *src
);

OCI_EXPORT int ociwcscasecmp
(
    const wchar_t *str1,
    const wchar_t *str2
);

/* special defines for Microsoft C runtime that is not C ISO compliant */

#ifdef _WINDOWS

#define vsnprintf  _vsnprintf
#define swprintf _snwprintf

#endif

/* helpers mapping macros */

#ifdef OCI_CHARSET_ANSI
#define ostrdup          ocistrdup
#define ostrcpy          strcpy
#define ostrncpy         strncpy
#define ostrcat          strcat
#define ostrncat         strncat
#define ostrlen          strlen
#define ostrcmp          strcmp
#define ostrcasecmp      ocistrcasecmp
#define osprintf         ocisprintf
#define ostrtol          strtol
#define osscanf          sscanf
#define otoupper         toupper
#define oisdigit         isdigit
#else
#define ostrdup          ociwcsdup
#define ostrcpy          wcscpy
#define ostrncpy         wcsncpy
#define ostrcat          wcscat
#define ostrncat         wcsncat
#define ostrlen          wcslen
#define ostrcmp          wcscmp
#define ostrcasecmp      ociwcscasecmp
#define osprintf         swprintf
#define ostrtol          wcstol
#define osscanf          swscanf
#define otoupper         towupper
#define oisdigit         iswdigit
#endif

/* string size macros */

#define otextsize(s) (ostrlen(s) * sizeof(otext))

/**
 * @} OcilibCApiSupportedCharsets
 */

/**
 * @addtogroup OcilibCApiDatatypes
 * @{
     
/**
 * @typedef big_int
 *
 * @brief
 * big_int is a C scalar integer (32 or 64 bits) depending on compiler support for 64bits integers.
 * big_uint is an unsigned big_int
 *
 */

/* check for long long support */

#if defined(_LONGLONG) || defined(LONG_LONG_MAX) || defined(LLONG_MAX) || defined(__LONG_LONG_MAX__)

/* C99 long long supported */

typedef long long          big_int;
typedef unsigned long long big_uint;

#define OCI_BIG_UINT_ENABLED

#elif defined(_WINDOWS)

/* Microsoft extension supported */

typedef __int64 big_int;
typedef unsigned __int64 big_uint;

  #define OCI_BIG_UINT_ENABLED

#else

typedef int big_int;
typedef unsigned int big_uint;

#endif

/* boolean values */

#ifndef TRUE
  #define TRUE                   1
  #define FALSE                  0
#endif

#ifndef boolean
#define boolean int
#endif

/**
 * @} OcilibCApiDatatypes
 */

/**
 * @} OcilibCApi
 */

#endif /* OCILIB_OCILIBC_PLATFORM_H_INCLUDED */
