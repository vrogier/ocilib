---
title: DBMS_OUTPUT — server-side print capture
navtitle: DBMS_OUTPUT
part: 3
level: beginner
demos: [output.c]
---

# `DBMS_OUTPUT` — server-side print capture

**Objectives** — by the end of this page you will be able to:

- enable and drain `DBMS_OUTPUT` from OCILIB
- tune buffer sizes for chatty PL/SQL
- know when it's useful (and when it isn't)

## Concepts

`DBMS_OUTPUT.PUT_LINE` writes to a session buffer that clients can read *after*
executing PL/SQL — Oracle's `printf` debugging. The connection exposes it:

| C function | C++ method | Purpose |
|------------|------------|---------|
| `OCI_ServerEnableOutput` | `EnableServerOutput` | allocate the buffer (`bufsize`, `arrsize`, `linesize`) |
| `OCI_ServerGetOutput` | `GetServerOutput` | pop the next buffered line (NULL/empty when drained) |
| `OCI_ServerDisableOutput` | `DisableServerOutput` | turn it off |

:::sample
```c
OCI_ServerEnableOutput(cn, 32000, 5, 255);   /* buf 32k, 5 lines/batch, ≤255 chars */

OCI_ExecuteStmt(st, OTEXT(
    "begin "
    "  dbms_output.put_line('First  line'); "
    "  dbms_output.put_line('Second line'); "
    "end;"));

const otext *line;
while ((line = OCI_ServerGetOutput(cn)))
{
    printf("%s\n", line);
}

OCI_ServerDisableOutput(cn);
```

```cpp
con.EnableServerOutput(32000, 5, 255);

st.Execute("begin dbms_output.put_line('First  line');"
           "       dbms_output.put_line('Second line'); end;");

std::vector<String> lines;
while (con.GetServerOutput(lines))          /* batch-fills lines */
{
    for (auto &l : lines) std::cout << l << "\n";
}

con.DisableServerOutput();
```
:::

## Flow

1. **Enable** output *before* executing the PL/SQL that prints.
2. Execute — `put_line` writes go into the buffer.
3. **Read** the buffer *after* the call returns — lines arrive FIFO; drain fully.
4. Repeat for more calls, or disable when done.

## Sizing

- `bufsize` — total server buffer (default Oracle limit is fine for learning).
- `arrsize` — lines fetched per round-trip.
- `linesize` — max chars per line (Oracle caps at 32767 in recent versions; 255 is the
  classic default).

## Try it yourself

1. Print a progress trace from a multi-statement block (`put_line` between steps).
2. Set `linesize` to 10 and watch long lines wrap.
3. Use `GetServerOutput` to build a "log pane" around your `qtool` from Checkpoint 1.

## Common pitfalls

- **Getting NULL immediately** — you enabled output *after* the execute, or forgot to
  drain until NULL before the next call.
- **Expecting real-time output** — lines only arrive after the call completes
  (not during a long-running block).
- **Production use** — it's a debugging channel, not a data API; prefer logging
  tables for anything durable.

## Recap

- Enable → execute → drain the buffer → disable.
- A debugging lifeline for PL/SQL — but not a streaming channel.

**Checkpoint:** Part 3 ends with a small payroll-procedure exercise.
