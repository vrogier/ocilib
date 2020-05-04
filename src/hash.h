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

#ifndef OCILIB_HASH_H_INCLUDED
#define OCILIB_HASH_H_INCLUDED

#include "types.h"

OCI_HashTable * HashCreate
(
    unsigned int size,
    unsigned int type
);

boolean HashFree
(
    OCI_HashTable *table
);

unsigned int HashGetSize
(
    OCI_HashTable *table
);

unsigned int HashGetType
(
    OCI_HashTable *table
);

boolean HashAddString
(
    OCI_HashTable *table,
    const otext   *key,
    const otext   *value
);

const otext * HashGetString
(
    OCI_HashTable *table,
    const otext   *key
);

boolean HashAddInt
(
    OCI_HashTable *table,
    const otext   *key,
    int            value
);

int HashGetInt
(
    OCI_HashTable *table,
    const otext   *key
);

boolean HashAddPointer
(
    OCI_HashTable *table,
    const otext   *key,
    void          *value
);

void * HashGetPointer
(
    OCI_HashTable *table,
    const otext   *key
);

OCI_HashEntry * HashLookup
(
    OCI_HashTable *table,
    const otext   *key,
    boolean        create
);

OCI_HashValue * HashGetValue
(
    OCI_HashTable *table,
    const otext   *key
);

OCI_HashEntry * HashGetEntry
(
    OCI_HashTable *table,
    unsigned int   index
);

#endif /* OCILIB_HASH_H_INCLUDED */
