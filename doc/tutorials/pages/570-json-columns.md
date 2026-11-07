---
title: JSON columns — OCI_Json documents
navtitle: JSON columns
part: 5
level: intermediate
oracle: "21"
---

# JSON columns — the `Json` document object

**Objectives** — by the end of this page you will be able to:

- create `Json` documents from text and binary
- bind them into `JSON` columns and fetch them back
- control mutability, printing and validation

## Meet the Json object

Oracle's native `JSON` type (21c+) stores parsed binary JSON (OSON). A `Json` is the
**document handle** — created on the connection, loaded from text or bytes, bound
into columns, and the root of the DOM you explore with `JsonNode` (next page). It
has a mutability flag: fetched docs are read-only until you clone them into a
mutable one.

| C function | C++ method | Purpose |
|------------|------------|---------|
| `OCI_JsonCreate` | `Json` ctor (`con`) | document lifecycle |
| `OCI_JsonFree` | — (RAII) | release |
| `OCI_JsonLoadText` | `LoadText` | build from JSON text |
| `OCI_JsonLoadBinary` | `LoadBinary` | build from OSON bytes |
| `OCI_JsonToText` | `ToText` | render out |
| `OCI_JsonToBinary` | `ToBinary` | serialize to OSON |
| `OCI_JsonClone` | `Clone` | deep copy |
| `OCI_JsonGetRoot` | `GetRoot()` | DOM entry point |
| `OCI_JsonSetRoot` | `SetRoot` | set DOM root |
| `OCI_JsonSetMutable` | `SetMutable` | writable vs read-only |
| `OCI_JsonIsMutable` | `IsMutable()` | query mutability |
| `OCI_JsonGetConnection` | `GetConnection()` | owning connection |
| `OCI_JsonValidate` | `Validate(schema, &errors)` | JSON-schema validation (23ai) |
| `OCI_BindJson` | `Bind(":j", doc)` | bind JSON column |
| `OCI_GetJson` | `Get<Json>(i)` | fetch JSON column |

Validation flags (`OCI_JTV_*`) control lenient parsing (`ALLOW_SINGLE_QUOTES`,
`ALLOW_TRAILING_COMMAS`, … `ALLOW_ALL`); encodings are `OCI_JTE_*` (`DIRECT` = UTF-8,
`UTF16`, `DETECT`…).

## Load → bind → fetch

:::sample
```c
OCI_Json *doc = OCI_JsonCreate(cn);
OCI_JsonSetMutable(doc, TRUE);
const otext *txt = OTEXT("{\"name\":\"SMITH\",\"skills\":[\"SQL\",\"C\"]}");
OCI_JsonLoadText(doc, txt, 0, OCI_JTV_ALLOW_NONE, OCI_JTE_UTF8);   /* 0 = NUL-terminated */

OCI_Prepare(st, OTEXT("insert into emp_profile(empno, profile) values(:i, :j)"));
OCI_BindJson(st, OTEXT(":j"), doc);
OCI_Execute(st);

OCI_ExecuteStmt(st, OTEXT("select profile from emp_profile where empno = 7369"));
OCI_Resultset *rs = OCI_GetResultset(st);
OCI_FetchNext(rs);
OCI_Json *got = OCI_GetJson(rs, 1);                /* borrowed */

otext buf[2048]; OCI_UInt32 n = sizeof(buf);
OCI_JsonToText(got, buf, &n, OCI_JTP_PRETTY);
```

```cpp
Json doc(con);
doc.SetMutable(true);
doc.LoadText("{\"name\":\"SMITH\",\"skills\":[\"SQL\",\"C\"]}");
st.Bind(":j", doc, BindInfo::In);
/* fetch: Json got = rs.Get<Json>(1); String s = got.ToText(Json::Pretty); */
```
:::

## Schema validation (23ai)

:::sample
```c
OCI_Json *schema = OCI_JsonCreate(cn);
OCI_JsonLoadText(schema, schema_txt, len, OCI_JTV_ALLOW_NONE, OCI_JTE_UTF8);
OCI_Json *errors = NULL;
if (!OCI_JsonValidate(doc, schema, &errors) && errors)
{
    /* errors is a JSON doc describing violations */
}
```

```cpp
Json schema(con);
schema.LoadText(schema_txt);                      /* strict + UTF-8 defaults  */
Json errors;
if (!doc.Validate(schema, errors))
{
    /* errors is a JSON doc describing violations */
}
```
:::

## Mutable vs immutable

Mark a document mutable to enable DOM editing (node setters, next page). Fetched
docs come immutable by default — clone into a mutable one to edit.

## Try it yourself

1. `create table emp_profile(empno number, profile json)`; insert + re-read a doc.
2. Load a doc with single quotes using `OCI_JTV_ALLOW_SINGLE_QUOTES`.
3. Validate a doc against a schema and dump `errors` with `ToText`.

## Common pitfalls

- **Mutating an immutable doc** — check `OCI_JsonIsMutable`; `SetMutable`/`Clone` first.
- **`VARCHAR2` columns, not `JSON`** — DOM works client-side, but bind/fetch path
  differs (string vs `OCI_Json`).
- **Under-sized `ToText` buffer** — `length` is in/out; grow and retry.

## Recap

- `OCI_Json` = document; `LoadText`/`LoadBinary`/`ToText`/`ToBinary`/`Clone`/
  `Validate`; `BindJson`/`GetJson` move it through SQL.

**Next:** `OCI_JsonNode` — the full DOM API.
