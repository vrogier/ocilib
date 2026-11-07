---
title: Procedures, functions & OUT parameters
navtitle: Procedures & OUT params
part: 3
level: intermediate
---

# Procedures, functions & OUT parameters

**Objectives** — by the end of this page you will be able to:

- call stored procedures and functions with IN/OUT/IN OUT parameters
- control bind directions explicitly with `OCI_BindSetDirection`
- capture a function's return value

## Setup

```sql
create or replace procedure raise_sal
    (p_empno in number, p_pct in number, p_new_sal out number) is
begin
    update employees set sal = sal * (1 + p_pct/100)
     where empno = p_empno
    returning sal into p_new_sal;
end;
/
create or replace function dept_headcount(p_deptno in number) return number is
    n number;
begin
    select count(*) into n from employees where deptno = p_deptno;
    return n;
end;
/
```

## Calling a procedure — explicit directions

Params are ordinary binds; the direction flag (page 255) marks which ones the server
writes back. Find the bind by name and flip its direction to `OUT`:

:::sample
```c
OCI_Int32  empno = 7566;
OCI_Double pct = 10, new_sal = 0;

OCI_Prepare(st, OTEXT("begin raise_sal(:empno, :pct, :new_sal); end;"));
OCI_BindInt   (st, OTEXT(":empno"),   &empno);
OCI_BindDouble(st, OTEXT(":pct"),     &pct);
OCI_BindDouble(st, OTEXT(":new_sal"), &new_sal);

OCI_BindSetDirection(OCI_GetBindByName(st, OTEXT(":new_sal")), OCI_BDM_OUT);

OCI_Execute(st);
printf("new salary: %.2f\n", new_sal);
```

```cpp
double pct = 10, new_sal = 0;
st.Prepare("begin raise_sal(:empno, :pct, :new_sal); end;");
st.Bind(":empno",   empno,   BindInfo::In);
st.Bind(":pct",     pct,     BindInfo::In);
st.Bind(":new_sal", new_sal, BindInfo::Out);   /* direction right in the Bind call */
st.ExecutePrepared();
std::cout << new_sal << "\n";
```
:::

For `IN OUT` parameters use the `InOut`/`IN_OUT` direction — your variable is read
*and* written.

## Calling a function — `:return`

A function's return value is just one more placeholder, conventionally `:f` or
`:ret`, bound OUT:

:::sample
```c
OCI_Int32 n = 0;
OCI_Prepare(st, OTEXT("begin :n := dept_headcount(:d); end;"));
OCI_BindInt(st, OTEXT(":n"), &n);
OCI_BindInt(st, OTEXT(":d"), &deptno);
OCI_BindSetDirection(OCI_GetBindByName(st, OTEXT(":n")), OCI_BDM_OUT);
OCI_Execute(st);
```

```cpp
int n = 0;
st.Prepare("begin :n := dept_headcount(:d); end;");
st.Bind(":n", n, BindInfo::Out);
st.Bind(":d", deptno);
st.ExecutePrepared();
```
:::

## Try it yourself

1. Add an `IN OUT p_factor` parameter — pass a multiplier in, get the applied
   factor back.
2. Call `sys.dbms_lock.sleep(1)` through a block — then cancel it with `OCI_Break`
   from another thread (page 130 synergy).

## Common pitfalls

- **Missing OUT direction** — the placeholder keeps `IN` and your variable stays
  stale.
- **Positional vs named PL/SQL args** — binds always match by *name*, regardless of
  parameter order.
- **`no data found`** — OUT params stay untouched when the proc raises before
  assigning.

## Recap

- Procedures/functions run inside `begin … end;` — params are binds;
  `OCI_BindSetDirection` marks OUT/IN-OUT.

**Next:** `REF CURSOR` parameters — resultsets as PL/SQL outputs.
