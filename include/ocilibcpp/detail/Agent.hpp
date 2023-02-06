/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2023 Vincent ROGIER <vince.rogier@ocilib.net>
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

#pragma once

#include "ocilibcpp/types.hpp"

namespace ocilib
{

inline Agent::Agent(const Connection &connection, const ostring& name, const ostring& address)
{
    AcquireAllocated
    (
        core::Check(OCI_AgentCreate(connection, name.c_str(), address.c_str())),
        connection.GetHandle()
    );
}

inline Agent::Agent(OCI_Agent *pAgent, core::Handle *parent)
{
    AcquireTransient(pAgent, parent);
}

inline ostring Agent::GetName() const
{
    return core::MakeString(core::Check(OCI_AgentGetName(*this)));
}

inline void Agent::SetName(const ostring& value)
{
    core::Check(OCI_AgentSetName(*this, value.c_str()));
}

inline ostring Agent::GetAddress() const
{
    return core::MakeString(core::Check(OCI_AgentGetAddress(*this)));
}

inline void Agent::SetAddress(const ostring& value)
{
    core::Check(OCI_AgentSetAddress(*this, value.c_str()));
}

}
