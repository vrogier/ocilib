---
title: Anatomy of an OCILIB program
navtitle: Anatomy of a program
part: 0
level: beginner
---

# Anatomy of an OCILIB program

**Objectives** — by the end of this page you will be able to:

- recognize the objects OCILIB exposes and how they relate
- predict the lifecycle (create → use → free) of any OCILIB object
- tell owned handles from borrowed (transient) handles
- map a C API call to its C++ equivalent at a glance

## The handle model

OCILIB exposes every Oracle resource as an **opaque handle**: a `struct` you cannot see inside, always used through a pointer.

The naming convention is uniform — once you know it, you can guess most of the API.
The C++ API mirrors it 1:1:

| C pattern | C++ equivalent | Role | Example |
|-----------|----------------|------|---------|
| `OCI_<Type>` | `ocilib::<Type>` | handle type / wrapper class | `OCI_Connection` ↔ `ocilib::Connection` |
| `OCI_<Type>Create…()` | `ocilib::<Type>(…)` | constructor | `OCI_ConnectionCreate(env, …)` ↔ `Connection cn(env, …)` |
| `OCI_<Type>Free()` | destructor — RAII | end of life | `OCI_ConnectionFree(cn)` ↔ end of `cn`'s scope |
| `OCI_<Type><Action>()` | `<obj>.<Action>()` | method | `OCI_StatementExecute(st, …)` ↔ `st.Execute(…)` |
| `OCI_<Type>Get/Set<Prop>()` | `<obj>.Get/Set<Prop>()` | property | `OCI_LobGetLength(lob)` ↔ `lob.GetLength()` |
| `OCI_<PREFIX>_*` | `<Type>::<Member>` | constant / enum member | `OCI_SESSION_SYSDBA` ↔ `Environment::SessionSysDba` |

## The ownership tree

Handles are not independent — they form a tree rooted at the **environment**:

```text
environment                        ← created once, freed last
├── connection                     ← one per database session
│   ├── statement                  ← one per SQL command
│   │   ├── resultset              ← borrowed — never freed
│   │   │   └── column             ← borrowed — never freed
│   │   └── bind                   ← owned by the statement
│   └── transaction                ← optional explicit unit of work
└── pool                           ← a group of reusable connections
    └── connection                 ← borrowed — returned to the pool
```

Each concept maps to one handle type in C and one wrapper class in C++ — the
complete inventory is listed at the bottom of this page:

| Concept | C handle type | C++ wrapper | Born by | Dies by |
|---------|---------------|-------------|---------|---------|
| environment | `OCI_Environment` | `ocilib::Environment` | `OCI_EnvironmentCreate` | `OCI_EnvironmentFree` |
| connection | `OCI_Connection` | `ocilib::Connection` | `OCI_ConnectionCreate` | `OCI_ConnectionFree` |
| statement | `OCI_Statement` | `ocilib::Statement` | `OCI_StatementCreate` | `OCI_StatementFree` |
| resultset | `OCI_Resultset` | `ocilib::Resultset` | `OCI_StatementGetResultset` | never — dies with its statement |
| column | `OCI_Column` | `ocilib::Column` | `OCI_ResultsetGetColumn` | never — dies with its statement |
| bind | `OCI_Bind` | `ocilib::BindInfo` | `OCI_StatementBindXxx` factories + `OCI_Bind*` methods | never — dies with its statement |
| transaction | `OCI_Transaction` | `ocilib::Transaction` | `OCI_TransactionCreate` | `OCI_TransactionFree` |
| pool | `OCI_Pool` | `ocilib::Pool` | `OCI_PoolCreate` | `OCI_PoolFree` |
| pooled connection | `OCI_Connection` | `ocilib::Connection` | `OCI_PoolGetConnection` | `OCI_ConnectionFree` — returns it to the pool |

Three rules cover every case:

1. **Create/free pairs.** Anything you create, you free — before freeing its
   parent. Children die with their parent anyway, so the safe order is always
   *leaf → root*.
2. **Borrowed handles are never freed.** Resultsets, columns and pooled
   connections are handed to you by the object that owns them. Use them, don't
   free them. In C++ these are acquired *transiently* and release their parent
   automatically.
3. **The C++ layer removes rule 1 entirely.** A `Connection` object frees its
   handle when it goes out of scope — and exceptions unwind everything correctly.

## Every handle type

The same pattern extends to every OCILIB object. Here is the complete inventory —
the parent is always the object that creates (or lends) the handle:

| C type | C++ wrapper | Parent |
|--------|-------------|--------|
| `OCI_Environment` | `ocilib::Environment` | — (root) |
| `OCI_Pool` | `ocilib::Pool` | environment |
| `OCI_Connection` | `ocilib::Connection` | environment — or a pool |
| `OCI_Transaction` | `ocilib::Transaction` | connection |
| `OCI_Statement` | `ocilib::Statement` | connection |
| `OCI_Resultset` | `ocilib::Resultset` | statement |
| `OCI_Column` | `ocilib::Column` | resultset |
| `OCI_Bind` | `ocilib::BindInfo` | statement |
| `OCI_Error` | `ocilib::Exception` | — (delivered to your error handler) |
| `OCI_Number` | `ocilib::Number` | environment |
| `OCI_Date` | `ocilib::Date` | environment |
| `OCI_Timestamp` | `ocilib::Timestamp` | environment |
| `OCI_Interval` | `ocilib::Interval` | environment |
| `OCI_Lob` | `ocilib::Lob` | connection — or statement when fetched |
| `OCI_File` | `ocilib::File` | connection — or statement when fetched |
| `OCI_Long` | `ocilib::Long` | statement |
| `OCI_Json` | `ocilib::Json` | connection — or statement when fetched |
| `OCI_JsonNode` | `ocilib::JsonNode` | a JSON document — or a parent node |
| `OCI_Vector` | `ocilib::Vector` | connection — or statement when fetched |
| `OCI_XmlType` | `ocilib::XmlType` | statement |
| `OCI_TypeInfo` | `ocilib::TypeInfo` | connection |
| `OCI_Object` | `ocilib::Object` | type info — or statement when fetched |
| `OCI_Collection` | `ocilib::Collection` | type info — or statement when fetched |
| `OCI_Element` | `ocilib::CollectionElement` | collection |
| `OCI_Iterator` | — | collection |
| `OCI_Reference` | `ocilib::Reference` | type info — or statement when fetched |
| `OCI_HashTable` | — | environment |
| `OCI_Subscription` | `ocilib::Subscription` | connection |
| `OCI_Event` | `ocilib::Event` | subscription |
| `OCI_Message` | `ocilib::Message` | type info — or a dequeue |
| `OCI_Agent` | `ocilib::Agent` | connection |
| `OCI_Enqueue` | `ocilib::Enqueue` | type info |
| `OCI_Dequeue` | `ocilib::Dequeue` | type info |
| `OCI_SodaDatabase` | `ocilib::SodaDatabase` | connection |
| `OCI_SodaCollection` | `ocilib::SodaCollection` | SODA database |
| `OCI_SodaDocument` | `ocilib::SodaDocument` | SODA database — or a doc cursor |
| `OCI_SodaOptions` | `ocilib::SodaOptions` | SODA database |
| `OCI_SodaCollectionCursor` | `ocilib::SodaCollectionCursor` | SODA database |
| `OCI_SodaDocumentCursor` | `ocilib::SodaDocumentCursor` | SODA collection |
| `OCI_DirectPath` | `ocilib::DirectPath` | type info |
| `OCI_ShardingKey` | `ocilib::ShardingKey` | environment |

A few types have no C++ twin (`—`) because the C++ layer covers the job
differently — collections are iterated with C++-style iterators instead of an
`OCI_Iterator` handle, for instance.

## Errors: the fourth leg

Every call that can fail either returns `FALSE`/NULL (C) or throws an
`ocilib::Exception` (C++). In C, two channels deliver the details:

- your **error handler callback** — installed when the environment is created,
  invoked immediately with a filled error object;
- the **last-error poll** — `OCI_GetLastError()` returns the calling thread's
  most recent error at any time.

## The two build flavors

| Flavor | Compile define | Meaning |
|--------|----------------|---------|
| MBCS | `OCI_CHARSET_MBCS` | `otext` = `char`, strings are bytes |
| Wide | `OCI_CHARSET_WIDE` | `otext` = `wchar_t` (UTF-16) |

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
- Ownership is a tree rooted at the environment; borrowed handles are never freed.
- C returns `FALSE`/NULL + error callback; C++ throws `ocilib::Exception`.
- MBCS/wide charset and linkage/runtime import are build-time choices; your code does not change.

**Next:** get the two things every sample needs — an Oracle database and the Instant Client.
