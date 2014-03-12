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
 * $Id: ocilib_impl.hpp, Vincent Rogier $
 * --------------------------------------------------------------------------------------------- */

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
inline ManagedBuffer<TBufferType>::operator TBufferType* () const
{
    return  _buffer;
}

template< typename TBufferType>
inline ManagedBuffer<TBufferType>::operator const TBufferType* () const
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
inline bool HandleHolder<THandleType>::IsNull() const
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
inline HandleHolder<THandleType>::operator bool() const
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
inline TValue ConcurrentPool<TKey, TValue>::Get(TKey key) const
{
    TValue value = 0;

    Lock();
    typename ConcurrentPoolMap::const_iterator it = _map.find(key);
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
inline void ConcurrentPool<TKey, TValue>::Lock() const
{
    if (_mutex)
    {
        Mutex::Acquire(_mutex);
    }
}

template <class TKey, class TValue>
inline void ConcurrentPool<TKey, TValue>::Unlock() const
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
        Handle *handle = *it;

        handle->DetachFromParent();
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
inline bool HandleHolder<THandleType>::SmartHandle<TSmartHandleType>::IsLastHolder(HandleHolder<TSmartHandleType> *holder) const
{
    return ((_holders.size() == 1) && (*(_holders.begin()) == holder));
}

template <class THandleType>
template <class TSmartHandleType>
inline TSmartHandleType HandleHolder<THandleType>::SmartHandle<TSmartHandleType>::GetHandle() const
{
    return _handle;
}

template <class THandleType>
template <class TSmartHandleType>
inline void * HandleHolder<THandleType>::SmartHandle<TSmartHandleType>::GetExtraInfos() const
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
inline std::list<Handle *> & HandleHolder<THandleType>::SmartHandle<TSmartHandleType>::GetChildren()
{
    return _children;
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
inline void HandleHolder<THandleType>::SmartHandle<TSmartHandleType>::DetachFromParent()
{
    _parent = 0;
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

inline ostring Exception::GetMessage() const
{
    return MakeString(OCI_ErrorGetString(*this));
}

inline Exception::ExceptionType Exception::GetType() const
{
    return (Exception::ExceptionType) OCI_ErrorGetType(*this);
}

inline int Exception::GetOracleErrorCode() const
{
    return OCI_ErrorGetOCICode(*this);
}

inline int Exception::GetInternalErrorCode() const
{
    return OCI_ErrorGetInternalCode(*this);
}

inline Statement Exception::GetStatement() const
{
    return Statement(OCI_ErrorGetStatement(*this), 0);
}

inline Connection Exception::GetConnection() const
{
    return Connection(OCI_ErrorGetConnection(*this), 0);
}

inline unsigned int Exception::GetRow() const
{
    return OCI_ErrorGetRow(*this);
}

/* --------------------------------------------------------------------------------------------- *
 * Environment
 * --------------------------------------------------------------------------------------------- */

inline void Environment::Initialize(Environment::EnvMode mode, ostring libpath)
{
    unsigned int ociMode =  (unsigned int) mode;

    ociMode |= OCI_ENV_CONTEXT;

    OCI_Initialize(0, libpath.c_str(),  ociMode);

    Check();

    GetEnvironmentHandle().Initialize((void *) OCI_HandleGetEnvironment(), ociMode);
}

inline void Environment::Cleanup()
{
    GetEnvironmentHandle().Finalize();

    OCI_Cleanup();

    Check();
}

inline Environment::EnvMode Environment::GetMode()
{
    return (Environment::EnvMode) GetEnvironmentHandle().Mode;
}

inline Environment::ImportMode Environment::GetImportMode()
{
    return (Environment::ImportMode) Check(OCI_GetImportMode());
}

inline Environment::Charset Environment::GetCharset()
{
    return (Environment::Charset) Check(OCI_GetCharset());
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

inline void Environment::StartDatabase(ostring db, ostring user, ostring pwd, Environment::StartFlag startFlags,
                                       Environment::StartMode startMode, Environment::SessionMode sessionMode, ostring spfile)
{
    Check(OCI_DatabaseStartup(db.c_str(), user.c_str(), pwd.c_str(), (unsigned int) sessionMode,
                              (unsigned int) startMode, (unsigned int) startFlags, spfile.c_str() ));
}

inline void Environment::ShutdownDatabase(ostring db, ostring user, ostring pwd, Environment::ShutdownFlag shutdownFlag,
                                       Environment::ShutdownMode shutdownMode, Environment::SessionMode sessionMode)
{
    Check(OCI_DatabaseShutdown(db.c_str(), user.c_str(), pwd.c_str(), (unsigned int) sessionMode, (unsigned int) shutdownMode, (unsigned int) shutdownFlag ));
}

inline void Environment::ChangeUserPassword(ostring db, ostring user, ostring pwd, ostring newPwd)
{
    Check(OCI_SetUserPassword(db.c_str(), user.c_str(), pwd.c_str(), newPwd.c_str()));
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

        handler(connection, (HAEventSource) source, (HAEventType) event, timestamp);
    }
}

inline unsigned int Environment::TAFHandler(OCI_Connection *pConnection, unsigned int type, unsigned int event)
{
    unsigned int res = OCI_FOC_OK;

    Connection::TAFHandlerProc handler = (Connection::TAFHandlerProc) GetEnvironmentHandle().Callbacks.Get(pConnection);

    if (handler)
    {
        Connection connection(pConnection, 0);

        res = (unsigned int) handler(connection, (Connection::FailoverRequest) type, (Connection::FailoverEvent) event);
    }

    return res;
}

inline void Environment::NotifyHandler(OCI_Event *pEvent)
{
    Subscription::NotifyHandlerProc handler = ( Subscription::NotifyHandlerProc) GetEnvironmentHandle().Callbacks.Get(Check(OCI_EventGetSubscription(pEvent)));

    if (handler)
    {
        Event evt(pEvent);
        handler(evt);
    }
}

inline void Environment::NotifyHandlerAQ(OCI_Dequeue *pDequeue)
{
    Dequeue::NotifyAQHandlerProc handler = (Dequeue::NotifyAQHandlerProc) GetEnvironmentHandle().Callbacks.Get(Check(pDequeue));

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

inline void Environment::EnvironmentHandle::Initialize(UnknownHandle handle, unsigned int envMode)
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

inline void ThreadKey::Create(ostring name, ThreadKeyFreeProc freeProc)
{
    Check(OCI_ThreadKeyCreate(name.c_str(), freeProc));
}

inline void ThreadKey::SetValue(ostring name, void *value)
{
    Check(OCI_ThreadKeySetValue(name.c_str(), value));
}

inline void * ThreadKey::GetValue(ostring name)
{
    return Check(OCI_ThreadKeyGetValue(name.c_str()));
}

/* --------------------------------------------------------------------------------------------- *
 * Pool
 * --------------------------------------------------------------------------------------------- */

inline Pool::Pool()
{

}

inline Pool::Pool(ostring db, ostring user, ostring pwd, Pool::PoolType poolType,
        unsigned int minSize, unsigned int maxSize, unsigned int increment, Environment::SessionMode sessionMode)
{
    Open(db, user, pwd, poolType, minSize, maxSize, increment, sessionMode);
}

inline void Pool::Open(ostring db, ostring user, ostring pwd, Pool::PoolType poolType,
            unsigned int minSize, unsigned int maxSize, unsigned int increment, Environment::SessionMode sessionMode)
{
    Release();

    Acquire(Check(OCI_PoolCreate(db.c_str(), user.c_str(), pwd.c_str(), (unsigned int) poolType, (unsigned int) sessionMode,
            minSize, maxSize, increment)), (HandleFreeFunc) OCI_PoolFree, 0);
}

inline void Pool::Close()
{
    Release();
}

inline Connection Pool::GetConnection(ostring sessionTag)
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

inline Connection::Connection(ostring db, ostring user, ostring pwd, Environment::SessionMode sessionMode)
{
    Open(db, user, pwd, sessionMode);
}

inline Connection::Connection(OCI_Connection *con,  Handle *parent)
{
    Acquire(con, parent ? (HandleFreeFunc) OCI_ConnectionFree : 0, parent);
}

inline void Connection::Open(ostring db, ostring user, ostring pwd, Environment::SessionMode sessionMode)
{
    Acquire(Check(OCI_ConnectionCreate(db.c_str(), user.c_str(), pwd.c_str(), (unsigned int) sessionMode)),
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

inline bool Connection::GetAutoCommit() const
{
    return (Check(OCI_GetAutoCommit(*this)) == TRUE);
}

/**
 * @brief
 * Check if the underlying psysical connection is still alive
 *
 * @return 
 * true if the connection is live otherwise false
 *
 */
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

inline unsigned int Connection::GetVersion() const
{
    return Check(OCI_GetVersionConnection(*this));
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

inline void Connection::ChangePassword(ostring newPwd)
{
    Check(OCI_SetPassword(*this, newPwd.c_str()));
}

inline ostring Connection::GetSessionTag() const
{
    return MakeString(Check(OCI_GetSessionTag(*this)));
}

inline void Connection::SetSessionTag(ostring tag)
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

inline void Connection::SetDefaultDateFormat(ostring format)
{
    Check(OCI_SetDefaultFormatDate(*this, format.c_str()));
}

inline void Connection::SetDefaultNumericFormat(ostring format)
{
    Check(OCI_SetDefaultFormatNumeric(*this, format.c_str()));
}

inline ostring  Connection::GetDefaultDateFormat() const
{
    return MakeString(Check(OCI_GetDefaultFormatDate(*this)));
}

inline ostring  Connection::GetDefaultNumericFormat() const
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

inline bool Connection::GetServerOutput(ostring &line) const
{
    const otext * str = Check(OCI_ServerGetOutput(*this));

    line = MakeString(str);

    return (str != 0);
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

inline void Connection::SetTrace(SessionTrace trace, ostring value)
{
    Check(OCI_SetTrace(*this, (unsigned int) trace, value.c_str()));
}

inline ostring Connection::GetTrace(SessionTrace trace) const
{
    return MakeString(Check(OCI_GetTrace(*this, (unsigned int) trace)));
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

inline bool Connection::IsTAFCapable() const
{
    return (Check(OCI_IsTAFCapable(*this)) == TRUE);
}

inline void Connection::SetTAFHandler(TAFHandlerProc handler)
{
    Environment::CallbackPool & pool = Environment::GetEnvironmentHandle().Callbacks;

    Check(OCI_SetTAFHandler(*this, (POCI_TAF_HANDLER ) (handler != 0 ? Environment::TAFHandler : 0 )));

    pool.Set((OCI_Connection*) *this, (CallbackPointer) handler);
}

inline void* Connection::GetUserData()
{
    return Check(OCI_GetUserData(*this));
}

inline void Connection::SetUserData(void *value)
{
    Check(OCI_SetUserData(*this, value));
}

/* --------------------------------------------------------------------------------------------- *
 * Transaction
 * --------------------------------------------------------------------------------------------- */

inline Transaction::Transaction(const Connection &connection, unsigned int timeout, TransactionMode mode, OCI_XID *pxid)
{
    Acquire(Check(OCI_TransactionCreate(connection, timeout, (unsigned int) mode, pxid)), (HandleFreeFunc) OCI_TransactionFree, 0);
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

inline Transaction::TransactionMode Transaction::GetMode() const
{
    return (Transaction::TransactionMode) Check(OCI_TransactionGetMode(*this));
}

inline unsigned int Transaction::GetTimeout() const
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
    int year, month, day;

    GetDate(&year, &month, &day);

    return year;
}

inline void Date::SetYear(int value)
{
    int year, month, day;

    GetDate(&year, &month, &day);
    SetDate(value, month, day);
}
 
inline int Date::GetMonth() const
{
    int year, month, day;

    GetDate(&year, &month, &day);

    return month;
}

inline void Date::SetMonth(int value)
{
    int year, month, day;

    GetDate(&year, &month, &day);
    SetDate(year, value, day);
}

inline int Date::GetDay() const
{
    int year, month, day;

    GetDate(&year, &month, &day);

    return day;
}

inline void Date::SetDay(int value)
{
    int year, month, day;

    GetDate(&year, &month, &day);
    SetDate(year, month, value);
}
 
inline int Date::GetHours() const
{
    int hour, minutes, seconds;

    GetTime(&hour, &minutes, &seconds);

    return hour;
}

inline void Date::SetHours(int value)
{
    int hour, minutes, seconds;

    GetTime(&hour, &minutes, &seconds);
    SetTime(value, minutes, seconds);
}
  
inline int Date::GetMinutes() const
{
    int hour, minutes, seconds;

    GetTime(&hour, &minutes, &seconds);

    return minutes;
}

inline void Date::SetMinutes(int value)
{
    int hour, minutes, seconds;

    GetTime(&hour, &minutes, &seconds);
    SetTime(hour, minutes, value);
}

inline int Date::GetSeconds() const    
{
    int hour, minutes, seconds;

    GetTime(&hour, &minutes, &seconds);

    return seconds;
}

inline void Date::SetSeconds(int value)
{
    int hour, minutes, seconds;

    GetTime(&hour, &minutes, &seconds);
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

inline void Date::GetDate(int *year, int *month, int *day) const
{
    Check(OCI_DateGetDate(*this, year, month, day));
}

inline void Date::GetTime(int *hour, int *min, int *sec) const
{
    Check(OCI_DateGetTime(*this, hour, min , sec));
}

inline void Date::GetDateTime(int *year, int *month, int *day, int *hour, int *min, int *sec) const
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

inline Date Date::NextDay(ostring day) const
{
    Date date;

    date.Assign(*this);    
    Check(OCI_DateNextDay(date, day.c_str()));

    return date;   
}

inline Date Date::LastDay() const
{
    Date date;

    date.Assign(*this);
    Check(OCI_DateLastDay(date));

    return date;
}

inline void Date::ChangeTimeZone(ostring tzSrc, ostring tzDst)
{
    Check(OCI_DateZoneToZone(*this, tzSrc.c_str(), tzDst.c_str()));
}

inline void Date::FromString(ostring data, ostring format)
{
   Check(OCI_DateFromText(*this, data.c_str(), format.c_str()));
}

inline ostring Date::ToString(ostring format) const
{
    size_t size = OCI_SIZE_BUFFER;

    ManagedBuffer<otext> buffer = new otext[size+1];

    Check(OCI_DateToText(*this, format.c_str(), (int) size, (otext *) buffer));

    return MakeString((otext *) buffer);
}

inline Date::operator ostring() const
{
    return ToString();
}

inline Date& Date::operator ++ (int)
{
    return *this + 1;
}

inline Date& Date::operator -- (int)
{
    return *this - 1;
}
  
inline Date& Date::operator = (const Date& other)
{
    Assign(other);
    return *this;
}

inline Date& Date::operator + (int val)
{
    return *this += val;
}

inline Date& Date::operator - (int val)
{
    return *this -= val;
}

inline Date& Date::operator += (int val)
{
    AddDays(val);
    return *this;
}

inline Date& Date::operator -= (int val)
{
    AddDays(-val);
    return *this;
}

inline bool Date::operator == (const Date& other) const
{
    return Compare(other) == 0;
}

inline bool Date::operator != (const Date& other) const
{
	return (!(*this == other));
}
			
inline bool Date::operator > (const Date& other) const
{
    return (Compare(other) > 0);
}

inline bool Date::operator < (const Date& other) const
{
    return (Compare(other) < 0);
}

inline bool Date::operator >= (const Date& other) const
{
    int res = Compare(other);

    return (res == 0 || res < 0);
}

inline bool Date::operator <= (const Date& other) const
{
    int res = Compare(other);

    return (res == 0 || res > 0);
}

/* --------------------------------------------------------------------------------------------- *
 * Interval
 * --------------------------------------------------------------------------------------------- */

inline Interval::Interval(IntervalType type)
{
    Acquire(Check(OCI_IntervalCreate(NULL, (unsigned int) type)), (HandleFreeFunc) OCI_IntervalFree, 0);
}

inline Interval::Interval(OCI_Interval *pInterval, Handle *parent)
{
    Acquire(pInterval, 0, parent);
}

inline void Interval::Assign(const Interval& other)
{
   Check(OCI_IntervalAssign(*this, other));
}

inline int Interval::Compare(const Interval& other) const
{
    return Check(OCI_IntervalCompare(*this, other));
}

inline Interval::IntervalType Interval::GetType() const
{
    return (IntervalType) Check(OCI_IntervalGetType(*this));
}

inline void Interval::Add(const Interval& other)
{
     Check(OCI_IntervalAdd(*this, other));
}

inline void Interval::Substract(const Interval& other)
{
    Check(OCI_IntervalSubtract(*this, other));
}

inline bool Interval::IsValid() const
{
    return (Check(OCI_IntervalCheck(*this)) == 0);
}

inline int Interval::GetYear() const
{
    int year, month;

    GetYearMonth(&year, &month);

    return year;
}

inline void Interval::SetYear(int value)
{
    int year, month;

    GetYearMonth(&year, &month);
    SetYearMonth(value, month);
}
 
inline int Interval::GetMonth() const
{
    int year, month;

    GetYearMonth(&year, &month);

    return month;
}

inline void Interval::SetMonth(int value)
{
    int year, month;

    GetYearMonth(&year, &month);
    SetYearMonth(year, value);
}

inline int Interval::GetDay() const
{
    int day, hour, minutes, seconds, milliseconds;

    GetDaySecond(&day, &hour, &minutes, &seconds, &milliseconds);

    return day;
}

inline void Interval::SetDay(int value)
{
    int day, hour, minutes, seconds, milliseconds;

    GetDaySecond(&day, &hour, &minutes, &seconds, &milliseconds);
    SetDaySecond(value, hour, minutes, seconds, milliseconds);
}
 
inline int Interval::GetHours() const
{
    int day, hour, minutes, seconds, milliseconds;

    GetDaySecond(&day, &hour, &minutes, &seconds, &milliseconds);

    return day;
}

inline void Interval::SetHours(int value)
{
    int day, hour, minutes, seconds, milliseconds;

    GetDaySecond(&day, &hour, &minutes, &seconds, &milliseconds);
    SetDaySecond(day, value, minutes, seconds, milliseconds);
}
  
inline int Interval::GetMinutes() const
{
    int day, hour, minutes, seconds, milliseconds;

    GetDaySecond(&day, &hour, &minutes, &seconds, &milliseconds);

    return minutes;
}

inline void Interval::SetMinutes(int value)
{
    int day, hour, minutes, seconds, milliseconds;

    GetDaySecond(&day, &hour, &minutes, &seconds, &milliseconds);
    SetDaySecond(day, hour, value, seconds, milliseconds);
}

inline int Interval::GetSeconds() const    
{
    int day, hour, minutes, seconds, milliseconds;

    GetDaySecond(&day, &hour, &minutes, &seconds, &milliseconds);

    return seconds;
}

inline void Interval::SetSeconds(int value)
{
    int day, hour, minutes, seconds, milliseconds;

    GetDaySecond(&day, &hour, &minutes, &seconds, &milliseconds);
    SetDaySecond(day, hour, minutes, value, milliseconds);
}

inline int Interval::GetMilliSeconds() const    
{
    int day, hour, minutes, seconds, milliseconds;

    GetDaySecond(&day, &hour, &minutes, &seconds, &milliseconds);

    return milliseconds;
}

inline void Interval::SetMilliSeconds(int value)
{
    int day, hour, minutes, seconds, milliseconds;

    GetDaySecond(&day, &hour, &minutes, &seconds, &milliseconds);
    SetDaySecond(day, hour, minutes, seconds, value);
}

inline void Interval::GetDaySecond(int *day, int *hour, int *min, int *sec, int *fsec) const
{
    Check(OCI_IntervalGetDaySecond(*this, day, hour, min, sec, fsec));
}

inline void Interval::SetDaySecond(int day, int hour, int min, int sec, int fsec)
{
    Check(OCI_IntervalSetDaySecond(*this, day, hour, min, sec, fsec));
}

inline void Interval::GetYearMonth(int *year, int *month) const
{
    Check(OCI_IntervalGetYearMonth(*this, year, month));
}
inline void Interval::SetYearMonth(int year, int month)
{
    Check(OCI_IntervalSetYearMonth(*this, year, month));
}

inline void Interval::FromTimeZone(ostring timeZone)
{
    Check(OCI_IntervalFromTimeZone(*this, timeZone.c_str()));
}

inline void Interval::FromString(ostring data)
{
    Check(OCI_IntervalFromText(*this, data.c_str()));
}

inline ostring Interval::ToString(int leadingPrecision, int fractionPrecision) const
{
    size_t size = OCI_SIZE_BUFFER;

    ManagedBuffer<otext> buffer = new otext[size+1];

    Check(OCI_IntervalToText(*this, leadingPrecision, fractionPrecision,  (int) size, (otext *) buffer));

    return MakeString((otext *) buffer);
}

inline Interval::operator ostring() const
{
    return ToString();
}

/* --------------------------------------------------------------------------------------------- *
 * Timestamp
 * --------------------------------------------------------------------------------------------- */

inline Timestamp::Timestamp(TimestampType type)
{
    Acquire(Check(OCI_TimestampCreate(NULL, (unsigned int) type)), (HandleFreeFunc) OCI_TimestampFree, 0);
}

inline Timestamp::Timestamp(OCI_Timestamp *pTimestamp, Handle *parent)
{
    Acquire(pTimestamp, 0, parent);
}

inline void Timestamp::Assign(const Timestamp& other)
{
    Check(OCI_TimestampAssign(*this, other));
}

inline int Timestamp::Compare(const Timestamp& other) const
{
    return Check(OCI_TimestampCompare(*this, other));
}

inline Timestamp::TimestampType Timestamp::GetType() const
{
    return (TimestampType) Check(OCI_TimestampGetType(*this));
}

inline void Timestamp::Construct(int year, int month, int day, int hour, int min, int sec, int fsec, ostring timeZone)
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

    GetDate(&year, &month, &day);

    return year;
}

inline void Timestamp::SetYear(int value)
{
    int year, month, day, hour, minutes, seconds, milliseconds;

    GetDateTime(&year, &month, &day, &hour, &minutes, &seconds, &milliseconds);
    Construct(value, month, day, hour, minutes, seconds, milliseconds);
}
 
inline int Timestamp::GetMonth() const
{
    int year, month, day;

    GetDate(&year, &month, &day);

    return month;
}

inline void Timestamp::SetMonth(int value)
{
    int year, month, day, hour, minutes, seconds, milliseconds;

    GetDateTime(&year, &month, &day, &hour, &minutes, &seconds, &milliseconds);
    Construct(year, value, day, hour, minutes, seconds, milliseconds);
}

inline int Timestamp::GetDay() const
{
    int year, month, day;

    GetDate(&year, &month, &day);

    return day;
}

inline void Timestamp::SetDay(int value)
{
    int year, month, day, hour, minutes, seconds, milliseconds;

    GetDateTime(&year, &month, &day, &hour, &minutes, &seconds, &milliseconds);
    Construct(year, month, value, hour, minutes, seconds, milliseconds);
}
 
inline int Timestamp::GetHours() const
{
    int hour, minutes, seconds, milliseconds;

    GetTime(&hour, &minutes, &seconds, &milliseconds);

    return hour;
}

inline void Timestamp::SetHours(int value)
{
    int year, month, day, hour, minutes, seconds, milliseconds;

    GetDateTime(&year, &month, &day, &hour, &minutes, &seconds, &milliseconds);
    Construct(year, month, day, value, minutes, seconds, milliseconds);
}
  
inline int Timestamp::GetMinutes() const
{
    int hour, minutes, seconds, milliseconds;

    GetTime(&hour, &minutes, &seconds, &milliseconds);

    return minutes;
}

inline void Timestamp::SetMinutes(int value)
{
    int year, month, day, hour, minutes, seconds, milliseconds;

    GetDateTime(&year, &month, &day, &hour, &minutes, &seconds, &milliseconds);
    Construct(year, month, day, hour, value, seconds, milliseconds);
}

inline int Timestamp::GetSeconds() const    
{
    int hour, minutes, seconds, milliseconds;

    GetTime(&hour, &minutes, &seconds, &milliseconds);

    return seconds;
}

inline void Timestamp::SetSeconds(int value)
{
    int year, month, day, hour, minutes, seconds, milliseconds;

    GetDateTime(&year, &month, &day, &hour, &minutes, &seconds, &milliseconds);
    Construct(year, month, day, hour, minutes, value, milliseconds);
}

inline int Timestamp::GetMilliSeconds() const    
{
    int hour, minutes, seconds, milliseconds;

    GetTime(&hour, &minutes, &seconds, &milliseconds);

    return milliseconds;
}

inline void Timestamp::SetMilliSeconds(int value)
{
    int year, month, day, hour, minutes, seconds, milliseconds;

    GetDateTime(&year, &month, &day, &hour, &minutes, &seconds, &milliseconds);
    Construct(year, month, day, hour, minutes, seconds, value);
}

inline void Timestamp::GetDate(int *year, int *month, int *day) const
{
    Check(OCI_TimestampGetDate(*this, year, month, day));
}

inline void Timestamp::GetTime(int *hour, int *min, int *sec, int *fsec) const
{
    Check(OCI_TimestampGetTime(*this, hour, min, sec, fsec));
}

inline void Timestamp::GetDateTime(int *year, int *month, int *day, int *hour, int *min, int *sec, int *fsec) const
{
    Check(OCI_TimestampGetDateTime(*this, year, month, day, hour, min, sec, fsec));
}

inline ostring Timestamp::GetTimeZone() const
{
    size_t size = OCI_SIZE_BUFFER;

    ManagedBuffer<otext> buffer = new otext[size+1];

    Check(OCI_TimestampGetTimeZoneName(*this,  (int) size, (otext *) buffer));

    return MakeString((otext *) buffer);
}

inline void Timestamp::GetTimeZoneOffset(int *hour, int *min) const
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

inline void Timestamp::FromString(ostring data, ostring format)
{
    Check(OCI_TimestampFromText(*this, data.c_str(), format.c_str()));
}

inline ostring Timestamp::ToString(ostring format, int precision) const
{
    size_t size = OCI_SIZE_BUFFER;

    ManagedBuffer<otext> buffer = new otext[size+1];

    Check(OCI_TimestampToText(*this, format.c_str(), (int) size, (otext *) buffer, precision));

    return MakeString((otext *) buffer);
}

inline Timestamp::operator ostring() const
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

inline ostring Clob::Read(unsigned int size)
{
    ManagedBuffer<otext> buffer = new otext[size+1];

    Check(OCI_LobRead(*this, (void *) buffer, size));

    return MakeString( (const otext *) buffer);
}

inline unsigned int Clob::Write(ostring content)
{
    return Check(OCI_LobWrite(*this, (void *) content.c_str(), (unsigned int) content.size()));
}

inline unsigned int Clob::Append(ostring content)
{
    return Check(OCI_LobAppend(*this, (void *) content.c_str(), (unsigned int) content.size()));
}

inline bool Clob::Seek(SeekMode seekMode, big_uint offset)
{
    return (Check(OCI_LobSeek(*this, offset, (unsigned int) seekMode)) == TRUE);
}

inline void Clob::Append(const Clob &other)
{
    Check(OCI_LobAppendLob(*this, other));
}

inline void Clob::Assign(const Clob &other)
{
    Check(OCI_LobAssign(*this, other));
}

inline bool Clob::Equals(const Clob &other) const
{
    return (Check(OCI_LobIsEqual(*this, other)) == TRUE);
}

inline big_uint Clob::GetOffset() const
{
    return Check(OCI_LobGetOffset(*this));
}

inline big_uint Clob::GetLength() const
{
    return Check(OCI_LobGetLength(*this));
}

inline big_uint Clob::GetMaxSize() const
{
    return Check(OCI_LobGetMaxSize(*this));
}

inline big_uint Clob::GetChunkSize() const
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

inline void Clob::Copy(Clob &dest, big_uint offset, big_uint offsetDest, big_uint size) const
{
    Check(OCI_LobCopy(dest, *this, offsetDest, offset, size));
}

inline bool Clob::IsTemporary() const
{
    return (Check(OCI_LobIsTemporary(*this)) == TRUE);
}

inline void Clob::Open(OpenMode mode)
{
    Check(OCI_LobOpen(*this, (unsigned int) mode));
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

inline Clob::operator ostring() const
{
    size_t size   = (size_t) GetLength();
    size_t offset = (size_t) GetOffset();

    ManagedBuffer<otext> buffer = new otext[size+1];

    Check(OCI_LobRead(*this, (void *) buffer, (unsigned int) size));
    Check(OCI_LobSeek(*this, offset, OCI_SEEK_SET)); 

    return MakeString( (const otext *) buffer);
}

inline Clob& Clob::operator = (const Clob& other)
{
    Assign(other);
    return *this;
}

inline Clob& Clob::operator + (const Clob& other)
{
    Append(other);
    return *this;
}

inline bool Clob::operator == (const Clob& other) const
{
    return Equals(other);
}

inline bool Clob::operator != (const Clob& other) const
{
    return (!(*this == other));
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

inline bool Blob::Seek(SeekMode seekMode, big_uint offset)
{
    return (Check(OCI_LobSeek(*this, offset, (unsigned int) seekMode)) == TRUE);
}

inline void Blob::Append(const Blob &other)
{
    Check(OCI_LobAppendLob(*this, other));
}

inline void Blob::Assign(const Blob &other)
{
    Check(OCI_LobAssign(*this, other));
}

inline bool Blob::Equals(const Blob &other) const
{
    return (Check(OCI_LobIsEqual(*this, other)) == TRUE);
}

inline big_uint Blob::GetOffset() const
{
    return Check(OCI_LobGetOffset(*this));
}

inline big_uint Blob::GetLength() const
{
    return Check(OCI_LobGetLength(*this));
}

inline big_uint Blob::GetMaxSize() const
{
    return Check(OCI_LobGetMaxSize(*this));
}

inline big_uint Blob::GetChunkSize() const
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

inline void Blob::Copy(Blob &dest, big_uint offset, big_uint offsetDest, big_uint size) const
{
    Check(OCI_LobCopy(dest, *this, offsetDest, offset, size));
}

inline bool Blob::IsTemporary() const
{
    return (Check(OCI_LobIsTemporary(*this)) == TRUE);
}

inline void Blob::Open(OpenMode mode)
{
    Check(OCI_LobOpen(*this, (unsigned int) mode));
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

inline Blob& Blob::operator = (const Blob& other)
{
    Assign(other);
    return *this;
}

inline Blob& Blob::operator + (const Blob& other)
{
    Append(other);
    return *this;
}

inline bool Blob::operator == (const Blob& other) const
{
    return Equals(other);
}

inline bool Blob::operator != (const Blob& other) const
{
    return (!(*this == other));
}

/* --------------------------------------------------------------------------------------------- *
 * File
 * --------------------------------------------------------------------------------------------- */

inline File::File(const Connection &connection)
{
    Acquire(Check(OCI_FileCreate(connection, OCI_BFILE)), (HandleFreeFunc) OCI_FileFree, connection.GetHandle());
}

inline File::File(const Connection &connection, ostring directory, ostring name)
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

inline bool File::Seek(SeekMode seekMode, big_uint offset)
{
    return (Check(OCI_FileSeek(*this, offset, (unsigned int) seekMode)) == TRUE);
}

inline void File::Assign(const File &other)
{
    Check(OCI_FileAssign(*this, other));
}

inline bool File::Equals(const File &other) const
{
    return (Check(OCI_FileIsEqual(*this, other)) == TRUE);
}

inline big_uint File::GetOffset() const
{
    return Check(OCI_FileGetOffset(*this));
}

inline big_uint File::GetSize() const
{
    return Check(OCI_FileGetSize(*this));
}

inline bool File::Exists() const
{
    return (Check(OCI_FileExists(*this)) == TRUE);
}

inline void File::SetInfos(ostring directory, ostring name)
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

/* --------------------------------------------------------------------------------------------- *
 * TypeInfo
 * --------------------------------------------------------------------------------------------- */

inline TypeInfo::TypeInfo(const Connection &connection, ostring name, TypeInfoObjectType type)
{
    Acquire(Check(OCI_TypeInfoGet(connection, name.c_str(), (unsigned int) type)), (HandleFreeFunc) 0, connection.GetHandle());
}

inline TypeInfo::TypeInfo(OCI_TypeInfo *pTypeInfo)
{
    Acquire(pTypeInfo, 0, 0);
}

inline TypeInfo::TypeInfoObjectType TypeInfo::GetType() const
{
    return (TypeInfoObjectType) Check(OCI_TypeInfoGetType(*this));
}

inline ostring TypeInfo::GetName() const
{
    return Check(OCI_TypeInfoGetName(*this));
}

/**
 * @brief
 * Return the related connection
 *
 */
inline Connection TypeInfo::GetConnection() const
{
    return Connection(Check(OCI_TypeInfoGetConnection(*this)), 0);
}

inline unsigned int TypeInfo::GetColumnCount() const
{
    return Check(OCI_TypeInfoGetColumnCount(*this));
}

inline Column TypeInfo::GetColumn(unsigned int index) const
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

inline bool Object::IsAttributeNull(ostring name) const
{
    return (Check(OCI_ObjectIsNull(*this, name.c_str())) == TRUE);
}

inline void Object::SetAttributeNull(ostring name)
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
    return (ObjectType) Check(OCI_ObjectGetType(*this));
}

template<>
inline short Object::Get<short>(ostring name) const
{
    return Check(OCI_ObjectGetShort(*this, name.c_str()));
}

template<>
inline unsigned short Object::Get<unsigned short>(ostring name) const
{
    return Check(OCI_ObjectGetUnsignedShort(*this, name.c_str()));
}

template<>
inline int Object::Get<int>(ostring name) const
{
    return Check(OCI_ObjectGetInt(*this, name.c_str()));
}

template<>
inline unsigned int Object::Get<unsigned int>(ostring name) const
{
    return Check(OCI_ObjectGetUnsignedInt(*this, name.c_str()));
}

template<>
inline big_int Object::Get<big_int>(ostring name) const
{
    return Check(OCI_ObjectGetBigInt(*this, name.c_str()));
}

template<>
inline big_uint Object::Get<big_uint>(ostring name) const
{
    return Check(OCI_ObjectGetUnsignedBigInt(*this, name.c_str()));
}

template<>
inline float Object::Get<float>(ostring name) const
{
    return Check(OCI_ObjectGetFloat(*this, name.c_str()));
}

template<>
inline double Object::Get<double>(ostring name) const
{
    return Check(OCI_ObjectGetDouble(*this, name.c_str()));
}

template<>
inline ostring Object::Get<ostring>(ostring name) const
{
    return MakeString(Check(OCI_ObjectGetString(*this,name.c_str())));
}

template<>
inline Date Object::Get<Date>(ostring name) const
{
    return Date(Check(OCI_ObjectGetDate(*this,name.c_str())), GetHandle());
}

template<>
inline Timestamp Object::Get<Timestamp>(ostring name) const
{
    return Timestamp(Check(OCI_ObjectGetTimestamp(*this,name.c_str())), GetHandle());
}

template<>
inline Interval Object::Get<Interval>(ostring name) const
{
    return Interval(Check(OCI_ObjectGetInterval(*this,name.c_str())), GetHandle());
}

template<>
inline Object Object::Get<Object>(ostring name) const
{
    return Object(Check(OCI_ObjectGetObject(*this,name.c_str())), GetHandle());
}

template<>
inline Collection Object::Get<Collection>(ostring name) const
{
    return Collection(Check(OCI_ObjectGetColl(*this,name.c_str())), GetHandle());
}

template<>
inline Reference Object::Get<Reference>(ostring name) const
{
    return Reference(Check(OCI_ObjectGetRef(*this,name.c_str())), GetHandle());
}

template<>
inline Clob Object::Get<Clob>(ostring name) const
{
    return Clob(Check(OCI_ObjectGetLob(*this,name.c_str())), GetHandle());
}

template<>
inline Blob Object::Get<Blob>(ostring name) const
{
    return Blob(Check(OCI_ObjectGetLob(*this,name.c_str())), GetHandle());
}

template<>
inline File Object::Get<File>(ostring name) const
{
    return File(Check(OCI_ObjectGetFile(*this,name.c_str())), GetHandle());
}

template<>
inline void Object::Get<BufferPointer>(ostring name, BufferPointer value, unsigned int &size) const
{
    Check(OCI_ObjectGetRaw(*this,name.c_str(), value, size));
}

template<>
inline void Object::Set<short>(ostring name, const short &value)
{
    Check(OCI_ObjectSetShort(*this, name.c_str(), value));
}

template<>
inline void Object::Set<unsigned short>(ostring name, const unsigned short &value)
{
    Check(OCI_ObjectSetUnsignedShort(*this, name.c_str(), value));
}

template<>
inline void Object::Set<int>(ostring name, const int &value)
{
    Check(OCI_ObjectSetInt(*this, name.c_str(), value));
}

template<>
inline void Object::Set<unsigned int>(ostring name, const unsigned int &value)
{
    Check(OCI_ObjectSetUnsignedInt(*this, name.c_str(), value));
}

template<>
inline void Object::Set<big_int>(ostring name, const big_int &value)
{
    Check(OCI_ObjectSetBigInt(*this, name.c_str(), value));
}

template<>
inline void Object::Set<big_uint>(ostring name, const big_uint &value)
{
    Check(OCI_ObjectSetUnsignedBigInt(*this, name.c_str(), value));
}

template<>
inline void Object::Set<float>(ostring name, const float &value)
{
    Check(OCI_ObjectSetFloat(*this, name.c_str(), value));
}

template<>
inline void Object::Set<double>(ostring name, const double &value)
{
    Check(OCI_ObjectSetDouble(*this, name.c_str(), value));
}

template<>
inline void Object::Set<ostring>(ostring name, const ostring &value)
{
    Check(OCI_ObjectSetString(*this, name.c_str(), value.c_str()));
}

template<>
inline void Object::Set<Date>(ostring name, const Date &value)
{
    Check(OCI_ObjectSetDate(*this, name.c_str(), value));
}

template<>
inline void Object::Set<Timestamp>(ostring name, const Timestamp &value)
{
    Check(OCI_ObjectSetTimestamp(*this, name.c_str(), value));
}

template<>
inline void Object::Set<Interval>(ostring name, const Interval &value)
{
    Check(OCI_ObjectSetInterval(*this, name.c_str(), value));
}

template<>
inline void Object::Set<Object>(ostring name, const Object &value)
{
    Check(OCI_ObjectSetObject(*this, name.c_str(), value));
}

template<>
inline void Object::Set<Collection>(ostring name, const Collection &value)
{
    Check(OCI_ObjectSetColl(*this, name.c_str(), value));
}

template<>
inline void Object::Set<Reference>(ostring name, const Reference &value)
{
    Check(OCI_ObjectSetRef(*this, name.c_str(), value));
}

template<>
inline void Object::Set<Clob>(ostring name, const Clob &value)
{
    Check(OCI_ObjectSetLob(*this, name.c_str(), value));
}

template<>
inline void Object::Set<Blob>(ostring name, const Blob &value)
{
    Check(OCI_ObjectSetLob(*this, name.c_str(), value));
}

template<>
inline void Object::Set<File>(ostring name, const File &value)
{
    Check(OCI_ObjectSetFile(*this, name.c_str(), value));
}

template<>
inline void Object::Set<BufferPointer>(ostring name, const BufferPointer & value, unsigned int size)
{
    Check(OCI_ObjectSetRaw(*this, name.c_str(), value, size));
}

inline ostring Object::ToString() const
{
    unsigned int len = 0;

    Check(OCI_ObjectToText(*this, &len, 0));

    ManagedBuffer<otext> buffer = new otext[len+1];

    Check(OCI_ObjectToText(*this, &len, (otext *) buffer));

    return MakeString((otext *) buffer);
}

inline Object::operator ostring() const
{
    return ToString();
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

inline TypeInfo Reference::GetTypeInfo() const
{
    return TypeInfo(Check(OCI_RefGetTypeInfo(*this)));
}

inline Object Reference::GetObject() const
{
    return Object(Check(OCI_RefGetObject(*this)), GetHandle());
}

inline void Reference::Assign(const Reference& other)
{
   Check(OCI_RefAssign(*this, other));
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
    unsigned int size =  Check(OCI_RefGetHexSize(*this));

    ManagedBuffer<otext> buffer = new otext [ size+1 ];

    Check(OCI_RefToText(*this, size, (otext *) buffer));

    return MakeString( (const otext *) buffer);
}

inline Reference::operator ostring() const
{
    return ToString();
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

inline TypeInfo Collection::GetTypeInfo() const
{
    return TypeInfo(Check(OCI_CollGetTypeInfo(*this)));
}

inline Collection::CollectionType Collection::GetType() const
{
    return (CollectionType) Check(OCI_CollGetType(*this));
}

inline unsigned int Collection::GetMax() const
{
    return Check(OCI_CollGetMax(*this));
}

inline unsigned int Collection::GetSize() const

{
    return Check(OCI_CollGetSize(*this));
}

inline unsigned int Collection::GetCount() const

{
    return Check(OCI_CollGetCount(*this));
}

inline void Collection::Truncate(unsigned int size)
{
    Check(OCI_CollTrim(*this, size));
}

inline void Collection::Clear()
{
    Check(OCI_CollClear(*this));
}

inline bool Collection::IsElementNull(unsigned int index) const
{
   return (Check(OCI_ElemIsNull(Check(OCI_CollGetElem(*this, index)))) == TRUE);
}

inline void Collection::SetElementNull(unsigned int index)
{
    Check(OCI_ElemSetNull(Check(OCI_CollGetElem(*this, index))));
}

inline bool Collection::Delete(unsigned int index) const
{
   return (Check(OCI_CollDeleteElem(*this, index)) == TRUE);
}

template <class TDataType>
inline TDataType Collection::Get(unsigned int index) const
{
    return GetElem<TDataType>(Check(OCI_CollGetElem(*this, index)), GetHandle());
}

template <class TDataType>
inline void Collection::Get(unsigned int index, TDataType value, unsigned int &size) const
{
    GetElem<TDataType>(Check(OCI_CollGetElem(*this, index)), value, size);
}

template <class TDataType>
inline void Collection::Set(unsigned int index, const TDataType &data)
{
    OCI_Elem * elem = Check(OCI_CollGetElem(*this, index));

    SetElem<TDataType>(elem, data);

    Check(OCI_CollSetElem(*this, index, elem));
}

template <class TDataType>
inline void Collection::Set(unsigned int index, const TDataType value, unsigned int size)
{
    OCI_Elem * elem = Check(OCI_CollGetElem(*this, index));

    SetElem<TDataType>(elem, value, size);

    Check(OCI_CollSetElem(*this, index, elem));
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
inline ostring Collection::GetElem<ostring>(OCI_Elem *elem, Handle *parent)
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
inline void Collection::SetElem<ostring>(OCI_Elem *elem, const ostring &value)
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

inline ostring Collection::ToString() const
{
    unsigned int len = 0;

    Check(OCI_CollToText(*this, &len, 0));

    ManagedBuffer<otext> buffer = new otext[len+1];

    Check(OCI_CollToText(*this, &len, (otext *) buffer));

    return MakeString((otext *) buffer);
}

inline Collection::operator ostring() const
{
    return ToString();
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

inline bool CollectionIterator::operator ++ (int)
{
    return Next();
}

inline bool CollectionIterator::operator -- (int)
{
    return Prev();
}

template <class TDataType>
inline void CollectionIterator::Set(TDataType &value)
{
    Collection::SetElem<TDataType>(Check(OCI_IterGetCurrent(*this)), value);
}

template <class TDataType>
inline TDataType CollectionIterator::Get() const
{
    return Collection::GetElem<TDataType>(Check(OCI_IterGetCurrent(*this)), GetHandle());
}

inline bool CollectionIterator::IsElementNull() const
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

inline ostring CLong::Read(unsigned int size)
{
    ManagedBuffer<otext> buffer = new otext[size+1];

    size = Check(OCI_LongRead(*this, (void *) buffer, size));

    return MakeString( (const otext *) buffer);
}

inline unsigned int CLong::Write(ostring content)
{
    return Check(OCI_LongWrite(*this, (void *) content.c_str(), (unsigned int) content.size()));
}

inline unsigned int CLong::GetSize() const
{
    return Check(OCI_LongGetSize(*this));
}

inline ostring CLong::GetContent() const
{
    return MakeString((otext *) Check(OCI_LongGetBuffer(*this)));
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

inline unsigned int BLong::GetSize() const
{
    return Check(OCI_LongGetSize(*this));
}

inline void * BLong::GetContent() const
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
inline BindValue<TValueType>::operator TValueType() const
{
    return _value;
}

/* --------------------------------------------------------------------------------------------- *
 * BindArray
 * --------------------------------------------------------------------------------------------- */

inline BindArray::BindArray() : _object(0)
{

}

template <class TObjectType, class TDataType>
inline void BindArray::SetVector(std::vector<TObjectType> & vector, unsigned int mode, unsigned int elemCount, unsigned int elemSize)
{
    _object = new BindArrayObject<TObjectType, TDataType>(vector, mode, elemCount, elemSize);
}

inline BindArray::~BindArray()
{
    delete _object;
}

template <class TObjectType, class TDataType>
inline TDataType *  BindArray::GetData ()  const
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
inline BindArray::BindArrayObject<TObjectType, TDataType>::BindArrayObject(std::vector<TObjectType> &vector, unsigned int mode, unsigned int elemCount, unsigned int elemSize)
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
inline void BindArray::BindArrayObject<ostring, otext>::AllocData()
{
    _data = (otext *) new otext[_elemSize * _elemCount];

    memset(_data, 0, _elemSize * _elemCount * sizeof(otext));
}

template <class TObjectType, class TDataType>
inline void BindArray::BindArrayObject<TObjectType, TDataType>::FreeData()
{
    delete [] _data ;
}

template <class TObjectType, class TDataType>
inline BindArray::BindArrayObject<TObjectType, TDataType> & BindArray::BindArrayObject<TObjectType, TDataType>::operator = (const BindArrayObject<TObjectType, TDataType> & other)
{
    _object = other._object;
    return *this;
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
inline void BindArray::BindArrayObject<ostring, otext>::SetInData()
{
    if (_mode & OCI_BDM_IN)
    {
        std::vector<ostring>::iterator it, it_end;

        unsigned int index = 0;

        for (it = _vector.begin(), it_end = _vector.end(); it != it_end && index < _elemCount; it++, index++)
        {
            ostring & value = *it;

            memcpy((((otext *) _data) + (_elemSize * index)), value.c_str(), value.size() * sizeof(otext));
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
inline void BindArray::BindArrayObject<ostring, otext>::SetOutData()
{
    if (_mode & OCI_BDM_OUT)
    {
        std::vector<ostring>::iterator it, it_end;

        unsigned int index = 0;

        for (it = _vector.begin(), it_end = _vector.end(); it != it_end && index < _elemCount; it++, index++)
        {
            *it = (otext*) (((otext *) _data) + (_elemSize * index));
        }
    }
}

template <class TObjectType, class TDataType>
inline BindArray::BindArrayObject<TObjectType, TDataType>:: operator std::vector<TObjectType> & ()  const
{
    return _vector;
}

template <class TObjectType, class TDataType>
inline BindArray::BindArrayObject<TObjectType, TDataType>:: operator TDataType * ()  const
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

    memcpy(_data, _string.c_str(), size * sizeof(otext));
}

inline void BindString::SetOutData()
{
    _string = _data;
}

inline BindString::BindString(ostring &string, unsigned int elemSize) : _string(string), _elemSize(elemSize)
{
    _data = new otext[_elemSize];

     memset( _data, 0, _elemSize * sizeof(otext));
}

inline BindString::~BindString()
{
    delete [] _data;
}

inline BindString::operator otext *()  const
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

inline ostring BindInfo::GetName() const
{
    return MakeString(Check(OCI_BindGetName(*this)));
}

inline unsigned int BindInfo::GetType() const
{
    return Check(OCI_BindGetType(*this));
}

inline unsigned int BindInfo::GetSubType() const
{
    return Check(OCI_BindGetSubtype(*this));
}

inline unsigned int BindInfo::GetElemcount() const
{
    return Check(OCI_BindGetDirection(*this));
}

inline Statement BindInfo::GetStatement() const
{
    return Statement(Check(OCI_BindGetStatement(*this)));
}

inline void BindInfo::SetNull(bool value)
{
    if (value)
    {
        Check(OCI_BindSetNull(*this));
    }
    else
    {
        Check(OCI_BindSetNotNull(*this));
    }
}

inline void BindInfo::SetNull(bool value, unsigned int pos)
{
    if (value)
    {
        Check(OCI_BindSetNullAtPos(*this, pos));
    }
    else
    {
        Check(OCI_BindSetNotNullAtPos(*this, pos));
    }
}

inline bool BindInfo::IsNull() const
{
    return (Check(OCI_BindIsNull(*this)) == TRUE);
}

inline bool BindInfo::IsNull(unsigned int pos) const
{
    return (Check(OCI_BindIsNullAtPos(*this, pos)) == TRUE);
}

inline void BindInfo::SetCharsetForm(Environment::CharsetForm value)
{
    Check(OCI_BindSetCharsetForm(*this, (unsigned int) value));
}

inline void BindInfo::SetDirection(BindDirection value)
{
    Check(OCI_BindSetDirection(*this, (unsigned int) value));
}

inline BindInfo::BindDirection BindInfo::GetDirection() const
{
    return (BindDirection) Check(OCI_BindGetDirection(*this));
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

inline Connection Statement::GetConnection() const
{
    return Connection(Check(OCI_StatementGetConnection(*this)), 0);
}

inline void Statement::Describe(ostring sql)
{
    ClearBinds();
    ReleaseResultsets();
    Check(OCI_Describe(*this, sql.c_str()));
}

inline void Statement::Parse(ostring sql)
{
    ClearBinds();
    ReleaseResultsets();
    Check(OCI_Parse(*this, sql.c_str()));
}

inline void Statement::Prepare(ostring sql)
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

inline void Statement::Execute(ostring sql)
{
    ClearBinds();
    ReleaseResultsets();
    Check(OCI_ExecuteStmt(*this, sql.c_str()));
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

inline unsigned int Statement::GetBindCount() const
{
    return Check(OCI_GetBindCount(*this));
}

inline BindInfo Statement::GetBind(unsigned int index) const
{
    return BindInfo(Check(OCI_GetBind(*this, index)), GetHandle());
}

inline BindInfo Statement::GetBind(ostring name) const
{
    return BindInfo(Check(OCI_GetBind2(*this, name.c_str())), GetHandle());
}

template <typename TBindMethod, class TDataType>
inline void Statement::Bind (TBindMethod &method, ostring name, TDataType& value, BindInfo::BindDirection mode)
{
    Check(method(*this, name.c_str(), &value));
    SetLastBindMode(mode);
}

template <typename TBindMethod, class TObjectType, class TDataType>
inline void Statement::Bind (TBindMethod &method, ostring name, TObjectType& value, BindValue<TDataType> datatype, BindInfo::BindDirection mode)
{
    Check(method(*this, name.c_str(), (TDataType) value));
    SetLastBindMode(mode);
}

template <typename TBindMethod, class TObjectType, class TDataType>
inline void Statement::Bind (TBindMethod &method, ostring name, std::vector<TObjectType> &values, BindValue<TDataType> datatype, BindInfo::BindDirection mode)
{
    BindArray * bnd = new BindArray();

    bnd->SetVector<TObjectType, TDataType>(values, mode, OCI_BindArrayGetSize(*this), sizeof(TDataType));

    BindsHolder *bindsHolder = GetBindsHolder(true);
    bindsHolder->AddBindObject(bnd);

    Check(method(*this, name.c_str(), (TDataType *) bnd->GetData<TObjectType, TDataType>(), 0));
    SetLastBindMode(mode);
}

template <typename TBindMethod, class TObjectType, class TDataType, class TElemType>
inline void Statement::Bind (TBindMethod &method, ostring name, std::vector<TObjectType> &values, BindValue<TDataType> datatype, BindInfo::BindDirection mode, TElemType type)
{
    BindArray * bnd = new BindArray();

    bnd->SetVector<TObjectType, TDataType>(values, mode, OCI_BindArrayGetSize(*this), sizeof(TDataType));

    BindsHolder *bindsHolder = GetBindsHolder(true);
    bindsHolder->AddBindObject(bnd);

    method(*this, name.c_str(), (TDataType *) bnd->GetData<TObjectType, TDataType>(), type, 0);
    SetLastBindMode(mode);
}

template <>
inline void Statement::Bind<short>(ostring name, short &value, BindInfo::BindDirection mode)
{
    Bind(OCI_BindShort, name, value, mode);
}

template <>
inline void Statement::Bind<unsigned short>(ostring name, unsigned short &value, BindInfo::BindDirection mode)
{
    Bind(OCI_BindUnsignedShort, name, value, mode);
}

template <>
inline void Statement::Bind<int>(ostring name, int &value, BindInfo::BindDirection mode)
{
    Bind(OCI_BindInt, name, value, mode);
}

template <>
inline void Statement::Bind<unsigned int>(ostring name, unsigned int &value, BindInfo::BindDirection mode)
{
    Bind(OCI_BindUnsignedInt, name, value, mode);
}

template <>
inline void Statement::Bind<big_int>(ostring name, big_int &value, BindInfo::BindDirection mode)
{
    Bind(OCI_BindBigInt, name, value, mode);
}

template <>
inline void Statement::Bind<big_uint>(ostring name, big_uint &value, BindInfo::BindDirection mode)
{
    Bind(OCI_BindUnsignedBigInt, name, value, mode);
}

template <>
inline void Statement::Bind<float>(ostring name, float &value, BindInfo::BindDirection mode)
{
    Bind(OCI_BindFloat, name, value, mode);
}

template <>
inline void Statement::Bind<double>(ostring name, double &value, BindInfo::BindDirection mode)
{
    Bind(OCI_BindDouble, name, value, mode);
}

template <>
inline void Statement::Bind<Date>(ostring name, Date &value, BindInfo::BindDirection mode)
{
    Bind(OCI_BindDate, name, value, BindValue<OCI_Date *>(),  mode);
}

template <>
inline void Statement::Bind<Timestamp>(ostring name, Timestamp &value, BindInfo::BindDirection mode)
{
    Bind(OCI_BindTimestamp, name, value, BindValue<OCI_Timestamp *>(), mode);
}

template <>
inline void Statement::Bind<Interval>(ostring name, Interval &value, BindInfo::BindDirection mode)
{
    Bind(OCI_BindInterval, name, value, BindValue<OCI_Interval *>(), mode);
}

template <>
inline void Statement::Bind<Clob>(ostring name, Clob &value, BindInfo::BindDirection mode)
{
    Bind(OCI_BindLob, name, value, BindValue<OCI_Lob *>(), mode);
}

template <>
inline void Statement::Bind<Blob>(ostring name, Blob &value, BindInfo::BindDirection mode)
{
    Bind(OCI_BindLob, name, value, BindValue<OCI_Lob *>(), mode);
}

template <>
inline void Statement::Bind<File>(ostring name, File &value, BindInfo::BindDirection mode)
{
    Bind(OCI_BindFile, name, value, BindValue<OCI_File *>(), mode);
}

template <>
inline void Statement::Bind<Object>(ostring name, Object &value, BindInfo::BindDirection mode)
{
    Bind(OCI_BindObject, name, value, BindValue<OCI_Object *>(), mode);
}

template <>
inline void Statement::Bind<Reference>(ostring name, Reference &value, BindInfo::BindDirection mode)
{
    Bind(OCI_BindRef, name, value, BindValue<OCI_Ref *>(), mode);
}

template <>
inline void Statement::Bind<Collection>(ostring name, Collection &value, BindInfo::BindDirection mode)
{
    Bind(OCI_BindColl, name, value, BindValue<OCI_Coll *>(), mode);
}

template <>
inline void Statement::Bind<Statement>(ostring name, Statement &value, BindInfo::BindDirection mode)
{
    Bind(OCI_BindStatement, name, value, BindValue<OCI_Statement *>(), mode);
}

template <>
inline void Statement::Bind<CLong, unsigned int>(ostring name, CLong &value, unsigned int maxSize, BindInfo::BindDirection mode)
{
    Check(OCI_BindLong(*this, name.c_str(), value, maxSize));
    SetLastBindMode(mode);
}

template <>
inline void Statement::Bind<CLong, int>(ostring name, CLong &value, int maxSize, BindInfo::BindDirection mode)
{
    Bind<CLong, unsigned int>(name, value, (unsigned int) maxSize,  mode);
}


template <>
inline void Statement::Bind<BLong, unsigned int>(ostring name, BLong &value, unsigned int maxSize, BindInfo::BindDirection mode)
{
    Check(OCI_BindLong(*this, name.c_str(), value, maxSize));
    SetLastBindMode(mode);
}

template <>
inline void Statement::Bind<BLong, int>(ostring name, BLong &value, int maxSize, BindInfo::BindDirection mode)
{
    Bind<BLong, unsigned int>(name, value, (unsigned int) maxSize,  mode);
}

template <>
inline void Statement::Bind<ostring, unsigned int>(ostring name, ostring &value, unsigned int maxSize, BindInfo::BindDirection mode)
{
    if (maxSize == 0)
    {
        maxSize = (unsigned int) value.size();
    }

    BindString * bnd = new BindString(value, maxSize+1);
    BindsHolder *bindsHolder = GetBindsHolder(true);
    bindsHolder->AddBindObject(bnd);

    Check(OCI_BindString(*this, name.c_str(), (otext*) (*bnd), maxSize));
    SetLastBindMode(mode);
}

template <>
inline void Statement::Bind<ostring, int>(ostring name, ostring &value, int maxSize, BindInfo::BindDirection mode)
{
    Bind<ostring, unsigned int>(name, value, (unsigned int) maxSize,  mode);
}

template <>
inline void Statement::Bind<BufferPointer, unsigned int>(ostring name, BufferPointer &value, unsigned int maxSize, BindInfo::BindDirection mode)
{
    Check(OCI_BindRaw(*this, name.c_str(), value, maxSize));
    SetLastBindMode(mode);
}

template <>
inline void Statement::Bind<BufferPointer,  int>(ostring name, BufferPointer &value,  int maxSize, BindInfo::BindDirection mode)
{
     Bind<void *, unsigned int>(name, value, (unsigned int) maxSize,  mode);
}

template <>
inline void Statement::Bind<short>(ostring name, std::vector<short> &values, BindInfo::BindDirection mode)
{
    Bind(OCI_BindArrayOfShorts, name, values, BindValue<short>(), mode);
}

template <>
inline void Statement::Bind<unsigned short>(ostring name, std::vector<unsigned short> &values, BindInfo::BindDirection mode)
{
    Bind(OCI_BindArrayOfUnsignedShorts, name, values, BindValue<unsigned short>(), mode);
}

template <>
inline void Statement::Bind<int>(ostring name, std::vector<int> &values, BindInfo::BindDirection mode)
{
    Bind(OCI_BindArrayOfInts, name, values, BindValue<int>(), mode);
}

template <>
inline void Statement::Bind<unsigned int>(ostring name, std::vector<unsigned int> &values, BindInfo::BindDirection mode)
{
    Bind(OCI_BindArrayOfUnsignedInts, name, values, BindValue<unsigned int>(), mode);
}

template <>
inline void Statement::Bind<big_int>(ostring name, std::vector<big_int> &values, BindInfo::BindDirection mode)
{
    Bind(OCI_BindArrayOfBigInts, name, values, BindValue<big_int>(), mode);
}

template <>
inline void Statement::Bind<big_uint>(ostring name, std::vector<big_uint> &values, BindInfo::BindDirection mode)
{
    Bind(OCI_BindArrayOfUnsignedBigInts, name, values, BindValue<big_uint>(), mode);
}

template <>
inline void Statement::Bind<float>(ostring name, std::vector<float> &values, BindInfo::BindDirection mode)
{
    Bind(OCI_BindArrayOfFloats, name, values, BindValue<float>(), mode);
}

template <>
inline void Statement::Bind<double>(ostring name, std::vector<double> &values, BindInfo::BindDirection mode)
{
    Bind(OCI_BindArrayOfDoubles, name, values, BindValue<double>(), mode);
}

template <>
inline void Statement::Bind<Date>(ostring name, std::vector<Date> &values, BindInfo::BindDirection mode)
{
    Bind(OCI_BindArrayOfDates, name, values, BindValue<OCI_Date *>(), mode);
}

template <>
inline void Statement::Bind<Timestamp, unsigned int>(ostring name, std::vector<Timestamp> &values, unsigned int type, BindInfo::BindDirection mode)
{
    Bind(OCI_BindArrayOfTimestamps, name, values, BindValue<OCI_Timestamp *>(), mode, type);
}

template <>
inline void Statement::Bind<Interval, unsigned int>(ostring name, std::vector<Interval> &values, unsigned int type, BindInfo::BindDirection mode)
{
    Bind(OCI_BindArrayOfIntervals, name, values, BindValue<OCI_Interval *>(), mode, type);
}

template <>
inline void Statement::Bind<Clob>(ostring name, std::vector<Clob> &values, BindInfo::BindDirection mode)
{
    Bind(OCI_BindArrayOfLobs, name, values, BindValue<OCI_Lob *>(), mode, OCI_CLOB);
}

template <>
inline void Statement::Bind<Blob>(ostring name, std::vector<Blob> &values, BindInfo::BindDirection mode)
{
    Bind(OCI_BindArrayOfLobs, name, values, BindValue<OCI_Lob *>(), mode, OCI_BLOB);
}

template <>
inline void Statement::Bind<File>(ostring name, std::vector<File> &values, BindInfo::BindDirection mode)
{
    Bind(OCI_BindArrayOfFiles, name, values, BindValue<OCI_File *>(), mode, OCI_BFILE);
}

template <>
inline void Statement::Bind<Object, TypeInfo>(ostring name, std::vector<Object> &values, TypeInfo &typeInfo, BindInfo::BindDirection mode)
{
   Bind(OCI_BindArrayOfObjects, name, values, BindValue<OCI_Object *>(), mode, (OCI_TypeInfo *) typeInfo);
}

template <>
inline void Statement::Bind<Reference, TypeInfo>(ostring name, std::vector<Reference> &values, TypeInfo &typeInfo, BindInfo::BindDirection mode)
{
   Bind(OCI_BindArrayOfRefs, name, values, BindValue<OCI_Ref *>(), mode, (OCI_TypeInfo *) typeInfo);
}

template <>
inline void Statement::Bind<Collection, TypeInfo>(ostring name, std::vector<Collection> &values, TypeInfo &typeInfo, BindInfo::BindDirection mode)
{
   Bind(OCI_BindArrayOfColls, name, values, BindValue<OCI_Coll *>(), mode, (OCI_TypeInfo *) typeInfo);
}

template <>
inline void Statement::Bind<ostring, unsigned int>(ostring name, std::vector<ostring> &values,  unsigned int maxSize, BindInfo::BindDirection mode)
{
    BindArray * bnd = new BindArray();

    bnd->SetVector<ostring, otext>(values, mode, OCI_BindArrayGetSize(*this), maxSize+1);

    BindsHolder *bindsHolder = GetBindsHolder(true);
    bindsHolder->AddBindObject(bnd);

    Check(OCI_BindArrayOfStrings(*this, name.c_str(), bnd->GetData<ostring, otext>(), maxSize, 0));
    SetLastBindMode(mode);
}

template <>
inline void Statement::Bind<ostring, int>(ostring name, std::vector<ostring> &values, int maxSize, BindInfo::BindDirection mode)
{
    Bind<ostring, unsigned int>(name, values, ( unsigned int) maxSize, mode);
}

template <>
inline void Statement::Bind<BufferPointer, unsigned int>(ostring name, std::vector<BufferPointer> &values, unsigned int maxSize, BindInfo::BindDirection mode)
{
    BindArray * bnd = new BindArray();

    bnd->SetVector<BufferPointer, void *>(values, mode, OCI_BindArrayGetSize(*this), maxSize+1);

    BindsHolder *bindsHolder = GetBindsHolder(true);
    bindsHolder->AddBindObject(bnd);

    Check(OCI_BindArrayOfRaws(*this, name.c_str(), bnd->GetData<void *, void *>(), maxSize, 0));
    SetLastBindMode(mode);
}

template <>
inline void Statement::Register<unsigned short>(ostring name)
{
    Check(OCI_RegisterUnsignedShort(*this, name.c_str()));
}

template <>
inline void Statement::Register<short>(ostring name)
{
    Check(OCI_RegisterShort(*this, name.c_str()));
}

template <>
inline void Statement::Register<unsigned int>(ostring name)
{
    Check(OCI_RegisterUnsignedInt(*this, name.c_str()));
}

template <>
inline void Statement::Register<int>(ostring name)
{
    Check(OCI_RegisterInt(*this, name.c_str()));
}

template <>
inline void Statement::Register<big_uint>(ostring name)
{
    Check(OCI_RegisterUnsignedBigInt(*this, name.c_str()));
}

template <>
inline void Statement::Register<big_int>(ostring name)
{
    Check(OCI_RegisterBigInt(*this, name.c_str()));
}

template <>
inline void Statement::Register<float>(ostring name)
{
    Check(OCI_RegisterFloat(*this, name.c_str()));
}

template <>
inline void Statement::Register<double>(ostring name)
{
    Check(OCI_RegisterDouble(*this, name.c_str()));
}

template <>
inline void Statement::Register<Date>(ostring name)
{
    Check(OCI_RegisterDate(*this, name.c_str()));
}

template <>
inline void Statement::Register<Timestamp, unsigned int>(ostring name, unsigned int elemType)
{
    Check(OCI_RegisterTimestamp(*this, name.c_str(), elemType));
}

template <>
inline void Statement::Register<Interval, unsigned int>(ostring name, unsigned int elemType)
{
    Check(OCI_RegisterInterval(*this, name.c_str(), elemType));
}

template <>
inline void Statement::Register<Clob>(ostring name)
{
    Check(OCI_RegisterLob(*this, name.c_str(), OCI_CLOB));
}

template <>
inline void Statement::Register<Blob>(ostring name)
{
    Check(OCI_RegisterLob(*this, name.c_str(), OCI_BLOB));
}

template <>
inline void Statement::Register<File>(ostring name)
{
    Check(OCI_RegisterFile(*this, name.c_str(), OCI_BFILE));
}

template <>
inline void Statement::Register<Object, TypeInfo>(ostring name, TypeInfo& typeInfo)
{
    Check(OCI_RegisterObject(*this, name.c_str(), typeInfo));
}

template <>
inline void Statement::Register<Reference, TypeInfo>(ostring name, TypeInfo& typeInfo)
{
    Check(OCI_RegisterRef(*this, name.c_str(), typeInfo));
}

template <>
inline void Statement::Register<ostring, unsigned int>(ostring name, unsigned int len)
{
   Check(OCI_RegisterString(*this, name.c_str(), len));
}

template <>
inline void Statement::Register<ostring, int>(ostring name, int len)
{
    Register<ostring, unsigned int>(name,  (unsigned int) len);
}

template <>
inline void Statement::Register<BufferPointer, unsigned int>(ostring name, unsigned int len)
{
    Check(OCI_RegisterRaw(*this, name.c_str(), len));
}

template <>
inline void Statement::Register<BufferPointer, int>(ostring name, int len)
{
  Register<ostring, unsigned int>(name,(unsigned int) len);
}


inline Statement::StatementType Statement::GetStatementType() const
{
    return (StatementType) Check(OCI_GetStatementType(*this));
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
    return (FetchMode) Check(OCI_GetFetchMode(*this));
}

inline void Statement::SetBindMode(BindMode value)
{
    Check(OCI_SetBindMode(*this, value));
}

inline Statement::BindMode Statement::GetBindMode() const
{
    return (BindMode) Check(OCI_GetBindMode(*this));
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
    Check(OCI_SetLongMode(*this, (unsigned int) value));
}

inline Statement::LongMode Statement::GetLongMode() const
{
    return (LongMode) Check(OCI_GetLongMode(*this));
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

inline void Statement::SetLastBindMode(BindInfo::BindDirection mode)
{
    Check(OCI_BindSetDirection(Check(OCI_GetBind(*this, Check(OCI_GetBindCount(*this)))), (unsigned int) mode));
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

inline bool Resultset::Seek(SeekMode mode, int offset)
{
    return (Check(OCI_FetchSeek(*this, (unsigned int) mode, offset)) == TRUE);
}

inline unsigned int Resultset::GetCount() const
{
    return Check(OCI_GetRowCount(*this));
}

inline unsigned int Resultset::GetCurrentRow() const
{
    return Check(OCI_GetCurrentRow(*this));
}

inline unsigned int Resultset::GetColumnCount() const
{
    return Check(OCI_GetColumnCount(*this));
}

inline Column Resultset::GetColumn(unsigned int index) const
{
    return Column(Check(OCI_GetColumn(*this, index)), GetHandle());
}

inline Column Resultset::GetColumn(ostring name) const
{
    return Column(Check(OCI_GetColumn2(*this, name.c_str())), GetHandle());
}

inline bool Resultset::IsColumnNull(unsigned int index) const
{
    return (Check(OCI_IsNull(*this, index)) == TRUE);
}

inline bool Resultset::IsColumnNull(ostring name) const
{
    return (Check(OCI_IsNull2(*this, name.c_str())) == TRUE);
}

inline Statement Resultset::GetStatement() const
{
    return Statement( Check(OCI_ResultsetGetStatement(*this)), 0);
}

inline bool Resultset::operator ++ (int)
{
    return Next();
}

inline bool Resultset::operator -- (int)
{
    return Prev();
}

inline bool Resultset::operator += (int value)
{
    return Seek(Resultset::SeeKRelative, value);
}

inline bool Resultset::operator -= (int value)
{
    return Seek(Resultset::SeeKRelative, -value);
}

template<>
inline short Resultset::Get<short>(unsigned int index) const
{
    return Check(OCI_GetShort(*this, index));
}

template<>
inline short Resultset::Get<short>(ostring name) const
{
    return Check(OCI_GetShort2(*this, name.c_str()));
}

template<>
inline unsigned short Resultset::Get<unsigned short>(unsigned int index) const
{
    return Check(OCI_GetUnsignedShort(*this, index));
}

template<>
inline unsigned short Resultset::Get<unsigned short>(ostring name) const
{
    return Check(OCI_GetUnsignedShort2(*this, name.c_str()));
}

template<>
inline int Resultset::Get<int>(unsigned int index) const
{
    return Check(OCI_GetInt(*this, index));
}

template<>
inline int Resultset::Get<int>(ostring name) const
{
    return Check(OCI_GetInt2(*this, name.c_str()));
}

template<>
inline unsigned int Resultset::Get<unsigned int>(unsigned int index) const
{
    return Check(OCI_GetUnsignedInt(*this, index));
}

template<>
inline unsigned int Resultset::Get<unsigned int>(ostring name) const
{
    return Check(OCI_GetUnsignedInt2(*this, name.c_str()));
}

template<>
inline big_int Resultset::Get<big_int>(unsigned int index) const
{
    return Check(OCI_GetBigInt(*this, index));
}

template<>
inline big_int Resultset::Get<big_int>(ostring name) const
{
    return Check(OCI_GetBigInt2(*this, name.c_str()));
}

template<>
inline big_uint Resultset::Get<big_uint>(unsigned int index) const
{
    return Check(OCI_GetUnsignedBigInt(*this, index));
}

template<>
inline big_uint Resultset::Get<big_uint>(ostring name) const
{
    return Check(OCI_GetUnsignedBigInt2(*this, name.c_str()));
}

template<>
inline float Resultset::Get<float>(unsigned int index) const
{
    return Check(OCI_GetFloat(*this, index));
}

template<>
inline float Resultset::Get<float>(ostring name) const
{
    return Check(OCI_GetFloat2(*this, name.c_str()));
}

template<>
inline double Resultset::Get<double>(unsigned int index) const
{
    return Check(OCI_GetDouble(*this, index));
}

template<>
inline double Resultset::Get<double>(ostring name) const
{
    return Check(OCI_GetDouble2(*this, name.c_str()));
}

template<>
inline ostring Resultset::Get<ostring>(unsigned int index) const
{
    return MakeString(Check(OCI_GetString(*this, index)));
}

template<>
inline ostring Resultset::Get<ostring>(ostring name) const
{
    return MakeString(Check(OCI_GetString2(*this,name.c_str())));
}

template<>
inline void Resultset::Get<BufferPointer>(unsigned int index, BufferPointer value, unsigned int &size) const
{
    Check(OCI_GetRaw(*this, index, value, size));
}

template<>
inline void Resultset::Get<BufferPointer>(ostring name, BufferPointer value, unsigned int &size) const
{
    Check(OCI_GetRaw2(*this,name.c_str(), value, size));
}

template<>
inline Date Resultset::Get<Date>(unsigned int index) const
{
    return Date(Check(OCI_GetDate(*this, index)), GetHandle());
}

template<>
inline Date Resultset::Get<Date>(ostring name) const
{
    return Date(Check(OCI_GetDate2(*this,name.c_str())), GetHandle());
}

template<>
inline Timestamp Resultset::Get<Timestamp>(unsigned int index) const
{
    return Timestamp(Check(OCI_GetTimestamp(*this, index)), GetHandle());
}

template<>
inline Timestamp Resultset::Get<Timestamp>(ostring name) const
{
    return Timestamp(Check(OCI_GetTimestamp2(*this,name.c_str())), GetHandle());
}

template<>
inline Interval Resultset::Get<Interval>(unsigned int index) const
{
    return Interval(Check(OCI_GetInterval(*this, index)), GetHandle());
}

template<>
inline Interval Resultset::Get<Interval>(ostring name) const
{
    return Interval(Check(OCI_GetInterval2(*this,name.c_str())), GetHandle());
}

template<>
inline Object Resultset::Get<Object>(unsigned int index) const
{
    return Object(Check(OCI_GetObject(*this, index)), GetHandle());
}

template<>
inline Object Resultset::Get<Object>(ostring name) const
{
    return Object(Check(OCI_GetObject2(*this,name.c_str())), GetHandle());
}

template<>
inline Collection Resultset::Get<Collection>(unsigned int index) const
{
    return Collection(Check(OCI_GetColl(*this, index)), GetHandle());
}

template<>
inline Collection Resultset::Get<Collection>(ostring name) const
{
    return Collection(Check(OCI_GetColl2(*this,name.c_str())), GetHandle());
}

template<>
inline Reference Resultset::Get<Reference>(unsigned int index) const
{
    return Reference(Check(OCI_GetRef(*this, index)), GetHandle());
}

template<>
inline Reference Resultset::Get<Reference>(ostring name) const
{
    return Reference(Check(OCI_GetRef2(*this,name.c_str())), GetHandle());
}

template<>
inline Statement Resultset::Get<Statement>(unsigned int index) const
{
    return Statement(Check(OCI_GetStatement(*this, index)), GetHandle());
}

template<>
inline Statement Resultset::Get<Statement>(ostring name) const
{
    return Statement(Check(OCI_GetStatement2(*this,name.c_str())), GetHandle());
}

template<>
inline Clob Resultset::Get<Clob>(unsigned int index) const
{
    return Clob(Check(OCI_GetLob(*this, index)), GetHandle());
}

template<>
inline Clob Resultset::Get<Clob>(ostring name) const
{
    return Clob(Check(OCI_GetLob2(*this,name.c_str())), GetHandle());
}

template<>
inline Blob Resultset::Get<Blob>(unsigned int index) const
{
    return Blob(Check(OCI_GetLob(*this, index)), GetHandle());
}

template<>
inline Blob Resultset::Get<Blob>(ostring name) const
{
    return Blob(Check(OCI_GetLob2(*this,name.c_str())), GetHandle());
}

template<>
inline File Resultset::Get<File>(unsigned int index) const
{
    return File(Check(OCI_GetFile(*this, index)), GetHandle());
}

template<>
inline File Resultset::Get<File>(ostring name) const
{
    return File(Check(OCI_GetFile2(*this,name.c_str())), GetHandle());
}

template<>
inline CLong Resultset::Get<CLong>(unsigned int index) const
{
    return CLong(Check(OCI_GetLong(*this, index)), GetHandle());
}

template<>
inline CLong Resultset::Get<CLong>(ostring name) const
{
    return CLong(Check(OCI_GetLong2(*this,name.c_str())), GetHandle());
}

template<>
inline BLong Resultset::Get<BLong>(unsigned int index) const
{
    return BLong(Check(OCI_GetLong(*this, index)), GetHandle());
}

template<>
inline BLong Resultset::Get<BLong>(ostring name) const
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

    ManagedBuffer<otext> buffer = new otext[size+1];

    Check(OCI_ColumnGetFullSQLType(*this, buffer, size));

    return MakeString((otext *) buffer);
}

inline Column::ColumnType Column::GetType() const
{
    return (ColumnType) Check(OCI_ColumnGetType(*this));
}

inline unsigned int Column::GetSubType() const
{
    return Check(OCI_ColumnGetSubType(*this));
}

inline Environment::CharsetForm Column::GetCharsetForm() const
{
    return (Environment::CharsetForm) Check(OCI_ColumnGetCharsetForm(*this));
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
    Acquire(pSubcription, 0, 0);
}

inline void Subscription::Register(const Connection &con, ostring name, ChangeTypes changeTypes, NotifyHandlerProc handler, unsigned int port, unsigned int timeout)
{
    Acquire(Check(OCI_SubscriptionRegister(con, name.c_str(), (unsigned int) changeTypes, (POCI_NOTIFY) (handler != 0 ? Environment::NotifyHandler : 0 ), port, timeout)), (HandleFreeFunc) OCI_SubscriptionUnregister, 0);

    Environment::GetEnvironmentHandle().Callbacks.Set((OCI_Subscription*) *this, (CallbackPointer) handler);
}

inline void Subscription::Unregister()
{
    Environment::GetEnvironmentHandle().Callbacks.Remove(*this);
    Release();
}

inline void Subscription::Watch(ostring sql)
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
    return Connection(Check(OCI_SubscriptionGetConnection(*this)), 0);
}

/* --------------------------------------------------------------------------------------------- *
 * Event
 * --------------------------------------------------------------------------------------------- */

inline Event::Event(OCI_Event *pEvent)
{
    Acquire(pEvent, 0, 0);
}

inline Event::EventType Event::GetType() const
{
    return (EventType) Check(OCI_EventGetType(*this));
}

inline Event::ObjectEvent Event::GetObjectEvent() const
{
    return (ObjectEvent) Check(OCI_EventGetOperation(*this));
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

inline Agent::Agent(const Connection &connection, ostring name, ostring address)
{
    Acquire(Check(OCI_AgentCreate(connection, name.c_str(), address.c_str())), (HandleFreeFunc) OCI_AgentFree, 0);
}

inline Agent::Agent(OCI_Agent *pAgent, Handle *parent)
{
    Acquire(pAgent, 0, parent);
}

inline ostring Agent::GetName() const
{
    return MakeString(Check(OCI_AgentGetName(*this)));
}

inline void Agent::SetName(ostring value)
{
    Check(OCI_AgentSetName(*this, value.c_str()));
}

inline ostring Agent::GetAddress() const
{
    return MakeString(Check(OCI_AgentGetAddress(*this)));
}

inline void Agent::SetAddress(ostring value)
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

inline Date Message::GetEnqueueTime() const
{
    return Date(Check(OCI_MsgGetEnqueueTime(*this)), 0);
}

inline int Message::GetAttemptCount() const
{
    return Check(OCI_MsgGetAttemptCount(*this));
}

inline Message::MessageState Message::GetState() const
{
    return (MessageState) Check(OCI_MsgGetState(*this));
}

inline void Message::GetID(BufferPointer value, unsigned int &size) const
{
    Check(OCI_MsgGetID(*this, value, &size));
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

inline void Message::GetOriginalID(BufferPointer value, unsigned int &size) const
{
    Check(OCI_MsgGetOriginalID(*this, value, &size));
}

inline void Message::SetOriginalID(const BufferPointer &value, unsigned int size)
{
    Check(OCI_MsgSetOriginalID(*this, value, size));
}

inline ostring Message::GetCorrelation() const
{
    return MakeString(Check(OCI_MsgGetCorrelation(*this)));
}

inline void Message::SetCorrelation(ostring value)
{
    Check(OCI_MsgSetCorrelation(*this, value.c_str()));
}

inline ostring Message::GetExceptionQueue() const
{
    return MakeString(Check(OCI_MsgGetExceptionQueue(*this)));
}

inline void Message::SetExceptionQueue(ostring value)
{
    Check(OCI_MsgSetExceptionQueue(*this, value.c_str()));
}

inline Agent Message::GetSender() const
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

inline Enqueue::Enqueue(const TypeInfo &typeInfo, ostring queueName)
{
   Acquire(Check(OCI_EnqueueCreate(typeInfo, queueName.c_str())), (HandleFreeFunc) OCI_EnqueueFree, 0);
}

inline void Enqueue::Put(const Message &message)
{
    Check(OCI_EnqueuePut(*this, message));
}

inline Enqueue::EnqueueVisibility Enqueue::GetVisibility() const
{
    return (EnqueueVisibility) Check(OCI_EnqueueGetVisibility(*this));
}

inline void Enqueue::SetVisibility(EnqueueVisibility value)
{
    Check(OCI_EnqueueSetVisibility(*this, (unsigned int) value));
}

inline Enqueue::EnqueueMode Enqueue::GetMode() const
{
    return (EnqueueMode) Check(OCI_EnqueueGetSequenceDeviation(*this));
}

inline void Enqueue::SetMode(EnqueueMode value)
{
    Check(OCI_EnqueueSetSequenceDeviation(*this, (unsigned int) value));
}

inline void Enqueue::GetRelativeMsgID(BufferPointer value, unsigned int &size) const
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

inline Dequeue::Dequeue(const TypeInfo &typeInfo, ostring queueName)
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

inline ostring Dequeue::GetConsumer() const
{
    return MakeString(Check(OCI_DequeueGetConsumer(*this)));
}

inline void Dequeue::SetConsumer(ostring value)
{
    Check(OCI_DequeueSetConsumer(*this, value.c_str()));
}

inline ostring Dequeue::GetCorrelation() const
{
    return MakeString(Check(OCI_DequeueGetCorrelation(*this)));
}

inline void Dequeue::SetCorrelation(ostring value)
{
    Check(OCI_DequeueSetCorrelation(*this, value.c_str()));
}

inline void Dequeue::GetRelativeMsgID(BufferPointer value, unsigned int &size) const
{
    Check(OCI_DequeueGetRelativeMsgID(*this, value, &size));
}

inline void Dequeue::SetRelativeMsgID(const BufferPointer &value, unsigned int size)
{
    Check(OCI_DequeueSetRelativeMsgID(*this, value, size));
}

inline Dequeue::DequeueVisibility Dequeue::GetVisibility() const
{
    return (DequeueVisibility) Check(OCI_DequeueGetVisibility(*this));
}

inline void Dequeue::SetVisibility(DequeueVisibility value)
{
    Check(OCI_DequeueSetVisibility(*this, (unsigned int) value));
}

inline Dequeue::DequeueMode Dequeue::GetMode() const
{
    return (DequeueMode) Check(OCI_DequeueGetMode(*this));
}

inline void Dequeue::SetMode(DequeueMode value)
{
    Check(OCI_DequeueSetMode(*this, (unsigned int) value));
}

inline Dequeue::NavigationMode Dequeue::GetNavigation() const
{
    return (NavigationMode) Check(OCI_DequeueGetNavigation(*this));
}

inline void Dequeue::SetNavigation(NavigationMode value)
{
    Check(OCI_DequeueSetNavigation(*this, (unsigned int) value));
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

inline DirectPath::DirectPath(const TypeInfo &typeInfo, unsigned int nbCols, unsigned int  nbRows, ostring partition)
{
    Acquire(Check(OCI_DirPathCreate(typeInfo, partition.c_str(), nbCols, nbRows)), (HandleFreeFunc) OCI_DirPathFree, 0);
}

inline void DirectPath::SetColumn(unsigned int colIndex, ostring name, unsigned int maxSize,  ostring format)
{
    Check(OCI_DirPathSetColumn(*this, colIndex, name.c_str(), maxSize, format.c_str()));
}

inline void DirectPath::SetEntry(unsigned int rowIndex, unsigned int colIndex,  const ostring &value,  bool complete)
{
    Check(OCI_DirPathSetEntry(*this, rowIndex, colIndex, (void *) value.c_str(), (unsigned int) value.size(), complete));
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

inline DirectPath::Result DirectPath::Convert()
{
    return (Result) Check(OCI_DirPathConvert(*this));
}

inline DirectPath::Result DirectPath::Load()
{
    return (Result) Check(OCI_DirPathLoad(*this));
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

inline void DirectPath::SetDateFormat(ostring format)
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
    Check(OCI_DirPathSetConvertMode(*this, (unsigned int) value));
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

inline void Queue::Create(const Connection &connection, ostring queue, ostring table, QueueType queueType, unsigned int maxRetries,
            unsigned int retryDelay, unsigned int retentionTime, bool dependencyTracking, ostring comment)
{
    Check(OCI_QueueCreate(connection, queue.c_str(), table.c_str(), (unsigned int) queueType, maxRetries, retryDelay, retentionTime, dependencyTracking, comment.c_str()));
}

inline void Queue::Alter(const Connection &connection, ostring queue, unsigned int maxRetries, unsigned int retryDelay, unsigned int retentionTime, ostring comment)
{
    Check(OCI_QueueAlter(connection, queue.c_str(), maxRetries, retryDelay, retentionTime, comment.c_str()));
}

inline void Queue::Drop(const Connection &connection, ostring queue)
{
    Check(OCI_QueueDrop(connection, queue.c_str()));
}

inline void Queue::Start(const Connection &connection, ostring queue, bool startEnqueue, bool startDequeue)
{
    Check(OCI_QueueStart(connection, queue.c_str(), startEnqueue, startDequeue));
}

inline void Queue::Stop(const Connection &connection, ostring queue, bool stopEnqueue, bool stopDequeue, bool wait)
{
    Check(OCI_QueueStop(connection, queue.c_str(), stopEnqueue, stopDequeue, wait));
}

/* --------------------------------------------------------------------------------------------- *
 * QueueTable
 * --------------------------------------------------------------------------------------------- */

inline void QueueTable::Create(const Connection &connection, ostring table, ostring payloadType, bool multipleConsumers, ostring storageClause, ostring sortList,
                   GroupingMode groupingMode, ostring comment, unsigned int primaryInstance, unsigned int secondaryInstance, ostring compatible)

{
    Check(OCI_QueueTableCreate(connection, table.c_str(), payloadType.c_str(), storageClause.c_str(), sortList.c_str(), multipleConsumers,
                                   (unsigned int) groupingMode, comment.c_str(), primaryInstance, secondaryInstance, compatible.c_str()));
}

inline void QueueTable::Alter(const Connection &connection, ostring table, ostring comment, unsigned int primaryInstance, unsigned int secondaryInstance)
{
    Check(OCI_QueueTableAlter(connection, table.c_str(), comment.c_str(), primaryInstance, secondaryInstance));
}

inline void QueueTable::Drop(const Connection &connection, ostring table, bool force)
{
    Check(OCI_QueueTableDrop(connection, table.c_str(), force));
}

inline void QueueTable::Purge(const Connection &connection, ostring table, PurgeMode purgeMode, ostring purgeCondition, bool block)
{
    Check(OCI_QueueTablePurge(connection, table.c_str(), purgeCondition.c_str(), block, (unsigned int) purgeMode));
}

/**
 * @brief
 * Migrate a queue table from one version to another
 *
 * @note
 * refer to OCI_QueueTableMigrate() documentation
 *
 */
inline void QueueTable::Migrate(const Connection &connection, ostring table, ostring compatible)
{
    Check(OCI_QueueTableMigrate(connection, table.c_str(), compatible.c_str()));
}

/**
 * @}
 */

