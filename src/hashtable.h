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

#ifndef OCILIB_HASH_H_INCLUDED
#define OCILIB_HASH_H_INCLUDED

#include "types.h"

OCI_SYM_LOCAL OCI_HashTable * OcilibHashTableCreate
(
    OCI_Environment *env,
    OCI_UInt32       size,
    OCI_HashType     type
);

OCI_SYM_LOCAL OCI_Boolean OcilibHashTableFree
(
    OCI_HashTable *table
);

OCI_SYM_LOCAL OCI_UInt32 OcilibHashTableGetSize
(
    OCI_HashTable *table
);

OCI_SYM_LOCAL OCI_HashType OcilibHashTableGetType
(
    OCI_HashTable *table
);

OCI_SYM_LOCAL OCI_Boolean OcilibHashTableAddString
(
    OCI_HashTable *table,
    const otext   *key,
    const otext   *value
);

OCI_SYM_LOCAL const otext * OcilibHashTableGetString
(
    OCI_HashTable *table,
    const otext   *key
);

OCI_SYM_LOCAL OCI_Boolean OcilibHashTableAddInt32
(
    OCI_HashTable *table,
    const otext   *key,
    OCI_Int32      value
);

OCI_SYM_LOCAL OCI_Int32 OcilibHashTableGetInt32
(
    OCI_HashTable *table,
    const otext   *key
);

OCI_SYM_LOCAL OCI_Boolean OcilibHashTableAddPointer
(
    OCI_HashTable *table,
    const otext   *key,
    void          *value
);

OCI_SYM_LOCAL void * OcilibHashTableGetPointer
(
    OCI_HashTable *table,
    const otext   *key
);

OCI_SYM_LOCAL OCI_HashEntry * OcilibHashTableLookup
(
    OCI_HashTable *table,
    const otext   *key,
    OCI_Boolean    create
);

OCI_SYM_LOCAL OCI_HashValue * OcilibHashTableGetValue
(
    OCI_HashTable *table,
    const otext   *key
);

OCI_SYM_LOCAL OCI_HashEntry * OcilibHashTableGetEntry
(
    OCI_HashTable *table,
    OCI_UInt32     index
);

#endif /* OCILIB_HASH_H_INCLUDED */
