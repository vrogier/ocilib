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

    inline Date::Date()
    {
    }

    inline Date::Date(const Environment& env, bool create)
    {
        if (create)
        {
            Allocate(env.GetHandle(), env.GetEnvironmentHandle());
        }
    }

    inline Date::Date(const Date& other)
    {
        *this = other;
    }

    inline Date& Date::operator= (const Date& other)
    {
        if (this != &other)
        {
            if (IsNull())
            {
                Allocate(other.GetEnvironment(), other.GetParent());
            }

            core::Check(OCI_DateAssign(*this, other));
        }

        return *this;
    }

    inline Date::Date(const Environment& env, const String& str, const String& format)
    {
        Allocate(env.GetHandle(), env.GetEnvironmentHandle());

        FromString(str, format);
    }

    inline Date::Date(const Environment& env, const otext* str, const otext* format)
    {
        Allocate(env.GetHandle(), env.GetEnvironmentHandle());

        FromString(String(str), String(format ? format : OTEXT("")));
    }

    inline Date::Date(OCI_Date *pDate, core::Handle *parent)
    {
        AcquireTransient(pDate, parent);
    }

    inline void Date::Allocate(OCI_Environment* env, core::Handle* parent)
    {
        AcquireAllocated
        (
            core::Check(OCI_DateCreate(env)),
            parent
        );
    }

    inline Date Date::SysDate(const Environment& env)
    {
        Date result;

        result.Allocate(env.GetHandle(), env.GetEnvironmentHandle());

        core::Check(OCI_DateSysDate(result));

        return result;
    }

    inline Date Date::Clone() const
    {
        Date result;
        result.AcquireAllocated
        (
            core::Check(OCI_DateCreate(GetEnvironment())),
            GetParent()
        );

        core::Check(OCI_DateAssign(result, *this));

        return result;
    }

    inline Int32 Date::Compare(const Date& other) const
    {
        return core::Check(OCI_DateCompare(*this, other));
    }

    inline bool Date::IsValid() const
    {
        return (core::Check(OCI_DateCheck(*this)) == 0);
    }

    inline Int32 Date::GetYear() const
    {
        Int32 year = 0, month = 0, day = 0;

        GetDate(year, month, day);

        return year;
    }

    inline void Date::SetYear(Int32 value)
    {
        Int32 year = 0, month = 0, day = 0;

        GetDate(year, month, day);
        SetDate(value, month, day);
    }

    inline Int32 Date::GetMonth() const
    {
        Int32 year = 0, month = 0, day = 0;

        GetDate(year, month, day);

        return month;
    }

    inline void Date::SetMonth(Int32 value)
    {
        Int32 year = 0, month = 0, day = 0;

        GetDate(year, month, day);
        SetDate(year, value, day);
    }

    inline Int32 Date::GetDay() const
    {
        Int32 year = 0, month = 0, day = 0;

        GetDate(year, month, day);

        return day;
    }

    inline void Date::SetDay(Int32 value)
    {
        Int32 year = 0, month = 0, day = 0;

        GetDate(year, month, day);
        SetDate(year, month, value);
    }

    inline Int32 Date::GetHours() const
    {
        Int32 hour = 0, minutes = 0, seconds = 0;

        GetTime(hour, minutes, seconds);

        return hour;
    }

    inline void Date::SetHours(Int32 value)
    {
        Int32 hour = 0, minutes = 0, seconds = 0;

        GetTime(hour, minutes, seconds);
        SetTime(value, minutes, seconds);
    }

    inline Int32 Date::GetMinutes() const
    {
        Int32 hour = 0, minutes = 0, seconds = 0;

        GetTime(hour, minutes, seconds);

        return minutes;
    }

    inline void Date::SetMinutes(Int32 value)
    {
        Int32 hour = 0, minutes = 0, seconds = 0;

        GetTime(hour, minutes, seconds);
        SetTime(hour, value, seconds);
    }

    inline Int32 Date::GetSeconds() const
    {
        Int32 hour = 0, minutes = 0, seconds = 0;

        GetTime(hour, minutes, seconds);

        return seconds;
    }

    inline void Date::SetSeconds(Int32 value)
    {
        Int32 hour = 0, minutes = 0, seconds = 0;

        GetTime(hour, minutes, seconds);
        SetTime(hour, minutes, value);
    }

    inline Int32 Date::DaysBetween(const Date& other) const
    {
        return core::Check(OCI_DateDaysBetween(*this, other));
    }

    inline void Date::SetDate(Int32 year, Int32 month, Int32 day)
    {
        core::Check(OCI_DateSetDate(*this, year, month, day));
    }

    inline void Date::SetTime(Int32 hour, Int32 min, Int32 sec)
    {
        core::Check(OCI_DateSetTime(*this, hour, min, sec));
    }

    inline void Date::SetDateTime(Int32 year, Int32 month, Int32 day, Int32 hour, Int32 min, Int32 sec)
    {
        core::Check(OCI_DateSetDateTime(*this, year, month, day, hour, min, sec));
    }

    inline void Date::GetDate(Int32 &year, Int32 &month, Int32 &day) const
    {
        core::Check(OCI_DateGetDate(*this, &year, &month, &day));
    }

    inline void Date::GetTime(Int32 &hour, Int32 &min, Int32 &sec) const
    {
        core::Check(OCI_DateGetTime(*this, &hour, &min, &sec));
    }

    inline void Date::GetDateTime(Int32 &year, Int32 &month, Int32 &day, Int32 &hour, Int32 &min, Int32 &sec) const
    {
        core::Check(OCI_DateGetDateTime(*this, &year, &month, &day, &hour, &min, &sec));
    }

    inline void Date::AddDays(Int32 days)
    {
        core::Check(OCI_DateAddDays(*this, days));
    }

    inline void Date::AddMonths(Int32 months)
    {
        OCI_DateAddMonths(*this, months);
    }

    inline Date Date::NextDay(const String& day) const
    {
        Date result = Clone();

        core::Check(OCI_DateNextDay(result, day.c_str()));

        return result;
    }

    inline Date Date::LastDay() const
    {
        Date result = Clone();

        core::Check(OCI_DateLastDay(result));

        return result;
    }

    inline void Date::ChangeTimeZone(const String& tzSrc, const String& tzDst)
    {
        core::Check(OCI_DateZoneToZone(*this, tzSrc.c_str(), tzDst.c_str()));
    }

    inline void Date::FromString(const String& str, const String& format)
    {
        core::Check(OCI_DateFromText(*this, str.c_str(),
                                     format.empty() ?  core::MakeString(core::Check(OCI_EnvironmentGetFormat(GetEnvironment(),
                                                                                                             FormatDate))).c_str() : format.c_str()));
    }

    inline String Date::ToString(const String& format) const
    {
        if (!IsNull())
        {
            const Size size = OCI_SIZE_BUFFER;

            std::vector<otext> buffer(static_cast<Size>(size + 1));

            core::Check(OCI_DateToText(*this, format.c_str(), static_cast<Int32>(size), buffer.data()));

            return core::MakeString(static_cast<const otext*>(buffer.data()));
        }

        return OCI_STRING_NULL;
    }

    inline String Date::ToString() const
    {
        if (IsNull())
        {
            return OCI_STRING_NULL;
        }

        return ToString(core::MakeString(core::Check(OCI_EnvironmentGetFormat(GetEnvironment(), FormatDate))));
    }

    inline Date& Date::operator ++ ()
    {
        return *this += 1;
    }

    inline Date Date::operator ++ (Int32)
    {
        Date result = Clone();

        *this += 1;

        return result;
    }

    inline Date& Date::operator -- ()
    {
        return *this -= 1;
    }

    inline Date Date::operator -- (Int32)
    {
        Date result = Clone();

        *this -= 1;

        return result;
    }

    inline Date Date::operator + (Int32 value) const
    {
        Date result = Clone();
        return result += value;
    }

    inline Date Date::operator - (Int32 value) const
    {
        Date result = Clone();
        return result -= value;
    }

    inline Date& Date::operator += (Int32 value)
    {
        AddDays(value);
        return *this;
    }

    inline Date& Date::operator -= (Int32 value)
    {
        AddDays(-value);
        return *this;
    }

    inline bool Date::operator == (const Date& other) const
    {
        return Compare(other) == 0;
    }

    inline bool Date::operator != (const Date& other) const
    {
        return !(*this == other);
    }

    inline bool Date::operator > (const Date& other) const
    {
        return Compare(other) > 0;
    }

    inline bool Date::operator < (const Date& other) const
    {
        return Compare(other) < 0;
    }

    inline bool Date::operator >= (const Date& other) const
    {
        const Int32 res = Compare(other);

        return res == 0 || res > 0;
    }

    inline bool Date::operator <= (const Date& other) const
    {
        const Int32 res = Compare(other);

        return res == 0 || res < 0;
    }

}
