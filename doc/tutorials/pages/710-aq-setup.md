---
title: Advanced Queuing — queues & queue tables
navtitle: Queues & queue tables
part: 7
level: intermediate
demos: [queue.c]
sql: queue.sql
---

# Advanced Queuing — queues & queue tables

**Objectives** — by the end of this page you will be able to:

- define a payload type and queue table
- create, start, alter, stop and drop queues
- understand what `Enqueue`/`Dequeue` objects bind to

## Concepts

Oracle AQ needs two server objects:

1. **Queue table** — storage for messages of a given payload type (an object type,
   or `RAW` for opaque payloads).
2. **Queue** — the logical channel over the table; must be *started* before use.

```sql
create type message_t as object (title varchar2(50), content varchar2(100));
```

## Creating the plumbing

The connection carries the queue-admin calls — create the storage table, then the
queue over it, then start it:

:::sample
```c
OCI_QueueTableCreate(con, OTEXT("message_queue_table"), OTEXT("message_t"),
                     NULL, NULL, FALSE, OCI_AGM_NONE, NULL, 0, 0, NULL);
OCI_QueueCreate(con, OTEXT("message_queue"), OTEXT("message_queue_table"),
                OCI_AQT_NORMAL, 0, 0, 0, FALSE, NULL);
OCI_QueueStart(con, OTEXT("message_queue"), TRUE, TRUE);   /* enqueue+dequeue */
```

```cpp
con.QueueTableCreate("message_queue_table", "message_t",
                     ..., Message::MessageGroupingNone, ...);
con.QueueCreate("message_queue", "message_queue_table",
                Queue::Normal, 0, 0, 0, false, ...);
con.QueueStart("message_queue", true, true);
```
:::

| C function | C++ method | Purpose |
|------------|------------|---------|
| `OCI_QueueTableCreate` | `Connection::QueueTableCreate` | create storage |
| `OCI_QueueTableAlter` | `QueueTableAlter` | alter storage |
| `OCI_QueueTableDrop` | `QueueTableDrop` | drop storage |
| `OCI_QueueTableMigrate` | `QueueTableMigrate` | migrate storage |
| `OCI_QueueTablePurge` | `QueueTablePurge` | purge storage |
| `OCI_QueueCreate` | `Connection::QueueCreate` | create the queue |
| `OCI_QueueStart` | `QueueStart` | enable enqueue/dequeue |
| `OCI_QueueStop` | `QueueStop` | stop it |
| `OCI_QueueAlter` | `QueueAlter` | alter it |
| `OCI_QueueDrop` | `QueueDrop` | drop it |

| C enum | C++ enum | Meaning |
|--------|----------|---------|
| `OCI_AQT_NORMAL` | `Queue::Normal` | durable queue |
| `OCI_AQT_EXCEPTION` | `Queue::Exception` | exception queue |
| `OCI_AQT_NON_PERSISTENT` | `Queue::NonPersistent` | non-persistent |
| `OCI_AGM_NONE` | `Message::MessageGroupingNone` | no grouping |
| `OCI_AGM_TRANSACTIONNAL` | `Message::MessageGroupingTransactional` | grouped by transaction |

## The client-side trio

Three objects bridge your program to a queue — and all hang off the payload's
`TypeInfo` (Part 6 pays off here: **AQ payloads are objects**):

- **TypeInfo** — the payload descriptor (`TypeInfo(con, "message_t", TypeInfo::Type)`).
- **Enqueue / Dequeue** — endpoints bound to the queue, one per direction
  (`EnqueueCreate(ti, name)` / `Enqueue(ti, name)`).
- **Msg / Message** — a message shell carrying an `Object` payload (or raw bytes
  for RAW queues).

## Putting it together — the typical sequence

AQ needs server setup before any client call:

1. **Create the queue server-side** — `DBMS_AQADM.CREATE_QUEUE` +
   `CREATE_QUEUE_TABLE` + `START_QUEUE`, or a `QUEUE TABLE` with typed
   payload.
2. **Get a `OCI_Queue` handle** — `OCI_QueueCreate(con, "qname", type)` on
   the client.
3. **Enqueue/dequeue** — the two verbs, detailed on the next pages.

Hints: the queue must be *started* server-side — `START_QUEUE` is the
commonly-forgotten step; the payload type determines which OCILIB calls
apply — `OCI_Object` payloads for typed queues, `OCI_Raw`/text for generic;
and dequeue without a started queue hangs or errors — `GET_QUEUE_STATUS`
server-side is the health check.

## Try it yourself

1. Create queue table + queue, start it, then `QueueAlter` the retention time.
2. `Purge` a test queue table after playing (careful — destructive).
3. Describe `user_queues` in SQL to confirm your OCILIB calls took effect.

## Common pitfalls

- **Queue not started** — `EnqueuePut`/`DequeueGet` fail until `QueueStart`.
- **Multiple consumers** — must be set at `QueueTableCreate`; can't be added later.
- **`OCI_AGM_TRANSACTIONNAL`** — message grouping needs it for group dequeues.

## Recap

- TypeInfo(payload) → QueueTable → Queue → Start → Enqueue/Dequeue endpoints.

**Next:** enqueue — building and posting messages.
