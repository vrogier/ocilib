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

// ReSharper disable CppClangTidyHicppUseEqualsDefault
// ReSharper disable CppClangTidyModernizeUseEqualsDefault

namespace ocilib
{

inline Timestamp::Timestamp()
{
}

inline Timestamp::Timestamp(const Environment& env, TimestampType type)
{
    Allocate(env.GetHandle(), type);
}

inline Timestamp::Timestamp(const Environment& env, TimestampType type, const ostring& data, const ostring& format)
{
    AcquireAllocated
    (
        core::Check(OCI_TimestampCreate(env.GetHandle(), type)),
        static_cast<core::Handle*>(nullptr)
    );

    FromString(data, format);
}

inline Timestamp::Timestamp(OCI_Timestamp *pTimestamp, core::Handle *parent)
{
    AcquireTransient(pTimestamp, parent);
}

inline Timestamp::Timestamp(const Timestamp& other)
{
    *this = other;
}

inline Timestamp& Timestamp::operator= (const Timestamp& other) noexcept
{
    if (this != &other)
    {
        if (IsNull())
        {
            Allocate(other.GetEnvironment(), other.GetType());
        }

        core::Check(OCI_TimestampAssign(*this, other));
    }

    return *this;
}

inline void Timestamp::Allocate(OCI_Environment* env, TimestampType type)
{
    AcquireAllocated
    (
        core::Check(OCI_TimestampCreate(env, type)),
        static_cast<core::Handle*>(nullptr)
    );
}

inline Timestamp Timestamp::Clone() const
{
    Timestamp result;
    result.AcquireAllocated
    (
        core::Check(OCI_TimestampCreate(GetEnvironment(), GetType())),
        static_cast<core::Handle*>(nullptr)
    );

    core::Check(OCI_TimestampAssign(result, *this));

    return result;
}

inline OCI_Int32 Timestamp::Compare(const Timestamp& other) const
{
    return core::Check(OCI_TimestampCompare(*this, other));
}

inline Timestamp::TimestampType Timestamp::GetType() const
{
    return TimestampType(static_cast<TimestampType::Type>(core::Check(OCI_TimestampGetType(*this))));
}

inline void Timestamp::SetDateTime(OCI_Int32 year, OCI_Int32 month, OCI_Int32 day, OCI_Int32 hour, OCI_Int32 min, OCI_Int32 sec, OCI_Int32 fsec, const ostring& timeZone)
{
    core::Check(OCI_TimestampConstruct(*this, year, month, day, hour, min,sec, fsec, timeZone.c_str()));
}

inline void Timestamp::Convert(const Timestamp& other)
{
    core::Check(OCI_TimestampConvert(*this, other));
}

inline bool Timestamp::IsValid() const
{
    return (core::Check(OCI_TimestampCheck(*this)) == 0);
}

inline OCI_Int32 Timestamp::GetYear() const
{
    OCI_Int32 year, month, day;

    GetDate(year, month, day);

    return year;
}

inline void Timestamp::SetYear(OCI_Int32 value)
{
    OCI_Int32 year, month, day;

    GetDate(year, month, day);
    SetDate(value, month, day);
}

inline OCI_Int32 Timestamp::GetMonth() const
{
    OCI_Int32 year, month, day;

    GetDate(year, month, day);

    return month;
}

inline void Timestamp::SetMonth(OCI_Int32 value)
{
    OCI_Int32 year, month, day;

    GetDate(year, month, day);
    SetDate(year, value, day);
}

inline OCI_Int32 Timestamp::GetDay() const
{
    OCI_Int32 year, month, day;

    GetDate(year, month, day);

    return day;
}

inline void Timestamp::SetDay(OCI_Int32 value)
{
    OCI_Int32 year, month, day;

    GetDate(year, month, day);
    SetDate(year, month, value);
}

inline OCI_Int32 Timestamp::GetHours() const
{
    OCI_Int32 hour, minutes, seconds, milliseconds;

    GetTime(hour, minutes, seconds, milliseconds);

    return hour;
}

inline void Timestamp::SetHours(OCI_Int32 value)
{
    OCI_Int32 hour, minutes, seconds, milliseconds;

    GetTime(hour, minutes, seconds, milliseconds);
    SetTime(value, minutes, seconds, milliseconds);
}

inline OCI_Int32 Timestamp::GetMinutes() const
{
    OCI_Int32 hour, minutes, seconds, milliseconds;

    GetTime(hour, minutes, seconds, milliseconds);

    return minutes;
}

inline void Timestamp::SetMinutes(OCI_Int32 value)
{
    OCI_Int32 hour = 0, minutes = 0, seconds = 0, milliseconds = 0;

    GetTime(hour, minutes, seconds, milliseconds);
    SetTime(hour, value, seconds, milliseconds);
}

inline OCI_Int32 Timestamp::GetSeconds() const
{
    OCI_Int32 hour = 0, minutes = 0, seconds = 0, milliseconds = 0;

    GetTime(hour, minutes, seconds, milliseconds);

    return seconds;
}

inline void Timestamp::SetSeconds(OCI_Int32 value)
{
    OCI_Int32 hour = 0, minutes = 0, seconds = 0, milliseconds = 0;

    GetTime(hour, minutes, seconds, milliseconds);
    SetTime(hour, minutes, value, milliseconds);
}

inline OCI_Int32 Timestamp::GetMilliSeconds() const
{
    OCI_Int32 hour = 0, minutes = 0, seconds = 0, milliseconds = 0;

    GetTime(hour, minutes, seconds, milliseconds);

    return milliseconds;
}

inline void Timestamp::SetMilliSeconds(OCI_Int32 value)
{
    OCI_Int32 hour = 0, minutes = 0, seconds = 0, milliseconds = 0;

    GetTime(hour, minutes, seconds, milliseconds);
    SetTime(hour, minutes, seconds, value);
}

inline void Timestamp::GetDate(OCI_Int32 &year, OCI_Int32 &month, OCI_Int32 &day) const
{
    core::Check(OCI_TimestampGetDate(*this, &year, &month, &day));
}

inline void Timestamp::GetTime(OCI_Int32 &hour, OCI_Int32 &min, OCI_Int32 &sec, OCI_Int32 &fsec) const
{
    core::Check(OCI_TimestampGetTime(*this, &hour, &min, &sec, &fsec));
}

inline void Timestamp::GetDateTime(OCI_Int32 &year, OCI_Int32 &month, OCI_Int32 &day, OCI_Int32 &hour, OCI_Int32 &min, OCI_Int32 &sec, OCI_Int32 &fsec) const
{
    core::Check(OCI_TimestampGetDateTime(*this, &year, &month, &day, &hour, &min, &sec, &fsec));
}

inline void Timestamp::SetDate(OCI_Int32 year, OCI_Int32 month, OCI_Int32 day)
{
    OCI_Int32 tmpYear = 0, tmpMonth = 0, tempDay = 0, hour = 0, minutes = 0, seconds = 0, milliseconds = 0;

    GetDateTime(tmpYear, tmpMonth, tempDay, hour, minutes, seconds, milliseconds);
    SetDateTime(year, month, day, hour, minutes, seconds, milliseconds);
}

inline void Timestamp::SetTime(OCI_Int32 hour, OCI_Int32 min, OCI_Int32 sec, OCI_Int32 fsec)
{
    OCI_Int32 year = 0, month = 0, day = 0, tmpHour = 0, tmpMinutes = 0, tmpSeconds = 0, tmpMilliseconds = 0;

    GetDateTime(year, month, day, tmpHour, tmpMinutes, tmpSeconds, tmpMilliseconds);
    SetDateTime(year, month, day, hour, min, sec, fsec);
}

inline void Timestamp::SetTimeZone(const ostring& timeZone)
{
    if (GetType() == WithTimeZone)
    {
        OCI_Int32 year = 0, month = 0, day = 0, hour = 0, minutes = 0, seconds = 0, milliseconds = 0;

        GetDateTime(year, month, day, hour, minutes, seconds, milliseconds);
        SetDateTime(year, month, day, hour, minutes, seconds, milliseconds, timeZone);
    }
}

inline ostring Timestamp::GetTimeZone() const
{
    if (GetType() != NoTimeZone)
    {
        const OCI_Size size = OCI_SIZE_BUFFER;

        std::vector<otext> buffer(static_cast<OCI_Size>(size + 1));

        core::Check(OCI_TimestampGetTimeZoneName(*this, static_cast<OCI_Int32>(size), buffer.data()) == TRUE);

        return core::MakeString(static_cast<const otext*>(buffer.data()));
    }

    return ostring();
}

inline void Timestamp::GetTimeZoneOffset(OCI_Int32 &hour, OCI_Int32 &min) const
{
    core::Check(OCI_TimestampGetTimeZoneOffset(*this, &hour, &min));
}

inline void Timestamp::Substract(const Timestamp &lsh, const Timestamp &rsh, Interval& result)
{
    core::Check(OCI_TimestampSubtract(lsh, rsh, result));
}

inline Timestamp Timestamp::SysTimestamp(const Environment& env, TimestampType type)
{
    Timestamp result(env, type);

    core::Check(OCI_TimestampSysTimestamp(result));

    return result;
}

inline void Timestamp::FromString(const ostring& data, const ostring& format)
{
    core::Check(OCI_TimestampFromText(*this, data.c_str(), format.empty() ?  core::MakeString(core::Check(OCI_GetFormat(GetEnvironment(), FormatTimestamp))).c_str() : format.c_str()));
}

inline ostring Timestamp::ToString(const ostring& format, OCI_Int32 precision = OCI_STRING_DEFAULT_PREC) const
{
    if (!IsNull())
    {
        const OCI_Size size = OCI_SIZE_BUFFER;

        std::vector<otext> buffer(static_cast<OCI_Size>(size + 1));

        core::Check(OCI_TimestampToText(*this, format.c_str(), static_cast<OCI_Int32>(size), buffer.data(), precision));

        return core::MakeString(static_cast<const otext*>(buffer.data()));
    }

    return OCI_STRING_NULL;
}

inline ostring Timestamp::ToString() const
{
    if (IsNull())
    {
        return OCI_STRING_NULL;
    }

    return ToString(core::MakeString(core::Check(OCI_GetFormat(GetEnvironment(), FormatTimestamp))), OCI_STRING_DEFAULT_PREC);
}

inline Timestamp& Timestamp::operator ++ ()
{
    return *this += 1;
}

inline Timestamp Timestamp::operator ++ (OCI_Int32)
{
    Timestamp result = Clone();

    *this += 1;

    return result;
}

inline Timestamp& Timestamp::operator -- ()
{
    return *this -= 1;
}

inline Timestamp Timestamp::operator -- (OCI_Int32)
{
    Timestamp result = Clone();

    *this -= 1;

    return result;
}

inline Timestamp Timestamp::operator + (OCI_Int32 value) const
{
    Timestamp result = Clone();
    Interval interval(GetEnvironment(), Interval::DaySecond);
    interval.SetDay(1);
    return result += value;
}

inline Timestamp Timestamp::operator - (OCI_Int32 value) const
{
    Timestamp result = Clone();
    Interval interval(GetEnvironment(), Interval::DaySecond);
    interval.SetDay(1);
    return result -= value;
}

inline Interval Timestamp::operator - (const Timestamp& other)
{
    Interval interval(GetEnvironment(), Interval::DaySecond);
    core::Check(OCI_TimestampSubtract(*this, other, interval));
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
    core::Check(OCI_TimestampIntervalAdd(*this, other));
    return *this;
}

inline Timestamp& Timestamp::operator -= (const Interval& other)
{
    core::Check(OCI_TimestampIntervalSub(*this, other));
    return *this;
}

inline Timestamp& Timestamp::operator += (OCI_Int32 value)
{
    Interval interval(GetEnvironment(), Interval::DaySecond);
    interval.SetDay(value);
    return *this += interval;
}

inline Timestamp& Timestamp::operator -= (OCI_Int32 value)
{
    Interval interval(GetEnvironment(), Interval::DaySecond);
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
    const OCI_Int32 res = Compare(other);

    return (res == 0 || res < 0);
}

inline bool Timestamp::operator <= (const Timestamp& other) const
{
    const OCI_Int32 res = Compare(other);

    return (res == 0 || res > 0);
}

}
