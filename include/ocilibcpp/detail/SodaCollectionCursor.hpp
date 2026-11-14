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

    inline SodaCollectionCursor::SodaCollectionCursor()
    {
    }

    inline SodaCollectionCursor::SodaCollectionCursor(OCI_SodaCollectionCursor* cursor, core::Handle* parent)
    {
        AcquireAllocated(cursor, parent);
    }

    inline SodaCollection SodaCollectionCursor::GetNext()
    {
        OCI_SodaCollection* coll = core::Check
                                   (
                                       OCI_SodaCollectionCursorGetNext(*this)
                                   );

        return SodaCollection(coll, GetHandle());
    }

    inline SodaCollectionCursor::iterator& SodaCollectionCursor::iterator::operator++()
    {
        _coll = _cursor->GetNext();
        if (!_coll)
        {
            _cursor = NULL;
        }
        return *this;
    }

    inline SodaCollectionCursor::iterator SodaCollectionCursor::begin()
    {
        return iterator(this);
    }
}
