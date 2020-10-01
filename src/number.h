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

#ifndef OCILIB_NUMBER_H_INCLUDED
#define OCILIB_NUMBER_H_INCLUDED

#include "types.h"

boolean OcilibNumberTranslateValue
(
    OCI_Connection* con,
    void          * in_value,
    uword           in_type,
    void          * out_value,
    uword           out_type
);

OCI_Number* OcilibNumberInitialize
(
    OCI_Connection* con,
    OCI_Number    * number,
    OCINumber     * buffer
);

boolean OcilibNumberFromStringInternal
(
    OCI_Connection* con,
    void          * out_value,
    uword           type,
    const otext   * in_value,
    const otext   * fmt
);

boolean OcilibNumberToStringInternal
(
    OCI_Connection* con,
    void          * number,
    unsigned int    type,
    otext         * out_value,
    int             out_value_size,
    const otext   * fmt
);

OCI_Number* OcilibNumberCreate
(
    OCI_Connection* con
);

boolean OcilibNumberFree
(
    OCI_Number* number
);

OCI_Number** OcilibNumberCreateArray
(
    OCI_Connection* con,
    unsigned int    nbelem
);

boolean OcilibNumberFreeArray
(
    OCI_Number** numbmers
);

boolean OcilibNumberAssign
(
    OCI_Number* number,
    OCI_Number* number_src
);

boolean OcilibNumberToString
(
    OCI_Number * number,
    const otext* fmt,
    int          size,
    otext      * str
);

boolean OcilibNumberFromString
(
    OCI_Number * number,
    const otext* str,
    const otext* fmt
);

unsigned char* OcilibNumberGetContent
(
    OCI_Number* number
);

boolean OcilibNumberSetContent
(
    OCI_Number   * number,
    unsigned char* content
);

boolean OcilibNumberSetValue
(
    OCI_Number * number,
    unsigned int type,
    void       * value
);

boolean OcilibNumberGetValue
(
    OCI_Number * number,
    unsigned int type,
    void       * value
);

boolean OcilibNumberAdd
(
    OCI_Number * number,
    unsigned int type,
    void       * value
);

boolean OcilibNumberSub
(
    OCI_Number * number,
    unsigned int type,
    void       * value
);

boolean OcilibNumberMultiply
(
    OCI_Number * number,
    unsigned int type,
    void       * value
);

boolean OcilibNumberDivide
(
    OCI_Number * number,
    unsigned int type,
    void       * value
);

int OcilibNumberCompare
(
    OCI_Number* number1,
    OCI_Number* number2
);

#endif /* OCILIB_NUMBER_H_INCLUDED */
