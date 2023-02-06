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
        core::Check(OCI_ObjectCreate(connection, typeInfo)),
        connection.GetHandle()
    );
}

inline Object::Object(OCI_Object *pObject, core::Handle *parent)
{
    AcquireTransient(pObject, parent);
}

inline Object Object::Clone() const
{
    Object result(GetTypeInfo());

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
        Environment::GetEnvironmentHandle()
    );

    return TypeInfo(typeInfo, connection.GetHandle());
}

inline Reference Object::GetReference() const
{
    TypeInfo typeInfo = GetTypeInfo();
    Connection connection = typeInfo.GetConnection();

    OCI_Ref *pRef = OCI_RefCreate(connection, typeInfo);

    core::Check(OCI_ObjectGetSelfRef(*this, pRef));

    return Reference(pRef, GetHandle());
}

inline Object::ObjectType Object::GetType() const
{
    return ObjectType(static_cast<ObjectType::Type>(core::Check(OCI_ObjectGetType(*this))));
}

template<>
inline bool Object::Get<bool>(const ostring& name) const
{
    return (core::Check(OCI_ObjectGetBoolean(*this, name.c_str())) == TRUE);
}

template<>
inline short Object::Get<short>(const ostring& name) const
{
    return core::Check(OCI_ObjectGetShort(*this, name.c_str()));
}

template<>
inline unsigned short Object::Get<unsigned short>(const ostring& name) const
{
    return core::Check(OCI_ObjectGetUnsignedShort(*this, name.c_str()));
}

template<>
inline int Object::Get<int>(const ostring& name) const
{
    return core::Check(OCI_ObjectGetInt(*this, name.c_str()));
}

template<>
inline unsigned int Object::Get<unsigned int>(const ostring& name) const
{
    return core::Check(OCI_ObjectGetUnsignedInt(*this, name.c_str()));
}

template<>
inline big_int Object::Get<big_int>(const ostring& name) const
{
    return core::Check(OCI_ObjectGetBigInt(*this, name.c_str()));
}

template<>
inline big_uint Object::Get<big_uint>(const ostring& name) const
{
    return core::Check(OCI_ObjectGetUnsignedBigInt(*this, name.c_str()));
}

template<>
inline float Object::Get<float>(const ostring& name) const
{
    return core::Check(OCI_ObjectGetFloat(*this, name.c_str()));
}

template<>
inline double Object::Get<double>(const ostring& name) const
{
    return core::Check(OCI_ObjectGetDouble(*this, name.c_str()));
}

template<>
inline Number Object::Get<Number>(const ostring& name) const
{
    return Number(core::Check(OCI_ObjectGetNumber(*this, name.c_str())), GetHandle());
}

template<>
inline ostring Object::Get<ostring>(const ostring& name) const
{
    return core::MakeString(core::Check(OCI_ObjectGetString(*this,name.c_str())));
}

template<>
inline Date Object::Get<Date>(const ostring& name) const
{
    return Date(core::Check(OCI_ObjectGetDate(*this,name.c_str())), GetHandle());
}

template<>
inline Timestamp Object::Get<Timestamp>(const ostring& name) const
{
    return Timestamp(core::Check(OCI_ObjectGetTimestamp(*this,name.c_str())), GetHandle());
}

template<>
inline Interval Object::Get<Interval>(const ostring& name) const
{
    return Interval(core::Check(OCI_ObjectGetInterval(*this,name.c_str())), GetHandle());
}

template<>
inline Object Object::Get<Object>(const ostring& name) const
{
    return Object(core::Check(OCI_ObjectGetObject(*this,name.c_str())), GetHandle());
}

template<>
inline Reference Object::Get<Reference>(const ostring& name) const
{
    return Reference(core::Check(OCI_ObjectGetRef(*this,name.c_str())), GetHandle());
}

template<>
inline Clob Object::Get<Clob>(const ostring& name) const
{
    return Clob(core::Check(OCI_ObjectGetLob(*this,name.c_str())), GetHandle());
}

template<>
inline NClob Object::Get<NClob>(const ostring& name) const
{
    return NClob(core::Check(OCI_ObjectGetLob(*this, name.c_str())), GetHandle());
}

template<>
inline Blob Object::Get<Blob>(const ostring& name) const
{
    return Blob(core::Check(OCI_ObjectGetLob(*this,name.c_str())), GetHandle());
}

template<>
inline File Object::Get<File>(const ostring& name) const
{
    return File(core::Check(OCI_ObjectGetFile(*this,name.c_str())), GetHandle());
}

template<>
inline Raw Object::Get<Raw>(const ostring& name) const
{
    unsigned int size = core::Check(OCI_ObjectGetRawSize(*this, name.c_str()));

    core::ManagedBuffer<unsigned char> buffer(static_cast<size_t>(size + 1));

    size = static_cast<unsigned int>(core::Check(OCI_ObjectGetRaw(*this, name.c_str(), static_cast<AnyPointer>(buffer), size)));

    return core::MakeRaw(buffer, size);
}

template<class T>
T Object::Get(const ostring& name) const
{
    return T(core::Check(OCI_ObjectGetColl(*this, name.c_str())), GetHandle());
}

template<>
inline void Object::Set<bool>(const ostring& name, const bool &value)
{
    core::Check(OCI_ObjectSetBoolean(*this, name.c_str(), static_cast<boolean>(value)));
}

template<>
inline void Object::Set<short>(const ostring& name, const short &value)
{
    core::Check(OCI_ObjectSetShort(*this, name.c_str(), value));
}

template<>
inline void Object::Set<unsigned short>(const ostring& name, const unsigned short &value)
{
    core::Check(OCI_ObjectSetUnsignedShort(*this, name.c_str(), value));
}

template<>
inline void Object::Set<int>(const ostring& name, const int &value)
{
    core::Check(OCI_ObjectSetInt(*this, name.c_str(), value));
}

template<>
inline void Object::Set<unsigned int>(const ostring& name, const unsigned int &value)
{
    core::Check(OCI_ObjectSetUnsignedInt(*this, name.c_str(), value));
}

template<>
inline void Object::Set<big_int>(const ostring& name, const big_int &value)
{
    core::Check(OCI_ObjectSetBigInt(*this, name.c_str(), value));
}

template<>
inline void Object::Set<big_uint>(const ostring& name, const big_uint &value)
{
    core::Check(OCI_ObjectSetUnsignedBigInt(*this, name.c_str(), value));
}

template<>
inline void Object::Set<float>(const ostring& name, const float &value)
{
    core::Check(OCI_ObjectSetFloat(*this, name.c_str(), value));
}

template<>
inline void Object::Set<double>(const ostring& name, const double &value)
{
    core::Check(OCI_ObjectSetDouble(*this, name.c_str(), value));
}

template<>
inline void Object::Set<Number>(const ostring& name, const Number &value)
{
    core::Check(OCI_ObjectSetNumber(*this, name.c_str(), value));
}

template<>
inline void Object::Set<ostring>(const ostring& name, const ostring &value)
{
    core::Check(OCI_ObjectSetString(*this, name.c_str(), value.c_str()));
}

template<>
inline void Object::Set<Date>(const ostring& name, const Date &value)
{
    core::Check(OCI_ObjectSetDate(*this, name.c_str(), value));
}

template<>
inline void Object::Set<Timestamp>(const ostring& name, const Timestamp &value)
{
    core::Check(OCI_ObjectSetTimestamp(*this, name.c_str(), value));
}

template<>
inline void Object::Set<Interval>(const ostring& name, const Interval &value)
{
    core::Check(OCI_ObjectSetInterval(*this, name.c_str(), value));
}

template<>
inline void Object::Set<Object>(const ostring& name, const Object &value)
{
    core::Check(OCI_ObjectSetObject(*this, name.c_str(), value));
}

template<>
inline void Object::Set<Reference>(const ostring& name, const Reference &value)
{
    core::Check(OCI_ObjectSetRef(*this, name.c_str(), value));
}

template<>
inline void Object::Set<Clob>(const ostring& name, const Clob &value)
{
    core::Check(OCI_ObjectSetLob(*this, name.c_str(), value));
}

template<>
inline void Object::Set<NClob>(const ostring& name, const NClob &value)
{
    core::Check(OCI_ObjectSetLob(*this, name.c_str(), value));
}

template<>
inline void Object::Set<Blob>(const ostring& name, const Blob &value)
{
    core::Check(OCI_ObjectSetLob(*this, name.c_str(), value));
}

template<>
inline void Object::Set<File>(const ostring& name, const File &value)
{
    core::Check(OCI_ObjectSetFile(*this, name.c_str(), value));
}

template<>
inline void Object::Set<Raw>(const ostring& name, const Raw &value)
{
    const AnyPointer data = value.empty() ? nullptr : static_cast<AnyPointer>(const_cast<Raw::value_type*>(&value[0])) ;

    core::Check(OCI_ObjectSetRaw(*this, name.c_str(), data, static_cast<unsigned int>(value.size())));

}

template<class T>
void Object::Set(const ostring& name, const T &value)
{
    core::Check(OCI_ObjectSetColl(*this, name.c_str(), value));
}

inline ostring Object::ToString() const
{
    if (!IsNull())
    {
        unsigned int len = 0;

        core::Check(OCI_ObjectToText(*this, &len, nullptr));

        core::ManagedBuffer<otext> buffer(static_cast<size_t>(len + 1));

        core::Check(OCI_ObjectToText(*this, &len, buffer));

        return core::MakeString(static_cast<const otext *>(buffer), static_cast<int>(len));
    }

    return OCI_STRING_NULL;
}

}
