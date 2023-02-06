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

inline BindInfo::BindInfo(OCI_Bind *pBind, core::Handle *parent)
{
    AcquireTransient(pBind, parent);
}

inline ostring BindInfo::GetName() const
{
    return core::MakeString(core::Check(OCI_BindGetName(*this)));
}

inline DataType BindInfo::GetType() const
{
    return DataType(static_cast<DataType::Type>(core::Check(OCI_BindGetType(*this))));
}

inline unsigned int BindInfo::GetSubType() const
{
    return core::Check(OCI_BindGetSubtype(*this));
}

inline unsigned int BindInfo::GetDataCount() const
{
    return core::Check(OCI_BindGetDataCount(*this));
}

inline Statement BindInfo::GetStatement() const
{
    return Statement(core::Check(OCI_BindGetStatement(*this)));
}

inline void BindInfo::SetDataNull(bool value, unsigned int index)
{
    if (value)
    {
        core::Check(OCI_BindSetNullAtPos(*this, index));
    }
    else
    {
        core::Check(OCI_BindSetNotNullAtPos(*this, index));
    }
}

inline bool BindInfo::IsDataNull(unsigned int index) const
{
    return (core::Check(OCI_BindIsNullAtPos(*this, index)) == TRUE);
}

inline void BindInfo::SetCharsetForm(CharsetForm value)
{
    core::Check(OCI_BindSetCharsetForm(*this, value));
}

inline BindInfo::BindDirection BindInfo::GetDirection() const
{
    return BindDirection(static_cast<BindDirection::Type>(core::Check(OCI_BindGetDirection(*this))));
}

}
