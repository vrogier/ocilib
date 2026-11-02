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

#include <algorithm>

#include "ocilibcpp/core.hpp"
#include "ocilibcpp/detail/core/HandleStore.hpp"
#include "ocilibcpp/detail/support/HandleDeleter.hpp"

namespace ocilib
{
    namespace core
    {
        inline HandleControlBase::HandleControlBase(std::weak_ptr<HandleStore> store, FreeNotifyFunc freeNotify, AnyPointer extraInfo)
            : _store(store), _freeNotify(freeNotify), _extraInfo(extraInfo)
        {
        }

        inline void HandleControlBase::AddChild(std::shared_ptr<HandleControlBase> child)
        {
            child->SetParent(this);

            std::lock_guard<std::mutex> lock(_childrenMutex);

            _children.insert_or_assign(child.get(), child);
        }

        inline void HandleControlBase::RemoveChild(HandleControlBase* child)
        {
            std::lock_guard<std::mutex> lock(_childrenMutex);

            _children.erase(child);
        }

        inline void HandleControlBase::FreeChildren()
        {
            std::vector<std::shared_ptr<HandleControlBase>> children;

            {
                std::lock_guard<std::mutex> lock(_childrenMutex);

                for (auto& kv : _children)
                {
                    auto c = kv.second.lock();
                    if (c)
                    {
                        children.push_back(c);
                    }
                }

                _children.clear();
            }

            for (auto& c : children)
            {
                c->Free();
            }
        }

        inline HandleControlBase* HandleControlBase::GetParent() const
        {
            return _parent.lock().get();
        }

        inline std::shared_ptr<HandleStore> HandleControlBase::GetStore() const
        {
            return _store.lock();
        }

        inline void HandleControlBase::SetParent(HandleControlBase* parent)
        {
            _parent = parent ? parent->shared_from_this() : std::shared_ptr<HandleControlBase>();
        }

        template<class T>
        inline HandleControl<T>::HandleControl(T handle, bool allocated, std::weak_ptr<HandleStore> store, FreeNotifyFunc freeNotify, AnyPointer extraInfo)
            : HandleControlBase(store, freeNotify, extraInfo), _handle(handle), _allocated(allocated)
        {
        }

        template<class T>
        inline HandleControl<T>::~HandleControl() noexcept
        {
            try
            {
                FreeChildren();

                if (_freeNotify)
                {
                    _freeNotify(this);
                }

                Free();
            }
            catch (...)
            {
            }
        }

        template<class T>
        inline void HandleControl<T>::Free()
        {
            T h = _handle.exchange(nullptr);

            if (h && _allocated)
            {
                OCI_Environment* env = OCI_HandleGetEnvironment(h);
                support::HandleDeleter<T> deleter;
                deleter(h, env);
            }

            if (auto store = _store.lock())
            {
                store->Remove(h, this);
            }
        }

        template<class T>
        inline T HandleControl<T>::GetHandle() const
        {
            return _handle.load();
        }

        template<class T>
        inline OCI_Environment* HandleControl<T>::GetEnvironment() const
        {
            T h = _handle.load();
            return h ? OCI_HandleGetEnvironment(h) : nullptr;
        }

        template<class T>
        inline std::shared_ptr<HandleControl<T>> HandleStore::Find(AnyPointer handle)
        {
            std::lock_guard<std::mutex> lock(_mutex);

            auto it = _map.find(handle);

            if (it == _map.end())
            {
                return nullptr;
            }

            auto base = it->second.lock();

            if (!base)
            {
                _map.erase(it);
                return nullptr;
            }

            return std::dynamic_pointer_cast<HandleControl<T>>(base);
        }

        inline void HandleControlBase::FreeChildren(const std::function<bool(HandleControlBase*)>& predicate)
        {
            std::vector<std::shared_ptr<HandleControlBase>> children;

            {
                std::lock_guard<std::mutex> lock(_childrenMutex);

                for (auto it = _children.begin(); it != _children.end();)
                {
                    auto c = it->second.lock();
                    if (!c)
                    {
                        it = _children.erase(it);
                    }
                    else if (predicate(c.get()))
                    {
                        children.push_back(c);
                        it = _children.erase(it);
                    }
                    else
                    {
                        ++it;
                    }
                }
            }

            for (auto& c : children)
            {
                c->Free();
            }
        }
    }
}
