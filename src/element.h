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

#ifndef OCILIB_ELEMENT_H_INCLUDED
#define OCILIB_ELEMENT_H_INCLUDED

#include "types.h"

OCI_SYM_LOCAL OCI_Element* OcilibElementInitialize
(
    OCI_Element  * elem,
    void         * handle,
    OCIInd       * pind,
    OCI_TypeInfo * typinf
);

OCI_SYM_LOCAL OCI_Boolean OcilibElementSetNullIndicator
(
    OCI_Element* elem,
    OCIInd       value
);

OCI_SYM_LOCAL OCI_Element* OcilibElementCreate
(
    OCI_TypeInfo* typinf
);

OCI_SYM_LOCAL OCI_Boolean OcilibElementFree
(
    OCI_Element* elem
);

OCI_SYM_LOCAL OCI_Boolean OcilibElementGetBoolean
(
    OCI_Element* elem
);

OCI_SYM_LOCAL OCI_Number* OcilibElementGetNumber
(
    OCI_Element* elem
);

OCI_SYM_LOCAL OCI_Int16 OcilibElementGetInt16
(
    OCI_Element* elem
);

OCI_SYM_LOCAL OCI_UInt16 OcilibElementGetUInt16
(
    OCI_Element* elem
);

OCI_SYM_LOCAL OCI_Int32 OcilibElementGetInt32
(
    OCI_Element* elem
);

OCI_SYM_LOCAL OCI_UInt32 OcilibElementGetUInt32
(
    OCI_Element* elem
);

OCI_SYM_LOCAL OCI_Int64 OcilibElementGetInt64
(
    OCI_Element* elem
);

OCI_SYM_LOCAL OCI_UInt64 OcilibElementGetUInt64
(
    OCI_Element* elem
);

OCI_SYM_LOCAL OCI_Double OcilibElementGetDouble
(
    OCI_Element* elem
);

OCI_SYM_LOCAL OCI_Float OcilibElementGetFloat
(
    OCI_Element* elem
);

OCI_SYM_LOCAL const otext* OcilibElementGetString
(
    OCI_Element* elem
);

OCI_SYM_LOCAL OCI_UInt32 OcilibElementGetRaw
(
    OCI_Element * elem,
    void        * value,
    OCI_UInt32    len
);

OCI_SYM_LOCAL OCI_UInt32 OcilibElementGetRawSize
(
    OCI_Element* elem
);

OCI_SYM_LOCAL OCI_Date* OcilibElementGetDate
(
    OCI_Element* elem
);

OCI_SYM_LOCAL OCI_Timestamp* OcilibElementGetTimestamp
(
    OCI_Element* elem
);

OCI_SYM_LOCAL OCI_Interval* OcilibElementGetInterval
(
    OCI_Element* elem
);

OCI_SYM_LOCAL OCI_Lob* OcilibElementGetLob
(
    OCI_Element* elem
);

OCI_SYM_LOCAL OCI_File* OcilibElementGetFile
(
    OCI_Element* elem
);

OCI_SYM_LOCAL OCI_Reference* OcilibElementGetReference
(
    OCI_Element* elem
);

OCI_SYM_LOCAL OCI_Object* OcilibElementGetObject
(
    OCI_Element* elem
);

OCI_SYM_LOCAL OCI_Collection* OcilibElementGetCollection
(
    OCI_Element* elem
);

OCI_SYM_LOCAL OCI_Boolean OcilibElementSetBoolean
(
    OCI_Element * elem,
    OCI_Boolean   value
);

OCI_SYM_LOCAL OCI_Boolean OcilibElementSetNumber
(
    OCI_Element * elem,
    OCI_Number  * value
);

OCI_SYM_LOCAL OCI_Boolean OcilibElementSetInt16
(
    OCI_Element* elem,
    OCI_Int16    value
);

OCI_SYM_LOCAL OCI_Boolean OcilibElementSetUInt16
(
    OCI_Element * elem,
    OCI_UInt16    value
);

OCI_SYM_LOCAL OCI_Boolean OcilibElementSetInt32
(
    OCI_Element* elem,
    OCI_Int32    value
);

OCI_SYM_LOCAL OCI_Boolean OcilibElementSetUInt32
(
    OCI_Element * elem,
    OCI_UInt32    value
);

OCI_SYM_LOCAL OCI_Boolean OcilibElementSetInt64
(
    OCI_Element* elem,
    OCI_Int64    value
);

OCI_SYM_LOCAL OCI_Boolean OcilibElementSetUInt64
(
    OCI_Element * elem,
    OCI_UInt64    value
);

OCI_SYM_LOCAL OCI_Boolean OcilibElementSetDouble
(
    OCI_Element * elem,
    OCI_Double    value
);

OCI_SYM_LOCAL OCI_Boolean OcilibElementSetFloat
(
    OCI_Element* elem,
    OCI_Float    value
);

OCI_SYM_LOCAL OCI_Boolean OcilibElementSetString
(
    OCI_Element * elem,
    const otext * value
);

OCI_SYM_LOCAL OCI_Boolean OcilibElementSetRaw
(
    OCI_Element * elem,
    void        * value,
    OCI_UInt32    len
);

OCI_SYM_LOCAL OCI_Boolean OcilibElementSetDate
(
    OCI_Element* elem,
    OCI_Date   * value
);

OCI_SYM_LOCAL OCI_Boolean OcilibElementSetTimestamp
(
    OCI_Element  * elem,
    OCI_Timestamp* value
);

OCI_SYM_LOCAL OCI_Boolean OcilibElementSetInterval
(
    OCI_Element * elem,
    OCI_Interval* value
);

OCI_SYM_LOCAL OCI_Boolean OcilibElementSetCollection
(
    OCI_Element   * elem,
    OCI_Collection* value
);

OCI_SYM_LOCAL OCI_Boolean OcilibElementSetObject
(
    OCI_Element * elem,
    OCI_Object  * value
);

OCI_SYM_LOCAL OCI_Boolean OcilibElementSetLob
(
    OCI_Element* elem,
    OCI_Lob    * value
);

OCI_SYM_LOCAL OCI_Boolean OcilibElementSetFile
(
    OCI_Element* elem,
    OCI_File   * value
);

OCI_SYM_LOCAL OCI_Boolean OcilibElementSetReference
(
    OCI_Element   * elem,
    OCI_Reference * value
);

OCI_SYM_LOCAL OCI_Boolean OcilibElementIsNull
(
    OCI_Element* elem
);

OCI_SYM_LOCAL OCI_Boolean OcilibElementSetNull
(
    OCI_Element* elem
);

#endif /* OCILIB_ELEMENT_H_INCLUDED */
