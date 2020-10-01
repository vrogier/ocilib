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

#ifndef OCILIB_REF_H_INCLUDED
#define OCILIB_REF_H_INCLUDED

#include "types.h"

OCI_Ref * OcilibReferenceInitialize
(
    OCI_Connection *con,
    OCI_TypeInfo   *typinf,
    OCI_Ref        *ref,
    void           *handle
);

boolean OcilibReferencePin
(
    OCI_Ref* ref
);

boolean OcilibReferenceUnpin
(
    OCI_Ref* ref
);

OCI_Ref * OcilibReferenceCreate
(
    OCI_Connection *con,
    OCI_TypeInfo   *typinf
);

boolean OcilibReferenceFree
(
    OCI_Ref *ref
);

OCI_Ref ** OcilibReferenceCreateArray
(
    OCI_Connection *con,
    OCI_TypeInfo   *typinf,
    unsigned int    nbelem
);

boolean OcilibReferenceFreeArray
(
    OCI_Ref **refs
);

OCI_Object * OcilibReferenceGetObject
(
    OCI_Ref *ref
);

boolean OcilibReferenceAssign
(
    OCI_Ref *ref,
    OCI_Ref *ref_src
);

boolean OcilibReferenceIsNull
(
    OCI_Ref *ref
);

boolean OcilibReferenceSetNull
(
    OCI_Ref *ref
);

boolean OcilibReferenceToString
(
    OCI_Ref     *ref,
    unsigned int size,
    otext       *str
);

unsigned int OcilibReferenceGetHexSize
(
    OCI_Ref *ref
);

OCI_TypeInfo * OcilibReferenceGetTypeInfo
(
    OCI_Ref *ref
);

#endif /* OCILIB_REF_H_INCLUDED */
