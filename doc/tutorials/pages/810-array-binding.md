---
title: Array binding — bulk DML
navtitle: Array binding
part: 8
level: intermediate
demos: [array.c, array.cpp]
---

# Array binding — bulk DML

**Objectives** — by the end of this page you will be able to:

- bind whole arrays to one placeholder
- control batch size for the statement
- find bad rows in a batch (error row info)

## Concepts

One array-bind call links an **array of host variables** to a single placeholder — one execute then performs the DML for *every* array element.

That's the classic "insert 10k rows in one round-trip" trick:

:::sample
```c
#define N 1000
OCI_Int32  ids[N];
otext      names[N][31];
OCI_Double sals[N];

OCI_Prepare(st, OTEXT("insert into employees(empno, ename, sal) values(:i, :e, :s)"));
OCI_BindArrayOfInts   (st, OTEXT(":i"), ids,   N);
OCI_BindArrayOfStrings(st, OTEXT(":e"), (otext*)names, 30, N);
OCI_BindArrayOfDoubles(st, OTEXT(":s"), sals,  N);
OCI_BindArraySetSize(st, N);

OCI_Execute(st);                       /* N inserts, one call */
printf("%u rows\n", OCI_GetAffectedRows(st));
```

```cpp
std::vector<Int32>  ids(N);
std::vector<std::string> names(N);
std::vector<double>   sals(N);

st.Prepare("insert into employees(empno, ename, sal) values(:i, :e, :s)");
st.Bind(":i", ids,   BindInfo::In, BindInfo::AsArray);
st.Bind(":e", names, BindInfo::In, BindInfo::AsArray);
st.Bind(":s", sals,  BindInfo::In, BindInfo::AsArray);
st.SetBindArraySize(N);
st.ExecutePrepared();                  /* N inserts, one call */
```
:::

## The array-bind family

One call per C type: `Shorts`, `UnsignedShorts`, `Ints`, `UnsignedInts`, `BigInts`, `UnsignedBigInts`, `Floats`, `Doubles`, `Strings` (+elem size), `Raws`, `Numbers`, `Dates`, `Timestamps`, `Intervals`, `Lobs`, `Files`, `Objects`, `Colls`, `Refs`, `Jsons`, `Vectors`.

In C++ a `std::vector<T>` + `AsArray` dispatches on the type.

| C function | C++ method | Purpose |
|------------|------------|---------|
| `OCI_BindArraySetSize` | `SetBindArraySize` | how many array elements this execute uses |
| `OCI_BindArrayGetSize` | `GetBindArraySize()` | read it |
| `OCI_BindSetNullAtPos` | `BindInfo::SetDataNull(true, i)` | NULL flag per row |
| `OCI_BindSetNotNullAtPos` | `SetDataNull(false, i)` | back to NOT NULL |
| `OCI_BindIsNullAtPos` | `IsDataNull(i)` | read the flag |
| `OCI_GetAffectedRows` | `GetAffectedRows()` | total rows across the batch |

## Batch errors

When one row of the batch violates a constraint, the failure carries **row
information** — the error's row field identifies the offending array index
(`OCI_ErrorGetRow` / C++ `st.GetBatchErrors()` for the full list).

## Sizing rules

- The array size ≤ the capacity you bound — shrink per execute, never grow past
  capacity.
- Keep arrays alive through all executes (same binding rules as scalar binds).
- Batch size is a memory↔round-trip trade, like fetch size.

## Try it yourself

1. Insert 500 employees in one execute; verify `GetAffectedRows` and elapsed time
   vs row-by-row.
2. Set a NULL `comm` only at positions 3 and 7 via `BindSetNullAtPos`.
3. Force one bad row (duplicate empno) and print the failing index.

## Common pitfalls

- **`SetSize` > capacity** — UB/overflow; always ≤ bound array length.
- **String array element size** — `BindArrayOfStrings` needs per-element capacity.
- **Expecting per-row commits** — the batch is one execute; it commits as a unit.

## Recap

- `OCI_BindArrayOf*` + `BindArraySetSize` + one `Execute` = bulk DML;
  `BindSetNullAtPos` for sparse NULLs.

**Next:** direct path loading — the fastest bulk ingest of all.
