/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2016 Vincent ROGIER <vince.rogier@ocilib.net>
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
    OCI_Agent     **pagent,
    OCIAQAgent     *handle,
    const otext    *name,
    const otext    *address
)
{
    OCI_Agent *agent = NULL;
    boolean    res   = FALSE;

    OCI_CHECK(NULL == pagent, NULL)

    /* allocate agent structure */

    if (!*pagent)
    {
        *pagent = (OCI_Agent *) OCI_MemAlloc(OCI_IPC_AGENT, sizeof(*agent),  (size_t) 1, TRUE);
    }

    if (*pagent)
    {
        res = TRUE;

        agent = *pagent;

        OCI_FREE(agent->name)
        OCI_FREE(agent->address)

        agent->con    = con;
        agent->handle = handle;

        if (!handle)
        {
            agent->hstate = OCI_OBJECT_ALLOCATED;

            res = OCI_SUCCESSFUL(OCI_DescriptorAlloc((dvoid * ) agent->con->env,
                                                     (dvoid **) &agent->handle,
                                                     OCI_DTYPE_AQAGENT,
                                                     (size_t) 0, (dvoid **) NULL));
        }
        else
        {
            agent->hstate = OCI_OBJECT_FETCHED_CLEAN;
        }

        /* set name attribute if provided */

        if (res && name && name[0])
        {
            res = OCI_AgentSetName(agent, name);
        }

        /* set address attribute if provided */

        if (res && address && address[0])
        {
            res = OCI_AgentSetAddress(agent, address);
        }
    }

    /* check for failure */

    if (!res && agent)
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
    OCI_LIB_CALL_ENTER(OCI_Agent *, NULL)

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con)

    call_retval = OCI_AgentInit(con, &call_retval, NULL, name, address);
    call_status = (NULL != call_retval);

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_AgentFree
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_AgentFree
(
    OCI_Agent *agent
)
{
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_AGENT, agent)

    call_status = TRUE;

    if (OCI_OBJECT_ALLOCATED == agent->hstate)
    {
        OCI_DescriptorFree((dvoid *) agent->handle, OCI_DTYPE_AQAGENT);
    }

    OCI_FREE(agent->address)
    OCI_FREE(agent->name)
    OCI_FREE(agent)

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_AgentGetName
 * --------------------------------------------------------------------------------------------- */

const otext * OCI_API OCI_AgentGetName
(
    OCI_Agent *agent
)
{
    OCI_LIB_CALL_ENTER(otext *, NULL)

    OCI_CHECK_PTR(OCI_IPC_AGENT, agent)

    call_status = TRUE;

    if (!agent->name)
    {
        call_status = OCI_GetStringAttribute(agent->con, agent->handle,  OCI_DTYPE_AQAGENT,
                                             OCI_ATTR_AGENT_NAME,  &agent->name);
    }

    call_retval = agent->name;

    OCI_LIB_CALL_EXIT()
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
    OCI_LIB_CALL_ENTER(boolean , FALSE)
        
    OCI_CHECK_PTR(OCI_IPC_AGENT, agent)

    call_retval = call_status = OCI_SetStringAttribute(agent->con, agent->handle,  OCI_DTYPE_AQAGENT,
                                                       OCI_ATTR_AGENT_NAME, &agent->name, name);

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_AgentGetAddress
 * --------------------------------------------------------------------------------------------- */

const otext * OCI_API OCI_AgentGetAddress
(
    OCI_Agent *agent
)
{
    OCI_LIB_CALL_ENTER(otext *, NULL)

    OCI_CHECK_PTR(OCI_IPC_AGENT, agent)

    call_status = TRUE;

    if (!agent->address)
    {
        call_status = OCI_GetStringAttribute(agent->con, agent->handle, OCI_DTYPE_AQAGENT,
                                             OCI_ATTR_AGENT_ADDRESS, &agent->address);
    }

    call_retval = agent->address;

    OCI_LIB_CALL_EXIT()
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
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_AGENT, agent)

    call_retval = call_status = OCI_SetStringAttribute(agent->con, agent->handle, OCI_DTYPE_AQAGENT,
                                                       OCI_ATTR_AGENT_ADDRESS, &agent->address, address);
    
    OCI_LIB_CALL_EXIT()
}
