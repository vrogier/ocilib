---
title: AQ — async dequeue & named agents
navtitle: Async dequeue & agents
part: 7
level: advanced
demos: [queue_async.c]
---

# AQ — async dequeue & named agents

**Objectives** — by the end of this page you will be able to:

- register dequeue callbacks instead of polling
- create named `OCI_Agent` consumers/producers
- unsubscribe cleanly

## Concepts

Polling wastes round-trips.

**Subscribing** your dequeues once lets Oracle call your callback when a message arrives — needs the environment's events flag (like all async features):

:::sample
```c
void on_message(OCI_Dequeue *deq)
{
    OCI_Message *msg = OCI_DequeueGet(deq);        /* the message that woke us */
    OCI_Object *obj = OCI_MessageGetObject(msg);
    printf("got: %s\n", OCI_ObjectGetString(obj, OTEXT("TITLE")));
    OCI_MessageFree(msg);
    OCI_ConnectionCommit(cn);                          /* dequeue is transactional — commit it */
}
```

```cpp
// handler runs inside Dequeue::Get context
```
:::

:::sample
```c
OCI_Environment *env = OCI_EnvironmentCreate(err, NULL, OCI_ENV_EVENTS);
/* … queue table/queue started, dequeues created … */

OCI_Agent *c1 = OCI_AgentCreate(con, OTEXT("C1"), NULL);
OCI_DequeueSetConsumer(deq1, OTEXT("C1"));
OCI_DequeueSetNavigation(deq1, OCI_ADN_FIRST_MSG);

OCI_Dequeue *deqs[2] = { deq1, deq2 };
OCI_EnvironmentSetSubscriptionPort(env, 9998);
OCI_DequeueSubscribe(con, deqs, 2, 0, on_message);   /* async! */

/* … later … */
OCI_DequeueUnsubscribe(deq1);
OCI_DequeueUnsubscribe(deq2);
```

```cpp
Environment env(Environment::Events);               /* events required */
/* … queue table/queue started, dequeues created … */

Agent c1(con, "C1", "");
deq1.SetConsumer("C1");
deq1.SetNavigation(Dequeue::NavFirst);

std::vector<Dequeue> deqs{deq1, deq2};
env.SetSubscriptionPort(9998);
Dequeue::Subscribe(con, deqs, 0, on_message);          /* async! */

/* … later … */
for (auto& dq : deqs) dq.Unsubscribe();
```
:::

## The Agent — named endpoints

An `Agent` is a named producer/consumer identity (`AgentCreate(con, name, addr)` /
`Agent(con, name, addr)`):

- A dequeue set to a consumer name only gets that agent's messages.
- A message's consumer list targets specific agents at enqueue time.
- Name/address are get/set on the object.

That's pub/sub: enqueue to a consumer list, each named consumer gets its copy.

## The async lifecycle

1. Events-flagged environment + a subscription port for callbacks.
2. Create dequeues, set consumer/navigation.
3. Subscribe a dequeue set to a callback — one registration per set.
4. Callbacks fire on message arrival — dequeue inside the callback.
5. Unsubscribe to stop.

## Putting it together — the typical sequence

Async dequeue is a callback, not a loop:

1. **Register the callback** — `OCI_DequeueSubscribe(queue, callback)` with
   your handler signature.
2. **Provide an `OCI_Event`-style context** — the callback fires when a
   message arrives.
3. **Process in the callback** — payload access is the same `MsgGet` calls.
4. **`DequeueUnsubscribe`** when done — the subscription dies with the
   connection otherwise.

Hints: callbacks run on an OCILIB notification thread — keep them short and
thread-aware; subscribe once per queue, not per message; and async is for
consumers — producers still enqueue the synchronous way.

## Try it yourself

1. Subscribe one dequeue; enqueue 3 messages; count callback invocations.
2. Two named consumers — send targeted messages and verify routing.
3. Unsubscribe mid-stream and confirm callbacks stop.

## Common pitfalls

- **No `OCI_ENV_EVENTS`** — subscribe fails silently/errors; the flag is mandatory.
- **Callback doing too much** — it's a notification context; dequeue quickly and
  defer heavy work.
- **Port conflicts** — `OCI_EnvironmentSetSubscriptionPort` needs a free UDP/TCP port.

## Recap

- `DequeueSubscribe` + `OCI_ENV_EVENTS` → callback-driven consumption; `OCI_Agent`
  names route messages.

**Next:** change notification — database events on any table.
