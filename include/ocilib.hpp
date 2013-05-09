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
    |             Copyright (c) 2007-2013 Vincent ROGIER <vince.rogier@ocilib.net>            |
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
 * $Id: ocilib.hpp, Vincent Rogier $
 * --------------------------------------------------------------------------------------------- */

#ifndef OCILIBCPP_H_INCLUDED
#define OCILIBCPP_H_INCLUDED

/* includes */

#include <string>
#include <list>
#include <algorithm>
#include <iostream>
#include <vector>
#include <map>

#include "ocilib.h"

namespace ocilib
{

typedef std::basic_string<mtext, std::char_traits<mtext>, std::allocator<mtext> > mstring;
typedef std::basic_string<dtext, std::char_traits<dtext>, std::allocator<dtext> > dstring;
typedef void *              BufferPointer;
typedef OCI_Thread *        ThreadHandle;
typedef OCI_Mutex *         MutexHandle ;
typedef POCI_THREAD         ThreadProc;
typedef POCI_THREADKEYDEST  ThreadKeyFreeProc;

/* ********************************************************************************************* *
 *                                       CLASS DECLARATIONS
 * ********************************************************************************************* */

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
class Subscription;
class Queue;
class Dequeue;
class Enqueue;
class Pool;
class Message;
class CLong;
class BLong;
class HashTable;
class Event;
class Element;
class Column;
class Agent;

template <class TDataType>
class ManagedPtr;

/**
 * @typedef HAHandlerProc
 *
 * @brief
 *
 *
 */
typedef void (*HAHandlerProc) (const Connection &con, unsigned int source, unsigned int event, const Timestamp  &time);

/**
 * @typedef TAFHandlerProc
 *
 * @brief
 *
 *
 */
typedef void (*TAFHandlerProc) (const Connection &con, unsigned int type, unsigned int event);

/**
 * @namespace API
 *
 * @brief
 *
 *
 */
namespace API
{
    void Call();

    template<class TResultType>
    TResultType Call(TResultType result);

    template<class TCharType>
    std::basic_string<TCharType, std::char_traits<TCharType>, std::allocator<TCharType> > MakeString(const TCharType *result);
}


template< typename TBufferType>
class ManagedBuffer
{
public:
    ManagedBuffer();
    ManagedBuffer(TBufferType *buffer);
    ~ManagedBuffer();

    ManagedBuffer<TBufferType> & operator= (TBufferType *buffer);

    operator TBufferType* ();
    operator const TBufferType* ();

private:

    TBufferType* _buffer;
};

/**
 * @class Handle
 *
 * @brief
 *
 *
 */
class Handle
{
public:

    virtual std::list<Handle *> & GetChildren() = 0;

};


class HandlePool
{
public:

    void Remove(void * ociHandle);
    Handle * Get(void *ociHandle);
    void Set(void * ociHandle, Handle *Handle);

private:

    std::map<void *, Handle * > _map;

    void Lock();
    void Unlock();
};

/**
 * @class HandleHolder
 *
 * @brief
 *
 *
 */
template<class THandleType>
class HandleHolder
{
public:

    bool IsNull();

    operator bool();
    operator THandleType();
    operator THandleType() const;

protected:

    template <class TSmartHandleType>
    class SmartHandle;

    HandleHolder(const HandleHolder &src);
    HandleHolder();
    ~HandleHolder();

    HandleHolder<THandleType>& operator= (HandleHolder<THandleType> &other);

    typedef boolean (OCI_API *HandleFreeFunc)(void *handle);

    Handle* GetHandle() const;

	void Acquire(THandleType handle, HandleFreeFunc func, Handle *parent);
    void Acquire(THandleType handle);
    void Acquire(HandleHolder<THandleType> &other);
    void Release();

    template <class TSmartHandleType>
    class SmartHandle : public Handle
    {
    public:

        SmartHandle(HandleHolder<TSmartHandleType> *holder, TSmartHandleType handle, HandleFreeFunc func, Handle *parent);
        ~SmartHandle();

        void Acquire(HandleHolder<TSmartHandleType> *holder);
        void Release(HandleHolder<TSmartHandleType> *holder);

        TSmartHandleType GetHandle();

        void * GetExtraInfos();
        void   SetExtraInfos(void *extraInfo);

        bool IsLastHolder(HandleHolder<TSmartHandleType> *holder);

        std::list<Handle *> & GetChildren();

    private:

        void DetachFromHolders();

        std::list<HandleHolder<TSmartHandleType> *> _holders;
        std::list<Handle *>  _children;

        THandleType _handle;
        HandleFreeFunc _func;
        Handle *_parent;
        void * _extraInfo;

    };

protected:

    SmartHandle<THandleType> *_smartHandle;
 };


/**
 * @class Exception
 *
 * @brief
 *
 *
 */
class Exception : public HandleHolder<OCI_Error *>
{
    friend void API::Call();
    friend class Statement;

public:

    mstring         GetMessage();
    unsigned int    GetType();
    unsigned int    GetOracleErrorCode();
    unsigned int    GetInternalErrorCode();
    Statement       GetStatement();
    Connection      GetConnection();

private:

    Exception();
    Exception(OCI_Error *err);
};

/**
 * @class Environment
 *
 * @brief
 *
 *
 */

class Environment
{
    friend class Connection;
    template<class THandleType>
    friend class HandleHolder;

public:

    static void Initialize(unsigned int mode = OCI_ENV_DEFAULT, mstring libpath = MT(""));
    static void Cleanup();

    static unsigned int GetImportMode();

    static unsigned int GetCharsetMetaData();
    static unsigned int GetCharsetUserData();

    static unsigned int GetCompileVersion();
    static unsigned int GetRuntimeVersion();

    static void EnableWarnings(bool value);

    static void StartDatabase(mstring db, mstring user, mstring pwd, unsigned int start_flag, unsigned int start_mode,
                              unsigned int sess_mode = OCI_SESSION_SYSDBA, mstring spfile = MT(""));

    static void ShutdownDatabase(mstring db, mstring user, mstring pwd, unsigned int shut_flag, unsigned int shut_mode,
                              unsigned int sess_mode = OCI_SESSION_SYSDBA);

    static void ChangeUserPassword(mstring db, mstring user, mstring pwd, mstring newPassword);

    static void SetHAHandler(HAHandlerProc handler);

    static void CreateThreadKey(mstring name, ThreadKeyFreeProc freeProc = 0);
    static void ThreadKeySetValue(mstring name, void *value);
    static void * ThreadKeyGetValue(mstring name);

    static ThreadHandle CreateThread();
    static void DestroyThread(ThreadHandle handle);
    static void RunThread(ThreadHandle handle, ThreadProc func, void *args);
    static void JoinThread(ThreadHandle thrhandleead);

    static MutexHandle CreateMutex();
    static void DestroyMutex(MutexHandle handle);
    static void AcquireMutex(MutexHandle handle);
    static void ReleaseMutex(MutexHandle handle);

private:

    static unsigned int &GetMode();

    static HAHandlerProc &GetHAHandler();
    static void HAHandler(OCI_Connection *con, unsigned int source, unsigned int event, OCI_Timestamp  *time);

    static HandlePool &GetHandlePool();
};


/**
 * @class Pool
 *
 * @brief
 *
 *
 */
class Pool : public HandleHolder<OCI_Pool *>
{
public:

    Pool();
    Pool(mstring db, mstring user, mstring pwd, unsigned int poolType,
         unsigned int minCon, unsigned int maxCon, unsigned int incrCon = 1,
         unsigned int sessionMode = OCI_SESSION_DEFAULT);


    void Open(mstring db, mstring user, mstring pwd, unsigned int poolType,
              unsigned int minCon, unsigned int maxCon, unsigned int incrCon = 1,
              unsigned int sessionMode = OCI_SESSION_DEFAULT);
    void Close();

    Connection GetConnection(mstring sessionTag = MT(""));

    unsigned int GetTimeout();
    void SetTimeout(unsigned int value);

    bool GetNoWait();
    void SetNoWait(bool value);

    unsigned int GetBusyConnectionsCount();
    unsigned int GetOpenedConnectionsCount();

    unsigned int GetMinSize();
    unsigned int GetMaxSize();
    unsigned int GetIncrement();

    unsigned int GetStatementCacheSize();
    void SetStatementCacheSize(unsigned int value);
};

/**
 * @class Connection
 *
 * @brief
 *
 *
 */
class Connection : public HandleHolder<OCI_Connection *>
{
    friend class Environment;
    friend class Exception;
    friend class Statement;
    friend class Clob;
    friend class Blob;
    friend class File;
    friend class Timestamp;
    friend class Pool;
    friend class Object;
    friend class TypeInfo;
    friend class Reference;
    friend class Resultset;
    friend class Collection;

public:

    Connection();
    Connection(mstring db, mstring user, mstring pwd, unsigned int sessionMode = OCI_SESSION_DEFAULT);

    void Open(mstring db, mstring user, mstring pwd, unsigned int sessionMode = OCI_SESSION_DEFAULT);
    void Close();

    void Commit();
    void Rollback();
    void Break();

    void SetAutoCommit(bool enabled);
    bool GetAutoCommit();

    bool IsServerAlive();

    bool PingServer();

    mstring GetConnectionString();
    mstring GetUserName();
    mstring GetPassword();

    unsigned int GetVersion();

    mstring GetServerVersion();
    unsigned int GetServerMajorVersion();
    unsigned int GetServerMinorVersion();
    unsigned int GetServerRevisionVersion();

    void ChangePassword(mstring newPassword);

    mstring GetSessionTag();
    void SetSessionTag(mstring tag);

    Transaction GetTransaction();
    void SetTransaction(const Transaction &transaction);

    void SetDefaultDateFormat(mstring format);
    void SetDefaultNumericFormat(mstring format);

    mstring  GetDefaultDateFormat();
    mstring  GetDefaultNumericFormat();

    void EnableServerOutput(unsigned int bufsize, unsigned int arrsize, unsigned int lnsize);
    void DisableServerOutput();
    bool GetServerOutput(mstring &line);
    void GetServerOutput(std::vector<mstring> &lines);

    void SetTrace(unsigned int trace, mstring value);
    mstring GetTrace(unsigned int trace);

    mstring GetDatabase();
    mstring GetInstance();
    mstring GetService();
    mstring GetServer();
    mstring GetDomain();

    Timestamp GetInstanceStartTime();

    unsigned int GetStatementCacheSize();
    void SetStatementCacheSize(unsigned int value);

    unsigned int GetDefaultLobPrefetchSize();
    void SetDefaultLobPrefetchSize(unsigned int value);

    bool IsTAFCapable();

    void SetTAFHandler(TAFHandlerProc handler);

private:

    Connection(OCI_Connection *con, Handle *parent = NULL);

    static void TAFHandler(OCI_Connection *con, unsigned int type, unsigned int event);

    TAFHandlerProc _tafHandler;
};

/**
 * @class Transaction
 *
 * @brief
 *
 *
 */
class Transaction : public HandleHolder<OCI_Transaction *>
{
    friend class Connection;

public:

   Transaction(const Connection &connection, unsigned int timeout, unsigned int mode, OCI_XID *pxid);

   void Prepare();
   void Start();
   void Stop();
   void Resume();
   void Forget();

   unsigned int GetMode();
   unsigned int GetTimeout();

private:

    Transaction(OCI_Transaction *trans);
};

/**
 * @class Date
 *
 * @brief
 *
 *
 */
class Date : public HandleHolder<OCI_Date *>
{
    friend class Statement;
    friend class Resultset;
    friend class BindArray;
    friend class Object;
    friend class Collection;

public:

    Date();

    void Assign(const Date& other);
    int Compare(const Date& other);

    int DaysBetween(const Date& other);

    void SetDate(int year, int month, int day);
    void SetTime(int hour, int min,   int sec);
    void SetDateTime(int year, int month, int day, int hour, int min, int sec);

    void GetDate(int *year, int *month, int *day);
    void GetTime(int *hour, int *min,   int *sec);
    void GetDateTime(int *year, int *month, int *day, int *hour, int *min, int *sec);

    void AddDays(int days);
    void AddMonths(int months);

    void SysDate();
    void NextDay(mstring day);
    void LastDay();

    void ChangeTimeZone(mstring tzSrc, mstring tzDst);

    void FromString(mstring data, mstring format = OCI_STRING_FORMAT_DATE);
    mstring ToString(mstring format = OCI_STRING_FORMAT_DATE);

    operator mstring();

private:

    Date(OCI_Date *pDate);
};

/**
 * @class Interval
 *
 * @brief
 *
 *
 */
class Interval : public HandleHolder<OCI_Interval *>
{
    friend class Environment;
    friend class Statement;
    friend class Resultset;
    friend class BindArray;
    friend class Object;
    friend class Collection;

public:

    Interval(unsigned int type);

    void Assign(const Interval& other);
    int Compare(const Interval& other);

    unsigned int GetType();

    void Add(const Interval& other);
    void Substract(const Interval& other);

    void GetDaySecond(int *day, int *hour, int *min, int *sec, int *fsec);
    void SetDaySecond(int day, int hour, int min, int sec, int fsec);

    void GetYearMonth(int *year, int *month);
    void SetYearMonth(int year, int month);

    void FromTimeZone(mstring timeZone);

    void FromString(mstring data, mstring format);
    mstring ToString(int leadingPrecision = 10, int fractionPrecision = 10);

    operator mstring();

private:

    Interval(OCI_Interval *pInterval);
};

/**
 * @class Timestamp
 *
 * @brief
 *
 *
 */
class Timestamp : public HandleHolder<OCI_Timestamp *>
{
    friend class Environment;
    friend class Statement;
    friend class Resultset;
    friend class BindArray;
    friend class Object;
    friend class Connection;
    friend class Collection;

public:

    Timestamp(unsigned int type);

    void Assign(const Timestamp& other);
    int Compare(const Timestamp& other);

    unsigned int GetType();

    void Construct(int year, int month, int day, int hour, int min, int sec, int fsec, mstring timeZone = MT(""));
    void Convert(const Timestamp& other);

    void GetDate(int *year, int *month, int *day);
    void GetTime(int *hour, int *min,   int *sec, int *fsec);
    void GetDateTime(int *year, int *month, int *day, int *hour, int *min, int *sec, int *fsec);

    mstring GetTimeZone();
    void GetTimeZoneOffset(int *hour, int *min);

    void AddInterval(const Interval& other);
    void SubstractInterval(const Interval& other);

    void Substract(const Timestamp &other, Interval &result);

    void SysTimestamp();

    void FromString(mstring data, mstring format = OCI_STRING_FORMAT_DATE);
    mstring ToString(mstring format = OCI_STRING_FORMAT_DATE, int precision = 0);

    operator mstring();

private:

    Timestamp(OCI_Timestamp *pTimestamp);
};

/**
 * @class Clob
 *
 * @brief
 *
 *
 */
class Clob : public HandleHolder<OCI_Lob *>
{
    friend class Statement;
    friend class Resultset;
    friend class BindArray;
    friend class Object;
    friend class Collection;

public:

    Clob(const Connection &connection);

    dstring Read(unsigned int size);
    unsigned int Write(dstring content);
    unsigned int Append(dstring content);
    unsigned int Seek(unsigned int seekMode, big_uint offset);

    void Append(const Clob &other);
    void Assign(const Clob &other);
    bool Equals(const Clob &other);

    big_uint GetOffset();
    big_uint GetLength();
    big_uint GetMaxSize();
    big_uint GetChunkSize();

    void Truncate(big_uint size);
    big_uint Erase(big_uint offset, big_uint size);
    void Copy(Clob &dest, big_uint offset, big_uint offsetDest, big_uint size);

    bool IsTemporary();

    void Open(unsigned int mode);
    void Flush();
    void Close();

    void EnableBuffering(bool value);

private:

    Clob(OCI_Lob *pLob);
};

/**
 * @class Blob
 *
 * @brief
 *
 *
 */
class Blob : public HandleHolder<OCI_Lob *>
{
    friend class Statement;
    friend class Resultset;
    friend class BindArray;
    friend class Object;
    friend class Collection;

public:

    Blob(const Connection &connection);

    unsigned int Read(void *buffer, unsigned int size);
    unsigned int Write(void *buffer, unsigned int size);
    unsigned int Append(void *buffer, unsigned int size);
    unsigned int Seek(unsigned int seekMode, big_uint offset);

    void Append(const Blob &other);
    void Assign(const Blob &other);
    bool Equals(const Blob &other);

    big_uint GetOffset();
    big_uint GetLength();
    big_uint GetMaxSize();
    big_uint GetChunkSize();

    void Truncate(big_uint size);
    big_uint Erase(big_uint offset, big_uint size);
    void Copy(Blob &dest, big_uint offset, big_uint offsetDest, big_uint size);

    bool IsTemporary();

    void Open(unsigned int mode);
    void Flush();
    void Close();

    void EnableBuffering(bool value);

private:

    Blob(OCI_Lob *pLob);
};

/**
 * @class File
 *
 * @brief
 *
 *
 */
class File : public HandleHolder<OCI_File *>
{
    friend class Statement;
    friend class Resultset;
    friend class BindArray;
    friend class Object;
    friend class Collection;

public:

    File(const Connection &connection);
    File(const Connection &connection, mstring directory, mstring name);

    unsigned int Read(void *buffer, unsigned int size);
    unsigned int Seek(unsigned int seekMode, big_uint offset);

    void Assign(const File &other);
    bool Equals(const File &other);

    bool Exists();

    big_uint GetOffset();
    big_uint GetSize();

    void SetInfos(mstring directory, mstring name);

    mstring GetName();
    mstring GetDirectory();

    void Open();
    void Close();
    bool IsOpened();

private:

    File(OCI_File *pFile);
};

/**
 * @class TypeInfo
 *
 * @brief
 *
 *
 */
class TypeInfo : public HandleHolder<OCI_TypeInfo *>
{
    friend class Object;
    friend class Reference;
    friend class Collection;
    friend class Column;
public:

    TypeInfo(const Connection &connection, mstring name, unsigned int type);

    unsigned int GetType();
    mstring GetName();
    Connection GetConnection();

    unsigned int GetColumnCount();
    Column GetColumn(unsigned int index);

private:

    Connection GetConnection() const;

    TypeInfo(OCI_TypeInfo *pTypeInfo);
};

/**
 * @class Object
 *
 * @brief
 *
 *
 */
class Object : public HandleHolder<OCI_Object *>
{
    friend class Statement;
    friend class Resultset;
    friend class BindArray;
    friend class Reference;
    friend class Collection;

public:

    Object(const TypeInfo &typeInfo);

    TypeInfo GetTypeInfo();

    template<class TDataType>
    TDataType Get(mstring name);

    template<class TDataType>
    void Set(mstring name, const TDataType &value);

    template<class TDataType>
    void Get(mstring name, TDataType &value, unsigned int &size);
 
    template<class TDataType>
    void Set(mstring name, const TDataType &value, unsigned int size);

private:

    Object(OCI_Object *pObject);
};

/**
 * @class Reference
 *
 * @brief
 *
 *
 */
class Reference : public HandleHolder<OCI_Ref *>
{
    friend class Statement;
    friend class Resultset;
    friend class BindArray;
    friend class Object;
    friend class Collection;

public:

    Reference(const Connection &connection, const TypeInfo &typeInfo);

    TypeInfo GetTypeInfo();
    Object GetObject();

    void Assign(const Reference& other);

    bool IsNullReference();
    void SetNulReference();

    mstring ToString();

private:

    Reference(OCI_Ref *pRef);
};

/**
 * @class Collection
 *
 * @brief
 *
 *
 */
class Collection : public HandleHolder<OCI_Coll *>
{
    friend class Statement;
    friend class Resultset;
    friend class BindArray;
    friend class Object;
    friend class CollectionIterator;
public:

    Collection(const TypeInfo &typeInfo);

    void Assign(const Collection& other);

    unsigned int GetType();
    unsigned int GetMax();
    unsigned int GetSize();

    void Truncate(unsigned int size);
    void Clear();

    template <class TDataType>
    TDataType Get(unsigned int index);

    template <class TDataType>
    void Set(unsigned int index, const TDataType &data);

    template<class TDataType>
    void Get(unsigned int index, TDataType &value, unsigned int &size);
 
    template<class TDataType>
    void Set(unsigned int index, const TDataType &value, unsigned int size);

    template <class TDataType>
    void Append(const TDataType &data);

    template <class TDataType>
    void Append(const TDataType &data, unsigned int size);

    TypeInfo GetTypeInfo();

private:

    template <class TDataType>
    static TDataType GetElem(OCI_Elem *elem);

    template <class TDataType>
    static void SetElem(OCI_Elem *elem, const TDataType &value);

    template <class TDataType>
    static TDataType GetElem(OCI_Elem *elem,  unsigned int &size);

    template <class TDataType>
    static void SetElem(OCI_Elem *elem, const TDataType &value,  unsigned int size);

    Collection(OCI_Coll *pColl);
};


/**
 * @class CollectionIterator
 *
 * @brief
 *
 *
 */
class CollectionIterator : public HandleHolder<OCI_Iter *>
{
public:

    CollectionIterator(const Collection &collection);

    template <class TDataType>
    TDataType Get();

    template <class TDataType>
    void Set(TDataType &value);

    bool Next();
    bool Prev();

};


/**
 * @class CLong
 *
 * @brief
 *
 *
 */
class CLong : public HandleHolder<OCI_Long *>
{
    friend class Statement;
    friend class Resultset;
    friend class BindArray;

public:

    CLong(const Statement &statement);

    dstring Read(unsigned int size);
    unsigned int Write(dstring content);

    unsigned int GetSize();
    dstring GetContent();

private:

    CLong(OCI_Long *pLong);
};

/**
 * @class CLong
 *
 * @brief
 *
 *
 */
class BLong : public HandleHolder<OCI_Long *>
{
    friend class Statement;
    friend class Resultset;
    friend class BindArray;

public:

    BLong(const Statement &statement);

    unsigned int Read(void *buffer, unsigned int size);
    unsigned int Write(void *buffer, unsigned int size);

    unsigned int GetSize();
    void * GetContent();

private:

    BLong(OCI_Long *pLong);
};

/**
 * @class BindValue
 *
 * @brief
 *
 *
 */
template <class TValueType>
class BindValue
{
public:

    BindValue();
    BindValue(TValueType value);

    operator TValueType();

private:

    TValueType _value;
};

/**
 * @class BindArray
 *
 * @brief
 *
 *
 */
class BindArray
{
public:

    template <class TObjectType, class TDataType>
    BindArray(std::vector<TObjectType> & vector, BindValue<TDataType> datatype, unsigned int mode, unsigned int elemCount, unsigned int elemSize);
    ~BindArray();

    template <class TObjectType, class TDataType>
    std::vector<TObjectType> & GetVector(BindValue<TDataType> datatype);

     template <class TObjectType, class TDataType>
     TDataType * GetData ();

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

    public:

        BindArrayObject(std::vector<TObjectType> &vector, BindValue<TDataType> datatype, unsigned int mode, unsigned int elemCount, unsigned int elemSize);
        ~BindArrayObject();
        void SetInData();
        void SetOutData();
        void AllocData();
        void FreeData();
        operator std::vector<TObjectType> & ();
        operator TDataType * ();
    };

    AbstractBindArrayObject * _object;
};

/**
 * @class BindBuffer
 *
 * @brief
 *
 *
 */
class BindString
{
    friend class Statement;

public:

    operator dtext *();

    void SetOutData();

    BindString(dstring &source, unsigned int elemSize);
    ~BindString();

private:

    dstring&        _string;
    dtext *         _data;
    unsigned int    _elemSize;
};

/**
 * @class BindsHolder
 *
 * @brief
 *
 *
 */
class BindsHolder
{
public:
     
    BindsHolder();
    ~BindsHolder();

    void Clear();

    void AddBindArray(BindArray *bindArray);
    void AddBindString(BindString *bindString);

    void SetOutData();
    void SetInData();

private:

    std::vector<BindString *> _bindStrings;
    std::vector<BindArray  *> _bindArrays;
};

/**
 * @class Statement
 *
 * @brief
 *
 *
 */
class Statement : public HandleHolder<OCI_Statement *>
{
    friend class Exception;
    friend class Resultset;
    friend class CLong;
    friend class BLong;

public:

    Statement(const Connection &connection);
    ~Statement();

    Connection GetConnection();

    void Describe(mstring sql);
    void Parse(mstring sql);
    void Prepare(mstring sql);
    void Execute();
    void Execute(mstring sql);

    unsigned int GetAffectedRows();

    mstring GetSql();

    Resultset GetResultset();
    Resultset GetNextResultset();

    void SetBindArraySize(unsigned int size);
    unsigned int GetBindArraySize();

    void AllowRebinding(bool value);

    template <class TDataType>
    void Bind(mstring name, TDataType &value, unsigned int mode);

    template <class TDataType, class TExtraInfo>
    void Bind(mstring name, TDataType &value, TExtraInfo extraInfo, unsigned int mode);

    template <class TDataType>
    void Bind(mstring name, std::vector<TDataType> &values, unsigned int mode);

    template <class TDataType, class TExtraInfo>
    void Bind(mstring name, std::vector<TDataType> &values, TExtraInfo extraInfo, unsigned int mode);

    template <class TDataType, class TExtraInfo>
    void Bind(mstring name, std::vector<TDataType> &values, TExtraInfo &extraInfo, unsigned int mode);

    template <class TDataType>
    void Register(mstring name);

    template <class TDataType, class TExtraInfo>
    void Register(mstring name, TExtraInfo extraInfo);

    template <class TDataType, class TExtraInfo>
    void Register(mstring name, TExtraInfo &extraInfo);

    unsigned int GetStatementType();

    unsigned int GetSqlErrorPos();

    unsigned int GetBindCount();

    void SetFetchMode(unsigned int value);
    unsigned int GetFetchMode();

    void SetBindMode(unsigned int value);
    unsigned int GetBindMode();

    void SetFetchSize(unsigned int value);
    unsigned int GetFetchSize();

    void SetPrefetchSize(unsigned int value);
    unsigned int GetPrefetchSize();

    void SetPrefetchMemory(unsigned int value);
    unsigned int GetPrefetchMemory();

    void SetLongMaxSize(unsigned int value);
    unsigned int GetLongMaxSize();

    void SetLongMode(unsigned int value);
    unsigned int GetLongMode();

    unsigned int GetSQLCommand();
    mstring GetSQLVerb();

    void GetBatchErrors(std::vector<Exception> &exceptions);

private:

    Statement(OCI_Statement *stmt);
 
    BindsHolder *GetBindsHolder(bool allocate);

    void SetInData();
    void SetOutData();
    void ClearBinds();

    template <typename TBindMethod, class TDataType>
    void Bind (TBindMethod &method, mstring name, TDataType& value, unsigned int mode);

    template <typename TBindMethod, class TObjectType, class TDataType>
    void Bind (TBindMethod &method, mstring name, TObjectType &value, BindValue<TDataType> datatype, unsigned int mode);

    template <typename TBindMethod, class TObjectType, class TDataType>
    void Bind (TBindMethod &method, mstring name, std::vector<TObjectType> &values, BindValue<TDataType> datatype, unsigned int mode);

    template <typename TBindMethod, class TObjectType, class TDataType, class TElemType>
    void Bind (TBindMethod &method, mstring name, std::vector<TObjectType> &values, BindValue<TDataType> datatype, unsigned int mode, TElemType type);
};

/**
 * @class Resultset
 *
 * @brief
 *
 *
 */
class Resultset : public HandleHolder<OCI_Resultset *>
{
    friend class Statement;
public:

    template<class TDataType>
    TDataType Get(int index);

    template<class TDataType>
    TDataType Get(mstring name);

    bool Next();
    bool Prev();
    bool First();
    bool Last();
    bool Seek(unsigned int mode, int offset);

    unsigned int GetCount();
    unsigned int GetCurrentRow();

    unsigned int GetColumnCount();
    Column GetColumn(unsigned int index);
    Column GetColumn(mstring name);

    bool IsColumnNull(unsigned int index);
    bool IsColumnNull(mstring name);

    Statement GetStatement();

private:

   Resultset(OCI_Resultset *resultset);
};

/**
 * @class Column
 *
 * @brief
 *
 *
 */
class Column : public HandleHolder<OCI_Column *>
{
    friend class TypeInfo;
    friend class Resultset;

public:

    mstring GetName();
    mstring GetSQLType();
    mstring GetFullSQLType();

    unsigned int GetType();
    unsigned int GetSubType();
    unsigned int GetCharsetForm();
    unsigned int GetSize();

    int GetScale();
    int GetPrecision();
    int GetFractionalPrecision();
    int GetLeadingPrecision();

    bool GetNullable();
    bool GetCharUsed();

    TypeInfo GetTypeInfo();

private:

    Column(OCI_Column *column);
};

/* ********************************************************************************************* *
 *                                         IMPLEMENTATION
 * ********************************************************************************************* */

inline void API::Call()
{
    OCI_Error *err = OCI_GetLastError();

    if (err)
    {
        throw Exception(err);
    }
}

template<class TResultType>
inline TResultType API::Call(TResultType result)
{
    Call();

    return result;
}

template<class TCharType>
inline std::basic_string<TCharType, std::char_traits<TCharType>, std::allocator<TCharType> > API::MakeString(const TCharType *result)
{
    if (result)
    {
        return std::basic_string<TCharType, std::char_traits<TCharType>, std::allocator<TCharType> > (result);
    }
    else
    {
        return std::basic_string<TCharType, std::char_traits<TCharType>, std::allocator<TCharType> > ();
    }
}

/* --------------------------------------------------------------------------------------------- *
 * ManagedBuffer
 * --------------------------------------------------------------------------------------------- */

template< typename TBufferType>
inline ManagedBuffer<TBufferType>::ManagedBuffer()  : _buffer( NULL )
{
}

template< typename TBufferType>
inline ManagedBuffer<TBufferType>::ManagedBuffer( TBufferType *buffer )  : _buffer( buffer )
{
}

template< typename TBufferType>
inline ManagedBuffer<TBufferType>::~ManagedBuffer()
{
    delete [] _buffer;
}

template< typename TBufferType>
inline ManagedBuffer<TBufferType> & ManagedBuffer<TBufferType>::operator=( TBufferType *buffer )
{
    delete [] _buffer;
    _buffer = buffer;

    return( *this );
}

template< typename TBufferType>
inline ManagedBuffer<TBufferType>::operator TBufferType* ()
{
    return  _buffer;
}

template< typename TBufferType>
inline ManagedBuffer<TBufferType>::operator const TBufferType* ()
{
    return  _buffer;
}

/* --------------------------------------------------------------------------------------------- *
 * Handle
 * --------------------------------------------------------------------------------------------- */

template<class THandleType>
inline HandleHolder<THandleType>::HandleHolder() :  _smartHandle(0)
{
}

template<class THandleType>
inline HandleHolder<THandleType>::HandleHolder(const HandleHolder &src) :  _smartHandle(0)
{
    Acquire(src);
}

template<class THandleType>
inline HandleHolder<THandleType>::~HandleHolder()
{
    Release();
}

template<class THandleType>
inline HandleHolder<THandleType>& HandleHolder<THandleType>::operator = (HandleHolder<THandleType> &other)
{
    Acquire(other);
    return *this;
}

template<class THandleType>
inline bool HandleHolder<THandleType>::IsNull()
{
    return  (((THandleType) *this) == 0);
}

template<class THandleType>
inline HandleHolder<THandleType>::operator THandleType()
{
    return _smartHandle ? _smartHandle->GetHandle() : 0;
}

template<class THandleType>
inline HandleHolder<THandleType>::operator THandleType() const
{
    return _smartHandle ? _smartHandle->GetHandle() : 0;
}

template<class THandleType>
inline HandleHolder<THandleType>::operator bool()
{
    return IsNull();
}

template<class THandleType>
inline Handle * HandleHolder<THandleType>::GetHandle() const
{
    return ( Handle *) _smartHandle;
}

template<class THandleType>
inline void HandleHolder<THandleType>::Acquire(THandleType handle, HandleFreeFunc func, Handle *parent)
{
    _smartHandle = new HandleHolder::SmartHandle<THandleType>(this, handle, func, parent);

    Environment::GetHandlePool().Set(handle, _smartHandle);
}

template<class THandleType>
inline void HandleHolder<THandleType>::Acquire(THandleType handle)
{
    _smartHandle = dynamic_cast<SmartHandle<THandleType> *>(Environment::GetHandlePool().Get(handle));

    if (!_smartHandle)
    {
        _smartHandle = new HandleHolder::SmartHandle<THandleType>(this, handle, 0, 0);
    }

    _smartHandle->Acquire(this);
}

template<class THandleType>
inline void HandleHolder<THandleType>::Acquire(HandleHolder<THandleType> &other)
{
    if (&other != this)
    {
        Release();

        if (other._smartHandle)
        {
            other._smartHandle->Acquire(this);
            _smartHandle = other._smartHandle;
        }
    }
}

template<class THandleType>
inline void HandleHolder<THandleType>::Release()
{
    if (_smartHandle)
    {
        _smartHandle->Release(this);
    }

    _smartHandle = 0;
}

inline void HandlePool::Remove(void *ociHandle)
{
    Lock();
    _map.erase(ociHandle);
    Unlock();
}

inline Handle * HandlePool::Get(void *ociHandle)
{
    Handle *handle = 0;

    Lock();
    std::map<void *, Handle *>::iterator it = _map.find(ociHandle);
    if (it != _map.end() )
    {
        handle = it->second;
    }
    Unlock();

    return handle;
}

inline void HandlePool::Set(void *ociHandle, Handle *handle)
{
    Lock();
    _map[ociHandle] = handle;
    Unlock();
}

inline void HandlePool::Lock()
{

}

inline void HandlePool::Unlock()
{

}

template <class THandleType>
template <class TSmartHandleType>
inline HandleHolder<THandleType>::SmartHandle<TSmartHandleType>::SmartHandle(HandleHolder<TSmartHandleType> *holder, TSmartHandleType handle, HandleFreeFunc func, Handle *parent)
    : _handle(handle), _func(func), _parent(parent), _extraInfo(0)
{
    Acquire(holder);

    if (_parent && _handle)
    {
        _parent->GetChildren().push_back(this);
    }
}

template <class THandleType>
template <class TSmartHandleType>
inline HandleHolder<THandleType>::SmartHandle<TSmartHandleType>::~SmartHandle()
{
    boolean ret = TRUE;
    boolean chk = FALSE;

    if (_parent && _handle)
    {
        _parent->GetChildren().remove(this);
    }

    if (_func)
    {
        for(std::list<Handle *>::iterator it = _children.begin(); it != _children.end(); it++)
        {
            SmartHandle<TSmartHandleType> *handle = reinterpret_cast<SmartHandle<TSmartHandleType> *> (*it);

            handle->_parent = 0;

            handle->DetachFromHolders();

            delete handle;
        }

        Environment::GetHandlePool().Remove(_handle);

        ret = _func(_handle);
        chk = TRUE;
    }

    if (chk)
    {
        API::Call(ret);
    }
}

template <class THandleType>
template <class TSmartHandleType>
inline void HandleHolder<THandleType>::SmartHandle<TSmartHandleType>::Acquire(HandleHolder<TSmartHandleType> *holder)
{
    _holders.push_back(holder);
}

template <class THandleType>
template <class TSmartHandleType>
inline void HandleHolder<THandleType>::SmartHandle<TSmartHandleType>::Release(HandleHolder<TSmartHandleType> *holder)
{
    _holders.remove(holder);

    if (_holders.size() == 0)
    {
        delete this;
    }

    holder->_smartHandle = 0;
}

template <class THandleType>
template <class TSmartHandleType>
inline bool HandleHolder<THandleType>::SmartHandle<TSmartHandleType>::IsLastHolder(HandleHolder<TSmartHandleType> *holder)
{
    return ((_holders.size() == 1) && (*(_holders.begin()) == holder));
}

template <class THandleType>
template <class TSmartHandleType>
inline TSmartHandleType HandleHolder<THandleType>::SmartHandle<TSmartHandleType>::GetHandle()
{
    return _handle;
}

template <class THandleType>
template <class TSmartHandleType>
inline void * HandleHolder<THandleType>::SmartHandle<TSmartHandleType>::GetExtraInfos()
{
    return _extraInfo;
}

template <class THandleType>
template <class TSmartHandleType>
inline void HandleHolder<THandleType>::SmartHandle<TSmartHandleType>::SetExtraInfos(void *extraInfo)
{
    _extraInfo = extraInfo;
}

template <class THandleType>
template <class TSmartHandleType>
inline void HandleHolder<THandleType>::SmartHandle<TSmartHandleType>::DetachFromHolders()
{
    for(typename std::list<HandleHolder<THandleType> *>::iterator it = _holders.begin(); it != _holders.end(); it++)
    {
        (*it)->_smartHandle = 0;
    }

    _holders.clear();
}

template <class THandleType>
template <class TSmartHandleType>
inline std::list<Handle *> & HandleHolder<THandleType>::SmartHandle<TSmartHandleType>::GetChildren()
{
    return _children;
}

/* --------------------------------------------------------------------------------------------- *
 * Exception
 * --------------------------------------------------------------------------------------------- */

inline Exception::Exception()
{

}

inline Exception::Exception(OCI_Error *err)
{
    Acquire(err);
}

inline mstring Exception::GetMessage()
{
    return API::MakeString(OCI_ErrorGetString(*this));
}

inline unsigned int Exception::GetType()
{
    return OCI_ErrorGetType(*this);
}

inline unsigned int Exception::GetOracleErrorCode()
{
    return OCI_ErrorGetOCICode(*this);
}

inline unsigned int Exception::GetInternalErrorCode()
{
    return OCI_ErrorGetInternalCode(*this);
}

inline Statement Exception::GetStatement()
{
    return Statement(OCI_ErrorGetStatement(*this));
}

inline Connection Exception::GetConnection()
{
    return Connection(OCI_ErrorGetConnection(*this));
}

/* --------------------------------------------------------------------------------------------- *
 * Environment
 * --------------------------------------------------------------------------------------------- */

inline void Environment::Initialize(unsigned int mode, mstring libpath)
{
    mode |=  OCI_ENV_CONTEXT;

    OCI_Initialize(0, libpath.c_str(),  mode);

    GetMode() =  mode;

    API::Call();
}

inline void Environment::Cleanup()
{
    OCI_Cleanup();

    API::Call();
}

inline unsigned int Environment::GetImportMode()
{
    return API::Call(OCI_GetImportMode());
}

inline unsigned int Environment::GetCharsetMetaData()
{
    return API::Call(OCI_GetCharsetMetaData());
}

inline unsigned int Environment::GetCharsetUserData()
{
    return API::Call(OCI_GetCharsetUserData());
}

inline unsigned int Environment::GetCompileVersion()
{
    return API::Call(OCI_GetOCICompileVersion());
}

inline unsigned int Environment::GetRuntimeVersion()
{
    return API::Call(OCI_GetOCIRuntimeVersion());
}

inline void Environment::EnableWarnings(bool value)
{
    OCI_EnableWarnings((boolean) value);

    API::Call();
}

inline void Environment::StartDatabase(mstring db, mstring user, mstring pwd, unsigned int start_flag, unsigned
                                       int start_mode, unsigned int sess_mode, mstring spfile)
{
    API::Call(OCI_DatabaseStartup(db.c_str(), user.c_str(), pwd.c_str(), sess_mode, start_mode, start_flag, spfile.c_str() ));
}

inline void Environment::ShutdownDatabase(mstring db, mstring user, mstring pwd, unsigned int shut_flag, unsigned int shut_mode,
                                          unsigned int sess_mode)
{
    API::Call(OCI_DatabaseShutdown(db.c_str(), user.c_str(), pwd.c_str(), sess_mode, shut_mode, shut_flag ));
}

inline void Environment::ChangeUserPassword(mstring db, mstring user, mstring pwd, mstring newPassword)
{
    API::Call(OCI_SetUserPassword(db.c_str(), user.c_str(), pwd.c_str(), newPassword.c_str()));
}

inline MutexHandle Environment::CreateMutex()
{
    return API::Call(OCI_MutexCreate());
}

inline void Environment::DestroyMutex(MutexHandle mutex)
{
    API::Call(OCI_MutexFree(mutex));
}

inline void Environment::AcquireMutex(MutexHandle mutex)
{
    API::Call(OCI_MutexAcquire(mutex));
}

inline void Environment::ReleaseMutex(MutexHandle mutex)
{
    API::Call(OCI_MutexRelease(mutex));
}

inline ThreadHandle Environment::CreateThread()
{
    return API::Call(OCI_ThreadCreate());
}

inline void Environment::DestroyThread(ThreadHandle handle)
{
    API::Call(OCI_ThreadFree(handle));
}

inline void Environment::RunThread(ThreadHandle handle, ThreadProc func, void *args)
{
    API::Call(OCI_ThreadRun(handle, func, args));
}

inline void Environment::JoinThread(ThreadHandle handle)
{
    API::Call(OCI_ThreadJoin(handle));
}

inline void Environment::CreateThreadKey(mstring name, ThreadKeyFreeProc freeProc)
{
    API::Call(OCI_ThreadKeyCreate(name.c_str(), freeProc));
}

inline void Environment::ThreadKeySetValue(mstring name, void *value)
{
    API::Call(OCI_ThreadKeySetValue(name.c_str(), value));
}

inline void * Environment::ThreadKeyGetValue(mstring name)
{
    return API::Call(OCI_ThreadKeyGetValue(name.c_str()));
}

inline void Environment::SetHAHandler(HAHandlerProc handler)
{
    GetHAHandler() = handler;
}

inline HAHandlerProc & Environment::GetHAHandler()
{
    static HAHandlerProc _haHandler;

    return _haHandler;
}

inline void Environment::HAHandler(OCI_Connection *con, unsigned int source, unsigned int event, OCI_Timestamp  *time)
{
    HAHandlerProc &handler = GetHAHandler();

    if (handler)
    {
        handler(Connection(con), source, event, Timestamp(time));
    }
}

inline HandlePool & Environment::GetHandlePool()
{
    static HandlePool pool;

    return pool;
}

inline unsigned int & Environment::GetMode()
{
    static unsigned int mode;

    return mode;
}

/* --------------------------------------------------------------------------------------------- *
 * Pool
 * --------------------------------------------------------------------------------------------- */

inline Pool::Pool()
{

}

inline Pool::Pool(mstring db, mstring user, mstring pwd, unsigned int poolType,
        unsigned int minCon, unsigned int maxCon, unsigned int incrCon, unsigned int sessionMode)
{
    Open(db, user, pwd, poolType, minCon, maxCon, incrCon, sessionMode);
}

inline void Pool::Open(mstring db, mstring user, mstring pwd, unsigned int poolType,
            unsigned int minCon, unsigned int maxCon, unsigned int incrCon, unsigned int sessionMode)
{
    Release();

    Acquire(API::Call(OCI_PoolCreate(db.c_str(), user.c_str(), pwd.c_str(), poolType, sessionMode,
            minCon, maxCon, incrCon)), (HandleFreeFunc) OCI_PoolFree, 0);
}

inline void Pool::Close()
{
    Release();
}

inline Connection Pool::GetConnection(mstring sessionTag)
{
    return Connection(API::Call( OCI_PoolGetConnection(*this, sessionTag.c_str())), GetHandle());
}

inline unsigned int Pool::GetTimeout()
{
    return API::Call( OCI_PoolGetTimeout(*this));
}

inline void Pool::SetTimeout(unsigned int value)
{
    API::Call( OCI_PoolSetTimeout(*this, value));
}

inline bool Pool::GetNoWait()
{
    return (API::Call( OCI_PoolGetNoWait(*this)) == TRUE);
}

inline void Pool::SetNoWait(bool value)
{
    API::Call( OCI_PoolSetNoWait(*this, value));
}

inline unsigned int Pool::GetBusyConnectionsCount()
{
    return API::Call( OCI_PoolGetBusyCount(*this));
}

inline unsigned int Pool::GetOpenedConnectionsCount()
{
    return API::Call( OCI_PoolGetOpenedCount(*this));
}

inline unsigned int Pool::GetMinSize()
{
    return API::Call( OCI_PoolGetMin(*this));
}

inline unsigned int Pool::GetMaxSize()
{
    return API::Call( OCI_PoolGetMax(*this));
}

inline unsigned int Pool::GetIncrement()
{
    return API::Call( OCI_PoolGetIncrement(*this));
}

inline unsigned int Pool::GetStatementCacheSize()
{
    return API::Call( OCI_PoolGetStatementCacheSize(*this));
}

inline void Pool::SetStatementCacheSize(unsigned int value)
{
    API::Call( OCI_PoolSetStatementCacheSize(*this, value));
}

/* --------------------------------------------------------------------------------------------- *
 * Connection
 * --------------------------------------------------------------------------------------------- */

inline Connection::Connection()
{

}

inline Connection::Connection(mstring db, mstring user, mstring pwd, unsigned int sessionMode)
{
    Open(db, user, pwd, sessionMode);
}

inline Connection::Connection(OCI_Connection *con,  Handle *parent)
{
    if (parent)
    {
        Acquire(con, (HandleFreeFunc) OCI_ConnectionFree, parent);
    }
    else
    {
        Acquire(con);
    }
}

inline void Connection::Open(mstring db, mstring user, mstring pwd, unsigned int sessionMode)
{
    Release();

    Acquire(API::Call(OCI_ConnectionCreate(db.c_str(), user.c_str(), pwd.c_str(), (int) sessionMode)),
            (HandleFreeFunc) OCI_ConnectionFree, 0);
}

inline void Connection::Close()
{
    Release();
}

inline void Connection::Commit()
{
    API::Call(OCI_Commit(*this));
}

inline void Connection::Rollback()
{
    API::Call(OCI_Rollback(*this));
}

inline void Connection::Break()
{
    API::Call(OCI_Break(*this));
}

inline void Connection::SetAutoCommit(bool enabled)
{
    API::Call(OCI_SetAutoCommit(*this, enabled));
}

inline bool Connection::GetAutoCommit()
{
    return (API::Call(OCI_GetAutoCommit(*this)) == TRUE);
}

inline bool Connection::IsServerAlive()
{
    return (API::Call(OCI_IsConnected(*this)) == TRUE);
}

inline bool Connection::PingServer()
{
    return( API::Call(OCI_Ping(*this)) == TRUE);
}

inline mstring Connection::GetConnectionString()
{
    return API::MakeString(API::Call(OCI_GetDatabase(*this)));
}

inline mstring Connection::GetUserName()
{
    return API::MakeString(API::Call(OCI_GetUserName(*this)));
}

inline mstring Connection::GetPassword()
{
    return API::MakeString(API::Call(OCI_GetPassword(*this)));
}

inline unsigned int Connection::GetVersion()
{
    return API::Call(OCI_GetVersionConnection(*this));
}

inline mstring Connection::GetServerVersion()
{
    return API::MakeString(API::Call( OCI_GetVersionServer(*this)));
}

inline unsigned int Connection::GetServerMajorVersion()
{
    return API::Call(OCI_GetServerMajorVersion(*this));
}

inline unsigned int Connection::GetServerMinorVersion()
{
    return API::Call(OCI_GetServerMinorVersion(*this));
}

inline unsigned int Connection::GetServerRevisionVersion()
{
    return API::Call(OCI_GetServerRevisionVersion(*this));
}

inline void Connection::ChangePassword(mstring newPassword)
{
    API::Call(OCI_SetPassword(*this, newPassword.c_str()));
}

inline mstring Connection::GetSessionTag()
{
    return API::MakeString(API::Call(OCI_GetSessionTag(*this)));
}

inline void Connection::SetSessionTag(mstring tag)
{
    API::Call(OCI_SetSessionTag(*this, tag.c_str()));
}

inline Transaction Connection::GetTransaction()
{
    return Transaction(API::Call(OCI_GetTransaction(*this)));
}

inline void Connection::SetTransaction(const Transaction &transaction)
{
    API::Call(OCI_SetTransaction(*this, transaction));
}

inline void Connection::SetDefaultDateFormat(mstring format)
{
    API::Call(OCI_SetDefaultFormatDate(*this, format.c_str()));
}

inline void Connection::SetDefaultNumericFormat(mstring format)
{
    API::Call(OCI_SetDefaultFormatNumeric(*this, format.c_str()));
}

inline mstring  Connection::GetDefaultDateFormat()
{
    return API::MakeString(API::Call(OCI_GetDefaultFormatDate(*this)));
}

inline mstring  Connection::GetDefaultNumericFormat()
{
   return  API::MakeString(API::Call(OCI_GetDefaultFormatNumeric(*this)));
}

inline void Connection::EnableServerOutput(unsigned int bufsize, unsigned int arrsize, unsigned int lnsize)
{
    API::Call(OCI_ServerEnableOutput(*this, bufsize, arrsize, lnsize));
}

inline void Connection::DisableServerOutput()
{
    API::Call(OCI_ServerDisableOutput(*this));
}

inline bool Connection::GetServerOutput(mstring &line)
{
    const mtext * str = API::Call(OCI_ServerGetOutput(*this));

    line = API::MakeString(str);

    return (str != 0);
}

inline void Connection::GetServerOutput(std::vector<mstring> &lines)
{
    const mtext * str = API::Call(OCI_ServerGetOutput(*this));

    while (str)
    {
        lines.push_back(str);
        str = API::Call(OCI_ServerGetOutput(*this));
    }
}

inline void Connection::SetTrace(unsigned int trace, mstring value)
{
    API::Call(OCI_SetTrace(*this, trace, value.c_str()));
}

inline mstring Connection::GetTrace(unsigned int trace)
{
    return API::MakeString(API::Call(OCI_GetTrace(*this, trace)));
}

inline mstring Connection::GetDatabase()
{
    return API::MakeString(API::Call(OCI_GetDBName(*this)));
}

inline mstring Connection::GetInstance()
{
    return API::Call(OCI_GetInstanceName(*this));
}

inline mstring Connection::GetService()
{
    return API::MakeString(API::Call(OCI_GetServiceName(*this)));
}

inline mstring Connection::GetServer()
{
    return API::Call(OCI_GetServerName(*this));
}

inline mstring Connection::GetDomain()
{
    return API::MakeString(API::Call(OCI_GetDomainName(*this)));
}

inline Timestamp Connection::GetInstanceStartTime()
{
    return Timestamp(API::Call(OCI_GetInstanceStartTime(*this)));
}

inline unsigned int Connection::GetStatementCacheSize()
{
    return API::Call(OCI_GetStatementCacheSize(*this));
}

inline void Connection::SetStatementCacheSize(unsigned int value)
{
    API::Call(OCI_SetStatementCacheSize(*this, value));
}

inline unsigned int Connection::GetDefaultLobPrefetchSize()
{
    return API::Call(OCI_GetDefaultLobPrefetchSize  (*this));
}

inline void Connection::SetDefaultLobPrefetchSize(unsigned int value)
{
    API::Call(OCI_SetDefaultLobPrefetchSize(*this, value));
}

inline bool Connection::IsTAFCapable()
{
    return (API::Call(OCI_IsTAFCapable(*this)) == TRUE);
}

inline void Connection::SetTAFHandler(TAFHandlerProc handler)
{
    _tafHandler = handler;

    API::Call(OCI_SetTAFHandler(*this, (POCI_TAF_HANDLER ) (_tafHandler != 0 ? TAFHandler : 0 )));
}

/* --------------------------------------------------------------------------------------------- *
 * Transaction
 * --------------------------------------------------------------------------------------------- */

inline Transaction::Transaction(const Connection &connection, unsigned int timeout, unsigned int mode, OCI_XID *pxid)
{
    Acquire(API::Call(OCI_TransactionCreate(connection, timeout, mode, pxid)), (HandleFreeFunc) OCI_TransactionFree, 0);
}

inline Transaction::Transaction(OCI_Transaction *trans)
{
    Acquire(trans);
}

inline void Transaction::Prepare()
{
    API::Call(OCI_TransactionPrepare(*this));
}

inline void Transaction::Start()
{
    API::Call(OCI_TransactionStart(*this));
}

inline void Transaction::Stop()
{
    API::Call(OCI_TransactionStop(*this));
}

inline void Transaction::Resume()
{
    API::Call(OCI_TransactionResume(*this));
}

inline void Transaction::Forget()
{
    API::Call(OCI_TransactionForget(*this));
}

inline unsigned int Transaction::GetMode()
{
    return API::Call(OCI_TransactionGetMode(*this));
}

inline unsigned int Transaction::GetTimeout()
{
    return API::Call(OCI_TransactionGetTimeout(*this));
}

/* --------------------------------------------------------------------------------------------- *
 * Date
 * --------------------------------------------------------------------------------------------- */

inline Date::Date()
{
    Acquire(API::Call(OCI_DateCreate(NULL)), (HandleFreeFunc) OCI_DateFree, 0);
}

inline Date::Date(OCI_Date *pDate)
{
    Acquire(pDate);
}

inline void Date::Assign(const Date& other)
{
    API::Call(OCI_DateAssign(*this, other));
}

inline int Date::Compare(const Date& other)
{
    return API::Call(OCI_DateCompare(*this, other));
}

inline int Date::DaysBetween(const Date& other)
{
    return API::Call(OCI_DateDaysBetween(*this, other));
}

inline void Date::SetDate(int year, int month, int day)
{
    API::Call(OCI_DateSetDate(*this, year, month, day));
}

inline void Date::SetTime(int hour, int min, int sec)
{
    API::Call(OCI_DateSetTime(*this, hour, min , sec));
}

inline void Date::SetDateTime(int year, int month, int day, int hour, int min, int sec)
{
    API::Call(OCI_DateSetDateTime(*this, year, month, day, hour, min , sec));
}

inline void Date::GetDate(int *year, int *month, int *day)
{
    API::Call(OCI_DateGetDate(*this, year, month, day));
}

inline void Date::GetTime(int *hour, int *min, int *sec)
{
    API::Call(OCI_DateGetTime(*this, hour, min , sec));
}

inline void Date::GetDateTime(int *year, int *month, int *day, int *hour, int *min, int *sec)
{
    API::Call(OCI_DateGetDateTime(*this, year, month, day, hour, min , sec));
}

inline void Date::AddDays(int days)
{
    API::Call(OCI_DateAddDays(*this, days));
}

inline void Date::AddMonths(int months)
{
    OCI_DateAddMonths(*this, months);
}

inline void Date::SysDate()
{
   API::Call(OCI_DateSysDate(*this));
}

inline void Date::NextDay(mstring day)
{
   API::Call(OCI_DateNextDay(*this, day.c_str()));
}

inline void Date::LastDay()
{
    API::Call(OCI_DateLastDay(*this));
}

inline void Date::ChangeTimeZone(mstring tzSrc, mstring tzDst)
{
    API::Call(OCI_DateZoneToZone(*this, tzSrc.c_str(), tzDst.c_str()));
}

inline void Date::FromString(mstring data, mstring format)
{
   API::Call(OCI_DateFromText(*this, data.c_str(), format.c_str()));
}

inline mstring Date::ToString(mstring format)
{
    int size = OCI_SIZE_BUFFER;

    ManagedBuffer<mtext> buffer = new mtext[size+1];

    API::Call(OCI_DateToText(*this, format.c_str(), size, (mtext *) buffer));

    return API::MakeString((mtext *) buffer);
}

inline Date::operator mstring()
{
    return ToString();
}

/* --------------------------------------------------------------------------------------------- *
 * Interval
 * --------------------------------------------------------------------------------------------- */

inline Interval::Interval(unsigned int type)
{
    Acquire(API::Call(OCI_IntervalCreate(NULL, type)), (HandleFreeFunc) OCI_IntervalFree, 0);
}

inline Interval::Interval(OCI_Interval *pInterval)
{
    Acquire(pInterval);
}

inline void Interval::Assign(const Interval& other)
{
   API::Call(OCI_IntervalAssign(*this, other));
}

inline int Interval::Compare(const Interval& other)
{
    return API::Call(OCI_IntervalCompare(*this, other));
}

inline unsigned int Interval::GetType()   
{
    return API::Call(OCI_IntervalGetType(*this));
}

inline void Interval::Add(const Interval& other)
{
     API::Call(OCI_IntervalAdd(*this, other));
}

inline void Interval::Substract(const Interval& other)
{
    API::Call(OCI_IntervalSubtract(*this, other));
}

inline void Interval::GetDaySecond(int *day, int *hour, int *min, int *sec, int *fsec)
{
    API::Call(OCI_IntervalGetDaySecond(*this, day, hour, min, sec, fsec));
}

inline void Interval::SetDaySecond(int day, int hour, int min, int sec, int fsec)
{
    API::Call(OCI_IntervalSetDaySecond(*this, day, hour, min, sec, fsec));
}

inline void Interval::GetYearMonth(int *year, int *month)
{
    API::Call(OCI_IntervalGetYearMonth(*this, year, month));
}
inline void Interval::SetYearMonth(int year, int month)
{
    API::Call(OCI_IntervalSetYearMonth(*this, year, month));
}

inline void Interval::FromTimeZone(mstring timeZone)
{
    API::Call(OCI_IntervalFromTimeZone(*this, timeZone.c_str()));
}

inline void Interval::FromString(mstring data, mstring format)
{
    API::Call(OCI_IntervalFromText(*this, format.c_str()));
}

inline mstring Interval::ToString(int leadingPrecision, int fractionPrecision)
{
    int size = OCI_SIZE_BUFFER;

    ManagedBuffer<mtext> buffer = new mtext[size+1];

    API::Call(OCI_IntervalToText(*this, leadingPrecision, fractionPrecision,  size, (mtext *) buffer));

    return API::MakeString((mtext *) buffer);
}

inline Interval::operator mstring()
{
    return ToString();
}

/* --------------------------------------------------------------------------------------------- *
 * Timestamp
 * --------------------------------------------------------------------------------------------- */

inline Timestamp::Timestamp(unsigned int type)
{
    Acquire(API::Call(OCI_TimestampCreate(NULL, type)), (HandleFreeFunc) OCI_TimestampFree, 0);
}

inline Timestamp::Timestamp(OCI_Timestamp *pTimestamp)
{
    Acquire(pTimestamp);
}

inline void Timestamp::Assign(const Timestamp& other)
{
    API::Call(OCI_TimestampAssign(*this, other));
}

inline int Timestamp::Compare(const Timestamp& other)
{
    return API::Call(OCI_TimestampCompare(*this, other));
}

inline unsigned int Timestamp::GetType()
{
    return API::Call(OCI_TimestampGetType(*this));
}

inline void Timestamp::Construct(int year, int month, int day, int hour, int min, int sec, int fsec, mstring timeZone)
{
    API::Call(OCI_TimestampConstruct(*this, year, month, day, hour, min,sec, fsec, timeZone.c_str()));
}

inline void Timestamp::Convert(const Timestamp& other)
{
    API::Call(OCI_TimestampConvert(*this, other));
}

inline void Timestamp::GetDate(int *year, int *month, int *day)
{
    API::Call(OCI_TimestampGetDate(*this, year, month, day));
}

inline void Timestamp::GetTime(int *hour, int *min, int *sec, int *fsec)
{
    API::Call(OCI_TimestampGetTime(*this, hour, min, sec, fsec));
}

inline void Timestamp::GetDateTime(int *year, int *month, int *day, int *hour, int *min, int *sec, int *fsec)
{
    API::Call(OCI_TimestampGetDateTime(*this, year, month, day, hour, min, sec, fsec));
}

inline mstring Timestamp::GetTimeZone()
{
    int size = OCI_SIZE_BUFFER;

    ManagedBuffer<mtext> buffer = new mtext[size+1];

    API::Call(OCI_TimestampGetTimeZoneName(*this,  size, (mtext *) buffer));

    return API::MakeString((mtext *) buffer);
}

inline void Timestamp::GetTimeZoneOffset(int *hour, int *min)
{
    API::Call(OCI_TimestampGetTimeZoneOffset(*this, hour, min));
}

inline void Timestamp::AddInterval(const Interval& other)
{
    API::Call(OCI_TimestampIntervalAdd(*this, other));
}

inline void Timestamp::SubstractInterval(const Interval& other)
{
    API::Call(OCI_TimestampIntervalSub(*this, other));
}

inline void Timestamp::Substract(const Timestamp &other, Interval &result)
{
    API::Call(OCI_TimestampSubtract(*this, other, result));
}

inline void Timestamp::SysTimestamp()
{
    API::Call(OCI_TimestampSysTimestamp(*this));
}

inline void Timestamp::FromString(mstring data, mstring format)
{
    API::Call(OCI_TimestampFromText(*this, data.c_str(), format.c_str()));
}

inline mstring Timestamp::ToString(mstring format, int precision)
{
    int size = OCI_SIZE_BUFFER;

    ManagedBuffer<mtext> buffer = new mtext[size+1];

    API::Call(OCI_TimestampToText(*this, format.c_str(), size, (mtext *) buffer, precision));

    return API::MakeString((mtext *) buffer);
}

inline Timestamp::operator mstring()
{
    return ToString();
}

/* --------------------------------------------------------------------------------------------- *
 * Clob
 * --------------------------------------------------------------------------------------------- */

inline Clob::Clob(const Connection &connection)
{
    Acquire(API::Call(OCI_LobCreate(connection, OCI_CLOB)), (HandleFreeFunc) OCI_LobFree, connection.GetHandle());
}

inline Clob::Clob(OCI_Lob *pLob)
{
    Acquire(pLob);
}

inline dstring Clob::Read(unsigned int size)
{
    ManagedBuffer<dtext> buffer = new dtext[size+1];

    size = API::Call(OCI_LobRead(*this, (void *) buffer, size));

    return API::MakeString( (const dtext *) buffer);
}

inline unsigned int Clob::Write(dstring content)
{
    return API::Call(OCI_LobWrite(*this, (void *) content.c_str(), content.size()));
}

inline unsigned int Clob::Append(dstring content)
{
    return API::Call(OCI_LobAppend(*this, (void *) content.c_str(), content.size()));
}

inline unsigned int Clob::Seek(unsigned int seekMode, big_uint offset)
{
    return API::Call(OCI_LobSeek(*this, offset, (unsigned int) seekMode));
}

inline void Clob::Append(const Clob &other)
{
    API::Call(OCI_LobAppendLob(*this, other));
}

inline void Clob::Assign(const Clob &other)
{
    API::Call(OCI_LobAssign(*this, other));
}

inline bool Clob::Equals(const Clob &other)
{
    return (API::Call(OCI_LobIsEqual(*this, other)) == TRUE);
}

inline big_uint Clob::GetOffset()
{
    return API::Call(OCI_LobGetOffset(*this));
}

inline big_uint Clob::GetLength()
{
    return API::Call(OCI_LobGetLength(*this));
}

inline big_uint Clob::GetMaxSize()
{
    return API::Call(OCI_LobGetMaxSize(*this));
}

inline big_uint Clob::GetChunkSize()
{
    return API::Call(OCI_LobGetChunkSize(*this));
}

inline void Clob::Truncate(big_uint size)
{
    API::Call(OCI_LobTruncate(*this, size));
}

inline big_uint Clob::Erase(big_uint offset, big_uint size)
{
    return API::Call(OCI_LobErase(*this, offset, size));
}

inline void Clob::Copy(Clob &dest, big_uint offset, big_uint offsetDest, big_uint size)
{
    API::Call(OCI_LobCopy(dest, *this, offsetDest, offset, size));
}

inline bool Clob::IsTemporary()
{
    return (API::Call(OCI_LobIsTemporary(*this)) == TRUE);
}

inline void Clob::Open(unsigned int mode)
{
    API::Call(OCI_LobOpen(*this, mode));
}

inline void Clob::Flush()
{
    API::Call(OCI_LobFlush(*this));
}

inline void Clob::Close()
{
    API::Call(OCI_LobClose(*this));
}

inline void Clob::EnableBuffering(bool value)
{
    API::Call(OCI_LobEnableBuffering(*this, value));
}

/* --------------------------------------------------------------------------------------------- *
 * Blob
 * --------------------------------------------------------------------------------------------- */

inline Blob::Blob(const Connection &connection)
{
    Acquire(API::Call(OCI_LobCreate(connection, OCI_BLOB)), (HandleFreeFunc) OCI_LobFree, connection.GetHandle());
}

inline Blob::Blob(OCI_Lob *pLob)
{
    Acquire(pLob);
}

inline unsigned int Blob::Read(void *buffer, unsigned int size)
{
    return API::Call(OCI_LobRead(*this, buffer, size));
}

inline unsigned int Blob::Write(void *buffer, unsigned int size)
{
    return API::Call(OCI_LobWrite(*this, buffer, size));
}

inline unsigned int Blob::Append(void *buffer, unsigned int size)
{
    return API::Call(OCI_LobAppend(*this, buffer, size));
}

inline unsigned int Blob::Seek(unsigned int seekMode, big_uint offset)
{
    return API::Call(OCI_LobSeek(*this, offset, (unsigned int) seekMode));
}

inline void Blob::Append(const Blob &other)
{
    API::Call(OCI_LobAppendLob(*this, other));
}

inline void Blob::Assign(const Blob &other)
{
    API::Call(OCI_LobAssign(*this, other));
}

inline bool Blob::Equals(const Blob &other)
{
    return (API::Call(OCI_LobIsEqual(*this, other)) == TRUE);
}

inline big_uint Blob::GetOffset()
{
    return API::Call(OCI_LobGetOffset(*this));
}

inline big_uint Blob::GetLength()
{
    return API::Call(OCI_LobGetLength(*this));
}

inline big_uint Blob::GetMaxSize()
{
    return API::Call(OCI_LobGetMaxSize(*this));
}

inline big_uint Blob::GetChunkSize()
{
    return API::Call(OCI_LobGetChunkSize(*this));
}

inline void Blob::Truncate(big_uint size)
{
    API::Call(OCI_LobTruncate(*this, size));
}

inline big_uint Blob::Erase(big_uint offset, big_uint size)
{
    return API::Call(OCI_LobErase(*this, offset, size));
}

inline void Blob::Copy(Blob &dest, big_uint offset, big_uint offsetDest, big_uint size)
{
    API::Call(OCI_LobCopy(dest, *this, offsetDest, offset, size));
}

inline bool Blob::IsTemporary()
{
    return (API::Call(OCI_LobIsTemporary(*this)) == TRUE);
}

inline void Blob::Open(unsigned int mode)
{
    API::Call(OCI_LobOpen(*this, mode));
}

inline void Blob::Flush()
{
    API::Call(OCI_LobFlush(*this));
}

inline void Blob::Close()
{
    API::Call(OCI_LobClose(*this));
}

inline void Blob::EnableBuffering(bool value)
{
    API::Call(OCI_LobEnableBuffering(*this, value));
}

/* --------------------------------------------------------------------------------------------- *
 * File
 * --------------------------------------------------------------------------------------------- */

inline File::File(const Connection &connection)
{
    Acquire(API::Call(OCI_FileCreate(connection, OCI_BFILE)), (HandleFreeFunc) OCI_FileFree, connection.GetHandle());
}

inline File::File(const Connection &connection, mstring directory, mstring name)
{
    Acquire(API::Call(OCI_FileCreate(connection, OCI_BFILE)), (HandleFreeFunc) OCI_FileFree, connection.GetHandle());

    SetInfos(directory, name);
}

inline File::File(OCI_File *pFile)
{
    Acquire(pFile);
}

inline unsigned int File::Read(void *buffer, unsigned int size)
{
    return API::Call(OCI_FileRead(*this, buffer, size));
}

inline unsigned int File::Seek(unsigned int seekMode, big_uint offset)
{
    return API::Call(OCI_FileSeek(*this, offset, (unsigned int) seekMode));
}

inline void File::Assign(const File &other)
{
    API::Call(OCI_FileAssign(*this, other));
}

inline bool File::Equals(const File &other)
{
    return (API::Call(OCI_FileIsEqual(*this, other)) == TRUE);
}

inline big_uint File::GetOffset()
{
    return API::Call(OCI_FileGetOffset(*this));
}

inline big_uint File::GetSize()
{
    return API::Call(OCI_FileGetSize(*this));
}

inline bool File::Exists()
{
    return (API::Call(OCI_FileExists(*this)) == TRUE);
}

inline void File::SetInfos(mstring directory, mstring name)
{
    API::Call(OCI_FileSetName(*this, directory.c_str(), name.c_str()));
}

inline mstring File::GetName()
{
    return API::MakeString(API::Call(OCI_FileGetName(*this)));
}

inline mstring File::GetDirectory()
{
    return API::MakeString(API::Call(OCI_FileGetDirectory(*this)));
}

inline void File::Open()
{
    API::Call(OCI_FileOpen(*this));
}

inline bool File::IsOpened()
{
    return (API::Call(OCI_FileIsOpen(*this)) == TRUE);
}

inline void File::Close()
{
    API::Call(OCI_FileClose(*this));
}

/* --------------------------------------------------------------------------------------------- *
 * TypeInfo
 * --------------------------------------------------------------------------------------------- */

inline TypeInfo::TypeInfo(const Connection &connection, mstring name, unsigned int type)
{
    Acquire(API::Call(OCI_TypeInfoGet(connection, name.c_str(), type)), (HandleFreeFunc) 0, connection.GetHandle());
}

inline TypeInfo::TypeInfo(OCI_TypeInfo *pTypeInfo)
{
    Acquire(pTypeInfo);
}

inline unsigned int TypeInfo::GetType()
{
    return API::Call(OCI_TypeInfoGetType(*this));
}

inline mstring TypeInfo::GetName()
{
    return API::Call(OCI_TypeInfoGetName(*this));
}

inline Connection TypeInfo::GetConnection()
{
    return Connection(API::Call(OCI_TypeInfoGetConnection(*this)));
}

inline Connection TypeInfo::GetConnection() const
{
    return Connection(API::Call(OCI_TypeInfoGetConnection(*this)));
}

inline unsigned int TypeInfo::GetColumnCount()
{
    return API::Call(OCI_TypeInfoGetColumnCount(*this));
}

inline Column TypeInfo::GetColumn(unsigned int index)
{
    return Column(API::Call(OCI_TypeInfoGetColumn(*this, index)));
}

/* --------------------------------------------------------------------------------------------- *
 * Object
 * --------------------------------------------------------------------------------------------- */

inline Object::Object(const TypeInfo &typeInfo)
{
    Connection connection = typeInfo.GetConnection();
    Acquire(API::Call(OCI_ObjectCreate(connection, typeInfo)), (HandleFreeFunc) OCI_ObjectFree, connection.GetHandle());
}

inline TypeInfo Object::GetTypeInfo()
{
    return TypeInfo(API::Call(OCI_ObjectGetTypeInfo(*this)));
}

template<>
inline short Object::Get<short>(mstring name)
{
    return API::Call(OCI_ObjectGetShort(*this, name.c_str()));
}

template<>
inline unsigned short Object::Get<unsigned short>(mstring name)
{
    return API::Call(OCI_ObjectGetUnsignedShort(*this, name.c_str()));
}

template<>
inline int Object::Get<int>(mstring name)
{
    return API::Call(OCI_ObjectGetInt(*this, name.c_str()));
}

template<>
inline unsigned int Object::Get<unsigned int>(mstring name)
{
    return API::Call(OCI_ObjectGetUnsignedInt(*this, name.c_str()));
}

template<>
inline big_int Object::Get<big_int>(mstring name)
{
    return API::Call(OCI_ObjectGetBigInt(*this, name.c_str()));
}

template<>
inline big_uint Object::Get<big_uint>(mstring name)
{
    return API::Call(OCI_ObjectGetUnsignedBigInt(*this, name.c_str()));
}

template<>
inline float Object::Get<float>(mstring name)
{
    return API::Call(OCI_ObjectGetFloat(*this, name.c_str()));
}

template<>
inline double Object::Get<double>(mstring name)
{
    return API::Call(OCI_ObjectGetDouble(*this, name.c_str()));
}

template<>
inline dstring Object::Get<dstring>(mstring name)
{
    return API::MakeString(API::Call(OCI_ObjectGetString(*this,name.c_str())));
}

template<>
inline Date Object::Get<Date>(mstring name)
{
    return API::Call(OCI_ObjectGetDate(*this,name.c_str()));
}

template<>
inline Timestamp Object::Get<Timestamp>(mstring name)
{
    return API::Call(OCI_ObjectGetTimestamp(*this,name.c_str()));
}

template<>
inline Interval Object::Get<Interval>(mstring name)
{
    return API::Call(OCI_ObjectGetInterval(*this,name.c_str()));
}

template<>
inline Object Object::Get<Object>(mstring name)
{
    return API::Call(OCI_ObjectGetObject(*this,name.c_str()));
}

template<>
inline Collection Object::Get<Collection>(mstring name)
{
    return API::Call(OCI_ObjectGetColl(*this,name.c_str()));
}

template<>
inline Reference Object::Get<Reference>(mstring name)
{
    return API::Call(OCI_ObjectGetRef(*this,name.c_str()));
}

template<>
inline Clob Object::Get<Clob>(mstring name)
{
    return API::Call(OCI_ObjectGetLob(*this,name.c_str()));
}

template<>
inline Blob Object::Get<Blob>(mstring name)
{
    return API::Call(OCI_ObjectGetLob(*this,name.c_str()));
}

template<>
inline File Object::Get<File>(mstring name)
{
    return API::Call(OCI_ObjectGetFile(*this,name.c_str()));
}

template<>
inline void Object::Get<BufferPointer>(mstring name, BufferPointer &value, unsigned int &size)
{
    API::Call(OCI_ObjectGetRaw(*this,name.c_str(), value, size));
}

template<>
inline void Object::Set<short>(mstring name, const short &value)
{
    API::Call(OCI_ObjectSetShort(*this, name.c_str(), value));
}

template<>
inline void Object::Set<unsigned short>(mstring name, const unsigned short &value)
{
    API::Call(OCI_ObjectSetUnsignedShort(*this, name.c_str(), value));
}

template<>
inline void Object::Set<int>(mstring name, const int &value)
{
    API::Call(OCI_ObjectSetInt(*this, name.c_str(), value));
}

template<>
inline void Object::Set<unsigned int>(mstring name, const unsigned int &value)
{
    API::Call(OCI_ObjectSetUnsignedInt(*this, name.c_str(), value));
}

template<>
inline void Object::Set<big_int>(mstring name, const big_int &value)
{
    API::Call(OCI_ObjectSetBigInt(*this, name.c_str(), value));
}

template<>
inline void Object::Set<big_uint>(mstring name, const big_uint &value)
{
    API::Call(OCI_ObjectSetUnsignedBigInt(*this, name.c_str(), value));
}

template<>
inline void Object::Set<float>(mstring name, const float &value)
{
    API::Call(OCI_ObjectSetFloat(*this, name.c_str(), value));
}

template<>
inline void Object::Set<double>(mstring name, const double &value)
{
    API::Call(OCI_ObjectSetDouble(*this, name.c_str(), value));
}

template<>
inline void Object::Set<dstring>(mstring name, const dstring &value)
{
    API::Call(OCI_ObjectSetString(*this, name.c_str(), value.c_str()));
}

template<>
inline void Object::Set<Date>(mstring name, const Date &value)
{
    API::Call(OCI_ObjectSetDate(*this, name.c_str(), value));
}

template<>
inline void Object::Set<Timestamp>(mstring name, const Timestamp &value)
{
    API::Call(OCI_ObjectSetTimestamp(*this, name.c_str(), value));
}

template<>
inline void Object::Set<Interval>(mstring name, const Interval &value)
{
    API::Call(OCI_ObjectSetInterval(*this, name.c_str(), value));
}

template<>
inline void Object::Set<Object>(mstring name, const Object &value)
{
    API::Call(OCI_ObjectSetObject(*this, name.c_str(), value));
}

template<>
inline void Object::Set<Collection>(mstring name, const Collection &value)
{
    API::Call(OCI_ObjectSetColl(*this, name.c_str(), value));
}

template<>
inline void Object::Set<Reference>(mstring name, const Reference &value)
{
    API::Call(OCI_ObjectSetRef(*this, name.c_str(), value));
}

template<>
inline void Object::Set<Clob>(mstring name, const Clob &value)
{
    API::Call(OCI_ObjectSetLob(*this, name.c_str(), value));
}

template<>
inline void Object::Set<Blob>(mstring name, const Blob &value)
{
    API::Call(OCI_ObjectSetLob(*this, name.c_str(), value));
}

template<>
inline void Object::Set<File>(mstring name, const File &value)
{
    API::Call(OCI_ObjectSetFile(*this, name.c_str(), value));
}

template<>
inline void Object::Set<BufferPointer>(mstring name, const BufferPointer & value, unsigned int size)
{
    API::Call(OCI_ObjectSetRaw(*this, name.c_str(), value, size));
}

/* --------------------------------------------------------------------------------------------- *
 * Reference
 * --------------------------------------------------------------------------------------------- */

inline Reference::Reference(const Connection &connection, const TypeInfo &typeInfo)
{
    Acquire(API::Call(OCI_RefCreate(connection, typeInfo)), (HandleFreeFunc) OCI_RefFree, connection.GetHandle());
}

inline TypeInfo Reference::GetTypeInfo()
{
    return TypeInfo(API::Call(OCI_RefGetTypeInfo(*this)));
}

inline Object Reference::GetObject()
{
    return Object(API::Call(OCI_RefGetObject(*this)));
}

inline void Reference::Assign(const Reference& other)
{
   API::Call(OCI_RefAssign(*this, other));
}

inline bool Reference::IsNullReference()
{
    return (API::Call(OCI_RefIsNull(*this)) == TRUE);
}

inline void Reference::SetNulReference()
{
    API::Call(OCI_RefSetNull(*this));
}

inline mstring Reference::ToString()
{
    unsigned int size =  API::Call(OCI_RefGetHexSize(*this));

    ManagedBuffer<mtext> buffer = new dtext [ size+1 ];

    API::Call(OCI_RefToText(*this, size, (mtext *) buffer));

    return API::MakeString( (const dtext *) buffer);

}

/* --------------------------------------------------------------------------------------------- *
 * Collection
 * --------------------------------------------------------------------------------------------- */

inline Collection::Collection(const TypeInfo &typeInfo)
{
    Acquire(API::Call(OCI_CollCreate(typeInfo)), (HandleFreeFunc) OCI_CollFree, typeInfo.GetConnection().GetHandle());
}

inline Collection::Collection(OCI_Coll *pColl)
{
     Acquire(pColl);
}

inline void Collection::Assign(const Collection& other)
{
    API::Call(OCI_CollAssign(*this, other));
}

inline TypeInfo Collection::GetTypeInfo()
{
    return TypeInfo(API::Call(OCI_CollGetTypeInfo(*this)));
}

inline unsigned int Collection::GetType()
{
    return API::Call(OCI_CollGetType(*this));
}

inline unsigned int Collection::GetMax()
{
    return API::Call(OCI_CollGetMax(*this));
}

inline unsigned int Collection::GetSize()

{
    return API::Call(OCI_CollGetSize(*this));
}

inline void Collection::Truncate(unsigned int size)
{
    API::Call(OCI_CollTrim(*this, size));
}

inline void Collection::Clear()
{
    API::Call(OCI_CollClear(*this));
}

template <class TDataType>
inline TDataType Collection::Get(unsigned int index)
{
    return GetElem<TDataType>(API::Call(OCI_CollGetAt(*this, index)));
}

template <class TDataType>
inline void Collection::Set(unsigned int index, const TDataType &data)
{
    OCI_Elem * elem = API::Call(OCI_CollGetAt(*this, index));

    SetElem<TDataType>(elem, data);

    API::Call(OCI_CollSetAt(*this, index, elem));
}

template <class TDataType>
inline void Collection::Append(const TDataType &data)
{
    OCI_Elem * elem = API::Call(OCI_ElemCreate(OCI_CollGetTypeInfo(*this)));

    SetElem<TDataType>(elem, data);

    API::Call(OCI_CollAppend(*this, elem));
    API::Call(OCI_ElemFree(elem));
}

template<>
inline short Collection::GetElem<short>(OCI_Elem *elem)
{
    return API::Call(OCI_ElemGetShort(elem));
}

template<>
inline unsigned short Collection::GetElem<unsigned short>(OCI_Elem *elem)
{
    return API::Call(OCI_ElemGetUnsignedShort(elem));
}

template<>
inline int Collection::GetElem<int>(OCI_Elem *elem)
{
    return API::Call(OCI_ElemGetInt(elem));
}

template<>
inline unsigned int Collection::GetElem<unsigned int>(OCI_Elem *elem)
{
    return API::Call(OCI_ElemGetUnsignedInt(elem));
}

template<>
inline big_int Collection::GetElem<big_int>(OCI_Elem *elem)
{
    return API::Call(OCI_ElemGetBigInt(elem));
}

template<>
inline big_uint Collection::GetElem<big_uint>(OCI_Elem *elem)
{
    return API::Call(OCI_ElemGetUnsignedBigInt(elem));
}

template<>
inline float Collection::GetElem<float>(OCI_Elem *elem)
{
    return API::Call(OCI_ElemGetFloat(elem));
}

template<>
inline double Collection::GetElem<double>(OCI_Elem *elem)
{
    return API::Call(OCI_ElemGetDouble(elem));
}

template<>
inline dstring Collection::GetElem<dstring>(OCI_Elem *elem)
{
    return API::MakeString(API::Call(OCI_ElemGetString(elem)));
}

template<>
inline Date Collection::GetElem<Date>(OCI_Elem *elem)
{
    return API::Call(OCI_ElemGetDate(elem));
}

template<>
inline Timestamp Collection::GetElem<Timestamp>(OCI_Elem *elem)
{
    return API::Call(OCI_ElemGetTimestamp(elem));
}

template<>
inline Interval Collection::GetElem<Interval>(OCI_Elem *elem)
{
    return API::Call(OCI_ElemGetInterval(elem));
}

template<>
inline Object Collection::GetElem<Object>(OCI_Elem *elem)
{
    return API::Call(OCI_ElemGetObject(elem));
}

template<>
inline Collection Collection::GetElem<Collection>(OCI_Elem *elem)
{
    return API::Call(OCI_ElemGetColl(elem));
}

template<>
inline Reference Collection::GetElem<Reference>(OCI_Elem *elem)
{
    return API::Call(OCI_ElemGetRef(elem));
}

template<>
inline Clob Collection::GetElem<Clob>(OCI_Elem *elem)
{
    return API::Call(OCI_ElemGetLob(elem));
}

template<>
inline Blob Collection::GetElem<Blob>(OCI_Elem *elem)
{
    return API::Call(OCI_ElemGetLob(elem));
}

template<>
inline File Collection::GetElem<File>(OCI_Elem *elem)
{
    return API::Call(OCI_ElemGetFile(elem));
}

template<>
inline void Collection::SetElem<short>(OCI_Elem *elem, const short &value)
{
    API::Call(OCI_ElemSetShort(elem, value));
}

template<>
inline void Collection::SetElem<unsigned short>(OCI_Elem *elem, const unsigned short &value)
{
    API::Call(OCI_ElemSetUnsignedShort(elem, value));
}

template<>
inline void Collection::SetElem<int>(OCI_Elem *elem, const int &value)
{
    API::Call(OCI_ElemSetInt(elem, value));
}

template<>
inline void Collection::SetElem<unsigned int>(OCI_Elem *elem, const unsigned int &value)
{
    API::Call(OCI_ElemSetUnsignedInt(elem, value));
}

template<>
inline void Collection::SetElem<big_int>(OCI_Elem *elem, const big_int &value)
{
    API::Call(OCI_ElemSetBigInt(elem, value));
}

template<>
inline void Collection::SetElem<big_uint>(OCI_Elem *elem, const big_uint &value)
{
    API::Call(OCI_ElemSetUnsignedBigInt(elem, value));
}

template<>
inline void Collection::SetElem<float>(OCI_Elem *elem, const float &value)
{
    API::Call(OCI_ElemSetFloat(elem, value));
}

template<>
inline void Collection::SetElem<double>(OCI_Elem *elem, const double &value)
{
    API::Call(OCI_ElemSetDouble(elem, value));
}


template <>
inline void Collection::SetElem<dstring>(OCI_Elem *elem, const dstring &value)
{
    API::Call(OCI_ElemSetString(elem, value.c_str()));
}


template<>
inline void Collection::SetElem<Date>(OCI_Elem *elem, const Date &value)
{
    API::Call(OCI_ElemSetDate(elem, value));
}

template<>
inline void Collection::SetElem<Timestamp>(OCI_Elem *elem, const Timestamp &value)
{
    API::Call(OCI_ElemSetTimestamp(elem, value));
}

template<>
inline void Collection::SetElem<Interval>(OCI_Elem *elem, const Interval &value)
{
    API::Call(OCI_ElemSetInterval(elem, value));
}

template<>
inline void Collection::SetElem<Object>(OCI_Elem *elem, const Object &value)
{
    API::Call(OCI_ElemSetObject(elem, value));
}

template<>
inline void Collection::SetElem<Collection>(OCI_Elem *elem, const Collection &value)
{
    API::Call(OCI_ElemSetColl(elem, value));
}

template<>
inline void Collection::SetElem<Reference>(OCI_Elem *elem, const Reference &value)
{
    API::Call(OCI_ElemSetRef(elem, value));
}

template<>
inline void Collection::SetElem<Clob>(OCI_Elem *elem, const Clob &value)
{
    API::Call(OCI_ElemSetLob(elem, value));
}

template<>
inline void Collection::SetElem<Blob>(OCI_Elem *elem, const Blob &value)
{
    API::Call(OCI_ElemSetLob(elem, value));
}

template<>
inline void Collection::SetElem<File>(OCI_Elem *elem, const File &value)
{
    API::Call(OCI_ElemSetFile(elem, value));
}

/* --------------------------------------------------------------------------------------------- *
 * CollectionIterator
 * --------------------------------------------------------------------------------------------- */

inline CollectionIterator::CollectionIterator(const Collection &collection)
{
    Acquire(API::Call(OCI_IterCreate(collection)), (HandleFreeFunc) OCI_IterFree, collection.GetHandle());
}

inline bool CollectionIterator::Next()
{
   return (API::Call(OCI_IterGetNext(*this)) != 0);
}

inline bool CollectionIterator::Prev()
{
   return (API::Call(OCI_IterGetPrev(*this)) != 0);
}

template <class TDataType>
inline void CollectionIterator::Set(TDataType &value)
{
    Collection::SetElem<TDataType>(API::Call(OCI_IterGetCurrent(*this)), value);
}

template <class TDataType>
inline TDataType CollectionIterator::Get()
{
    return Collection::GetElem<TDataType>(API::Call(OCI_IterGetCurrent(*this)));
}

/* --------------------------------------------------------------------------------------------- *
 * CLong
 * --------------------------------------------------------------------------------------------- */

inline CLong::CLong(const Statement &statement)
{
    Acquire(API::Call(OCI_LongCreate(statement, OCI_CLONG)), (HandleFreeFunc) OCI_LongFree, statement.GetHandle());
}

inline CLong::CLong(OCI_Long *pLong)
{
    Acquire(pLong);
}

inline dstring CLong::Read(unsigned int size)
{
    ManagedBuffer<dtext> buffer = new dtext[size+1];

    size = API::Call(OCI_LongRead(*this, (void *) buffer, size));

    return API::MakeString( (const dtext *) buffer);
}

inline unsigned int CLong::Write(dstring content)
{
    return API::Call(OCI_LongWrite(*this, (void *) content.c_str(), content.size()));
}

inline unsigned int CLong::GetSize()
{
    return API::Call(OCI_LongGetSize(*this));
}

inline dstring CLong::GetContent()
{
    return API::MakeString((dtext *) API::Call(OCI_LongGetBuffer(*this)));
}

/* --------------------------------------------------------------------------------------------- *
 * BLong
 * --------------------------------------------------------------------------------------------- */

inline BLong::BLong(const Statement &statement)
{
    Acquire(API::Call(OCI_LongCreate(statement, OCI_CLONG)), (HandleFreeFunc) OCI_LongFree, statement.GetHandle());
}

inline BLong::BLong(OCI_Long *pLong)
{
    Acquire(pLong);
}

inline unsigned int BLong::Read(void *buffer, unsigned int size)
{
    return API::Call(OCI_LongRead(*this ,buffer, size));
}

inline unsigned int BLong::Write(void *buffer, unsigned int size)
{
    return API::Call(OCI_LongWrite(*this ,buffer, size));
}

inline unsigned int BLong::GetSize()
{
    return API::Call(OCI_LongGetSize(*this));
}

inline void * BLong::GetContent()
{
    return  API::Call(OCI_LongGetBuffer(*this));
}

/* --------------------------------------------------------------------------------------------- *
 * BindValue
 * --------------------------------------------------------------------------------------------- */

template<class TValueType>
inline BindValue<TValueType>::BindValue()
{

}

template<class TValueType>
inline BindValue<TValueType>::BindValue(TValueType value) : _value(value)
{

}

template<class TValueType>
inline BindValue<TValueType>::operator TValueType()
{
    return _value;
}

/* --------------------------------------------------------------------------------------------- *
 * BindArray
 * --------------------------------------------------------------------------------------------- */

template <class TObjectType, class TDataType>
inline BindArray::BindArray(std::vector<TObjectType> & vector, BindValue<TDataType> datatype, unsigned int mode, unsigned int elemCount, unsigned int elemSize)
{
    _object = new BindArrayObject<TObjectType, TDataType>(vector, datatype, mode, elemCount, elemSize);
}

inline BindArray::~BindArray()
{
    delete _object;
}

template <class TObjectType, class TDataType>
inline std::vector<TObjectType> &  BindArray::GetVector (BindValue<TDataType> datatype)
{
    return (std::vector<TObjectType> &) (*(reinterpret_cast< BindArrayObject<TObjectType, TDataType> * > (_object)));
}

template <class TObjectType, class TDataType>
inline TDataType *  BindArray::GetData ()
{
    return (TDataType *) (*(reinterpret_cast< BindArrayObject<TObjectType, TDataType> * > (_object)));
}

inline void BindArray::SetInData()
{
    _object->SetInData();
}

inline void BindArray::SetOutData()
{
    _object->SetOutData();
}

template <class TObjectType, class TDataType>
inline BindArray::BindArrayObject<TObjectType, TDataType>::BindArrayObject(std::vector<TObjectType> &vector, BindValue<TDataType> datatype, unsigned int mode, unsigned int elemCount, unsigned int elemSize)
    : _vector(vector), _data(0), _mode(mode), _elemCount(elemCount), _elemSize(elemSize)
{
    AllocData();
}

template <class TObjectType, class TDataType>
inline BindArray::BindArrayObject<TObjectType, TDataType>::~BindArrayObject()
{
    FreeData();
}

template <class TObjectType, class TDataType>
inline void BindArray::BindArrayObject<TObjectType, TDataType>::AllocData()
{
    _data = new TDataType[_elemCount];
}

template<>
inline void BindArray::BindArrayObject<dstring, dtext>::AllocData()
{
    _data = (dtext *) new dtext[_elemSize * _elemCount];

    memset(_data, 0, _elemSize * _elemCount);
}

template <class TObjectType, class TDataType>
inline void BindArray::BindArrayObject<TObjectType, TDataType>::FreeData()
{
    delete [] _data ;
}

template <class TObjectType, class TDataType>
inline void BindArray::BindArrayObject<TObjectType, TDataType>::SetInData()
{
    if (_mode & OCI_BDM_IN)
    {
        typename std::vector<TObjectType>::iterator it, it_end;

        unsigned int index = 0;

        for (it = _vector.begin(), it_end = _vector.end(); it != it_end && index < _elemCount; it++, index++)
        {
            _data[index] = BindValue<TDataType>( *it);
        }
    }
}

template<>
inline void BindArray::BindArrayObject<dstring, dtext>::SetInData()
{
    if (_mode & OCI_BDM_IN)
    {
        std::vector<dstring>::iterator it, it_end;

        unsigned int index = 0;

        for (it = _vector.begin(), it_end = _vector.end(); it != it_end && index < _elemCount; it++, index++)
        {
            dstring & value = *it;

            memcpy((((dtext *) _data) + (_elemSize * index)), value.c_str(), value.size() * sizeof(dtext));
        }
    }
}

template <class TObjectType, class TDataType>
inline void BindArray::BindArrayObject<TObjectType, TDataType>::SetOutData()
{
    if (_mode & OCI_BDM_OUT)
    {
        typename std::vector<TObjectType>::iterator it, it_end;

        unsigned int index = 0;

        for (it = _vector.begin(), it_end = _vector.end(); it != it_end && index < _elemCount; it++, index++)
        {
            *it = (TObjectType) _data[index];
        }
    }
}

template<>
inline void BindArray::BindArrayObject<dstring, dtext>::SetOutData()
{
    if (_mode & OCI_BDM_OUT)
    {
        std::vector<dstring>::iterator it, it_end;

        unsigned int index = 0;

        for (it = _vector.begin(), it_end = _vector.end(); it != it_end && index < _elemCount; it++, index++)
        {
            *it = (dtext*) (((dtext *) _data) + (_elemSize * index));
        }
    }
}

template <class TObjectType, class TDataType>
inline BindArray::BindArrayObject<TObjectType, TDataType>:: operator std::vector<TObjectType> & ()
{
    return _vector;
}

template <class TObjectType, class TDataType>
inline BindArray::BindArrayObject<TObjectType, TDataType>:: operator TDataType * ()
{
    return _data;
}

/* --------------------------------------------------------------------------------------------- *
 * BindString
 * --------------------------------------------------------------------------------------------- */

inline void BindString::SetOutData()
{
    _string = _data;
}

inline BindString::BindString(dstring &string, unsigned int elemSize) : _string(string), _elemSize(elemSize)
{
    _data = new dtext[_elemSize];

     memset( _data, 0, _elemSize);
}

inline BindString::~BindString()
{
    delete [] _data;
}

inline BindString::operator dtext *()
{
    return _data;
}

/* --------------------------------------------------------------------------------------------- *
 * BindsHolder
 * --------------------------------------------------------------------------------------------- */

inline BindsHolder::BindsHolder()
{

}

inline BindsHolder::~BindsHolder()
{
    Clear();
}

inline void BindsHolder::Clear()
{
    std::vector<BindString *>::iterator its, its_end;

    for(its = _bindStrings.begin(), its_end = _bindStrings.end(); its < its_end; its++)
    {
        delete (*its);
    }

    _bindStrings.clear();

    std::vector<BindArray *>::iterator ita, ita_end;

    for(ita = _bindArrays.begin(), ita_end = _bindArrays.end(); ita < ita_end; ita++)
    {
        delete (*ita);
    }

    _bindArrays.clear();
}

inline void BindsHolder::AddBindArray(BindArray *bindArray)
{
    _bindArrays.push_back(bindArray);
}

inline void BindsHolder::AddBindString(BindString *bindString)
{
    _bindStrings.push_back(bindString);
}

inline void BindsHolder::SetOutData()
{
    std::vector<BindString *>::iterator its, its_end;

    for(its = _bindStrings.begin(), its_end = _bindStrings.end(); its < its_end; its++)
    {
        (*its)->SetOutData();
    }

    std::vector<BindArray *>::iterator ita, ita_end;

    for(ita = _bindArrays.begin(), ita_end = _bindArrays.end(); ita < ita_end; ita++)
    {
        (*ita)->SetOutData();
    }
}

inline void BindsHolder::SetInData()
{
    std::vector<BindArray *>::iterator ita, ita_end;

    for(ita = _bindArrays.begin(), ita_end = _bindArrays.end(); ita < ita_end; ita++)
    {
        (*ita)->SetInData();
    }
}

/* --------------------------------------------------------------------------------------------- *
 * Statement
 * --------------------------------------------------------------------------------------------- */

inline Statement::Statement(const Connection &connection)
{
    Acquire(API::Call(OCI_StatementCreate(connection)), (HandleFreeFunc) OCI_StatementFree, connection.GetHandle());
}

inline Statement::Statement(OCI_Statement *stmt)
{
     Acquire(stmt, (HandleFreeFunc) OCI_StatementFree, 0);
}

inline Statement::~Statement()
{
    if (_smartHandle->IsLastHolder(this))
    {
        BindsHolder *bindsHolder = GetBindsHolder(false);

        delete bindsHolder;
    }
}

inline Connection Statement::GetConnection()
{
    return Connection(API::Call(OCI_StatementGetConnection(*this)));
}

inline void Statement::Describe(mstring sql)
{
    ClearBinds();
    API::Call(OCI_Describe(*this, sql.c_str()));
}

inline void Statement::Parse(mstring sql)
{
    ClearBinds();
    API::Call(OCI_Parse(*this, sql.c_str()));
}

inline void Statement::Prepare(mstring sql)
{
    ClearBinds();
    API::Call(OCI_Prepare(*this, sql.c_str()));
}

inline void Statement::Execute()
{
    SetInData();
    API::Call(OCI_Execute(*this));
    SetOutData();
}

inline void Statement::Execute(mstring sql)
{
    API::Call(OCI_ExecuteStmt(*this, sql.c_str()));
}

inline unsigned int Statement::GetAffectedRows()
{
    return API::Call(OCI_GetAffectedRows(*this));
}

inline mstring Statement::GetSql()
{
    return API::MakeString(API::Call(OCI_GetSql(*this)));
}

inline Resultset Statement::GetResultset()
{
   return Resultset(API::Call(OCI_GetResultset(*this)));
}

inline Resultset Statement::GetNextResultset()
{
   return Resultset(API::Call(OCI_GetNextResultset(*this)));
}

inline void Statement::SetBindArraySize(unsigned int size)
{
    API::Call(OCI_BindArraySetSize(*this, size));
}

inline unsigned int Statement::GetBindArraySize()
{
    return API::Call(OCI_BindArrayGetSize(*this));
}

inline void Statement::AllowRebinding(bool value)
{
    API::Call(OCI_AllowRebinding(*this, value));
}

template <typename TBindMethod, class TDataType>
inline void Statement::Bind (TBindMethod &method, mstring name, TDataType& value, unsigned int mode)
{
    API::Call(method(*this, name.c_str(), &value));
    API::Call(OCI_BindSetDirection(API::Call(OCI_GetBind(*this, API::Call(OCI_GetBindCount(*this)))), mode));
}

template <typename TBindMethod, class TObjectType, class TDataType>
inline void Statement::Bind (TBindMethod &method, mstring name, TObjectType& value, BindValue<TDataType> datatype, unsigned int mode)
{
    API::Call(method(*this, name.c_str(), (TDataType) value));
    API::Call(OCI_BindSetDirection(API::Call(OCI_GetBind(*this, API::Call(OCI_GetBindCount(*this)))), mode));
}

template <typename TBindMethod, class TObjectType, class TDataType>
inline void Statement::Bind (TBindMethod &method, mstring name, std::vector<TObjectType> &values, BindValue<TDataType> datatype, unsigned int mode)
{
    BindArray * bnd = new BindArray(values, datatype, mode, OCI_BindArrayGetSize(*this), sizeof(TDataType));

    BindsHolder *bindsHolder = GetBindsHolder(true);
    bindsHolder->AddBindArray(bnd);

    API::Call(method(*this, name.c_str(), (TDataType *) bnd->GetData<TObjectType, TDataType>(), 0));
    API::Call(OCI_BindSetDirection(API::Call(OCI_GetBind(*this, API::Call(OCI_GetBindCount(*this)))), mode));
}

template <typename TBindMethod, class TObjectType, class TDataType, class TElemType>
inline void Statement::Bind (TBindMethod &method, mstring name, std::vector<TObjectType> &values, BindValue<TDataType> datatype, unsigned int mode, TElemType type)
{
    BindArray * bnd = new BindArray(values, datatype, mode, OCI_BindArrayGetSize(*this), sizeof(TDataType));

    BindsHolder *bindsHolder = GetBindsHolder(true);
    bindsHolder->AddBindArray(bnd);

    method(*this, name.c_str(), (TDataType *) bnd->GetData<TObjectType, TDataType>(), type, 0);
    API::Call(OCI_BindSetDirection(API::Call(OCI_GetBind(*this, API::Call(OCI_GetBindCount(*this)))), mode));
}

template <>
inline void Statement::Bind<short>(mstring name, short &value, unsigned int mode)
{
    Bind(OCI_BindShort, name, value, mode);
}

template <>
inline void Statement::Bind<unsigned short>(mstring name, unsigned short &value, unsigned int mode)
{
    Bind(OCI_BindUnsignedShort, name, value, mode);
}

template <>
inline void Statement::Bind<int>(mstring name, int &value, unsigned int mode)
{
    Bind(OCI_BindInt, name, value, mode);
}

template <>
inline void Statement::Bind<unsigned int>(mstring name, unsigned int &value, unsigned int mode)
{
    Bind(OCI_BindUnsignedInt, name, value, mode);
}

template <>
inline void Statement::Bind<big_int>(mstring name, big_int &value, unsigned int mode)
{
    Bind(OCI_BindBigInt, name, value, mode);
}

template <>
inline void Statement::Bind<big_uint>(mstring name, big_uint &value, unsigned int mode)
{
    Bind(OCI_BindUnsignedBigInt, name, value, mode);
}

template <>
inline void Statement::Bind<float>(mstring name, float &value, unsigned int mode)
{
    Bind(OCI_BindFloat, name, value, mode);
}

template <>
inline void Statement::Bind<double>(mstring name, double &value, unsigned int mode)
{
    Bind(OCI_BindDouble, name, value, mode);
}

template <>
inline void Statement::Bind<Date>(mstring name, Date &value, unsigned int mode)
{
    Bind(OCI_BindDate, name, value, BindValue<OCI_Date *>(), mode);
}

template <>
inline void Statement::Bind<Timestamp>(mstring name, Timestamp &value, unsigned int mode)
{
    Bind(OCI_BindTimestamp, name, value, BindValue<OCI_Timestamp *>(), mode);
}

template <>
inline void Statement::Bind<Interval>(mstring name, Interval &value, unsigned int mode)
{
    Bind(OCI_BindInterval, name, value, BindValue<OCI_Interval *>(), mode);
}

template <>
inline void Statement::Bind<Clob>(mstring name, Clob &value, unsigned int mode)
{
    Bind(OCI_BindLob, name, value, BindValue<OCI_Lob *>(), mode);
}

template <>
inline void Statement::Bind<Blob>(mstring name, Blob &value, unsigned int mode)
{
    Bind(OCI_BindLob, name, value, BindValue<OCI_Lob *>(), mode);
}

template <>
inline void Statement::Bind<File>(mstring name, File &value, unsigned int mode)
{
    Bind(OCI_BindFile, name, value, BindValue<OCI_File *>(), mode);
}

template <>
inline void Statement::Bind<Object>(mstring name, Object &value, unsigned int mode)
{
    Bind(OCI_BindObject, name, value, BindValue<OCI_Object *>(), mode);
}

template <>
inline void Statement::Bind<Reference>(mstring name, Reference &value, unsigned int mode)
{
    Bind(OCI_BindRef, name, value, BindValue<OCI_Ref *>(), mode);
}

template <>
inline void Statement::Bind<Collection>(mstring name, Collection &value, unsigned int mode)
{
    Bind(OCI_BindColl, name, value, BindValue<OCI_Coll *>(), mode);
}

template <>
inline void Statement::Bind<Statement>(mstring name, Statement &value, unsigned int mode)
{
    Bind(OCI_BindStatement, name, value, BindValue<OCI_Statement *>(), mode);
}

template <>
inline void Statement::Bind<CLong, unsigned int>(mstring name, CLong &value, unsigned int maxSize, unsigned int mode)
{
    API::Call(OCI_BindLong(*this, name.c_str(), value, maxSize));
    API::Call(OCI_BindSetDirection(API::Call(OCI_GetBind(*this, API::Call(OCI_GetBindCount(*this)))), mode));
}

template <>
inline void Statement::Bind<CLong, int>(mstring name, CLong &value, int maxSize, unsigned int mode)
{
    Bind<CLong, unsigned int>(name, value, (unsigned int) maxSize,  mode);
}


template <>
inline void Statement::Bind<BLong, unsigned int>(mstring name, BLong &value, unsigned int maxSize, unsigned int mode)
{
    API::Call(OCI_BindLong(*this, name.c_str(), value, maxSize));
    API::Call(OCI_BindSetDirection(API::Call(OCI_GetBind(*this, API::Call(OCI_GetBindCount(*this)))), mode));
}

template <>
inline void Statement::Bind<BLong, int>(mstring name, BLong &value, int maxSize, unsigned int mode)
{
    Bind<BLong, unsigned int>(name, value, (unsigned int) maxSize,  mode);
}

template <>
inline void Statement::Bind<dstring, unsigned int>(mstring name, dstring &value, unsigned int maxSize, unsigned int mode)
{
    if (maxSize == 0)
    {
        maxSize = value.size();
    }

    BindString * bnd = new BindString(value, maxSize+1);
    BindsHolder *bindsHolder = GetBindsHolder(true);
    bindsHolder->AddBindString(bnd);

    API::Call(OCI_BindString(*this, name.c_str(), (dtext*) (*bnd), maxSize));
    API::Call(OCI_BindSetDirection(API::Call(OCI_GetBind(*this, API::Call(OCI_GetBindCount(*this)))), mode));
}

template <>
inline void Statement::Bind<dstring, int>(mstring name, dstring &value, int maxSize, unsigned int mode)
{
    Bind<dstring, unsigned int>(name, value, (unsigned int) maxSize,  mode);
}

template <>
inline void Statement::Bind<BufferPointer, unsigned int>(mstring name, BufferPointer &value, unsigned int maxSize, unsigned int mode)
{
    API::Call(OCI_BindRaw(*this, name.c_str(), value, maxSize));
    API::Call(OCI_BindSetDirection(API::Call(OCI_GetBind(*this, API::Call(OCI_GetBindCount(*this)))), mode));
}

template <>
inline void Statement::Bind<BufferPointer,  int>(mstring name, BufferPointer &value,  int maxSize, unsigned int mode)
{
     Bind<void *, unsigned int>(name, value, (unsigned int) maxSize,  mode);
}

template <>
inline void Statement::Bind<short>(mstring name, std::vector<short> &values, unsigned int mode)
{
    Bind(OCI_BindArrayOfShorts, name, values, BindValue<short>(), mode);
}

template <>
inline void Statement::Bind<unsigned short>(mstring name, std::vector<unsigned short> &values, unsigned int mode)
{
    Bind(OCI_BindArrayOfUnsignedShorts, name, values, BindValue<unsigned short>(), mode);
}

template <>
inline void Statement::Bind<int>(mstring name, std::vector<int> &values, unsigned int mode)
{
    Bind(OCI_BindArrayOfInts, name, values, BindValue<int>(), mode);
}

template <>
inline void Statement::Bind<unsigned int>(mstring name, std::vector<unsigned int> &values, unsigned int mode)
{
    Bind(OCI_BindArrayOfUnsignedInts, name, values, BindValue<unsigned int>(), mode);
}

template <>
inline void Statement::Bind<big_int>(mstring name, std::vector<big_int> &values, unsigned int mode)
{
    Bind(OCI_BindArrayOfBigInts, name, values, BindValue<big_int>(), mode);
}

template <>
inline void Statement::Bind<big_uint>(mstring name, std::vector<big_uint> &values, unsigned int mode)
{
    Bind(OCI_BindArrayOfUnsignedBigInts, name, values, BindValue<big_uint>(), mode);
}

template <>
inline void Statement::Bind<float>(mstring name, std::vector<float> &values, unsigned int mode)
{
    Bind(OCI_BindArrayOfFloats, name, values, BindValue<float>(), mode);
}

template <>
inline void Statement::Bind<double>(mstring name, std::vector<double> &values, unsigned int mode)
{
    Bind(OCI_BindArrayOfDoubles, name, values, BindValue<double>(), mode);
}

template <>
inline void Statement::Bind<Date>(mstring name, std::vector<Date> &values, unsigned int mode)
{
    Bind(OCI_BindArrayOfDates, name, values, BindValue<OCI_Date *>(), mode);
}

template <>
inline void Statement::Bind<Timestamp, unsigned int>(mstring name, std::vector<Timestamp> &values, unsigned int type, unsigned int mode)
{
    Bind(OCI_BindArrayOfTimestamps, name, values, BindValue<OCI_Timestamp *>(), mode, type);
}

template <>
inline void Statement::Bind<Timestamp, int>(mstring name, std::vector<Timestamp> &values, int type, unsigned int mode)
{
    Bind<Timestamp, unsigned int>(name, values, type, mode);
}

template <>
inline void Statement::Bind<Interval, unsigned int>(mstring name, std::vector<Interval> &values, unsigned int type, unsigned int mode)
{
    Bind(OCI_BindArrayOfIntervals, name, values, BindValue<OCI_Interval *>(), mode, type);
}

template <>
inline void Statement::Bind<Interval, int>(mstring name, std::vector<Interval> &values, int type, unsigned int mode)
{
    Bind<Interval, unsigned int>(name, values, type, mode);
}

template <>
inline void Statement::Bind<Clob>(mstring name, std::vector<Clob> &values, unsigned int mode)
{
    Bind(OCI_BindArrayOfLobs, name, values, BindValue<OCI_Lob *>(), mode, OCI_CLOB);
}

template <>
inline void Statement::Bind<Blob>(mstring name, std::vector<Blob> &values, unsigned int mode)
{
    Bind(OCI_BindArrayOfLobs, name, values, BindValue<OCI_Lob *>(), mode, OCI_BLOB);
}

template <>
inline void Statement::Bind<File>(mstring name, std::vector<File> &values, unsigned int mode)
{
    Bind(OCI_BindArrayOfFiles, name, values, BindValue<OCI_File *>(), mode, OCI_BFILE);
}

template <>
inline void Statement::Bind<Object, TypeInfo>(mstring name, std::vector<Object> &values, TypeInfo &typeInfo, unsigned int mode)
{
   Bind(OCI_BindArrayOfObjects, name, values, BindValue<OCI_Object *>(), mode, (OCI_TypeInfo *) typeInfo);
}

template <>
inline void Statement::Bind<Reference, TypeInfo>(mstring name, std::vector<Reference> &values, TypeInfo &typeInfo, unsigned int mode)
{
   Bind(OCI_BindArrayOfRefs, name, values, BindValue<OCI_Ref *>(), mode, (OCI_TypeInfo *) typeInfo);
}

template <>
inline void Statement::Bind<Collection, TypeInfo>(mstring name, std::vector<Collection> &values, TypeInfo &typeInfo, unsigned int mode)
{
   Bind(OCI_BindArrayOfColls, name, values, BindValue<OCI_Coll *>(), mode, (OCI_TypeInfo *) typeInfo);
}

template <>
inline void Statement::Bind<dstring, unsigned int>(mstring name, std::vector<dstring> &values,  unsigned int maxSize, unsigned int mode)
{
    BindArray * bnd = new BindArray(values, BindValue<dtext>(), mode, OCI_BindArrayGetSize(*this), maxSize+1);

    BindsHolder *bindsHolder = GetBindsHolder(true);
    bindsHolder->AddBindArray(bnd);

    API::Call(OCI_BindArrayOfStrings(*this, name.c_str(), bnd->GetData<dstring, dtext>(), maxSize, 0));
    API::Call(OCI_BindSetDirection(API::Call(OCI_GetBind(*this, API::Call(OCI_GetBindCount(*this)))), mode));
}

template <>
inline void Statement::Bind<dstring, int>(mstring name, std::vector<dstring> &values, int maxSize, unsigned int mode)
{
    Bind<dstring, unsigned int>(name, values, ( unsigned int) maxSize, mode);
}

template <>
inline void Statement::Bind<BufferPointer, unsigned int>(mstring name, std::vector<BufferPointer> &values, unsigned int maxSize,  unsigned int mode)
{
    BindArray * bnd = new BindArray(values, BindValue<void *>(), mode, OCI_BindArrayGetSize(*this), maxSize+1);

    BindsHolder *bindsHolder = GetBindsHolder(true);
    bindsHolder->AddBindArray(bnd);

    API::Call(OCI_BindArrayOfRaws(*this, name.c_str(), bnd->GetData<void *, void *>(), maxSize, 0));
    API::Call(OCI_BindSetDirection(API::Call(OCI_GetBind(*this, API::Call(OCI_GetBindCount(*this)))), mode));
}

template <>
inline void Statement::Bind<BufferPointer, int>(mstring name, std::vector<BufferPointer> &values, int maxSize, unsigned int mode)
{
    Bind<void *, int>(name, values, ( unsigned int) maxSize, mode);
}

template <>
inline void Statement::Register<unsigned short>(mstring name)
{
    API::Call(OCI_RegisterUnsignedShort(*this, name.c_str()));
}

template <>
inline void Statement::Register<short>(mstring name)
{
    API::Call(OCI_RegisterShort(*this, name.c_str()));
}

template <>
inline void Statement::Register<unsigned int>(mstring name)
{
    API::Call(OCI_RegisterUnsignedInt(*this, name.c_str()));
}

template <>
inline void Statement::Register<int>(mstring name)
{
    API::Call(OCI_RegisterInt(*this, name.c_str()));
}

template <>
inline void Statement::Register<big_uint>(mstring name)
{
    API::Call(OCI_RegisterUnsignedBigInt(*this, name.c_str()));
}

template <>
inline void Statement::Register<big_int>(mstring name)
{
    API::Call(OCI_RegisterBigInt(*this, name.c_str()));
}

template <>
inline void Statement::Register<float>(mstring name)
{
    API::Call(OCI_RegisterFloat(*this, name.c_str()));
}

template <>
inline void Statement::Register<double>(mstring name)
{
    API::Call(OCI_RegisterDouble(*this, name.c_str()));
}

template <>
inline void Statement::Register<Date>(mstring name)
{
    API::Call(OCI_RegisterDate(*this, name.c_str()));
}

template <>
inline void Statement::Register<Timestamp, unsigned int>(mstring name, unsigned int elemType)
{
    API::Call(OCI_RegisterTimestamp(*this, name.c_str(), elemType));
}

template <>
inline void Statement::Register<Interval, unsigned int>(mstring name, unsigned int elemType)
{
    API::Call(OCI_RegisterInterval(*this, name.c_str(), elemType));
}

template <>
inline void Statement::Register<Clob>(mstring name)
{
    API::Call(OCI_RegisterLob(*this, name.c_str(), OCI_CLOB));
}

template <>
inline void Statement::Register<Blob>(mstring name)
{
    API::Call(OCI_RegisterLob(*this, name.c_str(), OCI_BLOB));
}

template <>
inline void Statement::Register<File>(mstring name)
{
    API::Call(OCI_RegisterFile(*this, name.c_str(), OCI_BFILE));
}

template <>
inline void Statement::Register<Object, TypeInfo>(mstring name, TypeInfo& typeInfo)
{
    API::Call(OCI_RegisterObject(*this, name.c_str(), typeInfo));
}

template <>
inline void Statement::Register<Reference, TypeInfo>(mstring name, TypeInfo& typeInfo)
{
    API::Call(OCI_RegisterRef(*this, name.c_str(), typeInfo));
}

template <>
inline void Statement::Register<dstring, unsigned int>(mstring name, unsigned int len)
{
    API::Call(OCI_RegisterString(*this, name.c_str(), len));
}

template <>
inline void Statement::Register<dstring, int>(mstring name, int len)
{
    Register<dstring, unsigned int>(name, len);
}

template <>
inline void Statement::Register<BufferPointer, unsigned int>(mstring name, unsigned int len)
{
    API::Call(OCI_RegisterRaw(*this, name.c_str(), len));
}

template <>
inline void Statement::Register<BufferPointer, int>(mstring name, int len)
{
  Register<dstring, unsigned int>(name,(unsigned int) len);
}


inline unsigned int Statement::GetStatementType()
{
    return API::Call(OCI_GetStatementType(*this));
}

inline unsigned int Statement::GetSqlErrorPos()
{
    return API::Call(OCI_GetSqlErrorPos(*this));
}

inline unsigned int Statement::GetBindCount()
{
    return API::Call(OCI_GetBindCount(*this));
}

inline void Statement::SetFetchMode(unsigned int value)
{
    API::Call(OCI_SetFetchMode(*this, value));
}

inline unsigned int Statement::GetFetchMode()
{
    return API::Call(OCI_GetFetchMode(*this));
}

inline void Statement::SetBindMode(unsigned int value)
{
    API::Call(OCI_SetBindMode(*this, value));
}

inline unsigned int Statement::GetBindMode()
{
    return API::Call(OCI_GetBindMode(*this));
}

inline void Statement::SetFetchSize(unsigned int value)
{
    API::Call(OCI_SetFetchSize(*this, value));
}

inline unsigned int Statement::GetFetchSize()
{
   return API::Call(OCI_GetFetchSize(*this));
}

inline void Statement::SetPrefetchSize(unsigned int value)
{
    API::Call(OCI_SetPrefetchSize(*this, value));
}

inline unsigned int Statement::GetPrefetchSize()
{
    return API::Call(OCI_GetPrefetchSize(*this));
}

inline void Statement::SetPrefetchMemory(unsigned int value)
{
    API::Call(OCI_SetPrefetchMemory(*this, value));
}

inline unsigned int Statement::GetPrefetchMemory()
{
    return API::Call(OCI_GetPrefetchMemory(*this));
}

inline void Statement::SetLongMaxSize(unsigned int value)
{
    API::Call(OCI_SetLongMaxSize(*this, value));
}

inline unsigned int Statement::GetLongMaxSize()
{
    return API::Call(OCI_GetLongMaxSize(*this));
}

inline void Statement::SetLongMode(unsigned int value)
{
    API::Call(OCI_SetLongMode(*this, value));
}

inline unsigned int Statement::GetLongMode()
{
    return API::Call(OCI_GetLongMode(*this));
}

inline unsigned int Statement::GetSQLCommand()
{
    return API::Call(OCI_GetSQLCommand(*this));
}

inline mstring Statement::GetSQLVerb()
{
    return API::MakeString(API::Call(OCI_GetSQLVerb(*this)));
}

inline void Statement::GetBatchErrors(std::vector<Exception> &exceptions)
{
    OCI_Error *err =  API::Call(OCI_GetBatchError(*this));

    while (err)
    {
        exceptions.push_back(Exception(err));

        err =  API::Call(OCI_GetBatchError(*this));
    }
}

inline void Statement::ClearBinds()
{
    BindsHolder *bindsHolder = GetBindsHolder(false);

    if (bindsHolder)
    {
         bindsHolder->Clear();
    }
}

inline void Statement::SetOutData()
{
    BindsHolder *bindsHolder = GetBindsHolder(false);

    if (bindsHolder)
    {
        bindsHolder->SetOutData();
    }
}

inline void Statement::SetInData()
{
    BindsHolder *bindsHolder = GetBindsHolder(false);

    if (bindsHolder)
    {
        bindsHolder->SetInData();
    }
}

inline BindsHolder * Statement::GetBindsHolder(bool create)
{
    BindsHolder * bindsHolder = (BindsHolder *) _smartHandle->GetExtraInfos();

    if (bindsHolder == 0 && create)
    {
        bindsHolder = new BindsHolder();
        _smartHandle->SetExtraInfos(bindsHolder);
    }

    return bindsHolder;
}

/* --------------------------------------------------------------------------------------------- *
 * Resultset
 * --------------------------------------------------------------------------------------------- */

inline Resultset::Resultset(OCI_Resultset *resultset)
{
    Acquire(resultset);
}

inline bool Resultset::Next()
{
    return (API::Call(OCI_FetchNext(*this)) == TRUE);
}

inline bool Resultset::Prev()
{
    return (API::Call(OCI_FetchPrev(*this)) == TRUE);
}

inline bool Resultset::First()
{
    return (API::Call(OCI_FetchFirst(*this)) == TRUE);
}

inline bool Resultset::Last()
{
    return (API::Call(OCI_FetchLast(*this)) == TRUE);
}

inline bool Resultset::Seek(unsigned int mode, int offset)
{
    return (API::Call(OCI_FetchSeek(*this, mode, offset)) == TRUE);
}

inline unsigned int Resultset::GetCount()
{
    return API::Call(OCI_GetRowCount(*this));
}

inline unsigned int Resultset::GetCurrentRow()
{
    return API::Call(OCI_GetCurrentRow(*this));
}

inline unsigned int Resultset::GetColumnCount()
{
    return API::Call(OCI_GetColumnCount(*this));
}

inline Column Resultset::GetColumn(unsigned int index)
{
    return Column(API::Call(OCI_GetColumn(*this, index)));
}

inline Column Resultset::GetColumn(mstring name)
{
    return Column(API::Call(OCI_GetColumn2(*this, name.c_str())));
}

inline bool Resultset::IsColumnNull(unsigned int index)
{
    return (API::Call(OCI_IsNull(*this, index)) == TRUE);
}

inline bool Resultset::IsColumnNull(mstring name)
{
    return (API::Call(OCI_IsNull2(*this, name.c_str())) == TRUE);
}

inline Statement Resultset::GetStatement()
{
    return Statement( API::Call(OCI_ResultsetGetStatement(*this)));
}

template<>
inline short Resultset::Get<short>(int index)
{
    return API::Call(OCI_GetShort(*this, index));
}

template<>
inline short Resultset::Get<short>(mstring name)
{
    return API::Call(OCI_GetShort2(*this, name.c_str()));
}

template<>
inline unsigned short Resultset::Get<unsigned short>(int index)
{
    return API::Call(OCI_GetUnsignedShort(*this, index));
}

template<>
inline unsigned short Resultset::Get<unsigned short>(mstring name)
{
    return API::Call(OCI_GetUnsignedShort2(*this, name.c_str()));
}

template<>
inline int Resultset::Get<int>(int index)
{
    return API::Call(OCI_GetInt(*this, index));
}

template<>
inline int Resultset::Get<int>(mstring name)
{
    return API::Call(OCI_GetInt2(*this, name.c_str()));
}

template<>
inline unsigned int Resultset::Get<unsigned int>(int index)
{
    return API::Call(OCI_GetUnsignedInt(*this, index));
}

template<>
inline unsigned int Resultset::Get<unsigned int>(mstring name)
{
    return API::Call(OCI_GetUnsignedInt2(*this, name.c_str()));
}

template<>
inline big_int Resultset::Get<big_int>(int index)
{
    return API::Call(OCI_GetBigInt(*this, index));
}

template<>
inline big_int Resultset::Get<big_int>(mstring name)
{
    return API::Call(OCI_GetBigInt2(*this, name.c_str()));
}

template<>
inline big_uint Resultset::Get<big_uint>(int index)
{
    return API::Call(OCI_GetUnsignedBigInt(*this, index));
}

template<>
inline big_uint Resultset::Get<big_uint>(mstring name)
{
    return API::Call(OCI_GetUnsignedBigInt2(*this, name.c_str()));
}

template<>
inline float Resultset::Get<float>(int index)
{
    return API::Call(OCI_GetFloat(*this, index));
}

template<>
inline float Resultset::Get<float>(mstring name)
{
    return API::Call(OCI_GetFloat2(*this, name.c_str()));
}

template<>
inline double Resultset::Get<double>(int index)
{
    return API::Call(OCI_GetDouble(*this, index));
}

template<>
inline double Resultset::Get<double>(mstring name)
{
    return API::Call(OCI_GetDouble2(*this, name.c_str()));
}

template<>
inline dstring Resultset::Get<dstring>(int index)
{
    return API::MakeString(API::Call(OCI_GetString(*this, index)));
}

template<>
inline dstring Resultset::Get<dstring>(mstring name)
{
    return API::MakeString(API::Call(OCI_GetString2(*this,name.c_str())));
}

template<>
inline Date Resultset::Get<Date>(int index)
{
    return API::Call(OCI_GetDate(*this, index));
}

template<>
inline Date Resultset::Get<Date>(mstring name)
{
    return API::Call(OCI_GetDate2(*this,name.c_str()));
}

template<>
inline Timestamp Resultset::Get<Timestamp>(int index)
{
    return API::Call(OCI_GetTimestamp(*this, index));
}

template<>
inline Timestamp Resultset::Get<Timestamp>(mstring name)
{
    return API::Call(OCI_GetTimestamp2(*this,name.c_str()));
}

template<>
inline Interval Resultset::Get<Interval>(int index)
{
    return API::Call(OCI_GetInterval(*this, index));
}

template<>
inline Interval Resultset::Get<Interval>(mstring name)
{
    return API::Call(OCI_GetInterval2(*this,name.c_str()));
}

template<>
inline Object Resultset::Get<Object>(int index)
{
    return API::Call(OCI_GetObject(*this, index));
}

template<>
inline Object Resultset::Get<Object>(mstring name)
{
    return API::Call(OCI_GetObject2(*this,name.c_str()));
}

template<>
inline Collection Resultset::Get<Collection>(int index)
{
    return API::Call(OCI_GetColl(*this, index));
}

template<>
inline Collection Resultset::Get<Collection>(mstring name)
{
    return API::Call(OCI_GetColl2(*this,name.c_str()));
}

template<>
inline Reference Resultset::Get<Reference>(int index)
{
    return API::Call(OCI_GetRef(*this, index));
}

template<>
inline Reference Resultset::Get<Reference>(mstring name)
{
    return API::Call(OCI_GetRef2(*this,name.c_str()));
}

template<>
inline Statement Resultset::Get<Statement>(int index)
{
    return API::Call(OCI_GetStatement(*this, index));
}

template<>
inline Statement Resultset::Get<Statement>(mstring name)
{
    return API::Call(OCI_GetStatement2(*this,name.c_str()));
}

template<>
inline Clob Resultset::Get<Clob>(int index)
{
    return API::Call(OCI_GetLob(*this, index));
}

template<>
inline Clob Resultset::Get<Clob>(mstring name)
{
    return API::Call(OCI_GetLob2(*this,name.c_str()));
}

template<>
inline Blob Resultset::Get<Blob>(int index)
{
    return API::Call(OCI_GetLob(*this, index));
}

template<>
inline Blob Resultset::Get<Blob>(mstring name)
{
    return API::Call(OCI_GetLob2(*this,name.c_str()));
}

template<>
inline File Resultset::Get<File>(int index)
{
    return API::Call(OCI_GetFile(*this, index));
}

template<>
inline File Resultset::Get<File>(mstring name)
{
    return API::Call(OCI_GetFile2(*this,name.c_str()));
}

template<>
inline CLong Resultset::Get<CLong>(int index)
{
    return API::Call(OCI_GetLong(*this, index));
}

template<>
inline CLong Resultset::Get<CLong>(mstring name)
{
    return API::Call(OCI_GetLong2(*this,name.c_str()));
}


template<>
inline BLong Resultset::Get<BLong>(int index)
{
    return API::Call(OCI_GetLong(*this, index));
}

template<>
inline BLong Resultset::Get<BLong>(mstring name)
{
    return API::Call(OCI_GetLong2(*this,name.c_str()));
}

/* --------------------------------------------------------------------------------------------- *
 * Column
 * --------------------------------------------------------------------------------------------- */


inline Column::Column(OCI_Column *column)
{
    Acquire(column);
}

inline mstring Column::GetName()
{
    return API::MakeString(API::Call(OCI_ColumnGetName(*this)));
}

inline mstring Column::GetSQLType()
{
    return API::MakeString(API::Call(OCI_ColumnGetSQLType(*this)));
}

inline mstring Column::GetFullSQLType()
{
    unsigned int size = OCI_SIZE_BUFFER;

    ManagedBuffer<mtext> buffer = new mtext[size+1];

    API::Call(OCI_ColumnGetFullSQLType(*this, buffer, size));

    return API::MakeString((mtext *) buffer);
}

inline unsigned int Column::GetType()
{
    return API::Call(OCI_ColumnGetType(*this));
}

inline unsigned int Column::GetSubType()
{
    return API::Call(OCI_ColumnGetSubType(*this));
}

inline unsigned int Column::GetCharsetForm()
{
    return API::Call(OCI_ColumnGetCharsetForm(*this));
}

inline unsigned int Column::GetSize()
{
    return API::Call(OCI_ColumnGetSize(*this));
}

inline int Column::GetScale()
{
    return API::Call(OCI_ColumnGetScale(*this));
}

inline int Column::GetPrecision()
{
    return API::Call(OCI_ColumnGetPrecision(*this));
}

inline int Column::GetFractionalPrecision()
{
    return API::Call(OCI_ColumnGetFractionalPrecision(*this));
}

inline int Column::GetLeadingPrecision()
{
    return API::Call(OCI_ColumnGetLeadingPrecision(*this));
}

inline bool Column::GetNullable()
{
    return (API::Call(OCI_ColumnGetNullable(*this)) == TRUE);
}

inline bool Column::GetCharUsed()
{
    return (API::Call(OCI_ColumnGetCharUsed(*this)) == TRUE);
}

inline TypeInfo Column::GetTypeInfo()
{
    return TypeInfo(API::Call(OCI_ColumnGetTypeInfo(*this)));
}



}

#endif
