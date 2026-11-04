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

// ReSharper disable CppClangTidyHicppUseEqualsDefault
// ReSharper disable CppClangTidyModernizeUseEqualsDefault

namespace ocilib
{

inline ShardingKey::ShardingKey(Environment& env)
{
    AcquireAllocated
    (
        core::Check(OCI_ShardingKeyCreate(env.GetHandle())),
        env.GetEnvironmentHandle()
    );
}

template<class T>
inline void ShardingKey::AddColumn(const T& value)
{
    using U = std::decay_t<T>;

    if constexpr      (std::is_same_v<U, ostring>)
    {
        core::Check(OCI_ShardingKeyAddString(*this, value.c_str()));
    }
    else if constexpr (std::is_same_v<U, Number>)
    {
        core::Check(OCI_ShardingKeyAddNumber(*this, value));
    }
    else if constexpr (std::is_same_v<U, Date>)
    {
        core::Check(OCI_ShardingKeyAddDate(*this, value));
    }
    else if constexpr (std::is_same_v<U, Timestamp>)
    {
        core::Check(OCI_ShardingKeyAddTimestamp(*this, value));
    }
    else if constexpr (std::is_same_v<U, std::vector<unsigned char>>)
    {
        core::Check(OCI_ShardingKeyAddRaw(*this, value.data(), static_cast<OCI_UInt32>(value.size())));
    }
    else
    {
        static_assert(!sizeof(U*), "Unsupported sharding key column type");
    }
}

inline void ShardingKey::Reset()
{
    core::Check(OCI_ShardingKeyReset(*this));
}

inline ostring ShardingKey::ToString() const
{
    return core::MakeString(core::Check(OCI_ShardingKeyToText(*this)));
}

}
