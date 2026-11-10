---
title: LOBs — basics
navtitle: LOBs basics
part: 5
level: intermediate
demos: [lob.c]
sql: lob.sql
---

# LOBs — basics

**Objectives** — by the end of this page you will be able to:

- create and bind `CLOB`/`BLOB`/`NCLOB` locators
- read and write LOB content in chunks
- tell temporary LOBs from column locators

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
OCI_LobWrite(clob, text, len);          /* otext data for CLOB */

OCI_Prepare(st, OTEXT("insert into articles(id, body) values(:i, :b)"));
OCI_BindLob(st, OTEXT(":b"), clob, OCI_CLOB);
OCI_Execute(st);
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
OCI_ExecuteStmt(st, OTEXT("select body from articles where id = :i"));
/* bind :i, execute, fetch */
OCI_Lob *clob = OCI_GetLob(rs, 0);      /* borrowed locator */

otext buf[4096]; OCI_UInt32 n = sizeof(buf);
while (OCI_LobRead(clob, buf, &n))      /* n = chars/bytes actually read */
{
    /* consume n units of buf */
    n = sizeof(buf);
}
```

```cpp
st.Execute("select body from articles where id = :i");
/* bind :i, execute, fetch */
Lob clob = st.GetResultset().Get<Lob>(0); /* borrowed locator */

otext buf[4096]; UInt32 n = sizeof(buf);
while (clob.Read(buf, n))
{
    /* consume n units of buf */
    n = sizeof(buf);
}
```
:::

!!! note "Char vs byte semantics"
    `CLOB`/`NCLOB` counts are in **characters**; `BLOB` in **bytes**. On wide builds
    CLOB buffers are UTF-16 `otext`.

## Temporary LOBs

`OCI_LobCreate`d locators are *temporary* — they live until freed. You bind them like
column values; persistent LOBs come back from `OCI_GetLob` and are borrowed.

C++: `Lob l(con, Lob::Clob)` / `rs.Get<Lob>(i)`; `l.Write(buf)`, `l.Read(buf)`,
`l.GetLength()`.

## Try it yourself

1. Insert a 1 MB text file into a CLOB in 64 KB chunks.
2. Fetch it back and verify `GetLength` matches.
3. Use `OCI_LobSeek` to patch a value mid-LOB.

## Common pitfalls

- **Freeing a fetched locator** — `OCI_GetLob` returns borrowed; only `Create`d ones
  are freed.
- **Reading past `GetLength`** — `LobRead` returns FALSE at EOF; rely on it.
- **`empty_clob()` columns** — Oracle's `EMPTY_CLOB()` is *not* NULL and *not* data.

## Recap

- `OCI_Lob` = locator; chunked `Read`/`Write`, `GetLength`, byte vs char counts.

**Next:** LOB advanced — append, copy, buffering, remote lobs.
