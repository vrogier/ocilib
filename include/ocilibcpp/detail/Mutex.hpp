/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2025 Vincent ROGIER <vince.rogier@ocilib.net>
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

#include "ocilibcpp/types.hpp"

namespace ocilib
{

inline MutexHandle Mutex::Create()
{
    return Environment::GetInstance()->Initialized() ? core::Check(OCI_MutexCreate()) : nullptr;
}

inline void Mutex::Destroy(MutexHandle handle)
{
    core::Check(OCI_MutexFree(handle));
}

inline void Mutex::Acquire(MutexHandle handle)
{
    core::Check(OCI_MutexAcquire(handle));
}

inline void Mutex::Release(MutexHandle handle)
{
    core::Check(OCI_MutexRelease(handle));
}

}
