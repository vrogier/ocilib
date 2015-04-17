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
    |             Copyright (c) 2007-2015 Vincent ROGIER <vince.rogier@ocilib.net>            |
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
 * $Id: ocilib_core.hpp, Vincent Rogier $
 * --------------------------------------------------------------------------------------------- */

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

    Handle* GetHandle() const;

	void Acquire(THandleType handle, HandleFreeFunc func, Handle *parent);
    void Acquire(HandleHolder &other);
    void Release();

    class SmartHandle : public Handle
    {
    public:

		SmartHandle(HandleHolder *holder, THandleType handle, HandleFreeFunc func, Handle *parent);
		virtual ~SmartHandle();

        void Acquire(HandleHolder *holder);
        void Release(HandleHolder *holder);

		const THandleType GetHandle() const;

		Handle *GetParent() const;

		AnyPointer GetExtraInfos() const;
		void  SetExtraInfos(AnyPointer extraInfo);

        bool IsLastHolder(HandleHolder *holder);

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
        HandleFreeFunc _func;
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

	BindObject(const Statement &statement, const ostring& name);

	virtual ~BindObject();

	ostring GetName() const;

	Statement GetStatement() const;

    virtual void SetInData()  = 0;
    virtual void SetOutData() = 0;

protected:

    ostring _name;
	OCI_Statement *_pStatement;
};

class BindArray : public BindObject
{
public:

	 BindArray(const Statement &statement, const ostring& name);
     virtual ~BindArray();

     template <class TObjectType, class TDataType>
	 void SetVector(std::vector<TObjectType> & vector, unsigned int mode, unsigned int elemSize);

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
class BindAdaptor : public BindObject
{
    friend class Statement;

public:

	operator TNativeType *()  const;

	void SetInData();
	void SetOutData();

	BindAdaptor(const Statement &statement, const ostring& name, TObjectType &object, unsigned int size);
	virtual ~BindAdaptor();

private:

	TObjectType&    _object;
    TNativeType*    _data;
    unsigned int    _size;
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
