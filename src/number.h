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

#ifndef OCILIB_NUMBER_H_INCLUDED
#define OCILIB_NUMBER_H_INCLUDED

#include "types.h"

OCI_SYM_LOCAL OCI_Boolean OcilibNumberTranslateValue
(
    OCI_Environment* env,
    void           * in_value,
    uword            in_type,
    void           * out_value,
    uword            out_type
);

OCI_SYM_LOCAL OCI_Number * OcilibNumberInitialize
(
    OCI_Environment* env,
    OCI_Number     * number,
    OCINumber      * buffer
);

OCI_SYM_LOCAL OCI_Boolean OcilibNumberFromStringInternal
(
    OCI_Environment* env,
    void           * out_value,
    uword            type,
    const otext    * in_value,
    const otext    * fmt
);

OCI_SYM_LOCAL OCI_Boolean OcilibNumberToStringInternal
(
    OCI_Environment* env,
    void           * number,
    OCI_NumericType  type,
    otext          * out_value,
    OCI_Int32              out_value_size,
    const otext    * fmt
);

OCI_SYM_LOCAL OCI_Number * OcilibNumberCreate
(
    OCI_Environment* env
);

OCI_SYM_LOCAL OCI_Boolean OcilibNumberFree
(
    OCI_Number* number
);

OCI_SYM_LOCAL OCI_Number ** OcilibNumberCreateArray
(
    OCI_Environment* env,
    OCI_UInt32     nbelem
);

OCI_SYM_LOCAL OCI_Boolean OcilibNumberFreeArray
(
    OCI_Number** numbmers
);

OCI_SYM_LOCAL OCI_Boolean OcilibNumberAssign
(
    OCI_Number* number,
    OCI_Number* number_src
);

OCI_SYM_LOCAL OCI_Boolean OcilibNumberToString
(
    OCI_Number * number,
    const otext* fmt,
    OCI_Int32          size,
    otext      * str
);

OCI_SYM_LOCAL OCI_Boolean OcilibNumberFromString
(
    OCI_Number * number,
    const otext* str,
    const otext* fmt
);

OCI_SYM_LOCAL unsigned char * OcilibNumberGetContent
(
    OCI_Number* number
);

OCI_SYM_LOCAL OCI_Boolean OcilibNumberSetContent
(
    OCI_Number   * number,
    unsigned char* content
);

OCI_SYM_LOCAL OCI_Boolean OcilibNumberSetValue
(
    OCI_Number    * number,
    OCI_NumericType type,
    void          * value
);

OCI_SYM_LOCAL OCI_Boolean OcilibNumberGetValue
(
    OCI_Number    * number,
    OCI_NumericType type,
    void          * value
);

OCI_SYM_LOCAL OCI_Boolean OcilibNumberAdd
(
    OCI_Number    * number,
    OCI_NumericType type,
    void          * value
);

OCI_SYM_LOCAL OCI_Boolean OcilibNumberSub
(
    OCI_Number    * number,
    OCI_NumericType type,
    void          * value
);

OCI_SYM_LOCAL OCI_Boolean OcilibNumberMultiply
(
    OCI_Number    * number,
    OCI_NumericType type,
    void          * value
);

OCI_SYM_LOCAL OCI_Boolean OcilibNumberDivide
(
    OCI_Number    * number,
    OCI_NumericType type,
    void          * value
);

OCI_SYM_LOCAL OCI_Int32 OcilibNumberCompare
(
    OCI_Number* number1,
    OCI_Number* number2
);

#endif /* OCILIB_NUMBER_H_INCLUDED */
