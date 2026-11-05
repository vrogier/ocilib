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
// ReSharper disable CppClangTidyModernizeUseAuto

namespace ocilib
{

    inline SodaOptions::SodaOptions()
    {
    }

    inline SodaOptions::SodaOptions(const SodaDb& sdb)
    {
        AcquireAllocated(core::Check(OCI_SodaOptionsCreate(sdb)), sdb.GetHandle());
    }

    inline void SodaOptions::SetKey(const String& key)
    {
        core::Check(OCI_SodaOptionsSetKey(*this, key.c_str()));
    }

    inline void SodaOptions::SetKeys(const std::vector<String>& keys)
    {
        std::vector<const otext*> ptrs(keys.size());

        for (Size i = 0; i < keys.size(); ++i)
        {
            ptrs[i] = keys[i].c_str();
        }

        core::Check(OCI_SodaOptionsSetKeys(*this, ptrs.data(), static_cast<UInt32>(keys.size())));
    }

    inline void SodaOptions::SetVersion(const String& version)
    {
        core::Check(OCI_SodaOptionsSetVersion(*this, version.c_str()));
    }

    inline void SodaOptions::SetFilter(const String& filter)
    {
        core::Check(OCI_SodaOptionsSetFilter(*this, filter.c_str()));
    }

    inline void SodaOptions::SetSkip(UInt32 count)
    {
        core::Check(OCI_SodaOptionsSetSkip(*this, count));
    }

    inline void SodaOptions::SetLimit(UInt32 count)
    {
        core::Check(OCI_SodaOptionsSetLimit(*this, count));
    }

    inline void SodaOptions::SetFetchArraySize(UInt32 count)
    {
        core::Check(OCI_SodaOptionsSetFetchArraySize(*this, count));
    }

    inline void SodaOptions::SetHint(const String& hint)
    {
        core::Check(OCI_SodaOptionsSetHint(*this, hint.c_str()));
    }

    inline void SodaOptions::SetLock(bool lock)
    {
        core::Check(OCI_SodaOptionsSetLock(*this, lock ? TRUE : FALSE));
    }

}
