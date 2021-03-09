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

#pragma once

#include <list>
#include <map>

#include "ocilibcpp/config.hpp"

// ReSharper disable CppClangTidyCppcoreguidelinesMacroUsage
// ReSharper disable CppClangTidyHicppSpecialMemberFunctions
// ReSharper disable CppClangTidyCppcoreguidelinesSpecialMemberFunctions
// ReSharper disable CppClangTidyModernizeUseNodiscard
// ReSharper disable CppClangTidyHicppUseEqualsDefault
// ReSharper disable CppClangTidyModernizeUseEqualsDefault

namespace ocilib
{
    /**
     * @namespace ocilib::core
     * @brief OCILIB internal core classes 
     *
     */
     
    namespace core
    {
#ifdef OCILIBPP_HAS_ENABLEIF

        template<bool B, class T = void>
        using EnableIf = std::enable_if<B, T>;

        template<class T, class U>
        using IsSame = std::is_same<T, U>;

#else

        template<bool B, class T = void>
        struct EnableIf {};

        template<class T>
        struct EnableIf<true, T> { typedef T type; };

        template<bool B>
        struct BoolConstant { static const bool value = B; };

        template<class T, class U>
        struct IsSame : BoolConstant<false> {};

        template<class T>
        struct IsSame<T, T> : BoolConstant<true> {};

#endif

#define ARG_NOT_USED(a) (a) = (a)

        /**
         * @brief Internal usage.
         * Checks if the last OCILIB function call has raised an error.
         * If so, it raises a C++ exception using the retrieved error handle
         */
        template<class T>
        static T Check(T result);

        /**
         * @brief Internal usage.
         * Constructs a C++ string object from the given OCILIB string pointer
         */
        ostring MakeString(const otext* result, int size = -1);

        /**
        * @brief Internal usage.
        * Constructs a C++ Raw object from the given OCILIB raw buffer
        */
        Raw MakeRaw(AnyPointer result, unsigned int size);

       /**
        * @brief Internal usage.
        * Determine if the given type is a supported numeric type
        */   
        template<class T>
        struct SupportedNumeric
        {
            typedef EnableIf<IsSame<T, short>::value ||
                IsSame<T, unsigned short>::value ||
                IsSame<T, int>::value ||
                IsSame<T, unsigned int>::value ||
                IsSame<T, big_int>::value ||
                IsSame<T, big_uint>::value ||
                IsSame<T, float>::value ||
                IsSame<T, double>::value ||
                IsSame<T, Number>::value> Type;
        };

        /**
         * @brief
         * Template Enumeration template class providing some type safety to some extends for manipulating enumerated variables
         */
        template<class T>
        class Enum
        {
        public:

            typedef T Type;

            Enum();
            Enum(T value);

            T GetValue();

            operator T ();
            operator unsigned int() const;

            bool operator == (const Enum& other) const;
            bool operator != (const Enum& other) const;

            bool operator == (const T& other) const;
            bool operator != (const T& other) const;

        private:

            T _value;
        };

        /**
         * @brief
         * Template Flags template class providing some type safety to some extends for manipulating flags set variables
         */
        template<class T>
        class Flags
        {
        public:

            typedef T Type;

            Flags();
            Flags(T flag);
            Flags(const Flags& other);

            Flags& operator = (const Flags& other) noexcept;

            Flags operator~ () const;

            Flags operator | (T other) const;
            Flags operator & (T other) const;
            Flags operator ^ (T other) const;

            Flags operator | (const Flags& other) const;
            Flags operator & (const Flags& other) const;
            Flags operator ^ (const Flags& other) const;

            Flags& operator |= (T other);
            Flags& operator &= (T other);
            Flags& operator ^= (T other);

            Flags& operator |= (const Flags& other);
            Flags& operator &= (const Flags& other);
            Flags& operator ^= (const Flags& other);

            bool operator == (T other) const;
            bool operator == (const Flags& other) const;

            unsigned int GetValues() const;

            bool IsSet(T other) const;

        private:

            Flags(unsigned int flags);

            unsigned int _flags;
        };

       /**
        * @brief Internal usage.
        * Provide a buffer class with RAII capabilities
        */  
        template< typename T>
        class ManagedBuffer
        {
        public:
            ManagedBuffer();
            ManagedBuffer(size_t size);
            ManagedBuffer(T* buffer, size_t size);

            ~ManagedBuffer() noexcept;

            operator T* ();

        private:

            T* _buffer;
            size_t _size;
        };

       /**
        * @brief Internal usage.
        * Locker object
        */ 
        class Locker
        {
        public:

            Locker();
            virtual ~Locker() noexcept;

            void Lock() const;
            void Unlock() const;

            void SetAccessMode(bool threaded);

        private:

            MutexHandle _mutex;
        };

       /**
        * @brief Internal usage.
        * Base class for types that can be locked
        */ 
        class Lockable
        {
        public:

            Lockable();
            virtual  ~Lockable() noexcept;

            void SetLocker(Locker* locker);

            void Lock() const;
            void Unlock() const;

        private:

            Locker* _locker;
        };

       /**
        * @brief Internal usage.
        * Map supporting concurrent access from multiple threads
        */ 
        template<class K, class V>
        class ConcurrentMap : public Lockable
        {
        public:

            ConcurrentMap();
            virtual ~ConcurrentMap() noexcept;

            void Remove(K key);
            V Get(K key);
            void Set(K key, V value);
            void Clear();
            size_t GetSize();

        private:

            std::map<K, V> _map;

        };

       /**
        * @brief Internal usage.
        * List supporting concurrent access from multiple threads
        */ 
        template<class T>
        class ConcurrentList : public Lockable
        {
        public:

            ConcurrentList();
            virtual ~ConcurrentList() noexcept;

            void Add(T value);
            void Remove(T value);
            void Clear();
            size_t GetSize();
            bool Exists(const T& value);

            template<class P>
            bool FindIf(P predicate, T& value);

            template<class A>
            void ForEach(A action);

        private:

            std::list<T> _list;
        };

       /**
        * @brief Internal usage.
        * Interface for handling ownership and relationship of a C API handle
        */ 
        class Handle
        {
        public:

            virtual ~Handle() noexcept {}
            virtual ConcurrentList<Handle*>& GetChildren() = 0;
            virtual void DetachFromHolders() = 0;
            virtual void DetachFromParent() = 0;
        };

        /**
        * @brief Internal usage.
        * Smart pointer class with reference counting for managing OCILIB object handles
        */
        template<class T>
        class HandleHolder
        {
        public:

            bool IsNull() const;

            operator bool();
            operator bool() const;

            operator T();
            operator T() const;

        protected:

            class SmartHandle;

            HandleHolder(const HandleHolder& other);
            HandleHolder();
            ~HandleHolder() noexcept;

            HandleHolder& operator= (const HandleHolder& other) noexcept;

            typedef boolean(OCI_API* HandleFreeFunc)(AnyPointer handle);

            typedef void(*SmartHandleFreeNotifyFunc)(SmartHandle* smartHandle);

            Handle* GetHandle() const;

            void Acquire(T handle, HandleFreeFunc handleFreefunc, SmartHandleFreeNotifyFunc freeNotifyFunc, Handle* parent);
            void Acquire(HandleHolder& other);
            void Release();

            class SmartHandle : public Handle
            {
            public:

                SmartHandle(HandleHolder* holder, T handle, HandleFreeFunc handleFreefunc, SmartHandleFreeNotifyFunc freeNotifyFunc, Handle* parent);
                virtual ~SmartHandle() noexcept;

                void Acquire(HandleHolder* holder);
                void Release(HandleHolder* holder);

                void Destroy();

                T GetHandle() const;

                Handle* GetParent() const;

                AnyPointer GetExtraInfos() const;
                void  SetExtraInfos(AnyPointer extraInfo);

                ConcurrentList<Handle*>& GetChildren() override;
                void DetachFromHolders() override;
                void DetachFromParent() override;

            private:

                static void DeleteHandle(Handle* handle);
                static void ResetHolder(HandleHolder* holder);

                ConcurrentList<HandleHolder*> _holders;
                ConcurrentList<Handle*>  _children;

                Locker _locker;

                T _handle;
                HandleFreeFunc _handleFreeFunc;
                SmartHandleFreeNotifyFunc _freeNotifyFunc;
                Handle* _parent;
                AnyPointer _extraInfo;
            };

            SmartHandle* _smartHandle;
        };

        /**
        * @brief
        * Abstract class allowing derived classes to be compatible
        * with any type supporting the operator << ocilib::ostring
        */
        class Streamable
        {
        public:

            virtual ~Streamable() noexcept {}

            operator ostring() const
            {
                return ToString();
            }

            virtual ostring ToString() const = 0;

            template<class T>
            friend T& operator << (T& lhs, const Streamable& rhs)
            {
                lhs << static_cast<ostring>(rhs);
                return lhs;
            }
        };

    }
}