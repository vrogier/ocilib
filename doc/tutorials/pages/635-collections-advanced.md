---
title: Collections — OCI_Iterator, element types & nesting
navtitle: Collections advanced
part: 6
level: advanced
---

# Collections — `Iter`, element types & nesting

**Objectives** — by the end of this page you will be able to:

- iterate collections forward/backward with the iterator object
- use the full element get/set matrix (same types as objects)
- handle collections of objects and nested collections

## The iterator — the traversal handle

An `Iter` is a cursor over a collection — the idiomatic traversal: no index
arithmetic, works for varrays and nested tables alike:

:::sample
```c
OCI_Iterator *it = OCI_IteratorCreate(coll);
OCI_Element *e  = OCI_IteratorGetNext(it);          /* NULL when done */
while (e)
{
    /* read e */
    e = OCI_IteratorGetNext(it);
}
OCI_IteratorFree(it);
```

```cpp
for (auto &&e : coll)                        /* CollectionIterator */
{
    /* read e */
}
```
:::

| C function | C++ method | Purpose |
|------------|------------|---------|
| `OCI_IteratorCreate` | `CollectionIterator` / `begin()` | traversal handle |
| `OCI_IteratorGetNext` | `it++` | next element (NULL/end at done) |
| `OCI_IteratorGetPrev` | `it--` | previous element |
| `OCI_IteratorGetCurrent` | `*it` | current element again |
| `OCI_IteratorFree` | — (RAII) | release |

## The element — the same matrix as objects

The `Elem` slot supports the identical get/set family as `Object` (one per SQL
type — C++ `ElemGet`/typed `Get<T>` dispatch on the element type):

| Get | Set | For |
|-----|-----|-----|
| `ElemGet{Short,UShort,Int,UInt,BigInt,BigUInt,Float,Double}` | `ElemSet…` | numerics |
| `ElemGetBoolean` | `ElemSetBoolean` | booleans |
| `ElemGetString` / `ElemGetRaw`+`GetRawSize` | `ElemSetString`/`SetRaw` | text/binary |
| `ElemGetDate` / `Timestamp` / `Interval` / `Number` | `ElemSet…` | temporals/decimal |
| `ElemGetLob` / `ElemGetFile` | `ElemSetLob` / `SetFile` | large data |
| `ElemGetObject` | `ElemSetObject` | object elements |
| `ElemGetColl` | `ElemSetColl` | **nested collections** |
| `ElemGetRef` | `ElemSetRef` | REF elements |
| `ElemIsNull` | `ElemSetNull` | NULL elements |

:::sample
```c
/* nested collection: a coll inside a coll element */
OCI_Collection *inner = OCI_ElementGetCollection(e);        /* e is element of the outer coll */
```

```cpp
/* nested collection: a coll inside a coll element */
Coll inner = e.Get<Coll>();                  /* e is element of the outer coll */
```
:::

## Binding collections to PL/SQL & SQL

- The collection bind (`OCI_StatementBindCollection` / `Bind(":c", coll)`) passes a whole
  collection as one bind.
- The coll getter on a row (`OCI_ResultsetGetColl` / `Get<Coll>` / `Get<Collection<T>>`)
  fetches collection columns.
- `ObjectSetColl` wires a collection-valued object attribute.

## Putting it together — the typical sequence

Advanced collection work adds ordering, trimming, and nested shapes:

1. **Insert at position** — `CollectionInsert`/`InsertElem` puts a value at an
   index, shifting the rest.
2. **Trim from the tail** — `CollectionTrim` removes the last N elements —
   nested-table style, since VARRAYs can't have holes.
3. **Iterate by value** — `CollGetElem` per index for reads, `CollSetElem`
   for writes.
4. **Nested collections** — an element can itself be a collection;
   `CollGetElem` on it returns another `OCI_Collection`.

Hints: `Delete` (nested tables) leaves holes — `Trim` doesn't; iterating
holes returns NULL indicators — handle them or `SetElem` over them; and
collections of objects use the same API — `GetElem` returns an `OCI_Object*`
when the element type is object.

## Try it yourself

1. Iterate a varray forward then backward (`GetNext` → end, `GetPrev` → start).
2. Create `table of t_person` (object coll) and print each object's `NAME`.
3. Build `table of t_names` (coll of coll) — append inner colls as elements.

## Common pitfalls

- **Iterating past the end** — returns NULL/end, not an error; end your loop on it.
- **Leaking elements** — elements pulled from a coll are owned refs to free.
- **Setting elem type mismatch** — a string setter on an object-element coll fails.

## Recap

- The iterator = clean traversal; the element mirrors the object accessor matrix;
  collections nest arbitrarily.

**Next:** REFs — pointers between objects.
