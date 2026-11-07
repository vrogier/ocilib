---
title: Every bind type — the complete family
navtitle: All bind types
part: 2
level: intermediate
---

# Every bind type — the complete family

**Objectives** — by the end of this page you will be able to:

- bind any OCILIB type, not just scalars
- know which binds carry sizes, subtypes or extra arguments
- pick `OCI_BindStatement` vs `OCI_BindLong` for edge cases

## The full table

Every bind links a `:name` placeholder to a host value — scalars bind raw pointers, everything else binds an OCILIB datatype handle (Part 5 covers creating them).

In C++ one templated `Bind(":n", value)` dispatches on the bound type — subtypes come from the object itself, so no subtype argument is needed there.

| C function | Bound type | C++ equivalent | Extra args / notes |
|------------|-----------|----------------|--------------------|
| `OCI_BindShort` | `OCI_Int16*` | `Bind(":n", v)` | — |
| `OCI_BindUnsignedShort` | `OCI_UInt16*` | `Bind(":n", v)` | — |
| `OCI_BindInt` | `OCI_Int32*` | `Bind(":n", v)` | — |
| `OCI_BindUnsignedInt` | `OCI_UInt32*` | `Bind(":n", v)` | — |
| `OCI_BindBigInt` | `OCI_Int64*` | `Bind(":n", v)` | — |
| `OCI_BindUnsignedBigInt` | `OCI_UInt64*` | `Bind(":n", v)` | — |
| `OCI_BindFloat` | `OCI_Float*` | `Bind(":n", v)` | — |
| `OCI_BindDouble` | `OCI_Double*` | `Bind(":n", v)` | — |
| `OCI_BindBoolean` | `OCI_Boolean*` | `Bind(":n", v)` | SQL BOOLEAN (23ai) |
| `OCI_BindString` | `otext*` | `Bind(":n", str)` | `size` — buffer capacity |
| `OCI_BindRaw` | `unsigned char*` | `Bind(":n", raw)` | `size` |
| `OCI_BindNumber` | `OCI_Number*` | `Bind(":n", num)` | exact decimal |
| `OCI_BindDate` | `OCI_Date*` | `Bind(":n", date)` | — |
| `OCI_BindTimestamp` | `OCI_Timestamp*` | `Bind(":n", ts)` | subtype (`OCI_TIMESTAMP*`); C++ infers from the object |
| `OCI_BindInterval` | `OCI_Interval*` | `Bind(":n", iv)` | subtype (`OCI_INTERVAL_YM`/`DS`); C++ infers |
| `OCI_BindLob` | `OCI_Lob*` | `Bind(":n", lob)` | subtype (`OCI_BLOB`/`CLOB`/`NCLOB`); C++ infers |
| `OCI_BindFile` | `OCI_File*` | `Bind(":n", file)` | subtype (`OCI_CFILE`/`BFILE`); C++ infers |
| `OCI_BindLong` | `OCI_Long*` | `Bind(":n", lng)` | subtype (`OCI_LONG`/`LONGRAW`); C++ infers |
| `OCI_BindObject` | `OCI_Object*` | `Bind(":n", obj)` | object types (Part 6) |
| `OCI_BindColl` | `OCI_Coll*` | `Bind(":n", coll)` | collections (Part 6) |
| `OCI_BindRef` | `OCI_Ref*` | `Bind(":n", ref)` | REFs (Part 6) |
| `OCI_BindJson` | `OCI_Json*` | `Bind(":n", json)` | JSON columns |
| `OCI_BindVector` | `OCI_Vector*` | `Bind(":n", vec)` | 23ai vectors |
| `OCI_BindStatement` | `OCI_Statement**` | `Bind(":n", stmt)` | REF CURSOR parameters (Part 3) |

!!! note "The subtype argument — C only"
    LOB/timestamp/interval/file/long binds take a trailing subtype constant so OCILIB
    can size the OCI bind correctly — e.g.
    `OCI_BindTimestamp(st, OTEXT(":ts"), &ts, OCI_TIMESTAMP_TZ)`.
    In C++ the bound object's own type supplies it.
    XMLType has no dedicated bind call — bind XML as a string/LOB into `xmltype(:x)`,
    or fetch `Get<XmlType>` on the result side.

## A non-scalar example — DATE bind

:::sample
```c
OCI_Date *d = OCI_DateCreate(env);
OCI_DateFromText(d, OTEXT("1981-04-02"), OTEXT("YYYY-MM-DD"));

OCI_Prepare(st, OTEXT("insert into employees(empno, ename, hiredate, deptno) "
                      "values(:n, :e, :h, :d)"));
OCI_BindInt   (st, OTEXT(":n"), &empno);
OCI_BindString(st, OTEXT(":e"),  ename, sizeof(ename) - 1);
OCI_BindDate  (st, OTEXT(":h"),  d);
OCI_BindInt   (st, OTEXT(":d"), &deptno);
OCI_Execute(st);
OCI_DateFree(d);
```

```cpp
Date d(env);
d.FromText("1981-04-02", "YYYY-MM-DD");

st.Prepare("insert into employees(empno, ename, hiredate, deptno) "
           "values(:n, :e, :h, :d)");
st.Bind(":n", empno);
st.Bind(":e", ename);
st.Bind(":h", d);
st.Bind(":d", deptno);
st.ExecutePrepared();
```
:::

## Binds are first-class objects

Every successful bind registers a bind object on the statement — you can enumerate them (`GetBindCount`/`GetBind`/`GetBindIndex` on the statement) and inspect or tweak each one.

In C++ the `Bind` call hands you a `BindInfo` for exactly that.

That's the hook for the next page's direction/null/flag management.

## Try it yourself

1. Bind a `TIMESTAMP WITH TIME ZONE` (subtype!) into an insert.
2. Bind a `NUMBER` via `OCI_Number` and compare precision vs a `double` bind.
3. Print `OCI_GetBindCount` after binding a query with three placeholders.

## Recap

- Scalars bind directly; every other type binds its `OCI_*` handle, sometimes with a
  subtype constant.
- `OCI_BindStatement` is the REF CURSOR special case (Part 3).

**Next:** directions, NULL flags and rebinding — managing existing binds.
