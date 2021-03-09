/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2021 Vincent ROGIER <vince.rogier@ocilib.net>
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

OCI_SYM_LOCAL boolean OcilibResultsetInitialize
(
    OCI_Resultset* rs
);

OCI_SYM_LOCAL OCI_Resultset* OcilibResultsetCreate
(
    OCI_Statement * stmt,
    int             size
);

OCI_SYM_LOCAL boolean OcilibResultsetFree
(
    OCI_Resultset* rs
);

OCI_SYM_LOCAL boolean OcilibResultsetFetchPrev
(
    OCI_Resultset* rs
);

OCI_SYM_LOCAL boolean OcilibResultsetFetchNext
(
    OCI_Resultset* rs
);

OCI_SYM_LOCAL boolean OcilibResultsetFetchFirst
(
    OCI_Resultset* rs
);

OCI_SYM_LOCAL boolean OcilibResultsetFetchLast
(
    OCI_Resultset* rs
);

OCI_SYM_LOCAL boolean OcilibResultsetFetchSeek
(
    OCI_Resultset* rs,
    unsigned int   mode,
    int            offset
);

OCI_SYM_LOCAL unsigned int OcilibResultsetGetRowCount
(
    OCI_Resultset* rs
);

OCI_SYM_LOCAL unsigned int OcilibResultsetGetCurrentRow
(
    OCI_Resultset* rs
);

OCI_SYM_LOCAL unsigned int OcilibResultsetGetColumnCount
(
    OCI_Resultset* rs
);

OCI_SYM_LOCAL OCI_Column* OcilibResultsetGetColumn
(
    OCI_Resultset* rs,
    unsigned int   index
);

OCI_SYM_LOCAL OCI_Column* OcilibResultsetGetColumn2
(
    OCI_Resultset* rs,
    const otext  * name
);

OCI_SYM_LOCAL unsigned int OcilibResultsetGetColumnIndex
(
    OCI_Resultset* rs,
    const otext  * name
);

OCI_SYM_LOCAL boolean OcilibResultsetSetStructNumericType
(
    OCI_Resultset* rs,
    unsigned int   index,
    unsigned int   type
);

OCI_SYM_LOCAL boolean OcilibResultsetSetStructNumericType2
(
    OCI_Resultset* rs,
    const otext  * name,
    unsigned int   type
);

OCI_SYM_LOCAL boolean OcilibResultsetGetStruct
(
    OCI_Resultset* rs,
    void         * row_struct,
    void         * row_struct_ind
);

OCI_SYM_LOCAL OCI_Number* OcilibResultsetGetNumber
(
    OCI_Resultset* rs,
    unsigned int   index
);

OCI_SYM_LOCAL OCI_Number* OcilibResultsetGetNumber2
(
    OCI_Resultset* rs,
    const otext  * name
);

OCI_SYM_LOCAL short OcilibResultsetGetShort
(
    OCI_Resultset* rs,
    unsigned int   index
);

OCI_SYM_LOCAL short OcilibResultsetGetShort2
(
    OCI_Resultset* rs,
    const otext  * name
);

OCI_SYM_LOCAL unsigned short OcilibResultsetGetUnsignedShort
(
    OCI_Resultset* rs,
    unsigned int   index
);

OCI_SYM_LOCAL unsigned short OcilibResultsetGetUnsignedShort2
(
    OCI_Resultset* rs,
    const otext  * name
);

OCI_SYM_LOCAL int OcilibResultsetGetInt
(
    OCI_Resultset* rs,
    unsigned int   index
);

OCI_SYM_LOCAL int OcilibResultsetGetInt2
(
    OCI_Resultset* rs,
    const otext  * name
);

OCI_SYM_LOCAL unsigned int OcilibResultsetGetUnsignedInt
(
    OCI_Resultset* rs,
    unsigned int   index
);

OCI_SYM_LOCAL unsigned int OcilibResultsetGetUnsignedInt2
(
    OCI_Resultset* rs,
    const otext  * name
);

OCI_SYM_LOCAL big_int OcilibResultsetGetBigInt
(
    OCI_Resultset* rs,
    unsigned int   index
);

OCI_SYM_LOCAL big_int OcilibResultsetGetBigInt2
(
    OCI_Resultset* rs,
    const otext  * name
);

OCI_SYM_LOCAL big_uint OcilibResultsetGetUnsignedBigInt
(
    OCI_Resultset* rs,
    unsigned int   index
);

OCI_SYM_LOCAL big_uint OcilibResultsetGetUnsignedBigInt2
(
    OCI_Resultset* rs,
    const otext  * name
);

OCI_SYM_LOCAL const otext * OcilibResultsetGetString
(
    OCI_Resultset* rs,
    unsigned int   index
);

OCI_SYM_LOCAL const otext * OcilibResultsetGetString2
(
    OCI_Resultset* rs,
    const otext  * name
);

OCI_SYM_LOCAL unsigned int OcilibResultsetGetRaw
(
    OCI_Resultset* rs,
    unsigned int   index,
    void         * buffer,
    unsigned int   len
);

OCI_SYM_LOCAL unsigned int OcilibResultsetGetRaw2
(
    OCI_Resultset* rs,
    const otext  * name,
    void         * buffer,
    unsigned int   len
);

OCI_SYM_LOCAL double OcilibResultsetGetDouble
(
    OCI_Resultset* rs,
    unsigned int   index
);

OCI_SYM_LOCAL double OcilibResultsetGetDouble2
(
    OCI_Resultset* rs,
    const otext  * name
);

OCI_SYM_LOCAL float OcilibResultsetGetFloat
(
    OCI_Resultset* rs,
    unsigned int   index
);

OCI_SYM_LOCAL float OcilibResultsetGetFloat2
(
    OCI_Resultset* rs,
    const otext  * name
);

OCI_SYM_LOCAL OCI_Date * OcilibResultsetGetDate
(
    OCI_Resultset* rs,
    unsigned int   index
);

OCI_SYM_LOCAL OCI_Date * OcilibResultsetGetDate2
(
    OCI_Resultset* rs,
    const otext  * name
);

OCI_SYM_LOCAL OCI_Timestamp * OcilibResultsetGetTimestamp
(
    OCI_Resultset* rs,
    unsigned int   index
);

OCI_SYM_LOCAL OCI_Timestamp * OcilibResultsetGetTimestamp2
(
    OCI_Resultset* rs,
    const otext  * name
);

OCI_SYM_LOCAL OCI_Interval * OcilibResultsetGetInterval
(
    OCI_Resultset* rs,
    unsigned int   index
);

OCI_SYM_LOCAL OCI_Interval * OcilibResultsetGetInterval2
(
    OCI_Resultset* rs,
    const otext  * name
);

OCI_SYM_LOCAL OCI_Object * OcilibResultsetGetObject
(
    OCI_Resultset* rs,
    unsigned int   index
);

OCI_SYM_LOCAL OCI_Object * OcilibResultsetGetObject2
(
    OCI_Resultset* rs,
    const otext  * name
);

OCI_SYM_LOCAL OCI_Coll * OcilibResultsetGetColl
(
    OCI_Resultset* rs,
    unsigned int   index
);

OCI_SYM_LOCAL OCI_Coll * OcilibResultsetGetColl2
(
    OCI_Resultset* rs,
    const otext  * name
);

OCI_SYM_LOCAL OCI_Ref * OcilibResultsetGetReference
(
    OCI_Resultset* rs,
    unsigned int   index
);

OCI_SYM_LOCAL OCI_Ref * OcilibResultsetGetReference2
(
    OCI_Resultset* rs,
    const otext  * name
);

OCI_SYM_LOCAL OCI_Statement * OcilibResultsetGetStatement
(
    OCI_Resultset* rs,
    unsigned int   index
);

OCI_SYM_LOCAL OCI_Statement * OcilibResultsetGetStatement2
(
    OCI_Resultset* rs,
    const otext  * name
);

OCI_SYM_LOCAL OCI_Lob * OcilibResultsetGetLob
(
    OCI_Resultset* rs,
    unsigned int   index
);

OCI_SYM_LOCAL OCI_Lob * OcilibResultsetGetLob2
(
    OCI_Resultset* rs,
    const otext  * name
);

OCI_SYM_LOCAL OCI_File * OcilibResultsetGetFile
(
    OCI_Resultset* rs,
    unsigned int   index
);

OCI_SYM_LOCAL OCI_File * OcilibResultsetGetFile2
(
    OCI_Resultset* rs,
    const otext  * name
);

OCI_SYM_LOCAL OCI_Long * OcilibResultsetGetLong
(
    OCI_Resultset* rs,
    unsigned int   index
);

OCI_SYM_LOCAL OCI_Long * OcilibResultsetGetLong2
(
    OCI_Resultset* rs,
    const otext  * name
);

OCI_SYM_LOCAL unsigned int OcilibResultsetGetDataSize
(
    OCI_Resultset* rs,
    unsigned int   index
);

OCI_SYM_LOCAL unsigned int OcilibResultsetGetDataSize2
(
    OCI_Resultset* rs,
    const otext  * name
);

OCI_SYM_LOCAL boolean OcilibResultsetIsNull
(
    OCI_Resultset* rs,
    unsigned int   index
);

OCI_SYM_LOCAL boolean OcilibResultsetIsNull2
(
    OCI_Resultset* rs,
    const otext  * name
);

OCI_SYM_LOCAL OCI_Statement * OcilibResultsetResultsetGetStatement
(
    OCI_Resultset* rs
);

OCI_SYM_LOCAL unsigned int OcilibResultsetGetDataLength
(
    OCI_Resultset* rs,
    unsigned int   index
);

#endif /* OCILIB_RESULTSET_H_INCLUDED */
