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

#ifndef OCILIB_COLLECTION_H_INCLUDED
#define OCILIB_COLLECTION_H_INCLUDED

#include "types.h"

OCI_SYM_LOCAL OCI_Collection * OcilibCollectionInitialize
(
    OCI_Connection *con,
    OCI_Collection *Coll,
    void           *handle,
    OCI_TypeInfo   *typeinf
);

OCI_SYM_LOCAL OCI_Collection* OcilibCollectionCreate
(
    OCI_TypeInfo* typinf
);

OCI_SYM_LOCAL OCI_Boolean OcilibCollectionFree
(
    OCI_Collection* coll
);

OCI_SYM_LOCAL OCI_Collection** OcilibCollectionArrayCreate
(
    OCI_TypeInfo * typinf,
    OCI_UInt32     nbelem
);

OCI_SYM_LOCAL OCI_Boolean OcilibCollectionArrayFree
(
    OCI_Collection** colls
);

OCI_SYM_LOCAL OCI_Boolean OcilibCollectionAssign
(
    OCI_Collection* coll,
    OCI_Collection* coll_src
);

OCI_SYM_LOCAL OCI_Boolean OcilibCollectionTrim
(
    OCI_Collection * coll,
    OCI_UInt32       nb_elem
);

OCI_SYM_LOCAL OCI_Boolean OcilibCollectionClear
(
    OCI_Collection* coll
);

OCI_SYM_LOCAL OCI_CollType OcilibCollectionGetType
(
    OCI_Collection* coll
);

OCI_SYM_LOCAL OCI_UInt32 OcilibCollectionGetMax
(
    OCI_Collection* coll
);

OCI_SYM_LOCAL OCI_UInt32 OcilibCollectionGetSize
(
    OCI_Collection* coll
);

OCI_SYM_LOCAL OCI_UInt32 OcilibCollectionGetCount
(
    OCI_Collection* coll
);

OCI_SYM_LOCAL OCI_Element* OcilibCollectionGetElement
(
    OCI_Collection * coll,
    OCI_UInt32       index
);

OCI_SYM_LOCAL OCI_Boolean OcilibCollectionSetElement
(
    OCI_Collection * coll,
    OCI_UInt32       index,
    OCI_Element    * elem
);

OCI_SYM_LOCAL OCI_Boolean OcilibCollectionAppend
(
    OCI_Collection* coll,
    OCI_Element   * elem
);

OCI_SYM_LOCAL OCI_Boolean OcilibCollectionDeleteElement
(
    OCI_Collection * coll,
    OCI_UInt32       index
);

OCI_SYM_LOCAL OCI_TypeInfo* OcilibCollectionGetTypeInfo
(
    OCI_Collection* coll
);

OCI_SYM_LOCAL OCI_Boolean OcilibCollectionToText
(
    OCI_Collection * coll,
    OCI_UInt32     * size,
    otext          * str
);

#endif /* OCILIB_COLLECTION_H_INCLUDED */