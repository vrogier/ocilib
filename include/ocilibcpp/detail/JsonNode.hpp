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

namespace ocilib
{
    inline JsonNode::JsonNode()
    {
    }

    inline JsonNode::JsonNode(OCI_JsonNode* pNode, core::Handle* parent)
    {
        AcquireAllocated(pNode, parent);
    }

    inline JsonNode::JsonType JsonNode::GetType() const
    {
        return JsonType(static_cast<JsonType::Type>(core::Check(OCI_JsonNodeGetType(*this))));
    }

    inline bool JsonNode::IsNull() const
    {
        return core::Check(OCI_JsonNodeIsNull(*this));
    }

    inline UInt32 JsonNode::GetFieldCount() const
    {
        return core::Check(OCI_JsonNodeGetFieldCount(*this));
    }

    inline String JsonNode::GetFieldName(UInt32 index) const
    {
        return core::MakeString(core::Check(OCI_JsonNodeGetFieldName(*this, index)));
    }

    inline JsonNode JsonNode::GetFieldByName(const String& name) const
    {
        return JsonNode(core::Check(OCI_JsonNodeGetFieldByName(*this, name.c_str())), GetHandle());
    }

    inline JsonNode JsonNode::GetFieldByIndex(UInt32 index) const
    {
        return JsonNode(core::Check(OCI_JsonNodeGetFieldByIndex(*this, index)), GetHandle());
    }

    inline JsonNode JsonNode::GetField(const String& name) const
    {
        return GetFieldByName(name);
    }

    inline JsonNode JsonNode::GetField(UInt32 index) const
    {
        return GetFieldByIndex(index);
    }

    inline UInt32 JsonNode::GetItemCount() const
    {
        return core::Check(OCI_JsonNodeGetItemCount(*this));
    }

    inline JsonNode JsonNode::GetItem(UInt32 index) const
    {
        return JsonNode(core::Check(OCI_JsonNodeGetItem(*this, index)), GetHandle());
    }

    inline JsonNode JsonNode::operator[](const String& name) const
    {
        return GetField(name);
    }

    inline JsonNode JsonNode::operator[](UInt32 index) const
    {
        return GetItem(index);
    }

    inline JsonNode JsonNode::operator[](Int32 index) const
    {
        return GetItem((UInt32)index);
    }

    inline String JsonNode::GetString() const
    {
        UInt32 length = 0;
        const otext* value = core::Check(OCI_JsonNodeGetString(*this, &length));
        return core::MakeString(value, static_cast<Int32>(length));
    }

    inline bool JsonNode::GetBoolean() const
    {
        return core::Check(OCI_JsonNodeGetBoolean(*this));
    }

    inline Raw JsonNode::GetBinary() const
    {
        UInt32 length = 0;
        const void* value = core::Check(OCI_JsonNodeGetBinary(*this, &length));
        return core::MakeRaw(const_cast<void*>(value), length);
    }

    inline Date JsonNode::GetDate() const
    {
        Date date;
        date.AcquireAllocated(core::Check(OCI_JsonNodeGetDate(*this)), GetHandle());
        return date;
    }

    inline Timestamp JsonNode::GetTimestamp() const
    {
        Timestamp ts;
        ts.AcquireAllocated(core::Check(OCI_JsonNodeGetTimestamp(*this)), GetHandle());
        return ts;
    }

    inline Interval JsonNode::GetInterval() const
    {
        Interval iv;
        iv.AcquireAllocated(core::Check(OCI_JsonNodeGetInterval(*this)), GetHandle());
        return iv;
    }

    inline void JsonNode::SetString(const String& value)
    {
        core::Check(OCI_JsonNodeSetString(*this, value.c_str(), static_cast<UInt32>(value.size())));
    }

    inline void JsonNode::SetBoolean(bool value)
    {
        core::Check(OCI_JsonNodeSetBoolean(*this, value));
    }

    inline void JsonNode::SetBinary(const void* data, UInt32 length)
    {
        core::Check(OCI_JsonNodeSetBinary(*this, data, length));
    }

    inline void JsonNode::SetBinary(const Raw& data)
    {
        SetBinary(data.data(), static_cast<UInt32>(data.size()));
    }

    inline void JsonNode::SetNull()
    {
        core::Check(OCI_JsonNodeSetNull(*this));
    }

    inline void JsonNode::SetDate(const Date& value)
    {
        core::Check(OCI_JsonNodeSetDate(*this, value));
    }

    inline void JsonNode::SetTimestamp(const Timestamp& value)
    {
        core::Check(OCI_JsonNodeSetTimestamp(*this, value));
    }

    inline void JsonNode::SetInterval(const Interval& value)
    {
        core::Check(OCI_JsonNodeSetInterval(*this, value));
    }

    inline void JsonNode::SetField(const String& name, const JsonNode& value)
    {
        core::Check(OCI_JsonNodeSetField(*this, name.c_str(), value));
    }

    inline void JsonNode::SetItem(UInt32 index, const JsonNode& value)
    {
        core::Check(OCI_JsonNodeSetItem(*this, index, value));
    }

    inline void JsonNode::AppendItem(const JsonNode& value)
    {
        core::Check(OCI_JsonNodeAppendItem(*this, value));
    }

    inline void JsonNode::InsertItem(UInt32 index, const JsonNode& value)
    {
        core::Check(OCI_JsonNodeInsertItem(*this, index, value));
    }

    inline void JsonNode::RemoveItem(UInt32 index)
    {
        core::Check(OCI_JsonNodeRemoveItem(*this, index));
    }

    inline bool JsonNode::HasField(const String& name) const
    {
        return TRUE == core::Check(OCI_JsonNodeHasField(*this, name.c_str()));
    }

    inline void JsonNode::RemoveField(const String& name)
    {
        core::Check(OCI_JsonNodeRemoveField(*this, name.c_str()));
    }

    inline void JsonNode::Clear()
    {
        core::Check(OCI_JsonNodeClear(*this));
    }

    inline JsonNode JsonNode::GetParent() const
    {
        return JsonNode(core::Check(OCI_JsonNodeGetParent(*this)), GetHandle());
    }

    inline JsonNode JsonNode::GetByPath(const String& path) const
    {
        return JsonNode(core::Check(OCI_JsonNodeGetByPath(*this, path.c_str())), GetHandle());
    }

    template<class T, core::SupportedNumericT<T>*>
    T JsonNode::GetNumeric() const
    {
        using U = std::decay_t<T>;

        if constexpr (std::is_same_v<U, Number>)
        {
            Number number;
            number.AcquireAllocated(core::Check(OCI_JsonNodeGetNumber(*this)), GetHandle());
            return number;
        }
        else
        {
            T value{};
            OCI_Boolean res = FALSE;

            if constexpr      (std::is_same_v<U, Int16>)   res = OCI_JsonNodeGetShort(*this, reinterpret_cast<OCI_Int16*>(&value));
            else if constexpr (std::is_same_v<U, UInt16>)  res = OCI_JsonNodeGetUShort(*this, reinterpret_cast<OCI_UInt16*>(&value));
            else if constexpr (std::is_same_v<U, Int32>)   res = OCI_JsonNodeGetInt(*this, reinterpret_cast<OCI_Int32*>(&value));
            else if constexpr (std::is_same_v<U, UInt32>)  res = OCI_JsonNodeGetUInt(*this, reinterpret_cast<OCI_UInt32*>(&value));
            else if constexpr (std::is_same_v<U, Int64>)   res = OCI_JsonNodeGetBigInt(*this, reinterpret_cast<OCI_Int64*>(&value));
            else if constexpr (std::is_same_v<U, UInt64>)  res = OCI_JsonNodeGetBigUInt(*this, reinterpret_cast<OCI_UInt64*>(&value));
            else if constexpr (std::is_same_v<U, Float>)   res = OCI_JsonNodeGetFloat(*this, reinterpret_cast<OCI_Float*>(&value));
            else if constexpr (std::is_same_v<U, Double>)  res = OCI_JsonNodeGetDouble(*this, reinterpret_cast<OCI_Double*>(&value));

            core::Check(res);

            return value;
        }
    }

    template<class T, core::SupportedNumericT<T>*>
    void JsonNode::SetNumeric(const T& value)
    {
        using U = std::decay_t<T>;

        if constexpr      (std::is_same_v<U, Number>)   core::Check(OCI_JsonNodeSetNumber(*this, value));
        else if constexpr (std::is_same_v<U, Int16>)    core::Check(OCI_JsonNodeSetShort(*this, static_cast<OCI_Int16>(value)));
        else if constexpr (std::is_same_v<U, UInt16>)   core::Check(OCI_JsonNodeSetUShort(*this, static_cast<OCI_UInt16>(value)));
        else if constexpr (std::is_same_v<U, Int32>)    core::Check(OCI_JsonNodeSetInt(*this, static_cast<OCI_Int32>(value)));
        else if constexpr (std::is_same_v<U, UInt32>)   core::Check(OCI_JsonNodeSetUInt(*this, static_cast<OCI_UInt32>(value)));
        else if constexpr (std::is_same_v<U, Int64>)    core::Check(OCI_JsonNodeSetBigInt(*this, static_cast<OCI_Int64>(value)));
        else if constexpr (std::is_same_v<U, UInt64>)   core::Check(OCI_JsonNodeSetBigUInt(*this, static_cast<OCI_UInt64>(value)));
        else if constexpr (std::is_same_v<U, Float>)    core::Check(OCI_JsonNodeSetFloat(*this, static_cast<OCI_Float>(value)));
        else if constexpr (std::is_same_v<U, Double>)   core::Check(OCI_JsonNodeSetDouble(*this, static_cast<OCI_Double>(value)));
    }

    inline JsonNodeIterator JsonNode::begin() const
    {
        const Json::JsonType type = GetType();
        return (type == Json::NodeArray || type == Json::NodeObject) ? JsonNodeIterator(*this, 1) : end();
    }

    inline JsonNodeIterator JsonNode::end() const
    {
        const Json::JsonType type = GetType();
        return JsonNodeIterator(*this, (type == Json::NodeArray ? GetItemCount() : type == Json::NodeObject ? GetFieldCount() : 0) + 1);
    }
}
