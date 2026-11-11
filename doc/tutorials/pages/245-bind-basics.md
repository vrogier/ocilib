---
title: Bind variables — the basics
navtitle: Bind variables
part: 3
level: beginner
demos: [bind.c, bind.cpp]
---

# Bind variables — the basics

**Objectives** — by the end of this page you will be able to:

- explain why binds beat string concatenation (safety **and** speed)
- bind scalar values with `:name` placeholders
- re-execute with different values without re-preparing

## Why binds

```sql
-- bad: string-built SQL
select * from employees where ename = 'O''BRIEN'
-- good: bind variable
select * from employees where ename = :name
```

- **Injection-proof** — values travel as data, never as SQL text.
- **Shareable cursors** — Oracle parses `:name` once and reuses the plan for every
  value; concatenated literals flood the shared pool with one-off SQL.
- **Typed** — a date goes in as a date, not as a string that must be re-parsed.

## The bind

A **bind** is the link between a `:name` placeholder in your SQL and a host variable in your program.

When you bind, you hand the statement a *pointer* — it reads (or writes) your variable's current contents at each execute, so the same prepared SQL serves every value you feed it.

Each statement keeps its set of binds; you can enumerate and inspect them (page 255).

The pattern is always:

1. Write SQL with `:name` placeholders.
2. Prepare the statement.
3. Bind your variable **by address** to the placeholder.
4. Execute — repeat 3-4 as values change; re-executing re-reads the variable.

:::sample
```c
int       empno = 7566;
otext     ename[31] = OTEXT("");
OCI_Float sal   = 0.f;

OCI_StatementPrepare(st, OTEXT("select ename, sal from employees where empno = :empno"));
OCI_StatementBindInt32(st, OTEXT(":empno"), &empno);

OCI_StatementExecutePrepared(st);                              /* reads &empno at execute time */
OCI_Resultset *rs = OCI_StatementGetResultset(st);
while (OCI_ResultsetFetchNext(rs))
{
    printf("%s %.2f\n", OCI_ResultsetGetString(rs, 0), OCI_ResultsetGetDouble(rs, 1));
}

empno = 7839;                                 /* same statement, new value */
OCI_StatementExecutePrepared(st);                              /* no re-bind needed */
```

```cpp
int empno = 7566;

st.Prepare("select ename, sal from employees where empno = :empno");
st.Bind(":empno", empno, BindInfo::In);       // bound by reference — keep it alive

st.ExecutePrepared();
Resultset rs = st.GetResultset();
while (rs++)
{
    std::cout << rs.Get<String>(0) << " " << rs.Get<double>(1) << "\n";
}

empno = 7839;
st.ExecutePrepared();                         // re-reads empno — no re-bind
```
:::

## The scalar bind family

In C there's one bind call per host type; in C++ a single templated `Bind` covers
them all — the direction argument defaults to input:

| C function | Bound type | C++ equivalent |
|------------|-----------|----------------|
| `OCI_StatementBindInt16` | `OCI_Int16*` | `Bind(":n", v, BindInfo::In)` |
| `OCI_StatementBindUInt16` | `OCI_UInt16*` | `Bind(":n", v, BindInfo::In)` |
| `OCI_StatementBindInt32` | `OCI_Int32*` | `Bind(":n", v, BindInfo::In)` |
| `OCI_StatementBindUInt32` | `OCI_UInt32*` | `Bind(":n", v, BindInfo::In)` |
| `OCI_StatementBindInt64` | `OCI_Int64*` | `Bind(":n", v, BindInfo::In)` |
| `OCI_StatementBindUInt64` | `OCI_UInt64*` | `Bind(":n", v, BindInfo::In)` |
| `OCI_StatementBindFloat` | `OCI_Float*` | `Bind(":n", v, BindInfo::In)` |
| `OCI_StatementBindDouble` | `OCI_Double*` | `Bind(":n", v, BindInfo::In)` |
| `OCI_StatementBindString` | `otext*` buffer + size | `Bind(":n", str)` |
| `OCI_StatementBindRaw` | `unsigned char*` + size | `Bind(":n", raw)` |
| `OCI_StatementBindBoolean` | `OCI_Boolean*` (23ai SQL BOOLEAN) | `Bind(":n", v, BindInfo::In)` |

The string bind takes your buffer's max size — Oracle uses it to bound OUT values
too.

## Rules that bite beginners

- The bound **variable must stay alive** until you're done executing — the statement
  keeps your pointer, not a copy.
- Re-executing reads the *current* contents — update the variable, don't re-bind.
- The statement can list the placeholders the server actually found
  (`OCI_StatementGetParseBindNames` / `GetParseBindNames()`) — handy to verify names/colons.

## Putting it together — the typical sequence

Binding is the three-step "fill a `:placeholder`" routine:

1. **Prepare first** — binds attach to a prepared statement's `:name`
   placeholders.
2. **Call the typed bind** — `StatementBindInt32(st, ":id", &var)`: statement,
   bind name, *address of* the variable.
3. **Execute** — OCILIB reads your variable at execute time, not bind time.

The key mechanics: the bind is **by reference** — OCILIB stores a pointer, not
a copy. Changing `var` between executes re-sends the new value without
re-binding, which is exactly what array binds exploit.

Hints: pass the *address* — `&var`, not `var`; the variable must stay alive
until the last execute (stack variables that go out of scope are the classic
dangling bind); and `IN` binds need a value *before* execute, `OUT` binds get
one *after* — the same call handles both.

## Try it yourself

1. Insert a row with three binds (`empno`, `ename`, `sal`), then `select` it back.
2. Loop over an array of `empno` values re-executing the same prepared SELECT.
3. Change `:empno` to `?`-style positional binds (OCI positional syntax) and compare.

## Common pitfalls

- **Stack variables dying** — binding a local, then executing after the function
  returns → garbage/or crash. Keep bound buffers alive.
- **Missing colon in the bind name** — `OCI_StatementBindInt32(st, OTEXT("empno"), …)` still
  resolves in practice, but use the `":name"` form for clarity.
- **Rebinding needlessly** — re-binding between Executes of the same prepare works,
  but wastes a call; just change the value.

## Recap

- `:name` placeholders + `Prepare` + `OCI_Bind*` + `Execute` — the secure fast path.
- OCILIB stores your *pointer*; keep the variable alive, update it between executes.

**Next:** every `OCI_StatementBindXxx` — dates, LOBs, objects, cursors and friends.
