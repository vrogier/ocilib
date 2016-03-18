****************************************************************************

   +----------------------------------------------------------------------+   
   |                                                                      |
   |                     OCILIB - C Driver for Oracle                     |
   |                                                                      |
   |                      (C Wrapper for Oracle OCI)                      |
   |                                                                      |
   +----------------------------------------------------------------------+
   |                                                                      |
   |                     Website : http://www.ocilib.net                  |
   |                                                                      |
   +----------------------------------------------------------------------+
   |                                                                      |
   |   Copyright (c) 2007-2016 Vincent ROGIER <vince.rogier@ocilib.net>   |
   |                                                                      |
   +----------------------------------------------------------------------+
   |                                                                      |
   |                          (WINDOWS PACKAGE)                           |
   |                                                                      |
   +----------------------------------------------------------------------+   

****************************************************************************

DESCRIPTION 
===========

OCILIB is an open source and portable Oracle Driver that delivers really fast and reliable access to Oracle databases.

The OCILIB library :

    * offers a rich, full featured and easy to use API
    * runs on all Oracle platforms
    * is written in pure ISO C code with native ISO C Unicode support
    * encapsulates OCI (Oracle Call Interface)
    * is the most complete available OCI wrapper
 
VERSION 
========

4.2.1


FEATURES 
========
   
    * Full Ansi and Unicode support on all platforms (ISO C wide strings or UTF8 strings)
    * Full 32/64 bits compatibility
    * Comptabile with all Oracle version >= 8i
    * Automatic adaptation to the runtime Oracle client version
    * Runtime loading of Oracle libraries
    * Builtin error handling (global and thread context)
    * Full support for SQL API and Object API
    * Full support for ALL Oracle SQL and PL/SQL datatypes (scalars, objects, refs, collections, ..)
    * Full support for PL/SQL (blocks, cursors, Index by Tables and Nested tables)
    * Support for non scalar datatype with trough library objects 
    * Oracle Pooling (connections and sessions pools)
    * Oracle XA connectivity (X/Open Distributed Transaction Processing XA interface)
    * Oracle AQ (Advanded Queues)
    * Oracle TAF (Transparent Application Failover) and HA (High availabality) support 
    * Binding array Interface
    * Returning DML feature
    * Scrollable statements
    * Statement cache
    * Direct Path loading
    * Remote Instances Startup/Shutdown
    * Oracle Database Change notification / Continuous Query Notification
    * Oracle warnings support
    * Global and local transactions 
    * Describe database schema objects
    * Hash tables API
    * Portable Threads and mutexes API

DOWNLOAD
========

Available at :

URL : http://www.ocilib.net

LICENSE
=======

The source code is free source code licensed under the Apache License, Version 2.0.

(see the file license.txt)

COMPATIBILITY 
=============

This current source is pure ISO C and should compile with any C90 Compliant compiler
for ansi version and C99 for Unicode versions.
 
 Validated Platforms : 
 
 - Windows 
 - UP/UX 
 - Linux 
 - Solaris 
 - AIX 
 - Mac OS X
 - OpenVMS (full validation pending)

 Validated Compilers : 

 - GCC / MinGW
 - MS Compilers 
 - XLC  
 - CCs
 - LabView 

 Validated Oracle versions : 
 
 - Oracle 8i 
 - Oracle 9i 
 - Oracle 10g 
 - Oracle 11g 


INSTALLATION (WINDOWS)
============

Precompiled OCILIB dynamic libraries are provided for Microsoft Windows :

     - ocilib\lib32 : 32bits binaires
     - ocilib\lib64 : 64bits binaires

Precompiled OCILIB static libraries are provided for Microsoft Windows :

     - ocilib\lib32 : 32bits binaries

2 methods :

1/
     - add the ocilib\include  directory to your include's path
     - add the ocilib\lib32|64 directory to your libraries path
     - add the ocilib\lib32|64 directory to your windows path

2/
     - copy  ocilib\include\ocilib.h to any place located in your include's path
     - copy  ocilib\lib32|64\ocilib[x].lib to any place located in your libraries path
     - copy  ocilib\lib32|64\ocilib[x].dll to any place located in your windows path
     - copy  ocilib\lib32\libocilib[x].a to any place located in your windows path


LIBRARY BUILDS (WINDOWS)
==============

OCILIB supports 3 builds options on Windows platforms :

1/ oracle import (linkage or runtime loading)

On win32, the provided build were compiled using is runtime loading.
If you want to import Oracle OCI at linkage time , you need to rebuild the libraries

Defines :

     - OCI_IMPORT_LINKAGE for linkage at compile time
     - OCI_IMPORT_RUNTIME for runtime loading

if no import mode is set, the library sets the default to OCI_IMPORT_LINKAGE

2/ charset (ansi, wide, mixed)

this pakcage provides 2 OCILIB Dlls (32bits and 64bits) :

      - ociliba.dll (ANSI / UTF8 version)
      - ocilibw.dll (UNICODE/UFT16/USC2/wchar_t version) 

this package provides 3 OCILIB static libs (32 bits only) :

      - libociliba.a (ANSI / UTF8 version)
      - libocilibw.a (UNICODE/UFT16/USC2/wchar_t version)  

Defines :

     - OCI_CHARSET_ANSI for ansi / UTF8
     - OCI_CHARSET_WIDE for unicode (wide strings)

if no charset mode is set, the library sets the default to OCI_CHARSET_ANSI 

3/ calling convention

On win32, the default calling convention is __stdcall.
So the OCILIB function using a variable number or parameters (OCI_xxxxFmt) won't work
If you need them, modify the preprocessor options to change OCI_API to __cdecl and rebuild !

Defines :

     - OCI_API =  __stdcall to link OCILIB to any language independent module
     - OCI_API = __cdecl (or no value) for C/C++ only !

if no charset mode is set, the library sets the default to no value


To rebuild the dynamic libraries (MS Visual Studio) :

      - open the dll project ocilib\proj\dll\ocilib_dll_vs200x.sln
      - do a batch build

To rebuild the static libraries (Code::blocks) :

      - open the static project ocilib\proj\mingw\ocilib_static_lib_mingw.cbp
      - do a batch build

BUILD DEMO (WINDOWS) 
==========

1/ run an already built demo

     - run ocilib_demo.exe located in ocilib\proj\test\Win32|64\release [build]

2/ build a demo 

     - open the test project ocilib\proj\test\ocilib_demo_vs200x.sln or ocilib_demo_codeblocks.cbp
     - choose an configuration
     - build

MAKE ANY PROGRAM WITH OCILIB (WINDOWS)
============================

1/ use the prebuilt Dlls

     - add ocilib/include folder to your compiler includes path
     - add ocilib/lib folder to your linker library path
     - For MinGW only, rename/copy ocilib[x].lib to libocilib[x].a
     - add ocilib[x].lib (or -locilib[x] from MinGw) to your linking options
     - add OCI_CHARSET_XXX macro to your preprocessor options
     - add OCI_API macro definition to your preprocessor options (set it to __stdcall)

2/ use the prebuilt Dlls

     - add ocilib/include folder to your compiler includes path
     - add ocilib/lib folder to your linker library path
     - add libocilib[x].a to your linking options
     - add OCI_CHARSET_XXX macro to your preprocessor options
     - add OCI_API macro definition to your preprocessor options (set it to __stdcall)

3/ link source code

     - add the source files located in ocilib/src to your project and defines the 3 build options

FEED BACK, BUG REPORTS 
======================

URL : http://www.ocilib.net/support

DOCUMENTATION
=============

Included in the archive  : ocilib\doc\html folder (index.html)

Online : http://www.ocilib.net/documentation/html

USE OF OCILIB
=============

Please, let me know about projects using OCILIB.

Send me a mail at <vince.rogier@ocilib.net> with a brief description of your project

Thanks.



