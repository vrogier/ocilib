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

#ifndef OCILIB_STATEMENT_H_INCLUDED
#define OCILIB_STATEMENT_H_INCLUDED

#include "types.h"

boolean OcilibStatementFetchIntoUserVariables
(
    OCI_Statement* stmt,
    va_list        args
);

boolean OcilibStatementDispose
(
    OCI_Statement* stmt
);

OCI_Statement* OcilibStatementInitialize
(
    OCI_Connection* con,
    OCI_Statement * stmt,
    OCIStmt       * handle,
    boolean         is_desc,
    const otext   * sql
);

boolean OcilibStatementPrepareInternal
(
    OCI_Statement* stmt,
    const otext  * sql
);

boolean OcilibStatementExecuteInternal
(
    OCI_Statement* stmt,
    ub4            mode
);

OCI_Statement* OcilibStatementCreate
(
    OCI_Connection* con
);

boolean OcilibStatementFree
(
    OCI_Statement* stmt
);

OCI_Resultset* OcilibStatementGetResultset
(
    OCI_Statement* stmt
);

OCI_Resultset* OcilibStatementGetNextResultset
(
    OCI_Statement* stmt
);

boolean OcilibStatementReleaseResultsets
(
    OCI_Statement* stmt
);

boolean OcilibStatementPrepare
(
    OCI_Statement* stmt,
    const otext  * sql
);

boolean OcilibStatementExecute
(
    OCI_Statement* stmt
);

boolean OcilibStatementExecuteStmt
(
    OCI_Statement* stmt,
    const otext  * sql
);

boolean OcilibStatementParse
(
    OCI_Statement* stmt,
    const otext  * sql
);

boolean OcilibStatementDescribe
(
    OCI_Statement* stmt,
    const otext  * sql
);

boolean OcilibStatementPrepareFmt
(
    OCI_Statement* stmt,
    const otext  * sql,
    va_list        args
);

boolean OcilibStatementExecuteStmtFmt
(
    OCI_Statement* stmt,
    const otext  * sql,
    va_list        args
);

boolean OcilibStatementParseFmt
(
    OCI_Statement* stmt,
    const otext  * sql,
    va_list        args
);

boolean OcilibStatementDescribeFmt
(
    OCI_Statement* stmt,
    const otext  * sql,
    va_list        args
);

boolean OcilibStatementSetBindArraySize
(
    OCI_Statement* stmt,
    unsigned int   size
);

unsigned int OcilibStatementGetBindArraySize
(
    OCI_Statement* stmt
);

boolean OcilibStatementAllowRebinding
(
    OCI_Statement* stmt,
    boolean        value
);

boolean OcilibStatementIsRebindingAllowed
(
    OCI_Statement* stmt
);

boolean OcilibStatementBindBoolean
(
    OCI_Statement* stmt,
    const otext  * name,
    boolean      * data
);

boolean OcilibStatementBindNumber
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Number   * data
);

boolean OcilibStatementBindArrayOfNumbers
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Number  ** data,
    unsigned int   nbelem
);

boolean OcilibStatementBindShort
(
    OCI_Statement* stmt,
    const otext  * name,
    short        * data
);

boolean OcilibStatementBindArrayOfShorts
(
    OCI_Statement* stmt,
    const otext  * name,
    short        * data,
    unsigned int   nbelem
);

boolean OcilibStatementBindUnsignedShort
(
    OCI_Statement * stmt,
    const otext   * name,
    unsigned short* data
);

boolean OcilibStatementBindArrayOfUnsignedShorts
(
    OCI_Statement * stmt,
    const otext   * name,
    unsigned short* data,
    unsigned int    nbelem
);

boolean OcilibStatementBindInt
(
    OCI_Statement* stmt,
    const otext  * name,
    int          * data
);

boolean OcilibStatementBindArrayOfInts
(
    OCI_Statement* stmt,
    const otext  * name,
    int          * data,
    unsigned int   nbelem
);

boolean OcilibStatementBindUnsignedInt
(
    OCI_Statement* stmt,
    const otext  * name,
    unsigned int * data
);

boolean OcilibStatementBindArrayOfUnsignedInts
(
    OCI_Statement* stmt,
    const otext  * name,
    unsigned int * data,
    unsigned int   nbelem
);

boolean OcilibStatementBindBigInt
(
    OCI_Statement* stmt,
    const otext  * name,
    big_int      * data
);

boolean OcilibStatementBindArrayOfBigInts
(
    OCI_Statement* stmt,
    const otext  * name,
    big_int      * data,
    unsigned int   nbelem
);

boolean OcilibStatementBindUnsignedBigInt
(
    OCI_Statement* stmt,
    const otext  * name,
    big_uint     * data
);

boolean OcilibStatementBindArrayOfUnsignedBigInts
(
    OCI_Statement* stmt,
    const otext  * name,
    big_uint     * data,
    unsigned int   nbelem
);

boolean OcilibStatementBindString
(
    OCI_Statement* stmt,
    const otext  * name,
    otext        * data,
    unsigned int   len
);

boolean OcilibStatementBindArrayOfStrings
(
    OCI_Statement* stmt,
    const otext  * name,
    otext        * data,
    unsigned int   len,
    unsigned int   nbelem
);

boolean OcilibStatementBindRaw
(
    OCI_Statement* stmt,
    const otext  * name,
    void         * data,
    unsigned int   len
);

boolean OcilibStatementBindArrayOfRaws
(
    OCI_Statement* stmt,
    const otext  * name,
    void         * data,
    unsigned int   len,
    unsigned int   nbelem
);

boolean OcilibStatementBindDouble
(
    OCI_Statement* stmt,
    const otext  * name,
    double       * data
);

boolean OcilibStatementBindArrayOfDoubles
(
    OCI_Statement* stmt,
    const otext  * name,
    double       * data,
    unsigned int   nbelem
);

boolean OcilibStatementBindFloat
(
    OCI_Statement* stmt,
    const otext  * name,
    float        * data
);

boolean OcilibStatementBindArrayOfFloats
(
    OCI_Statement* stmt,
    const otext  * name,
    float        * data,
    unsigned int   nbelem
);

boolean OcilibStatementBindDate
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Date     * data
);

boolean OcilibStatementBindArrayOfDates
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Date    ** data,
    unsigned int   nbelem
);

boolean OcilibStatementBindTimestamp
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Timestamp* data
);

boolean OcilibStatementBindArrayOfTimestamps
(
    OCI_Statement * stmt,
    const otext   * name,
    OCI_Timestamp** data,
    unsigned int    type,
    unsigned int    nbelem
);

boolean OcilibStatementBindInterval
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Interval * data
);

boolean OcilibStatementBindArrayOfIntervals
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Interval** data,
    unsigned int   type,
    unsigned int   nbelem
);

boolean OcilibStatementBindObject
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Object   * data
);

boolean OcilibStatementBindArrayOfObjects
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Object  ** data,
    OCI_TypeInfo * typinf,
    unsigned int   nbelem
);

boolean OcilibStatementBindLob
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Lob      * data
);

boolean OcilibStatementBindArrayOfLobs
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Lob     ** data,
    unsigned int   type,
    unsigned int   nbelem
);

boolean OcilibStatementBindFile
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_File     * data
);

boolean OcilibStatementBindArrayOfFiles
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_File    ** data,
    unsigned int   type,
    unsigned int   nbelem
);

boolean OcilibStatementBindReference
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Ref      * data
);

boolean OcilibStatementBindArrayOfReferences
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Ref     ** data,
    OCI_TypeInfo * typinf,
    unsigned int   nbelem
);

boolean OcilibStatementBindCollection
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Coll     * data
);

boolean OcilibStatementBindArrayOfCollections
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Coll    ** data,
    OCI_TypeInfo * typinf,
    unsigned int   nbelem
);

boolean OcilibStatementBindStatement
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Statement* data
);

boolean OcilibStatementBindLong
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_Long     * data,
    unsigned int   size
);

boolean OcilibStatementRegisterNumber
(
    OCI_Statement* stmt,
    const otext  * name
);

boolean OcilibStatementRegisterShort
(
    OCI_Statement* stmt,
    const otext  * name
);

boolean OcilibStatementRegisterUnsignedShort
(
    OCI_Statement* stmt,
    const otext  * name
);

boolean OcilibStatementRegisterInt
(
    OCI_Statement* stmt,
    const otext  * name
);

boolean OcilibStatementRegisterUnsignedInt
(
    OCI_Statement* stmt,
    const otext  * name
);

boolean OcilibStatementRegisterBigInt
(
    OCI_Statement* stmt,
    const otext  * name
);

boolean OcilibStatementRegisterUnsignedBigInt
(
    OCI_Statement* stmt,
    const otext  * name
);

boolean OcilibStatementRegisterString
(
    OCI_Statement* stmt,
    const otext  * name,
    unsigned int   len
);

boolean OcilibStatementRegisterRaw
(
    OCI_Statement* stmt,
    const otext  * name,
    unsigned int   len
);

boolean OcilibStatementRegisterDouble
(
    OCI_Statement* stmt,
    const otext  * name
);

boolean OcilibStatementRegisterFloat
(
    OCI_Statement* stmt,
    const otext  * name
);

boolean OcilibStatementRegisterDate
(
    OCI_Statement* stmt,
    const otext  * name
);

boolean OcilibStatementRegisterTimestamp
(
    OCI_Statement* stmt,
    const otext  * name,
    unsigned int   type
);

boolean OcilibStatementRegisterInterval
(
    OCI_Statement* stmt,
    const otext  * name,
    unsigned int   type
);

boolean OcilibStatementRegisterObject
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_TypeInfo * typinf
);

boolean OcilibStatementRegisterLob
(
    OCI_Statement* stmt,
    const otext  * name,
    unsigned int   type
);

boolean OcilibStatementRegisterFile
(
    OCI_Statement* stmt,
    const otext  * name,
    unsigned int   type
);

boolean OcilibStatementRegisterReference
(
    OCI_Statement* stmt,
    const otext  * name,
    OCI_TypeInfo * typinf
);

unsigned int OcilibStatementGetStatementType
(
    OCI_Statement* stmt
);

boolean OcilibStatementSetFetchMode
(
    OCI_Statement* stmt,
    unsigned int   mode
);

unsigned int OcilibStatementGetFetchMode
(
    OCI_Statement* stmt
);

boolean OcilibStatementSetBindMode
(
    OCI_Statement* stmt,
    unsigned int   mode
);

unsigned int OcilibStatementGetBindMode
(
    OCI_Statement* stmt
);

boolean OcilibStatementSetBindAllocation
(
    OCI_Statement* stmt,
    unsigned int   mode
);

unsigned int OcilibStatementGetBindAllocation
(
    OCI_Statement* stmt
);

boolean OcilibStatementSetFetchSize
(
    OCI_Statement* stmt,
    unsigned int   size
);

unsigned int OcilibStatementGetFetchSize
(
    OCI_Statement* stmt
);

boolean OcilibStatementSetPrefetchSize
(
    OCI_Statement* stmt,
    unsigned int   size
);

unsigned int OcilibStatementGetPrefetchSize
(
    OCI_Statement* stmt
);

boolean OcilibStatementSetPrefetchMemory
(
    OCI_Statement* stmt,
    unsigned int   size
);

unsigned int OcilibStatementGetPrefetchMemory
(
    OCI_Statement* stmt
);

boolean OcilibStatementSetLongMaxSize
(
    OCI_Statement* stmt,
    unsigned int   size
);

unsigned int OcilibStatementGetLongMaxSize
(
    OCI_Statement* stmt
);

boolean OcilibStatementSetLongMode
(
    OCI_Statement* stmt,
    unsigned int   mode
);

unsigned int OcilibStatementGetLongMode
(
    OCI_Statement* stmt
);

OCI_Connection* OcilibStatementGetConnection
(
    OCI_Statement* stmt
);

const otext* OcilibStatementGetSql
(
    OCI_Statement* stmt
);

const otext* OcilibStatementGetSqlIdentifier
(
    OCI_Statement* stmt
);

unsigned int OcilibStatementGetSqlErrorPos
(
    OCI_Statement* stmt
);

unsigned int OcilibStatementGetAffectedRows
(
    OCI_Statement* stmt
);

unsigned int OcilibStatementGetBindCount
(
    OCI_Statement* stmt
);

OCI_Bind* OcilibStatementGetBind
(
    OCI_Statement* stmt,
    unsigned int   index
);

OCI_Bind* OcilibStatementGetBind2
(
    OCI_Statement* stmt,
    const otext  * name
);

unsigned int OcilibStatementGetBindIndex
(
    OCI_Statement* stmt,
    const otext  * name
);

unsigned int OcilibStatementGetSqlCommand
(
    OCI_Statement* stmt
);

const otext* OcilibStatementGetSqlVerb
(
    OCI_Statement* stmt
);

OCI_Error* OcilibStatementGetBatchError
(
    OCI_Statement* stmt
);

unsigned int OcilibStatementGetBatchErrorCount
(
    OCI_Statement* stmt
);

#endif /* OCILIB_STATEMENT_H_INCLUDED */
