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

#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>

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
        String MakeString(const otext* result, Int32 size = -1);

        /**
        * @brief Internal usage.
        * Constructs a C++ Raw object from the given OCILIB raw buffer
        */
        Raw MakeRaw(AnyPointer result, UInt32 size);

        /**
         * @brief Internal usage.
         * Determine if the given type is a supported numeric type
         */
        template<class T>
        struct SupportedNumeric
        {
            static constexpr bool value = std::disjunction_v<
                std::is_same<T, Int16>,
                std::is_same<T, UInt16>,
                std::is_same<T, Int32>,
                std::is_same<T, UInt32>,
                std::is_same<T, Int64>,
                std::is_same<T, UInt64>,
                std::is_same<T, Float>,
                std::is_same<T, Double>,
                std::is_same<T, Number>
            >;
        };

        template<class T>
        using SupportedNumericT = std::enable_if_t<SupportedNumeric<T>::value>;

        /**
         * @brief Internal usage.
         * Determine if the given type is a supported numeric type
         */
        template<class T>
        struct SupportedVectorNumeric
        {
            static constexpr bool value = std::disjunction_v<
                std::is_same<T, unsigned char>,
                std::is_same<T, char>,
                std::is_same<T, Float>,
                std::is_same<T, Double>
            >;
        };

        template<class T>
        using SupportedVectorNumericT = std::enable_if_t<SupportedVectorNumeric<T>::value>;

        /**
         * @brief Internal usage.
         * Determine if the given type is an ocilib::Collection<> instantiation
         */
        template<class T>
        struct IsCollection : std::false_type {};

        template<class T>
        struct IsCollection<Collection<T>> : std::true_type {};

        template<class T>
        inline constexpr bool IsCollectionV = IsCollection<T>::value;

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

            T GetValue() const;

            operator T () const;
            explicit operator UInt32() const;

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

            UInt32 GetValues() const;

            bool IsSet(T other) const;

        private:

            Flags(UInt32 flags);

            UInt32 _flags;
        };

        /* Forward declarations */
        class HandleControlBase;
        template<class T> class HandleControl;
        class HandleStore;
        template<class T> class HandleHolder;
        template<class D, class T, class V> class IndexedIterator;

        using Handle = HandleControlBase;

        /**
         * @brief Internal usage.
         * Base class for C API handle control blocks (parent/child tree, shared ownership)
         */
        class HandleControlBase : public std::enable_shared_from_this<HandleControlBase>
        {
        public:

            using FreeNotifyFunc = void(*)(HandleControlBase*);

            HandleControlBase(std::weak_ptr<HandleStore> store = std::weak_ptr<HandleStore>(), FreeNotifyFunc freeNotify = nullptr, AnyPointer extraInfo = nullptr);

            virtual ~HandleControlBase() = default;

            virtual void Free() = 0;
            virtual OCI_Environment* GetEnvironment() const = 0;

            AnyPointer GetExtraInfos() const { return _extraInfo; }
            void SetExtraInfos(AnyPointer extraInfo) { _extraInfo = extraInfo; }

            void AddChild(std::shared_ptr<HandleControlBase> child);
            void RemoveChild(HandleControlBase* child);
            void FreeChildren();
            void FreeChildren(const std::function<bool(HandleControlBase*)>& predicate);

            HandleControlBase* GetParent() const;
            std::shared_ptr<HandleStore> GetStore() const;

        protected:

            void SetParent(HandleControlBase* parent);

            std::unordered_map<HandleControlBase*, std::weak_ptr<HandleControlBase>> _children;
            std::mutex _childrenMutex;
            std::weak_ptr<HandleControlBase> _parent;
            std::weak_ptr<HandleStore> _store;
            AnyPointer _extraInfo = nullptr;
            FreeNotifyFunc _freeNotify = nullptr;
        };

        /**
         * @brief Internal usage.
         * Typed C API handle control block
         */
        template<class T>
        class HandleControl : public HandleControlBase
        {
        public:

            HandleControl(T handle, bool allocated, std::weak_ptr<HandleStore> store, FreeNotifyFunc freeNotify = nullptr, AnyPointer extraInfo = nullptr);
            ~HandleControl() noexcept override;

            void Free() override;
            T GetHandle() const;
            OCI_Environment* GetEnvironment() const override;

        private:

            std::atomic<T> _handle;
            bool _allocated;
        };

        /**
        * @brief Internal usage.
         * Provide a store for C Handles to C++ Handles mapping
         */
        class HandleStore
        {
        public:

            HandleStore();

            template <class T>
            std::shared_ptr<HandleControl<T>> Find(AnyPointer handle);

            void Register(AnyPointer handle, std::shared_ptr<HandleControlBase> ctrl);
            void Remove(AnyPointer handle, const HandleControlBase* ctrl);

            static std::shared_ptr<HandleStore> ResolveStore(Handle* parent, AnyPointer handle);

        private:

            mutable std::mutex _mutex;
            std::unordered_map<AnyPointer, std::weak_ptr<HandleControlBase>> _map;
        };

        /**
        * @brief Internal usage.
        * Smart pointer class with shared ownership for managing OCILIB object handles
        */
        template<class T>
        class HandleHolder
        {
        public:

            HandleHolder() = default;
            HandleHolder(const HandleHolder& other) = default;
            HandleHolder& operator=(const HandleHolder& other) = default;
            HandleHolder(HandleHolder&& other) noexcept = default;
            HandleHolder& operator=(HandleHolder&& other) noexcept = default;

            ~HandleHolder() = default;

            bool IsNull() const;

            explicit operator bool();
            explicit operator bool() const;

            operator T();
            operator T() const;

            OCI_Environment* GetEnvironment() const;

            Handle* GetHandle() const;
            HandleControlBase* GetHandleControl() const;
            HandleControlBase* GetParent() const;

        protected:

            using FreeNotifyFunc = HandleControlBase::FreeNotifyFunc;

            void AcquireAllocated(T handle, Handle* parent);
            void AcquireTransient(T handle, Handle* parent);
            void AcquireAllocatedWithNotification(T handle, Handle* parent, FreeNotifyFunc freeNotify);
            void Acquire(HandleHolder& other);

            void Acquire(T handle, bool allocated, Handle* parent, std::shared_ptr<HandleStore> store, FreeNotifyFunc freeNotify = nullptr, AnyPointer extraInfo = nullptr);
            void Release();

        private:

            std::shared_ptr<HandleControl<T>> _control;
        };

        /**
        * @brief
        * Abstract class allowing derived classes to be compatible
        * with any type supporting the operator << String
        */
        class Streamable
        {
        public:

            virtual ~Streamable() noexcept {}

            operator String() const
            {
                return ToString();
            }

            virtual String ToString() const = 0;

            template<class T>
            friend T& operator << (T& lhs, const Streamable& rhs)
            {
                lhs << static_cast<String>(rhs);
                return lhs;
            }
        };
    }
}
