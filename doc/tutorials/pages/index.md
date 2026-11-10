---
title: Welcome
---

# OCILIB Tutorials

**OCILIB** is a free, open-source C and C++ driver for Oracle Database.

It is proven, mature technology — maintained by Vincent Rogier for nearly 20 years and distributed under the Apache License 2.0.

OCILIB wraps the low-level Oracle Call Interface (OCI) — the same C library that Oracle's own drivers use — in a clean, object-oriented API, while still exposing the full power of Oracle.

These tutorials take you from *"I just heard of OCILIB"* to *"I can use every part of
the API"*, step by step.

## Why OCILIB?

Raw OCI is powerful but famously painful: dozens of opaque handle types, attribute getters/setters that take `void*`, manual descriptor management, and pages of boilerplate for a simple `SELECT`.

OCILIB keeps the power and removes the pain:

- **One concept everywhere** — every Oracle resource (connection, statement, resultset,
  LOB, object, collection…) is an opaque `OCI_<Type>` handle with a `Create`/`Free`
  lifecycle in C, and an RAII class in C++.
- **Typed API** — no `OCIAttrGet`/`OCIAttrSet` generic calls; you call strongly-typed
  functions like `OCI_GetString(rs, 0)` or `OCI_BindDate(st, ":d", &date)`.
- **Full Oracle coverage** — scalar and LOB types, dates/timestamps/intervals, objects,
  collections, REFs, AQ messaging, change notification, direct path loading, JSON DOM,
  SODA document store, sharding, and more.
- **Cross-platform** — Windows, Linux and Unixes; 32/64-bit; ANSI or Unicode builds;
  Oracle client linked at compile time **or loaded at runtime** (no Oracle SDK needed
  to build).
- **Two APIs, one engine** — a C API for maximum portability and a header-only C++17
  API that adds RAII, exceptions, `std::optional` support and STL-style iterators.

## How OCILIB is designed

Understanding the shape of the library makes every tutorial page obvious:

```text
 your code
     │
 ┌───▼───────────────────────────┐
 │ C++ API  (header-only, C++17) │   optional layer — RAII wrappers,
 │   ocilib::Connection, …       │   exceptions, iterators
 ├───▼───────────────────────────┤
 │ C API    (ISO C11)            │   OCI_<Type> handles, OCI_<Type><Action>()
 │   OCI_Connection, …           │   error handler / OCI_GetLastError
 ├───▼───────────────────────────┤
 │ OCI      (Oracle client)      │   shipped by Oracle — Instant Client
 │   OCIServerAttach, …          │   or full client, linkage or runtime import
 └───▼───────────────────────────┘
 Oracle Database
```

Three design decisions explain everything else you will learn:

1. **The environment is the root object.** `OCI_EnvironmentCreate()` loads the Oracle
   client (it can even locate `oci.dll`/`libclntsh.so` at run time), installs *your*
   error handler, and owns every other handle. Create it once, free it last.
2. **Handles form an ownership tree.** A `Connection` owns its `Statement`s; a
   `Statement` owns its `Resultset`s and `Bind`s. Freeing a parent frees its children —
   and in C++, destructors do it all for you in reverse order.
3. **Errors are data, not exceptions — unless you want them to be.** In C, failures
   invoke your error callback with an `OCI_Error` (type, category, code, message) and
   are also available through `OCI_GetLastError()`. In C++, the same errors become
   `ocilib::Exception` thrown automatically.

## How the tutorials work

Every page is a self-contained lesson: objectives → concepts → a guided walkthrough →
a complete code example → short exercises → pitfalls → recap. Code is always shown in
**both C and C++** — click the tabs on any code card:

:::sample
```c
OCI_Connection *cn = OCI_ConnectionCreate(env, "localhost/XEPDB1", "scott", "tiger",
                                        OCI_SESSION_DEFAULT);
```

```cpp
ocilib::Connection con(env, "localhost/XEPDB1", "scott", "tiger");
```
:::

Your choice is remembered across all pages.

### Conventions used everywhere

- **Credentials** — samples read `OCILIB_DEMO_DB`, `OCILIB_DEMO_USR`, `OCILIB_DEMO_PWD`
  from the environment (same convention as the `demo/` programs). Set them once and
  every sample runs unmodified.
- **Schema** — the running project uses two tables, `departments` and `employees`.
  Create them once with [`assets/sql/tutorial-schema.sql`](assets/sql/tutorial-schema.sql).
- **Badges** — each page shows its difficulty (*Beginner / Intermediate / Advanced*),
  an *Oracle ≥ X* chip when a feature needs a minimum server version, and links to the
  related `demo/` programs.

## The learning path

| Part | What you will learn |
|------|---------------------|
| 0 — Getting started | Install Oracle client + OCILIB on Windows/Linux, first program, first connection, error handling, diagnostics logging |
| 1 — Connections & environments | Connection strings, session modes, ping/break, introspection |
| 2 — SQL, fetching & binding | Statements, resultsets, every getter, bind variables, scrollable cursors, bulk fetch |
| 3 — PL/SQL | Blocks, OUT parameters, ref cursors, index-by tables, `DBMS_OUTPUT` |
| 4 — Transactions, pooling & threads | Commit/rollback, transaction objects, connection/session pools, multithreading |
| 5 — Oracle data types | Charsets, dates, timestamps, intervals, numbers, RAW, vectors, LOBs, BFILEs, XML, JSON |
| 6 — Object-relational API | Type describe, objects, collections, REFs |
| 7 — Messaging & notifications | Advanced Queuing, publish/subscribe, change notification (CQN) |
| 8 — Bulk & administration | Array binding, direct path loading, database startup/shutdown |
| 9 — Modern Oracle features | Sharding, SODA document store |
| Appendices | C ↔ C++ API map, type cheat sheets, build flags, FAQ, demo index |

Ready? Continue to **[Your first connection](first-connection.html)** — or jump to any
topic in the sidebar.

!!! tip "Prerequisites"
    You need an Oracle database to connect to (a free
    [Oracle XE](https://www.oracle.com/database/technologies/appdev/xe.html) or a Docker
    image works fine) and the Oracle Instant Client. Detailed setup instructions for
    Windows and Linux are coming in Part 0.
