---
title: Intervals — OCI_Interval
navtitle: Intervals
part: 2
level: intermediate
---

# Intervals — the `Interval` object

**Objectives** — by the end of this page you will be able to:

- create year-month and day-second intervals
- get/set all interval parts
- do interval arithmetic and use intervals with dates/timestamps

## In context — durations are two types, not one

Oracle splits durations into `INTERVAL YEAR TO MONTH` and `INTERVAL DAY TO
SECOND` — and they are *not* interchangeable. The reason is calendrical: a month
has no fixed length, so "3 months" and "90 days" describe different things
(Jan 31 + 3 months = Apr 30; Jan 31 + 90 days = May 1). Year-month values add
cleanly to dates under `ADD_MONTHS` semantics; day-second values behave like a
plain time span. Oracle keeps the two forms at the type level precisely so you
cannot silently mix them — `INTERVAL '3' MONTH + INTERVAL '5' DAY` is a server
error, and OCILIB's `IntervalAdd` inherits the same restriction.

That split drives OCILIB's API shape too: a single `OCI_Interval` handle whose
subtype (`OCI_INTERVAL_YM` / `OCI_INTERVAL_DS`) is fixed at creation, with
separate `GetYearMonth`/`SetYearMonth` and `GetDaySecond`/`SetDaySecond`
accessors. Calling the wrong accessor on the wrong subtype is the classic
mistake this page exists to prevent — `OCI_IntervalCheck` validates a value
*after* construction when you build intervals from untrusted input.

In OCILIB terms, intervals are the glue between the two previous pages: a
timestamp minus a timestamp *is* an interval (`OCI_TimestampSubtract` fills a DS
interval for you), and a timestamp plus an interval is the only way OCILIB
shifts time (`OCI_TimestampIntervalAdd`). There is no date-minus-date shortcut —
the round trip through the interval type is deliberate, because the result is
exactly what you bind into `INTERVAL` columns.

## The Interval object

An `Interval` is a duration — the difference you add to or subtract from dates and
timestamps. Two kinds exist, chosen at creation on the **environment**:

| C enum | C++ enum | SQL type | Parts |
|--------|----------|----------|-------|
| `OCI_INTERVAL_YM` | `Interval::YearMonth` | `INTERVAL YEAR TO MONTH` | years, months |
| `OCI_INTERVAL_DS` | `Interval::DaySecond` | `INTERVAL DAY TO SECOND` | days, hours, minutes, seconds, frac-seconds |

:::sample
```c
OCI_Interval *ym = OCI_IntervalCreate(env, OCI_INTERVAL_YM);
OCI_Interval *ds = OCI_IntervalCreate(env, OCI_INTERVAL_DS);
OCI_IntervalFree(ym);
OCI_IntervalFree(ds);
```

```cpp
Interval ym(env, Interval::YearMonth);
Interval ds(env, Interval::DaySecond);
```
:::

## Get & set parts

:::sample
```c
OCI_IntervalSetYearMonth(ym, 3, 6);                    /* +3 years 6 months      */
OCI_IntervalGetYearMonth(ym, &years, &months);

OCI_IntervalSetDaySecond(ds, 15, 4, 30, 0, 500000000); /* 15d 4:30:00.5          */
OCI_IntervalGetDaySecond(ds, &days, &hours, &mins, &secs, &fsecs);

OCI_IntervalGetType(ym);                               /* OCI_INTERVAL_YM        */
OCI_IntervalCheck(ds);                                 /* validity               */
```

```cpp
ym.SetYearMonth(3, 6);                                 /* +3 years 6 months      */
ym.GetYearMonth(years, months);

ds.SetDaySecond(15, 4, 30, 0, 500000000);              /* 15d 4:30:00.5          */
ds.GetDaySecond(days, hours, mins, secs, fsecs);

ym.GetType();                                          /* Interval::YearMonth    */
ds.Check();                                            /* validity               */
```
:::

## Arithmetic

:::sample
```c
OCI_IntervalAssign(ds2, ds);
OCI_IntervalAdd(ds, ds2);          /* ds += ds2     (same kind only) */
OCI_IntervalSubtract(ds, ds2);     /* ds -= ds2                      */
OCI_IntervalCompare(ds, ds2);      /* -1/0/1                         */

otext buf[64];
OCI_IntervalFromText(ds, OTEXT("15 04:30:00.5"));
OCI_IntervalToText (ym, 3, 0, sizeof(buf), buf);
```

```cpp
ds2.Assign(ds);
ds.Add(ds2);            /* ds += ds2     (same kind only) */
ds.Subtract(ds2);       /* ds -= ds2                      */
ds.Compare(ds2);        /* -1/0/1                         */

ds.FromText("15 04:30:00.5", "DD HH24:MI:SS.FF");
String s = ym.ToText("YYYY-MM");
```
:::

## Where intervals plug in

- `OCI_TimestampIntervalAdd/Sub` — timestamp ± interval.
- `OCI_TimestampSubtract` — produces a DS interval from two timestamps.
- `OCI_IntervalFromTimeZone(itv, OTEXT("+02:00"))` — TZ offset → interval.
- Bind them like any type: `OCI_StatementBindInterval(st, ":i", itv)` — the YM/DS
  kind travels on the handle; fetch with `OCI_ResultsetGetInterval(rs, i)`.

## Scenario — measure a duration between two timestamps

`OCI_TimestampSubtract` is how you answer "how long did it take":

:::sample
```c
OCI_Timestamp *t1 = OCI_TimestampCreate(env, OCI_TIMESTAMP);
OCI_Timestamp *t2 = OCI_TimestampCreate(env, OCI_TIMESTAMP);
OCI_Interval  *iv = OCI_IntervalCreate(env, OCI_INTERVAL_DS);

OCI_TimestampConstruct(t1, 2026, 11, 6,  9, 0,  0, 0, NULL);  /* start   */
OCI_TimestampConstruct(t2, 2026, 11, 6, 17, 45, 30, 0, NULL); /* end     */
OCI_TimestampSubtract(t2, t1, iv);          /* iv = t2 - t1              */

OCI_Int32 d, h, m, s, f;
OCI_IntervalGetDaySecond(iv, &d, &h, &m, &s, &f);
printf("elapsed: %dd %02d:%02d:%02d\n", d, h, m, s);   /* 0d 08:45:30   */
```

```cpp
Timestamp t1(env, Timestamp::NoTimeZone), t2(env, Timestamp::NoTimeZone);
Interval  iv(env, Interval::DaySecond);

t1.Construct(2026, 11, 6,  9, 0,  0, 0);   /* start                     */
t2.Construct(2026, 11, 6, 17, 45, 30, 0);  /* end                       */
t2.Subtract(t1, iv);                       /* iv = t2 - t1              */

Int32 d, h, m, s, f;
iv.GetDaySecond(d, h, m, s, f);
std::cout << "elapsed: " << d << "d " << h << ':' << m << ':' << s << '\n';
```
:::

## Scenario — shift a timestamp by an interval

SLA checks, retries, expirations — timestamp ± interval covers them:

:::sample
```c
OCI_Timestamp *deadline = OCI_TimestampCreate(env, OCI_TIMESTAMP);
OCI_TimestampSysTimestamp(deadline);

OCI_Interval *grace = OCI_IntervalCreate(env, OCI_INTERVAL_DS);
OCI_IntervalSetDaySecond(grace, 0, 1, 30, 0, 0);   /* +1h30             */
OCI_TimestampIntervalAdd(deadline, grace);         /* deadline += 1h30  */

/* and how far in the future is it now? */
OCI_Interval *remaining = OCI_IntervalCreate(env, OCI_INTERVAL_DS);
OCI_Timestamp *now = OCI_TimestampCreate(env, OCI_TIMESTAMP);
OCI_TimestampSysTimestamp(now);
OCI_TimestampSubtract(deadline, now, remaining);
```

```cpp
Timestamp deadline = Timestamp::SysTimestamp(env);

Interval grace(env, Interval::DaySecond);
grace.SetDaySecond(0, 1, 30, 0, 0);              /* +1h30              */
deadline.IntervalAdd(grace);                     /* deadline += 1h30   */

Interval remaining(env, Interval::DaySecond);
deadline.Subtract(Timestamp::SysTimestamp(env), remaining);
```
:::

## Scenario — TZ offset string → interval

`OCI_IntervalFromTimeZone` turns `"+02:00"`-style offsets into usable DS
intervals — bridging TZ names and arithmetic:

:::sample
```c
OCI_IntervalFromTimeZone(iv, OTEXT("+02:00"));
OCI_IntervalGetDaySecond(iv, &d, &h, &m, &s, &f);  /* 0d +02:00:00      */
```

```cpp
iv.FromTimeZone("+02:00");
iv.GetDaySecond(d, h, m, s, f);                  /* 0d +02:00:00      */
```
:::

## Scenario — bind & fetch

:::sample
```c
OCI_Interval *ym = OCI_IntervalCreate(env, OCI_INTERVAL_YM);
OCI_IntervalSetYearMonth(ym, 2, 6);              /* 2 years 6 months   */

OCI_StatementPrepare(st, OTEXT(
    "insert into contracts(id, duration) values(:i, :d)"));
OCI_StatementBindInt32   (st, OTEXT(":i"), &id);
OCI_StatementBindInterval(st, OTEXT(":d"), ym);
OCI_StatementExecutePrepared(st);

/* fetch back — borrowed handle */
OCI_ResultsetFetchNext(rs);
OCI_Interval *got = OCI_ResultsetGetInterval(rs, 0);
```

```cpp
Interval ym(env, Interval::YearMonth);
ym.SetYearMonth(2, 6);                           /* 2 years 6 months   */

st.Prepare("insert into contracts(id, duration) values(:i, :d)");
st.Bind(":i", id);
st.Bind(":d", ym);
st.ExecutePrepared();

/* fetch back — borrowed handle */
rs.Next();
Interval got = rs.Get<Interval>(0);
```
:::

## Putting it together — the typical sequence

Intervals are short-lived objects — build, use, discard:

1. **Decide YM or DS first** — there is no conversion between the two, and
   picking wrong is a type error, not a rounding error.
2. **Fill the parts**: `SetYearMonth` / `SetDaySecond` for programmatic values,
   `FromText` for `"3-6"`-style literals.
3. **`Check` it when the input is untrusted** — a bad part set silently gives a
   nonsense interval that only surfaces at bind time.
4. **Use it**: `TimestampIntervalAdd`/`Sub` to shift a timestamp, or bind into
   an `INTERVAL` column. To *get* an interval, `TimestampSubtract` two
   timestamps and OCILIB fills a DS interval for you.
5. **Compare like with like** — `IntervalCompare` only accepts two intervals of
   the same subtype.

Hints: negative intervals are legal everywhere OCILIB takes an interval —
`SetDaySecond(-1, ...)` subtracts days; `IntervalFromTimeZone` turns a
`"+02:00"` offset into a usable DS interval when you need to normalize zones.

## Try it yourself

1. Build `INTERVAL '2-6' YEAR TO MONTH` programmatically and add it to `sysdate`.
2. Compute the interval between two timestamps, then split it into parts.
3. Compare a `INTERVAL '36' HOUR` vs `INTERVAL '1' DAY` — they're not equal types;
   convert and check.

## Common pitfalls

- **Mixing YM and DS in arithmetic** — different types; convert first.
- **Negative intervals** — valid; check `GetDaySecond` sign handling.
- **`FF` precision in text** — fractional seconds need the format model.

## Recap

- `OCI_INTERVAL_YM` (y/m) and `OCI_INTERVAL_DS` (d/h/m/s/f) — set/get parts, add,
  subtract, compare, to/from text; the glue for timestamp arithmetic.

**Next:** `OCI_Number` — exact decimal math when `double` isn't enough.
