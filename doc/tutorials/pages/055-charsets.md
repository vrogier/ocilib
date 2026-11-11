---
title: MBCS vs wide builds — which one do you need?
navtitle: Charset builds
part: 0
level: beginner
---

# MBCS vs wide builds — which one do you need?

**Objectives** — by the end of this page you will be able to:

- explain what the `OCI_CHARSET_MBCS` and `OCI_CHARSET_WIDE` builds really do
- understand why the MBCS ("ANSI") build also handles UTF-8
- pick the right flavor for your platform and application

## In context — one API, two compiled flavors

OCILIB ships in two compile-time flavors. Every string in the public API —
SQL text, object names, connection strings, fetched `VARCHAR2` data — uses the
portable type `otext`, which maps to a different C type in each flavor:

| Build macro | `otext` | `OTEXT("x")` | C++ `String` | OCI is initialized in… |
|-------------|---------|--------------|--------------|------------------------|
| `OCI_CHARSET_MBCS` | `char` | `"x"` | `std::string` | default NLS mode |
| `OCI_CHARSET_WIDE` | `wchar_t` | `L"x"` | `std::wstring` | **UTF-16** (`OCI_UTF16ID`) |

The API surface is identical — same functions, same handles. What changes is
the encoding OCI itself runs in, and therefore the encoding your strings must
be in. That is a build-time property of the *library binary*: on Windows the
prebuilt DLLs are even named differently (`ocilibm.dll` vs `ocilibw.dll`).

## The MBCS build — formerly "ANSI"

The old name `OCI_CHARSET_ANSI` was misleading: the build is **not**
limited to single-byte codepages — "MBCS" says it properly. OCILIB creates the OCI environment in
Oracle's *default* NLS mode — so the client-side encoding is whatever the
Oracle client is configured for, driven by `NLS_LANG` (or the Windows registry
equivalent):

- `NLS_LANG=AMERICAN_AMERICA.WE8MSWIN1252` → single-byte ANSI-codepage strings
- `NLS_LANG=AMERICAN_AMERICA.AL32UTF8` → **full UTF-8** in `char*`

With an UTF-8 Oracle charset in `NLS_LANG`, `char*` strings carry UTF-8 end to
end: SQL text, object names, bound values, fetched data. If your app is
UTF-8-native — most Unix software today — the MBCS build *is* the UTF-8 build.
You do not need the wide build for UTF-8 data.

## The wide build always means UTF-16

Wide builds ignore `NLS_LANG` entirely. OCILIB calls `OCIEnvNlsCreate` with
`OCI_UTF16ID` for both the database charset and the national charset — OCI's
Unicode mode, a fixed-length 2-byte UTF-16 encoding. All string traffic —
including statement text and object names — goes through it.

`wchar_t` is where platforms diverge, and OCILIB absorbs the difference:

- **Windows** — `wchar_t` is 2 bytes, already UTF-16. `otext` buffers go to and
  from Oracle with **zero conversion**.
- **Unix** — `wchar_t` is 4 bytes (UTF-32). OCILIB transparently converts every
  string between UTF-16 and UTF-32 — temporary buffers for statements and
  names going in, in-place buffer expansion for fetched/bound strings coming
  out. No extra allocations, invisible to you.

!!! warning "Database charset caveat"
    In wide builds, UTF-16 is used for **all** OCI string traffic regardless
    of `NLS_LANG`. The target database charset should be a Unicode charset
    (AL32UTF8, UTF8) or a superset of UTF-16 — otherwise the Oracle client
    may silently substitute characters it cannot represent.

## Choosing — a practical decision tree

- **Application uses `char*`/`std::string`, runs on Unix, or wants UTF-8** →
  `OCI_CHARSET_MBCS` (set `NLS_LANG=….AL32UTF8` for UTF-8).
- **Application is natively `wchar_t`/`std::wstring`** — typically Windows
  `UNICODE`/`_UNICODE` projects → `OCI_CHARSET_WIDE`.
- **Prebuilt OCILIB binary already picked for you?** Match it: the macro in
  your build must equal the one OCILIB was compiled with. Mixing them breaks
  the ABI (`sizeof(otext)` differs — a `char*` where `wchar_t*` is expected,
  or the reverse).

Source code stays portable across both flavors if you route every literal and
string through `otext`/`OTEXT()` (C) or the `String` typedef (C++) — the
tutorials do this everywhere.

## Putting it together — the typical sequence

Charset choice happens once, at build time:

1. **Decide** — `char*`/UTF-8 world → MBCS build; `wchar_t` world → wide.
2. **Configure** — `--with-oracle-charset=mbcs|wide` (autotools),
   `-DOCI_CHARSET_MULTIBYTE|WIDE` in your own project, or pick `ocilibm`/`ocilibw`.
3. **(MBCS only) set `NLS_LANG`** — to the Oracle charset matching your
   strings; `AL32UTF8` for UTF-8.
4. **Write portable code** — `OTEXT()` literals in C, `String` in C++, and the
   same source compiles under either flavor.

Hints: you can query the compiled flavor at runtime with
`OCI_EnvironmentGetCharset` (`OCI_CHAR_MBCS`/`OCI_CHAR_WIDE`); the flavor is
fixed per process — you cannot switch mid-run; and database *column* charsets
(`NCHAR`/`NVARCHAR2`, charset forms) are a separate, runtime topic covered by
the [strings & charsets page](text-charsets.html).

## Try it yourself

1. Print `OCI_EnvironmentGetCharset(env)` — which flavor did your OCILIB come
   in as?
2. In an MBCS build, set `NLS_LANG=AMERICAN_AMERICA.AL32UTF8`, insert a
   `VARCHAR2` containing non-ASCII text, fetch it back — UTF-8 round-trip.
3. Compile the same sample source with both `OCI_CHARSET_MBCS` and
   `OCI_CHARSET_WIDE` — confirm nothing changes but the macro.

## Common pitfalls

- **"ANSI means ASCII-only"** — the old name lied: with a UTF-8 `NLS_LANG` it is a UTF-8
  build. The name refers to `char*`, not to a codepage.
- **Mixing flavors** — your app's `OCI_CHARSET_*` define must match the
  OCILIB binary's; a mismatch compiles on `char`-only paths and corrupts the
  first `wchar_t` argument.
- **Expecting `NLS_LANG` to matter in a wide build** — it doesn't; OCI is
  pinned to UTF-16 either way.

## Recap

- Two builds, one API: MBCS = `char` — ANSI codepages *or* UTF-8
  via `NLS_LANG`. Wide = `wchar_t`, OCI pinned to UTF-16 with automatic
  UTF-16↔UTF-32 conversion on 4-byte-`wchar_t` platforms.

**Next:** [Hello OCILIB](hello-ocilib.html) — your first program.
