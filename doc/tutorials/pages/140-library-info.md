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
| `OCI_EnvironmentGetOCICompileVersion` | `Environment::GetCompileVersion()` | OCI headers OCILIB was **compiled** against |
| `OCI_EnvironmentGetOCIRuntimeVersion` | `Environment::GetRuntimeVersion()` | Oracle client loaded **at run time** (`MMmmRR`, e.g. `230500`) |
| `OCI_EnvironmentGetImportMode` | `GetImportMode()` | `ImportLinkage` or `ImportRuntime` |
| `OCI_EnvironmentGetCharset` | `GetCharset()` | `CharsetAnsi` or `CharsetWide` |
| `OCI_ConnectionGetVersion` | `Connection::GetVersion()` | negotiated OCI version for the connection |
| `OCI_ConnectionGetServerVersionString` | `Connection::GetServerVersion()` | server version **string** (`"23.5.0.0"`) |

The numeric versions decode with `OCI_VER_*` macros in C and split getters in C++:

:::sample
```c
OCI_UInt32 v = OCI_EnvironmentGetOCIRuntimeVersion(env);
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
if (OCI_VER_MAJ(OCI_EnvironmentGetOCIRuntimeVersion(env)) >= 23)
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
OCI_Environment *env = OCI_HandleGetEnvironment(cn);
```

```cpp
/* C++ objects keep their environment internally — you rarely ask */
```
:::

## Putting it together — the typical sequence

Version introspection answers "what am I running on" at two levels:

1. **Client side**: `OCI_GetCompileVersion` / `OCI_GetRuntimeVersion` — what
   OCILIB was built as vs what OCI is loaded; `OCI_GetVersionConnection`
   reports what a connection sees.
2. **Server side**: `OCI_ConnectionGetServerVersion*` and
   `GetServerVersionString` — the banner-level answer.
3. **Feature gates**: `OCILIB_HAS_*` / version comparisons let you decide at
   runtime whether to offer sharding, SODA, or vectors — the pages in Part 9
   all hang off these calls.

Hints: mismatched client/server versions are the #1 source of "works on my
machine" — print both in your startup banner; `OCI_GetRuntimeVersion` catches
the case where someone deployed the wrong `oci.dll`; and version-gate at
startup, not deep in your feature code — it's cheaper to refuse early.

## Try it yourself

1. Print all six values on startup — a 10-line "diagnostic dump" you can paste into
   bug reports.
2. Compare compile vs runtime version when running against a *newer* Instant Client
   than you built with (runtime import shines here).

## Recap

- Compile vs runtime OCI version, import mode, charset, server version — all queryable.
- Gate version-dependent features with major-version checks.

**Next:** multiple environments and handle ownership rules.
