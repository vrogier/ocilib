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

namespace ocilib
{

    inline SodaDocumentCursor::SodaDocumentCursor()
    {
    }

    inline SodaDocumentCursor::SodaDocumentCursor(OCI_SodaDocumentCursor* cursor, core::Handle* parent)
    {
        AcquireAllocated(cursor, parent);
    }

    inline SodaDocument SodaDocumentCursor::GetNext()
    {
        OCI_SodaDocument* doc = core::Check
                                (
                                    OCI_SodaDocumentCursorGetNext(*this)
                                );

        return SodaDocument(doc, GetHandle());
    }

    inline SodaDocumentCursor::iterator& SodaDocumentCursor::iterator::operator++()
    {
        _doc = _cursor->GetNext();
        if (!_doc)
        {
            _cursor = NULL;
        }
        return *this;
    }

    inline SodaDocumentCursor::iterator SodaDocumentCursor::begin()
    {
        return iterator(this);
    }
}
