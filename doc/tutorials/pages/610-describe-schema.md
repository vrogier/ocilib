---
title: Describing schema objects — OCI_TypeInfo
navtitle: Describing schema objects
part: 6
level: intermediate
demos: [desc.c, meta.c]
---

# Describing schema objects — the `TypeInfo` descriptor

**Objectives** — by the end of this page you will be able to:

- describe tables, views and object types server-side
- walk `TypeInfo` metadata: name, kind, columns, super type
- use it as the factory key for objects, collections and REFs

## Meet the TypeInfo

A `TypeInfo` is a **server-side description** of a schema object — the same info `describe` gives you in SQL*Plus.

It does double duty: pure introspection, and as the *type descriptor* every object-API constructor needs — objects, collections, REFs and collection elements are all created *from* a TypeInfo, not from a connection directly.

:::sample
```c
OCI_TypeInfo *ti = OCI_TypeInfoGet(cn, OTEXT("employees"), OCI_TIF_TABLE);
/* … use it … */
OCI_TypeInfoFree(ti);
```

```cpp
TypeInfo ti(con, "employees", TypeInfo::Table);
```
:::

## Kinds — what to describe

| C enum | C++ enum | Describes |
|--------|----------|-----------|
| `OCI_TIF_TABLE` | `TypeInfo::Table` | a table — columns via `GetColumn{,Count}` |
| `OCI_TIF_VIEW` | `TypeInfo::View` | a view |
| `OCI_TIF_TYPE` | `TypeInfo::Type` | an object/collection type — attributes become column descriptors |

## The metadata surface

| C function | C++ method | Returns |
|------------|------------|---------|
| `OCI_TypeInfoGetName` | `GetName()` | object/type name |
| `OCI_TypeInfoGetType` | `GetType()` | kind detail (varies by kind flag) |
| `OCI_TypeInfoGetColumnCount` | `GetColumnCount()` | number of columns/attributes |
| `OCI_TypeInfoGetColumn` | `GetColumn(i)` | column descriptor (page 240) |
| `OCI_TypeInfoGetSuperType` | `GetSuperType()` | parent type for inherited types |
| `OCI_TypeInfoIsFinalType` | `IsFinalType()` | whether the type can be extended |
| `OCI_TypeInfoGetConnection` | `GetConnection()` | owning connection |

## Describe a table

:::sample
```c
OCI_TypeInfo *ti = OCI_TypeInfoGet(cn, OTEXT("employees"), OCI_TIF_TABLE);
printf("%s has %u columns\n",
       OCI_TypeInfoGetName(ti), OCI_TypeInfoGetColumnCount(ti));

for (OCI_UInt32 i = 1; i <= OCI_TypeInfoGetColumnCount(ti); i++)
{
    OCI_Column *col = OCI_TypeInfoGetColumn(ti, i);
    printf("  %-12s %s\n", OCI_ColumnGetName(col), OCI_ColumnGetSQLType(col));
}
OCI_TypeInfoFree(ti);
```

```cpp
TypeInfo ti(con, "employees", TypeInfo::Table);
std::cout << ti.GetName() << " has " << ti.GetColumnCount() << " columns\n";

for (UInt32 i = 1; i <= ti.GetColumnCount(); i++)
{
    Column col = ti.GetColumn(i);
    std::cout << "  " << col.GetName() << " " << col.GetSQLType() << "\n";
}
```
:::

## TypeInfo as factory input

:::sample
```c
OCI_TypeInfo *tt = OCI_TypeInfoGet(cn, OTEXT("t_vendor"), OCI_TIF_TYPE);
OCI_Object *obj = OCI_ObjectCreate(tt);      /* new t_vendor instance */
OCI_TypeInfoFree(tt);                        /* obj keeps what it needs */
```

```cpp
TypeInfo tt(con, "t_vendor", TypeInfo::Type);
Object obj(tt);                              /* new t_vendor instance */
```
:::

## Try it yourself

1. Describe `employees` and `departments` — compare with `describe` in SQL*Plus.
2. Describe an object type (`create type t_point as object(x number, y number)`)
   and print its attribute list.
3. Walk `GetSuperType` on a type created `under` another.

## Common pitfalls

- **`Type` kind on a table** — returns NULL; match the kind flag to the object.
- **Forgetting to free the handle** — owned, not borrowed (C++ RAII covers it).
- **Assuming attribute order matches declaration** — use `GetColumn` indexes 1..N.

## Recap

- TypeInfo = schema describe + the key that creates objects, collections, REFs and
  elements.

**Next:** objects — create them from a TypeInfo and fill their attributes.
