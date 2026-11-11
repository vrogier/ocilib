---
title: Checkpoint 0 — quickstart checklist
navtitle: Checkpoint 0 quickstart
part: 0
level: beginner
---

# Checkpoint 0 — quickstart checklist

You should now be able to install the Oracle client, build OCILIB on your platform,
create an environment, open a connection, and read an `OCI_Error`.

## Sanity checklist

Before moving to Part 1, make sure all of these are true for **you**:

- [ ] `sqlplus usr/pwd@db` connects (proves client + database + credentials)
- [ ] `tutorial-schema.sql` has been run in your schema (`select count(*) from employees` → 8)
- [ ] `OCILIB_DEMO_DB`, `OCILIB_DEMO_USR`, `OCILIB_DEMO_PWD` are exported
- [ ] You can build a trivial `OCI_EnvironmentCreate`/`Free` program and it exits 0
- [ ] The connection sample prints a server version

## First-run failures — diagnostic table

| Symptom | Most likely cause | Fix |
|---------|-------------------|-----|
| `OCI_EnvironmentCreate` returns NULL / throws | `oci.dll`/`libclntsh.so` not found | add Instant Client to `PATH`/`LD_LIBRARY_PATH` |
| Loader error *before* `main` | 32/64-bit mixup (exe vs Instant Client vs `ocilib*.dll`) | match all three bitnesses |
| `ORA-12154` on connect | bad connect string or missing `tnsnames.ora` | use Easy Connect `host:port/service` |
| `ORA-12541` / `ORA-12514` | DB down / wrong service name | start XE/container; check `lsnrctl status` |
| `ORA-01017` | wrong credentials | re-export `OCILIB_DEMO_*` |
| `ORA-00942` on samples | schema not loaded | run `tutorial-schema.sql` |
| Link error `OCI_*` unresolved (Windows) | wrong import lib | `ocilibm.lib` ≠ `ocilibw.lib` |
| `libocilib.so` not found at run time | missing `ldconfig`/`LD_LIBRARY_PATH` | `sudo ldconfig` or export the path |

## Mini-exercise (no solution given)

Write `toolbox.c` (or `.cpp`) — a program that:

1. Creates the environment with a printing error handler.
2. Connects using the `OCILIB_DEMO_*` variables.
3. Prints **server version string**, **connection version**, and `OCI_ConnectionPing` result.
4. Takes an optional first command-line argument as the connect string (falling back
   to the env var) — you'll reuse this habit in later exercises.
5. Frees everything in the right order and exits 0 on success.

If it compiles, connects, and prints — you're done with Part 0.

## Recap of Part 0

- Oracle client + env vars + tutorial schema = the whole lab setup.
- `Environment` → `Connection`, always in that order, freed in reverse.
- Errors: `OCI_Error` via callback or `OCI_GetLastError`; `ocilib::Exception` in C++.

**Next:** Part 1 — everything about connection strings, session modes, ping/break and
introspecting what you connected to.
