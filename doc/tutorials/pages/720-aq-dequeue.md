---
title: AQ — dequeueing messages
navtitle: Dequeueing messages
part: 7
level: intermediate
demos: [queue.c]
---

# AQ — dequeueing messages

**Objectives** — by the end of this page you will be able to:

- pull messages off a queue
- tune dequeue behavior: wait, navigation, visibility, consumer, correlation
- inspect message state and metadata

## Meet the Dequeue endpoint

The `Dequeue` is the consuming endpoint — bound to the queue's TypeInfo+name like `Enqueue`, it pulls `Msg` objects off the queue.

Pulls are transactional: consuming a message marks it removed *inside your transaction* — commit to make it stick, rollback puts it back.

:::sample
```c
OCI_Dequeue *deq = OCI_DequeueCreate(inf, OTEXT("message_queue"));
OCI_Msg *msg = OCI_DequeueGet(deq);               /* blocks per wait settings */
if (msg)
{
    OCI_Object *obj = OCI_MsgGetObject(msg);      /* payload as object */
    printf("%s\n", OCI_ObjectGetString(obj, OTEXT("TITLE")));
    OCI_MsgFree(msg);
}
OCI_Commit(con);                                  /* dequeue is transactional too */
```

```cpp
Dequeue deq(inf, "message_queue");
Message msg = deq.Get();                          /* blocks per wait settings */
Object obj = msg.GetObject();
std::cout << obj.Get<String>("TITLE") << "\n";
con.Commit();
```
:::

## Dequeue options

| C function | C++ method | Meaning |
|------------|------------|---------|
| `OCI_DequeueSetWaitTime` | `SetWaitTime` | block this long for a message (0 = don't wait, -1 = forever) |
| `OCI_DequeueGetWaitTime` | `GetWaitTime()` | read it |
| `OCI_DequeueSetNavigation` | `SetNavigation` | `FIRST_MSG`/`NEXT_MSG`/`NEXT_TRANSACTION` — cursor behavior |
| `OCI_DequeueGetNavigation` | `GetNavigation()` | read it |
| `OCI_DequeueSetMode` | `SetMode` | remove vs browse (peek without consuming) |
| `OCI_DequeueGetMode` | `GetMode()` | read it |
| `OCI_DequeueSetVisibility` | `SetVisibility` | within-transaction vs immediate |
| `OCI_DequeueGetVisibility` | `GetVisibility()` | read it |
| `OCI_DequeueSetConsumer` | `SetConsumer` | named consumer on multi-consumer queues |
| `OCI_DequeueGetConsumer` | `GetConsumer()` | read it |
| `OCI_DequeueSetCorrelation` | `SetCorrelation` | only msgs with this correlation id |
| `OCI_DequeueSetRelativeMsgID` | `SetRelativeMsgID` | relative positioning |
| `OCI_DequeueSetAgentList` | `SetAgentList` | candidate consumers |
| `OCI_DequeueListen` | `Listen` | which agent should dequeue (multi-consumer) |
| `OCI_DequeueSubscribe` | `Connection::DequeueSubscribe` | async callback (next page) |

## Which agent — `Listen`

For multi-consumer queues, listen tells you *which* agent should dequeue:

:::sample
```c
OCI_Agent *a = OCI_DequeueListen(deq, 5);   /* wait ≤5s for an agent signal */
if (a) printf("message for consumer %s\n", OCI_AgentGetName(a));
```

```cpp
Agent a = deq.Listen(5);                    /* wait ≤5s for an agent signal */
if (a)
{
    std::cout << "message for consumer " << a.GetName() << "\n";
}
```
:::

## Message metadata on the way out

Attempt count, enqueue time, state (READY/WAITING/PROCESSED/EXPIRED), id,
correlation, sender, original id, exception queue — everything needed for
poison-message handling and routing logic (page 715's table).

## Try it yourself

1. Dequeue with `WaitTime(0)` on an empty queue vs `WaitTime(-1)` after a enqueue.
2. Browse (peek) the same message twice with browse mode, then remove-mode get it.
3. Filter by correlation: enqueue two messages with different correlation ids and
   dequeue only one.

## Common pitfalls

- **Consumed message not committed** — rollback puts it back on the queue.
- **`FIRST_MSG` each call** — restarts the scan; use `NEXT_MSG` to walk forward.
- **Multi-consumer without consumer name** — no message is delivered.

## Recap

- `DequeueGet` pulls (respecting wait/navigation/mode/visibility/consumer);
  `Commit` removes for good; `DequeueListen` routes named consumers.

**Next:** async dequeue callbacks and `OCI_Agent` publish/subscribe.
