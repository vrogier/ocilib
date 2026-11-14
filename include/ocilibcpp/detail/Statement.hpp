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

    inline Statement::Statement()
    {
    }

    inline Statement::Statement(const Connection& connection)
    {
        AcquireAllocatedWithNotification
        (
            core::Check(OCI_StatementCreate(connection)),
            connection.GetHandle(),
            OnFreeSmartHandle
        );
    }

    inline Statement::Statement(OCI_Statement* stmt, core::Handle* parent)
    {
        AcquireTransient(stmt, parent);
    }

    inline Connection Statement::GetConnection() const
    {
        core::Handle* selfHandle = this->GetHandle();
        core::Handle* parentHandle = selfHandle ? selfHandle->GetParent() : nullptr;
        core::Handle* connectionParent = parentHandle ? parentHandle->GetParent() : GetHandle();

        return Connection(core::Check(OCI_StatementGetConnection(*this)), connectionParent, false);
    }

    inline void Statement::Describe(const String& sql)
    {
        ClearBinds();
        ReleaseResultsets();
        core::Check(OCI_StatementDescribe(*this, sql.c_str()));
    }

    inline void Statement::Parse(const String& sql)
    {
        ClearBinds();
        ReleaseResultsets();
        core::Check(OCI_StatementParse(*this, sql.c_str()));
    }

    inline void Statement::Prepare(const String& sql)
    {
        ClearBinds();
        ReleaseResultsets();
        core::Check(OCI_StatementPrepare(*this, sql.c_str()));
    }

    inline void Statement::ExecutePrepared()
    {
        ReleaseResultsets();
        SetInData();
        core::Check(OCI_StatementExecutePrepared(*this));
        SetOutData();
    }

    template<class T>
    UInt32 Statement::ExecutePrepared(T callback)
    {
        ExecutePrepared();

        return Fetch(callback);
    }

    template<class T, class U>
    UInt32 Statement::ExecutePrepared(T callback, U adapter)
    {
        ExecutePrepared();

        return Fetch(callback, adapter);
    }

    inline void Statement::Execute(const String& sql)
    {
        ClearBinds();
        ReleaseResultsets();
        core::Check(OCI_StatementExecute(*this, sql.c_str()));
    }

    inline Resultset Statement::ExecuteAndGetResultset()
    {
        ExecutePrepared();

        return GetResultset();
    }

    inline Resultset Statement::ExecuteAndGetResultset(const String& sql)
    {
        Execute(sql);

        return GetResultset();
    }

    template<class T>
    UInt32 Statement::Execute(const String& sql, T callback)
    {
        Execute(sql);

        return Fetch(callback);
    }

    template<class T, class U>
    UInt32 Statement::Execute(const String& sql, T callback, U adapter)
    {
        Execute(sql);

        return Fetch(callback, adapter);
    }

    template<typename T>
    UInt32 Statement::Fetch(T callback)
    {
        UInt32 res = 0;

        Resultset rs = GetResultset();

        while (rs)
        {
            res += rs.ForEach(callback);
            rs = GetNextResultset();
        }

        return res;
    }

    template<class T, class U>
    UInt32 Statement::Fetch(T callback, U adapter)
    {
        UInt32 res = 0;

        Resultset rs = GetResultset();

        while (rs)
        {
            res += rs.ForEach(callback, adapter);
            rs = GetNextResultset();
        }

        return res;
    }

    inline UInt32 Statement::GetAffectedRows() const
    {
        return core::Check(OCI_StatementGetAffectedRows(*this));
    }

    inline String Statement::GetSql() const
    {
        return core::MakeString(core::Check(OCI_StatementGetSql(*this)));
    }

    inline String Statement::GetSqlIdentifier() const
    {
        return core::MakeString(core::Check(OCI_StatementGetSqlIdentifier(*this)));
    }

    inline Resultset Statement::GetResultset()
    {
        return Resultset(core::Check(OCI_StatementGetResultset(*this)), GetHandle());
    }

    inline Resultset Statement::GetNextResultset()
    {
        return Resultset(core::Check(OCI_StatementGetNextResultset(*this)), GetHandle());
    }

    inline void Statement::SetBindArraySize(UInt32 size)
    {
        core::Check(OCI_StatementBindArraySetSize(*this, size));
    }

    inline UInt32 Statement::GetBindArraySize() const
    {
        return core::Check(OCI_StatementBindArrayGetSize(*this));
    }

    inline void Statement::AllowRebinding(bool value)
    {
        core::Check(OCI_StatementAllowRebinding(*this, value));
    }

    inline bool Statement::IsRebindingAllowed() const
    {
        return (core::Check(OCI_StatementIsRebindingAllowed(*this)) == TRUE);
    }

    inline Int32 Statement::GetBindIndex(const String& name) const
    {
        return core::Check(OCI_StatementGetBindIndex(*this, name.c_str()));
    }

    inline UInt32 Statement::GetBindCount() const
    {
        return core::Check(OCI_StatementGetBindCount(*this));
    }

    inline BindInfo Statement::GetBind(UInt32 index) const
    {
        return BindInfo(core::Check(OCI_StatementGetBind(*this, index)), GetHandle());
    }

    inline BindInfo Statement::GetBind(const String& name) const
    {
        return BindInfo(core::Check(OCI_StatementGetBindByName(*this, name.c_str())), GetHandle());
    }

    template<typename M, class T>
    BindInfo Statement::Bind1(M& method, const String& name, T& value, BindInfo::BindDirection mode)
    {
        BindInfo info(core::Check(method(*this, name.c_str(), &value)), GetHandle());
        if (!info.IsNull())
        {
            info.SetDirection(mode);
        }
        return info;
    }

    template<typename M, class T>
    BindInfo Statement::Bind2(M& method, const String& name, T& value, BindInfo::BindDirection mode)
    {
        BindInfo info(core::Check(method(*this, name.c_str(), static_cast<typename support::BindResolver<T>::OutputType>(value))), GetHandle());
        if (!info.IsNull())
        {
            info.SetDirection(mode);
        }
        return info;
    }

    template<typename M, class T>
    BindInfo Statement::BindVector1(M& method, const String& name, std::vector<T>& values, BindInfo::BindDirection mode, BindInfo::VectorType type)
    {
        support::BindArray* bnd = core::OnAllocate(new support::BindArray(*this, name, mode));
        bnd->SetVector<T>(values, type == BindInfo::AsPlSqlTable, sizeof(typename support::BindResolver<T>::OutputType));

        OCI_Bind* res = method(*this, name.c_str(), bnd->GetData<T>(), bnd->GetSizeForBindCall());

        if (res)
        {
            support::BindsHolder* bindsHolder = GetBindsHolder(true);
            bindsHolder->AddBindObject(bnd);
            BindInfo(res, GetHandle()).SetDirection(mode);
        }
        else
        {
            delete core::OnDeallocate(bnd);
        }

        return BindInfo(core::Check(res), GetHandle());
    }

    template<typename M, class T, class U>
    BindInfo Statement::BindVector2(M& method, const String& name, std::vector<T>& values, BindInfo::BindDirection mode, U subType, BindInfo::VectorType type)
    {
        support::BindArray* bnd = core::OnAllocate(new support::BindArray(*this, name, mode));
        bnd->SetVector<T>(values, type == BindInfo::AsPlSqlTable, sizeof(typename support::BindResolver<T>::OutputType));

        OCI_Bind* res = method(*this, name.c_str(), bnd->GetData<T>(), subType, bnd->GetSizeForBindCall());

        if (res)
        {
            support::BindsHolder* bindsHolder = GetBindsHolder(true);
            bindsHolder->AddBindObject(bnd);
            BindInfo(res, GetHandle()).SetDirection(mode);
        }
        else
        {
            delete core::OnDeallocate(bnd);
        }

        return BindInfo(core::Check(res), GetHandle());
    }

    template<class T>
    inline BindInfo Statement::Bind(const String& name, T& value, BindInfo::BindDirection mode)
    {
        if constexpr (std::is_same_v<T, bool>)
        {
            support::BindTypeAdaptor<bool>* bnd = core::OnAllocate(new support::BindTypeAdaptor<bool>(*this, name, mode, value));

            OCI_Bind* res = OCI_StatementBindBoolean(*this, name.c_str(), static_cast<OCI_Boolean*>(*bnd));

            if (res)
            {
                support::BindsHolder* bindsHolder = GetBindsHolder(true);
                bindsHolder->AddBindObject(bnd);
                BindInfo(res, GetHandle()).SetDirection(mode);
            }
            else
            {
                delete core::OnDeallocate(bnd);
            }

            return BindInfo(core::Check(res), GetHandle());
        }
        else if constexpr (std::is_same_v<T, Int16>)
        {
            return Bind1(OCI_StatementBindInt16, name, value, mode);
        }
        else if constexpr (std::is_same_v<T, UInt16>)
        {
            return Bind1(OCI_StatementBindUInt16, name, value, mode);
        }
        else if constexpr (std::is_same_v<T, Int32>)
        {
            return Bind1(OCI_StatementBindInt32, name, value, mode);
        }
        else if constexpr (std::is_same_v<T, UInt32>)
        {
            return Bind1(OCI_StatementBindUInt32, name, value, mode);
        }
        else if constexpr (std::is_same_v<T, Int64>)
        {
            return Bind1(OCI_StatementBindInt64, name, value, mode);
        }
        else if constexpr (std::is_same_v<T, UInt64>)
        {
            return Bind1(OCI_StatementBindUInt64, name, value, mode);
        }
        else if constexpr (std::is_same_v<T, Float>)
        {
            return Bind1(OCI_StatementBindFloat, name, value, mode);
        }
        else if constexpr (std::is_same_v<T, Double>)
        {
            return Bind1(OCI_StatementBindDouble, name, value, mode);
        }
        else if constexpr (std::is_same_v<T, Number>)
        {
            return Bind2(OCI_StatementBindNumber, name, value, mode);
        }
        else if constexpr (std::is_same_v<T, Date>)
        {
            return Bind2(OCI_StatementBindDate, name, value, mode);
        }
        else if constexpr (std::is_same_v<T, Timestamp>)
        {
            return Bind2(OCI_StatementBindTimestamp, name, value, mode);
        }
        else if constexpr (std::is_same_v<T, Interval>)
        {
            return Bind2(OCI_StatementBindInterval, name, value, mode);
        }
        else if constexpr (std::is_same_v<T, Clob>)
        {
            return Bind2(OCI_StatementBindLob, name, value, mode);
        }
        else if constexpr (std::is_same_v<T, NClob>)
        {
            return Bind2(OCI_StatementBindLob, name, value, mode);
        }
        else if constexpr (std::is_same_v<T, Blob>)
        {
            return Bind2(OCI_StatementBindLob, name, value, mode);
        }
        else if constexpr (std::is_same_v<T, File>)
        {
            return Bind2(OCI_StatementBindFile, name, value, mode);
        }
        else if constexpr (std::is_same_v<T, Object>)
        {
            return Bind2(OCI_StatementBindObject, name, value, mode);
        }
        else if constexpr (std::is_same_v<T, Reference>)
        {
            return Bind2(OCI_StatementBindReference, name, value, mode);
        }
        else if constexpr (std::is_same_v<T, Vector>)
        {
            return Bind2(OCI_StatementBindVector, name, value, mode);
        }
        else if constexpr (std::is_same_v<T, Statement>)
        {
            return Bind2(OCI_StatementBindCursor, name, value, mode);
        }
        else if constexpr (std::is_same_v<T, Json>)
        {
            return Bind2(OCI_StatementBindJson, name, value, mode);
        }
        else
        {
            static_assert(!sizeof(T), "unsupported type");
        }
    }

    template<class T, class U>
    inline BindInfo Statement::Bind(const String& name, T& value, U maxSize, BindInfo::BindDirection mode)
    {
        if constexpr (std::is_same_v<T, Clong> && std::is_same_v<U, UInt32>)
        {
            BindInfo info(core::Check(OCI_StatementBindLong(*this, name.c_str(), value, maxSize)), GetHandle());
            if (!info.IsNull())
            {
                info.SetDirection(mode);
            }
            return info;
        }
        else if constexpr (std::is_same_v<T, Clong> && std::is_same_v<U, Int32>)
        {
            return Bind<Clong, UInt32>(name, value, static_cast<UInt32>(maxSize), mode);
        }
        else if constexpr (std::is_same_v<T, Blong> && std::is_same_v<U, UInt32>)
        {
            BindInfo info(core::Check(OCI_StatementBindLong(*this, name.c_str(), value, maxSize)), GetHandle());
            if (!info.IsNull())
            {
                info.SetDirection(mode);
            }
            return info;
        }
        else if constexpr (std::is_same_v<T, Blong> && std::is_same_v<U, Int32>)
        {
            return Bind<Blong, UInt32>(name, value, static_cast<UInt32>(maxSize), mode);
        }
        else if constexpr (std::is_same_v<T, String> && std::is_same_v<U, UInt32>)
        {
            if (maxSize == 0)
            {
                maxSize = static_cast<UInt32>(value.size());
            }

            value.reserve(maxSize);

            support::BindObjectAdaptor<String>* bnd = core::OnAllocate(new support::BindObjectAdaptor<String>(*this, name, mode, value, maxSize + 1));

            OCI_Bind* res = OCI_StatementBindString(*this, name.c_str(), static_cast<otext*>(*bnd), maxSize);

            if (res)
            {
                support::BindsHolder* bindsHolder = GetBindsHolder(true);
                bindsHolder->AddBindObject(bnd);
                BindInfo(res, GetHandle()).SetDirection(mode);
            }
            else
            {
                delete core::OnDeallocate(bnd);
            }

            return BindInfo(core::Check(res), GetHandle());
        }
        else if constexpr (std::is_same_v<T, String> && std::is_same_v<U, Int32>)
        {
            return Bind<String, UInt32>(name, value, static_cast<UInt32>(maxSize), mode);
        }
        else if constexpr (std::is_same_v<T, Raw> && std::is_same_v<U, UInt32>)
        {
            if (maxSize == 0)
            {
                maxSize = static_cast<UInt32>(value.size());
            }

            value.reserve(maxSize);

            support::BindObjectAdaptor<Raw>* bnd = core::OnAllocate(new support::BindObjectAdaptor<Raw>(*this, name, mode, value, maxSize));

            OCI_Bind* res = OCI_StatementBindRaw(*this, name.c_str(), static_cast<unsigned char*>(*bnd), maxSize);

            if (res)
            {
                support::BindsHolder* bindsHolder = GetBindsHolder(true);
                bindsHolder->AddBindObject(bnd);
                BindInfo(res, GetHandle()).SetDirection(mode);
            }
            else
            {
                delete core::OnDeallocate(bnd);
            }

            return BindInfo(core::Check(res), GetHandle());
        }
        else if constexpr (std::is_same_v<T, Raw> && std::is_same_v<U, Int32>)
        {
            return Bind<Raw, UInt32>(name, value, static_cast<UInt32>(maxSize), mode);
        }
        else
        {
            static_assert(!sizeof(T), "unsupported type");
        }
    }

    template<class T>
    inline BindInfo Statement::Bind(const String& name, std::vector<T>& values, BindInfo::BindDirection mode, BindInfo::VectorType type)
    {
        if constexpr (std::is_same_v<T, Int16>)
        {
            return BindVector1(OCI_StatementBindInt16Array, name, values, mode, type);
        }
        else if constexpr (std::is_same_v<T, UInt16>)
        {
            return BindVector1(OCI_StatementBindUInt16Array, name, values, mode, type);
        }
        else if constexpr (std::is_same_v<T, Int32>)
        {
            return BindVector1(OCI_StatementBindInt32Array, name, values, mode, type);
        }
        else if constexpr (std::is_same_v<T, UInt32>)
        {
            return BindVector1(OCI_StatementBindUInt32Array, name, values, mode, type);
        }
        else if constexpr (std::is_same_v<T, Int64>)
        {
            return BindVector1(OCI_StatementBindInt64Array, name, values, mode, type);
        }
        else if constexpr (std::is_same_v<T, UInt64>)
        {
            return BindVector1(OCI_StatementBindUInt64Array, name, values, mode, type);
        }
        else if constexpr (std::is_same_v<T, Float>)
        {
            return BindVector1(OCI_StatementBindFloatArray, name, values, mode, type);
        }
        else if constexpr (std::is_same_v<T, Double>)
        {
            return BindVector1(OCI_StatementBindDoubleArray, name, values, mode, type);
        }
        else if constexpr (std::is_same_v<T, Date>)
        {
            return BindVector1(OCI_StatementBindDateArray, name, values, mode, type);
        }
        else if constexpr (std::is_same_v<T, Number>)
        {
            return BindVector1(OCI_StatementBindNumberArray, name, values, mode, type);
        }
        else if constexpr (std::is_same_v<T, Json>)
        {
            return BindVector1(OCI_StatementBindJsonArray, name, values, mode, type);
        }
        else if constexpr (std::is_same_v<T, Clob>)
        {
            return BindVector2(OCI_StatementBindLobArray, name, values, mode, static_cast<UInt32>(OCI_CLOB), type);
        }
        else if constexpr (std::is_same_v<T, NClob>)
        {
            return BindVector2(OCI_StatementBindLobArray, name, values, mode, static_cast<UInt32>(OCI_NCLOB), type);
        }
        else if constexpr (std::is_same_v<T, Blob>)
        {
            return BindVector2(OCI_StatementBindLobArray, name, values, mode, static_cast<UInt32>(OCI_BLOB), type);
        }
        else if constexpr (std::is_same_v<T, File>)
        {
            return BindVector2(OCI_StatementBindFileArray, name, values, mode, static_cast<UInt32>(OCI_BFILE), type);
        }
        else
        {
            static_assert(!sizeof(T), "unsupported type");
        }
    }

    template<class T>
    inline BindInfo Statement::Bind(const String& name, std::vector<T>& values, TypeInfo& typeInfo, BindInfo::BindDirection mode, BindInfo::VectorType type)
    {
        if constexpr (std::is_same_v<T, Object>)
        {
            return BindVector2(OCI_StatementBindObjectArray, name, values, mode, static_cast<OCI_TypeInfo*>(typeInfo), type);
        }
        else if constexpr (std::is_same_v<T, Reference>)
        {
            return BindVector2(OCI_StatementBindReferenceArray, name, values, mode, static_cast<OCI_TypeInfo*>(typeInfo), type);
        }
        else
        {
            return BindVector2(OCI_StatementBindCollectionArray, name, values, mode, static_cast<OCI_TypeInfo*>(typeInfo), GetArraysize(type, values));
        }
    }

    template<class T, class U>
    inline BindInfo Statement::Bind(const String& name, std::vector<T>& values, U extraInfo, BindInfo::BindDirection mode, BindInfo::VectorType type)
    {
        if constexpr (std::is_same_v<T, Timestamp> && std::is_same_v<U, Timestamp::TimestampTypeValues>)
        {
            return BindVector2(OCI_StatementBindTimestampArray, name, values, mode, extraInfo, type);
        }
        else if constexpr (std::is_same_v<T, Timestamp> && std::is_same_v<U, Timestamp::TimestampType>)
        {
            return Bind<Timestamp, Timestamp::TimestampTypeValues>(name, values, extraInfo.GetValue(), mode, type);
        }
        else if constexpr (std::is_same_v<T, Interval> && std::is_same_v<U, Interval::IntervalTypeValues>)
        {
            return BindVector2(OCI_StatementBindIntervalArray, name, values, mode, extraInfo, type);
        }
        else if constexpr (std::is_same_v<T, Interval> && std::is_same_v<U, Interval::IntervalType>)
        {
            return Bind<Interval, Interval::IntervalTypeValues>(name, values, extraInfo.GetValue(), mode, type);
        }
        else if constexpr (std::is_same_v<T, String> && std::is_same_v<U, UInt32>)
        {
            support::BindArray* bnd = core::OnAllocate(new support::BindArray(*this, name, mode));
            bnd->SetVector<String>(values, type == BindInfo::AsPlSqlTable, extraInfo + 1);

            OCI_Bind* res = OCI_StatementBindStringArray(*this, name.c_str(), bnd->GetData<String>(), extraInfo, bnd->GetSizeForBindCall());

            if (res)
            {
                support::BindsHolder* bindsHolder = GetBindsHolder(true);
                bindsHolder->AddBindObject(bnd);
                BindInfo(res, GetHandle()).SetDirection(mode);
            }
            else
            {
                delete core::OnDeallocate(bnd);
            }

            return BindInfo(core::Check(res), GetHandle());
        }
        else if constexpr (std::is_same_v<T, String> && std::is_same_v<U, Int32>)
        {
            return Bind<String, UInt32>(name, values, static_cast<UInt32>(extraInfo), mode, type);
        }
        else if constexpr (std::is_same_v<T, Raw> && std::is_same_v<U, UInt32>)
        {
            support::BindArray* bnd = core::OnAllocate(new support::BindArray(*this, name, mode));
            bnd->SetVector<Raw>(values, type == BindInfo::AsPlSqlTable, extraInfo);

            OCI_Bind* res = OCI_StatementBindRawArray(*this, name.c_str(), bnd->GetData<Raw>(), extraInfo, bnd->GetSizeForBindCall());

            if (res)
            {
                support::BindsHolder* bindsHolder = GetBindsHolder(true);
                bindsHolder->AddBindObject(bnd);
                BindInfo(res, GetHandle()).SetDirection(mode);
            }
            else
            {
                delete core::OnDeallocate(bnd);
            }

            return BindInfo(core::Check(res), GetHandle());
        }
        else
        {
            static_assert(!sizeof(T), "unsupported type");
        }
    }

    template<class T>
    inline void Statement::Register(const String& name)
    {
        if constexpr (std::is_same_v<T, UInt16>)
        {
            core::Check(OCI_StatementRegisterUInt16(*this, name.c_str()));
        }
        else if constexpr (std::is_same_v<T, Int16>)
        {
            core::Check(OCI_StatementRegisterInt16(*this, name.c_str()));
        }
        else if constexpr (std::is_same_v<T, UInt32>)
        {
            core::Check(OCI_StatementRegisterUInt32(*this, name.c_str()));
        }
        else if constexpr (std::is_same_v<T, Int32>)
        {
            core::Check(OCI_StatementRegisterInt32(*this, name.c_str()));
        }
        else if constexpr (std::is_same_v<T, UInt64>)
        {
            core::Check(OCI_StatementRegisterUInt64(*this, name.c_str()));
        }
        else if constexpr (std::is_same_v<T, Int64>)
        {
            core::Check(OCI_StatementRegisterInt64(*this, name.c_str()));
        }
        else if constexpr (std::is_same_v<T, Float>)
        {
            core::Check(OCI_StatementRegisterFloat(*this, name.c_str()));
        }
        else if constexpr (std::is_same_v<T, Double>)
        {
            core::Check(OCI_StatementRegisterDouble(*this, name.c_str()));
        }
        else if constexpr (std::is_same_v<T, Number>)
        {
            core::Check(OCI_StatementRegisterNumber(*this, name.c_str()));
        }
        else if constexpr (std::is_same_v<T, Date>)
        {
            core::Check(OCI_StatementRegisterDate(*this, name.c_str()));
        }
        else if constexpr (std::is_same_v<T, Clob>)
        {
            core::Check(OCI_StatementRegisterLob(*this, name.c_str(), OCI_CLOB));
        }
        else if constexpr (std::is_same_v<T, NClob>)
        {
            core::Check(OCI_StatementRegisterLob(*this, name.c_str(), OCI_NCLOB));
        }
        else if constexpr (std::is_same_v<T, Blob>)
        {
            core::Check(OCI_StatementRegisterLob(*this, name.c_str(), OCI_BLOB));
        }
        else if constexpr (std::is_same_v<T, File>)
        {
            core::Check(OCI_StatementRegisterFile(*this, name.c_str(), OCI_BFILE));
        }
        else if constexpr (std::is_same_v<T, Vector>)
        {
            core::Check(OCI_StatementRegisterVector(*this, name.c_str()));
        }
        else if constexpr (std::is_same_v<T, Json>)
        {
            core::Check(OCI_StatementRegisterJson(*this, name.c_str()));
        }
        else
        {
            static_assert(!sizeof(T), "unsupported type");
        }
    }

    template<class T, class U>
    inline void Statement::Register(const String& name, U& extraInfo)
    {
        if constexpr (std::is_same_v<T, Object> && std::is_same_v<U, TypeInfo>)
        {
            core::Check(OCI_StatementRegisterObject(*this, name.c_str(), extraInfo));
        }
        else if constexpr (std::is_same_v<T, Reference> && std::is_same_v<U, TypeInfo>)
        {
            core::Check(OCI_StatementRegisterReference(*this, name.c_str(), extraInfo));
        }
        else
        {
            static_assert(!sizeof(T), "unsupported type");
        }
    }

    template<class T, class U>
    inline void Statement::Register(const String& name, U extraInfo)
    {
        if constexpr (std::is_same_v<T, Timestamp> && std::is_same_v<U, Timestamp::TimestampTypeValues>)
        {
            core::Check(OCI_StatementRegisterTimestamp(*this, name.c_str(), extraInfo));
        }
        else if constexpr (std::is_same_v<T, Timestamp> && std::is_same_v<U, Timestamp::TimestampType>)
        {
            Register<Timestamp, Timestamp::TimestampTypeValues>(name, extraInfo.GetValue());
        }
        else if constexpr (std::is_same_v<T, Interval> && std::is_same_v<U, Interval::IntervalTypeValues>)
        {
            core::Check(OCI_StatementRegisterInterval(*this, name.c_str(), extraInfo));
        }
        else if constexpr (std::is_same_v<T, Interval> && std::is_same_v<U, Interval::IntervalType>)
        {
            Register<Interval, Interval::IntervalTypeValues>(name, extraInfo.GetValue());
        }
        else if constexpr (std::is_same_v<T, String> && std::is_same_v<U, UInt32>)
        {
            core::Check(OCI_StatementRegisterString(*this, name.c_str(), extraInfo));
        }
        else if constexpr (std::is_same_v<T, String> && std::is_same_v<U, Int32>)
        {
            Register<String, UInt32>(name, static_cast<UInt32>(extraInfo));
        }
        else if constexpr (std::is_same_v<T, Raw> && std::is_same_v<U, UInt32>)
        {
            core::Check(OCI_StatementRegisterRaw(*this, name.c_str(), extraInfo));
        }
        else if constexpr (std::is_same_v<T, Raw> && std::is_same_v<U, Int32>)
        {
            Register<Raw, UInt32>(name, static_cast<UInt32>(extraInfo));
        }
        else
        {
            static_assert(!sizeof(T), "unsupported type");
        }
    }

    template<class T>
    BindInfo Statement::Bind(const String& name, Collection<T>& value, BindInfo::BindDirection mode)
    {
        BindInfo info(core::Check(OCI_StatementBindCollection(*this, name.c_str(), value)), GetHandle());
        if (!info.IsNull())
        {
            info.SetDirection(mode);
        }
        return info;
    }

    template<class T>
    BindInfo Statement::Bind(const String& name, std::vector<Collection<T> >& values, TypeInfo& typeInfo, BindInfo::BindDirection mode,
                             BindInfo::VectorType type)
    {
        return BindVector2(OCI_StatementBindCollectionArray, name, values, mode, static_cast<OCI_TypeInfo*>(typeInfo), type);
    }

    inline Statement::StatementType Statement::GetStatementType() const
    {
        return StatementType(static_cast<StatementType::Type>(core::Check(OCI_StatementGetType(*this))));
    }

    inline UInt32 Statement::GetSqlErrorPos() const
    {
        return core::Check(OCI_StatementGetSqlErrorPos(*this));
    }

    inline void Statement::SetFetchMode(FetchMode value)
    {
        core::Check(OCI_StatementSetFetchMode(*this, value));
    }

    inline Statement::FetchMode Statement::GetFetchMode() const
    {
        return FetchMode(static_cast<FetchMode::Type>(core::Check(OCI_StatementGetFetchMode(*this))));
    }

    inline void Statement::SetBindMode(BindMode value)
    {
        core::Check(OCI_StatementSetBindMode(*this, value));
    }

    inline Statement::BindMode Statement::GetBindMode() const
    {
        return BindMode(static_cast<BindMode::Type>(core::Check(OCI_StatementGetBindMode(*this))));
    }

    inline void Statement::SetFetchSize(UInt32 value)
    {
        core::Check(OCI_StatementSetFetchSize(*this, value));
    }

    inline UInt32 Statement::GetFetchSize() const
    {
        return core::Check(OCI_StatementGetFetchSize(*this));
    }

    inline void Statement::SetPrefetchSize(UInt32 value)
    {
        core::Check(OCI_StatementSetPrefetchSize(*this, value));
    }

    inline UInt32 Statement::GetPrefetchSize() const
    {
        return core::Check(OCI_StatementGetPrefetchSize(*this));
    }

    inline void Statement::SetPrefetchMemory(UInt32 value)
    {
        core::Check(OCI_StatementSetPrefetchMemory(*this, value));
    }

    inline UInt32 Statement::GetPrefetchMemory() const
    {
        return core::Check(OCI_StatementGetPrefetchMemory(*this));
    }

    inline void Statement::SetPieceSize(UInt32 value)
    {
        core::Check(OCI_StatementSetPieceSize(*this, value));
    }

    inline UInt32 Statement::GetPieceSize() const
    {
        return core::Check(OCI_StatementGetPieceSize(*this));
    }

    inline void Statement::SetLongMode(LongMode value)
    {
        core::Check(OCI_StatementSetLongMode(*this, value));
    }

    inline Statement::LongMode Statement::GetLongMode() const
    {
        return LongMode(static_cast<LongMode::Type>(core::Check(OCI_StatementGetLongMode(*this))));
    }

    inline UInt32 Statement::GetSQLCommand() const
    {
        return core::Check(OCI_StatementGetSqlCommand(*this));
    }

    inline String Statement::GetSQLVerb() const
    {
        return core::MakeString(core::Check(OCI_StatementGetSqlVerb(*this)));
    }

    inline std::vector<String> Statement::GetParseBindNames() const
    {
        UInt32        count = 0;
        const otext** names = nullptr;

        core::Check(OCI_StatementGetParseBindNames(*this, &count, &names));

        std::vector<String> result;

        result.reserve(count);

        for (UInt32 i = 0; i < count; ++i)
        {
            result.push_back(core::MakeString(names[i]));
        }

        return result;
    }

    inline void Statement::GetBatchErrors(std::vector<Exception>& exceptions)
    {
        exceptions.clear();

        OCI_Error* err = core::Check(OCI_StatementGetBatchError(*this));

        while (err)
        {
            exceptions.push_back(Exception(err));

            err = core::Check(OCI_StatementGetBatchError(*this));
        }
    }

    inline void Statement::ClearBinds() const
    {
        support::BindsHolder* bindsHolder = GetBindsHolder(false);

        if (bindsHolder)
        {
            bindsHolder->Clear();
        }
    }

    inline void Statement::SetOutData() const
    {
        support::BindsHolder* bindsHolder = GetBindsHolder(false);

        if (bindsHolder)
        {
            bindsHolder->SetOutData();
        }
    }

    inline void Statement::SetInData() const
    {
        support::BindsHolder* bindsHolder = GetBindsHolder(false);

        if (bindsHolder)
        {
            bindsHolder->SetInData();
        }
    }

    inline void Statement::ReleaseResultsets() const
    {
        if (auto ctrl = GetHandleControl())
        {
            ctrl->FreeChildren([](core::HandleControlBase* c)
                               {
                                   return dynamic_cast<core::HandleControl<OCI_Resultset*>*>(c) != nullptr;
                               });
        }
    }

    inline void Statement::OnFreeSmartHandle(core::Handle* smartHandle)
    {
        if (smartHandle)
        {
            support::BindsHolder* bindsHolder = static_cast<support::BindsHolder*>(smartHandle->GetExtraInfos());

            smartHandle->SetExtraInfos(nullptr);

            delete core::OnDeallocate(bindsHolder);
        }
    }

    inline support::BindsHolder* Statement::GetBindsHolder(bool create) const
    {
        support::BindsHolder* bindsHolder = static_cast<support::BindsHolder*>(GetHandleControl()->GetExtraInfos());

        if (bindsHolder == nullptr && create)
        {
            bindsHolder = core::OnAllocate(new support::BindsHolder(*this));
            GetHandleControl()->SetExtraInfos(bindsHolder);
        }

        return bindsHolder;
    }

}
