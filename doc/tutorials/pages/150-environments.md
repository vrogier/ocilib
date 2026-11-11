---
title: Working with environments — ownership & lifecycle rules
navtitle: Environments in depth
part: 1
level: intermediate
---

# Working with environments — ownership & lifecycle rules

**Objectives** — by the end of this page you will be able to:

- explain exactly which object owns which
- decide when (and whether) to use more than one environment
- avoid the classic use-after-free ordering bugs

## The ownership contract

Every OCILIB call takes place **inside** an environment. The rules, restated now that
you have seen the moving parts:

1. Create the environment **before** any other call; free it **after** all others.
2. A handle created *from* an object is owned by it — a connection belongs to its
   environment, a statement to its connection.
3. Freeing a parent frees its children — but freeing in leaf→root order keeps your
   intent obvious and debuggable.
4. **Borrowed handles are never freed** — resultsets handed out by statements,
   columns handed out by resultsets, connections checked out of pools: their
   lifetime belongs to the parent.

```text
OCI_EnvironmentFree(env)        /* everything below dies with it        */
├── OCI_ConnectionFree(cn)
│   ├── OCI_StatementFree(st)   /* its resultset + binds die too        */
│   └── OCI_TransactionFree(tx)
└── OCI_PoolFree(pool)
```

## One environment or several?

v5 allows **multiple environments** in one process — each carries its own error
handler, settings, and per-environment OCI symbol table. Sensible reasons to create a
second one:

- a plugin/library that must not disturb the host app's handler;
- isolating a component that needs different flags (`OCI_ENV_EVENTS`).

And the hard rule that comes with it:

!!! warning "Handles never cross environments"
    A connection created on `env1` cannot be used with a statement created under
    `env2`. OCILIB validates the context and fails fast — but design so it never
    happens. When in doubt, ask any handle which environment it belongs to
    (`OCI_HandleGetEnvironment`, page 140).

## The C++ view

`ocilib::Environment` is the RAII mirror; `Connection`, `Statement`, … capture their
environment from the handle they're built on (via `OCI_HandleGetEnvironment`). Passing
an `Environment` object to constructors keeps the ownership explicit:

```cpp
Environment env;
Connection con(env, db, usr, pwd);      // owned by env
Statement st(con);                      // owned by con
Resultset rs = st.GetResultset();       // borrowed — releases automatically
```

When `st` goes out of scope it frees the statement; `rs` releases the borrowed
resultset; `con` disconnects; `env` unloads OCI — all automatically, in order.

## Putting it together — the typical sequence

Environment setup happens once, at process start, before anything else:

1. **Create the environment** — `OCI_EnvironmentCreate(handler, NULL, flags)`:
   the error handler callback goes here, the mode flags (threaded, events)
   lock in the OCILIB feature set.
2. **Register the error handler** — the C API reports via return codes; the
   callback is where you log *why*.
3. **Everything else is scoped to it**: connections, datatypes, pools all live
   under the environment — creating them without one doesn't exist.
4. **Free it last** — `OCI_EnvironmentFree` tears down every child handle.

Hints: the environment is the *root* of the ownership tree — freeing it frees
the children, so order matters; `OCI_ENV_THREADED` is required before
`OCI_Thread*` APIs work; and the error handler is C-only — C++ translates the
same failures into `ocilib::Exception` throws.

## Try it yourself

1. Deliberately free a connection *after* its environment — observe the error path.
2. Create two environments, connect on each, and try to use env2's statement with
   env1's connection — see it rejected.

## Recap

- Environment first, environment last; children before parents.
- Borrowed handles (resultsets, pool connections, columns) are never freed.
- Multiple environments are legal in v5 but handles stay inside their own.

**Checkpoint:** you now own every connection concept. Part 3 puts them to work — SQL
execution, fetching, and binding.
