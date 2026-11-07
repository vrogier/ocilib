---
title: Anatomy of an OCILIB program
navtitle: Anatomy of a program
part: 0
level: beginner
---

# Anatomy of an OCILIB program

**Objectives** — by the end of this page you will be able to:

- recognize the handle types OCILIB exposes and how they relate
- predict the lifecycle (Create → use → Free) of any OCILIB object
- tell owned handles from borrowed (transient) handles
- map a C API call to its C++ equivalent at a glance

## The handle model

OCILIB exposes every Oracle resource as an **opaque handle**: a `struct` you cannot see inside, always used through a pointer.

The naming convention is uniform — once you know it, you can guess most of the API:

| Pattern | Meaning | Example |
|---------|---------|---------|
| `OCI_<Type>` | handle type | `OCI_Connection`, `OCI_Statement` |
| `OCI_<Type>Create…()` | constructor | `OCI_ConnectionCreate()` |
| `OCI_<Type>Free()` | destructor | `OCI_ConnectionFree()` |
| `OCI_<Type><Action>()` | method | `OCI_StatementSetFetchSize()` |
| `OCI_<Type>Get/Set<Prop>()` | property | `OCI_LobGetLength()` |
| `OCI_<PREFIX>_*` | constant | `OCI_SESSION_DEFAULT`, `OCI_CLOB` |

The C++ API mirrors this 1:1 — `OCI_ConnectionCreate` becomes
`ocilib::Connection(env, db, usr, pwd)`, `OCI_FetchNext` becomes `rs.Next()`, and so on.

## The ownership tree

Handles are not independent — they form a tree rooted at the **environment**:

```text
OCI_Environment                    ← OCI_EnvironmentCreate / OCI_EnvironmentFree
└── OCI_Connection                 ← OCI_ConnectionCreate / OCI_ConnectionFree
    ├── OCI_Statement              ← OCI_StatementCreate / OCI_StatementFree
    │   ├── OCI_Resultset          ← OCI_GetResultset      (borrowed — never freed)
    │   └── OCI_Bind               ← OCI_BindXxx           (owned by statement)
    ├── OCI_Transaction            ← OCI_TransactionCreate / Free
    └── …
└── OCI_Pool                       ← OCI_PoolCreate / OCI_PoolFree
    └── OCI_Connection             ← OCI_PoolGetConnection  (borrowed — released, not freed)
```

Three rules cover every case:

1. **Create/Free pairs.** Anything you `Create`, you must `Free` — before freeing its
   parent. Children die with their parent anyway, so the safe order is always
   *leaf → root*.
2. **Borrowed handles are never freed.** `OCI_GetResultset()`, `OCI_GetColumn()`,
   `OCI_PoolGetConnection()` hand you objects owned by someone else. Use them, don't
   free them. In C++ these are acquired *transiently* and release their parent
   automatically.
3. **The C++ layer removes rule 1 entirely.** `ocilib::Connection con(env, …)` frees
   itself when it goes out of scope — and exceptions unwind everything correctly.

## Errors: the fourth leg

Every call that can fail either returns `FALSE`/NULL (C) or throws
`ocilib::Exception` (C++). In C, two channels deliver the details:

- your **error handler callback** (installed at `OCI_EnvironmentCreate`), invoked
  immediately with a filled `OCI_Error`;
- `OCI_GetLastError()` — poll the last error of the calling thread any time.

## The two build flavors

| Flavor | Compile define | Meaning |
|--------|----------------|---------|
| ANSI | `OCI_CHARSET_ANSI` | `otext` = `char`, strings are bytes |
| Unicode | `OCI_CHARSET_WIDE` | `otext` = `unsigned short` (UTF-16) |

| Import | Compile define | Meaning |
|--------|----------------|---------|
| Linkage | `OCI_IMPORT_LINKAGE` | link against `oci.lib`/`libclntsh` at build time |
| Runtime | `OCI_IMPORT_RUNTIME` | OCILIB `dlopen`s the Oracle client itself — no Oracle SDK needed to build |

Both pairs are chosen when **OCILIB** is built; your application code stays identical
thanks to the `otext`/`OTEXT()` abstraction you will see in every sample.

## Try it yourself

1. Open `include/ocilibc/api.h` and pick any type (e.g. `OCI_Date`). Count how many
   of its functions you can *predict* from the naming rules above.
2. In `include/ocilibcpp/types.hpp`, find the C++ class matching your chosen type.

## Recap

- Handles are opaque, named `OCI_<Type>`, with uniform Create/Free/Get/Set naming.
- Ownership is a tree rooted at `OCI_Environment`; borrowed handles are never freed.
- C returns `FALSE`/NULL + error callback; C++ throws `ocilib::Exception`.
- ANSI/Unicode and linkage/runtime import are build-time choices; your code does not change.

**Next:** get the two things every sample needs — an Oracle database and the Instant Client.
