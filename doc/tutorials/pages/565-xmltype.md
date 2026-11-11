---
title: SYS.XMLTYPE — OCI_XmlType
navtitle: XMLType
part: 2
level: intermediate
---

# `SYS.XMLTYPE` — the `XmlType` object

**Objectives** — by the end of this page you will be able to:

- fetch `SYS.XMLTYPE` column values by index and by name
- extract content as text and measure it
- write XML back via `xmltype()` + string/LOB binds
- decide when to keep XMLType vs move to JSON

## In context — the pre-JSON XML era

`SYS.XMLTYPE` predates native JSON support by a decade: it is how schemas from
the 2000s store XML — either as a structured object validated against an XML
schema, or as an opaque CLOB carrying XML text. You will meet it constantly in
legacy and enterprise schemas even though Oracle now steers new designs to
`JSON` columns.

OCILIB keeps the client surface deliberately small because the heavy lifting
happens in SQL: the server parses (`XMLTYPE()`), validates, queries (`XQUERY`,
`XMLTABLE`), and shreds XML — the client only fetches the result. A fetched
`OCI_XmlType` is therefore a read-only, *borrowed* view of one row's document:
you get its text and its size, and anything smarter (extracting nodes,
filtering elements) is best expressed as SQL rather than client-side parsing.

Writing follows the same philosophy: there is no XML bind type in OCILIB — you
hand the server a string or a LOB and let `xmltype()` do the parse, which also
gives you server-side schema validation for free. If a column returns a value
too large for a VARCHAR2 bind, stage it in a temporary `OCI_CLOB` and bind the
locator instead — the pattern is the LOB page's, reused verbatim.

## The XmlType object

`SYS.XMLTYPE` is Oracle's legacy XML storage type — still present in lots of
schemas that predate `JSON` columns.

An `XmlType` object wraps one fetched value — you never create it; it arrives
**borrowed** from a fetched row, and gives you the XML as text.

The surface is deliberately small:

| C function | C++ method | Purpose |
|------------|------------|---------|
| `OCI_ResultsetGetXmlType` | `Get<XmlType>(i)` | fetch → borrowed `XmlType` |
| `OCI_ResultsetGetXmlTypeByName` | `Get<XmlType>("col")` | fetch by name |
| `OCI_XmlTypeGetContent` | `GetContent()` | content as text |
| `OCI_XmlTypeGetContentSize` | `GetContentSize()` | content length |

## Scenario — reading an XML column

:::sample
```c
OCI_StatementExecute(st, OTEXT("select id, xmldoc from orders where dept = :d"));
OCI_StatementBindInt32(st, OTEXT(":d"), &deptno);

OCI_Resultset *rs = OCI_StatementGetResultset(st);
while (OCI_ResultsetFetchNext(rs))
{
    OCI_XmlType *x = OCI_ResultsetGetXmlType(rs, 1);          /* borrowed */

    printf("order %d: %u bytes of XML\n",
           OCI_ResultsetGetInt32(rs, 0),
           OCI_XmlTypeGetContentSize(x));
    printf("%.*s\n", 200, OCI_XmlTypeGetContent(x));    /* otext* XML */
}
```

```cpp
st.Execute("select id, xmldoc from orders where dept = :d");
st.Bind(":d", deptno);

Resultset rs = st.GetResultset();
while (rs.Next())
{
    XmlType x = rs.Get<XmlType>(1);                    /* borrowed */

    std::cout << "order " << rs.Get<Int32>(0)
              << ": " << x.GetContentSize() << " bytes\n"
              << x.GetContent() << "\n";
}
```
:::

## Scenario — fetch by name & NULL handling

An `XmlType` column can be NULL — check before you read:

:::sample
```c
OCI_ResultsetFetchNext(rs);

if (!OCI_ResultsetIsColumnNullByName(rs, OTEXT("xmldoc")))
{
    OCI_XmlType *x = OCI_ResultsetGetXmlTypeByName(rs, OTEXT("xmldoc"));
    /* … */
}
```

```cpp
rs.Next();

if (!rs.IsColumnNull("xmldoc"))
{
    XmlType x = rs.Get<XmlType>("xmldoc");
    /* … */
}
```
:::

## Scenario — writing XML back

There's no dedicated XML bind — write via `xmltype()` server-side parsing plus a
normal string bind (works for documents up to the VARCHAR2 limit; use a LOB bind
for bigger docs):

:::sample
```c
const otext *xml = OTEXT("<order id=\"42\"><item>USB-C cable</item></order>");

OCI_StatementPrepare(st,
    OTEXT("insert into orders(id, xmldoc) values(:i, xmltype(:x))"));
OCI_StatementBindInt32 (st, OTEXT(":i"), &order_id);
OCI_StatementBindString(st, OTEXT(":x"), (otext*) xml, (OCI_UInt32) 0);
OCI_StatementExecutePrepared(st);
```

```cpp
String xml = "<order id=\"42\"><item>USB-C cable</item></order>";

st.Prepare("insert into orders(id, xmldoc) values(:i, xmltype(:x))");
st.Bind(":i", order_id);
st.Bind(":x", xml);
st.ExecutePrepared();
```
:::

For docs larger than ~32 KB, bind an `OCI_Lob` instead — write the XML into a
temporary `OCI_CLOB` (page 550) and use the same `xmltype(:x)` call:

:::sample
```c
OCI_Lob *c = OCI_LobCreate(cn, OCI_CLOB);
OCI_UInt32 nch = xml_len, nby = xml_len;
OCI_LobWrite(c, big_xml, &nch, &nby);

OCI_StatementPrepare(st,
    OTEXT("update orders set xmldoc = xmltype(:x) where id = :i"));
OCI_StatementBindLob  (st, OTEXT(":x"), c);
OCI_StatementBindInt32(st, OTEXT(":i"), &order_id);
OCI_StatementExecutePrepared(st);
OCI_LobFree(c);
```

```cpp
Lob c(con, Lob::Clob);
c.Write(big_xml, xml_len);

st.Prepare("update orders set xmldoc = xmltype(:x) where id = :i");
st.Bind(":x", c);
st.Bind(":i", order_id);
st.ExecutePrepared();
```
:::

## Scenario — shredding to relational rows

Sometimes you don't want the XML at all — shred it server-side and fetch plain
columns with `XMLTABLE`:

:::sample
```c
OCI_StatementExecute(st, OTEXT(
    "select x.id, x.item"
    "  from orders o,"
    "       xmltable('/order/item' passing o.xmldoc"
    "                columns id   number      path '@id',"
    "                        item varchar2(40) path '.') x"));
```

```cpp
st.Execute(
    "select x.id, x.item"
    "  from orders o,"
    "       xmltable('/order/item' passing o.xmldoc"
    "                columns id   number      path '@id',"
    "                        item varchar2(40) path '.') x");
```
:::

The resultset is then plain `GetInt32`/`GetString` — no `XmlType` needed at all.

!!! tip "New code? Prefer JSON"
    `SYS.XMLTYPE` is in maintenance across Oracle; for new designs use `JSON`
    columns (next pages) unless the schema is already XML.

## Putting it together — the typical sequence

XmlType work splits cleanly into reading, writing, and shredding:

1. **Read**: fetch → borrowed `XmlType` → `GetContentSize` → `GetContent`.
   The returned text belongs to the row — copy it if you need it past the next
   `FetchNext`.
2. **Write**: there is no XmlType bind — express the XML in SQL as
   `xmltype(:x)` and bind a string (≤ ~32 KB) or a temporary `OCI_CLOB`
   (anything bigger). The server parses and validates for you.
3. **Shred when you want relational output**: `XMLTABLE` turns elements and
   attributes into plain columns — then it's just `GetInt32`/`GetString`, no
   `XmlType` at all.

Hints: always `IsColumnNull` before `Get<XmlType>` — NULL XMLType columns
deref to nothing; if you find yourself parsing the XML text client-side,
that's usually a sign you should be using `XMLTABLE` or `XQUERY` in the SQL
instead; and for new schemas, `JSON` columns (next pages) do what XMLType was
built for, with a much better client API.

## Try it yourself

1. `select xmltype('<a><b>1</b></a>') from dual` and print `GetContent`.
2. Fetch `sys.xmlgen.getxml('select 1 from dual')` results — nested XmlType.
3. Insert an XML doc via `xmltype(:x)` string bind and read it back by name.
4. Shred `orders.xmldoc` with `xmltable` into `id`/`item` columns.

## Common pitfalls

- **Expecting an Xml bind** — there is none; `xmltype()` + string/LOB is the way.
- **Deref'ing a NULL column** — check `IsColumnNull` before `Get<XmlType>`.
- **Treating `GetContent` as owned memory** — the text belongs to the row;
  copy it if you keep it past the next fetch.

## Recap

- Fetch gives a borrowed `XmlType` → `GetContent` + `GetContentSize`; writing
  goes through `xmltype()` + string/lob binds; `xmltable` shreds to columns.

**Next:** the modern JSON column type — fetch & bind `OCI_Json`.
