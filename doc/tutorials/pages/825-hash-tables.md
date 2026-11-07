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

## Meet the hash table

A small public utility — string keys, values typed as int or pointer — handy for caching metadata, building name→value maps, or deduplication in your app's code.

It's an OCILIB convenience, **not** an Oracle feature — a purely client-side container.

```c
OCI_HashTable *ht = OCI_HashCreate(env, 256, OCI_HASH_INTEGER);

OCI_HashAddInt(ht, OTEXT("SMITH"), 7369);
OCI_HashAddInt(ht, OTEXT("ALLEN"), 7499);
printf("%d\n", OCI_HashGetInt(ht, OTEXT("SMITH")));
OCI_HashFree(ht);
```

| Function | Purpose |
|----------|---------|
| `OCI_HashCreate(env, size, type)` | `OCI_HASH_INTEGER` or `OCI_HASH_POINTER` |
| `OCI_HashAdd{String,Int,Pointer}` | add entries (string→string/int/ptr) |
| `OCI_HashGet{String,Int,Pointer}` / `OCI_HashLookup` | lookup |
| `OCI_HashGetSize` | entry count |
| `OCI_HashGetEntry(ht, i)` → `OCI_HashEntry*` | indexed iteration |
| `OCI_HashGetValue` / `OCI_HashGetType` | entry payload/kind |
| `OCI_HashFree` | release |

## Iterating

```c
for (OCI_UInt32 i = 0; i < OCI_HashGetSize(ht); i++)
{
    OCI_HashEntry *e = OCI_HashGetEntry(ht, i);
    /* entry carries the key + typed value */
}
```

## Where it earns its keep

- Cache TypeInfo/column metadata per table name (avoid repeated describes).
- Index fetched rows by a key column for in-memory joins.
- Name → handler dispatch tables in tools.

**No C++ twin** — `std::unordered_map` does it better; the C++ API doesn't wrap the
hash table.

## Try it yourself

1. Load `ename → empno` from `employees` into a hash; look up three names.
2. Cache `OCI_ColumnGetName` results per table while building your `qtool` header.

## Recap

- `OCI_HashTable` = simple public helper; string keys, int/pointer values, indexed
  iteration.

**Next:** Checkpoint 7 — a CSV bulk loader comparing array binds vs direct path.
