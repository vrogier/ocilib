---
title: Anonymous PL/SQL blocks
part: 3
level: beginner
demos: [plsql_block.c]
---

# Anonymous PL/SQL blocks

**Objectives** — by the end of this page you will be able to:

- run `BEGIN … END;` blocks through a normal statement
- bind values into *and out of* a block
- read block errors precisely

## Concepts

An anonymous block is just SQL text to OCILIB — prepare, bind, execute, like
everything else. The magic is that **bind placeholders work both ways** inside PL/SQL:

:::sample
```c
OCI_Int32 res = 0;

OCI_Prepare(st, OTEXT("begin :res := trunc(sysdate+1) - trunc(sysdate-1); end;"));
OCI_BindInt(st, OTEXT(":res"), &res);       /* assignment target → OUT automatically */
OCI_Execute(st);

printf("res = %d\n", res);                   /* 2 — written by the block */
```

```cpp
int res = 0;
st.Prepare("begin :res := trunc(sysdate+1) - trunc(sysdate-1); end;");
st.Bind(":res", res, BindInfo::Out);          /* assignment target → OUT */
st.ExecutePrepared();
std::cout << res << "\n";
```
:::

A placeholder on the **left of `:=`** acts as OUT — Oracle writes into your variable
during execute. Placeholders used as values act as IN.

:::sample
```c
int days = 30, result = 0;
OCI_Prepare(st, OTEXT("begin :out := :days * 2; end;"));
OCI_BindInt(st, OTEXT(":days"), &days);      /* IN  */
OCI_BindInt(st, OTEXT(":out"),  &result);    /* OUT — filled after Execute */
OCI_Execute(st);
```

```cpp
int days = 30, result = 0;
st.Prepare("begin :out := :days * 2; end;");
st.Bind(":days", days);
st.Bind(":out",  result, BindInfo::InOut);   /* written by the block */
st.ExecutePrepared();
```
:::

## Why blocks instead of single SQL?

- Several statements in one round-trip (`insert` + `update` + counter).
- Conditional logic server-side (`if … then …`).
- Calling package procedures without a wrapper query.

```sql
begin
    update employees set sal = sal * 1.1 where deptno = :d;
    :rows := sql%rowcount;
end;
```

## Errors in blocks

PL/SQL exceptions surface as regular errors — `ORA-06550` (compile),
`ORA-01403`/`ORA-06502` at runtime — with the statement's error-position field
pointing into your block text when the error is syntactic.

## Try it yourself

1. Run the `trunc(sysdate)` sample and print `res`.
2. Write a block taking `:empno` IN and producing `:ename` and `:sal` OUT.
3. Raise `raise_application_error(-20001, 'boom')` inside a block and catch it.

## Common pitfalls

- **Forgetting `;` inside the block** — PL/SQL needs its own terminators.
- **Binding literals to OUT targets** — OUT placeholders must be variables.
- **SQL%ROWCOUNT in SQL, not PL/SQL** — `%rowcount` only exists inside blocks.

## Recap

- Blocks are normal statements; `:var` targets of `:=` come back OUT.
- One execute = one round-trip for arbitrarily complex server-side logic.

**Next:** calling real procedures & functions with OUT/IN-OUT parameters.
