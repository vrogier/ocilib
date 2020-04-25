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
    OCIError      * p_err,
    OCI_Connection* con,
    OCI_Statement * stmt,
    boolean         warning
);

void ExceptionMemory
(
    int             type,
    size_t          nb_bytes,
    OCI_Connection* con,
    OCI_Statement * stmt
);

void ExceptionNotAvailable
(
    OCI_Connection* con,
    int             feature
);

void ExceptionNullPointer
(
    int type
);

void ExceptionDatatypeNotSupported
(
    OCI_Connection* con,
    OCI_Statement * stmt,
    int             code
);

void ExceptionParsingToken
(
    OCI_Connection* con,
    OCI_Statement * stmt,
    otext           token
);

void ExceptionMappingArgument
(
    OCI_Connection* con,
    OCI_Statement * stmt,
    int             arg
);

void ExceptionNotInitialized
(
    void
);

void ExceptionLoadingSharedLib
(
    void
);

void ExceptionLoadingSymbols
(
    void
);

void ExceptionNotMultithreaded
(
    void
);

void ExceptionOutOfBounds
(
    OCI_Connection* con,
    int             value
);

void ExceptionUnfreedData
(
    int type_elem,
    int nb_elem
);

void ExceptionMaxBind
(
    OCI_Statement* stmt
);

void ExceptionAttributeNotFound
(
    OCI_Connection* con,
    const otext   * attr
);

void ExceptionMinimumValue
(
    OCI_Connection* con,
    OCI_Statement * stmt,
    int             min
);

void ExceptionTypeNotCompatible
(
    OCI_Connection* con
);

void ExceptionStatementState
(
    OCI_Statement* stmt,
    int            state
);

void ExceptionStatementNotScrollable
(
    OCI_Statement* stmt
);

void ExceptionBindAlreadyUsed
(
    OCI_Statement* stmt,
    const otext  * bind
);

void ExceptionBindArraySize
(
    OCI_Statement* stmt,
    unsigned int   maxsize,
    unsigned int   cursize,
    unsigned int   newsize
);

void ExceptionDirPathColNotFound
(
    OCI_DirPath* dp,
    const otext* column,
    const otext* table
);

void ExceptionDirPathState
(
    OCI_DirPath* dp,
    int          state
);

void ExceptionOCIEnvironment
(
    void
);

void ExceptionRebindBadDatatype
(
    OCI_Statement* stmt,
    const otext  * bind
);

void ExceptionTypeInfoWrongType
(
    OCI_Connection* con,
    const otext   * name
);

void ExceptionItemNotFound
(
    OCI_Connection* con,
    OCI_Statement * stmt,
    const otext   * name,
    unsigned int    type
);

void ExceptionArgInvalidValue
(
    OCI_Connection* con,
    OCI_Statement * stmt,
    const otext   * name,
    unsigned int    value
);

void ExceptionEnvFromXaString
(
    const otext* value
);

void ExceptionConnFromXaString
(
    const otext* value
);

void ExceptionExternalBindingNotAllowed
(
    OCI_Statement* stmt,
    const otext  * bind
);

#endif /* OCILIB_EXCEPTION_H_INCLUDED */
