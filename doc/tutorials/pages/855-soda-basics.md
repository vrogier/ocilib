---
title: SODA — documents without SQL
navtitle: SODA basics
part: 9
level: intermediate
oracle: "18"
demos: [soda.c, soda.cpp]
---

# SODA — documents without SQL

**Objectives** — by the end of this page you will be able to:

- open `OCI_SodaDatabase`, create collections and documents
- insert, find and count documents
- read document metadata (key, version, timestamps)

## Concepts

**Simple Oracle Document Access** is Oracle's NoSQL-style API over `JSON` columns:
collections of documents, queried by example — no SQL needed.

OCILIB exposes the full native surface since v5.

The object chain is short: a `SodaDatabase` session on your
connection → `SodaCollection` collections → `SodaDocument` documents, with `SodaOptions` for
query modifiers:

:::sample
```c
OCI_SodaDatabase *sdb = OCI_SodaDatabaseCreate(cn);                    /* SODA session    */
OCI_SodaCollection *coll = OCI_SodaDatabaseCreateCollection(sdb,
                       OTEXT("soda_employees"), NULL);     /* metadata = NULL */

OCI_SodaDocument *doc = OCI_SodaDocumentCreateFromText(
    sdb, OTEXT("{\"name\":\"Fred\",\"age\":42}"), 0, NULL);/* content, len, key */
OCI_SodaCollectionInsertDocument(coll, doc);

printf("key=%s version=%s created=%s\n",
       OCI_SodaDocumentGetKey(doc), OCI_SodaDocumentGetVersion(doc),
       OCI_SodaDocumentGetCreatedOn(doc));
OCI_SodaDocumentFree(doc);
```

```cpp
SodaDatabase sdb(con);                                 /* SODA session    */
SodaCollection coll = sdb.CreateCollection("soda_employees");

SodaDocument doc(sdb, "{\"name\":\"Fred\",\"age\":42}");
coll.InsertDoc(doc);
std::cout << doc.GetKey() << " " << doc.GetVersion() << "\n";
```
:::

## Reading documents

A find yields a **cursor** — each pulled document is owned (free it in C):

:::sample
```c
OCI_SodaDocumentCursor *cur = OCI_SodaCollectionFind(coll, NULL);     /* all docs */
while ((doc = OCI_SodaDocumentCursorGetNext(cur)))
{
    printf("%s\n", OCI_SodaDocumentGetText(doc));
    OCI_SodaDocumentFree(doc);                                 /* each doc owned */
}
OCI_SodaDocumentCursorFree(cur);
```

```cpp
for (auto &&doc : coll.Find())              /* cursor → range-for, RAII   */
{
    std::cout << doc.GetText() << "\n";
}
```
:::

## The object map

| Object | C/C++ role |
|--------|------------|
| `SodaDatabase` | SODA session (`CreateColl`, `OpenColl`, `GetCollectionNames`, `GetColls`) |
| `SodaCollection` | a collection (`InsertDoc`, `InsertMany`, `Save`, `ReplaceOne{,ByKey}`, `Find`, `FindOne{,ByKey}`, `DocCount{,WithFilter}`, `Remove{,OneByKey}`, `Truncate`, `Drop`, `GetMetadata`, `GetDataGuide`, index ops) |
| `SodaDocument` | a document (`GetContent`/`GetText`/`GetJson`, `GetKey`, `GetVersion`, `GetCreatedOn`, `GetLastModified`, `GetMediaType`, `GetIsJson`) |
| `SodaOptions` | query modifiers (`SetFilter` QBE, `SetKey`/`SetKeys`, `SetLimit`, `SetSkip`, `SetLock`, `SetHint`, `SetFetchArraySize`, `SetVersion`) |
| `SodaDocumentCursor` / `SodaCollectionCursor` | doc/collection iterators — C++ range-for |

## Find one + count

:::sample
```c
OCI_SodaOptions *opt = OCI_SodaOptionsCreate(sdb);
OCI_SodaOptionsSetFilter(opt, OTEXT("{\"name\":\"Jane\"}"));     /* QBE filter */
OCI_SodaDocument *jane = OCI_SodaCollectionFindOne(coll, opt);
OCI_UInt64 n = OCI_SodaCollectionDocumentCount(coll, opt);
OCI_SodaOptionsFree(opt);
```

```cpp
SodaOptions opt(sdb);
opt.SetFilter("{\"name\":\"Jane\"}");            /* QBE filter */
SodaDocument jane = coll.FindOne(opt);
ULong64 n = coll.DocCount(opt);
```
:::

## Putting it together — the typical sequence

SODA is the document-store view of the same database:

1. **Get the SODA database** — `OCI_SodaDatabase` from a connection.
2. **Get or create a collection** — `SodaCollection` is a JSON-document bucket.
3. **Insert documents** — `SodaDocument` wraps each JSON doc; `Insert` puts
   them in.
4. **Find them** — `Find` with a filter or `FindKeys` for a cursor over keys.
5. **Fetch documents** — the cursor's `Next` yields `SodaDocument`s.

Hints: SODA collections are just tables underneath — `select * from
"soda_coll"` still works; documents are JSON — the same OSON storage as the
JSON columns page; and `SodaDb` → `SodaDatabase` is the C++ name — the C API
was `OCI_SodaDatabase*` all along.

## Try it yourself

1. Insert 10 docs; `DocCount` them; `FindOne` by a QBE filter.
2. Insert with an explicit key arg and `FindOneByKey` it.
3. `GetCollectionNames`/`GetColls` to list what's in your schema.

## Common pitfalls

- **Documents are owned** — every `SodaDocCursorGetNext`/`FindOne` doc needs
  `SodaDocFree`.
- **Forgetting `OCI_ConnectionCommit`** — SODA writes are transactional.
- **`InsertDoc` vs `Save`** — insert fails on existing key; save upserts.

## Recap

- `SodaDatabase` → `Coll` → `Doc` + `Options` → `Find`/cursors — full CRUD without SQL.

**Next:** SODA advanced — bulk ops, indexes, data guide, the refresh flag.
