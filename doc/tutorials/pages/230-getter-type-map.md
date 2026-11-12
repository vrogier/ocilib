---
title: The complete getter type map
navtitle: Getter type map
part: 3
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
| `NUMBER` (small int) | `OCI_ResultsetGetInt16` | `Get<Int16>` |
| `NUMBER` (small unsigned) | `OCI_ResultsetGetUInt16` | `Get<UInt16>` |
| `NUMBER` (int) | `OCI_ResultsetGetInt32` | `Get<Int32>` |
| `NUMBER` (unsigned int) | `OCI_ResultsetGetUInt32` | `Get<UInt32>` |
| `NUMBER` (big int) | `OCI_ResultsetGetInt64` | `Get<Int64>` |
| `NUMBER` (big unsigned) | `OCI_ResultsetGetUInt64` | `Get<UInt64>` |
| `NUMBER` (float) | `OCI_ResultsetGetFloat` | `Get<Float>` |
| `NUMBER` / `FLOAT` | `OCI_ResultsetGetDouble` | `Get<Double>` |
| `NUMBER` (exact) | `OCI_ResultsetGetNumber` | `Get<Number>` |
| `BOOLEAN` (23ai) | `OCI_ResultsetGetInt32` | `Get<Int32>` |
| `CHAR/VARCHAR2/NCHAR` | `OCI_ResultsetGetString` | `Get<String>` |
| `RAW` | `OCI_ResultsetGetRaw` | `Get<Raw>` (`std::vector<unsigned char>`) |

## Object handles (borrowed — never freed)

| SQL type | C getter | C++ `Get<T>` | Handle type |
|----------|----------|--------------|-------------|
| `DATE` | `OCI_ResultsetGetDate` | `Get<Date>` | `OCI_Date` / `Date` → Part 2 |
| `TIMESTAMP*` / `WITH [LOCAL] TZ` | `OCI_ResultsetGetTimestamp` | `Get<Timestamp>` | `OCI_Timestamp` / `Timestamp` |
| `INTERVAL YM/DS` | `OCI_ResultsetGetInterval` | `Get<Interval>` | `OCI_Interval` / `Interval` |
| `BLOB/CLOB/NCLOB` | `OCI_ResultsetGetLob` | `Get<Lob>` | `OCI_Lob` / `Lob` |
| `BFILE` | `OCI_ResultsetGetFile` | `Get<File>` | `OCI_File` / `File` |
| `LONG`, `LONG RAW` | `OCI_ResultsetGetLong` | `Get<Long>` | `OCI_Long` / `Long` |
| object types | `OCI_ResultsetGetObject` | `Get<Object>` | `OCI_Object` / `Object` |
| `VARRAY` / nested table | `OCI_ResultsetGetColl` | `Get<Collection<T>>` | `OCI_Collection` / `Collection` |
| `REF` | `OCI_ResultsetGetReference` | `Get<Reference>` | `OCI_Reference` / `Reference` |
| `SYS.XMLTYPE` | `OCI_ResultsetGetXmlType` | `Get<XmlType>` | `OCI_XmlType` / `XmlType` |
| `JSON` | `OCI_ResultsetGetJson` | `Get<Json>` | `OCI_Json` / `Json` |
| `VECTOR` (23ai) | `OCI_ResultsetGetVector` | `Get<Vector>` | `OCI_Vector` / `Vector` |
| `REF CURSOR` | `OCI_ResultsetGetCursor` | `Get<Statement>` | `OCI_Statement` / `Statement` |

Every getter also exists in a `*ByName` variant that resolves the column name.

!!! tip "The mental rule"
    - **Scalars** → copied into your variables; safe after the next fetch.
    - **Handles** → *transient*: they point into the current row. Read them,
      don't free them, and don't keep them across `FetchNext` unless the type
      supports it (LOBs/objects are locators — they stay valid; use them
      before freeing the statement).

## In C++

`rs.Get<T>(i)` dispatches on `T` via type traits — the table above is literally the `Get<T>` specialization list (`Get<Number>`, `Get<Lob>`, `Get<Object>`, `Get<Collection<T>>`, `Get<Json>`, `Get<Vector>`…).

`GetOptional<T>` wraps any of them in `std::optional` for NULL-safety.

```cpp
auto name  = rs.Get<String>(1);
auto photo = rs.GetOptional<Lob>(5);      // std::optional<Lob>
auto doc   = rs.Get<Json>(5);
```

## Putting it together — the typical sequence

Choosing the right getter is a two-step lookup:

1. **Know the column's SQL type** — from `describe`, `GetColumn`, or the
   schema you wrote.
2. **Map it**: `NUMBER` int → `GetInt32`/`Get<Int32>`, `NUMBER` float →
   `GetDouble`, `VARCHAR2`/`CHAR` → `GetString`, `DATE` → `GetDate`,
   `TIMESTAMP` → `GetTimestamp`, `CLOB`/`BLOB` → `GetLob`, `RAW` → `GetRaw`,
   `JSON` → `GetJson`, `XMLTYPE` → `GetXmlType`, object → `Get<Object>`.
3. **C++ `Get<T>` collapses the choice** — the template dispatches on the type
   you ask for; `Get<Int32>(0)` is the same call as `OCI_ResultsetGetInt32`.

Hints: `GetString` is the universal fallback — it converts any column to text;
BOOLEAN columns are `GetInt32`, there is no `GetBoolean` on a resultset; and
the C++ `Get<T>` accepts the same set of types as the C getters — `Raw`,
`Date`, `Timestamp`, `Lob`, `Json`, `Number`, `XmlType`, `Object`, `Interval`,
`Vector`, `Long` all work.

## Try it yourself

1. `describe employees` in SQL*Plus and pick the correct getter for each column.
2. Fetch `rowid` — which getter do you use? (Hint: it's a string.)
3. Fetch a `sys_refcursor` returned by a query — walk it with `OCI_ResultsetGetCursor` then
   `OCI_StatementGetResultset` on that nested statement.

## Recap

- One getter family per SQL type; `*ByName` twin for each.
- Values are copied; handles are borrowed from the current row.
- C++ `Get<T>` maps the same table onto types, `GetOptional<T>` adds NULL safety.

**Next:** skip getters entirely for fixed shapes — map a row straight into a C struct.
