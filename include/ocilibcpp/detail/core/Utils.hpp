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

#include "ocilibcpp/core.hpp"

namespace ocilib
{
    namespace core
    {
        template<class T>
        static T* OnAllocate(T* address, [[maybe_unused]] Size count = 1)
        {
#ifdef OCILIBPP_DEBUG_MEMORY_ENABLED
            GetMemoryDebugInfo().OnAllocate(address, count);
#endif

            return address;
        }

        template<class T>
        static T* OnDeallocate(T* address)
        {
#ifdef OCILIBPP_DEBUG_MEMORY_ENABLED
            GetMemoryDebugInfo().OnDeallocate(address);
#endif

            return address;
        }

        template<class T>
        T Check(T result)
        {
            /* OCI_HasError() applies the warnings gate: a suppressed warning
               records an OCI_Error but must not raise a C++ exception */

            if (OCI_HasError())
            {
                throw Exception(OCI_GetLastError());
            }

            return result;
        }

        inline String MakeString(const otext* result, Int32 size)
        {
            return result ? (size >= 0 ? String(result, result + size) : String(result)) : String();
        }

        inline Raw MakeRaw(AnyPointer result, UInt32 size)
        {
            unsigned char* ptr = static_cast<unsigned char*>(result);

            return (ptr && size > 0 ? Raw(ptr, ptr + size) : Raw());
        }

        inline UInt32 ComputeCharMaxSize(Environment::CharsetMode charsetMode)
        {
            const Int32 UTF8_BytesPerChar = 4;

            UInt32 res = sizeof(String::value_type);

            if (charsetMode == Environment::CharsetAnsi)
            {
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4996)
#endif
                char* str = getenv("NLS_LANG");

#ifdef _MSC_VER
#pragma warning(pop)
#endif

                if (str)
                {
                    std::string nlsLang = str;

                    for (char &i :nlsLang)
                    {
                        i = static_cast<std::string::value_type>(toupper(i));
                    }

                    if (String::npos != nlsLang.find("UTF8"))
                    {
                        res = UTF8_BytesPerChar;
                    }
                }
            }

            return res;
        }
    }
}
