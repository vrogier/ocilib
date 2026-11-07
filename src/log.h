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

#ifndef OCILIB_LOG_H_INCLUDED
#define OCILIB_LOG_H_INCLUDED

#include "types.h"

/* Fast-path gate used by the LOG_ENABLED() macro in macros.h */

OCI_SYM_LOCAL OCI_Boolean OcilibLogIsEnabled
(
    OCI_LogCategory category,
    OCI_LogLevel    level
);

OCI_SYM_LOCAL OCI_Boolean OcilibLogInitialize
(
    void
);

OCI_SYM_LOCAL OCI_Boolean OcilibLogCleanup
(
    void
);

OCI_SYM_LOCAL void OcilibLogWrite
(
    OCI_LogCategory category,
    OCI_LogLevel    level,
    const char     *step,
    const char     *function,
    const otext    *format,
    ...
) OCI_ATTR_FORMAT(5, 6);

/* internal helpers exposed for unit testing (not part of the public API) */

OCI_SYM_LOCAL const char * OcilibLogGetLevelName
(
    OCI_LogLevel level
);

OCI_SYM_LOCAL const char * OcilibLogGetCategoryName
(
    OCI_LogCategory category
);

OCI_SYM_LOCAL OCI_Boolean OcilibLogStrEquals
(
    const char *value,
    const char *name
);

OCI_SYM_LOCAL OCI_Int32 OcilibLogCodepointToUTF8
(
    OCI_UInt32 code,
    char      *dst,
    OCI_Int32  dst_size
);

OCI_SYM_LOCAL OCI_Int32 OcilibLogNativeToUTF8
(
    const otext *src,
    char        *dst,
    OCI_Int32    dst_size
);

#endif    /* OCILIB_LOG_H_INCLUDED */
