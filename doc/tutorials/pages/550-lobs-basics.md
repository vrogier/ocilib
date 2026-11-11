---
title: LOBs — basics
navtitle: LOBs basics
part: 2
level: intermediate
demos: [lob.c]
sql: lob.sql
---

# LOBs — basics

**Objectives** — by the end of this page you will be able to:

- create and bind `CLOB`/`BLOB`/`NCLOB` locators
- read and write LOB content in chunks
- tell temporary LOBs from column locators

## In context — locators, not payloads

A `CLOB`/`BLOB`/`NCLOB` column does not store its content in the row — the row
holds a **locator**, a pointer to data Oracle manages elsewhere (in-line up to
~4 KB, then out-of-line in a LOB segment). `OCI_Lob` wraps that locator: every
`Read`/`Write` you issue is a round-trip asking the server to move a chunk
through the pointer. That is why LOB code is written as chunked I/O loops rather
than "give me the whole string" — the handle is a cursor over content, not the
content.

Two locator kinds exist. **Persistent** locators come from a fetched column and
refer to stored data; they are borrowed — you never free them, and changes go
through the row. **Temporary** locators come from `OCI_LobCreate`: a scratch LOB
living on the server until you free it, useful as a staging area before a bind
or a copy. Keeping that distinction straight — fetched = borrowed/persistent,
created = owned/temporary — is most of what LOB code gets wrong.

The units trip people too: `CLOB`/`NCLOB` count in *characters*, `BLOB` in
*bytes*, and every length/offset/counter in the API follows the locator's unit.
`OCI_LobWrite`/`OCI_LobRead` therefore take *two* in/out counters — char count
and byte count — because on a wide build a CLOB's char count and byte count
differ by the `otext` width; pass the buffer sized for the *byte* count.

## Concepts

A `Lob` is a **locator** — a pointer to content living in the database (or a temporary server-side LOB), created on the connection by LOB subtype.

Operations are chunked I/O over it:

| C function | C++ method | Purpose |
|------------|------------|---------|
| `OCI_LobCreate` | `Lob` ctor (`con`, `Lob::Blob`/`Clob`/`NClob`) | locator |
| `OCI_LobFree` | — (RAII) | release |
| `OCI_LobWrite` | `Write` | write chunk (CLOB chars / BLOB bytes) |
| `OCI_LobRead` | `Read` | read chunk — `len` in/out |
| `OCI_LobGetLength` | `GetLength()` | content length |
| `OCI_LobGetType` | `GetType()` | BLOB vs CLOB vs NCLOB |
| `OCI_LobIsTemporary` | `IsTemporary()` | temp vs persistent |
| `OCI_LobSeek` | `Seek` | position for partial I/O |
| `OCI_LobGetOffset` | `GetOffset()` | current position |

## Writing a CLOB

:::sample
```c
OCI_Lob *clob = OCI_LobCreate(cn, OCI_CLOB);
OCI_UInt32 nch = len, nby = len;
OCI_LobWrite(clob, text, &nch, &nby); /* otext data for CLOB */

OCI_StatementPrepare(st, OTEXT("insert into articles(id, body) values(:i, :b)"));
OCI_StatementBindLob(st, OTEXT(":b"), clob);
OCI_StatementExecutePrepared(st);
OCI_LobFree(clob);
```

```cpp
Lob clob(con, Lob::Clob);
clob.Write(text, len);                  /* otext data for CLOB */

st.Prepare("insert into articles(id, body) values(:i, :b)");
st.Bind(":b", clob);
st.ExecutePrepared();
```
:::

## Reading one back

:::sample
```c
OCI_StatementExecute(st, OTEXT("select body from articles where id = :i"));
/* bind :i, execute, fetch */
OCI_Lob *clob = OCI_ResultsetGetLob(rs, 0);      /* borrowed locator */

otext buf[4096];
OCI_UInt32 nch = sizeof(buf) / sizeof(otext);
OCI_UInt32 nby = sizeof(buf);
while (OCI_LobRead(clob, buf, &nch, &nby))      /* n = chars/bytes actually read */
{
    /* consume n units of buf */
    n = sizeof(buf);
}
```

```cpp
st.Execute("select body from articles where id = :i");
/* bind :i, execute, fetch */
Lob clob = st.GetResultset().Get<Lob>(0); /* borrowed locator */

/* C++ Read(n) reads n units at the current offset — loop for chunks  */
String body = clob.Read(clob.GetLength());    /* whole content        */
/* or chunked: */
UInt32 off = 0;
for (; off < clob.GetLength(); off += 4096)
{
    clob.Seek(SeekSet, off);
    String chunk = clob.Read(4096);
}
```
:::

!!! note "Char vs byte semantics"
    `CLOB`/`NCLOB` counts are in **characters**; `BLOB` in **bytes**. On wide builds
    CLOB buffers are UTF-16 `otext`.

## Temporary LOBs

`OCI_LobCreate`d locators are *temporary* — they live until freed. You bind them like
column values; persistent LOBs come back from `OCI_ResultsetGetLob` and are borrowed.

C++: `Lob l(con, Lob::Clob)` / `rs.Get<Lob>(i)`; `l.Write(buf)`, `l.Read(buf)`,
`l.GetLength()`.

## Scenario — the three basic writes

:::sample
```c
OCI_Lob *lob = OCI_LobCreate(cn, OCI_BLOB);

/* 1. sequential build — write chunk after chunk                      */
OCI_UInt32 n = sizeof(chunk);
OCI_LobWrite(lob, chunk, &n, &n);

/* 2. append — same as write-at-end, explicit                         */
OCI_LobAppend(lob, tail_bytes, &n, &n);

/* 3. partial write — seek then overwrite a slice                     */
OCI_LobSeek(lob, (OCI_UInt64) 1024, OCI_SEEK_SET);
OCI_UInt64 off = OCI_LobGetOffset(lob);       /* = 1024                */
OCI_LobWrite(lob, patch, &n, &n);             /* overwrites 1024..     */
```

```cpp
Lob lob(con, Lob::Blob);
Raw chunk{ /* … */ }, tail_bytes{ /* … */ }, patch{ /* … */ };

/* 1. sequential build — write chunk after chunk                      */
lob.Write(chunk);

/* 2. append — same as write-at-end, explicit                         */
lob.Append(tail_bytes);

/* 3. partial write — seek then overwrite a slice                     */
lob.Seek(SeekSet, 1024);
UInt64 off = lob.GetOffset();                 /* = 1024                */
lob.Write(patch);                             /* overwrites 1024..     */
```
:::

## Scenario — streaming read until EOF

`OCI_LobRead` is your iterator — each call advances the internal offset; FALSE
means end-of-content:

:::sample
```c
OCI_Lob *body = OCI_ResultsetGetLob(rs, 0);        /* borrowed           */

OCI_UInt64 total = 0;
otext      buf[8192];
OCI_UInt32 nch = 8192, nby = sizeof(buf);
while (OCI_LobRead(body, buf, &nch, &nby) && nch > 0)
{
    total += nch;                                /* chars for CLOB      */
    nch = 8192;
    nby = sizeof(buf);                         /* refill capacities   */
}
printf("streamed %llu of %u chars\n",
       (unsigned long long) total, OCI_LobGetLength(body));
```

```cpp
Lob body = st.GetResultset().Get<Lob>(0);        /* borrowed           */

UInt64 total = 0;
for (UInt64 off = 0; off < body.GetLength(); )
{
    body.Seek(SeekSet, off);
    String chunk = body.Read(8192);              /* up to 8192 units    */
    total += chunk.size();
    off   += chunk.size();
}
```
:::

## Scenario — append an existing LOB to another

Server-side append copies content without a single client round-trip — see
`OCI_LobAppendLob` on the next page.

## Putting it together — the typical sequence

LOB code always reduces to the same loop — learn it once:

1. **Get a locator**: `OCI_ResultsetGetLob` on a fetched row (borrowed,
   persistent) or `OCI_LobCreate` (owned, temporary) for staging.
2. **Ask its size** with `GetLength` — in *chars* for CLOB/NCLOB, *bytes* for
   BLOB. That unit follows every offset and counter you touch.
3. **Move data in chunks**: `Read` in a loop until it returns FALSE (each call
   advances the internal offset), `Write`/`Append` the same way — both take
   the in/out char+byte counters.
4. **`Seek` for random access**, then `GetOffset` to know where you are.
5. **Bind the locator** into INSERT/UPDATE; **free only what you created** —
   fetched locators die with their row.

Hints: `EMPTY_CLOB()`/`EMPTY_BLOB()` columns are *not* NULL and *not* data —
check `GetLength`, not just null-ness; on wide builds a CLOB's byte count and
char count differ by `sizeof(otext)`, which is exactly why the API hands you
both counters; and when writes get chatty, the next page's buffering family is
the answer.

## Try it yourself

1. Insert a 1 MB text file into a CLOB in 64 KB chunks.
2. Fetch it back and verify `GetLength` matches.
3. Use `OCI_LobSeek` to patch a value mid-LOB.

## Common pitfalls

- **Freeing a fetched locator** — `OCI_ResultsetGetLob` returns borrowed; only `Create`d ones
  are freed.
- **Reading past `GetLength`** — `LobRead` returns FALSE at EOF; rely on it.
- **`empty_clob()` columns** — Oracle's `EMPTY_CLOB()` is *not* NULL and *not* data.

## Recap

- `OCI_Lob` = locator; chunked `Read`/`Write`, `GetLength`, byte vs char counts.

**Next:** LOB advanced — append, copy, buffering, remote lobs.
