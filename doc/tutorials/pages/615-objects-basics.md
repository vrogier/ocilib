---
title: Objects — create & fill
navtitle: Objects basics
part: 6
level: intermediate
demos: [object.c, object.cpp]
sql: object.sql
---

# Objects — create & fill

**Objectives** — by the end of this page you will be able to:

- instantiate Oracle object types with the Object datatype
- set scalar attributes by name
- bind objects into columns and fetch them back

## Meet the Object

An `Object` is one **instance of an Oracle object type** — the client-side twin of a `t_sale` or `t_vendor`.

It's created *from a `TypeInfo`* (the type descriptor, page 610), carries one value per named attribute — scalars, nested objects, collections, REFs — and moves through SQL by bind and fetch like any datatype.

Objects you create are yours to free; objects fetched from rows are borrowed.

## Setup

```sql
create type t_vendor as object (code number, name varchar2(30));
create type t_sale   as object (code number, price number, name varchar2(30),
                                ref varchar2(20), vendor t_vendor, date_sale date);
create table sales of t_sale;
```

## Create + fill + insert

:::sample
```c
OCI_TypeInfo *tiv = OCI_TypeInfoGet(cn, OTEXT("t_vendor"), OCI_TIF_TYPE);
OCI_Object *vendor = OCI_ObjectCreate(tiv);
OCI_TypeInfoFree(tiv);

OCI_ObjectSetInt   (vendor, OTEXT("CODE"), 134);
OCI_ObjectSetString(vendor, OTEXT("NAME"), OTEXT("JOHN SMITH"));

OCI_Object *sale = OCI_ObjectCreate(OCI_TypeInfoGet(cn, OTEXT("t_sale"), OCI_TIF_TYPE));
OCI_ObjectSetInt   (sale, OTEXT("CODE"),   1);
OCI_ObjectSetDouble(sale, OTEXT("PRICE"),  12.99);
OCI_ObjectSetObject(sale, OTEXT("VENDOR"), vendor);      /* nested object */
OCI_ObjectSetDate  (sale, OTEXT("DATE_SALE"), date);

OCI_Prepare(st, OTEXT("insert into sales values(:s)"));
OCI_BindObject(st, OTEXT(":s"), sale);
OCI_Execute(st);

OCI_ObjectFree(sale);
OCI_ObjectFree(vendor);
```

```cpp
TypeInfo tiv(con, "t_vendor", TypeInfo::Type);
Object vendor(tiv);
vendor.Set<int>("CODE", 134);
vendor.Set<String>("NAME", "JOHN SMITH");

Object sale(TypeInfo(con, "t_sale", TypeInfo::Type));
sale.Set<int>("CODE", 1);
sale.Set<double>("PRICE", 12.99);
sale.Set<Object>("VENDOR", vendor);
st.Bind(":s", sale, BindInfo::In);
```
:::

## Fetching objects

An object-valued column fetches back a borrowed `Object` — attribute getters read
the parts:

:::sample
```c
OCI_ExecuteStmt(st, OTEXT("select value(s) from sales s"));
OCI_Resultset *rs = OCI_GetResultset(st);
while (OCI_FetchNext(rs))
{
    OCI_Object *s = OCI_GetObject(rs, 1);          /* borrowed */
    printf("%d %s %.2f\n",
           OCI_ObjectGetInt(s, OTEXT("CODE")),
           OCI_ObjectGetString(s, OTEXT("NAME")),
           OCI_ObjectGetDouble(s, OTEXT("PRICE")));
}
```

```cpp
st.Execute("select value(s) from sales s");
for (auto &&row : st.GetResultset().Rows())
{
    Object s = row.Get<Object>(1);                 /* borrowed */
    std::cout << s.Get<int>("CODE") << " "
              << s.Get<String>("NAME") << " "
              << s.Get<double>("PRICE") << "\n";
}
```
:::

## Attribute names & NULL

- Names are the attribute names as declared — usually upper case in Oracle.
- Each attribute carries its own null flag (`SetNull`/`IsNull` per attribute).
- The object can recover its describing `TypeInfo` and its kind.

## Try it yourself

1. Create the types above, insert two sales, fetch and print both.
2. Set `VENDOR` NULL on one sale and read `ObjectIsNull` on fetch.
3. Fetch `ref(s)` from the table — a REF column (bridges to page 640).

## Common pitfalls

- **Attribute name case** — Oracle upper-cases unquoted names; `"code"` won't find
  `CODE`.
- **Freeing a fetched object** — fetched objects are borrowed, not yours.
- **Wrong attr type getter** — `Get<int>` on a `varchar2` attr fails; check the
  TypeInfo's column types.

## Recap

- TypeInfo → create object → set attributes → bind; fetch gives a borrowed object
  and named getters read it back.

**Next:** the complete attribute accessor matrix.
