---
title: Column metadata — OCI_Column attributes
navtitle: Column metadata
part: 2
level: intermediate
---

# Column metadata — the column descriptor

**Objectives** — by the end of this page you will be able to:

- retrieve column descriptors from a resultset
- read every column attribute: name, type, size, precision, nullability…
- build a generic "describe resultset" routine

## Meet the column descriptor

A **column** is a read-only descriptor of one result-set column — everything `describe`-style metadata knows about it: name, type, size, precision, nullability.

You don't create columns: ask the resultset for them, they're **borrowed** (owned by the resultset, never freed) and they're what generic tools — report writers, serializers, ORM-ish mappers — are built on.

:::sample
```c
OCI_UInt32 n = OCI_GetColumnCount(rs);
for (OCI_UInt32 i = 0; i < n; ++i)
{
    OCI_Column *col = OCI_GetColumn(rs, i);             /* or OCI_GetColumnByName */
    /* … read attributes … */
}
```

```cpp
for (UInt32 i = 0; i < rs.GetColumnCount(); ++i)
{
    Column col = rs.GetColumn(i);          /* or rs.GetColumn("ENAME") */
    /* … read attributes … */
}
```
:::

## The attribute table

| C function | C++ method | Meaning |
|------------|------------|---------|
| `OCI_ColumnGetName` | `GetName()` | column name (as returned, usually UPPER) |
| `OCI_ColumnGetType` | `GetType()` | OCILIB type class (`OCI_CDT_*`: numeric, text, lob, object…) |
| `OCI_ColumnGetSQLType` | `GetSQLType()` | SQL type name (`"NUMBER"`, `"TIMESTAMP"`…) |
| `OCI_ColumnGetFullSQLType` | `GetFullSQLType()` | SQL type with precision (`"TIMESTAMP(6) WITH TIME ZONE"`) |
| `OCI_ColumnGetSubType` | `GetSubType()` | subtype detail (e.g. CLOB vs BLOB) |
| `OCI_ColumnGetSize` | `GetSize()` | declared size (chars/bytes) |
| `OCI_ColumnGetPrecision` | `GetPrecision()` | NUMBER precision |
| `OCI_ColumnGetScale` | `GetScale()` | NUMBER scale |
| `OCI_ColumnGetLeadingPrecision` | `GetLeadingPrecision()` | INTERVAL leading precision |
| `OCI_ColumnGetFractionalPrecision` | `GetFractionalPrecision()` | TIMESTAMP fractional precision |
| `OCI_ColumnGetNullable` | `GetNullable()` | can it be NULL |
| `OCI_ColumnGetCharUsed` | `GetCharUsed()` | char vs byte length semantics |
| `OCI_ColumnGetCharsetForm` | `GetCharsetForm()` | NLS charset form |
| `OCI_ColumnGetCollationID` | `GetCollationID()` | collation id |
| `OCI_ColumnGetPropertyFlags` | `GetPropertyFlags()` | PK / identity / hidden-column flags |
| `OCI_ColumnGetTypeInfo` | `GetTypeInfo()` | type descriptor for object/collection columns (→ Part 6) |
| `OCI_ColumnHasJsonSchema` | `HasJsonSchema()` | JSON-schema constrained column (23ai) |
| `OCI_ColumnGetDimension` | `GetDimension()` | VECTOR dimension count |

## A generic column dump

:::sample
```c
static void describe_resultset(OCI_Resultset *rs)
{
    for (OCI_UInt32 i = 0; i < OCI_GetColumnCount(rs); ++i)
    {
        OCI_Column *col = OCI_GetColumn(rs, i);
        printf("#%-2u %-20s %-15s size=%-4u prec=%u scale=%u %s\n",
               i,
               OCI_ColumnGetName(col),
               OCI_ColumnGetSQLType(col),
               OCI_ColumnGetSize(col),
               OCI_ColumnGetPrecision(col),
               OCI_ColumnGetScale(col),
               OCI_ColumnGetNullable(col) ? "NULL" : "NOT NULL");
    }
}
```

```cpp
static void describe_resultset(Resultset &rs)
{
    for (UInt32 i = 0; i < rs.GetColumnCount(); ++i)
    {
        Column col = rs.GetColumn(i);
        std::cout << "#" << i << " " << col.GetName() << " " << col.GetSQLType()
                  << " size=" << col.GetSize() << " prec=" << col.GetPrecision()
                  << " scale=" << col.GetScale()
                  << (col.GetNullable() ? " NULL" : " NOT NULL") << "\n";
    }
}
```
:::

C++ equivalent: `rs.GetColumn(i)` → `Column` object with the same getters
(`col.GetName()`, `col.GetSQLType()`, `col.GetNullable()`, `col.HasJsonSchema()`…).

## Try it yourself

1. Run the dump against `select * from employees` — match each row to `describe` output.
2. Describe a query with expressions (`select sal*1.1 raise …`) and see what
   `GetSQLType` reports.
3. Print `OCI_ColumnGetFullSQLType` on a `TIMESTAMP WITH TIME ZONE` column.

## Common pitfalls

- **Freeing the `OCI_Column*`** — borrowed; owned by the resultset.
- **Byte vs char size confusion** — `GetSize` + `GetCharUsed` together tell the story.
- **Expecting property flags on every driver version** — some attributes need recent
  Oracle clients; check `OCI_ColumnGetPropertyFlags` defensively.

## Recap

- `OCI_GetColumn(rs, i)` → full per-column metadata for generic tooling.
- `OCI_CDT_*` type class + `GetSQLType` name + size/precision/nullable cover a describe.

**Next:** feeding values *in* — bind variables, part 1.
