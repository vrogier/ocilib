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
    Connection connection = typeInfo.GetConnection();
    AcquireAllocated
    (
        core::Check(OCI_CollCreate(typeInfo)), 
        connection.GetHandle()
    );
}

template<class T>
Collection<T>::Collection(OCI_Coll *pColl, [[maybe_unused]] core::Handle *parent)
{
    AcquireTransient(pColl, parent);
}

template<class T>
Collection<T> Collection<T>::Clone() const
{
    Collection<T> result;
    TypeInfo typeInfo = GetTypeInfo();
    Connection connection = typeInfo.GetConnection();
    result.AcquireAllocated
    (
        core::Check(OCI_CollCreate(typeInfo)),
        connection.GetHandle()
    );

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
UInt32 Collection<T>::GetMax() const
{
    return core::Check(OCI_CollGetMax(*this));
}

template<class T>
UInt32 Collection<T>::GetSize() const

{
    return core::Check(OCI_CollGetSize(*this));
}

template<class T>
UInt32 Collection<T>::GetCount() const

{
    return core::Check(OCI_CollGetCount(*this));
}

template<class T>
void Collection<T>::Truncate(UInt32 size)
{
    core::Check(OCI_CollTrim(*this, size));
}

template<class T>
void Collection<T>::Clear()
{
    core::Check(OCI_CollClear(*this));
}

template<class T>
bool Collection<T>::IsElementNull(UInt32 index) const
{
   return (core::Check(OCI_ElemIsNull(core::Check(OCI_CollGetElem(*this, index)))) == TRUE);
}

template<class T>
void Collection<T>::SetElementNull(UInt32 index)
{
    core::Check(OCI_ElemSetNull(core::Check(OCI_CollGetElem(*this, index))));
}

template<class T>
bool Collection<T>::Delete(UInt32 index) const
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
T Collection<T>::Get(UInt32 index) const
{
    return GetElem(core::Check(OCI_CollGetElem(*this, index)), GetHandle());
}

template<class T>
void Collection<T>::Set(UInt32 index, const T & value)
{
    OCI_Elem * elem = core::Check(OCI_CollGetElem(*this, index));

    SetElem(elem, value, GetHandle());

    core::Check(OCI_CollSetElem(*this, index, elem));
}

template<class T>
void Collection<T>::Append(const T &value)
{
    OCI_Elem * elem = core::Check(OCI_ElemCreate(OCI_CollGetTypeInfo(*this)));

    SetElem(elem, value, GetHandle());

    core::Check(OCI_CollAppend(*this, elem));
    core::Check(OCI_ElemFree(elem));
}

template<class T>
inline T Collection<T>::GetElem(OCI_Elem *elem, [[maybe_unused]] core::Handle *parent)
{
    if constexpr (std::is_same_v<T, bool>)
    {

    return (core::Check(OCI_ElemGetBoolean(elem)) == TRUE);
    }
    else if constexpr (std::is_same_v<T, Int16>)
    {

    return core::Check(OCI_ElemGetShort(elem));
    }
    else if constexpr (std::is_same_v<T, UInt16>)
    {

    return core::Check(OCI_ElemGetUnsignedShort(elem));
    }
    else if constexpr (std::is_same_v<T, Int32>)
    {

    return core::Check(OCI_ElemGetInt(elem));
    }
    else if constexpr (std::is_same_v<T, UInt32>)
    {

    return core::Check(OCI_ElemGetUnsignedInt(elem));
    }
    else if constexpr (std::is_same_v<T, Int64>)
    {

    return core::Check(OCI_ElemGetBigInt(elem));
    }
    else if constexpr (std::is_same_v<T, UInt64>)
    {

    return core::Check(OCI_ElemGetUnsignedBigInt(elem));
    }
    else if constexpr (std::is_same_v<T, Float>)
    {

    return core::Check(OCI_ElemGetFloat(elem));
    }
    else if constexpr (std::is_same_v<T, Double>)
    {

    return core::Check(OCI_ElemGetDouble(elem));
    }
    else if constexpr (std::is_same_v<T, Number>)
    {

    return Number(core::Check(OCI_ElemGetNumber(elem)), parent);
    }
    else if constexpr (std::is_same_v<T, String>)
    {

    return core::MakeString(core::Check(OCI_ElemGetString(elem)));
    }
    else if constexpr (std::is_same_v<T, Date>)
    {

    return Date(core::Check(OCI_ElemGetDate(elem)), parent);
    }
    else if constexpr (std::is_same_v<T, Timestamp>)
    {

    return Timestamp(core::Check(OCI_ElemGetTimestamp(elem)), parent);
    }
    else if constexpr (std::is_same_v<T, Interval>)
    {

    return Interval(core::Check(OCI_ElemGetInterval(elem)), parent);
    }
    else if constexpr (std::is_same_v<T, Object>)
    {

    return Object(core::Check(OCI_ElemGetObject(elem)), parent);
    }
    else if constexpr (std::is_same_v<T, Reference>)
    {

    return Reference(core::Check(OCI_ElemGetRef(elem)), parent);
    }
    else if constexpr (std::is_same_v<T, Clob>)
    {

    return Clob(core::Check(OCI_ElemGetLob(elem)), parent);
    }
    else if constexpr (std::is_same_v<T, NClob>)
    {

    return NClob(core::Check(OCI_ElemGetLob(elem)), parent);
    }
    else if constexpr (std::is_same_v<T, Blob>)
    {

    return Blob(core::Check(OCI_ElemGetLob(elem)), parent);
    }
    else if constexpr (std::is_same_v<T, File>)
    {

    return File(core::Check(OCI_ElemGetFile(elem)), parent);
    }
    else if constexpr (std::is_same_v<T, Raw>)
    {

    UInt32 size = core::Check(OCI_ElemGetRawSize(elem));

    Raw value(static_cast<Size>(size + 1));

    size = core::Check(OCI_ElemGetRaw(elem, static_cast<AnyPointer>(value.data()), size));
    value.resize(size);

    return value;
    }
    else
    {
        static_assert(core::IsCollectionV<T>, "Collection::Get<T> only supports scalar types, Oracle object types and Collection<U>");

    return T(core::Check(OCI_ElemGetColl(elem)), parent);
    }
}

template<class T>
inline void Collection<T>::SetElem(OCI_Elem *elem, const T &value, [[maybe_unused]] core::Handle *parent)
{
    if constexpr (std::is_same_v<T, bool>)
    {

    core::Check(OCI_ElemSetBoolean(elem, static_cast<OCI_Boolean>(value)));
    }
    else if constexpr (std::is_same_v<T, Int16>)
    {

    core::Check(OCI_ElemSetShort(elem, value));
    }
    else if constexpr (std::is_same_v<T, UInt16>)
    {

    core::Check(OCI_ElemSetUnsignedShort(elem, value));
    }
    else if constexpr (std::is_same_v<T, Int32>)
    {

    core::Check(OCI_ElemSetInt(elem, value));
    }
    else if constexpr (std::is_same_v<T, UInt32>)
    {

    core::Check(OCI_ElemSetUnsignedInt(elem, value));
    }
    else if constexpr (std::is_same_v<T, Int64>)
    {

    core::Check(OCI_ElemSetBigInt(elem, value));
    }
    else if constexpr (std::is_same_v<T, UInt64>)
    {

    core::Check(OCI_ElemSetUnsignedBigInt(elem, value));
    }
    else if constexpr (std::is_same_v<T, Float>)
    {

    core::Check(OCI_ElemSetFloat(elem, value));
    }
    else if constexpr (std::is_same_v<T, Double>)
    {

    core::Check(OCI_ElemSetDouble(elem, value));
    }
    else if constexpr (std::is_same_v<T, Number>)
    {

    core::Check(OCI_ElemSetNumber(elem, value));
    }
    else if constexpr (std::is_same_v<T, String>)
    {

    core::Check(OCI_ElemSetString(elem, value.c_str()));
    }
    else if constexpr (std::is_same_v<T, Date>)
    {

    core::Check(OCI_ElemSetDate(elem, value));
    }
    else if constexpr (std::is_same_v<T, Timestamp>)
    {

    core::Check(OCI_ElemSetTimestamp(elem, value));
    }
    else if constexpr (std::is_same_v<T, Interval>)
    {

    core::Check(OCI_ElemSetInterval(elem, value));
    }
    else if constexpr (std::is_same_v<T, Object>)
    {

    core::Check(OCI_ElemSetObject(elem, value));
    }
    else if constexpr (std::is_same_v<T, Reference>)
    {

    core::Check(OCI_ElemSetRef(elem, value));
    }
    else if constexpr (std::is_same_v<T, Clob>)
    {

    core::Check(OCI_ElemSetLob(elem, value));
    }
    else if constexpr (std::is_same_v<T, NClob>)
    {

    core::Check(OCI_ElemSetLob(elem, value));
    }
    else if constexpr (std::is_same_v<T, Blob>)
    {

    core::Check(OCI_ElemSetLob(elem, value));
    }
    else if constexpr (std::is_same_v<T, File>)
    {

    core::Check(OCI_ElemSetFile(elem, value));
    }
    else if constexpr (std::is_same_v<T, Raw>)
    {

    const AnyPointer data = value.empty() ? nullptr : static_cast<AnyPointer>(const_cast<Raw::value_type*>(&value[0])) ;

    core::Check(OCI_ElemSetRaw(elem, data, static_cast<UInt32>(value.size())));
    }
    else
    {

    core::Check(OCI_ElemSetColl(elem, value));
    }
}












































template<class T>
String Collection<T>::ToString() const
{
    if (!IsNull())
    {
        UInt32 size{};
        
        core::Check(OCI_CollToText(*this, &size, nullptr));

        String result;

        result.resize(static_cast<Size>(size));

        core::Check(OCI_CollToText(*this, &size , const_cast<otext*>(result.data())));

        result.resize(static_cast<Size>(size));

        return result;
    }

    return OCI_STRING_NULL;
}

template<class T>
CollectionElement<T> Collection<T>::operator [] (UInt32 index)
{
    return CollectionElement<T>(this, index);
}

template<class T>
CollectionElement<T> Collection<T>::operator [](UInt32 index) const
{
    return CollectionElement<T>(const_cast<Collection<T>*>(this), index);
}

}
