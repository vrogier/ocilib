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

inline Column::Column(OCI_Column *pColumn, core::Handle *parent)
{
    AcquireTransient(pColumn, parent);
}

inline ostring Column::GetName() const
{
    return core::MakeString(core::Check(OCI_ColumnGetName(*this)));
}

inline ostring Column::GetSQLType() const
{
    return core::MakeString(core::Check(OCI_ColumnGetSQLType(*this)));
}

inline ostring Column::GetFullSQLType() const
{
    const unsigned int size = OCI_SIZE_BUFFER;

    core::ManagedBuffer<otext> buffer(static_cast<size_t>(size + 1));

    core::Check(OCI_ColumnGetFullSQLType(*this, buffer, size));

    return core::MakeString(static_cast<const otext *>(buffer));
}

inline DataType Column::GetType() const
{
    return DataType(static_cast<DataType::Type>(core::Check(OCI_ColumnGetType(*this))));
}

inline unsigned int Column::GetSubType() const
{
    return core::Check(OCI_ColumnGetSubType(*this));
}

inline CharsetForm Column::GetCharsetForm() const
{
    return CharsetForm(static_cast<CharsetForm::Type>(core::Check(OCI_ColumnGetCharsetForm(*this))));
}

inline CollationID Column::GetCollationID() const
{
    return CollationID(static_cast<CollationID::Type>(core::Check(OCI_ColumnGetCollationID(*this))));
}

inline unsigned int Column::GetSize() const
{
    return core::Check(OCI_ColumnGetSize(*this));
}

inline int Column::GetScale() const
{
    return core::Check(OCI_ColumnGetScale(*this));
}

inline int Column::GetPrecision() const
{
    return core::Check(OCI_ColumnGetPrecision(*this));
}

inline int Column::GetFractionalPrecision() const
{
    return core::Check(OCI_ColumnGetFractionalPrecision(*this));
}

inline int Column::GetLeadingPrecision() const
{
    return core::Check(OCI_ColumnGetLeadingPrecision(*this));
}

inline Column::PropertyFlags Column::GetPropertyFlags() const
{
    return PropertyFlags(static_cast<PropertyFlags::Type>(core::Check(OCI_ColumnGetPropertyFlags(*this))));
}

inline bool Column::IsNullable() const
{
    return (core::Check(OCI_ColumnGetNullable(*this)) == TRUE);
}

inline bool Column::IsCharSemanticUsed() const
{
    return (core::Check(OCI_ColumnGetCharUsed(*this)) == TRUE);
}

inline TypeInfo Column::GetTypeInfo() const
{
    OCI_TypeInfo* typeInfo = core::Check(OCI_ColumnGetTypeInfo(*this));

    Connection connection
    (
        core::Check(OCI_TypeInfoGetConnection(typeInfo)),
        Environment::GetEnvironmentHandle()
    );

    return TypeInfo(typeInfo, connection.GetHandle());
}

}
