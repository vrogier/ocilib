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

#include "ocilibcpp/core.hpp"

// ReSharper disable CppClangTidyHicppUseAuto
// ReSharper disable CppClangTidyModernizeUseAuto

namespace ocilib
{
    namespace core
    {
        template<class T>
        static T* OnAllocate(T* address, size_t count = 1)
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
            OCI_Error* err = OCI_GetLastError();

            if (err)
            {
                throw Exception(err);
            }

            return result;
        }

        inline ostring MakeString(const otext* result, int size)
        {
            return result ? (size >= 0 ? ostring(result, result + size) : ostring(result)) : ostring();
        }

        inline Raw MakeRaw(AnyPointer result, unsigned int size)
        {
            unsigned char* ptr = static_cast<unsigned char*>(result);

            return (ptr && size > 0 ? Raw(ptr, ptr + size) : Raw());
        }

        inline unsigned int ComputeCharMaxSize(Environment::CharsetMode charsetMode)
        {
            const int UTF8_BytesPerChar = 4;

            unsigned int res = sizeof(ostring::value_type);

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

                    if (ostring::npos != nlsLang.find("UTF8"))
                    {
                        res = UTF8_BytesPerChar;
                    }
                }
            }

            return res;
        }
    }
}
