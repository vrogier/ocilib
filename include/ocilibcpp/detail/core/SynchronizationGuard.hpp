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
        inline SynchronizationGuard::SynchronizationGuard(SynchronizationMode mode) : _mutex(nullptr)
        {
            SetMode(mode);
        }

        inline SynchronizationGuard::~SynchronizationGuard() noexcept
        {
            SILENT_CATCH(SetMode(SynchronizationMode::Unsafe));
        }

        inline void SynchronizationGuard::SetMode(SynchronizationMode mode)
        {
            if (SynchronizationMode::Safe == mode && !_mutex)
            {
                _mutex = Mutex::Create();
            }
            else if (SynchronizationMode::Unsafe == mode && _mutex)
            {
                Mutex::Destroy(_mutex);
                _mutex = nullptr;
            }
        }

        inline void SynchronizationGuard::Acquire() const
        {
            if (_mutex)
            {
                Mutex::Acquire(_mutex);
            }
        }

        inline void SynchronizationGuard::Release() const
        {
            if (_mutex)
            {
                Mutex::Release(_mutex);
            }
        }

    }
}