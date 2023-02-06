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

#include "ocilibcpp/types.hpp"

namespace ocilib
{

inline ThreadHandle Thread::Create()
{
    return core::Check(OCI_ThreadCreate());
}

inline void Thread::Destroy(ThreadHandle handle)
{
    core::Check(OCI_ThreadFree(handle));
}

inline void Thread::Run(ThreadHandle handle, ThreadProc func, AnyPointer arg)
{
    core::Check(OCI_ThreadRun(handle, func, arg));
}

inline void Thread::Join(ThreadHandle handle)
{
    core::Check(OCI_ThreadJoin(handle));
}

inline ThreadId Thread::GetThreadId(ThreadHandle handle)
{
    return core::Check(OCI_HandleGetThreadID(handle));
}

}
