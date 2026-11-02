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
    
inline Dequeue::Dequeue(const TypeInfo &typeInfo, const ostring& queueName)
{
    Connection connection = typeInfo.GetConnection();

    AcquireAllocated
    (
        core::Check(OCI_DequeueCreate(typeInfo, queueName.c_str())), 
        connection.GetHandle()
    );
}

inline Dequeue::Dequeue(OCI_Dequeue *pDequeue, core::Handle* parent)
{
    AcquireTransient(pDequeue,parent);
}

inline Message Dequeue::Get()
{
    return Message(core::Check(OCI_DequeueGet(*this)), nullptr);
}

inline Agent Dequeue::Listen(Int32 timeout)
{
    return Agent(core::Check(OCI_DequeueListen(*this, timeout)), nullptr);
}

inline ostring Dequeue::GetConsumer() const
{
    return core::MakeString(core::Check(OCI_DequeueGetConsumer(*this)));
}

inline void Dequeue::SetConsumer(const ostring& value)
{
    core::Check(OCI_DequeueSetConsumer(*this, value.c_str()));
}

inline ostring Dequeue::GetCorrelation() const
{
    return core::MakeString(core::Check(OCI_DequeueGetCorrelation(*this)));
}

inline void Dequeue::SetCorrelation(const ostring& value)
{
    core::Check(OCI_DequeueSetCorrelation(*this, value.c_str()));
}

inline Raw Dequeue::GetRelativeMsgID() const
{
    UInt32 size = OCI_SIZE_BUFFER;

    std::vector<unsigned char> buffer(static_cast<Size>(size + 1));

    core::Check(OCI_DequeueGetRelativeMsgID(*this, static_cast<AnyPointer>(buffer.data()), &size));

    return core::MakeRaw(buffer.data(), size);
}

inline void Dequeue::SetRelativeMsgID(const Raw &value)
{
    const AnyPointer data = value.empty() ? nullptr : static_cast<AnyPointer>(const_cast<Raw::value_type*>(&value[0])) ;

    core::Check(OCI_DequeueSetRelativeMsgID(*this, data, static_cast<UInt32>(value.size())));
}

inline Dequeue::DequeueVisibility Dequeue::GetVisibility() const
{
    return DequeueVisibility(static_cast<DequeueVisibility::Type>(core::Check(OCI_DequeueGetVisibility(*this))));
}

inline void Dequeue::SetVisibility(DequeueVisibility value)
{
    core::Check(OCI_DequeueSetVisibility(*this, value));
}

inline Dequeue::DequeueMode Dequeue::GetMode() const
{
    return DequeueMode(static_cast<DequeueMode::Type>(core::Check(OCI_DequeueGetMode(*this))));
}

inline void Dequeue::SetMode(DequeueMode value)
{
    core::Check(OCI_DequeueSetMode(*this, value));
}

inline Dequeue::NavigationMode Dequeue::GetNavigation() const
{
    return NavigationMode(static_cast<NavigationMode::Type>(core::Check(OCI_DequeueGetNavigation(*this))));
}

inline void Dequeue::SetNavigation(NavigationMode value)
{
    core::Check(OCI_DequeueSetNavigation(*this, value));
}

inline Int32 Dequeue::GetWaitTime() const
{
    return core::Check(OCI_DequeueGetWaitTime(*this));
}

inline void Dequeue::SetWaitTime(Int32 value)
{
    core::Check(OCI_DequeueSetWaitTime(*this, value));
}

inline void Dequeue::SetAgents(std::vector<Agent> &agents)
{
    const Size size = agents.size();
    std::vector<OCI_Agent*> buffer(size);

    for (Size i = 0; i < size; ++i)
    {
        buffer[i] = static_cast<const Agent &>(agents[i]);
    }

    core::Check(OCI_DequeueSetAgentList(*this, buffer.data(), static_cast<UInt32>(size)));
}
inline void Dequeue::Subscribe(const Connection& connection, std::vector<Dequeue>& dequeues, UInt32 timeout, NotifyAQHandlerProc handler)
{
    const Size size = dequeues.size();
    std::vector<OCI_Dequeue*> handles(size);

    for (Size i = 0; i < size; ++i)
    {
        handles[i] = static_cast<OCI_Dequeue*>(dequeues[i]);
    }

    OCI_Environment* env = connection.GetEnvironment();

    core::Check(OCI_DequeueSubscribe(connection, handles.data(), static_cast<UInt32>(size), timeout,
                                          static_cast<POCI_NOTIFY_AQ>(handler != nullptr ? Environment::NotifyHandlerAQ : nullptr)));

    Environment* cpp_env = Environment::GetInstance(env);
    if (cpp_env)
    {
        for (Size i = 0; i < size; ++i)
        {
            cpp_env->SetUserCallback<NotifyAQHandlerProc>(handles[i], handler);
        }
    }
}
inline void Dequeue::Unsubscribe()
{
    core::Check(OCI_DequeueUnsubscribe(*this));
}

}
