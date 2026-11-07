---
title: PL/SQL index-by tables — array parameters
navtitle: PL/SQL tables
part: 3
level: intermediate
demos: [plsql_table.c]
sql: plsql_table.sql
---

# PL/SQL index-by tables — array parameters

**Objectives** — by the end of this page you will be able to:

- pass C arrays into `TABLE OF …` PL/SQL parameters
- receive arrays back with OUT direction
- use `OCI_BindArrayOf*` for PL/SQL table types

## Setup (from `demo/plsql_table.sql`)

```sql
create or replace package test_plsqltables as
    type t_tab_names  is table of varchar2(50) index by binary_integer;
    type t_tab_values is table of varchar2(50) index by binary_integer;
    procedure combine_values
        (p_names in t_tab_names, p_values in t_tab_values,
         p_results out t_tab_values);
end;
/
```

## Binding table parameters

A `TABLE OF varchar2 INDEX BY …` parameter maps to **an array of buffers** bound
with the array-bind family — element capacity + count go in the bind call, and the
direction flag marks which tables come back:

:::sample
```c
#define SIZE_NAME  50
#define SIZE_VALUE 50
#define COUNT      4

otext tab_names [COUNT][SIZE_NAME];
otext tab_values[COUNT][SIZE_VALUE];
otext tab_out   [COUNT][SIZE_VALUE];

OCI_Prepare(st, OTEXT("begin test_plsqltables.combine_values"
                      "(:tab_names, :tab_values, :tab_out); end;"));

OCI_BindArrayOfStrings(st, OTEXT(":tab_names"),  (otext*)tab_names,  SIZE_NAME,  COUNT);
OCI_BindArrayOfStrings(st, OTEXT(":tab_values"), (otext*)tab_values, SIZE_VALUE, COUNT);
OCI_BindArrayOfStrings(st, OTEXT(":tab_out"),    (otext*)tab_out,    SIZE_VALUE, COUNT);

/* directions: first two IN, third OUT */
OCI_BindSetDirection(OCI_GetBind(st, 1), OCI_BDM_IN);
OCI_BindSetDirection(OCI_GetBind(st, 2), OCI_BDM_IN);
OCI_BindSetDirection(OCI_GetBind(st, 3), OCI_BDM_OUT);

OCI_Execute(st);        /* tab_out[][] now holds the combined strings */
```

```cpp
std::vector<std::string> names, values, out(COUNT);
st.Prepare("begin test_plsqltables.combine_values(:tab_names, :tab_values, :tab_out); end;");
st.Bind(":tab_names",  names,  BindInfo::In,  BindInfo::AsPlSqlTable);
st.Bind(":tab_values", values, BindInfo::In,  BindInfo::AsPlSqlTable);
st.Bind(":tab_out",    out,    BindInfo::Out, BindInfo::AsPlSqlTable);
st.ExecutePrepared();
```
:::

Numeric tables bind plain C/vector arrays — no element-size arg needed:

:::sample
```c
OCI_Int32 ids[64];
OCI_BindArrayOfInts(st, OTEXT(":ids"), ids, 64);
```

```cpp
std::vector<Int32> ids(64);
st.Bind(":ids", ids, BindInfo::In, BindInfo::AsPlSqlTable);
```
:::

## Rules that differ from array DML binds

- The **size arg** in the array-bind call is the table element count; for string
  arrays you also give the per-element buffer size.
- OUT tables: the bound array must be big enough to receive every element the block
  writes.
- Direction is per-bind, not per element.
- C++ binds index-by tables with `BindInfo::AsPlSqlTable` (vs `AsArray` for SQL
  array binds, page 810) — and `SetBindArraySize` is **not** used for PL/SQL tables.

## Try it yourself

1. Run `plsql_table.sql` + the sample; print `tab_out`.
2. Pass an `in out` table and modify two elements server-side.
3. Bind a `table of number` parameter with `OCI_BindArrayOfInts`.

## Common pitfalls

- **`SetBindArraySize` on PL/SQL tables** — the docs warn against it; sizing comes
  from the bind call itself.
- **Too-small OUT buffer** — writes beyond capacity error or truncate.
- **Confusing `AsArray` vs `AsPlSqlTable`** — same C++ `Bind` overload, different
  server semantics (SQL collections vs PL/SQL index-by tables).

## Recap

- `TABLE OF … INDEX BY` params = `OCI_BindArrayOf*` + `OCI_BDM_*` direction flags.
- C++: `vector` + `BindInfo::AsPlSqlTable`.

**Next:** `DBMS_OUTPUT` — printing server-side text from PL/SQL.
