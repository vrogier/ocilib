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

#pragma once

#include "ocilibcpp/types.hpp"

namespace ocilib
{

    inline Message::Message(const TypeInfo &typeInfo)
    {
        Connection connection = typeInfo.GetConnection();

        AcquireAllocated
        (
            core::Check(OCI_MessageCreate(typeInfo)),
            connection.GetHandle()
        );
    }

    inline Message::Message(OCI_Message *pMessage, core::Handle *parent)
    {
        AcquireTransient(pMessage, parent);
    }

    inline void Message::Reset()
    {
        core::Check(OCI_MessageReset(*this));
    }

    template<class T>
    inline T Message::GetPayload()
    {
        if constexpr (std::is_same_v<T, Object>)
        {
            return Object(core::Check(OCI_MessageGetObject(*this)), nullptr);
        }
        else if constexpr (std::is_same_v<T, Raw>)
        {
            UInt32 size = 0;

            core::Check(OCI_MessageGetRaw(*this, nullptr, &size));

            std::vector<unsigned char> buffer(static_cast<Size>(size + 1));

            core::Check(OCI_MessageGetRaw(*this, static_cast<AnyPointer>(buffer.data()), &size));

            return core::MakeRaw(buffer.data(), size);
        }
        else
        {
            static_assert(!sizeof(T), "unsupported payload type");
        }
    }

    template<class T>
    inline void Message::SetPayload(const T &value)
    {
        if constexpr (std::is_same_v<T, Object>)
        {
            core::Check(OCI_MessageSetObject(*this, value));
        }
        else if constexpr (std::is_same_v<T, Raw>)
        {
            const AnyPointer data = value.empty() ? nullptr : static_cast<AnyPointer>(const_cast<Raw::value_type*>(&value[0]));

            core::Check(OCI_MessageSetRaw(*this, data, static_cast<UInt32>(value.size())));
        }
        else
        {
            static_assert(!sizeof(T), "unsupported payload type");
        }
    }

    inline Date Message::GetEnqueueTime() const
    {
        return Date(core::Check(OCI_MessageGetEnqueueTime(*this)), nullptr);
    }

    inline Int32 Message::GetAttemptCount() const
    {
        return core::Check(OCI_MessageGetAttemptCount(*this));
    }

    inline Message::MessageState Message::GetState() const
    {
        return MessageState(static_cast<MessageState::Type>(core::Check(OCI_MessageGetState(*this))));
    }

    inline Raw Message::GetID() const
    {
        UInt32 size = OCI_SIZE_BUFFER;

        std::vector<unsigned char> buffer(static_cast<Size>(size + 1));

        core::Check(OCI_MessageGetID(*this, static_cast<AnyPointer>(buffer.data()), &size));

        return core::MakeRaw(buffer.data(), size);
    }

    inline Int32 Message::GetExpiration() const
    {
        return core::Check(OCI_MessageGetExpiration(*this));
    }

    inline void Message::SetExpiration(Int32 value)
    {
        core::Check(OCI_MessageSetExpiration(*this, value));
    }

    inline Int32 Message::GetEnqueueDelay() const
    {
        return core::Check(OCI_MessageGetEnqueueDelay(*this));
    }

    inline void Message::SetEnqueueDelay(Int32 value)
    {
        core::Check(OCI_MessageSetEnqueueDelay(*this, value));
    }

    inline Int32 Message::GetPriority() const
    {
        return core::Check(OCI_MessageGetPriority(*this));
    }

    inline void Message::SetPriority(Int32 value)
    {
        core::Check(OCI_MessageSetPriority(*this, value));
    }

    inline Raw Message::GetOriginalID() const
    {
        UInt32 size = OCI_SIZE_BUFFER;

        std::vector<unsigned char> buffer(static_cast<Size>(size + 1));

        core::Check(OCI_MessageGetOriginalID(*this, static_cast<AnyPointer>(buffer.data()), &size));

        return core::MakeRaw(buffer.data(), size);
    }

    inline void Message::SetOriginalID(const Raw &value)
    {
        const AnyPointer data = value.empty() ? nullptr : static_cast<AnyPointer>(const_cast<Raw::value_type*>(&value[0]));

        core::Check(OCI_MessageSetOriginalID(*this, data, static_cast<UInt32>(value.size())));
    }

    inline String Message::GetCorrelation() const
    {
        return core::MakeString(core::Check(OCI_MessageGetCorrelation(*this)));
    }

    inline void Message::SetCorrelation(const String& value)
    {
        core::Check(OCI_MessageSetCorrelation(*this, value.c_str()));
    }

    inline String Message::GetExceptionQueue() const
    {
        return core::MakeString(core::Check(OCI_MessageGetExceptionQueue(*this)));
    }

    inline void Message::SetExceptionQueue(const String& value)
    {
        core::Check(OCI_MessageSetExceptionQueue(*this, value.c_str()));
    }

    inline Agent Message::GetSender() const
    {
        return Agent(core::Check(OCI_MessageGetSender(*this)), nullptr);
    }

    inline void Message::SetSender(const Agent &agent)
    {
        core::Check(OCI_MessageSetSender(*this, agent));
    }

    inline void Message::SetConsumers(std::vector<Agent> &agents)
    {
        const Size              size = agents.size();
        std::vector<OCI_Agent*> buffer(size);

        for (Size i = 0; i < size; ++i)
        {
            buffer[i] = static_cast<const Agent &>(agents[i]);
        }

        core::Check(OCI_MessageSetConsumers(*this, buffer.data(), static_cast<UInt32>(size)));
    }

}
