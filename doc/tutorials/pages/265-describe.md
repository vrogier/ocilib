---
title: Describing SQL without executing it
navtitle: Describing SQL
part: 2
level: intermediate
---

# Describing SQL without executing it

**Objectives** — by the end of this page you will be able to:

- get column metadata for a query without fetching a single row
- tell `OCI_Parse`, `OCI_Describe` and `OCI_Prepare` apart
- build "preview" / schema-inspection features

## Concepts

**Describing** a statement asks the server to parse the SQL and hand back the *shape*
of its result — columns, types, sizes — without executing a single row:

:::sample
```c
OCI_Describe(st, OTEXT("select empno, ename, sal from employees where deptno = :d"));

OCI_Resultset *rs = OCI_GetResultset(st);     /* metadata-only resultset */
for (OCI_UInt32 i = 1; i <= OCI_GetColumnCount(rs); i++)
{
    OCI_Column *col = OCI_GetColumn(rs, i);
    printf("%-15s %s\n", OCI_ColumnGetName(col), OCI_ColumnGetSQLType(col));
}
```

```cpp
st.Describe("select empno, ename, sal from employees where deptno = :d");

Resultset rs = st.GetResultset();           /* metadata-only */
for (UInt32 i = 1; i <= rs.GetColumnCount(); i++)
    std::cout << rs.GetColumn(i).GetName() << " "
              << rs.GetColumn(i).GetSQLType() << "\n";
```
:::

## Parse vs Describe vs Prepare

| C call | C++ method | Server parse | Exec plan | Column metadata | Rows |
|--------|------------|:---:|:---:|:---:|:---:|
| `OCI_Parse` | `Parse` | ✓ | — | — | — |
| `OCI_Describe` | `Describe` | ✓ | ✓ | ✓ | — |
| `OCI_Prepare` + `OCI_Execute` | `Prepare` + `ExecutePrepared` | ✓ | ✓ | ✓ | ✓ |

- **Parse** is a syntax check and a way to list the placeholders found.
- **Describe** adds the *describe information* — the metadata resultset you inspect
  above.
- **Prepare** readies execution; describe-then-execute would re-parse — pick one.

## Where it shines

- **Generic query tools**: print a header row before fetching (name + width from the
  describe).
- **Validation**: reject non-SELECT or wrong-shaped queries before running them.
- **Bind discovery**: `GetParseBindNames` (C++) lists placeholders the server found —
  combine with `Describe` to auto-build input forms.

## Try it yourself

1. Describe `select * from employees` and render a `sqlplus`-style header.
2. Describe an `insert` statement — what does the column count look like?
3. Compare `OCI_Parse` vs `OCI_Describe` on a syntactically wrong query — which one
   surfaces `OCI_GetSqlErrorPos`?

## Common pitfalls

- **Fetching from a describe resultset** — there are no rows; `FetchNext` returns
  FALSE immediately.
- **Expecting execute-side info** — row counts, affected rows need `OCI_Execute`.
- **Binds in described SQL** — placeholders are fine (`:d` above), you just don't bind
  values for a describe-only flow.

## Recap

- `OCI_Describe` = parse + metadata, no rows; perfect for tooling and validation.
- `OCI_Parse` = syntax/bind check only; `OCI_Prepare`+`Execute` = the real thing.

**Next:** making large fetches fast — fetch size, prefetch, long mode.
