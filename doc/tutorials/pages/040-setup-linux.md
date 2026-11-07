---
title: Developing on Linux
part: 0
level: beginner
---

# Developing on Linux

**Objectives** — by the end of this page you will be able to:

- build and install OCILIB with autotools
- choose linkage vs runtime Oracle import, ANSI vs wide charset, static vs shared
- link your program with `-locilib` or pkg-config

## The short version

```bash
./configure
make
sudo make install
```

That builds `libocilib.so` (ANSI, **linkage** import, shared) and installs it to
`/usr/local` — headers in `include/`, library + `ocilib.pc` in `lib/`. The default
**linkage** mode needs the Oracle Instant Client **SDK** (headers + `libclntsh.so`)
reachable at configure time — pass `--with-oci-include`/`--with-oci-lib` if they're
not in the standard locations.

## Choosing your build

| Option | Values | Notes |
|--------|--------|-------|
| `--with-oracle-import` | `linkage` \| `runtime` | **`linkage` is the default** (needs Instant Client + SDK at build time). `runtime` is opt-in: OCILIB `dlopen`s `libclntsh.so` itself — builds with **no Oracle SDK** (headers vendored in `src/oci/`). |
| `--with-oci-include` / `--with-oci-lib` | paths | for `linkage` in non-standard locations |
| `--with-oracle-charset` | `ansi` \| `wide` | `ansi` = `char` (start here); `wide` = UTF-16 |
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

After `make install`, run `sudo ldconfig` so the loader sees the new library.

## Linking your program

```bash
gcc  -std=c11   app.c   $(pkg-config --cflags --libs ocilib) -o app
g++  -std=c++17 app.cpp $(pkg-config --cflags --libs ocilib) -o app
```

At run time the loader must find **both** `libocilib.so` and `libclntsh.so`:

```bash
export LD_LIBRARY_PATH=/usr/local/lib:/opt/oracle/instantclient_23_x
```

## Try it yourself

1. `./configure --with-oracle-import=runtime && make` on a machine with **no** Oracle
   SDK — it still compiles; that's the runtime-import trick.
2. `pkg-config --modversion ocilib` after install.
3. Build `demo/conn.c` using `demo/Makefile_demo` as a template.

## Common pitfalls

- **`error while loading shared libraries: libocilib.so`** — `ldconfig` or
  `LD_LIBRARY_PATH` missing.
- **`configure: OCI headers not found`** — `linkage` without SDK paths; switch to
  `runtime`.
- **Charset mismatch** — `wide` lib + `char` code compiles but strings garble;
  `otext`/`OTEXT()` keeps your source compatible with both.

## Recap

- `./configure && make && make install`; `runtime` import = zero Oracle SDK needed.
- Link with `-locilib`/pkg-config; keep both `.so`s on `LD_LIBRARY_PATH`.

**Next:** create your first OCILIB project and compile it on either OS.
