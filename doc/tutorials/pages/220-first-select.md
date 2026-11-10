---
title: Running your first SELECT
part: 2
level: beginner
demos: [fetch.c, fetch.cpp]
sql: tutorial-schema.sql
---

# Running your first SELECT

**Objectives** — by the end of this page you will be able to:

- create an `OCI_Statement`, execute a `SELECT`, and walk the returned rows
- read column values with typed getters, by index and by name
- handle `NULL` values correctly
- use the C++ `Resultset` range-for loop

## Meet the resultset

Running a `SELECT` on a statement produces a **resultset** — the cursor over the
rows the server has waiting. Two facts define how you use it:

- it's **borrowed**: the resultset lives inside the statement — free the statement
  and the resultset dies with it;
- it's **forward-moving**: a fetch call advances to the next row, and the typed
  getters always read the *current* row's columns (0-based).

The read loop never changes:

```
Statement ──execute──> Resultset ──fetch──> row ──get──> column values
```

In C++ the same objects behave like a container: the resultset is an input range
you can walk with a range-`for`, and `row.Get<T>(i)` infers the right getter for
the type you ask for.

## Walkthrough

We query `employees` from `tutorial-schema.sql` and print a small report. Steps:

1. Create environment + connection (as in the previous lesson — elided below? No —
   the sample stays complete so it compiles as-is).
2. Create a statement, execute the SELECT.
3. Loop `OCI_FetchNext`, printing columns by index.
4. Use `OCI_GetColumnCount`/`OCI_IsNull` to be safe on the nullable `comm` column.

## Full example

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
    if (!env) return EXIT_FAILURE;

    OCI_Connection *cn = OCI_ConnectionCreate(env,
        getenv("OCILIB_DEMO_DB"), getenv("OCILIB_DEMO_USR"),
        getenv("OCILIB_DEMO_PWD"), OCI_SESSION_DEFAULT);

    OCI_Statement *st = OCI_StatementCreate(cn);

    OCI_ExecuteStmt(st, "select empno, ename, sal, comm from employees order by empno");

    OCI_Resultset *rs = OCI_GetResultset(st);
    while (OCI_FetchNext(rs))
    {
        printf("%5i %-10s %8.2f", OCI_GetInt(rs, 0), OCI_GetString(rs, 1),
               OCI_GetDouble(rs, 2));

        if (!OCI_IsNull(rs, 4))                 /* comm is nullable */
        {
            printf("  comm=%.2f", OCI_GetDouble(rs, 3));
        }

        printf("\n");
    }

    printf("=> %u row(s)\n", OCI_GetRowCount(rs));

    OCI_StatementFree(st);                      /* frees rs with it */
    OCI_ConnectionFree(cn);
    OCI_EnvironmentFree(env);
    return EXIT_SUCCESS;
}
```

```cpp
#include "ocilib.hpp"
#include <cstdlib>
#include <iostream>

using namespace ocilib;

int main()
{
    try
    {
        Environment env;
        Connection con(env, getenv("OCILIB_DEMO_DB"), getenv("OCILIB_DEMO_USR"),
                           getenv("OCILIB_DEMO_PWD"));

        Statement st(con);
        st.Execute("select empno, ename, sal, comm from employees order by empno");

        Resultset rs = st.GetResultset();
        for (auto &&row : rs.Rows())                   /* v5: range-for iterator */
        {
            std::cout << row.Get<int>(0) << " " << row.Get<String>(1)
                      << " " << row.Get<double>(2);

            auto comm = row.GetOptional<double>(4);   /* std::optional — NULL-safe */
            if (comm)
            {
                std::cout << "  comm=" << *comm;
            }

            std::cout << "\n";
        }
        std::cout << "=> " << rs.GetCount() << " row(s)\n";
    }
    catch (const std::exception &ex)
    {
        std::cout << ex.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
```
:::

```output
 7369 SMITH       800.00
 7499 ALLEN      1600.00  comm=300.00
 7521 WARD       1250.00  comm=500.00
 ...
=> 8 row(s)
```

## Try it yourself

1. Replace index `2` by the column name: `OCI_GetString(rs, OTEXT("ename"))` —
   getters accept names too (`rs.Get<String>("ename")` in C++).
2. Add `dname` via `join departments d using (deptno)` and print it.
3. Print `OCI_GetColumnCount(rs)` and loop columns generically by index.
4. In C++, try `while (rs++)` instead of the range-`for`.

## Common pitfalls

- **Reading before fetching** — getters only see the *current* row; call
  `OCI_FetchNext` first.
- **Using a 1-based index** — columns start at 0. `OCI_GetX(rs, 1)` reads the *second* column, not the first.
- **Getting a NULL `comm` as a number** — always test `OCI_IsNull` (C) or use
  `GetOptional<T>` (C++) on nullable columns.
- **`ORA-00942: table or view does not exist`** — you forgot to run
  [`assets/sql/tutorial-schema.sql`](assets/sql/tutorial-schema.sql) as the connecting user.

## Recap

- `ExecuteStmt` → `GetResultset` → `FetchNext` → typed getters is *the* OCILIB loop.
- Columns are 0-based; getters accept index or name.
- C++ adds `Get<T>` type inference, `GetOptional<T>` for NULLs, and iterators.

**Next:** the full getter table — every `OCI_Get*` function and the SQL type each one
maps to.
