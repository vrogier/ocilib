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

// ReSharper disable CppClangTidyHicppUseEqualsDefault
// ReSharper disable CppClangTidyModernizeUseEqualsDefault

namespace ocilib
{
    namespace core
    {
        template<class K, class V>
        ConcurrentMap<K, V>::ConcurrentMap()
        {

        }

        template<class K, class V>
        ConcurrentMap<K, V>::~ConcurrentMap() noexcept
        {
            SILENT_CATCH(Clear());
        }

        template<class K, class V>
        void ConcurrentMap<K, V>::Remove(K key)
        {
            Acquire();
            _map.erase(key);
            Release();
        }

        template<class K, class V>
        V ConcurrentMap<K, V>::Get(K key)
        {
            V value = 0;

            Acquire();
            typename std::map< K, V >::const_iterator it = _map.find(key);
            if (it != _map.end())
            {
                value = it->second;
            }
            Release();

            return value;
        }

        template<class K, class V>
        void ConcurrentMap<K, V>::Set(K key, V value)
        {
            Acquire();
            _map[key] = value;
            Release();
        }

        template<class K, class V>
        void ConcurrentMap<K, V>::Clear()
        {
            Acquire();
            _map.clear();
            Release();
        }

        template<class K, class V>
        size_t ConcurrentMap<K, V>::GetSize()
        {
            Acquire();
            const size_t size = _map.size();
            Release();

            return size;
        }

    }
}