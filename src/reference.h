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

#ifndef OCILIB_REF_H_INCLUDED
#define OCILIB_REF_H_INCLUDED

#include "types.h"

OCI_SYM_LOCAL OCI_Ref * OcilibReferenceInitialize
(
    OCI_Connection *con,
    OCI_TypeInfo   *typinf,
    OCI_Ref        *ref,
    void           *handle
);

OCI_SYM_LOCAL boolean OcilibReferencePin
(
    OCI_Ref* ref
);

OCI_SYM_LOCAL boolean OcilibReferenceUnpin
(
    OCI_Ref* ref
);

OCI_SYM_LOCAL OCI_Ref * OcilibReferenceCreate
(
    OCI_Connection *con,
    OCI_TypeInfo   *typinf
);

OCI_SYM_LOCAL boolean OcilibReferenceFree
(
    OCI_Ref *ref
);

OCI_SYM_LOCAL OCI_Ref ** OcilibReferenceCreateArray
(
    OCI_Connection *con,
    OCI_TypeInfo   *typinf,
    unsigned int    nbelem
);

OCI_SYM_LOCAL boolean OcilibReferenceFreeArray
(
    OCI_Ref **refs
);

OCI_SYM_LOCAL OCI_Object * OcilibReferenceGetObject
(
    OCI_Ref *ref
);

OCI_SYM_LOCAL boolean OcilibReferenceAssign
(
    OCI_Ref *ref,
    OCI_Ref *ref_src
);

OCI_SYM_LOCAL boolean OcilibReferenceIsNull
(
    OCI_Ref *ref
);

OCI_SYM_LOCAL boolean OcilibReferenceSetNull
(
    OCI_Ref *ref
);

OCI_SYM_LOCAL boolean OcilibReferenceToString
(
    OCI_Ref     *ref,
    unsigned int size,
    otext       *str
);

OCI_SYM_LOCAL unsigned int OcilibReferenceGetHexSize
(
    OCI_Ref *ref
);

OCI_SYM_LOCAL OCI_TypeInfo * OcilibReferenceGetTypeInfo
(
    OCI_Ref *ref
);

#endif /* OCILIB_REF_H_INCLUDED */
