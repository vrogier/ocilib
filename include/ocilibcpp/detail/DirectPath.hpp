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

    inline DirectPath::DirectPath(const TypeInfo &typeInfo, UInt32 nbCols, UInt32 nbRows, const String& partition)
    {
        Connection connection = typeInfo.GetConnection();

        AcquireAllocated
        (
            core::Check(OCI_DirectPathCreate(typeInfo, partition.c_str(), nbCols, nbRows)),
            connection.GetHandle()
        );
    }

    inline void DirectPath::SetColumn(UInt32 colIndex, const String& name, UInt32 maxSize,  const String& format)
    {
        core::Check(OCI_DirectPathSetColumn(*this, colIndex, name.c_str(), maxSize, format.c_str()));
    }

    template<class T>
    inline void DirectPath::SetEntry(UInt32 rowIndex, UInt32 colIndex, const T &value, bool complete)
    {
        core::Check(OCI_DirectPathSetEntry(*this, rowIndex, colIndex, static_cast<const AnyPointer>(const_cast<typename T::value_type *>(value.c_str())),
                                           static_cast<UInt32>(value.size()), complete));
    }

    inline void DirectPath::Reset()
    {
        core::Check(OCI_DirectPathReset(*this));
    }

    inline void DirectPath::Prepare()
    {
        core::Check(OCI_DirectPathPrepare(*this));
    }

    inline DirectPath::Result DirectPath::Convert()
    {
        return Result(static_cast<Result::Type>(core::Check(OCI_DirectPathConvert(*this))));
    }

    inline DirectPath::Result DirectPath::Load()
    {
        return Result(static_cast<Result::Type>(core::Check(OCI_DirectPathLoad(*this))));
    }

    inline void DirectPath::Finish()
    {
        core::Check(OCI_DirectPathFinish(*this));
    }

    inline void DirectPath::Abort()
    {
        core::Check(OCI_DirectPathAbort(*this));
    }

    inline void DirectPath::Save()
    {
        core::Check(OCI_DirectPathSave(*this));
    }

    inline void DirectPath::FlushRow()
    {
        core::Check(OCI_DirectPathFlushRow(*this));
    }

    inline void DirectPath::SetCurrentRows(UInt32 value)
    {
        core::Check(OCI_DirectPathSetCurrentRows(*this, value));
    }

    inline UInt32 DirectPath::GetCurrentRows() const
    {
        return core::Check(OCI_DirectPathGetCurrentRows(*this));
    }

    inline UInt32 DirectPath::GetMaxRows() const
    {
        return core::Check(OCI_DirectPathGetMaxRows(*this));
    }

    inline UInt32 DirectPath::GetRowCount() const
    {
        return core::Check(OCI_DirectPathGetRowCount(*this));
    }

    inline UInt32 DirectPath::GetAffectedRows() const
    {
        return core::Check(OCI_DirectPathGetAffectedRows(*this));
    }

    inline void DirectPath::SetDateFormat(const String& format)
    {
        core::Check(OCI_DirectPathSetDateFormat(*this, format.c_str()));
    }

    inline void DirectPath::SetParallel(bool value)
    {
        core::Check(OCI_DirectPathSetParallel(*this, value));
    }

    inline void DirectPath::SetNoLog(bool value)
    {
        core::Check(OCI_DirectPathSetNoLog(*this, value));
    }

    inline void DirectPath::SetCacheSize(UInt32 value)
    {
        core::Check(OCI_DirectPathSetCacheSize(*this, value));
    }

    inline void DirectPath::SetBufferSize(UInt32 value)
    {
        core::Check(OCI_DirectPathSetBufferSize(*this, value));
    }

    inline void DirectPath::SetConversionMode(ConversionMode value)
    {
        core::Check(OCI_DirectPathSetConvertMode(*this, value));
    }

    inline Int32 DirectPath::GetErrorColumn()
    {
        return core::Check(OCI_DirectPathGetErrorColumn(*this));
    }

    inline Int32 DirectPath::GetErrorRow()
    {
        return core::Check(OCI_DirectPathGetErrorRow(*this));
    }

}
