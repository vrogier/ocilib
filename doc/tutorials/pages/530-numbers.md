---
title: Exact numbers — OCI_Number
navtitle: Numbers
part: 5
level: intermediate
demos: [number.c]
---

# Exact numbers — the `Number` object

**Objectives** — by the end of this page you will be able to:

- decide when `double` isn't good enough and `Number` is
- convert between `Number` and C types/text
- do exact decimal arithmetic

## Meet the Number object

Oracle `NUMBER` stores up to **38 significant decimal digits** — `double` holds ~15.

Money, high-precision measurements, big counters: that's what `Number` keeps exact.

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
OCI_NumberGetValue(n, OCI_NUM_BIGINT, &bval);       /* Number → C   */

otext buf[64];
OCI_NumberToText(n, OTEXT("999999999.999999999"), sizeof(buf), buf);
```

```cpp
Number n(env);

n.FromText("123456789.123456789", "999999999.999999999");

n.SetValue<double>(dval);                          /* C → Number   */
BigInt bval = n.GetValue<BigInt>();                /* Number → C   */

String s = n.ToText("999999999.999999999");
```
:::

## Arithmetic

:::sample
```c
OCI_Number *r = OCI_NumberCreate(env);
OCI_NumberAssign(r, n1);
OCI_NumberAdd     (r, n2);          /* r += n2 — exact, no float error */
OCI_NumberMultiply(r, n3);
OCI_NumberDivide  (r, n4);
OCI_NumberSub     (r, n5);
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

- A fetched exact `NUMBER` column gives a borrowed `Number` (`OCI_GetNumber` /
  `Get<Number>`).
- Bind one with the number bind (`OCI_BindNumber` / `Bind(":n", n)`).
- `GetNumeric<T>` on `JsonNode` accepts `Number` too (v5 JSON).

## Try it yourself

1. Sum `sal` over all employees in `OCI_Number` vs `double` — print both, watch
   float drift on big values.
2. Round-trip `123456789012345678901234567890.123` through C `double` — it breaks;
   through `OCI_Number` — it survives.
3. Compute `(sal * 1.075)` exactly and insert the result.

## Common pitfalls

- **`GetValue` overflow** — converting a 38-digit NUMBER to `OCI_NUM_INT` truncates;
  use `BIGINT`/`NUMBER`.
- **Forgetting format for `ToText`** — format model controls digits/decimal sep.
- **Using Number for simple ints** — overkill; `OCI_GetInt` is fine for normal ranges.

## Recap

- `OCI_Number` = Oracle's exact 38-digit decimal in C — converts, compares, and does
  exact arithmetic.

**Next:** `RAW` data and `OCI_BindRaw`.
