---
title: Prepare once, execute many
part: 2
level: beginner
---

# Prepare once, execute many

**Objectives** — by the end of this page you will be able to:

- use the prepare → bind → execute-in-a-loop pattern correctly
- update host variables between executions
- know when re-binding is required vs wasteful

## The canonical loop

:::sample
```c
OCI_Statement *st = OCI_StatementCreate(cn);
OCI_Prepare(st, OTEXT("insert into employees(empno, ename, deptno) "
                      "values(:n, :e, :d)"));

int   empno;
otext ename[31];
int   deptno = 20;

OCI_BindInt   (st, OTEXT(":n"), &empno);
OCI_BindString(st, OTEXT(":e"), ename, sizeof(ename) - 1);
OCI_BindInt   (st, OTEXT(":d"), &deptno);

static const char *names[] = {"ADAMS", "BAKER", "COSTA", NULL};
for (int i = 1000, j = 0; names[j]; i++, j++)
{
    empno = i;
    /* copy name into ename[] */

    OCI_Execute(st);          /* binds are re-read; nothing re-parsed */
}
```

```cpp
st.Prepare("insert into employees(empno, ename, deptno) values(:n, :e, :d)");

int empno;
std::string ename;
int deptno = 20;
st.Bind(":n", empno); st.Bind(":e", ename); st.Bind(":d", deptno);

for (int i = 1000, j = 0; names[j]; i++, j++)
{
    empno = i;  ename = names[j];
    st.ExecutePrepared();              /* binds re-read in place */
}
```
:::

Three things make this fast: one parse, one network protocol for executes, and bound
variables re-read **in place**.

## Changing values between executions

- **Same buffer, new value** — just write into the bound variable (as above).
- **New buffer entirely** — re-call the bind with the new pointer/variable.
- **NULL in the middle** — flip the bind's null flag for the next execute; reset it
  after (flags persist — page 255).

## When you *must* rebind

- Different SQL text (prepare again) — binds belong to the prepared statement.
- Different buffer size (string outgrew the original capacity).
- Direction changes (page 255).

## Measuring the win

Prepare+execute×N vs one-shot×N on 1000 inserts is typically a **3–10×** round-trip
reduction — and pairs naturally with the statement cache (page 120) so even repeated
prepares stay cheap across statements.

## Try it yourself

1. Time 500 inserts done with `OCI_ExecuteStmt` vs `Prepare`+`Execute` loop.
2. Change `deptno` inside the loop without rebinding; verify in SQL*Plus.
3. Insert a NULL `comm` for every third row via `OCI_BindSetNullAtPos`.

## Common pitfalls

- **Rebinding identical pointers each iteration** — legal but pointless; bind once.
- **Forgetting `OCI_Execute` returns the row-affected status** — check it per iteration.
- **Autocommit on** — each `Execute` commits; for bulk inserts prefer manual commit
  once at the end (Part 4).

## Recap

- `Prepare` once, `Bind` once, `Execute` per row, commit once.
- Update values in place; rebind only when pointers/sizes/directions change.

**Next:** describing SQL without executing it.
