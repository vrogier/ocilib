/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2026 Vincent ROGIER <vince.rogier@ocilib.net>
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
    namespace support
    {
        template<class T> struct VectorFormatResolver {};

        template<> struct VectorFormatResolver<char> { enum { Value = Vector::Int8 }; };
        template<> struct VectorFormatResolver<float> { enum { Value = Vector::Float32 }; };
        template<> struct VectorFormatResolver<double> { enum { Value = Vector::Float64 }; };
        template<> struct VectorFormatResolver<unsigned char> { enum { Value = Vector::Binary }; };
    }


inline Vector::Vector()
{

}

inline Vector::Vector(const Connection& connection)
{
    AcquireAllocated
    (
        core::Check(OCI_VectorCreate(connection)),
        connection.GetHandle()
    );
}

inline Vector::Vector(OCI_Vector *pVector, core::Handle *parent)
{
    AcquireTransient(pVector, parent);
}

template<class T, typename core::SupportedVectorNumeric<T>::Type::type*>
void Vector::Set(T* values, OCI_UInt32 size)
{
    core::Check(OCI_VectorSetValues(*this, support::VectorFormatResolver<T>::Value, size, reinterpret_cast<void*>(values)));
}

template<class T, typename core::SupportedVectorNumeric<T>::Type::type*>
void Vector::Get(T* values)
{
    core::Check(OCI_VectorGetValues(*this, reinterpret_cast<void*>(values)));
}

template<class T, typename core::SupportedVectorNumeric<T>::Type::type*>
void Vector::Set(const std::vector<T>& values)
{
    Set(const_cast<T*>(values.data()), static_cast<OCI_UInt32>(values.size()));
}

template<class T, typename core::SupportedVectorNumeric<T>::Type::type*>
std::vector<T> Vector::Get()
{
    std::vector<T> values;
    values.resize(static_cast<OCI_Size>(GetDimensionCount()));

    Get(const_cast<T*>(values.data()));

    return values;
}

inline OCI_UInt32 Vector::GetDimensionCount()
{
    OCI_UInt32 dimensions{};
    OCI_UInt32 format{};

    core::Check(OCI_VectorGetInfo(*this, &format, &dimensions));

    return dimensions;
}

inline Vector::VectorFormat Vector::GetFormat()
{
    OCI_UInt32 dimensions{};
    OCI_UInt32 format{};

    core::Check(OCI_VectorGetInfo(*this, &format, &dimensions));

    return VectorFormat(static_cast<VectorFormat::Type>(format));
};

inline void Vector::FromString(const ostring& str, VectorFormat format, OCI_UInt32 dimensions)
{
    core::Check(OCI_VectorFromText(*this, str.c_str(), static_cast<OCI_UInt32>(str.size()), format, dimensions));
}

inline ostring Vector::ToString() const
{
    if (!IsNull())
    {       
        OCI_UInt32 size{};
        
        core::Check(OCI_VectorToText(*this, &size, nullptr));

        ostring result;

        result.resize(static_cast<OCI_Size>(size));

        core::Check(OCI_VectorToText(*this, &size, const_cast<otext*>(result.data())));

        result.resize(static_cast<OCI_Size>(size));

        return result;
    }

    return OCI_STRING_NULL;
}

}