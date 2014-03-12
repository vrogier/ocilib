/*
    +-----------------------------------------------------------------------------------------+
    |                                                                                         |
    |                                                                                         |
    |                          OCILIB ++ - C++ wrapper around OCILIB                          |
    |                                                                                         |
    |                                (C Wrapper for Oracle OCI)                               |
    |                                                                                         |
    |                              Website : http://www.ocilib.net                            |
    |                                                                                         |
    |             Copyright (c) 2007-2014 Vincent ROGIER <vince.rogier@ocilib.net>            |
    |                                                                                         |
    +-----------------------------------------------------------------------------------------+
    |                                                                                         |
    |             This library is free software; you can redistribute it and/or               |
    |             modify it under the terms of the GNU Lesser General Public                  |
    |             License as published by the Free Software Foundation; either                |
    |             version 2 of the License, or (at your option) any later version.            |
    |                                                                                         |
    |             This library is distributed in the hope that it will be useful,             |
    |             but WITHOUT ANY WARRANTY; without even the implied warranty of              |
    |             MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU           |
    |             Lesser General Public License for more details.                             |
    |                                                                                         |
    |             You should have received a copy of the GNU Lesser General Public            |
    |             License along with this library; if not, write to the Free                  |
    |             Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.          |
    |                                                                                         |
    +-----------------------------------------------------------------------------------------+

    +-----------------------------------------------------------------------------------------+
    |                                     IMPORTANT NOTICE                                    |
    +-----------------------------------------------------------------------------------------+
    |                                                                                         |
    |            This C++ header defines C++ wrapper classes around the OCILIB C API          |
    |            It requires a compatible version of OCILIB                                   |
    +-----------------------------------------------------------------------------------------+

 */

/* --------------------------------------------------------------------------------------------- *
 * $Id: ocilib_priv.hpp, Vincent Rogier $
 * --------------------------------------------------------------------------------------------- */

#define ARG_NOT_USED(a) (a) = (a)

/**
 * @brief Internal usage.
 * Checks if the last OCILIB method call has raised an error.
 * If so, it raises a C++ exception using the retrieved error handle
 */
static void Check();

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
template<class TCharType>
static std::basic_string<TCharType, std::char_traits<TCharType>, std::allocator<TCharType> > MakeString(const TCharType *result);

/**
 * @class HandleHolder
 * @brief
 * Template class providing OCILIB handles auto memory, life cycle and scope management
 */
template <class THandleType>
class HandleHolder;

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
class Collection;
class CollectionIterator;
class Clob;
class Blob;
class File;
class Pool;
class CLong;
class BLong;
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
 * @class ManagedBuffer
 *
 * @brief
 *
 */
template< typename TBufferType>
class ManagedBuffer
{
public:
    ManagedBuffer();
    ManagedBuffer(TBufferType *buffer);
    ~ManagedBuffer();

    ManagedBuffer<TBufferType> & operator= (TBufferType *buffer);

    operator TBufferType* () const;
    operator const TBufferType* () const;

private:

    TBufferType* _buffer;
};

/**
 * @class ConcurrentPool
 *
 * @brief
 *
 */
template <class TKey, class TValue>
class ConcurrentPool
{
public:

    void Initialize(unsigned int envMode);
    void Release();

    void Remove(TKey key);
    TValue Get(TKey key) const;
    void Set(TKey key, TValue value);

private:

    typedef std::map< TKey, TValue > ConcurrentPoolMap;

    ConcurrentPoolMap  _map;
    MutexHandle _mutex;

    void Lock() const;
    void Unlock() const;
};

/**
 * @class Handle
 *
 * @brief
 *
 */
class Handle
{
public:

    virtual ~Handle() {};
    virtual std::list<Handle *> & GetChildren() = 0;
    virtual void DetachFromHolders() = 0;
    virtual void DetachFromParent() = 0;
};

/**
 * @class HandleHolder
 *
 * @brief
 *
 */
template<class THandleType>
class HandleHolder
{
public:

    bool IsNull() const;

    operator bool() const;
    operator THandleType();
    operator THandleType() const;

protected:

    template <class TSmartHandleType>
    class SmartHandle;

    HandleHolder(const HandleHolder &other);
    HandleHolder();
    ~HandleHolder();

    HandleHolder<THandleType>& operator= (const HandleHolder<THandleType> &other);

    typedef boolean (OCI_API *HandleFreeFunc)(UnknownHandle handle);

    Handle* GetHandle() const;

	void Acquire(THandleType handle, HandleFreeFunc func, Handle *parent);
    void Acquire(HandleHolder<THandleType> &other);
    void Release();

    template <class TSmartHandleType>
    class SmartHandle : public Handle
    {
    public:

        SmartHandle(HandleHolder<TSmartHandleType> *holder, TSmartHandleType handle, HandleFreeFunc func, Handle *parent);
        virtual ~SmartHandle();

        void Acquire(HandleHolder<TSmartHandleType> *holder);
        void Release(HandleHolder<TSmartHandleType> *holder);

        TSmartHandleType GetHandle() const;

        Handle *GetParent() const
        {
            return _parent;
        }

        BufferPointer GetExtraInfos() const;
        void   SetExtraInfos(BufferPointer extraInfo);

        bool IsLastHolder(HandleHolder<TSmartHandleType> *holder) const;

        std::list<Handle *> & GetChildren();
        void DetachFromHolders();
        void DetachFromParent();

    private:

        std::list<HandleHolder<TSmartHandleType> *> _holders;
        std::list<Handle *>  _children;

        THandleType _handle;
        HandleFreeFunc _func;
        Handle *_parent;
        BufferPointer _extraInfo;

    };

protected:

    SmartHandle<THandleType> *_smartHandle;
 };


/**
 * @class BindValue
 *
 * @brief
 *
 */
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

/**
 * @class BindObject
 *
 * @brief
 *
 */

class BindObject
{
public:
     virtual void SetInData()  = 0;
     virtual void SetOutData() = 0;
};

/**
 * @class BindArray
 *
 * @brief
 *
 */
class BindArray : public BindObject
{
public:

     BindArray();
     virtual ~BindArray();

     template <class TObjectType, class TDataType>
     void SetVector(std::vector<TObjectType> & vector, unsigned int mode, unsigned int elemCount, unsigned int elemSize);

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
    };

    template <class TObjectType, class TDataType>
    class BindArrayObject : public  AbstractBindArrayObject
    {
    private:

        std::vector<TObjectType> & _vector;
        TDataType *_data;
        unsigned int _mode;
        unsigned int _elemCount;
        unsigned int _elemSize;

        BindArrayObject & operator=( const BindArrayObject & other);

    public:

        BindArrayObject(std::vector<TObjectType> &vector, unsigned int mode, unsigned int elemCount, unsigned int elemSize);
        virtual ~BindArrayObject();
        void SetInData();
        void SetOutData();
        void AllocData();
        void FreeData();
        operator std::vector<TObjectType> & () const;
        operator TDataType * () const;
    };

    AbstractBindArrayObject * _object;
};

/**
 * @class BindBuffer
 *
 * @brief
 *
 */
class BindString : public BindObject
{
    friend class Statement;

public:

    operator otext *()  const;

    void SetInData();
    void SetOutData();

    BindString(ostring &source, unsigned int elemSize);
    virtual ~BindString();

private:

    ostring&        _string;
    otext *         _data;
    unsigned int    _elemSize;
};

/**
 * @class BindsHolder
 *
 * @brief
 *
 */
class BindsHolder
{
public:

    BindsHolder();
    ~BindsHolder();

    void Clear();

    void AddBindObject(BindObject *bindObject);

    void SetOutData();
    void SetInData();

private:

    std::vector<BindObject *> _bindObjects;
};
