---
title: PL/SQL-only types — BOOLEAN and RECORD
navtitle: PL/SQL types
part: 2
level: intermediate
---

# PL/SQL-only types — `BOOLEAN` and `RECORD`

**Objectives** — by the end of this page you will be able to:

- pass booleans in and out of PL/SQL calls
- describe, fill and bind a PL/SQL `RECORD`
- know where user-defined types (UDT) fit in the picture

## In context — types that only exist inside PL/SQL

PL/SQL has its own type system layered on top of SQL's. Two of its types are
worth singling out because they behave *differently* from everything else in
this part: `BOOLEAN` exists as a PL/SQL parameter type long before it became a
SQL column (23ai), and `RECORD` is a package-level composite that lives entirely
inside PL/SQL — you cannot `CREATE TABLE … (r RECORD)`, only declare variables
of it inside `BEGIN…END` blocks and package specs.

That distinction explains the OCILIB story for both: a `BOOLEAN` **parameter**
binds through the scalar bind path, while a `BOOLEAN` **column** (23ai+) fetches
through the integer resultset getters — there is no `OCI_ResultsetGetBoolean`
because the wire representation is `OCI_Boolean` (an int). And a `RECORD` is not
a column type at all — it rides on the *object* machinery OCILIB uses for UDTs:
you describe the record type via `TypeInfo` and it arrives as `OCI_Object`, with
`Get`/`Set` accessors on its named fields.

## The PL/SQL types

Two Oracle types exist **only inside PL/SQL** — no SQL column can hold them, so
no `OCI_ResultsetGet*` is needed: they flow through binds and objects only.

- `BOOLEAN` — a PL/SQL parameter type (and since 23ai, a *SQL* column type too)
- `RECORD` — a package-level composite type (`type t is record(a …, b …)`)

## BOOLEAN

`BOOLEAN` is bound like any scalar — the *same* bind carries `IN`, `OUT` and
`IN OUT` traffic: assign to the placeholder in PL/SQL and the host variable
comes back updated. For `BOOLEAN` **columns** (23ai), use the integer getter
(`OCI_CDT_BOOLEAN` values are `OCI_Boolean` in the row buffer).

| C function | C++ method | purpose |
|------------|------------|---------|
| `OCI_StatementBindBoolean` | `Bind(":b", v)` | bool param, any direction |
| `OCI_ResultsetGetInt32` | `Get<Int32>` | read a `BOOLEAN` column (0/1) |

:::sample
```c
OCI_Boolean ok = FALSE;

OCI_StatementPrepare(st, OTEXT("begin :b := check_employee(:e); end;"));
OCI_StatementBindBoolean(st, OTEXT(":b"), &ok);
OCI_StatementBindInt32  (st, OTEXT(":e"), &empno);
OCI_StatementExecutePrepared(st);          /* ok now holds the result */
```

```cpp
bool ok = false;

st.Prepare("begin :b := check_employee(:e); end;");
st.Bind(":b", ok);
st.Bind(":e", empno);
st.ExecutePrepared();                      /* ok now holds the result */
```
:::

## RECORD

A PL/SQL `RECORD` is a *package-level* named type — OCILIB models it as an
`OCI_Object`, exactly like a schema UDT. Describe it by its **fully-qualified**
name `schema.package.type` (unqualified names describe schema objects only),
create the object, fill its attributes by name, bind it.

| C function | C++ method | purpose |
|------------|------------|---------|
| `OCI_TypeInfoGet` (`OCI_TIF_TYPE`) | `TypeInfo` ctor | describe the record |
| `OCI_ObjectCreate` | `Object` ctor | instantiate it |
| `OCI_ObjectSet{Type}` / `OCI_ObjectGet{Type}` | `Set<T>` / `Get<T>` | attribute access |
| `OCI_StatementBindObject` | `Bind(":r", rec)` | pass it to PL/SQL |

:::sample
```c
OCI_TypeInfo *ti  = OCI_TypeInfoGet(con, OTEXT("hr.emp_pkg.t_emp_rec"), OCI_TIF_TYPE);
OCI_Object   *rec = OCI_ObjectCreate(ti);

OCI_ObjectSetInt32 (rec, OTEXT("EMPNO"), 7788);
OCI_ObjectSetString(rec, OTEXT("ENAME"), OTEXT("SCOTT"));

OCI_StatementPrepare(st, OTEXT("begin emp_pkg.promote(:r, :pct); end;"));
OCI_StatementBindObject(st, OTEXT(":r"), rec);
OCI_StatementExecutePrepared(st);

OCI_ObjectFree(rec);   /* caller-created — you free it */
```

```cpp
Object rec(TypeInfo(con, "hr.emp_pkg.t_emp_rec", TypeInfo::Type));

rec.Set<Int32>("EMPNO", 7788);
rec.Set<String> ("ENAME", "SCOTT");

st.Prepare("begin emp_pkg.promote(:r, :pct); end;");
st.Bind(":r", rec);
st.ExecutePrepared();            /* rec is RAII — no free needed */
```
:::

Records nest cleanly — a field can itself be another record, a PL/SQL table, or
a schema UDT (`OCI_ObjectGetObject`, `OCI_ObjectGetColl`…, same `Get<T>`/`Set<T>`
in C++). `OCI_TypeInfoGetColumn` introspects the record's fields exactly like a
UDT's attributes.

## And user-defined types?

`CREATE TYPE` objects (UDTs) are schema-level named types — SQL columns *can*
hold them. They share the same `OCI_Object` machinery as records: see
[user-defined types](user-defined-types.html) next, then Part 6 for
collections and REFs.

## Putting it together — the typical sequence

PL/SQL types follow the bind path, not the fetch path:

1. **BOOLEAN parameter**: `StatementBindBoolean` — the *same* bind carries IN,
   OUT, and IN OUT traffic. Set the variable before `Execute` for IN, read it
   after for OUT.
2. **BOOLEAN column (23ai)**: fetch with the *integer* getters —
   `GetInt32`/`Get<Int32>` — because the wire value is `OCI_Boolean` (an int).
   There is no `GetBoolean` on a resultset.
3. **RECORD parameter**: describe the package type with `TypeInfoGet` —
   *fully qualified* (`schema.pkg.t_rec`) — create an `Object`, `Set` its
   fields by name, bind it with `BindObject`. Records never appear as
   resultset columns; they only flow through PL/SQL.

Hints: the `TypeInfoGet` call must be qualified — `pkg.type`, not just `type`;
a `BindBoolean` bound to a non-BOOLEAN param fails at execute time, not bind
time; and remember `RECORD` is package-scoped — a top-level `CREATE TYPE`
that's a *record* is a UDT, and belongs to the next page.

## Try it yourself

1. Declare a package `record` with `NUMBER`/`VARCHAR2`/`BOOLEAN` fields; call a
   procedure taking it as `IN OUT` — set it, call, read it back.
2. `OCI_TypeInfoGet` the record, loop `GetColumnCount`/`GetColumn` and dump each
   field's `OCI_ColumnGetSqlType`.
3. Return `BOOLEAN` from a function and print it — compare with an `Int32` bind.

## Common pitfalls

- **Describing `pkg.type` without the schema** — `OCI_TypeInfoGet` resolves
  `schema.object`, so `"pkg.t_rec"` misses; pass `"schema.pkg.t_rec"`.
- **`OCI_StatementBindBoolean` in plain SQL** — `BOOLEAN` binds are for PL/SQL
  parameters; a `BOOLEAN` *column* in a SELECT needs `OCI_ResultsetGetInt32`.
- **Expecting `OCI_ResultsetGetRecord`** — records never appear as columns;
  they arrive as `OCI_Object` via binds, ref-cursor attrs, or OUT params.

## Recap

- PL/SQL `BOOLEAN` = scalar bind (`OCI_StatementBindBoolean`); `BOOLEAN` columns
  (23ai) read via `OCI_ResultsetGetInt32`.
- PL/SQL `RECORD` = an `OCI_Object` described by `OCI_TypeInfoGet` on
  `schema.pkg.type`, filled by attribute, bound with `OCI_StatementBindObject`.
- UDTs share the same object machinery — covered in Part 6.

**Next:** checkpoint — a schema that exercises every type from Part 2.
