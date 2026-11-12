---
title: The statement lifecycle
part: 3
level: beginner
demos: [exec.c]
---

# The statement lifecycle

**Objectives** — by the end of this page you will be able to:

- create, prepare, execute and free a statement
- choose between prepare-then-execute, one-shot execution, and immediate scalars
- read statement metadata: SQL verb, statement type, affected rows, error position
- use the `*Fmt` printf-style variants

## The statement

A **statement** is the work slot where SQL happens.

Every statement lives on a connection and carries three things: the **SQL text** (and its server-side parse context — that's why reusing a statement is fast), the **bind slots** for its variables, and one **current resultset** at a time — the rows your last `SELECT` produced (a queue of further resultsets can sit behind it — see *several resultsets* below).

One statement = one active SQL at a time.

Running a new SQL on the same handle replaces the old text and resultset — statements are meant to be *reused*, not allocated per query.

Freeing it destroys SQL context, binds and resultset together; freed or not, it dies with its connection.

!!! warning "No `;` at the end — unlike SQL*Plus"
    SQL text passed to `Prepare`/`Execute` must **not** end with `;`. In
    SQL*Plus the `;` is a *tool-level* terminator telling the client to send
    the statement — it is not part of the SQL. OCILIB hands your string
    verbatim to the server, so `select 1 from dual;` is a syntax error
    (`ORA-00911: invalid character`). Drop the `;` (and the `/` on PL/SQL
    blocks, see the PL/SQL pages).

## The four verbs of execution

There are three ways to run SQL — plus a scalar shortcut:

| C function | C++ method | What it does | Use when |
|------------|------------|--------------|----------|
| `OCI_StatementParse` | `Parse` | asks the **server** to parse without executing — one round-trip | syntax-check or describe without running |
| `OCI_StatementPrepare` | `Prepare` | registers the SQL **locally** — no round-trip; the server parse is deferred to execute | you will execute **repeatedly** or bind |
| `OCI_StatementExecutePrepared` | `ExecutePrepared` | runs the prepared statement | after `Prepare` |
| `OCI_StatementExecute` | `Execute` | prepare + execute in one shot | one-off statements, DDL |
| `OCI_StatementExecuteAndGetResultset` | `ExecuteAndGetResultset` | execute + return the resultset directly | SELECT shortcuts |
| `OCI_ConnectionImmediate` | — | run SQL *without* a statement handle, outputs straight into variables | `select count(*)`-style scalars |

:::sample
```c
OCI_UInt32 n = 0;
OCI_ConnectionImmediate(cn, OTEXT("select count(*) from employees"), OCI_ARG_UINT32, &n);
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
    `OCI_StatementPrepareFormat`, `OCI_StatementExecuteFormat`, `OCI_ConnectionImmediateFormat`, `OCI_StatementParseFormat` and
    `OCI_StatementDescribeFormat` accept `printf`-style SQL:
    `OCI_StatementExecuteFormat(st, OTEXT("insert into t values(%d, '%s')"), 1, "x")`.
    Convenient — but prefer **bind variables** for anything user-supplied (page 245).

## Reading what happened

After execution, the statement reports what the server did with it:

| C function | C++ method | Returns |
|------------|------------|---------|
| `OCI_StatementGetSql` | `GetSql()` | the current SQL text |
| `OCI_StatementGetSqlVerb` | `GetSQLVerb()` | `"SELECT"`, `"INSERT"`, … |
| `OCI_StatementGetSqlCommand` | `GetSQLCommand()` | OCI command code |
| `OCI_StatementGetType` | `GetStatementType()` | statement class enum |
| `OCI_StatementGetSqlIdentifier` | `GetSqlIdentifier()` | shared-cursor hash id |
| `OCI_StatementGetSqlErrorPos` | `GetSqlErrorPos()` | parse-error byte offset in the SQL |
| `OCI_StatementGetParseBindNames` | `GetParseBindNames()` | bind variable names found in the SQL |
| `OCI_StatementGetAffectedRows` | `GetAffectedRows()` | rows touched by DML |

:::sample
```c
OCI_StatementExecute(st, OTEXT("update employees set sal = sal * 1.05 where deptno = 20"));
printf("%s -> %u row(s)\n", OCI_StatementGetSqlVerb(st), OCI_StatementGetAffectedRows(st));
```

```cpp
st.Execute("update employees set sal = sal * 1.05 where deptno = 20");
std::cout << st.GetSQLVerb() << " -> " << st.GetAffectedRows() << " row(s)\n";
```
:::

## Which calls reach the server?

The verbs differ mostly in round-trips: `Prepare` is local, `Parse`/`Describe`/
`Execute` each cost one, and `FetchNext` only crosses the wire when its buffers
run dry. For the full map across the API — connections, transactions, LOBs,
describes — see [server round-trips](round-trips.html).

## One statement, several resultsets

A statement is not limited to one resultset: some executions produce a
**queue** of them. `GetResultset` hands you the first, `GetNextResultset`
each following one, until NULL.

Two cases generate that queue:

| Case | How it happens | Details |
|------|----------------|---------|
| `DML … RETURNING INTO` + array binds | Oracle runs one iteration per array element — each iteration yields a resultset holding that iteration's returned values (scalar binds → a single resultset) | [ROWID & RETURNING](rowid-returning.html) |
| Implicit resultsets (12c+) | a PL/SQL block calls `DBMS_SQL.RETURN_RESULT` — every pushed cursor arrives as another resultset on the same statement | [implicit resultsets](implicit-resultsets.html) |

Not to be confused with `REF CURSOR` out-params or `CURSOR` columns: those
return *statement* handles you execute separately — each with its own single
resultset — not a queue on one statement. See [REF CURSORs](ref-cursors.html).

Whatever produced the queue, the walking pattern is the same:

:::sample
```c
for (OCI_Resultset *rs = OCI_StatementGetResultset(st);
     NULL != rs;
     rs = OCI_StatementGetNextResultset(st))
{
    while (OCI_FetchNext(rs))
    {
        /* rows of this resultset */
    }
}
```

```cpp
for (ocilib::Resultset rs = st.GetResultset(); rs; rs = st.GetNextResultset())
{
    while (rs.Next())
    {
        /* rows of this resultset */
    }
}
```
:::

## Putting it together — the typical sequence

Every SQL statement follows the same five-phase lifecycle — internalize it and
every later page clicks:

1. **Create** — `OCI_StatementCreate(cn)` gives an empty handle; `Statement
   st(con)` in C++.
2. **Prepare** — `OCI_StatementPrepare(st, sql)` registers the SQL on the
   client — no round-trip yet; the server parse is deferred and bundled into
   the first execute.
3. **Bind** (if placeholders) — `StatementBind*` by `:name`; bind variables
   exist between prepare and execute.
4. **Execute** — `ExecutePrepared` for already-prepared statements; fetch
   afterward if it returned a resultset.
5. **Free** — `OCI_StatementFree` in C; RAII in C++.

Shortcuts exist at each phase (`OCI_StatementExecute` = prepare+execute,
`OCI_ExecuteFormat`/`ImmediateFormat` = the whole pipeline) — but the long
form is what you reach for when a statement runs more than once, because the
prepare cost is paid exactly once.

Hints: preparing is server-side parse — re-executing a prepared statement is
cheap, re-preparing isn't; a `SELECT` gives you a resultset *after* execute,
not before — `GetResultset` on a non-SELECT returns NULL.

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
