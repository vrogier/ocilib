/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2023 Vincent ROGIER <vince.rogier@ocilib.net>
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

OCI_SYM_LOCAL boolean OcilibStatementFetchIntoUserVariables
(
    OCI_Statement* stmt,
    va_list        args
);

OCI_SYM_LOCAL boolean OcilibStatementDispose
(
    OCI_Statement* stmt
);

OCI_SYM_LOCAL OCI_Statement * OcilibStatementInitialize
(
    OCI_Connection* con,
    OCI_Statement * stmt,
    OCIStmt       * handle,
    boolean         is_desc,
    const otext   * sql
);

OCI_SYM_LOCAL boolean OcilibStatementPrepareInternal
(
    OCI_Statement* stmt,
    const otext  * sql
);

OCI_SYM_LOCAL boolean OcilibStatementExecuteInternal
(
    OCI_Statement* stmt,
    ub4            mode
);

OCI_SYM_LOCAL OCI_Statement * OcilibStatementCreate
(
    OCI_Connection* con
);

OCI_SYM_LOCAL boolean OcilibStatementFree
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

OCI_SYM_LOCAL boolean OcilibStatementReleaseResultsets
(
    OCI_Statement* stmt
);

OCI_SYM_LOCAL boolean OcilibStatementPrepare
(
    OCI_Statement* stmt,
    const otext  * sql
);

OCI_SYM_LOCAL boolean OcilibStatementExecute
(
    OCI_Statement* stmt
);

OCI_SYM_LOCAL boolean OcilibStatementExecuteStmt
(
    OCI_Statement* stmt,
    const otext  * sql
);

OCI_SYM_LOCAL boolean OcilibStatementParse
(
    OCI_Statement* stmt,
    const otext  * sql
);

OCI_SYM_LOCAL boolean OcilibStatementDescribe
(
    OCI_Statement* stmt,
    const otext  * sql
);

OCI_SYM_LOCAL boolean OcilibStatementPrepareFmt
(
    OCI_Statement* stmt,
    const otext  * sql,
    va_list        args
);

OCI_SYM_LOCAL boolean OcilibStatementExecuteStmtFmt
(
    OCI_Statement* stmt,
    const otext  * sql,
    va_list        args
);

OCI_SYM_LOCAL boolean OcilibStatementParseFmt
(
    OCI_Statement* stmt,
    const otext  * sql,
    va_list        args
);

OCI_SYM_LOCAL boolean OcilibStatementDescribeFmt
(
    OCI_Statement* stmt,
    const otext  * sql,
    va_list        args
);

OCI_SYM_LOCAL boolean OcilibStatementSetBindArraySize
(
    OCI_Statement* stmt,
    unsigned int   size
);

OCI_SYM_LOCAL unsigned int OcilibStatementGetBindArraySize
(
    OCI_Statement* stmt
);

OCI_SYM_LOCAL boolean OcilibStatementAllowRebinding
(
    OCI_Statement* stmt,
    boolean        value
);

OCI_SYM_LOCAL boolean OcilibStatementIsRebindingAllowed
(
    OCI_Statement* stmt
);

OCI_SYM_LOCAL boolean OcilibStatementBindBoolean
(
    OCI_Statement* stmt,
    const otext  * name,
    boolean      * data
);

OCI_SYM_LOCAL boolean OcilibStatementBindNumber
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Number   * data
);

OCI_SYM_LOCAL boolean OcilibStatementBindArrayOfNumbers
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Number  ** data,
    unsigned int   nbelem
);

OCI_SYM_LOCAL boolean OcilibStatementBindShort
(
    OCI_Statement* stmt,
    const otext  * name,
    short        * data
);

OCI_SYM_LOCAL boolean OcilibStatementBindArrayOfShorts
(
    OCI_Statement* stmt,
    const otext  * name,
    short        * data,
    unsigned int   nbelem
);

OCI_SYM_LOCAL boolean OcilibStatementBindUnsignedShort
(
    OCI_Statement * stmt,
    const otext   * name,
    unsigned short* data
);

OCI_SYM_LOCAL boolean OcilibStatementBindArrayOfUnsignedShorts
(
    OCI_Statement * stmt,
    const otext   * name,
    unsigned short* data,
    unsigned int    nbelem
);

OCI_SYM_LOCAL boolean OcilibStatementBindInt
(
    OCI_Statement* stmt,
    const otext  * name,
    int          * data
);

OCI_SYM_LOCAL boolean OcilibStatementBindArrayOfInts
(
    OCI_Statement* stmt,
    const otext  * name,
    int          * data,
    unsigned int   nbelem
);

OCI_SYM_LOCAL boolean OcilibStatementBindUnsignedInt
(
    OCI_Statement* stmt,
    const otext  * name,
    unsigned int * data
);

OCI_SYM_LOCAL boolean OcilibStatementBindArrayOfUnsignedInts
(
    OCI_Statement* stmt,
    const otext  * name,
    unsigned int * data,
    unsigned int   nbelem
);

OCI_SYM_LOCAL boolean OcilibStatementBindBigInt
(
    OCI_Statement* stmt,
    const otext  * name,
    big_int      * data
);

OCI_SYM_LOCAL boolean OcilibStatementBindArrayOfBigInts
(
    OCI_Statement* stmt,
    const otext  * name,
    big_int      * data,
    unsigned int   nbelem
);

OCI_SYM_LOCAL boolean OcilibStatementBindUnsignedBigInt
(
    OCI_Statement* stmt,
    const otext  * name,
    big_uint     * data
);

OCI_SYM_LOCAL boolean OcilibStatementBindArrayOfUnsignedBigInts
(
    OCI_Statement* stmt,
    const otext  * name,
    big_uint     * data,
    unsigned int   nbelem
);

OCI_SYM_LOCAL boolean OcilibStatementBindString
(
    OCI_Statement* stmt,
    const otext  * name,
    otext        * data,
    unsigned int   len
);

OCI_SYM_LOCAL boolean OcilibStatementBindArrayOfStrings
(
    OCI_Statement* stmt,
    const otext  * name,
    otext        * data,
    unsigned int   len,
    unsigned int   nbelem
);

OCI_SYM_LOCAL boolean OcilibStatementBindRaw
(
    OCI_Statement* stmt,
    const otext  * name,
    void         * data,
    unsigned int   len
);

OCI_SYM_LOCAL boolean OcilibStatementBindArrayOfRaws
(
    OCI_Statement* stmt,
    const otext  * name,
    void         * data,
    unsigned int   len,
    unsigned int   nbelem
);

OCI_SYM_LOCAL boolean OcilibStatementBindDouble
(
    OCI_Statement* stmt,
    const otext  * name,
    double       * data
);

OCI_SYM_LOCAL boolean OcilibStatementBindArrayOfDoubles
(
    OCI_Statement* stmt,
    const otext  * name,
    double       * data,
    unsigned int   nbelem
);

OCI_SYM_LOCAL boolean OcilibStatementBindFloat
(
    OCI_Statement* stmt,
    const otext  * name,
    float        * data
);

OCI_SYM_LOCAL boolean OcilibStatementBindArrayOfFloats
(
    OCI_Statement* stmt,
    const otext  * name,
    float        * data,
    unsigned int   nbelem
);

OCI_SYM_LOCAL boolean OcilibStatementBindDate
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Date     * data
);

OCI_SYM_LOCAL boolean OcilibStatementBindArrayOfDates
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Date    ** data,
    unsigned int   nbelem
);

OCI_SYM_LOCAL boolean OcilibStatementBindTimestamp
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Timestamp* data
);

OCI_SYM_LOCAL boolean OcilibStatementBindArrayOfTimestamps
(
    OCI_Statement * stmt,
    const otext   * name,
    OCI_Timestamp** data,
    unsigned int    type,
    unsigned int    nbelem
);

OCI_SYM_LOCAL boolean OcilibStatementBindInterval
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Interval * data
);

OCI_SYM_LOCAL boolean OcilibStatementBindArrayOfIntervals
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Interval** data,
    unsigned int   type,
    unsigned int   nbelem
);

OCI_SYM_LOCAL boolean OcilibStatementBindObject
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Object   * data
);

OCI_SYM_LOCAL boolean OcilibStatementBindArrayOfObjects
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Object  ** data,
    OCI_TypeInfo * typinf,
    unsigned int   nbelem
);

OCI_SYM_LOCAL boolean OcilibStatementBindLob
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Lob      * data
);

OCI_SYM_LOCAL boolean OcilibStatementBindArrayOfLobs
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Lob     ** data,
    unsigned int   type,
    unsigned int   nbelem
);

OCI_SYM_LOCAL boolean OcilibStatementBindFile
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_File     * data
);

OCI_SYM_LOCAL boolean OcilibStatementBindArrayOfFiles
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_File    ** data,
    unsigned int   type,
    unsigned int   nbelem
);

OCI_SYM_LOCAL boolean OcilibStatementBindReference
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Ref      * data
);

OCI_SYM_LOCAL boolean OcilibStatementBindArrayOfReferences
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Ref     ** data,
    OCI_TypeInfo * typinf,
    unsigned int   nbelem
);

OCI_SYM_LOCAL boolean OcilibStatementBindCollection
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Coll     * data
);

OCI_SYM_LOCAL boolean OcilibStatementBindArrayOfCollections
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Coll    ** data,
    OCI_TypeInfo * typinf,
    unsigned int   nbelem
);

OCI_SYM_LOCAL boolean OcilibStatementBindStatement
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Statement* data
);

OCI_SYM_LOCAL boolean OcilibStatementBindLong
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Long     * data,
    unsigned int   size
);

OCI_SYM_LOCAL boolean OcilibStatementRegisterNumber
(
    OCI_Statement* stmt,
    const otext  * name
);

OCI_SYM_LOCAL boolean OcilibStatementRegisterShort
(
    OCI_Statement* stmt,
    const otext  * name
);

OCI_SYM_LOCAL boolean OcilibStatementRegisterUnsignedShort
(
    OCI_Statement* stmt,
    const otext  * name
);

OCI_SYM_LOCAL boolean OcilibStatementRegisterInt
(
    OCI_Statement* stmt,
    const otext  * name
);

OCI_SYM_LOCAL boolean OcilibStatementRegisterUnsignedInt
(
    OCI_Statement* stmt,
    const otext  * name
);

OCI_SYM_LOCAL boolean OcilibStatementRegisterBigInt
(
    OCI_Statement* stmt,
    const otext  * name
);

OCI_SYM_LOCAL boolean OcilibStatementRegisterUnsignedBigInt
(
    OCI_Statement* stmt,
    const otext  * name
);

OCI_SYM_LOCAL boolean OcilibStatementRegisterString
(
    OCI_Statement* stmt,
    const otext  * name,
    unsigned int   len
);

OCI_SYM_LOCAL boolean OcilibStatementRegisterRaw
(
    OCI_Statement* stmt,
    const otext  * name,
    unsigned int   len
);

OCI_SYM_LOCAL boolean OcilibStatementRegisterDouble
(
    OCI_Statement* stmt,
    const otext  * name
);

OCI_SYM_LOCAL boolean OcilibStatementRegisterFloat
(
    OCI_Statement* stmt,
    const otext  * name
);

OCI_SYM_LOCAL boolean OcilibStatementRegisterDate
(
    OCI_Statement* stmt,
    const otext  * name
);

OCI_SYM_LOCAL boolean OcilibStatementRegisterTimestamp
(
    OCI_Statement* stmt,
    const otext  * name,
    unsigned int   type
);

OCI_SYM_LOCAL boolean OcilibStatementRegisterInterval
(
    OCI_Statement* stmt,
    const otext  * name,
    unsigned int   type
);

OCI_SYM_LOCAL boolean OcilibStatementRegisterObject
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_TypeInfo * typinf
);

OCI_SYM_LOCAL boolean OcilibStatementRegisterLob
(
    OCI_Statement* stmt,
    const otext  * name,
    unsigned int   type
);

OCI_SYM_LOCAL boolean OcilibStatementRegisterFile
(
    OCI_Statement* stmt,
    const otext  * name,
    unsigned int   type
);

OCI_SYM_LOCAL boolean OcilibStatementRegisterReference
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_TypeInfo * typinf
);

OCI_SYM_LOCAL unsigned int OcilibStatementGetStatementType
(
    OCI_Statement* stmt
);

OCI_SYM_LOCAL boolean OcilibStatementSetFetchMode
(
    OCI_Statement* stmt,
    unsigned int   mode
);

OCI_SYM_LOCAL unsigned int OcilibStatementGetFetchMode
(
    OCI_Statement* stmt
);

OCI_SYM_LOCAL boolean OcilibStatementSetBindMode
(
    OCI_Statement* stmt,
    unsigned int   mode
);

OCI_SYM_LOCAL unsigned int OcilibStatementGetBindMode
(
    OCI_Statement* stmt
);

OCI_SYM_LOCAL boolean OcilibStatementSetBindAllocation
(
    OCI_Statement* stmt,
    unsigned int   mode
);

OCI_SYM_LOCAL unsigned int OcilibStatementGetBindAllocation
(
    OCI_Statement* stmt
);

OCI_SYM_LOCAL boolean OcilibStatementSetFetchSize
(
    OCI_Statement* stmt,
    unsigned int   size
);

OCI_SYM_LOCAL unsigned int OcilibStatementGetFetchSize
(
    OCI_Statement* stmt
);

OCI_SYM_LOCAL boolean OcilibStatementSetPrefetchSize
(
    OCI_Statement* stmt,
    unsigned int   size
);

OCI_SYM_LOCAL unsigned int OcilibStatementGetPrefetchSize
(
    OCI_Statement* stmt
);

OCI_SYM_LOCAL boolean OcilibStatementSetPrefetchMemory
(
    OCI_Statement* stmt,
    unsigned int   size
);

OCI_SYM_LOCAL unsigned int OcilibStatementGetPrefetchMemory
(
    OCI_Statement* stmt
);

OCI_SYM_LOCAL boolean OcilibStatementSetPieceSize
(
    OCI_Statement* stmt,
    unsigned int   size
);

OCI_SYM_LOCAL unsigned int OcilibStatementGetPieceSize
(
    OCI_Statement* stmt
);

OCI_SYM_LOCAL boolean OcilibStatementSetLongMode
(
    OCI_Statement* stmt,
    unsigned int   mode
);

OCI_SYM_LOCAL unsigned int OcilibStatementGetLongMode
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

OCI_SYM_LOCAL unsigned int OcilibStatementGetSqlErrorPos
(
    OCI_Statement* stmt
);

OCI_SYM_LOCAL unsigned int OcilibStatementGetAffectedRows
(
    OCI_Statement* stmt
);

OCI_SYM_LOCAL unsigned int OcilibStatementGetBindCount
(
    OCI_Statement* stmt
);

OCI_SYM_LOCAL OCI_Bind * OcilibStatementGetBind
(
    OCI_Statement* stmt,
    unsigned int   index
);

OCI_SYM_LOCAL OCI_Bind * OcilibStatementGetBind2
(
    OCI_Statement* stmt,
    const otext  * name
);

OCI_SYM_LOCAL unsigned int OcilibStatementGetBindIndex
(
    OCI_Statement* stmt,
    const otext  * name
);

OCI_SYM_LOCAL unsigned int OcilibStatementGetSqlCommand
(
    OCI_Statement* stmt
);

OCI_SYM_LOCAL const otext * OcilibStatementGetSqlVerb
(
    OCI_Statement* stmt
);

OCI_SYM_LOCAL OCI_Error * OcilibStatementGetBatchError
(
    OCI_Statement* stmt
);

OCI_SYM_LOCAL unsigned int OcilibStatementGetBatchErrorCount
(
    OCI_Statement* stmt
);

#endif /* OCILIB_STATEMENT_H_INCLUDED */
