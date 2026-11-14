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

    inline SodaCollection::SodaCollection()
    {
    }

    inline SodaCollection::SodaCollection(OCI_SodaCollection* coll, core::Handle* parent)
    {
        AcquireAllocated(coll, parent);
    }

    inline Connection SodaCollection::GetConnection() const
    {
        return Connection(core::Check(OCI_SodaCollectionGetConnection(*this)), GetHandle(), false);
    }

    inline String SodaCollection::GetName() const
    {
        return core::MakeString(core::Check(OCI_SodaCollectionGetName(*this)));
    }

    inline String SodaCollection::GetMetadata() const
    {
        return core::MakeString(core::Check(OCI_SodaCollectionGetMetadata(*this)));
    }

    inline bool SodaCollection::IsMapped() const
    {
        return (core::Check(OCI_SodaCollectionIsMapped(*this)) == TRUE);
    }

    inline bool SodaCollection::Drop()
    {
        return (core::Check(OCI_SodaCollectionDrop(*this)) == TRUE);
    }

    inline bool SodaCollection::Truncate()
    {
        return (core::Check(OCI_SodaCollectionTruncate(*this)) == TRUE);
    }

    inline void SodaCollection::Insert(SodaDocument& doc)
    {
        core::Check(OCI_SodaCollectionInsertDocument(*this, doc));
    }

    inline SodaDocumentCursor SodaCollection::Find()
    {
        OCI_SodaDocumentCursor* cursor = core::Check
                                         (
                                             OCI_SodaCollectionFind(*this, nullptr)
                                         );

        return SodaDocumentCursor(cursor, GetHandle());
    }

    inline SodaDocumentCursor SodaCollection::Find(const SodaOptions& options)
    {
        OCI_SodaDocumentCursor* cursor = core::Check
                                         (
                                             OCI_SodaCollectionFind(*this, options)
                                         );

        return SodaDocumentCursor(cursor, GetHandle());
    }

    inline SodaDocument SodaCollection::FindOne(const SodaOptions& options)
    {
        OCI_SodaDocument* doc = core::Check
                                (
                                    OCI_SodaCollectionFindOne(*this, options)
                                );

        return SodaDocument(doc, GetHandle());
    }

    inline SodaDocument SodaCollection::FindOne(const String& key)
    {
        OCI_SodaDocument* doc = core::Check
                                (
                                    OCI_SodaCollectionFindOneByKey(*this, key.c_str())
                                );

        return SodaDocument(doc, GetHandle());
    }

    inline UInt64 SodaCollection::GetCount(const SodaOptions& options)
    {
        return OCI_SodaCollectionDocumentCount(*this, options);
    }

    inline UInt64 SodaCollection::GetCount(const String& filter)
    {
        return OCI_SodaCollectionDocumentCountWithFilter(*this, filter.c_str());
    }

    inline void SodaCollection::ReplaceOne(const SodaOptions& options, SodaDocument& doc, bool refresh)
    {
        core::Check(OCI_SodaCollectionReplaceOne(*this, options, doc, refresh ? TRUE : FALSE));
    }

    inline void SodaCollection::ReplaceOneByKey(const String& key, SodaDocument& doc, bool refresh)
    {
        core::Check(OCI_SodaCollectionReplaceOneByKey(*this, key.c_str(), doc, refresh ? TRUE : FALSE));
    }

    inline bool SodaCollection::Remove(const SodaOptions& options, UInt64* count)
    {
        return (core::Check(OCI_SodaCollectionRemove(*this, options, count)) == TRUE);
    }

    inline bool SodaCollection::RemoveOne(const String& key)
    {
        return (core::Check(OCI_SodaCollectionRemoveOneByKey(*this, key.c_str())) == TRUE);
    }

    inline void SodaCollection::InsertMany(std::vector<SodaDocument>& docs, bool refresh)
    {
        std::vector<OCI_SodaDocument*> handles(docs.size());

        for (size_t i = 0; i < docs.size(); i++)
        {
            handles[i] = docs[i];
        }

        core::Check(OCI_SodaCollectionInsertMany(*this, handles.data(), static_cast<OCI_UInt32>(docs.size()), refresh ? TRUE : FALSE));
    }

    inline void SodaCollection::CreateIndex(const String& indexSpec)
    {
        core::Check(OCI_SodaCollectionCreateIndex(*this, indexSpec.c_str()));
    }

    inline bool SodaCollection::DropIndex(const String& name)
    {
        return (core::Check(OCI_SodaCollectionDropIndex(*this, name.c_str())) == TRUE);
    }

    inline SodaDocument SodaCollection::GetDataGuide()
    {
        return SodaDocument(core::Check(OCI_SodaCollectionGetDataGuide(*this)), GetHandle());
    }

    inline std::vector<String> SodaCollection::ListIndexes()
    {
        UInt32  count = 0;
        otext** names = core::Check(OCI_SodaCollectionListIndexes(*this, &count));

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
            OCI_SodaCollectionFreeIndexNames(*this, names, count);
        }

        return result;
    }

    inline void SodaCollection::Save(SodaDocument& doc, bool refresh)
    {
        core::Check(OCI_SodaCollectionSave(*this, doc, refresh ? TRUE : FALSE));
    }

}
