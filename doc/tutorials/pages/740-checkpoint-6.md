---
title: Checkpoint 6 — jobs & notifications
navtitle: Checkpoint 6 jobs
part: 7
level: advanced
---

# Checkpoint 6 — jobs & notifications

You should now be able to create queues, enqueue/dequeue messages with payloads and
properties, consume asynchronously, and react to database change events.

## The exercise — employee job queue

**Server side:** a `job_t` object payload, queue table + started queue (page 710).

```sql
create type job_t as object (job_id number, action varchar2(20), empno number);
```

**The program** — two modes in one binary:

1. `jobs submit` — reads job lines from stdin (`RAISE 7566`), builds `job_t`
   objects, sets correlation `"emp-" || empno` and priority, enqueues, commits.
2. `jobs work` — an `OCI_Dequeue` consumer that:
   - waits with `DequeueSetWaitTime(-1)`,
   - on each `job_t`: performs the action (e.g. `update employees set sal=sal*1.1`),
     commits, prints the `MsgGetID` + `MsgGetCorrelation`.

**Stretch:**
- Replace polling with `OCI_DequeueSubscribe` + `OCI_ENV_EVENTS` callback.
- Add `OCI_SubscriptionRegister` on `employees` with `OCI_CNT_ROWS` and log which
  ROWIDs your jobs changed.
- Two named consumers (`OCI_Agent` "CLERK"/"MANAGER") routed by `MsgSetConsumers`.

## What it exercises

| Skill | Pages |
|-------|-------|
| Queue plumbing | 710 |
| Msg payloads + props | 715 |
| Dequeue options | 720 |
| Async agents | 725 |
| CQN subscriptions/events | 730 |

Part 7 done. Next: bulk loading (array binds, direct path) and admin.
