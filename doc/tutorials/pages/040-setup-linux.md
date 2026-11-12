---
title: Developing on Linux, macOS & Unix
navtitle: Linux, macOS & Unix
part: 0
level: beginner
---

# Developing on Linux, macOS & Unix

**Objectives** — by the end of this page you will be able to:

- build and install OCILIB with autotools — the same flow on Linux, macOS and
  the other Unixes (Solaris, AIX, HP-UX)
- choose linkage vs runtime Oracle import, static vs shared (non-Windows builds are always MBCS)
- link your program with `-locilib` or pkg-config

## The short version

```bash
./configure
make
sudo make install
```

That builds the OCILIB shared library (`libocilib.so`, `libocilib.dylib` on
macOS; MBCS, **linkage** import) and installs it to
`/usr/local` — headers in `include/`, library + `ocilib.pc` in `lib/`. The default
**linkage** mode needs the Oracle Instant Client **SDK** (headers + `libclntsh.so`)
reachable at configure time — pass `--with-oci-include`/`--with-oci-lib` if they're
not in the standard locations.

## Choosing your build

| Option | Values | Notes |
|--------|--------|-------|
| `--with-oracle-import` | `linkage` \| `runtime` | **`linkage` is the default** (needs Instant Client + SDK at build time). `runtime` is opt-in: OCILIB `dlopen`s `libclntsh.so` itself — builds with **no Oracle SDK** (headers vendored in `src/oci/`). |
| `--with-oci-include` / `--with-oci-lib` | paths | for `linkage` in non-standard locations |
| `--enable-static` / `--enable-shared` | yes/no | shared is default |
| `--enable-warnings` | `no`/`yes`/`paranoid` | dev diagnostics |

```bash
# default build — linkage import, SDK paths given
./configure --with-oci-include=/opt/oracle/instantclient_23_x/sdk/include \
            --with-oci-lib=/opt/oracle/instantclient_23_x && make && sudo make install

# no Oracle SDK on the machine? opt into runtime import
./configure --with-oracle-import=runtime && make && sudo make install

# custom prefix, no sudo
./configure --prefix=$HOME/ocilib && make install
```

After `make install` on Linux, run `sudo ldconfig` so the loader sees the new
library.

## macOS and the other Unixes

The autotools flow is identical — only the shared-library filename and the
loader variable change:

| Platform | Shared library | Loader variable |
|---|---|---|
| Linux | `libocilib.so` | `LD_LIBRARY_PATH` |
| macOS | `libocilib.dylib` | `DYLD_LIBRARY_PATH` |
| AIX | `libocilib.a` (shared archive) | `LIBPATH` |
| HP-UX PA-RISC | `libocilib.sl` | `SHLIB_PATH` |
| Solaris / HP-UX Itanium | `libocilib.so` | `LD_LIBRARY_PATH` |

`ldconfig` is Linux-specific; on macOS `/usr/local/lib` is already in the
loader's default search path, and the other Unixes rely on the loader variable
(or the rpath `libtool` bakes in at install time). The Oracle Instant Client is
available for all of these platforms — substitute its directory wherever
`/opt/oracle/instantclient_23_x` appears below.

## Linking your program

```bash
gcc  -std=c11   app.c   $(pkg-config --cflags --libs ocilib) -o app
g++  -std=c++17 app.cpp $(pkg-config --cflags --libs ocilib) -o app
```

At run time the loader must find **both** OCILIB's and Oracle's shared library
(`libocilib` and `libclntsh`) — set the loader variable for your platform
(`LD_LIBRARY_PATH` shown, `DYLD_LIBRARY_PATH` on macOS):

```bash
export LD_LIBRARY_PATH=/usr/local/lib:/opt/oracle/instantclient_23_x
```

## Try it yourself

1. `./configure --with-oracle-import=runtime && make` on a machine with **no** Oracle
   SDK — it still compiles; that's the runtime-import trick.
2. `pkg-config --modversion ocilib` after install.
3. Build `demo/conn.c` using `demo/Makefile_demo` as a template.

## Common pitfalls

- **`error while loading shared libraries: libocilib`** — `ldconfig` (Linux) or
  the platform loader variable (`LD_LIBRARY_PATH`, `DYLD_LIBRARY_PATH`,
  `LIBPATH`, `SHLIB_PATH`) missing.
- **`configure: OCI headers not found`** — `linkage` without SDK paths; switch to
  `runtime`.
- **Charset mismatch** — on Windows, a `wide` lib + `char` code compiles but
  strings garble; `otext`/`OTEXT()` keeps your source compatible with both.
  Off Windows there is only the MBCS build, so this can't happen.

## Recap

- `./configure && make && make install`; `runtime` import = zero Oracle SDK needed.
- Link with `-locilib`/pkg-config; keep both libraries reachable through your
  platform's loader variable.

**Next:** create your first OCILIB project and compile it on any OS.
