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

    inline OCI_UInt32 JsonNode::GetFieldCount() const
    {
        return core::Check(OCI_JsonNodeGetFieldCount(*this));
    }

    inline ostring JsonNode::GetFieldName(OCI_UInt32 index) const
    {
        return core::MakeString(core::Check(OCI_JsonNodeGetFieldName(*this, index)));
    }

    inline JsonNode JsonNode::GetFieldByName(const ostring& name) const
    {
        return JsonNode(core::Check(OCI_JsonNodeGetFieldByName(*this, name.c_str())), GetHandle());
    }

    inline JsonNode JsonNode::GetFieldByIndex(OCI_UInt32 index) const
    {
        return JsonNode(core::Check(OCI_JsonNodeGetFieldByIndex(*this, index)), GetHandle());
    }

    inline JsonNode JsonNode::GetField(const ostring& name) const
    {
        return GetFieldByName(name);
    }

    inline JsonNode JsonNode::GetField(OCI_UInt32 index) const
    {
        return GetFieldByIndex(index);
    }

    inline OCI_UInt32 JsonNode::GetItemCount() const
    {
        return core::Check(OCI_JsonNodeGetItemCount(*this));
    }

    inline JsonNode JsonNode::GetItem(OCI_UInt32 index) const
    {
        return JsonNode(core::Check(OCI_JsonNodeGetItem(*this, index)), GetHandle());
    }

    inline ostring JsonNode::GetString() const
    {
        OCI_UInt32 length = 0;
        const otext* value = core::Check(OCI_JsonNodeGetString(*this, &length));
        return core::MakeString(value, static_cast<OCI_Int32>(length));
    }

    inline Number JsonNode::GetNumber() const
    {
        Number number;
        number.AcquireAllocated(core::Check(OCI_JsonNodeGetNumber(*this)), GetHandle());
        return number;
    }

    inline bool JsonNode::GetBoolean() const
    {
        return core::Check(OCI_JsonNodeGetBoolean(*this));
    }

    inline Raw JsonNode::GetBinary() const
    {
        OCI_UInt32 length = 0;
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

    inline void JsonNode::SetString(const ostring& value)
    {
        core::Check(OCI_JsonNodeSetString(*this, value.c_str(), static_cast<OCI_UInt32>(value.size())));
    }

    inline void JsonNode::SetNumber(const Number& value)
    {
        core::Check(OCI_JsonNodeSetNumber(*this, value));
    }

    inline void JsonNode::SetBoolean(bool value)
    {
        core::Check(OCI_JsonNodeSetBoolean(*this, value));
    }

    inline void JsonNode::SetBinary(const void* data, OCI_UInt32 length)
    {
        core::Check(OCI_JsonNodeSetBinary(*this, data, length));
    }

    inline void JsonNode::SetBinary(const Raw& data)
    {
        SetBinary(data.data(), static_cast<OCI_UInt32>(data.size()));
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

    inline void JsonNode::SetField(const ostring& name, const JsonNode& value)
    {
        core::Check(OCI_JsonNodeSetField(*this, name.c_str(), value));
    }

    inline void JsonNode::SetItem(OCI_UInt32 index, const JsonNode& value)
    {
        core::Check(OCI_JsonNodeSetItem(*this, index, value));
    }

    inline void JsonNode::AppendItem(const JsonNode& value)
    {
        core::Check(OCI_JsonNodeAppendItem(*this, value));
    }
}
