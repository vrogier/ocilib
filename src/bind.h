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

#ifndef OCILIB_BIND_H_INCLUDED
#define OCILIB_BIND_H_INCLUDED

#include "types.h"

OCI_Bind* BindCreate
(
    OCI_Context* ctx,
    OCI_Statement* stmt,
    void* data,
    const otext* name,
    unsigned int   mode,
    ub4            size,
    ub1            type,
    unsigned int   code,
    unsigned int   subtype,
    OCI_TypeInfo* typinf,
    unsigned int   nbelem
);

boolean BindFree
(
    OCI_Bind* bnd
);

boolean BindAllocData
(
    OCI_Bind* bnd
);

int BindGetInternalIndex
(
    OCI_Statement* stmt,
    const otext* name
);

boolean BindSetNullIndicator
(
    OCI_Bind* bnd,
    unsigned int position,
    sb2          value
);

const otext* BindGetName
(
    OCI_Bind* bnd
);

unsigned int BindGetType
(
    OCI_Bind* bnd
);

unsigned int BindGetSubtype
(
    OCI_Bind* bnd
);

unsigned int BindGetDataCount
(
    OCI_Bind* bnd
);

void* BindGetData
(
    OCI_Bind* bnd
);

OCI_Statement* BindGetStatement
(
    OCI_Bind* bnd
);

boolean BindSetDataSize
(
    OCI_Bind* bnd,
    unsigned int size
);

boolean BindSetDataSizeAtPos
(
    OCI_Bind* bnd,
    unsigned int position,
    unsigned int size
);

unsigned int BindGetDataSize
(
    OCI_Bind* bnd
);

unsigned int BindGetDataSizeAtPos
(
    OCI_Bind* bnd,
    unsigned int position
);

boolean BindSetNullAtPos
(
    OCI_Bind* bnd,
    unsigned int position
);

boolean BindSetNull
(
    OCI_Bind* bnd
);

boolean BindSetNotNullAtPos
(
    OCI_Bind* bnd,
    unsigned int position
);

boolean BindSetNotNull
(
    OCI_Bind* bnd
);

boolean BindIsNullAtPos
(
    OCI_Bind* bnd,
    unsigned int position
);

boolean BindIsNull
(
    OCI_Bind* bnd
);

boolean BindSetCharsetForm
(
    OCI_Bind *bnd,
    unsigned int csfrm
);

boolean BindSetDirection
(
    OCI_Bind *bnd,
    unsigned int direction
);

unsigned int BindGetDirection
(
    OCI_Bind* bnd
);

unsigned int BindGetAllocationMode
(
    OCI_Bind* bnd
);


#endif /* OCILIB_BIND_H_INCLUDED */
