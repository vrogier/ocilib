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

