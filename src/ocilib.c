/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2025 Vincent ROGIER <vince.rogier@ocilib.net>
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
#include "vector.h"
#include "xmltype.h"

static void OcilibCheckContext
(
    void
)
{
    if (Env.env_mode & OCI_ENV_CONTEXT)
    {
        OcilibErrorReset(OcilibErrorGet(TRUE, TRUE));
    }
}

#define CALL_IMPL(impl, ...)        \
    OcilibCheckContext();           \
    return impl(__VA_ARGS__);       \


#define CALL_IMPL_NO_ARGS(impl)     \
    OcilibCheckContext();           \
    return impl();                  \

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
    CALL_IMPL(OcilibAgentCreate, con, name, address)
}

boolean OCI_API OCI_AgentFree
(
    OCI_Agent* agent
)
{
    CALL_IMPL(OcilibAgentFree, agent)
}

const otext* OCI_API OCI_AgentGetName
(
    OCI_Agent* agent
)
{
    CALL_IMPL(OcilibAgentGetName, agent)
}

boolean OCI_API OCI_AgentSetName
(
    OCI_Agent  * agent,
    const otext* name
)
{
    CALL_IMPL(OcilibAgentSetName, agent, name)
}

const otext* OCI_API OCI_AgentGetAddress
(
    OCI_Agent* agent
)
{
    CALL_IMPL(OcilibAgentGetAddress, agent)
}

boolean OCI_API OCI_AgentSetAddress
(
    OCI_Agent  * agent,
    const otext* address
)
{
    CALL_IMPL(OcilibAgentSetAddress, agent, address)
}

/* --------------------------------------------------------------------------------------------- *
  * bind
  * --------------------------------------------------------------------------------------------- */

const otext* OCI_API OCI_BindGetName
(
    OCI_Bind* bnd
)
{
    CALL_IMPL(OcilibBindGetName, bnd)
}

unsigned int OCI_API OCI_BindGetType
(
    OCI_Bind* bnd
)
{
    CALL_IMPL(OcilibBindGetType, bnd)
}

unsigned int OCI_API OCI_BindGetSubtype
(
    OCI_Bind* bnd
)
{
    CALL_IMPL(OcilibBindGetSubtype, bnd)
}

unsigned int OCI_API OCI_BindGetDataCount
(
    OCI_Bind* bnd
)
{
    CALL_IMPL(OcilibBindGetDataCount, bnd)
}

void* OCI_API OCI_BindGetData
(
    OCI_Bind* bnd
)
{
    CALL_IMPL(OcilibBindGetData, bnd)
}

OCI_Statement* OCI_API OCI_BindGetStatement
(
    OCI_Bind* bnd
)
{
    CALL_IMPL(OcilibBindGetStatement, bnd)
}

boolean OCI_API OCI_BindSetDataSize
(
    OCI_Bind   * bnd,
    unsigned int size
)
{
    CALL_IMPL(OcilibBindSetDataSize, bnd, size)
}

boolean OCI_API OCI_BindSetDataSizeAtPos
(
    OCI_Bind   * bnd,
    unsigned int position,
    unsigned int size
)
{
    CALL_IMPL(OcilibBindSetDataSizeAtPos, bnd, position, size)
}

unsigned int OCI_API OCI_BindGetDataSize
(
    OCI_Bind* bnd
)
{
    CALL_IMPL(OcilibBindGetDataSize, bnd)
}

unsigned int OCI_API OCI_BindGetDataSizeAtPos
(
    OCI_Bind   * bnd,
    unsigned int position
)
{
    CALL_IMPL(OcilibBindGetDataSizeAtPos, bnd, position)
}

boolean OCI_API OCI_BindSetNullAtPos
(
    OCI_Bind   * bnd,
    unsigned int position
)
{
    CALL_IMPL(OcilibBindSetNullAtPos, bnd, position)
}

boolean OCI_API OCI_BindSetNull
(
    OCI_Bind* bnd
)
{
    CALL_IMPL(OcilibBindSetNull, bnd)
}

boolean OCI_API OCI_BindSetNotNullAtPos
(
    OCI_Bind   * bnd,
    unsigned int position
)
{
    CALL_IMPL(OcilibBindSetNotNullAtPos, bnd, position)
}

boolean OCI_API OCI_BindSetNotNull
(
    OCI_Bind* bnd
)
{
    CALL_IMPL(OcilibBindSetNotNull, bnd)
}

boolean OCI_API OCI_BindIsNullAtPos
(
    OCI_Bind   * bnd,
    unsigned int position
)
{
    CALL_IMPL(OcilibBindIsNullAtPos, bnd, position)
}

boolean OCI_API OCI_BindIsNull
(
    OCI_Bind* bnd
)
{
    CALL_IMPL(OcilibBindIsNull, bnd)
}

boolean OCI_API OCI_BindSetCharsetForm
(
    OCI_Bind   * bnd,
    unsigned int csfrm
)
{
    CALL_IMPL(OcilibBindSetCharsetForm, bnd, csfrm)
}

boolean OCI_API OCI_BindSetDirection
(
    OCI_Bind   * bnd,
    unsigned int direction
)
{
    CALL_IMPL(OcilibBindSetDirection, bnd, direction)
}

unsigned int OCI_API OCI_BindGetDirection
(
    OCI_Bind* bnd
)
{
    CALL_IMPL(OcilibBindGetDirection, bnd)
}

unsigned int OCI_API OCI_BindGetAllocationMode
(
    OCI_Bind* bnd
)
{
    CALL_IMPL(OcilibBindGetAllocationMode, bnd)
}

/* --------------------------------------------------------------------------------------------- *
 * collection
 * --------------------------------------------------------------------------------------------- */

OCI_Coll * OCI_API OCI_CollCreate
(
    OCI_TypeInfo *typinf
)
{
    CALL_IMPL(OcilibCollectionCreate, typinf)
}

boolean OCI_API OCI_CollFree
(
    OCI_Coll *coll
)
{
    CALL_IMPL(OcilibCollectionFree, coll)
}

OCI_Coll ** OCI_API OCI_CollArrayCreate
(
    OCI_Connection *con,
    OCI_TypeInfo   *typinf,
    unsigned int    nbelem
)
{
    CALL_IMPL(OcilibCollectionCreateArray, con, typinf, nbelem)
}

boolean OCI_API OCI_CollArrayFree
(
    OCI_Coll **colls
)
{
    CALL_IMPL(OcilibCollectionFreeArray, colls)
}

boolean OCI_API OCI_CollAssign
(
    OCI_Coll *coll,
    OCI_Coll *coll_src
)
{
    CALL_IMPL(OcilibCollectionAssign, coll, coll_src)
}

unsigned int OCI_API OCI_CollGetType
(
    OCI_Coll *coll
)
{
    CALL_IMPL(OcilibCollectionGetType, coll)
}

unsigned int OCI_API OCI_CollGetMax
(
    OCI_Coll *coll
)
{
    CALL_IMPL(OcilibCollectionGetMax, coll)
}

unsigned int OCI_API OCI_CollGetSize
(
    OCI_Coll *coll
)
{
    CALL_IMPL(OcilibCollectionGetSize, coll)
}

boolean OCI_API OCI_CollTrim
(
    OCI_Coll    *coll,
    unsigned int nb_elem
)
{
    CALL_IMPL(OcilibCollectionTrim, coll, nb_elem)
}

OCI_Elem * OCI_API OCI_CollGetElem
(
    OCI_Coll    *coll,
    unsigned int index
)
{
    CALL_IMPL(OcilibCollectionGetElement, coll, index)
}

boolean OCI_API OCI_CollGetElem2
(
    OCI_Coll    *coll,
    unsigned int index,
    OCI_Elem    *elem
)
{
    CALL_IMPL(OcilibCollectionGetElement2, coll, index, elem)
}

boolean OCI_API OCI_CollSetElem
(
    OCI_Coll    *coll,
    unsigned int index,
    OCI_Elem    *elem
)
{
    CALL_IMPL(OcilibCollectionSetElement, coll, index, elem)
}

boolean OCI_API OCI_CollAppend
(
    OCI_Coll *coll,
    OCI_Elem *elem
)
{
    CALL_IMPL(OcilibCollectionAddElement, coll, elem)
}

OCI_TypeInfo * OCI_API OCI_CollGetTypeInfo
(
    OCI_Coll *coll
)
{
    CALL_IMPL(OcilibCollectionGetTypeInfo, coll)
}

boolean OCI_API OCI_CollClear
(
    OCI_Coll *coll
)
{
    CALL_IMPL(OcilibCollectionClear, coll)
}

boolean OCI_API OCI_CollDeleteElem
(
    OCI_Coll    *coll,
    unsigned int index
)
{
    CALL_IMPL(OcilibCollectionRemoveElement, coll, index)
}

unsigned int OCI_API OCI_CollGetCount
(
    OCI_Coll *coll
)
{
    CALL_IMPL(OcilibCollectionGetCount, coll)
}

boolean OCI_API OCI_CollToText
(
    OCI_Coll     *coll,
    unsigned int *size,
    otext        *str
)
{
    CALL_IMPL(OcilibCollectionToString, coll, size, str)
}

/* --------------------------------------------------------------------------------------------- *
 * column
 * --------------------------------------------------------------------------------------------- */

const otext* OCI_API OCI_ColumnGetName
(
    OCI_Column* col
)
{
    CALL_IMPL(OcilibColumnGetName, col)
}

unsigned int OCI_API OCI_ColumnGetType
(
    OCI_Column* col
)
{
    CALL_IMPL(OcilibColumnGetType, col)
}

unsigned int OCI_API OCI_ColumnGetCharsetForm
(
    OCI_Column* col
)
{
    CALL_IMPL(OcilibColumnGetCharsetForm, col)
}

unsigned int OCI_API OCI_ColumnGetSize
(
    OCI_Column* col
)
{
    CALL_IMPL(OcilibColumnGetSize, col)
}

int OCI_API OCI_ColumnGetScale
(
    OCI_Column* col
)
{
    CALL_IMPL(OcilibColumnGetScale, col)
}

int OCI_API OCI_ColumnGetPrecision
(
    OCI_Column* col
)
{
    CALL_IMPL(OcilibColumnGetPrecision, col)
}

int OCI_API OCI_ColumnGetFractionalPrecision
(
    OCI_Column* col
)
{
    CALL_IMPL(OcilibColumnGetFractionalPrecision, col)
}

int OCI_API OCI_ColumnGetLeadingPrecision
(
    OCI_Column* col
)
{
    CALL_IMPL(OcilibColumnGetLeadingPrecision, col)
}

boolean OCI_API OCI_ColumnGetNullable
(
    OCI_Column* col
)
{
    CALL_IMPL(OcilibColumnGetNullable, col)
}

boolean OCI_API OCI_ColumnGetCharUsed
(
    OCI_Column* col
)
{
    CALL_IMPL(OcilibColumnGetCharUsed, col)
}

unsigned int OCI_API OCI_ColumnGetPropertyFlags
(
    OCI_Column* col
)
{
    CALL_IMPL(OcilibColumnGetPropertyFlags, col)
}

unsigned int OCI_API OCI_ColumnGetCollationID
(
    OCI_Column* col
)
{
    CALL_IMPL(OcilibColumnGetCollationID, col)
}

unsigned int OCI_API OCI_ColumnGetDimension
(
    OCI_Column* col
)
{
    CALL_IMPL(OcilibColumnGetDimension, col)
}

const otext* OCI_API OCI_ColumnGetSQLType
(
    OCI_Column* col
)
{
    CALL_IMPL(OcilibColumnGetSqlType, col)
}

unsigned int OCI_API OCI_ColumnGetFullSQLType
(
    OCI_Column * col,
    otext      * buffer,
    unsigned int len
)
{
    CALL_IMPL(OcilibColumnGetFullSqlType, col, buffer, len)
}

OCI_TypeInfo* OCI_API OCI_ColumnGetTypeInfo
(
    OCI_Column* col
)
{
    CALL_IMPL(OcilibColumnGetTypeInfo, col)
}

unsigned int OCI_API OCI_ColumnGetSubType
(
    OCI_Column* col
)
{
    CALL_IMPL(OcilibColumnGetSubType, col)
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
    CALL_IMPL(OcilibConnectionCreate, db, user, pwd, mode)
}

boolean OCI_API OCI_ConnectionFree
(
    OCI_Connection *con
)
{
    CALL_IMPL(OcilibConnectionFree, con)
}

boolean OCI_API OCI_Commit
(
    OCI_Connection *con
)
{
    CALL_IMPL(OcilibConnectionCommit, con)
}

boolean OCI_API OCI_Rollback
(
    OCI_Connection *con
)
{
    CALL_IMPL(OcilibConnectionRollback, con)
}

boolean OCI_API OCI_SetAutoCommit
(
    OCI_Connection *con,
    boolean         enable
)
{
    CALL_IMPL(OcilibConnectionSetAutoCommit, con, enable)
}

boolean OCI_API OCI_GetAutoCommit
(
    OCI_Connection *con
)
{
    CALL_IMPL(OcilibConnectionGetAutoCommit, con)
}

boolean OCI_API OCI_IsConnected
(
    OCI_Connection *con
)
{
    CALL_IMPL(OcilibConnectionIsConnected, con)
}

void * OCI_API OCI_GetUserData
(
    OCI_Connection *con
)
{
    CALL_IMPL(OcilibConnectionGetUserData, con)
}

boolean OCI_API OCI_SetUserData
(
    OCI_Connection *con,
    void           *data
)
{
    CALL_IMPL(OcilibConnectionSetUserData, con, data)
}

boolean OCI_API OCI_SetSessionTag
(
    OCI_Connection *con,
    const otext    *tag
)
{
    CALL_IMPL(OcilibConnectionSetSessionTag, con, tag)
}

const otext * OCI_API OCI_GetSessionTag
(
    OCI_Connection *con
)
{
    CALL_IMPL(OcilibConnectionGetSessionTag, con)
}

const otext * OCI_API OCI_GetDatabase
(
    OCI_Connection *con
)
{
    CALL_IMPL(OcilibConnectionGetConnectionString, con)
}

const otext * OCI_API OCI_GetUserName
(
    OCI_Connection *con
)
{
    CALL_IMPL(OcilibConnectionGetUserName, con)
}

const otext * OCI_API OCI_GetPassword
(
    OCI_Connection *con
)
{
    CALL_IMPL(OcilibConnectionGetPassword, con)
}

boolean OCI_API OCI_SetPassword
(
    OCI_Connection *con,
    const otext    *password
)
{
    CALL_IMPL(OcilibConnectionSetPassword, con, password)
}

unsigned int OCI_API OCI_GetSessionMode
(
    OCI_Connection *con
)
{
    CALL_IMPL(OcilibConnectionGetSessionMode, con)
}

const otext * OCI_API OCI_GetVersionServer
(
    OCI_Connection *con
)
{
    CALL_IMPL(OcilibConnectionGetServerVersion, con)
}

unsigned int OCI_API OCI_GetServerMajorVersion
(
    OCI_Connection *con
)
{
    CALL_IMPL(OcilibConnectionGetServerMajorVersion, con)
}

unsigned int OCI_API OCI_GetServerMinorVersion
(
    OCI_Connection *con
)
{
    CALL_IMPL(OcilibConnectionGetServerMinorVersion, con)
}

unsigned int OCI_API OCI_GetServerRevisionVersion
(
    OCI_Connection *con
)
{
    CALL_IMPL(OcilibConnectionGetServerRevisionVersion, con)
}

OCI_Transaction * OCI_API OCI_GetTransaction
(
    OCI_Connection *con
)
{
    CALL_IMPL(OcilibConnectionGetTransaction, con)
}

boolean OCI_API OCI_SetTransaction
(
    OCI_Connection  *con,
    OCI_Transaction *trans
)
{
    CALL_IMPL(OcilibConnectionSetTransaction, con, trans)
}

unsigned int OCI_API OCI_GetVersionConnection
(
    OCI_Connection *con
)
{
    CALL_IMPL(OcilibConnectionGetVersion, con)
}

boolean OCI_API OCI_Break
(
    OCI_Connection *con
)
{
    CALL_IMPL(OcilibConnectionBreak, con)
}

boolean OCI_API OCI_ServerEnableOutput
(
    OCI_Connection *con,
    unsigned int    bufsize,
    unsigned int    arrsize,
    unsigned int    lnsize
)
{
    CALL_IMPL(OcilibConnectionEnableServerOutput, con, bufsize, arrsize, lnsize)
}

boolean OCI_API OCI_ServerDisableOutput
(
    OCI_Connection *con
)
{
    CALL_IMPL(OcilibConnectionDisableServerOutput, con)
}

const otext * OCI_API OCI_ServerGetOutput
(
    OCI_Connection *con
)
{
    CALL_IMPL(OcilibConnectionGetServerOutput, con)
}

boolean OCI_API OCI_SetTrace
(
    OCI_Connection *con,
    unsigned int    trace,
    const otext    *value
)
{
    CALL_IMPL(OcilibConnectionSetTrace, con, trace, value)
}

const otext * OCI_API OCI_GetTrace
(
    OCI_Connection *con,
    unsigned int    trace
)
{
    CALL_IMPL(OcilibConnectionGetTrace, con, trace)
}

boolean OCI_API OCI_Ping
(
    OCI_Connection *con
)
{
    CALL_IMPL(OcilibConnectionPing, con)
}

boolean OCI_API OCI_SetTimeout
(
    OCI_Connection *con,
    unsigned int    type,
    unsigned int    value
)
{
    CALL_IMPL(OcilibConnectionSetTimeout, con, type, value)
}

unsigned int OCI_API OCI_GetTimeout
(
    OCI_Connection *con,
    unsigned int    type
)
{
    CALL_IMPL(OcilibConnectionGetTimeout, con, type)
}

const otext * OCI_API OCI_GetDBName
(
    OCI_Connection *con
)
{
    CALL_IMPL(OcilibConnectionGetDatabaseName, con)
}

const otext * OCI_API OCI_GetInstanceName
(
    OCI_Connection *con
)
{
    CALL_IMPL(OcilibConnectionGetInstanceName, con)
}

const otext * OCI_API OCI_GetServiceName
(
    OCI_Connection *con
)
{
    CALL_IMPL(OcilibConnectionGetServiceName, con)
}

const otext * OCI_API OCI_GetServerName
(
    OCI_Connection *con
)
{
    CALL_IMPL(OcilibConnectionGetServerName, con)
}

const otext * OCI_API OCI_GetDomainName
(
    OCI_Connection *con
)
{
    CALL_IMPL(OcilibConnectionGetDomainName, con)
}

OCI_Timestamp * OCI_API OCI_GetInstanceStartTime
(
    OCI_Connection *con
)
{
    CALL_IMPL(OcilibConnectionGetInstanceStartTime, con)
}

boolean OCI_API OCI_IsTAFCapable
(
    OCI_Connection *con
)
{
    CALL_IMPL(OcilibConnectionIsTAFCapable, con)
}

boolean OCI_API OCI_SetTAFHandler
(
    OCI_Connection  *con,
    POCI_TAF_HANDLER handler
)
{
    CALL_IMPL(OcilibConnectionSetTAFHandler, con, handler)
}

unsigned int OCI_API OCI_GetStatementCacheSize
(
    OCI_Connection *con
)
{
    CALL_IMPL(OcilibConnectionGetStatementCacheSize, con)
}

boolean OCI_API OCI_SetStatementCacheSize
(
    OCI_Connection *con,
    unsigned int    value
)
{
    CALL_IMPL(OcilibConnectionSetStatementCacheSize, con, value)
}

unsigned int OCI_API OCI_GetDefaultLobPrefetchSize
(
    OCI_Connection *con
)
{
    CALL_IMPL(OcilibConnectionGetDefaultLobPrefetchSize, con)
}

boolean OCI_API OCI_SetDefaultLobPrefetchSize
(
    OCI_Connection *con,
    unsigned int    value
)
{
    CALL_IMPL(OcilibConnectionSetDefaultLobPrefetchSize, con, value)
}

unsigned int OCI_API OCI_GetMaxCursors
(
    OCI_Connection *con
)
{
    CALL_IMPL(OcilibConnectionGetMaxCursors, con)
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
    res = OcilibConnectionExecuteImmediate(con, sql, args);
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
    res = OcilibConnectionExecuteImmediateFmt(con, sql, args);
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
    CALL_IMPL(OcilibDateCreate, con)
}

boolean OCI_API OCI_DateFree
(
    OCI_Date* date
)
{
    CALL_IMPL(OcilibDateFree, date)
}

OCI_Date** OCI_API OCI_DateArrayCreate
(
    OCI_Connection* con,
    unsigned int    nbelem
)
{
    CALL_IMPL(OcilibDateCreateArray, con, nbelem)
}

boolean OCI_API OCI_DateArrayFree
(
    OCI_Date** dates
)
{
    CALL_IMPL(OcilibDateFreeArray, dates)
}

boolean OCI_API OCI_DateAddDays
(
    OCI_Date* date,
    int       nb
)
{
    CALL_IMPL(OcilibDateAddDays, date, nb)
}

boolean OCI_API OCI_DateAddMonths
(
    OCI_Date* date,
    int       nb
)
{
    CALL_IMPL(OcilibDateAddMonths, date, nb)
}

boolean OCI_API OCI_DateAssign
(
    OCI_Date* date,
    OCI_Date* date_src
)
{
    CALL_IMPL(OcilibDateAssign, date, date_src)
}

int OCI_API OCI_DateCheck
(
    OCI_Date* date
)
{
    CALL_IMPL(OcilibDateCheck, date)
}

int OCI_API OCI_DateCompare
(
    OCI_Date* date,
    OCI_Date* date2
)
{
    CALL_IMPL(OcilibDateCompare, date, date2)
}

int OCI_API OCI_DateDaysBetween
(
    OCI_Date* date,
    OCI_Date* date2
)
{
    CALL_IMPL(OcilibDateDaysBetween, date, date2)
}

boolean OCI_API OCI_DateFromText
(
    OCI_Date   * date,
    const otext* str,
    const otext* fmt
)
{
    CALL_IMPL(OcilibDateFromString, date, str, fmt)
}

boolean OCI_API OCI_DateGetDate
(
    OCI_Date* date,
    int     * year,
    int     * month,
    int     * day
)
{
    CALL_IMPL(OcilibDateGetDate, date, year, month, day)
}

boolean OCI_API OCI_DateGetTime
(
    OCI_Date* date,
    int     * hour,
    int     * min,
    int     * sec
)
{
    CALL_IMPL(OcilibDateGetTime, date, hour, min, sec)
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
    CALL_IMPL(OcilibDateGetDateTime, date, year, month, day, hour, min, sec)
}

boolean OCI_API OCI_DateLastDay
(
    OCI_Date* date
)
{
    CALL_IMPL(OcilibDateLastDay, date)
}

boolean OCI_API OCI_DateNextDay
(
    OCI_Date   * date,
    const otext* day
)
{
    CALL_IMPL(OcilibDateNextDay, date, day)
}

boolean OCI_API OCI_DateSetDate
(
    OCI_Date* date,
    int       year,
    int       month,
    int       day
)
{
    CALL_IMPL(OcilibDateSetDate, date, year, month, day)
}

boolean OCI_API OCI_DateSetTime
(
    OCI_Date* date,
    int       hour,
    int       min,
    int       sec
)
{
    CALL_IMPL(OcilibDateSetTime, date, hour, min, sec)
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
    CALL_IMPL(OcilibDateSetDateTime, date, year, month, day, hour, min,sec)
}

boolean OCI_API OCI_DateSysDate
(
    OCI_Date* date
)
{
    CALL_IMPL(OcilibDateSysDate, date)
}

boolean OCI_API OCI_DateToText
(
    OCI_Date   * date,
    const otext* fmt,
    int          size,
    otext      * str
)
{
    CALL_IMPL(OcilibDateToString, date, fmt, size, str)
}

boolean OCI_API OCI_DateZoneToZone
(
    OCI_Date   * date,
    const otext* zone1,
    const otext* zone2
)
{
    CALL_IMPL(OcilibDateZoneToZone, date, zone1, zone2)
}

boolean OCI_API OCI_DateToCTime
(
    OCI_Date * date,
    struct tm* ptm,
    time_t   * pt
)
{
    CALL_IMPL(OcilibDateToCTime, date, ptm, pt)
}

boolean OCI_API OCI_DateFromCTime
(
    OCI_Date * date,
    struct tm* ptm,
    time_t     t
)
{
    CALL_IMPL(OcilibDateFromCTime, date, ptm, t)
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
    CALL_IMPL(OcilibDequeueCreate,typinf, name)
}

boolean OCI_API OCI_DequeueFree
(
    OCI_Dequeue *dequeue
)
{
    CALL_IMPL(OcilibDequeueFree, dequeue)
}

OCI_Msg * OCI_API OCI_DequeueGet
(
    OCI_Dequeue *dequeue
)
{
    CALL_IMPL(OcilibDequeueGetMessage, dequeue)
}

boolean OCI_API OCI_DequeueSubscribe
(
    OCI_Dequeue   *dequeue,
    unsigned int   port,
    unsigned int   timeout,
    POCI_NOTIFY_AQ callback
)
{
    CALL_IMPL(OcilibDequeueSubscribe, dequeue, port, timeout, callback)
}

boolean OCI_API OCI_DequeueUnsubscribe
(
    OCI_Dequeue *dequeue
)
{
    CALL_IMPL(OcilibDequeueUnsubscribe, dequeue)
}

boolean OCI_API OCI_DequeueSetConsumer
(
    OCI_Dequeue *dequeue,
    const otext *consumer
)
{
    CALL_IMPL(OcilibDequeueSetConsumer, dequeue, consumer)
}

const otext * OCI_API OCI_DequeueGetConsumer
(
    OCI_Dequeue *dequeue
)
{
    CALL_IMPL(OcilibDequeueGetConsumer, dequeue)
}

boolean OCI_API OCI_DequeueSetCorrelation
(
    OCI_Dequeue *dequeue,
    const otext *pattern
)
{
    CALL_IMPL(OcilibDequeueSetCorrelation, dequeue, pattern)
}

const otext * OCI_API OCI_DequeueGetCorrelation
(
    OCI_Dequeue *dequeue
)
{
    CALL_IMPL(OcilibDequeueGetCorrelation, dequeue)
}

boolean OCI_API OCI_DequeueSetRelativeMsgID
(
    OCI_Dequeue *dequeue,
    const void  *id,
    unsigned int len
)
{
    CALL_IMPL(OcilibDequeueSetRelativeMsgID, dequeue, id, len)
}

boolean OCI_API OCI_DequeueGetRelativeMsgID
(
    OCI_Dequeue  *dequeue,
    void         *id,
    unsigned int *len
)
{
    CALL_IMPL(OcilibDequeueGetRelativeMsgID, dequeue, id, len)
}

boolean OCI_API OCI_DequeueSetVisibility
(
    OCI_Dequeue *dequeue,
    unsigned int visibility
)
{
    CALL_IMPL(OcilibDequeueSetVisibility, dequeue, visibility)
}

unsigned int OCI_API OCI_DequeueGetVisibility
(
    OCI_Dequeue *dequeue
)
{
    CALL_IMPL(OcilibDequeueGetVisibility, dequeue)
}

boolean OCI_API OCI_DequeueSetMode
(
    OCI_Dequeue *dequeue,
    unsigned int mode
)
{
    CALL_IMPL(OcilibDequeueSetMode, dequeue, mode)
}

unsigned int OCI_API OCI_DequeueGetMode
(
    OCI_Dequeue *dequeue
)
{
    CALL_IMPL(OcilibDequeueGetMode, dequeue)
}

boolean OCI_API OCI_DequeueSetNavigation
(
    OCI_Dequeue *dequeue,
    unsigned int position
)
{
    CALL_IMPL(OcilibDequeueSetNavigation, dequeue, position)
}

unsigned int OCI_API OCI_DequeueGetNavigation
(
    OCI_Dequeue *dequeue
)
{
    CALL_IMPL(OcilibDequeueGetNavigation, dequeue)
}

boolean OCI_API OCI_DequeueSetWaitTime
(
    OCI_Dequeue *dequeue,
    int          timeout
)
{
    CALL_IMPL(OcilibDequeueSetWaitTime, dequeue, timeout)
}

int OCI_API OCI_DequeueGetWaitTime
(
    OCI_Dequeue *dequeue
)
{
    CALL_IMPL(OcilibDequeueGetWaitTime, dequeue)
}

boolean OCI_API OCI_DequeueSetAgentList
(
    OCI_Dequeue *dequeue,
    OCI_Agent  **consumers,
    unsigned int count
)
{
    CALL_IMPL(OcilibDequeueSetAgentList, dequeue, consumers, count)
}

OCI_Agent * OCI_API OCI_DequeueListen
(
    OCI_Dequeue *dequeue,
    int          timeout
)
{
    CALL_IMPL(OcilibDequeueListen, dequeue, timeout)
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
    CALL_IMPL(OcilibDirPathCreate, typinf, partition, nb_cols, nb_rows)
}

boolean OCI_API OCI_DirPathFree
(
    OCI_DirPath* dp
)
{
    CALL_IMPL(OcilibDirPathFree, dp)
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
    CALL_IMPL(OcilibDirPathSetColumn, dp, index, name, maxsize, format)
}

boolean OCI_API OCI_DirPathPrepare
(
    OCI_DirPath* dp
)
{
    CALL_IMPL(OcilibDirPathPrepare, dp)
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
    CALL_IMPL(OcilibDirPathSetEntry, dp, row, index, value, size, complete)
}

unsigned int OCI_API OCI_DirPathConvert
(
    OCI_DirPath* dp
)
{
    CALL_IMPL(OcilibDirPathConvert, dp)
}

unsigned int OCI_API OCI_DirPathLoad
(
    OCI_DirPath* dp
)
{
    CALL_IMPL(OcilibDirPathLoad, dp)
}

boolean OCI_API OCI_DirPathReset
(
    OCI_DirPath* dp
)
{
    CALL_IMPL(OcilibDirPathReset, dp)
}

boolean OCI_API OCI_DirPathFinish
(
    OCI_DirPath* dp
)
{
    CALL_IMPL(OcilibDirPathFinish, dp)
}

boolean OCI_API OCI_DirPathAbort
(
    OCI_DirPath* dp
)
{
    CALL_IMPL(OcilibDirPathAbort, dp)
}

boolean OCI_API OCI_DirPathSave
(
    OCI_DirPath* dp
)
{
    CALL_IMPL(OcilibDirPathSave, dp)
}

boolean OCI_API OCI_DirPathFlushRow
(
    OCI_DirPath* dp
)
{
    CALL_IMPL(OcilibDirPathFlushRow, dp)
}

boolean OCI_API OCI_DirPathSetCurrentRows
(
    OCI_DirPath* dp,
    unsigned int nb_rows
)
{
    CALL_IMPL(OcilibDirPathSetCurrentRows, dp, nb_rows)
}

unsigned int OCI_API OCI_DirPathGetCurrentRows
(
    OCI_DirPath* dp
)
{
    CALL_IMPL(OcilibDirPathGetCurrentRows, dp)
}

unsigned int OCI_API OCI_DirPathGetMaxRows
(
    OCI_DirPath* dp
)
{
    CALL_IMPL(OcilibDirPathGetMaxRows, dp)
}

boolean OCI_API OCI_DirPathSetDateFormat
(
    OCI_DirPath* dp,
    const otext* format
)
{
    CALL_IMPL(OcilibDirPathSetDateFormat, dp, format)
}

boolean OCI_API OCI_DirPathSetParallel
(
    OCI_DirPath* dp,
    boolean      value
)
{
    CALL_IMPL(OcilibDirPathSetParallel, dp, value)
}

boolean OCI_API OCI_DirPathSetNoLog
(
    OCI_DirPath* dp,
    boolean      value
)
{
    CALL_IMPL(OcilibDirPathSetNoLog, dp, value)
}

boolean OCI_API OCI_DirPathSetCacheSize
(
    OCI_DirPath* dp,
    unsigned int size
)
{
    CALL_IMPL(OcilibDirPathSetCacheSize, dp, size)
}

boolean OCI_API OCI_DirPathSetBufferSize
(
    OCI_DirPath* dp,
    unsigned int size
)
{
    CALL_IMPL(OcilibDirPathSetBufferSize, dp, size)
}

boolean OCI_API OCI_DirPathSetConvertMode
(
    OCI_DirPath* dp,
    unsigned int mode
)
{
    CALL_IMPL(OcilibDirPathSetConvertMode, dp, mode)
}

unsigned int OCI_API OCI_DirPathGetRowCount
(
    OCI_DirPath* dp
)
{
    CALL_IMPL(OcilibDirPathGetRowCount, dp)
}

unsigned int OCI_API OCI_DirPathGetAffectedRows
(
    OCI_DirPath* dp
)
{
    CALL_IMPL(OcilibDirPathGetAffectedRows, dp)
}

unsigned int OCI_API OCI_DirPathGetErrorColumn
(
    OCI_DirPath* dp
)
{
    CALL_IMPL(OcilibDirPathGetErrorColumn, dp)
}

unsigned int OCI_API OCI_DirPathGetErrorRow
(
    OCI_DirPath* dp
)
{
    CALL_IMPL(OcilibDirPathGetErrorRow, dp)
}

/* --------------------------------------------------------------------------------------------- *
 * element
 * --------------------------------------------------------------------------------------------- */

OCI_Elem* OCI_API OCI_ElemCreate
(
    OCI_TypeInfo* typinf
)
{
    CALL_IMPL(OcilibElementCreate, typinf)
}

boolean OCI_API OCI_ElemFree
(
    OCI_Elem* elem
)
{
    CALL_IMPL(OcilibElementFree, elem)
}

boolean OCI_API OCI_ElemGetBoolean
(
    OCI_Elem* elem
)
{
    CALL_IMPL(OcilibElementGetBoolean, elem)
}

OCI_Number* OCI_API OCI_ElemGetNumber
(
    OCI_Elem* elem
)
{
    CALL_IMPL(OcilibElementGetNumber, elem)
}

short OCI_API OCI_ElemGetShort
(
    OCI_Elem* elem
)
{
    CALL_IMPL(OcilibElementGetShort, elem)
}

unsigned short OCI_API OCI_ElemGetUnsignedShort
(
    OCI_Elem* elem
)
{
    CALL_IMPL(OcilibElementGetUnsignedShort, elem)
}

int OCI_API OCI_ElemGetInt
(
    OCI_Elem* elem
)
{
    CALL_IMPL(OcilibElementGetInt, elem)
}

unsigned int OCI_API OCI_ElemGetUnsignedInt
(
    OCI_Elem* elem
)
{
    CALL_IMPL(OcilibElementGetUnsignedInt, elem)
}

big_int OCI_API OCI_ElemGetBigInt
(
    OCI_Elem* elem
)
{
    CALL_IMPL(OcilibElementGetBigInt, elem)
}

big_uint OCI_API OCI_ElemGetUnsignedBigInt
(
    OCI_Elem* elem
)
{
    CALL_IMPL(OcilibElementGetUnsignedBigInt, elem)
}

double OCI_API OCI_ElemGetDouble
(
    OCI_Elem* elem
)
{
    CALL_IMPL(OcilibElementGetDouble, elem)
}

float OCI_API OCI_ElemGetFloat
(
    OCI_Elem* elem
)
{
    CALL_IMPL(OcilibElementGetFloat, elem)
}

const otext* OCI_API OCI_ElemGetString
(
    OCI_Elem* elem
)
{
    CALL_IMPL(OcilibElementGetString, elem)
}

unsigned int OCI_API OCI_ElemGetRaw
(
    OCI_Elem   * elem,
    void       * value,
    unsigned int len
)
{
    CALL_IMPL(OcilibElementGetRaw, elem, value, len)
}

unsigned int OCI_API OCI_ElemGetRawSize
(
    OCI_Elem* elem
)
{
    CALL_IMPL(OcilibElementGetRawSize, elem)
}

OCI_Date* OCI_API OCI_ElemGetDate
(
    OCI_Elem* elem
)
{
    CALL_IMPL(OcilibElementGetDate, elem)
}

OCI_Timestamp* OCI_API OCI_ElemGetTimestamp
(
    OCI_Elem* elem
)
{
    CALL_IMPL(OcilibElementGetTimestamp, elem)
}

OCI_Interval* OCI_API OCI_ElemGetInterval
(
    OCI_Elem* elem
)
{
    CALL_IMPL(OcilibElementGetInterval, elem)
}

OCI_Lob* OCI_API OCI_ElemGetLob
(
    OCI_Elem* elem
)
{
    CALL_IMPL(OcilibElementGetLob, elem)
}

OCI_File* OCI_API OCI_ElemGetFile
(
    OCI_Elem* elem
)
{
    CALL_IMPL(OcilibElementGetFile, elem)
}

OCI_Ref* OCI_API OCI_ElemGetRef
(
    OCI_Elem* elem
)
{
    CALL_IMPL(OcilibElementGetReference, elem)
}

OCI_Object* OCI_API OCI_ElemGetObject
(
    OCI_Elem* elem
)
{
    CALL_IMPL(OcilibElementGetObject, elem)
}

OCI_Coll* OCI_API OCI_ElemGetColl
(
    OCI_Elem* elem
)
{
    CALL_IMPL(OcilibElementGetCollection, elem)
}

boolean OCI_API OCI_ElemSetBoolean
(
    OCI_Elem* elem,
    boolean   value
)
{
    CALL_IMPL(OcilibElementSetBoolean, elem, value)
}

boolean OCI_API OCI_ElemSetNumber
(
    OCI_Elem  * elem,
    OCI_Number* value
)
{
    CALL_IMPL(OcilibElementSetNumber, elem, value)
}

boolean OCI_API OCI_ElemSetShort
(
    OCI_Elem* elem,
    short     value
)
{
    CALL_IMPL(OcilibElementSetShort, elem, value)
}

boolean OCI_API OCI_ElemSetUnsignedShort
(
    OCI_Elem     * elem,
    unsigned short value
)
{
    CALL_IMPL(OcilibElementSetUnsignedShort, elem, value)
}

boolean OCI_API OCI_ElemSetInt
(
    OCI_Elem* elem,
    int       value
)
{
    CALL_IMPL(OcilibElementSetInt, elem, value)
}

boolean OCI_API OCI_ElemSetUnsignedInt
(
    OCI_Elem   * elem,
    unsigned int value
)
{
    CALL_IMPL(OcilibElementSetUnsignedInt, elem, value)
}

boolean OCI_API OCI_ElemSetBigInt
(
    OCI_Elem* elem,
    big_int   value
)
{
    CALL_IMPL(OcilibElementSetBigInt, elem, value)
}

boolean OCI_API OCI_ElemSetUnsignedBigInt
(
    OCI_Elem* elem,
    big_uint  value
)
{
    CALL_IMPL(OcilibElementSetUnsignedBigInt, elem, value)
}

boolean OCI_API OCI_ElemSetDouble
(
    OCI_Elem* elem,
    double    value
)
{
    CALL_IMPL(OcilibElementSetDouble, elem, value)
}

boolean OCI_API OCI_ElemSetFloat
(
    OCI_Elem* elem,
    float     value
)
{
    CALL_IMPL(OcilibElementSetFloat, elem, value)
}

boolean OCI_API OCI_ElemSetString
(
    OCI_Elem   * elem,
    const otext* value
)
{
    CALL_IMPL(OcilibElementSetString, elem, value)
}

boolean OCI_API OCI_ElemSetRaw
(
    OCI_Elem   * elem,
    void       * value,
    unsigned int len
)
{
    CALL_IMPL(OcilibElementSetRaw, elem, value, len)
}

boolean OCI_API OCI_ElemSetDate
(
    OCI_Elem* elem,
    OCI_Date* value
)
{
    CALL_IMPL(OcilibElementSetDate, elem, value)
}

boolean OCI_API OCI_ElemSetTimestamp
(
    OCI_Elem     * elem,
    OCI_Timestamp* value
)
{
    CALL_IMPL(OcilibElementSetTimestamp, elem, value)
}

boolean OCI_API OCI_ElemSetInterval
(
    OCI_Elem    * elem,
    OCI_Interval* value
)
{
    CALL_IMPL(OcilibElementSetInterval, elem, value)
}

boolean OCI_API OCI_ElemSetColl
(
    OCI_Elem* elem,
    OCI_Coll* value
)
{
    CALL_IMPL(OcilibElementSetCollection, elem, value)
}

boolean OCI_API OCI_ElemSetObject
(
    OCI_Elem  * elem,
    OCI_Object* value
)
{
    CALL_IMPL(OcilibElementSetObject, elem, value)
}

boolean OCI_API OCI_ElemSetLob
(
    OCI_Elem* elem,
    OCI_Lob * value
)
{
    CALL_IMPL(OcilibElementSetLob, elem, value)
}

boolean OCI_API OCI_ElemSetFile
(
    OCI_Elem* elem,
    OCI_File* value
)
{
    CALL_IMPL(OcilibElementSetFile, elem, value)
}

boolean OCI_API OCI_ElemSetRef
(
    OCI_Elem* elem,
    OCI_Ref * value
)
{
    CALL_IMPL(OcilibElementSetReference, elem, value)
}

boolean OCI_API OCI_ElemIsNull
(
    OCI_Elem* elem
)
{
    CALL_IMPL(OcilibElementIsNull, elem)
}

boolean OCI_API OCI_ElemSetNull
(
    OCI_Elem* elem
)
{
    CALL_IMPL(OcilibElementSetNull, elem)
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
    CALL_IMPL(OcilibEnqueueCreate, typinf, name)
}

boolean OCI_API OCI_EnqueueFree
(
    OCI_Enqueue* enqueue
)
{
    CALL_IMPL(OcilibEnqueueFree, enqueue)
}

boolean OCI_API OCI_EnqueuePut
(
    OCI_Enqueue* enqueue,
    OCI_Msg    * msg
)
{
    CALL_IMPL(OcilibEnqueuePut, enqueue, msg)
}

boolean OCI_API OCI_EnqueueSetSequenceDeviation
(
    OCI_Enqueue* enqueue,
    unsigned int sequence
)
{
    CALL_IMPL(OcilibEnqueueSetSequenceDeviation, enqueue, sequence)
}

unsigned int OCI_API OCI_EnqueueGetSequenceDeviation
(
    OCI_Enqueue* enqueue
)
{
    CALL_IMPL(OcilibEnqueueGetSequenceDeviation, enqueue)
}

boolean OCI_API OCI_EnqueueSetVisibility
(
    OCI_Enqueue* enqueue,
    unsigned int visibility
)
{
    CALL_IMPL(OcilibEnqueueSetVisibility, enqueue, visibility)
}

unsigned int OCI_API OCI_EnqueueGetVisibility
(
    OCI_Enqueue* enqueue
)
{
    CALL_IMPL(OcilibEnqueueGetVisibility, enqueue)
}

boolean OCI_API OCI_EnqueueSetRelativeMsgID
(
    OCI_Enqueue* enqueue,
    const void * id,
    unsigned int len
)
{
    CALL_IMPL(OcilibEnqueueSetRelativeMsgID, enqueue, id, len)
}

boolean OCI_API OCI_EnqueueGetRelativeMsgID
(
    OCI_Enqueue * enqueue,
    void        * id,
    unsigned int* len
)
{
    CALL_IMPL(OcilibEnqueueGetRelativeMsgID, enqueue, id, len)
}

/* --------------------------------------------------------------------------------------------- *
 * error
 * --------------------------------------------------------------------------------------------- */

const otext* OCI_API OCI_ErrorGetString
(
    OCI_Error* err
)
{
    return OcilibErrorGetString(err);
}

unsigned int OCI_API OCI_ErrorGetType
(
    OCI_Error* err
)
{
    return OcilibErrorGetType(err);
}

int OCI_API OCI_ErrorGetOCICode
(
    OCI_Error* err
)
{
    return OcilibErrorGetOCICode(err);
}

int OCI_API OCI_ErrorGetInternalCode
(
    OCI_Error* err
)
{
    return OcilibErrorGetInternalCode(err);
}

OCI_Connection* OCI_API OCI_ErrorGetConnection
(
    OCI_Error* err
)
{
    return OcilibErrorGetConnection(err);
}

OCI_Statement* OCI_API OCI_ErrorGetStatement
(
    OCI_Error* err
)
{
    return OcilibErrorGetStatement(err);
}

unsigned int OCI_API OCI_ErrorGetRow
(
    OCI_Error* err
)
{
    return OcilibErrorGetRow(err);
}

const otext* OCI_API OCI_ErrorGetLocation
(
    OCI_Error* err
)
{
    return OcilibErrorGetLocation(err);
}

/* --------------------------------------------------------------------------------------------- *
 * event
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_EventGetType
(
    OCI_Event* event
)
{
    CALL_IMPL(OcilibEventGetType, event)
}

unsigned int OCI_API OCI_EventGetOperation
(
    OCI_Event* event
)
{
    CALL_IMPL(OcilibEventGetOperation, event)
}

const otext* OCI_API OCI_EventGetDatabase
(
    OCI_Event* event
)
{
    CALL_IMPL(OcilibEventGetDatabase, event)
}

const otext* OCI_API OCI_EventGetObject
(
    OCI_Event* event
)
{
    CALL_IMPL(OcilibEventGetObject, event)
}

const otext* OCI_API OCI_EventGetRowid
(
    OCI_Event* event
)
{
    CALL_IMPL(OcilibEventGetRowid, event)
}

OCI_Subscription* OCI_API OCI_EventGetSubscription
(
    OCI_Event* event
)
{
    CALL_IMPL(OcilibEventGetSubscription, event)
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
    CALL_IMPL(OcilibFileCreate, con, type)
}

boolean OCI_API OCI_FileFree
(
    OCI_File* file
)
{
    CALL_IMPL(OcilibFileFree, file)
}

OCI_File** OCI_API OCI_FileArrayCreate
(
    OCI_Connection* con,
    unsigned int    type,
    unsigned int    nbelem
)
{
    CALL_IMPL(OcilibFileCreateArray, con, type, nbelem)
}

boolean OCI_API OCI_FileArrayFree
(
    OCI_File** files
)
{
    CALL_IMPL(OcilibFileFreeArray, files)
}

unsigned int OCI_API OCI_FileGetType
(
    OCI_File* file
)
{
    CALL_IMPL(OcilibFileGetType, file)
}

boolean OCI_API OCI_FileSeek
(
    OCI_File   * file,
    big_uint     offset,
    unsigned int mode
)
{
    CALL_IMPL(OcilibFileSeek, file, offset, mode)
}

big_uint OCI_API OCI_FileGetOffset
(
    OCI_File* file
)
{
    CALL_IMPL(OcilibFileGetOffset, file)
}

unsigned int OCI_API OCI_FileRead
(
    OCI_File   * file,
    void       * buffer,
    unsigned int len
)
{
    CALL_IMPL(OcilibFileRead, file, buffer, len)
}

big_uint OCI_API OCI_FileGetSize
(
    OCI_File* file
)
{
    CALL_IMPL(OcilibFileGetSize, file)
}

boolean OCI_API OCI_FileExists
(
    OCI_File* file
)
{
    CALL_IMPL(OcilibFileExists, file)
}

boolean OCI_API OCI_FileSetName
(
    OCI_File   * file,
    const otext* dir,
    const otext* name
)
{
    CALL_IMPL(OcilibFileSetName, file, dir, name)
}

const otext* OCI_API OCI_FileGetDirectory
(
    OCI_File* file
)
{
    CALL_IMPL(OcilibFileGetDirectory, file)
}

const otext* OCI_API OCI_FileGetName
(
    OCI_File* file
)
{
    CALL_IMPL(OcilibFileGetName, file)
}

boolean OCI_API OCI_FileOpen
(
    OCI_File* file
)
{
    CALL_IMPL(OcilibFileOpen, file)
}

boolean OCI_API OCI_FileIsOpen
(
    OCI_File* file
)
{
    CALL_IMPL(OcilibFileIsOpen, file)
}

boolean OCI_API OCI_FileClose
(
    OCI_File* file
)
{
    CALL_IMPL(OcilibFileClose, file)
}

boolean OCI_API OCI_FileIsEqual
(
    OCI_File* file,
    OCI_File* file2
)
{
    CALL_IMPL(OcilibFileIsEqual, file, file2)
}

boolean OCI_API OCI_FileAssign
(
    OCI_File* file,
    OCI_File* file_src
)
{
    CALL_IMPL(OcilibFileAssign, file, file_src)
}

OCI_Connection* OCI_API OCI_FileGetConnection
(
    OCI_File* file
)
{
    CALL_IMPL(OcilibFileGetConnection, file)
}

/* --------------------------------------------------------------------------------------------- *
 * handle
 * --------------------------------------------------------------------------------------------- */

const void* OCI_API OCI_HandleGetEnvironment
(
    void
)
{
    CALL_IMPL_NO_ARGS(OcilibHandleGetEnvironment)
}

const void* OCI_API OCI_HandleGetContext
(
    OCI_Connection* con
)
{
    CALL_IMPL(OcilibHandleGetContext, con)
}

const void* OCI_API OCI_HandleGetServer
(
    OCI_Connection* con
)
{
    CALL_IMPL(OcilibHandleGetServer, con)
}

const void* OCI_API OCI_HandleGetError
(
    OCI_Connection* con
)
{
    CALL_IMPL(OcilibHandleGetError, con)
}

const void* OCI_API OCI_HandleGetSession
(
    OCI_Connection* con
)
{
    CALL_IMPL(OcilibHandleGetSession, con)
}

const void* OCI_API OCI_HandleGetTransaction
(
    OCI_Transaction* trans
)
{
    CALL_IMPL(OcilibHandleGetTransaction, trans)
}

const void* OCI_API OCI_HandleGetStatement
(
    OCI_Statement* stmt
)
{
    CALL_IMPL(OcilibHandleGetStatement, stmt)
}

const void* OCI_API OCI_HandleGetLob
(
    OCI_Lob* lob
)
{
    CALL_IMPL(OcilibHandleGetLob, lob)
}

const void* OCI_API OCI_HandleGetFile
(
    OCI_File* file
)
{
    CALL_IMPL(OcilibHandleGetFile, file)
}

const void* OCI_API OCI_HandleGetDate
(
    OCI_Date* date
)
{
    CALL_IMPL(OcilibHandleGetDate, date)
}

const void* OCI_API OCI_HandleGetTimestamp
(
    OCI_Timestamp* tmsp
)
{
    CALL_IMPL(OcilibHandleGetTimestamp, tmsp)
}

const void* OCI_API OCI_HandleGetInterval
(
    OCI_Interval* itv
)
{
    CALL_IMPL(OcilibHandleGetInterval, itv)
}

const void* OCI_API OCI_HandleGetObject
(
    OCI_Object* obj
)
{
    CALL_IMPL(OcilibHandleGetObject, obj)
}

const void* OCI_API OCI_HandleGetColl
(
    OCI_Coll* coll
)
{
    CALL_IMPL(OcilibHandleGetColl, coll)
}

const void* OCI_API OCI_HandleGetRef
(
    OCI_Ref* ref
)
{
    CALL_IMPL(OcilibHandleGetReference, ref)
}

const void* OCI_API OCI_HandleGetMutex
(
    OCI_Mutex* mutex
)
{
    CALL_IMPL(OcilibHandleGetMutex, mutex)
}

const void* OCI_API OCI_HandleGetThreadID
(
    OCI_Thread* thread
)
{
    CALL_IMPL(OcilibHandleGetThreadID, thread)
}

const void* OCI_API OCI_HandleGetThread
(
    OCI_Thread* thread
)
{
    CALL_IMPL(OcilibHandleGetThread, thread)
}

const void* OCI_API OCI_HandleGetDirPathCtx
(
    OCI_DirPath* dp
)
{
    CALL_IMPL(OcilibHandleGetDirPathCtx, dp)
}

const void* OCI_API OCI_HandleGetDirPathColArray
(
    OCI_DirPath* dp
)
{
    CALL_IMPL(OcilibHandleGetDirPathColArray, dp)
}

const void* OCI_API OCI_HandleGetDirPathStream
(
    OCI_DirPath* dp
)
{
    CALL_IMPL(OcilibHandleGetDirPathStream, dp)
}

const void* OCI_API OCI_HandleGetSubscription
(
    OCI_Subscription* sub
)
{
    CALL_IMPL(OcilibHandleGetSubscription, sub)
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
    CALL_IMPL(OcilibHashCreate, size, type)
}

boolean OCI_API OCI_HashFree
(
    OCI_HashTable* table
)
{
    CALL_IMPL(OcilibHashFree, table)
}

unsigned int OCI_API OCI_HashGetSize
(
    OCI_HashTable* table
)
{
    CALL_IMPL(OcilibHashGetSize, table)
}

unsigned int OCI_API OCI_HashGetType
(
    OCI_HashTable* table
)
{
    CALL_IMPL(OcilibHashGetType, table)
}

boolean OCI_API OCI_HashAddString
(
    OCI_HashTable* table,
    const otext  * key,
    const otext  * value
)
{
    CALL_IMPL(OcilibHashAddString, table, key, value)
}

const otext* OCI_API OCI_HashGetString
(
    OCI_HashTable* table,
    const otext  * key
)
{
    CALL_IMPL(OcilibHashGetString, table, key)
}

boolean OCI_API OCI_HashAddInt
(
    OCI_HashTable* table,
    const otext  * key,
    int            value
)
{
    CALL_IMPL(OcilibHashAddInt, table, key, value)
}

int OCI_API OCI_HashGetInt
(
    OCI_HashTable* table,
    const otext  * key
)
{
    CALL_IMPL(OcilibHashGetInt, table, key)
}

boolean OCI_API OCI_HashAddPointer
(
    OCI_HashTable* table,
    const otext  * key,
    void         * value
)
{
    CALL_IMPL(OcilibHashAddPointer, table, key, value)
}

void* OCI_API OCI_HashGetPointer
(
    OCI_HashTable* table,
    const otext  * key
)
{
    CALL_IMPL(OcilibHashGetPointer, table, key)
}

OCI_HashEntry* OCI_API OCI_HashLookup
(
    OCI_HashTable* table,
    const otext  * key,
    boolean        create
)
{
    CALL_IMPL(OcilibHashLookup, table, key, create)
}

OCI_HashValue* OCI_API OCI_HashGetValue
(
    OCI_HashTable* table,
    const otext  * key
)
{
    CALL_IMPL(OcilibHashGetValue, table, key)
}

OCI_HashEntry* OCI_API OCI_HashGetEntry
(
    OCI_HashTable* table,
    unsigned int   index
)
{
    CALL_IMPL(OcilibHashGetEntry, table, index)
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
    CALL_IMPL(OcilibIntervalCreate, con, type)
}

boolean OCI_API OCI_IntervalFree
(
    OCI_Interval* itv
)
{
    CALL_IMPL(OcilibIntervalFree, itv)
}

OCI_Interval** OCI_API OCI_IntervalArrayCreate
(
    OCI_Connection* con,
    unsigned int    type,
    unsigned int    nbelem
)
{
    CALL_IMPL(OcilibIntervalCreateArray, con, type, nbelem)
}

boolean OCI_API OCI_IntervalArrayFree
(
    OCI_Interval** itvs
)
{
    CALL_IMPL(OcilibIntervalFreeArray, itvs)
}

unsigned int OCI_API OCI_IntervalGetType
(
    OCI_Interval* itv
)
{
    CALL_IMPL(OcilibIntervalGetType, itv)
}

boolean OCI_API OCI_IntervalAssign
(
    OCI_Interval* itv,
    OCI_Interval* itv_src
)
{
    CALL_IMPL(OcilibIntervalAssign, itv, itv_src)
}

int OCI_API OCI_IntervalCheck
(
    OCI_Interval* itv
)
{
    CALL_IMPL(OcilibIntervalCheck, itv)
}

int OCI_API OCI_IntervalCompare
(
    OCI_Interval* itv,
    OCI_Interval* itv2
)
{
    CALL_IMPL(OcilibIntervalCompare, itv, itv2)
}

boolean OCI_API OCI_IntervalFromText
(
    OCI_Interval* itv,
    const otext * str
)
{
    CALL_IMPL(OcilibIntervalFromString, itv, str)
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
    CALL_IMPL(OcilibIntervalToString, itv, leading_prec, fraction_prec, size, str)
}

boolean OCI_API OCI_IntervalFromTimeZone
(
    OCI_Interval* itv,
    const otext * str
)
{
    CALL_IMPL(OcilibIntervalFromTimeZone, itv, str)
}

boolean OCI_API OCI_IntervalGetDaySecond
(
    OCI_Interval* itv,
    int         * day,
    int         * hour,
    int         * min,
    int         * sec,
    int         * nsec
)
{
    CALL_IMPL(OcilibIntervalGetDaySecond, itv, day, hour, min, sec, nsec)
}

boolean OCI_API OCI_IntervalGetYearMonth
(
    OCI_Interval* itv,
    int         * year,
    int         * month
)
{
    CALL_IMPL(OcilibIntervalGetYearMonth, itv, year, month)
}

boolean OCI_API OCI_IntervalSetDaySecond
(
    OCI_Interval* itv,
    int           day,
    int           hour,
    int           min,
    int           sec,
    int           nsec
)
{
    CALL_IMPL(OcilibIntervalSetDaySecond, itv, day, hour, min, sec, nsec)
}

boolean OCI_API OCI_IntervalSetYearMonth
(
    OCI_Interval* itv,
    int           year,
    int           month
)
{
    CALL_IMPL(OcilibIntervalSetYearMonth, itv, year, month)
}

boolean OCI_API OCI_IntervalAdd
(
    OCI_Interval* itv,
    OCI_Interval* itv2
)
{
    CALL_IMPL(OcilibIntervalAdd, itv, itv2)
}

boolean OCI_API OCI_IntervalSubtract
(
    OCI_Interval* itv,
    OCI_Interval* itv2
)
{
    CALL_IMPL(OcilibIntervalSubtract, itv, itv2)
}

/* --------------------------------------------------------------------------------------------- *
 * iterator
 * --------------------------------------------------------------------------------------------- */

OCI_Iter* OCI_API OCI_IterCreate
(
    OCI_Coll* coll
)
{
    CALL_IMPL(OcilibIteratorCreate, coll)
}

boolean OCI_API OCI_IterFree
(
    OCI_Iter* iter
)
{
    CALL_IMPL(OcilibIteratorFree, iter)
}

OCI_Elem* OCI_API OCI_IterGetNext
(
    OCI_Iter* iter
)
{
    CALL_IMPL(OcilibIteratorGetNext, iter)
}

OCI_Elem* OCI_API OCI_IterGetPrev
(
    OCI_Iter* iter
)
{
    CALL_IMPL(OcilibIteratorGetPrev, iter)
}

OCI_Elem* OCI_API OCI_IterGetCurrent
(
    OCI_Iter* iter
)
{
    CALL_IMPL(OcilibIteratorGetCurrent, iter)
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
    CALL_IMPL(OcilibEnvironmentInitialize, err_handler, lib_path, mode)
}

boolean OCI_API OCI_Cleanup
(
    void
)
{
    CALL_IMPL_NO_ARGS(OcilibEnvironmentCleanup)
}

unsigned int OCI_API OCI_GetOCICompileVersion
(
    void
)
{
    CALL_IMPL_NO_ARGS(OcilibEnvironmentGetOCICompileVersion)
}

unsigned int OCI_API OCI_GetOCIRuntimeVersion
(
    void
)
{
    CALL_IMPL_NO_ARGS(OcilibEnvironmentGetOCIRuntimeVersion)
}

unsigned int OCI_API OCI_GetImportMode
(
    void
)
{
    CALL_IMPL_NO_ARGS(OcilibEnvironmentGetOCIRuntimeImportMode)
}

unsigned int OCI_API OCI_GetCharset
(
    void
)
{
    CALL_IMPL_NO_ARGS(OcilibEnvironmentGetCharset)
}

big_uint OCI_API OCI_GetAllocatedBytes
(
    unsigned int mem_type
)
{
    CALL_IMPL(OcilibEnvironmentGetAllocatedBytes, mem_type)
}

OCI_Error* OCI_API OCI_GetLastError
(
    void
)
{
    return OcilibEnvironmentGetLastError();
}

boolean OCI_API OCI_EnableWarnings
(
    boolean value
)
{
    CALL_IMPL(OcilibEnvironmentEnableWarnings, value)
}

boolean OCI_API OCI_SetErrorHandler
(
    POCI_ERROR handler
)
{
    CALL_IMPL(OcilibEnvironmentSetErrorHandler, handler)
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
    CALL_IMPL(OcilibDatabaseStartup, db, user, pwd, sess_mode, start_mode, start_flag, spfile)
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
    CALL_IMPL(OcilibDatabaseShutdown, db, user, pwd, sess_mode, shut_mode, shut_flag)
}

boolean OCI_API OCI_SetHAHandler
(
    POCI_HA_HANDLER handler
)
{
    CALL_IMPL(OcilibEnvironmentSetHAHandler, handler)
}

boolean OCI_API OCI_SetFormat
(
    OCI_Connection* con,
    unsigned int    type,
    const otext   * format
)
{
    CALL_IMPL(OcilibEnvironmentSetFormat, con, type, format)
}

const otext* OCI_API OCI_GetFormat
(
    OCI_Connection* con,
    unsigned int    type
)
{
    CALL_IMPL(OcilibEnvironmentGetFormat, con, type)
}

boolean OCI_API OCI_SetUserPassword
(
    const otext* db,
    const otext* user,
    const otext* pwd,
    const otext* new_pwd
)
{
    CALL_IMPL(OcilibDatabaseSetUserPassword, db, user, pwd, new_pwd);
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
    CALL_IMPL(OcilibLobCreate, con, type);
}

boolean OCI_API OCI_LobFree
(
    OCI_Lob* lob
)
{
    CALL_IMPL(OcilibLobFree, lob);
}

OCI_Lob** OCI_API OCI_LobArrayCreate
(
    OCI_Connection* con,
    unsigned int    type,
    unsigned int    nbelem
)
{
    CALL_IMPL(OcilibLobCreateArray, con, type, nbelem);
}

boolean OCI_API OCI_LobArrayFree
(
    OCI_Lob** lobs
)
{
    CALL_IMPL(OcilibLobFreeArray, lobs);
}

unsigned int OCI_API OCI_LobGetType
(
    OCI_Lob* lob
)
{
    CALL_IMPL(OcilibLobGetType, lob);
}

boolean OCI_API OCI_LobSeek
(
    OCI_Lob    * lob,
    big_uint     offset,
    unsigned int mode
)
{
    CALL_IMPL(OcilibLobSeek, lob, offset, mode);
}

big_uint OCI_API OCI_LobGetOffset
(
    OCI_Lob* lob
)
{
    CALL_IMPL(OcilibLobGetOffset, lob);
}

unsigned int OCI_API OCI_LobRead
(
    OCI_Lob    * lob,
    void       * buffer,
    unsigned int len
)
{
    CALL_IMPL(OcilibLobRead, lob, buffer, len);
}

boolean OCI_API OCI_LobRead2
(
    OCI_Lob     * lob,
    void        * buffer,
    unsigned int* char_count,
    unsigned int* byte_count
)
{
    CALL_IMPL(OcilibLobRead2, lob, buffer, char_count, byte_count);
}

unsigned int OCI_API OCI_LobWrite
(
    OCI_Lob    * lob,
    void       * buffer,
    unsigned int len
)
{
    CALL_IMPL(OcilibLobWrite, lob, buffer, len);
}

boolean OCI_API OCI_LobWrite2
(
    OCI_Lob     * lob,
    void        * buffer,
    unsigned int* char_count,
    unsigned int* byte_count
)
{
    CALL_IMPL(OcilibLobWrite2, lob, buffer, char_count, byte_count);
}

boolean OCI_API OCI_LobTruncate
(
    OCI_Lob* lob,
    big_uint size
)
{
    CALL_IMPL(OcilibLobTruncate, lob, size);
}

big_uint OCI_API OCI_LobGetLength
(
    OCI_Lob* lob
)
{
    CALL_IMPL(OcilibLobGetLength, lob);
}

unsigned int OCI_API OCI_LobGetChunkSize
(
    OCI_Lob* lob
)
{
    CALL_IMPL(OcilibLobGetChunkSize, lob);
}

big_uint OCI_API OCI_LobErase
(
    OCI_Lob* lob,
    big_uint offset,
    big_uint len
)
{
    CALL_IMPL(OcilibLobErase, lob, offset, len);
}

unsigned int OCI_API OCI_LobAppend
(
    OCI_Lob    * lob,
    void       * buffer,
    unsigned int len
)
{
    CALL_IMPL(OcilibLobAppend, lob, buffer, len);
}

boolean OCI_API OCI_LobAppend2
(
    OCI_Lob     * lob,
    void        * buffer,
    unsigned int* char_count,
    unsigned int* byte_count
)
{
    CALL_IMPL(OcilibLobAppend2, lob, buffer, char_count, byte_count);
}

boolean OCI_API OCI_LobAppendLob
(
    OCI_Lob* lob,
    OCI_Lob* lob_src
)
{
    CALL_IMPL(OcilibLobAppendLob, lob, lob_src);
}

boolean OCI_API OCI_LobIsTemporary
(
    OCI_Lob* lob
)
{
    CALL_IMPL(OcilibLobIsTemporary, lob);
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
    CALL_IMPL(OcilibLobCopy, lob, lob_src, offset_dst, offset_src, count);
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
    CALL_IMPL(OcilibLobCopyFromFile, lob, file, offset_dst, offset_src, count);
}

boolean OCI_API OCI_LobOpen
(
    OCI_Lob    * lob,
    unsigned int mode
)
{
    CALL_IMPL(OcilibLobOpen, lob, mode);
}

boolean OCI_API OCI_LobClose
(
    OCI_Lob* lob
)
{
    CALL_IMPL(OcilibLobClose, lob);
}

boolean OCI_API OCI_LobIsEqual
(
    OCI_Lob* lob,
    OCI_Lob* lob2
)
{
    CALL_IMPL(OcilibLobIsEqual, lob, lob2);
}

boolean OCI_API OCI_LobAssign
(
    OCI_Lob* lob,
    OCI_Lob* lob_src
)
{
    CALL_IMPL(OcilibLobAssign, lob, lob_src);
}

big_uint OCI_API OCI_LobGetMaxSize
(
    OCI_Lob* lob
)
{
    CALL_IMPL(OcilibLobGetMaxSize, lob);
}

boolean OCI_API OCI_LobFlush
(
    OCI_Lob* lob
)
{
    CALL_IMPL(OcilibLobFlush, lob);
}

boolean OCI_API OCI_LobEnableBuffering
(
    OCI_Lob* lob,
    boolean  value
)
{
    CALL_IMPL(OcilibLobEnableBuffering, lob, value);
}

OCI_Connection* OCI_API OCI_LobGetConnection
(
    OCI_Lob* lob
)
{
    CALL_IMPL(OcilibLobGetConnection, lob);
}

boolean OCI_API OCI_LobIsRemote
(
    OCI_Lob* lob
)
{
    CALL_IMPL(OcilibLobIsRemote, lob);
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
    CALL_IMPL(OcilibLongCreate, stmt, type);
}

boolean OCI_API OCI_LongFree
(
    OCI_Long* lg
)
{
    CALL_IMPL(OcilibLongFree, lg);
}

unsigned int OCI_API OCI_LongGetType
(
    OCI_Long* lg
)
{
    CALL_IMPL(OcilibLongGetType, lg);
}

unsigned int OCI_API OCI_LongRead
(
    OCI_Long   * lg,
    void       * buffer,
    unsigned int len
)
{
    CALL_IMPL(OcilibLongRead, lg, buffer, len);
}

unsigned int OCI_API OCI_LongWrite
(
    OCI_Long   * lg,
    void       * buffer,
    unsigned int len
)
{
    CALL_IMPL(OcilibLongWrite, lg, buffer, len);
}

unsigned int OCI_API OCI_LongGetSize
(
    OCI_Long* lg
)
{
    CALL_IMPL(OcilibLongGetSize, lg);
}

void* OCI_API OCI_LongGetBuffer
(
    OCI_Long* lg
)
{
    CALL_IMPL(OcilibLongGetBuffer, lg);
}

/* --------------------------------------------------------------------------------------------- *
 *  msg
 * --------------------------------------------------------------------------------------------- */

OCI_Msg* OCI_API OCI_MsgCreate
(
    OCI_TypeInfo* typinf
)
{
    CALL_IMPL(OcilibMessageCreate, typinf);
}

boolean OCI_API OCI_MsgFree
(
    OCI_Msg* msg
)
{
    CALL_IMPL(OcilibMessageFree, msg);
}

boolean OCI_API OCI_MsgReset
(
    OCI_Msg* msg
)
{
    CALL_IMPL(OcilibMessageReset, msg);
}

OCI_Object* OCI_API OCI_MsgGetObject
(
    OCI_Msg* msg
)
{
    CALL_IMPL(OcilibMessageGetObject, msg);
}

boolean OCI_API OCI_MsgSetObject
(
    OCI_Msg   * msg,
    OCI_Object* obj
)
{
    CALL_IMPL(OcilibMessageSetObject, msg, obj);
}

boolean OCI_API OCI_MsgGetRaw
(
    OCI_Msg     * msg,
    void        * raw,
    unsigned int* size
)
{
    CALL_IMPL(OcilibMessageGetRaw, msg, raw, size);
}

boolean OCI_API OCI_MsgSetRaw
(
    OCI_Msg    * msg,
    const void * raw,
    unsigned int size
)
{
    CALL_IMPL(OcilibMessageSetRaw, msg, raw, size);
}

int OCI_API OCI_MsgGetAttemptCount
(
    OCI_Msg* msg
)
{
    CALL_IMPL(OcilibMessageGetAttemptCount, msg);
}

int OCI_API OCI_MsgGetEnqueueDelay
(
    OCI_Msg* msg
)
{
    CALL_IMPL(OcilibMessageGetEnqueueDelay, msg);
}

boolean OCI_API OCI_MsgSetEnqueueDelay
(
    OCI_Msg* msg,
    int      value
)
{
    CALL_IMPL(OcilibMessageSetEnqueueDelay, msg, value);
}

OCI_Date* OCI_API OCI_MsgGetEnqueueTime
(
    OCI_Msg* msg
)
{
    CALL_IMPL(OcilibMessageGetEnqueueTime, msg);
}

int OCI_API OCI_MsgGetExpiration
(
    OCI_Msg* msg
)
{
    CALL_IMPL(OcilibMessageGetExpiration, msg);
}

boolean OCI_API OCI_MsgSetExpiration
(
    OCI_Msg* msg,
    int      value
)
{
    CALL_IMPL(OcilibMessageSetExpiration, msg, value);
}

unsigned int OCI_API OCI_MsgGetState
(
    OCI_Msg* msg
)
{
    CALL_IMPL(OcilibMessageGetState, msg);
}

int OCI_API OCI_MsgGetPriority
(
    OCI_Msg* msg
)
{
    CALL_IMPL(OcilibMessageGetPriority, msg);
}

boolean OCI_API OCI_MsgSetPriority
(
    OCI_Msg* msg,
    int      value
)
{
    CALL_IMPL(OcilibMessageSetPriority, msg, value);
}

boolean OCI_API OCI_MsgGetID
(
    OCI_Msg     * msg,
    void        * id,
    unsigned int* len
)
{
    CALL_IMPL(OcilibMessageGetID, msg, id, len);
}

boolean OCI_API OCI_MsgGetOriginalID
(
    OCI_Msg     * msg,
    void        * id,
    unsigned int* len
)
{
    CALL_IMPL(OcilibMessageGetOriginalID, msg, id, len);
}

boolean OCI_API OCI_MsgSetOriginalID
(
    OCI_Msg    * msg,
    const void * id,
    unsigned int len
)
{
    CALL_IMPL(OcilibMessageSetOriginalID, msg, id, len);
}

OCI_Agent* OCI_API OCI_MsgGetSender
(
    OCI_Msg* msg
)
{
    CALL_IMPL(OcilibMessageGetSender, msg);
}

boolean OCI_API OCI_MsgSetSender
(
    OCI_Msg  * msg,
    OCI_Agent* sender
)
{
    CALL_IMPL(OcilibMessageSetSender, msg, sender);
}

boolean OCI_API OCI_MsgSetConsumers
(
    OCI_Msg    * msg,
    OCI_Agent ** consumers,
    unsigned int count
)
{
    CALL_IMPL(OcilibMessageSetConsumers, msg, consumers, count);
}

const otext* OCI_API OCI_MsgGetCorrelation
(
    OCI_Msg* msg
)
{
    CALL_IMPL(OcilibMessageGetCorrelation, msg);
}

boolean OCI_API OCI_MsgSetCorrelation
(
    OCI_Msg    * msg,
    const otext* correlation
)
{
    CALL_IMPL(OcilibMessageSetCorrelation, msg, correlation);
}

const otext* OCI_API OCI_MsgGetExceptionQueue
(
    OCI_Msg* msg
)
{
    CALL_IMPL(OcilibMessageGetExceptionQueue, msg);
}

boolean OCI_API OCI_MsgSetExceptionQueue
(
    OCI_Msg    * msg,
    const otext* queue
)
{
    CALL_IMPL(OcilibMessageSetExceptionQueue, msg, queue);
}

/* --------------------------------------------------------------------------------------------- *
 *  mutex
 * --------------------------------------------------------------------------------------------- */

OCI_Mutex* OCI_API OCI_MutexCreate
(
    void
)
{
    CALL_IMPL_NO_ARGS(OcilibMutexCreate);
}

boolean OCI_API OCI_MutexFree
(
    OCI_Mutex* mutex
)
{
    CALL_IMPL(OcilibMutexFree, mutex);
}

boolean OCI_API OCI_MutexAcquire
(
    OCI_Mutex* mutex
)
{
    CALL_IMPL(OcilibMutexAcquire, mutex);
}

boolean OCI_API OCI_MutexRelease
(
    OCI_Mutex* mutex
)
{
    CALL_IMPL(OcilibMutexRelease, mutex);
}

/* --------------------------------------------------------------------------------------------- *
 *  number
 * --------------------------------------------------------------------------------------------- */

OCI_Number* OCI_API OCI_NumberCreate
(
    OCI_Connection* con
)
{
    CALL_IMPL(OcilibNumberCreate, con);
}

boolean OCI_API OCI_NumberFree
(
    OCI_Number* number
)
{
    CALL_IMPL(OcilibNumberFree, number);
}

OCI_Number** OCI_API OCI_NumberArrayCreate
(
    OCI_Connection* con,
    unsigned int    nbelem
)
{
    CALL_IMPL(OcilibNumberCreateArray, con, nbelem);
}

boolean OCI_API OCI_NumberArrayFree
(
    OCI_Number** numbmers
)
{
    CALL_IMPL(OcilibNumberFreeArray, numbmers);
}

boolean OCI_API OCI_NumberAssign
(
    OCI_Number* number,
    OCI_Number* number_src
)
{
    CALL_IMPL(OcilibNumberAssign, number, number_src);
}

boolean OCI_API OCI_NumberToText
(
    OCI_Number * number,
    const otext* fmt,
    int          size,
    otext      * str
)
{
    CALL_IMPL(OcilibNumberToString, number, fmt, size, str);
}

boolean OCI_API OCI_NumberFromText
(
    OCI_Number * number,
    const otext* str,
    const otext* fmt
)
{
    CALL_IMPL(OcilibNumberFromString, number, str, fmt);
}

unsigned char* OCI_API OCI_NumberGetContent
(
    OCI_Number* number
)
{
    CALL_IMPL(OcilibNumberGetContent, number);
}

boolean OCI_API OCI_NumberSetContent
(
    OCI_Number   * number,
    unsigned char* content
)
{
    CALL_IMPL(OcilibNumberSetContent, number, content);
}

boolean OCI_API OCI_NumberSetValue
(
    OCI_Number * number,
    unsigned int type,
    void       * value
)
{
    CALL_IMPL(OcilibNumberSetValue, number, type, value);
}

boolean OCI_API OCI_NumberGetValue
(
    OCI_Number * number,
    unsigned int type,
    void       * value
)
{
    CALL_IMPL(OcilibNumberGetValue, number, type, value);
}

boolean OCI_API OCI_NumberAdd
(
    OCI_Number * number,
    unsigned int type,
    void       * value
)
{
    CALL_IMPL(OcilibNumberAdd, number, type, value);
}

boolean OCI_API OCI_NumberSub
(
    OCI_Number * number,
    unsigned int type,
    void       * value
)
{
    CALL_IMPL(OcilibNumberSub, number, type, value);
}

boolean OCI_API OCI_NumberMultiply
(
    OCI_Number * number,
    unsigned int type,
    void       * value
)
{
    CALL_IMPL(OcilibNumberMultiply, number, type, value);
}

boolean OCI_API OCI_NumberDivide
(
    OCI_Number * number,
    unsigned int type,
    void       * value
)
{
    CALL_IMPL(OcilibNumberDivide, number, type, value);
}

int OCI_API OCI_NumberCompare
(
    OCI_Number* number1,
    OCI_Number* number2
)
{
    CALL_IMPL(OcilibNumberCompare, number1, number2);
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
    CALL_IMPL(OcilibObjectCreate, con, typinf);
}

boolean OCI_API OCI_ObjectFree
(
    OCI_Object* obj
)
{
    CALL_IMPL(OcilibObjectFree, obj);
}

OCI_Object** OCI_API OCI_ObjectArrayCreate
(
    OCI_Connection* con,
    OCI_TypeInfo  * typinf,
    unsigned int    nbelem
)
{
    CALL_IMPL(OcilibObjectCreateArray, con,typinf, nbelem);
}

boolean OCI_API OCI_ObjectArrayFree
(
    OCI_Object** objs
)
{
    CALL_IMPL(OcilibObjectFreeArray, objs);
}

boolean OCI_API OCI_ObjectAssign
(
    OCI_Object* obj,
    OCI_Object* obj_src
)
{
    CALL_IMPL(OcilibObjectAssign, obj, obj_src);
}

unsigned int OCI_API OCI_ObjectGetType
(
    OCI_Object* obj
)
{
    CALL_IMPL(OcilibObjectGetType, obj);
}

boolean OCI_API OCI_ObjectGetSelfRef
(
    OCI_Object* obj,
    OCI_Ref   * ref
)
{
    CALL_IMPL(OcilibObjectGetSelfRef, obj, ref);
}

OCI_TypeInfo* OCI_API OCI_ObjectGetTypeInfo
(
    OCI_Object* obj
)
{
    CALL_IMPL(OcilibObjectGetTypeInfo, obj);
}

boolean OCI_API OCI_ObjectGetBoolean
(
    OCI_Object * obj,
    const otext* attr
)
{
    CALL_IMPL(OcilibObjectGetBoolean, obj, attr);
}

OCI_Number* OCI_API OCI_ObjectGetNumber
(
    OCI_Object * obj,
    const otext* attr
)
{
    CALL_IMPL(OcilibObjectGetNumber, obj, attr);
}

short OCI_API OCI_ObjectGetShort
(
    OCI_Object * obj,
    const otext* attr
)
{
    CALL_IMPL(OcilibObjectGetShort, obj, attr);
}

unsigned short OCI_API OCI_ObjectGetUnsignedShort
(
    OCI_Object * obj,
    const otext* attr
)
{
    CALL_IMPL(OcilibObjectGetUnsignedShort, obj, attr);
}

int OCI_API OCI_ObjectGetInt
(
    OCI_Object * obj,
    const otext* attr
)
{
    CALL_IMPL(OcilibObjectGetInt, obj, attr);
}

unsigned int OCI_API OCI_ObjectGetUnsignedInt
(
    OCI_Object * obj,
    const otext* attr
)
{
    CALL_IMPL(OcilibObjectGetUnsignedInt, obj, attr);
}

big_int OCI_API OCI_ObjectGetBigInt
(
    OCI_Object * obj,
    const otext* attr
)
{
    CALL_IMPL(OcilibObjectGetBigInt, obj, attr);
}

big_uint OCI_API OCI_ObjectGetUnsignedBigInt
(
    OCI_Object * obj,
    const otext* attr
)
{
    CALL_IMPL(OcilibObjectGetUnsignedBigInt, obj, attr);
}

double OCI_API OCI_ObjectGetDouble
(
    OCI_Object * obj,
    const otext* attr
)
{
    CALL_IMPL(OcilibObjectGetDouble, obj, attr);
}

float OCI_API OCI_ObjectGetFloat
(
    OCI_Object * obj,
    const otext* attr
)
{
    CALL_IMPL(OcilibObjectGetFloat, obj, attr);
}

const otext* OCI_API OCI_ObjectGetString
(
    OCI_Object * obj,
    const otext* attr
)
{
    CALL_IMPL(OcilibObjectGetString, obj, attr);
}

int OCI_API OCI_ObjectGetRaw
(
    OCI_Object * obj,
    const otext* attr,
    void       * value,
    unsigned int len
)
{
    CALL_IMPL(OcilibObjectGetRaw, obj, attr, value, len);
}

unsigned int OCI_API OCI_ObjectGetRawSize
(
    OCI_Object * obj,
    const otext* attr
)
{
    CALL_IMPL(OcilibObjectGetRawSize, obj, attr);
}

OCI_Date* OCI_API OCI_ObjectGetDate
(
    OCI_Object * obj,
    const otext* attr
)
{
    CALL_IMPL(OcilibObjectGetDate, obj, attr);
}

OCI_Timestamp* OCI_API OCI_ObjectGetTimestamp
(
    OCI_Object * obj,
    const otext* attr
)
{
    CALL_IMPL(OcilibObjectGetTimestamp, obj, attr);
}

OCI_Interval* OCI_API OCI_ObjectGetInterval
(
    OCI_Object * obj,
    const otext* attr
)
{
    CALL_IMPL(OcilibObjectGetInterval, obj, attr);
}

OCI_Coll* OCI_API OCI_ObjectGetColl
(
    OCI_Object * obj,
    const otext* attr)
{
    CALL_IMPL(OcilibObjectGetColl, obj, attr);
}

OCI_Ref* OCI_API OCI_ObjectGetRef
(
    OCI_Object * obj,
    const otext* attr
)
{
    CALL_IMPL(OcilibObjectGetReference, obj, attr);
}

OCI_Object* OCI_API OCI_ObjectGetObject
(
    OCI_Object * obj,
    const otext* attr
)
{
    CALL_IMPL(OcilibObjectGetObject, obj, attr);
}

OCI_Lob* OCI_API OCI_ObjectGetLob
(
    OCI_Object * obj,
    const otext* attr
)
{
    CALL_IMPL(OcilibObjectGetLob, obj, attr);
}

OCI_File* OCI_API OCI_ObjectGetFile
(
    OCI_Object * obj,
    const otext* attr
)
{
    CALL_IMPL(OcilibObjectGetFile, obj, attr);
}

boolean OCI_API OCI_ObjectSetBoolean
(
    OCI_Object * obj,
    const otext* attr,
    boolean      value
)
{
    CALL_IMPL(OcilibObjectSetBoolean, obj, attr, value);
}

boolean OCI_API OCI_ObjectSetNumber
(
    OCI_Object * obj,
    const otext* attr,
    OCI_Number * value
)
{
    CALL_IMPL(OcilibObjectSetNumber, obj, attr, value);
}

boolean OCI_API OCI_ObjectSetShort
(
    OCI_Object * obj,
    const otext* attr,
    short        value
)
{
    CALL_IMPL(OcilibObjectSetShort, obj, attr, value);
}

boolean OCI_API OCI_ObjectSetUnsignedShort
(
    OCI_Object   * obj,
    const otext  * attr,
    unsigned short value
)
{
    CALL_IMPL(OcilibObjectSetUnsignedShort, obj, attr, value);
}

boolean OCI_API OCI_ObjectSetInt
(
    OCI_Object * obj,
    const otext* attr,
    int          value
)
{
    CALL_IMPL(OcilibObjectSetInt, obj, attr, value);
}

boolean OCI_API OCI_ObjectSetUnsignedInt
(
    OCI_Object * obj,
    const otext* attr,
    unsigned int value
)
{
    CALL_IMPL(OcilibObjectSetUnsignedInt, obj, attr, value);
}

boolean OCI_API OCI_ObjectSetBigInt
(
    OCI_Object * obj,
    const otext* attr,
    big_int      value
)
{
    CALL_IMPL(OcilibObjectSetBigInt, obj, attr, value);
}

boolean OCI_API OCI_ObjectSetUnsignedBigInt
(
    OCI_Object * obj,
    const otext* attr,
    big_uint     value
)
{
    CALL_IMPL(OcilibObjectSetUnsignedBigInt, obj, attr, value);
}

boolean OCI_API OCI_ObjectSetDouble
(
    OCI_Object * obj,
    const otext* attr,
    double       value
)
{
    CALL_IMPL(OcilibObjectSetDouble, obj, attr, value);
}

boolean OCI_API OCI_ObjectSetFloat
(
    OCI_Object * obj,
    const otext* attr,
    float        value
)
{
    CALL_IMPL(OcilibObjectSetFloat, obj, attr, value);
}

boolean OCI_API OCI_ObjectSetString
(
    OCI_Object * obj,
    const otext* attr,
    const otext* value
)
{
    CALL_IMPL(OcilibObjectSetString, obj, attr, value);
}

boolean OCI_API OCI_ObjectSetRaw
(
    OCI_Object * obj,
    const otext* attr,
    void       * value,
    unsigned int len
)
{
    CALL_IMPL(OcilibObjectSetRaw, obj, attr, value, len);
}

boolean OCI_API OCI_ObjectSetDate
(
    OCI_Object * obj,
    const otext* attr,
    OCI_Date   * value
)
{
    CALL_IMPL(OcilibObjectSetDate, obj, attr, value);
}

boolean OCI_API OCI_ObjectSetTimestamp
(
    OCI_Object   * obj,
    const otext  * attr,
    OCI_Timestamp* value
)
{
    CALL_IMPL(OcilibObjectSetTimestamp, obj, attr, value);
}

boolean OCI_API OCI_ObjectSetInterval
(
    OCI_Object  * obj,
    const otext * attr,
    OCI_Interval* value
)
{
    CALL_IMPL(OcilibObjectSetInterval, obj, attr, value);
}

boolean OCI_API OCI_ObjectSetColl
(
    OCI_Object * obj,
    const otext* attr,
    OCI_Coll   * value
)
{
    CALL_IMPL(OcilibObjectSetColl, obj, attr, value);
}

boolean OCI_API OCI_ObjectSetObject
(
    OCI_Object * obj,
    const otext* attr,
    OCI_Object * value
)
{
    CALL_IMPL(OcilibObjectSetObject, obj, attr, value);
}

boolean OCI_API OCI_ObjectSetLob
(
    OCI_Object * obj,
    const otext* attr,
    OCI_Lob    * value
)
{
    CALL_IMPL(OcilibObjectSetLob, obj, attr, value);
}

boolean OCI_API OCI_ObjectSetFile
(
    OCI_Object * obj,
    const otext* attr,
    OCI_File   * value
)
{
    CALL_IMPL(OcilibObjectSetFile, obj, attr, value);
}

boolean OCI_API OCI_ObjectSetRef
(
    OCI_Object * obj,
    const otext* attr,
    OCI_Ref    * value
)
{
    CALL_IMPL(OcilibObjectSetReference, obj, attr, value);
}

boolean OCI_API OCI_ObjectIsNull
(
    OCI_Object * obj,
    const otext* attr
)
{
    CALL_IMPL(OcilibObjectIsNull, obj, attr);
}

boolean OCI_API OCI_ObjectSetNull
(
    OCI_Object * obj,
    const otext* attr
)
{
    CALL_IMPL(OcilibObjectSetNull, obj, attr);
}

boolean OCI_API OCI_ObjectGetStruct
(
    OCI_Object* obj,
    void     ** pp_struct,
    void     ** pp_ind
)
{
    CALL_IMPL(OcilibObjectGetStruct, obj, pp_struct, pp_ind);
}

boolean OCI_API OCI_ObjectToText
(
    OCI_Object  * obj,
    unsigned int* size,
    otext       * str
)
{
    CALL_IMPL(OcilibObjectToString, obj, size, str);
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
    CALL_IMPL(OcilibPoolCreate, db, user, pwd, type, mode, min_con, max_con, incr_con);
}

boolean OCI_API OCI_PoolFree
(
    OCI_Pool* pool
)
{
    CALL_IMPL(OcilibPoolFree, pool);
}

OCI_Connection* OCI_API OCI_PoolGetConnection
(
    OCI_Pool   * pool,
    const otext* tag
)
{
    CALL_IMPL(OcilibPoolGetConnection, pool, tag);
}

unsigned int OCI_API OCI_PoolGetTimeout
(
    OCI_Pool* pool
)
{
    CALL_IMPL(OcilibPoolGetTimeout, pool);
}

boolean OCI_API OCI_PoolSetTimeout
(
    OCI_Pool   * pool,
    unsigned int value
)
{
    CALL_IMPL(OcilibPoolSetTimeout, pool, value);
}

boolean OCI_API OCI_PoolGetNoWait
(
    OCI_Pool* pool
)
{
    CALL_IMPL(OcilibPoolGetNoWait, pool);
}

boolean OCI_API OCI_PoolSetNoWait
(
    OCI_Pool* pool,
    boolean   value
)
{
    CALL_IMPL(OcilibPoolSetNoWait, pool, value);
}

unsigned int OCI_API OCI_PoolGetBusyCount
(
    OCI_Pool* pool
)
{
    CALL_IMPL(OcilibPoolGetBusyCount, pool);
}

unsigned int OCI_API OCI_PoolGetOpenedCount
(
    OCI_Pool* pool
)
{
    CALL_IMPL(OcilibPoolGetOpenedCount, pool);
}

unsigned int OCI_API OCI_PoolGetMin
(
    OCI_Pool* pool
)
{
    CALL_IMPL(OcilibPoolGetMin, pool);
}

unsigned int OCI_API OCI_PoolGetMax
(
    OCI_Pool* pool
)
{
    CALL_IMPL(OcilibPoolGetMax, pool);
}

unsigned int OCI_API OCI_PoolGetIncrement
(
    OCI_Pool* pool
)
{
    CALL_IMPL(OcilibPoolGetIncrement, pool);
}

unsigned int OCI_API OCI_PoolGetStatementCacheSize
(
    OCI_Pool* pool
)
{
    CALL_IMPL(OcilibPoolGetStatementCacheSize, pool);
}

boolean OCI_API OCI_PoolSetStatementCacheSize
(
    OCI_Pool   * pool,
    unsigned int value
)
{
    CALL_IMPL(OcilibPoolSetStatementCacheSize, pool, value);
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
    CALL_IMPL(OcilibQueueCreate, con, queue_name, queue_table, queue_type, max_retries,
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
    CALL_IMPL(OcilibQueueAlter, con, queue_name, max_retries, retry_delay, retention_time, comment);
}

boolean OCI_API OCI_QueueDrop
(
    OCI_Connection* con,
    const otext   * queue_name
)
{
    CALL_IMPL(OcilibQueueDrop, con, queue_name);
}

boolean OCI_API OCI_QueueStart
(
    OCI_Connection* con,
    const otext   * queue_name,
    boolean         enqueue,
    boolean         dequeue
)
{
    CALL_IMPL(OcilibQueueStart, con, queue_name, enqueue, dequeue);
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
    CALL_IMPL(OcilibQueueStop, con, queue_name, enqueue, dequeue, wait);
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
    CALL_IMPL(OcilibQueueTableCreate, con, queue_table, queue_payload_type, storage_clause,
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
    CALL_IMPL(OcilibQueueTableAlter, con, queue_table, comment, primary_instance, secondary_instance);
}

boolean OCI_API OCI_QueueTableDrop
(
    OCI_Connection* con,
    const otext   * queue_table,
    boolean         force
)
{
    CALL_IMPL(OcilibQueueTableDrop, con, queue_table, force);
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
    CALL_IMPL(OcilibQueueTablePurge, con, queue_table, purge_condition, block, delivery_mode);
}

boolean OCI_API OCI_QueueTableMigrate
(
    OCI_Connection* con,
    const otext   * queue_table,
    const otext   * compatible
)
{
    CALL_IMPL(OcilibQueueTableMigrate, con, queue_table, compatible);
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
    CALL_IMPL(OcilibReferenceCreate, con, typinf);
}

boolean OCI_API OCI_RefFree
(
    OCI_Ref* ref
)
{
    CALL_IMPL(OcilibReferenceFree, ref);
}

OCI_Ref** OCI_API OCI_RefArrayCreate
(
    OCI_Connection* con,
    OCI_TypeInfo  * typinf,
    unsigned int    nbelem
)
{
    CALL_IMPL(OcilibReferenceCreateArray, con, typinf, nbelem);
}

boolean OCI_API OCI_RefArrayFree
(
    OCI_Ref** refs
)
{
    CALL_IMPL(OcilibReferenceFreeArray, refs);
}

boolean OCI_API OCI_RefAssign
(
    OCI_Ref* ref,
    OCI_Ref* ref_src
)
{
    CALL_IMPL(OcilibReferenceAssign, ref, ref_src);
}

OCI_TypeInfo* OCI_API OCI_RefGetTypeInfo
(
    OCI_Ref* ref
)
{
    CALL_IMPL(OcilibReferenceGetTypeInfo, ref);
}

OCI_Object* OCI_API OCI_RefGetObject
(
    OCI_Ref* ref
)
{
    CALL_IMPL(OcilibReferenceGetObject, ref);
}

boolean OCI_API OCI_RefIsNull
(
    OCI_Ref* ref
)
{
    CALL_IMPL(OcilibReferenceIsNull, ref);
}

boolean OCI_API OCI_RefSetNull
(
    OCI_Ref* ref
)
{
    CALL_IMPL(OcilibReferenceSetNull, ref);
}

unsigned int OCI_API OCI_RefGetHexSize
(
    OCI_Ref* ref
)
{
    CALL_IMPL(OcilibReferenceGetHexSize, ref);
}

boolean OCI_API OCI_RefToText
(
    OCI_Ref    * ref,
    unsigned int size,
    otext      * str
)
{
    CALL_IMPL(OcilibReferenceToString, ref, size, str);
}

/* --------------------------------------------------------------------------------------------- *
 *  XmlType
 * --------------------------------------------------------------------------------------------- */

const otext* OCI_API OCI_XmlTypeGetContent
(
    OCI_XmlType * xmlType
)
{
    CALL_IMPL(OcilibXmlTypeGetContent, xmlType);
}

unsigned int OCI_API OCI_XmlTypeGetContentSize
(
    OCI_XmlType * xmlType
)
{
    CALL_IMPL(OcilibXmlTypeGetContentSize, xmlType);
}

/* --------------------------------------------------------------------------------------------- *
 *  Vector
 * --------------------------------------------------------------------------------------------- */

OCI_Vector * OCI_API OCI_VectorCreate
(
    OCI_Connection *con
)
{
    CALL_IMPL(OcilibVectorCreate, con);
}

boolean OCI_API OCI_VectorFree
(
    OCI_Vector *vect
)
{
    CALL_IMPL(OcilibVectorFree, vect);
}

OCI_Vector** OCI_API OCI_VectorArrayCreate
(
    OCI_Connection* con,
    unsigned int    nbelem
)
{
    CALL_IMPL(OcilibVectorCreateArray, con, nbelem);
}

boolean OCI_API OCI_VectorArrayFree
(
    OCI_Vector** vects
)
{
    CALL_IMPL(OcilibVectorFreeArray, vects);
}

boolean OCI_API OCI_VectorGetInfo
(
    OCI_Vector   *vect,
    unsigned int *format,
    unsigned int *dimensions
)
{
    CALL_IMPL(OcilibVectorGetInfo, vect, format, dimensions);
}

boolean OCI_API OCI_VectorGetValues
(
    OCI_Vector *vect,
    void       *values
)
{
    CALL_IMPL(OcilibVectorGetValues, vect, values);
}

boolean OCI_API OCI_VectorSetValues
(
    OCI_Vector       *vect,
    unsigned int      format,
    unsigned int      dimensions,
    void             *values
)
{
    CALL_IMPL(OcilibVectorSetValues, vect, format, dimensions, values);
}

boolean OCI_API OCI_VectorFromText
(
    OCI_Vector  *vect,
    const otext* str,
    unsigned int size,
    unsigned int format,
    unsigned int dimensions
)
{
    CALL_IMPL(OcilibVectorFromString, vect, str, size, format, dimensions);
}

boolean OCI_API OCI_VectorToText
(
    OCI_Vector    *vect,
    unsigned int  *size,
    otext         *str
)
{
    CALL_IMPL(OcilibVectorToString, vect, size, str);
}

/* --------------------------------------------------------------------------------------------- *
 *  resultset
 * --------------------------------------------------------------------------------------------- */

OCI_Resultset* OCI_API OCI_GetResultset
(
    OCI_Statement* stmt
)
{
    CALL_IMPL(OcilibStatementGetResultset, stmt);
}

OCI_Resultset* OCI_API OCI_GetNextResultset
(
    OCI_Statement* stmt
)
{
    CALL_IMPL(OcilibStatementGetNextResultset, stmt);
}

boolean OCI_API OCI_FetchPrev
(
    OCI_Resultset* rs
)
{
    CALL_IMPL(OcilibResultsetFetchPrev, rs);
}

boolean OCI_API OCI_FetchNext
(
    OCI_Resultset* rs
)
{
    CALL_IMPL(OcilibResultsetFetchNext, rs);
}

boolean OCI_API OCI_FetchFirst
(
    OCI_Resultset* rs
)
{
    CALL_IMPL(OcilibResultsetFetchFirst, rs);
}

boolean OCI_API OCI_FetchLast
(
    OCI_Resultset* rs
)
{
    CALL_IMPL(OcilibResultsetFetchLast, rs);
}

boolean OCI_API OCI_FetchSeek
(
    OCI_Resultset* rs,
    unsigned int   mode,
    int            offset
)
{
    CALL_IMPL(OcilibResultsetFetchSeek, rs, mode, offset);
}

unsigned int OCI_API OCI_GetRowCount
(
    OCI_Resultset* rs
)
{
    CALL_IMPL(OcilibResultsetGetRowCount, rs);
}

unsigned int OCI_API OCI_GetCurrentRow
(
    OCI_Resultset* rs
)
{
    CALL_IMPL(OcilibResultsetGetCurrentRow, rs);
}

unsigned int OCI_API OCI_GetColumnCount
(
    OCI_Resultset* rs
)
{
    CALL_IMPL(OcilibResultsetGetColumnCount, rs);
}

OCI_Column* OCI_API OCI_GetColumn
(
    OCI_Resultset* rs,
    unsigned int   index
)
{
    CALL_IMPL(OcilibResultsetGetColumn, rs, index);
}

OCI_Column* OCI_API OCI_GetColumn2
(
    OCI_Resultset* rs,
    const otext  * name
)
{
    CALL_IMPL(OcilibResultsetGetColumn2, rs, name);
}

unsigned int OCI_API OCI_GetColumnIndex
(
    OCI_Resultset* rs,
    const otext  * name
)
{
    CALL_IMPL(OcilibResultsetGetColumnIndex, rs, name);
}

boolean OCI_API OCI_SetStructNumericType
(
    OCI_Resultset* rs,
    unsigned int   index,
    unsigned int   type
)
{
    CALL_IMPL(OcilibResultsetSetStructNumericType, rs, index, type);
}

boolean OCI_API OCI_SetStructNumericType2
(
    OCI_Resultset* rs,
    const otext  * name,
    unsigned int   type
)
{
    CALL_IMPL(OcilibResultsetSetStructNumericType2, rs, name, type);
}

boolean OCI_API OCI_GetStruct
(
    OCI_Resultset* rs,
    void         * row_struct,
    void         * row_struct_ind
)
{
    CALL_IMPL(OcilibResultsetGetStruct, rs, row_struct, row_struct_ind);
}

OCI_Number* OCI_API OCI_GetNumber
(
    OCI_Resultset* rs,
    unsigned int   index
)
{
    CALL_IMPL(OcilibResultsetGetNumber, rs, index);
}

OCI_Number* OCI_API OCI_GetNumber2
(
    OCI_Resultset* rs,
    const otext  * name
)
{
    CALL_IMPL(OcilibResultsetGetNumber2, rs, name);
}

short OCI_API OCI_GetShort
(
    OCI_Resultset* rs,
    unsigned int   index
)
{
    CALL_IMPL(OcilibResultsetGetShort, rs, index);
}

short OCI_API OCI_GetShort2
(
    OCI_Resultset* rs,
    const otext  * name
)
{
    CALL_IMPL(OcilibResultsetGetShort2, rs, name);
}

unsigned short OCI_API OCI_GetUnsignedShort
(
    OCI_Resultset* rs,
    unsigned int   index
)
{
    CALL_IMPL(OcilibResultsetGetUnsignedShort, rs, index);
}

unsigned short OCI_API OCI_GetUnsignedShort2
(
    OCI_Resultset* rs,
    const otext  * name
)
{
    CALL_IMPL(OcilibResultsetGetUnsignedShort2, rs, name);
}

int OCI_API OCI_GetInt
(
    OCI_Resultset* rs,
    unsigned int   index
)
{
    CALL_IMPL(OcilibResultsetGetInt, rs, index);
}

int OCI_API OCI_GetInt2
(
    OCI_Resultset* rs,
    const otext  * name
)
{
    CALL_IMPL(OcilibResultsetGetInt2, rs, name);
}

unsigned int OCI_API OCI_GetUnsignedInt
(
    OCI_Resultset* rs,
    unsigned int   index
)
{
    CALL_IMPL(OcilibResultsetGetUnsignedInt, rs, index);
}

unsigned int OCI_API OCI_GetUnsignedInt2
(
    OCI_Resultset* rs,
    const otext  * name
)
{
    CALL_IMPL(OcilibResultsetGetUnsignedInt2, rs, name);
}

big_int OCI_API OCI_GetBigInt
(
    OCI_Resultset* rs,
    unsigned int   index
)
{
    CALL_IMPL(OcilibResultsetGetBigInt, rs, index);
}

big_int OCI_API OCI_GetBigInt2
(
    OCI_Resultset* rs,
    const otext  * name
)
{
    CALL_IMPL(OcilibResultsetGetBigInt2, rs, name);
}

big_uint OCI_API OCI_GetUnsignedBigInt
(
    OCI_Resultset* rs,
    unsigned int   index
)
{
    CALL_IMPL(OcilibResultsetGetUnsignedBigInt, rs, index);
}

big_uint OCI_API OCI_GetUnsignedBigInt2
(
    OCI_Resultset* rs,
    const otext  * name
)
{
    CALL_IMPL(OcilibResultsetGetUnsignedBigInt2, rs, name);
}

const otext* OCI_API OCI_GetString
(
    OCI_Resultset* rs,
    unsigned int   index
)
{
    CALL_IMPL(OcilibResultsetGetString, rs, index);
}

const otext* OCI_API OCI_GetString2
(
    OCI_Resultset* rs,
    const otext  * name
)
{
    CALL_IMPL(OcilibResultsetGetString2, rs, name);
}

unsigned int OCI_API OCI_GetRaw
(
    OCI_Resultset* rs,
    unsigned int   index,
    void         * buffer,
    unsigned int   len
)
{
    CALL_IMPL(OcilibResultsetGetRaw, rs, index, buffer,len);
}

unsigned int OCI_API OCI_GetRaw2
(
    OCI_Resultset* rs,
    const otext  * name,
    void         * buffer,
    unsigned int   len
)
{
    CALL_IMPL(OcilibResultsetGetRaw2, rs, name, buffer, len);
}

double OCI_API OCI_GetDouble
(
    OCI_Resultset* rs,
    unsigned int   index
)
{
    CALL_IMPL(OcilibResultsetGetDouble, rs, index);
}

double OCI_API OCI_GetDouble2
(
    OCI_Resultset* rs,
    const otext  * name
)
{
    CALL_IMPL(OcilibResultsetGetDouble2, rs, name);
}

float OCI_API OCI_GetFloat
(
    OCI_Resultset* rs,
    unsigned int   index
)
{
    CALL_IMPL(OcilibResultsetGetFloat, rs, index);
}

float OCI_API OCI_GetFloat2
(
    OCI_Resultset* rs,
    const otext  * name
)
{
    CALL_IMPL(OcilibResultsetGetFloat2, rs, name);
}

OCI_Date* OCI_API OCI_GetDate
(
    OCI_Resultset* rs,
    unsigned int   index
)
{
    CALL_IMPL(OcilibResultsetGetDate, rs, index);
}

OCI_Date* OCI_API OCI_GetDate2
(
    OCI_Resultset* rs,
    const otext  * name
)
{
    CALL_IMPL(OcilibResultsetGetDate2, rs, name);
}

OCI_Timestamp* OCI_API OCI_GetTimestamp
(
    OCI_Resultset* rs,
    unsigned int   index
)
{
    CALL_IMPL(OcilibResultsetGetTimestamp, rs, index);
}

OCI_Timestamp* OCI_API OCI_GetTimestamp2
(
    OCI_Resultset* rs,
    const otext  * name
)
{
    CALL_IMPL(OcilibResultsetGetTimestamp2, rs, name);
}

OCI_Interval* OCI_API OCI_GetInterval
(
    OCI_Resultset* rs,
    unsigned int   index
)
{
    CALL_IMPL(OcilibResultsetGetInterval, rs, index);
}

OCI_Interval* OCI_API OCI_GetInterval2
(
    OCI_Resultset* rs,
    const otext  * name
)
{
    CALL_IMPL(OcilibResultsetGetInterval2, rs, name);
}

OCI_Object* OCI_API OCI_GetObject
(
    OCI_Resultset* rs,
    unsigned int   index
)
{
    CALL_IMPL(OcilibResultsetGetObject, rs, index);
}

OCI_Object* OCI_API OCI_GetObject2
(
    OCI_Resultset* rs,
    const otext  * name
)
{
    CALL_IMPL(OcilibResultsetGetObject2, rs, name);
}

OCI_Coll* OCI_API OCI_GetColl
(
    OCI_Resultset* rs,
    unsigned int   index
)
{
    CALL_IMPL(OcilibResultsetGetColl, rs, index);
}

OCI_Coll* OCI_API OCI_GetColl2
(
    OCI_Resultset* rs,
    const otext  * name
)
{
    CALL_IMPL(OcilibResultsetGetColl2, rs, name);
}

OCI_Ref* OCI_API OCI_GetRef
(
    OCI_Resultset* rs,
    unsigned int   index
)
{
    CALL_IMPL(OcilibResultsetGetReference, rs, index);
}

OCI_Ref* OCI_API OCI_GetRef2
(
    OCI_Resultset* rs,
    const otext  * name
)
{
    CALL_IMPL(OcilibResultsetGetReference2, rs, name);
}

OCI_XmlType* OCI_API OCI_GetXmlType
(
    OCI_Resultset* rs,
    unsigned int   index
)
{
    CALL_IMPL(OcilibResultsetGetXmlType, rs, index);
}

OCI_XmlType* OCI_API OCI_GetXmlType2
(
    OCI_Resultset* rs,
    const otext  * name
)
{
    CALL_IMPL(OcilibResultsetGetXmlType2, rs, name);
}

OCI_Vector * OCI_API OCI_GetVector
(
    OCI_Resultset *rs,
    unsigned int   index
)
{
    CALL_IMPL(OcilibResultsetGetVector, rs, index);
}

OCI_Vector * OCI_API OCI_GetVector2
(
    OCI_Resultset *rs,
    const otext *  name
)
{
    CALL_IMPL(OcilibResultsetGetVector2, rs, name);
}

OCI_Statement* OCI_API OCI_GetStatement
(
    OCI_Resultset* rs,
    unsigned int   index
)
{
    CALL_IMPL(OcilibResultsetGetStatement, rs, index);
}

OCI_Statement* OCI_API OCI_GetStatement2
(
    OCI_Resultset* rs,
    const otext  * name
)
{
    CALL_IMPL(OcilibResultsetGetStatement2, rs, name);
}

OCI_Lob* OCI_API OCI_GetLob
(
    OCI_Resultset* rs,
    unsigned int   index
)
{
    CALL_IMPL(OcilibResultsetGetLob, rs, index);
}

OCI_Lob* OCI_API OCI_GetLob2
(
    OCI_Resultset* rs,
    const otext  * name
)
{
    CALL_IMPL(OcilibResultsetGetLob2, rs, name);
}

OCI_File* OCI_API OCI_GetFile
(
    OCI_Resultset* rs,
    unsigned int   index
)
{
    CALL_IMPL(OcilibResultsetGetFile, rs, index);
}

OCI_File* OCI_API OCI_GetFile2
(
    OCI_Resultset* rs,
    const otext  * name
)
{
    CALL_IMPL(OcilibResultsetGetFile2, rs, name);
}

OCI_Long* OCI_API OCI_GetLong
(
    OCI_Resultset* rs,
    unsigned int   index
)
{
    CALL_IMPL(OcilibResultsetGetLong, rs, index);
}

OCI_Long* OCI_API OCI_GetLong2
(
    OCI_Resultset* rs,
    const otext  * name
)
{
    CALL_IMPL(OcilibResultsetGetLong2, rs, name);
}

unsigned int OCI_API OCI_GetDataSize
(
    OCI_Resultset* rs,
    unsigned int   index
)
{
    CALL_IMPL(OcilibResultsetGetDataSize, rs, index);
}

unsigned int OCI_API OCI_GetDataSize2
(
    OCI_Resultset* rs,
    const otext  * name
)
{
    CALL_IMPL(OcilibResultsetGetDataSize2, rs, name);
}

boolean OCI_API OCI_IsNull
(
    OCI_Resultset* rs,
    unsigned int   index
)
{
    CALL_IMPL(OcilibResultsetIsNull, rs, index);
}

boolean OCI_API OCI_IsNull2
(
    OCI_Resultset* rs,
    const otext  * name
)
{
    CALL_IMPL(OcilibResultsetIsNull2, rs, name);
}

OCI_Statement* OCI_API OCI_ResultsetGetStatement
(
    OCI_Resultset* rs
)
{
    CALL_IMPL(OcilibResultsetResultsetGetStatement, rs);
}

unsigned int OCI_API OCI_GetDataLength
(
    OCI_Resultset* rs,
    unsigned int   index
)
{
    CALL_IMPL(OcilibResultsetGetDataLength, rs, index);
}

/* --------------------------------------------------------------------------------------------- *
 *  statement
 * --------------------------------------------------------------------------------------------- */

OCI_Statement* OCI_API OCI_StatementCreate
(
    OCI_Connection* con
)
{
    CALL_IMPL(OcilibStatementCreate, con);
}

boolean OCI_API OCI_StatementFree
(
    OCI_Statement* stmt
)
{
    CALL_IMPL(OcilibStatementFree, stmt);
}

boolean OCI_API OCI_ReleaseResultsets
(
    OCI_Statement* stmt
)
{
    CALL_IMPL(OcilibStatementReleaseResultsets, stmt);
}

boolean OCI_API OCI_Prepare
(
    OCI_Statement* stmt,
    const otext  * sql
)
{
    CALL_IMPL(OcilibStatementPrepare, stmt, sql);
}

boolean OCI_API OCI_Execute
(
    OCI_Statement* stmt
)
{
    CALL_IMPL(OcilibStatementExecute, stmt);
}

boolean OCI_API OCI_ExecuteStmt
(
    OCI_Statement* stmt,
    const otext  * sql
)
{
    CALL_IMPL(OcilibStatementExecuteStmt, stmt, sql);
}

boolean OCI_API OCI_Parse
(
    OCI_Statement* stmt,
    const otext  * sql
)
{
    CALL_IMPL(OcilibStatementParse, stmt, sql);
}

boolean OCI_API OCI_Describe
(
    OCI_Statement* stmt,
    const otext  * sql
)
{
    CALL_IMPL(OcilibStatementDescribe, stmt, sql);
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
    res = OcilibStatementPrepareFmt(stmt, sql, args);
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
    res = OcilibStatementExecuteStmtFmt(stmt, sql, args);
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
    res = OcilibStatementParseFmt(stmt, sql, args);
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
    res = OcilibStatementDescribeFmt(stmt, sql, args);
    va_end(args);

    return res;
}

boolean OCI_API OCI_BindArraySetSize
(
    OCI_Statement* stmt,
    unsigned int   size
)
{
    CALL_IMPL(OcilibStatementSetBindArraySize, stmt, size);
}

unsigned int OCI_API OCI_BindArrayGetSize
(
    OCI_Statement* stmt
)
{
    CALL_IMPL(OcilibStatementGetBindArraySize, stmt);
}

boolean OCI_API OCI_AllowRebinding
(
    OCI_Statement* stmt,
    boolean        value
)
{
    CALL_IMPL(OcilibStatementAllowRebinding, stmt, value);
}

boolean OCI_API OCI_IsRebindingAllowed
(
    OCI_Statement* stmt
)
{
    CALL_IMPL(OcilibStatementIsRebindingAllowed, stmt);
}

boolean OCI_API OCI_BindBoolean
(
    OCI_Statement* stmt,
    const otext  * name,
    boolean      * data
)
{
    CALL_IMPL(OcilibStatementBindBoolean, stmt, name, data);
}

boolean OCI_API OCI_BindNumber
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Number   * data
)
{
    CALL_IMPL(OcilibStatementBindNumber, stmt, name, data);
}

boolean OCI_API OCI_BindArrayOfNumbers
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Number  ** data,
    unsigned int   nbelem
)
{
    CALL_IMPL(OcilibStatementBindArrayOfNumbers, stmt, name, data, nbelem);
}

boolean OCI_API OCI_BindShort
(
    OCI_Statement* stmt,
    const otext  * name,
    short        * data
)
{
    CALL_IMPL(OcilibStatementBindShort, stmt, name, data);
}

boolean OCI_API OCI_BindArrayOfShorts
(
    OCI_Statement* stmt,
    const otext  * name,
    short        * data,
    unsigned int   nbelem
)
{
    CALL_IMPL(OcilibStatementBindArrayOfShorts, stmt, name, data, nbelem);
}

boolean OCI_API OCI_BindUnsignedShort
(
    OCI_Statement * stmt,
    const otext   * name,
    unsigned short* data
)
{
    CALL_IMPL(OcilibStatementBindUnsignedShort, stmt, name, data);
}

boolean OCI_API OCI_BindArrayOfUnsignedShorts
(
    OCI_Statement * stmt,
    const otext   * name,
    unsigned short* data,
    unsigned int    nbelem
)
{
    CALL_IMPL(OcilibStatementBindArrayOfUnsignedShorts, stmt, name, data, nbelem);
}

boolean OCI_API OCI_BindInt
(
    OCI_Statement* stmt,
    const otext  * name,
    int          * data
)
{
    CALL_IMPL(OcilibStatementBindInt, stmt, name, data);
}

boolean OCI_API OCI_BindArrayOfInts
(
    OCI_Statement* stmt,
    const otext  * name,
    int          * data,
    unsigned int   nbelem
)
{
    CALL_IMPL(OcilibStatementBindArrayOfInts, stmt, name, data, nbelem);
}

boolean OCI_API OCI_BindUnsignedInt
(
    OCI_Statement* stmt,
    const otext  * name,
    unsigned int * data
)
{
    CALL_IMPL(OcilibStatementBindUnsignedInt, stmt, name, data);
}

boolean OCI_API OCI_BindArrayOfUnsignedInts
(
    OCI_Statement* stmt,
    const otext  * name,
    unsigned int * data,
    unsigned int   nbelem
)
{
    CALL_IMPL(OcilibStatementBindArrayOfUnsignedInts, stmt, name, data, nbelem);
}

boolean OCI_API OCI_BindBigInt
(
    OCI_Statement* stmt,
    const otext  * name,
    big_int      * data
)
{
    CALL_IMPL(OcilibStatementBindBigInt, stmt, name, data);
}

boolean OCI_API OCI_BindArrayOfBigInts
(
    OCI_Statement* stmt,
    const otext  * name,
    big_int      * data,
    unsigned int   nbelem
)
{
    CALL_IMPL(OcilibStatementBindArrayOfBigInts, stmt, name, data, nbelem);
}

boolean OCI_API OCI_BindUnsignedBigInt
(
    OCI_Statement* stmt,
    const otext  * name,
    big_uint     * data
)
{
    CALL_IMPL(OcilibStatementBindUnsignedBigInt, stmt, name, data);
}

boolean OCI_API OCI_BindArrayOfUnsignedBigInts
(
    OCI_Statement* stmt,
    const otext  * name,
    big_uint     * data,
    unsigned int   nbelem
)
{
    CALL_IMPL(OcilibStatementBindArrayOfUnsignedBigInts, stmt, name, data, nbelem);
}

boolean OCI_API OCI_BindString
(
    OCI_Statement* stmt,
    const otext  * name,
    otext        * data,
    unsigned int   len
)
{
    CALL_IMPL(OcilibStatementBindString, stmt, name, data, len);
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
    CALL_IMPL(OcilibStatementBindArrayOfStrings, stmt, name, data, len, nbelem);
}

boolean OCI_API OCI_BindRaw
(
    OCI_Statement* stmt,
    const otext  * name,
    void         * data,
    unsigned int   len
)
{
    CALL_IMPL(OcilibStatementBindRaw, stmt, name, data, len);
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
    CALL_IMPL(OcilibStatementBindArrayOfRaws, stmt, name, data, len, nbelem);
}

boolean OCI_API OCI_BindDouble
(
    OCI_Statement* stmt,
    const otext  * name,
    double       * data
)
{
    CALL_IMPL(OcilibStatementBindDouble, stmt, name, data);
}

boolean OCI_API OCI_BindArrayOfDoubles
(
    OCI_Statement* stmt,
    const otext  * name,
    double       * data,
    unsigned int   nbelem
)
{
    CALL_IMPL(OcilibStatementBindArrayOfDoubles, stmt, name, data, nbelem);
}

boolean OCI_API OCI_BindFloat
(
    OCI_Statement* stmt,
    const otext  * name,
    float        * data
)
{
    CALL_IMPL(OcilibStatementBindFloat, stmt, name, data);
}

boolean OCI_API OCI_BindArrayOfFloats
(
    OCI_Statement* stmt,
    const otext  * name,
    float        * data,
    unsigned int   nbelem
)
{
    CALL_IMPL(OcilibStatementBindArrayOfFloats, stmt, name, data, nbelem);
}

boolean OCI_API OCI_BindDate
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Date     * data
)
{
    CALL_IMPL(OcilibStatementBindDate, stmt, name, data);
}

boolean OCI_API OCI_BindArrayOfDates
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Date    ** data,
    unsigned int   nbelem
)
{
    CALL_IMPL(OcilibStatementBindArrayOfDates, stmt, name, data, nbelem);
}

boolean OCI_API OCI_BindTimestamp
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Timestamp* data
)
{
    CALL_IMPL(OcilibStatementBindTimestamp, stmt, name, data);
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
    CALL_IMPL(OcilibStatementBindArrayOfTimestamps, stmt, name, data, type, nbelem);
}

boolean OCI_API OCI_BindInterval
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Interval * data
)
{
    CALL_IMPL(OcilibStatementBindInterval, stmt, name, data);
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
    CALL_IMPL(OcilibStatementBindArrayOfIntervals, stmt, name, data, type, nbelem);
}

boolean OCI_API OCI_BindObject
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Object   * data
)
{
    CALL_IMPL(OcilibStatementBindObject, stmt, name, data);
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
    CALL_IMPL(OcilibStatementBindArrayOfObjects, stmt, name, data, typinf, nbelem);
}

boolean OCI_API OCI_BindVector
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Vector   * data
)
{
    CALL_IMPL(OcilibStatementBindVector, stmt, name, data);
}

boolean OCI_API OCI_BindArrayOfVectors
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Vector  ** data,
    unsigned int   nbelem
)
{
    CALL_IMPL(OcilibStatementBindArrayOfVectors, stmt, name, data, nbelem);
}

boolean OCI_API OCI_BindLob
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Lob      * data
)
{
    CALL_IMPL(OcilibStatementBindLob, stmt, name, data);
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
    CALL_IMPL(OcilibStatementBindArrayOfLobs, stmt, name, data, type, nbelem);
}

boolean OCI_API OCI_BindFile
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_File     * data
)
{
    CALL_IMPL(OcilibStatementBindFile, stmt, name, data);
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
    CALL_IMPL(OcilibStatementBindArrayOfFiles, stmt, name, data, type, nbelem);
}

boolean OCI_API OCI_BindRef
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Ref      * data
)
{
    CALL_IMPL(OcilibStatementBindReference, stmt, name, data);
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
    CALL_IMPL(OcilibStatementBindArrayOfReferences, stmt, name, data, typinf, nbelem);
}

boolean OCI_API OCI_BindColl
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Coll     * data
)
{
    CALL_IMPL(OcilibStatementBindCollection, stmt, name, data);
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
    CALL_IMPL(OcilibStatementBindArrayOfCollections, stmt, name, data, typinf, nbelem);
}

boolean OCI_API OCI_BindStatement
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Statement* data
)
{
    CALL_IMPL(OcilibStatementBindStatement, stmt, name, data);
}

boolean OCI_API OCI_BindLong
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Long     * data,
    unsigned int   size
)
{
    CALL_IMPL(OcilibStatementBindLong, stmt, name, data, size);
}

boolean OCI_API OCI_RegisterNumber
(
    OCI_Statement* stmt,
    const otext  * name
)
{
    CALL_IMPL(OcilibStatementRegisterNumber, stmt, name);
}

boolean OCI_API OCI_RegisterShort
(
    OCI_Statement* stmt,
    const otext  * name
)
{
    CALL_IMPL(OcilibStatementRegisterShort, stmt, name);
}

boolean OCI_API OCI_RegisterUnsignedShort
(
    OCI_Statement* stmt,
    const otext  * name
)
{
    CALL_IMPL(OcilibStatementRegisterUnsignedShort, stmt, name);
}

boolean OCI_API OCI_RegisterInt
(
    OCI_Statement* stmt,
    const otext  * name
)
{
    CALL_IMPL(OcilibStatementRegisterInt, stmt, name);
}

boolean OCI_API OCI_RegisterUnsignedInt
(
    OCI_Statement* stmt,
    const otext  * name
)
{
    CALL_IMPL(OcilibStatementRegisterUnsignedInt, stmt, name);
}

boolean OCI_API OCI_RegisterBigInt
(
    OCI_Statement* stmt,
    const otext  * name
)
{
    CALL_IMPL(OcilibStatementRegisterBigInt, stmt, name);
}

boolean OCI_API OCI_RegisterUnsignedBigInt
(
    OCI_Statement* stmt,
    const otext  * name
)
{
    CALL_IMPL(OcilibStatementRegisterUnsignedBigInt, stmt, name);
}

boolean OCI_API OCI_RegisterString
(
    OCI_Statement* stmt,
    const otext  * name,
    unsigned int   len
)
{
    CALL_IMPL(OcilibStatementRegisterString, stmt, name, len);
}

boolean OCI_API OCI_RegisterRaw
(
    OCI_Statement* stmt,
    const otext  * name,
    unsigned int   len
)
{
    CALL_IMPL(OcilibStatementRegisterRaw, stmt, name, len);
}

boolean OCI_API OCI_RegisterDouble
(
    OCI_Statement* stmt,
    const otext  * name
)
{
    CALL_IMPL(OcilibStatementRegisterDouble, stmt, name);
}

boolean OCI_API OCI_RegisterFloat
(
    OCI_Statement* stmt,
    const otext  * name
)
{
    CALL_IMPL(OcilibStatementRegisterFloat, stmt, name);
}

boolean OCI_API OCI_RegisterDate
(
    OCI_Statement* stmt,
    const otext  * name
)
{
    CALL_IMPL(OcilibStatementRegisterDate, stmt, name);
}

boolean OCI_API OCI_RegisterTimestamp
(
    OCI_Statement* stmt,
    const otext  * name,
    unsigned int   type
)
{
    CALL_IMPL(OcilibStatementRegisterTimestamp, stmt, name, type);
}

boolean OCI_API OCI_RegisterInterval
(
    OCI_Statement* stmt,
    const otext  * name,
    unsigned int   type
)
{
    CALL_IMPL(OcilibStatementRegisterInterval, stmt, name, type);
}

boolean OCI_API OCI_RegisterObject
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_TypeInfo * typinf
)
{
    CALL_IMPL(OcilibStatementRegisterObject, stmt, name, typinf);
}

boolean OCI_API OCI_RegisterLob
(
    OCI_Statement* stmt,
    const otext  * name,
    unsigned int   type
)
{
    CALL_IMPL(OcilibStatementRegisterLob, stmt, name, type);
}

boolean OCI_API OCI_RegisterFile
(
    OCI_Statement* stmt,
    const otext  * name,
    unsigned int   type
)
{
    CALL_IMPL(OcilibStatementRegisterFile, stmt, name, type);
}

boolean OCI_API OCI_RegisterRef
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_TypeInfo * typinf
)
{
    CALL_IMPL(OcilibStatementRegisterReference, stmt, name, typinf);
}

boolean OCI_API OCI_RegisterVector
(
    OCI_Statement* stmt,
    const otext  * name
)
{
    CALL_IMPL(OcilibStatementRegisterVector, stmt, name);
}

unsigned int OCI_API OCI_GetStatementType
(
    OCI_Statement* stmt
)
{
    CALL_IMPL(OcilibStatementGetStatementType, stmt);
}

boolean OCI_API OCI_SetFetchMode
(
    OCI_Statement* stmt,
    unsigned int   mode
)
{
    CALL_IMPL(OcilibStatementSetFetchMode, stmt, mode);
}

unsigned int OCI_API OCI_GetFetchMode
(
    OCI_Statement* stmt
)
{
    CALL_IMPL(OcilibStatementGetFetchMode, stmt);
}

boolean OCI_API OCI_SetBindMode
(
    OCI_Statement* stmt,
    unsigned int   mode
)
{
    CALL_IMPL(OcilibStatementSetBindMode, stmt, mode);
}

unsigned int OCI_API OCI_GetBindMode
(
    OCI_Statement* stmt
)
{
    CALL_IMPL(OcilibStatementGetBindMode, stmt);
}

boolean OCI_API OCI_SetBindAllocation
(
    OCI_Statement* stmt,
    unsigned int   mode
)
{
    CALL_IMPL(OcilibStatementSetBindAllocation, stmt, mode);
}

unsigned int OCI_API OCI_GetBindAllocation
(
    OCI_Statement* stmt
)
{
    CALL_IMPL(OcilibStatementGetBindAllocation, stmt);
}

boolean OCI_API OCI_SetFetchSize
(
    OCI_Statement* stmt,
    unsigned int   size
)
{
    CALL_IMPL(OcilibStatementSetFetchSize, stmt, size);
}

unsigned int OCI_API OCI_GetFetchSize
(
    OCI_Statement* stmt
)
{
    CALL_IMPL(OcilibStatementGetFetchSize, stmt);
}

boolean OCI_API OCI_SetPrefetchSize
(
    OCI_Statement* stmt,
    unsigned int   size
)
{
    CALL_IMPL(OcilibStatementSetPrefetchSize, stmt, size);
}

unsigned int OCI_API OCI_GetPrefetchSize
(
    OCI_Statement* stmt
)
{
    CALL_IMPL(OcilibStatementGetPrefetchSize, stmt);
}

boolean OCI_API OCI_SetPrefetchMemory
(
    OCI_Statement* stmt,
    unsigned int   size
)
{
    CALL_IMPL(OcilibStatementSetPrefetchMemory, stmt, size);
}

unsigned int OCI_API OCI_GetPrefetchMemory
(
    OCI_Statement* stmt
)
{
    CALL_IMPL(OcilibStatementGetPrefetchMemory, stmt);
}

boolean OCI_API OCI_SetPieceSize
(
    OCI_Statement* stmt,
    unsigned int   size
)
{
    CALL_IMPL(OcilibStatementSetPieceSize, stmt, size);
}

unsigned int OCI_API OCI_GetPieceSize
(
    OCI_Statement* stmt
)
{
    CALL_IMPL(OcilibStatementGetPieceSize, stmt);
}

boolean OCI_API OCI_SetLongMode
(
    OCI_Statement* stmt,
    unsigned int   mode
)
{
    CALL_IMPL(OcilibStatementSetLongMode, stmt, mode);
}

unsigned int OCI_API OCI_GetLongMode
(
    OCI_Statement* stmt
)
{
    CALL_IMPL(OcilibStatementGetLongMode, stmt);
}

OCI_Connection* OCI_API OCI_StatementGetConnection
(
    OCI_Statement* stmt
)
{
    CALL_IMPL(OcilibStatementGetConnection, stmt);
}

const otext* OCI_API OCI_GetSql
(
    OCI_Statement* stmt
)
{
    CALL_IMPL(OcilibStatementGetSql, stmt);
}

const otext* OCI_API OCI_GetSqlIdentifier
(
    OCI_Statement* stmt
)
{
    CALL_IMPL(OcilibStatementGetSqlIdentifier, stmt);
}

unsigned int OCI_API OCI_GetSqlErrorPos
(
    OCI_Statement* stmt
)
{
    CALL_IMPL(OcilibStatementGetSqlErrorPos, stmt);
}

unsigned int OCI_API OCI_GetAffectedRows
(
    OCI_Statement* stmt
)
{
    CALL_IMPL(OcilibStatementGetAffectedRows, stmt);
}

unsigned int OCI_API OCI_GetBindCount
(
    OCI_Statement* stmt
)
{
    CALL_IMPL(OcilibStatementGetBindCount, stmt);
}

OCI_Bind* OCI_API OCI_GetBind
(
    OCI_Statement* stmt,
    unsigned int   index
)
{
    CALL_IMPL(OcilibStatementGetBind, stmt, index);
}

OCI_Bind* OCI_API OCI_GetBind2
(
    OCI_Statement* stmt,
    const otext  * name
)
{
    CALL_IMPL(OcilibStatementGetBind2, stmt, name);
}

unsigned int OCI_API OCI_GetBindIndex
(
    OCI_Statement* stmt,
    const otext  * name
)
{
    CALL_IMPL(OcilibStatementGetBindIndex, stmt, name);
}

unsigned int OCI_API OCI_GetSQLCommand
(
    OCI_Statement* stmt
)
{
    CALL_IMPL(OcilibStatementGetSqlCommand, stmt);
}

const otext* OCI_API OCI_GetSQLVerb
(
    OCI_Statement* stmt
)
{
    CALL_IMPL(OcilibStatementGetSqlVerb, stmt);
}

OCI_Error* OCI_API OCI_GetBatchError
(
    OCI_Statement* stmt
)
{
    CALL_IMPL(OcilibStatementGetBatchError, stmt);
}

unsigned int OCI_API OCI_GetBatchErrorCount
(
    OCI_Statement* stmt
)
{
    CALL_IMPL(OcilibStatementGetBatchErrorCount, stmt);
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
    CALL_IMPL(OcilibSubscriptionRegister, con, name, type, handler, port, timeout);
}

boolean OCI_API OCI_SubscriptionUnregister
(
    OCI_Subscription* sub
)
{
    CALL_IMPL(OcilibSubscriptionUnregister, sub);
}

boolean OCI_API OCI_SubscriptionAddStatement
(
    OCI_Subscription* sub,
    OCI_Statement   * stmt
)
{
    CALL_IMPL(OcilibSubscriptionAddStatement, sub, stmt);
}

const otext* OCI_API OCI_SubscriptionGetName
(
    OCI_Subscription* sub
)
{
    CALL_IMPL(OcilibSubscriptionGetName, sub);
}

unsigned int OCI_API OCI_SubscriptionGetPort
(
    OCI_Subscription* sub
)
{
    CALL_IMPL(OcilibSubscriptionGetPort, sub);
}

unsigned int OCI_API OCI_SubscriptionGetTimeout
(
    OCI_Subscription* sub
)
{
    CALL_IMPL(OcilibSubscriptionGetTimeout, sub);
}

OCI_Connection* OCI_API OCI_SubscriptionGetConnection
(
    OCI_Subscription* sub
)
{
    CALL_IMPL(OcilibSubscriptionGetConnection, sub);
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
    CALL_IMPL(OcilibTimestampCreate, con, type);
}

boolean OCI_API OCI_TimestampFree
(
    OCI_Timestamp* tmsp
)
{
    CALL_IMPL(OcilibTimestampFree, tmsp);
}

OCI_Timestamp** OCI_API OCI_TimestampArrayCreate
(
    OCI_Connection* con,
    unsigned int    type,
    unsigned int    nbelem
)
{
    CALL_IMPL(OcilibTimestampCreateArray, con, type, nbelem);
}

boolean OCI_API OCI_TimestampArrayFree
(
    OCI_Timestamp** tmsps
)
{
    CALL_IMPL(OcilibTimestampFreeArray, tmsps);
}

unsigned int OCI_API OCI_TimestampGetType
(
    OCI_Timestamp* tmsp
)
{
    CALL_IMPL(OcilibTimestampGetType, tmsp);
}

boolean OCI_API OCI_TimestampAssign
(
    OCI_Timestamp* tmsp,
    OCI_Timestamp* tmsp_src
)
{
    CALL_IMPL(OcilibTimestampAssign, tmsp, tmsp_src);
}

int OCI_API OCI_TimestampCheck
(
    OCI_Timestamp* tmsp
)
{
    CALL_IMPL(OcilibTimestampCheck, tmsp);
}

int OCI_API OCI_TimestampCompare
(
    OCI_Timestamp* tmsp,
    OCI_Timestamp* tmsp2
)
{
    CALL_IMPL(OcilibTimestampCompare, tmsp, tmsp2);
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
    CALL_IMPL(OcilibTimestampConstruct, tmsp, year, month, day, hour, min, sec,fsec, time_zone);
}

boolean OCI_API OCI_TimestampConvert
(
    OCI_Timestamp* tmsp,
    OCI_Timestamp* tmsp_src
)
{
    CALL_IMPL(OcilibTimestampConvert, tmsp, tmsp_src);
}

boolean OCI_API OCI_TimestampFromText
(
    OCI_Timestamp* tmsp,
    const otext  * str,
    const otext  * fmt
)
{
    CALL_IMPL(OcilibTimestampFromString, tmsp, str, fmt);
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
    CALL_IMPL(OcilibTimestampToString, tmsp, fmt, size, str, precision);
}

boolean OCI_API OCI_TimestampGetDate
(
    OCI_Timestamp* tmsp,
    int          * year,
    int          * month,
    int          * day
)
{
    CALL_IMPL(OcilibTimestampGetDate, tmsp, year, month, day);
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
    CALL_IMPL(OcilibTimestampGetTime, tmsp, hour, min, sec, fsec);
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
    CALL_IMPL(OcilibTimestampGetDateTime, tmsp, year, month, day, hour, min, sec, fsec);
}

boolean OCI_API OCI_TimestampGetTimeZoneName
(
    OCI_Timestamp* tmsp,
    int            size,
    otext        * str
)
{
    CALL_IMPL(OcilibTimestampGetTimeZoneName, tmsp, size, str);
}

boolean OCI_API OCI_TimestampGetTimeZoneOffset
(
    OCI_Timestamp* tmsp,
    int          * hour,
    int          * min
)
{
    CALL_IMPL(OcilibTimestampGetTimeZoneOffset, tmsp, hour,min);
}

boolean OCI_API OCI_TimestampIntervalAdd
(
    OCI_Timestamp* tmsp,
    OCI_Interval * itv
)
{
    CALL_IMPL(OcilibTimestampIntervalAdd, tmsp, itv);
}

boolean OCI_API OCI_TimestampIntervalSub
(
    OCI_Timestamp* tmsp,
    OCI_Interval * itv
)
{
    CALL_IMPL(OcilibTimestampIntervalSub, tmsp, itv);
}

boolean OCI_API OCI_TimestampSubtract
(
    OCI_Timestamp* tmsp,
    OCI_Timestamp* tmsp2,
    OCI_Interval * itv
)
{
    CALL_IMPL(OcilibTimestampSubtract, tmsp, tmsp2, itv);
}

boolean OCI_API OCI_TimestampSysTimestamp
(
    OCI_Timestamp* tmsp
)
{
    CALL_IMPL(OcilibTimestampSysTimestamp, tmsp);
}

boolean OCI_API OCI_TimestampToCTime
(
    OCI_Timestamp* tmsp,
    struct tm    * ptm,
    time_t       * pt
)
{
    CALL_IMPL(OcilibTimestampToCTime, tmsp, ptm, pt);
}

boolean OCI_API OCI_TimestampFromCTime
(
    OCI_Timestamp* tmsp,
    struct tm    * ptm,
    time_t         t
)
{
    CALL_IMPL(OcilibTimestampFromCTime, tmsp, ptm, t);
}

/* --------------------------------------------------------------------------------------------- *
 *  thread
 * --------------------------------------------------------------------------------------------- */

OCI_Thread* OCI_API OCI_ThreadCreate
(
    void
)
{
    CALL_IMPL_NO_ARGS(OcilibThreadCreate);
}

boolean OCI_API OCI_ThreadFree
(
    OCI_Thread* thread
)
{
    CALL_IMPL(OcilibThreadFree, thread);
}

boolean OCI_API OCI_ThreadRun
(
    OCI_Thread* thread,
    POCI_THREAD proc,
    void      * arg
)
{
    CALL_IMPL(OcilibThreadRun, thread, proc, arg);
}

boolean OCI_API OCI_ThreadJoin
(
    OCI_Thread* thread
)
{
    CALL_IMPL(OcilibThreadJoin, thread);
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
    CALL_IMPL(OcilibThreadKeyCreate, name, destfunc);
}

boolean OCI_API OCI_ThreadKeySetValue
(
    const otext* name,
    void       * value
)
{
    CALL_IMPL(OcilibThreadKeySetValue, name, value);
}

void* OCI_API OCI_ThreadKeyGetValue
(
    const otext* name
)
{
    CALL_IMPL(OcilibThreadKeyGetValue, name);
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
    CALL_IMPL(OcilibTransactionCreate, con, timeout, mode, pxid);
}

boolean OCI_API OCI_TransactionFree
(
    OCI_Transaction* trans
)
{
    CALL_IMPL(OcilibTransactionFree, trans);
}

boolean OCI_API OCI_TransactionStart
(
    OCI_Transaction* trans
)
{
    CALL_IMPL(OcilibTransactionStart, trans);
}

boolean OCI_API OCI_TransactionStop
(
    OCI_Transaction* trans
)
{
    CALL_IMPL(OcilibTransactionStop, trans);
}

boolean OCI_API OCI_TransactionResume
(
    OCI_Transaction* trans
)
{
    CALL_IMPL(OcilibTransactionResume, trans);
}

boolean OCI_API OCI_TransactionPrepare
(
    OCI_Transaction* trans
)
{
    CALL_IMPL(OcilibTransactionPrepare, trans);
}

boolean OCI_API OCI_TransactionForget
(
    OCI_Transaction* trans
)
{
    CALL_IMPL(OcilibTransactionForget, trans);
}

unsigned int OCI_API OCI_TransactionGetMode
(
    OCI_Transaction* trans
)
{
    CALL_IMPL(OcilibTransactionGetMode, trans);
}

unsigned int OCI_API OCI_TransactionGetTimeout
(
    OCI_Transaction* trans
)
{
    CALL_IMPL(OcilibTransactionGetTimeout, trans);
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
    CALL_IMPL(OcilibTypeInfoGet, con, name, type);
}

unsigned int OCI_API OCI_TypeInfoGetType
(
    OCI_TypeInfo* typinf
)
{
    CALL_IMPL(OcilibTypeInfoGetType, typinf);
}

OCI_Connection* OCI_API OCI_TypeInfoGetConnection
(
    OCI_TypeInfo* typinf
)
{
    CALL_IMPL(OcilibTypeInfoGetConnection, typinf);
}

boolean OCI_API OCI_TypeInfoFree
(
    OCI_TypeInfo* typinf
)
{
    CALL_IMPL(OcilibTypeInfoFree, typinf);
}

unsigned int OCI_API OCI_TypeInfoGetColumnCount
(
    OCI_TypeInfo* typinf
)
{
    CALL_IMPL(OcilibTypeInfoGetColumnCount, typinf);
}

OCI_Column* OCI_API OCI_TypeInfoGetColumn
(
    OCI_TypeInfo* typinf,
    unsigned int  index
)
{
    CALL_IMPL(OcilibTypeInfoGetColumn, typinf, index);
}

const otext* OCI_API OCI_TypeInfoGetName
(
    OCI_TypeInfo* typinf
)
{
    CALL_IMPL(OcilibTypeInfoGetName, typinf);
}

boolean OCI_API OCI_TypeInfoIsFinalType
(
    OCI_TypeInfo* typinf
)
{
    CALL_IMPL(OcilibTypeInfoIsFinalType, typinf);
}

OCI_TypeInfo* OCI_API OCI_TypeInfoGetSuperType
(
    OCI_TypeInfo* typinf
)
{
    CALL_IMPL(OcilibTypeInfoGetSuperType, typinf);
}
