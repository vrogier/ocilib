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

#ifndef OCILIB_JSON_H_INCLUDED
#define OCILIB_JSON_H_INCLUDED

#include "types.h"

OCI_SYM_LOCAL OCI_Json * OcilibJsonInitialize
(
    OCI_Connection *con,
    OCI_Json       *json,
    OCIJson        *handle
);

OCI_SYM_LOCAL OCI_Json * OcilibJsonCreate
(
    OCI_Connection *con
);

OCI_SYM_LOCAL OCI_Json ** OcilibJsonArrayCreate
(
    OCI_Connection *con,
    OCI_UInt32    nbelem
);

OCI_SYM_LOCAL OCI_Boolean OcilibJsonFree
(
    OCI_Json *json
);

OCI_SYM_LOCAL OCI_Boolean OcilibJsonArrayFree
(
    OCI_Json **jsons
);

OCI_SYM_LOCAL OCI_Connection * OcilibJsonGetConnection
(
    OCI_Json *json
);

OCI_SYM_LOCAL OCI_Boolean OcilibJsonLoadText
(
    OCI_Json          *json,
    const otext       *text,
    OCI_UInt32       length,
    OCI_JsonValidation validation,
    OCI_JsonEncoding   encoding
);

OCI_SYM_LOCAL OCI_Boolean OcilibJsonLoadBinary
(
    OCI_Json    *json,
    const void  *data,
    OCI_UInt32 length
);

OCI_SYM_LOCAL OCI_Boolean OcilibJsonToText
(
    OCI_Json     *json,
    otext        *buffer,
    OCI_UInt32 *length,
    OCI_JsonPrint print,
    OCI_Boolean   useEnvCharset
);

OCI_SYM_LOCAL OCI_Boolean OcilibJsonToBinary
(
    OCI_Json     *json,
    void         *buffer,
    OCI_UInt32 *length
);

OCI_SYM_LOCAL OCI_Boolean OcilibJsonClone
(
    OCI_Json *src,
    OCI_Json *dest
);

OCI_SYM_LOCAL OCI_Boolean OcilibJsonAssign
(
    OCI_Json *json,
    OCI_Json *json_src
);

OCI_SYM_LOCAL OCI_Boolean OcilibJsonSetMutable
(
    OCI_Json   *json,
    OCI_Boolean value
);

OCI_SYM_LOCAL OCI_Boolean OcilibJsonIsMutable
(
    OCI_Json *json
);

OCI_SYM_LOCAL OCI_JsonNode * OcilibJsonGetRoot
(
    OCI_Json *json
);

OCI_SYM_LOCAL OCI_Boolean OcilibJsonSetRoot
(
    OCI_Json     *json,
    OCI_JsonNode *node
);

#endif /* OCILIB_JSON_H_INCLUDED */
