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

#define OCILIB_MAJOR_VERSION     5
#define OCILIB_MINOR_VERSION     0
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

/* Calling convention */

#ifndef OCI_API
  #ifdef _MSC_VER
#define OCI_API __stdcall
  #else
#define OCI_API
  #endif
#endif

/* Control of symbol visibility for shared libraries */

#if defined _WINDOWS
#define OCI_SYM_PUBLIC_IMPORT     __declspec(dllimport)
#define OCI_SYM_PUBLIC_EXPORT     __declspec(dllexport)
#define OCI_SYM_LOCAL
#else
  #if __GNUC__ >= 4
#define OCI_SYM_PUBLIC_IMPORT   __attribute__ ((visibility ("default")))
#define OCI_SYM_PUBLIC_EXPORT   __attribute__ ((visibility ("default")))
#define OCI_SYM_LOCAL           __attribute__ ((visibility ("hidden")))
  #else
#define OCI_SYM_PUBLIC_IMPORT
#define OCI_SYM_PUBLIC_EXPORT
#define OCI_SYM_LOCAL
  #endif
#endif

#ifdef OCI_LIB_LOCAL_COMPILE
#define OCI_SYM_PUBLIC
#else
  #ifdef OCI_EXPORT
#define OCI_SYM_PUBLIC  OCI_SYM_PUBLIC_EXPORT
  #else
#define OCI_SYM_PUBLIC  OCI_SYM_PUBLIC_IMPORT
  #endif
#endif

/* Charset modes */

#if defined(OCI_CHARSET_UNICODE) && !defined(OCI_CHARSET_WIDE)
#define OCI_CHARSET_WIDE
#endif

#if defined(OCI_CHARSET_ANSI) && !defined(OCI_CHARSET_MBCS)
#define OCI_CHARSET_MBCS
#endif

#ifdef OCI_CHARSET_WIDE
  #undef OCI_CHARSET_MBCS
  #undef OCI_CHARSET_ANSI
#endif

#ifdef OCI_CHARSET_MBCS
  #undef OCI_CHARSET_WIDE
#endif

#if !defined(OCI_CHARSET_MBCS)  && !defined(OCI_CHARSET_WIDE)
#define OCI_CHARSET_MBCS
#endif

/* keep the legacy flag defined on MBCS builds so that user code
   doing #ifdef OCI_CHARSET_ANSI keeps working */

#if defined(OCI_CHARSET_MBCS) && !defined(OCI_CHARSET_ANSI)
#define OCI_CHARSET_ANSI
#endif

/* printf-style format checking for functions taking an otext format string
   followed by variadic arguments. GCC and Clang check the format/argument
   consistency at compile time; MSVC does not support __attribute__ and the
   wide-format archetypes (wprintf/gnu_wprintf/ms_wprintf) are not consistently
   available across GCC targets, so the macro is only enabled for MBCS builds
   on GCC/Clang and expands to nothing otherwise. */

#if (defined(__GNUC__) || defined(__clang__)) && defined(OCI_CHARSET_MBCS)
#define OCI_ATTR_FORMAT(fmt_idx, arg_idx) __attribute__((format(printf, fmt_idx, arg_idx)))
#else
#define OCI_ATTR_FORMAT(fmt_idx, arg_idx)
#endif

/**
 * @addtogroup OcilibCApiSupportedCharsets
 * @{
 *
 * OCILIB is built in one of two mutually exclusive charset flavors, selected at
 * compile time by defining either OCI_CHARSET_MBCS or OCI_CHARSET_WIDE.
 * The public API is identical in both flavors: every string parameter and return
 * value uses the portable type 'otext', which maps to char or wchar_t.
 *
 * The two flavors differ in which encoding OCI itself is initialized with, and
 * therefore which encoding your application strings must use:
 *
 * @par Option OCI_CHARSET_MBCS — the "MBCS" build (default)
 *
 *  - otext    -->    char
 *  - OTEXT(x) -->    x
 *
 * The name is historical and slightly misleading: this build is NOT limited to
 * single-byte ANSI codepages. OCILIB creates the OCI environment in Oracle's
 * default NLS mode, so the client-side encoding is whatever the Oracle client
 * is configured for — driven by the NLS_LANG environment variable (or the
 * equivalent registry setting on Windows):
 *
 *  - NLS_LANG=AMERICAN_AMERICA.WE8MSWIN1252 → single-byte ANSI strings
 *  - NLS_LANG=AMERICAN_AMERICA.AL32UTF8    → full UTF-8 strings in char*
 *
 * With any UTF-8 Oracle charset in NLS_LANG, char* strings carry UTF-8 end to
 * end: SQL text, object names, fetched data and bound values are all UTF-8
 * between your application and Oracle.
 *
 * @par Option OCI_CHARSET_WIDE — the "wide" build
 *
 *  - otext    -->    wchar_t
 *  - OTEXT(x) -->    L ## x
 *
 * Wide builds do NOT rely on NLS_LANG. OCILIB initializes OCI explicitly in
 * UTF-16 mode: OCIEnvNlsCreate() is called with the OCI_UTF16ID character set
 * for both the charset and the national charset. OCI's Unicode mode uses a
 * fixed-length 2-byte UTF-16 encoding internally.
 *
 * OCILIB bridges wchar_t and that fixed UTF-16 encoding automatically:
 *
 *  - On Windows, wchar_t is 2 bytes and already UTF-16: otext buffers are
 *    passed to and received from Oracle directly, with no conversion.
 *  - On most Unix systems, wchar_t is 4 bytes (UTF-32): OCILIB transparently
 *    converts every string between UTF-16 and UTF-32 — temporary buffers for
 *    statements and object names on the way in, in-place buffer expansion for
 *    fetched and bound strings on the way out. This costs no extra allocation
 *    and is invisible to the application.
 *
 * @warning
 * In wide builds, UTF-16 is used for ALL OCI string traffic — including SQL
 * statement text and object names — regardless of NLS_LANG. Make sure the
 * target database charset is a Unicode charset or a superset of UTF-16;
 * otherwise the Oracle client may silently substitute characters.
 *
 * @par UTF8 strings
 *
 * UTF-8 is fully supported through the OCI_CHARSET_MBCS (MBCS) build:
 * set NLS_LANG to any valid UTF-8 Oracle charset (for example
 * AMERICAN_AMERICA.AL32UTF8) and char* strings carry UTF-8.
 * You do NOT need the wide build for UTF-8 data.
 *
 * @par Choosing a flavor
 *
 *  - Pick OCI_CHARSET_MBCS when your application works in char/UTF-8 —
 *    the common choice on Unix and for portable code.
 *  - Pick OCI_CHARSET_WIDE when your application is natively wchar_t —
 *    typically Windows UNICODE/_UNICODE programs.
 *
 * The flavor is a property of the compiled OCILIB library, not a runtime
 * option: an application must be built with the same OCI_CHARSET_* macro as
 * the OCILIB library it links against. Mixing them leads to ABI mismatches
 * (sizeof(otext) differs).
 *
 **/

#if defined(__cplusplus) && defined(_MSC_VER) && (_MSC_VER < 1300)
#define OCI_EXTERN_CXX_OPEN  extern "C++" {
#define OCI_EXTERN_CXX_CLOSE }
#else
#define OCI_EXTERN_CXX_OPEN
#define OCI_EXTERN_CXX_CLOSE
#endif

OCI_EXTERN_CXX_OPEN

#include <wchar.h>

OCI_EXTERN_CXX_CLOSE

#undef OCI_EXTERN_CXX_OPEN
#undef OCI_EXTERN_CXX_CLOSE

/* Charset macros */

#define OCI_CHAR_MBCS       1
#define OCI_CHAR_WIDE       2

#ifdef OCI_CHARSET_MBCS
typedef char otext;
#define OTEXT(x)          x
#define OCI_CHAR_TEXT     OCI_CHAR_MBCS
#else
typedef wchar_t otext;
#define OTEXT(x)          L ## x
#define OCI_CHAR_TEXT     OCI_CHAR_WIDE
#endif

/**
 * @} OcilibCApiSupportedCharsets
 */

/**
 * @addtogroup OcilibCApiDatatypes
 * @{
*/

/**
 * @typedef OCI_Int16
 * @typedef OCI_UInt16
 * @typedef OCI_Int32
 * @typedef OCI_UInt32
 * @typedef OCI_Int64
 * @typedef OCI_UInt64
 * @typedef OCI_Size
 * @typedef OCI_Float
 * @typedef OCI_Double
 *
 * @brief
 * Fixed width integer types.
 * OCI_Int16 and OCI_UInt16 are 16 bits signed / unsigned integers.
 * OCI_Int32 and OCI_UInt32 are 32 bits signed / unsigned integers.
 * OCI_Int64 and OCI_UInt64 are 64 bits signed / unsigned integers.
 * OCI_Size is used for byte sizes, buffer lengths and element counts.
 * OCI_Float and OCI_Double are the single / double precision floating
 * point types.
 *
 * big_int and big_uint are kept as compatibility aliases for OCI_Int64
 * and OCI_UInt64; new code should use the OCI_ prefixed names.
 *
 */

typedef short OCI_Int16;
typedef unsigned short OCI_UInt16;
typedef int OCI_Int32;
typedef unsigned int OCI_UInt32;
typedef long long OCI_Int64;
typedef unsigned long long OCI_UInt64;
typedef size_t OCI_Size;
typedef float OCI_Float;
typedef double OCI_Double;

/* compatibility aliases */

typedef OCI_Int64 big_int;
typedef OCI_UInt64 big_uint;

#define OCI_BIG_UINT_ENABLED

/* boolean values */

#ifndef TRUE
#define TRUE                   1
#define FALSE                  0
#endif

typedef int OCI_Boolean;

/**
 * @} OcilibCApiDatatypes
 */

/**
 * @} OcilibCApi
 */

#endif /* OCILIB_OCILIBC_PLATFORM_H_INCLUDED */
