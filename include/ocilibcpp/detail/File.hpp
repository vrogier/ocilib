/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2023 Vincent ROGIER <vince.rogier@ocilib.net>
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

inline File::File()
{
}

inline File::File(const Connection &connection)
{
    AcquireAllocated
    (
        core::Check(OCI_FileCreate(connection, OCI_BFILE)), 
        connection.GetHandle()
    );
}

inline File::File(const Connection &connection, const ostring& directory, const ostring& name)
{
    AcquireAllocated
    (
        core::Check(OCI_FileCreate(connection, OCI_BFILE)), 
        connection.GetHandle()
    );

    SetInfos(directory, name);
}

inline File::File(OCI_File *pFile, core::Handle *parent)
{
    AcquireTransient(pFile,parent);
}

inline Raw File::Read(unsigned int size)
{
    core::ManagedBuffer<unsigned char> buffer(static_cast<size_t>(size + 1));

    size = core::Check(OCI_FileRead(*this, static_cast<AnyPointer>(buffer), size));

    return core::MakeRaw(buffer, size);
}

inline bool File::Seek(SeekMode seekMode, big_uint offset)
{
    return (core::Check(OCI_FileSeek(*this, offset, seekMode)) == TRUE);
}

inline File File::Clone() const
{
    File result(GetConnection());

    core::Check(OCI_FileAssign(result, *this));

    return result;
}

inline bool File::Equals(const File &other) const
{
    return (core::Check(OCI_FileIsEqual(*this, other)) == TRUE);
}

inline big_uint File::GetOffset() const
{
    return core::Check(OCI_FileGetOffset(*this));
}

inline big_uint File::GetLength() const
{
    return core::Check(OCI_FileGetSize(*this));
}

inline Connection File::GetConnection() const
{
    return Connection
    (
        core::Check(OCI_FileGetConnection(*this)), 
        Environment::GetEnvironmentHandle()
    );
}

inline bool File::Exists() const
{
    return (core::Check(OCI_FileExists(*this)) == TRUE);
}

inline void File::SetInfos(const ostring& directory, const ostring& name)
{
    core::Check(OCI_FileSetName(*this, directory.c_str(), name.c_str()));
}

inline ostring File::GetName() const
{
    return core::MakeString(core::Check(OCI_FileGetName(*this)));
}

inline ostring File::GetDirectory() const
{
    return core::MakeString(core::Check(OCI_FileGetDirectory(*this)));
}

inline void File::Open()
{
    core::Check(OCI_FileOpen(*this));
}

inline bool File::IsOpened() const
{
    return (core::Check(OCI_FileIsOpen(*this)) == TRUE);
}

inline void File::Close()
{
    core::Check(OCI_FileClose(*this));
}

inline bool File::operator == (const File& other) const
{
    return Equals(other);
}

inline bool File::operator != (const File& other) const
{
    return (!(*this == other));
}

}
