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

OCI_Coll * CollInit
(
    OCI_Connection *con,
    OCI_Coll       *Coll,
    void           *handle,
    OCI_TypeInfo   *typeinf
);

OCI_Coll* CollCreate
(
    OCI_TypeInfo* typinf
);

boolean CollFree
(
    OCI_Coll* coll
);

OCI_Coll** CollArrayCreate
(
    OCI_Connection* con,
    OCI_TypeInfo* typinf,
    unsigned int    nbelem
);

boolean CollArrayFree
(
    OCI_Coll** colls
);

boolean CollAssign
(
    OCI_Coll* coll,
    OCI_Coll* coll_src
);

unsigned int CollGetType
(
    OCI_Coll* coll
);

unsigned int CollGetMax
(
    OCI_Coll* coll
);

unsigned int CollGetSize
(
    OCI_Coll* coll
);

boolean CollTrim
(
    OCI_Coll* coll,
    unsigned int  nb_elem
);

OCI_Elem* CollGetElem
(
    OCI_Coll* coll,
    unsigned int index
);

boolean CollGetElem2
(
    OCI_Coll* coll,
    unsigned int index,
    OCI_Elem* elem
);

boolean CollSetElem
(
    OCI_Coll* coll,
    unsigned int  index,
    OCI_Elem* elem
);

boolean CollAppend
(
    OCI_Coll* coll,
    OCI_Elem* elem
);

OCI_TypeInfo* CollGetTypeInfo
(
    OCI_Coll* coll
);

boolean CollClear
(
    OCI_Coll* coll
);

boolean CollDeleteElem
(
    OCI_Coll* coll,
    unsigned int index
);

unsigned int CollGetCount
(
    OCI_Coll* coll
);

boolean CollToText
(
    OCI_Coll* coll,
    unsigned int* size,
    otext* str
);

#endif /* OCILIB_COLLECTION_H_INCLUDED */