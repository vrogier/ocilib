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

namespace ocilib
{

inline Date::Date(bool create)
{
    if (create)
    {
        Allocate();
    }
}

inline Date::Date(const ostring& str, const ostring& format)
{
    Allocate();

    FromString(str, format);
}

inline Date::Date(const otext* str, const otext* format)
{
    Allocate();

    FromString(str, format);
}

inline Date::Date(OCI_Date *pDate, core::Handle *parent)
{
    AcquireTransient(pDate, parent);
}

inline void Date::Allocate()
{
    AcquireAllocated
    (
        core::Check(OCI_DateCreate(nullptr)),
        Environment::GetEnvironmentHandle()
    );
}

inline Date Date::SysDate()
{
    Date result;

    result.Allocate();

    core::Check(OCI_DateSysDate(result));

    return result;
}

inline Date Date::Clone() const
{
    Date result;

    result.Allocate();

    core::Check(OCI_DateAssign(result, *this));

    return result;
}

inline int Date::Compare(const Date& other) const
{
    return core::Check(OCI_DateCompare(*this, other));
}

inline bool Date::IsValid() const
{
    return (core::Check(OCI_DateCheck(*this)) == 0);
}

inline int Date::GetYear() const
{
    int year = 0, month = 0, day = 0;

    GetDate(year, month, day);

    return year;
}

inline void Date::SetYear(int value)
{
    int year = 0, month = 0, day = 0;

    GetDate(year, month, day);
    SetDate(value, month, day);
}

inline int Date::GetMonth() const
{
    int year = 0, month = 0, day = 0;

    GetDate(year, month, day);

    return month;
}

inline void Date::SetMonth(int value)
{
    int year = 0, month = 0, day = 0;

    GetDate(year, month, day);
    SetDate(year, value, day);
}

inline int Date::GetDay() const
{
    int year = 0, month = 0, day = 0;

    GetDate(year, month, day);

    return day;
}

inline void Date::SetDay(int value)
{
    int year = 0, month = 0, day = 0;

    GetDate(year, month, day);
    SetDate(year, month, value);
}

inline int Date::GetHours() const
{
    int hour = 0, minutes = 0, seconds = 0;

    GetTime(hour, minutes, seconds);

    return hour;
}

inline void Date::SetHours(int value)
{
    int hour = 0, minutes = 0, seconds = 0;

    GetTime(hour, minutes, seconds);
    SetTime(value, minutes, seconds);
}

inline int Date::GetMinutes() const
{
    int hour = 0, minutes = 0, seconds = 0;

    GetTime(hour, minutes, seconds);

    return minutes;
}

inline void Date::SetMinutes(int value)
{
    int hour = 0, minutes = 0, seconds = 0;

    GetTime(hour, minutes, seconds);
    SetTime(hour, value, seconds);
}

inline int Date::GetSeconds() const
{
    int hour = 0, minutes = 0, seconds = 0;

    GetTime(hour, minutes, seconds);

    return seconds;
}

inline void Date::SetSeconds(int value)
{
    int hour = 0, minutes = 0, seconds = 0;

    GetTime(hour, minutes, seconds);
    SetTime(hour, minutes, value);
}

inline int Date::DaysBetween(const Date& other) const
{
    return core::Check(OCI_DateDaysBetween(*this, other));
}

inline void Date::SetDate(int year, int month, int day)
{
    core::Check(OCI_DateSetDate(*this, year, month, day));
}

inline void Date::SetTime(int hour, int min, int sec)
{
    core::Check(OCI_DateSetTime(*this, hour, min , sec));
}

inline void Date::SetDateTime(int year, int month, int day, int hour, int min, int sec)
{
    core::Check(OCI_DateSetDateTime(*this, year, month, day, hour, min , sec));
}

inline void Date::GetDate(int &year, int &month, int &day) const
{
    core::Check(OCI_DateGetDate(*this, &year, &month, &day));
}

inline void Date::GetTime(int &hour, int &min, int &sec) const
{
    core::Check(OCI_DateGetTime(*this, &hour, &min , &sec));
}

inline void Date::GetDateTime(int &year, int &month, int &day, int &hour, int &min, int &sec) const
{
    core::Check(OCI_DateGetDateTime(*this, &year, &month, &day, &hour, &min , &sec));
}

inline void Date::AddDays(int days)
{
    core::Check(OCI_DateAddDays(*this, days));
}

inline void Date::AddMonths(int months)
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
    core::Check(OCI_DateFromText(*this, str.c_str(), format.empty() ?  Environment::GetFormat(FormatDate).c_str() : format.c_str()));
}

inline ostring Date::ToString(const ostring& format) const
{
    if (!IsNull())
    {
        const size_t size = OCI_SIZE_BUFFER;

        core::ManagedBuffer<otext> buffer(static_cast<size_t>(size + 1));

        core::Check(OCI_DateToText(*this, format.c_str(), static_cast<int>(size), buffer));

        return core::MakeString(static_cast<const otext *>(buffer));
    }

    return OCI_STRING_NULL;
}

inline ostring Date::ToString() const
{
    return ToString(Environment::GetFormat(FormatDate));
}

inline Date& Date::operator ++ ()
{
    return *this += 1;
}

inline Date Date::operator ++ (int)
{
    Date result = Clone();

    *this += 1;

    return result;
}

inline Date& Date::operator -- ()
{
    return *this -= 1;
}

inline Date Date::operator -- (int)
{
    Date result = Clone();

    *this -= 1;

    return result;
}

inline Date Date::operator + (int value) const
{
    Date result = Clone();
    return result += value;
}

inline Date Date::operator - (int value) const
{
    Date result = Clone();
    return result -= value;
}

inline Date& Date::operator += (int value)
{
    AddDays(value);
    return *this;
}

inline Date& Date::operator -= (int value)
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
    const int res = Compare(other);

    return res == 0 || res > 0;
}

inline bool Date::operator <= (const Date& other) const
{
    const int res = Compare(other);

    return res == 0 || res < 0;
}

}
