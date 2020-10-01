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

OCI_Connection * OcilibConnectionCreateInternal
(
    OCI_Pool    *pool,
    const otext *db,
    const otext *user,
    const otext *pwd,
    unsigned int mode,
    const otext *tag
);

OCI_Connection* OcilibConnectionAllocate
(
    OCI_Pool   * pool,
    const otext* db,
    const otext* user,
    const otext* pwd,
    unsigned int mode
);

boolean OcilibConnectionAttach
(
    OCI_Connection* con
);

boolean OcilibConnectionLogon
(
    OCI_Connection* con,
    const otext   * new_pwd,
    const otext   * tag
);

boolean OcilibConnectionDispose
(
    OCI_Connection* con
);

unsigned int OcilibConnectionGetMinSupportedVersion
(
    OCI_Connection *con
);

boolean OcilibConnectionIsVersionSupported
(
    OCI_Connection *con,
    unsigned int    version
);

OCI_Connection* OcilibConnectionCreate
(
    const otext* db,
    const otext* user,
    const otext* pwd,
    unsigned int mode
);

boolean OcilibConnectionFree
(
    OCI_Connection* con
);

boolean OcilibConnectionCommit
(
    OCI_Connection* con
);

boolean OcilibConnectionRollback
(
    OCI_Connection* con
);

boolean OcilibConnectionSetAutoCommit
(
    OCI_Connection* con,
    boolean         enable
);

boolean OcilibConnectionGetAutoCommit
(
    OCI_Connection* con
);

boolean OcilibConnectionIsConnected
(
    OCI_Connection* con
);

void* OcilibConnectionGetUserData
(
    OCI_Connection* con
);

boolean OcilibConnectionSetUserData
(
    OCI_Connection* con,
    void          * data
);

boolean OcilibConnectionSetSessionTag
(
    OCI_Connection* con,
    const otext   * tag
);

const otext* OcilibConnectionGetSessionTag
(
    OCI_Connection* con
);

const otext* OcilibConnectionGetConnectionString
(
    OCI_Connection* con
);

const otext* OcilibConnectionGetUserName
(
    OCI_Connection* con
);

const otext* OcilibConnectionGetPassword
(
    OCI_Connection* con
);

boolean OcilibConnectionSetPassword
(
    OCI_Connection* con,
    const otext   * password
);

unsigned int OcilibConnectionGetSessionMode
(
    OCI_Connection* con
);

const otext* OcilibConnectionGetServerVersion
(
    OCI_Connection* con
);

unsigned int OcilibConnectionGetServerMajorVersion
(
    OCI_Connection* con
);

unsigned int OcilibConnectionGetServerMinorVersion
(
    OCI_Connection* con
);

unsigned int OcilibConnectionGetServerRevisionVersion
(
    OCI_Connection* con
);

OCI_Transaction* OcilibConnectionGetTransaction
(
    OCI_Connection* con
);

boolean OcilibConnectionSetTransaction
(
    OCI_Connection * con,
    OCI_Transaction* trans
);

unsigned int OcilibConnectionGetVersion
(
    OCI_Connection* con
);

boolean OcilibConnectionBreak
(
    OCI_Connection* con
);

boolean OcilibConnectionEnableServerOutput
(
    OCI_Connection* con,
    unsigned int    bufsize,
    unsigned int    arrsize,
    unsigned int    lnsize
);

boolean OcilibConnectionDisableServerOutput
(
    OCI_Connection* con
);

const otext* OcilibConnectionGetServerOutput
(
    OCI_Connection* con
);

boolean OcilibConnectionSetTrace
(
    OCI_Connection* con,
    unsigned int    trace,
    const otext   * value
);

const otext* OcilibConnectionGetTrace
(
    OCI_Connection* con,
    unsigned int    trace
);

boolean OcilibConnectionPing
(
    OCI_Connection* con
);

boolean OcilibConnectionSetTimeout
(
    OCI_Connection* con,
    unsigned int    type,
    unsigned int    value
);

unsigned int OcilibConnectionGetTimeout
(
    OCI_Connection* con,
    unsigned int    type
);

const otext* OcilibConnectionGetDatabaseName
(
    OCI_Connection* con
);

const otext* OcilibConnectionGetInstanceName
(
    OCI_Connection* con
);

const otext* OcilibConnectionGetServiceName
(
    OCI_Connection* con
);

const otext* OcilibConnectionGetServerName
(
    OCI_Connection* con
);

const otext* OcilibConnectionGetDomainName
(
    OCI_Connection* con
);

OCI_Timestamp* OcilibConnectionGetInstanceStartTime
(
    OCI_Connection* con
);

boolean OcilibConnectionIsTAFCapable
(
    OCI_Connection* con
);

boolean OcilibConnectionSetTAFHandler
(
    OCI_Connection * con,
    POCI_TAF_HANDLER handler
);

unsigned int OcilibConnectionGetStatementCacheSize
(
    OCI_Connection* con
);

boolean OcilibConnectionSetStatementCacheSize
(
    OCI_Connection* con,
    unsigned int    value
);

unsigned int OcilibConnectionGetDefaultLobPrefetchSize
(
    OCI_Connection* con
);

boolean OcilibConnectionSetDefaultLobPrefetchSize
(
    OCI_Connection* con,
    unsigned int    value
);

unsigned int OcilibConnectionGetMaxCursors
(
    OCI_Connection* con
);

boolean OcilibConnectionExecuteImmediate
(
    OCI_Connection* con,
    const otext   * sql,
    va_list         args
);

boolean OcilibConnectionExecuteImmediateFmt
(
    OCI_Connection* con,
    const otext   * sql,
    va_list         args
);

#endif /* OCILIB_CONNECTION_H_INCLUDED */