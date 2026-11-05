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
    return (core::Check(OCI_FetchNext(*this)) == TRUE);
}

inline bool Resultset::Prev()
{
    return (core::Check(OCI_FetchPrev(*this)) == TRUE);
}

inline bool Resultset::First()
{
    return (core::Check(OCI_FetchFirst(*this)) == TRUE);
}

inline bool Resultset::Last()
{
    return (core::Check(OCI_FetchLast(*this)) == TRUE);
}

inline bool Resultset::Seek(SeekMode mode, Int32 offset)
{
    return (core::Check(OCI_FetchSeek(*this, mode, offset)) == TRUE);
}

inline UInt32 Resultset::GetCount() const
{
    return core::Check(OCI_GetRowCount(*this));
}

inline UInt32 Resultset::GetCurrentRow() const
{
    return core::Check(OCI_GetCurrentRow(*this));
}

inline UInt32 Resultset::GetColumnIndex(const String& name) const
{
    return core::Check(OCI_GetColumnIndex(*this, name.c_str()));
}

inline UInt32 Resultset::GetColumnCount() const
{
    return core::Check(OCI_GetColumnCount(*this));
}

inline Column Resultset::GetColumn(UInt32 index) const
{
    return Column(core::Check(OCI_GetColumn(*this, index)), GetHandle());
}

inline Column Resultset::GetColumn(const String& name) const
{
    return Column(core::Check(OCI_GetColumnByName(*this, name.c_str())), GetHandle());
}

inline bool Resultset::IsColumnNull(UInt32 index) const
{
    return (core::Check(OCI_IsNull(*this, index)) == TRUE);
}

inline bool Resultset::IsColumnNull(const String& name) const
{
    return (core::Check(OCI_IsNullByName(*this, name.c_str())) == TRUE);
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

inline ResultsetIterator Resultset::begin()
{
    return ResultsetIterator(this, !Next());
}

inline ResultsetIterator Resultset::end()
{
    return ResultsetIterator(this, true);
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
    if      constexpr (std::is_same_v<T, Int16>)  { return core::Check(OCI_GetShort(*this, index)); }
    else if constexpr (std::is_same_v<T, UInt16>) { return core::Check(OCI_GetUnsignedShort(*this, index)); }
    else if constexpr (std::is_same_v<T, Int32>)  { return core::Check(OCI_GetInt(*this, index)); }
    else if constexpr (std::is_same_v<T, UInt32>) { return core::Check(OCI_GetUnsignedInt(*this, index)); }
    else if constexpr (std::is_same_v<T, Int64>)  { return core::Check(OCI_GetBigInt(*this, index)); }
    else if constexpr (std::is_same_v<T, UInt64>) { return core::Check(OCI_GetUnsignedBigInt(*this, index)); }
    else if constexpr (std::is_same_v<T, Float>)      { return core::Check(OCI_GetFloat(*this, index)); }
    else if constexpr (std::is_same_v<T, Double>)     { return core::Check(OCI_GetDouble(*this, index)); }
    else if constexpr (std::is_same_v<T, Number>)     { return Number(core::Check(OCI_GetNumber(*this, index)), GetHandle()); }
    else if constexpr (std::is_same_v<T, String>)    { return core::MakeString(core::Check(OCI_GetString(*this, index))); }
    else if constexpr (std::is_same_v<T, Date>)       { return Date(core::Check(OCI_GetDate(*this, index)), GetHandle()); }
    else if constexpr (std::is_same_v<T, Timestamp>)  { return Timestamp(core::Check(OCI_GetTimestamp(*this, index)), GetHandle()); }
    else if constexpr (std::is_same_v<T, Interval>)   { return Interval(core::Check(OCI_GetInterval(*this, index)), GetHandle()); }
    else if constexpr (std::is_same_v<T, Object>)     { return Object(core::Check(OCI_GetObject(*this, index)), GetHandle()); }
    else if constexpr (std::is_same_v<T, Reference>)  { return Reference(core::Check(OCI_GetRef(*this, index)), GetHandle()); }
    else if constexpr (std::is_same_v<T, XmlType>)    { return XmlType(core::Check(OCI_GetXmlType(*this, index)), GetHandle()); }
    else if constexpr (std::is_same_v<T, Vector>)     { return Vector(core::Check(OCI_GetVector(*this, index)), GetHandle()); }
    else if constexpr (std::is_same_v<T, Json>)       { return Json(core::Check(OCI_GetJson(*this, index)), GetHandle()); }
    else if constexpr (std::is_same_v<T, Statement>)  { return Statement(core::Check(OCI_GetStatement(*this, index)), GetHandle()); }
    else if constexpr (std::is_same_v<T, Clob>)       { return Clob(core::Check(OCI_GetLob(*this, index)), GetHandle()); }
    else if constexpr (std::is_same_v<T, NClob>)      { return NClob(core::Check(OCI_GetLob(*this, index)), GetHandle()); }
    else if constexpr (std::is_same_v<T, Blob>)       { return Blob(core::Check(OCI_GetLob(*this, index)), GetHandle()); }
    else if constexpr (std::is_same_v<T, File>)       { return File(core::Check(OCI_GetFile(*this, index)), GetHandle()); }
    else if constexpr (std::is_same_v<T, Clong>)      { return Clong(core::Check(OCI_GetLong(*this, index)), GetHandle()); }
    else if constexpr (std::is_same_v<T, Blong>)      { return Blong(core::Check(OCI_GetLong(*this, index)), GetHandle()); }
    else if constexpr (std::is_same_v<T, Raw>)
    {
        UInt32 size = core::Check(OCI_GetDataLength(*this, index));

        Raw value(static_cast<Size>(size + 1));

        size = core::Check(OCI_GetRaw(*this, index, static_cast<AnyPointer>(value.data()), size));
        value.resize(size);

        return value;
    }
    else
    {
        static_assert(core::IsCollectionV<T>, "Resultset::Get<T> only supports scalar types, Oracle object types and Collection<U>");

        return T(core::Check(OCI_GetColl(*this, index)), GetHandle()); }
}

template<class T>
inline T Resultset::Get(const String& name) const
{
    if      constexpr (std::is_same_v<T, Int16>)  { return core::Check(OCI_GetShortByName(*this, name.c_str())); }
    else if constexpr (std::is_same_v<T, UInt16>) { return core::Check(OCI_GetUnsignedShortByName(*this, name.c_str())); }
    else if constexpr (std::is_same_v<T, Int32>)  { return core::Check(OCI_GetIntByName(*this, name.c_str())); }
    else if constexpr (std::is_same_v<T, UInt32>) { return core::Check(OCI_GetUnsignedIntByName(*this, name.c_str())); }
    else if constexpr (std::is_same_v<T, Int64>)  { return core::Check(OCI_GetBigIntByName(*this, name.c_str())); }
    else if constexpr (std::is_same_v<T, UInt64>) { return core::Check(OCI_GetUnsignedBigIntByName(*this, name.c_str())); }
    else if constexpr (std::is_same_v<T, Float>)      { return core::Check(OCI_GetFloatByName(*this, name.c_str())); }
    else if constexpr (std::is_same_v<T, Double>)     { return core::Check(OCI_GetDoubleByName(*this, name.c_str())); }
    else if constexpr (std::is_same_v<T, Number>)     { return Number(core::Check(OCI_GetNumberByName(*this, name.c_str())), GetHandle()); }
    else if constexpr (std::is_same_v<T, String>)    { return core::MakeString(core::Check(OCI_GetStringByName(*this, name.c_str()))); }
    else if constexpr (std::is_same_v<T, Date>)       { return Date(core::Check(OCI_GetDateByName(*this, name.c_str())), GetHandle()); }
    else if constexpr (std::is_same_v<T, Timestamp>)  { return Timestamp(core::Check(OCI_GetTimestampByName(*this, name.c_str())), GetHandle()); }
    else if constexpr (std::is_same_v<T, Interval>)   { return Interval(core::Check(OCI_GetIntervalByName(*this, name.c_str())), GetHandle()); }
    else if constexpr (std::is_same_v<T, Object>)     { return Object(core::Check(OCI_GetObjectByName(*this, name.c_str())), GetHandle()); }
    else if constexpr (std::is_same_v<T, Reference>)  { return Reference(core::Check(OCI_GetRefByName(*this, name.c_str())), GetHandle()); }
    else if constexpr (std::is_same_v<T, XmlType>)    { return XmlType(core::Check(OCI_GetXmlTypeByName(*this, name.c_str())), GetHandle()); }
    else if constexpr (std::is_same_v<T, Vector>)     { return Vector(core::Check(OCI_GetVectorByName(*this, name.c_str())), GetHandle()); }
    else if constexpr (std::is_same_v<T, Json>)       { return Json(core::Check(OCI_GetJsonByName(*this, name.c_str())), GetHandle()); }
    else if constexpr (std::is_same_v<T, Statement>)  { return Statement(core::Check(OCI_GetStatementByName(*this, name.c_str())), GetHandle()); }
    else if constexpr (std::is_same_v<T, Clob>)       { return Clob(core::Check(OCI_GetLobByName(*this, name.c_str())), GetHandle()); }
    else if constexpr (std::is_same_v<T, NClob>)      { return NClob(core::Check(OCI_GetLobByName(*this, name.c_str())), GetHandle()); }
    else if constexpr (std::is_same_v<T, Blob>)       { return Blob(core::Check(OCI_GetLobByName(*this, name.c_str())), GetHandle()); }
    else if constexpr (std::is_same_v<T, File>)       { return File(core::Check(OCI_GetFileByName(*this, name.c_str())), GetHandle()); }
    else if constexpr (std::is_same_v<T, Clong>)      { return Clong(core::Check(OCI_GetLongByName(*this, name.c_str())), GetHandle()); }
    else if constexpr (std::is_same_v<T, Blong>)      { return Blong(core::Check(OCI_GetLongByName(*this, name.c_str())), GetHandle()); }
    else if constexpr (std::is_same_v<T, Raw>)
    {
        UInt32 size = core::Check(OCI_GetDataLength(*this, core::Check(OCI_GetColumnIndex(*this, name.c_str()))));

        Raw value(static_cast<Size>(size + 1));

        size = core::Check(OCI_GetRawByName(*this, name.c_str(), static_cast<AnyPointer>(value.data()), size));
        value.resize(size);

        return value;
    }
    else
    {
        static_assert(core::IsCollectionV<T>, "Resultset::Get<T> only supports scalar types, Oracle object types and Collection<U>");

        return T(core::Check(OCI_GetCollByName(*this, name.c_str())), GetHandle()); }
}

}
