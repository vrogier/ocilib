---
title: Checkpoint 3 — pooled multi-worker loader
navtitle: Checkpoint 3 pool workers
part: 5
level: advanced
---

# Checkpoint 3 — pooled multi-worker loader

You should now be able to manage transactions explicitly, pool connections, and run
OCILIB from multiple threads safely.

## The exercise — parallel bulk insert

Extend your employee-directory CLI into a **parallel importer**:

1. Create `OCI_ENV_THREADED` environment and a `OCI_POOL_CONNECTION` pool
   (`min=2, max=4, incr=1`) with `OCI_PoolSetStatementCacheSize(pool, 20)`.
2. Read a CSV file (`empno,ename,job,sal,deptno` — invent ~200 rows) on the main
   thread into an in-memory list.
3. Spawn 4 worker threads. Each worker:
   - checks out its own connection from the pool,
   - prepares `insert into employees … values(:n,:e,:j,:s,:d)` once,
   - loops over its share of rows — `Execute` each, `OCI_ConnectionCommit` every 25 rows,
   - releases the connection and reports rows done + elapsed.
4. Main thread joins all workers, prints `OCI_PoolGetBusyCount`/`GetOpenedCount`
   stats, verifies `select count(*)` grew by 200.

## Acceptance checks

- Rows are all there once — `select empno, count(*) … having count(*)>1` returns
  nothing.
- With a deliberate duplicate `empno` in the CSV, exactly one worker's commit fails
  and its rollback is clean — other workers finish fine.
- `busy` never exceeds `opened` or `max`.

## Stretch goals

- Swap `OCI_POOL_CONNECTION` → `OCI_POOL_SESSION`, tag sessions, confirm reuse.
- Wrap each worker's 25-row block in an `OCI_Transaction` object instead of
  connection-level commit.
- Have a 5th thread call `OCI_ConnectionBreak` on a long-running query and log ORA-01013.

## What it exercises

| Skill | Page |
|-------|------|
| Transactions & boundaries | 410/415 |
| Pool create/checkout/monitor | 420/425 |
| Threaded env + sharing rules | 430 |
| Prepared execute loops | 260 |
| Error handling per thread | 080 |

Part 5 done. Next: Part 2 — the Oracle data-type toolbox, starting with strings and
charsets.
