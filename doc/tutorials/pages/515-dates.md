---
title: Dates — the Date object
navtitle: Dates
part: 2
level: beginner
demos: [date.c, date.cpp]
---

# Dates — the `Date` object

**Objectives** — by the end of this page you will be able to:

- create, read and write date values
- do date arithmetic: add days/months, diff, next/last day
- convert to/from text and C `time_t`

## In context — what an Oracle DATE really is

Oracle `DATE` is a 7-byte packed value — century, year, month, day, hour, minute,
second — a format unchanged since the earliest Oracle versions. Two consequences
follow directly from that storage, and both surprise newcomers: a DATE *always*
carries a time component (there is no "date-only" column — the h/m/s bytes are
always written, usually `00:00:00`), and a DATE holds *no timezone* — it is a
naive local instant and the server cannot tell you where it was entered. If your
data is timezone-aware, `TIMESTAMP WITH TIME ZONE` is the right column.

In SQL, date arithmetic is numeric: `hiredate + 1` adds one day, `d2 - d1`
returns a `NUMBER` of days — possibly fractional. OCILIB mirrors that semantic
with explicit calls rather than operators: `AddDays` takes negative and
fractional values, `AddMonths` follows Oracle's `ADD_MONTHS` rule (January 31
plus one month lands on the *last* day of February, not on an invalid Feb 31),
and `DaysBetween`/`Compare` give you the delta and the ordering.

On the OCILIB side, `OCI_Date` is an OCI descriptor allocated against the
**environment**, not the connection — which is why `OCI_DateCreate` takes `env`.
You can build dates before connecting, stockpile them, and bind them later; the
rule is the one this page keeps repeating: a date you created is freed by you, a
date you fetched is borrowed from the row and dies with it. Every call returns
`OCI_Boolean` (check it or consult `OCI_GetLastError`) — in C++, `ocilib::Date`
is RAII and failures throw `ocilib::Exception`.

## The Date object

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
OCI_DateArrayFree(arr);
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

## Scenario — set, shift and inspect a date

:::sample
```c
OCI_Date *d = OCI_DateCreate(env);
OCI_DateSysDate(d);                       /* server "now"               */
OCI_DateAddDays(d, -30);                  /* 30 days ago                */
OCI_DateAddMonths(d, 1);                  /* then +1 month              */

OCI_Int32 y, m, day, h, mi, s;
OCI_DateGetDateTime(d, &y, &m, &day, &h, &mi, &s);
printf("%04d-%02d-%02d %02d:%02d:%02d\n", y, m, day, h, mi, s);

if (OCI_DateCheck(d) == 0)                /* 0 = valid                  */
    puts("valid date");
```

```cpp
Date d = Date::SysDate(env);              /* server "now"               */
d.AddDays(-30);                           /* 30 days ago                */
d.AddMonths(1);                           /* then +1 month              */

Int32 y, m, day, h, mi, s;
d.GetDateTime(y, m, day, h, mi, s);
std::cout << y << '-' << m << '-' << day << '\n';

if (d.Check())
    std::cout << "valid date\n";
```
:::

## Scenario — tenure and calendar maths

:::sample
```c
OCI_Date *today = OCI_DateCreate(env);
OCI_Date *hire  = OCI_DateCreate(env);
OCI_DateSysDate(today);
OCI_DateFromText(hire, OTEXT("2015-03-17"), OTEXT("YYYY-MM-DD"));

printf("tenure: %d days\n", OCI_DateDaysBetween(today, hire));
printf("cmp   : %d\n",     OCI_DateCompare(today, hire));   /* >0 */

OCI_DateAssign(hire, today);              /* copy                       */
OCI_DateNextDay(hire, OTEXT("FRIDAY"));   /* next Friday                */
OCI_DateLastDay(hire);                    /* month end                  */

OCI_DateFree(hire);
OCI_DateFree(today);
```

```cpp
Date today = Date::SysDate(env);
Date hire(env);
hire.FromText("2015-03-17", "YYYY-MM-DD");

std::cout << "tenure: " << today.DaysBetween(hire) << " days\n";
std::cout << "cmp   : " << today.Compare(hire)    << '\n';  /* >0 */

hire.Assign(today);                       /* copy                       */
hire = hire.NextDay("FRIDAY");            /* next Friday                */
hire.LastDay();                           /* month end                  */
```
:::

## Scenario — text and `time_t` round-trips

:::sample
```c
otext buf[64];
struct tm tmv;
time_t    t;

OCI_DateFromText(d, OTEXT("2026-11-06 14:30"), OTEXT("YYYY-MM-DD HH24:MI"));
OCI_DateToText  (d, OTEXT("DD-MON-YYYY"), sizeof(buf), buf);

OCI_DateToCTime (d, &tmv, &t);            /* out: broken-down + epoch   */
OCI_DateFromCTime(d, &tmv, t);            /* in: both forms             */
```

```cpp
d.FromText("2026-11-06 14:30", "YYYY-MM-DD HH24:MI");
String s = d.ToText("DD-MON-YYYY");

time_t t = d.ToCTime();                   /* → epoch                    */
d.FromCTime(t);                           /* ← epoch                    */
```
:::

## Binding & fetching

A fetched `DATE` column gives a borrowed `Date` (C `OCI_ResultsetGetDate` / C++ `Get<Date>`);
bind one with the date bind (`OCI_StatementBindDate` / `Bind(":d", d)`) — plug into the Part 3
patterns.

:::sample
```c
OCI_StatementPrepare(st, OTEXT("update employees set hiredate = :d where empno = :e"));
OCI_StatementBindDate (st, OTEXT(":d"), d);
OCI_StatementBindInt32(st, OTEXT(":e"), &empno);
OCI_StatementExecutePrepared(st);
```

```cpp
st.Prepare("update employees set hiredate = :d where empno = :e");
st.Bind(":d", d);
st.Bind(":e", empno);
st.ExecutePrepared();
```
:::

## Putting it together — the typical sequence

Most `Date` code follows the same five steps, in this order:

1. **Create the handle on the environment** — `OCI_DateCreate(env)` /
   `Date d(env)`. No connection needed; do it before or after you connect.
2. **Fill it one of four ways**: `SysDate` for "now", `SetDateTime` for explicit
   parts, `FromText` to parse a string, `FromCTime` to import a `time_t`.
3. **Do the arithmetic**: `Compare` for ordering, `DaysBetween` for the delta,
   `AddDays`/`AddMonths` to shift, `NextDay`/`LastDay` for calendar questions.
4. **Bind it into a statement, or fetch it** — the fetched one is borrowed from
   the row; never free it.
5. **Render it with `ToText`** and a format model when you need a string —
   never rely on a default date format.

Hints: check the return of `FromText` — a wrong format model silently produces
garbage dates; `NextDay` takes the *day name* in the session language; for bulk
work the `OCI_DateArray*` family allocates a whole array of dates in one call.

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
