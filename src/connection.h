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

#ifndef OCILIB_CONNECTION_H_INCLUDED
#define OCILIB_CONNECTION_H_INCLUDED

#include "types.h"

OCI_SYM_LOCAL OCI_Connection * OcilibConnectionCreateInternal
(
    OCI_Environment *env,
    OCI_Pool        *pool,
    const otext     *db,
    const otext     *user,
    const otext     *pwd,
    OCI_SessionMode  mode,
    const otext     *tag,
    OCI_ShardingKey *skey,
    OCI_ShardingKey *sskey
);

OCI_SYM_LOCAL OCI_Connection* OcilibConnectionAllocate
(
    OCI_Environment* env,
    OCI_Pool       * pool,
    const otext    * db,
    const otext    * user,
    const otext    * pwd,
    OCI_ShardingKey* skey,
    OCI_ShardingKey* sskey,
    OCI_SessionMode  mode
);

OCI_SYM_LOCAL OCI_Boolean OcilibConnectionAttach
(
    OCI_Connection* con
);

OCI_SYM_LOCAL OCI_Boolean OcilibConnectionLogon
(
    OCI_Connection * con,
    const otext    * new_pwd,
    const otext    * tag,
    OCI_ShardingKey* skey,
    OCI_ShardingKey* sskey
);

OCI_SYM_LOCAL OCI_Boolean OcilibConnectionDispose
(
    OCI_Connection* con
);

OCI_SYM_LOCAL OCI_UInt32 OcilibConnectionGetMinSupportedVersion
(
    OCI_Connection *con
);

OCI_SYM_LOCAL OCI_Boolean OcilibConnectionIsVersionSupported
(
    OCI_Connection *con,
    OCI_UInt32      version
);

OCI_SYM_LOCAL OCI_Connection* OcilibConnectionCreate
(
    OCI_Environment* env,
    const otext    * db,
    const otext    * user,
    const otext    * pwd,
    OCI_SessionMode  mode
);

OCI_SYM_LOCAL OCI_Connection* OcilibConnectionCreateSharded
(
    OCI_Environment* env,
    const otext    * db,
    const otext    * user,
    const otext    * pwd,
    OCI_SessionMode  mode,
    OCI_ShardingKey* skey,
    OCI_ShardingKey* sskey
);

OCI_SYM_LOCAL OCI_Boolean OcilibConnectionFree
(
    OCI_Connection* con
);

OCI_SYM_LOCAL OCI_Boolean OcilibConnectionCommit
(
    OCI_Connection* con
);

OCI_SYM_LOCAL OCI_Boolean OcilibConnectionRollback
(
    OCI_Connection* con
);

OCI_SYM_LOCAL OCI_Boolean OcilibConnectionSetAutoCommit
(
    OCI_Connection* con,
    OCI_Boolean     enable
);

OCI_SYM_LOCAL OCI_Boolean OcilibConnectionGetAutoCommit
(
    OCI_Connection* con
);

OCI_SYM_LOCAL OCI_Boolean OcilibConnectionIsAlive
(
    OCI_Connection* con
);

OCI_SYM_LOCAL OCI_Boolean OcilibConnectionSetSessionTag
(
    OCI_Connection* con,
    const otext   * tag
);

OCI_SYM_LOCAL const otext* OcilibConnectionGetSessionTag
(
    OCI_Connection* con
);

OCI_SYM_LOCAL const otext* OcilibConnectionGetConnectionString
(
    OCI_Connection* con
);

OCI_SYM_LOCAL const otext* OcilibConnectionGetUserName
(
    OCI_Connection* con
);

OCI_SYM_LOCAL const otext* OcilibConnectionGetPassword
(
    OCI_Connection* con
);

OCI_SYM_LOCAL OCI_Boolean OcilibConnectionSetPassword
(
    OCI_Connection* con,
    const otext   * password
);

OCI_SYM_LOCAL OCI_SessionMode OcilibConnectionGetSessionMode
(
    OCI_Connection* con
);

OCI_SYM_LOCAL const otext* OcilibConnectionGetServerVersionString
(
    OCI_Connection* con
);

OCI_SYM_LOCAL OCI_UInt32 OcilibConnectionGetServerMajorVersion
(
    OCI_Connection* con
);

OCI_SYM_LOCAL OCI_UInt32 OcilibConnectionGetServerMinorVersion
(
    OCI_Connection* con
);

OCI_SYM_LOCAL OCI_UInt32 OcilibConnectionGetServerRevisionVersion
(
    OCI_Connection* con
);

OCI_SYM_LOCAL OCI_Transaction* OcilibConnectionGetTransaction
(
    OCI_Connection* con
);

OCI_SYM_LOCAL OCI_Boolean OcilibConnectionSetTransaction
(
    OCI_Connection * con,
    OCI_Transaction* trans
);

OCI_SYM_LOCAL OCI_UInt32 OcilibConnectionGetVersion
(
    OCI_Connection* con
);

OCI_SYM_LOCAL OCI_Boolean OcilibConnectionBreak
(
    OCI_Connection* con
);

OCI_SYM_LOCAL OCI_Boolean OcilibConnectionEnableServerOutput
(
    OCI_Connection* con,
    OCI_UInt32      bufsize,
    OCI_UInt32      arrsize,
    OCI_UInt32      lnsize
);

OCI_SYM_LOCAL OCI_Boolean OcilibConnectionDisableServerOutput
(
    OCI_Connection* con
);

OCI_SYM_LOCAL const otext* OcilibConnectionGetServerOutput
(
    OCI_Connection* con
);

OCI_SYM_LOCAL OCI_Boolean OcilibConnectionSetTrace
(
    OCI_Connection* con,
    OCI_TraceType   trace,
    const otext   * value
);

OCI_SYM_LOCAL const otext* OcilibConnectionGetTrace
(
    OCI_Connection* con,
    OCI_TraceType   trace
);

OCI_SYM_LOCAL OCI_Boolean OcilibConnectionPing
(
    OCI_Connection* con
);

OCI_SYM_LOCAL OCI_Boolean OcilibConnectionSetTimeout
(
    OCI_Connection* con,
    OCI_TimeoutType type,
    OCI_UInt32      value
);

OCI_SYM_LOCAL OCI_UInt32 OcilibConnectionGetTimeout
(
    OCI_Connection* con,
    OCI_TimeoutType type
);

OCI_SYM_LOCAL const otext* OcilibConnectionGetDatabaseName
(
    OCI_Connection* con
);

OCI_SYM_LOCAL const otext* OcilibConnectionGetInstanceName
(
    OCI_Connection* con
);

OCI_SYM_LOCAL const otext* OcilibConnectionGetServiceName
(
    OCI_Connection* con
);

OCI_SYM_LOCAL const otext* OcilibConnectionGetServerName
(
    OCI_Connection* con
);

OCI_SYM_LOCAL const otext* OcilibConnectionGetDomainName
(
    OCI_Connection* con
);

OCI_SYM_LOCAL OCI_Timestamp* OcilibConnectionGetInstanceStartTime
(
    OCI_Connection* con
);

OCI_SYM_LOCAL OCI_Boolean OcilibConnectionIsTAFCapable
(
    OCI_Connection* con
);

OCI_SYM_LOCAL OCI_Boolean OcilibConnectionSetTAFHandler
(
    OCI_Connection * con,
    POCI_TAF_HANDLER handler
);

OCI_SYM_LOCAL OCI_UInt32 OcilibConnectionGetStatementCacheSize
(
    OCI_Connection* con
);

OCI_SYM_LOCAL OCI_Boolean OcilibConnectionSetStatementCacheSize
(
    OCI_Connection* con,
    OCI_UInt32      value
);

OCI_SYM_LOCAL OCI_UInt32 OcilibConnectionGetDefaultLobPrefetchSize
(
    OCI_Connection* con
);

OCI_SYM_LOCAL OCI_Boolean OcilibConnectionSetDefaultLobPrefetchSize
(
    OCI_Connection* con,
    OCI_UInt32      value
);

OCI_SYM_LOCAL OCI_UInt32 OcilibConnectionGetMaxCursors
(
    OCI_Connection* con
);

OCI_SYM_LOCAL OCI_Boolean OcilibConnectionImmediate
(
    OCI_Connection* con,
    const otext   * sql,
    va_list         args
);

OCI_SYM_LOCAL OCI_Boolean OcilibConnectionImmediateFormat
(
    OCI_Connection* con,
    const otext   * sql,
    va_list         args
);

#endif /* OCILIB_CONNECTION_H_INCLUDED */