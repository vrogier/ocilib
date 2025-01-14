/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2025 Vincent ROGIER <vince.rogier@ocilib.net>
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

OCI_SYM_LOCAL OCI_Coll * OcilibCollectionInitialize
(
    OCI_Connection *con,
    OCI_Coll       *Coll,
    void           *handle,
    OCI_TypeInfo   *typeinf
);

OCI_SYM_LOCAL OCI_Coll* OcilibCollectionCreate
(
    OCI_TypeInfo* typinf
);

OCI_SYM_LOCAL boolean OcilibCollectionFree
(
    OCI_Coll* coll
);

OCI_SYM_LOCAL OCI_Coll** OcilibCollectionCreateArray
(
    OCI_Connection* con,
    OCI_TypeInfo  * typinf,
    unsigned int    nbelem
);

OCI_SYM_LOCAL boolean OcilibCollectionFreeArray
(
    OCI_Coll** colls
);

OCI_SYM_LOCAL boolean OcilibCollectionAssign
(
    OCI_Coll* coll,
    OCI_Coll* coll_src
);

OCI_SYM_LOCAL boolean OcilibCollectionTrim
(
    OCI_Coll   * coll,
    unsigned int nb_elem
);

OCI_SYM_LOCAL boolean OcilibCollectionClear
(
    OCI_Coll* coll
);

OCI_SYM_LOCAL unsigned int OcilibCollectionGetType
(
    OCI_Coll* coll
);

OCI_SYM_LOCAL unsigned int OcilibCollectionGetMax
(
    OCI_Coll* coll
);

OCI_SYM_LOCAL unsigned int OcilibCollectionGetSize
(
    OCI_Coll* coll
);

OCI_SYM_LOCAL unsigned int OcilibCollectionGetCount
(
    OCI_Coll* coll
);

OCI_SYM_LOCAL OCI_Elem* OcilibCollectionGetElement
(
    OCI_Coll   * coll,
    unsigned int index
);

OCI_SYM_LOCAL boolean OcilibCollectionGetElement2
(
    OCI_Coll   * coll,
    unsigned int index,
    OCI_Elem   * elem
);

OCI_SYM_LOCAL boolean OcilibCollectionSetElement
(
    OCI_Coll   * coll,
    unsigned int index,
    OCI_Elem   * elem
);

OCI_SYM_LOCAL boolean OcilibCollectionAddElement
(
    OCI_Coll* coll,
    OCI_Elem* elem
);

OCI_SYM_LOCAL boolean OcilibCollectionRemoveElement
(
    OCI_Coll   * coll,
    unsigned int index
);

OCI_SYM_LOCAL OCI_TypeInfo* OcilibCollectionGetTypeInfo
(
    OCI_Coll* coll
);

OCI_SYM_LOCAL boolean OcilibCollectionToString
(
    OCI_Coll    * coll,
    unsigned int* size,
    otext       * str
);

#endif /* OCILIB_COLLECTION_H_INCLUDED */