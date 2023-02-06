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
        inline BindsHolder::BindsHolder(const ocilib::Statement& statement) : _statement(statement)
        {

        }

        inline BindsHolder::~BindsHolder() noexcept
        {
            Clear();
        }

        inline void BindsHolder::Clear()
        {
            std::vector<BindObject*>::iterator it, it_end;

            for (it = _bindObjects.begin(), it_end = _bindObjects.end(); it != it_end; ++it)
            {
                delete core::OnDeallocate(*it);
            }

            _bindObjects.clear();
        }

        inline void BindsHolder::AddBindObject(BindObject* bindObject)
        {
            if (core::Check(OCI_IsRebindingAllowed(_statement)))
            {
                std::vector<BindObject*>::iterator it, it_end;

                for (it = _bindObjects.begin(), it_end = _bindObjects.end(); it != it_end; ++it)
                {
                    BindObject* previousBindObject = *it;
                    if (previousBindObject->GetName() == bindObject->GetName())
                    {
                        _bindObjects.erase(it);
                        delete core::OnDeallocate(previousBindObject);
                        break;
                    }
                }
            }

            _bindObjects.push_back(bindObject);
        }

        inline void BindsHolder::SetOutData()
        {
            std::vector<BindObject*>::iterator it, it_end;

            for (it = _bindObjects.begin(), it_end = _bindObjects.end(); it != it_end; ++it)
            {
                (*it)->SetOutData();
            }
        }

        inline void BindsHolder::SetInData()
        {
            std::vector<BindObject*>::iterator it, it_end;

            for (it = _bindObjects.begin(), it_end = _bindObjects.end(); it != it_end; ++it)
            {
                (*it)->SetInData();
            }
        }
    }
}
