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

#ifndef OCILIB_JSONNODE_H_INCLUDED
#define OCILIB_JSONNODE_H_INCLUDED

#include "json.h"

OCI_SYM_LOCAL OCI_JsonNode * OcilibJsonNodeInitialize
(
    OCI_Json    *json,
    JsonDomNode *handle,
    OCI_JsonType type
);

OCI_SYM_LOCAL OCI_JsonNode * OcilibJsonNodeCreate
(
    OCI_Json    *json,
    OCI_JsonType type
);

OCI_SYM_LOCAL OCI_JsonNode * OcilibJsonNodeCreateString
(
    OCI_Json    *json,
    const otext *value,
    OCI_UInt32   length
);

OCI_SYM_LOCAL OCI_JsonNode * OcilibJsonNodeCreateNumber
(
    OCI_Json   *json,
    OCI_Number *value
);

OCI_SYM_LOCAL OCI_JsonNode * OcilibJsonNodeCreateBoolean
(
    OCI_Json   *json,
    OCI_Boolean value
);

OCI_SYM_LOCAL OCI_JsonNode * OcilibJsonNodeCreateNull
(
    OCI_Json *json
);

OCI_SYM_LOCAL OCI_JsonNode * OcilibJsonNodeCreateBinary
(
    OCI_Json   *json,
    const void *value,
    OCI_UInt32  length
);

OCI_SYM_LOCAL OCI_JsonNode * OcilibJsonNodeCreateDate
(
    OCI_Json *json,
    OCI_Date *value
);

OCI_SYM_LOCAL OCI_JsonNode * OcilibJsonNodeCreateTimestamp
(
    OCI_Json      *json,
    OCI_Timestamp *value
);

OCI_SYM_LOCAL OCI_JsonNode * OcilibJsonNodeCreateInterval
(
    OCI_Json     *json,
    OCI_Interval *value
);

OCI_SYM_LOCAL OCI_JsonNode * OcilibJsonNodeCreateObject
(
    OCI_Json *json
);

OCI_SYM_LOCAL OCI_JsonNode * OcilibJsonNodeCreateArray
(
    OCI_Json *json
);

OCI_SYM_LOCAL OCI_Boolean OcilibJsonNodeFree
(
    OCI_JsonNode *node
);

OCI_SYM_LOCAL OCI_JsonType OcilibJsonNodeGetType
(
    OCI_JsonNode *node
);

OCI_SYM_LOCAL OCI_Boolean OcilibJsonNodeIsNull
(
    OCI_JsonNode *node
);

OCI_SYM_LOCAL OCI_UInt32 OcilibJsonNodeGetFieldCount
(
    OCI_JsonNode *node
);

OCI_SYM_LOCAL OCI_JsonNode * OcilibJsonNodeGetFieldByName
(
    OCI_JsonNode *node,
    const otext  *name
);

OCI_SYM_LOCAL OCI_JsonNode * OcilibJsonNodeGetFieldByIndex
(
    OCI_JsonNode *node,
    OCI_UInt32    index
);

OCI_SYM_LOCAL const otext * OcilibJsonNodeGetFieldName
(
    OCI_JsonNode *node,
    OCI_UInt32    index
);

OCI_SYM_LOCAL OCI_UInt32 OcilibJsonNodeGetItemCount
(
    OCI_JsonNode *node
);

OCI_SYM_LOCAL OCI_JsonNode * OcilibJsonNodeGetItem
(
    OCI_JsonNode *node,
    OCI_UInt32    index
);

OCI_SYM_LOCAL const otext * OcilibJsonNodeGetString
(
    OCI_JsonNode *node,
    OCI_UInt32   *length
);

OCI_SYM_LOCAL OCI_Number * OcilibJsonNodeGetNumber
(
    OCI_JsonNode *node
);

OCI_SYM_LOCAL OCI_Boolean OcilibJsonNodeGetBoolean
(
    OCI_JsonNode *node
);

OCI_SYM_LOCAL const void * OcilibJsonNodeGetBinary
(
    OCI_JsonNode *node,
    OCI_UInt32   *length
);

OCI_SYM_LOCAL OCI_Date * OcilibJsonNodeGetDate
(
    OCI_JsonNode *node
);

OCI_SYM_LOCAL OCI_Timestamp * OcilibJsonNodeGetTimestamp
(
    OCI_JsonNode *node
);

OCI_SYM_LOCAL OCI_Interval * OcilibJsonNodeGetInterval
(
    OCI_JsonNode *node
);

OCI_SYM_LOCAL OCI_Boolean OcilibJsonNodeSetString
(
    OCI_JsonNode *node,
    const otext  *value,
    OCI_UInt32    length
);

OCI_SYM_LOCAL OCI_Boolean OcilibJsonNodeSetNumber
(
    OCI_JsonNode *node,
    OCI_Number   *value
);

OCI_SYM_LOCAL OCI_Boolean OcilibJsonNodeSetBoolean
(
    OCI_JsonNode *node,
    OCI_Boolean   value
);

OCI_SYM_LOCAL OCI_Boolean OcilibJsonNodeSetBinary
(
    OCI_JsonNode *node,
    const void   *value,
    OCI_UInt32    length
);

OCI_SYM_LOCAL OCI_Boolean OcilibJsonNodeSetDate
(
    OCI_JsonNode *node,
    OCI_Date     *value
);

OCI_SYM_LOCAL OCI_Boolean OcilibJsonNodeSetTimestamp
(
    OCI_JsonNode  *node,
    OCI_Timestamp *value
);

OCI_SYM_LOCAL OCI_Boolean OcilibJsonNodeSetInterval
(
    OCI_JsonNode *node,
    OCI_Interval *value
);

OCI_SYM_LOCAL OCI_Boolean OcilibJsonNodeSetNull
(
    OCI_JsonNode *node
);

OCI_SYM_LOCAL OCI_Boolean OcilibJsonNodeSetField
(
    OCI_JsonNode *obj,
    const otext  *name,
    OCI_JsonNode *value
);

OCI_SYM_LOCAL OCI_Boolean OcilibJsonNodeAppendItem
(
    OCI_JsonNode *arr,
    OCI_JsonNode *value
);

OCI_SYM_LOCAL OCI_Boolean OcilibJsonNodeInsertItem
(
    OCI_JsonNode *arr,
    OCI_UInt32    index,
    OCI_JsonNode *value
);

OCI_SYM_LOCAL OCI_Boolean OcilibJsonNodeRemoveItem
(
    OCI_JsonNode *arr,
    OCI_UInt32    index
);

OCI_SYM_LOCAL OCI_UInt32 OcilibJsonNodeRemoveItems
(
    OCI_JsonNode * arr,
    OCI_UInt32     start,
    OCI_UInt32     count
);

OCI_SYM_LOCAL OCI_Boolean OcilibJsonNodeRenameField
(
    OCI_JsonNode * node,
    const otext  * old_name,
    const otext  * new_name
);

OCI_SYM_LOCAL OCI_Boolean OcilibJsonNodeEquals
(
    OCI_JsonNode * node1,
    OCI_JsonNode * node2
);

OCI_SYM_LOCAL OCI_Json * OcilibJsonNodeGetJson
(
    OCI_JsonNode * node
);

OCI_SYM_LOCAL OCI_Boolean OcilibJsonNodeAssign
(
    OCI_JsonNode * node,
    OCI_JsonNode * src
);

OCI_SYM_LOCAL OCI_Boolean OcilibJsonNodeHasField
(
    OCI_JsonNode *node,
    const otext  *name
);

OCI_SYM_LOCAL OCI_Boolean OcilibJsonNodeRemoveField
(
    OCI_JsonNode *node,
    const otext  *name
);

OCI_SYM_LOCAL OCI_Boolean OcilibJsonNodeClear
(
    OCI_JsonNode *node
);

OCI_SYM_LOCAL OCI_JsonNode * OcilibJsonNodeGetByPath
(
    OCI_JsonNode *node,
    const otext  *path
);

OCI_SYM_LOCAL OCI_JsonNode * OcilibJsonNodeGetParent
(
    OCI_JsonNode *node
);

OCI_SYM_LOCAL OCI_Boolean OcilibJsonNodeGetValue
(
    OCI_JsonNode   *node,
    OCI_NumericType type,
    void           *value
);

OCI_SYM_LOCAL OCI_Boolean OcilibJsonNodeGetInt16
(
    OCI_JsonNode *node,
    OCI_Int16    *value
);

OCI_SYM_LOCAL OCI_Boolean OcilibJsonNodeGetUInt16
(
    OCI_JsonNode *node,
    OCI_UInt16   *value
);

OCI_SYM_LOCAL OCI_Boolean OcilibJsonNodeGetInt32
(
    OCI_JsonNode *node,
    OCI_Int32    *value
);

OCI_SYM_LOCAL OCI_Boolean OcilibJsonNodeGetUInt32
(
    OCI_JsonNode *node,
    OCI_UInt32   *value
);

OCI_SYM_LOCAL OCI_Boolean OcilibJsonNodeGetInt64
(
    OCI_JsonNode *node,
    OCI_Int64    *value
);

OCI_SYM_LOCAL OCI_Boolean OcilibJsonNodeGetUInt64
(
    OCI_JsonNode *node,
    OCI_UInt64   *value
);

OCI_SYM_LOCAL OCI_Boolean OcilibJsonNodeGetFloat
(
    OCI_JsonNode *node,
    OCI_Float    *value
);

OCI_SYM_LOCAL OCI_Boolean OcilibJsonNodeGetDouble
(
    OCI_JsonNode *node,
    OCI_Double   *value
);

OCI_SYM_LOCAL OCI_Boolean OcilibJsonNodeSetValue
(
    OCI_JsonNode   *node,
    OCI_NumericType type,
    void           *value
);

OCI_SYM_LOCAL OCI_Boolean OcilibJsonNodeSetInt16
(
    OCI_JsonNode *node,
    OCI_Int16     value
);

OCI_SYM_LOCAL OCI_Boolean OcilibJsonNodeSetUInt16
(
    OCI_JsonNode *node,
    OCI_UInt16    value
);

OCI_SYM_LOCAL OCI_Boolean OcilibJsonNodeSetInt32
(
    OCI_JsonNode *node,
    OCI_Int32     value
);

OCI_SYM_LOCAL OCI_Boolean OcilibJsonNodeSetUInt32
(
    OCI_JsonNode *node,
    OCI_UInt32    value
);

OCI_SYM_LOCAL OCI_Boolean OcilibJsonNodeSetInt64
(
    OCI_JsonNode *node,
    OCI_Int64     value
);

OCI_SYM_LOCAL OCI_Boolean OcilibJsonNodeSetUInt64
(
    OCI_JsonNode *node,
    OCI_UInt64    value
);

OCI_SYM_LOCAL OCI_Boolean OcilibJsonNodeSetFloat
(
    OCI_JsonNode *node,
    OCI_Float     value
);

OCI_SYM_LOCAL OCI_Boolean OcilibJsonNodeSetDouble
(
    OCI_JsonNode *node,
    OCI_Double    value
);

OCI_SYM_LOCAL OCI_Boolean OcilibJsonNodeSetItem
(
    OCI_JsonNode *arr,
    OCI_UInt32    index,
    OCI_JsonNode *value
);

#endif /* OCILIB_JSONNODE_H_INCLUDED */
