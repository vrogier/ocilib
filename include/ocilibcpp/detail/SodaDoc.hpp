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
// ReSharper disable CppClangTidyReadabilityInconsistentDeclarationParameterName

namespace ocilib
{

    inline SodaDoc::SodaDoc()
    {
    }

    inline SodaDoc::SodaDoc(OCI_SodaDoc* doc, core::Handle* parent)
    {
        AcquireAllocated(doc, parent);
    }

    inline ostring SodaDoc::GetKey() const
    {
        return core::MakeString(core::Check(OCI_SodaDocGetKey(*this)));
    }

    inline ostring SodaDoc::GetVersion() const
    {
        return core::MakeString(core::Check(OCI_SodaDocGetVersion(*this)));
    }

    inline ostring SodaDoc::GetCreatedOn() const
    {
        return core::MakeString(core::Check(OCI_SodaDocGetCreatedOn(*this)));
    }

    inline ostring SodaDoc::GetLastModified() const
    {
        return core::MakeString(core::Check(OCI_SodaDocGetLastModified(*this)));
    }

    inline ostring SodaDoc::GetMediaType() const
    {
        return core::MakeString(core::Check(OCI_SodaDocGetMediaType(*this)));
    }

    inline bool SodaDoc::IsJson() const
    {
        return TRUE == core::Check(OCI_SodaDocGetIsJson(*this));
    }

    inline ostring SodaDoc::GetText() const
    {
        return core::MakeString(core::Check(OCI_SodaDocGetText(*this)));
    }

    inline Json SodaDoc::GetJson() const
    {
        OCI_Json* value = OCI_SodaDocGetJson(*this);

        if (value)
        {
            return Json(value, GetHandle());
        }

        return Json();
    }

}
