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

#include "ocilibcpp/config.hpp"

namespace ocilib
{
    namespace support
    {
        /**
        * @brief Allow resolving a the C API numeric enumerated type from a C++ type
        */
        template<class T> struct NumericTypeResolver {};

        template<> struct NumericTypeResolver<OCI_Number*> { enum { Value = NumericNumber }; };
        template<> struct NumericTypeResolver<Number> { enum { Value = NumericNumber }; };
        template<> struct NumericTypeResolver<short> { enum { Value = NumericShort }; };
        template<> struct NumericTypeResolver<unsigned short> { enum { Value = NumericUnsignedShort }; };
        template<> struct NumericTypeResolver<int> { enum { Value = NumericInt }; };
        template<> struct NumericTypeResolver<unsigned int> { enum { Value = NumericUnsignedInt }; };
        template<> struct NumericTypeResolver<big_int> { enum { Value = NumericBigInt }; };
        template<> struct NumericTypeResolver<big_uint> { enum { Value = NumericUnsignedBigInt }; };
        template<> struct NumericTypeResolver<double> { enum { Value = NumericDouble }; };
        template<> struct NumericTypeResolver<float> { enum { Value = NumericFloat }; };
    }
}
