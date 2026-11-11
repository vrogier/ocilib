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

namespace ocilib
{

    inline Number::Number()
    {
    }

    inline Number::Number(const Environment& env, bool create)
    {
        if (create)
        {
            Allocate(env.GetHandle(), env.GetEnvironmentHandle());
        }
    }

    template<class T, core::SupportedNumericT<T>*>
    Number::Number(const Environment& env, const T &value)
    {
        Allocate(env.GetHandle(), env.GetEnvironmentHandle());
        *this = value;
    }

    inline Number::Number(const Number& other)
    {
        *this = other;
    }

    inline Number& Number::operator= (const Number& other)
    {
        if (this != &other)
        {
            if (IsNull())
            {
                Allocate(other.GetEnvironment(), other.GetParent());
            }

            core::Check(OCI_NumberAssign(*this, other));
        }

        return *this;
    }

    inline Number::Number(OCI_Number *pNumber, core::Handle *parent)
    {
        AcquireTransient(pNumber, parent);
    }

    inline Number::Number(const Environment& env, const String& str, const String& format)
    {
        Allocate(env.GetHandle(), env.GetEnvironmentHandle());

        FromString(str, format);
    }

    inline Number::Number(const Environment& env, const otext* str, const otext* format)
    {
        Allocate(env.GetHandle(), env.GetEnvironmentHandle());

        FromString(String(str), String(format ? format : OTEXT("")));
    }

    inline void Number::Allocate(OCI_Environment* env, core::Handle* parent)
    {
        AcquireAllocated
        (
            core::Check(OCI_NumberCreate(env)),
            parent
        );
    }

    inline void Number::FromString(const String& str, const String& format) const
    {
        core::Check(OCI_NumberFromText(*this, str.c_str(),
                                       format.empty() ? core::MakeString(core::Check(OCI_EnvironmentGetFormat(GetEnvironment(),
                                                                                                              FormatNumeric))).c_str() : format.c_str()));
    }

    inline String Number::ToString(const String& format) const
    {
        if (!IsNull())
        {
            const Size size = OCI_SIZE_BUFFER;

            std::vector<otext> buffer(static_cast<Size>(size + 1));

            core::Check(OCI_NumberToText(*this, format.c_str(), static_cast<Int32>(size), buffer.data()));

            return core::MakeString(static_cast<const otext*>(buffer.data()));
        }

        return OCI_STRING_NULL;
    }

    inline String Number::ToString() const
    {
        if (IsNull())
        {
            return OCI_STRING_NULL;
        }

        return ToString(core::MakeString(core::Check(OCI_EnvironmentGetFormat(GetEnvironment(), FormatNumeric))));
    }

    inline Number Number::Clone() const
    {
        Number result;
        result.AcquireAllocated
        (
            core::Check(OCI_NumberCreate(GetEnvironment())),
            GetParent()
        );

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

    inline Int32 Number::Compare(const Number& other) const
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
            Allocate(GetEnvironment(), GetParent());
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
            GetParent()
        );

        return *this;
    }

    template<class T, core::SupportedNumericT<T>*>
    Number& Number::operator = (const T &lhs)
    {
        SetValue<T>(lhs);
        return *this;
    }

    template<class T, core::SupportedNumericT<T>*>
    Number::operator T() const
    {
        return GetValue<T>();
    }

    template<class T, core::SupportedNumericT<T>*>
    Number Number::operator + (const T &value)
    {
        Number result = Clone();
        result.Add(value);
        return result;
    }

    template<class T, core::SupportedNumericT<T>*>
    Number Number::operator - (const T &value)
    {
        Number result = Clone();
        result.Sub(value);
        return result;
    }

    template<class T, core::SupportedNumericT<T>*>
    Number Number::operator * (const T &value)
    {
        Number result = Clone();
        result.Multiply(value);
        return result;
    }

    template<class T, core::SupportedNumericT<T>*>
    Number Number::operator / (const T &value)
    {
        Number result = Clone();
        result.Divide(value);
        return result;
    }

    template<class T, core::SupportedNumericT<T>*>
    Number& Number::operator += (const T &value)
    {
        Add<T>(value);
        return *this;
    }

    template<class T, core::SupportedNumericT<T>*>
    Number& Number::operator -= (const T &value)
    {
        Sub<T>(value);
        return *this;
    }

    template<class T, core::SupportedNumericT<T>*>
    Number& Number::operator *= (const T &value)
    {
        Multiply<T>(value);
        return *this;
    }

    template<class T, core::SupportedNumericT<T>*>
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

    inline Number Number::operator ++ (Int32)
    {
        return *this + 1;
    }

    inline Number Number::operator -- (Int32)
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
        const Int32 res = Compare(other);

        return res == 0 || res < 0;
    }

    inline bool Number::operator <= (const Number& other) const
    {
        const Int32 res = Compare(other);

        return res == 0 || res > 0;
    }

}
