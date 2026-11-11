---
title: Collections — VARRAY & nested tables
navtitle: Collections basics
part: 6
level: intermediate
demos: [coll.c, coll.cpp]
sql: collections.sql
---

# Collections — VARRAY & nested tables

**Objectives** — by the end of this page you will be able to:

- create and fill collections
- bind/fetch collection columns
- get sizes, max sizes and element handles

## The collection and the element

Oracle has two collection flavors: bounded `VARRAY(n)` and unbounded nested tables (`table of …`).

On the client, a `Coll` is the container — created *from* the collection's `TypeInfo` — and an `Elem` is a **slot for one element**: you create an element, fill it (scalar setters, or an Object/Coll for structured elements), and append it into the collection.

Elements pulled out of a collection are owned refs to free; the collection itself is borrowed when fetched.

## Setup

```sql
create type product_t as object (code number, name varchar2(50));
create type product_varray_t as varray(10) of product_t;
create type t_names as table of varchar2(30);      -- nested table type
create table vendors (code number, products product_varray_t);
```

## Create & fill

:::sample
```c
OCI_TypeInfo *ti = OCI_TypeInfoGet(cn, OTEXT("product_varray_t"), OCI_TIF_TYPE);
OCI_Collection *coll = OCI_CollectionCreate(ti);          /* element type from the TypeInfo */

OCI_Element *e = OCI_ElementCreate(ti);             /* a product_t element slot      */
OCI_Object *p = OCI_ElementGetObject(e);         /* object element → set attrs    */
OCI_ObjectSetInt32   (p, OTEXT("CODE"), 123);
OCI_ObjectSetString(p, OTEXT("NAME"), OTEXT("usb key"));
OCI_CollectionAppend(coll, e);
OCI_ElementFree(e);

OCI_StatementPrepare(st, OTEXT("insert into vendors values(1, :c)"));
OCI_StatementBindCollection(st, OTEXT(":c"), coll);
OCI_StatementExecutePrepared(st);
```

```cpp
TypeInfo ti(con, "product_varray_t", TypeInfo::Type);
Collection<Object> coll(ti);                  /* element type from the TypeInfo */

Elem e(ti);                                   /* a product_t element slot      */
Object p = e.Get<Object>();                   /* object element → set attrs    */
p.Set<int>   ("CODE", 123);
p.Set<String>("NAME", "usb key");
coll.Append(e);

st.Prepare("insert into vendors values(1, :c)");
st.Bind(":c", coll);
st.ExecutePrepared();
```
:::

Scalar collections (`table of varchar2`) are simpler — set the element's scalar:

:::sample
```c
OCI_ElementSetString(e, OTEXT("hello"));
OCI_CollectionAppend(coll, e);
```

```cpp
e.Set<String>("hello");
coll.Append(e);
```
:::

## Inspecting

| C function | C++ method | Meaning |
|------------|------------|---------|
| `OCI_CollectionGetSize` | `GetSize()` | current element count |
| `OCI_CollectionGetMax` | `GetMax()` | capacity (VARRAY bound; 0 = unbounded) |
| `OCI_CollectionGetCount` | `GetCount()` | same as size for most cases |
| `OCI_CollectionGetType` | `GetType()` | element/collection type info |
| `OCI_CollectionGetTypeInfo` | `GetTypeInfo()` | describing TypeInfo |
| `OCI_CollectionGetElement` | `Get<Elem>(i)` / typed `Get(i)` | element at index `i` (0-based) |
| `OCI_CollectionSetElement` | `Set` | replace element `i` |
| `OCI_CollectionClear` | `Clear()` | empty it |
| `OCI_CollectionTrim` | `Trim(n)` | drop trailing elements |
| `OCI_CollectionDeleteElement` | `DeleteElem(i)` | remove element `i` |
| `OCI_CollectionAssign` | `Assign` | copy |
| `OCI_CollectionToText` | `ToText()` | debug dump |

## Fetching

:::sample
```c
OCI_Collection *c = OCI_ResultsetGetColl(rs, 0);                 /* borrowed */
for (OCI_UInt32 i = 0; i < OCI_CollectionGetSize(c); ++i)
{
    OCI_Element *e = OCI_CollectionGetElement(c, i);
    printf("%s\n", OCI_ElementGetString(e));
    OCI_ElementFree(e);
}
```

```cpp
Coll c = row.Get<Coll>(0);                          /* borrowed */
for (UInt32 i = 0; i < c.GetSize(); ++i)
{
    Elem e = c.GetElem(i);                          /* owned — RAII frees */
    std::cout << e.Get<String>() << "\n";
}
```
:::

## The C++ way — typed `Collection<T>` and iterators

C++ wraps the collection in `Collection<T>` — typed on the element type so indexing
and iteration yield `T` directly, no element handle to free:

```cpp
Collection<String> names = row.Get<Collection<String>>(1);
for (auto &&n : names)                 /* CollectionIterator — range-for */
{
    std::cout << n << "\n";
}

Collection<Object> prods = row.Get<Collection<Object>>(2);
for (auto &&p : prods)
{
    std::cout << p.Get<int>("CODE") << "\n";
}
```

## Putting it together — the typical sequence

Collections are array-typed columns — VARRAYs and nested tables:

1. **Describe the collection type** — `TypeInfoGet` on the collection type
   itself.
2. **Create or fetch** — `CollectionCreate` for building, `Get<Collection>`
   on a fetched column.
3. **Walk elements** — `CollectionGetCount`, `GetElem`/`GetElem2` per index,
   `CollectionNext` to iterate.
4. **Mutate** — `Append`, `DeleteElem`, `SetElem` for writes; fetched
   collections are borrowed.
5. **Bind** — `StatementBindColl` for passing a collection as a parameter.

Hints: `GetElem` returns a borrowed element — `GetElem2` gives one you can
modify; `CollectionNext` is the iteration idiom — `Reset` rewinds it; and
VARRAYs are bounded (max size in the type), nested tables aren't —
`CollectionGetMax` reports the bound or 0 for unbounded.

## Try it yourself

1. Build a `table of varchar2` of employee names and insert it.
2. Fetch a `product_varray_t` column and print size vs max.
3. `Trim` the last two elements and re-insert.

## Common pitfalls

- **`CollGetElem`/`GetElem` indexes** — 0-based, like everything OCILIB.
- **Forgetting `ElemFree`** — elements pulled from a coll are owned refs.
- **Nested-table vs VARRAY max** — `GetMax` is 0 for unbounded nested tables.

## Recap

- `TypeInfo` → `CollCreate` → `ElemCreate`+fill → `CollAppend` → `BindColl`;
  fetched via `GetColl` + `CollGetElem`.

**Next:** iterating collections with `OCI_Iterator` and nested collections.
