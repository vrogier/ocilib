---
title: Transactions — commit, rollback, autocommit
navtitle: Transactions
part: 5
level: beginner
---

# Transactions — commit, rollback, autocommit

**Objectives** — by the end of this page you will be able to:

- control when DML becomes permanent
- use `OCI_ConnectionSetAutoCommit` deliberately
- choose the right transaction boundary for batch work

## Concepts

Every DML your connection runs lives inside **its** transaction — invisible to other
sessions until you decide. The connection carries the whole state; three verbs
control it:

| C function | C++ method | Effect |
|------------|------------|--------|
| `OCI_ConnectionCommit` | `Commit()` | make pending changes permanent |
| `OCI_ConnectionRollback` | `Rollback()` | discard pending changes |
| `OCI_ConnectionSetAutoCommit` | `SetAutoCommit` | auto-commit after each execute |
| `OCI_ConnectionGetAutoCommit` | `GetAutoCommit()` | current flag |

:::sample
```c
OCI_StatementExecute(st, OTEXT("update employees set comm = 0 where deptno = 30"));
OCI_ConnectionCommit(cn);        /* permanent   */
/* or OCI_ConnectionRollback(cn); → undone      */
```

```cpp
st.Execute("update employees set comm = 0 where deptno = 30");
con.Commit();          /* permanent   */
/* or con.Rollback();  → undone      */
```
:::

## Default behavior

Connections start with **autocommit off** — nothing persists until you commit.

That's the safe default for applications; turn it on only for scratch/repl-style tools — every execute then commits itself.

## Transaction boundaries that work

- **Report/read-only code** — leave autocommit off; a single commit at the end (or
  none) is fine.
- **Bulk import** — autocommit off + one commit per batch (say every 10k rows) keeps
  undo manageable and throughput high.
- **All-or-nothing** — wrap the whole logical unit, commit at the end, rollback on
  any error:

:::sample
```c
if (!OCI_StatementExecute(st, OTEXT("update accounts set bal = bal - :amt where id = :src"))
 || !OCI_StatementExecute(st, OTEXT("update accounts set bal = bal + :amt where id = :dst")))
{
    OCI_ConnectionRollback(cn);          /* either both or neither */
}
else
{
    OCI_ConnectionCommit(cn);
}
```

```cpp
try
{
    st.Execute("update accounts set bal = bal - :amt where id = :src");
    st.Execute("update accounts set bal = bal + :amt where id = :dst");
    con.Commit();              /* both applied */
}
catch (...)
{
    con.Rollback();            /* either both or neither */
}
```
:::

!!! tip "DDL commits anyway"
    `create`/`alter`/`drop` implicitly commit in Oracle — a rollback can't undo them.

## Putting it together — the typical sequence

Transaction control is two calls and a mental model:

1. **Work** — any DML between `Connect` and `Commit`/`Rollback` is in one
   implicit transaction; OCILIB doesn't auto-commit.
2. **Commit** — `OCI_ConnectionCommit` makes it permanent.
3. **Or rollback** — `OCI_ConnectionRollback` undoes everything since the last commit.
4. **Set the mode** — `SetAutoCommit` when you want per-statement commits
   (rare in real apps; it kills batching).

Hints: OCILIB defaults to *manual* commit — forgetting `Commit` loses work
silently on disconnect; `AutoCommit` on is the demo convenience that becomes
a production footgun — one DML per commit is fine for a config table, a
disaster for a bulk load; and `Rollback` undoes *everything* pending — you
can't partially commit mid-transaction.

## Try it yourself

1. Insert a row, open a *second* session and `select` — nothing. Commit, re-select —
   now it's there.
2. Turn autocommit on and repeat: appears immediately.
3. Run the all-or-nothing transfer with a deliberate failure on the second update.

## Common pitfalls

- **"Where did my rows go?"** — forgot the commit before closing.
- **Autocommit on during batch loops** — each row = one commit = slow + redo churn.
- **Partial work visible** — committing between logically-linked statements.

## Recap

- `Commit`/`Rollback` decide; `SetAutoCommit` picks per-execute vs manual.
- Group logical units; commit once at a real boundary.

**Next:** explicit `OCI_Transaction` objects for finer control.
