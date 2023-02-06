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

inline Number::Number(bool create)
{
    if (create)
    {
        Allocate();
    }
}

inline Number::Number(OCI_Number *pNumber, core::Handle *parent)
{
    AcquireTransient(pNumber, parent);
}

inline Number::Number(const ostring& str, const ostring& format)
{
    Allocate();

    FromString(str, format);
}

inline Number::Number(const otext* str, const otext* format)
{
    Allocate();

    FromString(str, format);
}

inline void Number::Allocate()
{
    AcquireAllocated
    (
        core::Check(OCI_NumberCreate(nullptr)),
        Environment::GetEnvironmentHandle()
    );
}

inline void Number::FromString(const ostring& str, const ostring& format) const
{
    core::Check(OCI_NumberFromText(*this, str.c_str(), format.empty() ? Environment::GetFormat(FormatNumeric).c_str() : format.c_str()));
}

inline ostring Number::ToString(const ostring& format) const
{
    if (!IsNull())
    {
       const  size_t size = OCI_SIZE_BUFFER;

        core::ManagedBuffer<otext> buffer(static_cast<size_t>(size + 1));

        core::Check(OCI_NumberToText(*this, format.c_str(), static_cast<int>(size), buffer));

        return core::MakeString(static_cast<const otext *>(buffer));
    }

    return OCI_STRING_NULL;
}

inline ostring Number::ToString() const
{
    return ToString(Environment::GetFormat(FormatNumeric));
}

inline Number Number::Clone() const
{
    Number result;

    result.Allocate();

    core::Check(OCI_NumberAssign(result, *this));

    return result;
}

template<class T>
AnyPointer Number::GetNativeValue(const T& value)
{
    return reinterpret_cast<AnyPointer>(const_cast<T*>(&value));
}

template<>
inline AnyPointer Number::GetNativeValue(const Number& value)
{
    return (reinterpret_cast<OCI_Number*>(core::Check(OCI_NumberGetContent(value))));
}

inline int Number::Compare(const Number& other) const
{
    return core::Check(OCI_NumberCompare(*this, other));
}

template<class T>
T Number::GetValue() const
{
    T value;

    core::Check(OCI_NumberGetValue(*this, support::NumericTypeResolver<T>::Value, &value));

    return value;
}

template<class T>
Number& Number::SetValue(const T &value)
{
    if (IsNull())
    {
        Allocate();
    }

    core::Check(OCI_NumberSetValue(*this, support::NumericTypeResolver<T>::Value, GetNativeValue(value)));

    return *this;
}

template<class T>
void Number::Add(const T &value)
{
    core::Check(OCI_NumberAdd(*this, support::NumericTypeResolver<T>::Value, GetNativeValue(value)));
}

template<class T>
void Number::Sub(const T &value)
{
    core::Check(OCI_NumberSub(*this, support::NumericTypeResolver<T>::Value, GetNativeValue(value)));
}

template<class T>
void Number::Multiply(const T &value)
{
    core::Check(OCI_NumberMultiply(*this, support::NumericTypeResolver<T>::Value, GetNativeValue(value)));
}

template<class T>
void Number::Divide(const T &value)
{
    core::Check(OCI_NumberDivide(*this, support::NumericTypeResolver<T>::Value, GetNativeValue(value)));
}

inline Number& Number::operator = (OCI_Number * &lhs)
{
    AcquireAllocated
    (
        lhs, 
        Environment::GetEnvironmentHandle()
    );

    return *this;
}

template<class T, typename core::SupportedNumeric<T>::Type::type*>
Number& Number::operator = (const T &lhs)
{
    SetValue<T>(lhs);
    return *this;
}

template<class T, typename core::SupportedNumeric<T>::Type::type*>
Number::operator T() const
{
    return GetValue<T>();
}

template<class T, typename core::SupportedNumeric<T>::Type::type*>
Number Number::operator + (const T &value)
{
    Number result = Clone();
    result.Add(value);
    return result;
}

template<class T, typename core::SupportedNumeric<T>::Type::type*>
Number Number::operator - (const T &value)
{
    Number result = Clone();
    result.Sub(value);
    return result;
}

template<class T, typename core::SupportedNumeric<T>::Type::type*>
Number Number::operator * (const T &value)
{
    Number result = Clone();
    result.Multiply(value);
    return result;
}

template<class T, typename core::SupportedNumeric<T>::Type::type*>
Number Number::operator / (const T &value)
{
    Number result = Clone();
    result.Divide(value);
    return result;
}

template<class T, typename core::SupportedNumeric<T>::Type::type*>
Number& Number::operator += (const T &value)
{
    Add<T>(value);
    return *this;
}

template<class T, typename core::SupportedNumeric<T>::Type::type*>
Number& Number::operator -= (const T &value)
{
    Sub<T>(value);
    return *this;
}

template<class T, typename core::SupportedNumeric<T>::Type::type*>
Number& Number::operator *= (const T &value)
{
    Multiply<T>(value);
    return *this;
}

template<class T, typename core::SupportedNumeric<T>::Type::type*>
Number& Number::operator /= (const T &value)
{
    Divide<T>(value);
    return *this;
}

inline Number& Number::operator ++ ()
{
    return *this += 1;
}

inline Number& Number::operator -- ()
{
    return *this += 1;
}

inline Number Number::operator ++ (int)
{
    return *this + 1;
}

inline Number Number::operator -- (int)
{
    return *this - 1;
}

inline bool Number::operator == (const Number& other) const
{
    return Compare(other) == 0;
}

inline bool Number::operator != (const Number& other) const
{
    return !(*this == other);
}

inline bool Number::operator > (const Number& other) const
{
    return Compare(other) > 0;
}

inline bool Number::operator < (const Number& other) const
{
    return Compare(other) < 0;
}

inline bool Number::operator >= (const Number& other) const
{
    const int res = Compare(other);

    return res == 0 || res < 0;
}

inline bool Number::operator <= (const Number& other) const
{
    const int res = Compare(other);

    return res == 0 || res > 0;
}

}
