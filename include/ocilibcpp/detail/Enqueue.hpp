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

// ReSharper disable CppClangTidyMiscMisplacedConst

namespace ocilib
{

inline Enqueue::Enqueue(const TypeInfo &typeInfo, const ostring& queueName)
{
    Connection connection = typeInfo.GetConnection();
    
    AcquireAllocated
    (
        core::Check(OCI_EnqueueCreate(typeInfo, queueName.c_str())), 
        connection.GetHandle()
    );
}

inline void Enqueue::Put(const Message &message)
{
    core::Check(OCI_EnqueuePut(*this, message));
}

inline Enqueue::EnqueueVisibility Enqueue::GetVisibility() const
{
    return EnqueueVisibility(static_cast<EnqueueVisibility::Type>(core::Check(OCI_EnqueueGetVisibility(*this))));
}

inline void Enqueue::SetVisibility(EnqueueVisibility value)
{
    core::Check(OCI_EnqueueSetVisibility(*this, value));
}

inline Enqueue::EnqueueMode Enqueue::GetMode() const
{
    return EnqueueMode(static_cast<EnqueueMode::Type>(core::Check(OCI_EnqueueGetSequenceDeviation(*this))));
}

inline void Enqueue::SetMode(EnqueueMode value)
{
    core::Check(OCI_EnqueueSetSequenceDeviation(*this, value));
}

inline Raw Enqueue::GetRelativeMsgID() const
{
    unsigned int size = OCI_SIZE_BUFFER;

    core::ManagedBuffer<unsigned char> buffer(static_cast<size_t>(size + 1));

    core::Check(OCI_EnqueueGetRelativeMsgID(*this, static_cast<AnyPointer>(buffer), &size));

    return core::MakeRaw(buffer, size);
}

inline void Enqueue::SetRelativeMsgID(const Raw &value)
{
    const AnyPointer data = value.empty() ? nullptr : static_cast<AnyPointer>(const_cast<Raw::value_type*>(&value[0])) ;

    core::Check(OCI_EnqueueSetRelativeMsgID(*this, data, static_cast<unsigned int>(value.size())));
}

}
