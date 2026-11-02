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

#ifndef OCILIB_COLUMN_H_INCLUDED
#define OCILIB_COLUMN_H_INCLUDED

#include "types.h"

OCI_SYM_LOCAL const otext* OcilibColumnGetVectorFormatString
(
    OCI_VectorFormat format
);

OCI_SYM_LOCAL OCI_Boolean OcilibColumnGetAttributeInfo
(
    OCI_Column  *col,
    OCI_UInt32 count,
    OCI_UInt32 index,
    OCI_Size      *p_size,
    OCI_Size      *p_align
);

OCI_SYM_LOCAL OCI_Boolean OcilibColumnRetrieveInfo
(
    OCI_Column     *col,
    OCI_Connection *con,
    OCI_Statement  *stmt,
    void           *handle,
    OCI_Int32             index,
    OCI_Int32             ptype
);

OCI_SYM_LOCAL OCI_Boolean OcilibColumnMapInfo
(
    OCI_Column    *col,
    OCI_Connection*con,
    OCI_Statement *stmt
);

OCI_SYM_LOCAL const otext* OcilibColumnGetName
(
    OCI_Column* col
);

OCI_SYM_LOCAL OCI_SqlType OcilibColumnGetType
(
    OCI_Column* col
);

OCI_SYM_LOCAL OCI_CharsetForm OcilibColumnGetCharsetForm
(
    OCI_Column* col
);

OCI_SYM_LOCAL OCI_UInt32 OcilibColumnGetSize
(
    OCI_Column* col
);

OCI_SYM_LOCAL OCI_Int32 OcilibColumnGetScale
(
    OCI_Column* col
);

OCI_SYM_LOCAL OCI_Int32 OcilibColumnGetPrecision
(
    OCI_Column* col
);

OCI_SYM_LOCAL OCI_Int32 OcilibColumnGetFractionalPrecision
(
    OCI_Column* col
);

OCI_SYM_LOCAL OCI_Int32 OcilibColumnGetLeadingPrecision
(
    OCI_Column* col
);

OCI_SYM_LOCAL OCI_Boolean OcilibColumnGetNullable
(
    OCI_Column* col
);

OCI_SYM_LOCAL OCI_Boolean OcilibColumnGetCharUsed
(
    OCI_Column* col
);

OCI_SYM_LOCAL OCI_ColumnPropFlags OcilibColumnGetPropertyFlags
(
    OCI_Column* col
);

OCI_SYM_LOCAL OCI_CollationID OcilibColumnGetCollationID
(
    OCI_Column* col
);

OCI_SYM_LOCAL OCI_UInt32 OcilibColumnGetDimension
(
    OCI_Column* col
);

OCI_SYM_LOCAL const otext* OcilibColumnGetSqlType
(
    OCI_Column* col
);

OCI_SYM_LOCAL OCI_UInt32 OcilibColumnGetFullSqlType
(
    OCI_Column * col,
    otext      * buffer,
    OCI_UInt32 len
);

OCI_SYM_LOCAL OCI_TypeInfo* OcilibColumnGetTypeInfo
(
    OCI_Column* col
);

OCI_SYM_LOCAL OCI_SubType OcilibColumnGetSubType
(
    OCI_Column* col
);

#endif /* OCILIB_COLUMN_H_INCLUDED */
