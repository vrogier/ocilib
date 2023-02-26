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

inline XmlType::XmlType(OCI_XmlType *pXmlType, core::Handle *parent)
{
    AcquireTransient(pXmlType, parent);
}

inline ostring XmlType::ToString() const
{
    if (!IsNull())
    {
        unsigned int size{};
        
        core::Check(OCI_XmlTypeToText(*this, &size, nullptr));

        ostring result;

        result.resize(static_cast<size_t>(size));

        core::Check(OCI_XmlTypeToText(*this, &size , const_cast<otext*>(result.data())));

        return result;
    }

    return OCI_STRING_NULL;
}

}