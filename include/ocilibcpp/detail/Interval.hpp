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

    inline Interval::Interval()
    {
    }

    inline Interval::Interval(const Environment& env, IntervalType type)
    {
        Allocate(env.GetHandle(), static_cast<IntervalTypeValues>(type), env.GetEnvironmentHandle());
    }

    inline Interval::Interval(const Interval& other)
    {
        *this = other;
    }

    inline Interval& Interval::operator= (const Interval& other)
    {
        if (this != &other)
        {
            if (IsNull())
            {
                Allocate(other.GetEnvironment(), static_cast<IntervalTypeValues>(other.GetType()), other.GetParent());
            }

            core::Check(OCI_IntervalAssign(*this, other));
        }

        return *this;
    }

    inline Interval::Interval(const Environment& env, IntervalType type, const String& data)
    {
        AcquireAllocated
        (
            core::Check(OCI_IntervalCreate(env.GetHandle(), type)),
            env.GetEnvironmentHandle()
        );

        FromString(data);
    }

    inline Interval::Interval(OCI_Interval *pInterval, core::Handle *parent)
    {
        AcquireTransient(pInterval, parent);
    }

    inline void Interval::Allocate(OCI_Environment* env, IntervalTypeValues type, core::Handle* parent)
    {
        AcquireAllocated
        (
            core::Check(OCI_IntervalCreate(env, static_cast<UInt32>(type))),
            parent
        );
    }

    inline Interval Interval::Clone() const
    {
        Interval result;
        result.AcquireAllocated
        (
            core::Check(OCI_IntervalCreate(GetEnvironment(), static_cast<UInt32>(GetType()))),
            GetParent()
        );

        core::Check(OCI_IntervalAssign(result, *this));

        return result;
    }

    inline Int32 Interval::Compare(const Interval& other) const
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

    inline Int32 Interval::GetYear() const
    {
        Int32 year = 0, month = 0;

        GetYearMonth(year, month);

        return year;
    }

    inline void Interval::SetYear(Int32 value)
    {
        Int32 year = 0, month = 0;

        GetYearMonth(year, month);
        SetYearMonth(value, month);
    }

    inline Int32 Interval::GetMonth() const
    {
        Int32 year = 0, month = 0;

        GetYearMonth(year, month);

        return month;
    }

    inline void Interval::SetMonth(Int32 value)
    {
        Int32 year = 0, month = 0;

        GetYearMonth(year, month);
        SetYearMonth(year, value);
    }

    inline Int32 Interval::GetDay() const
    {
        Int32 day = 0, hour = 0, minutes = 0, seconds = 0, milliseconds = 0;

        GetDaySecond(day, hour, minutes, seconds, milliseconds);

        return day;
    }

    inline void Interval::SetDay(Int32 value)
    {
        Int32 day = 0, hour = 0, minutes = 0, seconds = 0, milliseconds = 0;

        GetDaySecond(day, hour, minutes, seconds, milliseconds);
        SetDaySecond(value, hour, minutes, seconds, milliseconds);
    }

    inline Int32 Interval::GetHours() const
    {
        Int32 day = 0, hour = 0, minutes = 0, seconds = 0, milliseconds = 0;

        GetDaySecond(day, hour, minutes, seconds, milliseconds);

        return hour;
    }

    inline void Interval::SetHours(Int32 value)
    {
        Int32 day = 0, hour = 0, minutes = 0, seconds = 0, milliseconds = 0;

        GetDaySecond(day, hour, minutes, seconds, milliseconds);
        SetDaySecond(day, value, minutes, seconds, milliseconds);
    }

    inline Int32 Interval::GetMinutes() const
    {
        Int32 day = 0, hour = 0, minutes = 0, seconds = 0, milliseconds = 0;

        GetDaySecond(day, hour, minutes, seconds, milliseconds);

        return minutes;
    }

    inline void Interval::SetMinutes(Int32 value)
    {
        Int32 day = 0, hour = 0, minutes = 0, seconds = 0, milliseconds = 0;

        GetDaySecond(day, hour, minutes, seconds, milliseconds);
        SetDaySecond(day, hour, value, seconds, milliseconds);
    }

    inline Int32 Interval::GetSeconds() const
    {
        Int32 day, hour, minutes, seconds, milliseconds;

        GetDaySecond(day, hour, minutes, seconds, milliseconds);

        return seconds;
    }

    inline void Interval::SetSeconds(Int32 value)
    {
        Int32 day = 0, hour = 0, minutes = 0, seconds = 0, milliseconds = 0;

        GetDaySecond(day, hour, minutes, seconds, milliseconds);
        SetDaySecond(day, hour, minutes, value, milliseconds);
    }

    inline Int32 Interval::GetMilliSeconds() const
    {
        Int32 day = 0, hour = 0, minutes = 0, seconds = 0, milliseconds = 0;

        GetDaySecond(day, hour, minutes, seconds, milliseconds);

        return milliseconds / 1000000;
    }

    inline void Interval::SetMilliSeconds(Int32 value)
    {
        Int32 day = 0, hour = 0, minutes = 0, seconds = 0, milliseconds = 0;

        GetDaySecond(day, hour, minutes, seconds, milliseconds);
        SetDaySecond(day, hour, minutes, seconds, value * 1000000);
    }

    inline Int32 Interval::GetNanoSeconds() const
    {
        Int32 day = 0, hour = 0, minutes = 0, seconds = 0, nanoseconds = 0;

        GetDaySecond(day, hour, minutes, seconds, nanoseconds);

        return nanoseconds;
    }

    inline void Interval::SetNanoSeconds(Int32 value)
    {
        Int32 day = 0, hour = 0, minutes = 0, seconds = 0, nanoseconds = 0;

        GetDaySecond(day, hour, minutes, seconds, nanoseconds);
        SetDaySecond(day, hour, minutes, seconds, value);
    }

    inline void Interval::GetDaySecond(Int32 &day, Int32 &hour, Int32 &min, Int32 &sec, Int32 &nanosec) const
    {
        core::Check(OCI_IntervalGetDaySecond(*this, &day, &hour, &min, &sec, &nanosec));
    }

    inline void Interval::SetDaySecond(Int32 day, Int32 hour, Int32 min, Int32 sec, Int32 nanosec)
    {
        core::Check(OCI_IntervalSetDaySecond(*this, day, hour, min, sec, nanosec));
    }

    inline void Interval::GetYearMonth(Int32 &year, Int32 &month) const
    {
        core::Check(OCI_IntervalGetYearMonth(*this, &year, &month));
    }
    inline void Interval::SetYearMonth(Int32 year, Int32 month)
    {
        core::Check(OCI_IntervalSetYearMonth(*this, year, month));
    }

    inline void Interval::UpdateTimeZone(const String& timeZone)
    {
        core::Check(OCI_IntervalFromTimeZone(*this, timeZone.c_str()));
    }

    inline void Interval::FromString(const String& data)
    {
        core::Check(OCI_IntervalFromText(*this, data.c_str()));
    }

    inline String Interval::ToString(Int32 leadingPrecision, Int32 fractionPrecision) const
    {
        if (!IsNull())
        {
            const Size size = OCI_SIZE_BUFFER;

            std::vector<otext> buffer(static_cast<Size>(size + 1));

            core::Check(OCI_IntervalToText(*this, leadingPrecision, fractionPrecision, static_cast<Int32>(size), buffer.data()));

            return core::MakeString(static_cast<const otext*>(buffer.data()));
        }

        return OCI_STRING_NULL;
    }

    inline String Interval::ToString() const
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
        const Int32 res = Compare(other);

        return (res == 0 || res < 0);
    }

    inline bool Interval::operator <= (const Interval& other) const
    {
        const Int32 res = Compare(other);

        return (res == 0 || res > 0);
    }

}
