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

## Meet the collection and the element

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
OCI_Coll *coll = OCI_CollCreate(ti);          /* element type from the TypeInfo */

OCI_Elem *e = OCI_ElemCreate(ti);             /* a product_t element slot      */
OCI_Object *p = OCI_ElemGetObject(e);         /* object element → set attrs    */
OCI_ObjectSetInt   (p, OTEXT("CODE"), 123);
OCI_ObjectSetString(p, OTEXT("NAME"), OTEXT("usb key"));
OCI_CollAppend(coll, e);
OCI_ElemFree(e);

OCI_Prepare(st, OTEXT("insert into vendors values(1, :c)"));
OCI_BindColl(st, OTEXT(":c"), coll);
OCI_Execute(st);
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
OCI_ElemSetString(e, OTEXT("hello"));
OCI_CollAppend(coll, e);
```

```cpp
e.Set<String>("hello");
coll.Append(e);
```
:::

## Inspecting

| C function | C++ method | Meaning |
|------------|------------|---------|
| `OCI_CollGetSize` | `GetSize()` | current element count |
| `OCI_CollGetMax` | `GetMax()` | capacity (VARRAY bound; 0 = unbounded) |
| `OCI_CollGetCount` | `GetCount()` | same as size for most cases |
| `OCI_CollGetType` | `GetType()` | element/collection type info |
| `OCI_CollGetTypeInfo` | `GetTypeInfo()` | describing TypeInfo |
| `OCI_CollGetElem` | `Get<Elem>(i)` / typed `Get(i)` | element at index `i` (0-based) |
| `OCI_CollSetElem` | `Set` | replace element `i` |
| `OCI_CollClear` | `Clear()` | empty it |
| `OCI_CollTrim` | `Trim(n)` | drop trailing elements |
| `OCI_CollDeleteElem` | `DeleteElem(i)` | remove element `i` |
| `OCI_CollAssign` | `Assign` | copy |
| `OCI_CollToText` | `ToText()` | debug dump |

## Fetching

:::sample
```c
OCI_Coll *c = OCI_GetColl(rs, 0);                 /* borrowed */
for (OCI_UInt32 i = 0; i < OCI_CollGetSize(c); ++i)
{
    OCI_Elem *e = OCI_CollGetElem(c, i);
    printf("%s\n", OCI_ElemGetString(e));
    OCI_ElemFree(e);
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

**Next:** iterating collections with `OCI_Iter` and nested collections.
