/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2018 Vincent ROGIER <vince.rogier@ocilib.net>
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

/* ********************************************************************************************* *
 *                             PRIVATE FUNCTIONS
 * ********************************************************************************************* */

/* --------------------------------------------------------------------------------------------- *
 * OCI_AgentInit
 * --------------------------------------------------------------------------------------------- */

OCI_Agent * OCI_AgentInit
(
    OCI_Connection *con,
    OCI_Agent      *agent,
    OCIAQAgent     *handle,
    const otext    *name,
    const otext    *address
)
{
    OCI_CALL_DECLARE_CONTEXT(TRUE)
    OCI_CALL_CONTEXT_SET_FROM_CONN(con)

    OCI_ALLOCATE_DATA(OCI_IPC_AGENT, agent, 1);

    if (OCI_STATUS)
    {
        OCI_FREE(agent->name)
        OCI_FREE(agent->address)

        agent->con    = con;
        agent->handle = handle;

        if (!handle)
        {
            agent->hstate = OCI_OBJECT_ALLOCATED;

            OCI_STATUS = OCI_DescriptorAlloc((dvoid *)agent->con->env, (dvoid **)&agent->handle, OCI_DTYPE_AQAGENT);
        }
        else
        {
            agent->hstate = OCI_OBJECT_FETCHED_CLEAN;
        }

        /* set name attribute if provided */

        if (OCI_STATUS && OCI_STRING_VALID(name))
        {
            OCI_STATUS = OCI_AgentSetName(agent, name);
        }

        /* set address attribute if provided */

        if (OCI_STATUS && OCI_STRING_VALID(address))
        {
            OCI_STATUS = OCI_AgentSetAddress(agent, address);
        }
    }

    /* check for failure */

    if (!OCI_STATUS && agent)
    {
        OCI_AgentFree(agent);
        agent = NULL;
    }

    return agent;
}

/* ********************************************************************************************* *
 *                            PUBLIC FUNCTIONS
 * ********************************************************************************************* */

/* --------------------------------------------------------------------------------------------- *
 * OCI_AgentCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Agent * OCI_API OCI_AgentCreate
(
    OCI_Connection *con,
    const otext    *name,
    const otext    *address
)
{
    OCI_CALL_ENTER(OCI_Agent *, NULL)
    OCI_CALL_CHECK_PTR(OCI_IPC_CONNECTION, con)
    OCI_CALL_CONTEXT_SET_FROM_CONN(con)

    OCI_RETVAL = OCI_AgentInit(con, NULL, NULL, name, address);
    OCI_STATUS = (NULL != OCI_RETVAL);

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_AgentFree
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_AgentFree
(
    OCI_Agent *agent
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_AGENT, agent)
    OCI_CALL_CONTEXT_SET_FROM_CONN(agent->con)

    if (OCI_OBJECT_ALLOCATED == agent->hstate)
    {
        OCI_DescriptorFree((dvoid *) agent->handle, OCI_DTYPE_AQAGENT);
    }

    OCI_FREE(agent->address)
    OCI_FREE(agent->name)
    OCI_FREE(agent)

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_AgentGetName
 * --------------------------------------------------------------------------------------------- */

const otext * OCI_API OCI_AgentGetName
(
    OCI_Agent *agent
)
{
    OCI_CALL_ENTER(otext *, NULL)
    OCI_CALL_CHECK_PTR(OCI_IPC_AGENT, agent)
    OCI_CALL_CONTEXT_SET_FROM_CONN(agent->con)

    if (!agent->name)
    {
        unsigned int size = 0;
        
        OCI_STATUS = OCI_GetStringAttribute(agent->con, agent->handle, OCI_DTYPE_AQAGENT,
                                            OCI_ATTR_AGENT_NAME,  &agent->name, &size);
    }

    OCI_RETVAL = agent->name;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_AgentSetName
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_AgentSetName
(
    OCI_Agent   *agent,
    const otext *name
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_AGENT, agent)
    OCI_CALL_CONTEXT_SET_FROM_CONN(agent->con)

    OCI_STATUS = OCI_SetStringAttribute(agent->con, agent->handle, OCI_DTYPE_AQAGENT, OCI_ATTR_AGENT_NAME, &agent->name, name);
    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_AgentGetAddress
 * --------------------------------------------------------------------------------------------- */

const otext * OCI_API OCI_AgentGetAddress
(
    OCI_Agent *agent
)
{
    
    OCI_CALL_ENTER(otext *, NULL)
    OCI_CALL_CHECK_PTR(OCI_IPC_AGENT, agent)
    OCI_CALL_CONTEXT_SET_FROM_CONN(agent->con)

    if (!agent->address)
    {
        unsigned int size = 0;
        
        OCI_STATUS = OCI_GetStringAttribute(agent->con, agent->handle, OCI_DTYPE_AQAGENT,
                                            OCI_ATTR_AGENT_ADDRESS, &agent->address, &size);
    }

    OCI_RETVAL = agent->address;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_AgentSetAddress
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_AgentSetAddress
(
    OCI_Agent   *agent,
    const otext *address
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_AGENT, agent)
    OCI_CALL_CONTEXT_SET_FROM_CONN(agent->con)

    OCI_STATUS = OCI_SetStringAttribute(agent->con, agent->handle, OCI_DTYPE_AQAGENT, OCI_ATTR_AGENT_ADDRESS, &agent->address, address);
    OCI_RETVAL = OCI_STATUS;
    
    OCI_CALL_EXIT()
}
