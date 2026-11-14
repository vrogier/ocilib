/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2026 Vincent ROGIER <vince.rogier@gmail.com>
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
    Collection<T>::Collection()
    {
    }

    template<class T>
    Collection<T>::Collection(const TypeInfo &typeInfo)
    {
        Connection connection = typeInfo.GetConnection();
        AcquireAllocated
        (
            core::Check(OCI_CollectionCreate(typeInfo)),
            connection.GetHandle()
        );
    }

    template<class T>
    Collection<T>::Collection(OCI_Collection *pColl, [[maybe_unused]] core::Handle *parent)
    {
        AcquireTransient(pColl, parent);
    }

    template<class T>
    Collection<T> Collection<T>::Clone() const
    {
        Collection<T> result;
        TypeInfo      typeInfo = GetTypeInfo();
        Connection    connection = typeInfo.GetConnection();
        result.AcquireAllocated
        (
            core::Check(OCI_CollectionCreate(typeInfo)),
            connection.GetHandle()
        );

        core::Check(OCI_CollectionAssign(result, *this));

        return result;
    }

    template<class T>
    TypeInfo Collection<T>::GetTypeInfo() const
    {
        return TypeInfo(core::Check(OCI_CollectionGetTypeInfo(*this)));
    }

    template<class T>
    typename Collection<T>::CollectionType Collection<T>::GetType() const
    {
        return CollectionType(core::Check(OCI_CollectionGetType(*this)));
    }

    template<class T>
    UInt32 Collection<T>::GetMax() const
    {
        return core::Check(OCI_CollectionGetMax(*this));
    }

    template<class T>
    UInt32 Collection<T>::GetSize() const

    {
        return core::Check(OCI_CollectionGetSize(*this));
    }

    template<class T>
    UInt32 Collection<T>::GetCount() const

    {
        return core::Check(OCI_CollectionGetCount(*this));
    }

    template<class T>
    void Collection<T>::Truncate(UInt32 size)
    {
        core::Check(OCI_CollectionTrim(*this, size));
    }

    template<class T>
    void Collection<T>::Clear()
    {
        core::Check(OCI_CollectionClear(*this));
    }

    template<class T>
    bool Collection<T>::IsElementNull(UInt32 index) const
    {
        return (core::Check(OCI_ElementIsNull(core::Check(OCI_CollectionGetElement(*this, index)))) == TRUE);
    }

    template<class T>
    void Collection<T>::SetElementNull(UInt32 index)
    {
        core::Check(OCI_ElementSetNull(core::Check(OCI_CollectionGetElement(*this, index))));
    }

    template<class T>
    bool Collection<T>::Delete(UInt32 index) const
    {
        return (core::Check(OCI_CollectionDeleteElement(*this, index)) == TRUE);
    }

    template<class T>
    typename Collection<T>::iterator Collection<T>::begin()
    {
        return iterator(this, 0);
    }

    template<class T>
    typename Collection<T>::const_iterator Collection<T>::begin() const
    {
        return const_iterator(const_cast<Collection*>(this), 0);
    }

    template<class T>
    typename Collection<T>::iterator Collection<T>::end()
    {
        return iterator(const_cast<Collection*>(this), GetCount());
    }

    template<class T>
    typename Collection<T>::const_iterator Collection<T>::end() const
    {
        return const_iterator(const_cast<Collection*>(this), GetCount());
    }

    template<class T>
    T Collection<T>::Get(UInt32 index) const
    {
        return GetElem(core::Check(OCI_CollectionGetElement(*this, index)), GetHandle());
    }

    template<class T>
    void Collection<T>::Set(UInt32 index, const T & value)
    {
        OCI_Element * elem = core::Check(OCI_CollectionGetElement(*this, index));

        SetElem(elem, value, GetHandle());

        core::Check(OCI_CollectionSetElement(*this, index, elem));
    }

    template<class T>
    void Collection<T>::Append(const T &value)
    {
        OCI_Element * elem = core::Check(OCI_ElementCreate(OCI_CollectionGetTypeInfo(*this)));

        SetElem(elem, value, GetHandle());

        core::Check(OCI_CollectionAppend(*this, elem));
        core::Check(OCI_ElementFree(elem));
    }

    template<class T>
    inline T Collection<T>::GetElem(OCI_Element *elem, [[maybe_unused]] core::Handle *parent)
    {
        if constexpr (std::is_same_v<T, bool>)
        {

            return (core::Check(OCI_ElementGetBoolean(elem)) == TRUE);
        }
        else if constexpr (std::is_same_v<T, Int16>)
        {

            return core::Check(OCI_ElementGetInt16(elem));
        }
        else if constexpr (std::is_same_v<T, UInt16>)
        {

            return core::Check(OCI_ElementGetUInt16(elem));
        }
        else if constexpr (std::is_same_v<T, Int32>)
        {

            return core::Check(OCI_ElementGetInt32(elem));
        }
        else if constexpr (std::is_same_v<T, UInt32>)
        {

            return core::Check(OCI_ElementGetUInt32(elem));
        }
        else if constexpr (std::is_same_v<T, Int64>)
        {

            return core::Check(OCI_ElementGetInt64(elem));
        }
        else if constexpr (std::is_same_v<T, UInt64>)
        {

            return core::Check(OCI_ElementGetUInt64(elem));
        }
        else if constexpr (std::is_same_v<T, Float>)
        {

            return core::Check(OCI_ElementGetFloat(elem));
        }
        else if constexpr (std::is_same_v<T, Double>)
        {

            return core::Check(OCI_ElementGetDouble(elem));
        }
        else if constexpr (std::is_same_v<T, Number>)
        {

            return Number(core::Check(OCI_ElementGetNumber(elem)), parent);
        }
        else if constexpr (std::is_same_v<T, String>)
        {

            return core::MakeString(core::Check(OCI_ElementGetString(elem)));
        }
        else if constexpr (std::is_same_v<T, Date>)
        {

            return Date(core::Check(OCI_ElementGetDate(elem)), parent);
        }
        else if constexpr (std::is_same_v<T, Timestamp>)
        {

            return Timestamp(core::Check(OCI_ElementGetTimestamp(elem)), parent);
        }
        else if constexpr (std::is_same_v<T, Interval>)
        {

            return Interval(core::Check(OCI_ElementGetInterval(elem)), parent);
        }
        else if constexpr (std::is_same_v<T, Object>)
        {

            return Object(core::Check(OCI_ElementGetObject(elem)), parent);
        }
        else if constexpr (std::is_same_v<T, Reference>)
        {

            return Reference(core::Check(OCI_ElementGetReference(elem)), parent);
        }
        else if constexpr (std::is_same_v<T, Clob>)
        {

            return Clob(core::Check(OCI_ElementGetLob(elem)), parent);
        }
        else if constexpr (std::is_same_v<T, NClob>)
        {

            return NClob(core::Check(OCI_ElementGetLob(elem)), parent);
        }
        else if constexpr (std::is_same_v<T, Blob>)
        {

            return Blob(core::Check(OCI_ElementGetLob(elem)), parent);
        }
        else if constexpr (std::is_same_v<T, File>)
        {

            return File(core::Check(OCI_ElementGetFile(elem)), parent);
        }
        else if constexpr (std::is_same_v<T, Raw>)
        {

            UInt32 size = core::Check(OCI_ElementGetRawSize(elem));

            Raw value(static_cast<Size>(size + 1));

            size = core::Check(OCI_ElementGetRaw(elem, static_cast<AnyPointer>(value.data()), size));
            value.resize(size);

            return value;
        }
        else
        {
            static_assert(core::IsCollectionV<T>, "Collection::Get<T> only supports scalar types, Oracle object types and Collection<U>");

            return T(core::Check(OCI_ElementGetCollection(elem)), parent);
        }
    }

    template<class T>
    inline void Collection<T>::SetElem(OCI_Element *elem, const T &value, [[maybe_unused]] core::Handle *parent)
    {
        if constexpr (std::is_same_v<T, bool>)
        {

            core::Check(OCI_ElementSetBoolean(elem, static_cast<OCI_Boolean>(value)));
        }
        else if constexpr (std::is_same_v<T, Int16>)
        {

            core::Check(OCI_ElementSetInt16(elem, value));
        }
        else if constexpr (std::is_same_v<T, UInt16>)
        {

            core::Check(OCI_ElementSetUInt16(elem, value));
        }
        else if constexpr (std::is_same_v<T, Int32>)
        {

            core::Check(OCI_ElementSetInt32(elem, value));
        }
        else if constexpr (std::is_same_v<T, UInt32>)
        {

            core::Check(OCI_ElementSetUInt32(elem, value));
        }
        else if constexpr (std::is_same_v<T, Int64>)
        {

            core::Check(OCI_ElementSetInt64(elem, value));
        }
        else if constexpr (std::is_same_v<T, UInt64>)
        {

            core::Check(OCI_ElementSetUInt64(elem, value));
        }
        else if constexpr (std::is_same_v<T, Float>)
        {

            core::Check(OCI_ElementSetFloat(elem, value));
        }
        else if constexpr (std::is_same_v<T, Double>)
        {

            core::Check(OCI_ElementSetDouble(elem, value));
        }
        else if constexpr (std::is_same_v<T, Number>)
        {

            core::Check(OCI_ElementSetNumber(elem, value));
        }
        else if constexpr (std::is_same_v<T, String>)
        {

            core::Check(OCI_ElementSetString(elem, value.c_str()));
        }
        else if constexpr (std::is_same_v<T, Date>)
        {

            core::Check(OCI_ElementSetDate(elem, value));
        }
        else if constexpr (std::is_same_v<T, Timestamp>)
        {

            core::Check(OCI_ElementSetTimestamp(elem, value));
        }
        else if constexpr (std::is_same_v<T, Interval>)
        {

            core::Check(OCI_ElementSetInterval(elem, value));
        }
        else if constexpr (std::is_same_v<T, Object>)
        {

            core::Check(OCI_ElementSetObject(elem, value));
        }
        else if constexpr (std::is_same_v<T, Reference>)
        {

            core::Check(OCI_ElementSetReference(elem, value));
        }
        else if constexpr (std::is_same_v<T, Clob>)
        {

            core::Check(OCI_ElementSetLob(elem, value));
        }
        else if constexpr (std::is_same_v<T, NClob>)
        {

            core::Check(OCI_ElementSetLob(elem, value));
        }
        else if constexpr (std::is_same_v<T, Blob>)
        {

            core::Check(OCI_ElementSetLob(elem, value));
        }
        else if constexpr (std::is_same_v<T, File>)
        {

            core::Check(OCI_ElementSetFile(elem, value));
        }
        else if constexpr (std::is_same_v<T, Raw>)
        {

            const AnyPointer data = value.empty() ? nullptr : static_cast<AnyPointer>(const_cast<Raw::value_type*>(&value[0]));

            core::Check(OCI_ElementSetRaw(elem, data, static_cast<UInt32>(value.size())));
        }
        else
        {

            core::Check(OCI_ElementSetCollection(elem, value));
        }
    }

    template<class T>
    String Collection<T>::ToString() const
    {
        if (!IsNull())
        {
            UInt32 size{};

            core::Check(OCI_CollectionToText(*this, &size, nullptr));

            String result;

            result.resize(static_cast<Size>(size));

            core::Check(OCI_CollectionToText(*this, &size, const_cast<otext*>(result.data())));

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
