---
title: Appendix — SQL ↔ C ↔ C++ type map
navtitle: Type map
part: 10
---

# Appendix — SQL ↔ C ↔ C++ type map

The cheat sheet for picking a getter, a bind, and a `Get<T>`.

| SQL type | C getter | C bind | C++ `Get<T>` / `Bind` |
|----------|----------|--------|------------------------|
| `NUMBER` small | `OCI_ResultsetGetInt16`/`GetUInt16`/`GetInt32`/`GetUInt32` | `OCI_StatementBindInt16`/`…UInt16`/`…Int32`/`…UInt32` | `Get<Int16>`/`Get<Int32>`/`Get<UInt16>`/`Get<UInt32>` |
| `NUMBER` big | `OCI_ResultsetGetInt64`/`GetUInt64` | `OCI_StatementBindInt64`/`…UInt64` | `Get<Int64>`/`Get<UInt64>` |
| `NUMBER`/`FLOAT` | `OCI_ResultsetGetFloat`/`GetDouble` | `OCI_StatementBindFloat`/`BindDouble` | `Get<Float>`/`Get<Double>` |
| `NUMBER` exact | `OCI_ResultsetGetNumber` | `OCI_StatementBindNumber` | `Get<Number>` |
| `BOOLEAN` (23ai) | `OCI_ResultsetGetInt32` | `OCI_StatementBindBoolean` | `Get<Int32>` |
| `CHAR`/`VARCHAR2`/`NCHAR`/`NVARCHAR2`/`CLOB`-as-text | `OCI_ResultsetGetString` | `OCI_StatementBindString` | `Get<String>` |
| `RAW` | `OCI_ResultsetGetRaw` | `OCI_StatementBindRaw` | `Get<Raw>` |
| `DATE` | `OCI_ResultsetGetDate` | `OCI_StatementBindDate` | `Get<Date>` |
| `TIMESTAMP`(+TZ/LTZ) | `OCI_ResultsetGetTimestamp` | `OCI_StatementBindTimestamp`(+subtype) | `Get<Timestamp>` |
| `INTERVAL YM/DS` | `OCI_ResultsetGetInterval` | `OCI_StatementBindInterval`(+subtype) | `Get<Interval>` |
| `BLOB`/`CLOB`/`NCLOB` | `OCI_ResultsetGetLob` | `OCI_StatementBindLob`(+subtype) | `Get<Clob>`/`Get<Blob>`/`Get<NClob>` |
| `BFILE`/`CFILE` | `OCI_ResultsetGetFile` | `OCI_StatementBindFile`(+subtype) | `Get<File>` |
| `LONG`/`LONG RAW` | `OCI_ResultsetGetLong` | `OCI_StatementBindLong`(+subtype) | `Get<Clong>`/`Get<Blong>` / `Get<String>` in `LongImplicit` |
| `SYS.XMLTYPE` | `OCI_ResultsetGetXmlType` | via `xmltype(:x)` + `OCI_StatementBindString`/`Lob` | `Get<XmlType>` |
| `JSON` | `OCI_ResultsetGetJson` | `OCI_StatementBindJson` | `Get<Json>` |
| `VECTOR` (23ai) | `OCI_ResultsetGetVector` | `OCI_StatementBindVector` | `Get<Vector>` |
| object types | `OCI_ResultsetGetObject` | `OCI_StatementBindObject` | `Get<Object>` |
| `VARRAY`/nested table | `OCI_ResultsetGetColl` | `OCI_StatementBindCollection` | `Get<Collection<T>>` |
| `REF` | `OCI_ResultsetGetReference` | `OCI_StatementBindReference` | `Get<Reference>` |
| `REF CURSOR` column | `OCI_ResultsetGetCursor` | `OCI_StatementBindCursor` | `Get<Statement>` |
| `ROWID` | `OCI_ResultsetGetString` | `OCI_StatementBindString` | `Get<String>` |
| PL/SQL `RECORD` | — (not a column) | `OCI_StatementBindObject` | `Object` / `Set<T>` |
| PL/SQL `BOOLEAN` param | — (not a column) | `OCI_StatementBindBoolean` | `Bind(":b", v)` |

**Rules of thumb**

- Every getter has a `*ByName` twin; every bind a matching `OCI_StatementBind*Array` and
  `OCI_StatementRegister*` form.
- Subtype-arg binds carry the SQL kind (`OCI_BLOB`, `OCI_TIMESTAMP_TZ`,
  `OCI_INTERVAL_DS`…) — match it to the column.
- `GetOptional<T>` wraps any `Get<T>` for NULL-safe C++17 code.
