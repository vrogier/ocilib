/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2026 Vincent ROGIER <vince.rogier@gmail.com>
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

#ifndef OCILIB_SODADB_H_INCLUDED
#define OCILIB_SODADB_H_INCLUDED

#include "types.h"

OCI_SYM_LOCAL OCI_SodaDatabase * OcilibSodaDatabaseCreate
(
    OCI_Connection *con
);

OCI_SYM_LOCAL OCI_Boolean OcilibSodaDatabaseFree
(
    OCI_SodaDatabase *sdb
);

OCI_SYM_LOCAL OCI_Connection * OcilibSodaDatabaseGetConnection
(
    OCI_SodaDatabase *sdb
);

OCI_SYM_LOCAL OCI_SodaCollection * OcilibSodaDatabaseCreateCollection
(
    OCI_SodaDatabase *sdb,
    const otext      *name,
    const otext      *metadata
);

OCI_SYM_LOCAL OCI_SodaCollection * OcilibSodaDatabaseOpenCollection
(
    OCI_SodaDatabase *sdb,
    const otext      *name
);

OCI_SYM_LOCAL otext ** OcilibSodaDatabaseGetCollectionNames
(
    OCI_SodaDatabase * sdb,
    OCI_UInt32       * count
);

OCI_SYM_LOCAL OCI_Boolean OcilibSodaDatabaseFreeCollectionNames
(
    OCI_SodaDatabase * sdb,
    otext           ** names,
    OCI_UInt32         count
);

OCI_SYM_LOCAL OCI_SodaCollectionCursor * OcilibSodaDatabaseGetCollections
(
    OCI_SodaDatabase *sdb,
    const otext      *startName
);

#endif /* OCILIB_SODADB_H_INCLUDED */
