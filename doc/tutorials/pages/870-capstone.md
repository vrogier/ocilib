---
title: Capstone — the complete employee service
navtitle: Capstone
part: 9
level: advanced
---

# Capstone — the complete employee service

You've covered the entire OCILIB API. This page ties it together into a reference
application — one program that uses a bit of everything.

## The app — `empdir` (employee directory)

A single-file CLI that already exists in spirit across the checkpoints:

```
empdir list [--dept N] [--json]
empdir get <empno>
empdir add <empno> <name> <job> <sal> <deptno>
empdir raise <deptno> <pct>
empdir import <csv> [--mode=array|dirpath]
empdir watch                      (CQN on employees)
empdir jobs submit|work           (AQ)
empdir profile <empno>            (JSON doc ops)
```

## What each command uses

| Command | API in play | Pages |
|---------|-------------|-------|
| `list`/`get` | statements, binds, resultset getters, scrollable | 210–275 |
| `add` | prepared binds, transactions | 245/410 |
| `raise` | PL/SQL proc + OUT param, RETURNING | 315/280 |
| `import` | array binds or `OCI_DirPath` | 810/815 |
| `watch` | `OCI_ENV_EVENTS` + `OCI_Subscription` | 730 |
| `jobs` | AQ enqueue/dequeue, `job_t` payload | 710–725 |
| `profile` | `OCI_Json`/`JsonNode` DOM | 570/575 |
| all | pool + `OCI_ENV_THREADED` + statement cache | 420/425/430 |

## Architecture sketch

```text
main
├── Environment (THREADED | EVENTS)
├── Pool (OCI_POOL_CONNECTION, min=2 max=8)
│     └── per-command OCI_Connection checkout
├── commands dispatch on argv[1]
└── error handler → stderr (+ OCILIB_LOG for tracing)
```

Each command checks out a pooled connection, does its work, commits once, and releases — the exact pattern from Checkpoint 3.

Statements are prepared once per command and bound by name.

## Stretch goals

- Add `empdir export --objects` writing departments as `dept_t` object graphs.
- Add `empdir docs` running the SODA collection CRUD from Part 9.
- Wire `OCI_SetTrace(MODULE/ACTION)` per command and watch `V$SESSION`.

## The source

The full program — one file per language, eight commands, one pool — lives on the
next page: [empdir — the capstone source code](empdir-implementation.html).

## Where to go next

- The **appendices** for the complete C↔C++ map, type cheat sheet, error codes,
  build flags and the version matrix.
- The **`demo/`** programs — every page's demo badge links to a runnable twin.
- The **Doxygen API reference** (`doc/html`) for per-function parameters.

Congratulations — you can now write real OCILIB applications.
