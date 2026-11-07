---
title: Prerequisites — database & Oracle client
navtitle: Prerequisites
part: 0
level: beginner
---

# Prerequisites — database & Oracle client

**Objectives** — by the end of this page you will be able to:

- pick a way to get an Oracle database for learning
- install the Oracle Instant Client on Windows or Linux
- set the environment variables every tutorial sample uses

## You need three things

1. **An Oracle database** to connect to.
2. **The Oracle client libraries** — OCILIB talks to the database *through* them.
3. **A C and/or C++ toolchain** — OCILIB is C11 / C++17.

## Getting a database

Any of these works; pick the easiest for you:

| Option | Notes |
|--------|-------|
| **Oracle XE** (free) | Full-featured free edition. Download the installer for Windows or Linux from oracle.com. Gives you `localhost:1521/XEPDB1` after a default install. |
| **Docker** | `docker run -d -p 1521:1521 gvenzl/oracle-xe` — fastest path on Linux/macOS/Windows with Docker. Same connect string after startup. |
| **Existing server** | Ask your DBA for a user/schema; note the `host:port/service` string. |

The tutorials assume a schema you can write to. The `SCOTT`/`tiger` tradition is used
in samples but **any user works** — that's what the environment variables below are for.

## Getting the Oracle client (Instant Client)

OCILIB never talks to the server directly — it loads Oracle's client library at run
time. The **Instant Client** is the lightweight, free way to get it.

### Windows

1. Download *Instant Client Basic* (x64) from oracle.com — a zip like
   `instantclient-basic-windows.x64-23.x.x.x.zip`.
2. Unzip to a permanent folder, e.g. `C:\oracle\instantclient_23_x`.
3. Add that folder to your `PATH` (System → Environment Variables), or copy the DLLs
   next to your executable.
4. If you will build OCILIB in **linkage** mode, also download the *SDK* package
   (headers + `oci.lib`) into the same folder.

### Linux

```bash
# Oracle Linux / RHEL-family
sudo dnf install oracle-instantclient-release-23ai-el9
sudo dnf install oracle-instantclient-basic oracle-instantclient-sqlplus

# or manually:
unzip instantclient-basic-linux.x64-23.*.zip -d /opt/oracle
export LD_LIBRARY_PATH=/opt/oracle/instantclient_23_x:$LD_LIBRARY_PATH
```

!!! tip "Runtime import needs no SDK"
    If you build OCILIB with `--with-oracle-import=runtime` (opt-in on Linux),
    it locates `libclntsh.so` / `oci.dll` itself at run time — you do **not** need
    Oracle headers to compile. Without it, the default `linkage` mode needs the
    Instant Client SDK.

## Environment variables used by all samples

Set these once; every tutorial program reads them so no credentials live in code:

:::sample
```sh
# Linux / macOS / WSL — add to ~/.bashrc
export OCILIB_DEMO_DB="localhost:1521/XEPDB1"
export OCILIB_DEMO_USR="scott"
export OCILIB_DEMO_PWD="tiger"
```

```powershell
# Windows — current session
$env:OCILIB_DEMO_DB  = "localhost:1521/XEPDB1"
$env:OCILIB_DEMO_USR = "scott"
$env:OCILIB_DEMO_PWD = "tiger"
# or persist: [Environment]::SetEnvironmentVariable("OCILIB_DEMO_DB", "...", "User")
```
:::

## Toolchain

| Platform | Compiler |
|----------|----------|
| Windows | Visual Studio 2022+ (C17 + `/std:c++17`) |
| Linux | gcc ≥ 10 or clang ≥ 12 (`-std=c11` / `-std=c++17`) |

## Try it yourself

1. Verify the client loads: run `sqlplus $OCILIB_DEMO_USR/$OCILIB_DEMO_PWD@$OCILIB_DEMO_DB`
   (or `sqlplus scott/tiger@localhost:1521/XEPDB1`) — if it connects, everything you
   need is installed.
2. Create the tutorial schema:
   `sqlplus usr/pwd@db @doc/tutorials/assets/sql/tutorial-schema.sql`

## Common pitfalls

- **32/64-bit mixup** — Instant Client must match your *executable* bitness, not your OS.
- **`sqlplus` works but OCILIB can't find `oci.dll`** — `PATH`/`LD_LIBRARY_PATH` set in a
  different shell than the one running your program.
- **ORA-12541 / ORA-12154** — database not up yet (Docker takes a minute) or wrong
  service name (`XEPDB1` vs `XE`).

## Recap

- Database (XE or Docker) + Instant Client + a C11/C++17 compiler = ready.
- Export `OCILIB_DEMO_DB/USR/PWD`; run `tutorial-schema.sql` once.

**Next:** build OCILIB itself — Windows on the left, Linux below it in the TOC.
