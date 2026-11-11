---
title: SODA — advanced operations
navtitle: SODA advanced
part: 9
level: advanced
oracle: "18"
---

# SODA — advanced operations

**Objectives** — by the end of this page you will be able to:

- bulk insert and use the `refresh` flag for server-assigned attributes
- replace/remove documents by key
- manage indexes and read the data guide
- drive finds with `OCI_SodaOptions`

## Bulk & the refresh flag

Insert/save/replace calls take a trailing **refresh** flag — when TRUE, the
server-assigned attributes (key, version, created-on) are pulled back into *your*
document in the same call:

:::sample
```c
OCI_SodaDocument *docs[3] = { d1, d2, d3 };
OCI_SodaCollectionInsertMany(coll, docs, 3, TRUE);     /* docs now carry keys+versions */
```

```cpp
std::vector<SodaDocument> docs{ d1, d2, d3 };
coll.InsertMany(docs, true);                     /* docs now carry keys+versions */
```
:::

Save upserts; replace-by-key targets an existing doc. Finds use the multi-key fast
path when options carry a keys list on capable clients.

## Finds with options

:::sample
```c
OCI_SodaOptions *opt = OCI_SodaOptionsCreate(sdb);
OCI_SodaOptionsSetFilter(opt, OTEXT("{\"age\":{\"$gt\":30}}"));
OCI_SodaOptionsSetLimit (opt, 10);
OCI_SodaOptionsSetSkip  (opt, 20);
OCI_SodaOptionsSetHint  (opt, OTEXT("MONITOR"));
OCI_SodaOptionsSetLock  (opt, TRUE);            /* SELECT FOR UPDATE semantics */
OCI_SodaDocumentCursor *cur = OCI_SodaCollectionFind(coll, opt);
```

```cpp
SodaOptions opt(sdb);
opt.SetFilter("{\"age\":{\"$gt\":30}}");
opt.SetLimit(10);
opt.SetSkip(20);
opt.SetHint("MONITOR");
opt.SetLock(true);                              /* SELECT FOR UPDATE semantics */
for (auto &&doc : coll.Find(opt))
{
    std::cout << doc.GetText() << "\n";
}
```
:::

`SetKey`/`SetKeys` for key lookups, `SetVersion` for optimistic concurrency,
`SetFetchArraySize` for cursor batching — same names on the C++ `SodaOptions`.

## Indexes & data guide

:::sample
```c
OCI_SodaCollectionCreateIndex(coll, OTEXT("{\"name\":\"ix_age\",\"fields\":[{\"path\":\"age\"}]}"));
otext **names;
OCI_UInt32 n;
names = OCI_SodaCollectionListIndexes(coll, &n);       /* + OCI_SodaCollectionFreeIndexNames */
OCI_SodaCollectionDropIndex(coll, OTEXT("ix_age"));
OCI_SodaDocument *guide = OCI_SodaCollectionGetDataGuide(coll);  /* inferred schema */
```

```cpp
coll.CreateIndex("{\"name\":\"ix_age\",\"fields\":[{\"path\":\"age\"}]}");
std::vector<String> names = coll.ListIndexes();
coll.DropIndex("ix_age");
Json guide = coll.GetDataGuide();                 /* inferred schema */
```
:::

!!! warning "Wide build caveat"
    `SodaCollCreateIndex`/`DropIndex`/`ListIndexes` and `OCI_SRM_SECURE_NOTIFICATION`
    fail **inside the Oracle client** under `OCI_CHARSET_WIDE` (verified on 23ai) —
    MBCS builds unaffected. See the v5 notes.

## Collection admin

`SodaCollTruncate` (empty it), `SodaCollDrop` (drop collection + backing table),
`SodaCollGetMetadata` (creation options), `SodaCollIsMapped` (mapped vs native),
`SodaDbGetCollectionNames`/`GetColls` (enumerate, + `FreeCollectionNames`).

## Putting it together — the typical sequence

Advanced SODA adds query-by-example, bulk ops, and indexes:

1. **QBE** — `SodaCollectionFind` with a JSON filter document, not SQL.
2. **Bulk insert** — `SodaCollectionInsertMany` for batches.
3. **Indexes** — `SodaCollectionCreateIndex`, `ListIndexes`, `DropIndex` on
   document fields.
4. **Data guide** — `SodaCollectionGetDataGuide` introspects the collection's
   document shapes.

Hints: QBE filters are documents — `{"field": {"$eq": value}}` syntax, not
WHERE clauses; `GetDataGuide` returns a `SodaDocument` describing observed
field paths; and index management is DDL — it commits, unlike document ops.

## Try it yourself

1. `InsertMany` 100 docs with refresh; verify each doc's key without re-reading.
2. ReplaceOneByKey a doc and check `GetVersion` changed.
3. Create an index on `age`, run a filtered find, then `DropIndex`.

## Recap

- `InsertMany`/`refresh`, `ReplaceOneByKey`, `SodaOptions` (filter/limit/skip/lock/
  hint/keys/version), index mgmt, data guide — full SODA coverage.

**Next:** the capstone — pulling the whole API together.
