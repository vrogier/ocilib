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

#ifndef OCILIB_STATEMENT_H_INCLUDED
#define OCILIB_STATEMENT_H_INCLUDED

#include "types.h"

OCI_SYM_LOCAL OCI_Boolean OcilibStatementFetchIntoUserVariables
(
    OCI_Statement* stmt,
    va_list        args
);

OCI_SYM_LOCAL OCI_Boolean OcilibStatementDispose
(
    OCI_Statement* stmt
);

OCI_SYM_LOCAL OCI_Statement * OcilibStatementInitialize
(
    OCI_Connection* con,
    OCI_Statement * stmt,
    OCIStmt       * handle,
    OCI_Boolean     is_desc,
    const otext   * sql
);

OCI_SYM_LOCAL OCI_Boolean OcilibStatementPrepareInternal
(
    OCI_Statement* stmt,
    const otext  * sql
);

OCI_SYM_LOCAL OCI_Boolean OcilibStatementExecuteInternal
(
    OCI_Statement* stmt,
    ub4            mode
);

OCI_SYM_LOCAL OCI_Statement * OcilibStatementCreate
(
    OCI_Connection* con
);

OCI_SYM_LOCAL OCI_Boolean OcilibStatementFree
(
    OCI_Statement* stmt
);

OCI_SYM_LOCAL OCI_Resultset * OcilibStatementGetResultset
(
    OCI_Statement* stmt
);

OCI_SYM_LOCAL OCI_Resultset * OcilibStatementGetNextResultset
(
    OCI_Statement* stmt
);

OCI_SYM_LOCAL OCI_Boolean OcilibStatementReleaseResultsets
(
    OCI_Statement* stmt
);

OCI_SYM_LOCAL OCI_Boolean OcilibStatementPrepare
(
    OCI_Statement* stmt,
    const otext  * sql
);

OCI_SYM_LOCAL OCI_Boolean OcilibStatementExecutePrepared
(
    OCI_Statement* stmt
);

OCI_SYM_LOCAL OCI_Boolean OcilibStatementExecute
(
    OCI_Statement* stmt,
    const otext  * sql
);

OCI_SYM_LOCAL OCI_Resultset* OcilibStatementExecutePreparedAndGetResultset
(
    OCI_Statement* stmt
);

OCI_SYM_LOCAL OCI_Resultset* OcilibStatementExecuteAndGetResultset
(
    OCI_Statement* stmt,
    const otext  * sql
);

OCI_SYM_LOCAL OCI_Boolean OcilibStatementParse
(
    OCI_Statement* stmt,
    const otext  * sql
);

OCI_SYM_LOCAL OCI_Boolean OcilibStatementDescribe
(
    OCI_Statement* stmt,
    const otext  * sql
);

OCI_SYM_LOCAL OCI_Boolean OcilibStatementPrepareFormat
(
    OCI_Statement* stmt,
    const otext  * sql,
    va_list        args
);

OCI_SYM_LOCAL OCI_Boolean OcilibStatementExecuteFormat
(
    OCI_Statement* stmt,
    const otext  * sql,
    va_list        args
);

OCI_SYM_LOCAL OCI_Boolean OcilibStatementParseFormat
(
    OCI_Statement* stmt,
    const otext  * sql,
    va_list        args
);

OCI_SYM_LOCAL OCI_Boolean OcilibStatementDescribeFormat
(
    OCI_Statement* stmt,
    const otext  * sql,
    va_list        args
);

OCI_SYM_LOCAL OCI_Boolean OcilibStatementBindArraySetSize
(
    OCI_Statement* stmt,
    OCI_UInt32     size
);

OCI_SYM_LOCAL OCI_UInt32 OcilibStatementBindArrayGetSize
(
    OCI_Statement* stmt
);

OCI_SYM_LOCAL OCI_Boolean OcilibStatementAllowRebinding
(
    OCI_Statement* stmt,
    OCI_Boolean    value
);

OCI_SYM_LOCAL OCI_Boolean OcilibStatementIsRebindingAllowed
(
    OCI_Statement* stmt
);

OCI_SYM_LOCAL OCI_Bind* OcilibStatementBindBoolean
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Boolean  * data
);

OCI_SYM_LOCAL OCI_Bind* OcilibStatementBindNumber
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Number   * data
);

OCI_SYM_LOCAL OCI_Bind* OcilibStatementBindNumberArray
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Number  ** data,
    OCI_UInt32     nbelem
);

OCI_SYM_LOCAL OCI_Bind* OcilibStatementBindInt16
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Int16    * data
);

OCI_SYM_LOCAL OCI_Bind* OcilibStatementBindInt16Array
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Int16    * data,
    OCI_UInt32     nbelem
);

OCI_SYM_LOCAL OCI_Bind* OcilibStatementBindUInt16
(
    OCI_Statement * stmt,
    const otext   * name,
    OCI_UInt16    * data
);

OCI_SYM_LOCAL OCI_Bind* OcilibStatementBindUInt16Array
(
    OCI_Statement * stmt,
    const otext   * name,
    OCI_UInt16    * data,
    OCI_UInt32      nbelem
);

OCI_SYM_LOCAL OCI_Bind* OcilibStatementBindInt32
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Int32    * data
);

OCI_SYM_LOCAL OCI_Bind* OcilibStatementBindInt32Array
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Int32    * data,
    OCI_UInt32     nbelem
);

OCI_SYM_LOCAL OCI_Bind* OcilibStatementBindUInt32
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_UInt32   * data
);

OCI_SYM_LOCAL OCI_Bind* OcilibStatementBindUInt32Array
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_UInt32   * data,
    OCI_UInt32     nbelem
);

OCI_SYM_LOCAL OCI_Bind* OcilibStatementBindInt64
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Int64    * data
);

OCI_SYM_LOCAL OCI_Bind* OcilibStatementBindInt64Array
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Int64    * data,
    OCI_UInt32     nbelem
);

OCI_SYM_LOCAL OCI_Bind* OcilibStatementBindUInt64
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_UInt64   * data
);

OCI_SYM_LOCAL OCI_Bind* OcilibStatementBindUInt64Array
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_UInt64   * data,
    OCI_UInt32     nbelem
);

OCI_SYM_LOCAL OCI_Bind* OcilibStatementBindString
(
    OCI_Statement* stmt,
    const otext  * name,
    otext        * data,
    OCI_UInt32     len
);

OCI_SYM_LOCAL OCI_Bind* OcilibStatementBindStringArray
(
    OCI_Statement* stmt,
    const otext  * name,
    otext        * data,
    OCI_UInt32     len,
    OCI_UInt32     nbelem
);

OCI_SYM_LOCAL OCI_Bind* OcilibStatementBindRaw
(
    OCI_Statement* stmt,
    const otext  * name,
    void         * data,
    OCI_UInt32     len
);

OCI_SYM_LOCAL OCI_Bind* OcilibStatementBindRawArray
(
    OCI_Statement* stmt,
    const otext  * name,
    void         * data,
    OCI_UInt32     len,
    OCI_UInt32     nbelem
);

OCI_SYM_LOCAL OCI_Bind* OcilibStatementBindDouble
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Double   * data
);

OCI_SYM_LOCAL OCI_Bind* OcilibStatementBindDoubleArray
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Double   * data,
    OCI_UInt32     nbelem
);

OCI_SYM_LOCAL OCI_Bind* OcilibStatementBindFloat
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Float    * data
);

OCI_SYM_LOCAL OCI_Bind* OcilibStatementBindFloatArray
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Float    * data,
    OCI_UInt32     nbelem
);

OCI_SYM_LOCAL OCI_Bind* OcilibStatementBindDate
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Date     * data
);

OCI_SYM_LOCAL OCI_Bind* OcilibStatementBindDateArray
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Date    ** data,
    OCI_UInt32     nbelem
);

OCI_SYM_LOCAL OCI_Bind* OcilibStatementBindTimestamp
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Timestamp* data
);

OCI_SYM_LOCAL OCI_Bind* OcilibStatementBindTimestampArray
(
    OCI_Statement   * stmt,
    const otext     * name,
    OCI_Timestamp  ** data,
    OCI_TimestampType type,
    OCI_UInt32        nbelem
);

OCI_SYM_LOCAL OCI_Bind* OcilibStatementBindInterval
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Interval * data
);

OCI_SYM_LOCAL OCI_Bind* OcilibStatementBindIntervalArray
(
    OCI_Statement  * stmt,
    const otext    * name,
    OCI_Interval  ** data,
    OCI_IntervalType type,
    OCI_UInt32       nbelem
);

OCI_SYM_LOCAL OCI_Bind* OcilibStatementBindObject
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Object   * data
);

OCI_SYM_LOCAL OCI_Bind* OcilibStatementBindObjectArray
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Object  ** data,
    OCI_TypeInfo * typinf,
    OCI_UInt32     nbelem
);

OCI_SYM_LOCAL OCI_Bind* OcilibStatementBindVector
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Vector   * data
);

OCI_SYM_LOCAL OCI_Bind* OcilibStatementBindVectorArray
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Vector  ** data,
    OCI_UInt32     nbelem
);

OCI_SYM_LOCAL OCI_Bind* OcilibStatementBindLob
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Lob      * data
);

OCI_SYM_LOCAL OCI_Bind* OcilibStatementBindJson
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Json     * data
);

OCI_SYM_LOCAL OCI_Bind* OcilibStatementBindJsonArray
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Json    ** data,
    OCI_UInt32     nbelem
);

OCI_SYM_LOCAL OCI_Bind* OcilibStatementBindLobArray
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Lob     ** data,
    OCI_LobType    type,
    OCI_UInt32     nbelem
);

OCI_SYM_LOCAL OCI_Bind* OcilibStatementBindFile
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_File     * data
);

OCI_SYM_LOCAL OCI_Bind* OcilibStatementBindFileArray
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_File    ** data,
    OCI_FileType   type,
    OCI_UInt32     nbelem
);

OCI_SYM_LOCAL OCI_Bind* OcilibStatementBindReference
(
    OCI_Statement * stmt,
    const otext   * name,
    OCI_Reference * data
);

OCI_SYM_LOCAL OCI_Bind* OcilibStatementBindReferenceArray
(
    OCI_Statement  * stmt,
    const otext    * name,
    OCI_Reference ** data,
    OCI_TypeInfo   * typinf,
    OCI_UInt32       nbelem
);

OCI_SYM_LOCAL OCI_Bind* OcilibStatementBindCollection
(
    OCI_Statement  * stmt,
    const otext    * name,
    OCI_Collection * data
);

OCI_SYM_LOCAL OCI_Bind* OcilibStatementBindCollectionArray
(
    OCI_Statement   * stmt,
    const otext     * name,
    OCI_Collection ** data,
    OCI_TypeInfo    * typinf,
    OCI_UInt32        nbelem
);

OCI_SYM_LOCAL OCI_Bind* OcilibStatementBindCursor
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Statement* data
);

OCI_SYM_LOCAL OCI_Bind* OcilibStatementBindLong
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Long     * data,
    OCI_UInt32     size
);

OCI_SYM_LOCAL OCI_Boolean OcilibStatementRegisterNumber
(
    OCI_Statement* stmt,
    const otext  * name
);

OCI_SYM_LOCAL OCI_Boolean OcilibStatementRegisterInt16
(
    OCI_Statement* stmt,
    const otext  * name
);

OCI_SYM_LOCAL OCI_Boolean OcilibStatementRegisterUInt16
(
    OCI_Statement* stmt,
    const otext  * name
);

OCI_SYM_LOCAL OCI_Boolean OcilibStatementRegisterInt32
(
    OCI_Statement* stmt,
    const otext  * name
);

OCI_SYM_LOCAL OCI_Boolean OcilibStatementRegisterUInt32
(
    OCI_Statement* stmt,
    const otext  * name
);

OCI_SYM_LOCAL OCI_Boolean OcilibStatementRegisterInt64
(
    OCI_Statement* stmt,
    const otext  * name
);

OCI_SYM_LOCAL OCI_Boolean OcilibStatementRegisterUInt64
(
    OCI_Statement* stmt,
    const otext  * name
);

OCI_SYM_LOCAL OCI_Boolean OcilibStatementRegisterString
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_UInt32     len
);

OCI_SYM_LOCAL OCI_Boolean OcilibStatementRegisterRaw
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_UInt32     len
);

OCI_SYM_LOCAL OCI_Boolean OcilibStatementRegisterDouble
(
    OCI_Statement* stmt,
    const otext  * name
);

OCI_SYM_LOCAL OCI_Boolean OcilibStatementRegisterFloat
(
    OCI_Statement* stmt,
    const otext  * name
);

OCI_SYM_LOCAL OCI_Boolean OcilibStatementRegisterDate
(
    OCI_Statement* stmt,
    const otext  * name
);

OCI_SYM_LOCAL OCI_Boolean OcilibStatementRegisterTimestamp
(
    OCI_Statement   * stmt,
    const otext     * name,
    OCI_TimestampType type
);

OCI_SYM_LOCAL OCI_Boolean OcilibStatementRegisterInterval
(
    OCI_Statement  * stmt,
    const otext    * name,
    OCI_IntervalType type
);

OCI_SYM_LOCAL OCI_Boolean OcilibStatementRegisterObject
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_TypeInfo * typinf
);

OCI_SYM_LOCAL OCI_Boolean OcilibStatementRegisterLob
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_LobType    type
);

OCI_SYM_LOCAL OCI_Boolean OcilibStatementRegisterFile
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_FileType   type
);

OCI_SYM_LOCAL OCI_Boolean OcilibStatementRegisterReference
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_TypeInfo * typinf
);

OCI_SYM_LOCAL OCI_Boolean OcilibStatementRegisterVector
(
    OCI_Statement* stmt,
    const otext  * name
);

OCI_SYM_LOCAL OCI_Boolean OcilibStatementRegisterJson
(
    OCI_Statement* stmt,
    const otext  * name
);

OCI_SYM_LOCAL OCI_StatementType OcilibStatementGetType
(
    OCI_Statement* stmt
);

OCI_SYM_LOCAL OCI_Boolean OcilibStatementSetFetchMode
(
    OCI_Statement* stmt,
    OCI_FetchMode  mode
);

OCI_SYM_LOCAL OCI_FetchMode OcilibStatementGetFetchMode
(
    OCI_Statement* stmt
);

OCI_SYM_LOCAL OCI_Boolean OcilibStatementSetBindMode
(
    OCI_Statement* stmt,
    OCI_BindMode   mode
);

OCI_SYM_LOCAL OCI_BindMode OcilibStatementGetBindMode
(
    OCI_Statement* stmt
);

OCI_SYM_LOCAL OCI_Boolean OcilibStatementSetBindAllocation
(
    OCI_Statement        * stmt,
    OCI_BindAllocationMode mode
);

OCI_SYM_LOCAL OCI_BindAllocationMode OcilibStatementGetBindAllocation
(
    OCI_Statement* stmt
);

OCI_SYM_LOCAL OCI_Boolean OcilibStatementSetFetchSize
(
    OCI_Statement* stmt,
    OCI_UInt32     size
);

OCI_SYM_LOCAL OCI_UInt32 OcilibStatementGetFetchSize
(
    OCI_Statement* stmt
);

OCI_SYM_LOCAL OCI_Boolean OcilibStatementSetPrefetchSize
(
    OCI_Statement* stmt,
    OCI_UInt32     size
);

OCI_SYM_LOCAL OCI_UInt32 OcilibStatementGetPrefetchSize
(
    OCI_Statement* stmt
);

OCI_SYM_LOCAL OCI_Boolean OcilibStatementSetPrefetchMemory
(
    OCI_Statement* stmt,
    OCI_UInt32     size
);

OCI_SYM_LOCAL OCI_UInt32 OcilibStatementGetPrefetchMemory
(
    OCI_Statement* stmt
);

OCI_SYM_LOCAL OCI_Boolean OcilibStatementSetPieceSize
(
    OCI_Statement* stmt,
    OCI_UInt32     size
);

OCI_SYM_LOCAL OCI_UInt32 OcilibStatementGetPieceSize
(
    OCI_Statement* stmt
);

OCI_SYM_LOCAL OCI_Boolean OcilibStatementSetLongMode
(
    OCI_Statement* stmt,
    OCI_LongMode   mode
);

OCI_SYM_LOCAL OCI_LongMode OcilibStatementGetLongMode
(
    OCI_Statement* stmt
);

OCI_SYM_LOCAL OCI_Connection * OcilibStatementGetConnection
(
    OCI_Statement* stmt
);

OCI_SYM_LOCAL const otext * OcilibStatementGetSql
(
    OCI_Statement* stmt
);

OCI_SYM_LOCAL const otext * OcilibStatementGetSqlIdentifier
(
    OCI_Statement* stmt
);

OCI_SYM_LOCAL OCI_UInt32 OcilibStatementGetSqlErrorPos
(
    OCI_Statement* stmt
);

OCI_SYM_LOCAL OCI_UInt32 OcilibStatementGetAffectedRows
(
    OCI_Statement* stmt
);

OCI_SYM_LOCAL OCI_UInt32 OcilibStatementGetBindCount
(
    OCI_Statement* stmt
);

OCI_SYM_LOCAL OCI_Bind * OcilibStatementGetBind
(
    OCI_Statement* stmt,
    OCI_UInt32     index
);

OCI_SYM_LOCAL OCI_Bind * OcilibStatementGetBindByName
(
    OCI_Statement* stmt,
    const otext  * name
);

OCI_SYM_LOCAL OCI_Int32 OcilibStatementGetBindIndex
(
    OCI_Statement* stmt,
    const otext  * name
);

OCI_SYM_LOCAL OCI_SQLCommand OcilibStatementGetSqlCommand
(
    OCI_Statement* stmt
);

OCI_SYM_LOCAL const otext * OcilibStatementGetSqlVerb
(
    OCI_Statement* stmt
);

OCI_SYM_LOCAL OCI_Boolean OcilibStatementGetParseBindNames
(
    OCI_Statement* stmt,
    OCI_UInt32   * count,
    const otext*** names
);

OCI_SYM_LOCAL OCI_Error * OcilibStatementGetBatchError
(
    OCI_Statement* stmt
);

OCI_SYM_LOCAL OCI_UInt32 OcilibStatementGetBatchErrorCount
(
    OCI_Statement* stmt
);

#endif /* OCILIB_STATEMENT_H_INCLUDED */
