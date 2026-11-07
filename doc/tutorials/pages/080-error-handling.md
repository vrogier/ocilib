---
title: Error handling in depth
part: 0
level: beginner
demos: [err.c, err_ctx.c, err_warning.c]
---

# Error handling in depth

**Objectives** — by the end of this page you will be able to:

- read every field of an error object (severity, category, code, source)
- choose between callback-driven and polling error handling
- enable Oracle warnings
- map the C error model onto C++ exceptions

## Meet the error object

The **error** is different from every other handle in this tutorial: *you* never create or free one.

When a call fails, the library builds a small record describing exactly what went wrong and hands it to you through one of two channels — your error callback, or the thread's "last error" slot.

It's transient: read it, extract what you need, move on — the next failure replaces it.

In v5 every error answers three questions — how bad is it, who raised it, and which
code identifies it — plus context about *where* it occurred.

**Severity** — the error's type:

| C enum | C++ enum | Meaning |
|--------|----------|---------|
| `OCI_ERR_TYPE_ERROR` | `Exception::Error` | failure — the call did not complete |
| `OCI_ERR_TYPE_WARNING` | `Exception::Warning` | non-fatal Oracle notice |

**Category** — which layer reported it:

| C enum | C++ enum | Meaning |
|--------|----------|---------|
| `OCI_ERR_CAT_LIB` | `Exception::OcilibError` | OCILIB itself (null handle, bad arg, unsupported feature) |
| `OCI_ERR_CAT_OCI` | `Exception::OracleError` | the Oracle client/server (`ORA-xxxxx`) |
| `OCI_ERR_CAT_JZN` | `Exception::JsonError` | the JSON DOM engine (`JZN-xxxxx`) |

**Fields** — everything the error object carries:

| C function | C++ method | Returns |
|------------|------------|---------|
| `OCI_ErrorGetType` | `GetType()` | severity enum above |
| `OCI_ErrorGetCategory` | `GetCategory()` | category enum above |
| `OCI_ErrorGetCode` | `GetCode()` | `ORA-xxxxx` or OCILIB internal code |
| `OCI_ErrorGetString` | `GetMessage()` | formatted message text |
| `OCI_ErrorGetConnection` | `GetConnection()` | handle involved (may be empty) |
| `OCI_ErrorGetStatement` | `GetStatement()` | statement involved (may be empty) |
| `OCI_ErrorGetRow` | `GetRow()` | batch/array row index when relevant |
| `OCI_ErrorGetLocation` | `GetLocation()` | the OCILIB function that raised it |
| — | `what()` | `std::exception` message (same text) |

## Two delivery channels

**1. The error handler** — the callback you registered when creating the environment.
It fires *immediately* inside the failing call, before control returns to you; you
can swap it at any time on the environment.

:::sample
```c
static void err_handler(OCI_Error *err)
{
    fprintf(stderr, "[%s/%s] %s in %s (code %u)\n",
        OCI_ErrorGetType(err)     == OCI_ERR_TYPE_WARNING ? "warn" : "err",
        OCI_ErrorGetCategory(err) == OCI_ERR_CAT_OCI      ? "oci"  : "lib",
        OCI_ErrorGetString(err), OCI_ErrorGetLocation(err),
        OCI_ErrorGetCode(err));
}
```

```cpp
/* no handler — a failed call throws instead (see below) */
```
:::

**2. Polling the last error** — every thread keeps a "last error" slot you can read
after a failed call, handy when no handler is registered or you want details at the call site.

In C++, both channels collapse into one: a failed call throws `ocilib::Exception` carrying all the fields above.

:::sample
```c
if (!OCI_Commit(cn))
{
    OCI_Error *err = OCI_GetLastError();
    fprintf(stderr, "commit failed: %s (code %u)\n",
            OCI_ErrorGetString(err), OCI_ErrorGetCode(err));
}
```

```cpp
try
{
    con.Commit();
}
catch (const ocilib::Exception &ex)
{
    std::cerr << "commit failed: " << ex.GetMessage()
              << " (code " << ex.GetCode() << ")\n";
    /* ex also has GetType(), GetCategory(), GetConnection(), GetStatement(), GetRow() */
}
```
:::

## Warnings

By default only errors are reported. Enabling warnings on the environment routes
Oracle *notices* (e.g. `ORA-24347` on aggregation in a rollback) through the same
handler, tagged with the warning severity so you can tell them apart:

:::sample
```c
OCI_EnableWarnings(env, TRUE);
```

```cpp
env.EnableWarnings(true);
```
:::

## C++ error handling

`ocilib::Exception` derives from `std::exception` — catch it specifically when you need the OCILIB fields (`GetType`, `GetCategory`, `GetRow`…); `std::exception` suffices otherwise.

There is no "last error" polling and no callback — the exception *is* the error object.

## Try it yourself

1. Force `ORA-00942` (`select * from nope`) and print **all** error fields.
2. Enable warnings, then run a query that produces one — compare its type to an error.
3. In C, drop the handler (pass NULL at environment creation) and rely solely on
   last-error polling.

## Common pitfalls

- **Testing the wrong return** — most calls return a boolean; getters report
  through the handler, not a magic return value.
- **Assuming last-error is global** — it is per-environment, per-thread
  (`OCI_ENV_THREADED` / `Environment::Threaded` gives each thread its own slot).
- **Letting a noisy handler hide logic** — for batch work, check returns at the call
  site; use the handler for logging only.

## Recap

- An error = severity (`Error`/`Warning`) × category (`Ocilib`/`Oracle`/`Json`) × code,
  plus message and source context.
- C: handler callback and/or `OCI_GetLastError()`. C++: `ocilib::Exception` carrying
  the same fields.

**Next:** watch every call — OCILIB's built-in logging subsystem.
