---
title: Appendix — SQL ↔ C ↔ C++ type map
navtitle: Type map
part: 10
---

# Appendix — SQL ↔ C ↔ C++ type map

The cheat sheet for picking a getter, a bind, and a `Get<T>`.

| SQL type | C getter | C bind | C++ `Get<T>` / `Bind` |
|----------|----------|--------|------------------------|
| `NUMBER` small | `OCI_GetShort`/`UShort`/`Int`/`UInt` | `OCI_BindShort`/`…UInt` | `Get<short>`/`Get<int>`/`Get<UInt32>` |
| `NUMBER` big | `OCI_GetBigInt`/`GetUnsignedBigInt` | `OCI_BindBigInt`/`…UBigInt` | `Get<BigInt>`/`Get<UBigInt>` |
| `NUMBER`/`FLOAT` | `OCI_GetFloat`/`GetDouble` | `OCI_BindFloat`/`BindDouble` | `Get<float>`/`Get<double>` |
| `NUMBER` exact | `OCI_GetNumber` | `OCI_BindNumber` | `Get<Number>` |
| `BOOLEAN` (23ai) | `OCI_GetBoolean` | `OCI_BindBoolean` | `Get<bool>` |
| `CHAR`/`VARCHAR2`/`NCHAR`/`NVARCHAR2`/`CLOB`-as-text | `OCI_GetString` | `OCI_BindString` | `Get<String>` |
| `RAW` | `OCI_GetRaw` | `OCI_BindRaw` | `Get<Raw>` |
| `DATE` | `OCI_GetDate` | `OCI_BindDate` | `Get<Date>` |
| `TIMESTAMP`(+TZ/LTZ) | `OCI_GetTimestamp` | `OCI_BindTimestamp`(+subtype) | `Get<Timestamp>` |
| `INTERVAL YM/DS` | `OCI_GetInterval` | `OCI_BindInterval`(+subtype) | `Get<Interval>` |
| `BLOB`/`CLOB`/`NCLOB` | `OCI_GetLob` | `OCI_BindLob`(+subtype) | `Get<Lob>` |
| `BFILE`/`CFILE` | `OCI_GetFile` | `OCI_BindFile`(+subtype) | `Get<File>` |
| `LONG`/`LONG RAW` | `OCI_GetLong` | `OCI_BindLong`(+subtype) | `Get<Long>` / `Get<String>` in `LongImplicit` |
| `SYS.XMLTYPE` | `OCI_GetXmlType` | via `xmltype(:x)` + `OCI_BindString`/`Lob` | `Get<XmlType>` |
| `JSON` | `OCI_GetJson` | `OCI_BindJson` | `Get<Json>` |
| `VECTOR` (23ai) | `OCI_GetVector` | `OCI_BindVector` | `Get<Vector>` |
| object types | `OCI_GetObject` | `OCI_BindObject` | `Get<Object>` |
| `VARRAY`/nested table | `OCI_GetColl` | `OCI_BindColl` | `Get<Collection<T>>` |
| `REF` | `OCI_GetRef` | `OCI_BindRef` | `Get<Reference>` |
| `REF CURSOR` column | `OCI_GetStatement` | `OCI_BindStatement` | `Get<Statement>` |
| `ROWID` | `OCI_GetString` | `OCI_BindString` | `Get<String>` |

**Rules of thumb**

- Every getter has a `*ByName` twin; every bind a matching `OCI_BindArrayOf*` and
  `OCI_Register*` form.
- Subtype-arg binds carry the SQL kind (`OCI_BLOB`, `OCI_TIMESTAMP_TZ`,
  `OCI_INTERVAL_DS`…) — match it to the column.
- `GetOptional<T>` wraps any `Get<T>` for NULL-safe C++17 code.
