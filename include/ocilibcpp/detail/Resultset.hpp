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

inline bool Resultset::Seek(SeekMode mode, OCI_Int32 offset)
{
    return (core::Check(OCI_FetchSeek(*this, mode, offset)) == TRUE);
}

inline OCI_UInt32 Resultset::GetCount() const
{
    return core::Check(OCI_GetRowCount(*this));
}

inline OCI_UInt32 Resultset::GetCurrentRow() const
{
    return core::Check(OCI_GetCurrentRow(*this));
}

inline OCI_UInt32 Resultset::GetColumnIndex(const ostring& name) const
{
    return core::Check(OCI_GetColumnIndex(*this, name.c_str()));
}

inline OCI_UInt32 Resultset::GetColumnCount() const
{
    return core::Check(OCI_GetColumnCount(*this));
}

inline Column Resultset::GetColumn(OCI_UInt32 index) const
{
    return Column(core::Check(OCI_GetColumn(*this, index)), GetHandle());
}

inline Column Resultset::GetColumn(const ostring& name) const
{
    return Column(core::Check(OCI_GetColumnByName(*this, name.c_str())), GetHandle());
}

inline bool Resultset::IsColumnNull(OCI_UInt32 index) const
{
    return (core::Check(OCI_IsNull(*this, index)) == TRUE);
}

inline bool Resultset::IsColumnNull(const ostring& name) const
{
    return (core::Check(OCI_IsNullByName(*this, name.c_str())) == TRUE);
}

inline Statement Resultset::GetStatement() const
{
    return Statement( core::Check(OCI_ResultsetGetStatement(*this)), nullptr);
}

inline bool Resultset::operator ++ (OCI_Int32)
{
    return Next();
}

inline bool Resultset::operator -- (OCI_Int32)
{
    return Prev();
}

inline bool Resultset::operator += (OCI_Int32 offset)
{
    return Seek(SeekRelative, offset);
}

inline bool Resultset::operator -= (OCI_Int32 offset)
{
    return Seek(SeekRelative, -offset);
}

template<class T>
void Resultset::Get(OCI_UInt32 index, T& value) const
{
    value = Get<T>(index);
}

template<class T>
void Resultset::Get(const ostring &name, T& value) const
{
    value = Get<T>(name);
}

template<class T, class TAdapter>
bool Resultset::Get(T& value, TAdapter adapter) const
{
    return adapter(static_cast<const Resultset&>(*this), value);
}

template<class TCallback>
OCI_UInt32 Resultset::ForEach(TCallback callback)
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
OCI_UInt32 Resultset::ForEach(T callback, U adapter)
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

template<>
inline OCI_Int16 Resultset::Get<OCI_Int16>(OCI_UInt32 index) const
{
    return core::Check(OCI_GetShort(*this, index));
}

template<>
inline OCI_Int16 Resultset::Get<OCI_Int16>(const ostring& name) const
{
    return core::Check(OCI_GetShortByName(*this, name.c_str()));
}

template<>
inline OCI_UInt16 Resultset::Get<OCI_UInt16>(OCI_UInt32 index) const
{
    return core::Check(OCI_GetUnsignedShort(*this, index));
}

template<>
inline OCI_UInt16 Resultset::Get<OCI_UInt16>(const ostring& name) const
{
    return core::Check(OCI_GetUnsignedShortByName(*this, name.c_str()));
}

template<>
inline OCI_Int32 Resultset::Get<OCI_Int32>(OCI_UInt32 index) const
{
    return core::Check(OCI_GetInt(*this, index));
}

template<>
inline OCI_Int32 Resultset::Get<OCI_Int32>(const ostring& name) const
{
    return core::Check(OCI_GetIntByName(*this, name.c_str()));
}

template<>
inline OCI_UInt32 Resultset::Get<OCI_UInt32>(OCI_UInt32 index) const
{
    return core::Check(OCI_GetUnsignedInt(*this, index));
}

template<>
inline OCI_UInt32 Resultset::Get<OCI_UInt32>(const ostring& name) const
{
    return core::Check(OCI_GetUnsignedIntByName(*this, name.c_str()));
}

template<>
inline OCI_Int64 Resultset::Get<OCI_Int64>(OCI_UInt32 index) const
{
    return core::Check(OCI_GetBigInt(*this, index));
}

template<>
inline OCI_Int64 Resultset::Get<OCI_Int64>(const ostring& name) const
{
    return core::Check(OCI_GetBigIntByName(*this, name.c_str()));
}

template<>
inline OCI_UInt64 Resultset::Get<OCI_UInt64>(OCI_UInt32 index) const
{
    return core::Check(OCI_GetUnsignedBigInt(*this, index));
}

template<>
inline OCI_UInt64 Resultset::Get<OCI_UInt64>(const ostring& name) const
{
    return core::Check(OCI_GetUnsignedBigIntByName(*this, name.c_str()));
}

template<>
inline float Resultset::Get<float>(OCI_UInt32 index) const
{
    return core::Check(OCI_GetFloat(*this, index));
}

template<>
inline float Resultset::Get<float>(const ostring& name) const
{
    return core::Check(OCI_GetFloatByName(*this, name.c_str()));
}

template<>
inline double Resultset::Get<double>(OCI_UInt32 index) const
{
    return core::Check(OCI_GetDouble(*this, index));
}

template<>
inline double Resultset::Get<double>(const ostring& name) const
{
    return core::Check(OCI_GetDoubleByName(*this, name.c_str()));
}

template<>
inline Number Resultset::Get<Number>(OCI_UInt32 index) const
{
    return Number(core::Check(OCI_GetNumber(*this, index)), GetHandle());
}

template<>
inline Number Resultset::Get<Number>(const ostring& name) const
{
    return Number(core::Check(OCI_GetNumberByName(*this, name.c_str())), GetHandle());
}

template<>
inline ostring Resultset::Get<ostring>(OCI_UInt32 index) const
{
    return core::MakeString(core::Check(OCI_GetString(*this, index)));
}

template<>
inline ostring Resultset::Get<ostring>(const ostring& name) const
{
    return core::MakeString(core::Check(OCI_GetStringByName(*this,name.c_str())));
}

template<>
inline Raw Resultset::Get<Raw>(OCI_UInt32 index) const
{
    OCI_UInt32 size = core::Check(OCI_GetDataLength(*this,index));

    std::vector<unsigned char> buffer(static_cast<OCI_Size>(size + 1));

    size = core::Check(OCI_GetRaw(*this, index, static_cast<AnyPointer>(buffer.data()), size));

    return core::MakeRaw(buffer.data(), size);
}

template<>
inline Raw Resultset::Get<Raw>(const ostring& name) const
{
    OCI_UInt32 size = core::Check(OCI_GetDataLength(*this, core::Check(OCI_GetColumnIndex(*this, name.c_str()))));

    std::vector<unsigned char> buffer(static_cast<OCI_Size>(size + 1));

    size = core::Check(OCI_GetRawByName(*this, name.c_str(), static_cast<AnyPointer>(buffer.data()), size));

    return core::MakeRaw(buffer.data(), size);
}

template<>
inline Date Resultset::Get<Date>(OCI_UInt32 index) const
{
    return Date(core::Check(OCI_GetDate(*this, index)), GetHandle());
}

template<>
inline Date Resultset::Get<Date>(const ostring& name) const
{
    return Date(core::Check(OCI_GetDateByName(*this,name.c_str())), GetHandle());
}

template<>
inline Timestamp Resultset::Get<Timestamp>(OCI_UInt32 index) const
{
    return Timestamp(core::Check(OCI_GetTimestamp(*this, index)), GetHandle());
}

template<>
inline Timestamp Resultset::Get<Timestamp>(const ostring& name) const
{
    return Timestamp(core::Check(OCI_GetTimestampByName(*this,name.c_str())), GetHandle());
}

template<>
inline Interval Resultset::Get<Interval>(OCI_UInt32 index) const
{
    return Interval(core::Check(OCI_GetInterval(*this, index)), GetHandle());
}

template<>
inline Interval Resultset::Get<Interval>(const ostring& name) const
{
    return Interval(core::Check(OCI_GetIntervalByName(*this,name.c_str())), GetHandle());
}

template<>
inline Object Resultset::Get<Object>(OCI_UInt32 index) const
{
    return Object(core::Check(OCI_GetObject(*this, index)), GetHandle());
}

template<>
inline Object Resultset::Get<Object>(const ostring& name) const
{
    return Object(core::Check(OCI_GetObjectByName(*this,name.c_str())), GetHandle());
}

template<>
inline Reference Resultset::Get<Reference>(OCI_UInt32 index) const
{
    return Reference(core::Check(OCI_GetRef(*this, index)), GetHandle());
}

template<>
inline Reference Resultset::Get<Reference>(const ostring& name) const
{
    return Reference(core::Check(OCI_GetRefByName(*this,name.c_str())), GetHandle());
}

template<>
inline XmlType Resultset::Get<XmlType>(OCI_UInt32 index) const
{
    return XmlType(core::Check(OCI_GetXmlType(*this, index)), GetHandle());
}

template<>
inline XmlType Resultset::Get<XmlType>(const ostring& name) const
{
    return XmlType(core::Check(OCI_GetXmlTypeByName(*this,name.c_str())), GetHandle());
}

template<>
inline Vector Resultset::Get<Vector>(OCI_UInt32 index) const
{
    return Vector(core::Check(OCI_GetVector(*this, index)), GetHandle());
}

template<>
inline Vector Resultset::Get<Vector>(const ostring& name) const
{
    return Vector(core::Check(OCI_GetVectorByName(*this,name.c_str())), GetHandle());
}

template<>
inline Json Resultset::Get<Json>(OCI_UInt32 index) const
{
    return Json(core::Check(OCI_GetJson(*this, index)), GetHandle());
}

template<>
inline Json Resultset::Get<Json>(const ostring& name) const
{
    return Json(core::Check(OCI_GetJsonByName(*this, name.c_str())), GetHandle());
}

template<>
inline Statement Resultset::Get<Statement>(OCI_UInt32 index) const
{
    return Statement(core::Check(OCI_GetStatement(*this, index)), GetHandle());
}

template<>
inline Statement Resultset::Get<Statement>(const ostring& name) const
{
    return Statement(core::Check(OCI_GetStatementByName(*this,name.c_str())), GetHandle());
}

template<>
inline Clob Resultset::Get<Clob>(OCI_UInt32 index) const
{
    return Clob(core::Check(OCI_GetLob(*this, index)), GetHandle());
}

template<>
inline Clob Resultset::Get<Clob>(const ostring& name) const
{
    return Clob(core::Check(OCI_GetLobByName(*this,name.c_str())), GetHandle());
}

template<>
inline NClob Resultset::Get<NClob>(OCI_UInt32 index) const
{
    return NClob(core::Check(OCI_GetLob(*this, index)), GetHandle());
}

template<>
inline NClob Resultset::Get<NClob>(const ostring& name) const
{
    return NClob(core::Check(OCI_GetLobByName(*this, name.c_str())), GetHandle());
}

template<>
inline Blob Resultset::Get<Blob>(OCI_UInt32 index) const
{
    return Blob(core::Check(OCI_GetLob(*this, index)), GetHandle());
}

template<>
inline Blob Resultset::Get<Blob>(const ostring& name) const
{
    return Blob(core::Check(OCI_GetLobByName(*this,name.c_str())), GetHandle());
}

template<>
inline File Resultset::Get<File>(OCI_UInt32 index) const
{
    return File(core::Check(OCI_GetFile(*this, index)), GetHandle());
}

template<>
inline File Resultset::Get<File>(const ostring& name) const
{
    return File(core::Check(OCI_GetFileByName(*this,name.c_str())), GetHandle());
}

template<>
inline Clong Resultset::Get<Clong>(OCI_UInt32 index) const
{
    return Clong(core::Check(OCI_GetLong(*this, index)), GetHandle());
}

template<>
inline Clong Resultset::Get<Clong>(const ostring& name) const
{
    return Clong(core::Check(OCI_GetLongByName(*this,name.c_str())), GetHandle());
}

template<>
inline Blong Resultset::Get<Blong>(OCI_UInt32 index) const
{
    return Blong(core::Check(OCI_GetLong(*this, index)), GetHandle());
}

template<>
inline Blong Resultset::Get<Blong>(const ostring& name) const
{
    return Blong(core::Check(OCI_GetLongByName(*this,name.c_str())), GetHandle());
}

template<class T>
T Resultset::Get(OCI_UInt32 index) const
{
    return T(core::Check(OCI_GetColl(*this, index)), GetHandle());
}

template<class T>
T Resultset::Get(const ostring& name) const
{
    return T(core::Check(OCI_GetCollByName(*this, name.c_str())), GetHandle());
}

}
