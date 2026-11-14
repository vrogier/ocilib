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

namespace ocilib
{

    inline Connection::Connection()
    {

    }

    inline Connection::Connection(const Environment& env, const String& db, const String& user, const String& pwd, Environment::SessionFlags sessionFlags,
                                  const ShardingKey& skey, const ShardingKey& sskey)
    {
        Open(env, db, user, pwd, sessionFlags, skey, sskey);
    }

    inline Connection::Connection(const Environment& env, const String& db, const String& user, const String& pwd, Environment::SessionFlags sessionFlags)
    {
        Open(env, db, user, pwd, sessionFlags);
    }

    inline Connection::Connection(OCI_Connection *con,  core::Handle *parent, bool allocated)
    {
        if (allocated)
        {
            AcquireAllocated(con, parent);
        }
        else
        {
            AcquireTransient(con, parent);
        }
    }

    inline void Connection::Open(const Environment& env, const String& db, const String& user, const String& pwd, Environment::SessionFlags sessionFlags)
    {
        AcquireAllocated
        (
            core::Check(OCI_ConnectionCreate(env.GetHandle(), db.c_str(), user.c_str(), pwd.c_str(), sessionFlags.GetValues())),
            env.GetEnvironmentHandle()
        );
    }

    inline void Connection::Open(const Environment& env, const String& db, const String& user, const String& pwd, Environment::SessionFlags sessionFlags,
                                 const ShardingKey& skey, const ShardingKey& sskey)
    {
        AcquireAllocated
        (
            core::Check(OCI_ConnectionCreateSharded(env.GetHandle(), db.c_str(), user.c_str(), pwd.c_str(), sessionFlags.GetValues(), skey, sskey)),
            env.GetEnvironmentHandle()
        );
    }

    inline void Connection::Close()
    {
        Release();
    }

    inline void Connection::Commit()
    {
        core::Check(OCI_ConnectionCommit(*this));
    }

    inline void Connection::Rollback()
    {
        core::Check(OCI_ConnectionRollback(*this));
    }

    inline void Connection::Break()
    {
        core::Check(OCI_ConnectionBreak(*this));
    }

    inline void Connection::SetAutoCommit(bool enabled)
    {
        core::Check(OCI_ConnectionSetAutoCommit(*this, enabled));
    }

    inline bool Connection::GetAutoCommit() const
    {
        return (core::Check(OCI_ConnectionGetAutoCommit(*this)) == TRUE);
    }

    inline bool Connection::IsServerAlive() const
    {
        return (core::Check(OCI_ConnectionIsAlive(*this)) == TRUE);
    }

    inline bool Connection::PingServer() const
    {
        return( core::Check(OCI_ConnectionPing(*this)) == TRUE);
    }

    inline String Connection::GetConnectionString() const
    {
        return core::MakeString(core::Check(OCI_ConnectionGetConnectionString(*this)));
    }

    inline String Connection::GetUserName() const
    {
        return core::MakeString(core::Check(OCI_ConnectionGetUserName(*this)));
    }

    inline String Connection::GetPassword() const
    {
        return core::MakeString(core::Check(OCI_ConnectionGetPassword(*this)));
    }

    inline OracleVersion Connection::GetVersion() const
    {
        return OracleVersion(static_cast<OracleVersion::Type>(core::Check(OCI_ConnectionGetVersion(*this))));
    }

    inline String Connection::GetServerVersion() const
    {
        return core::MakeString(core::Check(OCI_ConnectionGetServerVersionString(*this)));
    }

    inline UInt32 Connection::GetServerMajorVersion() const
    {
        return core::Check(OCI_ConnectionGetServerMajorVersion(*this));
    }

    inline UInt32 Connection::GetServerMinorVersion() const
    {
        return core::Check(OCI_ConnectionGetServerMinorVersion(*this));
    }

    inline UInt32 Connection::GetServerRevisionVersion() const
    {
        return core::Check(OCI_ConnectionGetServerRevisionVersion(*this));
    }

    inline void Connection::ChangePassword(const String& newPwd)
    {
        core::Check(OCI_ConnectionSetPassword(*this, newPwd.c_str()));
    }

    inline String Connection::GetSessionTag() const
    {
        return core::MakeString(core::Check(OCI_ConnectionGetSessionTag(*this)));
    }

    inline void Connection::SetSessionTag(const String& tag)
    {
        core::Check(OCI_ConnectionSetSessionTag(*this, tag.c_str()));
    }

    inline Transaction Connection::GetTransaction() const
    {
        return Transaction(core::Check(OCI_ConnectionGetTransaction(*this)));
    }

    inline void Connection::SetTransaction(const Transaction &transaction)
    {
        core::Check(OCI_ConnectionSetTransaction(*this, transaction));
    }

    inline void Connection::EnableServerOutput(UInt32 bufsize, UInt32 arrsize, UInt32 lnsize)
    {
        core::Check(OCI_ConnectionEnableServerOutput(*this, bufsize, arrsize, lnsize));
    }

    inline void Connection::DisableServerOutput()
    {
        core::Check(OCI_ConnectionDisableServerOutput(*this));
    }

    inline bool Connection::GetServerOutput(String &line) const
    {
        const otext * str = core::Check(OCI_ConnectionGetServerOutput(*this));

        line = core::MakeString(str);

        return (str != nullptr);
    }

    inline void Connection::GetServerOutput(std::vector<String> &lines) const
    {
        const otext * str = core::Check(OCI_ConnectionGetServerOutput(*this));

        while (str)
        {
            lines.push_back(str);
            str = core::Check(OCI_ConnectionGetServerOutput(*this));
        }
    }

    inline void Connection::SetTrace(SessionTrace trace, const String& value)
    {
        core::Check(OCI_ConnectionSetTrace(*this, trace, value.c_str()));
    }

    inline String Connection::GetTrace(SessionTrace trace) const
    {
        return core::MakeString(core::Check(OCI_ConnectionGetTrace(*this, trace)));
    }

    inline String Connection::GetDatabase() const
    {
        return core::MakeString(core::Check(OCI_ConnectionGetDatabaseName(*this)));
    }

    inline String Connection::GetInstance() const
    {
        return core::Check(OCI_ConnectionGetInstanceName(*this));
    }

    inline String Connection::GetService() const
    {
        return core::MakeString(core::Check(OCI_ConnectionGetServiceName(*this)));
    }

    inline String Connection::GetServer() const
    {
        return core::Check(OCI_ConnectionGetServerName(*this));
    }

    inline String Connection::GetDomain() const
    {
        return core::MakeString(core::Check(OCI_ConnectionGetDomainName(*this)));
    }

    inline Timestamp Connection::GetInstanceStartTime() const
    {
        return Timestamp(core::Check(OCI_ConnectionGetInstanceStartTime(*this)), GetHandle());
    }

    inline UInt32 Connection::GetStatementCacheSize() const
    {
        return core::Check(OCI_ConnectionGetStatementCacheSize(*this));
    }

    inline void Connection::SetStatementCacheSize(UInt32 value)
    {
        core::Check(OCI_ConnectionSetStatementCacheSize(*this, value));
    }

    inline UInt32 Connection::GetDefaultLobPrefetchSize() const
    {
        return core::Check(OCI_ConnectionGetDefaultLobPrefetchSize(*this));
    }

    inline void Connection::SetDefaultLobPrefetchSize(UInt32 value)
    {
        core::Check(OCI_ConnectionSetDefaultLobPrefetchSize(*this, value));
    }

    inline UInt32 Connection::GetMaxCursors() const
    {
        return core::Check(OCI_ConnectionGetMaxCursors(*this));
    }

    inline bool Connection::IsTAFCapable() const
    {
        return (core::Check(OCI_ConnectionIsTAFCapable(*this)) == TRUE);
    }

    inline void Connection::SetTAFHandler(TAFHandlerProc handler)
    {
        core::Check(OCI_ConnectionSetTAFHandler(*this, static_cast<POCI_TAF_HANDLER>(handler != nullptr ? Environment::TAFHandler : nullptr)));

        Environment* env = Environment::GetInstance(GetEnvironment());
        if (env)
        {
            env->SetUserCallback<Connection::TAFHandlerProc>(static_cast<OCI_Connection*>(*this), handler);
        }
    }

    inline AnyPointer Connection::GetUserData()
    {
        return core::Check(OCI_HandleGetUserData(*this));
    }

    inline void Connection::SetUserData(AnyPointer value)
    {
        core::Check(OCI_HandleSetUserData(*this, value));
    }

    inline UInt32 Connection::GetTimeout(TimeoutType timeout)
    {
        return core::Check(OCI_ConnectionGetTimeout(*this, timeout));
    }

    inline void Connection::SetTimeout(TimeoutType timeout, UInt32 value)
    {
        core::Check(OCI_ConnectionSetTimeout(*this, timeout, value));
    }

}
