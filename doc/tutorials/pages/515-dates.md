---
title: Dates — the Date object
navtitle: Dates
part: 5
level: beginner
demos: [date.c, date.cpp]
---

# Dates — the `Date` object

**Objectives** — by the end of this page you will be able to:

- create, read and write date values
- do date arithmetic: add days/months, diff, next/last day
- convert to/from text and C `time_t`

## Meet the Date object

A `Date` holds one SQL `DATE` value — day precision plus hours/minutes/seconds, no time zone.

It's the first of OCILIB's *datatype objects*: unlike scalar getters, it's a handle you create on the **environment**, fill, bind into statements, and free — or, in C++, a RAII object.

Borrowed-vs-owned matters everywhere here: a `Date` you fetch from a row is borrowed
by the row; a `Date` you `Create` is yours to fill and free.

:::sample
```c
OCI_Date *d = OCI_DateCreate(env);              /* tied to the environment   */
/* … use … */
OCI_DateFree(d);

OCI_Date **arr = OCI_DateArrayCreate(env, 10);  /* arrays for bulk work      */
OCI_DateArrayFree(env, arr);
```

```cpp
Date d(env);                                    /* RAII — no free            */
```
:::

## Get & set

| C function | C++ method | Role |
|------------|------------|------|
| `OCI_DateSysDate` | `SysDate()` | set to server "now" |
| `OCI_DateSetDate` | `SetDate(y,m,d)` | set y/m/d |
| `OCI_DateSetTime` | `SetTime(h,mi,s)` | set h/m/s |
| `OCI_DateSetDateTime` | `SetDateTime(...)` | set all parts |
| `OCI_DateGetDate` | `GetDate(&y,&m,&d)` | read y/m/d |
| `OCI_DateGetTime` | `GetTime(&h,&mi,&s)` | read h/m/s |
| `OCI_DateGetDateTime` | `GetDateTime(...)` | read all parts |
| `OCI_DateCheck` | `Check()` | sanity-check the parts |

## Arithmetic & comparison

| C function | C++ method | Role |
|------------|------------|------|
| `OCI_DateAssign` | `Assign` / `operator=` | `d2 = d` |
| `OCI_DateAddDays` | `AddDays(n)` | add days |
| `OCI_DateAddMonths` | `AddMonths(n)` | add months |
| `OCI_DateCompare` | `Compare` | `<0 / 0 / >0` |
| `OCI_DateDaysBetween` | `DaysBetween` | signed day delta |
| `OCI_DateNextDay` | `NextDay(day)` | next weekday |
| `OCI_DateLastDay` | `LastDay()` | last day of month |
| `OCI_DateZoneToZone` | `ZoneToZone` | move between time zones |

## Text & C time conversion

| C function | C++ method | Role |
|------------|------------|------|
| `OCI_DateFromText` | `FromText` | parse with a format model |
| `OCI_DateToText` | `ToText` | format with a model |
| `OCI_DateToCTime` | `ToCTime()` | → `time_t` |
| `OCI_DateFromCTime` | `FromCTime` | ← `time_t` |

## Binding & fetching

A fetched `DATE` column gives a borrowed `Date` (C `OCI_GetDate` / C++ `Get<Date>`);
bind one with the date bind (`OCI_BindDate` / `Bind(":d", d)`) — plug into the Part 2
patterns.

## Try it yourself

1. Compute each employee's tenure: days-between(today, hiredate)/365.
2. Insert a row with `hiredate` parsed from text.
3. Print next Friday's date from today.

## Common pitfalls

- **Freeing a fetched date** — fetched handles are borrowed; only created ones are
  yours.
- **Expecting time zones** — `DATE` has none; need TZ → `Timestamp` (next page).
- **Wrong format model** — `YYYY-MM-DD` vs `DD-MON-YYYY` mismatches = garbage dates.

## Recap

- The `Date` object: create/get/set/add/compare/convert — the full SQL DATE toolbox.
- Borrowed on fetch, owned on create, bound by pointer.

**Next:** `Timestamp` — sub-second precision and time zones.
