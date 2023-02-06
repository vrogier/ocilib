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

// ReSharper disable CppClangTidyHicppUseEqualsDefault
// ReSharper disable CppClangTidyPerformanceUnnecessaryValueParam

namespace ocilib
{

inline Subscription::Subscription()
{

}

inline Subscription::Subscription(OCI_Subscription *pSubcription)
{
    AcquireTransient
    (
        pSubcription,
        Environment::GetEnvironmentHandle()
    );
}

inline void Subscription::Register(const Connection &connection, const ostring& name, ChangeTypes changeTypes, NotifyHandlerProc handler, unsigned int port, unsigned int timeout)
{
    AcquireAllocated
    (
        core::Check
        ( 
            OCI_SubscriptionRegister
            (
                connection, name.c_str(), changeTypes.GetValues(),
                static_cast<POCI_NOTIFY> (handler != nullptr ? Environment::NotifyHandler : nullptr), 
                port, timeout
            )
        ),
        Environment::GetEnvironmentHandle()
    );

    Environment::SetUserCallback<Subscription::NotifyHandlerProc>(static_cast<OCI_Subscription*>(*this), handler);
}

inline void Subscription::Unregister()
{
    Environment::SetUserCallback<Subscription::NotifyHandlerProc>(static_cast<OCI_Subscription*>(*this), nullptr);

    Release();
}

inline void Subscription::Watch(const ostring& sql)
{
    Statement st(GetConnection());

    st.Execute(sql);

    core::Check(OCI_SubscriptionAddStatement(*this, st));
}

inline ostring Subscription::GetName() const
{
    return core::MakeString(core::Check(OCI_SubscriptionGetName(*this)));
}

inline unsigned int Subscription::GetTimeout() const
{
    return core::Check(OCI_SubscriptionGetTimeout(*this));
}

inline unsigned int Subscription::GetPort() const
{
    return core::Check(OCI_SubscriptionGetPort(*this));
}

inline Connection Subscription::GetConnection() const
{
    return Connection
    (
        core::Check(OCI_SubscriptionGetConnection(*this)),
        Environment::GetEnvironmentHandle()
    );
}

}
