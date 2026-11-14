/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2026 Vincent ROGIER <vince.rogier@gmail.com>
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

#include "ocilibcpp/detail/core/HandleStore.hpp"
#include "ocilibcpp/detail/core/SmartHandle.hpp"

namespace ocilib
{
    namespace core
    {
        template<class T>
        bool HandleHolder<T>::IsNull() const
        {
            return !_control || _control->GetHandle() == nullptr;
        }

        template<class T>
        HandleHolder<T>::operator T()
        {
            return _control ? _control->GetHandle() : nullptr;
        }

        template<class T>
        HandleHolder<T>::operator T() const
        {
            return _control ? _control->GetHandle() : nullptr;
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
        OCI_Environment* HandleHolder<T>::GetEnvironment() const
        {
            return _control ? _control->GetEnvironment() : nullptr;
        }

        template<class T>
        HandleControlBase* HandleHolder<T>::GetHandle() const
        {
            return _control.get();
        }

        template<class T>
        HandleControlBase* HandleHolder<T>::GetHandleControl() const
        {
            return _control.get();
        }

        template<class T>
        void HandleHolder<T>::AcquireAllocated(T handle, Handle* parent)
        {
            std::shared_ptr<HandleStore> store = HandleStore::ResolveStore(parent, static_cast<AnyPointer>(handle));

            Acquire(handle, true, parent, store);
        }

        template<class T>
        void HandleHolder<T>::AcquireTransient(T handle, Handle* parent)
        {
            std::shared_ptr<HandleStore> store = HandleStore::ResolveStore(parent, static_cast<AnyPointer>(handle));

            Acquire(handle, false, parent, store);
        }

        template<class T>
        void HandleHolder<T>::AcquireAllocatedWithNotification(T handle, Handle* parent, FreeNotifyFunc freeNotify)
        {
            std::shared_ptr<HandleStore> store = HandleStore::ResolveStore(parent, static_cast<AnyPointer>(handle));

            Acquire(handle, true, parent, store, freeNotify);
        }

        template<class T>
        void HandleHolder<T>::Acquire(HandleHolder<T>& other)
        {
            if (&other != this)
            {
                Release();

                _control = other._control;
            }
        }

        template<class T>
        void HandleHolder<T>::Acquire(T handle, bool allocated, Handle* parent, std::shared_ptr<HandleStore> store, FreeNotifyFunc freeNotify,
                                      AnyPointer extraInfo)
        {
            if (_control && _control->GetHandle() == handle)
            {
                return;
            }

            Release();

            if (!handle)
            {
                return;
            }

            if (store)
            {
                _control = store->Find<T>(handle);
            }

            if (_control)
            {
                return;
            }

            _control = std::make_shared<HandleControl<T> >(handle, allocated, std::weak_ptr<HandleStore>(store), freeNotify, extraInfo);

            if (store)
            {
                store->Register(handle, _control);
            }

            if (parent)
            {
                parent->AddChild(_control);
            }
        }

        template<class T>
        inline HandleControlBase* HandleHolder<T>::GetParent() const
        {
            return _control ? _control->GetParent() : nullptr;
        }

        template<class T>
        void HandleHolder<T>::Release()
        {
            _control.reset();
        }
    }
}
