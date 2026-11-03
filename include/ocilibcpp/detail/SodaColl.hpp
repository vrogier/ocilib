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
// ReSharper disable CppClangTidyModernizeUseAuto
// ReSharper disable CppClangTidyReadabilityInconsistentDeclarationParameterName

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

    inline ostring SodaColl::GetName() const
    {
        return core::MakeString(core::Check(OCI_SodaCollGetName(*this)));
    }

    inline ostring SodaColl::GetMetadata() const
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

    inline SodaDoc SodaColl::FindOne(const ostring& key)
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

    inline UInt64 SodaColl::GetCount(const ostring& filter)
    {
        return OCI_SodaCollDocCountWithFilter(*this, filter.c_str());
    }

    inline bool SodaColl::ReplaceOne(const SodaOptions& options, const SodaDoc& doc)
    {
        return (core::Check(OCI_SodaCollReplaceOne(*this, options, doc)) == TRUE);
    }

    inline SodaDoc SodaColl::ReplaceOneAndGet(const SodaOptions& options, SodaDoc& doc)
    {
        core::Check(OCI_SodaCollReplaceOneAndGet(*this, options, doc));
        return doc;
    }

    inline bool SodaColl::ReplaceOne(const ostring& key, const SodaDoc& doc)
    {
        return (core::Check(OCI_SodaCollReplaceOneByKey(*this, key.c_str(), doc)) == TRUE);
    }

    inline SodaDoc SodaColl::ReplaceOneAndGet(const ostring& key, SodaDoc& doc)
    {
        core::Check(OCI_SodaCollReplaceOneAndGetByKey(*this, key.c_str(), doc));
        return doc;
    }

    inline bool SodaColl::Remove(const SodaOptions& options, UInt64* count)
    {
        return (core::Check(OCI_SodaCollRemove(*this, options, count)) == TRUE);
    }

    inline bool SodaColl::RemoveOne(const ostring& key)
    {
        return (core::Check(OCI_SodaCollRemoveOneByKey(*this, key.c_str())) == TRUE);
    }

}
