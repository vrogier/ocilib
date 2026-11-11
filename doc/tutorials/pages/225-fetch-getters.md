---
title: Fetching rows — getters, indexes & NULLs
navtitle: Fetching rows
part: 3
level: beginner
demos: [fetch.c, fetch.cpp]
---

# Fetching rows — getters, indexes & NULLs

**Objectives** — by the end of this page you will be able to:

- drive the fetch loop and know what each call does to the cursor
- read columns by 0-based index **or** by name
- detect NULLs correctly
- use C++ `Get<T>`/`GetOptional<T>`/iterators

## The fetch state machine

```
Execute ──> before first row ──fetch──> row 1 ──fetch──> row 2 ──…──> done
```

The cursor moves one row at a time; getters only see the *current* row. The calls
that drive it:

| C function | C++ method | Role |
|------------|------------|------|
| `OCI_ResultsetFetchNext` | `Next()` / `operator++` | advance to the next row; `FALSE`/`false` at the end |
| `OCI_ResultsetGetCurrentRow` | `GetCurrentRow()` | 0-based index of the row you're on |
| `OCI_ResultsetGetRowCount` | `GetCount()` | rows fetched so far |
| `OCI_ResultsetGetColumnCount` | `GetColumnCount()` | how many columns the row has |
| `OCI_ResultsetGetColumnIndex` | `GetColumnIndex(name)` | resolve a column name to its index |

## By index or by name

Every getter comes in pairs — the `ByName` variant resolves the name to an index:

:::sample
```c
while (OCI_ResultsetFetchNext(rs))
{
    printf("%6d %-12s %8.2f\n",
           OCI_ResultsetGetInt32(rs, 0),                          /* by index    */
           OCI_ResultsetGetString(rs, 1),
           OCI_ResultsetGetDouble(rs, OCI_ResultsetGetColumnIndex(rs, OTEXT("sal"))));
    /* or directly: OCI_ResultsetGetDoubleByName(rs, OTEXT("sal")) */
}
```

```cpp
for (auto &&row : rs.Rows())
{
    std::cout << row.Get<int>(0) << ' '
              << row.Get<String>(1) << ' '
              << row.Get<double>("sal") << '\n';     // by name
}
```
:::

Index access is faster (no name lookup) — use names for readability, indexes in hot
loops.

## NULL handling — the rules

| C function | C++ method | Role |
|------------|------------|------|
| `OCI_ResultsetIsColumnNull` | `IsNull` | test the column **before** reading |
| `OCI_ResultsetIsColumnNullByName` | `IsNull` (by name) | same, resolved by column name |
| — | `GetOptional<T>` | returns `std::optional<T>` — `nullopt` when NULL |

Getting a NULL numeric returns `0`; a NULL string returns `NULL` — never rely on it,
test first. In C++, `GetOptional<T>` is the idiomatic NULL story — it wraps any
getter:

```cpp
if (auto comm = row.GetOptional<double>(4))
{
    std::cout << *comm;
}
```

## Row counting subtleties

- The fetched-row count grows as you fetch in **default** mode; in **scrollable**
  mode (page 275) the full count is known upfront.
- Rows-affected-on-the-statement is the DML counterpart — don't confuse the two.

## Putting it together — the typical sequence

The fetch loop is muscle memory in OCILIB — it never changes shape:

1. **Execute** the SELECT.
2. **`FetchNext` in a `while`** — FALSE at EOF, row data live inside the loop.
3. **Pick the getter that matches the column**: `GetInt32` for `NUMBER` ints,
   `GetString` for `VARCHAR2`, `GetDouble` for `NUMBER` floats — each returns
   a typed value or a borrowed handle.
4. **Borrowed vs owned**: `GetString`/`GetLob`/`GetObject` return pointers the
   *row* owns — never free them, never keep them past the next `FetchNext`.
5. **`IsColumnNull` first** on anything nullable — the getters don't protect
   you from NULL columns.

Hints: `GetString` works on *any* column — it converts — so when you're
unsure of the SQL type, text is the safe read; by-name getters are slower than
by-index but self-documenting; and the biggest beginner bug is storing the
`otext*` past the next fetch — the row buffer is reused, copy if you need it.

## Try it yourself

1. Fetch `empno, ename, comm` and replace NULL comm with `0.00` in the output —
   once with `OCI_ResultsetIsColumnNull`, once (C++) with `GetOptional`.
2. Print `OCI_ResultsetGetCurrentRow` inside the loop and `OCI_ResultsetGetRowCount` after it.
3. Select a column alias `ename employee` and fetch it by the alias name.

## Common pitfalls

- **String-getter on a NUMBER column** — you get a conversion where supported, but
  always prefer the typed getter; implicit conversions hide bugs.
- **Name lookups are case-sensitive** as Oracle returns them — usually UPPER CASE;
  quote the alias in SQL if you want lowercase.
- **Using the resultset after its statement is freed** — the rs dies with it.

## Recap

- Fetch → typed getters (index or `*ByName`) → NULL test first for nullable cols.
- `GetOptional<T>` is the idiomatic C++17 NULL story.

**Next:** the complete getter → SQL type map.
