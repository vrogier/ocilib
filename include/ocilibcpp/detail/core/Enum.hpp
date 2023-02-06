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

#include "ocilibcpp/core.hpp"

namespace ocilib
{
    namespace core
    {
        template<class T>
        Enum<T>::Enum() : _value(static_cast<T>(0))
        {
        }

        template<class T>
        Enum<T>::Enum(T value) : _value(value)
        {
        }

        template<class T>
        T Enum<T>::GetValue()
        {
            return _value;
        }

        template<class T>
        Enum<T>::operator T ()
        {
            return GetValue();
        }

        template<class T>
        Enum<T>::operator unsigned int() const
        {
            return static_cast<unsigned int>(_value);
        }

        template<class T>
        bool Enum<T>::operator == (const Enum& other) const
        {
            return other._value == _value;
        }

        template<class T>
        bool Enum<T>::operator != (const Enum& other) const
        {
            return !(*this == other);
        }

        template<class T>
        bool Enum<T>::operator == (const T& other) const
        {
            return other == _value;
        }

        template<class T>
        bool Enum<T>::operator != (const T& other) const
        {
            return !(*this == other);
        }

    }
}