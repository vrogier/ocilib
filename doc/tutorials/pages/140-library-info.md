---
title: Library introspection — versions, import mode, charset
navtitle: Library introspection
part: 1
level: beginner
---

# Library introspection — versions, import mode, charset

**Objectives** — by the end of this page you will be able to:

- report which Oracle client and OCILIB you are running against
- detect the build flavor (charset, import mode) at run time
- retrieve the owning environment of any handle

## What you can ask the library

The environment answers three kinds of questions: *which OCI did you build against?*,
*which OCI is loaded right now?*, and *how was OCILIB built?* — plus per-connection
facts like the server version. All available without any round-trip:

| C function | C++ method | Tells you |
|------------|------------|-----------|
| `OCI_GetOCICompileVersion` | `Environment::GetCompileVersion()` | OCI headers OCILIB was **compiled** against |
| `OCI_GetOCIRuntimeVersion` | `Environment::GetRuntimeVersion()` | Oracle client loaded **at run time** (`MMmmRR`, e.g. `230500`) |
| `OCI_GetImportMode` | `GetImportMode()` | `ImportLinkage` or `ImportRuntime` |
| `OCI_GetCharset` | `GetCharset()` | `CharsetAnsi` or `CharsetWide` |
| `OCI_GetVersionConnection` | `Connection::GetVersion()` | negotiated OCI version for the connection |
| `OCI_GetVersionServer` | `Connection::GetServerVersion()` | server version **string** (`"23.5.0.0"`) |

The numeric versions decode with `OCI_VER_*` macros in C and split getters in C++:

:::sample
```c
OCI_UInt32 v = OCI_GetOCIRuntimeVersion();
printf("OCI client %u.%u.%u\n",
       OCI_VER_MAJ(v), OCI_VER_MIN(v), OCI_VER_REV(v));
```

```cpp
std::cout << "OCI client " << env.GetRuntimeMajorVersion() << "."
          << env.GetRuntimeMinorVersion() << "."
          << env.GetRuntimeRevisionVersion() << "\n";
```
:::

## Why it matters — feature gating

Oracle capabilities depend on **both** client and server versions. A robust app
checks before touching a newer feature:

:::sample
```c
if (OCI_VER_MAJ(OCI_GetOCIRuntimeVersion()) >= 23)
{
    /* safe to use vectors, JSON schema validation, … */
}
```

```cpp
if (env.GetRuntimeMajorVersion() >= 23)
{
    /* safe to use vectors, JSON schema validation, … */
}
```
:::

The same pattern guards SODA (≥18c), sharding (≥12.2), implicit resultsets (≥12c)…
The appendix collects the whole feature → version matrix (page 925).

## Who owns this handle?

Every handle knows the environment it belongs to — rarely needed in simple apps,
essential in libraries and callbacks where a handle arrives without context:

| C function | C++ equivalent | Returns |
|------------|----------------|---------|
| `OCI_HandleGetEnvironment` | `handle->GetEnvironmentHandle()` (internal) | the owning environment |

:::sample
```c
OCI_Environment *env = OCI_HandleGetEnvironment((OCI_Handle *)cn);
```

```cpp
/* C++ objects keep their environment internally — you rarely ask */
```
:::

## Try it yourself

1. Print all six values on startup — a 10-line "diagnostic dump" you can paste into
   bug reports.
2. Compare compile vs runtime version when running against a *newer* Instant Client
   than you built with (runtime import shines here).

## Recap

- Compile vs runtime OCI version, import mode, charset, server version — all queryable.
- Gate version-dependent features with major-version checks.

**Next:** multiple environments and handle ownership rules.
