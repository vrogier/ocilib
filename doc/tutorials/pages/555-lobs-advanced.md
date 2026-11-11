---
title: LOBs — append, copy, buffering & remote LOBs
navtitle: LOBs advanced
part: 2
level: advanced
---

# LOBs — append, copy, buffering & remote LOBs

**Objectives** — by the end of this page you will be able to:

- append and copy LOB content efficiently
- use buffering, flush and open/close modes
- work with remote and file-sourced LOBs

## In context — why these calls exist

Every LOB operation crosses the wire: a `Write` of 4 KB costs a round-trip, and
a read-modify-write of a LOB segment on the server costs even more. The advanced
API exists to cut that traffic in three ways. **Server-side moves**
(`AppendLob`, `Copy`, `CopyFromFile`) keep the bytes inside the database —
copying a 500 MB document between rows takes one call, not a download and an
upload. **Buffering** batches many small client writes into one flush — the
client accumulates writes and `OCI_LobFlush` sends them as a single operation.
**Chunk alignment** (`GetChunkSize`) lets you write in the storage unit Oracle
actually uses, avoiding the read-modify-write penalty on partial blocks.

`Open`/`Close` belong to the same family: opening a LOB explicitly holds the
server-side LOB open across several operations instead of paying an open cost
per call — worthwhile inside a loop of reads or mixed read/write cycles.
`IsRemote` tells you when the locator points through a database link
(`@remote_db`), where some of these optimizations simply are not available —
always worth checking before assuming a copy stays server-side.

## The advanced surface

| C function | C++ method | Purpose |
|------------|------------|---------|
| `OCI_LobAppend` | `Append` | append data at the end |
| `OCI_LobAppendLob` | `AppendLob` | append one LOB to another server-side |
| `OCI_LobCopy` | `Copy` | clone content |
| `OCI_LobCopyFromFile` | — (C only) | clone from a `File` |
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
OCI_Lob *a = OCI_ResultsetGetLob(rs, 0);            /* source row's lob  */
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
"external file → database" migration (C only; call it on raw handles from C++).

## Scenario — surgical edits: erase & truncate

`Erase` zeroes a range (the length stays); `Truncate` shrinks the LOB:

:::sample
```c
OCI_UInt64 erased = OCI_LobErase(lob, 1024, 512);   /* wipe 512 units @1024 */
OCI_LobTruncate(lob, (OCI_UInt64) 4096);      /* drop everything > 4 KiB */
```

```cpp
UInt64 erased = lob.Erase(1024, 512);         /* wipe 512 units @1024 */
lob.Truncate(4096);                           /* drop everything > 4 KiB */
```
:::

## Scenario — chunk-aligned throughput

`GetChunkSize` returns the storage-aligned I/O unit. Writing in multiples of it
avoids the server's read-modify-write penalty on partial blocks:

:::sample
```c
OCI_UInt32 chunk = OCI_LobGetChunkSize(lob);  /* e.g. 8132              */
OCI_UInt64 cap   = OCI_LobGetMaxSize(lob);    /* e.g. 4 TB              */

/* write buffer sized to a multiple of `chunk` */
for (OCI_UInt64 off = 0; off < total; off += chunk)
    OCI_LobWrite(lob, stage, &nch, &nby);
```

```cpp
UInt32 chunk = lob.GetChunkSize();            /* e.g. 8132              */
UInt64 cap   = lob.GetMaxSize();              /* e.g. 4 TB              */

for (UInt64 off = 0; off < total; off += chunk)
    lob.Write(stage);
```
:::

## Buffering writes

Many small `LobWrite`s = many round-trips. Buffer them:

:::sample
```c
OCI_LobEnableBuffering(lob, TRUE);
for (…) OCI_LobWrite(lob, chunk, &nch, &nby);   /* batched client-side */
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

## Putting it together — the typical sequence

Advanced LOB work is a decision tree, and the decisions come first:

1. **Can the data stay on the server?** Copying a LOB through the client is a
   download-then-upload — `AppendLob`, `Copy`, and `CopyFromFile` (BFILE→LOB)
   all keep the bytes inside the database. Reach for them before any
   read-write loop.
2. **Are you doing many small writes?** Enable buffering, write freely, then
   `Flush` once — one round-trip instead of hundreds. Forgetting the flush is
   the classic bug: buffered data never reaches the server.
3. **Writing a lot?** Align to `GetChunkSize` — partial-block writes force the
   server into read-modify-write; chunk-aligned writes don't.
4. **Long I/O session?** `Open` explicitly, do the work, `Close` — otherwise
   every op pays an implicit open.
5. **Crossing a database link?** `IsRemote` tells you — and several of these
   paths are restricted remotely, so check before assuming.

Hints: `Erase` punches a hole (length unchanged) while `Truncate` shortens —
different operations, easy to confuse; `GetMaxSize` reports the platform cap
(~TB range), useful sanity check before streaming unknown sources.

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
