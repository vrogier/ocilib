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

OCI_Elem* ElementInitialize
(
    OCI_Connection* con,
    OCI_Elem      * elem,
    void          * handle,
    OCIInd        * pind,
    OCI_TypeInfo  * typinf
);

boolean ElementSetNullIndicator
(
    OCI_Elem* elem,
    OCIInd    value
);

OCI_Elem* ElementCreate
(
    OCI_TypeInfo* typinf
);

boolean ElementFree
(
    OCI_Elem* elem
);

boolean ElementGetBoolean
(
    OCI_Elem* elem
);

OCI_Number* ElementGetNumber
(
    OCI_Elem* elem
);

short ElementGetShort
(
    OCI_Elem* elem
);

unsigned short ElementGetUnsignedShort
(
    OCI_Elem* elem
);

int ElemGetInt
(
    OCI_Elem* elem
);

unsigned int ElementGetUnsignedInt
(
    OCI_Elem* elem
);

big_int ElementGetBigInt
(
    OCI_Elem* elem
);

big_uint ElementGetUnsignedBigInt
(
    OCI_Elem* elem
);

double ElementGetDouble
(
    OCI_Elem* elem
);

float ElementGetFloat
(
    OCI_Elem* elem
);

const otext* ElementGetString
(
    OCI_Elem* elem
);

unsigned int ElementGetRaw
(
    OCI_Elem   * elem,
    void       * value,
    unsigned int len
);

unsigned int ElementGetRawSize
(
    OCI_Elem* elem
);

OCI_Date* ElementGetDate
(
    OCI_Elem* elem
);

OCI_Timestamp* ElementGetTimestamp
(
    OCI_Elem* elem
);

OCI_Interval* ElementGetInterval
(
    OCI_Elem* elem
);

OCI_Lob* ElementGetLob
(
    OCI_Elem* elem
);

OCI_File* ElementGetFile
(
    OCI_Elem* elem
);

OCI_Ref* ElementGetReference
(
    OCI_Elem* elem
);

OCI_Object* ElementGetObject
(
    OCI_Elem* elem
);

OCI_Coll* ElementGetCollection
(
    OCI_Elem* elem
);

boolean ElementSetBoolean
(
    OCI_Elem* elem,
    boolean   value
);

boolean ElementSetNumber
(
    OCI_Elem  * elem,
    OCI_Number* value
);

boolean ElementSetShort
(
    OCI_Elem* elem,
    short     value
);

boolean ElementSetUnsignedShort
(
    OCI_Elem     * elem,
    unsigned short value
);

boolean ElementSetInt
(
    OCI_Elem* elem,
    int       value
);

boolean ElementSetUnsignedInt
(
    OCI_Elem   * elem,
    unsigned int value
);

boolean ElementSetBigInt
(
    OCI_Elem* elem,
    big_int   value
);

boolean ElementSetUnsignedBigInt
(
    OCI_Elem* elem,
    big_uint  value
);

boolean ElementSetDouble
(
    OCI_Elem* elem,
    double    value
);

boolean ElementSetFloat
(
    OCI_Elem* elem,
    float     value
);

boolean ElementSetString
(
    OCI_Elem   * elem,
    const otext* value
);

boolean ElementSetRaw
(
    OCI_Elem   * elem,
    void       * value,
    unsigned int len
);

boolean ElementSetDate
(
    OCI_Elem* elem,
    OCI_Date* value
);

boolean ElementSetTimestamp
(
    OCI_Elem     * elem,
    OCI_Timestamp* value
);

boolean ElementSetInterval
(
    OCI_Elem    * elem,
    OCI_Interval* value
);

boolean ElementSetCollection
(
    OCI_Elem* elem,
    OCI_Coll* value
);

boolean ElementSetObject
(
    OCI_Elem  * elem,
    OCI_Object* value
);

boolean ElementSetLob
(
    OCI_Elem* elem,
    OCI_Lob * value
);

boolean ElementSetFile
(
    OCI_Elem* elem,
    OCI_File* value
);

boolean ElementSetReference
(
    OCI_Elem* elem,
    OCI_Ref * value
);
boolean ElementIsNull
(
    OCI_Elem* elem
);

boolean ElementSetNull
(
    OCI_Elem* elem
);

#endif /* OCILIB_ELEMENT_H_INCLUDED */
