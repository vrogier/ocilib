---
title: Checkpoint 1 — build a query console
navtitle: Checkpoint 1 query console
part: 2
level: intermediate
---

# Checkpoint 1 — build a query console

You should now be able to run the full statement cycle: prepare → bind → execute →
fetch → getters, plus RETURNING, scrollable mode and piecewise LONGs.

## The exercise — `qtool`

Write a small program that works like a pocket `sqlplus`:

1. Connect with `OCILIB_DEMO_*` (or `argv[1..3]` overrides).
2. Loop reading a line of SQL from stdin until `quit`.
3. If the line starts with `:set fetchsize N`, call `OCI_SetFetchSize(st, N)`.
4. Otherwise execute it:
   - for `select`/`with`/`describe`-able statements: print a header using
     `OCI_GetColumnCount` + `OCI_ColumnGetName`/`GetSize`, then rows with
     `OCI_GetString` on every column (universal getter fallback);
   - for DML/DDL: print `OCI_GetSQLVerb` + `OCI_GetAffectedRows`.
5. Report errors through your handler and keep looping — the tool must not die on
   `ORA-00942`.

### Stretch goals

- `:scroll` command toggles `OCI_SFM_SCROLLABLE` so `OCI_FetchPrev` works on the last
  resultset.
- `:returning` runs `update employees set sal=sal returning empno into :e` with
  `OCI_RegisterInt` and lists returned keys.
- Time each statement and print the elapsed ms after `GetAffectedRows`.

## What it exercises

| Skill | Pages |
|-------|-------|
| env + connection + error handler | 060–080 |
| Execute vs Prepare, affected rows, SQL verb | 210 |
| Column metadata, header rendering | 225/240 |
| Generic getters, NULLs | 225/230 |
| Fetch tuning | 270 |
| Scrollable, RETURNING | 275/280 |

## Suggested acceptance checks

```text
> select ename, sal from employees where deptno = 20
ENAME       SAL
JONES      2975
FORD       3000
2 row(s)

> update employees set comm = 0 where comm is null
UPDATE -> 5 row(s)

> select * from nope
ORA-00942: table or view does not exist
```

If it runs without crashing on bad SQL, prints a formatted table, and survives a
`quit` — **Part 2 is done**. Onward to PL/SQL.
