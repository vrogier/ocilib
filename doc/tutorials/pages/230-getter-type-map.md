---
title: The complete getter type map
navtitle: Getter type map
part: 2
level: beginner
---

# The complete getter type map

**Objectives** — by the end of this page you will be able to:

- pick the right `OCI_Get*` getter for any SQL column type
- know which getters return values vs handles
- predict the C++ `Get<T>` equivalent

## Scalars (returned by value)

| SQL type | C getter | C++ `Get<T>` |
|----------|----------|--------------|
| `NUMBER` (small int) | `OCI_GetShort` | `Get<short>` |
| `NUMBER` (small unsigned) | `OCI_GetUnsignedShort` | `Get<unsigned short>` |
| `NUMBER` (int) | `OCI_GetInt` | `Get<int>` |
| `NUMBER` (unsigned int) | `OCI_GetUnsignedInt` | `Get<unsigned int>` |
| `NUMBER` (big int) | `OCI_GetBigInt` | `Get<BigInt>` |
| `NUMBER` (big unsigned) | `OCI_GetUnsignedBigInt` | `Get<UBigInt>` |
| `NUMBER` (float) | `OCI_GetFloat` | `Get<float>` |
| `NUMBER` / `FLOAT` | `OCI_GetDouble` | `Get<double>` |
| `NUMBER` (exact) | `OCI_GetNumber` | `Get<Number>` |
| `BOOLEAN` (23ai) | `OCI_GetBoolean` | `Get<bool>` |
| `CHAR/VARCHAR2/NCHAR` | `OCI_GetString` | `Get<String>` |
| `RAW` | `OCI_GetRaw` | `Get<Raw>` (`std::vector<unsigned char>`) |

## Object handles (borrowed — never freed)

| SQL type | C getter | C++ `Get<T>` | Handle type |
|----------|----------|--------------|-------------|
| `DATE` | `OCI_GetDate` | `Get<Date>` | `OCI_Date` / `Date` → Part 5 |
| `TIMESTAMP*` / `WITH [LOCAL] TZ` | `OCI_GetTimestamp` | `Get<Timestamp>` | `OCI_Timestamp` / `Timestamp` |
| `INTERVAL YM/DS` | `OCI_GetInterval` | `Get<Interval>` | `OCI_Interval` / `Interval` |
| `BLOB/CLOB/NCLOB` | `OCI_GetLob` | `Get<Lob>` | `OCI_Lob` / `Lob` |
| `BFILE` | `OCI_GetFile` | `Get<File>` | `OCI_File` / `File` |
| `LONG`, `LONG RAW` | `OCI_GetLong` | `Get<Long>` | `OCI_Long` / `Long` |
| object types | `OCI_GetObject` | `Get<Object>` | `OCI_Object` / `Object` |
| `VARRAY` / nested table | `OCI_GetColl` | `Get<Collection<T>>` | `OCI_Coll` / `Collection` |
| `REF` | `OCI_GetRef` | `Get<Reference>` | `OCI_Ref` / `Reference` |
| `SYS.XMLTYPE` | `OCI_GetXmlType` | `Get<XmlType>` | `OCI_XmlType` / `XmlType` |
| `JSON` | `OCI_GetJson` | `Get<Json>` | `OCI_Json` / `Json` |
| `VECTOR` (23ai) | `OCI_GetVector` | `Get<Vector>` | `OCI_Vector` / `Vector` |
| `REF CURSOR` | `OCI_GetStatement` | `Get<Statement>` | `OCI_Statement` / `Statement` |

Every getter also exists in a `*ByName` variant that resolves the column name.

## The mental rule

- **Scalars** → copied into your variables; safe after the next fetch.
- **Handles** → *transient*: they point into the current row. Read them, don't free
  them, and don't keep them across `FetchNext` unless the type supports it
  (LOBs/objects are locators — they stay valid; use them before freeing the statement).

## In C++

`rs.Get<T>(i)` dispatches on `T` via type traits — the table above is literally the `Get<T>` specialization list (`Get<Number>`, `Get<Lob>`, `Get<Object>`, `Get<Collection<T>>`, `Get<Json>`, `Get<Vector>`…).

`GetOptional<T>` wraps any of them in `std::optional` for NULL-safety.

```cpp
auto name  = rs.Get<String>(2);
auto photo = rs.GetOptional<Lob>(5);      // std::optional<Lob>
auto doc   = rs.Get<Json>(6);
```

## Try it yourself

1. `describe employees` in SQL*Plus and pick the correct getter for each column.
2. Fetch `rowid` — which getter do you use? (Hint: it's a string.)
3. Fetch a `sys_refcursor` returned by a query — walk it with `OCI_GetStatement` then
   `OCI_GetResultset` on that nested statement.

## Recap

- One getter family per SQL type; `*ByName` twin for each.
- Values are copied; handles are borrowed from the current row.
- C++ `Get<T>` maps the same table onto types, `GetOptional<T>` adds NULL safety.

**Next:** skip getters entirely for fixed shapes — map a row straight into a C struct.
