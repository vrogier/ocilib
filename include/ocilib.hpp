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


#define ARG_NOT_USED(a) (a) = (a)

/**
 * @namespace ocilib
 * @brief
 */
namespace ocilib
{

/**
 * @defgroup OcilibCppApi C++ API
 * @{
 */

/**
 * @defgroup OcilibCppApiDesign Design
 * @{
 *
 * @}
 */

/**
 * @defgroup OcilibCppApiTypes Types
 * @{
 */
    
/**
 * @typedef mstring
 * @brief    
 * string class wrapping the OCILIB mtext * type and MT() macros (see @ref OcilibCApiSupportedCharsets)
 */

typedef std::basic_string<mtext, std::char_traits<mtext>, std::allocator<mtext> > mstring;
    
/**
 * @typedef dstring
 * @brief 
 * string class wrapping the OCILIB dtext * type and DT() macros (see @ref OcilibCApiSupportedCharsets)
 */

typedef std::basic_string<dtext, std::char_traits<dtext>, std::allocator<dtext> > dstring;

/**
 * @typedef BufferPointer
 * @brief 
 * Alias for the generic void pointer
 */

typedef void *              BufferPointer;

/**
 * @typedef UnknownHandle
 * @brief 
 * Alias used for manipulating unknown handle types
 */

typedef void *              UnknownHandle;

/**
 * @typedef CallbackPointer
 * @brief 
 * Alias used for storing user callback method pointers
 */

typedef void *              CallbackPointer;

/**
 * @typedef ThreadHandle
 * @brief 
 * Alias for an OCI_Thread pointer
 */

typedef OCI_Thread *        ThreadHandle;

/**
 * @typedef MutexHandle
 * @brief 
 * Alias for an OCI_Mutex pointer
 */

typedef OCI_Mutex *         MutexHandle ;

/**
 * @}
 */

/* ********************************************************************************************* *
 *                                 STATIC INTERNAL METHODS
 * ********************************************************************************************* */

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

/* ********************************************************************************************* *
 *                                INTERNAL CLASS DECLARATIONS
 * ********************************************************************************************* */
/**
 * @class HandleHolder
 * @brief 
 * Template class providing OCILIB handles auto memory, life cycle and scope management
 */
template <class THandleType>
class HandleHolder;

/* ********************************************************************************************* *
 *                                PUBLIC CLASS DECLARATIONS
 * ********************************************************************************************* */

/**
 * @defgroup OcilibCppApiClasses Classes
 * @{
 *
 */

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
 *
 * @}
 */

/**
 * @defgroup OcilibCppApiCallbacks Callbacks
 * @{

 *
 */

/**
 * @typedef HAHandlerProc
 *
 * @brief
 *
 */
typedef POCI_THREAD         ThreadProc;

/**
 * @typedef HAHandlerProc
 *
 * @brief
 *
 */
typedef POCI_THREADKEYDEST  ThreadKeyFreeProc;

/**
 * @typedef HAHandlerProc
 *
 * @brief
 *
 */
typedef void (*HAHandlerProc) (Connection &con, unsigned int source, unsigned int event, Timestamp  &time);

/**
 * @typedef TAFHandlerProc
 *
 * @brief
 *
 */
typedef void (*TAFHandlerProc) (Connection &con, unsigned int type, unsigned int event);

/**
 * @typedef NotifyHandlerProc
 *
 * @brief
 *
 */
typedef void (*NotifyHandlerProc) (Event &evt);

/**
 * @typedef NotifyHandlerProc
 *
 * @brief
 *
 */
typedef void (*NotifyAQHandlerProc) (Dequeue &dequeue);

/**
 *
 * @}
 */

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

    operator TBufferType* ();
    operator const TBufferType* ();

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
    TValue Get(TKey key);
    void Set(TKey key, TValue value);

private:

    typedef std::map< TKey, TValue > ConcurrentPoolMap;

    ConcurrentPoolMap _map;
    MutexHandle _mutex;

    void Lock();
    void Unlock();
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

    virtual std::list<Handle *> & GetChildren() = 0;

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

    bool IsNull();

    operator bool();
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

        TSmartHandleType GetHandle();

        Handle *GetParent()
        {
            return _parent;
        }

        void DetachFromHolders();

        BufferPointer GetExtraInfos();
        void   SetExtraInfos(BufferPointer extraInfo);

        bool IsLastHolder(HandleHolder<TSmartHandleType> *holder);

        std::list<Handle *> & GetChildren();

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

    operator TValueType();

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

    template <class TObjectType, class TDataType>
    BindArray(std::vector<TObjectType> & vector, BindValue<TDataType> datatype, unsigned int mode, unsigned int elemCount, unsigned int elemSize);
    virtual ~BindArray();

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
        virtual ~BindArrayObject();
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
 */
class BindString : public BindObject
{
    friend class Statement;

public:

    operator dtext *();

    void SetInData();
    void SetOutData();

    BindString(dstring &source, unsigned int elemSize);
    virtual ~BindString();

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

 /**
 *
 * @}
 */

/**
 * @defgroup OcilibCppApiClasses Classes
 * @{
 *
 */

/**
 * @class Exception
 *
 * @brief
 *
 */
class Exception : public HandleHolder<OCI_Error *>
{
    friend void ocilib::Check();
    friend class Statement;

public:

    mstring         GetMessage();
    unsigned int    GetType();
    int             GetOracleErrorCode();
    int             GetInternalErrorCode();
    Statement       GetStatement();
    Connection      GetConnection();
    unsigned int    GetRow();

private:

    Exception();
    Exception(OCI_Error *err);
};

/**
 * @class Environment
 *
 * @brief
 *
 */
class Environment
{
    friend class Connection;
    friend class Subscription;
    friend class Dequeue;
    template<class THandleType>
    friend class HandleHolder;

public:

    static void Initialize(unsigned int mode = OCI_ENV_DEFAULT, mstring libpath = MT(""));
    static void Cleanup();

    static unsigned int GetMode();

    static unsigned int GetImportMode();

    static unsigned int GetCharsetMetaData();
    static unsigned int GetCharsetUserData();

    static unsigned int GetCompileVersion();
    static unsigned int GetRuntimeVersion();

    static void EnableWarnings(bool value);

    static void StartDatabase(mstring db, mstring user, mstring pwd, unsigned int start_flag, unsigned int startMode,
                              unsigned int sessMode = OCI_SESSION_SYSDBA, mstring spfile = MT(""));

    static void ShutdownDatabase(mstring db, mstring user, mstring pwd, unsigned int shut_flag, unsigned int shutMode,
                              unsigned int sessMode = OCI_SESSION_SYSDBA);

    static void ChangeUserPassword(mstring db, mstring user, mstring pwd, mstring newPassword);

    static void SetHAHandler(HAHandlerProc handler);

private:

    static void HAHandler(OCI_Connection *pConnection, unsigned int source, unsigned int event, OCI_Timestamp  *pTimestamp);
    static void TAFHandler(OCI_Connection *pConnection, unsigned int type, unsigned int event);
    static void NotifyHandler(OCI_Event *pEvent);
    static void NotifyHandlerAQ(OCI_Dequeue *pDequeue);

    typedef ConcurrentPool<UnknownHandle, Handle *> HandlePool;
    typedef ConcurrentPool<UnknownHandle, CallbackPointer> CallbackPool;


    class EnvironmentHandle : public HandleHolder<UnknownHandle>
    {
        friend class Connection;

    public:

        HandlePool   Handles;
        CallbackPool Callbacks;
        unsigned int Mode;

        void Initialize(UnknownHandle pEnv, unsigned int envMode);
        void Finalize();
    };

    static EnvironmentHandle& GetEnvironmentHandle();
};


/**
 * @class Thread
 *
 * @brief
 *
 */
class Thread
{
public:
    static ThreadHandle Create();
    static void Destroy(ThreadHandle handle);
    static void Run(ThreadHandle handle, ThreadProc func, void *args);
    static void Join(ThreadHandle thrhandleead);
};

/**
 * @class ThreadKey
 *
 * @brief
 *
 */
class Mutex
{
public:
    static MutexHandle Create();
    static void Destroy(MutexHandle handle);
    static void Acquire(MutexHandle handle);
    static void Release(MutexHandle handle);
};

/**
 * @class ThreadKey
 *
 * @brief
 *
 */
class ThreadKey
{
public:
    static void Create(mstring name, ThreadKeyFreeProc freeProc = 0);
    static void SetValue(mstring name, void *value);
    static void * GetValue(mstring name);
};

/**
 * @class Pool
 *
 * @brief
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
    friend class Subscription;

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

    Connection(OCI_Connection *con, Handle *parent);

};

/**
 * @class Transaction
 *
 * @brief
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
 */
class Date : public HandleHolder<OCI_Date *>
{
    friend class Statement;
    friend class Resultset;
    friend class BindArray;
    friend class Object;
    friend class Collection;
    friend class Message;

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

    Date(OCI_Date *pDate, Handle *parent = 0);
};

/**
 * @class Interval
 *
 * @brief
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

    Interval(OCI_Interval *pInterval, Handle *parent = 0);
};

/**
 * @class Timestamp
 *
 * @brief
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

    Timestamp(OCI_Timestamp *pTimestamp, Handle *parent = 0);
};

/**
 * @class Clob
 *
 * @brief
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
    bool Seek(unsigned int seekMode, big_uint offset);

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

    Clob(OCI_Lob *pLob, Handle *parent = 0);
};

/**
 * @class Blob
 *
 * @brief
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
    bool Seek(unsigned int seekMode, big_uint offset);

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

    Blob(OCI_Lob *pLob, Handle *parent = 0);
};

/**
 * @class File
 *
 * @brief
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
    bool Seek(unsigned int seekMode, big_uint offset);

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

    File(OCI_File *pFile, Handle *parent = 0);
};

/**
 * @class TypeInfo
 *
 * @brief
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
 */
class Object : public HandleHolder<OCI_Object *>
{
    friend class Statement;
    friend class Resultset;
    friend class BindArray;
    friend class Reference;
    friend class Collection;
    friend class Message;

public:

    Object(const TypeInfo &typeInfo);

    void Assign(const Object& other);

    bool IsAttributeNull(mstring name);
    void SetAttributeNull(mstring name);

    TypeInfo GetTypeInfo();
    Reference GetReference();

    template<class TDataType>
    TDataType Get(mstring name);

    template<class TDataType>
    void Set(mstring name, const TDataType &value);

    template<class TDataType>
    void Get(mstring name, TDataType value, unsigned int &size);

    template<class TDataType>
    void Set(mstring name, const TDataType &value, unsigned int size);

private:

    Object(OCI_Object *pObject, Handle *parent = 0);
};

/**
 * @class Reference
 *
 * @brief
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

    Reference(const TypeInfo &typeInfo);

    TypeInfo GetTypeInfo();
    Object GetObject();

    void Assign(const Reference& other);

    bool IsReferenceNull();
    void SetReferenceNull();

    mstring ToString();

private:

    Reference(OCI_Ref *pRef, Handle *parent = 0);
};

/**
 * @class Collection
 *
 * @brief
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

    bool IsElementNull(unsigned int index);
    void SetElementNull(unsigned int index);

    template <class TDataType>
    TDataType Get(unsigned int index);

    template <class TDataType>
    void Set(unsigned int index, const TDataType &data);

    template<class TDataType>
    void Get(unsigned int index, TDataType value, unsigned int &size);

    template<class TDataType>
    void Set(unsigned int index, const TDataType value, unsigned int size);

    template <class TDataType>
    void Append(const TDataType &data);

    template <class TDataType>
    void Append(const TDataType &data, unsigned int size);

    TypeInfo GetTypeInfo();

private:

    template <class TDataType>
    static TDataType GetElem(OCI_Elem *elem, Handle *parent);

    template <class TDataType>
    static void SetElem(OCI_Elem *elem, const TDataType &value);

    template <class TDataType>
    static void GetElem(OCI_Elem *elem,  TDataType value, unsigned int &size);

    template <class TDataType>
    static void SetElem(OCI_Elem *elem, const TDataType value,  unsigned int size);

    Collection(OCI_Coll *pColl, Handle *parent = 0);
};


/**
 * @class CollectionIterator
 *
 * @brief
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

    bool IsElementNull();
    void SetElementNull();
};

/**
 * @class CLong
 *
 * @brief
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

    CLong(OCI_Long *pLong, Handle *parent = 0);
};

/**
 * @class CLong
 *
 * @brief
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

    BLong(OCI_Long *pLong, Handle *parent = 0);
};

/**
 * @class BindInfo
 *
 * @brief
 *
 */
class BindInfo : public HandleHolder<OCI_Bind *>
{
    friend class Statement;

public:

    mstring GetName();
    unsigned int GetType();
    unsigned int GetSubType();
    unsigned int GetElemcount();

    Statement GetStatement();

    void SetNull();
    void SetNull(unsigned int pos);

    bool IsNull();
    bool IsNull(unsigned int pos);

    void SetCharsetForm(unsigned int value);

    void SetDirection(unsigned int value);
    unsigned int GetDirection();

private:

    BindInfo(OCI_Bind *pBind, Handle *parent);
};

/**
 * @class Statement
 *
 * @brief
 *
 */
class Statement : public HandleHolder<OCI_Statement *>
{
    friend class Exception;
    friend class Resultset;
    friend class CLong;
    friend class BLong;
    friend class BindInfo;

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

    unsigned int GetBindCount();

    BindInfo GetBind(unsigned int index);
    BindInfo GetBind(mstring name);

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

    Statement(OCI_Statement *stmt, Handle *parent = 0);

    BindsHolder *GetBindsHolder(bool allocate);

    void ReleaseResultsets();

    void SetLastBindMode(unsigned int mode);

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
 */
class Resultset : public HandleHolder<OCI_Resultset *>
{
    friend class Statement;
public:

    template<class TDataType>
    TDataType Get(unsigned int index);

    template<class TDataType>
    TDataType Get(mstring name);

    template<class TDataType>
    void Get(unsigned int index, TDataType value, unsigned int &size);

    template<class TDataType>
    void Get(mstring name, TDataType value, unsigned int &size);

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

   Resultset(OCI_Resultset *resultset, Handle *parent);
};

/**
 * @class Column
 *
 * @brief
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

    Column(OCI_Column *pColumn, Handle *parent);
};

/**
 * @class Subscription
 *
 * @brief
 *
 */
class Subscription : public HandleHolder<OCI_Subscription *>
{
    friend class Event;

public:

    Subscription();

    void Register(const Connection &con, mstring name, unsigned int type, NotifyHandlerProc handler, unsigned int port = 0, unsigned int timeout = 0);
    void Unregister();

    void Watch(mstring sql);

    mstring GetName();
    unsigned int GetTimeout();
    unsigned int GetPort();

    Connection GetConnection();

private:

    Subscription(OCI_Subscription *pSubcription);
};

/**
 * @class Event
 *
 * @brief
 *
 */
class Event : public HandleHolder<OCI_Event *>
{
    friend class Subscription;
    friend class Environment;

public:

    unsigned int GetType();
    unsigned int GetOperation();
    mstring GetDatabaseName();
    mstring GetObjectName();
    mstring GetRowID();
    Subscription GetSubscription();

private:

    Event(OCI_Event *pEvent);
};


/**
 * @class Agent
 *
 * @brief
 *
 */
class Agent : public HandleHolder<OCI_Agent *>
{
    friend class Message;
    friend class Dequeue;

public:

    Agent(const Connection &connection, mstring name = MT(""), mstring address = MT(""));

    mstring GetName();
    void SetName(mstring value);

    mstring GetAddress();
    void SetAddress(mstring value);

private:

    Agent(OCI_Agent *pAgent, Handle *parent);
};

/**
 * @class Message
 *
 * @brief
 *
 */
class Message : public HandleHolder<OCI_Msg *>
{
    friend class Dequeue;

public:

    Message(const TypeInfo &typeInfo);

    void Reset();

    Object Get();
    void Set(const Object &value);

    void Get(BufferPointer value, unsigned int &size);
    void Set(const BufferPointer &value, unsigned int size);

    Date GetEnqueueTime();
    int GetAttemptCount();
    unsigned int GetState();
    void GetID(BufferPointer value, unsigned int &size);

    int GetExpiration();
    void SetExpiration(int value);

    int GetEnqueueDelay();
    void SetEnqueueDelay(int value);

    int GetPriority();
    void SetPriority(int value);

    void GetOriginalID(BufferPointer value, unsigned int &size);
    void SetOriginalID(const BufferPointer &value, unsigned int size);

    mstring GetCorrelation();
    void SetCorrelation(mstring value);

    mstring GetExceptionQueue();
    void SetExceptionQueue(mstring value);

    Agent GetSender();
    void SetSender(const Agent &agent);

    void SetConsumers(std::vector<Agent> &agents);

private:

    Message(OCI_Msg *pMessage, Handle *parent);
};

/**
 * @class Enqueue
 *
 * @brief
 *
 */
class Enqueue : public HandleHolder<OCI_Enqueue *>
{
public:

    Enqueue(const TypeInfo &typeInfo, mstring queueName);

    void Put(const Message &message);

    unsigned int GetVisibility();
    void SetVisibility(unsigned int value);

    unsigned int GetSequenceDeviation();
    void SetSequenceDeviation(unsigned int value);

    void GetRelativeMsgID(BufferPointer value, unsigned int &size);
    void SetRelativeMsgID(const BufferPointer &value, unsigned int size);
};

/**
 * @class Dequeue
 *
 * @brief
 *
 */
class Dequeue : public HandleHolder<OCI_Dequeue *>
{
    friend class Environment;

public:

    Dequeue(const TypeInfo &typeInfo, mstring queueName);

    Message Get();

    Agent Listen(int timeout);

    mstring GetConsumer();
    void SetConsumer(mstring value);

    mstring GetCorrelation();
    void SetCorrelation(mstring value);

    void GetRelativeMsgID(BufferPointer value, unsigned int &size);
    void SetRelativeMsgID(const BufferPointer &value, unsigned int size);

    unsigned int GetVisibility();
    void SetVisibility(unsigned int value);

    unsigned int GetMode();
    void SetMode(unsigned int value);

    unsigned int GetNavigation();
    void SetNavigation(unsigned int value);

    int GetWaitTime();
    void SetWaitTime(int value);

    void SetAgents(std::vector<Agent> &agents);

    void Subscribe(unsigned int port, unsigned int timeout, NotifyAQHandlerProc handler);
    void Unsubscribe();

private:

    Dequeue(OCI_Dequeue *pDequeue);
};

/**
 * @class Queue
 *
 * @brief
 *
 */
class Queue
{
public:

    static void Create(const Connection &connection, mstring queue, mstring table, unsigned int queueType = OCI_AQT_NORMAL, unsigned int maxRetries = 0,
                       unsigned int retryDelay = 0, unsigned int retentionTime = 0, bool dependencyTracking = false, mstring comment = MT(""));
    static void Alter (const Connection &connection, mstring queue, unsigned int maxRetries= 0, unsigned int retryDelay= 0, unsigned int retentionTime= 0, mstring comment = MT(""));
    static void Drop  (const Connection &connection, mstring queue);
    static void Start (const Connection &connection, mstring queue, bool startEnqueue = true, bool startDequeue = true);
    static void Stop  (const Connection &connection, mstring queue, bool stopEnqueue = true, bool stopDequeue = true, bool wait = true);
};

/**
 * @class QueueTable
 *
 * @brief
 *
 */
class QueueTable
{
public:

    static void Create (const Connection &connection, mstring table, mstring payloadType, bool multipleConsumers, mstring storageClause = MT(""), mstring sortList = MT(""),
                        unsigned int messageGrouping = OCI_AGM_NONE, mstring comment = MT(""), unsigned int primaryInstance = 0, unsigned int secondaryInstance = 0, mstring compatible = MT(""));
    static void Alter  (const Connection &connection, mstring table, mstring comment, unsigned int primaryInstance = 0, unsigned int secondaryInstance = 0);
    static void Drop   (const Connection &connection, mstring table, bool force = true);
    static void Purge  (const Connection &connection, mstring table,  unsigned int deliveryMode, mstring purgeCondition = MT(""), bool block = true);
    static void Migrate(const Connection &connection, mstring table, mstring compatible = MT(""));
};

/**
 * @class DirectPath
 *
 * @brief
 *
 */
class DirectPath : public HandleHolder<OCI_DirPath *>
{
public:

    DirectPath(const TypeInfo &typeInfo, unsigned int nbCols, unsigned int  nbRows, mstring partition = MT(""));

    void SetColumn(unsigned int colIndex, mstring name, unsigned int maxSize,  mstring format = MT(""));
    void SetEntry(unsigned int rowIndex, unsigned int colIndex,  const dstring &value,  bool complete = true);
    void SetEntry(unsigned int rowIndex, unsigned int colIndex,  const BufferPointer &value, unsigned int size,  bool complete = true);

    void Reset();
    void Prepare();
    unsigned int Convert();
    unsigned int Load();
    void Finish();
    void Abort();
    void Save();
    void FlushRow();

    void SetCurrentRows(unsigned int nbRows);
    unsigned int GetCurrentRows();

    unsigned int GetMaxRows();
    unsigned int GetRowCount();
    unsigned int GetAffectedRows();

    void SetDateFormat(mstring format);

    void SetParallel(bool value);
    void SetNoLog(bool value);
    void SetCacheSize(unsigned int value);
    void SetBufferSize(unsigned int value);

    void SetConvertMode(unsigned int value);

    unsigned int GetErrorColumn();
    unsigned int GetErrorRow();
};

 /**
 *
 * @}
 */

/* ********************************************************************************************* *
 *                                         IMPLEMENTATION
 * ********************************************************************************************* */

inline void Check()
{
    OCI_Error *err = OCI_GetLastError();

    if (err)
    {
        throw Exception(err);
    }
}

template<class TResultType>
inline TResultType Check(TResultType result)
{
    Check();

    return result;
}

template<class TCharType>
inline std::basic_string<TCharType, std::char_traits<TCharType>, std::allocator<TCharType> > MakeString(const TCharType *result)
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
inline HandleHolder<THandleType>::HandleHolder(const HandleHolder &other) :  _smartHandle(0)
{
    Acquire(other, 0,  other._smartHandle ? other._smartHandle->GetParent() : 0);
}

template<class THandleType>
inline HandleHolder<THandleType>::~HandleHolder()
{
    Release();
}

template<class THandleType>
inline HandleHolder<THandleType>& HandleHolder<THandleType>::operator = (const HandleHolder<THandleType> &other)
{
    Acquire(other, 0, other._smartHandle ? other._smartHandle->GetParent() : 0);
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
    Release();

    if (func)
    {
        _smartHandle = new HandleHolder::SmartHandle<THandleType>(this, handle, func, parent);

       Environment::GetEnvironmentHandle().Handles.Set(handle, _smartHandle);
    }
    else
    {
        _smartHandle = dynamic_cast<SmartHandle<THandleType> *>(Environment::GetEnvironmentHandle().Handles.Get(handle));

        if (!_smartHandle)
        {
            _smartHandle = new HandleHolder::SmartHandle<THandleType>(this, handle, 0, parent);
        }
        else
        {
            _smartHandle->Acquire(this);
        }
    }
}

template<class THandleType>
inline void HandleHolder<THandleType>::Acquire(HandleHolder<THandleType> &other)
{
    if (&other != this && _smartHandle != other._smartHandle)
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

template <class TKey, class TValue>
inline void ConcurrentPool<TKey, TValue>::Initialize(unsigned int envMode)
{
    if (envMode & OCI_ENV_THREADED)
    {
        _mutex = Mutex::Create();
    }
}

template <class TKey, class TValue>
inline void ConcurrentPool<TKey, TValue>::Release()
{
    if (_mutex)
    {
        Mutex::Destroy(_mutex);
    }

    _mutex = 0;

    _map.clear();
}

template <class TKey, class TValue>
inline void ConcurrentPool<TKey, TValue>::Remove(TKey key)
{
    Lock();
    _map.erase(key);
    Unlock();
}

template <class TKey, class TValue>
inline TValue ConcurrentPool<TKey, TValue>::Get(TKey key)
{
    TValue value = 0;

    Lock();
    typename ConcurrentPoolMap::iterator it = _map.find(key);
    if (it != _map.end() )
    {
        value = it->second;
    }
    Unlock();

    return value;
}

template <class TKey, class TValue>
inline void ConcurrentPool<TKey, TValue>::Set(TKey key, TValue value)
{
    Lock();
    _map[key] = value;
    Unlock();
}

template <class TKey, class TValue>
inline void ConcurrentPool<TKey, TValue>::Lock()
{
    if (_mutex)
    {
        Mutex::Acquire(_mutex);
    }
}

template <class TKey, class TValue>
inline void ConcurrentPool<TKey, TValue>::Unlock()
{
    if (_mutex)
    {
        Mutex::Release(_mutex);
    }
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

    for(std::list<Handle *>::iterator it = _children.begin(); it != _children.end(); it++)
    {
        SmartHandle<TSmartHandleType> *handle = dynamic_cast<SmartHandle<TSmartHandleType> *> (*it);

        handle->_parent = 0;

        handle->DetachFromHolders();

        delete handle;
    }

    Environment::GetEnvironmentHandle().Handles.Remove(_handle);

    if (_func)
    {

        ret = _func(_handle);
        chk = TRUE;
    }

    if (chk)
    {
        Check(ret);
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
    Acquire(err, 0, 0);
}

inline mstring Exception::GetMessage()
{
    return MakeString(OCI_ErrorGetString(*this));
}

inline unsigned int Exception::GetType()
{
    return OCI_ErrorGetType(*this);
}

inline int Exception::GetOracleErrorCode()
{
    return OCI_ErrorGetOCICode(*this);
}

inline int Exception::GetInternalErrorCode()
{
    return OCI_ErrorGetInternalCode(*this);
}

inline Statement Exception::GetStatement()
{
    return Statement(OCI_ErrorGetStatement(*this), 0);
}

inline Connection Exception::GetConnection()
{
    return Connection(OCI_ErrorGetConnection(*this), 0);
}

inline unsigned int Exception::GetRow()
{
    return OCI_ErrorGetRow(*this);
}


/* --------------------------------------------------------------------------------------------- *
 * Environment
 * --------------------------------------------------------------------------------------------- */

inline void Environment::Initialize(unsigned int mode, mstring libpath)
{
    mode |=  OCI_ENV_CONTEXT;

    OCI_Initialize(0, libpath.c_str(),  mode);

    Check();

    GetEnvironmentHandle().Initialize((void *) OCI_HandleGetEnvironment(), mode);
}

inline void Environment::Cleanup()
{
    GetEnvironmentHandle().Finalize();

    OCI_Cleanup();

    Check();
}

inline unsigned int Environment::GetMode()
{
    return GetEnvironmentHandle().Mode;
}

inline unsigned int Environment::GetImportMode()
{
    return Check(OCI_GetImportMode());
}

inline unsigned int Environment::GetCharsetMetaData()
{
    return Check(OCI_GetCharsetMetaData());
}

inline unsigned int Environment::GetCharsetUserData()
{
    return Check(OCI_GetCharsetUserData());
}

inline unsigned int Environment::GetCompileVersion()
{
    return Check(OCI_GetOCICompileVersion());
}

inline unsigned int Environment::GetRuntimeVersion()
{
    return Check(OCI_GetOCIRuntimeVersion());
}

inline void Environment::EnableWarnings(bool value)
{
    OCI_EnableWarnings((boolean) value);

    Check();
}

inline void Environment::StartDatabase(mstring db, mstring user, mstring pwd, unsigned int start_flag, unsigned
                                       int startMode, unsigned int sessMode, mstring spfile)
{
    Check(OCI_DatabaseStartup(db.c_str(), user.c_str(), pwd.c_str(), sessMode, startMode, start_flag, spfile.c_str() ));
}

inline void Environment::ShutdownDatabase(mstring db, mstring user, mstring pwd, unsigned int shut_flag, unsigned int shutMode,
                                          unsigned int sessMode)
{
    Check(OCI_DatabaseShutdown(db.c_str(), user.c_str(), pwd.c_str(), sessMode, shutMode, shut_flag ));
}

inline void Environment::ChangeUserPassword(mstring db, mstring user, mstring pwd, mstring newPassword)
{
    Check(OCI_SetUserPassword(db.c_str(), user.c_str(), pwd.c_str(), newPassword.c_str()));
}

inline void Environment::SetHAHandler(HAHandlerProc handler)
{
    Environment::CallbackPool & pool = GetEnvironmentHandle().Callbacks;

    Check(OCI_SetHAHandler((POCI_HA_HANDLER ) (handler != 0 ? Environment::HAHandler : 0 )));

    pool.Set((UnknownHandle) GetEnvironmentHandle(), (CallbackPointer) handler);
}

inline void Environment::HAHandler(OCI_Connection *pConnection, unsigned int source, unsigned int event, OCI_Timestamp  *pTimestamp)
{
    HAHandlerProc handler = (HAHandlerProc) GetEnvironmentHandle().Callbacks.Get((void *) GetEnvironmentHandle());

    if (handler)
    {
        Connection connection(pConnection, 0);
        Timestamp timestamp(pTimestamp, connection.GetHandle());

        handler(connection, source, event, timestamp);
    }
}

inline void Environment::TAFHandler(OCI_Connection *pConnection, unsigned int type, unsigned int event)
{
    TAFHandlerProc handler = (TAFHandlerProc) GetEnvironmentHandle().Callbacks.Get(pConnection);

    if (handler)
    {
        Connection connection(pConnection, 0);

        handler(connection, type, event);
    }
}

inline void Environment::NotifyHandler(OCI_Event *pEvent)
{
    NotifyHandlerProc handler = (NotifyHandlerProc) GetEnvironmentHandle().Callbacks.Get(Check(OCI_EventGetSubscription(pEvent)));

    if (handler)
    {
        Event evt(pEvent);
        handler(evt);
    }
}

inline void Environment::NotifyHandlerAQ(OCI_Dequeue *pDequeue)
{
    NotifyAQHandlerProc handler = (NotifyAQHandlerProc) GetEnvironmentHandle().Callbacks.Get(Check(pDequeue));

    if (handler)
    {
        Dequeue dequeue(pDequeue);
        handler(dequeue);
    }
}

inline Environment::EnvironmentHandle & Environment::GetEnvironmentHandle()
{
    static EnvironmentHandle envHandle;

    return envHandle;
}

inline void Environment::EnvironmentHandle::Initialize(void *handle, unsigned int envMode)
{
    Mode = envMode;
    Callbacks.Initialize(envMode);
    Handles.Initialize(envMode);

    Acquire(handle, 0, 0);
}

inline void Environment::EnvironmentHandle::Finalize()
{
    Callbacks.Release();
    Handles.Release();

    Release();
}

/* --------------------------------------------------------------------------------------------- *
 * Mutex
 * --------------------------------------------------------------------------------------------- */

inline MutexHandle Mutex::Create()
{
    return Check(OCI_MutexCreate());
}

inline void Mutex::Destroy(MutexHandle mutex)
{
    Check(OCI_MutexFree(mutex));
}

inline void Mutex::Acquire(MutexHandle mutex)
{
    Check(OCI_MutexAcquire(mutex));
}

inline void Mutex::Release(MutexHandle mutex)
{
    Check(OCI_MutexRelease(mutex));
}

/* --------------------------------------------------------------------------------------------- *
 * Thread
 * --------------------------------------------------------------------------------------------- */

inline ThreadHandle Thread::Create()
{
    return Check(OCI_ThreadCreate());
}

inline void Thread::Destroy(ThreadHandle handle)
{
    Check(OCI_ThreadFree(handle));
}

inline void Thread::Run(ThreadHandle handle, ThreadProc func, void *args)
{
    Check(OCI_ThreadRun(handle, func, args));
}

inline void Thread::Join(ThreadHandle handle)
{
    Check(OCI_ThreadJoin(handle));
}

/* --------------------------------------------------------------------------------------------- *
 * ThreadKey
 * --------------------------------------------------------------------------------------------- */

inline void ThreadKey::Create(mstring name, ThreadKeyFreeProc freeProc)
{
    Check(OCI_ThreadKeyCreate(name.c_str(), freeProc));
}

inline void ThreadKey::SetValue(mstring name, void *value)
{
    Check(OCI_ThreadKeySetValue(name.c_str(), value));
}

inline void * ThreadKey::GetValue(mstring name)
{
    return Check(OCI_ThreadKeyGetValue(name.c_str()));
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

    Acquire(Check(OCI_PoolCreate(db.c_str(), user.c_str(), pwd.c_str(), poolType, sessionMode,
            minCon, maxCon, incrCon)), (HandleFreeFunc) OCI_PoolFree, 0);
}

inline void Pool::Close()
{
    Release();
}

inline Connection Pool::GetConnection(mstring sessionTag)
{
    return Connection(Check( OCI_PoolGetConnection(*this, sessionTag.c_str())), GetHandle());
}

inline unsigned int Pool::GetTimeout()
{
    return Check( OCI_PoolGetTimeout(*this));
}

inline void Pool::SetTimeout(unsigned int value)
{
    Check( OCI_PoolSetTimeout(*this, value));
}

inline bool Pool::GetNoWait()
{
    return (Check( OCI_PoolGetNoWait(*this)) == TRUE);
}

inline void Pool::SetNoWait(bool value)
{
    Check( OCI_PoolSetNoWait(*this, value));
}

inline unsigned int Pool::GetBusyConnectionsCount()
{
    return Check( OCI_PoolGetBusyCount(*this));
}

inline unsigned int Pool::GetOpenedConnectionsCount()
{
    return Check( OCI_PoolGetOpenedCount(*this));
}

inline unsigned int Pool::GetMinSize()
{
    return Check( OCI_PoolGetMin(*this));
}

inline unsigned int Pool::GetMaxSize()
{
    return Check( OCI_PoolGetMax(*this));
}

inline unsigned int Pool::GetIncrement()
{
    return Check( OCI_PoolGetIncrement(*this));
}

inline unsigned int Pool::GetStatementCacheSize()
{
    return Check( OCI_PoolGetStatementCacheSize(*this));
}

inline void Pool::SetStatementCacheSize(unsigned int value)
{
    Check( OCI_PoolSetStatementCacheSize(*this, value));
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
    Acquire(con, parent ? (HandleFreeFunc) OCI_ConnectionFree : 0, parent);
}

inline void Connection::Open(mstring db, mstring user, mstring pwd, unsigned int sessionMode)
{
    Acquire(Check(OCI_ConnectionCreate(db.c_str(), user.c_str(), pwd.c_str(), sessionMode)),
            (HandleFreeFunc) OCI_ConnectionFree, Environment::GetEnvironmentHandle().GetHandle());
}

inline void Connection::Close()
{
    Release();
}

inline void Connection::Commit()
{
    Check(OCI_Commit(*this));
}

inline void Connection::Rollback()
{
    Check(OCI_Rollback(*this));
}

inline void Connection::Break()
{
    Check(OCI_Break(*this));
}

inline void Connection::SetAutoCommit(bool enabled)
{
    Check(OCI_SetAutoCommit(*this, enabled));
}

inline bool Connection::GetAutoCommit()
{
    return (Check(OCI_GetAutoCommit(*this)) == TRUE);
}

inline bool Connection::IsServerAlive()
{
    return (Check(OCI_IsConnected(*this)) == TRUE);
}

inline bool Connection::PingServer()
{
    return( Check(OCI_Ping(*this)) == TRUE);
}

inline mstring Connection::GetConnectionString()
{
    return MakeString(Check(OCI_GetDatabase(*this)));
}

inline mstring Connection::GetUserName()
{
    return MakeString(Check(OCI_GetUserName(*this)));
}

inline mstring Connection::GetPassword()
{
    return MakeString(Check(OCI_GetPassword(*this)));
}

inline unsigned int Connection::GetVersion()
{
    return Check(OCI_GetVersionConnection(*this));
}

inline mstring Connection::GetServerVersion()
{
    return MakeString(Check( OCI_GetVersionServer(*this)));
}

inline unsigned int Connection::GetServerMajorVersion()
{
    return Check(OCI_GetServerMajorVersion(*this));
}

inline unsigned int Connection::GetServerMinorVersion()
{
    return Check(OCI_GetServerMinorVersion(*this));
}

inline unsigned int Connection::GetServerRevisionVersion()
{
    return Check(OCI_GetServerRevisionVersion(*this));
}

inline void Connection::ChangePassword(mstring newPassword)
{
    Check(OCI_SetPassword(*this, newPassword.c_str()));
}

inline mstring Connection::GetSessionTag()
{
    return MakeString(Check(OCI_GetSessionTag(*this)));
}

inline void Connection::SetSessionTag(mstring tag)
{
    Check(OCI_SetSessionTag(*this, tag.c_str()));
}

inline Transaction Connection::GetTransaction()
{
    return Transaction(Check(OCI_GetTransaction(*this)));
}

inline void Connection::SetTransaction(const Transaction &transaction)
{
    Check(OCI_SetTransaction(*this, transaction));
}

inline void Connection::SetDefaultDateFormat(mstring format)
{
    Check(OCI_SetDefaultFormatDate(*this, format.c_str()));
}

inline void Connection::SetDefaultNumericFormat(mstring format)
{
    Check(OCI_SetDefaultFormatNumeric(*this, format.c_str()));
}

inline mstring  Connection::GetDefaultDateFormat()
{
    return MakeString(Check(OCI_GetDefaultFormatDate(*this)));
}

inline mstring  Connection::GetDefaultNumericFormat()
{
   return  MakeString(Check(OCI_GetDefaultFormatNumeric(*this)));
}

inline void Connection::EnableServerOutput(unsigned int bufsize, unsigned int arrsize, unsigned int lnsize)
{
    Check(OCI_ServerEnableOutput(*this, bufsize, arrsize, lnsize));
}

inline void Connection::DisableServerOutput()
{
    Check(OCI_ServerDisableOutput(*this));
}

inline bool Connection::GetServerOutput(mstring &line)
{
    const mtext * str = Check(OCI_ServerGetOutput(*this));

    line = MakeString(str);

    return (str != 0);
}

inline void Connection::GetServerOutput(std::vector<mstring> &lines)
{
    const mtext * str = Check(OCI_ServerGetOutput(*this));

    while (str)
    {
        lines.push_back(str);
        str = Check(OCI_ServerGetOutput(*this));
    }
}

inline void Connection::SetTrace(unsigned int trace, mstring value)
{
    Check(OCI_SetTrace(*this, trace, value.c_str()));
}

inline mstring Connection::GetTrace(unsigned int trace)
{
    return MakeString(Check(OCI_GetTrace(*this, trace)));
}

inline mstring Connection::GetDatabase()
{
    return MakeString(Check(OCI_GetDBName(*this)));
}

inline mstring Connection::GetInstance()
{
    return Check(OCI_GetInstanceName(*this));
}

inline mstring Connection::GetService()
{
    return MakeString(Check(OCI_GetServiceName(*this)));
}

inline mstring Connection::GetServer()
{
    return Check(OCI_GetServerName(*this));
}

inline mstring Connection::GetDomain()
{
    return MakeString(Check(OCI_GetDomainName(*this)));
}

inline Timestamp Connection::GetInstanceStartTime()
{
    return Timestamp(Check(OCI_GetInstanceStartTime(*this)), GetHandle());
}

inline unsigned int Connection::GetStatementCacheSize()
{
    return Check(OCI_GetStatementCacheSize(*this));
}

inline void Connection::SetStatementCacheSize(unsigned int value)
{
    Check(OCI_SetStatementCacheSize(*this, value));
}

inline unsigned int Connection::GetDefaultLobPrefetchSize()
{
    return Check(OCI_GetDefaultLobPrefetchSize  (*this));
}

inline void Connection::SetDefaultLobPrefetchSize(unsigned int value)
{
    Check(OCI_SetDefaultLobPrefetchSize(*this, value));
}

inline bool Connection::IsTAFCapable()
{
    return (Check(OCI_IsTAFCapable(*this)) == TRUE);
}

inline void Connection::SetTAFHandler(TAFHandlerProc handler)
{
    Environment::CallbackPool & pool = Environment::GetEnvironmentHandle().Callbacks;

    Check(OCI_SetTAFHandler(*this, (POCI_TAF_HANDLER ) (handler != 0 ? Environment::TAFHandler : 0 )));

    pool.Set((OCI_Connection*) *this, (CallbackPointer) handler);
}

/* --------------------------------------------------------------------------------------------- *
 * Transaction
 * --------------------------------------------------------------------------------------------- */

inline Transaction::Transaction(const Connection &connection, unsigned int timeout, unsigned int mode, OCI_XID *pxid)
{
    Acquire(Check(OCI_TransactionCreate(connection, timeout, mode, pxid)), (HandleFreeFunc) OCI_TransactionFree, 0);
}

inline Transaction::Transaction(OCI_Transaction *trans)
{
    Acquire(trans, 0, 0);
}

inline void Transaction::Prepare()
{
    Check(OCI_TransactionPrepare(*this));
}

inline void Transaction::Start()
{
    Check(OCI_TransactionStart(*this));
}

inline void Transaction::Stop()
{
    Check(OCI_TransactionStop(*this));
}

inline void Transaction::Resume()
{
    Check(OCI_TransactionResume(*this));
}

inline void Transaction::Forget()
{
    Check(OCI_TransactionForget(*this));
}

inline unsigned int Transaction::GetMode()
{
    return Check(OCI_TransactionGetMode(*this));
}

inline unsigned int Transaction::GetTimeout()
{
    return Check(OCI_TransactionGetTimeout(*this));
}

/* --------------------------------------------------------------------------------------------- *
 * Date
 * --------------------------------------------------------------------------------------------- */

inline Date::Date()
{
    Acquire(Check(OCI_DateCreate(NULL)), (HandleFreeFunc) OCI_DateFree, 0);
}

inline Date::Date(OCI_Date *pDate, Handle *parent)
{
    Acquire(pDate, 0, parent);
}

inline void Date::Assign(const Date& other)
{
    Check(OCI_DateAssign(*this, other));
}

inline int Date::Compare(const Date& other)
{
    return Check(OCI_DateCompare(*this, other));
}

inline int Date::DaysBetween(const Date& other)
{
    return Check(OCI_DateDaysBetween(*this, other));
}

inline void Date::SetDate(int year, int month, int day)
{
    Check(OCI_DateSetDate(*this, year, month, day));
}

inline void Date::SetTime(int hour, int min, int sec)
{
    Check(OCI_DateSetTime(*this, hour, min , sec));
}

inline void Date::SetDateTime(int year, int month, int day, int hour, int min, int sec)
{
    Check(OCI_DateSetDateTime(*this, year, month, day, hour, min , sec));
}

inline void Date::GetDate(int *year, int *month, int *day)
{
    Check(OCI_DateGetDate(*this, year, month, day));
}

inline void Date::GetTime(int *hour, int *min, int *sec)
{
    Check(OCI_DateGetTime(*this, hour, min , sec));
}

inline void Date::GetDateTime(int *year, int *month, int *day, int *hour, int *min, int *sec)
{
    Check(OCI_DateGetDateTime(*this, year, month, day, hour, min , sec));
}

inline void Date::AddDays(int days)
{
    Check(OCI_DateAddDays(*this, days));
}

inline void Date::AddMonths(int months)
{
    OCI_DateAddMonths(*this, months);
}

inline void Date::SysDate()
{
   Check(OCI_DateSysDate(*this));
}

inline void Date::NextDay(mstring day)
{
   Check(OCI_DateNextDay(*this, day.c_str()));
}

inline void Date::LastDay()
{
    Check(OCI_DateLastDay(*this));
}

inline void Date::ChangeTimeZone(mstring tzSrc, mstring tzDst)
{
    Check(OCI_DateZoneToZone(*this, tzSrc.c_str(), tzDst.c_str()));
}

inline void Date::FromString(mstring data, mstring format)
{
   Check(OCI_DateFromText(*this, data.c_str(), format.c_str()));
}

inline mstring Date::ToString(mstring format)
{
    size_t size = OCI_SIZE_BUFFER;

    ManagedBuffer<mtext> buffer = new mtext[size+1];

    Check(OCI_DateToText(*this, format.c_str(), (int) size, (mtext *) buffer));

    return MakeString((mtext *) buffer);
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
    Acquire(Check(OCI_IntervalCreate(NULL, type)), (HandleFreeFunc) OCI_IntervalFree, 0);
}

inline Interval::Interval(OCI_Interval *pInterval, Handle *parent)
{
    Acquire(pInterval, 0, parent);
}

inline void Interval::Assign(const Interval& other)
{
   Check(OCI_IntervalAssign(*this, other));
}

inline int Interval::Compare(const Interval& other)
{
    return Check(OCI_IntervalCompare(*this, other));
}

inline unsigned int Interval::GetType()
{
    return Check(OCI_IntervalGetType(*this));
}

inline void Interval::Add(const Interval& other)
{
     Check(OCI_IntervalAdd(*this, other));
}

inline void Interval::Substract(const Interval& other)
{
    Check(OCI_IntervalSubtract(*this, other));
}

inline void Interval::GetDaySecond(int *day, int *hour, int *min, int *sec, int *fsec)
{
    Check(OCI_IntervalGetDaySecond(*this, day, hour, min, sec, fsec));
}

inline void Interval::SetDaySecond(int day, int hour, int min, int sec, int fsec)
{
    Check(OCI_IntervalSetDaySecond(*this, day, hour, min, sec, fsec));
}

inline void Interval::GetYearMonth(int *year, int *month)
{
    Check(OCI_IntervalGetYearMonth(*this, year, month));
}
inline void Interval::SetYearMonth(int year, int month)
{
    Check(OCI_IntervalSetYearMonth(*this, year, month));
}

inline void Interval::FromTimeZone(mstring timeZone)
{
    Check(OCI_IntervalFromTimeZone(*this, timeZone.c_str()));
}

inline void Interval::FromString(mstring data, mstring format)
{
    Check(OCI_IntervalFromText(*this, format.c_str()));
}

inline mstring Interval::ToString(int leadingPrecision, int fractionPrecision)
{
    size_t size = OCI_SIZE_BUFFER;

    ManagedBuffer<mtext> buffer = new mtext[size+1];

    Check(OCI_IntervalToText(*this, leadingPrecision, fractionPrecision,  (int) size, (mtext *) buffer));

    return MakeString((mtext *) buffer);
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
    Acquire(Check(OCI_TimestampCreate(NULL, type)), (HandleFreeFunc) OCI_TimestampFree, 0);
}

inline Timestamp::Timestamp(OCI_Timestamp *pTimestamp, Handle *parent)
{
    Acquire(pTimestamp, 0, parent);
}

inline void Timestamp::Assign(const Timestamp& other)
{
    Check(OCI_TimestampAssign(*this, other));
}

inline int Timestamp::Compare(const Timestamp& other)
{
    return Check(OCI_TimestampCompare(*this, other));
}

inline unsigned int Timestamp::GetType()
{
    return Check(OCI_TimestampGetType(*this));
}

inline void Timestamp::Construct(int year, int month, int day, int hour, int min, int sec, int fsec, mstring timeZone)
{
    Check(OCI_TimestampConstruct(*this, year, month, day, hour, min,sec, fsec, timeZone.c_str()));
}

inline void Timestamp::Convert(const Timestamp& other)
{
    Check(OCI_TimestampConvert(*this, other));
}

inline void Timestamp::GetDate(int *year, int *month, int *day)
{
    Check(OCI_TimestampGetDate(*this, year, month, day));
}

inline void Timestamp::GetTime(int *hour, int *min, int *sec, int *fsec)
{
    Check(OCI_TimestampGetTime(*this, hour, min, sec, fsec));
}

inline void Timestamp::GetDateTime(int *year, int *month, int *day, int *hour, int *min, int *sec, int *fsec)
{
    Check(OCI_TimestampGetDateTime(*this, year, month, day, hour, min, sec, fsec));
}

inline mstring Timestamp::GetTimeZone()
{
    size_t size = OCI_SIZE_BUFFER;

    ManagedBuffer<mtext> buffer = new mtext[size+1];

    Check(OCI_TimestampGetTimeZoneName(*this,  (int) size, (mtext *) buffer));

    return MakeString((mtext *) buffer);
}

inline void Timestamp::GetTimeZoneOffset(int *hour, int *min)
{
    Check(OCI_TimestampGetTimeZoneOffset(*this, hour, min));
}

inline void Timestamp::AddInterval(const Interval& other)
{
    Check(OCI_TimestampIntervalAdd(*this, other));
}

inline void Timestamp::SubstractInterval(const Interval& other)
{
    Check(OCI_TimestampIntervalSub(*this, other));
}

inline void Timestamp::Substract(const Timestamp &other, Interval &result)
{
    Check(OCI_TimestampSubtract(*this, other, result));
}

inline void Timestamp::SysTimestamp()
{
    Check(OCI_TimestampSysTimestamp(*this));
}

inline void Timestamp::FromString(mstring data, mstring format)
{
    Check(OCI_TimestampFromText(*this, data.c_str(), format.c_str()));
}

inline mstring Timestamp::ToString(mstring format, int precision)
{
    size_t size = OCI_SIZE_BUFFER;

    ManagedBuffer<mtext> buffer = new mtext[size+1];

    Check(OCI_TimestampToText(*this, format.c_str(), (int) size, (mtext *) buffer, precision));

    return MakeString((mtext *) buffer);
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
    Acquire(Check(OCI_LobCreate(connection, OCI_CLOB)), (HandleFreeFunc) OCI_LobFree, connection.GetHandle());
}

inline Clob::Clob(OCI_Lob *pLob, Handle *parent)
{
    Acquire(pLob, 0, parent);
}

inline dstring Clob::Read(unsigned int size)
{
    ManagedBuffer<dtext> buffer = new dtext[size+1];

    size = Check(OCI_LobRead(*this, (void *) buffer, size));

    return MakeString( (const dtext *) buffer);
}

inline unsigned int Clob::Write(dstring content)
{
    return Check(OCI_LobWrite(*this, (void *) content.c_str(), content.size()));
}

inline unsigned int Clob::Append(dstring content)
{
    return Check(OCI_LobAppend(*this, (void *) content.c_str(), content.size()));
}

inline bool Clob::Seek(unsigned int seekMode, big_uint offset)
{
    return (Check(OCI_LobSeek(*this, offset, seekMode)) == TRUE);
}

inline void Clob::Append(const Clob &other)
{
    Check(OCI_LobAppendLob(*this, other));
}

inline void Clob::Assign(const Clob &other)
{
    Check(OCI_LobAssign(*this, other));
}

inline bool Clob::Equals(const Clob &other)
{
    return (Check(OCI_LobIsEqual(*this, other)) == TRUE);
}

inline big_uint Clob::GetOffset()
{
    return Check(OCI_LobGetOffset(*this));
}

inline big_uint Clob::GetLength()
{
    return Check(OCI_LobGetLength(*this));
}

inline big_uint Clob::GetMaxSize()
{
    return Check(OCI_LobGetMaxSize(*this));
}

inline big_uint Clob::GetChunkSize()
{
    return Check(OCI_LobGetChunkSize(*this));
}

inline void Clob::Truncate(big_uint size)
{
    Check(OCI_LobTruncate(*this, size));
}

inline big_uint Clob::Erase(big_uint offset, big_uint size)
{
    return Check(OCI_LobErase(*this, offset, size));
}

inline void Clob::Copy(Clob &dest, big_uint offset, big_uint offsetDest, big_uint size)
{
    Check(OCI_LobCopy(dest, *this, offsetDest, offset, size));
}

inline bool Clob::IsTemporary()
{
    return (Check(OCI_LobIsTemporary(*this)) == TRUE);
}

inline void Clob::Open(unsigned int mode)
{
    Check(OCI_LobOpen(*this, mode));
}

inline void Clob::Flush()
{
    Check(OCI_LobFlush(*this));
}

inline void Clob::Close()
{
    Check(OCI_LobClose(*this));
}

inline void Clob::EnableBuffering(bool value)
{
    Check(OCI_LobEnableBuffering(*this, value));
}

/* --------------------------------------------------------------------------------------------- *
 * Blob
 * --------------------------------------------------------------------------------------------- */

inline Blob::Blob(const Connection &connection)
{
    Acquire(Check(OCI_LobCreate(connection, OCI_BLOB)), (HandleFreeFunc) OCI_LobFree, connection.GetHandle());
}

inline Blob::Blob(OCI_Lob *pLob, Handle *parent)
{
    Acquire(pLob, 0, parent);
}

inline unsigned int Blob::Read(void *buffer, unsigned int size)
{
    return Check(OCI_LobRead(*this, buffer, size));
}

inline unsigned int Blob::Write(void *buffer, unsigned int size)
{
    return Check(OCI_LobWrite(*this, buffer, size));
}

inline unsigned int Blob::Append(void *buffer, unsigned int size)
{
    return Check(OCI_LobAppend(*this, buffer, size));
}

inline bool Blob::Seek(unsigned int seekMode, big_uint offset)
{
    return (Check(OCI_LobSeek(*this, offset, seekMode)) == TRUE);
}

inline void Blob::Append(const Blob &other)
{
    Check(OCI_LobAppendLob(*this, other));
}

inline void Blob::Assign(const Blob &other)
{
    Check(OCI_LobAssign(*this, other));
}

inline bool Blob::Equals(const Blob &other)
{
    return (Check(OCI_LobIsEqual(*this, other)) == TRUE);
}

inline big_uint Blob::GetOffset()
{
    return Check(OCI_LobGetOffset(*this));
}

inline big_uint Blob::GetLength()
{
    return Check(OCI_LobGetLength(*this));
}

inline big_uint Blob::GetMaxSize()
{
    return Check(OCI_LobGetMaxSize(*this));
}

inline big_uint Blob::GetChunkSize()
{
    return Check(OCI_LobGetChunkSize(*this));
}

inline void Blob::Truncate(big_uint size)
{
    Check(OCI_LobTruncate(*this, size));
}

inline big_uint Blob::Erase(big_uint offset, big_uint size)
{
    return Check(OCI_LobErase(*this, offset, size));
}

inline void Blob::Copy(Blob &dest, big_uint offset, big_uint offsetDest, big_uint size)
{
    Check(OCI_LobCopy(dest, *this, offsetDest, offset, size));
}

inline bool Blob::IsTemporary()
{
    return (Check(OCI_LobIsTemporary(*this)) == TRUE);
}

inline void Blob::Open(unsigned int mode)
{
    Check(OCI_LobOpen(*this, mode));
}

inline void Blob::Flush()
{
    Check(OCI_LobFlush(*this));
}

inline void Blob::Close()
{
    Check(OCI_LobClose(*this));
}

inline void Blob::EnableBuffering(bool value)
{
    Check(OCI_LobEnableBuffering(*this, value));
}

/* --------------------------------------------------------------------------------------------- *
 * File
 * --------------------------------------------------------------------------------------------- */

inline File::File(const Connection &connection)
{
    Acquire(Check(OCI_FileCreate(connection, OCI_BFILE)), (HandleFreeFunc) OCI_FileFree, connection.GetHandle());
}

inline File::File(const Connection &connection, mstring directory, mstring name)
{
    Acquire(Check(OCI_FileCreate(connection, OCI_BFILE)), (HandleFreeFunc) OCI_FileFree, connection.GetHandle());

    SetInfos(directory, name);
}

inline File::File(OCI_File *pFile, Handle *parent)
{
    Acquire(pFile, 0, parent);
}

inline unsigned int File::Read(void *buffer, unsigned int size)
{
    return Check(OCI_FileRead(*this, buffer, size));
}

inline bool File::Seek(unsigned int seekMode, big_uint offset)
{
    return (Check(OCI_FileSeek(*this, offset, seekMode)) == TRUE);
}

inline void File::Assign(const File &other)
{
    Check(OCI_FileAssign(*this, other));
}

inline bool File::Equals(const File &other)
{
    return (Check(OCI_FileIsEqual(*this, other)) == TRUE);
}

inline big_uint File::GetOffset()
{
    return Check(OCI_FileGetOffset(*this));
}

inline big_uint File::GetSize()
{
    return Check(OCI_FileGetSize(*this));
}

inline bool File::Exists()
{
    return (Check(OCI_FileExists(*this)) == TRUE);
}

inline void File::SetInfos(mstring directory, mstring name)
{
    Check(OCI_FileSetName(*this, directory.c_str(), name.c_str()));
}

inline mstring File::GetName()
{
    return MakeString(Check(OCI_FileGetName(*this)));
}

inline mstring File::GetDirectory()
{
    return MakeString(Check(OCI_FileGetDirectory(*this)));
}

inline void File::Open()
{
    Check(OCI_FileOpen(*this));
}

inline bool File::IsOpened()
{
    return (Check(OCI_FileIsOpen(*this)) == TRUE);
}

inline void File::Close()
{
    Check(OCI_FileClose(*this));
}

/* --------------------------------------------------------------------------------------------- *
 * TypeInfo
 * --------------------------------------------------------------------------------------------- */

inline TypeInfo::TypeInfo(const Connection &connection, mstring name, unsigned int type)
{
    Acquire(Check(OCI_TypeInfoGet(connection, name.c_str(), type)), (HandleFreeFunc) 0, connection.GetHandle());
}

inline TypeInfo::TypeInfo(OCI_TypeInfo *pTypeInfo)
{
    Acquire(pTypeInfo, 0, 0);
}

inline unsigned int TypeInfo::GetType()
{
    return Check(OCI_TypeInfoGetType(*this));
}

inline mstring TypeInfo::GetName()
{
    return Check(OCI_TypeInfoGetName(*this));
}

inline Connection TypeInfo::GetConnection()
{
    return Connection(Check(OCI_TypeInfoGetConnection(*this)), 0);
}

inline Connection TypeInfo::GetConnection() const
{
    return Connection(Check(OCI_TypeInfoGetConnection(*this)), 0);
}

inline unsigned int TypeInfo::GetColumnCount()
{
    return Check(OCI_TypeInfoGetColumnCount(*this));
}

inline Column TypeInfo::GetColumn(unsigned int index)
{
    return Column(Check(OCI_TypeInfoGetColumn(*this, index)), GetHandle());
}

/* --------------------------------------------------------------------------------------------- *
 * Object
 * --------------------------------------------------------------------------------------------- */

inline Object::Object(const TypeInfo &typeInfo)
{
    Connection connection = typeInfo.GetConnection();
    Acquire(Check(OCI_ObjectCreate(connection, typeInfo)), (HandleFreeFunc) OCI_ObjectFree, connection.GetHandle());
}

inline Object::Object(OCI_Object *pObject, Handle *parent)
{
    Acquire(pObject, 0, parent);
}

inline void Object::Assign(const Object& other)
{
    Check(OCI_ObjectAssign(*this, other));
}

inline bool Object::IsAttributeNull(mstring name)
{
    return (Check(OCI_ObjectIsNull(*this, name.c_str())) == TRUE);
}

inline void Object::SetAttributeNull(mstring name)
{
    Check(OCI_ObjectSetNull(*this, name.c_str()));
}

inline TypeInfo Object::GetTypeInfo()
{
    return TypeInfo(Check(OCI_ObjectGetTypeInfo(*this)));
}

inline Reference Object::GetReference()
{
    TypeInfo typeInfo = GetTypeInfo();
    Connection connection = typeInfo.GetConnection();

    OCI_Ref *pRef = OCI_RefCreate(connection, typeInfo);
    
    Check(OCI_ObjectGetSelfRef(*this, pRef));
    
    return Reference(pRef, GetHandle());
}

template<>
inline short Object::Get<short>(mstring name)
{
    return Check(OCI_ObjectGetShort(*this, name.c_str()));
}

template<>
inline unsigned short Object::Get<unsigned short>(mstring name)
{
    return Check(OCI_ObjectGetUnsignedShort(*this, name.c_str()));
}

template<>
inline int Object::Get<int>(mstring name)
{
    return Check(OCI_ObjectGetInt(*this, name.c_str()));
}

template<>
inline unsigned int Object::Get<unsigned int>(mstring name)
{
    return Check(OCI_ObjectGetUnsignedInt(*this, name.c_str()));
}

template<>
inline big_int Object::Get<big_int>(mstring name)
{
    return Check(OCI_ObjectGetBigInt(*this, name.c_str()));
}

template<>
inline big_uint Object::Get<big_uint>(mstring name)
{
    return Check(OCI_ObjectGetUnsignedBigInt(*this, name.c_str()));
}

template<>
inline float Object::Get<float>(mstring name)
{
    return Check(OCI_ObjectGetFloat(*this, name.c_str()));
}

template<>
inline double Object::Get<double>(mstring name)
{
    return Check(OCI_ObjectGetDouble(*this, name.c_str()));
}

template<>
inline dstring Object::Get<dstring>(mstring name)
{
    return MakeString(Check(OCI_ObjectGetString(*this,name.c_str())));
}

template<>
inline Date Object::Get<Date>(mstring name)
{
    return Date(Check(OCI_ObjectGetDate(*this,name.c_str())), GetHandle());
}

template<>
inline Timestamp Object::Get<Timestamp>(mstring name)
{
    return Timestamp(Check(OCI_ObjectGetTimestamp(*this,name.c_str())), GetHandle());
}

template<>
inline Interval Object::Get<Interval>(mstring name)
{
    return Interval(Check(OCI_ObjectGetInterval(*this,name.c_str())), GetHandle());
}

template<>
inline Object Object::Get<Object>(mstring name)
{
    return Object(Check(OCI_ObjectGetObject(*this,name.c_str())), GetHandle());
}

template<>
inline Collection Object::Get<Collection>(mstring name)
{
    return Collection(Check(OCI_ObjectGetColl(*this,name.c_str())), GetHandle());
}

template<>
inline Reference Object::Get<Reference>(mstring name)
{
    return Reference(Check(OCI_ObjectGetRef(*this,name.c_str())), GetHandle());
}

template<>
inline Clob Object::Get<Clob>(mstring name)
{
    return Clob(Check(OCI_ObjectGetLob(*this,name.c_str())), GetHandle());
}

template<>
inline Blob Object::Get<Blob>(mstring name)
{
    return Blob(Check(OCI_ObjectGetLob(*this,name.c_str())), GetHandle());
}

template<>
inline File Object::Get<File>(mstring name)
{
    return File(Check(OCI_ObjectGetFile(*this,name.c_str())), GetHandle());
}

template<>
inline void Object::Get<BufferPointer>(mstring name, BufferPointer value, unsigned int &size)
{
    Check(OCI_ObjectGetRaw(*this,name.c_str(), value, size));
}

template<>
inline void Object::Set<short>(mstring name, const short &value)
{
    Check(OCI_ObjectSetShort(*this, name.c_str(), value));
}

template<>
inline void Object::Set<unsigned short>(mstring name, const unsigned short &value)
{
    Check(OCI_ObjectSetUnsignedShort(*this, name.c_str(), value));
}

template<>
inline void Object::Set<int>(mstring name, const int &value)
{
    Check(OCI_ObjectSetInt(*this, name.c_str(), value));
}

template<>
inline void Object::Set<unsigned int>(mstring name, const unsigned int &value)
{
    Check(OCI_ObjectSetUnsignedInt(*this, name.c_str(), value));
}

template<>
inline void Object::Set<big_int>(mstring name, const big_int &value)
{
    Check(OCI_ObjectSetBigInt(*this, name.c_str(), value));
}

template<>
inline void Object::Set<big_uint>(mstring name, const big_uint &value)
{
    Check(OCI_ObjectSetUnsignedBigInt(*this, name.c_str(), value));
}

template<>
inline void Object::Set<float>(mstring name, const float &value)
{
    Check(OCI_ObjectSetFloat(*this, name.c_str(), value));
}

template<>
inline void Object::Set<double>(mstring name, const double &value)
{
    Check(OCI_ObjectSetDouble(*this, name.c_str(), value));
}

template<>
inline void Object::Set<dstring>(mstring name, const dstring &value)
{
    Check(OCI_ObjectSetString(*this, name.c_str(), value.c_str()));
}

template<>
inline void Object::Set<Date>(mstring name, const Date &value)
{
    Check(OCI_ObjectSetDate(*this, name.c_str(), value));
}

template<>
inline void Object::Set<Timestamp>(mstring name, const Timestamp &value)
{
    Check(OCI_ObjectSetTimestamp(*this, name.c_str(), value));
}

template<>
inline void Object::Set<Interval>(mstring name, const Interval &value)
{
    Check(OCI_ObjectSetInterval(*this, name.c_str(), value));
}

template<>
inline void Object::Set<Object>(mstring name, const Object &value)
{
    Check(OCI_ObjectSetObject(*this, name.c_str(), value));
}

template<>
inline void Object::Set<Collection>(mstring name, const Collection &value)
{
    Check(OCI_ObjectSetColl(*this, name.c_str(), value));
}

template<>
inline void Object::Set<Reference>(mstring name, const Reference &value)
{
    Check(OCI_ObjectSetRef(*this, name.c_str(), value));
}

template<>
inline void Object::Set<Clob>(mstring name, const Clob &value)
{
    Check(OCI_ObjectSetLob(*this, name.c_str(), value));
}

template<>
inline void Object::Set<Blob>(mstring name, const Blob &value)
{
    Check(OCI_ObjectSetLob(*this, name.c_str(), value));
}

template<>
inline void Object::Set<File>(mstring name, const File &value)
{
    Check(OCI_ObjectSetFile(*this, name.c_str(), value));
}

template<>
inline void Object::Set<BufferPointer>(mstring name, const BufferPointer & value, unsigned int size)
{
    Check(OCI_ObjectSetRaw(*this, name.c_str(), value, size));
}

/* --------------------------------------------------------------------------------------------- *
 * Reference
 * --------------------------------------------------------------------------------------------- */

inline Reference::Reference(const TypeInfo &typeInfo)
{
    Connection connection = typeInfo.GetConnection();
    Acquire(Check(OCI_RefCreate(connection, typeInfo)), (HandleFreeFunc) OCI_RefFree, connection.GetHandle());
}

inline Reference::Reference(OCI_Ref *pRef, Handle *parent)
{
    Acquire(pRef, 0, parent);
}

inline TypeInfo Reference::GetTypeInfo()
{
    return TypeInfo(Check(OCI_RefGetTypeInfo(*this)));
}

inline Object Reference::GetObject()
{
    return Object(Check(OCI_RefGetObject(*this)), GetHandle());
}

inline void Reference::Assign(const Reference& other)
{
   Check(OCI_RefAssign(*this, other));
}

inline bool Reference::IsReferenceNull()
{
    return (Check(OCI_RefIsNull(*this)) == TRUE);
}

inline void Reference::SetReferenceNull()
{
    Check(OCI_RefSetNull(*this));
}

inline mstring Reference::ToString()
{
    unsigned int size =  Check(OCI_RefGetHexSize(*this));

    ManagedBuffer<mtext> buffer = new dtext [ size+1 ];

    Check(OCI_RefToText(*this, size, (mtext *) buffer));

    return MakeString( (const dtext *) buffer);

}

/* --------------------------------------------------------------------------------------------- *
 * Collection
 * --------------------------------------------------------------------------------------------- */

inline Collection::Collection(const TypeInfo &typeInfo)
{
    Acquire(Check(OCI_CollCreate(typeInfo)), (HandleFreeFunc) OCI_CollFree, typeInfo.GetConnection().GetHandle());
}

inline Collection::Collection(OCI_Coll *pColl, Handle *parent)
{
     Acquire(pColl, 0, parent);
}

inline void Collection::Assign(const Collection& other)
{
    Check(OCI_CollAssign(*this, other));
}

inline TypeInfo Collection::GetTypeInfo()
{
    return TypeInfo(Check(OCI_CollGetTypeInfo(*this)));
}

inline unsigned int Collection::GetType()
{
    return Check(OCI_CollGetType(*this));
}

inline unsigned int Collection::GetMax()
{
    return Check(OCI_CollGetMax(*this));
}

inline unsigned int Collection::GetSize()

{
    return Check(OCI_CollGetSize(*this));
}

inline void Collection::Truncate(unsigned int size)
{
    Check(OCI_CollTrim(*this, size));
}

inline void Collection::Clear()
{
    Check(OCI_CollClear(*this));
}

inline bool Collection::IsElementNull(unsigned int index)
{
   return (Check(OCI_ElemIsNull(Check(OCI_CollGetAt(*this, index)))) == TRUE);
}

inline void Collection::SetElementNull(unsigned int index)
{
    Check(OCI_ElemSetNull(Check(OCI_CollGetAt(*this, index))));
}

template <class TDataType>
inline TDataType Collection::Get(unsigned int index)
{
    return GetElem<TDataType>(Check(OCI_CollGetAt(*this, index)), GetHandle());
}

template <class TDataType>
inline void Collection::Get(unsigned int index, TDataType value, unsigned int &size)
{
    GetElem<TDataType>(Check(OCI_CollGetAt(*this, index)), value, size);
}

template <class TDataType>
inline void Collection::Set(unsigned int index, const TDataType &data)
{
    OCI_Elem * elem = Check(OCI_CollGetAt(*this, index));

    SetElem<TDataType>(elem, data);

    Check(OCI_CollSetAt(*this, index, elem));
}

template <class TDataType>
inline void Collection::Set(unsigned int index, const TDataType value, unsigned int size)
{
    OCI_Elem * elem = Check(OCI_CollGetAt(*this, index));

    SetElem<TDataType>(elem, value, size);

    Check(OCI_CollSetAt(*this, index, elem));
}

template <class TDataType>
inline void Collection::Append(const TDataType &data)
{
    OCI_Elem * elem = Check(OCI_ElemCreate(OCI_CollGetTypeInfo(*this)));

    SetElem<TDataType>(elem, data);

    Check(OCI_CollAppend(*this, elem));
    Check(OCI_ElemFree(elem));
}

template<>
inline short Collection::GetElem<short>(OCI_Elem *elem, Handle *parent)
{
    ARG_NOT_USED(parent);

    return Check(OCI_ElemGetShort(elem));
}

template<>
inline unsigned short Collection::GetElem<unsigned short>(OCI_Elem *elem, Handle *parent)
{
    ARG_NOT_USED(parent);

    return Check(OCI_ElemGetUnsignedShort(elem));
}

template<>
inline int Collection::GetElem<int>(OCI_Elem *elem, Handle *parent)
{
    ARG_NOT_USED(parent);

    return Check(OCI_ElemGetInt(elem));
}

template<>
inline unsigned int Collection::GetElem<unsigned int>(OCI_Elem *elem, Handle *parent)
{
    ARG_NOT_USED(parent);

    return Check(OCI_ElemGetUnsignedInt(elem));
}

template<>
inline big_int Collection::GetElem<big_int>(OCI_Elem *elem, Handle *parent)
{
    ARG_NOT_USED(parent);

    return Check(OCI_ElemGetBigInt(elem));
}

template<>
inline big_uint Collection::GetElem<big_uint>(OCI_Elem *elem, Handle *parent)
{
    ARG_NOT_USED(parent);

    return Check(OCI_ElemGetUnsignedBigInt(elem));
}

template<>
inline float Collection::GetElem<float>(OCI_Elem *elem, Handle *parent)
{
    ARG_NOT_USED(parent);

    return Check(OCI_ElemGetFloat(elem));
}

template<>
inline double Collection::GetElem<double>(OCI_Elem *elem, Handle *parent)
{
    ARG_NOT_USED(parent);

    return Check(OCI_ElemGetDouble(elem));
}

template<>
inline dstring Collection::GetElem<dstring>(OCI_Elem *elem, Handle *parent)
{
    ARG_NOT_USED(parent);

    return MakeString(Check(OCI_ElemGetString(elem)));
}

template<>
inline void Collection::GetElem<BufferPointer>(OCI_Elem *elem, BufferPointer value, unsigned int &size)
{
    Check(OCI_ElemGetRaw(elem, value, size));
}

template<>
inline Date Collection::GetElem<Date>(OCI_Elem *elem, Handle *parent)
{
    return Date(Check(OCI_ElemGetDate(elem)), parent);
}

template<>
inline Timestamp Collection::GetElem<Timestamp>(OCI_Elem *elem, Handle *parent)
{
    return Timestamp(Check(OCI_ElemGetTimestamp(elem)), parent);
}

template<>
inline Interval Collection::GetElem<Interval>(OCI_Elem *elem, Handle *parent)
{
    return Interval(Check(OCI_ElemGetInterval(elem)), parent);
}

template<>
inline Object Collection::GetElem<Object>(OCI_Elem *elem, Handle *parent)
{
    return Object(Check(OCI_ElemGetObject(elem)), parent);
}

template<>
inline Collection Collection::GetElem<Collection>(OCI_Elem *elem, Handle *parent)
{
    return Collection(Check(OCI_ElemGetColl(elem)), parent);
}

template<>
inline Reference Collection::GetElem<Reference>(OCI_Elem *elem, Handle *parent)
{
    return Reference(Check(OCI_ElemGetRef(elem)), parent);
}

template<>
inline Clob Collection::GetElem<Clob>(OCI_Elem *elem, Handle *parent)
{
    return Clob(Check(OCI_ElemGetLob(elem)), parent);
}

template<>
inline Blob Collection::GetElem<Blob>(OCI_Elem *elem, Handle *parent)
{
    return Blob(Check(OCI_ElemGetLob(elem)), parent);
}

template<>
inline File Collection::GetElem<File>(OCI_Elem *elem, Handle *parent)
{
    return File(Check(OCI_ElemGetFile(elem)), parent);
}

template<>
inline void Collection::SetElem<short>(OCI_Elem *elem, const short &value)
{
    Check(OCI_ElemSetShort(elem, value));
}

template<>
inline void Collection::SetElem<unsigned short>(OCI_Elem *elem, const unsigned short &value)
{
    Check(OCI_ElemSetUnsignedShort(elem, value));
}

template<>
inline void Collection::SetElem<int>(OCI_Elem *elem, const int &value)
{
    Check(OCI_ElemSetInt(elem, value));
}

template<>
inline void Collection::SetElem<unsigned int>(OCI_Elem *elem, const unsigned int &value)
{
    Check(OCI_ElemSetUnsignedInt(elem, value));
}

template<>
inline void Collection::SetElem<big_int>(OCI_Elem *elem, const big_int &value)
{
    Check(OCI_ElemSetBigInt(elem, value));
}

template<>
inline void Collection::SetElem<big_uint>(OCI_Elem *elem, const big_uint &value)
{
    Check(OCI_ElemSetUnsignedBigInt(elem, value));
}

template<>
inline void Collection::SetElem<float>(OCI_Elem *elem, const float &value)
{
    Check(OCI_ElemSetFloat(elem, value));
}

template<>
inline void Collection::SetElem<double>(OCI_Elem *elem, const double &value)
{
    Check(OCI_ElemSetDouble(elem, value));
}


template <>
inline void Collection::SetElem<dstring>(OCI_Elem *elem, const dstring &value)
{
    Check(OCI_ElemSetString(elem, value.c_str()));
}

template<>
inline void Collection::SetElem<BufferPointer>(OCI_Elem *elem, const BufferPointer value, unsigned int size)
{
    Check(OCI_ElemSetRaw(elem, value, size));
}

template<>
inline void Collection::SetElem<Date>(OCI_Elem *elem, const Date &value)
{
    Check(OCI_ElemSetDate(elem, value));
}

template<>
inline void Collection::SetElem<Timestamp>(OCI_Elem *elem, const Timestamp &value)
{
    Check(OCI_ElemSetTimestamp(elem, value));
}

template<>
inline void Collection::SetElem<Interval>(OCI_Elem *elem, const Interval &value)
{
    Check(OCI_ElemSetInterval(elem, value));
}

template<>
inline void Collection::SetElem<Object>(OCI_Elem *elem, const Object &value)
{
    Check(OCI_ElemSetObject(elem, value));
}

template<>
inline void Collection::SetElem<Collection>(OCI_Elem *elem, const Collection &value)
{
    Check(OCI_ElemSetColl(elem, value));
}

template<>
inline void Collection::SetElem<Reference>(OCI_Elem *elem, const Reference &value)
{
    Check(OCI_ElemSetRef(elem, value));
}

template<>
inline void Collection::SetElem<Clob>(OCI_Elem *elem, const Clob &value)
{
    Check(OCI_ElemSetLob(elem, value));
}

template<>
inline void Collection::SetElem<Blob>(OCI_Elem *elem, const Blob &value)
{
    Check(OCI_ElemSetLob(elem, value));
}

template<>
inline void Collection::SetElem<File>(OCI_Elem *elem, const File &value)
{
    Check(OCI_ElemSetFile(elem, value));
}

/* --------------------------------------------------------------------------------------------- *
 * CollectionIterator
 * --------------------------------------------------------------------------------------------- */

inline CollectionIterator::CollectionIterator(const Collection &collection)
{
    Acquire(Check(OCI_IterCreate(collection)), (HandleFreeFunc) OCI_IterFree, collection.GetHandle());
}

inline bool CollectionIterator::Next()
{
   return (Check(OCI_IterGetNext(*this)) != 0);
}

inline bool CollectionIterator::Prev()
{
   return (Check(OCI_IterGetPrev(*this)) != 0);
}

template <class TDataType>
inline void CollectionIterator::Set(TDataType &value)
{
    Collection::SetElem<TDataType>(Check(OCI_IterGetCurrent(*this)), value);
}

template <class TDataType>
inline TDataType CollectionIterator::Get()
{
    return Collection::GetElem<TDataType>(Check(OCI_IterGetCurrent(*this)), GetHandle());
}

inline bool CollectionIterator::IsElementNull()
{
   return (Check(OCI_ElemIsNull(Check(OCI_IterGetCurrent(*this)))) == TRUE);
}

inline void CollectionIterator::SetElementNull()
{
    Check(OCI_ElemSetNull(Check(OCI_IterGetCurrent(*this))));
}

/* --------------------------------------------------------------------------------------------- *
 * CLong
 * --------------------------------------------------------------------------------------------- */

inline CLong::CLong(const Statement &statement)
{
    Acquire(Check(OCI_LongCreate(statement, OCI_CLONG)), (HandleFreeFunc) OCI_LongFree, statement.GetHandle());
}

inline CLong::CLong(OCI_Long *pLong, Handle* parent)
{
    Acquire(pLong, 0, parent);
}

inline dstring CLong::Read(unsigned int size)
{
    ManagedBuffer<dtext> buffer = new dtext[size+1];

    size = Check(OCI_LongRead(*this, (void *) buffer, size));

    return MakeString( (const dtext *) buffer);
}

inline unsigned int CLong::Write(dstring content)
{
    return Check(OCI_LongWrite(*this, (void *) content.c_str(), content.size()));
}

inline unsigned int CLong::GetSize()
{
    return Check(OCI_LongGetSize(*this));
}

inline dstring CLong::GetContent()
{
    return MakeString((dtext *) Check(OCI_LongGetBuffer(*this)));
}

/* --------------------------------------------------------------------------------------------- *
 * BLong
 * --------------------------------------------------------------------------------------------- */

inline BLong::BLong(const Statement &statement)
{
    Acquire(Check(OCI_LongCreate(statement, OCI_BLONG)), (HandleFreeFunc) OCI_LongFree, statement.GetHandle());
}

inline BLong::BLong(OCI_Long *pLong, Handle *parent)
{
    Acquire(pLong, 0, parent);
}

inline unsigned int BLong::Read(void *buffer, unsigned int size)
{
    return Check(OCI_LongRead(*this ,buffer, size));
}

inline unsigned int BLong::Write(void *buffer, unsigned int size)
{
    return Check(OCI_LongWrite(*this ,buffer, size));
}

inline unsigned int BLong::GetSize()
{
    return Check(OCI_LongGetSize(*this));
}

inline void * BLong::GetContent()
{
    return  Check(OCI_LongGetBuffer(*this));
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
    return (std::vector<TObjectType> &) (*(dynamic_cast< BindArrayObject<TObjectType, TDataType> * > (_object)));
}

template <class TObjectType, class TDataType>
inline TDataType *  BindArray::GetData ()
{
    return (TDataType *) (*(dynamic_cast< BindArrayObject<TObjectType, TDataType> * > (_object)));
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
            TObjectType& object = *it;

            object = (TDataType) _data[index];
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

inline void BindString::SetInData()
{
    unsigned int size = (unsigned int) _string.size();

    if (size > _elemSize)
    {
        size = _elemSize;
    }

    memcpy(_data, _string.c_str(), size * sizeof(dtext));
}

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
    std::vector<BindObject *>::iterator it, it_end;

    for(it = _bindObjects.begin(), it_end = _bindObjects.end(); it < it_end; it++)
    {
        delete (*it);
    }
}

inline void BindsHolder::AddBindObject(BindObject *bindObject)
{
    _bindObjects.push_back(bindObject);
}

inline void BindsHolder::SetOutData()
{
    std::vector<BindObject *>::iterator it, it_end;

    for(it = _bindObjects.begin(), it_end = _bindObjects.end(); it < it_end; it++)
    {
        (*it)->SetOutData();
    }
}

inline void BindsHolder::SetInData()
{
    std::vector<BindObject *>::iterator it, it_end;

    for(it = _bindObjects.begin(), it_end = _bindObjects.end(); it < it_end; it++)
    {
        (*it)->SetInData();
    }
}

/* --------------------------------------------------------------------------------------------- *
 * Bind
 * --------------------------------------------------------------------------------------------- */

inline BindInfo::BindInfo(OCI_Bind *pBind, Handle *parent)
{
    Acquire(pBind, 0, parent);
}

inline mstring BindInfo::GetName()
{
    return MakeString(Check(OCI_BindGetName(*this)));
}

inline unsigned int BindInfo::GetType()
{
    return Check(OCI_BindGetType(*this));
}

inline unsigned int BindInfo::GetSubType()
{
    return Check(OCI_BindGetSubtype(*this));
}

inline unsigned int BindInfo::GetElemcount()
{
    return Check(OCI_BindGetDirection(*this));
}

inline Statement BindInfo::GetStatement()
{
    return Statement(Check(OCI_BindGetStatement(*this)));
}

inline void BindInfo::SetNull()
{
    Check(OCI_BindSetNull(*this));
}

inline void BindInfo::SetNull(unsigned int pos)
{
    Check(OCI_BindSetNullAtPos(*this, pos));
}

inline bool BindInfo::IsNull()
{
    return (Check(OCI_BindIsNull(*this)) == TRUE);
}

inline bool BindInfo::IsNull(unsigned int pos)
{
    return (Check(OCI_BindIsNullAtPos(*this, pos)) == TRUE);
}

inline void BindInfo::SetCharsetForm(unsigned int value)
{
    Check(OCI_BindSetCharsetForm(*this, value));
}

inline void BindInfo::SetDirection(unsigned int value)
{
    Check(OCI_BindSetDirection(*this, value));
}

inline unsigned int BindInfo::GetDirection()
{
    return Check(OCI_BindGetDirection(*this));
}

/* --------------------------------------------------------------------------------------------- *
 * Statement
 * --------------------------------------------------------------------------------------------- */

inline Statement::Statement(const Connection &connection)
{
    Acquire(Check(OCI_StatementCreate(connection)), (HandleFreeFunc) OCI_StatementFree, connection.GetHandle());
}

inline Statement::Statement(OCI_Statement *stmt, Handle *parent)
{
     Acquire(stmt, parent ? (HandleFreeFunc) OCI_StatementFree : 0, parent);
}

inline Statement::~Statement()
{
    if (_smartHandle && _smartHandle->IsLastHolder(this))
    {
        BindsHolder *bindsHolder = GetBindsHolder(false);

        delete bindsHolder;
    }
}

inline Connection Statement::GetConnection()
{
    return Connection(Check(OCI_StatementGetConnection(*this)), 0);
}

inline void Statement::Describe(mstring sql)
{
    ClearBinds();
    ReleaseResultsets();
    Check(OCI_Describe(*this, sql.c_str()));
}

inline void Statement::Parse(mstring sql)
{
    ClearBinds();
    ReleaseResultsets();
    Check(OCI_Parse(*this, sql.c_str()));
}

inline void Statement::Prepare(mstring sql)
{
    ClearBinds();
    ReleaseResultsets();
    Check(OCI_Prepare(*this, sql.c_str()));
}

inline void Statement::Execute()
{
    ReleaseResultsets();
    SetInData();
    Check(OCI_Execute(*this));
    SetOutData();
}

inline void Statement::Execute(mstring sql)
{
    ClearBinds();
    ReleaseResultsets();
    Check(OCI_ExecuteStmt(*this, sql.c_str()));
}

inline unsigned int Statement::GetAffectedRows()
{
    return Check(OCI_GetAffectedRows(*this));
}

inline mstring Statement::GetSql()
{
    return MakeString(Check(OCI_GetSql(*this)));
}

inline Resultset Statement::GetResultset()
{
   return Resultset(Check(OCI_GetResultset(*this)), GetHandle());
}

inline Resultset Statement::GetNextResultset()
{
   return Resultset(Check(OCI_GetNextResultset(*this)), GetHandle());
}

inline void Statement::SetBindArraySize(unsigned int size)
{
    Check(OCI_BindArraySetSize(*this, size));
}

inline unsigned int Statement::GetBindArraySize()
{
    return Check(OCI_BindArrayGetSize(*this));
}

inline void Statement::AllowRebinding(bool value)
{
    Check(OCI_AllowRebinding(*this, value));
}

inline unsigned int Statement::GetBindCount()
{
    return Check(OCI_GetBindCount(*this));
}

inline BindInfo Statement::GetBind(unsigned int index)
{
    return BindInfo(Check(OCI_GetBind(*this, index)), GetHandle());
}

inline BindInfo Statement::GetBind(mstring name)
{
    return BindInfo(Check(OCI_GetBind2(*this, name.c_str())), GetHandle());
}

template <typename TBindMethod, class TDataType>
inline void Statement::Bind (TBindMethod &method, mstring name, TDataType& value, unsigned int mode)
{
    Check(method(*this, name.c_str(), &value));
    SetLastBindMode(mode);
}

template <typename TBindMethod, class TObjectType, class TDataType>
inline void Statement::Bind (TBindMethod &method, mstring name, TObjectType& value, BindValue<TDataType> datatype, unsigned int mode)
{
    Check(method(*this, name.c_str(), (TDataType) value));
    SetLastBindMode(mode);
}

template <typename TBindMethod, class TObjectType, class TDataType>
inline void Statement::Bind (TBindMethod &method, mstring name, std::vector<TObjectType> &values, BindValue<TDataType> datatype, unsigned int mode)
{
    BindArray * bnd = new BindArray(values, datatype, mode, OCI_BindArrayGetSize(*this), sizeof(TDataType));

    BindsHolder *bindsHolder = GetBindsHolder(true);
    bindsHolder->AddBindObject(bnd);

    Check(method(*this, name.c_str(), (TDataType *) bnd->GetData<TObjectType, TDataType>(), 0));
    SetLastBindMode(mode);
}

template <typename TBindMethod, class TObjectType, class TDataType, class TElemType>
inline void Statement::Bind (TBindMethod &method, mstring name, std::vector<TObjectType> &values, BindValue<TDataType> datatype, unsigned int mode, TElemType type)
{
    BindArray * bnd = new BindArray(values, datatype, mode, OCI_BindArrayGetSize(*this), sizeof(TDataType));

    BindsHolder *bindsHolder = GetBindsHolder(true);
    bindsHolder->AddBindObject(bnd);

    method(*this, name.c_str(), (TDataType *) bnd->GetData<TObjectType, TDataType>(), type, 0);
    SetLastBindMode(mode);
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
    Check(OCI_BindLong(*this, name.c_str(), value, maxSize));
    SetLastBindMode(mode);
}

template <>
inline void Statement::Bind<CLong, int>(mstring name, CLong &value, int maxSize, unsigned int mode)
{
    Bind<CLong, unsigned int>(name, value, (unsigned int) maxSize,  mode);
}


template <>
inline void Statement::Bind<BLong, unsigned int>(mstring name, BLong &value, unsigned int maxSize, unsigned int mode)
{
    Check(OCI_BindLong(*this, name.c_str(), value, maxSize));
    SetLastBindMode(mode);
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
    bindsHolder->AddBindObject(bnd);

    Check(OCI_BindString(*this, name.c_str(), (dtext*) (*bnd), maxSize));
    SetLastBindMode(mode);
}

template <>
inline void Statement::Bind<dstring, int>(mstring name, dstring &value, int maxSize, unsigned int mode)
{
    Bind<dstring, unsigned int>(name, value, (unsigned int) maxSize,  mode);
}

template <>
inline void Statement::Bind<BufferPointer, unsigned int>(mstring name, BufferPointer &value, unsigned int maxSize, unsigned int mode)
{
    Check(OCI_BindRaw(*this, name.c_str(), value, maxSize));
    SetLastBindMode(mode);
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
inline void Statement::Bind<Interval, unsigned int>(mstring name, std::vector<Interval> &values, unsigned int type, unsigned int mode)
{
    Bind(OCI_BindArrayOfIntervals, name, values, BindValue<OCI_Interval *>(), mode, type);
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
    bindsHolder->AddBindObject(bnd);

    Check(OCI_BindArrayOfStrings(*this, name.c_str(), bnd->GetData<dstring, dtext>(), maxSize, 0));
    SetLastBindMode(mode);
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
    bindsHolder->AddBindObject(bnd);

    Check(OCI_BindArrayOfRaws(*this, name.c_str(), bnd->GetData<void *, void *>(), maxSize, 0));
    SetLastBindMode(mode);
}

template <>
inline void Statement::Register<unsigned short>(mstring name)
{
    Check(OCI_RegisterUnsignedShort(*this, name.c_str()));
}

template <>
inline void Statement::Register<short>(mstring name)
{
    Check(OCI_RegisterShort(*this, name.c_str()));
}

template <>
inline void Statement::Register<unsigned int>(mstring name)
{
    Check(OCI_RegisterUnsignedInt(*this, name.c_str()));
}

template <>
inline void Statement::Register<int>(mstring name)
{
    Check(OCI_RegisterInt(*this, name.c_str()));
}

template <>
inline void Statement::Register<big_uint>(mstring name)
{
    Check(OCI_RegisterUnsignedBigInt(*this, name.c_str()));
}

template <>
inline void Statement::Register<big_int>(mstring name)
{
    Check(OCI_RegisterBigInt(*this, name.c_str()));
}

template <>
inline void Statement::Register<float>(mstring name)
{
    Check(OCI_RegisterFloat(*this, name.c_str()));
}

template <>
inline void Statement::Register<double>(mstring name)
{
    Check(OCI_RegisterDouble(*this, name.c_str()));
}

template <>
inline void Statement::Register<Date>(mstring name)
{
    Check(OCI_RegisterDate(*this, name.c_str()));
}

template <>
inline void Statement::Register<Timestamp, unsigned int>(mstring name, unsigned int elemType)
{
    Check(OCI_RegisterTimestamp(*this, name.c_str(), elemType));
}

template <>
inline void Statement::Register<Interval, unsigned int>(mstring name, unsigned int elemType)
{
    Check(OCI_RegisterInterval(*this, name.c_str(), elemType));
}

template <>
inline void Statement::Register<Clob>(mstring name)
{
    Check(OCI_RegisterLob(*this, name.c_str(), OCI_CLOB));
}

template <>
inline void Statement::Register<Blob>(mstring name)
{
    Check(OCI_RegisterLob(*this, name.c_str(), OCI_BLOB));
}

template <>
inline void Statement::Register<File>(mstring name)
{
    Check(OCI_RegisterFile(*this, name.c_str(), OCI_BFILE));
}

template <>
inline void Statement::Register<Object, TypeInfo>(mstring name, TypeInfo& typeInfo)
{
    Check(OCI_RegisterObject(*this, name.c_str(), typeInfo));
}

template <>
inline void Statement::Register<Reference, TypeInfo>(mstring name, TypeInfo& typeInfo)
{
    Check(OCI_RegisterRef(*this, name.c_str(), typeInfo));
}
 
template <>
inline void Statement::Register<dstring, unsigned int>(mstring name, unsigned int len)
{
   Check(OCI_RegisterString(*this, name.c_str(), len));
}

template <>
inline void Statement::Register<dstring, int>(mstring name, int len)
{
    Register<dstring, unsigned int>(name,  (unsigned int) len);
}

template <>
inline void Statement::Register<BufferPointer, unsigned int>(mstring name, unsigned int len)
{
    Check(OCI_RegisterRaw(*this, name.c_str(), len));
}

template <>
inline void Statement::Register<BufferPointer, int>(mstring name, int len)
{
  Register<dstring, unsigned int>(name,(unsigned int) len);
}


inline unsigned int Statement::GetStatementType()
{
    return Check(OCI_GetStatementType(*this));
}

inline unsigned int Statement::GetSqlErrorPos()
{
    return Check(OCI_GetSqlErrorPos(*this));
}

inline void Statement::SetFetchMode(unsigned int value)
{
    Check(OCI_SetFetchMode(*this, value));
}

inline unsigned int Statement::GetFetchMode()
{
    return Check(OCI_GetFetchMode(*this));
}

inline void Statement::SetBindMode(unsigned int value)
{
    Check(OCI_SetBindMode(*this, value));
}

inline unsigned int Statement::GetBindMode()
{
    return Check(OCI_GetBindMode(*this));
}

inline void Statement::SetFetchSize(unsigned int value)
{
    Check(OCI_SetFetchSize(*this, value));
}

inline unsigned int Statement::GetFetchSize()
{
   return Check(OCI_GetFetchSize(*this));
}

inline void Statement::SetPrefetchSize(unsigned int value)
{
    Check(OCI_SetPrefetchSize(*this, value));
}

inline unsigned int Statement::GetPrefetchSize()
{
    return Check(OCI_GetPrefetchSize(*this));
}

inline void Statement::SetPrefetchMemory(unsigned int value)
{
    Check(OCI_SetPrefetchMemory(*this, value));
}

inline unsigned int Statement::GetPrefetchMemory()
{
    return Check(OCI_GetPrefetchMemory(*this));
}

inline void Statement::SetLongMaxSize(unsigned int value)
{
    Check(OCI_SetLongMaxSize(*this, value));
}

inline unsigned int Statement::GetLongMaxSize()
{
    return Check(OCI_GetLongMaxSize(*this));
}

inline void Statement::SetLongMode(unsigned int value)
{
    Check(OCI_SetLongMode(*this, value));
}

inline unsigned int Statement::GetLongMode()
{
    return Check(OCI_GetLongMode(*this));
}

inline unsigned int Statement::GetSQLCommand()
{
    return Check(OCI_GetSQLCommand(*this));
}

inline mstring Statement::GetSQLVerb()
{
    return MakeString(Check(OCI_GetSQLVerb(*this)));
}

inline void Statement::GetBatchErrors(std::vector<Exception> &exceptions)
{
    exceptions.clear();

    OCI_Error *err =  Check(OCI_GetBatchError(*this));

    while (err)
    {
        exceptions.push_back(Exception(err));

        err =  Check(OCI_GetBatchError(*this));
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

inline void Statement::ReleaseResultsets()
{
    if (_smartHandle)
    {
        std::list<Handle *> &children = _smartHandle->GetChildren();

        size_t nbHandles = children.size();

        while (nbHandles-- > 0)
        {
            Resultset::SmartHandle<OCI_Resultset *> *smartHandle = dynamic_cast<Resultset::SmartHandle<OCI_Resultset *> *>(*children.begin());

            if (smartHandle)
            {
                smartHandle->DetachFromHolders();

                delete smartHandle;
            }
        }
    }
}

inline void Statement::SetLastBindMode(unsigned int mode)
{
    Check(OCI_BindSetDirection(Check(OCI_GetBind(*this, Check(OCI_GetBindCount(*this)))), mode));
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

inline Resultset::Resultset(OCI_Resultset *resultset, Handle *parent)
{
    Acquire(resultset, 0, parent);
}

inline bool Resultset::Next()
{
    return (Check(OCI_FetchNext(*this)) == TRUE);
}

inline bool Resultset::Prev()
{
    return (Check(OCI_FetchPrev(*this)) == TRUE);
}

inline bool Resultset::First()
{
    return (Check(OCI_FetchFirst(*this)) == TRUE);
}

inline bool Resultset::Last()
{
    return (Check(OCI_FetchLast(*this)) == TRUE);
}

inline bool Resultset::Seek(unsigned int mode, int offset)
{
    return (Check(OCI_FetchSeek(*this, mode, offset)) == TRUE);
}

inline unsigned int Resultset::GetCount()
{
    return Check(OCI_GetRowCount(*this));
}

inline unsigned int Resultset::GetCurrentRow()
{
    return Check(OCI_GetCurrentRow(*this));
}

inline unsigned int Resultset::GetColumnCount()
{
    return Check(OCI_GetColumnCount(*this));
}

inline Column Resultset::GetColumn(unsigned int index)
{
    return Column(Check(OCI_GetColumn(*this, index)), GetHandle());
}

inline Column Resultset::GetColumn(mstring name)
{
    return Column(Check(OCI_GetColumn2(*this, name.c_str())), GetHandle());
}

inline bool Resultset::IsColumnNull(unsigned int index)
{
    return (Check(OCI_IsNull(*this, index)) == TRUE);
}

inline bool Resultset::IsColumnNull(mstring name)
{
    return (Check(OCI_IsNull2(*this, name.c_str())) == TRUE);
}

inline Statement Resultset::GetStatement()
{
    return Statement( Check(OCI_ResultsetGetStatement(*this)), 0);
}

template<>
inline short Resultset::Get<short>(unsigned int index)
{
    return Check(OCI_GetShort(*this, index));
}

template<>
inline short Resultset::Get<short>(mstring name)
{
    return Check(OCI_GetShort2(*this, name.c_str()));
}

template<>
inline unsigned short Resultset::Get<unsigned short>(unsigned int index)
{
    return Check(OCI_GetUnsignedShort(*this, index));
}

template<>
inline unsigned short Resultset::Get<unsigned short>(mstring name)
{
    return Check(OCI_GetUnsignedShort2(*this, name.c_str()));
}

template<>
inline int Resultset::Get<int>(unsigned int index)
{
    return Check(OCI_GetInt(*this, index));
}

template<>
inline int Resultset::Get<int>(mstring name)
{
    return Check(OCI_GetInt2(*this, name.c_str()));
}

template<>
inline unsigned int Resultset::Get<unsigned int>(unsigned int index)
{
    return Check(OCI_GetUnsignedInt(*this, index));
}

template<>
inline unsigned int Resultset::Get<unsigned int>(mstring name)
{
    return Check(OCI_GetUnsignedInt2(*this, name.c_str()));
}

template<>
inline big_int Resultset::Get<big_int>(unsigned int index)
{
    return Check(OCI_GetBigInt(*this, index));
}

template<>
inline big_int Resultset::Get<big_int>(mstring name)
{
    return Check(OCI_GetBigInt2(*this, name.c_str()));
}

template<>
inline big_uint Resultset::Get<big_uint>(unsigned int index)
{
    return Check(OCI_GetUnsignedBigInt(*this, index));
}

template<>
inline big_uint Resultset::Get<big_uint>(mstring name)
{
    return Check(OCI_GetUnsignedBigInt2(*this, name.c_str()));
}

template<>
inline float Resultset::Get<float>(unsigned int index)
{
    return Check(OCI_GetFloat(*this, index));
}

template<>
inline float Resultset::Get<float>(mstring name)
{
    return Check(OCI_GetFloat2(*this, name.c_str()));
}

template<>
inline double Resultset::Get<double>(unsigned int index)
{
    return Check(OCI_GetDouble(*this, index));
}

template<>
inline double Resultset::Get<double>(mstring name)
{
    return Check(OCI_GetDouble2(*this, name.c_str()));
}

template<>
inline dstring Resultset::Get<dstring>(unsigned int index)
{
    return MakeString(Check(OCI_GetString(*this, index)));
}

template<>
inline dstring Resultset::Get<dstring>(mstring name)
{
    return MakeString(Check(OCI_GetString2(*this,name.c_str())));
}

template<>
inline void Resultset::Get<BufferPointer>(unsigned int index, BufferPointer value, unsigned int &size)
{
    Check(OCI_GetRaw(*this, index, value, size));
}

template<>
inline void Resultset::Get<BufferPointer>(mstring name, BufferPointer value, unsigned int &size)
{
    Check(OCI_GetRaw2(*this,name.c_str(), value, size));
}

template<>
inline Date Resultset::Get<Date>(unsigned int index)
{
    return Date(Check(OCI_GetDate(*this, index)), GetHandle());
}

template<>
inline Date Resultset::Get<Date>(mstring name)
{
    return Date(Check(OCI_GetDate2(*this,name.c_str())), GetHandle());
}

template<>
inline Timestamp Resultset::Get<Timestamp>(unsigned int index)
{
    return Timestamp(Check(OCI_GetTimestamp(*this, index)), GetHandle());
}

template<>
inline Timestamp Resultset::Get<Timestamp>(mstring name)
{
    return Timestamp(Check(OCI_GetTimestamp2(*this,name.c_str())), GetHandle());
}

template<>
inline Interval Resultset::Get<Interval>(unsigned int index)
{
    return Interval(Check(OCI_GetInterval(*this, index)), GetHandle());
}

template<>
inline Interval Resultset::Get<Interval>(mstring name)
{
    return Interval(Check(OCI_GetInterval2(*this,name.c_str())), GetHandle());
}

template<>
inline Object Resultset::Get<Object>(unsigned int index)
{
    return Object(Check(OCI_GetObject(*this, index)), GetHandle());
}

template<>
inline Object Resultset::Get<Object>(mstring name)
{
    return Object(Check(OCI_GetObject2(*this,name.c_str())), GetHandle());
}

template<>
inline Collection Resultset::Get<Collection>(unsigned int index)
{
    return Collection(Check(OCI_GetColl(*this, index)), GetHandle());
}

template<>
inline Collection Resultset::Get<Collection>(mstring name)
{
    return Collection(Check(OCI_GetColl2(*this,name.c_str())), GetHandle());
}

template<>
inline Reference Resultset::Get<Reference>(unsigned int index)
{
    return Reference(Check(OCI_GetRef(*this, index)), GetHandle());
}

template<>
inline Reference Resultset::Get<Reference>(mstring name)
{
    return Reference(Check(OCI_GetRef2(*this,name.c_str())), GetHandle());
}

template<>
inline Statement Resultset::Get<Statement>(unsigned int index)
{
    return Statement(Check(OCI_GetStatement(*this, index)), GetHandle());
}

template<>
inline Statement Resultset::Get<Statement>(mstring name)
{
    return Statement(Check(OCI_GetStatement2(*this,name.c_str())), GetHandle());
}

template<>
inline Clob Resultset::Get<Clob>(unsigned int index)
{
    return Clob(Check(OCI_GetLob(*this, index)), GetHandle());
}

template<>
inline Clob Resultset::Get<Clob>(mstring name)
{
    return Clob(Check(OCI_GetLob2(*this,name.c_str())), GetHandle());
}

template<>
inline Blob Resultset::Get<Blob>(unsigned int index)
{
    return Blob(Check(OCI_GetLob(*this, index)), GetHandle());
}

template<>
inline Blob Resultset::Get<Blob>(mstring name)
{
    return Blob(Check(OCI_GetLob2(*this,name.c_str())), GetHandle());
}

template<>
inline File Resultset::Get<File>(unsigned int index)
{
    return File(Check(OCI_GetFile(*this, index)), GetHandle());
}

template<>
inline File Resultset::Get<File>(mstring name)
{
    return File(Check(OCI_GetFile2(*this,name.c_str())), GetHandle());
}

template<>
inline CLong Resultset::Get<CLong>(unsigned int index)
{
    return CLong(Check(OCI_GetLong(*this, index)), GetHandle());
}

template<>
inline CLong Resultset::Get<CLong>(mstring name)
{
    return CLong(Check(OCI_GetLong2(*this,name.c_str())), GetHandle());
}

template<>
inline BLong Resultset::Get<BLong>(unsigned int index)
{
    return BLong(Check(OCI_GetLong(*this, index)), GetHandle());
}

template<>
inline BLong Resultset::Get<BLong>(mstring name)
{
    return BLong(Check(OCI_GetLong2(*this,name.c_str())), GetHandle());
}

/* --------------------------------------------------------------------------------------------- *
 * Column
 * --------------------------------------------------------------------------------------------- */

inline Column::Column(OCI_Column *pColumn, Handle *parent)
{
    Acquire(pColumn, 0, parent);
}

inline mstring Column::GetName()
{
    return MakeString(Check(OCI_ColumnGetName(*this)));
}

inline mstring Column::GetSQLType()
{
    return MakeString(Check(OCI_ColumnGetSQLType(*this)));
}

inline mstring Column::GetFullSQLType()
{
    unsigned int size = OCI_SIZE_BUFFER;

    ManagedBuffer<mtext> buffer = new mtext[size+1];

    Check(OCI_ColumnGetFullSQLType(*this, buffer, size));

    return MakeString((mtext *) buffer);
}

inline unsigned int Column::GetType()
{
    return Check(OCI_ColumnGetType(*this));
}

inline unsigned int Column::GetSubType()
{
    return Check(OCI_ColumnGetSubType(*this));
}

inline unsigned int Column::GetCharsetForm()
{
    return Check(OCI_ColumnGetCharsetForm(*this));
}

inline unsigned int Column::GetSize()
{
    return Check(OCI_ColumnGetSize(*this));
}

inline int Column::GetScale()
{
    return Check(OCI_ColumnGetScale(*this));
}

inline int Column::GetPrecision()
{
    return Check(OCI_ColumnGetPrecision(*this));
}

inline int Column::GetFractionalPrecision()
{
    return Check(OCI_ColumnGetFractionalPrecision(*this));
}

inline int Column::GetLeadingPrecision()
{
    return Check(OCI_ColumnGetLeadingPrecision(*this));
}

inline bool Column::GetNullable()
{
    return (Check(OCI_ColumnGetNullable(*this)) == TRUE);
}

inline bool Column::GetCharUsed()
{
    return (Check(OCI_ColumnGetCharUsed(*this)) == TRUE);
}

inline TypeInfo Column::GetTypeInfo()
{
    return TypeInfo(Check(OCI_ColumnGetTypeInfo(*this)));
}

/* --------------------------------------------------------------------------------------------- *
 * Subscription
 * --------------------------------------------------------------------------------------------- */

inline Subscription::Subscription()
{

}

inline Subscription::Subscription(OCI_Subscription *pSubcription)
{
    Acquire(pSubcription, 0, 0);
}

inline void Subscription::Register(const Connection &con, mstring name, unsigned int type, NotifyHandlerProc handler, unsigned int port, unsigned int timeout)
{
    Acquire(Check(OCI_SubscriptionRegister(con, name.c_str(), type, (POCI_NOTIFY) (handler != 0 ? Environment::NotifyHandler : 0 ), port, timeout)), (HandleFreeFunc) OCI_SubscriptionUnregister, 0);

    Environment::GetEnvironmentHandle().Callbacks.Set((OCI_Subscription*) *this, (CallbackPointer) handler);
}

inline void Subscription::Unregister()
{
    Environment::GetEnvironmentHandle().Callbacks.Remove(*this);
    Release();
}

inline void Subscription::Watch(mstring sql)
{
    Statement st(GetConnection());

    st.Execute(sql);

    Check(OCI_SubscriptionAddStatement(*this, st));
}

inline mstring Subscription::GetName()
{
    return MakeString(Check(OCI_SubscriptionGetName(*this)));
}

inline unsigned int Subscription::GetTimeout()
{
    return Check(OCI_SubscriptionGetTimeout(*this));
}

inline unsigned int Subscription::GetPort()
{
    return Check(OCI_SubscriptionGetPort(*this));
}

inline Connection Subscription::GetConnection()
{
    return Connection(Check(OCI_SubscriptionGetConnection(*this)), 0);
}

/* --------------------------------------------------------------------------------------------- *
 * Event
 * --------------------------------------------------------------------------------------------- */

inline Event::Event(OCI_Event *pEvent)
{
    Acquire(pEvent, 0, 0);
}

inline unsigned int Event::GetType()
{
    return Check(OCI_EventGetType(*this));
}

inline unsigned int Event::GetOperation()
{
    return Check(OCI_EventGetOperation(*this));
}

inline mstring Event::GetDatabaseName()
{
    return MakeString(Check(OCI_EventGetDatabase(*this)));
}

inline mstring Event::GetObjectName()
{
    return MakeString(Check(OCI_EventGetObject(*this)));
}

inline mstring Event::GetRowID()
{
    return MakeString(Check(OCI_EventGetRowid(*this)));
}

inline Subscription Event::GetSubscription()
{
    return Subscription(Check(OCI_EventGetSubscription(*this)));
}

/* --------------------------------------------------------------------------------------------- *
 * Agent
 * --------------------------------------------------------------------------------------------- */

inline Agent::Agent(const Connection &connection, mstring name, mstring address)
{
    Acquire(Check(OCI_AgentCreate(connection, name.c_str(), address.c_str())), (HandleFreeFunc) OCI_AgentFree, 0);
}

inline Agent::Agent(OCI_Agent *pAgent, Handle *parent)
{
    Acquire(pAgent, 0, parent);
}

inline mstring Agent::GetName()
{
    return MakeString(Check(OCI_AgentGetName(*this)));
}

inline void Agent::SetName(mstring value)
{
    Check(OCI_AgentSetName(*this, value.c_str()));
}

inline mstring Agent::GetAddress()
{
    return MakeString(Check(OCI_AgentGetAddress(*this)));
}

inline void Agent::SetAddress(mstring value)
{
    Check(OCI_AgentSetAddress(*this, value.c_str()));
}

/* --------------------------------------------------------------------------------------------- *
 * Message
 * --------------------------------------------------------------------------------------------- */

inline Message::Message(const TypeInfo &typeInfo)
{
    Acquire(Check(OCI_MsgCreate(typeInfo)), (HandleFreeFunc) OCI_MsgFree, 0);
}

inline Message::Message(OCI_Msg *pMessage, Handle *parent)
{
    Acquire(pMessage, 0, parent);
}

inline void Message::Reset()
{
    Check(OCI_MsgReset(*this));
}

inline Object Message::Get()
{
    return Object(Check(OCI_MsgGetObject(*this)), 0);
}

inline void Message::Set(const Object &value)
{
    Check(OCI_MsgSetObject(*this, value));
}

inline void Message::Get(BufferPointer value, unsigned int &size)
{
    Check(OCI_MsgGetRaw(*this, value, &size));
}

inline void Message::Set(const BufferPointer &value, unsigned int size)
{
    Check(OCI_MsgSetRaw(*this, value, size));
}

inline Date Message::GetEnqueueTime()
{
    return Date(Check(OCI_MsgGetEnqueueTime(*this)), 0);
}

inline int Message::GetAttemptCount()
{
    return Check(OCI_MsgGetAttemptCount(*this));
}

inline unsigned int Message::GetState()
{
    return Check(OCI_MsgGetState(*this));
}

inline void Message::GetID(BufferPointer value, unsigned int &size)
{
    Check(OCI_MsgGetID(*this, value, &size));
}

inline int Message::GetExpiration()
{
    return Check(OCI_MsgGetExpiration(*this));
}

inline void Message::SetExpiration(int value)
{
    Check(OCI_MsgSetExpiration(*this, value));
}

inline int Message::GetEnqueueDelay()
{
    return Check(OCI_MsgGetEnqueueDelay(*this));
}

inline void Message::SetEnqueueDelay(int value)
{
    Check(OCI_MsgSetEnqueueDelay(*this, value));
}

inline int Message::GetPriority()
{
    return Check(OCI_MsgGetPriority(*this));
}

inline void Message::SetPriority(int value)
{
    Check(OCI_MsgSetPriority(*this, value));
}

inline void Message::GetOriginalID(BufferPointer value, unsigned int &size)
{
    Check(OCI_MsgGetOriginalID(*this, value, &size));
}

inline void Message::SetOriginalID(const BufferPointer &value, unsigned int size)
{
    Check(OCI_MsgSetOriginalID(*this, value, size));
}

inline mstring Message::GetCorrelation()
{
    return MakeString(Check(OCI_MsgGetCorrelation(*this)));
}

inline void Message::SetCorrelation(mstring value)
{
    Check(OCI_MsgSetCorrelation(*this, value.c_str()));
}

inline mstring Message::GetExceptionQueue()
{
    return MakeString(Check(OCI_MsgGetExceptionQueue(*this)));
}

inline void Message::SetExceptionQueue(mstring value)
{
    Check(OCI_MsgSetExceptionQueue(*this, value.c_str()));
}

inline Agent Message::GetSender()
{
    return Agent(Check(OCI_MsgGetSender(*this)), 0);
}

inline void Message::SetSender(const Agent &agent)
{
    Check(OCI_MsgSetSender(*this, agent));
}

inline void Message::SetConsumers(std::vector<Agent> &agents)
{
    size_t size = agents.size();
    ManagedBuffer<OCI_Agent*> buffer = new OCI_Agent * [size];

    for (size_t i = 0; i < size; i++)
    {
       ((OCI_Agent **) buffer)[i] = (const Agent &) agents[i];
    }

    Check(OCI_MsgSetConsumers(*this, (OCI_Agent **) buffer, (unsigned int) size));
}

/* --------------------------------------------------------------------------------------------- *
 * Enqueue
 * --------------------------------------------------------------------------------------------- */

inline Enqueue::Enqueue(const TypeInfo &typeInfo, mstring queueName)
{
   Acquire(Check(OCI_EnqueueCreate(typeInfo, queueName.c_str())), (HandleFreeFunc) OCI_EnqueueFree, 0);
}

inline void Enqueue::Put(const Message &message)
{
    Check(OCI_EnqueuePut(*this, message));
}

inline unsigned int Enqueue::GetVisibility()
{
    return Check(OCI_EnqueueGetVisibility(*this));
}

inline void Enqueue::SetVisibility(unsigned int value)
{
    Check(OCI_EnqueueSetVisibility(*this, value));
}

inline unsigned int Enqueue::GetSequenceDeviation()
{
    return Check(OCI_EnqueueGetSequenceDeviation(*this));
}

inline void Enqueue::SetSequenceDeviation(unsigned int value)
{
    Check(OCI_EnqueueSetSequenceDeviation(*this, value));
}

inline void Enqueue::GetRelativeMsgID(BufferPointer value, unsigned int &size)
{
    Check(OCI_EnqueueGetRelativeMsgID(*this, value, &size));
}

inline void Enqueue::SetRelativeMsgID(const BufferPointer &value, unsigned int size)
{
    Check(OCI_EnqueueSetRelativeMsgID(*this, value, size));
}

/* --------------------------------------------------------------------------------------------- *
 * Dequeue
 * --------------------------------------------------------------------------------------------- */

inline Dequeue::Dequeue(const TypeInfo &typeInfo, mstring queueName)
{
   Acquire(Check(OCI_DequeueCreate(typeInfo, queueName.c_str())), (HandleFreeFunc) OCI_DequeueFree, 0);
}

inline Dequeue::Dequeue(OCI_Dequeue *pDequeue)
{
    Acquire(pDequeue, 0, 0);
}

inline Message Dequeue::Get()
{
    return Message(Check(OCI_DequeueGet(*this)), 0);
}

inline Agent Dequeue::Listen(int timeout)
{
    return Agent(Check(OCI_DequeueListen(*this, timeout)), 0);
}

inline mstring Dequeue::GetConsumer()
{
    return MakeString(Check(OCI_DequeueGetConsumer(*this)));
}

inline void Dequeue::SetConsumer(mstring value)
{
    Check(OCI_DequeueSetConsumer(*this, value.c_str()));
}

inline mstring Dequeue::GetCorrelation()
{
    return MakeString(Check(OCI_DequeueGetCorrelation(*this)));
}

inline void Dequeue::SetCorrelation(mstring value)
{
    Check(OCI_DequeueSetCorrelation(*this, value.c_str()));
}

inline void Dequeue::GetRelativeMsgID(BufferPointer value, unsigned int &size)
{
    Check(OCI_DequeueGetRelativeMsgID(*this, value, &size));
}

inline void Dequeue::SetRelativeMsgID(const BufferPointer &value, unsigned int size)
{
    Check(OCI_DequeueSetRelativeMsgID(*this, value, size));
}

inline unsigned int Dequeue::GetVisibility()
{
    return Check(OCI_DequeueGetVisibility(*this));
}

inline void Dequeue::SetVisibility(unsigned int value)
{
    Check(OCI_DequeueSetVisibility(*this, value));
}

inline unsigned int Dequeue::GetMode()
{
    return Check(OCI_DequeueGetMode(*this));
}

inline void Dequeue::SetMode(unsigned int value)
{
    Check(OCI_DequeueSetMode(*this, value));
}

inline unsigned int Dequeue::GetNavigation()
{
    return Check(OCI_DequeueGetNavigation(*this));
}

inline void Dequeue::SetNavigation(unsigned int value)
{
    Check(OCI_DequeueSetNavigation(*this, value));
}

inline int Dequeue::GetWaitTime()
{
    return Check(OCI_DequeueGetWaitTime(*this));
}

inline void Dequeue::SetWaitTime(int value)
{
    Check(OCI_DequeueSetWaitTime(*this, value));
}

inline void Dequeue::SetAgents(std::vector<Agent> &agents)
{
    size_t size = agents.size();
    ManagedBuffer<OCI_Agent*> buffer = new OCI_Agent * [size];

    for (size_t i = 0; i < size; i++)
    {
       ((OCI_Agent **) buffer)[i] = (const Agent &) agents[i];
    }

    Check(OCI_DequeueSetAgentList(*this, (OCI_Agent **) buffer, (unsigned int) size));
}

inline void Dequeue::Subscribe(unsigned int port, unsigned int timeout, NotifyAQHandlerProc handler)
{
    Check(OCI_DequeueSubscribe(*this, port, timeout, (POCI_NOTIFY_AQ) (handler != 0 ? Environment::NotifyHandlerAQ : 0 )));

    Environment::GetEnvironmentHandle().Callbacks.Set((OCI_Dequeue*) *this, (CallbackPointer) handler);
}

inline void Dequeue::Unsubscribe()
{
    Check(OCI_DequeueUnsubscribe(*this));
}

/* --------------------------------------------------------------------------------------------- *
 * DirectPath
 * --------------------------------------------------------------------------------------------- */

inline DirectPath::DirectPath(const TypeInfo &typeInfo, unsigned int nbCols, unsigned int  nbRows, mstring partition)
{
    Acquire(Check(OCI_DirPathCreate(typeInfo, partition.c_str(), nbCols, nbRows)), (HandleFreeFunc) OCI_DirPathFree, 0);
}

inline void DirectPath::SetColumn(unsigned int colIndex, mstring name, unsigned int maxSize,  mstring format)
{
    Check(OCI_DirPathSetColumn(*this, colIndex, name.c_str(), maxSize, format.c_str()));
}

inline void DirectPath::SetEntry(unsigned int rowIndex, unsigned int colIndex,  const dstring &value,  bool complete)
{
    Check(OCI_DirPathSetEntry(*this, rowIndex, colIndex, (void *) value.c_str(), value.size(), complete));
}

inline void DirectPath::SetEntry(unsigned int rowIndex, unsigned int colIndex,  const BufferPointer &value, unsigned int size,  bool complete )
{
    Check(OCI_DirPathSetEntry(*this, rowIndex, colIndex, value, size, complete));
}

inline void DirectPath::Reset()
{
    Check(OCI_DirPathReset(*this));
}

inline void DirectPath::Prepare()
{
    Check(OCI_DirPathPrepare(*this));
}

inline unsigned int DirectPath::Convert()
{
    return Check(OCI_DirPathConvert(*this));
}

inline unsigned int DirectPath::Load()
{
    return Check(OCI_DirPathLoad(*this));
}

inline void DirectPath::Finish()
{
    Check(OCI_DirPathFinish(*this));
}

inline void DirectPath::Abort()
{
    Check(OCI_DirPathAbort(*this));
}

inline void DirectPath::Save()
{
    Check(OCI_DirPathSave(*this));
}

inline void DirectPath::FlushRow()
{
    Check(OCI_DirPathFlushRow(*this));
}

inline void DirectPath::SetCurrentRows(unsigned int value)
{
    Check(OCI_DirPathSetCurrentRows(*this, value));
}

inline unsigned int DirectPath::GetCurrentRows()
{
    return Check(OCI_DirPathGetCurrentRows(*this));
}

inline unsigned int DirectPath::GetMaxRows()
{
    return Check(OCI_DirPathGetMaxRows(*this));
}

inline unsigned int DirectPath::GetRowCount()
{
    return Check(OCI_DirPathGetRowCount(*this));
}

inline unsigned int DirectPath::GetAffectedRows()
{
    return Check(OCI_DirPathGetAffectedRows(*this));
}

inline void DirectPath::SetDateFormat(mstring format)
{
    Check(OCI_DirPathSetDateFormat(*this, format.c_str()));
}

inline void DirectPath::SetParallel(bool value)
{
    Check(OCI_DirPathSetParallel(*this, value));
}

inline void DirectPath::SetNoLog(bool value)
{
    Check(OCI_DirPathSetNoLog(*this, value));
}

inline void DirectPath::SetCacheSize(unsigned int value)
{
    Check(OCI_DirPathSetCacheSize(*this, value));
}

inline void DirectPath::SetBufferSize(unsigned int value)
{
    Check(OCI_DirPathSetBufferSize(*this, value));
}

inline void DirectPath::SetConvertMode(unsigned int value)
{
    Check(OCI_DirPathSetConvertMode(*this, value));
}

inline unsigned int DirectPath::GetErrorColumn()
{
    return Check(OCI_DirPathGetErrorColumn(*this));
}

inline unsigned int DirectPath::GetErrorRow()
{
    return Check(OCI_DirPathGetErrorRow(*this));
}

/* --------------------------------------------------------------------------------------------- *
 * Queue
 * --------------------------------------------------------------------------------------------- */

inline void Queue::Create(const Connection &connection, mstring queue, mstring table, unsigned int queueType, unsigned int maxRetries,
            unsigned int retryDelay, unsigned int retentionTime, bool dependencyTracking, mstring comment)
{
    Check(OCI_QueueCreate(connection, queue.c_str(), table.c_str(), queueType, maxRetries, retryDelay, retentionTime, dependencyTracking, comment.c_str()));
}

inline void Queue::Alter(const Connection &connection, mstring queue, unsigned int maxRetries, unsigned int retryDelay, unsigned int retentionTime, mstring comment)
{
    Check(OCI_QueueAlter(connection, queue.c_str(), maxRetries, retryDelay, retentionTime, comment.c_str()));
}

inline void Queue::Drop(const Connection &connection, mstring queue)
{
    Check(OCI_QueueDrop(connection, queue.c_str()));
}

inline void Queue::Start(const Connection &connection, mstring queue, bool startEnqueue, bool startDequeue)
{
    Check(OCI_QueueStart(connection, queue.c_str(), startEnqueue, startDequeue));
}

inline void Queue::Stop(const Connection &connection, mstring queue, bool stopEnqueue, bool stopDequeue, bool wait)
{
    Check(OCI_QueueStop(connection, queue.c_str(), stopEnqueue, stopDequeue, wait));
}

/* --------------------------------------------------------------------------------------------- *
 * QueueTable
 * --------------------------------------------------------------------------------------------- */

inline void QueueTable::Create(const Connection &connection, mstring table, mstring payloadType, bool multipleConsumers, mstring storageClause, mstring sortList,
                    unsigned int messageGrouping, mstring comment, unsigned int primaryInstance, unsigned int secondaryInstance, mstring compatible)

{
    Check(OCI_QueueTableCreate(connection, table.c_str(), payloadType.c_str(), storageClause.c_str(), sortList.c_str(), multipleConsumers,
                                   messageGrouping, comment.c_str(), primaryInstance, secondaryInstance, compatible.c_str()));
}

inline void QueueTable::Alter(const Connection &connection, mstring table, mstring comment, unsigned int primaryInstance, unsigned int secondaryInstance)
{
    Check(OCI_QueueTableAlter(connection, table.c_str(), comment.c_str(), primaryInstance, secondaryInstance));
}

inline void QueueTable::Drop(const Connection &connection, mstring table, bool force)
{
    Check(OCI_QueueTableDrop(connection, table.c_str(), force));
}

inline void QueueTable::Purge(const Connection &connection, mstring table, unsigned int deliveryMode, mstring purgeCondition, bool block)
{
    Check(OCI_QueueTablePurge(connection, table.c_str(), purgeCondition.c_str(), block, deliveryMode));
}

inline void QueueTable::Migrate(const Connection &connection, mstring table, mstring compatible)
{
    Check(OCI_QueueTableMigrate(connection, table.c_str(), compatible.c_str()));
}

/**
 * @}
 */

}

#endif
