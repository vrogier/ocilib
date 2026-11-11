---
title: Exact numbers — OCI_Number
navtitle: Numbers
part: 2
level: intermediate
demos: [number.c]
---

# Exact numbers — the `Number` object

**Objectives** — by the end of this page you will be able to:

- decide when `double` isn't good enough and `Number` is
- convert between `Number` and C types/text
- do exact decimal arithmetic

## In context — why NUMBER is not a double

Oracle `NUMBER` is packed decimal, not binary floating point: up to 38
significant decimal digits stored digit-by-digit, which means `0.1 + 0.2`
returns exactly `0.3` and money computations never drift. A C `double` holds ~15
significant digits in binary — the moment you fetch a 38-digit NUMBER into one,
you have silently rounded. That is the gap `OCI_Number` exists to fill: it is a
descriptor holding the exact decimal, with conversions to/from C scalars and
exact arithmetic that never leaves decimal space.

OCILIB exposes NUMBER at two levels, and choosing between them is the real
skill. For everyday magnitudes — employee numbers, quantities, percentages —
bind and fetch the native scalar (`OCI_Int32`, `OCI_Double`, …): the getter is
cheaper and the code is simpler. Reach for `OCI_Number` when precision is the
point: monetary aggregates, high-precision scientific values, anything that
must round-trip through the database unchanged. `GetValue`/`SetValue` convert
between the descriptor and C scalars with explicit `OCI_NUM_*` type tags, and
overflow is a *failure you must check* — a 38-digit column does not fit in an
`OCI_Int32`, and the API returns FALSE rather than truncate.

The C++ layer wraps the same descriptor as `ocilib::Number`: `SetValue<T>`/
`GetValue<T>` map the tag for you at compile time, and arithmetic operators
(`+=`, `*=`…) call the same exact-decimal OCILIB functions — so the choice of
scalar-vs-object in C++ is identical, it just reads more idiomatically.

## The Number object

Oracle `NUMBER` stores up to **38 significant decimal digits** — `double` holds ~15.

Money, high-precision measurements, big counters: that's what `Number` keeps exact.

But `Number` isn't the only way to read a `NUMBER` column — it's the *exact* way.
For everyday values, plain scalar C/C++ types are simpler and faster:

| Scalar C type | C getter | C++ |
|---------------|----------|-----|
| `OCI_Int16`/`OCI_UInt16` | `OCI_ResultsetGetInt16`/`GetUInt16` | `Get<Int16>`/`Get<UInt16>` |
| `OCI_Int32`/`OCI_UInt32` | `OCI_ResultsetGetInt32`/`GetUInt32` | `Get<Int32>`/`Get<UInt32>` |
| `OCI_Int64`/`OCI_UInt64` | `OCI_ResultsetGetInt64`/`GetUInt64` | `Get<Int64>`/`Get<UInt64>` |
| `OCI_Float`/`OCI_Double` | `OCI_ResultsetGetFloat`/`GetDouble` | `Get<Float>`/`Get<Double>` |
| exact decimal | `OCI_ResultsetGetNumber` | `Get<Number>` |

Scalar binds mirror getters one-to-one: `OCI_StatementBindInt32`, `Bind(":n", v)`…
Overflow **truncates** — a 38-digit `NUMBER` won't fit an `OCI_Int32`. Reach for
`OCI_Number`/`Number` when precision is the point; reach for scalars the rest
of the time.

It's an environment-created datatype handle like `Date`, carrying one exact decimal value you can convert, compare and compute on.

| C function | C++ method | Purpose |
|------------|------------|---------|
| `OCI_NumberCreate` | `Number` ctor (`env`) | lifecycle |
| `OCI_NumberFree` | — (RAII) | release |
| `OCI_NumberArrayCreate` / `OCI_NumberArrayFree` | — | bulk arrays |
| `OCI_NumberAssign` | `Assign` / `operator=` | copy |
| `OCI_NumberCompare` | `Compare` | exact compare |
| `OCI_NumberFromText` | `FromText` | parse with format model |
| `OCI_NumberToText` | `ToText` | render with format model |
| `OCI_NumberSetValue` | `SetValue<T>` | C numeric → Number |
| `OCI_NumberGetValue` | `GetValue<T>` | Number → C numeric |
| `OCI_NumberAdd` | `Add` | exact arithmetic |
| `OCI_NumberSub` | `Sub` | exact arithmetic |
| `OCI_NumberMultiply` | `Multiply` | exact arithmetic |
| `OCI_NumberDivide` | `Divide` | exact arithmetic |

## Conversions

:::sample
```c
OCI_Number *n = OCI_NumberCreate(env);

OCI_NumberFromText(n, OTEXT("123456789.123456789"), OTEXT("999999999.999999999"));

OCI_NumberSetValue(n, OCI_NUM_DOUBLE, &dval);       /* C → Number   */
OCI_NumberGetValue(n, OCI_NUM_INT64, &bval);       /* Number → C   */

otext buf[64];
OCI_NumberToText(n, OTEXT("999999999.999999999"), sizeof(buf), buf);
```

```cpp
Number n(env);

n.FromText("123456789.123456789", "999999999.999999999");

n.SetValue(dval);                                /* C → Number   */
Int64 bval = n.GetValue<Int64>();                /* Number → C   */

String s = n.ToText("999999999.999999999");
```
:::

## Arithmetic

:::sample
```c
OCI_Number *r = OCI_NumberCreate(env);
OCI_NumberAssign(r, n1);
OCI_NumberAdd     (r, OCI_NUM_NUMBER, n2);          /* r += n2 — exact, no float error */
OCI_NumberMultiply(r, OCI_NUM_NUMBER, n3);
OCI_NumberDivide  (r, OCI_NUM_NUMBER, n4);
OCI_NumberSub     (r, OCI_NUM_NUMBER, n5);
```

```cpp
Number r(env);
r.Assign(n1);
r.Add     (n2);                     /* r += n2 — exact, no float error */
r.Multiply(n3);
r.Divide  (n4);
r.Sub     (n5);
```
:::

## Binding & fetching

- A fetched exact `NUMBER` column gives a borrowed `Number` (`OCI_ResultsetGetNumber` /
  `Get<Number>`).
- Bind one with the number bind (`OCI_StatementBindNumber` / `Bind(":n", n)`).
- `GetNumeric<T>` on `JsonNode` accepts `Number` too (v5 JSON).

## Scenario — exact money arithmetic

`double` money drifts; `Number` doesn't. Compute `total = qty * price` with full
decimal precision:

:::sample
```c
OCI_Number *qty   = OCI_NumberCreate(env);
OCI_Number *price = OCI_NumberCreate(env);
OCI_Number *total = OCI_NumberCreate(env);

OCI_NumberFromText(qty,   OTEXT("3"),        OTEXT("9"));
OCI_NumberFromText(price, OTEXT("19.99"),    OTEXT("99D99"));
OCI_NumberAssign  (total, qty);
OCI_NumberMultiply(total, OCI_NUM_NUMBER, price);   /* 59.97 exact */

otext buf[64];
OCI_NumberToText(total, OTEXT("999D99"), sizeof(buf), buf);   /* "59.97" */
```

```cpp
Number qty(env), price(env), total(env);

qty.FromText("3", "9");
price.FromText("19.99", "99D99");
total.Assign(qty);
total.Multiply(price);                                   /* 59.97 exact */

String s = total.ToText("999D99");                       /* "59.97"      */
```
:::

## Scenario — bind scalars without a Number object

For normal magnitudes you don't need `Number` at all — bind the C scalar directly:

:::sample
```c
OCI_Int32 qty   = 3;
OCI_Double rate = 0.075;

OCI_StatementPrepare(st, OTEXT(
    "update invoices set qty = :q, tax = :r where id = :i"));
OCI_StatementBindInt32 (st, OTEXT(":q"), &qty);
OCI_StatementBindDouble(st, OTEXT(":r"), &rate);
OCI_StatementBindInt32 (st, OTEXT(":i"), &inv_id);
```

```cpp
Int32  qty   = 3;
Double rate = 0.075;

st.Prepare("update invoices set qty = :q, tax = :r where id = :i");
st.Bind(":q", qty);
st.Bind(":r", rate);
st.Bind(":i", inv_id);
```
:::

## Scenario — 38-digit safety check

When a `NUMBER` column can exceed a scalar's range, fetch as `Number` and compare
before converting:

:::sample
```c
OCI_Number *v = OCI_ResultsetGetNumber(rs, 0);       /* borrowed, exact   */

OCI_Int64 as_i64;
if (OCI_NumberGetValue(v, OCI_NUM_INT64, &as_i64))
{
    /* fits — use as_i64 */
}
else
{
    /* > int64 range — keep as Number or format via ToText */
}
```

```cpp
Number v = rs.Get<Number>(0);                        /* borrowed, exact   */
try
{
    Int64 as_i64 = v.GetValue<Int64>();              /* throws on overflow */
}
catch (...)
{
    /* > int64 range — keep as Number or format via ToText */
}
```
:::

## Putting it together — the typical sequence

Number handling starts with a fork in the road:

1. **Decide: scalar or `Number`?** Employee numbers, counts, percentages →
   scalar getters/binds, done. Money, high precision, must-round-trip →
   `OCI_Number`.
2. **If scalar**: `Get<T>`/`StatementBind<Int32|Double|…>` — that is the whole
   story; stop here.
3. **If `Number`**: create on the environment, then fill from text
   (`FromText` + format model) or from a C scalar (`SetValue` + `OCI_NUM_*` tag).
4. **Arithmetic mutates in place** — this is the trap: `Add`/`Sub`/`Multiply`/
   `Divide` modify the *target* descriptor. `Assign` a copy first if you still
   need the original.
5. **Get the value out** with `GetValue` + a `OCI_NUM_*` tag — and check the
   return: a 38-digit column doesn't fit `OCI_NUM_INT32` and OCILIB fails
   rather than truncate. `ToText` with a format model is the safe last resort.

Hints: `FromText` format models use Oracle number format characters
(`999D99`, `99G990`), not `printf`; the C++ `Number` does the same through
operators and templates — the underlying descriptor is identical.

## Try it yourself

1. Sum `sal` over all employees in `OCI_Number` vs `double` — print both, watch
   float drift on big values.
2. Round-trip `123456789012345678901234567890.123` through C `double` — it breaks;
   through `OCI_Number` — it survives.
3. Compute `(sal * 1.075)` exactly and insert the result.

## Common pitfalls

- **`GetValue` overflow** — converting a 38-digit NUMBER to `OCI_NUM_INT32` truncates;
  use `BIGINT`/`NUMBER`.
- **Forgetting format for `ToText`** — format model controls digits/decimal sep.
- **Using Number for simple ints** — overkill; `OCI_ResultsetGetInt32` is fine for normal ranges.

## Recap

- `OCI_Number` = Oracle's exact 38-digit decimal in C — converts, compares, and does
  exact arithmetic.

**Next:** `RAW` data and `OCI_StatementBindRaw`.
