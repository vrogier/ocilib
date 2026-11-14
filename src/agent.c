/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2026 Vincent ROGIER <vince.rogier@gmail.com>
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
#include "stringutils.h"

/* --------------------------------------------------------------------------------------------- *
 * OcilibAgentInit
 * --------------------------------------------------------------------------------------------- */

OCI_Agent * OcilibAgentInitialize
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
    );

    CHECK_PTR(OCI_IPC_CONNECTION, con);

    ALLOC_HANDLE(OCI_IPC_AGENT, agent);

    FREE(agent->name);
    FREE(agent->address);

    agent->con    = con;
    agent->handle = handle;

    if (NULL == handle)
    {
        agent->hstate = OCI_OBJECT_ALLOCATED;

        CHECK(OcilibMemoryAllocDescriptor(agent->con->env,
                                          (dvoid *)agent->con->env->handle,
                                          (dvoid **)&agent->handle,
                                          OCI_DTYPE_AQAGENT));
    }
    else
    {
        agent->hstate = OCI_OBJECT_FETCHED_CLEAN;
    }

    /* set name attribute if provided */

    if (IS_STRING_VALID(name))
    {
        CHECK(OcilibAgentSetName(agent, name));
    }

    /* set address attribute if provided */

    if (IS_STRING_VALID(address))
    {
        CHECK(OcilibAgentSetAddress(agent, address));
    }

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE)
        {
            OcilibAgentFree(agent);
            agent = NULL;
        }

        SET_RETVAL(agent);
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibAgentCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Agent * OcilibAgentCreate
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
    );

    CHECK_PTR(OCI_IPC_CONNECTION, con);

    SET_RETVAL(OcilibAgentInitialize(con, NULL, NULL, name, address));

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibAgentFree
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibAgentFree
(
    OCI_Agent *agent
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_AGENT, agent
    );

    CHECK_PTR(OCI_IPC_AGENT, agent);

    if (OCI_OBJECT_ALLOCATED == agent->hstate)
    {
        OcilibMemoryFreeDescriptor(agent->con->env, (dvoid*)agent->handle, OCI_DTYPE_AQAGENT);
    }

    OcilibErrorResetSource(NULL, agent);

    FREE(agent->address);
    FREE(agent->name);
    FREE(agent);

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibAgentGetName
 * --------------------------------------------------------------------------------------------- */

const otext * OcilibAgentGetName
(
    OCI_Agent *agent
)
{
    ENTER_FUNC
    (
        /* returns */ otext*, NULL,
        /* context */ OCI_IPC_AGENT, agent
    );

    CHECK_PTR(OCI_IPC_AGENT, agent);

    if (NULL == agent->name)
    {
        OCI_UInt32 size = 0;

        CHECK(OcilibStringGetAttribute(agent->con,
                                       agent->handle,
                                       OCI_DTYPE_AQAGENT,
                                       OCI_ATTR_AGENT_NAME,
                                       &agent->name,
                                       &size));
    }

    SET_RETVAL(agent->name);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibAgentSetName
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibAgentSetName
(
    OCI_Agent   *agent,
    const otext *name
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_AGENT, agent
    );

    CHECK_PTR(OCI_IPC_AGENT, agent);

    CHECK(OcilibStringSetAttribute(agent->con,
                                   agent->handle,
                                   OCI_DTYPE_AQAGENT,
                                   OCI_ATTR_AGENT_NAME,
                                   &agent->name,
                                   name));

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibAgentGetAddress
 * --------------------------------------------------------------------------------------------- */

const otext * OcilibAgentGetAddress
(
    OCI_Agent *agent
)
{
    ENTER_FUNC
    (
        /* returns */ otext*, NULL,
        /* context */ OCI_IPC_AGENT, agent
    );

    CHECK_PTR(OCI_IPC_AGENT, agent);

    if (NULL == agent->address)
    {
        OCI_UInt32 size = 0;

        CHECK(OcilibStringGetAttribute(agent->con,
                                       agent->handle,
                                       OCI_DTYPE_AQAGENT,
                                       OCI_ATTR_AGENT_ADDRESS,
                                       &agent->address,
                                       &size));
    }

    SET_RETVAL(agent->address);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibAgentSetAddress
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibAgentSetAddress
(
    OCI_Agent   *agent,
    const otext *address
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_AGENT, agent
    );

    CHECK_PTR(OCI_IPC_AGENT, agent);

    CHECK(OcilibStringSetAttribute(agent->con,
                                   agent->handle,
                                   OCI_DTYPE_AQAGENT,
                                   OCI_ATTR_AGENT_ADDRESS,
                                   &agent->address,
                                   address));

    SET_SUCCESS();

    EXIT_FUNC();
}
