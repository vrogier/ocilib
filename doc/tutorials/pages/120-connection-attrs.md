---
title: Connection attributes, traces & statement cache
navtitle: Connection attributes
part: 1
level: intermediate
---

# Connection attributes, traces & statement cache

**Objectives** — by the end of this page you will be able to:

- tag sessions with end-to-end tracing attributes (module/action/client id)
- read server-side connection properties
- enable and size the client-side statement cache
- attach user data and TAF/HA handlers to a connection

## What a connection carries

An open connection is more than a pipe to the server — it's an object *carrying
state* in two flavors:

- **identity** — read-only answers about what you're attached to: which database,
  which instance, which service, what server version, which session mode was used;
- **tunables** — behavior knobs you set: trace fields for observability, the
  statement cache size for performance, network timeouts, LOB prefetch, and
  failover/event callbacks for HA setups.

This page walks that surface — attributes first, then the knobs worth turning.

## Session traces — see yourself in `V$SESSION`

Oracle records client attributes on every session so DBAs can see *which part of your
application* is running each statement. A connection exposes five trace fields, each
mapping to a `V$SESSION` column:

| C enum | C++ enum | `V$SESSION` column | Max size |
|--------|----------|--------------------|----------|
| `OCI_TRC_IDENTITY` | `Connection::TraceIdentity` | `CLIENT_IDENTIFIER` | 64 |
| `OCI_TRC_MODULE` | `Connection::TraceModule` | `MODULE` | 48 |
| `OCI_TRC_ACTION` | `Connection::TraceAction` | `ACTION` | 32 |
| `OCI_TRC_DETAIL` | `Connection::TraceDetail` | `CLIENT_INFO` | 64 |
| `OCI_TRC_OPERATION` | — | `V$SQL_MONITOR.DBOP_NAME` | 32 |

Tagging a session is a set of `Trace` calls on the connection:

:::sample
```c
OCI_ConnectionSetTrace(cn, OCI_TRC_MODULE,   OTEXT("empdir"));
OCI_ConnectionSetTrace(cn, OCI_TRC_ACTION,   OTEXT("list-employees"));
OCI_ConnectionSetTrace(cn, OCI_TRC_IDENTITY, OTEXT("vince"));
/* now:  select module, action, client_identifier from v$session;  */
```

```cpp
con.SetTrace(ocilib::Connection::TraceModule,   "empdir");
con.SetTrace(ocilib::Connection::TraceAction,   "list-employees");
con.SetTrace(ocilib::Connection::TraceIdentity, "vince");
```
:::

Set `MODULE`/`ACTION` around logical operations — it makes `V$SQL` and AWR reports
infinitely more readable.

## Server-side properties

The connection answers what you're attached to — database, instance, service,
limits and timeouts:

| C function | C++ method | Returns |
|------------|------------|---------|
| `OCI_ConnectionGetDatabaseName` | `GetDbName()` | database name |
| `OCI_ConnectionGetConnectionString` | `GetDbName()` | database/service used at connect |
| `OCI_ConnectionGetInstanceName` | `GetInstanceName()` | instance name |
| `OCI_ConnectionGetInstanceStartTime` | `GetInstanceStartTime()` | instance start time (`Timestamp` in C++) |
| `OCI_ConnectionGetServiceName` | `GetService()` | service name |
| `OCI_ConnectionGetDomainName` | `GetDomain()` | domain |
| `OCI_ConnectionGetServerName` | `GetServerName()` | server host |
| `OCI_ConnectionGetSessionMode` | — | the `OCI_SESSION_*` mode used at connect |
| `OCI_ConnectionGetServerVersionString` | `GetServerVersion()` | server version string (`"23.5.0.0"`) |
| `OCI_ConnectionGetMaxCursors` | `GetMaxCursors()` | `open_cursors` limit |
| `OCI_ConnectionGetTimeout` | `GetTimeout(TimeoutType)` | network/call timeout |
| `OCI_ConnectionSetTimeout` | `SetTimeout(TimeoutType, ms)` | set a timeout |

Timeouts come in three kinds — the C++ enum maps them under `Connection::TimeoutType`:

| C enum | C++ enum | Covers |
|--------|----------|--------|
| `OCI_NTO_SEND` | `Connection::TimeoutSend` | `SQLNET.SEND_TIMEOUT` |
| `OCI_NTO_RECEIVE` | `Connection::TimeoutReceive` | `SQLNET.RECV_TIMEOUT` |
| `OCI_NTO_CALL` | `Connection::TimeoutCall` | OCI call timeout (overrides the others) |

## Statement cache

Re-preparing the same SQL costs a server round-trip. The client-side **statement
cache** keeps recently used statement handles warm:

:::sample
```c
OCI_ConnectionSetStatementCacheSize(cn, 50);          /* statements to keep */
printf("cache size: %u\n", OCI_ConnectionGetStatementCacheSize(cn));
```

```cpp
con.SetStatementCacheSize(50);
std::cout << "cache size: " << con.GetStatementCacheSize() << "\n";
```
:::

With a cache, freeing a statement may return the handle to the cache instead of destroying it — repeat executions of the same SQL skip the parse.

Big win on OLTP-style code that loops over a fixed set of queries.

## Other per-connection knobs

| C function | C++ method | Purpose |
|------------|------------|---------|
| `OCI_ConnectionSetDefaultLobPrefetchSize` | `SetDefaultLobPrefetchSize` | LOB bytes prefetched per row (Part 2) |
| `OCI_ConnectionGetDefaultLobPrefetchSize` | `GetDefaultLobPrefetchSize` | read the prefetch size |
| `OCI_HandleSetUserData` | `SetUserData` | attach a `void*`/`AnyPointer` to the connection |
| `OCI_HandleGetUserData` | `GetUserData()` | read it back |
| `OCI_ConnectionIsTAFCapable` | `IsTAFCapable()` | Transparent Application Failover supported? |
| `OCI_ConnectionSetTAFHandler` | `SetTAFHandler` | TAF failover callback (RAC) |
| `OCI_EnvironmentSetHAHandler` | `SetHAHandler` | High-Availability event callback — needs `OCI_ENV_EVENTS` / `Environment::Events` |
| `OCI_ConnectionGetAutoCommit` | `GetAutoCommit()` | read autocommit state |
| `OCI_ConnectionSetAutoCommit` | `SetAutoCommit` | enable/disable autocommit (page 410) |

## Putting it together — the typical sequence

Session attributes are mostly set-once-then-use:

1. **Connect first** — attributes live on an open connection, not the
   environment.
2. **Read what you got**: `GetServerVersion`, `GetDBName`, `GetSessionTag`,
   `GetStatementCacheSize` — the diagnostics calls that answer "what am I
   actually talking to?".
3. **Set what the server should know**: `SetSessionTag` (for DRP/FCF routing),
   `SetDBA` mode if you're admin, statement-cache size for hot loops.
4. **Use them**: a bigger statement cache pays off on repeated `Prepare` of the
   same SQL; a session tag lets a DBA trace *your* app in `v$session`.

Hints: `GetVersion` (client) and `GetServerVersion` (remote) answer different
questions — mismatch them and you'll chase phantom bugs; statement caching is
free performance on every prepare-heavy path — raise it before profiling.

## Try it yourself

1. Set `MODULE`/`ACTION`, run a query, and find your session in
   `select module, action from v$session where username = USER`.
2. Time 1000 prepare+execute cycles of the same statement with cache size 0 vs 50.
3. Print the max-cursors limit and set a 5 s call timeout, then watch a long query
   abort on time.

## Common pitfalls

- **Trace strings silently truncated** — respect the byte limits above.
- **Cache "leaks"** — a large cache holds statement handles open; size it to your
  working set, not to "a lot".
- **Reading session attrs on a dead connection** — check the connection state first
  (next page).
- **`OCI_TRC_OPERATION` has no C++ enum** — the C++ `Trace` values cover the four
  `V$SESSION` fields; use the C call for the operation name.

## Recap

- Trace fields feed `V$SESSION` — invaluable for debugging and DBA relations.
- Server-side properties describe database, instance, service, cursors, timeouts.
- The statement cache turns repeated parses into cache hits.

**Next:** liveness — ping and break.
