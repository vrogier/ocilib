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

#ifndef OCILIB_RESULTSET_H_INCLUDED
#define OCILIB_RESULTSET_H_INCLUDED

#include "types.h"

OCI_SYM_LOCAL OCI_Boolean OcilibResultsetInitialize
(
    OCI_Resultset* rs
);

OCI_SYM_LOCAL OCI_Resultset* OcilibResultsetCreate
(
    OCI_Statement * stmt,
    OCI_Int32       size
);

OCI_SYM_LOCAL OCI_Boolean OcilibResultsetFree
(
    OCI_Resultset* rs
);

OCI_SYM_LOCAL OCI_Boolean OcilibResultsetFetchPrev
(
    OCI_Resultset* rs
);

OCI_SYM_LOCAL OCI_Boolean OcilibResultsetFetchNext
(
    OCI_Resultset* rs
);

OCI_SYM_LOCAL OCI_Boolean OcilibResultsetFetchFirst
(
    OCI_Resultset* rs
);

OCI_SYM_LOCAL OCI_Boolean OcilibResultsetFetchLast
(
    OCI_Resultset* rs
);

OCI_SYM_LOCAL OCI_Boolean OcilibResultsetFetchSeek
(
    OCI_Resultset   * rs,
    OCI_FetchSeekMode mode,
    OCI_Int32         offset
);

OCI_SYM_LOCAL OCI_UInt32 OcilibResultsetGetRowCount
(
    OCI_Resultset* rs
);

OCI_SYM_LOCAL OCI_Int32 OcilibResultsetGetCurrentRow
(
    OCI_Resultset* rs
);

OCI_SYM_LOCAL OCI_UInt32 OcilibResultsetGetColumnCount
(
    OCI_Resultset* rs
);

OCI_SYM_LOCAL OCI_Column* OcilibResultsetGetColumn
(
    OCI_Resultset* rs,
    OCI_UInt32     index
);

OCI_SYM_LOCAL OCI_Column* OcilibResultsetGetColumnByName
(
    OCI_Resultset* rs,
    const otext  * name
);

OCI_SYM_LOCAL OCI_Int32 OcilibResultsetGetColumnIndex
(
    OCI_Resultset* rs,
    const otext  * name
);

OCI_SYM_LOCAL OCI_Boolean OcilibResultsetSetStructNumericType
(
    OCI_Resultset * rs,
    OCI_UInt32      index,
    OCI_NumericType type
);

OCI_SYM_LOCAL OCI_Boolean OcilibResultsetSetStructNumericTypeByName
(
    OCI_Resultset * rs,
    const otext   * name,
    OCI_NumericType type
);

OCI_SYM_LOCAL OCI_Boolean OcilibResultsetGetStruct
(
    OCI_Resultset* rs,
    void         * row_struct,
    void         * row_struct_ind
);

OCI_SYM_LOCAL OCI_Number* OcilibResultsetGetNumber
(
    OCI_Resultset* rs,
    OCI_UInt32     index
);

OCI_SYM_LOCAL OCI_Number* OcilibResultsetGetNumberByName
(
    OCI_Resultset* rs,
    const otext  * name
);

OCI_SYM_LOCAL OCI_Int16 OcilibResultsetGetInt16
(
    OCI_Resultset* rs,
    OCI_UInt32     index
);

OCI_SYM_LOCAL OCI_Int16 OcilibResultsetGetInt16ByName
(
    OCI_Resultset* rs,
    const otext  * name
);

OCI_SYM_LOCAL OCI_UInt16 OcilibResultsetGetUInt16
(
    OCI_Resultset* rs,
    OCI_UInt32     index
);

OCI_SYM_LOCAL OCI_UInt16 OcilibResultsetGetUInt16ByName
(
    OCI_Resultset* rs,
    const otext  * name
);

OCI_SYM_LOCAL OCI_Int32 OcilibResultsetGetInt32
(
    OCI_Resultset* rs,
    OCI_UInt32     index
);

OCI_SYM_LOCAL OCI_Int32 OcilibResultsetGetInt32ByName
(
    OCI_Resultset* rs,
    const otext  * name
);

OCI_SYM_LOCAL OCI_UInt32 OcilibResultsetGetUInt32
(
    OCI_Resultset* rs,
    OCI_UInt32     index
);

OCI_SYM_LOCAL OCI_UInt32 OcilibResultsetGetUInt32ByName
(
    OCI_Resultset* rs,
    const otext  * name
);

OCI_SYM_LOCAL OCI_Int64 OcilibResultsetGetInt64
(
    OCI_Resultset* rs,
    OCI_UInt32     index
);

OCI_SYM_LOCAL OCI_Int64 OcilibResultsetGetInt64ByName
(
    OCI_Resultset* rs,
    const otext  * name
);

OCI_SYM_LOCAL OCI_UInt64 OcilibResultsetGetUInt64
(
    OCI_Resultset* rs,
    OCI_UInt32     index
);

OCI_SYM_LOCAL OCI_UInt64 OcilibResultsetGetUInt64ByName
(
    OCI_Resultset* rs,
    const otext  * name
);

OCI_SYM_LOCAL const otext * OcilibResultsetGetString
(
    OCI_Resultset* rs,
    OCI_UInt32     index
);

OCI_SYM_LOCAL const otext * OcilibResultsetGetStringByName
(
    OCI_Resultset* rs,
    const otext  * name
);

OCI_SYM_LOCAL OCI_UInt32 OcilibResultsetGetRaw
(
    OCI_Resultset* rs,
    OCI_UInt32     index,
    void         * buffer,
    OCI_UInt32     len
);

OCI_SYM_LOCAL OCI_UInt32 OcilibResultsetGetRawByName
(
    OCI_Resultset* rs,
    const otext  * name,
    void         * buffer,
    OCI_UInt32     len
);

OCI_SYM_LOCAL OCI_Double OcilibResultsetGetDouble
(
    OCI_Resultset* rs,
    OCI_UInt32     index
);

OCI_SYM_LOCAL OCI_Double OcilibResultsetGetDoubleByName
(
    OCI_Resultset* rs,
    const otext  * name
);

OCI_SYM_LOCAL OCI_Float OcilibResultsetGetFloat
(
    OCI_Resultset* rs,
    OCI_UInt32     index
);

OCI_SYM_LOCAL OCI_Float OcilibResultsetGetFloatByName
(
    OCI_Resultset* rs,
    const otext  * name
);

OCI_SYM_LOCAL OCI_Date * OcilibResultsetGetDate
(
    OCI_Resultset* rs,
    OCI_UInt32     index
);

OCI_SYM_LOCAL OCI_Date * OcilibResultsetGetDateByName
(
    OCI_Resultset* rs,
    const otext  * name
);

OCI_SYM_LOCAL OCI_Timestamp * OcilibResultsetGetTimestamp
(
    OCI_Resultset* rs,
    OCI_UInt32     index
);

OCI_SYM_LOCAL OCI_Timestamp * OcilibResultsetGetTimestampByName
(
    OCI_Resultset* rs,
    const otext  * name
);

OCI_SYM_LOCAL OCI_Interval * OcilibResultsetGetInterval
(
    OCI_Resultset* rs,
    OCI_UInt32     index
);

OCI_SYM_LOCAL OCI_Interval * OcilibResultsetGetIntervalByName
(
    OCI_Resultset* rs,
    const otext  * name
);

OCI_SYM_LOCAL OCI_Object * OcilibResultsetGetObject
(
    OCI_Resultset* rs,
    OCI_UInt32     index
);

OCI_SYM_LOCAL OCI_Object * OcilibResultsetGetObjectByName
(
    OCI_Resultset* rs,
    const otext  * name
);

OCI_SYM_LOCAL OCI_Collection * OcilibResultsetGetColl
(
    OCI_Resultset* rs,
    OCI_UInt32     index
);

OCI_SYM_LOCAL OCI_Collection * OcilibResultsetGetCollByName
(
    OCI_Resultset* rs,
    const otext  * name
);

OCI_SYM_LOCAL OCI_Reference * OcilibResultsetGetReference
(
    OCI_Resultset* rs,
    OCI_UInt32     index
);

OCI_SYM_LOCAL OCI_Reference * OcilibResultsetGetReferenceByName
(
    OCI_Resultset* rs,
    const otext  * name
);

OCI_SYM_LOCAL OCI_XmlType * OcilibResultsetGetXmlType
(
    OCI_Resultset* rs,
    OCI_UInt32     index
);

OCI_SYM_LOCAL OCI_XmlType * OcilibResultsetGetXmlTypeByName
(
    OCI_Resultset* rs,
    const otext  * name
);

OCI_SYM_LOCAL OCI_Vector * OcilibResultsetGetVector
(
    OCI_Resultset* rs,
    OCI_UInt32     index
);

OCI_SYM_LOCAL OCI_Vector * OcilibResultsetGetVectorByName
(
    OCI_Resultset* rs,
    const otext  * name
);

OCI_SYM_LOCAL OCI_Statement * OcilibResultsetGetCursor
(
    OCI_Resultset* rs,
    OCI_UInt32     index
);

OCI_SYM_LOCAL OCI_Statement * OcilibResultsetGetCursorByName
(
    OCI_Resultset* rs,
    const otext  * name
);

OCI_SYM_LOCAL OCI_Lob * OcilibResultsetGetLob
(
    OCI_Resultset* rs,
    OCI_UInt32     index
);

OCI_SYM_LOCAL OCI_Lob * OcilibResultsetGetLobByName
(
    OCI_Resultset* rs,
    const otext  * name
);

OCI_SYM_LOCAL OCI_Json * OcilibResultsetGetJson
(
    OCI_Resultset* rs,
    OCI_UInt32     index
);

OCI_SYM_LOCAL OCI_Json * OcilibResultsetGetJsonByName
(
    OCI_Resultset* rs,
    const otext  * name
);

OCI_SYM_LOCAL OCI_File * OcilibResultsetGetFile
(
    OCI_Resultset* rs,
    OCI_UInt32     index
);

OCI_SYM_LOCAL OCI_File * OcilibResultsetGetFileByName
(
    OCI_Resultset* rs,
    const otext  * name
);

OCI_SYM_LOCAL OCI_Long * OcilibResultsetGetLong
(
    OCI_Resultset* rs,
    OCI_UInt32     index
);

OCI_SYM_LOCAL OCI_Long * OcilibResultsetGetLongByName
(
    OCI_Resultset* rs,
    const otext  * name
);

OCI_SYM_LOCAL OCI_UInt32 OcilibResultsetGetDataSize
(
    OCI_Resultset* rs,
    OCI_UInt32     index
);

OCI_SYM_LOCAL OCI_UInt32 OcilibResultsetGetDataSizeByName
(
    OCI_Resultset* rs,
    const otext  * name
);

OCI_SYM_LOCAL OCI_Boolean OcilibResultsetIsColumnNull
(
    OCI_Resultset* rs,
    OCI_UInt32     index
);

OCI_SYM_LOCAL OCI_Boolean OcilibResultsetIsColumnNullByName
(
    OCI_Resultset* rs,
    const otext  * name
);

OCI_SYM_LOCAL OCI_Statement * OcilibResultsetResultsetGetStatement
(
    OCI_Resultset* rs
);

OCI_SYM_LOCAL OCI_UInt32 OcilibResultsetGetDataLength
(
    OCI_Resultset* rs,
    OCI_UInt32     index
);

#endif /* OCILIB_RESULTSET_H_INCLUDED */
