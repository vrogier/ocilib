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

#ifndef OCILIB_ERROR_H_INCLUDED
#define OCILIB_ERROR_H_INCLUDED

#include "types.h"

OCI_Error * ErrorCreate
(
    void
);

void ErrorFree
(
    OCI_Error *err
);

void ErrorReset
(
    OCI_Error *err
);

void ErrorSet
(
    OCI_Error   *err,
    unsigned int type,
    int          code,
    void        *source_ptr,
    unsigned int source_type,
    const char  *location,
    otext       *message,
    unsigned int row
);

void ErrorResetSource
(
    OCI_Error* err,
    void* source_ptr
);

OCI_Error * ErrorGet
(
    boolean check_state,
    boolean reset_err
);

const otext* ErrorGetString
(
    OCI_Error* err
);

unsigned int ErrorGetType
(
    OCI_Error* err
);

int ErrorGetOCICode
(
    OCI_Error* err
);

int ErrorGetInternalCode
(
    OCI_Error* err
);

void* ErrorGetSource
(
    OCI_Error* err
);

unsigned int ErrorGetSourceType
(
    OCI_Error* err
);

const otext* ErrorGetLocation
(
    OCI_Error* err
);

OCI_Connection* ErrorGetConnection
(
    OCI_Error* err
);

OCI_Statement* ErrorGetStatement
(
    OCI_Error* err
);

unsigned int ErrorGetRow
(
    OCI_Error* err
);

#endif /* OCILIB_ERROR_H_INCLUDED */
