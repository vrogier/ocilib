/*
   +----------------------------------------------------------------------+   
   |                                                                      |
   |                     OCILIB - C Driver for Oracle                     |
   |                                                                      |
   |                      (C Wrapper for Oracle OCI)                      |
   |                                                                      |
   +----------------------------------------------------------------------+
   |                      Website : http://ocilib.net                     |
   +----------------------------------------------------------------------+
   |               Copyright (c) 2007-2009 Vincent ROGIER                 |
   +----------------------------------------------------------------------+
   | This library is free software; you can redistribute it and/or        |
   | modify it under the terms of the GNU Library General Public          |
   | License as published by the Free Software Foundation; either         |
   | version 2 of the License, or (at your option) any later version.     |
   |                                                                      |
   | This library is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU    |
   | Library General Public License for more details.                     |
   |                                                                      |
   | You should have received a copy of the GNU Library General Public    |
   | License along with this library; if not, write to the Free           |
   | Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.   |
   +----------------------------------------------------------------------+
   |          Author: Vincent ROGIER <vince.rogier@gmail.com>             |
   +----------------------------------------------------------------------+ 
*/

/* ------------------------------------------------------------------------ *
 * $Id: oci_loader.h, v 3.2.0 2009/04/20 00:00 Vince $
 * ------------------------------------------------------------------------ */

#ifndef OCILIB_OCI_LOADER_H_INCLUDED 
#define OCILIB_OCI_LOADER_H_INCLUDED 


#if defined(_AIX)
  #define  LIB_OPEN_FLAGS        RTLD_NOW | RTLD_GLOBAL | RTLD_MEMBER
#elif defined(__hpux)           
  #define  LIB_OPEN_FLAGS        BIND_DEFERRED |BIND_VERBOSE| DYNAMIC_PATH
#else
  #define  LIB_OPEN_FLAGS        RTLD_NOW | RTLD_GLOBAL
#endif

#if defined(_WINDOWS)

  #include <windows.h>

  #define LIB_HANDLE               HMODULE
  #define LIB_OPEN(l)              LoadLibraryA(l)
  #define LIB_CLOSE                FreeLibrary
  #define LIB_SYMBOL(h, s, p, t)   p = (t) GetProcAddress(h, s)
 
#elif defined(__hpux)

  #include <dl.h>

  #define LIB_HANDLE               shl_t
  #define LIB_OPEN(l)              shl_load(l, LIB_OPEN_FLAGS , 0L)
  #define LIB_CLOSE                shl_unload
  #define LIB_SYMBOL(h, s, p, t)   shl_findsym(&h, s, (short) TYPE_PROCEDURE, (void *) &p)

#else

  #ifdef HAVE_DLFCN_H
    #include <dlfcn.h>
  #endif

  #define LIB_HANDLE               void *
  #define LIB_OPEN(l)              dlopen(l, LIB_OPEN_FLAGS)
  #define LIB_CLOSE                dlclose
  #define LIB_SYMBOL(h, s, p, t)   p = (t) dlsym(h, s)

#endif

#endif    /* OCILIB_OCI_LOADER_H_INCLUDED */

