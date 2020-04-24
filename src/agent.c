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
    DECLARE_CTX(TRUE)
    CTX_SET_FROM_CON(con)

    ALLOC_DATA(OCI_IPC_AGENT, agent, 1);

    if (STATUS)
    {
        FREE(agent->name)
        FREE(agent->address)

        agent->con    = con;
        agent->handle = handle;

        if (!handle)
        {
            agent->hstate = OCI_OBJECT_ALLOCATED;

            STATUS = MemoryAllocDescriptor((dvoid *)agent->con->env, (dvoid **)&agent->handle, OCI_DTYPE_AQAGENT);
        }
        else
        {
            agent->hstate = OCI_OBJECT_FETCHED_CLEAN;
        }

        /* set name attribute if provided */

        if (STATUS && IS_STRING_VALID(name))
        {
            STATUS = AgentSetName(agent, name);
        }

        /* set address attribute if provided */

        if (STATUS && IS_STRING_VALID(address))
        {
            STATUS = AgentSetAddress(agent, address);
        }
    }

    /* check for failure */

    if (!STATUS && agent)
    {
        AgentFree(agent);
        agent = NULL;
    }

    return agent;
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
    CALL_ENTER(OCI_Agent *, NULL)
    CHECK_PTR(OCI_IPC_CONNECTION, con)
    CTX_SET_FROM_CON(con)

    RETVAL = AgentInitialize(con, NULL, NULL, name, address);
    STATUS = (NULL != RETVAL);

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * AgentFree
 * --------------------------------------------------------------------------------------------- */

boolean AgentFree
(
    OCI_Agent *agent
)
{
    CALL_ENTER(boolean, FALSE)
    CHECK_PTR(OCI_IPC_AGENT, agent)
    CTX_SET_FROM_CON(agent->con)

    if (OCI_OBJECT_ALLOCATED == agent->hstate)
    {
        MemoryFreeDescriptor((dvoid *) agent->handle, OCI_DTYPE_AQAGENT);
    }

    FREE(agent->address)
    FREE(agent->name)
    FREE(agent)

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * AgentGetName
 * --------------------------------------------------------------------------------------------- */

const otext * AgentGetName
(
    OCI_Agent *agent
)
{
    CALL_ENTER(otext *, NULL)
    CHECK_PTR(OCI_IPC_AGENT, agent)
    CTX_SET_FROM_CON(agent->con)

    if (!agent->name)
    {
        unsigned int size = 0;
        
        STATUS = StringGetAttribute(agent->con, agent->handle, OCI_DTYPE_AQAGENT,
                                        OCI_ATTR_AGENT_NAME,  &agent->name, &size);
    }

    RETVAL = agent->name;

    CALL_EXIT()
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
    CALL_ENTER(boolean, FALSE)
    CHECK_PTR(OCI_IPC_AGENT, agent)
    CTX_SET_FROM_CON(agent->con)

    STATUS = StringSetAttribute(agent->con, agent->handle, OCI_DTYPE_AQAGENT, OCI_ATTR_AGENT_NAME, &agent->name, name);
    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * AgentGetAddress
 * --------------------------------------------------------------------------------------------- */

const otext * AgentGetAddress
(
    OCI_Agent *agent
)
{
    CALL_ENTER(otext *, NULL)
    CHECK_PTR(OCI_IPC_AGENT, agent)
    CTX_SET_FROM_CON(agent->con)

    if (!agent->address)
    {
        unsigned int size = 0;
        
        STATUS = StringGetAttribute(agent->con, agent->handle, OCI_DTYPE_AQAGENT,
                                        OCI_ATTR_AGENT_ADDRESS, &agent->address, &size);
    }

    RETVAL = agent->address;

    CALL_EXIT()
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
    CALL_ENTER(boolean, FALSE)
    CHECK_PTR(OCI_IPC_AGENT, agent)
    CTX_SET_FROM_CON(agent->con)

    STATUS = StringSetAttribute(agent->con, agent->handle, OCI_DTYPE_AQAGENT, OCI_ATTR_AGENT_ADDRESS, &agent->address, address);
    RETVAL = STATUS;
    
    CALL_EXIT()
}
