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

#include <algorithm>

namespace ocilib
{

/* ********************************************************************************************* *
 *                                         IMPLEMENTATION
 * ********************************************************************************************* */

/**
*
* @brief
* Class handling LONG oracle type
*
* @note
* Length and size arguments / returned values are expressed in number of characters
*
*/
typedef Long<ostring, LongCharacter> Clong;

/**
*
* @brief
* Class handling LONG RAW oracle type
*
* @note
* Length and size arguments / returned values are expressed in number of bytes
*
*/
typedef Long<Raw, LongBinary> Blong;

/**
*
* @brief Resolve a bind input / output types
*
*/
template<class I, class O>
struct BindResolverType
{
    typedef I InputType;
    typedef O OutputType;
};

/**
*
* @brief Simplified resolver for scalar types that do not need translation
*
*/
template<class T>
struct BindResolverScalarType : BindResolverType<T, T> {};

template<> struct BindResolver<bool> : BindResolverType<bool, boolean>{};
template<> struct BindResolver<short> : BindResolverScalarType<short>{};
template<> struct BindResolver<unsigned short> : BindResolverScalarType<unsigned short>{};
template<> struct BindResolver<int> : BindResolverScalarType<int>{};
template<> struct BindResolver<unsigned int> : BindResolverScalarType<unsigned  int>{};
template<> struct BindResolver<big_int> : BindResolverScalarType<big_int>{};
template<> struct BindResolver<big_uint> : BindResolverScalarType<big_uint>{};
template<> struct BindResolver<float> : BindResolverScalarType<float>{};
template<> struct BindResolver<double> : BindResolverScalarType<double>{};
template<> struct BindResolver<ostring> : BindResolverType<ostring, otext>{};
template<> struct BindResolver<Raw> : BindResolverType<ostring, unsigned char>{};
template<> struct BindResolver<Number> : BindResolverType<Number, OCI_Number*>{};
template<> struct BindResolver<Date> : BindResolverType<Date, OCI_Date*>{};
template<> struct BindResolver<Timestamp> : BindResolverType<Timestamp, OCI_Timestamp*>{};
template<> struct BindResolver<Interval> : BindResolverType<Interval, OCI_Interval*>{};
template<> struct BindResolver<Clob> : BindResolverType<Clob, OCI_Lob*>{};
template<> struct BindResolver<NClob> : BindResolverType<NClob, OCI_Lob*>{};
template<> struct BindResolver<Blob> : BindResolverType<Blob, OCI_Lob*>{};
template<> struct BindResolver<File> : BindResolverType<File, OCI_File*>{};
template<> struct BindResolver<Clong> : BindResolverType<Clong, OCI_Long*>{};
template<> struct BindResolver<Blong> : BindResolverType<Blong, OCI_Long*>{};
template<> struct BindResolver<Reference> : BindResolverType<Reference, OCI_Ref*>{};
template<> struct BindResolver<Object> : BindResolverType<Object, OCI_Object*>{};
template<> struct BindResolver<Statement> : BindResolverType<Statement, OCI_Statement*>{};

/**
* @brief Allow resolving a the C API numeric enumerated type from a C++ type
*/
template<class T> struct NumericTypeResolver{};

template<> struct NumericTypeResolver<OCI_Number*>    { enum { Value = NumericNumber }; };
template<> struct NumericTypeResolver<Number>         { enum { Value = NumericNumber }; };
template<> struct NumericTypeResolver<short>          { enum { Value = NumericShort }; };
template<> struct NumericTypeResolver<unsigned short> { enum { Value = NumericUnsignedShort }; };
template<> struct NumericTypeResolver<int>            { enum { Value = NumericInt }; };
template<> struct NumericTypeResolver<unsigned int>   { enum { Value = NumericUnsignedInt }; };
template<> struct NumericTypeResolver<big_int>        { enum { Value = NumericBigInt }; };
template<> struct NumericTypeResolver<big_uint>       { enum { Value = NumericUnsignedBigInt }; };
template<> struct NumericTypeResolver<double>         { enum { Value = NumericDouble }; };
template<> struct NumericTypeResolver<float>          { enum { Value = NumericFloat }; };

template<class T>
T Check(T result)
{
    OCI_Error *err = OCI_GetLastError();

    if (err)
    {
        throw Exception(err);
    }

    return result;
}

inline ostring MakeString(const otext *result, int size)
{
    return result ? (size >= 0 ? ostring(result, result + size) : ostring(result)) : ostring();
}

inline Raw MakeRaw(void *result, unsigned int size)
{
    unsigned char *ptr = static_cast<unsigned char *>(result);

    return (ptr && size > 0 ? Raw(ptr, ptr + size) : Raw());
}

template<class S, class C>
void ConverString(S &dest, const C *src, size_t length)
{
    size_t i = 0;

    dest.clear();

    if (src)
    {
        dest.resize(length);

        while (i < length)
        {
            dest[i] = static_cast<typename S::value_type>(src[i]);

            ++i;
        }
    }
}

/* --------------------------------------------------------------------------------------------- *
 * Enum
 * --------------------------------------------------------------------------------------------- */

template<class T>
Enum<T>::Enum() : _value(0)
{
}

template<class T>
Enum<T>::Enum(T value) : _value(value)
{
}

template<class T>
T Enum<T>::GetValue()
{
    return _value;
}

template<class T>
Enum<T>::operator T ()
{
    return GetValue();
}

template<class T>
Enum<T>::operator unsigned int () const
{
    return static_cast<unsigned int>(_value);
}

template<class T>
bool Enum<T>::operator == (const Enum& other) const
{
    return other._value == _value;
}

template<class T>
bool Enum<T>::operator != (const Enum& other) const
{
    return !(*this == other);
}

template<class T>
bool Enum<T>::operator == (const T& other) const
{
    return other == _value;
}

template<class T>
bool Enum<T>::operator != (const T& other) const
{
    return !(*this == other);
}

/* --------------------------------------------------------------------------------------------- *
 * Flags
 * --------------------------------------------------------------------------------------------- */

template<class T>
Flags<T>::Flags() : _flags(static_cast<T>(0))
{
}

template<class T>
Flags<T>::Flags(T flag) : _flags( flag)
{
}

template<class T>
Flags<T>::Flags(const Flags& other) : _flags(other._flags)
{
}

template<class T>
Flags<T>::Flags(unsigned int flag) : _flags(static_cast<T>(flag))
{
}

template<class T>
Flags<T> Flags<T>::operator~ () const
{
    return Flags<T>(~_flags);
}

template<class T>
Flags<T> Flags<T>::operator | (const Flags& other) const
{
    return Flags<T>(_flags | other._flags);
}

template<class T>
Flags<T> Flags<T>::operator & (const Flags& other) const
{
    return Flags<T>(_flags & other._flags);
}

template<class T>
Flags<T> Flags<T>::operator ^ (const Flags& other) const
{
    return Flags<T>(_flags ^ other._flags);
}

template<class T>
Flags<T> Flags<T>::operator | (T other) const
{
    return Flags<T>(_flags | other);
}

template<class T>
Flags<T> Flags<T>::operator & (T other) const
{
    return Flags<T>(_flags & other);
}

template<class T>
Flags<T> Flags<T>::operator ^ (T other) const
{
    return Flags<T>(_flags ^ other);
}

template<class T>
Flags<T>& Flags<T>::operator |= (const Flags<T>& other)
{
    _flags |= other._flags;
    return *this;
}

template<class T>
Flags<T>& Flags<T>::operator &= (const Flags<T>& other)
{
    _flags &= other._flags;
    return *this;
}

template<class T>
Flags<T>& Flags<T>::operator ^= (const Flags<T>& other)
{
    _flags ^= other._flags;
    return *this;
}

template<class T>
Flags<T>& Flags<T>::operator |= (T other)
{
    _flags |= other;
    return *this;
}

template<class T>
Flags<T>& Flags<T>::operator &= (T other)
{
    _flags &= other;
    return *this;
}

template<class T>
Flags<T>& Flags<T>::operator ^= (T other)
{
    _flags ^= other;
    return *this;
}

template<class T>
bool Flags<T>::operator == (T other) const
{
    return _flags == static_cast<unsigned int>(other);
}

template<class T>
bool Flags<T>::operator == (const Flags& other) const
{
    return _flags == other._flags;
}

template<class T>
bool Flags<T>::IsSet(T other) const
{
    return ((_flags & other) == _flags);
}

template<class T>
unsigned int Flags<T>::GetValues() const
{
    return _flags;
}

#define OCI_DEFINE_FLAG_OPERATORS(T) \
inline Flags<T> operator | (T a, T b) { return Flags<T>(a) | Flags<T>(b); } \

OCI_DEFINE_FLAG_OPERATORS(Environment::EnvironmentFlagsValues)
OCI_DEFINE_FLAG_OPERATORS(Environment::SessionFlagsValues)
OCI_DEFINE_FLAG_OPERATORS(Environment::StartFlagsValues)
OCI_DEFINE_FLAG_OPERATORS(Environment::StartModeValues)
OCI_DEFINE_FLAG_OPERATORS(Environment::ShutdownModeValues)
OCI_DEFINE_FLAG_OPERATORS(Environment::ShutdownFlagsValues)
OCI_DEFINE_FLAG_OPERATORS(Environment::AllocatedBytesValues)
OCI_DEFINE_FLAG_OPERATORS(Transaction::TransactionFlagsValues)
OCI_DEFINE_FLAG_OPERATORS(Column::PropertyFlagsValues)
OCI_DEFINE_FLAG_OPERATORS(Subscription::ChangeTypesValues)

/* --------------------------------------------------------------------------------------------- *
 * ManagedBuffer
 * --------------------------------------------------------------------------------------------- */

template<typename T>
ManagedBuffer<T>::ManagedBuffer() : _buffer(nullptr), _size(0)
{
}

template<typename T>
ManagedBuffer<T>::ManagedBuffer(T *buffer, size_t size) : _buffer(buffer), _size(size)
{
}

template<typename T>
ManagedBuffer<T>::ManagedBuffer(size_t size) : _buffer(new T[size]), _size(size)
{
    memset(_buffer, 0, sizeof(T) * _size);
}
template<typename T>
ManagedBuffer<T>::~ManagedBuffer()
{
    delete [] _buffer;
}

template<typename T>
ManagedBuffer<T>::operator T* () const
{
    return  _buffer;
}

template<typename T>
ManagedBuffer<T>::operator const T* () const
{
    return  _buffer;
}

/* --------------------------------------------------------------------------------------------- *
 * Handle
 * --------------------------------------------------------------------------------------------- */

template<class T>
HandleHolder<T>::HandleHolder() : _smartHandle(nullptr)
{
}

template<class T>
HandleHolder<T>::HandleHolder(const HandleHolder &other) : _smartHandle(nullptr)
{
    Acquire(other, nullptr, nullptr, other._smartHandle ? other._smartHandle->GetParent() : nullptr);
}

template<class T>
HandleHolder<T>::~HandleHolder()
{
    Release();
}

template<class T>
HandleHolder<T>& HandleHolder<T>::operator = (const HandleHolder<T> &other)
{
    Acquire(other, nullptr, nullptr, other._smartHandle ? other._smartHandle->GetParent() : nullptr);
    return *this;
}

template<class T>
bool HandleHolder<T>::IsNull() const
{
    return (static_cast<T>(*this) == 0);
}

template<class T>
HandleHolder<T>::operator T()
{
    return _smartHandle ? _smartHandle->GetHandle() : nullptr;
}

template<class T>
HandleHolder<T>::operator T() const
{
    return _smartHandle ? _smartHandle->GetHandle() : nullptr;
}

template<class T>
HandleHolder<T>::operator bool()
{
    return !IsNull();
}

template<class T>
HandleHolder<T>::operator bool() const
{
    return !IsNull();
}

template<class T>
Handle * HandleHolder<T>::GetHandle() const
{
    return static_cast<Handle *>(_smartHandle);
}

template<class T>
void HandleHolder<T>::Acquire(T handle, HandleFreeFunc handleFreefunc, SmartHandleFreeNotifyFunc freeNotifyFunc, Handle *parent)
{
    Release();

    if (handle)
    {
        _smartHandle = Environment::GetSmartHandle<SmartHandle*>(handle);

        if (!_smartHandle)
        {
            _smartHandle = new SmartHandle(this, handle, handleFreefunc, freeNotifyFunc, parent);
        }
        else
        {
            _smartHandle->Acquire(this);
        }
    }
}

template<class T>
void HandleHolder<T>::Acquire(HandleHolder<T> &other)
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

template<class T>
void HandleHolder<T>::Release()
{
    if (_smartHandle)
    {
        _smartHandle->Release(this);
    }

    _smartHandle = nullptr;
}

inline Locker::Locker() : _mutex(nullptr)
{
    SetAccessMode(false);
}

inline Locker::~Locker()
{
    SetAccessMode(false);
}

inline void Locker::SetAccessMode(bool threaded)
{
    if (threaded && !_mutex)
    {
        _mutex = Mutex::Create();
    }
    else if (!threaded && _mutex)
    {
        Mutex::Destroy(_mutex);
        _mutex = nullptr;
    }
}

inline void Locker::Lock() const
{
    if (_mutex)
    {
        Mutex::Acquire(_mutex);
    }
}

inline void Locker::Unlock() const
{
    if (_mutex)
    {
        Mutex::Release(_mutex);
    }
}

inline Lockable::Lockable() : _locker(nullptr)
{

}

inline Lockable::~Lockable()
{

}

inline void Lockable::Lock() const
{
    if (_locker)
    {
        _locker->Lock();
    }
}

inline void Lockable::Unlock() const
{
    if (_locker)
    {
        _locker->Unlock();
    }
}

inline void Lockable::SetLocker(Locker *locker)
{
    _locker = locker;
}

template<class K, class V>
ConcurrentMap<K, V>::ConcurrentMap()
{

}

template<class K, class V>
ConcurrentMap<K, V>::~ConcurrentMap()
{
    Clear();
}

template<class K, class V>
void ConcurrentMap<K, V>::Remove(K key)
{
    Lock();
    _map.erase(key);
    Unlock();
}

template<class K, class V>
V ConcurrentMap<K, V>::Get(K key)
{
    V value = 0;

    Lock();
    typename std::map< K, V >::const_iterator it = _map.find(key);
    if (it != _map.end())
    {
        value = it->second;
    }
    Unlock();

    return value;
}

template<class K, class V>
void ConcurrentMap<K, V>::Set(K key, V value)
{
    Lock();
    _map[key] = value;
    Unlock();
}

template<class K, class V>
void ConcurrentMap<K, V>::Clear()
{
    Lock();
    _map.clear();
    Unlock();
}

template<class K, class V>
size_t ConcurrentMap<K, V>::GetSize()
{
    Lock();
    size_t size = _map.size();
    Unlock();

    return size;
}

template<class T>
ConcurrentList<T>::ConcurrentList()
{

}

template<class T>
ConcurrentList<T>::~ConcurrentList()
{
    Clear();
}

template<class T>
void ConcurrentList<T>::Add(T value)
{
    Lock();
    _list.push_back(value);
    Unlock();
}

template<class T>
void ConcurrentList<T>::Remove(T value)
{
    Lock();
    _list.remove(value);
    Unlock();
}

template<class T>
void ConcurrentList<T>::Clear()
{
    Lock();
    _list.clear();
    Unlock();
}

template<class T>
size_t ConcurrentList<T>::GetSize()
{
    Lock();
    size_t size = _list.size();
    Unlock();

    return size;
}

template<class T>
bool ConcurrentList<T>::Exists(const T &value)
{
    Lock();

	bool res = std::find(_list.begin(), _list.end(), value) != _list.end();

    Unlock();

    return res;
}

template<class T>
template<class P>
bool ConcurrentList<T>::FindIf(P predicate, T &value)
{
    bool res = false;
   
	Lock();

	typename std::list<T>::iterator it = std::find_if(_list.begin(), _list.end(), predicate);
	
	if (it != _list.end())
	{
        value = *it;
        res = true;
    }

    Unlock();

    return res;
}

template<class T>
template<class A>
void ConcurrentList<T>::ForEach(A action)
{
    Lock();

	std::for_each(_list.begin(), _list.end(), action);

    Unlock();
}

template<class T>
HandleHolder<T>::SmartHandle::SmartHandle
(
    HandleHolder *holder, T handle, HandleFreeFunc handleFreefunc,
    SmartHandleFreeNotifyFunc freeNotifyFunc, Handle *parent
)
    : _holders(), _children(), _locker(), _handle(handle), _handleFreeFunc(handleFreefunc),
      _freeNotifyFunc(freeNotifyFunc), _parent(parent), _extraInfo(nullptr)
{
    _locker.SetAccessMode((Environment::GetMode() & Environment::Threaded) == Environment::Threaded);

    _holders.SetLocker(&_locker);
    _children.SetLocker(&_locker);

    Environment::SetSmartHandle<SmartHandle*>(handle, this);

    Acquire(holder);

    if (_parent && _handle)
    {
        _parent->GetChildren().Add(this);
    }
}

template<class T>
HandleHolder<T>::SmartHandle::~SmartHandle()
{
    boolean ret = TRUE;
    boolean chk = FALSE;

    if (_parent && _handle)
    {
        _parent->GetChildren().Remove(this);
    }

    _children.ForEach(DeleteHandle);
    _children.Clear();

    _holders.SetLocker(nullptr);
    _children.SetLocker(nullptr);

    Environment::SetSmartHandle<SmartHandle*>(_handle, nullptr);

    if (_freeNotifyFunc)
    {
        _freeNotifyFunc(this);
    }

    if (_handleFreeFunc && _handle)
    {
        ret = _handleFreeFunc(_handle);
        chk = TRUE;
    }

    if (chk)
    {
        Check(ret);
    }
}

template<class T>
void HandleHolder<T>::SmartHandle::DeleteHandle(Handle *handle)
{
    if (handle)
    {
        handle->DetachFromParent();
        handle->DetachFromHolders();

        delete handle;
    }
}

template<class T>
void HandleHolder<T>::SmartHandle::ResetHolder(HandleHolder *holder)
{
    if (holder)
    {
        holder->_smartHandle = 0;
    }
}

template<class T>
void HandleHolder<T>::SmartHandle::Acquire(HandleHolder *holder)
{
    _holders.Add(holder);
}

template<class T>
void HandleHolder<T>::SmartHandle::Release(HandleHolder *holder)
{
    _holders.Remove(holder);

    if (_holders.GetSize() == 0)
    {
        delete this;
    }

    holder->_smartHandle = nullptr;
}

template<class T>
T HandleHolder<T>::SmartHandle::GetHandle() const
{
    return _handle;
}

template<class T>
Handle * HandleHolder<T>::SmartHandle::GetParent() const
{
    return _parent;
}

template<class T>
AnyPointer HandleHolder<T>::SmartHandle::GetExtraInfos() const
{
    return _extraInfo;
}

template<class T>
void HandleHolder<T>::SmartHandle::SetExtraInfos(AnyPointer extraInfo)
{
    _extraInfo = extraInfo;
}

template<class T>
ConcurrentList<Handle *> & HandleHolder<T>::SmartHandle::GetChildren()
{
    return _children;
}

template<class T>
void HandleHolder<T>::SmartHandle::DetachFromHolders()
{
    _holders.ForEach(ResetHolder);
    _holders.Clear();
}

template<class T>
void HandleHolder<T>::SmartHandle::DetachFromParent()
{
    _parent = nullptr;
}

/* --------------------------------------------------------------------------------------------- *
 * Exception
 * --------------------------------------------------------------------------------------------- */

inline Exception::Exception()
    : _what(),
    _pStatement(nullptr),
    _pConnnection(nullptr),
    _row(0),
    _type(static_cast<ExceptionType::Type>(0)),
    _errLib(0),
    _errOracle(0)
{

}

inline Exception::~Exception() throw ()
{

}

inline Exception::Exception(OCI_Error *err)
    : _what(),
    _pStatement(OCI_ErrorGetStatement(err)),
    _pConnnection(OCI_ErrorGetConnection(err)),
    _row(OCI_ErrorGetRow(err)),
    _type(static_cast<ExceptionType::Type>(OCI_ErrorGetType(err))),
    _errLib(OCI_ErrorGetInternalCode(err)),
    _errOracle(OCI_ErrorGetOCICode(err))
{
    const otext *str = OCI_ErrorGetString(err);

    if (str)
    {
        ConverString(_what, str, ostrlen(str));
    }
}

inline const char * Exception::what() const throw()
{
    return _what.c_str();
}

inline ostring Exception::GetMessage() const
{
    ostring message;

#ifdef OCI_CHARSET_WIDE

    ConverString(message, _what.c_str(), _what.size());

#else

    message = _what;

#endif

    return message;
}

inline Exception::ExceptionType Exception::GetType() const
{
    return _type;
}

inline int Exception::GetOracleErrorCode() const
{
    return _errOracle;
}

inline int Exception::GetInternalErrorCode() const
{
    return _errLib;
}

inline Statement Exception::GetStatement() const
{
    return Statement(_pStatement, nullptr);
}

inline Connection Exception::GetConnection() const
{
    return Connection(_pConnnection, nullptr);
}

inline unsigned int Exception::GetRow() const
{
    return _row;
}

/* --------------------------------------------------------------------------------------------- *
 * Environment
 * --------------------------------------------------------------------------------------------- */

inline void Environment::Initialize(EnvironmentFlags mode, const ostring& libpath)
{
    GetInstance().SelfInitialize(mode, libpath);
}

inline void Environment::Cleanup()
{
    GetInstance().SelfCleanup();
}

inline Environment::EnvironmentFlags Environment::GetMode()
{
    return GetInstance()._mode;
}

inline Environment::ImportMode Environment::GetImportMode()
{
    return ImportMode(static_cast<ImportMode::Type>(Check(OCI_GetImportMode())));
}

inline Environment::CharsetMode Environment::GetCharset()
{
    return CharsetMode(static_cast<CharsetMode::Type>(Check(OCI_GetCharset())));
}

inline big_uint Environment::GetAllocatedBytes(AllocatedBytesFlags type)
{
    return Check(OCI_GetAllocatedBytes(type.GetValues()));
}

inline bool Environment::Initialized()
{
    return GetInstance()._initialized;
}

inline OracleVersion Environment::GetCompileVersion()
{
    return OracleVersion(static_cast<OracleVersion::Type>(Check(OCI_GetOCICompileVersion())));
}

inline OracleVersion Environment::GetRuntimeVersion()
{
    return  OracleVersion(static_cast<OracleVersion::Type>(Check(OCI_GetOCIRuntimeVersion())));
}

inline unsigned int Environment::GetCompileMajorVersion()
{
    return OCI_VER_MAJ(Check(OCI_GetOCICompileVersion()));
}

inline unsigned int Environment::GetCompileMinorVersion()
{
    return OCI_VER_MIN(Check(OCI_GetOCICompileVersion()));
}

inline unsigned int Environment::GetCompileRevisionVersion()
{
    return OCI_VER_REV(Check(OCI_GetOCICompileVersion()));
}

inline unsigned int Environment::GetRuntimeMajorVersion()
{
    return OCI_VER_MAJ(Check(OCI_GetOCIRuntimeVersion()));
}

inline unsigned int Environment::GetRuntimeMinorVersion()
{
    return OCI_VER_MIN(Check(OCI_GetOCIRuntimeVersion()));
}

inline unsigned int Environment::GetRuntimeRevisionVersion()
{
    return OCI_VER_REV(Check(OCI_GetOCIRuntimeVersion()));
}

inline void Environment::EnableWarnings(bool value)
{
    OCI_EnableWarnings(static_cast<boolean>(value));
}

inline bool Environment::SetFormat(FormatType formatType, const ostring& format)
{
    return Check(OCI_SetFormat(nullptr, formatType, format.c_str()) == TRUE);
}

inline ostring Environment::GetFormat(FormatType formatType)
{
    return MakeString(Check(OCI_GetFormat(nullptr, formatType)));
}

inline void Environment::StartDatabase(const ostring& db, const ostring& user, const ostring &pwd, Environment::StartFlags startFlags,
                                       Environment::StartMode startMode, Environment::SessionFlags sessionFlags, const ostring& spfile)
{
    Check(OCI_DatabaseStartup(db.c_str(), user.c_str(), pwd.c_str(), sessionFlags.GetValues(),
                              startMode.GetValues(), startFlags.GetValues(), spfile.c_str() ));
}

inline void Environment::ShutdownDatabase(const ostring& db, const ostring& user, const ostring &pwd, Environment::ShutdownFlags shutdownFlags,
                                       Environment::ShutdownMode shutdownMode, Environment::SessionFlags sessionFlags)
{
   Check(OCI_DatabaseShutdown(db.c_str(), user.c_str(), pwd.c_str(), sessionFlags.GetValues(),
                              shutdownMode.GetValues(),  shutdownFlags.GetValues() ));
}

inline void Environment::ChangeUserPassword(const ostring& db, const ostring& user, const ostring& pwd, const ostring& newPwd)
{
    Check(OCI_SetUserPassword(db.c_str(), user.c_str(), pwd.c_str(), newPwd.c_str()));
}

inline void Environment::SetHAHandler(HAHandlerProc handler)
{
    Check(OCI_SetHAHandler(static_cast<POCI_HA_HANDLER>(handler != nullptr ? Environment::HAHandler : nullptr)));

    Environment::SetUserCallback<HAHandlerProc>(GetEnvironmentHandle(), handler);
}

inline void Environment::HAHandler(OCI_Connection *pConnection, unsigned int source, unsigned int event, OCI_Timestamp  *pTimestamp)
{
    HAHandlerProc handler = Environment::GetUserCallback<HAHandlerProc>(GetEnvironmentHandle());

    if (handler)
    {
        Connection connection(pConnection, nullptr);
        Timestamp timestamp(pTimestamp, connection.GetHandle());

        handler(connection,
                HAEventSource(static_cast<HAEventSource::Type>(source)),
                HAEventType  (static_cast<HAEventType::Type>  (event)),
                timestamp);
    }
}

inline unsigned int Environment::TAFHandler(OCI_Connection *pConnection, unsigned int type, unsigned int event)
{
    unsigned int res = OCI_FOC_OK;

    Connection::TAFHandlerProc handler = Environment::GetUserCallback<Connection::TAFHandlerProc>(Check(pConnection));

    if (handler)
    {
        Connection connection(pConnection, nullptr);

        res = handler(connection,
                      Connection::FailoverRequest( static_cast<Connection::FailoverRequest::Type> (type)),
                      Connection::FailoverEvent  ( static_cast<Connection::FailoverEvent::Type>   (event)));
    }

    return res;
}

inline void Environment::NotifyHandler(OCI_Event *pEvent)
{
    Subscription::NotifyHandlerProc handler = Environment::GetUserCallback<Subscription::NotifyHandlerProc>((Check(OCI_EventGetSubscription(pEvent))));

    if (handler)
    {
        Event evt(pEvent);
        handler(evt);
    }
}

inline void Environment::NotifyHandlerAQ(OCI_Dequeue *pDequeue)
{
    Dequeue::NotifyAQHandlerProc handler = Environment::GetUserCallback<Dequeue::NotifyAQHandlerProc>(Check(pDequeue));

    if (handler)
    {
        Dequeue dequeue(pDequeue);
        handler(dequeue);
    }
}

template<class T>
T Environment::GetUserCallback(AnyPointer ptr)
{
    return reinterpret_cast<T>(GetInstance()._callbacks.Get(ptr));
}

template<class T>
void Environment::SetUserCallback(AnyPointer ptr, T callback)
{
    if (callback)
    {
        GetInstance()._callbacks.Set(ptr, reinterpret_cast<CallbackPointer>(callback));
    }
    else
    {
        GetInstance()._callbacks.Remove(ptr);
    }
}

template<class T>
void Environment::SetSmartHandle(AnyPointer ptr, T handle)
{
    if (handle)
    {
        GetInstance()._handles.Set(ptr, handle);
    }
    else
    {
        GetInstance()._handles.Remove(ptr);
    }
}

template<class T>
T Environment::GetSmartHandle(AnyPointer ptr)
{
    return dynamic_cast<T>(GetInstance()._handles.Get(ptr));
}

inline Handle * Environment::GetEnvironmentHandle()
{
    return GetInstance()._handle.GetHandle();
}

inline Environment& Environment::GetInstance()
{
    static Environment envHandle;

    return envHandle;
}

inline Environment::Environment() : _locker(), _handle(), _handles(), _callbacks(), _mode(), _initialized(false)
{

}

inline void Environment::SelfInitialize(EnvironmentFlags mode, const ostring& libpath)
{
    _mode = mode;

    Check(OCI_Initialize(nullptr, libpath.c_str(), _mode.GetValues() | OCI_ENV_CONTEXT));

    _initialized = true;

    _locker.SetAccessMode((_mode & Environment::Threaded) == Environment::Threaded);

    _callbacks.SetLocker(&_locker);
    _handles.SetLocker(&_locker);

    _handle.Acquire(const_cast<AnyPointer>(Check(OCI_HandleGetEnvironment())), nullptr, nullptr, nullptr);
}

inline void Environment::SelfCleanup()
{
    _locker.SetAccessMode(false);

    _callbacks.SetLocker(nullptr);
    _handles.SetLocker(nullptr);

    _handle.Release();

    if (_initialized)
    {
        Check(OCI_Cleanup());
    }

    _initialized = false;
}

/* --------------------------------------------------------------------------------------------- *
 * Mutex
 * --------------------------------------------------------------------------------------------- */

inline MutexHandle Mutex::Create()
{
    return Environment::GetInstance().Initialized() ? Check(OCI_MutexCreate()) : nullptr;
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

inline void Thread::Run(ThreadHandle handle, ThreadProc func, AnyPointer args)
{
    Check(OCI_ThreadRun(handle, func, args));
}

inline void Thread::Join(ThreadHandle handle)
{
    Check(OCI_ThreadJoin(handle));
}

inline ThreadId Thread::GetThreadId(ThreadHandle handle)
{
    return Check(OCI_HandleGetThreadID(handle));
}

/* --------------------------------------------------------------------------------------------- *
 * ThreadKey
 * --------------------------------------------------------------------------------------------- */

inline void ThreadKey::Create(const ostring& name, ThreadKeyFreeProc freeProc)
{
    Check(OCI_ThreadKeyCreate(name.c_str(), freeProc));
}

inline void ThreadKey::SetValue(const ostring& name, AnyPointer value)
{
    Check(OCI_ThreadKeySetValue(name.c_str(), value));
}

inline AnyPointer ThreadKey::GetValue(const ostring& name)
{
    return Check(OCI_ThreadKeyGetValue(name.c_str()));
}

/* --------------------------------------------------------------------------------------------- *
 * Pool
 * --------------------------------------------------------------------------------------------- */

inline Pool::Pool()
{

}

inline Pool::Pool(const ostring& db, const ostring& user, const ostring& pwd, Pool::PoolType poolType,
        unsigned int minSize, unsigned int maxSize, unsigned int increment, Environment::SessionFlags sessionFlags)
{
    Open(db, user, pwd, poolType, minSize, maxSize, increment, sessionFlags);
}

inline void Pool::Open(const ostring& db, const ostring& user, const ostring& pwd, Pool::PoolType poolType,
            unsigned int minSize, unsigned int maxSize, unsigned int increment, Environment::SessionFlags sessionFlags)
{
    Release();

    Acquire(Check(OCI_PoolCreate(db.c_str(), user.c_str(), pwd.c_str(), poolType, sessionFlags.GetValues(),
        minSize, maxSize, increment)), reinterpret_cast<HandleFreeFunc>(OCI_PoolFree), nullptr, Environment::GetEnvironmentHandle());
}

inline void Pool::Close()
{
    Release();
}

inline Connection Pool::GetConnection(const ostring& sessionTag)
{
    return Connection(Check( OCI_PoolGetConnection(*this, sessionTag.c_str())), GetHandle());
}

inline unsigned int Pool::GetTimeout() const
{
    return Check( OCI_PoolGetTimeout(*this));
}

inline void Pool::SetTimeout(unsigned int value)
{
    Check( OCI_PoolSetTimeout(*this, value));
}

inline bool Pool::GetNoWait() const
{
    return (Check( OCI_PoolGetNoWait(*this)) == TRUE);
}

inline void Pool::SetNoWait(bool value)
{
    Check( OCI_PoolSetNoWait(*this, value));
}

inline unsigned int Pool::GetBusyConnectionsCount() const
{
    return Check( OCI_PoolGetBusyCount(*this));
}

inline unsigned int Pool::GetOpenedConnectionsCount() const
{
    return Check( OCI_PoolGetOpenedCount(*this));
}

inline unsigned int Pool::GetMinSize() const
{
    return Check( OCI_PoolGetMin(*this));
}

inline unsigned int Pool::GetMaxSize() const
{
    return Check( OCI_PoolGetMax(*this));
}

inline unsigned int Pool::GetIncrement() const
{
    return Check( OCI_PoolGetIncrement(*this));
}

inline unsigned int Pool::GetStatementCacheSize() const
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

inline Connection::Connection(const ostring& db, const ostring& user, const ostring& pwd, Environment::SessionFlags sessionFlags)
{
    Open(db, user, pwd, sessionFlags);
}

inline Connection::Connection(OCI_Connection *con,  Handle *parent)
{
    Acquire(con, reinterpret_cast<HandleFreeFunc>(parent ? OCI_ConnectionFree : nullptr), nullptr, parent);
}

inline void Connection::Open(const ostring& db, const ostring& user, const ostring& pwd, Environment::SessionFlags sessionFlags)
{
    Acquire(Check(OCI_ConnectionCreate(db.c_str(), user.c_str(), pwd.c_str(), sessionFlags.GetValues())),
        reinterpret_cast<HandleFreeFunc>(OCI_ConnectionFree), nullptr, Environment::GetEnvironmentHandle());
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

inline bool Connection::GetAutoCommit() const
{
    return (Check(OCI_GetAutoCommit(*this)) == TRUE);
}

inline bool Connection::IsServerAlive() const
{
    return (Check(OCI_IsConnected(*this)) == TRUE);
}

inline bool Connection::PingServer() const
{
    return( Check(OCI_Ping(*this)) == TRUE);
}

inline ostring Connection::GetConnectionString() const
{
    return MakeString(Check(OCI_GetDatabase(*this)));
}

inline ostring Connection::GetUserName() const
{
    return MakeString(Check(OCI_GetUserName(*this)));
}

inline ostring Connection::GetPassword() const
{
    return MakeString(Check(OCI_GetPassword(*this)));
}

inline OracleVersion Connection::GetVersion() const
{
    return OracleVersion(static_cast<OracleVersion::Type>(Check(OCI_GetVersionConnection(*this))));
}

inline ostring Connection::GetServerVersion() const
{
    return MakeString(Check( OCI_GetVersionServer(*this)));
}

inline unsigned int Connection::GetServerMajorVersion() const
{
    return Check(OCI_GetServerMajorVersion(*this));
}

inline unsigned int Connection::GetServerMinorVersion() const
{
    return Check(OCI_GetServerMinorVersion(*this));
}

inline unsigned int Connection::GetServerRevisionVersion() const
{
    return Check(OCI_GetServerRevisionVersion(*this));
}

inline void Connection::ChangePassword(const ostring& newPwd)
{
    Check(OCI_SetPassword(*this, newPwd.c_str()));
}

inline ostring Connection::GetSessionTag() const
{
    return MakeString(Check(OCI_GetSessionTag(*this)));
}

inline void Connection::SetSessionTag(const ostring& tag)
{
    Check(OCI_SetSessionTag(*this, tag.c_str()));
}

inline Transaction Connection::GetTransaction() const
{
    return Transaction(Check(OCI_GetTransaction(*this)));
}

inline void Connection::SetTransaction(const Transaction &transaction)
{
    Check(OCI_SetTransaction(*this, transaction));
}

inline bool Connection::SetFormat(FormatType formatType, const ostring& format)
{
    return Check(OCI_SetFormat(*this, formatType, format.c_str()) == TRUE);
}

inline ostring Connection::GetFormat(FormatType formatType)
{
    return MakeString(Check(OCI_GetFormat(*this, formatType)));
}

inline void Connection::EnableServerOutput(unsigned int bufsize, unsigned int arrsize, unsigned int lnsize)
{
    Check(OCI_ServerEnableOutput(*this, bufsize, arrsize, lnsize));
}

inline void Connection::DisableServerOutput()
{
    Check(OCI_ServerDisableOutput(*this));
}

inline bool Connection::GetServerOutput(ostring &line) const
{
    const otext * str = Check(OCI_ServerGetOutput(*this));

    line = MakeString(str);

    return (str != nullptr);
}

inline void Connection::GetServerOutput(std::vector<ostring> &lines) const
{
    const otext * str = Check(OCI_ServerGetOutput(*this));

    while (str)
    {
        lines.push_back(str);
        str = Check(OCI_ServerGetOutput(*this));
    }
}

inline void Connection::SetTrace(SessionTrace trace, const ostring& value)
{
    Check(OCI_SetTrace(*this, trace, value.c_str()));
}

inline ostring Connection::GetTrace(SessionTrace trace) const
{
    return MakeString(Check(OCI_GetTrace(*this, trace)));
}

inline ostring Connection::GetDatabase() const
{
    return MakeString(Check(OCI_GetDBName(*this)));
}

inline ostring Connection::GetInstance() const
{
    return Check(OCI_GetInstanceName(*this));
}

inline ostring Connection::GetService() const
{
    return MakeString(Check(OCI_GetServiceName(*this)));
}

inline ostring Connection::GetServer() const
{
    return Check(OCI_GetServerName(*this));
}

inline ostring Connection::GetDomain() const
{
    return MakeString(Check(OCI_GetDomainName(*this)));
}

inline Timestamp Connection::GetInstanceStartTime() const
{
    return Timestamp(Check(OCI_GetInstanceStartTime(*this)), GetHandle());
}

inline unsigned int Connection::GetStatementCacheSize() const
{
    return Check(OCI_GetStatementCacheSize(*this));
}

inline void Connection::SetStatementCacheSize(unsigned int value)
{
    Check(OCI_SetStatementCacheSize(*this, value));
}

inline unsigned int Connection::GetDefaultLobPrefetchSize() const
{
    return Check(OCI_GetDefaultLobPrefetchSize(*this));
}

inline void Connection::SetDefaultLobPrefetchSize(unsigned int value)
{
    Check(OCI_SetDefaultLobPrefetchSize(*this, value));
}

inline unsigned int  Connection::GetMaxCursors() const
{
    return Check(OCI_GetMaxCursors(*this));
}

inline bool Connection::IsTAFCapable() const
{
    return (Check(OCI_IsTAFCapable(*this)) == TRUE);
}

inline void Connection::SetTAFHandler(TAFHandlerProc handler)
{
    Check(OCI_SetTAFHandler(*this, static_cast<POCI_TAF_HANDLER>(handler != nullptr ? Environment::TAFHandler : nullptr)));

    Environment::SetUserCallback<Connection::TAFHandlerProc>(static_cast<OCI_Connection*>(*this), handler);
}

inline void* Connection::GetUserData()
{
    return Check(OCI_GetUserData(*this));
}

inline void Connection::SetUserData(AnyPointer value)
{
    Check(OCI_SetUserData(*this, value));
}

/* --------------------------------------------------------------------------------------------- *
 * Transaction
 * --------------------------------------------------------------------------------------------- */

inline Transaction::Transaction(const Connection &connection, unsigned int timeout, TransactionFlags flags, OCI_XID *pxid)
{
    Acquire(Check(OCI_TransactionCreate(connection, timeout, flags.GetValues(), pxid)), reinterpret_cast<HandleFreeFunc>(OCI_TransactionFree), nullptr, nullptr);
}

inline Transaction::Transaction(OCI_Transaction *trans)
{
    Acquire(trans, nullptr, nullptr, nullptr);
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

inline Transaction::TransactionFlags Transaction::GetFlags() const
{
    return TransactionFlags(static_cast<TransactionFlags::Type>(Check(OCI_TransactionGetMode(*this))));
}

inline unsigned int Transaction::GetTimeout() const
{
    return Check(OCI_TransactionGetTimeout(*this));
}

/* --------------------------------------------------------------------------------------------- *
* Number
* --------------------------------------------------------------------------------------------- */

inline Number::Number(bool create)
{
    if (create)
    {
        Allocate();
    }
}

inline Number::Number(OCI_Number *pNumber, Handle *parent)
{
    Acquire(pNumber, nullptr, nullptr, parent);
}

inline Number::Number(const ostring& str, const ostring& format)
{
    Allocate();

    FromString(str, format);
}

inline Number::Number(const otext* str, const otext* format)
{
    Allocate();

    FromString(str, format);
}

inline void Number::Allocate()
{
    Acquire(Check(OCI_NumberCreate(nullptr)), reinterpret_cast<HandleFreeFunc>(OCI_NumberFree), nullptr, nullptr);
}

inline void Number::FromString(const ostring& str, const ostring& format) const
{
    Check(OCI_NumberFromText(*this, str.c_str(), format.size() > 0 ? format.c_str() : Environment::GetFormat(FormatNumeric).c_str()));
}

inline ostring Number::ToString(const ostring& format) const
{
    if (!IsNull())
    {
        size_t size = OCI_SIZE_BUFFER;

        ManagedBuffer<otext> buffer(size + 1);

        Check(OCI_NumberToText(*this, format.c_str(), static_cast<int>(size), buffer));

        return MakeString(static_cast<const otext *>(buffer));
    }

    return OCI_STRING_NULL;
}

inline ostring Number::ToString() const
{
    return ToString(Environment::GetFormat(FormatNumeric));
}

inline Number Number::Clone() const
{
    Number result;

    result.Allocate();

    Check(OCI_NumberAssign(result, *this));

    return result;
}

inline int Number::Compare(const Number& other) const
{
    return Check(OCI_NumberCompare(*this, other));
}

template<class T>
T Number::GetValue() const
{
    T value;

    Check(OCI_NumberGetValue(*this, NumericTypeResolver<T>::Value, &value));

    return value;
}

template<class T>
Number& Number::SetValue(const T &value)
{
    if (IsNull())
    {
        Allocate();
    }

    Check(OCI_NumberSetValue(*this, NumericTypeResolver<T>::Value, reinterpret_cast<void*>(const_cast<T*>(&value))));

    return *this;
}

template<class T>
void Number::Add(const T &value)
{
    Check(OCI_NumberAdd(*this, NumericTypeResolver<T>::Value, reinterpret_cast<void*>(const_cast<T*>(&value))));
}

template<class T>
void Number::Sub(const T &value)
{
    Check(OCI_NumberSub(*this, NumericTypeResolver<T>::Value, reinterpret_cast<void*>(const_cast<T*>(&value))));
}

template<class T>
void Number::Multiply(const T &value)
{
    Check(OCI_NumberMultiply(*this, NumericTypeResolver<T>::Value, reinterpret_cast<void*>(const_cast<T*>(&value))));
}

template<class T>
void Number::Divide(const T &value)
{
    Check(OCI_NumberDivide(*this, NumericTypeResolver<T>::Value, reinterpret_cast<void*>(const_cast<T*>(&value))));
}

inline Number& Number::operator = (OCI_Number * &lhs)
{
    Acquire(lhs, reinterpret_cast<HandleFreeFunc>(OCI_NumberFree), nullptr, nullptr);
    return *this;
}

template<class T>
Number& Number::operator = (const T &lhs)
{
    SetValue<T>(lhs);
    return *this;
}

template<class T>
Number::operator T() const
{
    return GetValue<T>();
}

template<class T>
Number Number::operator + (const T &value)
{
    Number result = Clone();
    result.Add(value);
    return result;
}

template<class T>
Number Number::operator - (const T &value)
{
    Number result = Clone();
    result.Sub(value);
    return result;
}

template<class T>
Number Number::operator * (const T &value)
{
    Number result = Clone();
    result.Multiply(value);
    return result;
}

template<class T>
Number Number::operator / (const T &value)
{
    Number result = Clone();
    result.Divide(value);
    return result;
}

template<class T>
Number& Number::operator += (const T &value)
{
    Add<T>(value);
    return *this;
}

template<class T>
Number& Number::operator -= (const T &value)
{
    Sub<T>(value);
    return *this;
}

template<class T>
Number& Number::operator *= (const T &value)
{
    Multiply<T>(value);
    return *this;
}

template<class T>
Number& Number::operator /= (const T &value)
{
    Divide<T>(value);
    return *this;
}

inline Number& Number::operator ++ ()
{
    return *this += 1;
}

inline Number& Number::operator -- ()
{
    return *this += 1;
}

inline Number Number::operator ++ (int)
{
    return *this + 1;
}

inline Number Number::operator -- (int)
{
    return *this - 1;
}

inline bool Number::operator == (const Number& other) const
{
    return Compare(other) == 0;
}

inline bool Number::operator != (const Number& other) const
{
    return !(*this == other);
}

inline bool Number::operator > (const Number& other) const
{
    return Compare(other) > 0;
}

inline bool Number::operator < (const Number& other) const
{
    return Compare(other) < 0;
}

inline bool Number::operator >= (const Number& other) const
{
    int res = Compare(other);

    return res == 0 || res < 0;
}

inline bool Number::operator <= (const Number& other) const
{
    int res = Compare(other);

    return res == 0 || res > 0;
}

/* --------------------------------------------------------------------------------------------- *
 * Date
 * --------------------------------------------------------------------------------------------- */

inline Date::Date(bool create)
{
    if (create)
    {
        Allocate();
    }
}

inline Date::Date(const ostring& str, const ostring& format)
{
    Allocate();

    FromString(str, format);
}

inline Date::Date(const otext* str, const otext* format)
{
    Allocate();

    FromString(str, format);
}

inline Date::Date(OCI_Date *pDate, Handle *parent)
{
    Acquire(pDate, nullptr, nullptr, parent);
}

inline void Date::Allocate()
{
    Acquire(Check(OCI_DateCreate(nullptr)), reinterpret_cast<HandleFreeFunc>(OCI_DateFree), nullptr, nullptr);
}

inline Date Date::SysDate()
{
    Date result;

    result.Allocate();

    Check(OCI_DateSysDate(result));

    return result;
}

inline Date Date::Clone() const
{
    Date result;

    result.Allocate();

    Check(OCI_DateAssign(result, *this));

    return result;
}

inline int Date::Compare(const Date& other) const
{
    return Check(OCI_DateCompare(*this, other));
}

inline bool Date::IsValid() const
{
    return (Check(OCI_DateCheck(*this)) == 0);
}

inline int Date::GetYear() const
{
    int year = 0, month = 0, day = 0;

    GetDate(year, month, day);

    return year;
}

inline void Date::SetYear(int value)
{
    int year = 0, month = 0, day = 0;

    GetDate(year, month, day);
    SetDate(value, month, day);
}

inline int Date::GetMonth() const
{
    int year = 0, month = 0, day = 0;

    GetDate(year, month, day);

    return month;
}

inline void Date::SetMonth(int value)
{
    int year = 0, month = 0, day = 0;

    GetDate(year, month, day);
    SetDate(year, value, day);
}

inline int Date::GetDay() const
{
    int year = 0, month = 0, day = 0;

    GetDate(year, month, day);

    return day;
}

inline void Date::SetDay(int value)
{
    int year = 0, month = 0, day = 0;

    GetDate(year, month, day);
    SetDate(year, month, value);
}

inline int Date::GetHours() const
{
    int hour = 0, minutes = 0, seconds = 0;

    GetTime(hour, minutes, seconds);

    return hour;
}

inline void Date::SetHours(int value)
{
    int hour = 0, minutes = 0, seconds = 0;

    GetTime(hour, minutes, seconds);
    SetTime(value, minutes, seconds);
}

inline int Date::GetMinutes() const
{
    int hour = 0, minutes = 0, seconds = 0;

    GetTime(hour, minutes, seconds);

    return minutes;
}

inline void Date::SetMinutes(int value)
{
    int hour = 0, minutes = 0, seconds = 0;

    GetTime(hour, minutes, seconds);
    SetTime(hour, value, seconds);
}

inline int Date::GetSeconds() const
{
    int hour = 0, minutes = 0, seconds = 0;

    GetTime(hour, minutes, seconds);

    return seconds;
}

inline void Date::SetSeconds(int value)
{
    int hour = 0, minutes = 0, seconds = 0;

    GetTime(hour, minutes, seconds);
    SetTime(hour, minutes, value);
}

inline int Date::DaysBetween(const Date& other) const
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

inline void Date::GetDate(int &year, int &month, int &day) const
{
    Check(OCI_DateGetDate(*this, &year, &month, &day));
}

inline void Date::GetTime(int &hour, int &min, int &sec) const
{
    Check(OCI_DateGetTime(*this, &hour, &min , &sec));
}

inline void Date::GetDateTime(int &year, int &month, int &day, int &hour, int &min, int &sec) const
{
    Check(OCI_DateGetDateTime(*this, &year, &month, &day, &hour, &min , &sec));
}

inline void Date::AddDays(int days)
{
    Check(OCI_DateAddDays(*this, days));
}

inline void Date::AddMonths(int months)
{
    OCI_DateAddMonths(*this, months);
}

inline Date Date::NextDay(const ostring& day) const
{
    Date result = Clone();

    Check(OCI_DateNextDay(result, day.c_str()));

    return result;
}

inline Date Date::LastDay() const
{
    Date result = Clone();

    Check(OCI_DateLastDay(result));

    return result;
}

inline void Date::ChangeTimeZone(const ostring& tzSrc, const ostring& tzDst)
{
    Check(OCI_DateZoneToZone(*this, tzSrc.c_str(), tzDst.c_str()));
}

inline void Date::FromString(const ostring& str, const ostring& format)
{
    Check(OCI_DateFromText(*this, str.c_str(), format.size() > 0 ? format.c_str() : Environment::GetFormat(FormatDate).c_str()));
}

inline ostring Date::ToString(const ostring& format) const
{
    if (!IsNull())
    {
        size_t size = OCI_SIZE_BUFFER;

        ManagedBuffer<otext> buffer(size + 1);

        Check(OCI_DateToText(*this, format.c_str(), static_cast<int>(size), buffer));

        return MakeString(static_cast<const otext *>(buffer));
    }

    return OCI_STRING_NULL;
}

inline ostring Date::ToString() const
{
    return ToString(Environment::GetFormat(FormatDate));
}

inline Date& Date::operator ++ ()
{
    return *this += 1;
}

inline Date Date::operator ++ (int)
{
    Date result = Clone();

    *this += 1;

    return result;
}

inline Date& Date::operator -- ()
{
    return *this -= 1;
}

inline Date Date::operator -- (int)
{
    Date result = Clone();

    *this -= 1;

    return result;
}

inline Date Date::operator + (int value) const
{
    Date result = Clone();
    return result += value;
}

inline Date Date::operator - (int value) const
{
    Date result = Clone();
    return result -= value;
}

inline Date& Date::operator += (int value)
{
    AddDays(value);
    return *this;
}

inline Date& Date::operator -= (int value)
{
    AddDays(-value);
    return *this;
}

inline bool Date::operator == (const Date& other) const
{
    return Compare(other) == 0;
}

inline bool Date::operator != (const Date& other) const
{
    return !(*this == other);
}

inline bool Date::operator > (const Date& other) const
{
    return Compare(other) > 0;
}

inline bool Date::operator < (const Date& other) const
{
    return Compare(other) < 0;
}

inline bool Date::operator >= (const Date& other) const
{
    int res = Compare(other);

    return res == 0 || res > 0;
}

inline bool Date::operator <= (const Date& other) const
{
    int res = Compare(other);

    return res == 0 || res < 0;
}

/* --------------------------------------------------------------------------------------------- *
 * Interval
 * --------------------------------------------------------------------------------------------- */

inline Interval::Interval()
{
}

inline Interval::Interval(IntervalType type)
{
    Acquire(Check(OCI_IntervalCreate(nullptr, type)), reinterpret_cast<HandleFreeFunc>(OCI_IntervalFree), nullptr, nullptr);
}

inline Interval::Interval(IntervalType type, const ostring& data)
{
    Acquire(Check(OCI_IntervalCreate(nullptr, type)), reinterpret_cast<HandleFreeFunc>(OCI_IntervalFree), nullptr, nullptr);

    FromString(data);
}

inline Interval::Interval(OCI_Interval *pInterval, Handle *parent)
{
    Acquire(pInterval, nullptr, nullptr, parent);
}

inline Interval Interval::Clone() const
{
    Interval result(GetType());

    Check(OCI_IntervalAssign(result, *this));

    return result;
}

inline int Interval::Compare(const Interval& other) const
{
    return Check(OCI_IntervalCompare(*this, other));
}

inline Interval::IntervalType Interval::GetType() const
{
    return IntervalType(static_cast<IntervalType::Type>(Check(OCI_IntervalGetType(*this))));
}

inline bool Interval::IsValid() const
{
    return (Check(OCI_IntervalCheck(*this)) == 0);
}

inline int Interval::GetYear() const
{
    int year = 0, month = 0;

    GetYearMonth(year, month);

    return year;
}

inline void Interval::SetYear(int value)
{
    int year = 0, month = 0;

    GetYearMonth(year, month);
    SetYearMonth(value, month);
}

inline int Interval::GetMonth() const
{
    int year = 0, month = 0;

    GetYearMonth(year, month);

    return month;
}

inline void Interval::SetMonth(int value)
{
    int year = 0, month = 0;

    GetYearMonth(year, month);
    SetYearMonth(year, value);
}

inline int Interval::GetDay() const
{
    int day = 0, hour = 0, minutes = 0, seconds = 0, milliseconds = 0;

    GetDaySecond(day, hour, minutes, seconds, milliseconds);

    return day;
}

inline void Interval::SetDay(int value)
{
    int day = 0, hour = 0, minutes = 0, seconds = 0, milliseconds = 0;

    GetDaySecond(day, hour, minutes, seconds, milliseconds);
    SetDaySecond(value, hour, minutes, seconds, milliseconds);
}

inline int Interval::GetHours() const
{
    int day = 0, hour = 0, minutes = 0, seconds = 0, milliseconds = 0;

    GetDaySecond(day, hour, minutes, seconds, milliseconds);

    return hour;
}

inline void Interval::SetHours(int value)
{
    int day = 0, hour = 0, minutes = 0, seconds = 0, milliseconds = 0;

    GetDaySecond(day, hour, minutes, seconds, milliseconds);
    SetDaySecond(day, value, minutes, seconds, milliseconds);
}

inline int Interval::GetMinutes() const
{
    int day = 0, hour = 0, minutes = 0, seconds = 0, milliseconds = 0;

    GetDaySecond(day, hour, minutes, seconds, milliseconds);

    return minutes;
}

inline void Interval::SetMinutes(int value)
{
    int day = 0, hour = 0, minutes = 0, seconds = 0, milliseconds = 0;

    GetDaySecond(day, hour, minutes, seconds, milliseconds);
    SetDaySecond(day, hour, value, seconds, milliseconds);
}

inline int Interval::GetSeconds() const
{
    int day, hour, minutes, seconds, milliseconds;

    GetDaySecond(day, hour, minutes, seconds, milliseconds);

    return seconds;
}

inline void Interval::SetSeconds(int value)
{
    int day = 0, hour = 0, minutes = 0, seconds = 0, milliseconds = 0;

    GetDaySecond(day, hour, minutes, seconds, milliseconds);
    SetDaySecond(day, hour, minutes, value, milliseconds);
}

inline int Interval::GetMilliSeconds() const
{
    int day = 0, hour = 0, minutes = 0, seconds = 0, milliseconds = 0;

    GetDaySecond(day, hour, minutes, seconds, milliseconds);

    return milliseconds;
}

inline void Interval::SetMilliSeconds(int value)
{
    int day = 0, hour = 0, minutes = 0, seconds = 0, milliseconds = 0;

    GetDaySecond(day, hour, minutes, seconds, milliseconds);
    SetDaySecond(day, hour, minutes, seconds, value);
}

inline void Interval::GetDaySecond(int &day, int &hour, int &min, int &sec, int &fsec) const
{
    Check(OCI_IntervalGetDaySecond(*this, &day, &hour, &min, &sec, &fsec));
}

inline void Interval::SetDaySecond(int day, int hour, int min, int sec, int fsec)
{
    Check(OCI_IntervalSetDaySecond(*this, day, hour, min, sec, fsec));
}

inline void Interval::GetYearMonth(int &year, int &month) const
{
    Check(OCI_IntervalGetYearMonth(*this, &year, &month));
}
inline void Interval::SetYearMonth(int year, int month)
{
    Check(OCI_IntervalSetYearMonth(*this, year, month));
}

inline void Interval::UpdateTimeZone(const ostring& timeZone)
{
    Check(OCI_IntervalFromTimeZone(*this, timeZone.c_str()));
}

inline void Interval::FromString(const ostring& data)
{
    Check(OCI_IntervalFromText(*this, data.c_str()));
}

inline ostring Interval::ToString(int leadingPrecision, int fractionPrecision) const
{
    if (!IsNull())
    {
        size_t size = OCI_SIZE_BUFFER;

        ManagedBuffer<otext> buffer(size + 1);

        Check(OCI_IntervalToText(*this, leadingPrecision, fractionPrecision, static_cast<int>(size), buffer));

        return MakeString(static_cast<const otext *>(buffer));
    }

    return OCI_STRING_NULL;
}

inline ostring Interval::ToString() const
{
    return ToString(OCI_STRING_DEFAULT_PREC, OCI_STRING_DEFAULT_PREC);
}

inline Interval Interval::operator + (const Interval& other) const
{
    Interval result = Clone();
    return result += other;
}

inline Interval Interval::operator - (const Interval& other) const
{
    Interval result = Clone();
    return result -= other;
}

inline Interval& Interval::operator += (const Interval& other)
{
    Check(OCI_IntervalAdd(*this, other));
    return *this;
}

inline Interval& Interval::operator -= (const Interval& other)
{
    Check(OCI_IntervalSubtract(*this, other));
    return *this;
}

inline bool Interval::operator == (const Interval& other) const
{
    return Compare(other) == 0;
}

inline bool Interval::operator != (const Interval& other) const
{
    return (!(*this == other));
}

inline bool Interval::operator > (const Interval& other) const
{
    return (Compare(other) > 0);
}

inline bool Interval::operator < (const Interval& other) const
{
    return (Compare(other) < 0);
}

inline bool Interval::operator >= (const Interval& other) const
{
    int res = Compare(other);

    return (res == 0 || res < 0);
}

inline bool Interval::operator <= (const Interval& other) const
{
    int res = Compare(other);

    return (res == 0 || res > 0);
}

/* --------------------------------------------------------------------------------------------- *
 * Timestamp
 * --------------------------------------------------------------------------------------------- */

inline Timestamp::Timestamp()
{
}

inline Timestamp::Timestamp(TimestampType type)
{
    Acquire(Check(OCI_TimestampCreate(nullptr, type)), reinterpret_cast<HandleFreeFunc>(OCI_TimestampFree), nullptr, nullptr);
}

inline Timestamp::Timestamp(TimestampType type, const ostring& data, const ostring& format)
{
    Acquire(Check(OCI_TimestampCreate(nullptr, type)), reinterpret_cast<HandleFreeFunc>(OCI_TimestampFree), nullptr, nullptr);
    FromString(data, format);
}

inline Timestamp::Timestamp(OCI_Timestamp *pTimestamp, Handle *parent)
{
    Acquire(pTimestamp, nullptr, nullptr, parent);
}

inline Timestamp Timestamp::Clone() const
{
    Timestamp result(GetType());

    Check(OCI_TimestampAssign(result, *this));

    return result;
}

inline int Timestamp::Compare(const Timestamp& other) const
{
    return Check(OCI_TimestampCompare(*this, other));
}

inline Timestamp::TimestampType Timestamp::GetType() const
{
    return TimestampType(static_cast<TimestampType::Type>(Check(OCI_TimestampGetType(*this))));
}

inline void Timestamp::SetDateTime(int year, int month, int day, int hour, int min, int sec, int fsec, const ostring& timeZone)
{
    Check(OCI_TimestampConstruct(*this, year, month, day, hour, min,sec, fsec, timeZone.c_str()));
}

inline void Timestamp::Convert(const Timestamp& other)
{
    Check(OCI_TimestampConvert(*this, other));
}

inline bool Timestamp::IsValid() const
{
    return (Check(OCI_TimestampCheck(*this)) == 0);
}

inline int Timestamp::GetYear() const
{
    int year, month, day;

    GetDate(year, month, day);

    return year;
}

inline void Timestamp::SetYear(int value)
{
    int year, month, day;

    GetDate(year, month, day);
    SetDate(value, month, day);
}

inline int Timestamp::GetMonth() const
{
    int year, month, day;

    GetDate(year, month, day);

    return month;
}

inline void Timestamp::SetMonth(int value)
{
    int year, month, day;

    GetDate(year, month, day);
    SetDate(year, value, day);
}

inline int Timestamp::GetDay() const
{
    int year, month, day;

    GetDate(year, month, day);

    return day;
}

inline void Timestamp::SetDay(int value)
{
    int year, month, day;

    GetDate(year, month, day);
    SetDate(year, month, value);
}

inline int Timestamp::GetHours() const
{
    int hour, minutes, seconds, milliseconds;

    GetTime(hour, minutes, seconds, milliseconds);

    return hour;
}

inline void Timestamp::SetHours(int value)
{
    int hour, minutes, seconds, milliseconds;

    GetTime(hour, minutes, seconds, milliseconds);
    SetTime(value, minutes, seconds, milliseconds);
}

inline int Timestamp::GetMinutes() const
{
    int hour, minutes, seconds, milliseconds;

    GetTime(hour, minutes, seconds, milliseconds);

    return minutes;
}

inline void Timestamp::SetMinutes(int value)
{
    int hour = 0, minutes = 0, seconds = 0, milliseconds = 0;

    GetTime(hour, minutes, seconds, milliseconds);
    SetTime(hour, value, seconds, milliseconds);
}

inline int Timestamp::GetSeconds() const
{
    int hour = 0, minutes = 0, seconds = 0, milliseconds = 0;

    GetTime(hour, minutes, seconds, milliseconds);

    return seconds;
}

inline void Timestamp::SetSeconds(int value)
{
    int hour = 0, minutes = 0, seconds = 0, milliseconds = 0;

    GetTime(hour, minutes, seconds, milliseconds);
    SetTime(hour, minutes, value, milliseconds);
}

inline int Timestamp::GetMilliSeconds() const
{
    int hour = 0, minutes = 0, seconds = 0, milliseconds = 0;

    GetTime(hour, minutes, seconds, milliseconds);

    return milliseconds;
}

inline void Timestamp::SetMilliSeconds(int value)
{
    int hour = 0, minutes = 0, seconds = 0, milliseconds = 0;

    GetTime(hour, minutes, seconds, milliseconds);
    SetTime(hour, minutes, seconds, value);
}

inline void Timestamp::GetDate(int &year, int &month, int &day) const
{
    Check(OCI_TimestampGetDate(*this, &year, &month, &day));
}

inline void Timestamp::GetTime(int &hour, int &min, int &sec, int &fsec) const
{
    Check(OCI_TimestampGetTime(*this, &hour, &min, &sec, &fsec));
}

inline void Timestamp::GetDateTime(int &year, int &month, int &day, int &hour, int &min, int &sec, int &fsec) const
{
    Check(OCI_TimestampGetDateTime(*this, &year, &month, &day, &hour, &min, &sec, &fsec));
}

inline void Timestamp::SetDate(int year, int month, int day)
{
    int tmpYear = 0, tmpMonth = 0, tempDay = 0, hour = 0, minutes = 0, seconds = 0, milliseconds = 0;

    GetDateTime(tmpYear, tmpMonth, tempDay, hour, minutes, seconds, milliseconds);
    SetDateTime(year, month, day, hour, minutes, seconds, milliseconds);
}

inline void Timestamp::SetTime(int hour, int min, int sec, int fsec)
{
    int year = 0, month = 0, day = 0, tmpHour = 0, tmpMinutes = 0, tmpSeconds = 0, tmpMilliseconds = 0;

    GetDateTime(year, month, day, tmpHour, tmpMinutes, tmpSeconds, tmpMilliseconds);
    SetDateTime(year, month, day, hour, min, sec, fsec);
}

inline void Timestamp::SetTimeZone(const ostring& timeZone)
{
    if (GetType() == WithTimeZone)
    {
        int year = 0, month = 0, day = 0, hour = 0, minutes = 0, seconds = 0, milliseconds = 0;

        GetDateTime(year, month, day, hour, minutes, seconds, milliseconds);
        SetDateTime(year, month, day, hour, minutes, seconds, milliseconds, timeZone);
    }
}

inline ostring Timestamp::GetTimeZone() const
{
    if (GetType() != NoTimeZone)
    {
        size_t size = OCI_SIZE_BUFFER;

        ManagedBuffer<otext> buffer(size + 1);

        Check(OCI_TimestampGetTimeZoneName(*this, static_cast<int>(size), buffer) == TRUE);

        return MakeString(static_cast<const otext *>(buffer));
    }

    return ostring();
}

inline void Timestamp::GetTimeZoneOffset(int &hour, int &min) const
{
    Check(OCI_TimestampGetTimeZoneOffset(*this, &hour, &min));
}

inline void Timestamp::Substract(const Timestamp &lsh, const Timestamp &rsh, Interval& result)
{
    Check(OCI_TimestampSubtract(lsh, rsh, result));
}

inline Timestamp Timestamp::SysTimestamp(TimestampType type)
{
    Timestamp result(type);

    Check(OCI_TimestampSysTimestamp(result));

    return result;
}

inline void Timestamp::FromString(const ostring& data, const ostring& format)
{
    Check(OCI_TimestampFromText(*this, data.c_str(), format.size() > 0 ? format.c_str() : Environment::GetFormat(FormatTimestamp).c_str()));
}

inline ostring Timestamp::ToString(const ostring& format, int precision = OCI_STRING_DEFAULT_PREC) const
{
    if (!IsNull())
    {
        size_t size = OCI_SIZE_BUFFER;

        ManagedBuffer<otext> buffer(size + 1);

        Check(OCI_TimestampToText(*this, format.c_str(), static_cast<int>(size), buffer, precision));

        return MakeString(static_cast<const otext *>(buffer));
    }

    return OCI_STRING_NULL;
}

inline ostring Timestamp::ToString() const
{
    return ToString(Environment::GetFormat(FormatTimestamp), OCI_STRING_DEFAULT_PREC);
}

inline Timestamp& Timestamp::operator ++ ()
{
    return *this += 1;
}

inline Timestamp Timestamp::operator ++ (int)
{
    Timestamp result = Clone();

    *this += 1;

    return result;
}

inline Timestamp& Timestamp::operator -- ()
{
    return *this -= 1;
}

inline Timestamp Timestamp::operator -- (int)
{
    Timestamp result = Clone();

    *this -= 1;

    return result;
}

inline Timestamp Timestamp::operator + (int value) const
{
    Timestamp result = Clone();
    Interval interval(Interval::DaySecond);
    interval.SetDay(1);
    return result += value;
}

inline Timestamp Timestamp::operator - (int value) const
{
    Timestamp result = Clone();
    Interval interval(Interval::DaySecond);
    interval.SetDay(1);
    return result -= value;
}

inline Interval Timestamp::operator - (const Timestamp& other)
{
    Interval interval(Interval::DaySecond);
    Check(OCI_TimestampSubtract(*this, other, interval));
    return interval;
}

inline Timestamp Timestamp::operator + (const Interval& other) const
{
    Timestamp result = Clone();
    return result += other;
}

inline Timestamp Timestamp::operator - (const Interval& other) const
{
    Timestamp result = Clone();
    return result -= other;
}

inline Timestamp& Timestamp::operator += (const Interval& other)
{
    Check(OCI_TimestampIntervalAdd(*this, other));
    return *this;
}

inline Timestamp& Timestamp::operator -= (const Interval& other)
{
    Check(OCI_TimestampIntervalSub(*this, other));
    return *this;
}

inline Timestamp& Timestamp::operator += (int value)
{
    Interval interval(Interval::DaySecond);
    interval.SetDay(value);
    return *this += interval;
}

inline Timestamp& Timestamp::operator -= (int value)
{
    Interval interval(Interval::DaySecond);
    interval.SetDay(value);
    return *this -= interval;
}

inline bool Timestamp::operator == (const Timestamp& other) const
{
    return Compare(other) == 0;
}

inline bool Timestamp::operator != (const Timestamp& other) const
{
    return (!(*this == other));
}

inline bool Timestamp::operator > (const Timestamp& other) const
{
    return (Compare(other) > 0);
}

inline bool Timestamp::operator < (const Timestamp& other) const
{
    return (Compare(other) < 0);
}

inline bool Timestamp::operator >= (const Timestamp& other) const
{
    int res = Compare(other);

    return (res == 0 || res < 0);
}

inline bool Timestamp::operator <= (const Timestamp& other) const
{
    int res = Compare(other);

    return (res == 0 || res > 0);
}

/* --------------------------------------------------------------------------------------------- *
 * Lob
 * --------------------------------------------------------------------------------------------- */

template<class T, int U>
inline Lob<T, U>::Lob()
{
}

template<class T, int U>
Lob<T, U>::Lob(const Connection &connection)
{
    Acquire(Check(OCI_LobCreate(connection, U)), reinterpret_cast<HandleFreeFunc>(OCI_LobFree), nullptr, connection.GetHandle());
}

template<class T, int U>
Lob<T, U>::Lob(OCI_Lob *pLob, Handle *parent)
{
    Acquire(pLob, nullptr, nullptr, parent);
}

template<>
inline ostring Lob<ostring, LobCharacter>::Read(unsigned int length)
{
    ManagedBuffer<otext> buffer(length + 1);

    length = Check(OCI_LobRead(*this, static_cast<AnyPointer>(buffer), length));

    return MakeString(static_cast<const otext *>(buffer), static_cast<int>(length));
}

template<>
inline ostring Lob<ostring, LobNationalCharacter>::Read(unsigned int length)
{
    ManagedBuffer<otext> buffer(length + 1);

    length = Check(OCI_LobRead(*this, static_cast<AnyPointer>(buffer), length));

    return MakeString(static_cast<const otext *>(buffer), static_cast<int>(length));
}

template<>
inline Raw Lob<Raw, LobBinary>::Read(unsigned int length)
{
    ManagedBuffer<unsigned char> buffer(length + 1);

    length = Check(OCI_LobRead(*this, static_cast<AnyPointer>(buffer), length));

    return MakeRaw(buffer, length);
}

template<class T, int U>
unsigned int Lob<T, U>::Write(const T& content)
{
    unsigned int res = 0;

    if (content.size() > 0)
    {
        res = Check(OCI_LobWrite(*this, static_cast<AnyPointer>(const_cast<typename T::value_type *>(&content[0])), static_cast<unsigned int>(content.size())));
    }

    return res;
}

template<class T, int U>
void Lob<T, U>::Append(const Lob& other)
{
    Check(OCI_LobAppendLob(*this, other));
}

template<class T, int U>
unsigned int Lob<T, U>::Append(const T& content)
{
    unsigned int res = 0;

    if (content.size() > 0)
    {
        Check(OCI_LobAppend(*this, static_cast<AnyPointer>(const_cast<typename T::value_type *>(&content[0])), static_cast<unsigned int>(content.size())));
    }

    return res;
}

template<class T, int U>
bool Lob<T, U>::Seek(SeekMode seekMode, big_uint offset)
{
    return (Check(OCI_LobSeek(*this, offset, seekMode)) == TRUE);
}

template<class T, int U>
Lob<T, U> Lob<T, U>::Clone() const
{
    Lob result(GetConnection());

    Check(OCI_LobAssign(result, *this));

    return result;
}

template<class T, int U>
bool Lob<T, U>::Equals(const Lob &other) const
{
    return (Check(OCI_LobIsEqual(*this, other)) == TRUE);
}

template<class T, int U>
LobType Lob<T, U>::GetType() const
{
    return LobType(static_cast<LobType::Type>(Check(OCI_LobGetType(*this))));
}

template<class T, int U>
big_uint Lob<T, U>::GetOffset() const
{
    return Check(OCI_LobGetOffset(*this));
}

template<class T, int U>
big_uint Lob<T, U>::GetLength() const
{
    return Check(OCI_LobGetLength(*this));
}

template<class T, int U>
big_uint Lob<T, U>::GetMaxSize() const
{
    return Check(OCI_LobGetMaxSize(*this));
}

template<class T, int U>
big_uint Lob<T, U>::GetChunkSize() const
{
    return Check(OCI_LobGetChunkSize(*this));
}

template<class T, int U>
Connection Lob<T, U>::GetConnection() const
{
    return Connection(Check(OCI_LobGetConnection(*this)), nullptr);
}

template<class T, int U>
void Lob<T, U>::Truncate(big_uint length)
{
    Check(OCI_LobTruncate(*this, length));
}

template<class T, int U>
big_uint Lob<T, U>::Erase(big_uint offset, big_uint length)
{
    return Check(OCI_LobErase(*this, offset, length));
}

template<class T, int U>
void Lob<T, U>::Copy(Lob &dest, big_uint offset, big_uint offsetDest, big_uint size) const
{
    Check(OCI_LobCopy(dest, *this, offsetDest, offset, size));
}

template<class T, int U>
bool Lob<T, U>::IsTemporary() const
{
    return (Check(OCI_LobIsTemporary(*this)) == TRUE);
}

template<class T, int U>
void Lob<T, U>::Open(OpenMode mode)
{
    Check(OCI_LobOpen(*this, mode));
}

template<class T, int U>
void Lob<T, U>::Flush()
{
    Check(OCI_LobFlush(*this));
}

template<class T, int U>
void Lob<T, U>::Close()
{
    Check(OCI_LobClose(*this));
}

template<class T, int U>
void Lob<T, U>::EnableBuffering(bool value)
{
    Check(OCI_LobEnableBuffering(*this, value));
}

template<class T, int U>
Lob<T, U>& Lob<T, U>::operator += (const Lob<T, U>& other)
{
    Append(other);
    return *this;
}

template<class T, int U>
bool Lob<T, U>::operator == (const Lob<T, U>& other) const
{
    return Equals(other);
}

template<class T, int U>
bool Lob<T, U>::operator != (const Lob<T, U>& other) const
{
    return !(*this == other);
}

/* --------------------------------------------------------------------------------------------- *
 * File
 * --------------------------------------------------------------------------------------------- */

inline File::File()
{
}

inline File::File(const Connection &connection)
{
    Acquire(Check(OCI_FileCreate(connection, OCI_BFILE)), reinterpret_cast<HandleFreeFunc>(OCI_FileFree), nullptr, connection.GetHandle());
}

inline File::File(const Connection &connection, const ostring& directory, const ostring& name)
{
    Acquire(Check(OCI_FileCreate(connection, OCI_BFILE)), reinterpret_cast<HandleFreeFunc>(OCI_FileFree), nullptr, connection.GetHandle());

    SetInfos(directory, name);
}

inline File::File(OCI_File *pFile, Handle *parent)
{
    Acquire(pFile, nullptr, nullptr, parent);
}

inline Raw File::Read(unsigned int size)
{
    ManagedBuffer<unsigned char> buffer(size + 1);

    size = Check(OCI_FileRead(*this, static_cast<AnyPointer>(buffer), size));

    return MakeRaw(buffer, size);
}

inline bool File::Seek(SeekMode seekMode, big_uint offset)
{
    return (Check(OCI_FileSeek(*this, offset, seekMode)) == TRUE);
}

inline File File::Clone() const
{
    File result(GetConnection());

    Check(OCI_FileAssign(result, *this));

    return result;
}

inline bool File::Equals(const File &other) const
{
    return (Check(OCI_FileIsEqual(*this, other)) == TRUE);
}

inline big_uint File::GetOffset() const
{
    return Check(OCI_FileGetOffset(*this));
}

inline big_uint File::GetLength() const
{
    return Check(OCI_FileGetSize(*this));
}

inline Connection File::GetConnection() const
{
    return Connection(Check(OCI_FileGetConnection(*this)), nullptr);
}

inline bool File::Exists() const
{
    return (Check(OCI_FileExists(*this)) == TRUE);
}

inline void File::SetInfos(const ostring& directory, const ostring& name)
{
    Check(OCI_FileSetName(*this, directory.c_str(), name.c_str()));
}

inline ostring File::GetName() const
{
    return MakeString(Check(OCI_FileGetName(*this)));
}

inline ostring File::GetDirectory() const
{
    return MakeString(Check(OCI_FileGetDirectory(*this)));
}

inline void File::Open()
{
    Check(OCI_FileOpen(*this));
}

inline bool File::IsOpened() const
{
    return (Check(OCI_FileIsOpen(*this)) == TRUE);
}

inline void File::Close()
{
    Check(OCI_FileClose(*this));
}

inline bool File::operator == (const File& other) const
{
    return Equals(other);
}

inline bool File::operator != (const File& other) const
{
    return (!(*this == other));
}

/* --------------------------------------------------------------------------------------------- *
 * TypeInfo
 * --------------------------------------------------------------------------------------------- */

inline TypeInfo::TypeInfo(const Connection &connection, const ostring& name, TypeInfoType type)
{
    Acquire(Check(OCI_TypeInfoGet(connection, name.c_str(), type)), static_cast<HandleFreeFunc>(nullptr), nullptr, connection.GetHandle());
}

inline TypeInfo::TypeInfo(OCI_TypeInfo *pTypeInfo)
{
    Acquire(pTypeInfo, nullptr, nullptr, nullptr);
}

inline TypeInfo::TypeInfoType TypeInfo::GetType() const
{
    return TypeInfoType(static_cast<TypeInfoType::Type>(Check(OCI_TypeInfoGetType(*this))));
}

inline ostring TypeInfo::GetName() const
{
    return Check(OCI_TypeInfoGetName(*this));
}

inline Connection TypeInfo::GetConnection() const
{
    return Connection(Check(OCI_TypeInfoGetConnection(*this)), nullptr);
}

inline unsigned int TypeInfo::GetColumnCount() const
{
    return Check(OCI_TypeInfoGetColumnCount(*this));
}

inline Column TypeInfo::GetColumn(unsigned int index) const
{
    return Column(Check(OCI_TypeInfoGetColumn(*this, index)), GetHandle());
}

inline boolean TypeInfo::IsFinalType() const
{
    return (Check(OCI_TypeInfoIsFinalType(*this)) == TRUE);
}

inline TypeInfo TypeInfo::GetSuperType() const
{
    return TypeInfo(Check(OCI_TypeInfoGetSuperType(*this)));
}

/* --------------------------------------------------------------------------------------------- *
 * Object
 * --------------------------------------------------------------------------------------------- */

inline Object::Object()
{
}

inline Object::Object(const TypeInfo &typeInfo)
{
    Connection connection = typeInfo.GetConnection();
    Acquire(Check(OCI_ObjectCreate(connection, typeInfo)), reinterpret_cast<HandleFreeFunc>(OCI_ObjectFree), nullptr, connection.GetHandle());
}

inline Object::Object(OCI_Object *pObject, Handle *parent)
{
    Acquire(pObject, nullptr, nullptr, parent);
}

inline Object Object::Clone() const
{
    Object result(GetTypeInfo());

    Check(OCI_ObjectAssign(result, *this));

    return result;
}

inline bool Object::IsAttributeNull(const ostring& name) const
{
    return (Check(OCI_ObjectIsNull(*this, name.c_str())) == TRUE);
}

inline void Object::SetAttributeNull(const ostring& name)
{
    Check(OCI_ObjectSetNull(*this, name.c_str()));
}

inline TypeInfo Object::GetTypeInfo() const
{
    return TypeInfo(Check(OCI_ObjectGetTypeInfo(*this)));
}

inline Reference Object::GetReference() const
{
    TypeInfo typeInfo = GetTypeInfo();
    Connection connection = typeInfo.GetConnection();

    OCI_Ref *pRef = OCI_RefCreate(connection, typeInfo);

    Check(OCI_ObjectGetSelfRef(*this, pRef));

    return Reference(pRef, GetHandle());
}

inline Object::ObjectType Object::GetType() const
{
    return ObjectType(static_cast<ObjectType::Type>(Check(OCI_ObjectGetType(*this))));
}

template<>
inline bool Object::Get<bool>(const ostring& name) const
{
    return (Check(OCI_ObjectGetBoolean(*this, name.c_str())) == TRUE);
}

template<>
inline short Object::Get<short>(const ostring& name) const
{
    return Check(OCI_ObjectGetShort(*this, name.c_str()));
}

template<>
inline unsigned short Object::Get<unsigned short>(const ostring& name) const
{
    return Check(OCI_ObjectGetUnsignedShort(*this, name.c_str()));
}

template<>
inline int Object::Get<int>(const ostring& name) const
{
    return Check(OCI_ObjectGetInt(*this, name.c_str()));
}

template<>
inline unsigned int Object::Get<unsigned int>(const ostring& name) const
{
    return Check(OCI_ObjectGetUnsignedInt(*this, name.c_str()));
}

template<>
inline big_int Object::Get<big_int>(const ostring& name) const
{
    return Check(OCI_ObjectGetBigInt(*this, name.c_str()));
}

template<>
inline big_uint Object::Get<big_uint>(const ostring& name) const
{
    return Check(OCI_ObjectGetUnsignedBigInt(*this, name.c_str()));
}

template<>
inline float Object::Get<float>(const ostring& name) const
{
    return Check(OCI_ObjectGetFloat(*this, name.c_str()));
}

template<>
inline double Object::Get<double>(const ostring& name) const
{
    return Check(OCI_ObjectGetDouble(*this, name.c_str()));
}

template<>
inline Number Object::Get<Number>(const ostring& name) const
{
    return Number(Check(OCI_ObjectGetNumber(*this, name.c_str())), GetHandle());
}

template<>
inline ostring Object::Get<ostring>(const ostring& name) const
{
    return MakeString(Check(OCI_ObjectGetString(*this,name.c_str())));
}

template<>
inline Date Object::Get<Date>(const ostring& name) const
{
    return Date(Check(OCI_ObjectGetDate(*this,name.c_str())), GetHandle());
}

template<>
inline Timestamp Object::Get<Timestamp>(const ostring& name) const
{
    return Timestamp(Check(OCI_ObjectGetTimestamp(*this,name.c_str())), GetHandle());
}

template<>
inline Interval Object::Get<Interval>(const ostring& name) const
{
    return Interval(Check(OCI_ObjectGetInterval(*this,name.c_str())), GetHandle());
}

template<>
inline Object Object::Get<Object>(const ostring& name) const
{
    return Object(Check(OCI_ObjectGetObject(*this,name.c_str())), GetHandle());
}

template<>
inline Reference Object::Get<Reference>(const ostring& name) const
{
    return Reference(Check(OCI_ObjectGetRef(*this,name.c_str())), GetHandle());
}

template<>
inline Clob Object::Get<Clob>(const ostring& name) const
{
    return Clob(Check(OCI_ObjectGetLob(*this,name.c_str())), GetHandle());
}

template<>
inline NClob Object::Get<NClob>(const ostring& name) const
{
    return NClob(Check(OCI_ObjectGetLob(*this, name.c_str())), GetHandle());
}

template<>
inline Blob Object::Get<Blob>(const ostring& name) const
{
    return Blob(Check(OCI_ObjectGetLob(*this,name.c_str())), GetHandle());
}

template<>
inline File Object::Get<File>(const ostring& name) const
{
    return File(Check(OCI_ObjectGetFile(*this,name.c_str())), GetHandle());
}

template<>
inline Raw Object::Get<Raw>(const ostring& name) const
{
    unsigned int size = Check(OCI_ObjectGetRawSize(*this, name.c_str()));

    ManagedBuffer<unsigned char> buffer(size + 1);

    size = static_cast<unsigned int>(Check(OCI_ObjectGetRaw(*this, name.c_str(), static_cast<AnyPointer>(buffer), size)));

    return MakeRaw(buffer, size);
}

template<class T>
T Object::Get(const ostring& name) const
{
    return T(Check(OCI_ObjectGetColl(*this, name.c_str())), GetHandle());
}

template<>
inline void Object::Set<bool>(const ostring& name, const bool &value)
{
    Check(OCI_ObjectSetBoolean(*this, name.c_str(), static_cast<boolean>(value)));
}

template<>
inline void Object::Set<short>(const ostring& name, const short &value)
{
    Check(OCI_ObjectSetShort(*this, name.c_str(), value));
}

template<>
inline void Object::Set<unsigned short>(const ostring& name, const unsigned short &value)
{
    Check(OCI_ObjectSetUnsignedShort(*this, name.c_str(), value));
}

template<>
inline void Object::Set<int>(const ostring& name, const int &value)
{
    Check(OCI_ObjectSetInt(*this, name.c_str(), value));
}

template<>
inline void Object::Set<unsigned int>(const ostring& name, const unsigned int &value)
{
    Check(OCI_ObjectSetUnsignedInt(*this, name.c_str(), value));
}

template<>
inline void Object::Set<big_int>(const ostring& name, const big_int &value)
{
    Check(OCI_ObjectSetBigInt(*this, name.c_str(), value));
}

template<>
inline void Object::Set<big_uint>(const ostring& name, const big_uint &value)
{
    Check(OCI_ObjectSetUnsignedBigInt(*this, name.c_str(), value));
}

template<>
inline void Object::Set<float>(const ostring& name, const float &value)
{
    Check(OCI_ObjectSetFloat(*this, name.c_str(), value));
}

template<>
inline void Object::Set<double>(const ostring& name, const double &value)
{
    Check(OCI_ObjectSetDouble(*this, name.c_str(), value));
}

template<>
inline void Object::Set<Number>(const ostring& name, const Number &value)
{
    Check(OCI_ObjectSetNumber(*this, name.c_str(), value));
}

template<>
inline void Object::Set<ostring>(const ostring& name, const ostring &value)
{
    Check(OCI_ObjectSetString(*this, name.c_str(), value.c_str()));
}

template<>
inline void Object::Set<Date>(const ostring& name, const Date &value)
{
    Check(OCI_ObjectSetDate(*this, name.c_str(), value));
}

template<>
inline void Object::Set<Timestamp>(const ostring& name, const Timestamp &value)
{
    Check(OCI_ObjectSetTimestamp(*this, name.c_str(), value));
}

template<>
inline void Object::Set<Interval>(const ostring& name, const Interval &value)
{
    Check(OCI_ObjectSetInterval(*this, name.c_str(), value));
}

template<>
inline void Object::Set<Object>(const ostring& name, const Object &value)
{
    Check(OCI_ObjectSetObject(*this, name.c_str(), value));
}

template<>
inline void Object::Set<Reference>(const ostring& name, const Reference &value)
{
    Check(OCI_ObjectSetRef(*this, name.c_str(), value));
}

template<>
inline void Object::Set<Clob>(const ostring& name, const Clob &value)
{
    Check(OCI_ObjectSetLob(*this, name.c_str(), value));
}

template<>
inline void Object::Set<NClob>(const ostring& name, const NClob &value)
{
    Check(OCI_ObjectSetLob(*this, name.c_str(), value));
}

template<>
inline void Object::Set<Blob>(const ostring& name, const Blob &value)
{
    Check(OCI_ObjectSetLob(*this, name.c_str(), value));
}

template<>
inline void Object::Set<File>(const ostring& name, const File &value)
{
    Check(OCI_ObjectSetFile(*this, name.c_str(), value));
}

template<>
inline void Object::Set<Raw>(const ostring& name, const Raw &value)
{
    if (value.size() > 0)
    {
        Check(OCI_ObjectSetRaw(*this, name.c_str(), static_cast<AnyPointer>(const_cast<Raw::value_type *>(&value[0])), static_cast<unsigned int>(value.size())));
    }
    else
    {
        Check(OCI_ObjectSetRaw(*this, name.c_str(), nullptr, 0));
    }
}

template<class T>
void Object::Set(const ostring& name, const T &value)
{
    Check(OCI_ObjectSetColl(*this, name.c_str(), value));
}

inline ostring Object::ToString() const
{
    if (!IsNull())
    {
        unsigned int len = 0;

        Check(OCI_ObjectToText(*this, &len, nullptr));

        ManagedBuffer<otext> buffer(len + 1);

        Check(OCI_ObjectToText(*this, &len, buffer));

        return MakeString(static_cast<const otext *>(buffer), static_cast<int>(len));
    }

    return OCI_STRING_NULL;
}

/* --------------------------------------------------------------------------------------------- *
 * Reference
 * --------------------------------------------------------------------------------------------- */

inline Reference::Reference()
{
}

inline Reference::Reference(const TypeInfo &typeInfo)
{
    Connection connection = typeInfo.GetConnection();
    Acquire(Check(OCI_RefCreate(connection, typeInfo)), reinterpret_cast<HandleFreeFunc>(OCI_RefFree), nullptr, connection.GetHandle());
}

inline Reference::Reference(OCI_Ref *pRef, Handle *parent)
{
    Acquire(pRef, nullptr, nullptr, parent);
}

inline TypeInfo Reference::GetTypeInfo() const
{
    return TypeInfo(Check(OCI_RefGetTypeInfo(*this)));
}

inline Object Reference::GetObject() const
{
    return Object(Check(OCI_RefGetObject(*this)), GetHandle());
}

inline Reference Reference::Clone() const
{
    Reference result(GetTypeInfo());

    Check(OCI_RefAssign(result, *this));

    return result;
}

inline bool Reference::IsReferenceNull() const
{
    return (Check(OCI_RefIsNull(*this)) == TRUE);
}

inline void Reference::SetReferenceNull()
{
    Check(OCI_RefSetNull(*this));
}

inline ostring Reference::ToString() const
{
    if (!IsNull())
    {
        unsigned int size = Check(OCI_RefGetHexSize(*this));

        ManagedBuffer<otext> buffer(size + 1);

        Check(OCI_RefToText(*this, size, buffer));

        return MakeString(static_cast<const otext *>(buffer), static_cast<int>(size));
    }

    return OCI_STRING_NULL;
}

/* --------------------------------------------------------------------------------------------- *
 * Collection
 * --------------------------------------------------------------------------------------------- */

template<class T>
Collection<T>::Collection()
{
}

template<class T>
Collection<T>::Collection(const TypeInfo &typeInfo)
{
    Acquire(Check(OCI_CollCreate(typeInfo)), reinterpret_cast<HandleFreeFunc>(OCI_CollFree), nullptr, typeInfo.GetConnection().GetHandle());
}

template<class T>
Collection<T>::Collection(OCI_Coll *pColl, Handle *parent)
{
    Acquire(pColl, nullptr, nullptr, parent);
}

template<class T>
Collection<T> Collection<T>::Clone() const
{
    Collection<T> result(GetTypeInfo());

    Check(OCI_CollAssign(result, *this));

    return result;
}

template<class T>
TypeInfo Collection<T>::GetTypeInfo() const
{
    return TypeInfo(Check(OCI_CollGetTypeInfo(*this)));
}

template<class T>
typename Collection<T>::CollectionType Collection<T>::GetType() const
{
    return CollectionType(Check(OCI_CollGetType(*this)));
}

template<class T>
unsigned int Collection<T>::GetMax() const
{
    return Check(OCI_CollGetMax(*this));
}

template<class T>
unsigned int Collection<T>::GetSize() const

{
    return Check(OCI_CollGetSize(*this));
}

template<class T>
unsigned int Collection<T>::GetCount() const

{
    return Check(OCI_CollGetCount(*this));
}

template<class T>
void Collection<T>::Truncate(unsigned int size)
{
    Check(OCI_CollTrim(*this, size));
}

template<class T>
void Collection<T>::Clear()
{
    Check(OCI_CollClear(*this));
}

template<class T>
bool Collection<T>::IsElementNull(unsigned int index) const
{
   return (Check(OCI_ElemIsNull(Check(OCI_CollGetElem(*this, index)))) == TRUE);
}

template<class T>
void Collection<T>::SetElementNull(unsigned int index)
{
    Check(OCI_ElemSetNull(Check(OCI_CollGetElem(*this, index))));
}

template<class T>
bool Collection<T>::Delete(unsigned int index) const
{
   return (Check(OCI_CollDeleteElem(*this, index)) == TRUE);
}

template<class T>
typename Collection<T>::iterator Collection<T>::begin()
{
    return iterator(this, 1);
}

template<class T>
typename Collection<T>::const_iterator Collection<T>::begin() const
{
    return const_iterator(const_cast<Collection*>(this), 1);
}

template<class T>
typename Collection<T>::iterator Collection<T>::end()
{
    return iterator(const_cast<Collection*>(this), GetCount() + 1);
}

template<class T>
typename Collection<T>::const_iterator Collection<T>::end() const
{
    return const_iterator(const_cast<Collection*>(this), GetCount() + 1);
}

template<class T>
T Collection<T>::Get(unsigned int index) const
{
    return GetElem(Check(OCI_CollGetElem(*this, index)), GetHandle());
}

template<class T>
void Collection<T>::Set(unsigned int index, const T &data)
{
    OCI_Elem * elem = Check(OCI_CollGetElem(*this, index));

    SetElem(elem, data);

    Check(OCI_CollSetElem(*this, index, elem));
}

template<class T>
void Collection<T>::Append(const T &value)
{
    OCI_Elem * elem = Check(OCI_ElemCreate(OCI_CollGetTypeInfo(*this)));

    SetElem(elem, value);

    Check(OCI_CollAppend(*this, elem));
    Check(OCI_ElemFree(elem));
}

template<>
inline bool Collection<bool>::GetElem(OCI_Elem *elem, Handle *parent)
{
    ARG_NOT_USED(parent);

    return (Check(OCI_ElemGetBoolean(elem)) == TRUE);
}

template<>
inline short Collection<short>::GetElem(OCI_Elem *elem, Handle *parent)
{
    ARG_NOT_USED(parent);

    return Check(OCI_ElemGetShort(elem));
}

template<>
inline unsigned short Collection<unsigned short>::GetElem(OCI_Elem *elem, Handle *parent)
{
    ARG_NOT_USED(parent);

    return Check(OCI_ElemGetUnsignedShort(elem));
}

template<>
inline int Collection<int>::GetElem(OCI_Elem *elem, Handle *parent)
{
    ARG_NOT_USED(parent);

    return Check(OCI_ElemGetInt(elem));
}

template<>
inline unsigned int Collection<unsigned int>::GetElem(OCI_Elem *elem, Handle *parent)
{
    ARG_NOT_USED(parent);

    return Check(OCI_ElemGetUnsignedInt(elem));
}

template<>
inline big_int Collection<big_int>::GetElem(OCI_Elem *elem, Handle *parent)
{
    ARG_NOT_USED(parent);

    return Check(OCI_ElemGetBigInt(elem));
}

template<>
inline big_uint Collection<big_uint>::GetElem(OCI_Elem *elem, Handle *parent)
{
    ARG_NOT_USED(parent);

    return Check(OCI_ElemGetUnsignedBigInt(elem));
}

template<>
inline float Collection<float>::GetElem(OCI_Elem *elem, Handle *parent)
{
    ARG_NOT_USED(parent);

    return Check(OCI_ElemGetFloat(elem));
}

template<>
inline double Collection<double>::GetElem(OCI_Elem *elem, Handle *parent)
{
    ARG_NOT_USED(parent);

    return Check(OCI_ElemGetDouble(elem));
}

template<>
inline Number Collection<Number>::GetElem(OCI_Elem *elem, Handle *parent)
{
    return Number(Check(OCI_ElemGetNumber(elem)), parent);
}

template<>
inline ostring Collection<ostring>::GetElem(OCI_Elem *elem, Handle *parent)
{
    ARG_NOT_USED(parent);

    return MakeString(Check(OCI_ElemGetString(elem)));
}

template<>
inline Raw Collection<Raw>::GetElem(OCI_Elem *elem, Handle *parent)
{
    ARG_NOT_USED(parent);

    unsigned int size = Check(OCI_ElemGetRawSize(elem));

    ManagedBuffer<unsigned char> buffer(size + 1);

    size = Check(OCI_ElemGetRaw(elem, static_cast<AnyPointer>(buffer), size));

    return MakeRaw(buffer, size);
}

template<>
inline Date Collection<Date>::GetElem(OCI_Elem *elem, Handle *parent)
{
    return Date(Check(OCI_ElemGetDate(elem)), parent);
}

template<>
inline Timestamp Collection<Timestamp>::GetElem(OCI_Elem *elem, Handle *parent)
{
    return Timestamp(Check(OCI_ElemGetTimestamp(elem)), parent);
}

template<>
inline Interval Collection<Interval>::GetElem(OCI_Elem *elem, Handle *parent)
{
    return Interval(Check(OCI_ElemGetInterval(elem)), parent);
}

template<>
inline Object Collection<Object>::GetElem(OCI_Elem *elem, Handle *parent)
{
    return Object(Check(OCI_ElemGetObject(elem)), parent);
}

template<>
inline Reference Collection<Reference>::GetElem(OCI_Elem *elem, Handle *parent)
{
    return Reference(Check(OCI_ElemGetRef(elem)), parent);
}

template<>
inline Clob Collection<Clob>::GetElem(OCI_Elem *elem, Handle *parent)
{
    return Clob(Check(OCI_ElemGetLob(elem)), parent);
}

template<>
inline NClob Collection<NClob>::GetElem(OCI_Elem *elem, Handle *parent)
{
    return NClob(Check(OCI_ElemGetLob(elem)), parent);
}
template<>
inline Blob Collection<Blob>::GetElem(OCI_Elem *elem, Handle *parent)
{
    return Blob(Check(OCI_ElemGetLob(elem)), parent);
}

template<>
inline File Collection<File>::GetElem(OCI_Elem *elem, Handle *parent)
{
    return File(Check(OCI_ElemGetFile(elem)), parent);
}

template<class T>
 T Collection<T>::GetElem(OCI_Elem *elem, Handle *parent)
{
    return T(Check(OCI_ElemGetColl(elem)), parent);
}

template<>
inline void Collection<bool>::SetElem(OCI_Elem *elem, const bool &value)
{
    Check(OCI_ElemSetBoolean(elem, static_cast<boolean>(value)));
}

template<>
inline void Collection<short>::SetElem(OCI_Elem *elem, const short &value)
{
    Check(OCI_ElemSetShort(elem, value));
}

template<>
inline void Collection<unsigned short>::SetElem(OCI_Elem *elem, const unsigned short &value)
{
    Check(OCI_ElemSetUnsignedShort(elem, value));
}

template<>
inline void Collection<int>::SetElem(OCI_Elem *elem, const int &value)
{
    Check(OCI_ElemSetInt(elem, value));
}

template<>
inline void Collection<unsigned int>::SetElem(OCI_Elem *elem, const unsigned int &value)
{
    Check(OCI_ElemSetUnsignedInt(elem, value));
}

template<>
inline void Collection<big_int>::SetElem(OCI_Elem *elem, const big_int &value)
{
    Check(OCI_ElemSetBigInt(elem, value));
}

template<>
inline void Collection<big_uint>::SetElem(OCI_Elem *elem, const big_uint &value)
{
    Check(OCI_ElemSetUnsignedBigInt(elem, value));
}

template<>
inline void Collection<float>::SetElem(OCI_Elem *elem, const float &value)
{
    Check(OCI_ElemSetFloat(elem, value));
}

template<>
inline void Collection<double>::SetElem(OCI_Elem *elem, const double &value)
{
    Check(OCI_ElemSetDouble(elem, value));
}

template<>
inline void Collection<Number>::SetElem(OCI_Elem *elem, const Number &value)
{
    Check(OCI_ElemSetNumber(elem, value));
}

template<>
inline void Collection<ostring>::SetElem(OCI_Elem *elem, const ostring& value)
{
    Check(OCI_ElemSetString(elem, value.c_str()));
}

template<>
inline void Collection<Raw>::SetElem(OCI_Elem *elem, const Raw &value)
{
    if (value.size() > 0)
    {
        Check(OCI_ElemSetRaw(elem, static_cast<AnyPointer>(const_cast<Raw::value_type *>(&value[0])), static_cast<unsigned int>(value.size())));
    }
    else
    {
        Check(OCI_ElemSetRaw(elem, nullptr, 0));
    }
}

template<>
inline void Collection<Date>::SetElem(OCI_Elem *elem, const Date &value)
{
    Check(OCI_ElemSetDate(elem, value));
}

template<>
inline void Collection<Timestamp>::SetElem(OCI_Elem *elem, const Timestamp &value)
{
    Check(OCI_ElemSetTimestamp(elem, value));
}

template<>
inline void Collection<Interval>::SetElem(OCI_Elem *elem, const Interval &value)
{
    Check(OCI_ElemSetInterval(elem, value));
}

template<>
inline void Collection<Object>::SetElem(OCI_Elem *elem, const Object &value)
{
    Check(OCI_ElemSetObject(elem, value));
}

template<>
inline void Collection<Reference>::SetElem(OCI_Elem *elem, const Reference &value)
{
    Check(OCI_ElemSetRef(elem, value));
}

template<>
inline void Collection<Clob>::SetElem(OCI_Elem *elem, const Clob &value)
{
    Check(OCI_ElemSetLob(elem, value));
}

template<>
inline void Collection<NClob>::SetElem(OCI_Elem *elem, const NClob &value)
{
    Check(OCI_ElemSetLob(elem, value));
}

template<>
inline void Collection<Blob>::SetElem(OCI_Elem *elem, const Blob &value)
{
    Check(OCI_ElemSetLob(elem, value));
}

template<>
inline void Collection<File>::SetElem(OCI_Elem *elem, const File &value)
{
    Check(OCI_ElemSetFile(elem, value));
}

template<class T>
void Collection<T>::SetElem(OCI_Elem *elem, const T &value)
{
    Check(OCI_ElemSetColl(elem, value));
}

template<class T>
ostring Collection<T>::ToString() const
{
    if (!IsNull())
    {
        unsigned int len = 0;

        Check(OCI_CollToText(*this, &len, nullptr));

        ManagedBuffer<otext> buffer(len + 1);

        Check(OCI_CollToText(*this, &len, buffer));

        return MakeString(static_cast<const otext *>(buffer), static_cast<int>(len));
    }

    return OCI_STRING_NULL;
}

template<class T>
CollectionElement<T> Collection<T>::operator [] (unsigned int index)
{
    return CollectionElement<T>(this, index);
}

template<class T>
const CollectionElement<T> Collection<T>::operator [] (unsigned int index) const
{
    return  CollectionElement<T>(this, index);
}

template<class T>
CollectionIterator<T>::CollectionIterator() : _elem()
{

}

template<class T>
CollectionIterator<T>::CollectionIterator(CollectionType *collection, unsigned int pos) : _elem(collection, pos)
{

}

template<class T>
CollectionIterator<T>::CollectionIterator(const CollectionIterator& other) : _elem(other._elem)
{

}

template<class T>
CollectionIterator<T>& CollectionIterator<T>::operator = (const CollectionIterator& other)
{
	_elem._pos  = other._elem._pos;
	_elem._coll = other._elem._coll;

	return *this;
}

template<class T>
CollectionIterator<T>& CollectionIterator<T>::operator += (difference_type value)
{
	_elem._pos += static_cast<unsigned int>(value);
	return *this;
}

template<class T>
CollectionIterator<T>& CollectionIterator<T>::operator -= (difference_type value)
{
	_elem._pos -= static_cast<unsigned int>(value);
	return *this;
}

template<class T>
T& CollectionIterator<T>::operator*()
{
	return _elem;
}

template<class T>
T* CollectionIterator<T>::operator->()
{
	return &_elem;
}

template<class T>
CollectionIterator<T>& CollectionIterator<T>::operator--()
{
	--_elem._pos;
	return *this;
}

template<class T>
CollectionIterator<T>& CollectionIterator<T>::operator++()
{
	++*(const_cast<unsigned int*>(&_elem._pos));
	return *this;
}

template<class T>
CollectionIterator<T> CollectionIterator<T>::operator++(int)
{
	CollectionIterator res(_elem._coll, _elem._pos);
	++(*this);
	return res;
}

template<class T>
CollectionIterator<T> CollectionIterator<T>::operator--(int)
{
	CollectionIterator res(_elem);
	--(*this);
	return res;
}

template<class T>
CollectionIterator<T>  CollectionIterator<T>::operator + (difference_type value)
{
	return CollectionIterator(_elem._coll, _elem._pos + static_cast<unsigned int>(value));
}

template<class T>
CollectionIterator<T> CollectionIterator<T>::operator - (difference_type value)
{
	return CollectionIterator(_elem._coll, _elem._pos - static_cast<unsigned int>(value));
}

template<class T>
typename CollectionIterator<T>::difference_type CollectionIterator<T>::operator - (const CollectionIterator &value)
{
	return static_cast<difference_type>(_elem._pos - value._elem._pos);
}

template<class T>
bool CollectionIterator<T>::operator == (const CollectionIterator& other)
{
	return _elem._pos == other._elem._pos && (static_cast<OCI_Coll *>(*_elem._coll)) == (static_cast<OCI_Coll *>(*other._elem._coll));
}

template<class T>
bool CollectionIterator<T>::operator != (const CollectionIterator& other)
{
	return !(*this == other);
}

template<class T>
bool CollectionIterator<T>::operator > (const CollectionIterator& other)
{
	return _elem._pos > other._elem._pos;
}

template<class T>
bool CollectionIterator<T>::operator < (const CollectionIterator& other)
{
	return _elem._pos < other._elem._pos;
}

template<class T>
bool CollectionIterator<T>::operator >= (const CollectionIterator& other)
{
	return _elem._pos >= other._elem._pos;
}

template<class T>
bool CollectionIterator<T>::operator <= (const CollectionIterator& other)
{
	return _elem._pos <= other._elem._pos;
}

template<class T>
CollectionElement<T>::CollectionElement() : _coll(nullptr), _pos(0)
{

}

template<class T>
CollectionElement<T>::CollectionElement(CollectionType *coll, unsigned int pos) : _coll(coll), _pos(pos)
{

}

template<class T>
CollectionElement<T>::operator T() const
{
    return _coll->Get(_pos);
}

template<class T>
CollectionElement<T>& CollectionElement<T>::operator = (const ValueType& value)
{
    _coll->Set(_pos, value);
    return *this;
}

template<class T>
CollectionElement<T>& CollectionElement<T>::operator = (const CollectionElement &other)
{
	_coll->Set(_pos, static_cast<T>(other));
	return *this;
}

template<class T>
bool CollectionElement<T>::IsNull() const
{
    return _coll->IsElementNull(_pos);
}

template<class T>
void CollectionElement<T>::SetNull()
{
    _coll->SetElementNull(_pos);
}

/* --------------------------------------------------------------------------------------------- *
 * Long
 * --------------------------------------------------------------------------------------------- */

template<class T, int U>
Long<T, U>::Long()
{
}

template<class T, int U>
Long<T, U>::Long(const Statement &statement)
{
    Acquire(Check(OCI_LongCreate(statement, U)), reinterpret_cast<HandleFreeFunc>(OCI_LongFree), nullptr, statement.GetHandle());
}

template<class T, int U>
Long<T, U>::Long(OCI_Long *pLong, Handle* parent)
{
    Acquire(pLong, nullptr, nullptr, parent);
}

template<class T, int U>
unsigned int Long<T, U>::Write(const T& content)
{
    unsigned int res = 0;

    if (content.size() > 0)
    {
        res = Check(OCI_LongWrite(*this, static_cast<AnyPointer>(const_cast<typename T::value_type *>(&content[0])), static_cast<unsigned int>(content.size())));
    }

    return res;
}

template<class T, int U>
unsigned int Long<T, U>::GetLength() const
{
    return Check(OCI_LongGetSize(*this));
}

template<>
inline ostring Long<ostring, LongCharacter>::GetContent() const
{
    return MakeString(static_cast<const otext *>(Check(OCI_LongGetBuffer(*this))));
}

template<>
inline Raw Long<Raw, LongBinary>::GetContent() const
{
    return MakeRaw(Check(OCI_LongGetBuffer(*this)), GetLength());
}

/* --------------------------------------------------------------------------------------------- *
 * BindObject
 * --------------------------------------------------------------------------------------------- */

inline BindObject::BindObject(const Statement &statement, const ostring& name, unsigned int mode) : _pStatement(statement), _name(name), _mode(mode)
{
}

inline BindObject::~BindObject()
{
}

inline ostring BindObject::GetName() const
{
    return _name;
}

inline Statement BindObject::GetStatement() const
{
    return Statement(_pStatement);
}

inline unsigned int BindObject::GetMode() const
{
    return _mode;
}

/* --------------------------------------------------------------------------------------------- *
 * BindArray
 * --------------------------------------------------------------------------------------------- */

inline BindArray::BindArray(const Statement &statement, const ostring& name, unsigned int mode) : BindObject(statement, name, mode), _object(nullptr)
{

}

template<class T>
void BindArray::SetVector(std::vector<T> & vector, unsigned int elemSize)
{
    _object = new BindArrayObject<T>(GetStatement(), GetName(), vector, GetMode(), elemSize);
}

inline BindArray::~BindArray()
{
    delete _object;
}

template<class T>
typename BindResolver<T>::OutputType * BindArray::GetData()  const
{
    return static_cast<typename BindResolver<T>::OutputType *>(*(dynamic_cast< BindArrayObject<T> * > (_object)));
}

inline void BindArray::SetInData()
{
    if (GetMode() & OCI_BDM_IN)
    {
        _object->SetInData();
    }
}

inline void BindArray::SetOutData()
{
    if (GetMode() & OCI_BDM_OUT)
    {
        _object->SetOutData();
    }
}

template<class T>
BindArray::BindArrayObject<T>::BindArrayObject(const Statement &statement, const ostring& name, ObjectVector &vector, unsigned int mode, unsigned int elemSize)
    : _pStatement(statement), _name(name), _vector(vector), _data(nullptr), _mode(mode), _elemCount(statement.GetBindArraySize()), _elemSize(elemSize)
{
    AllocData();
}

template<class T>
BindArray::BindArrayObject<T>::~BindArrayObject()
{
    FreeData();
}

template<class T>
void BindArray::BindArrayObject<T>::AllocData()
{
    _data = new NativeType[_elemCount];

    memset(_data, 0, sizeof(NativeType) * _elemCount);
}

template<>
inline void BindArray::BindArrayObject<ostring>::AllocData()
{
    _data = new otext[_elemSize * _elemCount];

    memset(_data, 0, _elemSize * _elemCount * sizeof(otext));
}

template<>
inline void BindArray::BindArrayObject<Raw> ::AllocData()
{
    _data = new unsigned char[_elemSize * _elemCount];

    memset(_data, 0, _elemSize * _elemCount * sizeof(unsigned char));
}

template<class T>
void BindArray::BindArrayObject<T>::FreeData() const
{
    delete [] _data ;
}

template<class T>
void BindArray::BindArrayObject<T>::SetInData()
{
    typename ObjectVector::iterator it, it_end;

    unsigned int index = 0;
    unsigned int currElemCount = Check(OCI_BindArrayGetSize(_pStatement));

    for (it = _vector.begin(), it_end = _vector.end(); it != it_end && index < _elemCount && index < currElemCount; ++it, ++index)
    {
        _data[index] = static_cast<NativeType>(*it);
    }
}

template<>
inline void BindArray::BindArrayObject<ostring>::SetInData()
{
    std::vector<ostring>::iterator it, it_end;

    unsigned int index = 0;
    unsigned int currElemCount = Check(OCI_BindArrayGetSize(_pStatement));

    for (it = _vector.begin(), it_end = _vector.end(); it != it_end && index < _elemCount && index < currElemCount; ++it, ++index)
    {
        const ostring & value = *it;

        memcpy( _data + (_elemSize * index), value.c_str(), (value.size() + 1) * sizeof(otext));
    }
}

template<>
inline void BindArray::BindArrayObject<Raw>::SetInData()
{
    std::vector<Raw>::iterator it, it_end;

    unsigned int index = 0;
    unsigned int currElemCount = Check(OCI_BindArrayGetSize(_pStatement));

    for (it = _vector.begin(), it_end = _vector.end(); it != it_end && index < _elemCount && index < currElemCount; ++it, ++index)
    {
        Raw & value = *it;

        if (value.size() > 0)
        {
            memcpy(_data + (_elemSize * index), &value[0], value.size());
        }

        OCI_BindSetDataSizeAtPos(OCI_GetBind2(_pStatement, GetName().c_str()), index + 1, static_cast<unsigned int>(value.size()));
    }
}

template<class T>
void BindArray::BindArrayObject<T>::SetOutData()
{
    typename ObjectVector::iterator it, it_end;

    unsigned int index = 0;
    unsigned int currElemCount = Check(OCI_BindArrayGetSize(_pStatement));

    for (it = _vector.begin(), it_end = _vector.end(); it != it_end && index < _elemCount && index < currElemCount; ++it, ++index)
    {
        T& object = *it;

        object = static_cast<NativeType>(_data[index]);
    }
}

template<>
inline void BindArray::BindArrayObject<Raw>::SetOutData()
{
    std::vector<Raw>::iterator it, it_end;

    OCI_Bind *pBind = Check(OCI_GetBind2(_pStatement, GetName().c_str()));

    unsigned int index = 0;
    unsigned int currElemCount = Check(OCI_BindArrayGetSize(_pStatement));

    for (it = _vector.begin(), it_end = _vector.end(); it != it_end && index < _elemCount && index < currElemCount; ++it, ++index)
    {
        unsigned char *currData = _data + (_elemSize * index);

        (*it).assign(currData, currData + Check(OCI_BindGetDataSizeAtPos(pBind, index + 1)));
    }
}

template<class T>
ostring BindArray::BindArrayObject<T>::GetName()
{
    return _name;
}

template<class T>
BindArray::BindArrayObject<T>::operator ObjectVector & ()  const
{
    return _vector;
}

template<class T>
BindArray::BindArrayObject<T>::operator NativeType * ()  const
{
    return _data;
}

/* --------------------------------------------------------------------------------------------- *
 * BindObjectAdaptor
 * --------------------------------------------------------------------------------------------- */

template<class T>
void BindObjectAdaptor<T>::SetInData()
{
    if (GetMode() & OCI_BDM_IN)
    {
        size_t size = _object.size();

        if (size > _size)
        {
            size = _size;
        }

        if (size > 0)
        {
            memcpy(_data, &_object[0], size * sizeof(NativeType));
        }

        _data[size] = 0;
    }
}

template<class T>
void BindObjectAdaptor<T>::SetOutData()
{
    if (GetMode() & OCI_BDM_OUT)
    {
        size_t size = Check(OCI_BindGetDataSize(Check(OCI_GetBind2(_pStatement, _name.c_str()))));

        _object.assign(_data, _data + size);
    }
}

template<class T>
BindObjectAdaptor<T>::BindObjectAdaptor(const Statement &statement, const ostring& name, unsigned int mode, ObjectType &object, unsigned int size) :
     BindObject(statement, name, mode),
     _object(object),
     _data(new NativeType[size + 1]),
     _size(size)
{
    memset(_data, 0, _size * sizeof(NativeType));
}

template<class T>
BindObjectAdaptor<T>::~BindObjectAdaptor()
{
    delete [] _data;
}

template<class T>
BindObjectAdaptor<T>::operator NativeType *()  const
{
    return _data;
}

/* --------------------------------------------------------------------------------------------- *
 * BindTypeAdaptor
 * --------------------------------------------------------------------------------------------- */

template<class T>
void BindTypeAdaptor<T>::SetInData()
{
    if (GetMode() & OCI_BDM_IN)
    {
        *_data = static_cast<NativeType>(_object);
    }
}

template<class T>
void BindTypeAdaptor<T>::SetOutData()
{
    if (GetMode() & OCI_BDM_OUT)
    {
        _object = static_cast<T>(*_data);
    }
}

template<class T>
BindTypeAdaptor<T>::BindTypeAdaptor(const Statement &statement, const ostring& name, unsigned int mode, ObjectType &object) :
BindObject(statement, name, mode),
_object(object),
_data(new NativeType)
{

}

template<class T>
BindTypeAdaptor<T>::~BindTypeAdaptor()
{
    delete _data;
}

template<class T>
BindTypeAdaptor<T>::operator NativeType *()  const
{
    return _data;
}

template<>
inline void BindTypeAdaptor<bool>::SetInData()
{
    if (GetMode() & OCI_BDM_IN)
    {
        *_data = (_object == true);
    }
}

template<>
inline void BindTypeAdaptor<bool>::SetOutData()
{
    if (GetMode() & OCI_BDM_OUT)
    {
        _object = (*_data == TRUE);
    }
}

/* --------------------------------------------------------------------------------------------- *
 * BindsHolder
 * --------------------------------------------------------------------------------------------- */

inline BindsHolder::BindsHolder(const Statement &statement) : _bindObjects(), _pStatement(statement)
{

}

inline BindsHolder::~BindsHolder()
{
    Clear();
}

inline void BindsHolder::Clear()
{
    std::vector<BindObject *>::iterator it, it_end;

    for(it = _bindObjects.begin(), it_end = _bindObjects.end(); it != it_end; ++it)
    {
        delete (*it);
    }

    _bindObjects.clear();
}

inline void BindsHolder::AddBindObject(BindObject *bindObject)
{
    if (Check(OCI_IsRebindingAllowed(_pStatement)))
    {
        std::vector<BindObject *>::iterator it, it_end;

        for(it = _bindObjects.begin(), it_end = _bindObjects.end(); it != it_end; ++it)
        {
            if ((*it)->GetName() == bindObject->GetName())
            {
                _bindObjects.erase(it);
                break;
            }
        }
    }

    _bindObjects.push_back(bindObject);
}

inline void BindsHolder::SetOutData()
{
    std::vector<BindObject *>::iterator it, it_end;

    for(it = _bindObjects.begin(), it_end = _bindObjects.end(); it != it_end; ++it)
    {
        (*it)->SetOutData();
    }
}

inline void BindsHolder::SetInData()
{
    std::vector<BindObject *>::iterator it, it_end;

    for(it = _bindObjects.begin(), it_end = _bindObjects.end(); it != it_end; ++it)
    {
        (*it)->SetInData();
    }
}

/* --------------------------------------------------------------------------------------------- *
 * Bind
 * --------------------------------------------------------------------------------------------- */

inline BindInfo::BindInfo(OCI_Bind *pBind, Handle *parent)
{
    Acquire(pBind, nullptr, nullptr, parent);
}

inline ostring BindInfo::GetName() const
{
    return MakeString(Check(OCI_BindGetName(*this)));
}

inline DataType BindInfo::GetType() const
{
    return DataType(static_cast<DataType::Type>(Check(OCI_BindGetType(*this))));
}

inline unsigned int BindInfo::GetSubType() const
{
    return Check(OCI_BindGetSubtype(*this));
}

inline unsigned int BindInfo::GetDataCount() const
{
    return Check(OCI_BindGetDataCount(*this));
}

inline Statement BindInfo::GetStatement() const
{
    return Statement(Check(OCI_BindGetStatement(*this)));
}

inline void BindInfo::SetDataNull(bool value, unsigned int index)
{
    if (value)
    {
        Check(OCI_BindSetNullAtPos(*this, index));
    }
    else
    {
        Check(OCI_BindSetNotNullAtPos(*this, index));
    }
}

inline bool BindInfo::IsDataNull(unsigned int index) const
{
    return (Check(OCI_BindIsNullAtPos(*this, index)) == TRUE);
}

inline void BindInfo::SetCharsetForm(CharsetForm value)
{
    Check(OCI_BindSetCharsetForm(*this, value));
}

inline BindInfo::BindDirection BindInfo::GetDirection() const
{
    return BindDirection(static_cast<BindDirection::Type>(Check(OCI_BindGetDirection(*this))));
}

/* --------------------------------------------------------------------------------------------- *
 * Statement
 * --------------------------------------------------------------------------------------------- */

inline Statement::Statement()
{
}

inline Statement::Statement(const Connection &connection)
{
    Acquire(Check(OCI_StatementCreate(connection)), reinterpret_cast<HandleFreeFunc>(OCI_StatementFree), OnFreeSmartHandle, connection.GetHandle());
}

inline Statement::Statement(OCI_Statement *stmt, Handle *parent)
{
    Acquire(stmt, reinterpret_cast<HandleFreeFunc>(parent ? OCI_StatementFree : nullptr), OnFreeSmartHandle, parent);
}

inline Connection Statement::GetConnection() const
{
    return Connection(Check(OCI_StatementGetConnection(*this)), nullptr);
}

inline void Statement::Describe(const ostring& sql)
{
    ClearBinds();
    ReleaseResultsets();
    Check(OCI_Describe(*this, sql.c_str()));
}

inline void Statement::Parse(const ostring& sql)
{
    ClearBinds();
    ReleaseResultsets();
    Check(OCI_Parse(*this, sql.c_str()));
}

inline void Statement::Prepare(const ostring& sql)
{
    ClearBinds();
    ReleaseResultsets();
    Check(OCI_Prepare(*this, sql.c_str()));
}

inline void Statement::ExecutePrepared()
{
    ReleaseResultsets();
    SetInData();
    Check(OCI_Execute(*this));
    SetOutData();
}

template<class T>
unsigned int Statement::ExecutePrepared(T callback)
{
    ExecutePrepared();

    return Fetch(callback);
}

template<class T, class U>
unsigned int Statement::ExecutePrepared(T callback, U adapter)
{
    ExecutePrepared();

    return Fetch(callback, adapter);
}

inline void Statement::Execute(const ostring& sql)
{
    ClearBinds();
    ReleaseResultsets();
    Check(OCI_ExecuteStmt(*this, sql.c_str()));
}

template<class T>
unsigned int Statement::Execute(const ostring& sql, T callback)
{
    Execute(sql);

    return Fetch(callback);
}

template<class T, class U>
unsigned int Statement::Execute(const ostring& sql, T callback, U adapter)
{
    Execute(sql);

    return Fetch(callback, adapter);
}

template<typename T>
unsigned int  Statement::Fetch(T callback)
{
    unsigned int res = 0;

    Resultset rs = GetResultset();

    while (rs)
    {
        res += rs.ForEach(callback);
        rs = GetNextResultset();
    }

    return res;
}

template<class T, class U>
unsigned int Statement::Fetch(T callback, U adapter)
{
    unsigned int res = 0;

    Resultset rs = GetResultset();

    while (rs)
    {
        res += rs.ForEach(callback, adapter);
        rs = GetNextResultset();
    }

    return res;
}

inline unsigned int Statement::GetAffectedRows() const
{
    return Check(OCI_GetAffectedRows(*this));
}

inline ostring Statement::GetSql() const
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

inline unsigned int Statement::GetBindArraySize() const
{
    return Check(OCI_BindArrayGetSize(*this));
}

inline void Statement::AllowRebinding(bool value)
{
    Check(OCI_AllowRebinding(*this, value));
}

inline bool Statement::IsRebindingAllowed() const
{
    return (Check(OCI_IsRebindingAllowed(*this)) == TRUE);
}

inline unsigned int Statement::GetBindIndex(const ostring& name) const
{
    return Check(OCI_GetBindIndex(*this, name.c_str()));
}

inline unsigned int Statement::GetBindCount() const
{
    return Check(OCI_GetBindCount(*this));
}

inline BindInfo Statement::GetBind(unsigned int index) const
{
    return BindInfo(Check(OCI_GetBind(*this, index)), GetHandle());
}

inline BindInfo Statement::GetBind(const ostring& name) const
{
    return BindInfo(Check(OCI_GetBind2(*this, name.c_str())), GetHandle());
}

template<typename M, class T>
void Statement::Bind1(M &method, const ostring& name, T& value, BindInfo::BindDirection mode)
{
    Check(method(*this, name.c_str(), &value));
    SetLastBindMode(mode);
}

template<typename M, class T>
void Statement::Bind2(M &method, const ostring& name, T& value, BindInfo::BindDirection mode)
{
    Check(method(*this, name.c_str(), static_cast<typename BindResolver<T>::OutputType>(value)));
    SetLastBindMode(mode);
}

template<typename M, class T>
void Statement::BindVector1(M &method, const ostring& name, std::vector<T> &values,  BindInfo::BindDirection mode)
{
    BindArray * bnd = new BindArray(*this, name, mode);
    bnd->SetVector<T>(values, sizeof(typename BindResolver<T>::OutputType));

    boolean res = method(*this, name.c_str(), bnd->GetData<T>(), 0);

    if (res)
    {
        BindsHolder *bindsHolder = GetBindsHolder(true);
        bindsHolder->AddBindObject(bnd);
        SetLastBindMode(mode);
    }
    else
    {
        delete bnd;
    }

    Check(res);
}

template<typename M, class T, class U>
void Statement::BindVector2(M &method, const ostring& name, std::vector<T> &values, BindInfo::BindDirection mode, U type)
{
    BindArray * bnd = new BindArray(*this, name, mode);
    bnd->SetVector<T>(values, sizeof(typename BindResolver<T>::OutputType));

    boolean res = method(*this, name.c_str(), bnd->GetData<T>(), type, 0);

    if (res)
    {
        BindsHolder *bindsHolder = GetBindsHolder(true);
        bindsHolder->AddBindObject(bnd);
        SetLastBindMode(mode);
    }
    else
    {
        delete bnd;
    }

    Check(res);
}

template<>
inline void Statement::Bind<bool>(const ostring& name, bool &value, BindInfo::BindDirection mode)
{
    BindTypeAdaptor<bool> * bnd = new BindTypeAdaptor<bool>(*this, name, mode, value);

    boolean res = OCI_BindBoolean(*this, name.c_str(), static_cast<boolean *>(*bnd));

    if (res)
    {
        BindsHolder *bindsHolder = GetBindsHolder(true);
        bindsHolder->AddBindObject(bnd);
        SetLastBindMode(mode);
    }
    else
    {
        delete bnd;
    }

    Check(res);
}

template<>
inline void Statement::Bind<short>(const ostring& name, short &value, BindInfo::BindDirection mode)
{
    Bind1(OCI_BindShort, name, value, mode);
}

template<>
inline void Statement::Bind<unsigned short>(const ostring& name, unsigned short &value, BindInfo::BindDirection mode)
{
    Bind1(OCI_BindUnsignedShort, name, value, mode);
}

template<>
inline void Statement::Bind<int>(const ostring& name, int &value, BindInfo::BindDirection mode)
{
    Bind1(OCI_BindInt, name, value, mode);
}

template<>
inline void Statement::Bind<unsigned int>(const ostring& name, unsigned int &value, BindInfo::BindDirection mode)
{
    Bind1(OCI_BindUnsignedInt, name, value, mode);
}

template<>
inline void Statement::Bind<big_int>(const ostring& name, big_int &value, BindInfo::BindDirection mode)
{
    Bind1(OCI_BindBigInt, name, value, mode);
}

template<>
inline void Statement::Bind<big_uint>(const ostring& name, big_uint &value, BindInfo::BindDirection mode)
{
    Bind1(OCI_BindUnsignedBigInt, name, value, mode);
}

template<>
inline void Statement::Bind<float>(const ostring& name, float &value, BindInfo::BindDirection mode)
{
    Bind1(OCI_BindFloat, name, value, mode);
}

template<>
inline void Statement::Bind<double>(const ostring& name, double &value, BindInfo::BindDirection mode)
{
    Bind1(OCI_BindDouble, name, value, mode);
}

template<>
inline void Statement::Bind<Number>(const ostring& name, Number &value, BindInfo::BindDirection mode)
{
    Bind2(OCI_BindNumber, name, value, mode);
}

template<>
inline void Statement::Bind<Date>(const ostring& name, Date &value, BindInfo::BindDirection mode)
{
    Bind2(OCI_BindDate, name, value, mode);
}

template<>
inline void Statement::Bind<Timestamp>(const ostring& name, Timestamp &value, BindInfo::BindDirection mode)
{
    Bind2(OCI_BindTimestamp, name, value, mode);
}

template<>
inline void Statement::Bind<Interval>(const ostring& name, Interval &value, BindInfo::BindDirection mode)
{
    Bind2(OCI_BindInterval, name, value, mode);
}

template<>
inline void Statement::Bind<Clob>(const ostring& name, Clob &value, BindInfo::BindDirection mode)
{
    Bind2(OCI_BindLob, name, value, mode);
}

template<>
inline void Statement::Bind<NClob>(const ostring& name, NClob &value, BindInfo::BindDirection mode)
{
    Bind2(OCI_BindLob, name, value, mode);
}

template<>
inline void Statement::Bind<Blob>(const ostring& name, Blob &value, BindInfo::BindDirection mode)
{
    Bind2(OCI_BindLob, name, value, mode);
}

template<>
inline void Statement::Bind<File>(const ostring& name, File &value, BindInfo::BindDirection mode)
{
    Bind2(OCI_BindFile, name, value, mode);
}

template<>
inline void Statement::Bind<Object>(const ostring& name, Object &value, BindInfo::BindDirection mode)
{
    Bind2(OCI_BindObject, name, value, mode);
}

template<>
inline void Statement::Bind<Reference>(const ostring& name, Reference &value, BindInfo::BindDirection mode)
{
    Bind2(OCI_BindRef, name, value, mode);
}

template<>
inline void Statement::Bind<Statement>(const ostring& name, Statement &value, BindInfo::BindDirection mode)
{
    Bind2(OCI_BindStatement, name, value, mode);
}

template<>
inline void Statement::Bind<Clong, unsigned int>(const ostring& name, Clong &value, unsigned int maxSize, BindInfo::BindDirection mode)
{
    Check(OCI_BindLong(*this, name.c_str(), value, maxSize));
    SetLastBindMode(mode);
}

template<>
inline void Statement::Bind<Clong, int>(const ostring& name, Clong &value, int maxSize, BindInfo::BindDirection mode)
{
    Bind<Clong, unsigned int>(name, value, static_cast<unsigned int>(maxSize), mode);
}

template<>
inline void Statement::Bind<Blong, unsigned int>(const ostring& name, Blong &value, unsigned int maxSize, BindInfo::BindDirection mode)
{
    Check(OCI_BindLong(*this, name.c_str(), value, maxSize));
    SetLastBindMode(mode);
}

template<>
inline void Statement::Bind<Blong, int>(const ostring& name, Blong &value, int maxSize, BindInfo::BindDirection mode)
{
    Bind<Blong, unsigned int>(name, value, static_cast<unsigned int>(maxSize), mode);
}

template<>
inline void Statement::Bind<ostring, unsigned int>(const ostring& name, ostring &value, unsigned int maxSize, BindInfo::BindDirection mode)
{
    if (maxSize == 0)
    {
        maxSize = static_cast<unsigned int>(value.size());
    }

    value.reserve(maxSize);

    BindObjectAdaptor<ostring> * bnd = new BindObjectAdaptor<ostring>(*this, name, mode, value, maxSize + 1);

    boolean res = OCI_BindString(*this, name.c_str(), static_cast<otext *>(*bnd), maxSize);

    if (res)
    {
        BindsHolder *bindsHolder = GetBindsHolder(true);
        bindsHolder->AddBindObject(bnd);
        SetLastBindMode(mode);
    }
    else
    {
        delete bnd;
    }

    Check(res);
}

template<>
inline void Statement::Bind<ostring, int>(const ostring& name, ostring &value, int maxSize, BindInfo::BindDirection mode)
{
    Bind<ostring, unsigned int>(name, value, static_cast<unsigned int>(maxSize),  mode);
}

template<>
inline void Statement::Bind<Raw, unsigned int>(const ostring& name, Raw &value, unsigned int maxSize, BindInfo::BindDirection mode)
{
    if (maxSize == 0)
    {
        maxSize = static_cast<unsigned int>(value.size());
    }

    value.reserve(maxSize);

    BindObjectAdaptor<Raw> * bnd = new BindObjectAdaptor<Raw>(*this, name, mode, value, maxSize);

    boolean res = OCI_BindRaw(*this, name.c_str(), static_cast<unsigned char *>(*bnd), maxSize);

    if (res)
    {
        BindsHolder *bindsHolder = GetBindsHolder(true);
        bindsHolder->AddBindObject(bnd);
        SetLastBindMode(mode);
    }
    else
    {
        delete bnd;
    }

    Check(res);
}

template<>
inline void Statement::Bind<Raw, int>(const ostring& name, Raw &value,  int maxSize, BindInfo::BindDirection mode)
{
    Bind<Raw, unsigned int>(name, value, static_cast<unsigned int>(maxSize), mode);
}

template<>
inline void Statement::Bind<short>(const ostring& name, std::vector<short> &values, BindInfo::BindDirection mode)
{
    BindVector1(OCI_BindArrayOfShorts, name, values, mode);
}

template<>
inline void Statement::Bind<unsigned short>(const ostring& name, std::vector<unsigned short> &values, BindInfo::BindDirection mode)
{
    BindVector1(OCI_BindArrayOfUnsignedShorts, name, values, mode);
}

template<>
inline void Statement::Bind<int>(const ostring& name, std::vector<int> &values, BindInfo::BindDirection mode)
{
    BindVector1(OCI_BindArrayOfInts, name, values, mode);
}

template<>
inline void Statement::Bind<unsigned int>(const ostring& name, std::vector<unsigned int> &values, BindInfo::BindDirection mode)
{
    BindVector1(OCI_BindArrayOfUnsignedInts, name, values, mode);
}

template<>
inline void Statement::Bind<big_int>(const ostring& name, std::vector<big_int> &values, BindInfo::BindDirection mode)
{
    BindVector1(OCI_BindArrayOfBigInts, name, values, mode);
}

template<>
inline void Statement::Bind<big_uint>(const ostring& name, std::vector<big_uint> &values, BindInfo::BindDirection mode)
{
    BindVector1(OCI_BindArrayOfUnsignedBigInts, name, values, mode);
}

template<>
inline void Statement::Bind<float>(const ostring& name, std::vector<float> &values, BindInfo::BindDirection mode)
{
    BindVector1(OCI_BindArrayOfFloats, name, values, mode);
}

template<>
inline void Statement::Bind<double>(const ostring& name, std::vector<double> &values, BindInfo::BindDirection mode)
{
    BindVector1(OCI_BindArrayOfDoubles, name, values, mode);
}

template<>
inline void Statement::Bind<Date>(const ostring& name, std::vector<Date> &values, BindInfo::BindDirection mode)
{
    BindVector1(OCI_BindArrayOfDates, name, values, mode);
}

template<>
inline void Statement::Bind<Number>(const ostring& name, std::vector<Number> &values, BindInfo::BindDirection mode)
{
    BindVector1(OCI_BindArrayOfNumbers, name, values, mode);
}

template<class T>
void Statement::Bind(const ostring& name, Collection<T> &value, BindInfo::BindDirection mode)
{
    Check(OCI_BindColl(*this, name.c_str(), value));
    SetLastBindMode(mode);
}

template<>
inline void Statement::Bind<Timestamp, Timestamp::TimestampTypeValues>(const ostring& name, std::vector<Timestamp> &values, Timestamp::TimestampTypeValues type, BindInfo::BindDirection mode)
{
    BindVector2(OCI_BindArrayOfTimestamps, name, values, mode, type);
}

template<>
inline void Statement::Bind<Timestamp, Timestamp::TimestampType>(const ostring& name, std::vector<Timestamp> &values, Timestamp::TimestampType type, BindInfo::BindDirection mode)
{
    Bind<Timestamp, Timestamp::TimestampTypeValues>(name, values, type.GetValue(), mode);
}

template<>
inline void Statement::Bind<Interval, Interval::IntervalTypeValues>(const ostring& name, std::vector<Interval> &values, Interval::IntervalTypeValues type, BindInfo::BindDirection mode)
{
    BindVector2(OCI_BindArrayOfIntervals, name, values, mode, type);
}

template<>
inline void Statement::Bind<Interval, Interval::IntervalType>(const ostring& name, std::vector<Interval> &values, Interval::IntervalType type, BindInfo::BindDirection mode)
{
    Bind<Interval, Interval::IntervalTypeValues>(name, values, type.GetValue(), mode);
}

template<>
inline void Statement::Bind<Clob>(const ostring& name, std::vector<Clob> &values, BindInfo::BindDirection mode)
{
    BindVector2(OCI_BindArrayOfLobs, name, values, mode, static_cast<unsigned int>(OCI_CLOB));
}

template<>
inline void Statement::Bind<NClob>(const ostring& name, std::vector<NClob> &values, BindInfo::BindDirection mode)
{
    BindVector2(OCI_BindArrayOfLobs, name, values, mode, static_cast<unsigned int>(OCI_NCLOB));
}

template<>
inline void Statement::Bind<Blob>(const ostring& name, std::vector<Blob> &values, BindInfo::BindDirection mode)
{
    BindVector2(OCI_BindArrayOfLobs, name, values, mode, static_cast<unsigned int>(OCI_BLOB));
}

template<>
inline void Statement::Bind<File>(const ostring& name, std::vector<File> &values, BindInfo::BindDirection mode)
{
    BindVector2(OCI_BindArrayOfFiles, name, values, mode, static_cast<unsigned int>(OCI_BFILE));
}

template<>
inline void Statement::Bind<Object>(const ostring& name, std::vector<Object> &values, TypeInfo &typeInfo, BindInfo::BindDirection mode)
{
    BindVector2(OCI_BindArrayOfObjects, name, values, mode, static_cast<OCI_TypeInfo *>(typeInfo));
}

template<>
inline void Statement::Bind<Reference>(const ostring& name, std::vector<Reference> &values, TypeInfo &typeInfo, BindInfo::BindDirection mode)
{
    BindVector2(OCI_BindArrayOfRefs, name, values, mode, static_cast<OCI_TypeInfo *>(typeInfo));
}

template<class T>
void Statement::Bind(const ostring& name, std::vector<Collection<T> > &values, TypeInfo &typeInfo, BindInfo::BindDirection mode)
{
    BindVector2(OCI_BindArrayOfColls, name, values, mode, static_cast<OCI_TypeInfo *>(typeInfo));
}

template<>
inline void Statement::Bind<ostring, unsigned int>(const ostring& name, std::vector<ostring> &values,  unsigned int maxSize, BindInfo::BindDirection mode)
{
    BindArray * bnd = new BindArray(*this, name, mode);
    bnd->SetVector<ostring>(values, maxSize+1);

    boolean res = OCI_BindArrayOfStrings(*this, name.c_str(), bnd->GetData<ostring>(), maxSize, 0);

    if (res)
    {
        BindsHolder *bindsHolder = GetBindsHolder(true);
        bindsHolder->AddBindObject(bnd);
        SetLastBindMode(mode);
    }
    else
    {
        delete bnd;
    }

    Check(res);
}

template<>
inline void Statement::Bind<ostring, int>(const ostring& name, std::vector<ostring> &values, int maxSize, BindInfo::BindDirection mode)
{
    Bind<ostring, unsigned int>(name, values, static_cast<unsigned int>(maxSize), mode);
}

template<>
inline void Statement::Bind<Raw, unsigned int>(const ostring& name, std::vector<Raw> &values, unsigned int maxSize, BindInfo::BindDirection mode)
{
    BindArray * bnd = new BindArray(*this, name, mode);
    bnd->SetVector<Raw>(values, maxSize);

    boolean res = OCI_BindArrayOfRaws(*this, name.c_str(), bnd->GetData<Raw>(), maxSize, 0);

    if (res)
    {
        BindsHolder *bindsHolder = GetBindsHolder(true);
        bindsHolder->AddBindObject(bnd);
        SetLastBindMode(mode);
    }
    else
    {
        delete bnd;
    }

    Check(res);
}

template<class T>
void Statement::Bind(const ostring& name, std::vector<T> &values, TypeInfo &typeInfo, BindInfo::BindDirection mode)
{
    BindVector2(OCI_BindArrayOfColls, name, values, mode, static_cast<OCI_TypeInfo *>(typeInfo));
}

template<>
inline void Statement::Register<unsigned short>(const ostring& name)
{
    Check(OCI_RegisterUnsignedShort(*this, name.c_str()));
}

template<>
inline void Statement::Register<short>(const ostring& name)
{
    Check(OCI_RegisterShort(*this, name.c_str()));
}

template<>
inline void Statement::Register<unsigned int>(const ostring& name)
{
    Check(OCI_RegisterUnsignedInt(*this, name.c_str()));
}

template<>
inline void Statement::Register<int>(const ostring& name)
{
    Check(OCI_RegisterInt(*this, name.c_str()));
}

template<>
inline void Statement::Register<big_uint>(const ostring& name)
{
    Check(OCI_RegisterUnsignedBigInt(*this, name.c_str()));
}

template<>
inline void Statement::Register<big_int>(const ostring& name)
{
    Check(OCI_RegisterBigInt(*this, name.c_str()));
}

template<>
inline void Statement::Register<float>(const ostring& name)
{
    Check(OCI_RegisterFloat(*this, name.c_str()));
}

template<>
inline void Statement::Register<double>(const ostring& name)
{
    Check(OCI_RegisterDouble(*this, name.c_str()));
}

template<>
inline void Statement::Register<Number>(const ostring& name)
{
    Check(OCI_RegisterNumber(*this, name.c_str()));
}

template<>
inline void Statement::Register<Date>(const ostring& name)
{
    Check(OCI_RegisterDate(*this, name.c_str()));
}

template<>
inline void Statement::Register<Timestamp, Timestamp::TimestampTypeValues>(const ostring& name, Timestamp::TimestampTypeValues type)
{
    Check(OCI_RegisterTimestamp(*this, name.c_str(), type));
}

template<>
inline void Statement::Register<Timestamp, Timestamp::TimestampType>(const ostring& name, Timestamp::TimestampType type)
{
    Register<Timestamp, Timestamp::TimestampTypeValues>(name, type.GetValue());
}

template<>
inline void Statement::Register<Interval, Interval::IntervalTypeValues>(const ostring& name, Interval::IntervalTypeValues type)
{
    Check(OCI_RegisterInterval(*this, name.c_str(), type));
}

template<>
inline void Statement::Register<Interval, Interval::IntervalType>(const ostring& name, Interval::IntervalType type)
{
    Register<Interval, Interval::IntervalTypeValues>(name, type.GetValue());
}

template<>
inline void Statement::Register<Clob>(const ostring& name)
{
    Check(OCI_RegisterLob(*this, name.c_str(), OCI_CLOB));
}

template<>
inline void Statement::Register<NClob>(const ostring& name)
{
    Check(OCI_RegisterLob(*this, name.c_str(), OCI_NCLOB));
}

template<>
inline void Statement::Register<Blob>(const ostring& name)
{
    Check(OCI_RegisterLob(*this, name.c_str(), OCI_BLOB));
}

template<>
inline void Statement::Register<File>(const ostring& name)
{
    Check(OCI_RegisterFile(*this, name.c_str(), OCI_BFILE));
}

template<>
inline void Statement::Register<Object, TypeInfo>(const ostring& name, TypeInfo& typeInfo)
{
    Check(OCI_RegisterObject(*this, name.c_str(), typeInfo));
}

template<>
inline void Statement::Register<Reference, TypeInfo>(const ostring& name, TypeInfo& typeInfo)
{
    Check(OCI_RegisterRef(*this, name.c_str(), typeInfo));
}

template<>
inline void Statement::Register<ostring, unsigned int>(const ostring& name, unsigned int len)
{
   Check(OCI_RegisterString(*this, name.c_str(), len));
}

template<>
inline void Statement::Register<ostring, int>(const ostring& name, int len)
{
    Register<ostring, unsigned int>(name, static_cast<unsigned int>(len));
}

template<>
inline void Statement::Register<Raw, unsigned int>(const ostring& name, unsigned int len)
{
    Check(OCI_RegisterRaw(*this, name.c_str(), len));
}

template<>
inline void Statement::Register<Raw, int>(const ostring& name, int len)
{
    Register<Raw, unsigned int>(name, static_cast<unsigned int>(len));
}

inline Statement::StatementType Statement::GetStatementType() const
{
    return StatementType(static_cast<StatementType::Type>(Check(OCI_GetStatementType(*this))));
}

inline unsigned int Statement::GetSqlErrorPos() const
{
    return Check(OCI_GetSqlErrorPos(*this));
}

inline void Statement::SetFetchMode(FetchMode value)
{
    Check(OCI_SetFetchMode(*this, value));
}

inline Statement::FetchMode Statement::GetFetchMode() const
{
    return FetchMode(static_cast<FetchMode::Type>(Check(OCI_GetFetchMode(*this))));
}

inline void Statement::SetBindMode(BindMode value)
{
    Check(OCI_SetBindMode(*this, value));
}

inline Statement::BindMode Statement::GetBindMode() const
{
    return BindMode(static_cast<BindMode::Type>(Check(OCI_GetBindMode(*this))));
}

inline void Statement::SetFetchSize(unsigned int value)
{
    Check(OCI_SetFetchSize(*this, value));
}

inline unsigned int Statement::GetFetchSize() const
{
   return Check(OCI_GetFetchSize(*this));
}

inline void Statement::SetPrefetchSize(unsigned int value)
{
    Check(OCI_SetPrefetchSize(*this, value));
}

inline unsigned int Statement::GetPrefetchSize() const
{
    return Check(OCI_GetPrefetchSize(*this));
}

inline void Statement::SetPrefetchMemory(unsigned int value)
{
    Check(OCI_SetPrefetchMemory(*this, value));
}

inline unsigned int Statement::GetPrefetchMemory() const
{
    return Check(OCI_GetPrefetchMemory(*this));
}

inline void Statement::SetLongMaxSize(unsigned int value)
{
    Check(OCI_SetLongMaxSize(*this, value));
}

inline unsigned int Statement::GetLongMaxSize() const
{
    return Check(OCI_GetLongMaxSize(*this));
}

inline void Statement::SetLongMode(LongMode value)
{
    Check(OCI_SetLongMode(*this, value));
}

inline Statement::LongMode Statement::GetLongMode() const
{
    return LongMode(static_cast<LongMode::Type>(Check(OCI_GetLongMode(*this))));
}

inline unsigned int Statement::GetSQLCommand() const
{
    return Check(OCI_GetSQLCommand(*this));
}

inline ostring Statement::GetSQLVerb() const
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

inline void Statement::ClearBinds() const
{
    BindsHolder *bindsHolder = GetBindsHolder(false);

    if (bindsHolder)
    {
         bindsHolder->Clear();
    }
}

inline void Statement::SetOutData() const
{
    BindsHolder *bindsHolder = GetBindsHolder(false);

    if (bindsHolder)
    {
        bindsHolder->SetOutData();
    }
}

inline void Statement::SetInData() const
{
    BindsHolder *bindsHolder = GetBindsHolder(false);

    if (bindsHolder)
    {
        bindsHolder->SetInData();
    }
}

inline void Statement::ReleaseResultsets() const
{
    if (_smartHandle)
    {
        Handle *handle = nullptr;

        while (_smartHandle->GetChildren().FindIf(IsResultsetHandle, handle))
        {
            if (handle)
            {
                handle->DetachFromHolders();

                delete handle;

                handle = nullptr;
            }
        }
    }
}

inline bool Statement::IsResultsetHandle(Handle *handle)
{
    Resultset::SmartHandle *smartHandle = dynamic_cast<Resultset::SmartHandle *>(handle);

    return smartHandle != nullptr;
}

inline void Statement::OnFreeSmartHandle(SmartHandle *smartHandle)
{
    if (smartHandle)
    {
        BindsHolder *bindsHolder = static_cast<BindsHolder *>(smartHandle->GetExtraInfos());

        smartHandle->SetExtraInfos(nullptr);

        delete bindsHolder;
    }
}

inline void Statement::SetLastBindMode(BindInfo::BindDirection mode)
{
    Check(OCI_BindSetDirection(Check(OCI_GetBind(*this, Check(OCI_GetBindCount(*this)))),  mode));
}

inline BindsHolder * Statement::GetBindsHolder(bool create) const
{
    BindsHolder * bindsHolder = static_cast<BindsHolder *>(_smartHandle->GetExtraInfos());

    if (bindsHolder == nullptr && create)
    {
        bindsHolder = new BindsHolder(*this);
        _smartHandle->SetExtraInfos(bindsHolder);
    }

    return bindsHolder;
}

/* --------------------------------------------------------------------------------------------- *
 * Resultset
 * --------------------------------------------------------------------------------------------- */

inline Resultset::Resultset(OCI_Resultset *resultset, Handle *parent)
{
    Acquire(resultset, nullptr, nullptr, parent);
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

inline bool Resultset::Seek(SeekMode mode, int offset)
{
    return (Check(OCI_FetchSeek(*this, mode, offset)) == TRUE);
}

inline unsigned int Resultset::GetCount() const
{
    return Check(OCI_GetRowCount(*this));
}

inline unsigned int Resultset::GetCurrentRow() const
{
    return Check(OCI_GetCurrentRow(*this));
}

inline unsigned int Resultset::GetColumnIndex(const ostring& name) const
{
    return Check(OCI_GetColumnIndex(*this, name.c_str()));
}

inline unsigned int Resultset::GetColumnCount() const
{
    return Check(OCI_GetColumnCount(*this));
}

inline Column Resultset::GetColumn(unsigned int index) const
{
    return Column(Check(OCI_GetColumn(*this, index)), GetHandle());
}

inline Column Resultset::GetColumn(const ostring& name) const
{
    return Column(Check(OCI_GetColumn2(*this, name.c_str())), GetHandle());
}

inline bool Resultset::IsColumnNull(unsigned int index) const
{
    return (Check(OCI_IsNull(*this, index)) == TRUE);
}

inline bool Resultset::IsColumnNull(const ostring& name) const
{
    return (Check(OCI_IsNull2(*this, name.c_str())) == TRUE);
}

inline Statement Resultset::GetStatement() const
{
    return Statement( Check(OCI_ResultsetGetStatement(*this)), nullptr);
}

inline bool Resultset::operator ++ (int)
{
    return Next();
}

inline bool Resultset::operator -- (int)
{
    return Prev();
}

inline bool Resultset::operator += (int offset)
{
    return Seek(SeekRelative, offset);
}

inline bool Resultset::operator -= (int offset)
{
    return Seek(SeekRelative, -offset);
}

template<class T>
void Resultset::Get(unsigned int index, T& value) const
{
    value = Get<T>(index);
}

template<class T>
void Resultset::Get(const ostring &name, T& value) const
{
    value = Get<T>(name);
}

template<class T, class TAdapter>
bool Resultset::Get(T& value, TAdapter adapter) const
{
    return adapter(static_cast<const Resultset&>(*this), value);
}

template<class TCallback>
unsigned int Resultset::ForEach(TCallback callback)
{
    while (Next())
    {
        if (!callback(static_cast<const Resultset&>(*this)))
        {
            break;
        }
    }

    return GetCurrentRow();
}

template<class T, class U>
unsigned int Resultset::ForEach(T callback, U adapter)
{
    while (Next())
    {
        if (!callback(adapter(static_cast<const Resultset&>(*this))))
        {
            break;
        }
    }

    return GetCurrentRow();
}

template<>
inline short Resultset::Get<short>(unsigned int index) const
{
    return Check(OCI_GetShort(*this, index));
}

template<>
inline short Resultset::Get<short>(const ostring& name) const
{
    return Check(OCI_GetShort2(*this, name.c_str()));
}

template<>
inline unsigned short Resultset::Get<unsigned short>(unsigned int index) const
{
    return Check(OCI_GetUnsignedShort(*this, index));
}

template<>
inline unsigned short Resultset::Get<unsigned short>(const ostring& name) const
{
    return Check(OCI_GetUnsignedShort2(*this, name.c_str()));
}

template<>
inline int Resultset::Get<int>(unsigned int index) const
{
    return Check(OCI_GetInt(*this, index));
}

template<>
inline int Resultset::Get<int>(const ostring& name) const
{
    return Check(OCI_GetInt2(*this, name.c_str()));
}

template<>
inline unsigned int Resultset::Get<unsigned int>(unsigned int index) const
{
    return Check(OCI_GetUnsignedInt(*this, index));
}

template<>
inline unsigned int Resultset::Get<unsigned int>(const ostring& name) const
{
    return Check(OCI_GetUnsignedInt2(*this, name.c_str()));
}

template<>
inline big_int Resultset::Get<big_int>(unsigned int index) const
{
    return Check(OCI_GetBigInt(*this, index));
}

template<>
inline big_int Resultset::Get<big_int>(const ostring& name) const
{
    return Check(OCI_GetBigInt2(*this, name.c_str()));
}

template<>
inline big_uint Resultset::Get<big_uint>(unsigned int index) const
{
    return Check(OCI_GetUnsignedBigInt(*this, index));
}

template<>
inline big_uint Resultset::Get<big_uint>(const ostring& name) const
{
    return Check(OCI_GetUnsignedBigInt2(*this, name.c_str()));
}

template<>
inline float Resultset::Get<float>(unsigned int index) const
{
    return Check(OCI_GetFloat(*this, index));
}

template<>
inline float Resultset::Get<float>(const ostring& name) const
{
    return Check(OCI_GetFloat2(*this, name.c_str()));
}

template<>
inline double Resultset::Get<double>(unsigned int index) const
{
    return Check(OCI_GetDouble(*this, index));
}

template<>
inline double Resultset::Get<double>(const ostring& name) const
{
    return Check(OCI_GetDouble2(*this, name.c_str()));
}

template<>
inline Number Resultset::Get<Number>(unsigned int index) const
{
    return Number(Check(OCI_GetNumber(*this, index)), GetHandle());
}

template<>
inline Number Resultset::Get<Number>(const ostring& name) const
{
    return Number(Check(OCI_GetNumber2(*this, name.c_str())), GetHandle());
}

template<>
inline ostring Resultset::Get<ostring>(unsigned int index) const
{
    return MakeString(Check(OCI_GetString(*this, index)));
}

template<>
inline ostring Resultset::Get<ostring>(const ostring& name) const
{
    return MakeString(Check(OCI_GetString2(*this,name.c_str())));
}

template<>
inline Raw Resultset::Get<Raw>(unsigned int index) const
{
    unsigned int size = Check(OCI_GetDataLength(*this,index));

    ManagedBuffer<unsigned char> buffer(size + 1);

    size = Check(OCI_GetRaw(*this, index, static_cast<AnyPointer>(buffer), size));

    return MakeRaw(buffer, size);
}

template<>
inline Raw Resultset::Get<Raw>(const ostring& name) const
{
    unsigned int size = Check(OCI_GetDataLength(*this, Check(OCI_GetColumnIndex(*this, name.c_str()))));

    ManagedBuffer<unsigned char> buffer(size + 1);

    size = Check(OCI_GetRaw2(*this, name.c_str(), static_cast<AnyPointer>(buffer), size));

    return MakeRaw(buffer, size);
}

template<>
inline Date Resultset::Get<Date>(unsigned int index) const
{
    return Date(Check(OCI_GetDate(*this, index)), GetHandle());
}

template<>
inline Date Resultset::Get<Date>(const ostring& name) const
{
    return Date(Check(OCI_GetDate2(*this,name.c_str())), GetHandle());
}

template<>
inline Timestamp Resultset::Get<Timestamp>(unsigned int index) const
{
    return Timestamp(Check(OCI_GetTimestamp(*this, index)), GetHandle());
}

template<>
inline Timestamp Resultset::Get<Timestamp>(const ostring& name) const
{
    return Timestamp(Check(OCI_GetTimestamp2(*this,name.c_str())), GetHandle());
}

template<>
inline Interval Resultset::Get<Interval>(unsigned int index) const
{
    return Interval(Check(OCI_GetInterval(*this, index)), GetHandle());
}

template<>
inline Interval Resultset::Get<Interval>(const ostring& name) const
{
    return Interval(Check(OCI_GetInterval2(*this,name.c_str())), GetHandle());
}

template<>
inline Object Resultset::Get<Object>(unsigned int index) const
{
    return Object(Check(OCI_GetObject(*this, index)), GetHandle());
}

template<>
inline Object Resultset::Get<Object>(const ostring& name) const
{
    return Object(Check(OCI_GetObject2(*this,name.c_str())), GetHandle());
}

template<>
inline Reference Resultset::Get<Reference>(unsigned int index) const
{
    return Reference(Check(OCI_GetRef(*this, index)), GetHandle());
}

template<>
inline Reference Resultset::Get<Reference>(const ostring& name) const
{
    return Reference(Check(OCI_GetRef2(*this,name.c_str())), GetHandle());
}

template<>
inline Statement Resultset::Get<Statement>(unsigned int index) const
{
    return Statement(Check(OCI_GetStatement(*this, index)), GetHandle());
}

template<>
inline Statement Resultset::Get<Statement>(const ostring& name) const
{
    return Statement(Check(OCI_GetStatement2(*this,name.c_str())), GetHandle());
}

template<>
inline Clob Resultset::Get<Clob>(unsigned int index) const
{
    return Clob(Check(OCI_GetLob(*this, index)), GetHandle());
}

template<>
inline Clob Resultset::Get<Clob>(const ostring& name) const
{
    return Clob(Check(OCI_GetLob2(*this,name.c_str())), GetHandle());
}

template<>
inline NClob Resultset::Get<NClob>(unsigned int index) const
{
    return NClob(Check(OCI_GetLob(*this, index)), GetHandle());
}

template<>
inline NClob Resultset::Get<NClob>(const ostring& name) const
{
    return NClob(Check(OCI_GetLob2(*this, name.c_str())), GetHandle());
}

template<>
inline Blob Resultset::Get<Blob>(unsigned int index) const
{
    return Blob(Check(OCI_GetLob(*this, index)), GetHandle());
}

template<>
inline Blob Resultset::Get<Blob>(const ostring& name) const
{
    return Blob(Check(OCI_GetLob2(*this,name.c_str())), GetHandle());
}

template<>
inline File Resultset::Get<File>(unsigned int index) const
{
    return File(Check(OCI_GetFile(*this, index)), GetHandle());
}

template<>
inline File Resultset::Get<File>(const ostring& name) const
{
    return File(Check(OCI_GetFile2(*this,name.c_str())), GetHandle());
}

template<>
inline Clong Resultset::Get<Clong>(unsigned int index) const
{
    return Clong(Check(OCI_GetLong(*this, index)), GetHandle());
}

template<>
inline Clong Resultset::Get<Clong>(const ostring& name) const
{
    return Clong(Check(OCI_GetLong2(*this,name.c_str())), GetHandle());
}

template<>
inline Blong Resultset::Get<Blong>(unsigned int index) const
{
    return Blong(Check(OCI_GetLong(*this, index)), GetHandle());
}

template<>
inline Blong Resultset::Get<Blong>(const ostring& name) const
{
    return Blong(Check(OCI_GetLong2(*this,name.c_str())), GetHandle());
}

template<class T>
T Resultset::Get(unsigned int index) const
{
    return T(Check(OCI_GetColl(*this, index)), GetHandle());
}

template<class T>
T Resultset::Get(const ostring& name) const
{
    return T(Check(OCI_GetColl2(*this, name.c_str())), GetHandle());
}

/* --------------------------------------------------------------------------------------------- *
 * Column
 * --------------------------------------------------------------------------------------------- */

inline Column::Column(OCI_Column *pColumn, Handle *parent)
{
    Acquire(pColumn, nullptr, nullptr, parent);
}

inline ostring Column::GetName() const
{
    return MakeString(Check(OCI_ColumnGetName(*this)));
}

inline ostring Column::GetSQLType() const
{
    return MakeString(Check(OCI_ColumnGetSQLType(*this)));
}

inline ostring Column::GetFullSQLType() const
{
    unsigned int size = OCI_SIZE_BUFFER;

    ManagedBuffer<otext> buffer(size + 1);

    Check(OCI_ColumnGetFullSQLType(*this, buffer, size));

    return MakeString(static_cast<const otext *>(buffer));
}

inline DataType Column::GetType() const
{
    return DataType(static_cast<DataType::Type>(Check(OCI_ColumnGetType(*this))));
}

inline unsigned int Column::GetSubType() const
{
    return Check(OCI_ColumnGetSubType(*this));
}

inline CharsetForm Column::GetCharsetForm() const
{
    return CharsetForm(static_cast<CharsetForm::Type>(Check(OCI_ColumnGetCharsetForm(*this))));
}

inline CollationID Column::GetCollationID() const
{
    return CollationID(static_cast<CollationID::Type>(Check(OCI_ColumnGetCollationID(*this))));
}

inline unsigned int Column::GetSize() const
{
    return Check(OCI_ColumnGetSize(*this));
}

inline int Column::GetScale() const
{
    return Check(OCI_ColumnGetScale(*this));
}

inline int Column::GetPrecision() const
{
    return Check(OCI_ColumnGetPrecision(*this));
}

inline int Column::GetFractionalPrecision() const
{
    return Check(OCI_ColumnGetFractionalPrecision(*this));
}

inline int Column::GetLeadingPrecision() const
{
    return Check(OCI_ColumnGetLeadingPrecision(*this));
}

inline Column::PropertyFlags Column::GetPropertyFlags() const
{
    return PropertyFlags(static_cast<PropertyFlags::Type>(Check(OCI_ColumnGetPropertyFlags(*this))));
}

inline bool Column::IsNullable() const
{
    return (Check(OCI_ColumnGetNullable(*this)) == TRUE);
}

inline bool Column::IsCharSemanticUsed() const
{
    return (Check(OCI_ColumnGetCharUsed(*this)) == TRUE);
}

inline TypeInfo Column::GetTypeInfo() const
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
    Acquire(pSubcription, nullptr, nullptr, nullptr);
}

inline void Subscription::Register(const Connection &connection, const ostring& name, ChangeTypes changeTypes, NotifyHandlerProc handler, unsigned int port, unsigned int timeout)
{
    Acquire(Check(OCI_SubscriptionRegister(connection, name.c_str(), changeTypes.GetValues(),
        static_cast<POCI_NOTIFY> (handler != nullptr ? Environment::NotifyHandler : nullptr), port, timeout)),
                                           reinterpret_cast<HandleFreeFunc>(OCI_SubscriptionUnregister), nullptr, nullptr);

    Environment::SetUserCallback<Subscription::NotifyHandlerProc>(static_cast<OCI_Subscription*>(*this), handler);
}

inline void Subscription::Unregister()
{
    Environment::SetUserCallback<Subscription::NotifyHandlerProc>(static_cast<OCI_Subscription*>(*this), nullptr);

    Release();
}

inline void Subscription::Watch(const ostring& sql)
{
    Statement st(GetConnection());

    st.Execute(sql);

    Check(OCI_SubscriptionAddStatement(*this, st));
}

inline ostring Subscription::GetName() const
{
    return MakeString(Check(OCI_SubscriptionGetName(*this)));
}

inline unsigned int Subscription::GetTimeout() const
{
    return Check(OCI_SubscriptionGetTimeout(*this));
}

inline unsigned int Subscription::GetPort() const
{
    return Check(OCI_SubscriptionGetPort(*this));
}

inline Connection Subscription::GetConnection() const
{
    return Connection(Check(OCI_SubscriptionGetConnection(*this)), nullptr);
}

/* --------------------------------------------------------------------------------------------- *
 * Event
 * --------------------------------------------------------------------------------------------- */

inline Event::Event(OCI_Event *pEvent)
{
    Acquire(pEvent, nullptr, nullptr, nullptr);
}

inline Event::EventType Event::GetType() const
{
    return EventType(static_cast<EventType::Type>(Check(OCI_EventGetType(*this))));
}

inline Event::ObjectEvent Event::GetObjectEvent() const
{
    return ObjectEvent(static_cast<ObjectEvent::Type>(Check(OCI_EventGetOperation(*this))));
}

inline ostring Event::GetDatabaseName() const
{
    return MakeString(Check(OCI_EventGetDatabase(*this)));
}

inline ostring Event::GetObjectName() const
{
    return MakeString(Check(OCI_EventGetObject(*this)));
}

inline ostring Event::GetRowID() const
{
    return MakeString(Check(OCI_EventGetRowid(*this)));
}

inline Subscription Event::GetSubscription() const
{
    return Subscription(Check(OCI_EventGetSubscription(*this)));
}

/* --------------------------------------------------------------------------------------------- *
 * Agent
 * --------------------------------------------------------------------------------------------- */

inline Agent::Agent(const Connection &connection, const ostring& name, const ostring& address)
{
    Acquire(Check(OCI_AgentCreate(connection, name.c_str(), address.c_str())), reinterpret_cast<HandleFreeFunc>(OCI_AgentFree), nullptr, nullptr);
}

inline Agent::Agent(OCI_Agent *pAgent, Handle *parent)
{
    Acquire(pAgent, nullptr, nullptr, parent);
}

inline ostring Agent::GetName() const
{
    return MakeString(Check(OCI_AgentGetName(*this)));
}

inline void Agent::SetName(const ostring& value)
{
    Check(OCI_AgentSetName(*this, value.c_str()));
}

inline ostring Agent::GetAddress() const
{
    return MakeString(Check(OCI_AgentGetAddress(*this)));
}

inline void Agent::SetAddress(const ostring& value)
{
    Check(OCI_AgentSetAddress(*this, value.c_str()));
}

/* --------------------------------------------------------------------------------------------- *
 * Message
 * --------------------------------------------------------------------------------------------- */

inline Message::Message(const TypeInfo &typeInfo)
{
    Acquire(Check(OCI_MsgCreate(typeInfo)), reinterpret_cast<HandleFreeFunc>(OCI_MsgFree), nullptr, nullptr);
}

inline Message::Message(OCI_Msg *pMessage, Handle *parent)
{
    Acquire(pMessage, nullptr, nullptr, parent);
}

inline void Message::Reset()
{
    Check(OCI_MsgReset(*this));
}

template<>
inline Object Message::GetPayload<Object>()
{
    return Object(Check(OCI_MsgGetObject(*this)), nullptr);
}

template<>
inline void Message::SetPayload<Object>(const Object &value)
{
    Check(OCI_MsgSetObject(*this, value));
}

template<>
inline Raw Message::GetPayload<Raw>()
{
    unsigned int size = 0;

    ManagedBuffer<unsigned char> buffer(size + 1);

    Check(OCI_MsgGetRaw(*this, static_cast<AnyPointer>(buffer), &size));

    return MakeRaw(buffer, size);
}

template<>
inline void Message::SetPayload<Raw>(const Raw &value)
{
    if (value.size() > 0)
    {
        Check(OCI_MsgSetRaw(*this, static_cast<AnyPointer>(const_cast<Raw::value_type *>(&value[0])), static_cast<unsigned int>(value.size())));
    }
    else
    {
        Check(OCI_MsgSetRaw(*this, nullptr, 0));
    }
}

inline Date Message::GetEnqueueTime() const
{
    return Date(Check(OCI_MsgGetEnqueueTime(*this)), nullptr);
}

inline int Message::GetAttemptCount() const
{
    return Check(OCI_MsgGetAttemptCount(*this));
}

inline Message::MessageState Message::GetState() const
{
    return MessageState(static_cast<MessageState::Type>(Check(OCI_MsgGetState(*this))));
}

inline Raw Message::GetID() const
{
    unsigned int size = OCI_SIZE_BUFFER;

    ManagedBuffer<unsigned char> buffer(size + 1);

    Check(OCI_MsgGetID(*this, static_cast<AnyPointer>(buffer), &size));

    return MakeRaw(buffer, size);
}

inline int Message::GetExpiration() const
{
    return Check(OCI_MsgGetExpiration(*this));
}

inline void Message::SetExpiration(int value)
{
    Check(OCI_MsgSetExpiration(*this, value));
}

inline int Message::GetEnqueueDelay() const
{
    return Check(OCI_MsgGetEnqueueDelay(*this));
}

inline void Message::SetEnqueueDelay(int value)
{
    Check(OCI_MsgSetEnqueueDelay(*this, value));
}

inline int Message::GetPriority() const
{
    return Check(OCI_MsgGetPriority(*this));
}

inline void Message::SetPriority(int value)
{
    Check(OCI_MsgSetPriority(*this, value));
}

inline Raw Message::GetOriginalID() const
{
    unsigned int size = OCI_SIZE_BUFFER;

    ManagedBuffer<unsigned char> buffer(size + 1);

    Check(OCI_MsgGetOriginalID(*this, static_cast<AnyPointer>(buffer), &size));

    return MakeRaw(buffer, size);
}

inline void Message::SetOriginalID(const Raw &value)
{
    if (value.size() > 0)
    {
        Check(OCI_MsgSetOriginalID(*this, static_cast<AnyPointer>(const_cast<Raw::value_type *>(&value[0])), static_cast<unsigned int>(value.size())));
    }
    else
    {
        Check(OCI_MsgSetOriginalID(*this, nullptr, 0));
    }
}

inline ostring Message::GetCorrelation() const
{
    return MakeString(Check(OCI_MsgGetCorrelation(*this)));
}

inline void Message::SetCorrelation(const ostring& value)
{
    Check(OCI_MsgSetCorrelation(*this, value.c_str()));
}

inline ostring Message::GetExceptionQueue() const
{
    return MakeString(Check(OCI_MsgGetExceptionQueue(*this)));
}

inline void Message::SetExceptionQueue(const ostring& value)
{
    Check(OCI_MsgSetExceptionQueue(*this, value.c_str()));
}

inline Agent Message::GetSender() const
{
    return Agent(Check(OCI_MsgGetSender(*this)), nullptr);
}

inline void Message::SetSender(const Agent &agent)
{
    Check(OCI_MsgSetSender(*this, agent));
}

inline void Message::SetConsumers(std::vector<Agent> &agents)
{
    size_t size = agents.size();
    ManagedBuffer<OCI_Agent*> buffer(size);

    OCI_Agent ** pAgents = static_cast<OCI_Agent **>(buffer);

    for (size_t i = 0; i < size; ++i)
    {
        pAgents[i] = static_cast<const Agent &>(agents[i]);
    }

    Check(OCI_MsgSetConsumers(*this, pAgents, static_cast<unsigned int>(size)));
}

/* --------------------------------------------------------------------------------------------- *
 * Enqueue
 * --------------------------------------------------------------------------------------------- */

inline Enqueue::Enqueue(const TypeInfo &typeInfo, const ostring& queueName)
{
    Acquire(Check(OCI_EnqueueCreate(typeInfo, queueName.c_str())), reinterpret_cast<HandleFreeFunc>(OCI_EnqueueFree), nullptr, nullptr);
}

inline void Enqueue::Put(const Message &message)
{
    Check(OCI_EnqueuePut(*this, message));
}

inline Enqueue::EnqueueVisibility Enqueue::GetVisibility() const
{
    return EnqueueVisibility(static_cast<EnqueueVisibility::Type>(Check(OCI_EnqueueGetVisibility(*this))));
}

inline void Enqueue::SetVisibility(EnqueueVisibility value)
{
    Check(OCI_EnqueueSetVisibility(*this, value));
}

inline Enqueue::EnqueueMode Enqueue::GetMode() const
{
    return EnqueueMode(static_cast<EnqueueMode::Type>(Check(OCI_EnqueueGetSequenceDeviation(*this))));
}

inline void Enqueue::SetMode(EnqueueMode value)
{
    Check(OCI_EnqueueSetSequenceDeviation(*this, value));
}

inline Raw Enqueue::GetRelativeMsgID() const
{
    unsigned int size = OCI_SIZE_BUFFER;

    ManagedBuffer<unsigned char> buffer(size + 1);

    Check(OCI_EnqueueGetRelativeMsgID(*this, static_cast<AnyPointer>(buffer), &size));

    return MakeRaw(buffer, size);
}

inline void Enqueue::SetRelativeMsgID(const Raw &value)
{
    if (value.size() > 0)
    {
        Check(OCI_EnqueueSetRelativeMsgID(*this, static_cast<AnyPointer>(const_cast<Raw::value_type *>(&value[0])), static_cast<unsigned int>(value.size())));
    }
    else
    {
        Check(OCI_EnqueueSetRelativeMsgID(*this, nullptr, 0));
    }
}

/* --------------------------------------------------------------------------------------------- *
 * Dequeue
 * --------------------------------------------------------------------------------------------- */

inline Dequeue::Dequeue(const TypeInfo &typeInfo, const ostring& queueName)
{
    Acquire(Check(OCI_DequeueCreate(typeInfo, queueName.c_str())), reinterpret_cast<HandleFreeFunc>(OCI_DequeueFree), nullptr, nullptr);
}

inline Dequeue::Dequeue(OCI_Dequeue *pDequeue)
{
    Acquire(pDequeue, nullptr, nullptr, nullptr);
}

inline Message Dequeue::Get()
{
    return Message(Check(OCI_DequeueGet(*this)), nullptr);
}

inline Agent Dequeue::Listen(int timeout)
{
    return Agent(Check(OCI_DequeueListen(*this, timeout)), nullptr);
}

inline ostring Dequeue::GetConsumer() const
{
    return MakeString(Check(OCI_DequeueGetConsumer(*this)));
}

inline void Dequeue::SetConsumer(const ostring& value)
{
    Check(OCI_DequeueSetConsumer(*this, value.c_str()));
}

inline ostring Dequeue::GetCorrelation() const
{
    return MakeString(Check(OCI_DequeueGetCorrelation(*this)));
}

inline void Dequeue::SetCorrelation(const ostring& value)
{
    Check(OCI_DequeueSetCorrelation(*this, value.c_str()));
}

inline Raw Dequeue::GetRelativeMsgID() const
{
    unsigned int size = OCI_SIZE_BUFFER;

    ManagedBuffer<unsigned char> buffer(size + 1);

    Check(OCI_DequeueGetRelativeMsgID(*this, static_cast<AnyPointer>(buffer), &size));

    return MakeRaw(buffer, size);
}

inline void Dequeue::SetRelativeMsgID(const Raw &value)
{
    if (value.size() > 0)
    {
        Check(OCI_DequeueSetRelativeMsgID(*this, static_cast<AnyPointer>(const_cast<Raw::value_type *>(&value[0])), static_cast<unsigned int>(value.size())));
    }
    else
    {
        Check(OCI_DequeueSetRelativeMsgID(*this, nullptr, 0));
    }
}

inline Dequeue::DequeueVisibility Dequeue::GetVisibility() const
{
    return DequeueVisibility(static_cast<DequeueVisibility::Type>(Check(OCI_DequeueGetVisibility(*this))));
}

inline void Dequeue::SetVisibility(DequeueVisibility value)
{
    Check(OCI_DequeueSetVisibility(*this, value));
}

inline Dequeue::DequeueMode Dequeue::GetMode() const
{
    return DequeueMode(static_cast<DequeueMode::Type>(Check(OCI_DequeueGetMode(*this))));
}

inline void Dequeue::SetMode(DequeueMode value)
{
    Check(OCI_DequeueSetMode(*this, value));
}

inline Dequeue::NavigationMode Dequeue::GetNavigation() const
{
    return NavigationMode(static_cast<NavigationMode::Type>(Check(OCI_DequeueGetNavigation(*this))));
}

inline void Dequeue::SetNavigation(NavigationMode value)
{
    Check(OCI_DequeueSetNavigation(*this, value));
}

inline int Dequeue::GetWaitTime() const
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
    ManagedBuffer<OCI_Agent*> buffer(size);

    OCI_Agent ** pAgents = static_cast<OCI_Agent **>(buffer);

    for (size_t i = 0; i < size; ++i)
    {
        pAgents[i] = static_cast<const Agent &>(agents[i]);
    }

    Check(OCI_DequeueSetAgentList(*this, pAgents, static_cast<unsigned int>(size)));
}

inline void Dequeue::Subscribe(unsigned int port, unsigned int timeout, NotifyAQHandlerProc handler)
{
    Check(OCI_DequeueSubscribe(*this, port, timeout, static_cast<POCI_NOTIFY_AQ>(handler != nullptr ? Environment::NotifyHandlerAQ : nullptr)));

    Environment::SetUserCallback<NotifyAQHandlerProc>(static_cast<OCI_Dequeue*>(*this), handler);
}

inline void Dequeue::Unsubscribe()
{
    Check(OCI_DequeueUnsubscribe(*this));
}

/* --------------------------------------------------------------------------------------------- *
 * DirectPath
 * --------------------------------------------------------------------------------------------- */

inline DirectPath::DirectPath(const TypeInfo &typeInfo, unsigned int nbCols, unsigned int  nbRows, const ostring& partition)
{
    Acquire(Check(OCI_DirPathCreate(typeInfo, partition.c_str(), nbCols, nbRows)), reinterpret_cast<HandleFreeFunc>(OCI_DirPathFree), nullptr, nullptr);
}

inline void DirectPath::SetColumn(unsigned int colIndex, const ostring& name, unsigned int maxSize,  const ostring& format)
{
    Check(OCI_DirPathSetColumn(*this, colIndex, name.c_str(), maxSize, format.c_str()));
}

template<class T>
inline void DirectPath::SetEntry(unsigned int rowIndex, unsigned int colIndex, const T &value, bool complete)
{
    Check(OCI_DirPathSetEntry(*this, rowIndex, colIndex, static_cast<const AnyPointer>(const_cast<typename T::value_type *>(value.c_str())), static_cast<unsigned int>(value.size()), complete));
}

inline void DirectPath::Reset()
{
    Check(OCI_DirPathReset(*this));
}

inline void DirectPath::Prepare()
{
    Check(OCI_DirPathPrepare(*this));
}

inline DirectPath::Result DirectPath::Convert()
{
    return Result(static_cast<Result::Type>(Check(OCI_DirPathConvert(*this))));
}

inline DirectPath::Result DirectPath::Load()
{
    return Result(static_cast<Result::Type>(Check(OCI_DirPathLoad(*this))));
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

inline unsigned int DirectPath::GetCurrentRows() const
{
    return Check(OCI_DirPathGetCurrentRows(*this));
}

inline unsigned int DirectPath::GetMaxRows() const
{
    return Check(OCI_DirPathGetMaxRows(*this));
}

inline unsigned int DirectPath::GetRowCount() const
{
    return Check(OCI_DirPathGetRowCount(*this));
}

inline unsigned int DirectPath::GetAffectedRows() const
{
    return Check(OCI_DirPathGetAffectedRows(*this));
}

inline void DirectPath::SetDateFormat(const ostring& format)
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

inline void DirectPath::SetConversionMode(ConversionMode value)
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

inline void Queue::Create(const Connection &connection, const ostring& queue, const ostring& table, QueueType queueType, unsigned int maxRetries,
            unsigned int retryDelay, unsigned int retentionTime, bool dependencyTracking, const ostring& comment)
{
    Check(OCI_QueueCreate(connection, queue.c_str(), table.c_str(), queueType, maxRetries, retryDelay, retentionTime, dependencyTracking, comment.c_str()));
}

inline void Queue::Alter(const Connection &connection, const ostring& queue, unsigned int maxRetries, unsigned int retryDelay, unsigned int retentionTime, const ostring& comment)
{
    Check(OCI_QueueAlter(connection, queue.c_str(), maxRetries, retryDelay, retentionTime, comment.c_str()));
}

inline void Queue::Drop(const Connection &connection, const ostring& queue)
{
    Check(OCI_QueueDrop(connection, queue.c_str()));
}

inline void Queue::Start(const Connection &connection, const ostring& queue, bool enableEnqueue, bool enableDequeue)
{
    Check(OCI_QueueStart(connection, queue.c_str(), enableEnqueue, enableDequeue));
}

inline void Queue::Stop(const Connection &connection, const ostring& queue, bool stopEnqueue, bool stopDequeue, bool wait)
{
    Check(OCI_QueueStop(connection, queue.c_str(), stopEnqueue, stopDequeue, wait));
}

/* --------------------------------------------------------------------------------------------- *
 * QueueTable
 * --------------------------------------------------------------------------------------------- */

inline void QueueTable::Create(const Connection &connection, const ostring& table, const ostring& payloadType, bool multipleConsumers, const ostring& storageClause, const ostring& sortList,
                   GroupingMode groupingMode, const ostring& comment, unsigned int primaryInstance, unsigned int secondaryInstance, const ostring& compatible)

{
    Check(OCI_QueueTableCreate(connection, table.c_str(), payloadType.c_str(), storageClause.c_str(), sortList.c_str(), multipleConsumers,
                               groupingMode, comment.c_str(), primaryInstance, secondaryInstance, compatible.c_str()));
}

inline void QueueTable::Alter(const Connection &connection, const ostring& table, const ostring& comment, unsigned int primaryInstance, unsigned int secondaryInstance)
{
    Check(OCI_QueueTableAlter(connection, table.c_str(), comment.c_str(), primaryInstance, secondaryInstance));
}

inline void QueueTable::Drop(const Connection &connection, const ostring& table, bool force)
{
    Check(OCI_QueueTableDrop(connection, table.c_str(), force));
}

inline void QueueTable::Purge(const Connection &connection, const ostring& table, PurgeMode mode, const ostring& condition, bool block)
{
    Check(OCI_QueueTablePurge(connection, table.c_str(), condition.c_str(), block, static_cast<unsigned int>(mode)));
}

inline void QueueTable::Migrate(const Connection &connection, const ostring& table, const ostring& compatible)
{
    Check(OCI_QueueTableMigrate(connection, table.c_str(), compatible.c_str()));
}

/**
 * @}
 */

}
