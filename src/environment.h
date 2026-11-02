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

#ifndef OCILIB_ENVIRONMENT_H_INCLUDED
#define OCILIB_ENVIRONMENT_H_INCLUDED

#include "types.h"

OCI_SYM_LOCAL char* OcilibEnvironmentGetVariable
(
    OCI_Environment *env,
    const char      *name
);

OCI_SYM_LOCAL OCI_Environment * OcilibEnvironmentCreate
(
    POCI_ERROR   err_handler,
    const otext *lib_path,
    OCI_EnvMode  mode
);

OCI_SYM_LOCAL OCI_Boolean OcilibEnvironmentFree
(
    OCI_Environment *env
);

OCI_SYM_LOCAL OCI_UInt32 OcilibEnvironmentGetOCICompileVersion
(
    OCI_Environment *env
);

OCI_SYM_LOCAL OCI_UInt32 OcilibEnvironmentGetOCIRuntimeVersion
(
    OCI_Environment *env
);

OCI_SYM_LOCAL OCI_ImportMode OcilibEnvironmentGetOCIRuntimeImportMode
(
    OCI_Environment *env
);

OCI_SYM_LOCAL OCI_Charset OcilibEnvironmentGetCharset
(
    OCI_Environment *env
);

OCI_SYM_LOCAL OCI_UInt64 OcilibEnvironmentGetStats
(
    OCI_Environment *env,
    OCI_Stat         stat
);

OCI_SYM_LOCAL OCI_Boolean OcilibEnvironmentEnableWarnings
(
    OCI_Environment *env,
    OCI_Boolean      value
);

OCI_SYM_LOCAL OCI_Boolean OcilibEnvironmentSetErrorHandler
(
    OCI_Environment *env,
    POCI_ERROR       handler
);

OCI_SYM_LOCAL OCI_Boolean OcilibEnvironmentSetHAHandler
(
    OCI_Environment *env,
    POCI_HA_HANDLER  handler
);

OCI_SYM_LOCAL OCI_Boolean OcilibEnvironmentSetSubscriptionPort
(
    OCI_Environment *env,
    OCI_UInt32     port
);

OCI_SYM_LOCAL OCI_UInt32 OcilibEnvironmentGetSubscriptionPort
(
    OCI_Environment *env
);

OCI_SYM_LOCAL const otext * OcilibGetLocaleString
(
    OCI_Environment *env,
    OCI_LocaleCode   code
);

OCI_SYM_LOCAL OCI_Boolean OcilibEnvironmentSetFormat
(
    OCI_Environment *env,
    OCI_FormatType   type,
    const otext    * format
);

OCI_SYM_LOCAL const otext* OcilibEnvironmentGetFormat
(
    OCI_Environment *env,
    OCI_FormatType   type
);

#endif /* OCILIB_ENVIRONMENT_H_INCLUDED */
