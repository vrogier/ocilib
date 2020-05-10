/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2020 Vincent ROGIER <vince.rogier@ocilib.net>
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
        template<> struct BindResolver<ostring> : BindResolverType<ostring, otext, false> {};
        template<> struct BindResolver<Raw> : BindResolverType<ostring, unsigned char, false> {};
        template<> struct BindResolver<Number> : BindResolverHandleType<Number, OCI_Number*> {};
        template<> struct BindResolver<Date> : BindResolverHandleType<Date, OCI_Date*> {};
        template<> struct BindResolver<Timestamp> : BindResolverHandleType<Timestamp, OCI_Timestamp*> {};
        template<> struct BindResolver<Interval> : BindResolverHandleType<Interval, OCI_Interval*> {};
        template<> struct BindResolver<Clob> : BindResolverHandleType<Clob, OCI_Lob*> {};
        template<> struct BindResolver<NClob> : BindResolverHandleType<NClob, OCI_Lob*> {};
        template<> struct BindResolver<Blob> : BindResolverHandleType<Blob, OCI_Lob*> {};
        template<> struct BindResolver<File> : BindResolverHandleType<File, OCI_File*> {};
        template<> struct BindResolver<Clong> : BindResolverHandleType<Clong, OCI_Long*> {};
        template<> struct BindResolver<Blong> : BindResolverHandleType<Blong, OCI_Long*> {};
        template<> struct BindResolver<Reference> : BindResolverHandleType<Reference, OCI_Ref*> {};
        template<> struct BindResolver<Object> : BindResolverHandleType<Object, OCI_Object*> {};
        template<> struct BindResolver<Statement> : BindResolverHandleType<Statement, OCI_Statement*> {};
    }
}
