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
        /**
        *
        * @brief Resolve a bind input / output types
        *
        */
        template<class I, class O, boolean B>
        struct BindResolverType
        {
            typedef I InputType;
            typedef O OutputType;
            static const bool IsHandle = B;
        };

        /**
        *
        * @brief Simplified resolver for scalar types that do not need translation
        *
        */
        template<class T>
        struct BindResolverScalarType : BindResolverType<T, T, false> {};

        /**
        *
        * @brief Simplified resolver for handle types
        *
        */
        template<class I, class O>
        struct BindResolverHandleType : BindResolverType<I, O, true> {};

        template<> struct BindResolver<bool> : BindResolverType<bool, boolean, false> {};
        template<> struct BindResolver<short> : BindResolverScalarType<short> {};
        template<> struct BindResolver<unsigned short> : BindResolverScalarType<unsigned short> {};
        template<> struct BindResolver<int> : BindResolverScalarType<int> {};
        template<> struct BindResolver<unsigned int> : BindResolverScalarType<unsigned  int> {};
        template<> struct BindResolver<big_int> : BindResolverScalarType<big_int> {};
        template<> struct BindResolver<big_uint> : BindResolverScalarType<big_uint> {};
        template<> struct BindResolver<float> : BindResolverScalarType<float> {};
        template<> struct BindResolver<double> : BindResolverScalarType<double> {};
        template<> struct BindResolver<ocilib::ostring> : BindResolverType<ocilib::ostring, otext, false> {};
        template<> struct BindResolver<ocilib::Raw> : BindResolverType<ocilib::ostring, unsigned char, false> {};
        template<> struct BindResolver<ocilib::Number> : BindResolverHandleType<ocilib::Number, OCI_Number*> {};
        template<> struct BindResolver<ocilib::Date> : BindResolverHandleType<ocilib::Date, OCI_Date*> {};
        template<> struct BindResolver<ocilib::Timestamp> : BindResolverHandleType<ocilib::Timestamp, OCI_Timestamp*> {};
        template<> struct BindResolver<ocilib::Interval> : BindResolverHandleType<ocilib::Interval, OCI_Interval*> {};
        template<> struct BindResolver<ocilib::Clob> : BindResolverHandleType<ocilib::Clob, OCI_Lob*> {};
        template<> struct BindResolver<ocilib::NClob> : BindResolverHandleType<ocilib::NClob, OCI_Lob*> {};
        template<> struct BindResolver<ocilib::Blob> : BindResolverHandleType<ocilib::Blob, OCI_Lob*> {};
        template<> struct BindResolver<ocilib::File> : BindResolverHandleType<ocilib::File, OCI_File*> {};
        template<> struct BindResolver<ocilib::Clong> : BindResolverHandleType<ocilib::Clong, OCI_Long*> {};
        template<> struct BindResolver<ocilib::Blong> : BindResolverHandleType<ocilib::Blong, OCI_Long*> {};
        template<> struct BindResolver<ocilib::Reference> : BindResolverHandleType<ocilib::Reference, OCI_Ref*> {};
        template<> struct BindResolver<ocilib::Object> : BindResolverHandleType<ocilib::Object, OCI_Object*> {};
        template<> struct BindResolver<ocilib::Statement> : BindResolverHandleType<ocilib::Statement, OCI_Statement*> {};
    }
}
