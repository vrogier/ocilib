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

#include "ocilibcpp/support.hpp"

namespace ocilib
{
    namespace support
    {
        inline BindObject::BindObject(const Statement& statement, const String& name, UInt32 mode) : _statement(statement), _name(name), _mode(mode)
        {
        }

        inline BindObject::~BindObject() noexcept
        {
        }

        inline String BindObject::GetName() const
        {
            return _name;
        }

        inline Statement BindObject::GetStatement() const
        {
            return _statement;
        }

        inline UInt32 BindObject::GetMode() const
        {
            return _mode;
        }
    }
}
