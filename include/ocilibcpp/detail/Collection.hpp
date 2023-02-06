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

// ReSharper disable CppClangTidyMiscMisplacedConst

namespace ocilib
{

template<class T>
Collection<T>::Collection()
{
}

template<class T>
Collection<T>::Collection(const TypeInfo &typeInfo)
{
    AcquireAllocated
    (
        core::Check(OCI_CollCreate(typeInfo)), 
        typeInfo.GetConnection().GetHandle()
    );
}

template<class T>
Collection<T>::Collection(OCI_Coll *pColl, core::Handle *parent)
{
    AcquireTransient(pColl,parent);
}

template<class T>
Collection<T> Collection<T>::Clone() const
{
    Collection<T> result(GetTypeInfo());

    core::Check(OCI_CollAssign(result, *this));

    return result;
}

template<class T>
TypeInfo Collection<T>::GetTypeInfo() const
{
    return TypeInfo(core::Check(OCI_CollGetTypeInfo(*this)));
}

template<class T>
typename Collection<T>::CollectionType Collection<T>::GetType() const
{
    return CollectionType(core::Check(OCI_CollGetType(*this)));
}

template<class T>
unsigned int Collection<T>::GetMax() const
{
    return core::Check(OCI_CollGetMax(*this));
}

template<class T>
unsigned int Collection<T>::GetSize() const

{
    return core::Check(OCI_CollGetSize(*this));
}

template<class T>
unsigned int Collection<T>::GetCount() const

{
    return core::Check(OCI_CollGetCount(*this));
}

template<class T>
void Collection<T>::Truncate(unsigned int size)
{
    core::Check(OCI_CollTrim(*this, size));
}

template<class T>
void Collection<T>::Clear()
{
    core::Check(OCI_CollClear(*this));
}

template<class T>
bool Collection<T>::IsElementNull(unsigned int index) const
{
   return (core::Check(OCI_ElemIsNull(core::Check(OCI_CollGetElem(*this, index)))) == TRUE);
}

template<class T>
void Collection<T>::SetElementNull(unsigned int index)
{
    core::Check(OCI_ElemSetNull(core::Check(OCI_CollGetElem(*this, index))));
}

template<class T>
bool Collection<T>::Delete(unsigned int index) const
{
   return (core::Check(OCI_CollDeleteElem(*this, index)) == TRUE);
}

template<class T>
typename Collection<T>::iterator Collection<T>::begin()
{
    return iterator(this, 1);
}

template<class T>
typename Collection<T>::const_iterator Collection<T>::begin() const
{
    return const_iterator(const_cast<Collection*>(this), 1);
}

template<class T>
typename Collection<T>::iterator Collection<T>::end()
{
    return iterator(const_cast<Collection*>(this), GetCount() + 1);
}

template<class T>
typename Collection<T>::const_iterator Collection<T>::end() const
{
    return const_iterator(const_cast<Collection*>(this), GetCount() + 1);
}

template<class T>
T Collection<T>::Get(unsigned int index) const
{
    return GetElem(core::Check(OCI_CollGetElem(*this, index)), GetHandle());
}

template<class T>
void Collection<T>::Set(unsigned int index, const T & value)
{
    OCI_Elem * elem = core::Check(OCI_CollGetElem(*this, index));

    SetElem(elem, value);

    core::Check(OCI_CollSetElem(*this, index, elem));
}

template<class T>
void Collection<T>::Append(const T &value)
{
    OCI_Elem * elem = core::Check(OCI_ElemCreate(OCI_CollGetTypeInfo(*this)));

    SetElem(elem, value);

    core::Check(OCI_CollAppend(*this, elem));
    core::Check(OCI_ElemFree(elem));
}

template<>
inline bool Collection<bool>::GetElem(OCI_Elem *elem, core::Handle *parent)
{
    ARG_NOT_USED(parent);

    return (core::Check(OCI_ElemGetBoolean(elem)) == TRUE);
}

template<>
inline short Collection<short>::GetElem(OCI_Elem *elem, core::Handle *parent)
{
    ARG_NOT_USED(parent);

    return core::Check(OCI_ElemGetShort(elem));
}

template<>
inline unsigned short Collection<unsigned short>::GetElem(OCI_Elem *elem, core::Handle *parent)
{
    ARG_NOT_USED(parent);

    return core::Check(OCI_ElemGetUnsignedShort(elem));
}

template<>
inline int Collection<int>::GetElem(OCI_Elem *elem, core::Handle *parent)
{
    ARG_NOT_USED(parent);

    return core::Check(OCI_ElemGetInt(elem));
}

template<>
inline unsigned int Collection<unsigned int>::GetElem(OCI_Elem *elem, core::Handle *parent)
{
    ARG_NOT_USED(parent);

    return core::Check(OCI_ElemGetUnsignedInt(elem));
}

template<>
inline big_int Collection<big_int>::GetElem(OCI_Elem *elem, core::Handle *parent)
{
    ARG_NOT_USED(parent);

    return core::Check(OCI_ElemGetBigInt(elem));
}

template<>
inline big_uint Collection<big_uint>::GetElem(OCI_Elem *elem, core::Handle *parent)
{
    ARG_NOT_USED(parent);

    return core::Check(OCI_ElemGetUnsignedBigInt(elem));
}

template<>
inline float Collection<float>::GetElem(OCI_Elem *elem, core::Handle *parent)
{
    ARG_NOT_USED(parent);

    return core::Check(OCI_ElemGetFloat(elem));
}

template<>
inline double Collection<double>::GetElem(OCI_Elem *elem, core::Handle *parent)
{
    ARG_NOT_USED(parent);

    return core::Check(OCI_ElemGetDouble(elem));
}

template<>
inline Number Collection<Number>::GetElem(OCI_Elem *elem, core::Handle *parent)
{
    return Number(core::Check(OCI_ElemGetNumber(elem)), parent);
}

template<>
inline ostring Collection<ostring>::GetElem(OCI_Elem *elem, core::Handle *parent)
{
    ARG_NOT_USED(parent);

    return core::MakeString(core::Check(OCI_ElemGetString(elem)));
}

template<>
inline Raw Collection<Raw>::GetElem(OCI_Elem *elem, core::Handle *parent)
{
    ARG_NOT_USED(parent);

    unsigned int size = core::Check(OCI_ElemGetRawSize(elem));

    core::ManagedBuffer<unsigned char> buffer(static_cast<size_t>(size + 1));

    size = core::Check(OCI_ElemGetRaw(elem, static_cast<AnyPointer>(buffer), size));

    return core::MakeRaw(buffer, size);
}

template<>
inline Date Collection<Date>::GetElem(OCI_Elem *elem, core::Handle *parent)
{
    return Date(core::Check(OCI_ElemGetDate(elem)), parent);
}

template<>
inline Timestamp Collection<Timestamp>::GetElem(OCI_Elem *elem, core::Handle *parent)
{
    return Timestamp(core::Check(OCI_ElemGetTimestamp(elem)), parent);
}

template<>
inline Interval Collection<Interval>::GetElem(OCI_Elem *elem, core::Handle *parent)
{
    return Interval(core::Check(OCI_ElemGetInterval(elem)), parent);
}

template<>
inline Object Collection<Object>::GetElem(OCI_Elem *elem, core::Handle *parent)
{
    return Object(core::Check(OCI_ElemGetObject(elem)), parent);
}

template<>
inline Reference Collection<Reference>::GetElem(OCI_Elem *elem, core::Handle *parent)
{
    return Reference(core::Check(OCI_ElemGetRef(elem)), parent);
}

template<>
inline Clob Collection<Clob>::GetElem(OCI_Elem *elem, core::Handle *parent)
{
    return Clob(core::Check(OCI_ElemGetLob(elem)), parent);
}

template<>
inline NClob Collection<NClob>::GetElem(OCI_Elem *elem, core::Handle *parent)
{
    return NClob(core::Check(OCI_ElemGetLob(elem)), parent);
}
template<>
inline Blob Collection<Blob>::GetElem(OCI_Elem *elem, core::Handle *parent)
{
    return Blob(core::Check(OCI_ElemGetLob(elem)), parent);
}

template<>
inline File Collection<File>::GetElem(OCI_Elem *elem, core::Handle *parent)
{
    return File(core::Check(OCI_ElemGetFile(elem)), parent);
}

template<class T>
 T Collection<T>::GetElem(OCI_Elem *elem, core::Handle *parent)
{
    return T(core::Check(OCI_ElemGetColl(elem)), parent);
}

template<>
inline void Collection<bool>::SetElem(OCI_Elem *elem, const bool &value)
{
    core::Check(OCI_ElemSetBoolean(elem, static_cast<boolean>(value)));
}

template<>
inline void Collection<short>::SetElem(OCI_Elem *elem, const short &value)
{
    core::Check(OCI_ElemSetShort(elem, value));
}

template<>
inline void Collection<unsigned short>::SetElem(OCI_Elem *elem, const unsigned short &value)
{
    core::Check(OCI_ElemSetUnsignedShort(elem, value));
}

template<>
inline void Collection<int>::SetElem(OCI_Elem *elem, const int &value)
{
    core::Check(OCI_ElemSetInt(elem, value));
}

template<>
inline void Collection<unsigned int>::SetElem(OCI_Elem *elem, const unsigned int &value)
{
    core::Check(OCI_ElemSetUnsignedInt(elem, value));
}

template<>
inline void Collection<big_int>::SetElem(OCI_Elem *elem, const big_int &value)
{
    core::Check(OCI_ElemSetBigInt(elem, value));
}

template<>
inline void Collection<big_uint>::SetElem(OCI_Elem *elem, const big_uint &value)
{
    core::Check(OCI_ElemSetUnsignedBigInt(elem, value));
}

template<>
inline void Collection<float>::SetElem(OCI_Elem *elem, const float &value)
{
    core::Check(OCI_ElemSetFloat(elem, value));
}

template<>
inline void Collection<double>::SetElem(OCI_Elem *elem, const double &value)
{
    core::Check(OCI_ElemSetDouble(elem, value));
}

template<>
inline void Collection<Number>::SetElem(OCI_Elem *elem, const Number &value)
{
    core::Check(OCI_ElemSetNumber(elem, value));
}

template<>
inline void Collection<ostring>::SetElem(OCI_Elem *elem, const ostring& value)
{
    core::Check(OCI_ElemSetString(elem, value.c_str()));
}

template<>
inline void Collection<Raw>::SetElem(OCI_Elem *elem, const Raw &value)
{
    const AnyPointer data = value.empty() ? nullptr : static_cast<AnyPointer>(const_cast<Raw::value_type*>(&value[0])) ;

    core::Check(OCI_ElemSetRaw(elem, data, static_cast<unsigned int>(value.size())));
}

template<>
inline void Collection<Date>::SetElem(OCI_Elem *elem, const Date &value)
{
    core::Check(OCI_ElemSetDate(elem, value));
}

template<>
inline void Collection<Timestamp>::SetElem(OCI_Elem *elem, const Timestamp &value)
{
    core::Check(OCI_ElemSetTimestamp(elem, value));
}

template<>
inline void Collection<Interval>::SetElem(OCI_Elem *elem, const Interval &value)
{
    core::Check(OCI_ElemSetInterval(elem, value));
}

template<>
inline void Collection<Object>::SetElem(OCI_Elem *elem, const Object &value)
{
    core::Check(OCI_ElemSetObject(elem, value));
}

template<>
inline void Collection<Reference>::SetElem(OCI_Elem *elem, const Reference &value)
{
    core::Check(OCI_ElemSetRef(elem, value));
}

template<>
inline void Collection<Clob>::SetElem(OCI_Elem *elem, const Clob &value)
{
    core::Check(OCI_ElemSetLob(elem, value));
}

template<>
inline void Collection<NClob>::SetElem(OCI_Elem *elem, const NClob &value)
{
    core::Check(OCI_ElemSetLob(elem, value));
}

template<>
inline void Collection<Blob>::SetElem(OCI_Elem *elem, const Blob &value)
{
    core::Check(OCI_ElemSetLob(elem, value));
}

template<>
inline void Collection<File>::SetElem(OCI_Elem *elem, const File &value)
{
    core::Check(OCI_ElemSetFile(elem, value));
}

template<class T>
void Collection<T>::SetElem(OCI_Elem *elem, const T &value)
{
    core::Check(OCI_ElemSetColl(elem, value));
}

template<class T>
ostring Collection<T>::ToString() const
{
    if (!IsNull())
    {
        unsigned int len = 0;

        core::Check(OCI_CollToText(*this, &len, nullptr));

        core::ManagedBuffer<otext> buffer(static_cast<size_t>(len + 1));

        core::Check(OCI_CollToText(*this, &len, buffer));

        return core::MakeString(static_cast<const otext *>(buffer), static_cast<int>(len));
    }

    return OCI_STRING_NULL;
}

template<class T>
CollectionElement<T> Collection<T>::operator [] (unsigned int index)
{
    return CollectionElement<T>(this, index);
}

template<class T>
CollectionElement<T> Collection<T>::operator [](unsigned int index) const
{
    return CollectionElement<T>(const_cast<Collection<T>*>(this), index);
}

}
