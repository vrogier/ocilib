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

template<class T, int U>
Long<T, U>::Long()
{
}

template<class T, int U>
Long<T, U>::Long(const Statement &statement)
{
    AcquireAllocated
    (
        core::Check(OCI_LongCreate(statement, U)),
        statement.GetHandle()
    );
}

template<class T, int U>
Long<T, U>::Long(OCI_Long *pLong, core::Handle* parent)
{
    AcquireTransient(pLong, parent);
}

template<class T, int U>
unsigned int Long<T, U>::Write(const T& content)
{
    if (content.empty())
    {
        return 0;
    }

    return core::Check(OCI_LongWrite(*this, static_cast<AnyPointer>(const_cast<typename T::value_type*>(&content[0])), static_cast<unsigned int>(content.size())));
}

template<class T, int U>
unsigned int Long<T, U>::GetLength() const
{
    return core::Check(OCI_LongGetSize(*this));
}

template<>
inline ostring Long<ostring, LongCharacter>::GetContent() const
{
    return core::MakeString(static_cast<const otext *>(core::Check(OCI_LongGetBuffer(*this))));
}

template<>
inline Raw Long<Raw, LongBinary>::GetContent() const
{
    return core::MakeRaw(core::Check(OCI_LongGetBuffer(*this)), GetLength());
}

}
