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

#include <string>
#include <vector>

// ReSharper disable CppClangTidyCppcoreguidelinesMacroUsage

/* Including C API declarations  */

extern "C" {
#include "ocilib.h"
}

/* Try to guess C++ Compiler capabilities ... */

#define OCILIBPP_CPP_98 199711L
#define OCILIBPP_CPP_11 201103L
#define OCILIBPP_CPP_14 201402L

#if __cplusplus < OCILIBPP_CPP_11

    // GCC
#if defined(__GNUC__)
#if defined(__GXX_EXPERIMENTAL_CXX0X__)
#define OCILIBPP_HAS_CXX
#endif
// MSVC
#elif defined(_MSC_VER)
#if _MSVC_LANG >= OCILIBPP_CPP_11
#define OCILIBPP_HAS_CXX
#endif
#endif

#else

#define OCILIBPP_HAS_CXX

#endif

#ifdef OCILIBPP_HAS_CXX

#define OCILIBPP_HAS_ENABLEIF
#define OCILIBPP_HAS_VARIADIC

#ifdef  OCILIBCPP_DEBUG_MEMORY

#include <iostream>
#define OCILIBPP_DEBUG_MEMORY_ENABLED

#endif

#else

#define OCILIBPP_DEFINE_CXX_KEYWORDS

#endif

#ifdef OCILIBPP_TEST_CPP98

#ifdef OCILIBPP_DEFINE_CXX_KEYWORDS
#undef OCILIBPP_DEFINE_CXX_KEYWORDS
#endif

#ifdef OCILIBPP_HAS_ENABLEIF
#undef OCILIBPP_HAS_ENABLEIF
#endif

#ifdef OCILIBPP_HAS_VARIADIC
#undef OCILIBPP_HAS_VARIADIC
#endif

#ifdef OCILIBPP_DEBUG_MEMORY_ENABLED
#undef OCILIBPP_DEBUG_MEMORY_ENABLED
#endif

#endif

#ifdef OCILIBPP_DEFINE_CXX_KEYWORDS

#define nullptr 0
#define override
#define noexcept

#endif

namespace ocilib
{

/**
 * @typedef ocilib::ostring
 *
 * @brief
 * string class wrapping the OCILIB otext * type and OTEXT() macros ( see @ref OcilibCApiSupportedCharsets )
 *
 * @note
 * - for ANSI builds, ocilib::ostring is equivalent to std::string
 * - for UNICODE builds, ocilib::ostring is equivalent to std::wstring
 *
 */
typedef std::basic_string<otext, std::char_traits<otext>, std::allocator<otext> > ostring;

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
 * @typedef ocilib::MutexHandle
 *
 * @brief
 * Alias for an OCI_Mutex pointer
 *
 */
typedef OCI_Mutex *  MutexHandle;

/**
 * @typedef ocilib::ThreadHandle
 *
 * @brief
 * Alias for an OCI_Thread pointer
 *
 */
typedef OCI_Thread * ThreadHandle;

/**
* @typedef ocilib::ThreadId
*
* @brief
* Thread Unique ID
*
*/
typedef const void * ThreadId;

/**
 * @typedef ocilib::CallbackPointer
 *
 * @brief
 * Alias used for storing user callback method pointers
 *
 */
typedef void * CallbackPointer;

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
class Object;
template<class>
class Element;
template<class>
class Iterator;
template<class>
class Collection;
template<class, int>
class Lob;
class File;
class Pool;
template<class, int>
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
class Thread;
class ThreadKey;
class Mutex;
class BindInfo;


/**
* Catch silently any exception thrown in the given expression
* Used for compliance with the standard (copy constructor, assignment operator, destructor)
*
*/
#define SILENT_CATCH(exp) try { exp; } catch (...) {}

}
