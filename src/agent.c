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

#include "agent.h"

#include "macros.h"
#include "strings.h"

/* --------------------------------------------------------------------------------------------- *
 * AgentInit
 * --------------------------------------------------------------------------------------------- */

OCI_Agent * AgentInitialize
(
    OCI_Connection *con,
    OCI_Agent      *agent,
    OCIAQAgent     *handle,
    const otext    *name,
    const otext    *address
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Agent*, agent,
        /* context */ OCI_IPC_CONNECTION, con
    )

    CHECK_PTR(OCI_IPC_CONNECTION, con)

    ALLOC_DATA(OCI_IPC_AGENT, agent, 1)

    FREE(agent->name)
    FREE(agent->address)

    agent->con    = con;
    agent->handle = handle;

    if (NULL == handle)
    {
        agent->hstate = OCI_OBJECT_ALLOCATED;

        CHECK(MemoryAllocDescriptor((dvoid *)agent->con->env, (dvoid **)&agent->handle, OCI_DTYPE_AQAGENT))
    }
    else
    {
        agent->hstate = OCI_OBJECT_FETCHED_CLEAN;
    }

    /* set name attribute if provided */

    if (IS_STRING_VALID(name))
    {
        CHECK(AgentSetName(agent, name))
    }

    /* set address attribute if provided */

    if (IS_STRING_VALID(address))
    {
        CHECK(AgentSetAddress(agent, address))
    }

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE)
        {
            AgentFree(agent);
            agent = NULL;
        }

        SET_RETVAL(agent)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * AgentCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Agent * AgentCreate
(
    OCI_Connection *con,
    const otext    *name,
    const otext    *address
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Agent*, NULL,
        /* context */ OCI_IPC_CONNECTION, con
    )

    CHECK_PTR(OCI_IPC_CONNECTION, con)

    SET_RETVAL(AgentInitialize(con, NULL, NULL, name, address))

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * AgentFree
 * --------------------------------------------------------------------------------------------- */

boolean AgentFree
(
    OCI_Agent *agent
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_AGENT, agent
    )

    CHECK_PTR(OCI_IPC_AGENT, agent)

    if (OCI_OBJECT_ALLOCATED == agent->hstate)
    {
        MemoryFreeDescriptor((dvoid*)agent->handle, OCI_DTYPE_AQAGENT);
    }

    ErrorResetSource(NULL, agent);

    FREE(agent->address)
    FREE(agent->name)
    FREE(agent)

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * AgentGetName
 * --------------------------------------------------------------------------------------------- */

const otext * AgentGetName
(
    OCI_Agent *agent
)
{
    ENTER_FUNC
    (
        /* returns */ otext*, NULL,
        /* context */ OCI_IPC_AGENT, agent
    )

    CHECK_PTR(OCI_IPC_AGENT, agent)

    if (NULL == agent->name)
    {
        unsigned int size = 0;

        CHECK(StringGetAttribute(agent->con,
                                 agent->handle,
                                 OCI_DTYPE_AQAGENT,
                                 OCI_ATTR_AGENT_NAME,
                                 &agent->name,
                                 &size))
    }

    SET_RETVAL(agent->name)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * AgentSetName
 * --------------------------------------------------------------------------------------------- */

boolean AgentSetName
(
    OCI_Agent   *agent,
    const otext *name
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_AGENT, agent
    )

    CHECK_PTR(OCI_IPC_AGENT, agent)

    CHECK(StringSetAttribute(agent->con,
                             agent->handle,
                             OCI_DTYPE_AQAGENT,
                             OCI_ATTR_AGENT_NAME,
                             &agent->name,
                             name))

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * AgentGetAddress
 * --------------------------------------------------------------------------------------------- */

const otext * AgentGetAddress
(
    OCI_Agent *agent
)
{
    ENTER_FUNC
    (
        /* returns */ otext*, NULL,
        /* context */ OCI_IPC_AGENT, agent
    )

    CHECK_PTR(OCI_IPC_AGENT, agent)

    if (NULL == agent->address)
    {
        unsigned int size = 0;

        CHECK(StringGetAttribute(agent->con,
                                 agent->handle,
                                 OCI_DTYPE_AQAGENT,
                                 OCI_ATTR_AGENT_ADDRESS,
                                 &agent->address,
                                 &size))
    }

    SET_RETVAL(agent->address)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * AgentSetAddress
 * --------------------------------------------------------------------------------------------- */

boolean AgentSetAddress
(
    OCI_Agent   *agent,
    const otext *address
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_AGENT, agent
    )

    CHECK_PTR(OCI_IPC_AGENT, agent)

    CHECK(StringSetAttribute(agent->con,
                             agent->handle,
                             OCI_DTYPE_AQAGENT,
                             OCI_ATTR_AGENT_ADDRESS,
                             &agent->address,
                             address))

    SET_SUCCESS()

    EXIT_FUNC()
}
