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

OCI_Error* ExceptionGetError
(
    void
);

void ExceptionOCI
(
    OCI_Context* ctx,
    OCIError *oci_err,
    sword call_ret
);

void ExceptionMemory
(
    OCI_Context* ctx,
    int             type,
    size_t          nb_bytes
);

void ExceptionNotAvailable
(
    OCI_Context* ctx,
    int             feature
);

void ExceptionNullPointer
(
    OCI_Context* ctx,
    int type
);

void ExceptionDatatypeNotSupported
(
    OCI_Context* ctx,
    int             code
);

void ExceptionParsingToken
(
    OCI_Context* ctx,
    otext           token
);

void ExceptionMappingArgument
(
    OCI_Context* ctx,
    int             arg
);

void ExceptionNotInitialized
(
    OCI_Context* ctx
);

void ExceptionLoadingSharedLib
(
    OCI_Context* ctx
);

void ExceptionLoadingSymbols
(
    OCI_Context* ctx
);

void ExceptionNotMultithreaded
(
    OCI_Context* ctx
);

void ExceptionOutOfBounds
(
    OCI_Context* ctx,
    int             value
);

void ExceptionUnfreedData
(
    OCI_Context* ctx,
    int type_elem,
    int nb_elem
);

void ExceptionMaxBind
(
    OCI_Context* ctx
);

void ExceptionAttributeNotFound
(
    OCI_Context* ctx,
    const otext   * attr
);

void ExceptionMinimumValue
(
    OCI_Context* ctx,
    int             min
);

void ExceptionTypeNotCompatible
(
    OCI_Context* ctx
);

void ExceptionStatementState
(
    OCI_Context* ctx,
    int            state
);

void ExceptionStatementNotScrollable
(
    OCI_Context* ctx
);

void ExceptionBindAlreadyUsed
(
    OCI_Context* ctx,
    const otext  * bind
);

void ExceptionBindArraySize
(
    OCI_Context* ctx,
    unsigned int   maxsize,
    unsigned int   cursize,
    unsigned int   newsize
);

void ExceptionDirPathColNotFound
(
    OCI_Context* ctx,
    const otext* column,
    const otext* table
);

void ExceptionDirPathState
(
    OCI_Context* ctx,
    int          state
);

void ExceptionOCIEnvironment
(
    OCI_Context* ctx
);

void ExceptionRebindBadDatatype
(
    OCI_Context* ctx,
    const otext  * bind
);

void ExceptionTypeInfoWrongType
(
    OCI_Context* ctx,
    const otext   * name
);

void ExceptionItemNotFound
(
    OCI_Context* ctx,
    const otext   * name,
    unsigned int    type
);

void ExceptionArgInvalidValue
(
    OCI_Context* ctx,
    const otext   * name,
    unsigned int    value
);

void ExceptionEnvFromXaString
(
    OCI_Context* ctx,
    const otext* value
);

void ExceptionConnFromXaString
(
    OCI_Context* ctx,
    const otext* value
);

void ExceptionExternalBindingNotAllowed
(
    OCI_Context* ctx,
    const otext  * bind
);

#endif /* OCILIB_EXCEPTION_H_INCLUDED */
