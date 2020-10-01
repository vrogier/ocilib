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

#ifndef OCILIB_RESULTSET_H_INCLUDED
#define OCILIB_RESULTSET_H_INCLUDED

#include "types.h"

boolean OcilibResultsetInitialize
(
    OCI_Resultset* rs
);

OCI_Resultset* OcilibResultsetCreate
(
    OCI_Statement * stmt,
    int             size
);

boolean OcilibResultsetFree
(
    OCI_Resultset* rs
);

boolean OcilibResultsetFetchPrev
(
    OCI_Resultset* rs
);

boolean OcilibResultsetFetchNext
(
    OCI_Resultset* rs
);

boolean OcilibResultsetFetchFirst
(
    OCI_Resultset* rs
);

boolean OcilibResultsetFetchLast
(
    OCI_Resultset* rs
);

boolean OcilibResultsetFetchSeek
(
    OCI_Resultset* rs,
    unsigned int   mode,
    int            offset
);

unsigned int OcilibResultsetGetRowCount
(
    OCI_Resultset* rs
);

unsigned int OcilibResultsetGetCurrentRow
(
    OCI_Resultset* rs
);

unsigned int OcilibResultsetGetColumnCount
(
    OCI_Resultset* rs
);

OCI_Column* OcilibResultsetGetColumn
(
    OCI_Resultset* rs,
    unsigned int   index
);

OCI_Column* OcilibResultsetGetColumn2
(
    OCI_Resultset* rs,
    const otext  * name
);

unsigned int OcilibResultsetGetColumnIndex
(
    OCI_Resultset* rs,
    const otext  * name
);

boolean OcilibResultsetSetStructNumericType
(
    OCI_Resultset* rs,
    unsigned int   index,
    unsigned int   type
);

boolean OcilibResultsetSetStructNumericType2
(
    OCI_Resultset* rs,
    const otext  * name,
    unsigned int   type
);

boolean OcilibResultsetGetStruct
(
    OCI_Resultset* rs,
    void         * row_struct,
    void         * row_struct_ind
);

OCI_Number* OcilibResultsetGetNumber
(
    OCI_Resultset* rs,
    unsigned int   index
);

OCI_Number* OcilibResultsetGetNumber2
(
    OCI_Resultset* rs,
    const otext  * name
);

short OcilibResultsetGetShort
(
    OCI_Resultset* rs,
    unsigned int   index
);

short OcilibResultsetGetShort2
(
    OCI_Resultset* rs,
    const otext  * name
);

unsigned short OcilibResultsetGetUnsignedShort
(
    OCI_Resultset* rs,
    unsigned int   index
);

unsigned short OcilibResultsetGetUnsignedShort2
(
    OCI_Resultset* rs,
    const otext  * name
);

int OcilibResultsetGetInt
(
    OCI_Resultset* rs,
    unsigned int   index
);

int OcilibResultsetGetInt2
(
    OCI_Resultset* rs,
    const otext  * name
);

unsigned int OcilibResultsetGetUnsignedInt
(
    OCI_Resultset* rs,
    unsigned int   index
);

unsigned int OcilibResultsetGetUnsignedInt2
(
    OCI_Resultset* rs,
    const otext  * name
);

big_int OcilibResultsetGetBigInt
(
    OCI_Resultset* rs,
    unsigned int   index
);

big_int OcilibResultsetGetBigInt2
(
    OCI_Resultset* rs,
    const otext  * name
);

big_uint OcilibResultsetGetUnsignedBigInt
(
    OCI_Resultset* rs,
    unsigned int   index
);

big_uint OcilibResultsetGetUnsignedBigInt2
(
    OCI_Resultset* rs,
    const otext  * name
);

const otext* OcilibResultsetGetString
(
    OCI_Resultset* rs,
    unsigned int   index
);

const otext* OcilibResultsetGetString2
(
    OCI_Resultset* rs,
    const otext  * name
);

unsigned int OcilibResultsetGetRaw
(
    OCI_Resultset* rs,
    unsigned int   index,
    void         * buffer,
    unsigned int   len
);

unsigned int OcilibResultsetGetRaw2
(
    OCI_Resultset* rs,
    const otext  * name,
    void         * buffer,
    unsigned int   len
);

double OcilibResultsetGetDouble
(
    OCI_Resultset* rs,
    unsigned int   index
);

double OcilibResultsetGetDouble2
(
    OCI_Resultset* rs,
    const otext  * name
);

float OcilibResultsetGetFloat
(
    OCI_Resultset* rs,
    unsigned int   index
);

float OcilibResultsetGetFloat2
(
    OCI_Resultset* rs,
    const otext  * name
);

OCI_Date* OcilibResultsetGetDate
(
    OCI_Resultset* rs,
    unsigned int   index
);

OCI_Date* OcilibResultsetGetDate2
(
    OCI_Resultset* rs,
    const otext  * name
);

OCI_Timestamp* OcilibResultsetGetTimestamp
(
    OCI_Resultset* rs,
    unsigned int   index
);

OCI_Timestamp* OcilibResultsetGetTimestamp2
(
    OCI_Resultset* rs,
    const otext  * name
);

OCI_Interval* OcilibResultsetGetInterval
(
    OCI_Resultset* rs,
    unsigned int   index
);

OCI_Interval* OcilibResultsetGetInterval2
(
    OCI_Resultset* rs,
    const otext  * name
);

OCI_Object* OcilibResultsetGetObject
(
    OCI_Resultset* rs,
    unsigned int   index
);

OCI_Object* OcilibResultsetGetObject2
(
    OCI_Resultset* rs,
    const otext  * name
);

OCI_Coll* OcilibResultsetGetColl
(
    OCI_Resultset* rs,
    unsigned int   index
);

OCI_Coll* OcilibResultsetGetColl2
(
    OCI_Resultset* rs,
    const otext  * name
);

OCI_Ref* OcilibResultsetGetReference
(
    OCI_Resultset* rs,
    unsigned int   index
);

OCI_Ref* OcilibResultsetGetReference2
(
    OCI_Resultset* rs,
    const otext  * name
);

OCI_Statement* OcilibResultsetGetStatement
(
    OCI_Resultset* rs,
    unsigned int   index
);

OCI_Statement* OcilibResultsetGetStatement2
(
    OCI_Resultset* rs,
    const otext  * name
);

OCI_Lob* OcilibResultsetGetLob
(
    OCI_Resultset* rs,
    unsigned int   index
);

OCI_Lob* OcilibResultsetGetLob2
(
    OCI_Resultset* rs,
    const otext  * name
);

OCI_File* OcilibResultsetGetFile
(
    OCI_Resultset* rs,
    unsigned int   index
);

OCI_File* OcilibResultsetGetFile2
(
    OCI_Resultset* rs,
    const otext  * name
);

OCI_Long* OcilibResultsetGetLong
(
    OCI_Resultset* rs,
    unsigned int   index
);

OCI_Long* OcilibResultsetGetLong2
(
    OCI_Resultset* rs,
    const otext  * name
);

unsigned int OcilibResultsetGetDataSize
(
    OCI_Resultset* rs,
    unsigned int   index
);

unsigned int OcilibResultsetGetDataSize2
(
    OCI_Resultset* rs,
    const otext  * name
);

boolean OcilibResultsetIsNull
(
    OCI_Resultset* rs,
    unsigned int   index
);

boolean OcilibResultsetIsNull2
(
    OCI_Resultset* rs,
    const otext  * name
);

OCI_Statement* OcilibResultsetResultsetGetStatement
(
    OCI_Resultset* rs
);

unsigned int OcilibResultsetGetDataLength
(
    OCI_Resultset* rs,
    unsigned int   index
);

#endif /* OCILIB_RESULTSET_H_INCLUDED */
