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

#ifndef OCILIB_CONNECTION_H_INCLUDED
#define OCILIB_CONNECTION_H_INCLUDED

#include "types.h"

OCI_Connection * ConnectionCreateInternal
(
    OCI_Pool    *pool,
    const otext *db,
    const otext *user,
    const otext *pwd,
    unsigned int mode,
    const otext *tag
);

OCI_Connection* ConnectionAllocate
(
    OCI_Pool   * pool,
    const otext* db,
    const otext* user,
    const otext* pwd,
    unsigned int mode
);

boolean ConnectionAttach
(
    OCI_Connection* con
);

boolean ConnectionLogon
(
    OCI_Connection* con,
    const otext   * new_pwd,
    const otext   * tag
);

boolean ConnectionDispose
(
    OCI_Connection* con
);

unsigned int ConnectionGetMinSupportedVersion
(
    OCI_Connection *con
);

boolean ConnectionIsVersionSupported
(
    OCI_Connection *con,
    unsigned int    version
);

OCI_Connection* ConnectionCreate
(
    const otext* db,
    const otext* user,
    const otext* pwd,
    unsigned int mode
);

boolean ConnectionFree
(
    OCI_Connection* con
);

boolean ConnectionCommit
(
    OCI_Connection* con
);

boolean ConnectionRollback
(
    OCI_Connection* con
);

boolean ConnectionSetAutoCommit
(
    OCI_Connection* con,
    boolean         enable
);

boolean ConnectionGetAutoCommit
(
    OCI_Connection* con
);

boolean ConnectionIsConnected
(
    OCI_Connection* con
);

void* ConnectionGetUserData
(
    OCI_Connection* con
);

boolean ConnectionSetUserData
(
    OCI_Connection* con,
    void          * data
);

boolean ConnectionSetSessionTag
(
    OCI_Connection* con,
    const otext   * tag
);

const otext* ConnectionGetSessionTag
(
    OCI_Connection* con
);

const otext* ConnectionGetConnectionString
(
    OCI_Connection* con
);

const otext* ConnectionGetUserName
(
    OCI_Connection* con
);

const otext* ConnectionGetPassword
(
    OCI_Connection* con
);

boolean ConnectionSetPassword
(
    OCI_Connection* con,
    const otext   * password
);

unsigned int ConnectionGetSessionMode
(
    OCI_Connection* con
);

const otext* ConnectionGetServerVersion
(
    OCI_Connection* con
);

unsigned int ConnectionGetServerMajorVersion
(
    OCI_Connection* con
);

unsigned int ConnectionGetServerMinorVersion
(
    OCI_Connection* con
);

unsigned int ConnectionGetServerRevisionVersion
(
    OCI_Connection* con
);

OCI_Transaction* ConnectionGetTransaction
(
    OCI_Connection* con
);

boolean ConnectionSetTransaction
(
    OCI_Connection * con,
    OCI_Transaction* trans
);

unsigned int ConnectionGetVersion
(
    OCI_Connection* con
);

boolean ConnectionBreak
(
    OCI_Connection* con
);

boolean ConnectionEnableServerOutput
(
    OCI_Connection* con,
    unsigned int    bufsize,
    unsigned int    arrsize,
    unsigned int    lnsize
);

boolean ConnectionDisableServerOutput
(
    OCI_Connection* con
);

const otext* ConnectionGetServerOutput
(
    OCI_Connection* con
);

boolean ConnectionSetTrace
(
    OCI_Connection* con,
    unsigned int    trace,
    const otext   * value
);

const otext* ConnectionGetTrace
(
    OCI_Connection* con,
    unsigned int    trace
);

boolean ConnectionPing
(
    OCI_Connection* con
);

boolean ConnectionSetTimeout
(
    OCI_Connection* con,
    unsigned int    type,
    unsigned int    value
);

unsigned int ConnectionGetTimeout
(
    OCI_Connection* con,
    unsigned int    type
);

const otext* ConnectionGetDatabaseName
(
    OCI_Connection* con
);

const otext* ConnectionGetInstanceName
(
    OCI_Connection* con
);

const otext* ConnectionGetServiceName
(
    OCI_Connection* con
);

const otext* ConnectionGetServerName
(
    OCI_Connection* con
);

const otext* ConnectionGetDomainName
(
    OCI_Connection* con
);

OCI_Timestamp* ConnectionGetInstanceStartTime
(
    OCI_Connection* con
);

boolean ConnectionIsTAFCapable
(
    OCI_Connection* con
);

boolean ConnectionSetTAFHandler
(
    OCI_Connection * con,
    POCI_TAF_HANDLER handler
);

unsigned int ConnectionGetStatementCacheSize
(
    OCI_Connection* con
);

boolean ConnectionSetStatementCacheSize
(
    OCI_Connection* con,
    unsigned int    value
);

unsigned int ConnectionGetDefaultLobPrefetchSize
(
    OCI_Connection* con
);

boolean ConnectionSetDefaultLobPrefetchSize
(
    OCI_Connection* con,
    unsigned int    value
);

unsigned int ConnectionGetMaxCursors
(
    OCI_Connection* con
);

boolean ConnectionExecuteImmediate
(
    OCI_Connection* con,
    const otext   * sql,
    va_list         args
);

boolean ConnectionExecuteImmediateFmt
(
    OCI_Connection* con,
    const otext   * sql,
    va_list         args
);

#endif /* OCILIB_CONNECTION_H_INCLUDED */