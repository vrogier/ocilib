---
title: Procedures, functions & OUT parameters
navtitle: Procedures & OUT params
part: 4
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

OCI_StatementPrepare(st, OTEXT("begin raise_sal(:empno, :pct, :new_sal); end;"));
OCI_StatementBindInt32   (st, OTEXT(":empno"),   &empno);
OCI_StatementBindDouble(st, OTEXT(":pct"),     &pct);
OCI_StatementBindDouble(st, OTEXT(":new_sal"), &new_sal);

OCI_BindSetDirection(OCI_StatementGetBindByName(st, OTEXT(":new_sal")), OCI_BDM_OUT);

OCI_StatementExecutePrepared(st);
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
OCI_StatementPrepare(st, OTEXT("begin :n := dept_headcount(:d); end;"));
OCI_StatementBindInt32(st, OTEXT(":n"), &n);
OCI_StatementBindInt32(st, OTEXT(":d"), &deptno);
OCI_BindSetDirection(OCI_StatementGetBindByName(st, OTEXT(":n")), OCI_BDM_OUT);
OCI_StatementExecutePrepared(st);
```

```cpp
int n = 0;
st.Prepare("begin :n := dept_headcount(:d); end;");
st.Bind(":n", n, BindInfo::Out);
st.Bind(":d", deptno);
st.ExecutePrepared();
```
:::

## Putting it together — the typical sequence

OUT parameters are where binds get interesting — the same call, two
directions:

1. **Declare a host variable** for the OUT slot — it must exist before the
   bind.
2. **Bind it as OUT** — `StatementBind*` on the placeholder; OCILIB doesn't
   need a direction flag, the PL/SQL signature decides.
3. **Execute** — the block writes into your variable.
4. **Read the variable** — it's now filled with whatever the PL/SQL assigned.

`IN OUT` is the same with a pre-seeded value: set before execute, read after.
The buffer rules matter most on OUT string/raw binds — pass capacity in via
`BindSetDataSize`, the server refuses to write more than you declared.

Hints: an OUT bind's variable must outlive the execute — stack-allocate it
before the call; `OUT VARCHAR2` needs `BindSetDataSize` or the server truncates
to a default; and `IN OUT` params are just `IN` + `OUT` on the same slot —
read the value after `Execute` exactly like an `OUT`.

## Try it yourself

1. Add an `IN OUT p_factor` parameter — pass a multiplier in, get the applied
   factor back.
2. Call `sys.dbms_lock.sleep(1)` through a block — then cancel it with `OCI_ConnectionBreak`
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
