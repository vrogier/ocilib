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

    inline SodaColl::SodaColl()
    {
    }

    inline SodaColl::SodaColl(OCI_SodaColl* coll, core::Handle* parent)
    {
        AcquireAllocated(coll, parent);
    }

    inline Connection SodaColl::GetConnection() const
    {
        return Connection(core::Check(OCI_SodaCollGetConnection(*this)), GetHandle(), false);
    }

    inline String SodaColl::GetName() const
    {
        return core::MakeString(core::Check(OCI_SodaCollGetName(*this)));
    }

    inline String SodaColl::GetMetadata() const
    {
        return core::MakeString(core::Check(OCI_SodaCollGetMetadata(*this)));
    }

    inline bool SodaColl::IsMapped() const
    {
        return (core::Check(OCI_SodaCollIsMapped(*this)) == TRUE);
    }

    inline bool SodaColl::Drop()
    {
        return (core::Check(OCI_SodaCollDrop(*this)) == TRUE);
    }

    inline bool SodaColl::Truncate()
    {
        return (core::Check(OCI_SodaCollTruncate(*this)) == TRUE);
    }

    inline void SodaColl::Insert(SodaDoc& doc)
    {
        core::Check(OCI_SodaCollInsertDoc(*this, doc));
    }

    inline SodaDocCursor SodaColl::Find()
    {
        OCI_SodaDocCursor* cursor = core::Check
                                    (
                                        OCI_SodaCollFind(*this, nullptr)
                                    );

        return SodaDocCursor(cursor, GetHandle());
    }

    inline SodaDocCursor SodaColl::Find(const SodaOptions& options)
    {
        OCI_SodaDocCursor* cursor = core::Check
                                    (
                                        OCI_SodaCollFind(*this, options)
                                    );

        return SodaDocCursor(cursor, GetHandle());
    }

    inline SodaDoc SodaColl::FindOne(const SodaOptions& options)
    {
        OCI_SodaDoc* doc = core::Check
                           (
                               OCI_SodaCollFindOne(*this, options)
                           );

        return SodaDoc(doc, GetHandle());
    }

    inline SodaDoc SodaColl::FindOne(const String& key)
    {
        OCI_SodaDoc* doc = core::Check
                           (
                               OCI_SodaCollFindOneByKey(*this, key.c_str())
                           );

        return SodaDoc(doc, GetHandle());
    }

    inline UInt64 SodaColl::GetCount(const SodaOptions& options)
    {
        return OCI_SodaCollDocCount(*this, options);
    }

    inline UInt64 SodaColl::GetCount(const String& filter)
    {
        return OCI_SodaCollDocCountWithFilter(*this, filter.c_str());
    }

    inline void SodaColl::ReplaceOne(const SodaOptions& options, SodaDoc& doc, bool refresh)
    {
        core::Check(OCI_SodaCollReplaceOne(*this, options, doc, refresh ? TRUE : FALSE));
    }

    inline void SodaColl::ReplaceOneByKey(const String& key, SodaDoc& doc, bool refresh)
    {
        core::Check(OCI_SodaCollReplaceOneByKey(*this, key.c_str(), doc, refresh ? TRUE : FALSE));
    }

    inline bool SodaColl::Remove(const SodaOptions& options, UInt64* count)
    {
        return (core::Check(OCI_SodaCollRemove(*this, options, count)) == TRUE);
    }

    inline bool SodaColl::RemoveOne(const String& key)
    {
        return (core::Check(OCI_SodaCollRemoveOneByKey(*this, key.c_str())) == TRUE);
    }

    inline void SodaColl::InsertMany(std::vector<SodaDoc>& docs, bool refresh)
    {
        std::vector<OCI_SodaDoc*> handles(docs.size());

        for (size_t i = 0; i < docs.size(); i++)
        {
            handles[i] = docs[i];
        }

        core::Check(OCI_SodaCollInsertMany(*this, handles.data(), static_cast<OCI_UInt32>(docs.size()), refresh ? TRUE : FALSE));
    }

    inline void SodaColl::CreateIndex(const String& indexSpec)
    {
        core::Check(OCI_SodaCollCreateIndex(*this, indexSpec.c_str()));
    }

    inline bool SodaColl::DropIndex(const String& name)
    {
        return (core::Check(OCI_SodaCollDropIndex(*this, name.c_str())) == TRUE);
    }

    inline SodaDoc SodaColl::GetDataGuide()
    {
        return SodaDoc(core::Check(OCI_SodaCollGetDataGuide(*this)), GetHandle());
    }

    inline std::vector<String> SodaColl::ListIndexes()
    {
        UInt32  count = 0;
        otext** names = core::Check(OCI_SodaCollListIndexes(*this, &count));

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
            OCI_SodaCollFreeIndexNames(*this, names, count);
        }

        return result;
    }

    inline void SodaColl::Save(SodaDoc& doc, bool refresh)
    {
        core::Check(OCI_SodaCollSave(*this, doc, refresh ? TRUE : FALSE));
    }

}
