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

#include "ocilibcpp/core.hpp"

namespace ocilib
{
    namespace core
    {
        inline HandleStore::HandleStore(SynchronizationGuard* guard)
        {
            _handles.SetGuard(guard);
        }

        template <class T>
        T HandleStore::Get(AnyPointer ptr)
        {
            return dynamic_cast<T>(_handles.Get(ptr));
        }

        template <class T>
        void HandleStore::Set(AnyPointer ptr, T handle)
        {
            if (handle)
            {
                _handles.Set(ptr, handle);
            }
            else
            {
                _handles.Remove(ptr);
            }
        }

        inline HandleStore& HandleStore::GetStoreForHandle(Handle* handle)
        {
            HandleStore* store = handle ? handle->GetStore() : nullptr;
            
            return store ? *store : GetDefaultStore();
        }

        inline HandleStore& HandleStore::GetDefaultStore()
        {
            static SynchronizationGuard guard(SynchronizationMode::Unsafe);
            static HandleStore store(&guard);

            return store;
        }

    }
}