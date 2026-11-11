---
title: Your first connection
part: 0
level: beginner
demos: [conn.c]
---

# Your first connection

**Objectives** — by the end of this page you will be able to:

- explain the role of the environment and the connection
- create and free both objects in the right order
- write a working connection string
- compile and run a program that prints the Oracle server version

## Concepts

Every OCILIB program follows the same skeleton:

```
Environment  ── owns the loaded Oracle client, the error handler, all handles
   └── Connection ── one database session
          └── Statement ── SQL execution (next pages)
```

- The **environment** is the root object (previous page). Creating it locates and
  loads the Oracle client library (`oci.dll` / `libclntsh.so`), installs your **error
  handler callback**, and produces the handle every other object is attached to.

## The connection

A **connection** represents one database *session* — your authenticated context on
the server. While it's open, it carries real server-side state:

- **who you are** — the authenticated user and session attributes (`V$SESSION` sees it);
- **your transaction** — every statement you run through it is part of one in-flight
  transaction until you commit or rollback;
- **client-side caches and settings** — the statement cache, LOB prefetch size,
  trace fields, timeouts (all tunable — page 120).

You create one from the environment plus a connect string, credentials and a session
mode — `Default` for a normal user:

:::sample
```c
OCI_Connection *cn =
    OCI_ConnectionCreate(env, db, usr, pwd, OCI_SESSION_DEFAULT);
```

```cpp
ocilib::Connection con(env, db, usr, pwd);   /* default session mode */
```
:::

Typical usage: **one connection per logical session** — per user login, per worker thread, or per checkout from a pool (Part 5).

It stays open as long as you need it; closing it (freeing the handle) ends the session and rolls back anything uncommitted.

Freeing is explicit in C (`OCI_ConnectionFree`, then the environment's `Free`);
in C++ both are RAII objects — destruction does the work, and errors are thrown as
`ocilib::Exception` instead of reported through a callback.

### Connection strings

OCILIB accepts all the usual Oracle forms:

| Form | Example |
|------|---------|
| Easy Connect | `localhost:1521/XEPDB1` |
| TNS alias | `MYDB` (resolved via `tnsnames.ora`) |
| Full descriptor | `(DESCRIPTION=(ADDRESS=…)(CONNECT_DATA=…))` |

## Walkthrough

1. Define an **error handler** — a function OCILIB calls whenever a call fails. Here it
   just prints the message.
2. Create the environment with `OCI_ENV_DEFAULT` (single-threaded program).
3. Read credentials from `OCILIB_DEMO_*` environment variables and create the
   connection.
4. Print the server version to prove the session is alive, then free in reverse order.

## Full example

:::sample
```c
#include "ocilib.h"
#include <stdio.h>
#include <stdlib.h>

void err_handler(OCI_Error *err)
{
    printf("%s\n", OCI_ErrorGetString(err));
}

int main(void)
{
    OCI_Environment *env = OCI_EnvironmentCreate(err_handler, NULL, OCI_ENV_DEFAULT);
    if (!env)
    {
        return EXIT_FAILURE;
    }

    const char *db  = getenv("OCILIB_DEMO_DB");   /* e.g. localhost:1521/XEPDB1 */
    const char *usr = getenv("OCILIB_DEMO_USR");  /* e.g. scott */
    const char *pwd = getenv("OCILIB_DEMO_PWD");  /* e.g. tiger */

    OCI_Connection *cn = OCI_ConnectionCreate(env, db, usr, pwd, OCI_SESSION_DEFAULT);

    printf("Server major    version : %i\n", OCI_ConnectionGetServerMajorVersion(cn));
    printf("Connection      version : %u\n", OCI_ConnectionGetVersion(cn));

    OCI_ConnectionFree(cn);
    OCI_EnvironmentFree(env);

    return EXIT_SUCCESS;
}
```

```cpp
#include "ocilib.hpp"
#include <cstdlib>
#include <iostream>

using namespace ocilib;

int main()
{
    try
    {
        Environment env;

        const char *db  = getenv("OCILIB_DEMO_DB");
        const char *usr = getenv("OCILIB_DEMO_USR");
        const char *pwd = getenv("OCILIB_DEMO_PWD");

        Connection con(env, db, usr, pwd);

        std::cout << "Server version        : " << con.GetServerVersion() << "\n";   /* string "23.x.y" */
        std::cout << "Server major version  : " << con.GetServerMajorVersion() << "\n";
    }
    catch (const std::exception &ex)
    {
        std::cout << ex.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
```
:::

```output
Server major    version : 23
Connection      version : 2300
```

!!! note "v5 naming"
    `OCI_EnvironmentCreate()` / `OCI_EnvironmentFree()` replace the older
    `OCI_Initialize()` / `OCI_Cleanup()` from OCILIB 4.x. If you have used an earlier
    version, see the migration guide linked from the Appendices.

## Putting it together — the typical sequence

Connecting is the first real call — and the first place failures surface:

1. **Have the three pieces ready**: db string (`//host:port/service`), user,
   password — `OCILIB_DEMO_*` env vars in samples.
2. **Call `OCI_ConnectionCreate`** — it opens a session, authenticates, and
   hands you the `OCI_Connection` everything else hangs off.
3. **Check the result** — NULL means refused: bad service name, wrong
   credentials, unreachable host. `OCI_GetLastError` tells you which.
4. **Free it when done** — `OCI_ConnectionFree` drops the session.

Hints: the connection handle is the *parent* of statements, transactions,
queues — freeing it while children are alive is the classic leak; a refused
connect is not an exception in C — check the NULL, don't rely on the handler
firing; and `//host:port/service` is EZCONNECT — no `tnsnames.ora` needed.

## Try it yourself

1. Add `OCI_ConnectionPing(cn)` after connecting and print its result — it verifies the session
   round-trips to the server.
2. Set `OCILIB_DEMO_PWD` to a wrong value and observe what your error handler prints.
3. Try the three connection-string forms if you have a `tnsnames.ora` alias configured.

## Common pitfalls

- **`ORA-12154: could not resolve the connect identifier`** — the connection string is
  wrong or the Instant Client cannot find `tnsnames.ora`. Prefer Easy Connect
  (`host:port/service`) while learning.
- **Crash in `OCI_EnvironmentCreate`** — the Oracle client library is not on
  `PATH` (Windows) or `LD_LIBRARY_PATH` (Linux), or the bitness (32/64) does not match
  your executable.
- **Freeing in the wrong order** — always free children before parents:
  `OCI_ConnectionFree` then `OCI_EnvironmentFree`. The C++ API does it for you via
  destructors.

## Recap

- One environment per program; it loads OCI and owns the error handler.
- A connection is created from env + connect string + credentials + session mode.
- C frees handles manually; C++ uses RAII and throws `ocilib::Exception`.

**Next:** error handling in depth — severity, categories, error fields, and how
the C++ `Exception` mirrors them.
