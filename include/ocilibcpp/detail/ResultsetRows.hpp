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

    inline ResultsetRows::ResultsetRows(Resultset* resultset)
        : _rs(resultset)
    {
    }

    inline RowIterator ResultsetRows::begin()
    {
        return RowIterator(_rs, !_rs->Next());
    }

    inline RowIterator ResultsetRows::end()
    {
        return RowIterator(_rs, true);
    }

    inline UInt32 ResultsetRows::Size() const
    {
        return static_cast<UInt32>(_rs->GetCurrentRow() + 1);
    }

    inline Row ResultsetRows::operator[](Int32 index) const
    {
        _rs->Seek(Resultset::SeekAbsolute, index);
        return Row(_rs);
    }

}
