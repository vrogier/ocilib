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

#include <optional>
#include <string>
#include <vector>

/* Including C API declarations  */

extern "C" {
#include "ocilib.h"
}

/* C++17 is the minimum supported C++ standard */

#if __cplusplus < 201703L && (!defined(_MSVC_LANG) || _MSVC_LANG < 201703L)
#error "OCILIB C++ API requires C++17 or later"
#endif

#define OCILIBPP_HAS_CXX

#ifdef  OCILIBCPP_DEBUG_MEMORY

#include <iostream>
#define OCILIBPP_DEBUG_MEMORY_ENABLED

#endif

namespace ocilib
{

/**
 * @typedef ocilib::String
 *
 * @brief
 * string class wrapping the OCILIB otext * type and OTEXT() macros ( see @ref OcilibCApiSupportedCharsets )
 *
 * @note
 * - for ANSI builds, String is equivalent to std::string
 * - for UNICODE builds, String is equivalent to std::wstring
 *
 */
    typedef std::basic_string<otext, std::char_traits<otext>, std::allocator<otext> > String;

/**
 * @typedef ocilib::ostring
 * @brief
 * Alias for String (kept for backward compatibility)
 *
 */
    typedef String ostring;

/**
 * @typedef ocilib::StringView
 * @brief
 * A non-owning reference to a sequence of otext characters (std::basic_string_view<otext>)
 * Accepts String, const otext*, string literals and spans without allocation
 *
 */
    typedef std::basic_string_view<otext> StringView;

/**
 * @typedef ocilib::AnyPointer
 *
 * @brief
 * Alias for the generic void pointer
 *
 */
    typedef void * AnyPointer;

/**
* @typedef ocilib::Raw
*
* @brief
* C++ counterpart of SQL RAW data type
*
*/
    typedef std::vector<unsigned char> Raw;

/**
* @typedef ocilib::Int16
* @typedef ocilib::UInt16
* @typedef ocilib::Int32
* @typedef ocilib::UInt32
* @typedef ocilib::Int64
* @typedef ocilib::UInt64
* @typedef ocilib::Float
* @typedef ocilib::Double
* @typedef ocilib::Size
*
* @brief
* C++ API short aliases for the C API fixed-width scalar types.
* Same types as Int16, UInt16, Int32, UInt32,
* Int64, UInt64, Float, Double and Size.
*
*/
    typedef OCI_Int16 Int16;
    typedef OCI_UInt16 UInt16;
    typedef OCI_Int32 Int32;
    typedef OCI_UInt32 UInt32;
    typedef OCI_Int64 Int64;
    typedef OCI_UInt64 UInt64;
    typedef OCI_Float Float;
    typedef OCI_Double Double;
    typedef OCI_Size Size;

/* OCI_Environment* GetEnvironmentHandle(); */

/* class forward declarations */

    class Exception;
    class Connection;
    class Transaction;
    class Environment;
    class Statement;
    class Resultset;
    class Date;
    class Timestamp;
    class Interval;
    class Number;
    class TypeInfo;
    class Reference;
    class XmType;
    class Object;
    template<class>
    class Element;
    template<class>
    class Iterator;
    template<class>
    class Collection;
    template<class, Int32>
    class Lob;
    class File;
    class Pool;
    template<class, Int32>
    class Long;
    class Column;
    class Subscription;
    class Event;
    class Agent;
    class Message;
    class Enqueue;
    class Dequeue;
    class Queue;
    class QueueTable;
    class DirectPath;
    class BindInfo;
    class SodaDatabase;
    class SodaCollection;
    class SodaDocument;
    class SodaOptions;
    class SodaCollectionCursor;
    class SodaDocumentCursor;
    class ShardingKey;
    class Row;
    class RowIterator;
    class ResultsetRows;
    template<class T>
    class ColumnIterator;
    template<class T>
    class ColumnView;

    using ResultsetColumns = ColumnView<Resultset>;
    using TypeInfoColumns = ColumnView<TypeInfo>;

/**
* Catch silently any exception thrown in the given expression
* Used for compliance with the standard (copy constructor, assignment operator, destructor)
*
*/
#define SILENT_CATCH(exp) try { exp; } catch (...) {}

}
