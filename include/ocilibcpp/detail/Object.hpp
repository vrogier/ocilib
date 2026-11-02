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

// ReSharper disable CppClangTidyHicppUseEqualsDefault
// ReSharper disable CppClangTidyModernizeUseEqualsDefault
// ReSharper disable CppClangTidyMiscMisplacedConst

namespace ocilib
{

inline Object::Object()
{
}

inline Object::Object(const TypeInfo &typeInfo)
{
    Connection connection = typeInfo.GetConnection();
    AcquireAllocated
    (
        core::Check(OCI_ObjectCreate(typeInfo)),
        connection.GetHandle()
    );
}

inline Object::Object(OCI_Object *pObject, core::Handle *parent)
{
    AcquireTransient(pObject, parent);
}

inline Object Object::Clone() const
{
    Object result;
    TypeInfo typeInfo = GetTypeInfo();
    Connection connection = typeInfo.GetConnection();
    result.AcquireAllocated
    (
        core::Check(OCI_ObjectCreate(typeInfo)),
        connection.GetHandle()
    );

    core::Check(OCI_ObjectAssign(result, *this));

    return result;
}

inline bool Object::IsAttributeNull(const ostring& name) const
{
    return (core::Check(OCI_ObjectIsNull(*this, name.c_str())) == TRUE);
}

inline void Object::SetAttributeNull(const ostring& name)
{
    core::Check(OCI_ObjectSetNull(*this, name.c_str()));
}

inline TypeInfo Object::GetTypeInfo() const
{
    OCI_TypeInfo* typeInfo = core::Check(OCI_ObjectGetTypeInfo(*this));
  
    Connection connection
    (
        core::Check(OCI_TypeInfoGetConnection(typeInfo)),
        GetHandle(),
        false
    );

    return TypeInfo(typeInfo, connection.GetHandle());
}

inline Reference Object::GetReference() const
{
    TypeInfo typeInfo = GetTypeInfo();

    OCI_Ref *pRef = OCI_RefCreate(typeInfo);

    core::Check(OCI_ObjectGetSelfRef(*this, pRef));

    return Reference(pRef, GetHandle());
}

inline Object::ObjectType Object::GetType() const
{
    return ObjectType(static_cast<ObjectType::Type>(core::Check(OCI_ObjectGetType(*this))));
}

template<class T>
inline T Object::Get(const ostring& name) const
{
    if      constexpr (std::is_same_v<T, bool>)      { return (core::Check(OCI_ObjectGetBoolean(*this, name.c_str())) == TRUE); }
    else if constexpr (std::is_same_v<T, Int16>)  { return core::Check(OCI_ObjectGetShort(*this, name.c_str())); }
    else if constexpr (std::is_same_v<T, UInt16>) { return core::Check(OCI_ObjectGetUnsignedShort(*this, name.c_str())); }
    else if constexpr (std::is_same_v<T, Int32>)  { return core::Check(OCI_ObjectGetInt(*this, name.c_str())); }
    else if constexpr (std::is_same_v<T, UInt32>) { return core::Check(OCI_ObjectGetUnsignedInt(*this, name.c_str())); }
    else if constexpr (std::is_same_v<T, Int64>)  { return core::Check(OCI_ObjectGetBigInt(*this, name.c_str())); }
    else if constexpr (std::is_same_v<T, UInt64>) { return core::Check(OCI_ObjectGetUnsignedBigInt(*this, name.c_str())); }
    else if constexpr (std::is_same_v<T, Float>)      { return core::Check(OCI_ObjectGetFloat(*this, name.c_str())); }
    else if constexpr (std::is_same_v<T, Double>)     { return core::Check(OCI_ObjectGetDouble(*this, name.c_str())); }
    else if constexpr (std::is_same_v<T, Number>)     { return Number(core::Check(OCI_ObjectGetNumber(*this, name.c_str())), GetHandle()); }
    else if constexpr (std::is_same_v<T, ostring>)    { return core::MakeString(core::Check(OCI_ObjectGetString(*this, name.c_str()))); }
    else if constexpr (std::is_same_v<T, Date>)       { return Date(core::Check(OCI_ObjectGetDate(*this, name.c_str())), GetHandle()); }
    else if constexpr (std::is_same_v<T, Timestamp>)  { return Timestamp(core::Check(OCI_ObjectGetTimestamp(*this, name.c_str())), GetHandle()); }
    else if constexpr (std::is_same_v<T, Interval>)   { return Interval(core::Check(OCI_ObjectGetInterval(*this, name.c_str())), GetHandle()); }
    else if constexpr (std::is_same_v<T, Object>)     { return Object(core::Check(OCI_ObjectGetObject(*this, name.c_str())), GetHandle()); }
    else if constexpr (std::is_same_v<T, Reference>)  { return Reference(core::Check(OCI_ObjectGetRef(*this, name.c_str())), GetHandle()); }
    else if constexpr (std::is_same_v<T, Clob>)       { return Clob(core::Check(OCI_ObjectGetLob(*this, name.c_str())), GetHandle()); }
    else if constexpr (std::is_same_v<T, NClob>)      { return NClob(core::Check(OCI_ObjectGetLob(*this, name.c_str())), GetHandle()); }
    else if constexpr (std::is_same_v<T, Blob>)       { return Blob(core::Check(OCI_ObjectGetLob(*this, name.c_str())), GetHandle()); }
    else if constexpr (std::is_same_v<T, File>)       { return File(core::Check(OCI_ObjectGetFile(*this, name.c_str())), GetHandle()); }
    else if constexpr (std::is_same_v<T, Raw>)
    {
        UInt32 size = core::Check(OCI_ObjectGetRawSize(*this, name.c_str()));

        std::vector<unsigned char> buffer(static_cast<Size>(size + 1));

        size = static_cast<UInt32>(core::Check(OCI_ObjectGetRaw(*this, name.c_str(), static_cast<AnyPointer>(buffer.data()), size)));

        return core::MakeRaw(buffer.data(), size);
    }
    else
    {
        return T(core::Check(OCI_ObjectGetColl(*this, name.c_str())), GetHandle()); }
}

template<class T>
inline void Object::Set(const ostring& name, const T &value)
{
    if      constexpr (std::is_same_v<T, bool>)      { core::Check(OCI_ObjectSetBoolean(*this, name.c_str(), static_cast<OCI_Boolean>(value))); }
    else if constexpr (std::is_same_v<T, Int16>)  { core::Check(OCI_ObjectSetShort(*this, name.c_str(), value)); }
    else if constexpr (std::is_same_v<T, UInt16>) { core::Check(OCI_ObjectSetUnsignedShort(*this, name.c_str(), value)); }
    else if constexpr (std::is_same_v<T, Int32>)  { core::Check(OCI_ObjectSetInt(*this, name.c_str(), value)); }
    else if constexpr (std::is_same_v<T, UInt32>) { core::Check(OCI_ObjectSetUnsignedInt(*this, name.c_str(), value)); }
    else if constexpr (std::is_same_v<T, Int64>)  { core::Check(OCI_ObjectSetBigInt(*this, name.c_str(), value)); }
    else if constexpr (std::is_same_v<T, UInt64>) { core::Check(OCI_ObjectSetUnsignedBigInt(*this, name.c_str(), value)); }
    else if constexpr (std::is_same_v<T, Float>)      { core::Check(OCI_ObjectSetFloat(*this, name.c_str(), value)); }
    else if constexpr (std::is_same_v<T, Double>)     { core::Check(OCI_ObjectSetDouble(*this, name.c_str(), value)); }
    else if constexpr (std::is_same_v<T, Number>)     { core::Check(OCI_ObjectSetNumber(*this, name.c_str(), value)); }
    else if constexpr (std::is_same_v<T, ostring>)    { core::Check(OCI_ObjectSetString(*this, name.c_str(), value.c_str())); }
    else if constexpr (std::is_same_v<T, Date>)       { core::Check(OCI_ObjectSetDate(*this, name.c_str(), value)); }
    else if constexpr (std::is_same_v<T, Timestamp>)  { core::Check(OCI_ObjectSetTimestamp(*this, name.c_str(), value)); }
    else if constexpr (std::is_same_v<T, Interval>)   { core::Check(OCI_ObjectSetInterval(*this, name.c_str(), value)); }
    else if constexpr (std::is_same_v<T, Object>)     { core::Check(OCI_ObjectSetObject(*this, name.c_str(), value)); }
    else if constexpr (std::is_same_v<T, Reference>)  { core::Check(OCI_ObjectSetRef(*this, name.c_str(), value)); }
    else if constexpr (std::is_same_v<T, Clob>)       { core::Check(OCI_ObjectSetLob(*this, name.c_str(), value)); }
    else if constexpr (std::is_same_v<T, NClob>)      { core::Check(OCI_ObjectSetLob(*this, name.c_str(), value)); }
    else if constexpr (std::is_same_v<T, Blob>)       { core::Check(OCI_ObjectSetLob(*this, name.c_str(), value)); }
    else if constexpr (std::is_same_v<T, File>)       { core::Check(OCI_ObjectSetFile(*this, name.c_str(), value)); }
    else if constexpr (std::is_same_v<T, Raw>)
    {
        const AnyPointer data = value.empty() ? nullptr : static_cast<AnyPointer>(const_cast<Raw::value_type*>(&value[0])) ;

        core::Check(OCI_ObjectSetRaw(*this, name.c_str(), data, static_cast<UInt32>(value.size())));
    }
    else
    {
        core::Check(OCI_ObjectSetColl(*this, name.c_str(), value)); }
}

inline ostring Object::ToString() const
{
    if (!IsNull())
    {       
        UInt32 size{};
        
        core::Check(OCI_ObjectToText(*this, &size, nullptr));

        ostring result;

        result.resize(static_cast<Size>(size));

        core::Check(OCI_ObjectToText(*this, &size, const_cast<otext*>(result.data())));

        result.resize(static_cast<Size>(size));

        return result;
    }

    return OCI_STRING_NULL;
}

}
