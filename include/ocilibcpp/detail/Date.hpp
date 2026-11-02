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

inline Date::Date()
{
}

inline Date::Date(const Environment& env, bool create)
{
    if (create)
    {
        Allocate(env.GetHandle());
    }
}

inline Date::Date(const Date& other)
{
    *this = other;
}

inline Date& Date::operator= (const Date& other) noexcept
{
    if (this != &other)
    {
        if (IsNull())
        {
            Allocate(other.GetEnvironment());
        }

        core::Check(OCI_DateAssign(*this, other));
    }

    return *this;
}

inline Date::Date(const Environment& env, const ostring& str, const ostring& format)
{
    Allocate(env.GetHandle());

    FromString(str, format);
}

inline Date::Date(const Environment& env, const otext* str, const otext* format)
{
    Allocate(env.GetHandle());

    FromString(ostring(str), ostring(format ? format : OTEXT("")));
}

inline Date::Date(OCI_Date *pDate, core::Handle *parent)
{
    AcquireTransient(pDate, parent);
}

inline void Date::Allocate(OCI_Environment* env)
{
    AcquireAllocated
    (
        core::Check(OCI_DateCreate(env)),
        static_cast<core::Handle*>(nullptr)
    );
}

inline Date Date::SysDate(const Environment& env)
{
    Date result;

    result.Allocate(env.GetHandle());

    core::Check(OCI_DateSysDate(result));

    return result;
}

inline Date Date::Clone() const
{
    Date result;
    result.AcquireAllocated
    (
        core::Check(OCI_DateCreate(GetEnvironment())),
        static_cast<core::Handle*>(nullptr)
    );

    core::Check(OCI_DateAssign(result, *this));

    return result;
}

inline OCI_Int32 Date::Compare(const Date& other) const
{
    return core::Check(OCI_DateCompare(*this, other));
}

inline bool Date::IsValid() const
{
    return (core::Check(OCI_DateCheck(*this)) == 0);
}

inline OCI_Int32 Date::GetYear() const
{
    OCI_Int32 year = 0, month = 0, day = 0;

    GetDate(year, month, day);

    return year;
}

inline void Date::SetYear(OCI_Int32 value)
{
    OCI_Int32 year = 0, month = 0, day = 0;

    GetDate(year, month, day);
    SetDate(value, month, day);
}

inline OCI_Int32 Date::GetMonth() const
{
    OCI_Int32 year = 0, month = 0, day = 0;

    GetDate(year, month, day);

    return month;
}

inline void Date::SetMonth(OCI_Int32 value)
{
    OCI_Int32 year = 0, month = 0, day = 0;

    GetDate(year, month, day);
    SetDate(year, value, day);
}

inline OCI_Int32 Date::GetDay() const
{
    OCI_Int32 year = 0, month = 0, day = 0;

    GetDate(year, month, day);

    return day;
}

inline void Date::SetDay(OCI_Int32 value)
{
    OCI_Int32 year = 0, month = 0, day = 0;

    GetDate(year, month, day);
    SetDate(year, month, value);
}

inline OCI_Int32 Date::GetHours() const
{
    OCI_Int32 hour = 0, minutes = 0, seconds = 0;

    GetTime(hour, minutes, seconds);

    return hour;
}

inline void Date::SetHours(OCI_Int32 value)
{
    OCI_Int32 hour = 0, minutes = 0, seconds = 0;

    GetTime(hour, minutes, seconds);
    SetTime(value, minutes, seconds);
}

inline OCI_Int32 Date::GetMinutes() const
{
    OCI_Int32 hour = 0, minutes = 0, seconds = 0;

    GetTime(hour, minutes, seconds);

    return minutes;
}

inline void Date::SetMinutes(OCI_Int32 value)
{
    OCI_Int32 hour = 0, minutes = 0, seconds = 0;

    GetTime(hour, minutes, seconds);
    SetTime(hour, value, seconds);
}

inline OCI_Int32 Date::GetSeconds() const
{
    OCI_Int32 hour = 0, minutes = 0, seconds = 0;

    GetTime(hour, minutes, seconds);

    return seconds;
}

inline void Date::SetSeconds(OCI_Int32 value)
{
    OCI_Int32 hour = 0, minutes = 0, seconds = 0;

    GetTime(hour, minutes, seconds);
    SetTime(hour, minutes, value);
}

inline OCI_Int32 Date::DaysBetween(const Date& other) const
{
    return core::Check(OCI_DateDaysBetween(*this, other));
}

inline void Date::SetDate(OCI_Int32 year, OCI_Int32 month, OCI_Int32 day)
{
    core::Check(OCI_DateSetDate(*this, year, month, day));
}

inline void Date::SetTime(OCI_Int32 hour, OCI_Int32 min, OCI_Int32 sec)
{
    core::Check(OCI_DateSetTime(*this, hour, min , sec));
}

inline void Date::SetDateTime(OCI_Int32 year, OCI_Int32 month, OCI_Int32 day, OCI_Int32 hour, OCI_Int32 min, OCI_Int32 sec)
{
    core::Check(OCI_DateSetDateTime(*this, year, month, day, hour, min , sec));
}

inline void Date::GetDate(OCI_Int32 &year, OCI_Int32 &month, OCI_Int32 &day) const
{
    core::Check(OCI_DateGetDate(*this, &year, &month, &day));
}

inline void Date::GetTime(OCI_Int32 &hour, OCI_Int32 &min, OCI_Int32 &sec) const
{
    core::Check(OCI_DateGetTime(*this, &hour, &min , &sec));
}

inline void Date::GetDateTime(OCI_Int32 &year, OCI_Int32 &month, OCI_Int32 &day, OCI_Int32 &hour, OCI_Int32 &min, OCI_Int32 &sec) const
{
    core::Check(OCI_DateGetDateTime(*this, &year, &month, &day, &hour, &min , &sec));
}

inline void Date::AddDays(OCI_Int32 days)
{
    core::Check(OCI_DateAddDays(*this, days));
}

inline void Date::AddMonths(OCI_Int32 months)
{
    OCI_DateAddMonths(*this, months);
}

inline Date Date::NextDay(const ostring& day) const
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

inline void Date::ChangeTimeZone(const ostring& tzSrc, const ostring& tzDst)
{
    core::Check(OCI_DateZoneToZone(*this, tzSrc.c_str(), tzDst.c_str()));
}

inline void Date::FromString(const ostring& str, const ostring& format)
{
    core::Check(OCI_DateFromText(*this, str.c_str(), format.empty() ?  core::MakeString(core::Check(OCI_GetFormat(GetEnvironment(), FormatDate))).c_str() : format.c_str()));
}

inline ostring Date::ToString(const ostring& format) const
{
    if (!IsNull())
    {
        const OCI_Size size = OCI_SIZE_BUFFER;

        std::vector<otext> buffer(static_cast<OCI_Size>(size + 1));

        core::Check(OCI_DateToText(*this, format.c_str(), static_cast<OCI_Int32>(size), buffer.data()));

        return core::MakeString(static_cast<const otext*>(buffer.data()));
    }

    return OCI_STRING_NULL;
}

inline ostring Date::ToString() const
{
    if (IsNull())
    {
        return OCI_STRING_NULL;
    }

    return ToString(core::MakeString(core::Check(OCI_GetFormat(GetEnvironment(), FormatDate))));
}

inline Date& Date::operator ++ ()
{
    return *this += 1;
}

inline Date Date::operator ++ (OCI_Int32)
{
    Date result = Clone();

    *this += 1;

    return result;
}

inline Date& Date::operator -- ()
{
    return *this -= 1;
}

inline Date Date::operator -- (OCI_Int32)
{
    Date result = Clone();

    *this -= 1;

    return result;
}

inline Date Date::operator + (OCI_Int32 value) const
{
    Date result = Clone();
    return result += value;
}

inline Date Date::operator - (OCI_Int32 value) const
{
    Date result = Clone();
    return result -= value;
}

inline Date& Date::operator += (OCI_Int32 value)
{
    AddDays(value);
    return *this;
}

inline Date& Date::operator -= (OCI_Int32 value)
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
    const OCI_Int32 res = Compare(other);

    return res == 0 || res > 0;
}

inline bool Date::operator <= (const Date& other) const
{
    const OCI_Int32 res = Compare(other);

    return res == 0 || res < 0;
}

}
