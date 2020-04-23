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
 
#ifndef OCILIB_ELEMENT_H_INCLUDED
#define OCILIB_ELEMENT_H_INCLUDED

#include "types.h"

OCI_Elem* ElemInit
(
    OCI_Connection* con,
    OCI_Elem* elem,
    void* handle,
    OCIInd* pind,
    OCI_TypeInfo* typinf
);

boolean ElemSetNullIndicator
(
    OCI_Elem* elem,
    OCIInd    value
);

OCI_Elem* ElemCreate
(
    OCI_TypeInfo* typinf
);

boolean ElemFree
(
    OCI_Elem* elem
);

boolean ElemGetBoolean
(
    OCI_Elem* elem
);

OCI_Number* ElemGetNumber
(
    OCI_Elem* elem
);

short ElemGetShort
(
    OCI_Elem* elem
);

unsigned short ElemGetUnsignedShort
(
    OCI_Elem* elem
);

int ElemGetInt
(
    OCI_Elem* elem
);

unsigned int ElemGetUnsignedInt
(
    OCI_Elem* elem
);

big_int ElemGetBigInt
(
    OCI_Elem* elem
);

big_uint ElemGetUnsignedBigInt
(
    OCI_Elem* elem
);

double ElemGetDouble
(
    OCI_Elem* elem
);

float ElemGetFloat
(
    OCI_Elem* elem
);

const otext* ElemGetString
(
    OCI_Elem* elem
);

unsigned int ElemGetRaw
(
    OCI_Elem* elem,
    void* value,
    unsigned int len
);

unsigned int ElemGetRawSize
(
    OCI_Elem* elem
);

OCI_Date* ElemGetDate
(
    OCI_Elem* elem
);

OCI_Timestamp* ElemGetTimestamp
(
    OCI_Elem* elem
);

OCI_Interval* ElemGetInterval
(
    OCI_Elem* elem
);

OCI_Lob* ElemGetLob
(
    OCI_Elem* elem
);

OCI_File* ElemGetFile
(
    OCI_Elem* elem
);

OCI_Ref* ElemGetRef
(
    OCI_Elem* elem
);

OCI_Object* ElemGetObject
(
    OCI_Elem* elem
);

OCI_Coll* ElemGetColl
(
    OCI_Elem* elem
);

boolean ElemSetBoolean
(
    OCI_Elem* elem,
    boolean   value
);

boolean ElemSetNumber
(
    OCI_Elem* elem,
    OCI_Number* value
);

boolean ElemSetShort
(
    OCI_Elem* elem,
    short     value
);

boolean ElemSetUnsignedShort
(
    OCI_Elem* elem,
    unsigned short value
);

boolean ElemSetInt
(
    OCI_Elem* elem,
    int       value
);

boolean ElemSetUnsignedInt
(
    OCI_Elem* elem,
    unsigned int value
);

boolean ElemSetBigInt
(
    OCI_Elem* elem,
    big_int   value
);

boolean ElemSetUnsignedBigInt
(
    OCI_Elem* elem,
    big_uint  value
);

boolean ElemSetDouble
(
    OCI_Elem* elem,
    double    value
);

boolean ElemSetFloat
(
    OCI_Elem* elem,
    float     value
);

boolean ElemSetString
(
    OCI_Elem* elem,
    const otext* value
);

boolean ElemSetRaw
(
    OCI_Elem* elem,
    void* value,
    unsigned int len
);

boolean ElemSetDate
(
    OCI_Elem* elem,
    OCI_Date* value
);

boolean ElemSetTimestamp
(
    OCI_Elem* elem,
    OCI_Timestamp* value
);

boolean ElemSetInterval
(
    OCI_Elem* elem,
    OCI_Interval* value
);

boolean ElemSetColl
(
    OCI_Elem* elem,
    OCI_Coll* value
);

boolean ElemSetObject
(
    OCI_Elem* elem,
    OCI_Object* value
);

boolean ElemSetLob
(
    OCI_Elem* elem,
    OCI_Lob* value
);

boolean ElemSetFile
(
    OCI_Elem* elem,
    OCI_File* value
);

boolean ElemSetRef
(
    OCI_Elem* elem,
    OCI_Ref* value
);
boolean ElemIsNull
(
    OCI_Elem* elem
);

boolean ElemSetNull
(
    OCI_Elem* elem
);

#endif /* OCILIB_ELEMENT_H_INCLUDED */
