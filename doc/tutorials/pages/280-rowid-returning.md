---
title: ROWID and DML RETURNING
part: 2
level: intermediate
demos: [rowid.c, returning.c, returning.cpp]
---

# ROWID and DML `RETURNING`

**Objectives** — by the end of this page you will be able to:

- fetch and re-use a row's ROWID
- get back generated keys/values with `RETURNING … INTO`
- use `OCI_Register*` output placeholders correctly

## ROWID — the fastest row address

Every Oracle row has a physical `ROWID` — its on-disk address. Selecting it as a
string and binding it back is the fastest possible "find this exact row again":

:::sample
```c
otext rid[64];

OCI_Immediate(cn, OTEXT("select rowid from employees where empno = 7839"),
              OCI_ARG_TEXT, rid);            /* fetched as plain text */

OCI_Prepare(st, OTEXT("update employees set sal = sal + 100 where rowid = :rid"));
OCI_BindString(st, OTEXT(":rid"), rid, sizeof(rid) - 1);
OCI_Execute(st);
```

```cpp
st.Execute("select rowid from employees where empno = 7839");
std::string rid;
if (st.GetResultset().Next())
{
    rid = st.GetResultset().Get<String>(0);   /* ROWID arrives as a string */
}
```
:::

A `ROWID` column fetched in a resultset also arrives as a plain string.

## `RETURNING … INTO` — values back from DML

`INSERT/UPDATE/DELETE … RETURNING col INTO :v` returns post-change values (new keys, defaults, triggers' output) in **one** round-trip.

The targets are declared as **registered output placeholders** — and the values come back as a
**resultset** you fetch normally:

:::sample
```c
OCI_Prepare(st, OTEXT("update employees set sal = sal * 1.1 "
                      "returning empno, sal into :id, :new_sal"));

OCI_RegisterInt(st, OTEXT(":id"));
OCI_RegisterDouble(st, OTEXT(":new_sal"));

OCI_Execute(st);

OCI_Resultset *rs = OCI_GetResultset(st);      /* rows returned by RETURNING */
while (OCI_FetchNext(rs))
{
    printf("emp %d → new sal %.2f\n", OCI_GetInt(rs, 0), OCI_GetDouble(rs, 1));
}
```

```cpp
st.Prepare("update employees set sal = sal * 1.1 "
           "returning empno, sal into :id, :new_sal");
st.Register(":id");           // Register() returns BindInfo for the OUT target
st.Register(":new_sal");
st.ExecutePrepared();
for (auto &&row : st.GetResultset().Rows())
{
    std::cout << row.Get<int>(0) << " -> " << row.Get<double>(1) << "\n";
}
```
:::

## The register family — one per type

One `OCI_Register*` call per type — the same coverage as the bind family, but
declaring an **output** placeholder:

`OCI_Register{Short,Int,UnsignedShort,UnsignedInt,BigInt,UnsignedBigInt,Float,Double,
String,Raw,Number,Date,Timestamp,Interval,Lob,File,Object,Ref,Coll,Json,Vector}`

In C++ a single `st.Register(":name")` covers them — the returned `BindInfo` marks
the target.

Why not just an OUT bind? Because `RETURNING` can produce **multiple rows**
(`update … returning` fires once per affected row) — a resultset is the natural
container, so the API models it that way.

## Try it yourself

1. Insert a row `returning empno into :id` where `empno` comes from a trigger/sequence —
   read the generated key without a second query.
2. `delete from employees where deptno = :d returning ename into :n` — list who left.
3. Fetch a ROWID, update by it, and verify with `OCI_GetAffectedRows`.

## Common pitfalls

- **Using `OCI_BindXxx` + `OCI_BDM_OUT` for RETURNING** — it's `OCI_Register*`;
  direction-flag OUT binds are for PL/SQL parameters.
- **Forgetting the fetch loop** — registered values live in the resultset, not in
  your variable.
- **Expecting a single row** — `RETURNING` on multi-row DML returns one row **per**
  affected row.

## Recap

- `select rowid` → string; bind it back for pinpoint updates.
- `RETURNING … INTO :x` + `OCI_Register{Type}` + normal resultset fetch = generated
  values without extra queries.

**Next:** piecewise LONG handling with `OCI_Long`.
