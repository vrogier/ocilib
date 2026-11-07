---
title: Checkpoint 2 — payroll procedure
navtitle: Checkpoint 2 payroll
part: 3
level: intermediate
---

# Checkpoint 2 — payroll procedure

You should now be able to run anonymous blocks, call procedures with IN/OUT/IN OUT
params, consume ref cursors and implicit resultsets, pass PL/SQL tables, and read
`DBMS_OUTPUT`.

## The exercise

**Step 1 — build the server side.** Create this package in your schema:

```sql
create or replace package emp_payroll as
    procedure apply_raise(p_deptno  in  number,
                          p_pct     in  number,
                          p_count   out number,
                          p_list    out sys_refcursor);
end;
/
create or replace package body emp_payroll as
    procedure apply_raise(p_deptno  in  number,
                          p_pct     in  number,
                          p_count   out number,
                          p_list    out sys_refcursor) is
    begin
        update employees set sal = sal * (1 + p_pct/100) where deptno = p_deptno;
        p_count := sql%rowcount;
        dbms_output.put_line('apply_raise: ' || p_count || ' row(s) updated');
        open p_list for
            select empno, ename, sal from employees where deptno = p_deptno;
    end;
end;
/
```

**Step 2 — the client.** Write a program that:

1. Connects and calls `emp_payroll.apply_raise(20, 7.5, …)`:
   - `p_deptno`/`p_pct` IN binds, `p_count` OUT bind (direction!),
   - `p_list` via `OCI_BindStatement` onto a second statement handle.
2. Prints `p_count`, then iterates the returned cursor.
3. Enables server output first and prints the `put_line` trace at the end.

**Stretch:**

- Add a `p_names in t_tab` index-by parameter listing names to *exclude* — bind a
  `OCI_BindArrayOfStrings` for it.
- Re-implement `p_list` with `dbms_sql.return_result` inside the proc and compare
  the client code (`GetNextResultset` vs `BindStatement`).

## What it exercises

| Skill | Page |
|-------|------|
| Anonymous blocks / proc calls | 310/315 |
| OUT direction flags | 255/315 |
| Ref cursor OUT → resultset | 320 |
| Implicit resultsets | 325 |
| PL/SQL tables | 330 |
| `DBMS_OUTPUT` | 335 |

When `apply_raise` prints the trace line, the count, and the updated salary list —
Part 3 is done. Next: transactions, pools and threads.
