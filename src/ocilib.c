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

#include "ocilib_internal.h"

 /* --------------------------------------------------------------------------------------------- *
  * agent
  * --------------------------------------------------------------------------------------------- */

OCI_Agent* OCI_API OCI_AgentCreate
(
    OCI_Connection* con,
    const otext* name,
    const otext* address
)
{
    return AgentCreate(con, name, address);
}

boolean OCI_API OCI_AgentFree
(
    OCI_Agent* agent
)
{
    return AgentFree(agent);
}

const otext* OCI_API OCI_AgentGetName
(
    OCI_Agent* agent
)
{
    return AgentGetName(agent);
}

boolean OCI_API OCI_AgentSetName
(
    OCI_Agent* agent,
    const otext* name
)
{
    return AgentSetName(agent, name);
}

const otext* OCI_API OCI_AgentGetAddress
(
    OCI_Agent* agent
)
{
    return AgentGetAddress(agent);
}

boolean OCI_API OCI_AgentSetAddress
(
    OCI_Agent* agent,
    const otext* address
)
{
    return AgentSetAddress(agent, address);
}

/* --------------------------------------------------------------------------------------------- *
 * array
 * --------------------------------------------------------------------------------------------- */

 /* --------------------------------------------------------------------------------------------- *
  * bind
  * --------------------------------------------------------------------------------------------- */

const otext* OCI_API OCI_BindGetName
(
    OCI_Bind* bnd
)
{
    return BindGetName(bnd);
}

unsigned int OCI_API OCI_BindGetType
(
    OCI_Bind* bnd
)
{
    return BindGetType(bnd);
}

unsigned int OCI_API OCI_BindGetSubtype
(
    OCI_Bind* bnd
)
{
    return BindGetSubtype(bnd);
}

unsigned int OCI_API OCI_BindGetDataCount
(
    OCI_Bind* bnd
)
{
    return BindGetDataCount(bnd);
}

void* OCI_API OCI_BindGetData
(
    OCI_Bind* bnd
)
{
    return BindGetData(bnd);
}

OCI_Statement* OCI_API OCI_BindGetStatement
(
    OCI_Bind* bnd
)
{
    return BindGetStatement(bnd);
}

boolean OCI_API OCI_BindSetDataSize
(
    OCI_Bind* bnd,
    unsigned int size
)
{
    return BindSetDataSize(bnd, size);
}

boolean OCI_API OCI_BindSetDataSizeAtPos
(
    OCI_Bind* bnd,
    unsigned int position,
    unsigned int size
)
{
    return BindSetDataSizeAtPos(bnd, position, size);
}

unsigned int OCI_API OCI_BindGetDataSize
(
    OCI_Bind* bnd
)
{
    return BindGetDataSize(bnd);
}

unsigned int OCI_API OCI_BindGetDataSizeAtPos
(
    OCI_Bind* bnd,
    unsigned int position
)
{
    return BindGetDataSizeAtPos(bnd, position);
}

boolean OCI_API OCI_BindSetNullAtPos
(
    OCI_Bind* bnd,
    unsigned int position
)
{
    return BindSetNullAtPos(bnd, position);
}

boolean OCI_API OCI_BindSetNull
(
    OCI_Bind* bnd
)
{
    return BindSetNull(bnd);
}

boolean OCI_API OCI_BindSetNotNullAtPos
(
    OCI_Bind* bnd,
    unsigned int position
)
{
    return BindSetNotNullAtPos(bnd, position);
}

boolean OCI_API OCI_BindSetNotNull
(
    OCI_Bind* bnd
)
{
    return BindSetNotNull(bnd);
}

boolean OCI_API OCI_BindIsNullAtPos
(
    OCI_Bind* bnd,
    unsigned int position
)
{
    return BindIsNullAtPos(bnd, position);
}

boolean OCI_API OCI_BindIsNull
(
    OCI_Bind* bnd
)
{
    return BindIsNull(bnd);
}

boolean OCI_API OCI_BindSetCharsetForm
(
    OCI_Bind* bnd,
    unsigned int csfrm
)
{
    return BindSetCharsetForm(bnd, csfrm);
}

boolean OCI_API OCI_BindSetDirection
(
    OCI_Bind* bnd,
    unsigned int direction
)
{
    return BindSetDirection(bnd, direction);
}

unsigned int OCI_API OCI_BindGetDirection
(
    OCI_Bind* bnd
)
{
    return BindGetDirection(bnd);
}

unsigned int OCI_API OCI_BindGetAllocationMode
(
    OCI_Bind* bnd
)
{
    return BindGetAllocationMode(bnd);
}

/* --------------------------------------------------------------------------------------------- *
 * collection
 * --------------------------------------------------------------------------------------------- */

OCI_Coll * OCI_API OCI_CollCreate
(
    OCI_TypeInfo *typinf
)
{
    return CollCreate(typinf);
}

boolean OCI_API OCI_CollFree
(
    OCI_Coll *coll
)
{
    return CollFree(coll);
}

OCI_Coll ** OCI_API OCI_CollArrayCreate
(
    OCI_Connection *con,
    OCI_TypeInfo   *typinf,
    unsigned int    nbelem
)
{
    return CollArrayCreate(con, typinf, nbelem);
}

boolean OCI_API OCI_CollArrayFree
(
    OCI_Coll **colls
)
{
    return CollArrayFree(colls);
}

boolean OCI_API OCI_CollAssign
(
    OCI_Coll *coll,
    OCI_Coll *coll_src
)
{
    return CollAssign(coll, coll_src);
}

unsigned int OCI_API OCI_CollGetType
(
    OCI_Coll *coll
)
{
    return CollGetType(coll);
}

unsigned int OCI_API OCI_CollGetMax
(
    OCI_Coll *coll
)
{
    return CollGetMax(coll);
}

unsigned int OCI_API OCI_CollGetSize
(
    OCI_Coll *coll
)
{
    return CollGetSize(coll);
}

boolean OCI_API OCI_CollTrim
(
    OCI_Coll     *coll,
    unsigned int  nb_elem
)
{
    return CollTrim(coll, nb_elem);
}

OCI_Elem * OCI_API OCI_CollGetElem
(
    OCI_Coll    *coll,
    unsigned int index
)
{
    return CollGetElem(coll, index);
}

boolean OCI_API OCI_CollGetElem2
(
    OCI_Coll    *coll,
    unsigned int index,
    OCI_Elem    *elem
)
{
    return CollGetElem2(coll, index, elem);
}

boolean OCI_API OCI_CollSetElem
(
    OCI_Coll     *coll,
    unsigned int  index,
    OCI_Elem     *elem
)
{
    return CollSetElem(coll, index, elem);
}

boolean OCI_API OCI_CollAppend
(
    OCI_Coll *coll,
    OCI_Elem *elem
)
{
    return CollAppend(coll, elem);
}

OCI_TypeInfo * OCI_API OCI_CollGetTypeInfo
(
    OCI_Coll *coll
)
{
    return CollGetTypeInfo(coll);
}

boolean OCI_API OCI_CollClear
(
    OCI_Coll *coll
)
{
    return CollClear(coll);
}

boolean OCI_API OCI_CollDeleteElem
(
    OCI_Coll    *coll,
    unsigned int index
)
{
    return CollDeleteElem(coll, index);
}

unsigned int OCI_API OCI_CollGetCount
(
    OCI_Coll *coll
)
{
    return CollGetCount(coll);
}

boolean OCI_API OCI_CollToText
(
    OCI_Coll     *coll,
    unsigned int *size,
    otext        *str
)
{
    return CollToText(coll, size, str);
}

/* --------------------------------------------------------------------------------------------- *
 * column
 * --------------------------------------------------------------------------------------------- */

const otext* OCI_API OCI_ColumnGetName
(
    OCI_Column* col
)
{
    return ColumnGetName(col);
}

unsigned int OCI_API OCI_ColumnGetType
(
    OCI_Column* col
)
{
    return ColumnGetType(col);
}

unsigned int OCI_API OCI_ColumnGetCharsetForm
(
    OCI_Column* col
)
{
    return ColumnGetCharsetForm(col);
}

unsigned int OCI_API OCI_ColumnGetSize
(
    OCI_Column* col
)
{
    return ColumnGetSize(col);
}

int OCI_API OCI_ColumnGetScale
(
    OCI_Column* col
)
{
    return ColumnGetScale(col);
}

int OCI_API OCI_ColumnGetPrecision
(
    OCI_Column* col
)
{
    return ColumnGetPrecision(col);
}

int OCI_API OCI_ColumnGetFractionalPrecision
(
    OCI_Column* col
)
{
    return ColumnGetFractionalPrecision(col);
}

int OCI_API OCI_ColumnGetLeadingPrecision
(
    OCI_Column* col
)
{
    return ColumnGetLeadingPrecision(col);
}

boolean OCI_API OCI_ColumnGetNullable
(
    OCI_Column* col
)
{
    return ColumnGetNullable(col);
}

boolean OCI_API OCI_ColumnGetCharUsed
(
    OCI_Column* col
)
{
    return ColumnGetCharUsed(col);
}

unsigned int OCI_API OCI_ColumnGetPropertyFlags
(
    OCI_Column* col
)
{
    return ColumnGetPropertyFlags(col);
}

unsigned int OCI_API OCI_ColumnGetCollationID
(
    OCI_Column* col
)
{
    return ColumnGetCollationID(col);
}

const otext* OCI_API OCI_ColumnGetSQLType
(
    OCI_Column* col
)
{
    return ColumnGetSQLType(col);
}

unsigned int OCI_API OCI_ColumnGetFullSQLType
(
    OCI_Column* col,
    otext* buffer,
    unsigned int len
)
{
    return ColumnGetFullSQLType(col, buffer, len);
}

OCI_TypeInfo* OCI_API OCI_ColumnGetTypeInfo
(
    OCI_Column* col
)
{
    return ColumnGetTypeInfo(col);
}

unsigned int OCI_API OCI_ColumnGetSubType
(
    OCI_Column* col
)
{
    return ColumnGetSubType(col);
}

/* --------------------------------------------------------------------------------------------- *
 * connection
 * --------------------------------------------------------------------------------------------- */

OCI_Connection * OCI_API OCI_ConnectionCreate
(
    const otext *db,
    const otext *user,
    const otext *pwd,
    unsigned int mode
)
{
    return ConnectionCreate(db, user, pwd, mode);
}

boolean OCI_API OCI_ConnectionFree
(
    OCI_Connection *con
)
{
    return ConnectionFree(con);
}

boolean OCI_API OCI_Commit
(
    OCI_Connection *con
)
{
    return ConnectionCommit(con);
}

boolean OCI_API OCI_Rollback
(
    OCI_Connection *con
)
{
    return ConnectionRollback(con);
}

boolean OCI_API OCI_SetAutoCommit
(
    OCI_Connection *con,
    boolean         enable
)
{
    return ConnectionSetAutoCommit(con, enable);
}

boolean OCI_API OCI_GetAutoCommit
(
    OCI_Connection *con
)
{
    return ConnectionGetAutoCommit(con);
}

boolean OCI_API OCI_IsConnected
(
    OCI_Connection *con
)
{
    return ConnectionIsConnected(con);
}

void * OCI_API OCI_GetUserData
(
    OCI_Connection *con
)
{
    return ConnectionGetUserData(con);
}

boolean OCI_API OCI_SetUserData
(
    OCI_Connection *con,
    void           *data
)
{
    return ConnectionSetUserData(con, data);
}

boolean OCI_API OCI_SetSessionTag
(
    OCI_Connection *con,
    const otext    *tag
)
{
    return ConnectionSetSessionTag(con, tag);
}

const otext * OCI_API OCI_GetSessionTag
(
    OCI_Connection *con
)
{
    return ConnectionGetSessionTag(con);
}

const otext * OCI_API OCI_GetDatabase
(
    OCI_Connection *con
)
{
    return ConnectionGetDatabase(con);
}

const otext * OCI_API OCI_GetUserName
(
    OCI_Connection *con
)
{
    return ConnectionGetUserName(con);
}

const otext * OCI_API OCI_GetPassword
(
    OCI_Connection *con
)
{
    return ConnectionGetPassword(con);
}

boolean OCI_API OCI_SetPassword
(
    OCI_Connection *con,
    const otext    *password
)
{
    return ConnectionSetPassword(con, password);
}

unsigned int OCI_API OCI_GetSessionMode
(
    OCI_Connection *con
)
{
    return ConnectionGetSessionMode(con);
}

const otext * OCI_API OCI_GetVersionServer
(
    OCI_Connection *con
)
{
    return ConnectionGetVersionServer(con);
}

unsigned int OCI_API OCI_GetServerMajorVersion
(
    OCI_Connection *con
)
{
    return ConnectionGetServerMajorVersion(con);
}

unsigned int OCI_API OCI_GetServerMinorVersion
(
    OCI_Connection *con
)
{
    return ConnectionGetServerMinorVersion(con);
}

unsigned int OCI_API OCI_GetServerRevisionVersion
(
    OCI_Connection *con
)
{
    return ConnectionGetServerRevisionVersion(con);
}

OCI_Transaction * OCI_API OCI_GetTransaction
(
    OCI_Connection *con
)
{
    return ConnectionGetTransaction(con);
}

boolean OCI_API OCI_SetTransaction
(
    OCI_Connection  *con,
    OCI_Transaction *trans
)
{
    return ConnectionSetTransaction(con, trans);
}

unsigned int OCI_API OCI_GetVersionConnection
(
    OCI_Connection *con
)
{
    return ConnectionGetVersionConnection(con);
}

boolean OCI_API OCI_Break
(
    OCI_Connection *con
)
{
    return ConnectionBreak(con);
}

boolean OCI_API OCI_ServerEnableOutput
(
    OCI_Connection *con,
    unsigned int    bufsize,
    unsigned int    arrsize,
    unsigned int    lnsize
)
{
    return ConnectionServerEnableOutput(con, bufsize, arrsize, lnsize);
}

boolean OCI_API OCI_ServerDisableOutput
(
    OCI_Connection *con
)
{
    return ConnectionServerDisableOutput(con);
}

const otext * OCI_API OCI_ServerGetOutput
(
    OCI_Connection *con
)
{
    return ConnectionServerGetOutput(con);
}

boolean OCI_API OCI_SetTrace
(
    OCI_Connection *con,
    unsigned int    trace,
    const otext    *value
)
{
    return ConnectionSetTrace(con, trace, value);
}

const otext * OCI_API OCI_GetTrace
(
    OCI_Connection *con,
    unsigned int    trace
)
{
    return ConnectionGetTrace(con, trace);
}

boolean OCI_API OCI_Ping
(
    OCI_Connection *con
)
{
    return ConnectionPing(con);
}

boolean OCI_API OCI_SetTimeout
(
    OCI_Connection *con,
    unsigned int    type,
    unsigned int    value
)
{
    return ConnectionSetTimeout(con, type, value);
}

unsigned int OCI_API OCI_GetTimeout
(
    OCI_Connection *con,
    unsigned int    type
)
{
    return ConnectionGetTimeout(con, type);
}

const otext * OCI_API OCI_GetDBName
(
    OCI_Connection *con
)
{
    return ConnectionGetDBName(con);
}

const otext * OCI_API OCI_GetInstanceName
(
    OCI_Connection *con
)
{
    return ConnectionGetInstanceName(con);
}

const otext * OCI_API OCI_GetServiceName
(
    OCI_Connection *con
)
{
    return ConnectionGetServiceName(con);
}

const otext * OCI_API OCI_GetServerName
(
    OCI_Connection *con
)
{
    return ConnectionGetServerName(con);
}

const otext * OCI_API OCI_GetDomainName
(
    OCI_Connection *con
)
{
    return ConnectionGetDomainName(con);
}

OCI_Timestamp * OCI_API OCI_GetInstanceStartTime
(
    OCI_Connection *con
)
{
    return ConnectionGetInstanceStartTime(con);
}

boolean OCI_API OCI_IsTAFCapable
(
    OCI_Connection *con
)
{
    return ConnectionIsTAFCapable(con);
}

boolean OCI_API OCI_SetTAFHandler
(
    OCI_Connection  *con,
    POCI_TAF_HANDLER handler
)
{
    return ConnectionSetTAFHandler(con, handler);
}

unsigned int OCI_API OCI_GetStatementCacheSize
(
    OCI_Connection  *con
)
{
    return ConnectionGetStatementCacheSize(con);
}

boolean OCI_API OCI_SetStatementCacheSize
(
    OCI_Connection  *con,
    unsigned int     value
)
{
    return ConnectionSetStatementCacheSize(con, value);
}

unsigned int OCI_API OCI_GetDefaultLobPrefetchSize
(
    OCI_Connection *con
)
{
    return ConnectionGetDefaultLobPrefetchSize(con);
}

boolean OCI_API OCI_SetDefaultLobPrefetchSize
(
    OCI_Connection *con,
    unsigned int    value
)
{
    return ConnectionSetDefaultLobPrefetchSize(con, value);
}

unsigned int OCI_API OCI_GetMaxCursors
(
    OCI_Connection *con
)
{
    return ConnectionGetMaxCursors(con);
}

boolean OCI_Immediate
(
    OCI_Connection *con,
    const otext    *sql,
    ...
)
{
    boolean res = FALSE;
    va_list args;

    va_start(args, sql);
    res = ConnectionExecuteImmediate(con, sql, args);
    va_end(args);

    return res;
}

boolean OCI_ImmediateFmt
(
    OCI_Connection *con,
    const otext    *sql,
    ...
)
{
    boolean res = FALSE;
    va_list args;

    va_start(args, sql);
    res =  ConnectionExecuteImmediateFmt(con, sql, args);
    va_end(args);

    return res;
}














boolean OCI_API OCI_SetUserPassword
(
    const otext* db,
    const otext* user,
    const otext* pwd,
    const otext* new_pwd
)
{
    return SetUserPassword(db, user, pwd, new_pwd);
}