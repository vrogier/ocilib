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

#ifndef OCILIB_OBJECT_H_INCLUDED
#define OCILIB_OBJECT_H_INCLUDED

#include "types.h"

void OcilibObjectGetUserStructSize
(
    OCI_TypeInfo* typinf,
    size_t      * p_size,
    size_t      * p_align
);

OCI_Object * OcilibObjectInitialize
(
    OCI_Connection *con,
    OCI_Object     *obj,
    void           *handle,
    OCI_TypeInfo   *typinf,
    OCI_Object     *parent,
    int             index,
    boolean         reset
);

OCI_Object * OcilibObjectCreate
(
    OCI_Connection *con,
    OCI_TypeInfo   *typinf
);

boolean OcilibObjectFree
(
    OCI_Object *obj
);

OCI_Object ** OcilibObjectCreateArray
(
    OCI_Connection *con,
    OCI_TypeInfo   *typinf,
    unsigned int    nbelem
);

boolean OcilibObjectFreeArray
(
    OCI_Object **objs
);

boolean OcilibObjectAssign
(
    OCI_Object *obj,
    OCI_Object *obj_src
);

boolean OcilibObjectGetBoolean
(
    OCI_Object  *obj,
    const otext *attr
);

OCI_Number * OcilibObjectGetNumber
(
    OCI_Object  *obj,
    const otext *attr
);

short OcilibObjectGetShort
(
    OCI_Object  *obj,
    const otext *attr
);

unsigned short OcilibObjectGetUnsignedShort
(
    OCI_Object  *obj,
    const otext *attr
);

int OcilibObjectGetInt
(
    OCI_Object  *obj,
    const otext *attr
);

unsigned int OcilibObjectGetUnsignedInt
(
    OCI_Object  *obj,
    const otext *attr
);

big_int OcilibObjectGetBigInt
(
    OCI_Object  *obj,
    const otext *attr
);

big_uint OcilibObjectGetUnsignedBigInt
(
    OCI_Object  *obj,
    const otext *attr
);

double OcilibObjectGetDouble
(
    OCI_Object  *obj,
    const otext *attr
);

float OcilibObjectGetFloat
(
    OCI_Object  *obj,
    const otext *attr
);

const otext * OcilibObjectGetString
(
    OCI_Object  *obj,
    const otext *attr
);

int OcilibObjectGetRaw
(
    OCI_Object  *obj,
    const otext *attr,
    void        *buffer,
    unsigned int len
);

unsigned int OcilibObjectGetRawSize
(
    OCI_Object  *obj,
    const otext *attr
);

OCI_Date * OcilibObjectGetDate
(
    OCI_Object  *obj,
    const otext *attr
);

OCI_Timestamp * OcilibObjectGetTimestamp
(
    OCI_Object  *obj,
    const otext *attr
);

OCI_Interval * OcilibObjectGetInterval
(
    OCI_Object  *obj,
    const otext *attr
);

OCI_Coll * OcilibObjectGetColl
(
    OCI_Object  *obj,
    const otext *attr
);

OCI_Object * OcilibObjectGetObject
(
    OCI_Object  *obj,
    const otext *attr
);

OCI_Lob * OcilibObjectGetLob
(
    OCI_Object  *obj,
    const otext *attr
);

OCI_File * OcilibObjectGetFile
(
    OCI_Object  *obj,
    const otext *attr
);

OCI_Ref * OcilibObjectGetReference
(
    OCI_Object  *obj,
    const otext *attr
);

boolean OcilibObjectSetBoolean
(
    OCI_Object  *obj,
    const otext *attr,
    boolean      value
);

boolean OcilibObjectSetNumber
(
    OCI_Object  *obj,
    const otext *attr,
    OCI_Number  *value
);

boolean OcilibObjectSetShort
(
    OCI_Object  *obj,
    const otext *attr,
    short        value
);

boolean OcilibObjectSetUnsignedShort
(
    OCI_Object    *obj,
    const otext   *attr,
    unsigned short value
);

boolean OcilibObjectSetInt
(
    OCI_Object  *obj,
    const otext *attr,
    int          value
);

boolean OcilibObjectSetUnsignedInt
(
    OCI_Object  *obj,
    const otext *attr,
    unsigned int value
);

boolean OcilibObjectSetBigInt
(
    OCI_Object  *obj,
    const otext *attr,
    big_int      value
);

boolean OcilibObjectSetUnsignedBigInt
(
    OCI_Object  *obj,
    const otext *attr,
    big_uint     value
);

boolean OcilibObjectSetDouble
(
    OCI_Object  *obj,
    const otext *attr,
    double       value
);

boolean OcilibObjectSetFloat
(
    OCI_Object  *obj,
    const otext *attr,
    float        value
);

boolean OcilibObjectSetString
(
    OCI_Object  *obj,
    const otext *attr,
    const otext *value
);

boolean OcilibObjectSetRaw
(
    OCI_Object  *obj,
    const otext *attr,
    void       * value,
    unsigned int len
);

boolean OcilibObjectSetDate
(
    OCI_Object * obj,
    const otext *attr,
    OCI_Date    *value
);

boolean OcilibObjectSetTimestamp
(
    OCI_Object   * obj,
    const otext   *attr,
    OCI_Timestamp *value
);

boolean OcilibObjectSetInterval
(
    OCI_Object   *obj,
    const otext  *attr,
    OCI_Interval *value
);

boolean OcilibObjectSetColl
(
    OCI_Object  *obj,
    const otext *attr,
    OCI_Coll    *value
);

boolean OcilibObjectSetObject
(
    OCI_Object  *obj,
    const otext *attr,
    OCI_Object  *value
);

boolean OcilibObjectSetLob
(
    OCI_Object  *obj,
    const otext *attr,
    OCI_Lob     *value
);

boolean OcilibObjectSetFile
(
    OCI_Object  *obj,
    const otext *attr,
    OCI_File    *value
);

boolean OcilibObjectSetReference
(
    OCI_Object  *obj,
    const otext *attr,
    OCI_Ref     *value
);

boolean OcilibObjectSetNull
(
    OCI_Object  *obj,
    const otext *attr
);

boolean OcilibObjectIsNull
(
    OCI_Object  *obj,
    const otext *attr
);

OCI_TypeInfo * OcilibObjectGetTypeInfo
(
    OCI_Object *obj
);

unsigned int OcilibObjectGetType
(
    OCI_Object *obj
);

boolean OcilibObjectGetSelfRef
(
    OCI_Object *obj,
    OCI_Ref    *ref
);

boolean OcilibObjectGetStruct
(
    OCI_Object *obj,
    void      **pp_struct,
    void      **pp_ind
);

boolean OcilibObjectToString
(
    OCI_Object   *obj,
    unsigned int *size,
    otext        *str
);

#endif /* OCILIB_OBJECT_H_INCLUDED */
