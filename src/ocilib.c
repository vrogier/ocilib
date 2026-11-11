/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2026 Vincent ROGIER <vince.rogier@ocilib.net>
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

#include "macros.h"

#include "agent.h"
#include "array.h"
#include "bind.h"
#include "collection.h"
#include "column.h"
#include "connection.h"
#include "database.h"
#include "date.h"
#include "dequeue.h"
#include "directpath.h"
#include "element.h"
#include "enqueue.h"
#include "error.h"
#include "event.h"
#include "file.h"
#include "handle.h"
#include "hashtable.h"
#include "interval.h"
#include "iterator.h"
#include "json.h"
#include "jsonnode.h"
#include "environment.h"
#include "lob.h"
#include "long.h"
#include "message.h"
#include "number.h"
#include "object.h"
#include "pool.h"
#include "queue.h"
#include "reference.h"
#include "resultset.h"
#include "statement.h"
#include "subscription.h"
#include "sodacollection.h"
#include "sodacollectioncursor.h"
#include "shardingkey.h"
#include "sodadatabase.h"
#include "sodadocument.h"
#include "sodadocumentcursor.h"
#include "sodaoptions.h"

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
    OcilibErrorReset(OcilibErrorGet(NULL, TRUE, TRUE));
}

#define CALL_IMPL(impl, ...)                                                                                           \
        do {                                                                                                           \
            OCILIB_LOG_STEP_WITH_ENV(NULL, OCI_LOG_CAT_LIB, OCI_LOG_TRACE, "ENTER", __func__, OTEXT("%s"), OTEXT("")); \
            OcilibCheckContext();                                                                                      \
            return impl(__VA_ARGS__);                                                                                  \
        } while(0);

#define CALL_IMPL_NO_ARGS(impl)                                                                                        \
        do {                                                                                                           \
            OCILIB_LOG_STEP_WITH_ENV(NULL, OCI_LOG_CAT_LIB, OCI_LOG_TRACE, "ENTER", __func__, OTEXT("%s"), OTEXT("")); \
            OcilibCheckContext();                                                                                      \
            return impl();                                                                                             \
        } while(0);

#define CALL_IMPL_E(impl, env, ...)                                                                                     \
        do {                                                                                                            \
            OCILIB_LOG_STEP_WITH_ENV((env), OCI_LOG_CAT_LIB, OCI_LOG_TRACE, "ENTER", __func__, OTEXT("%s"), OTEXT("")); \
            OcilibCheckContext();                                                                                       \
            return impl(env, __VA_ARGS__);                                                                              \
        } while(0);

#define CALL_IMPL_NO_ARGS_E(impl, env)                                                                                  \
        do {                                                                                                            \
            OCILIB_LOG_STEP_WITH_ENV((env), OCI_LOG_CAT_LIB, OCI_LOG_TRACE, "ENTER", __func__, OTEXT("%s"), OTEXT("")); \
            OcilibCheckContext();                                                                                       \
            return impl(env);                                                                                           \
        } while(0);

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
    CALL_IMPL(OcilibAgentCreate, con, name, address);
}

OCI_Boolean OCI_API OCI_AgentFree
(
    OCI_Agent* agent
)
{
    CALL_IMPL(OcilibAgentFree, agent);
}

const otext* OCI_API OCI_AgentGetName
(
    OCI_Agent* agent
)
{
    CALL_IMPL(OcilibAgentGetName, agent);
}

OCI_Boolean OCI_API OCI_AgentSetName
(
    OCI_Agent  * agent,
    const otext* name
)
{
    CALL_IMPL(OcilibAgentSetName, agent, name);
}

const otext* OCI_API OCI_AgentGetAddress
(
    OCI_Agent* agent
)
{
    CALL_IMPL(OcilibAgentGetAddress, agent);
}

OCI_Boolean OCI_API OCI_AgentSetAddress
(
    OCI_Agent  * agent,
    const otext* address
)
{
    CALL_IMPL(OcilibAgentSetAddress, agent, address);
}

/* --------------------------------------------------------------------------------------------- *
  * bind
  * --------------------------------------------------------------------------------------------- */

const otext* OCI_API OCI_BindGetName
(
    OCI_Bind* bnd
)
{
    CALL_IMPL(OcilibBindGetName, bnd);
}

OCI_SqlType OCI_API OCI_BindGetType
(
    OCI_Bind* bnd
)
{
    CALL_IMPL(OcilibBindGetType, bnd);
}

OCI_SubType OCI_API OCI_BindGetSubtype
(
    OCI_Bind* bnd
)
{
    CALL_IMPL(OcilibBindGetSubtype, bnd);
}

OCI_UInt32 OCI_API OCI_BindGetDataCount
(
    OCI_Bind* bnd
)
{
    CALL_IMPL(OcilibBindGetDataCount, bnd);
}

void* OCI_API OCI_BindGetData
(
    OCI_Bind* bnd
)
{
    CALL_IMPL(OcilibBindGetData, bnd);
}

OCI_Statement* OCI_API OCI_BindGetStatement
(
    OCI_Bind* bnd
)
{
    CALL_IMPL(OcilibBindGetStatement, bnd);
}

OCI_Boolean OCI_API OCI_BindSetDataSize
(
    OCI_Bind * bnd,
    OCI_UInt32 size
)
{
    CALL_IMPL(OcilibBindSetDataSize, bnd, size);
}

OCI_Boolean OCI_API OCI_BindSetDataSizeAtPos
(
    OCI_Bind * bnd,
    OCI_UInt32 position,
    OCI_UInt32 size
)
{
    CALL_IMPL(OcilibBindSetDataSizeAtPos, bnd, position, size);
}

OCI_UInt32 OCI_API OCI_BindGetDataSize
(
    OCI_Bind* bnd
)
{
    CALL_IMPL(OcilibBindGetDataSize, bnd);
}

OCI_UInt32 OCI_API OCI_BindGetDataSizeAtPos
(
    OCI_Bind * bnd,
    OCI_UInt32 position
)
{
    CALL_IMPL(OcilibBindGetDataSizeAtPos, bnd, position);
}

OCI_Boolean OCI_API OCI_BindSetNullAtPos
(
    OCI_Bind * bnd,
    OCI_UInt32 position
)
{
    CALL_IMPL(OcilibBindSetNullAtPos, bnd, position);
}

OCI_Boolean OCI_API OCI_BindSetNull
(
    OCI_Bind* bnd
)
{
    CALL_IMPL(OcilibBindSetNull, bnd);
}

OCI_Boolean OCI_API OCI_BindSetNotNullAtPos
(
    OCI_Bind * bnd,
    OCI_UInt32 position
)
{
    CALL_IMPL(OcilibBindSetNotNullAtPos, bnd, position);
}

OCI_Boolean OCI_API OCI_BindSetNotNull
(
    OCI_Bind* bnd
)
{
    CALL_IMPL(OcilibBindSetNotNull, bnd);
}

OCI_Boolean OCI_API OCI_BindIsNullAtPos
(
    OCI_Bind * bnd,
    OCI_UInt32 position
)
{
    CALL_IMPL(OcilibBindIsNullAtPos, bnd, position);
}

OCI_Boolean OCI_API OCI_BindIsNull
(
    OCI_Bind* bnd
)
{
    CALL_IMPL(OcilibBindIsNull, bnd);
}

OCI_Boolean OCI_API OCI_BindSetCharsetForm
(
    OCI_Bind      * bnd,
    OCI_CharsetForm csfrm
)
{
    CALL_IMPL(OcilibBindSetCharsetForm, bnd, csfrm);
}

OCI_Boolean OCI_API OCI_BindSetDirection
(
    OCI_Bind        * bnd,
    OCI_BindDirection direction
)
{
    CALL_IMPL(OcilibBindSetDirection, bnd, direction);
}

OCI_BindDirection OCI_API OCI_BindGetDirection
(
    OCI_Bind* bnd
)
{
    CALL_IMPL(OcilibBindGetDirection, bnd);
}

OCI_BindAllocationMode OCI_API OCI_BindGetAllocationMode
(
    OCI_Bind* bnd
)
{
    CALL_IMPL(OcilibBindGetAllocationMode, bnd);
}

/* --------------------------------------------------------------------------------------------- *
 * collection
 * --------------------------------------------------------------------------------------------- */

OCI_Collection * OCI_API OCI_CollectionCreate
(
    OCI_TypeInfo *typinf
)
{
    CALL_IMPL(OcilibCollectionCreate, typinf);
}

OCI_Boolean OCI_API OCI_CollectionFree
(
    OCI_Collection *coll
)
{
    CALL_IMPL(OcilibCollectionFree, coll);
}

OCI_Collection ** OCI_API OCI_CollectionArrayCreate
(
    OCI_TypeInfo *typinf,
    OCI_UInt32    nbelem
)
{
    CALL_IMPL(OcilibCollectionArrayCreate, typinf, nbelem);
}

OCI_Boolean OCI_API OCI_CollectionArrayFree
(
    OCI_Collection **colls
)
{
    CALL_IMPL(OcilibCollectionArrayFree, colls);
}

OCI_Boolean OCI_API OCI_CollectionAssign
(
    OCI_Collection *coll,
    OCI_Collection *coll_src
)
{
    CALL_IMPL(OcilibCollectionAssign, coll, coll_src);
}

OCI_CollType OCI_API OCI_CollectionGetType
(
    OCI_Collection *coll
)
{
    CALL_IMPL(OcilibCollectionGetType, coll);
}

OCI_UInt32 OCI_API OCI_CollectionGetMax
(
    OCI_Collection *coll
)
{
    CALL_IMPL(OcilibCollectionGetMax, coll);
}

OCI_UInt32 OCI_API OCI_CollectionGetSize
(
    OCI_Collection *coll
)
{
    CALL_IMPL(OcilibCollectionGetSize, coll);
}

OCI_Boolean OCI_API OCI_CollectionTrim
(
    OCI_Collection *coll,
    OCI_UInt32      nb_elem
)
{
    CALL_IMPL(OcilibCollectionTrim, coll, nb_elem);
}

OCI_Element * OCI_API OCI_CollectionGetElement
(
    OCI_Collection *coll,
    OCI_UInt32      index
)
{
    CALL_IMPL(OcilibCollectionGetElement, coll, index);
}

OCI_Boolean OCI_API OCI_CollectionSetElement
(
    OCI_Collection *coll,
    OCI_UInt32      index,
    OCI_Element    *elem
)
{
    CALL_IMPL(OcilibCollectionSetElement, coll, index, elem);
}

OCI_Boolean OCI_API OCI_CollectionAppend
(
    OCI_Collection *coll,
    OCI_Element    *elem
)
{
    CALL_IMPL(OcilibCollectionAppend, coll, elem);
}

OCI_TypeInfo * OCI_API OCI_CollectionGetTypeInfo
(
    OCI_Collection *coll
)
{
    CALL_IMPL(OcilibCollectionGetTypeInfo, coll);
}

OCI_Boolean OCI_API OCI_CollectionClear
(
    OCI_Collection *coll
)
{
    CALL_IMPL(OcilibCollectionClear, coll);
}

OCI_Boolean OCI_API OCI_CollectionDeleteElement
(
    OCI_Collection *coll,
    OCI_UInt32      index
)
{
    CALL_IMPL(OcilibCollectionDeleteElement, coll, index);
}

OCI_UInt32 OCI_API OCI_CollectionGetCount
(
    OCI_Collection *coll
)
{
    CALL_IMPL(OcilibCollectionGetCount, coll);
}

OCI_Boolean OCI_API OCI_CollectionToText
(
    OCI_Collection *coll,
    OCI_UInt32     *size,
    otext          *str
)
{
    CALL_IMPL(OcilibCollectionToText, coll, size, str);
}

/* --------------------------------------------------------------------------------------------- *
 * column
 * --------------------------------------------------------------------------------------------- */

const otext* OCI_API OCI_ColumnGetName
(
    OCI_Column* col
)
{
    CALL_IMPL(OcilibColumnGetName, col);
}

OCI_SqlType OCI_API OCI_ColumnGetType
(
    OCI_Column* col
)
{
    CALL_IMPL(OcilibColumnGetType, col);
}

OCI_CharsetForm OCI_API OCI_ColumnGetCharsetForm
(
    OCI_Column* col
)
{
    CALL_IMPL(OcilibColumnGetCharsetForm, col);
}

OCI_UInt32 OCI_API OCI_ColumnGetSize
(
    OCI_Column* col
)
{
    CALL_IMPL(OcilibColumnGetSize, col);
}

OCI_Int32 OCI_API OCI_ColumnGetScale
(
    OCI_Column* col
)
{
    CALL_IMPL(OcilibColumnGetScale, col);
}

OCI_Int32 OCI_API OCI_ColumnGetPrecision
(
    OCI_Column* col
)
{
    CALL_IMPL(OcilibColumnGetPrecision, col);
}

OCI_Int32 OCI_API OCI_ColumnGetFractionalPrecision
(
    OCI_Column* col
)
{
    CALL_IMPL(OcilibColumnGetFractionalPrecision, col);
}

OCI_Int32 OCI_API OCI_ColumnGetLeadingPrecision
(
    OCI_Column* col
)
{
    CALL_IMPL(OcilibColumnGetLeadingPrecision, col);
}

OCI_Boolean OCI_API OCI_ColumnGetNullable
(
    OCI_Column* col
)
{
    CALL_IMPL(OcilibColumnGetNullable, col);
}

OCI_Boolean OCI_API OCI_ColumnGetCharUsed
(
    OCI_Column* col
)
{
    CALL_IMPL(OcilibColumnGetCharUsed, col);
}

OCI_ColumnPropFlags OCI_API OCI_ColumnGetPropertyFlags
(
    OCI_Column* col
)
{
    CALL_IMPL(OcilibColumnGetPropertyFlags, col);
}

OCI_CollationID OCI_API OCI_ColumnGetCollationID
(
    OCI_Column* col
)
{
    CALL_IMPL(OcilibColumnGetCollationID, col);
}

OCI_UInt32 OCI_API OCI_ColumnGetDimension
(
    OCI_Column* col
)
{
    CALL_IMPL(OcilibColumnGetDimension, col);
}

const otext* OCI_API OCI_ColumnGetSqlType
(
    OCI_Column* col
)
{
    CALL_IMPL(OcilibColumnGetSqlType, col);
}

OCI_UInt32 OCI_API OCI_ColumnGetFullSqlType
(
    OCI_Column * col,
    otext      * buffer,
    OCI_UInt32   len
)
{
    CALL_IMPL(OcilibColumnGetFullSqlType, col, buffer, len);
}

OCI_TypeInfo* OCI_API OCI_ColumnGetTypeInfo
(
    OCI_Column* col
)
{
    CALL_IMPL(OcilibColumnGetTypeInfo, col);
}

OCI_Boolean OCI_API OCI_ColumnHasJsonSchema
(
    OCI_Column * col
)
{
    CALL_IMPL(OcilibColumnHasJsonSchema, col);
}

OCI_SubType OCI_API OCI_ColumnGetSubType
(
    OCI_Column* col
)
{
    CALL_IMPL(OcilibColumnGetSubType, col);
}

/* --------------------------------------------------------------------------------------------- *
 * sharding key
 * --------------------------------------------------------------------------------------------- */

OCI_ShardingKey* OCI_API OCI_ShardingKeyCreate
(
    OCI_Environment* env
)
{
    CALL_IMPL_NO_ARGS_E(OcilibShardingKeyCreate, env);
}

OCI_Boolean OCI_API OCI_ShardingKeyFree
(
    OCI_ShardingKey* skey
)
{
    CALL_IMPL(OcilibShardingKeyFree, skey);
}

OCI_Boolean OCI_API OCI_ShardingKeyReset
(
    OCI_ShardingKey* skey
)
{
    CALL_IMPL(OcilibShardingKeyReset, skey);
}

OCI_Boolean OCI_API OCI_ShardingKeyAddString
(
    OCI_ShardingKey* skey,
    const otext    * value
)
{
    CALL_IMPL(OcilibShardingKeyAddString, skey, value);
}

OCI_Boolean OCI_API OCI_ShardingKeyAddNumber
(
    OCI_ShardingKey* skey,
    OCI_Number     * value
)
{
    CALL_IMPL(OcilibShardingKeyAddNumber, skey, value);
}

OCI_Boolean OCI_API OCI_ShardingKeyAddDate
(
    OCI_ShardingKey* skey,
    OCI_Date       * value
)
{
    CALL_IMPL(OcilibShardingKeyAddDate, skey, value);
}

OCI_Boolean OCI_API OCI_ShardingKeyAddTimestamp
(
    OCI_ShardingKey* skey,
    OCI_Timestamp  * value
)
{
    CALL_IMPL(OcilibShardingKeyAddTimestamp, skey, value);
}

OCI_Boolean OCI_API OCI_ShardingKeyAddRaw
(
    OCI_ShardingKey* skey,
    const void     * value,
    OCI_UInt32       size
)
{
    CALL_IMPL(OcilibShardingKeyAddRaw, skey, value, size);
}

const otext* OCI_API OCI_ShardingKeyToText
(
    OCI_ShardingKey* skey
)
{
    CALL_IMPL(OcilibShardingKeyToText, skey);
}

/* --------------------------------------------------------------------------------------------- *
 * connection
 * --------------------------------------------------------------------------------------------- */

OCI_Connection * OCI_API OCI_ConnectionCreate
(
    OCI_Environment *env,
    const otext     *db,
    const otext     *user,
    const otext     *pwd,
    OCI_SessionMode  mode
)
{
    CALL_IMPL_E(OcilibConnectionCreate, env, db, user, pwd, mode);
}

OCI_Connection * OCI_API OCI_ConnectionCreateSharded
(
    OCI_Environment *env,
    const otext     *db,
    const otext     *user,
    const otext     *pwd,
    OCI_SessionMode  mode,
    OCI_ShardingKey *skey,
    OCI_ShardingKey *sskey
)
{
    CALL_IMPL_E(OcilibConnectionCreateSharded, env, db, user, pwd, mode, skey, sskey);
}

OCI_Boolean OCI_API OCI_ConnectionFree
(
    OCI_Connection *con
)
{
    CALL_IMPL(OcilibConnectionFree, con);
}

OCI_Boolean OCI_API OCI_ConnectionCommit
(
    OCI_Connection *con
)
{
    CALL_IMPL(OcilibConnectionCommit, con);
}

OCI_Boolean OCI_API OCI_ConnectionRollback
(
    OCI_Connection *con
)
{
    CALL_IMPL(OcilibConnectionRollback, con);
}

OCI_Boolean OCI_API OCI_ConnectionSetAutoCommit
(
    OCI_Connection *con,
    OCI_Boolean     enable
)
{
    CALL_IMPL(OcilibConnectionSetAutoCommit, con, enable);
}

OCI_Boolean OCI_API OCI_ConnectionGetAutoCommit
(
    OCI_Connection *con
)
{
    CALL_IMPL(OcilibConnectionGetAutoCommit, con);
}

OCI_Boolean OCI_API OCI_ConnectionIsAlive
(
    OCI_Connection *con
)
{
    CALL_IMPL(OcilibConnectionIsAlive, con);
}

void * OCI_API OCI_HandleGetUserData
(
    void *handle
)
{
    CALL_IMPL(OcilibHandleGetUserData, handle);
}

OCI_Boolean OCI_API OCI_HandleSetUserData
(
    void *handle,
    void *data
)
{
    CALL_IMPL(OcilibHandleSetUserData, handle, data);
}

OCI_Boolean OCI_API OCI_ConnectionSetSessionTag
(
    OCI_Connection *con,
    const otext    *tag
)
{
    CALL_IMPL(OcilibConnectionSetSessionTag, con, tag);
}

const otext * OCI_API OCI_ConnectionGetSessionTag
(
    OCI_Connection *con
)
{
    CALL_IMPL(OcilibConnectionGetSessionTag, con);
}

const otext * OCI_API OCI_ConnectionGetConnectionString
(
    OCI_Connection *con
)
{
    CALL_IMPL(OcilibConnectionGetConnectionString, con);
}

const otext * OCI_API OCI_ConnectionGetUserName
(
    OCI_Connection *con
)
{
    CALL_IMPL(OcilibConnectionGetUserName, con);
}

const otext * OCI_API OCI_ConnectionGetPassword
(
    OCI_Connection *con
)
{
    CALL_IMPL(OcilibConnectionGetPassword, con);
}

OCI_Boolean OCI_API OCI_ConnectionSetPassword
(
    OCI_Connection *con,
    const otext    *password
)
{
    CALL_IMPL(OcilibConnectionSetPassword, con, password);
}

OCI_SessionMode OCI_API OCI_ConnectionGetSessionMode
(
    OCI_Connection *con
)
{
    CALL_IMPL(OcilibConnectionGetSessionMode, con);
}

const otext * OCI_API OCI_ConnectionGetServerVersionString
(
    OCI_Connection *con
)
{
    CALL_IMPL(OcilibConnectionGetServerVersionString, con);
}

OCI_UInt32 OCI_API OCI_ConnectionGetServerMajorVersion
(
    OCI_Connection *con
)
{
    CALL_IMPL(OcilibConnectionGetServerMajorVersion, con);
}

OCI_UInt32 OCI_API OCI_ConnectionGetServerMinorVersion
(
    OCI_Connection *con
)
{
    CALL_IMPL(OcilibConnectionGetServerMinorVersion, con);
}

OCI_UInt32 OCI_API OCI_ConnectionGetServerRevisionVersion
(
    OCI_Connection *con
)
{
    CALL_IMPL(OcilibConnectionGetServerRevisionVersion, con);
}

OCI_Transaction * OCI_API OCI_ConnectionGetTransaction
(
    OCI_Connection *con
)
{
    CALL_IMPL(OcilibConnectionGetTransaction, con);
}

OCI_Boolean OCI_API OCI_ConnectionSetTransaction
(
    OCI_Connection  *con,
    OCI_Transaction *trans
)
{
    CALL_IMPL(OcilibConnectionSetTransaction, con, trans);
}

OCI_UInt32 OCI_API OCI_ConnectionGetVersion
(
    OCI_Connection *con
)
{
    CALL_IMPL(OcilibConnectionGetVersion, con);
}

OCI_Boolean OCI_API OCI_ConnectionBreak
(
    OCI_Connection *con
)
{
    CALL_IMPL(OcilibConnectionBreak, con);
}

OCI_Boolean OCI_API OCI_ConnectionEnableServerOutput
(
    OCI_Connection *con,
    OCI_UInt32      bufsize,
    OCI_UInt32      arrsize,
    OCI_UInt32      lnsize
)
{
    CALL_IMPL(OcilibConnectionEnableServerOutput, con, bufsize, arrsize, lnsize);
}

OCI_Boolean OCI_API OCI_ConnectionDisableServerOutput
(
    OCI_Connection *con
)
{
    CALL_IMPL(OcilibConnectionDisableServerOutput, con);
}

const otext * OCI_API OCI_ConnectionGetServerOutput
(
    OCI_Connection *con
)
{
    CALL_IMPL(OcilibConnectionGetServerOutput, con);
}

OCI_Boolean OCI_API OCI_ConnectionSetTrace
(
    OCI_Connection *con,
    OCI_TraceType   trace,
    const otext    *value
)
{
    CALL_IMPL(OcilibConnectionSetTrace, con, trace, value);
}

const otext * OCI_API OCI_ConnectionGetTrace
(
    OCI_Connection *con,
    OCI_TraceType   trace
)
{
    CALL_IMPL(OcilibConnectionGetTrace, con, trace);
}

OCI_Boolean OCI_API OCI_ConnectionPing
(
    OCI_Connection *con
)
{
    CALL_IMPL(OcilibConnectionPing, con);
}

OCI_Boolean OCI_API OCI_ConnectionSetTimeout
(
    OCI_Connection *con,
    OCI_TimeoutType type,
    OCI_UInt32      value
)
{
    CALL_IMPL(OcilibConnectionSetTimeout, con, type, value);
}

OCI_UInt32 OCI_API OCI_ConnectionGetTimeout
(
    OCI_Connection *con,
    OCI_TimeoutType type
)
{
    CALL_IMPL(OcilibConnectionGetTimeout, con, type);
}

const otext * OCI_API OCI_ConnectionGetDatabaseName
(
    OCI_Connection *con
)
{
    CALL_IMPL(OcilibConnectionGetDatabaseName, con);
}

const otext * OCI_API OCI_ConnectionGetInstanceName
(
    OCI_Connection *con
)
{
    CALL_IMPL(OcilibConnectionGetInstanceName, con);
}

const otext * OCI_API OCI_ConnectionGetServiceName
(
    OCI_Connection *con
)
{
    CALL_IMPL(OcilibConnectionGetServiceName, con);
}

const otext * OCI_API OCI_ConnectionGetServerName
(
    OCI_Connection *con
)
{
    CALL_IMPL(OcilibConnectionGetServerName, con);
}

const otext * OCI_API OCI_ConnectionGetDomainName
(
    OCI_Connection *con
)
{
    CALL_IMPL(OcilibConnectionGetDomainName, con);
}

OCI_Timestamp * OCI_API OCI_ConnectionGetInstanceStartTime
(
    OCI_Connection *con
)
{
    CALL_IMPL(OcilibConnectionGetInstanceStartTime, con);
}

OCI_Boolean OCI_API OCI_ConnectionIsTAFCapable
(
    OCI_Connection *con
)
{
    CALL_IMPL(OcilibConnectionIsTAFCapable, con);
}

OCI_Boolean OCI_API OCI_ConnectionSetTAFHandler
(
    OCI_Connection  *con,
    POCI_TAF_HANDLER handler
)
{
    CALL_IMPL(OcilibConnectionSetTAFHandler, con, handler);
}

OCI_UInt32 OCI_API OCI_ConnectionGetStatementCacheSize
(
    OCI_Connection *con
)
{
    CALL_IMPL(OcilibConnectionGetStatementCacheSize, con);
}

OCI_Boolean OCI_API OCI_ConnectionSetStatementCacheSize
(
    OCI_Connection *con,
    OCI_UInt32      value
)
{
    CALL_IMPL(OcilibConnectionSetStatementCacheSize, con, value);
}

OCI_UInt32 OCI_API OCI_ConnectionGetDefaultLobPrefetchSize
(
    OCI_Connection *con
)
{
    CALL_IMPL(OcilibConnectionGetDefaultLobPrefetchSize, con);
}

OCI_Boolean OCI_API OCI_ConnectionSetDefaultLobPrefetchSize
(
    OCI_Connection *con,
    OCI_UInt32      value
)
{
    CALL_IMPL(OcilibConnectionSetDefaultLobPrefetchSize, con, value);
}

OCI_UInt32 OCI_API OCI_ConnectionGetMaxCursors
(
    OCI_Connection *con
)
{
    CALL_IMPL(OcilibConnectionGetMaxCursors, con);
}

OCI_Boolean OCI_ConnectionImmediate
(
    OCI_Connection *con,
    const otext    *sql,
    ...
)
{
    OCI_Boolean res = FALSE;
    va_list     args;

    va_start(args, sql);
    res = OcilibConnectionImmediate(con, sql, args);
    va_end(args);

    return res;
}

OCI_Boolean OCI_ConnectionImmediateFormat
(
    OCI_Connection *con,
    const otext    *sql,
    ...
)
{
    OCI_Boolean res = FALSE;
    va_list     args;

    va_start(args, sql);
    res = OcilibConnectionImmediateFormat(con, sql, args);
    va_end(args);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * date
 * --------------------------------------------------------------------------------------------- */

OCI_Date* OCI_API OCI_DateCreate
(
    OCI_Environment* env
)
{
    CALL_IMPL_NO_ARGS_E(OcilibDateCreate, env);
}

OCI_Boolean OCI_API OCI_DateFree
(
    OCI_Date* date
)
{
    CALL_IMPL(OcilibDateFree, date);
}

OCI_Date** OCI_API OCI_DateArrayCreate
(
    OCI_Environment* env,
    OCI_UInt32       nbelem
)
{
    CALL_IMPL_E(OcilibDateCreateArray, env, nbelem);
}

OCI_Boolean OCI_API OCI_DateArrayFree
(
    OCI_Date** dates
)
{
    CALL_IMPL(OcilibDateFreeArray, dates);
}

OCI_Boolean OCI_API OCI_DateAddDays
(
    OCI_Date* date,
    OCI_Int32 nb
)
{
    CALL_IMPL(OcilibDateAddDays, date, nb);
}

OCI_Boolean OCI_API OCI_DateAddMonths
(
    OCI_Date* date,
    OCI_Int32 nb
)
{
    CALL_IMPL(OcilibDateAddMonths, date, nb);
}

OCI_Boolean OCI_API OCI_DateAssign
(
    OCI_Date* date,
    OCI_Date* date_src
)
{
    CALL_IMPL(OcilibDateAssign, date, date_src);
}

OCI_Int32 OCI_API OCI_DateCheck
(
    OCI_Date* date
)
{
    CALL_IMPL(OcilibDateCheck, date);
}

OCI_Int32 OCI_API OCI_DateCompare
(
    OCI_Date* date,
    OCI_Date* date2
)
{
    CALL_IMPL(OcilibDateCompare, date, date2);
}

OCI_Int32 OCI_API OCI_DateDaysBetween
(
    OCI_Date* date,
    OCI_Date* date2
)
{
    CALL_IMPL(OcilibDateDaysBetween, date, date2);
}

OCI_Boolean OCI_API OCI_DateFromText
(
    OCI_Date   * date,
    const otext* str,
    const otext* fmt
)
{
    CALL_IMPL(OcilibDateFromString, date, str, fmt);
}

OCI_Boolean OCI_API OCI_DateGetDate
(
    OCI_Date  * date,
    OCI_Int32 * year,
    OCI_Int32 * month,
    OCI_Int32 * day
)
{
    CALL_IMPL(OcilibDateGetDate, date, year, month, day);
}

OCI_Boolean OCI_API OCI_DateGetTime
(
    OCI_Date  * date,
    OCI_Int32 * hour,
    OCI_Int32 * min,
    OCI_Int32 * sec
)
{
    CALL_IMPL(OcilibDateGetTime, date, hour, min, sec);
}

OCI_Boolean OCI_API OCI_DateGetDateTime
(
    OCI_Date  * date,
    OCI_Int32 * year,
    OCI_Int32 * month,
    OCI_Int32 * day,
    OCI_Int32 * hour,
    OCI_Int32 * min,
    OCI_Int32 * sec
)
{
    CALL_IMPL(OcilibDateGetDateTime, date, year, month, day, hour, min, sec);
}

OCI_Boolean OCI_API OCI_DateLastDay
(
    OCI_Date* date
)
{
    CALL_IMPL(OcilibDateLastDay, date);
}

OCI_Boolean OCI_API OCI_DateNextDay
(
    OCI_Date   * date,
    const otext* day
)
{
    CALL_IMPL(OcilibDateNextDay, date, day);
}

OCI_Boolean OCI_API OCI_DateSetDate
(
    OCI_Date* date,
    OCI_Int32 year,
    OCI_Int32 month,
    OCI_Int32 day
)
{
    CALL_IMPL(OcilibDateSetDate, date, year, month, day);
}

OCI_Boolean OCI_API OCI_DateSetTime
(
    OCI_Date* date,
    OCI_Int32 hour,
    OCI_Int32 min,
    OCI_Int32 sec
)
{
    CALL_IMPL(OcilibDateSetTime, date, hour, min, sec);
}

OCI_Boolean OCI_API OCI_DateSetDateTime
(
    OCI_Date* date,
    OCI_Int32 year,
    OCI_Int32 month,
    OCI_Int32 day,
    OCI_Int32 hour,
    OCI_Int32 min,
    OCI_Int32 sec
)
{
    CALL_IMPL(OcilibDateSetDateTime, date, year, month, day, hour, min,sec);
}

OCI_Boolean OCI_API OCI_DateSysDate
(
    OCI_Date* date
)
{
    CALL_IMPL(OcilibDateSysDate, date);
}

OCI_Boolean OCI_API OCI_DateToText
(
    OCI_Date   * date,
    const otext* fmt,
    OCI_Int32    size,
    otext      * str
)
{
    CALL_IMPL(OcilibDateToString, date, fmt, size, str);
}

OCI_Boolean OCI_API OCI_DateZoneToZone
(
    OCI_Date   * date,
    const otext* zone1,
    const otext* zone2
)
{
    CALL_IMPL(OcilibDateZoneToZone, date, zone1, zone2);
}

OCI_Boolean OCI_API OCI_DateToCTime
(
    OCI_Date * date,
    struct tm* ptm,
    time_t   * pt
)
{
    CALL_IMPL(OcilibDateToCTime, date, ptm, pt);
}

OCI_Boolean OCI_API OCI_DateFromCTime
(
    OCI_Date * date,
    struct tm* ptm,
    time_t     t
)
{
    CALL_IMPL(OcilibDateFromCTime, date, ptm, t);
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
    CALL_IMPL(OcilibDequeueCreate,typinf, name);
}

OCI_Boolean OCI_API OCI_DequeueFree
(
    OCI_Dequeue *dequeue
)
{
    CALL_IMPL(OcilibDequeueFree, dequeue);
}

OCI_Message * OCI_API OCI_DequeueGet
(
    OCI_Dequeue *dequeue
)
{
    CALL_IMPL(OcilibDequeueGetMessage, dequeue);
}
OCI_Boolean OCI_API OCI_DequeueSubscribe
(
    OCI_Connection* con,
    OCI_Dequeue  ** dequeues,
    OCI_UInt32      count,
    OCI_UInt32      timeout,
    POCI_NOTIFY_AQ  callback
)
{
    CALL_IMPL(OcilibDequeueSubscribe, con, dequeues, count, timeout, callback);
}

OCI_Boolean OCI_API OCI_DequeueUnsubscribe
(
    OCI_Dequeue *dequeue
)
{
    CALL_IMPL(OcilibDequeueUnsubscribe, dequeue);
}

OCI_Boolean OCI_API OCI_DequeueSetConsumer
(
    OCI_Dequeue *dequeue,
    const otext *consumer
)
{
    CALL_IMPL(OcilibDequeueSetConsumer, dequeue, consumer);
}

const otext * OCI_API OCI_DequeueGetConsumer
(
    OCI_Dequeue *dequeue
)
{
    CALL_IMPL(OcilibDequeueGetConsumer, dequeue);
}

OCI_Boolean OCI_API OCI_DequeueSetCorrelation
(
    OCI_Dequeue *dequeue,
    const otext *pattern
)
{
    CALL_IMPL(OcilibDequeueSetCorrelation, dequeue, pattern);
}

const otext * OCI_API OCI_DequeueGetCorrelation
(
    OCI_Dequeue *dequeue
)
{
    CALL_IMPL(OcilibDequeueGetCorrelation, dequeue);
}

OCI_Boolean OCI_API OCI_DequeueSetRelativeMsgID
(
    OCI_Dequeue *dequeue,
    const void  *id,
    OCI_UInt32   len
)
{
    CALL_IMPL(OcilibDequeueSetRelativeMsgID, dequeue, id, len);
}

OCI_Boolean OCI_API OCI_DequeueGetRelativeMsgID
(
    OCI_Dequeue *dequeue,
    void        *id,
    OCI_UInt32  *len
)
{
    CALL_IMPL(OcilibDequeueGetRelativeMsgID, dequeue, id, len);
}

OCI_Boolean OCI_API OCI_DequeueSetVisibility
(
    OCI_Dequeue        *dequeue,
    OCI_QueueVisibility visibility
)
{
    CALL_IMPL(OcilibDequeueSetVisibility, dequeue, visibility);
}

OCI_QueueVisibility OCI_API OCI_DequeueGetVisibility
(
    OCI_Dequeue *dequeue
)
{
    CALL_IMPL(OcilibDequeueGetVisibility, dequeue);
}

OCI_Boolean OCI_API OCI_DequeueSetMode
(
    OCI_Dequeue    *dequeue,
    OCI_DequeueMode mode
)
{
    CALL_IMPL(OcilibDequeueSetMode, dequeue, mode);
}

OCI_DequeueMode OCI_API OCI_DequeueGetMode
(
    OCI_Dequeue *dequeue
)
{
    CALL_IMPL(OcilibDequeueGetMode, dequeue);
}

OCI_Boolean OCI_API OCI_DequeueSetNavigation
(
    OCI_Dequeue *dequeue,
    OCI_UInt32   position
)
{
    CALL_IMPL(OcilibDequeueSetNavigation, dequeue, position);
}

OCI_DequeueNav OCI_API OCI_DequeueGetNavigation
(
    OCI_Dequeue *dequeue
)
{
    CALL_IMPL(OcilibDequeueGetNavigation, dequeue);
}

OCI_Boolean OCI_API OCI_DequeueSetWaitTime
(
    OCI_Dequeue *dequeue,
    OCI_Int32    timeout
)
{
    CALL_IMPL(OcilibDequeueSetWaitTime, dequeue, timeout);
}

OCI_Int32 OCI_API OCI_DequeueGetWaitTime
(
    OCI_Dequeue *dequeue
)
{
    CALL_IMPL(OcilibDequeueGetWaitTime, dequeue);
}

OCI_Boolean OCI_API OCI_DequeueSetAgentList
(
    OCI_Dequeue *dequeue,
    OCI_Agent  **consumers,
    OCI_UInt32   count
)
{
    CALL_IMPL(OcilibDequeueSetAgentList, dequeue, consumers, count);
}

OCI_Agent * OCI_API OCI_DequeueListen
(
    OCI_Dequeue *dequeue,
    OCI_Int32    timeout
)
{
    CALL_IMPL(OcilibDequeueListen, dequeue, timeout);
}

/* --------------------------------------------------------------------------------------------- *
 * direct path
 * --------------------------------------------------------------------------------------------- */

OCI_DirectPath* OCI_API OCI_DirectPathCreate
(
    OCI_TypeInfo* typinf,
    const otext * partition,
    OCI_UInt32    nb_cols,
    OCI_UInt32    nb_rows
)
{
    CALL_IMPL(OcilibDirectPathCreate, typinf, partition, nb_cols, nb_rows);
}

OCI_Boolean OCI_API OCI_DirectPathFree
(
    OCI_DirectPath* dp
)
{
    CALL_IMPL(OcilibDirectPathFree, dp);
}

OCI_Boolean OCI_API OCI_DirectPathSetColumn
(
    OCI_DirectPath* dp,
    OCI_UInt32      index,
    const otext   * name,
    OCI_UInt32      maxsize,
    const otext   * format
)
{
    CALL_IMPL(OcilibDirectPathSetColumn, dp, index, name, maxsize, format);
}

OCI_Boolean OCI_API OCI_DirectPathPrepare
(
    OCI_DirectPath* dp
)
{
    CALL_IMPL(OcilibDirectPathPrepare, dp);
}

OCI_Boolean OCI_API OCI_DirectPathSetEntry
(
    OCI_DirectPath* dp,
    OCI_UInt32      row,
    OCI_UInt32      index,
    const void    * value,
    unsigned        size,
    OCI_Boolean     complete
)
{
    CALL_IMPL(OcilibDirectPathSetEntry, dp, row, index, value, size, complete);
}

OCI_DirPathResult OCI_API OCI_DirectPathConvert
(
    OCI_DirectPath* dp
)
{
    CALL_IMPL(OcilibDirectPathConvert, dp);
}

OCI_DirPathResult OCI_API OCI_DirectPathLoad
(
    OCI_DirectPath* dp
)
{
    CALL_IMPL(OcilibDirectPathLoad, dp);
}

OCI_Boolean OCI_API OCI_DirectPathReset
(
    OCI_DirectPath* dp
)
{
    CALL_IMPL(OcilibDirectPathReset, dp);
}

OCI_Boolean OCI_API OCI_DirectPathFinish
(
    OCI_DirectPath* dp
)
{
    CALL_IMPL(OcilibDirectPathFinish, dp);
}

OCI_Boolean OCI_API OCI_DirectPathAbort
(
    OCI_DirectPath* dp
)
{
    CALL_IMPL(OcilibDirectPathAbort, dp);
}

OCI_Boolean OCI_API OCI_DirectPathSave
(
    OCI_DirectPath* dp
)
{
    CALL_IMPL(OcilibDirectPathSave, dp);
}

OCI_Boolean OCI_API OCI_DirectPathFlushRow
(
    OCI_DirectPath* dp
)
{
    CALL_IMPL(OcilibDirectPathFlushRow, dp);
}

OCI_Boolean OCI_API OCI_DirectPathSetCurrentRows
(
    OCI_DirectPath* dp,
    OCI_UInt32      nb_rows
)
{
    CALL_IMPL(OcilibDirectPathSetCurrentRows, dp, nb_rows);
}

OCI_UInt32 OCI_API OCI_DirectPathGetCurrentRows
(
    OCI_DirectPath* dp
)
{
    CALL_IMPL(OcilibDirectPathGetCurrentRows, dp);
}

OCI_UInt32 OCI_API OCI_DirectPathGetMaxRows
(
    OCI_DirectPath* dp
)
{
    CALL_IMPL(OcilibDirectPathGetMaxRows, dp);
}

OCI_Boolean OCI_API OCI_DirectPathSetDateFormat
(
    OCI_DirectPath* dp,
    const otext   * format
)
{
    CALL_IMPL(OcilibDirectPathSetDateFormat, dp, format);
}

OCI_Boolean OCI_API OCI_DirectPathSetParallel
(
    OCI_DirectPath* dp,
    OCI_Boolean     value
)
{
    CALL_IMPL(OcilibDirectPathSetParallel, dp, value);
}

OCI_Boolean OCI_API OCI_DirectPathSetNoLog
(
    OCI_DirectPath* dp,
    OCI_Boolean     value
)
{
    CALL_IMPL(OcilibDirectPathSetNoLog, dp, value);
}

OCI_Boolean OCI_API OCI_DirectPathSetCacheSize
(
    OCI_DirectPath* dp,
    OCI_UInt32      size
)
{
    CALL_IMPL(OcilibDirectPathSetCacheSize, dp, size);
}

OCI_Boolean OCI_API OCI_DirectPathSetBufferSize
(
    OCI_DirectPath* dp,
    OCI_UInt32      size
)
{
    CALL_IMPL(OcilibDirectPathSetBufferSize, dp, size);
}

OCI_Boolean OCI_API OCI_DirectPathSetConvertMode
(
    OCI_DirectPath       * dp,
    OCI_DirPathConvertMode mode
)
{
    CALL_IMPL(OcilibDirectPathSetConvertMode, dp, mode);
}

OCI_UInt32 OCI_API OCI_DirectPathGetRowCount
(
    OCI_DirectPath* dp
)
{
    CALL_IMPL(OcilibDirectPathGetRowCount, dp);
}

OCI_UInt32 OCI_API OCI_DirectPathGetAffectedRows
(
    OCI_DirectPath* dp
)
{
    CALL_IMPL(OcilibDirectPathGetAffectedRows, dp);
}

OCI_Int32 OCI_API OCI_DirectPathGetErrorColumn
(
    OCI_DirectPath* dp
)
{
    CALL_IMPL(OcilibDirectPathGetErrorColumn, dp);
}

OCI_Int32 OCI_API OCI_DirectPathGetErrorRow
(
    OCI_DirectPath* dp
)
{
    CALL_IMPL(OcilibDirectPathGetErrorRow, dp);
}

/* --------------------------------------------------------------------------------------------- *
 * element
 * --------------------------------------------------------------------------------------------- */

OCI_Element* OCI_API OCI_ElementCreate
(
    OCI_TypeInfo* typinf
)
{
    CALL_IMPL(OcilibElementCreate, typinf);
}

OCI_Boolean OCI_API OCI_ElementFree
(
    OCI_Element* elem
)
{
    CALL_IMPL(OcilibElementFree, elem);
}

OCI_Boolean OCI_API OCI_ElementGetBoolean
(
    OCI_Element* elem
)
{
    CALL_IMPL(OcilibElementGetBoolean, elem);
}

OCI_Number* OCI_API OCI_ElementGetNumber
(
    OCI_Element* elem
)
{
    CALL_IMPL(OcilibElementGetNumber, elem);
}

OCI_Int16 OCI_API OCI_ElementGetInt16
(
    OCI_Element* elem
)
{
    CALL_IMPL(OcilibElementGetInt16, elem);
}

OCI_UInt16 OCI_API OCI_ElementGetUInt16
(
    OCI_Element* elem
)
{
    CALL_IMPL(OcilibElementGetUInt16, elem);
}

OCI_Int32 OCI_API OCI_ElementGetInt32
(
    OCI_Element* elem
)
{
    CALL_IMPL(OcilibElementGetInt32, elem);
}

OCI_UInt32 OCI_API OCI_ElementGetUInt32
(
    OCI_Element* elem
)
{
    CALL_IMPL(OcilibElementGetUInt32, elem);
}

OCI_Int64 OCI_API OCI_ElementGetInt64
(
    OCI_Element* elem
)
{
    CALL_IMPL(OcilibElementGetInt64, elem);
}

OCI_UInt64 OCI_API OCI_ElementGetUInt64
(
    OCI_Element* elem
)
{
    CALL_IMPL(OcilibElementGetUInt64, elem);
}

OCI_Double OCI_API OCI_ElementGetDouble
(
    OCI_Element* elem
)
{
    CALL_IMPL(OcilibElementGetDouble, elem);
}

OCI_Float OCI_API OCI_ElementGetFloat
(
    OCI_Element* elem
)
{
    CALL_IMPL(OcilibElementGetFloat, elem);
}

const otext* OCI_API OCI_ElementGetString
(
    OCI_Element* elem
)
{
    CALL_IMPL(OcilibElementGetString, elem);
}

OCI_UInt32 OCI_API OCI_ElementGetRaw
(
    OCI_Element * elem,
    void        * value,
    OCI_UInt32    len
)
{
    CALL_IMPL(OcilibElementGetRaw, elem, value, len);
}

OCI_UInt32 OCI_API OCI_ElementGetRawSize
(
    OCI_Element* elem
)
{
    CALL_IMPL(OcilibElementGetRawSize, elem);
}

OCI_Date* OCI_API OCI_ElementGetDate
(
    OCI_Element* elem
)
{
    CALL_IMPL(OcilibElementGetDate, elem);
}

OCI_Timestamp* OCI_API OCI_ElementGetTimestamp
(
    OCI_Element* elem
)
{
    CALL_IMPL(OcilibElementGetTimestamp, elem);
}

OCI_Interval* OCI_API OCI_ElementGetInterval
(
    OCI_Element* elem
)
{
    CALL_IMPL(OcilibElementGetInterval, elem);
}

OCI_Lob* OCI_API OCI_ElementGetLob
(
    OCI_Element* elem
)
{
    CALL_IMPL(OcilibElementGetLob, elem);
}

OCI_File* OCI_API OCI_ElementGetFile
(
    OCI_Element* elem
)
{
    CALL_IMPL(OcilibElementGetFile, elem);
}

OCI_Reference* OCI_API OCI_ElementGetReference
(
    OCI_Element* elem
)
{
    CALL_IMPL(OcilibElementGetReference, elem);
}

OCI_Object* OCI_API OCI_ElementGetObject
(
    OCI_Element* elem
)
{
    CALL_IMPL(OcilibElementGetObject, elem);
}

OCI_Collection* OCI_API OCI_ElementGetCollection
(
    OCI_Element* elem
)
{
    CALL_IMPL(OcilibElementGetCollection, elem);
}

OCI_Boolean OCI_API OCI_ElementSetBoolean
(
    OCI_Element * elem,
    OCI_Boolean   value
)
{
    CALL_IMPL(OcilibElementSetBoolean, elem, value);
}

OCI_Boolean OCI_API OCI_ElementSetNumber
(
    OCI_Element * elem,
    OCI_Number  * value
)
{
    CALL_IMPL(OcilibElementSetNumber, elem, value);
}

OCI_Boolean OCI_API OCI_ElementSetInt16
(
    OCI_Element* elem,
    OCI_Int16    value
)
{
    CALL_IMPL(OcilibElementSetInt16, elem, value);
}

OCI_Boolean OCI_API OCI_ElementSetUInt16
(
    OCI_Element * elem,
    OCI_UInt16    value
)
{
    CALL_IMPL(OcilibElementSetUInt16, elem, value);
}

OCI_Boolean OCI_API OCI_ElementSetInt32
(
    OCI_Element* elem,
    OCI_Int32    value
)
{
    CALL_IMPL(OcilibElementSetInt32, elem, value);
}

OCI_Boolean OCI_API OCI_ElementSetUInt32
(
    OCI_Element * elem,
    OCI_UInt32    value
)
{
    CALL_IMPL(OcilibElementSetUInt32, elem, value);
}

OCI_Boolean OCI_API OCI_ElementSetInt64
(
    OCI_Element* elem,
    OCI_Int64    value
)
{
    CALL_IMPL(OcilibElementSetInt64, elem, value);
}

OCI_Boolean OCI_API OCI_ElementSetUInt64
(
    OCI_Element * elem,
    OCI_UInt64    value
)
{
    CALL_IMPL(OcilibElementSetUInt64, elem, value);
}

OCI_Boolean OCI_API OCI_ElementSetDouble
(
    OCI_Element * elem,
    OCI_Double    value
)
{
    CALL_IMPL(OcilibElementSetDouble, elem, value);
}

OCI_Boolean OCI_API OCI_ElementSetFloat
(
    OCI_Element* elem,
    OCI_Float    value
)
{
    CALL_IMPL(OcilibElementSetFloat, elem, value);
}

OCI_Boolean OCI_API OCI_ElementSetString
(
    OCI_Element * elem,
    const otext * value
)
{
    CALL_IMPL(OcilibElementSetString, elem, value);
}

OCI_Boolean OCI_API OCI_ElementSetRaw
(
    OCI_Element * elem,
    void        * value,
    OCI_UInt32    len
)
{
    CALL_IMPL(OcilibElementSetRaw, elem, value, len);
}

OCI_Boolean OCI_API OCI_ElementSetDate
(
    OCI_Element* elem,
    OCI_Date   * value
)
{
    CALL_IMPL(OcilibElementSetDate, elem, value);
}

OCI_Boolean OCI_API OCI_ElementSetTimestamp
(
    OCI_Element  * elem,
    OCI_Timestamp* value
)
{
    CALL_IMPL(OcilibElementSetTimestamp, elem, value);
}

OCI_Boolean OCI_API OCI_ElementSetInterval
(
    OCI_Element * elem,
    OCI_Interval* value
)
{
    CALL_IMPL(OcilibElementSetInterval, elem, value);
}

OCI_Boolean OCI_API OCI_ElementSetCollection
(
    OCI_Element   * elem,
    OCI_Collection* value
)
{
    CALL_IMPL(OcilibElementSetCollection, elem, value);
}

OCI_Boolean OCI_API OCI_ElementSetObject
(
    OCI_Element * elem,
    OCI_Object  * value
)
{
    CALL_IMPL(OcilibElementSetObject, elem, value);
}

OCI_Boolean OCI_API OCI_ElementSetLob
(
    OCI_Element* elem,
    OCI_Lob    * value
)
{
    CALL_IMPL(OcilibElementSetLob, elem, value);
}

OCI_Boolean OCI_API OCI_ElementSetFile
(
    OCI_Element* elem,
    OCI_File   * value
)
{
    CALL_IMPL(OcilibElementSetFile, elem, value);
}

OCI_Boolean OCI_API OCI_ElementSetReference
(
    OCI_Element   * elem,
    OCI_Reference * value
)
{
    CALL_IMPL(OcilibElementSetReference, elem, value);
}

OCI_Boolean OCI_API OCI_ElementIsNull
(
    OCI_Element* elem
)
{
    CALL_IMPL(OcilibElementIsNull, elem);
}

OCI_Boolean OCI_API OCI_ElementSetNull
(
    OCI_Element* elem
)
{
    CALL_IMPL(OcilibElementSetNull, elem);
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
    CALL_IMPL(OcilibEnqueueCreate, typinf, name);
}

OCI_Boolean OCI_API OCI_EnqueueFree
(
    OCI_Enqueue* enqueue
)
{
    CALL_IMPL(OcilibEnqueueFree, enqueue);
}

OCI_Boolean OCI_API OCI_EnqueuePut
(
    OCI_Enqueue * enqueue,
    OCI_Message * msg
)
{
    CALL_IMPL(OcilibEnqueuePut, enqueue, msg);
}

OCI_Boolean OCI_API OCI_EnqueueSetSequenceDeviation
(
    OCI_Enqueue         * enqueue,
    OCI_QueueSeqDeviation sequence
)
{
    CALL_IMPL(OcilibEnqueueSetSequenceDeviation, enqueue, sequence);
}

OCI_QueueSeqDeviation OCI_API OCI_EnqueueGetSequenceDeviation
(
    OCI_Enqueue* enqueue
)
{
    CALL_IMPL(OcilibEnqueueGetSequenceDeviation, enqueue);
}

OCI_Boolean OCI_API OCI_EnqueueSetVisibility
(
    OCI_Enqueue       * enqueue,
    OCI_QueueVisibility visibility
)
{
    CALL_IMPL(OcilibEnqueueSetVisibility, enqueue, visibility);
}

OCI_QueueVisibility OCI_API OCI_EnqueueGetVisibility
(
    OCI_Enqueue* enqueue
)
{
    CALL_IMPL(OcilibEnqueueGetVisibility, enqueue);
}

OCI_Boolean OCI_API OCI_EnqueueSetRelativeMsgID
(
    OCI_Enqueue* enqueue,
    const void * id,
    OCI_UInt32   len
)
{
    CALL_IMPL(OcilibEnqueueSetRelativeMsgID, enqueue, id, len);
}

OCI_Boolean OCI_API OCI_EnqueueGetRelativeMsgID
(
    OCI_Enqueue * enqueue,
    void        * id,
    OCI_UInt32  * len
)
{
    CALL_IMPL(OcilibEnqueueGetRelativeMsgID, enqueue, id, len);
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

OCI_ErrorType OCI_API OCI_ErrorGetType
(
    OCI_Error* err
)
{
    return OcilibErrorGetType(err);
}

OCI_ErrorCategory OCI_API OCI_ErrorGetCategory
(
    OCI_Error* err
)
{
    return OcilibErrorGetCategory(err);
}

OCI_ErrorCode OCI_API OCI_ErrorGetCode
(
    OCI_Error* err
)
{
    return OcilibErrorGetCode(err);
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

OCI_Int32 OCI_API OCI_ErrorGetRow
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

OCI_EventType OCI_API OCI_EventGetType
(
    OCI_Event* event
)
{
    CALL_IMPL(OcilibEventGetType, event);
}

OCI_EventOperation OCI_API OCI_EventGetOperation
(
    OCI_Event* event
)
{
    CALL_IMPL(OcilibEventGetOperation, event);
}

const otext* OCI_API OCI_EventGetDatabase
(
    OCI_Event* event
)
{
    CALL_IMPL(OcilibEventGetDatabase, event);
}

const otext* OCI_API OCI_EventGetObject
(
    OCI_Event* event
)
{
    CALL_IMPL(OcilibEventGetObject, event);
}

const otext* OCI_API OCI_EventGetRowid
(
    OCI_Event* event
)
{
    CALL_IMPL(OcilibEventGetRowid, event);
}

OCI_Subscription* OCI_API OCI_EventGetSubscription
(
    OCI_Event* event
)
{
    CALL_IMPL(OcilibEventGetSubscription, event);
}

/* --------------------------------------------------------------------------------------------- *
 * file
 * --------------------------------------------------------------------------------------------- */

OCI_File* OCI_API OCI_FileCreate
(
    OCI_Connection* con,
    OCI_FileType    type
)
{
    CALL_IMPL(OcilibFileCreate, con, type);
}

OCI_Boolean OCI_API OCI_FileFree
(
    OCI_File* file
)
{
    CALL_IMPL(OcilibFileFree, file);
}

OCI_File** OCI_API OCI_FileArrayCreate
(
    OCI_Connection* con,
    OCI_FileType    type,
    OCI_UInt32      nbelem
)
{
    CALL_IMPL(OcilibFileCreateArray, con, type, nbelem);
}

OCI_Boolean OCI_API OCI_FileArrayFree
(
    OCI_File** files
)
{
    CALL_IMPL(OcilibFileFreeArray, files);
}

OCI_FileType OCI_API OCI_FileGetType
(
    OCI_File* file
)
{
    CALL_IMPL(OcilibFileGetType, file);
}

OCI_Boolean OCI_API OCI_FileSeek
(
    OCI_File   * file,
    OCI_UInt64   offset,
    OCI_SeekMode mode
)
{
    CALL_IMPL(OcilibFileSeek, file, offset, mode);
}

OCI_UInt64 OCI_API OCI_FileGetOffset
(
    OCI_File* file
)
{
    CALL_IMPL(OcilibFileGetOffset, file);
}

OCI_UInt32 OCI_API OCI_FileRead
(
    OCI_File * file,
    void     * buffer,
    OCI_UInt32 len
)
{
    CALL_IMPL(OcilibFileRead, file, buffer, len);
}

OCI_UInt64 OCI_API OCI_FileGetSize
(
    OCI_File* file
)
{
    CALL_IMPL(OcilibFileGetSize, file);
}

OCI_Boolean OCI_API OCI_FileExists
(
    OCI_File* file
)
{
    CALL_IMPL(OcilibFileExists, file);
}

OCI_Boolean OCI_API OCI_FileSetName
(
    OCI_File   * file,
    const otext* dir,
    const otext* name
)
{
    CALL_IMPL(OcilibFileSetName, file, dir, name);
}

const otext* OCI_API OCI_FileGetDirectory
(
    OCI_File* file
)
{
    CALL_IMPL(OcilibFileGetDirectory, file);
}

const otext* OCI_API OCI_FileGetName
(
    OCI_File* file
)
{
    CALL_IMPL(OcilibFileGetName, file);
}

OCI_Boolean OCI_API OCI_FileOpen
(
    OCI_File* file
)
{
    CALL_IMPL(OcilibFileOpen, file);
}

OCI_Boolean OCI_API OCI_FileIsOpen
(
    OCI_File* file
)
{
    CALL_IMPL(OcilibFileIsOpen, file);
}

OCI_Boolean OCI_API OCI_FileClose
(
    OCI_File* file
)
{
    CALL_IMPL(OcilibFileClose, file);
}

OCI_Boolean OCI_API OCI_FileIsEqual
(
    OCI_File* file,
    OCI_File* file2
)
{
    CALL_IMPL(OcilibFileIsEqual, file, file2);
}

OCI_Boolean OCI_API OCI_FileAssign
(
    OCI_File* file,
    OCI_File* file_src
)
{
    CALL_IMPL(OcilibFileAssign, file, file_src);
}

OCI_Connection* OCI_API OCI_FileGetConnection
(
    OCI_File* file
)
{
    CALL_IMPL(OcilibFileGetConnection, file);
}

/* --------------------------------------------------------------------------------------------- *
 * handle
 * --------------------------------------------------------------------------------------------- */

OCI_Environment* OCI_API OCI_HandleGetEnvironment
(
    void* handle
)
{
    return OcilibHandleGetEnvironment(handle);
}

/* --------------------------------------------------------------------------------------------- *
 * hash
 * --------------------------------------------------------------------------------------------- */

OCI_HashTable* OCI_API OCI_HashTableCreate
(
    OCI_Environment *env,
    OCI_UInt32       size,
    OCI_HashType     type
)
{
    CALL_IMPL_E(OcilibHashTableCreate, env, size, type);
}

OCI_Boolean OCI_API OCI_HashTableFree
(
    OCI_HashTable* table
)
{
    CALL_IMPL(OcilibHashTableFree, table);
}

OCI_UInt32 OCI_API OCI_HashTableGetSize
(
    OCI_HashTable* table
)
{
    CALL_IMPL(OcilibHashTableGetSize, table);
}

OCI_HashType OCI_API OCI_HashTableGetType
(
    OCI_HashTable* table
)
{
    CALL_IMPL(OcilibHashTableGetType, table);
}

OCI_Boolean OCI_API OCI_HashTableAddString
(
    OCI_HashTable* table,
    const otext  * key,
    const otext  * value
)
{
    CALL_IMPL(OcilibHashTableAddString, table, key, value);
}

const otext* OCI_API OCI_HashTableGetString
(
    OCI_HashTable* table,
    const otext  * key
)
{
    CALL_IMPL(OcilibHashTableGetString, table, key);
}

OCI_Boolean OCI_API OCI_HashTableAddInt32
(
    OCI_HashTable* table,
    const otext  * key,
    OCI_Int32      value
)
{
    CALL_IMPL(OcilibHashTableAddInt32, table, key, value);
}

OCI_Int32 OCI_API OCI_HashTableGetInt32
(
    OCI_HashTable* table,
    const otext  * key
)
{
    CALL_IMPL(OcilibHashTableGetInt32, table, key);
}

OCI_Boolean OCI_API OCI_HashTableAddPointer
(
    OCI_HashTable* table,
    const otext  * key,
    void         * value
)
{
    CALL_IMPL(OcilibHashTableAddPointer, table, key, value);
}

void* OCI_API OCI_HashTableGetPointer
(
    OCI_HashTable* table,
    const otext  * key
)
{
    CALL_IMPL(OcilibHashTableGetPointer, table, key);
}

OCI_HashEntry* OCI_API OCI_HashTableLookup
(
    OCI_HashTable* table,
    const otext  * key,
    OCI_Boolean    create
)
{
    CALL_IMPL(OcilibHashTableLookup, table, key, create);
}

OCI_HashValue* OCI_API OCI_HashTableGetValue
(
    OCI_HashTable* table,
    const otext  * key
)
{
    CALL_IMPL(OcilibHashTableGetValue, table, key);
}

OCI_HashEntry* OCI_API OCI_HashTableGetEntry
(
    OCI_HashTable* table,
    OCI_UInt32     index
)
{
    CALL_IMPL(OcilibHashTableGetEntry, table, index);
}

/* --------------------------------------------------------------------------------------------- *
 * interval
 * --------------------------------------------------------------------------------------------- */

OCI_Interval* OCI_API OCI_IntervalCreate
(
    OCI_Environment* env,
    OCI_IntervalType type
)
{
    CALL_IMPL(OcilibIntervalCreate, env, type);
}

OCI_Boolean OCI_API OCI_IntervalFree
(
    OCI_Interval* itv
)
{
    CALL_IMPL(OcilibIntervalFree, itv);
}

OCI_Interval** OCI_API OCI_IntervalArrayCreate
(
    OCI_Environment* env,
    OCI_IntervalType type,
    OCI_UInt32       nbelem
)
{
    CALL_IMPL_E(OcilibIntervalCreateArray, env, type, nbelem);
}

OCI_Boolean OCI_API OCI_IntervalArrayFree
(
    OCI_Interval** itvs
)
{
    CALL_IMPL(OcilibIntervalFreeArray, itvs);
}

OCI_IntervalType OCI_API OCI_IntervalGetType
(
    OCI_Interval* itv
)
{
    CALL_IMPL(OcilibIntervalGetType, itv);
}

OCI_Boolean OCI_API OCI_IntervalAssign
(
    OCI_Interval* itv,
    OCI_Interval* itv_src
)
{
    CALL_IMPL(OcilibIntervalAssign, itv, itv_src);
}

OCI_Int32 OCI_API OCI_IntervalCheck
(
    OCI_Interval* itv
)
{
    CALL_IMPL(OcilibIntervalCheck, itv);
}

OCI_Int32 OCI_API OCI_IntervalCompare
(
    OCI_Interval* itv,
    OCI_Interval* itv2
)
{
    CALL_IMPL(OcilibIntervalCompare, itv, itv2);
}

OCI_Boolean OCI_API OCI_IntervalFromText
(
    OCI_Interval* itv,
    const otext * str
)
{
    CALL_IMPL(OcilibIntervalFromString, itv, str);
}

OCI_Boolean OCI_API OCI_IntervalToText
(
    OCI_Interval* itv,
    OCI_Int32     leading_prec,
    OCI_Int32     fraction_prec,
    OCI_Int32     size,
    otext       * str
)
{
    CALL_IMPL(OcilibIntervalToString, itv, leading_prec, fraction_prec, size, str);
}

OCI_Boolean OCI_API OCI_IntervalFromTimeZone
(
    OCI_Interval* itv,
    const otext * str
)
{
    CALL_IMPL(OcilibIntervalFromTimeZone, itv, str);
}

OCI_Boolean OCI_API OCI_IntervalGetDaySecond
(
    OCI_Interval* itv,
    OCI_Int32   * day,
    OCI_Int32   * hour,
    OCI_Int32   * min,
    OCI_Int32   * sec,
    OCI_Int32   * nsec
)
{
    CALL_IMPL(OcilibIntervalGetDaySecond, itv, day, hour, min, sec, nsec);
}

OCI_Boolean OCI_API OCI_IntervalGetYearMonth
(
    OCI_Interval* itv,
    OCI_Int32   * year,
    OCI_Int32   * month
)
{
    CALL_IMPL(OcilibIntervalGetYearMonth, itv, year, month);
}

OCI_Boolean OCI_API OCI_IntervalSetDaySecond
(
    OCI_Interval* itv,
    OCI_Int32     day,
    OCI_Int32     hour,
    OCI_Int32     min,
    OCI_Int32     sec,
    OCI_Int32     nsec
)
{
    CALL_IMPL(OcilibIntervalSetDaySecond, itv, day, hour, min, sec, nsec);
}

OCI_Boolean OCI_API OCI_IntervalSetYearMonth
(
    OCI_Interval* itv,
    OCI_Int32     year,
    OCI_Int32     month
)
{
    CALL_IMPL(OcilibIntervalSetYearMonth, itv, year, month);
}

OCI_Boolean OCI_API OCI_IntervalAdd
(
    OCI_Interval* itv,
    OCI_Interval* itv2
)
{
    CALL_IMPL(OcilibIntervalAdd, itv, itv2);
}

OCI_Boolean OCI_API OCI_IntervalSubtract
(
    OCI_Interval* itv,
    OCI_Interval* itv2
)
{
    CALL_IMPL(OcilibIntervalSubtract, itv, itv2);
}

/* --------------------------------------------------------------------------------------------- *
 * iterator
 * --------------------------------------------------------------------------------------------- */

OCI_Iterator* OCI_API OCI_IteratorCreate
(
    OCI_Collection* coll
)
{
    CALL_IMPL(OcilibIteratorCreate, coll);
}

OCI_Boolean OCI_API OCI_IteratorFree
(
    OCI_Iterator* iter
)
{
    CALL_IMPL(OcilibIteratorFree, iter);
}

OCI_Element* OCI_API OCI_IteratorGetNext
(
    OCI_Iterator* iter
)
{
    CALL_IMPL(OcilibIteratorGetNext, iter);
}

OCI_Element* OCI_API OCI_IteratorGetPrev
(
    OCI_Iterator* iter
)
{
    CALL_IMPL(OcilibIteratorGetPrev, iter);
}

OCI_Element* OCI_API OCI_IteratorGetCurrent
(
    OCI_Iterator* iter
)
{
    CALL_IMPL(OcilibIteratorGetCurrent, iter);
}

/* --------------------------------------------------------------------------------------------- *
 * library
 * --------------------------------------------------------------------------------------------- */

OCI_Environment* OCI_API OCI_EnvironmentCreate
(
    POCI_ERROR   err_handler,
    const otext* lib_path,
    OCI_EnvMode  mode
)
{
    OcilibCheckContext();
    return OcilibEnvironmentCreate(err_handler, lib_path, mode, OCI_UNKNOWN);
}

OCI_Boolean OCI_API OCI_EnvironmentFree
(
    OCI_Environment *env
)
{
    OcilibCheckContext();
    return OcilibEnvironmentFree(env);
}

OCI_UInt32 OCI_API OCI_EnvironmentGetOCICompileVersion
(
    OCI_Environment *env
)
{
    CALL_IMPL_NO_ARGS_E(OcilibEnvironmentGetOCICompileVersion, env);
}

OCI_UInt32 OCI_API OCI_EnvironmentGetOCIRuntimeVersion
(
    OCI_Environment *env
)
{
    CALL_IMPL_NO_ARGS_E(OcilibEnvironmentGetOCIRuntimeVersion, env);
}

OCI_ImportMode OCI_API OCI_EnvironmentGetImportMode
(
    OCI_Environment *env
)
{
    CALL_IMPL_NO_ARGS_E(OcilibEnvironmentGetImportMode, env);
}

OCI_Charset OCI_API OCI_EnvironmentGetCharset
(
    OCI_Environment *env
)
{
    CALL_IMPL_NO_ARGS_E(OcilibEnvironmentGetCharset, env);
}

OCI_UInt64 OCI_API OCI_EnvironmentGetStats
(
    OCI_Environment *env,
    OCI_Stat         stat
)
{
    return OcilibEnvironmentGetStats(env, stat);
}

OCI_Error* OCI_API OCI_GetLastError
(
    void
)
{
    return OcilibErrorGetLastError();
}

OCI_Boolean OCI_API OCI_EnvironmentEnableWarnings
(
    OCI_Environment *env,
    OCI_Boolean      value
)
{
    CALL_IMPL_E(OcilibEnvironmentEnableWarnings, env, value);
}

OCI_Boolean OCI_API OCI_EnvironmentSetErrorHandler
(
    OCI_Environment *env,
    POCI_ERROR       handler
)
{
    CALL_IMPL_E(OcilibEnvironmentSetErrorHandler, env, handler);
}

OCI_Boolean OCI_API OCI_DatabaseStartup
(
    OCI_Environment      *env,
    const otext         * db,
    const otext         * user,
    const otext         * pwd,
    OCI_SessionMode       sess_mode,
    OCI_DatabaseStartMode start_mode,
    OCI_DatabaseStartFlag start_flag,
    const otext         * spfile
)
{
    CALL_IMPL_E(OcilibDatabaseStartup, env, db, user, pwd, sess_mode, start_mode, start_flag, spfile);
}

OCI_Boolean OCI_API OCI_DatabaseShutdown
(
    OCI_Environment         *env,
    const otext            * db,
    const otext            * user,
    const otext            * pwd,
    OCI_SessionMode          sess_mode,
    OCI_DatabaseShutdownMode shut_mode,
    OCI_DatabaseShutdownFlag shut_flag
)
{
    CALL_IMPL_E(OcilibDatabaseShutdown, env, db, user, pwd, sess_mode, shut_mode, shut_flag);
}

OCI_Boolean OCI_API OCI_EnvironmentSetHAHandler
(
    OCI_Environment *env,
    POCI_HA_HANDLER  handler
)
{
    CALL_IMPL_E(OcilibEnvironmentSetHAHandler, env, handler);
}

OCI_Boolean OCI_API OCI_EnvironmentSetSubscriptionPort
(
    OCI_Environment *env,
    OCI_UInt32       port
)
{
    CALL_IMPL_E(OcilibEnvironmentSetSubscriptionPort, env, port);
}

OCI_UInt32 OCI_API OCI_EnvironmentGetSubscriptionPort
(
    OCI_Environment *env
)
{
    CALL_IMPL_NO_ARGS_E(OcilibEnvironmentGetSubscriptionPort, env);
}

const otext* OCI_API OCI_EnvironmentGetLocaleString
(
    OCI_Environment *env,
    OCI_LocaleCode   code
)
{
    CALL_IMPL_E(OcilibEnvironmentGetLocaleString, env, code);
}

OCI_Boolean OCI_API OCI_EnvironmentSetFormat
(
    OCI_Environment *env,
    OCI_FormatType   type,
    const otext    * format
)
{
    CALL_IMPL_E(OcilibEnvironmentSetFormat, env, type, format);
}

const otext* OCI_API OCI_EnvironmentGetFormat
(
    OCI_Environment *env,
    OCI_FormatType   type
)
{
    CALL_IMPL_E(OcilibEnvironmentGetFormat, env, type);
}

OCI_Boolean OCI_API OCI_EnvironmentSetUserPassword
(
    OCI_Environment *env,
    const otext    * db,
    const otext    * user,
    const otext    * pwd,
    const otext    * new_pwd
)
{
    CALL_IMPL_E(OcilibEnvironmentSetUserPassword, env, db, user, pwd, new_pwd);
}

/* --------------------------------------------------------------------------------------------- *
 * lob
 * --------------------------------------------------------------------------------------------- */

OCI_Lob* OCI_API OCI_LobCreate
(
    OCI_Connection* con,
    OCI_LobType     type
)
{
    CALL_IMPL(OcilibLobCreate, con, type);
}

OCI_Boolean OCI_API OCI_LobFree
(
    OCI_Lob* lob
)
{
    CALL_IMPL(OcilibLobFree, lob);
}

OCI_Lob** OCI_API OCI_LobArrayCreate
(
    OCI_Connection* con,
    OCI_LobType     type,
    OCI_UInt32      nbelem
)
{
    CALL_IMPL(OcilibLobCreateArray, con, type, nbelem);
}

OCI_Boolean OCI_API OCI_LobArrayFree
(
    OCI_Lob** lobs
)
{
    CALL_IMPL(OcilibLobFreeArray, lobs);
}

OCI_LobType OCI_API OCI_LobGetType
(
    OCI_Lob* lob
)
{
    CALL_IMPL(OcilibLobGetType, lob);
}

OCI_Boolean OCI_API OCI_LobSeek
(
    OCI_Lob    * lob,
    OCI_UInt64   offset,
    OCI_SeekMode mode
)
{
    CALL_IMPL(OcilibLobSeek, lob, offset, mode);
}

OCI_UInt64 OCI_API OCI_LobGetOffset
(
    OCI_Lob* lob
)
{
    CALL_IMPL(OcilibLobGetOffset, lob);
}

OCI_Boolean OCI_API OCI_LobRead
(
    OCI_Lob   * lob,
    void      * buffer,
    OCI_UInt32* char_count,
    OCI_UInt32* byte_count
)
{
    CALL_IMPL(OcilibLobRead, lob, buffer, char_count, byte_count);
}

OCI_Boolean OCI_API OCI_LobWrite
(
    OCI_Lob   * lob,
    void      * buffer,
    OCI_UInt32* char_count,
    OCI_UInt32* byte_count
)
{
    CALL_IMPL(OcilibLobWrite, lob, buffer, char_count, byte_count);
}

OCI_Boolean OCI_API OCI_LobTruncate
(
    OCI_Lob  * lob,
    OCI_UInt64 size
)
{
    CALL_IMPL(OcilibLobTruncate, lob, size);
}

OCI_UInt64 OCI_API OCI_LobGetLength
(
    OCI_Lob* lob
)
{
    CALL_IMPL(OcilibLobGetLength, lob);
}

OCI_UInt32 OCI_API OCI_LobGetChunkSize
(
    OCI_Lob* lob
)
{
    CALL_IMPL(OcilibLobGetChunkSize, lob);
}

OCI_UInt64 OCI_API OCI_LobErase
(
    OCI_Lob  * lob,
    OCI_UInt64 offset,
    OCI_UInt64 len
)
{
    CALL_IMPL(OcilibLobErase, lob, offset, len);
}

OCI_Boolean OCI_API OCI_LobAppend
(
    OCI_Lob   * lob,
    void      * buffer,
    OCI_UInt32* char_count,
    OCI_UInt32* byte_count
)
{
    CALL_IMPL(OcilibLobAppend, lob, buffer, char_count, byte_count);
}

OCI_Boolean OCI_API OCI_LobAppendLob
(
    OCI_Lob* lob,
    OCI_Lob* lob_src
)
{
    CALL_IMPL(OcilibLobAppendLob, lob, lob_src);
}

OCI_Boolean OCI_API OCI_LobIsTemporary
(
    OCI_Lob* lob
)
{
    CALL_IMPL(OcilibLobIsTemporary, lob);
}

OCI_Boolean OCI_API OCI_LobCopy
(
    OCI_Lob  * lob,
    OCI_Lob  * lob_src,
    OCI_UInt64 offset_dst,
    OCI_UInt64 offset_src,
    OCI_UInt64 count
)
{
    CALL_IMPL(OcilibLobCopy, lob, lob_src, offset_dst, offset_src, count);
}

OCI_Boolean OCI_API OCI_LobCopyFromFile
(
    OCI_Lob  * lob,
    OCI_File * file,
    OCI_UInt64 offset_dst,
    OCI_UInt64 offset_src,
    OCI_UInt64 count
)
{
    CALL_IMPL(OcilibLobCopyFromFile, lob, file, offset_dst, offset_src, count);
}

OCI_Boolean OCI_API OCI_LobOpen
(
    OCI_Lob   * lob,
    OCI_LobMode mode
)
{
    CALL_IMPL(OcilibLobOpen, lob, mode);
}

OCI_Boolean OCI_API OCI_LobClose
(
    OCI_Lob* lob
)
{
    CALL_IMPL(OcilibLobClose, lob);
}

OCI_Boolean OCI_API OCI_LobIsEqual
(
    OCI_Lob* lob,
    OCI_Lob* lob2
)
{
    CALL_IMPL(OcilibLobIsEqual, lob, lob2);
}

OCI_Boolean OCI_API OCI_LobAssign
(
    OCI_Lob* lob,
    OCI_Lob* lob_src
)
{
    CALL_IMPL(OcilibLobAssign, lob, lob_src);
}

OCI_UInt64 OCI_API OCI_LobGetMaxSize
(
    OCI_Lob* lob
)
{
    CALL_IMPL(OcilibLobGetMaxSize, lob);
}

OCI_Boolean OCI_API OCI_LobFlush
(
    OCI_Lob* lob
)
{
    CALL_IMPL(OcilibLobFlush, lob);
}

OCI_Boolean OCI_API OCI_LobEnableBuffering
(
    OCI_Lob   * lob,
    OCI_Boolean value
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

OCI_Boolean OCI_API OCI_LobIsRemote
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
    OCI_LongType   type
)
{
    CALL_IMPL(OcilibLongCreate, stmt, type);
}

OCI_Boolean OCI_API OCI_LongFree
(
    OCI_Long* lg
)
{
    CALL_IMPL(OcilibLongFree, lg);
}

OCI_LongType OCI_API OCI_LongGetType
(
    OCI_Long* lg
)
{
    CALL_IMPL(OcilibLongGetType, lg);
}

OCI_UInt32 OCI_API OCI_LongRead
(
    OCI_Long * lg,
    void     * buffer,
    OCI_UInt32 len
)
{
    CALL_IMPL(OcilibLongRead, lg, buffer, len);
}

OCI_UInt32 OCI_API OCI_LongWrite
(
    OCI_Long * lg,
    void     * buffer,
    OCI_UInt32 len
)
{
    CALL_IMPL(OcilibLongWrite, lg, buffer, len);
}

OCI_UInt32 OCI_API OCI_LongGetSize
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

OCI_Message* OCI_API OCI_MessageCreate
(
    OCI_TypeInfo* typinf
)
{
    CALL_IMPL(OcilibMessageCreate, typinf);
}

OCI_Boolean OCI_API OCI_MessageFree
(
    OCI_Message* msg
)
{
    CALL_IMPL(OcilibMessageFree, msg);
}

OCI_Boolean OCI_API OCI_MessageReset
(
    OCI_Message* msg
)
{
    CALL_IMPL(OcilibMessageReset, msg);
}

OCI_Object* OCI_API OCI_MessageGetObject
(
    OCI_Message* msg
)
{
    CALL_IMPL(OcilibMessageGetObject, msg);
}

OCI_Boolean OCI_API OCI_MessageSetObject
(
    OCI_Message * msg,
    OCI_Object  * obj
)
{
    CALL_IMPL(OcilibMessageSetObject, msg, obj);
}

OCI_Boolean OCI_API OCI_MessageGetRaw
(
    OCI_Message * msg,
    void        * raw,
    OCI_UInt32  * size
)
{
    CALL_IMPL(OcilibMessageGetRaw, msg, raw, size);
}

OCI_Boolean OCI_API OCI_MessageSetRaw
(
    OCI_Message * msg,
    const void  * raw,
    OCI_UInt32    size
)
{
    CALL_IMPL(OcilibMessageSetRaw, msg, raw, size);
}

OCI_Int32 OCI_API OCI_MessageGetAttemptCount
(
    OCI_Message* msg
)
{
    CALL_IMPL(OcilibMessageGetAttemptCount, msg);
}

OCI_Int32 OCI_API OCI_MessageGetEnqueueDelay
(
    OCI_Message* msg
)
{
    CALL_IMPL(OcilibMessageGetEnqueueDelay, msg);
}

OCI_Boolean OCI_API OCI_MessageSetEnqueueDelay
(
    OCI_Message * msg,
    OCI_Int32     value
)
{
    CALL_IMPL(OcilibMessageSetEnqueueDelay, msg, value);
}

OCI_Date* OCI_API OCI_MessageGetEnqueueTime
(
    OCI_Message* msg
)
{
    CALL_IMPL(OcilibMessageGetEnqueueTime, msg);
}

OCI_Int32 OCI_API OCI_MessageGetExpiration
(
    OCI_Message* msg
)
{
    CALL_IMPL(OcilibMessageGetExpiration, msg);
}

OCI_Boolean OCI_API OCI_MessageSetExpiration
(
    OCI_Message * msg,
    OCI_Int32     value
)
{
    CALL_IMPL(OcilibMessageSetExpiration, msg, value);
}

OCI_MsgState OCI_API OCI_MessageGetState
(
    OCI_Message* msg
)
{
    CALL_IMPL(OcilibMessageGetState, msg);
}

OCI_Int32 OCI_API OCI_MessageGetPriority
(
    OCI_Message* msg
)
{
    CALL_IMPL(OcilibMessageGetPriority, msg);
}

OCI_Boolean OCI_API OCI_MessageSetPriority
(
    OCI_Message * msg,
    OCI_Int32     value
)
{
    CALL_IMPL(OcilibMessageSetPriority, msg, value);
}

OCI_Boolean OCI_API OCI_MessageGetID
(
    OCI_Message * msg,
    void        * id,
    OCI_UInt32  * len
)
{
    CALL_IMPL(OcilibMessageGetID, msg, id, len);
}

OCI_Boolean OCI_API OCI_MessageGetOriginalID
(
    OCI_Message * msg,
    void        * id,
    OCI_UInt32  * len
)
{
    CALL_IMPL(OcilibMessageGetOriginalID, msg, id, len);
}

OCI_Boolean OCI_API OCI_MessageSetOriginalID
(
    OCI_Message * msg,
    const void  * id,
    OCI_UInt32    len
)
{
    CALL_IMPL(OcilibMessageSetOriginalID, msg, id, len);
}

OCI_Agent* OCI_API OCI_MessageGetSender
(
    OCI_Message* msg
)
{
    CALL_IMPL(OcilibMessageGetSender, msg);
}

OCI_Boolean OCI_API OCI_MessageSetSender
(
    OCI_Message * msg,
    OCI_Agent   * sender
)
{
    CALL_IMPL(OcilibMessageSetSender, msg, sender);
}

OCI_Boolean OCI_API OCI_MessageSetConsumers
(
    OCI_Message * msg,
    OCI_Agent  ** consumers,
    OCI_UInt32    count
)
{
    CALL_IMPL(OcilibMessageSetConsumers, msg, consumers, count);
}

const otext* OCI_API OCI_MessageGetCorrelation
(
    OCI_Message* msg
)
{
    CALL_IMPL(OcilibMessageGetCorrelation, msg);
}

OCI_Boolean OCI_API OCI_MessageSetCorrelation
(
    OCI_Message * msg,
    const otext * correlation
)
{
    CALL_IMPL(OcilibMessageSetCorrelation, msg, correlation);
}

const otext* OCI_API OCI_MessageGetExceptionQueue
(
    OCI_Message* msg
)
{
    CALL_IMPL(OcilibMessageGetExceptionQueue, msg);
}

OCI_Boolean OCI_API OCI_MessageSetExceptionQueue
(
    OCI_Message * msg,
    const otext * queue
)
{
    CALL_IMPL(OcilibMessageSetExceptionQueue, msg, queue);
}

/* --------------------------------------------------------------------------------------------- *
 *  number
 * --------------------------------------------------------------------------------------------- */

OCI_Number* OCI_API OCI_NumberCreate
(
    OCI_Environment* env
)
{
    CALL_IMPL_NO_ARGS_E(OcilibNumberCreate, env);
}

OCI_Boolean OCI_API OCI_NumberFree
(
    OCI_Number* number
)
{
    CALL_IMPL(OcilibNumberFree, number);
}

OCI_Number** OCI_API OCI_NumberArrayCreate
(
    OCI_Environment* env,
    OCI_UInt32       nbelem
)
{
    CALL_IMPL_E(OcilibNumberCreateArray, env, nbelem);
}

OCI_Boolean OCI_API OCI_NumberArrayFree
(
    OCI_Number** numbmers
)
{
    CALL_IMPL(OcilibNumberFreeArray, numbmers);
}

OCI_Boolean OCI_API OCI_NumberAssign
(
    OCI_Number* number,
    OCI_Number* number_src
)
{
    CALL_IMPL(OcilibNumberAssign, number, number_src);
}

OCI_Boolean OCI_API OCI_NumberToText
(
    OCI_Number * number,
    const otext* fmt,
    OCI_Int32    size,
    otext      * str
)
{
    CALL_IMPL(OcilibNumberToString, number, fmt, size, str);
}

OCI_Boolean OCI_API OCI_NumberFromText
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

OCI_Boolean OCI_API OCI_NumberSetContent
(
    OCI_Number   * number,
    unsigned char* content
)
{
    CALL_IMPL(OcilibNumberSetContent, number, content);
}

OCI_Boolean OCI_API OCI_NumberSetValue
(
    OCI_Number    * number,
    OCI_NumericType type,
    void          * value
)
{
    CALL_IMPL(OcilibNumberSetValue, number, type, value);
}

OCI_Boolean OCI_API OCI_NumberGetValue
(
    OCI_Number    * number,
    OCI_NumericType type,
    void          * value
)
{
    CALL_IMPL(OcilibNumberGetValue, number, type, value);
}

OCI_Boolean OCI_API OCI_NumberAdd
(
    OCI_Number    * number,
    OCI_NumericType type,
    void          * value
)
{
    CALL_IMPL(OcilibNumberAdd, number, type, value);
}

OCI_Boolean OCI_API OCI_NumberSub
(
    OCI_Number    * number,
    OCI_NumericType type,
    void          * value
)
{
    CALL_IMPL(OcilibNumberSub, number, type, value);
}

OCI_Boolean OCI_API OCI_NumberMultiply
(
    OCI_Number    * number,
    OCI_NumericType type,
    void          * value
)
{
    CALL_IMPL(OcilibNumberMultiply, number, type, value);
}

OCI_Boolean OCI_API OCI_NumberDivide
(
    OCI_Number    * number,
    OCI_NumericType type,
    void          * value
)
{
    CALL_IMPL(OcilibNumberDivide, number, type, value);
}

OCI_Int32 OCI_API OCI_NumberCompare
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
    OCI_TypeInfo* typinf
)
{
    CALL_IMPL(OcilibObjectCreate, typinf);
}

OCI_Boolean OCI_API OCI_ObjectFree
(
    OCI_Object* obj
)
{
    CALL_IMPL(OcilibObjectFree, obj);
}

OCI_Object** OCI_API OCI_ObjectArrayCreate
(
    OCI_TypeInfo* typinf,
    OCI_UInt32    nbelem
)
{
    CALL_IMPL(OcilibObjectCreateArray, typinf, nbelem);
}

OCI_Boolean OCI_API OCI_ObjectArrayFree
(
    OCI_Object** objs
)
{
    CALL_IMPL(OcilibObjectFreeArray, objs);
}

OCI_Boolean OCI_API OCI_ObjectAssign
(
    OCI_Object* obj,
    OCI_Object* obj_src
)
{
    CALL_IMPL(OcilibObjectAssign, obj, obj_src);
}

OCI_ObjectType OCI_API OCI_ObjectGetType
(
    OCI_Object* obj
)
{
    CALL_IMPL(OcilibObjectGetType, obj);
}

OCI_Boolean OCI_API OCI_ObjectGetSelfReference
(
    OCI_Object    * obj,
    OCI_Reference * ref
)
{
    CALL_IMPL(OcilibObjectGetSelfReference, obj, ref);
}

OCI_TypeInfo* OCI_API OCI_ObjectGetTypeInfo
(
    OCI_Object* obj
)
{
    CALL_IMPL(OcilibObjectGetTypeInfo, obj);
}

OCI_Boolean OCI_API OCI_ObjectGetBoolean
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

OCI_Int16 OCI_API OCI_ObjectGetInt16
(
    OCI_Object * obj,
    const otext* attr
)
{
    CALL_IMPL(OcilibObjectGetInt16, obj, attr);
}

OCI_UInt16 OCI_API OCI_ObjectGetUInt16
(
    OCI_Object * obj,
    const otext* attr
)
{
    CALL_IMPL(OcilibObjectGetUInt16, obj, attr);
}

OCI_Int32 OCI_API OCI_ObjectGetInt32
(
    OCI_Object * obj,
    const otext* attr
)
{
    CALL_IMPL(OcilibObjectGetInt32, obj, attr);
}

OCI_UInt32 OCI_API OCI_ObjectGetUInt32
(
    OCI_Object * obj,
    const otext* attr
)
{
    CALL_IMPL(OcilibObjectGetUInt32, obj, attr);
}

OCI_Int64 OCI_API OCI_ObjectGetInt64
(
    OCI_Object * obj,
    const otext* attr
)
{
    CALL_IMPL(OcilibObjectGetInt64, obj, attr);
}

OCI_UInt64 OCI_API OCI_ObjectGetUInt64
(
    OCI_Object * obj,
    const otext* attr
)
{
    CALL_IMPL(OcilibObjectGetUInt64, obj, attr);
}

OCI_Double OCI_API OCI_ObjectGetDouble
(
    OCI_Object * obj,
    const otext* attr
)
{
    CALL_IMPL(OcilibObjectGetDouble, obj, attr);
}

OCI_Float OCI_API OCI_ObjectGetFloat
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

OCI_Int32 OCI_API OCI_ObjectGetRaw
(
    OCI_Object * obj,
    const otext* attr,
    void       * value,
    OCI_UInt32   len
)
{
    CALL_IMPL(OcilibObjectGetRaw, obj, attr, value, len);
}

OCI_UInt32 OCI_API OCI_ObjectGetRawSize
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

OCI_Collection* OCI_API OCI_ObjectGetColl
(
    OCI_Object * obj,
    const otext* attr)
{
    CALL_IMPL(OcilibObjectGetColl, obj, attr);
}

OCI_Reference* OCI_API OCI_ObjectGetReference
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

OCI_Boolean OCI_API OCI_ObjectSetBoolean
(
    OCI_Object * obj,
    const otext* attr,
    OCI_Boolean  value
)
{
    CALL_IMPL(OcilibObjectSetBoolean, obj, attr, value);
}

OCI_Boolean OCI_API OCI_ObjectSetNumber
(
    OCI_Object * obj,
    const otext* attr,
    OCI_Number * value
)
{
    CALL_IMPL(OcilibObjectSetNumber, obj, attr, value);
}

OCI_Boolean OCI_API OCI_ObjectSetInt16
(
    OCI_Object * obj,
    const otext* attr,
    OCI_Int16    value
)
{
    CALL_IMPL(OcilibObjectSetInt16, obj, attr, value);
}

OCI_Boolean OCI_API OCI_ObjectSetUInt16
(
    OCI_Object  * obj,
    const otext * attr,
    OCI_UInt16    value
)
{
    CALL_IMPL(OcilibObjectSetUInt16, obj, attr, value);
}

OCI_Boolean OCI_API OCI_ObjectSetInt32
(
    OCI_Object * obj,
    const otext* attr,
    OCI_Int32    value
)
{
    CALL_IMPL(OcilibObjectSetInt32, obj, attr, value);
}

OCI_Boolean OCI_API OCI_ObjectSetUInt32
(
    OCI_Object * obj,
    const otext* attr,
    OCI_UInt32   value
)
{
    CALL_IMPL(OcilibObjectSetUInt32, obj, attr, value);
}

OCI_Boolean OCI_API OCI_ObjectSetInt64
(
    OCI_Object * obj,
    const otext* attr,
    OCI_Int64    value
)
{
    CALL_IMPL(OcilibObjectSetInt64, obj, attr, value);
}

OCI_Boolean OCI_API OCI_ObjectSetUInt64
(
    OCI_Object * obj,
    const otext* attr,
    OCI_UInt64   value
)
{
    CALL_IMPL(OcilibObjectSetUInt64, obj, attr, value);
}

OCI_Boolean OCI_API OCI_ObjectSetDouble
(
    OCI_Object * obj,
    const otext* attr,
    OCI_Double   value
)
{
    CALL_IMPL(OcilibObjectSetDouble, obj, attr, value);
}

OCI_Boolean OCI_API OCI_ObjectSetFloat
(
    OCI_Object * obj,
    const otext* attr,
    OCI_Float    value
)
{
    CALL_IMPL(OcilibObjectSetFloat, obj, attr, value);
}

OCI_Boolean OCI_API OCI_ObjectSetString
(
    OCI_Object * obj,
    const otext* attr,
    const otext* value
)
{
    CALL_IMPL(OcilibObjectSetString, obj, attr, value);
}

OCI_Boolean OCI_API OCI_ObjectSetRaw
(
    OCI_Object * obj,
    const otext* attr,
    void       * value,
    OCI_UInt32   len
)
{
    CALL_IMPL(OcilibObjectSetRaw, obj, attr, value, len);
}

OCI_Boolean OCI_API OCI_ObjectSetDate
(
    OCI_Object * obj,
    const otext* attr,
    OCI_Date   * value
)
{
    CALL_IMPL(OcilibObjectSetDate, obj, attr, value);
}

OCI_Boolean OCI_API OCI_ObjectSetTimestamp
(
    OCI_Object   * obj,
    const otext  * attr,
    OCI_Timestamp* value
)
{
    CALL_IMPL(OcilibObjectSetTimestamp, obj, attr, value);
}

OCI_Boolean OCI_API OCI_ObjectSetInterval
(
    OCI_Object  * obj,
    const otext * attr,
    OCI_Interval* value
)
{
    CALL_IMPL(OcilibObjectSetInterval, obj, attr, value);
}

OCI_Boolean OCI_API OCI_ObjectSetColl
(
    OCI_Object     * obj,
    const otext    * attr,
    OCI_Collection * value
)
{
    CALL_IMPL(OcilibObjectSetColl, obj, attr, value);
}

OCI_Boolean OCI_API OCI_ObjectSetObject
(
    OCI_Object * obj,
    const otext* attr,
    OCI_Object * value
)
{
    CALL_IMPL(OcilibObjectSetObject, obj, attr, value);
}

OCI_Boolean OCI_API OCI_ObjectSetLob
(
    OCI_Object * obj,
    const otext* attr,
    OCI_Lob    * value
)
{
    CALL_IMPL(OcilibObjectSetLob, obj, attr, value);
}

OCI_Boolean OCI_API OCI_ObjectSetFile
(
    OCI_Object * obj,
    const otext* attr,
    OCI_File   * value
)
{
    CALL_IMPL(OcilibObjectSetFile, obj, attr, value);
}

OCI_Boolean OCI_API OCI_ObjectSetReference
(
    OCI_Object    * obj,
    const otext   * attr,
    OCI_Reference * value
)
{
    CALL_IMPL(OcilibObjectSetReference, obj, attr, value);
}

OCI_Boolean OCI_API OCI_ObjectIsNull
(
    OCI_Object * obj,
    const otext* attr
)
{
    CALL_IMPL(OcilibObjectIsNull, obj, attr);
}

OCI_Boolean OCI_API OCI_ObjectSetNull
(
    OCI_Object * obj,
    const otext* attr
)
{
    CALL_IMPL(OcilibObjectSetNull, obj, attr);
}

OCI_Boolean OCI_API OCI_ObjectGetStruct
(
    OCI_Object* obj,
    void     ** pp_struct,
    void     ** pp_ind
)
{
    CALL_IMPL(OcilibObjectGetStruct, obj, pp_struct, pp_ind);
}

OCI_Boolean OCI_API OCI_ObjectToText
(
    OCI_Object * obj,
    OCI_UInt32 * size,
    otext      * str
)
{
    CALL_IMPL(OcilibObjectToString, obj, size, str);
}

/* --------------------------------------------------------------------------------------------- *
 *  pool
 * --------------------------------------------------------------------------------------------- */

OCI_Pool* OCI_API OCI_PoolCreate
(
    OCI_Environment *env,
    const otext    * db,
    const otext    * user,
    const otext    * pwd,
    OCI_PoolType     type,
    OCI_SessionMode  mode,
    OCI_UInt32       min_con,
    OCI_UInt32       max_con,
    OCI_UInt32       incr_con
)
{
    CALL_IMPL_E(OcilibPoolCreate, env, db, user, pwd, type, mode, min_con, max_con, incr_con);
}

OCI_Boolean OCI_API OCI_PoolFree
(
    OCI_Environment *env,
    OCI_Pool       * pool
)
{
    CALL_IMPL_E(OcilibPoolFree, env, pool);
}

OCI_Connection* OCI_API OCI_PoolGetConnection
(
    OCI_Environment *env,
    OCI_Pool       * pool,
    const otext    * tag
)
{
    CALL_IMPL_E(OcilibPoolGetConnection, env, pool, tag, NULL, NULL);
}

OCI_Connection* OCI_API OCI_PoolGetConnectionSharded
(
    OCI_Environment *env,
    OCI_Pool       * pool,
    const otext    * tag,
    OCI_ShardingKey* skey,
    OCI_ShardingKey* sskey
)
{
    CALL_IMPL_E(OcilibPoolGetConnection, env, pool, tag, skey, sskey);
}

OCI_UInt32 OCI_API OCI_PoolGetTimeout
(
    OCI_Pool* pool
)
{
    CALL_IMPL(OcilibPoolGetTimeout, pool);
}

OCI_Boolean OCI_API OCI_PoolSetTimeout
(
    OCI_Pool * pool,
    OCI_UInt32 value
)
{
    CALL_IMPL(OcilibPoolSetTimeout, pool, value);
}

OCI_Boolean OCI_API OCI_PoolGetNoWait
(
    OCI_Pool* pool
)
{
    CALL_IMPL(OcilibPoolGetNoWait, pool);
}

OCI_Boolean OCI_API OCI_PoolSetNoWait
(
    OCI_Pool  * pool,
    OCI_Boolean value
)
{
    CALL_IMPL(OcilibPoolSetNoWait, pool, value);
}

OCI_UInt32 OCI_API OCI_PoolGetBusyCount
(
    OCI_Pool* pool
)
{
    CALL_IMPL(OcilibPoolGetBusyCount, pool);
}

OCI_UInt32 OCI_API OCI_PoolGetOpenedCount
(
    OCI_Pool* pool
)
{
    CALL_IMPL(OcilibPoolGetOpenedCount, pool);
}

OCI_UInt32 OCI_API OCI_PoolGetMin
(
    OCI_Pool* pool
)
{
    CALL_IMPL(OcilibPoolGetMin, pool);
}

OCI_UInt32 OCI_API OCI_PoolGetMax
(
    OCI_Pool* pool
)
{
    CALL_IMPL(OcilibPoolGetMax, pool);
}

OCI_UInt32 OCI_API OCI_PoolGetIncrement
(
    OCI_Pool* pool
)
{
    CALL_IMPL(OcilibPoolGetIncrement, pool);
}

OCI_UInt32 OCI_API OCI_PoolGetStatementCacheSize
(
    OCI_Pool* pool
)
{
    CALL_IMPL(OcilibPoolGetStatementCacheSize, pool);
}

OCI_Boolean OCI_API OCI_PoolSetStatementCacheSize
(
    OCI_Pool * pool,
    OCI_UInt32 value
)
{
    CALL_IMPL(OcilibPoolSetStatementCacheSize, pool, value);
}

/* --------------------------------------------------------------------------------------------- *
 *  queue
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OCI_API OCI_QueueCreate
(
    OCI_Connection* con,
    const otext   * queue_name,
    const otext   * queue_table,
    OCI_QueueType   queue_type,
    OCI_UInt32      max_retries,
    OCI_UInt32      retry_delay,
    OCI_UInt32      retention_time,
    OCI_Boolean     dependency_tracking,
    const otext   * comment
)
{
    CALL_IMPL(OcilibQueueCreate, con, queue_name, queue_table, queue_type, max_retries,
              retry_delay, retention_time, dependency_tracking, comment);
}

OCI_Boolean OCI_API OCI_QueueAlter
(
    OCI_Connection* con,
    const otext   * queue_name,
    OCI_UInt32      max_retries,
    OCI_UInt32      retry_delay,
    OCI_UInt32      retention_time,
    const otext   * comment
)
{
    CALL_IMPL(OcilibQueueAlter, con, queue_name, max_retries, retry_delay, retention_time, comment);
}

OCI_Boolean OCI_API OCI_QueueDrop
(
    OCI_Connection* con,
    const otext   * queue_name
)
{
    CALL_IMPL(OcilibQueueDrop, con, queue_name);
}

OCI_Boolean OCI_API OCI_QueueStart
(
    OCI_Connection* con,
    const otext   * queue_name,
    OCI_Boolean     enqueue,
    OCI_Boolean     dequeue
)
{
    CALL_IMPL(OcilibQueueStart, con, queue_name, enqueue, dequeue);
}

OCI_Boolean OCI_API OCI_QueueStop
(
    OCI_Connection* con,
    const otext   * queue_name,
    OCI_Boolean     enqueue,
    OCI_Boolean     dequeue,
    OCI_Boolean     wait
)
{
    CALL_IMPL(OcilibQueueStop, con, queue_name, enqueue, dequeue, wait);
}

OCI_Boolean OCI_API OCI_QueueTableCreate
(
    OCI_Connection  * con,
    const otext     * queue_table,
    const otext     * queue_payload_type,
    const otext     * storage_clause,
    const otext     * sort_list,
    OCI_Boolean       multiple_consumers,
    OCI_QueueGrouping message_grouping,
    const otext     * comment,
    OCI_UInt32        primary_instance,
    OCI_UInt32        secondary_instance,
    const otext     * compatible
)
{
    CALL_IMPL(OcilibQueueTableCreate, con, queue_table, queue_payload_type, storage_clause,
              sort_list, multiple_consumers, message_grouping, comment, primary_instance,
              secondary_instance, compatible);
}

OCI_Boolean OCI_API OCI_QueueTableAlter
(
    OCI_Connection* con,
    const otext   * queue_table,
    const otext   * comment,
    OCI_UInt32      primary_instance,
    OCI_UInt32      secondary_instance
)
{
    CALL_IMPL(OcilibQueueTableAlter, con, queue_table, comment, primary_instance, secondary_instance);
}

OCI_Boolean OCI_API OCI_QueueTableDrop
(
    OCI_Connection* con,
    const otext   * queue_table,
    OCI_Boolean     force
)
{
    CALL_IMPL(OcilibQueueTableDrop, con, queue_table, force);
}

OCI_Boolean OCI_API OCI_QueueTablePurge
(
    OCI_Connection   * con,
    const otext      * queue_table,
    const otext      * purge_condition,
    OCI_Boolean        block,
    OCI_QueuePurgeMode delivery_mode
)
{
    CALL_IMPL(OcilibQueueTablePurge, con, queue_table, purge_condition, block, delivery_mode);
}

OCI_Boolean OCI_API OCI_QueueTableMigrate
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

OCI_Reference* OCI_API OCI_ReferenceCreate
(
    OCI_TypeInfo* typinf
)
{
    CALL_IMPL(OcilibReferenceCreate, typinf);
}

OCI_Boolean OCI_API OCI_ReferenceFree
(
    OCI_Reference* ref
)
{
    CALL_IMPL(OcilibReferenceFree, ref);
}

OCI_Reference** OCI_API OCI_ReferenceArrayCreate
(
    OCI_TypeInfo* typinf,
    OCI_UInt32    nbelem
)
{
    CALL_IMPL(OcilibReferenceArrayCreate, typinf, nbelem);
}

OCI_Boolean OCI_API OCI_ReferenceArrayFree
(
    OCI_Reference** refs
)
{
    CALL_IMPL(OcilibReferenceArrayFree, refs);
}

OCI_Boolean OCI_API OCI_ReferenceAssign
(
    OCI_Reference* ref,
    OCI_Reference* ref_src
)
{
    CALL_IMPL(OcilibReferenceAssign, ref, ref_src);
}

OCI_TypeInfo* OCI_API OCI_ReferenceGetTypeInfo
(
    OCI_Reference* ref
)
{
    CALL_IMPL(OcilibReferenceGetTypeInfo, ref);
}

OCI_Object* OCI_API OCI_ReferenceGetObject
(
    OCI_Reference* ref
)
{
    CALL_IMPL(OcilibReferenceGetObject, ref);
}

OCI_Boolean OCI_API OCI_ReferenceIsNull
(
    OCI_Reference* ref
)
{
    CALL_IMPL(OcilibReferenceIsNull, ref);
}

OCI_Boolean OCI_API OCI_ReferenceSetNull
(
    OCI_Reference* ref
)
{
    CALL_IMPL(OcilibReferenceSetNull, ref);
}

OCI_UInt32 OCI_API OCI_ReferenceGetHexSize
(
    OCI_Reference* ref
)
{
    CALL_IMPL(OcilibReferenceGetHexSize, ref);
}

OCI_Boolean OCI_API OCI_ReferenceToText
(
    OCI_Reference * ref,
    OCI_UInt32      size,
    otext         * str
)
{
    CALL_IMPL(OcilibReferenceToText, ref, size, str);
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

OCI_UInt32 OCI_API OCI_XmlTypeGetContentSize
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

OCI_Boolean OCI_API OCI_VectorFree
(
    OCI_Vector *vect
)
{
    CALL_IMPL(OcilibVectorFree, vect);
}

OCI_Vector** OCI_API OCI_VectorArrayCreate
(
    OCI_Connection* con,
    OCI_UInt32      nbelem
)
{
    CALL_IMPL(OcilibVectorCreateArray, con, nbelem);
}

OCI_Boolean OCI_API OCI_VectorArrayFree
(
    OCI_Vector** vects
)
{
    CALL_IMPL(OcilibVectorFreeArray, vects);
}

OCI_Boolean OCI_API OCI_VectorGetInfo
(
    OCI_Vector *vect,
    OCI_UInt32 *format,
    OCI_UInt32 *dimensions
)
{
    CALL_IMPL(OcilibVectorGetInfo, vect, format, dimensions);
}

OCI_Boolean OCI_API OCI_VectorGetValues
(
    OCI_Vector *vect,
    void       *values
)
{
    CALL_IMPL(OcilibVectorGetValues, vect, values);
}

OCI_Boolean OCI_API OCI_VectorSetValues
(
    OCI_Vector      *vect,
    OCI_VectorFormat format,
    OCI_UInt32       dimensions,
    void            *values
)
{
    CALL_IMPL(OcilibVectorSetValues, vect, format, dimensions, values);
}

OCI_Boolean OCI_API OCI_VectorFromText
(
    OCI_Vector      *vect,
    const otext    * str,
    OCI_UInt32       size,
    OCI_VectorFormat format,
    OCI_UInt32       dimensions
)
{
    CALL_IMPL(OcilibVectorFromString, vect, str, size, format, dimensions);
}

OCI_Boolean OCI_API OCI_VectorToText
(
    OCI_Vector *vect,
    OCI_UInt32 *size,
    otext      *str
)
{
    CALL_IMPL(OcilibVectorToString, vect, size, str);
}

/* --------------------------------------------------------------------------------------------- *
 *  resultset
 * --------------------------------------------------------------------------------------------- */

OCI_Resultset* OCI_API OCI_StatementGetResultset
(
    OCI_Statement* stmt
)
{
    CALL_IMPL(OcilibStatementGetResultset, stmt);
}

OCI_Resultset* OCI_API OCI_StatementGetNextResultset
(
    OCI_Statement* stmt
)
{
    CALL_IMPL(OcilibStatementGetNextResultset, stmt);
}

OCI_Boolean OCI_API OCI_ResultsetFetchPrev
(
    OCI_Resultset* rs
)
{
    CALL_IMPL(OcilibResultsetFetchPrev, rs);
}

OCI_Boolean OCI_API OCI_ResultsetFetchNext
(
    OCI_Resultset* rs
)
{
    CALL_IMPL(OcilibResultsetFetchNext, rs);
}

OCI_Boolean OCI_API OCI_ResultsetFetchFirst
(
    OCI_Resultset* rs
)
{
    CALL_IMPL(OcilibResultsetFetchFirst, rs);
}

OCI_Boolean OCI_API OCI_ResultsetFetchLast
(
    OCI_Resultset* rs
)
{
    CALL_IMPL(OcilibResultsetFetchLast, rs);
}

OCI_Boolean OCI_API OCI_ResultsetFetchSeek
(
    OCI_Resultset   * rs,
    OCI_FetchSeekMode mode,
    OCI_Int32         offset
)
{
    CALL_IMPL(OcilibResultsetFetchSeek, rs, mode, offset);
}

OCI_UInt32 OCI_API OCI_ResultsetGetRowCount
(
    OCI_Resultset* rs
)
{
    CALL_IMPL(OcilibResultsetGetRowCount, rs);
}

OCI_Int32 OCI_API OCI_ResultsetGetCurrentRow
(
    OCI_Resultset* rs
)
{
    CALL_IMPL(OcilibResultsetGetCurrentRow, rs);
}

OCI_UInt32 OCI_API OCI_ResultsetGetColumnCount
(
    OCI_Resultset* rs
)
{
    CALL_IMPL(OcilibResultsetGetColumnCount, rs);
}

OCI_Column* OCI_API OCI_ResultsetGetColumn
(
    OCI_Resultset* rs,
    OCI_UInt32     index
)
{
    CALL_IMPL(OcilibResultsetGetColumn, rs, index);
}

OCI_Column* OCI_API OCI_ResultsetGetColumnByName
(
    OCI_Resultset* rs,
    const otext  * name
)
{
    CALL_IMPL(OcilibResultsetGetColumnByName, rs, name);
}

OCI_Int32 OCI_API OCI_ResultsetGetColumnIndex
(
    OCI_Resultset* rs,
    const otext  * name
)
{
    CALL_IMPL(OcilibResultsetGetColumnIndex, rs, name);
}

OCI_Boolean OCI_API OCI_ResultsetSetStructNumericType
(
    OCI_Resultset * rs,
    OCI_UInt32      index,
    OCI_NumericType type
)
{
    CALL_IMPL(OcilibResultsetSetStructNumericType, rs, index, type);
}

OCI_Boolean OCI_API OCI_ResultsetSetStructNumericTypeByName
(
    OCI_Resultset * rs,
    const otext   * name,
    OCI_NumericType type
)
{
    CALL_IMPL(OcilibResultsetSetStructNumericTypeByName, rs, name, type);
}

OCI_Boolean OCI_API OCI_ResultsetGetStruct
(
    OCI_Resultset* rs,
    void         * row_struct,
    void         * row_struct_ind
)
{
    CALL_IMPL(OcilibResultsetGetStruct, rs, row_struct, row_struct_ind);
}

OCI_Number* OCI_API OCI_ResultsetGetNumber
(
    OCI_Resultset* rs,
    OCI_UInt32     index
)
{
    CALL_IMPL(OcilibResultsetGetNumber, rs, index);
}

OCI_Number* OCI_API OCI_ResultsetGetNumberByName
(
    OCI_Resultset* rs,
    const otext  * name
)
{
    CALL_IMPL(OcilibResultsetGetNumberByName, rs, name);
}

OCI_Int16 OCI_API OCI_ResultsetGetInt16
(
    OCI_Resultset* rs,
    OCI_UInt32     index
)
{
    CALL_IMPL(OcilibResultsetGetInt16, rs, index);
}

OCI_Int16 OCI_API OCI_ResultsetGetInt16ByName
(
    OCI_Resultset* rs,
    const otext  * name
)
{
    CALL_IMPL(OcilibResultsetGetInt16ByName, rs, name);
}

OCI_UInt16 OCI_API OCI_ResultsetGetUInt16
(
    OCI_Resultset* rs,
    OCI_UInt32     index
)
{
    CALL_IMPL(OcilibResultsetGetUInt16, rs, index);
}

OCI_UInt16 OCI_API OCI_ResultsetGetUInt16ByName
(
    OCI_Resultset* rs,
    const otext  * name
)
{
    CALL_IMPL(OcilibResultsetGetUInt16ByName, rs, name);
}

OCI_Int32 OCI_API OCI_ResultsetGetInt32
(
    OCI_Resultset* rs,
    OCI_UInt32     index
)
{
    CALL_IMPL(OcilibResultsetGetInt32, rs, index);
}

OCI_Int32 OCI_API OCI_ResultsetGetInt32ByName
(
    OCI_Resultset* rs,
    const otext  * name
)
{
    CALL_IMPL(OcilibResultsetGetInt32ByName, rs, name);
}

OCI_UInt32 OCI_API OCI_ResultsetGetUInt32
(
    OCI_Resultset* rs,
    OCI_UInt32     index
)
{
    CALL_IMPL(OcilibResultsetGetUInt32, rs, index);
}

OCI_UInt32 OCI_API OCI_ResultsetGetUInt32ByName
(
    OCI_Resultset* rs,
    const otext  * name
)
{
    CALL_IMPL(OcilibResultsetGetUInt32ByName, rs, name);
}

OCI_Int64 OCI_API OCI_ResultsetGetInt64
(
    OCI_Resultset* rs,
    OCI_UInt32     index
)
{
    CALL_IMPL(OcilibResultsetGetInt64, rs, index);
}

OCI_Int64 OCI_API OCI_ResultsetGetInt64ByName
(
    OCI_Resultset* rs,
    const otext  * name
)
{
    CALL_IMPL(OcilibResultsetGetInt64ByName, rs, name);
}

OCI_UInt64 OCI_API OCI_ResultsetGetUInt64
(
    OCI_Resultset* rs,
    OCI_UInt32     index
)
{
    CALL_IMPL(OcilibResultsetGetUInt64, rs, index);
}

OCI_UInt64 OCI_API OCI_ResultsetGetUInt64ByName
(
    OCI_Resultset* rs,
    const otext  * name
)
{
    CALL_IMPL(OcilibResultsetGetUInt64ByName, rs, name);
}

const otext* OCI_API OCI_ResultsetGetString
(
    OCI_Resultset* rs,
    OCI_UInt32     index
)
{
    CALL_IMPL(OcilibResultsetGetString, rs, index);
}

const otext* OCI_API OCI_ResultsetGetStringByName
(
    OCI_Resultset* rs,
    const otext  * name
)
{
    CALL_IMPL(OcilibResultsetGetStringByName, rs, name);
}

OCI_UInt32 OCI_API OCI_ResultsetGetRaw
(
    OCI_Resultset* rs,
    OCI_UInt32     index,
    void         * buffer,
    OCI_UInt32     len
)
{
    CALL_IMPL(OcilibResultsetGetRaw, rs, index, buffer,len);
}

OCI_UInt32 OCI_API OCI_ResultsetGetRawByName
(
    OCI_Resultset* rs,
    const otext  * name,
    void         * buffer,
    OCI_UInt32     len
)
{
    CALL_IMPL(OcilibResultsetGetRawByName, rs, name, buffer, len);
}

OCI_Double OCI_API OCI_ResultsetGetDouble
(
    OCI_Resultset* rs,
    OCI_UInt32     index
)
{
    CALL_IMPL(OcilibResultsetGetDouble, rs, index);
}

OCI_Double OCI_API OCI_ResultsetGetDoubleByName
(
    OCI_Resultset* rs,
    const otext  * name
)
{
    CALL_IMPL(OcilibResultsetGetDoubleByName, rs, name);
}

OCI_Float OCI_API OCI_ResultsetGetFloat
(
    OCI_Resultset* rs,
    OCI_UInt32     index
)
{
    CALL_IMPL(OcilibResultsetGetFloat, rs, index);
}

OCI_Float OCI_API OCI_ResultsetGetFloatByName
(
    OCI_Resultset* rs,
    const otext  * name
)
{
    CALL_IMPL(OcilibResultsetGetFloatByName, rs, name);
}

OCI_Date* OCI_API OCI_ResultsetGetDate
(
    OCI_Resultset* rs,
    OCI_UInt32     index
)
{
    CALL_IMPL(OcilibResultsetGetDate, rs, index);
}

OCI_Date* OCI_API OCI_ResultsetGetDateByName
(
    OCI_Resultset* rs,
    const otext  * name
)
{
    CALL_IMPL(OcilibResultsetGetDateByName, rs, name);
}

OCI_Timestamp* OCI_API OCI_ResultsetGetTimestamp
(
    OCI_Resultset* rs,
    OCI_UInt32     index
)
{
    CALL_IMPL(OcilibResultsetGetTimestamp, rs, index);
}

OCI_Timestamp* OCI_API OCI_ResultsetGetTimestampByName
(
    OCI_Resultset* rs,
    const otext  * name
)
{
    CALL_IMPL(OcilibResultsetGetTimestampByName, rs, name);
}

OCI_Interval* OCI_API OCI_ResultsetGetInterval
(
    OCI_Resultset* rs,
    OCI_UInt32     index
)
{
    CALL_IMPL(OcilibResultsetGetInterval, rs, index);
}

OCI_Interval* OCI_API OCI_ResultsetGetIntervalByName
(
    OCI_Resultset* rs,
    const otext  * name
)
{
    CALL_IMPL(OcilibResultsetGetIntervalByName, rs, name);
}

OCI_Object* OCI_API OCI_ResultsetGetObject
(
    OCI_Resultset* rs,
    OCI_UInt32     index
)
{
    CALL_IMPL(OcilibResultsetGetObject, rs, index);
}

OCI_Object* OCI_API OCI_ResultsetGetObjectByName
(
    OCI_Resultset* rs,
    const otext  * name
)
{
    CALL_IMPL(OcilibResultsetGetObjectByName, rs, name);
}

OCI_Collection* OCI_API OCI_ResultsetGetColl
(
    OCI_Resultset* rs,
    OCI_UInt32     index
)
{
    CALL_IMPL(OcilibResultsetGetColl, rs, index);
}

OCI_Collection* OCI_API OCI_ResultsetGetCollByName
(
    OCI_Resultset* rs,
    const otext  * name
)
{
    CALL_IMPL(OcilibResultsetGetCollByName, rs, name);
}

OCI_Reference* OCI_API OCI_ResultsetGetReference
(
    OCI_Resultset* rs,
    OCI_UInt32     index
)
{
    CALL_IMPL(OcilibResultsetGetReference, rs, index);
}

OCI_Reference* OCI_API OCI_ResultsetGetReferenceByName
(
    OCI_Resultset* rs,
    const otext  * name
)
{
    CALL_IMPL(OcilibResultsetGetReferenceByName, rs, name);
}

OCI_XmlType* OCI_API OCI_ResultsetGetXmlType
(
    OCI_Resultset* rs,
    OCI_UInt32     index
)
{
    CALL_IMPL(OcilibResultsetGetXmlType, rs, index);
}

OCI_XmlType* OCI_API OCI_ResultsetGetXmlTypeByName
(
    OCI_Resultset* rs,
    const otext  * name
)
{
    CALL_IMPL(OcilibResultsetGetXmlTypeByName, rs, name);
}

OCI_Vector * OCI_API OCI_ResultsetGetVector
(
    OCI_Resultset *rs,
    OCI_UInt32     index
)
{
    CALL_IMPL(OcilibResultsetGetVector, rs, index);
}

OCI_Vector * OCI_API OCI_ResultsetGetVectorByName
(
    OCI_Resultset *rs,
    const otext  * name
)
{
    CALL_IMPL(OcilibResultsetGetVectorByName, rs, name);
}

OCI_Statement* OCI_API OCI_ResultsetGetCursor
(
    OCI_Resultset* rs,
    OCI_UInt32     index
)
{
    CALL_IMPL(OcilibResultsetGetCursor, rs, index);
}

OCI_Statement* OCI_API OCI_ResultsetGetCursorByName
(
    OCI_Resultset* rs,
    const otext  * name
)
{
    CALL_IMPL(OcilibResultsetGetCursorByName, rs, name);
}

OCI_Lob* OCI_API OCI_ResultsetGetLob
(
    OCI_Resultset* rs,
    OCI_UInt32     index
)
{
    CALL_IMPL(OcilibResultsetGetLob, rs, index);
}

OCI_Lob* OCI_API OCI_ResultsetGetLobByName
(
    OCI_Resultset* rs,
    const otext  * name
)
{
    CALL_IMPL(OcilibResultsetGetLobByName, rs, name);
}

OCI_Json* OCI_API OCI_ResultsetGetJson
(
    OCI_Resultset* rs,
    OCI_UInt32     index
)
{
    CALL_IMPL(OcilibResultsetGetJson, rs, index);
}

OCI_Json* OCI_API OCI_ResultsetGetJsonByName
(
    OCI_Resultset* rs,
    const otext  * name
)
{
    CALL_IMPL(OcilibResultsetGetJsonByName, rs, name);
}

OCI_File* OCI_API OCI_ResultsetGetFile
(
    OCI_Resultset* rs,
    OCI_UInt32     index
)
{
    CALL_IMPL(OcilibResultsetGetFile, rs, index);
}

OCI_File* OCI_API OCI_ResultsetGetFileByName
(
    OCI_Resultset* rs,
    const otext  * name
)
{
    CALL_IMPL(OcilibResultsetGetFileByName, rs, name);
}

OCI_Long* OCI_API OCI_ResultsetGetLong
(
    OCI_Resultset* rs,
    OCI_UInt32     index
)
{
    CALL_IMPL(OcilibResultsetGetLong, rs, index);
}

OCI_Long* OCI_API OCI_ResultsetGetLongByName
(
    OCI_Resultset* rs,
    const otext  * name
)
{
    CALL_IMPL(OcilibResultsetGetLongByName, rs, name);
}

OCI_UInt32 OCI_API OCI_ResultsetGetDataSize
(
    OCI_Resultset* rs,
    OCI_UInt32     index
)
{
    CALL_IMPL(OcilibResultsetGetDataSize, rs, index);
}

OCI_UInt32 OCI_API OCI_ResultsetGetDataSizeByName
(
    OCI_Resultset* rs,
    const otext  * name
)
{
    CALL_IMPL(OcilibResultsetGetDataSizeByName, rs, name);
}

OCI_Boolean OCI_API OCI_ResultsetIsColumnNull
(
    OCI_Resultset* rs,
    OCI_UInt32     index
)
{
    CALL_IMPL(OcilibResultsetIsColumnNull, rs, index);
}

OCI_Boolean OCI_API OCI_ResultsetIsColumnNullByName
(
    OCI_Resultset* rs,
    const otext  * name
)
{
    CALL_IMPL(OcilibResultsetIsColumnNullByName, rs, name);
}

OCI_Statement* OCI_API OCI_ResultsetGetStatement
(
    OCI_Resultset* rs
)
{
    CALL_IMPL(OcilibResultsetResultsetGetStatement, rs);
}

OCI_UInt32 OCI_API OCI_ResultsetGetDataLength
(
    OCI_Resultset* rs,
    OCI_UInt32     index
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

OCI_Boolean OCI_API OCI_StatementFree
(
    OCI_Statement* stmt
)
{
    CALL_IMPL(OcilibStatementFree, stmt);
}

OCI_Boolean OCI_API OCI_StatementReleaseResultsets
(
    OCI_Statement* stmt
)
{
    CALL_IMPL(OcilibStatementReleaseResultsets, stmt);
}

OCI_Boolean OCI_API OCI_StatementPrepare
(
    OCI_Statement* stmt,
    const otext  * sql
)
{
    CALL_IMPL(OcilibStatementPrepare, stmt, sql);
}

OCI_Boolean OCI_API OCI_StatementExecutePrepared
(
    OCI_Statement* stmt
)
{
    CALL_IMPL(OcilibStatementExecutePrepared, stmt);
}

OCI_Boolean OCI_API OCI_StatementExecute
(
    OCI_Statement* stmt,
    const otext  * sql
)
{
    CALL_IMPL(OcilibStatementExecute, stmt, sql);
}

OCI_Resultset* OCI_API OCI_StatementExecutePreparedAndGetResultset
(
    OCI_Statement* stmt
)
{
    CALL_IMPL(OcilibStatementExecutePreparedAndGetResultset, stmt);
}

OCI_Resultset* OCI_API OCI_StatementExecuteAndGetResultset
(
    OCI_Statement* stmt,
    const otext  * sql
)
{
    CALL_IMPL(OcilibStatementExecuteAndGetResultset, stmt, sql);
}

OCI_Boolean OCI_API OCI_StatementParse
(
    OCI_Statement* stmt,
    const otext  * sql
)
{
    CALL_IMPL(OcilibStatementParse, stmt, sql);
}

OCI_Boolean OCI_API OCI_StatementDescribe
(
    OCI_Statement* stmt,
    const otext  * sql
)
{
    CALL_IMPL(OcilibStatementDescribe, stmt, sql);
}

OCI_Boolean OCI_StatementPrepareFormat
(
    OCI_Statement* stmt,
    const otext  * sql,
    ...
)
{
    OCI_Boolean res = FALSE;
    va_list     args;

    va_start(args, sql);
    res = OcilibStatementPrepareFormat(stmt, sql, args);
    va_end(args);

    return res;
}

OCI_Boolean OCI_StatementExecuteFormat
(
    OCI_Statement* stmt,
    const otext  * sql,
    ...
)
{
    OCI_Boolean res = FALSE;
    va_list     args;

    va_start(args, sql);
    res = OcilibStatementExecuteFormat(stmt, sql, args);
    va_end(args);

    return res;
}

OCI_Boolean OCI_StatementParseFormat
(
    OCI_Statement* stmt,
    const otext  * sql,
    ...
)
{
    OCI_Boolean res = FALSE;
    va_list     args;

    va_start(args, sql);
    res = OcilibStatementParseFormat(stmt, sql, args);
    va_end(args);

    return res;
}

OCI_Boolean OCI_StatementDescribeFormat
(
    OCI_Statement* stmt,
    const otext  * sql,
    ...
)
{
    OCI_Boolean res = FALSE;
    va_list     args;

    va_start(args, sql);
    res = OcilibStatementDescribeFormat(stmt, sql, args);
    va_end(args);

    return res;
}

OCI_Boolean OCI_API OCI_StatementBindArraySetSize
(
    OCI_Statement* stmt,
    OCI_UInt32     size
)
{
    CALL_IMPL(OcilibStatementBindArraySetSize, stmt, size);
}

OCI_UInt32 OCI_API OCI_StatementBindArrayGetSize
(
    OCI_Statement* stmt
)
{
    CALL_IMPL(OcilibStatementBindArrayGetSize, stmt);
}

OCI_Boolean OCI_API OCI_StatementAllowRebinding
(
    OCI_Statement* stmt,
    OCI_Boolean    value
)
{
    CALL_IMPL(OcilibStatementAllowRebinding, stmt, value);
}

OCI_Boolean OCI_API OCI_StatementIsRebindingAllowed
(
    OCI_Statement* stmt
)
{
    CALL_IMPL(OcilibStatementIsRebindingAllowed, stmt);
}

OCI_Bind* OCI_API OCI_StatementBindBoolean
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Boolean  * data
)
{
    CALL_IMPL(OcilibStatementBindBoolean, stmt, name, data);
}

OCI_Bind* OCI_API OCI_StatementBindNumber
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Number   * data
)
{
    CALL_IMPL(OcilibStatementBindNumber, stmt, name, data);
}

OCI_Bind* OCI_API OCI_StatementBindNumberArray
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Number  ** data,
    OCI_UInt32     nbelem
)
{
    CALL_IMPL(OcilibStatementBindNumberArray, stmt, name, data, nbelem);
}

OCI_Bind* OCI_API OCI_StatementBindInt16
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Int16    * data
)
{
    CALL_IMPL(OcilibStatementBindInt16, stmt, name, data);
}

OCI_Bind* OCI_API OCI_StatementBindInt16Array
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Int16    * data,
    OCI_UInt32     nbelem
)
{
    CALL_IMPL(OcilibStatementBindInt16Array, stmt, name, data, nbelem);
}

OCI_Bind* OCI_API OCI_StatementBindUInt16
(
    OCI_Statement * stmt,
    const otext   * name,
    OCI_UInt16    * data
)
{
    CALL_IMPL(OcilibStatementBindUInt16, stmt, name, data);
}

OCI_Bind* OCI_API OCI_StatementBindUInt16Array
(
    OCI_Statement * stmt,
    const otext   * name,
    OCI_UInt16    * data,
    OCI_UInt32      nbelem
)
{
    CALL_IMPL(OcilibStatementBindUInt16Array, stmt, name, data, nbelem);
}

OCI_Bind* OCI_API OCI_StatementBindInt32
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Int32    * data
)
{
    CALL_IMPL(OcilibStatementBindInt32, stmt, name, data);
}

OCI_Bind* OCI_API OCI_StatementBindInt32Array
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Int32    * data,
    OCI_UInt32     nbelem
)
{
    CALL_IMPL(OcilibStatementBindInt32Array, stmt, name, data, nbelem);
}

OCI_Bind* OCI_API OCI_StatementBindUInt32
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_UInt32   * data
)
{
    CALL_IMPL(OcilibStatementBindUInt32, stmt, name, data);
}

OCI_Bind* OCI_API OCI_StatementBindUInt32Array
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_UInt32   * data,
    OCI_UInt32     nbelem
)
{
    CALL_IMPL(OcilibStatementBindUInt32Array, stmt, name, data, nbelem);
}

OCI_Bind* OCI_API OCI_StatementBindInt64
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Int64    * data
)
{
    CALL_IMPL(OcilibStatementBindInt64, stmt, name, data);
}

OCI_Bind* OCI_API OCI_StatementBindInt64Array
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Int64    * data,
    OCI_UInt32     nbelem
)
{
    CALL_IMPL(OcilibStatementBindInt64Array, stmt, name, data, nbelem);
}

OCI_Bind* OCI_API OCI_StatementBindUInt64
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_UInt64   * data
)
{
    CALL_IMPL(OcilibStatementBindUInt64, stmt, name, data);
}

OCI_Bind* OCI_API OCI_StatementBindUInt64Array
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_UInt64   * data,
    OCI_UInt32     nbelem
)
{
    CALL_IMPL(OcilibStatementBindUInt64Array, stmt, name, data, nbelem);
}

OCI_Bind* OCI_API OCI_StatementBindString
(
    OCI_Statement* stmt,
    const otext  * name,
    otext        * data,
    OCI_UInt32     len
)
{
    CALL_IMPL(OcilibStatementBindString, stmt, name, data, len);
}

OCI_Bind* OCI_API OCI_StatementBindStringArray
(
    OCI_Statement* stmt,
    const otext  * name,
    otext        * data,
    OCI_UInt32     len,
    OCI_UInt32     nbelem
)
{
    CALL_IMPL(OcilibStatementBindStringArray, stmt, name, data, len, nbelem);
}

OCI_Bind* OCI_API OCI_StatementBindRaw
(
    OCI_Statement* stmt,
    const otext  * name,
    void         * data,
    OCI_UInt32     len
)
{
    CALL_IMPL(OcilibStatementBindRaw, stmt, name, data, len);
}

OCI_Bind* OCI_API OCI_StatementBindRawArray
(
    OCI_Statement* stmt,
    const otext  * name,
    void         * data,
    OCI_UInt32     len,
    OCI_UInt32     nbelem
)
{
    CALL_IMPL(OcilibStatementBindRawArray, stmt, name, data, len, nbelem);
}

OCI_Bind* OCI_API OCI_StatementBindDouble
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Double   * data
)
{
    CALL_IMPL(OcilibStatementBindDouble, stmt, name, data);
}

OCI_Bind* OCI_API OCI_StatementBindDoubleArray
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Double   * data,
    OCI_UInt32     nbelem
)
{
    CALL_IMPL(OcilibStatementBindDoubleArray, stmt, name, data, nbelem);
}

OCI_Bind* OCI_API OCI_StatementBindFloat
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Float    * data
)
{
    CALL_IMPL(OcilibStatementBindFloat, stmt, name, data);
}

OCI_Bind* OCI_API OCI_StatementBindFloatArray
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Float    * data,
    OCI_UInt32     nbelem
)
{
    CALL_IMPL(OcilibStatementBindFloatArray, stmt, name, data, nbelem);
}

OCI_Bind* OCI_API OCI_StatementBindDate
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Date     * data
)
{
    CALL_IMPL(OcilibStatementBindDate, stmt, name, data);
}

OCI_Bind* OCI_API OCI_StatementBindDateArray
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Date    ** data,
    OCI_UInt32     nbelem
)
{
    CALL_IMPL(OcilibStatementBindDateArray, stmt, name, data, nbelem);
}

OCI_Bind* OCI_API OCI_StatementBindTimestamp
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Timestamp* data
)
{
    CALL_IMPL(OcilibStatementBindTimestamp, stmt, name, data);
}

OCI_Bind* OCI_API OCI_StatementBindTimestampArray
(
    OCI_Statement   * stmt,
    const otext     * name,
    OCI_Timestamp  ** data,
    OCI_TimestampType type,
    OCI_UInt32        nbelem
)
{
    CALL_IMPL(OcilibStatementBindTimestampArray, stmt, name, data, type, nbelem);
}

OCI_Bind* OCI_API OCI_StatementBindInterval
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Interval * data
)
{
    CALL_IMPL(OcilibStatementBindInterval, stmt, name, data);
}

OCI_Bind* OCI_API OCI_StatementBindIntervalArray
(
    OCI_Statement  * stmt,
    const otext    * name,
    OCI_Interval  ** data,
    OCI_IntervalType type,
    OCI_UInt32       nbelem
)
{
    CALL_IMPL(OcilibStatementBindIntervalArray, stmt, name, data, type, nbelem);
}

OCI_Bind* OCI_API OCI_StatementBindObject
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Object   * data
)
{
    CALL_IMPL(OcilibStatementBindObject, stmt, name, data);
}

OCI_Bind* OCI_API OCI_StatementBindObjectArray
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Object  ** data,
    OCI_TypeInfo * typinf,
    OCI_UInt32     nbelem
)
{
    CALL_IMPL(OcilibStatementBindObjectArray, stmt, name, data, typinf, nbelem);
}

OCI_Bind* OCI_API OCI_StatementBindVector
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Vector   * data
)
{
    CALL_IMPL(OcilibStatementBindVector, stmt, name, data);
}

OCI_Bind* OCI_API OCI_StatementBindVectorArray
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Vector  ** data,
    OCI_UInt32     nbelem
)
{
    CALL_IMPL(OcilibStatementBindVectorArray, stmt, name, data, nbelem);
}

OCI_Bind* OCI_API OCI_StatementBindLob
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Lob      * data
)
{
    CALL_IMPL(OcilibStatementBindLob, stmt, name, data);
}

OCI_Bind* OCI_API OCI_StatementBindJson
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Json     * data
)
{
    CALL_IMPL(OcilibStatementBindJson, stmt, name, data);
}

OCI_Bind* OCI_API OCI_StatementBindJsonArray
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Json    ** data,
    OCI_UInt32     nbelem
)
{
    CALL_IMPL(OcilibStatementBindJsonArray, stmt, name, data, nbelem);
}

OCI_Bind* OCI_API OCI_StatementBindLobArray
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Lob     ** data,
    OCI_LobType    type,
    OCI_UInt32     nbelem
)
{
    CALL_IMPL(OcilibStatementBindLobArray, stmt, name, data, type, nbelem);
}

OCI_Bind* OCI_API OCI_StatementBindFile
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_File     * data
)
{
    CALL_IMPL(OcilibStatementBindFile, stmt, name, data);
}

OCI_Bind* OCI_API OCI_StatementBindFileArray
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_File    ** data,
    OCI_FileType   type,
    OCI_UInt32     nbelem
)
{
    CALL_IMPL(OcilibStatementBindFileArray, stmt, name, data, type, nbelem);
}

OCI_Bind* OCI_API OCI_StatementBindReference
(
    OCI_Statement * stmt,
    const otext   * name,
    OCI_Reference * data
)
{
    CALL_IMPL(OcilibStatementBindReference, stmt, name, data);
}

OCI_Bind* OCI_API OCI_StatementBindReferenceArray
(
    OCI_Statement  * stmt,
    const otext    * name,
    OCI_Reference ** data,
    OCI_TypeInfo   * typinf,
    OCI_UInt32       nbelem
)
{
    CALL_IMPL(OcilibStatementBindReferenceArray, stmt, name, data, typinf, nbelem);
}

OCI_Bind* OCI_API OCI_StatementBindCollection
(
    OCI_Statement  * stmt,
    const otext    * name,
    OCI_Collection * data
)
{
    CALL_IMPL(OcilibStatementBindCollection, stmt, name, data);
}

OCI_Bind* OCI_API OCI_StatementBindCollectionArray
(
    OCI_Statement   * stmt,
    const otext     * name,
    OCI_Collection ** data,
    OCI_TypeInfo    * typinf,
    OCI_UInt32        nbelem
)
{
    CALL_IMPL(OcilibStatementBindCollectionArray, stmt, name, data, typinf, nbelem);
}

OCI_Bind* OCI_API OCI_StatementBindCursor
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Statement* data
)
{
    CALL_IMPL(OcilibStatementBindCursor, stmt, name, data);
}

OCI_Bind* OCI_API OCI_StatementBindLong
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Long     * data,
    OCI_UInt32     size
)
{
    CALL_IMPL(OcilibStatementBindLong, stmt, name, data, size);
}

OCI_Boolean OCI_API OCI_StatementRegisterNumber
(
    OCI_Statement* stmt,
    const otext  * name
)
{
    CALL_IMPL(OcilibStatementRegisterNumber, stmt, name);
}

OCI_Boolean OCI_API OCI_StatementRegisterInt16
(
    OCI_Statement* stmt,
    const otext  * name
)
{
    CALL_IMPL(OcilibStatementRegisterInt16, stmt, name);
}

OCI_Boolean OCI_API OCI_StatementRegisterUInt16
(
    OCI_Statement* stmt,
    const otext  * name
)
{
    CALL_IMPL(OcilibStatementRegisterUInt16, stmt, name);
}

OCI_Boolean OCI_API OCI_StatementRegisterInt32
(
    OCI_Statement* stmt,
    const otext  * name
)
{
    CALL_IMPL(OcilibStatementRegisterInt32, stmt, name);
}

OCI_Boolean OCI_API OCI_StatementRegisterUInt32
(
    OCI_Statement* stmt,
    const otext  * name
)
{
    CALL_IMPL(OcilibStatementRegisterUInt32, stmt, name);
}

OCI_Boolean OCI_API OCI_StatementRegisterInt64
(
    OCI_Statement* stmt,
    const otext  * name
)
{
    CALL_IMPL(OcilibStatementRegisterInt64, stmt, name);
}

OCI_Boolean OCI_API OCI_StatementRegisterUInt64
(
    OCI_Statement* stmt,
    const otext  * name
)
{
    CALL_IMPL(OcilibStatementRegisterUInt64, stmt, name);
}

OCI_Boolean OCI_API OCI_StatementRegisterString
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_UInt32     len
)
{
    CALL_IMPL(OcilibStatementRegisterString, stmt, name, len);
}

OCI_Boolean OCI_API OCI_StatementRegisterRaw
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_UInt32     len
)
{
    CALL_IMPL(OcilibStatementRegisterRaw, stmt, name, len);
}

OCI_Boolean OCI_API OCI_StatementRegisterDouble
(
    OCI_Statement* stmt,
    const otext  * name
)
{
    CALL_IMPL(OcilibStatementRegisterDouble, stmt, name);
}

OCI_Boolean OCI_API OCI_StatementRegisterFloat
(
    OCI_Statement* stmt,
    const otext  * name
)
{
    CALL_IMPL(OcilibStatementRegisterFloat, stmt, name);
}

OCI_Boolean OCI_API OCI_StatementRegisterDate
(
    OCI_Statement* stmt,
    const otext  * name
)
{
    CALL_IMPL(OcilibStatementRegisterDate, stmt, name);
}

OCI_Boolean OCI_API OCI_StatementRegisterTimestamp
(
    OCI_Statement   * stmt,
    const otext     * name,
    OCI_TimestampType type
)
{
    CALL_IMPL(OcilibStatementRegisterTimestamp, stmt, name, type);
}

OCI_Boolean OCI_API OCI_StatementRegisterInterval
(
    OCI_Statement  * stmt,
    const otext    * name,
    OCI_IntervalType type
)
{
    CALL_IMPL(OcilibStatementRegisterInterval, stmt, name, type);
}

OCI_Boolean OCI_API OCI_StatementRegisterObject
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_TypeInfo * typinf
)
{
    CALL_IMPL(OcilibStatementRegisterObject, stmt, name, typinf);
}

OCI_Boolean OCI_API OCI_StatementRegisterLob
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_LobType    type
)
{
    CALL_IMPL(OcilibStatementRegisterLob, stmt, name, type);
}

OCI_Boolean OCI_API OCI_StatementRegisterFile
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_FileType   type
)
{
    CALL_IMPL(OcilibStatementRegisterFile, stmt, name, type);
}

OCI_Boolean OCI_API OCI_StatementRegisterReference
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_TypeInfo * typinf
)
{
    CALL_IMPL(OcilibStatementRegisterReference, stmt, name, typinf);
}

OCI_Boolean OCI_API OCI_StatementRegisterVector
(
    OCI_Statement* stmt,
    const otext  * name
)
{
    CALL_IMPL(OcilibStatementRegisterVector, stmt, name);
}

OCI_Boolean OCI_API OCI_StatementRegisterJson
(
    OCI_Statement* stmt,
    const otext  * name
)
{
    CALL_IMPL(OcilibStatementRegisterJson, stmt, name);
}

OCI_StatementType OCI_API OCI_StatementGetType
(
    OCI_Statement* stmt
)
{
    CALL_IMPL(OcilibStatementGetType, stmt);
}

OCI_Boolean OCI_API OCI_StatementSetFetchMode
(
    OCI_Statement* stmt,
    OCI_FetchMode  mode
)
{
    CALL_IMPL(OcilibStatementSetFetchMode, stmt, mode);
}

OCI_FetchMode OCI_API OCI_StatementGetFetchMode
(
    OCI_Statement* stmt
)
{
    CALL_IMPL(OcilibStatementGetFetchMode, stmt);
}

OCI_Boolean OCI_API OCI_StatementSetBindMode
(
    OCI_Statement* stmt,
    OCI_BindMode   mode
)
{
    CALL_IMPL(OcilibStatementSetBindMode, stmt, mode);
}

OCI_BindMode OCI_API OCI_StatementGetBindMode
(
    OCI_Statement* stmt
)
{
    CALL_IMPL(OcilibStatementGetBindMode, stmt);
}

OCI_Boolean OCI_API OCI_StatementSetBindAllocation
(
    OCI_Statement        * stmt,
    OCI_BindAllocationMode mode
)
{
    CALL_IMPL(OcilibStatementSetBindAllocation, stmt, mode);
}

OCI_BindAllocationMode OCI_API OCI_StatementGetBindAllocation
(
    OCI_Statement* stmt
)
{
    CALL_IMPL(OcilibStatementGetBindAllocation, stmt);
}

OCI_Boolean OCI_API OCI_StatementSetFetchSize
(
    OCI_Statement* stmt,
    OCI_UInt32     size
)
{
    CALL_IMPL(OcilibStatementSetFetchSize, stmt, size);
}

OCI_UInt32 OCI_API OCI_StatementGetFetchSize
(
    OCI_Statement* stmt
)
{
    CALL_IMPL(OcilibStatementGetFetchSize, stmt);
}

OCI_Boolean OCI_API OCI_StatementSetPrefetchSize
(
    OCI_Statement* stmt,
    OCI_UInt32     size
)
{
    CALL_IMPL(OcilibStatementSetPrefetchSize, stmt, size);
}

OCI_UInt32 OCI_API OCI_StatementGetPrefetchSize
(
    OCI_Statement* stmt
)
{
    CALL_IMPL(OcilibStatementGetPrefetchSize, stmt);
}

OCI_Boolean OCI_API OCI_StatementSetPrefetchMemory
(
    OCI_Statement* stmt,
    OCI_UInt32     size
)
{
    CALL_IMPL(OcilibStatementSetPrefetchMemory, stmt, size);
}

OCI_UInt32 OCI_API OCI_StatementGetPrefetchMemory
(
    OCI_Statement* stmt
)
{
    CALL_IMPL(OcilibStatementGetPrefetchMemory, stmt);
}

OCI_Boolean OCI_API OCI_StatementSetPieceSize
(
    OCI_Statement* stmt,
    OCI_UInt32     size
)
{
    CALL_IMPL(OcilibStatementSetPieceSize, stmt, size);
}

OCI_UInt32 OCI_API OCI_StatementGetPieceSize
(
    OCI_Statement* stmt
)
{
    CALL_IMPL(OcilibStatementGetPieceSize, stmt);
}

OCI_Boolean OCI_API OCI_StatementSetLongMode
(
    OCI_Statement* stmt,
    OCI_LongMode   mode
)
{
    CALL_IMPL(OcilibStatementSetLongMode, stmt, mode);
}

OCI_LongMode OCI_API OCI_StatementGetLongMode
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

const otext* OCI_API OCI_StatementGetSql
(
    OCI_Statement* stmt
)
{
    CALL_IMPL(OcilibStatementGetSql, stmt);
}

const otext* OCI_API OCI_StatementGetSqlIdentifier
(
    OCI_Statement* stmt
)
{
    CALL_IMPL(OcilibStatementGetSqlIdentifier, stmt);
}

OCI_UInt32 OCI_API OCI_StatementGetSqlErrorPos
(
    OCI_Statement* stmt
)
{
    CALL_IMPL(OcilibStatementGetSqlErrorPos, stmt);
}

OCI_UInt32 OCI_API OCI_StatementGetAffectedRows
(
    OCI_Statement* stmt
)
{
    CALL_IMPL(OcilibStatementGetAffectedRows, stmt);
}

OCI_UInt32 OCI_API OCI_StatementGetBindCount
(
    OCI_Statement* stmt
)
{
    CALL_IMPL(OcilibStatementGetBindCount, stmt);
}

OCI_Bind* OCI_API OCI_StatementGetBind
(
    OCI_Statement* stmt,
    OCI_UInt32     index
)
{
    CALL_IMPL(OcilibStatementGetBind, stmt, index);
}

OCI_Bind* OCI_API OCI_StatementGetBindByName
(
    OCI_Statement* stmt,
    const otext  * name
)
{
    CALL_IMPL(OcilibStatementGetBindByName, stmt, name);
}

OCI_Int32 OCI_API OCI_StatementGetBindIndex
(
    OCI_Statement* stmt,
    const otext  * name
)
{
    CALL_IMPL(OcilibStatementGetBindIndex, stmt, name);
}

OCI_SQLCommand OCI_API OCI_StatementGetSqlCommand
(
    OCI_Statement* stmt
)
{
    CALL_IMPL(OcilibStatementGetSqlCommand, stmt);
}

const otext* OCI_API OCI_StatementGetSqlVerb
(
    OCI_Statement* stmt
)
{
    CALL_IMPL(OcilibStatementGetSqlVerb, stmt);
}

OCI_Boolean OCI_API OCI_StatementGetParseBindNames
(
    OCI_Statement* stmt,
    OCI_UInt32   * count,
    const otext*** names
)
{
    CALL_IMPL(OcilibStatementGetParseBindNames, stmt, count, names);
}

OCI_Error* OCI_API OCI_StatementGetBatchError
(
    OCI_Statement* stmt
)
{
    CALL_IMPL(OcilibStatementGetBatchError, stmt);
}

OCI_UInt32 OCI_API OCI_StatementGetBatchErrorCount
(
    OCI_Statement* stmt
)
{
    CALL_IMPL(OcilibStatementGetBatchErrorCount, stmt);
}

/* --------------------------------------------------------------------------------------------- *
 *  subscription
 * --------------------------------------------------------------------------------------------- */

OCI_Subscription* OCI_API OCI_SubscriptionCreate
(
    OCI_Connection     * con,
    const otext        * name,
    OCI_SubscriptionType type,
    POCI_NOTIFY          handler,
    OCI_SubscriptionMode mode,
    OCI_UInt32           timeout
)
{
    CALL_IMPL(OcilibSubscriptionCreate, con, name, type, handler, mode, timeout);
}

OCI_Boolean OCI_API OCI_SubscriptionUnregister
(
    OCI_Subscription* sub
)
{
    CALL_IMPL(OcilibSubscriptionUnregister, sub);
}

OCI_Boolean OCI_API OCI_SubscriptionAddStatement
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

OCI_UInt32 OCI_API OCI_SubscriptionGetTimeout
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
    OCI_Environment * env,
    OCI_TimestampType type
)
{
    CALL_IMPL(OcilibTimestampCreate, env, type);
}

OCI_Boolean OCI_API OCI_TimestampFree
(
    OCI_Timestamp* tmsp
)
{
    CALL_IMPL(OcilibTimestampFree, tmsp);
}

OCI_Timestamp** OCI_API OCI_TimestampArrayCreate
(
    OCI_Environment * env,
    OCI_TimestampType type,
    OCI_UInt32        nbelem
)
{
    CALL_IMPL_E(OcilibTimestampCreateArray, env, type, nbelem);
}

OCI_Boolean OCI_API OCI_TimestampArrayFree
(
    OCI_Timestamp** tmsps
)
{
    CALL_IMPL(OcilibTimestampFreeArray, tmsps);
}

OCI_TimestampType OCI_API OCI_TimestampGetType
(
    OCI_Timestamp* tmsp
)
{
    CALL_IMPL(OcilibTimestampGetType, tmsp);
}

OCI_Boolean OCI_API OCI_TimestampAssign
(
    OCI_Timestamp* tmsp,
    OCI_Timestamp* tmsp_src
)
{
    CALL_IMPL(OcilibTimestampAssign, tmsp, tmsp_src);
}

OCI_Int32 OCI_API OCI_TimestampCheck
(
    OCI_Timestamp* tmsp
)
{
    CALL_IMPL(OcilibTimestampCheck, tmsp);
}

OCI_Int32 OCI_API OCI_TimestampCompare
(
    OCI_Timestamp* tmsp,
    OCI_Timestamp* tmsp2
)
{
    CALL_IMPL(OcilibTimestampCompare, tmsp, tmsp2);
}

OCI_Boolean OCI_API OCI_TimestampConstruct
(
    OCI_Timestamp* tmsp,
    OCI_Int32      year,
    OCI_Int32      month,
    OCI_Int32      day,
    OCI_Int32      hour,
    OCI_Int32      min,
    OCI_Int32      sec,
    OCI_Int32      fsec,
    const otext  * time_zone
)
{
    CALL_IMPL(OcilibTimestampConstruct, tmsp, year, month, day, hour, min, sec,fsec, time_zone);
}

OCI_Boolean OCI_API OCI_TimestampConvert
(
    OCI_Timestamp* tmsp,
    OCI_Timestamp* tmsp_src
)
{
    CALL_IMPL(OcilibTimestampConvert, tmsp, tmsp_src);
}

OCI_Boolean OCI_API OCI_TimestampFromText
(
    OCI_Timestamp* tmsp,
    const otext  * str,
    const otext  * fmt
)
{
    CALL_IMPL(OcilibTimestampFromString, tmsp, str, fmt);
}

OCI_Boolean OCI_API OCI_TimestampToText
(
    OCI_Timestamp* tmsp,
    const otext  * fmt,
    OCI_Int32      size,
    otext        * str,
    OCI_Int32      precision
)
{
    CALL_IMPL(OcilibTimestampToString, tmsp, fmt, size, str, precision);
}

OCI_Boolean OCI_API OCI_TimestampGetDate
(
    OCI_Timestamp* tmsp,
    OCI_Int32    * year,
    OCI_Int32    * month,
    OCI_Int32    * day
)
{
    CALL_IMPL(OcilibTimestampGetDate, tmsp, year, month, day);
}

OCI_Boolean OCI_API OCI_TimestampGetTime
(
    OCI_Timestamp* tmsp,
    OCI_Int32    * hour,
    OCI_Int32    * min,
    OCI_Int32    * sec,
    OCI_Int32    * fsec
)
{
    CALL_IMPL(OcilibTimestampGetTime, tmsp, hour, min, sec, fsec);
}

OCI_Boolean OCI_API OCI_TimestampGetDateTime
(
    OCI_Timestamp* tmsp,
    OCI_Int32    * year,
    OCI_Int32    * month,
    OCI_Int32    * day,
    OCI_Int32    * hour,
    OCI_Int32    * min,
    OCI_Int32    * sec,
    OCI_Int32    * fsec
)
{
    CALL_IMPL(OcilibTimestampGetDateTime, tmsp, year, month, day, hour, min, sec, fsec);
}

OCI_Boolean OCI_API OCI_TimestampGetTimeZoneName
(
    OCI_Timestamp* tmsp,
    OCI_Int32      size,
    otext        * str
)
{
    CALL_IMPL(OcilibTimestampGetTimeZoneName, tmsp, size, str);
}

OCI_Boolean OCI_API OCI_TimestampGetTimeZoneOffset
(
    OCI_Timestamp* tmsp,
    OCI_Int32    * hour,
    OCI_Int32    * min
)
{
    CALL_IMPL(OcilibTimestampGetTimeZoneOffset, tmsp, hour,min);
}

OCI_Boolean OCI_API OCI_TimestampIntervalAdd
(
    OCI_Timestamp* tmsp,
    OCI_Interval * itv
)
{
    CALL_IMPL(OcilibTimestampIntervalAdd, tmsp, itv);
}

OCI_Boolean OCI_API OCI_TimestampIntervalSub
(
    OCI_Timestamp* tmsp,
    OCI_Interval * itv
)
{
    CALL_IMPL(OcilibTimestampIntervalSub, tmsp, itv);
}

OCI_Boolean OCI_API OCI_TimestampSubtract
(
    OCI_Timestamp* tmsp,
    OCI_Timestamp* tmsp2,
    OCI_Interval * itv
)
{
    CALL_IMPL(OcilibTimestampSubtract, tmsp, tmsp2, itv);
}

OCI_Boolean OCI_API OCI_TimestampSysTimestamp
(
    OCI_Timestamp* tmsp
)
{
    CALL_IMPL(OcilibTimestampSysTimestamp, tmsp);
}

OCI_Boolean OCI_API OCI_TimestampToCTime
(
    OCI_Timestamp* tmsp,
    struct tm    * ptm,
    time_t       * pt
)
{
    CALL_IMPL(OcilibTimestampToCTime, tmsp, ptm, pt);
}

OCI_Boolean OCI_API OCI_TimestampFromCTime
(
    OCI_Timestamp* tmsp,
    struct tm    * ptm,
    time_t         t
)
{
    CALL_IMPL(OcilibTimestampFromCTime, tmsp, ptm, t);
}

/* --------------------------------------------------------------------------------------------- *
 *  transaction
 * --------------------------------------------------------------------------------------------- */

OCI_Transaction* OCI_API OCI_TransactionCreate
(
    OCI_Connection    * con,
    OCI_UInt32          timeout,
    OCI_TransactionMode mode,
    OCI_XID           * pxid
)
{
    CALL_IMPL(OcilibTransactionCreate, con, timeout, mode, pxid);
}

OCI_Boolean OCI_API OCI_TransactionFree
(
    OCI_Transaction* trans
)
{
    CALL_IMPL(OcilibTransactionFree, trans);
}

OCI_Boolean OCI_API OCI_TransactionStart
(
    OCI_Transaction* trans
)
{
    CALL_IMPL(OcilibTransactionStart, trans);
}

OCI_Boolean OCI_API OCI_TransactionStop
(
    OCI_Transaction* trans
)
{
    CALL_IMPL(OcilibTransactionStop, trans);
}

OCI_Boolean OCI_API OCI_TransactionResume
(
    OCI_Transaction* trans
)
{
    CALL_IMPL(OcilibTransactionResume, trans);
}

OCI_Boolean OCI_API OCI_TransactionPrepare
(
    OCI_Transaction* trans
)
{
    CALL_IMPL(OcilibTransactionPrepare, trans);
}

OCI_Boolean OCI_API OCI_TransactionForget
(
    OCI_Transaction* trans
)
{
    CALL_IMPL(OcilibTransactionForget, trans);
}

OCI_TransactionMode OCI_API OCI_TransactionGetMode
(
    OCI_Transaction* trans
)
{
    CALL_IMPL(OcilibTransactionGetMode, trans);
}

OCI_UInt32 OCI_API OCI_TransactionGetTimeout
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
    OCI_Connection * con,
    const otext    * name,
    OCI_TypeInfoType type
)
{
    CALL_IMPL(OcilibTypeInfoGet, con, name, type);
}

OCI_TypeInfoType OCI_API OCI_TypeInfoGetType
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

OCI_Boolean OCI_API OCI_TypeInfoFree
(
    OCI_TypeInfo* typinf
)
{
    CALL_IMPL(OcilibTypeInfoFree, typinf);
}

OCI_UInt32 OCI_API OCI_TypeInfoGetColumnCount
(
    OCI_TypeInfo* typinf
)
{
    CALL_IMPL(OcilibTypeInfoGetColumnCount, typinf);
}

OCI_Column* OCI_API OCI_TypeInfoGetColumn
(
    OCI_TypeInfo* typinf,
    OCI_UInt32    index
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

OCI_Boolean OCI_API OCI_TypeInfoIsFinalType
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

/* --------------------------------------------------------------------------------------------- *
 * SODA
 * --------------------------------------------------------------------------------------------- */

OCI_SodaDatabase* OCI_API OCI_SodaDatabaseCreate
(
    OCI_Connection* con
)
{
    CALL_IMPL(OcilibSodaDatabaseCreate, con);
}

OCI_Boolean OCI_API OCI_SodaDatabaseFree
(
    OCI_SodaDatabase* sdb
)
{
    CALL_IMPL(OcilibSodaDatabaseFree, sdb);
}

OCI_Connection* OCI_API OCI_SodaDatabaseGetConnection
(
    OCI_SodaDatabase* sdb
)
{
    CALL_IMPL(OcilibSodaDatabaseGetConnection, sdb);
}

OCI_SodaCollection* OCI_API OCI_SodaDatabaseCreateCollection
(
    OCI_SodaDatabase * sdb,
    const otext      * name,
    const otext      * metadata
)
{
    CALL_IMPL(OcilibSodaDatabaseCreateCollection, sdb, name, metadata);
}

OCI_SodaCollection* OCI_API OCI_SodaDatabaseOpenCollection
(
    OCI_SodaDatabase * sdb,
    const otext      * name
)
{
    CALL_IMPL(OcilibSodaDatabaseOpenCollection, sdb, name);
}

OCI_SodaCollectionCursor* OCI_API OCI_SodaDatabaseGetCollections
(
    OCI_SodaDatabase * sdb,
    const otext      * startName
)
{
    CALL_IMPL(OcilibSodaDatabaseGetCollections, sdb, startName);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_SodaDatabaseGetCollectionNames
 * --------------------------------------------------------------------------------------------- */

otext** OCI_API OCI_SodaDatabaseGetCollectionNames
(
    OCI_SodaDatabase *sdb,
    OCI_UInt32       *count
)
{
    CALL_IMPL(OcilibSodaDatabaseGetCollectionNames, sdb, count);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_SodaDatabaseFreeCollectionNames
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OCI_API OCI_SodaDatabaseFreeCollectionNames
(
    OCI_SodaDatabase *sdb,
    otext           **names,
    OCI_UInt32        count
)
{
    CALL_IMPL(OcilibSodaDatabaseFreeCollectionNames, sdb, names, count);
}

OCI_SodaDocument* OCI_API OCI_SodaDocumentCreateFromText
(
    OCI_SodaDatabase * sdb,
    const otext      * content,
    OCI_UInt32         contentSize,
    const otext      * key
)
{
    CALL_IMPL(OcilibSodaDocumentCreateFromText, sdb, content, contentSize, key);
}

OCI_SodaDocument* OCI_API OCI_SodaDocumentCreateFromJson
(
    OCI_SodaDatabase * sdb,
    OCI_Json         * json,
    const otext      * key
)
{
    CALL_IMPL(OcilibSodaDocumentCreateFromJson, sdb, json, key);
}

OCI_Boolean OCI_API OCI_SodaDocumentFree
(
    OCI_SodaDocument* doc
)
{
    CALL_IMPL(OcilibSodaDocumentFree, doc);
}

const otext* OCI_API OCI_SodaCollectionGetName
(
    OCI_SodaCollection* coll
)
{
    CALL_IMPL(OcilibSodaCollectionGetName, coll);
}

const otext* OCI_API OCI_SodaCollectionGetMetadata
(
    OCI_SodaCollection* coll
)
{
    CALL_IMPL(OcilibSodaCollectionGetMetadata, coll);
}

OCI_Connection* OCI_API OCI_SodaCollectionGetConnection
(
    OCI_SodaCollection* coll
)
{
    CALL_IMPL(OcilibSodaCollectionGetConnection, coll);
}

OCI_Boolean OCI_API OCI_SodaCollectionIsMapped
(
    OCI_SodaCollection* coll
)
{
    CALL_IMPL(OcilibSodaCollectionIsMapped, coll);
}

OCI_Boolean OCI_API OCI_SodaCollectionDrop
(
    OCI_SodaCollection * coll
)
{
    CALL_IMPL(OcilibSodaCollectionDrop, coll);
}

OCI_Boolean OCI_API OCI_SodaCollectionTruncate
(
    OCI_SodaCollection * coll
)
{
    CALL_IMPL(OcilibSodaCollectionTruncate, coll);
}

OCI_Boolean OCI_API OCI_SodaCollectionFree
(
    OCI_SodaCollection* coll
)
{
    CALL_IMPL(OcilibSodaCollectionFree, coll);
}

const otext* OCI_API OCI_SodaDocumentGetKey
(
    OCI_SodaDocument* doc
)
{
    CALL_IMPL(OcilibSodaDocumentGetKey, doc);
}

const otext* OCI_API OCI_SodaDocumentGetVersion
(
    OCI_SodaDocument* doc
)
{
    CALL_IMPL(OcilibSodaDocumentGetVersion, doc);
}

const otext* OCI_API OCI_SodaDocumentGetCreatedOn
(
    OCI_SodaDocument* doc
)
{
    CALL_IMPL(OcilibSodaDocumentGetCreatedOn, doc);
}

const otext* OCI_API OCI_SodaDocumentGetLastModified
(
    OCI_SodaDocument* doc
)
{
    CALL_IMPL(OcilibSodaDocumentGetLastModified, doc);
}

const otext* OCI_API OCI_SodaDocumentGetMediaType
(
    OCI_SodaDocument* doc
)
{
    CALL_IMPL(OcilibSodaDocumentGetMediaType, doc);
}

OCI_Boolean OCI_API OCI_SodaDocumentGetIsJson
(
    OCI_SodaDocument* doc
)
{
    CALL_IMPL(OcilibSodaDocumentGetIsJson, doc);
}

const otext* OCI_API OCI_SodaDocumentGetText
(
    OCI_SodaDocument* doc
)
{
    CALL_IMPL(OcilibSodaDocumentGetText, doc);
}

OCI_Json* OCI_API OCI_SodaDocumentGetJson
(
    OCI_SodaDocument* doc
)
{
    CALL_IMPL(OcilibSodaDocumentGetJson, doc);
}

OCI_SodaOptions* OCI_API OCI_SodaOptionsCreate
(
    OCI_SodaDatabase* sdb
)
{
    CALL_IMPL(OcilibSodaOptionsCreate, sdb);
}

OCI_Boolean OCI_API OCI_SodaOptionsFree
(
    OCI_SodaOptions* options
)
{
    CALL_IMPL(OcilibSodaOptionsFree, options);
}

OCI_Boolean OCI_API OCI_SodaOptionsSetKey
(
    OCI_SodaOptions * options,
    const otext     * key
)
{
    CALL_IMPL(OcilibSodaOptionsSetKey, options, key);
}

OCI_Boolean OCI_API OCI_SodaOptionsSetKeys
(
    OCI_SodaOptions * options,
    const otext    ** keys,
    OCI_UInt32        count
)
{
    CALL_IMPL(OcilibSodaOptionsSetKeys, options, keys, count);
}

OCI_Boolean OCI_API OCI_SodaOptionsSetVersion
(
    OCI_SodaOptions * options,
    const otext     * version
)
{
    CALL_IMPL(OcilibSodaOptionsSetVersion, options, version);
}

OCI_Boolean OCI_API OCI_SodaOptionsSetFilter
(
    OCI_SodaOptions * options,
    const otext     * filterSpec
)
{
    CALL_IMPL(OcilibSodaOptionsSetFilter, options, filterSpec);
}

OCI_Boolean OCI_API OCI_SodaOptionsSetSkip
(
    OCI_SodaOptions * options,
    OCI_UInt32        count
)
{
    CALL_IMPL(OcilibSodaOptionsSetSkip, options, count);
}

OCI_Boolean OCI_API OCI_SodaOptionsSetLimit
(
    OCI_SodaOptions * options,
    OCI_UInt32        count
)
{
    CALL_IMPL(OcilibSodaOptionsSetLimit, options, count);
}

OCI_Boolean OCI_API OCI_SodaOptionsSetFetchArraySize
(
    OCI_SodaOptions * options,
    OCI_UInt32        count
)
{
    CALL_IMPL(OcilibSodaOptionsSetFetchArraySize, options, count);
}

OCI_Boolean OCI_API OCI_SodaOptionsSetHint
(
    OCI_SodaOptions * options,
    const otext     * hint
)
{
    CALL_IMPL(OcilibSodaOptionsSetHint, options, hint);
}

OCI_Boolean OCI_API OCI_SodaOptionsSetLock
(
    OCI_SodaOptions * options,
    OCI_Boolean       lock
)
{
    CALL_IMPL(OcilibSodaOptionsSetLock, options, lock);
}

OCI_SodaCollection* OCI_API OCI_SodaCollectionCursorGetNext
(
    OCI_SodaCollectionCursor* cursor
)
{
    CALL_IMPL(OcilibSodaCollectionCursorGetNext, cursor);
}

OCI_Boolean OCI_API OCI_SodaCollectionCursorFree
(
    OCI_SodaCollectionCursor* cursor
)
{
    CALL_IMPL(OcilibSodaCollectionCursorFree, cursor);
}

OCI_SodaDocument* OCI_API OCI_SodaDocumentCursorGetNext
(
    OCI_SodaDocumentCursor * cursor
)
{
    CALL_IMPL(OcilibSodaDocumentCursorGetNext, cursor);
}

OCI_Boolean OCI_API OCI_SodaDocumentCursorFree
(
    OCI_SodaDocumentCursor* cursor
)
{
    CALL_IMPL(OcilibSodaDocumentCursorFree, cursor);
}

OCI_Boolean OCI_API OCI_SodaCollectionInsertDocument
(
    OCI_SodaCollection * coll,
    OCI_SodaDocument   * doc
)
{
    CALL_IMPL(OcilibSodaCollectionInsertDocument, coll, doc);
}

OCI_SodaDocumentCursor* OCI_API OCI_SodaCollectionFind
(
    OCI_SodaCollection * coll,
    OCI_SodaOptions    * options
)
{
    CALL_IMPL(OcilibSodaCollectionFind, coll, options);
}

OCI_SodaDocument* OCI_API OCI_SodaCollectionFindOne
(
    OCI_SodaCollection * coll,
    OCI_SodaOptions    * options
)
{
    CALL_IMPL(OcilibSodaCollectionFindOne, coll, options);
}

OCI_SodaDocument* OCI_API OCI_SodaCollectionFindOneByKey
(
    OCI_SodaCollection * coll,
    const otext        * key
)
{
    CALL_IMPL(OcilibSodaCollectionFindOneByKey, coll, key);
}

OCI_UInt64 OCI_API OCI_SodaCollectionDocumentCount
(
    OCI_SodaCollection * coll,
    OCI_SodaOptions    * options
)
{
    CALL_IMPL(OcilibSodaCollectionDocumentCount, coll, options);
}

OCI_UInt64 OCI_API OCI_SodaCollectionDocumentCountWithFilter
(
    OCI_SodaCollection * coll,
    const otext        * filterSpec
)
{
    CALL_IMPL(OcilibSodaCollectionDocumentCountWithFilter, coll, filterSpec);
}

OCI_Boolean OCI_API OCI_SodaCollectionReplaceOne
(
    OCI_SodaCollection * coll,
    OCI_SodaOptions    * options,
    OCI_SodaDocument   * doc,
    OCI_Boolean          refresh
)
{
    CALL_IMPL(OcilibSodaCollectionReplaceOne, coll, options, doc, refresh);
}

OCI_Boolean OCI_API OCI_SodaCollectionReplaceOneByKey
(
    OCI_SodaCollection * coll,
    const otext        * key,
    OCI_SodaDocument   * doc,
    OCI_Boolean          refresh
)
{
    CALL_IMPL(OcilibSodaCollectionReplaceOneByKey, coll, key, doc, refresh);
}

OCI_Boolean OCI_API OCI_SodaCollectionRemove
(
    OCI_SodaCollection * coll,
    OCI_SodaOptions    * options,
    OCI_UInt64         * count
)
{
    CALL_IMPL(OcilibSodaCollectionRemove, coll, options, count);
}

OCI_Boolean OCI_API OCI_SodaCollectionRemoveOneByKey
(
    OCI_SodaCollection * coll,
    const otext        * key
)
{
    CALL_IMPL(OcilibSodaCollectionRemoveOneByKey, coll, key);
}

OCI_Boolean OCI_API OCI_SodaCollectionInsertMany
(
    OCI_SodaCollection *coll,
    OCI_SodaDocument  **docs,
    OCI_UInt32          count,
    OCI_Boolean         refresh
)
{
    CALL_IMPL(OcilibSodaCollectionInsertMany, coll, docs, count, refresh);
}

OCI_Boolean OCI_API OCI_SodaCollectionCreateIndex
(
    OCI_SodaCollection *coll,
    const otext        *indexSpec
)
{
    CALL_IMPL(OcilibSodaCollectionCreateIndex, coll, indexSpec);
}

OCI_Boolean OCI_API OCI_SodaCollectionDropIndex
(
    OCI_SodaCollection *coll,
    const otext        *name
)
{
    CALL_IMPL(OcilibSodaCollectionDropIndex, coll, name);
}

OCI_SodaDocument* OCI_API OCI_SodaCollectionGetDataGuide
(
    OCI_SodaCollection *coll
)
{
    CALL_IMPL(OcilibSodaCollectionGetDataGuide, coll);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_SodaCollectionListIndexes
 * --------------------------------------------------------------------------------------------- */

otext** OCI_API OCI_SodaCollectionListIndexes
(
    OCI_SodaCollection *coll,
    OCI_UInt32         *count
)
{
    CALL_IMPL(OcilibSodaCollectionListIndexes, coll, count);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_SodaCollectionFreeIndexNames
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OCI_API OCI_SodaCollectionFreeIndexNames
(
    OCI_SodaCollection *coll,
    otext             **names,
    OCI_UInt32          count
)
{
    CALL_IMPL(OcilibSodaCollectionFreeIndexNames, coll, names, count);
}

OCI_Boolean OCI_API OCI_SodaCollectionSave
(
    OCI_SodaCollection *coll,
    OCI_SodaDocument   *doc,
    OCI_Boolean         refresh
)
{
    CALL_IMPL(OcilibSodaCollectionSave, coll, doc, refresh);
}

/* --------------------------------------------------------------------------------------------- *
 * JSON
 * --------------------------------------------------------------------------------------------- */

OCI_Json * OCI_API OCI_JsonCreate
(
    OCI_Connection *con
)
{
    CALL_IMPL(OcilibJsonCreate, con);
}

OCI_Json ** OCI_API OCI_JsonArrayCreate
(
    OCI_Connection *con,
    OCI_UInt32      nbelem
)
{
    CALL_IMPL(OcilibJsonArrayCreate, con, nbelem);
}

OCI_Boolean OCI_API OCI_JsonFree
(
    OCI_Json *json
)
{
    CALL_IMPL(OcilibJsonFree, json);
}

OCI_Boolean OCI_API OCI_JsonArrayFree
(
    OCI_Json **jsons
)
{
    CALL_IMPL(OcilibJsonArrayFree, jsons);
}

OCI_Connection* OCI_API OCI_JsonGetConnection
(
    OCI_Json* json
)
{
    CALL_IMPL(OcilibJsonGetConnection, json);
}

OCI_Boolean OCI_API OCI_JsonValidate
(
    OCI_Json  * json,
    OCI_Json  * schema,
    OCI_Json ** errors
)
{
    CALL_IMPL(OcilibJsonValidate, json, schema, errors);
}

OCI_Boolean OCI_API OCI_JsonLoadText
(
    OCI_Json          *json,
    const otext       *json_text,
    OCI_UInt32         length,
    OCI_JsonValidation validation,
    OCI_JsonEncoding   encoding
)
{
    CALL_IMPL(OcilibJsonLoadText, json, json_text, length, validation, encoding);
}

OCI_Boolean OCI_API OCI_JsonLoadBinary
(
    OCI_Json   *json,
    const void *data,
    OCI_UInt32  length
)
{
    CALL_IMPL(OcilibJsonLoadBinary, json, data, length);
}

OCI_Boolean OCI_API OCI_JsonToText
(
    OCI_Json     *json,
    otext        *buffer,
    OCI_UInt32   *length,
    OCI_JsonPrint print
)
{
    CALL_IMPL(OcilibJsonToText, json, buffer, length, print);
}

OCI_Boolean OCI_API OCI_JsonToBinary
(
    OCI_Json   *json,
    void       *buffer,
    OCI_UInt32 *length
)
{
    CALL_IMPL(OcilibJsonToBinary, json, buffer, length);
}

OCI_Boolean OCI_API OCI_JsonClone
(
    OCI_Json *src,
    OCI_Json *dest
)
{
    CALL_IMPL(OcilibJsonClone, src, dest);
}

OCI_Boolean OCI_API OCI_JsonSetMutable
(
    OCI_Json   *json,
    OCI_Boolean value
)
{
    CALL_IMPL(OcilibJsonSetMutable, json, value);
}

OCI_Boolean OCI_API OCI_JsonIsMutable
(
    OCI_Json *json
)
{
    CALL_IMPL(OcilibJsonIsMutable, json);
}

OCI_JsonNode * OCI_API OCI_JsonGetRoot
(
    OCI_Json *json
)
{
    CALL_IMPL(OcilibJsonGetRoot, json);
}

OCI_Boolean OCI_API OCI_JsonSetRoot
(
    OCI_Json     *json,
    OCI_JsonNode *node
)
{
    CALL_IMPL(OcilibJsonSetRoot, json, node);
}

OCI_JsonNode * OCI_API OCI_JsonNodeCreate
(
    OCI_Json    *json,
    OCI_JsonType type
)
{
    CALL_IMPL(OcilibJsonNodeCreate, json, type);
}

OCI_JsonNode * OCI_API OCI_JsonNodeCreateString
(
    OCI_Json    *json,
    const otext *value,
    OCI_UInt32   length
)
{
    CALL_IMPL(OcilibJsonNodeCreateString, json, value, length);
}

OCI_JsonNode * OCI_API OCI_JsonNodeCreateNumber
(
    OCI_Json   *json,
    OCI_Number *value
)
{
    CALL_IMPL(OcilibJsonNodeCreateNumber, json, value);
}

OCI_JsonNode * OCI_API OCI_JsonNodeCreateBoolean
(
    OCI_Json   *json,
    OCI_Boolean value
)
{
    CALL_IMPL(OcilibJsonNodeCreateBoolean, json, value);
}

OCI_JsonNode * OCI_API OCI_JsonNodeCreateNull
(
    OCI_Json *json
)
{
    CALL_IMPL(OcilibJsonNodeCreateNull, json);
}

OCI_JsonNode * OCI_API OCI_JsonNodeCreateBinary
(
    OCI_Json   *json,
    const void *value,
    OCI_UInt32  length
)
{
    CALL_IMPL(OcilibJsonNodeCreateBinary, json, value, length);
}

OCI_JsonNode * OCI_API OCI_JsonNodeCreateDate
(
    OCI_Json *json,
    OCI_Date *value
)
{
    CALL_IMPL(OcilibJsonNodeCreateDate, json, value);
}

OCI_JsonNode * OCI_API OCI_JsonNodeCreateTimestamp
(
    OCI_Json      *json,
    OCI_Timestamp *value
)
{
    CALL_IMPL(OcilibJsonNodeCreateTimestamp, json, value);
}

OCI_JsonNode * OCI_API OCI_JsonNodeCreateInterval
(
    OCI_Json     *json,
    OCI_Interval *value
)
{
    CALL_IMPL(OcilibJsonNodeCreateInterval, json, value);
}

OCI_JsonNode * OCI_API OCI_JsonNodeCreateObject
(
    OCI_Json *json
)
{
    CALL_IMPL(OcilibJsonNodeCreateObject, json);
}

OCI_JsonNode * OCI_API OCI_JsonNodeCreateArray
(
    OCI_Json *json
)
{
    CALL_IMPL(OcilibJsonNodeCreateArray, json);
}

OCI_Boolean OCI_API OCI_JsonNodeFree
(
    OCI_JsonNode *node
)
{
    CALL_IMPL(OcilibJsonNodeFree, node);
}

OCI_JsonType OCI_API OCI_JsonNodeGetType
(
    OCI_JsonNode *node
)
{
    CALL_IMPL(OcilibJsonNodeGetType, node);
}

OCI_Boolean OCI_API OCI_JsonNodeIsNull
(
    OCI_JsonNode *node
)
{
    CALL_IMPL(OcilibJsonNodeIsNull, node);
}

OCI_UInt32 OCI_API OCI_JsonNodeGetFieldCount
(
    OCI_JsonNode *node
)
{
    CALL_IMPL(OcilibJsonNodeGetFieldCount, node);
}

const otext * OCI_API OCI_JsonNodeGetFieldName
(
    OCI_JsonNode *node,
    OCI_UInt32    index
)
{
    CALL_IMPL(OcilibJsonNodeGetFieldName, node, index);
}

OCI_JsonNode * OCI_API OCI_JsonNodeGetFieldByName
(
    OCI_JsonNode *node,
    const otext  *name
)
{
    CALL_IMPL(OcilibJsonNodeGetFieldByName, node, name);
}

OCI_JsonNode * OCI_API OCI_JsonNodeGetFieldByIndex
(
    OCI_JsonNode *node,
    OCI_UInt32    index
)
{
    CALL_IMPL(OcilibJsonNodeGetFieldByIndex, node, index);
}

OCI_UInt32 OCI_API OCI_JsonNodeGetItemCount
(
    OCI_JsonNode *node
)
{
    CALL_IMPL(OcilibJsonNodeGetItemCount, node);
}

OCI_JsonNode * OCI_API OCI_JsonNodeGetItem
(
    OCI_JsonNode *node,
    OCI_UInt32    index
)
{
    CALL_IMPL(OcilibJsonNodeGetItem, node, index);
}

const otext * OCI_API OCI_JsonNodeGetString
(
    OCI_JsonNode *node,
    OCI_UInt32   *length
)
{
    CALL_IMPL(OcilibJsonNodeGetString, node, length);
}

OCI_Number * OCI_API OCI_JsonNodeGetNumber
(
    OCI_JsonNode *node
)
{
    CALL_IMPL(OcilibJsonNodeGetNumber, node);
}

OCI_Boolean OCI_API OCI_JsonNodeGetBoolean
(
    OCI_JsonNode *node
)
{
    CALL_IMPL(OcilibJsonNodeGetBoolean, node);
}

const void * OCI_API OCI_JsonNodeGetBinary
(
    OCI_JsonNode *node,
    OCI_UInt32   *length
)
{
    CALL_IMPL(OcilibJsonNodeGetBinary, node, length);
}

OCI_Date * OCI_API OCI_JsonNodeGetDate
(
    OCI_JsonNode *node
)
{
    CALL_IMPL(OcilibJsonNodeGetDate, node);
}

OCI_Timestamp * OCI_API OCI_JsonNodeGetTimestamp
(
    OCI_JsonNode *node
)
{
    CALL_IMPL(OcilibJsonNodeGetTimestamp, node);
}

OCI_Interval * OCI_API OCI_JsonNodeGetInterval
(
    OCI_JsonNode *node
)
{
    CALL_IMPL(OcilibJsonNodeGetInterval, node);
}

OCI_Boolean OCI_API OCI_JsonNodeSetString
(
    OCI_JsonNode *node,
    const otext  *value,
    OCI_UInt32    length
)
{
    CALL_IMPL(OcilibJsonNodeSetString, node, value, length);
}

OCI_Boolean OCI_API OCI_JsonNodeSetNumber
(
    OCI_JsonNode *node,
    OCI_Number   *value
)
{
    CALL_IMPL(OcilibJsonNodeSetNumber, node, value);
}

OCI_Boolean OCI_API OCI_JsonNodeSetBoolean
(
    OCI_JsonNode *node,
    OCI_Boolean   value
)
{
    CALL_IMPL(OcilibJsonNodeSetBoolean, node, value);
}

OCI_Boolean OCI_API OCI_JsonNodeSetBinary
(
    OCI_JsonNode *node,
    const void   *value,
    OCI_UInt32    length
)
{
    CALL_IMPL(OcilibJsonNodeSetBinary, node, value, length);
}

OCI_Boolean OCI_API OCI_JsonNodeSetNull
(
    OCI_JsonNode *node
)
{
    CALL_IMPL(OcilibJsonNodeSetNull, node);
}

OCI_Boolean OCI_API OCI_JsonNodeSetDate
(
    OCI_JsonNode *node,
    OCI_Date     *value
)
{
    CALL_IMPL(OcilibJsonNodeSetDate, node, value);
}

OCI_Boolean OCI_API OCI_JsonNodeSetTimestamp
(
    OCI_JsonNode  *node,
    OCI_Timestamp *value
)
{
    CALL_IMPL(OcilibJsonNodeSetTimestamp, node, value);
}

OCI_Boolean OCI_API OCI_JsonNodeSetInterval
(
    OCI_JsonNode *node,
    OCI_Interval *value
)
{
    CALL_IMPL(OcilibJsonNodeSetInterval, node, value);
}

OCI_Boolean OCI_API OCI_JsonNodeSetField
(
    OCI_JsonNode *obj,
    const otext  *name,
    OCI_JsonNode *value
)
{
    CALL_IMPL(OcilibJsonNodeSetField, obj, name, value);
}

OCI_Boolean OCI_API OCI_JsonNodeSetItem
(
    OCI_JsonNode *arr,
    OCI_UInt32    index,
    OCI_JsonNode *value
)
{
    CALL_IMPL(OcilibJsonNodeSetItem, arr, index, value);
}

OCI_Boolean OCI_API OCI_JsonNodeAppendItem
(
    OCI_JsonNode *arr,
    OCI_JsonNode *value
)
{
    CALL_IMPL(OcilibJsonNodeAppendItem, arr, value);
}

OCI_Boolean OCI_API OCI_JsonNodeInsertItem
(
    OCI_JsonNode *arr,
    OCI_UInt32    index,
    OCI_JsonNode *value
)
{
    CALL_IMPL(OcilibJsonNodeInsertItem, arr, index, value);
}

OCI_Boolean OCI_API OCI_JsonNodeRemoveItem
(
    OCI_JsonNode *arr,
    OCI_UInt32    index
)
{
    CALL_IMPL(OcilibJsonNodeRemoveItem, arr, index);
}

OCI_Boolean OCI_API OCI_JsonNodeHasField
(
    OCI_JsonNode *node,
    const otext  *name
)
{
    CALL_IMPL(OcilibJsonNodeHasField, node, name);
}

OCI_UInt32 OCI_API OCI_JsonNodeRemoveItems
(
    OCI_JsonNode * arr,
    OCI_UInt32     start,
    OCI_UInt32     count
)
{
    CALL_IMPL(OcilibJsonNodeRemoveItems, arr, start, count);
}

OCI_Boolean OCI_API OCI_JsonNodeRenameField
(
    OCI_JsonNode * node,
    const otext  * old_name,
    const otext  * new_name
)
{
    CALL_IMPL(OcilibJsonNodeRenameField, node, old_name, new_name);
}

OCI_Boolean OCI_API OCI_JsonNodeEquals
(
    OCI_JsonNode * node1,
    OCI_JsonNode * node2
)
{
    CALL_IMPL(OcilibJsonNodeEquals, node1, node2);
}

OCI_Json * OCI_API OCI_JsonNodeGetJson
(
    OCI_JsonNode * node
)
{
    CALL_IMPL(OcilibJsonNodeGetJson, node);
}

OCI_Boolean OCI_API OCI_JsonNodeAssign
(
    OCI_JsonNode * node,
    OCI_JsonNode * src
)
{
    CALL_IMPL(OcilibJsonNodeAssign, node, src);
}

OCI_Boolean OCI_API OCI_JsonNodeRemoveField
(
    OCI_JsonNode *node,
    const otext  *name
)
{
    CALL_IMPL(OcilibJsonNodeRemoveField, node, name);
}

OCI_Boolean OCI_API OCI_JsonNodeClear
(
    OCI_JsonNode *node
)
{
    CALL_IMPL(OcilibJsonNodeClear, node);
}

OCI_JsonNode * OCI_API OCI_JsonNodeGetParent
(
    OCI_JsonNode *node
)
{
    CALL_IMPL(OcilibJsonNodeGetParent, node);
}

OCI_JsonNode * OCI_API OCI_JsonNodeGetByPath
(
    OCI_JsonNode *node,
    const otext  *path
)
{
    CALL_IMPL(OcilibJsonNodeGetByPath, node, path);
}

OCI_Boolean OCI_API OCI_JsonNodeGetInt16
(
    OCI_JsonNode *node,
    OCI_Int16    *value
)
{
    CALL_IMPL(OcilibJsonNodeGetInt16, node, value);
}

OCI_Boolean OCI_API OCI_JsonNodeGetUInt16
(
    OCI_JsonNode *node,
    OCI_UInt16   *value
)
{
    CALL_IMPL(OcilibJsonNodeGetUInt16, node, value);
}

OCI_Boolean OCI_API OCI_JsonNodeGetInt32
(
    OCI_JsonNode *node,
    OCI_Int32    *value
)
{
    CALL_IMPL(OcilibJsonNodeGetInt32, node, value);
}

OCI_Boolean OCI_API OCI_JsonNodeGetUInt32
(
    OCI_JsonNode *node,
    OCI_UInt32   *value
)
{
    CALL_IMPL(OcilibJsonNodeGetUInt32, node, value);
}

OCI_Boolean OCI_API OCI_JsonNodeGetInt64
(
    OCI_JsonNode *node,
    OCI_Int64    *value
)
{
    CALL_IMPL(OcilibJsonNodeGetInt64, node, value);
}

OCI_Boolean OCI_API OCI_JsonNodeGetUInt64
(
    OCI_JsonNode *node,
    OCI_UInt64   *value
)
{
    CALL_IMPL(OcilibJsonNodeGetUInt64, node, value);
}

OCI_Boolean OCI_API OCI_JsonNodeGetFloat
(
    OCI_JsonNode *node,
    OCI_Float    *value
)
{
    CALL_IMPL(OcilibJsonNodeGetFloat, node, value);
}

OCI_Boolean OCI_API OCI_JsonNodeGetDouble
(
    OCI_JsonNode *node,
    OCI_Double   *value
)
{
    CALL_IMPL(OcilibJsonNodeGetDouble, node, value);
}

OCI_Boolean OCI_API OCI_JsonNodeSetInt16
(
    OCI_JsonNode *node,
    OCI_Int16     value
)
{
    CALL_IMPL(OcilibJsonNodeSetInt16, node, value);
}

OCI_Boolean OCI_API OCI_JsonNodeSetUInt16
(
    OCI_JsonNode *node,
    OCI_UInt16    value
)
{
    CALL_IMPL(OcilibJsonNodeSetUInt16, node, value);
}

OCI_Boolean OCI_API OCI_JsonNodeSetInt32
(
    OCI_JsonNode *node,
    OCI_Int32     value
)
{
    CALL_IMPL(OcilibJsonNodeSetInt32, node, value);
}

OCI_Boolean OCI_API OCI_JsonNodeSetUInt32
(
    OCI_JsonNode *node,
    OCI_UInt32    value
)
{
    CALL_IMPL(OcilibJsonNodeSetUInt32, node, value);
}

OCI_Boolean OCI_API OCI_JsonNodeSetInt64
(
    OCI_JsonNode *node,
    OCI_Int64     value
)
{
    CALL_IMPL(OcilibJsonNodeSetInt64, node, value);
}

OCI_Boolean OCI_API OCI_JsonNodeSetUInt64
(
    OCI_JsonNode *node,
    OCI_UInt64    value
)
{
    CALL_IMPL(OcilibJsonNodeSetUInt64, node, value);
}

OCI_Boolean OCI_API OCI_JsonNodeSetFloat
(
    OCI_JsonNode *node,
    OCI_Float     value
)
{
    CALL_IMPL(OcilibJsonNodeSetFloat, node, value);
}

OCI_Boolean OCI_API OCI_JsonNodeSetDouble
(
    OCI_JsonNode *node,
    OCI_Double    value
)
{
    CALL_IMPL(OcilibJsonNodeSetDouble, node, value);
}
