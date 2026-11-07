---
title: Object ops — assign, compare, inheritance
navtitle: Object operations
part: 6
level: advanced
demos: [super_types.c, super_types.cpp]
sql: suber_types.sql
---

# Object ops — assign, compare, inheritance

**Objectives** — by the end of this page you will be able to:

- copy and compare objects
- work with type hierarchies (`under`, `not final`)
- use `GetSelfRef` and `ObjectToText` for identity and debugging

## Assign & compare

Assign deep-copies every attribute — nested objects and collections included:

:::sample
```c
OCI_Object *copy = OCI_ObjectCreate(OCI_ObjectGetTypeInfo(obj));
OCI_ObjectAssign(copy, obj);          /* deep copy — all attrs duplicated */
```

```cpp
Object copy = obj;                     /* copy ctor — deep                 */
```
:::

`ObjectGetStruct` copies attributes into a C struct (page 235 rules: member names match attribute names + indicator struct for NULLs).

Per-attribute `IsNull` checks are the compare primitive.

## Self references

A fetched object's **self-REF** is what you'd bind into a `REF` column elsewhere —
the link between object values and object identity (page 640):

:::sample
```c
OCI_Ref *self = OCI_ObjectGetSelfRef(obj);   /* a REF pointing at obj */
```

```cpp
Reference self = obj.GetSelfRef();
```
:::

## Inheritance — super types

```sql
create type t_person as object (name varchar2(30)) not final;
create type t_employee under t_person (empno number, salary number);
```

The chain is inspectable through the TypeInfo:

:::sample
```c
OCI_TypeInfo *ti  = OCI_TypeInfoGet(cn, OTEXT("t_employee"), OCI_TIF_TYPE);
OCI_TypeInfo *sup = OCI_TypeInfoGetSuperType(ti);            /* → t_person   */
OCI_Boolean final = OCI_TypeInfoIsFinalType(ti);             /* FALSE here   */
```

```cpp
TypeInfo sup = obj.GetTypeInfo().GetSuperType();             /* → t_person   */
bool final   = obj.GetTypeInfo().IsFinalType();
```
:::

Fetched subtype objects carry **all** attributes (own + inherited) — attribute
getters read `NAME` on a `t_employee` because it's inherited from `t_person`. The
demos `super_types.c`/`suber_types.sql` build a full hierarchy to play with.

## Debugging

`ToText` renders the whole nested structure — the single most useful call when
learning this API:

:::sample
```c
otext buf[4096];
OCI_ObjectToText(obj, sizeof(buf), buf);    /* "t_sale(1, 'x', t_vendor(134,…))" */
printf("%s\n", buf);
```

```cpp
std::cout << obj.ToText() << "\n";
```
:::

## Try it yourself

1. Assign one sale to another handle, change an attr on the copy, verify the
   original is untouched.
2. Build the `t_person`/`t_employee` hierarchy; describe `t_employee` and confirm
   `NAME` appears via `GetSuperType`.
3. `ObjectToText` a deeply nested object and compare with the describe tree.

## Common pitfalls

- **Assuming shallow assign** — `Assign` is deep; edits on copies don't alias.
- **`GetSelfRef` on un-fetched objects** — a created-not-yet-inserted object has no
  persistent REF.
- **Subtype attr not found** — inherited attrs are fetched through the *declared*
  column type; check `GetSuperType` when an attr looks missing.

## Recap

- `Assign` deep-copies; `GetSelfRef` yields the REF; `GetSuperType`/`IsFinalType`
  walk inheritance; `ToText` dumps everything.

**Next:** collections — VARRAYs and nested tables.
