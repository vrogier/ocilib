/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2025 Vincent ROGIER <vince.rogier@ocilib.net>
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

#include "ocilibcpp/config.hpp"
#include "ocilibcpp/core.hpp"

namespace ocilib
{
    namespace support
    {
        /**
        * @brief Internal usage.
        */
        template<bool T, int U>
        struct HandleStoreResolverType
        {
            static const bool RequireStore = T;
            static const core::SynchronizationMode SynchMode = static_cast<core::SynchronizationMode>(U);
        };

        /**
        * @brief Internal usage.
        * Allow resolving information required creating a handle store for a given C Handle
        */
        template<class T>
        struct HandleStoreResolver : HandleStoreResolverType<false, core::SynchronizationMode::Unsafe> {};

        template<> struct HandleStoreResolver<OCI_Environment*> : HandleStoreResolverType<true, core::SynchronizationMode::Safe> {};
        template<> struct HandleStoreResolver<OCI_Connection*> : HandleStoreResolverType<true, core::SynchronizationMode::Unsafe> {};
        template<> struct HandleStoreResolver<OCI_Statement*> : HandleStoreResolverType<true, core::SynchronizationMode::Unsafe> {};
        template<> struct HandleStoreResolver<OCI_Resultset*> : HandleStoreResolverType<true, core::SynchronizationMode::Unsafe> {};
        template<> struct HandleStoreResolver<OCI_TypeInfo*> : HandleStoreResolverType<true, core::SynchronizationMode::Unsafe> {};
        template<> struct HandleStoreResolver<OCI_Object*> : HandleStoreResolverType<true, core::SynchronizationMode::Unsafe> {};
    }
}
