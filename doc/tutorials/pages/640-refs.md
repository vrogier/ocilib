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

## Meet the Ref

A `REF` column stores a **persistent pointer** to an object row — not the object itself.

A `Ref` is the client-side locator: created from the referenced type's `TypeInfo` when you build one, borrowed when fetched, and it *dereferences* — "pins" the pointed-to row as an `Object` you can read.

```sql
create table sales     of t_sale;
create table vendors   of t_vendor;
alter table sales add (vend_ref ref t_vendor scope is vendors);
```

| C function | C++ method | Purpose |
|------------|------------|---------|
| `OCI_RefCreate` | `Reference` ctor (`ti`) | lifecycle |
| `OCI_RefFree` | — (RAII) | release |
| `OCI_GetRef` | `Get<Ref>(i)` / `Get<Reference>` | fetch REF columns |
| `OCI_BindRef` | `Bind(":r", ref)` | bind REF columns |
| `OCI_RefGetObject` | `GetObject()` | deref → pinned `Object` |
| `OCI_RefIsNull` | `IsNull()` | NULL REFs |
| `OCI_RefSetNull` | `SetNull()` | NULL REFs |
| `OCI_RefAssign` | `Assign` | copy |
| `OCI_RefGetTypeInfo` | `GetTypeInfo()` | describe target type |
| `OCI_RefToText` | `ToText()` | debug the ref bytes |
| `OCI_RefGetHexSize` | `GetHexSize()` | ref byte size |

## Deref walk

:::sample
```c
OCI_ExecuteStmt(st, OTEXT("select vend_ref from sales"));
OCI_Resultset *rs = OCI_GetResultset(st);
while (OCI_FetchNext(rs))
{
    OCI_Ref *r = OCI_GetRef(rs, 0);
    if (!OCI_RefIsNull(r))
    {
        OCI_Object *v = OCI_RefGetObject(r);     /* follow the pointer */
        printf("vendor %d: %s\n",
               OCI_ObjectGetInt(v, OTEXT("CODE")),
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

- `OCI_ObjectGetSelfRef(obj)` — a REF to a *fetched/persisted* object; bind it into a
  REF column to link rows.
- `OCI_RefCreate(typinf)` + `OCI_BindRef` — store a REF you built.

:::sample
```c
OCI_Ref *r = OCI_ObjectGetSelfRef(vendor_obj);
OCI_ObjectSetRef(sale_obj, OTEXT("VEND_REF"), r);
OCI_RefFree(r);
```

```cpp
Ref r = vendor_obj.GetSelfRef();
sale_obj.Set<Ref>("VEND_REF", r);
```
:::

## Try it yourself

1. Build `vendors` + `sales` with a scoped REF, link one sale to a vendor, deref it.
2. `RefToText` a REF and compare two refs to the same row.
3. Create a circular pair (sale ↔ vendor refs) and traverse both directions.

## Common pitfalls

- **Deref of a NULL ref** — check `RefIsNull` first.
- **Ref to a deleted row** — dangling REFs fail on `GetObject`.
- **`GetObject` result is owned** — free the pinned object.

## Recap

- `OCI_Ref` = persistent object pointer; `GetRef`/`BindRef` move it,
  `RefGetObject` dereferences it.

**Next:** case study — a full object graph (geometry demo).
