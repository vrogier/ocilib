---
title: Running your first SELECT
part: 3
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

## The resultset

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
3. Loop `OCI_ResultsetFetchNext`, printing columns by index.
4. Use `OCI_ResultsetGetColumnCount`/`OCI_ResultsetIsColumnNull` to be safe on the nullable `comm` column.

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

    OCI_StatementExecute(st, "select empno, ename, sal, comm from employees order by empno");

    OCI_Resultset *rs = OCI_StatementGetResultset(st);
    while (OCI_ResultsetFetchNext(rs))
    {
        printf("%5i %-10s %8.2f", OCI_ResultsetGetInt32(rs, 0), OCI_ResultsetGetString(rs, 1),
               OCI_ResultsetGetDouble(rs, 2));

        if (!OCI_ResultsetIsColumnNull(rs, 4))                 /* comm is nullable */
        {
            printf("  comm=%.2f", OCI_ResultsetGetDouble(rs, 3));
        }

        printf("\n");
    }

    printf("=> %u row(s)\n", OCI_ResultsetGetRowCount(rs));

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

## Putting it together — the typical sequence

Your first SELECT is the template every query follows:

1. **Prepare** the SQL — one call.
2. **Execute** — the statement now holds an internal cursor.
3. **Get the resultset** — `OCI_StatementGetResultset` hands you the cursor.
4. **Fetch rows** — `OCI_ResultsetFetchNext` walks forward, one row at a time,
   FALSE at EOF.
5. **Get values** — `OCI_ResultsetGet*` by index (0-based in the SELECT list)
   or by name.
6. **Free** — the statement owns the resultset; freeing it kills both.

The one-call variants (`ExecuteStmt`, `ImmediateFormat`) collapse steps 1–4 for
queries you run once — reach for them when you don't need to re-execute.

Hints: indexes are **0-based** — `GetX(rs, 0)` is the *first* column; `IsColumnNull` before
reading nullable columns, or you'll dereference borrowed NULL; and always
check `FetchNext` — an empty resultset exits the loop silently.

## Try it yourself

1. Replace index `2` by the column name: `OCI_ResultsetGetString(rs, OTEXT("ename"))` —
   getters accept names too (`rs.Get<String>("ename")` in C++).
2. Add `dname` via `join departments d using (deptno)` and print it.
3. Print `OCI_ResultsetGetColumnCount(rs)` and loop columns generically by index.
4. In C++, try `while (rs++)` instead of the range-`for`.

## Common pitfalls

- **Reading before fetching** — getters only see the *current* row; call
  `OCI_ResultsetFetchNext` first.
- **Using a 1-based index** — columns start at 0. `OCI_ResultsetGetXxx(rs, 1)` reads the *second* column, not the first.
- **Getting a NULL `comm` as a number** — always test `OCI_ResultsetIsColumnNull` (C) or use
  `GetOptional<T>` (C++) on nullable columns.
- **`ORA-00942: table or view does not exist`** — you forgot to run
  [`assets/sql/tutorial-schema.sql`](assets/sql/tutorial-schema.sql) as the connecting user.

## Recap

- `ExecuteStmt` → `GetResultset` → `FetchNext` → typed getters is *the* OCILIB loop.
- Columns are 0-based; getters accept index or name.
- C++ adds `Get<T>` type inference, `GetOptional<T>` for NULLs, and iterators.

**Next:** the full getter table — every `OCI_Get*` function and the SQL type each one
maps to.
