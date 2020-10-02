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

OCI_SYM_LOCAL void OcilibObjectGetUserStructSize
(
    OCI_TypeInfo* typinf,
    size_t      * p_size,
    size_t      * p_align
);

OCI_SYM_LOCAL OCI_Object * OcilibObjectInitialize
(
    OCI_Connection *con,
    OCI_Object     *obj,
    void           *handle,
    OCI_TypeInfo   *typinf,
    OCI_Object     *parent,
    int             index,
    boolean         reset
);

OCI_SYM_LOCAL OCI_Object * OcilibObjectCreate
(
    OCI_Connection *con,
    OCI_TypeInfo   *typinf
);

OCI_SYM_LOCAL boolean OcilibObjectFree
(
    OCI_Object *obj
);

OCI_SYM_LOCAL OCI_Object ** OcilibObjectCreateArray
(
    OCI_Connection *con,
    OCI_TypeInfo   *typinf,
    unsigned int    nbelem
);

OCI_SYM_LOCAL boolean OcilibObjectFreeArray
(
    OCI_Object **objs
);

OCI_SYM_LOCAL boolean OcilibObjectAssign
(
    OCI_Object *obj,
    OCI_Object *obj_src
);

OCI_SYM_LOCAL boolean OcilibObjectGetBoolean
(
    OCI_Object  *obj,
    const otext *attr
);

OCI_SYM_LOCAL OCI_Number * OcilibObjectGetNumber
(
    OCI_Object  *obj,
    const otext *attr
);

OCI_SYM_LOCAL short OcilibObjectGetShort
(
    OCI_Object  *obj,
    const otext *attr
);

OCI_SYM_LOCAL unsigned short OcilibObjectGetUnsignedShort
(
    OCI_Object  *obj,
    const otext *attr
);

OCI_SYM_LOCAL int OcilibObjectGetInt
(
    OCI_Object  *obj,
    const otext *attr
);

OCI_SYM_LOCAL unsigned int OcilibObjectGetUnsignedInt
(
    OCI_Object  *obj,
    const otext *attr
);

OCI_SYM_LOCAL big_int OcilibObjectGetBigInt
(
    OCI_Object  *obj,
    const otext *attr
);

OCI_SYM_LOCAL big_uint OcilibObjectGetUnsignedBigInt
(
    OCI_Object  *obj,
    const otext *attr
);

OCI_SYM_LOCAL double OcilibObjectGetDouble
(
    OCI_Object  *obj,
    const otext *attr
);

OCI_SYM_LOCAL float OcilibObjectGetFloat
(
    OCI_Object  *obj,
    const otext *attr
);

OCI_SYM_LOCAL const otext * OcilibObjectGetString
(
    OCI_Object  *obj,
    const otext *attr
);

OCI_SYM_LOCAL int OcilibObjectGetRaw
(
    OCI_Object  *obj,
    const otext *attr,
    void        *buffer,
    unsigned int len
);

OCI_SYM_LOCAL unsigned int OcilibObjectGetRawSize
(
    OCI_Object  *obj,
    const otext *attr
);

OCI_SYM_LOCAL OCI_Date * OcilibObjectGetDate
(
    OCI_Object  *obj,
    const otext *attr
);

OCI_SYM_LOCAL OCI_Timestamp * OcilibObjectGetTimestamp
(
    OCI_Object  *obj,
    const otext *attr
);

OCI_SYM_LOCAL OCI_Interval * OcilibObjectGetInterval
(
    OCI_Object  *obj,
    const otext *attr
);

OCI_SYM_LOCAL OCI_Coll * OcilibObjectGetColl
(
    OCI_Object  *obj,
    const otext *attr
);

OCI_SYM_LOCAL OCI_Object * OcilibObjectGetObject
(
    OCI_Object  *obj,
    const otext *attr
);

OCI_SYM_LOCAL OCI_Lob * OcilibObjectGetLob
(
    OCI_Object  *obj,
    const otext *attr
);

OCI_SYM_LOCAL OCI_File * OcilibObjectGetFile
(
    OCI_Object  *obj,
    const otext *attr
);

OCI_SYM_LOCAL OCI_Ref * OcilibObjectGetReference
(
    OCI_Object  *obj,
    const otext *attr
);

OCI_SYM_LOCAL boolean OcilibObjectSetBoolean
(
    OCI_Object  *obj,
    const otext *attr,
    boolean      value
);

OCI_SYM_LOCAL boolean OcilibObjectSetNumber
(
    OCI_Object  *obj,
    const otext *attr,
    OCI_Number  *value
);

OCI_SYM_LOCAL boolean OcilibObjectSetShort
(
    OCI_Object  *obj,
    const otext *attr,
    short        value
);

OCI_SYM_LOCAL boolean OcilibObjectSetUnsignedShort
(
    OCI_Object    *obj,
    const otext   *attr,
    unsigned short value
);

OCI_SYM_LOCAL boolean OcilibObjectSetInt
(
    OCI_Object  *obj,
    const otext *attr,
    int          value
);

OCI_SYM_LOCAL boolean OcilibObjectSetUnsignedInt
(
    OCI_Object  *obj,
    const otext *attr,
    unsigned int value
);

OCI_SYM_LOCAL boolean OcilibObjectSetBigInt
(
    OCI_Object  *obj,
    const otext *attr,
    big_int      value
);

OCI_SYM_LOCAL boolean OcilibObjectSetUnsignedBigInt
(
    OCI_Object  *obj,
    const otext *attr,
    big_uint     value
);

OCI_SYM_LOCAL boolean OcilibObjectSetDouble
(
    OCI_Object  *obj,
    const otext *attr,
    double       value
);

OCI_SYM_LOCAL boolean OcilibObjectSetFloat
(
    OCI_Object  *obj,
    const otext *attr,
    float        value
);

OCI_SYM_LOCAL boolean OcilibObjectSetString
(
    OCI_Object  *obj,
    const otext *attr,
    const otext *value
);

OCI_SYM_LOCAL boolean OcilibObjectSetRaw
(
    OCI_Object  *obj,
    const otext *attr,
    void       * value,
    unsigned int len
);

OCI_SYM_LOCAL boolean OcilibObjectSetDate
(
    OCI_Object * obj,
    const otext *attr,
    OCI_Date    *value
);

OCI_SYM_LOCAL boolean OcilibObjectSetTimestamp
(
    OCI_Object   * obj,
    const otext   *attr,
    OCI_Timestamp *value
);

OCI_SYM_LOCAL boolean OcilibObjectSetInterval
(
    OCI_Object   *obj,
    const otext  *attr,
    OCI_Interval *value
);

OCI_SYM_LOCAL boolean OcilibObjectSetColl
(
    OCI_Object  *obj,
    const otext *attr,
    OCI_Coll    *value
);

OCI_SYM_LOCAL boolean OcilibObjectSetObject
(
    OCI_Object  *obj,
    const otext *attr,
    OCI_Object  *value
);

OCI_SYM_LOCAL boolean OcilibObjectSetLob
(
    OCI_Object  *obj,
    const otext *attr,
    OCI_Lob     *value
);

OCI_SYM_LOCAL boolean OcilibObjectSetFile
(
    OCI_Object  *obj,
    const otext *attr,
    OCI_File    *value
);

OCI_SYM_LOCAL boolean OcilibObjectSetReference
(
    OCI_Object  *obj,
    const otext *attr,
    OCI_Ref     *value
);

OCI_SYM_LOCAL boolean OcilibObjectSetNull
(
    OCI_Object  *obj,
    const otext *attr
);

OCI_SYM_LOCAL boolean OcilibObjectIsNull
(
    OCI_Object  *obj,
    const otext *attr
);

OCI_TypeInfo * OcilibObjectGetTypeInfo
(
    OCI_Object *obj
);

OCI_SYM_LOCAL unsigned int OcilibObjectGetType
(
    OCI_Object *obj
);

OCI_SYM_LOCAL boolean OcilibObjectGetSelfRef
(
    OCI_Object *obj,
    OCI_Ref    *ref
);

OCI_SYM_LOCAL boolean OcilibObjectGetStruct
(
    OCI_Object *obj,
    void      **pp_struct,
    void      **pp_ind
);

OCI_SYM_LOCAL boolean OcilibObjectToString
(
    OCI_Object   *obj,
    unsigned int *size,
    otext        *str
);

#endif /* OCILIB_OBJECT_H_INCLUDED */
