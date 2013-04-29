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

#include "ocilib.h"

namespace ocilib
{

/* Types */

#if defined(OCI_CHARSET_WIDE)

  typedef std::wstring  mstring;
  typedef std::wstring  dstring;

#elif defined(OCI_CHARSET_MIXED)

  typedef std::string   mstring;
  typedef std::wstring  dstring;

#else

  typedef std::string   mstring;
  typedef std::string   dstring;

#endif

  typedef std::basic_string<char, std::char_traits<unsigned char>, std::allocator<unsigned char> > Raw;

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
class Ref;
class Object;
class Collection;
class Clob;
class Blob;
class File;
class Long;
class Subscription;
class Queue;
class Dequeue;
class Enqueue;
class Pool;
class Mutex;
class Thread;
class Message;
class Long;
class Iterator;
class HashTable;
class Event;
class Element;
class Column;
class Agent;

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
}


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

    virtual void Destroy() = 0;
    virtual std::list<Handle *> & GetChildren() = 0;

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

    typedef boolean (OCI_API *HandleFreeFunc)(void *handle);

    Handle* GetHandle();

	void Acquire(THandleType handle, HandleFreeFunc func, bool releasable, Handle *parent = NULL);
    void Release();

    template <class TSmartHandleType>
    class SmartHandle : public Handle
    {
    public:

        SmartHandle(HandleHolder<TSmartHandleType> *holder, TSmartHandleType handle, HandleFreeFunc func, bool releasable, Handle *parent = NULL);

        void Acquire(HandleHolder<TSmartHandleType> *holder);
        void Release(HandleHolder<TSmartHandleType> *holder);

        TSmartHandleType GetHandle();

        void Destroy();
        std::list<Handle *> & GetChildren();

    private:

        void DetachFromHolders();

        std::list<HandleHolder<TSmartHandleType> *> _holders;
        std::list<Handle *>  _children;

        THandleType _handle;
        bool _releasable;
        HandleFreeFunc _func;
        Handle *_parent;

    };

protected:

    SmartHandle<THandleType> *_smartHandle;
 };


/**
 * @class Thread
 *
 * @brief
 *
 *
 */
class Thread : public HandleHolder<OCI_Thread *>
{
 public:

    Thread();

    void Run(POCI_THREAD func, void *args);
    void Join();
};


/**
 * @class Mutex
 *
 * @brief
 *
 *
 */
class Mutex : public HandleHolder<OCI_Mutex *>
{
public:

    Mutex();

    void Acquire();
    void Release();
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

public:

    typedef void (*HAHandlerProc) (const Connection &con, unsigned int source, unsigned int event, const Timestamp  &time);

    static void Initialize(unsigned int mode = OCI_ENV_DEFAULT, mstring libpath = MT(""));
    static void Cleanup();

    static unsigned int GetCompiledVersion();
    static unsigned int GetRuntimeVersion();

    static void EnableWarnings(bool value);

    static void StartDatabase(mstring db, mstring user, mstring pwd, unsigned int start_flag, unsigned int start_mode,
                              unsigned int sess_mode = OCI_SESSION_SYSDBA, mstring spfile = MT(""));

    static void ShutdownDatabase(mstring db, mstring user, mstring pwd, unsigned int shut_flag, unsigned int shut_mode,
                              unsigned int sess_mode = OCI_SESSION_SYSDBA);

    static void SetHAHandler(HAHandlerProc handler);

private:

    static void HAHandler(OCI_Connection *con, unsigned int source, unsigned int event, OCI_Timestamp  *time);

    static HAHandlerProc _haHandler;
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
    friend class Ref;

public:

    typedef void (*TAFHandlerProc) (Connection &con, unsigned int source, unsigned int event, Timestamp  &time);

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

    void PingServer();

    mstring GetConnectionString();
    mstring GetUserName();
    mstring GetPassword();

    unsigned int GetVersion();

    mstring GetServerVersion();
    unsigned int GetServerMajorVersion();
    unsigned int GetServerMinorVersion();
    unsigned int GetServerRevisionVersion();

    void SetPassword(mstring newPassword);
    void SetPassword(mstring db, mstring user, mstring pwd, mstring newPassword);

    mstring GetSessionTag();
    void SetSessionTag(mstring tag);

    Transaction GetTransaction();
    void SetTransaction(Transaction &transaction);

    void SetDefaultDateFormat(mstring format);
    void SetDefaultNumericFormat(mstring format);

    mstring  GetDefaultDateFormat();
    mstring  GetDefaultNumericFormat();

    void EnableServerOutput(unsigned int bufsize, unsigned int arrsize, unsigned int lnsize);
    void DisableServerOutput();
    mstring GetServerOutput();

    void SetTrace(unsigned int trace, mstring value);
    mstring GetTrace(unsigned int trace);

    mstring GetDatabase();
    mstring GetInstance();
    mstring GetService();
    mstring GettServer();
    mstring GetDomain();

    Timestamp GetInstanceStartTime();

    unsigned int GetStatementCacheSize();
    void SetStatementCacheSize(unsigned int value);

    unsigned int GetDefaultLobPrefetchSize();
    void SetDefaultLobPrefetchSize(unsigned int value);

    bool IsTAFCapable();

  //  void SetTAFHandler(TAFHandlerProc handler);

private:

    Connection(OCI_Connection *con, Handle *parent = NULL);

private:

  //  static void TAFHandler(OCI_Connection *con, unsigned int source, unsigned int event, OCI_Timestamp  *time);

 //   TAFHandlerProc _tafHandler;
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

public:

    Date();

    void Assign(const Date& source);

    int Compare(const Date& source);

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

public:

    Interval(Connection &con, unsigned int type);

    void Assign(const Interval& source);

    int Compare(const Interval& source);

    unsigned int GetType();

    void Add(const Interval& interval);
    void Substract(const Interval& interval);

    void FromTimeZone(mstring timeZone);

    void FromString(mstring data, mstring format = OCI_STRING_FORMAT_DATE);
    mstring ToString(mstring format = OCI_STRING_FORMAT_DATE);

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

public:

    Timestamp(Connection &con, unsigned int type);

    void Assign(const Timestamp& source);

    int Compare(const Timestamp& source);

    unsigned int GetType();

    void Construct(int year, int month, int day, int hour, int min, int sec, mstring timeZone = MT(""));
    void Convert(const Timestamp& source);

    void SetDate(int year, int month, int day);
    void SetTime(int hour, int min,   int sec);
    void SetDateTime(int year, int month, int day, int hour, int min, int sec);

    void GetDate(int *year, int *month, int *day);
    void GetTime(int *hour, int *min,   int *sec);
    void GetDateTime(int *year, int *month, int *day, int *hour, int *min, int *sec);

    mstring GetTimeZone();
    void GetTimeZoneOffset(int *hour, int *min);

    void AddInterval(const Interval& interval);
    void SubstractInterval(const Interval& interval);

    void Substract(const Interval& interval);

    void SysDatetime();

    void FromString(mstring data, mstring format = OCI_STRING_FORMAT_DATE);
    mstring ToString(mstring format = OCI_STRING_FORMAT_DATE);

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

public:

    Clob(Connection &connection);

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

public:

    Blob(Connection &connection);

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

public:

    File(Connection &connection);
    File(Connection &connection, mstring directory, mstring name);

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
    friend class Ref;

public:

    TypeInfo(Connection &connection, mstring name, unsigned int type);

private:

    TypeInfo(Connection &connection, OCI_TypeInfo *pTypeInfo);

    Connection &_connection;
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

public:

    Object(TypeInfo &typeInfo);

    TypeInfo &GetTypeInfo();

    short   GetShort    (mstring name);
    int     GetInt      (mstring name);
    float   GetFloat    (mstring name);
    double  GetDouble   (mstring name);
    big_int GetBigInt   (mstring name);
    dstring GetString   (mstring name);
    Date    GetDate     (mstring name);
    Clob    GetClob     (mstring name);

    void    SetShort    (mstring name, short    value);
    void    SetInt      (mstring name, int      value);
    void    SetFloat    (mstring name, float    value);
    void    SetDouble   (mstring name, double   value);
    void    SetBigInt   (mstring name, big_int  value);
    void    SetString   (mstring name, dstring  value);
    void    SetDate     (mstring name, Date     value);
    void    SetClob     (mstring name, Clob     value);

private:

    Object(OCI_Object *pObject);
    TypeInfo &_typeInfo;

};

/**
 * @class Object
 *
 * @brief
 *
 *
 */
class Ref : public HandleHolder<OCI_Ref *>
{
    friend class Statement;
    friend class Resultset;
    friend class BindArray;

public:

    Ref(TypeInfo &typeInfo);

    TypeInfo &GetTypeInfo();
    Object GetObject();

    void Assign(const Ref& other);

    bool GetNullRef();
    void SetNulRef();

    mstring ToString();

private:

    Ref(OCI_Ref *pRef);
    TypeInfo &_typeInfo;
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

public:

    Statement(Connection &connection);
    ~Statement();

    void Prepare(mstring sql);
    void Execute();
    void Execute(mstring sql);

    unsigned int GetAffectedRows();

    void SetBindArraySize(unsigned int size);

    void Bind(mstring name, short &value, unsigned int mode = OCI_BDM_IN);
    void Bind(mstring name, unsigned short &value, unsigned int mode = OCI_BDM_IN);

    void Bind(mstring name, int &value, unsigned int mode = OCI_BDM_IN);
    void Bind(mstring name, unsigned int &value, unsigned int mode = OCI_BDM_IN);

    void Bind(mstring name, big_int &value, unsigned int mode = OCI_BDM_IN);
    void Bind(mstring name, big_uint &value, unsigned int mode = OCI_BDM_IN);

    void Bind(mstring name, float &value, unsigned int mode = OCI_BDM_IN);
    void Bind(mstring name, double &value, unsigned int mode = OCI_BDM_IN);

    void Bind(mstring name, Clob &value, unsigned int mode = OCI_BDM_IN);
    void Bind(mstring name, Date &value, unsigned int mode = OCI_BDM_IN);

    void Bind(mstring name, dstring &value, unsigned int maxSize = 0);

    void Bind(mstring name, std::vector<short> &values, unsigned int mode = OCI_BDM_IN);
    void Bind(mstring name, std::vector<unsigned short> &values, unsigned int mode = OCI_BDM_IN);
    void Bind(mstring name, std::vector<int> &values, unsigned int mode = OCI_BDM_IN);
    void Bind(mstring name, std::vector<unsigned int> &values, unsigned int mode = OCI_BDM_IN);
    void Bind(mstring name, std::vector<big_int> &values, unsigned int mode = OCI_BDM_IN);
    void Bind(mstring name, std::vector<big_uint> &values, unsigned int mode = OCI_BDM_IN);
    void Bind(mstring name, std::vector<float> &values, unsigned int mode = OCI_BDM_IN);
    void Bind(mstring name, std::vector<double> &values, unsigned int mode = OCI_BDM_IN);

    void Bind(mstring name, std::vector<Clob>    &values, unsigned int mode = OCI_BDM_IN);
    void Bind(mstring name, std::vector<Date>    &values, unsigned int mode = OCI_BDM_IN);

    void Bind(mstring name, std::vector<dstring> &values, unsigned int mode = OCI_BDM_IN, unsigned int maxSize = 0);

private:

    Statement(const Connection &connection, OCI_Statement *stmt);

    void SetInData();
    void SetOutData();
    void ClearBinds();

    template <typename TBindMethod, class TDataType>
    void Bind (TBindMethod &method, mstring name, TDataType& value, unsigned int mode);

    template <typename TBindMethod, class TObjectType, class TDataType>
    void Bind (TBindMethod &method, mstring name, TObjectType &value, BindValue<TDataType> datatype, unsigned int mode);

    template <typename TBindMethod, class TObjectType, class TDataType>
    void Bind (TBindMethod &method, mstring name, std::vector<TObjectType> &values, BindValue<TDataType> datatype, unsigned int mode);

    template <typename TBindMethod, class TObjectType, class TDataType>
    void Bind (TBindMethod &method, mstring name, std::vector<TObjectType> &values, BindValue<TDataType> datatype, unsigned int mode, unsigned int type);

    std::vector<BindString *> _bindStrings;
    std::vector<BindArray  *> _bindArrays;
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
public:

    Resultset(Statement &statement);

    bool Next();

    short   GetShort    (mstring name);
    int     GetInt      (mstring name);
    float   GetFloat    (mstring name);
    double  GetDouble   (mstring name);
    big_int GetBigInt   (mstring name);
    dstring GetString   (mstring name);
    Date    GetDate     (mstring name);
    Clob    GetClob     (mstring name);

    short   GetShort    (int index);
    int     GetInt      (int index);
    float   GetFloat    (int index);
    double  GetDouble   (int index);
    big_int GetBigInt   (int index);
    dstring GetString   (int index);
    Date    GetDate     (int index);
    Clob    GetClob     (int index);

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
    if (src._smartHandle)
    {
        src._smartHandle->Acquire(this);
        _smartHandle = src._smartHandle;
    }
}

template<class THandleType>
inline HandleHolder<THandleType>::~HandleHolder()
{
    Release();
}


template<class THandleType>
inline bool HandleHolder<THandleType>::IsNull()
{
    return  (((THandleType) *this) != 0);
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
inline Handle * HandleHolder<THandleType>::GetHandle()
{
    return ( Handle *) _smartHandle;
}

template<class THandleType>
inline void HandleHolder<THandleType>::Acquire(THandleType handle, HandleFreeFunc func, bool releasable, Handle *parent)
{
    _smartHandle = new HandleHolder::SmartHandle<THandleType>(this, handle, func, releasable, parent);
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

template <class THandleType>
template <class TSmartHandleType>
inline HandleHolder<THandleType>::SmartHandle<TSmartHandleType>::SmartHandle(HandleHolder<TSmartHandleType> *holder, TSmartHandleType handle, HandleFreeFunc func, bool releasable, Handle *parent) : _handle(handle), _releasable(releasable), _func(func), _parent(parent)
{
    Acquire(holder);

    if (_parent && _handle)
    {
        _parent->GetChildren().push_back(this);
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
    holder->_smartHandle = 0;

    _holders.remove(holder);

    if (_holders.size() == 0)
    {
        Destroy();
    }
}

template <class THandleType>
template <class TSmartHandleType>
inline TSmartHandleType HandleHolder<THandleType>::SmartHandle<TSmartHandleType>::GetHandle()
{
    return _handle;
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

template <class THandleType>
template <class TSmartHandleType>
inline void HandleHolder<THandleType>::SmartHandle<TSmartHandleType>::Destroy()
{
    boolean ret = TRUE;
    boolean chk = FALSE;

    if (_releasable)
    {
        if (_parent && _handle)
        {
            _parent->GetChildren().remove(this);
        }

        for(std::list<Handle *>::iterator it = _children.begin(); it != _children.end(); it++)
        {
            SmartHandle<TSmartHandleType> *handle = reinterpret_cast<SmartHandle<TSmartHandleType> *> (*it);

            handle->_parent = 0;

            handle->DetachFromHolders();
            handle->Destroy();
        }

        if (_func)
        {
            ret = _func(_handle);
            chk = TRUE;
        }
    }

    if (chk)
    {
        API::Call(ret);
    }

    delete this;
}

/* --------------------------------------------------------------------------------------------- *
 * Thread
 * --------------------------------------------------------------------------------------------- */

inline Thread::Thread()
{
   Acquire(API::Call(OCI_ThreadCreate()), (HandleFreeFunc) OCI_ThreadFree, true);
}

inline void Thread::Run(POCI_THREAD func, void *args)
{
    API::Call(OCI_ThreadRun(*this, func, args));
}

inline void Thread::Join()
{
    API::Call(OCI_ThreadJoin(*this));
}

/* --------------------------------------------------------------------------------------------- *
 * Mutex
 * --------------------------------------------------------------------------------------------- */

inline Mutex::Mutex()
{
   HandleHolder::Acquire(API::Call(OCI_MutexCreate()), (HandleFreeFunc) OCI_MutexFree, true);
}

inline void Mutex::Acquire()
{
    API::Call(OCI_MutexAcquire(*this));
}

inline void Mutex::Release()
{
    API::Call(OCI_MutexRelease(*this));
}

/* --------------------------------------------------------------------------------------------- *
 * Exception
 * --------------------------------------------------------------------------------------------- */

inline Exception::Exception()
{

}

inline Exception::Exception(OCI_Error *err)
{
    Acquire(err, 0, false);
}

inline mstring Exception::GetMessage()
{
    return OCI_ErrorGetString(*this);
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
    return Statement(Connection(OCI_ErrorGetConnection(*this)), OCI_ErrorGetStatement(*this));
}

inline Connection Exception::GetConnection()
{
    return Connection(OCI_ErrorGetConnection(*this));
}

/* --------------------------------------------------------------------------------------------- *
 * Environment
 * --------------------------------------------------------------------------------------------- */

Environment::HAHandlerProc Environment::_haHandler = 0;

inline void Environment::Initialize(unsigned int mode, mstring libpath)
{
    OCI_Initialize(0, libpath.c_str(),  mode | OCI_ENV_CONTEXT);

    API::Call();
}

inline void Environment::Cleanup()
{
    OCI_Cleanup();

    API::Call();
}

inline unsigned int Environment::GetCompiledVersion()
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

inline void Environment::SetHAHandler(HAHandlerProc handler)
{
    _haHandler = handler;
}

void Environment::HAHandler(OCI_Connection *con, unsigned int source, unsigned int event, OCI_Timestamp  *time)
{
    if (_haHandler)
    {
        _haHandler(Connection(con), source, event, Timestamp(time));
    }
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
            minCon, maxCon, incrCon)), (HandleFreeFunc) OCI_PoolFree, true);
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
    Acquire(con, (HandleFreeFunc) (parent != 0 ? OCI_ConnectionFree : 0), (parent != 0), parent);
}

inline void Connection::Open(mstring db, mstring user, mstring pwd, unsigned int sessionMode)
{
    Release();

    Acquire(API::Call(OCI_ConnectionCreate(db.c_str(), user.c_str(), pwd.c_str(), (int) sessionMode)),
            (HandleFreeFunc) OCI_ConnectionFree, true);
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

inline mstring Connection::GetServerVersion()
{
    return API::Call( OCI_GetVersionServer(*this));
}

/* --------------------------------------------------------------------------------------------- *
 * Date
 * --------------------------------------------------------------------------------------------- */

inline Date::Date()
{
    Acquire(API::Call(OCI_DateCreate(NULL)), (HandleFreeFunc) OCI_DateFree, true);
}

inline Date::Date(OCI_Date *pDate)
{
    Acquire(pDate, (HandleFreeFunc) OCI_DateFree, false);
}

inline void Date::Assign(const Date& source)
{
    API::Call(OCI_DateAssign(*this, source));
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

inline void Date::GetTime(int *hour, int *min,   int *sec)
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
    dtext buffer[256];

    API::Call(OCI_DateToText(*this, format.c_str(), 256, buffer));

    return buffer;
}

inline Date::operator mstring()
{
    return ToString();
}


/* --------------------------------------------------------------------------------------------- *
 * Timestamp
 * --------------------------------------------------------------------------------------------- */

inline Timestamp::Timestamp(Connection &connection, unsigned int type)
{
    Acquire(API::Call(OCI_TimestampCreate(connection, type)), (HandleFreeFunc) OCI_TimestampFree, true, connection.GetHandle());
}

inline Timestamp::Timestamp(OCI_Timestamp *pTimestamp)
{
    Acquire(pTimestamp, (HandleFreeFunc) OCI_TimestampFree, false);
}

/* --------------------------------------------------------------------------------------------- *
 * Clob
 * --------------------------------------------------------------------------------------------- */

inline Clob::Clob(Connection &connection)
{
    Acquire(API::Call(OCI_LobCreate(connection, OCI_CLOB)), (HandleFreeFunc) OCI_LobFree, true, connection.GetHandle());
}

inline Clob::Clob(OCI_Lob *pLob)
{
    Acquire(pLob, (HandleFreeFunc) OCI_LobFree, false);
}

inline dstring Clob::Read(unsigned int size)
{
    dstring result;

    result.reserve(size);

    size = API::Call(OCI_LobRead(*this, (void *) result.c_str(), size));

    return result.c_str();
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

inline Blob::Blob(Connection &connection)
{
    Acquire(API::Call(OCI_LobCreate(connection, OCI_BLOB)), (HandleFreeFunc) OCI_LobFree, true, connection.GetHandle());
}

inline Blob::Blob(OCI_Lob *pLob)
{
    Acquire(pLob, (HandleFreeFunc) OCI_LobFree, false);
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

inline File::File(Connection &connection)
{
    Acquire(API::Call(OCI_FileCreate(connection, OCI_BFILE)), (HandleFreeFunc) OCI_FileFree, true, connection.GetHandle());
}


inline File::File(Connection &connection, mstring directory, mstring name)
{
    Acquire(API::Call(OCI_FileCreate(connection, OCI_BFILE)), (HandleFreeFunc) OCI_FileFree, true, connection.GetHandle());

    SetInfos(directory, name);
}

inline File::File(OCI_File *pFile)
{
    Acquire(pFile, (HandleFreeFunc) OCI_FileFree, false);
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
    return API::Call(OCI_FileGetName(*this));
}

inline mstring File::GetDirectory()
{
    return API::Call(OCI_FileGetDirectory(*this));
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

inline TypeInfo::TypeInfo(Connection &connection, mstring name, unsigned int type) : _connection(connection)
{
    Acquire(API::Call(OCI_TypeInfoGet(_connection, name.c_str(), type)), (HandleFreeFunc) 0, false, connection.GetHandle());
}

inline TypeInfo::TypeInfo(Connection &connection, OCI_TypeInfo *pTypeInfo) : _connection(connection)
{
    Acquire(pTypeInfo, (HandleFreeFunc) 0, false);
}

/* --------------------------------------------------------------------------------------------- *
 * Object
 * --------------------------------------------------------------------------------------------- */

inline Object::Object(TypeInfo &typeInfo) : _typeInfo(typeInfo)
{
    Acquire(API::Call(OCI_ObjectCreate(_typeInfo._connection, _typeInfo)), (HandleFreeFunc) OCI_ObjectFree, true, _typeInfo._connection.GetHandle());
}

inline TypeInfo & Object::GetTypeInfo  ()
{
    return _typeInfo;
}

inline short Object::GetShort(mstring name)
{
    return API::Call(OCI_ObjectGetShort(*this, name.c_str()));
}

inline int Object::GetInt(mstring name)
{
    return API::Call(OCI_ObjectGetInt(*this, name.c_str()));
}

inline float Object::GetFloat(mstring name)
{
    return API::Call(OCI_ObjectGetFloat(*this, name.c_str()));
}

inline double Object::GetDouble(mstring name)
{
    return API::Call(OCI_ObjectGetDouble(*this, name.c_str()));
}

inline big_int Object::GetBigInt(mstring name)
{
    return API::Call(OCI_ObjectGetBigInt(*this, name.c_str()));
}

inline dstring Object::GetString(mstring name)
{
    return API::Call(OCI_ObjectGetString(*this, name.c_str()));
}

inline Date Object::GetDate(mstring name)
{
    return API::Call(OCI_ObjectGetDate(*this, name.c_str()));
}

inline Clob Object::GetClob(mstring name)
{
    return API::Call(OCI_ObjectGetLob(*this, name.c_str()));
}

inline void Object::SetShort(mstring name, short value)
{
    API::Call(OCI_ObjectSetShort(*this, name.c_str(), value));
}

inline void Object::SetInt(mstring name, int value)
{
    API::Call(OCI_ObjectSetInt(*this, name.c_str(), value));
}

inline void Object::SetFloat(mstring name, float value)
{
    API::Call(OCI_ObjectSetFloat(*this, name.c_str(), value));
}

inline void Object::SetDouble(mstring name, double value)
{
    API::Call(OCI_ObjectSetDouble(*this, name.c_str(), value));
}

inline void Object::SetBigInt(mstring name, big_int value)
{
    API::Call(OCI_ObjectSetBigInt(*this, name.c_str(), value));
}

inline void Object::SetString(mstring name, dstring value)
{
    API::Call(OCI_ObjectSetString(*this, name.c_str(), value.c_str()));
}

inline void Object::SetDate(mstring name, Date value)
{
    API::Call(OCI_ObjectSetDate(*this, name.c_str(), value));
}

inline void Object::SetClob(mstring name, Clob value)
{
    API::Call(OCI_ObjectSetLob(*this, name.c_str(), value));
}

/* --------------------------------------------------------------------------------------------- *
 * Ref
 * --------------------------------------------------------------------------------------------- */

inline Ref::Ref(TypeInfo &typeInfo) : _typeInfo(typeInfo)
{
    Acquire(API::Call(OCI_RefCreate(_typeInfo._connection, _typeInfo)), (HandleFreeFunc) OCI_RefFree, true, _typeInfo._connection.GetHandle());
}

inline TypeInfo& Ref::GetTypeInfo()
{
    return _typeInfo;
}

inline Object Ref::GetObject()
{

}

inline void Ref::Assign(const Ref& other)
{
   API::Call(OCI_RefAssign(*this, other));
}

inline bool Ref::GetNullRef()
{
    return (API::Call(OCI_RefIsNull(*this)) == TRUE);
}

inline void Ref::SetNulRef()
{
    API::Call(OCI_RefSetNull(*this));
}

inline mstring Ref::ToString()
{
    mstring res;
    
    unsigned int size = API::Call(OCI_RefGetHexSize(*this));

    res.reserve(size);

    API::Call(OCI_RefToText(*this, size, (mtext *) res.c_str()));

    return res.c_str();
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

        for (it = _vector.begin(), it_end = _vector.end(); it != it_end && index < _elemSize; it++, index++)
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

        for (it = _vector.begin(), it_end = _vector.end(); it != it_end && index < _elemSize; it++, index++)
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

        for (it = _vector.begin(), it_end = _vector.end(); it != it_end && index < _elemSize; it++, index++)
        {
            *it = _data[index];
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

        for (it = _vector.begin(), it_end = _vector.end(); it != it_end && index < _elemSize; it++, index++)
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

void BindString::SetOutData()
{
    _string = _data;
}

BindString::BindString(dstring &string, unsigned int elemSize) : _string(string), _elemSize(elemSize)
{
    _data = new dtext[_elemSize];

     memset( _data, 0, _elemSize);
}

BindString::~BindString()
{
    delete [] _data;
}

BindString::operator dtext *()
{
    return _data;
}

/* --------------------------------------------------------------------------------------------- *
 * Statement
 * --------------------------------------------------------------------------------------------- */

inline Statement::Statement(Connection &connection)
{
    Acquire(API::Call(OCI_StatementCreate(connection)), (HandleFreeFunc) OCI_StatementFree, true, connection.GetHandle());
}

inline Statement::Statement(const Connection &connection, OCI_Statement *stmt)
{
     Acquire(stmt, (HandleFreeFunc) OCI_StatementFree, false);
}

inline Statement::~Statement()
{
    ClearBinds();
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

inline void Statement::SetBindArraySize(unsigned int size)
{
    API::Call(OCI_BindArraySetSize(*this, size));
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

    _bindArrays.push_back(bnd);

    API::Call(method(*this, name.c_str(), (TDataType *) bnd->GetData<TObjectType, TDataType>(), 0));
    API::Call(OCI_BindSetDirection(API::Call(OCI_GetBind(*this, API::Call(OCI_GetBindCount(*this)))), mode));
}

template <typename TBindMethod, class TObjectType, class TDataType>
inline void Statement::Bind (TBindMethod &method, mstring name, std::vector<TObjectType> &values, BindValue<TDataType> datatype, unsigned int mode, unsigned int type)
{
    BindArray * bnd = new BindArray(values, datatype, mode, OCI_BindArrayGetSize(*this), sizeof(TDataType));

    _bindArrays.push_back(bnd);

    method(*this, name.c_str(), (TDataType *) bnd->GetData<TObjectType, TDataType>(), type, 0);
    API::Call(OCI_BindSetDirection(API::Call(OCI_GetBind(*this, API::Call(OCI_GetBindCount(*this)))), mode));
}

inline void Statement::Bind(mstring name, short &value, unsigned int mode)
{
    Bind(OCI_BindShort, name, value, mode);
}

inline void Statement::Bind(mstring name, unsigned short &value, unsigned int mode)
{
    Bind(OCI_BindUnsignedShort, name, value, mode);
}

inline void Statement::Bind(mstring name, int &value, unsigned int mode)
{
    Bind(OCI_BindInt, name, value, mode);
}

inline void Statement::Bind(mstring name, unsigned int &value, unsigned int mode)
{
    Bind(OCI_BindUnsignedInt, name, value, mode);
}

inline void Statement::Bind(mstring name, big_int &value, unsigned int mode)
{
    Bind(OCI_BindBigInt, name, value, mode);
}

inline void Statement::Bind(mstring name, big_uint &value, unsigned int mode)
{
    Bind(OCI_BindUnsignedBigInt, name, value, mode);
}

inline void Statement::Bind(mstring name, float &value, unsigned int mode)
{
    Bind(OCI_BindFloat, name, value, mode);
}

inline void Statement::Bind(mstring name, double &value, unsigned int mode)
{
    Bind(OCI_BindDouble, name, value, mode);
}

inline void Statement::Bind(mstring name, Clob &value, unsigned int mode)
{
    Bind(OCI_BindLob, name, value, BindValue<OCI_Lob *>(), mode);
}

inline void Statement::Bind(mstring name, Date &value, unsigned int mode)
{
    Bind(OCI_BindDate, name, value, BindValue<OCI_Date *>(), mode);
}

inline void Statement::Bind(mstring name, dstring &value, unsigned int maxSize)
{
    int mode = OCI_BDM_IN_OUT;

    if (maxSize > 0)
    {
        BindString * bnd = new BindString(value, maxSize+1);
        _bindStrings.push_back(bnd);

        API::Call(OCI_BindString(*this, name.c_str(), (dtext*) (*bnd), maxSize));
        mode = OCI_BDM_IN_OUT;
    }
    else
    {
        API::Call(OCI_BindString(*this, name.c_str(), (dtext *) value.c_str(), value.size()));
        mode = OCI_BDM_IN;
    }

    API::Call(OCI_BindSetDirection(API::Call(OCI_GetBind(*this, API::Call(OCI_GetBindCount(*this)))), mode));
}

inline void Statement::Bind(mstring name, std::vector<short> &values, unsigned int mode)
{
    Bind(OCI_BindArrayOfShorts, name, values, BindValue<short>(), mode);
}

inline void Statement::Bind(mstring name, std::vector<unsigned short> &values, unsigned int mode)
{
    Bind(OCI_BindArrayOfUnsignedShorts, name, values, BindValue<unsigned short>(), mode);
}

inline void Statement::Bind(mstring name, std::vector<int> &values, unsigned int mode)
{
    Bind(OCI_BindArrayOfInts, name, values, BindValue<int>(), mode);
}

inline void Statement::Bind(mstring name, std::vector<unsigned int> &values, unsigned int mode)
{
    Bind(OCI_BindArrayOfUnsignedInts, name, values, BindValue<unsigned int>(), mode);
}

inline void Statement::Bind(mstring name, std::vector<big_int> &values, unsigned int mode)
{
    Bind(OCI_BindArrayOfBigInts, name, values, BindValue<big_int>(), mode);
}

inline void Statement::Bind(mstring name, std::vector<big_uint> &values, unsigned int mode)
{
    Bind(OCI_BindArrayOfUnsignedBigInts, name, values, BindValue<big_uint>(), mode);
}

inline void Statement::Bind(mstring name, std::vector<float> &values, unsigned int mode)
{
    Bind(OCI_BindArrayOfFloats, name, values, BindValue<float>(), mode);
}

inline void Statement::Bind(mstring name, std::vector<double> &values, unsigned int mode)
{
    Bind(OCI_BindArrayOfDoubles, name, values, BindValue<double>(), mode);
}

inline void Statement::Bind(mstring name, std::vector<Clob> &values, unsigned int mode)
{
    Bind(OCI_BindArrayOfLobs, name, values, BindValue<OCI_Lob *>(), mode, OCI_CLOB);
}

inline void Statement::Bind(mstring name, std::vector<Date> &values, unsigned int mode)
{
    Bind(OCI_BindArrayOfDates, name, values, BindValue<OCI_Date *>(), mode);
}

inline void Statement::Bind(mstring name, std::vector<dstring> &values, unsigned int mode, unsigned int maxSize)
{
    BindArray * bnd = new BindArray(values, BindValue<dtext>(), mode, OCI_BindArrayGetSize(*this), maxSize+1);

    _bindArrays.push_back(bnd);

    API::Call(OCI_BindArrayOfStrings(*this, name.c_str(), bnd->GetData<dstring, dtext>(), maxSize, 0));
    API::Call(OCI_BindSetDirection(API::Call(OCI_GetBind(*this, API::Call(OCI_GetBindCount(*this)))), mode));
}

inline void Statement::ClearBinds()
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

inline void Statement::SetOutData()
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

inline void Statement::SetInData()
{
    std::vector<BindArray *>::iterator ita, ita_end;

    for(ita = _bindArrays.begin(), ita_end = _bindArrays.end(); ita < ita_end; ita++)
    {
        (*ita)->SetInData();
    }
}

/* --------------------------------------------------------------------------------------------- *
 * Resultset
 * --------------------------------------------------------------------------------------------- */

inline Resultset::Resultset(Statement &statement)
{
    Acquire(API::Call(OCI_GetResultset(statement)), 0, false);
}

inline bool Resultset::Next()
{
    return (API::Call(OCI_FetchNext(*this)) == TRUE);
}

inline short Resultset::GetShort(mstring name)
{
    return API::Call(OCI_GetShort2(*this, name.c_str()));
}

inline int Resultset::GetInt(mstring name)
{
    return API::Call(OCI_GetInt2(*this, name.c_str()));
}

inline float Resultset::GetFloat(mstring name)
{
    return API::Call(OCI_GetFloat2(*this, name.c_str()));
}

inline double Resultset::GetDouble(mstring name)
{
    return API::Call(OCI_GetDouble2(*this, name.c_str()));
}

inline big_int Resultset::GetBigInt(mstring name)
{
    return API::Call(OCI_GetBigInt2(*this, name.c_str()));
}

inline dstring Resultset::GetString(mstring name)
{
    return API::Call(OCI_GetString2(*this, name.c_str()));
}

inline Date Resultset::GetDate(mstring name)
{
    return API::Call(OCI_GetDate2(*this, name.c_str()));
}

inline Clob Resultset::GetClob(mstring name)
{
    return API::Call(OCI_GetLob2(*this, name.c_str()));
}

inline short Resultset::GetShort(int index)
{
    return API::Call(OCI_GetShort(*this, index));
}

inline int Resultset::GetInt(int index)
{
    return API::Call(OCI_GetInt(*this, index));
}

inline float Resultset::GetFloat(int index)
{
    return API::Call(OCI_GetFloat(*this, index));
}

inline double Resultset::GetDouble(int index)
{
    return API::Call(OCI_GetDouble(*this, index));
}

inline big_int Resultset::GetBigInt(int index)
{
    return API::Call(OCI_GetBigInt(*this, index));
}

inline dstring Resultset::GetString(int index)
{
    return API::Call(OCI_GetString(*this, index));
}

inline Date Resultset::GetDate(int index)
{
    return API::Call(OCI_GetDate(*this, index));
}

inline Clob Resultset::GetClob(int index)
{
    return API::Call(OCI_GetLob(*this, index));
}


}

#endif
