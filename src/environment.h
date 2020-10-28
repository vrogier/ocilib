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

#ifndef OCILIB_ENVIRONMENT_H_INCLUDED
#define OCILIB_ENVIRONMENT_H_INCLUDED

#include "types.h"

OCI_SYM_LOCAL char* OcilibEnvironmentGetVariable
(
    const char* name
);

OCI_SYM_LOCAL OCI_Error* OcilibEnvironmentGetLastError
(
    void
);

OCI_SYM_LOCAL boolean OcilibEnvironmentInitialize
(
    POCI_ERROR   err_handler,
    const otext* lib_path,
    unsigned int mode
);

OCI_SYM_LOCAL boolean OcilibEnvironmentCleanup
(
    void
);

OCI_SYM_LOCAL unsigned int OcilibEnvironmentGetOCICompileVersion
(
    void
);

OCI_SYM_LOCAL unsigned int OcilibEnvironmentGetOCIRuntimeVersion
(
    void
);

OCI_SYM_LOCAL unsigned int OcilibEnvironmentGetOCIRuntimeImportMode
(
    void
);

OCI_SYM_LOCAL unsigned int OcilibEnvironmentGetCharset
(
    void
);

OCI_SYM_LOCAL big_uint OcilibEnvironmentGetAllocatedBytes
(
    unsigned int mem_type
);

OCI_SYM_LOCAL boolean OcilibEnvironmentEnableWarnings
(
    boolean value
);

OCI_SYM_LOCAL boolean OcilibEnvironmentSetErrorHandler
(
    POCI_ERROR handler
);

OCI_SYM_LOCAL boolean OcilibEnvironmentSetHAHandler
(
    POCI_HA_HANDLER handler
);

OCI_SYM_LOCAL boolean OcilibEnvironmentSetFormat
(
    OCI_Connection* con,
    unsigned int    type,
    const otext   * format
);

OCI_SYM_LOCAL const otext* OcilibEnvironmentGetFormat
(
    OCI_Connection* con,
    unsigned int    type
);

#endif /* OCILIB_ENVIRONMENT_H_INCLUDED */
