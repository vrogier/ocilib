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

// ReSharper disable CppClangTidyBugproneUnhandledSelfAssignment

namespace ocilib
{

template<class T>
CollectionElement<T>::CollectionElement() : _coll(nullptr), _pos(0)
{

}

template<class T>
CollectionElement<T>::CollectionElement(CollectionType *coll, unsigned int pos) : _coll(coll), _pos(pos)
{

}

template<class T>
CollectionElement<T>::operator T() const
{
    return _coll->Get(_pos);
}

template<class T>
CollectionElement<T>& CollectionElement<T>::operator = (const ValueType& other)
{
    if (this != &other)
    {
        _coll->Set(_pos, other);
    }

    return *this;
}

template<class T>
CollectionElement<T>& CollectionElement<T>::operator = (const CollectionElement &other)
{
    if (this != &other)
    {
        _coll->Set(_pos, static_cast<T>(other));
    }

	return *this;
}

template<class T>
bool CollectionElement<T>::IsNull() const
{
    return _coll->IsElementNull(_pos);
}

template<class T>
void CollectionElement<T>::SetNull()
{
    _coll->SetElementNull(_pos);
}

}
