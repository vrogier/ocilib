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

| Attribute type | Setter | Getter |
|----------------|--------|--------|
| NUMBER small | `OCI_ObjectSetShort` / `SetUnsignedShort` | `GetShort` / `GetUnsignedShort` |
| NUMBER int | `SetInt` / `SetUnsignedInt` | `GetInt` / `GetUnsignedInt` |
| NUMBER big | `SetBigInt` / `SetUnsignedBigInt` | `GetBigInt` / `GetUnsignedBigInt` |
| NUMBER float | `SetFloat` / `SetDouble` | `GetFloat` / `GetDouble` |
| NUMBER exact | `SetNumber` | `GetNumber` |
| BOOLEAN | `SetBoolean` | `GetBoolean` |
| CHAR/VARCHAR2 | `SetString` | `GetString` |
| RAW | `SetRaw` | `GetRaw` + `GetRawSize` |
| DATE | `SetDate` | `GetDate` |
| TIMESTAMP | `SetTimestamp` | `GetTimestamp` |
| INTERVAL | `SetInterval` | `GetInterval` |
| LOB | `SetLob` | `GetLob` |
| BFILE | `SetFile` | `GetFile` |
| nested object | `SetObject` | `GetObject` |
| collection | `SetColl` | `GetColl` |
| REF | `SetRef` | `GetRef` |
| (any) NULL | `SetNull` | `IsNull` |

Plus utilities: `OCI_ObjectGetStruct` (whole object → C struct, same rules as
`OCI_GetStruct` page 235), `OCI_ObjectGetSelfRef` (a REF pointing at the object),
`OCI_ObjectToText` (debug dump), `OCI_ObjectAssign`, `OCI_ObjectGetType`/`GetTypeInfo`.

## Nested structures

:::sample
```c
OCI_Object *sale = OCI_GetObject(rs, 0);

OCI_Object *vendor = OCI_ObjectGetObject(sale, OTEXT("VENDOR"));   /* borrowed */
printf("vendor: %s\n", OCI_ObjectGetString(vendor, OTEXT("NAME")));
OCI_ObjectFree(vendor);            /* Get* handles on objects are owned → free */

OCI_Coll *tags = OCI_ObjectGetColl(sale, OTEXT("TAGS"));           /* → page 630 */
OCI_Ref  *r    = OCI_ObjectGetRef (sale, OTEXT("MANAGER_REF"));    /* → page 640 */
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
    it came from `OCI_GetObject` on a row.

## Generic attribute dump

:::sample
```c
OCI_TypeInfo *ti = OCI_ObjectGetTypeInfo(obj);
for (OCI_UInt32 i = 0; i < OCI_TypeInfoGetColumnCount(ti); ++i)
{
    OCI_Column *a = OCI_TypeInfoGetColumn(ti, i);
    const otext *n = OCI_ColumnGetName(a);
    printf("%-12s %-15s %s\n", n, OCI_ColumnGetSQLType(a),
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
