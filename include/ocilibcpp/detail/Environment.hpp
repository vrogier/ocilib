/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2026 Vincent ROGIER <vince.rogier@gmail.com>
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
#include "ocilibcpp/detail/core/HandleStore.hpp"

namespace ocilib
{

    inline Environment::Environment(EnvironmentFlags mode, const String& libpath) : _charMaxSize(0), _initialized(false),
        _store(std::make_shared<core::HandleStore>())
    {
        _mode = mode;

        OCI_Environment* env = OCI_EnvironmentCreate(nullptr, libpath.c_str(), _mode.GetValues());

        if (env)
        {
            core::Check(TRUE);

            _handle.Acquire(env, true, nullptr, _store);
            _initialized = true;

            OCI_HandleSetUserData(env, this);

            _charMaxSize = core::ComputeCharMaxSize(GetCharset());
        }
    }

    inline Environment::~Environment()
    {
        if (GetHandle())
        {
            OCI_HandleSetUserData(GetHandle(), nullptr);
        }

        _handle.Release();
        _initialized = false;
    }

    inline Environment::EnvironmentFlags Environment::GetMode() const
    {
        return _mode;
    }

    inline Environment::ImportMode Environment::GetImportMode() const
    {
        return ImportMode(static_cast<ImportMode::Type>(core::Check(OCI_EnvironmentGetImportMode(GetHandle()))));
    }

    inline Environment::CharsetMode Environment::GetCharset() const
    {
        return CharsetMode(static_cast<CharsetMode::Type>(core::Check(OCI_EnvironmentGetCharset(GetHandle()))));
    }

    inline UInt32 Environment::GetCharMaxSize() const
    {
        return _charMaxSize;
    }

    inline UInt64 Environment::GetStats(OCI_Stat stat) const
    {
        return core::Check(OCI_EnvironmentGetStats(GetHandle(), stat));
    }

    inline OracleVersion Environment::GetCompileVersion() const
    {
        return OracleVersion(static_cast<OracleVersion::Type>(core::Check(OCI_EnvironmentGetOCICompileVersion(GetHandle()))));
    }

    inline OracleVersion Environment::GetRuntimeVersion() const
    {
        return OracleVersion(static_cast<OracleVersion::Type>(core::Check(OCI_EnvironmentGetOCIRuntimeVersion(GetHandle()))));
    }

    inline UInt32 Environment::GetCompileMajorVersion() const
    {
        return OCI_VER_MAJ(core::Check(OCI_EnvironmentGetOCICompileVersion(GetHandle())));
    }

    inline UInt32 Environment::GetCompileMinorVersion() const
    {
        return OCI_VER_MIN(core::Check(OCI_EnvironmentGetOCICompileVersion(GetHandle())));
    }

    inline UInt32 Environment::GetCompileRevisionVersion() const
    {
        return OCI_VER_REV(core::Check(OCI_EnvironmentGetOCICompileVersion(GetHandle())));
    }

    inline UInt32 Environment::GetRuntimeMajorVersion() const
    {
        return OCI_VER_MAJ(core::Check(OCI_EnvironmentGetOCIRuntimeVersion(GetHandle())));
    }

    inline UInt32 Environment::GetRuntimeMinorVersion() const
    {
        return OCI_VER_MIN(core::Check(OCI_EnvironmentGetOCIRuntimeVersion(GetHandle())));
    }

    inline UInt32 Environment::GetRuntimeRevisionVersion() const
    {
        return OCI_VER_REV(core::Check(OCI_EnvironmentGetOCIRuntimeVersion(GetHandle())));
    }

    inline void Environment::EnableWarnings(bool value)
    {
        OCI_EnvironmentEnableWarnings(GetHandle(), static_cast<OCI_Boolean>(value));
    }

    inline bool Environment::SetFormat(FormatType formatType, const String& format)
    {
        return core::Check(OCI_EnvironmentSetFormat(GetHandle(), formatType, format.c_str()) == TRUE);
    }

    inline String Environment::GetFormat(FormatType formatType) const
    {
        return core::MakeString(core::Check(OCI_EnvironmentGetFormat(GetHandle(), formatType)));
    }

    inline String Environment::GetLocaleString(LocaleCode code) const
    {
        return core::MakeString(core::Check(OCI_EnvironmentGetLocaleString(GetHandle(), code)));
    }

    inline void Environment::StartDatabase(const String& db, const String& user, const String &pwd, Environment::StartFlags startFlags,
                                           Environment::StartMode startMode, Environment::SessionFlags sessionFlags, const String& spfile)
    {
        core::Check(OCI_DatabaseStartup(GetHandle(), db.c_str(), user.c_str(), pwd.c_str(), sessionFlags.GetValues(),
                                        startMode.GetValues(), startFlags.GetValues(), spfile.c_str()));
    }

    inline void Environment::ShutdownDatabase(const String& db, const String& user, const String &pwd, Environment::ShutdownFlags shutdownFlags,
                                              Environment::ShutdownMode shutdownMode, Environment::SessionFlags sessionFlags)
    {
        core::Check(OCI_DatabaseShutdown(GetHandle(), db.c_str(), user.c_str(), pwd.c_str(), sessionFlags.GetValues(),
                                         shutdownMode.GetValues(),  shutdownFlags.GetValues()));
    }

    inline void Environment::ChangeUserPassword(const String& db, const String& user, const String& pwd, const String& newPwd)
    {
        core::Check(OCI_EnvironmentSetUserPassword(GetHandle(), db.c_str(), user.c_str(), pwd.c_str(), newPwd.c_str()));
    }

    inline void Environment::SetHAHandler(HAHandlerProc handler)
    {
        core::Check(OCI_EnvironmentSetHAHandler(GetHandle(), static_cast<POCI_HA_HANDLER>(handler != nullptr ? Environment::HAHandler : nullptr)) == TRUE);

        SetUserCallback<HAHandlerProc>(GetEnvironmentHandle(), handler);
    }

    inline void Environment::SetSubscriptionPort(UInt32 port)
    {
        core::Check(OCI_EnvironmentSetSubscriptionPort(GetHandle(), port) == TRUE);
    }

    inline UInt32 Environment::GetSubscriptionPort() const
    {
        return core::Check(OCI_EnvironmentGetSubscriptionPort(GetHandle()));
    }

    inline Environment* Environment::GetInstance(OCI_Environment* env)
    {
        return env ? static_cast<Environment*>(OCI_HandleGetUserData(env)) : nullptr;
    }

    namespace core
    {
        inline std::shared_ptr<HandleStore> HandleStore::ResolveStore(Handle* parent, AnyPointer handle)
        {
            if (parent)
            {
                return parent->GetStore();
            }

            if (handle != nullptr)
            {
                Environment* e = Environment::GetInstance(OCI_HandleGetEnvironment(handle));

                if (e != nullptr)
                {
                    return e->_store;
                }
            }

            return std::shared_ptr<HandleStore>();
        }
    }

    inline void Environment::HAHandler(OCI_Connection *pConnection, UInt32 source, UInt32 event, OCI_Timestamp  *pTimestamp)
    {
        OCI_Environment* env = pConnection ? OCI_HandleGetEnvironment(pConnection) : nullptr;
        Environment    * self = GetInstance(env);

        if (!self)
        {
            return;
        }

        auto cb = self->GetUserCallback<HAHandlerProc>(self->GetEnvironmentHandle());

        if (cb)
        {
            Connection con(pConnection, self->GetEnvironmentHandle(), true);
            Timestamp  time(pTimestamp, self->GetEnvironmentHandle());

            cb(con, HAEventSource(static_cast<HAEventSourceValues>(source)), HAEventType(static_cast<HAEventTypeValues>(event)), time);
        }
    }

    inline UInt32 Environment::TAFHandler(OCI_Connection *pConnection, UInt32 type, UInt32 event)
    {
        OCI_Environment* env = pConnection ? OCI_HandleGetEnvironment(pConnection) : nullptr;
        Environment    * self = GetInstance(env);

        if (!self)
        {
            return OCI_FOC_OK;
        }

        auto cb = self->GetUserCallback<Connection::TAFHandlerProc>(pConnection);

        if (cb)
        {
            Connection con(pConnection, self->GetEnvironmentHandle(), true);

            return static_cast<UInt32>(cb(con, Connection::FailoverRequest(static_cast<Connection::FailoverRequestValues>(type)),
                                          Connection::FailoverEvent(static_cast<Connection::FailoverEventValues>(event))));
        }

        return OCI_FOC_OK;
    }

    inline void Environment::NotifyHandler(OCI_Event *pEvent)
    {
        OCI_Subscription* sub = pEvent ? OCI_EventGetSubscription(pEvent) : nullptr;
        OCI_Environment * env = pEvent ? OCI_HandleGetEnvironment(pEvent) : nullptr;
        Environment     * self = GetInstance(env);

        if (!self)
        {
            return;
        }

        auto cb = self->GetUserCallback<Subscription::NotifyHandlerProc>(sub);

        if (cb)
        {
            Event evt(pEvent);

            cb(evt);
        }
    }

    inline void Environment::NotifyHandlerAQ(OCI_Dequeue *pDequeue)
    {
        OCI_Environment* env = pDequeue ? OCI_HandleGetEnvironment(pDequeue) : nullptr;
        Environment    * self = GetInstance(env);

        if (!self)
        {
            return;
        }

        auto cb = self->GetUserCallback<Dequeue::NotifyAQHandlerProc>(pDequeue);

        if (cb)
        {
            Dequeue dequeue(pDequeue, self->GetEnvironmentHandle());

            cb(dequeue);
        }
    }

    template<class T>
    inline std::function<typename std::remove_pointer<T>::type> Environment::GetUserCallback(AnyPointer ptr)
    {
        using Signature = typename std::remove_pointer<T>::type;

        std::lock_guard<std::mutex> lock(_callbacksMutex);

        auto it = _callbacks.find(ptr);
        if (it == _callbacks.end())
        {
            return std::function<Signature>();
        }

        Callback<Signature>* cb = dynamic_cast<Callback<Signature>*>(it->second.get());
        return cb ? cb->func : std::function<Signature>();
    }

    template<class T>
    inline void Environment::SetUserCallback(AnyPointer ptr, T callback)
    {
        using Signature = typename std::remove_pointer<T>::type;

        std::lock_guard<std::mutex> lock(_callbacksMutex);

        if (callback)
        {
            auto cb = std::make_unique<Callback<Signature> >();
            cb->func = std::function<Signature>(callback);
            _callbacks[ptr] = std::move(cb);
        }
        else
        {
            _callbacks.erase(ptr);
        }
    }

    inline OCI_Environment* Environment::GetHandle() const
    {
        return static_cast<OCI_Environment*>(_handle);
    }

    inline core::Handle * Environment::GetEnvironmentHandle() const
    {
        return _handle.GetHandleControl();
    }

}
