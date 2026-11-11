---
title: AQ — enqueueing messages
navtitle: Enqueueing messages
part: 7
level: intermediate
demos: [queue.c]
---

# AQ — enqueueing messages

**Objectives** — by the end of this page you will be able to:

- build `OCI_Message` payloads (objects or raw)
- set message properties: priority, expiration, correlation, delay
- post messages and understand the transaction rules

## The message object

A `Msg` is the unit you post — a shell created from the payload's `TypeInfo`, filled with either a typed `Object` (same TypeInfo as the queue payload) or opaque raw bytes for RAW-payload queues.

Enqueue posts it; **the post is transactional** — it becomes visible at commit.

:::sample
```c
OCI_TypeInfo *inf = OCI_TypeInfoGet(con, OTEXT("message_t"), OCI_TIF_TYPE);
OCI_Enqueue  *enq = OCI_EnqueueCreate(inf, OTEXT("message_queue"));
OCI_Message      *msg = OCI_MessageCreate(inf);
OCI_Object   *obj = OCI_ObjectCreate(inf);

OCI_ObjectSetString(obj, OTEXT("TITLE"),   OTEXT("NEXT MEETING"));
OCI_ObjectSetString(obj, OTEXT("CONTENT"), OTEXT("12:00 PM"));
OCI_MessageSetObject(msg, obj);

OCI_EnqueuePut(enq, msg);
OCI_ConnectionCommit(con);                        /* enqueue is transactional! */

OCI_MessageFree(msg);
OCI_ObjectFree(obj);
```

```cpp
TypeInfo inf(con, "message_t", TypeInfo::Type);
Enqueue  enq(inf, "message_queue");
Message  msg(inf);
Object   obj(inf);

obj.Set<String>("TITLE",   "NEXT MEETING");
obj.Set<String>("CONTENT", "12:00 PM");
msg.SetObject(obj);

enq.Put(msg);
con.Commit();                           /* enqueue is transactional! */
```
:::

## Message properties

| C function | C++ method | Meaning |
|------------|------------|---------|
| `OCI_MessageSetObject` | `Message::SetObject` | object payload |
| `OCI_MessageSetRaw` | `Message::SetRaw` | opaque bytes payload |
| `OCI_MessageSetPriority` | `SetPriority` | queue priority (higher = sooner) |
| `OCI_MessageSetExpiration` | `SetExpiration` | move to exception queue after N s |
| `OCI_MessageSetEnqueueDelay` | `SetEnqueueDelay` | delayed delivery |
| `OCI_MessageSetCorrelation` | `SetCorrelation` | correlation id — dequeue filters on it |
| `OCI_MessageSetExceptionQueue` | `SetExceptionQueue` | dead-letter queue |
| `OCI_MessageSetSender` | `SetSender` | `Agent` producer identity |
| `OCI_MessageSetConsumers` | `SetConsumers` | named consumers (multi-consumer queues) |
| `OCI_MessageSetOriginalID` | `SetOriginalID` | propagate/dequeue chains |
| `OCI_MessageGetID` | `GetID()` | assigned id after `Put` |
| `OCI_MessageGetEnqueueTime` | `GetEnqueueTime()` | when posted |
| `OCI_MessageGetState` | `GetState()` | READY/WAITING/PROCESSED/EXPIRED |

Enqueue-level options live on the endpoint: visibility (`OCI_AMV_*` ↔
`Enqueue::Immediate`/`OnCommit`), relative message id, sequence deviation.

## Transactions matter

Enqueue participates in your transaction — the message becomes visible only at
commit unless you set immediate visibility instead.

## Putting it together — the typical sequence

Enqueueing pushes a message into the queue:

1. **Build the message** — `OCI_MsgCreate` on the queue's payload type.
2. **Fill it** — `MsgSet` attributes (payload object, correlation, priority,
   expiration).
3. **Enqueue** — `OCI_EnqueuePut(queue, msg)` posts it; the message ID comes
   back.
4. **Check the result** — enqueue is transactional — it commits with the
   rest of your work.

Hints: message priority decides dequeue order — same-priority is FIFO;
correlation IDs are how you route replies — set one if the consumer filters;
and the payload type must match the queue's declared type — `OCI_Object` for
ADT queues, `OCI_Raw` for raw queues.

## Try it yourself

1. Enqueue 3 messages with different priorities; dequeue in priority order.
2. Send a delayed message (`SetEnqueueDelay(5)`) and time its arrival.
3. Publish to a named consumer list and watch only matching dequeues get it.

## Common pitfalls

- **Forgetting `OCI_ConnectionCommit`** — messages never become visible.
- **Msg/Object freed too early** — keep the payload alive through `EnqueuePut`.
- **Correlation mismatch** — dequeue filter must match exactly.

## Recap

- `MsgCreate` + `MsgSetObject`/`SetRaw` + `MsgSet*` props → `EnqueuePut` → `Commit`.

**Next:** dequeue — options, navigation, and waiting for messages.
