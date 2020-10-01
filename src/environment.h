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

char* OcilibEnvironmentGetVariable
(
    const char* name
);

OCI_Error* OcilibEnvironmentGetLastError
(
    void
);

boolean OcilibEnvironmentInitialize
(
    POCI_ERROR   err_handler,
    const otext* lib_path,
    unsigned int mode
);

boolean OcilibEnvironmentCleanup
(
    void
);

unsigned int OcilibEnvironmentGetOCICompileVersion
(
    void
);

unsigned int OcilibEnvironmentGetOCIRuntimeVersion
(
    void
);

unsigned int OcilibEnvironmentGetOCIRuntimeImportMode
(
    void
);

unsigned int OcilibEnvironmentGetCharset
(
    void
);

big_uint OcilibEnvironmentGetAllocatedBytes
(
    unsigned int mem_type
);

boolean OcilibEnvironmentEnableWarnings
(
    boolean value
);

boolean OcilibEnvironmentSetErrorHandler
(
    POCI_ERROR handler
);

boolean OcilibEnvironmentSetHAHandler
(
    POCI_HA_HANDLER handler
);

boolean OcilibEnvironmentSetFormat
(
    OCI_Connection* con,
    unsigned int    type,
    const otext   * format
);

const otext* OcilibEnvironmentGetFormat
(
    OCI_Connection* con,
    unsigned int    type
);

#endif /* OCILIB_ENVIRONMENT_H_INCLUDED */
