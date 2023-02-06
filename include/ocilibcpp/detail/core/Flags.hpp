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

// ReSharper disable CppClangTidyCertOop54Cpp

namespace ocilib
{
    namespace core
    {
        template<class T>
        Flags<T>::Flags() : _flags(static_cast<T>(0))
        {
        }

        template<class T>
        Flags<T>::Flags(T flag) : _flags(flag)
        {
        }

        template<class T>
        Flags<T>::Flags(const Flags& other) : _flags(other._flags)
        {
        }

        template<class T>
        Flags<T>::Flags(unsigned int flag) : _flags(static_cast<T>(flag))
        {
        }

        template<class T>
        Flags<T>& Flags<T>::operator = (const Flags<T>& other) noexcept
        {
            if (this != &other)
            {
                _flags = other._flags;
            }

            return *this;
        }

        template<class T>
        Flags<T> Flags<T>::operator~ () const
        {
            return Flags<T>(~_flags);
        }

        template<class T>
        Flags<T> Flags<T>::operator | (const Flags& other) const
        {
            return Flags<T>(_flags | other._flags);
        }

        template<class T>
        Flags<T> Flags<T>::operator & (const Flags& other) const
        {
            return Flags<T>(_flags & other._flags);
        }

        template<class T>
        Flags<T> Flags<T>::operator ^ (const Flags& other) const
        {
            return Flags<T>(_flags ^ other._flags);
        }

        template<class T>
        Flags<T> Flags<T>::operator | (T other) const
        {
            return Flags<T>(_flags | other);
        }

        template<class T>
        Flags<T> Flags<T>::operator & (T other) const
        {
            return Flags<T>(_flags & other);
        }

        template<class T>
        Flags<T> Flags<T>::operator ^ (T other) const
        {
            return Flags<T>(_flags ^ other);
        }

        template<class T>
        Flags<T>& Flags<T>::operator |= (const Flags<T>& other)
        {
            _flags |= other._flags;
            return *this;
        }

        template<class T>
        Flags<T>& Flags<T>::operator &= (const Flags<T>& other)
        {
            _flags &= other._flags;
            return *this;
        }

        template<class T>
        Flags<T>& Flags<T>::operator ^= (const Flags<T>& other)
        {
            _flags ^= other._flags;
            return *this;
        }

        template<class T>
        Flags<T>& Flags<T>::operator |= (T other)
        {
            _flags |= other;
            return *this;
        }

        template<class T>
        Flags<T>& Flags<T>::operator &= (T other)
        {
            _flags &= other;
            return *this;
        }

        template<class T>
        Flags<T>& Flags<T>::operator ^= (T other)
        {
            _flags ^= other;
            return *this;
        }

        template<class T>
        bool Flags<T>::operator == (T other) const
        {
            return _flags == static_cast<unsigned int>(other);
        }

        template<class T>
        bool Flags<T>::operator == (const Flags& other) const
        {
            return _flags == other._flags;
        }

        template<class T>
        bool Flags<T>::IsSet(T other) const
        {
            return ((_flags & other) == _flags);
        }

        template<class T>
        unsigned int Flags<T>::GetValues() const
        {
            return _flags;
        }
    }
}