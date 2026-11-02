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
    inline Json::Json()
    {
    }

    inline Json::Json(const Connection& connection)
    {
        AcquireAllocated(core::Check(OCI_JsonCreate(connection)), connection.GetHandle());
    }

    inline Json::Json(OCI_Json* pJson, core::Handle* parent)
    {
        AcquireTransient(pJson, parent);
    }

    inline Json& Json::operator=(OCI_Json*& lhs)
    {
        AcquireAllocated(lhs, static_cast<core::Handle*>(nullptr));
        return *this;
    }

    inline void Json::LoadText(const ostring& text, JsonValidation validation, JsonEncoding encoding)
    {
        LoadText(text.c_str(), static_cast<UInt32>(text.size()), validation, encoding);
    }

    inline void Json::LoadText(const otext* text, UInt32 length, JsonValidation validation, JsonEncoding encoding)
    {
        core::Check(OCI_JsonLoadText(*this, text, length,
            static_cast<UInt32>(validation.GetValues()),
            static_cast<UInt32>(encoding)));
    }

    inline void Json::LoadBinary(const void* data, UInt32 length)
    {
        core::Check(OCI_JsonLoadBinary(*this, data, length));
    }

    inline void Json::LoadBinary(const Raw& data)
    {
        LoadBinary(data.data(), static_cast<UInt32>(data.size()));
    }

    inline ostring Json::ToText(JsonPrint print) const
    {
        UInt32 size = 0;

        core::Check(OCI_JsonToText(*this, NULL, &size,
            static_cast<UInt32>(print.GetValues()), TRUE));

        UInt32 maxSize = size + 1;
        ostring result;
        result.resize(static_cast<Size>(maxSize));

        core::Check(OCI_JsonToText(*this, &result[0], &maxSize,
            static_cast<UInt32>(print.GetValues()), TRUE));

        result.resize(static_cast<Size>(maxSize));

        return result;
    }

    inline Raw Json::ToBinary() const
    {
        UInt32 size = 0;

        core::Check(OCI_JsonToBinary(*this, NULL, &size));

        Raw result;
        result.resize(static_cast<Size>(size));

        if (size > 0)
        {
            core::Check(OCI_JsonToBinary(*this, result.data(), &size));
        }

        result.resize(static_cast<Size>(size));

        return result;
    }

    inline bool Json::Clone(Json& dest) const
    {
        return core::Check(OCI_JsonClone(*this, dest));
    }

    inline void Json::SetMutable(bool value)
    {
        core::Check(OCI_JsonSetMutable(*this, value));
    }

    inline bool Json::IsMutable() const
    {
        return core::Check(OCI_JsonIsMutable(*this));
    }

    inline JsonNode Json::GetRoot()
    {
        return JsonNode(core::Check(OCI_JsonGetRoot(*this)), GetHandle());
    }

    inline void Json::SetRoot(const JsonNode& node)
    {
        core::Check(OCI_JsonSetRoot(*this, node));
    }

    inline JsonNode Json::CreateNode(JsonType type)
    {
        return JsonNode(core::Check(OCI_JsonNodeCreate(*this, type)), GetHandle());
    }

    inline JsonNode Json::CreateObject()
    {
        return JsonNode(core::Check(OCI_JsonNodeCreateObject(*this)), GetHandle());
    }

    inline JsonNode Json::CreateArray()
    {
        return JsonNode(core::Check(OCI_JsonNodeCreateArray(*this)), GetHandle());
    }

    inline JsonNode Json::CreateNull()
    {
        return JsonNode(core::Check(OCI_JsonNodeCreateNull(*this)), GetHandle());
    }

    inline JsonNode Json::CreateString(const ostring& value)
    {
        return JsonNode(core::Check(OCI_JsonNodeCreateString(*this, value.c_str(), static_cast<UInt32>(value.size()))), GetHandle());
    }

    inline JsonNode Json::CreateNumber(const Number& value)
    {
        return JsonNode(core::Check(OCI_JsonNodeCreateNumber(*this, value)), GetHandle());
    }

    inline JsonNode Json::CreateBoolean(bool value)
    {
        return JsonNode(core::Check(OCI_JsonNodeCreateBoolean(*this, value)), GetHandle());
    }

    inline JsonNode Json::CreateBinary(const void* data, UInt32 length)
    {
        return JsonNode(core::Check(OCI_JsonNodeCreateBinary(*this, data, length)), GetHandle());
    }

    inline JsonNode Json::CreateBinary(const Raw& data)
    {
        return CreateBinary(data.data(), static_cast<UInt32>(data.size()));
    }

    inline JsonNode Json::CreateDate(const Date& value)
    {
        return JsonNode(core::Check(OCI_JsonNodeCreateDate(*this, value)), GetHandle());
    }

    inline JsonNode Json::CreateTimestamp(const Timestamp& value)
    {
        return JsonNode(core::Check(OCI_JsonNodeCreateTimestamp(*this, value)), GetHandle());
    }

    inline JsonNode Json::CreateInterval(const Interval& value)
    {
        return JsonNode(core::Check(OCI_JsonNodeCreateInterval(*this, value)), GetHandle());
    }

    inline ostring Json::ToString() const
    {
        return ToText();
    }
}
