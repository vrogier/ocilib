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
 // ReSharper disable CppClangTidyHicppUseAuto
 // ReSharper disable CppClangTidyModernizeUseAuto
 // ReSharper disable CppClangTidyMiscMisplacedConst

namespace ocilib
{

    template<class T, Int32 U>
    inline Lob<T, U>::Lob()
    {
    }

    template<class T, Int32 U>
    Lob<T, U>::Lob(const Connection& connection)
    {
        AcquireAllocated
        (
            core::Check(OCI_LobCreate(connection, U)),
            connection.GetHandle()
        );
    }

    template<class T, Int32 U>
    Lob<T, U>::Lob(OCI_Lob* pLob, core::Handle* parent)
    {
        AcquireTransient(pLob, parent);
    }

    template<>
    inline String Lob<String, LobCharacter>::Read(UInt32 length)
    {
        std::vector<otext> buffer(static_cast<Size>(core::ComputeCharMaxSize(Environment::CharsetMode(static_cast<Environment::CharsetMode::Type>(core::Check(OCI_GetCharset(GetEnvironment()))))) * (length + 1)));

        UInt32 charCount = length;
        UInt32 byteCount = 0;

        if (core::Check(OCI_LobRead(*this, static_cast<AnyPointer>(buffer.data()), &charCount, &byteCount)))
        {
            length = byteCount / sizeof(otext);
        }

        return core::MakeString(static_cast<const otext*>(buffer.data()), static_cast<Int32>(length));
    }

    template<>
    inline String Lob<String, LobNationalCharacter>::Read(UInt32 length)
    {
        std::vector<otext> buffer(static_cast<Size>(core::ComputeCharMaxSize(Environment::CharsetMode(static_cast<Environment::CharsetMode::Type>(core::Check(OCI_GetCharset(GetEnvironment()))))) * (length + 1)));

        UInt32 charCount = length;
        UInt32 byteCount = 0;

        if (core::Check(OCI_LobRead(*this, static_cast<AnyPointer>(buffer.data()), &charCount, &byteCount)))
        {
            length = byteCount / sizeof(otext);
        }

        return core::MakeString(static_cast<const otext*>(buffer.data()), static_cast<Int32>(length));

    }

    template<>
    inline Raw Lob<Raw, LobBinary>::Read(UInt32 length)
    {
        std::vector<unsigned char> buffer(static_cast<Size>(length + 1));

        UInt32 charCount = 0;
        UInt32 byteCount = length;

        core::Check(OCI_LobRead(*this, static_cast<AnyPointer>(buffer.data()), &charCount, &byteCount));

        return core::MakeRaw(buffer.data(), byteCount);
    }

    template<class T, Int32 U>
    UInt32 Lob<T, U>::Write(const T& content)
    {
        if (content.empty())
        {
            return 0;
        }

        UInt32 res = 0;
        UInt32 charCount = 0;
        UInt32 byteCount = static_cast<UInt32>(content.size() * sizeof(typename T::value_type));
        const AnyPointer buffer = static_cast<AnyPointer>(const_cast<typename T::value_type*>(&content[0]));

        if (core::Check(OCI_LobWrite(*this, buffer, &charCount, &byteCount)))
        {
            res = U == LobBinary ? byteCount : charCount;
        }

        return res;
    }

    template<class T, Int32 U>
    void Lob<T, U>::Append(const Lob& other)
    {
        core::Check(OCI_LobAppendLob(*this, other));
    }

    template<class T, Int32 U>
    UInt32 Lob<T, U>::Append(const T& content)
    {
        if (content.empty())
        {
            return 0;
        }

        const AnyPointer data = static_cast<AnyPointer>(const_cast<typename T::value_type*>(&content[0]));

        UInt32 charCount = 0;
        UInt32 byteCount = static_cast<UInt32>(content.size() * sizeof(typename T::value_type));

        core::Check(OCI_LobAppend(*this, data, &charCount, &byteCount));

        return (U == LobBinary ? byteCount : charCount);
    }

    template<class T, Int32 U>
    bool Lob<T, U>::Seek(SeekMode seekMode, UInt64 offset)
    {
        return (core::Check(OCI_LobSeek(*this, offset, seekMode)) == TRUE);
    }

    template<class T, Int32 U>
    Lob<T, U> Lob<T, U>::Clone() const
    {
        Lob result;
        Connection connection = GetConnection();

        result.AcquireAllocated
        (
            core::Check(OCI_LobCreate(connection, U)),
            connection.GetHandle()
        );

        core::Check(OCI_LobAssign(result, *this));

        return result;
    }

    template<class T, Int32 U>
    bool Lob<T, U>::Equals(const Lob& other) const
    {
        return (core::Check(OCI_LobIsEqual(*this, other)) == TRUE);
    }

    template<class T, Int32 U>
    LobType Lob<T, U>::GetType() const
    {
        return LobType(static_cast<LobType::Type>(core::Check(OCI_LobGetType(*this))));
    }

    template<class T, Int32 U>
    UInt64 Lob<T, U>::GetOffset() const
    {
        return core::Check(OCI_LobGetOffset(*this));
    }

    template<class T, Int32 U>
    UInt64 Lob<T, U>::GetLength() const
    {
        return core::Check(OCI_LobGetLength(*this));
    }

    template<class T, Int32 U>
    UInt64 Lob<T, U>::GetMaxSize() const
    {
        return core::Check(OCI_LobGetMaxSize(*this));
    }

    template<class T, Int32 U>
    UInt64 Lob<T, U>::GetChunkSize() const
    {
        return core::Check(OCI_LobGetChunkSize(*this));
    }

    template<class T, Int32 U>
    Connection Lob<T, U>::GetConnection() const
    {
        core::Handle* selfHandle = this->GetHandle();
        core::Handle* parentHandle = selfHandle ? selfHandle->GetParent() : nullptr;
        core::Handle* connectionParent = parentHandle ? parentHandle->GetParent() : GetHandle();

        return Connection
        (
            core::Check(OCI_LobGetConnection(*this)),
            connectionParent,
            false
        );
    }

    template<class T, Int32 U>
    void Lob<T, U>::Truncate(UInt64 length)
    {
        core::Check(OCI_LobTruncate(*this, length));
    }

    template<class T, Int32 U>
    UInt64 Lob<T, U>::Erase(UInt64 offset, UInt64 length)
    {
        return core::Check(OCI_LobErase(*this, offset, length));
    }

    template<class T, Int32 U>
    void Lob<T, U>::Copy(Lob& dest, UInt64 offset, UInt64 offsetDest, UInt64 length) const
    {
        core::Check(OCI_LobCopy(dest, *this, offsetDest, offset, length));
    }

    template<class T, Int32 U>
    bool Lob<T, U>::IsTemporary() const
    {
        return (core::Check(OCI_LobIsTemporary(*this)) == TRUE);
    }

    template<class T, Int32 U>
    bool Lob<T, U>::IsRemote() const
    {
        return (core::Check(OCI_LobIsRemote(*this)) == TRUE);
    }

    template<class T, Int32 U>
    void Lob<T, U>::Open(OpenMode mode)
    {
        core::Check(OCI_LobOpen(*this, mode));
    }

    template<class T, Int32 U>
    void Lob<T, U>::Flush()
    {
        core::Check(OCI_LobFlush(*this));
    }

    template<class T, Int32 U>
    void Lob<T, U>::Close()
    {
        core::Check(OCI_LobClose(*this));
    }

    template<class T, Int32 U>
    void Lob<T, U>::EnableBuffering(bool value)
    {
        core::Check(OCI_LobEnableBuffering(*this, value));
    }

    template<class T, Int32 U>
    Lob<T, U>& Lob<T, U>::operator += (const Lob<T, U>& other)
    {
        Append(other);
        return *this;
    }

    template<class T, Int32 U>
    bool Lob<T, U>::operator == (const Lob<T, U>& other) const
    {
        return Equals(other);
    }

    template<class T, Int32 U>
    bool Lob<T, U>::operator != (const Lob<T, U>& other) const
    {
        return !(*this == other);
    }

}
