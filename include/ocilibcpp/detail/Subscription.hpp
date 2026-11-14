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

    inline Subscription::Subscription()
    {

    }

    inline Subscription::Subscription(OCI_Subscription* pSubscription)
    {
        AcquireTransient(pSubscription, static_cast<core::Handle*>(nullptr));
    }

    inline void Subscription::Register(const Connection& connection, const String& name, ChangeTypes changeTypes, NotifyHandlerProc handler,
                                       SubscriptionMode mode, UInt32 timeout)
    {
        AcquireAllocated
        (
            core::Check
            (
                OCI_SubscriptionCreate
                (
                    connection, name.c_str(), changeTypes.GetValues(),
                    static_cast<POCI_NOTIFY> (handler != nullptr ? Environment::NotifyHandler : nullptr),
                    mode.GetValue(), timeout
                )
            ),
            connection.GetHandle()
        );

        Environment* env = Environment::GetInstance(connection.GetEnvironment());
        if (env)
        {
            env->SetUserCallback<Subscription::NotifyHandlerProc>(static_cast<OCI_Subscription*>(*this), handler);
        }
    }

    inline void Subscription::Unregister()
    {
        Environment* env = Environment::GetInstance(GetEnvironment());
        if (env)
        {
            env->SetUserCallback<Subscription::NotifyHandlerProc>(static_cast<OCI_Subscription*>(*this), nullptr);
        }

        Release();
    }

    inline void Subscription::Watch(const String& sql)
    {
        Connection connection = GetConnection();
        Statement  st(core::Check(OCI_StatementCreate(connection)), connection.GetHandle());

        st.Execute(sql);

        core::Check(OCI_SubscriptionAddStatement(*this, st));
    }

    inline String Subscription::GetName() const
    {
        return core::MakeString(core::Check(OCI_SubscriptionGetName(*this)));
    }

    inline UInt32 Subscription::GetTimeout() const
    {
        return core::Check(OCI_SubscriptionGetTimeout(*this));
    }

    inline Connection Subscription::GetConnection() const
    {
        core::Handle* selfHandle = this->GetHandle();
        core::Handle* parentHandle = selfHandle ? selfHandle->GetParent() : nullptr;
        core::Handle* connectionParent = parentHandle ? parentHandle->GetParent() : GetHandle();

        return Connection
               (
                   core::Check(OCI_SubscriptionGetConnection(*this)),
                   connectionParent,
                   false
               );
    }

}
