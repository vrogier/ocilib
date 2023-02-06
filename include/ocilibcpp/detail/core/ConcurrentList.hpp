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

#include "ocilibcpp/core.hpp"

// ReSharper disable CppClangTidyHicppUseEqualsDefault
// ReSharper disable CppClangTidyModernizeUseEqualsDefault
// ReSharper disable CppClangTidyHicppUseAuto
// ReSharper disable CppClangTidyModernizeUseAuto

namespace ocilib
{
    namespace core
    {
        template<class T>
        ConcurrentList<T>::ConcurrentList() : _list()
        {

        }

        template<class T>
        ConcurrentList<T>::~ConcurrentList() noexcept
        {
            SILENT_CATCH(Clear())
        }

        template<class T>
        void ConcurrentList<T>::Add(T value)
        {
            Acquire();
            _list.push_back(value);
            Release();
        }

        template<class T>
        void ConcurrentList<T>::Remove(T value)
        {
            Acquire();
            _list.remove(value);
            Release();
        }

        template<class T>
        void ConcurrentList<T>::Clear()
        {
            Acquire();
            _list.clear();
            Release();
        }

        template<class T>
        size_t ConcurrentList<T>::GetSize()
        {
            Acquire();
            const size_t size = _list.size();
            Release();

            return size;
        }

        template<class T>
        bool ConcurrentList<T>::Exists(const T& value)
        {
            Acquire();

            const bool res = std::find(_list.begin(), _list.end(), value) != _list.end();

            Release();

            return res;
        }

        template<class T>
        template<class P>
        bool ConcurrentList<T>::FindIf(P predicate, T& value)
        {
            bool res = false;

            Acquire();

            typename std::list<T>::iterator it = std::find_if(_list.begin(), _list.end(), predicate);

            if (it != _list.end())
            {
                value = *it;
                res = true;
            }

            Release();

            return res;
        }

        template<class T>
        template<class A>
        void ConcurrentList<T>::ForEach(A action)
        {
            Acquire();

            std::for_each(_list.begin(), _list.end(), action);

            Release();
        }

    }
}