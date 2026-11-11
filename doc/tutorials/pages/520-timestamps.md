---
title: Timestamps — OCI_Timestamp
navtitle: Timestamps
part: 2
level: intermediate
demos: [timestamp.c, timestamp.cpp]
---

# Timestamps — the `Timestamp` object

**Objectives** — by the end of this page you will be able to:

- work with `TIMESTAMP`, `TIMESTAMP WITH TIME ZONE` and `WITH LOCAL TIME ZONE`
- read/write fractional seconds and TZ offsets
- convert between timestamp kinds and do interval arithmetic

## In context — DATE grew up

`TIMESTAMP` is the 9i-era answer to DATE's two limits: second-only precision and
no timezone. Oracle added fractional seconds (up to nanoseconds) and a zone
payload, then split the family into three storage flavors that all map to
`OCI_Timestamp`:

- `TIMESTAMP` — the bigger DATE: nanosecond clock, still zone-naive.
- `TIMESTAMP WITH TIME ZONE` — stores the offset (or zone name) with the value;
  two clients in different zones see the same instant rendered differently.
- `TIMESTAMP WITH LOCAL TIME ZONE` — stores only the instant; Oracle normalizes
  it to the *session* timezone on insert and renders it back in the reader's own
  zone on fetch. Same bytes for everyone, different displayed clock.

Choosing between them is a schema decision you should understand before picking
the OCILIB subtype: TSTZ preserves *where* the event happened, LTZ preserves
*when* relative to whoever reads it, and plain TIMESTAMP keeps the naive
behavior most legacy schemas expect.

OCILIB models the three flavors as one handle whose subtype is fixed at
creation (`OCI_TIMESTAMP`, `OCI_TIMESTAMP_TZ`, `OCI_TIMESTAMP_LTZ` — the C++
`Timestamp::NoTimeZone`/`WithTimeZone`/`LocalTimeZone`). The subtype travels on
the handle: binds and fetches check it, and `OCI_TimestampConvert` is the
explicit bridge when you need to move a value from one flavor to another —
normalizing a TSTZ into the session zone is exactly the `Convert` call, not
implicit magic. Interval arithmetic (`TimestampIntervalAdd`/`Subtract`) uses the
`OCI_Interval` values of the previous page, which is why the datatype section
introduces them in this order.

## The Timestamp object

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

OCI_TimestampConstruct(ts, 2026, 11, 6, 14, 30, 15, 123456789, NULL);
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
ts.GetDate();
ts.GetTime();                       /* parts only                */
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
struct tm tmv;
time_t t;
OCI_TimestampToCTime(ts, &tmv, &t);   /* out: broken-down + epoch */
OCI_TimestampFromCTime(ts, &tmv, t); /* in: both forms           */
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

A fetched `TIMESTAMP` column gives a borrowed `Timestamp` (`OCI_ResultsetGetTimestamp` /
`Get<Timestamp>`); the bind takes the handle (`OCI_StatementBindTimestamp(st, ":ts", ts)`)
— the subtype travels on the handle itself, so create it with the right kind to match
the column (`Timestamp ts(env, Timestamp::WithTimeZone)` + `Bind`).

:::sample
```c
OCI_Timestamp *ts = OCI_TimestampCreate(env, OCI_TIMESTAMP_LTZ);
OCI_TimestampSysTimestamp(ts);

OCI_StatementPrepare(st, OTEXT(
    "insert into events(id, at) values(:i, :t)"));
OCI_StatementBindInt32    (st, OTEXT(":i"), &ev_id);
OCI_StatementBindTimestamp(st, OTEXT(":t"), ts);
OCI_StatementExecutePrepared(st);

/* fetch: borrowed handle, same subtype as the column                 */
OCI_ResultsetFetchNext(rs);
OCI_Timestamp *got = OCI_ResultsetGetTimestamp(rs, 0);
```

```cpp
Timestamp ts(env, Timestamp::LocalTimeZone);
ts.SysTimestamp();

st.Prepare("insert into events(id, at) values(:i, :t)");
st.Bind(":i", ev_id);
st.Bind(":t", ts);
st.ExecutePrepared();

/* fetch: borrowed handle, same subtype as the column                 */
rs.Next();
Timestamp got = rs.Get<Timestamp>(0);
```
:::

## Scenario — find entries newer than 24 h

Interval arithmetic answers staleness queries without touching SQL:

:::sample
```c
OCI_Timestamp *now = OCI_TimestampCreate(env, OCI_TIMESTAMP);
OCI_Interval  *age = OCI_IntervalCreate(env, OCI_INTERVAL_DS);
OCI_TimestampSysTimestamp(now);

while (OCI_ResultsetFetchNext(rs))
{
    OCI_Timestamp *t = OCI_ResultsetGetTimestamp(rs, 0);   /* borrowed */
    OCI_TimestampSubtract(now, t, age);                    /* age      */

    OCI_Int32 d;
    OCI_IntervalGetDaySecond(age, &d, NULL, NULL, NULL, NULL);
    if (d < 1) { /* fresh row */ }
}
```

```cpp
Timestamp now = Timestamp::SysTimestamp(env);
Interval  age(env, Interval::DaySecond);

while (rs.Next())
{
    Timestamp t = rs.Get<Timestamp>(0);                    /* borrowed */
    now.Subtract(t, age);                                  /* age      */

    Int32 d;
    age.GetDaySecond(d, h, m, s, f);
    if (d < 1) { /* fresh row */ }
}
```
:::

## Scenario — normalize a TZ timestamp to session-local

`Convert` moves between subtypes — useful when a column is `TIMESTAMP WITH LOCAL
TIME ZONE` but the client got a `WITH TIME ZONE` value:

:::sample
```c
OCI_Timestamp *tz  = OCI_TimestampCreate(env, OCI_TIMESTAMP_TZ);
OCI_Timestamp *ltz = OCI_TimestampCreate(env, OCI_TIMESTAMP_LTZ);
OCI_TimestampConstruct(tz, 2026, 6, 15, 14, 0, 0, 0, OTEXT("+05:30"));
OCI_TimestampConvert(ltz, tz);                /* normalized to session   */
```

```cpp
Timestamp tz (env, Timestamp::WithTimeZone);
Timestamp ltz(env, Timestamp::LocalTimeZone);
tz.Construct(2026, 6, 15, 14, 0, 0, 0, "+05:30");
ltz.Convert(tz);                              /* normalized to session   */
```
:::

## Putting it together — the typical sequence

Timestamp work starts with a schema question, then a mechanical flow:

1. **Match the subtype to the column first.** Check the column definition (or
   `GetSubType` on a describe) — `OCI_TIMESTAMP`, `_TZ`, or `_LTZ`. Picking the
   wrong one is the #1 timestamp bug: the offset is silently dropped.
2. **Create and fill**: `Construct` for parts + optional zone, `SysTimestamp`
   for "now", or `FromText` with `FF` and `TZH:TZM` in the model.
3. **Fetch** — the handle is borrowed; convert subtype with `Convert` if the
   column and your target disagree.
4. **Do arithmetic through intervals**: `Subtract` two timestamps into a DS
   interval to measure a duration, `IntervalAdd`/`IntervalSub` to shift a
   timestamp by a span.
5. **Bind** — the subtype travels on the handle, so the bind call itself is
   just `OCI_StatementBindTimestamp(st, name, ts)`.

Hints: include `FF` in your format model or fractional seconds are truncated on
the way out; `GetTimeZoneName`/`GetTimeZoneOffset` only return real data on the
`_TZ` subtype; on `_LTZ` columns the stored offset is always the *session's* —
never assume it kept the zone you inserted.

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
