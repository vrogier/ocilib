---
title: Managing binds — direction, NULLs, rebinding
navtitle: Managing binds
part: 3
level: intermediate
---

# Managing binds — direction, NULLs, rebinding

**Objectives** — by the end of this page you will be able to:

- mark binds IN / OUT / IN-OUT
- set and test NULL flags — per bind or per array position
- inspect bound variables after execution
- control when OCILIB may re-bind

## Directions

Every bind has a direction — input by default, switched when a placeholder must
return a value:

| C enum | C++ enum | Meaning | Typical use |
|--------|----------|---------|-------------|
| `OCI_BDM_IN` | `BindInfo::In` | value goes in | `where x = :v` |
| `OCI_BDM_OUT` | `BindInfo::Out` | value comes back | PL/SQL `OUT` parameter |
| `OCI_BDM_IN_OUT` | `BindInfo::InOut` | both | PL/SQL `IN OUT` parameter |

!!! note "DML `RETURNING` is different"
    `insert … returning col into :v` does **not** use direction flags — it uses
    `OCI_StatementRegister*` output placeholders whose values come back as a resultset.
    That's page 280; keep `OUT` for PL/SQL parameters.

:::sample
```c
OCI_Bind *b = OCI_StatementGetBindByName(st, OTEXT(":new_sal"));
OCI_BindSetDirection(b, OCI_BDM_OUT);
/* …after Execute… */
OCI_Double *out = OCI_BindGetData(b);        /* the returned value */
```

```cpp
BindInfo b = st.Bind(":new_sal", out, BindInfo::Out);
/* …after Execute… */                        /* out now holds the value */
```
:::

## NULL flags

Oracle NULL travels via an **indicator**, not the value — each bind carries a null
flag you set and test, with per-position variants because array binds (page 810)
have one flag per row:

| C function | C++ method | Role |
|------------|------------|------|
| `OCI_BindSetNull` | `BindInfo::SetDataNull(true)` | mark the bind NULL |
| `OCI_BindSetNotNull` | `BindInfo::SetDataNull(false)` | back to NOT NULL |
| `OCI_BindIsNull` | `IsDataNull()` | test the flag |
| `OCI_BindSetNullAtPos` | `SetDataNull(true, i)` | position `i` of an array bind |
| `OCI_BindSetNotNullAtPos` | `SetDataNull(false, i)` | same, back to NOT NULL |
| `OCI_BindIsNullAtPos` | `IsDataNull(i)` | test position `i` |

## Inspecting binds

The statement knows every bind it holds — enumerate them, and each bind answers its
own metadata:

| C function | C++ method | Returns |
|------------|------------|---------|
| `OCI_StatementGetBindCount` | `GetBindCount()` | how many binds |
| `OCI_StatementGetBind` | `GetBind(index)` | bind by position |
| `OCI_StatementGetBindByName` | `GetBind(name)` | bind by `:name` |
| `OCI_StatementGetBindIndex` | `GetBindIndex(name)` | position of a named bind |
| `OCI_BindGetName` | `BindInfo::GetName()` | `":name"` |
| `OCI_BindGetType` | `BindInfo::GetType()` | bind kind |
| `OCI_BindGetSubtype` | `GetSubType()` | subtype detail |
| `OCI_BindGetData` | `GetData*()` accessors | the host buffer |
| `OCI_BindGetDataCount` | `GetDataCount()` | element count |
| `OCI_BindGetDataSize` | `GetDataSize()` | buffer size |
| `OCI_BindGetDataSizeAtPos` | `GetDataSize(i)` | size at array position |
| `OCI_BindGetDirection` | `GetDirection()` | current direction |
| `OCI_BindGetAllocationMode` | `GetAllocationMode()` | your buffer vs internal |
| `OCI_BindGetStatement` | `GetStatement()` | statement handle for statement binds |
| `OCI_StatementGetBindAllocation` | `GetBindAllocation()` | statement-level buffer policy |
| `OCI_StatementGetBindMode` | `GetBindMode()` | statement-level bind policy |

## Rebinding — when the statement re-attaches your variables

Normally you bind once and re-execute many times. But if you re-prepare *different*
SQL on the same statement, or need new buffers, rebinding policy matters:

- **Allow rebinding** — let the next bind call replace an existing bind of the same
  name instead of erroring (`OCI_StatementAllowRebinding`; `IsRebindingAllowed` queries it).
- **Buffer allocation mode** — switch between your buffers (`OCI_BAM_EXTERNAL`, the
  default) and internally-owned buffers (`OCI_BAM_INTERNAL`) for binds the library
  allocates itself, e.g. `RETURNING` targets.

## Putting it together — the typical sequence

Bind management covers the bookkeeping around a bound statement:

1. **Inspect** — `OCI_StatementGetBindCount`, `GetBind`, `GetBindByName`,
   `GetBindIndex` — asking "what placeholders are live?".
2. **Adjust** — `BindSetDataSize`, `BindSetCharsetForm`, `BindSetDirection` —
   per-bind tweaks after creation.
3. **Rebind** — `OCI_StatementBindAgain` re-binds a different variable to the
   same placeholder — cheaper than unbind+rebind when the loop variable
   changes.
4. **Clean** — binds die with the statement; `OCI_StatementFree` releases them
   all.

Hints: `BindSetDataSize` matters for `OUT` binds — the server needs to know
your buffer's capacity before it writes into it; `GetBindIndex` is the
debugging tool for "which bind is the third `:x`?"; and direction (`IN`/`OUT`/
`IN OUT`) is a bind-time hint the server checks at execute — get it right
there, not in your app code.

## Try it yourself

1. Call a PL/SQL proc with an `OUT` param — mark the bind `OCI_BDM_OUT`, execute,
   read `OCI_BindGetData`.
2. Insert a NULL `comm` via `OCI_BindSetNull` then `SetNotNull` on the next execute.
3. Print `OCI_BindGetName`/`GetType`/`GetDirection` for every bind of a prepared stmt.

## Common pitfalls

- **Forgetting `OCI_BDM_OUT`** — value stays unwritten and you read stale data.
- **Reading `GetData` before `Execute`** — OUT buffers fill *during* execution.
- **`SetNull` on a value you then forgot** — the flag persists across executes.

## Recap

- `OCI_BDM_IN/OUT/IN_OUT` on the `OCI_Bind` object; NULL via `BindSetNull{,AtPos}`.
- `OCI_StatementGetBind{,ByName,Index,Count}` enumerate; `BindGet*` inspect.

**Next:** prepare-once/execute-many in practice — the re-execution pattern.
