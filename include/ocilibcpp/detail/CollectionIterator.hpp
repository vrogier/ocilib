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

// ReSharper disable CppClangTidyCertOop54Cpp

namespace ocilib
{

template<class T>
CollectionIterator<T>::CollectionIterator() : _elem()
{

}

template<class T>
CollectionIterator<T>::CollectionIterator(CollectionType *collection, unsigned int pos) : _elem(collection, pos)
{

}

template<class T>
CollectionIterator<T>::CollectionIterator(const CollectionIterator& other) : _elem(other._elem)
{

}

template<class T>
CollectionIterator<T>& CollectionIterator<T>::operator = (const CollectionIterator& other)
{
    if (this != &other)
    {
        _elem._pos = other._elem._pos;
        _elem._coll = other._elem._coll;
    }

	return *this;
}

template<class T>
CollectionIterator<T>& CollectionIterator<T>::operator += (difference_type value)
{
	_elem._pos += static_cast<unsigned int>(value);
	return *this;
}

template<class T>
CollectionIterator<T>& CollectionIterator<T>::operator -= (difference_type value)
{
	_elem._pos -= static_cast<unsigned int>(value);
	return *this;
}

template<class T>
T& CollectionIterator<T>::operator*()
{
	return _elem;
}

template<class T>
T* CollectionIterator<T>::operator->()
{
	return &_elem;
}

template<class T>
CollectionIterator<T>& CollectionIterator<T>::operator--()
{
	--_elem._pos;
	return *this;
}

template<class T>
CollectionIterator<T>& CollectionIterator<T>::operator++()
{
	++*(const_cast<unsigned int*>(&_elem._pos));
	return *this;
}

template<class T>
CollectionIterator<T> CollectionIterator<T>::operator++(int)
{
	CollectionIterator res(_elem._coll, _elem._pos);
	++(*this);
	return res;
}

template<class T>
CollectionIterator<T> CollectionIterator<T>::operator--(int)
{
	CollectionIterator res(_elem);
	--(*this);
	return res;
}

template<class T>
CollectionIterator<T>  CollectionIterator<T>::operator + (difference_type value)
{
	return CollectionIterator(_elem._coll, _elem._pos + static_cast<unsigned int>(value));
}

template<class T>
CollectionIterator<T> CollectionIterator<T>::operator - (difference_type value)
{
	return CollectionIterator(_elem._coll, _elem._pos - static_cast<unsigned int>(value));
}

template<class T>
typename CollectionIterator<T>::difference_type CollectionIterator<T>::operator - (const CollectionIterator & other)
{
	return static_cast<difference_type>(_elem._pos - other._elem._pos);
}

template<class T>
bool CollectionIterator<T>::operator == (const CollectionIterator& other)
{
	return _elem._pos == other._elem._pos && (static_cast<OCI_Coll *>(*_elem._coll)) == (static_cast<OCI_Coll *>(*other._elem._coll));
}

template<class T>
bool CollectionIterator<T>::operator != (const CollectionIterator& other)
{
	return !(*this == other);
}

template<class T>
bool CollectionIterator<T>::operator > (const CollectionIterator& other)
{
	return _elem._pos > other._elem._pos;
}

template<class T>
bool CollectionIterator<T>::operator < (const CollectionIterator& other)
{
	return _elem._pos < other._elem._pos;
}

template<class T>
bool CollectionIterator<T>::operator >= (const CollectionIterator& other)
{
	return _elem._pos >= other._elem._pos;
}

template<class T>
bool CollectionIterator<T>::operator <= (const CollectionIterator& other)
{
	return _elem._pos <= other._elem._pos;
}

}
