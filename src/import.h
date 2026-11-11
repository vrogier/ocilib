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

#ifndef OCILIB_OCI_IMPORT_H_INCLUDED
#define OCILIB_OCI_IMPORT_H_INCLUDED

#ifdef OCI_IMPORT_LINKAGE

/* this is necessary because some Oracle headers do NOT include C++ checks !
   By example, the file orid.h is not protected !
*/

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

  #include <oci.h>
  #include <orid.h>
  #include <oci8dp.h>

  #if defined(OCI_MAJOR_VERSION) && (OCI_MAJOR_VERSION >= 21)
    #include <ocijson.h>
    #include <jzntypes.h>
    #include <jzndom.h>
  #endif

/* the Json DOM API public names changed between OCI versions:
     map the legacy names used in the code to the newer ones      */
  #if defined(JsonDomCreateObject) && !defined(JsonDomNewObject)
    #define JsonDomNewObject(jd, sz)        JsonDomCreateObject(jd)
    #define JsonDomNewArray(jd, sz)         JsonDomCreateArray(jd)
    #define JsonDomNewScalar(jd, sv)        JsonDomCreateScalar(jd, sv)
    #define JsonDomNewScalarVal             JsonDomCreateScalarVal
    #define JsonDomPutFieldValue            JsonDomSetField
    #define JsonDomAppendItem               JsonDomAppendElement
    #define JsonDomPutItem(jd, ar, ix, nd)  JsonDomAddElement(jd, ar, nd, ix)
  #endif

/* the real jzndom.h exposes the same jznDomDoc method table but with
     different public macro names/arity than the vendored oci/jsondom.h.
     Map the helper forms used by OCILIB onto the underlying jznDomAPI
     methods so the same call sites compile in both import modes.       */
  #if defined(JsonDomGetFields)
    #undef JsonDomGetError
    #define JsonDomGetError(jd, jerr, clr) \
            ((jd)->mthds_jznDomDoc->jznDomGetError((jd), (jerr), (clr)))
    #undef JsonDomInsertElements
    #define JsonDomInsertElements(jd, ary, pos, node) \
            ((jd)->mthds_jznDomDoc->jznDomPutItem((jd), (ary), (node), (pos)))
    #if !defined(JsonDomDeleteItem)
      #define JsonDomDeleteItem(jd, ary, pos) \
              ((jd)->mthds_jznDomDoc->jznDomDeleteItem((jd), (ary), (pos)))
    #endif
    #if !defined(JsonDomDeleteItems)
      #define JsonDomDeleteItems(jd, ary, pos, num) \
              ((jd)->mthds_jznDomDoc->jznDomDeleteItemBatch((jd), (ary), (pos), (num)))
    #endif
    #if !defined(JsonDomGetFieldPos)
      #define JsonDomGetFieldPos(jd, obj, fname, fnlen) \
              ((jd)->mthds_jznDomDoc->jznDomGetFieldByName((jd), (obj), (fname), (fnlen)))
    #endif
    #if !defined(JsonDomDocFree)
      #define JsonDomDocFree(jd) \
              ((jd)->mthds_jznDomDoc->jznDomDocFree((jd)))
    #endif
  #endif

#ifdef  __cplusplus
}
#endif

  #ifdef _MSC_VER
    #pragma comment(lib, "oci.lib")
  #endif

#else

  #include "loader.h"

/* Setup Oracle shared library name if not provided */

  #ifndef OCI_DL
    #if defined(_WINDOWS)
#define OCI_DL                   oci.dll
    #elif defined(__APPLE__)
#define OCI_DL                   libclntsh.dylib
    #elif defined(__hppa)
#define OCI_DL                   libclntsh.sl
    #else
#define OCI_DL                   libclntsh.so
    #endif
  #endif

/* ANSI string version of Oracle shared lib */

#define OCI_DL_ANSI_GET(s) OCI_DL_ANSI_CVT(s)
#define OCI_DL_ANSI_CVT(s)  # s
#define OCI_DL_ANSI_NAME OCI_DL_ANSI_GET(OCI_DL)

/* Meta string version of Oracle shared lib */

#define OCI_DL_GET(s) OCI_DL_CVT(s)
#define OCI_DL_CVT(s) OTEXT(# s)
#define OCI_DL_NAME   OCI_DL_GET(OCI_DL)

/* symbol list */

#define OCIDateGetTime(date, hour, min, sec)         \
        {                                            \
            *(hour) = (date)->OCIDateTime.OCITimeHH; \
            *(min)  = (date)->OCIDateTime.OCITimeMI; \
            *(sec)  = (date)->OCIDateTime.OCITimeSS; \
        }

#define OCIDateGetDate(date, year, month, day) \
        {                                      \
            *(year)  = (date)->OCIDateYYYY;    \
            *(month) = (date)->OCIDateMM;      \
            *(day)   = (date)->OCIDateDD;      \
        }

#define OCIDateSetTime(date, hour, min, sec)      \
        {                                         \
            (date)->OCIDateTime.OCITimeHH = hour; \
            (date)->OCIDateTime.OCITimeMI = min;  \
            (date)->OCIDateTime.OCITimeSS = sec;  \
        }

#define OCIDateSetDate(date, year, month, day) \
        {                                      \
            (date)->OCIDateYYYY = year;        \
            (date)->OCIDateMM   = month;       \
            (date)->OCIDateDD   = day;         \
        }

#endif

#include "oci/api.h"

#endif    /* OCILIB_OCI_IMPORT_H_INCLUDED */