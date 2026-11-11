---
title: Appendix — build options & compile defines
navtitle: Build flags
part: 10
---

# Appendix — build options & compile defines

## `./configure` options (Unix)

| Option | Effect |
|--------|--------|
| `--with-oracle-import=(linkage\|runtime)` | link `-lclntsh` vs `dlopen` at run time |
| `--with-oracle-headers-path=` / `--with-oracle-lib-path=` | OCI headers/library paths (linkage mode) |
| `--enable-static` / `--enable-shared` | library flavor(s) |
| `--enable-warnings=(no\|yes\|paranoid)` | dev diagnostics level |
| `--enable-werror` | warnings as errors (CI only — never ship) |
| `--enable-sanitizers[=list]` | ASan/UBSan/TSan dev builds |
| `--prefix=` | install root |

## Compile-time defines (visible to your code)

| Define | Meaning |
|--------|---------|
| `OCI_CHARSET_MBCS` / `OCI_CHARSET_WIDE` | string flavor of this build — `WIDE` is Windows-only (`#error` elsewhere) |
| `OCI_IMPORT_LINKAGE` / `OCI_IMPORT_RUNTIME` | OCI loading mode |
| `OCILIB_MAJOR/MINOR/REVISION_VERSION` | library version (platform.h) |

## Windows configurations

`proj/dll/ocilib_dll.sln` — `Release - MBCS`/`Release - WIDE` × `Win32`/`x64`
→ `lib32|64\ocilib.{dll,lib}` (MBCS) / `ocilibw.{dll,lib}` (wide) + `.pdb`.

## Environment flags (`OCI_EnvironmentCreate` mode)

| Flag | Effect |
|------|--------|
| `OCI_ENV_DEFAULT` | single-threaded |
| `OCI_ENV_THREADED` | internal synchronization on |
| `OCI_ENV_EVENTS` | async callbacks (AQ subscribe, CQN, HA) |

## Session modes (`OCI_ConnectionCreate` mode)

`OCI_SESSION_DEFAULT`, `SYSDBA`, `SYSOPER`, `SYSASM`, `SYSBKP`, `SYSDGD`, `SYSKMT`,
`SYSRAC`, `PRELIM_AUTH`.

## Diagnostics env vars

`OCILIB_LOG`, `OCILIB_LOG_CATEGORIES`, `OCILIB_LOG_OUTPUT` — see the
[logging page](logging.html).
`OCILIB_DEMO_DB/USR/PWD` — tutorial/demo credentials convention.
