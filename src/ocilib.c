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
