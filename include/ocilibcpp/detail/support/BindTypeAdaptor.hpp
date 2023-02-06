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
        void BindTypeAdaptor<T>::SetInData()
        {
            if (GetMode() & OCI_BDM_IN)
            {
                *_data = static_cast<NativeType>(_object);
            }
        }

        template<class T>
        void BindTypeAdaptor<T>::SetOutData()
        {
            if (GetMode() & OCI_BDM_OUT)
            {
                _object = static_cast<T&>(*_data);
            }
        }

        template<class T>
        BindTypeAdaptor<T>::BindTypeAdaptor(const ocilib::Statement& statement, const ostring& name, unsigned int mode, ObjectType& object) :
            BindObject(statement, name, mode),
            _object(object),
            _data(core::OnAllocate(new NativeType))
        {

        }

        template<class T>
        BindTypeAdaptor<T>::~BindTypeAdaptor() noexcept
        {
            delete core::OnDeallocate(_data);
        }

        template<class T>
        BindTypeAdaptor<T>::operator NativeType* ()  const
        {
            return _data;
        }

        template<>
        inline void BindTypeAdaptor<bool>::SetInData()
        {
            if (GetMode() & OCI_BDM_IN)
            {
                *_data = (_object == true);
            }
        }

        template<>
        inline void BindTypeAdaptor<bool>::SetOutData()
        {
            if (GetMode() & OCI_BDM_OUT)
            {
                _object = (*_data == TRUE);
            }
        }
    }
}
