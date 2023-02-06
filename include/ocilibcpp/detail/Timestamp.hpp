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

namespace ocilib
{

inline Timestamp::Timestamp()
{
}

inline Timestamp::Timestamp(TimestampType type)
{
    AcquireAllocated
    (
        core::Check(OCI_TimestampCreate(nullptr, type)),
        Environment::GetEnvironmentHandle()
    );
}

inline Timestamp::Timestamp(TimestampType type, const ostring& data, const ostring& format)
{
    AcquireAllocated
    (
        core::Check(OCI_TimestampCreate(nullptr, type)),
        Environment::GetEnvironmentHandle()
    );

    FromString(data, format);
}

inline Timestamp::Timestamp(OCI_Timestamp *pTimestamp, core::Handle *parent)
{
    AcquireTransient(pTimestamp, parent);
}

inline Timestamp Timestamp::Clone() const
{
    Timestamp result(GetType());

    core::Check(OCI_TimestampAssign(result, *this));

    return result;
}

inline int Timestamp::Compare(const Timestamp& other) const
{
    return core::Check(OCI_TimestampCompare(*this, other));
}

inline Timestamp::TimestampType Timestamp::GetType() const
{
    return TimestampType(static_cast<TimestampType::Type>(core::Check(OCI_TimestampGetType(*this))));
}

inline void Timestamp::SetDateTime(int year, int month, int day, int hour, int min, int sec, int fsec, const ostring& timeZone)
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
    core::Check(OCI_TimestampGetDate(*this, &year, &month, &day));
}

inline void Timestamp::GetTime(int &hour, int &min, int &sec, int &fsec) const
{
    core::Check(OCI_TimestampGetTime(*this, &hour, &min, &sec, &fsec));
}

inline void Timestamp::GetDateTime(int &year, int &month, int &day, int &hour, int &min, int &sec, int &fsec) const
{
    core::Check(OCI_TimestampGetDateTime(*this, &year, &month, &day, &hour, &min, &sec, &fsec));
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
        const size_t size = OCI_SIZE_BUFFER;

        core::ManagedBuffer<otext> buffer(static_cast<size_t>(size + 1));

        core::Check(OCI_TimestampGetTimeZoneName(*this, static_cast<int>(size), buffer) == TRUE);

        return core::MakeString(static_cast<const otext *>(buffer));
    }

    return ostring();
}

inline void Timestamp::GetTimeZoneOffset(int &hour, int &min) const
{
    core::Check(OCI_TimestampGetTimeZoneOffset(*this, &hour, &min));
}

inline void Timestamp::Substract(const Timestamp &lsh, const Timestamp &rsh, Interval& result)
{
    core::Check(OCI_TimestampSubtract(lsh, rsh, result));
}

inline Timestamp Timestamp::SysTimestamp(TimestampType type)
{
    Timestamp result(type);

    core::Check(OCI_TimestampSysTimestamp(result));

    return result;
}

inline void Timestamp::FromString(const ostring& data, const ostring& format)
{
    core::Check(OCI_TimestampFromText(*this, data.c_str(), format.empty() ?  Environment::GetFormat(FormatTimestamp).c_str() : format.c_str()));
}

inline ostring Timestamp::ToString(const ostring& format, int precision = OCI_STRING_DEFAULT_PREC) const
{
    if (!IsNull())
    {
        const size_t size = OCI_SIZE_BUFFER;

        core::ManagedBuffer<otext> buffer(static_cast<size_t>(size + 1));

        core::Check(OCI_TimestampToText(*this, format.c_str(), static_cast<int>(size), buffer, precision));

        return core::MakeString(static_cast<const otext *>(buffer));
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
    const int res = Compare(other);

    return (res == 0 || res < 0);
}

inline bool Timestamp::operator <= (const Timestamp& other) const
{
    const int res = Compare(other);

    return (res == 0 || res > 0);
}

}
