---
title: LONG and LONG RAW — OCI_Long piecewise access
navtitle: LONG & LONG RAW
part: 3
level: intermediate
demos: [long.c]
sql: long.sql
---

# LONG and LONG RAW — the `Long` streaming object

**Objectives** — by the end of this page you will be able to:

- read `LONG`/`LONG RAW` columns in chunks through the Long object
- write long values piecewise on insert/update
- choose between explicit streaming and implicit string mode

!!! warning "Deprecated Oracle feature"
    `LONG` and `LONG RAW` are **deprecated** — Oracle has marked them obsolete
    since version 8 (1999) and steers all new development to `CLOB`/`BLOB`,
    which are strictly more capable: they support random access, piecewise
    read/write, multiple locators per row, and no arbitrary restrictions (a
    table can only have *one* `LONG` column, which can't appear in
    `GROUP BY`/`ORDER BY`/`WHERE` or be copied with `INSERT … SELECT`).

    **This page exists for legacy code and schema migration** — you'll
    encounter `LONG`/`LONG RAW` in old schemas (`DBA_VIEWS.TEXT`, `USER_SOURCE`,
    legacy app tables). For *new* schemas use the LOB types instead
    (see [LOBs basics](lobs-basics.html)). To migrate: `ALTER TABLE … MODIFY
    col CLOB` or `TO_LOB(col)` in a CTAS.

## The Long object

`LONG` (up to 2 GB text) and `LONG RAW` predate LOBs and don't fit normal row buffers — they arrive **piecewise**.

The `Long` object is a streaming buffer around one such value: on fetch it gives you a read cursor over the column's bytes, and on binding it collects the chunks you push before execute.

| C function | C++ method | Purpose |
|------------|------------|---------|
| `OCI_LongCreate` | `Long` ctor | make a LONG (`OCI_CLONG`) / LONG RAW (`OCI_BLONG`) buffer for binding |
| `OCI_LongFree` | — (RAII) | release it |
| `OCI_LongGetSize` | `GetSize()` | current content size |
| `OCI_LongGetType` | `GetType()` | LONG vs LONG RAW |
| `OCI_LongGetBuffer` | `GetBuffer()` | raw content pointer |
| `OCI_LongRead` | `Read` | pull the next chunk (fetched column) |
| `OCI_LongWrite` | `Write` | push a chunk (binding) |
| `OCI_ResultsetGetLong` | `Get<Long>(i)` | get the Long object of a fetched column |

Fetched Longs are **borrowed** — owned by the statement; only `LongCreate`d ones
are freed.

## Reading a LONG

:::sample
```c
OCI_StatementSetLongMode(st, OCI_LONG_EXPLICIT);            /* ask for OCI_Long handles */
OCI_StatementExecute(st, OTEXT("select big_text from articles"));
OCI_Resultset *rs = OCI_StatementGetResultset(st);

while (OCI_ResultsetFetchNext(rs))
{
    OCI_Long *lg = OCI_ResultsetGetLong(rs, 0);             /* borrowed — statement owns it */
    unsigned char buf[8192];
    OCI_UInt32 len = sizeof(buf);
    while (OCI_LongRead(lg, buf, &len))            /* FALSE when done */
    {
        fwrite(buf, 1, len, stdout);
        len = sizeof(buf);
    }
}
```

```cpp
st.SetLongMode(Statement::LongExplicit);
st.Execute("select big_text from articles");

for (auto &&row : st.GetResultset().Rows())
{
    Long lg = row.Get<Long>(0);                     /* borrowed */
    unsigned char buf[8192];
    UInt32 len = sizeof(buf);
    while (lg.Read(buf, len))
    {
        fwrite(buf, 1, len, stdout);
        len = sizeof(buf);
    }
}
```
:::

`OCI_StatementSetPieceSize(st, n)` on the statement controls the streaming chunk size.

## Writing a LONG

:::sample
```c
OCI_Long *lg = OCI_LongCreate(st, OCI_CLONG);
OCI_LongWrite(lg, chunk1, len1);
OCI_LongWrite(lg, chunk2, len2);                    /* append more pieces */

OCI_StatementPrepare(st, OTEXT("insert into articles(id, big_text) values(:i, :t)"));
OCI_StatementBindLong(st, OTEXT(":t"), lg, OCI_LongGetSize(lg));
OCI_StatementExecutePrepared(st);
OCI_LongFree(lg);
```

```cpp
Long lg(st, LongType::LongCharacter);
lg.Write(chunk1, len1);
lg.Write(chunk2, len2);                             /* append more pieces */

st.Prepare("insert into articles(id, big_text) values(:i, :t)");
st.Bind(":t", lg);
st.ExecutePrepared();
```
:::

## The easy way out — implicit mode

If you know the values are small enough to be strings:

:::sample
```c
OCI_StatementSetLongMode(st, OCI_LONG_IMPLICIT);
/* LONG columns now arrive via OCI_ResultsetGetString — no OCI_Long needed.
   (does not apply to LONG RAW) */
```

```cpp
st.SetLongMode(Statement::LongImplicit);
/* LONG columns now arrive via Get<String>(i); Get<Long>(i) for the object */
```
:::

## Putting it together — the typical sequence

`LONG`/`LONG RAW` are legacy columns with a streaming twist:

1. **Create the reader** — `OCI_LongCreate` gives the piecewise handle;
   fetch it with `OCI_ResultsetGetLong`.
2. **Read in chunks** — `OCI_LongRead` in a loop, like a LOB but on the older
   type.
3. **Bind piecewise** — `OCI_StatementBindLong` for writing `LONG` data you
   don't want in a single buffer.

Hints: `LONG`/`LONG RAW` are deprecated in Oracle — new schemas use LOB, and
so should you; `OCI_CLONG` (character) and `OCI_BLONG` (binary) select which
flavor — `LONG` has text semantics, `LONG RAW` binary; and the streaming API
exists precisely because `LONG` can exceed any reasonable buffer — don't try
to fetch one into a fixed array.

## Try it yourself

1. Select `data_default` from `user_tab_columns` (a LONG) in explicit mode and dump it.
2. Insert a 100 KB text in two `OCI_LongWrite` pieces and read it back.

## Common pitfalls

- **Treating `OCI_Long*` as owned** — fetched longs are transient (statement-owned);
  only `OCI_LongCreate`d ones are freed.
- **Implicit mode on LONG RAW** — unsupported; stays explicit.
- **Forgetting `SetLongMode` before execute** — mode applies to the next execute.

## Recap

- `OCI_LONG_EXPLICIT` + `OCI_ResultsetGetLong` + `OCI_LongRead`/`Write` = streaming LONGs.
- `OCI_LONG_IMPLICIT` = plain strings when values are small.

**Next:** Checkpoint 1 — build a tiny query console with everything so far.
