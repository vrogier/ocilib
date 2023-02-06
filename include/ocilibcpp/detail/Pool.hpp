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
// ReSharper disable CppClangTidyModernizeUseEqualsDefault
// ReSharper disable CppClangTidyPerformanceUnnecessaryValueParam

namespace ocilib
{

inline Pool::Pool()
{

}

inline Pool::Pool(const ostring& db, const ostring& user, const ostring& pwd, Pool::PoolType poolType,
        unsigned int minSize, unsigned int maxSize, unsigned int increment, Environment::SessionFlags sessionFlags)
{
    Open(db, user, pwd, poolType, minSize, maxSize, increment, sessionFlags);
}

inline void Pool::Open(const ostring& db, const ostring& user, const ostring& pwd, Pool::PoolType poolType,
            unsigned int minSize, unsigned int maxSize, unsigned int increment, Environment::SessionFlags sessionFlags)
{
    Release();

    AcquireAllocated
    (
        core::Check
        (
            OCI_PoolCreate(db.c_str(), user.c_str(), pwd.c_str(), poolType, sessionFlags.GetValues(),  minSize, maxSize, increment)
        ), 
        Environment::GetEnvironmentHandle()
    );
}

inline void Pool::Close()
{
    Release();
}

inline Connection Pool::GetConnection(const ostring& sessionTag)
{
    return Connection(core::Check(OCI_PoolGetConnection(*this, sessionTag.c_str())), GetHandle());
}

inline unsigned int Pool::GetTimeout() const
{
    return core::Check(OCI_PoolGetTimeout(*this));
}

inline void Pool::SetTimeout(unsigned int value)
{
    core::Check(OCI_PoolSetTimeout(*this, value));
}

inline bool Pool::GetNoWait() const
{
    return (core::Check(OCI_PoolGetNoWait(*this)) == TRUE);
}

inline void Pool::SetNoWait(bool value)
{
    core::Check(OCI_PoolSetNoWait(*this, value));
}

inline unsigned int Pool::GetBusyConnectionsCount() const
{
    return core::Check(OCI_PoolGetBusyCount(*this));
}

inline unsigned int Pool::GetOpenedConnectionsCount() const
{
    return core::Check(OCI_PoolGetOpenedCount(*this));
}

inline unsigned int Pool::GetMinSize() const
{
    return core::Check(OCI_PoolGetMin(*this));
}

inline unsigned int Pool::GetMaxSize() const
{
    return core::Check(OCI_PoolGetMax(*this));
}

inline unsigned int Pool::GetIncrement() const
{
    return core::Check(OCI_PoolGetIncrement(*this));
}

inline unsigned int Pool::GetStatementCacheSize() const
{
    return core::Check(OCI_PoolGetStatementCacheSize(*this));
}

inline void Pool::SetStatementCacheSize(unsigned int value)
{
    core::Check(OCI_PoolSetStatementCacheSize(*this, value));
}

}
