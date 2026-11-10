---
title: Scrollable resultsets — fetch in any direction
navtitle: Scrollable resultsets
part: 2
level: intermediate
demos: [scroll.c, scroll.cpp]
---

# Scrollable resultsets — fetch in any direction

**Objectives** — by the end of this page you will be able to:

- enable scrollable mode and use all fetch directions
- jump to absolute or relative row positions
- use the bidirectional C++ `RowIterator`

## Enabling it

Fetch mode is a statement property set **before** execute — scrollable tells the
server to keep the whole result addressable:

:::sample
```c
OCI_SetFetchMode(st, OCI_SFM_SCROLLABLE);
OCI_ExecuteStmt(st, OTEXT("select empno, ename from employees order by empno"));
OCI_Resultset *rs = OCI_GetResultset(st);
```

```cpp
st.SetFetchMode(Statement::FetchScrollable);
st.Execute("select empno, ename from employees order by empno");
auto rs = st.GetResultset();
```
:::

| C enum | C++ enum | Meaning |
|--------|----------|---------|
| `OCI_SFM_DEFAULT` | `Statement::FetchDefault` | forward-only fetch |
| `OCI_SFM_SCROLLABLE` | `Statement::FetchScrollable` | bidirectional, seekable resultset |

## The fetch verbs

| C function | C++ method | Movement |
|------------|------------|----------|
| `OCI_FetchNext` | `Next()` | next row |
| `OCI_FetchPrev` | `Prev()` | previous row |
| `OCI_FetchFirst` | `First()` | jump to row 1 |
| `OCI_FetchLast` | `Last()` | jump to last row — also reveals the total |
| `OCI_FetchSeek` | `Seek(mode, offset)` | absolute or relative jump |

Seek directions:

| C enum | C++ enum | Meaning |
|--------|----------|---------|
| `OCI_SFD_ABSOLUTE` | `Resultset::SeekAbsolute` | jump to row `n` |
| `OCI_SFD_RELATIVE` | `Resultset::SeekRelative` | move `k` rows from current (±) |

:::sample
```c
OCI_FetchLast(rs);                                  /* land on last row        */
printf("total rows: %u\n", OCI_GetRowCount(rs));    /* full count now known    */

OCI_FetchSeek(rs, OCI_SFD_ABSOLUTE, 2);             /* row 3                   */
OCI_FetchSeek(rs, OCI_SFD_RELATIVE, -1);            /* back one → row 2        */
while (OCI_FetchPrev(rs))                           /* walk to the top         */
{
    printf("%s\n", OCI_GetString(rs, 1));
}
```

```cpp
rs.Last();
std::cout << "total rows: " << rs.GetCount() << "\n";

rs.Seek(Resultset::SeekAbsolute, 2);
rs.Seek(Resultset::SeekRelative, -1);
while (rs.Prev())
{
    std::cout << rs.Get<String>(1) << "\n";
}
```
:::

In scrollable mode the row count is *complete* right after a jump to the last row —
unlike default mode where it grows as you read.

## The C++ way — real iterators

Beyond mirroring the C verbs, the `RowIterator` from `begin()`/`end()` becomes
**bidirectional** in scrollable mode — random-access-style navigation over rows:

```cpp
rs.Last();
std::cout << "total: " << rs.GetCount() << "\n";

auto it = rs.Rows().begin();
it += 2;              // absolute-ish navigation
--it;                 // relative backwards
it[0];                // seek to a row — std::input_iterator + extras
```

## Try it yourself

1. Print employees in reverse order with `FetchLast` + `FetchPrev`.
2. Implement "page 3 of 10 rows per page" with `OCI_SFD_ABSOLUTE` seeks.
3. Compare `OCI_GetRowCount` before/after `FetchLast` in both fetch modes.

## Common pitfalls

- **Setting scrollable *after* execute** — no effect; it shapes the next execute.
- **Scrollable for giant tables** — server-side cursor keeps more state; use it for
  paged UIs, not bulk exports.
- **Switching mode back to default mid-resultset** — create a new statement instead.

## Recap

- Scrollable fetch mode + first/last/prev/seek = navigation in any direction.
- C++ iterators go bidirectional in scrollable mode (`--`, `+=`, `[]`).

**Next:** ROWID and DML `RETURNING` — round-trip-friendly row identity.
