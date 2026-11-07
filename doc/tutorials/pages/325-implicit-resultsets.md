---
title: Implicit resultsets (12c+)
part: 3
level: intermediate
oracle: "12.1"
demos: [implicit_resultset.c, implicit_resultset.cpp]
---

# Implicit resultsets (12c+)

**Objectives** — by the end of this page you will be able to:

- consume multiple resultsets returned by `DBMS_SQL.RETURN_RESULT`
- walk them with `OCI_GetNextResultset`
- know when implicit resultsets beat ref-cursor OUT params

## Concepts

Since Oracle 12c, a PL/SQL block can return result sets **without declaring them as
parameters** — `DBMS_SQL.RETURN_RESULT` pushes cursors the client picks up
implicitly:

```sql
declare
    c1 sys_refcursor;
    c2 sys_refcursor;
begin
    open c1 for select ename from employees;
    dbms_sql.return_result(c1);
    open c2 for select dname from departments;
    dbms_sql.return_result(c2);
end;
```

After the block executes, the statement's resultset is the **first** pushed cursor;
asking for the *next* resultset advances through the queue until none remain:

:::sample
```c
OCI_ExecuteStmt(st, OTEXT(
    "declare "
    "  c1 sys_refcursor; "
    "  c2 sys_refcursor; "
    "begin "
    "  open c1 for select ename from employees where deptno = 20; "
    "  dbms_sql.return_result(c1); "
    "  open c2 for select dname from departments; "
    "  dbms_sql.return_result(c2); "
    "end;"));

OCI_Resultset *rs = OCI_GetResultset(st);
while (rs)
{
    while (OCI_FetchNext(rs))
    {
        printf("%s\n", OCI_GetString(rs, 1));
    }
    printf("---\n");
    rs = OCI_GetNextResultset(st);            /* NULL when no more */
}
```

```cpp
st.Execute("declare c sys_refcursor; begin "
           "  open c for select ename from employees; "
           "  dbms_sql.return_result(c); end;");
for (Resultset rs = st.GetResultset(); rs; rs = st.GetNextResultset())
    for (auto &&row : rs.Rows())
    {
        std::cout << row.Get<String>(1) << "\n";
    }
```
:::

## Implicit vs REF CURSOR param

| | Implicit (`RETURN_RESULT`) | `sys_refcursor` OUT |
|---|---|---|
| PL/SQL signature | none needed | `out sys_refcursor` param |
| Client call | `GetResultset` + `GetNextResultset` | bind-a-statement (`Bind`/`BindStatement`) |
| Multiple sets | natural — a queue of cursors | one per OUT param |
| Server version | ≥ 12c | any |

Choose implicit for "return N lists" and 12c+ deployments; ref cursors when the
procedure signature must be explicit or for older servers.

## Try it yourself

1. Return 3 implicit resultsets and print the count you consumed.
2. Mix `return_result` with an OUT scalar bind in the same block.

## Common pitfalls

- **Re-reading after advancing** — each next-resultset step moves forward; you can't
  go back (scrollable mode can't fix that either).
- **Assuming every driver version supports it** — needs Oracle client + server ≥ 12c.

## Recap

- `dbms_sql.return_result` → get the resultset → get-next until exhausted.
- Zero parameter ceremony, multi-resultset-friendly, Oracle ≥ 12c.

**Next:** index-by table parameters — arrays into and out of PL/SQL.
