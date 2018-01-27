/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2018 Vincent ROGIER <vince.rogier@ocilib.net>
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

#pragma once

#include <map>

namespace ocilib
{

/* Try to guess C++ Compiler capabilities ... */

#define CPP_98 199711L
#define CPP_11 201103L
#define CPP_14 201402L

#if __cplusplus < CPP_11
    #if defined(__GNUC__)
        #if defined(__GXX_EXPERIMENTAL_CXX0X__)
            #define HAVE_NULLPTR
            #define HAVE_MOVE_SEMANTICS
        #else
            #define override
        #endif
    #elif defined(_MSC_VER)
        #if _MSC_VER >= 1600
            #define HAVE_NULLPTR
            #define HAVE_MOVE_SEMANTICS
        #else
            #define override
        #endif
    #endif
#else
    #define HAVE_NULLPTR
    #define HAVE_MOVE_SEMANTICS
#endif


/* guessing if nullptr is supported */

#ifndef HAVE_NULLPTR
    #define nullptr 0
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
Raw MakeRaw(void *result, unsigned int size);

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

    ~ManagedBuffer();

    operator T* () const;
    operator const T* () const;

private:

    T* _buffer;
    size_t _size;
};

class Locker
{
public:

    Locker();
    virtual ~Locker();

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
    virtual  ~Lockable();

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
    virtual ~ConcurrentMap();

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
    virtual ~ConcurrentList();

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

    virtual ~Handle() {}
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
    ~HandleHolder();

    HandleHolder& operator= (const HandleHolder &other);

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
        virtual ~SmartHandle();

        void Acquire(HandleHolder *holder);
        void Release(HandleHolder *holder);

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

    virtual ~Streamable() {}

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

    virtual ~BindObject();

    ostring GetName() const;

    Statement GetStatement() const;

    unsigned int GetMode() const;

    virtual void SetInData()  = 0;
    virtual void SetOutData() = 0;

protected:

    OCI_Statement *_pStatement;
    ostring _name;
    unsigned int _mode;
};

class BindArray : public BindObject
{
public:

     BindArray(const Statement &statement, const ostring& name, unsigned int mode);
     virtual ~BindArray();

     template<class T>
     void SetVector(std::vector<T> & vector, unsigned int elemSize);

     template<class T>
     typename BindResolver<T>::OutputType * GetData() const;

     void SetInData() override;
     void SetOutData() override;

private:

    class AbstractBindArrayObject
    {
    public:
        AbstractBindArrayObject()  { }
        virtual ~AbstractBindArrayObject()  { }
        virtual void SetInData() = 0;
        virtual void SetOutData() = 0;
        virtual ostring GetName() = 0;
    };

    template<class T>
    class BindArrayObject : public  AbstractBindArrayObject
    {
    public:

		typedef T ObjectType;
		typedef std::vector<ObjectType> ObjectVector;
        typedef typename BindResolver<ObjectType>::OutputType NativeType;

        BindArrayObject(const Statement &statement, const ostring& name, ObjectVector &vector, unsigned int mode, unsigned int elemSize);
        virtual ~BindArrayObject();
        void SetInData() override;
        void SetOutData() override;
        ostring GetName() override;

        operator ObjectVector & () const;
        operator NativeType * () const;

    private:

        void AllocData();
        void FreeData() const;

        OCI_Statement *_pStatement;
        ostring _name;
        ObjectVector& _vector;
        NativeType *_data;
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
    virtual ~BindObjectAdaptor();

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
    virtual ~BindTypeAdaptor();

private:

    ObjectType& _object;
    NativeType* _data;
};

class BindsHolder
{
public:

    BindsHolder(const Statement &statement);
    ~BindsHolder();

    void Clear();

    void AddBindObject(BindObject *bindObject);

    void SetOutData();
    void SetInData();

private:

    std::vector<BindObject *> _bindObjects;
    OCI_Statement * _pStatement;
};

}
