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

    inline JsonNodeIterator::JsonNodeIterator(const JsonNode& node, UInt32 index)
        : _node(&node), _index(index)
    {
    }

    inline JsonNode JsonNodeIterator::operator*() const
    {
        return (_node->GetType() == Json::NodeObject) ? _node->GetFieldByIndex(_index) : _node->GetItem(_index);
    }

    inline JsonNodeIterator& JsonNodeIterator::operator++()
    {
        ++_index;
        return *this;
    }

    inline JsonNodeIterator JsonNodeIterator::operator++(Int32)
    {
        JsonNodeIterator other = *this;
        ++_index;
        return other;
    }

    inline bool JsonNodeIterator::operator == (const JsonNodeIterator& other) const
    {
        return _index == other._index;
    }

    inline bool JsonNodeIterator::operator != (const JsonNodeIterator& other) const
    {
        return !(*this == other);
    }
}
