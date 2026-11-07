---
title: AQ — enqueueing messages
navtitle: Enqueueing messages
part: 7
level: intermediate
demos: [queue.c]
---

# AQ — enqueueing messages

**Objectives** — by the end of this page you will be able to:

- build `OCI_Msg` payloads (objects or raw)
- set message properties: priority, expiration, correlation, delay
- post messages and understand the transaction rules

## Meet the message object

A `Msg` is the unit you post — a shell created from the payload's `TypeInfo`, filled with either a typed `Object` (same TypeInfo as the queue payload) or opaque raw bytes for RAW-payload queues.

Enqueue posts it; **the post is transactional** — it becomes visible at commit.

:::sample
```c
OCI_TypeInfo *inf = OCI_TypeInfoGet(con, OTEXT("message_t"), OCI_TIF_TYPE);
OCI_Enqueue  *enq = OCI_EnqueueCreate(inf, OTEXT("message_queue"));
OCI_Msg      *msg = OCI_MsgCreate(inf);
OCI_Object   *obj = OCI_ObjectCreate(inf);

OCI_ObjectSetString(obj, OTEXT("TITLE"),   OTEXT("NEXT MEETING"));
OCI_ObjectSetString(obj, OTEXT("CONTENT"), OTEXT("12:00 PM"));
OCI_MsgSetObject(msg, obj);

OCI_EnqueuePut(enq, msg);
OCI_Commit(con);                        /* enqueue is transactional! */

OCI_MsgFree(msg); OCI_ObjectFree(obj);
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
| `OCI_MsgSetObject` | `Message::SetObject` | object payload |
| `OCI_MsgSetRaw` | `Message::SetRaw` | opaque bytes payload |
| `OCI_MsgSetPriority` | `SetPriority` | queue priority (higher = sooner) |
| `OCI_MsgSetExpiration` | `SetExpiration` | move to exception queue after N s |
| `OCI_MsgSetEnqueueDelay` | `SetEnqueueDelay` | delayed delivery |
| `OCI_MsgSetCorrelation` | `SetCorrelation` | correlation id — dequeue filters on it |
| `OCI_MsgSetExceptionQueue` | `SetExceptionQueue` | dead-letter queue |
| `OCI_MsgSetSender` | `SetSender` | `Agent` producer identity |
| `OCI_MsgSetConsumers` | `SetConsumers` | named consumers (multi-consumer queues) |
| `OCI_MsgSetOriginalID` | `SetOriginalID` | propagate/dequeue chains |
| `OCI_MsgGetID` | `GetID()` | assigned id after `Put` |
| `OCI_MsgGetEnqueueTime` | `GetEnqueueTime()` | when posted |
| `OCI_MsgGetState` | `GetState()` | READY/WAITING/PROCESSED/EXPIRED |

Enqueue-level options live on the endpoint: visibility (`OCI_AMV_*` ↔
`Enqueue::Immediate`/`OnCommit`), relative message id, sequence deviation.

## Transactions matter

Enqueue participates in your transaction — the message becomes visible only at
commit unless you set immediate visibility instead.

## Try it yourself

1. Enqueue 3 messages with different priorities; dequeue in priority order.
2. Send a delayed message (`SetEnqueueDelay(5)`) and time its arrival.
3. Publish to a named consumer list and watch only matching dequeues get it.

## Common pitfalls

- **Forgetting `OCI_Commit`** — messages never become visible.
- **Msg/Object freed too early** — keep the payload alive through `EnqueuePut`.
- **Correlation mismatch** — dequeue filter must match exactly.

## Recap

- `MsgCreate` + `MsgSetObject`/`SetRaw` + `MsgSet*` props → `EnqueuePut` → `Commit`.

**Next:** dequeue — options, navigation, and waiting for messages.
