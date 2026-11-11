---
title: Diagnostics — logging in depth
navtitle: Logging
part: 0
level: beginner
---

# Diagnostics — logging in depth

**Objectives** — by the end of this page you will be able to:

- switch on OCILIB's structured logging without touching your code
- choose the right level and categories for what you're debugging
- read every field of a log line and trace calls from ENTER to LEAVE

## Concepts

OCILIB emits **structured, opt-in logging** — turned on and steered entirely through environment variables, read once when the environment is created.

No code changes, no rebuild, and it applies identically to C and C++ programs (it lives below the language API).

Three variables configure everything:

| Variable | Values | Default |
|----------|--------|---------|
| `OCILIB_LOG` | `ERROR` \| `WARN` \| `INFO` \| `DEBUG` \| `TRACE` | off |
| `OCILIB_LOG_CATEGORIES` | `LIB`, `MEM`, `OCI`, `ALL` — comma-separated list | `ALL` |
| `OCILIB_LOG_OUTPUT` | a file path \| `STDERR` \| `STDOUT` | `stderr` |

```bash
export OCILIB_LOG=DEBUG
export OCILIB_LOG_CATEGORIES=LIB,OCI
export OCILIB_LOG_OUTPUT=/tmp/ocilib.log
./my_app
```

## Levels — cumulative, from quiet to everything

A level shows itself **plus every lower level** — `INFO` also prints warnings and
errors, `TRACE` prints all of it:

| Level | What it shows |
|-------|---------------|
| `ERROR` | OCILIB exceptions and memory-leak reports |
| `WARN` | OCI exception warnings |
| `INFO` | connect/disconnect, statements prepared/executed, successful database operations |
| `DEBUG` | other non-critical diagnostics — execute results and details |
| `TRACE` | `ENTER`/`LEAVE` call tracing plus the deepest diagnostics — the most verbose |

Pick the *smallest* level that answers your question: `INFO` for "did it connect?",
`DEBUG` for "what did the calls return?", `TRACE` for "show me the whole flow".

## Categories — which subsystem reports

Categories filter *what* logs; they're a bitmask combined as a comma list in
`OCILIB_LOG_CATEGORIES`:

| Category | Contents |
|----------|----------|
| `LIB` | OCILIB's own diagnostics: exceptions, connect/prepare/execute, and the API `ENTER`/`LEAVE` call trace |
| `MEM` | memory + handle allocations and frees, and memory-leak reports |
| `OCI` | `ENTER`/`LEAVE` trace of the **raw OCI calls** OCILIB issues (`env->api->*`) — see exactly which Oracle calls happen under each API call |
| `ALL` | `LIB` + `MEM` + `OCI` |

!!! note "One deliberate exception"
    Error retrieval itself (`OCIErrorGet`) is never traced — it runs inside error
    handling, so logging it would recurse.

## Anatomy of a log line

Every line follows one shape:

```text
YYYY-MM-DD HH:MM:SS.mmm  [LVL]  [CAT]  [step]  fn: message
└─ timestamp           └─3-char level└─category└─ENTER/LEAVE (call-trace only)
```

`[LVL]` is `ERR`, `WRN`, `INF`, `DBG` or `TRC`; `[step]` appears only on call-trace
lines and is either `ENTER` or `LEAVE` — the two bookends of a call.

## Full example — tracing a connection

Take the smallest possible program — create the environment, connect, free
everything:

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
    OCI_Connection  *cn  = OCI_ConnectionCreate(env,
        getenv("OCILIB_DEMO_DB"), getenv("OCILIB_DEMO_USR"),
        getenv("OCILIB_DEMO_PWD"), OCI_SESSION_DEFAULT);

    printf("connected: %s\n", cn ? "yes" : "no");

    OCI_ConnectionFree(cn);
    OCI_EnvironmentFree(env);
    return EXIT_SUCCESS;
}
```

```cpp
#include "ocilib.hpp"
#include <cstdlib>
#include <iostream>

int main()
{
    try
    {
        ocilib::Environment env;
        ocilib::Connection  con(env, getenv("OCILIB_DEMO_DB"),
                                     getenv("OCILIB_DEMO_USR"),
                                     getenv("OCILIB_DEMO_PWD"));
        std::cout << "connected: yes\n";
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

Run it with tracing on:

```bash
export OCILIB_LOG=TRACE
export OCILIB_LOG_CATEGORIES=ALL
./connect_demo            # log goes to stderr
```

and the log tells the whole internal story — your API call opening (`ENTER`), the
raw Oracle calls it makes underneath (`OCI` category), the allocations behind the
scenes (`MEM`), and the call closing (`LEAVE`) with its return value:

```output
2026-11-07 10:12:03.411 [TRC] [LIB] ENTER OcilibConnectionCreate
2026-11-07 10:12:03.412 [DBG] [MEM] OcilibMemoryAlloc: bytes=27 type=7
2026-11-07 10:12:03.418 [TRC] [OCI] ENTER OCIServerAttach
2026-11-07 10:12:03.530 [TRC] [OCI] LEAVE OCIServerAttach (ret=0000000000000000)
2026-11-07 10:12:03.531 [INF] [LIB] OcilibConnectionCreateInternal: handle=0000000000484510 connected to FREEPDB1
2026-11-07 10:12:03.531 [TRC] [LIB] LEAVE OcilibConnectionCreate (ret=0000000000484510)
2026-11-07 10:12:03.540 [TRC] [LIB] ENTER OcilibConnectionFree
2026-11-07 10:12:03.545 [TRC] [LIB] LEAVE OcilibConnectionFree (ret=1)
```

Read traces as **matched ENTER/LEAVE pairs**: the calls between an `ENTER` and its `LEAVE` are what that call did internally — including which OCI primitives it used and what they returned.

A `LEAVE (ret=…)` showing an error return is where a failure surfaced.

## Picking the right mix

| Problem | Recommended |
|---------|-------------|
| "It won't connect" | `INFO` + `LIB` — see the connect call and its params |
| "Which OCI call fails?" | `DEBUG` or `TRACE` + `OCI` — raw `env->api->*` ENTER/LEAVE lines |
| "Handle/memory leak?" | `WARN` or higher + `MEM` — alloc/free + leak reports |
| "Trace my whole session" | `TRACE` + `ALL` — every API and OCI call, ENTER/LEAVE paired |

## Using it well

- **File output for runs, stderr for REPL** — `OCILIB_LOG_OUTPUT=/tmp/ocilib.log`
  keeps program output readable while preserving the trace.
- **Errors appear in context** — the same errors your handler sees show up in the
  `LIB` stream, surrounded by the calls that led to them.
- **Pair with server-side traces** (page 120) — client-side `ENTER`/`LEAVE` call
  flow *and* `V$SESSION` module/action visibility in the same session.

!!! warning "Set them before startup"
    Logging is environment-variable driven — the variables are read once at
    environment creation, so set them before the program starts.

## Putting it together — the typical sequence

OCILIB logging is a per-environment diagnostic channel:

1. **Enable it** — `OCI_EnableTrace`/`OCI_TraceSet*` on the environment with
   the flags for what you want traced.
2. **Run your code** — every OCI call, duration, and SQL gets logged.
3. **Read it** — log callback or file, per `TraceSetOut`.
4. **Disable for production** — tracing is verbose and slow; it's a debug
   tool, not a monitoring feature.

Hints: trace flags compose — OR them for SQL + timing + errors together;
the output format is designed for diffing — two runs, one flag difference,
diff the logs; and tracing is the fastest way to see *what OCILIB actually
sent* — invaluable when your SQL works in SQL*Plus but not here.

## Try it yourself

1. Run your connection sample with `OCILIB_LOG=TRACE` — follow the
   create→connect→free pairs from ENTER to LEAVE.
2. Introduce a bad password and read the `ERR` line *in context* of the calls
   around it.
3. Filter to `OCI` only and count the real Oracle calls a single connect makes —
   you'll see where the round-trips are.
4. Deliberately leak a handle and confirm `MEM` reports it at environment free.

## Common pitfalls

- **Leaving `TRACE` on in production** — it's for debugging; scope it with levels
  and categories instead of disabling it.
- **Setting the variables after startup** — they're read once at environment
  creation; export before launching.
- **Expecting output on `stdout` by default** — it goes to `stderr`; redirect or
  set `OCILIB_LOG_OUTPUT`.

## Recap

- `OCILIB_LOG` + `OCILIB_LOG_CATEGORIES` + `OCILIB_LOG_OUTPUT` = structured tracing
  with zero code.
- Levels stack `ERROR→TRACE`; categories pick `LIB` (API), `MEM` (memory) or `OCI`
  (raw Oracle calls); every line is `[LVL] [CAT] [step] fn: message`.

**Next:** a checklist for the classic "it doesn't even start" problems.
