---
title: Connection strings & session modes
navtitle: Connection strings
part: 1
level: beginner
demos: [conn.c]
---

# Connection strings & session modes

**Objectives** — by the end of this page you will be able to:

- pick the right connect-string form for your database
- choose a session mode (ordinary or privileged)
- read back attributes that describe what you connected to

## Connect-string forms

The connect string answers one question: **where is the database?** The client needs a host, a port and a service identity — and Oracle has offered several syntaxes for that over the years.

Creating a connection takes that string plus a user name, a password and a session mode:

:::sample
```c
OCI_Connection *cn = OCI_ConnectionCreate(env, db, user, pwd, mode);
```

```cpp
ocilib::Connection cn(env, db, user, pwd, mode);
```
:::

| Form | Example | Notes |
|------|---------|-------|
| TNS alias | `prod` | needs `tnsnames.ora` on `TNS_ADMIN`/`ORACLE_HOME` |
| Easy Connect | `dbhost:1521/XEPDB1` | no config file needed — best for getting started |
| Easy Connect + params | `dbhost:1521/XEPDB1?connect_timeout=5` | timeouts, retries |
| Full descriptor | `(DESCRIPTION=(ADDRESS=…)…)` | RAC/scan lists, load balancing |
| Empty / local | `""` | `ORACLE_SID`-style local bequeath |

## Session modes

The last argument selects the session's **privilege level** — which Oracle role the connection opens with.

Normal work uses the default; admin tasks (page 820) need privileged modes:

| C enum | C++ enum | Meaning |
|--------|----------|---------|
| `OCI_SESSION_DEFAULT` | `Environment::SessionDefault` | ordinary session |
| `OCI_SESSION_SYSDBA` | `Environment::SessionSysDba` | DBA ops incl. database startup/shutdown |
| `OCI_SESSION_SYSOPER` | `Environment::SessionSysOper` | limited admin |
| `OCI_SESSION_SYSASM` | `Environment::SessionSysAsm` | ASM instance |
| `OCI_SESSION_SYSBKP` | `Environment::SessionSysBkp` | backup role |
| `OCI_SESSION_SYSDGD` | `Environment::SessionSysDgD` | Data Guard role |
| `OCI_SESSION_SYSKMT` | `Environment::SessionSysKmt` | key management |
| `OCI_SESSION_SYSRAC` | `Environment::SessionSysRac` | RAC role |
| `OCI_SESSION_PRELIM_AUTH` | `Environment::SessionPreAuth` | preliminary auth for `STARTUP` |

## What did you just connect to?

The connection answers that — a quick sanity check worth doing once:

:::sample
```c
printf("connected as %s to %s (server %s)\n",
       OCI_ConnectionGetUserName(cn), OCI_ConnectionGetConnectionString(cn), OCI_ConnectionGetServerVersionString(cn));
```

```cpp
std::cout << "connected as " << con.GetUserName() << " to " << con.GetDbName()
          << " (server " << con.GetServerVersion() << ")\n";
```
:::

The connection exposes a much larger attribute surface — service/instance details, trace fields, the statement cache, timeouts and TAF/HA hooks.

That's the subject of the next page.

## Putting it together — the typical sequence

Connecting is a two-line operation, but the decisions happen before the call:

1. **Pick the addressing form**: `//host:port/service` (EZCONNECT, the default),
   `//host:port/service:dedicated`, a full TNS alias, or a descriptor string —
   the syntax decides *how* Oracle locates the instance, not what OCILIB does
   with it.
2. **Choose the connect flavor**: plain `Connection` for normal work, pooled
   `PoolGetConnection` for repeated connects, `SYSDBA`/`SYSOPER` only for admin
   pages.
3. **Pass the credentials** — `OCILIB_DEMO_*` env vars in the samples, real ones
   in your code.
4. **Check the result** — `OCI_ConnectionCreate` returns a handle or NULL;
   `OCI_GetLastError` explains a refused connect (bad service, bad credentials,
   unreachable host).

Hints: the *service* name is not the SID — `/ORCL` and `//host/orcl` mean
different things on misconfigured servers; connection strings with `@` and `//`
are the most-quoted source of "connects in SQL*Plus but not here"; and a pooled
handle is returned by `PoolGetConnection`, not `Connection` — don't mix the
lifecycles.

## Try it yourself

1. Connect with an Easy Connect string including `connect_timeout`, print the
   resolved `GetConnectionString`.
2. On an XE sandbox, connect `SYS … as SYSDBA` with the SYSDBA session mode and
   print `GetServiceName`/`GetInstanceName`.
3. Set module+action trace and find them in `v$session` columns `module`/`action`.

## Common pitfalls

- **Service vs SID** — `dbhost/XEPDB1` is a *service*; SIDs need
  `dbhost:1521:SID`-style descriptors.
- **Privileged mode on a normal account** — `SYSDBA` modes fail unless the user
  actually holds them.
- **Assuming default = SYSDBA** — admin paths explicitly opt into privileged modes;
  everyday connections should stay `DEFAULT`.

## Recap

- Connect string forms: TNS alias, Easy Connect (+params), full descriptor, local.
- Session modes select privileges — C `OCI_SESSION_*` ↔ C++ `Environment::Session*`.
- Connection attributes read back login, server version, service, trace info.

**Next:** liveness — ping and break.
