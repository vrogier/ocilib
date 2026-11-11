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

    inline Resultset::Resultset()
    {
    }

    inline Resultset::Resultset(OCI_Resultset *resultset, core::Handle *parent)
    {
        AcquireTransient(resultset, parent);
    }

    inline bool Resultset::Next()
    {
        return (core::Check(OCI_ResultsetFetchNext(*this)) == TRUE);
    }

    inline bool Resultset::Prev()
    {
        return (core::Check(OCI_ResultsetFetchPrev(*this)) == TRUE);
    }

    inline bool Resultset::First()
    {
        return (core::Check(OCI_ResultsetFetchFirst(*this)) == TRUE);
    }

    inline bool Resultset::Last()
    {
        return (core::Check(OCI_ResultsetFetchLast(*this)) == TRUE);
    }

    inline bool Resultset::Seek(SeekMode mode, Int32 offset)
    {
        return (core::Check(OCI_ResultsetFetchSeek(*this, mode, offset)) == TRUE);
    }

    inline UInt32 Resultset::GetCount() const
    {
        return core::Check(OCI_ResultsetGetRowCount(*this));
    }

    inline Int32 Resultset::GetCurrentRow() const
    {
        return core::Check(OCI_ResultsetGetCurrentRow(*this));
    }

    inline Int32 Resultset::GetColumnIndex(const String& name) const
    {
        return core::Check(OCI_ResultsetGetColumnIndex(*this, name.c_str()));
    }

    inline UInt32 Resultset::GetColumnCount() const
    {
        return core::Check(OCI_ResultsetGetColumnCount(*this));
    }

    inline Column Resultset::GetColumn(UInt32 index) const
    {
        return Column(core::Check(OCI_ResultsetGetColumn(*this, index)), GetHandle());
    }

    inline Column Resultset::GetColumn(const String& name) const
    {
        return Column(core::Check(OCI_ResultsetGetColumnByName(*this, name.c_str())), GetHandle());
    }

    inline bool Resultset::IsColumnNull(UInt32 index) const
    {
        return (core::Check(OCI_ResultsetIsColumnNull(*this, index)) == TRUE);
    }

    inline bool Resultset::IsColumnNull(const String& name) const
    {
        return (core::Check(OCI_ResultsetIsColumnNullByName(*this, name.c_str())) == TRUE);
    }

    inline Statement Resultset::GetStatement() const
    {
        return Statement( core::Check(OCI_ResultsetGetStatement(*this)), nullptr);
    }

    inline bool Resultset::operator ++ (Int32)
    {
        return Next();
    }

    inline bool Resultset::operator -- (Int32)
    {
        return Prev();
    }

    inline bool Resultset::operator += (Int32 offset)
    {
        return Seek(SeekRelative, offset);
    }

    inline bool Resultset::operator -= (Int32 offset)
    {
        return Seek(SeekRelative, -offset);
    }

    inline ResultsetRows Resultset::Rows()
    {
        return ResultsetRows(this);
    }

    inline ResultsetColumns Resultset::Columns()
    {
        return ResultsetColumns(this);
    }

    template<class T>
    void Resultset::Get(UInt32 index, T& value) const
    {
        value = Get<T>(index);
    }

    template<class T>
    void Resultset::Get(const String &name, T& value) const
    {
        value = Get<T>(name);
    }

    template<class T, class TAdapter>
    bool Resultset::Get(T& value, TAdapter adapter) const
    {
        return adapter(static_cast<const Resultset&>(*this), value);
    }

    template<class TCallback>
    UInt32 Resultset::ForEach(TCallback callback)
    {
        while (Next())
        {
            if (!callback(static_cast<const Resultset&>(*this)))
            {
                break;
            }
        }

        return GetCurrentRow();
    }

    template<class T, class U>
    UInt32 Resultset::ForEach(T callback, U adapter)
    {
        while (Next())
        {
            if (!callback(adapter(static_cast<const Resultset&>(*this))))
            {
                break;
            }
        }

        return GetCurrentRow();
    }

    template<class T>
    inline std::optional<T> Resultset::GetOptional(UInt32 index) const
    {
        return IsColumnNull(index) ? std::nullopt : std::optional<T>(Get<T>(index));
    }

    template<class T>
    inline std::optional<T> Resultset::GetOptional(const String& name) const
    {
        return IsColumnNull(name) ? std::nullopt : std::optional<T>(Get<T>(name));
    }

    template<class T>
    inline T Resultset::Get(UInt32 index) const
    {
        if      constexpr (std::is_same_v<T, Int16>)  { return core::Check(OCI_ResultsetGetInt16(*this, index)); }
        else if constexpr (std::is_same_v<T, UInt16>) { return core::Check(OCI_ResultsetGetUInt16(*this, index)); }
        else if constexpr (std::is_same_v<T, Int32>)  { return core::Check(OCI_ResultsetGetInt32(*this, index)); }
        else if constexpr (std::is_same_v<T, UInt32>) { return core::Check(OCI_ResultsetGetUInt32(*this, index)); }
        else if constexpr (std::is_same_v<T, Int64>)  { return core::Check(OCI_ResultsetGetInt64(*this, index)); }
        else if constexpr (std::is_same_v<T, UInt64>) { return core::Check(OCI_ResultsetGetUInt64(*this, index)); }
        else if constexpr (std::is_same_v<T, Float>)      { return core::Check(OCI_ResultsetGetFloat(*this, index)); }
        else if constexpr (std::is_same_v<T, Double>)     { return core::Check(OCI_ResultsetGetDouble(*this, index)); }
        else if constexpr (std::is_same_v<T, Number>)     { return Number(core::Check(OCI_ResultsetGetNumber(*this, index)), GetHandle()); }
        else if constexpr (std::is_same_v<T, String>)    { return core::MakeString(core::Check(OCI_ResultsetGetString(*this, index))); }
        else if constexpr (std::is_same_v<T, Date>)       { return Date(core::Check(OCI_ResultsetGetDate(*this, index)), GetHandle()); }
        else if constexpr (std::is_same_v<T, Timestamp>)  { return Timestamp(core::Check(OCI_ResultsetGetTimestamp(*this, index)), GetHandle()); }
        else if constexpr (std::is_same_v<T, Interval>)   { return Interval(core::Check(OCI_ResultsetGetInterval(*this, index)), GetHandle()); }
        else if constexpr (std::is_same_v<T, Object>)     { return Object(core::Check(OCI_ResultsetGetObject(*this, index)), GetHandle()); }
        else if constexpr (std::is_same_v<T, Reference>)  { return Reference(core::Check(OCI_ResultsetGetReference(*this, index)), GetHandle()); }
        else if constexpr (std::is_same_v<T, XmlType>)    { return XmlType(core::Check(OCI_ResultsetGetXmlType(*this, index)), GetHandle()); }
        else if constexpr (std::is_same_v<T, Vector>)     { return Vector(core::Check(OCI_ResultsetGetVector(*this, index)), GetHandle()); }
        else if constexpr (std::is_same_v<T, Json>)       { return Json(core::Check(OCI_ResultsetGetJson(*this, index)), GetHandle()); }
        else if constexpr (std::is_same_v<T, Statement>)  { return Statement(core::Check(OCI_ResultsetGetCursor(*this, index)), GetHandle()); }
        else if constexpr (std::is_same_v<T, Clob>)       { return Clob(core::Check(OCI_ResultsetGetLob(*this, index)), GetHandle()); }
        else if constexpr (std::is_same_v<T, NClob>)      { return NClob(core::Check(OCI_ResultsetGetLob(*this, index)), GetHandle()); }
        else if constexpr (std::is_same_v<T, Blob>)       { return Blob(core::Check(OCI_ResultsetGetLob(*this, index)), GetHandle()); }
        else if constexpr (std::is_same_v<T, File>)       { return File(core::Check(OCI_ResultsetGetFile(*this, index)), GetHandle()); }
        else if constexpr (std::is_same_v<T, Clong>)      { return Clong(core::Check(OCI_ResultsetGetLong(*this, index)), GetHandle()); }
        else if constexpr (std::is_same_v<T, Blong>)      { return Blong(core::Check(OCI_ResultsetGetLong(*this, index)), GetHandle()); }
        else if constexpr (std::is_same_v<T, StringView>)
        {
            const otext* value = core::Check(OCI_ResultsetGetString(*this, index));

            return value ? StringView(value) : StringView();
        }
        else if constexpr (std::is_same_v<T, Raw>)
        {
            UInt32 size = core::Check(OCI_ResultsetGetDataLength(*this, index));

            Raw value(static_cast<Size>(size + 1));

            size = core::Check(OCI_ResultsetGetRaw(*this, index, static_cast<AnyPointer>(value.data()), size));
            value.resize(size);

            return value;
        }
        else
        {
            static_assert(core::IsCollectionV<T>, "Resultset::Get<T> only supports scalar types, Oracle object types and Collection<U>");

            return T(core::Check(OCI_ResultsetGetColl(*this, index)), GetHandle());
        }
    }

    template<class T>
    inline T Resultset::Get(const String& name) const
    {
        if      constexpr (std::is_same_v<T, Int16>)  { return core::Check(OCI_ResultsetGetInt16ByName(*this, name.c_str())); }
        else if constexpr (std::is_same_v<T, UInt16>) { return core::Check(OCI_ResultsetGetUInt16ByName(*this, name.c_str())); }
        else if constexpr (std::is_same_v<T, Int32>)  { return core::Check(OCI_ResultsetGetInt32ByName(*this, name.c_str())); }
        else if constexpr (std::is_same_v<T, UInt32>) { return core::Check(OCI_ResultsetGetUInt32ByName(*this, name.c_str())); }
        else if constexpr (std::is_same_v<T, Int64>)  { return core::Check(OCI_ResultsetGetInt64ByName(*this, name.c_str())); }
        else if constexpr (std::is_same_v<T, UInt64>) { return core::Check(OCI_ResultsetGetUInt64ByName(*this, name.c_str())); }
        else if constexpr (std::is_same_v<T, Float>)      { return core::Check(OCI_ResultsetGetFloatByName(*this, name.c_str())); }
        else if constexpr (std::is_same_v<T, Double>)     { return core::Check(OCI_ResultsetGetDoubleByName(*this, name.c_str())); }
        else if constexpr (std::is_same_v<T, Number>)     { return Number(core::Check(OCI_ResultsetGetNumberByName(*this, name.c_str())), GetHandle()); }
        else if constexpr (std::is_same_v<T, String>)    { return core::MakeString(core::Check(OCI_ResultsetGetStringByName(*this, name.c_str()))); }
        else if constexpr (std::is_same_v<T, Date>)       { return Date(core::Check(OCI_ResultsetGetDateByName(*this, name.c_str())), GetHandle()); }
        else if constexpr (std::is_same_v<T, Timestamp>)  { return Timestamp(core::Check(OCI_ResultsetGetTimestampByName(*this, name.c_str())), GetHandle()); }
        else if constexpr (std::is_same_v<T, Interval>)   { return Interval(core::Check(OCI_ResultsetGetIntervalByName(*this, name.c_str())), GetHandle()); }
        else if constexpr (std::is_same_v<T, Object>)     { return Object(core::Check(OCI_ResultsetGetObjectByName(*this, name.c_str())), GetHandle()); }
        else if constexpr (std::is_same_v<T, Reference>)  { return Reference(core::Check(OCI_ResultsetGetReferenceByName(*this, name.c_str())), GetHandle()); }
        else if constexpr (std::is_same_v<T, XmlType>)    { return XmlType(core::Check(OCI_ResultsetGetXmlTypeByName(*this, name.c_str())), GetHandle()); }
        else if constexpr (std::is_same_v<T, Vector>)     { return Vector(core::Check(OCI_ResultsetGetVectorByName(*this, name.c_str())), GetHandle()); }
        else if constexpr (std::is_same_v<T, Json>)       { return Json(core::Check(OCI_ResultsetGetJsonByName(*this, name.c_str())), GetHandle()); }
        else if constexpr (std::is_same_v<T, Statement>)  { return Statement(core::Check(OCI_ResultsetGetCursorByName(*this, name.c_str())), GetHandle()); }
        else if constexpr (std::is_same_v<T, Clob>)       { return Clob(core::Check(OCI_ResultsetGetLobByName(*this, name.c_str())), GetHandle()); }
        else if constexpr (std::is_same_v<T, NClob>)      { return NClob(core::Check(OCI_ResultsetGetLobByName(*this, name.c_str())), GetHandle()); }
        else if constexpr (std::is_same_v<T, Blob>)       { return Blob(core::Check(OCI_ResultsetGetLobByName(*this, name.c_str())), GetHandle()); }
        else if constexpr (std::is_same_v<T, File>)       { return File(core::Check(OCI_ResultsetGetFileByName(*this, name.c_str())), GetHandle()); }
        else if constexpr (std::is_same_v<T, Clong>)      { return Clong(core::Check(OCI_ResultsetGetLongByName(*this, name.c_str())), GetHandle()); }
        else if constexpr (std::is_same_v<T, Blong>)      { return Blong(core::Check(OCI_ResultsetGetLongByName(*this, name.c_str())), GetHandle()); }
        else if constexpr (std::is_same_v<T, StringView>)
        {
            const otext* value = core::Check(OCI_ResultsetGetStringByName(*this, name.c_str()));

            return value ? StringView(value) : StringView();
        }
        else if constexpr (std::is_same_v<T, Raw>)
        {
            UInt32 size = core::Check(OCI_ResultsetGetDataLength(*this, core::Check(OCI_ResultsetGetColumnIndex(*this, name.c_str()))));

            Raw value(static_cast<Size>(size + 1));

            size = core::Check(OCI_ResultsetGetRawByName(*this, name.c_str(), static_cast<AnyPointer>(value.data()), size));
            value.resize(size);

            return value;
        }
        else
        {
            static_assert(core::IsCollectionV<T>, "Resultset::Get<T> only supports scalar types, Oracle object types and Collection<U>");

            return T(core::Check(OCI_ResultsetGetCollByName(*this, name.c_str())), GetHandle());
        }
    }

}
