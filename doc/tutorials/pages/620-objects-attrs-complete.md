---
title: Object attributes — the complete accessor matrix
navtitle: Object attributes
part: 6
level: intermediate
---

# Object attributes — the complete accessor matrix

**Objectives** — by the end of this page you will be able to:

- pick the right attribute getter/setter for any attribute type
- handle nested objects, collections, REFs, LOBs and NULL indicators
- dump any object generically

## The matrix

For every attribute type there's a Get/Set pair on the object — one name per bound
type (the C calls shown; C++ templated `Get<T>`/`Set<T>` cover them all, see below):

| Attribute type | C setter / getter | C++ |
|----------------|-------------------|-----|
| NUMBER small | `OCI_ObjectSetInt16`/`SetUInt16` — `OCI_ObjectGetInt16`/`GetUInt16` | `Set<Int16>`/`Get<Int16>`, `Set<UInt16>`/`Get<UInt16>` |
| NUMBER int | `OCI_ObjectSetInt32`/`SetUInt32` — `OCI_ObjectGetInt32`/`GetUInt32` | `Set<Int32>`/`Get<Int32>`, `Set<UInt32>`/`Get<UInt32>` |
| NUMBER big | `OCI_ObjectSetInt64`/`SetUInt64` — `OCI_ObjectGetInt64`/`GetUInt64` | `Set<Int64>`/`Get<Int64>`, `Set<UInt64>`/`Get<UInt64>` |
| NUMBER float | `OCI_ObjectSetFloat`/`SetDouble` — `OCI_ObjectGetFloat`/`GetDouble` | `Set<Float>`/`Get<Float>`, `Set<Double>`/`Get<Double>` |
| NUMBER exact | `OCI_ObjectSetNumber` — `OCI_ObjectGetNumber` | `Set<Number>`/`Get<Number>` |
| BOOLEAN | `OCI_ObjectSetBoolean` — `OCI_ObjectGetBoolean` | `Set<bool>`/`Get<bool>` |
| CHAR/VARCHAR2 | `OCI_ObjectSetString` — `OCI_ObjectGetString` | `Set<String>`/`Get<String>` |
| RAW | `OCI_ObjectSetRaw` — `OCI_ObjectGetRaw` + `GetRawSize` | `Set<Raw>`/`Get<Raw>` |
| DATE | `OCI_ObjectSetDate` — `OCI_ObjectGetDate` | `Set<Date>`/`Get<Date>` |
| TIMESTAMP | `OCI_ObjectSetTimestamp` — `OCI_ObjectGetTimestamp` | `Set<Timestamp>`/`Get<Timestamp>` |
| INTERVAL | `OCI_ObjectSetInterval` — `OCI_ObjectGetInterval` | `Set<Interval>`/`Get<Interval>` |
| LOB | `OCI_ObjectSetLob` — `OCI_ObjectGetLob` | `Set<Clob>`/`Get<Clob>` etc. |
| BFILE | `OCI_ObjectSetFile` — `OCI_ObjectGetFile` | `Set<File>`/`Get<File>` |
| nested object | `SetObject` | `GetObject` |
| collection | `SetColl` | `GetColl` |
| REF | `SetRef` | `GetRef` |
| (any) NULL | `SetNull` | `IsNull` |

Plus utilities: `OCI_ObjectGetStruct` (whole object → C struct, same rules as
`OCI_ResultsetGetStruct` page 235), `OCI_ObjectGetSelfReference` (a REF pointing at the object),
`OCI_ObjectToText` (debug dump), `OCI_ObjectAssign`, `OCI_ObjectGetType`/`GetTypeInfo`.

## Nested structures

:::sample
```c
OCI_Object *sale = OCI_ResultsetGetObject(rs, 0);

OCI_Object *vendor = OCI_ObjectGetObject(sale, OTEXT("VENDOR"));   /* borrowed */
printf("vendor: %s\n", OCI_ObjectGetString(vendor, OTEXT("NAME")));
OCI_ObjectFree(vendor);            /* Get* handles on objects are owned → free */

OCI_Collection *tags = OCI_ObjectGetColl(sale, OTEXT("TAGS"));           /* → page 630 */
OCI_Reference  *r    = OCI_ObjectGetReference (sale, OTEXT("MANAGER_REF"));    /* → page 640 */
```

```cpp
Object sale = row.Get<Object>(0);

Object vendor = sale.Get<Object>("VENDOR");       /* owned ref — RAII frees it */
std::cout << "vendor: " << vendor.Get<String>("NAME") << "\n";

Collection<Object> tags = sale.Get<Collection<Object>>("TAGS");   /* → page 630 */
Ref r = sale.Get<Ref>("MANAGER_REF");                             /* → page 640 */
```
:::

!!! note "Who frees what"
    Handles pulled **out of** an object (`GetObject`, `GetColl`, `GetRef`, `GetLob`,
    `GetDate`…) are owned references — free them. The object itself is borrowed when
    it came from `OCI_ResultsetGetObject` on a row.

## Generic attribute dump

:::sample
```c
OCI_TypeInfo *ti = OCI_ObjectGetTypeInfo(obj);
for (OCI_UInt32 i = 0; i < OCI_TypeInfoGetColumnCount(ti); ++i)
{
    OCI_Column *a = OCI_TypeInfoGetColumn(ti, i);
    const otext *n = OCI_ColumnGetName(a);
    printf("%-12s %-15s %s\n", n, OCI_ColumnGetSqlType(a),
           OCI_ObjectIsNull(obj, n) ? OTEXT("(null)") : OTEXT("…"));
}
```

```cpp
TypeInfo ti = obj.GetTypeInfo();
for (UInt32 i = 0; i < ti.GetColumnCount(); ++i)
{
    Column a = ti.GetColumn(i);
    String n = a.GetName();
    std::cout << n << " " << a.GetSQLType() << " "
              << (obj.IsNull(n) ? "(null)" : "…") << "\n";
}
```
:::

## The C++ way — typed `Get<T>`/`Set<T>`/`GetOptional<T>`

Where C needs one call per attribute type, C++ collapses the whole matrix into a
single templated accessor — the attribute's column type decides `T`:

```cpp
Object s = row.Get<Object>(0);

int code    = s.Get<int>("CODE");
String name = s.Get<String>("NAME");
double p    = s.Get<double>("PRICE");

s.Set<int>("CODE", 2);
s.Set<String>("NAME", "NEW");

/* nullable attribute — no separate NULL-flag dance: */
auto price = s.GetOptional<double>("PRICE");   /* std::optional<double> */
if (price)  std::cout << *price << "\n";

/* nested handle — a typed Get yields the right wrapper, RAII-freed: */
Object vendor = s.Get<Object>("VENDOR");
std::cout << vendor.Get<String>("NAME") << "\n";
```

`GetOptional<T>` is the C++ answer to the per-attribute null flag — it returns an
empty `std::optional` when the attribute is NULL, so the flag check and the value
read are one call.

## Putting it together — the typical sequence

Attribute access is a name+type lookup per field:

1. **Get the TypeInfo** — it knows every attribute's name, type, order.
2. **Read attrs** — `ObjectGet<Type>(obj, "attr")` per field — the type must
   match the attribute's declared type.
3. **Write attrs** — `ObjectSet<Type>(obj, "attr", val)` — same name, same
   type-match rule.
4. **NULL attrs** — `ObjectSetNull`/`IsNullAttribute` handle the not-set case.

Hints: the getter/setter type must match the attribute's SQL type — a NUMBER
attr takes `Int32`/`Double`, not `String`; NULL attributes need the `SetNull`
family, not a zeroed `Set` — a zero isn't NULL; and attribute order in the
TypeInfo matches the `CREATE TYPE` declaration — index access is faster than
name lookup when you have the layout.

## Try it yourself

1. Add a `RAW(16)` and a `CLOB` attribute to `t_sale`; get/set them.
2. Write the generic dumper and run it on a fetched `t_sale`.
3. Use `GetOptional<double>` (C++) on a nullable `PRICE`.

## Common pitfalls

- **Leaking nested handles** — every `GetX` that yields a handle returns an owned one.
- **`SetObject` on wrong-typed nested value** — must match the attribute's declared type.
- **`GetString` on a NUMBER attr** — check the attr's `OCI_Column` type first.

## Recap

- One Get/Set pair per attribute type; NULL via `SetNull`/`IsNull`; nested handles
  are owned and freed.

**Next:** object operations — assign, compare, inheritance and self-refs.
