/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2026 Vincent ROGIER <vince.rogier@ocilib.net>
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

 // ReSharper disable CppUseAuto
 // ReSharper disable CppParameterMayBeConst
 // ReSharper disable CppClangTidyCppcoreguidelinesMacroUsage
 // ReSharper disable CppClangTidyModernizeReturnBracedInitList
 // ReSharper disable CppClangTidyModernizePassByValue
 // ReSharper disable CppClangTidyHicppUseAuto
 // ReSharper disable CppClangTidyModernizeUseAuto
 // ReSharper disable CppClangTidyReadabilityInconsistentDeclarationParameterName
 // ReSharper disable CppClangTidyPerformanceUnnecessaryValueParam
 // ReSharper disable CppClangTidyHicppUseEqualsDefault
 // ReSharper disable CppClangTidyModernizeLoopConvert
 // ReSharper disable CppClangTidyModernizeUseEmplace
 // ReSharper disable CppClangTidyModernizeUseEqualsDefault
 // ReSharper disable CppClangTidyHicppUseEmplace
 // ReSharper disable CppClangTidyCertOop54Cpp
 // ReSharper disable CppClangTidyMiscMisplacedConst
 // ReSharper disable CppClangTidyBugproneUnhandledSelfAssignment

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

    inline void Statement::Describe(const ostring& sql)
    {
        ClearBinds();
        ReleaseResultsets();
        core::Check(OCI_Describe(*this, sql.c_str()));
    }

    inline void Statement::Parse(const ostring& sql)
    {
        ClearBinds();
        ReleaseResultsets();
        core::Check(OCI_Parse(*this, sql.c_str()));
    }

    inline void Statement::Prepare(const ostring& sql)
    {
        ClearBinds();
        ReleaseResultsets();
        core::Check(OCI_Prepare(*this, sql.c_str()));
    }

    inline void Statement::ExecutePrepared()
    {
        ReleaseResultsets();
        SetInData();
        core::Check(OCI_Execute(*this));
        SetOutData();
    }

    template<class T>
    OCI_UInt32 Statement::ExecutePrepared(T callback)
    {
        ExecutePrepared();

        return Fetch(callback);
    }

    template<class T, class U>
    OCI_UInt32 Statement::ExecutePrepared(T callback, U adapter)
    {
        ExecutePrepared();

        return Fetch(callback, adapter);
    }

    inline void Statement::Execute(const ostring& sql)
    {
        ClearBinds();
        ReleaseResultsets();
        core::Check(OCI_ExecuteStmt(*this, sql.c_str()));
    }

    inline Resultset Statement::ExecuteAndGetResultset()
    {
        ExecutePrepared();

        return GetResultset();
    }

    inline Resultset Statement::ExecuteAndGetResultset(const ostring& sql)
    {
        Execute(sql);

        return GetResultset();
    }

    template<class T>
    OCI_UInt32 Statement::Execute(const ostring& sql, T callback)
    {
        Execute(sql);

        return Fetch(callback);
    }

    template<class T, class U>
    OCI_UInt32 Statement::Execute(const ostring& sql, T callback, U adapter)
    {
        Execute(sql);

        return Fetch(callback, adapter);
    }

    template<typename T>
    OCI_UInt32  Statement::Fetch(T callback)
    {
        OCI_UInt32 res = 0;

        Resultset rs = GetResultset();

        while (rs)
        {
            res += rs.ForEach(callback);
            rs = GetNextResultset();
        }

        return res;
    }

    template<class T, class U>
    OCI_UInt32 Statement::Fetch(T callback, U adapter)
    {
        OCI_UInt32 res = 0;

        Resultset rs = GetResultset();

        while (rs)
        {
            res += rs.ForEach(callback, adapter);
            rs = GetNextResultset();
        }

        return res;
    }

    inline OCI_UInt32 Statement::GetAffectedRows() const
    {
        return core::Check(OCI_GetAffectedRows(*this));
    }

    inline ostring Statement::GetSql() const
    {
        return core::MakeString(core::Check(OCI_GetSql(*this)));
    }

    inline ostring Statement::GetSqlIdentifier() const
    {
        return core::MakeString(core::Check(OCI_GetSqlIdentifier(*this)));
    }

    inline Resultset Statement::GetResultset()
    {
        return Resultset(core::Check(OCI_GetResultset(*this)), GetHandle());
    }

    inline Resultset Statement::GetNextResultset()
    {
        return Resultset(core::Check(OCI_GetNextResultset(*this)), GetHandle());
    }

    inline void Statement::SetBindArraySize(OCI_UInt32 size)
    {
        core::Check(OCI_BindArraySetSize(*this, size));
    }

    inline OCI_UInt32 Statement::GetBindArraySize() const
    {
        return core::Check(OCI_BindArrayGetSize(*this));
    }

    inline void Statement::AllowRebinding(bool value)
    {
        core::Check(OCI_AllowRebinding(*this, value));
    }

    inline bool Statement::IsRebindingAllowed() const
    {
        return (core::Check(OCI_IsRebindingAllowed(*this)) == TRUE);
    }

    inline OCI_UInt32 Statement::GetBindIndex(const ostring& name) const
    {
        return core::Check(OCI_GetBindIndex(*this, name.c_str()));
    }

    inline OCI_UInt32 Statement::GetBindCount() const
    {
        return core::Check(OCI_GetBindCount(*this));
    }

    inline BindInfo Statement::GetBind(OCI_UInt32 index) const
    {
        return BindInfo(core::Check(OCI_GetBind(*this, index)), GetHandle());
    }

    inline BindInfo Statement::GetBind(const ostring& name) const
    {
        return BindInfo(core::Check(OCI_GetBindByName(*this, name.c_str())), GetHandle());
    }

    template<typename M, class T>
    BindInfo Statement::Bind1(M& method, const ostring& name, T& value, BindInfo::BindDirection mode)
    {
        BindInfo info(core::Check(method(*this, name.c_str(), &value)), GetHandle());
        if (!info.IsNull())
        {
            info.SetDirection(mode);
        }
        return info;
    }

    template<typename M, class T>
    BindInfo Statement::Bind2(M& method, const ostring& name, T& value, BindInfo::BindDirection mode)
    {
        BindInfo info(core::Check(method(*this, name.c_str(), static_cast<typename support::BindResolver<T>::OutputType>(value))), GetHandle());
        if (!info.IsNull())
        {
            info.SetDirection(mode);
        }
        return info;
    }

    template<typename M, class T>
    BindInfo Statement::BindVector1(M& method, const ostring& name, std::vector<T>& values, BindInfo::BindDirection mode, BindInfo::VectorType type)
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
    BindInfo Statement::BindVector2(M& method, const ostring& name, std::vector<T>& values, BindInfo::BindDirection mode, U subType, BindInfo::VectorType type)
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

    template<>
    inline BindInfo Statement::Bind<bool>(const ostring& name, bool& value, BindInfo::BindDirection mode)
    {
        support::BindTypeAdaptor<bool>* bnd = core::OnAllocate(new support::BindTypeAdaptor<bool>(*this, name, mode, value));

        OCI_Bind* res = OCI_BindBoolean(*this, name.c_str(), static_cast<OCI_Boolean*>(*bnd));

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

    template<>
    inline BindInfo Statement::Bind<OCI_Int16>(const ostring& name, OCI_Int16& value, BindInfo::BindDirection mode)
    {
        return Bind1(OCI_BindShort, name, value, mode);
    }

    template<>
    inline BindInfo Statement::Bind<OCI_UInt16>(const ostring& name, OCI_UInt16& value, BindInfo::BindDirection mode)
    {
        return Bind1(OCI_BindUnsignedShort, name, value, mode);
    }

    template<>
    inline BindInfo Statement::Bind<OCI_Int32>(const ostring& name, OCI_Int32& value, BindInfo::BindDirection mode)
    {
        return Bind1(OCI_BindInt, name, value, mode);
    }

    template<>
    inline BindInfo Statement::Bind<OCI_UInt32>(const ostring& name, OCI_UInt32& value, BindInfo::BindDirection mode)
    {
        return Bind1(OCI_BindUnsignedInt, name, value, mode);
    }

    template<>
    inline BindInfo Statement::Bind<OCI_Int64>(const ostring& name, OCI_Int64& value, BindInfo::BindDirection mode)
    {
        return Bind1(OCI_BindBigInt, name, value, mode);
    }

    template<>
    inline BindInfo Statement::Bind<OCI_UInt64>(const ostring& name, OCI_UInt64& value, BindInfo::BindDirection mode)
    {
        return Bind1(OCI_BindUnsignedBigInt, name, value, mode);
    }

    template<>
    inline BindInfo Statement::Bind<float>(const ostring& name, float& value, BindInfo::BindDirection mode)
    {
        return Bind1(OCI_BindFloat, name, value, mode);
    }

    template<>
    inline BindInfo Statement::Bind<double>(const ostring& name, double& value, BindInfo::BindDirection mode)
    {
        return Bind1(OCI_BindDouble, name, value, mode);
    }

    template<>
    inline BindInfo Statement::Bind<Number>(const ostring& name, Number& value, BindInfo::BindDirection mode)
    {
        return Bind2(OCI_BindNumber, name, value, mode);
    }

    template<>
    inline BindInfo Statement::Bind<Date>(const ostring& name, Date& value, BindInfo::BindDirection mode)
    {
        return Bind2(OCI_BindDate, name, value, mode);
    }

    template<>
    inline BindInfo Statement::Bind<Timestamp>(const ostring& name, Timestamp& value, BindInfo::BindDirection mode)
    {
        return Bind2(OCI_BindTimestamp, name, value, mode);
    }

    template<>
    inline BindInfo Statement::Bind<Interval>(const ostring& name, Interval& value, BindInfo::BindDirection mode)
    {
        return Bind2(OCI_BindInterval, name, value, mode);
    }

    template<>
    inline BindInfo Statement::Bind<Clob>(const ostring& name, Clob& value, BindInfo::BindDirection mode)
    {
        return Bind2(OCI_BindLob, name, value, mode);
    }

    template<>
    inline BindInfo Statement::Bind<NClob>(const ostring& name, NClob& value, BindInfo::BindDirection mode)
    {
        return Bind2(OCI_BindLob, name, value, mode);
    }

    template<>
    inline BindInfo Statement::Bind<Blob>(const ostring& name, Blob& value, BindInfo::BindDirection mode)
    {
        return Bind2(OCI_BindLob, name, value, mode);
    }

    template<>
    inline BindInfo Statement::Bind<File>(const ostring& name, File& value, BindInfo::BindDirection mode)
    {
        return Bind2(OCI_BindFile, name, value, mode);
    }

    template<>
    inline BindInfo Statement::Bind<Object>(const ostring& name, Object& value, BindInfo::BindDirection mode)
    {
        return Bind2(OCI_BindObject, name, value, mode);
    }

    template<>
    inline BindInfo Statement::Bind<Reference>(const ostring& name, Reference& value, BindInfo::BindDirection mode)
    {
        return Bind2(OCI_BindRef, name, value, mode);
    }

    template<>
    inline BindInfo Statement::Bind<Vector>(const ostring& name, Vector& value, BindInfo::BindDirection mode)
    {
        return Bind2(OCI_BindVector, name, value, mode);
    }

    template<>
    inline BindInfo Statement::Bind<Statement>(const ostring& name, Statement& value, BindInfo::BindDirection mode)
    {
        return Bind2(OCI_BindStatement, name, value, mode);
    }

    template<>
    inline BindInfo Statement::Bind<Json>(const ostring& name, Json& value, BindInfo::BindDirection mode)
    {
        return Bind2(OCI_BindJson, name, value, mode);
    }

    template<>
    inline BindInfo Statement::Bind<Clong, OCI_UInt32>(const ostring& name, Clong& value, OCI_UInt32 maxSize, BindInfo::BindDirection mode)
    {
        BindInfo info(core::Check(OCI_BindLong(*this, name.c_str(), value, maxSize)), GetHandle());
        if (!info.IsNull())
        {
            info.SetDirection(mode);
        }
        return info;
    }

    template<>
    inline BindInfo Statement::Bind<Clong, OCI_Int32>(const ostring& name, Clong& value, OCI_Int32 maxSize, BindInfo::BindDirection mode)
    {
        return Bind<Clong, OCI_UInt32>(name, value, static_cast<OCI_UInt32>(maxSize), mode);
    }

    template<>
    inline BindInfo Statement::Bind<Blong, OCI_UInt32>(const ostring& name, Blong& value, OCI_UInt32 maxSize, BindInfo::BindDirection mode)
    {
        BindInfo info(core::Check(OCI_BindLong(*this, name.c_str(), value, maxSize)), GetHandle());
        if (!info.IsNull())
        {
            info.SetDirection(mode);
        }
        return info;
    }

    template<>
    inline BindInfo Statement::Bind<Blong, OCI_Int32>(const ostring& name, Blong& value, OCI_Int32 maxSize, BindInfo::BindDirection mode)
    {
        return Bind<Blong, OCI_UInt32>(name, value, static_cast<OCI_UInt32>(maxSize), mode);
    }

    template<>
    inline BindInfo Statement::Bind<ostring, OCI_UInt32>(const ostring& name, ostring& value, OCI_UInt32 maxSize, BindInfo::BindDirection mode)
    {
        if (maxSize == 0)
        {
            maxSize = static_cast<OCI_UInt32>(value.size());
        }

        value.reserve(maxSize);

        support::BindObjectAdaptor<ostring>* bnd = core::OnAllocate(new support::BindObjectAdaptor<ostring>(*this, name, mode, value, maxSize + 1));

        OCI_Bind* res = OCI_BindString(*this, name.c_str(), static_cast<otext*>(*bnd), maxSize);

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

    template<>
    inline BindInfo Statement::Bind<ostring, OCI_Int32>(const ostring& name, ostring& value, OCI_Int32 maxSize, BindInfo::BindDirection mode)
    {
        return Bind<ostring, OCI_UInt32>(name, value, static_cast<OCI_UInt32>(maxSize), mode);
    }

    template<>
    inline BindInfo Statement::Bind<Raw, OCI_UInt32>(const ostring& name, Raw& value, OCI_UInt32 maxSize, BindInfo::BindDirection mode)
    {
        if (maxSize == 0)
        {
            maxSize = static_cast<OCI_UInt32>(value.size());
        }

        value.reserve(maxSize);

        support::BindObjectAdaptor<Raw>* bnd = core::OnAllocate(new support::BindObjectAdaptor<Raw>(*this, name, mode, value, maxSize));

        OCI_Bind* res = OCI_BindRaw(*this, name.c_str(), static_cast<unsigned char*>(*bnd), maxSize);

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

    template<>
    inline BindInfo Statement::Bind<Raw, OCI_Int32>(const ostring& name, Raw& value, OCI_Int32 maxSize, BindInfo::BindDirection mode)
    {
        return Bind<Raw, OCI_UInt32>(name, value, static_cast<OCI_UInt32>(maxSize), mode);
    }

    template<>
    inline BindInfo Statement::Bind<OCI_Int16>(const ostring& name, std::vector<OCI_Int16>& values, BindInfo::BindDirection mode, BindInfo::VectorType type)
    {
        return BindVector1(OCI_BindArrayOfShorts, name, values, mode, type);
    }

    template<>
    inline BindInfo Statement::Bind<OCI_UInt16>(const ostring& name, std::vector<OCI_UInt16>& values, BindInfo::BindDirection mode, BindInfo::VectorType type)
    {
        return BindVector1(OCI_BindArrayOfUnsignedShorts, name, values, mode, type);
    }

    template<>
    inline BindInfo Statement::Bind<OCI_Int32>(const ostring& name, std::vector<OCI_Int32>& values, BindInfo::BindDirection mode, BindInfo::VectorType type)
    {
        return BindVector1(OCI_BindArrayOfInts, name, values, mode, type);
    }

    template<>
    inline BindInfo Statement::Bind<OCI_UInt32>(const ostring& name, std::vector<OCI_UInt32>& values, BindInfo::BindDirection mode, BindInfo::VectorType type)
    {
        return BindVector1(OCI_BindArrayOfUnsignedInts, name, values, mode, type);
    }

    template<>
    inline BindInfo Statement::Bind<OCI_Int64>(const ostring& name, std::vector<OCI_Int64>& values, BindInfo::BindDirection mode, BindInfo::VectorType type)
    {
        return BindVector1(OCI_BindArrayOfBigInts, name, values, mode, type);
    }

    template<>
    inline BindInfo Statement::Bind<OCI_UInt64>(const ostring& name, std::vector<OCI_UInt64>& values, BindInfo::BindDirection mode, BindInfo::VectorType type)
    {
        return BindVector1(OCI_BindArrayOfUnsignedBigInts, name, values, mode, type);
    }

    template<>
    inline BindInfo Statement::Bind<float>(const ostring& name, std::vector<float>& values, BindInfo::BindDirection mode, BindInfo::VectorType type)
    {
        return BindVector1(OCI_BindArrayOfFloats, name, values, mode, type);
    }

    template<>
    inline BindInfo Statement::Bind<double>(const ostring& name, std::vector<double>& values, BindInfo::BindDirection mode, BindInfo::VectorType type)
    {
        return BindVector1(OCI_BindArrayOfDoubles, name, values, mode, type);
    }

    template<>
    inline BindInfo Statement::Bind<Date>(const ostring& name, std::vector<Date>& values, BindInfo::BindDirection mode, BindInfo::VectorType type)
    {
        return BindVector1(OCI_BindArrayOfDates, name, values, mode, type);
    }

    template<>
    inline BindInfo Statement::Bind<Number>(const ostring& name, std::vector<Number>& values, BindInfo::BindDirection mode, BindInfo::VectorType type)
    {
        return BindVector1(OCI_BindArrayOfNumbers, name, values, mode, type);
    }

    template<>
    inline BindInfo Statement::Bind<Json>(const ostring& name, std::vector<Json>& values, BindInfo::BindDirection mode, BindInfo::VectorType type)
    {
        return BindVector1(OCI_BindArrayOfJsons, name, values, mode, type);
    }

    template<class T>
    BindInfo Statement::Bind(const ostring& name, Collection<T>& value, BindInfo::BindDirection mode)
    {
        BindInfo info(core::Check(OCI_BindColl(*this, name.c_str(), value)), GetHandle());
        if (!info.IsNull())
        {
            info.SetDirection(mode);
        }
        return info;
    }

    template<>
    inline BindInfo Statement::Bind<Timestamp, Timestamp::TimestampTypeValues>(const ostring& name, std::vector<Timestamp>& values, Timestamp::TimestampTypeValues subType, BindInfo::BindDirection mode, BindInfo::VectorType type)
    {
        return BindVector2(OCI_BindArrayOfTimestamps, name, values, mode, subType, type);
    }

    template<>
    inline BindInfo Statement::Bind<Timestamp, Timestamp::TimestampType>(const ostring& name, std::vector<Timestamp>& values, Timestamp::TimestampType subType, BindInfo::BindDirection mode, BindInfo::VectorType type)
    {
        return Bind<Timestamp, Timestamp::TimestampTypeValues>(name, values, subType.GetValue(), mode, type);
    }

    template<>
    inline BindInfo Statement::Bind<Interval, Interval::IntervalTypeValues>(const ostring& name, std::vector<Interval>& values, Interval::IntervalTypeValues subType, BindInfo::BindDirection mode, BindInfo::VectorType type)
    {
        return BindVector2(OCI_BindArrayOfIntervals, name, values, mode, subType, type);
    }

    template<>
    inline BindInfo Statement::Bind<Interval, Interval::IntervalType>(const ostring& name, std::vector<Interval>& values, Interval::IntervalType subType, BindInfo::BindDirection mode, BindInfo::VectorType type)
    {
        return Bind<Interval, Interval::IntervalTypeValues>(name, values, subType.GetValue(), mode, type);
    }

    template<>
    inline BindInfo Statement::Bind<Clob>(const ostring& name, std::vector<Clob>& values, BindInfo::BindDirection mode, BindInfo::VectorType type)
    {
        return BindVector2(OCI_BindArrayOfLobs, name, values, mode, static_cast<OCI_UInt32>(OCI_CLOB), type);
    }

    template<>
    inline BindInfo Statement::Bind<NClob>(const ostring& name, std::vector<NClob>& values, BindInfo::BindDirection mode, BindInfo::VectorType type)
    {
        return BindVector2(OCI_BindArrayOfLobs, name, values, mode, static_cast<OCI_UInt32>(OCI_NCLOB), type);
    }

    template<>
    inline BindInfo Statement::Bind<Blob>(const ostring& name, std::vector<Blob>& values, BindInfo::BindDirection mode, BindInfo::VectorType type)
    {
        return BindVector2(OCI_BindArrayOfLobs, name, values, mode, static_cast<OCI_UInt32>(OCI_BLOB), type);
    }

    template<>
    inline BindInfo Statement::Bind<File>(const ostring& name, std::vector<File>& values, BindInfo::BindDirection mode, BindInfo::VectorType type)
    {
        return BindVector2(OCI_BindArrayOfFiles, name, values, mode, static_cast<OCI_UInt32>(OCI_BFILE), type);
    }

    template<>
    inline BindInfo Statement::Bind<Object>(const ostring& name, std::vector<Object>& values, TypeInfo& typeInfo, BindInfo::BindDirection mode, BindInfo::VectorType type)
    {
        return BindVector2(OCI_BindArrayOfObjects, name, values, mode, static_cast<OCI_TypeInfo*>(typeInfo), type);
    }

    template<>
    inline BindInfo Statement::Bind<Reference>(const ostring& name, std::vector<Reference>& values, TypeInfo& typeInfo, BindInfo::BindDirection mode, BindInfo::VectorType type)
    {
        return BindVector2(OCI_BindArrayOfRefs, name, values, mode, static_cast<OCI_TypeInfo*>(typeInfo), type);
    }

    template<class T>
    BindInfo Statement::Bind(const ostring& name, std::vector<Collection<T> >& values, TypeInfo& typeInfo, BindInfo::BindDirection mode, BindInfo::VectorType type)
    {
        return BindVector2(OCI_BindArrayOfColls, name, values, mode, static_cast<OCI_TypeInfo*>(typeInfo), type);
    }

    template<>
    inline BindInfo Statement::Bind<ostring, OCI_UInt32>(const ostring& name, std::vector<ostring>& values, OCI_UInt32 maxSize, BindInfo::BindDirection mode, BindInfo::VectorType type)
    {
        support::BindArray* bnd = core::OnAllocate(new support::BindArray(*this, name, mode));
        bnd->SetVector<ostring>(values, type == BindInfo::AsPlSqlTable, maxSize + 1);

        OCI_Bind* res = OCI_BindArrayOfStrings(*this, name.c_str(), bnd->GetData<ostring>(), maxSize, bnd->GetSizeForBindCall());

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

    template<>
    inline BindInfo Statement::Bind<ostring, OCI_Int32>(const ostring& name, std::vector<ostring>& values, OCI_Int32 maxSize, BindInfo::BindDirection mode, BindInfo::VectorType type)
    {
        return Bind<ostring, OCI_UInt32>(name, values, static_cast<OCI_UInt32>(maxSize), mode, type);
    }

    template<>
    inline BindInfo Statement::Bind<Raw, OCI_UInt32>(const ostring& name, std::vector<Raw>& values, OCI_UInt32 maxSize, BindInfo::BindDirection mode, BindInfo::VectorType type)
    {
        support::BindArray* bnd = core::OnAllocate(new support::BindArray(*this, name, mode));
        bnd->SetVector<Raw>(values, type == BindInfo::AsPlSqlTable, maxSize);

        OCI_Bind* res = OCI_BindArrayOfRaws(*this, name.c_str(), bnd->GetData<Raw>(), maxSize, bnd->GetSizeForBindCall());

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
    BindInfo Statement::Bind(const ostring& name, std::vector<T>& values, TypeInfo& typeInfo, BindInfo::BindDirection mode, BindInfo::VectorType type)
    {
        return BindVector2(OCI_BindArrayOfColls, name, values, mode, static_cast<OCI_TypeInfo*>(typeInfo), GetArraysize(type, values));
    }

    template<>
    inline void Statement::Register<OCI_UInt16>(const ostring& name)
    {
        core::Check(OCI_RegisterUnsignedShort(*this, name.c_str()));
    }

    template<>
    inline void Statement::Register<OCI_Int16>(const ostring& name)
    {
        core::Check(OCI_RegisterShort(*this, name.c_str()));
    }

    template<>
    inline void Statement::Register<OCI_UInt32>(const ostring& name)
    {
        core::Check(OCI_RegisterUnsignedInt(*this, name.c_str()));
    }

    template<>
    inline void Statement::Register<OCI_Int32>(const ostring& name)
    {
        core::Check(OCI_RegisterInt(*this, name.c_str()));
    }

    template<>
    inline void Statement::Register<OCI_UInt64>(const ostring& name)
    {
        core::Check(OCI_RegisterUnsignedBigInt(*this, name.c_str()));
    }

    template<>
    inline void Statement::Register<OCI_Int64>(const ostring& name)
    {
        core::Check(OCI_RegisterBigInt(*this, name.c_str()));
    }

    template<>
    inline void Statement::Register<float>(const ostring& name)
    {
        core::Check(OCI_RegisterFloat(*this, name.c_str()));
    }

    template<>
    inline void Statement::Register<double>(const ostring& name)
    {
        core::Check(OCI_RegisterDouble(*this, name.c_str()));
    }

    template<>
    inline void Statement::Register<Number>(const ostring& name)
    {
        core::Check(OCI_RegisterNumber(*this, name.c_str()));
    }

    template<>
    inline void Statement::Register<Date>(const ostring& name)
    {
        core::Check(OCI_RegisterDate(*this, name.c_str()));
    }

    template<>
    inline void Statement::Register<Timestamp, Timestamp::TimestampTypeValues>(const ostring& name, Timestamp::TimestampTypeValues type)
    {
        core::Check(OCI_RegisterTimestamp(*this, name.c_str(), type));
    }

    template<>
    inline void Statement::Register<Timestamp, Timestamp::TimestampType>(const ostring& name, Timestamp::TimestampType type)
    {
        Register<Timestamp, Timestamp::TimestampTypeValues>(name, type.GetValue());
    }

    template<>
    inline void Statement::Register<Interval, Interval::IntervalTypeValues>(const ostring& name, Interval::IntervalTypeValues type)
    {
        core::Check(OCI_RegisterInterval(*this, name.c_str(), type));
    }

    template<>
    inline void Statement::Register<Interval, Interval::IntervalType>(const ostring& name, Interval::IntervalType type)
    {
        Register<Interval, Interval::IntervalTypeValues>(name, type.GetValue());
    }

    template<>
    inline void Statement::Register<Clob>(const ostring& name)
    {
        core::Check(OCI_RegisterLob(*this, name.c_str(), OCI_CLOB));
    }

    template<>
    inline void Statement::Register<NClob>(const ostring& name)
    {
        core::Check(OCI_RegisterLob(*this, name.c_str(), OCI_NCLOB));
    }

    template<>
    inline void Statement::Register<Blob>(const ostring& name)
    {
        core::Check(OCI_RegisterLob(*this, name.c_str(), OCI_BLOB));
    }

    template<>
    inline void Statement::Register<File>(const ostring& name)
    {
        core::Check(OCI_RegisterFile(*this, name.c_str(), OCI_BFILE));
    }

    template<>
    inline void Statement::Register<Object, TypeInfo>(const ostring& name, TypeInfo& typeInfo)
    {
        core::Check(OCI_RegisterObject(*this, name.c_str(), typeInfo));
    }

    template<>
    inline void Statement::Register<Reference, TypeInfo>(const ostring& name, TypeInfo& typeInfo)
    {
        core::Check(OCI_RegisterRef(*this, name.c_str(), typeInfo));
    }

    template<>
    inline void Statement::Register<Vector>(const ostring& name)
    {
        core::Check(OCI_RegisterVector(*this, name.c_str()));
    }

    template<>
    inline void Statement::Register<Json>(const ostring& name)
    {
        core::Check(OCI_RegisterJson(*this, name.c_str()));
    }

    template<>
    inline void Statement::Register<ostring, OCI_UInt32>(const ostring& name, OCI_UInt32 len)
    {
        core::Check(OCI_RegisterString(*this, name.c_str(), len));
    }

    template<>
    inline void Statement::Register<ostring, OCI_Int32>(const ostring& name, OCI_Int32 len)
    {
        Register<ostring, OCI_UInt32>(name, static_cast<OCI_UInt32>(len));
    }

    template<>
    inline void Statement::Register<Raw, OCI_UInt32>(const ostring& name, OCI_UInt32 len)
    {
        core::Check(OCI_RegisterRaw(*this, name.c_str(), len));
    }

    template<>
    inline void Statement::Register<Raw, OCI_Int32>(const ostring& name, OCI_Int32 len)
    {
        Register<Raw, OCI_UInt32>(name, static_cast<OCI_UInt32>(len));
    }

    inline Statement::StatementType Statement::GetStatementType() const
    {
        return StatementType(static_cast<StatementType::Type>(core::Check(OCI_GetStatementType(*this))));
    }

    inline OCI_UInt32 Statement::GetSqlErrorPos() const
    {
        return core::Check(OCI_GetSqlErrorPos(*this));
    }

    inline void Statement::SetFetchMode(FetchMode value)
    {
        core::Check(OCI_SetFetchMode(*this, value));
    }

    inline Statement::FetchMode Statement::GetFetchMode() const
    {
        return FetchMode(static_cast<FetchMode::Type>(core::Check(OCI_GetFetchMode(*this))));
    }

    inline void Statement::SetBindMode(BindMode value)
    {
        core::Check(OCI_SetBindMode(*this, value));
    }

    inline Statement::BindMode Statement::GetBindMode() const
    {
        return BindMode(static_cast<BindMode::Type>(core::Check(OCI_GetBindMode(*this))));
    }

    inline void Statement::SetFetchSize(OCI_UInt32 value)
    {
        core::Check(OCI_SetFetchSize(*this, value));
    }

    inline OCI_UInt32 Statement::GetFetchSize() const
    {
        return core::Check(OCI_GetFetchSize(*this));
    }

    inline void Statement::SetPrefetchSize(OCI_UInt32 value)
    {
        core::Check(OCI_SetPrefetchSize(*this, value));
    }

    inline OCI_UInt32 Statement::GetPrefetchSize() const
    {
        return core::Check(OCI_GetPrefetchSize(*this));
    }

    inline void Statement::SetPrefetchMemory(OCI_UInt32 value)
    {
        core::Check(OCI_SetPrefetchMemory(*this, value));
    }

    inline OCI_UInt32 Statement::GetPrefetchMemory() const
    {
        return core::Check(OCI_GetPrefetchMemory(*this));
    }

    inline void Statement::SetPieceSize(OCI_UInt32 value)
    {
        core::Check(OCI_SetPieceSize(*this, value));
    }

    inline OCI_UInt32 Statement::GetPieceSize() const
    {
        return core::Check(OCI_GetPieceSize(*this));
    }

    inline void Statement::SetLongMode(LongMode value)
    {
        core::Check(OCI_SetLongMode(*this, value));
    }

    inline Statement::LongMode Statement::GetLongMode() const
    {
        return LongMode(static_cast<LongMode::Type>(core::Check(OCI_GetLongMode(*this))));
    }

    inline OCI_UInt32 Statement::GetSQLCommand() const
    {
        return core::Check(OCI_GetSQLCommand(*this));
    }

    inline ostring Statement::GetSQLVerb() const
    {
        return core::MakeString(core::Check(OCI_GetSQLVerb(*this)));
    }
    
    inline std::vector<ostring> Statement::GetParseBindNames() const
    {
        OCI_UInt32 count = 0;
        const otext** names = nullptr;

        core::Check(OCI_GetParseBindNames(*this, &count, &names));

        std::vector<ostring> result;

        result.reserve(count);

        for (OCI_UInt32 i = 0; i < count; ++i)
        {
            result.push_back(core::MakeString(names[i]));
        }

        return result;
    }

    inline void Statement::GetBatchErrors(std::vector<Exception>& exceptions)
    {
        exceptions.clear();

        OCI_Error* err = core::Check(OCI_GetBatchError(*this));

        while (err)
        {
            exceptions.push_back(Exception(err));

            err = core::Check(OCI_GetBatchError(*this));
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
