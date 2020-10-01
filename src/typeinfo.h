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

#ifndef OCILIB_TYPEINFO_H_INCLUDED
#define OCILIB_TYPEINFO_H_INCLUDED

#include "types.h"

boolean TypeInfoDispose
(
    OCI_TypeInfo* typinf
);

OCI_TypeInfo * OcilibTypeInfoGet
(
    OCI_Connection *con,
    const otext    *name,
    unsigned int    type
);

boolean OcilibTypeInfoFree
(
    OCI_TypeInfo *typinf
);

unsigned int OcilibTypeInfoGetType
(
    OCI_TypeInfo *typinf
);

OCI_Connection * OcilibTypeInfoGetConnection
(
    OCI_TypeInfo *typinf
);

unsigned int OcilibTypeInfoGetColumnCount
(
    OCI_TypeInfo *typinf
);

OCI_Column * OcilibTypeInfoGetColumn
(
    OCI_TypeInfo *typinf,
    unsigned int  index
);

const otext * OcilibTypeInfoGetName
(
    OCI_TypeInfo *typinf
);

boolean OcilibTypeInfoIsFinalType
(
    OCI_TypeInfo* typinf
);

OCI_TypeInfo* OcilibTypeInfoGetSuperType
(
    OCI_TypeInfo* typinf
);

#endif /* OCILIB_TYPEINFO_H_INCLUDED */
