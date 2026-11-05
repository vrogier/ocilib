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

    inline ResultsetIterator::ResultsetIterator(Resultset* resultset, bool eof)
        : _rs(resultset), _eof(eof)
    {
    }

    inline Row ResultsetIterator::operator * () const
    {
        return Row(_rs);
    }

    inline ResultsetIterator& ResultsetIterator::operator ++ ()
    {
        _eof = !_rs->Next();
        return *this;
    }

    inline ResultsetIterator ResultsetIterator::operator ++ (Int32)
    {
        ResultsetIterator other = *this;
        _eof = !_rs->Next();
        return other;
    }

    inline ResultsetIterator& ResultsetIterator::operator -- ()
    {
        _eof = !_rs->Prev();
        return *this;
    }

    inline ResultsetIterator ResultsetIterator::operator -- (Int32)
    {
        ResultsetIterator other = *this;
        _rs->Prev();
        return other;
    }

    inline ResultsetIterator& ResultsetIterator::operator += (Int32 offset)
    {
        _rs->Seek(Resultset::SeekRelative, offset);
        _eof = false;
        return *this;
    }

    inline ResultsetIterator& ResultsetIterator::operator -= (Int32 offset)
    {
        _rs->Seek(Resultset::SeekRelative, -offset);
        return *this;
    }

    inline Row ResultsetIterator::operator [] (Int32 offset) const
    {
        _rs->Seek(Resultset::SeekRelative, offset);
        return Row(_rs);
    }

    inline bool ResultsetIterator::operator == (const ResultsetIterator& other) const
    {
        return (_eof && other._eof) || (_eof == other._eof && _rs == other._rs);
    }

    inline bool ResultsetIterator::operator != (const ResultsetIterator& other) const
    {
        return !(*this == other);
    }

}
