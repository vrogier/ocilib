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

#ifndef OCILIB_SODACOLL_H_INCLUDED
#define OCILIB_SODACOLL_H_INCLUDED

#include "types.h"

OCI_SYM_LOCAL OCI_SodaCollection * OcilibSodaCollectionInitialize
(
    OCI_SodaCollection *coll,
    OCI_SodaDatabase   *sdb,
    OCISodaColl        *handle
);

OCI_SYM_LOCAL const otext * OcilibSodaCollectionGetName
(
    OCI_SodaCollection *coll
);

OCI_SYM_LOCAL const otext * OcilibSodaCollectionGetMetadata
(
    OCI_SodaCollection *coll
);

OCI_SYM_LOCAL OCI_Connection * OcilibSodaCollectionGetConnection
(
    OCI_SodaCollection *coll
);

OCI_SYM_LOCAL OCI_Boolean OcilibSodaCollectionIsMapped
(
    OCI_SodaCollection *coll
);

OCI_SYM_LOCAL OCI_Boolean OcilibSodaCollectionDrop
(
    OCI_SodaCollection *coll
);

OCI_SYM_LOCAL OCI_Boolean OcilibSodaCollectionTruncate
(
    OCI_SodaCollection *coll
);

OCI_SYM_LOCAL otext ** OcilibSodaCollectionListIndexes
(
    OCI_SodaCollection * coll,
    OCI_UInt32         * count
);

OCI_SYM_LOCAL OCI_Boolean OcilibSodaCollectionFreeIndexNames
(
    OCI_SodaCollection * coll,
    otext             ** names,
    OCI_UInt32           count
);

OCI_SYM_LOCAL OCI_Boolean OcilibSodaCollectionFree
(
    OCI_SodaCollection *coll
);

OCI_SYM_LOCAL OCI_Boolean OcilibSodaCollectionInsertDocument
(
    OCI_SodaCollection *coll,
    OCI_SodaDocument   *doc
);

OCI_SYM_LOCAL OCI_SodaDocumentCursor * OcilibSodaCollectionFind
(
    OCI_SodaCollection *coll,
    OCI_SodaOptions    *options
);

OCI_SYM_LOCAL OCI_SodaDocument * OcilibSodaCollectionFindOne
(
    OCI_SodaCollection *coll,
    OCI_SodaOptions    *options
);

OCI_SYM_LOCAL OCI_SodaDocument * OcilibSodaCollectionFindOneByKey
(
    OCI_SodaCollection *coll,
    const otext        *key
);

OCI_SYM_LOCAL OCI_UInt64 OcilibSodaCollectionDocumentCount
(
    OCI_SodaCollection *coll,
    OCI_SodaOptions    *options
);

OCI_SYM_LOCAL OCI_UInt64 OcilibSodaCollectionDocumentCountWithFilter
(
    OCI_SodaCollection *coll,
    const otext        *filterSpec
);

OCI_SYM_LOCAL OCI_Boolean OcilibSodaCollectionReplaceOne
(
    OCI_SodaCollection *coll,
    OCI_SodaOptions    *options,
    OCI_SodaDocument   *doc,
    OCI_Boolean         refresh
);

OCI_SYM_LOCAL OCI_Boolean OcilibSodaCollectionReplaceOneByKey
(
    OCI_SodaCollection *coll,
    const otext        *key,
    OCI_SodaDocument   *doc,
    OCI_Boolean         refresh
);

OCI_SYM_LOCAL OCI_Boolean OcilibSodaCollectionRemove
(
    OCI_SodaCollection *coll,
    OCI_SodaOptions    *options,
    OCI_UInt64         *count
);

OCI_SYM_LOCAL OCI_Boolean OcilibSodaCollectionRemoveOneByKey
(
    OCI_SodaCollection *coll,
    const otext        *key
);

OCI_SYM_LOCAL OCI_Boolean OcilibSodaCollectionInsertMany
(
    OCI_SodaCollection *coll,
    OCI_SodaDocument  **docs,
    OCI_UInt32          count,
    OCI_Boolean         refresh
);

OCI_SYM_LOCAL OCI_Boolean OcilibSodaCollectionCreateIndex
(
    OCI_SodaCollection *coll,
    const otext        *indexSpec
);

OCI_SYM_LOCAL OCI_Boolean OcilibSodaCollectionDropIndex
(
    OCI_SodaCollection *coll,
    const otext        *name
);

OCI_SYM_LOCAL OCI_SodaDocument * OcilibSodaCollectionGetDataGuide
(
    OCI_SodaCollection *coll
);

OCI_SYM_LOCAL OCI_Boolean OcilibSodaCollectionSave
(
    OCI_SodaCollection *coll,
    OCI_SodaDocument   *doc,
    OCI_Boolean         refresh
);

#endif /* OCILIB_SODACOLL_H_INCLUDED */
