---
title: The statement lifecycle
part: 2
level: beginner
demos: [exec.c]
---

# The statement lifecycle

**Objectives** — by the end of this page you will be able to:

- create, prepare, execute and free a statement
- choose between prepare-then-execute, one-shot execution, and immediate scalars
- read statement metadata: SQL verb, statement type, affected rows, error position
- use the `*Fmt` printf-style variants

## Meet the statement

A **statement** is the work slot where SQL happens.

Every statement lives on a connection and carries three things: the **SQL text** (and its server-side parse context — that's why reusing a statement is fast), the **bind slots** for its variables, and at most **one current resultset** — the rows your last `SELECT` produced.

One statement = one active SQL at a time.

Running a new SQL on the same handle replaces the old text and resultset — statements are meant to be *reused*, not allocated per query.

Freeing it destroys SQL context, binds and resultset together; freed or not, it dies with its connection.

## The four verbs of execution

There are three ways to run SQL — plus a scalar shortcut:

| C function | C++ method | What it does | Use when |
|------------|------------|--------------|----------|
| `OCI_Parse` | `Parse` | sends SQL to the server, parses only | syntax-check or describe without running |
| `OCI_Prepare` | `Prepare` | parse + allocate the execution context | you will execute **repeatedly** or bind |
| `OCI_Execute` | `ExecutePrepared` | runs the prepared statement | after `Prepare` |
| `OCI_ExecuteStmt` | `Execute` | prepare + execute in one shot | one-off statements, DDL |
| `OCI_ExecuteStmtAndGetResultset` | `ExecuteAndGetResultset` | execute + return the resultset directly | SELECT shortcuts |
| `OCI_Immediate` | — | run SQL *without* a statement handle, outputs straight into variables | `select count(*)`-style scalars |

:::sample
```c
OCI_UInt32 n = 0;
OCI_Immediate(cn, OTEXT("select count(*) from employees"), OCI_ARG_UINT, &n);
```

```cpp
/* no C++ twin — run the statement and read the scalar: */
st.Execute("select count(*) from employees");
UInt32 n = 0;
if (st.GetResultset().Next())
{
    n = st.GetResultset().Get<UInt32>(0);
}
```
:::

!!! tip "`*Fmt` variants — printf-style SQL"
    `OCI_PrepareFmt`, `OCI_ExecuteStmtFmt`, `OCI_ImmediateFmt`, `OCI_ParseFmt` and
    `OCI_DescribeFmt` accept `printf`-style SQL:
    `OCI_ExecuteStmtFmt(st, OTEXT("insert into t values(%d, '%s')"), 1, "x")`.
    Convenient — but prefer **bind variables** for anything user-supplied (page 245).

## Reading what happened

After execution, the statement reports what the server did with it:

| C function | C++ method | Returns |
|------------|------------|---------|
| `OCI_GetSql` | `GetSql()` | the current SQL text |
| `OCI_GetSQLVerb` | `GetSQLVerb()` | `"SELECT"`, `"INSERT"`, … |
| `OCI_GetSQLCommand` | `GetSQLCommand()` | OCI command code |
| `OCI_GetStatementType` | `GetStatementType()` | statement class enum |
| `OCI_GetSqlIdentifier` | `GetSqlIdentifier()` | shared-cursor hash id |
| `OCI_GetSqlErrorPos` | `GetSqlErrorPos()` | parse-error byte offset in the SQL |
| `OCI_GetParseBindNames` | `GetParseBindNames()` | bind variable names found in the SQL |
| `OCI_GetAffectedRows` | `GetAffectedRows()` | rows touched by DML |

:::sample
```c
OCI_ExecuteStmt(st, OTEXT("update employees set sal = sal * 1.05 where deptno = 20"));
printf("%s -> %u row(s)\n", OCI_GetSQLVerb(st), OCI_GetAffectedRows(st));
```

```cpp
st.Execute("update employees set sal = sal * 1.05 where deptno = 20");
std::cout << st.GetSQLVerb() << " -> " << st.GetAffectedRows() << " row(s)\n";
```
:::

## Try it yourself

1. Run a `create table`/`drop table` pair one-shot and print the verb.
2. Execute `"select * employees"` (missing `from`) and print the error position.
3. Rewrite the `count(*)` above with the `*Fmt` variant adding `where deptno=%d`.

## Common pitfalls

- **DDL commits implicitly** — Oracle auto-commits DDL; don't expect rollback there.
- **Forgetting to execute after preparing** — prepare only parses.
- **Reading affected-rows after a SELECT** — it's for DML; count rows on the
  resultset instead.

## Recap

- A statement is a reusable SQL slot on a connection: text + binds + one resultset.
- One-shot for single runs; prepare+execute for reuse; immediate for scalars.

**Next:** the read loop in detail — every way to get values out of a resultset.
