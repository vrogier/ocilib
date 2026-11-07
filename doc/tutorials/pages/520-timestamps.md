---
title: Timestamps — OCI_Timestamp
navtitle: Timestamps
part: 5
level: intermediate
demos: [timestamp.c, timestamp.cpp]
---

# Timestamps — the `Timestamp` object

**Objectives** — by the end of this page you will be able to:

- work with `TIMESTAMP`, `TIMESTAMP WITH TIME ZONE` and `WITH LOCAL TIME ZONE`
- read/write fractional seconds and TZ offsets
- convert between timestamp kinds and do interval arithmetic

## Meet the Timestamp object

A `Timestamp` is a `Date` upgraded: sub-second precision (fractional seconds) and,
optionally, a time zone. Like `Date`, it's a datatype handle created on the
**environment** — three subtypes pick which SQL flavor it represents:

| C enum | C++ enum | SQL type | TZ stored? |
|--------|----------|----------|-----------|
| `OCI_TIMESTAMP` | `Timestamp::NoTimeZone` | `TIMESTAMP` | no |
| `OCI_TIMESTAMP_TZ` | `Timestamp::WithTimeZone` | `TIMESTAMP WITH TIME ZONE` | yes |
| `OCI_TIMESTAMP_LTZ` | `Timestamp::WithLocalTimeZone` | `TIMESTAMP WITH LOCAL TIME ZONE` | normalized to session TZ |

The subtype is chosen at creation and must match the column it binds to — the
library tracks it (`GetType`/`GetSubType` on the object).

## Get, set, construct

:::sample
```c
OCI_Timestamp *ts = OCI_TimestampCreate(env, OCI_TIMESTAMP_TZ);
OCI_TimestampSysTimestamp(ts);                    /* now, with fraction + tz     */

OCI_TimestampSetDateTime(ts, 2026, 11, 6, 14, 30, 15, 123456789);
/*                                                     frac seconds (ns)       */
OCI_TimestampConstruct(ts, 2026, 11, 6, 14, 30, 15, 123456789,
                       OTEXT("+02:00"));            /* explicit tz name          */

OCI_TimestampGetDateTime(ts, &y, &mo, &d, &h, &mi, &s, &fsec);
OCI_TimestampGetTimeZoneName  (ts, sizeof(tzname), tzname);
OCI_TimestampGetTimeZoneOffset(ts, &tzh, &tzm);
OCI_TimestampGetDate / GetTime                     /* parts only                */
OCI_TimestampGetType                               /* which subtype             */
```

```cpp
Timestamp ts(env, Timestamp::WithTimeZone);
ts.SysTimestamp();                                /* now, with fraction + tz   */

ts.SetDateTime(2026, 11, 6, 14, 30, 15, 123456789);
ts.Construct(2026, 11, 6, 14, 30, 15, 123456789, "+02:00");

ts.GetDateTime(y, mo, d, h, mi, s, fsec);
ts.GetTimeZoneName();
ts.GetTimeZoneOffset();
ts.GetDate(); ts.GetTime();                       /* parts only                */
ts.GetType();                                     /* which subtype             */
```
:::

## Conversion & arithmetic

:::sample
```c
OCI_TimestampConvert(ts2, ts);            /* change subtype (TZ → no-TZ etc.) */
OCI_TimestampAssign(ts2, ts);             /* copy                             */
OCI_TimestampCompare(ts, ts2);            /* -1/0/1                           */

OCI_TimestampIntervalAdd(ts, itv);        /* ts += OCI_Interval               */
OCI_TimestampIntervalSub(ts, itv);
OCI_TimestampSubtract(ts, ts2, itv);      /* itv = ts - ts2 (day-second)      */

OCI_TimestampFromText(ts, OTEXT("2026-11-06 14:30:15.123 +02:00"),
                      OTEXT("YYYY-MM-DD HH24:MI:SS.FF TZH:TZM"));
time_t t = OCI_TimestampToCTime(ts);
OCI_TimestampFromCTime(ts, t);
```

```cpp
ts2.Convert(ts);                          /* change subtype (TZ → no-TZ etc.) */
ts2.Assign(ts);                           /* copy                             */
ts.Compare(ts2);                          /* -1/0/1                           */

ts.IntervalAdd(itv);                      /* ts += Interval                   */
ts.IntervalSub(itv);
ts.Subtract(ts2, itv);                    /* itv = ts - ts2 (day-second)      */

ts.FromText("2026-11-06 14:30:15.123 +02:00", "YYYY-MM-DD HH24:MI:SS.FF TZH:TZM");
time_t t = ts.ToCTime();
ts.FromCTime(t);
```
:::

## Binding & fetching

A fetched `TIMESTAMP` column gives a borrowed `Timestamp` (`OCI_GetTimestamp` /
`Get<Timestamp>`); the C bind carries the subtype arg matching the column kind
(`OCI_BindTimestamp(st, ":ts", ts, OCI_TIMESTAMP_TZ)`) — in C++ the object's own
type supplies it (`Timestamp ts(env, Timestamp::WithTimeZone)` + `Bind`).

## Try it yourself

1. Insert a `TIMESTAMP WITH TIME ZONE` built with `Construct` + `"+02:00"` and
   fetch it back — compare the offset.
2. Compute `ts2 - ts1` into a day-second interval and print it.
3. Convert a TZ timestamp to `OCI_TIMESTAMP` and see the TZ info drop.

## Common pitfalls

- **Binding a TZ timestamp into a plain TIMESTAMP column** — the offset is dropped;
  match subtypes.
- **Assuming LTZ keeps your tz** — it's normalized to the *session* timezone.
- **`FF` format missing** — fractional seconds silently truncated in `ToText`.

## Recap

- Three subtypes; `Construct`/`Get*` handle parts+TZ; `Convert` moves between kinds;
  interval arithmetic via `TimestampInterval*`/`Subtract`.

**Next:** `OCI_Interval` — the durations you add to dates and timestamps.
