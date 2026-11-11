---
title: Multithreading
part: 5
level: advanced
demos: [abort.c]
---

# Multithreading

**Objectives** — by the end of this page you will be able to:

- create a thread-safe environment (`OCI_ENV_THREADED`)
- state the object-sharing rules across threads
- use per-thread error contexts correctly

## Concepts

Oracle's client library is thread-capable, but you must *tell* the library your
program uses threads — the environment's threaded flag engages the internal
synchronization:

:::sample
```c
OCI_Environment *env = OCI_EnvironmentCreate(
    err_handler, NULL,
    OCI_ENV_THREADED);     /* enables internal synchronization */
```

```cpp
ocilib::Environment env(Environment::Threaded);
/* combine: Environment::Threaded | Environment::Events */
```
:::

| C enum | C++ enum | Meaning |
|--------|----------|---------|
| `OCI_ENV_DEFAULT` | `Environment::Default` | single-threaded app — no sync overhead |
| `OCI_ENV_THREADED` | `Environment::Threaded` | threads will call the API — mutexes engaged |
| `OCI_ENV_EVENTS` | `Environment::Events` | notification/event work (Part 7) |

## The sharing rules

| Object | Safe to share across threads? |
|--------|-------------------------------|
| environment | ✓ — it *is* the synchronization root |
| connection | ✓ — calls are serialized internally; still prefer one per thread for real parallelism |
| statement / resultset | ✗ — one thread at a time; hand it off, don't share |
| pool | ✓ — built for concurrent checkouts (this is what it's for) |

So the canonical threaded design: **one threaded environment + one pool + a
connection per worker thread** — each worker runs its own statements on its own
connection.

:::sample
```c
/* worker */
OCI_Connection *cn = OCI_PoolGetConnection(env, pool, NULL);   /* my own session */
OCI_Statement  *st = OCI_StatementCreate(cn);
OCI_StatementExecute(st, OTEXT("update …"));
OCI_StatementFree(st);
OCI_ConnectionFree(cn);                                    /* back to pool */
```

```cpp
/* worker */
auto cn = pool.GetConnection();            /* my own session */
Statement st(cn);
st.Execute("update …");
/* scope exit → statement + connection released back to the pool */
```
:::

## Errors per thread

Last-error is **thread-local** — each thread sees its own.

The error handler callback also fires on the calling thread. v5 internals use ISO C11 `<threads.h>` primitives (see `demo/abort.c` for a threaded break example).

!!! warning "Serialized is not parallel"
    Two threads executing on the *same* connection don't run in parallel — OCI
    serializes them. Parallel work needs parallel connections (the pool).

## Putting it together — the typical sequence

Threads + OCI is mostly about what's safe to share:

1. **Create the environment threaded** — `OCI_ENV_THREADED` at
   `EnvironmentCreate` is non-negotiable for multi-threaded use.
2. **One connection per thread** — connections are *not* thread-safe;
   share a pool, not a session.
3. **Use the OCILIB primitives** — `OCI_ThreadCreate`, `OCI_Mutex*`,
   `OCI_Condition*` for portable threading, or C++ `std::thread` + the same
   rules.
4. **`OCI_ThreadJoin`** for the join; `OCI_MutexAcquire`/`Release` for the
   shared state.

Hints: sharing a connection across threads serializes *and* corrupts — one
per thread, always; `OCI_ENV_THREADED` is checked at environment creation —
forgetting it is the classic crash-under-load bug; and OCILIB's own mutex/
condition are thin wrappers — `std::mutex` in C++ or `pthread` directly is
equally fine, the OCI side only cares about the environment flag.

## Try it yourself

1. Spawn 4 workers each checking out a pool connection and inserting — verify row
   count and no interleaved corruption.
2. Intentionally run two threads on one connection — watch serialized behavior.
3. Create the env **without** `OCI_ENV_THREADED` under contention — flaky errors.

## Common pitfalls

- **Forgetting the threaded flag** — intermittent corruption/crashes, worst kind.
- **Sharing statements across threads** — race on the cursor state.
- **Reading last-error on the wrong thread** — you see *that* thread's error.

## Recap

- Threaded environment upfront; env/pool shared, connections-per-worker, statements
  never shared.

**Checkpoint:** next page — put pool + threads + statements together.
