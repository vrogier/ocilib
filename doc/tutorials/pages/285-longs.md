---
title: LONG and LONG RAW — OCI_Long piecewise access
navtitle: LONG & LONG RAW
part: 2
level: intermediate
demos: [long.c]
sql: long.sql
---

# LONG and LONG RAW — the `Long` streaming object

**Objectives** — by the end of this page you will be able to:

- read `LONG`/`LONG RAW` columns in chunks through the Long object
- write long values piecewise on insert/update
- choose between explicit streaming and implicit string mode

## Meet the Long object

`LONG` (up to 2 GB text) and `LONG RAW` predate LOBs and don't fit normal row buffers — they arrive **piecewise**.

The `Long` object is a streaming buffer around one such value: on fetch it gives you a read cursor over the column's bytes, and on binding it collects the chunks you push before execute.

| C function | C++ method | Purpose |
|------------|------------|---------|
| `OCI_LongCreate` | `Long` ctor | make a LONG (`OCI_LONG`) / LONG RAW (`OCI_LONGRAW`) buffer for binding |
| `OCI_LongFree` | — (RAII) | release it |
| `OCI_LongGetSize` | `GetSize()` | current content size |
| `OCI_LongGetType` | `GetType()` | LONG vs LONG RAW |
| `OCI_LongGetBuffer` | `GetBuffer()` | raw content pointer |
| `OCI_LongRead` | `Read` | pull the next chunk (fetched column) |
| `OCI_LongWrite` | `Write` | push a chunk (binding) |
| `OCI_GetLong` | `Get<Long>(i)` | get the Long object of a fetched column |

Fetched Longs are **borrowed** — owned by the statement; only `LongCreate`d ones
are freed.

## Reading a LONG

:::sample
```c
OCI_SetLongMode(st, OCI_LONG_EXPLICIT);            /* ask for OCI_Long handles */
OCI_ExecuteStmt(st, OTEXT("select big_text from articles"));
OCI_Resultset *rs = OCI_GetResultset(st);

while (OCI_FetchNext(rs))
{
    OCI_Long *lg = OCI_GetLong(rs, 1);             /* borrowed — statement owns it */
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
    Long lg = row.Get<Long>(1);                     /* borrowed */
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

`OCI_SetPieceSize(st, n)` on the statement controls the streaming chunk size.

## Writing a LONG

:::sample
```c
OCI_Long *lg = OCI_LongCreate(st, OCI_LONG);
OCI_LongWrite(lg, chunk1, len1);
OCI_LongWrite(lg, chunk2, len2);                    /* append more pieces */

OCI_Prepare(st, OTEXT("insert into articles(id, big_text) values(:i, :t)"));
OCI_BindLong(st, OTEXT(":t"), lg, OCI_LONG);
OCI_Execute(st);
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
OCI_SetLongMode(st, OCI_LONG_IMPLICIT);
/* LONG columns now arrive via OCI_GetString — no OCI_Long needed.
   (does not apply to LONG RAW) */
```

```cpp
st.SetLongMode(Statement::LongImplicit);
/* LONG columns now arrive via Get<String>(i); Get<Long>(i) for the object */
```
:::

## Try it yourself

1. Select `data_default` from `user_tab_columns` (a LONG) in explicit mode and dump it.
2. Insert a 100 KB text in two `OCI_LongWrite` pieces and read it back.

## Common pitfalls

- **Treating `OCI_Long*` as owned** — fetched longs are transient (statement-owned);
  only `OCI_LongCreate`d ones are freed.
- **Implicit mode on LONG RAW** — unsupported; stays explicit.
- **Forgetting `SetLongMode` before execute** — mode applies to the next execute.

## Recap

- `OCI_LONG_EXPLICIT` + `OCI_GetLong` + `OCI_LongRead`/`Write` = streaming LONGs.
- `OCI_LONG_IMPLICIT` = plain strings when values are small.

**Next:** Checkpoint 1 — build a tiny query console with everything so far.
