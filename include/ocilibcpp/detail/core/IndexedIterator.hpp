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
    namespace core
    {

        /**
         * @brief
         * CRTP base for random-access index-based iterators
         *
         * Provides the full random-access operator set; the derived class
         * supplies deref(pos) converting a 0-based position into a value
         * (usually translating +1 into the underlying 1-based API).
         *
         */
        template<class D, class T, class V>
        class IndexedIterator
        {
        public:

            typedef std::random_access_iterator_tag iterator_category;
            typedef V                          value_type;
            typedef ptrdiff_t                       difference_type;
            typedef void                            pointer;
            typedef V                          reference;

            IndexedIterator() = default;

            V operator * () const
            {
                return derived().deref(static_cast<UInt32>(_pos));
            }

            V operator [] (difference_type offset) const
            {
                return derived().deref(static_cast<UInt32>(_pos + offset));
            }

            D& operator ++ ()
            {
                ++_pos;
                return derived();
            }

            D operator ++ (int)
            {
                D other = derived();
                ++_pos;
                return other;
            }

            D& operator -- ()
            {
                --_pos;
                return derived();
            }

            D operator -- (int)
            {
                D other = derived();
                --_pos;
                return other;
            }

            D& operator += (difference_type offset)
            {
                _pos += offset;
                return derived();
            }

            D& operator -= (difference_type offset)
            {
                _pos -= offset;
                return derived();
            }

            D operator + (difference_type offset) const
            {
                D other = derived();
                other._pos += offset;
                return other;
            }

            D operator - (difference_type offset) const
            {
                D other = derived();
                other._pos -= offset;
                return other;
            }

            difference_type operator - (const IndexedIterator& other) const
            {
                return _pos - other._pos;
            }

            bool operator == (const IndexedIterator& other) const
            {
                return _pos == other._pos && _owner == other._owner;
            }

            bool operator != (const IndexedIterator& other) const
            {
                return !(*this == other);
            }

            bool operator < (const IndexedIterator& other) const
            {
                return _pos < other._pos;
            }

            bool operator > (const IndexedIterator& other) const
            {
                return _pos > other._pos;
            }

            bool operator <= (const IndexedIterator& other) const
            {
                return _pos <= other._pos;
            }

            bool operator >= (const IndexedIterator& other) const
            {
                return _pos >= other._pos;
            }

        protected:

            IndexedIterator(T* owner, difference_type pos)
                : _owner(owner), _pos(pos)
            {
            }

            T* _owner = nullptr;
            difference_type _pos  = 0;

        private:

            D& derived()
            {
                return *static_cast<D*>(this);
            }

            const D& derived() const
            {
                return *static_cast<const D*>(this);
            }
        };

    }
}
