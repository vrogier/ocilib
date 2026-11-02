/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2026 Vincent ROGIER <vince.rogier@ocilib.net>
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

// ReSharper disable CppClangTidyHicppUseAuto
// ReSharper disable CppClangTidyModernizeUseAuto
// ReSharper disable CppClangTidyMiscMisplacedConst

namespace ocilib
{

inline Message::Message(const TypeInfo &typeInfo)
{
    Connection connection = typeInfo.GetConnection();

    AcquireAllocated
    (
        core::Check(OCI_MsgCreate(typeInfo)),
        connection.GetHandle()
    );
}

inline Message::Message(OCI_Msg *pMessage, core::Handle *parent)
{
    AcquireTransient(pMessage, parent);
}

inline void Message::Reset()
{
    core::Check(OCI_MsgReset(*this));
}

template<>
inline Object Message::GetPayload<Object>()
{
    return Object(core::Check(OCI_MsgGetObject(*this)), nullptr);
}

template<>
inline void Message::SetPayload<Object>(const Object &value)
{
    core::Check(OCI_MsgSetObject(*this, value));
}

template<>
inline Raw Message::GetPayload<Raw>()
{
    OCI_UInt32 size = 0;

    core::Check(OCI_MsgGetRaw(*this, nullptr, &size));

    std::vector<unsigned char> buffer(static_cast<OCI_Size>(size + 1));

    core::Check(OCI_MsgGetRaw(*this, static_cast<AnyPointer>(buffer.data()), &size));

    return core::MakeRaw(buffer.data(), size);
}

template<>
inline void Message::SetPayload<Raw>(const Raw &value)
{
    const AnyPointer data = value.empty() ? nullptr : static_cast<AnyPointer>(const_cast<Raw::value_type*>(&value[0])) ;

    core::Check(OCI_MsgSetRaw(*this, data, static_cast<OCI_UInt32>(value.size())));
}

inline Date Message::GetEnqueueTime() const
{
    return Date(core::Check(OCI_MsgGetEnqueueTime(*this)), nullptr);
}

inline OCI_Int32 Message::GetAttemptCount() const
{
    return core::Check(OCI_MsgGetAttemptCount(*this));
}

inline Message::MessageState Message::GetState() const
{
    return MessageState(static_cast<MessageState::Type>(core::Check(OCI_MsgGetState(*this))));
}

inline Raw Message::GetID() const
{
    OCI_UInt32 size = OCI_SIZE_BUFFER;

    std::vector<unsigned char> buffer(static_cast<OCI_Size>(size + 1));

    core::Check(OCI_MsgGetID(*this, static_cast<AnyPointer>(buffer.data()), &size));

    return core::MakeRaw(buffer.data(), size);
}

inline OCI_Int32 Message::GetExpiration() const
{
    return core::Check(OCI_MsgGetExpiration(*this));
}

inline void Message::SetExpiration(OCI_Int32 value)
{
    core::Check(OCI_MsgSetExpiration(*this, value));
}

inline OCI_Int32 Message::GetEnqueueDelay() const
{
    return core::Check(OCI_MsgGetEnqueueDelay(*this));
}

inline void Message::SetEnqueueDelay(OCI_Int32 value)
{
    core::Check(OCI_MsgSetEnqueueDelay(*this, value));
}

inline OCI_Int32 Message::GetPriority() const
{
    return core::Check(OCI_MsgGetPriority(*this));
}

inline void Message::SetPriority(OCI_Int32 value)
{
    core::Check(OCI_MsgSetPriority(*this, value));
}

inline Raw Message::GetOriginalID() const
{
    OCI_UInt32 size = OCI_SIZE_BUFFER;

    std::vector<unsigned char> buffer(static_cast<OCI_Size>(size + 1));

    core::Check(OCI_MsgGetOriginalID(*this, static_cast<AnyPointer>(buffer.data()), &size));

    return core::MakeRaw(buffer.data(), size);
}

inline void Message::SetOriginalID(const Raw &value)
{
    const AnyPointer data = value.empty() ? nullptr : static_cast<AnyPointer>(const_cast<Raw::value_type*>(&value[0])) ;

    core::Check(OCI_MsgSetOriginalID(*this, data, static_cast<OCI_UInt32>(value.size())));
}

inline ostring Message::GetCorrelation() const
{
    return core::MakeString(core::Check(OCI_MsgGetCorrelation(*this)));
}

inline void Message::SetCorrelation(const ostring& value)
{
    core::Check(OCI_MsgSetCorrelation(*this, value.c_str()));
}

inline ostring Message::GetExceptionQueue() const
{
    return core::MakeString(core::Check(OCI_MsgGetExceptionQueue(*this)));
}

inline void Message::SetExceptionQueue(const ostring& value)
{
    core::Check(OCI_MsgSetExceptionQueue(*this, value.c_str()));
}

inline Agent Message::GetSender() const
{
    return Agent(core::Check(OCI_MsgGetSender(*this)), nullptr);
}

inline void Message::SetSender(const Agent &agent)
{
    core::Check(OCI_MsgSetSender(*this, agent));
}

inline void Message::SetConsumers(std::vector<Agent> &agents)
{
    const OCI_Size size = agents.size();
    std::vector<OCI_Agent*> buffer(size);

    for (OCI_Size i = 0; i < size; ++i)
    {
        buffer[i] = static_cast<const Agent &>(agents[i]);
    }

    core::Check(OCI_MsgSetConsumers(*this, buffer.data(), static_cast<OCI_UInt32>(size)));
}

}
