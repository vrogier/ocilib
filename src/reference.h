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

#ifndef OCILIB_REF_H_INCLUDED
#define OCILIB_REF_H_INCLUDED

#include "types.h"

OCI_SYM_LOCAL OCI_Reference * OcilibReferenceInitialize
(
    OCI_Connection *con,
    OCI_TypeInfo   *typinf,
    OCI_Reference  *ref,
    void           *handle
);

OCI_SYM_LOCAL OCI_Boolean OcilibReferencePin
(
    OCI_Reference* ref
);

OCI_SYM_LOCAL OCI_Boolean OcilibReferenceUnpin
(
    OCI_Reference* ref
);

OCI_SYM_LOCAL OCI_Reference * OcilibReferenceCreate
(
    OCI_TypeInfo *typinf
);

OCI_SYM_LOCAL OCI_Boolean OcilibReferenceFree
(
    OCI_Reference *ref
);

OCI_SYM_LOCAL OCI_Reference ** OcilibReferenceArrayCreate
(
    OCI_TypeInfo *typinf,
    OCI_UInt32    nbelem
);

OCI_SYM_LOCAL OCI_Boolean OcilibReferenceArrayFree
(
    OCI_Reference **refs
);

OCI_SYM_LOCAL OCI_Object * OcilibReferenceGetObject
(
    OCI_Reference *ref
);

OCI_SYM_LOCAL OCI_Boolean OcilibReferenceAssign
(
    OCI_Reference *ref,
    OCI_Reference *ref_src
);

OCI_SYM_LOCAL OCI_Boolean OcilibReferenceIsNull
(
    OCI_Reference *ref
);

OCI_SYM_LOCAL OCI_Boolean OcilibReferenceSetNull
(
    OCI_Reference *ref
);

OCI_SYM_LOCAL OCI_Boolean OcilibReferenceToText
(
    OCI_Reference *ref,
    OCI_UInt32     size,
    otext         *str
);

OCI_SYM_LOCAL OCI_UInt32 OcilibReferenceGetHexSize
(
    OCI_Reference *ref
);

OCI_SYM_LOCAL OCI_TypeInfo * OcilibReferenceGetTypeInfo
(
    OCI_Reference *ref
);

#endif /* OCILIB_REF_H_INCLUDED */
