---
title: Tuning fetches — array size, prefetch, LONG mode
navtitle: Fetch tuning
part: 3
level: intermediate
demos: [scroll.c]
---

# Tuning fetches — array size, prefetch, LONG mode

**Objectives** — by the end of this page you will be able to:

- cut round-trips with `OCI_StatementSetFetchSize`
- tune OCI prefetch memory/rows
- handle `LONG`/`LONG RAW` columns with long modes and piece size

## The problem

Fetching looks like "one row per call" — under the hood the client fetches rows in
batches per round-trip. The batch size is the knob, set on the statement **before**
executing:

:::sample
```c
OCI_Statement *st = OCI_StatementCreate(cn);
OCI_StatementSetFetchSize(st, 500);                 /* rows per round-trip */
OCI_StatementExecute(st, OTEXT("select * from employees"));
```

```cpp
Statement st(con);
st.SetFetchSize(500);                      /* rows per round-trip */
st.Execute("select * from employees");
```
:::

Rule of thumb: size it to expected row count (100–1000 for reports); too big wastes
client memory, too small wastes round-trips.

## The tuning knobs

| C function | C++ method | Role |
|------------|------------|------|
| `OCI_StatementSetFetchSize` | `SetFetchSize` | rows fetched per round-trip |
| `OCI_StatementGetFetchSize` | `GetFetchSize()` | read it back |
| `OCI_StatementSetPrefetchSize` | `SetPrefetchSize` | rows prefetched on the first fetch |
| `OCI_StatementGetPrefetchSize` | `GetPrefetchSize()` | read it back |
| `OCI_StatementSetPrefetchMemory` | `SetPrefetchMemory` | prefetch memory ceiling — whichever limit hits first wins |
| `OCI_StatementGetPrefetchMemory` | `GetPrefetchMemory()` | read it back |
| `OCI_StatementSetLongMode` | `SetLongMode` | how LONG columns materialize (below) |
| `OCI_StatementGetLongMode` | `GetLongMode()` | read it back |
| `OCI_StatementSetPieceSize` | `SetPieceSize` | streaming chunk size for piecewise reads |
| `OCI_StatementGetPieceSize` | `GetPieceSize()` | read it back |

All are per-statement and must be set **after** create, **before** execute — they
shape the upcoming execute, not the current resultset.

## LONG / LONG RAW columns

Legacy `LONG`/`LONG RAW` don't arrive in normal row buffers — choose how the
statement materializes them:

| C enum | C++ enum | Meaning |
|--------|----------|---------|
| `OCI_LONG_EXPLICIT` | `Statement::LongExplicit` | get a `Long` object via the LONG getter and stream it (page 285) |
| `OCI_LONG_IMPLICIT` | `Statement::LongImplicit` | LONG columns arrive as strings (LONG RAW can't use this) |

For new schemas prefer LOBs — but `LONG` still lives in `USER_TAB_COLUMNS`, `V$`
views and old apps.

## Putting it together — the typical sequence

Fetch performance is three tunables, applied in order:

1. **`OCI_StatementSetFetchArraySize`** — how many rows come back per
   round-trip; the single biggest fetch win. Default is small; raise it on
   bulk reads.
2. **`OCI_StatementSetPrefetchSize`/`SetPrefetchMemory`** — how much data the
   client buffers ahead; bigger = fewer round-trips at more RAM.
3. **Bind/output buffer strategy** — reuse buffers across fetches rather than
   reallocating per row.

Hints: array size is a trade-off — too big and you waste memory per fetch, too
small and every 10 rows costs a round-trip; LOB and LONG columns don't
prefetch — their content streams regardless; and measure before tuning —
`OCI_GetLastError`-visible timing in your own wrapper beats guessing.

## Try it yourself

1. Benchmark `select * from employees` (or a big table) with fetch size 1, 100, 1000 —
   print elapsed time.
2. Select `data_default from user_tab_columns` (a LONG) with both long modes and
   compare what the getter returns.
3. Read `OCI_StatementGetFetchSize` on a fresh statement — what's the default on your client?

## Common pitfalls

- **Setting after execute** — silently does nothing for that resultset.
- **Giant fetch sizes on wide rows** — `rows × row_size` is allocated client-side.
- **Assuming prefetch = fetch size** — prefetch fills the first batch; fetch size
  sizes every batch after.

## Recap

- `SetFetchSize` = rows per round-trip; prefetch = first-batch hints; long mode +
  piece size = legacy LONG strategy.

**Next:** scrollable resultsets — fetch backwards, jump to absolute rows.
