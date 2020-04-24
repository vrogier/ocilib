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

boolean StatementFetchIntoUserVariables
(
    OCI_Statement* stmt,
    va_list        args
);

boolean StatementDispose
(
    OCI_Statement* stmt
);

OCI_Statement* StatementInitialize
(
    OCI_Connection* con,
    OCI_Statement* stmt,
    OCIStmt* handle,
    boolean         is_desc,
    const otext* sql
);

boolean StatementPrepareInternal
(
    OCI_Statement* stmt,
    const otext* sql
);

boolean StatementExecuteInternal
(
    OCI_Statement* stmt,
    ub4            mode
);

OCI_Statement* StatementCreate
(
    OCI_Connection* con
);

boolean StatementFree
(
    OCI_Statement* stmt
);

OCI_Resultset* StatementGetResultset
(
    OCI_Statement* stmt
);

OCI_Resultset* StatementGetNextResultset
(
    OCI_Statement* stmt
);

boolean StatementReleaseResultsets
(
    OCI_Statement* stmt
);

boolean StatementPrepare
(
    OCI_Statement* stmt,
    const otext* sql
);

boolean StatementExecute
(
    OCI_Statement* stmt
);

boolean StatementExecuteStmt
(
    OCI_Statement* stmt,
    const otext* sql
);

boolean StatementParse
(
    OCI_Statement* stmt,
    const otext* sql
);

boolean StatementDescribe
(
    OCI_Statement* stmt,
    const otext* sql
);

boolean StatementPrepareFmt
(
    OCI_Statement* stmt,
    const otext* sql,
    va_list args
);

boolean StatementExecuteStmtFmt
(
    OCI_Statement* stmt,
    const otext* sql,
    va_list args
);

boolean StatementParseFmt
(
    OCI_Statement* stmt,
    const otext* sql,
    va_list args
);

boolean StatementDescribeFmt
(
    OCI_Statement* stmt,
    const otext* sql,
    va_list args
);

boolean StatementSetBindArraySize
(
    OCI_Statement* stmt,
    unsigned int   size
);

unsigned int StatementGetBindArraySize
(
    OCI_Statement* stmt
);

boolean StatementAllowRebinding
(
    OCI_Statement* stmt,
    boolean        value
);

boolean StatementIsRebindingAllowed
(
    OCI_Statement* stmt
);

boolean StatementBindBoolean
(
    OCI_Statement* stmt,
    const otext* name,
    boolean* data
);

boolean StatementBindNumber
(
    OCI_Statement* stmt,
    const otext* name,
    OCI_Number* data
);

boolean StatementBindArrayOfNumbers
(
    OCI_Statement* stmt,
    const otext* name,
    OCI_Number** data,
    unsigned int   nbelem
);

boolean StatementBindShort
(
    OCI_Statement* stmt,
    const otext* name,
    short* data
);

boolean StatementBindArrayOfShorts
(
    OCI_Statement* stmt,
    const otext* name,
    short* data,
    unsigned int   nbelem
);

boolean StatementBindUnsignedShort
(
    OCI_Statement* stmt,
    const otext* name,
    unsigned short* data
);

boolean StatementBindArrayOfUnsignedShorts
(
    OCI_Statement* stmt,
    const otext* name,
    unsigned short* data,
    unsigned int    nbelem
);

boolean StatementBindInt
(
    OCI_Statement* stmt,
    const otext* name,
    int* data
);

boolean StatementBindArrayOfInts
(
    OCI_Statement* stmt,
    const otext* name,
    int* data,
    unsigned int   nbelem
);

boolean StatementBindUnsignedInt
(
    OCI_Statement* stmt,
    const otext* name,
    unsigned int* data
);

boolean StatementBindArrayOfUnsignedInts
(
    OCI_Statement* stmt,
    const otext* name,
    unsigned int* data,
    unsigned int   nbelem
);

boolean StatementBindBigInt
(
    OCI_Statement* stmt,
    const otext* name,
    big_int* data
);

boolean StatementBindArrayOfBigInts
(
    OCI_Statement* stmt,
    const otext* name,
    big_int* data,
    unsigned int   nbelem
);

boolean StatementBindUnsignedBigInt
(
    OCI_Statement* stmt,
    const otext* name,
    big_uint* data
);

boolean StatementBindArrayOfUnsignedBigInts
(
    OCI_Statement* stmt,
    const otext* name,
    big_uint* data,
    unsigned int   nbelem
);

boolean StatementBindString
(
    OCI_Statement* stmt,
    const otext* name,
    otext* data,
    unsigned int   len
);

boolean StatementBindArrayOfStrings
(
    OCI_Statement* stmt,
    const otext* name,
    otext* data,
    unsigned int   len,
    unsigned int   nbelem
);

boolean StatementBindRaw
(
    OCI_Statement* stmt,
    const otext* name,
    void* data,
    unsigned int   len
);

boolean StatementBindArrayOfRaws
(
    OCI_Statement* stmt,
    const otext* name,
    void* data,
    unsigned int   len,
    unsigned int   nbelem
);

boolean StatementBindDouble
(
    OCI_Statement* stmt,
    const otext* name,
    double* data
);

boolean StatementBindArrayOfDoubles
(
    OCI_Statement* stmt,
    const otext* name,
    double* data,
    unsigned int   nbelem
);

boolean StatementBindFloat
(
    OCI_Statement* stmt,
    const otext* name,
    float* data
);

boolean StatementBindArrayOfFloats
(
    OCI_Statement* stmt,
    const otext* name,
    float* data,
    unsigned int   nbelem
);

boolean StatementBindDate
(
    OCI_Statement* stmt,
    const otext* name,
    OCI_Date* data
);

boolean StatementBindArrayOfDates
(
    OCI_Statement* stmt,
    const otext* name,
    OCI_Date** data,
    unsigned int   nbelem
);

boolean StatementBindTimestamp
(
    OCI_Statement* stmt,
    const otext* name,
    OCI_Timestamp* data
);

boolean StatementBindArrayOfTimestamps
(
    OCI_Statement* stmt,
    const otext* name,
    OCI_Timestamp** data,
    unsigned int    type,
    unsigned int    nbelem
);

boolean StatementBindInterval
(
    OCI_Statement* stmt,
    const otext* name,
    OCI_Interval* data
);

boolean StatementBindArrayOfIntervals
(
    OCI_Statement* stmt,
    const otext* name,
    OCI_Interval** data,
    unsigned int   type,
    unsigned int   nbelem
);

boolean StatementBindObject
(
    OCI_Statement* stmt,
    const otext* name,
    OCI_Object* data
);

boolean StatementBindArrayOfObjects
(
    OCI_Statement* stmt,
    const otext* name,
    OCI_Object** data,
    OCI_TypeInfo* typinf,
    unsigned int   nbelem
);

boolean StatementBindLob
(
    OCI_Statement* stmt,
    const otext* name,
    OCI_Lob* data
);

boolean StatementBindArrayOfLobs
(
    OCI_Statement* stmt,
    const otext* name,
    OCI_Lob** data,
    unsigned int   type,
    unsigned int   nbelem
);

boolean StatementBindFile
(
    OCI_Statement* stmt,
    const otext* name,
    OCI_File* data
);

boolean StatementBindArrayOfFiles
(
    OCI_Statement* stmt,
    const otext* name,
    OCI_File** data,
    unsigned int   type,
    unsigned int   nbelem
);

boolean StatementBindReference
(
    OCI_Statement* stmt,
    const otext* name,
    OCI_Ref* data
);

boolean StatementBindArrayOfReferences
(
    OCI_Statement* stmt,
    const otext* name,
    OCI_Ref** data,
    OCI_TypeInfo* typinf,
    unsigned int   nbelem
);

boolean StatementBindCollection
(
    OCI_Statement* stmt,
    const otext* name,
    OCI_Coll* data
);

boolean StatementBindArrayOfCollections
(
    OCI_Statement* stmt,
    const otext* name,
    OCI_Coll** data,
    OCI_TypeInfo* typinf,
    unsigned int   nbelem
);

boolean StatementBindStatement
(
    OCI_Statement* stmt,
    const otext* name,
    OCI_Statement* data
);

boolean StatementBindLong
(
    OCI_Statement* stmt,
    const otext* name,
    OCI_Long* data,
    unsigned int   size
);

boolean StatementRegisterNumber
(
    OCI_Statement* stmt,
    const otext* name
);

boolean StatementRegisterShort
(
    OCI_Statement* stmt,
    const otext* name
);

boolean StatementRegisterUnsignedShort
(
    OCI_Statement* stmt,
    const otext* name
);

boolean StatementRegisterInt
(
    OCI_Statement* stmt,
    const otext* name
);

boolean StatementRegisterUnsignedInt
(
    OCI_Statement* stmt,
    const otext* name
);

boolean StatementRegisterBigInt
(
    OCI_Statement* stmt,
    const otext* name
);

boolean StatementRegisterUnsignedBigInt
(
    OCI_Statement* stmt,
    const otext* name
);

boolean StatementRegisterString
(
    OCI_Statement* stmt,
    const otext* name,
    unsigned int   len
);

boolean StatementRegisterRaw
(
    OCI_Statement* stmt,
    const otext* name,
    unsigned int   len
);

boolean StatementRegisterDouble
(
    OCI_Statement* stmt,
    const otext* name
);

boolean StatementRegisterFloat
(
    OCI_Statement* stmt,
    const otext* name
);

boolean StatementRegisterDate
(
    OCI_Statement* stmt,
    const otext* name
);

boolean StatementRegisterTimestamp
(
    OCI_Statement* stmt,
    const otext* name,
    unsigned int   type
);

boolean StatementRegisterInterval
(
    OCI_Statement* stmt,
    const otext* name,
    unsigned int   type
);

boolean StatementRegisterObject
(
    OCI_Statement* stmt,
    const otext* name,
    OCI_TypeInfo* typinf
);

boolean StatementRegisterLob
(
    OCI_Statement* stmt,
    const otext* name,
    unsigned int   type
);

boolean StatementRegisterFile
(
    OCI_Statement* stmt,
    const otext* name,
    unsigned int   type
);

boolean StatementRegisterReference
(
    OCI_Statement* stmt,
    const otext* name,
    OCI_TypeInfo* typinf
);

unsigned int StatementGetStatementType
(
    OCI_Statement* stmt
);

boolean StatementSetFetchMode
(
    OCI_Statement* stmt,
    unsigned int   mode
);

unsigned int StatementGetFetchMode
(
    OCI_Statement* stmt
);

boolean StatementSetBindMode
(
    OCI_Statement* stmt,
    unsigned int   mode
);

unsigned int StatementGetBindMode
(
    OCI_Statement* stmt
);

boolean StatementSetBindAllocation
(
    OCI_Statement* stmt,
    unsigned int   mode
);

unsigned int StatementGetBindAllocation
(
    OCI_Statement* stmt
);

boolean StatementSetFetchSize
(
    OCI_Statement* stmt,
    unsigned int   size
);

unsigned int StatementGetFetchSize
(
    OCI_Statement* stmt
);

boolean StatementSetPrefetchSize
(
    OCI_Statement* stmt,
    unsigned int   size
);

unsigned int StatementGetPrefetchSize
(
    OCI_Statement* stmt
);

boolean StatementSetPrefetchMemory
(
    OCI_Statement* stmt,
    unsigned int   size
);

unsigned int StatementGetPrefetchMemory
(
    OCI_Statement* stmt
);

boolean StatementSetLongMaxSize
(
    OCI_Statement* stmt,
    unsigned int   size
);

unsigned int StatementGetLongMaxSize
(
    OCI_Statement* stmt
);

boolean StatementSetLongMode
(
    OCI_Statement* stmt,
    unsigned int   mode
);

unsigned int StatementGetLongMode
(
    OCI_Statement* stmt
);

OCI_Connection* StatementGetConnection
(
    OCI_Statement* stmt
);

const otext* StatementGetSql
(
    OCI_Statement* stmt
);

const otext* StatementGetSqlIdentifier
(
    OCI_Statement* stmt
);

unsigned int StatementGetSqlErrorPos
(
    OCI_Statement* stmt
);

unsigned int StatementGetAffectedRows
(
    OCI_Statement* stmt
);

unsigned int StatementGetBindCount
(
    OCI_Statement* stmt
);

OCI_Bind* StatementGetBind
(
    OCI_Statement* stmt,
    unsigned int   index
);

OCI_Bind* StatementGetBind2
(
    OCI_Statement* stmt,
    const otext* name
);

unsigned int StatementGetBindIndex
(
    OCI_Statement* stmt,
    const otext* name
);

unsigned int StatementGetSqlCommand
(
    OCI_Statement* stmt
);

const otext* StatementGetSqlVerb
(
    OCI_Statement* stmt
);

OCI_Error* StatementGetBatchError
(
    OCI_Statement* stmt
);

unsigned int StatementGetBatchErrorCount
(
    OCI_Statement* stmt
);

#endif /* OCILIB_STATEMENT_H_INCLUDED */
