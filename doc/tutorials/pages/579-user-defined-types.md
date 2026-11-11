---
title: User-defined types — OCI_Object
navtitle: User types
part: 2
level: intermediate
demos: [object.c]
---

# User-defined types — `OCI_Object` / `Object`

**Objectives** — by the end of this page you will be able to:

- describe a schema `CREATE TYPE` with `OCI_TypeInfo`
- create, read and write object attributes
- bind and fetch objects in SQL and PL/SQL

## In context — schema objects vs PL/SQL records

Oracle gives you two ways to build a composite type, and they land on different
OCILIB paths. A **UDT** (`CREATE TYPE … AS OBJECT`) is a *schema-level* object —
it lives in the data dictionary, can be a column type in any table, and can be
returned by SQL functions. A **PL/SQL RECORD** (previous page) is anonymous and
package-scoped — it exists only inside PL/SQL execution. Both map to
`OCI_Object`, but only the UDT can appear in a SELECT list or a column
definition.

Describing is the entry point for both: `OCI_TypeInfoGet` walks the data
dictionary and hands you back the attribute layout — names, types, order — which
is what the `Get`/`Set` accessors then index. Because describing hits the
dictionary, OCILIB caches type info internally; the `TypeInfo` object itself is
borrowed (owned by the connection's cache) while each `Object` you create is
yours to free.

## The Object handle

A **user-defined type** (UDT) is a schema-level `CREATE TYPE … AS OBJECT` — a
structured row of named attributes, stored as a table column or passed to
PL/SQL.

OCILIB represents every UDT instance as **`OCI_Object`** (C) / **`Object`**
(C++) — the same handle that backs PL/SQL `RECORD`s (previous page). Describe
the type once, then create instances, get/set attributes by name, and bind them
into statements.

| C function | C++ method | purpose |
|------------|------------|---------|
| `OCI_TypeInfoGet` (`OCI_TIF_TYPE`) | `TypeInfo` ctor | describe the UDT |
| `OCI_ObjectCreate` | `Object` ctor | instantiate |
| `OCI_ObjectFree` | — (RAII) | release |
| `OCI_ObjectGet{Type}` / `OCI_ObjectSet{Type}` | `Get<T>` / `Set<T>` | attribute access |
| `OCI_ObjectGetStruct` | — | all attrs → C struct |
| `OCI_StatementBindObject` | `Bind(":o", obj)` | SQL/PLSQL param |
| `OCI_ResultsetGetObject` | `Get<Object>` | fetch a UDT column |

:::sample
```c
OCI_TypeInfo *ti = OCI_TypeInfoGet(con, OTEXT("VENDOR"), OCI_TIF_TYPE);
OCI_Object   *v  = OCI_ObjectCreate(ti);

OCI_ObjectSetInt32 (v, OTEXT("CODE"), 42);
OCI_ObjectSetString(v, OTEXT("NAME"), OTEXT("ACME"));

OCI_StatementPrepare(st,
    OTEXT("insert into vendors(id, vendor) values(:i, :v)"));
OCI_StatementBindInt32 (st, OTEXT(":i"), &id);
OCI_StatementBindObject(st, OTEXT(":v"), v);
OCI_StatementExecutePrepared(st);

OCI_ObjectFree(v);
```

```cpp
Object v(TypeInfo(con, "VENDOR", TypeInfo::Type));

v.Set<Int32>("CODE", 42);
v.Set<String> ("NAME", "ACME");

st.Prepare("insert into vendors(id, vendor) values(:i, :v)");
st.Bind(":i", id);
st.Bind(":v", v);
st.ExecutePrepared();          /* v is RAII — no free needed */
```
:::

Fetching a `VENDOR`-typed column gives a **borrowed** `Object` — read it, never
free it:

:::sample
```c
OCI_Resultset *rs = OCI_StatementGetResultset(st);
while (OCI_ResultsetFetchNext(rs))
{
    OCI_Object *v = OCI_ResultsetGetObject(rs, 1);   /* borrowed! */
    printf("%d %s\n",
           OCI_ObjectGetInt32 (v, OTEXT("CODE")),
           OCI_ObjectGetString(v, OTEXT("NAME")));
}
```

```cpp
Resultset rs = st.GetResultset();
while (rs.Next())
{
    Object v = rs.Get<Object>(1);                    /* borrowed! */
    std::cout << v.Get<Int32>("CODE") << ' '
              << v.Get<String>("NAME")  << '\n';
}
```
:::

## Beyond plain objects

UDTs compose — an attribute can be another UDT, a `VARRAY`/nested table, a
`REF`, or any scalar. That whole family shares the object-relational API:

- **[Objects & attributes](objects-basics.html)** — the full `Object` API
- **[Collections](collections-basics.html)** — `OCI_Collection`/`Collection` for
  `VARRAY` and nested-table attributes
- **[REFs](refs.html)** — `OCI_Reference`/`Reference` pointers between objects
- **[Describe](describe-schema.html)** — `OCI_TypeInfo`/`TypeInfo` introspection

## Putting it together — the typical sequence

UDT work always starts with the dictionary, not the data:

1. **Describe the type** with `TypeInfoGet(con, "MY_TYPE", OCI_TIF_TYPE)` —
   fully qualified with the schema when it isn't yours. This one call walks
   the data dictionary and gives you the attribute layout everything else
   depends on.
2. **Create the object** from that `TypeInfo` — `ObjectCreate` builds an
   `OCI_Object` you own.
3. **Fill the attributes** by name — `Set`/`Set<Type>` — then bind it into an
   INSERT or PL/SQL call with `BindObject`.
4. **Fetch it back**: a UDT column arrives as a borrowed `Object` — read the
   attributes with `Get`/`Get<Type>`; never free the handle.
5. **Go deeper**: `ObjectGetColl` pulls a nested collection attribute,
   `ObjectGetSelfReference` gives a `REF` you can bind elsewhere — both come
   from the same describe-first machinery.

Hints: `OCI_TIF_TYPE` describes a *type*, `OCI_TIF_TABLE` a *table* — mixing
them is a common source of "type not found"; describe results are cached per
connection, so repeating `TypeInfoGet` is cheap; and a `NULL` object attribute
needs `ObjectSetNull`, not a zeroed `Set`.

## Common pitfalls

- **Freeing a fetched `Object`** — resultset objects are borrowed; only free
  objects you created with `OCI_ObjectCreate`.
- **`OCI_TypeInfoGet` on a table** — pass `OCI_TIF_TYPE` for UDTs; tables/views
  use `OCI_TIF_TABLE`/`OCI_TIF_VIEW` (see [describe](describe-schema.html)).
- **`Get<int>` on a `VARCHAR2` attr** — match the getter to the attribute type;
  check `OCI_ColumnGetType` on the type's `OCI_Column` first.

## Recap

- UDT = `CREATE TYPE … AS OBJECT` → `OCI_Object`/`Object` handle
- `OCI_TypeInfoGet(con, name, OCI_TIF_TYPE)` → `OCI_ObjectCreate` →
  `Get/Set` attrs → `OCI_StatementBindObject`
- fetched objects are borrowed; created ones you free
- collections/REFs/describe → Part 6

**Next:** checkpoint — put every type from Part 2 to work.
