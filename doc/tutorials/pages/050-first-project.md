---
title: Your first OCILIB project
part: 0
level: beginner
---

# Your first OCILIB project

**Objectives** — by the end of this page you will be able to:

- lay out a minimal C and C++ OCILIB application
- compile and link it with gcc, MSVC or CMake
- use `otext`/`OTEXT()` so your source works on ANSI *and* Unicode builds

## The skeleton

Every program in these tutorials has the same shape:

:::sample
```c
#include "ocilib.h"
#include <stdio.h>
#include <stdlib.h>

void err_handler(OCI_Error *err)
{
    printf("%s\n", OCI_ErrorGetString(err));
}

int main(void)
{
    OCI_Environment *env = OCI_EnvironmentCreate(err_handler, NULL, OCI_ENV_DEFAULT);
    if (!env)
    {
        return EXIT_FAILURE;
    }

    /* … your database work here … */

    OCI_EnvironmentFree(env);
    return EXIT_SUCCESS;
}
```

```cpp
#include "ocilib.hpp"
#include <iostream>

using namespace ocilib;

int main()
{
    try
    {
        Environment env;
        /* … your database work here … */
    }
    catch (const std::exception &ex)
    {
        std::cerr << ex.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
```
:::

One header per language — `#include "ocilib.h"` for C, `"ocilib.hpp"` for C++ (the C++
header is self-contained and pulls in the C header for you).

## Compile & link

Assume OCILIB installed in `/usr/local` (Linux) or built in `lib64\` with headers in
`include\` (Windows):

:::sample
```sh
# Linux / macOS — C
gcc  -std=c11   app.c   $(pkg-config --cflags --libs ocilib) -o app
# C++
g++  -std=c++17 app.cpp $(pkg-config --cflags --libs ocilib) -o app
```

```powershell
# Windows (x64 ANSI build of ocilib)
cl /std:c17   /I include app.c   /link lib64\ociliba.lib
cl /std:c++17 /I include app.cpp /link lib64\ociliba.lib
```

```cmake
# CMakeLists.txt — works on both OSes
cmake_minimum_required(VERSION 3.20)
project(ocilib_app C CXX)

find_library(OCILIB_LIB NAMES ociliba ocilib
             PATHS ${OCILIB_ROOT}/lib64 /usr/local/lib)
find_path(OCILIB_INC NAMES ocilib.h
          PATHS ${OCILIB_ROOT}/include /usr/local/include)

add_executable(app app.c)          # or app.cpp
target_include_directories(app PRIVATE ${OCILIB_INC})
target_link_libraries(app PRIVATE ${OCILIB_LIB})
```
:::

## Strings the OCILIB way: `otext` and `OTEXT()`

Because the same code must compile against ANSI (`char`) and Unicode (`unsigned short`)
builds, OCILIB never uses plain string literals for API calls:

- **`otext`** — the character type of the build (`char` or `unsigned short`)
- **`OTEXT("…")`** — a literal of the right kind, everywhere the API takes a string
- **`ocilib::String`** — C++ string type (`std::string` or `std::wstring`)

```c
OCI_ExecuteStmt(st, OTEXT("select ename from employees"));   /* portable */
OCI_ExecuteStmt(st, "select ename from employees");          /* ANSI builds only */
```

Plain `const char*` literals happen to compile on ANSI builds, and C++ samples accept
`const char*` too — but get in the `OTEXT()` habit early; it is free and portable.

## Try it yourself

1. Copy the C skeleton into `app.c`, compile, run — it should exit silently with code 0.
2. Deliberately remove the Instant Client from `PATH`/`LD_LIBRARY_PATH` and run again:
   `OCI_EnvironmentCreate` returns NULL / throws — that is your error channel working.
3. Add `-Wall -Wextra` (gcc/clang) or `/W4` (MSVC) — the OCILIB headers are warning-clean.

## Common pitfalls

- **Linking the wrong lib name on Windows** — `ociliba.lib` (ANSI) vs `ocilibw.lib`
  (UNICODE); they are not interchangeable.
- **`fatal error: ocilib.h: No such file`** — include dir missing; point at the repo's
  `include/` or the install `include/`.
- **It builds but does nothing** — correct! The skeleton only creates and frees the
  environment. The next page makes it talk to Oracle.

## Recap

- One include, one env object, try/catch (C++) or NULL check (C).
- `pkg-config --cflags --libs ocilib` on Linux; `ociliba.lib`/`ocilibw.lib` on Windows.
- `otext`/`OTEXT()`/`ocilib::String` keep your code charset-agnostic.

**Next:** make the environment actually *do* something — error handlers and env flags.
