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

#ifndef OCILIB_SHARDINGKEY_H_INCLUDED
#define OCILIB_SHARDINGKEY_H_INCLUDED

#include "types.h"

OCI_SYM_LOCAL OCI_ShardingKey * OcilibShardingKeyCreate
(
    OCI_Environment *env
);

OCI_SYM_LOCAL OCI_Boolean OcilibShardingKeyFree
(
    OCI_ShardingKey *skey
);

OCI_SYM_LOCAL OCI_Boolean OcilibShardingKeyReset
(
    OCI_ShardingKey *skey
);

OCI_SYM_LOCAL OCI_Boolean OcilibShardingKeyAddString
(
    OCI_ShardingKey *skey,
    const otext     *value
);

OCI_SYM_LOCAL OCI_Boolean OcilibShardingKeyAddNumber
(
    OCI_ShardingKey *skey,
    OCI_Number      *value
);

OCI_SYM_LOCAL OCI_Boolean OcilibShardingKeyAddDate
(
    OCI_ShardingKey *skey,
    OCI_Date        *value
);

OCI_SYM_LOCAL OCI_Boolean OcilibShardingKeyAddTimestamp
(
    OCI_ShardingKey *skey,
    OCI_Timestamp   *value
);

OCI_SYM_LOCAL OCI_Boolean OcilibShardingKeyAddRaw
(
    OCI_ShardingKey *skey,
    const void      *value,
    OCI_UInt32       size
);

OCI_SYM_LOCAL const otext * OcilibShardingKeyToText
(
    OCI_ShardingKey *skey
);

#endif /* OCILIB_SHARDINGKEY_H_INCLUDED */
