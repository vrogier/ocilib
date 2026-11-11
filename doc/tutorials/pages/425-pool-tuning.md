---
title: Pool tuning & monitoring
part: 5
level: intermediate
---

# Pool tuning & monitoring

**Objectives** — by the end of this page you will be able to:

- set checkout timeout and no-wait behavior
- monitor pool occupancy
- share a statement cache across pooled connections

## The tuning surface

The pool carries four knobs — how long checkout waits, whether it fails fast, the
shared statement cache, and the sizing you set at creation:

| C function | C++ method | Meaning |
|------------|------------|---------|
| `OCI_PoolSetTimeout` | `SetTimeout` | max seconds to wait for a free connection |
| `OCI_PoolGetTimeout` | `GetTimeout()` | read it |
| `OCI_PoolSetNoWait` | `SetNoWait` | fail instantly instead of waiting |
| `OCI_PoolGetNoWait` | `GetNoWait()` | read it |
| `OCI_PoolSetStatementCacheSize` | `SetStatementCacheSize` | statement cache shared by pooled connections |
| `OCI_PoolGetStatementCacheSize` | `GetStatementCacheSize()` | read it |
| `OCI_PoolGetMin` | `GetMin()` | minimum sessions |
| `OCI_PoolGetMax` | `GetMax()` | maximum sessions |
| `OCI_PoolGetIncrement` | `GetIncrement()` | growth step |

:::sample
```c
OCI_PoolSetTimeout(pool, 10);        /* wait up to 10 s for a free conn   */
OCI_PoolSetNoWait(pool, FALSE);      /* block rather than fail            */
OCI_PoolSetStatementCacheSize(pool, 20);
```

```cpp
pool.SetTimeout(10);
pool.SetNoWait(false);
pool.SetStatementCacheSize(20);
```
:::

## Occupancy counters

Two counters tell you how loaded the pool is — how many sessions exist vs how many
are checked out:

| C function | C++ method | Meaning |
|------------|------------|---------|
| `OCI_PoolGetOpenedCount` | `GetOpenedCount()` | physical sessions created |
| `OCI_PoolGetBusyCount` | `GetBusyCount()` | currently checked out |

- `opened` grows toward `max` under load and (implementation permitting) shrinks on
  idle.
- `busy > opened` should never happen; `busy == opened == max` means contention —
  either raise `max` or find the connection-hogging code.

## Sizing heuristics

- `min` = your normal idle concurrency (keeps sessions warm).
- `max` = bounded by server `processes`/`sessions` limits and per-conn memory.
- `incr` = 1–2; big increments create stampeding connection storms.

## Failure semantics

- Timeout/NoWait exhausted → checkout returns NULL/throws — always check.
- Dead pooled session → OCI drops it transparently; ping on checkout if you want
  certainty.

## Putting it together — the typical sequence

Pool tuning is matching the pool's growth curve to your load:

1. **Min connections** — the warm floor, kept open idle.
2. **Max connections** — the cap; beyond it `PoolGetConnection` blocks or
   errors.
3. **Increment** — how many to add per growth step.
4. **`OCI_PoolSetTimeout`** — how long a borrower waits before failing.
5. **`OCI_PoolSetNoWait`** — fail immediately instead of blocking when full.

Hints: min = your average concurrency, max = your spike; a pool that grows
and shrinks constantly is worse than a static one — set min to your working
set; `NoWait` is for latency-sensitive paths that prefer an error over a wait;
and pool exhaustion shows as `PoolGetConnection` timeout — monitor it before
raising max blindly.

## Try it yourself

1. Set `min=1 max=2`, check out 3 with `SetNoWait(TRUE)` — observe the failure.
2. Loop checkouts while printing `opened`/`busy` — watch it grow then cap.
3. Compare a prepare-heavy loop on pooled vs fresh connections with cache size 0 vs 20.

## Common pitfalls

- **`SetNoWait` in production** — a busy pool starts failing user requests instead of
  queueing; usually a monitoring signal, not a fix.
- **Confusing pool statement cache with per-connection cache** — the pool-level one
  applies to all checkouts.
- **Leaking checkouts** — a `ConnectionFree` forgotten inside an error path.

## Recap

- `Timeout`/`NoWait`/`StatementCacheSize` tune waits; `opened`/`busy` monitor health.

**Next:** threads — `OCI_ENV_THREADED`, sharing rules, per-thread errors.
