---
title: Database change notification — OCI_Subscription & OCI_Event
navtitle: Change notification
part: 7
level: advanced
demos: [notification.c, notification.cpp]
---

# Database change notification — `Subscription` & `Event`

**Objectives** — by the end of this page you will be able to:

- register for change notifications on objects and rows (CQN)
- read event contents: type, object, rowid, operation
- scope notifications to specific statements

## Meet the subscription and the event

**Continuous Query Notification** lets the database *push* events to your app when
data changes — no polling.

A `Subscription` is a registered interest: you subscribe on a connection, attach statements to scope it, and your handler fires when the watched objects change.

Each firing hands the handler an `Event` — a transient record (like the error object) telling you what changed.

Two environment prerequisites: the **events flag** at creation, and a
**subscription port** for callbacks.

:::sample
```c
void event_handler(OCI_Event *event)
{
    printf("type=%u db=%s obj=%s rowid=%s\n",
           OCI_EventGetType(event), OCI_EventGetDatabase(event),
           OCI_EventGetObject(event), OCI_EventGetRowid(event));
}

OCI_Environment *env = OCI_EnvironmentCreate(err, NULL, OCI_ENV_EVENTS);
/* … connect … */
OCI_SetSubscriptionPort(env, 5468);

OCI_Subscription *sub = OCI_SubscriptionRegister(
    con, OTEXT("sub-00"), OCI_CNT_ALL, event_handler, OCI_SRM_DEFAULT, 0);

OCI_Prepare(st, OTEXT("select * from employees"));
OCI_SubscriptionAddStatement(sub, st);       /* watch objects this stmt touches */

/* … when done … */
OCI_SubscriptionUnregister(sub);
```

```cpp
void event_handler(ocilib::Event &event)
{
    std::cout << "type=" << event.GetType() << " db=" << event.GetDatabase()
              << " obj=" << event.GetObject() << "\n";
}

Environment env(Environment::Events);
/* … connect … */
env.SetSubscriptionPort(5468);

Subscription sub = con.Subscribe("sub-00",
    Subscription::NotifyAll, event_handler, Subscription::SecureNone);

st.Prepare("select * from employees");
sub.Register(st);                            /* watch objects this stmt touches */

/* … when done … */
sub.Unregister();
```
:::

## What an event carries

| C function | C++ method | Returns |
|------------|------------|---------|
| `OCI_EventGetType` | `Event::GetType()` | `OBJECT_CHANGED`/`STARTUP`/`SHUTDOWN`/`DROP_DATABASE`/`DEREGISTER` |
| `OCI_EventGetDatabase` | `GetDatabase()` | db that changed |
| `OCI_EventGetObject` | `GetObject()` | table that changed |
| `OCI_EventGetRowid` | `GetRowid()` | the changed row (with rows-level) |
| `OCI_EventGetOperation` | `GetOperation()` | insert/update/delete/alter flag |
| `OCI_EventGetSubscription` | `GetSubscription()` | owning subscription |

## Notification levels

| C enum | C++ enum | Granularity |
|--------|----------|-------------|
| `OCI_CNT_OBJECTS` | `Subscription::NotifyObjects` | object-level: "employees changed" |
| `OCI_CNT_ROWS` | `Subscription::NotifyRows` | row-level: which ROWIDs changed |
| `OCI_CNT_DATABASES` | `Subscription::NotifyDatabases` | db-level events (startup/shutdown/HA) |
| `OCI_CNT_ALL` | `Subscription::NotifyAll` | everything |

The secure-notification mode flag (19.4+) requests client-initiated secure
notification — **UNICODE builds hit a known Oracle client bug** on it; ANSI builds
are fine.

## Subscription management

- Attach statements to scope the subscription to the objects they touch.
- Name/timeout/connection inspect the registration.
- Unregistering releases it server-side too.

## Try it yourself

1. Register `OCI_CNT_ROWS` on `employees`, then `update` a row from SQL*Plus —
   watch the event arrive.
2. Subscribe with `OCI_CNT_DATABASES` and bounce a test instance (if you can).
3. Add a second statement and confirm both tables' events fire.

## Common pitfalls

- **No `OCI_ENV_EVENTS`** — registration fails; flag first.
- **Firewall blocks the port** — `SetSubscriptionPort` must be reachable.
- **Expecting rowid on `OCI_CNT_OBJECTS`** — row-level data needs `OCI_CNT_ROWS`.
- **UNICODE + `SRM_SECURE_NOTIFICATION`** — known Oracle client limitation (v5 notes).

## Recap

- `SubscriptionRegister` + `AddStatement` → `event_handler(OCI_Event*)` gets every
  change; event carries type/object/rowid/operation.

**Next:** Checkpoint 6 — a job queue with workers and notifications.
