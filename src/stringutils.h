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

#ifndef OCILIB_STRING_UTILS_H_INCLUDED
#define OCILIB_STRING_UTILS_H_INCLUDED

#include "types.h"

OCI_SYM_LOCAL OCI_Size OcilibStringLength
(
    void const* ptr,
    OCI_Size    size_elem
);

OCI_SYM_LOCAL OCI_UInt32 OcilibStringBinaryToString
(
    const unsigned char* binary,
    OCI_UInt32           binary_size,
    otext              * buffer
);

OCI_SYM_LOCAL OCI_Boolean OcilibStringRequestBuffer
(
    OCI_Environment *env,
    otext          **buffer,
    OCI_UInt32      *buffer_size,
    OCI_UInt32       request_size
);

OCI_SYM_LOCAL void OcilibStringTranslate
(
    void    * src,
    void    * dst,
    OCI_Int32 char_count,
    OCI_Size  size_char_in,
    OCI_Size  size_char_out
);

#define OcilibStringMbcsToNative(s, d, l) \
        OcilibStringTranslate((void *) (s), (void *) (d), l, sizeof(char),   sizeof(otext))

#define OcilibStringNativeToMbcs(s, d, l) \
        OcilibStringTranslate((void *) (s), (void *) (d), l, sizeof(otext),  sizeof(char))

#define OcilibStringUTF16ToUTF32(s, d, l) \
        OcilibStringTranslate((void *) (s), (void *) (d), l, sizeof(OCI_Int16),  sizeof(OCI_Int32))

#define OcilibStringUTF32ToUTF16(s, d, l) \
        OcilibStringTranslate((void *) (s), (void *) (d), l, sizeof(OCI_Int32),    sizeof(OCI_Int16))

#define OcilibStringOracleToNative(s, d, l) \
        OcilibStringTranslate((void *) (s), (void *) (d), l, sizeof(dbtext), sizeof(otext))

#define OcilibStringRawCopy(s, d, l) \
        OcilibStringTranslate((void *) (s), (void *) (d), l, sizeof(otext),  sizeof(otext))

OCI_SYM_LOCAL dbtext * OcilibStringGetDBString
(
    OCI_Environment *env,
    const otext     *src,
    OCI_Int32       *size
);

OCI_SYM_LOCAL dbtext* OcilibStringGetDBAnsiString
(
    OCI_Environment* env,
    const otext    * src,
    OCI_Int32      * size
);

OCI_SYM_LOCAL void OcilibStringReleaseDBAnsiString
(
    OCI_Environment* env,
    dbtext         * str
);

OCI_SYM_LOCAL void OcilibStringReleaseDBString
(
    OCI_Environment *env,
    dbtext          *str
);

OCI_SYM_LOCAL OCI_Int32 OcilibStringCopyDBStringToNativeString
(
    OCI_Environment *env,
    const dbtext    *src,
    otext           *dst,
    OCI_Int32        len
);

OCI_SYM_LOCAL otext * OcilibStringDuplicateFromDBString
(
    OCI_Environment *env,
    const dbtext    *src,
    OCI_Int32        len
);

OCI_SYM_LOCAL otext * OcilibStringFromStringPtr
(
    OCI_Environment *libenv,
    OCIEnv          *envh,
    OCIString       *str,
    otext          **buffer,
    OCI_UInt32      *buffer_size
);

OCI_SYM_LOCAL OCI_Boolean OcilibStringToStringPtr
(
    OCI_Environment *libenv,
    OCIEnv          *envh,
    OCIString      **str,
    OCIError        *err,
    const otext     *value
);

OCI_SYM_LOCAL OCI_Boolean OcilibStringFreeStringPtr
(
    OCI_Environment *libenv,
    OCIEnv          *envh,
    OCIString      **str,
    OCIError        *err
);

OCI_SYM_LOCAL OCI_UInt32 OcilibStringGetFromType
(
    OCI_Connection* con,
    OCI_Column    * col,
    void          * data,
    OCI_UInt32      data_size,
    otext         * buffer,
    OCI_UInt32      buffer_size,
    OCI_Boolean     quote
);

OCI_SYM_LOCAL OCI_UInt32 OcilibStringAddToBuffer
(
    otext      * buffer,
    OCI_UInt32   offset,
    const otext* str,
    OCI_UInt32   length,
    OCI_Boolean  check_quote
);

OCI_SYM_LOCAL OCI_UInt32 OcilibStringGetTypeName
(
    const otext* source,
    otext      * dest,
    OCI_UInt32   length
);

OCI_SYM_LOCAL OCI_UInt32 OcilibStringGetFullTypeName
(
    const otext* schema,
    const otext* package,
    const otext* type,
    const otext* link,
    otext      * name,
    OCI_UInt32   length
);

OCI_SYM_LOCAL OCI_Boolean OcilibStringGetAttribute
(
    OCI_Connection* con,
    void          * handle,
    OCI_UInt32      type,
    OCI_UInt32      attr,
    otext        ** str,
    OCI_UInt32    * size
);

OCI_SYM_LOCAL OCI_Boolean OcilibStringSetAttribute
(
    OCI_Connection* con,
    void          * handle,
    OCI_UInt32      type,
    OCI_UInt32      attr,
    otext        ** str,
    const otext   * value
);

OCI_SYM_LOCAL otext * OcilibStringDuplicate
(
    OCI_Environment *env,
    const otext    * src
);

OCI_SYM_LOCAL char * OcilibStringMbcsDuplicate
(
    OCI_Environment *env,
    const char     * src
);

OCI_SYM_LOCAL OCI_Int32 OcilibStringCaseCompare
(
    const otext* str1,
    const otext* str2
);

OCI_SYM_LOCAL OCI_Int32 OcilibStringMbcsCaseCompare
(
    const char* str1,
    const char* str2
);

OCI_SYM_LOCAL OCI_Int32 OcilibStringFormat
(
    otext       * str,
    OCI_Int32     size,
    const otext * format,
    ...
) OCI_ATTR_FORMAT(3, 4);

#endif /* OCILIB_STRING_UTILS_H_INCLUDED */
