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

#ifndef OCILIB_OCI_LOADER_H_INCLUDED
#define OCILIB_OCI_LOADER_H_INCLUDED

#if defined(_AIX)
  #define  LIB_OPEN_FLAGS        (RTLD_NOW | RTLD_GLOBAL | RTLD_MEMBER)
#elif defined(__hpux)
  #define  LIB_OPEN_FLAGS        (BIND_DEFERRED |BIND_VERBOSE| DYNAMIC_PATH)
#else
  #define  LIB_OPEN_FLAGS        (RTLD_NOW | RTLD_GLOBAL)
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
  #define LIB_OPEN(l)              shl_load(l, LIB_OPEN_FLAGS, 0L)
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

