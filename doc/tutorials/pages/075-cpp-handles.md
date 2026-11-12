---
title: How the C++ API manages handles
navtitle: C++ handle management
part: 0
level: beginner
---

# How the C++ API manages handles

**Objectives** — by the end of this page you will be able to:

- explain what happens when you copy a `ocilib::Connection` (or any wrapper)
- tell owned handles from borrowed ones in the C++ API
- predict when the underlying `OCI_*` handle actually gets freed
- use `Close()` for early release without breaking the sharing model

## From manual frees to shared ownership

In the C API every handle follows the rule from the
[architecture page](architecture.html): whoever creates it frees it, exactly
once, before its parent goes away. The C++ API keeps the same ownership tree
but takes the bookkeeping off your hands — there is no `Free()` to call, and
leaving scope is enough:

```cpp
{
    ocilib::Environment env;
    ocilib::Connection  con(env, db, usr, pwd);
    ocilib::Statement   stmt(con);
}   // stmt's OCI_Statement, con's OCI_Connection and env's OCI_Environment
    // are freed here — in reverse construction order, for free
```

What makes it work is that each wrapper does not own the handle directly — it
owns a *share* of a small control block that knows how to free it.

## The layers under every C++ object

```
ocilib::Connection ─┐
ocilib::Connection ─┼─ shared_ptr ──► HandleControl ──► OCI_Connection*
   (copies)         │                    │  raw OCI handle (atomic)
                    │                    │  owned or borrowed flag
                    │                    │  parent / children links
                    │                    │  free-notification hook
                    └─ the last shared_ptr out triggers the free
```

- **`HandleHolder<T>`** — what every public class (`Connection`, `Statement`,
  `Lob`, `Date`, …) contains: a `std::shared_ptr` to a control block.
- **`HandleControl<T>`** — the control block: the atomic raw `OCI_*` handle, an
  *owned/borrowed* flag, parent/child links, and an optional free-notification
  hook. Its destructor releases the Oracle handle (through the matching
  `OCI_*Free` call), removes it from the handle store, unlinks it from its
  parent — and frees live children first.

**Copying a C++ object copies the share, not the handle.** Two `Connection`
variables hold one `OCI_Connection`; the handle is freed when the *last* copy
is released — which is why passing wrappers by value to functions is safe.

## Owned vs borrowed handles

Not every wrapper frees its handle — the control block records whether the C++
side *created* the object or merely *observed* it:

| Kind | Created when | Freed by the wrapper? |
|------|--------------|------------------------|
| **Owned** (`AcquireAllocated`) | the wrapper ran `OCI_*Create`/`OCI_*Get`-style construction — `Connection(env, …)`, `Statement(con)` | yes — on last release |
| **Borrowed** (`AcquireTransient`) | the handle came *from* another object — `Column` of a resultset, `TypeInfo` from a describe, `Resultset` from a statement, or a constructor taking a raw `OCI_*` pointer | no — its owner manages it |

Borrowed handles mirror the C rule "don't free what you didn't create" — the
wrapper stays valid only while its owner lives.

## The handle store — one control block per OCI handle

Each `ocilib::Environment` keeps a registry (the `HandleStore`) mapping raw
`OCI_*` handles to their control blocks. When the C++ layer wraps a handle it
consults the store first:

- **already wrapped?** → the existing control block is *shared*, never
  duplicated. If `OCI_StatementGetConnection` returns the connection you
  already hold as a `ocilib::Connection`, `stmt.GetConnection()` gives you a
  wrapper sharing *that same* control block — not a second owner that would
  double-free it.
- **new?** → a fresh control block is created, registered, and linked under its
  parent.

!!! warning "Environments must come from the C++ API"
    Deduplication requires the `OCI_Environment` to have been created through
    `ocilib::Environment` — the C++ object registers itself so any raw handle
    can find its store. Wrapping handles that belong to a *C-created*
    environment (`OCI_EnvironmentCreate`) is unsupported and can double-free.

## The parent/child tree

The C ownership tree exists in C++ too — every control block links to its
parent and tracks live children (Environment → Connection → Statement →
Resultset):

- **cascade on free** — freeing a parent control block frees its children that
  are still alive: closing a `Connection` releases the statements, resultsets
  and handles built on it, in the right order.
- **navigation** — the parent links are how `Statement::GetConnection()` or
  `Resultset::GetStatement()` reach upward without owning what they find.
- **children are weak references** — a parent does not extend a child's
  lifetime; if you drop every wrapper of a statement, it is freed even though
  its connection outlives it.

Freeing is **idempotent and thread-safe**: the raw handle is swapped out
atomically, so the first `Free()`/`Close()` wins and racing wrappers see a
no-op — no double-free even under `OCI_ENV_THREADED`.

## Explicit release — `Close()` and friends

Automatic release happens at scope exit, but Oracle-side resources are real
(server sessions, cursors, latches). To give one up *early*, call the wrapper's
explicit close — it drops that wrapper's share; the actual OCI free still waits
for the last sharer:

```cpp
ocilib::Connection con(env, db, usr, pwd);
ocilib::Connection alsoCon = con;   // same OCI_Connection

con.Close();                        // drops con's share — still open!
alsoCon.Close();                    // last share out → OCI_ConnectionFree now
```

`Close()` exists on `Connection`, `Lob`, `File`, `Pool`, …; a closed wrapper
reads as false in `if (wrapper)` and can be re-`Open`ed/re-acquired.

## Try it yourself

1. Copy a `Connection` into a second variable, `Close()` the first — run a
   statement through the second: still connected, because the last sharer
   holds the handle.
2. Call `stmt.GetConnection()` twice and compare with `==` — same underlying
   control block, courtesy of the handle store.
3. Let a `Resultset` outlive… nothing — observe it becomes unusable after its
   `Statement` is released: borrowed handles don't own their parents.

## Common pitfalls

- **"Copies are deep"** — they aren't: `ocilib::Connection b = a` shares the
  session; `b`'s use affects `a` (same transactions, same state).
- **Wrapping a C-made environment's handles** — the store only exists under
  `ocilib::Environment`; mixed C/C++ ownership is unsupported.
- **Keeping children after freeing the parent** — the cascade frees them for
  you, but the wrapper objects then hold closed handles; check with
  `if (wrapper)` before reuse.
- **Expecting `Close()` to close now** — it releases *your* share; other
  copies keep the handle alive by design.

## Recap

- C++ wrappers share a control block; the last `shared_ptr` out frees the OCI
  handle — copies share, never duplicate.
- Owned handles are freed on release; borrowed ones (getters, raw-pointer
  constructors) are never freed by the wrapper.
- The environment's handle store deduplicates wrappers — one control block per
  raw handle — and requires `ocilib::Environment`-created environments.
- Freeing cascades down the parent/child tree; `Close()` releases your share
  early, safely, thread-safe.

**Next:** when things go wrong — the [error handling page](error-handling.html).
