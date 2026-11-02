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

OCI_SYM_LOCAL OCI_SodaColl * OcilibSodaCollInitialize
(
    OCI_SodaColl *coll,
    OCI_SodaDb   *sdb,
    OCISodaColl  *handle
);

OCI_SYM_LOCAL const otext * OcilibSodaCollGetName
(
    OCI_SodaColl *coll
);

OCI_SYM_LOCAL const otext * OcilibSodaCollGetMetadata
(
    OCI_SodaColl *coll
);

OCI_SYM_LOCAL OCI_Connection * OcilibSodaCollGetConnection
(
    OCI_SodaColl *coll
);

OCI_SYM_LOCAL OCI_Boolean OcilibSodaCollIsMapped
(
    OCI_SodaColl *coll
);

OCI_SYM_LOCAL OCI_Boolean OcilibSodaCollDrop
(
    OCI_SodaColl *coll
);

OCI_SYM_LOCAL OCI_Boolean OcilibSodaCollTruncate
(
    OCI_SodaColl *coll
);

OCI_SYM_LOCAL OCI_Boolean OcilibSodaCollFree
(
    OCI_SodaColl *coll
);

OCI_SYM_LOCAL OCI_Boolean OcilibSodaCollInsertDoc
(
    OCI_SodaColl *coll,
    OCI_SodaDoc  *doc
);

OCI_SYM_LOCAL OCI_SodaDocCursor * OcilibSodaCollFind
(
    OCI_SodaColl    *coll,
    OCI_SodaOptions *options
);

OCI_SYM_LOCAL OCI_SodaDoc * OcilibSodaCollFindOne
(
    OCI_SodaColl    *coll,
    OCI_SodaOptions *options
);

OCI_SYM_LOCAL OCI_SodaDoc * OcilibSodaCollFindOneByKey
(
    OCI_SodaColl *coll,
    const otext  *key
);

OCI_SYM_LOCAL OCI_UInt64 OcilibSodaCollDocCount
(
    OCI_SodaColl    *coll,
    OCI_SodaOptions *options
);

OCI_SYM_LOCAL OCI_UInt64 OcilibSodaCollDocCountWithFilter
(
    OCI_SodaColl *coll,
    const otext  *filterSpec
);

OCI_SYM_LOCAL OCI_Boolean OcilibSodaCollReplaceOne
(
    OCI_SodaColl    *coll,
    OCI_SodaOptions *options,
    OCI_SodaDoc     *doc
);

OCI_SYM_LOCAL OCI_Boolean OcilibSodaCollReplaceOneAndGet
(
    OCI_SodaColl    *coll,
    OCI_SodaOptions *options,
    OCI_SodaDoc     *doc
);

OCI_SYM_LOCAL OCI_Boolean OcilibSodaCollReplaceOneByKey
(
    OCI_SodaColl *coll,
    const otext  *key,
    OCI_SodaDoc  *doc
);

OCI_SYM_LOCAL OCI_Boolean OcilibSodaCollReplaceOneAndGetByKey
(
    OCI_SodaColl *coll,
    const otext  *key,
    OCI_SodaDoc  *doc
);

OCI_SYM_LOCAL OCI_Boolean OcilibSodaCollRemove
(
    OCI_SodaColl    *coll,
    OCI_SodaOptions *options,
    OCI_UInt64      *count
);

OCI_SYM_LOCAL OCI_Boolean OcilibSodaCollRemoveOneByKey
(
    OCI_SodaColl *coll,
    const otext  *key
);

#endif /* OCILIB_SODACOLL_H_INCLUDED */
