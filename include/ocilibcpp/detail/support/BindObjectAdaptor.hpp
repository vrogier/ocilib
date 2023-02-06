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

#include "ocilibcpp/support.hpp"

namespace ocilib
{
    namespace support
    {
        template<class T>
        void BindObjectAdaptor<T>::SetInData()
        {
            if (GetMode() & OCI_BDM_IN)
            {
                size_t size = _object.size();

                if (size > _size)
                {
                    size = _size;
                }

                if (size > 0)
                {
                    memcpy(_data, &_object[0], size * sizeof(NativeType));
                }

                _data[size] = 0;
            }
        }

        template<class T>
        void BindObjectAdaptor<T>::SetOutData()
        {
            if (GetMode() & OCI_BDM_OUT)
            {
                OCI_Bind* bnd = core::Check(OCI_GetBind2(_statement, _name.c_str()));
                size_t size = core::Check(OCI_BindGetDataSize(bnd));

                _object.assign(_data, _data + size);
            }
        }

        template<class T>
        BindObjectAdaptor<T>::BindObjectAdaptor(const ocilib::Statement& statement, const ostring& name, unsigned int mode, ObjectType& object, unsigned int size) :
            BindObject(statement, name, mode),
            _object(object),
            _data(core::OnAllocate(new NativeType[size + 1], size + 1)),
            _size(size)
        {
            memset(_data, 0, (_size + 1) * sizeof(NativeType));
        }

        template<class T>
        BindObjectAdaptor<T>::~BindObjectAdaptor() noexcept
        {
            delete[] core::OnDeallocate(_data);
        }

        template<class T>
        BindObjectAdaptor<T>::operator NativeType* ()  const
        {
            return _data;
        }
    }
}
