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

    inline SodaDb::SodaDb()
    {
    }

    inline SodaDb::SodaDb(const Connection& connection)
    {
        AcquireAllocated(core::Check(OCI_SodaDbCreate(connection)), connection.GetHandle());
    }

    inline Connection SodaDb::GetConnection() const
    {
        return Connection(core::Check(OCI_SodaDbGetConnection(*this)), GetHandle(), false);
    }

    inline SodaColl SodaDb::CreateCollection(const String& name, const String& metadata)
    {
        OCI_SodaColl* coll = core::Check
        (
            OCI_SodaDbCreateColl(*this, name.c_str(), metadata.empty() ? nullptr : metadata.c_str())
        );

        return SodaColl(coll, GetHandle());
    }

    inline SodaColl SodaDb::OpenCollection(const String& name)
    {
        OCI_SodaColl* coll = core::Check
        (
            OCI_SodaDbOpenColl(*this, name.c_str())
        );

        return SodaColl(coll, GetHandle());
    }

    inline SodaCollCursor SodaDb::GetCollections(const String& startName)
    {
        OCI_SodaCollCursor* cursor = core::Check
        (
            OCI_SodaDbGetColls(*this, startName.empty() ? nullptr : startName.c_str())
        );

        return SodaCollCursor(cursor, GetHandle());
    }

    inline SodaDoc SodaDb::CreateDocumentFromText(const String& content, const String& key)
    {
        OCI_SodaDoc* doc = core::Check
        (
            OCI_SodaDocCreateFromText
            (
                *this,
                content.c_str(),
                static_cast<UInt32>(content.size() * sizeof(otext)),
                key.empty() ? nullptr : key.c_str()
            )
        );

        return SodaDoc(doc, GetHandle());
    }

    inline SodaDoc SodaDb::CreateDocumentFromJson(const Json& json, const String& key)
    {
        OCI_SodaDoc* doc = core::Check
        (
            OCI_SodaDocCreateFromJson
            (
                *this,
                json,
                key.empty() ? nullptr : key.c_str()
            )
        );

        return SodaDoc(doc, GetHandle());
    }


    inline std::vector<String> SodaDb::GetCollectionNames()
    {
        UInt32 count = 0;
        otext** names = core::Check(OCI_SodaDbGetCollectionNames(*this, &count));

        std::vector<String> result;

        if (names)
        {
            for (UInt32 i = 0; i < count; i++)
            {
                if (names[i])
                {
                    result.push_back(core::MakeString(names[i]));
                }
            }
            OCI_SodaDbFreeCollectionNames(*this, names, count);
        }

        return result;
    }
}
