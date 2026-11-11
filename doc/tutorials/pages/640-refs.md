---
title: REFs — pointers between objects
navtitle: REFs
part: 6
level: advanced
demos: [ref.c]
sql: ref.sql
---

# REFs — pointers between objects

**Objectives** — by the end of this page you will be able to:

- fetch and bind `REF` columns
- dereference a REF into its object
- wire object graphs with self-refs

## The Ref

A `REF` column stores a **persistent pointer** to an object row — not the object itself.

A `Ref` is the client-side locator: created from the referenced type's `TypeInfo` when you build one, borrowed when fetched, and it *dereferences* — "pins" the pointed-to row as an `Object` you can read.

```sql
create table sales     of t_sale;
create table vendors   of t_vendor;
alter table sales add (vend_ref ref t_vendor scope is vendors);
```

| C function | C++ method | Purpose |
|------------|------------|---------|
| `OCI_ReferenceCreate` | `Reference` ctor (`ti`) | lifecycle |
| `OCI_ReferenceFree` | — (RAII) | release |
| `OCI_ResultsetGetReference` | `Get<Ref>(i)` / `Get<Reference>` | fetch REF columns |
| `OCI_StatementBindReference` | `Bind(":r", ref)` | bind REF columns |
| `OCI_ReferenceGetObject` | `GetObject()` | deref → pinned `Object` |
| `OCI_ReferenceIsNull` | `IsNull()` | NULL REFs |
| `OCI_ReferenceSetNull` | `SetNull()` | NULL REFs |
| `OCI_ReferenceAssign` | `Assign` | copy |
| `OCI_ReferenceGetTypeInfo` | `GetTypeInfo()` | describe target type |
| `OCI_ReferenceToText` | `ToText()` | debug the ref bytes |
| `OCI_ReferenceGetHexSize` | `GetHexSize()` | ref byte size |

## Deref walk

:::sample
```c
OCI_StatementExecute(st, OTEXT("select vend_ref from sales"));
OCI_Resultset *rs = OCI_StatementGetResultset(st);
while (OCI_ResultsetFetchNext(rs))
{
    OCI_Reference *r = OCI_ResultsetGetReference(rs, 0);
    if (!OCI_ReferenceIsNull(r))
    {
        OCI_Object *v = OCI_ReferenceGetObject(r);     /* follow the pointer */
        printf("vendor %d: %s\n",
               OCI_ObjectGetInt32(v, OTEXT("CODE")),
               OCI_ObjectGetString(v, OTEXT("NAME")));
        OCI_ObjectFree(v);
    }
}
```

```cpp
st.Execute("select vend_ref from sales");
for (auto &&row : st.GetResultset().Rows())
{
    Ref r = row.Get<Ref>(0);
    if (!r.IsNull())
    {
        Object v = r.GetObject();                /* follow the pointer */
        std::cout << "vendor " << v.Get<int>("CODE") << ": "
                  << v.Get<String>("NAME") << "\n";
    }
}
```
:::

## Writing REFs

- `OCI_ObjectGetSelfReference(obj, ref)` — a REF to a *fetched/persisted* object; bind it into a
  REF column to link rows.
- `OCI_ReferenceCreate(typinf)` + `OCI_StatementBindReference` — store a REF you built.

:::sample
```c
OCI_Reference *r = OCI_ReferenceCreate(OCI_ObjectGetTypeInfo(vendor_obj));
OCI_ObjectGetSelfReference(vendor_obj, r);
OCI_ObjectSetReference(sale_obj, OTEXT("VEND_REF"), r);
OCI_ReferenceFree(r);
```

```cpp
Ref r = vendor_obj.GetSelfRef();
sale_obj.Set<Ref>("VEND_REF", r);
```
:::

## Putting it together — the typical sequence

`REF`s are Oracle's object-pointers:

1. **Fetch a REF column** — `OCI_ResultsetGetReference` gives a borrowed
   `OCI_Reference` pointing at an object row.
2. **Dereference it** — `OCI_ReferenceGetObject` follows the pointer to the
   real `OCI_Object`.
3. **Bind REFs** — `StatementBindRef` for REF-typed columns or params.
4. **Make one** — `ObjectGetSelfReference` mints a REF for an object you
   have.

Hints: a REF is a pointer into the object table — dangling REFs happen when
the target row is deleted; `ReferenceGetObject` is a round-trip per call —
batch your dereferences; and REF columns in `SELECT` give you the pointer —
deref lazily, only when you need the attributes.

## Try it yourself

1. Build `vendors` + `sales` with a scoped REF, link one sale to a vendor, deref it.
2. `RefToText` a REF and compare two refs to the same row.
3. Create a circular pair (sale ↔ vendor refs) and traverse both directions.

## Common pitfalls

- **Deref of a NULL ref** — check `RefIsNull` first.
- **Ref to a deleted row** — dangling REFs fail on `GetObject`.
- **`GetObject` result is owned** — free the pinned object.

## Recap

- `OCI_Reference` = persistent object pointer; `GetRef`/`BindRef` move it,
  `RefGetObject` dereferences it.

**Next:** case study — a full object graph (geometry demo).
