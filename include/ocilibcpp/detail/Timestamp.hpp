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

namespace ocilib
{

    inline Timestamp::Timestamp()
    {
    }

    inline Timestamp::Timestamp(const Environment& env, TimestampType type)
    {
        Allocate(env.GetHandle(), type, env.GetEnvironmentHandle());
    }

    inline Timestamp::Timestamp(const Environment& env, TimestampType type, const String& data, const String& format)
    {
        AcquireAllocated
        (
            core::Check(OCI_TimestampCreate(env.GetHandle(), type)),
            env.GetEnvironmentHandle()
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

    inline Timestamp& Timestamp::operator= (const Timestamp& other)
    {
        if (this != &other)
        {
            if (IsNull())
            {
                Allocate(other.GetEnvironment(), other.GetType(), other.GetParent());
            }

            core::Check(OCI_TimestampAssign(*this, other));
        }

        return *this;
    }

    inline void Timestamp::Allocate(OCI_Environment* env, TimestampType type, core::Handle* parent)
    {
        AcquireAllocated
        (
            core::Check(OCI_TimestampCreate(env, type)),
            parent
        );
    }

    inline Timestamp Timestamp::Clone() const
    {
        Timestamp result;
        result.AcquireAllocated
        (
            core::Check(OCI_TimestampCreate(GetEnvironment(), GetType())),
            GetParent()
        );

        core::Check(OCI_TimestampAssign(result, *this));

        return result;
    }

    inline Int32 Timestamp::Compare(const Timestamp& other) const
    {
        return core::Check(OCI_TimestampCompare(*this, other));
    }

    inline Timestamp::TimestampType Timestamp::GetType() const
    {
        return TimestampType(static_cast<TimestampType::Type>(core::Check(OCI_TimestampGetType(*this))));
    }

    inline void Timestamp::SetDateTime(Int32 year, Int32 month, Int32 day, Int32 hour, Int32 min, Int32 sec, Int32 fsec, const String& timeZone)
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

    inline Int32 Timestamp::GetYear() const
    {
        Int32 year, month, day;

        GetDate(year, month, day);

        return year;
    }

    inline void Timestamp::SetYear(Int32 value)
    {
        Int32 year, month, day;

        GetDate(year, month, day);
        SetDate(value, month, day);
    }

    inline Int32 Timestamp::GetMonth() const
    {
        Int32 year, month, day;

        GetDate(year, month, day);

        return month;
    }

    inline void Timestamp::SetMonth(Int32 value)
    {
        Int32 year, month, day;

        GetDate(year, month, day);
        SetDate(year, value, day);
    }

    inline Int32 Timestamp::GetDay() const
    {
        Int32 year, month, day;

        GetDate(year, month, day);

        return day;
    }

    inline void Timestamp::SetDay(Int32 value)
    {
        Int32 year, month, day;

        GetDate(year, month, day);
        SetDate(year, month, value);
    }

    inline Int32 Timestamp::GetHours() const
    {
        Int32 hour, minutes, seconds, milliseconds;

        GetTime(hour, minutes, seconds, milliseconds);

        return hour;
    }

    inline void Timestamp::SetHours(Int32 value)
    {
        Int32 hour, minutes, seconds, milliseconds;

        GetTime(hour, minutes, seconds, milliseconds);
        SetTime(value, minutes, seconds, milliseconds);
    }

    inline Int32 Timestamp::GetMinutes() const
    {
        Int32 hour, minutes, seconds, milliseconds;

        GetTime(hour, minutes, seconds, milliseconds);

        return minutes;
    }

    inline void Timestamp::SetMinutes(Int32 value)
    {
        Int32 hour = 0, minutes = 0, seconds = 0, milliseconds = 0;

        GetTime(hour, minutes, seconds, milliseconds);
        SetTime(hour, value, seconds, milliseconds);
    }

    inline Int32 Timestamp::GetSeconds() const
    {
        Int32 hour = 0, minutes = 0, seconds = 0, milliseconds = 0;

        GetTime(hour, minutes, seconds, milliseconds);

        return seconds;
    }

    inline void Timestamp::SetSeconds(Int32 value)
    {
        Int32 hour = 0, minutes = 0, seconds = 0, milliseconds = 0;

        GetTime(hour, minutes, seconds, milliseconds);
        SetTime(hour, minutes, value, milliseconds);
    }

    inline Int32 Timestamp::GetMilliSeconds() const
    {
        Int32 hour = 0, minutes = 0, seconds = 0, milliseconds = 0;

        GetTime(hour, minutes, seconds, milliseconds);

        return milliseconds;
    }

    inline void Timestamp::SetMilliSeconds(Int32 value)
    {
        Int32 hour = 0, minutes = 0, seconds = 0, milliseconds = 0;

        GetTime(hour, minutes, seconds, milliseconds);
        SetTime(hour, minutes, seconds, value);
    }

    inline void Timestamp::GetDate(Int32 &year, Int32 &month, Int32 &day) const
    {
        core::Check(OCI_TimestampGetDate(*this, &year, &month, &day));
    }

    inline void Timestamp::GetTime(Int32 &hour, Int32 &min, Int32 &sec, Int32 &fsec) const
    {
        core::Check(OCI_TimestampGetTime(*this, &hour, &min, &sec, &fsec));
    }

    inline void Timestamp::GetDateTime(Int32 &year, Int32 &month, Int32 &day, Int32 &hour, Int32 &min, Int32 &sec, Int32 &fsec) const
    {
        core::Check(OCI_TimestampGetDateTime(*this, &year, &month, &day, &hour, &min, &sec, &fsec));
    }

    inline void Timestamp::SetDate(Int32 year, Int32 month, Int32 day)
    {
        Int32 tmpYear = 0, tmpMonth = 0, tempDay = 0, hour = 0, minutes = 0, seconds = 0, milliseconds = 0;

        GetDateTime(tmpYear, tmpMonth, tempDay, hour, minutes, seconds, milliseconds);
        SetDateTime(year, month, day, hour, minutes, seconds, milliseconds);
    }

    inline void Timestamp::SetTime(Int32 hour, Int32 min, Int32 sec, Int32 fsec)
    {
        Int32 year = 0, month = 0, day = 0, tmpHour = 0, tmpMinutes = 0, tmpSeconds = 0, tmpMilliseconds = 0;

        GetDateTime(year, month, day, tmpHour, tmpMinutes, tmpSeconds, tmpMilliseconds);
        SetDateTime(year, month, day, hour, min, sec, fsec);
    }

    inline void Timestamp::SetTimeZone(const String& timeZone)
    {
        if (GetType() == WithTimeZone)
        {
            Int32 year = 0, month = 0, day = 0, hour = 0, minutes = 0, seconds = 0, milliseconds = 0;

            GetDateTime(year, month, day, hour, minutes, seconds, milliseconds);
            SetDateTime(year, month, day, hour, minutes, seconds, milliseconds, timeZone);
        }
    }

    inline String Timestamp::GetTimeZone() const
    {
        if (GetType() != NoTimeZone)
        {
            const Size size = OCI_SIZE_BUFFER;

            std::vector<otext> buffer(static_cast<Size>(size + 1));

            core::Check(OCI_TimestampGetTimeZoneName(*this, static_cast<Int32>(size), buffer.data()) == TRUE);

            return core::MakeString(static_cast<const otext*>(buffer.data()));
        }

        return String();
    }

    inline void Timestamp::GetTimeZoneOffset(Int32 &hour, Int32 &min) const
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

    inline void Timestamp::FromString(const String& data, const String& format)
    {
        core::Check(OCI_TimestampFromText(*this, data.c_str(),
                                          format.empty() ?  core::MakeString(core::Check(OCI_GetFormat(GetEnvironment(),
                                                                                                       FormatTimestamp))).c_str() : format.c_str()));
    }

    inline String Timestamp::ToString(const String& format, Int32 precision = OCI_STRING_DEFAULT_PREC) const
    {
        if (!IsNull())
        {
            const Size size = OCI_SIZE_BUFFER;

            std::vector<otext> buffer(static_cast<Size>(size + 1));

            core::Check(OCI_TimestampToText(*this, format.c_str(), static_cast<Int32>(size), buffer.data(), precision));

            return core::MakeString(static_cast<const otext*>(buffer.data()));
        }

        return OCI_STRING_NULL;
    }

    inline String Timestamp::ToString() const
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

    inline Timestamp Timestamp::operator ++ (Int32)
    {
        Timestamp result = Clone();

        *this += 1;

        return result;
    }

    inline Timestamp& Timestamp::operator -- ()
    {
        return *this -= 1;
    }

    inline Timestamp Timestamp::operator -- (Int32)
    {
        Timestamp result = Clone();

        *this -= 1;

        return result;
    }

    inline Timestamp Timestamp::operator + (Int32 value) const
    {
        Timestamp result = Clone();
        Interval  interval(GetEnvironment(), Interval::DaySecond);
        interval.SetDay(1);
        return result += value;
    }

    inline Timestamp Timestamp::operator - (Int32 value) const
    {
        Timestamp result = Clone();
        Interval  interval(GetEnvironment(), Interval::DaySecond);
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

    inline Timestamp& Timestamp::operator += (Int32 value)
    {
        Interval interval(GetEnvironment(), Interval::DaySecond);
        interval.SetDay(value);
        return *this += interval;
    }

    inline Timestamp& Timestamp::operator -= (Int32 value)
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
        const Int32 res = Compare(other);

        return (res == 0 || res < 0);
    }

    inline bool Timestamp::operator <= (const Timestamp& other) const
    {
        const Int32 res = Compare(other);

        return (res == 0 || res > 0);
    }

}
