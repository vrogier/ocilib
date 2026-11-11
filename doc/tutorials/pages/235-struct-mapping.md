---
title: Mapping rows into C structs
part: 3
level: intermediate
demos: [fetch_struct.c]
---

# Mapping rows into C structs

**Objectives** — by the end of this page you will be able to:

- fetch a whole row into a C `struct` in one call with `OCI_ResultsetGetStruct`
- write the companion *indicator struct* that captures NULLs
- control how NUMBER columns map to C numeric types

## Concepts

Struct mapping is a **C-only** convenience (the C++ side has `Get<T>` instead): one
call copies the whole current row into *your* `struct`. Two companion structs do
the work:

1. **Row struct** — one member per result-set column; **member names must match the
   column names** (as Oracle returns them — usually upper case… keep reading for the
   practical trick). Member types:
   - character columns → `otext*` (a borrowed pointer — copy if you keep it)
   - numeric columns → the C type you declare via `OCI_ResultsetSetStructNumericType`
   - DATE/TIMESTAMP/LOB/OBJECT… → the matching `OCI_*` handle pointer
2. **Indicator struct** — one `OCI_Boolean` member per column, same order;
   `TRUE` = the value is NOT NULL. Mandatory — this is how NULLs are reported.

## Example — employee row → struct

```c
typedef struct
{
    OCI_Int32  empno;
    otext     *ename;          /* char* on ANSI builds — a borrowed pointer */
    OCI_Float  sal;
    OCI_Float  comm;
} employee_t;

typedef struct
{
    OCI_Boolean empno;
    OCI_Boolean ename;
    OCI_Boolean sal;
    OCI_Boolean comm;
} employee_ind_t;
```

```c
OCI_StatementExecute(st,
    OTEXT("select empno, ename, sal, comm from employees order by empno"));
OCI_Resultset *rs = OCI_StatementGetResultset(st);

/* tell OCILIB how NUMBER maps to the C members */
OCI_ResultsetSetStructNumericType(rs, 1, OCI_NUM_INT32);
OCI_ResultsetSetStructNumericType(rs, 3, OCI_NUM_FLOAT);
OCI_ResultsetSetStructNumericType(rs, 4, OCI_NUM_FLOAT);
/* …or by name: OCI_ResultsetSetStructNumericTypeByName(rs, OTEXT("SAL"), OCI_NUM_FLOAT); */

employee_t     e;
employee_ind_t ind;
while (OCI_ResultsetFetchNext(rs))
{
    OCI_ResultsetGetStruct(rs, &e, &ind);
    printf("%d %-10s %8.2f", e.empno, e.ename, e.sal);
    if (!ind.comm)            /* indicator FALSE → column was NULL */
    {
        printf("  (no comm)");
    }
    printf("\n");
}
```

## The `OCI_NUM_*` mapping flags

| Flag | struct member should be |
|------|------------------------|
| `OCI_NUM_INT16` / `OCI_NUM_UINT16` | `OCI_Int16` / `OCI_UInt16` |
| `OCI_NUM_INT32` / `OCI_NUM_UINT32` | `OCI_Int32` / `OCI_UInt32` |
| `OCI_NUM_INT64` / `OCI_NUM_UINT64` | `OCI_Int64` / `OCI_UInt64` |
| `OCI_NUM_FLOAT` / `OCI_NUM_DOUBLE` | `OCI_Float` / `OCI_Double` |
| `OCI_NUM_NUMBER` | `OCI_Number*` (exact decimal — see Part 2) |

Call `OCI_ResultsetSetStructNumericType` once per numeric column **before** the fetch loop.

!!! tip "Member names vs column names"
    OCILIB matches struct members to columns **by name**. Since Oracle upper-cases
    unquoted identifiers, either name your members `EMPNO`-style or alias the columns
    to match (`select empno "empno" …`).

## When to use it

- Fixed, known column shapes — one call per row instead of N getters.
- Not for dynamic SQL — the layout is compile-time.
- No direct C++ equivalent: `Row`/`Get<T>` is the idiomatic C++ way.

## Putting it together — the typical sequence

Struct mapping turns a row into a C struct — the workflow:

1. **Define the struct** to mirror the SELECT list, field for field.
2. **Call `OCI_GetStruct`** (C only) with the resultset and the struct —
   OCILIB fills it by column position, using the type map implicitly.
3. **Handle NULLs** — an indicator array tells you which fields came back NULL
   vs actually filled.

Hints: the struct must match column *order and count* — OCILIB can't verify
your layout; `GetStruct` is C-only because C++ uses `Get<T>` + a struct
constructor instead — don't look for a `GetStruct` method on `Resultset`; and
indicator variables matter most when the schema has nullable columns —
`GetStruct` without indicators can't tell a real zero from a NULL.

## Try it yourself

1. Add `hiredate` (`OCI_Date*`) and `job` (`otext*`) to the struct and re-run.
2. Drop the `OCI_ResultsetSetStructNumericType` call for `sal` and watch the mismatch error —
   that's the API telling you it needs the hint.

## Common pitfalls

- **Dangling `ename` pointers** — string members point into the row buffer; copy with
  your string function if you store them.
- **Forgetting the indicator struct** — it is mandatory; NULL members are only
  reported there.
- **Wrong member count** — every column needs a struct member, in any order but by name.

## Recap

- `OCI_ResultsetGetStruct(rs, &row, &ind)` + `OCI_ResultsetSetStructNumericType` = row → struct.
- Indicator struct reports NULLs; string members are borrowed pointers.

**Next:** column metadata — `OCI_Column` attributes for building generic tools.
