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

#ifndef OCILIB_STRING_UTILS_H_INCLUDED
#define OCILIB_STRING_UTILS_H_INCLUDED

#include "types.h"

OCI_SYM_LOCAL size_t OcilibStringLength
(
    void const* ptr,
    size_t      size_elem
);

OCI_SYM_LOCAL unsigned int OcilibStringBinaryToString
(
    const unsigned char* binary,
    unsigned int         binary_size,
    otext              * buffer
);

OCI_SYM_LOCAL boolean OcilibStringRequestBuffer
(
    otext      ** buffer,
    unsigned int* buffer_size,
    unsigned int  request_size
);

OCI_SYM_LOCAL void OcilibStringTranslate
(
    void * src,
    void * dst,
    int    char_count,
    size_t size_char_in,
    size_t size_char_out
);

#define OcilibStringAnsiToNative(s, d, l) \
    OcilibStringTranslate( (void *) (s), (void *) (d), l, sizeof(char),   sizeof(otext) )

#define OcilibStringNativeToAnsi(s, d, l) \
    OcilibStringTranslate( (void *) (s), (void *) (d), l, sizeof(otext),  sizeof(char)  )

#define OcilibStringUTF16ToUTF32(s, d, l) \
   OcilibStringTranslate( (void *) (s), (void *) (d), l, sizeof(short),  sizeof(int)   )

#define OcilibStringUTF32ToUTF16(s, d, l) \
    OcilibStringTranslate( (void *) (s), (void *) (d), l, sizeof(int),    sizeof(short) )

#define OcilibStringOracleToNative(s, d, l) \
    OcilibStringTranslate( (void *) (s), (void *) (d), l, sizeof(dbtext), sizeof(otext) )

#define OcilibStringRawCopy(s, d, l) \
    OcilibStringTranslate( (void *) (s), (void *) (d), l, sizeof(otext),  sizeof(otext) )

OCI_SYM_LOCAL dbtext * OcilibStringGetDBString
(
    const otext* src,
    int        * size
);

OCI_SYM_LOCAL void OcilibStringReleaseDBString
(
    dbtext* str
);

OCI_SYM_LOCAL int OcilibStringCopyDBStringToNativeString
(
    const dbtext* src,
    otext       * dst,
    int           len
);

OCI_SYM_LOCAL otext * OcilibStringDuplicateFromDBString
(
    const dbtext* src,
    int           len
);

OCI_SYM_LOCAL otext * OcilibStringFromStringPtr
(
    OCIEnv      * env,
    OCIString   * str,
    otext      ** buffer,
    unsigned int* buffer_size
);

OCI_SYM_LOCAL boolean OcilibStringToStringPtr
(
    OCIEnv     * env,
    OCIString ** str,
    OCIError   * err,
    const otext* value
);

OCI_SYM_LOCAL boolean OcilibStringFreeStringPtr
(
    OCIEnv    * env,
    OCIString** str,
    OCIError  * err
);

OCI_SYM_LOCAL unsigned int OcilibStringGetFromType
(
    OCI_Connection* con,
    OCI_Column    * col,
    void          * data,
    unsigned int    data_size,
    otext         * buffer,
    unsigned int    buffer_size,
    boolean         quote
);

OCI_SYM_LOCAL unsigned int OcilibStringAddToBuffer
(
    otext      * buffer,
    unsigned int offset,
    const otext* str,
    unsigned int length,
    boolean      check_quote
);

OCI_SYM_LOCAL unsigned int OcilibStringGetTypeName
(
    const otext* source,
    otext      * dest,
    unsigned int length
);

OCI_SYM_LOCAL unsigned int OcilibStringGetFullTypeName
(
    const otext* schema,
    const otext* package,
    const otext* type,
    const otext* link,
    otext      * name,
    unsigned int length
);

OCI_SYM_LOCAL boolean OcilibStringGetAttribute
(
    OCI_Connection* con,
    void          * handle,
    unsigned int    type,
    unsigned int    attr,
    otext        ** str,
    unsigned int  * size
);

OCI_SYM_LOCAL boolean OcilibStringSetAttribute
(
    OCI_Connection* con,
    void          * handle,
    unsigned int    type,
    unsigned int    attr,
    otext        ** str,
    const otext   * value
);

#endif /* OCILIB_STRING_UTILS_H_INCLUDED */
