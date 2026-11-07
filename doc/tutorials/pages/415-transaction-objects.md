---
title: OCI_Transaction — explicit transaction objects
navtitle: Transaction objects
part: 4
level: advanced
---

# The transaction object — explicit units of work

**Objectives** — by the end of this page you will be able to:

- create named transaction objects with modes and timeouts
- attach/detach transactions to connections
- resume suspended work

## Meet the transaction object

Commit and rollback operate on whatever transaction is *current* on the connection.

A **transaction object** gives that unit of work a handle — so it can have a timeout, an isolation mode, a name, be suspended and resumed, or be part of a distributed branch.

You create it on a connection, make it the connection's current transaction, and drive it through its lifecycle.

:::sample
```c
OCI_Transaction *tx = OCI_TransactionCreate(
    cn,                 /* connection          */
    60,                 /* timeout (seconds)   */
    OCI_TRS_NEW,        /* mode                */
    NULL);              /* OCI_XID* for distributed tx (XA-like) */
```

```cpp
ocilib::Transaction tx(con, 60, Transaction::New);
```
:::

| C function | C++ method | Purpose |
|------------|------------|---------|
| `OCI_TransactionStart` | `Start()` | begin the transaction |
| `OCI_TransactionStop` | `Stop()` | end it (commit semantics) |
| `OCI_TransactionPrepare` | `Prepare()` | 2-phase commit: prepare phase |
| `OCI_TransactionForget` | `Forget()` | forget a prepared branch |
| `OCI_TransactionResume` | `Resume()` | re-attach a suspended tx |
| `OCI_TransactionGetMode` | `GetMode()` | current mode |
| `OCI_TransactionGetTimeout` | `GetTimeout()` | configured timeout |
| `OCI_SetTransaction` | `Connection::SetTransaction` | make it the connection's current tx |
| `OCI_GetTransaction` | `Connection::GetTransaction` | current tx |
| `OCI_TransactionFree` | — (RAII) | release the handle |

## Mode flags

| C enum | C++ enum | Use |
|--------|----------|-----|
| `OCI_TRS_NEW` | `Transaction::New` | fresh transaction (the usual one) |
| `OCI_TRS_READONLY` | `Transaction::ReadOnly` | read-consistent transaction |
| `OCI_TRS_READWRITE` | `Transaction::ReadWrite` | explicit read-write |
| `OCI_TRS_SERIALIZABLE` | `Transaction::Serializable` | serializable isolation |
| `OCI_TRS_LOOSE` | `Transaction::Loose` | loosely-coupled distributed branch |
| `OCI_TRS_TIGHT` | `Transaction::Tight` | tightly-coupled distributed branch |

## Typical flow

:::sample
```c
OCI_Transaction *tx = OCI_TransactionCreate(cn, 0, OCI_TRS_NEW, NULL);
OCI_SetTransaction(cn, tx);
OCI_TransactionStart(tx);

OCI_ExecuteStmt(st, OTEXT("update employees set sal = sal * 1.1 where deptno = 20"));

OCI_TransactionStop(tx);      /* commit through the transaction */
OCI_TransactionFree(tx);
```

```cpp
Transaction tx(con, 0, Transaction::New);
con.SetTransaction(tx);
tx.Start();

st.Execute("update employees set sal = sal * 1.1 where deptno = 20");

tx.Stop();                    /* commit through the transaction */
```
:::

## When you actually need this

- **Named/timed transactions** — a transaction that must expire (`timeout` param).
- **Suspend/resume** — detach work, run something else, `Resume` later.
- **Distributed transactions** — `OCI_XID*` branches (the pxid arg).
- Everyday apps are fine with `Commit`/`Rollback` — this page is for the 5%.

## Try it yourself

1. Start a read-only transaction, run selects, stop — confirm no writes slip in.
2. Start, suspend by detaching (set the connection's transaction to none), run an
   unrelated statement, resume and commit.

## Recap

- The transaction object = a handle on the unit of work: start/stop/prepare/resume,
  attached to a connection as its current transaction.
- Use it for timeouts, isolation modes, suspend/resume and distributed branches.

**Next:** connection pooling — many sessions without reconnect costs.
