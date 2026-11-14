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

#ifndef OCILIB_OBJECT_H_INCLUDED
#define OCILIB_OBJECT_H_INCLUDED

#include "types.h"

OCI_SYM_LOCAL void OcilibObjectGetUserStructSize
(
    OCI_TypeInfo* typinf,
    OCI_Size    * p_size,
    OCI_Size    * p_align
);

OCI_SYM_LOCAL OCI_Object * OcilibObjectInitialize
(
    OCI_Connection *con,
    OCI_Object     *obj,
    void           *handle,
    OCI_TypeInfo   *typinf,
    OCI_Object     *parent,
    OCI_Int32       index,
    OCI_Boolean     reset
);

OCI_SYM_LOCAL OCI_Object * OcilibObjectCreate
(
    OCI_TypeInfo *typinf
);

OCI_SYM_LOCAL OCI_Boolean OcilibObjectFree
(
    OCI_Object *obj
);

OCI_SYM_LOCAL OCI_Object ** OcilibObjectCreateArray
(
    OCI_TypeInfo *typinf,
    OCI_UInt32    nbelem
);

OCI_SYM_LOCAL OCI_Boolean OcilibObjectFreeArray
(
    OCI_Object **objs
);

OCI_SYM_LOCAL OCI_Boolean OcilibObjectAssign
(
    OCI_Object *obj,
    OCI_Object *obj_src
);

OCI_SYM_LOCAL OCI_Boolean OcilibObjectGetBoolean
(
    OCI_Object  *obj,
    const otext *attr
);

OCI_SYM_LOCAL OCI_Number * OcilibObjectGetNumber
(
    OCI_Object  *obj,
    const otext *attr
);

OCI_SYM_LOCAL OCI_Int16 OcilibObjectGetInt16
(
    OCI_Object  *obj,
    const otext *attr
);

OCI_SYM_LOCAL OCI_UInt16 OcilibObjectGetUInt16
(
    OCI_Object  *obj,
    const otext *attr
);

OCI_SYM_LOCAL OCI_Int32 OcilibObjectGetInt32
(
    OCI_Object  *obj,
    const otext *attr
);

OCI_SYM_LOCAL OCI_UInt32 OcilibObjectGetUInt32
(
    OCI_Object  *obj,
    const otext *attr
);

OCI_SYM_LOCAL OCI_Int64 OcilibObjectGetInt64
(
    OCI_Object  *obj,
    const otext *attr
);

OCI_SYM_LOCAL OCI_UInt64 OcilibObjectGetUInt64
(
    OCI_Object  *obj,
    const otext *attr
);

OCI_SYM_LOCAL OCI_Double OcilibObjectGetDouble
(
    OCI_Object  *obj,
    const otext *attr
);

OCI_SYM_LOCAL OCI_Float OcilibObjectGetFloat
(
    OCI_Object  *obj,
    const otext *attr
);

OCI_SYM_LOCAL const otext * OcilibObjectGetString
(
    OCI_Object  *obj,
    const otext *attr
);

OCI_SYM_LOCAL OCI_Int32 OcilibObjectGetRaw
(
    OCI_Object  *obj,
    const otext *attr,
    void        *buffer,
    OCI_UInt32   len
);

OCI_SYM_LOCAL OCI_UInt32 OcilibObjectGetRawSize
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

OCI_SYM_LOCAL OCI_Collection * OcilibObjectGetColl
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

OCI_SYM_LOCAL OCI_Reference * OcilibObjectGetReference
(
    OCI_Object  *obj,
    const otext *attr
);

OCI_SYM_LOCAL OCI_Boolean OcilibObjectSetBoolean
(
    OCI_Object  *obj,
    const otext *attr,
    OCI_Boolean  value
);

OCI_SYM_LOCAL OCI_Boolean OcilibObjectSetNumber
(
    OCI_Object  *obj,
    const otext *attr,
    OCI_Number  *value
);

OCI_SYM_LOCAL OCI_Boolean OcilibObjectSetInt16
(
    OCI_Object  *obj,
    const otext *attr,
    OCI_Int16    value
);

OCI_SYM_LOCAL OCI_Boolean OcilibObjectSetUInt16
(
    OCI_Object  *obj,
    const otext *attr,
    OCI_UInt16   value
);

OCI_SYM_LOCAL OCI_Boolean OcilibObjectSetInt32
(
    OCI_Object  *obj,
    const otext *attr,
    OCI_Int32    value
);

OCI_SYM_LOCAL OCI_Boolean OcilibObjectSetUInt32
(
    OCI_Object  *obj,
    const otext *attr,
    OCI_UInt32   value
);

OCI_SYM_LOCAL OCI_Boolean OcilibObjectSetInt64
(
    OCI_Object  *obj,
    const otext *attr,
    OCI_Int64    value
);

OCI_SYM_LOCAL OCI_Boolean OcilibObjectSetUInt64
(
    OCI_Object  *obj,
    const otext *attr,
    OCI_UInt64   value
);

OCI_SYM_LOCAL OCI_Boolean OcilibObjectSetDouble
(
    OCI_Object  *obj,
    const otext *attr,
    OCI_Double   value
);

OCI_SYM_LOCAL OCI_Boolean OcilibObjectSetFloat
(
    OCI_Object  *obj,
    const otext *attr,
    OCI_Float    value
);

OCI_SYM_LOCAL OCI_Boolean OcilibObjectSetString
(
    OCI_Object  *obj,
    const otext *attr,
    const otext *value
);

OCI_SYM_LOCAL OCI_Boolean OcilibObjectSetRaw
(
    OCI_Object  *obj,
    const otext *attr,
    void       * value,
    OCI_UInt32   len
);

OCI_SYM_LOCAL OCI_Boolean OcilibObjectSetDate
(
    OCI_Object * obj,
    const otext *attr,
    OCI_Date    *value
);

OCI_SYM_LOCAL OCI_Boolean OcilibObjectSetTimestamp
(
    OCI_Object   * obj,
    const otext   *attr,
    OCI_Timestamp *value
);

OCI_SYM_LOCAL OCI_Boolean OcilibObjectSetInterval
(
    OCI_Object   *obj,
    const otext  *attr,
    OCI_Interval *value
);

OCI_SYM_LOCAL OCI_Boolean OcilibObjectSetColl
(
    OCI_Object     *obj,
    const otext    *attr,
    OCI_Collection *value
);

OCI_SYM_LOCAL OCI_Boolean OcilibObjectSetObject
(
    OCI_Object  *obj,
    const otext *attr,
    OCI_Object  *value
);

OCI_SYM_LOCAL OCI_Boolean OcilibObjectSetLob
(
    OCI_Object  *obj,
    const otext *attr,
    OCI_Lob     *value
);

OCI_SYM_LOCAL OCI_Boolean OcilibObjectSetFile
(
    OCI_Object  *obj,
    const otext *attr,
    OCI_File    *value
);

OCI_SYM_LOCAL OCI_Boolean OcilibObjectSetReference
(
    OCI_Object    *obj,
    const otext   *attr,
    OCI_Reference *value
);

OCI_SYM_LOCAL OCI_Boolean OcilibObjectSetNull
(
    OCI_Object  *obj,
    const otext *attr
);

OCI_SYM_LOCAL OCI_Boolean OcilibObjectIsNull
(
    OCI_Object  *obj,
    const otext *attr
);

OCI_SYM_LOCAL OCI_TypeInfo * OcilibObjectGetTypeInfo
(
    OCI_Object *obj
);

OCI_SYM_LOCAL OCI_ObjectType OcilibObjectGetType
(
    OCI_Object *obj
);

OCI_SYM_LOCAL OCI_Boolean OcilibObjectGetSelfReference
(
    OCI_Object    *obj,
    OCI_Reference *ref
);

OCI_SYM_LOCAL OCI_Boolean OcilibObjectGetStruct
(
    OCI_Object *obj,
    void      **pp_struct,
    void      **pp_ind
);

OCI_SYM_LOCAL OCI_Boolean OcilibObjectToString
(
    OCI_Object *obj,
    OCI_UInt32 *size,
    otext      *str
);

#endif /* OCILIB_OBJECT_H_INCLUDED */
