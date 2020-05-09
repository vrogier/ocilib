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

/*
 * IMPORTANT NOTICE
 *
 * This C++ header defines C++ wrapper classes around the OCILIB C API
 * It requires a compatible version of OCILIB
 *
 */

//
// ReSharper inspection rules customization (mainly for C98 backward compatibility)
//
// ReSharper disable CppClassCanBeFinal
// ReSharper disable CppClangTidyCppcoreguidelinesMacroUsage
// ReSharper disable CppClangTidyHicppSpecialMemberFunctions
// ReSharper disable CppClangTidyHicppUseEqualsDefault
// ReSharper disable CppClangTidyCppcoreguidelinesSpecialMemberFunctions
// ReSharper disable CppClangTidyModernizePassByValue
// ReSharper disable CppClangTidyModernizeUseEqualsDefault
//

#pragma once

#include <map>


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

#endif

#ifdef OCILIBPP_DEFINE_CXX_KEYWORDS

    #define nullptr 0
    #define override
    #define noexcept

#endif

namespace ocilib
{
#ifdef OCILIBPP_HAS_ENABLEIF

    template<bool B, class T = void>
    using EnableIf  =  std::enable_if<B, T>;

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
* @brief Internal usage.
* Allow resolving a native type used by C API from a C++ type in binding operations
*/
template<class T> struct BindResolver {};

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
ostring MakeString(const otext *result, int size = -1);

/**
* @brief Internal usage.
* Constructs a C++ Raw object from the given OCILIB raw buffer
*/
Raw MakeRaw(AnyPointer result, unsigned int size);

/**
 * @brief
 * Template class providing OCILIB handles auto memory, life cycle and scope management
 */
template<class>
class HandleHolder;

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
    operator unsigned int () const;

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

template< typename T>
class ManagedBuffer
{
public:
    ManagedBuffer();
    ManagedBuffer(size_t size);
    ManagedBuffer(T *buffer, size_t size);

    ~ManagedBuffer() noexcept;

    operator T* ();

private:

    T* _buffer;
    size_t _size;
};

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

class Lockable
{
public:

    Lockable();
    virtual  ~Lockable() noexcept;

    void SetLocker(Locker *locker);

    void Lock() const;
    void Unlock() const;

private:

    Locker *_locker;
};

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
    bool Exists(const T &value);

    template<class P>
    bool FindIf(P predicate, T &value);

    template<class A>
    void ForEach(A action);

private:

    std::list<T> _list;
};

class Handle
{
public:

    virtual ~Handle() noexcept {}
    virtual ConcurrentList<Handle *> & GetChildren() = 0;
    virtual void DetachFromHolders() = 0;
    virtual void DetachFromParent() = 0;
};

/**
* @brief
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

    HandleHolder(const HandleHolder &other);
	HandleHolder();
    ~HandleHolder() noexcept;

    HandleHolder& operator= (const HandleHolder& other) noexcept;

    typedef boolean(OCI_API *HandleFreeFunc)(AnyPointer handle);

    typedef void(*SmartHandleFreeNotifyFunc)(SmartHandle *smartHandle);

    Handle* GetHandle() const;

    void Acquire(T handle, HandleFreeFunc handleFreefunc, SmartHandleFreeNotifyFunc freeNotifyFunc, Handle *parent);
    void Acquire(HandleHolder &other);
    void Release();

    class SmartHandle : public Handle
    {
    public:

        SmartHandle(HandleHolder *holder, T handle, HandleFreeFunc handleFreefunc, SmartHandleFreeNotifyFunc freeNotifyFunc, Handle *parent);
        virtual ~SmartHandle() noexcept;

        void Acquire(HandleHolder *holder);
        void Release(HandleHolder *holder);

        void Destroy();

        T GetHandle() const;

        Handle *GetParent() const;

        AnyPointer GetExtraInfos() const;
        void  SetExtraInfos(AnyPointer extraInfo);

        ConcurrentList<Handle *> & GetChildren() override;
        void DetachFromHolders() override;
        void DetachFromParent() override;

    private:

        static void DeleteHandle(Handle *handle);
        static void ResetHolder(HandleHolder *holder);

        ConcurrentList<HandleHolder *> _holders;
        ConcurrentList<Handle *>  _children;

        Locker _locker;

        T _handle;
        HandleFreeFunc _handleFreeFunc;
        SmartHandleFreeNotifyFunc _freeNotifyFunc;
        Handle *_parent;
        AnyPointer _extraInfo;
    };

    SmartHandle *_smartHandle;
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
    friend T& operator << (T &lhs, const Streamable &rhs)
    {
        lhs << static_cast<ostring>(rhs);
        return lhs;
    }
};

class BindObject
{
public:

    BindObject(const Statement &statement, const ostring& name, unsigned int mode);

    virtual ~BindObject() noexcept;

    ostring GetName() const;

    Statement GetStatement() const;

    unsigned int GetMode() const;

    virtual void SetInData()  = 0;
    virtual void SetOutData() = 0;

protected:

    const Statement& _statement;
    ostring _name;
    unsigned int _mode;
};

class BindArray : public BindObject
{
public:

     BindArray(const Statement &statement, const ostring& name, unsigned int mode);
     virtual ~BindArray() noexcept;

     template<class T>
     void SetVector(std::vector<T> & vector, bool isPlSqlTable, unsigned int elemSize);
  
     template<class T>
     typename BindResolver<T>::OutputType * GetData() const;

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

        BindArrayObject(const Statement &statement, const ostring &name, ObjectVector &vector, bool isPlSqlTable, unsigned int mode, unsigned int elemSize);
        virtual ~BindArrayObject() noexcept;
        void SetInData() override;
        void SetOutData() override;
        ostring GetName()const  override;
        bool IsHandleObject() const override;
        unsigned int GetSize() const override;
        unsigned int GetSizeForBindCall() const override;

        operator ObjectVector & () const;
        operator NativeType * () const;

    private:

        void AllocData();
        void FreeData() const;

        const Statement& _statement;
        ostring _name;
        ObjectVector& _vector;
        NativeType *_data;
        bool _isPlSqlTable;
        unsigned int _mode;
        unsigned int _elemCount;
        unsigned int _elemSize;
    };

    AbstractBindArrayObject * _object;
};

template<class T>
class BindObjectAdaptor : public BindObject
{
    friend class Statement;

public:

    typedef T ObjectType;
    typedef typename BindResolver<ObjectType>::OutputType NativeType;

    operator NativeType *()  const;

    void SetInData() override;
    void SetOutData() override;

    BindObjectAdaptor(const Statement &statement, const ostring& name, unsigned int mode, ObjectType &object, unsigned int size);
    virtual ~BindObjectAdaptor() noexcept;

private:

    ObjectType&    _object;
    NativeType*    _data;
    unsigned int   _size;
};

template<class T>
class BindTypeAdaptor : public BindObject
{
    friend class Statement;

public:

    typedef T ObjectType;
    typedef typename BindResolver<ObjectType>::OutputType NativeType;

    operator NativeType *()  const;

    void SetInData() override;
    void SetOutData() override;

    BindTypeAdaptor(const Statement &statement, const ostring& name, unsigned int mode, ObjectType &object);
    virtual ~BindTypeAdaptor() noexcept;

private:

    ObjectType& _object;
    NativeType* _data;
};

class BindsHolder
{
public:

    BindsHolder(const Statement &statement);
    ~BindsHolder() noexcept;

    void Clear();

    void AddBindObject(BindObject *bindObject);

    void SetOutData();
    void SetInData();

private:

    std::vector<BindObject *> _bindObjects;
    const Statement& _statement;
};

}
