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

#include "ocilib.h"

#include "agent.h"
#include "array.h"
#include "bind.h"
#include "collection.h"
#include "column.h"
#include "connection.h"
#include "database.h"
#include "date.h"
#include "dequeue.h"
#include "dirpath.h"
#include "element.h"
#include "enqueue.h"
#include "error.h"
#include "event.h"
#include "file.h"
#include "handle.h"
#include "hash.h"
#include "interval.h"
#include "iterator.h"
#include "environment.h"
#include "lob.h"
#include "long.h"
#include "message.h"
#include "mutex.h"
#include "number.h"
#include "object.h"
#include "pool.h"
#include "queue.h"
#include "reference.h"
#include "resultset.h"
#include "statement.h"
#include "subscription.h"
#include "thread.h"
#include "threadkey.h"
#include "timestamp.h"
#include "transaction.h"
#include "typeinfo.h"

void CheckContext()
{
    if (Env.env_mode & OCI_ENV_CONTEXT)
    {
        ErrorReset(ErrorGet(TRUE, TRUE));
    }
}

#define CALL_IMPL(impl, ...)  \
    CheckContext();           \
    return impl(__VA_ARGS__); \


/* --------------------------------------------------------------------------------------------- *
  * agent
  * --------------------------------------------------------------------------------------------- */

OCI_Agent* OCI_API OCI_AgentCreate
(
    OCI_Connection* con,
    const otext   * name,
    const otext   * address
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
    OCI_Agent  * agent,
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
    OCI_Agent  * agent,
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
    OCI_Bind   * bnd,
    unsigned int size
)
{
    CALL_IMPL(BindSetDataSize, bnd, size)
}

boolean OCI_API OCI_BindSetDataSizeAtPos
(
    OCI_Bind   * bnd,
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
    OCI_Bind   * bnd,
    unsigned int position
)
{
    CALL_IMPL(BindGetDataSizeAtPos, bnd, position)
}

boolean OCI_API OCI_BindSetNullAtPos
(
    OCI_Bind   * bnd,
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
    OCI_Bind   * bnd,
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
    OCI_Bind   * bnd,
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
    OCI_Bind   * bnd,
    unsigned int csfrm
)
{
    CALL_IMPL(BindSetCharsetForm, bnd, csfrm)
}

boolean OCI_API OCI_BindSetDirection
(
    OCI_Bind   * bnd,
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
    CALL_IMPL(CollectionCreate, typinf)
}

boolean OCI_API OCI_CollFree
(
    OCI_Coll *coll
)
{
    CALL_IMPL(CollectionFree, coll)
}

OCI_Coll ** OCI_API OCI_CollArrayCreate
(
    OCI_Connection *con,
    OCI_TypeInfo   *typinf,
    unsigned int    nbelem
)
{
    CALL_IMPL(CollectionCreateArray, con, typinf, nbelem)
}

boolean OCI_API OCI_CollArrayFree
(
    OCI_Coll **colls
)
{
    CALL_IMPL(CollectionFreeArray, colls)
}

boolean OCI_API OCI_CollAssign
(
    OCI_Coll *coll,
    OCI_Coll *coll_src
)
{
    CALL_IMPL(CollectionAssign, coll, coll_src)
}

unsigned int OCI_API OCI_CollGetType
(
    OCI_Coll *coll
)
{
    CALL_IMPL(CollectionGetType, coll)
}

unsigned int OCI_API OCI_CollGetMax
(
    OCI_Coll *coll
)
{
    CALL_IMPL(CollectionGetMax, coll)
}

unsigned int OCI_API OCI_CollGetSize
(
    OCI_Coll *coll
)
{
    CALL_IMPL(CollectionGetSize, coll)
}

boolean OCI_API OCI_CollTrim
(
    OCI_Coll    *coll,
    unsigned int nb_elem
)
{
    CALL_IMPL(CollectionTrim, coll, nb_elem)
}

OCI_Elem * OCI_API OCI_CollGetElem
(
    OCI_Coll    *coll,
    unsigned int index
)
{
    CALL_IMPL(CollectionGetElement, coll, index)
}

boolean OCI_API OCI_CollGetElem2
(
    OCI_Coll    *coll,
    unsigned int index,
    OCI_Elem    *elem
)
{
    CALL_IMPL(CollectionGetElement2, coll, index, elem)
}

boolean OCI_API OCI_CollSetElem
(
    OCI_Coll    *coll,
    unsigned int index,
    OCI_Elem    *elem
)
{
    CALL_IMPL(CollectionSetElement, coll, index, elem)
}

boolean OCI_API OCI_CollAppend
(
    OCI_Coll *coll,
    OCI_Elem *elem
)
{
    CALL_IMPL(CollectionAddElement, coll, elem)
}

OCI_TypeInfo * OCI_API OCI_CollGetTypeInfo
(
    OCI_Coll *coll
)
{
    CALL_IMPL(CollectionGetTypeInfo, coll)
}

boolean OCI_API OCI_CollClear
(
    OCI_Coll *coll
)
{
    CALL_IMPL(CollectionClear, coll)
}

boolean OCI_API OCI_CollDeleteElem
(
    OCI_Coll    *coll,
    unsigned int index
)
{
    CALL_IMPL(CollectionRemoveElement, coll, index)
}

unsigned int OCI_API OCI_CollGetCount
(
    OCI_Coll *coll
)
{
    CALL_IMPL(CollectionGetCount, coll)
}

boolean OCI_API OCI_CollToText
(
    OCI_Coll     *coll,
    unsigned int *size,
    otext        *str
)
{
    CALL_IMPL(CollectionToString, coll, size, str)
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
    CALL_IMPL(ColumnGetSqlType, col)
}

unsigned int OCI_API OCI_ColumnGetFullSQLType
(
    OCI_Column * col,
    otext      * buffer,
    unsigned int len
)
{
    CALL_IMPL(ColumnGetFullSqlType, col, buffer, len)
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
    CALL_IMPL(ConnectionSetAutoCommit, con, enable)
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
    CALL_IMPL(ConnectionSetUserData, con, data)
}

boolean OCI_API OCI_SetSessionTag
(
    OCI_Connection *con,
    const otext    *tag
)
{
    CALL_IMPL(ConnectionSetSessionTag, con, tag)
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
    CALL_IMPL(ConnectionGetConnectionString, con)
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
    CALL_IMPL(ConnectionSetPassword, con, password)
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
    CALL_IMPL(ConnectionGetServerVersion, con)
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
    CALL_IMPL(ConnectionSetTransaction, con, trans)
}

unsigned int OCI_API OCI_GetVersionConnection
(
    OCI_Connection *con
)
{
    CALL_IMPL(ConnectionGetVersion, con)
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
    CALL_IMPL(ConnectionEnableServerOutput, con, bufsize, arrsize, lnsize)
}

boolean OCI_API OCI_ServerDisableOutput
(
    OCI_Connection *con
)
{
    CALL_IMPL(ConnectionDisableServerOutput, con)
}

const otext * OCI_API OCI_ServerGetOutput
(
    OCI_Connection *con
)
{
    CALL_IMPL(ConnectionGetServerOutput, con)
}

boolean OCI_API OCI_SetTrace
(
    OCI_Connection *con,
    unsigned int    trace,
    const otext    *value
)
{
    CALL_IMPL(ConnectionSetTrace, con, trace, value)
}

const otext * OCI_API OCI_GetTrace
(
    OCI_Connection *con,
    unsigned int    trace
)
{
    CALL_IMPL(ConnectionGetTrace, con, trace)
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
    CALL_IMPL(ConnectionSetTimeout, con, type, value)
}

unsigned int OCI_API OCI_GetTimeout
(
    OCI_Connection *con,
    unsigned int    type
)
{
    CALL_IMPL(ConnectionGetTimeout, con, type)
}

const otext * OCI_API OCI_GetDBName
(
    OCI_Connection *con
)
{
    CALL_IMPL(ConnectionGetDatabaseName, con)
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
    CALL_IMPL(ConnectionSetTAFHandler, con, handler)
}

unsigned int OCI_API OCI_GetStatementCacheSize
(
    OCI_Connection *con
)
{
    CALL_IMPL(ConnectionGetStatementCacheSize, con)
}

boolean OCI_API OCI_SetStatementCacheSize
(
    OCI_Connection *con,
    unsigned int    value
)
{
    CALL_IMPL(ConnectionSetStatementCacheSize, con, value)
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
    CALL_IMPL(ConnectionSetDefaultLobPrefetchSize, con, value)
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
    CALL_IMPL(DateCreateArray, con, nbelem)
}

boolean OCI_API OCI_DateArrayFree
(
    OCI_Date** dates
)
{
    CALL_IMPL(DateFreeArray, dates)
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
    OCI_Date   * date,
    const otext* str,
    const otext* fmt
)
{
    CALL_IMPL(DateFromString, date, str, fmt)
}

boolean OCI_API OCI_DateGetDate
(
    OCI_Date* date,
    int     * year,
    int     * month,
    int     * day
)
{
    CALL_IMPL(DateGetDate, date, year, month, day)
}

boolean OCI_API OCI_DateGetTime
(
    OCI_Date* date,
    int     * hour,
    int     * min,
    int     * sec
)
{
    CALL_IMPL(DateGetTime, date, hour, min, sec)
}

boolean OCI_API OCI_DateGetDateTime
(
    OCI_Date* date,
    int     * year,
    int     * month,
    int     * day,
    int     * hour,
    int     * min,
    int     * sec
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
    OCI_Date   * date,
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
    CALL_IMPL(DateSetDateTime, date, year, month, day, hour, min,sec)
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
    OCI_Date   * date,
    const otext* fmt,
    int          size,
    otext      * str
)
{
    CALL_IMPL(DateToString, date, fmt, size, str)
}

boolean OCI_API OCI_DateZoneToZone
(
    OCI_Date   * date,
    const otext* zone1,
    const otext* zone2
)
{
    CALL_IMPL(DateZoneToZone, date, zone1, zone2)
}

boolean OCI_API OCI_DateToCTime
(
    OCI_Date * date,
    struct tm* ptm,
    time_t   * pt
)
{
    CALL_IMPL(DateToCTime, date, ptm, pt)
}

boolean OCI_API OCI_DateFromCTime
(
    OCI_Date * date,
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
    CALL_IMPL(DequeueGetMessage, dequeue)
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
    CALL_IMPL(DequeueGetRelativeMsgID, dequeue, id, len)
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
 * direct path
 * --------------------------------------------------------------------------------------------- */

OCI_DirPath* OCI_API OCI_DirPathCreate
(
    OCI_TypeInfo* typinf,
    const otext * partition,
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
    void       * value,
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
    CALL_IMPL(ElementCreate, typinf)
}

boolean OCI_API OCI_ElemFree
(
    OCI_Elem* elem
)
{
    CALL_IMPL(ElementFree, elem)
}

boolean OCI_API OCI_ElemGetBoolean
(
    OCI_Elem* elem
)
{
    CALL_IMPL(ElementGetBoolean, elem)
}

OCI_Number* OCI_API OCI_ElemGetNumber
(
    OCI_Elem* elem
)
{
    CALL_IMPL(ElementGetNumber, elem)
}

short OCI_API OCI_ElemGetShort
(
    OCI_Elem* elem
)
{
    CALL_IMPL(ElementGetShort, elem)
}

unsigned short OCI_API OCI_ElemGetUnsignedShort
(
    OCI_Elem* elem
)
{
    CALL_IMPL(ElementGetUnsignedShort, elem)
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
    CALL_IMPL(ElementGetUnsignedInt, elem)
}

big_int OCI_API OCI_ElemGetBigInt
(
    OCI_Elem* elem
)
{
    CALL_IMPL(ElementGetBigInt, elem)
}

big_uint OCI_API OCI_ElemGetUnsignedBigInt
(
    OCI_Elem* elem
)
{
    CALL_IMPL(ElementGetUnsignedBigInt, elem)
}

double OCI_API OCI_ElemGetDouble
(
    OCI_Elem* elem
)
{
    CALL_IMPL(ElementGetDouble, elem)
}

float OCI_API OCI_ElemGetFloat
(
    OCI_Elem* elem
)
{
    CALL_IMPL(ElementGetFloat, elem)
}

const otext* OCI_API OCI_ElemGetString
(
    OCI_Elem* elem
)
{
    CALL_IMPL(ElementGetString, elem)
}

unsigned int OCI_API OCI_ElemGetRaw
(
    OCI_Elem   * elem,
    void       * value,
    unsigned int len
)
{
    CALL_IMPL(ElementGetRaw, elem, value, len)
}

unsigned int OCI_API OCI_ElemGetRawSize
(
    OCI_Elem* elem
)
{
    CALL_IMPL(ElementGetRawSize, elem)
}

OCI_Date* OCI_API OCI_ElemGetDate
(
    OCI_Elem* elem
)
{
    CALL_IMPL(ElementGetDate, elem)
}

OCI_Timestamp* OCI_API OCI_ElemGetTimestamp
(
    OCI_Elem* elem
)
{
    CALL_IMPL(ElementGetTimestamp, elem)
}

OCI_Interval* OCI_API OCI_ElemGetInterval
(
    OCI_Elem* elem
)
{
    CALL_IMPL(ElementGetInterval, elem)
}

OCI_Lob* OCI_API OCI_ElemGetLob
(
    OCI_Elem* elem
)
{
    CALL_IMPL(ElementGetLob, elem)
}

OCI_File* OCI_API OCI_ElemGetFile
(
    OCI_Elem* elem
)
{
    CALL_IMPL(ElementGetFile, elem)
}

OCI_Ref* OCI_API OCI_ElemGetRef
(
    OCI_Elem* elem
)
{
    CALL_IMPL(ElementGetReference, elem)
}

OCI_Object* OCI_API OCI_ElemGetObject
(
    OCI_Elem* elem
)
{
    CALL_IMPL(ElementGetObject, elem)
}

OCI_Coll* OCI_API OCI_ElemGetColl
(
    OCI_Elem* elem
)
{
    CALL_IMPL(ElementGetCollection, elem)
}

boolean OCI_API OCI_ElemSetBoolean
(
    OCI_Elem* elem,
    boolean   value
)
{
    CALL_IMPL(ElementSetBoolean, elem, value)
}

boolean OCI_API OCI_ElemSetNumber
(
    OCI_Elem  * elem,
    OCI_Number* value
)
{
    CALL_IMPL(ElementSetNumber, elem, value)
}

boolean OCI_API OCI_ElemSetShort
(
    OCI_Elem* elem,
    short     value
)
{
    CALL_IMPL(ElementSetShort, elem, value)
}

boolean OCI_API OCI_ElemSetUnsignedShort
(
    OCI_Elem     * elem,
    unsigned short value
)
{
    CALL_IMPL(ElementSetUnsignedShort, elem, value)
}

boolean OCI_API OCI_ElemSetInt
(
    OCI_Elem* elem,
    int       value
)
{
    CALL_IMPL(ElementSetInt, elem, value)
}

boolean OCI_API OCI_ElemSetUnsignedInt
(
    OCI_Elem   * elem,
    unsigned int value
)
{
    CALL_IMPL(ElementSetUnsignedInt, elem, value)
}

boolean OCI_API OCI_ElemSetBigInt
(
    OCI_Elem* elem,
    big_int   value
)
{
    CALL_IMPL(ElementSetBigInt, elem, value)
}

boolean OCI_API OCI_ElemSetUnsignedBigInt
(
    OCI_Elem* elem,
    big_uint  value
)
{
    CALL_IMPL(ElementSetUnsignedBigInt, elem, value)
}

boolean OCI_API OCI_ElemSetDouble
(
    OCI_Elem* elem,
    double    value
)
{
    CALL_IMPL(ElementSetDouble, elem, value)
}

boolean OCI_API OCI_ElemSetFloat
(
    OCI_Elem* elem,
    float     value
)
{
    CALL_IMPL(ElementSetFloat, elem, value)
}

boolean OCI_API OCI_ElemSetString
(
    OCI_Elem   * elem,
    const otext* value
)
{
    CALL_IMPL(ElementSetString, elem, value)
}

boolean OCI_API OCI_ElemSetRaw
(
    OCI_Elem   * elem,
    void       * value,
    unsigned int len
)
{
    CALL_IMPL(ElementSetRaw, elem, value, len)
}

boolean OCI_API OCI_ElemSetDate
(
    OCI_Elem* elem,
    OCI_Date* value
)
{
    CALL_IMPL(ElementSetDate, elem, value)
}

boolean OCI_API OCI_ElemSetTimestamp
(
    OCI_Elem     * elem,
    OCI_Timestamp* value
)
{
    CALL_IMPL(ElementSetTimestamp, elem, value)
}

boolean OCI_API OCI_ElemSetInterval
(
    OCI_Elem    * elem,
    OCI_Interval* value
)
{
    CALL_IMPL(ElementSetInterval, elem, value)
}

boolean OCI_API OCI_ElemSetColl
(
    OCI_Elem* elem,
    OCI_Coll* value
)
{
    CALL_IMPL(ElementSetCollection, elem, value)
}

boolean OCI_API OCI_ElemSetObject
(
    OCI_Elem  * elem,
    OCI_Object* value
)
{
    CALL_IMPL(ElementSetObject, elem, value)
}

boolean OCI_API OCI_ElemSetLob
(
    OCI_Elem* elem,
    OCI_Lob * value
)
{
    CALL_IMPL(ElementSetLob, elem, value)
}

boolean OCI_API OCI_ElemSetFile
(
    OCI_Elem* elem,
    OCI_File* value
)
{
    CALL_IMPL(ElementSetFile, elem, value)
}

boolean OCI_API OCI_ElemSetRef
(
    OCI_Elem* elem,
    OCI_Ref * value
)
{
    CALL_IMPL(ElementSetReference, elem, value)
}

boolean OCI_API OCI_ElemIsNull
(
    OCI_Elem* elem
)
{
    CALL_IMPL(ElementIsNull, elem)
}

boolean OCI_API OCI_ElemSetNull
(
    OCI_Elem* elem
)
{
    CALL_IMPL(ElementSetNull, elem)
}

/* --------------------------------------------------------------------------------------------- *
 * enqueue
 * --------------------------------------------------------------------------------------------- */

OCI_Enqueue* OCI_API OCI_EnqueueCreate
(
    OCI_TypeInfo* typinf,
    const otext * name
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
    OCI_Msg    * msg
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
    const void * id,
    unsigned int len
)
{
    CALL_IMPL(EnqueueSetRelativeMsgID, enqueue, id, len)
}

boolean OCI_API OCI_EnqueueGetRelativeMsgID
(
    OCI_Enqueue * enqueue,
    void        * id,
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
    return ErrorGetString(err);
}

unsigned int OCI_API OCI_ErrorGetType
(
    OCI_Error* err
)
{
    return ErrorGetType(err);
}

int OCI_API OCI_ErrorGetOCICode
(
    OCI_Error* err
)
{
    return ErrorGetOCICode(err);
}

int OCI_API OCI_ErrorGetInternalCode
(
    OCI_Error* err
)
{
    return ErrorGetInternalCode(err);
}

OCI_Connection* OCI_API OCI_ErrorGetConnection
(
    OCI_Error* err
)
{
    return ErrorGetConnection(err);
}

OCI_Statement* OCI_API OCI_ErrorGetStatement
(
    OCI_Error* err
)
{
    return ErrorGetStatement(err);
}

unsigned int OCI_API OCI_ErrorGetRow
(
    OCI_Error* err
)
{
    return ErrorGetRow(err);
}

const otext* OCI_API OCI_ErrorGetLocation
(
    OCI_Error* err
)
{
    return ErrorGetLocation(err);
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
    CALL_IMPL(FileCreateArray, con, type, nbelem)
}

boolean OCI_API OCI_FileArrayFree
(
    OCI_File** files
)
{
    CALL_IMPL(FileFreeArray, files)
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
    OCI_File   * file,
    big_uint     offset,
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
    OCI_File   * file,
    void       * buffer,
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
    OCI_File   * file,
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
    CALL_IMPL(HandleGetReference, ref)
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
    const otext  * key,
    const otext  * value
)
{
    CALL_IMPL(HashAddString, table, key, value)
}

const otext* OCI_API OCI_HashGetString
(
    OCI_HashTable* table,
    const otext  * key
)
{
    CALL_IMPL(HashGetString, table, key)
}

boolean OCI_API OCI_HashAddInt
(
    OCI_HashTable* table,
    const otext  * key,
    int            value
)
{
    CALL_IMPL(HashAddInt, table, key, value)
}

int OCI_API OCI_HashGetInt
(
    OCI_HashTable* table,
    const otext  * key
)
{
    CALL_IMPL(HashGetInt, table, key)
}

boolean OCI_API OCI_HashAddPointer
(
    OCI_HashTable* table,
    const otext  * key,
    void         * value
)
{
    CALL_IMPL(HashAddPointer, table, key, value)
}

void* OCI_API OCI_HashGetPointer
(
    OCI_HashTable* table,
    const otext  * key
)
{
    CALL_IMPL(HashGetPointer, table, key)
}

OCI_HashEntry* OCI_API OCI_HashLookup
(
    OCI_HashTable* table,
    const otext  * key,
    boolean        create
)
{
    CALL_IMPL(HashLookup, table, key, create)
}

OCI_HashValue* OCI_API OCI_HashGetValue
(
    OCI_HashTable* table,
    const otext  * key
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
    CALL_IMPL(IntervalCreateArray, con, type, nbelem)
}

boolean OCI_API OCI_IntervalArrayFree
(
    OCI_Interval** itvs
)
{
    CALL_IMPL(IntervalFreeArray, itvs)
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
    const otext * str
)
{
    CALL_IMPL(IntervalFromString, itv, str)
}

boolean OCI_API OCI_IntervalToText
(
    OCI_Interval* itv,
    int           leading_prec,
    int           fraction_prec,
    int           size,
    otext       * str
)
{
    CALL_IMPL(IntervalToString, itv, leading_prec, fraction_prec, size, str)
}

boolean OCI_API OCI_IntervalFromTimeZone
(
    OCI_Interval* itv,
    const otext * str
)
{
    CALL_IMPL(IntervalFromTimeZone, itv, str)
}

boolean OCI_API OCI_IntervalGetDaySecond
(
    OCI_Interval* itv,
    int         * day,
    int         * hour,
    int         * min,
    int         * sec,
    int         * fsec
)
{
    CALL_IMPL(IntervalGetDaySecond, itv, day, hour, min, sec, fsec)
}

boolean OCI_API OCI_IntervalGetYearMonth
(
    OCI_Interval* itv,
    int         * year,
    int         * month
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
    CALL_IMPL(IteratorCreate, coll)
}

boolean OCI_API OCI_IterFree
(
    OCI_Iter* iter
)
{
    CALL_IMPL(IteratorFree, iter)
}

OCI_Elem* OCI_API OCI_IterGetNext
(
    OCI_Iter* iter
)
{
    CALL_IMPL(IteratorGetNext, iter)
}

OCI_Elem* OCI_API OCI_IterGetPrev
(
    OCI_Iter* iter
)
{
    CALL_IMPL(IteratorGetPrev, iter)
}

OCI_Elem* OCI_API OCI_IterGetCurrent
(
    OCI_Iter* iter
)
{
    CALL_IMPL(IteratorGetCurrent, iter)
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
    CALL_IMPL(EnvironmentInitialize, err_handler, lib_path, mode)
}

boolean OCI_API OCI_Cleanup
(
    void
)
{
    CALL_IMPL(EnvironmentCleanup)
}

unsigned int OCI_API OCI_GetOCICompileVersion
(
    void
)
{
    CALL_IMPL(EnvironmentGetOCICompileVersion)
}

unsigned int OCI_API OCI_GetOCIRuntimeVersion
(
    void
)
{
    CALL_IMPL(EnvironmentGetOCIRuntimeVersion)
}

unsigned int OCI_API OCI_GetImportMode
(
    void
)
{
    CALL_IMPL(EnvironmentGetOCIRuntimeImportMode)
}

unsigned int OCI_API OCI_GetCharset
(
    void
)
{
    CALL_IMPL(EnvironmentGetCharset)
}

big_uint OCI_API OCI_GetAllocatedBytes
(
    unsigned int mem_type
)
{
    CALL_IMPL(EnvironmentGetAllocatedBytes, mem_type)
}

OCI_Error* OCI_API OCI_GetLastError
(
    void
)
{
    return EnvironmentGetLastError();
}

boolean OCI_API OCI_EnableWarnings
(
    boolean value
)
{
    CALL_IMPL(EnvironmentEnableWarnings, value)
}

boolean OCI_API OCI_SetErrorHandler
(
    POCI_ERROR handler
)
{
    CALL_IMPL(EnvironmentSetErrorHandler, handler)
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
    POCI_HA_HANDLER handler
)
{
    CALL_IMPL(EnvironmentSetHAHandler, handler)
}

boolean OCI_API OCI_SetFormat
(
    OCI_Connection* con,
    unsigned int    type,
    const otext   * format
)
{
    CALL_IMPL(EnvironmentSetFormat, con, type, format)
}

const otext* OCI_API OCI_GetFormat
(
    OCI_Connection* con,
    unsigned int    type
)
{
    CALL_IMPL(EnvironmentGetFormat, con, type)
}

boolean OCI_API OCI_SetUserPassword
(
    const otext* db,
    const otext* user,
    const otext* pwd,
    const otext* new_pwd
)
{
    CALL_IMPL(DatabaseSetUserPassword, db, user, pwd, new_pwd);
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
    CALL_IMPL(LobCreateArray, con, type, nbelem);
}

boolean OCI_API OCI_LobArrayFree
(
    OCI_Lob** lobs
)
{
    CALL_IMPL(LobFreeArray, lobs);
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
    OCI_Lob    * lob,
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
    OCI_Lob    * lob,
    void       * buffer,
    unsigned int len
)
{
    CALL_IMPL(LobRead, lob, buffer, len);
}

boolean OCI_API OCI_LobRead2
(
    OCI_Lob     * lob,
    void        * buffer,
    unsigned int* char_count,
    unsigned int* byte_count
)
{
    CALL_IMPL(LobRead2, lob, buffer, char_count, byte_count);
}

unsigned int OCI_API OCI_LobWrite
(
    OCI_Lob    * lob,
    void       * buffer,
    unsigned int len
)
{
    CALL_IMPL(LobWrite, lob, buffer, len);
}

boolean OCI_API OCI_LobWrite2
(
    OCI_Lob     * lob,
    void        * buffer,
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
    OCI_Lob    * lob,
    void       * buffer,
    unsigned int len
)
{
    CALL_IMPL(LobAppend, lob, buffer, len);
}

boolean OCI_API OCI_LobAppend2
(
    OCI_Lob     * lob,
    void        * buffer,
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
    OCI_Lob * lob,
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
    OCI_Lob    * lob,
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
    OCI_Long   * lg,
    void       * buffer,
    unsigned int len
)
{
    CALL_IMPL(LongRead, lg, buffer, len);
}

unsigned int OCI_API OCI_LongWrite
(
    OCI_Long   * lg,
    void       * buffer,
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
    CALL_IMPL(MessageCreate, typinf);
}

boolean OCI_API OCI_MsgFree
(
    OCI_Msg* msg
)
{
    CALL_IMPL(MessageFree, msg);
}

boolean OCI_API OCI_MsgReset
(
    OCI_Msg* msg
)
{
    CALL_IMPL(MessageReset, msg);
}

OCI_Object* OCI_API OCI_MsgGetObject
(
    OCI_Msg* msg
)
{
    CALL_IMPL(MessageGetObject, msg);
}

boolean OCI_API OCI_MsgSetObject
(
    OCI_Msg   * msg,
    OCI_Object* obj
)
{
    CALL_IMPL(MessageSetObject, msg, obj);
}

boolean OCI_API OCI_MsgGetRaw
(
    OCI_Msg     * msg,
    void        * raw,
    unsigned int* size
)
{
    CALL_IMPL(MessageGetRaw, msg, raw, size);
}

boolean OCI_API OCI_MsgSetRaw
(
    OCI_Msg    * msg,
    const void * raw,
    unsigned int size
)
{
    CALL_IMPL(MessageSetRaw, msg, raw, size);
}

int OCI_API OCI_MsgGetAttemptCount
(
    OCI_Msg* msg
)
{
    CALL_IMPL(MessageGetAttemptCount, msg);
}

int OCI_API OCI_MsgGetEnqueueDelay
(
    OCI_Msg* msg
)
{
    CALL_IMPL(MessageGetEnqueueDelay, msg);
}

boolean OCI_API OCI_MsgSetEnqueueDelay
(
    OCI_Msg* msg,
    int      value
)
{
    CALL_IMPL(MessageSetEnqueueDelay, msg, value);
}

OCI_Date* OCI_API OCI_MsgGetEnqueueTime
(
    OCI_Msg* msg
)
{
    CALL_IMPL(MessageGetEnqueueTime, msg);
}

int OCI_API OCI_MsgGetExpiration
(
    OCI_Msg* msg
)
{
    CALL_IMPL(MessageGetExpiration, msg);
}

boolean OCI_API OCI_MsgSetExpiration
(
    OCI_Msg* msg,
    int      value
)
{
    CALL_IMPL(MessageSetExpiration, msg, value);
}

unsigned int OCI_API OCI_MsgGetState
(
    OCI_Msg* msg
)
{
    CALL_IMPL(MessageGetState, msg);
}

int OCI_API OCI_MsgGetPriority
(
    OCI_Msg* msg
)
{
    CALL_IMPL(MessageGetPriority, msg);
}

boolean OCI_API OCI_MsgSetPriority
(
    OCI_Msg* msg,
    int      value
)
{
    CALL_IMPL(MessageSetPriority, msg, value);
}

boolean OCI_API OCI_MsgGetID
(
    OCI_Msg     * msg,
    void        * id,
    unsigned int* len
)
{
    CALL_IMPL(MessageGetID, msg, id, len);
}

boolean OCI_API OCI_MsgGetOriginalID
(
    OCI_Msg     * msg,
    void        * id,
    unsigned int* len
)
{
    CALL_IMPL(MessageGetOriginalID, msg, id, len);
}

boolean OCI_API OCI_MsgSetOriginalID
(
    OCI_Msg    * msg,
    const void * id,
    unsigned int len
)
{
    CALL_IMPL(MessageSetOriginalID, msg, id, len);
}

OCI_Agent* OCI_API OCI_MsgGetSender
(
    OCI_Msg* msg
)
{
    CALL_IMPL(MessageGetSender, msg);
}

boolean OCI_API OCI_MsgSetSender
(
    OCI_Msg  * msg,
    OCI_Agent* sender
)
{
    CALL_IMPL(MessageSetSender, msg, sender);
}

boolean OCI_API OCI_MsgSetConsumers
(
    OCI_Msg    * msg,
    OCI_Agent ** consumers,
    unsigned int count
)
{
    CALL_IMPL(MessageSetConsumers, msg, consumers, count);
}

const otext* OCI_API OCI_MsgGetCorrelation
(
    OCI_Msg* msg
)
{
    CALL_IMPL(MessageGetCorrelation, msg);
}

boolean OCI_API OCI_MsgSetCorrelation
(
    OCI_Msg    * msg,
    const otext* correlation
)
{
    CALL_IMPL(MessageSetCorrelation, msg, correlation);
}

const otext* OCI_API OCI_MsgGetExceptionQueue
(
    OCI_Msg* msg
)
{
    CALL_IMPL(MessageGetExceptionQueue, msg);
}

boolean OCI_API OCI_MsgSetExceptionQueue
(
    OCI_Msg    * msg,
    const otext* queue
)
{
    CALL_IMPL(MessageSetExceptionQueue, msg, queue);
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
    CALL_IMPL(NumberCreateArray, con, nbelem);
}

boolean OCI_API OCI_NumberArrayFree
(
    OCI_Number** numbmers
)
{
    CALL_IMPL(NumberFreeArray, numbmers);
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
    OCI_Number * number,
    const otext* fmt,
    int          size,
    otext      * str
)
{
    CALL_IMPL(NumberToString, number, fmt, size, str);
}

boolean OCI_API OCI_NumberFromText
(
    OCI_Number * number,
    const otext* str,
    const otext* fmt
)
{
    CALL_IMPL(NumberFromString, number, str, fmt);
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
    OCI_Number   * number,
    unsigned char* content
)
{
    CALL_IMPL(NumberSetContent, number, content);
}

boolean OCI_API OCI_NumberSetValue
(
    OCI_Number * number,
    unsigned int type,
    void       * value
)
{
    CALL_IMPL(NumberSetValue, number, type, value);
}

boolean OCI_API OCI_NumberGetValue
(
    OCI_Number * number,
    unsigned int type,
    void       * value
)
{
    CALL_IMPL(NumberGetValue, number, type, value);
}

boolean OCI_API OCI_NumberAdd
(
    OCI_Number * number,
    unsigned int type,
    void       * value
)
{
    CALL_IMPL(NumberAdd, number, type, value);
}

boolean OCI_API OCI_NumberSub
(
    OCI_Number * number,
    unsigned int type,
    void       * value
)
{
    CALL_IMPL(NumberSub, number, type, value);
}

boolean OCI_API OCI_NumberMultiply
(
    OCI_Number * number,
    unsigned int type,
    void       * value
)
{
    CALL_IMPL(NumberMultiply, number, type, value);
}

boolean OCI_API OCI_NumberDivide
(
    OCI_Number * number,
    unsigned int type,
    void       * value
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
 *  object
 * --------------------------------------------------------------------------------------------- */

OCI_Object* OCI_API OCI_ObjectCreate
(
    OCI_Connection* con,
    OCI_TypeInfo  * typinf
)
{
    CALL_IMPL(ObjectCreate, con, typinf);
}

boolean OCI_API OCI_ObjectFree
(
    OCI_Object* obj
)
{
    CALL_IMPL(ObjectFree, obj);
}

OCI_Object** OCI_API OCI_ObjectArrayCreate
(
    OCI_Connection* con,
    OCI_TypeInfo  * typinf,
    unsigned int    nbelem
)
{
    CALL_IMPL(ObjectCreateArray, con,typinf, nbelem);
}

boolean OCI_API OCI_ObjectArrayFree
(
    OCI_Object** objs
)
{
    CALL_IMPL(ObjectFreeArray, objs);
}

boolean OCI_API OCI_ObjectAssign
(
    OCI_Object* obj,
    OCI_Object* obj_src
)
{
    CALL_IMPL(ObjectAssign, obj, obj_src);
}

unsigned int OCI_API OCI_ObjectGetType
(
    OCI_Object* obj
)
{
    CALL_IMPL(ObjectGetType, obj);
}

boolean OCI_API OCI_ObjectGetSelfRef
(
    OCI_Object* obj,
    OCI_Ref   * ref
)
{
    CALL_IMPL(ObjectGetSelfRef, obj, ref);
}

OCI_TypeInfo* OCI_API OCI_ObjectGetTypeInfo
(
    OCI_Object* obj
)
{
    CALL_IMPL(ObjectGetTypeInfo, obj);
}

boolean OCI_API OCI_ObjectGetBoolean
(
    OCI_Object * obj,
    const otext* attr
)
{
    CALL_IMPL(ObjectGetBoolean, obj, attr);
}

OCI_Number* OCI_API OCI_ObjectGetNumber
(
    OCI_Object * obj,
    const otext* attr
)
{
    CALL_IMPL(ObjectGetNumber, obj, attr);
}

short OCI_API OCI_ObjectGetShort
(
    OCI_Object * obj,
    const otext* attr
)
{
    CALL_IMPL(ObjectGetShort, obj, attr);
}

unsigned short OCI_API OCI_ObjectGetUnsignedShort
(
    OCI_Object * obj,
    const otext* attr
)
{
    CALL_IMPL(ObjectGetUnsignedShort, obj, attr);
}

int OCI_API OCI_ObjectGetInt
(
    OCI_Object * obj,
    const otext* attr
)
{
    CALL_IMPL(ObjectGetInt, obj, attr);
}

unsigned int OCI_API OCI_ObjectGetUnsignedInt
(
    OCI_Object * obj,
    const otext* attr
)
{
    CALL_IMPL(ObjectGetUnsignedInt, obj, attr);
}

big_int OCI_API OCI_ObjectGetBigInt
(
    OCI_Object * obj,
    const otext* attr
)
{
    CALL_IMPL(ObjectGetBigInt, obj, attr);
}

big_uint OCI_API OCI_ObjectGetUnsignedBigInt
(
    OCI_Object * obj,
    const otext* attr
)
{
    CALL_IMPL(ObjectGetUnsignedBigInt, obj, attr);
}

double OCI_API OCI_ObjectGetDouble
(
    OCI_Object * obj,
    const otext* attr
)
{
    CALL_IMPL(ObjectGetDouble, obj, attr);
}

float OCI_API OCI_ObjectGetFloat
(
    OCI_Object * obj,
    const otext* attr
)
{
    CALL_IMPL(ObjectGetFloat, obj, attr);
}

const otext* OCI_API OCI_ObjectGetString
(
    OCI_Object * obj,
    const otext* attr
)
{
    CALL_IMPL(ObjectGetString, obj, attr);
}

int OCI_API OCI_ObjectGetRaw
(
    OCI_Object * obj,
    const otext* attr,
    void       * value,
    unsigned int len
)
{
    CALL_IMPL(ObjectGetRaw, obj, attr, value, len);
}

unsigned int OCI_API OCI_ObjectGetRawSize
(
    OCI_Object * obj,
    const otext* attr
)
{
    CALL_IMPL(ObjectGetRawSize, obj, attr);
}

OCI_Date* OCI_API OCI_ObjectGetDate
(
    OCI_Object * obj,
    const otext* attr
)
{
    CALL_IMPL(ObjectGetDate, obj, attr);
}

OCI_Timestamp* OCI_API OCI_ObjectGetTimestamp
(
    OCI_Object * obj,
    const otext* attr
)
{
    CALL_IMPL(ObjectGetTimestamp, obj, attr);
}

OCI_Interval* OCI_API OCI_ObjectGetInterval
(
    OCI_Object * obj,
    const otext* attr
)
{
    CALL_IMPL(ObjectGetInterval, obj, attr);
}

OCI_Coll* OCI_API OCI_ObjectGetColl
(
    OCI_Object * obj,
    const otext* attr)
{
    CALL_IMPL(ObjectGetColl, obj, attr);
}

OCI_Ref* OCI_API OCI_ObjectGetRef
(
    OCI_Object * obj,
    const otext* attr
)
{
    CALL_IMPL(ObjectGetReference, obj, attr);
}

OCI_Object* OCI_API OCI_ObjectGetObject
(
    OCI_Object * obj,
    const otext* attr
)
{
    CALL_IMPL(ObjectGetObject, obj, attr);
}

OCI_Lob* OCI_API OCI_ObjectGetLob
(
    OCI_Object * obj,
    const otext* attr
)
{
    CALL_IMPL(ObjectGetLob, obj, attr);
}

OCI_File* OCI_API OCI_ObjectGetFile
(
    OCI_Object * obj,
    const otext* attr
)
{
    CALL_IMPL(ObjectGetFile, obj, attr);
}

boolean OCI_API OCI_ObjectSetBoolean
(
    OCI_Object * obj,
    const otext* attr,
    boolean      value
)
{
    CALL_IMPL(ObjectSetBoolean, obj, attr, value);
}

boolean OCI_API OCI_ObjectSetNumber
(
    OCI_Object * obj,
    const otext* attr,
    OCI_Number * value
)
{
    CALL_IMPL(ObjectSetNumber, obj, attr, value);
}

boolean OCI_API OCI_ObjectSetShort
(
    OCI_Object * obj,
    const otext* attr,
    short        value
)
{
    CALL_IMPL(ObjectSetShort, obj, attr, value);
}

boolean OCI_API OCI_ObjectSetUnsignedShort
(
    OCI_Object   * obj,
    const otext  * attr,
    unsigned short value
)
{
    CALL_IMPL(ObjectSetUnsignedShort, obj, attr, value);
}

boolean OCI_API OCI_ObjectSetInt
(
    OCI_Object * obj,
    const otext* attr,
    int          value
)
{
    CALL_IMPL(ObjectSetInt, obj, attr, value);
}

boolean OCI_API OCI_ObjectSetUnsignedInt
(
    OCI_Object * obj,
    const otext* attr,
    unsigned int value
)
{
    CALL_IMPL(ObjectSetUnsignedInt, obj, attr, value);
}

boolean OCI_API OCI_ObjectSetBigInt
(
    OCI_Object * obj,
    const otext* attr,
    big_int      value
)
{
    CALL_IMPL(ObjectSetBigInt, obj, attr, value);
}

boolean OCI_API OCI_ObjectSetUnsignedBigInt
(
    OCI_Object * obj,
    const otext* attr,
    big_uint     value
)
{
    CALL_IMPL(ObjectSetUnsignedBigInt, obj, attr, value);
}

boolean OCI_API OCI_ObjectSetDouble
(
    OCI_Object * obj,
    const otext* attr,
    double       value
)
{
    CALL_IMPL(ObjectSetDouble, obj, attr, value);
}

boolean OCI_API OCI_ObjectSetFloat
(
    OCI_Object * obj,
    const otext* attr,
    float        value
)
{
    CALL_IMPL(ObjectSetFloat, obj, attr, value);
}

boolean OCI_API OCI_ObjectSetString
(
    OCI_Object * obj,
    const otext* attr,
    const otext* value
)
{
    CALL_IMPL(ObjectSetString, obj, attr, value);
}

boolean OCI_API OCI_ObjectSetRaw
(
    OCI_Object * obj,
    const otext* attr,
    void       * value,
    unsigned int len
)
{
    CALL_IMPL(ObjectSetRaw, obj, attr, value, len);
}

boolean OCI_API OCI_ObjectSetDate
(
    OCI_Object * obj,
    const otext* attr,
    OCI_Date   * value
)
{
    CALL_IMPL(ObjectSetDate, obj, attr, value);
}

boolean OCI_API OCI_ObjectSetTimestamp
(
    OCI_Object   * obj,
    const otext  * attr,
    OCI_Timestamp* value
)
{
    CALL_IMPL(ObjectSetTimestamp, obj, attr, value);
}

boolean OCI_API OCI_ObjectSetInterval
(
    OCI_Object  * obj,
    const otext * attr,
    OCI_Interval* value
)
{
    CALL_IMPL(ObjectSetInterval, obj, attr, value);
}

boolean OCI_API OCI_ObjectSetColl
(
    OCI_Object * obj,
    const otext* attr,
    OCI_Coll   * value
)
{
    CALL_IMPL(ObjectSetColl, obj, attr, value);
}

boolean OCI_API OCI_ObjectSetObject
(
    OCI_Object * obj,
    const otext* attr,
    OCI_Object * value
)
{
    CALL_IMPL(ObjectSetObject, obj, attr, value);
}

boolean OCI_API OCI_ObjectSetLob
(
    OCI_Object * obj,
    const otext* attr,
    OCI_Lob    * value
)
{
    CALL_IMPL(ObjectSetLob, obj, attr, value);
}

boolean OCI_API OCI_ObjectSetFile
(
    OCI_Object * obj,
    const otext* attr,
    OCI_File   * value
)
{
    CALL_IMPL(ObjectSetFile, obj, attr, value);
}

boolean OCI_API OCI_ObjectSetRef
(
    OCI_Object * obj,
    const otext* attr,
    OCI_Ref    * value
)
{
    CALL_IMPL(ObjectSetReference, obj, attr, value);
}

boolean OCI_API OCI_ObjectIsNull
(
    OCI_Object * obj,
    const otext* attr
)
{
    CALL_IMPL(ObjectIsNull, obj, attr);
}

boolean OCI_API OCI_ObjectSetNull
(
    OCI_Object * obj,
    const otext* attr
)
{
    CALL_IMPL(ObjectSetNull, obj, attr);
}

boolean OCI_API OCI_ObjectGetStruct
(
    OCI_Object* obj,
    void     ** pp_struct,
    void     ** pp_ind
)
{
    CALL_IMPL(ObjectGetStruct, obj, pp_struct, pp_ind);
}

boolean OCI_API OCI_ObjectToText
(
    OCI_Object  * obj,
    unsigned int* size,
    otext       * str
)
{
    CALL_IMPL(ObjectToString, obj, size, str);
}

/* --------------------------------------------------------------------------------------------- *
 *  pool
 * --------------------------------------------------------------------------------------------- */

OCI_Pool* OCI_API OCI_PoolCreate
(
    const otext* db,
    const otext* user,
    const otext* pwd,
    unsigned int type,
    unsigned int mode,
    unsigned int min_con,
    unsigned int max_con,
    unsigned int incr_con
)
{
    CALL_IMPL(PoolCreate, db, user, pwd, type, mode, min_con, max_con, incr_con);
}

boolean OCI_API OCI_PoolFree
(
    OCI_Pool* pool
)
{
    CALL_IMPL(PoolFree, pool);
}

OCI_Connection* OCI_API OCI_PoolGetConnection
(
    OCI_Pool   * pool,
    const otext* tag
)
{
    CALL_IMPL(PoolGetConnection, pool, tag);
}

unsigned int OCI_API OCI_PoolGetTimeout
(
    OCI_Pool* pool
)
{
    CALL_IMPL(PoolGetTimeout, pool);
}

boolean OCI_API OCI_PoolSetTimeout
(
    OCI_Pool   * pool,
    unsigned int value
)
{
    CALL_IMPL(PoolSetTimeout, pool, value);
}

boolean OCI_API OCI_PoolGetNoWait
(
    OCI_Pool* pool
)
{
    CALL_IMPL(PoolGetNoWait, pool);
}

boolean OCI_API OCI_PoolSetNoWait
(
    OCI_Pool* pool,
    boolean   value
)
{
    CALL_IMPL(PoolSetNoWait, pool, value);
}

unsigned int OCI_API OCI_PoolGetBusyCount
(
    OCI_Pool* pool
)
{
    CALL_IMPL(PoolGetBusyCount, pool);
}

unsigned int OCI_API OCI_PoolGetOpenedCount
(
    OCI_Pool* pool
)
{
    CALL_IMPL(PoolGetOpenedCount, pool);
}

unsigned int OCI_API OCI_PoolGetMin
(
    OCI_Pool* pool
)
{
    CALL_IMPL(PoolGetMin, pool);
}

unsigned int OCI_API OCI_PoolGetMax
(
    OCI_Pool* pool
)
{
    CALL_IMPL(PoolGetMax, pool);
}

unsigned int OCI_API OCI_PoolGetIncrement
(
    OCI_Pool* pool
)
{
    CALL_IMPL(PoolGetIncrement, pool);
}

unsigned int OCI_API OCI_PoolGetStatementCacheSize
(
    OCI_Pool* pool
)
{
    CALL_IMPL(PoolGetStatementCacheSize, pool);
}

boolean OCI_API OCI_PoolSetStatementCacheSize
(
    OCI_Pool   * pool,
    unsigned int value
)
{
    CALL_IMPL(PoolSetStatementCacheSize, pool, value);
}

/* --------------------------------------------------------------------------------------------- *
 *  queue
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_QueueCreate
(
    OCI_Connection* con,
    const otext   * queue_name,
    const otext   * queue_table,
    unsigned int    queue_type,
    unsigned int    max_retries,
    unsigned int    retry_delay,
    unsigned int    retention_time,
    boolean         dependency_tracking,
    const otext   * comment
)
{
    CALL_IMPL(QueueCreate, con, queue_name, queue_table, queue_type, max_retries,
              retry_delay, retention_time, dependency_tracking, comment);
}

boolean OCI_API OCI_QueueAlter
(
    OCI_Connection* con,
    const otext   * queue_name,
    unsigned int    max_retries,
    unsigned int    retry_delay,
    unsigned int    retention_time,
    const otext   * comment
)
{
    CALL_IMPL(QueueAlter, con, queue_name, max_retries, retry_delay, retention_time, comment);
}

boolean OCI_API OCI_QueueDrop
(
    OCI_Connection* con,
    const otext   * queue_name
)
{
    CALL_IMPL(QueueDrop, con, queue_name);
}

boolean OCI_API OCI_QueueStart
(
    OCI_Connection* con,
    const otext   * queue_name,
    boolean         enqueue,
    boolean         dequeue
)
{
    CALL_IMPL(QueueStart, con, queue_name, enqueue, dequeue);
}

boolean OCI_API OCI_QueueStop
(
    OCI_Connection* con,
    const otext   * queue_name,
    boolean         enqueue,
    boolean         dequeue,
    boolean         wait
)
{
    CALL_IMPL(QueueStop, con, queue_name, enqueue, dequeue, wait);
}

boolean OCI_API OCI_QueueTableCreate
(
    OCI_Connection* con,
    const otext   * queue_table,
    const otext   * queue_payload_type,
    const otext   * storage_clause,
    const otext   * sort_list,
    boolean         multiple_consumers,
    unsigned int    message_grouping,
    const otext   * comment,
    unsigned int    primary_instance,
    unsigned int    secondary_instance,
    const otext   * compatible
)
{
    CALL_IMPL(QueueTableCreate, con, queue_table, queue_payload_type, storage_clause,
              sort_list, multiple_consumers, message_grouping, comment, primary_instance,
              secondary_instance, compatible);
}

boolean OCI_API OCI_QueueTableAlter
(
    OCI_Connection* con,
    const otext   * queue_table,
    const otext   * comment,
    unsigned int    primary_instance,
    unsigned int    secondary_instance
)
{
    CALL_IMPL(QueueTableAlter, con, queue_table, comment, primary_instance, secondary_instance);
}

boolean OCI_API OCI_QueueTableDrop
(
    OCI_Connection* con,
    const otext   * queue_table,
    boolean         force
)
{
    CALL_IMPL(QueueTableDrop, con, queue_table, force);
}

boolean OCI_API OCI_QueueTablePurge
(
    OCI_Connection* con,
    const otext   * queue_table,
    const otext   * purge_condition,
    boolean         block,
    unsigned int    delivery_mode
)
{
    CALL_IMPL(QueueTablePurge, con, queue_table, purge_condition, block, delivery_mode);
}

boolean OCI_API OCI_QueueTableMigrate
(
    OCI_Connection* con,
    const otext   * queue_table,
    const otext   * compatible
)
{
    CALL_IMPL(QueueTableMigrate, con, queue_table, compatible);
}

/* --------------------------------------------------------------------------------------------- *
 *  ref
 * --------------------------------------------------------------------------------------------- */

OCI_Ref* OCI_API OCI_RefCreate
(
    OCI_Connection* con,
    OCI_TypeInfo  * typinf
)
{
    CALL_IMPL(ReferenceCreate, con, typinf);
}

boolean OCI_API OCI_RefFree
(
    OCI_Ref* ref
)
{
    CALL_IMPL(ReferenceFree, ref);
}

OCI_Ref** OCI_API OCI_RefArrayCreate
(
    OCI_Connection* con,
    OCI_TypeInfo  * typinf,
    unsigned int    nbelem
)
{
    CALL_IMPL(ReferenceCreateArray, con, typinf, nbelem);
}

boolean OCI_API OCI_RefArrayFree
(
    OCI_Ref** refs
)
{
    CALL_IMPL(ReferenceFreeArray, refs);
}

boolean OCI_API OCI_RefAssign
(
    OCI_Ref* ref,
    OCI_Ref* ref_src
)
{
    CALL_IMPL(ReferenceAssign, ref, ref_src);
}

OCI_TypeInfo* OCI_API OCI_RefGetTypeInfo
(
    OCI_Ref* ref
)
{
    CALL_IMPL(ReferenceGetTypeInfo, ref);
}

OCI_Object* OCI_API OCI_RefGetObject
(
    OCI_Ref* ref
)
{
    CALL_IMPL(ReferenceGetObject, ref);
}

boolean OCI_API OCI_RefIsNull
(
    OCI_Ref* ref
)
{
    CALL_IMPL(ReferenceIsNull, ref);
}

boolean OCI_API OCI_RefSetNull
(
    OCI_Ref* ref
)
{
    CALL_IMPL(ReferenceSetNull, ref);
}

unsigned int OCI_API OCI_RefGetHexSize
(
    OCI_Ref* ref
)
{
    CALL_IMPL(ReferenceGetHexSize, ref);
}

boolean OCI_API OCI_RefToText
(
    OCI_Ref    * ref,
    unsigned int size,
    otext      * str
)
{
    CALL_IMPL(ReferenceToString, ref, size, str);
}

/* --------------------------------------------------------------------------------------------- *
 *  resultset
 * --------------------------------------------------------------------------------------------- */

OCI_Resultset* OCI_API OCI_GetResultset
(
    OCI_Statement* stmt
)
{
    CALL_IMPL(StatementGetResultset, stmt);
}

OCI_Resultset* OCI_API OCI_GetNextResultset
(
    OCI_Statement* stmt
)
{
    CALL_IMPL(StatementGetNextResultset, stmt);
}

boolean OCI_API OCI_FetchPrev
(
    OCI_Resultset* rs
)
{
    CALL_IMPL(ResultsetFetchPrev, rs);
}

boolean OCI_API OCI_FetchNext
(
    OCI_Resultset* rs
)
{
    CALL_IMPL(ResultsetFetchNext, rs);
}

boolean OCI_API OCI_FetchFirst
(
    OCI_Resultset* rs
)
{
    CALL_IMPL(ResultsetFetchFirst, rs);
}

boolean OCI_API OCI_FetchLast
(
    OCI_Resultset* rs
)
{
    CALL_IMPL(ResultsetFetchLast, rs);
}

boolean OCI_API OCI_FetchSeek
(
    OCI_Resultset* rs,
    unsigned int   mode,
    int            offset
)
{
    CALL_IMPL(ResultsetFetchSeek, rs, mode, offset);
}

unsigned int OCI_API OCI_GetRowCount
(
    OCI_Resultset* rs
)
{
    CALL_IMPL(ResultsetGetRowCount, rs);
}

unsigned int OCI_API OCI_GetCurrentRow
(
    OCI_Resultset* rs
)
{
    CALL_IMPL(ResultsetGetCurrentRow, rs);
}

unsigned int OCI_API OCI_GetColumnCount
(
    OCI_Resultset* rs
)
{
    CALL_IMPL(ResultsetGetColumnCount, rs);
}

OCI_Column* OCI_API OCI_GetColumn
(
    OCI_Resultset* rs,
    unsigned int   index
)
{
    CALL_IMPL(ResultsetGetColumn, rs, index);
}

OCI_Column* OCI_API OCI_GetColumn2
(
    OCI_Resultset* rs,
    const otext  * name
)
{
    CALL_IMPL(ResultsetGetColumn2, rs, name);
}

unsigned int OCI_API OCI_GetColumnIndex
(
    OCI_Resultset* rs,
    const otext  * name
)
{
    CALL_IMPL(ResultsetGetColumnIndex, rs, name);
}

boolean OCI_API OCI_SetStructNumericType
(
    OCI_Resultset* rs,
    unsigned int   index,
    unsigned int   type
)
{
    CALL_IMPL(ResultsetSetStructNumericType, rs, index, type);
}

boolean OCI_API OCI_SetStructNumericType2
(
    OCI_Resultset* rs,
    const otext  * name,
    unsigned int   type
)
{
    CALL_IMPL(ResultsetSetStructNumericType2, rs, name, type);
}

boolean OCI_API OCI_GetStruct
(
    OCI_Resultset* rs,
    void         * row_struct,
    void         * row_struct_ind
)
{
    CALL_IMPL(ResultsetGetStruct, rs, row_struct, row_struct_ind);
}

OCI_Number* OCI_API OCI_GetNumber
(
    OCI_Resultset* rs,
    unsigned int   index
)
{
    CALL_IMPL(ResultsetGetNumber, rs, index);
}

OCI_Number* OCI_API OCI_GetNumber2
(
    OCI_Resultset* rs,
    const otext  * name
)
{
    CALL_IMPL(ResultsetGetNumber2, rs, name);
}

short OCI_API OCI_GetShort
(
    OCI_Resultset* rs,
    unsigned int   index
)
{
    CALL_IMPL(ResultsetGetShort, rs, index);
}

short OCI_API OCI_GetShort2
(
    OCI_Resultset* rs,
    const otext  * name
)
{
    CALL_IMPL(ResultsetGetShort2, rs, name);
}

unsigned short OCI_API OCI_GetUnsignedShort
(
    OCI_Resultset* rs,
    unsigned int   index
)
{
    CALL_IMPL(ResultsetGetUnsignedShort, rs, index);
}

unsigned short OCI_API OCI_GetUnsignedShort2
(
    OCI_Resultset* rs,
    const otext  * name
)
{
    CALL_IMPL(ResultsetGetUnsignedShort2, rs, name);
}

int OCI_API OCI_GetInt
(
    OCI_Resultset* rs,
    unsigned int   index
)
{
    CALL_IMPL(ResultsetGetInt, rs, index);
}

int OCI_API OCI_GetInt2
(
    OCI_Resultset* rs,
    const otext  * name
)
{
    CALL_IMPL(ResultsetGetInt2, rs, name);
}

unsigned int OCI_API OCI_GetUnsignedInt
(
    OCI_Resultset* rs,
    unsigned int   index
)
{
    CALL_IMPL(ResultsetGetUnsignedInt, rs, index);
}

unsigned int OCI_API OCI_GetUnsignedInt2
(
    OCI_Resultset* rs,
    const otext  * name
)
{
    CALL_IMPL(ResultsetGetUnsignedInt2, rs, name);
}

big_int OCI_API OCI_GetBigInt
(
    OCI_Resultset* rs,
    unsigned int   index
)
{
    CALL_IMPL(ResultsetGetBigInt, rs, index);
}

big_int OCI_API OCI_GetBigInt2
(
    OCI_Resultset* rs,
    const otext  * name
)
{
    CALL_IMPL(ResultsetGetBigInt2, rs, name);
}

big_uint OCI_API OCI_GetUnsignedBigInt
(
    OCI_Resultset* rs,
    unsigned int   index
)
{
    CALL_IMPL(ResultsetGetUnsignedBigInt, rs, index);
}

big_uint OCI_API OCI_GetUnsignedBigInt2
(
    OCI_Resultset* rs,
    const otext  * name
)
{
    CALL_IMPL(ResultsetGetUnsignedBigInt2, rs, name);
}

const otext* OCI_API OCI_GetString
(
    OCI_Resultset* rs,
    unsigned int   index
)
{
    CALL_IMPL(ResultsetGetString, rs, index);
}

const otext* OCI_API OCI_GetString2
(
    OCI_Resultset* rs,
    const otext  * name
)
{
    CALL_IMPL(ResultsetGetString2, rs, name);
}

unsigned int OCI_API OCI_GetRaw
(
    OCI_Resultset* rs,
    unsigned int   index,
    void         * buffer,
    unsigned int   len
)
{
    CALL_IMPL(ResultsetGetRaw, rs, index, buffer,len);
}

unsigned int OCI_API OCI_GetRaw2
(
    OCI_Resultset* rs,
    const otext  * name,
    void         * buffer,
    unsigned int   len
)
{
    CALL_IMPL(ResultsetGetRaw2, rs, name, buffer, len);
}

double OCI_API OCI_GetDouble
(
    OCI_Resultset* rs,
    unsigned int   index
)
{
    CALL_IMPL(ResultsetGetDouble, rs, index);
}

double OCI_API OCI_GetDouble2
(
    OCI_Resultset* rs,
    const otext  * name
)
{
    CALL_IMPL(ResultsetGetDouble2, rs, name);
}

float OCI_API OCI_GetFloat
(
    OCI_Resultset* rs,
    unsigned int   index
)
{
    CALL_IMPL(ResultsetGetFloat, rs, index);
}

float OCI_API OCI_GetFloat2
(
    OCI_Resultset* rs,
    const otext  * name
)
{
    CALL_IMPL(ResultsetGetFloat2, rs, name);
}

OCI_Date* OCI_API OCI_GetDate
(
    OCI_Resultset* rs,
    unsigned int   index
)
{
    CALL_IMPL(ResultsetGetDate, rs, index);
}

OCI_Date* OCI_API OCI_GetDate2
(
    OCI_Resultset* rs,
    const otext  * name
)
{
    CALL_IMPL(ResultsetGetDate2, rs, name);
}

OCI_Timestamp* OCI_API OCI_GetTimestamp
(
    OCI_Resultset* rs,
    unsigned int   index
)
{
    CALL_IMPL(ResultsetGetTimestamp, rs, index);
}

OCI_Timestamp* OCI_API OCI_GetTimestamp2
(
    OCI_Resultset* rs,
    const otext  * name
)
{
    CALL_IMPL(ResultsetGetTimestamp2, rs, name);
}

OCI_Interval* OCI_API OCI_GetInterval
(
    OCI_Resultset* rs,
    unsigned int   index
)
{
    CALL_IMPL(ResultsetGetInterval, rs, index);
}

OCI_Interval* OCI_API OCI_GetInterval2
(
    OCI_Resultset* rs,
    const otext  * name
)
{
    CALL_IMPL(ResultsetGetInterval2, rs, name);
}

OCI_Object* OCI_API OCI_GetObject
(
    OCI_Resultset* rs,
    unsigned int   index
)
{
    CALL_IMPL(ResultsetGetObject, rs, index);
}

OCI_Object* OCI_API OCI_GetObject2
(
    OCI_Resultset* rs,
    const otext  * name
)
{
    CALL_IMPL(ResultsetGetObject2, rs, name);
}

OCI_Coll* OCI_API OCI_GetColl
(
    OCI_Resultset* rs,
    unsigned int   index
)
{
    CALL_IMPL(ResultsetGetColl, rs, index);
}

OCI_Coll* OCI_API OCI_GetColl2
(
    OCI_Resultset* rs,
    const otext  * name
)
{
    CALL_IMPL(ResultsetGetColl2, rs, name);
}

OCI_Ref* OCI_API OCI_GetRef
(
    OCI_Resultset* rs,
    unsigned int   index
)
{
    CALL_IMPL(ResultsetGetReference, rs, index);
}

OCI_Ref* OCI_API OCI_GetRef2
(
    OCI_Resultset* rs,
    const otext  * name
)
{
    CALL_IMPL(ResultsetGetReference2, rs, name);
}

OCI_Statement* OCI_API OCI_GetStatement
(
    OCI_Resultset* rs,
    unsigned int   index
)
{
    CALL_IMPL(ResultsetGetStatement, rs, index);
}

OCI_Statement* OCI_API OCI_GetStatement2
(
    OCI_Resultset* rs,
    const otext  * name
)
{
    CALL_IMPL(ResultsetGetStatement2, rs, name);
}

OCI_Lob* OCI_API OCI_GetLob
(
    OCI_Resultset* rs,
    unsigned int   index
)
{
    CALL_IMPL(ResultsetGetLob, rs, index);
}

OCI_Lob* OCI_API OCI_GetLob2
(
    OCI_Resultset* rs,
    const otext  * name
)
{
    CALL_IMPL(ResultsetGetLob2, rs, name);
}

OCI_File* OCI_API OCI_GetFile
(
    OCI_Resultset* rs,
    unsigned int   index
)
{
    CALL_IMPL(ResultsetGetFile, rs, index);
}

OCI_File* OCI_API OCI_GetFile2
(
    OCI_Resultset* rs,
    const otext  * name
)
{
    CALL_IMPL(ResultsetGetFile2, rs, name);
}

OCI_Long* OCI_API OCI_GetLong
(
    OCI_Resultset* rs,
    unsigned int   index
)
{
    CALL_IMPL(ResultsetGetLong, rs, index);
}

OCI_Long* OCI_API OCI_GetLong2
(
    OCI_Resultset* rs,
    const otext  * name
)
{
    CALL_IMPL(ResultsetGetLong2, rs, name);
}

unsigned int OCI_API OCI_GetDataSize
(
    OCI_Resultset* rs,
    unsigned int   index
)
{
    CALL_IMPL(ResultsetGetDataSize, rs, index);
}

unsigned int OCI_API OCI_GetDataSize2
(
    OCI_Resultset* rs,
    const otext  * name
)
{
    CALL_IMPL(ResultsetGetDataSize2, rs, name);
}

boolean OCI_API OCI_IsNull
(
    OCI_Resultset* rs,
    unsigned int   index
)
{
    CALL_IMPL(ResultsetIsNull, rs, index);
}

boolean OCI_API OCI_IsNull2
(
    OCI_Resultset* rs,
    const otext  * name
)
{
    CALL_IMPL(ResultsetIsNull2, rs, name);
}

OCI_Statement* OCI_API OCI_ResultsetGetStatement
(
    OCI_Resultset* rs
)
{
    CALL_IMPL(ResultsetResultsetGetStatement, rs);
}

unsigned int OCI_API OCI_GetDataLength
(
    OCI_Resultset* rs,
    unsigned int   index
)
{
    CALL_IMPL(ResultsetGetDataLength, rs, index);
}

/* --------------------------------------------------------------------------------------------- *
 *  statement
 * --------------------------------------------------------------------------------------------- */

OCI_Statement* OCI_API OCI_StatementCreate
(
    OCI_Connection* con
)
{
    CALL_IMPL(StatementCreate, con);
}

boolean OCI_API OCI_StatementFree
(
    OCI_Statement* stmt
)
{
    CALL_IMPL(StatementFree, stmt);
}

boolean OCI_API OCI_ReleaseResultsets
(
    OCI_Statement* stmt
)
{
    CALL_IMPL(StatementReleaseResultsets, stmt);
}

boolean OCI_API OCI_Prepare
(
    OCI_Statement* stmt,
    const otext  * sql
)
{
    CALL_IMPL(StatementPrepare, stmt, sql);
}

boolean OCI_API OCI_Execute
(
    OCI_Statement* stmt
)
{
    CALL_IMPL(StatementExecute, stmt);
}

boolean OCI_API OCI_ExecuteStmt
(
    OCI_Statement* stmt,
    const otext  * sql
)
{
    CALL_IMPL(StatementExecuteStmt, stmt, sql);
}

boolean OCI_API OCI_Parse
(
    OCI_Statement* stmt,
    const otext  * sql
)
{
    CALL_IMPL(StatementParse, stmt, sql);
}

boolean OCI_API OCI_Describe
(
    OCI_Statement* stmt,
    const otext  * sql
)
{
    CALL_IMPL(StatementDescribe, stmt, sql);
}

boolean OCI_PrepareFmt
(
    OCI_Statement* stmt,
    const otext  * sql,
    ...
)
{
    boolean res = FALSE;
    va_list args;

    va_start(args, sql);
    res = StatementPrepareFmt(stmt, sql, args);
    va_end(args);

    return res;
}

boolean OCI_ExecuteStmtFmt
(
    OCI_Statement* stmt,
    const otext  * sql,
    ...
)
{
    boolean res = FALSE;
    va_list args;

    va_start(args, sql);
    res = StatementExecuteStmtFmt(stmt, sql, args);
    va_end(args);

    return res;
}

boolean OCI_ParseFmt
(
    OCI_Statement* stmt,
    const otext  * sql,
    ...
)
{
    boolean res = FALSE;
    va_list args;

    va_start(args, sql);
    res = StatementParseFmt(stmt, sql, args);
    va_end(args);

    return res;
}

boolean OCI_DescribeFmt
(
    OCI_Statement* stmt,
    const otext  * sql,
    ...
)
{
    boolean res = FALSE;
    va_list args;

    va_start(args, sql);
    res = StatementDescribeFmt(stmt, sql, args);
    va_end(args);

    return res;
}

boolean OCI_API OCI_BindArraySetSize
(
    OCI_Statement* stmt,
    unsigned int   size
)
{
    CALL_IMPL(StatementSetBindArraySize, stmt, size);
}

unsigned int OCI_API OCI_BindArrayGetSize
(
    OCI_Statement* stmt
)
{
    CALL_IMPL(StatementGetBindArraySize, stmt);
}

boolean OCI_API OCI_AllowRebinding
(
    OCI_Statement* stmt,
    boolean        value
)
{
    CALL_IMPL(StatementAllowRebinding, stmt, value);
}

boolean OCI_API OCI_IsRebindingAllowed
(
    OCI_Statement* stmt
)
{
    CALL_IMPL(StatementIsRebindingAllowed, stmt);
}

boolean OCI_API OCI_BindBoolean
(
    OCI_Statement* stmt,
    const otext  * name,
    boolean      * data
)
{
    CALL_IMPL(StatementBindBoolean, stmt, name, data);
}

boolean OCI_API OCI_BindNumber
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Number   * data
)
{
    CALL_IMPL(StatementBindNumber, stmt, name, data);
}

boolean OCI_API OCI_BindArrayOfNumbers
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Number  ** data,
    unsigned int   nbelem
)
{
    CALL_IMPL(StatementBindArrayOfNumbers, stmt, name, data, nbelem);
}

boolean OCI_API OCI_BindShort
(
    OCI_Statement* stmt,
    const otext  * name,
    short        * data
)
{
    CALL_IMPL(StatementBindShort, stmt, name, data);
}

boolean OCI_API OCI_BindArrayOfShorts
(
    OCI_Statement* stmt,
    const otext  * name,
    short        * data,
    unsigned int   nbelem
)
{
    CALL_IMPL(StatementBindArrayOfShorts, stmt, name, data, nbelem);
}

boolean OCI_API OCI_BindUnsignedShort
(
    OCI_Statement * stmt,
    const otext   * name,
    unsigned short* data
)
{
    CALL_IMPL(StatementBindUnsignedShort, stmt, name, data);
}

boolean OCI_API OCI_BindArrayOfUnsignedShorts
(
    OCI_Statement * stmt,
    const otext   * name,
    unsigned short* data,
    unsigned int    nbelem
)
{
    CALL_IMPL(StatementBindArrayOfUnsignedShorts, stmt, name, data, nbelem);
}

boolean OCI_API OCI_BindInt
(
    OCI_Statement* stmt,
    const otext  * name,
    int          * data
)
{
    CALL_IMPL(StatementBindInt, stmt, name, data);
}

boolean OCI_API OCI_BindArrayOfInts
(
    OCI_Statement* stmt,
    const otext  * name,
    int          * data,
    unsigned int   nbelem
)
{
    CALL_IMPL(StatementBindArrayOfInts, stmt, name, data, nbelem);
}

boolean OCI_API OCI_BindUnsignedInt
(
    OCI_Statement* stmt,
    const otext  * name,
    unsigned int * data
)
{
    CALL_IMPL(StatementBindUnsignedInt, stmt, name, data);
}

boolean OCI_API OCI_BindArrayOfUnsignedInts
(
    OCI_Statement* stmt,
    const otext  * name,
    unsigned int * data,
    unsigned int   nbelem
)
{
    CALL_IMPL(StatementBindArrayOfUnsignedInts, stmt, name, data, nbelem);
}

boolean OCI_API OCI_BindBigInt
(
    OCI_Statement* stmt,
    const otext  * name,
    big_int      * data
)
{
    CALL_IMPL(StatementBindBigInt, stmt, name, data);
}

boolean OCI_API OCI_BindArrayOfBigInts
(
    OCI_Statement* stmt,
    const otext  * name,
    big_int      * data,
    unsigned int   nbelem
)
{
    CALL_IMPL(StatementBindArrayOfBigInts, stmt, name, data, nbelem);
}

boolean OCI_API OCI_BindUnsignedBigInt
(
    OCI_Statement* stmt,
    const otext  * name,
    big_uint     * data
)
{
    CALL_IMPL(StatementBindUnsignedBigInt, stmt, name, data);
}

boolean OCI_API OCI_BindArrayOfUnsignedBigInts
(
    OCI_Statement* stmt,
    const otext  * name,
    big_uint     * data,
    unsigned int   nbelem
)
{
    CALL_IMPL(StatementBindArrayOfUnsignedBigInts, stmt, name, data, nbelem);
}

boolean OCI_API OCI_BindString
(
    OCI_Statement* stmt,
    const otext  * name,
    otext        * data,
    unsigned int   len
)
{
    CALL_IMPL(StatementBindString, stmt, name, data, len);
}

boolean OCI_API OCI_BindArrayOfStrings
(
    OCI_Statement* stmt,
    const otext  * name,
    otext        * data,
    unsigned int   len,
    unsigned int   nbelem
)
{
    CALL_IMPL(StatementBindArrayOfStrings, stmt, name, data, len, nbelem);
}

boolean OCI_API OCI_BindRaw
(
    OCI_Statement* stmt,
    const otext  * name,
    void         * data,
    unsigned int   len
)
{
    CALL_IMPL(StatementBindRaw, stmt, name, data, len);
}

boolean OCI_API OCI_BindArrayOfRaws
(
    OCI_Statement* stmt,
    const otext  * name,
    void         * data,
    unsigned int   len,
    unsigned int   nbelem
)
{
    CALL_IMPL(StatementBindArrayOfRaws, stmt, name, data, len, nbelem);
}

boolean OCI_API OCI_BindDouble
(
    OCI_Statement* stmt,
    const otext  * name,
    double       * data
)
{
    CALL_IMPL(StatementBindDouble, stmt, name, data);
}

boolean OCI_API OCI_BindArrayOfDoubles
(
    OCI_Statement* stmt,
    const otext  * name,
    double       * data,
    unsigned int   nbelem
)
{
    CALL_IMPL(StatementBindArrayOfDoubles, stmt, name, data, nbelem);
}

boolean OCI_API OCI_BindFloat
(
    OCI_Statement* stmt,
    const otext  * name,
    float        * data
)
{
    CALL_IMPL(StatementBindFloat, stmt, name, data);
}

boolean OCI_API OCI_BindArrayOfFloats
(
    OCI_Statement* stmt,
    const otext  * name,
    float        * data,
    unsigned int   nbelem
)
{
    CALL_IMPL(StatementBindArrayOfFloats, stmt, name, data, nbelem);
}

boolean OCI_API OCI_BindDate
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Date     * data
)
{
    CALL_IMPL(StatementBindDate, stmt, name, data);
}

boolean OCI_API OCI_BindArrayOfDates
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Date    ** data,
    unsigned int   nbelem
)
{
    CALL_IMPL(StatementBindArrayOfDates, stmt, name, data, nbelem);
}

boolean OCI_API OCI_BindTimestamp
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Timestamp* data
)
{
    CALL_IMPL(StatementBindTimestamp, stmt, name, data);
}

boolean OCI_API OCI_BindArrayOfTimestamps
(
    OCI_Statement * stmt,
    const otext   * name,
    OCI_Timestamp** data,
    unsigned int    type,
    unsigned int    nbelem
)
{
    CALL_IMPL(StatementBindArrayOfTimestamps, stmt, name, data, type, nbelem);
}

boolean OCI_API OCI_BindInterval
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Interval * data
)
{
    CALL_IMPL(StatementBindInterval, stmt, name, data);
}

boolean OCI_API OCI_BindArrayOfIntervals
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Interval** data,
    unsigned int   type,
    unsigned int   nbelem
)
{
    CALL_IMPL(StatementBindArrayOfIntervals, stmt, name, data, type, nbelem);
}

boolean OCI_API OCI_BindObject
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Object   * data
)
{
    CALL_IMPL(StatementBindObject, stmt, name, data);
}

boolean OCI_API OCI_BindArrayOfObjects
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Object  ** data,
    OCI_TypeInfo * typinf,
    unsigned int   nbelem
)
{
    CALL_IMPL(StatementBindArrayOfObjects, stmt, name, data, typinf, nbelem);
}

boolean OCI_API OCI_BindLob
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Lob      * data
)
{
    CALL_IMPL(StatementBindLob, stmt, name, data);
}

boolean OCI_API OCI_BindArrayOfLobs
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Lob     ** data,
    unsigned int   type,
    unsigned int   nbelem
)
{
    CALL_IMPL(StatementBindArrayOfLobs, stmt, name, data, type, nbelem);
}

boolean OCI_API OCI_BindFile
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_File     * data
)
{
    CALL_IMPL(StatementBindFile, stmt, name, data);
}

boolean OCI_API OCI_BindArrayOfFiles
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_File    ** data,
    unsigned int   type,
    unsigned int   nbelem
)
{
    CALL_IMPL(StatementBindArrayOfFiles, stmt, name, data, type, nbelem);
}

boolean OCI_API OCI_BindRef
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Ref      * data
)
{
    CALL_IMPL(StatementBindReference, stmt, name, data);
}

boolean OCI_API OCI_BindArrayOfRefs
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Ref     ** data,
    OCI_TypeInfo * typinf,
    unsigned int   nbelem
)
{
    CALL_IMPL(StatementBindArrayOfReferences, stmt, name, data, typinf, nbelem);
}

boolean OCI_API OCI_BindColl
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Coll     * data
)
{
    CALL_IMPL(StatementBindCollection, stmt, name, data);
}

boolean OCI_API OCI_BindArrayOfColls
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Coll    ** data,
    OCI_TypeInfo * typinf,
    unsigned int   nbelem
)
{
    CALL_IMPL(StatementBindArrayOfCollections, stmt, name, data, typinf, nbelem);
}

boolean OCI_API OCI_BindStatement
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Statement* data
)
{
    CALL_IMPL(StatementBindStatement, stmt, name, data);
}

boolean OCI_API OCI_BindLong
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Long     * data,
    unsigned int   size
)
{
    CALL_IMPL(StatementBindLong, stmt, name, data, size);
}

boolean OCI_API OCI_RegisterNumber
(
    OCI_Statement* stmt,
    const otext  * name
)
{
    CALL_IMPL(StatementRegisterNumber, stmt, name);
}

boolean OCI_API OCI_RegisterShort
(
    OCI_Statement* stmt,
    const otext  * name
)
{
    CALL_IMPL(StatementRegisterShort, stmt, name);
}

boolean OCI_API OCI_RegisterUnsignedShort
(
    OCI_Statement* stmt,
    const otext  * name
)
{
    CALL_IMPL(StatementRegisterUnsignedShort, stmt, name);
}

boolean OCI_API OCI_RegisterInt
(
    OCI_Statement* stmt,
    const otext  * name
)
{
    CALL_IMPL(StatementRegisterInt, stmt, name);
}

boolean OCI_API OCI_RegisterUnsignedInt
(
    OCI_Statement* stmt,
    const otext  * name
)
{
    CALL_IMPL(StatementRegisterUnsignedInt, stmt, name);
}

boolean OCI_API OCI_RegisterBigInt
(
    OCI_Statement* stmt,
    const otext  * name
)
{
    CALL_IMPL(StatementRegisterBigInt, stmt, name);
}

boolean OCI_API OCI_RegisterUnsignedBigInt
(
    OCI_Statement* stmt,
    const otext  * name
)
{
    CALL_IMPL(StatementRegisterUnsignedBigInt, stmt, name);
}

boolean OCI_API OCI_RegisterString
(
    OCI_Statement* stmt,
    const otext  * name,
    unsigned int   len
)
{
    CALL_IMPL(StatementRegisterString, stmt, name, len);
}

boolean OCI_API OCI_RegisterRaw
(
    OCI_Statement* stmt,
    const otext  * name,
    unsigned int   len
)
{
    CALL_IMPL(StatementRegisterRaw, stmt, name, len);
}

boolean OCI_API OCI_RegisterDouble
(
    OCI_Statement* stmt,
    const otext  * name
)
{
    CALL_IMPL(StatementRegisterDouble, stmt, name);
}

boolean OCI_API OCI_RegisterFloat
(
    OCI_Statement* stmt,
    const otext  * name
)
{
    CALL_IMPL(StatementRegisterFloat, stmt, name);
}

boolean OCI_API OCI_RegisterDate
(
    OCI_Statement* stmt,
    const otext  * name
)
{
    CALL_IMPL(StatementRegisterDate, stmt, name);
}

boolean OCI_API OCI_RegisterTimestamp
(
    OCI_Statement* stmt,
    const otext  * name,
    unsigned int   type
)
{
    CALL_IMPL(StatementRegisterTimestamp, stmt, name, type);
}

boolean OCI_API OCI_RegisterInterval
(
    OCI_Statement* stmt,
    const otext  * name,
    unsigned int   type
)
{
    CALL_IMPL(StatementRegisterInterval, stmt, name, type);
}

boolean OCI_API OCI_RegisterObject
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_TypeInfo * typinf
)
{
    CALL_IMPL(StatementRegisterObject, stmt, name, typinf);
}

boolean OCI_API OCI_RegisterLob
(
    OCI_Statement* stmt,
    const otext  * name,
    unsigned int   type
)
{
    CALL_IMPL(StatementRegisterLob, stmt, name, type);
}

boolean OCI_API OCI_RegisterFile
(
    OCI_Statement* stmt,
    const otext  * name,
    unsigned int   type
)
{
    CALL_IMPL(StatementRegisterFile, stmt, name, type);
}

boolean OCI_API OCI_RegisterRef
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_TypeInfo * typinf
)
{
    CALL_IMPL(StatementRegisterReference, stmt, name, typinf);
}

unsigned int OCI_API OCI_GetStatementType
(
    OCI_Statement* stmt
)
{
    CALL_IMPL(StatementGetStatementType, stmt);
}

boolean OCI_API OCI_SetFetchMode
(
    OCI_Statement* stmt,
    unsigned int   mode
)
{
    CALL_IMPL(StatementSetFetchMode, stmt, mode);
}

unsigned int OCI_API OCI_GetFetchMode
(
    OCI_Statement* stmt
)
{
    CALL_IMPL(StatementGetFetchMode, stmt);
}

boolean OCI_API OCI_SetBindMode
(
    OCI_Statement* stmt,
    unsigned int   mode
)
{
    CALL_IMPL(StatementSetBindMode, stmt, mode);
}

unsigned int OCI_API OCI_GetBindMode
(
    OCI_Statement* stmt
)
{
    CALL_IMPL(StatementGetBindMode, stmt);
}

boolean OCI_API OCI_SetBindAllocation
(
    OCI_Statement* stmt,
    unsigned int   mode
)
{
    CALL_IMPL(StatementSetBindAllocation, stmt, mode);
}

unsigned int OCI_API OCI_GetBindAllocation
(
    OCI_Statement* stmt
)
{
    CALL_IMPL(StatementGetBindAllocation, stmt);
}

boolean OCI_API OCI_SetFetchSize
(
    OCI_Statement* stmt,
    unsigned int   size
)
{
    CALL_IMPL(StatementSetFetchSize, stmt, size);
}

unsigned int OCI_API OCI_GetFetchSize
(
    OCI_Statement* stmt
)
{
    CALL_IMPL(StatementGetFetchSize, stmt);
}

boolean OCI_API OCI_SetPrefetchSize
(
    OCI_Statement* stmt,
    unsigned int   size
)
{
    CALL_IMPL(StatementSetPrefetchSize, stmt, size);
}

unsigned int OCI_API OCI_GetPrefetchSize
(
    OCI_Statement* stmt
)
{
    CALL_IMPL(StatementGetPrefetchSize, stmt);
}

boolean OCI_API OCI_SetPrefetchMemory
(
    OCI_Statement* stmt,
    unsigned int   size
)
{
    CALL_IMPL(StatementSetPrefetchMemory, stmt, size);
}

unsigned int OCI_API OCI_GetPrefetchMemory
(
    OCI_Statement* stmt
)
{
    CALL_IMPL(StatementGetPrefetchMemory, stmt);
}

boolean OCI_API OCI_SetLongMaxSize
(
    OCI_Statement* stmt,
    unsigned int   size
)
{
    CALL_IMPL(StatementSetLongMaxSize, stmt, size);
}

unsigned int OCI_API OCI_GetLongMaxSize
(
    OCI_Statement* stmt
)
{
    CALL_IMPL(StatementGetLongMaxSize, stmt);
}

boolean OCI_API OCI_SetLongMode
(
    OCI_Statement* stmt,
    unsigned int   mode
)
{
    CALL_IMPL(StatementSetLongMode, stmt, mode);
}

unsigned int OCI_API OCI_GetLongMode
(
    OCI_Statement* stmt
)
{
    CALL_IMPL(StatementGetLongMode, stmt);
}

OCI_Connection* OCI_API OCI_StatementGetConnection
(
    OCI_Statement* stmt
)
{
    CALL_IMPL(StatementGetConnection, stmt);
}

const otext* OCI_API OCI_GetSql
(
    OCI_Statement* stmt
)
{
    CALL_IMPL(StatementGetSql, stmt);
}

const otext* OCI_API OCI_GetSqlIdentifier
(
    OCI_Statement* stmt
)
{
    CALL_IMPL(StatementGetSqlIdentifier, stmt);
}

unsigned int OCI_API OCI_GetSqlErrorPos
(
    OCI_Statement* stmt
)
{
    CALL_IMPL(StatementGetSqlErrorPos, stmt);
}

unsigned int OCI_API OCI_GetAffectedRows
(
    OCI_Statement* stmt
)
{
    CALL_IMPL(StatementGetAffectedRows, stmt);
}

unsigned int OCI_API OCI_GetBindCount
(
    OCI_Statement* stmt
)
{
    CALL_IMPL(StatementGetBindCount, stmt);
}

OCI_Bind* OCI_API OCI_GetBind
(
    OCI_Statement* stmt,
    unsigned int   index
)
{
    CALL_IMPL(StatementGetBind, stmt, index);
}

OCI_Bind* OCI_API OCI_GetBind2
(
    OCI_Statement* stmt,
    const otext  * name
)
{
    CALL_IMPL(StatementGetBind2, stmt, name);
}

unsigned int OCI_API OCI_GetBindIndex
(
    OCI_Statement* stmt,
    const otext  * name
)
{
    CALL_IMPL(StatementGetBindIndex, stmt, name);
}

unsigned int OCI_API OCI_GetSQLCommand
(
    OCI_Statement* stmt
)
{
    CALL_IMPL(StatementGetSqlCommand, stmt);
}

const otext* OCI_API OCI_GetSQLVerb
(
    OCI_Statement* stmt
)
{
    CALL_IMPL(StatementGetSqlVerb, stmt);
}

OCI_Error* OCI_API OCI_GetBatchError
(
    OCI_Statement* stmt
)
{
    CALL_IMPL(StatementGetBatchError, stmt);
}

unsigned int OCI_API OCI_GetBatchErrorCount
(
    OCI_Statement* stmt
)
{
    CALL_IMPL(StatementGetBatchErrorCount, stmt);
}

/* --------------------------------------------------------------------------------------------- *
 *  subscription
 * --------------------------------------------------------------------------------------------- */

OCI_Subscription* OCI_API OCI_SubscriptionRegister
(
    OCI_Connection* con,
    const otext   * name,
    unsigned int    type,
    POCI_NOTIFY     handler,
    unsigned int    port,
    unsigned int    timeout
)
{
    CALL_IMPL(SubscriptionRegister, con, name, type, handler, port, timeout);
}

boolean OCI_API OCI_SubscriptionUnregister
(
    OCI_Subscription* sub
)
{
    CALL_IMPL(SubscriptionUnregister, sub);
}

boolean OCI_API OCI_SubscriptionAddStatement
(
    OCI_Subscription* sub,
    OCI_Statement   * stmt
)
{
    CALL_IMPL(SubscriptionAddStatement, sub, stmt);
}

const otext* OCI_API OCI_SubscriptionGetName
(
    OCI_Subscription* sub
)
{
    CALL_IMPL(SubscriptionGetName, sub);
}

unsigned int OCI_API OCI_SubscriptionGetPort
(
    OCI_Subscription* sub
)
{
    CALL_IMPL(SubscriptionGetPort, sub);
}

unsigned int OCI_API OCI_SubscriptionGetTimeout
(
    OCI_Subscription* sub
)
{
    CALL_IMPL(SubscriptionGetTimeout, sub);
}

OCI_Connection* OCI_API OCI_SubscriptionGetConnection
(
    OCI_Subscription* sub
)
{
    CALL_IMPL(SubscriptionGetConnection, sub);
}

/* --------------------------------------------------------------------------------------------- *
 * timestamp
 * --------------------------------------------------------------------------------------------- */

OCI_Timestamp* OCI_API OCI_TimestampCreate
(
    OCI_Connection* con,
    unsigned int    type
)
{
    CALL_IMPL(TimestampCreate, con, type);
}

boolean OCI_API OCI_TimestampFree
(
    OCI_Timestamp* tmsp
)
{
    CALL_IMPL(TimestampFree, tmsp);
}

OCI_Timestamp** OCI_API OCI_TimestampArrayCreate
(
    OCI_Connection* con,
    unsigned int    type,
    unsigned int    nbelem
)
{
    CALL_IMPL(TimestampCreateArray, con, type, nbelem);
}

boolean OCI_API OCI_TimestampArrayFree
(
    OCI_Timestamp** tmsps
)
{
    CALL_IMPL(TimestampFreeArray, tmsps);
}

unsigned int OCI_API OCI_TimestampGetType
(
    OCI_Timestamp* tmsp
)
{
    CALL_IMPL(TimestampGetType, tmsp);
}

boolean OCI_API OCI_TimestampAssign
(
    OCI_Timestamp* tmsp,
    OCI_Timestamp* tmsp_src
)
{
    CALL_IMPL(TimestampAssign, tmsp, tmsp_src);
}

int OCI_API OCI_TimestampCheck
(
    OCI_Timestamp* tmsp
)
{
    CALL_IMPL(TimestampCheck, tmsp);
}

int OCI_API OCI_TimestampCompare
(
    OCI_Timestamp* tmsp,
    OCI_Timestamp* tmsp2
)
{
    CALL_IMPL(TimestampCompare, tmsp, tmsp2);
}

boolean OCI_API OCI_TimestampConstruct
(
    OCI_Timestamp* tmsp,
    int            year,
    int            month,
    int            day,
    int            hour,
    int            min,
    int            sec,
    int            fsec,
    const otext  * time_zone
)
{
    CALL_IMPL(TimestampConstruct, tmsp, year, month, day, hour, min, sec,fsec, time_zone);
}

boolean OCI_API OCI_TimestampConvert
(
    OCI_Timestamp* tmsp,
    OCI_Timestamp* tmsp_src
)
{
    CALL_IMPL(TimestampConvert, tmsp, tmsp_src);
}

boolean OCI_API OCI_TimestampFromText
(
    OCI_Timestamp* tmsp,
    const otext  * str,
    const otext  * fmt
)
{
    CALL_IMPL(TimestampFromString, tmsp, str, fmt);
}

boolean OCI_API OCI_TimestampToText
(
    OCI_Timestamp* tmsp,
    const otext  * fmt,
    int            size,
    otext        * str,
    int            precision
)
{
    CALL_IMPL(TimestampToString, tmsp, fmt, size, str, precision);
}

boolean OCI_API OCI_TimestampGetDate
(
    OCI_Timestamp* tmsp,
    int          * year,
    int          * month,
    int          * day
)
{
    CALL_IMPL(TimestampGetDate, tmsp, year, month, day);
}

boolean OCI_API OCI_TimestampGetTime
(
    OCI_Timestamp* tmsp,
    int          * hour,
    int          * min,
    int          * sec,
    int          * fsec
)
{
    CALL_IMPL(TimestampGetTime, tmsp, hour, min, sec, fsec);
}

boolean OCI_API OCI_TimestampGetDateTime
(
    OCI_Timestamp* tmsp,
    int          * year,
    int          * month,
    int          * day,
    int          * hour,
    int          * min,
    int          * sec,
    int          * fsec
)
{
    CALL_IMPL(TimestampGetDateTime, tmsp, year, month, day, hour, min, sec, fsec);
}

boolean OCI_API OCI_TimestampGetTimeZoneName
(
    OCI_Timestamp* tmsp,
    int            size,
    otext        * str
)
{
    CALL_IMPL(TimestampGetTimeZoneName, tmsp, size, str);
}

boolean OCI_API OCI_TimestampGetTimeZoneOffset
(
    OCI_Timestamp* tmsp,
    int          * hour,
    int          * min
)
{
    CALL_IMPL(TimestampGetTimeZoneOffset, tmsp, hour,min);
}

boolean OCI_API OCI_TimestampIntervalAdd
(
    OCI_Timestamp* tmsp,
    OCI_Interval * itv
)
{
    CALL_IMPL(TimestampIntervalAdd, tmsp, itv);
}

boolean OCI_API OCI_TimestampIntervalSub
(
    OCI_Timestamp* tmsp,
    OCI_Interval * itv
)
{
    CALL_IMPL(TimestampIntervalSub, tmsp, itv);
}

boolean OCI_API OCI_TimestampSubtract
(
    OCI_Timestamp* tmsp,
    OCI_Timestamp* tmsp2,
    OCI_Interval * itv
)
{
    CALL_IMPL(TimestampSubtract, tmsp, tmsp2, itv);
}

boolean OCI_API OCI_TimestampSysTimestamp
(
    OCI_Timestamp* tmsp
)
{
    CALL_IMPL(TimestampSysTimestamp, tmsp);
}

boolean OCI_API OCI_TimestampToCTime
(
    OCI_Timestamp* tmsp,
    struct tm    * ptm,
    time_t       * pt
)
{
    CALL_IMPL(TimestampToCTime, tmsp, ptm, pt);
}

boolean OCI_API OCI_TimestampFromCTime
(
    OCI_Timestamp* tmsp,
    struct tm    * ptm,
    time_t         t
)
{
    CALL_IMPL(TimestampFromCTime, tmsp, ptm, t);
}

/* --------------------------------------------------------------------------------------------- *
 *  thread
 * --------------------------------------------------------------------------------------------- */

OCI_Thread* OCI_API OCI_ThreadCreate
(
    void
)
{
    CALL_IMPL(ThreadCreate);
}

boolean OCI_API OCI_ThreadFree
(
    OCI_Thread* thread
)
{
    CALL_IMPL(ThreadFree, thread);
}

boolean OCI_API OCI_ThreadRun
(
    OCI_Thread* thread,
    POCI_THREAD proc,
    void      * arg
)
{
    CALL_IMPL(ThreadRun, thread, proc, arg);
}

boolean OCI_API OCI_ThreadJoin
(
    OCI_Thread* thread
)
{
    CALL_IMPL(ThreadJoin, thread);
}

/* --------------------------------------------------------------------------------------------- *
 *  thread key
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ThreadKeyCreate
(
    const otext      * name,
    POCI_THREADKEYDEST destfunc
)
{
    CALL_IMPL(ThreadKeyCreate, name, destfunc);
}

boolean OCI_API OCI_ThreadKeySetValue
(
    const otext* name,
    void       * value
)
{
    CALL_IMPL(ThreadKeySetValue, name, value);
}

void* OCI_API OCI_ThreadKeyGetValue
(
    const otext* name
)
{
    CALL_IMPL(ThreadKeyGetValue, name);
}

/* --------------------------------------------------------------------------------------------- *
 *  transaction
 * --------------------------------------------------------------------------------------------- */

OCI_Transaction* OCI_API OCI_TransactionCreate
(
    OCI_Connection* con,
    unsigned int    timeout,
    unsigned int    mode,
    OCI_XID       * pxid
)
{
    CALL_IMPL(TransactionCreate, con, timeout, mode, pxid);
}

boolean OCI_API OCI_TransactionFree
(
    OCI_Transaction* trans
)
{
    CALL_IMPL(TransactionFree, trans);
}

boolean OCI_API OCI_TransactionStart
(
    OCI_Transaction* trans
)
{
    CALL_IMPL(TransactionStart, trans);
}

boolean OCI_API OCI_TransactionStop
(
    OCI_Transaction* trans
)
{
    CALL_IMPL(TransactionStop, trans);
}

boolean OCI_API OCI_TransactionResume
(
    OCI_Transaction* trans
)
{
    CALL_IMPL(TransactionResume, trans);
}

boolean OCI_API OCI_TransactionPrepare
(
    OCI_Transaction* trans
)
{
    CALL_IMPL(TransactionPrepare, trans);
}

boolean OCI_API OCI_TransactionForget
(
    OCI_Transaction* trans
)
{
    CALL_IMPL(TransactionForget, trans);
}

unsigned int OCI_API OCI_TransactionGetMode
(
    OCI_Transaction* trans
)
{
    CALL_IMPL(TransactionGetMode, trans);
}

unsigned int OCI_API OCI_TransactionGetTimeout
(
    OCI_Transaction* trans
)
{
    CALL_IMPL(TransactionGetTimeout, trans);
}

/* --------------------------------------------------------------------------------------------- *
 *  type info
 * --------------------------------------------------------------------------------------------- */

OCI_TypeInfo* OCI_API OCI_TypeInfoGet
(
    OCI_Connection* con,
    const otext   * name,
    unsigned int    type
)
{
    CALL_IMPL(TypeInfoGet, con, name, type);
}

unsigned int OCI_API OCI_TypeInfoGetType
(
    OCI_TypeInfo* typinf
)
{
    CALL_IMPL(TypeInfoGetType, typinf);
}

OCI_Connection* OCI_API OCI_TypeInfoGetConnection
(
    OCI_TypeInfo* typinf
)
{
    CALL_IMPL(TypeInfoGetConnection, typinf);
}

boolean OCI_API OCI_TypeInfoFree
(
    OCI_TypeInfo* typinf
)
{
    CALL_IMPL(TypeInfoFree, typinf);
}

unsigned int OCI_API OCI_TypeInfoGetColumnCount
(
    OCI_TypeInfo* typinf
)
{
    CALL_IMPL(TypeInfoGetColumnCount, typinf);
}

OCI_Column* OCI_API OCI_TypeInfoGetColumn
(
    OCI_TypeInfo* typinf,
    unsigned int  index
)
{
    CALL_IMPL(TypeInfoGetColumn, typinf, index);
}

const otext* OCI_API OCI_TypeInfoGetName
(
    OCI_TypeInfo* typinf
)
{
    CALL_IMPL(TypeInfoGetName, typinf);
}

boolean OCI_API OCI_TypeInfoIsFinalType
(
    OCI_TypeInfo* typinf
)
{
    CALL_IMPL(TypeInfoIsFinalType, typinf);
}

OCI_TypeInfo* OCI_API OCI_TypeInfoGetSuperType
(
    OCI_TypeInfo* typinf
)
{
    CALL_IMPL(TypeInfoGetSuperType, typinf);
}
