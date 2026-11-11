---
title: REF CURSORs — resultsets as PL/SQL outputs
navtitle: REF CURSORs
part: 4
level: intermediate
demos: [cursor.c, cursor.cpp]
---

# REF CURSORs — resultsets as PL/SQL outputs

**Objectives** — by the end of this page you will be able to:

- receive a `REF CURSOR` OUT parameter as a fetchable resultset
- read *nested cursors* inside a SELECT
- tell the two cursor sources apart: a cursor *parameter* vs a cursor *column*

## The cursor statement

A `REF CURSOR` is Oracle's way of returning a resultset *through a parameter*.

On the client side it materializes as a second **statement** — you bind a prepared statement into the `:cur` placeholder, execute, and fetch its resultset like any SELECT.

Two different API calls feed you cursors, and confusing them is the classic mistake:

| C function | C++ equivalent | Source |
|------------|----------------|--------|
| `OCI_StatementBindCursor` | `Bind(":n", stmt)` | binds a statement as an OUT cursor parameter |
| `OCI_ResultsetGetCursor` | `Get<Statement>(i)` | fetches a `CURSOR` **column** as a nested statement |
| `OCI_BindGetStatement` | `BindInfo::GetStatement()` | recovers the bound statement from a bind object |

## Setup

```sql
create or replace procedure get_emps(p_deptno in number, p_cur out sys_refcursor) is
begin
    open p_cur for select empno, ename, sal from employees where deptno = p_deptno;
end;
/
```

## Binding a cursor OUT

A cursor parameter binds to a **statement handle** — after execute, that statement
carries the cursor's resultset:

:::sample
```c
OCI_Statement *cur_st = OCI_StatementCreate(cn);   /* the cursor lands here   */

OCI_StatementPrepare(st, OTEXT("begin get_emps(:d, :cur); end;"));
OCI_StatementBindInt32      (st, OTEXT(":d"),  &deptno);
OCI_StatementBindCursor(st, OTEXT(":cur"), cur_st);      /* declares OUT ref cursor */
OCI_StatementExecutePrepared(st);

OCI_Resultset *rs = OCI_StatementGetResultset(cur_st);      /* fetch like any select   */
while (OCI_ResultsetFetchNext(rs))
{
    printf("%d %s\n", OCI_ResultsetGetInt32(rs, 0), OCI_ResultsetGetString(rs, 1));
}

OCI_StatementFree(cur_st);
```

```cpp
Statement cur(con);
st.Prepare("begin get_emps(:d, :cur); end;");
st.Bind(":d", deptno, BindInfo::In);
st.BindStatement(":cur", cur);                     /* OUT cursor */
st.ExecutePrepared();
for (auto &&row : cur.GetResultset().Rows())
{
    std::cout << row.Get<int>(0) << " " << row.Get<String>(1) << "\n";
}
```
:::

## Nested cursors inside SELECT

Oracle can return a `CURSOR(…)` *column* — each row carries its own resultset, and
the statement-getter yields a nested statement you fetch through:

```sql
select rownum, cursor(select ename from employees where deptno = d.deptno)
  from departments d
```

:::sample
```c
while (OCI_ResultsetFetchNext(rs))
{
    OCI_Statement *inner = OCI_ResultsetGetCursor(rs, 1);   /* cursor column → stmt  */
    OCI_Resultset *rs2  = OCI_StatementGetResultset(inner);    /* borrowed both         */
    while (OCI_ResultsetFetchNext(rs2))
    {
        printf("  %s\n", OCI_ResultsetGetString(rs2, 0));
    }
}
```

```cpp
for (auto &&row : rs.Rows())
{
    Statement inner = row.Get<Statement>(1);
    for (auto &&r2 : inner.GetResultset().Rows())
    {
        std::cout << "  " << r2.Get<String>(0) << "\n";
    }
}
```
:::


## Putting it together — the typical sequence

Ref cursors are PL/SQL's way of returning a resultset:

1. **Declare a `SYS_REFCURSOR` OUT param** in the PL/SQL signature — the
   stored proc opens it, you fetch it.
2. **Bind the placeholder** — `StatementBindStatement` binds an `OCI_Statement`
   as the cursor slot (yes, a statement bound into another statement).
3. **Execute** — the block opens the cursor; your bound statement now holds
   the resultset.
4. **Fetch it** — `GetResultset` on the *bound* statement walks the rows like
   any SELECT.

Hints: the bound `OCI_Statement` becomes a live cursor after execute — you
fetch it, you don't prepare it; `SYS_REFCURSOR` is the generic type — a
typed `REF CURSOR` works the same; and you can chain cursors — one PL/SQL
call can return several resultsets via several OUT binds.

## Try it yourself

1. Call `get_emps` for each `deptno` in `departments` — reuse `cur_st` between calls.
2. Select a nested `CURSOR` of employees per department and print an indented tree.
3. Try `OCI_ResultsetGetCursorByName(rs, OTEXT("MYCURSOR"))` on an aliased cursor column.

## Common pitfalls

- **Freeing the cursor statement early** — the resultset dies with it; fetch first.
- **Rebinding the same cursor statement twice** — free/re-create (or let the API
  reset) when calling the proc repeatedly.
- **Confusing the two sources** — a cursor *bind* (parameter) vs a cursor *column*
  (nested statement from a row): see the table above.

## Recap

- Bind-a-statement = OUT ref-cursor parameter; fetch its resultset.
- A `CURSOR` column in a resultset yields a nested statement via the statement getter.

**Next:** implicit resultsets — multiple resultsets without ref cursor params.
