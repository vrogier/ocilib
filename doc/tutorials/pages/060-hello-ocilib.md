---
title: The environment — init, flags & error handler
navtitle: The environment
part: 0
level: beginner
---

# The environment — init, flags & error handler

**Objectives** — by the end of this page you will be able to:

- create and destroy the environment correctly
- write an error handler and understand when it fires
- pick the right environment flags for your application
- point OCILIB at a non-standard Oracle client location

## The environment

The **environment** is OCILIB's root object — the context in which everything else
exists. Creating it does three jobs at once:

- it **loads the Oracle client** (`oci.dll` / `libclntsh.so`) and prepares OCILIB's
  session with it;
- it **owns the error channel** — your error callback lives here, and every failing
  call on any child handle reports through it;
- it is the **parent of all other objects** — connections, pools, datatypes… every
  handle belongs to exactly one environment.

Think of it as "the library, initialized": nothing works until it exists, and freeing it ends the program's relationship with Oracle.

**Most programs need exactly one environment**, created at startup and freed at
exit — the object tree makes it naturally process-wide. v5 does allow several
independent environments in one process, but that is for specific cases (a
plugin needing its own error handler, a component needing different flags) and
handles never cross environment boundaries — the
[environments page](environments.html) in Part 1 covers when a second one is
worth it.

Creating it takes three inputs — your error callback, an optional path to the
Oracle client, and a set of flags:

:::sample
```c
OCI_Environment *OCI_EnvironmentCreate
(
    POCI_ERROR       handler,   /* your error callback (may be NULL) */
    const otext      *libpath,   /* Oracle client path (may be NULL) */
    unsigned int      mode       /* environment flags */
);
```

```cpp
ocilib::Environment env(mode);            /* flags + optional libpath ctor arg */
ocilib::Environment env(mode, libpath);
```
:::

- **`handler`** — a function of type `void (*)(OCI_Error *err)` called on every error.
  C++ has no callback: failures arrive as `ocilib::Exception`.
- **`libpath`** — directory or file where the Oracle client library lives; empty means
  "search the usual places" (`PATH`, `LD_LIBRARY_PATH`, registry on Windows).
- **`mode`** — a bit mask of environment flags:

| C enum | C++ enum | Meaning |
|--------|----------|---------|
| `OCI_ENV_DEFAULT` | `Environment::Default` | plain single-threaded app |
| `OCI_ENV_THREADED` | `Environment::Threaded` | app uses multiple threads (Part 5) |
| `OCI_ENV_EVENTS` | `Environment::Events` | subscriptions/HA events (Part 7) |

Destroying the environment releases everything — do it **once**, **last**, after
every child handle is gone. In C++ destruction is automatic; in C it's explicit:

:::sample
```c
OCI_EnvironmentFree(env);
```

```cpp
/* nothing — env's destructor runs at end of scope */
```
:::

## The error handler, minimal and realistic

The handler receives a filled error object — its fields are the subject of the next
page. For now, printing the message is plenty:

:::sample
```c
#include "ocilib.h"
#include <stdio.h>
#include <stdlib.h>

static void err_handler(OCI_Error *err)
{
    fprintf(stderr, "[ocilib] %s\n", OCI_ErrorGetString(err));
}

int main(void)
{
    OCI_Environment *env = OCI_EnvironmentCreate(err_handler, NULL,
                                                 OCI_ENV_DEFAULT);
    if (!env)
    {
        fprintf(stderr, "Cannot initialize OCILIB — is the Oracle client on PATH?\n");
        return EXIT_FAILURE;
    }

    printf("OCILIB environment ready.\n");

    OCI_EnvironmentFree(env);
    return EXIT_SUCCESS;
}
```

```cpp
#include "ocilib.hpp"
#include <iostream>

using namespace ocilib;

int main()
{
    try
    {
        Environment env(Environment::Default);   /* RAII — no free, no handler */
        std::cout << "OCILIB environment ready.\n";
    }
    catch (const std::exception &ex)
    {
        std::cerr << "Cannot initialize OCILIB: " << ex.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
```
:::

```output
OCILIB environment ready.
```

!!! note "C++ has no error callback"
    The C++ layer converts every failure into `ocilib::Exception` — that *is* the
    error channel, so the `Environment` takes only flags and an optional lib path.

## When does the handler fire?

On **every** failed public call — connection failures, SQL syntax errors, constraint violations, and even *warnings* once you enable them on the environment (next page shows the severity/category fields).

A handler that prints is great for learning; real apps usually log and let last-error polling carry details to the call site.

## Putting it together — the typical sequence

Every OCILIB program — hello-world or production — runs the same five steps:

1. **Create the environment** — `OCI_EnvironmentCreate(handler, NULL, flags)`
   or `Environment env;` in C++. Nothing works without it.
2. **Connect** — `OCI_ConnectionCreate` with db string, user, password.
3. **Do work** — statements, fetches, binds; this is where your code lives.
4. **Disconnect** — `OCI_ConnectionFree` releases the session.
5. **Free the environment** — last, after every child handle is gone.

Hints: order matters — environment first, free last, children in between;
the C++ `Environment` is RAII — the ctor/dtor bracket the same calls; and the
error handler you register at step 1 is what `OCI_GetLastError` consults
throughout — write it before you need it.

## Try it yourself

1. Pass a bogus `libpath` — observe the environment creation fail cleanly.
2. In C, pass `NULL` as the handler and poll the last error after a failed call:
   the same error is still retrievable.
3. Skim `demo/err.c`, `demo/err_ctx.c`, `demo/err_warning.c` — three complete
   error-handling demos matching this page.

## Common pitfalls

- **Calling anything before the environment exists** — every call fails until it does.
- **Two environments, one program** — legal since v5, but almost never needed:
  each owns its handles and mixing them fails fast. Keep one env unless you
  have one of the reasons listed on the [environments page](environments.html).
- **Forgetting the threaded flag** in a multithreaded app — undefined behavior; see
  the threading page in Part 5.

## Recap

- Create the environment first, free it last — `Environment` RAII in C++.
- Flags: `Default` / `Threaded` / `Events`; `libpath` only for exotic client layouts.
- Errors arrive through your C callback or as C++ exceptions — same information.

**Next:** put it to work — open your first connection.
