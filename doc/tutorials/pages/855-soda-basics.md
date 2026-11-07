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

- open `OCI_SodaDb`, create collections and documents
- insert, find and count documents
- read document metadata (key, version, timestamps)

## Concepts

**Simple Oracle Document Access** is Oracle's NoSQL-style API over `JSON` columns:
collections of documents, queried by example — no SQL needed.

OCILIB exposes the full native surface since v5.

The object chain is short: a `SodaDb` session on your
connection → `SodaColl` collections → `SodaDoc` documents, with `SodaOptions` for
query modifiers:

:::sample
```c
OCI_SodaDb *sdb = OCI_SodaDbCreate(cn);                    /* SODA session    */
OCI_SodaColl *coll = OCI_SodaDbCreateColl(sdb,
                       OTEXT("soda_employees"), NULL);     /* metadata = NULL */

OCI_SodaDoc *doc = OCI_SodaDocCreateFromText(
    sdb, OTEXT("{\"name\":\"Fred\",\"age\":42}"), 0, NULL);/* content, len, key */
OCI_SodaCollInsertDoc(coll, doc);

printf("key=%s version=%s created=%s\n",
       OCI_SodaDocGetKey(doc), OCI_SodaDocGetVersion(doc),
       OCI_SodaDocGetCreatedOn(doc));
OCI_SodaDocFree(doc);
```

```cpp
SodaDb sdb(con);                                 /* SODA session    */
SodaColl coll = sdb.CreateCollection("soda_employees");

SodaDoc doc(sdb, "{\"name\":\"Fred\",\"age\":42}");
coll.InsertDoc(doc);
std::cout << doc.GetKey() << " " << doc.GetVersion() << "\n";
```
:::

## Reading documents

A find yields a **cursor** — each pulled document is owned (free it in C):

:::sample
```c
OCI_SodaDocCursor *cur = OCI_SodaCollFind(coll, NULL);     /* all docs */
while ((doc = OCI_SodaDocCursorGetNext(cur)))
{
    printf("%s\n", OCI_SodaDocGetText(doc, NULL));
    OCI_SodaDocFree(doc);                                 /* each doc owned */
}
OCI_SodaDocCursorFree(cur);
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
| `SodaDb` | SODA session (`CreateColl`, `OpenColl`, `GetCollectionNames`, `GetColls`) |
| `SodaColl` | a collection (`InsertDoc`, `InsertMany`, `Save`, `ReplaceOne{,ByKey}`, `Find`, `FindOne{,ByKey}`, `DocCount{,WithFilter}`, `Remove{,OneByKey}`, `Truncate`, `Drop`, `GetMetadata`, `GetDataGuide`, index ops) |
| `SodaDoc` | a document (`GetContent`/`GetText`/`GetJson`, `GetKey`, `GetVersion`, `GetCreatedOn`, `GetLastModified`, `GetMediaType`, `GetIsJson`) |
| `SodaOptions` | query modifiers (`SetFilter` QBE, `SetKey`/`SetKeys`, `SetLimit`, `SetSkip`, `SetLock`, `SetHint`, `SetFetchArraySize`, `SetVersion`) |
| `SodaDocCursor` / `SodaCollCursor` | doc/collection iterators — C++ range-for |

## Find one + count

:::sample
```c
OCI_SodaOptions *opt = OCI_SodaOptionsCreate(sdb);
OCI_SodaOptionsSetFilter(opt, OTEXT("{\"name\":\"Jane\"}"));     /* QBE filter */
OCI_SodaDoc *jane = OCI_SodaCollFindOne(coll, opt);
OCI_ULong64 n = OCI_SodaCollDocCount(coll, opt);
OCI_SodaOptionsFree(opt);
```

```cpp
SodaOptions opt(sdb);
opt.SetFilter("{\"name\":\"Jane\"}");            /* QBE filter */
SodaDoc jane = coll.FindOne(opt);
ULong64 n = coll.DocCount(opt);
```
:::

## Try it yourself

1. Insert 10 docs; `DocCount` them; `FindOne` by a QBE filter.
2. Insert with an explicit key arg and `FindOneByKey` it.
3. `GetCollectionNames`/`GetColls` to list what's in your schema.

## Common pitfalls

- **Documents are owned** — every `SodaDocCursorGetNext`/`FindOne` doc needs
  `SodaDocFree`.
- **Forgetting `OCI_Commit`** — SODA writes are transactional.
- **`InsertDoc` vs `Save`** — insert fails on existing key; save upserts.

## Recap

- `SodaDb` → `Coll` → `Doc` + `Options` → `Find`/cursors — full CRUD without SQL.

**Next:** SODA advanced — bulk ops, indexes, data guide, the refresh flag.
