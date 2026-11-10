---
title: LOBs — append, copy, buffering & remote LOBs
navtitle: LOBs advanced
part: 5
level: advanced
---

# LOBs — append, copy, buffering & remote LOBs

**Objectives** — by the end of this page you will be able to:

- append and copy LOB content efficiently
- use buffering, flush and open/close modes
- work with remote and file-sourced LOBs

## The advanced surface

| C function | C++ method | Purpose |
|------------|------------|---------|
| `OCI_LobAppend` | `Append` | append data at the end |
| `OCI_LobAppendLob` | `AppendLob` | append one LOB to another server-side |
| `OCI_LobCopy` | `Copy` | clone content |
| `OCI_LobCopyFromFile` | `CopyFromFile` | clone from a `File` |
| `OCI_LobErase` | `Erase` | punch a hole |
| `OCI_LobTruncate` | `Truncate` | shrink |
| `OCI_LobAssign` | `Assign` | copy locator |
| `OCI_LobIsEqual` | `Equals` | compare locators |
| `OCI_LobOpen` | `Open` | explicit open mode for read/write cycles |
| `OCI_LobClose` | `Close` | close |
| `OCI_LobEnableBuffering` | `EnableBuffering` | client-side write buffering |
| `OCI_LobFlush` | `Flush` | push buffered writes |
| `OCI_LobGetChunkSize` | `GetChunkSize()` | optimal I/O unit |
| `OCI_LobGetMaxSize` | `GetMaxSize()` | storage limit |
| `OCI_LobIsRemote` | `IsRemote()` | locator's provenance |
| `OCI_LobGetConnection` | `GetConnection()` | owning connection |

## Server-side copy — the fast path

Copying a LOB client→server→client is wasteful. `OCI_LobAppendLob` keeps bytes on the
server:

:::sample
```c
OCI_Lob *a = OCI_GetLob(rs, 0);            /* source row's lob  */
OCI_Lob *b = OCI_LobCreate(cn, OCI_CLOB);  /* new temp lob      */
OCI_LobAppendLob(b, a);                    /* a's content → b   */
```

```cpp
Lob a = st.GetResultset().Get<Lob>(0);     /* source row's lob  */
Lob b(con, Lob::Clob);                     /* new temp lob      */
b.AppendLob(a);                            /* a's content → b   */
```
:::

`OCI_LobCopyFromFile` streams an `OCI_File` (BFILE) into a LOB — the classic
"external file → database" migration.

## Buffering writes

Many small `LobWrite`s = many round-trips. Buffer them:

:::sample
```c
OCI_LobEnableBuffering(lob);
for (…) OCI_LobWrite(lob, chunk, n);   /* batched client-side */
OCI_LobFlush(lob);                     /* one flush           */
```

```cpp
lob.EnableBuffering();
for (…) lob.Write(chunk, n);           /* batched client-side */
lob.Flush();                           /* one flush           */
```
:::

`OCI_LobGetChunkSize` tells you the storage-aligned chunk — writing in multiples of
it avoids read-modify-write overhead.

## Open/close

`OCI_LobOpen(lob, mode)` keeps a LOB open across multiple ops — useful for
interleaved read/write without the server re-opening per call; close with
`OCI_LobClose`.

## Remote LOBs

LOBs fetched through a database link (`@remote_db`) are *remote* — `OCI_LobIsRemote`
reports it, `OCI_LobGetConnection` gives the owning connection handle for diagnostics.
Some operations have restrictions remotely; test on your topology.

## Try it yourself

1. Build a LOB from 1000 small writes with and without buffering — time both.
2. Copy one row's CLOB into another row entirely server-side.
3. Read `GetChunkSize`/`GetMaxSize` for a BLOB column and explain the numbers.

## Common pitfalls

- **`LobFlush` forgotten** — buffered writes never reach the server.
- **Copying through the client** — `AppendLob`/`Copy` stay server-side; manual
  read+write doubles traffic.
- **Offsets off by char/byte** — `Seek`/`Truncate`/`GetLength` follow the LOB's
  unit (chars for CLOB, bytes for BLOB).

## Recap

- `Append`/`AppendLob`/`Copy`/`Erase`/`Truncate` for surgical edits; buffering +
  `GetChunkSize` for throughput; `Open`/`Close` for I/O sessions.

**Next:** BFILEs — the filesystem inside the database.
