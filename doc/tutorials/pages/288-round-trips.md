---
title: Server round-trips — which calls cross the wire
navtitle: Round-trips
part: 3
level: intermediate
---

# Server round-trips — which calls cross the wire

**Objectives** — by the end of this page you will be able to:

- tell which OCILIB calls hit the network and which are purely client-side
- pick the cheaper verb for the same job (`Prepare` vs `Parse`, prefetching, autocommit)

## The rule of thumb

Every OCILIB call is either **local** — it works on client-side handles, buffers
and cached metadata — or a **round-trip** — a request sent to the server and
answered. Oracle counts round-trips per OCI call in its *OCI Function Server
Round-Trips* appendix (OCI Programmer's Guide); the tables below map those
counts onto the OCILIB surface.

The mental model: **creating, configuring, binding and reading back are local;
asking the server to do something is a round-trip.** The few surprises are worth
memorizing — `Prepare` is local while `Parse` is not, and `FetchNext` only
crosses the wire when its buffers run dry.

## Environment, connections, sessions

| Call | Round-trips | Note |
|------|-------------|------|
| `OCI_EnvironmentCreate` | 0 | pure client-side init |
| `OCI_ConnectionCreate` | ~2 | server attach + session begin (authentication-dependent) |
| `OCI_PoolCreate` | pool-min × logins | sessions established upfront — then `OCI_PoolGetConnection` reuses them locally |
| `OCI_ConnectionPing` | 1 | that's its job |
| `OCI_ConnectionBreak` | 1 | interrupts a blocked call on the server side |
| `OCI_EnvironmentSetUserPassword` | 1 | |
| `OCI_DatabaseStartup`, `OCI_DatabaseShutdown` | 1 each | |

## Statements — the one surprise

| Call | Round-trips | Note |
|------|-------------|------|
| `OCI_StatementCreate` | 0 | local handle |
| `OCI_StatementPrepare` | **0** | registers the SQL client-side — the real parse is deferred to execute |
| `OCI_StatementParse` | **1** | asks the server to parse without executing |
| `OCI_StatementDescribe` | 1 | select-list description without executing |
| `OCI_StatementExecutePrepared`, `OCI_StatementExecute`, `OCI_ConnectionImmediate*` | 1 | parse + execute bundled into one trip |
| `OCI_Bind*`, `OCI_Register*`, `OCI_StatementSetFetchSize` and friends | 0 | all setters are local — bound data ships with the execute |

The asymmetry that catches people: `Prepare` is a *local* call — it stores the
SQL text and allocates the context without contacting the server, and the parse
rides along inside the first execute round-trip. `Parse` exists for the
opposite need: it *forces* a server round-trip to validate the SQL (or warm the
server's cursor cache) without running it.

## Resultsets and fetches

| Call | Round-trips | Note |
|------|-------------|------|
| `OCI_StatementGetResultset`, `OCI_StatementGetNextResultset` | 0 | resultsets materialized during execute |
| `OCI_FetchNext` | 0 while the row buffer lasts, 1 per refill | the refill serves `OCI_StatementSetFetchSize` rows plus prefetched ones — see [fetch tuning](fetch-tuning.html) |
| `OCI_ResultsetFetchPrev`/`First`/`Last`/`Seek` | 0 | scrollable cursors fetch the whole set client-side |
| all resultset getters | 0 | read client buffers |

## Transactions

| Call | Round-trips | Note |
|------|-------------|------|
| `OCI_Commit`, `OCI_Rollback` | 1 | — `OCI_ConnectionSetAutoCommit` folds the commit into the execute trip instead |
| `OCI_TransactionStart`, `OCI_TransactionStop`, `OCI_TransactionResume`, `OCI_TransactionPrepare`, `OCI_TransactionForget` | 1 each | local/global transaction verbs |
| `OCI_TransactionCreate` | 0 | local object |

## LOBs and LONGs

LOB locator calls each cross the wire, which is why streaming a LOB piece by
piece adds up:

| Call | Round-trips | Note |
|------|-------------|------|
| `OCI_LobGetLength`, `OCI_LobGetChunkSize` | 1 | |
| `OCI_LobOpen`, `OCI_LobClose`, `OCI_LobIsOpen` | 1 | |
| `OCI_LobRead`, `OCI_LobWrite`, `OCI_LobAppend` | 0 or 1 | 0 while data stays in the client-side LOB buffer; `OCI_LobFlush` pushes modified pages (1 per page) |
| `OCI_LobTruncate`, `OCI_LobErase`, `OCI_LobCopy` | 1 | |
| `OCI_LobIsTemporary`, `OCI_LobGetType`, `OCI_LobGetOffset` | 0 | locator state is client-side |
| `OCI_Long*` piecewise read/write | 1 per piece | piece info is negotiated with the server — see [LONGs & LONG RAW](longs.html) |

Oracle's own guidance: when the data fits, prefer binding/defining text→`CLOB`
and raw→`BLOB` through the normal data interface — it avoids locator calls
entirely.

## Describes and metadata

| Call | Round-trips | Note |
|------|-------------|------|
| `OCI_TypeInfoGet` | 1 | server describe; the returned `OCI_TypeInfo` is then inspectable locally |
| column info on an executed statement | 0 | describe data rides back with execute/fetch — the reason `Execute` is preferred over `Describe` + `Execute` |

## Objects and collections

Once an object or collection is loaded in the client-side object cache,
**everything you do to it is local**: `OCI_Object*` attribute accessors,
`OCI_Collection*` getters (`GetElement`, `Append`, `Trim`, `Count`, …),
iterators, mark/unmark — all 0. The trips hide in two places:

| Call | Round-trips | Note |
|------|-------------|------|
| first touch of a not-yet-loaded object/collection | 1 | lazy-load into the object cache, then local |
| `OCI_TypeInfoGet` on object types | 1 + | describing a type not in the cache costs extra trips — one per collection element/attribute/method descriptor |

## SODA

The document store is deliberately simple: **every operation is one
round-trip, everything on your side of the handle is local**.

| Call | Round-trips | Note |
|------|-------------|------|
| `OCI_SodaDatabaseCreateCollection`, `OCI_SodaDatabaseOpenCollection`, `OCI_SodaDatabaseGetCollections`, `OCI_SodaDatabaseGetCollectionNames` | 1 each | collection DDL/listing |
| `OCI_SodaCollectionInsertDocument`, `OCI_SodaCollectionInsertMany`, `OCI_SodaCollectionSave` | 1 | `InsertMany` ships the whole batch in one trip — the SODA equivalent of array binding |
| `OCI_SodaCollectionFind`, `FindOne`, `FindOneByKey`, `DocumentCount*`, `ReplaceOne*`, `Remove*` | 1 each | |
| `OCI_SodaCollectionDrop`, `Truncate`, `CreateIndex`, `DropIndex`, `GetDataGuide`, `ListIndexes` | 1 each | |
| `OCI_SodaDocumentCursorGetNext`, `OCI_SodaCollectionCursorGetNext` | 0 or 1 | batched like `FetchNext` — sized by `OCI_SodaOptionsSetFetchArraySize` |
| `OCI_SodaDatabaseCreate`, `OCI_SodaDocumentCreate*`, `OCI_SodaOptionsCreate`/`Set*`, document getters, `IsMapped`, `GetMetadata`, all `*Free` | 0 | local objects and metadata |

## JSON

The in-memory JSON DOM is essentially a client-side library — parsing, node
creation and tree manipulation don't touch the server:

| Call | Round-trips | Note |
|------|-------------|------|
| `OCI_JsonLoadText`, `OCI_JsonLoadBinary` | 0 | parsing is local |
| `OCI_JsonNodeCreate*`, `OCI_JsonNodeGet*`/`Set*`, `OCI_JsonSetRoot` | 0 | pure DOM work |
| `OCI_JsonToText`, `OCI_JsonToBinary`, `OCI_JsonClone` | 0 or 1 | usually local — a trip can occur when the document is bound to server state |

## Queues and notifications

`OCI_EnqueuePut` and `OCI_DequeueGet` cost one trip each — they post and pop
against a server queue. `OCI_DequeueSubscribe` registers a server-side
subscription (1); after that, callbacks arrive server-initiated — no polling
trips. `OCI_SubscriptionCreate` registers for change notification (1); the
notifications themselves are pushed.

## Always local

These never reach the server: all resultset getters, `OCI_GetLastError` and the
whole `OCI_Error*` surface, every `OCI_*Free` (and C++ destructor), all datatype
manipulation — `OCI_Date*`, `OCI_Interval*`, `OCI_Timestamp*`, `OCI_Number*`,
`OCI_String*` — handle attribute getters/setters, and `OCI_Statement*` setters
of all kinds.

## Cutting round-trips

The levers, in order of payoff:

1. **Raise `OCI_StatementSetFetchSize`** — one execute, then batches of rows per
   refill instead of one round-trip per row. See [fetch tuning](fetch-tuning.html).
2. **`Prepare` once, `ExecutePrepared` many** — never `Parse` in a loop; let the
   parse ride inside the execute. See [prepare once, execute many](prepared-reexecute.html).
3. **Array binding** — N rows of DML in one execute round-trip. See
   [array binding](array-binding.html).
4. **`OCI_ConnectionSetAutoCommit` / `OCI_COMMIT_ON_SUCCESS`** — the commit
   shares the execute trip instead of adding its own.
5. **Pool connections** — session-establishment trips are paid once at pool
   creation, not per use. See [connection pools](pooling-basics.html).

## Recap

- `Prepare` is free, `Parse` costs a trip — the names suggest the opposite.
- `Execute` bundles parse + run + describe in a single trip; `Describe` adds a
  redundant one.
- `FetchNext` is only as chatty as your fetch size is small.
- Everything else is local: getters, binds, setters, frees, errors.
