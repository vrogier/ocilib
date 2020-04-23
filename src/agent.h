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

#ifndef OCILIB_AGENT_H_INCLUDED
#define OCILIB_AGENT_H_INCLUDED

#include "types.h"

OCI_Agent * AgentInit
(
    OCI_Connection *con,
    OCI_Agent      *agent,
    OCIAQAgent     *handle,
    const otext    *name,
    const otext    *address
);

OCI_Agent* AgentCreate
(
    OCI_Connection* con,
    const otext* name,
    const otext* address
);

boolean AgentFree
(
    OCI_Agent* agent
);

const otext* AgentGetName
(
    OCI_Agent* agent
);

boolean AgentSetName
(
    OCI_Agent* agent,
    const otext* name
);

const otext* AgentGetAddress
(
    OCI_Agent* agent
);

boolean AgentSetAddress
(
    OCI_Agent* agent,
    const otext* address
);

#endif /* OCILIB_AGENT_H_INCLUDED */
