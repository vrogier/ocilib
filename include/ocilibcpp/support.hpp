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

#include "ocilibcpp/core.hpp"

// ReSharper disable CppClangTidyModernizeUseNodiscard
// ReSharper disable CppClangTidyHicppUseEqualsDefault
// ReSharper disable CppClangTidyHicppSpecialMemberFunctions
// ReSharper disable CppClangTidyCppcoreguidelinesSpecialMemberFunctions
// ReSharper disable CppClangTidyModernizePassByValue

namespace ocilib
{
    /**
     * @namespace ocilib::support
     * @brief OCILIB internal support classes 
     *
     */
     
    namespace support
    {
        /**
        * @brief Internal usage.
        * Allow resolving a native type used by C API from a C++ type in binding operations
        */
        template<class T> struct BindResolver {};

       /**
        * @brief Internal usage.
        * Base class for implementing bind translations for C++ objects and their counter parts in the C API
        */ 
        class BindObject
        {
        public:

            BindObject(const Statement& statement, const String& name, UInt32 mode);

            virtual ~BindObject() noexcept;

            String GetName() const;

            Statement GetStatement() const;

            UInt32 GetMode() const;

            virtual void SetInData() = 0;
            virtual void SetOutData() = 0;

        protected:

            const Statement& _statement;
            String _name;
            UInt32 _mode;
        };

       /**
        * @brief Internal usage.
        * Class implementing bind translations between C++ vectors and C API arrays
        */ 
        class BindArray : public BindObject
        {
        public:

            BindArray(const Statement& statement, const String& name, UInt32 mode);
            virtual ~BindArray() noexcept;

            template<class T>
            void SetVector(std::vector<T>& vector, bool isPlSqlTable, UInt32 elemSize);

            template<class T>
            typename BindResolver<T>::OutputType* GetData() const;

            void SetInData() override;
            void SetOutData() override;

            UInt32 GetSize() const;
            UInt32 GetSizeForBindCall() const;

        private:

            class AbstractBindArrayObject
            {
            public:
                virtual ~AbstractBindArrayObject() {};
                virtual void SetInData() = 0;
                virtual void SetOutData() = 0;
                virtual String GetName() const = 0;
                virtual bool IsHandleObject() const = 0;
                virtual UInt32 GetSize() const = 0;
                virtual UInt32 GetSizeForBindCall() const = 0;
            };

            template<class T>
            class BindArrayObject : public  AbstractBindArrayObject
            {
            public:

                typedef T ObjectType;
                typedef std::vector<ObjectType> ObjectVector;
                typedef typename BindResolver<ObjectType>::OutputType NativeType;

                BindArrayObject(const Statement& statement, const String& name, ObjectVector& vector, bool isPlSqlTable, UInt32 mode, UInt32 elemSize);
                virtual ~BindArrayObject() noexcept;
                void SetInData() override;
                void SetOutData() override;
                String GetName()const  override;
                bool IsHandleObject() const override;
                UInt32 GetSize() const override;
                UInt32 GetSizeForBindCall() const override;

                operator ObjectVector& () const;
                operator NativeType* () const;

            private:

                void AllocData();
                void FreeData() const;

                const Statement& _statement;
                String _name;
                ObjectVector& _vector;
                NativeType* _data;
                bool _isPlSqlTable;
                UInt32 _mode;
                UInt32 _elemCount;
                UInt32 _elemSize;
            };

            AbstractBindArrayObject* _object;
        };

       /**
        * @brief Internal usage.
        * Class implementing bind adapters between C++ class and C API types
        */ 
        template<class T>
        class BindObjectAdaptor : public BindObject
        {
            friend class Statement;

        public:

            typedef T ObjectType;
            typedef typename BindResolver<ObjectType>::OutputType NativeType;

            operator NativeType* ()  const;

            void SetInData() override;
            void SetOutData() override;

            BindObjectAdaptor(const Statement& statement, const String& name, UInt32 mode, ObjectType& object, UInt32 size);
            virtual ~BindObjectAdaptor() noexcept;

        private:

            ObjectType& _object;
            NativeType* _data;
            UInt32   _size;
        };
        
       /**
        * @brief Internal usage.
        * Class implementing bind adapters between C++ types and C API types when C++ types do not exist in C 
        */ 
        template<class T>
        class BindTypeAdaptor : public BindObject
        {
            friend class Statement;

        public:

            typedef T ObjectType;
            typedef typename BindResolver<ObjectType>::OutputType NativeType;

            operator NativeType* ()  const;

            void SetInData() override;
            void SetOutData() override;

            BindTypeAdaptor(const Statement& statement, const String& name, UInt32 mode, ObjectType& object);
            virtual ~BindTypeAdaptor() noexcept;

        private:

            ObjectType& _object;
            NativeType* _data;
        };

       /**
        * @brief Internal usage.
        * Class owning bind objects allowing to set/get C data prior/after a statement execution
        */ 
        class BindsHolder
        {
        public:

            BindsHolder(const Statement& statement);
            ~BindsHolder() noexcept;

            void Clear();

            void AddBindObject(BindObject* bindObject);

            void SetOutData();
            void SetInData();

        private:

            std::vector<BindObject*> _bindObjects;
            const Statement& _statement;
        };
    }
}
