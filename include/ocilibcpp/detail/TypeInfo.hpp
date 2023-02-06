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

inline TypeInfo::TypeInfo(const Connection &connection, const ostring& name, TypeInfoType type)
{
    AcquireTransient
    (
        core::Check(OCI_TypeInfoGet(connection, name.c_str(), type)), 
        connection.GetHandle()
    );
}

inline TypeInfo::TypeInfo(OCI_TypeInfo *pTypeInfo, core::Handle* parent)
{    
    AcquireTransient(pTypeInfo, parent);
}

inline TypeInfo::TypeInfoType TypeInfo::GetType() const
{
    return TypeInfoType(static_cast<TypeInfoType::Type>(core::Check(OCI_TypeInfoGetType(*this))));
}

inline ostring TypeInfo::GetName() const
{
    return core::Check(OCI_TypeInfoGetName(*this));
}

inline Connection TypeInfo::GetConnection() const
{
    return Connection
    (
        core::Check(OCI_TypeInfoGetConnection(*this)),
        Environment::GetEnvironmentHandle()
    );
}

inline unsigned int TypeInfo::GetColumnCount() const
{
    return core::Check(OCI_TypeInfoGetColumnCount(*this));
}

inline Column TypeInfo::GetColumn(unsigned int index) const
{
    return Column(core::Check(OCI_TypeInfoGetColumn(*this, index)), GetHandle());
}

inline boolean TypeInfo::IsFinalType() const
{
    return (core::Check(OCI_TypeInfoIsFinalType(*this)) == TRUE);
}

inline TypeInfo TypeInfo::GetSuperType() const
{
    Connection connection = GetConnection();

    return TypeInfo(core::Check(OCI_TypeInfoGetSuperType(*this)), connection.GetHandle());
}

}
