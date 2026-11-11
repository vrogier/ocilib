---
title: JSON columns — OCI_Json documents
navtitle: JSON columns
part: 2
level: intermediate
oracle: "21"
---

# JSON columns — the `Json` document object

**Objectives** — by the end of this page you will be able to:

- create `Json` documents from text and binary
- bind them into `JSON` columns and fetch them back
- control mutability, printing and validation

## In context — JSON is a real type now

Since 21c (and everywhere in 23ai), Oracle has a native `JSON` column type that
stores documents in **OSON**, a binary format designed for the database: faster
to parse, indexable, and queryable with the `JSON_VALUE`/`JSON_QUERY`/`json_table`
family and the `data.field.path` dot notation. Storing JSON in `VARCHAR2`/`CLOB`
still works, but it stays a dumb string — no binary access path, no schema
validation, weaker indexing.

`OCI_Json` is the client-side handle to one document, and it is where OCILIB's
ownership rules show up most visibly. A document can be **immutable**
(read-only, as fetched from a column) or **mutable** (editable through the DOM
API of the next page) — `OCI_JsonIsMutable`/`SetMutable` gate the DOM setters.
A fetched `Json` is *borrowed and immutable*: to edit it you clone it into an
owned, mutable document (`OCI_JsonClone` + `SetMutable`), modify the copy, and
bind it back. Loading goes through `LoadText` (with `OCI_JTV_*` parse-strictness
flags and `OCI_JTE_*` input encodings) or `LoadBinary` for OSON; output is
`ToText` (with `OCI_JTP_*` formatting flags) or `ToBinary`. The binary path
matters when you shuttle documents between services — it skips the
serialize-then-reparse round-trip entirely.

Finally, `OCI_JsonValidate` runs server-grade JSON Schema validation
client-side: pass your document and a schema document, get a boolean plus an
errors document describing every violation — the same engine Oracle applies to
`IS JSON` check constraints.

## The Json object

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
| `OCI_StatementBindJson` | `Bind(":j", doc)` | bind JSON column |
| `OCI_ResultsetGetJson` | `Get<Json>(i)` | fetch JSON column |

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

OCI_StatementPrepare(st, OTEXT("insert into emp_profile(empno, profile) values(:i, :j)"));
OCI_StatementBindJson(st, OTEXT(":j"), doc);
OCI_StatementExecutePrepared(st);

OCI_StatementExecute(st, OTEXT("select profile from emp_profile where empno = 7369"));
OCI_Resultset *rs = OCI_StatementGetResultset(st);
OCI_ResultsetFetchNext(rs);
OCI_Json *got = OCI_ResultsetGetJson(rs, 0);                /* borrowed */

otext buf[2048];
OCI_UInt32 n = sizeof(buf);
OCI_JsonToText(got, buf, &n, OCI_JTP_PRETTY);
```

```cpp
Json doc(con);
doc.SetMutable(true);
doc.LoadText("{\"name\":\"SMITH\",\"skills\":[\"SQL\",\"C\"]}");
st.Bind(":j", doc, BindInfo::In);
/* fetch: Json got = rs.Get<Json>(0); String s = got.ToText(Json::Pretty); */
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

## Scenario — binary (OSON) round-trip

Text is for humans; OSON is for machines. `ToBinary`/`LoadBinary` skip the
parse/serialize cost when shuttling docs between services:

:::sample
```c
unsigned char oson[8192];
OCI_UInt32    n = sizeof(oson);
OCI_JsonToBinary(doc, oson, &n);           /* doc → OSON bytes         */

OCI_Json *copy = OCI_JsonCreate(cn);
OCI_JsonLoadBinary(copy, oson, n);         /* bytes → doc (no parse)   */
```

```cpp
std::vector<unsigned char> oson(8192);
UInt32 n = (UInt32) oson.size();
doc.ToBinary(oson.data(), n);              /* doc → OSON bytes         */

Json copy(con);
copy.LoadBinary(oson.data(), n);           /* bytes → doc (no parse)   */
```
:::

## Scenario — cloning a fetched doc into an editable one

A fetched `Json` is borrowed *and* immutable. Clone → set mutable → edit:

:::sample
```c
OCI_Json *fetched = OCI_ResultsetGetJson(rs, 0);        /* borrowed, immutable */
OCI_Json *edit    = OCI_JsonCreate(cn);
OCI_JsonClone(edit, fetched);
OCI_JsonSetMutable(edit, TRUE);                          /* now DOM-editable    */
```

```cpp
Json fetched = rs.Get<Json>(0);                         /* borrowed, immutable */
Json edit(con);
edit.Clone(fetched);
edit.SetMutable(true);                                   /* now DOM-editable    */
```
:::

## Putting it together — the typical sequence

The JSON column workflow is: build → bind → fetch → read → (optionally)
validate:

1. **Create the document** on the connection; call `SetMutable` *now* if you
   plan to edit it through the DOM later — it's much easier than converting
   afterwards.
2. **Fill it**: `LoadText` with a parse-strictness flag (`OCI_JTV_*`) and
   input encoding (`OCI_JTE_*`), or `LoadBinary` for OSON you already have.
3. **Bind it** with `StatementBindJson` into the `JSON` column — or fetch an
   existing one, which arrives *borrowed and immutable*.
4. **Read it back**: `ToText` with a format flag (`OCI_JTP_PRETTY` for humans)
   or `ToBinary` for the compact OSON round-trip.
5. **Validate** against a JSON Schema document with `OCI_JsonValidate` — the
   errors document it fills tells you exactly what failed.

Hints: `ToText`/`ToBinary` use in/out length counters — grow the buffer and
retry when they return FALSE; to edit a fetched document, clone → `SetMutable`
→ modify → bind — the fetched handle itself stays immutable; and `VARCHAR2`
columns holding JSON text are *not* JSON columns — the `BindJson`/`GetJson`
path doesn't apply to them.

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
