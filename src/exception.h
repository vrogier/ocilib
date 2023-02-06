/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2023 Vincent ROGIER <vince.rogier@ocilib.net>
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

#ifndef OCILIB_EXCEPTION_H_INCLUDED
#define OCILIB_EXCEPTION_H_INCLUDED

#include "types.h"

OCI_SYM_LOCAL OCI_Error* OcilibExceptionGetError
(
    void
);

OCI_SYM_LOCAL void OcilibExceptionOCI
(
    OCI_Context* ctx,
    OCIError    *oci_err,
    sword        call_ret
);

OCI_SYM_LOCAL void OcilibExceptionMemory
(
    OCI_Context* ctx,
    int          type,
    size_t       nb_bytes
);

OCI_SYM_LOCAL void OcilibExceptionNotAvailable
(
    OCI_Context* ctx,
    int          feature
);

OCI_SYM_LOCAL void OcilibExceptionNullPointer
(
    OCI_Context* ctx,
    int          type
);

OCI_SYM_LOCAL void OcilibExceptionDatatypeNotSupported
(
    OCI_Context* ctx,
    int          code
);

OCI_SYM_LOCAL void OcilibExceptionParsingToken
(
    OCI_Context* ctx,
    otext        token
);

OCI_SYM_LOCAL void OcilibExceptionMappingArgument
(
    OCI_Context* ctx,
    int          arg
);

OCI_SYM_LOCAL void OcilibExceptionNotInitialized
(
    OCI_Context* ctx
);

OCI_SYM_LOCAL void OcilibExceptionLoadingSharedLib
(
    OCI_Context* ctx
);

OCI_SYM_LOCAL void OcilibExceptionLoadingSymbols
(
    OCI_Context* ctx
);

OCI_SYM_LOCAL void OcilibExceptionNotMultithreaded
(
    OCI_Context* ctx
);

OCI_SYM_LOCAL void OcilibExceptionOutOfBounds
(
    OCI_Context* ctx,
    int          value
);

OCI_SYM_LOCAL void OcilibExceptionUnfreedData
(
    OCI_Context* ctx,
    int          type_elem,
    int          nb_elem
);

OCI_SYM_LOCAL void OcilibExceptionUnfreedBytes
(
    OCI_Context* ctx,
    big_uint     nb_bytes
);

OCI_SYM_LOCAL void OcilibExceptionMaxBind
(
    OCI_Context* ctx
);

OCI_SYM_LOCAL void OcilibExceptionAttributeNotFound
(
    OCI_Context * ctx,
    const otext * attr
);

OCI_SYM_LOCAL void OcilibExceptionMinimumValue
(
    OCI_Context* ctx,
    int          min
);

OCI_SYM_LOCAL void OcilibExceptionTypeNotCompatible
(
    OCI_Context* ctx
);

OCI_SYM_LOCAL void OcilibExceptionStatementState
(
    OCI_Context* ctx,
    int          state
);

OCI_SYM_LOCAL void OcilibExceptionStatementNotScrollable
(
    OCI_Context* ctx
);

OCI_SYM_LOCAL void OcilibExceptionBindAlreadyUsed
(
    OCI_Context * ctx,
    const otext * bind
);

OCI_SYM_LOCAL void OcilibExceptionBindArraySize
(
    OCI_Context* ctx,
    unsigned int maxsize,
    unsigned int cursize,
    unsigned int newsize
);

OCI_SYM_LOCAL void OcilibExceptionDirPathColNotFound
(
    OCI_Context* ctx,
    const otext* column,
    const otext* table
);

OCI_SYM_LOCAL void OcilibExceptionDirPathState
(
    OCI_Context* ctx,
    int          state
);

OCI_SYM_LOCAL void OcilibExceptionOCIEnvironment
(
    OCI_Context* ctx
);

OCI_SYM_LOCAL void OcilibExceptionRebindBadDatatype
(
    OCI_Context * ctx,
    const otext * bind
);

OCI_SYM_LOCAL void OcilibExceptionTypeInfoWrongType
(
    OCI_Context * ctx,
    const otext * name
);

OCI_SYM_LOCAL void OcilibExceptionItemNotFound
(
    OCI_Context * ctx,
    const otext * name,
    unsigned int  type
);

OCI_SYM_LOCAL void OcilibExceptionArgInvalidValue
(
    OCI_Context * ctx,
    const otext * name,
    unsigned int  value
);

OCI_SYM_LOCAL void OcilibExceptionEnvFromXaString
(
    OCI_Context* ctx,
    const otext* value
);

OCI_SYM_LOCAL void OcilibExceptionConnFromXaString
(
    OCI_Context* ctx,
    const otext* value
);

OCI_SYM_LOCAL void OcilibExceptionExternalBindingNotAllowed
(
    OCI_Context * ctx,
    const otext * bind
);

#endif /* OCILIB_EXCEPTION_H_INCLUDED */
