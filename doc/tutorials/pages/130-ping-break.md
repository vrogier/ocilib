---
title: Ping & break — liveness and aborting calls
navtitle: Ping & break
part: 1
level: intermediate
demos: [abort.c]
---

# Ping & break — liveness and aborting calls

**Objectives** — by the end of this page you will be able to:

- check that a connection is still alive before trusting it
- abort a long-running call from another thread
- understand what happens to the interrupted call

## Two liveness questions

A connection that *looks* open can be dead — network blips, idle timeouts, database
restarts. Two operations deal with that reality:

- **ping** answers "*is the session still alive?*" — a cheap round-trip probe;
- **break** answers "*cancel the call running on this connection*" — used from
  another thread while the first is blocked inside a call.

## Ping — is the server still there?

The ping sends a lightweight round-trip; `TRUE` means the session round-trips, `FALSE` means it's dead.

Use it before reusing a long-idle connection, after a network blip, or in health-check loops:

:::sample
```c
if (!OCI_ConnectionPing(cn))
{
    fprintf(stderr, "connection lost\n");
}
```

```cpp
if (!con.PingServer())
{
    std::cerr << "connection lost\n";
}
/* con.IsServerAlive() pings implicitly */
```
:::

!!! note "Not an error check"
    Ping answers "alive?" — `FALSE` means *the session is dead*, not that an error
    occurred; don't route it through the error-handler mindset.

## Break — abort a running call

Some calls block for a long time: `dbms_lock.sleep`, a huge query, a dequeue wait.
**Break** interrupts the call currently executing *on that connection* — invoked
from a *different* thread:

:::sample
```c
#include <threads.h>

static int run_sleep(void *arg)
{
    OCI_Statement *st = arg;                       /* blocked ~10 s unless broken */
    OCI_StatementExecute(st, OTEXT("begin dbms_lock.sleep(10); end;"));
    return 0;
}

int main(void)
{
    /* … env + connection … */

    OCI_Statement *st = OCI_StatementCreate(cn);
    thrd_t t;
thrd_create(&t, run_sleep, st);

    sleep(1);
    OCI_ConnectionBreak(cn);                                 /* aborts the call in thread t */

    thrd_join(t, NULL);                            /* worker saw ORA-01013        */
    /* … */
}
```

```cpp
/* same idea: another thread calls con.Break() while this thread is inside st.Execute() */
```
:::

The interrupted call fails with **ORA-01013: user requested cancel of current
operation** — delivered to the error channel of the *worker* thread, not the
breaking one.

!!! warning "Break is per-connection, per in-flight call"
    It cancels the operation *in progress* on that connection — not a general
    "kill session". Use it for UI cancel buttons and watchdogs, and create the
    environment with the threaded flag whenever threads touch the API.

## Putting it together — the typical sequence

Liveness and interruption are the two sides of "what if the connection dies":

1. **Probe cheaply**: `OCI_Ping` is the lightweight "still there?" check — a
   single round-trip, far cheaper than a dummy `select 1 from dual`.
2. **Interrupt deliberately**: `OCI_Break` aborts an in-flight call *from
   another thread* — it's how you implement a cancel button or a watchdog
   timeout on a long query.
3. **Reconnect on failure**: `OCI_Reconnect` re-establishes a dead session
   without rebuilding your statements from scratch.

Hints: `Ping` costs a round-trip — don't call it inside a fetch loop, call it
before starting real work or when the app goes idle→active; `Break` is for
*other* threads — calling it from the executing thread does nothing useful;
and a broken call returns FALSE with `OCI_GetLastError` saying interrupted —
that's a normal path, not a bug.

## Try it yourself

1. Run the pattern above and confirm the worker finishes in ~1 s with ORA-01013
   instead of 10 s.
2. Break when nothing is running — it is a no-op.
3. Loop ping while you kill the database/network and watch it flip to FALSE.

## Common pitfalls

- **Breaking from the same thread** — pointless; that thread is inside the call.
- **Expecting ping to throw** — it answers "alive?" — check the return.
- **Sharing one connection across worker + breaker without the threaded environment
  flag** — undefined behavior; see Part 5.

## Recap

- Ping = liveness probe (`TRUE` = alive); break = cancel the in-flight call,
  interrupted side gets ORA-01013.

**Next:** introspect the library itself — versions, import mode, charset.
