/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2026 Vincent ROGIER <vince.rogier@gmail.com>
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
        template<class I, class O, OCI_Boolean B>
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

        template<> struct BindResolver<bool> : BindResolverType<bool, OCI_Boolean, false> {};
        template<> struct BindResolver<Int16> : BindResolverScalarType<Int16> {};
        template<> struct BindResolver<UInt16> : BindResolverScalarType<UInt16> {};
        template<> struct BindResolver<Int32> : BindResolverScalarType<Int32> {};
        template<> struct BindResolver<UInt32> : BindResolverScalarType<UInt32> {};
        template<> struct BindResolver<Int64> : BindResolverScalarType<Int64> {};
        template<> struct BindResolver<UInt64> : BindResolverScalarType<UInt64> {};
        template<> struct BindResolver<Float> : BindResolverScalarType<Float> {};
        template<> struct BindResolver<Double> : BindResolverScalarType<Double> {};
        template<> struct BindResolver<String> : BindResolverType<String, otext, false> {};
        template<> struct BindResolver<Raw> : BindResolverType<String, unsigned char, false> {};
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
        template<> struct BindResolver<Reference> : BindResolverHandleType<Reference, OCI_Reference*> {};
        template<> struct BindResolver<Object> : BindResolverHandleType<Object, OCI_Object*> {};
        template<> struct BindResolver<Vector> : BindResolverHandleType<Vector, OCI_Vector*> {};
        template<> struct BindResolver<Json> : BindResolverHandleType<Json, OCI_Json*> {};
        template<> struct BindResolver<Statement> : BindResolverHandleType<Statement, OCI_Statement*> {};
    }
}
