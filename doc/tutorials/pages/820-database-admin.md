---
title: Database administration — startup & shutdown
navtitle: Database admin
part: 8
level: advanced
demos: [instance.c]
---

# Database administration — startup & shutdown

**Objectives** — by the end of this page you will be able to:

- start and stop a database instance with `OCI_DatabaseStartup`/`Shutdown`
- pick the right mode and flag constants
- connect with admin session modes (`SYSDBA`, `PRELIM_AUTH`)

## Concepts

These calls mirror `STARTUP`/`SHUTDOWN` in SQL*Plus — they need a privileged
session mode and take the environment (not a connection — the DB may not even be
up yet):

:::sample
```c
OCI_DatabaseStartup(env, db, usr, pwd,
                    OCI_SESSION_SYSDBA,   /* admin session          */
                    OCI_DB_SPM_FULL,      /* startup mode           */
                    OCI_DB_SPF_FORCE,
                    NULL);              /* spfile (optional)      */

OCI_DatabaseShutdown(env, db, usr, pwd,
                     OCI_SESSION_SYSDBA,
                     OCI_DB_SDM_FULL,     /* shutdown mode          */
                     OCI_DB_SDF_ABORT);   /* shutdown flags         */
```

```cpp
Database::Startup(env, db, usr, pwd,
                  Environment::SessionSysDba,
                  Database::StartupFull,      /* startup mode  */
                  Database::StartupForce);    /* startup flags */

Database::Shutdown(env, db, usr, pwd,
                   Environment::SessionSysDba,
                   Database::ShutdownFull,    /* shutdown mode  */
                   Database::ShutdownAbort);  /* shutdown flags */
```
:::

## Modes & flags

| Stage | C enum | C++ enum |
|-------|--------|----------|
| Startup modes | `OCI_DB_SPM_*` | `Database::Startup*` — `StartupStart`(nomount), `StartupMount`, `StartupOpen`, `StartupFull`(all) |
| Startup flags | `OCI_DB_SPF_*` | `Database::StartupDefault`, `StartupForce`, `StartupRestrict` |
| Shutdown modes | `OCI_DB_SDM_*` | `Database::ShutdownFull` (clean cycle) |
| Shutdown flags | `OCI_DB_SDF_*` | `Database::ShutdownDefault`, `ShutdownTransactional`, `ShutdownImmediate`, `ShutdownAbort` |

A normal shutdown sequence is `CLOSE → DISMOUNT → SHUTDOWN` — `FULL` does all of it;
finer modes run stages individually.

## When you need it

- DBA tooling, instance management utilities, test rigs that bounce a DB.
- Requires connecting as `sys`/`system` with `OCI_SESSION_SYSDBA` (or
  `OCI_SESSION_PRELIM_AUTH` + sysdba for the startup-before-connect pattern).
- Instance inspection meanwhile is for everyone: `OCI_ConnectionGetInstanceName`,
  `OCI_ConnectionGetInstanceStartTime`, `OCI_ConnectionGetServerVersionString` (page 120).

!!! warning "You can really shut down production"
    `OCI_DB_SDF_ABORT` kills sessions mid-flight. Gate this behind admin workflows.

## Putting it together — the typical sequence

DBA work in OCILIB is privileged-connection + specific calls:

1. **Connect as SYSDBA/SYSOPER** — the session mode flag at connect.
2. **Startup/shutdown** — `OCI_DatabaseStartup`, `OCI_DatabaseShutdown` with
   the mode flags.
3. **Instance queries** — `OCI_InstanceStartup` for the instance-level
   control.
4. **Environment checks** — `OCI_Get*` version and status calls for
   pre-flight.

Hints: `SYSDBA` connections are privileged — guard them like root;
startup/shutdown need the right mode flags — `OCI_DB_SPF_START` selects the
parameter file; and admin connections don't pool — create them ad-hoc.

## Try it yourself

1. On a disposable XE: `Shutdown(IMMEDIATE)` then `Startup(FULL)` and reconnect.
2. Start in `RESTRICT` mode and check `V$INSTANCE.LOGINS`.

## Recap

- `DatabaseStartup`/`Shutdown` = programmatic SQL*Plus admin, SYSDBA-only.

**Next:** `OCI_HashTable` — a handy public helper container.
