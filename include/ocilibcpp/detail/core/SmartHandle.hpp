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
#include "ocilibcpp/detail/support/HandleStoreResolver.hpp"
#include "ocilibcpp/detail/support/HandleDeleter.hpp"

namespace ocilib
{
    namespace core
    {

        template<class T>
        HandleHolder<T>::SmartHandle::SmartHandle
        (
            HandleHolder* holder, T handle, bool allocated,
            SmartHandleFreeNotifyFunc freeNotifyFunc, Handle* parent
        )
            : _holders(), _handle(handle), _allocated(allocated),
            _freeNotifyFunc(freeNotifyFunc), _parent(parent), _extraInfo(nullptr), _store{nullptr},
            _guard(GetSynchronizationMode())
        {
            _holders.SetGuard(&_guard);
            _children.SetGuard(&_guard);

            if (support::HandleStoreResolver<T>::RequireStore)
            {
                _store = core::OnAllocate(new core::HandleStore(&_guard));
            }

            HandleStore::GetStoreForHandle(parent).Set<SmartHandle*>(handle, this);
            
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

            _holders.SetGuard(nullptr);
            _children.SetGuard(nullptr);

            HandleStore::GetStoreForHandle(_parent).Set<SmartHandle*>(_handle, nullptr);

            if (_freeNotifyFunc)
            {
                _freeNotifyFunc(this);
            }

            if (_handle && _allocated)
            {
                support::HandleDeleter<T> deleter;
                deleter(_handle);
            }

            if (_store)
            {
                delete core::OnDeallocate(_store);
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
        SynchronizationMode HandleHolder<T>::SmartHandle::GetSynchronizationMode()
        {  
            if ((Environment::GetMode() & Environment::Threaded) == Environment::Threaded)
            {
                return support::HandleStoreResolver<T>::SynchMode;
            }
           
            return SynchronizationMode::Unsafe;
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

        template<class T>    
        HandleStore* HandleHolder<T>::SmartHandle::GetStore()
        {
            return _store;
        }
    }
}