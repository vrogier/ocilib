/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2023 Vincent ROGIER <vince.rogier@ocilib.net>
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

#pragma once

#include "ocilibcpp/types.hpp"

// ReSharper disable CppClangTidyPerformanceUnnecessaryValueParam
// ReSharper disable CppClangTidyHicppUseAuto
// ReSharper disable CppClangTidyModernizeUseAuto

namespace ocilib
{
    
inline void Environment::Initialize(EnvironmentFlags mode, const ostring& libpath)
{
    GetInstance().SelfInitialize(mode, libpath);
}

inline void Environment::Cleanup()
{
    GetInstance().SelfCleanup();

	Environment* handle = static_cast<Environment*>(OCI_GetUserData(nullptr));	
	OCI_SetUserData(nullptr, handle);

#ifdef OCILIBPP_DEBUG_MEMORY_ENABLED

    core::GetMemoryDebugInfo().PrintAllocations();

#endif

}

inline Environment::EnvironmentFlags Environment::GetMode()
{
    return GetInstance()._mode;
}

inline Environment::ImportMode Environment::GetImportMode()
{
    return ImportMode(static_cast<ImportMode::Type>(core::Check(OCI_GetImportMode())));
}

inline Environment::CharsetMode Environment::GetCharset()
{
    return CharsetMode(static_cast<CharsetMode::Type>(core::Check(OCI_GetCharset())));
}

inline unsigned int Environment::GetCharMaxSize()
{
    return GetInstance()._charMaxSize;
}

inline big_uint Environment::GetAllocatedBytes(AllocatedBytesFlags type)
{
    return core::Check(OCI_GetAllocatedBytes(type.GetValues()));
}

inline bool Environment::Initialized()
{
    return GetInstance()._initialized;
}

inline OracleVersion Environment::GetCompileVersion()
{
    return OracleVersion(static_cast<OracleVersion::Type>(core::Check(OCI_GetOCICompileVersion())));
}

inline OracleVersion Environment::GetRuntimeVersion()
{
    return  OracleVersion(static_cast<OracleVersion::Type>(core::Check(OCI_GetOCIRuntimeVersion())));
}

inline unsigned int Environment::GetCompileMajorVersion()
{
    return OCI_VER_MAJ(core::Check(OCI_GetOCICompileVersion()));
}

inline unsigned int Environment::GetCompileMinorVersion()
{
    return OCI_VER_MIN(core::Check(OCI_GetOCICompileVersion()));
}

inline unsigned int Environment::GetCompileRevisionVersion()
{
    return OCI_VER_REV(core::Check(OCI_GetOCICompileVersion()));
}

inline unsigned int Environment::GetRuntimeMajorVersion()
{
    return OCI_VER_MAJ(core::Check(OCI_GetOCIRuntimeVersion()));
}

inline unsigned int Environment::GetRuntimeMinorVersion()
{
    return OCI_VER_MIN(core::Check(OCI_GetOCIRuntimeVersion()));
}

inline unsigned int Environment::GetRuntimeRevisionVersion()
{
    return OCI_VER_REV(core::Check(OCI_GetOCIRuntimeVersion()));
}

inline void Environment::EnableWarnings(bool value)
{
    OCI_EnableWarnings(static_cast<boolean>(value));
}

inline bool Environment::SetFormat(FormatType formatType, const ostring& format)
{
    return core::Check(OCI_SetFormat(nullptr, formatType, format.c_str()) == TRUE);
}

inline ostring Environment::GetFormat(FormatType formatType)
{
    return core::MakeString(core::Check(OCI_GetFormat(nullptr, formatType)));
}

inline void Environment::StartDatabase(const ostring& db, const ostring& user, const ostring &pwd, Environment::StartFlags startFlags,
                                       Environment::StartMode startMode, Environment::SessionFlags sessionFlags, const ostring& spfile)
{
    core::Check(OCI_DatabaseStartup(db.c_str(), user.c_str(), pwd.c_str(), sessionFlags.GetValues(),
                              startMode.GetValues(), startFlags.GetValues(), spfile.c_str() ));
}

inline void Environment::ShutdownDatabase(const ostring& db, const ostring& user, const ostring &pwd, Environment::ShutdownFlags shutdownFlags,
                                       Environment::ShutdownMode shutdownMode, Environment::SessionFlags sessionFlags)
{
   core::Check(OCI_DatabaseShutdown(db.c_str(), user.c_str(), pwd.c_str(), sessionFlags.GetValues(),
                              shutdownMode.GetValues(),  shutdownFlags.GetValues() ));
}

inline void Environment::ChangeUserPassword(const ostring& db, const ostring& user, const ostring& pwd, const ostring& newPwd)
{
    core::Check(OCI_SetUserPassword(db.c_str(), user.c_str(), pwd.c_str(), newPwd.c_str()));
}

inline void Environment::SetHAHandler(HAHandlerProc handler)
{
    core::Check(OCI_SetHAHandler(static_cast<POCI_HA_HANDLER>(handler != nullptr ? Environment::HAHandler : nullptr)));

    SetUserCallback<HAHandlerProc>(GetEnvironmentHandle(), handler);
}

inline void Environment::HAHandler(OCI_Connection *pConnection, unsigned int source, unsigned int event, OCI_Timestamp  *pTimestamp)
{
    const HAHandlerProc handler = GetUserCallback<HAHandlerProc>(GetEnvironmentHandle());

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

    const Connection::TAFHandlerProc handler = GetUserCallback<Connection::TAFHandlerProc>(core::Check(pConnection));

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
    const Subscription::NotifyHandlerProc handler = GetUserCallback<Subscription::NotifyHandlerProc>((core::Check(OCI_EventGetSubscription(pEvent))));

    if (handler)
    {
        Event evt(pEvent);
        handler(evt);
    }
}

inline void Environment::NotifyHandlerAQ(OCI_Dequeue *pDequeue)
{
    const Dequeue::NotifyAQHandlerProc handler = GetUserCallback<Dequeue::NotifyAQHandlerProc>(core::Check(pDequeue));

    if (handler)
    {
        Dequeue dequeue(pDequeue, Environment::GetEnvironmentHandle());
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

inline core::Handle * Environment::GetEnvironmentHandle()
{
    return GetInstance()._handle.GetHandle();
}

inline Environment& Environment::GetInstance()
{
	Environment* handle = static_cast<Environment*>(OCI_GetUserData(nullptr));
    if (handle != nullptr)
    {
        return *handle;
    }

    static Environment environment;

    OCI_SetUserData(nullptr,&environment);

    return environment;
}

inline Environment::Environment() : _charMaxSize(0), _initialized(false), _guard(core::SynchronizationMode::Unsafe)
{

}

inline void Environment::SelfInitialize(EnvironmentFlags mode, const ostring& libpath)
{
    _mode = mode;

    core::Check(OCI_Initialize(nullptr, libpath.c_str(), _mode.GetValues() | OCI_ENV_CONTEXT));

    _initialized = true;

    _guard.SetMode((_mode & Environment::Threaded) == Environment::Threaded ? core::SynchronizationMode::Safe : core::SynchronizationMode::Unsafe);

    _callbacks.SetGuard(&_guard);

    _handle.AcquireTransient
    (
        /* returned value IS NOT an OCI_Environment* but OCIEnv* direct handle 
           to be changed when C API we have public methods for OCI_Environment */
        static_cast<OCI_Environment*>(const_cast<AnyPointer>(core::Check(OCI_HandleGetEnvironment()))),
        nullptr
    );

    _charMaxSize = ComputeCharMaxSize(GetCharset());
}

inline void Environment::SelfCleanup()
{
    _guard.SetMode(core::SynchronizationMode::Unsafe);

    _callbacks.SetGuard(nullptr);

    _handle.Release();

    if (_initialized)
    {
        core::Check(OCI_Cleanup());
    }

    _initialized = false;
}

}
