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

#ifndef OCILIB_HASH_H_INCLUDED
#define OCILIB_HASH_H_INCLUDED

#include "types.h"

OCI_SYM_LOCAL OCI_HashTable * OcilibHashCreate
(
    unsigned int size,
    unsigned int type
);

OCI_SYM_LOCAL boolean OcilibHashFree
(
    OCI_HashTable *table
);

OCI_SYM_LOCAL unsigned int OcilibHashGetSize
(
    OCI_HashTable *table
);

OCI_SYM_LOCAL unsigned int OcilibHashGetType
(
    OCI_HashTable *table
);

OCI_SYM_LOCAL boolean OcilibHashAddString
(
    OCI_HashTable *table,
    const otext   *key,
    const otext   *value
);

OCI_SYM_LOCAL const otext * OcilibHashGetString
(
    OCI_HashTable *table,
    const otext   *key
);

OCI_SYM_LOCAL boolean OcilibHashAddInt
(
    OCI_HashTable *table,
    const otext   *key,
    int            value
);

OCI_SYM_LOCAL int OcilibHashGetInt
(
    OCI_HashTable *table,
    const otext   *key
);

OCI_SYM_LOCAL boolean OcilibHashAddPointer
(
    OCI_HashTable *table,
    const otext   *key,
    void          *value
);

OCI_SYM_LOCAL void * OcilibHashGetPointer
(
    OCI_HashTable *table,
    const otext   *key
);

OCI_SYM_LOCAL OCI_HashEntry * OcilibHashLookup
(
    OCI_HashTable *table,
    const otext   *key,
    boolean        create
);

OCI_SYM_LOCAL OCI_HashValue * OcilibHashGetValue
(
    OCI_HashTable *table,
    const otext   *key
);

OCI_SYM_LOCAL OCI_HashEntry * OcilibHashGetEntry
(
    OCI_HashTable *table,
    unsigned int   index
);

#endif /* OCILIB_HASH_H_INCLUDED */
