/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2020 Vincent ROGIER <vince.rogier@ocilib.net>
 *
 * Licensed under the Apache License, Version 2.0 (the "License")
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



#define CALL_IMPL(impl, ...)                                                    \
                                                                                \
    return  impl(__VA_ARGS__);                                                  \



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
    CALL_IMPL(AgentCreate, con, name, address)
}

boolean OCI_API OCI_AgentFree
(
    OCI_Agent* agent
)
{
    CALL_IMPL(AgentFree, agent)
}

const otext* OCI_API OCI_AgentGetName
(
    OCI_Agent* agent
)
{
    CALL_IMPL(AgentGetName, agent)
}

boolean OCI_API OCI_AgentSetName
(
    OCI_Agent* agent,
    const otext* name
)
{
    CALL_IMPL(AgentSetName, agent, name)
}

const otext* OCI_API OCI_AgentGetAddress
(
    OCI_Agent* agent
)
{
    CALL_IMPL(AgentGetAddress, agent)
}

boolean OCI_API OCI_AgentSetAddress
(
    OCI_Agent* agent,
    const otext* address
)
{
    CALL_IMPL(AgentSetAddress, agent, address)
}

 /* --------------------------------------------------------------------------------------------- *
  * bind
  * --------------------------------------------------------------------------------------------- */

const otext* OCI_API OCI_BindGetName
(
    OCI_Bind* bnd
)
{
    CALL_IMPL(BindGetName, bnd)
}

unsigned int OCI_API OCI_BindGetType
(
    OCI_Bind* bnd
)
{
    CALL_IMPL(BindGetType, bnd)
}

unsigned int OCI_API OCI_BindGetSubtype
(
    OCI_Bind* bnd
)
{
    CALL_IMPL(BindGetSubtype, bnd)
}

unsigned int OCI_API OCI_BindGetDataCount
(
    OCI_Bind* bnd
)
{
    CALL_IMPL(BindGetDataCount, bnd)
}

void* OCI_API OCI_BindGetData
(
    OCI_Bind* bnd
)
{
    CALL_IMPL(BindGetData, bnd)
}

OCI_Statement* OCI_API OCI_BindGetStatement
(
    OCI_Bind* bnd
)
{
    CALL_IMPL(BindGetStatement, bnd)
}

boolean OCI_API OCI_BindSetDataSize
(
    OCI_Bind* bnd,
    unsigned int size
)
{
    CALL_IMPL(BindSetDataSize, bnd, size)
}

boolean OCI_API OCI_BindSetDataSizeAtPos
(
    OCI_Bind* bnd,
    unsigned int position,
    unsigned int size
)
{
    CALL_IMPL(BindSetDataSizeAtPos, bnd, position, size)
}

unsigned int OCI_API OCI_BindGetDataSize
(
    OCI_Bind* bnd
)
{
    CALL_IMPL(BindGetDataSize, bnd)
}

unsigned int OCI_API OCI_BindGetDataSizeAtPos
(
    OCI_Bind* bnd,
    unsigned int position
)
{
    CALL_IMPL(BindGetDataSizeAtPos, bnd, position)
}

boolean OCI_API OCI_BindSetNullAtPos
(
    OCI_Bind* bnd,
    unsigned int position
)
{
    CALL_IMPL(BindSetNullAtPos, bnd, position)
}

boolean OCI_API OCI_BindSetNull
(
    OCI_Bind* bnd
)
{
    CALL_IMPL(BindSetNull, bnd)
}

boolean OCI_API OCI_BindSetNotNullAtPos
(
    OCI_Bind* bnd,
    unsigned int position
)
{
    CALL_IMPL(BindSetNotNullAtPos, bnd, position)
}

boolean OCI_API OCI_BindSetNotNull
(
    OCI_Bind* bnd
)
{
    CALL_IMPL(BindSetNotNull, bnd)
}

boolean OCI_API OCI_BindIsNullAtPos
(
    OCI_Bind* bnd,
    unsigned int position
)
{
    CALL_IMPL(BindIsNullAtPos, bnd, position)
}

boolean OCI_API OCI_BindIsNull
(
    OCI_Bind* bnd
)
{
    CALL_IMPL(BindIsNull, bnd)
}

boolean OCI_API OCI_BindSetCharsetForm
(
    OCI_Bind* bnd,
    unsigned int csfrm
)
{
    CALL_IMPL(BindSetCharsetForm, bnd, csfrm)
}

boolean OCI_API OCI_BindSetDirection
(
    OCI_Bind* bnd,
    unsigned int direction
)
{
    CALL_IMPL(BindSetDirection, bnd, direction)
}

unsigned int OCI_API OCI_BindGetDirection
(
    OCI_Bind* bnd
)
{
    CALL_IMPL(BindGetDirection, bnd)
}

unsigned int OCI_API OCI_BindGetAllocationMode
(
    OCI_Bind* bnd
)
{
    CALL_IMPL(BindGetAllocationMode, bnd)
}

/* --------------------------------------------------------------------------------------------- *
 * collection
 * --------------------------------------------------------------------------------------------- */

OCI_Coll * OCI_API OCI_CollCreate
(
    OCI_TypeInfo *typinf
)
{
    CALL_IMPL(CollCreate, typinf)
}

boolean OCI_API OCI_CollFree
(
    OCI_Coll *coll
)
{
    CALL_IMPL(CollFree, coll)
}

OCI_Coll ** OCI_API OCI_CollArrayCreate
(
    OCI_Connection *con,
    OCI_TypeInfo   *typinf,
    unsigned int    nbelem
)
{
    CALL_IMPL(CollArrayCreate, con,  typinf, nbelem)
}

boolean OCI_API OCI_CollArrayFree
(
    OCI_Coll **colls
)
{
    CALL_IMPL(CollArrayFree, colls)
}

boolean OCI_API OCI_CollAssign
(
    OCI_Coll *coll,
    OCI_Coll *coll_src
)
{
    CALL_IMPL(CollAssign, coll, coll_src)
}

unsigned int OCI_API OCI_CollGetType
(
    OCI_Coll *coll
)
{
    CALL_IMPL(CollGetType, coll)
}

unsigned int OCI_API OCI_CollGetMax
(
    OCI_Coll *coll
)
{
    CALL_IMPL(CollGetMax, coll)
}

unsigned int OCI_API OCI_CollGetSize
(
    OCI_Coll *coll
)
{
    CALL_IMPL(CollGetSize, coll)
}

boolean OCI_API OCI_CollTrim
(
    OCI_Coll     *coll,
    unsigned int  nb_elem
)
{
    CALL_IMPL(CollTrim, coll, nb_elem)
}

OCI_Elem * OCI_API OCI_CollGetElem
(
    OCI_Coll    *coll,
    unsigned int index
)
{
    CALL_IMPL(CollGetElem, coll, index)
}

boolean OCI_API OCI_CollGetElem2
(
    OCI_Coll    *coll,
    unsigned int index,
    OCI_Elem    *elem
)
{
    CALL_IMPL(CollGetElem2, coll, index, elem)
}

boolean OCI_API OCI_CollSetElem
(
    OCI_Coll     *coll,
    unsigned int  index,
    OCI_Elem     *elem
)
{
    CALL_IMPL(CollSetElem, coll, index, elem)
}

boolean OCI_API OCI_CollAppend
(
    OCI_Coll *coll,
    OCI_Elem *elem
)
{
    CALL_IMPL(CollAppend, coll, elem)
}

OCI_TypeInfo * OCI_API OCI_CollGetTypeInfo
(
    OCI_Coll *coll
)
{
    CALL_IMPL(CollGetTypeInfo, coll)
}

boolean OCI_API OCI_CollClear
(
    OCI_Coll *coll
)
{
    CALL_IMPL(CollClear, coll)
}

boolean OCI_API OCI_CollDeleteElem
(
    OCI_Coll    *coll,
    unsigned int index
)
{
    CALL_IMPL(CollDeleteElem, coll, index)
}

unsigned int OCI_API OCI_CollGetCount
(
    OCI_Coll *coll
)
{
    CALL_IMPL(CollGetCount, coll)
}

boolean OCI_API OCI_CollToText
(
    OCI_Coll     *coll,
    unsigned int *size,
    otext        *str
)
{
    CALL_IMPL(CollToText, coll, size, str)
}

/* --------------------------------------------------------------------------------------------- *
 * column
 * --------------------------------------------------------------------------------------------- */

const otext* OCI_API OCI_ColumnGetName
(
    OCI_Column* col
)
{
    CALL_IMPL(ColumnGetName, col)
}

unsigned int OCI_API OCI_ColumnGetType
(
    OCI_Column* col
)
{
    CALL_IMPL(ColumnGetType, col)
}

unsigned int OCI_API OCI_ColumnGetCharsetForm
(
    OCI_Column* col
)
{
    CALL_IMPL(ColumnGetCharsetForm, col)
}

unsigned int OCI_API OCI_ColumnGetSize
(
    OCI_Column* col
)
{
    CALL_IMPL(ColumnGetSize, col)
}

int OCI_API OCI_ColumnGetScale
(
    OCI_Column* col
)
{
    CALL_IMPL(ColumnGetScale, col)
}

int OCI_API OCI_ColumnGetPrecision
(
    OCI_Column* col
)
{
    CALL_IMPL(ColumnGetPrecision, col)
}

int OCI_API OCI_ColumnGetFractionalPrecision
(
    OCI_Column* col
)
{
    CALL_IMPL(ColumnGetFractionalPrecision, col)
}

int OCI_API OCI_ColumnGetLeadingPrecision
(
    OCI_Column* col
)
{
    CALL_IMPL(ColumnGetLeadingPrecision, col)
}

boolean OCI_API OCI_ColumnGetNullable
(
    OCI_Column* col
)
{
    CALL_IMPL(ColumnGetNullable, col)
}

boolean OCI_API OCI_ColumnGetCharUsed
(
    OCI_Column* col
)
{
    CALL_IMPL(ColumnGetCharUsed, col)
}

unsigned int OCI_API OCI_ColumnGetPropertyFlags
(
    OCI_Column* col
)
{
    CALL_IMPL(ColumnGetPropertyFlags, col)
}

unsigned int OCI_API OCI_ColumnGetCollationID
(
    OCI_Column* col
)
{
    CALL_IMPL(ColumnGetCollationID, col)
}

const otext* OCI_API OCI_ColumnGetSQLType
(
    OCI_Column* col
)
{
    CALL_IMPL(ColumnGetSQLType, col)
}

unsigned int OCI_API OCI_ColumnGetFullSQLType
(
    OCI_Column* col,
    otext* buffer,
    unsigned int len
)
{
    CALL_IMPL(ColumnGetFullSQLType, col, buffer, len)
}

OCI_TypeInfo* OCI_API OCI_ColumnGetTypeInfo
(
    OCI_Column* col
)
{
    CALL_IMPL(ColumnGetTypeInfo, col)
}

unsigned int OCI_API OCI_ColumnGetSubType
(
    OCI_Column* col
)
{
    CALL_IMPL(ColumnGetSubType, col)
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
    CALL_IMPL(ConnectionCreate, db, user, pwd, mode)
}

boolean OCI_API OCI_ConnectionFree
(
    OCI_Connection *con
)
{
    CALL_IMPL(ConnectionFree, con)
}

boolean OCI_API OCI_Commit
(
    OCI_Connection *con
)
{
    CALL_IMPL(ConnectionCommit, con)
}

boolean OCI_API OCI_Rollback
(
    OCI_Connection *con
)
{
    CALL_IMPL(ConnectionRollback, con)
}

boolean OCI_API OCI_SetAutoCommit
(
    OCI_Connection *con,
    boolean         enable
)
{
    CALL_IMPL(ConnectionSetAutoCommit, con,  enable)
}

boolean OCI_API OCI_GetAutoCommit
(
    OCI_Connection *con
)
{
    CALL_IMPL(ConnectionGetAutoCommit, con)
}

boolean OCI_API OCI_IsConnected
(
    OCI_Connection *con
)
{
    CALL_IMPL(ConnectionIsConnected, con)
}

void * OCI_API OCI_GetUserData
(
    OCI_Connection *con
)
{
    CALL_IMPL(ConnectionGetUserData, con)
}

boolean OCI_API OCI_SetUserData
(
    OCI_Connection *con,
    void           *data
)
{
    CALL_IMPL(ConnectionSetUserData, con,  data)
}

boolean OCI_API OCI_SetSessionTag
(
    OCI_Connection *con,
    const otext    *tag
)
{
    CALL_IMPL(ConnectionSetSessionTag, con,  tag)
}

const otext * OCI_API OCI_GetSessionTag
(
    OCI_Connection *con
)
{
    CALL_IMPL(ConnectionGetSessionTag, con)
}

const otext * OCI_API OCI_GetDatabase
(
    OCI_Connection *con
)
{
    CALL_IMPL(ConnectionGetDatabase, con)
}

const otext * OCI_API OCI_GetUserName
(
    OCI_Connection *con
)
{
    CALL_IMPL(ConnectionGetUserName, con)
}

const otext * OCI_API OCI_GetPassword
(
    OCI_Connection *con
)
{
    CALL_IMPL(ConnectionGetPassword, con)
}

boolean OCI_API OCI_SetPassword
(
    OCI_Connection *con,
    const otext    *password
)
{
    CALL_IMPL(ConnectionSetPassword, con,  password)
}

unsigned int OCI_API OCI_GetSessionMode
(
    OCI_Connection *con
)
{
    CALL_IMPL(ConnectionGetSessionMode, con)
}

const otext * OCI_API OCI_GetVersionServer
(
    OCI_Connection *con
)
{
    CALL_IMPL(ConnectionGetVersionServer, con)
}

unsigned int OCI_API OCI_GetServerMajorVersion
(
    OCI_Connection *con
)
{
    CALL_IMPL(ConnectionGetServerMajorVersion, con)
}

unsigned int OCI_API OCI_GetServerMinorVersion
(
    OCI_Connection *con
)
{
    CALL_IMPL(ConnectionGetServerMinorVersion, con)
}

unsigned int OCI_API OCI_GetServerRevisionVersion
(
    OCI_Connection *con
)
{
    CALL_IMPL(ConnectionGetServerRevisionVersion, con)
}

OCI_Transaction * OCI_API OCI_GetTransaction
(
    OCI_Connection *con
)
{
    CALL_IMPL(ConnectionGetTransaction, con)
}

boolean OCI_API OCI_SetTransaction
(
    OCI_Connection  *con,
    OCI_Transaction *trans
)
{
    CALL_IMPL(ConnectionSetTransaction, con,  trans)
}

unsigned int OCI_API OCI_GetVersionConnection
(
    OCI_Connection *con
)
{
    CALL_IMPL(ConnectionGetVersionConnection, con)
}

boolean OCI_API OCI_Break
(
    OCI_Connection *con
)
{
    CALL_IMPL(ConnectionBreak, con)
}

boolean OCI_API OCI_ServerEnableOutput
(
    OCI_Connection *con,
    unsigned int    bufsize,
    unsigned int    arrsize,
    unsigned int    lnsize
)
{
    CALL_IMPL(ConnectionServerEnableOutput, con,  bufsize, arrsize, lnsize)
}

boolean OCI_API OCI_ServerDisableOutput
(
    OCI_Connection *con
)
{
    CALL_IMPL(ConnectionServerDisableOutput, con)
}

const otext * OCI_API OCI_ServerGetOutput
(
    OCI_Connection *con
)
{
    CALL_IMPL(ConnectionServerGetOutput, con)
}

boolean OCI_API OCI_SetTrace
(
    OCI_Connection *con,
    unsigned int    trace,
    const otext    *value
)
{
    CALL_IMPL(ConnectionSetTrace, con,  trace, value)
}

const otext * OCI_API OCI_GetTrace
(
    OCI_Connection *con,
    unsigned int    trace
)
{
    CALL_IMPL(ConnectionGetTrace, con,  trace)
}

boolean OCI_API OCI_Ping
(
    OCI_Connection *con
)
{
    CALL_IMPL(ConnectionPing, con)
}

boolean OCI_API OCI_SetTimeout
(
    OCI_Connection *con,
    unsigned int    type,
    unsigned int    value
)
{
    CALL_IMPL(ConnectionSetTimeout, con,  type, value)
}

unsigned int OCI_API OCI_GetTimeout
(
    OCI_Connection *con,
    unsigned int    type
)
{
    CALL_IMPL(ConnectionGetTimeout, con,  type)
}

const otext * OCI_API OCI_GetDBName
(
    OCI_Connection *con
)
{
    CALL_IMPL(ConnectionGetDBName, con)
}

const otext * OCI_API OCI_GetInstanceName
(
    OCI_Connection *con
)
{
    CALL_IMPL(ConnectionGetInstanceName, con)
}

const otext * OCI_API OCI_GetServiceName
(
    OCI_Connection *con
)
{
    CALL_IMPL(ConnectionGetServiceName, con)
}

const otext * OCI_API OCI_GetServerName
(
    OCI_Connection *con
)
{
    CALL_IMPL(ConnectionGetServerName, con)
}

const otext * OCI_API OCI_GetDomainName
(
    OCI_Connection *con
)
{
    CALL_IMPL(ConnectionGetDomainName, con)
}

OCI_Timestamp * OCI_API OCI_GetInstanceStartTime
(
    OCI_Connection *con
)
{
    CALL_IMPL(ConnectionGetInstanceStartTime, con)
}

boolean OCI_API OCI_IsTAFCapable
(
    OCI_Connection *con
)
{
    CALL_IMPL(ConnectionIsTAFCapable, con)
}

boolean OCI_API OCI_SetTAFHandler
(
    OCI_Connection  *con,
    POCI_TAF_HANDLER handler
)
{
    CALL_IMPL(ConnectionSetTAFHandler, con,  handler)
}

unsigned int OCI_API OCI_GetStatementCacheSize
(
    OCI_Connection  *con
)
{
    CALL_IMPL(ConnectionGetStatementCacheSize, con)
}

boolean OCI_API OCI_SetStatementCacheSize
(
    OCI_Connection  *con,
    unsigned int     value
)
{
    CALL_IMPL(ConnectionSetStatementCacheSize, con,  value)
}

unsigned int OCI_API OCI_GetDefaultLobPrefetchSize
(
    OCI_Connection *con
)
{
    CALL_IMPL(ConnectionGetDefaultLobPrefetchSize, con)
}

boolean OCI_API OCI_SetDefaultLobPrefetchSize
(
    OCI_Connection *con,
    unsigned int    value
)
{
    CALL_IMPL(ConnectionSetDefaultLobPrefetchSize, con,  value)
}

unsigned int OCI_API OCI_GetMaxCursors
(
    OCI_Connection *con
)
{
    CALL_IMPL(ConnectionGetMaxCursors, con)
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

    return  res;
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
    res = ConnectionExecuteImmediateFmt(con, sql, args);
    va_end(args);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * date
 * --------------------------------------------------------------------------------------------- */

OCI_Date* OCI_API OCI_DateCreate
(
    OCI_Connection* con
)
{
    CALL_IMPL(DateCreate, con)
}

boolean OCI_API OCI_DateFree
(
    OCI_Date* date
)
{
    CALL_IMPL(DateFree, date)
}

OCI_Date** OCI_API OCI_DateArrayCreate
(
    OCI_Connection* con,
    unsigned int    nbelem
)
{
    CALL_IMPL(DateArrayCreate, con, nbelem)
}

boolean OCI_API OCI_DateArrayFree
(
    OCI_Date** dates
)
{
    CALL_IMPL(DateArrayFree, dates)
}

boolean OCI_API OCI_DateAddDays
(
    OCI_Date* date,
    int       nb
)
{
    CALL_IMPL(DateAddDays, date, nb)
}

boolean OCI_API OCI_DateAddMonths
(
    OCI_Date* date,
    int       nb
)
{
    CALL_IMPL(DateAddMonths, date, nb)
}

boolean OCI_API OCI_DateAssign
(
    OCI_Date* date,
    OCI_Date* date_src
)
{
    CALL_IMPL(DateAssign, date, date_src)
}

int OCI_API OCI_DateCheck
(
    OCI_Date* date
)
{
    CALL_IMPL(DateCheck, date)
}

int OCI_API OCI_DateCompare
(
    OCI_Date* date,
    OCI_Date* date2
)
{
    CALL_IMPL(DateCompare, date, date2)
}

int OCI_API OCI_DateDaysBetween
(
    OCI_Date* date,
    OCI_Date* date2
)
{
    CALL_IMPL(DateDaysBetween, date, date2)
}

boolean OCI_API OCI_DateFromText
(
    OCI_Date* date,
    const otext* str,
    const otext* fmt
)
{
    CALL_IMPL(DateFromText, date, str, fmt)
}

boolean OCI_API OCI_DateGetDate
(
    OCI_Date* date,
    int* year,
    int* month,
    int* day
)
{
    CALL_IMPL(DateGetDate, date, year, month, day)
}

boolean OCI_API OCI_DateGetTime
(
    OCI_Date* date,
    int* hour,
    int* min,
    int* sec
)
{
    CALL_IMPL(DateGetTime, date, hour, min, sec)
}

boolean OCI_API OCI_DateGetDateTime
(
    OCI_Date* date,
    int* year,
    int* month,
    int* day,
    int* hour,
    int* min,
    int* sec
)
{
    CALL_IMPL(DateGetDateTime, date, year, month, day, hour, min, sec)
}

boolean OCI_API OCI_DateLastDay
(
    OCI_Date* date
)
{
    CALL_IMPL(DateLastDay, date)
}

boolean OCI_API OCI_DateNextDay
(
    OCI_Date* date,
    const otext* day
)
{
    CALL_IMPL(DateNextDay, date, day)
}

boolean OCI_API OCI_DateSetDate
(
    OCI_Date* date,
    int       year,
    int       month,
    int       day
)
{
    CALL_IMPL(DateSetDate, date, year, month, day)
}

boolean OCI_API OCI_DateSetTime
(
    OCI_Date* date,
    int       hour,
    int       min,
    int       sec
)
{
    CALL_IMPL(DateSetTime, date, hour, min, sec)
}

boolean OCI_API OCI_DateSetDateTime
(
    OCI_Date* date,
    int       year,
    int       month,
    int       day,
    int       hour,
    int       min,
    int       sec
)
{
    CALL_IMPL(DateSetDateTime, date, year, month, day, hour, min ,sec)
}

boolean OCI_API OCI_DateSysDate
(
    OCI_Date* date
)
{
    CALL_IMPL(DateSysDate, date)
}

boolean OCI_API OCI_DateToText
(
    OCI_Date* date,
    const otext* fmt,
    int          size,
    otext* str
)
{
    CALL_IMPL(DateToText, date, fmt, size, str)
}

boolean OCI_API OCI_DateZoneToZone
(
    OCI_Date* date,
    const otext* zone1,
    const otext* zone2
)
{
    CALL_IMPL(DateZoneToZone, date, zone1, zone2)
}

boolean OCI_API OCI_DateToCTime
(
    OCI_Date* date,
    struct tm* ptm,
    time_t* pt
)
{
    CALL_IMPL(DateToCTime, date, ptm, pt)
}

boolean OCI_API OCI_DateFromCTime
(
    OCI_Date* date,
    struct tm* ptm,
    time_t     t
)
{
    CALL_IMPL(DateFromCTime, date, ptm, t)
}













boolean OCI_API OCI_SetUserPassword
(
    const otext* db,
    const otext* user,
    const otext* pwd,
    const otext* new_pwd
)
{
    CALL_IMPL(SetUserPassword, db, user, pwd, new_pwd);
}