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

inline bool Resultset::Seek(SeekMode mode, int offset)
{
    return (core::Check(OCI_FetchSeek(*this, mode, offset)) == TRUE);
}

inline unsigned int Resultset::GetCount() const
{
    return core::Check(OCI_GetRowCount(*this));
}

inline unsigned int Resultset::GetCurrentRow() const
{
    return core::Check(OCI_GetCurrentRow(*this));
}

inline unsigned int Resultset::GetColumnIndex(const ostring& name) const
{
    return core::Check(OCI_GetColumnIndex(*this, name.c_str()));
}

inline unsigned int Resultset::GetColumnCount() const
{
    return core::Check(OCI_GetColumnCount(*this));
}

inline Column Resultset::GetColumn(unsigned int index) const
{
    return Column(core::Check(OCI_GetColumn(*this, index)), GetHandle());
}

inline Column Resultset::GetColumn(const ostring& name) const
{
    return Column(core::Check(OCI_GetColumn2(*this, name.c_str())), GetHandle());
}

inline bool Resultset::IsColumnNull(unsigned int index) const
{
    return (core::Check(OCI_IsNull(*this, index)) == TRUE);
}

inline bool Resultset::IsColumnNull(const ostring& name) const
{
    return (core::Check(OCI_IsNull2(*this, name.c_str())) == TRUE);
}

inline Statement Resultset::GetStatement() const
{
    return Statement( core::Check(OCI_ResultsetGetStatement(*this)), nullptr);
}

inline bool Resultset::operator ++ (int)
{
    return Next();
}

inline bool Resultset::operator -- (int)
{
    return Prev();
}

inline bool Resultset::operator += (int offset)
{
    return Seek(SeekRelative, offset);
}

inline bool Resultset::operator -= (int offset)
{
    return Seek(SeekRelative, -offset);
}

template<class T>
void Resultset::Get(unsigned int index, T& value) const
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
unsigned int Resultset::ForEach(TCallback callback)
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
unsigned int Resultset::ForEach(T callback, U adapter)
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
inline short Resultset::Get<short>(unsigned int index) const
{
    return core::Check(OCI_GetShort(*this, index));
}

template<>
inline short Resultset::Get<short>(const ostring& name) const
{
    return core::Check(OCI_GetShort2(*this, name.c_str()));
}

template<>
inline unsigned short Resultset::Get<unsigned short>(unsigned int index) const
{
    return core::Check(OCI_GetUnsignedShort(*this, index));
}

template<>
inline unsigned short Resultset::Get<unsigned short>(const ostring& name) const
{
    return core::Check(OCI_GetUnsignedShort2(*this, name.c_str()));
}

template<>
inline int Resultset::Get<int>(unsigned int index) const
{
    return core::Check(OCI_GetInt(*this, index));
}

template<>
inline int Resultset::Get<int>(const ostring& name) const
{
    return core::Check(OCI_GetInt2(*this, name.c_str()));
}

template<>
inline unsigned int Resultset::Get<unsigned int>(unsigned int index) const
{
    return core::Check(OCI_GetUnsignedInt(*this, index));
}

template<>
inline unsigned int Resultset::Get<unsigned int>(const ostring& name) const
{
    return core::Check(OCI_GetUnsignedInt2(*this, name.c_str()));
}

template<>
inline big_int Resultset::Get<big_int>(unsigned int index) const
{
    return core::Check(OCI_GetBigInt(*this, index));
}

template<>
inline big_int Resultset::Get<big_int>(const ostring& name) const
{
    return core::Check(OCI_GetBigInt2(*this, name.c_str()));
}

template<>
inline big_uint Resultset::Get<big_uint>(unsigned int index) const
{
    return core::Check(OCI_GetUnsignedBigInt(*this, index));
}

template<>
inline big_uint Resultset::Get<big_uint>(const ostring& name) const
{
    return core::Check(OCI_GetUnsignedBigInt2(*this, name.c_str()));
}

template<>
inline float Resultset::Get<float>(unsigned int index) const
{
    return core::Check(OCI_GetFloat(*this, index));
}

template<>
inline float Resultset::Get<float>(const ostring& name) const
{
    return core::Check(OCI_GetFloat2(*this, name.c_str()));
}

template<>
inline double Resultset::Get<double>(unsigned int index) const
{
    return core::Check(OCI_GetDouble(*this, index));
}

template<>
inline double Resultset::Get<double>(const ostring& name) const
{
    return core::Check(OCI_GetDouble2(*this, name.c_str()));
}

template<>
inline Number Resultset::Get<Number>(unsigned int index) const
{
    return Number(core::Check(OCI_GetNumber(*this, index)), GetHandle());
}

template<>
inline Number Resultset::Get<Number>(const ostring& name) const
{
    return Number(core::Check(OCI_GetNumber2(*this, name.c_str())), GetHandle());
}

template<>
inline ostring Resultset::Get<ostring>(unsigned int index) const
{
    return core::MakeString(core::Check(OCI_GetString(*this, index)));
}

template<>
inline ostring Resultset::Get<ostring>(const ostring& name) const
{
    return core::MakeString(core::Check(OCI_GetString2(*this,name.c_str())));
}

template<>
inline Raw Resultset::Get<Raw>(unsigned int index) const
{
    unsigned int size = core::Check(OCI_GetDataLength(*this,index));

    core::ManagedBuffer<unsigned char> buffer(static_cast<size_t>(size + 1));

    size = core::Check(OCI_GetRaw(*this, index, static_cast<AnyPointer>(buffer), size));

    return core::MakeRaw(buffer, size);
}

template<>
inline Raw Resultset::Get<Raw>(const ostring& name) const
{
    unsigned int size = core::Check(OCI_GetDataLength(*this, core::Check(OCI_GetColumnIndex(*this, name.c_str()))));

    core::ManagedBuffer<unsigned char> buffer(static_cast<size_t>(size + 1));

    size = core::Check(OCI_GetRaw2(*this, name.c_str(), static_cast<AnyPointer>(buffer), size));

    return core::MakeRaw(buffer, size);
}

template<>
inline Date Resultset::Get<Date>(unsigned int index) const
{
    return Date(core::Check(OCI_GetDate(*this, index)), GetHandle());
}

template<>
inline Date Resultset::Get<Date>(const ostring& name) const
{
    return Date(core::Check(OCI_GetDate2(*this,name.c_str())), GetHandle());
}

template<>
inline Timestamp Resultset::Get<Timestamp>(unsigned int index) const
{
    return Timestamp(core::Check(OCI_GetTimestamp(*this, index)), GetHandle());
}

template<>
inline Timestamp Resultset::Get<Timestamp>(const ostring& name) const
{
    return Timestamp(core::Check(OCI_GetTimestamp2(*this,name.c_str())), GetHandle());
}

template<>
inline Interval Resultset::Get<Interval>(unsigned int index) const
{
    return Interval(core::Check(OCI_GetInterval(*this, index)), GetHandle());
}

template<>
inline Interval Resultset::Get<Interval>(const ostring& name) const
{
    return Interval(core::Check(OCI_GetInterval2(*this,name.c_str())), GetHandle());
}

template<>
inline Object Resultset::Get<Object>(unsigned int index) const
{
    return Object(core::Check(OCI_GetObject(*this, index)), GetHandle());
}

template<>
inline Object Resultset::Get<Object>(const ostring& name) const
{
    return Object(core::Check(OCI_GetObject2(*this,name.c_str())), GetHandle());
}

template<>
inline Reference Resultset::Get<Reference>(unsigned int index) const
{
    return Reference(core::Check(OCI_GetRef(*this, index)), GetHandle());
}

template<>
inline Reference Resultset::Get<Reference>(const ostring& name) const
{
    return Reference(core::Check(OCI_GetRef2(*this,name.c_str())), GetHandle());
}

template<>
inline Statement Resultset::Get<Statement>(unsigned int index) const
{
    return Statement(core::Check(OCI_GetStatement(*this, index)), GetHandle());
}

template<>
inline Statement Resultset::Get<Statement>(const ostring& name) const
{
    return Statement(core::Check(OCI_GetStatement2(*this,name.c_str())), GetHandle());
}

template<>
inline Clob Resultset::Get<Clob>(unsigned int index) const
{
    return Clob(core::Check(OCI_GetLob(*this, index)), GetHandle());
}

template<>
inline Clob Resultset::Get<Clob>(const ostring& name) const
{
    return Clob(core::Check(OCI_GetLob2(*this,name.c_str())), GetHandle());
}

template<>
inline NClob Resultset::Get<NClob>(unsigned int index) const
{
    return NClob(core::Check(OCI_GetLob(*this, index)), GetHandle());
}

template<>
inline NClob Resultset::Get<NClob>(const ostring& name) const
{
    return NClob(core::Check(OCI_GetLob2(*this, name.c_str())), GetHandle());
}

template<>
inline Blob Resultset::Get<Blob>(unsigned int index) const
{
    return Blob(core::Check(OCI_GetLob(*this, index)), GetHandle());
}

template<>
inline Blob Resultset::Get<Blob>(const ostring& name) const
{
    return Blob(core::Check(OCI_GetLob2(*this,name.c_str())), GetHandle());
}

template<>
inline File Resultset::Get<File>(unsigned int index) const
{
    return File(core::Check(OCI_GetFile(*this, index)), GetHandle());
}

template<>
inline File Resultset::Get<File>(const ostring& name) const
{
    return File(core::Check(OCI_GetFile2(*this,name.c_str())), GetHandle());
}

template<>
inline Clong Resultset::Get<Clong>(unsigned int index) const
{
    return Clong(core::Check(OCI_GetLong(*this, index)), GetHandle());
}

template<>
inline Clong Resultset::Get<Clong>(const ostring& name) const
{
    return Clong(core::Check(OCI_GetLong2(*this,name.c_str())), GetHandle());
}

template<>
inline Blong Resultset::Get<Blong>(unsigned int index) const
{
    return Blong(core::Check(OCI_GetLong(*this, index)), GetHandle());
}

template<>
inline Blong Resultset::Get<Blong>(const ostring& name) const
{
    return Blong(core::Check(OCI_GetLong2(*this,name.c_str())), GetHandle());
}

template<class T>
T Resultset::Get(unsigned int index) const
{
    return T(core::Check(OCI_GetColl(*this, index)), GetHandle());
}

template<class T>
T Resultset::Get(const ostring& name) const
{
    return T(core::Check(OCI_GetColl2(*this, name.c_str())), GetHandle());
}

}
