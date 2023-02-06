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

#include <algorithm>

namespace ocilib
{
    namespace core
    {
#ifdef OCILIBPP_DEBUG_MEMORY_ENABLED

        struct MemoryAllocation
        {
            void* Address;
            const char* Name;
            size_t Size;
            size_t Count;
        };

        class MemoryDebugInfo
        {
        private:
            std::vector<MemoryAllocation> Allocations;
        public:

            template<class T>
            void OnAllocate(T* address, size_t count)
            {
                Allocations.push_back({ address, typeid(T).name(), sizeof(T), count });
            }

            template<class T>
            void OnDeallocate(T* address)
            {
                auto it = std::find_if(std::begin(Allocations), std::end(Allocations), [address](const auto& alloc)
                {
                    return alloc.Address == address;
                });
                if (it != std::end(Allocations))
                {
                    Allocations.erase(it);
                }
            }

            void PrintAllocations()
            {
                if (Allocations.empty()) return;

                std::cout << "Unfreed memory found" << std::endl;

                for (auto& alloc : Allocations)
                {
                    std::cout << "=> Address " << alloc.Address
                        << " - Size " << alloc.Count
                        << " - Count " << alloc.Size
                        << " - Type " << alloc.Name
                        << std::endl;
                }
            }
        };

        inline MemoryDebugInfo& GetMemoryDebugInfo()
        {
            static MemoryDebugInfo memoryDebugInfo;

            return memoryDebugInfo;
        }

#endif
    }
}
