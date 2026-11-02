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

#ifndef OCILIB_ELEMENT_H_INCLUDED
#define OCILIB_ELEMENT_H_INCLUDED

#include "types.h"

OCI_SYM_LOCAL OCI_Elem* OcilibElementInitialize
(
    OCI_Elem     * elem,
    void         * handle,
    OCIInd       * pind,
    OCI_TypeInfo * typinf
);

OCI_SYM_LOCAL OCI_Boolean OcilibElementSetNullIndicator
(
    OCI_Elem* elem,
    OCIInd    value
);

OCI_SYM_LOCAL OCI_Elem* OcilibElementCreate
(
    OCI_TypeInfo* typinf
);

OCI_SYM_LOCAL OCI_Boolean OcilibElementFree
(
    OCI_Elem* elem
);

OCI_SYM_LOCAL OCI_Boolean OcilibElementGetBoolean
(
    OCI_Elem* elem
);

OCI_SYM_LOCAL OCI_Number* OcilibElementGetNumber
(
    OCI_Elem* elem
);

OCI_SYM_LOCAL OCI_Int16 OcilibElementGetShort
(
    OCI_Elem* elem
);

OCI_SYM_LOCAL OCI_UInt16 OcilibElementGetUnsignedShort
(
    OCI_Elem* elem
);

OCI_SYM_LOCAL OCI_Int32 OcilibElementGetInt
(
    OCI_Elem* elem
);

OCI_SYM_LOCAL OCI_UInt32 OcilibElementGetUnsignedInt
(
    OCI_Elem* elem
);

OCI_SYM_LOCAL OCI_Int64 OcilibElementGetBigInt
(
    OCI_Elem* elem
);

OCI_SYM_LOCAL OCI_UInt64 OcilibElementGetUnsignedBigInt
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

OCI_SYM_LOCAL OCI_UInt32 OcilibElementGetRaw
(
    OCI_Elem   * elem,
    void       * value,
    OCI_UInt32 len
);

OCI_SYM_LOCAL OCI_UInt32 OcilibElementGetRawSize
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

OCI_SYM_LOCAL OCI_Boolean OcilibElementSetBoolean
(
    OCI_Elem  * elem,
    OCI_Boolean value
);

OCI_SYM_LOCAL OCI_Boolean OcilibElementSetNumber
(
    OCI_Elem  * elem,
    OCI_Number* value
);

OCI_SYM_LOCAL OCI_Boolean OcilibElementSetShort
(
    OCI_Elem* elem,
    OCI_Int16     value
);

OCI_SYM_LOCAL OCI_Boolean OcilibElementSetUnsignedShort
(
    OCI_Elem     * elem,
    OCI_UInt16 value
);

OCI_SYM_LOCAL OCI_Boolean OcilibElementSetInt
(
    OCI_Elem* elem,
    OCI_Int32       value
);

OCI_SYM_LOCAL OCI_Boolean OcilibElementSetUnsignedInt
(
    OCI_Elem   * elem,
    OCI_UInt32 value
);

OCI_SYM_LOCAL OCI_Boolean OcilibElementSetBigInt
(
    OCI_Elem* elem,
    OCI_Int64   value
);

OCI_SYM_LOCAL OCI_Boolean OcilibElementSetUnsignedBigInt
(
    OCI_Elem* elem,
    OCI_UInt64  value
);

OCI_SYM_LOCAL OCI_Boolean OcilibElementSetDouble
(
    OCI_Elem* elem,
    double    value
);

OCI_SYM_LOCAL OCI_Boolean OcilibElementSetFloat
(
    OCI_Elem* elem,
    float     value
);

OCI_SYM_LOCAL OCI_Boolean OcilibElementSetString
(
    OCI_Elem   * elem,
    const otext* value
);

OCI_SYM_LOCAL OCI_Boolean OcilibElementSetRaw
(
    OCI_Elem   * elem,
    void       * value,
    OCI_UInt32 len
);

OCI_SYM_LOCAL OCI_Boolean OcilibElementSetDate
(
    OCI_Elem* elem,
    OCI_Date* value
);

OCI_SYM_LOCAL OCI_Boolean OcilibElementSetTimestamp
(
    OCI_Elem     * elem,
    OCI_Timestamp* value
);

OCI_SYM_LOCAL OCI_Boolean OcilibElementSetInterval
(
    OCI_Elem    * elem,
    OCI_Interval* value
);

OCI_SYM_LOCAL OCI_Boolean OcilibElementSetCollection
(
    OCI_Elem* elem,
    OCI_Coll* value
);

OCI_SYM_LOCAL OCI_Boolean OcilibElementSetObject
(
    OCI_Elem  * elem,
    OCI_Object* value
);

OCI_SYM_LOCAL OCI_Boolean OcilibElementSetLob
(
    OCI_Elem* elem,
    OCI_Lob * value
);

OCI_SYM_LOCAL OCI_Boolean OcilibElementSetFile
(
    OCI_Elem* elem,
    OCI_File* value
);

OCI_SYM_LOCAL OCI_Boolean OcilibElementSetReference
(
    OCI_Elem* elem,
    OCI_Ref * value
);

OCI_SYM_LOCAL OCI_Boolean OcilibElementIsNull
(
    OCI_Elem* elem
);

OCI_SYM_LOCAL OCI_Boolean OcilibElementSetNull
(
    OCI_Elem* elem
);

#endif /* OCILIB_ELEMENT_H_INCLUDED */
