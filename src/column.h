/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2022 Vincent ROGIER <vince.rogier@ocilib.net>
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

OCI_SYM_LOCAL boolean OcilibColumnGetAttributeInfo
(
    OCI_Column  *col,
    unsigned int count,
    unsigned int index,
    size_t      *p_size,
    size_t      *p_align
);

OCI_SYM_LOCAL boolean OcilibColumnRetrieveInfo
(
    OCI_Column     *col,
    OCI_Connection *con,
    OCI_Statement  *stmt,
    void           *handle,
    int             index,
    int             ptype
);

OCI_SYM_LOCAL boolean OcilibColumnMapInfo
(
    OCI_Column    *col,
    OCI_Statement *stmt
);

OCI_SYM_LOCAL const otext* OcilibColumnGetName
(
    OCI_Column* col
);

OCI_SYM_LOCAL unsigned int OcilibColumnGetType
(
    OCI_Column* col
);

OCI_SYM_LOCAL unsigned int OcilibColumnGetCharsetForm
(
    OCI_Column* col
);

OCI_SYM_LOCAL unsigned int OcilibColumnGetSize
(
    OCI_Column* col
);

OCI_SYM_LOCAL int OcilibColumnGetScale
(
    OCI_Column* col
);

OCI_SYM_LOCAL int OcilibColumnGetPrecision
(
    OCI_Column* col
);

OCI_SYM_LOCAL int OcilibColumnGetFractionalPrecision
(
    OCI_Column* col
);

OCI_SYM_LOCAL int OcilibColumnGetLeadingPrecision
(
    OCI_Column* col
);

OCI_SYM_LOCAL boolean OcilibColumnGetNullable
(
    OCI_Column* col
);

OCI_SYM_LOCAL boolean OcilibColumnGetCharUsed
(
    OCI_Column* col
);

OCI_SYM_LOCAL unsigned int OcilibColumnGetPropertyFlags
(
    OCI_Column* col
);

OCI_SYM_LOCAL unsigned int OcilibColumnGetCollationID
(
    OCI_Column* col
);

OCI_SYM_LOCAL const otext* OcilibColumnGetSqlType
(
    OCI_Column* col
);

OCI_SYM_LOCAL unsigned int OcilibColumnGetFullSqlType
(
    OCI_Column * col,
    otext      * buffer,
    unsigned int len
);

OCI_SYM_LOCAL OCI_TypeInfo* OcilibColumnGetTypeInfo
(
    OCI_Column* col
);

OCI_SYM_LOCAL unsigned int OcilibColumnGetSubType
(
    OCI_Column* col
);

#endif /* OCILIB_COLUMN_H_INCLUDED */
