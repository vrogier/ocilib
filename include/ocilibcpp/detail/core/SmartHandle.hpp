/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2021 Vincent ROGIER <vince.rogier@ocilib.net>
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

        template<class T>
        HandleHolder<T>::SmartHandle::SmartHandle
        (
            HandleHolder* holder, T handle, HandleFreeFunc handleFreefunc,
            SmartHandleFreeNotifyFunc freeNotifyFunc, Handle* parent
        )
            : _holders(), _handle(handle), _handleFreeFunc(handleFreefunc),
            _freeNotifyFunc(freeNotifyFunc), _parent(parent), _extraInfo(nullptr)
        {
            _locker.SetAccessMode((Environment::GetMode() & Environment::Threaded) == Environment::Threaded);

            _holders.SetLocker(&_locker);
            _children.SetLocker(&_locker);

            Environment::SetSmartHandle<SmartHandle*>(handle, this);

            Acquire(holder);

            if (_parent && _handle)
            {
                _parent->GetChildren().Add(this);
            }
        }

        template<class T>
        HandleHolder<T>::SmartHandle::~SmartHandle() noexcept
        {
            SILENT_CATCH((Destroy()))
        }

        template<class T>
        void HandleHolder<T>::SmartHandle::Destroy()
        {
            if (_parent && _handle)
            {
                _parent->GetChildren().Remove(this);
            }

            _children.ForEach(DeleteHandle);
            _children.Clear();

            _holders.SetLocker(nullptr);
            _children.SetLocker(nullptr);

            Environment::SetSmartHandle<SmartHandle*>(_handle, nullptr);

            if (_freeNotifyFunc)
            {
                _freeNotifyFunc(this);
            }

            if (_handleFreeFunc && _handle)
            {
                _handleFreeFunc(_handle);
            }
        }

        template<class T>
        void HandleHolder<T>::SmartHandle::DeleteHandle(Handle* handle)
        {
            if (handle)
            {
                handle->DetachFromParent();
                handle->DetachFromHolders();

                delete core::OnDeallocate(handle);
            }
        }

        template<class T>
        void HandleHolder<T>::SmartHandle::ResetHolder(HandleHolder* holder)
        {
            if (holder)
            {
                holder->_smartHandle = nullptr;
            }
        }

        template<class T>
        void HandleHolder<T>::SmartHandle::Acquire(HandleHolder* holder)
        {
            _holders.Add(holder);
        }

        template<class T>
        void HandleHolder<T>::SmartHandle::Release(HandleHolder* holder)
        {
            _holders.Remove(holder);

            if (_holders.GetSize() == 0)
            {
                delete core::OnDeallocate(this);
            }

            holder->_smartHandle = nullptr;
        }

        template<class T>
        T HandleHolder<T>::SmartHandle::GetHandle() const
        {
            return _handle;
        }

        template<class T>
        Handle* HandleHolder<T>::SmartHandle::GetParent() const
        {
            return _parent;
        }

        template<class T>
        AnyPointer HandleHolder<T>::SmartHandle::GetExtraInfos() const
        {
            return _extraInfo;
        }

        template<class T>
        void HandleHolder<T>::SmartHandle::SetExtraInfos(AnyPointer extraInfo)
        {
            _extraInfo = extraInfo;
        }

        template<class T>
        ConcurrentList<Handle*>& HandleHolder<T>::SmartHandle::GetChildren()
        {
            return _children;
        }

        template<class T>
        void HandleHolder<T>::SmartHandle::DetachFromHolders()
        {
            _holders.ForEach(ResetHolder);
            _holders.Clear();
        }

        template<class T>
        void HandleHolder<T>::SmartHandle::DetachFromParent()
        {
            _parent = nullptr;
        }

    }
}