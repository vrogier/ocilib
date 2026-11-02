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

#include "ocilibcpp/core.hpp"

namespace ocilib
{
    namespace core
    {
        inline HandleStore::HandleStore()
        {
        }

        inline void HandleStore::Register(AnyPointer handle, std::shared_ptr<HandleControlBase> ctrl)
        {
            std::lock_guard<std::mutex> lock(_mutex);

            _map[handle] = ctrl;
        }

        inline void HandleStore::Remove(AnyPointer handle, const HandleControlBase* ctrl)
        {
            std::lock_guard<std::mutex> lock(_mutex);

            auto it = _map.find(handle);

            if (it != _map.end())
            {
                auto base = it->second.lock();

                if (base.get() == ctrl)
                {
                    _map.erase(it);
                }
            }
        }

    }
}
