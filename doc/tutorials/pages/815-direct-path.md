---
title: Direct path loading — OCI_DirPath
navtitle: Direct path loading
part: 8
level: advanced
demos: [dirpath.c, dirpath_complete.c]
sql: dirpath.sql
---

# Direct path loading — the `DirPath` loader

**Objectives** — by the end of this page you will be able to:

- bulk-load rows faster than any INSERT via direct path
- run the set-column → prepare → convert → load cycle
- handle conversion errors and tune buffers

## Meet the DirPath

A `DirPath` bypasses the SQL engine and writes blocks directly into the table segment — the same path `SQL*Loader` uses.

It's the fastest ingest in Oracle, at the cost of a stricter workflow: it's created **from the table's TypeInfo** (a table, not a query), describes its columns, fills a server row buffer in batches, converts and loads.

:::sample
```c
OCI_TypeInfo *tbl = OCI_TypeInfoGet(cn, OTEXT("dirpath_data"), OCI_TIF_TABLE);
OCI_DirPath  *dp  = OCI_DirPathCreate(tbl, NULL, NUM_COLS, MAX_ROWS);
```

```cpp
TypeInfo tbl(con, "dirpath_data", TypeInfo::Table);
DirectPath dp(tbl, NUM_COLS, MAX_ROWS);
```
:::

## The cycle

:::sample
```c
/* 1. describe columns */
OCI_DirPathSetColumn(dp, 1, OTEXT("val_int"),  20, NULL);
OCI_DirPathSetColumn(dp, 2, OTEXT("val_str"),  30, NULL);
OCI_DirPathSetColumn(dp, 3, OTEXT("val_date"),  8, OTEXT("YYYYMMDD"));

/* 2. prepare → server allocates the row buffer */
OCI_DirPathPrepare(dp);
OCI_UInt32 max = OCI_DirPathGetMaxRows(dp);        /* capacity for this buffer */

/* 3. fill + convert + load, looping over your data */
OCI_DirPathReset(dp);
for (row = 0; row < max && more_data; row++)
{
    OCI_DirPathSetEntry(dp, row, 1, &ival,  sizeof(ival), TRUE);
    OCI_DirPathSetEntry(dp, row, 2, sval,   strlen(sval),  TRUE);
    OCI_DirPathSetEntry(dp, row, 3, dval,   strlen(dval),  TRUE);
}
switch (OCI_DirPathConvert(dp))
{
    case OCI_DPR_FULL:      /* buffer full — load and continue */
    case OCI_DPR_COMPLETE:  /* data done — load and finish      */
        OCI_DirPathLoad(dp); break;
    case OCI_DPR_ERROR:     /* conversion errors — see below    */
        break;
}
/* 4. wrap up */
OCI_DirPathFinish(dp);
OCI_DirPathFree(dp);
```

```cpp
/* 1. describe columns */
dp.SetColumn(1, "val_int",  20, "");
dp.SetColumn(2, "val_str",  30, "");
dp.SetColumn(3, "val_date",  8, "YYYYMMDD");

/* 2. prepare → server allocates the row buffer */
dp.Prepare();
UInt32 max = dp.GetMaxRows();                  /* capacity for this buffer */

/* 3. fill + convert + load, looping over your data */
dp.Reset();
for (row = 0; row < max && more_data; row++)
{
    dp.SetEntry(row, 1, &ival, sizeof(ival), true);
    dp.SetEntry(row, 2, sval,  strlen(sval),  true);
    dp.SetEntry(row, 3, dval,  strlen(dval),  true);
}
switch (dp.Convert())
{
    case DirPath::ResFull:      /* buffer full — load and continue */
    case DirPath::ResComplete:  /* data done — load and finish      */
        dp.Load(); break;
    case DirPath::ResError:     /* conversion errors — see below    */
        break;
}
/* 4. wrap up */
dp.Finish();                                   /* RAII frees the handle */
```
:::

`SetEntry` args: (row, col, data, len, complete-flag) — FALSE marks the entry
incomplete for piecewise/long values.

## Tuning & errors

| C function | C++ method | Purpose |
|------------|------------|---------|
| `OCI_DirPathSetBufferSize` | `SetBufferSize` | I/O buffer |
| `OCI_DirPathSetCacheSize` | `SetCacheSize` | stream cache |
| `OCI_DirPathSetNoLog` | `SetNoLog` | `NOLOGGING` load (speed vs recoverability!) |
| `OCI_DirPathSetParallel` | `SetParallel` | parallel load |
| `OCI_DirPathSetDateFormat` | `SetDateFormat` | default date format for text→date columns |
| `OCI_DirPathSetConvertMode` | `SetConvertMode` | conversion behavior |
| `OCI_DirPathSetCurrentRows` | `SetCurrentRows` | partial-row fill |
| `OCI_DirPathSetEntry` | `SetEntry` | one cell (row, col, data, len, complete-flag) |
| `OCI_DirPathFlushRow` | `FlushRow` | flush a partial row |
| `OCI_DirPathSave` | `Save` | savepoint |
| `OCI_DirPathGetRowCount` | `GetRowCount()` | progress |
| `OCI_DirPathGetAffectedRows` | `GetAffectedRows()` | rows committed so far |
| `OCI_DirPathGetErrorRow` | `GetErrorRow()` | failing row on `DPR_ERROR` |
| `OCI_DirPathGetErrorColumn` | `GetErrorColumn()` | failing column on `DPR_ERROR` |
| `OCI_DirPathReset` | `Reset` | buffer reset |
| `OCI_DirPathAbort` | `Abort` | abort the stream |
| `OCI_DirPathFinish` | `Finish` | commit the load |

## Direct path vs array bind

| | `OCI_DirPath` | `OCI_BindArrayOf*` |
|---|---|---|
| Speed | fastest (direct blocks) | fast (round-trip batching) |
| Constraints | limited checking during load | full constraint evaluation |
| Triggers | mostly bypassed | fired |
| Logging | optional `NOLOGGING` | always logged |
| Best for | ETL / initial loads | everyday bulk DML |

## Try it yourself

1. Load 100k rows via direct path vs array bind — time both.
2. Deliberately convert a bad date → read `GetErrorRow`/`GetErrorColumn`.
3. Load into a partitioned table passing a partition name to `DirPathCreate`.

## Common pitfalls

- **Skipping `Prepare`** — `SetEntry` fails until `GetMaxRows` is known.
- **Forgetting `Finish`** — data stays uncommitted in the load stream.
- **Indexes during massive loads** — unusable-index strategy is faster than
  loading onto maintained indexes.

## Recap

- `TypeInfo` → `DirPathCreate` → `SetColumn` → `Prepare` → `SetEntry` loop →
  `Convert` → `Load` → `Finish`.

**Next:** database administration — startup/shutdown and instance info.
