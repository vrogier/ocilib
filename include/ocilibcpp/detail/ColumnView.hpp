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

    template<class T>
    inline ColumnView<T>::ColumnView(T* owner)
        : _owner(owner)
    {
    }

    template<class T>
    inline typename ColumnView<T>::iterator ColumnView<T>::begin() const
    {
        return iterator(const_cast<T*>(_owner), 0);
    }

    template<class T>
    inline typename ColumnView<T>::iterator ColumnView<T>::end() const
    {
        return iterator(const_cast<T*>(_owner),
                        static_cast<typename iterator::difference_type>(Size()));
    }

    template<class T>
    inline UInt32 ColumnView<T>::Size() const
    {
        return _owner->GetColumnCount();
    }

    template<class T>
    inline Column ColumnView<T>::operator[](UInt32 index) const
    {
        return _owner->GetColumn(index);
    }

    template<class T>
    inline Column ColumnView<T>::operator[](const String& name) const
    {
        return _owner->GetColumn(static_cast<UInt32>(IndexOf(name)));
    }

    template<class T>
    inline Column ColumnView<T>::At(UInt32 index) const
    {
        return _owner->GetColumn(index);
    }

    template<class T>
    inline bool ColumnView<T>::Contains(const String& name) const
    {
        return IndexOf(name) != -1;
    }

    template<class T>
    inline Int32 ColumnView<T>::IndexOf(const String& name) const
    {
        const UInt32 count = Size();
        for (UInt32 i = 0; i < count; i++)
        {
            if (_owner->GetColumn(i).GetName() == name)
            {
                return static_cast<Int32>(i);
            }
        }
        return -1;
    }

}
