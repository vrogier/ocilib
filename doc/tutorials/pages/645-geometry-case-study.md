---
title: Case study — a nested object graph (geometry demo)
navtitle: Case study object graph
part: 6
level: advanced
demos: [geometry.c, geometry.cpp]
sql: geometry.sql
---

# Case study — a nested object graph (geometry demo)

**Objectives** — by the end of this page you will be able to:

- combine TypeInfo → Object → Coll → Elem → Ref on a real schema
- read a deeply nested object structure
- write one back as a single bind

## The schema (from `demo/geometry.sql`)

```sql
create type point_t as object (x number, y number);
create type points_t as varray(10) of point_t;
create type polygon_t as object (id number, name varchar2(30), points points_t);
create table shapes (id number, shape polygon_t);
```

Three layers: `shapes` row → `polygon_t` object → `points_t` varray → `point_t`
objects. Exactly the nesting the API was built for.

## Reading the graph

:::sample
```c
OCI_StatementExecute(st, OTEXT("select shape from shapes"));
OCI_Resultset *rs = OCI_StatementGetResultset(st);
while (OCI_ResultsetFetchNext(rs))
{
    OCI_Object *poly = OCI_ResultsetGetObject(rs, 0);
    printf("polygon %d (%s):\n",
           OCI_ObjectGetInt32(poly, OTEXT("ID")),
           OCI_ObjectGetString(poly, OTEXT("NAME")));

    OCI_Collection *pts = OCI_ObjectGetColl(poly, OTEXT("POINTS"));   /* owned */
    OCI_Iterator *it  = OCI_IteratorCreate(pts);
    OCI_Element *e;
    while ((e = OCI_IteratorGetNext(it)))
    {
        OCI_Object *pt = OCI_ElementGetObject(e);                  /* owned */
        printf("   (%d, %d)\n",
               OCI_ObjectGetInt32(pt, OTEXT("X")),
               OCI_ObjectGetInt32(pt, OTEXT("Y")));
        OCI_ObjectFree(pt);
        OCI_ElementFree(e);
    }
    OCI_IteratorFree(it);
    OCI_CollectionFree(pts);
    OCI_ObjectFree(poly);
}
```

```cpp
st.Execute("select shape from shapes");
for (auto &&row : st.GetResultset().Rows())
{
    Object poly = row.Get<Object>(0);
    std::cout << "polygon " << poly.Get<int>("ID") << " ("
              << poly.Get<String>("NAME") << "):\n";

    Coll pts = poly.Get<Coll>("POINTS");            /* owned — RAII */
    for (auto &&e : pts)                            /* CollectionIterator  */
    {
        Object pt = e.Get<Object>();
        std::cout << "   (" << pt.Get<int>("X") << ", "
                  << pt.Get<int>("Y") << ")\n";
    }
}
```
:::

## Writing the graph

:::sample
```c
OCI_TypeInfo *ti = OCI_TypeInfoGet(cn, OTEXT("polygon_t"), OCI_TIF_TYPE);
OCI_Object *poly = OCI_ObjectCreate(ti);
OCI_ObjectSetInt32(poly, OTEXT("ID"), 1);
OCI_ObjectSetString(poly, OTEXT("NAME"), OTEXT("triangle"));

OCI_TypeInfo *cti = OCI_TypeInfoGet(cn, OTEXT("points_t"), OCI_TIF_TYPE);
OCI_Collection *pts = OCI_CollectionCreate(cti);
/* append point_t elems built via OCI_ElementCreate + ElemGetObject + ObjectSetInt */
OCI_ObjectSetColl(poly, OTEXT("POINTS"), pts);

OCI_StatementPrepare(st, OTEXT("insert into shapes values(:i, :p)"));
OCI_StatementBindObject(st, OTEXT(":p"), poly);
OCI_StatementExecutePrepared(st);
```

```cpp
TypeInfo ti(con, "polygon_t", TypeInfo::Type);
Object poly(ti);
poly.Set<int>   ("ID", 1);
poly.Set<String>("NAME", "triangle");

TypeInfo cti(con, "points_t", TypeInfo::Type);
Coll pts(cti);
/* append point_t elems built via Elem + e.Get<Object>() + pt.Set<int> */
poly.Set<Coll>("POINTS", pts);

st.Prepare("insert into shapes values(:i, :p)");
st.Bind(":p", poly);
st.ExecutePrepared();
```
:::

## The takeaway pattern

```text
TypeInfo (describe) → ObjectCreate / CollCreate / ElemCreate
   → ObjectSet* / ElemSet* / CollAppend / ObjectSetColl
   → one BindObject → one Execute
```

And on the way back: `GetObject` → `ObjectGetColl` → `IterGetNext` →
`ElemGetObject` → `ObjectGet*` — owned handles freed as you go.

## Putting it together — the typical sequence

The geometry case study shows the object API earning its keep:

1. **Describe `MDSYS.SDO_GEOMETRY`** — `TypeInfoGet` on the spatial type.
2. **Build the object** — `ObjectCreate` + `Set` attributes per field.
3. **Handle the nested types** — `SDO_POINT`, `SDO_ELEM_INFO_ARRAY` are
   objects/collections *inside* the outer object — `ObjectGetColl` reaches
   them.
4. **Bind into spatial queries** — `SDO_*` operators in SQL take the bound
   object directly.

Hints: `SDO_ELEM_INFO`/`SDO_ORDINATES` are VARRAYs of number — the collection
API fills them, not `Set` on a flat struct; the SDO type is nested 3 deep —
describe once, reuse the TypeInfos; and `ObjectGetStruct` fills a C struct
mirror of the object when you need the whole row at once.

## Try it yourself

1. Extend `polygon_t` with a `REF` to a `materials` object table and deref it.
2. Re-implement the reader with `OCI_ObjectToText` — compare effort vs the walk.

## Recap

- Nested object graphs are just the same four primitives composed — the demo schema
  proves the whole API in one page.

**Next:** Checkpoint 5 — model departments/employees as objects and collections.
