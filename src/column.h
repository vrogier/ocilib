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

#ifndef OCILIB_COLUMN_H_INCLUDED
#define OCILIB_COLUMN_H_INCLUDED

#include "types.h"

boolean OcilibColumnGetAttributeInfo
(
    OCI_Column  *col,
    unsigned int count,
    unsigned int index,
    size_t      *p_size,
    size_t      *p_align
);

boolean OcilibColumnRetrieveInfo
(
    OCI_Column     *col,
    OCI_Connection *con,
    OCI_Statement  *stmt,
    void           *handle,
    int             index,
    int             ptype
);

boolean OcilibColumnMapInfo
(
    OCI_Column    *col,
    OCI_Statement *stmt
);

const otext* OcilibColumnGetName
(
    OCI_Column* col
);

unsigned int OcilibColumnGetType
(
    OCI_Column* col
);

unsigned int OcilibColumnGetCharsetForm
(
    OCI_Column* col
);

unsigned int OcilibColumnGetSize
(
    OCI_Column* col
);

int OcilibColumnGetScale
(
    OCI_Column* col
);

int OcilibColumnGetPrecision
(
    OCI_Column* col
);

int OcilibColumnGetFractionalPrecision
(
    OCI_Column* col
);

int OcilibColumnGetLeadingPrecision
(
    OCI_Column* col
);

boolean OcilibColumnGetNullable
(
    OCI_Column* col
);

boolean OcilibColumnGetCharUsed
(
    OCI_Column* col
);

unsigned int OcilibColumnGetPropertyFlags
(
    OCI_Column* col
);

unsigned int OcilibColumnGetCollationID
(
    OCI_Column* col
);

const otext* OcilibColumnGetSqlType
(
    OCI_Column* col
);

unsigned int OcilibColumnGetFullSqlType
(
    OCI_Column * col,
    otext      * buffer,
    unsigned int len
);

OCI_TypeInfo* OcilibColumnGetTypeInfo
(
    OCI_Column* col
);

unsigned int OcilibColumnGetSubType
(
    OCI_Column* col
);

#endif /* OCILIB_COLUMN_H_INCLUDED */
