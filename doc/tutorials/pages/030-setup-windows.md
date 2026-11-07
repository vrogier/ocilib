---
title: Developing on Windows
part: 0
level: beginner
---

# Developing on Windows

**Objectives** — by the end of this page you will be able to:

- get OCILIB into a Windows project the easy way: **NuGet**
- use the shipped binaries directly when you don't want a package
- know when (rarely) you'd build the DLLs yourself

## Option A — NuGet (recommended)

The OCILIB NuGet package drops headers, import libs and DLLs into your project with
zero manual plumbing:

```powershell
Install-Package ocilib            # Package Manager Console
# or: dotnet add package ocilib   # .NET CLI / VS project
```

After install, `#include "ocilib.h"` / `"ocilib.hpp"` works, linking is wired, and the right `ocilib*.dll` lands next to your executable.

This is the preferred way to consume OCILIB on Windows — you only need the Oracle Instant Client on `PATH` at run time (see Prerequisites).

## Option B — shipped binaries

The repo ships prebuilt binaries in `lib32\` and `lib64\`:

| File | Charset | Arch |
|------|---------|------|
| `lib64\ociliba.dll` + `.lib` | ANSI (`char`) | x64 |
| `lib64\ocilibw.dll` + `.lib` | UNICODE (UTF-16) | x64 |
| `lib32\ociliba.dll` / `ocilibw.dll` | same, 32-bit | x86 |

Wire them into a `.vcxproj` manually:

```xml
<PropertyGroup>
  <IncludePath>$(OcilibRoot)include;$(IncludePath)</IncludePath>
  <LibraryPath>$(OcilibRoot)lib64;$(LibraryPath)</LibraryPath>
</PropertyGroup>
<ItemDefinitionGroup>
  <Link><AdditionalDependencies>ociliba.lib;%(AdditionalDependencies)</AdditionalDependencies></Link>
</ItemDefinitionGroup>
```

Plus `ociliba.dll` (or `ocilibw.dll`) next to your `.exe` — that's the whole setup.

!!! tip "ANSI or UNICODE?"
    `ociliba*` = `char` strings, `ocilibw*` = UTF-16 (`otext` = `unsigned short`).
    Pick **ANSI** unless you specifically want wide strings — samples assume ANSI.

## Option C — build from source (uncommon)

You only need this to patch/debug the library itself: open
`proj\dll\ocilib_dll.sln` and build `Release - ANSI|UNICODE` × `Win32|x64`, or build
from a developer prompt with `VsDevCmd.bat` + `msbuild`:

```powershell
# Visual Studio 2026 (v145 toolset)
cmd /c '"c:\Program Files\Microsoft Visual Studio\18\Enterprise\Common7\Tools\VsDevCmd.bat" -arch=amd64 && msbuild proj\dll\ocilib_dll.sln /p:Configuration="Release - ANSI" /p:Platform=x64'

# Visual Studio 2022  (v143 toolset) — same command, VS17 path 
cmd /c '"c:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat" -arch=amd64 && msbuild proj\dll\ocilib_dll.sln /p:Configuration="Release - ANSI" /p:Platform=x64'
```

For Unicode: `/p:Configuration="Release - UNICODE"`. For 32-bit:
`VsDevCmd.bat -arch=x86` + `/p:Platform=Win32`.

!!! note "`VsDevCmd.bat`, not `LaunchDevCmd.bat`"
    `LaunchDevCmd.bat` opens a *new* prompt and loses the environment for chained
    commands — always use `VsDevCmd.bat` for one-shot builds like the above.

Outputs land in `lib32\`/`lib64\` exactly like the shipped ones.

## Try it yourself

1. `Install-Package ocilib` in a fresh console app and compile `demo\conn.c`.
2. Point `OCILIB_DEMO_*` at your database and run it.

## Common pitfalls

- **Missing `oci.dll`** — NuGet ships OCILIB, not the Oracle client; Instant Client
  must be on `PATH`.
- **Mixing `ociliba`/`ocilibw`** — import lib must match your intended charset.
- **32/64-bit mixup** — app bitness must match the Instant Client's.

## Recap

- Prefer NuGet; shipped `lib32`/`lib64` binaries cover the manual path; building from
  source is for contributors.
- Oracle Instant Client is still required at run time — it is not part of OCILIB.

**Next:** the Linux side — autotools build, import modes, pkg-config.
