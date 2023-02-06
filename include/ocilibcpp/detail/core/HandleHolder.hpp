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

// ReSharper disable CppClangTidyBugproneUnhandledSelfAssignment

namespace ocilib
{
    namespace core
    {
        template<class T>
        HandleHolder<T>::HandleHolder() : _smartHandle(nullptr)
        {
        }

        template<class T>
        HandleHolder<T>::HandleHolder(const HandleHolder& other) : _smartHandle(nullptr)
        {
            AcquireTransient(other, other._smartHandle ? other._smartHandle->GetParent() : nullptr);
        }

        template<class T>
        HandleHolder<T>::~HandleHolder() noexcept
        {
            SILENT_CATCH(Release())
        }

        template<class T>
        HandleHolder<T>& HandleHolder<T>::operator= (const HandleHolder<T>& other) noexcept
        {
            if (this != &other)
            {
                Handle* parent = other._smartHandle ? other._smartHandle->GetParent() : nullptr;
                SILENT_CATCH(AcquireTransient(other, parent))
            }
            return *this;
        }

        template<class T>
        bool HandleHolder<T>::IsNull() const
        {
            return (static_cast<T>(*this) == 0);
        }

        template<class T>
        HandleHolder<T>::operator T()
        {
            return _smartHandle ? _smartHandle->GetHandle() : nullptr;
        }

        template<class T>
        HandleHolder<T>::operator T() const
        {
            return _smartHandle ? _smartHandle->GetHandle() : nullptr;
        }

        template<class T>
        HandleHolder<T>::operator bool()
        {
            return !IsNull();
        }

        template<class T>
        HandleHolder<T>::operator bool() const
        {
            return !IsNull();
        }

        template<class T>
        Handle* HandleHolder<T>::GetHandle() const
        {
            return static_cast<Handle*>(_smartHandle);
        }

        template<class T>
        void HandleHolder<T>::AcquireAllocated(T handle, Handle* parent)
        {
            Acquire(handle, true, nullptr, parent);
        }
  
        template<class T>
        void HandleHolder<T>::AcquireTransient(T handle, Handle* parent)
        {
            Acquire(handle, false, nullptr, parent);
        }
  
        template<class T>
        void HandleHolder<T>::AcquireAllocatedWithNotification(T handle, Handle* parent, SmartHandleFreeNotifyFunc freeNotifyFunc)
        {
            Acquire(handle, true, freeNotifyFunc, parent);
        }

        template<class T>
        void HandleHolder<T>::Acquire(T handle, bool allocated, SmartHandleFreeNotifyFunc freeNotifyFunc, Handle* parent)
        {
            if (_smartHandle && _smartHandle->GetHandle() == handle)
            {
                return;
            }

            Release();

            if (handle)
            {
                _smartHandle = HandleStore::GetStoreForHandle(parent).Get<SmartHandle*>(handle);

                if (!_smartHandle)
                {
                    _smartHandle = OnAllocate(new SmartHandle(this, handle, allocated, freeNotifyFunc, parent));
                }
                else
                {
                    _smartHandle->Acquire(this);
                }
            }
        }

        template<class T>
        void HandleHolder<T>::Acquire(HandleHolder<T>& other)
        {
            if (&other != this && _smartHandle != other._smartHandle)
            {
                Release();

                if (other._smartHandle)
                {
                    other._smartHandle->Acquire(this);
                    _smartHandle = other._smartHandle;
                }
            }
        }

        template<class T>
        void HandleHolder<T>::Release()
        {
            if (_smartHandle)
            {
                _smartHandle->Release(this);
            }

            _smartHandle = nullptr;
        }
    }
}
