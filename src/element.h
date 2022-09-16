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

#ifndef OCILIB_ELEMENT_H_INCLUDED
#define OCILIB_ELEMENT_H_INCLUDED

#include "types.h"

OCI_SYM_LOCAL OCI_Elem* OcilibElementInitialize
(
    OCI_Connection* con,
    OCI_Elem      * elem,
    void          * handle,
    OCIInd        * pind,
    OCI_TypeInfo  * typinf
);

OCI_SYM_LOCAL boolean OcilibElementSetNullIndicator
(
    OCI_Elem* elem,
    OCIInd    value
);

OCI_SYM_LOCAL OCI_Elem* OcilibElementCreate
(
    OCI_TypeInfo* typinf
);

OCI_SYM_LOCAL boolean OcilibElementFree
(
    OCI_Elem* elem
);

OCI_SYM_LOCAL boolean OcilibElementGetBoolean
(
    OCI_Elem* elem
);

OCI_SYM_LOCAL OCI_Number* OcilibElementGetNumber
(
    OCI_Elem* elem
);

OCI_SYM_LOCAL short OcilibElementGetShort
(
    OCI_Elem* elem
);

OCI_SYM_LOCAL unsigned short OcilibElementGetUnsignedShort
(
    OCI_Elem* elem
);

OCI_SYM_LOCAL int OcilibElementGetInt
(
    OCI_Elem* elem
);

OCI_SYM_LOCAL unsigned int OcilibElementGetUnsignedInt
(
    OCI_Elem* elem
);

OCI_SYM_LOCAL big_int OcilibElementGetBigInt
(
    OCI_Elem* elem
);

OCI_SYM_LOCAL big_uint OcilibElementGetUnsignedBigInt
(
    OCI_Elem* elem
);

OCI_SYM_LOCAL double OcilibElementGetDouble
(
    OCI_Elem* elem
);

OCI_SYM_LOCAL float OcilibElementGetFloat
(
    OCI_Elem* elem
);

OCI_SYM_LOCAL const otext* OcilibElementGetString
(
    OCI_Elem* elem
);

OCI_SYM_LOCAL unsigned int OcilibElementGetRaw
(
    OCI_Elem   * elem,
    void       * value,
    unsigned int len
);

OCI_SYM_LOCAL unsigned int OcilibElementGetRawSize
(
    OCI_Elem* elem
);

OCI_SYM_LOCAL OCI_Date* OcilibElementGetDate
(
    OCI_Elem* elem
);

OCI_SYM_LOCAL OCI_Timestamp* OcilibElementGetTimestamp
(
    OCI_Elem* elem
);

OCI_SYM_LOCAL OCI_Interval* OcilibElementGetInterval
(
    OCI_Elem* elem
);

OCI_SYM_LOCAL OCI_Lob* OcilibElementGetLob
(
    OCI_Elem* elem
);

OCI_SYM_LOCAL OCI_File* OcilibElementGetFile
(
    OCI_Elem* elem
);

OCI_SYM_LOCAL OCI_Ref* OcilibElementGetReference
(
    OCI_Elem* elem
);

OCI_SYM_LOCAL OCI_Object* OcilibElementGetObject
(
    OCI_Elem* elem
);

OCI_SYM_LOCAL OCI_Coll* OcilibElementGetCollection
(
    OCI_Elem* elem
);

OCI_SYM_LOCAL boolean OcilibElementSetBoolean
(
    OCI_Elem* elem,
    boolean   value
);

OCI_SYM_LOCAL boolean OcilibElementSetNumber
(
    OCI_Elem  * elem,
    OCI_Number* value
);

OCI_SYM_LOCAL boolean OcilibElementSetShort
(
    OCI_Elem* elem,
    short     value
);

OCI_SYM_LOCAL boolean OcilibElementSetUnsignedShort
(
    OCI_Elem     * elem,
    unsigned short value
);

OCI_SYM_LOCAL boolean OcilibElementSetInt
(
    OCI_Elem* elem,
    int       value
);

OCI_SYM_LOCAL boolean OcilibElementSetUnsignedInt
(
    OCI_Elem   * elem,
    unsigned int value
);

OCI_SYM_LOCAL boolean OcilibElementSetBigInt
(
    OCI_Elem* elem,
    big_int   value
);

OCI_SYM_LOCAL boolean OcilibElementSetUnsignedBigInt
(
    OCI_Elem* elem,
    big_uint  value
);

OCI_SYM_LOCAL boolean OcilibElementSetDouble
(
    OCI_Elem* elem,
    double    value
);

OCI_SYM_LOCAL boolean OcilibElementSetFloat
(
    OCI_Elem* elem,
    float     value
);

OCI_SYM_LOCAL boolean OcilibElementSetString
(
    OCI_Elem   * elem,
    const otext* value
);

OCI_SYM_LOCAL boolean OcilibElementSetRaw
(
    OCI_Elem   * elem,
    void       * value,
    unsigned int len
);

OCI_SYM_LOCAL boolean OcilibElementSetDate
(
    OCI_Elem* elem,
    OCI_Date* value
);

OCI_SYM_LOCAL boolean OcilibElementSetTimestamp
(
    OCI_Elem     * elem,
    OCI_Timestamp* value
);

OCI_SYM_LOCAL boolean OcilibElementSetInterval
(
    OCI_Elem    * elem,
    OCI_Interval* value
);

OCI_SYM_LOCAL boolean OcilibElementSetCollection
(
    OCI_Elem* elem,
    OCI_Coll* value
);

OCI_SYM_LOCAL boolean OcilibElementSetObject
(
    OCI_Elem  * elem,
    OCI_Object* value
);

OCI_SYM_LOCAL boolean OcilibElementSetLob
(
    OCI_Elem* elem,
    OCI_Lob * value
);

OCI_SYM_LOCAL boolean OcilibElementSetFile
(
    OCI_Elem* elem,
    OCI_File* value
);

OCI_SYM_LOCAL boolean OcilibElementSetReference
(
    OCI_Elem* elem,
    OCI_Ref * value
);

OCI_SYM_LOCAL boolean OcilibElementIsNull
(
    OCI_Elem* elem
);

OCI_SYM_LOCAL boolean OcilibElementSetNull
(
    OCI_Elem* elem
);

#endif /* OCILIB_ELEMENT_H_INCLUDED */
