---
title: Describing SQL without executing it
navtitle: Describing SQL
part: 3
level: intermediate
---

# Describing SQL without executing it

**Objectives** — by the end of this page you will be able to:

- get column metadata for a query without fetching a single row
- tell `OCI_StatementParse`, `OCI_StatementDescribe` and `OCI_StatementPrepare` apart
- build "preview" / schema-inspection features

## Concepts

**Describing** a statement asks the server to parse the SQL and hand back the *shape*
of its result — columns, types, sizes — without executing a single row:

:::sample
```c
OCI_StatementDescribe(st, OTEXT("select empno, ename, sal from employees where deptno = :d"));

OCI_Resultset *rs = OCI_StatementGetResultset(st);     /* metadata-only resultset */
for (OCI_UInt32 i = 0; i < OCI_ResultsetGetColumnCount(rs); ++i)
{
    OCI_Column *col = OCI_ResultsetGetColumn(rs, i);
    printf("%-15s %s\n", OCI_ColumnGetName(col), OCI_ColumnGetSqlType(col));
}
```

```cpp
st.Describe("select empno, ename, sal from employees where deptno = :d");

Resultset rs = st.GetResultset();           /* metadata-only */
for (UInt32 i = 0; i < rs.GetColumnCount(); ++i)
    std::cout << rs.GetColumn(i).GetName() << " "
              << rs.GetColumn(i).GetSQLType() << "\n";
```
:::

## Parse vs Describe vs Prepare

| C call | C++ method | Server parse | Exec plan | Column metadata | Rows |
|--------|------------|:---:|:---:|:---:|:---:|
| `OCI_StatementParse` | `Parse` | ✓ | — | — | — |
| `OCI_StatementDescribe` | `Describe` | ✓ | ✓ | ✓ | — |
| `OCI_StatementPrepare` + `OCI_StatementExecutePrepared` | `Prepare` + `ExecutePrepared` | ✓ | ✓ | ✓ | ✓ |

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

## Putting it together — the typical sequence

Describing asks the server "what is this object" — the schema-introspection
path:

1. **Call `OCI_Describe`** with the object name — table, view, procedure,
   sequence, type.
2. **Walk the returned metadata**: column list, types, sizes, nullability —
   the same `OCI_Column` attributes as a resultset describe.
3. **Use it for code generation** or dynamic SQL — any tool that adapts to
   schema at runtime starts here.

Hints: describe hits the data dictionary — cache the result if you call it in
a loop; `OCI_TIF_TABLE` vs `OCI_TIF_TYPE` selects table-vs-type — the same
function describes both; and describe answers "what the schema says", while
resultset metadata answers "what this query returned" — pick the right one
for the question you're asking.

## Try it yourself

1. Describe `select * from employees` and render a `sqlplus`-style header.
2. Describe an `insert` statement — what does the column count look like?
3. Compare `OCI_StatementParse` vs `OCI_StatementDescribe` on a syntactically wrong query — which one
   surfaces `OCI_StatementGetSqlErrorPos`?

## Common pitfalls

- **Fetching from a describe resultset** — there are no rows; `FetchNext` returns
  FALSE immediately.
- **Expecting execute-side info** — row counts, affected rows need `OCI_StatementExecutePrepared`.
- **Binds in described SQL** — placeholders are fine (`:d` above), you just don't bind
  values for a describe-only flow.

## Recap

- `OCI_StatementDescribe` = parse + metadata, no rows; perfect for tooling and validation.
- `OCI_StatementParse` = syntax/bind check only; `OCI_StatementPrepare`+`Execute` = the real thing.

**Next:** making large fetches fast — fetch size, prefetch, long mode.
