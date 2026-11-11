---
title: Every bind type — the complete family
navtitle: All bind types
part: 3
level: intermediate
---

# Every bind type — the complete family

**Objectives** — by the end of this page you will be able to:

- bind any OCILIB type, not just scalars
- know which binds carry sizes, subtypes or extra arguments
- pick `OCI_StatementBindCursor` vs `OCI_StatementBindLong` for edge cases

## The full table

Every bind links a `:name` placeholder to a host value — scalars bind raw pointers, everything else binds an OCILIB datatype handle (Part 2 covers creating them).

In C++ one templated `Bind(":n", value)` dispatches on the bound type — subtypes come from the object itself, so no subtype argument is needed there.

| C function | Bound type | C++ equivalent | Extra args / notes |
|------------|-----------|----------------|--------------------|
| `OCI_StatementBindInt16` | `OCI_Int16*` | `Bind(":n", v)` | — |
| `OCI_StatementBindUInt16` | `OCI_UInt16*` | `Bind(":n", v)` | — |
| `OCI_StatementBindInt32` | `OCI_Int32*` | `Bind(":n", v)` | — |
| `OCI_StatementBindUInt32` | `OCI_UInt32*` | `Bind(":n", v)` | — |
| `OCI_StatementBindInt64` | `OCI_Int64*` | `Bind(":n", v)` | — |
| `OCI_StatementBindUInt64` | `OCI_UInt64*` | `Bind(":n", v)` | — |
| `OCI_StatementBindFloat` | `OCI_Float*` | `Bind(":n", v)` | — |
| `OCI_StatementBindDouble` | `OCI_Double*` | `Bind(":n", v)` | — |
| `OCI_StatementBindBoolean` | `OCI_Boolean*` | `Bind(":n", v)` | SQL BOOLEAN (23ai) |
| `OCI_StatementBindString` | `otext*` | `Bind(":n", str)` | `size` — buffer capacity |
| `OCI_StatementBindRaw` | `unsigned char*` | `Bind(":n", raw)` | `size` |
| `OCI_StatementBindNumber` | `OCI_Number*` | `Bind(":n", num)` | exact decimal |
| `OCI_StatementBindDate` | `OCI_Date*` | `Bind(":n", date)` | — |
| `OCI_StatementBindTimestamp` | `OCI_Timestamp*` | `Bind(":n", ts)` | subtype (`OCI_TIMESTAMP*`); C++ infers from the object |
| `OCI_StatementBindInterval` | `OCI_Interval*` | `Bind(":n", iv)` | subtype (`OCI_INTERVAL_YM`/`DS`); C++ infers |
| `OCI_StatementBindLob` | `OCI_Lob*` | `Bind(":n", lob)` | subtype (`OCI_BLOB`/`CLOB`/`NCLOB`); C++ infers |
| `OCI_StatementBindFile` | `OCI_File*` | `Bind(":n", file)` | subtype (`OCI_CFILE`/`BFILE`); C++ infers |
| `OCI_StatementBindLong` | `OCI_Long*` | `Bind(":n", lng)` | subtype (`OCI_CLONG`/`OCI_BLONG`); C++ infers |
| `OCI_StatementBindObject` | `OCI_Object*` | `Bind(":n", obj)` | object types (Part 6) |
| `OCI_StatementBindCollection` | `OCI_Collection*` | `Bind(":n", coll)` | collections (Part 6) |
| `OCI_StatementBindReference` | `OCI_Reference*` | `Bind(":n", ref)` | REFs (Part 6) |
| `OCI_StatementBindJson` | `OCI_Json*` | `Bind(":n", json)` | JSON columns |
| `OCI_StatementBindVector` | `OCI_Vector*` | `Bind(":n", vec)` | 23ai vectors |
| `OCI_StatementBindCursor` | `OCI_Statement**` | `Bind(":n", stmt)` | REF CURSOR parameters (Part 4) |

!!! note "The subtype argument — C only"
    LOB/timestamp/interval/file/long binds take a trailing subtype constant so OCILIB
    can size the OCI bind correctly — e.g.
    `OCI_StatementBindTimestamp(st, OTEXT(":ts"), &ts, OCI_TIMESTAMP_TZ)`.
    In C++ the bound object's own type supplies it.
    XMLType has no dedicated bind call — bind XML as a string/LOB into `xmltype(:x)`,
    or fetch `Get<XmlType>` on the result side.

## A non-scalar example — DATE bind

:::sample
```c
OCI_Date *d = OCI_DateCreate(env);
OCI_DateFromText(d, OTEXT("1981-04-02"), OTEXT("YYYY-MM-DD"));

OCI_StatementPrepare(st, OTEXT("insert into employees(empno, ename, hiredate, deptno) "
                      "values(:n, :e, :h, :d)"));
OCI_StatementBindInt32   (st, OTEXT(":n"), &empno);
OCI_StatementBindString(st, OTEXT(":e"),  ename, sizeof(ename) - 1);
OCI_StatementBindDate  (st, OTEXT(":h"),  d);
OCI_StatementBindInt32   (st, OTEXT(":d"), &deptno);
OCI_StatementExecutePrepared(st);
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

## Putting it together — the typical sequence

Choosing the bind flavor mirrors the getter choice, with one addition:

1. **Scalar** — `Bind<Int32>`/`BindInt32` for NUMBER ints, `BindDouble` for
   NUMBER floats, `BindString` for VARCHAR2.
2. **Object** — `Bind<Date>`/`Bind<Timestamp>`/`Bind<Lob>`/`Bind<Json>`/
   `Bind<Object>` for datatype handles.
3. **Array** — `Bind<T>Array` / `StatementBind*Array` families for bulk (the
   Part 8 topic).
4. **OUT-capable** — anything can be OUT; the bind just needs a variable to
   write back into.
5. **Named vs positional** — binds are by `:name`; positional `?` is a
   different OCI convention OCILIB doesn't expose.

Hints: the bind name must include the colon — `":id"` not `"id"`; type
mismatches between bind and column are the top silent-failure source — a
`GetInt32` column bound as string returns text, and OCILIB doesn't warn; and
C++ `Bind(":x", obj)` infers the type from the C++ argument, removing the tag
entirely.

## Try it yourself

1. Bind a `TIMESTAMP WITH TIME ZONE` (subtype!) into an insert.
2. Bind a `NUMBER` via `OCI_Number` and compare precision vs a `double` bind.
3. Print `OCI_StatementGetBindCount` after binding a query with three placeholders.

## Recap

- Scalars bind directly; every other type binds its `OCI_*` handle, sometimes with a
  subtype constant.
- `OCI_StatementBindCursor` is the REF CURSOR special case (Part 4).

**Next:** directions, NULL flags and rebinding — managing existing binds.
