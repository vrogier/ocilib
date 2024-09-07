/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2024 Vincent ROGIER <vince.rogier@ocilib.net>
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
    OCI_Pool    *pool,
    const otext *db,
    const otext *user,
    const otext *pwd,
    unsigned int mode,
    const otext *tag
);

OCI_SYM_LOCAL OCI_Connection* OcilibConnectionAllocate
(
    OCI_Pool   * pool,
    const otext* db,
    const otext* user,
    const otext* pwd,
    unsigned int mode
);

OCI_SYM_LOCAL boolean OcilibConnectionAttach
(
    OCI_Connection* con
);

OCI_SYM_LOCAL boolean OcilibConnectionLogon
(
    OCI_Connection* con,
    const otext   * new_pwd,
    const otext   * tag
);

OCI_SYM_LOCAL boolean OcilibConnectionDispose
(
    OCI_Connection* con
);

OCI_SYM_LOCAL unsigned int OcilibConnectionGetMinSupportedVersion
(
    OCI_Connection *con
);

OCI_SYM_LOCAL boolean OcilibConnectionIsVersionSupported
(
    OCI_Connection *con,
    unsigned int    version
);

OCI_SYM_LOCAL OCI_Connection* OcilibConnectionCreate
(
    const otext* db,
    const otext* user,
    const otext* pwd,
    unsigned int mode
);

OCI_SYM_LOCAL boolean OcilibConnectionFree
(
    OCI_Connection* con
);

OCI_SYM_LOCAL boolean OcilibConnectionCommit
(
    OCI_Connection* con
);

OCI_SYM_LOCAL boolean OcilibConnectionRollback
(
    OCI_Connection* con
);

OCI_SYM_LOCAL boolean OcilibConnectionSetAutoCommit
(
    OCI_Connection* con,
    boolean         enable
);

OCI_SYM_LOCAL boolean OcilibConnectionGetAutoCommit
(
    OCI_Connection* con
);

OCI_SYM_LOCAL boolean OcilibConnectionIsConnected
(
    OCI_Connection* con
);

OCI_SYM_LOCAL void* OcilibConnectionGetUserData
(
    OCI_Connection* con
);

OCI_SYM_LOCAL boolean OcilibConnectionSetUserData
(
    OCI_Connection* con,
    void          * data
);

OCI_SYM_LOCAL boolean OcilibConnectionSetSessionTag
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

OCI_SYM_LOCAL boolean OcilibConnectionSetPassword
(
    OCI_Connection* con,
    const otext   * password
);

OCI_SYM_LOCAL unsigned int OcilibConnectionGetSessionMode
(
    OCI_Connection* con
);

OCI_SYM_LOCAL const otext* OcilibConnectionGetServerVersion
(
    OCI_Connection* con
);

OCI_SYM_LOCAL unsigned int OcilibConnectionGetServerMajorVersion
(
    OCI_Connection* con
);

OCI_SYM_LOCAL unsigned int OcilibConnectionGetServerMinorVersion
(
    OCI_Connection* con
);

OCI_SYM_LOCAL unsigned int OcilibConnectionGetServerRevisionVersion
(
    OCI_Connection* con
);

OCI_SYM_LOCAL OCI_Transaction* OcilibConnectionGetTransaction
(
    OCI_Connection* con
);

OCI_SYM_LOCAL boolean OcilibConnectionSetTransaction
(
    OCI_Connection * con,
    OCI_Transaction* trans
);

OCI_SYM_LOCAL unsigned int OcilibConnectionGetVersion
(
    OCI_Connection* con
);

OCI_SYM_LOCAL boolean OcilibConnectionBreak
(
    OCI_Connection* con
);

OCI_SYM_LOCAL boolean OcilibConnectionEnableServerOutput
(
    OCI_Connection* con,
    unsigned int    bufsize,
    unsigned int    arrsize,
    unsigned int    lnsize
);

OCI_SYM_LOCAL boolean OcilibConnectionDisableServerOutput
(
    OCI_Connection* con
);

OCI_SYM_LOCAL const otext* OcilibConnectionGetServerOutput
(
    OCI_Connection* con
);

OCI_SYM_LOCAL boolean OcilibConnectionSetTrace
(
    OCI_Connection* con,
    unsigned int    trace,
    const otext   * value
);

OCI_SYM_LOCAL const otext* OcilibConnectionGetTrace
(
    OCI_Connection* con,
    unsigned int    trace
);

OCI_SYM_LOCAL boolean OcilibConnectionPing
(
    OCI_Connection* con
);

OCI_SYM_LOCAL boolean OcilibConnectionSetTimeout
(
    OCI_Connection* con,
    unsigned int    type,
    unsigned int    value
);

OCI_SYM_LOCAL unsigned int OcilibConnectionGetTimeout
(
    OCI_Connection* con,
    unsigned int    type
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

OCI_SYM_LOCAL boolean OcilibConnectionIsTAFCapable
(
    OCI_Connection* con
);

OCI_SYM_LOCAL boolean OcilibConnectionSetTAFHandler
(
    OCI_Connection * con,
    POCI_TAF_HANDLER handler
);

OCI_SYM_LOCAL unsigned int OcilibConnectionGetStatementCacheSize
(
    OCI_Connection* con
);

OCI_SYM_LOCAL boolean OcilibConnectionSetStatementCacheSize
(
    OCI_Connection* con,
    unsigned int    value
);

OCI_SYM_LOCAL unsigned int OcilibConnectionGetDefaultLobPrefetchSize
(
    OCI_Connection* con
);

OCI_SYM_LOCAL boolean OcilibConnectionSetDefaultLobPrefetchSize
(
    OCI_Connection* con,
    unsigned int    value
);

OCI_SYM_LOCAL unsigned int OcilibConnectionGetMaxCursors
(
    OCI_Connection* con
);

OCI_SYM_LOCAL boolean OcilibConnectionExecuteImmediate
(
    OCI_Connection* con,
    const otext   * sql,
    va_list         args
);

OCI_SYM_LOCAL boolean OcilibConnectionExecuteImmediateFmt
(
    OCI_Connection* con,
    const otext   * sql,
    va_list         args
);

#endif /* OCILIB_CONNECTION_H_INCLUDED */