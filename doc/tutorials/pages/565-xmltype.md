---
title: SYS.XMLTYPE — OCI_XmlType
navtitle: XMLType
part: 5
level: intermediate
---

# `SYS.XMLTYPE` — the `XmlType` object

**Objectives** — by the end of this page you will be able to:

- fetch `SYS.XMLTYPE` column values
- extract their content as text
- understand the (small) surface and workarounds for writing XML

## Meet the XmlType object

`SYS.XMLTYPE` is Oracle's legacy XML storage type — still present in lots of schemas.

An `XmlType` object wraps one fetched value — you never create it; it arrives **borrowed** from a fetched row, and gives you the XML as text.

The surface is deliberately small:

| C function | C++ method | Purpose |
|------------|------------|---------|
| `OCI_GetXmlType` | `Get<XmlType>(i)` | fetch → borrowed `XmlType` |
| `OCI_GetXmlTypeByName` | `Get<XmlType>("col")` | fetch by name |
| `OCI_XmlTypeGetContent` | `GetContent()` | content as text |
| `OCI_XmlTypeGetContentSize` | `GetContentSize()` | content length |

:::sample
```c
OCI_ExecuteStmt(st, OTEXT("select xmldoc from orders where id = :i"));
OCI_Resultset *rs = OCI_GetResultset(st);
if (OCI_FetchNext(rs))
{
    OCI_XmlType *x = OCI_GetXmlType(rs, 1);          /* borrowed */
    printf("%u bytes\n", OCI_XmlTypeGetContentSize(x));
    printf("%s\n",     OCI_XmlTypeGetContent(x));     /* otext* XML  */
}
```

```cpp
st.Execute("select xmldoc from orders where id = :i");
if (st.GetResultset().Next())
{
    XmlType x = st.GetResultset().Get<XmlType>(1);   /* borrowed */
    std::cout << x.GetContent() << "\n";
}
```
:::

## Writing XML back

There's no dedicated XML bind — write via `xmltype()` server-side parsing plus a
normal string/LOB bind:

```sql
insert into orders(id, xmldoc) values(:i, xmltype(:x))
```

!!! tip "New code? Prefer JSON"
    `SYS.XMLTYPE` is in maintenance across Oracle; for new designs use `JSON` columns
    (next pages) unless the schema is already XML.

## Try it yourself

1. `select xmltype('<a><b>1</b></a>') from dual` and print `GetContent`.
2. Fetch `sys.xmlgen.getxml('select 1 from dual')` results.
3. Insert an XML doc via `xmltype(:x)` string bind and read it back.

## Recap

- Fetch gives a borrowed `XmlType` → content + size; writing goes through
  `xmltype()` + string/lob binds.

**Next:** the modern JSON column type — fetch & bind `OCI_Json`.
