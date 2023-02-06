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

// ReSharper disable CppClangTidyHicppUseEqualsDefault
// ReSharper disable CppClangTidyModernizeUseEqualsDefault
// ReSharper disable CppClangTidyPerformanceUnnecessaryValueParam
// ReSharper disable CppClangTidyHicppUseEmplace

namespace ocilib
{

inline Connection::Connection()
{

}

inline Connection::Connection(const ostring& db, const ostring& user, const ostring& pwd, Environment::SessionFlags sessionFlags)
{
    Open(db, user, pwd, sessionFlags);
}

inline Connection::Connection(OCI_Connection *con,  core::Handle *parent)
{
    AcquireTransient(con, parent);
}

inline void Connection::Open(const ostring& db, const ostring& user, const ostring& pwd, Environment::SessionFlags sessionFlags)
{
    AcquireAllocated
    (
        core::Check(OCI_ConnectionCreate(db.c_str(), user.c_str(), pwd.c_str(), sessionFlags.GetValues())),
        Environment::GetEnvironmentHandle()
    );
}

inline void Connection::Close()
{
    Release();
}

inline void Connection::Commit()
{
    core::Check(OCI_Commit(*this));
}

inline void Connection::Rollback()
{
    core::Check(OCI_Rollback(*this));
}

inline void Connection::Break()
{
    core::Check(OCI_Break(*this));
}

inline void Connection::SetAutoCommit(bool enabled)
{
    core::Check(OCI_SetAutoCommit(*this, enabled));
}

inline bool Connection::GetAutoCommit() const
{
    return (core::Check(OCI_GetAutoCommit(*this)) == TRUE);
}

inline bool Connection::IsServerAlive() const
{
    return (core::Check(OCI_IsConnected(*this)) == TRUE);
}

inline bool Connection::PingServer() const
{
    return( core::Check(OCI_Ping(*this)) == TRUE);
}

inline ostring Connection::GetConnectionString() const
{
    return core::MakeString(core::Check(OCI_GetDatabase(*this)));
}

inline ostring Connection::GetUserName() const
{
    return core::MakeString(core::Check(OCI_GetUserName(*this)));
}

inline ostring Connection::GetPassword() const
{
    return core::MakeString(core::Check(OCI_GetPassword(*this)));
}

inline OracleVersion Connection::GetVersion() const
{
    return OracleVersion(static_cast<OracleVersion::Type>(core::Check(OCI_GetVersionConnection(*this))));
}

inline ostring Connection::GetServerVersion() const
{
    return core::MakeString(core::Check(OCI_GetVersionServer(*this)));
}

inline unsigned int Connection::GetServerMajorVersion() const
{
    return core::Check(OCI_GetServerMajorVersion(*this));
}

inline unsigned int Connection::GetServerMinorVersion() const
{
    return core::Check(OCI_GetServerMinorVersion(*this));
}

inline unsigned int Connection::GetServerRevisionVersion() const
{
    return core::Check(OCI_GetServerRevisionVersion(*this));
}

inline void Connection::ChangePassword(const ostring& newPwd)
{
    core::Check(OCI_SetPassword(*this, newPwd.c_str()));
}

inline ostring Connection::GetSessionTag() const
{
    return core::MakeString(core::Check(OCI_GetSessionTag(*this)));
}

inline void Connection::SetSessionTag(const ostring& tag)
{
    core::Check(OCI_SetSessionTag(*this, tag.c_str()));
}

inline Transaction Connection::GetTransaction() const
{
    return Transaction(core::Check(OCI_GetTransaction(*this)));
}

inline void Connection::SetTransaction(const Transaction &transaction)
{
    core::Check(OCI_SetTransaction(*this, transaction));
}

inline bool Connection::SetFormat(FormatType formatType, const ostring& format)
{
    return core::Check(OCI_SetFormat(*this, formatType, format.c_str()) == TRUE);
}

inline ostring Connection::GetFormat(FormatType formatType)
{
    return core::MakeString(core::Check(OCI_GetFormat(*this, formatType)));
}

inline void Connection::EnableServerOutput(unsigned int bufsize, unsigned int arrsize, unsigned int lnsize)
{
    core::Check(OCI_ServerEnableOutput(*this, bufsize, arrsize, lnsize));
}

inline void Connection::DisableServerOutput()
{
    core::Check(OCI_ServerDisableOutput(*this));
}

inline bool Connection::GetServerOutput(ostring &line) const
{
    const otext * str = core::Check(OCI_ServerGetOutput(*this));

    line = core::MakeString(str);

    return (str != nullptr);
}

inline void Connection::GetServerOutput(std::vector<ostring> &lines) const
{
    const otext * str = core::Check(OCI_ServerGetOutput(*this));

    while (str)
    {
        lines.push_back(str);
        str = core::Check(OCI_ServerGetOutput(*this));
    }
}

inline void Connection::SetTrace(SessionTrace trace, const ostring& value)
{
    core::Check(OCI_SetTrace(*this, trace, value.c_str()));
}

inline ostring Connection::GetTrace(SessionTrace trace) const
{
    return core::MakeString(core::Check(OCI_GetTrace(*this, trace)));
}

inline ostring Connection::GetDatabase() const
{
    return core::MakeString(core::Check(OCI_GetDBName(*this)));
}

inline ostring Connection::GetInstance() const
{
    return core::Check(OCI_GetInstanceName(*this));
}

inline ostring Connection::GetService() const
{
    return core::MakeString(core::Check(OCI_GetServiceName(*this)));
}

inline ostring Connection::GetServer() const
{
    return core::Check(OCI_GetServerName(*this));
}

inline ostring Connection::GetDomain() const
{
    return core::MakeString(core::Check(OCI_GetDomainName(*this)));
}

inline Timestamp Connection::GetInstanceStartTime() const
{
    return Timestamp(core::Check(OCI_GetInstanceStartTime(*this)), GetHandle());
}

inline unsigned int Connection::GetStatementCacheSize() const
{
    return core::Check(OCI_GetStatementCacheSize(*this));
}

inline void Connection::SetStatementCacheSize(unsigned int value)
{
    core::Check(OCI_SetStatementCacheSize(*this, value));
}

inline unsigned int Connection::GetDefaultLobPrefetchSize() const
{
    return core::Check(OCI_GetDefaultLobPrefetchSize(*this));
}

inline void Connection::SetDefaultLobPrefetchSize(unsigned int value)
{
    core::Check(OCI_SetDefaultLobPrefetchSize(*this, value));
}

inline unsigned int  Connection::GetMaxCursors() const
{
    return core::Check(OCI_GetMaxCursors(*this));
}

inline bool Connection::IsTAFCapable() const
{
    return (core::Check(OCI_IsTAFCapable(*this)) == TRUE);
}

inline void Connection::SetTAFHandler(TAFHandlerProc handler)
{
    core::Check(OCI_SetTAFHandler(*this, static_cast<POCI_TAF_HANDLER>(handler != nullptr ? Environment::TAFHandler : nullptr)));

    Environment::SetUserCallback<Connection::TAFHandlerProc>(static_cast<OCI_Connection*>(*this), handler);
}

inline AnyPointer Connection::GetUserData()
{
    return core::Check(OCI_GetUserData(*this));
}

inline void Connection::SetUserData(AnyPointer value)
{
    core::Check(OCI_SetUserData(*this, value));
}

inline unsigned int Connection::GetTimeout(TimeoutType timeout)
{
    return core::Check(OCI_GetTimeout(*this, timeout));
}

inline void Connection::SetTimeout(TimeoutType timeout, unsigned int value)
{
    core::Check(OCI_SetTimeout(*this, timeout, value));
}

}
