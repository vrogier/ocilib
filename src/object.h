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

void ObjectGetUserStructSize
(
    OCI_TypeInfo* typinf,
    size_t      * p_size,
    size_t      * p_align
);

OCI_Object * ObjectInitialize
(
    OCI_Connection *con,
    OCI_Object     *obj,
    void           *handle,
    OCI_TypeInfo   *typinf,
    OCI_Object     *parent,
    int             index,
    boolean         reset
);

OCI_Object * ObjectCreate
(
    OCI_Connection *con,
    OCI_TypeInfo   *typinf
);

boolean ObjectFree
(
    OCI_Object *obj
);

OCI_Object ** ObjectCreateArray
(
    OCI_Connection *con,
    OCI_TypeInfo   *typinf,
    unsigned int    nbelem
);

boolean ObjectFreeArray
(
    OCI_Object **objs
);

boolean ObjectAssign
(
    OCI_Object *obj,
    OCI_Object *obj_src
);

boolean ObjectGetBoolean
(
    OCI_Object  *obj,
    const otext *attr
);

OCI_Number * ObjectGetNumber
(
    OCI_Object  *obj,
    const otext *attr
);

short ObjectGetShort
(
    OCI_Object  *obj,
    const otext *attr
);

unsigned short ObjectGetUnsignedShort
(
    OCI_Object  *obj,
    const otext *attr
);

int ObjectGetInt
(
    OCI_Object  *obj,
    const otext *attr
);

unsigned int ObjectGetUnsignedInt
(
    OCI_Object  *obj,
    const otext *attr
);

big_int ObjectGetBigInt
(
    OCI_Object  *obj,
    const otext *attr
);

big_uint ObjectGetUnsignedBigInt
(
    OCI_Object  *obj,
    const otext *attr
);

double ObjectGetDouble
(
    OCI_Object  *obj,
    const otext *attr
);

float ObjectGetFloat
(
    OCI_Object  *obj,
    const otext *attr
);

const otext * ObjectGetString
(
    OCI_Object  *obj,
    const otext *attr
);

int ObjectGetRaw
(
    OCI_Object  *obj,
    const otext *attr,
    void        *buffer,
    unsigned int len
);

unsigned int ObjectGetRawSize
(
    OCI_Object  *obj,
    const otext *attr
);

OCI_Date * ObjectGetDate
(
    OCI_Object  *obj,
    const otext *attr
);

OCI_Timestamp * ObjectGetTimestamp
(
    OCI_Object  *obj,
    const otext *attr
);

OCI_Interval * ObjectGetInterval
(
    OCI_Object  *obj,
    const otext *attr
);

OCI_Coll * ObjectGetColl
(
    OCI_Object  *obj,
    const otext *attr
);

OCI_Object * ObjectGetObject
(
    OCI_Object  *obj,
    const otext *attr
);

OCI_Lob * ObjectGetLob
(
    OCI_Object  *obj,
    const otext *attr
);

OCI_File * ObjectGetFile
(
    OCI_Object  *obj,
    const otext *attr
);

OCI_Ref * ObjectGetReference
(
    OCI_Object  *obj,
    const otext *attr
);

boolean ObjectSetBoolean
(
    OCI_Object  *obj,
    const otext *attr,
    boolean      value
);

boolean ObjectSetNumber
(
    OCI_Object  *obj,
    const otext *attr,
    OCI_Number  *value
);

boolean ObjectSetShort
(
    OCI_Object  *obj,
    const otext *attr,
    short        value
);

boolean ObjectSetUnsignedShort
(
    OCI_Object    *obj,
    const otext   *attr,
    unsigned short value
);

boolean ObjectSetInt
(
    OCI_Object  *obj,
    const otext *attr,
    int          value
);

boolean ObjectSetUnsignedInt
(
    OCI_Object  *obj,
    const otext *attr,
    unsigned int value
);

boolean ObjectSetBigInt
(
    OCI_Object  *obj,
    const otext *attr,
    big_int      value
);

boolean ObjectSetUnsignedBigInt
(
    OCI_Object  *obj,
    const otext *attr,
    big_uint     value
);

boolean ObjectSetDouble
(
    OCI_Object  *obj,
    const otext *attr,
    double       value
);

boolean ObjectSetFloat
(
    OCI_Object  *obj,
    const otext *attr,
    float        value
);

boolean ObjectSetString
(
    OCI_Object  *obj,
    const otext *attr,
    const otext *value
);

boolean ObjectSetRaw
(
    OCI_Object  *obj,
    const otext *attr,
    void       * value,
    unsigned int len
);

boolean ObjectSetDate
(
    OCI_Object * obj,
    const otext *attr,
    OCI_Date    *value
);

boolean ObjectSetTimestamp
(
    OCI_Object   * obj,
    const otext   *attr,
    OCI_Timestamp *value
);

boolean ObjectSetInterval
(
    OCI_Object   *obj,
    const otext  *attr,
    OCI_Interval *value
);

boolean ObjectSetColl
(
    OCI_Object  *obj,
    const otext *attr,
    OCI_Coll    *value
);

boolean ObjectSetObject
(
    OCI_Object  *obj,
    const otext *attr,
    OCI_Object  *value
);

boolean ObjectSetLob
(
    OCI_Object  *obj,
    const otext *attr,
    OCI_Lob     *value
);

boolean ObjectSetFile
(
    OCI_Object  *obj,
    const otext *attr,
    OCI_File    *value
);

boolean ObjectSetReference
(
    OCI_Object  *obj,
    const otext *attr,
    OCI_Ref     *value
);

boolean ObjectSetNull
(
    OCI_Object  *obj,
    const otext *attr
);

boolean ObjectIsNull
(
    OCI_Object  *obj,
    const otext *attr
);

OCI_TypeInfo * ObjectGetTypeInfo
(
    OCI_Object *obj
);

unsigned int ObjectGetType
(
    OCI_Object *obj
);

boolean ObjectGetSelfRef
(
    OCI_Object *obj,
    OCI_Ref    *ref
);

boolean ObjectGetStruct
(
    OCI_Object *obj,
    void      **pp_struct,
    void      **pp_ind
);

boolean ObjectToString
(
    OCI_Object   *obj,
    unsigned int *size,
    otext        *str
);

#endif /* OCILIB_OBJECT_H_INCLUDED */
