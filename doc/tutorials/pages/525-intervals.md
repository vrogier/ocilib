---
title: Intervals — OCI_Interval
navtitle: Intervals
part: 5
level: intermediate
---

# Intervals — the `Interval` object

**Objectives** — by the end of this page you will be able to:

- create year-month and day-second intervals
- get/set all interval parts
- do interval arithmetic and use intervals with dates/timestamps

## Meet the Interval object

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
OCI_IntervalFree(ym); OCI_IntervalFree(ds);
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
OCI_IntervalFromText(ds, OTEXT("15 04:30:00.5"), OTEXT("DD HH24:MI:SS.FF"));
OCI_IntervalToText (ym, OTEXT("YYYY-MM"), sizeof(buf), buf);
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
- Bind them like any type: `OCI_BindInterval(st, ":i", itv, OCI_INTERVAL_DS)`;
  fetch with `OCI_GetInterval(rs, i)`.

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
