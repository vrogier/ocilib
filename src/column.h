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

boolean ColumnGetAttributeInfo
(
    OCI_Column  *col,
    unsigned int count,
    unsigned int index,
    size_t      *p_size,
    size_t      *p_align
);

boolean ColumnRetrieveInfo
(
    OCI_Column     *col,
    OCI_Connection *con,
    OCI_Statement  *stmt,
    void           *handle,
    int             index,
    int             ptype
);

boolean ColumnMapInfo
(
    OCI_Column    *col,
    OCI_Statement *stmt
);

const otext* ColumnGetName
(
    OCI_Column* col
);

unsigned int ColumnGetType
(
    OCI_Column* col
);

unsigned int ColumnGetCharsetForm
(
    OCI_Column* col
);

unsigned int ColumnGetSize
(
    OCI_Column* col
);

int ColumnGetScale
(
    OCI_Column* col
);

int ColumnGetPrecision
(
    OCI_Column* col
);

int ColumnGetFractionalPrecision
(
    OCI_Column* col
);

int ColumnGetLeadingPrecision
(
    OCI_Column* col
);

boolean ColumnGetNullable
(
    OCI_Column* col
);

boolean ColumnGetCharUsed
(
    OCI_Column* col
);

unsigned int ColumnGetPropertyFlags
(
    OCI_Column* col
);

unsigned int ColumnGetCollationID
(
    OCI_Column* col
);

const otext* ColumnGetSqlType
(
    OCI_Column* col
);

unsigned int ColumnGetFullSqlType
(
    OCI_Column * col,
    otext      * buffer,
    unsigned int len
);

OCI_TypeInfo* ColumnGetTypeInfo
(
    OCI_Column* col
);

unsigned int ColumnGetSubType
(
    OCI_Column* col
);

#endif /* OCILIB_COLUMN_H_INCLUDED */
