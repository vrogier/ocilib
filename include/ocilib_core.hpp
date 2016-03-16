/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2016 Vincent ROGIER <vince.rogier@ocilib.net>
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

namespace ocilib
{

#define ARG_NOT_USED(a) (a) = (a)

/** class pre declarations */

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
template <class TDataType>
class Collection;
template<class TLobObjectType, int TLobOracleType>
class Lob;
class File;
class Pool;
template<class TLongObjectType, int TLongOracleType>
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
 * Checks if the last OCILIB function call has raised an error.
 * If so, it raises a C++ exception using the retrieved error handle
 */
template<class TResultType>
static TResultType Check(TResultType result);

/**
 * @brief Internal usage.
 * Constructs a C++ string object from the given OCILIB string pointer
 */
ostring MakeString(const otext *result);

/**
* @brief Internal usage.
* Constructs a C++ Raw object from the given OCILIB raw buffer
*/
Raw MakeRaw(void *result, unsigned int size);

/**
 * @brief
 * Template class providing OCILIB handles auto memory, life cycle and scope management
 */
template <class THandleType>
class HandleHolder;

/**
 * @brief
 * Template Enum template class providing some type safety to some extends for manipulating enum variables
 */
template <class TEnum>
class Enum
{
public:

    typedef TEnum type;

    Enum();
    Enum(TEnum value);

    TEnum GetValue();

    operator TEnum ();
    operator unsigned int ();

    bool operator == (const Enum& other) const;
    bool operator != (const Enum& other) const;

    bool operator == (const TEnum& other) const;
    bool operator != (const TEnum& other) const;

private:

    TEnum _value;
};

/**
 * @brief
 * Template Flags template class providing some type safety to some extends for manipulating flags set variables
 */
template <class TEnum>
class Flags
{
public:

    typedef TEnum type;

    Flags();
    Flags(TEnum flag);
    Flags(const Flags& other);
    Flags operator~ () const;

    Flags operator | (TEnum other) const;
    Flags operator & (TEnum other) const;
    Flags operator ^ (TEnum other) const;

    Flags operator | (const Flags& other) const;
    Flags operator & (const Flags& other) const;
    Flags operator ^ (const Flags& other) const;

    Flags& operator |= (TEnum other);
    Flags& operator &= (TEnum other);
    Flags& operator ^= (TEnum other);

    Flags& operator |= (const Flags& other);
    Flags& operator &= (const Flags& other);
    Flags& operator ^= (const Flags& other);

    bool operator == (TEnum other) const;
    bool operator == (const Flags& other) const;

    unsigned int GetValues() const;

    bool IsSet(TEnum other) const;

private:

    Flags(unsigned int flags);

    unsigned int _flags;
};

template< typename TBufferType>
class ManagedBuffer
{
public:
    ManagedBuffer();
    ManagedBuffer(size_t size);
    ManagedBuffer(TBufferType *buffer, size_t size);

    ~ManagedBuffer();

    operator TBufferType* () const;
    operator const TBufferType* () const;

private:

    TBufferType* _buffer;
    size_t _size;
};

class Locker
{
public:

    Locker();
    virtual ~Locker();

    void Lock();
    void Unlock();

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

    void Lock();
    void Unlock();

private:

    Locker *_locker;
};

template <class TKey, class TValue>
class ConcurrentMap : public Lockable
{
public:

    ConcurrentMap();
    virtual ~ConcurrentMap();

    void Remove(TKey key);
    TValue Get(TKey key);
    void Set(TKey key, TValue value);
    void Clear();
    size_t GetSize();

private:

    std::map<TKey, TValue> _map;

};

template <class TValue>
class ConcurrentList : public Lockable
{
public:

    ConcurrentList();
    virtual ~ConcurrentList();

    void Add(TValue value);
    void Remove(TValue value);
    void Clear();
    size_t GetSize();
    bool Exists(TValue value);

    template<class TPredicate>
    bool FindIf(TPredicate predicate, TValue &value);

    template<class TAction>
    void ForEach(TAction action);

private:

    std::list<TValue> _list;
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
template<class THandleType>
class HandleHolder
{
public:

    bool IsNull() const;

    operator bool();
    operator bool() const;

    operator THandleType();
    operator THandleType() const;

protected:

    class SmartHandle;

    HandleHolder(const HandleHolder &other);
    HandleHolder();
    ~HandleHolder();

    HandleHolder& operator= (const HandleHolder &other);

    typedef boolean(OCI_API *HandleFreeFunc)(AnyPointer handle);
    
    typedef void(*SmartHandleFreeNotifyFunc)(SmartHandle *smartHandle);

    Handle* GetHandle() const;

    void Acquire(THandleType handle, HandleFreeFunc handleFreefunc, SmartHandleFreeNotifyFunc freeNotifyFunc, Handle *parent);
    void Acquire(HandleHolder &other);
    void Release();

    class SmartHandle : public Handle
    {
    public:

        SmartHandle(HandleHolder *holder, THandleType handle, HandleFreeFunc handleFreefunc, SmartHandleFreeNotifyFunc freeNotifyFunc, Handle *parent);
        virtual ~SmartHandle();

        void Acquire(HandleHolder *holder);
        void Release(HandleHolder *holder);

        const THandleType GetHandle() const;

        Handle *GetParent() const;

        AnyPointer GetExtraInfos() const;
        void  SetExtraInfos(AnyPointer extraInfo);

        ConcurrentList<Handle *> & GetChildren();
        void DetachFromHolders();
        void DetachFromParent();

    private:

        static void DeleteHandle(Handle *handle);
        static void ResetHolder(HandleHolder *holder);

        ConcurrentList<HandleHolder *> _holders;
        ConcurrentList<Handle *>  _children;

        Locker _locker;

        THandleType _handle;
        HandleFreeFunc _handleFreeFunc;
        SmartHandleFreeNotifyFunc _freeNotifyFunc;
        Handle *_parent;
        AnyPointer _extraInfo;
    };

protected:

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

    template <class TStream>
    friend TStream& operator << (TStream &lhs, const Streamable &rhs)
    {
        lhs << static_cast<ostring>(rhs);
        return lhs;
    }
};

template <class TValueType>
class BindValue
{
public:

    BindValue();
    BindValue(TValueType value);

    operator TValueType() const;

private:

    TValueType _value;
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

     template <class TObjectType, class TDataType>
     void SetVector(std::vector<TObjectType> & vector, unsigned int elemSize);

     template <class TObjectType, class TDataType>
     TDataType * GetData () const;

     void SetInData();
     void SetOutData();

private:

    class AbstractBindArrayObject
    {
    public:
        AbstractBindArrayObject()  { }
        virtual ~AbstractBindArrayObject()  { }
        virtual void SetInData() = 0;
        virtual void SetOutData() = 0;
        ostring GetName();
    };

    template <class TObjectType, class TDataType>
    class BindArrayObject : public  AbstractBindArrayObject
    {
    private:

        OCI_Statement *_pStatement;
        ostring _name;
        std::vector<TObjectType> & _vector;
        TDataType *_data;
        unsigned int _mode;
        unsigned int _elemCount;
        unsigned int _elemSize;

    public:

        BindArrayObject(const Statement &statement, const ostring& name, std::vector<TObjectType> &vector, unsigned int mode, unsigned int elemSize);
        virtual ~BindArrayObject();
        void SetInData();
        void SetOutData();
        ostring GetName();

        operator std::vector<TObjectType> & () const;
        operator TDataType * () const;

    private:

        void AllocData();
        void FreeData();
    };

    AbstractBindArrayObject * _object;
};

template <class TNativeType, class TObjectType>
class BindObjectAdaptor : public BindObject
{
    friend class Statement;

public:

    operator TNativeType *()  const;

    void SetInData();
    void SetOutData();

    BindObjectAdaptor(const Statement &statement, const ostring& name, unsigned int mode, TObjectType &object, unsigned int size);
    virtual ~BindObjectAdaptor();

private:

    TObjectType&    _object;
    TNativeType*    _data;
    unsigned int    _size;
};

template <class TNativeType, class TObjectType>
class BindTypeAdaptor : public BindObject
{
    friend class Statement;

public:

    operator TNativeType *()  const;

    void SetInData();
    void SetOutData();

    BindTypeAdaptor(const Statement &statement, const ostring& name, unsigned int mode, TObjectType &object);
    virtual ~BindTypeAdaptor();

private:

    TObjectType&    _object;
    TNativeType*    _data;
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
