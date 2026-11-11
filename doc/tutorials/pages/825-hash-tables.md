---
title: Hash tables — OCI_HashTable
navtitle: Hash tables
part: 8
level: beginner
demos: [hash.c]
---

# Hash tables — the `HashTable` helper

**Objectives** — by the end of this page you will be able to:

- create and use OCILIB's public hash table helper
- store ints and pointers keyed by strings
- iterate entries for caching/indexing patterns

## The hash table

A small public utility — string keys, values typed as int or pointer — handy for caching metadata, building name→value maps, or deduplication in your app's code.

It's an OCILIB convenience, **not** an Oracle feature — a purely client-side container.

```c
OCI_HashTable *ht = OCI_HashTableCreate(env, 256, OCI_HASH_INTEGER);

OCI_HashTableAddInt32(ht, OTEXT("SMITH"), 7369);
OCI_HashTableAddInt32(ht, OTEXT("ALLEN"), 7499);
printf("%d\n", OCI_HashTableGetInt32(ht, OTEXT("SMITH")));
OCI_HashTableFree(ht);
```

| Function | Purpose |
|----------|---------|
| `OCI_HashTableCreate(env, size, type)` | `OCI_HASH_INTEGER` or `OCI_HASH_POINTER` |
| `OCI_HashTableAdd{String,Int32,Pointer}` | add entries (string→string/int/ptr) |
| `OCI_HashTableGet{String,Int32,Pointer}` / `OCI_HashTableLookup` | lookup |
| `OCI_HashTableGetSize` | entry count |
| `OCI_HashTableGetEntry(ht, i)` → `OCI_HashEntry*` | indexed iteration |
| `OCI_HashTableGetValue` / `OCI_HashTableGetType` | entry payload/kind |
| `OCI_HashTableFree` | release |

## Iterating

```c
for (OCI_UInt32 i = 0; i < OCI_HashTableGetSize(ht); i++)
{
    OCI_HashEntry *e = OCI_HashTableGetEntry(ht, i);
    /* entry carries the key + typed value */
}
```

## Where it earns its keep

- Cache TypeInfo/column metadata per table name (avoid repeated describes).
- Index fetched rows by a key column for in-memory joins.
- Name → handler dispatch tables in tools.

**No C++ twin** — `std::unordered_map` does it better; the C++ API doesn't wrap the
hash table.

## Putting it together — the typical sequence

`OCI_HashTable` is OCILIB's own client-side structure — no Oracle involved:

1. **Create** — `OCI_HashTableCreate(size, type)` picks the key/value kind.
2. **Populate** — `HashTableAdd`/`HashTableAddString`/`AddInt`/`AddPointer`
   per entry type.
3. **Query** — `HashTableLookup`/`GetString`/`GetInt`/`GetPointer`.
4. **Iterate** — `HashTableGetEntry` walks the table.

Hints: hash tables are the library's internals, exposed — use them for your
own lookups, they're O(1) and standalone; `GetSize`/`GetValue` report count
and payload; and the type picked at create decides which `Add*`/`Get*`
variants apply — mixing them fails.

## Try it yourself

1. Load `ename → empno` from `employees` into a hash; look up three names.
2. Cache `OCI_ColumnGetName` results per table while building your `qtool` header.

## Recap

- `OCI_HashTable` = simple public helper; string keys, int/pointer values, indexed
  iteration.

**Next:** Checkpoint 7 — a CSV bulk loader comparing array binds vs direct path.
