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

// ReSharper disable CppClangTidyHicppUseEqualsDefault
// ReSharper disable CppClangTidyModernizeUseEqualsDefault

namespace ocilib
{

inline Reference::Reference()
{
}

inline Reference::Reference(const TypeInfo &typeInfo)
{
    Connection connection = typeInfo.GetConnection();
    AcquireAllocated
    (
        core::Check(OCI_RefCreate(connection, typeInfo)),
        connection.GetHandle()
    );
}

inline Reference::Reference(OCI_Ref *pRef, core::Handle *parent)
{
    AcquireTransient(pRef, parent);
}

inline TypeInfo Reference::GetTypeInfo() const
{
    OCI_TypeInfo* typeInfo = core::Check(OCI_RefGetTypeInfo(*this));

    Connection connection
    (
        core::Check(OCI_TypeInfoGetConnection(typeInfo)),
        Environment::GetEnvironmentHandle()
    );

    return TypeInfo(typeInfo, connection.GetHandle());
}

inline Object Reference::GetObject() const
{
    return Object(core::Check(OCI_RefGetObject(*this)), GetHandle());
}

inline Reference Reference::Clone() const
{
    Reference result(GetTypeInfo());

    core::Check(OCI_RefAssign(result, *this));

    return result;
}

inline bool Reference::IsReferenceNull() const
{
    return (core::Check(OCI_RefIsNull(*this)) == TRUE);
}

inline void Reference::SetReferenceNull()
{
    core::Check(OCI_RefSetNull(*this));
}

inline ostring Reference::ToString() const
{
    if (!IsNull())
    {
        const unsigned int size = core::Check(OCI_RefGetHexSize(*this));

        core::ManagedBuffer<otext> buffer(static_cast<size_t>(size + 1));

        core::Check(OCI_RefToText(*this, size, buffer));

        return core::MakeString(static_cast<const otext *>(buffer), static_cast<int>(size));
    }

    return OCI_STRING_NULL;
}

}
