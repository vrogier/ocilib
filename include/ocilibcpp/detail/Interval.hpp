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

inline Interval::Interval()
{
}

inline Interval::Interval(IntervalType type)
{
    AcquireAllocated
    (
        core::Check(OCI_IntervalCreate(nullptr, type)), 
        Environment::GetEnvironmentHandle()
    );
}

inline Interval::Interval(IntervalType type, const ostring& data)
{
    AcquireAllocated
    (
        core::Check(OCI_IntervalCreate(nullptr, type)),
        Environment::GetEnvironmentHandle()
    );

    FromString(data);
}

inline Interval::Interval(OCI_Interval *pInterval, core::Handle *parent)
{
    AcquireTransient(pInterval, parent);
}

inline Interval Interval::Clone() const
{
    Interval result(GetType());

    core::Check(OCI_IntervalAssign(result, *this));

    return result;
}

inline int Interval::Compare(const Interval& other) const
{
    return core::Check(OCI_IntervalCompare(*this, other));
}

inline Interval::IntervalType Interval::GetType() const
{
    return IntervalType(static_cast<IntervalType::Type>(core::Check(OCI_IntervalGetType(*this))));
}

inline bool Interval::IsValid() const
{
    return (core::Check(OCI_IntervalCheck(*this)) == 0);
}

inline int Interval::GetYear() const
{
    int year = 0, month = 0;

    GetYearMonth(year, month);

    return year;
}

inline void Interval::SetYear(int value)
{
    int year = 0, month = 0;

    GetYearMonth(year, month);
    SetYearMonth(value, month);
}

inline int Interval::GetMonth() const
{
    int year = 0, month = 0;

    GetYearMonth(year, month);

    return month;
}

inline void Interval::SetMonth(int value)
{
    int year = 0, month = 0;

    GetYearMonth(year, month);
    SetYearMonth(year, value);
}

inline int Interval::GetDay() const
{
    int day = 0, hour = 0, minutes = 0, seconds = 0, milliseconds = 0;

    GetDaySecond(day, hour, minutes, seconds, milliseconds);

    return day;
}

inline void Interval::SetDay(int value)
{
    int day = 0, hour = 0, minutes = 0, seconds = 0, milliseconds = 0;

    GetDaySecond(day, hour, minutes, seconds, milliseconds);
    SetDaySecond(value, hour, minutes, seconds, milliseconds);
}

inline int Interval::GetHours() const
{
    int day = 0, hour = 0, minutes = 0, seconds = 0, milliseconds = 0;

    GetDaySecond(day, hour, minutes, seconds, milliseconds);

    return hour;
}

inline void Interval::SetHours(int value)
{
    int day = 0, hour = 0, minutes = 0, seconds = 0, milliseconds = 0;

    GetDaySecond(day, hour, minutes, seconds, milliseconds);
    SetDaySecond(day, value, minutes, seconds, milliseconds);
}

inline int Interval::GetMinutes() const
{
    int day = 0, hour = 0, minutes = 0, seconds = 0, milliseconds = 0;

    GetDaySecond(day, hour, minutes, seconds, milliseconds);

    return minutes;
}

inline void Interval::SetMinutes(int value)
{
    int day = 0, hour = 0, minutes = 0, seconds = 0, milliseconds = 0;

    GetDaySecond(day, hour, minutes, seconds, milliseconds);
    SetDaySecond(day, hour, value, seconds, milliseconds);
}

inline int Interval::GetSeconds() const
{
    int day, hour, minutes, seconds, milliseconds;

    GetDaySecond(day, hour, minutes, seconds, milliseconds);

    return seconds;
}

inline void Interval::SetSeconds(int value)
{
    int day = 0, hour = 0, minutes = 0, seconds = 0, milliseconds = 0;

    GetDaySecond(day, hour, minutes, seconds, milliseconds);
    SetDaySecond(day, hour, minutes, value, milliseconds);
}

inline int Interval::GetMilliSeconds() const
{
    int day = 0, hour = 0, minutes = 0, seconds = 0, milliseconds = 0;

    GetDaySecond(day, hour, minutes, seconds, milliseconds);

    return milliseconds;
}

inline void Interval::SetMilliSeconds(int value)
{
    int day = 0, hour = 0, minutes = 0, seconds = 0, milliseconds = 0;

    GetDaySecond(day, hour, minutes, seconds, milliseconds);
    SetDaySecond(day, hour, minutes, seconds, value);
}

inline void Interval::GetDaySecond(int &day, int &hour, int &min, int &sec, int &fsec) const
{
    core::Check(OCI_IntervalGetDaySecond(*this, &day, &hour, &min, &sec, &fsec));
}

inline void Interval::SetDaySecond(int day, int hour, int min, int sec, int fsec)
{
    core::Check(OCI_IntervalSetDaySecond(*this, day, hour, min, sec, fsec));
}

inline void Interval::GetYearMonth(int &year, int &month) const
{
    core::Check(OCI_IntervalGetYearMonth(*this, &year, &month));
}
inline void Interval::SetYearMonth(int year, int month)
{
    core::Check(OCI_IntervalSetYearMonth(*this, year, month));
}

inline void Interval::UpdateTimeZone(const ostring& timeZone)
{
    core::Check(OCI_IntervalFromTimeZone(*this, timeZone.c_str()));
}

inline void Interval::FromString(const ostring& data)
{
    core::Check(OCI_IntervalFromText(*this, data.c_str()));
}

inline ostring Interval::ToString(int leadingPrecision, int fractionPrecision) const
{
    if (!IsNull())
    {
        const size_t size = OCI_SIZE_BUFFER;

        core::ManagedBuffer<otext> buffer(static_cast<size_t>(size + 1));

        core::Check(OCI_IntervalToText(*this, leadingPrecision, fractionPrecision, static_cast<int>(size), buffer));

        return core::MakeString(static_cast<const otext *>(buffer));
    }

    return OCI_STRING_NULL;
}

inline ostring Interval::ToString() const
{
    return ToString(OCI_STRING_DEFAULT_PREC, OCI_STRING_DEFAULT_PREC);
}

inline Interval Interval::operator + (const Interval& other) const
{
    Interval result = Clone();
    return result += other;
}

inline Interval Interval::operator - (const Interval& other) const
{
    Interval result = Clone();
    return result -= other;
}

inline Interval& Interval::operator += (const Interval& other)
{
    core::Check(OCI_IntervalAdd(*this, other));
    return *this;
}

inline Interval& Interval::operator -= (const Interval& other)
{
    core::Check(OCI_IntervalSubtract(*this, other));
    return *this;
}

inline bool Interval::operator == (const Interval& other) const
{
    return Compare(other) == 0;
}

inline bool Interval::operator != (const Interval& other) const
{
    return (!(*this == other));
}

inline bool Interval::operator > (const Interval& other) const
{
    return (Compare(other) > 0);
}

inline bool Interval::operator < (const Interval& other) const
{
    return (Compare(other) < 0);
}

inline bool Interval::operator >= (const Interval& other) const
{
    const int res = Compare(other);

    return (res == 0 || res < 0);
}

inline bool Interval::operator <= (const Interval& other) const
{
    const int res = Compare(other);

    return (res == 0 || res > 0);
}

}
