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

    inline SodaDocument::SodaDocument()
    {
    }

    inline SodaDocument::SodaDocument(OCI_SodaDocument* doc, core::Handle* parent)
    {
        AcquireAllocated(doc, parent);
    }

    inline String SodaDocument::GetKey() const
    {
        return core::MakeString(core::Check(OCI_SodaDocumentGetKey(*this)));
    }

    inline String SodaDocument::GetVersion() const
    {
        return core::MakeString(core::Check(OCI_SodaDocumentGetVersion(*this)));
    }

    inline String SodaDocument::GetCreatedOn() const
    {
        return core::MakeString(core::Check(OCI_SodaDocumentGetCreatedOn(*this)));
    }

    inline String SodaDocument::GetLastModified() const
    {
        return core::MakeString(core::Check(OCI_SodaDocumentGetLastModified(*this)));
    }

    inline String SodaDocument::GetMediaType() const
    {
        return core::MakeString(core::Check(OCI_SodaDocumentGetMediaType(*this)));
    }

    inline bool SodaDocument::IsJson() const
    {
        return TRUE == core::Check(OCI_SodaDocumentGetIsJson(*this));
    }

    inline String SodaDocument::GetText() const
    {
        return core::MakeString(core::Check(OCI_SodaDocumentGetText(*this)));
    }

    inline Json SodaDocument::GetJson() const
    {
        OCI_Json* value = OCI_SodaDocumentGetJson(*this);

        if (value)
        {
            return Json(value, GetHandle());
        }

        return Json();
    }

}
