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

            BindObject(const ocilib::Statement& statement, const ostring& name, unsigned int mode);

            virtual ~BindObject() noexcept;

            ostring GetName() const;

            ocilib::Statement GetStatement() const;

            unsigned int GetMode() const;

            virtual void SetInData() = 0;
            virtual void SetOutData() = 0;

        protected:

            const ocilib::Statement& _statement;
            ostring _name;
            unsigned int _mode;
        };

       /**
        * @brief Internal usage.
        * Class implementing bind translations between C++ vectors and C API arrays
        */ 
        class BindArray : public BindObject
        {
        public:

            BindArray(const ocilib::Statement& statement, const ostring& name, unsigned int mode);
            virtual ~BindArray() noexcept;

            template<class T>
            void SetVector(std::vector<T>& vector, bool isPlSqlTable, unsigned int elemSize);

            template<class T>
            typename BindResolver<T>::OutputType* GetData() const;

            void SetInData() override;
            void SetOutData() override;

            unsigned int GetSize() const;
            unsigned int GetSizeForBindCall() const;

        private:

            class AbstractBindArrayObject
            {
            public:
                virtual ~AbstractBindArrayObject() {};
                virtual void SetInData() = 0;
                virtual void SetOutData() = 0;
                virtual ostring GetName() const = 0;
                virtual bool IsHandleObject() const = 0;
                virtual unsigned int GetSize() const = 0;
                virtual unsigned int GetSizeForBindCall() const = 0;
            };

            template<class T>
            class BindArrayObject : public  AbstractBindArrayObject
            {
            public:

                typedef T ObjectType;
                typedef std::vector<ObjectType> ObjectVector;
                typedef typename BindResolver<ObjectType>::OutputType NativeType;

                BindArrayObject(const ocilib::Statement& statement, const ostring& name, ObjectVector& vector, bool isPlSqlTable, unsigned int mode, unsigned int elemSize);
                virtual ~BindArrayObject() noexcept;
                void SetInData() override;
                void SetOutData() override;
                ostring GetName()const  override;
                bool IsHandleObject() const override;
                unsigned int GetSize() const override;
                unsigned int GetSizeForBindCall() const override;

                operator ObjectVector& () const;
                operator NativeType* () const;

            private:

                void AllocData();
                void FreeData() const;

                const ocilib::Statement& _statement;
                ostring _name;
                ObjectVector& _vector;
                NativeType* _data;
                bool _isPlSqlTable;
                unsigned int _mode;
                unsigned int _elemCount;
                unsigned int _elemSize;
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
            friend class ocilib::Statement;

        public:

            typedef T ObjectType;
            typedef typename BindResolver<ObjectType>::OutputType NativeType;

            operator NativeType* ()  const;

            void SetInData() override;
            void SetOutData() override;

            BindObjectAdaptor(const ocilib::Statement& statement, const ostring& name, unsigned int mode, ObjectType& object, unsigned int size);
            virtual ~BindObjectAdaptor() noexcept;

        private:

            ObjectType& _object;
            NativeType* _data;
            unsigned int   _size;
        };
        
       /**
        * @brief Internal usage.
        * Class implementing bind adapters between C++ types and C API types when C++ types do not exist in C 
        */ 
        template<class T>
        class BindTypeAdaptor : public BindObject
        {
            friend class ocilib::Statement;

        public:

            typedef T ObjectType;
            typedef typename BindResolver<ObjectType>::OutputType NativeType;

            operator NativeType* ()  const;

            void SetInData() override;
            void SetOutData() override;

            BindTypeAdaptor(const Statement& statement, const ostring& name, unsigned int mode, ObjectType& object);
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

            BindsHolder(const ocilib::Statement& statement);
            ~BindsHolder() noexcept;

            void Clear();

            void AddBindObject(BindObject* bindObject);

            void SetOutData();
            void SetInData();

        private:

            std::vector<BindObject*> _bindObjects;
            const ocilib::Statement& _statement;
        };
    }
}
