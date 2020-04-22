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

