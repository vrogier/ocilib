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

#ifndef OCILIB_COLLECTION_H_INCLUDED
#define OCILIB_COLLECTION_H_INCLUDED

#include "types.h"

OCI_Coll * CollectionInitialize
(
    OCI_Connection *con,
    OCI_Coll       *Coll,
    void           *handle,
    OCI_TypeInfo   *typeinf
);

OCI_Coll* CollectionCreate
(
    OCI_TypeInfo* typinf
);

boolean CollectionFree
(
    OCI_Coll* coll
);

OCI_Coll** CollectionCreateArray
(
    OCI_Connection* con,
    OCI_TypeInfo  * typinf,
    unsigned int    nbelem
);

boolean CollectionFreeArray
(
    OCI_Coll** colls
);

boolean CollectionAssign
(
    OCI_Coll* coll,
    OCI_Coll* coll_src
);

boolean CollectionTrim
(
    OCI_Coll   * coll,
    unsigned int nb_elem
);

boolean CollectionClear
(
    OCI_Coll* coll
);

unsigned int CollectionGetType
(
    OCI_Coll* coll
);

unsigned int CollectionGetMax
(
    OCI_Coll* coll
);

unsigned int CollectionGetSize
(
    OCI_Coll* coll
);

unsigned int CollectionGetCount
(
    OCI_Coll* coll
);

OCI_Elem* CollectionGetElement
(
    OCI_Coll   * coll,
    unsigned int index
);

boolean CollectionGetElement2
(
    OCI_Coll   * coll,
    unsigned int index,
    OCI_Elem   * elem
);

boolean CollectionSetElement
(
    OCI_Coll   * coll,
    unsigned int index,
    OCI_Elem   * elem
);

boolean CollectionAddElement
(
    OCI_Coll* coll,
    OCI_Elem* elem
);

boolean CollectionRemoveElement
(
    OCI_Coll   * coll,
    unsigned int index
);

OCI_TypeInfo* CollectionGetTypeInfo
(
    OCI_Coll* coll
);

boolean CollectionToString
(
    OCI_Coll    * coll,
    unsigned int* size,
    otext       * str
);

#endif /* OCILIB_COLLECTION_H_INCLUDED */