/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2021 Vincent ROGIER <vince.rogier@ocilib.net>
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

/*
 * IMPORTANT NOTICE
 *
 * This C++ header defines C++ wrapper classes around the OCILIB C API
 * It requires a compatible version of OCILIB
 *
 */

#ifndef OCILIBCPP_H_INCLUDED
#define OCILIBCPP_H_INCLUDED

/**
 * @namespace ocilib
 * @brief OCILIB ++ Namespace
 *
 */
 
namespace ocilib
{

/**
 * @defgroup OcilibCppApi C++ API
 * @{
 */

/**
 * @defgroup OcilibCppApiOverview Overview
 * @{
 *
 * @par Introduction
 * OCILIB++ is a C++ API for Oracle built on top of the C OCILIB API:
 *  - Full C API ported to C++
 *  - Implemented as a set of header files, no library compilation needed
 *  - Based on C++ and STL paradigms (Strong typing, templates, containers, RAII, exception handling, operators, stack objects)
 *  - Based on design patterns (RAII, delegation, reference counting, smart pointers, proxies, singleton)
 *  - No user dynamic object allocation required
 *  - The only dependencies are : STL and OCILIB C API
 *
 * @par C++ language requirements
 * - The OCILIB C++ API requires only C++03 features.
 * - It does not required C++11/14/17 but uses some CXX features when target compiler support them 
 *
 * @par Reference counting model
 * - API usage is very simple, based on stack objects wrapping OCILIB handles using reference counting.
 * - OCILIB handles are automatically allocated internally by C++ objects constructors or methods.
 * - They are also automatically freed when the last C++ object referencing it goes out of scope.
 * - Dynamic memory allocation is not required at all.
 * - OCILIB++ allows simple and safe usage of Oracle client without the worries of memory leakages.
 * - Using stack objects also makes error handling easier and program logic more robust
 *
 * @par C++ API classes usage
 * Most C++ API classes wrap C API handles.
 * When instances are created using the default constructors, they hold no C handles and have
 * their method IsNull() returning true.
 * Any use of other methods and functions on such object methods will throw an C++ exception.
 * Thus, in order to have a valid object :
 * - use a parametrized constructor
 * - use the default constructor and then later assign a value to the instance using assignment operator.
 * In the later case, the value can be provided by a function return value or using the object class constructor
 *
 * @par Exception model
 * - Any failure occurring within an OCILIB C API call will throw a ocilib::Exception
 * - For conformance reasons, this class derives from std::Exception
 *
 * @warning
 *  - OCILIB++ wraps the whole OCILIB C API.
 *  - Each C OCILIB object handle has its C++ class counter part.
 *  - The whole OCILIB C Documentation (concepts, use cases, features) is still valid for OCILIB++
 *
 * @} OcilibCppApiOverview
 */
 
/**
 * @defgroup OcilibCppApiMainDemoApplication OCILIB main C++ demo application code
 * @{
 *
 * Main C++ demo  source
 * @include ocilib_demo.cpp
 *
 * @} OcilibCppApiMainDemoApplication
 */

 /**
  * @defgroup OcilibCppApiDemoListApplication Some OCILIB C++ sample codes
  * @{
  *
  * Here are some C++ samples code. More samples can be found under the demo folder of ocilib packages.
  *
  * @par Fetching data
  * @include fetch.cpp
  *
  * @par Binding vectors
  * @include array.cpp
  *
  * @par Using collections
  * @include coll.cpp
  *
  * @par Using connection pools
  * @include pool.cpp
  *
  * @par Oracle 12c Implicit resultsets
  * @include implicit_resultset.cpp
  *
  * @par Using Oracle objects
  * @include object.cpp
  *
  * @par Database notifications
  * @include notification.cpp
  *
  * @} OcilibCppApiDemoListApplication
  */

 /**
 *
 * @} OcilibCppApi
 */

}

/*
 * IMPORTANT NOTICE
 *
 * This C++ header defines C++ wrapper classes around the OCILIB C API
 * It requires a compatible version of OCILIB
 *
 */

/* Including declarations  */

#include "ocilibcpp/config.hpp"
#include "ocilibcpp/core.hpp"
#include "ocilibcpp/support.hpp"
#include "ocilibcpp/types.hpp"

/* Including core implementations  */

#include "ocilibcpp/detail/core/Utils.hpp"
#include "ocilibcpp/detail/core/Enum.hpp"
#include "ocilibcpp/detail/core/Flags.hpp"
#include "ocilibcpp/detail/core/ManagedBuffer.hpp"
#include "ocilibcpp/detail/core/HandleHolder.hpp"
#include "ocilibcpp/detail/core/Locker.hpp"
#include "ocilibcpp/detail/core/Lockable.hpp"
#include "ocilibcpp/detail/core/ConcurrentMap.hpp"
#include "ocilibcpp/detail/core/ConcurrentList.hpp"
#include "ocilibcpp/detail/core/SmartHandle.hpp"
#include "ocilibcpp/detail/core/MemoryDebugInfo.hpp"

/* Including support implementations  */

#include "ocilibcpp/detail/support/BindResolver.hpp"
#include "ocilibcpp/detail/support/BindObject.hpp"
#include "ocilibcpp/detail/support/BindArray.hpp"
#include "ocilibcpp/detail/support/BindObjectAdaptor.hpp"
#include "ocilibcpp/detail/support/BindTypeAdaptor.hpp"
#include "ocilibcpp/detail/support/BindsHolder.hpp"
#include "ocilibcpp/detail/support/NumericTypeResolver.hpp"

/* Including types implementations  */

#include "ocilibcpp/detail/Exception.hpp"
#include "ocilibcpp/detail/Environment.hpp"
#include "ocilibcpp/detail/Mutex.hpp"
#include "ocilibcpp/detail/Thread.hpp"
#include "ocilibcpp/detail/ThreadKey.hpp"
#include "ocilibcpp/detail/Pool.hpp"
#include "ocilibcpp/detail/Connection.hpp"
#include "ocilibcpp/detail/Transaction.hpp"
#include "ocilibcpp/detail/Number.hpp"
#include "ocilibcpp/detail/Date.hpp"
#include "ocilibcpp/detail/Interval.hpp"
#include "ocilibcpp/detail/Timestamp.hpp"
#include "ocilibcpp/detail/Lob.hpp"
#include "ocilibcpp/detail/File.hpp"
#include "ocilibcpp/detail/TypeInfo.hpp"
#include "ocilibcpp/detail/Object.hpp"
#include "ocilibcpp/detail/Reference.hpp"
#include "ocilibcpp/detail/Collection.hpp"
#include "ocilibcpp/detail/CollectionIterator.hpp"
#include "ocilibcpp/detail/CollectionElement.hpp"
#include "ocilibcpp/detail/Long.hpp"
#include "ocilibcpp/detail/BindInfo.hpp"
#include "ocilibcpp/detail/Statement.hpp"
#include "ocilibcpp/detail/Resultset.hpp"
#include "ocilibcpp/detail/Column.hpp"
#include "ocilibcpp/detail/Subscription.hpp"
#include "ocilibcpp/detail/Event.hpp"
#include "ocilibcpp/detail/Agent.hpp"
#include "ocilibcpp/detail/Message.hpp"
#include "ocilibcpp/detail/Enqueue.hpp"
#include "ocilibcpp/detail/Dequeue.hpp"
#include "ocilibcpp/detail/DirectPath.hpp"
#include "ocilibcpp/detail/Queue.hpp"
#include "ocilibcpp/detail/QueueTable.hpp"


#endif

