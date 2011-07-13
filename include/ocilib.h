/*
    +-----------------------------------------------------------------------------------------+
    |                                                                                         |
    |                               OCILIB - C Driver for Oracle                              |
    |                                                                                         |
    |                                (C Wrapper for Oracle OCI)                               |
    |                                                                                         |
    |                              Website : http://www.ocilib.net                            |
    |                                                                                         |
    |             Copyright (c) 2007-2011 Vincent ROGIER <vince.rogier@ocilib.net>            |
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

    +-----------------------------------------------------------------------------------------+
    |                                     IMPORTANT NOTICE                                    |
    +-----------------------------------------------------------------------------------------+
    |                                                                                         |
    |             This file contains explanations about Oracle and OCI  technologies.         |
    |             OCILIB is a wrapper around OCI and thus exposes OCI features.               |
    |             The OCILIB documentation intends to explain Oracle / OCI concepts           |
    |             and is naturally based on the official Oracle OCI documentation.            |
    |                                                                                         |
    |             Some parts of OCILIB documentation may include some informations            |
    |             taken and adapted from the following Oracle documentations :                |
    |                 - Oracle Call Interface Programmer's Guide                              |
    |                 - Oracle Streams - Advanced Queuing User's Guide                        |
    |                                                                                         |
    +-----------------------------------------------------------------------------------------+

 */

/* --------------------------------------------------------------------------------------------- *
 * $Id: ocilib.h, v 3.9.2 2011-07-13 00:00 Vincent Rogier $
 * --------------------------------------------------------------------------------------------- */

#ifndef OCILIB_H_INCLUDED
#define OCILIB_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/**
 * @mainpage
 *
 * @image html logo-160x120.png
 *
 * @section s_intro Introduction
 *
 * OCILIB is an open source and portable Oracle Driver that delivers really fast
 * and reliable access to Oracle databases.
 *
 * The OCILIB library :
 *
 * - offers a rich, full featured and easy to use API
 * - runs on all Oracle platforms
 * - is written in pure ISO C code with native ISO C Unicode support
 * - encapsulates OCI (Oracle Call Interface)
 * - is the most complete available OCI wrapper
 *
 * @section s_version Version information
 *
 * <b>Current version : 3.9.2 (2011-07-13)</b>
 *
 * @section s_feats Main features
 *
 * - Full Ansi and Unicode support on all platforms (ISO C wide strings or UTF8 strings)
 * - Full 32/64 bits compatibility
 * - Comptabile with all Oracle version >= 8i
 * - Automatic adaptation to the runtime Oracle client version
 * - Runtime loading of Oracle libraries
 * - Builtin error handling (global and thread context)
 * - Full support for SQL API and Object API
 * - Full support for ALL Oracle SQL and PL/SQL datatypes (scalars, objects, refs, collections, ..)
 * - Full support for PL/SQL (blocks, cursors, Index by Tables and Nested tables)
 * - Support for non scalar datatype with trough library objects 
 * - Oracle Pooling (connections and sessions pools)
 * - Oracle XA connectivity (X/Open Distributed Transaction Processing XA interface)
 * - Oracle AQ (Advanded Queues)
 * - Oracle TAF (Transparent Application Failover) and HA (High availabality) support 
 * - Binding array Interface
 * - Returning DML feature
 * - Scrollable statements
 * - Statement cache
 * - Direct Path loading
 * - Remote Instances Startup/Shutdown
 * - Oracle Database Change notification / Continuous Query Notification
 * - Oracle warnings support
 * - Global and local transactions 
 * - Describe database schema objects
 * - Hash tables API
 * - Portable Threads and mutexes API
 *
 * @section s_down Download
 *
 * Get OCILIB from <a href="http://orclib.sourceforge.net">OCILIB Project page</a>
 * at Sourceforge Website
 *
 * @section s_author Author
 *
 * OCILIB is developed by <a href="mailto:vince.rogier@ocilib.net">Vincent Rogier</a>
 *
 * @section s_changelog ChangeLog
 *
 * @include ChangeLog.txt
 *
 */

/* --------------------------------------------------------------------------------------------- *
 * Platform config
 * --------------------------------------------------------------------------------------------- */

#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

/* --------------------------------------------------------------------------------------------- *
 * C headers
 * --------------------------------------------------------------------------------------------- */

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <limits.h>

/* --------------------------------------------------------------------------------------------- *
 * MS Windows plaform detection
 * --------------------------------------------------------------------------------------------- */

#ifndef _WINDOWS
  #if defined(_WIN32) || defined(_WIN64) ||defined(_WIN32_WINNT)
    #define _WINDOWS
  #endif
#endif

#ifdef _WINDOWS
  #ifdef boolean
    #undef boolean
  #endif
  #include <windows.h>
  #ifdef boolean
    #undef boolean
  #endif
#endif

/* --------------------------------------------------------------------------------------------- *
 * OCILIB version information
 * --------------------------------------------------------------------------------------------- */

#define OCILIB_MAJOR_VERSION     3
#define OCILIB_MINOR_VERSION     9
#define OCILIB_REVISION_VERSION  2

/* --------------------------------------------------------------------------------------------- *
 * Installing OCILIB
 * --------------------------------------------------------------------------------------------- */

/**
 * @defgroup g_install Installing OCILIB
 * @{
 *
 * @par Compatibilities
 *
 * Actual version of OCILIB has been validated on :
 *  - Platforms: Windows, HP/UX, Linux, Mac OS, Solaris, AIX
 *  - Architectures: 32/64bits
 *  - Compilers: GCC / MinGW, MS Compilers, IBM XLC, CCs, LabView
 *  - Oracle versions: 8i, 9i, 10g, 11g
 *
 * @note
 *
 * The validation of OCILIB on OpenVMS is still pending.
 *
 * Please, contact the author if you have validated OCILIB on platforms or compilers not listed here.
 *
 * @par Global build options
 *
 * OCILIB supports the following global build options:
 *
 * => Oracle import modes
 *
 *  - OCI_IMPORT_LINKAGE for linkage at compile time (default on Unix systems)
 *  - OCI_IMPORT_RUNTIME for runtime loading (default with prebuilt OCILIB libraries on MS Windows)
 *
 * => Oracle charset modes
 *
 *  - OCI_CHARSET_ANSI  : ANSI strings (default)
 *  - OCI_CHARSET_WIDE  : wide strings using ISO C wide character
 *  - OCI_CHARSET_MIXED : ANSI for meta data and wide characters for user data
 *  - OCI_CHARSET_UFT8  : UFT8 strings
 *
 * From v3.6.0, OCI_CHARSET_WIDE replaces OCI_CHARSET_UNICODE OCI_CHARSET_UNICODE remains a
 * valid identifier for backward compatibility
 *
 * => Calling convention (WINDOWS ONLY)
 *
 *  - OCI_API = __cdecl or blank for C/C++ only ! (default on Unix systems and non MSVC projects)
 *  - OCI_API = __stdcall to link OCILIB shared library on Ms Windows (default for MSVC projects)
 *
 * @note
 *
 * On Windows, OCI_API MUST be set to __stdcall in order to use prebuilt libraries
 * From v3.5.0, ocilib.h automatically sets OCI_API to  __stdcall with MS compilers
 *
 * @par Installing OCILIB on UNIX like systems
 *
 * OCILIB uses GNU tools for deployment and installation on UNIX like platforms
 *
 * Uncompress the archive (ocilib-x.y.z-gnu.tar.gz)
 *  - $ cd ocilib-x.y.z
 *  - $ ./configure
 *  - $ ./make
 *  - $ ./make install (this step might require admin rights)
 *
 * Check the shared library path environment variable (LD_LIBRARY_PATH, LD_PATH, ...):
 *  - it must include $ORACLE_HOME\[lib|lib32|lib64]
 *  - it must include the path where OCILIB has been installed
 *    (by example, typically /usr/local/lib under Linux)
 *
 * In order to get these values loaded at logon time, export these values in
 * your .profile configuration file :
 *  - > export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$ORACLE_HOME/lib:/usr/local/lib
 *
 * <b>GNU Installation options </b>:
 *
 * OCILIB supports some options that are not needed for most common configurations.
 *
 * List of available options:
 *
 *  - --with-oracle-import=(linkage|runtime)
 *  - --with-oracle-charset=(ansi|wide|mixed) (old mode 'unicode' mapped to 'wide')
 *  - --with-oracle-home=(custom oracle regular client directory)
 *  - --with-oracle-headers-path=(oracle header files directory)
 *  - --with-oracle-lib-path=(oracle shared lib directory)
 *  - --with-oracle-lib-name=(oracle shared lib name)
 *  - --with-custom-loader=(linker flag telling the linker which loader to use
 *      when loading dynamically at runtime Oracle shared libs.
 *      This option must be provide if the platform does not use the default
 *      loader flag '-ldl') and the --with-oracle-import is set to 'runtime'
 *
 * @note
 *
 * --with-oracle-headers-path and --with-oracle-lib-path are meant to be used with
 * Instant client only but can used for regular client of libs and headers are
 * not located in usual folders
 *
 * @note
 *
 * If the Oracle OCI linkage mode is set to 'linkage' (default) and no Oracle lib
 * path is provided, OCILIB configure script tries to located the Oracle library
 * folder following this sequence :
 *  - $ORACLE_HOME/lib32 (32 bits libs)
 *  - $ORACLE_HOME/lib   (32 or 64 bits libs)
 *  - $ORACLE_HOME/lib64 (64 bits libs)
 *
 * @note
 *
 * To compile native 64 bits versions of OCILIB, you need pass your compiler
 * specifics flags to the configure script.
 *
 * To use OCILIB in a project:
 *
 * - include "ocilib.h" in your application
 * - Add the flag -I$USER_LIBS/include to your compiler
 * - Defines OCILIB modes:
 *    - OCI import  mode (-DOCI_IMPORT_LINKAGE | -DOCI_IMPORT_RUNTIME)
 *    - OCI charset mode (-DOCI_CHARSET_ANSI   | -DOCI_CHARSET_WIDE  | -DOCI_CHARSET_MIXED)
 * - Add the flag -L/$ORACLE_HOME/[lib|lib32|lib64] -lclntsh to the linker
 * - Add the flag -L$USER_LIBS/lib -locilib to the linker
 *
 * where :
 * - $USER_LIBS is the folder where OCILIB was installed
 * - $ORACLE_LIB_PATH is Oracle client shared library path
 *
 * Some older version of Oracle 8 have direct path API symbols located in the library libclient8.
 * With these versions, you must include as well the linker flag -lclient8 to use Direct Path API.
 *
 * @par Installing and using OCILIB on Microsoft Windows
 *
 * 32bits and 64bits DLLs are provided for x86 architectures.
 * Visual .NET (2005/2008) solutions are also provided to recompile the Dlls and the demo.
 *
 * - Uncompress the archive (ocilib-x.y.z-windows.zip)
 * - Copy ocilib\include\ocilib.h to a folder listed in the compiler headers folders
 * - Copy ocilib\lib[32|64]\ocilib[x].lib to a folder listed in the linker libraries folders
 * - Copy ocilib\lib[32|64]\ocilib[x].dll to a folder included in the PATH
 *   environment variable
 *
 * [x] is the compiled version of OCILIB ('a' -> ANSI, 'w' -> Unicode, 'm' -> Mixed)
 *
 * To use OCILIB in a project :
 *
 * - include "ocilib.h" in your application
 * - define call convention (OCI_API) to __stdcall
 * - define charset mode (OCI_CHARSET_ANSI | OCI_CHARSET_MIXED| OCI_CHARSET_WIDE | OCI_CHARSET_UFT8)
 *
 * Note for MinGW users:
 * - Precompiled 32bits static libraries libocilib[x].a are provided
 * - To use OCILIB dll's, copy/rename import libraries ocilib[x].lib to libocilib[x].lib
 * - Add the desired version (static/shared + charset) of the library to the linker options
 *
 * @note
 *
 * The OCI import mode (OCI_IMPORT_LINKAGE or OCI_IMPORT_RUNTIME is only used when
 * compiling OCILIB source code

 * @par Oracle Instant Client Support
 *
 * OCILIB supports Oracle Instant Client.
 *
 * On Microsoft Windows, there is no difference between using a regular Oracle
 * client and an Instant Client with OCILIB
 *
 * On Unix-like systems, the Instant Client is divided in different packages.
 *
 * Public headers and shared libs are not part of the same package.
 *
 * So, you must provide the following options to the configure command:
 *
 * - with-oracle-headers-path: location the public header files
 * - with-oracle-lib-path: location the oracle shared lib
 *
 * If your instant client package containing the shared libs does not have a symbolic link
 * 'libclntsh.[shared lib extension]' to the fully qualified shared lib real name,
 * you must create it:
 *
 * Example on Linux:
 *
 * - $ ln -s $ORALIBPATH/libclntsh.so.10.1 $ORALIBPATH/libclntsh.so
 *
 * @}
 *
 */

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
  #undef OCI_CHARSET_MIXED
#endif

#ifdef OCI_CHARSET_MIXED
  #undef OCI_CHARSET_ANSI
  #undef OCI_CHARSET_WIDE
#endif

#ifdef OCI_CHARSET_ANSI
  #undef OCI_CHARSET_MIXED
  #undef OCI_CHARSET_WIDE
#endif

#if !defined(OCI_CHARSET_ANSI)  && !defined(OCI_CHARSET_MIXED) && \
    !defined(OCI_CHARSET_WIDE)
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
 * @defgroup g_charset Charset support
 * @{
 *
 * OCILIB supports ANSI and Unicode charsets
 *
 * Oracle started a real Unicode support with Oracle8i but only for user data.
 * All SQL and PL/SQ/ statements, metadata string, database objects names, etc,
 * ... were still only supported in ANSI.
 *
 * With Oracle 9i, Oracle provides a full Unicode support.
 *
 * So depending on the compile time Oracle library or the runtime loaded
 * library, the Unicode support differs.
 *
 * OCILIB supports:
 *
 * - ANSI (char)
 * - Unicode (wchar_t)
 * - Mixed charset: ANSI for metadata, Unicode for user data
 * - UTF8 strings
 *
 * OCILIB uses two types of strings:
 *
 * - mtext: for metadata, SQL strings, object attributes.
 * - dtext: for input binds and output data
 *
 * mtext and dtext are declared as defines around char and wchar_t depending on the charset option
 *
 * @par Text macro
 *
 *   - MT() macro : 'meta text' -> meta data and strings passed to OCI calls
 *   - DT() macro : 'data text' -> user input/output data
 *
 * @par Option OCI_CHARSET_ANSI
 *
 *  - dtext    -->    char
 *  - DT(x)    -->    x
 *
 *  - mtext    -->    char
 *  - MT(x)    -->    x
 *
 * @par Option OCI_CHARSET_WIDE
 *
 *  - dtext    -->    wchar_t
 *  - DT(x)    -->    L ## x
 *
 *  - mtext    -->    wchar_t
 *  - MT(x)    -->    L ## x
 *
 * @par Option OCI_CHARSET_MIXED
 *
 *  - dtext    -->    wchar_t
 *  - DT(x)    -->    L ## x
 *
 *  - mtext    -->    char
 *  - MT(x)    -->    x
 *
 * @par Unicode and ISO C
 *
 * Well, ISO C:
 * - doesn't know anything about Unicode.
 * - makes wide characters support tricky because the size of a wide character
 *   is not defined and is freely adaptable by implementations.
 *
 * OCILIB uses char/wchar_t strings for public interface and internal storage.
 *
 * OCILIB, for Unicode builds, initialize OCI in UTF16 Unicode mode. Oracle
 * implements this mode with a 2 bytes (fixed length) UTF16 encoding.
 *
 * So, on systems implementing wchar_t as 2 bytes based UTF16 (e.g. Ms Windows),
 * input strings are directly passed to Oracle and taken back from it.
 *
 * On other systems (most of the Unix systems) that use UTF32 as encoding
 * (4 bytes based wchar_t), OCILIB uses:
 * - temporary buffers to pass string to OCI for metadata strings
 * - buffer expansion from UTF16 to UTF32 for user data string:
 *     - allocation based on sizeof(wchar_t)
 *     - data filling based on sizeof(short) -> (UTF16 2 bytes)
 *     - data expansion to sizeof(wchar_t).
 *
 * The buffer expansion is done in place and has the advantage of not requiring extra buffer.
 * That reduces the cost of the Unicode/ISO C handling overhead on Unix systems.
 *
 * @par UTF8 strings
 *
 * From version 3.6.0, OCILIB fully supports UTF8 strings for all data in OCI_CHARSET_ANSI mode
 * if NLS_LANG environment variable is set to an valid UTF8 Oracle charset string
 *
 * @par Charset mapping macros
 *
 * OCILIB main header file provides macro around most common string functions of
 * the C standard library.
 *
 * these macros are based on the model:
 *
 * - mtsxxx() for mtext * typed strings
 * - dtsxxx() for dtext * typed strings
 *
 * xxx is the standard C library string function name without the character type prefix (str/wcs).
 *
 * List of available macros:
 * - mtsdup, dtsdup
 * - mtscpy, dtscpy
 * - mtsncpy, dtsncpy
 * - mtscat, dtscat
 * - mtsncat, dtsncat
 * - mtslen, dtslen
 * - mtscmp, dtscmp
 * - mtscasecmp, dtscasecmp
 * - mtsprintf, dtsprintf
 * - mtstol, dtstol
 *
**/

/* Unicode mode */

#ifdef OCI_CHARSET_WIDE
  #define OCI_METADATA_WIDE
  #define OCI_USERDATA_WIDE
  #define OCI_INCLUDE_WCHAR
#endif

#ifdef OCI_CHARSET_MIXED
  #define OCI_USERDATA_WIDE
  #define OCI_INCLUDE_WCHAR
#endif

/* include wchar header if needed */

#ifdef OCI_INCLUDE_WCHAR
  #include <wctype.h>

  #ifdef _MSC_VER

    #if (_MSC_VER < 1300) && defined(__cplusplus)
extern "C++" {
    #endif

    #include <wchar.h>
    #if (_MSC_VER < 1300) && defined(__cplusplus)
}
    #endif

  #else

    #include <wchar.h>

  #endif

#endif

/* Charset macros */

#define OCI_CHAR_ANSI       1
#define OCI_CHAR_WIDE       2

#ifdef OCI_METADATA_WIDE
  #define MT(x)           L ## x
  #define mtext           wchar_t
  #define OCI_CHAR_MTEXT  OCI_CHAR_WIDE
#else
  #define MT(x)           x
  #define mtext           char
  #define OCI_CHAR_MTEXT  OCI_CHAR_ANSI
#endif

#ifdef OCI_USERDATA_WIDE
  #define DT(x)           L ## x
  #define dtext           wchar_t
  #define OCI_CHAR_DTEXT  OCI_CHAR_WIDE
#else
  #define DT(x)           x
  #define dtext           char
  #define OCI_CHAR_DTEXT  OCI_CHAR_ANSI
#endif

/*
   For ISO conformance, strdup/wcsdup/stricmp/strncasecmp are not used.
   All wide char routines are part of the 1995 Normative Addendum 1 to the ISO C90 standard.
   OCILIB also needs an ANSI equivalent to swprintf => ocisprintf
   Thus OCILIB exports the following helper functions

*/

OCI_EXPORT int       ocisprintf
(
    char       *str,
    int         size,
    const char *format,
    ...
);
OCI_EXPORT char    * ocistrdup
(
    const char * src
);
OCI_EXPORT int       ocistrcasecmp
(
    const char *str1,
    const char *str2
);

#ifdef OCI_INCLUDE_WCHAR
OCI_EXPORT wchar_t * ociwcsdup
(
    const wchar_t * src
);
OCI_EXPORT int       ociwcscasecmp
(
    const wchar_t *str1,
    const wchar_t *str2
);
#endif

/* special defines for Microsoft C runtime that is not C ISO compliant */

#ifdef _WINDOWS

  #define vsnprintf  _vsnprintf

  #ifdef OCI_INCLUDE_WCHAR
    #define swprintf _snwprintf
  #endif

#endif

/* helpers mapping macros */

#ifdef OCI_METADATA_WIDE
  #define mtsdup          ociwcsdup
  #define mtscpy          wcscpy
  #define mtsncpy         wcsncpy
  #define mtscat          wcscat
  #define mtsncat         wcsncat
  #define mtslen          wcslen
  #define mtscmp          wcscmp
  #define mtscasecmp      ociwcscasecmp
  #define mtsprintf       swprintf
  #define mtstol          wcstol
#else
  #define mtsdup          ocistrdup
  #define mtscpy          strcpy
  #define mtsncpy         strncpy
  #define mtscat          strcat
  #define mtsncat         strncat
  #define mtslen          strlen
  #define mtscmp          strcmp
  #define mtscasecmp      ocistrcasecmp
  #define mtsprintf       ocisprintf
  #define mtstol          strtol
#endif

#ifdef OCI_USERDATA_WIDE
  #define dtsdup          ociwcsdup
  #define dtscpy          wcscpy
  #define dtsncpy         wcsncpy
  #define dtscat          wcscat
  #define dtsncat         wcsncat
  #define dtslen          wcslen
  #define dtscmp          wcscmp
  #define dtscasecmp      ociwcscasecmp
  #define dtsprintf       swprintf
  #define dtstol          wcstol

#else
  #define dtsdup          ocistrdup
  #define dtscpy          strcpy
  #define dtsncpy         strncpy
  #define dtscat          strcat
  #define dtsncat         strncat
  #define dtslen          strlen
  #define dtscmp          strcmp
  #define dtscasecmp      ocistrcasecmp
  #define dtsprintf       ocisprintf
  #define dtstol          strtol
#endif

/* string size macros */

#define mtextsize(s) (mtslen(s) * sizeof(mtext))
#define dtextsize(s) (dtslen(s) * sizeof(dtext))

/**
 * @}
 */

/**
 * @defgroup g_objects Library objects and datatypes
 * @{
 *
 * OCILIB implements:
 *
 * - Oracle Scalar datatypes through scalar C datatypes
 * - Oracle opaque/complex objects though opaque library handles
 * - Library objects for manipulating the database: connections, transactions, statements...
 *
 * @par Supported Oracle datatypes
 *
 * - All Database types are supported excluding REFs.
 *
 * Here is a summary of the supported datatypes:
 *
 * - Scalar types CHAR/NCHAR, VARCHAR2/NVARCHAR2, NUMBER, FLOAT, REAL, RAW, ...
 * - Binary types: RAW, LONG RAW, VARRAW, ..
 * - Larges Objects (Lobs and Files) : BLOB, CLOB, NCLOB, BFILE
 * - LONG types: LONG, VAR LONG
 * - Date, Timestamps et Intervals: DATE, TIMESTAMP, INTERVAL
 * - PL/SQL types: Ref cursors, PL/SQL Tables
 * - Named Types (by value): Built-in system objects and User defined objects
 * - VARRAYs and Nested Tables
 * - ROWIDs
 *
 * @par OCILIB library objects
 *
 * The public OCILIB library interface implements encapsulation for
 * representing database objects (such as connections, statements ...) through
 * opaque structures (pointers to structures whose definition is kept private)
 *
 * Instead of directly manipulating the structures and their members, the library
 * has functions to access the underlying members.
 *
 * It's designed to make the user code as more independent as possible of
 * the library details.
 *
**/

/**
 * @struct OCI_Pool
 *
 * @brief
 * Pool object (session or connection)
 *
 * A pool is a set of pooled objects
 *
 */

typedef struct OCI_Pool OCI_Pool;

/**
 * @struct OCI_Connection
 *
 * @brief
 * Oracle physical connection.
 *
 * It holds all information about a connection such as error handling, associated statements, ...
 * Error handling and transactions are embedded within a connection object.
 *
 * @warning
 * Multithreaded applications that use multiple connections should use one connection per thread
 * as all statements associated with a connection share the same context.
 *
 */

typedef struct OCI_Connection OCI_Connection;

/**
 * @struct OCI_Statement
 *
 * @brief
 * Oracle SQL or PL/SQL statement.
 *
 * A Statement object allows users to prepare, execute SQL orders or PL/SQL blocks
 *
 */

typedef struct OCI_Statement OCI_Statement;

/**
 * @struct OCI_Bind
 *
 * @brief
 * Internal bind representation.
 *
 * A bind object is an object that holds all information about an Oracle statement binding operation
 *
 */

typedef struct OCI_Bind OCI_Bind;

/**
 * @struct OCI_Resultset
 *
 * @brief
 * Collection of output columns from a select statement.
 *
 * A resultset object is the result of 'select' SQL Statement.
 *
 * It's a set of data (ordered in columns) that can be fetched row by row
 * to get data returned by the SQL statement
 *
 */

typedef struct OCI_Resultset OCI_Resultset;

/**
 * @struct OCI_Column
 *
 * @brief
 * Oracle SQL Column and Type member representation.
 *
 * A column object represents an output column from a select statement
 *
 */

typedef struct OCI_Column OCI_Column;

/**
 * @struct OCI_Lob
 *
 * @brief
 * Oracle Internal Large objects:
 *
 * The following internal Larges Objects are supported:
 *
 * - BLOBs           : Binary large objects
 * - CLOBs / NCLOBs  : Character large objects
 *
 * LOBs were introduced by OCI8 to replace Long datatypes.
 *
 * It's designed to store really larges objects (buffer, files) inside the database
 *
 * Oracle encourages programmers to use those objects instead of LONG, LONG RAW, ...
 *
 * OCILIB supports both LOBs and LONGs
 *
 */

typedef struct OCI_Lob OCI_Lob;

/**
 * @struct OCI_File
 *
 * @brief
 * Oracle External Large objects:
 *
 * The following external Larges Objects are supported:
 *
 * - BFILEs : Binary files
 * - CFILEs : Character files
 *
 * FILEs were introduced by OCI8 in order to store references to files located outside the database.
 *
 * @warning
 * Only Read-only access is allowed on BFILEs
 *
 * Two way to use FILEs :
 *
 * - within statement context (query, binding)
 * - without statement context (server files reading) through OCI_File properties functions
 *
 */

typedef struct OCI_File OCI_File;

/**
 * @struct OCI_Transaction
 *
 * @brief
 * Oracle Transaction.
 *
 * A transaction can be:
 *
 * - Local: it's implicitly created by OCILIB
 * - Global: it's explicitly created by the program
 *
 */

typedef struct OCI_Transaction OCI_Transaction;

/**
 * @struct OCI_Long
 *
 * @brief Oracle Long datatype.
 *
 * The following long Objects are supported:
 *
 * - LONG RAW : Binary long objects
 * - LONG     : Character long objects
 *
 * Those types were used in older versions of Oracle (before Oracle8i) to store
 * large chunks of data in the database.
 *
 * It's now depreciated by Oracle that recommends using LOBs
 *
 * Many databases and applications are still designed to use LONGs that's why
 * OCILIB supports Long Objects and piecewise operations
 *
 */

typedef struct OCI_Long OCI_Long;

/**
 * @struct OCI_Date
 *
 * @brief
 * Oracle internal date representation.
 *
 */

typedef struct OCI_Date OCI_Date;

/**
 * @struct OCI_Timestamp
 *
 * @brief
 * Oracle internal timestamp representation.
 *
 */

typedef struct OCI_Timestamp OCI_Timestamp;

/**
 * @struct OCI_Interval
 *
 * @brief
 * Oracle internal interval representation.
 *
 */

typedef struct OCI_Interval OCI_Interval;

/**
 * @struct OCI_Object
 *
 * @brief
 * Oracle Named types representation.
 *
 */

typedef struct OCI_Object OCI_Object;

/**
 * @struct OCI_Coll
 *
 * @brief
 * Oracle Collections (VARRAYs and Nested Tables) representation.
 *
 */

typedef struct OCI_Coll OCI_Coll;

/**
 * @struct OCI_Elem
 *
 * @brief
 * Oracle Collection item representation.
 *
 */

typedef struct OCI_Elem OCI_Elem;

/**
 * @struct OCI_Iter
 *
 * @brief
 * Oracle Collection iterator representation.
 *
 */
typedef struct OCI_Iter OCI_Iter;

/**
 * @struct OCI_TypeInfo
 *
 * @brief
 * Type info metadata handle.
 *
 */

/**
 * @struct OCI_Ref
 *
 * @brief
 * Oracle REF type representation.
 *
 */

typedef struct OCI_Ref OCI_Ref;

/**
 * @struct OCI_TypeInfo
 *
 * @brief
 * Type info metadata handle.
 *
 */

typedef struct OCI_TypeInfo OCI_TypeInfo;

/**
 * @struct OCI_HashTable
 *
 * @brief
 * OCILIB implementation of hash tables.
 *
 */

typedef struct OCI_HashTable OCI_HashTable;

/**
 * @struct OCI_Error
 *
 * @brief
 * Encapsulates an Oracle or OCILIB exception.
 *
 * The error object is used to raise internal or oracle errors.
 * When an error occurs, if the application has provided an error handler, an
 * error object is constructed and passed to the handler
 *
 */

typedef struct OCI_Error OCI_Error;

/**
 * @struct OCI_Mutex
 *
 * @brief
 * OCILIB encapsulation of OCI mutexes.
 *
 */

typedef struct OCI_Mutex OCI_Mutex;

/**
 * @struct OCI_Thread
 *
 * @brief
 * OCILIB encapsulation of OCI Threads.
 *
 */

typedef struct OCI_Thread OCI_Thread;

/**
 * @struct OCI_DirPath
 *
 * @brief
 * OCILIB encapsulation of OCI Direct Path handle.
 *
 */

typedef struct OCI_DirPath OCI_DirPath;

/**
 * @struct OCI_Subscription
 *
 * @brief
 * OCILIB encapsulation of Oracle DCN notification
 *
 */

typedef struct OCI_Subscription OCI_Subscription;

/**
 * @struct OCI_Event
 *
 * @brief
 * OCILIB encapsulation of Oracle DCN event
 *
 */

typedef struct OCI_Event OCI_Event;

/**
 * @struct OCI_Msg
 *
 * @brief
 * OCILIB encapsulation of A/Q message
 *
 */

typedef struct OCI_Msg OCI_Msg;

/**
 * @struct OCI_Agent
 *
 * @brief
 * OCILIB encapsulation of A/Q Agent
 *
 */

typedef struct OCI_Agent OCI_Agent;

/**
 * @struct OCI_Dequeue
 *
 * @brief
 * OCILIB encapsulation of A/Q dequeuing operations
 *
 */

typedef struct OCI_Dequeue OCI_Dequeue;

/**
 * @struct OCI_Enqueue
 *
 * @brief
 * OCILIB encapsulation of A/Q enqueuing operations
 *
 */

typedef struct OCI_Enqueue OCI_Enqueue;

/**
 * @var POCI_ERROR
 *
 * @brief
 * Error procedure prototype
 *
 * @param err - Error handle
 * 
 */

typedef void (*POCI_ERROR)
(
    OCI_Error *err
);

/**
 * @var POCI_THREAD
 *
 * @brief
 * Thread procedure prototype
 *
 * @param thread - Thread handle
 * @param arg    - Pointer passed to OCI_ThreadRun()
 *
 */

typedef void (*POCI_THREAD)
(
    OCI_Thread *thread, 
    void       *arg
);

/**
 * @var POCI_THREADKEYDEST
 *
 * @brief
 * Thread key destructor prototype.
 *
 * @param data - Thread Key current pointer value
 *
 */

typedef void (*POCI_THREADKEYDEST)
(
    void *data
);

/**
 * @var POCI_NOTIFY
 *
 * @brief
 * Database Change Notification User callback prototype.
 *
 * @param event - Event handle
 *
 */

typedef void (*POCI_NOTIFY)
(
    OCI_Event *event
);

/**
 * @var POCI_TAF_HANDLER
 *
 * @brief
 * Failover Notification User callback prototype.
 *
 * @param con   - Connection handle related to the event
 * @param type  - Event type
 * @param event - Event code
 *
 * @note
 * Possible values for parameter 'type' :
 *  - OCI_FOT_NONE
 *  - OCI_FOT_SESSION
 *  - OCI_FOT_SELECT
 *
 * @note
 * Possible values for parameter 'event' :
 *  - OCI_FOE_END
 *  - OCI_FOE_ABORT
 *  - OCI_FOE_REAUTH
 *  - OCI_FOE_BEGIN
 *  - OCI_FOE_ERROR
 *
 * @return
 * User callback should return one iof the following value :
 *  - OCI_FOC_OK 
 *  - OCI_FOC_RETRY
 *
 */

typedef unsigned int (*POCI_TAF_HANDLER)
(
    OCI_Connection *con, 
    unsigned int    type, 
    unsigned int    event
);

/**
 * @var POCI_HA_HANDLER
 *
 * @brief
 * HA (High Availabality) events Notification User callback prototype.
 *
 * @param con    - Connection handle related to the event
 * @param source - Connection handle related to the event
 * @param status - Timestamp of the event
 * @param time   - Timestamp of the event
 *
 * @note
 * Currently, Oracle only send HA down events
 *
 * @note
 * Possible values for parameter 'status' :
 *  - OCI_HET_DOWN : HA event type down
 *  - OCI_HET_UP   : HA event type up
 *
 * @note
 * Possible values for parameter 'event' :
 *  - OCI_HES_INSTANCE                    
 *  - OCI_HES_DATABASE                    
 *  - OCI_HES_NODE                        
 *  - OCI_HES_SERVICE                     
 *  - OCI_HES_SERVICE_MEMBER              
 *  - OCI_HES_ASM_INSTANCE                
 *  - OCI_HES_PRECONNECT                  
 *
 */

typedef void (*POCI_HA_HANDLER)
(
    OCI_Connection *con, 
    unsigned int    source, 
    unsigned int    event, 
    OCI_Timestamp  *time
);

/* public structures */

/**
 * @struct OCI_XID
 *
 * @brief
 * Global transaction identifier
 *
 */

typedef struct OCI_XID {
    long formatID;
    long gtrid_length;
    long bqual_length;
    char data[128];
} OCI_XID;

/**
 * @union OCI_Variant
 *
 * @brief
 * Internal Variant type based on union C type.
 *
 * @note
 *
 * Helpful for generic buffer, it reduces the amount of casts
 *
 */

typedef union OCI_Variant {
    /* integers */
    int num;

    /* raw data */
    unsigned char *p_bytes;

    /* pointer to c natives types */
    void          *p_void;
    int           *p_int;
    double        *p_double;
    dtext         *p_dtext;
    mtext         *p_mtext;

    /* ocilib object types */
    OCI_Date      *p_date;
    OCI_Interval  *p_interval;
    OCI_Timestamp *p_timestamp;
    OCI_Long      *p_long;
    OCI_Lob       *p_lob;
    OCI_File      *p_file;
    OCI_Statement *p_stmt;
    OCI_Column    *p_col;
    OCI_Object    *p_obj;
    OCI_Coll      *p_coll;
    OCI_Iter      *p_iter;
    OCI_Elem      *p_elem;
} OCI_Variant;

/**
* @struct OCI_HashValue
*
* @brief
* Hash table entry value
*
* OCILIB implementation of hash tables uses chaining method for dealing with collisions
*
*/

typedef struct OCI_HashValue {
    OCI_Variant value;
    struct OCI_HashValue *next;
} OCI_HashValue;

/**
 * @struct OCI_HashEntry
 *
 * @brief
 * Hash table entry
 *
 */

typedef struct OCI_HashEntry {
    mtext                 *key;
    struct OCI_HashValue  *values;
    struct OCI_HashEntry  *next;
} OCI_HashEntry;

/**
 * @typedef big_int
 *
 * @brief
 * big_int is a C scalar integer (32 or 64 bits) depending on compiler support for 64bits integers.
 * big_uint is an usigned big_int
 *
 */

/* check for long long support */

#if defined(_LONGLONG) || defined(LONG_LONG_MAX) || defined(LLONG_MAX) || defined(__LONG_LONG_MAX__)

/* C99 long long supported */

typedef long long big_int;
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

/**
 * @}
 */

/* boolean values */

#ifndef TRUE
  #define TRUE                   1
  #define FALSE                  0
#endif

#ifndef boolean
  #define boolean int
#endif

/* oracle OCI key versions*/

#define OCI_8_0                             800
#define OCI_8_1                             810
#define OCI_9_0                             900
#define OCI_9_2                             920
#define OCI_10_1                            1010
#define OCI_10_2                            1020
#define OCI_11_1                            1110
#define OCI_11_2                            1120

/* versions extract macros */

#define OCI_VER_MAJ(v)                      (unsigned int) (v/100)
#define OCI_VER_MIN(v)                      (unsigned int) ((v/10) - ((v/100)*10))
#define OCI_VER_REV(v)                      (unsigned int) ((v) - ((v/10)*10))

/* OCILIB Error types */

#define OCI_ERR_ORACLE                      1
#define OCI_ERR_OCILIB                      2
#define OCI_ERR_WARNING                     3

/* OCILIB Error codes */

#define OCI_ERR_NONE                        0
#define OCI_ERR_NOT_INITIALIZED             1
#define OCI_ERR_LOADING_SHARED_LIB          2
#define OCI_ERR_LOADING_SYMBOLS             3
#define OCI_ERR_MULTITHREADED               4
#define OCI_ERR_MEMORY                      5
#define OCI_ERR_NOT_AVAILABLE               6
#define OCI_ERR_NULL_POINTER                7
#define OCI_ERR_DATATYPE_NOT_SUPPORTED      8
#define OCI_ERR_PARSE_TOKEN                 9
#define OCI_ERR_MAP_ARGUMENT                10
#define OCI_ERR_OUT_OF_BOUNDS               11
#define OCI_ERR_UNFREED_DATA                12
#define OCI_ERR_MAX_BIND                    13
#define OCI_ERR_ATTR_NOT_FOUND              14
#define OCI_ERR_MIN_VALUE                   15
#define OCI_ERR_NOT_COMPATIBLE              16
#define OCI_ERR_STMT_STATE                  17
#define OCI_ERR_STMT_NOT_SCROLLABLE         18
#define OCI_ERR_BIND_ALREADY_USED           19
#define OCI_ERR_BIND_ARRAY_SIZE             20
#define OCI_ERR_COLUMN_NOT_FOUND            21
#define OCI_ERR_DIRPATH_STATE               22
#define OCI_ERR_CREATE_OCI_ENVIRONMENT      23
#define OCI_ERR_REBIND_BAD_DATATYPE         24

/* binding */

#define OCI_BIND_BY_POS                     0
#define OCI_BIND_BY_NAME                    1
#define OCI_BIND_SIZE                       6
#define OCI_BIND_MAX                        1024

/* fetching */

#define OCI_FETCH_SIZE                      20
#define OCI_PREFETCH_SIZE                   20
#define OCI_LONG_EXPLICIT                   1
#define OCI_LONG_IMPLICIT                   2

/* unknown value */

#define OCI_UNKNOWN                         0

/* C Data Type mapping */

#define OCI_CDT_NUMERIC                     1
#define OCI_CDT_DATETIME                    3
#define OCI_CDT_TEXT                        4
#define OCI_CDT_LONG                        5
#define OCI_CDT_CURSOR                      6
#define OCI_CDT_LOB                         7
#define OCI_CDT_FILE                        8
#define OCI_CDT_TIMESTAMP                   9
#define OCI_CDT_INTERVAL                    10
#define OCI_CDT_RAW                         11
#define OCI_CDT_OBJECT                      12
#define OCI_CDT_COLLECTION                  13
#define OCI_CDT_REF                         14

/* Data Type codes for OCI_ImmediateXXX() calls */

#define OCI_ARG_SHORT                       1
#define OCI_ARG_USHORT                      2
#define OCI_ARG_INT                         3
#define OCI_ARG_UINT                        4
#define OCI_ARG_BIGINT                      5
#define OCI_ARG_BIGUINT                     6
#define OCI_ARG_DOUBLE                      7
#define OCI_ARG_DATETIME                    8
#define OCI_ARG_TEXT                        9
#define OCI_ARG_LOB                         10
#define OCI_ARG_FILE                        11
#define OCI_ARG_TIMESTAMP                   12
#define OCI_ARG_INTERVAL                    13
#define OCI_ARG_RAW                         14
#define OCI_ARG_OBJECT                      15
#define OCI_ARG_COLLECTION                  16
#define OCI_ARG_REF                         17

/* statement types */

#define OCI_CST_SELECT                      1
#define OCI_CST_UPDATE                      2
#define OCI_CST_DELETE                      3
#define OCI_CST_INSERT                      4
#define OCI_CST_CREATE                      5
#define OCI_CST_DROP                        6
#define OCI_CST_ALTER                       7
#define OCI_CST_BEGIN                       8
#define OCI_CST_DECLARE                     9

/* environment modes */

#define OCI_ENV_DEFAULT                     0
#define OCI_ENV_THREADED                    1
#define OCI_ENV_CONTEXT                     2
#define OCI_ENV_EVENTS                      4

/* sessions modes */

#define OCI_SESSION_DEFAULT                 0
#define OCI_SESSION_XA                      1
#define OCI_SESSION_SYSDBA                  2
#define OCI_SESSION_SYSOPER                 4
#define OCI_SESSION_PRELIM_AUTH             8

/* change notification types */

#define OCI_CNT_OBJECTS                     1
#define OCI_CNT_ROWS                        2
#define OCI_CNT_DATABASES                   4
#define OCI_CNT_ALL                         OCI_CNT_OBJECTS | OCI_CNT_ROWS | OCI_CNT_DATABASES

/* event notification types */

#define OCI_ENT_STARTUP                     1
#define OCI_ENT_SHUTDOWN                    2
#define OCI_ENT_SHUTDOWN_ANY                3
#define OCI_ENT_DROP_DATABASE               4
#define OCI_ENT_DEREGISTER                  5
#define OCI_ENT_OBJECT_CHANGED              6

/* event object notification types */

#define OCI_ONT_INSERT                      0x2
#define OCI_ONT_UPDATE                      0x4
#define OCI_ONT_DELETE                      0x8
#define OCI_ONT_ALTER                       0x10
#define OCI_ONT_DROP                        0x20
#define OCI_ONT_GENERIC                     0x40

/* database startup modes */

#define OCI_DB_SPM_START                    1
#define OCI_DB_SPM_MOUNT                    2
#define OCI_DB_SPM_OPEN                     4
#define OCI_DB_SPM_FULL                     OCI_DB_SPM_START | OCI_DB_SPM_MOUNT | OCI_DB_SPM_OPEN

/* database startup flags */

#define OCI_DB_SPF_DEFAULT                  0
#define OCI_DB_SPF_FORCE                    1
#define OCI_DB_SPF_RESTRICT                 2

/* database shutdown modes */

#define OCI_DB_SDM_SHUTDOWN                 1
#define OCI_DB_SDM_CLOSE                    2
#define OCI_DB_SDM_DISMOUNT                 4
#define OCI_DB_SDM_FULL                     OCI_DB_SDM_SHUTDOWN | OCI_DB_SDM_CLOSE | OCI_DB_SDM_DISMOUNT

/* database shutdown flags */

#define OCI_DB_SDF_DEFAULT                  0
#define OCI_DB_SDF_TRANS                    1
#define OCI_DB_SDF_TRANS_LOCAL              2
#define OCI_DB_SDF_IMMEDIATE                3
#define OCI_DB_SDF_ABORT                    4

/* charset form types */

#define OCI_CSF_NONE                        0
#define OCI_CSF_DEFAULT                     1
#define OCI_CSF_NATIONAL                    2

/* statement fetch mode */

#define OCI_SFM_DEFAULT                     0
#define OCI_SFM_SCROLLABLE                  0x08

/* statement fetch direction */

#define OCI_SFD_ABSOLUTE                    0x20
#define OCI_SFD_RELATIVE                    0x40

/* bind allocation mode */

#define OCI_BAM_EXTERNAL                    1
#define OCI_BAM_INTERNAL                    2

/* bind direction mode */

#define OCI_BDM_IN                          1
#define OCI_BDM_OUT                         2
#define OCI_BDM_IN_OUT                      (OCI_BDM_IN | OCI_BDM_OUT)

/* Integer sign flag */

#define OCI_NUM_UNSIGNED                    2

/* External Integer types */

#define OCI_NUM_SHORT                       4
#define OCI_NUM_INT                         8
#define OCI_NUM_BIGINT                      16

#define OCI_NUM_DOUBLE                      64

#define OCI_NUM_USHORT                      (OCI_NUM_SHORT  | OCI_NUM_UNSIGNED)
#define OCI_NUM_UINT                        (OCI_NUM_INT    | OCI_NUM_UNSIGNED)
#define OCI_NUM_BIGUINT                     (OCI_NUM_BIGINT | OCI_NUM_UNSIGNED)

/* timestamp types */

#define OCI_TIMESTAMP                       1
#define OCI_TIMESTAMP_TZ                    2
#define OCI_TIMESTAMP_LTZ                   3

/* interval types */

#define OCI_INTERVAL_YM                     1
#define OCI_INTERVAL_DS                     2

/* long types */

#define OCI_BLONG                           1
#define OCI_CLONG                           2

/* lob types */

#define OCI_BLOB                            1
#define OCI_CLOB                            2
#define OCI_NCLOB                           3

/* lob opening mode */

#define OCI_LOB_READONLY                    1
#define OCI_LOB_READWRITE                   2

/* file types */

#define OCI_BFILE                           1
#define OCI_CFILE                           2

/* lob browsing mode */

#define OCI_SEEK_SET                        1
#define OCI_SEEK_END                        2
#define OCI_SEEK_CUR                        3

/* type info types */

#define OCI_TIF_TABLE                       1
#define OCI_TIF_VIEW                        2
#define OCI_TIF_TYPE                        3

/* object type */

#define OCI_OBJ_PERSISTENT                  1
#define OCI_OBJ_TRANSIENT                   2
#define OCI_OBJ_VALUE                       3

/* collection types */

#define OCI_COLL_VARRAY                     1
#define OCI_COLL_NESTED_TABLE               2

/* pool types */

#define OCI_POOL_CONNECTION                 1
#define OCI_POOL_SESSION                    2

/* AQ message state */

#define OCI_AMS_READY                       1
#define OCI_AMS_WAITING                     2
#define OCI_AMS_PROCESSED                   3
#define OCI_AMS_EXPIRED                     4

/* AQ sequence deviation    */

#define OCI_ASD_BEFORE                      2
#define OCI_ASD_TOP                         3

/* AQ message visibility    */

#define OCI_AMV_IMMEDIATE                   1
#define OCI_AMV_ON_COMMIT                   2

/* AQ dequeue mode    */

#define OCI_ADM_BROWSE                      1
#define OCI_ADM_LOCKED                      2
#define OCI_ADM_REMOVE                      3
#define OCI_ADM_REMOVE_NODATA               4

/* AQ dequeue navigation */

#define OCI_ADN_FIRST_MSG                   1
#define OCI_ADN_NEXT_TRANSACTION            2
#define OCI_ADN_NEXT_MSG                    3

/* AQ queue table purge mode */

#define OCI_APM_BUFFERED                    1
#define OCI_APM_PERSISTENT                  2
#define OCI_APM_ALL                         (OCI_APM_BUFFERED | OCI_APM_PERSISTENT)
/* AQ queue table grouping mode */

#define OCI_AGM_NONE                        0
#define OCI_AGM_TRANSACTIONNAL              1

/* AQ queue table type */

#define OCI_AQT_NORMAL                      0
#define OCI_AQT_EXCEPTION                   1
#define OCI_AQT_NON_PERSISTENT              2

/* direct path processing return status */

#define OCI_DPR_COMPLETE                    1
#define OCI_DPR_ERROR                       2
#define OCI_DPR_FULL                        3
#define OCI_DPR_PARTIAL                     4
#define OCI_DPR_EMPTY                       5

/* trace size constants */

#define OCI_SIZE_TRACE_ID                   64
#define OCI_SIZE_TRACE_MODULE               48
#define OCI_SIZE_TRACE_ACTION               32
#define OCI_SIZE_TRACE_INF0                 64

/* trace types */

#define OCI_TRC_IDENTITY                    1
#define OCI_TRC_MODULE                      2
#define OCI_TRC_ACTION                      3
#define OCI_TRC_DETAIL                      4

/* HA event type */

#define OCI_HET_DOWN                        0
#define OCI_HET_UP                          1

/* HA event source */
#define OCI_HES_INSTANCE                    0 
#define OCI_HES_DATABASE                    1
#define OCI_HES_NODE                        2
#define OCI_HES_SERVICE                     3
#define OCI_HES_SERVICE_MEMBER              4
#define OCI_HES_ASM_INSTANCE                5
#define OCI_HES_PRECONNECT                  6

/* Fail over types */

#define OCI_FOT_NONE                        1
#define OCI_FOT_SESSION                     2
#define OCI_FOT_SELECT                      4

/* fail over notifications */

#define OCI_FOE_END                         1
#define OCI_FOE_ABORT                       2
#define OCI_FOE_REAUTH                      4
#define OCI_FOE_BEGIN                       8
#define OCI_FOE_ERROR                       16

/* fail over callback return code */

#define OCI_FOC_OK                          0
#define OCI_FOC_RETRY                       25410

/* hash tables support */

#define OCI_HASH_STRING                     1
#define OCI_HASH_INTEGER                    2
#define OCI_HASH_POINTER                    3

/* transaction types */

#define OCI_TRS_NEW                         0x00000001
#define OCI_TRS_READONLY                    0x00000100
#define OCI_TRS_READWRITE                   0x00000200
#define OCI_TRS_SERIALIZABLE                0x00000400
#define OCI_TRS_LOOSE                       0x00010000
#define OCI_TRS_TIGHT                       0x00020000

/* sql function codes */

#define OCI_SFC_CREATE_TABLE                1
#define OCI_SFC_SET_ROLE                    2
#define OCI_SFC_INSERT                      3
#define OCI_SFC_SELECT                      4
#define OCI_SFC_UPDATE                      5
#define OCI_SFC_DROP_ROLE                   6
#define OCI_SFC_DROP_VIEW                   7
#define OCI_SFC_DROP_TABLE                  8
#define OCI_SFC_DELETE                      9
#define OCI_SFC_CREATE_VIEW                 10
#define OCI_SFC_DROP_USER                   11
#define OCI_SFC_CREATE_ROLE                 12
#define OCI_SFC_CREATE_SEQUENCE             13
#define OCI_SFC_ALTER_SEQUENCE              14

#define OCI_SFC_DROP_SEQUENCE               16
#define OCI_SFC_CREATE_SCHEMA               17
#define OCI_SFC_CREATE_CLUSTER              18
#define OCI_SFC_CREATE_USER                 19
#define OCI_SFC_CREATE_INDEX                20
#define OCI_SFC_DROP_INDEX                  21
#define OCI_SFC_DROP_CLUSTER                22
#define OCI_SFC_VALIDATE_INDEX              23
#define OCI_SFC_CREATE_PROCEDURE            24
#define OCI_SFC_ALTER_PROCEDURE             25
#define OCI_SFC_ALTER_TABLE                 26
#define OCI_SFC_EXPLAIN                     27
#define OCI_SFC_GRANT                       28
#define OCI_SFC_REVOKE                      29
#define OCI_SFC_CREATE_SYNONYM              30
#define OCI_SFC_DROP_SYNONYM                31
#define OCI_SFC_ALTER_SYSTEM_SWITCHLOG      32
#define OCI_SFC_SET_TRANSACTION             33
#define OCI_SFC_PLSQL_EXECUTE               34
#define OCI_SFC_LOCK                        35
#define OCI_SFC_NOOP                        36
#define OCI_SFC_RENAME                      37
#define OCI_SFC_COMMENT                     38
#define OCI_SFC_AUDIT                       39
#define OCI_SFC_NO_AUDIT                    40
#define OCI_SFC_ALTER_INDEX                 41
#define OCI_SFC_CREATE_EXTERNAL_DATABASE    42
#define OCI_SFC_DROP_EXTERNALDATABASE       43
#define OCI_SFC_CREATE_DATABASE             44
#define OCI_SFC_ALTER_DATABASE              45
#define OCI_SFC_CREATE_ROLLBACK_SEGMENT     46
#define OCI_SFC_ALTER_ROLLBACK_SEGMENT      47
#define OCI_SFC_DROP_ROLLBACK_SEGMENT       48
#define OCI_SFC_CREATE_TABLESPACE           49
#define OCI_SFC_ALTER_TABLESPACE            50
#define OCI_SFC_DROP_TABLESPACE             51
#define OCI_SFC_ALTER_SESSION               52
#define OCI_SFC_ALTER_USER                  53
#define OCI_SFC_COMMIT_WORK                 54
#define OCI_SFC_ROLLBACK                    55
#define OCI_SFC_SAVEPOINT                   56
#define OCI_SFC_CREATE_CONTROL_FILE         57
#define OCI_SFC_ALTER_TRACING               58
#define OCI_SFC_CREATE_TRIGGER              59
#define OCI_SFC_ALTER_TRIGGER               60
#define OCI_SFC_DROP_TRIGGER                61
#define OCI_SFC_ANALYZE_TABLE               62
#define OCI_SFC_ANALYZE_INDEX               63
#define OCI_SFC_ANALYZE_CLUSTER             64
#define OCI_SFC_CREATE_PROFILE              65
#define OCI_SFC_DROP_PROFILE                66
#define OCI_SFC_ALTER_PROFILE               67
#define OCI_SFC_DROP_PROCEDURE              68

#define OCI_SFC_ALTER_RESOURCE_COST         70
#define OCI_SFC_CREATE_SNAPSHOT_LOG         71
#define OCI_SFC_ALTER_SNAPSHOT_LOG          72
#define OCI_SFC_DROP_SNAPSHOT_LOG           73
#define OCI_SFC_DROP_SUMMARY                73
#define OCI_SFC_CREATE_SNAPSHOT             74
#define OCI_SFC_ALTER_SNAPSHOT              75
#define OCI_SFC_DROP_SNAPSHOT               76
#define OCI_SFC_CREATE_TYPE                 77
#define OCI_SFC_DROP_TYPE                   78
#define OCI_SFC_ALTER_ROLE                  79
#define OCI_SFC_ALTER_TYPE                  80
#define OCI_SFC_CREATE_TYPE_BODY            81
#define OCI_SFC_ALTER_TYPE_BODY             82
#define OCI_SFC_DROP_TYPE_BODY              83
#define OCI_SFC_DROP_LIBRARY                84
#define OCI_SFC_TRUNCATE_TABLE              85
#define OCI_SFC_TRUNCATE_CLUSTER            86
#define OCI_SFC_CREATE_BITMAPFILE           87
#define OCI_SFC_ALTER_VIEW                  88
#define OCI_SFC_DROP_BITMAPFILE             89
#define OCI_SFC_SET_CONSTRAINTS             90
#define OCI_SFC_CREATE_FUNCTION             91
#define OCI_SFC_ALTER_FUNCTION              92
#define OCI_SFC_DROP_FUNCTION               93
#define OCI_SFC_CREATE_PACKAGE              94
#define OCI_SFC_ALTER_PACKAGE               95
#define OCI_SFC_DROP_PACKAGE                96
#define OCI_SFC_CREATE_PACKAGE_BODY         97
#define OCI_SFC_ALTER_PACKAGE_BODY          98
#define OCI_SFC_DROP_PACKAGE_BODY           99
#define OCI_SFC_CREATE_DIRECTORY            157
#define OCI_SFC_DROP_DIRECTORY              158
#define OCI_SFC_CREATE_LIBRARY              159
#define OCI_SFC_CREATE_JAVA                 160
#define OCI_SFC_ALTER_JAVA                  161
#define OCI_SFC_DROP_JAVA                   162
#define OCI_SFC_CREATE_OPERATOR             163
#define OCI_SFC_CREATE_INDEXTYPE            164
#define OCI_SFC_DROP_INDEXTYPE              165
#define OCI_SFC_ALTER_INDEXTYPE             166
#define OCI_SFC_DROP_OPERATOR               167
#define OCI_SFC_ASSOCIATE_STATISTICS        168
#define OCI_SFC_DISASSOCIATE_STATISTICS     169
#define OCI_SFC_CALL_METHOD                 170
#define OCI_SFC_CREATE_SUMMARY              171
#define OCI_SFC_ALTER_SUMMARY               172
#define OCI_SFC_CREATE_DIMENSION            174
#define OCI_SFC_ALTER_DIMENSION             175
#define OCI_SFC_DROP_DIMENSION              176
#define OCI_SFC_CREATE_CONTEXT              177
#define OCI_SFC_DROP_CONTEXT                178
#define OCI_SFC_ALTER_OUTLINE               179
#define OCI_SFC_CREATE_OUTLINE              180
#define OCI_SFC_DROP_OUTLINE                181
#define OCI_SFC_UPDATE_INDEXES              182
#define OCI_SFC_ALTER_OPERATOR              183

/* size constants */

#define OCI_SIZE_FORMAT                     64
#define OCI_SIZE_BUFFER                     512
#define OCI_SIZE_LONG                       (64*1024)-1
#define OCI_SIZE_DATE                       45
#define OCI_SIZE_TIMESTAMP                  54
#define OCI_SIZE_FORMAT_TODATE              14
#define OCI_SIZE_NULL                       4
#define OCI_SIZE_PRECISION                  10
#define OCI_SIZE_ROWID                      23
#define OCI_SIZE_DIRECTORY                  30
#define OCI_SIZE_FILENAME                   255
#define OCI_SIZE_FORMAT_NUMS                40
#define OCI_SIZE_FORMAT_NUML                65
#define OCI_SIZE_OBJ_NAME                   30

#define OCI_HASH_DEFAULT_SIZE               256

/* string constants */

#define OCILIB_DRIVER_NAME                  MT("OCILIB")
#define OCI_STRING_NULL                     MT("NULL")
#define OCI_STRING_FORMAT_DATE              MT("YYYY-MM-DD")
#define OCI_STRING_DEFAULT_PREC             3
#define OCI_STRING_FORMAT_NUM   \
    MT("FM99999999999999999999999999999999999990.999999999999999999999999")

#ifdef _WINDOWS
  #define OCI_CHAR_SLASH                    '\\'
#else
  #define OCI_CHAR_SLASH                    '/'
#endif

/**
 * @defgroup g_init Initializing the library
 * @{
 *
 * To use OCILIB, it first needs to be initialized through a call to OCI_Initialize().
 *
 * Then, the application connects to server, executes queries...
 *
 * Finally, OCILIB resources must be released by OCI_Cleanup()
 *
 * @note
 *
 * The following objects are automatically freed by the library:
 * - Connections
 * - pools
 * - Statements
 * - Type info objects
 * - Thread keys
 *
 * @warning
 *
 * All other standalone object instances (mutexes, threads, dates, lobs, ...) <b>ARE NOT</b> freed.
 *
 * @par Example
 *
 * @include init.c
 *
 */

/**
 * @brief
 * Initialize the library
 *
 * @param err_handler  - Pointer to error handler procedure (optional)
 * @param lib_path     - Oracle shared library path (optional)
 * @param mode         - Environment mode
 *
 * Possible values for parameter mode:
 * - OCI_ENV_DEFAULT  : default mode
 * - OCI_ENV_THREADED : multithreading support
 * - OCI_ENV_CONTEXT  : thread contextual error handling
 * - OCI_ENV_EVENTS   : enables events for subscription
 *
 * @note
 *
 * This function must be called before any OCILIB library function.
 *
 * - It installs the error handler
 * - It loads the Oracle shared library located in the path pointed by 'home'
 *
 * @warning
 *
 * The parameter 'home' is only used if OCILIB has been built with the option
 * OCI_IMPORT_RUNTIME (default on MS windows but NOT on Unix systems
 *
 * @warning
 *
 * If the parameter 'lib_path' is NULL, the Oracle library is loaded from system
 * environment variables
 *
 * @warning
 *
 * OCI_Initialize() should be called <b>ONCE</b> per application
 *
 * @return
 *
 * TRUE on success otherwise FALSE (only with Oracle runtime loading mode
 * if the oracle shared libraries can't be loaded or if OCI subsystem cannot be initialized)
 *
 */

OCI_EXPORT boolean OCI_API OCI_Initialize
(
    POCI_ERROR   err_handler,
    const mtext *lib_path,
    unsigned int mode
);

/**
 * @brief
 * Clean up all resources allocated by the library
 *
 * @note
 *
 * This function must be the last OCILIB library function call.
 *
 * - It deallocates objects not explicitly freed by the program (connections, statements, ...)
 * - It unloads the Oracle shared library
 *
 * @warning
 *
 * OCI_Cleanup() should be called <b>ONCE</b> per application
 *
 * @return TRUE
 */

OCI_EXPORT boolean OCI_API OCI_Cleanup
(
    void
);

/**
 * @brief
 * Return the version of OCI used for compilation
 *
 * @note
 * - with linkage build option, the version is determined from the oci.h header
 *   through different ways
 * - with runtime loading build option, the version is set to the highest version
 *   of OCI needed by OCILIB, not necessarily the real OCI version
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_GetOCICompileVersion
(
    void
);

/**
 * @brief
 * Return the version of OCI used at runtime
 *
 * @note
 * - with linkage build option, the version is determined from the oci.h header
 *   through different ways
 * - with runtime loading build option, the version determined from the symbols
 *   dynamically loaded.
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_GetOCIRuntimeVersion
(
    void
);

/**
 * @brief
 * Return the Oracle shared library import mode
 *
 * @note
 * Possible values are:
 * - OCI_IMPORT_MODE_LINKAGE
 * - OCI_IMPORT_MODE_RUNTIME
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_GetImportMode
(
    void
);

/**
 * @brief
 * Return the metadata charset type
 *
 * @note
 * Possible values are:
 * - OCI_CHAR_ANSI
 * - OCI_CHAR_WIDE
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_GetCharsetMetaData
(
    void
);

/**
 * @brief
 * Return the user data charset type
 *
 * @note
 * Possible values are:
 * - OCI_CHAR_ANSI
 * - OCI_CHAR_WIDE
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_GetCharsetUserData
(
    void
);

/**
 * @brief
 * Enable or disable Oracle warning notifications
 *
 * @param value  - enable/disable warnings
 *
 * @note
 * Default value is FALSE
 *
 */

OCI_EXPORT void OCI_API OCI_EnableWarnings
(
    boolean value
);

/**
 * @brief
 * Set the global error user handler
 *
 * @param handler  - Pointer to error handler procedure
 *
 * @note
 * Use this call to change or remove the user callback error handler installed by OCI_Initialize()
 *
 */

OCI_EXPORT void OCI_API OCI_SetErrorHandler
(
    POCI_ERROR handler
);


/**
 * @brief
 * Set the High availabality (HA) user handler
 *
 * @param handler - Pointer to HA handler procedure
 *
 * @note
 * See POCI_HA_HANDLER documentation for more details
 *
 * @warning
 * This call is supported from Oracle 10gR2.
 * For previous versions, it returns FALSE without throwing any exception.
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_SetHAHandler
(
    POCI_HA_HANDLER handler
);

/**
 * @}
 */

/**
 * @defgroup g_error Error handling
 * @{
 *
 * OCILIB provides two mechanisms for error handling:
 *
 * - Global error handling through callbacks.
 * - Contextual thread error handling
 *
 * Exceptions are raised:
 *
 * - On Oracle OCI API call error
 * - On Oracle SQL statement error
 * - On Internal OCILIB error (type checking, memory allocations ...)
 * - On Oracle warnings (OCI API or SQL) from v3.5.0 is warnings are enabled
 *
 * If an error handler was provided to OCI_Initialize(), when an error occurs, the
 * library generates an OCI_Error handle and pass it to the error handler.
 *
 * In order to use the thread contextual error handling, you must call
 * OCI_Initialize() with the flag OCI_ENV_CONTEXT for the mode parameter. When
 * activated, error handles are stored per thread and the last error within a
 * thread can be retrieved with OCI_GetLastError()
 *
 * Exception properties are accessible through a set of functions
 *
 * @note
 * The two ways to handle errors are not exclusive and can be mixed.
 *
 * @note
 * Thread contextual error is also available for single thread based applications
 *
 * @par Oracle Warnings
 *
 * Oracle warnings are raised through OCI_Error API.
 * Such error handles have their error type property (OCI_ErrorGetType()) set to OCI_ERR_WARNING.
 * Warning handing is disabled by default. To activate/deactivate it, use OCI_EnableWarnings()
 *
 * @par Example with callbacks
 * @include err.c
 *
 * @par Example with thread context
 * @include err_ctx.c
 *
 *@par Example of warning handling
 * @include err_warning.c
 *
 */

/**
 * @brief
 * Retrieve the last error occurred within the last OCILIB call
 *
 * @note
 * OCI_GetLastError() is based on thread context and thus OCILIB must be
 * initialized with the flag OCI_ENV_CONTEXT
 *
 */

OCI_EXPORT OCI_Error * OCI_API OCI_GetLastError
(
    void
);

/**
 * @brief
 * Retrieve error message from error handle
 *
 * @param err - Error handle
 *
 */

OCI_EXPORT const mtext * OCI_API OCI_ErrorGetString
(
    OCI_Error *err
);

/**
 * @brief
 * Retrieve the type of error from error handle
 *
 * @param err - Error handle
 *
 * @note
 * Returns one of the following values:
 *
 * - OCI_ERR_ORACLE
 * - OCI_ERR_OCILIB
 * - OCI_ERR_WARNING
 *
 * @return
 * Object type or OCI_UNKNOWN the input handle is NULL
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_ErrorGetType
(
    OCI_Error *err
);

/**
 * @brief
 * Retrieve Oracle Error code from error handle
 *
 * @param err - Error handle
 *
 */

OCI_EXPORT int OCI_API OCI_ErrorGetOCICode
(
    OCI_Error *err
);

/**
 * @brief
 * Retrieve Internal Error code from error handle
 *
 * @param err - Error handle
 *
 */

OCI_EXPORT int OCI_API OCI_ErrorGetInternalCode
(
    OCI_Error *err
);

/**
 * @brief
 * Retrieve connection handle within the error occurred
 *
 * @param err - Error handle
 *
 */

OCI_EXPORT OCI_Connection * OCI_API OCI_ErrorGetConnection
(
    OCI_Error *err
);

/**
 * @brief
 * Retrieve statement handle within the error occurred
 *
 * @param err - Error handle
 *
 * @note
 * If the error occurred outside of a statement context, it returns NULL
 *
 */

OCI_EXPORT OCI_Statement * OCI_API OCI_ErrorGetStatement
(
    OCI_Error *err
);

/**
 * @brief
 * Return the row index which caused an error during statement execution
 *
 * @param err - Error handle
 *
 * @warning
 * Row index start at 1.
 *
 * @return
 * 0 is the error is not related to array DML otherwise the index of the given
 * row which caused the error
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_ErrorGetRow
(
    OCI_Error *err
);

/**
 * @}
 */

/**
 * @defgroup g_connect Connecting to Database
 * @{
 *
 * Connecting to a database server is done with one call to OCI_ConnectionCreate().
 *
 * OCI_ConnectionFree() closes the established connection.
 *
 * Connection properties are accessible through a set of functions
 *
 * @par Example
 * @include conn.c
 *
 */

/**
 * @brief
 * Create a physical connection to an Oracle database server
 *
 * @param db   - Oracle Service Name
 * @param user - Oracle User name
 * @param pwd  - Oracle User password
 * @param mode - Session mode
 *
 * Possible values for parameter mode :
 * - OCI_SESSION_DEFAULT
 * - OCI_SESSION_SYSDBA
 * - OCI_SESSION_SYSOPER
 * - OCI_SESSION_XA
 *
 * @note
 * External credentials are supported by supplying a null value for the
 * 'user' and 'pwd' parameters.
 * If the param 'db' is NULL then a connection to the default local DB is done
 *
 * @note
 * For parameter 'mode', the possible values are exclusive and cannot be combined
 *
 * @par Oracle XA support
 *
 * OCILIB supports Oracle XA connectivity. In order to get a connection using
 * the XA interface :
 *  - Pass to the parameter 'db' the value of the 'DB' parameter of the given
 *    XA connection string pased to the Transaction Processing Monitor (TPM)
 *  - Pass NULL to the 'user' and 'pwd' parameters
 *  - Pass the value OCI_SESSION_XA to parameter 'mode'
 *
 * @par Oracle XA Connection String
 *
 * The XA connection string used in a transaction monitor to connect to Oracle must
 * be compatible with OCILIB :
 *
 *   - the XA parameter 'Objects' MUST be set to 'true'
 *   - If OCI_ENV_THREADED is passed to OCI_Initialize(), the XA parameter 'Threads' must
 *     be set to 'true', otherwise to 'false'
 *   - If OCI_ENV_EVENTS is passed to OCI_Initialize(), the XA parameter 'Events' must
 *     be set to 'true', otherwise to 'false'
 *   - As Oracle does not support Unicode UTF16 characterset through the XA interface,
 *     Only OCI_CHARSET_ANSI builds of OCILIB can be used
 *   - You still can use UTF8 if the NLS_LANG environment variable is set with a valid
 *     UTF8 NLS value
 *   - DO NOT USE OCI_CHARSET_MIXED or OCI_CHARSET_WIDE OCILIB builds with XA connections
 *
 * @note
 * On success, a local transaction is automatically created and started
 *
 * @return
 * Connection handle on success or NULL on failure
 *
 */

OCI_EXPORT OCI_Connection * OCI_API OCI_ConnectionCreate
(
    const mtext *db,
    const mtext *user,
    const mtext *pwd,
    unsigned int mode
);

/**
 * @brief
 * Close a physical connection to an Oracle database server
 *
 * @param con - Connection handle
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_ConnectionFree
(
    OCI_Connection *con
);

/**
 * @brief
 * Returns TRUE is the given connection is still connected otherwise FALSE
 *
 * @param con - Connection handle
 *
 */

OCI_EXPORT boolean OCI_API OCI_IsConnected
(
    OCI_Connection *con
);

/**
 * @brief
 * Return the pointer to user data previously associated with the connection
 *
 * @param con - Connection handle
 *
 */

OCI_EXPORT void * OCI_API OCI_GetUserData
(
    OCI_Connection *con
);

/**
 * @brief
 * Associate a pointer to user data to the given connection
 *
 * @param con  - Connection handle
 * @param data - User data pointer
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_SetUserData
(
    OCI_Connection *con,
    void           *data
);

/**
 * @brief
 * Associate a tag to the given connection/session
 *
 * @param con - Connection handle
 * @param tag - user tag string
 *
 * @note
 * Use this call only for connections retrieved from a session pool
 * See OCI_PoolGetConnection() for more details
 *
 * @note
 * To untag a session, call OCI_SetSessionTag() with 'tag' parameter set ot NULL
 *
 * @warning
 * No error is raised if the connection is a standalone connection or retrieved from a connection
 * pool
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_SetSessionTag
(
    OCI_Connection *con,
    const mtext    *tag
);

/**
 * @brief
 * Return the tag associated the the given connection
 *
 * @param con - Connection handle
 *
 */

OCI_EXPORT const mtext * OCI_API OCI_GetSessionTag
(
    OCI_Connection *con
);

/**
 * @brief
 * Return the name of the connected database/service name
 *
 * @param con - Connection handle
 *
 */

OCI_EXPORT const mtext * OCI_API OCI_GetDatabase
(
    OCI_Connection *con
);

/**
 * @brief
 * Return the current logged user name
 *
 * @param con - Connection handle
 *
 */

OCI_EXPORT const mtext * OCI_API OCI_GetUserName
(
    OCI_Connection *con
);

/**
 * @brief
 * Return the current logged user password
 *
 * @param con - Connection handle
 *
 */

OCI_EXPORT const mtext * OCI_API OCI_GetPassword
(
    OCI_Connection *con
);

/**
 * @brief
 * Change the password of the logged user
 *
 * @param con      - Connection handle
 * @param password - New password
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_SetPassword
(
    OCI_Connection *con,
    const mtext    *password
);

/**
 * @brief
 * Change the password of the given user on the given database
 *
 * @param db      - Oracle Service Name
 * @param user    - Oracle User name
 * @param pwd     - Oracle User password
 * @param new_pwd - Oracle User New password
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_SetUserPassword
(
    const mtext *db,
    const mtext *user,
    const mtext *pwd,
    const mtext *new_pwd
);

/**
 * @brief
 * Return the current session mode
 *
 * @param con - Connection handle
 *
 * @note
 * See OCI_ConnectionCreate() for possible values
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_GetSessionMode
(
    OCI_Connection *con
);

/**
 * @brief
 * Return the connected database server version
 *
 * @param con - Connection handle
 *
 */

OCI_EXPORT const mtext * OCI_API OCI_GetVersionServer
(
    OCI_Connection *con
);

/**
 * @brief
 * Return the major version number of the connected database server
 *
 * @param con - Connection handle
 *
 * @return
 * Version number or 0 on failure
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_GetServerMajorVersion
(
    OCI_Connection *con
);

/**
 * @brief
 * Return the minor version number of the connected database server
 *
 * @param con - Connection handle
 *
 * @return
 * Version number or 0 on failure
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_GetServerMinorVersion
(
    OCI_Connection *con
);

/**
 * @brief
 * Return the revision version number of the connected database server
 *
 * @param con - Connection handle
 *
 * @return
 * Version number or 0 on failure
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_GetServerRevisionVersion
(
    OCI_Connection *con
);

/**
 * @brief
 * Set the date format for implicit string / date conversions
 *
 * @param con    - Connection handle
 * @param format - Date format
 *
 * @note
 * Default format is 'YYYY-MM-DD' defined by the public constant OCI_STRING_FORMAT_DATE
 *
  * @note
 * Possible values are the string date format supported by Oracle.
 * See documentation of Oracle SQL to_date() function for more details
 *
 */

OCI_EXPORT boolean OCI_API OCI_SetDefaultFormatDate
(
    OCI_Connection *con,
    const mtext    *format
);

/**
 * @brief
 * Return the current date format for implicit string / date conversions
 *
 * @param con - Connection handle
 *
 * @note
 *  See OCI_SetFormatDate() for possible values
 *
 */

OCI_EXPORT const mtext * OCI_API OCI_GetDefaultFormatDate
(
    OCI_Connection *con
);

/**
 * @brief
 * Set the numeric format for implicit string / numeric conversions
 *
 * @param con    - Connection handle
 * @param format - Numeric format
 *
 * @note
 * Possible values are the string numeric format supported by Oracle.
 * See documentation of Oracle SQL to_number() function for more details
 *
 * @note
 * Default format is 'FM99999999999999999999999999999999999990.999999999999999999999999'
 * defined by the public constant OCI_STRING_FORMAT_NUM
 *
 * @warning
 * If data fetched from a string column cannot be converted to a number value
 * with the given format, an error will be raised
 *
 */

OCI_EXPORT boolean OCI_API OCI_SetDefaultFormatNumeric
(
    OCI_Connection *con,
    const mtext    *format
);

/**
 * @brief
 * Return the current numeric format for implicit string / numeric conversions
 *
 * @param con - Connection handle
 *
 * @note
 *  See OCI_SetFormatNumeric() for possible values
 *
 */

OCI_EXPORT const mtext * OCI_API OCI_GetDefaultFormatNumeric
(
    OCI_Connection *con
);

/**
 * @brief
 * Return the current transaction of the connection
 *
 * @param con - Connection handle
 *
 */

OCI_EXPORT OCI_Transaction * OCI_API OCI_GetTransaction
(
    OCI_Connection *con
);

/**
 * @brief
 * Return the current transaction attached to the connection
 *
 * @param con   - Connection handle
 * @param trans - Transaction handle to assign
 *
 * @note
 * The current transaction is automatically stopped but the newly assigned is not started or resumed
 *
 */

OCI_EXPORT boolean OCI_API OCI_SetTransaction
(
    OCI_Connection  *con,
    OCI_Transaction *trans
);

/**
 * @brief
 * Return the highest Oracle version is supported by the connection
 *
 * @param con - connection handle
 *
 * @note
 * The highest supported version is the lower version between client and server:
 *
 * @note
 * Returns one of the following values:
 *
 * - OCI_UNKNOWN
 * - OCI_8_0
 * - OCI_8_1
 * - OCI_9_0
 * - OCI_9_2
 * - OCI_10_1
 * - OCI_10_2
 * - OCI_11_1
 * - OCI_11_2
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_GetVersionConnection
(
    OCI_Connection *con
);

/**
 * @brief
 * Set tracing information to the session of the given connection
 *
 * @param con   - connection handle
 * @param trace - trace type
 * @param value - trace content
 *
 * Store current trace information to the given connection handle.
 * These information:
 *
 * - is stored in the system view V$SESSION
 * - can be retrieved from the connection property of an OCI_Error handle
 *
 * @note
 * Possible values of parameter 'trace' :
 *
 * - OCI_TRC_IDENTITY : Specifies the user defined identifier in the session.
 *                      It's recorded in the column CLIENT_IDENTIFIER of the
 *                      system view V$SESSION
 * - OCI_TRC_MODULE   : name of the current module in the client application.
 *                      It's recorded in the column MODULE of the
 *                      system view V$SESSION
 * - OCI_TRC_ACTION   : name of the current action within the current module.
 *                      It's recorded in the column ACTION of the
 *                      system view V$SESSION
 * - OCI_TRC_DETAIL   : Client application additional information.
 *                      It's recorded in the column CLIENT_INFO of the
 *                      system view V$SESSION
 *
 * @warning
 * The system view V$SESSION is updated on Oracle versions >= 10g
 *
 * @warning
 * Oracle limits the size of these traces content and thus OCILIB will truncate
 * the given values if needed :
 *
 * - OCI_TRC_IDENTITY : 64 bytes
 * - OCI_TRC_MODULE   : 48 bytes
 * - OCI_TRC_ACTION   : 32 bytes
 * - OCI_TRC_DETAIL   : 64 bytes
 *
 */

OCI_EXPORT boolean OCI_API OCI_SetTrace
(
    OCI_Connection *con,
    unsigned int    trace,
    const mtext    *value
);

/**
 * @brief
 * Get the current trace for the trace type from the given connection.
 *
 * @param con   - connection handle
 * @param trace - trace type
 *
 * @note
 * See OCI_SetTrace() for more details.
 *
 */

OCI_EXPORT const mtext * OCI_API OCI_GetTrace
(
    OCI_Connection *con,
    unsigned int    trace
);

/**
 * @brief
 * Makes a round trip call to the server to confirm that the connection and the server are active.
 *
 * @param con - Connection handle
 *
 * @note
 * Returns TRUE is the connection is still alive otherwise FALSE
 *
 * @warning
 * This call is supported from Oracle 10g.
 * For previous versions, it returns FALSE without throwing any exception.
 *
 */

OCI_EXPORT boolean OCI_API OCI_Ping
(
    OCI_Connection *con
);

/**
 * @brief
 * Return the Oracle server database name of the connected database/service name
 *
 * @param con - Connection handle
 *
 * @warning
 * This call is supported from Oracle 10gR2.
 * For previous versions, it returns NULL without throwing any exception.
 *
 */

OCI_EXPORT const mtext * OCI_API OCI_GetDBName
(
    OCI_Connection *con
);

/**
 * @brief
 * Return the Oracle server Instance name of the connected database/service name
 *
 * @param con - Connection handle
 *
 * @warning
 * This call is supported from Oracle 10gR2.
 * For previous versions, it returns NULL without throwing any exception.
 *
 */

OCI_EXPORT const mtext * OCI_API OCI_GetInstanceName
(
    OCI_Connection *con
);


/**
 * @brief
 * Return the Oracle server service name of the connected database/service name
 *
 * @param con - Connection handle
 *
 * @warning
 * This call is supported from Oracle 10gR2.
 * For previous versions, it returns NULL without throwing any exception.
 *
 */

OCI_EXPORT const mtext * OCI_API OCI_GetServiceName
(
    OCI_Connection *con
);


/**
 * @brief
 * Return the Oracle server machine name of the connected database/service name
 *
 * @param con - Connection handle
 *
 * @warning
 * This call is supported from Oracle 10gR2.
 * For previous versions, it returns NULL without throwing any exception.
 *
 */

OCI_EXPORT const mtext * OCI_API OCI_GetServerName
(
    OCI_Connection *con
);


/**
 * @brief
 * Return the Oracle server domain name of the connected database/service name
 *
 * @param con - Connection handle
 *
 * @warning
 * This call is supported from Oracle 10gR2.
 * For previous versions, it returns NULL without throwing any exception.
 *
 */

OCI_EXPORT const mtext * OCI_API OCI_GetDomainName
(
    OCI_Connection *con
);


/**
 * @brief
 * Return the date and time (Timestamp) server instance start of the
 * connected database/service name
 *
 * @param con - Connection handle
 *
 * @warning
 * This call is supported from Oracle 10gR2.
 * For previous versions, it returns NULL without throwing any exception.
 *
 */

OCI_EXPORT OCI_Timestamp * OCI_API OCI_GetInstanceStartTime
(
    OCI_Connection *con
);

/**
 * @brief
 * Verifiy if the given connection support TAF events
 *
 * @param con - Connection handle
 *
 * @note
 * Returns TRUE is the connection supports TAF event otherwise FALSE
 *
 * @warning
 * This call is supported from Oracle 10gR2.
 * For previous versions, it returns FALSE without throwing any exception.
 *
 */

OCI_EXPORT boolean OCI_API OCI_IsTAFCapable
(
    OCI_Connection *con
);

/**
 * @brief
 * Set the Transparent Application Failover (TAF) user handler
 *
 * @param con     - Connection handle
 * @param handler - Pointer to TAF handler procedure
 *
 * @note
 * See POCI_TAF_HANDLER documentation for more details
 *
* @warning
 * This call is supported from Oracle 10gR2.
 * For previous versions, it returns FALSE without throwing any exception.
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_SetTAFHandler
(
    OCI_Connection   *con,
    POCI_TAF_HANDLER  handler
);

/**
 * @brief
 * Return the maximum number of statements to keep in the statement cache
 *
 * @param con  - Connection handle
 *
 * @note
 * Default value is 20 (value from Oracle Documentation)
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_GetStatementCacheSize
(
    OCI_Connection  *con
);

/**
 * @brief
 * Set the maximum number of statements to keep in the statement cache
 *
 * @param con   - Connection handle
 * @param value - maximun number of statements in the cache
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_SetStatementCacheSize
(
    OCI_Connection  *con,
    unsigned int     value
);

/**
 * @}
 */

/**
 * @defgroup g_pool Oracle Pools
 * @{
 *
 * OCILIB support the connections and sessions pooling featurse introduced
 * in Oracle 9i.
 *
 * Let's Oracle talk about this features !
 *
 * @par Connection pools (from Oracle Oracle Call Interface Programmer's Guide)
 *
 * Connection pooling is the use of a group (the pool) of reusable physical connections
 * by several sessions, in order to balance loads. The management of the pool is done
 * by OCI, not the application. Applications that can use connection pooling include
 * middle-tier applications for Web application servers and e-mail servers.
 *
 * @par Session Pools (from Oracle Oracl® Call Interface Programmer's Guide)
 *
 * Session pooling means that the application will create and maintain a group of stateless
 * sessions to the database. These sessions will be handed over to thin clients as requested.
 * If no sessions are available, a new one may be created. When the client is done with
 * the session, the client will release it to the pool. Thus, the number of sessions in
 * the pool can increase dynamically.
 *
 * @note
 * OCILIB implements homogeneous session pools only.
 *
 * @par When using Pools (from Oracle Oracle Call Interface Programmer's Guide)
 *
 * If database sessions are not reusable by mid-tier threads (that is, they are stateful)
 * and the number of back-end server processes may cause scaling problems on the database,
 * use OCI connection pooling.
 *
 * If database sessions are reusable by mid-tier threads (that is, they are stateless)
 * and the number of back-end server processes may cause scaling problems on the database,
 * use OCI session pooling.
 *
 * If database sessions are not reusable by mid-tier threads (that is, they are stateful)
 * and the number of back-end server processes will never be large enough to potentially
 * cause any scaling issue on the database, there is no need to use any pooling mechanism.
 *
 * @par Oracle 8i support
 *
 * Pooling has bee introduced in  :
 * - 9iR1 for connection pools
 * - 9iR2 for session pools
 * For Oracle 8i, OCILIB implements its own pooling mechanism in order to remain compatible
 * with older versions. But sessions pools then are handled as connection pools
 *
 * @par Example
 * @include pool.c
 *
 */

/**
 * @brief
 * Create an Oracle pool of connections or sessions
 *
 * @param db       - Oracle Service Name
 * @param user     - Oracle User name
 * @param pwd      - Oracle User password
 * @param type     - Type of pool
 * @param mode     - Session mode
 * @param min_con  - minimum number of  connections/sessions that can be opened.
 * @param max_con  - maximum number of  connections/sessions that can be opened.
 * @param incr_con - next increment for connections/sessions to be opened
 *
 * Possible values for parameter 'type':
 * - OCI_POOL_CONNECTION
 * - OCI_POOL_SESSION
 *
 * Possible values for parameter 'mode':
 * - OCI_SESSION_DEFAULT
 * - OCI_SESSION_SYSDAB
 * - OCI_SESSION_SYSOPER
 *
 * @note
 * External credentials are supported by supplying a null value for the 'user'
 * and 'pwd' parameters
 * If the param 'db' is NULL then a connection to the default local DB is done
 *
 * @note
 *
 * @return
 * Connection or session pool handle on success or NULL on failure
 *
 */

OCI_EXPORT OCI_Pool * OCI_API OCI_PoolCreate
(
    const mtext *db,
    const mtext *user,
    const mtext *pwd,
    unsigned int type,
    unsigned int mode,
    unsigned int min_con,
    unsigned int max_con,
    unsigned int incr_con
);

/**
 * @brief
 * Destroy a pool object
 *
 * @param pool - Pool handle
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_PoolFree
(
    OCI_Pool *pool
);

/**
 * @brief
 * Get a connection from the pool
 *
 * @param pool - Pool handle
 * @param tag  - user tag string
 *
 * @par Session tagging
 *
 * Session pools have a nice feature that is 'session tagging'
 * It's possible to tag a session with a string identifier
 * when the session is returned to the pool, it keeps its tags.
 * When requesting a connection from the session pool, it's
 * possible to request a session that has the given 'tag' parameter
 * If one exists, it is returned. If not and if an untagged session
 * is available, it is then returned. So check the connection tag
 * property with OCI_GetSessionTag() to find out if the returned
 * connection is tagged or not.
 *
 * This features is described in the OCI developper guide as the following :
 *
 *  "The tags provide a way for users to customize sessions in the pool.
 *   A client may get a default or untagged session from a pool, set certain
 *   attributes on the session (such as NLS settings), and return the session
 *   to the pool, labeling it with an appropriate tag.
 *   The user may request a session with the same tags in order to have a
 *   session with the same attributes"
 *
 * @return
 * Connection handle otherwise NULL on failure
 */

OCI_EXPORT OCI_Connection * OCI_API OCI_PoolGetConnection
(
    OCI_Pool *pool,
    mtext    *tag
);

/**
 * @brief
 * Get the idle timeout for connections/sessions in the pool
 *
 * @param pool - Pool handle
 *
 * @note
 * Connections/sessions idle for more than this time value (in seconds) is terminated
 *
 * @note
 * Timeout is not available for internal pooling implementation (client < 9i)
 *
 * @return
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_PoolGetTimeout
(
    OCI_Pool *pool
);

/**
 * @brief
 * Set the connections/sessions idle timeout
 *
 * @param pool  - Pool handle
 * @param value - Timeout value
 *
 * @note
 * connections/sessions idle for more than this time value (in seconds) is terminated
 *
 * @note
 * This call has no effect if pooling is internally implemented (client < 9i)
 *
 * @return
 *
 */

OCI_EXPORT boolean OCI_API OCI_PoolSetTimeout
(
    OCI_Pool    *pool,
    unsigned int value
);

/**
 * @brief
 * Get the waiting mode used when no more connections/sessions are available
 * from the pool
 *
 * @param pool - Pool handle
 *
 * @return
 * - FALSE to wait for an available object if the pool is saturated
 * - TRUE to not wait for an available object
 *
 */

OCI_EXPORT boolean OCI_API OCI_PoolGetNoWait
(
    OCI_Pool *pool
);

/**
 * @brief
 * Set the waiting mode used when no more connections/sessions are available
 * from the pool
 *
 * @param pool  - Pool handle
 * @param value - wait for object
 *
 * @note
 * Pass :
 * - FALSE to wait for an available object if the pool is saturated
 * - TRUE to not wait for an available object
 *
 * @return
 *
 */

OCI_EXPORT boolean OCI_API OCI_PoolSetNoWait
(
    OCI_Pool *pool,
    boolean   value
);

/**
 * @brief
 * Return the current number of busy connections/sessions
 *
 * @param pool - Pool handle
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_PoolGetBusyCount
(
    OCI_Pool *pool
);

/**
 * @brief
 * Return the current number of opened connections/sessions
 *
 * @param pool - Pool handle
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_PoolGetOpenedCount
(
    OCI_Pool *pool
);

/**
 * @brief
 * Return the minimum number of connections/sessions that can be opened to the database
 *
 * @param pool - Pool handle
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_PoolGetMin
(
    OCI_Pool *pool
);

/**
 * @brief
 * Return the maximum number of connections/sessions that can be opened to the database
 *
 * @param pool - Pool handle
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_PoolGetMax
(
    OCI_Pool *pool
);

/**
 * @brief
 * Return the increment for connections/sessions to be opened to the database when the pool is
 * not full
 *
 * @param pool - Pool handle
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_PoolGetIncrement
(
    OCI_Pool *pool
);

/**
 * @brief
 * Return the maximum number of statements to keep in the pool statement cache
 *
 * @param pool - Pool handle
 *
 * @note
 * Default value is 20 (value from Oracle Documentation)
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_PoolGetStatementCacheSize
(
    OCI_Pool *pool
);

/**
 * @brief
 * Set the maximum number of statements to keep in the pool statement cache
 *
 * @param pool  - Pool handle
 * @param value - maximun number of statements in the cache
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_PoolSetStatementCacheSize
(
    OCI_Pool     *pool,
    unsigned int  value
);

/**
 * @}
 */

/**
 * @defgroup g_transac Managing transactions
 * @{
 *
 * OCILIB supports local and global transactions.
 *
 * Local transactions are implicit within connection objects and there is no
 * specific call or programming step for using it.
 *
 * In order to control changes made in the database:
 *
 * - OCI_Commit() validates current pending modifications
 * - OCI_Rollback() discards current pending modifications
 *
 * OCILIB supports a feature called 'Auto Commit' that performs an implicit and
 * automatic commit call after every execute call
 *
 * @note
 * Those actions are executed within a connection context and not directly to a transaction.
 *
 * @warning
 * Global transactions are optional and are designed for distributed or global
 * transaction environments.
 *
 * OCILIB supports them by :
 *
 * - Creating/Destroying explicitly a transaction object
 * - Starting/Stopping/Resuming explicitly the transaction
 * - Preparing the transaction for specific calls
 *
 * @note
 * OCILIB does not yet suppport  XA environments
 *
 */

/**
 * @brief
 * Commit current pending changes
 *
 * @param con - Connection handle
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_Commit
(
    OCI_Connection *con
);

/**
 * @brief
 * Cancel current pending changes
 *
 * @param con - Connection handle
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_Rollback
(
    OCI_Connection *con
);

/**
 * @brief
 * Enable / disable auto commit mode
 *
 * The auto commit mode allows commit changes after every executed SQL order
 *
 * @param con    - Connection handle
 * @param enable - Enable (TRUE) or disable (FALSE)
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_SetAutoCommit
(
    OCI_Connection *con,
    boolean         enable
);

/**
 * @brief
 * Get current auto commit mode status
 *
 * @param con - Connection handle
 *
 * @return
 * TRUE if auto commit mode is activated otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_GetAutoCommit
(
    OCI_Connection *con
);

/**
 * @brief
 * Create a new global transaction or a serializable/read-only local transaction
 *
 * @param con     - Connection handle
 * @param timeout - Time that a transaction stays inactive after being stopped
 * @param mode    - Connection mode
 * @param pxid    - pointer to a global transaction identifier structure
 *
 *
 * @note
 * The parameter 'mode' can be one of the following values :
 *
 * - Global transactions:
 *      - OCI_TRS_NEW : By default starts a new, tightly coupled and
 *                      migratable branch.
 *      - OCI_TRS_TIGHT : explicitly specifies a tightly coupled branch
 *      - OCI_TRS_LOOSE : specifies a loosely coupled branch
 *
 * - Global and local transactions :
 *      - OCI_TRS_READONLY - start a read-only transaction
 *      - OCI_TRS_READWRITE - start a read-write transaction
 *      - OCI_TRS_SERIALIZABLE : start a serializable transaction
 *
 * @note
 * For local transaction:
 * - pass a NULL value for pxid
 *
 */

OCI_EXPORT OCI_Transaction * OCI_API OCI_TransactionCreate
(
    OCI_Connection *con,
    unsigned int    timeout,
    unsigned int    mode,
    OCI_XID        *pxid
);

/**
 * @brief
 * Free current transaction
 *
 * @param trans - Connection handle
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_TransactionFree
(
    OCI_Transaction *trans
);

/**
 * @brief
 * Start global transaction
 *
 * @param trans - Connection handle
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_TransactionStart
(
    OCI_Transaction *trans
);

/**
 * @brief
 * Stop current global transaction
 *
 * @param trans - Connection handle
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_TransactionStop
(
    OCI_Transaction *trans
);

/**
 * @brief
 * Resume a stopped global transaction
 *
 * @param trans - Global transaction handle
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_EXPORT boolean OCI_API OCI_TransactionResume
(
    OCI_Transaction *trans
);

/**
 * @brief
 * Prepare a global transaction validation
 *
 * @param trans - Global transaction handle
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_TransactionPrepare
(
    OCI_Transaction *trans
);

/**
 * @brief
 * Cancel the prepared global transaction validation
 *
 * @param trans - Global transaction handle
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_TransactionForget
(
    OCI_Transaction *trans
);

/**
 * @brief
 * Return global transaction mode.
 *
 * @note:
 * see OCI_TransactionCreate() for possible values
 *
 * @param trans - Global transaction handle
 *
 * @return
 * Transaction mode or OCI_UNKNOW if trans is NULL
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_TransactionGetMode
(
    OCI_Transaction *trans
);

/**
 * @brief
 * Return global transaction Timeout
 *
 * @param trans - Global transaction handle
 *
 * @return
 * Transaction timeout or 0 if trans is NULL
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_TransactionGetTimeout
(
    OCI_Transaction *trans
);

/**
 * @}
 */

/**
 * @defgroup g_exec Executing statements
 * @{
 *
 * Executing SQL statements or PL/SQL blocks is really simple with OCILIB.
 *
 * First, call OCI_StatementCreate() to allocate a statement handle. Then :
 *
 * - Prepare the SQL with OCI_Prepare()
 * - Parse and execute it with OCI_Execute()
 *
 * These two steps can be done together by calling OCI_ExecuteStmt() that
 * prepares and executes in one go.
 *
 * To find out if the statement has affected any rows, call OCI_GetAffectedRows()
 *
 * Finally, release the statement and its resources with OCI_StatementFree()
 *
 * @note
 * A statement can be prepared once and executed as many times as needed (see
 * Binding variables section)
 *
 * @note
 * An OCI_Statement can be used to prepare and/or execute different SQL and PL/SQL
 * statements as many times as needed.
 * For example, if the SQL processing of an application is sequential, only
 * one statement handle is required
 *
 * @note
 * OCILIB supports nested levels of SQL statement processing.
 * An application can loop through the resultset of the statement handle A,
 * executing statement B and fetching statement C at every loop, and so on ...
 *
 * @par Example
 * @include exec.c
 *
 */

/**
 * @brief
 * Create a statement object and return its handle
 *
 * @param con - Connection handle
 *
 * @return
 * A statement handle on success otherwise NULL
 *
 */

OCI_EXPORT OCI_Statement * OCI_API OCI_StatementCreate
(
    OCI_Connection *con
);

/**
 * @brief
 * Free a statement and all resources associated to it (resultsets ...)
 *
 * @param stmt - Connection handle
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_StatementFree
(
    OCI_Statement *stmt
);

/**
 * @brief
 * Prepare a SQL statement or PL/SQL block.
 *
 * @param stmt - Statement handle
 * @param sql  - SQL order or PL/SQL block
 *
 * @note
 * With version 1.3.0 and above, do not call this function for fetched statements (REF cursors)
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_EXPORT boolean OCI_API OCI_Prepare
(
    OCI_Statement *stmt,
    const mtext   *sql
);

/**
 * @brief
 * Execute a prepared SQL statement or PL/SQL block.
 *
 * @param stmt - Statement handle
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_EXPORT boolean OCI_API OCI_Execute
(
    OCI_Statement *stmt
);

/**
 * @brief
 * Execute a SQL statement or PL/SQL block.
 *
 * @param stmt - Statement handle
 * @param sql  - SQL order - PL/SQL block
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_EXPORT boolean OCI_API OCI_ExecuteStmt
(
    OCI_Statement *stmt,
    const mtext   *sql
);

/**
 * @brief
 * Parse a SQL statement or PL/SQL block.
 *
 * @param stmt - Statement handle
 * @param sql  - SQL order - PL/SQL block
 *
 * @note
 * This call sends the SQL or PL/SQL command to the server for parsing only.
 * The command is not executed.
 * This call is only useful to check is a command is valid or not.
 *
 * @note
 * This call prepares the statement (internal call to OCI_Prepare()) and ask
 * the Oracle server to parse its SQL or PL/SQL command.
 * OCI_Execute() can be call after OCI_Parse() in order to execute the
 * statement, which means that the server will reparse again the command.
 *
 * @warning
 * Do not use OCI_Parse() unless you're only interested in the parsing result
 * because the statement will be parsed again when executed and thus leading to
 * unnecessary server roundtrips and less performance
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_EXPORT boolean OCI_API OCI_Parse
(
    OCI_Statement *stmt,
    const mtext   *sql
);

/**
 * @brief
 * Describe the select list of a SQL select statement.
 *
 * @param stmt - Statement handle
 * @param sql  - SELECT sql statement
 *
 * @note
 * This call sends the SELECT SQL order to the server for retrieving the
 * description of the select order only.
 * The command is not executed.
 * This call is only useful to retreive information on the associated resultet
 * Call OCI_GetResultet() after OCI_Describe() to access to SELECT list
 * information
 *
 * @note
 * This call prepares the statement (internal call to OCI_Prepare()) and ask
 * the Oracle server to describe the output SELECT list.
 * OCI_Execute() can be call after OCI_Desbribe() in order to execute the
 * statement, which means that the server will parse, and describe again the SQL
 * order.
 *
 * @warning
 * Do not use OCI_Desbribe() unless you're only interested in the resultset
 * information because the statement will be parsed again when executed and thus
 * leading to unnecessary server roundtrips and less performance
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_EXPORT boolean OCI_API OCI_Describe
(
    OCI_Statement *stmt,
    const mtext   *sql
);

/**
 * @brief
 * Return the last SQL or PL/SQL statement prepared or executed by the statement
 *
 * @param stmt - Statement handle
 *
 */

OCI_EXPORT const mtext * OCI_API OCI_GetSql
(
    OCI_Statement *stmt
);

/**
 * @brief
 * Return the error position (in terms of characters) in the SQL statement
 * where the error occurred in case of SQL parsing error
 *
 * @param stmt - Statement handle
 *
 * @note
 * Positions start at 1.
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_GetSqlErrorPos
(
    OCI_Statement *stmt
);

/**
 * @brief
 * Return the number of rows affected by the SQL statement
 *
 * @param stmt - Statement handle
 *
 * The returned value is :
 *  - For UPDATEs : number of rows updated
 *  - For INSERTs : number of rows inserted
 *  - For DELETEs : number of rows deleted
 *
 * @note
 * For SELECTs  statements, use OCI_GetRowCount() instead
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_GetAffectedRows
(
    OCI_Statement *stmt
);

/**
 * @brief
 * Return the Oracle SQL code the command held by the statement handle
 *
 * @param stmt - Statement handle
 *
 * @warning
 * OCI_GetSQLCommand() must be called after the statement has be executed
 * because that's the server engine that computes the SQL command code
 *
 * @return
 * The SQL command code of the statement otherwise OCI_UNKOWN
 */

OCI_EXPORT unsigned int OCI_API OCI_GetSQLCommand
(
    OCI_Statement *stmt
);

/**
 * @brief
 * Return the verb of the SQL command held by the statement handle
 *
 * @param stmt - Statement handle
 *
 * @warning
 * OCI_GetSQLVerb() must be called after the statement has be executed
 * because that's the server engine that computes the SQL command code
 *
 * @note
 * The SQL command verb list is available in Oracle documentations and guides
 *
 * @return
 * The SQL command verb of the statement otherwise NULL
 */

OCI_EXPORT const mtext * OCI_API OCI_GetSQLVerb
(
    OCI_Statement *stmt
);

/**
 * @}
 */

/**
 * @defgroup g_bind Binding variables and arrays
 * @{
 *
 * OCILIB supports OCI data binding APIs
 *
 * Programs variables can be binded to an Oracle SQL PL/SQL statement in order to :
 *
 * - Provide input data for SQL statement
 * - Provide input/output data for PL/SQL blocks
 *
 * OCILIB provides a set of binding functions to use with:
 *
 * - Basic datatypes: string (char/wchar_t *), int, double, raw
 * - Object datatypes: lobs, files,longs, dates, cursors, statements,
 *                      timestamps, intervals, objects
 *
 * To use binding:
 *
 * - Prepare a statement with OCI_Prepare() (see Executing statements)
 * - Bind variables by calling one if the OCI_Bindxxxxx() function for every
 *   input variable referenced by the SQL statement
 * - Setup up values of the program variables
 * - Call OCI_Execute() as many times as needed
 * - Each OCI_Execute() call may be preceded by an update of the program
 *   variables (for INSERTs for example)
 *
 * OCILIB supports the OCI array Interface by binding arrays of C scalar types
 * and OCILIB object types.
 *
 * - all types supported the library can be used for array binding except
 *   OCI_Statement and OCI_Long
 * - Array binding is really fast for massive DML operations
 * - For string/RAW arrays, the input array MUST BE a contiguous block of data
 *   and not an array of pointers. So to bind an array of 10 elements for a
 *   varchar2(30) column, binded variable must be a like array[10][31]
 *
 * OCILIB does not pre-parse statements (like other frameworks such as JDBC, ...)
 * and lets Oracle recognize input variables embedded within the SQL statements.
 *
 * Bind variables must be preceded in the SQL code by the character ':'.
 *
 * Oracle and OCILIB supports two ways of binding:
 *
 * - by name (default mode in OCILIB): Oracle looks for variables in the SQL
 *   statement by searching their names provided to the binding function.
 *   So a variable can be binded once and used many times in the statement
 * - by position: Oracle binds variables by position, so every variable is
 *   binded with a position number
 *
 * OCILIB Default binding mode is OCI_BIND_BY_NAME.
 *
 * When using binding by position, provide the position to OCI_BindXXXX() call
 * through the name parameter. Within this mode the bind name must be the
 * position preceded by a semicolon like ':1', ':2', ....
 *
 * @par Internal Bind allocation mode
 *
 * From version 3.7.0, bind variables or arrays can be internally allocated by
 * OCILIB. That means that instead of allocating variables or arrays on the stack/heap
 * in the user program, bind contents can be allocated internally and thus :
 * - minimize the amount of program variables
 * - optimize internal memory management for arrays
 *
 * To do so :
 * - Call OCI_SetBindAllocation() with the mode OCI_BAM_INTERNAL
 * - pass a NULL variable or array to OCI_BindXXX() calls
 * - Retrieve the bind content allotated by OCILIB with OCI_BindGetData()
 *
 * @note
 * Rebinding is disabled by default (see OCI_AllowRebinding())
 * When using rebinding feature, host variable rebinded to a previously allocated
 * bind MUST be of the same datatype !
 *
 * @par Basic input bind Example
 * @include bind.c
 *
 * @par Array interface Example
 * @include array.c
 *
 * @par Internal Array interface Example
 * @include array_internal.c
 *
 * */

/**
 * @brief
 * Set the input array size for bulk operations
 *
 * @param stmt - Statement handle
 * @param size - Array size
 *
 * @warning
 * Do not use OCI_BindArraySetSize() for PL/SQL tables binding
 *
 * @note
 * OCI_BindArraySetSize() is used to set the size of input bind array when using
 * arrays for DML statements.
 * OCI_BindArraySetSize() MUST be called to set the maximum size of the arrays
 * to bind to the statement before any of its execution. This initial call must
 * be bone AFTER OCI_Prepare() and BEFORE any OCI_BindArrayOfxxx() call.
 *
 * @note
 * OCI_BindArraySetSize can optionally be called before any later OCI_Execute()
 * call in order to notify the statement of the exact number of elements
 * populating the input arrays for the next execution. The array size passed to
 * later OCI_BindArraySetSize() calls cannot be greater than the initial size
 * otherwise an exception will be thrown.
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_EXPORT boolean OCI_API OCI_BindArraySetSize
(
    OCI_Statement *stmt,
    unsigned int   size
);

/**
 * @brief
 * Return the current input array size for bulk operations
 *
 * @param stmt - Statement handle
 *
 * @return
 * Array size value or 0 if OCI_BindArraySetSize() has not been called
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_BindArrayGetSize
(
    OCI_Statement *stmt
);

/**
 * @brief
 * Allow different host variables to be binded using the same bind name or
 * position between executions of a prepared statement
 *
 * @param stmt  - Statement handle
 * @param value - Rebinding mode allowed
 *
 * @note
 * Default value is FALSE
 *
 * @warning
 * When using rebinding feature, host variable rebinded to a previously allocated
 * bind MUST be of the same datatype !
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_EXPORT boolean OCI_API OCI_AllowRebinding
(
    OCI_Statement *stmt,
    boolean        value
);

/**
 * @brief
 * Bind an short variable
 *
 * @param stmt - Statement handle
 * @param name - Variable name
 * @param data - Pointer to short variable
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_EXPORT boolean OCI_API OCI_BindShort
(
    OCI_Statement *stmt,
    const mtext   *name,
    short         *data
);

/**
 * @brief
 * Bind an array of shorts
 *
 * @param stmt   - Statement handle
 * @param name   - Variable name
 * @param data   - Array of shorts
 * @param nbelem - Number of element in the array (PL/SQL table only)
 *
 * @warning
 * Parameter 'nbelem' SHOULD ONLY be USED for PL/SQL tables.
 * For regular DML array operations, pass the value 0.
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_EXPORT boolean OCI_API OCI_BindArrayOfShorts
(
    OCI_Statement *stmt,
    const mtext   *name,
    short         *data,
    unsigned int   nbelem
);

/**
 * @brief
 * Bind an unsigned short variable
 *
 * @param stmt - Statement handle
 * @param name - Variable name
 * @param data - Pointer to unsigned short variable
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_EXPORT boolean OCI_API OCI_BindUnsignedShort
(
    OCI_Statement  *stmt,
    const mtext    *name,
    unsigned short *data
);

/**
 * @brief
 * Bind an array of unsigned shorts
 *
 * @param stmt   - Statement handle
 * @param name   - Variable name
 * @param data   - Array of unsigned shorts
 * @param nbelem - Number of element in the array (PL/SQL table only)
 *
 * @warning
 * Parameter 'nbelem' SHOULD ONLY be USED for PL/SQL tables.
 * For regular DML array operations, pass the value 0.
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_EXPORT boolean OCI_API OCI_BindArrayOfUnsignedShorts
(
    OCI_Statement  *stmt,
    const mtext    *name,
    unsigned short *data,
    unsigned int    nbelem
);

/**
 * @brief
 * Bind an integer variable
 *
 * @param stmt - Statement handle
 * @param name - Variable name
 * @param data - Pointer to int variable
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_EXPORT boolean OCI_API OCI_BindInt
(
    OCI_Statement *stmt,
    const mtext   *name,
    int           *data
);

/**
 * @brief
 * Bind an array of integers
 *
 * @param stmt   - Statement handle
 * @param name   - Variable name
 * @param data   - Array of int
 * @param nbelem - Number of element in the array (PL/SQL table only)
 *
 * @warning
 * Parameter 'nbelem' SHOULD ONLY be USED for PL/SQL tables.
 * For regular DML array operations, pass the value 0.
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_EXPORT boolean OCI_API OCI_BindArrayOfInts
(
    OCI_Statement *stmt,
    const mtext   *name,
    int           *data,
    unsigned int   nbelem
);

/**
 * @brief
 * Bind an unsigned integer variable
 *
 * @param stmt - Statement handle
 * @param name - Variable name
 * @param data - Pointer to unsigned int variable
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_EXPORT boolean OCI_API OCI_BindUnsignedInt
(
    OCI_Statement *stmt,
    const mtext   *name,
    unsigned int  *data
);

/**
 * @brief
 * Bind an array of unsigned integers
 *
 * @param stmt   - Statement handle
 * @param name   - Variable name
 * @param data   - Array of unsigned int
 * @param nbelem - Number of element in the array (PL/SQL table only)
 *
 * @warning
 * Parameter 'nbelem' SHOULD ONLY be USED for PL/SQL tables.
 * For regular DML array operations, pass the value 0.
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_EXPORT boolean OCI_API OCI_BindArrayOfUnsignedInts
(
    OCI_Statement *stmt,
    const mtext   *name,
    unsigned int  *data,
    unsigned int   nbelem
);

/**
 * @brief
 * Bind a big integer variable
 *
 * @param stmt - Statement handle
 * @param name - Variable name
 * @param data - Pointer to big int variable
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_EXPORT boolean OCI_API OCI_BindBigInt
(
    OCI_Statement *stmt,
    const mtext   *name,
    big_int       *data
);

/**
 * @brief
 * Bind an array of big integers
 *
 * @param stmt   - Statement handle
 * @param name   - Variable name
 * @param data   - Array of big int
 * @param nbelem - Number of element in the array (PL/SQL table only)
 *
 * @warning
 * Parameter 'nbelem' SHOULD ONLY be USED for PL/SQL tables.
 * For regular DML array operations, pass the value 0.
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_EXPORT boolean OCI_API OCI_BindArrayOfBigInts
(
    OCI_Statement *stmt,
    const mtext   *name,
    big_int       *data,
    unsigned int   nbelem
);

/**
 * @brief
 * Bind an unsigned big integer variable
 *
 * @param stmt - Statement handle
 * @param name - Variable name
 * @param data - Pointer to unsigned big int variable
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_EXPORT boolean OCI_API OCI_BindUnsignedBigInt
(
    OCI_Statement *stmt,
    const mtext   *name,
    big_uint      *data
);

/**
 * @brief
 * Bind an array of unsigned big integers
 *
 * @param stmt - Statement handle
 * @param name - Variable name
 * @param data - Array of unsigned big int
 * @param nbelem - Number of element in the array (PL/SQL table only)
 *
 * @warning
 * Parameter 'nbelem' SHOULD ONLY be USED for PL/SQL tables.
 * For regular DML array operations, pass the value 0.
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_EXPORT boolean OCI_API OCI_BindArrayOfUnsignedBigInts
(
    OCI_Statement *stmt,
    const mtext   *name,
    big_uint      *data,
    unsigned int   nbelem
);

/**
 * @brief
 * Bind a string variable
 *
 * @param stmt - Statement handle
 * @param name - Variable name
 * @param data - String to bind
 * @param len  - Max length of the string (in character without
 *               the zero null terminal character)
 *
 * @note
 * if len == 0, len is set to the string size
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_EXPORT boolean OCI_API OCI_BindString
(
    OCI_Statement *stmt,
    const mtext   *name,
    dtext         *data,
    unsigned int   len
);

/**
 * @brief
 * Bind an array of strings
 *
 * @param stmt   - Statement handle
 * @param name   - Variable name
 * @param data   - Array of string
 * @param len    - Max length of a single string element (in character without
 *                 the zero null terminal character)
 * @param nbelem - Number of element in the array
 * @param nbelem - Number of element in the array (PL/SQL table only)
 *
 * @warning
 * Parameter 'nbelem' SHOULD ONLY be USED for PL/SQL tables.
 * For regular DML array operations, pass the value 0.
 *
 * @note
 * if len <= 0, it returns FALSE
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_EXPORT boolean OCI_API OCI_BindArrayOfStrings
(
    OCI_Statement *stmt,
    const mtext   *name,
    dtext         *data,
    unsigned int   len,
    unsigned int   nbelem
);

/**
 * @brief
 * Bind a raw buffer
 *
 * @param stmt - Statement handle
 * @param name - Variable name
 * @param data - buffer to bind
 * @param len  - Max length of the buffer
 *
 * @note
 * if len <= 0, it returns false
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_EXPORT boolean OCI_API OCI_BindRaw
(
    OCI_Statement *stmt,
    const mtext   *name,
    void          *data,
    unsigned int   len
);

/**
 * @brief
 * Bind an array of raw buffers
 *
 * @param stmt   - Statement handle
 * @param name   - Variable name
 * @param data   - Array of buffers
 * @param len    - Size in bytes on a single RAW array element
 * @param nbelem - Number of element in the array (PL/SQL table only)
 *
 * @warning
 * Parameter 'nbelem' SHOULD ONLY be USED for PL/SQL tables.
 * For regular DML array operations, pass the value 0.
 *
 * @note
 * The buffer must be a contiguous block of data elements
 *
 * @note
 * If len <= 0, it returns FALSE
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_EXPORT boolean OCI_API OCI_BindArrayOfRaws
(
    OCI_Statement *stmt,
    const mtext   *name,
    void          *data,
    unsigned int   len,
    unsigned int   nbelem
);

/**
 * @brief
 * Bind a double variable
 *
 * @param stmt - Statement handle
 * @param name - Variable name
 * @param data - Pointer to double variable
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_EXPORT boolean OCI_API OCI_BindDouble
(
    OCI_Statement *stmt,
    const mtext   *name,
    double        *data
);

/**
 * @brief
 * Bind an array of doubles
 *
 * @param stmt   - Statement handle
 * @param name   - Variable name
 * @param data   - Array of double
 * @param nbelem - Number of element in the array (PL/SQL table only)
 *
 * @warning
 * Parameter 'nbelem' SHOULD ONLY be USED for PL/SQL tables.
 * For regular DML array operations, pass the value 0.
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_EXPORT boolean OCI_API OCI_BindArrayOfDoubles
(
    OCI_Statement *stmt,
    const mtext   *name,
    double        *data,
    unsigned int   nbelem
);

/**
 * @brief
 * Bind a date variable
 *
 * @param stmt - Statement handle
 * @param name - Variable name
 * @param data - Date handle
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_EXPORT boolean OCI_API OCI_BindDate
(
    OCI_Statement *stmt,
    const mtext   *name,
    OCI_Date      *data
);

/**
 * @brief
 * Bind an array of dates
 *
 * @param stmt   - Statement handle
 * @param name   - Variable name
 * @param data   - Array of date handle
 * @param nbelem - Number of element in the array (PL/SQL table only)
 *
 * @warning
 * Parameter 'nbelem' SHOULD ONLY be USED for PL/SQL tables.
 * For regular DML array operations, pass the value 0.
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_EXPORT boolean OCI_API OCI_BindArrayOfDates
(
    OCI_Statement *stmt,
    const mtext   *name,
    OCI_Date     **data,
    unsigned int   nbelem
);

/**
 * @brief
 * Bind a timestamp variable
 *
 * @param stmt - Statement handle
 * @param name - Variable name
 * @param data - Timestamp handle
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_EXPORT boolean OCI_API OCI_BindTimestamp
(
    OCI_Statement *stmt,
    const mtext   *name,
    OCI_Timestamp *data
);

/**
 * @brief
 * Bind an array of timestamp handles
 *
 * @param stmt   - Statement handle
 * @param name   - Variable name
 * @param data   - Array of Timestamp handle
 * @param type   - Timestamp type
 * @param nbelem - Number of element in the array (PL/SQL table only)
 *
 * @warning
 * Parameter 'nbelem' SHOULD ONLY be USED for PL/SQL tables.
 * For regular DML array operations, pass the value 0.
 *
 * @note
 * See OCI_TimestampCreate() for possible values of parameter 'type'
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_EXPORT boolean OCI_API OCI_BindArrayOfTimestamps
(
    OCI_Statement  *stmt,
    const mtext    *name,
    OCI_Timestamp **data,
    unsigned int    type,
    unsigned int    nbelem
);

/**
 * @brief
 * Bind an interval variable
 *
 * @param stmt - Statement handle
 * @param name - Variable name
 * @param data - Interval handle
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_BindInterval
(
    OCI_Statement *stmt,
    const mtext   *name,
    OCI_Interval  *data
);

/**
 * @brief
 * Bind an array of interval handles
 *
 * @param stmt   - Statement handle
 * @param name   - Variable name
 * @param data   - Array of Interval handle
 * @param type   - Interval type
 * @param nbelem - Number of element in the array (PL/SQL table only)
 *
 * @warning
 * Parameter 'nbelem' SHOULD ONLY be USED for PL/SQL tables.
 * For regular DML array operations, pass the value 0.
 *
 * @note
 * See OCI_IntervalCreate() for possible values of parameter 'type'
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_BindArrayOfIntervals
(
    OCI_Statement *stmt,
    const mtext   *name,
    OCI_Interval **data,
    unsigned int   type,
    unsigned int   nbelem
);

/**
 * @brief
 * Bind a Lob variable
 *
 * @param stmt - Statement handle
 * @param name - Variable name
 * @param data - Lob handle
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_EXPORT boolean OCI_API OCI_BindLob
(
    OCI_Statement *stmt,
    const mtext   *name,
    OCI_Lob       *data
);

/**
 * @brief
 * Bind an array of Lob handles
 *
 * @param stmt   - Statement handle
 * @param name   - Variable name
 * @param data   - Array of Lob handle
 * @param type   - Lob type
 * @param nbelem - Number of element in the array (PL/SQL table only)
 *
 * @warning
 * Parameter 'nbelem' SHOULD ONLY be USED for PL/SQL tables.
 * For regular DML array operations, pass the value 0.
 *
 * @note
 * See OCI_LobCreate() for possible values of parameter 'type'
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_EXPORT boolean OCI_API OCI_BindArrayOfLobs
(
    OCI_Statement *stmt,
    const mtext   *name,
    OCI_Lob      **data,
    unsigned int   type,
    unsigned int   nbelem
);

/**
 * @brief
 * Bind a File variable
 *
 * @param stmt - Statement handle
 * @param name - Variable name
 * @param data - File handle
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_EXPORT boolean OCI_API OCI_BindFile
(
    OCI_Statement *stmt,
    const mtext   *name,
    OCI_File      *data
);

/**
 * @brief
 * Bind an array of File handles
 *
 * @param stmt   - Statement handle
 * @param name   - Variable name
 * @param data   - Array of File handle
 * @param type   - File type
 * @param nbelem - Number of element in the array (PL/SQL table only)
 *
 * @warning
 * Parameter 'nbelem' SHOULD ONLY be USED for PL/SQL tables.
 * For regular DML array operations, pass the value 0.
 *
 * @note
 * See OCI_FileCreate() for possible values of parameter 'type'
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_EXPORT boolean OCI_API OCI_BindArrayOfFiles
(
    OCI_Statement *stmt,
    const mtext   *name,
    OCI_File     **data,
    unsigned int   type,
    unsigned int   nbelem
);

/**
 * @brief
 * Bind an object (named type) variable
 *
 * @param stmt - Statement handle
 * @param name - Variable name
 * @param data - Object handle
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_BindObject
(
    OCI_Statement *stmt,
    const mtext   *name,
    OCI_Object    *data
);

/**
 * @brief
 * Bind an array of object handles
 *
 * @param stmt   - Statement handle
 * @param name   - Variable name
 * @param data   - Array of object handle
 * @param typinf - type info handle
 * @param nbelem - Number of element in the array (PL/SQL table only)
 *
 * @warning
 * Parameter 'nbelem' SHOULD ONLY be USED for PL/SQL tables.
 * For regular DML array operations, pass the value 0.
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_BindArrayOfObjects
(
    OCI_Statement *stmt,
    const mtext   *name,
    OCI_Object   **data,
    OCI_TypeInfo  *typinf,
    unsigned int   nbelem
);

/**
 * @brief
 * Bind a Collection variable
 *
 * @param stmt - Statement handle
 * @param name - Variable name
 * @param data - Collection handle to bind
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_EXPORT boolean OCI_API OCI_BindColl
(
    OCI_Statement *stmt,
    const mtext   *name,
    OCI_Coll      *data
);

/**
 * @brief
 * Bind an array of Collection handles
 *
 * @param stmt   - Statement handle
 * @param name   - Variable name
 * @param data   - Array of Collection handle
 * @param typinf - Type info handle
 * @param nbelem - Number of element in the array (PL/SQL table only)
 *
 * @warning
 * Parameter 'nbelem' SHOULD ONLY be USED for PL/SQL tables.
 * For regular DML array operations, pass the value 0.
 *
 * @note
 * See OCI_CollCreate() for possible values of parameter 'type'
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_BindArrayOfColls
(
    OCI_Statement *stmt,
    const mtext   *name,
    OCI_Coll     **data,
    OCI_TypeInfo  *typinf,
    unsigned int   nbelem
);

/**
 * @brief
 * Bind a Ref variable
 *
 * @param stmt - Statement handle
 * @param name - Variable name
 * @param data - Ref handle to bind
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_EXPORT boolean OCI_API OCI_BindRef
(
    OCI_Statement *stmt,
    const mtext   *name,
    OCI_Ref       *data
);

/**
 * @brief
 * Bind an array of Ref handles
 *
 * @param stmt   - Statement handle
 * @param name   - Variable name
 * @param data   - Array of Ref handle
 * @param typinf - type info handle
 * @param nbelem - Number of element in the array (PL/SQL table only)
 *
 * @warning
 * Parameter 'nbelem' SHOULD ONLY be USED for PL/SQL tables.
 * For regular DML array operations, pass the value 0.
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_BindArrayOfRefs
(
    OCI_Statement *stmt,
    const mtext   *name,
    OCI_Ref      **data,
    OCI_TypeInfo  *typinf,
    unsigned int   nbelem
);

/**
 * @brief
 * Bind a Statement variable (PL/SQL Ref Cursor)
 *
 * @param stmt - Statement handle
 * @param name - Variable name
 * @param data - Statement handle to bind
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_EXPORT boolean OCI_API OCI_BindStatement
(
    OCI_Statement *stmt,
    const mtext   *name,
    OCI_Statement *data
);

/**
 * @brief
 * Bind a Long variable
 *
 * @param stmt - Statement handle
 * @param name - Variable name
 * @param data - Long handle
 * @param size - Size of the long buffer in bytes or characters
 *
 * @note
 * Size is expressed in:
 * - Bytes for BLONGs
 * - Characters for CLONGs
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_EXPORT boolean OCI_API OCI_BindLong
(
    OCI_Statement *stmt,
    const mtext   *name,
    OCI_Long      *data,
    unsigned int   size
);

/**
 * @brief
 * Returns the first or next error that occurred within a DML array statement
 *
 * @param stmt - Statement handle
 *
 * @note
 *
 * @return
 * The first or next error handle otherwise NULL
 */

OCI_EXPORT OCI_Error * OCI_API OCI_GetBatchError
(
    OCI_Statement *stmt
);

/**
 * @brief
 * Returns the number of errors that occurred within the last DML array statement
 *
 * @param stmt - Statement handle
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_GetBatchErrorCount
(
    OCI_Statement *stmt
);

/**
 * @brief
 * Return the number of binds currently associated to a statement
 *
 * @param stmt - Statement handle
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_GetBindCount
(
    OCI_Statement *stmt
);

/**
 * @brief
 * Return the bind handle at the given index in the internal array of bind
 * handle
 *
 * @param stmt  - Statement handle
 * @param index - Bind position
 *
 * @note
 * Index starts at 1.
 *
 * @note
 * Bind handle are created sequentially. By example, the third call to a
 * OCI_BindXXX() generates a bind handle of index 3.
 *
 * @return
 * The bind handle or NULL if index is out of bounds
 *
 */

OCI_EXPORT OCI_Bind * OCI_API OCI_GetBind
(
    OCI_Statement *stmt,
    unsigned int   index
);

/**
 * @brief
 * Return a bind handle from its name
 *
 * @param stmt - Statement handle
 * @param name - Bind variable name
 *
 * @note
 * Bind names must include a semicolon at the beginning.
 *
 * @return
 * The bind handle or NULL if not found
 *
 */

OCI_EXPORT OCI_Bind * OCI_API OCI_GetBind2
(
    OCI_Statement *stmt,
    const mtext   *name
);

/**
 * @brief
 * Return the name of the given bind
 *
 * @param bnd - Bind handle
 *
 */

OCI_EXPORT const mtext * OCI_API OCI_BindGetName
(
    OCI_Bind *bnd
);


/**
 * @brief
 * Set the direction mode of a bind handle
 *
 * @param bnd       - Bind handle
 * @param direction - direction mode
 *
 * @note
 * Possible values for parameter 'direction' :
 *   - OCI_BDM_IN      : input values  (not modified by the server)
 *   - OCI_BDM_OUT     : output values (modified by the server)
 *   - OCI_BDM_IN_OUT  : input and ouput values 
 *
 * @note
 * Default value is OCI_BDM_IN_OUT
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_BindSetDirection
(
    OCI_Bind    *bnd,
    unsigned int direction
);

/**
 * @brief
 * Get the direction mode of a bind handle
 *
 * @param bnd - Bind handle
 *
 * @note
 * see OCI_BindSetDirection() for more details
 *
 * return the bind direction mode on success otherwise OCI_UNKNWON
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_BindGetDirection
(
    OCI_Bind *bnd
);

/**
 * @brief
 * Return the OCILIB type of the given bind
 *
 * @param bnd - Bind handle
 *
 * @note
 * Possible values are :
 *
 * - OCI_CDT_NUMERIC     : short, int, long long, double
 * - OCI_CDT_DATETIME    : OCI_Date *
 * - OCI_CDT_TEXT        : dtext *
 * - OCI_CDT_LONG        : OCI_Long *
 * - OCI_CDT_CURSOR      : OCI_Statement *
 * - OCI_CDT_LOB         : OCI_Lob  *
 * - OCI_CDT_FILE        : OCI_File *
 * - OCI_CDT_TIMESTAMP   : OCI_Timestamp *
 * - OCI_CDT_INTERVAL    : OCI_Interval *
 * - OCI_CDT_RAW         : void *
 * - OCI_CDT_OBJECT      : OCI_Object *
 * - OCI_CDT_COLLECTION  : OCI_Coll *
 * - OCI_CDT_REF         : OCI_Ref *
 *
 * @return
 * The column type or OCI_CDT_UNKNOWN on error
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_BindGetType
(
    OCI_Bind *bnd
);

/**
 * @brief
 * Return the OCILIB object subtype of the given bind
 *
 * @param bnd - Bind handle
 *
 * @note
 *
 * This call is valid for the following OCILIB types:
 *
 * - OCI_CDT_NUMERIC
 * - OCI_CDT_LONG
 * - OCI_CDT_LOB
 * - OCI_CDT_FILE
 * - OCI_CDT_TIMESTAMP
 * - OCI_CDT_INTERVAL
 *
 * For numeric binds the possible values are:
 * - OCI_NUM_SHORT
 * - OCI_NUM_INT
 * - OCI_NUM_BIGINT
 * - OCI_NUM_USHORT
 * - OCI_NUM_UINT
 * - OCI_NUM_BIGUINT
 * - OCI_NUM_DOUBLE
 *
 * For OCI_Long type the possible values are:
 * - OCI_BLONG
 * - OCI_CLONG
 *
 * For OCI_Lob type the possible values are:
 * - OCI_BLOB
 * - OCI_CLOB
 * - OCI_NCLOB
 *
 * For OCI_File type the possible values are:
 * - OCI_BFILE
 * - OCI_CFILE
 *
 * For OCI_Timestamp type the possible values are:
 * - OCI_TIMESTAMP
 * - OCI_TIMESTAMP_TZ
 * - OCI_TIMESTAMP_LTZ
 *
 * For OCI_Interval type the possible values are:
 * - OCI_INTERVAL_YM
 * - OCI_INTERVAL_DS
 *
 * @note
 * For all other OCILIB types, it returns OCI_UNKNOWN
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_BindGetSubtype
(
    OCI_Bind *bnd
);

/**
 * @brief
 * Return the number of elements of the bind handle
 *
 * @param bnd - Bind handle
 *
 * @return
 * - For single binds, it returns 1
 * - For array binds, it returns the number of element in the array
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_BindGetDataCount
(
    OCI_Bind *bnd
);

/**
 * @brief
 * Return the user defined data associated with a bind handle
 *
 * @param bnd - Bind handle
 *
 * @return
 * - The pointer to variable/array passed to an OCI_BindXXX() or
 *   OCI_BindArrayOfXXX() call
 *
 */

OCI_EXPORT void * OCI_API OCI_BindGetData
(
    OCI_Bind *bnd
);

/**
 * @brief
 * Return the statement handle associated with a bind handle
 *
 * @param bnd - bind handle
 *
 */

OCI_EXPORT OCI_Statement * OCI_API OCI_BindGetStatement
(
    OCI_Bind *bnd
);

/**
 * @brief
 * Set the actual size of the element held by the given bind handle
 *
 * @param bnd  - bind handle
 * @param size - data size
 *
 * @note
 * This call is not mandatory and should ONLY be called for RAWs binds to set
 * the real size of the given data if different from the expected column or
 * parameter size
 *
 * @note
 * It works as well with string based PL/SQL tables (in or in/out but NOT out)
 * even if it's not necessary.
 *
 * @warning
 * For binds of type OCI_CDT_TEXT (strings), the parameter 'size' is expressed in
 * number of characters.
 *
 * @return
 * Data size if the bind type is listed above otherwise 0.
 *
 */

OCI_EXPORT boolean OCI_API OCI_BindSetDataSize
(
    OCI_Bind    *bnd,
    unsigned int size
);

/**
 * @brief
 * Set the size of the element at the given position in
 * the bind input array
 *
 * @param bnd      - bind handle
 * @param position - Position in the array
 * @param size     - data size
 *
 * @note
 * See OCI_BindSetDataSize() for supported datatypes
 *
 * @warning
 * Before execution, it returns the max default size for the bind and not the real
 * data size, unless a custom size has been set with OCI_BindSetDataSizeXXX()
 * After execution, it returns the real data size.
 *
 * @warning
 * For binds of type OCI_CDT_TEXT (strings), the parameter 'size' is expressed in
 * number of characters.
 *
 * @return
 * Data size if the bind type is listed above otherwise 0.
 *
 */

OCI_EXPORT boolean OCI_API OCI_BindSetDataSizeAtPos
(
    OCI_Bind    *bnd,
    unsigned int position,
    unsigned int size
);

/**
 * @brief
 * Return the actual size of the element held by the given bind handle
 *
 * @param bnd - bind handle
 *
 * @note
 * See OCI_BindSetDataSize() for supported datatypes
 *
 * @warning
 * For binds of type OCI_CDT_TEXT (strings), the returned value is expressed in
 * number of characters.
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_BindGetDataSize
(
    OCI_Bind *bnd
);

/**
 * @brief
 * Return the actual size of the element at the given position in
 * the bind input array
 *
 * @param bnd      - bind handle
 * @param position - Position in the array
 *
 * @note
 * See OCI_BindSetDataSize() for supported datatypes
 *
 * @warning
 * For binds of type OCI_CDT_TEXT (strings), the returned value is expressed in
 * number of characters.
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_BindGetDataSizeAtPos
(
    OCI_Bind    *bnd,
    unsigned int position
);

/**
 * @brief
 * Set the bind variable to null
 *
 * @param bnd   - Bind handle
 *
 * @note
 * There is no notion of null value in C.
 * It's necessary to explicitly tell Oracle that the bind has a null value.
 * It must be done before an OCI_Execute() call
 *
 * @note
 * For handled based datatypes (non scalar types), OCILIB performs an extra
 * check on handles and set the bind status to null is the handle is null
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_EXPORT boolean OCI_API OCI_BindSetNull
(
    OCI_Bind *bnd
);

/**
 * @brief
 * Set to null the entry in the bind variable input array
 *
 * @param bnd      - Bind handle
 * @param position - Position in the array
 *
 * @note
 * There is no notion of null value in C.
 * It's necessary to explicitly tell Oracle that the bind has a null value.
 * It must be done before an OCI_Execute() call
 *
 * @warning
 * Position starts with 1
 *
 * @note
 * For handled based datatypes (non scalar types), OCILIB performs an extra
 * check on handles and set the bind status to null is the handle is null
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_EXPORT boolean OCI_API OCI_BindSetNullAtPos
(
    OCI_Bind    *bnd,
    unsigned int position
);

/**
 * @brief
 * Check if the current value of the binded variable is marked as NULL
 *
 * @param bnd - Bind handle
 *
 * @return
 * TRUE if it's null otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_BindIsNull
(
    OCI_Bind *bnd
);

/**
 * @brief
 * Check if the current entry value at the given index of the binded array
 * is marked as NULL
 *
 * @param bnd      - Bind handle
 * @param position - Position in the array
 *
 * @warning
 * Position starts with 1
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_BindIsNullAtPos
(
    OCI_Bind    *bnd,
    unsigned int position
);

/**
 * @brief
 * Set the charset form of the given character based bind variable
 *
 * @param bnd   - Bind handle
 * @param csfrm - charset form
 *
 * @note
 * Possible values are :
 *
 * - OCI_CSF_DEFAULT : the column has default charset
 * - OCI_CSF_NATIONAL: the column has national charset
 *
 * @note
 * This call has to be made after OCI_Prepare() but before OCI_Execute()
 *
 * @warning
 * This call does nothing :
 *  - if the csform is out of range
 *  - if the bind type is not OCI_CFT_TEXT or OCI_CDT_LONG
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

boolean OCI_API OCI_BindSetCharsetForm
(
    OCI_Bind    *bnd,
    unsigned int csfrm
);

/**
 * @}
 */

/**
 * @defgroup g_fetch Fetching data
 * @{
 *
 * OCILIB offers a really easy and smart mechanism to fetch data from a SQL Statement.
 * It looks like what's found in JDBC and other object oriented databases frameworks.
 *
 * Only SELECTs statement, DML with returning clause and some PL/SQL blocks
 * return a cursor that can be fetched by host programs.
 * That cursor, or resultset, is encapsulated in OCILIB by the OCI_Resultset object.
 *
 * So, after any successful call to an OCI_Executexxx() function that executed
 * a fetchable statement or filled output bind variables, the resultset can be
 * retrieved by calling OCI_GetResultset()
 *
 * The creation of a OCI_Resultset object consists in :
 *
 * - Describing the output columns of the resultset
 * - Allocating memory to hold the content data
 *
 * OCILIB supports multi-row fetching for increasing performances. Instead of
 * fetching data row by row from the server (that induces lots of roundtrips
 * between the client and the server), the library prefetches data chunk by
 * chunks (default is 20 rows).
 * So, less network traffic and better performances.
 * These mechanisms are completely hidden from the application which fetches the
 * resultset row by row.
 *
 * Once the Resultset handle is retrieved :
 *
 * - It can be fetched by calling OCI_FetchNext() as long as it returns TRUE.
 * - To retrieve the value of a column, call OCI_GetXXXX() where XXXX is the
 *   type of data you want to fetch.
 *
 * @par Scrollable Resultsets
 *
 * Oracle 9i introduced scrollable cursors (resultsets in OCILIB) that can be
 * fetched:
 *
 * - Sequentially in both directions: OCI_FetchPrev() and OCI_FetchPrev()
 * - To a relative position in the resultset: OCI_FetchSeek() with OCI_SFD_RELATIVE
 * - To an absolute position in the resultset: OCI_FetchSeek() with OCI_SFD_ABOSLUTE
 * - To the first or last row in the resultset: OCI_FetchFirst() and OCI_FetchLast()
 *
 * Scrollable statements uses more server and client resources and should only
 * be used when necessary.
 *
 * OCILIB support scrollable cursors from version OCILIB 3.0.0.
 *
 * Resultsets are 'forward only' by default. Call OCI_SetFetchMode() with
 * OCI_SFM_SCROLLABLE to enable scrollable resultsets for a given statement.
 *
 * @warning
 * Any use of scrollable fetching functions with a resultset that depends on a
 * statement with fetch mode =  OCI_SFM_DEFAULT will fail !
 *
 * @note
 * If the column internal data does not match the requested type, OCILIB tries
 * to convert the data when it's possible and throws an error if not.
 *
 * The properties (columns names, types ...) of the resultset are accessible
 * through a set of APIs.
 *
 * @par Implicit conversion to string types
 *
 * OCI_GetString() performs an implicit conversion from  the
 * following datatypes:
 *
 * - Numerics (based on the current connection handle numeric format)
 * - OCI_Date (based on the current connection handle date format)
 * - OCI_Timestamp (based on the current connection handle date format)
 * - OCI_Interval (based on Oracle default conversion)
 * - OCI_Lob (maximum number of character is defined by OCI_SIZE_BUFFER)
 * - OCI_Long
 * - OCI_File (maximum number of character is defined by OCI_SIZE_BUFFER)
 * - RAW buffer
 *
 * The following type are not supported for implicit conversion:
 * - OCI_Statement
 * - OCI_Coll
 * - OCI_Object
 *
 * OCI_GetString() performs an implicit conversion from the following datatypes:
 *
 * @par Fetching rows into user structures
 *
 * From version 3.7.0, it is possible to fetch a complete row into a user
 * defined structure. Each column of the resultset is mapped to a structure
 * member.
 * The mapping rules are :
 *   - LOBs (CLOB, NCLOB, BLOB) : OCI_Lob *
 *   - DATE : OCI_Date *
 *   - TIMESTAMPS : OCI_Timestamp *
 *   - INTERVALS : OCI_Interval *
 *   - LONG, LONG RAW : OCI_Long *
 *   - REFs : OCI_Ref *
 *   - CURSOR, RESULSET : OCI_Statement *
 *   - OBJECTS, UDT : OCI_Object *
 *   - Character columns (CHAR,VARCHAR, etc..) : dtext *
 *   - All NUMERIC types :
 *        - default : big_int
 *        - user defined (see OCI_SetStructNumericType())
 *
 * See OCI_GetStruct() and OCI_SetStructNumericType() for more details
 *
 * @par Fetch Example
 * @include fetch.c
 *
 * @par Fetch Rows into user structures Example
 * @include fetch_struct.c
 *
 * @par Metadata Example
 * @include meta.c
 *
 * @par Ref cursor Example
 * @include cursor.c
 *
 * @par Scrollable resultset Example
 * @include scroll.c
 *
 */

/**
 * @brief
 * Retrieve the resultset handle from an executed statement
 *
 * @param stmt - Statement handle
 *
 * @note
 * On a successful SELECT execution, OCI_GetResultset() always allocates a
 * resultset wherever it contains rows.
 *
 * @note
 * If a DML statement includes an returning clause, a resultset is implicitly
 * created at the SQL statement execution time
 *
 * @warning
 * If the statement has not been prepared and executed, no resultset will be
 * returned
 *
 * @return
 * A resultset handle on success otherwise NULL
 *
 */

OCI_EXPORT OCI_Resultset * OCI_API OCI_GetResultset
(
    OCI_Statement *stmt
);

/**
 * @brief
 * Free the statement resultsets
 *
 * @param stmt - Statement handle
 *
 * @note
 * This call is optional. Resultsets are automatically freed when the
 * statement is destroyed or when it's reused.
 *
 * @note
 * This function has been introduced for releasing big resultsets when the
 * application wants to keep the statement alive and doesn't know when it
 * will be destroyed.
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_ReleaseResultsets
(
    OCI_Statement *stmt
);

/**
 * @brief
 * Fetch the next row of the resultset
 *
 * @param rs - Resultset handle
 *
 * @note
 * OCI_FetchNext() works for normal and scrollable resultsets
 *
 * @return
 * TRUE on success otherwise FALSE if :
 * - Empty resultset
 * - Last row already fetched
 * - An error occurred
 *
 */

OCI_EXPORT boolean OCI_API OCI_FetchNext
(
    OCI_Resultset *rs
);

/**
 * @brief
 * Fetch the previous row of the resultset
 *
 * @param rs - Resultset handle
 *
 * @note
 * OCI_FetchPrev() works ONLY for scrollable resultsets
 *
 * @return
 * TRUE on success otherwise FALSE if :
 * - Empty resultset
 * - First row already fetched
 * - An error occurred
 *
 */

OCI_EXPORT boolean OCI_API OCI_FetchPrev
(
    OCI_Resultset *rs
);

/**
 * @brief
 * Fetch the first row of the resultset
 *
 * @param rs - Resultset handle
 *
 * @note
 * OCI_FetchFirst() works ONLY for scrollable resultsets
 *
 * @return
 * TRUE on success otherwise FALSE if :
 * - Empty resultset
 * - An error occurred
 *
 */

OCI_EXPORT boolean OCI_API OCI_FetchFirst
(
    OCI_Resultset *rs
);

/**
 * @brief
 * Fetch the last row of the resultset
 *
 * @param rs - Resultset handle
 *
 * @note
 * OCI_FetchLast() works ONLY for scrollable resultsets
 *
 * @return
 * TRUE on success otherwise FALSE if:
 * - Empty resultset
 * - An error occurred
 *
 */

OCI_EXPORT boolean OCI_API OCI_FetchLast
(
    OCI_Resultset *rs
);

/**
 * @brief
 * Custom Fetch of the resultset
 *
 * @param rs      - Resultset handle
 * @param mode    - Fetch direction
 * @param offset  - Fetch offset
 *
 * @note
 * Possible values for 'direction' parameter are:
 *  - OCI_SFD_ABSOLUTE
 *  - OCI_SFD_RELATIVE
 *
 * @note
 * OCI_FetchSeek() works ONLY for scrollable resultsets
 *
 * @return
 * TRUE on success otherwise FALSE if:
 * - Empty resultset
 * - An error occurred
 * - OCI_SetFetchMode() has not been called with OCI_SFM_SCROLLABLE
 *
 */

OCI_EXPORT boolean OCI_API OCI_FetchSeek
(
    OCI_Resultset *rs,
    unsigned int   mode,
    int            offset
);

/**
 * @brief
 * Retrieve the number of rows fetched so far
 *
 * @param rs - Resultset handle
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_GetRowCount
(
    OCI_Resultset *rs
);

/**
 * @brief
 * Retrieve the current row number
 *
 * @param rs - Resultset handle
 *
 * @note
 * - OCI_GetCurrentRow() returns the current row number starting from 1
 * - If the resultset has not been fetched or if the resultset is empty, it returns 0
 * - If the resultset has been fully fetched, it returns the last fetched row number
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_GetCurrentRow
(
    OCI_Resultset *rs
);

/**
 * @brief
 * Return the number of columns in the resultset
 *
 * @param rs - Resultset handle
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_GetColumnCount
(
    OCI_Resultset *rs
);

/**
 * @brief
 * Return the column object handle at the given index in the resultset
 *
 * @param rs    - Resultset handle
 * @param index - Column position
 *
 * @return
 * - Column handle on success
 * - NULL if index is out of bounds or on error
 *
 */

OCI_EXPORT OCI_Column * OCI_API OCI_GetColumn
(
    OCI_Resultset *rs,
    unsigned int   index
);

/**
 * @brief
 * Return the column object handle from its name in the resultset
 *
 * @param rs    - Resultset handle
 * @param name  - Column name
 *
 * @note
 * The column name is case insensitive
 *
 * @return
 * - Column handle on success or
 * - NULL if no column found with the given name or on error
 *
 */

OCI_EXPORT OCI_Column * OCI_API OCI_GetColumn2
(
    OCI_Resultset *rs,
    const mtext   *name
);

/**
 * @brief
 * Return the index of the column in the result from its name
 *
 * @param rs    - Resultset handle
 * @param name  - Column name
 *
 * @note
 * The column name is case insensitive
 *
 * @note
 * Column indexes start with 1 in OCILIB
 *
 * @return
 * Column index on success or zero on error
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_GetColumnIndex
(
    OCI_Resultset *rs,
    const mtext   *name
);

/**
 * @brief
 * Return the name of the given column
 *
 * @param col   - Column handle
 *
 */

OCI_EXPORT const mtext * OCI_API OCI_ColumnGetName
(
    OCI_Column *col
);

/**
 * @brief
 * Return the type of the given column
 *
 * @param col   - Column handle
 *
 * @note
 * Possible values are :
 *
 * - OCI_CDT_NUMERIC     : short, int, long long, double
 * - OCI_CDT_DATETIME    : OCI_Date *
 * - OCI_CDT_TEXT        : dtext *
 * - OCI_CDT_LONG        : OCI_Long *
 * - OCI_CDT_CURSOR      : OCI_Statement *
 * - OCI_CDT_LOB         : OCI_Lob  *
 * - OCI_CDT_FILE        : OCI_File *
 * - OCI_CDT_TIMESTAMP   : OCI_Timestamp *
 * - OCI_CDT_INTERVAL    : OCI_Interval *
 * - OCI_CDT_RAW         : void *
 * - OCI_CDT_OBJECT      : OCI_Object *
 * - OCI_CDT_COLLECTION  : OCI_Coll *
 * - OCI_CDT_REF         : OCI_Ref *
 *
 * @return
 * The column type or OCI_CDT_UNKNOWN if index is out of bounds
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_ColumnGetType
(
    OCI_Column *col
);

/**
 * @brief
 * Return the charset form of the given column
 *
 * @param col   - Column handle
 *
 * @note
 * Possible values are :
 * - OCI_CSF_NONE     : the column is not an character or lob column
 * - OCI_CSF_DEFAULT  : the column has server default charset
 * - OCI_CSF_NATIONAL : the column has national server charset
 */

OCI_EXPORT unsigned int OCI_API OCI_ColumnGetCharsetForm
(
    OCI_Column *col
);

/**
 * @brief
 * Return the Oracle SQL type name of the column datatype
 *
 * @param col   - Column handle
 *
 * @note
 * For possible values, consults Oracle Documentation
 *
 */

OCI_EXPORT const mtext * OCI_API OCI_ColumnGetSQLType
(
    OCI_Column *col
);

/**
 * @brief
 * Return the Oracle SQL Full name including precision and size of the
 * column datatype
 *
 * @param col    - Column handle
 * @param buffer - buffer to store the full column type name and size
 * @param len    - max size of the buffer in characters
 *
 * @note
 * This function returns a description that matches the one given by SQL*Plus

 * @note
 * Return the number of characters written into the buffer
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_ColumnGetFullSQLType
(
    OCI_Column  *col,
    mtext       *buffer,
    unsigned int len
);

/**
 * @brief
 * Return the size of the column
 *
 * @note
 * For all types, the size is expressed is bytes, excepted for character
 * based columns that were created with a character based size or of type NCHAR/NVARCHAR
 *
 * @param col   - Column handle
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_ColumnGetSize
(
    OCI_Column *col
);

/**
 * @brief
 * Return the scale of the column for numeric columns
 *
 * @param col   - Column handle
 *
 */

OCI_EXPORT int OCI_API OCI_ColumnGetScale
(
    OCI_Column *col
);

/**
 * @brief
 * Return the precision of the column for numeric columns
 *
 * @param col   - Column handle
 *
 */

OCI_EXPORT int OCI_API OCI_ColumnGetPrecision
(
    OCI_Column *col
);

/**
 * @brief
 * Return the fractional precision of the column for timestamp and interval columns
 *
 * @param col   - Column handle
 *
 */

OCI_EXPORT int OCI_API OCI_ColumnGetFractionalPrecision
(
    OCI_Column *col
);

/**
 * @brief
 * Return the leading precision of the column for interval columns
 *
 * @param col   - Column handle
 *
 */

OCI_EXPORT int OCI_API OCI_ColumnGetLeadingPrecision
(
    OCI_Column *col
);

/**
 * @brief
 * Return the nullable attribute of the column
 *
 * @param col   - Column handle
 *
 * @return
 * Return TRUE if the column is nullable otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_ColumnGetNullable
(
    OCI_Column *col
);

/**
 * @brief
 * Return TRUE if the length of the column is character-length or  FALSE if
 * it is byte-length
 *
 * @param col - Column handle
 *
 * @note
 * This was introduced in Oracle 9i. So for version that are not supporting this
 * property, it always return FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_ColumnGetCharUsed
(
    OCI_Column *col
);

/**
 * @brief
 * Return the type information object associated to the column
 *
 * @param col - Column handle
 *
 * @note
 * This call is used only for Named Object typed and collection columns.
 * It returns NULL if the column is not a Named Object or a collection.
 *
 */

OCI_EXPORT OCI_TypeInfo * OCI_API OCI_ColumnGetTypeInfo
(
    OCI_Column *col
);

/**
 * @brief
 * Return the OCILIB object subtype of a column
 *
 * @param col - Column handle
 *
 * @note
 *
 * This call is valid for the following OCILIB types:
 *
 * - OCI_CDT_LONG
 * - OCI_CDT_LOB
 * - OCI_CDT_FILE
 * - OCI_CDT_TIMESTAMP
 * - OCI_CDT_INTERVAL
 *
 * For OCI_Long type the possible values are:
 * - OCI_BLONG
 * - OCI_CLONG
 *
 * For OCI_Lob type the possible values are:
 * - OCI_BLOB
 * - OCI_CLOB
 * - OCI_NCLOB
 *
 * For OCI_File type the possible values are:
 * - OCI_BFILE
 * - OCI_CFILE
 *
 * For OCI_Timestamp type the possible values are:
 * - OCI_TIMESTAMP
 * - OCI_TIMESTAMP_TZ
 * - OCI_TIMESTAMP_LTZ
 *
 * For OCI_Interval type the possible values are:
 * - OCI_INTERVAL_YM
 * - OCI_INTERVAL_DS
 *
 * @note
 * For all other OCILIB types, it returns OCI_UNKNOWN
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_ColumnGetSubType
(
    OCI_Column *col
);

/**
 * @brief
 * set the numeric datatype of the given structure member (identified from position in the
 * resultset)  to retrieve when calling OCI_GetStruct()
 *
 * @param rs    - Resultset handle
 * @param index - Column position
 * @param type  - Numeric type
 *
 * @note
 * Possible values for parameter 'type' :
 *   - OCI_NUM_SHORT
 *   - OCI_NUM_USHORT
 *   - OCI_NUM_INT
 *   - OCI_NUM_UINT
 *   - OCI_NUM_BIGINT
 *   - OCI_NUM_BIGUINT
 *   - OCI_NUM_DOUBLE
 *
 * @return
 * Return TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_SetStructNumericType
(
    OCI_Resultset *rs,
    unsigned int   index,
    unsigned int   type
);

/**
 * @brief
 * set the numeric datatype of the given structure member (identified from column name in the
 * resultset)  to retrieve when calling OCI_GetStruct()
 *
 * @param rs    - Resultset handle
 * @param name  - Column name
 * @param type  - Numeric type
 *
 * @note
 * Possible values for parameter 'type' :
 *   - OCI_NUM_SHORT
 *   - OCI_NUM_USHORT
 *   - OCI_NUM_INT
 *   - OCI_NUM_UINT
 *   - OCI_NUM_BIGINT
 *   - OCI_NUM_BIGUINT
 *   - OCI_NUM_DOUBLE
 *
 * @return
 * Return TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_SetStructNumericType2
(
    OCI_Resultset *rs,
    const mtext   *name,
    unsigned int   type
);

/**
 * @brief
 * Return the row columns values into a single structure
 *
 * @param rs              - Resultset handle
 * @param row_struct      - pointer to user row structure
 * @param row_struct_ind  - pointer to user indicator structure
 *
 * @note
 * Structure members values are contextual to the current row.
 * The returned values can get out of scope when the current row
 * changes when calling any OCI_FecthXXX() calls
 *
 * @par User row structure
 *
 * The user structure must have the same members than the resultset.
 * Each column in the resulset must have its equivalent in the structure.
 * Fields must be in the same order.
 *
 * The mapping rules are :
 *
 *   - LOBs (CLOB, NCLOB, BLOB) : OCI_Lob *
 *   - DATE : OCI_Date *
 *   - TIMESTAMPS : OCI_Timestamp *
 *   - INTERVALS : OCI_Interval *
 *   - LONG, LONG RAW : OCI_Long *
 *   - REFs : OCI_Ref *
 *   - CURSOR, RESULSET : OCI_Statement *
 *   - OBJECTS, UDT : OCI_Object *
 *   - Character columns (CHAR,VARCHAR, etc..) : dtext *
 *   - All NUMERIC types :
 *        - default : big_int
 *        - user defined (see OCI_SetStructNumericType())
 *
 * The user structure pointer is not mandatory
 *
 * @par User row indicator structure

 * This structure must have one boolean field per column in
 * the resulset and respect in the same member order.
 *
 * If the value of the given member is TRUE, it means the value in
 * the user row structure is NOT NULL, otherwise its NULL
 *
 * The user indicator structure pointer is  mandatory
 *
 * @return
 * Return TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_GetStruct
(
    OCI_Resultset *rs,
    void          *row_struct,
    void          *row_struct_ind
);

/**
 * @brief
 * Return the current short value of the column at the given index in the resultset
 *
 * @param rs    - Resultset handle
 * @param index - Column position
 *
 * @note
 * Column position starts at 1.
 *
 * @return
 * The column current row value or 0 if index is out of bounds
 *
 */

OCI_EXPORT short OCI_API OCI_GetShort
(
    OCI_Resultset *rs,
    unsigned int   index
);

/**
 * @brief
 * Return the current short value of the column from its name in the resultset
 *
 * @param rs    - Resultset handle
 * @param name  - Column name
 *
 * @note
 * The column name is case insensitive
 *
 * @return
 * The column current row value or 0 if no column found with the given name
 *
 */

OCI_EXPORT short OCI_API OCI_GetShort2
(
    OCI_Resultset *rs,
    const mtext   *name
);

/**
 * @brief
 * Return the current unsigned short value of the column at the given index in the resultset
 *
 * @param rs    - Resultset handle
 * @param index - Column position
 *
 * @note
 * Column position starts at 1.
 *
 * @return
 * The column current row value or 0 if index is out of bounds
 *
 */

OCI_EXPORT unsigned short OCI_API OCI_GetUnsignedShort
(
    OCI_Resultset *rs,
    unsigned int   index
);

/**
 * @brief
 * Return the current unsigned short value of the column from its name in the resultset
 *
 * @param rs    - Resultset handle
 * @param name  - Column name
 *
 * @note
 * The column name is case insensitive
 *
 * @return
 * The column current row value or 0 if no column found with the given name
 *
 */

OCI_EXPORT unsigned short OCI_API OCI_GetUnsignedShort2
(
    OCI_Resultset *rs,
    const mtext   *name
);

/**
 * @brief
 * Return the current integer value of the column at the given index in the resultset
 *
 * @param rs    - Resultset handle
 * @param index - Column position
 *
 * @note
 * Column position starts at 1.
 *
 * @return
 * The column current row value or 0 if index is out of bounds
 *
 */

OCI_EXPORT int OCI_API OCI_GetInt
(
    OCI_Resultset *rs,
    unsigned int   index
);

/**
 * @brief
 * Return the current integer value of the column from its name in the resultset
 *
 * @param rs    - Resultset handle
 * @param name  - Column name
 *
 * @note
 * The column name is case insensitive
 *
 * @return
 * The column current row value or 0 if no column found with the given name
 *
 */

OCI_EXPORT int OCI_API OCI_GetInt2
(
    OCI_Resultset *rs,
    const mtext   *name
);

/**
 * @brief
 * Return the current unsigned integer value of the column at the given index in the resultset
 *
 * @param rs    - Resultset handle
 * @param index - Column position
 *
 * @note
 * Column position starts at 1.
 *
 * @return
 * The column current row value or 0 if index is out of bounds
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_GetUnsignedInt
(
    OCI_Resultset *rs,
    unsigned int   index
);

/**
 * @brief
 * Return the current unsigned integer value of the column from its name in the resultset
 *
 * @param rs    - Resultset handle
 * @param name  - Column name
 *
 * @note
 * The column name is case insensitive
 *
 * @return
 * The column current row value or 0 if no column found with the given name
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_GetUnsignedInt2
(
    OCI_Resultset *rs,
    const mtext   *name
);

/**
 * @brief
 * Return the current big integer value of the column at the given index in the resultset
 *
 * @param rs    - Resultset handle
 * @param index - Column position
 *
 * @note
 * Column position starts at 1.
 *
 * @return
 * The column current row value or 0 if index is out of bounds
 *
 */

OCI_EXPORT big_int OCI_API OCI_GetBigInt
(
    OCI_Resultset *rs,
    unsigned int   index
);

/**
 * @brief
 * Return the current big integer value of the column from its name in the resultset
 *
 * @param rs    - Resultset handle
 * @param name  - Column name
 *
 * @note
 * The column name is case insensitive
 *
 * @return
 * The column current row value or 0 if no column found with the given name
 *
 */

OCI_EXPORT big_int OCI_API OCI_GetBigInt2
(
    OCI_Resultset *rs,
    const mtext   *name
);

/**
 * @brief
 * Return the current unsigned big integer value of the column at the given index in the resultset
 *
 * @param rs    - Resultset handle
 * @param index - Column position
 *
 * @note
 * Column position starts at 1.
 *
 * @return
 * The column current row value or 0 if index is out of bounds
 *
 */

OCI_EXPORT big_uint OCI_API OCI_GetUnsignedBigInt
(
    OCI_Resultset *rs,
    unsigned int   index
);

/**
 * @brief
 * Return the current unsigned big integer value of the column from its name in the resultset
 *
 * @param rs    - Resultset handle
 * @param name  - Column name
 *
 * @note
 * The column name is case insensitive
 *
 * @return
 * The column current row value or 0 if no column found with the given name
 *
 */

OCI_EXPORT big_uint OCI_API OCI_GetUnsignedBigInt2
(
    OCI_Resultset *rs,
    const mtext   *name
);

/**
 * @brief
 * Return the current string value of the column at the given index in the resultset
 *
 * @param rs    - Resultset handle
 * @param index - Column position
 *
 * @note
 * Column position starts at 1.
 *
 * @return
 * The column current row value or NULL if index is out of bounds
 *
 */

OCI_EXPORT const dtext * OCI_API OCI_GetString
(
    OCI_Resultset *rs,
    unsigned int   index
);

/**
 * @brief
 * Return the current string value of the column from its name in the resultset
 *
 * @param rs    - Resultset handle
 * @param name  - Column name
 *
 * @note
 * The column name is case insensitive
 *
 * @return
 * The column current row value or NULL if no column found with the given name
 *
 */

OCI_EXPORT const dtext * OCI_API OCI_GetString2
(
    OCI_Resultset *rs,
    const mtext   *name
);

/**
 * @brief
 * Copy the current raw value of the column at the given index into the specified buffer
 *
 * @param rs     - Resultset handle
 * @param index  - Column position
 * @param buffer - Buffer that receive the raw value
 * @param len    - Max size of the input buffer in bytes
 *
 * @note
 * Column position starts at 1.
 *
 * @return
 * Number of bytes copied into the buffer on SUCCESS otherwise 0
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_GetRaw
(
    OCI_Resultset *rs,
    unsigned int   index,
    void          *buffer,
    unsigned int   len
);

/**
 * @brief
 * Copy the current raw value of the column from its name into the specified buffer
 *
 * @param rs     - Resultset handle
 * @param name   - Column name
 * @param buffer - Buffer that receive the raw value
 * @param len    - Max size of the input buffer
 *
 * @note
 * The column name is case insensitive
 *
 * @return
 * Number of bytes copied into the buffer on SUCCESS otherwise 0
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_GetRaw2
(
    OCI_Resultset *rs,
    const mtext   *name,
    void          *buffer,
    unsigned int   len
);

/**
 * @brief
 * Return the current double value of the column at the given index in the resultset
 *
 * @param rs    - Resultset handle
 * @param index - Column position
 *
 * @note
 * Column position starts at 1.
 *
 * @return
 * The column current row value or 0.O if index is out of bounds
 *
 */

OCI_EXPORT double OCI_API OCI_GetDouble
(
    OCI_Resultset *rs,
    unsigned int   index
);

/**
 * @brief
 * Return the current double value of the column from its name in the resultset
 *
 * @param rs    - Resultset handle
 * @param name  - Column name
 *
 * @note
 * The column name is case insensitive
 *
 * @return
 * The column current row value or 0.0 if no column found with the given name
 *
 */

OCI_EXPORT double OCI_API OCI_GetDouble2
(
    OCI_Resultset *rs,
    const mtext   *name
);

/**
 * @brief
 * Return the current date value of the column at the given index in the resultset
 *
 * @param rs    - Resultset handle
 * @param index - Column position
 *
 * @note
 * Column position starts at 1.
 *
 * @return
 * The column current row value or NULL if index is out of bounds
 *
 */

OCI_EXPORT OCI_Date * OCI_API OCI_GetDate
(
    OCI_Resultset *rs,
    unsigned int   index
);

/**
 * @brief
 * Return the current date value of the column from its name in the resultset
 *
 * @param rs    - Resultset handle
 * @param name  - Column name
 *
 * @return
 * The column current row value or NULL if no column found with the given name
 *
 */

OCI_EXPORT OCI_Date * OCI_API OCI_GetDate2
(
    OCI_Resultset *rs,
    const mtext   *name
);

/**
 * @brief
 * Return the current timestamp value of the column at the given index in the resultset
 *
 * @param rs    - Resultset handle
 * @param index - Column position
 *
 * @note
 * Column position starts at 1.
 *
 * @return
 * The column current row value or NULL if index is out of bounds
 *
 */

OCI_EXPORT OCI_Timestamp * OCI_API OCI_GetTimestamp
(
    OCI_Resultset *rs,
    unsigned int   index
);

/**
 * @brief
 * Return the current timestamp value of the column from its name in the resultset
 *
 * @param rs    - Resultset handle
 * @param name  - Column name
 *
 * @return
 * The column current row value or NULL if no column found with the given name
 *
 */

OCI_EXPORT OCI_Timestamp * OCI_API OCI_GetTimestamp2
(
    OCI_Resultset *rs,
    const mtext   *name
);

/**
 * @brief
 * Return the current interval value of the column at the given index in the resultset
 *
 * @param rs    - Resultset handle
 * @param index - Column position
 *
 * @note
 * Column position starts at 1.
 *
 * @return
 * The column current row value or NULL if index is out of bounds
 *
 */

OCI_EXPORT OCI_Interval * OCI_API OCI_GetInterval
(
    OCI_Resultset *rs,
    unsigned int   index
);

/**
 * @brief
 * Return the current interval value of the column from its name in the resultset
 *
 * @param rs    - Resultset handle
 * @param name  - Column name
 *
 * @return
 * The column current row value or NULL if no column found with the given name
 *
 */

OCI_EXPORT OCI_Interval * OCI_API OCI_GetInterval2
(
    OCI_Resultset *rs,
    const mtext   *name
);

/**
 * @brief
 * Return the current cursor value (Nested table) of the column at the given index in the resultset
 *
 * @param rs    - Resultset handle
 * @param index - Column position
 *
 * @note
 * Column position starts at 1.
 *
 * @return
 * The column current row value or NULL if index is out of bounds
 *
 */

OCI_EXPORT OCI_Statement * OCI_API OCI_GetStatement
(
    OCI_Resultset *rs,
    unsigned int   index
);

/**
 * @brief
 * Return the current cursor value of the column from its name in the resultset
 *
 * @param rs    - Resultset handle
 * @param name  - Column name
 *
 * @return
 * The column current row value or NULL if no column found with the given name
 *
 */

OCI_EXPORT OCI_Statement * OCI_API OCI_GetStatement2
(
    OCI_Resultset *rs,
    const mtext   *name
);

/**
 * @brief
 * Return the current lob value of the column at the given index in the resultset
 *
 * @param rs    - Resultset handle
 * @param index - Column position
 *
 * @note
 * Column position starts at 1.
 *
 * @return
 * The column current row value or NULL if index is out of bounds
 *
 */

OCI_EXPORT OCI_Lob * OCI_API OCI_GetLob
(
    OCI_Resultset *rs,
    unsigned int   index
);

/**
 * @brief
 * Return the current lob value of the column from its name in the resultset
 *
 * @param rs    - Resultset handle
 * @param name  - Column name
 *
 * @return
 * The column current row value or NULL if no column found with the given name
 *
 */

OCI_EXPORT OCI_Lob * OCI_API OCI_GetLob2
(
    OCI_Resultset *rs,
    const mtext   *name
);

/**
 * @brief
 * Return the current File value of the column at the given index in the resultset
 *
 * @param rs    - Resultset handle
 * @param index - Column position
 *
 * @note
 * Column position starts at 1.
 *
 * @return
 * The column current row value or NULL if index is out of bounds
 *
 */

OCI_EXPORT OCI_File * OCI_API OCI_GetFile
(
    OCI_Resultset *rs,
    unsigned int   index
);

/**
 * @brief
 * Return the current File value of the column from its name in the resultset
 *
 * @param rs    - Resultset handle
 * @param name  - Column name
 *
 * @return
 * The column current row value or NULL if no column found with the given name
 *
 */

OCI_EXPORT OCI_File * OCI_API OCI_GetFile2
(
    OCI_Resultset *rs,
    const mtext   *name
);

/**
 * @brief
 * Return the current Object value of the column at the given index in the resultset
 *
 * @param rs    - Resultset handle
 * @param index - Column position
 *
 * @note
 * Column position starts at 1.
 *
 * @return
 * The column current row value or NULL if index is out of bounds
 *
 */

OCI_EXPORT OCI_Object * OCI_API OCI_GetObject
(
    OCI_Resultset *rs,
    unsigned int   index
);

/**
 * @brief
 * Return the current Object value of the column from its name in the resultset
 *
 * @param rs    - Resultset handle
 * @param name  - Column name
 *
 * @return
 * The column current row value or NULL if no column found with the given name
 *
 */

OCI_EXPORT OCI_Object * OCI_API OCI_GetObject2
(
    OCI_Resultset *rs,
    const mtext   *name
);

/**
 * @brief
 * Return the current Collection value of the column at the given index in the resultset
 *
 * @param rs    - Resultset handle
 * @param index - Column position
 *
 * @note
 * Column position starts at 1.
 *
 * @return
 * The column current row value or NULL if index is out of bounds
 *
 */

OCI_EXPORT OCI_Coll * OCI_API OCI_GetColl
(
    OCI_Resultset *rs,
    unsigned int   index
);

/**
 * @brief
 * Return the current Collection value of the column from its name in the resultset
 *
 * @param rs    - Resultset handle
 * @param name  - Column name
 *
 * @return
 * The column current row value or NULL if no column found with the given name
 *
 */

OCI_EXPORT OCI_Coll * OCI_API OCI_GetColl2
(
    OCI_Resultset *rs,
    const mtext   *name
);

/**
 * @brief
 * Return the current Ref value of the column at the given index in the resultset
 *
 * @param rs    - Resultset handle
 * @param index - Column position
 *
 * @note
 * Column position starts at 1.
 *
 * @return
 * The column current row value or NULL if index is out of bounds
 *
 */

OCI_EXPORT OCI_Ref * OCI_API OCI_GetRef
(
    OCI_Resultset *rs,
    unsigned int   index
);

/**
 * @brief
 * Return the current Ref value of the column from its name in the resultset
 *
 * @param rs    - Resultset handle
 * @param name  - Column name
 *
 * @return
 * The column current row value or NULL if no column found with the given name
 *
 */

OCI_EXPORT OCI_Ref * OCI_API OCI_GetRef2
(
    OCI_Resultset *rs,
    const mtext   *name
);

/**
 * @brief
 * Return the current Long value of the column at the given index in the resultset
 *
 * @param rs    - Resultset handle
 * @param index - Column position
 *
 * @note
 * Column position starts at 1.
 *
 * @return
 * The column current row value or NULL if index is out of bounds
 *
 */

OCI_EXPORT OCI_Long * OCI_API OCI_GetLong
(
    OCI_Resultset *rs,
    unsigned int   index
);

/**
 * @brief
 * Return the current Long value of the column from its name in the resultset
 *
 * @param rs    - Resultset handle
 * @param name  - Column name
 *
 * @return
 * The column current row value or NULL if no column found with the given name
 *
 */

OCI_EXPORT OCI_Long * OCI_API OCI_GetLong2
(
    OCI_Resultset *rs,
    const mtext   *name
);

/**
 * @brief
 * Check if the current row value is null for the column at the given index in the resultset
 *
 * @param rs    - Resultset handle
 * @param index - Column position
 *
 * @note
 * Column position starts at 1.
 *
 * @return
 * TRUE if it's null otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_IsNull
(
    OCI_Resultset *rs,
    unsigned int   index
);

/**
 * @brief
 * Check if the current row value is null for the column of the given name in the resultset
 *
 * @param rs    - Resultset handle
 * @param name  - Column name
 *
 * @return
 * TRUE if it's null otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_IsNull2
(
    OCI_Resultset *rs,
    const mtext   *name
);

/**
 * @brief
 * Return the statement handle associated with a resultset handle
 *
 * @param rs - resultset handle
 *
 */

OCI_EXPORT OCI_Statement * OCI_API OCI_ResultsetGetStatement
(
    OCI_Resultset *rs
);

/**
 * @brief
 * Return the current row data length of the column at the given index in the resultset
 *
 * @param rs    - Resultset handle
 * @param index - Column position
 *
 * @note
 * Column position starts at 1.
 *
 * @return
 * The column current row data length or 0 if index is out of bounds
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_GetDataLength
(
    OCI_Resultset *rs,
    unsigned int   index
);

/**
 * @}
 */

/**
 * @defgroup g_plsql PL/SQL Support
 * @{
 *
 * OCILIB has a strong PL/SQL support :
 *
 * - Blocks, procedures and function can be used with OCILIB statements.
 * - Ref cursors
 * - Nested tables
 * - Tables (indexed by integer types)
 * - Access to the server side output generated by the DBMS_OUTPUT package
 *
 * Stored procedures/functions calls, blocks declarations are done like regular
 * SQL calls using OCI_Prepare(), OCI_Execute(), OCI_ExecuteStmt() and
 * OCI_ExecuteStmtFmt() functions.
 *
 * All PL/SQL statements must:
 *
 * - start with a 'begin' or 'declare' keyword
 * - end with a 'end;' keyword
 *
 * Binding Host arrays to PL/SQL tables is done with OCI_BindArrayXXX() calls
 *
 * @par Using a PL/SQL block with OCILIB
 * @include plsql_block.c
 *
 * @par Binding host arrays to PL/SQL tables parameters of a stored procedure
 * @include plsql_table.c
 *
 * @par Retrieve the output generated by the dbms_output package on the server
 * @include output.c
 *
 */

/**
 * @brief
 * Enable the server output
 *
 * @param con     - Connection handle
 * @param bufsize - server buffer max size (server side)
 * @param arrsize - number of lines to retrieve per server roundtrip
 * @param lnsize  - maximum size of one line
 *
 * @note
 * This call is equivalent to the command 'set serveroutput on' in SQL*PLUS
 *
 * @note
 * 'bufsize' minimum value is 2000, maximum 1000000 with Oracle < 10.2g and can be unlimited above
 *
 * @note
 * 'lnsize' maximum value is 255 with Oracle < 10g R2 and 32767 above
 *
 * @warning
 * If OCI_ServerEnableOutput() is not called, OCI_ServerGetOutput() will return NULL
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_ServerEnableOutput
(
    OCI_Connection *con,
    unsigned int    bufsize,
    unsigned int    arrsize,
    unsigned int    lnsize
);

/**
 * @brief
 * Disable the server output
 *
 * @param con - Connection handle
 *
 * @note
 * After this call, OCI_ServerGetOutput() will return NULL.
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_ServerDisableOutput
(
    OCI_Connection *con
);

/**
 * @brief
 * Retrieve one line of the server buffer
 *
 * @param con   - Connection handle
 *
 * @note
 * The current transaction is automatically stopped but the newly assigned is not started or resumed
 *
 * @note
 * Internally, OCILIB gets the server buffer through an array of lines in
 * order to minimize roundtrips with the server
 *
 * @return
 * return a server output buffer line or NULL if the server buffer is empty
 *
 */

OCI_EXPORT const dtext * OCI_API OCI_ServerGetOutput
(
    OCI_Connection *con
);

/**
 * @}
 */

/**
 * @defgroup g_collection Oracle collections (Varrays and Nested Tables)
 * @{
 *
 * OCILIB supports all Oracle collections:
 *
 * - PL/SQL Tables: only available in PL/SQL, unbounded, sparse arrays of
                    homogeneous elements.
 * - Varrays      : available in SQL and PL/SQL, they are bounded arrays of
 *                  homogeneous elements
 * - Nested Tables: available in SQL and PL/SQL, they are unbounded arrays of
 *                  homogeneous elements and can become sparse through deletions
 *
 * PL/SQL tables are implemented by binding regular C arrays with the array
 * interface (using OCI_BindArrayOfXXX() calls)
 *
 * Varrays and Nested tables are implemented in OCILIB with the type OCI_Coll.
 * It's possible to bind and fetch Varrays and Nested tables using OCI_Coll handle.
 *
 * It's also possible to declare local collections based on some database type without using queries
 *
 * OCI (and thus OCILIB) offers the possibility to access collection elements :
 *
 * - directly by index (OCI_CollGetAt() and OCI_CollSetAt())
 * - using an iterator (OCI_Iter) to iterate through the collection
 *   (OCI_IterGetNext(), OCI_IterGetPrev())
 *
 * Collection Items are implemented through the type OCI_Elem and use the series
 * of calls OCI_ElemGetXXX() and OCI_ElemSetXXX() to manipulate elements
 * content values
 *
 * @par Example
 * @include coll.c
 *
 */

/**
 * @brief
 * Create a local collection instance
 *
 * @param typinf - Type info handle of the collection type descriptor
 *
 * @return
 * Return the collection object handle on success otherwise NULL on failure
 *
 */

OCI_EXPORT OCI_Coll * OCI_API OCI_CollCreate
(
    OCI_TypeInfo *typinf
);

/**
 * @brief
 * Free a local collection
 *
 * @param coll - Collection handle
 *
 * @warning
 * Only collection created with OCI_CollCreate() should be freed
 * by OCI_CollFree()
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_CollFree
(
    OCI_Coll *coll
);

/**
 * @brief
 * Create an array of Collection object
 *
 * @param con    - Connection handle
 * @param typinf - Object type (type info handle)
 * @param nbelem - number of elements in the array
 *
 * @note
 * see OCI_ObjectCreate() for more details
 *
 * @return
 * Return the Collection handle array on success otherwise NULL on failure
 *
 */

OCI_EXPORT OCI_Coll ** OCI_API OCI_CollArrayCreate
(
    OCI_Connection *con,
    OCI_TypeInfo   *typinf,
    unsigned int    nbelem
);

/**
 * @brief
 * Free an arrray of Collection objects
 *
 * @param colls - Array of Collection objects
 *
 * @warning
 * Only arrays of Collection created with OCI_CollArrayCreate()
 * should be freed by OCI_CollArrayFree()
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_CollArrayFree
(
    OCI_Coll **colls
);

/**
 * @brief
 * Assign a collection to another one
 *
 * @param coll     - Destination Collection handle
 * @param coll_src - Source Collection handle
 *
 * @note
 * Oracle proceeds to a deep copy of the collection content
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_CollAssign
(
    OCI_Coll *coll,
    OCI_Coll *coll_src
);

/**
 * @brief
 * Return the type info object associated to the collection
 *
 * @param coll - Collection handle
 *
 */

OCI_EXPORT OCI_TypeInfo * OCI_API OCI_CollGetTypeInfo
(
    OCI_Coll *coll
);

/**
 * @brief
 * Return the collection type
 *
 * @param coll - Collection handle
 *
 * @note
 * Current collection types are:
 *
 * - OCI_COLL_VARRAY: Oracle VARRAY
 * - OCI_COLL_NESTED_TABLE: Oracle Nested Table
 *
 * @return
 * Collection type or OCI_UNKNOWN if the collection handle is null
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_CollGetType
(
    OCI_Coll *coll
);

/**
 * @brief
 * Returns the maximum number of elements of the given collection.
 *
 * @param coll - Collection handle
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_CollGetMax
(
    OCI_Coll *coll
);

/**
 * @brief
 * Returns the current number of elements of the given collection.
 *
 * @param coll - Collection handle
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_CollGetSize
(
    OCI_Coll *coll
);

/**
 * @brief
 * Trims the given number of elements from the end of the collection
 *
 * @param coll    - Collection handle
 * @param nb_elem - Number of elements to trim
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_CollTrim
(
    OCI_Coll    *coll,
    unsigned int nb_elem
);

/**
 * @brief
 * clear all items of the given collection
 *
 * @param coll - Collection handle
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_CollClear
(
    OCI_Coll *coll
);

/**
 * @brief
 * Return the element at the given position in the collection
 *
 * @param coll  - Collection handle
 * @param index - Index of the destination element
 *
 * @note
 * Collection indexes start at position 1.
 *
 * @note
 * Up to 3.3.0, the library checked that the input index was fitting into the
 * collection bounds. From 3.3.1, this check has been removed for some internal
 * reasons. An exception will be still thrown in case of out of bounds index but
 * the exception type is now an OCI exception instead of an OCILIB one.
 *
 * @return
 * Element handle on success otherwise FALSE
 *
 */

OCI_EXPORT OCI_Elem * OCI_API OCI_CollGetAt
(
    OCI_Coll    *coll,
    unsigned int index
);

/**
 * @brief
 * Return the element at the given position in the collection
 *
 * @param coll  - Collection handle
 * @param index - Index of the destination element
 * @param elem  - Element handle to hold the collection item data
 *
 * @note
 * Collection indexes start at position 1.
 *
 * @note
 * Up to 3.3.0, the library checked that the input index was fitting into the
 * collection bounds. From 3.3.1, this check has been removed for some internal
 * reasons. An exception will be still thrown in case of out of bounds index but
 * the exception type is now an OCI exception instead of an OCILIB one.
 *
 * @return
 * Element handle on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_CollGetAt2
(
    OCI_Coll    *coll,
    unsigned int index,
    OCI_Elem    *elem
);

/**
 * @brief
 * Assign the given element value to the element at the given position in
 * the collection
 *
 * @param coll  - Collection handle
 * @param index - Index of the destination element
 * @param elem  - Source element handle to assign
 *
 * @note
 * Collection indexes start at position 1.
 *
 * @note
 * Up to 3.3.0, the library checked that the input index was fitting into the
 * collection bounds. From 3.3.1, this check has been removed for some internal
 * reasons. An exception will be still thrown in case of out of bounds index but
 * the exception type is now an OCI exception instead of an OCILIB one.
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_CollSetAt
(
    OCI_Coll    *coll,
    unsigned int index,
    OCI_Elem    *elem
);

/**
 * @brief
 * Append the given element at the end of the collection
 *
 * @param coll  - Collection handle
 * @param elem  - Element handle to add
 *
 * @note
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_CollAppend
(
    OCI_Coll *coll,
    OCI_Elem *elem
);

/**
 * @brief
 * Create an iterator handle to iterate through a collection
 *
 * @param coll  - Collection handle
 *
 * @return
 * Return the iterator handle on success otherwise NULL on failure
 *
 */

OCI_EXPORT OCI_Iter * OCI_API OCI_IterCreate
(
    OCI_Coll *coll
);

/**
 * @brief
 * Free an iterator handle
 *
 * @param iter - Iterator handle
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_IterFree
(
    OCI_Iter *iter
);

/**
 * @brief
 * Get the next element in the collection
 *
 * @param iter - Iterator handle
 *
 * @return
 * TRUE on success otherwise FALSE if:
 * - Empty collection
 * - Iterator already positioned on the last collection element
 * - An error occurred
 *
 */

OCI_EXPORT OCI_Elem * OCI_API OCI_IterGetNext
(
    OCI_Iter *iter
);

/**
 * @brief
 * Get the previous element in the collection
 *
 * @param iter - Iterator handle
 *
 * @return
 * TRUE on success otherwise FALSE if:
 * - Empty collection
 * - Iterator already positioned on the first collection element
 * - An error occurred
 *
 */

OCI_EXPORT OCI_Elem * OCI_API OCI_IterGetPrev
(
    OCI_Iter *iter
);

/**
 * @brief
 * Create a local collection element instance based on a collection type
 * descriptor
 *
 * @param typinf  - Type info handle
 *
 * @return
 * Return the collection element handle on success otherwise NULL on failure
 *
 */

OCI_EXPORT OCI_Elem * OCI_API OCI_ElemCreate
(
    OCI_TypeInfo *typinf
);

/**
 * @brief
 * Free a local collection element
 *
 * @param elem   - Element handle
 *
 * @warning
 * Only element created with OCI_ElemCreate() should be freed
 * by OCI_ElemFree()
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_ElemFree
(
    OCI_Elem *elem
);

/**
 * @brief
 * Return the short value of the given collection element
 *
 * @param elem   - Element handle
 *
 * @return
 * Short value or 0 on failure
 *
 */

OCI_EXPORT short OCI_API OCI_ElemGetShort
(
    OCI_Elem *elem
);

/**
 * @brief
 * Return the unsigned short value of the given collection element
 *
 * @param elem   - Element handle
 *
 * @return
 * Unsigned short value or 0 on failure
 *
 */

OCI_EXPORT unsigned short OCI_API OCI_ElemGetUnsignedShort
(
    OCI_Elem *elem
);

/**
 * @brief
 * Return the int value of the given collection element
 *
 * @param elem   - Element handle
 *
 * @return
 * Int value or 0 on failure
 *
 */

OCI_EXPORT int OCI_API OCI_ElemGetInt
(
    OCI_Elem *elem
);

/**
 * @brief
 * Return the unsigned int value of the given collection element
 *
 * @param elem   - Element handle
 *
 * @return
 * Unsigned int value or 0 on failure
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_ElemGetUnsignedInt
(
    OCI_Elem *elem
);

/**
 * @brief
 * Return the big int value of the given collection element
 *
 * @param elem   - Element handle
 *
 * @return
 * Big int value or 0 on failure
 *
 */

OCI_EXPORT big_int OCI_API OCI_ElemGetBigInt
(
    OCI_Elem *elem
);

/**
 * @brief
 * Return the unsigned big int value of the given collection element
 *
 * @param elem   - Element handle
 *
 * @return
 * Unsigned big int value or 0 on failure
 *
 */

OCI_EXPORT big_uint OCI_API OCI_ElemGetUnsignedBigInt
(
    OCI_Elem *elem
);

/**
 * @brief
 * Return the Double value of the given collection element
 *
 * @param elem   - Element handle
 *
 * @return
 * Double value or 0 on failure
 *
 */

OCI_EXPORT double OCI_API OCI_ElemGetDouble
(
    OCI_Elem *elem
);

/**
 * @brief
 * Return the String value of the given collection element
 *
 * @param elem   - Element handle
 *
 * @return
 * String value or NULL on failure
 *
 */

OCI_EXPORT const dtext * OCI_API OCI_ElemGetString
(
    OCI_Elem *elem
);

/**
 * @brief
 * Read the RAW value of the collection element into the given buffer
 *
 * @param elem   - Element handle
 * @param value  - Buffer to store the RAW value
 * @param len    - Size of the buffer
 *
 * @return
 * Number of bytes read from the RAW value or 0 on failure
 *
 */

OCI_EXPORT unsigned int OCI_API  OCI_ElemGetRaw
(
    OCI_Elem    *elem,
    void        *value,
    unsigned int len
);

/**
 * @brief
 * Return the Date value of the given collection element
 *
 * @param elem   - Element handle
 *
 * @return
 * Date handle or NULL on failure
 *
 */

OCI_EXPORT OCI_Date * OCI_API OCI_ElemGetDate
(
    OCI_Elem *elem
);

/**
 * @brief
 * Return the Timestamp value of the given collection element
 *
 * @param elem   - Element handle
 *
 * @return
 * Timestamp handle or NULL on failure
 *
 */

OCI_EXPORT OCI_Timestamp * OCI_API OCI_ElemGetTimestamp
(
    OCI_Elem *elem
);

/**
 * @brief
 * Return the Interval value of the given collection element
 *
 * @param elem   - Element handle
 *
 * @return
 * Interval handle or NULL on failure
 *
 */

OCI_EXPORT OCI_Interval * OCI_API OCI_ElemGetInterval
(
    OCI_Elem *elem
);

/**
 * @brief
 * Return the Lob value of the given collection element
 *
 * @param elem   - Element handle
 *
 * @return
 * Lob handle or NULL on failure
 *
 */

OCI_EXPORT OCI_Lob * OCI_API  OCI_ElemGetLob
(
    OCI_Elem *elem
);

/**
 * @brief
 * Return the File value of the given collection element
 *
 * @param elem   - Element handle
 *
 * @return
 * File handle or NULL on failure
 *
 */

OCI_EXPORT OCI_File * OCI_API  OCI_ElemGetFile
(
    OCI_Elem *elem
);

/**
 * @brief
 * Return the object value of the given collection element
 *
 * @param elem   - Element handle
 *
 * @return
 * Object handle or NULL on failure
 *
 */

OCI_EXPORT OCI_Object * OCI_API OCI_ElemGetObject
(
    OCI_Elem *elem
);

/**
 * @brief
 * Return the collection value of the given collection element
 *
 * @param elem   - Element handle
 *
 * @return
 * Collection handle or NULL on failure
 *
 */

OCI_EXPORT OCI_Coll * OCI_API OCI_ElemGetColl
(
    OCI_Elem *elem
);

/**
 * @brief
 * Return the Ref value of the given collection element
 *
 * @param elem   - Element handle
 *
 * @return
 * Ref handle or NULL on failure
 *
 */

OCI_EXPORT OCI_Ref * OCI_API OCI_ElemGetRef
(
    OCI_Elem *elem
);

/**
 * @brief
 * Set a short value to a collection element
 *
 * @param elem   - Element handle
 * @param value  - Short value
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_ElemSetShort
(
    OCI_Elem *elem,
    short     value
);

/**
 * @brief
 * Set a unsigned short value to a collection element
 *
 * @param elem   - Element handle
 * @param value  - Unsigned short value
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_ElemSetUnsignedShort
(
    OCI_Elem      *elem,
    unsigned short value
);

/**
 * @brief
 * Set a int value to a collection element
 *
 * @param elem   - Element handle
 * @param value  - Int value
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_ElemSetInt
(
    OCI_Elem *elem,
    int       value
);

/**
 * @brief
 * Set a unsigned int value to a collection element
 *
 * @param elem   - Element handle
 * @param value  - Unsigned int value
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_ElemSetUnsignedInt
(
    OCI_Elem    *elem,
    unsigned int value
);

/**
 * @brief
 * Set a big int value to a collection element
 *
 * @param elem   - Element handle
 * @param value  - big int value
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_ElemSetBigInt
(
    OCI_Elem *elem,
    big_int   value
);

/**
 * @brief
 * Set a unsigned big_int value to a collection element
 *
 * @param elem   - Element handle
 * @param value  - Unsigned big int value
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_ElemSetUnsignedBigInt
(
    OCI_Elem *elem,
    big_uint  value
);

/**
 * @brief
 * Set a double value to a collection element
 *
 * @param elem   - Element handle
 * @param value  - Double value
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_ElemSetDouble
(
    OCI_Elem *elem,
    double    value
);

/**
 * @brief
 * Set a string value to a collection element
 *
 * @param elem   - Element handle
 * @param value  - String value
 *
 * @note
 * passing a null pointer for value calls OCI_ElemSetNull()
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_ElemSetString
(
    OCI_Elem    *elem,
    const dtext *value
);

/**
 * @brief
 * Set a RAW value to a collection element
 *
 * @param elem   - Element handle
 * @param value  - Raw value
 * @param len    - Size of the raw value
 *
 * @note
 * passing a null pointer for value calls OCI_ElemSetNull()
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_ElemSetRaw
(
    OCI_Elem    *elem,
    void        *value,
    unsigned int len
);

/**
 * @brief
 * Assign a Date handle to a collection element
 *
 * @param elem   - Element handle
 * @param value  - Date Handle
 *
 * @note
 * passing a null pointer for value calls OCI_ElemSetNull()
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_ElemSetDate
(
    OCI_Elem *elem,
    OCI_Date *value
);

/**
 * @brief
 * Assign a Timestamp handle to a collection element
 *
 * @param elem   - Element handle
 * @param value  - Timestamp Handle
 *
 * @note
 * passing a null pointer for value calls OCI_ElemSetNull()
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_ElemSetTimestamp
(
    OCI_Elem      *elem,
    OCI_Timestamp *value
);

/**
 * @brief
 * Assign an Interval handle to a collection element
 *
 * @param elem   - Element handle
 * @param value  - Interval Handle
 *
 * @note
 * passing a null pointer for value calls OCI_ElemSetNull()
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_ElemSetInterval
(
    OCI_Elem     *elem,
    OCI_Interval *value
);

/**
 * @brief
 * Assign a Collection handle to a collection element
 *
 * @param elem   - Element handle
 * @param value  - Collection Handle
 *
 * @note
 * passing a null pointer for value calls OCI_ElemSetNull()
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_ElemSetColl
(
    OCI_Elem *elem,
    OCI_Coll *value
);

/**
 * @brief
 * Assign an Object handle to a collection element
 *
 * @param elem   - Element handle
 * @param value  - Object Handle
 *
 * @warning
 * This function assigns a copy of the object to the given attribute.
 * Any further modifications of the object passed as the parameter 'value'
 * will not be reflected to object 's attribute set with this call
 *
 * @note
 * passing a null pointer for value calls OCI_ElemSetNull()
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_ElemSetObject
(
    OCI_Elem   *elem,
    OCI_Object *value
);

/**
 * @brief
 * Assign a Lob handle to a collection element
 *
 * @param elem   - Element handle
 * @param value  - Lob Handle
 *
 * @note
 * passing a null pointer for value calls OCI_ElemSetNull()
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_ElemSetLob
(
    OCI_Elem *elem,
    OCI_Lob  *value
);

/**
 * @brief
 * Assign a File handle to a collection element
 *
 * @param elem   - Element handle
 * @param value  - File Handle
 *
 * @note
 * passing a null pointer for value calls OCI_ElemSetNull()
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_ElemSetFile
(
    OCI_Elem *elem,
    OCI_File *value
);

/**
 * @brief
 * Assign a Ref handle to a collection element
 *
 * @param elem   - Element handle
 * @param value  - Ref Handle
 *
 * @note
 * passing a null pointer for value calls OCI_ElemSetNull()
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_ElemSetRef
(
    OCI_Elem *elem,
    OCI_Ref  *value
);

/**
 * @brief
 * Check if the collection element value is null
 *
 * @param elem - Element handle
 *
 * @return
 * TRUE if it's null otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_ElemIsNull
(
    OCI_Elem *elem
);

/**
 * @brief
 * Set a collection element value to null
 *
 * @param elem - Element handle
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_ElemSetNull
(
    OCI_Elem *elem
);

/**
 * @}
 */

/**
 * @defgroup g_ora_ret Oracle Returning feature
 * @{
 *
 * OCILIB supports the Oracle feature 'Returning into' for DML statements.
 *
 * Let's Oracle talk about this features:
 *
 * @par
 * 'Using the RETURNING clause with a DML statement allows you to essentially
 * combine two SQL statements into one, possibly saving you a server round-trip.
 * This is accomplished by adding an extra clause to the traditional UPDATE,
 * INSERT, and DELETE statements. The extra clause effectively adds a query to
 * the DML statement. In the OCI, the values are returned to the application
 * through the use of OUT bind variables.'
 *
 * OCILIB implements this features by providing a set of functions that allows
 * to register output placeholders for the returned values.
 * Once the DML is executed with OCI_Execute(), the output returned data is
 * available through a regular resultset object that can be fetched.
 *
 * @note
 * Array binding interface is also supported with 'returning into' DML statement.
 * Every iteration (or row of given arrays) generates an resultset object.
 * Once a resultset is fetched, the next on can be retrieved with
 * OCI_GetNextResultset()
 *
 * @par
 *
 * @note
 * OCI_Long are not supported for 'returning into' clause .This is a limitation imposed by Oracle.
 *
 * @note
 * OCI_Column objects retrieved from output OCI_Resultset have the following
 * particularities:
 *
 * - their names are the provided bind names to the DML statement
 *   (by example, ':out1'). So any call to the functions OCI_GetXXX2()
 *   should be aware of it
 * - The columns detailed SQL attributes might be not all set or accurate. By
 *   example, the scale and precision are not set, the SQL type is the one
 *   chosen by OCILIB regarding the OCILIB object datatype and might be
 *   slightly different from the real one.
 *
 * @par Example
 * @include returning.c
 *
 */

/**
 * @brief
 * Retrieve the next resultset from an executed DML statement using a 'SQL returning' clause
 *
 * @param stmt - Statement handle
 *
 * @note
 * SQL statements with a 'returning' clause can return multiple resultsets.
 * When arrays of program variables are binded to the statement, Oracle will
 * execute the statement for every row (iteration).
 * Each iteration generates a resultset that can be fetched like regular ones.
 *
 * @return
 * A resultset handle on success otherwise NULL
 *
 */

OCI_EXPORT OCI_Resultset * OCI_API OCI_GetNextResultset
(
    OCI_Statement *stmt
);

/**
 * @brief
 * Register a short output bind placeholder
 *
 * @param stmt - Statement handle
 * @param name - Output bind name
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_RegisterShort
(
    OCI_Statement *stmt,
    const mtext   *name
);

/**
 * @brief
 * Register an unsigned short output bind placeholder
 *
 * @param stmt - Statement handle
 * @param name - Output bind name
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_RegisterUnsignedShort
(
    OCI_Statement *stmt,
    const mtext   *name
);

/**
 * @brief
 * Register an integer output bind placeholder
 *
 * @param stmt - Statement handle
 * @param name - Output bind name
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_RegisterInt
(
    OCI_Statement *stmt,
    const mtext   *name
);

/**
 * @brief
 * Register an unsigned integer output bind placeholder
 *
 * @param stmt - Statement handle
 * @param name - Output bind name
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_RegisterUnsignedInt
(
    OCI_Statement *stmt,
    const mtext   *name
);

/**
 * @brief
 * Register a big integer output bind placeholder
 *
 * @param stmt - Statement handle
 * @param name - Output bind name
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_RegisterBigInt
(
    OCI_Statement *stmt,
    const mtext   *name
);

/**
 * @brief
 * Register an unsigned big integer output bind placeholder
 *
 * @param stmt - Statement handle
 * @param name - Output bind name
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_RegisterUnsignedBigInt
(
    OCI_Statement *stmt,
    const mtext   *name
);

/**
 * @brief
 * Register a string output bind placeholder
 *
 * @param stmt - Statement handle
 * @param name - Output bind name
 * @param len  - Max length of single string (in characters)
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_RegisterString
(
    OCI_Statement *stmt,
    const mtext   *name,
    unsigned int   len
);

/**
 * @brief
 * Register an raw output bind placeholder
 *
 * @param stmt - Statement handle
 * @param name - Output bind name
 * @param len  - Max length of the buffer (in bytes)
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_EXPORT boolean OCI_API OCI_RegisterRaw
(
    OCI_Statement *stmt,
    const mtext   *name,
    unsigned int   len
);

/**
 * @brief
 * Register a double output bind placeholder
 *
 * @param stmt - Statement handle
 * @param name - Output bind name
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_EXPORT boolean OCI_API OCI_RegisterDouble
(
    OCI_Statement *stmt,
    const mtext   *name
);

/**
 * @brief
 * Register a date output bind placeholder
 *
 * @param stmt - Statement handle
 * @param name - Output bind name
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_EXPORT boolean OCI_API OCI_RegisterDate
(
    OCI_Statement *stmt,
    const mtext   *name
);

/**
 * @brief
 * Register a timestamp output bind placeholder
 *
 * @param stmt - Statement handle
 * @param name - Output bind name
 * @param type - Timestamp type
 *
 * @note
 * See OCI_TimestampCreate() for possible values of parameter 'type'
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_EXPORT boolean OCI_API OCI_RegisterTimestamp
(
    OCI_Statement *stmt,
    const mtext   *name,
    unsigned int   type
);

/**
 * @brief
 * Register an interval output bind placeholder
 *
 * @param stmt - Statement handle
 * @param name - Output bind name
 * @param type - Interval type
 *
 * @note
 * See OCI_IntervalCreate() for possible values of parameter 'type'
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_EXPORT boolean OCI_API OCI_RegisterInterval
(
    OCI_Statement *stmt,
    const mtext   *name,
    unsigned int   type
);

/**
 * @brief
 * Register an object output bind placeholder
 *
 * @param stmt   - Statement handle
 * @param name   - Output bind name
 * @param typinf - Type info handle
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_EXPORT boolean OCI_API OCI_RegisterObject
(
    OCI_Statement *stmt,
    const mtext   *name,
    OCI_TypeInfo  *typinf
);

/**
 * @brief
 * Register a lob output bind placeholder
 *
 * @param stmt - Statement handle
 * @param name - Output bind name
 * @param type - Lob type
 *
 * @note
 * See OCI_LobCreate() for possible values of parameter 'type'
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_EXPORT boolean OCI_API OCI_RegisterLob
(
    OCI_Statement *stmt,
    const mtext   *name,
    unsigned int   type
);

/**
 * @brief
 * Register a file output bind placeholder
 *
 * @param stmt - Statement handle
 * @param name - Output bind name
 * @param type - File type
 *
 * @note
 * See OCI_FileCreate() for possible values of parameter 'type'
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_EXPORT boolean OCI_API OCI_RegisterFile
(
    OCI_Statement *stmt,
    const mtext   *name,
    unsigned int   type
);

/**
 * @brief
 * Register a Ref output bind placeholder
 *
 * @param stmt   - Statement handle
 * @param name   - Output bind name
 * @param typinf - Type info handle
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_EXPORT boolean OCI_API OCI_RegisterRef
(
    OCI_Statement *stmt,
    const mtext   *name,
    OCI_TypeInfo  *typinf
);

/**
 * @}
 */

/**
 * @defgroup g_rowid Oracle Rowids
 * @{
 *
 * OCILIB supports the Oracle ROWID type through C scalar string types (dtext).
 *
 * - ROWIDs can be retrieved from resultset with OCI_GetString()
 * - ROWIDs can be binded to statements with OCI_BindString()
 *
 * The maximum size of an ROWID buffer is defined by the constant OCI_SIZE_ROWID
 *
 * @par Example
 * @include rowid.c
 *
 * @}
 */

/**
 * @defgroup g_stmt Statements control
 * @{
 *
 * Those functions give extra information about OCILIB statements and can modify their behaviour.
 *
 */

/**
 * @brief
 * Return the type of a SQL statement
 *
 * @param stmt - Statement handle
 *
 * @note
 * Possible values are :
 *
 * - OCI_CST_SELECT  : select statement
 * - OCI_CST_UPDATE  : update statement
 * - OCI_CST_DELETE  : delete statement
 * - OCI_CST_INSERT  : insert statement
 * - OCI_CST_CREATE  : create statement
 * - OCI_CST_DROP    : drop statement
 * - OCI_CST_ALTER   : alter statement
 * - OCI_CST_BEGIN   : begin (pl/sql) statement
 * - OCI_CST_DECLARE : declare (pl/sql) statement
 *
 * @return
 * The statement type on success or OCI_UNKOWN on error
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_GetStatementType
(
    OCI_Statement *stmt
);

/**
 * @brief
 * Set the fetch mode of a SQL statement
 *
 * @param stmt - Statement handle
 * @param mode - fetch mode value
 *
 * @warning
 * OCI_SetFetchMode() MUST be called before anyOCI_ExecuteXXX() call
 *
 * @note
 * Possible values are :
 *  - OCI_SFM_DEFAULT
 *  - OCI_SFM_SCROLLABLE
 *
 */

OCI_EXPORT boolean OCI_API OCI_SetFetchMode
(
    OCI_Statement *stmt,
    unsigned int   mode
);

/**
 * @brief
 * Return the fetch mode of a SQL statement
 *
 * @param stmt - Statement handle
 *
 * @note
 * See OCI_SetFetchMode() for possible values
 * Default value is OCI_FETCH_FORWARD_ONLY
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_GetFetchMode
(
    OCI_Statement *stmt
);

/**
 * @brief
 * Set the binding mode of a SQL statement
 *
 * @param stmt - Statement handle
 * @param mode - binding mode value
 *
 * @note
 * Possible values are :
 *  - OCI_BIND_BY_POS  : position binding
 *  - OCI_BIND_BY_NAME : name binding
 *
 */

OCI_EXPORT boolean OCI_API OCI_SetBindMode
(
    OCI_Statement *stmt,
    unsigned int   mode
);

/**
 * @brief
 * Return the binding mode of a SQL statement
 *
 * @param stmt - Statement handle
 *
 * @note
 * See OCI_SetBindMode() for possible values
 * Default value is OCI_BIND_BY_NAME
 *
 * @note
 * if stmt is NULL, the return value is OCI_UNKNOWN
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_GetBindMode
(
    OCI_Statement *stmt
);

/**
 * @brief
 * Set the bind allocation mode of a SQL statement
 *
 * @param stmt - Statement handle
 * @param mode - bind allocation mode value
 *
 * @note
 * Possible values are :
 *  - OCI_BAM_EXTERNAL : bind variable are allocated by user code
 *  - OCI_BAM_INTERNAL : bind variable are allocated internally
 *
 */

OCI_EXPORT boolean OCI_API OCI_SetBindAllocation
(
    OCI_Statement *stmt,
    unsigned int   mode
);

/**
 * @brief
 * Return the bind allocation mode of a SQL statement
 *
 * @param stmt - Statement handle
 *
 * @note
 * See OCI_SetBindAllocation() for possible values
 * Default value is OCI_BAM_EXTERNAL
 *
 * @note
 * if stmt is NULL, the return value is OCI_UNKNOWN
 *
 */
OCI_EXPORT unsigned int OCI_API OCI_GetBindAllocation
(
    OCI_Statement *stmt
);

/**
 * @brief
 * Set the number of rows fetched per internal server fetch call
 *
 * @param stmt - Statement handle
 * @param size - number of rows to fetch
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_SetFetchSize
(
    OCI_Statement *stmt,
    unsigned int   size
);

/**
 * @brief
 * Return the number of rows fetched per internal server fetch call
 *
 * @note
 * Default value is set to constant OCI_FETCH_SIZE
 *
 * @param stmt - Statement handle
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_GetFetchSize
(
    OCI_Statement *stmt
);

/**
 * @brief
 * Set the number of rows pre-fetched by OCI Client
 *
 * @param stmt - Statement handle
 * @param size - number of rows to pre-fetch
 *
 * @note
 * To turn off pre-fetching, set both attributes (size and memory) to 0.
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_SetPrefetchSize
(
    OCI_Statement *stmt,
    unsigned int   size
);

/**
 * @brief
 * Return the number of rows pre-fetched by OCI Client
 *
 * @note
 * Default value is set to constant OCI_PREFETCH_SIZE
 *
 * @param stmt - Statement handle
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_GetPrefetchSize
(
    OCI_Statement *stmt
);

/**
 * @brief
 * Set the number of rows pre-fetched by OCI Client
 *
 * @param stmt - Statement handle
 * @param size - amount of memory to fetch
 *
 * @note
 * Default value is 0 and the pre-fetch size attribute is used instead.
 * When both attributes are set (pre-fetch size and memory) and pre-fetch memory
 * value can hold more rows than specified by pre-fetch size, OCI uses pre-fetch
 * size instead.
 *
 * @note
 * OCILIB set pre-fetch attribute to OCI_PREFETCH_SIZE when a statement is created.
 * To setup a big value for OCI_SetPrefetchMemory(), you must call
 * OCI_SetPrefetchSize() to 0 to make OCI consider this attribute.
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_SetPrefetchMemory
(
    OCI_Statement *stmt,
    unsigned int   size
);

/**
 * @brief
 * Return the amount of memory used to retrieve rows pre-fetched by OCI Client
 *
 * @param stmt - Statement handle
 *
 * @note
 * Default value is 0
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_GetPrefetchMemory
(
    OCI_Statement *stmt
);

/**
 * @brief
 * Set the LONG datatype piece buffer size
 *
 * @param stmt - Statement handle
 * @param size - maximum size for long buffer
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_SetLongMaxSize
(
    OCI_Statement *stmt,
    unsigned int   size
);

/**
 * @brief
 * Return the LONG datatype piece buffer size
 *
 * @note
 * Default value is set to constant OCI_SIZE_LONG
 *
 * @param stmt - Statement handle
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_GetLongMaxSize
(
    OCI_Statement *stmt
);

/**
 * @brief
 * Set the long datatype handling mode of a SQL statement
 *
 * @param stmt - Statement handle
 * @param mode - binding mode value
 *
 * @note
 * Possible values are :
 *
 * - OCI_LONG_EXPLICIT : LONGs are explicitly handled by OCI_Long type
 * - OCI_LONG_IMPLICIT : LONGs are implicitly mapped to string type in the
 *   limits of VARCHAR2 size capacity
 *
 *  LONG RAWs can't be handled with OCI_LONG_IMPLICIT
 */

OCI_EXPORT boolean OCI_API OCI_SetLongMode
(
    OCI_Statement *stmt,
    unsigned int   mode
);

/**
 * @brief
 * Return the long datatype handling mode of a SQL statement
 *
 * @param stmt - Statement handle
 *
 * @note
 *  See OCI_SetLongMode() for possible values
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_GetLongMode
(
    OCI_Statement *stmt
);

/**
 * @brief
 * Return the connection handle associated with a statement handle
 *
 * @param stmt - Statement handle
 *
 */

OCI_EXPORT OCI_Connection * OCI_API OCI_StatementGetConnection
(
    OCI_Statement *stmt
);

/**
 * @}
 */

/**
 * @defgroup g_lob Internal Large Objects (LOBs)
 * @{
 *
 * Large Objects (LOBs) were introduced with Oracle 8i to replace LONGs
 *
 * Oracle OCI supplies a set APIs to manipulate this datatype.
 *
 * OCILIB encapsulates this API by supplying:
 *
 * - An OCI_Lob C type
 * - A set of really easy APIs to manipulate OCI_Lob objects
 *
 * OCILIB currently supports 3 types of Lobs :
 *
 * - BLOB  : Binary LOBs (replacement for LONG RAW datatype)
 * - CLOB  : Character LOBs (replacement for LONG datatype)
 * - NCLOB : National Character LOBs
 *
 * OCI_Lob objects can be :
 *
 * - Created as standalone instances
 * - Used for in/out binding
 * - Retrieved from select statements
 * - Manipulated (copy, append, ...)
 *
 * @par Lobs > 4 Go
 *
 * Oracle 10g extended lobs by increasing maximum size from 4Go to 128 To.
 *
 * OCILIB, with version 2.1.0, supports now this new limit.
 * For handling sizes and offsets up to 128 To, 64 bit integers are requested.
 *
 * So, A new scalar integer type has been introduced: big_uint (elderly lobsize_t).
 * This type can be a 32 bits or 64 bits integer depending on :
 * - Compiler support for 64 bits integers (C99 compiler, MS compilers)
 * - Oracle client version
 *
 * big_uint will be a 64 bits integer :
 * - if the compiler supports it
 * - if OCILIB is build with option OCI_IMPORT_LINKAGE and the Oracle version is >= 10.1
 * - or OCILIB is build with option OCI_IMPORT_RUNTIME (oracle version is not known at
 *   compilation stage)
 *
 * @par Example
 * @include lob.c
 *
 */

/**
 * @brief
 * Create a local temporary Lob instance
 *
 * @param con  - Connection handle
 * @param type - Lob type
 *
 * Supported lob types :
 *
 * - OCI_BLOB  : Binary Lob
 * - OCI_CLOB  : Character Lob
 * - OCI_NCLOB ! National Character Lob
 *
 * @return
 * Return the lob handle on success otherwise NULL on failure
 *
 */

OCI_EXPORT OCI_Lob * OCI_API OCI_LobCreate
(
    OCI_Connection *con,
    unsigned int    type
);

/**
 * @brief
 * Free a local temporary lob
 *
 * @param lob - Lob handle
 *
 * @warning
 * Only lobs created with OCI_LobCreate() should be freed by OCI_LobFree()
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_LobFree
(
    OCI_Lob *lob
);

/**
 * @brief
 * Create an array of lob object
 *
 * @param con    - Connection handle
 * @param type   - Lob type
 * @param nbelem - number of elements in the array
 *
 * @note
 * see OCI_LobCreate() for more details
 *
 * @return
 * Return the lob handle array on success otherwise NULL on failure
 *
 */

OCI_EXPORT OCI_Lob ** OCI_API OCI_LobArrayCreate
(
    OCI_Connection *con,
    unsigned int    type,
    unsigned int    nbelem
);

/**
* @brief
* Free an arrray of lob objects
*
* @param lobs - Array of lob objects
*
* @warning
* Only arrays of lobs created with OCI_LobArrayCreate() should be freed
* by OCI_LobArrayFree()
*
* @return
* TRUE on success otherwise FALSE
*
*/

OCI_EXPORT boolean OCI_API OCI_LobArrayFree
(
    OCI_Lob **lobs
);

/**
 * @brief
 * Return the type of the given Lob object
 *
 * @param lob - Lob handle
 *
 * @note
 * For possible values, see OCI_LobCreate()
 *
 * @return
 * Object type or OCI_UNKNOWN the input handle is NULL
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_LobGetType
(
    OCI_Lob *lob
);

/**
 * @brief
 * Perform a seek operation on the OCI_lob content buffer
 *
 * @param lob    - Lob handle
 * @param offset - Offset from current position (bytes or characters)
 * @param mode   - Seek mode
 *
 * Parameter 'mode' can be one of the following value :
 *
 * - OCI_SEEK_SET : set the lob current offset to the given absolute offset
 * - OCI_SEEK_END : set the lob current offset to the end of the lob
 * - OCI_SEEK_CUR : move the lob current offset to the number of bytes or
 *                  characters given by parameter 'offset'
 *
 * @note
 * - For CLOB and CLOB, offset in in characters
 * - For BLOB and BFILE, offset is in bytes
 *
 * @note
 * Position in the Lob buffer starts at 0.
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_LobSeek
(
    OCI_Lob     *lob,
    big_uint     offset,
    unsigned int mode
);

/**
 * @brief
 * Return the current position in the Lob content buffer
 *
 * @param lob - Lob handle
 *
 * @return
 * Lob position (starting with 0) or 0 on failure
 */

OCI_EXPORT big_uint OCI_API OCI_LobGetOffset
(
    OCI_Lob *lob
);

/**
 * @brief
 * [OBSOLETE] Read a portion of a lob into the given buffer
 *
 * @param lob    - Lob handle
 * @param buffer - Pointer to a buffer
 * @param len    - Length of the buffer (in bytes or characters)
 *
 * @note
 * Length is expressed in :
 * - Bytes for BLOBs
 * - Characters for CLOBs/NCLOBS
 *
 * @warning
 * This call is obsolete ! Use OCI_LobRead2() instead.
 *
 * @return
 * Number of bytes/characters read on success otherwise 0 on failure
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_LobRead
(
    OCI_Lob     *lob,
    void        *buffer,
    unsigned int len
);

/**
 * @brief
 * Read a portion of a lob into the given buffer
 *
 * @param lob        - Lob handle
 * @param buffer     - Pointer to a buffer
 * @param char_count - [in/out] Pointer to maximum number of characters
 * @param byte_count - [in/out] Pointer to maximum number of bytes
 *
 * @note
 * In input,  'char_count' and 'byte_count' are values to read into the buffer
 * In output, 'char_count' and 'byte_count' are values read into the buffer
 *
 * @note
 * For BLOBs, only the parameter 'byte_count' is used
 * For CLOBs, both parameters can be used :
 * In input :
 *  - if 'byte_count' is set to zero, it is computed from 'char_count'
 *  - if 'char_count' is set to zero, it is computed from 'byte_count'
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_LobRead2
(
    OCI_Lob      *lob,
    void         *buffer,
    unsigned int *char_count,
    unsigned int *byte_count
);

/**
 * @brief
 * [OBSOLETE] Write a buffer into a LOB
 *
 * @param lob    - Lob handle
 * @param buffer - Pointer to a buffer
 * @param len    - Length of the buffer (in bytes or characters)
 *
 * @note
 * Length is expressed in :
 * - Bytes for BLOBs
 * - Characters for CLOBs/NCLOBs
 *
 * @warning
 * This call is obsolete ! Use OCI_LobWrite2() instead.
 *
 * @return
 * Number of bytes / characters written on success otherwise 0 on failure
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_LobWrite
(
    OCI_Lob     *lob,
    void        *buffer,
    unsigned int len
);

/**
 * @brief
 * Write a buffer into a LOB
 *
 * @param lob        - Lob handle
 * @param buffer     - Pointer to a buffer
 * @param char_count - [in/out] Pointer to maximum number of characters
 * @param byte_count - [in/out] Pointer to maximum number of bytes
 *
 * @note
 * In input,  'char_count' and 'byte_count' are values to write from the buffer
 * In output, 'char_count' and 'byte_count' are values written from the buffer
 *
 * @note
 * For BLOBs, only the parameter 'byte_count' is used
 * For CLOBs, both parameters can be used :
 * In input :
 *  - if 'byte_count' is set to zero, it is computed from 'char_count'
 *  - if 'char_count' is set to zero, it is computed from 'byte_count'
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_LobWrite2
(
    OCI_Lob      *lob,
    void         *buffer,
    unsigned int *char_count,
    unsigned int *byte_count
);

/**
 * @brief
 * Truncate the given lob to a shorter length
 *
 * @param lob  - Lob handle
 * @param size - New length (in bytes or characters)
 *
 * @note
 * Length is expressed in :
 * - Bytes for BLOBs
 * - Characters for CLOBs/NCLOBs
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_LobTruncate
(
    OCI_Lob *lob,
    big_uint size
);

/**
 * @brief
 * Return the actual length of a lob
 *
 * @param lob - Lob handle
 *
 * @note
 * The returned value is in bytes for BLOBS and characters for CLOBS/NCLOBs
 *
 */

OCI_EXPORT big_uint OCI_API OCI_LobGetLength
(
    OCI_Lob *lob
);

/**
 * @brief
 * Returns the chunk size of a LOB
 *
 * @param lob - Lob handle
 *
 * @note
 * This chunk size corresponds to the chunk size used by the LOB data layer
 * when accessing and modifying the LOB value. According to Oracle
 * documentation, performance will be improved if the application issues
 * read or write requests using a multiple of this chunk size
 *
 * @note
 * The returned value is in bytes for BLOBS and characters for CLOBS/NCLOBs
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_LobGetChunkSize
(
    OCI_Lob *lob
);

/**
 * @brief
 * Erase a portion of the lob at a given position
 *
 * @param lob    - Lob handle
 * @param offset - Absolute position in source lob
 * @param len    - Number of bytes or characters to erase
 *
 * @note
 * Absolute position starts at 1.
 *
 * @return
 * Number of bytes (BLOB) or characters (CLOB/NCLOB) erased on success
 * otherwise 0 on failure
 *
 */

OCI_EXPORT big_uint OCI_API OCI_LobErase
(
    OCI_Lob *lob,
    big_uint offset,
    big_uint len
);

/**
 * @brief
 * Append a buffer at the end of a LOB
 *
 * @param lob    - Lob handle
 * @param buffer - Pointer to a buffer
 * @param len    - Length of the buffer (in bytes or characters)
 *
 * @note
 * Length is expressed in :
 * - Bytes for BLOBs
 * - Characters for CLOBs
 *
 * @return
 * Number of bytes / characters written on success otherwise 0 on failure
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_LobAppend
(
    OCI_Lob     *lob,
    void        *buffer,
    unsigned int len
);

/**
 * @brief
 * Append a buffer at the end of a LOB
 *
 * @param lob        - Lob handle
 * @param buffer     - Pointer to a buffer
 * @param char_count - [in/out] Pointer to maximum number of characters
 * @param byte_count - [in/out] Pointer to maximum number of bytes
 *
 * @note
 * In input,  'char_count' and 'byte_count' are values to write from the buffer
 * In output, 'char_count' and 'byte_count' are values written from the buffer
 *
 * @note
 * For BLOBs, only the parameter 'byte_count' is used
 * For CLOBs, both parameters can be used :
 * In input :
 *  - if 'byte_count' is set to zero, it is computed from 'char_count'
 *  - if 'char_count' is set to zero, it is computed from 'byte_count'
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_LobAppend2
(
    OCI_Lob      *lob,
    void         *buffer,
    unsigned int *char_count,
    unsigned int *byte_count
);

/**
 * @brief
 * Append a source LOB at the end of a destination LOB
 *
 * @param lob        - Destination Lob handle
 * @param lob_src    - Source Lob handle
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_LobAppendLob
(
    OCI_Lob *lob,
    OCI_Lob *lob_src
);

/**
 * @brief
 * Check if the given lob is a temporary lob
 *
 * @param lob - Lob handle
 *
 * @return
 * TRUE if it's a temporary lob otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_LobIsTemporary
(
    OCI_Lob *lob
);

/**
 * @brief
 * Copy a portion of a source LOB into a destination LOB
 *
 * @param lob        - Destination Lob handle
 * @param lob_src    - Source Lob handle
 * @param offset_dst - Absolute position in destination lob
 * @param offset_src - Absolute position in source lob
 * @param count      - Number of bytes or character to copy
 *
 * @note
 * For character LOB (CLOB/NCLOBS) the parameters count, offset_dst and
 * offset_src are expressed in characters and not in bytes.
 *
 * @note
 * Absolute position starts at 1.
 *
 */

OCI_EXPORT boolean OCI_API OCI_LobCopy
(
    OCI_Lob *lob,
    OCI_Lob *lob_src,
    big_uint offset_dst,
    big_uint offset_src,
    big_uint count
);

/**
 * @brief
 * Copy a portion of a source FILE into a destination LOB
 *
 * @param lob        - Destination Lob handle
 * @param file       - Source File handle
 * @param offset_dst - Absolute position in destination lob
 * @param offset_src - Absolute position in source file
 * @param count      - Number of bytes to copy
 *
 * @note
 * - For character LOB (CLOB/NCLOB) the parameter offset_src are expressed in
 *   characters and not in bytes.
 * - Offset_src is always in bytes
 *
 * @note
 * Absolute position starts at 1.
 *
 */

OCI_EXPORT boolean OCI_API OCI_LobCopyFromFile
(
    OCI_Lob  *lob,
    OCI_File *file,
    big_uint  offset_dst,
    big_uint  offset_src,
    big_uint  count
);

/**
 * @brief
 * Open explicitly a Lob
 *
 * @param lob  - Lob handle
 * @param mode - open mode
 *
 * Possible values for mode are :
 *
 * - OCI_LOB_READONLY  : read onluy access
 * - OCI_LOB_READWRITE : read/write access
 *
 * @note
 * - A call to OCI_LobOpen is not necessary to manipulate a Lob.
 * - If a lob hasn't been opened explicitly, triggers are fired and
 *   indexes updated at every read/write/append operation
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_LobOpen
(
    OCI_Lob     *lob,
    unsigned int mode
);

/**
 * @brief
 * Close explicitly a Lob
 *
 * @param lob - Lob handle
 *
 * @note
 * - A call to OCI_LobClose is not necessary to manipulate a Lob.
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_LobClose
(
    OCI_Lob *lob
);

/**
 * @brief
 * Compare two lob handles for equality
 *
 * @param lob  - Lob handle
 * @param lob2 - Lob2 handle
 *
 * @note
 *
 * @return
 * TRUE is the lobs are not null and equal otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_LobIsEqual
(
    OCI_Lob *lob,
    OCI_Lob *lob2
);

/**
 * @brief
 * Assign a lob to another one
 *
 * @param lob     - Destination Lob handle
 * @param lob_src - Source Lob handle
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_LobAssign
(
    OCI_Lob *lob,
    OCI_Lob *lob_src
);

/**
 * @brief
 * Return the maximum size that the lob can contain
 *
 * @param lob - Lob handle
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT big_uint OCI_API OCI_LobGetMaxSize
(
    OCI_Lob *lob
);

/**
 * @brief
 * Flush Lob content to the server
 *
 * @param lob   - Lob handle
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_LobFlush
(
    OCI_Lob *lob
);

/**
 * @brief
 * Enable / disable buffering mode on the given lob handle
 *
 * @param lob    - Lob handle
 * @param value  - Enable/disable buffering mode
 *
 * @note
 * Oracle "LOB Buffering Subsystem" allows client applications
 * to speedup read/write of small buffers on Lobs Objects.
 * Check Oracle Documentation for more details on "LOB Buffering Subsystem".
 * This reduces the number of network round trips and LOB versions, thereby
 * improving LOB performance significantly.
 *
 * @warning
 * According to Oracle documentation the following operations are not permitted
 * on Lobs when buffering is on : OCI_LobCopy(), OCI_LobAppend, OCI_LobErase(),
 * OCI_LobGetLength(), OCI_LobTrim()
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_LobEnableBuffering
(
    OCI_Lob *lob,
    boolean  value
);

/**
 * @}
 */

/**
 * @defgroup g_file External Large Objects (FILEs)
 * @{
 *
 * External Large Objects (FILEs) were introduced with Oracle 8i
 *
 * Oracle OCI supplies a set APIs to manipulate this datatype.
 *
 * OCILIB encapsulates this API by supplying:
 *
 * - An OCI_File C type
 * - A set of really easy APIs to manipulate OCI_File objects
 *
 * OCILIB currently supports 2 types of Lobs :
 *
 * - BFILE : Binary files
 * - CFILE : Character files
 *
 * @warning
 * FILEs are read-only.
 *
 * OCI_Lob objects can be :
 *
 * - Created as standalone instances
 * - Used for in/out binding
 * - Retrieved from select statements
 * - Used for reading server files content
 *
 * @par Files > 4 Go
 *
 * - New maximum file size limit (128 To) applies to OCI_Files objects.
 * - See Internal Large Objects (LOBs) section for Files > 4 Go information
 *
 * @par Example
 * @include file.c
 *
 */

/**
 * @brief
 * Create a file object instance
 *
 * @param con  - Connection handle
 * @param type - File type
 *
 * Supported file types :
 *
 * - OCI_BFILE : Binary file
 * - OCI_CFILE : Character file
 *
 * @return
 * Return the lob handle on success otherwise NULL on failure
 *
 */

OCI_EXPORT OCI_File * OCI_API OCI_FileCreate
(
    OCI_Connection *con,
    unsigned int    type
);

/**
 * @brief
 * Free a local File object
 *
 * @param file - File handle
 *
 * @warning
 * Only Files created with OCI_FileCreate() should be freed by OCI_FileFree()
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_FileFree
(
    OCI_File *file
);

/**
 * @brief
 * Create an array of file object
 *
 * @param con    - Connection handle
 * @param type   - File type
 * @param nbelem - number of elements in the array
 *
 * @note
 * see OCI_FileCreate() for more details
 *
 * @return
 * Return the file handle array on success otherwise NULL on failure
 *
 */

OCI_EXPORT OCI_File ** OCI_API OCI_FileArrayCreate
(
    OCI_Connection *con,
    unsigned int    type,
    unsigned int    nbelem
);

/**
* @brief
* Free an array of file objects
*
* @param files - Array of file objects
*
* @warning
* Only arrays of lobs created with OCI_FileArrayCreate() should be freed by OCI_FileArrayFree()
*
* @return
* TRUE on success otherwise FALSE
*
*/

OCI_EXPORT boolean OCI_API OCI_FileArrayFree
(
    OCI_File **files
);

/**
 * @brief
 * Return the type of the given File object
 *
 * @param file - File handle
 *
 * @note
 * For possible values, see OCI_FileCreate()
 *
 * @return
 * Object type or OCI_UNKNOWN the input handle is NULL
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_FileGetType
(
    OCI_File *file
);

/**
 * @brief
 * Perform a seek operation on the OCI_File content buffer
 *
 * @param file    - File handle
 * @param offset  - Offset from current position
 * @param mode    - Seek mode
 *
 * Mode parameter can be one of the following value :
 *
 * - OCI_SEEK_SET : set the file current offset to the given absolute offset
 * - OCI_SEEK_END : set the file current offset to the end of the lob
 * - OCI_SEEK_CUR : move the file current offset to the number of bytes given by
 *                  parameter 'offset'
 *
 * @note
 * Position in the File buffer starts at 0.
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_FileSeek
(
    OCI_File    *file,
    big_uint     offset,
    unsigned int mode
);

/**
 * @brief
 * Return the current position in the file
 *
 * @param file - File handle
 *
 * @return
 * File position (starting with 0) or 0 on failure
 */

OCI_EXPORT big_uint OCI_API OCI_FileGetOffset
(
    OCI_File *file
);

/**
 * @brief
 * Read a portion of a file into the given buffer
 *
 * @param file   - File handle
 * @param buffer - Pointer to a buffer
 * @param len    - Length of the buffer in bytes
 *
 * @return
 * Number of bytes read on success otherwise 0 on failure
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_FileRead
(
    OCI_File    *file,
    void        *buffer,
    unsigned int len
);

/**
 * @brief
 * Return the size in bytes of a file
 *
 * @param file - File handle
 *
 */

OCI_EXPORT big_uint OCI_API OCI_FileGetSize
(
    OCI_File *file
);

/**
 * @brief
 * Check if the given file exists on server
 *
 * @param file - File handle
 *
 * @note
 * For local FILEs object, OCI_LobFileSetName() must be called before to set the filename to check
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_FileExists
(
    OCI_File *file
);

/**
 * @brief
 * Set the directory and file name of FILE handle
 *
 * @param file  - File handle
 * @param dir   - File directory
 * @param name  - File name
 *
 * @note
 * - For local FILEs only
 * - Files fetched from resultset can't be assigned a new directory and name
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_FileSetName
(
    OCI_File    *file,
    const mtext *dir,
    const mtext *name
);

/**
 * @brief
 * Return the directory of the given file
 *
 * @param file  - File handle
 *
 */

OCI_EXPORT const mtext * OCI_API OCI_FileGetDirectory
(
    OCI_File *file
);

/**
 * @brief
 * Return the name of the given file
 *
 * @param file  - File handle
 *
 */

OCI_EXPORT const mtext * OCI_API OCI_FileGetName
(
    OCI_File *file
);

/**
 * @brief
 * Open a file for reading
 *
 * @param file  - File handle
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_FileOpen
(
    OCI_File *file
);

/**
 * @brief
 * Check if the specified file is opened within the file handle
 *
 * @param file  - File handle
 *
 * @return
 * TRUE if the file was opened with this handle otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_FileIsOpen
(
    OCI_File *file
);

/**
 * @brief
 * Close a file
 *
 * @param file  - File handle
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_FileClose
(
    OCI_File *file
);

/**
 * @brief
 * Compare two file handle for equality
 *
 * @param file  - File handle
 * @param file2 - File2 handle
 *
 * @note
 *
 * @return
 * TRUE is the lobs are not null and equal otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_FileIsEqual
(
    OCI_File *file,
    OCI_File *file2
);

/**
 * @brief
 * Assign a file to another one
 *
 * @param file     - Destination File handle
 * @param file_src - Source File handle
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_FileAssign
(
    OCI_File *file,
    OCI_File *file_src
);

/**
 * @}
 */

/**
 * @defgroup g_long Long objects
 * @{
 *
 * Long Objects encapsulate Oracle LONGs datatypes and were used to store large
 * buffers in Oracle database.
 *
 * They're still supported but are depreciated. Oracle now provides a
 * newer and better way to deal with data that needs large storage : LOBs
 *
 * OCILIB supports this datatype because it was and still is widely used
 *
 * OCILIB provides a set of API for manipulating LONGs that is really close to
 * the one provided for LOBs.
 *
 * OCILIB currently supports 3 types of Long Objects:
 *
 * - OCI_BLONG : LONG RAW columns
 * - OCI_CLONG : LONG columns
 *
 * OCI_Lob objects can be :
 *
 * - Created as standalone instances
 * - Used for in/out binding
 * - Retrieved from select statement
 *
 * @par Example
 * @include long.c
 *
 */

/**
 * @brief
 * Create a local temporary Long instance
 *
 * @param stmt - Statement handle
 * @param type - Long type
 *
 * Supported lob types :
 *
 * - OCI_BLONG : Binary Long
 * - OCI_CLONG : Character Long
 *
 * @return
 * Return the long handle on success otherwise NULL on failure
 *
 */

OCI_EXPORT OCI_Long * OCI_API OCI_LongCreate
(
    OCI_Statement *stmt,
    unsigned int   type
);

/**
 * @brief
 * Free a local temporary long
 *
 * @param lg - Long handle
 *
 * @warning
 * Only lobs created with OCI_LongCreate() should be freed by OCI_LongFree()
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_LongFree
(
    OCI_Long *lg
);

/**
 * @brief
 * Return the type of the given Long object
 *
 * @param lg - Long handle
 *
 * @note
 * For possible values, see OCI_LobCreate()
 *
 * @return
 * Object type or OCI_UNKNOWN the input handle is NULL
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_LongGetType
(
    OCI_Long *lg
);

/**
 * @brief
 * Read a portion of a long into the given buffer [Obsolete]
 *
 * @param lg     - Long handle
 * @param buffer - Pointer to a buffer
 * @param len    - Length of the buffer in bytes / characters
 *
 * @note
 * - From version 2.0.0, this function is obsolete because OCILIB fetches now
 *   all data during OCIFetchNext() call
 * - So, this call reads now the internal OCI_Long object allocated buffer
 * - The internal buffer can be directly accessed with OCI_LongGetBuffer()
 *
 * @note
 * - For OCI_CLONG, parameter 'len' and returned value are expressed in characters
 * - For OCI_BLONG, parameter 'len' and returned value are expressed in bytes
 *
 * @return
 * - Number of bytes/characters read on success
 * - 0 if there is nothing more to read
 * - 0 on failure
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_LongRead
(
    OCI_Long    *lg,
    void        *buffer,
    unsigned int len
);

/**
 * @brief
 * Write a buffer into a Long
 *
 * @param lg     - Long handle
 * @param buffer - the pointer to a buffer
 * @param len    - the length of the buffer in bytes (OCI_BLONG) or
 *                  character (OCI_CLONG)
 *
 * @return
 * Number of bytes (OCI_BLONG) / character (OCI_CLONG) written on success
 * - 0 if there is nothing more to read
 * - 0 on failure
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_LongWrite
(
    OCI_Long    *lg,
    void        *buffer,
    unsigned int len
);

/**
 * @brief
 * Return the buffer size of a long object in bytes (OCI_BLONG) or character (OCI_CLONG)
 *
 * @param lg - Long handle
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_LongGetSize
(
    OCI_Long *lg
);

/**
 * @brief
 * Return the internal buffer of an OCI_Long object read from a fetch sequence
 *
 * @param lg - Long handle
 *
 */

OCI_EXPORT void * OCI_API OCI_LongGetBuffer
(
    OCI_Long *lg
);

/**
 * @}
 */

/**
 * @defgroup g_date Date/time manipulation
 * @{
 *
 * OCILIB encapsulates Oracle SQL Date datatype within OCI_Date structure
 *
 * Basically, the OCI_Date routines are wrappers around the Oracle OCIDate APIs
 *
 * @par Example
 * @include date.c
 *
 */

/**
 * @brief
 * Create a local date object
 *
 * @param con - Connection handle
 *
 * @note
 * From version 2.5.0, parameter 'con' can be NULL in order to manipulate dates
 * independently from database connections
 *
 * @return
 * Return the date handle on success otherwise NULL on failure
 *
 */

OCI_EXPORT OCI_Date * OCI_API OCI_DateCreate
(
    OCI_Connection *con
);

/**
 * @brief
 * Free a date object
 *
 * @param date - Date handle
 *
 * @warning
 * Only dates created with OCI_DateCreate() should be freed by OCI_DateFree()
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_DateFree
(
    OCI_Date *date
);

/**
 * @brief
 * Create an array of date object
 *
 * @param con    - Connection handle
 * @param nbelem - number of elements in the array
 *
 * @note
 * see OCI_DateCreate() for more details
 *
 * @return
 * Return the date handle array on success otherwise NULL on failure
 *
 */

OCI_EXPORT OCI_Date ** OCI_API OCI_DateArrayCreate
(
    OCI_Connection *con,
    unsigned int    nbelem
);

/**
 * @brief
 * Free an arrray of date objects
 *
 * @param dates - Array of date objects
 *
 * @warning
 * Only arrays of dates created with OCI_DateArrayCreate() should be freed by OCI_DateArrayFree()
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_DateArrayFree
(
    OCI_Date **dates
);

/**
 * @brief
 * Add or subtract days to a date handle
 *
 * @param date - Date handle
 * @param nb   - Number of days to add/remove
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_DateAddDays
(
    OCI_Date *date,
    int       nb
);

/**
 * @brief
 * Add or subtract months to a date handle
 *
 * @param date - Date handle
 * @param nb   - Number of months to add/remove
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_DateAddMonths
(
    OCI_Date *date,
    int       nb
);

/**
 * @brief
 * Assign the value of a date handle to another one
 *
 * @param date     - Destination Date handle
 * @param date_src - Source Date handle
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT int OCI_API OCI_DateAssign
(
    OCI_Date *date,
    OCI_Date *date_src
);

/**
 * @brief
 * Check if the given date is valid
 *
 * @param date - Date handle
 *
 * @return
 * - Zero if date is valid
 * - Any other value means the date is invalid
 *
 */

OCI_EXPORT int OCI_API OCI_DateCheck
(
    OCI_Date *date
);

/**
 * @brief
 * Compares two date handles
 *
 * @param date  - Date1 handle
 * @param date2 - Date2 handle
 *
 * @return
 * - -1 if date1 is smaller than date2,
 * -  0 if they are equal
 * -  1 if date1 is greater than date2.
 *
 */

OCI_EXPORT int OCI_API OCI_DateCompare
(
    OCI_Date *date,
    OCI_Date *date2
);

/**
 * @brief
 * Return the number of days betWeen two dates
 *
 * @param date  - Date1 handle
 * @param date2 - Date2 handle
 *
 * @return
 * Number of days on success otherwise OCI_ERROR on failure
 *
 */

OCI_EXPORT int OCI_API OCI_DateDaysBetween
(
    OCI_Date *date,
    OCI_Date *date2
);

/**
 * @brief
 * Convert a string to a date and store it in the given date handle
 *
 * @param date - Destination Date handle
 * @param str  - Source date string
 * @param fmt  - Date format
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_DateFromText
(
    OCI_Date    *date,
    const mtext *str,
    const mtext *fmt
);

/**
 * @brief
 * Convert a Date value from the given date handle to a string
 *
 * @param date - source Date handle
 * @param fmt  - Date format
 * @param size - Destination string size in characters
 * @param str  - Destination date string
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_DateToText
(
    OCI_Date    *date,
    const mtext *fmt,
    int          size,
    mtext       *str
);

/**
 * @brief
 * Extract the date part from a date handle
 *
 * @param date  - Date handle
 * @param year  - Place holder for year value
 * @param month - Place holder for month value
 * @param day   - Place holder for day value
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_DateGetDate
(
    OCI_Date *date,
    int      *year,
    int      *month,
    int      *day
);

/**
 * @brief
 * Extract the time part from a date handle
 *
 * @param date  - Date handle
 * @param hour  - Place holder for hour value
 * @param min   - Place holder for minute value
 * @param sec   - Place holder for second value
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_DateGetTime
(
    OCI_Date *date,
    int      *hour,
    int      *min,
    int      *sec
);

/**
 * @brief
 * Extract the date and time parts from a date handle
 *
 * @param date  - Date handle
 * @param year  - Place holder for year value
 * @param month - Place holder for month value
 * @param day   - Place holder for day value
 * @param hour  - Place holder for hour value
 * @param min   - Place holder for minute value
 * @param sec   - Place holder for second value
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_DateGetDateTime
(
    OCI_Date *date,
    int      *year,
    int      *month,
    int      *day,
    int      *hour,
    int      *min,
    int      *sec
);

/**
 * @brief
 * Set the date portion if the given date handle
 *
 * @param date  - Date handle
 * @param year  - Year value
 * @param month - Month value
 * @param day   - Day value
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_DateSetDate
(
    OCI_Date *date,
    int       year,
    int       month,
    int       day
);

/**
 * @brief
 * Set the time portion if the given date handle
 *
 * @param date  - Date handle
 * @param hour  - Hour value
 * @param min   - Minute value
 * @param sec   - Second value
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_DateSetTime
(
    OCI_Date *date,
    int       hour,
    int       min,
    int       sec
);

/**
 * @brief
 * Set the date and time portions if the given date handle
 *
 * @param date  - Date handle
 * @param year  - Year value
 * @param month - Month value
 * @param day   - Day value
 * @param hour  - Hour value
 * @param min   - Minute value
 * @param sec   - Second value
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_DateSetDateTime
(
    OCI_Date *date,
    int       year,
    int       month,
    int       day,
    int       hour,
    int       min,
    int       sec
);

/**
 * @brief
 * Place the last day of month (from the given date) into the given date
 *
 * @param date  - Date handle
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_DateLastDay
(
    OCI_Date *date
);

/**
 * @brief
 * Gets the date of next day of the week, after a given date
 *
 * @param date  - Date handle
 * @param day   - Day of the week
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_DateNextDay
(
    OCI_Date    *date,
    const mtext *day
);

/**
 * @brief
 * Return the current system date/time into the date handle
 *
 * @param date  - Date handle
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_DateSysDate
(
    OCI_Date *date
);

/**
 * @brief
 * Convert a date from one zone to another zone
 *
 * @param date  - Date handle
 * @param zone1 - Source zone
 * @param zone2 - Destination zone
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_DateZoneToZone
(
    OCI_Date    *date,
    const mtext *zone1,
    const mtext *zone2
);

/**
 * @brief
 * Affect an OCI_Date handle value to ISO C time datatypes
 *
 * @param date  - Date handle
 * @param ptm   - Pointer to a structure tm to receive date/time values
 * @param pt    - Pointer to a time_t to hold the date/time in the time_t format
 *
 * @note
 * Both parameters 'ptm' and 'p' are optional but one of them has to be provided.
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_DateToCTime
(
    OCI_Date  *date,
    struct tm *ptm,
    time_t    *pt
);

/**
 * @brief
 * Affect ISO C time datatypes values to an OCI_Date handle
 *
 * @param date  - Date handle
 * @param ptm   - Pointer to a structure tm that hold the date/time value
 * @param t     - Value (time_t) that hold the date/time in the time_t format
 *
 * @note
 *
 * Both parameters 'ptm' and 'p' are optional but one of them has to be provided.
 * If 'ptm' is not null, its value is affected to the OCI_Timestamp handle,
 * otherwise the value of 't' is used.
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_DateFromCTime
(
    OCI_Date  *date,
    struct tm *ptm,
    time_t     t
);

/**
 * @}
 */

/**
 * @defgroup g_timestamp Timestamps and intervals manipulation
 * @{
 *
 * OCILIB encapsulates Oracle  :
 *
 * - SQL timestamp datatype within OCI_Timestamp structure
 * - SQL interval  datatype within OCI_Interval  structure
 *
 * Basically, the OCI_Timestamp and OCI_Interval routines are wrappers around
 *  the Oracle OCIDatetime and OCIInterval APIs
 *
 * @par Examples
 * @include timestamp.c
 *
 */

/**
 * @brief
 * Create a local Timestamp instance
 *
 * @param con  - Connection handle
 * @param type - Timestamp type
 *
 * @note
 * From version 2.5.0, parameter 'con' can be NULL in order to manipulate
 * timestamps independently from database connections
 *
 * @note
 * Timestamp type can be :
 *
 * - OCI_TIMESTAMP     : timestamp
 * - OCI_TIMESTAMP_TZ  : timestamp with time zone
 * - OCI_TIMESTAMP_LTZ : timestamp with local time zone
 *
 * @return
 * Return the Timestamp handle on success otherwise NULL on failure
 *
 */

OCI_EXPORT OCI_Timestamp * OCI_API OCI_TimestampCreate
(
    OCI_Connection *con,
    unsigned int    type
);

/**
 * @brief
 * Free an OCI_Timestamp handle
 *
 * @param tmsp - Timestamp handle
 *
 * @warning
 * Only Timestamp created with OCI_TimestampCreate() should be freed by OCI_IntervalFree()
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_TimestampFree
(
    OCI_Timestamp *tmsp
);

/**
 * @brief
 * Create an array of timestamp object
 *
 * @param con    - Connection handle
 * @param type   - Timestamp type
 * @param nbelem - number of elements in the array
 *
 * @note
 * see OCI_TimestampCreate() for more details
 *
 * @return
 * Return the timestamp handle array on success otherwise NULL on failure
 *
 */

OCI_EXPORT OCI_Timestamp ** OCI_API OCI_TimestampArrayCreate
(
    OCI_Connection *con,
    unsigned int    type,
    unsigned int    nbelem
);

/**
 * @brief
 * Free an arrray of timestamp objects
 *
 * @param tmsps - Array of timestamp objects
 *
 * @warning
 * Only arrays of timestamp created with OCI_TimestampArrayCreate()
 * should be freed by OCI_TimestampArrayFree()
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_TimestampArrayFree
(
    OCI_Timestamp **tmsps
);

/**
 * @brief
 * Return the type of the given Timestamp object
 *
 * @param tmsp - Timestamp handle
 *
 * @note
 * For possible values, see OCI_TimestampCreate()
 *
 * @return
 * Object type or OCI_UNKNOWN the input handle is NULL
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_TimestampGetType
(
    OCI_Timestamp *tmsp
);

/**
 * @brief
 * Assign the value of a timestamp handle to another one
 *
 * @param tmsp     - Destination Timestamp handle
 * @param tmsp_src - Source Timestamp handle
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_TimestampAssign
(
    OCI_Timestamp *tmsp,
    OCI_Timestamp *tmsp_src
);

/**
 * @brief
 * Check if the given timestamp is valid
 *
 * @param tmsp - Timestamp handle
 *
 * @return
 * - Zero if the timestamp value is valid
 * - Any other value means the timestamp value is invalid
 *
 */

OCI_EXPORT int OCI_API OCI_TimestampCheck
(
    OCI_Timestamp *tmsp
);

/**
 * @brief
 * Compares two timestamp handles
 *
 * @param tmsp  - Timestamp1 handle
 * @param tmsp2 - Timestamp2 handle
 *
 * @return
 * - -1 if Timestamp1 is smaller than Timestamp2,
 * -  0 if they are equal
 * -  1 if Timestamp1 is greater than Timestamp2.
 *
 */

OCI_EXPORT int OCI_API OCI_TimestampCompare
(
    OCI_Timestamp *tmsp,
    OCI_Timestamp *tmsp2
);

/**
 * @brief
 * Set a timestamp handle value
 *
 * @param tmsp     - Timestamp handle
 * @param year     - Year value
 * @param month    - Month value
 * @param day      - Day value
 * @param hour     - hour value
 * @param min      - minutes value
 * @param sec      - seconds value
 * @param fsec     - fractional part of seconds value
 * @param timezone - name of a time zone to use [optional]
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_TimestampConstruct
(
    OCI_Timestamp *tmsp,
    int            year,
    int            month,
    int            day,
    int            hour,
    int            min,
    int            sec,
    int            fsec,
    const mtext   *timezone
);

/**
 * @brief
 * Convert one timestamp value from one type to another.
 *
 * @param tmsp     - Timestamp handle to convert
 * @param tmsp_src - Timestamp handle to use for the type conversion
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_TimestampConvert
(
    OCI_Timestamp *tmsp,
    OCI_Timestamp *tmsp_src
);

/**
 * @brief
 * Convert a string to a timestamp and store it in the given timestamp handle
 *
 * @param tmsp - Destination Timestamp handle
 * @param str  - Source date string
 * @param fmt  - Date format
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_TimestampFromText
(
    OCI_Timestamp *tmsp,
    const mtext   *str,
    const mtext   *fmt
);

/**
 * @brief
 * Convert a timestamp value from the given timestamp handle to a string
 *
 * @param tmsp      - source Timestamp handle
 * @param fmt       - Timestamp format
 * @param size      - Destination string size in characters
 * @param str       - Destination date string
 * @param precision - Precision for fractional part of the seconds
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_TimestampToText
(
    OCI_Timestamp *tmsp,
    const mtext   *fmt,
    int            size,
    mtext         *str,
    int            precision
);

/**
 * @brief
 * Extract the date part from a timestamp handle
 *
 * @param tmsp  - Timestamp handle
 * @param year  - Place holder for year value
 * @param month - Place holder for month value
 * @param day   - Place holder for day value
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_TimestampGetDate
(
    OCI_Timestamp *tmsp,
    int           *year,
    int           *month,
    int           *day
);

/**
 * @brief
 * Extract the time portion from a timestamp handle
 *
 * @param tmsp  - Timestamp handle
 * @param hour  - Place holder for hour value
 * @param min   - Place holder for minute value
 * @param sec   - Place holder for second value
 * @param fsec  - Place holder for fractional part of second value
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_TimestampGetTime
(
    OCI_Timestamp *tmsp,
    int           *hour,
    int           *min,
    int           *sec,
    int           *fsec
);

/**
 * @brief
 * Extract the date and time parts from a date handle
 *
 * @param tmsp  - Date handle
 * @param year  - Place holder for year value
 * @param month - Place holder for month value
 * @param day   - Place holder for day value
 * @param hour  - Place holder for hour value
 * @param min   - Place holder for minute value
 * @param sec   - Place holder for second value
 * @param fsec  - Place holder for fractional part of seconds value
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_TimestampGetDateTime
(
    OCI_Timestamp *tmsp,
    int           *year,
    int           *month,
    int           *day,
    int           *hour,
    int           *min,
    int           *sec,
    int           *fsec
);

/**
 * @brief
 * Return the time zone name of a timestamp handle
 *
 * @param tmsp - Timestamp handle
 * @param size - Destination string size in characters
 * @param str  - Destination zone name string
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_TimestampGetTimeZoneName
(
    OCI_Timestamp *tmsp,
    int            size,
    mtext         *str
);

/**
 * @brief
 * Return the time zone (hour, minute) portion of a timestamp handle
 *
 * @param tmsp  - Timestamp handle
 * @param hour  - Place holder for hour value
 * @param min   - Place holder for min value
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_TimestampGetTimeZoneOffset
(
    OCI_Timestamp *tmsp,
    int           *hour,
    int           *min
);

/**
 * @brief
 * Add an interval value to a timestamp value of a timestamp handle
 *
 * @param tmsp  - Timestamp handle
 * @param itv   - Interval handle
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_TimestampIntervalAdd
(
    OCI_Timestamp *tmsp,
    OCI_Interval  *itv
);

/**
 * @brief
 * Subtract an interval value from a timestamp value of a timestamp handle
 *
 * @param tmsp  - Timestamp handle
 * @param itv   - Interval handle
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_TimestampIntervalSub
(
    OCI_Timestamp *tmsp,
    OCI_Interval  *itv
);

/**
 * @brief
 * Store the difference of two timestamp handles into an interval handle
 *
 * @param tmsp  - Timestamp handle (subtrahend)
 * @param tmsp2 - Timestamp2 handle (minuend)
 * @param itv   - Interval handle
 *
 * @note
 * The fonction acts like tmsp - tmsp2 = itv
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_TimestampSubtract
(
    OCI_Timestamp *tmsp,
    OCI_Timestamp *tmsp2,
    OCI_Interval  *itv
);

/**
 * @brief
 * Stores the system current date and time as a timestamp value with time zone
 * into the timestamp handle.
 *
 * @param tmsp  - Timestamp handle
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_TimestampSysTimestamp
(
    OCI_Timestamp *tmsp
);

/**
 * @brief
 * Affect an OCI_Timestamp handle value to ISO C time datatypes
 *
 * @param tmsp  - Timestamp handle
 * @param ptm   - Pointer to a structure tm to receive date/time values
 * @param pt    - Pointer to a time_t to hold the date/time in the time_t format
 *
 * @note
 * Both parameters 'ptm' and 'p' are optional but one of them has to be provided.
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_TimestampToCTime
(
    OCI_Timestamp *tmsp,
    struct tm     *ptm,
    time_t        *pt
);

/**
 * @brief
 * Affect ISO C time datatypes values to an OCI_Timestamp handle
 *
 * @param tmsp  - Timestamp handle
 * @param ptm   - Pointer to a structure tm that hold the date/time value
 * @param t     - Value (time_t) that hold the date/time in the time_t format
 *
 * @note
 *
 * Both parameters 'ptm' and 'p' are optional but one of them has to be provided.
 * If 'ptm' is not null, its value is affected to the OCI_Timestamp handle,
 * otherwise the value of 't' is used.
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_TimestampFromCTime
(
    OCI_Timestamp *tmsp,
    struct tm     *ptm,
    time_t         t
);

/**
 * @brief
 * Create a local interval object
 *
 * @param con  - Connection handle
 * @param type - Type of Interval
 *
 * @note
 * From version 2.5.0, parameter 'con' can be NULL in order to manipulate
 * intervals independently from database connections
 *
 * @note
 * Interval type can be :
 *
 * - OCI_INTERVAL_YM : year / month interval
 * - OCI_INTERVAL_DS : date/ time interval
 *
 * @return
 * Return the Interval handle on success otherwise NULL on failure
 *
 */

OCI_EXPORT OCI_Interval * OCI_API OCI_IntervalCreate
(
    OCI_Connection *con,
    unsigned int    type
);

/**
 * @brief
 * Free an OCI_Interval handle
 *
 * @param itv - Interval handle
 *
 * @warning
 * Only Intervals created with OCI_IntervalCreate() should be freed by
 * OCI_IntervalFree()
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_IntervalFree
(
    OCI_Interval *itv
);

/**
 * @brief
 * Create an array of Interval object
 *
 * @param con    - Connection handle
 * @param type   - Type of Interval
 * @param nbelem - number of elements in the array
 *
 * @note
 * see OCI_IntervalCreate() for more details
 *
 * @return
 * Return the Interval handle array on success otherwise NULL on failure
 *
 */

OCI_EXPORT OCI_Interval ** OCI_API OCI_IntervalArrayCreate
(
    OCI_Connection *con,
    unsigned int    type,
    unsigned int    nbelem
);

/**
 * @brief
 * Free an arrray of Interval objects
 *
 * @param itvs - Array of Interval objects
 *
 * @warning
 * Only arrays of Interval created with OCI_IntervalArrayCreate() should be freed by
 * OCI_IntervalArrayFree()
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_IntervalArrayFree
(
    OCI_Interval **itvs
);

/**
 * @brief
 * Return the type of the given Interval object
 *
 * @param itv - Interval handle
 *
 * @note
 * For possible values, see OCI_IntervalCreate()
 *
 * @return
 * Object type or OCI_UNKNOWN the input handle is NULL
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_IntervalGetType
(
    OCI_Interval *itv
);

/**
 * @brief
 * Assign the value of a interval handle to another one
 *
 * @param itv     - Destination interval handle
 * @param itv_src - Source interval handle
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_IntervalAssign
(
    OCI_Interval *itv,
    OCI_Interval *itv_src
);

/**
 * @brief
 * Check if the given interval is valid
 *
 * @param itv - Interval handle
 *
 * @return
 * - Zero if the interval value is valid
 * - Any other value means the interval value is invalid
 *
 */

OCI_EXPORT int OCI_API OCI_IntervalCheck
(
    OCI_Interval *itv
);

/**
 * @brief
 * Compares two interval handles
 *
 * @param itv  - Interval1 handle
 * @param itv2 - Interval2 handle
 *
 * @return
 * - -1 if interval1 is smaller than interval2,
 * -  0 if they are equal
 * -  1 if interval1 is greater than interval2.
 *
 */

OCI_EXPORT int OCI_API OCI_IntervalCompare
(
    OCI_Interval *itv,
    OCI_Interval *itv2
);

/**
 * @brief
 * Convert a string to an interval and store it in the given interval handle
 *
 * @param itv  - Destination interval handle
 * @param str  - Source date string
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_IntervalFromText
(
    OCI_Interval *itv,
    const mtext  *str
);

/**
 * @brief
 * Convert an interval value from the given interval handle to a string
 *
 * @param itv           - source Interval handle
 * @param leading_prec  - Precision of the leading part
 * @param fraction_prec - Precision of the fractional part
 * @param size          - Destination string size in characters
 * @param str           - Destination date string
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_IntervalToText
(
    OCI_Interval *itv,
    int           leading_prec,
    int           fraction_prec,
    int           size,
    mtext        *str
);

/**
 * @brief
 * Correct an interval handle value with the given time zone
 *
 * @param itv - Interval handle
 * @param str - Time zone name
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_IntervalFromTimeZone
(
    OCI_Interval *itv,
    const mtext  *str
);

/**
 * @brief
 * Return the day / time portion of an interval handle
 *
 * @param itv  - Interval handle
 * @param day  - Place holder for day value
 * @param hour - Place holder for hours value
 * @param min  - Place holder for minutes value
 * @param sec  - Place holder for seconds value
 * @param fsec - Place holder for fractional part of seconds value
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_IntervalGetDaySecond
(
    OCI_Interval *itv,
    int          *day,
    int          *hour,
    int          *min,
    int          *sec,
    int          *fsec
);

/**
 * @brief
 * Return the year / month portion of an interval handle
 *
 * @param itv   - Interval handle
 * @param year  - Place holder for year value
 * @param month - Place holder for month value
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_IntervalGetYearMonth
(
    OCI_Interval *itv,
    int          *year,
    int          *month
);

/**
 * @brief
 * Set the day / time portion if the given interval handle
 *
 * @param itv   - Interval handle
 * @param day   - day value
 * @param hour  - Hour value
 * @param min   - Minute value
 * @param sec   - Second value
 * @param fsec  - Fractional part of the seconds
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_IntervalSetDaySecond
(
    OCI_Interval *itv,
    int           day,
    int           hour,
    int           min,
    int           sec,
    int           fsec
);

/**
 * @brief
 * Set the year / month portion if the given Interval handle
 *
 * @param itv    - Interval handle
 * @param year   - Year value
 * @param month  - Month value
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_IntervalSetYearMonth
(
    OCI_Interval *itv,
    int           year,
    int           month
);

/**
 * @brief
 * Adds an interval handle value to another
 *
 * @param itv   - Interval handle from witch to add
 * @param itv2  - Interval handle to add
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_IntervalAdd
(
    OCI_Interval *itv,
    OCI_Interval *itv2
);

/**
 * @brief
 * Subtract an interval handle value from another
 *
 * @param itv   - Interval handle from witch to remove
 * @param itv2  - Interval handle to remove
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_IntervalSubtract
(
    OCI_Interval *itv,
    OCI_Interval *itv2
);

/**
 * @}
 */

/**
 * @defgroup g_usertypes Oracle Named Types (Oracle OBJECTs)
 * @{
 *
 * OCILIB implements Oracle Named types (user types and built-in types) through
 * the OCI_Object type.
 *
 * OTT and C structures are not required to use objects in OCILIB.
 *
 * In order to manipulate objects attributes, OCILIB proposes a set of functions
 * to get/set properties for various supported types.
 *
 * Objects can be:
 * - Created as standalone instances (transient objects)
 * - Used for binding (persistent / transient objects)
 * - Retrieved from select statements (persistent / embedded objects)
 *
 * References (Oracle type REF) are identifiers (smart pointers) to objects and
 * are implemented in OCILIB with the type OCI_Ref.
 *
 * OCILIB implements Oracle REFs as strong typed Reference (underlying OCI REFs
 * are weaker in terms of typing).
 * It means it's mandatory to provide type information to:
 * - create a local OCI_Ref handle.
 * - register an OCI_Ref handle for a 'returning into' clause.
 *
 * @note
 * See Oracle Database SQL Language Reference for more details about REF datatype
 *
 * @warning
 * Prior to v3.5.0, OCILIB relied on some OCI routines to set/get objects
 * attributes. these OCI calls had known bugs in Unicode mode that has been fixed in Oracle 11gR2.
 * From v3.5.0, OCILIB directly sets objects attributes and thus OCILIB objects
 * can now be used in Unicode mode.
 *
 * @par Example : Inserting a local object into a table
 * @include object.c
 *
 * @par Example : Using Object References
 * @include ref.c
 *
 */

/**
 * @brief
 * Create a local object instance
 *
 * @param con    - Connection handle
 * @param typinf - Object type (type info handle)
 *
 * @return
 * Return the object handle on success otherwise NULL on failure
 *
 */

OCI_EXPORT OCI_Object * OCI_API OCI_ObjectCreate
(
    OCI_Connection *con,
    OCI_TypeInfo   *typinf
);

/**
 * @brief
 * Free a local object
 *
 * @param obj - Object handle
 *
 * @warning
 * Only object created with OCI_ObjectCreate() should be freed
 * by OCI_ObjectFree()
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_ObjectFree
(
    OCI_Object *obj
);

/**
 * @brief
 * Create an array of Object object
 *
 * @param con    - Connection handle
 * @param typinf - Object type (type info handle)
 * @param nbelem - number of elements in the array
 *
 * @note
 * see OCI_ObjectCreate() for more details
 *
 * @return
 * Return the Object handle array on success otherwise NULL on failure
 *
 */

OCI_EXPORT OCI_Object ** OCI_API OCI_ObjectArrayCreate
(
    OCI_Connection *con,
    OCI_TypeInfo   *typinf,
    unsigned int    nbelem
);

/**
 * @brief
 * Free an arrray of Object objects
 *
 * @param objs - Array of Object objects
 *
 * @warning
 * Only arrays of Object created with OCI_ObjectArrayCreate()
 * should be freed by OCI_ObjectArrayFree()
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_ObjectArrayFree
(
    OCI_Object **objs
);

/**
 * @brief
 * Assign an object to another one
 *
 * @param obj     - Destination Object handle
 * @param obj_src - Source Object handle
 *
 * @note
 * Oracle proceeds to a deep copy of the object content
 *
 * @note
 * The two object handles must have the same type otherwise an exception is thrown
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_ObjectAssign
(
    OCI_Object *obj,
    OCI_Object *obj_src
);

/**
 * @brief
 * Return the type of an object instance
 *
 * @param obj - Object handle
 *
 * @note
 * Possibles values are :
 *
 * - OCI_OBJ_PERSISTENT: persistent object from the DB
 * - OCI_OBJ_TRANSIENT : local temporary object
 * - OCI_OBJ_VALUE     : embedded object
 *
 * @return
 * Instance type or OCI_UNKNOWN the input handle is NULL
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_ObjectGetType
(
    OCI_Object *obj
);

/**
 * @brief
 * Retrieve an Oracle Ref handle from an object and assign it to the given
 * OCILIB OCI_Ref handle
 *
 * @param obj - Object handle
 * @param ref - Ref handle
 *
 * @note
 * The type information of the object and the ref must be the same, otherwise
 * an exception is thrown
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_ObjectGetSelfRef
(
    OCI_Object *obj,
    OCI_Ref    *ref
);

/**
 * @brief
 * Return the type info object associated to the object
 *
 * @param obj - Object handle
 *
 */

OCI_EXPORT OCI_TypeInfo * OCI_API OCI_ObjectGetTypeInfo
(
    OCI_Object *obj
);

/**
 * @brief
 * Return the short value of the given object attribute
 *
 * @param obj  - Object handle
 * @param attr - Attribute name
 *
 * @note
 * If the attribute is found in the object descriptor attributes list, then a
 * datatype check is performed for integrity.
 * OCI_ObjectGetShort() returns a valid value only for integer and number based attributes
 *
 * @return
 * Attribute value or 0 on failure or wrong attribute type
 *
 */

OCI_EXPORT short OCI_API OCI_ObjectGetShort
(
    OCI_Object  *obj,
    const mtext *attr
);

/**
 * @brief
 * Return the unsigned short value of the given object attribute
 *
 * @param obj  - Object handle
 * @param attr - Attribute name
 *
 * @note
 * If the attribute is found in the object descriptor attributes list, then a
 * datatype check is performed for integrity.
 * OCI_ObjectGetUnsignedShort() returns a valid value only for integer and number based attributes
 *
 * @return
 * Attribute value or 0 on failure or wrong attribute type
 *
 */

OCI_EXPORT unsigned short OCI_API OCI_ObjectGetUnsignedShort
(
    OCI_Object  *obj,
    const mtext *attr
);

/**
 * @brief
 * Return the integer value of the given object attribute
 *
 * @param obj  - Object handle
 * @param attr - Attribute name
 *
 * @note
 * If the attribute is found in the object descriptor attributes list, then a
 * datatype check is performed for integrity.
 * OCI_ObjectGetInt() returns a valid value only for integer and number based attributes
 *
 * @return
 * Attribute value or 0 on failure or wrong attribute type
 *
 */

OCI_EXPORT int OCI_API OCI_ObjectGetInt
(
    OCI_Object  *obj,
    const mtext *attr
);

/**
 * @brief
 * Return the unsigned integer value of the given object attribute
 *
 * @param obj  - Object handle
 * @param attr - Attribute name
 *
 * @note
 * If the attribute is found in the object descriptor attributes list, then a
 * datatype check is performed for integrity.
 * OCI_ObjectGetUnsignedInt() returns a valid value only for integer and number based attributes
 *
 * @return
 * Attribute value or 0 on failure or wrong attribute type
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_ObjectGetUnsignedInt
(
    OCI_Object  *obj,
    const mtext *attr
);

/**
 * @brief
 * Return the big integer value of the given object attribute
 *
 * @param obj  - Object handle
 * @param attr - Attribute name
 *
 * @note
 * If the attribute is found in the object descriptor attributes list, then a
 * datatype check is performed for integrity.
 * OCI_ObjectGetBigInt() returns a valid value only for integer and number based attributes
 *
 * @return
 * Attribute value or 0 on failure or wrong attribute type
 *
 */

OCI_EXPORT big_int OCI_API OCI_ObjectGetBigInt
(
    OCI_Object  *obj,
    const mtext *attr
);

/**
 * @brief
 * Return the unsigned big integer value of the given object attribute
 *
 * @param obj  - Object handle
 * @param attr - Attribute name
 *
 * @note
 * If the attribute is found in the object descriptor attributes list, then a
 * datatype check is performed for integrity.
 * OCI_ObjectGetUnsignedBigInt() returns a valid value only for integer and number based attributes
 *
 * @return
 * Attribute value or 0 on failure or wrong attribute type
 *
 */

OCI_EXPORT big_uint OCI_API OCI_ObjectGetUnsignedBigInt
(
    OCI_Object  *obj,
    const mtext *attr
);

/**
 * @brief
 * Return the double value of the given object attribute
 *
 * @param obj  - Object handle
 * @param attr - Attribute name
 *
 * @note
 * If the attribute is found in the object descriptor attributes list, then a
 * datatype check is performed for integrity.
 * OCI_ObjectGetDouble() returns a valid value only for integer and number based attributes
 *
 * @return
 * Attribute value or 0.0 on failure or wrong attribute type
 *
 */

OCI_EXPORT double OCI_API OCI_ObjectGetDouble
(
    OCI_Object  *obj,
    const mtext *attr
);

/**
 * @brief
 * Return the string value of the given object attribute
 *
 * @param obj  - Object handle
 * @param attr - Attribute name
 *
 * @note
 * If the attribute is found in the object descriptor attributes list, then a
 * datatype check is performed for integrity.
 * OCI_ObjectGetString() returns a valid value only for string based attributes
 *
 * @return
 * Attribute value or NULL on failure or wrong attribute type
 *
 */

OCI_EXPORT const dtext * OCI_API OCI_ObjectGetString
(
    OCI_Object  *obj,
    const mtext *attr
);

/**
 * @brief
 * Return the raw attribute value of the given object attribute into the
 * given buffer
 *
 * @param obj   - Object handle
 * @param attr  - Attribute name
 * @param value - Destination buffer
 * @param len   - Max size to write into buffer

 * @note
 * If the attribute is found in the object descriptor attributes list, then a
 * datatype check is performed for integrity.
 * OCI_ObjectGetRaw() copies data into the buffer only for raw based attributes
 *
 * @return
 * Number of bytes written to the buffer or 0 on failure or wrong attribute type
 *
 */

OCI_EXPORT int OCI_API OCI_ObjectGetRaw
(
    OCI_Object  *obj,
    const mtext *attr,
    void        *value,
    unsigned int len
);

/**
 * @brief
 * Return the date value of the given object attribute
 *
 * @param obj  - Object handle
 * @param attr - Attribute name
 *
 * @note
 * If the attribute is found in the object descriptor attributes list, then a
 * datatype check is performed for integrity.
 * OCI_ObjectGetDate() returns a valid value only for date based attributes
 *
 * @return
 * Attribute value or NULL on failure or wrong attribute type
 *
 */

OCI_EXPORT OCI_Date * OCI_API OCI_ObjectGetDate
(
    OCI_Object  *obj,
    const mtext *attr
);

/**
 * @brief
 * Return the timestamp value of the given object attribute
 *
 * @param obj  - Object handle
 * @param attr - Attribute name
 *
 * @note
 * If the attribute is found in the object descriptor attributes list, then a
 * datatype check is performed for integrity.
 * OCI_ObjectGetTimestamp() returns a valid value only for timestamps based attributes
 *
 * @return
 * Attribute value or NULL on failure or wrong attribute type
 *
 */

OCI_EXPORT OCI_Timestamp * OCI_API OCI_ObjectGetTimestamp
(
    OCI_Object  *obj,
    const mtext *attr
);

/**
 * @brief
 * Return the interval value of the given object attribute
 *
 * @param obj  - Object handle
 * @param attr - Attribute name
 *
 * @note
 * If the attribute is found in the object descriptor attributes list, then a
 * datatype check is performed for integrity.
 * OCI_ObjectGetInterval() returns a valid value only for intervals based attributes
 *
 * @return
 * Attribute value or NULL on failure or wrong attribute type
 *
 */

OCI_EXPORT OCI_Interval * OCI_API OCI_ObjectGetInterval
(
    OCI_Object  *obj,
    const mtext *attr
);

/**
 * @brief
 * Return the collection value of the given object attribute
 *
 * @param obj  - Object handle
 * @param attr - Attribute name
 *
 * @note
 * If the attribute is found in the object descriptor attributes list, then a
 * datatype check is performed for integrity.
 * OCI_ObjectGetColl() returns a valid value only for intervals based attributes
 *
 * @return
 * Attribute value or NULL on failure or wrong attribute type
 *
 */

OCI_EXPORT OCI_Coll * OCI_API OCI_ObjectGetColl
(
    OCI_Object  *obj,
    const mtext *attr
);

/**
 * @brief
 * Return the Ref value of the given object attribute
 *
 * @param obj  - Object handle
 * @param attr - Attribute name
 *
 * @note
 * If the attribute is found in the object descriptor attributes list, then a
 * datatype check is performed for integrity.
 * OCI_ObjectGetRef() returns a valid value only for Refs based attributes
 *
 * @return
 * Attribute value or NULL on failure or wrong attribute type
 *
 */

OCI_EXPORT OCI_Ref * OCI_API OCI_ObjectGetRef
(
    OCI_Object  *obj,
    const mtext *attr
);

/**
 * @brief
 * Return the object value of the given object attribute
 *
 * @param obj  - Object handle
 * @param attr - Attribute name
 *
 * @note
 * If the attribute is found in the object descriptor attributes list, then a
 * datatype check is performed for integrity.
 * OCI_ObjectGetObject() returns a valid value only for object based attributes
 *
 * @return
 * Attribute value or NULL on failure or wrong attribute type
 *
 */

OCI_EXPORT OCI_Object * OCI_API OCI_ObjectGetObject
(
    OCI_Object  *obj,
    const mtext *attr
);

/**
 * @brief
 * Return the lob value of the given object attribute
 *
 * @param obj  - Object handle
 * @param attr - Attribute name
 *
 * @note
 * If the attribute is found in the object descriptor attributes list, then a
 * datatype check is performed for integrity.
 * OCI_ObjectGetLob() returns a valid value only for lobs based attributes
 *
 * @return
 * Attribute value or NULL on failure or wrong attribute type
 *
 */

OCI_EXPORT OCI_Lob * OCI_API OCI_ObjectGetLob
(
    OCI_Object  *obj,
    const mtext *attr
);

/**
 * @brief
 * Return the file value of the given object attribute
 *
 * @param obj  - Object handle
 * @param attr - Attribute name
 *
 * @note
 * If the attribute is found in the object descriptor attributes list, then a
 * datatype check is performed for integrity.
 * OCI_ObjectGetFile() returns a valid value only for files based attributes
 *
 * @return
 * Attribute value or NULL on failure or wrong attribute type
 *
 */

OCI_EXPORT OCI_File * OCI_API OCI_ObjectGetFile
(
    OCI_Object  *obj,
    const mtext *attr
);

/**
 * @brief
 * Set an object attribute of type short
 *
 * @param obj    - Object handle
 * @param attr   - Attribute name
 * @param value  - Short value
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_ObjectSetShort
(
    OCI_Object  *obj,
    const mtext *attr,
    short        value
);

/**
 * @brief
 * Set an object attribute of type unsigned short
 *
 * @param obj    - Object handle
 * @param attr   - Attribute name
 * @param value  - Unsigned short value
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_ObjectSetUnsignedShort
(
    OCI_Object    *obj,
    const mtext   *attr,
    unsigned short value
);

/**
 * @brief
 * Set an object attribute of type int
 *
 * @param obj    - Object handle
 * @param attr   - Attribute name
 * @param value  - Int value
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_ObjectSetInt
(
    OCI_Object  *obj,
    const mtext *attr,
    int          value
);

/**
 * @brief
 * Set an object attribute of type unsigned int
 *
 * @param obj    - Object handle
 * @param attr   - Attribute name
 * @param value  - Unsigned int value
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_ObjectSetUnsignedInt
(
    OCI_Object  *obj,
    const mtext *attr,
    unsigned int value
);

/**
 * @brief
 * Set an object attribute of type big int
 *
 * @param obj    - Object handle
 * @param attr   - Attribute name
 * @param value  - Big int value
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_ObjectSetBigInt
(
    OCI_Object  *obj,
    const mtext *attr,
    big_int      value
);

/**
 * @brief
 * Set an object attribute of type unsigned big int
 *
 * @param obj    - Object handle
 * @param attr   - Attribute name
 * @param value  - Unsigned big int value
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_ObjectSetUnsignedBigInt
(
    OCI_Object  *obj,
    const mtext *attr,
    big_uint     value
);

/**
 * @brief
 * Set an object attribute of type double
 *
 * @param obj    - Object handle
 * @param attr   - Attribute name
 * @param value  - Double value
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_ObjectSetDouble
(
    OCI_Object  *obj,
    const mtext *attr,
    double       value
);

/**
 * @brief
 * Set an object attribute of type string
 *
 * @param obj    - Object handle
 * @param attr   - Attribute name
 * @param value  - String value
 *
 * @note
 * passing a null pointer for value calls OCI_ObjectSetNull()
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_ObjectSetString
(
    OCI_Object  *obj,
    const mtext *attr,
    const dtext *value
);

/**
 * @brief
 * Set an object attribute of type RAW
 *
 * @param obj    - Object handle
 * @param attr   - Attribute name
 * @param value  - Raw value
 * @param len    - Size of the raw value
 *
 * @note
 * passing a null pointer for value calls OCI_ObjectSetNull()
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_ObjectSetRaw
(
    OCI_Object  *obj,
    const mtext *attr,
    void        *value,
    unsigned int len
);

/**
 * @brief
 * Set an object attribute of type Date
 *
 * @param obj    - Object handle
 * @param attr   - Attribute name
 * @param value  - Date Handle
 *
 * @note
 * passing a null pointer for value calls OCI_ObjectSetNull()
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_ObjectSetDate
(
    OCI_Object  *obj,
    const mtext *attr,
    OCI_Date    *value
);

/**
 * @brief
 * Set an object attribute of type Timestamp
 *
 * @param obj    - Object handle
 * @param attr   - Attribute name
 * @param value  - Timestamp Handle
 *
 * @note
 * passing a null pointer for value calls OCI_ObjectSetNull()
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_ObjectSetTimestamp
(
    OCI_Object    *obj,
    const mtext   *attr,
    OCI_Timestamp *value
);

/**
 * @brief
 * Set an object attribute of type Interval
 *
 * @param obj    - Object handle
 * @param attr   - Attribute name
 * @param value  - Interval Handle
 *
 * @note
 * passing a null pointer for value calls OCI_ObjectSetNull()
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_ObjectSetInterval
(
    OCI_Object   *obj,
    const mtext  *attr,
    OCI_Interval *value
);

/**
 * @brief
 * Set an object attribute of type Collection
 *
 * @param obj    - Object handle
 * @param attr   - Attribute name
 * @param value  - Collection Handle
 *
 * @note
 * passing a null pointer for value calls OCI_ObjectSetNull()
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_ObjectSetColl
(
    OCI_Object  *obj,
    const mtext *attr,
    OCI_Coll    *value
);

/**
 * @brief
 * Set an object attribute of type Object
 *
 * @param obj    - Object handle
 * @param attr   - Attribute name
 * @param value  - Object Handle
 *
 * @warning
 * This function assigns a copy of the object to the given attribute.
 * Any further modifications of the object passed as the parameter 'value'
 * will not be reflected to object 's attribute set with this call
 *
 * @note
 * passing a null pointer for value calls OCI_ObjectSetNull()
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_ObjectSetObject
(
    OCI_Object  *obj,
    const mtext *attr,
    OCI_Object  *value
);

/**
 * @brief
 * Set an object attribute of type Lob
 *
 * @param obj    - Object handle
 * @param attr   - Attribute name
 * @param value  - Lob Handle
 *
 * @note
 * passing a null pointer for value calls OCI_ObjectSetNull()
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_ObjectSetLob
(
    OCI_Object  *obj,
    const mtext *attr,
    OCI_Lob     *value
);

/**
 * @brief
 * Set an object attribute of type File
 *
 * @param obj    - Object handle
 * @param attr   - Attribute name
 * @param value  - File Handle
 *
 * @note
 * passing a null pointer for value calls OCI_ObjectSetNull()
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_ObjectSetFile
(
    OCI_Object  *obj,
    const mtext *attr,
    OCI_File    *value
);

/**
 * @brief
 * Set an object attribute of type Ref
 *
 * @param obj    - Object handle
 * @param attr   - Attribute name
 * @param value  - Ref Handle
 *
 * @note
 * passing a null pointer for value calls OCI_ObjectSetNull()
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_ObjectSetRef
(
    OCI_Object  *obj,
    const mtext *attr,
    OCI_Ref     *value
);

/**
 * @brief
 * Check if an object attribute is null
 *
 * @param obj    - Object handle
 * @param attr   - Attribute name
 *
 * @return
 * FALSE if the attribute is not null otherwise TRUE
 *
 */

OCI_EXPORT boolean OCI_API OCI_ObjectIsNull
(
    OCI_Object  *obj,
    const mtext *attr
);

/**
 * @brief
 * Set an object attribute to null
 *
 * @param obj    - Object handle
 * @param attr   - Attribute name
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_ObjectSetNull
(
    OCI_Object  *obj,
    const mtext *attr
);

/**
 * @brief
 * Retrieve the underlying C (OTT/OCI style) structure of an OCI_Object handle
 *
 * @param obj       - Object handle
 * @param pp_struct - Address of a pointer that retrieve the C structure of data
 * @param pp_ind    - Address of a pointer that retrieve the C structure of indicators
 *
 * @note
 * See Oracle OCI programming guide for more details about OTT structures.
 * The members of these structures are OCI datatypes like OCINumber, OCIString
 * that requires mixing OCILIB code and raw OCI code.
 * OCI Object API headers have to be included to handle this datatypes using OCI object functions
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_ObjectGetStruct
(
    OCI_Object *obj,
    void      **pp_struct,
    void      **pp_ind
);

/**
 * @brief
 * Create a local Ref instance
 *
 * @param con    - Connection handle
 * @param typinf - Ref type
 *
 * @return
 * Return the Ref handle on success otherwise NULL on failure
 *
 */

OCI_EXPORT OCI_Ref * OCI_API OCI_RefCreate
(
    OCI_Connection *con,
    OCI_TypeInfo   *typinf
);

/**
 * @brief
 * Free a local Ref
 *
 * @param ref - Ref handle
 *
 * @warning
 * Only Refs created with OCI_RefCreate() should be freed
 * by OCI_RefFree()
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_RefFree
(
    OCI_Ref *ref
);

/**
 * @brief
 * Create an array of Ref object
 *
 * @param con    - Connection handle
 * @param typinf - Object type (type info handle)
 * @param nbelem - number of elements in the array
 *
 * @note
 * see OCI_RefCreate() for more details
 *
 * @return
 * Return the Ref handle array on success otherwise NULL on failure
 *
 */

OCI_EXPORT OCI_Ref ** OCI_API OCI_RefArrayCreate
(
    OCI_Connection *con,
    OCI_TypeInfo   *typinf,
    unsigned int    nbelem
);

/**
 * @brief
 * Free an arrray of Ref objects
 *
 * @param refs - Array of Ref objects
 *
 * @warning
 * Only arrays of Ref created with OCI_RefArrayCreate()
 * should be freed by OCI_RefArrayFree()
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_RefArrayFree
(
    OCI_Ref **refs
);

/**
 * @brief
 * Assign a Ref to another one
 *
 * @param ref     - Destination Ref handle
 * @param ref_src - Source Ref handle
 *
 * @note
 * The two Ref handles must have the same type otherwise an exception is thrown
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_RefAssign
(
    OCI_Ref *ref,
    OCI_Ref *ref_src
);

/**
 * @brief
 * Return the type info object associated to the Ref
 *
 * @param ref - Ref handle
 *
 */

OCI_EXPORT OCI_TypeInfo * OCI_API OCI_RefGetTypeInfo
(
    OCI_Ref *ref
);

/**
 * @brief
 * Returns the object pointed by the Ref handle.
 *
 * @param ref   - Ref handle
 *
 * @return
 * The object handle is the ref is not null otherwise NULL
 *
 */

OCI_EXPORT OCI_Object * OCI_API OCI_RefGetObject
(
    OCI_Ref *ref
);

/**
 * @brief
 * Check if the Ref points to an object or not.
 *
 * @param ref   - Ref handle
 *
 * @return
 * TRUE if it does not point to a valid object otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_RefIsNull
(
    OCI_Ref *ref
);

/**
 * @brief
 * Nullify the given Ref handle
 *
 * @param ref   - Ref handle
 *
 * @note
 * this call clears the reference to object pointed by the Ref handle.
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_EXPORT boolean OCI_API OCI_RefSetNull
(
    OCI_Ref *ref
);

/**
 * @brief
 * Returns the size of the hex representation of the given Ref handle
 *
 * @param ref  - Ref handle
 *
 * @note
 * the returned size is the number of character needed to store the
 * hex representation of the Ref that can be retrieved with OCI_RefToText()
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_RefGetHexSize
(
    OCI_Ref *ref
);

/**
 * @brief
 * Converts a Ref handle value to a hexadecimal string.
 *
 * @param ref   - Ref handle
 * @param size - Destination string size in characters
 * @param str  - Destination string
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_RefToText
(
    OCI_Ref     *ref,
    unsigned int size,
    mtext       *str
);

/**
 * @}
 */

/**
 * @defgroup g_abort Aborting long operations
 * @{
 *
 * The Oracle OCI provides the ability to establish a server connection in :
 *
 * - blocking mode: each call to an OCI function returns control to the
 *   application when the call completes
 * - non-blocking mode (based on polling paradigm) : the application have to
 *   call each function until its has completed its job
 *
 * OCILIB implements OCI in blocking mode. The application has to wait for OCI
 * calls to complete to continue.
 *
 * Some operations can be long to be processed by the server.
 *
 * In order to cancel the current pending call, OCILIB provides OCI_Break() that
 * cancel the last internal OCI Call and then raise an OCI abortion error code.
 *
 * @note
 * Any call to OCI_Break() has to be done from a separate thread because the
 * thread that has executed a long OCI call is waiting for its OCI call to complete.
 *
 * @par Example
 * @include abort.c
 *
 */

/**
 * @brief
 * Perform an immediate abort of any currently Oracle OCI call
 *
 * @param con - connection handle
 *
 * @note
 * The current call will abort and generate an error
 *
 * @return
 * Returns FALSE if connection handle is NULL otherwise TRUE
 */

OCI_EXPORT boolean OCI_API OCI_Break
(
    OCI_Connection *con
);

/**
 * @}
 */

/**
 * @defgroup g_desc Describing Schema Metadata and Objects
 * @{
 *
 *
 * @par Example
 * @include desc.c
 *
 */

/**
 * @brief
 * Retrieve the available type info information
 *
 * @param con  - Connection handle
 * @param name - Table/view name to query for
 * @param type - Type of object
 *
 * @note
 * Possible values for parameter type are :
 *
 * - OCI_UNKNOWN
 * - OCI_TIF_TABLE
 * - OCI_TIF_VIEW
 * - OCI_TIF_TYPE
 *
 * @return
 * - Type info handle on success
 = - NULL if the object does not exist
 * - NULL on failure
 *
 */

OCI_EXPORT OCI_TypeInfo * OCI_API OCI_TypeInfoGet
(
    OCI_Connection *con,
    const mtext    *name,
    unsigned int    type
);

/**
 * @brief
 * Return the type of the type info object
 *
 * @param typinf - Type info handle
 *
 * @note
 * Possible values for parameter type are :
 *
 * - OCI_UNKNOWM
 * - OCI_TIF_TABLE
 * - OCI_TIF_VIEW
 * - OCI_TIF_TYPE
 *
 * @return
 * Object type or OCI_UNKNOWN the input handle is NULL
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_TypeInfoGetType
(
    OCI_TypeInfo *typinf
);

/**
 * @brief
 * Free a type info object
 *
 * @param typinf  - Type info handle
 *
 * @note
 * this call is optionnal.
 * OCI_TypeInfo object are internally tracked and
 * automatically freed when their related connection is freed
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_TypeInfoFree
(
    OCI_TypeInfo *typinf
);

/**
 * @brief
 * Retruns the number of columns of a table/view/object
 *
 * @param typinf - Type info handle
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_TypeInfoGetColumnCount
(
    OCI_TypeInfo *typinf
);

/**
 * @brief
 * Return the column object handle at the given index in the table
 *
 * @param typinf - Type info handle
 * @param index  - Column position
 *
 * @return
 * - Column handle on success
 * - NULL if index is out of bounds or on error
 *
 */

OCI_EXPORT OCI_Column * OCI_API OCI_TypeInfoGetColumn
(
    OCI_TypeInfo *typinf,
    unsigned int  index
);

/**
 * @brief
 * Return the name described by the type info object
 *
 * @param typinf - Type info handle
 *
 * @return
 *
 */

OCI_EXPORT const mtext * OCI_API OCI_TypeInfoGetName
(
    OCI_TypeInfo *typinf
);

/**
 * @}
 */

/**
 * @defgroup g_format Formatted functions
 * @{
 *
 * OCILIB offers some smart routines that takes a variable number of arguments
 * in order to minimize OCILIB function calls and reduce the amount of code lines
 *
 * On Windows platforms, the target programming language must support the __cdecl
 * calling convention
 *
 * @note
 * OCI_Immediate() and OCI_ImmediateFmt() support all OCILIB supported types
 * for output result, except :
 * - OCI_Long
 * - OCI_Statement
 * If a query output result contains one of these unsupported types, the function returns FALSE
 *
 * @note
 * In the parameter list, every output placeholder MUST be preceded by
 * an integer parameter that indicates the type of the placeholder
 * in order to handle correctly the given pointer.
 *
 * Possible values for indicating placeholders type :
 *
 * - OCI_ARG_SHORT ------> short *
 * - OCI_ARG_USHORT -----> unsigned short *
 * - OCI_ARG_INT --------> int *
 * - OCI_ARG_UINT -------> unsigned int*
 * - OCI_ARG_BIGINT -----> big_int *
 * - OCI_ARG_BIGUINT ----> unsigned big_int *
 * - OCI_ARG_DOUBLE  ----> double *
 * - OCI_ARG_TEXT -------> dtext *
 * - OCI_ARG_RAW --------> void *
 * - OCI_ARG_DATETIME ---> OCI_Date *
 * - OCI_ARG_LOB --------> OCI_Lob *
 * - OCI_ARG_FILE -------> OCI_File *
 * - OCI_ARG_TIMESTAMP --> OCI_Timstamp *
 * - OCI_ARG_INTERVAL ---> OCI_Interval *
 * - OCI_ARG_OBJECT -----> OCI_Object *
 * - OCI_ARG_COLLECTION -> OCI_Coll *
 * - OCI_ARG_REF --------> OCI_Ref *
 *
 * @note
 * For output strings and Raws, returned data is copied to the given buffer
 * instead of returning a pointer the real data.
 * So these buffers must be big enough to hold the column content. No size check is performed.
 *
 * - For strings, only the real string is copied.
 * - For Raws, the number of bytes copied is the column size
 *
 * @warning
 * Input parameters for formatted function only support a restricted set of datatypes !
 *
 * Supported input identifiers :
 *
 * - '%s'  : (dtext *) ----------> input string (quotes are added)
 * - '%m'  : (dtext *) ----------> metadata string (no quotes added)
 * - '%t'  : (OCI_Date *) -------> Date
 * - '%p'  : (OCI_Timestamp *) --> timestamp
 * - '%v'  : (OCI_Interval *) ---> Interval
 * - '%i'  : (int) --------------> signed 32 bits integer
 * - '%u'  : (unsigned int) -----> unsigned 32 bits integer
 * - '%li' : (big_int) ----------> signed 64 bits integer
 * - '%lu' : (big_uint) ---------> unsigned 64 bits integer
 * - '%hi' : (short) ------------> signed 16 bits integer
 * - '%hu' : (unsigned short) ---> unsigned 16 bits integer
 * - '%g'  : (double ) ----------> Numerics
 * - '%r'  : (OCI_Ref *) --------> Reference
 * - '%o'  : (OCI_Object *) -----> Object  (not implemented yet)
 * - '%c'  : (OCI_Coll *) -------> collection  (not implemented yet)
 *
 * @par Example
 * @include format.c
 *
 */

/**
 * @brief
 * Perform 3 calls (prepare+execute+fetch) in 1 call
 *
 * @param con  - Connection handle
 * @param sql  - SQL statement
 * @param ...  - List of program variables address to store the result of fetch operation
 *
 * @note
 * Every output parameter MUST be preceded by an integer parameter that indicates the type
 * of the placeholder in order to handle correctly the given pointer.
 *
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_Immediate
(
    OCI_Connection *con,
    const mtext    *sql,
    ...
);

/**
 * @brief
 * Performs 4 call (prepare+bind+execute+fetch) in 1 call
 *
 * @param con  - Connection handle
 * @param sql  - SQL statement
 * @param ...  - List of program values to format the SQL followed by the
 *               output variables addresses for the fetch operation
 *
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_ImmediateFmt
(
    OCI_Connection *con,
    const mtext    *sql,
    ...
);

/**
 * @brief
 * Prepare a formatted SQL statement or PL/SQL block.
 *
 * @param stmt - Statement handle
 * @param sql  - SQL statement
 * @param ...  - List of program values to format the SQL
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_EXPORT boolean OCI_PrepareFmt
(
    OCI_Statement *stmt,
    const mtext   *sql,
    ...
);

/**
 * @brief
 * Execute a formatted SQL statement or PL/SQL block.
 *
 * @param stmt - Statement handle
 * @param sql  - SQL statement
 * @param ...  - List of program values to format the SQL
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_ExecuteStmtFmt
(
    OCI_Statement *stmt,
    const mtext   *sql,
    ...
);

/**
 * @brief
 * Parse a formatted SQL statement or PL/SQL block.
 *
 * @param stmt - Statement handle
 * @param sql  - SQL statement
 * @param ...  - List of program values to format the SQL
 *
 * @note
 * This call sends the SQL or PL/SQL command to the server for parsing only.
 * The command is not executed.
 * This call is only useful to check is a command is valid or not.
 *
 * @note
 * This call prepares the statement (internal call to OCI_Prepare()) and ask
 * the Oracle server to parse its SQL or PL/SQL command.
 * OCI_Execute() can be call after OCI_ParseFmt() in order to execute the
 * statement, which means that the server will reparse again the command.
 *
 * @warning
 * Do not use OCI_ParseFmt() unless you're only interested in the parsing result
 * because the statement will be parsed again when executed and thus leading to
 * unnecessary server roundtrips and less performance
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_ParseFmt
(
    OCI_Statement *stmt,
    const mtext   *sql,
    ...
);

/**
 * @brief
 * Describe the select list of a formatted SQL select statement.
 *
 * @param stmt - Statement handle
 * @param sql  - SQL statement
 * @param ...  - List of program values to format the SQL
 *
 * @note
 * This call sends the SELECT SQL order to the server for retrieving the
 * description of the select order only.
 * The command is not executed.
 * This call is only useful to retreive information on the associated resultet
 * Call OCI_GetResultet() after OCI_Describe() to access to SELECT list
 * information
 *
 * @note
 * This call prepares the statement (internal call to OCI_Prepare()) and ask
 * the Oracle server to describe the output SELECT list.
 * OCI_Execute() can be call after OCI_Desbribe() in order to execute the
 * statement, which means that the server will parse, and describe again the SQL
 * order.
 *
 * @warning
 * Do not use OCI_Desbribe() unless you're only interested in the resultset
 * information because the statement will be parsed again when executed and thus
 * leading to unnecessary server roundtrips and less performance
 *
 * @return
 * TRUE on success otherwise FALSE
 */

OCI_EXPORT boolean OCI_DescribeFmt
(
    OCI_Statement *stmt,
    const mtext   *sql,
    ...
);

/**
 * @}
 */

/**
 * @defgroup g_hash Hash tables
 * @{
 *
 * OCILIB uses hash tables internally for index/name columns mapping.
 *
 * OCILIB makes public its hash table’s implementation public for general purpose
 * uses.
 *
 * OCI_HashTable objects manage string keys / values that can be :
 *
 * - integers
 * - strings
 * - pointers
 *
 * This hash table implementation :
 *
 * - handle collisions
 * - allows multiple values per key
 *
 * @par Internal conception
 *
 * - The hash table is composed of an array of slots.
 * - Each slot can hold a linked list of entries (one per key)
 * - Each entry can hold a linked list of values
 *
 * @note
 * - The internal hash function computes the index in the array where the entry
 *   has to be inserted/looked up.
 *
 *
 * @note
 * Collisions are handled by chaining method.
 *
 * @include hash.c
 *
 */

/**
 * @brief
 * Create a hash table
 *
 * @param size     - size of the hash table
 * @param type     - type of the hash table
 *
 * @note
 * Parameter can be one of the following values :
 *
 * - OCI_HASH_STRING  : string values
 * - OCI_HASH_INTEGER : integer values
 * - OCI_HASH_POINTER : pointer values
 *
 * @return
 * Hash handle on success or NULL on failure
 *
 */

OCI_EXPORT OCI_HashTable * OCI_API OCI_HashCreate
(
    unsigned int size,
    unsigned int type
);

/**
 * @brief
 * Destroy a hash table
 *
 * @param table - Table handle
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_HashFree
(
    OCI_HashTable *table
);

/**
 * @brief
 * Return the size of the hash table
 *
 * @param table  - Table handle
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_HashGetSize
(
    OCI_HashTable *table
);

/**
 * @brief
 * Return the type of the hash table
 *
 * @param table  - Table handle
 *
 * @note
 * the return value can be one of the following values :
 *
 * - OCI_HASH_STRING  : string values
 * - OCI_HASH_INTEGER : integer values
 * - OCI_HASH_POINTER : pointer values
 *
 * @return
 * Hashtable datatype or OCI_UNKNOWN the input handle is NULL
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_HashGetType
(
    OCI_HashTable *table
);

/**
 * @brief
 * Add a pair string key / string value to the hash table
 *
 * @param table  - Table handle
 * @param key    - String key
 * @param value  - string value
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_HashAddString
(
    OCI_HashTable *table,
    const mtext   *key,
    const mtext   *value
);

/**
 * @brief
 * Return the string value associated to the given key
 *
 * @param table  - Table handle
 * @param key    - String key
 *
 * @return
 * Stored string associated with the key otherwise NULL
 *
 */

OCI_EXPORT const mtext * OCI_API OCI_HashGetString
(
    OCI_HashTable *table,
    const mtext   *key
);

/**
 * @brief
 * Adds a pair string key / integer value to the hash table
 *
 * @param table  - Table handle
 * @param key    - String key
 * @param value  - Integer value
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_HashAddInt
(
    OCI_HashTable *table,
    const mtext   *key,
    int            value
);

/**
 * @brief
 * Return the integer value associated to the given key
 *
 * @param table  - Table handle
 * @param key    - String key
 *
 * @return
 * Stored integer associated with the key otherwise 0
 *
 */

OCI_EXPORT int OCI_API OCI_HashGetInt
(
    OCI_HashTable *table,
    const mtext   *key
);

/**
 * @brief
 * Adds a pair string key / pointer value to the hash table
 *
 * @param table  - Table handle
 * @param key    - String key
 * @param value  - Pointer value
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_HashAddPointer
(
    OCI_HashTable *table,
    const mtext   *key,
    void          *value
);

/**
 * @brief
 * Return a pointer associated with the given key
 *
 * @param table  - Table handle
 * @param key    - String key
 *
 * @return
 * Stored pointer associated with the key otherwise NULL
 *
 */

OCI_EXPORT void * OCI_API OCI_HashGetPointer
(
    OCI_HashTable *table,
    const mtext   *key
);

/**
 * @brief
 * Lookup for an entry matching the key in the table
 *
 * @param table  - Table handle
 * @param key    - String key
 * @param create - Do create the entry if not exists
 *
 * @return
 * Entry handle if key found/added otherwise NULL
 *
 */

OCI_EXPORT OCI_HashEntry * OCI_API OCI_HashLookup
(
    OCI_HashTable *table,
    const mtext   *key,
    boolean        create
);

/**
 * @brief
 * Return the first hash slot that matches the key
 *
 * @param table - Table handle
 * @param key   - String key
 *
 * @return
 * Slot handle if key found otherwise NULL
 *
 */

OCI_EXPORT OCI_HashValue * OCI_API OCI_HashGetValue
(
    OCI_HashTable *table,
    const mtext   *key
);

/**
 * @brief
 * Return the entry slot of the hash table internal list at the given position
 *
 * @param table - Table handle
 * @param index - index
 *
 * @return
 * Slot handle otherwise NULL
 *
 */

OCI_EXPORT OCI_HashEntry * OCI_API OCI_HashGetEntry
(
    OCI_HashTable *table,
    unsigned int   index
);

/**
 * @}
 */

/**
 * @defgroup g_threads Threads and mutexes
 * @{
 *
 * Oracle proposes a portable implementation of Mutex and Thread objects
 *
 * OCILIB implements these OCI features for portable multithreading support.
 *
 * Mutexes are designed for mutual exclusion between thread in order to lock resources temporarily
 *
 * Thread keys can be seen as process-wide variables that have a thread-specific
 * values. It allows to create a unique key identified by a name (string) that
 * can store values specific to each thread.
 *
 * OCILIB exposes the types OCI_Mutex, OCI_Thread
 *
 * @warning
 * OCILIB MUST be initialized with OCI_ENV_THREADED to enable threads support
 *
 * @warning
 * OCI_Thread relies on Oracle API which uses natives threading capabilities of
 * the supported platform
 *
 * @warning
 * Using OCI_Mutex :
 * - On Microsoft Windows, a thread can call OCI_MutexAcquire() more than once
 *   wihtout any blocking. Just be sure that there is an OCI_MutexRelease() for
 *   every OCI_MutexAcquire() call
 * - On Unix systems, a thread MUST call OCI_MutexRelease() after every call to
 *   OCI_MutexAcquire() in order to be able to call OCI_MutexAcquire() again. If
 *   not, it will be blocked...
 *
 * @par Example
 * @include thread.c
 *
 */

/**
 * @brief
 * Create a Mutex object
 *
 * @return
 * Mutex handle on success or NULL on failure
 *
 */

OCI_EXPORT OCI_Mutex * OCI_API OCI_MutexCreate
(
    void
);

/**
 * @brief
 * Destroy a mutex object
 *
 * @param mutex - Mutex handle
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_MutexFree
(
    OCI_Mutex *mutex
);

/**
 * @brief
 * Acquire a mutex lock
 *
 * @param mutex - Mutex handle
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_MutexAcquire
(
    OCI_Mutex *mutex
);

/**
 * @brief
 * Release a mutex lock
 *
 * @param mutex - Mutex handle
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_MutexRelease
(
    OCI_Mutex *mutex
);

/**
 * @brief
 * Create a Thread object
 *
 * @return
 * Thread handle on success or NULL on failure
 *
 */

OCI_EXPORT OCI_Thread * OCI_API OCI_ThreadCreate
(
    void
);

/**
 * @brief
 * Destroy a thread object
 *
 * @param thread - Thread handle
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_ThreadFree
(
    OCI_Thread *thread
);

/**
 * @brief
 * Execute the given routine within the given thread object
 *
 * @param thread - Thread handle
 * @param proc   - routine to execute
 * @param arg    - parameter to pass to the routine
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_ThreadRun
(
    OCI_Thread *thread,
    POCI_THREAD proc,
    void       *arg
);

/**
 * @brief
 * Join the given thread
 *
 * @param thread - Thread handle
 *
 * @note
 * This function waits for the given thread to finish

 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_ThreadJoin
(
    OCI_Thread *thread
);

/**
 * @brief
 * Create a thread key object
 *
 * @param name     - Thread key name
 * @param destfunc - Thread key value destructor function
 *
 * @note
 * Parameter proc is optional. It's called when the thread terminates to allow
 * the program to deal with the thread specific value of the key
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_ThreadKeyCreate
(
    const mtext       *name,
    POCI_THREADKEYDEST destfunc
);

/**
 * @brief
 * Set a thread key value
 *
 * @param name  - Thread key name
 * @param value - user value to set
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_ThreadKeySetValue
(
    const mtext *name,
    void        *value
);

/**
 * @brief
 * Get a thread key value
 *
 * @param name - Thread key name
 *
 * @return
 * Thread key value on success otherwise FALSE
 *
 */

OCI_EXPORT void * OCI_API OCI_ThreadKeyGetValue
(
    const mtext *name
);

/**
 * @}
 */

/**
 * @defgroup g_dirpath Direct Path loading
 * @{
 *
 * OCILIB (from version 3.2.0) support the OCI direct Path API.
 *
 * Actual implementation of direct path API does not support the following
 * elements :
 * - Objects datatypes (User Defined Types and Object References)
 * - Object tables
 * - Nested tables
 * - SQL String functions
 *
 * All scalar datatypes (numerics, characters and date/time), including LOBs
 * and LONG types are supported
 *
 * @par Oracle direct API features (from Oracle Documentation)
 *
 * The direct path load interface allows an application to access the direct path
 * load engine of the Oracle database server to perform the functions of the
 * Oracle SQL*Loader utility.
 * This functionality provides the ability to load data from external files
 * into Oracle database objects, either a table or a partition of a partitioned
 * table.
 * The OCI direct path load interface has the ability to load multiple rows by
 * loading a direct path stream which contains data for multiple rows.
 *
 * @par Oracle direct API limitation (from Oracle Documentation)
 * The direct path load interface has the following limitations which are the
 * same as SQL*Loader:
 *   - triggers are not supported
 *   - check constraints are not supported
 *   - referential integrity constraints are not supported
 *   - clustered tables are not supported
 *   - loading of remote objects is not supported
 *   - user-defined types are not supported
 *   - LOBs must be specified after all scalar columns
 *   - LONGs must be specified last
 *
 * @warning
 *
 * Its recommended to use direct path interface with an Oracle client that is
 * the same version than the database. With version < 10g, it is mandatory
 * regarding that it causes segmentation faults and it's known from Oracle that
 * advices to use the same version for client and server (see metalink KB)
 *
 * @par How to use direct path
 *
 * - 1 : Create a direct path handle with OCI_DirPathCreate()
 * - 2 : Set (optional) some direct path load attributes
 * - 3 : Describe the columns to load with OCI_DirPathSetColumn()
 * - 4 : Populate data with OCI_DirPathSetEntry()
 * - 5 : Convert the data with OCI_DirPathConvert()
 * - 6 : Load the data into the database with OCI_DirPathLoad()
 * - 7 : Repeat step 4,5,6 + reset the stream with OCI_DirPathReset() until all
 *       rows has been loaded
 * - 8 : Commit the load with OCI_DirPathFinish()
 * - 9 : Free the direct path handle with OCI_DirPathFree()
 *
 * @par Example
 * @include dirpath.c
 *
 */

/**
 * @brief
 * Create a direct path object
 *
 * @param typinf    - Table type info handle
 * @param partition - Partition name
 * @param nb_cols   - Number of columns to load
 * @param nb_rows   - Maximum of rows to handle per load operation
 *
 * @note
 * Retrieve the table type info handle with OCI_TypeInfoGet().
 * The partition name is not mandatory
 *
 * @note
 * Parameter 'nb_rows' is ignored for Oracle 8i. Prior to Oracle 9i, it's the
 * OCI client that decides of the number of rows to process per convert/load calls.
 * From Oracle 9i, OCI allows application to specify this value. Note that, the
 * OCI client might not accept the input value. After OCI_DirPathPrepare() has
 * been successfully called, OCI_DirPathGetMaxRows() returns the final number
 * of rows used for the given direct path operation.
 *
 * @return
 * Return the direct path handle on success otherwise NULL on failure
 *
 */

OCI_EXPORT OCI_DirPath * OCI_API OCI_DirPathCreate
(
    OCI_TypeInfo *typinf,
    const mtext  *partition,
    unsigned int  nb_cols,
    unsigned int  nb_rows
);

/**
 * @brief
 * Free an OCI_DirPath handle
 *
 * @param dp - Direct path Handle
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */
OCI_EXPORT boolean OCI_API OCI_DirPathFree
(
    OCI_DirPath *dp
);

/**
 * @brief
 * Describe a column to load into the given table
 *
 * @param dp      - Direct path Handle
 * @param index   - Column index
 * @param name    - Column name
 * @param maxsize - Maximum input value size for a column entry
 * @param format  - Date or numeric format to use
 *
 * @note
 * An error is thrown if :
 * - If the column specified by the 'name' parameter is not found in the table
 *   referenced by the type info handle passed to OCI_DirPathCreate()
 * - the index is out of bounds (= 0 or >= number of columns)
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_DirPathSetColumn
(
    OCI_DirPath *dp,
    unsigned int index,
    const mtext *name,
    unsigned int maxsize,
    const mtext *format
);

/**
 * @brief
 * Prepares the OCI direct path load interface before any rows can be converted
 * or loaded
 *
 * @param dp - Direct path Handle
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_DirPathPrepare
(
    OCI_DirPath *dp
);

/**
 * @brief
 * Set the value of the given row/column array entry
 *
 * @param dp        - Direct path Handle
 * @param row       - Row index
 * @param index     - Column index
 * @param value     - Value to set
 * @param size      - Size of the input value
 * @param complete  - Is the entry content fully provided ?
 *
 * @note
 * Rows and columns indexes start at 1.
 *
 * @note
 * The 'size' parameter is expressed in number of :
 * - bytes for binary columns
 * - characters for other columns
 *
 * @note
 * Direct path support piece loading for LONGs and LOBs columns. When filling
 * these columns, it's possible to provide input buffer piece by piece. In order
 * to do so :
 * - set the 'complete' parameter to FALSE
 * - set the 'size' parameter to the piece size
 * - Repeat calls to OCI_DirPathSetEntry() until the data is totally provided
 * - The last call that set the last piece or an entry must specify the value
 *   TRUE for the 'complete' parameter
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_DirPathSetEntry
(
    OCI_DirPath *dp,
    unsigned int row,
    unsigned int index,
    void        *value,
    unsigned     size,
    boolean      complete
);

/**
 * @brief
 * Convert user provided data to a direct path stream format
 *
 * @param dp - Direct path Handle
 *
 * @note
 * if the call does return another result than OCI_DPR_COMPLETE :
 * - OCI_DirPathGetErrorRow() returns the row where the error occured
 * - OCI_DirPathGetErrorColumn() returns the column where the error occured
 *
 * @note
 * OCI_DirPathGetAffectedRows() returns the number of rows processed in the last call.
 *
 * @return
 * Possible return values :
 * - OCI_DPR_COMPLETE : load has been successful
 * - OCI_DPR_ERROR    : an error happened while loading data
 * - OCI_DPR_FULL     : the internal stream is full
 * - OCI_DPR_PARTIAL  : a column hasn't been fully filled yet
 * - OCI_DPR_EMPTY    : no data was found to load
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_DirPathConvert
(
    OCI_DirPath *dp
);

/**
 * @brief
 * Loads the data converted to direct path stream format
 *
 * @param dp - Direct path Handle
 *
 * @note
 * OCI_DirPathGetAffectedRows() returns the number of rows successfully loaded in the last call.
 *
 * @return
 * Possible return values :
 * - OCI_DPR_COMPLETE : conversion has been successful
 * - OCI_DPR_ERROR    : an error happened while converting data
 * - OCI_DPR_FULL     : the internal stream is full
 * - OCI_DPR_PARTIAL  : a column hasn't been fully filled yet
 * - OCI_DPR_EMPTY    : no data was found to load
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_DirPathLoad
(
    OCI_DirPath *dp
);

/**
 * @brief
 * Reset internal arrays and streams to prepare another load
 *
 * @param dp - Direct path Handle
 *
 * @note
 * Once some data have been converted or loaded, OCI_DirPathReset() resets
 * internal OCI structures in order to prepare another load operation
 * (set entries, convert and load)
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_DirPathReset
(
    OCI_DirPath *dp
);

/**
 * @brief
 * Terminate a direct path operation and commit changes into the database
 *
 * @param dp - Direct path Handle
 *
 * @warning
 * The direct path handle cannot be used anymore after this call for any more
 * loading operations and must be freed with OCI_DirPathFree().
 *
 * @note
 * Some properties functions of the direct path handle, such as
 * OCI_DirPathGetRowCount() can be called on a terminated direct path handle
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_DirPathFinish
(
    OCI_DirPath *dp
);

/**
 * @brief
 * Terminate a direct path operation without committing changes
 *
 * @param dp - Direct path Handle
 *
 * @note
 * Any pending loaded data are cancelled.
 * Any load completion operations, such as index maintenance operations, are not performed.
 *
 * @warning
 * The direct path handle cannot be used anymore after this call for any more
 * loading operations and must be freed with OCI_DirPathFree().
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_DirPathAbort
(
    OCI_DirPath *dp
);

/**
 * @brief
 * Execute a data savepoint (server side)
 *
 * @param dp - Direct path Handle
 *
 * @note
 * Executing a data savepoint is not allowed for LOBs
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_DirPathSave
(
    OCI_DirPath *dp
);

/**
 * @brief
 * Flushes a partially loaded row from server
 *
 * @param dp - Direct path Handle
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_DirPathFlushRow
(
    OCI_DirPath *dp
);

/**
 * @brief
 * Set the current number of rows to convert and load
 *
 * @param dp      - Direct path Handle
 * @param nb_rows - Number of row to process
 *
 * @warning
 * An OCILIB error will be thrown if the value exceeds the maximum number of
 * rows in the internals arrays
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_DirPathSetCurrentRows
(
    OCI_DirPath *dp,
    unsigned int nb_rows
);

/**
 * @brief
 * Return the current number of rows used in the OCILIB internal
 * arrays of rows
 *
 * @param dp - Direct path Handle
 *
 * @return
 * Internal current array size on SUCCESS otherwise 0
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_DirPathGetCurrentRows
(
    OCI_DirPath *dp
);

/**
 * @brief
 * Return the maximum number of rows allocated in the OCI and OCILIB
 * internal arrays of rows
 *
 * @param dp - Direct path Handle
 *
 * @return
 * Internal maximum array size on SUCCESS otherwise 0
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_DirPathGetMaxRows
(
    OCI_DirPath *dp
);

/**
 * @brief
 * Set the default date format string for input conversion
 *
 * @param dp     - Direct path Handle
 * @param format - date format
 *
 * @note
 * For string to date conversion, Oracle uses :
 * - Column date format
 * - Default date format (modified by this call)
 * - Default global support environment setting
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_DirPathSetDateFormat
(
    OCI_DirPath *dp,
    const mtext *format
);

/**
 * @brief
 * Set the parallel loading mode
 *
 * @param dp    - Direct path Handle
 * @param value - enable/disable parallel mode
 *
 * @note
 * Default value is FALSE.
 *
 * @note
 * Setting the value to TRUE allows multiple load sessions to load the same
 * segment concurrently
 *
 * @par Parallel loading mode (From Oracle documentation)
 *
 * A direct load operation requires that the object being loaded is locked to
 * prevent DML on the object.
 * Note that queries are lock-free and are allowed while the object is being loaded.
 * - For a table load, if the option is set to:
 *   - FALSE, then the table DML X-Lock is acquired.
 *   - TRUE, then the table DML S-Lock is acquired.
 * - For a partition load, if the option is set to:
 *   - FALSE, then the table DML SX-Lock and partition DML X-Lock is acquired.
 *   - TRUE, then the table DML SS-Lock and partition DML S-Lock is acquired.
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_DirPathSetParallel
(
    OCI_DirPath *dp,
    boolean      value
);

/**
 * @brief
 * Set the logging mode for the loading operation
 *
 * @param dp    - Direct path Handle
 * @param value - enable/disable logging
 *
 * @par Logging mode (from Oracle Documentation)
 *
 * The NOLOG attribute of each segment determines whether image redo or
 * invalidation redo is generated:
 * - FALSE : Use the attribute of the segment being loaded.
 * - TRUE  : No logging. Overrides DDL statement, if necessary.
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_DirPathSetNoLog
(
    OCI_DirPath *dp,
    boolean      value
);

/**
 * @brief
 * Set number of elements in the date cache
 *
 * @param dp   - Direct path Handle
 * @param size - Buffer size
 *
 * @note
 * Default value is 0.
 *
 * @note
 * Setting the value to 0 disables the cache
 *
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_DirPathSetCacheSize
(
    OCI_DirPath *dp,
    unsigned int size
);

/**
 * @brief
 * Set the size of the internal stream transfer buffer
 *
 * @param dp   - Direct path Handle
 * @param size - Buffer size
 *
 * @note
 * Default value is 64KB.
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_DirPathSetBufferSize
(
    OCI_DirPath *dp,
    unsigned int size
);

/**
 * @brief
 * Return the number of rows successfully loaded into the database so far
 *
 * @param dp - Direct path Handle
 *
 * @note
 * Insertions are committed with OCI_DirPathFinish()
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_DirPathGetRowCount
(
    OCI_DirPath *dp
);

/**
 * @brief
 * return the number of rows successfully processed during in the last
 * conversion or loading call
 *
 * @param dp - Direct path Handle
 *
 * @note
 * This function called after :
 *
 * - OCI_DirPathConvert(), returns the number of converted rows
 * - OCI_DirPathload(), returns the number of loaded rows
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_DirPathGetAffectedRows
(
    OCI_DirPath *dp
);

/**
 * @brief
 * Return the column index which caused an error during data conversion
 *
 * @param dp - Direct path Handle
 *
 * @warning
 * Direct path column indexes start at 1.
 *
 * @note
 * The internal value is reset to 0 when calling OCI_DirPathConvert()
 *
 * @return
 * 0 is no error occurs otherwise the index of the given column which caused an
 * error
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_DirPathGetErrorColumn
(
    OCI_DirPath *dp
);

/**
 * @brief
 * Return the row index which caused an error during data conversion
 *
 * @param dp - Direct path Handle
 *
 * @warning
 * Direct path row indexes start at 1.
 *
 * @note
 * The internal value is reset to 0 when calling OCI_DirPathConvert()
 *
 * @return
 * 0 is no error occurs otherwise the index of the given row which caused an
 * error
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_DirPathGetErrorRow
(
    OCI_DirPath *dp
);

/**
 * @}
 */

/**
 * @defgroup g_aq Oracle Advanced Queuing (A/Q)
 * @{
 *
 * OCILIB supports Oracle Advanced Queues features
 *
 * Let's Oracle talk about this features !
 *
 * @par Oracle Queues (from Oracle Streams - Advanced Queuing User's Guide)
 *
 * Oracle Streams AQ provides database-integrated message queuing functionality.
 * It is built on top of Oracle Streams and leverages the functions of Oracle
 * Database so that messages can be stored persistently, propagated between
 * queues on different computers and databases, and transmitted using Oracle
 * Net Services and HTTP(S).
 * Because Oracle Streams AQ is implemented in database tables, all operational
 * benefits of high availability, scalability, and reliability are also
 * applicable to queue data. Standard database features such as recovery,
 * restart, and security are supported by Oracle Streams AQ. You can use
 * database development and management tools such as Oracle Enterprise Manager
 * to monitor queues. Like other database tables, queue tables can be imported
 * and exported.
 *
 * @par OCILIB implementation
 *
 * OCILIB provides a (nearly) full C implementation of Advanced Queues available in
 * Oracle OCI and proposes the following datatypes :
 *  - OCI_Msg     : Implementation of message to enqueue/dequeue from/to queues
 *  - OCI_Enqueue : Implementation of enqueuing process
 *  - OCI_Dequeue : Implementation of dequeuing process
 *  - OCI_Agent   : Implementation of Advanced queues Agents
 *
 * Note that the only AQ features not supported yet by OCILIB are :
 *   - Payloads of type AnyData
 *   - Enqueuing/dequeuing arrays of messages
 *   - Optionnal delivery mode introduced in 10gR2
 *
 * OCILIB provides as well a C API to administrate queues and queue tables initially
 * reserved to PL/SQL and Java (wrappers around PL/SQL calls).
 * This API, based on internal PL/SQL calls wrapping the DBMS_AQADM packages procedures, allow the
 * following actions :
 *  - create, alter, drop and purge queue tables (OCI_QueueTableXXX calls)
 *  - create, alter, drop, start, stop queues (OCI_QueueXXX calls)
 *
 * Note that the user connected to the database needs particular privileges to manipulate or
 * administrate queues (See Oracle Streams - Advanced Queuing User's Guide for more informations
 * on these privileges)
 *
 *@par Example
 * @include queue.c
 *
 */

/**
 * @brief
 * Create a message object based on the given payload type
 *
 * @param typinf - Type info handle
 *
 * @note
 * OCILIB supports 2 type of message payload :
 * - Oracle types (UDT)
 * - RAW data
 *
 * @note
 * Oracle Type AnyData is not supported in the current version of OCILIB
 *
 * @note
 * the parameter 'typinf' indicates the type of payload :
 * - For object payload, retrieve the object type information handle with
 *   OCI_TypeInfoGet() using the object type name
 * - For RAW payload, you MUST pass the object type information retrieved with
 *   OCI_TypeInfoGet() using "SYS.RAW" as object type name
 *
 * @warning
 * Newly created Message handles have NULL payloads.
 * For Message handling Objects payloads, OCI_MsgSetObject() returns NULL until an object handle is
 * assigned to the message.
 *
 * @note
 * When a local OCI_Msg handle is enqueued, it keeps its attributes. If it's enqeued again, another
 * identical message is posted into the queue.
 * To reset a message and empty all its properties, call OCI_MsgReset()
 * Note that OCI_MsgReset() clears the message payload.
 *
 * @return
 * Return the message handle on success otherwise NULL on failure
 *
 */


OCI_EXPORT OCI_Msg * OCI_API OCI_MsgCreate
(
    OCI_TypeInfo *typinf
);

/**
 * @brief
 * Free a message object
 *
 * @param msg - Message handle
 *
 * @warning
 * Only message handles created with OCI_MsgCreate() should be freed by OCI_MsgFree()
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_MsgFree
(
    OCI_Msg *msg
);

/**
 * @brief
 * Reset all attributes of a message object
 *
 * @param msg - Message handle
 *
 * @note
 * This function calls OCI_MsgSetxxx() with default or NULL attributes
 *
 * @warning
 * OCI_MsgReset() clears the message payload and set it to NULL
 * For messages handling objects payloads, OCI_MsgSetObject() must be called again to assign a
 * payload.

 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_MsgReset
(
    OCI_Msg *msg
);

/**
 * @brief
 * Get the object payload of the given message
 *
 * @param msg - Message handle
 *
 * @return
 * Return the object handle on success otherwise NULL on failure or if payload is NULL
 *
 */

OCI_EXPORT OCI_Object * OCI_API OCI_MsgGetObject
(
    OCI_Msg *msg
);

/**
 * @brief
 * Set the object payload of the given message
 *
 * @param msg - Message handle
 * @param obj - Object handle
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_MsgSetObject
(
    OCI_Msg    *msg,
    OCI_Object *obj
);

/**
 * @brief
 * Get the RAW payload of the given message
 *
 * @param msg  - Message handle
 * @param raw  - Input buffer
 * @param size - Input buffer maximum size
 *
 * @note
 * On output, parameter 'size' holds the number of bytes copied into the given buffer
 *
 * @return
 * TRUE on success otherwise FALSE on failure or if payload is object based.
 *
 */

OCI_EXPORT boolean OCI_API OCI_MsgGetRaw
(
    OCI_Msg      *msg,
    void         *raw,
    unsigned int *size
);

/**
 * @brief
 * Set the RAW payload of the given message
 *
 * @param msg  - Message handle
 * @param raw  - Raw data
 * @param size - Raw data size
 *
 * @return
 * TRUE on success otherwise FALSE on failure or if payload is object based.
 *
 */

OCI_EXPORT boolean OCI_API OCI_MsgSetRaw
(
    OCI_Msg      *msg,
    const void   *raw,
    unsigned int  size
);

/**
 * @brief
 * Return the number of attempts that have been made to dequeue the message
 *
 * @param msg - Message handle
 *
 */

OCI_EXPORT int OCI_API OCI_MsgGetAttemptCount
(
    OCI_Msg *msg
);

/**
 * @brief
 * Return the number of seconds that a message is delayed for dequeuing
 *
 * @param msg - Message handle
 *
 * @note
 * see OCI_MsgSetEnqueueDelay() for more details
 *
 */

OCI_EXPORT int OCI_API OCI_MsgGetEnqueueDelay
(
    OCI_Msg *msg
);

/**
 * @brief
 * set the number of seconds to delay the enqueued message
 *
 * @param msg   - Message handle
 * @param value - Delay in seconds
 *
 * @note
 * The delay represents the number of seconds after which a message is available for dequeuing.
 * When the message is enqueued, its state is set to OCI_AMS_WAITING.
 * When the delay expires, its state is set to OCI_AMS_READY.
 *
 * @note
 * If parameter 'value' is set to zero (default value), the message will be immediately available
 * for dequeuing
 *
 * @warning
 * Dequeuing by Message ID overrides the delay specification.
 *
 * @warning
 * Delaying processing requires the queue monitor to be started.
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_MsgSetEnqueueDelay
(
    OCI_Msg *msg,
    int      value
);

/**
 * @brief
 * return the time the message was enqueued
 *
 * @param msg - Message handle
 *
 * @note
 * Only use this function for message dequeued from queues
 *
 */

OCI_EXPORT OCI_Date * OCI_API OCI_MsgGetEnqueueTime
(
    OCI_Msg *msg
);

/**
 * @brief
 * Return the duration that the message is available for dequeuing
 *
 * @param msg - Message handle
 *
 * @note
 * see OCI_MsgSetExpiration() for more details
 *
 */

OCI_EXPORT int OCI_API OCI_MsgGetExpiration
(
    OCI_Msg *msg
);

/**
 * @brief
 * set the duration that the message is available for dequeuing
 *
 * @param msg   - Message handle
 * @param value - duration in seconds
 *
 * @note
 * This parameter is an offset from the delay (see OCI_MsgSetEnqueueDelay())
 * While waiting for expiration, the message state is set to OCI_AMS_READY.
 * If the message is not dequeued before it expires, it will be moved to the exception queue
 * with the state OCI_AMS_EXPIRED.
 *
 * @note
 * If parameter 'value' is set to -1 (default value), the message will not expire
 *
 * @warning
 * Expiration processing requires the queue monitor to be started.
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_MsgSetExpiration
(
    OCI_Msg *msg,
    int      value
);

/**
 * @brief
 * Return the state of the message at the time of the dequeue
 *
 * @param msg - Message handle
 *
 * @return
 *  - OCI_UNKNOWN       :  the function has failed to get the message state
 *  - OCI_AMS_READY     :  the message is ready to be processed
 *  - OCI_AMS_WAITING   :  the message delay has not yet completed
 *  - OCI_AMS_PROCESSED :  the message has been processed
 *  - OCI_AMS_EXPIRED   :  the message has moved to exception queue
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_MsgGetState
(
    OCI_Msg *msg
);

/**
 * @brief
 * Return the priority of the message
 *
 * @param msg - Message handle
 *
 * @note
 * see OCI_MsgSetPriority() for more details
 *
 */

OCI_EXPORT int OCI_API OCI_MsgGetPriority
(
    OCI_Msg *msg
);

/**
 * @brief
 * Set the priority of the message
 *
 * @param msg   - Message handle
 * @param value - Message priority
 *
 * @note
 *   - The priority can be any number, including negative numbers.
 *   - A smaller number indicates higher priority.
 *   - Default value is zero.
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_MsgSetPriority
(
    OCI_Msg *msg,
    int      value
);

/**
 * @brief
 * Return the ID of the message
 *
 * @param msg - Message handle
 * @param id  - Input buffer
 * @param len - Input buffer maximum size
 *
 * @note
 * The message ID is :
 *  - generated when the message is enqueued in the queue
 *  - retrieved when the message is dequeued from the queue
 *
 * @note
 * On output, parameter 'len' holds the number of bytes copied into the given buffer
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_MsgGetID
(
    OCI_Msg      *msg,
    void         *id,
    unsigned int *len
);

/**
 * @brief
 * Return the original ID of the message in the last queue that generated this message
 *
 * @param msg - Message handle
 * @param id  - Input buffer
 * @param len - Input buffer maximum size
 *
 * @warning
 * When a message is propagated from/to differents queues, this ID is the one generated for the
 * message in the previous queue.
 *
 * @note
 * On output, parameter 'len' holds the number of bytes copied into the given buffer
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_MsgGetOriginalID
(
    OCI_Msg      *msg,
    void         *id,
    unsigned int *len
);

/**
 * @brief
 * Set the original ID of the message in the last queue that generated this message
 *
 * @param msg  - Message handle
 * @param id   - Message ID
 * @param len  - Message ID size
 *
 * @warning
 * When a message is propagated from/to differents queues, this ID is the one generated for the
 * message in the previous queue.
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_MsgSetOriginalID
(
    OCI_Msg      *msg,
    const void   *id,
    unsigned int  len
);

/**
 * @brief
 * Return the original sender of a message
 *
 * @param msg    - Message handle
 *
 * @return
 * Sender Handle (OCI_Agent *) on success (if set at rnqueue time) otherwise NULL
 *
 */

OCI_EXPORT OCI_Agent * OCI_API OCI_MsgGetSender
(
    OCI_Msg   *msg
);

/**
 * @brief
 * Set the original sender of a message
 *
 * @param msg    - Message handle
 * @param sender - Message sender
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_MsgSetSender
(
    OCI_Msg   *msg,
    OCI_Agent *sender
);

/**
 * @brief
 * Set the recipient list of a message to enqueue
 *
 * @param msg       - Message handle
 * @param consumers - Recipients list (array of agent handles)
 * @param count     - Number of recipients
 *
 * @warning
 * This function should only be used for queues which allow multiple consumers.
 * The default recipients are the queue subscribers.
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_MsgSetConsumers
(
    OCI_Msg     *msg,
    OCI_Agent  **consumers,
    unsigned int count
);

/**
 * @brief
 * Get the correlation identifier of the message
 *
 * @param msg - Message handle
 *
 * @note
 * see OCI_MsgSetCorrelation() for more details
 *
 */

OCI_EXPORT const mtext * OCI_API OCI_MsgGetCorrelation
(
    OCI_Msg *msg
);

/**
 * @brief
 * set the correlation identifier of the message
 *
 * @param msg         - Message handle
 * @param correlation - Message correlation text
 *
 * @note
 * see OCI_DequeueSetCorrelation()  for more details
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_MsgSetCorrelation
(
    OCI_Msg     *msg,
    const mtext *correlation
);

/**
 * @brief
 * Get the Exception queue name of the message
 *
 * @param msg - Message handle
 *
 * @warning
 * When calling this function on a message retrieved with OCI_DequeueGet(), the returned value is
 * NULL if the default exception queue associated with the current queue is used (eg. no user
 * defined specified at enqueue time for the message)
 *
 * @note
 * see OCI_MsgSetExceptionQueue() for more details
 *
 */
OCI_EXPORT const mtext * OCI_API OCI_MsgGetExceptionQueue
(
    OCI_Msg *msg
);

/**
 * @brief
 * Set the name of the queue to which the message is moved to if it cannot be
 * processed successfully
 *
 * @param msg   - Message handle
 * @param queue - Exception queue name
 *
 * @warning
 * From Oracle Dopcumentation :
 *
 * "Messages are moved into exception queues in two cases :
 *  - If the number of unsuccessful dequeue attempts has exceeded the attribute 'max_retries' of
 *    given queue
 *  - if the message has expired.
 *
 * All messages in the exception queue are in the EXPIRED state.
 *
 * The default is the exception queue associated with the queue table.
 *
 * If the exception queue specified does not exist at the time of the move the message will be
 * moved to the default exception queue associated with the queue table and a warning will be
 * logged in the alert file.
 *
 * This attribute must refer to a valid queue name."
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_MsgSetExceptionQueue
(
    OCI_Msg     *msg,
    const mtext *queue
);

/**
 * @brief
 * Create a Enqueue object for the given queue
 *
 * @param typinf - Type info handle
 * @param name   - Queue name
 *
 * @note
 * OCILIB supports 2 type of message payload :
 * - Oracle types (UDT)
 * - RAW data
 *
 * @note
 * Oracle Type AnyData is not supported in the current version of OCILIB
 *
 * @note
 * the parameter 'typinf' indicates the type of payload to enqueue to the given queue :
 * - For object payload, retrieve the object type information handle with
 *   OCI_TypeInfoGet() using the object type name
 * - For RAW payload, you MUST pass the object type information retrieved with
 *   OCI_TypeInfoGet() using "SYS.RAW" as object type name
 *
 * @return
 * Return the Enqueue handle on success otherwise NULL on failure
 *
 */

OCI_EXPORT OCI_Enqueue * OCI_API OCI_EnqueueCreate
(
    OCI_TypeInfo *typinf,
    const mtext  *name
);

/**
 * @brief
 * Free a Enqueue object
 *
 * @param enqueue - Enqueue handle
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_EnqueueFree
(
    OCI_Enqueue *enqueue
);

/**
 * @brief
 * Enqueue a message on queue associated to the Enqueue object
 *
 * @param enqueue - Enqueue handle
 * @param msg     - Message handle to enqueue
 *
 * @return
 * Message handle on success otherwise NULL on failure or on timeout
 *
 */

OCI_EXPORT boolean OCI_API OCI_EnqueuePut
(
    OCI_Enqueue *enqueue,
    OCI_Msg     *msg
);

/**
* @brief
* Set the enqueing sequence of messages to put in the queue
*
* @param enqueue  - Enqueue handle
* @param sequence - enqueuing sequence
*
* @note
* Possible values for parameter 'sequence' :
*   - OCI_ASD_BEFORE : enqueue message before another message
*   - OCI_ASD_TOP    : enqueue message before all messages
*
* @note
* Default value is OCI_ASD_TOP
*
* @note
* if the parameter 'sequence' is set to OCI_ASD_BEFORE, the application must
* call OCI_EnqueueSetRelativeMsgID() before enqueuing the next message in the queue.
*
* @note
* In order to stop enqueuing message using a sequence deviation, call
* OCI_EnqueueSetSequenceDeviation() with the value OCI_ASD_TOP
*
* @return
* TRUE on success otherwise FALSE
*
*/

OCI_EXPORT boolean OCI_API OCI_EnqueueSetSequenceDeviation
(
    OCI_Enqueue *enqueue,
    unsigned int sequence
);

/**
 * @brief
 * Return the sequence deviation of messages to enqueue to the queue
 *
 * @param enqueue - Enqueue handle
 *
 * @note
 * see OCI_EnqueueSetSequenceDeviation() for more details
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_EnqueueGetSequenceDeviation
(
    OCI_Enqueue *enqueue
);

/**
 * @brief
 * Set whether the new message is enqueued as part of the current transaction
 *
 * @param enqueue    - Enqueue handle
 * @param visibility - Equeueing mode
 *
 * @note
 * Possible values for parameter 'mode' :
 *   - OCI_AMV_IMMEDIATE : enqueue is an independent transaction
 *   - OCI_AMV_ON_COMMIT : enqueue is part of current transaction
 *
 * @note
 * Default value is OCI_AMV_ON_COMMIT
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_EnqueueSetVisibility
(
    OCI_Enqueue *enqueue,
    unsigned int visibility
);

/**
 * @brief
 * Get the enqueueing/locking behavior
 *
 * @param enqueue - Enqueue handle
 *
 * @note
 * see OCI_EnqueueSetVisibility() for more details
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_EnqueueGetVisibility
(
    OCI_Enqueue *enqueue
);

/**
 * @brief
 * Set a message identifier to use for enqueuing messages using a sequence deviation
 *
 * @param enqueue - Enqueue handle
 * @param id      - message identifier
 * @param len     - pointer to message identifier length
 *
 * @note
 * This call is only valid if OCI_EnqueueSetSequenceDeviation() has been called
 * with the value  OCI_ASD_BEFORE
 *
 * @warning
 * if the function cannot assign the message id, the content of the parameter 'len' is set to zero.
 *
 * @note
 * see OCI_EnqueueSetSequenceDeviation() for more details
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_EnqueueSetRelativeMsgID
(
    OCI_Enqueue  *enqueue,
    const void   *id,
    unsigned int  len
);

/**
 * @brief
 * Get the current associated message identifier used for enqueuing messages
 * using a sequence deviation
 *
 * @param enqueue - Enqueue handle
 * @param id      - buffer to receive the message identifier
 * @param len     - pointer to buffer max length
 *
 * @warning
 * When the function returns, parameter 'len' hold the number of bytes assigned to parameter 'id'
 *
 * @note
 * see OCI_EnqueueGetRelativeMsgID() for more details
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_EnqueueGetRelativeMsgID
(
    OCI_Enqueue *enqueue,
    void         *id,
    unsigned int *len
);

/**
 * @brief
 * Create a Dequeue object for the given queue
 *
 * @param typinf - Type info handle
 * @param name   - Queue name
 *
 * @note
 * OCILIB supports 2 type of message payload :
 * - Oracle types (UDT)
 * - RAW data
 *
 * @note
 * Oracle Type AnyData is not supported in the current version of OCILIB
 *
 * @note
 * the parameter 'typinf' indicates the type of payload to dequeue from the given queue :
 * - For object payload, retrieve the object type information handle with
 *   OCI_TypeInfoGet() using the object type name
 * - For RAW payload, you MUST pass the object type information retrieved with
 *   OCI_TypeInfoGet() using "SYS.RAW" as object type name
 *
 * @return
 * Return the Dequeue handle on success otherwise NULL on failure
 *
 */

OCI_EXPORT OCI_Dequeue * OCI_API OCI_DequeueCreate
(
    OCI_TypeInfo *typinf,
    const mtext  *name
);

/**
 * @brief
 * Free a Dequeue object
 *
 * @param dequeue - Dequeue handle
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_DequeueFree
(
    OCI_Dequeue *dequeue
);

/**
 * @brief
 * Dequeue messages from the given queue
 *
 * @param dequeue - Dequeue handle
 *
 * @return
 * Message handle on success otherwise NULL on failure or on timeout
 *
 */

OCI_EXPORT OCI_Msg * OCI_API OCI_DequeueGet
(
    OCI_Dequeue *dequeue
);

/**
 * @brief
 * Set the current consumer name to retrieve message for.
 *
 * @param dequeue  - Dequeue handle
 * @param consumer - consumer name
 *
 * @warning
 * If a queue is not set up for multiple consumers, OCI_DequeueSetConsumer()
 * should not be called or called with parameter 'consumer' set to NULL
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_DequeueSetConsumer
(
    OCI_Dequeue *dequeue,
    const mtext *consumer
);

/**
 * @brief
 * Get the current consumer name associated with the dequeueing process.
 *
 * @param dequeue - Dequeue handle
 *
 * @note
 * see OCI_DequeueSetConsumer() for more details
 *
 */

OCI_EXPORT const mtext * OCI_API OCI_DequeueGetConsumer
(
    OCI_Dequeue *dequeue
);

/**
 * @brief
 * set the correlation identifier of the message to be dequeued
 *
 * @param dequeue - Dequeue handle
 * @param pattern - correlation identifier
 *
 * @note
 * Special pattern matching characters, such as "%" or "_" can be used.
 * If more than one message satisfies the pattern, the order of dequeuing is undetermined.
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_DequeueSetCorrelation
(
    OCI_Dequeue *dequeue,
    const mtext *pattern
);

/**
 * @brief
 * Get the correlation identifier of the message to be dequeued
 *
 * @param dequeue - Dequeue handle
 *
 * @note
 * see OCI_DequeueSetCorrelation() for more details
 *
 */

OCI_EXPORT const mtext * OCI_API OCI_DequeueGetCorrelation
(
    OCI_Dequeue *dequeue
);

/**
 * @brief
 * Set the message identifier of the message to be dequeued
 *
 * @param dequeue - Dequeue handle
 * @param id      - message identitier
 * @param len     - size of the message identitier
 *
 * @warning
 * if the function cannot assign the message id, the content of the parameter 'len' is set to zero.
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_DequeueSetRelativeMsgID
(
    OCI_Dequeue  *dequeue,
    const void   *id,
    unsigned int  len
);

/**
 * @brief
 * Get the message identifier of the message to be dequeued
 *
 * @param dequeue - Dequeue handle
 * @param id      - message identitier
 * @param len     - size of the message identitier
 *
 * @warning
 * When the function returns, parameter 'len' hold the number of bytes assigned to parameter 'id'
 *
 * @note
 * see OCI_DequeueSetRelativeMsgID() for more details
 *
 */

OCI_EXPORT boolean OCI_API OCI_DequeueGetRelativeMsgID
(
    OCI_Dequeue  *dequeue,
    void         *id,
    unsigned int *len
);

/**
 * @brief
 * Set whether the new message is dequeued as part of the current transaction
 *
 * @param dequeue    - Dequeue handle
 * @param visibility - dequeueing mode
 *
 * @warning
 * The visibility parameter is ignored when using the OCI_ADM_BROWSE dequeuing
 * mode (see OCI_DequeueSetMode())
 *
 * @note
 * Possible values for parameter 'mode' :
 *   - OCI_AMV_IMMEDIATE : dequeue is an independent transaction
 *   - OCI_AMV_ON_COMMIT : dequeue is part of current transaction
 *
 * @note
 * Default value is OCI_AMV_ON_COMMIT
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_DequeueSetVisibility
(
    OCI_Dequeue *dequeue,
    unsigned int visibility
);

/**
 * @brief
 * Get the dequeueing/locking behavior
 *
 * @param dequeue - Dequeue handle
 *
 * @note
 * see OCI_DequeueSetVisibility() for more details
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_DequeueGetVisibility
(
    OCI_Dequeue *dequeue
);

/**
 * @brief
 * Set the dequeueing/locking behavior
 *
 * @param dequeue - Dequeue handle
 * @param mode    - dequeueing mode
 *
 * @note
 * Possible values for parameter 'mode' :
 *   - OCI_ADM_BROWSE        : read message without acquiring a lock
 *   - OCI_ADM_LOCKED        : read and obtain write lock on message
 *   - OCI_ADM_REMOVE        : read the message and delete it
 *   - OCI_ADM_REMOVE_NODATA : confirm receipt of the message, but do not
 *                             deliver the actual message content
 *
 * @note
 * Default value is OCI_ADM_REMOVE
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_DequeueSetMode
(
    OCI_Dequeue *dequeue,
    unsigned int mode
);

/**
 * @brief
 * Get the dequeueing/locking behavior
 *
 * @param dequeue - Dequeue handle
 *
 * @note
 * see OCI_DequeueSetMode() for more details
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_DequeueGetMode
(
    OCI_Dequeue *dequeue
);

/**
 * @brief
 * Set the position of messages to be retrieved.
 *
 * @param dequeue  - Dequeue handle
 * @param position - navigation position
 *
 * @note
 * The dequeuing uses the following sequence :
 *   - find messages using the navigation position
 *   - apply search criterias (message correlation)
 *   - get message
 *
 * @note
 * Possible values for parameter 'position' :
 *   - OCI_ADN_FIRST_MSG        : retrieves the first message which is available
 *   - OCI_ADN_NEXT_MSG         : retrieves the next message which is available
 *   - OCI_ADN_NEXT_TRANSACTION : skips the remainder of the current transaction
 *                                group (if any) and retrieves the first message
 *                                of the next transactiong roup.
 *
 * @note
 * Default value is OCI_ADN_NEXT_MSG
 *
 * @warning
 * OCI_ADN_NEXT_TRANSACTION can only be used if message grouping is enabled for the given queue.
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_DequeueSetNavigation
(
    OCI_Dequeue *dequeue,
    unsigned int position
);

/**
 * @brief
 * Return the navigation position of messages to retrieve from the queue
 *
 * @param dequeue - Dequeue handle
 *
 * @note
 * see OCI_DequeueSetNavigation() for more details
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_DequeueGetNavigation
(
    OCI_Dequeue *dequeue
);

/**
 * @brief
 * set the time that OCIDequeueGet() waits for messages if no messages are
 * currently available
 *
 * @param dequeue - Dequeue handle
 * @param timeout - timeout in seconds
 *
 *@note
 * - Any positive values in seconds are valid.
 * - The value 0  is accepted and means OCIDequeueGet() does not wait for
 *   messages and returns immediately if no messages are available
 * - The value -1 is accepted and means OCIDequeueGet() waits for ever (until
 *   a message is available in the queue)
 *
 * @note
 * Default value is -1 (wait for ever)
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_DequeueSetWaitTime
(
    OCI_Dequeue *dequeue,
    int          timeout
);

/**
 * @brief
 * Return the time that OCIDequeueGet() waits for messages if no messages are currently available
 *
 * @param dequeue - Dequeue handle
 *
 * @note
 * see OCI_DequeueSetWaitTime() for more details
 *
 */

OCI_EXPORT int OCI_API OCI_DequeueGetWaitTime
(
    OCI_Dequeue *dequeue
);

/**
 * @brief
 * Set the Agent list to listen to message for
 *
 * @param dequeue   - Dequeue handle
 * @param consumers - Agent handle array
 * @param count     - Number of agents the the array
 *
 * @warning
 *
 * @note
 *
 * @return
 * return TRUE on success otherwise FALSE
 */

OCI_EXPORT boolean OCI_API OCI_DequeueSetAgentList
(
    OCI_Dequeue *dequeue,
    OCI_Agent  **consumers,
    unsigned int count
);

/**
 * @brief
 * Listen for messages that match any recipient of the associated Agent list
 *
 * @param dequeue - Dequeue handle
 * @param timeout - Timeout in second
 *
 * @note
 * If an Agent handle is returned, messages are available for this agent.
 * In order to retrieve its messsages :
 * - call OCI_DequeueSetConsumer() with the name of agent using OCI_AgentGetName()
 * - call OCI_DequeueGet() to dequeue it's pending messages
 *
 * @warning
 * The return value is valid only until:
 * - OCIDequeueListen() is called again
 * - OCI_DequeueFree(à is called to free the Dequeue object
 * So Do not store the handle value across calls to OCIDequeueListen()
 *
 * @return
 * An Agent handle for who messages are available on success otherwise NULL
 */

OCI_EXPORT OCI_Agent * OCI_API OCI_DequeueListen
(
    OCI_Dequeue *dequeue,
    int          timeout
);

/**
 * @brief
 * Create an AQ agent object
 *
 * @param con     - Connection handle
 * @param name    - Agent name
 * @param address - Agent address
 *
 * @note
 * An AQ agent object is :
 * - used as recipient information when enqueuing a message
 * - used as sender information when dequeuing a message
 * - used for listening message only from identified senders
 *
 * @note
 * the AQ agent address can be any Oracle identifier, up to 128 bytes.
 * the AQ agent name    can be any Oracle identifier, up to 30  bytes.
 *
 * @return
 * AQ agent handle on success otherwise NULL
 *
 */

OCI_EXPORT OCI_Agent * OCI_API OCI_AgentCreate
(
    OCI_Connection *con,
    const mtext    *name,
    const mtext    *address
);

/**
 * @brief
 * Free an AQ agent object
 *
 * @param agent - AQ agent handle
 *
 * @warning
 * Only AQ agent handle created with OCI_AgentCreate() should be freed by OCI_AgentFree()
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_AgentFree
(
    OCI_Agent *agent
);

/**
 * @brief
 * Set the given AQ agent name
 *
 * @param agent - AQ agent handle
 * @param name  - AQ agent name
 *
 * @note
 * the AQ agent name is used to identified an message send or recipient when enqueuing/dequeuing
 * a message
 *
 * @note
 * the AQ agent name can be any Oracle identifier, up to 30 bytes.

 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_AgentSetName
(
    OCI_Agent   *agent,
    const mtext *name
);

/**
 * @brief
 * Get the given AQ agent name
 *
 * @param agent - AQ agent handle
 *
 * @return
 * AQ agent name on success otherwise NULL on failure
 *
 */

OCI_EXPORT const mtext * OCI_API OCI_AgentGetName
(
    OCI_Agent *agent
);

/**
 * @brief
 * Set the given AQ agent address
 *
 * @param agent   - AQ agent handle
 * @param address - AQ agent address
 *
 * @note
 * the parameter 'address' must be of the form [schema.]queue_name[@dblink].
 *
 * @note
 * the AQ agent address can be any Oracle identifier, up to 128 bytes.
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_AgentSetAddress
(
    OCI_Agent   *agent,
    const mtext *address
);

/**
 * @brief
 * Get the given AQ agent address
 *
 * @param agent - AQ agent handle
 *
 * @note
 * See OCI_AgentSetAddress()
 *
 * @return
 * AQ agent address on success otherwise NULL on failure
 *
 */

OCI_EXPORT const mtext * OCI_API OCI_AgentGetAddress
(
    OCI_Agent *agent
);

/**
 * @brief
 * Create a queue
 *
 * @param con                   - Connection handle
 * @param queue_name            - Queue name
 * @param queue_table           - Queue table name
 * @param queue_type            - Queue type
 * @param max_retries           - Maximum number of attempts to dequeue a message
 * @param retry_delay           - Number of seconds between attempts to dequeue a message
 * @param retention_time        - number of seconds a message is retained in the queue table after
 *                                being dequeued from the queue
 * @param dependency_tracking   - Parameter reserved for future use by Oracle (MUST be set to FALSE)
 * @param comment               - Description of the queue
 *
 * @note
 * Parameter 'queue_name' can specify the shema where to create to queue ([schema.]queue_name)
 * Queue names cannot be longer than 24 characters (Oracle limit for user queues)
 *
 * @note
 * Possible values for parameter 'queue_type' :
 *  - OCI_AQT_NORMAL            : Normal queue
 *  - OCI_AQT_EXCEPTION         : Exception queue
 *  - OCI_AQT_NON_PERSISTENT    : Non persistent queue
 *
 * To set default values, pass :
 *  - queue_type     : OCI_AQT_NORMAL
 *  - max_retries    : 0
 *  - retry_delay    : 0
 *  - retention_time : 0
 *  - comment        : NULL
 *
 * @note
 * this call wraps the PL/SQL procedure DBMS_AQADM.CREATE_QUEUE().
 * Refer to Oracle Streams - Advanced Queuing User's Guide for more details
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_QueueCreate
(
    OCI_Connection *con,
    const mtext    *queue_name,
    const mtext    *queue_table,
    unsigned int    queue_type,
    unsigned int    max_retries,
    unsigned int    retry_delay,
    unsigned int    retention_time,
    boolean         dependency_tracking,
    const mtext    *comment
);

/**
 * @brief
 * Alter the given queue
 *
 * @param con                   - Connection handle
 * @param queue_name            - Queue name
 * @param max_retries           - Maximum number of attempts to dequeue a message
 * @param retry_delay           - Number of seconds between attempts to dequeue a message
 * @param retention_time        - number of seconds a message is retained in the queue table after
 *                                being dequeued from the queue
 * @param comment               - Description of the queue
 *
 * @note
 * See OCI_QueueCreate() for more details
 *
 * @warning
 * This fonction updates all attributes handled in the parameter list !
 *
 * @note
 * this call wraps the PL/SQL procedure DBMS_AQADM.ALTER_QUEUE().
 * Refer to Oracle Streams - Advanced Queuing User's Guide for more details
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_QueueAlter
(
    OCI_Connection *con,
    const mtext    *queue_name,
    unsigned int    max_retries,
    unsigned int    retry_delay,
    unsigned int    retention_time,
    const mtext    *comment
);

/**
 * @brief
 * Drop the given queue
 *
 * @param con        - Connection handle
 * @param queue_name - Queue name
 *
 * @warning
 * A queue can be dropped only if it has been stopped before.
 *
 * @note
 * this call wraps the PL/SQL procedure DBMS_AQADM.DROP_QUEUE().
 * Refer to Oracle Streams - Advanced Queuing User's Guide for more details
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_QueueDrop
(
    OCI_Connection *con,
    const mtext    *queue_name
);

/**
 * @brief
 * Start the given queue
 *
 * @param con        - Connection handle
 * @param queue_name - Queue name
 * @param enqueue    - Enable enqueue
 * @param dequeue    - Enable dequeue
 *
 * @warning
 * For exception queues, only enqueuing is allowed
 *
 * @note
 * this call wraps the PL/SQL procedure DBMS_AQADM.START_QUEUE().
 * Refer to Oracle Streams - Advanced Queuing User's Guide for more details
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_QueueStart
(
    OCI_Connection *con,
    const mtext    *queue_name,
    boolean         enqueue,
    boolean         dequeue
);

/**
 * @brief
 * Stop enqueuing or dequeuing or both on the given queue
 *
 * @param con        - Connection handle
 * @param queue_name - Queue name
 * @param enqueue    - Disable enqueue
 * @param dequeue    - Disable dequeue
 * @param wait       - Wait for current pending enqueuues/dequeues
 *
 * @warning
 * A queue cannot be stopped if there are pending transactions against the queue.
 *
 * @note
 * this call wraps the PL/SQL procedure DBMS_AQADM.STOP_QUEUE().
 * Refer to Oracle Streams - Advanced Queuing User's Guide for more details
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_QueueStop
(
    OCI_Connection *con,
    const mtext    *queue_name,
    boolean         enqueue,
    boolean         dequeue,
    boolean         wait
);

/**
 * @brief
 * Create a queue table for messages of the given type
 *
 * @param con                   - Connection handle
 * @param queue_table           - Queue table name
 * @param queue_payload_type    - Message type name
 * @param storage_clause        - Additionnal clauses for the table storage
 * @param sort_list             - Additional columns name to use for sorting
 * @param multiple_consumers    - Enable multiple consumers for each messages
 * @param message_grouping      - Specifies if messages are grouped within a transaction
 * @param comment               - Description of the queue table
 * @param primary_instance      - primary owner (instance) of the queue table
 * @param secondary_instance    - Owner of the queue table if the primary instance is not available
 * @param compatible            - lowest database version with which the queue table is compatible
 *
 * @note
 * Parameter 'queue_table' can specify the shema where to create to queue table ([schema.]queue_table)
 * Queue table names cannot be longer than 24 characters (Oracle limit for user queue tables)
 *
 * @note
 * Possible values for parameter 'queue_payload_type' :
 * - For Oracle types (UDT) : use the type name ([schema.].type_name)
 * - For RAW data           : use "SYS.RAW" or "RAW"
 *
 * @note
 * Possible values for parameter 'message_grouping' :
 *  - OCI_AGM_NONE            : each message is treated individually
 *  - OCI_AGM_TRANSACTIONNAL  : all messages enqueued in one transaction are considered part of
 *                              the same group and can be dequeued as a group of related messages.
 *
 * @note
 * Possible values for parameter 'compatible' :
 * - "8.0", "8.1", "10.0"
 *
 * To set default values, pass :
 *  - storage_clause    : NULL
 *  - sort_list         : NULL
 *  - message_grouping  : OCI_AGM_NONE
 *  - comment           : NULL
 *  - primary_instance  : 0
 *  - primary_instance  : 0
 *  - compatible        : NULL
 *
 * @note
 * this call wraps the PL/SQL procedure DBMS_AQADM.CREATE_QUEUE_TABLE().
 * Refer to Oracle Streams - Advanced Queuing User's Guide for more details
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_QueueTableCreate
(
    OCI_Connection *con,
    const mtext    *queue_table,
    const mtext    *queue_payload_type,
    const mtext    *storage_clause,
    const mtext    *sort_list,
    boolean         multiple_consumers,
    unsigned int    message_grouping,
    const mtext    *comment,
    unsigned int    primary_instance,
    unsigned int    secondary_instance,
    const mtext    *compatible
);

/**
 * @brief
 * Alter the given queue table
 *
 * @param con                   - Connection handle
 * @param queue_table           - Queue table name
 * @param comment               - Description of the queue table
 * @param primary_instance      - primary owner (instance) of the queue table
 * @param secondary_instance    - Owner of the queue table if the primary instance is not available
 *
 * @note
 * See OCI_QueueTableCreate() from more details
 *
 * @note
 * this call wraps the PL/SQL procedure DBMS_AQADM.ALTER_QUEUE_TABLE().
 * Refer to Oracle Streams - Advanced Queuing User's Guide for more details
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_QueueTableAlter
(
    OCI_Connection *con,
    const mtext    *queue_table,
    const mtext    *comment,
    unsigned int    primary_instance,
    unsigned int    secondary_instance
);

/**
 * @brief
 * Drop the given queue table
 *
 * @param con         - Connection handle
 * @param queue_table - Queue table name
 * @param force       - Force the deletion of objects related to the queue table
 *
 * @note
 * Possible values for 'force' :
 *  - TRUE  : all queues using the queue table and their associated propagation schedules are
 *            dropped automatically
 *  - FALSE : All the queues using the giben queue table must be stopped and dropped before the
 *            queue table can be dropped.
 *
 * @note
 * this call wraps the PL/SQL procedure DBMS_AQADM.DROP_QUEUE_TABLE().
 * Refer to Oracle Streams - Advanced Queuing User's Guide for more details
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_QueueTableDrop
(
    OCI_Connection *con,
    const mtext    *queue_table,
    boolean         force
);

/**
 * @brief
 * Purge messages from the given queue table
 *
 * @param con             - Connection handle
 * @param queue_table     - Queue table name
 * @param purge_condition - Optionnal SQL based conditions (see notes)
 * @param block           - Lock all queues using the queue table while doing the purge
 * @param delivery_mode   - Type of message to purge
 *
 * @note
 * Possible values for parameter 'delivery_mode' :
 *  - OCI_APM_BUFFERED      : purge only buffered   messages
 *  - OCI_APM_PERSISTENT    : purge only persistent messages
 *  - OCI_APM_ALL           : purge all messages
 *
 * @note
 * For more information about the SQL purge conditions, refer to
 *  Oracle Streams - Advanced Queuing User's Guide for more details
 *
 * @warning
 * This feature is onyl available from ORacle 10gR2.
 * This function does nothing and returns TRUE is the server version is < Oracle 10gR2
 *
 * @note
 * this call wraps the PL/SQL procedure DBMS_AQADM.PURGE_QUEUE_TABLE().
 * Refer to Oracle Streams - Advanced Queuing User's Guide for more details
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_QueueTablePurge
(
    OCI_Connection *con,
    const mtext    *queue_table,
    const mtext    *purge_condition,
    boolean         block,
    unsigned int    delivery_mode
);

/**
 * @brief
 * Migrate a queue table from one version to another
 *
 * @param con             - Connection handle
 * @param queue_table     - Queue table name
 * @param compatible      - Database version with witch the queue table has to migrate
 *
 * @note
 * Possible values for parameter 'compatible' :
 * - "8.0", "8.1", "10.0"
 *
 * @note
 * this call wraps the PL/SQL procedure DBMS_AQADM.MIGRATE_QUEUE_TABLE().
 * Refer to Oracle Streams - Advanced Queuing User's Guide for more details
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_QueueTableMigrate
(
    OCI_Connection *con,
    const mtext    *queue_table,
    const mtext    *compatible
);

/**
 * @}
 */

/**
 * @defgroup g_subscription Database Change notifications (DCN or CQN)
 * @{
 *
 * OCILIB supports Oracle 10gR2 feature Database Change Notifications (DCN)
 * also named Continuous Query Notifications (CQN)
 *
 * This features allows a client application to register notifications
 * when some changes are made in a database :
 * - Database status changes : startup and shutdown
 * - Database objects changes :
 *  - DDL changes : alter or drop actions
 *  - DML changes : insert, delete, update actions
 *
 * This feature can be really useful in applications that hold
 * a cache of data. Instead of refreshing data periodically by
 * connecting to the server, the application could only refresh
 * modified data when necessary or perform specific tasks depending on
 * the events. It saves application time, network traffic and can help
 * the design of the application logic.
 *
 * The database status change notification is also interesting to be
 * informed of instance startup / shutdown
 *
 * Check Oracle documentation for more details about this feature
 *
 * @note
 * No active database connection is required to receive the
 * notifications as they are handled by the Oracle client using a
 * dedicated socket connection to the server
 *
 * @par Dabatase changes
 *
 * The client application can be notified of any database status
 * change (single DB or multiple DB in a RAC environment).
 *
 * @par Object changes
 *
 * The notifications of object changes are based on the registration
 * of a query ('select' SQL statement).
 *
 * Oracle server will notify of any changes of any object that is
 * part of the statement result set.
 *
 * Registering a statement will notify about any changes on its
 * result set rows performed after the registration of the query.
 *
 * The query can be a simple 'select * from table' or a complex
 * query involving many tables and/or criteria in the where clause.
 *
 * For Object changes, the notification can be at :
 *  - At Object level : only the object name (schema + object) is given
 *  - At row level : same that object level + RowID of the altered row
 *
 * @warning
 * Trying to use this features with a client/server version < 10gR2 will raise an error
 *
 * @par Example
 * @include notification.c
 *
 */

/**
 * @brief
 *
 * Register a notification against the given database
 *
 * @param con      - Connection handle
 * @param name     - Notification name
 * @param type     - Subscription type
 * @param handler  - User handler callback
 * @param port     - Port to use for notifications
 * @param timeout  - notification timeout
 *
 * @note
 *
 * Parameter 'type' can be one of the following values :
 *
 * - OCI_CNT_OBJECTS   : request for changes at objects (eg. tables) level (DDL / DML)
 * - OCI_CNT_ROWS      : request for changes at rows level (DML)
 * - OCI_CNT_DATABASES : request for changes at database level (startup, shutdown)
 * - OCI_CNT_ALL       : request for all changes
 *
 * @note
 *
 * Subscription handles are automatically managed by the library
 *
 * @return
 *
 * Subscription handle on success or NULL on failure
 *
 */

OCI_EXPORT OCI_Subscription * OCI_API  OCI_SubscriptionRegister
(
    OCI_Connection *con,
    const mtext    *name,
    unsigned int    type,
    POCI_NOTIFY     handler,
    unsigned int    port,
    unsigned int    timeout
);

/**
 * @brief
 * Deregister a previously registered notification
 *
 * @param sub - Subscription handle
 *
 * @note
 * OCI_cleanup() will automatically deregister any non
 * deregistered subscriptions
 *
 * @note
 * If the database connection passed to OCI_SubscriptionRegister()
 * has been closed by the time that the application calls
 * OCI_SubscriptionUnregister, the library internally reconnects
 * to the given database, performs the deregistration and then disconnects
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_SubscriptionUnregister
(
    OCI_Subscription *sub
);

/**
 * @brief
 * Add a statement to the notification to monitor
 *
 * @param sub  - Subscription handle
 * @param stmt - Statement handle
 *
 * @note
 * The given statement must be prepared but not executed before being passed to this function.
 * OCI_SubscriptionAddStatement() executes the statement and register it for notifications
 *
 * @note
 * The given statement must hold a 'SELECT' SQL statement
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_SubscriptionAddStatement
(
    OCI_Subscription *sub,
    OCI_Statement    *stmt
);

/**
 * @brief
 * Return the name of the given registered subscription
 *
 * @param sub - Subscription handle
 *
 */

OCI_EXPORT const mtext * OCI_API OCI_SubscriptionGetName
(
    OCI_Subscription *sub
);

/**
 * @brief
 * Return the port used by the notification
 *
 * @param sub - Subscription handle
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_SubscriptionGetPort
(
    OCI_Subscription *sub
);

/**
 * @brief
 * Return the timeout of the given registered subscription
 *
 * @param sub - Subscription handle
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_SubscriptionGetTimeout
(
    OCI_Subscription *sub
);

/**
 * @brief
 * Return the type of event reported by a notification
 *
 * @param event - Event handle
 *
 * @note
 * The returned value can be one of the following values :
 *
 * - OCI_ENT_STARTUP        : a database has been started up
 * - OCI_ENT_SHUTDOWN       : a database has been shut down
 * - OCI_ENT_SHUTDOWN_ANY   : a database has been shut down (RAC)
 * - OCI_ENT_DROP_DATABASE  : a database has benn dropped
 * - OCI_ENT_DEREGISTER     : the notification is timed out
 * - OCI_ENT_OBJECT_CHANGED : a database object has been modified
 *
 * @note
 * OCI_EventGetDatabase() returns the affected database
 *
 * @note
 * OCI_EventGetObject() returns the affected object
 * ('schema_name'.'object_name')
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_EventGetType
(
    OCI_Event *event
);

/**
 * @brief
 * Return the type of operation reported by a notification
 *
 * @param event - Event handle
 *
 * @note
 * Thi call is only valid when OCI_EventGetType() reports the
 * event type OCI_ENT_OBJECT_CHANGED
 *
 * @note
 * The returned value can be one of the following values :
 *
 * - OCI_ONT_INSERT  : an insert has been performed
 * - OCI_ONT_UPDATE  : an update has been performed
 * - OCI_ONT_DELETE  : a  delete has been performed
 * - OCI_ONT_ALTER   : an alter  has been performed
 * - OCI_ONT_DROP    : a  drop   has been performed
 * - OCI_ONT_GENERIC : generic not defined action
 *
 * @note
 * OCI_EventGetDatabase() returns the affected database
 *
 * @note
 * OCI_EventGetObject() returns the affected object ('schema_name'.'object_name')
 *
 * @note
 * if OCI_CNT_ROWS is passed to OCI_SubscriptionRegister(),
 * the rowid of the altered row can be retrieved with OCI_EventGetRowid()
 *
 */

OCI_EXPORT unsigned int OCI_API OCI_EventGetOperation
(
    OCI_Event *event
);

/**
 * @brief
 * Return the name of the database that generated the event
 *
 * @param event - Event handle
 *
 */

OCI_EXPORT const dtext * OCI_API OCI_EventGetDatabase
(
    OCI_Event *event
);

/**
 * @brief
 * Return the name of the name of the object that generated the event
 *
 * @param event - Event handle
 *
 */

OCI_EXPORT const dtext * OCI_API OCI_EventGetObject
(
    OCI_Event *event
);

/**
 * @brief
 * Return the rowid of the altered database object row
 *
 * @param event - Event handle
 *
 */

OCI_EXPORT const dtext * OCI_API OCI_EventGetRowid
(
    OCI_Event *event
);

/**
 * @brief
 * Return the subscription handle that generated this event
 *
 * @param event - Event handle
 *
 * @return
 *
 *
 */

OCI_EXPORT OCI_Subscription * OCI_API OCI_EventGetSubscription
(
    OCI_Event *event
);

/**
 * @}
 */

/**
 * @defgroup g_instances Remote Instance startup/shutdown
 * @{
 *
 * OCILIB supports Oracle 11g client features for manuipulating remote Oracle instances.
 *
 * Oracle instances (on the same computer or on a remote server) can be :
 *
 * - started with OCI_DatabaseStartup()
 * - shutdown with OCI_DatabaseShutdown()
 *
 * Several options are handled for this actions
 *
 * @par Example
 * @include instance.c
 *
 */

/**
 * @brief
 * Start a database instance
 *
 * @param db         - Oracle Service Name
 * @param user       - Oracle User name
 * @param pwd        - Oracle User password
 * @param sess_mode  - Session mode
 * @param start_mode - Start mode
 * @param start_flag - Start flags
 * @param spfile     - Client-side spfile to start up the database (optionnal)
 *
 * Possible values for parameter sess_mode :
 * - OCI_SESSION_SYSDBA
 * - OCI_SESSION_SYSOPER
 *
 * @note
 * External credentials are supported by supplying a null value for the 'user' and 'pwd' parameters
 * If the param 'db' is NULL then a connection to the default local DB is done
 *
 * Possible (combined) values for parameter start_mode :
 * - OCI_DB_SPM_START : start the instance
 * - OCI_DB_SPM_MOUNT : mount the instance
 * - OCI_DB_SPM_OPEN  : open the instance
 * - OCI_DB_SPM_FULL  : start, mount and open the instance
 *
 * Possible (combined) values for parameter start_flag :
 * - OCI_DB_SPF_DEFAULT  : default startup
 * - OCI_DB_SPF_FORCE    : shuts down a running instance (if needed) using
 *                         ABORT command and starts a new instance
 * - OCI_DB_SPF_RESTRICT : allows database access only to users with both
 *                         CREATE SESSION and RESTRICTED SESSION privileges
 *
 * @note
 * If the client side spfile is not provided, the database is started with its server-side spfile
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_DatabaseStartup
(
    const mtext *db,
    const mtext *user,
    const mtext *pwd,
    unsigned int sess_mode,
    unsigned int start_mode,
    unsigned int start_flag,
    const mtext *spfile
);

/**
 * @brief
 * Shutdown a database instance
 *
 * @param db         - Oracle Service Name
 * @param user       - Oracle User name
 * @param pwd        - Oracle User password
 * @param sess_mode  - Session mode
 * @param shut_mode  - Shutdown mode
 * @param shut_flag  - Shutdown flag
 *
 *
 * @warning
 * Possible values for parameter sess_mode :
 * - OCI_SESSION_SYSDBA
 * - OCI_SESSION_SYSOPER
 *
 * @note
 * External credentials are supported by supplying a null value for the 'user' and 'pwd' parameters
 * If the param 'db' is NULL then a connection to the default local DB is done
 *
 * Possible (combined) values for parameter shut_mode :
 * - OCI_DB_SDM_SHUTDOWN : shutdown the instance
 * - OCI_DB_SDM_CLOSE    : close the instance
 * - OCI_DB_SDM_DISMOUNT : dismount the instance
 * - OCI_DB_SDM_FULL     : shutdown, close and dismount the instance
 *
 * Possible (exclusive) value for parameter shut_flag (from Oracle documentation) :
 * - OCI_DB_SDF_DEFAULT     :
 *   - Further connects are prohibited.
 *   - Waits for users to disconnect from the database
 * - OCI_DB_SDF_TRANS       :
 *   - Further connects are prohibited
 *   - No new transactions are allowed.
 *   - Waits for active transactions to complete
 * - OCI_DB_SDF_TRANS_LOCAL :
 *   - Further connects are prohibited
 *   - No new transactions are allowed.
 *   - Waits only for local transactions to complete
 * - OCI_DB_SDF_IMMEDIATE   :
 *   - Does not wait for current calls to complete or users to disconnect from the database.
 *   - All uncommitted transactions are terminated and rolled back
 * - OCI_DB_SDF_ABORT       :
 *   - Does not wait for current calls to complete or users to disconnect from the database.
 *   - All uncommitted transactions are terminated and are not rolled back.
 *   - This is the fastest possible way to shut down the database, but the next
 *     database startup may require instance recovery.
 *   - Therefore, this option should be used only in unusual circumstances
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

OCI_EXPORT boolean OCI_API OCI_DatabaseShutdown
(
    const mtext *db,
    const mtext *user,
    const mtext *pwd,
    unsigned int sess_mode,
    unsigned int shut_mode,
    unsigned int shut_flag
);

/**
 * @}
 */

/**
 * @defgroup g_handles Using OCI Handles directly
 * @{
 *
 * OCILIB conception was focused on a full but closed encapsulation of OCI.
 *
 * All OCI headers, datatypes, prototypes are imported internally
 * (linkage or runtime import).
 *
 * OCILIB public interface exposes only ISO C scalar types and OCILIB objects
 *
 * OCI is a wide and rich API that can deals with hundreds of options !
 *
 * OCILIB tries to implements most of it. But, sometimes in really specific
 * contexts, it might be necessary to directly call OCI APIs in order to use
 * uncovered OCI functionalities or options
 *
 * OCILIB proposes now a set of functions to retrieve its internal OCI handles
 *
 * @warning
 *
 * The OCILIB author strongly <b>advises</b> against the use of internal handles,
 * unless there is no other way to accomplish the task
 *
 * @warning
 *
 * Using these handles for direct application calls to OCI might lead
 * to OCILIB instability or crash if handles are incorrectly used !
 *
 */

/**
 * @brief
 * Return the OCI Environment Handle (OCIEnv *) of OCILIB library
 *
 * @return
 * OCI Environment handle otherwise NULL
 *
 */

OCI_EXPORT const void * OCI_API OCI_HandleGetEnvironment
(
    void
);

/**
 * @brief
 * Return the OCI Context Handle (OCISvcCtx *) of an OCILIB OCI_Connection object
 *
 * @param con - Connection handle
 *
 * @return
 * OCI Context handle otherwise NULL
 *
 */

OCI_EXPORT const void * OCI_API OCI_HandleGetContext
(
    OCI_Connection *con
);

/**
 * @brief
 * Return the OCI Server Handle (OCIServer *) of an OCILIB OCI_Connection object
 *
 * @param con - Connection handle
 *
 * @return
 * OCI Server handle otherwise NULL
 *
 */

OCI_EXPORT const void * OCI_API OCI_HandleGetServer
(
    OCI_Connection *con
);

/**
 * @brief
 * Return the OCI Error Handle (OCIError *) of an OCILIB OCI_Connection object
 *
 * @param con - Connection handle
 *
 * @return
 * OCI Error handle otherwise NULL
 *
 */

OCI_EXPORT const void * OCI_API OCI_HandleGetError
(
    OCI_Connection *con
);

/**
 * @brief
 * Return the OCI Session Handle (OCISession *) of an OCILIB OCI_Connection object
 *
 * @param con - Connection handle
 *
 * @return
 * OCI Session handle otherwise NULL
 *
 */

OCI_EXPORT const void * OCI_API OCI_HandleGetSession
(
    OCI_Connection *con
);

/**
 * @brief
 * Return the OCI Trasanction Handle (OCITrans *) of an OCILIB OCI_Transaction object
 *
 * @param trans - Trasanction handle
 *
 * @return
 * OCI Trasanction handle otherwise NULL
 *
 */

OCI_EXPORT const void * OCI_API OCI_HandleGetTransaction
(
    OCI_Transaction *trans
);

/**
 * @brief
 * Return the OCI Statement Handle (OCIStmt *) of an OCILIB OCI_Statement object
 *
 * @param stmt - Statement handle
 *
 * @return
 * OCI Statement handle otherwise NULL
 *
 */

OCI_EXPORT const void * OCI_API OCI_HandleGetStatement
(
    OCI_Statement *stmt
);

/**
 * @brief
 * Return the OCI LobLocator Handle (OCILobLocator *) of an OCILIB OCI_Lob object
 *
 * @param lob - Lob handle
 *
 * @return
 * OCI LobLocator handle otherwise NULL
 *
 */

OCI_EXPORT const void * OCI_API OCI_HandleGetLob
(
    OCI_Lob *lob
);

/**
 * @brief
 * Return the OCI LobLocator Handle (OCILobLocator *) of an OCILIB OCI_File object
 *
 * @param file - File handle
 *
 * @return
 * OCI LobLocator handle otherwise NULL
 *
 */

OCI_EXPORT const void * OCI_API OCI_HandleGetFile
(
    OCI_File *file
);

/**
 * @brief
 * Return the OCI Date Handle (OCIDate *) of an OCILIB OCI_Date object
 *
 * @param date - Date handle
 *
 * @return
 * OCI Date handle otherwise NULL
 *
 */

OCI_EXPORT const void * OCI_API OCI_HandleGetDate
(
    OCI_Date *date
);

/**
 * @brief
 * Return the OCI Datetime Handle (OCIDatetime *) of an OCILIB OCI_Timestamp  object
 *
 * @param tmsp - Timesstamp handle
 *
 * @return
 * OCI Datetime handle otherwise NULL
 *
 */

OCI_EXPORT const void * OCI_API OCI_HandleGetTimestamp
(
    OCI_Timestamp *tmsp
);

/**
 * @brief
 * Return OCI Interval Handle (OCIInterval *) of an OCILIB OCI_Interval object
 *
 * @param itv - Interval handle
 *
 * @return
 * OCI Interval handle otherwise NULL
 *
 */

OCI_EXPORT const void * OCI_API OCI_HandleGetInterval
(
    OCI_Interval *itv
);

/**
 * @brief
 * Return OCI Object Handle (void *) of an OCILIB OCI_Object object
 *
 * @param obj - Object handle
 *
 * @return
 * OCI Object handle otherwise NULL
 *
 */

OCI_EXPORT const void * OCI_API OCI_HandleGetObject
(
    OCI_Object *obj
);

/**
 * @brief
 * Return OCI Collection Handle (OCIColl *) of an OCILIB OCI_Coll object
 *
 * @param coll - Collection handle
 *
 * @return
 * OCI Collection handle otherwise NULL
 *
 */

OCI_EXPORT const void * OCI_API OCI_HandleGetColl
(
    OCI_Coll *coll
);

/**
 * @brief
 * Return OCI Ref Handle (OCIRef *) of an OCILIB OCI_Ref object
 *
 * @param ref - Ref handle
 *
 * @return
 * OCI Ref handle otherwise NULL
 *
 */

OCI_EXPORT const void * OCI_API OCI_HandleGetRef
(
    OCI_Ref *ref
);

/**
 * @brief
 * Return OCI Mutex handle (OCIThreadMutex *) of an OCILIB OCI_Mutex object
 *
 * @param mutex - Mutex handle
 *
 * @return
 * OCI Mutex handle otherwise NULL
 *
 */

OCI_EXPORT const void * OCI_API OCI_HandleGetMutex
(
    OCI_Mutex *mutex
);

/**
 * @brief
 * Return OCI Thread ID (OCIThreadId *) of an OCILIB OCI_Thread object
 *
 * @param thread - Thread handle
 *
 * @return
 * OCI Thread ID otherwise NULL
 *
 */

OCI_EXPORT const void * OCI_API OCI_HandleGetThreadID
(
    OCI_Thread *thread
);

/**
 * @brief
 * Return OCI Thread handle (OCIThreadHandle *) of an OCILIB OCI_Thread object
 *
 * @param thread - Thread handle
 *
 * @return
 * OCI Thread handle otherwise NULL
 *
 */

OCI_EXPORT const void * OCI_API OCI_HandleGetThread
(
    OCI_Thread *thread
);

/**
 * @brief
 * Return OCI DirectPath Context handle (OCIDirPathCtx *) of an OCILIB OCI_DirPath object
 *
 * @param dp - DirectPath handle
 *
 * @return
 * OCI DirectPath Context handle otherwise NULL
 *
 */

OCI_EXPORT const void * OCI_API OCI_HandleGetDirPathCtx
(
    OCI_DirPath *dp
);

/**
 * @brief
 * Return OCI DirectPath Column array handle (OCIDirPathColArray *) of an OCILIB OCI_DirPath object
 *
 * @param dp - DirectPath handle
 *
 * @return
 * OCI DirectPath Column array handle otherwise NULL
 *
 */

OCI_EXPORT const void * OCI_API OCI_HandleGetDirPathColArray
(
    OCI_DirPath *dp
);

/**
 * @brief
 * Return OCI DirectPath Stream handle (OCIDirPathStream *) of an OCILIB OCI_DirPath object
 *
 * @param dp - DirectPath handle
 *
 * @return
 * OCI DirectPath Stream handle otherwise NULL
 *
 */

OCI_EXPORT const void * OCI_API OCI_HandleGetDirPathStream
(
    OCI_DirPath *dp
);

/**
 * @brief
 * Return OCI Subscription handle (OCISubscription *) of an OCILIB OCI_Subscription object
 *
 * @param sub - Subscription handle
 *
 * @return
 * OCI Subscription otherwise NULL
 *
 */

OCI_EXPORT const void * OCI_API OCI_HandleGetSubscription
(
    OCI_Subscription *sub
);

/**
 * @}
 */

#ifdef  __cplusplus
}
#endif

/**
 * @defgroup g_demo OCILIB main demo application code
 * @{
 *
 * Portable Main demo application header
 * @include ocilib_demo.h
 *
 * Portable Main demo application source
 * @include ocilib_demo.c
 *
 * @}
 */

/* Compatibility with sources built with older versions of OCILIB */

/* macros added in version 2.3.0 */

#define OCI_CreateConnection                OCI_ConnectionCreate
#define OCI_FreeConnection                  OCI_ConnectionFree
#define OCI_CreateStatement                 OCI_StatementCreate
#define OCI_FreeStatement                   OCI_StatementFree

/* macros added in version 2.4.0 */

#define OCI_CreateTransaction               OCI_TransactionCreate
#define OCI_FreeTransaction                 OCI_TransactionFree
#define OCI_CreateHashTable                 OCI_HashCreate
#define OCI_FreeHashTable                   OCI_HashFree

/* macros added in version 3.0.0 */

#define OCI_GetColumnName                   OCI_ColumnGetName
#define OCI_GetColumnType                   OCI_ColumnGetType
#define OCI_GetColumnCharsetForm            OCI_ColumnGetCharsetForm
#define OCI_GetColumnSQLType                OCI_ColumnGetSQLType
#define OCI_GetColumnFullSQLType            OCI_ColumnGetFullSQLType
#define OCI_GetColumnSize                   OCI_ColumnGetSize
#define OCI_GetColumnScale                  OCI_ColumnGetScale
#define OCI_GetColumnPrecision              OCI_ColumnGetPrecision
#define OCI_GetColumnFractionnalPrecision   OCI_ColumnGetFractionnalPrecision
#define OCI_GetColumnLeadingPrecision       OCI_ColumnGetLeadingPrecision
#define OCI_GetColumnNullable               OCI_ColumnGetNullable
#define OCI_GetColumnCharUsed               OCI_ColumnGetCharUsed

#define OCI_GetFormatDate(s)                OCI_GetDefaultFormatDate(OCI_StatementGetConnection(s))
#define OCI_SetFormatDate(s, f)             OCI_SetDefaultFormatDate(OCI_StatementGetConnection(s), f)

#define OCI_ERR_API                         OCI_ERR_ORACLE

/* macros added in version 3.2.0 */

#define OCI_ERR_NOT_SUPPORTED               OCI_ERR_DATATYPE_NOT_SUPPORTED
#define OCI_SCHEMA_TABLE                    OCI_TIF_TABLE
#define OCI_SCHEMA_VIEW                     OCI_TIF_VIEW
#define OCI_SCHEMA_TYPE                     OCI_TIF_TYPE

#define OCI_Schema                          OCI_TypeInfo

#define OCI_SchemaGet                       OCI_TypeInfoGet
#define OCI_SchemaFree                      OCI_TypeInfoFree
#define OCI_SchemaGetColumnCount            OCI_TypeInfoGetColumnCount
#define OCI_SchemaGetColumn                 OCI_TypeInfoGetColumn
#define OCI_SchemaGetName                   OCI_TypeInfoGetName

#define OCI_ColumnGetFractionnalPrecision   OCI_ColumnGetFractionalPrecision

/* macro added in version 3.3.0 */

/**
 * @brief
 * [OBSOLETE] Set the bind variable at the given index to null
 *
 * @param stmt  - Statement handle
 * @param index - Index of the bind variable
 *
 * @warning
 * This call is obsolete ! Use OCI_BindSetNull() instead.
 *
 * @note
 * There is no notion of null value in C.
 * It's necessary to explicitly tell Oracle that the bind has a null value.
 * It must be done before an OCI_Execute() call
 *
 * @warning
 * Index starts with 1
 *
 * @note
 * For handled based datatypes (non scalar types), OCILIB performs an extra
 * check on handles and set the bind status to null is the handle is null
 *
 * @return
 * TRUE on success otherwise FALSE
 */

#define OCI_SetNull(stmt, index)                                               \
    OCI_BindSetNull(OCI_GetBind(stmt, index))

/**
 * @brief
 * [OBSOLETE] Set the bind variable of the given name to null
 *
 * @param stmt  - Statement handle
 * @param name  - Bind variable name
 *
 * @warning
 * This call is obsolete ! Use OCI_BindSetNull() instead.
 *
 * @note
 * There is no notion of null value in C.
 * it's necessary to explicitly tell Oracle that the bind has a null value.
 * It must be done before an OCI_Execute() call
 *
 * @note
 * For handled based datatypes (non scalar types), OCILIB performs an extra
 * check on handles and set the bind status to null is the handle is null
 *
 * @return
 * TRUE on success otherwise FALSE
 */

#define OCI_SetNull2(stmt, name)                                               \
    OCI_BindSetNull(OCI_GetBind2(stmt, name))

/**
 * @brief
 * [OBSOLETE] Set to null the bind variable at the given position in an input array
 *
 * @param stmt     - Statement handle
 * @param index    - Index of the bind variable
 * @param position - Position in the array
 *
 * @warning
 * This call is obsolete ! Use OCI_BindSetNullAtPos() instead.
 *
 * @note
 * There is no notion of null value in C.
 * It's necessary to explicitly tell Oracle that the bind has a null value.
 * It must be done before an OCI_Execute() call
 *
 * @warning
 * Index and Position starts with 1
 *
 * @note
 * For handled based datatypes (non scalar types), OCILIB performs an extra
 * check on handles and set the bind status to null is the handle is null
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

#define OCI_SetNullAtPos(stmt, index, position)                                \
    OCI_BindSetNullAtPos(OCI_GetBind(stmt, index), position)

/**
 * @brief
 * [OBSOLETE] Set to null the bind variable of the given name in an input array
 *
 * @param stmt     - Statement handle
 * @param name     - Bind variable name
 * @param position - Position in the array
 *
 * @warning
 * This call is obsolete ! Use OCI_BindSetNullAtPos() instead.
 *
 * @note
 * There is no notion of null value in C.
 * It's necessary to explicitly tell Oracle that the bind has a null value.
 * It must be done before an OCI_Execute() call
 *
 * @warning
 * Position starts with 1
 *
 * @note
 * For handled based datatypes (non scalar types), OCILIB performs an extra
 * check on handles and set the bind status to null is the handle is null
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

#define OCI_SetNullAtPos2(stmt, name, position)                                \
    OCI_BindSetNullAtPos(OCI_GetBind2(stmt, name), position)

/* macro added in version 3.4.0 */

#define OCI_8  OCI_8_1
#define OCI_9  OCI_9_0
#define OCI_10 OCI_10_1
#define OCI_11 OCI_11_1

/* macro added in version 3.6.0 */

#define OCI_CHAR_UNICODE  OCI_CHAR_WIDE
#define OCI_CSF_CHARSET   OCI_CSF_DEFAULT

/* macro added in version 3.7.0 */

#define OCI_ConnPool                OCI_Pool

#define OCI_ConnPoolCreate(db, us, pw, mo, mi, ma, in)                        \
    OCI_PoolCreate  (db, us, pw, OCI_POOL_CONNECTION, mo, mi, ma, in)

#define OCI_ConnPoolGetConnection(p)                                          \
    OCI_PoolGetConnection(p, NULL)

#define OCI_ConnPoolFree            OCI_PoolFree
#define OCI_ConnPoolGetTimeout      OCI_PoolGetConnection
#define OCI_ConnPoolSetTimeout      OCI_PoolSetTimeout
#define OCI_ConnPoolGetNoWait       OCI_PoolGetNoWait
#define OCI_ConnPoolSetNoWait       OCI_PoolSetNoWait
#define OCI_ConnPoolGetBusyCount    OCI_PoolGetBusyCount
#define OCI_ConnPoolGetOpenedCount  OCI_PoolGetOpenedCount
#define OCI_ConnPoolGetMin          OCI_PoolGetMin
#define OCI_ConnPoolGetMax          OCI_PoolGetMax
#define OCI_ConnPoolGetIncrement    OCI_PoolGetIncrement

/* macro added in version 3.8.0 */

#define OCI_ObjectGetTimeStamp      OCI_ObjectGetTimestamp
#define OCI_ElemGetTimeStamp        OCI_ElemGetTimestamp
#define OCI_TimestampSysTimeStamp   OCI_TimestampSysTimestamp

#endif    /* OCILIB_H_INCLUDED */

