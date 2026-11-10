---
title: Fetching rows — getters, indexes & NULLs
navtitle: Fetching rows
part: 2
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
| `OCI_FetchNext` | `Next()` / `operator++` | advance to the next row; `FALSE`/`false` at the end |
| `OCI_GetCurrentRow` | `GetCurrentRow()` | 0-based index of the row you're on |
| `OCI_GetRowCount` | `GetCount()` | rows fetched so far |
| `OCI_GetColumnCount` | `GetColumnCount()` | how many columns the row has |
| `OCI_GetColumnIndex` | `GetColumnIndex(name)` | resolve a column name to its index |

## By index or by name

Every getter comes in pairs — the `ByName` variant resolves the name to an index:

:::sample
```c
while (OCI_FetchNext(rs))
{
    printf("%6d %-12s %8.2f\n",
           OCI_GetInt(rs, 0),                          /* by index    */
           OCI_GetString(rs, 1),
           OCI_GetDouble(rs, OCI_GetColumnIndex(rs, OTEXT("sal"))));
    /* or directly: OCI_GetDoubleByName(rs, OTEXT("sal")) */
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
| `OCI_IsNull` | `IsNull` | test the column **before** reading |
| `OCI_IsNullByName` | `IsNull` (by name) | same, resolved by column name |
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

## Try it yourself

1. Fetch `empno, ename, comm` and replace NULL comm with `0.00` in the output —
   once with `OCI_IsNull`, once (C++) with `GetOptional`.
2. Print `OCI_GetCurrentRow` inside the loop and `OCI_GetRowCount` after it.
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
