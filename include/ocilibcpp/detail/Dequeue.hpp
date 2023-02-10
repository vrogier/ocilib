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

inline Agent Dequeue::Listen(int timeout)
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
    unsigned int size = OCI_SIZE_BUFFER;

    core::ManagedBuffer<unsigned char> buffer(size + 1);

    core::Check(OCI_DequeueGetRelativeMsgID(*this, static_cast<AnyPointer>(buffer), &size));

    return core::MakeRaw(buffer, size);
}

inline void Dequeue::SetRelativeMsgID(const Raw &value)
{
    const AnyPointer data = value.empty() ? nullptr : static_cast<AnyPointer>(const_cast<Raw::value_type*>(&value[0])) ;

    core::Check(OCI_DequeueSetRelativeMsgID(*this, data, static_cast<unsigned int>(value.size())));
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

inline int Dequeue::GetWaitTime() const
{
    return core::Check(OCI_DequeueGetWaitTime(*this));
}

inline void Dequeue::SetWaitTime(int value)
{
    core::Check(OCI_DequeueSetWaitTime(*this, value));
}

inline void Dequeue::SetAgents(std::vector<Agent> &agents)
{
    const size_t size = agents.size();
    core::ManagedBuffer<OCI_Agent*> buffer(size);

    OCI_Agent ** pAgents = static_cast<OCI_Agent **>(buffer);

    for (size_t i = 0; i < size; ++i)
    {
        pAgents[i] = static_cast<const Agent &>(agents[i]);
    }

    core::Check(OCI_DequeueSetAgentList(*this, pAgents, static_cast<unsigned int>(size)));
}

inline void Dequeue::Subscribe(unsigned int port, unsigned int timeout, NotifyAQHandlerProc handler)
{
    core::Check(OCI_DequeueSubscribe(*this, port, timeout, static_cast<POCI_NOTIFY_AQ>(handler != nullptr ? Environment::NotifyHandlerAQ : nullptr)));

    Environment::SetUserCallback<NotifyAQHandlerProc>(static_cast<OCI_Dequeue*>(*this), handler);
}

inline void Dequeue::Unsubscribe()
{
    core::Check(OCI_DequeueUnsubscribe(*this));
}

}
