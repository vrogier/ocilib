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

    inline SodaDatabase::SodaDatabase()
    {
    }

    inline SodaDatabase::SodaDatabase(const Connection& connection)
    {
        AcquireAllocated(core::Check(OCI_SodaDatabaseCreate(connection)), connection.GetHandle());
    }

    inline Connection SodaDatabase::GetConnection() const
    {
        return Connection(core::Check(OCI_SodaDatabaseGetConnection(*this)), GetHandle(), false);
    }

    inline SodaCollection SodaDatabase::CreateCollection(const String& name, const String& metadata)
    {
        OCI_SodaCollection* coll = core::Check
                                   (
                                       OCI_SodaDatabaseCreateCollection(*this, name.c_str(), metadata.empty() ? nullptr : metadata.c_str())
                                   );

        return SodaCollection(coll, GetHandle());
    }

    inline SodaCollection SodaDatabase::OpenCollection(const String& name)
    {
        OCI_SodaCollection* coll = core::Check
                                   (
                                       OCI_SodaDatabaseOpenCollection(*this, name.c_str())
                                   );

        return SodaCollection(coll, GetHandle());
    }

    inline SodaCollectionCursor SodaDatabase::GetCollections(const String& startName)
    {
        OCI_SodaCollectionCursor* cursor = core::Check
                                           (
                                               OCI_SodaDatabaseGetCollections(*this, startName.empty() ? nullptr : startName.c_str())
                                           );

        return SodaCollectionCursor(cursor, GetHandle());
    }

    inline SodaDocument SodaDatabase::CreateDocumentFromText(const String& content, const String& key)
    {
        OCI_SodaDocument* doc = core::Check
                                (
                                    OCI_SodaDocumentCreateFromText
                                    (
                                        *this,
                                        content.c_str(),
                                        static_cast<UInt32>(content.size() * sizeof(otext)),
                                        key.empty() ? nullptr : key.c_str()
                                    )
                                );

        return SodaDocument(doc, GetHandle());
    }

    inline SodaDocument SodaDatabase::CreateDocumentFromJson(const Json& json, const String& key)
    {
        OCI_SodaDocument* doc = core::Check
                                (
                                    OCI_SodaDocumentCreateFromJson
                                    (
                                        *this,
                                        json,
                                        key.empty() ? nullptr : key.c_str()
                                    )
                                );

        return SodaDocument(doc, GetHandle());
    }

    inline std::vector<String> SodaDatabase::GetCollectionNames()
    {
        UInt32  count = 0;
        otext** names = core::Check(OCI_SodaDatabaseGetCollectionNames(*this, &count));

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
            OCI_SodaDatabaseFreeCollectionNames(*this, names, count);
        }

        return result;
    }
}
