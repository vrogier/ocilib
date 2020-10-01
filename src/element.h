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

OCI_Elem* OcilibElementInitialize
(
    OCI_Connection* con,
    OCI_Elem      * elem,
    void          * handle,
    OCIInd        * pind,
    OCI_TypeInfo  * typinf
);

boolean OcilibElementSetNullIndicator
(
    OCI_Elem* elem,
    OCIInd    value
);

OCI_Elem* OcilibElementCreate
(
    OCI_TypeInfo* typinf
);

boolean OcilibElementFree
(
    OCI_Elem* elem
);

boolean OcilibElementGetBoolean
(
    OCI_Elem* elem
);

OCI_Number* OcilibElementGetNumber
(
    OCI_Elem* elem
);

short OcilibElementGetShort
(
    OCI_Elem* elem
);

unsigned short OcilibElementGetUnsignedShort
(
    OCI_Elem* elem
);

int OcilibElemGetInt
(
    OCI_Elem* elem
);

unsigned int OcilibElementGetUnsignedInt
(
    OCI_Elem* elem
);

big_int OcilibElementGetBigInt
(
    OCI_Elem* elem
);

big_uint OcilibElementGetUnsignedBigInt
(
    OCI_Elem* elem
);

double OcilibElementGetDouble
(
    OCI_Elem* elem
);

float OcilibElementGetFloat
(
    OCI_Elem* elem
);

const otext* OcilibElementGetString
(
    OCI_Elem* elem
);

unsigned int OcilibElementGetRaw
(
    OCI_Elem   * elem,
    void       * value,
    unsigned int len
);

unsigned int OcilibElementGetRawSize
(
    OCI_Elem* elem
);

OCI_Date* OcilibElementGetDate
(
    OCI_Elem* elem
);

OCI_Timestamp* OcilibElementGetTimestamp
(
    OCI_Elem* elem
);

OCI_Interval* OcilibElementGetInterval
(
    OCI_Elem* elem
);

OCI_Lob* OcilibElementGetLob
(
    OCI_Elem* elem
);

OCI_File* OcilibElementGetFile
(
    OCI_Elem* elem
);

OCI_Ref* OcilibElementGetReference
(
    OCI_Elem* elem
);

OCI_Object* OcilibElementGetObject
(
    OCI_Elem* elem
);

OCI_Coll* OcilibElementGetCollection
(
    OCI_Elem* elem
);

boolean OcilibElementSetBoolean
(
    OCI_Elem* elem,
    boolean   value
);

boolean OcilibElementSetNumber
(
    OCI_Elem  * elem,
    OCI_Number* value
);

boolean OcilibElementSetShort
(
    OCI_Elem* elem,
    short     value
);

boolean OcilibElementSetUnsignedShort
(
    OCI_Elem     * elem,
    unsigned short value
);

boolean OcilibElementSetInt
(
    OCI_Elem* elem,
    int       value
);

boolean OcilibElementSetUnsignedInt
(
    OCI_Elem   * elem,
    unsigned int value
);

boolean OcilibElementSetBigInt
(
    OCI_Elem* elem,
    big_int   value
);

boolean OcilibElementSetUnsignedBigInt
(
    OCI_Elem* elem,
    big_uint  value
);

boolean OcilibElementSetDouble
(
    OCI_Elem* elem,
    double    value
);

boolean OcilibElementSetFloat
(
    OCI_Elem* elem,
    float     value
);

boolean OcilibElementSetString
(
    OCI_Elem   * elem,
    const otext* value
);

boolean OcilibElementSetRaw
(
    OCI_Elem   * elem,
    void       * value,
    unsigned int len
);

boolean OcilibElementSetDate
(
    OCI_Elem* elem,
    OCI_Date* value
);

boolean OcilibElementSetTimestamp
(
    OCI_Elem     * elem,
    OCI_Timestamp* value
);

boolean OcilibElementSetInterval
(
    OCI_Elem    * elem,
    OCI_Interval* value
);

boolean OcilibElementSetCollection
(
    OCI_Elem* elem,
    OCI_Coll* value
);

boolean OcilibElementSetObject
(
    OCI_Elem  * elem,
    OCI_Object* value
);

boolean OcilibElementSetLob
(
    OCI_Elem* elem,
    OCI_Lob * value
);

boolean OcilibElementSetFile
(
    OCI_Elem* elem,
    OCI_File* value
);

boolean OcilibElementSetReference
(
    OCI_Elem* elem,
    OCI_Ref * value
);
boolean OcilibElementIsNull
(
    OCI_Elem* elem
);

boolean OcilibElementSetNull
(
    OCI_Elem* elem
);

#endif /* OCILIB_ELEMENT_H_INCLUDED */
