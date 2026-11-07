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
| `--with-oci-include=` / `--with-oci-lib=` | Oracle SDK/lib paths (linkage) |
| `--with-oracle-charset=(ansi\|wide)` | `otext` = char vs UTF-16 |
| `--enable-static` / `--enable-shared` | library flavor(s) |
| `--enable-unicode` | wide charset shortcut |
| `--enable-warnings=(no\|yes\|paranoid)` | dev diagnostics level |
| `--enable-werror` | warnings as errors (CI only — never ship) |
| `--enable-sanitizers[=list]` | ASan/UBSan/TSan dev builds |
| `--prefix=` | install root |

## Compile-time defines (visible to your code)

| Define | Meaning |
|--------|---------|
| `OCI_CHARSET_ANSI` / `OCI_CHARSET_WIDE` | string flavor of this build |
| `OCI_IMPORT_LINKAGE` / `OCI_IMPORT_RUNTIME` | OCI loading mode |
| `OCILIB_MAJOR/MINOR/REVISION_VERSION` | library version (platform.h) |

## Windows configurations

`proj/dll/ocilib_dll.sln` — `Release - ANSI`/`Release - UNICODE` × `Win32`/`x64`
→ `lib32|64\ociliba|w.{dll,lib}` + `.pdb`.

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
