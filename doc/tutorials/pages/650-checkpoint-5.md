---
title: Checkpoint 5 — org chart as objects
navtitle: Checkpoint 5 org chart
part: 6
level: advanced
---

# Checkpoint 5 — org chart as objects

You should now be able to describe schema objects, create and fill object types,
work collections and iterate them, and link objects with REFs.

## The exercise — object-relational org chart

**Step 1 — schema.** Model the running project with types:

```sql
create type emp_t      as object (empno number, ename varchar2(30),
                                  sal number, mgr ref emp_t);
create type emp_list_t as varray(50) of emp_t;
create type dept_t     as object (deptno number, dname varchar2(30),
                                  members emp_list_t);
create table emp_obj   of emp_t;
create table dept_obj  of dept_t;
```

**Step 2 — the program.**

1. Describe `dept_obj` via `OCI_TypeInfoGet` + `TypeInfoGetColumn` — print the
   attribute tree (members → varray → emp_t attrs).
2. Build one `dept_t` programmatically: `Deptno=20`, `Members` = a `emp_list_t`
   collection with two `emp_t` elements; bind it into `dept_obj`.
3. Fetch it back and walk `MEMBERS` with `OCI_Iter`, printing each `ENAME`/`SAL`.
4. For one member, set `mgr = OCI_ObjectGetSelfRef(other_emp_obj)` and update.
5. Dump everything once with `OCI_ObjectToText` for comparison.

## Acceptance checks

- `select * from dept_obj` shows the row; `select value(m) from dept_obj d,
   table(d.members) m` lists member attrs.
- Your program prints the same tree it inserted.
- No handle leaks: every `Get*` that yields a handle is freed.

## What it exercises

| Skill | Pages |
|-------|-------|
| TypeInfo describe | 610 |
| Object create/get/set, NULLs | 615–620 |
| Assign/inheritance/self-ref | 625/640 |
| Collections + `OCI_Iter` | 630/635 |
| End-to-end nested graph | 645 |

Part 6 done — the whole object-relational API is yours. Next: messaging — AQ and
change notification.
