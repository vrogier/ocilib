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

#include "resultset.h"

#include "statement.h"
#include "connection.h"

#include "hash.h"
#include "error.h"
#include "exception.h"
#include "column.h"
#include "format.h"
#include "macro.h"
#include "memory.h"
#include "strings.h"
#include "helpers.h"

#include "collection.h"
#include "date.h"
#include "file.h"
#include "interval.h"
#include "list.h"
#include "lob.h"
#include "long.h"
#include "number.h"
#include "object.h"
#include "ref.h"
#include "timestamp.h"

OCI_Resultset* ResultsetCreate
(
    OCI_Statement * stmt,
    int            size
);

boolean ResultsetInit
(
    OCI_Resultset* rs
);

boolean ResultsetFree
(
    OCI_Resultset* rs
);

OCI_Resultset* ResultsetGetResultset
(
    OCI_Statement* stmt
);

OCI_Resultset* ResultsetGetNextResultset
(
    OCI_Statement* stmt
);

boolean ResultsetFetchPrev
(
    OCI_Resultset* rs
);

boolean ResultsetFetchNext
(
    OCI_Resultset* rs
);

boolean ResultsetFetchFirst
(
    OCI_Resultset* rs
);

boolean ResultsetFetchLast
(
    OCI_Resultset* rs
);

boolean ResultsetFetchSeek
(
    OCI_Resultset* rs,
    unsigned int   mode,
    int            offset
);

unsigned int ResultsetGetRowCount
(
    OCI_Resultset* rs
);

unsigned int ResultsetGetCurrentRow
(
    OCI_Resultset* rs
);

unsigned int ResultsetGetColumnCount
(
    OCI_Resultset* rs
);

OCI_Column* ResultsetGetColumn
(
    OCI_Resultset* rs,
    unsigned int   index
);

OCI_Column* ResultsetGetColumn2
(
    OCI_Resultset* rs,
    const otext* name
);

unsigned int ResultsetGetColumnIndex
(
    OCI_Resultset* rs,
    const otext* name
);

boolean ResultsetSetStructNumericType
(
    OCI_Resultset* rs,
    unsigned int   index,
    unsigned int   type
);

boolean ResultsetSetStructNumericType2
(
    OCI_Resultset* rs,
    const otext* name,
    unsigned int   type
);

boolean ResultsetGetStruct
(
    OCI_Resultset* rs,
    void* row_struct,
    void* row_struct_ind
);

OCI_Number* ResultsetGetNumber
(
    OCI_Resultset* rs,
    unsigned int   index
);

OCI_Number* ResultsetGetNumber2
(
    OCI_Resultset* rs,
    const otext* name
);

short ResultsetGetShort
(
    OCI_Resultset* rs,
    unsigned int   index
);

short ResultsetGetShort2
(
    OCI_Resultset* rs,
    const otext* name
);

unsigned short ResultsetGetUnsignedShort
(
    OCI_Resultset* rs,
    unsigned int   index
);

unsigned short ResultsetGetUnsignedShort2
(
    OCI_Resultset* rs,
    const otext* name
);

int ResultsetGetInt
(
    OCI_Resultset* rs,
    unsigned int   index
);

int ResultsetGetInt2
(
    OCI_Resultset* rs,
    const otext* name
);

unsigned int ResultsetGetUnsignedInt
(
    OCI_Resultset* rs,
    unsigned int   index
);

unsigned int ResultsetGetUnsignedInt2
(
    OCI_Resultset* rs,
    const otext* name
);

big_int ResultsetGetBigInt
(
    OCI_Resultset* rs,
    unsigned int   index
);

big_int ResultsetGetBigInt2
(
    OCI_Resultset* rs,
    const otext* name
);

big_uint ResultsetGetUnsignedBigInt
(
    OCI_Resultset* rs,
    unsigned int   index
);

big_uint ResultsetGetUnsignedBigInt2
(
    OCI_Resultset* rs,
    const otext* name
);

const otext* ResultsetGetString
(
    OCI_Resultset* rs,
    unsigned int   index
);

const otext* ResultsetGetString2
(
    OCI_Resultset* rs,
    const otext* name
);

unsigned int ResultsetGetRaw
(
    OCI_Resultset* rs,
    unsigned int   index,
    void* buffer,
    unsigned int   len
);

unsigned int ResultsetGetRaw2
(
    OCI_Resultset* rs,
    const otext* name,
    void* buffer,
    unsigned int   len
);

double ResultsetGetDouble
(
    OCI_Resultset* rs,
    unsigned int   index
);

double ResultsetGetDouble2
(
    OCI_Resultset* rs,
    const otext* name
);

float ResultsetGetFloat
(
    OCI_Resultset* rs,
    unsigned int   index
);

float ResultsetGetFloat2
(
    OCI_Resultset* rs,
    const otext* name
);

OCI_Date* ResultsetGetDate
(
    OCI_Resultset* rs,
    unsigned int   index
);

OCI_Date* ResultsetGetDate2
(
    OCI_Resultset* rs,
    const otext* name
);

OCI_Timestamp* ResultsetGetTimestamp
(
    OCI_Resultset* rs,
    unsigned int   index
);

OCI_Timestamp* ResultsetGetTimestamp2
(
    OCI_Resultset* rs,
    const otext* name
);

OCI_Interval* ResultsetGetInterval
(
    OCI_Resultset* rs,
    unsigned int   index
);

OCI_Interval* ResultsetGetInterval2
(
    OCI_Resultset* rs,
    const otext* name
);


OCI_Object* ResultsetGetObject
(
    OCI_Resultset* rs,
    unsigned int   index
);

OCI_Object* ResultsetGetObject2
(
    OCI_Resultset* rs,
    const otext* name
);

OCI_Coll* ResultsetGetColl
(
    OCI_Resultset* rs,
    unsigned int   index
);

OCI_Coll* ResultsetGetColl2
(
    OCI_Resultset* rs,
    const otext* name
);

OCI_Ref* ResultsetGetRef
(
    OCI_Resultset* rs,
    unsigned int   index
);

OCI_Ref* ResultsetGetRef2
(
    OCI_Resultset* rs,
    const otext* name
);

OCI_Statement* ResultsetGetStatement
(
    OCI_Resultset* rs,
    unsigned int   index
);

OCI_Statement* ResultsetGetStatement2
(
    OCI_Resultset* rs,
    const otext* name
);

OCI_Lob* ResultsetGetLob
(
    OCI_Resultset* rs,
    unsigned int   index
);

OCI_Lob* ResultsetGetLob2
(
    OCI_Resultset* rs,
    const otext* name
);

OCI_File* ResultsetGetFile
(
    OCI_Resultset* rs,
    unsigned int   index
);

OCI_File* ResultsetGetFile2
(
    OCI_Resultset* rs,
    const otext* name
);

OCI_Long* ResultsetGetLong
(
    OCI_Resultset* rs,
    unsigned int   index
);

OCI_Long* ResultsetGetLong2
(
    OCI_Resultset* rs,
    const otext* name
);

unsigned int ResultsetGetDataSize
(
    OCI_Resultset* rs,
    unsigned int   index
);

unsigned int ResultsetGetDataSize2
(
    OCI_Resultset* rs,
    const otext* name
);

boolean ResultsetIsNull
(
    OCI_Resultset* rs,
    unsigned int   index
);

boolean ResultsetIsNull2
(
    OCI_Resultset* rs,
    const otext* name
);

OCI_Statement* ResultsetResultsetGetStatement
(
    OCI_Resultset* rs
);

unsigned int ResultsetGetDataLength
(
    OCI_Resultset* rs,
    unsigned int   index
);

#endif /* OCILIB_RESULTSET_H_INCLUDED */
