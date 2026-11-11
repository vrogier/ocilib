---
title: Connection pooling
part: 5
level: intermediate
demos: [pool.c, pool.cpp]
---

# Connection pooling

**Objectives** — by the end of this page you will be able to:

- create a connection or session pool with `OCI_Pool`
- check out and release connections correctly
- size a pool with min/max/increment

## The pool

Creating an Oracle session is expensive — network handshake, authentication, server process.

A **pool** keeps a set of sessions warm so your app *borrows* a connection instead of connecting: checkout is instant, release returns the session to the pool alive and ready for the next borrower.

You create it once on the environment with a type, a session mode and sizing, then hand out connections on demand:

:::sample
```c
OCI_Pool *pool = OCI_PoolCreate(
    env, db, usr, pwd,
    OCI_POOL_CONNECTION,       /* pool type          */
    OCI_SESSION_DEFAULT,       /* session mode       */
    2,                         /* min connections    */
    10,                        /* max connections    */
    1);                        /* grow by            */
```

```cpp
ocilib::Pool pool(env, db, usr, pwd,
                  Pool::ConnectionPool, /* pool type   */
                  Environment::SessionDefault,
                  2, 10, 1);            /* min, max, + */
```
:::

:::sample
```c
OCI_Connection *cn = OCI_PoolGetConnection(env, pool, NULL);  /* tag = NULL */
/* … work … */
OCI_ConnectionFree(cn);        /* returns to the pool — does NOT disconnect! */

OCI_PoolFree(env, pool);       /* closes all pooled sessions */
```

```cpp
{
    auto cn = pool.GetConnection();       /* check out */
    /* … work … */
}                                          /* released at scope exit */
// pool destructor closes the sessions
```
:::

## Two pool types

| C enum | C++ enum | What it pools | Pick when |
|--------|----------|---------------|-----------|
| `OCI_POOL_CONNECTION` | `Pool::ConnectionPool` | physical connections to the server | most apps — simple, server-agnostic |
| `OCI_POOL_SESSION` | `Pool::SessionPool` | server-side sessions (OCI session pool) | shared credentials + session tagging |

## Session tags — reuse the right session

Checkout accepts a **tag** — asking for a session *tagged* previously (session tags, page 120) brings back that exact session: temp tables, PL/SQL package state survive the checkout.

An empty tag means "any free session".

## Rules

- Freeing a pooled connection = **release**, not close. The same session can come
  back on the next checkout.
- Statements/resultsets die with the checkout as usual.
- The pool reports occupancy: `OCI_PoolGetBusyCount`/`GetOpenedCount` /
  `GetBusyCount()`/`GetOpenedCount()`.

## Putting it together — the typical sequence

Connection pooling replaces "connect per operation" with "borrow, use,
return":

1. **Create the pool** — `OCI_PoolCreate` with connect string, min, max,
   increment — the pool owns the real connections.
2. **Get a session** — `OCI_PoolGetConnection` borrows one (blocks or fails at
   max); `PoolGetConnectionSharded` adds sharding keys.
3. **Use it** — the borrowed `OCI_Connection` is a real session; run
   statements normally.
4. **Return it** — `OCI_ConnectionFree` on a pooled handle *returns* it to
   the pool, doesn't close it — that's the whole point.
5. **Free the pool** — `OCI_PoolFree` at shutdown drains the real
   connections.

Hints: `PoolGetConnection` blocks when the pool is empty — that's backpressure,
not a bug; returned sessions keep their state — clear session attributes you
set, or the next borrower inherits them; and pools are per-environment — one
pool per database service, not per query.

## Try it yourself

1. Check out 3 connections, print `GetBusyCount`/`GetOpenedCount` after each.
2. Set a session tag, release, re-checkout with the tag — prove state persisted.
3. Compare 100 sequential `OCI_ConnectionCreate` vs 100 `PoolGetConnection` timings.

## Common pitfalls

- **Freeing the pool while connections are out** — check `BusyCount` first or release
  them all.
- **Expecting `ConnectionFree` to close** — pooled sessions stay open by design.
- **Pool per request handler vs one global pool** — one pool per process, shared.

## Recap

- `OCI_PoolCreate(env, …)` → `OCI_PoolGetConnection(env, pool, tag)` → `OCI_PoolFree(env, pool)`
  releases.
- CONNECTION pools for speed, SESSION pools for stateful reuse via tags.

**Next:** tuning the pool — timeouts, no-wait, statement cache.
