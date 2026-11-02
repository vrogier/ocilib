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

#include "ocilibcpp/support.hpp"

// ReSharper disable CppClangTidyModernizePassByValue

namespace ocilib
{
    namespace support
    {
        inline BindArray::BindArray(const ocilib::Statement& statement, const ostring& name, OCI_UInt32 mode) : BindObject(statement, name, mode), _object(nullptr)
        {

        }

        template<class T>
        void BindArray::SetVector(std::vector<T>& vector, bool isPlSqlTable, OCI_UInt32 elemSize)
        {
            _object = core::OnAllocate(new BindArrayObject<T>(_statement, GetName(), vector, isPlSqlTable, GetMode(), elemSize));
        }

        inline BindArray::~BindArray() noexcept
        {
            delete core::OnDeallocate(_object);
        }

        template<class T>
        typename BindResolver<T>::OutputType* BindArray::GetData()  const
        {
            return static_cast<typename BindResolver<T>::OutputType*>(*(dynamic_cast<BindArrayObject<T>*> (_object)));
        }

        inline void BindArray::SetInData()
        {

            if (GetMode() & OCI_BDM_IN || _object->IsHandleObject())
            {
                _object->SetInData();
            }
        }

        inline void BindArray::SetOutData()
        {
            if (GetMode() & OCI_BDM_OUT)
            {
                _object->SetOutData();
            }
        }

        inline OCI_UInt32 BindArray::GetSize() const
        {
            return _object ? _object->GetSize() : _statement.GetBindArraySize();
        }

        inline OCI_UInt32 BindArray::GetSizeForBindCall() const
        {
            return _object ? _object->GetSizeForBindCall() : 0;
        }


        template<class T>
        BindArray::BindArrayObject<T>::BindArrayObject(const ocilib::Statement& statement, const ostring& name, ObjectVector& vector, bool isPlSqlTable, OCI_UInt32 mode, OCI_UInt32 elemSize)
            : _statement(statement), _name(name), _vector(vector), _data(nullptr), _isPlSqlTable(isPlSqlTable), _mode(mode), _elemCount(BindArrayObject<T>::GetSize()), _elemSize(elemSize)
        {
            AllocData();
        }

        template<class T>
        BindArray::BindArrayObject<T>::~BindArrayObject() noexcept
        {
            FreeData();
        }

        template<class T>
        void BindArray::BindArrayObject<T>::AllocData()
        {
            _data = core::OnAllocate(new NativeType[_elemCount], _elemCount);

            memset(_data, 0, sizeof(NativeType) * _elemCount);
        }

        template<>
        inline void BindArray::BindArrayObject<ocilib::ostring>::AllocData()
        {
            const OCI_Size count = static_cast<OCI_Size>(_elemSize * _elemCount);

            _data = core::OnAllocate(new otext[count], count);

            memset(_data, 0, count * sizeof(otext));
        }

        template<>
        inline void BindArray::BindArrayObject<ocilib::Raw> ::AllocData()
        {
            const OCI_Size count =  static_cast<OCI_Size>(_elemSize * _elemCount);

            _data = core::OnAllocate(new unsigned char[count], count);

            memset(_data, 0, count * sizeof(unsigned char));
        }

        template<class T>
        void BindArray::BindArrayObject<T>::FreeData() const
        {
            delete[] core::OnDeallocate(_data);
        }

        template<class T>
        void BindArray::BindArrayObject<T>::SetInData()
        {
            typename ObjectVector::iterator it, it_end;

            OCI_UInt32 index = 0;
            const OCI_UInt32 currElemCount = GetSize();

            for (it = _vector.begin(), it_end = _vector.end(); it != it_end && index < _elemCount && index < currElemCount; ++it, ++index)
            {
                _data[index] = static_cast<NativeType>(*it);
            }
        }

        template<>
        inline void BindArray::BindArrayObject<ocilib::ostring>::SetInData()
        {
            std::vector<ostring>::iterator it, it_end;

            OCI_UInt32 index = 0;
            const OCI_UInt32 currElemCount = GetSize();

            for (it = _vector.begin(), it_end = _vector.end(); it != it_end && index < _elemCount && index < currElemCount; ++it, ++index)
            {
                const ostring& value = *it;

                memcpy(_data + (_elemSize * index), value.c_str(), (value.size() + 1) * sizeof(otext));
            }
        }

        template<>
        inline void BindArray::BindArrayObject<ocilib::Raw>::SetInData()
        {
            std::vector<Raw>::iterator it, it_end;

            OCI_UInt32 index = 0;
            const OCI_UInt32 currElemCount = GetSize();

            for (it = _vector.begin(), it_end = _vector.end(); it != it_end && index < _elemCount && index < currElemCount; ++it, ++index)
            {
                Raw& value = *it;

                if (!value.empty())
                {
                    memcpy(_data + (_elemSize * index), &value[0], value.size());
                }

                OCI_BindSetDataSizeAtPos(OCI_GetBindByName(_statement, GetName().c_str()), index + 1, static_cast<OCI_UInt32>(value.size()));
            }
        }

        template<class T>
        void BindArray::BindArrayObject<T>::SetOutData()
        {
            typename ObjectVector::iterator it, it_end;

            OCI_UInt32 index = 0;
            const OCI_UInt32 currElemCount = GetSize();

            for (it = _vector.begin(), it_end = _vector.end(); it != it_end && index < _elemCount && index < currElemCount; ++it, ++index)
            {
                *it = static_cast<NativeType&>(_data[index]);
            }
        }

        template<>
        inline void BindArray::BindArrayObject<ocilib::ostring>::SetOutData()
        {
            std::vector<ostring>::iterator it, it_end;

            OCI_Bind* pBind = core::Check(OCI_GetBindByName(_statement, GetName().c_str()));

            OCI_UInt32 index = 0;
            const OCI_UInt32 currElemCount = GetSize();

            for (it = _vector.begin(), it_end = _vector.end(); it != it_end && index < _elemCount && index < currElemCount; ++it, ++index)
            {
                otext* currData = _data + (_elemSize * index);

                (*it).assign(currData, currData + core::Check(OCI_BindGetDataSizeAtPos(pBind, index + 1)));
            }
        }

        template<>
        inline void BindArray::BindArrayObject<ocilib::Raw>::SetOutData()
        {
            std::vector<Raw>::iterator it, it_end;

            OCI_Bind* pBind = core::Check(OCI_GetBindByName(_statement, GetName().c_str()));

            OCI_UInt32 index = 0;
            const OCI_UInt32 currElemCount = GetSize();

            for (it = _vector.begin(), it_end = _vector.end(); it != it_end && index < _elemCount && index < currElemCount; ++it, ++index)
            {
                unsigned char* currData = _data + (_elemSize * index);

                (*it).assign(currData, currData + core::Check(OCI_BindGetDataSizeAtPos(pBind, index + 1)));
            }
        }

        template<class T>
        ostring BindArray::BindArrayObject<T>::GetName() const
        {
            return _name;
        }

        template<class T>
        bool BindArray::BindArrayObject<T>::IsHandleObject() const
        {
            return BindResolver<T>::IsHandle;
        }

        template<class T>
        OCI_UInt32 BindArray::BindArrayObject<T>::GetSize() const
        {
            return _isPlSqlTable ? static_cast<OCI_UInt32>(_vector.size()) : _statement.GetBindArraySize();
        }

        template<class T>
        OCI_UInt32 BindArray::BindArrayObject<T>::GetSizeForBindCall() const
        {
            return _isPlSqlTable ? static_cast<OCI_UInt32>(_vector.size()) : 0;
        }

        template<class T>
        BindArray::BindArrayObject<T>::operator ObjectVector& ()  const
        {
            return _vector;
        }

        template<class T>
        BindArray::BindArrayObject<T>::operator NativeType* ()  const
        {
            return _data;
        }
    }
}
