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

    inline SodaCollCursor::SodaCollCursor()
    {
    }

    inline SodaCollCursor::SodaCollCursor(OCI_SodaCollCursor* cursor, core::Handle* parent)
    {
        AcquireAllocated(cursor, parent);
    }

    inline SodaColl SodaCollCursor::GetNext()
    {
        OCI_SodaColl* coll = core::Check
        (
            OCI_SodaCollCursorGetNext(*this)
        );

        return SodaColl(coll, GetHandle());
    }


    inline SodaCollCursor::iterator& SodaCollCursor::iterator::operator++()
    {
        _coll = _cursor->GetNext();
        if (!_coll)
        {
            _cursor = NULL;
        }
        return *this;
    }

    inline SodaCollCursor::iterator SodaCollCursor::begin()
    {
        return iterator(this);
    }
}
