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

#ifndef OCILIB_EXCEPTION_H_INCLUDED
#define OCILIB_EXCEPTION_H_INCLUDED

#include "types.h"

OCI_Error* OcilibExceptionGetError
(
    void
);

void OcilibExceptionOCI
(
    OCI_Context* ctx,
    OCIError    *oci_err,
    sword        call_ret
);

void OcilibExceptionMemory
(
    OCI_Context* ctx,
    int          type,
    size_t       nb_bytes
);

void OcilibExceptionNotAvailable
(
    OCI_Context* ctx,
    int          feature
);

void OcilibExceptionNullPointer
(
    OCI_Context* ctx,
    int          type
);

void OcilibExceptionDatatypeNotSupported
(
    OCI_Context* ctx,
    int          code
);

void OcilibExceptionParsingToken
(
    OCI_Context* ctx,
    otext        token
);

void OcilibExceptionMappingArgument
(
    OCI_Context* ctx,
    int          arg
);

void OcilibExceptionNotInitialized
(
    OCI_Context* ctx
);

void OcilibExceptionLoadingSharedLib
(
    OCI_Context* ctx
);

void OcilibExceptionLoadingSymbols
(
    OCI_Context* ctx
);

void OcilibExceptionNotMultithreaded
(
    OCI_Context* ctx
);

void OcilibExceptionOutOfBounds
(
    OCI_Context* ctx,
    int          value
);

void OcilibExceptionUnfreedData
(
    OCI_Context* ctx,
    int          type_elem,
    int          nb_elem
);

void OcilibExceptionUnfreedBytes
(
    OCI_Context* ctx,
    big_uint     nb_bytes
);

void OcilibExceptionMaxBind
(
    OCI_Context* ctx
);

void OcilibExceptionAttributeNotFound
(
    OCI_Context * ctx,
    const otext * attr
);

void OcilibExceptionMinimumValue
(
    OCI_Context* ctx,
    int          min
);

void OcilibExceptionTypeNotCompatible
(
    OCI_Context* ctx
);

void OcilibExceptionStatementState
(
    OCI_Context* ctx,
    int          state
);

void OcilibExceptionStatementNotScrollable
(
    OCI_Context* ctx
);

void OcilibExceptionBindAlreadyUsed
(
    OCI_Context * ctx,
    const otext * bind
);

void OcilibExceptionBindArraySize
(
    OCI_Context* ctx,
    unsigned int maxsize,
    unsigned int cursize,
    unsigned int newsize
);

void OcilibExceptionDirPathColNotFound
(
    OCI_Context* ctx,
    const otext* column,
    const otext* table
);

void OcilibExceptionDirPathState
(
    OCI_Context* ctx,
    int          state
);

void OcilibExceptionOCIEnvironment
(
    OCI_Context* ctx
);

void OcilibExceptionRebindBadDatatype
(
    OCI_Context * ctx,
    const otext * bind
);

void OcilibExceptionTypeInfoWrongType
(
    OCI_Context * ctx,
    const otext * name
);

void OcilibExceptionItemNotFound
(
    OCI_Context * ctx,
    const otext * name,
    unsigned int  type
);

void OcilibExceptionArgInvalidValue
(
    OCI_Context * ctx,
    const otext * name,
    unsigned int  value
);

void OcilibExceptionEnvFromXaString
(
    OCI_Context* ctx,
    const otext* value
);

void OcilibExceptionConnFromXaString
(
    OCI_Context* ctx,
    const otext* value
);

void OcilibExceptionExternalBindingNotAllowed
(
    OCI_Context * ctx,
    const otext * bind
);

#endif /* OCILIB_EXCEPTION_H_INCLUDED */
