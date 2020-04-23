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

/* --------------------------------------------------------------------------------------------- *
 * dequeue
 * --------------------------------------------------------------------------------------------- */

OCI_Dequeue * OCI_API OCI_DequeueCreate
(
    OCI_TypeInfo *typinf,
    const otext  *name
)
{
    CALL_IMPL(DequeueCreate,typinf, name)
}

boolean OCI_API OCI_DequeueFree
(
    OCI_Dequeue *dequeue
)
{
    CALL_IMPL(DequeueFree, dequeue)
}

OCI_Msg * OCI_API OCI_DequeueGet
(
    OCI_Dequeue *dequeue
)
{
    CALL_IMPL(DequeueGet, dequeue)
}

boolean OCI_API OCI_DequeueSubscribe
(
    OCI_Dequeue   *dequeue,
    unsigned int   port,
    unsigned int   timeout,
    POCI_NOTIFY_AQ callback
)
{
    CALL_IMPL(DequeueSubscribe, dequeue, port, timeout, callback)
}

boolean OCI_API OCI_DequeueUnsubscribe
(
    OCI_Dequeue *dequeue
)
{
    CALL_IMPL(DequeueUnsubscribe, dequeue)
}

boolean OCI_API OCI_DequeueSetConsumer
(
    OCI_Dequeue *dequeue,
    const otext *consumer
)
{
    CALL_IMPL(DequeueSetConsumer, dequeue, consumer)
}

const otext * OCI_API OCI_DequeueGetConsumer
(
    OCI_Dequeue *dequeue
)
{
    CALL_IMPL(DequeueGetConsumer, dequeue)
}

boolean OCI_API OCI_DequeueSetCorrelation
(
    OCI_Dequeue *dequeue,
    const otext *pattern
)
{
    CALL_IMPL(DequeueSetCorrelation, dequeue, pattern)
}

const otext * OCI_API OCI_DequeueGetCorrelation
(
    OCI_Dequeue *dequeue
)
{
    CALL_IMPL(DequeueGetCorrelation, dequeue)
}

boolean OCI_API OCI_DequeueSetRelativeMsgID
(
    OCI_Dequeue *dequeue,
    const void  *id,
    unsigned int len
)
{
    CALL_IMPL(DequeueSetRelativeMsgID, dequeue, id, len)
}

boolean OCI_API OCI_DequeueGetRelativeMsgID
(
    OCI_Dequeue  *dequeue,
    void         *id,
    unsigned int *len
)
{
    CALL_IMPL(DequeueGetRelativeMsgID, dequeue, id , len)
}

boolean OCI_API OCI_DequeueSetVisibility
(
    OCI_Dequeue *dequeue,
    unsigned int visibility
)
{
    CALL_IMPL(DequeueSetVisibility, dequeue, visibility)
}

unsigned int OCI_API OCI_DequeueGetVisibility
(
    OCI_Dequeue *dequeue
)
{
    CALL_IMPL(DequeueGetVisibility, dequeue)
}

boolean OCI_API OCI_DequeueSetMode
(
    OCI_Dequeue *dequeue,
    unsigned int mode
)
{
    CALL_IMPL(DequeueSetMode, dequeue, mode)
}

unsigned int OCI_API OCI_DequeueGetMode
(
    OCI_Dequeue *dequeue
)
{
    CALL_IMPL(DequeueGetMode, dequeue)
}

boolean OCI_API OCI_DequeueSetNavigation
(
    OCI_Dequeue *dequeue,
    unsigned int position
)
{
    CALL_IMPL(DequeueSetNavigation, dequeue, position)
}

unsigned int OCI_API OCI_DequeueGetNavigation
(
    OCI_Dequeue *dequeue
)
{
    CALL_IMPL(DequeueGetNavigation, dequeue)
}

boolean OCI_API OCI_DequeueSetWaitTime
(
    OCI_Dequeue *dequeue,
    int          timeout
)
{
    CALL_IMPL(DequeueSetWaitTime, dequeue, timeout)
}

int OCI_API OCI_DequeueGetWaitTime
(
    OCI_Dequeue *dequeue
)
{
    CALL_IMPL(DequeueGetWaitTime, dequeue)
}

boolean OCI_API OCI_DequeueSetAgentList
(
    OCI_Dequeue *dequeue,
    OCI_Agent  **consumers,
    unsigned int count
)
{
    CALL_IMPL(DequeueSetAgentList, dequeue, consumers, count)
}

OCI_Agent * OCI_API OCI_DequeueListen
(
    OCI_Dequeue *dequeue,
    int          timeout
)
{
    CALL_IMPL(DequeueListen, dequeue, timeout)
}

/* --------------------------------------------------------------------------------------------- *
 * dirpath
 * --------------------------------------------------------------------------------------------- */

OCI_DirPath* OCI_API OCI_DirPathCreate
(
    OCI_TypeInfo* typinf,
    const otext* partition,
    unsigned int  nb_cols,
    unsigned int  nb_rows
)
{
    CALL_IMPL(DirPathCreate, typinf, partition, nb_cols, nb_rows)
}

boolean OCI_API OCI_DirPathFree
(
    OCI_DirPath* dp
)
{
    CALL_IMPL(DirPathFree, dp)
}

boolean OCI_API OCI_DirPathSetColumn
(
    OCI_DirPath* dp,
    unsigned int index,
    const otext* name,
    unsigned int maxsize,
    const otext* format
)
{
    CALL_IMPL(DirPathSetColumn, dp, index, name, maxsize, format)
}

boolean OCI_API OCI_DirPathPrepare
(
    OCI_DirPath* dp
)
{
    CALL_IMPL(DirPathPrepare, dp)
}

boolean OCI_API OCI_DirPathSetEntry
(
    OCI_DirPath* dp,
    unsigned int row,
    unsigned int index,
    void* value,
    unsigned     size,
    boolean      complete
)
{
    CALL_IMPL(DirPathSetEntry, dp, row, index, value, size, complete)
}

unsigned int OCI_API OCI_DirPathConvert
(
    OCI_DirPath* dp
)
{
    CALL_IMPL(DirPathConvert, dp)
}

unsigned int OCI_API OCI_DirPathLoad
(
    OCI_DirPath* dp
)
{
    CALL_IMPL(DirPathLoad, dp)
}

boolean OCI_API OCI_DirPathReset
(
    OCI_DirPath* dp
)
{
    CALL_IMPL(DirPathReset, dp)
}

boolean OCI_API OCI_DirPathFinish
(
    OCI_DirPath* dp
)
{
    CALL_IMPL(DirPathFinish, dp)
}

boolean OCI_API OCI_DirPathAbort
(
    OCI_DirPath* dp
)
{
    CALL_IMPL(DirPathAbort, dp)
}

boolean OCI_API OCI_DirPathSave
(
    OCI_DirPath* dp
)
{
    CALL_IMPL(DirPathSave, dp)
}

boolean OCI_API OCI_DirPathFlushRow
(
    OCI_DirPath* dp
)
{
    CALL_IMPL(DirPathFlushRow, dp)
}

boolean OCI_API OCI_DirPathSetCurrentRows
(
    OCI_DirPath* dp,
    unsigned int nb_rows
)
{
    CALL_IMPL(DirPathSetCurrentRows, dp, nb_rows)
}

unsigned int OCI_API OCI_DirPathGetCurrentRows
(
    OCI_DirPath* dp
)
{
    CALL_IMPL(DirPathGetCurrentRows, dp)
}

unsigned int OCI_API OCI_DirPathGetMaxRows
(
    OCI_DirPath* dp
)
{
    CALL_IMPL(DirPathGetMaxRows, dp)
}

boolean OCI_API OCI_DirPathSetDateFormat
(
    OCI_DirPath* dp,
    const otext* format
)
{
    CALL_IMPL(DirPathSetDateFormat, dp, format)
}

boolean OCI_API OCI_DirPathSetParallel
(
    OCI_DirPath* dp,
    boolean      value
)
{
    CALL_IMPL(DirPathSetParallel, dp, value)
}

boolean OCI_API OCI_DirPathSetNoLog
(
    OCI_DirPath* dp,
    boolean      value
)
{
    CALL_IMPL(DirPathSetNoLog, dp, value)
}

boolean OCI_API OCI_DirPathSetCacheSize
(
    OCI_DirPath* dp,
    unsigned int size
)
{
    CALL_IMPL(DirPathSetCacheSize, dp, size)
}

boolean OCI_API OCI_DirPathSetBufferSize
(
    OCI_DirPath* dp,
    unsigned int size
)
{
    CALL_IMPL(DirPathSetBufferSize, dp, size)
}

boolean OCI_API OCI_DirPathSetConvertMode
(
    OCI_DirPath* dp,
    unsigned int mode
)
{
    CALL_IMPL(DirPathSetConvertMode, dp, mode)
}

unsigned int OCI_API OCI_DirPathGetRowCount
(
    OCI_DirPath* dp
)
{
    CALL_IMPL(DirPathGetRowCount, dp)
}

unsigned int OCI_API OCI_DirPathGetAffectedRows
(
    OCI_DirPath* dp
)
{
    CALL_IMPL(DirPathGetAffectedRows, dp)
}

unsigned int OCI_API OCI_DirPathGetErrorColumn
(
    OCI_DirPath* dp
)
{
    CALL_IMPL(DirPathGetErrorColumn, dp)
}

unsigned int OCI_API OCI_DirPathGetErrorRow
(
    OCI_DirPath* dp
)
{
    CALL_IMPL(DirPathGetErrorRow, dp)
}

/* --------------------------------------------------------------------------------------------- *
 * element
 * --------------------------------------------------------------------------------------------- */

OCI_Elem* OCI_API OCI_ElemCreate
(
    OCI_TypeInfo* typinf
)
{
    CALL_IMPL(ElemCreate, typinf)
}

boolean OCI_API OCI_ElemFree
(
    OCI_Elem* elem
)
{
    CALL_IMPL(ElemFree, elem)
}

boolean OCI_API OCI_ElemGetBoolean
(
    OCI_Elem* elem
)
{
    CALL_IMPL(ElemGetBoolean, elem)
}

OCI_Number* OCI_API OCI_ElemGetNumber
(
    OCI_Elem* elem
)
{
    CALL_IMPL(ElemGetNumber, elem)
}

short OCI_API OCI_ElemGetShort
(
    OCI_Elem* elem
)
{
    CALL_IMPL(ElemGetShort, elem)
}

unsigned short OCI_API OCI_ElemGetUnsignedShort
(
    OCI_Elem* elem
)
{
    CALL_IMPL(ElemGetUnsignedShort, elem)
}

int OCI_API OCI_ElemGetInt
(
    OCI_Elem* elem
)
{
    CALL_IMPL(ElemGetInt, elem)
}

unsigned int OCI_API OCI_ElemGetUnsignedInt
(
    OCI_Elem* elem
)
{
    CALL_IMPL(ElemGetUnsignedInt, elem)
}

big_int OCI_API OCI_ElemGetBigInt
(
    OCI_Elem* elem
)
{
    CALL_IMPL(ElemGetBigInt, elem)
}

big_uint OCI_API OCI_ElemGetUnsignedBigInt
(
    OCI_Elem* elem
)
{
    CALL_IMPL(ElemGetUnsignedBigInt, elem)
}

double OCI_API OCI_ElemGetDouble
(
    OCI_Elem* elem
)
{
    CALL_IMPL(ElemGetDouble, elem)
}

float OCI_API OCI_ElemGetFloat
(
    OCI_Elem* elem
)
{
    CALL_IMPL(ElemGetFloat, elem)
}

const otext* OCI_API OCI_ElemGetString
(
    OCI_Elem* elem
)
{
    CALL_IMPL(ElemGetString, elem)
}

unsigned int OCI_API OCI_ElemGetRaw
(
    OCI_Elem* elem,
    void* value,
    unsigned int len
)
{
    CALL_IMPL(ElemGetRaw, elem, value, len)
}

unsigned int OCI_API OCI_ElemGetRawSize
(
    OCI_Elem* elem
)
{
    CALL_IMPL(ElemGetRawSize, elem)
}

OCI_Date* OCI_API OCI_ElemGetDate
(
    OCI_Elem* elem
)
{
    CALL_IMPL(ElemGetDate, elem)
}

OCI_Timestamp* OCI_API OCI_ElemGetTimestamp
(
    OCI_Elem* elem
)
{
    CALL_IMPL(ElemGetTimestamp, elem)
}

OCI_Interval* OCI_API OCI_ElemGetInterval
(
    OCI_Elem* elem
)
{
    CALL_IMPL(ElemGetInterval, elem)
}

OCI_Lob* OCI_API OCI_ElemGetLob
(
    OCI_Elem* elem
)
{
    CALL_IMPL(ElemGetLob, elem)
}

OCI_File* OCI_API OCI_ElemGetFile
(
    OCI_Elem* elem
)
{
    CALL_IMPL(ElemGetFile, elem)
}

OCI_Ref* OCI_API OCI_ElemGetRef
(
    OCI_Elem* elem
)
{
    CALL_IMPL(ElemGetRef, elem)
}

OCI_Object* OCI_API OCI_ElemGetObject
(
    OCI_Elem* elem
)
{
    CALL_IMPL(ElemGetObject, elem)
}

OCI_Coll* OCI_API OCI_ElemGetColl
(
    OCI_Elem* elem
)
{
    CALL_IMPL(ElemGetColl, elem)
}

boolean OCI_API OCI_ElemSetBoolean
(
    OCI_Elem* elem,
    boolean   value
)
{
    CALL_IMPL(ElemSetBoolean, elem, value)
}

boolean OCI_API OCI_ElemSetNumber
(
    OCI_Elem* elem,
    OCI_Number* value
)
{
    CALL_IMPL(ElemSetNumber, elem, value)
}

boolean OCI_API OCI_ElemSetShort
(
    OCI_Elem* elem,
    short     value
)
{
    CALL_IMPL(ElemSetShort, elem, value)
}

boolean OCI_API OCI_ElemSetUnsignedShort
(
    OCI_Elem* elem,
    unsigned short value
)
{
    CALL_IMPL(ElemSetUnsignedShort, elem, value)
}

boolean OCI_API OCI_ElemSetInt
(
    OCI_Elem* elem,
    int       value
)
{
    CALL_IMPL(ElemSetInt, elem, value)
}

boolean OCI_API OCI_ElemSetUnsignedInt
(
    OCI_Elem* elem,
    unsigned int value
)
{
    CALL_IMPL(ElemSetUnsignedInt, elem, value)
}

boolean OCI_API OCI_ElemSetBigInt
(
    OCI_Elem* elem,
    big_int   value
)
{
    CALL_IMPL(ElemSetBigInt, elem, value)
}

boolean OCI_API OCI_ElemSetUnsignedBigInt
(
    OCI_Elem* elem,
    big_uint  value
)
{
    CALL_IMPL(ElemSetUnsignedBigInt, elem, value)
}

boolean OCI_API OCI_ElemSetDouble
(
    OCI_Elem* elem,
    double    value
)
{
    CALL_IMPL(ElemSetDouble, elem, value)
}

boolean OCI_API OCI_ElemSetFloat
(
    OCI_Elem* elem,
    float     value
)
{
    CALL_IMPL(ElemSetFloat, elem, value)
}

boolean OCI_API OCI_ElemSetString
(
    OCI_Elem* elem,
    const otext* value
)
{
    CALL_IMPL(ElemSetString, elem, value)
}

boolean OCI_API OCI_ElemSetRaw
(
    OCI_Elem* elem,
    void* value,
    unsigned int len
)
{
    CALL_IMPL(ElemSetRaw, elem, value, len)
}

boolean OCI_API OCI_ElemSetDate
(
    OCI_Elem* elem,
    OCI_Date* value
)
{
    CALL_IMPL(ElemSetDate, elem, value)
}

boolean OCI_API OCI_ElemSetTimestamp
(
    OCI_Elem* elem,
    OCI_Timestamp* value
)
{
    CALL_IMPL(ElemSetTimestamp, elem, value)
}

boolean OCI_API OCI_ElemSetInterval
(
    OCI_Elem* elem,
    OCI_Interval* value
)
{
    CALL_IMPL(ElemSetInterval, elem, value)
}

boolean OCI_API OCI_ElemSetColl
(
    OCI_Elem* elem,
    OCI_Coll* value
)
{
    CALL_IMPL(ElemSetColl, elem, value)
}

boolean OCI_API OCI_ElemSetObject
(
    OCI_Elem* elem,
    OCI_Object* value
)
{
    CALL_IMPL(ElemSetObject, elem, value)
}

boolean OCI_API OCI_ElemSetLob
(
    OCI_Elem* elem,
    OCI_Lob* value
)
{
    CALL_IMPL(ElemSetLob, elem, value)
}

boolean OCI_API OCI_ElemSetFile
(
    OCI_Elem* elem,
    OCI_File* value
)
{
    CALL_IMPL(ElemSetFile, elem, value)
}

boolean OCI_API OCI_ElemSetRef
(
    OCI_Elem* elem,
    OCI_Ref* value
)
{
    CALL_IMPL(ElemSetRef, elem, value)
}

boolean OCI_API OCI_ElemIsNull
(
    OCI_Elem* elem
)
{
    CALL_IMPL(ElemIsNull, elem)
}

boolean OCI_API OCI_ElemSetNull
(
    OCI_Elem* elem
)
{
    CALL_IMPL(ElemSetNull, elem)
}

/* --------------------------------------------------------------------------------------------- *
 * enqueue
 * --------------------------------------------------------------------------------------------- */

OCI_Enqueue* OCI_API OCI_EnqueueCreate
(
    OCI_TypeInfo* typinf,
    const otext* name
)
{
    CALL_IMPL(EnqueueCreate, typinf, name)
}

boolean OCI_API OCI_EnqueueFree
(
    OCI_Enqueue* enqueue
)
{
    CALL_IMPL(EnqueueFree, enqueue)
}

boolean OCI_API OCI_EnqueuePut
(
    OCI_Enqueue* enqueue,
    OCI_Msg* msg
)
{
    CALL_IMPL(EnqueuePut, enqueue, msg)
}

boolean OCI_API OCI_EnqueueSetSequenceDeviation
(
    OCI_Enqueue* enqueue,
    unsigned int sequence
)
{
    CALL_IMPL(EnqueueSetSequenceDeviation, enqueue, sequence)
}

unsigned int OCI_API OCI_EnqueueGetSequenceDeviation
(
    OCI_Enqueue* enqueue
)
{
    CALL_IMPL(EnqueueGetSequenceDeviation, enqueue)
}

boolean OCI_API OCI_EnqueueSetVisibility
(
    OCI_Enqueue* enqueue,
    unsigned int visibility
)
{
    CALL_IMPL(EnqueueSetVisibility, enqueue, visibility)
}

unsigned int OCI_API OCI_EnqueueGetVisibility
(
    OCI_Enqueue* enqueue
)
{
    CALL_IMPL(EnqueueGetVisibility, enqueue)
}

boolean OCI_API OCI_EnqueueSetRelativeMsgID
(
    OCI_Enqueue* enqueue,
    const void* id,
    unsigned int len
)
{
    CALL_IMPL(EnqueueSetRelativeMsgID, enqueue, id, len)
}

boolean OCI_API OCI_EnqueueGetRelativeMsgID
(
    OCI_Enqueue* enqueue,
    void* id,
    unsigned int* len
)
{
    CALL_IMPL(EnqueueGetRelativeMsgID, enqueue, id, len)
}

/* --------------------------------------------------------------------------------------------- *
 * error
 * --------------------------------------------------------------------------------------------- */

const otext* OCI_API OCI_ErrorGetString
(
    OCI_Error* err
)
{
    CALL_IMPL(ErrorGetString, err)
}

unsigned int OCI_API OCI_ErrorGetType
(
    OCI_Error* err
)
{
    CALL_IMPL(ErrorGetType, err)
}

int OCI_API OCI_ErrorGetOCICode
(
    OCI_Error* err
)
{
    CALL_IMPL(ErrorGetOCICode, err)
}

int OCI_API OCI_ErrorGetInternalCode
(
    OCI_Error* err
)
{
    CALL_IMPL(ErrorGetInternalCode, err)
}

OCI_Connection* OCI_API OCI_ErrorGetConnection
(
    OCI_Error* err
)
{
    CALL_IMPL(ErrorGetConnection, err)
}

OCI_Statement* OCI_API OCI_ErrorGetStatement
(
    OCI_Error* err
)
{
    CALL_IMPL(ErrorGetStatement, err)
}

unsigned int OCI_API OCI_ErrorGetRow
(
    OCI_Error* err
)
{
    CALL_IMPL(ErrorGetRow, err)
}

/* --------------------------------------------------------------------------------------------- *
 * event
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_EventGetType
(
    OCI_Event* event
)
{
    CALL_IMPL(EventGetType, event)
}


unsigned int OCI_API OCI_EventGetOperation
(
    OCI_Event* event
)
{
    CALL_IMPL(EventGetOperation, event)
}

const otext* OCI_API OCI_EventGetDatabase
(
    OCI_Event* event
)
{
    CALL_IMPL(EventGetDatabase, event)
}

const otext* OCI_API OCI_EventGetObject
(
    OCI_Event* event
)
{
    CALL_IMPL(EventGetObject, event)
}

const otext* OCI_API OCI_EventGetRowid
(
    OCI_Event* event
)
{
    CALL_IMPL(EventGetRowid, event)
}

OCI_Subscription* OCI_API OCI_EventGetSubscription
(
    OCI_Event* event
)
{
    CALL_IMPL(EventGetSubscription, event)
}

/* --------------------------------------------------------------------------------------------- *
 * file
 * --------------------------------------------------------------------------------------------- */

OCI_File* OCI_API OCI_FileCreate
(
    OCI_Connection* con,
    unsigned int    type
)
{
    CALL_IMPL(FileCreate, con, type)
}

boolean OCI_API OCI_FileFree
(
    OCI_File* file
)
{
    CALL_IMPL(FileFree, file)
}

OCI_File** OCI_API OCI_FileArrayCreate
(
    OCI_Connection* con,
    unsigned int    type,
    unsigned int    nbelem
)
{
    CALL_IMPL(FileArrayCreate, con, type, nbelem)
}

boolean OCI_API OCI_FileArrayFree
(
    OCI_File** files
)
{
    CALL_IMPL(FileArrayFree, files)
}

unsigned int OCI_API OCI_FileGetType
(
    OCI_File* file
)
{
    CALL_IMPL(FileGetType, file)
}

boolean OCI_API OCI_FileSeek
(
    OCI_File* file,
    big_uint offset,
    unsigned int mode
)
{
    CALL_IMPL(FileSeek, file, offset, mode)
}

big_uint OCI_API OCI_FileGetOffset
(
    OCI_File* file
)
{
    CALL_IMPL(FileGetOffset, file)
}

unsigned int OCI_API OCI_FileRead
(
    OCI_File* file,
    void* buffer,
    unsigned int len
)
{
    CALL_IMPL(FileRead, file, buffer, len)
}

big_uint OCI_API OCI_FileGetSize
(
    OCI_File* file
)
{
    CALL_IMPL(FileGetSize, file)
}

boolean OCI_API OCI_FileExists
(
    OCI_File* file
)
{
    CALL_IMPL(FileExists, file)
}

boolean OCI_API OCI_FileSetName
(
    OCI_File* file,
    const otext* dir,
    const otext* name
)
{
    CALL_IMPL(FileSetName, file, dir, name)
}

const otext* OCI_API OCI_FileGetDirectory
(
    OCI_File* file
)
{
    CALL_IMPL(FileGetDirectory, file)
}

const otext* OCI_API OCI_FileGetName
(
    OCI_File* file
)
{
    CALL_IMPL(FileGetName, file)
}

boolean OCI_API OCI_FileOpen
(
    OCI_File* file
)
{
    CALL_IMPL(FileOpen, file)
}

boolean OCI_API OCI_FileIsOpen
(
    OCI_File* file
)
{
    CALL_IMPL(FileIsOpen, file)
}

boolean OCI_API OCI_FileClose
(
    OCI_File* file
)
{
    CALL_IMPL(FileClose, file)
}

boolean OCI_API OCI_FileIsEqual
(
    OCI_File* file,
    OCI_File* file2
)
{
    CALL_IMPL(FileIsEqual, file, file2)
}

boolean OCI_API OCI_FileAssign
(
    OCI_File* file,
    OCI_File* file_src
)
{
    CALL_IMPL(FileAssign, file, file_src)
}

OCI_Connection* OCI_API OCI_FileGetConnection
(
    OCI_File* file
)
{
    CALL_IMPL(FileGetConnection, file)
}

/* --------------------------------------------------------------------------------------------- *
 * handle
 * --------------------------------------------------------------------------------------------- */


const void* OCI_API OCI_HandleGetEnvironment
(
    void
)
{
    CALL_IMPL(HandleGetEnvironment)
}

const void* OCI_API OCI_HandleGetContext
(
    OCI_Connection* con
)
{
    CALL_IMPL(HandleGetContext, con)
}

const void* OCI_API OCI_HandleGetServer
(
    OCI_Connection* con
)
{
    CALL_IMPL(HandleGetServer, con)
}

const void* OCI_API OCI_HandleGetError
(
    OCI_Connection* con
)
{
    CALL_IMPL(HandleGetError, con)
}

const void* OCI_API OCI_HandleGetSession
(
    OCI_Connection* con
)
{
    CALL_IMPL(HandleGetSession, con)
}

const void* OCI_API OCI_HandleGetTransaction
(
    OCI_Transaction* trans
)
{
    CALL_IMPL(HandleGetTransaction, trans)
}

const void* OCI_API OCI_HandleGetStatement
(
    OCI_Statement* stmt
)
{
    CALL_IMPL(HandleGetStatement, stmt)
}

const void* OCI_API OCI_HandleGetLob
(
    OCI_Lob* lob
)
{
    CALL_IMPL(HandleGetLob, lob)
}

const void* OCI_API OCI_HandleGetFile
(
    OCI_File* file
)
{
    CALL_IMPL(HandleGetFile, file)
}

const void* OCI_API OCI_HandleGetDate
(
    OCI_Date* date
)
{
    CALL_IMPL(HandleGetDate, date)
}

const void* OCI_API OCI_HandleGetTimestamp
(
    OCI_Timestamp* tmsp
)
{
    CALL_IMPL(HandleGetTimestamp, tmsp)
}

const void* OCI_API OCI_HandleGetInterval
(
    OCI_Interval* itv
)
{
    CALL_IMPL(HandleGetInterval, itv)
}

const void* OCI_API OCI_HandleGetObject
(
    OCI_Object* obj
)
{
    CALL_IMPL(HandleGetObject, obj)
}

const void* OCI_API OCI_HandleGetColl
(
    OCI_Coll* coll
)
{
    CALL_IMPL(HandleGetColl, coll)
}

const void* OCI_API OCI_HandleGetRef
(
    OCI_Ref* ref
)
{
    CALL_IMPL(HandleGetRef, ref)
}

const void* OCI_API OCI_HandleGetMutex
(
    OCI_Mutex* mutex
)
{
    CALL_IMPL(HandleGetMutex, mutex)
}

const void* OCI_API OCI_HandleGetThreadID
(
    OCI_Thread* thread
)
{
    CALL_IMPL(HandleGetThreadID, thread)
}

const void* OCI_API OCI_HandleGetThread
(
    OCI_Thread* thread
)
{
    CALL_IMPL(HandleGetThread, thread)
}

const void* OCI_API OCI_HandleGetDirPathCtx
(
    OCI_DirPath* dp
)
{
    CALL_IMPL(HandleGetDirPathCtx, dp)
}

const void* OCI_API OCI_HandleGetDirPathColArray
(
    OCI_DirPath* dp
)
{
    CALL_IMPL(HandleGetDirPathColArray, dp)
}

const void* OCI_API OCI_HandleGetDirPathStream
(
    OCI_DirPath* dp
)
{
    CALL_IMPL(HandleGetDirPathStream, dp)
}

const void* OCI_API OCI_HandleGetSubscription
(
    OCI_Subscription* sub
)
{
    CALL_IMPL(HandleGetSubscription, sub)
}

/* --------------------------------------------------------------------------------------------- *
 * hash
 * --------------------------------------------------------------------------------------------- */

OCI_HashTable* OCI_API OCI_HashCreate
(
    unsigned int size,
    unsigned int type
)
{
    CALL_IMPL(HashCreate, size, type)
}

boolean OCI_API OCI_HashFree
(
    OCI_HashTable* table
)
{
    CALL_IMPL(HashFree, table)
}

unsigned int OCI_API OCI_HashGetSize
(
    OCI_HashTable* table
)
{
    CALL_IMPL(HashGetSize, table)
}

unsigned int OCI_API OCI_HashGetType
(
    OCI_HashTable* table
)
{
    CALL_IMPL(HashGetType, table)
}

boolean OCI_API OCI_HashAddString
(
    OCI_HashTable* table,
    const otext* key,
    const otext* value
)
{
    CALL_IMPL(HashAddString, table, key, value)
}

const otext* OCI_API OCI_HashGetString
(
    OCI_HashTable* table,
    const otext* key
)
{
    CALL_IMPL(HashGetString, table, key)
}

boolean OCI_API OCI_HashAddInt
(
    OCI_HashTable* table,
    const otext* key,
    int value
)
{
    CALL_IMPL(HashAddInt, table, key, value)
}

int OCI_API OCI_HashGetInt
(
    OCI_HashTable* table,
    const otext* key
)
{
    CALL_IMPL(HashGetInt, table, key)
}

boolean OCI_API OCI_HashAddPointer
(
    OCI_HashTable* table,
    const otext* key,
    void* value
)
{
    CALL_IMPL(HashAddPointer, table, key, value)
}

void* OCI_API OCI_HashGetPointer
(
    OCI_HashTable* table,
    const otext* key
)
{
    CALL_IMPL(HashGetPointer, table, key)
}

OCI_HashEntry* OCI_API OCI_HashLookup
(
    OCI_HashTable* table,
    const otext* key,
    boolean create
)
{
    CALL_IMPL(HashLookup, table, key, create)
}

OCI_HashValue* OCI_API OCI_HashGetValue
(
    OCI_HashTable* table,
    const otext* key
)
{
    CALL_IMPL(HashGetValue, table, key)
}

OCI_HashEntry* OCI_API OCI_HashGetEntry
(
    OCI_HashTable* table,
    unsigned int   index
)
{
    CALL_IMPL(HashGetEntry, table, index)
}

/* --------------------------------------------------------------------------------------------- *
 * interval
 * --------------------------------------------------------------------------------------------- */

OCI_Interval* OCI_API OCI_IntervalCreate
(
    OCI_Connection* con,
    unsigned int    type
)
{
    CALL_IMPL(IntervalCreate, con, type)
}

boolean OCI_API OCI_IntervalFree
(
    OCI_Interval* itv
)
{
    CALL_IMPL(IntervalFree, itv)
}

OCI_Interval** OCI_API OCI_IntervalArrayCreate
(
    OCI_Connection* con,
    unsigned int    type,
    unsigned int    nbelem
)
{
    CALL_IMPL(IntervalArrayCreate, con, type, nbelem)
}

boolean OCI_API OCI_IntervalArrayFree
(
    OCI_Interval** itvs
)
{
    CALL_IMPL(IntervalArrayFree, itvs)
}

unsigned int OCI_API OCI_IntervalGetType
(
    OCI_Interval* itv
)
{
    CALL_IMPL(IntervalGetType, itv)
}

boolean OCI_API OCI_IntervalAssign
(
    OCI_Interval* itv,
    OCI_Interval* itv_src
)
{
    CALL_IMPL(IntervalAssign, itv, itv_src)
}

int OCI_API OCI_IntervalCheck
(
    OCI_Interval* itv
)
{
    CALL_IMPL(IntervalCheck, itv)
}

int OCI_API OCI_IntervalCompare
(
    OCI_Interval* itv,
    OCI_Interval* itv2
)
{
    CALL_IMPL(IntervalCompare, itv, itv2)
}

boolean OCI_API OCI_IntervalFromText
(
    OCI_Interval* itv,
    const otext* str
)
{
    CALL_IMPL(IntervalFromText, itv, str)
}

boolean OCI_API OCI_IntervalToText
(
    OCI_Interval* itv,
    int           leading_prec,
    int           fraction_prec,
    int           size,
    otext* str
)
{
    CALL_IMPL(IntervalToText, itv, leading_prec, fraction_prec, size, str)
}

boolean OCI_API OCI_IntervalFromTimeZone
(
    OCI_Interval* itv,
    const otext* str
)
{
    CALL_IMPL(IntervalFromTimeZone, itv, str)
}

boolean OCI_API OCI_IntervalGetDaySecond
(
    OCI_Interval* itv,
    int* day,
    int* hour,
    int* min,
    int* sec,
    int* fsec
)
{
    CALL_IMPL(IntervalGetDaySecond, itv, day, hour, min, sec, fsec)
}

boolean OCI_API OCI_IntervalGetYearMonth
(
    OCI_Interval* itv,
    int* year,
    int* month
)
{
    CALL_IMPL(IntervalGetYearMonth, itv, year, month)
}

boolean OCI_API OCI_IntervalSetDaySecond
(
    OCI_Interval* itv,
    int           day,
    int           hour,
    int           min,
    int           sec,
    int           fsec
)
{
    CALL_IMPL(IntervalSetDaySecond, itv, day, hour, min, sec, fsec)
}

boolean OCI_API OCI_IntervalSetYearMonth
(
    OCI_Interval* itv,
    int           year,
    int           month
)
{
    CALL_IMPL(IntervalSetYearMonth, itv, year, month)
}

boolean OCI_API OCI_IntervalAdd
(
    OCI_Interval* itv,
    OCI_Interval* itv2
)
{
    CALL_IMPL(IntervalAdd, itv, itv2)
}

boolean OCI_API OCI_IntervalSubtract
(
    OCI_Interval* itv,
    OCI_Interval* itv2
)
{
    CALL_IMPL(IntervalSubtract, itv, itv2)
}

/* --------------------------------------------------------------------------------------------- *
 * iterator
 * --------------------------------------------------------------------------------------------- */

OCI_Iter* OCI_API OCI_IterCreate
(
    OCI_Coll* coll
)
{
    CALL_IMPL(IterCreate, coll)
}

boolean OCI_API OCI_IterFree
(
    OCI_Iter* iter
)
{
    CALL_IMPL(IterFree, iter)
}

OCI_Elem* OCI_API OCI_IterGetNext
(
    OCI_Iter* iter
)
{
    CALL_IMPL(IterGetNext, iter)
}

OCI_Elem* OCI_API OCI_IterGetPrev
(
    OCI_Iter* iter
)
{
    CALL_IMPL(IterGetPrev, iter)
}

OCI_Elem* OCI_API OCI_IterGetCurrent
(
    OCI_Iter* iter
)
{
    CALL_IMPL(IterGetCurrent, iter)
}

/* --------------------------------------------------------------------------------------------- *
 * library
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_Initialize
(
    POCI_ERROR   err_handler,
    const otext* lib_path,
    unsigned int mode
)
{
    CALL_IMPL(Initialize, err_handler, lib_path, mode)
}

boolean OCI_API OCI_Cleanup
(
    void
)
{
    CALL_IMPL(Cleanup)
}

unsigned int OCI_API OCI_GetOCICompileVersion
(
    void
)
{
    CALL_IMPL(GetOCICompileVersion)
}

unsigned int OCI_API OCI_GetOCIRuntimeVersion
(
    void
)
{
    CALL_IMPL(GetOCIRuntimeVersion)
}

unsigned int OCI_API OCI_GetImportMode
(
    void
)
{
    CALL_IMPL(GetImportMode)
}

unsigned int OCI_API OCI_GetCharset
(
    void
)
{
    CALL_IMPL(GetCharset)
}


big_uint OCI_API OCI_GetAllocatedBytes
(
    unsigned int mem_type
)
{
    CALL_IMPL(GetAllocatedBytes, mem_type)
}

OCI_Error* OCI_API OCI_GetLastError
(
    void
)
{
    CALL_IMPL(EnvironmentGetLastError)
}

boolean OCI_API OCI_EnableWarnings
(
    boolean value
)
{
    CALL_IMPL(EnableWarnings, value)
}

boolean OCI_API OCI_SetErrorHandler
(
    POCI_ERROR handler
)
{
    CALL_IMPL(SetErrorHandler, handler)
}

boolean OCI_API OCI_DatabaseStartup
(
    const otext* db,
    const otext* user,
    const otext* pwd,
    unsigned int sess_mode,
    unsigned int start_mode,
    unsigned int start_flag,
    const otext* spfile
)
{
    CALL_IMPL(DatabaseStartup, db, user, pwd, sess_mode, start_mode, start_flag, spfile)
}

boolean OCI_API OCI_DatabaseShutdown
(
    const otext* db,
    const otext* user,
    const otext* pwd,
    unsigned int sess_mode,
    unsigned int shut_mode,
    unsigned int shut_flag
)
{
    CALL_IMPL(DatabaseShutdown, db, user, pwd, sess_mode, shut_mode, shut_flag)
}

boolean OCI_API OCI_SetHAHandler
(
    POCI_HA_HANDLER  handler
)
{
    CALL_IMPL(SetHAHandler, handler)
}

boolean OCI_API OCI_SetFormat
(
    OCI_Connection* con,
    unsigned int    type,
    const otext* format
)
{
    CALL_IMPL(SetFormat, con, type, format)
}

const otext* OCI_API OCI_GetFormat
(
    OCI_Connection* con,
    unsigned int    type
)
{
    CALL_IMPL(GetFormat, con, type)
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

/* --------------------------------------------------------------------------------------------- *
 * lob
 * --------------------------------------------------------------------------------------------- */

OCI_Lob* OCI_API OCI_LobCreate
(
    OCI_Connection* con,
    unsigned int    type
)
{
    CALL_IMPL(LobCreate, con, type);
}

boolean OCI_API OCI_LobFree
(
    OCI_Lob* lob
)
{
    CALL_IMPL(LobFree, lob);
}

OCI_Lob** OCI_API OCI_LobArrayCreate
(
    OCI_Connection* con,
    unsigned int    type,
    unsigned int    nbelem
)
{
    CALL_IMPL(LobArrayCreate, con, type, nbelem);
}

boolean OCI_API OCI_LobArrayFree
(
    OCI_Lob** lobs
)
{
    CALL_IMPL(LobArrayFree, lobs);
}

unsigned int OCI_API OCI_LobGetType
(
    OCI_Lob* lob
)
{
    CALL_IMPL(LobGetType, lob);
}

boolean OCI_API OCI_LobSeek
(
    OCI_Lob* lob,
    big_uint     offset,
    unsigned int mode
)
{
    CALL_IMPL(LobSeek, lob, offset, mode);
}

big_uint OCI_API OCI_LobGetOffset
(
    OCI_Lob* lob
)
{
    CALL_IMPL(LobGetOffset, lob);
}

unsigned int OCI_API OCI_LobRead
(
    OCI_Lob* lob,
    void* buffer,
    unsigned int len
)
{
    CALL_IMPL(LobRead, lob, buffer, len);
}

boolean OCI_API OCI_LobRead2
(
    OCI_Lob* lob,
    void* buffer,
    unsigned int* char_count,
    unsigned int* byte_count
)
{
    CALL_IMPL(LobRead2, lob, buffer, char_count, byte_count);
}

unsigned int OCI_API OCI_LobWrite
(
    OCI_Lob* lob,
    void* buffer,
    unsigned int len
)
{
    CALL_IMPL(LobWrite, lob, buffer, len);
}

boolean OCI_API OCI_LobWrite2
(
    OCI_Lob* lob,
    void* buffer,
    unsigned int* char_count,
    unsigned int* byte_count
)
{
    CALL_IMPL(LobWrite2, lob, buffer, char_count, byte_count);
}

boolean OCI_API OCI_LobTruncate
(
    OCI_Lob* lob,
    big_uint size
)
{
    CALL_IMPL(LobTruncate, lob, size);
}

big_uint OCI_API OCI_LobGetLength
(
    OCI_Lob* lob
)
{
    CALL_IMPL(LobGetLength, lob);
}

unsigned int OCI_API OCI_LobGetChunkSize
(
    OCI_Lob* lob
)
{
    CALL_IMPL(LobGetChunkSize, lob);
}

big_uint OCI_API OCI_LobErase
(
    OCI_Lob* lob,
    big_uint offset,
    big_uint len
)
{
    CALL_IMPL(LobErase, lob, offset, len);
}

unsigned int OCI_API OCI_LobAppend
(
    OCI_Lob* lob,
    void* buffer,
    unsigned int len
)
{
    CALL_IMPL(LobAppend, lob, buffer, len);
}

boolean OCI_API OCI_LobAppend2
(
    OCI_Lob* lob,
    void* buffer,
    unsigned int* char_count,
    unsigned int* byte_count
)
{
    CALL_IMPL(LobAppend2, lob, buffer, char_count, byte_count);
}

boolean OCI_API OCI_LobAppendLob
(
    OCI_Lob* lob,
    OCI_Lob* lob_src
)
{
    CALL_IMPL(LobAppendLob, lob, lob_src);
}

boolean OCI_API OCI_LobIsTemporary
(
    OCI_Lob* lob
)
{
    CALL_IMPL(LobIsTemporary, lob);
}

boolean OCI_API OCI_LobCopy
(
    OCI_Lob* lob,
    OCI_Lob* lob_src,
    big_uint offset_dst,
    big_uint offset_src,
    big_uint count
)
{
    CALL_IMPL(LobCopy, lob, lob_src, offset_dst, offset_src, count);
}

boolean OCI_API OCI_LobCopyFromFile
(
    OCI_Lob* lob,
    OCI_File* file,
    big_uint  offset_dst,
    big_uint  offset_src,
    big_uint  count
)
{
    CALL_IMPL(LobCopyFromFile, lob, file, offset_dst, offset_src, count);
}

boolean OCI_API OCI_LobOpen
(
    OCI_Lob* lob,
    unsigned int mode
)
{
    CALL_IMPL(LobOpen, lob, mode);
}

boolean OCI_API OCI_LobClose
(
    OCI_Lob* lob
)
{
    CALL_IMPL(LobClose, lob);
}

boolean OCI_API OCI_LobIsEqual
(
    OCI_Lob* lob,
    OCI_Lob* lob2
)
{
    CALL_IMPL(LobIsEqual, lob, lob2);
}

boolean OCI_API OCI_LobAssign
(
    OCI_Lob* lob,
    OCI_Lob* lob_src
)
{
    CALL_IMPL(LobAssign, lob, lob_src);
}

big_uint OCI_API OCI_LobGetMaxSize
(
    OCI_Lob* lob
)
{
    CALL_IMPL(LobGetMaxSize, lob);
}

boolean OCI_API OCI_LobFlush
(
    OCI_Lob* lob
)
{
    CALL_IMPL(LobFlush, lob);
}

boolean OCI_API OCI_LobEnableBuffering
(
    OCI_Lob* lob,
    boolean  value
)
{
    CALL_IMPL(LobEnableBuffering, lob, value);
}

OCI_Connection* OCI_API OCI_LobGetConnection
(
    OCI_Lob* lob
)
{
    CALL_IMPL(LobGetConnection, lob);
}

boolean OCI_API OCI_LobIsRemote
(
    OCI_Lob* lob
)
{
    CALL_IMPL(LobIsRemote, lob);
}

 /* --------------------------------------------------------------------------------------------- *
  *  long
  * --------------------------------------------------------------------------------------------- */

OCI_Long* OCI_API OCI_LongCreate
(
    OCI_Statement* stmt,
    unsigned int   type
)
{
    CALL_IMPL(LongCreate, stmt, type);
}

boolean OCI_API OCI_LongFree
(
    OCI_Long* lg
)
{
    CALL_IMPL(LongFree, lg);
}

unsigned int OCI_API OCI_LongGetType
(
    OCI_Long* lg
)
{
    CALL_IMPL(LongGetType, lg);
}

unsigned int OCI_API OCI_LongRead
(
    OCI_Long* lg,
    void* buffer,
    unsigned int len
)
{
    CALL_IMPL(LongRead, lg, buffer, len);
}

unsigned int OCI_API OCI_LongWrite
(
    OCI_Long* lg,
    void* buffer,
    unsigned int len
)
{
    CALL_IMPL(LongWrite, lg, buffer, len);
}

unsigned int OCI_API OCI_LongGetSize
(
    OCI_Long* lg
)
{
    CALL_IMPL(LongGetSize, lg);
}

void* OCI_API OCI_LongGetBuffer
(
    OCI_Long* lg
)
{
    CALL_IMPL(LongGetBuffer, lg);
}

/* --------------------------------------------------------------------------------------------- *
 *  msg
 * --------------------------------------------------------------------------------------------- */

OCI_Msg* OCI_API OCI_MsgCreate
(
    OCI_TypeInfo* typinf
)
{
    CALL_IMPL(MsgCreate, typinf);
}

boolean OCI_API OCI_MsgFree
(
    OCI_Msg* msg
)
{
    CALL_IMPL(MsgFree, msg);
}

boolean OCI_API OCI_MsgReset
(
    OCI_Msg* msg
)
{
    CALL_IMPL(MsgReset, msg);
}

OCI_Object* OCI_API OCI_MsgGetObject
(
    OCI_Msg* msg
)
{
    CALL_IMPL(MsgGetObject, msg);
}

boolean OCI_API OCI_MsgSetObject
(
    OCI_Msg* msg,
    OCI_Object* obj
)
{
    CALL_IMPL(MsgSetObject, msg, obj);
}

boolean OCI_API OCI_MsgGetRaw
(
    OCI_Msg* msg,
    void* raw,
    unsigned int* size
)
{
    CALL_IMPL(MsgGetRaw, msg, raw, size);
}

boolean OCI_API OCI_MsgSetRaw
(
    OCI_Msg* msg,
    const void* raw,
    unsigned int size
)
{
    CALL_IMPL(MsgSetRaw, msg, raw, size);
}

int OCI_API OCI_MsgGetAttemptCount
(
    OCI_Msg* msg
)
{
    CALL_IMPL(MsgGetAttemptCount, msg);
}

int OCI_API OCI_MsgGetEnqueueDelay
(
    OCI_Msg* msg
)
{
    CALL_IMPL(MsgGetEnqueueDelay, msg);
}

boolean OCI_API OCI_MsgSetEnqueueDelay
(
    OCI_Msg* msg,
    int      value
)
{
    CALL_IMPL(MsgSetEnqueueDelay, msg, value);
}

OCI_Date* OCI_API OCI_MsgGetEnqueueTime
(
    OCI_Msg* msg
)
{
    CALL_IMPL(MsgGetEnqueueTime, msg);
}

int OCI_API OCI_MsgGetExpiration
(
    OCI_Msg* msg
)
{
    CALL_IMPL(MsgGetExpiration, msg);
}

boolean OCI_API OCI_MsgSetExpiration
(
    OCI_Msg* msg,
    int      value
)
{
    CALL_IMPL(MsgSetExpiration, msg, value);
}

unsigned int OCI_API OCI_MsgGetState
(
    OCI_Msg* msg
)
{
    CALL_IMPL(MsgGetState, msg);
}

int OCI_API OCI_MsgGetPriority
(
    OCI_Msg* msg
)
{
    CALL_IMPL(MsgGetPriority, msg);
}

boolean OCI_API OCI_MsgSetPriority
(
    OCI_Msg* msg,
    int      value
)
{
    CALL_IMPL(MsgSetPriority, msg, value);
}

boolean OCI_API OCI_MsgGetID
(
    OCI_Msg* msg,
    void* id,
    unsigned int* len
)
{
    CALL_IMPL(MsgGetID, msg, id, len);
}

boolean OCI_API OCI_MsgGetOriginalID
(
    OCI_Msg* msg,
    void* id,
    unsigned int* len
)
{
    CALL_IMPL(MsgGetOriginalID, msg, id, len);
}

boolean OCI_API OCI_MsgSetOriginalID
(
    OCI_Msg* msg,
    const void* id,
    unsigned int len
)
{
    CALL_IMPL(MsgSetOriginalID, msg, id, len);
}

OCI_Agent* OCI_API OCI_MsgGetSender
(
    OCI_Msg* msg
)
{
    CALL_IMPL(MsgGetSender, msg);
}

boolean OCI_API OCI_MsgSetSender
(
    OCI_Msg* msg,
    OCI_Agent* sender
)
{
    CALL_IMPL(MsgSetSender, msg, sender);
}

boolean OCI_API OCI_MsgSetConsumers
(
    OCI_Msg* msg,
    OCI_Agent** consumers,
    unsigned int count
)
{
    CALL_IMPL(MsgSetConsumers, msg, consumers, count);
}

const otext* OCI_API OCI_MsgGetCorrelation
(
    OCI_Msg* msg
)
{
    CALL_IMPL(MsgGetCorrelation, msg);
}

boolean OCI_API OCI_MsgSetCorrelation
(
    OCI_Msg* msg,
    const otext* correlation
)
{
    CALL_IMPL(MsgSetCorrelation, msg, correlation);
}

const otext* OCI_API OCI_MsgGetExceptionQueue
(
    OCI_Msg* msg
)
{
    CALL_IMPL(MsgGetExceptionQueue, msg);
}

boolean OCI_API OCI_MsgSetExceptionQueue
(
    OCI_Msg* msg,
    const otext* queue
)
{
    CALL_IMPL(MsgSetExceptionQueue, msg, queue);
}

/* --------------------------------------------------------------------------------------------- *
 *  mutex
 * --------------------------------------------------------------------------------------------- */

OCI_Mutex* OCI_API OCI_MutexCreate
(
    void
)
{
    CALL_IMPL(MutexCreate);
}

boolean OCI_API OCI_MutexFree
(
    OCI_Mutex* mutex
)
{
    CALL_IMPL(MutexFree, mutex);
}

boolean OCI_API OCI_MutexAcquire
(
    OCI_Mutex* mutex
)
{
    CALL_IMPL(MutexAcquire, mutex);
}

boolean OCI_API OCI_MutexRelease
(
    OCI_Mutex* mutex
)
{
    CALL_IMPL(MutexRelease, mutex);
}

/* --------------------------------------------------------------------------------------------- *
 *  number
 * --------------------------------------------------------------------------------------------- */

OCI_Number* OCI_API OCI_NumberCreate
(
    OCI_Connection* con
)
{
    CALL_IMPL(NumberCreate, con);
}

boolean OCI_API OCI_NumberFree
(
    OCI_Number* number
)
{
    CALL_IMPL(NumberFree, number);
}

OCI_Number** OCI_API OCI_NumberArrayCreate
(
    OCI_Connection* con,
    unsigned int    nbelem
)
{
    CALL_IMPL(NumberArrayCreate, con, nbelem);
}

OCI_EXPORT boolean OCI_API OCI_NumberArrayFree
(
    OCI_Number** numbmers
)
{
    CALL_IMPL(NumberArrayFree, numbmers);
}

boolean OCI_API OCI_NumberAssign
(
    OCI_Number* number,
    OCI_Number* number_src
)
{
    CALL_IMPL(NumberAssign, number, number_src);
}

boolean OCI_API OCI_NumberToText
(
    OCI_Number* number,
    const otext* fmt,
    int          size,
    otext* str
)
{
    CALL_IMPL(NumberToText, number, fmt, size, str);
}

boolean OCI_API OCI_NumberFromText
(
    OCI_Number* number,
    const otext* str,
    const otext* fmt
)
{
    CALL_IMPL(NumberFromText, number, str, fmt);
}

unsigned char* OCI_API OCI_NumberGetContent
(
    OCI_Number* number
)
{
    CALL_IMPL(NumberGetContent, number);
}

boolean OCI_API OCI_NumberSetContent
(
    OCI_Number* number,
    unsigned char* content
)
{
    CALL_IMPL(NumberSetContent, number, content);
}

boolean OCI_API OCI_NumberSetValue
(
    OCI_Number* number,
    unsigned int    type,
    void* value
)
{
    CALL_IMPL(NumberSetValue, number, type, value);
}

boolean OCI_API OCI_NumberGetValue
(
    OCI_Number* number,
    unsigned int    type,
    void* value
)
{
    CALL_IMPL(NumberGetValue, number, type, value);
}

boolean OCI_API OCI_NumberAdd
(
    OCI_Number* number,
    unsigned int    type,
    void* value
)
{
    CALL_IMPL(NumberAdd, number, type, value);
}

boolean OCI_API OCI_NumberSub
(
    OCI_Number* number,
    unsigned int    type,
    void* value
)
{
    CALL_IMPL(NumberSub, number, type, value);
}

boolean OCI_API OCI_NumberMultiply
(
    OCI_Number* number,
    unsigned int    type,
    void* value
)
{
    CALL_IMPL(NumberMultiply, number, type, value);
}

boolean OCI_API OCI_NumberDivide
(
    OCI_Number* number,
    unsigned int    type,
    void* value
)
{
    CALL_IMPL(NumberDivide, number, type, value);
}

int OCI_API OCI_NumberCompare
(
    OCI_Number* number1,
    OCI_Number* number2
)
{
    CALL_IMPL(NumberCompare, number1, number2);
}

/* --------------------------------------------------------------------------------------------- *
 *  long
 * --------------------------------------------------------------------------------------------- */


/* --------------------------------------------------------------------------------------------- *
 *  long
 * --------------------------------------------------------------------------------------------- */


/* --------------------------------------------------------------------------------------------- *
 *  long
 * --------------------------------------------------------------------------------------------- */


/* --------------------------------------------------------------------------------------------- *
 *  long
 * --------------------------------------------------------------------------------------------- */


/* --------------------------------------------------------------------------------------------- *
 *  long
 * --------------------------------------------------------------------------------------------- */


/* --------------------------------------------------------------------------------------------- *
 *  long
 * --------------------------------------------------------------------------------------------- */


/* --------------------------------------------------------------------------------------------- *
 *  long
 * --------------------------------------------------------------------------------------------- */


/* --------------------------------------------------------------------------------------------- *
 *  long
 * --------------------------------------------------------------------------------------------- */


/* --------------------------------------------------------------------------------------------- *
 *  long
 * --------------------------------------------------------------------------------------------- */
