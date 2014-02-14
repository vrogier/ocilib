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
 * @typedef ostring
 * @brief
 * string class wrapping the OCILIB otext * type and OTEXT() macros (see @ref OcilibCApiSupportedCharsets)
 */

typedef std::basic_string<otext, std::char_traits<otext>, std::allocator<otext> > ostring;

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

#include "ocilib_priv.hpp"

/**
 * @defgroup OcilibCppApiTypes Types
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
typedef unsigned int (*TAFHandlerProc) (Connection &con, unsigned int type, unsigned int event);

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

    ostring         GetMessage() const;
    unsigned int    GetType() const;
    int             GetOracleErrorCode() const;
    int             GetInternalErrorCode() const;
    Statement       GetStatement() const;
    Connection      GetConnection() const;
    unsigned int    GetRow() const;

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

    static void Initialize(unsigned int mode = OCI_ENV_DEFAULT, ostring libpath = OTEXT(""));
    static void Cleanup();

    static unsigned int GetMode();

    static unsigned int GetImportMode();

    static unsigned int GetCharset();

    static unsigned int GetCompileVersion();
    static unsigned int GetRuntimeVersion();

    static void EnableWarnings(bool value);

    static void StartDatabase(ostring db, ostring user, ostring pwd, unsigned int start_flag, unsigned int startMode,
                              unsigned int sessMode = OCI_SESSION_SYSDBA, ostring spfile = OTEXT(""));

    static void ShutdownDatabase(ostring db, ostring user, ostring pwd, unsigned int shut_flag, unsigned int shutMode,
                              unsigned int sessMode = OCI_SESSION_SYSDBA);

    static void ChangeUserPassword(ostring db, ostring user, ostring pwd, ostring newPassword);

    static void SetHAHandler(HAHandlerProc handler);

private:

    static void HAHandler(OCI_Connection *pConnection, unsigned int source, unsigned int event, OCI_Timestamp  *pTimestamp);
    static unsigned int TAFHandler(OCI_Connection *pConnection, unsigned int type, unsigned int event);
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
    static void Join(ThreadHandle handle);
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
    static void Create(ostring name, ThreadKeyFreeProc freeProc = 0);
    static void SetValue(ostring name, void *value);
    static void * GetValue(ostring name);
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
    Pool(ostring db, ostring user, ostring pwd, unsigned int poolType,
         unsigned int minCon, unsigned int maxCon, unsigned int incrCon = 1,
         unsigned int sessionMode = OCI_SESSION_DEFAULT);


    void Open(ostring db, ostring user, ostring pwd, unsigned int poolType,
              unsigned int minCon, unsigned int maxCon, unsigned int incrCon = 1,
              unsigned int sessionMode = OCI_SESSION_DEFAULT);
    void Close();

    Connection GetConnection(ostring sessionTag = OTEXT(""));

    unsigned int GetTimeout() const;
    void SetTimeout(unsigned int value);

    bool GetNoWait() const;
    void SetNoWait(bool value);

    unsigned int GetBusyConnectionsCount() const;
    unsigned int GetOpenedConnectionsCount() const;

    unsigned int GetMinSize() const;
    unsigned int GetMaxSize() const;
    unsigned int GetIncrement() const;

    unsigned int GetStatementCacheSize() const;
    void SetStatementCacheSize(unsigned int value);
};

/**
 * @class Connection
 *
 * @brief
 * A connection or session with a specific database.
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
    Connection(ostring db, ostring user, ostring pwd, unsigned int sessionMode = OCI_SESSION_DEFAULT);

    void Open(ostring db, ostring user, ostring pwd, unsigned int sessionMode = OCI_SESSION_DEFAULT);
    void Close();

    void Commit();
    void Rollback();
    void Break();

    void SetAutoCommit(bool enabled);
    bool GetAutoCommit() const;

    bool IsServerAlive() const;

    bool PingServer() const;

    ostring GetConnectionString() const;
    ostring GetUserName() const;
    ostring GetPassword() const;

    unsigned int GetVersion() const;

    ostring GetServerVersion() const;
    unsigned int GetServerMajorVersion() const;
    unsigned int GetServerMinorVersion() const;
    unsigned int GetServerRevisionVersion() const;

    void ChangePassword(ostring newPassword);

    ostring GetSessionTag() const;
    void SetSessionTag(ostring tag);

    Transaction GetTransaction() const;
    void SetTransaction(const Transaction &transaction);

    void SetDefaultDateFormat(ostring format);
    void SetDefaultNumericFormat(ostring format);

    ostring  GetDefaultDateFormat() const;
    ostring  GetDefaultNumericFormat() const;

    void EnableServerOutput(unsigned int bufsize, unsigned int arrsize, unsigned int lnsize);
    void DisableServerOutput();
    bool GetServerOutput(ostring &line) const;
    void GetServerOutput(std::vector<ostring> &lines) const;

    void SetTrace(unsigned int trace, ostring value);
    ostring GetTrace(unsigned int trace) const;

    ostring GetDatabase() const;
    ostring GetInstance() const;
    ostring GetService() const;
    ostring GetServer() const;
    ostring GetDomain() const;

    Timestamp GetInstanceStartTime() const;

    unsigned int GetStatementCacheSize() const;
    void SetStatementCacheSize(unsigned int value);

    unsigned int GetDefaultLobPrefetchSize() const;
    void SetDefaultLobPrefetchSize(unsigned int value);

    bool IsTAFCapable() const;

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

   unsigned int GetMode() const;
   unsigned int GetTimeout() const;

private:

    Transaction(OCI_Transaction *trans);
};

/**
 * @class Date
 *
 * @brief
 * Object identififying the SQL datatype DATE.
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
    int Compare(const Date& other) const;

    bool IsValid() const;

    int GetYear() const;
    void SetYear(int value);
 
    int GetMonth() const;
    void SetMonth(int value);

    int GetDay() const;
    void SetDay(int value);
 
    int GetHours() const;
    void SetHours(int value);
  
    int GetMinutes() const;
    void SetMinutes(int value);

    int GetSeconds() const;
    void SetSeconds(int value);

    int DaysBetween(const Date& other) const;

    void SetDate(int year, int month, int day);
    void SetTime(int hour, int min,   int sec);
    void SetDateTime(int year, int month, int day, int hour, int min, int sec);

    void GetDate(int *year, int *month, int *day) const;
    void GetTime(int *hour, int *min,   int *sec) const;
    void GetDateTime(int *year, int *month, int *day, int *hour, int *min, int *sec) const;

    void AddDays(int days);
    void AddMonths(int months);

    void SysDate();
    void NextDay(ostring day);
    void LastDay();

    void ChangeTimeZone(ostring tzSrc, ostring tzDst);

    void FromString(ostring data, ostring format = OCI_STRING_FORMAT_DATE);
    ostring ToString(ostring format = OCI_STRING_FORMAT_DATE) const;

    operator ostring() const;
	Date& operator ++ (int);  
	Date& operator -- (int);  
	Date& operator = (const Date& other);
	Date& operator + (int val);
	Date& operator - (int val);
	Date& operator += (int val);
	Date& operator -= (int val);
	bool operator == (const Date& other) const;
	bool operator != (const Date& other) const;			
	bool operator > (const Date& other) const;
	bool operator < (const Date& other) const;
	bool operator >= (const Date& other) const;
	bool operator <= (const Date& other) const;

private:

    Date(OCI_Date *pDate, Handle *parent = 0);
};

/**
 * @class Interval
 *
 * @brief
 * Object identififying the SQL datatype INTERVAL.
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
    int Compare(const Interval& other) const;

    unsigned int GetType() const;

    void Add(const Interval& other);
    void Substract(const Interval& other);

    bool IsValid() const;

    int GetYear() const;
    void SetYear(int value);
 
    int GetMonth() const;
    void SetMonth(int value);

    int GetDay() const;
    void SetDay(int value);
 
    int GetHours() const;
    void SetHours(int value);
  
    int GetMinutes() const;
    void SetMinutes(int value);

    int GetSeconds() const;
    void SetSeconds(int value);

    int GetMilliSeconds() const;
    void SetMilliSeconds(int value);

    void GetDaySecond(int *day, int *hour, int *min, int *sec, int *fsec) const;
    void SetDaySecond(int day, int hour, int min, int sec, int fsec);

    void GetYearMonth(int *year, int *month) const;
    void SetYearMonth(int year, int month);

    void FromTimeZone(ostring timeZone);

    void FromString(ostring data);
    ostring ToString(int leadingPrecision = 10, int fractionPrecision = 10) const;

    operator ostring() const;

private:

    Interval(OCI_Interval *pInterval, Handle *parent = 0);
};

/**
 * @class Timestamp
 *
 * @brief
 * Object identififying the SQL datatype TIMESTAMP.
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
    int Compare(const Timestamp& other) const;

    unsigned int GetType() const;

    void Construct(int year, int month, int day, int hour, int min, int sec, int fsec, ostring timeZone = OTEXT(""));
    void Convert(const Timestamp& other);

    bool IsValid() const;

    int GetYear() const;
    void SetYear(int value);
 
    int GetMonth() const;
    void SetMonth(int value);

    int GetDay() const;
    void SetDay(int value);
 
    int GetHours() const;
    void SetHours(int value);
  
    int GetMinutes() const;
    void SetMinutes(int value);

    int GetSeconds() const;
    void SetSeconds(int value);

    int GetMilliSeconds() const;
    void SetMilliSeconds(int value);

    void GetDate(int *year, int *month, int *day) const;
    void GetTime(int *hour, int *min,   int *sec, int *fsec) const;
    void GetDateTime(int *year, int *month, int *day, int *hour, int *min, int *sec, int *fsec) const;

    ostring GetTimeZone() const;
    void GetTimeZoneOffset(int *hour, int *min) const;

    void AddInterval(const Interval& other);
    void SubstractInterval(const Interval& other);

    void Substract(const Timestamp &other, Interval &result);

    void SysTimestamp();

    void FromString(ostring data, ostring format = OCI_STRING_FORMAT_DATE);
    ostring ToString(ostring format = OCI_STRING_FORMAT_DATE, int precision = 0) const;

    operator ostring() const;

private:

    Timestamp(OCI_Timestamp *pTimestamp, Handle *parent = 0);
};

/**
 * @class Clob
 *
 * @brief
 * Object identififying the SQL datatype CLOB.
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

    ostring Read(unsigned int size);
    unsigned int Write(ostring content);
    unsigned int Append(ostring content);
    bool Seek(unsigned int seekMode, big_uint offset);

    void Append(const Clob &other);
    void Assign(const Clob &other);
    bool Equals(const Clob &other) const;

    big_uint GetOffset() const;
    big_uint GetLength() const;
    big_uint GetMaxSize() const;
    big_uint GetChunkSize() const;

    void Truncate(big_uint size);
    big_uint Erase(big_uint offset, big_uint size);
    void Copy(Clob &dest, big_uint offset, big_uint offsetDest, big_uint size) const;

    bool IsTemporary() const;

    void Open(unsigned int mode);
    void Flush();
    void Close();

    void EnableBuffering(bool value);

    operator ostring() const;
	Clob& operator = (const Clob& other);
	Clob& operator + (const Clob& other);
	bool operator == (const Clob& other) const;
	bool operator != (const Clob& other) const;			

private:

    Clob(OCI_Lob *pLob, Handle *parent = 0);
};

/**
 * @class Blob
 *
 * @brief
 * Object identififying the SQL datatype BLOB.
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
    bool Equals(const Blob &other) const;

    big_uint GetOffset() const;
    big_uint GetLength() const;
    big_uint GetMaxSize() const;
    big_uint GetChunkSize() const;

    void Truncate(big_uint size);
    big_uint Erase(big_uint offset, big_uint size);
    void Copy(Blob &dest, big_uint offset, big_uint offsetDest, big_uint size) const;

    bool IsTemporary() const;

    void Open(unsigned int mode);
    void Flush();
    void Close();

    void EnableBuffering(bool value);

	Blob& operator = (const Blob& other);
	Blob& operator + (const Blob& other);
	bool operator == (const Blob& other) const;
	bool operator != (const Blob& other) const;	

private:

    Blob(OCI_Lob *pLob, Handle *parent = 0);
};

/**
 * @class File
 *
 * @brief
 * Object identififying the SQL datatype BFILE.
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
    File(const Connection &connection, ostring directory, ostring name);

    unsigned int Read(void *buffer, unsigned int size);
    bool Seek(unsigned int seekMode, big_uint offset);

    void Assign(const File &other);
    bool Equals(const File &other) const;

    bool Exists() const;

    big_uint GetOffset() const;
    big_uint GetSize() const;

    void SetInfos(ostring directory, ostring name);

    ostring GetName() const;
    ostring GetDirectory() const;

    void Open();
    void Close();
    bool IsOpened() const;

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

    TypeInfo(const Connection &connection, ostring name, unsigned int type);

    unsigned int GetType() const;
    ostring GetName() const;
    Connection GetConnection() const;

    unsigned int GetColumnCount() const;
    Column GetColumn(unsigned int index) const;

private:

    TypeInfo(OCI_TypeInfo *pTypeInfo);
};

/**
 * @class Object
 *
 * @brief
 * Object identififying the SQL datatype OBJECT.
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

    bool IsAttributeNull(ostring name) const;
    void SetAttributeNull(ostring name);

    TypeInfo GetTypeInfo() const;
    Reference GetReference() const;

    template<class TDataType>
    TDataType Get(ostring name) const;

    template<class TDataType>
    void Set(ostring name, const TDataType &value);

    template<class TDataType>
    void Get(ostring name, TDataType value, unsigned int &size) const;

    template<class TDataType>
    void Set(ostring name, const TDataType &value, unsigned int size);

    ostring ToString() const;
    operator ostring() const;

private:

    Object(OCI_Object *pObject, Handle *parent = 0);
};

/**
 * @class Reference
 *
 * @brief
 * Object identififying the SQL datatype REF.
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

    TypeInfo GetTypeInfo() const;
    Object GetObject() const;

    void Assign(const Reference& other);

    bool IsReferenceNull() const;
    void SetReferenceNull();

    ostring ToString() const;
    operator ostring() const;

private:

    Reference(OCI_Ref *pRef, Handle *parent = 0);
};

/**
 * @class Collection
 *
 * @brief
 * Object identififying the SQL datatype VARRAY and NESTED TABLE.
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

    unsigned int GetType() const;
    unsigned int GetMax() const;
    unsigned int GetSize() const;
    unsigned int GetCount() const;

    void Truncate(unsigned int size);
    void Clear();

    bool IsElementNull(unsigned int index) const;
    void SetElementNull(unsigned int index);

    bool Delete(unsigned int index) const;

    template <class TDataType>
    TDataType Get(unsigned int index) const;

    template <class TDataType>
    void Set(unsigned int index, const TDataType &data);

    template<class TDataType>
    void Get(unsigned int index, TDataType value, unsigned int &size) const;

    template<class TDataType>
    void Set(unsigned int index, const TDataType value, unsigned int size);

    template <class TDataType>
    void Append(const TDataType &data);

    template <class TDataType>
    void Append(const TDataType &data, unsigned int size);

    TypeInfo GetTypeInfo() const;

    ostring ToString() const;
    operator ostring() const;

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
    TDataType Get() const;

    template <class TDataType>
    void Set(TDataType &value);

    bool Next();
    bool Prev();

    bool IsElementNull() const;
    void SetElementNull();

    bool operator ++ (int);
    bool operator -- (int value);
};

/**
 * @class CLong
 *
 * @brief
 * Object identififying the SQL datatype LONG.
 *
 */
class CLong : public HandleHolder<OCI_Long *>
{
    friend class Statement;
    friend class Resultset;
    friend class BindArray;

public:

    CLong(const Statement &statement);

    ostring Read(unsigned int size);
    unsigned int Write(ostring content);

    unsigned int GetSize() const;
    ostring GetContent() const;

private:

    CLong(OCI_Long *pLong, Handle *parent = 0);
};

/**
 * @class CLong
 *
 * @brief
 * Object identififying the SQL datatype LONG RAW
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

    unsigned int GetSize() const;
    void * GetContent() const;

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

    ostring GetName() const;
    unsigned int GetType() const;
    unsigned int GetSubType() const;
    unsigned int GetElemcount() const;

    Statement GetStatement() const;

    void SetNull(bool value);
    void SetNull(bool value, unsigned int pos);

    bool IsNull() const;
    bool IsNull(unsigned int pos) const;

    void SetCharsetForm(unsigned int value);

    void SetDirection(unsigned int value);
    unsigned int GetDirection() const;

private:

    BindInfo(OCI_Bind *pBind, Handle *parent);
};

/**
 * @class Statement
 *
 * @brief
 * Object used for executing SQL or PL/SQL statement and returning the produced results.
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

    Connection GetConnection() const;

    void Describe(ostring sql);
    void Parse(ostring sql);
    void Prepare(ostring sql);
    void Execute();
    void Execute(ostring sql);

    unsigned int GetAffectedRows() const;

    ostring GetSql()  const;

    Resultset GetResultset();
    Resultset GetNextResultset();

    void SetBindArraySize(unsigned int size);
    unsigned int GetBindArraySize() const;

    void AllowRebinding(bool value);

    unsigned int GetBindCount() const;

    BindInfo GetBind(unsigned int index) const;
    BindInfo GetBind(ostring name) const;

    template <class TDataType>
    void Bind(ostring name, TDataType &value, unsigned int mode);

    template <class TDataType, class TExtraInfo>
    void Bind(ostring name, TDataType &value, TExtraInfo extraInfo, unsigned int mode);

    template <class TDataType>
    void Bind(ostring name, std::vector<TDataType> &values, unsigned int mode);

    template <class TDataType, class TExtraInfo>
    void Bind(ostring name, std::vector<TDataType> &values, TExtraInfo extraInfo, unsigned int mode);

    template <class TDataType, class TExtraInfo>
    void Bind(ostring name, std::vector<TDataType> &values, TExtraInfo &extraInfo, unsigned int mode);

    template <class TDataType>
    void Register(ostring name);

    template <class TDataType, class TExtraInfo>
    void Register(ostring name, TExtraInfo extraInfo);

    template <class TDataType, class TExtraInfo>
    void Register(ostring name, TExtraInfo &extraInfo);

    unsigned int GetStatementType() const;

    unsigned int GetSqlErrorPos() const;

    void SetFetchMode(unsigned int value);
    unsigned int GetFetchMode() const;

    void SetBindMode(unsigned int value);
    unsigned int GetBindMode() const;

    void SetFetchSize(unsigned int value);
    unsigned int GetFetchSize() const;

    void SetPrefetchSize(unsigned int value);
    unsigned int GetPrefetchSize() const;

    void SetPrefetchMemory(unsigned int value);
    unsigned int GetPrefetchMemory() const;

    void SetLongMaxSize(unsigned int value);
    unsigned int GetLongMaxSize() const;

    void SetLongMode(unsigned int value);
    unsigned int GetLongMode() const;

    unsigned int GetSQLCommand() const;
    ostring GetSQLVerb() const;

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
    void Bind (TBindMethod &method, ostring name, TDataType& value, unsigned int mode);

    template <typename TBindMethod, class TObjectType, class TDataType>
    void Bind (TBindMethod &method, ostring name, TObjectType &value, BindValue<TDataType> datatype, unsigned int mode);

    template <typename TBindMethod, class TObjectType, class TDataType>
    void Bind (TBindMethod &method, ostring name, std::vector<TObjectType> &values, BindValue<TDataType> datatype, unsigned int mode);

    template <typename TBindMethod, class TObjectType, class TDataType, class TElemType>
    void Bind (TBindMethod &method, ostring name, std::vector<TObjectType> &values, BindValue<TDataType> datatype, unsigned int mode, TElemType type);
};

/**
 * @class Resultset
 *
 * @brief
 * Database result set
 */
class Resultset : public HandleHolder<OCI_Resultset *>
{
    friend class Statement;
public:

    template<class TDataType>
    TDataType Get(unsigned int index) const;

    template<class TDataType>
    TDataType Get(ostring name) const;

    template<class TDataType>
    void Get(unsigned int index, TDataType value, unsigned int &size) const;

    template<class TDataType>
    void Get(ostring name, TDataType value, unsigned int &size) const;

    bool Next();
    bool Prev();
    bool First();
    bool Last();
    bool Seek(unsigned int mode, int offset);

    unsigned int GetCount() const;
    unsigned int GetCurrentRow() const;

    unsigned int GetColumnCount() const;
    Column GetColumn(unsigned int index) const;
    Column GetColumn(ostring name) const;

    bool IsColumnNull(unsigned int index) const;
    bool IsColumnNull(ostring name) const;

    Statement GetStatement() const;

    bool operator ++ (int);
    bool operator -- (int value);

    bool operator += (int);
    bool operator -= (int value);

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

    ostring GetName() const;
    ostring GetSQLType() const;
    ostring GetFullSQLType() const;

    unsigned int GetType() const;
    unsigned int GetSubType() const;
    unsigned int GetCharsetForm() const;
    unsigned int GetSize() const;

    int GetScale() const;
    int GetPrecision() const;
    int GetFractionalPrecision() const;
    int GetLeadingPrecision() const;

    bool GetNullable() const;
    bool GetCharUsed() const;

    TypeInfo GetTypeInfo() const;

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

    void Register(const Connection &con, ostring name, unsigned int type, NotifyHandlerProc handler, unsigned int port = 0, unsigned int timeout = 0);
    void Unregister();

    void Watch(ostring sql);

    ostring GetName() const;
    unsigned int GetTimeout() const;
    unsigned int GetPort() const;

    Connection GetConnection() const;

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

    unsigned int GetType() const;
    unsigned int GetOperation() const;
    ostring GetDatabaseName() const;
    ostring GetObjectName() const;
    ostring GetRowID() const;
    Subscription GetSubscription() const;

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

    Agent(const Connection &connection, ostring name = OTEXT(""), ostring address = OTEXT(""));

    ostring GetName() const;
    void SetName(ostring value);

    ostring GetAddress() const;
    void SetAddress(ostring value);

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

    Date GetEnqueueTime() const;
    int GetAttemptCount() const;
    unsigned int GetState() const;
    void GetID(BufferPointer value, unsigned int &size) const;

    int GetExpiration() const;
    void SetExpiration(int value);

    int GetEnqueueDelay() const;
    void SetEnqueueDelay(int value);

    int GetPriority() const;
    void SetPriority(int value);

    void GetOriginalID(BufferPointer value, unsigned int &size) const;
    void SetOriginalID(const BufferPointer &value, unsigned int size);

    ostring GetCorrelation() const;
    void SetCorrelation(ostring value);

    ostring GetExceptionQueue() const;
    void SetExceptionQueue(ostring value);

    Agent GetSender() const;;
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

    Enqueue(const TypeInfo &typeInfo, ostring queueName);

    void Put(const Message &message);

    unsigned int GetVisibility() const;
    void SetVisibility(unsigned int value);

    unsigned int GetSequenceDeviation() const;
    void SetSequenceDeviation(unsigned int value);

    void GetRelativeMsgID(BufferPointer value, unsigned int &size) const;
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

    Dequeue(const TypeInfo &typeInfo, ostring queueName);

    Message Get();

    Agent Listen(int timeout);

    ostring GetConsumer() const;
    void SetConsumer(ostring value);

    ostring GetCorrelation() const;
    void SetCorrelation(ostring value);

    void GetRelativeMsgID(BufferPointer value, unsigned int &size) const;
    void SetRelativeMsgID(const BufferPointer &value, unsigned int size);

    unsigned int GetVisibility() const;
    void SetVisibility(unsigned int value);

    unsigned int GetMode() const;
    void SetMode(unsigned int value);

    unsigned int GetNavigation() const;
    void SetNavigation(unsigned int value);

    int GetWaitTime() const;
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

    static void Create(const Connection &connection, ostring queue, ostring table, unsigned int queueType = OCI_AQT_NORMAL, unsigned int maxRetries = 0,
                       unsigned int retryDelay = 0, unsigned int retentionTime = 0, bool dependencyTracking = false, ostring comment = OTEXT(""));
    static void Alter (const Connection &connection, ostring queue, unsigned int maxRetries= 0, unsigned int retryDelay= 0, unsigned int retentionTime= 0, ostring comment = OTEXT(""));
    static void Drop  (const Connection &connection, ostring queue);
    static void Start (const Connection &connection, ostring queue, bool startEnqueue = true, bool startDequeue = true);
    static void Stop  (const Connection &connection, ostring queue, bool stopEnqueue = true, bool stopDequeue = true, bool wait = true);
};

/**
 * @class QueueTable
 *
 * @brief
 * Static class using for manipulating AQ Tables
 *
 */
class QueueTable
{
public:

    static void Create (const Connection &connection, ostring table, ostring payloadType, bool multipleConsumers, ostring storageClause = OTEXT(""), ostring sortList = OTEXT(""),
                        unsigned int messageGrouping = OCI_AGM_NONE, ostring comment = OTEXT(""), unsigned int primaryInstance = 0, unsigned int secondaryInstance = 0, ostring compatible = OTEXT(""));
    static void Alter  (const Connection &connection, ostring table, ostring comment, unsigned int primaryInstance = 0, unsigned int secondaryInstance = 0);
    static void Drop   (const Connection &connection, ostring table, bool force = true);
    static void Purge  (const Connection &connection, ostring table,  unsigned int deliveryMode, ostring purgeCondition = OTEXT(""), bool block = true);
    static void Migrate(const Connection &connection, ostring table, ostring compatible = OTEXT(""));
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

    DirectPath(const TypeInfo &typeInfo, unsigned int nbCols, unsigned int  nbRows, ostring partition = OTEXT(""));

    void SetColumn(unsigned int colIndex, ostring name, unsigned int maxSize,  ostring format = OTEXT(""));
    void SetEntry(unsigned int rowIndex, unsigned int colIndex,  const ostring &value,  bool complete = true);
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
    unsigned int GetCurrentRows() const;

    unsigned int GetMaxRows() const;
    unsigned int GetRowCount() const;
    unsigned int GetAffectedRows() const;

    void SetDateFormat(ostring format);

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


#include "ocilib_impl.hpp"

}

#endif

