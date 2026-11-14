/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2026 Vincent ROGIER <vince.rogier@gmail.com>
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

#ifndef OCILIB_ERROR_H_INCLUDED
#define OCILIB_ERROR_H_INCLUDED

#include "types.h"

OCI_SYM_LOCAL OCI_Boolean OcilibErrorInitialize
(
    void
);

OCI_SYM_LOCAL OCI_Boolean OcilibErrorCleanup
(
    void
);

OCI_SYM_LOCAL OCI_Error * OcilibErrorCreate
(
    OCI_Environment *env
);

OCI_SYM_LOCAL void OcilibErrorFree
(
    OCI_Error *err
);

OCI_SYM_LOCAL void OcilibErrorReset
(
    OCI_Error *err
);

OCI_SYM_LOCAL void OcilibErrorSet
(
    OCI_Error        *err,
    OCI_ErrorType     type,
    OCI_ErrorCategory category,
    OCI_ErrorCode     code,
    void             *source_ptr,
    OCI_PointerType   source_type,
    const char       *location,
    otext            *message,
    OCI_UInt32        row
);

OCI_SYM_LOCAL void OcilibErrorResetSource
(
    OCI_Error* err,
    void     * source_ptr
);

OCI_SYM_LOCAL OCI_Error * OcilibErrorGet
(
    OCI_Environment *env,
    OCI_Boolean      check_state,
    OCI_Boolean      reset_err
);

OCI_SYM_LOCAL OCI_Error * OcilibErrorGetLastError
(
    void
);

OCI_SYM_LOCAL const otext* OcilibErrorGetString
(
    OCI_Error* err
);

OCI_SYM_LOCAL OCI_ErrorType OcilibErrorGetType
(
    OCI_Error* err
);

OCI_SYM_LOCAL OCI_ErrorCategory OcilibErrorGetCategory
(
    OCI_Error* err
);

OCI_SYM_LOCAL OCI_ErrorCode OcilibErrorGetCode
(
    OCI_Error* err
);

OCI_SYM_LOCAL void* OcilibErrorGetSource
(
    OCI_Error* err
);

OCI_SYM_LOCAL OCI_PointerType OcilibErrorGetSourceType
(
    OCI_Error* err
);

OCI_SYM_LOCAL const otext* OcilibErrorGetLocation
(
    OCI_Error* err
);

OCI_SYM_LOCAL OCI_Connection* OcilibErrorGetConnection
(
    OCI_Error* err
);

OCI_SYM_LOCAL OCI_Statement* OcilibErrorGetStatement
(
    OCI_Error* err
);

OCI_SYM_LOCAL OCI_Int32 OcilibErrorGetRow
(
    OCI_Error* err
);

#endif /* OCILIB_ERROR_H_INCLUDED */
