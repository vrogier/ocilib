# OCILIB v4 → v5 Migration Guide — C API

**Audience:** applications using the OCILIB **C** library (`ocilib.h`).
For the C++ layer (`ocilib.hpp`), see `migration-guide-cpp-api.md`.

**Status:** draft for v5.0.0 — maintained in `work-v5/` until release; update as the API settles.

> **Tip for automated migration:** this guide is written so it can be fed to an agentic
> coding tool. Every change is stated as a precise find → replace rule with exact
> signatures. Compile after each step; the compiler will surface every call site
> that still needs updating.

> **How to read this guide:** changes are ordered from most impactful to least.
> Steps 1–2 touch every program (compile-time). Steps 3–4 are the dangerous ones:
> they compile silently but change runtime behaviour. Steps 5–8 are mechanical,
> compiler-flagged renames. Steps 9–15 only affect users of specific features.

> **The big renaming:** v5 normalizes the entire public C API on the
> `OCI_<Type><Action>` handle model — ~460 functions, 11 handle typedefs and 12
> integer-type constants were renamed (e.g. `OCI_Execute` →
> `OCI_StatementExecutePrepared`, `OCI_Coll` → `OCI_Collection`,
> `OCI_NUM_INT` → `OCI_NUM_INT32`). `include/ocilibc/compat.h` maps every old
> name to its new one, so most v4 code still compiles as-is; define
> `OCI_NO_COMPAT` before including `ocilib.h` to disable the aliases and let the
> compiler flag every old name. The complete v4 → v5 table is in
> [Appendix A](#appendix-a--renamed-symbols-v4--v5).

---

## 1. Breaking changes at a glance

| # | Change | Effort |
|---|--------|--------|
| 1 | `OCI_Initialize`/`OCI_Cleanup` → `OCI_EnvironmentCreate`/`OCI_EnvironmentFree` | Low |
| 2 | Many top-level calls now take an `OCI_Environment*` first argument | Medium |
| 3 | All index parameters switch from 1-based to **0-based**; index lookups return `OCI_Int32` with `-1` = miss | Medium–High (runtime, not compile-time) |
| 4 | Unchanged signatures, changed behaviour (last-error scope, notifications, bind index) | Medium (runtime, not compile-time) |
| 5 | Error model split into error type + category + unified code | Low |
| 6 | `boolean` → `OCI_Boolean`; fixed-width `OCI_Int*`/`OCI_UInt*`/`OCI_Size`/`OCI_Float`/`OCI_Double` typedefs | Low (mechanical) |
| 7 | `*2` suffixes renamed `*ByName`; LOB read/write/append signatures; `OCI_CollGetElem2` removed | Low (mechanical) |
| 8 | `OCI_Bind{Type}` / `OCI_BindArrayOf{Type}` return `OCI_Bind*` instead of `OCI_Boolean` | Low |
| 9 | Subscription port moved to the environment; `OCI_SubscriptionRegister` gained `mode`; `OCI_DequeueSubscribe` is now a batch call | Low–Medium |
| 10 | Threading API removed (`OCI_Mutex`/`OCI_Thread`/`OCI_ThreadKey`) | Low–High (depends on usage) |
| 11 | Raw `OCI_HandleGet*` accessors removed | Low–Medium |
| 12 | XA / `OCI_SESSION_XA` removed | Low |
| 13 | `compat.h` recreated with v4→v5 aliases — v3-era macros are gone | Low–Medium |
| 14 | `OCI_GetAllocatedBytes` removed; use `OCI_GetStats(env, OCI_UST_BYTES)` | Trivial |
| 15 | New typed typedefs replace bare `unsigned int` parameters | None (source-compatible) |
| 16 | Charset build flag `OCI_CHARSET_ANSI` → `OCI_CHARSET_MBCS` (binaries `ociliba`/`ocilibm` → `ocilib` for MBCS, `ocilibw` for wide; configs `Release - ANSI` → `Release - MBCS`) | Trivial (old names still work) |
| 17 | `OCI_CHARSET_WIDE` is now **Windows-only** — it is a compile-time error on Unix/macOS, and `configure --with-oracle-charset` is removed (Unix builds are always MBCS) | High if you built OCILIB wide on Unix (switch to MBCS + `NLS_LANG=…AL32UTF8`) |

**Overall estimate:** a typical application (connect → statement → fetch) migrates in
**under an hour** — mostly mechanical edits caught by the compiler. Applications that
index into resultsets/collections (Step 3), use the removed threading API, XA, or
raw OCI handles need **real porting work** (see §4, §11, §12, §13).

---

## 2. Step 1 — Library lifecycle (the biggest change)

v4 had a single hidden global library state initialized once per process. v5 makes the
library state an explicit, public **`OCI_Environment`** handle. You create it, pass it
around, and free it.

### v4

```c
boolean err_handler(OCI_Error *err) { /* ... */ }

OCI_Initialize(err_handler, NULL, OCI_ENV_DEFAULT | OCI_ENV_CONTEXT);
/* ... work ... */
OCI_Cleanup();
```

### v5

```c
void err_handler(OCI_Error *err) { /* ... */ }

OCI_Environment *env = OCI_EnvironmentCreate(err_handler, NULL, OCI_ENV_DEFAULT);
/* ... work ... */
OCI_EnvironmentFree(env);
```

### Exact rules

| v4 | v5 |
|----|----|
| `OCI_Initialize(POCI_ERROR, const otext*, unsigned int mode)` | `OCI_Environment* OCI_EnvironmentCreate(POCI_ERROR, const otext*, OCI_EnvMode mode)` — **returns a handle**, not a boolean. Check `env != NULL` instead of `== TRUE`. |
| `OCI_Cleanup(void)` | `OCI_Boolean OCI_EnvironmentFree(OCI_Environment *env)` — takes the handle. |
| `OCI_ENV_CONTEXT` flag | **Removed** — thread-local error storage is always on. Just delete the flag. |

> **Behaviour notes**
> - `OCI_EnvironmentCreate` should be called **once per application** (as before).
> - `OCI_EnvironmentFree` automatically releases any objects not explicitly freed
>   (connections, pools, statements, type info, standalone datatype objects).
> - A failed create returns `NULL`. A failed free returns `FALSE` — typically because
>   objects are still alive or it was called twice.
> - Error-handler signature: `POCI_ERROR` is unchanged — `void (*)(OCI_Error *err)`.

---

## 3. Step 2 — Thread `OCI_Environment*` through the API

Once you hold an `env`, a large group of functions take it as their **first parameter**.
The rule of thumb:

> **Pass `env` to any function that does not already receive a "parent" handle**
> (`OCI_Connection*`, `OCI_TypeInfo*`, `OCI_Statement*`, `OCI_Pool*`, …).
> Objects that belong to another handle keep their old signature.

### 3.1 Functions that gained `OCI_Environment *env` as the first parameter

**Library-level queries and settings** (previously took no context or a different arg):

```c
OCI_UInt32     OCI_GetOCICompileVersion(OCI_Environment *env);
OCI_UInt32     OCI_GetOCIRuntimeVersion(OCI_Environment *env);
OCI_ImportMode OCI_GetImportMode(OCI_Environment *env);
OCI_Charset    OCI_GetCharset(OCI_Environment *env);
OCI_UInt64     OCI_GetStats(OCI_Environment *env, OCI_Stat stat); /* see §15 */
OCI_Boolean    OCI_EnableWarnings(OCI_Environment *env, OCI_Boolean value);
OCI_Boolean    OCI_SetErrorHandler(OCI_Environment *env, POCI_ERROR handler);
OCI_Boolean    OCI_SetHAHandler(OCI_Environment *env, POCI_HA_HANDLER handler);
const otext*   OCI_GetLocaleString(OCI_Environment *env, OCI_LocaleCode code);
OCI_Boolean    OCI_SetFormat(OCI_Environment *env, OCI_FormatType type, const otext *format);
const otext*   OCI_GetFormat(OCI_Environment *env, OCI_FormatType type);
OCI_Boolean    OCI_SetUserPassword(OCI_Environment *env, const otext *db,
                                   const otext *user, const otext *pwd, const otext *new_pwd);
```

New functions that also take `env` (no v4 equivalent — see §10):

```c
OCI_Boolean OCI_SetSubscriptionPort(OCI_Environment *env, OCI_UInt32 port);
OCI_UInt32  OCI_GetSubscriptionPort(OCI_Environment *env);
```

**Connections and pools:**

```c
/* v4 */ OCI_ConnectionCreate(const otext *db, const otext *user, const otext *pwd, unsigned int mode);
/* v5 */ OCI_ConnectionCreate(OCI_Environment *env, const otext *db, const otext *user,
                              const otext *pwd, OCI_SessionMode mode);

/* v4 */ OCI_PoolCreate(const otext *db, const otext *user, const otext *pwd,
                        unsigned int type, unsigned int mode,
                        unsigned int min_con, unsigned int max_con, unsigned int incr_con);
/* v5 */ OCI_PoolCreate(OCI_Environment *env, const otext *db, const otext *user,
                        const otext *pwd, OCI_PoolType type, OCI_SessionMode mode,
                        OCI_UInt32 min_con, OCI_UInt32 max_con, OCI_UInt32 incr_con);

/* v4 */ OCI_PoolGetConnection(OCI_Pool *pool, const otext *tag);
/* v5 */ OCI_PoolGetConnection(OCI_Environment *env, OCI_Pool *pool, const otext *tag);

/* v4 */ OCI_PoolFree(OCI_Pool *pool);
/* v5 */ OCI_PoolFree(OCI_Environment *env, OCI_Pool *pool);
```

**Standalone datatype objects** (previously took `OCI_Connection *con` — now owned by
the environment, not a connection):

```c
OCI_Date*      OCI_DateCreate(OCI_Environment *env);
OCI_Date**     OCI_DateArrayCreate(OCI_Environment *env, OCI_UInt32 nbelem);

OCI_Number*    OCI_NumberCreate(OCI_Environment *env);
OCI_Number**   OCI_NumberArrayCreate(OCI_Environment *env, OCI_UInt32 nbelem);

OCI_Timestamp*   OCI_TimestampCreate(OCI_Environment *env, OCI_TimestampType type);
OCI_Timestamp**  OCI_TimestampArrayCreate(OCI_Environment *env, OCI_TimestampType type,
                                          OCI_UInt32 nbelem);

OCI_Interval*    OCI_IntervalCreate(OCI_Environment *env, OCI_IntervalType type);
OCI_Interval**   OCI_IntervalArrayCreate(OCI_Environment *env, OCI_IntervalType type,
                                         OCI_UInt32 nbelem);
```

**Hash tables, database control:**

```c
OCI_HashTable* OCI_HashCreate(OCI_Environment *env, OCI_UInt32 size, OCI_HashType type);

OCI_Boolean OCI_DatabaseStartup(OCI_Environment *env, const otext *db, const otext *user,
                                const otext *pwd, OCI_SessionMode sess_mode,
                                OCI_DatabaseStartMode start_mode,
                                OCI_DatabaseStartFlag start_flag, const otext *spfile);

OCI_Boolean OCI_DatabaseShutdown(OCI_Environment *env, const otext *db, const otext *user,
                                 const otext *pwd, OCI_SessionMode sess_mode,
                                 OCI_DatabaseShutdownMode shut_mode,
                                 OCI_DatabaseShutdownFlag shut_flag);
```

### 3.2 Functions that did NOT change their first parameter

These still take a parent handle — **do not** add `env` here:

```c
OCI_StatementCreate(OCI_Connection *con);
OCI_LobCreate / OCI_LobArrayCreate        (OCI_Connection *con, OCI_LobType, ...);
OCI_FileCreate / OCI_FileArrayCreate      (OCI_Connection *con, OCI_FileType, ...);
OCI_LongCreate                            (OCI_Statement *stmt, OCI_LongType);
OCI_CollCreate                            (OCI_TypeInfo *typinf);
OCI_VectorCreate / OCI_VectorArrayCreate  (OCI_Connection *con, ...);
OCI_MsgCreate / OCI_EnqueueCreate / OCI_DequeueCreate  (OCI_TypeInfo *typinf);
OCI_AgentCreate                           (OCI_Connection *con, const otext *name, const otext *address);
OCI_DirPathCreate                         (OCI_TypeInfo *typinf, ...);
OCI_TypeInfoGet                           (OCI_Connection *con, const otext *name, OCI_TypeInfoType);
OCI_TransactionCreate                     (OCI_Connection *con, OCI_UInt32 timeout, OCI_UInt32 flags);
```

The following constructors **dropped their `OCI_Connection*` argument** — the owning
connection is now deduced from the `OCI_TypeInfo` handle itself:

```c
OCI_ObjectCreate(OCI_TypeInfo *typinf);
OCI_ObjectArrayCreate(OCI_TypeInfo *typinf, OCI_UInt32 nbelem);
OCI_RefCreate(OCI_TypeInfo *typinf);
OCI_RefArrayCreate(OCI_TypeInfo *typinf, OCI_UInt32 nbelem);
OCI_CollArrayCreate(OCI_TypeInfo *typinf, OCI_UInt32 nbelem);
```

### 3.3 Recovering an environment from a handle

If you need the environment inside a callback or deep in call stacks, use the new helper:

```c
OCI_Environment* OCI_HandleGetEnvironment(void *handle);   /* any OCILIB handle */
```

---

## 4. Step 3 — All indexes are now 0-based (the silent trap)

In v4 every index in the C API was 1-based. In v5 **all index parameters and
index-returning functions are 0-based**, and lookups that can miss return
`OCI_Int32` with **`-1`** as the "not found / no position" sentinel instead of
`0`.

> **This is the highest-risk change in v5.** `coll[1]`, `OCI_GetString(rs, 1)`,
> `for (i = 1; i <= n; ++i)` all still compile — they just read the *wrong* element.
> The compiler will not flag a single site. Audit every literal index and every
> index loop deliberately.

### Index parameters — now 0-based

| API group | What changed |
|-----------|--------------|
| Resultset columns | `OCI_GetColumn(rs, i)`, `OCI_Get*(rs, i)` getters (`OCI_GetInt`, `OCI_GetString`, `OCI_IsNull`, `OCI_GetRaw`, `OCI_GetDataSize`, `OCI_GetDataLength`, `OCI_GetStruct`, …), `OCI_FetchSeek(rs, OCI_SFD_ABSOLUTE, i)` |
| Type info columns | `OCI_TypeInfoGetColumn(typinf, i)` |
| Binds | `OCI_GetBind(stmt, i)`; array element position `pos` of `OCI_BindSetNullAtPos`, `OCI_BindSetNotNullAtPos`, `OCI_BindIsNullAtPos`, `OCI_BindSetDataSizeAtPos`, `OCI_BindGetDataSizeAtPos` (scalar binds use position `0`) |
| Collections | `OCI_CollGetElem`, `OCI_CollSetElem`, `OCI_CollDeleteElem` |
| JSON DOM | `OCI_JsonNodeGetFieldName`, `OCI_JsonNodeGetFieldByIndex`, `OCI_JsonNodeGetItem`, `OCI_JsonNodeSetItem`, `OCI_JsonNodeInsertItem`, `OCI_JsonNodeRemoveItem`, `OCI_JsonNodeRemoveItems` |
| Direct path | `OCI_DirPathSetColumn(dp, i,…)` and the row/column arguments of `OCI_DirPathSetEntry`/`GetEntry`/`GetEntryCount` |

### Return values — new 0-based or `-1` semantics

| v4 | v5 |
|----|----|
| `OCI_GetColumnIndex`, `OCI_GetBindIndex` — `unsigned int`, 1-based, `0` = miss | `OCI_Int32` — **0-based** index, **`-1`** = miss |
| `OCI_GetCurrentRow` — 1-based, `0` = unfetched | `OCI_Int32` — **0-based**, **`-1`** = unfetched/empty |
| `OCI_ErrorGetRow` — 1-based array-DML row, `0` = unrelated | `OCI_Int32` — **0-based**, **`-1`** = unrelated |
| `OCI_DirPathGetErrorRow`, `OCI_DirPathGetErrorColumn` — 1-based | **0-based** (`0` remains the "no error" sentinel) |
| `OCI_GetSqlErrorPos` — 1-based | **0-based** character offset; `(OCI_UInt32)-1` = no parse error |

> Note: some underlying OCI primitives (`OCICollGetElem`, `OCIParamGet`, batch
> error rows, `OCI_ATTR_ROWID`-style parameters) keep their own conventions;
> OCILIB translates internally, so application code should always think
> 0-based.

---

## 5. Step 4 — Unchanged signatures, changed behaviour

> **Warning:** a migration is not only about signatures. Several functions kept
> exactly the same prototype in v5 but behave differently. The compiler will **not**
> flag these — audit them deliberately.

1. **`OCI_GetLastError(void)`** — same prototype, different semantics. In v4 it
   returned the *global* last error unless you opted into thread-local errors with
   `OCI_ENV_CONTEXT`. In v5 it **always** returns the calling thread's `OCI_Error`;
   the flag is gone and there is no global error slot. Code that read "the last error
   produced anywhere" must be reworked to read errors on the thread that produced them.
2. **`OCI_EnableWarnings(env, value)`** — beyond the new `env` parameter, warnings are
   now scoped per-environment; enabling them no longer affects other environments.
3. **Row-level change notifications now fire.** `OCI_SubscriptionRegister` with
   `OCI_CNT_ROWS` now actually delivers per-row events with rowids — previously they
   were silently dropped. Audit notification handlers for assumptions.
4. **AQ dequeue registration is atomic.** `OCI_DequeueSubscribe` registers all queues
   in one `OCISubscriptionRegister` call — fixes `ORA-24915` from repeated listener
   starts.
5. **`OCI_EnvironmentFree` auto-frees children.** Objects not explicitly freed
   (connections, pools, statements, standalone datatypes) are released at environment
   free — v4 `OCI_Cleanup` did the same, but any code relying on object lifetimes
   *after* cleanup ordering should double-check.
6. **`OCI_GetBindIndex` no longer reports registered binds.** In v4, names created
   by `OCI_RegisterXxx()` leaked through the shared name map: the call returned a
   `rbinds[]` position that could not be fed back into `OCI_GetBind()` (which only
   indexes `ubinds[]`). In v5 it returns **`-1`** for register names — as was always
   intended; registers are out-params, not binds. Code relying on the leaked index
   should iterate register results via `OCI_Execute()` + `OCI_GetResultset()`/
   `OCI_RegisterGetXxx()` instead.
7. **`OCI_IPC_*` values were renumbered** (and thread/mutex/thread-key IDs removed).
   Never hard-code the numeric values; use the macros.
8. **`OCI_Environment` is public but opaque** — you never dereference it.
9. **One environment per application** is the documented pattern, but multiple
   environments are supported (each loads its own OCI symbol table). Two environments
   can even load *different* OCI client libraries.
10. **New opt-in logging.** Set environment variables before `OCI_EnvironmentCreate`:
    - `OCILIB_LOG` — `NONE` (default), `ERROR`, `WARN`, `INFO`, `DEBUG`, `TRACE`
    - `OCILIB_LOG_CATEGORIES` — `LIB`, `MEM`, `OCI`, `ALL` (default `ALL`)
    - `OCILIB_LOG_OUTPUT` — `STDERR` (default), `STDOUT`, or a file path

---

## 6. Step 5 — Error model: type, category and code are now separate

v4 conflated the **error type** (error vs warning) and the **origin** (Oracle vs
OCILIB) in the single `OCI_ErrorGetType()` result, and split the error code
across two getters. v5 exposes three independent axes:

| Axis | v4 | v5 |
|------|----|----|
| Type | `OCI_ErrorGetType()` → `OCI_ERR_WARNING` | `OCI_ErrorGetType()` → `OCI_ERR_TYPE_ERROR` / `OCI_ERR_TYPE_WARNING` |
| Origin | `OCI_ErrorGetType()` → `OCI_ERR_ORACLE` / `OCI_ERR_OCILIB` | `OCI_ErrorGetCategory()` → `OCI_ERR_CAT_LIB` / `OCI_ERR_CAT_OCI` / `OCI_ERR_CAT_JZN` |
| Code | `OCI_ErrorGetOCICode()` / `OCI_ErrorGetInternalCode()` | `OCI_ErrorGetCode()` — code in the category's namespace |

### Exact rules

| v4 | v5 |
|----|----|
| `OCI_ErrorGetType(e) == OCI_ERR_ORACLE` | `OCI_ErrorGetCategory(e) == OCI_ERR_CAT_OCI` |
| `OCI_ErrorGetType(e) == OCI_ERR_OCILIB` | `OCI_ErrorGetCategory(e) == OCI_ERR_CAT_LIB` |
| `OCI_ErrorGetType(e) == OCI_ERR_WARNING` | `OCI_ErrorGetType(e) == OCI_ERR_TYPE_WARNING` |
| `OCI_ErrorGetOCICode(e)` | `OCI_ErrorGetCode(e)` (category `OCI_ERR_CAT_OCI`) |
| `OCI_ErrorGetInternalCode(e)` | `OCI_ErrorGetCode(e)` (category `OCI_ERR_CAT_LIB`) |

- **Removed constants:** `OCI_ERR_ORACLE`, `OCI_ERR_OCILIB`, `OCI_ERR_WARNING`.
- **Removed functions:** `OCI_ErrorGetOCICode()`, `OCI_ErrorGetInternalCode()`.
- **New functions:**
  `OCI_ErrorCategory OCI_ErrorGetCategory(OCI_Error *err)` and
  `OCI_ErrorCode OCI_ErrorGetCode(OCI_Error *err)`.
- `OCI_ErrorGetType()` keeps its name but now returns an `OCI_ErrorType`
  (a `OCI_UInt32` typedef) holding only the error type: `OCI_ERR_TYPE_ERROR` or
  `OCI_ERR_TYPE_WARNING`.
- **New typedefs:** `OCI_ErrorType` (error type), `OCI_ErrorCategory` (origin),
  `OCI_ErrorCode` (unified code) — all `OCI_UInt32` aliases.
- **New category:** `OCI_ERR_CAT_JZN` — errors raised by the Oracle JSON DOM
  (JZN) layer; `OCI_ErrorGetCode()` then returns the JZN `jsonerr` code.
- `OCI_ErrorGetCode()` returns a single code whose namespace is given by the
  category: ORA-XXXXX for `OCI_ERR_CAT_OCI`, an `OCI_ERR_*` internal code for
  `OCI_ERR_CAT_LIB`, a JZN `jsonerr` for `OCI_ERR_CAT_JZN`.

### Error handler example

```c
/* v4 */
void err_handler(OCI_Error *err)
{
    if (OCI_ErrorGetType(err) == OCI_ERR_ORACLE)
        printf("ORA-%d: %s\n", OCI_ErrorGetOCICode(err), OCI_ErrorGetString(err));
    else if (OCI_ErrorGetType(err) == OCI_ERR_OCILIB)
        printf("OCILIB-%d: %s\n", OCI_ErrorGetInternalCode(err), OCI_ErrorGetString(err));
}

/* v5 */
void err_handler(OCI_Error *err)
{
    if (OCI_ErrorGetType(err) == OCI_ERR_TYPE_WARNING)
        return; /* warning — OCI_ErrorGetCategory() still identifies the origin */

    if (OCI_ErrorGetCategory(err) == OCI_ERR_CAT_OCI)
        printf("ORA-%u: %s\n", OCI_ErrorGetCode(err), OCI_ErrorGetString(err));
    else
        printf("OCILIB-%u: %s\n", OCI_ErrorGetCode(err), OCI_ErrorGetString(err));
}
```

> All old type constants and both old code getters are gone, so every affected
> call site fails to compile — the compiler will find them all. Just beware
> code that hard-coded the *numeric* values (`OCI_ERR_WARNING` was `3`;
> `OCI_ERR_TYPE_WARNING` is `2`).

---

## 7. Step 6 — `boolean` → `OCI_Boolean`, new scalar typedefs

### 7.1 `boolean` is gone entirely

v4 exported `#define boolean int`, which collided with other libraries and polluted
every translation unit. v5 uses a real typedef:

```c
typedef int OCI_Boolean;
```

`TRUE`/`FALSE` are unchanged. **There is no compatibility alias** — every `boolean`
in your code fails to compile and must be renamed to `OCI_Boolean` (a straight
textual rename; watch out for `struct boolean` fields if you had any).

### 7.2 Fixed-width typedefs

v5 introduces fixed-width integer typedefs in `platform.h`:

```c
typedef short              OCI_Int16;
typedef unsigned short     OCI_UInt16;
typedef int                OCI_Int32;
typedef unsigned int       OCI_UInt32;
typedef long long          OCI_Int64;
typedef unsigned long long OCI_UInt64;
typedef size_t             OCI_Size;
typedef float              OCI_Float;
typedef double             OCI_Double;
```

`big_int` and `big_uint` remain as compatibility typedefs in `platform.h`
(`typedef OCI_Int64 big_int; typedef OCI_UInt64 big_uint;`). Rename when
convenient; do not use the old names in new code.

Notes:

- `OCI_Int64`/`OCI_UInt64` are `long long`/`unsigned long long` on **every**
  supported platform — exactly what the old `big_int`/`big_uint` typedefs resolved
  to. `printf` specifiers (`%lld`/`%llu`), `OCI_ARG_OF` `_Generic` dispatch and the
  C++ `Get<>`/`Bind<>`/`Register<>` overload sets behave exactly as before.
- The old conditional fallback (`typedef int big_int` on compilers without 64-bit
  integers) is gone — the 64-bit types now always exist.
- `OCI_Int32`/`OCI_UInt32`/`OCI_Int16`/`OCI_UInt16` are used throughout the
  public API for numeric parameters and return values. `OCI_Size` covers byte
  sizes, buffer lengths and element counts in internal code.
- `OCI_Float`/`OCI_Double` are the single/double precision floating point
  types, used for float and double parameters and return values.
- `char` and `unsigned char` remain `char`/`unsigned char` — they denote
  characters and raw bytes, not integer quantities.

---

## 8. Step 7 — `*2` suffixed functions renamed / removed

The cryptic v4 `2` suffix is gone in v5. There were **two different meanings** —
check which group each function you use belongs to.

### By-name accessors → `*ByName`

All "by column/bind name" getters changed `Xxx2` → `XxxByName`. Purely mechanical:

```c
OCI_GetString2(rs, "name")              → OCI_GetStringByName(rs, "name")
OCI_GetInt2(rs, "id")                   → OCI_GetIntByName(rs, "id")
OCI_GetColumn2(rs, name)                → OCI_GetColumnByName(rs, name)
OCI_GetBind2(stmt, ":n")                → OCI_GetBindByName(stmt, ":n")
OCI_IsNull2(rs, "col")                  → OCI_IsNullByName(rs, "col")
OCI_GetDataSize2(rs, "col")             → OCI_GetDataSizeByName(rs, "col")
OCI_SetStructNumericType2(rs, "c", t)   → OCI_SetStructNumericTypeByName(rs, "c", t)
```

Applies to every `OCI_GetXxx2` resultset getter (Number, Short, UnsignedShort, Int,
UnsignedInt, BigInt, UnsignedBigInt, String, Raw, Double, Float, Date, Timestamp,
Interval, Statement, Lob, File, Object, Coll, Ref, XmlType, Vector, Long).

### LOB read/write/append — counts variant is now the only one

The legacy count-returning `OCI_LobRead`/`OCI_LobWrite`/`OCI_LobAppend` were removed
(the first two were already `[OBSOLETE]` in v4). The former `*2` variants were
promoted to the primary names:

```c
/* v4 */                                        /* v5 */
unsigned int n;                                 OCI_UInt32 cc = len, bc = 0;
n = OCI_LobWrite(lob, buf, len);            →   OCI_LobWrite(lob, buf, &cc, &bc);
n = OCI_LobRead(lob, buf, SIZE);            →   OCI_LobRead(lob, buf, &cc, &bc);
```

- Signature: `OCI_Boolean OCI_LobRead(lob, buffer, OCI_UInt32 *char_count, OCI_UInt32 *byte_count)` — counts are in/out.
- For CLOBs use `char_count`; for BLOBs use `byte_count`. Passing `0` for the unused
  count on input lets OCILIB compute it.
- Return value moved to `OCI_Boolean` — check the out counts for the amount transferred.

### `OCI_CollGetElem2` removed

`OCI_CollGetElem(coll, index)` keeps its v4 signature (`OCI_Elem*` return, shared
collection-owned slot). The redundant `OCI_CollGetElem2` was simply deleted —
if you used it, call `OCI_CollGetElem` instead.

---

## 9. Step 8 — Bind calls now return `OCI_Bind*`

All input bind-registration functions now return the created `OCI_Bind*` handle
instead of a boolean. `NULL` means failure (as before — an error is also recorded
and retrievable via `OCI_GetLastError()`).

Affected: `OCI_BindBoolean`, `OCI_Bind{Number,Short,UnsignedShort,Int,UnsignedInt,BigInt,UnsignedBigInt,String,Raw,Double,Float,Date,Timestamp,Interval,Lob,File,Object,Vector,Coll,Ref,Statement,Long,Json}` and all `OCI_BindArrayOf{…}` variants.
**`OCI_Register*` functions are unchanged** — they still return `OCI_Boolean`.

```c
/* v4 */ if (!OCI_BindInt(stmt, ":n", &n)) { /* fail */ }
/* v5 */ if (OCI_BindInt(stmt, ":n", &n) == NULL) { /* fail */ }
/* v5 — new capability: use the bind directly */
OCI_Bind *b = OCI_BindInt(stmt, ":n", &n);
OCI_BindSetDirection(b, OCI_BDM_OUT);   /* no need for OCI_GetBind() */
```

**Compatibility:** call sites that ignore the result are unaffected. `if (OCI_Bind…())`
still works (non-NULL is truthy). Only assignments into a `boolean`/`OCI_Boolean`
variable need a type change. The getter pair `OCI_GetBind`/`OCI_GetBindByName` still exists
for retrieval by index/name.

---

## 10. Step 9 — Notifications / AQ changes

### 10.1 Subscription port is now per-environment

v4 passed a `port` argument on every registration. v5 configures it once on the
environment, before the first subscription:

```c
OCI_Boolean OCI_SetSubscriptionPort(OCI_Environment *env, OCI_UInt32 port);
OCI_UInt32  OCI_GetSubscriptionPort(OCI_Environment *env);
```

### 10.2 `OCI_SubscriptionRegister` lost its `port` parameter, gained `mode`

```c
/* v4 */
OCI_SubscriptionRegister(con, name, type, handler, port, timeout);

/* v5 */
OCI_SubscriptionRegister(con, name, type, handler, mode, timeout);
```

`mode` is an `OCI_SubscriptionMode` (typed `OCI_UInt32` typedef): `OCI_SRM_DEFAULT`
(server initiates the notification listener connection — same as v4) or
`OCI_SRM_SECURE_NOTIFICATION` (client initiates it; requires Oracle client 12c+).
Use `OCI_SRM_DEFAULT` to keep v4 behavior.

### 10.3 `OCI_DequeueSubscribe` is now a batch call

```c
/* v4 — one dequeue, explicit port */
OCI_DequeueSubscribe(dequeue, port, timeout, callback);

/* v5 — array of dequeues from a connection */
OCI_DequeueSubscribe(con, dequeues /* OCI_Dequeue** */, count, timeout, callback);
```

Build an `OCI_Dequeue*[]` array of the queues you want notifications for and register
them in a single call. Individual dequeues can still be unsubscribed with
`OCI_DequeueUnsubscribe()`; `OCI_DequeueFree()` auto-unsubscribes.

> `OCI_ENV_EVENTS` must still be passed to `OCI_EnvironmentCreate` for notifications.

---

## 11. Step 10 — Removed threading API

All public threading primitives are gone. OCILIB internally uses ISO C11 `<threads.h>`,
and so should you — or `std::thread`/`std::mutex` in C++, pthreads, or Win32.

| Removed v4 API | Replacement |
|----------------|-------------|
| `OCI_MutexCreate()` / `OCI_MutexFree(m)` | `mtx_t m;` `mtx_init(&m, mtx_plain)` / `mtx_destroy(&m)` |
| `OCI_MutexAcquire(m)` / `OCI_MutexRelease(m)` | `mtx_lock(&m)` / `mtx_unlock(&m)` |
| `OCI_ThreadCreate()` / `OCI_ThreadFree(t)` | `thrd_t t;` / `(void)t` |
| `OCI_ThreadRun(t, proc, arg)` | `thrd_create(&t, proc, arg)` — note `thrd_start_t` is `int (*)(void*)`, not `void (*)(OCI_Thread*, void*)` |
| `OCI_ThreadJoin(t)` | `thrd_join(t, &res)` |
| `OCI_ThreadKeyCreate(name, dtor)` / `SetValue` / `GetValue` | `tss_t key;` `tss_create(&key, dtor)` / `tss_set` / `tss_get` |

Removed types: `OCI_Mutex`, `OCI_Thread`, `OCI_ThreadKey`.
Removed callback typedefs: `POCI_THREAD`, `POCI_THREADKEYDEST`.

> **Effort:** trivial if you only used `OCI_Mutex` for a critical section; more work if
> you built worker pools on `OCI_Thread`. The `OCI_ThreadKey` name→key registry has no
> direct equivalent — use a static `tss_t` per key instead.

Note: OCILIB is still thread-safe internally (`OCI_ENV_THREADED`); this change only
removes the *public* helper wrappers.

---

## 12. Step 11 — Raw OCI handle accessors removed

All `OCI_HandleGet<…>` getters that leaked raw OCI handles are gone. Only one survives,
with a **new meaning**:

```c
/* v4 */ const void* OCI_HandleGetEnvironment(void);                 /* returned OCIEnv*  */
/* v5 */ OCI_Environment* OCI_HandleGetEnvironment(void *handle);   /* returns env owner */
```

Removed: `OCI_HandleGetContext`, `OCI_HandleGetServer`, `OCI_HandleGetError`,
`OCI_HandleGetSession`, `OCI_HandleGetTransaction`, `OCI_HandleGetStatement`,
`OCI_HandleGetLob`, `OCI_HandleGetFile`, `OCI_HandleGetDate`, `OCI_HandleGetTimestamp`,
`OCI_HandleGetInterval`, `OCI_HandleGetObject`, `OCI_HandleGetColl`, `OCI_HandleGetRef`,
`OCI_HandleGetMutex`, `OCI_HandleGetThread`, `OCI_HandleGetThreadID`,
`OCI_HandleGetDirPathCtx`, `OCI_HandleGetDirPathColArray`, `OCI_HandleGetDirPathStream`,
`OCI_HandleGetSubscription`.

**Action:** if you passed raw OCI handles to direct OCI calls, those call sites must be
rewritten — either through OCILIB APIs or by keeping your own OCI layer. There is no
migration shim; grep your code for `OCI_HandleGet` and decide per site.

---

## 13. Step 12 — XA removed

`OCI_SESSION_XA` and the associated documentation are removed. OCILIB no longer offers
XA / distributed-transaction connections.

**Action:** delete `OCI_SESSION_XA` from session modes. If you need XA, manage the
resource manager integration outside OCILIB (or stay on v4 for that component).

---

## 14. Step 13 — Compatibility macros (`compat.h`)

`include/ocilibc/compat.h` has been **recreated** for the v5 naming overhaul: it
now defines every v4 API name as a macro mapping to its v5 equivalent (see
[Appendix A](#appendix-a--renamed-symbols-v4--v5)). Define `OCI_NO_COMPAT` before
including `ocilib.h` to disable the aliases.

The same treatment applies to the charset **build flag**: `OCI_CHARSET_ANSI` was
renamed `OCI_CHARSET_MBCS` — "ANSI" was misleading, the build covers ANSI
codepages *and* UTF-8 (whatever `NLS_LANG` configures). Mapped for you:

| v4 name | v5 name |
|---------|---------|
| `OCI_CHARSET_ANSI` (build macro) | `OCI_CHARSET_MBCS` (mapped in `platform.h`; the flag stays *defined* on MBCS builds so `#ifdef`s keep working) |
| `OCI_CHAR_ANSI` | `OCI_CHAR_MBCS` (via `compat.h`) |
| `ocilib::Environment::CharsetAnsi` | `CharsetMbcs` (no alias — update switch/case labels) |
| `--with-oracle-charset` | **removed** — Unix builds are always MBCS |
| `Release - ANSI` MSBuild config | `Release - MBCS` |
| `ociliba.dll`/`.lib`, `ocilibm.dll`/`.lib` | `ocilib.dll`/`.lib` (MBCS), `ocilibw.dll`/`.lib` (wide) |

**Wide builds are Windows-only in v5.** `OCI_CHARSET_WIDE` relies on `wchar_t`
being a 2-byte UTF-16 unit, which is only true on Windows; on Unix/macOS the
build now fails at compile time with a clear `#error`. If you ran OCILIB wide
on Unix, switch to `OCI_CHARSET_MBCS` and set `NLS_LANG=….AL32UTF8` — your
`char*` strings then carry UTF-8 end to end.

The **v3-era** backward-compatibility macros that the old file contained are gone
— rename each usage to its modern equivalent:

| Removed alias | Replacement |
|---------------|-------------|
| `OCI_CreateConnection` / `OCI_FreeConnection` | `OCI_ConnectionCreate` / `OCI_ConnectionFree` |
| `OCI_CreateStatement` / `OCI_FreeStatement` | `OCI_StatementCreate` / `OCI_StatementFree` |
| `OCI_CreateTransaction` / `OCI_FreeTransaction` | `OCI_TransactionCreate` / `OCI_TransactionFree` |
| `OCI_CreateHashTable` / `OCI_FreeHashTable` | `OCI_HashCreate` / `OCI_HashFree` |
| `OCI_GetColumn*` (`Name`, `Type`, `CharsetForm`, `SQLType`, `FullSQLType`, `Size`, `Scale`, `Precision`, `FractionnalPrecision`, `LeadingPrecision`, `Nullable`, `CharUsed`) | `OCI_ColumnGet<…>` (same suffix; note `Fractionnal` → `Fractional` spelling fix) |
| `OCI_GetFormatDate(s)` / `OCI_SetFormatDate(s,f)` | `OCI_GetFormat(env, OCI_FMT_DATE)` / `OCI_SetFormat(env, OCI_FMT_DATE, f)` |
| `OCI_ERR_API` | `OCI_ERR_CAT_OCI` (tested via `OCI_ErrorGetCategory` — see §6) |
| `OCI_ERR_NOT_SUPPORTED` | `OCI_ERR_DATATYPE_NOT_SUPPORTED` |
| `OCI_SCHEMA_TABLE` / `OCI_SCHEMA_VIEW` / `OCI_SCHEMA_TYPE` | `OCI_TIF_TABLE` / `OCI_TIF_VIEW` / `OCI_TIF_TYPE` |
| `OCI_Schema`, `OCI_SchemaGet`, `OCI_SchemaFree`, `OCI_SchemaGetColumnCount`, `OCI_SchemaGetColumn`, `OCI_SchemaGetName` | `OCI_TypeInfo`, `OCI_TypeInfoGet`, `OCI_TypeInfoFree`, `OCI_TypeInfoGetColumnCount`, `OCI_TypeInfoGetColumn`, `OCI_TypeInfoGetName` |
| `OCI_SetNull`, `OCI_SetNull2`, `OCI_SetNullAtPos`, `OCI_SetNullAtPos2` | `OCI_BindSetNull(OCI_GetBind(…))` / `OCI_BindSetNullAtPos(OCI_GetBind(…), pos)` |
| `OCI_ConnPool*` (`Create`, `GetConnection`, `Free`, `Get/SetTimeout`, `Get/SetNoWait`, `GetBusyCount`, `GetOpenedCount`, `GetMin`, `GetMax`, `GetIncrement`) | `OCI_Pool*` equivalents |
| `OCI_ObjectGetTimeStamp` / `OCI_ElemGetTimeStamp` | `OCI_ObjectGetTimestamp` / `OCI_ElemGetTimestamp` |
| `OCI_TimestampSysTimeStamp` | `OCI_TimestampSysTimestamp` |
| `OCI_CollSetAt` / `OCI_CollGetAt` / `OCI_CollGetAt2` | `OCI_CollSetElem` / `OCI_CollGetElem` / `OCI_CollGetElem` |
| `OCI_GetCharsetMetaData` / `OCI_GetCharsetUserData` | `OCI_GetCharset(env)` |
| `OCI_8`, `OCI_9`, `OCI_10`, `OCI_11` | `OCI_8_1`, `OCI_9_0`, `OCI_10_1`, `OCI_11_1` |
| `OCI_CHAR_UNICODE` | `OCI_CHAR_WIDE` |
| `OCI_CSF_CHARSET` | `OCI_CSF_DEFAULT` |
| `OCI_SIZE_TRACE_INF0` (with a zero) | `OCI_SIZE_TRACE_INFO` |
| `MT(x)`, `DT(x)` | `OTEXT(x)` |
| `mtext`, `dtext` | `otext` |
| `mts*`/`dts*` string functions (`mtsdup`, `mtscpy`, `mtsncpy`, `mtscat`, `mtsncat`, `mtslen`, `mtscmp`, `mtscasecmp`, `mtsprintf`, `mtstol`, `mtsscanf`, and `dts*` equivalents) | `o*` equivalents (`ostrdup`, `ostrcpy`, `ostrncpy`, `ostrcat`, `ostrncat`, `ostrlen`, `ostrcmp`, `ostrcasecmp`, `osprintf`, `ostrtol`, `osscanf`) |
| `OCI_STRING_FORMAT_NUM_BIN` | `OCI_STRING_FORMAT_NUM_BDOUBLE` |

> **Effort:** trivial textual renames if you used a few aliases; moderate if a codebase
> still relies on the v3 `OCI_ConnPool*`/`OCI_Schema*` layer.

---

## 15. Step 14 — Memory / resource statistics

`OCI_GetAllocatedBytes` has been replaced by the generic `OCI_GetStats` diagnostic API.

```c
/* v4 */ big_uint OCI_GetAllocatedBytes(unsigned int mem_type);   /* OCI_MEM_* */
/* v5 */ OCI_UInt64 OCI_GetStats(OCI_Environment *env, OCI_Stat stat);
```

The `OCI_MEM_ORACLE` / `OCI_MEM_OCILIB` / `OCI_MEM_ALL` constants are **removed**.
The new `OCI_Stat` type selects the requested counter:

| `stat` value | Meaning |
|--------------|---------|
| `OCI_UST_BYTES`      | Bytes allocated internally by OCILIB for that environment |
| `OCI_UST_HANDLES`    | OCI handles allocated for that environment |
| `OCI_UST_DESCRIPTORS`| OCI descriptors allocated for that environment |
| `OCI_UST_OBJECTS`    | OCILIB object instances allocated for that environment |

Replacement for the old byte counter:

```c
/* v4 */ OCI_GetAllocatedBytes(OCI_MEM_OCILIB);
/* v5 */ OCI_GetStats(env, OCI_UST_BYTES);
```

The OCI-client-side figure is no longer exposed.

---

## 16. Non-breaking changes you can ignore (or adopt)

- **Typed parameter typedefs** — `OCI_SessionMode`, `OCI_PoolType`, `OCI_LobType`,
  `OCI_FileType`, `OCI_LongType`, `OCI_TimestampType`, `OCI_IntervalType`,
  `OCI_TypeInfoType`, `OCI_SubscriptionType`, `OCI_SubscriptionMode`, `OCI_HashType`,
  `OCI_EnvMode`, `OCI_ImportMode`, `OCI_Charset`, `OCI_LocaleCode`,
  `OCI_FailoverType/Event/Code`, `OCI_HAEventSource/Type`,
  `OCI_DatabaseStartMode/Flag`, `OCI_DatabaseShutdownMode/Flag`, `OCI_SqlType`,
  `OCI_ErrorType`, `OCI_ErrorCategory`, `OCI_ErrorCode`, `OCI_Stat`, etc.
  All are `OCI_UInt32` (or `OCI_Int32`) aliases — **source-compatible**; passing
  literals or `unsigned int` still compiles. Exception: **callback typedefs must
  match exactly** — update your `POCI_TAF_HANDLER` / `POCI_HA_HANDLER`
  implementations to the typed signatures:

  ```c
  OCI_FailoverCode my_taf(OCI_Connection *con, OCI_FailoverType type, OCI_FailoverEvent event);
  void my_ha(OCI_Connection *con, OCI_HAEventSource src, OCI_HAEventType evt, OCI_Timestamp *t);
  ```

- **New convenience helpers** (pure additions, adopt if useful):

  ```c
  OCI_Resultset *OCI_ExecuteAndGetResultset(OCI_Statement *stmt);
  OCI_Resultset *OCI_ExecuteStmtAndGetResultset(OCI_Statement *stmt, const otext *sql);
  ```

  `OCI_Execute` + `OCI_GetResultset` and `OCI_ExecuteStmt` + `OCI_GetResultset` in
  one call. `NULL` = failure *or* no result set — check `OCI_GetLastError()` to
  disambiguate.

- **`OCI_ARG_OF(ptr)`** — compile-time-checked placeholder for `OCI_Immediate*` /
  `OCI_ImmediateFmt` output arguments. Expands to `tag, ptr` via C11 `_Generic`,
  so `OCI_Immediate(con, sql, OCI_ARG_OF(&count))` replaces `OCI_ARG_INT, &count`
  and a tag/pointer mismatch becomes a compile error. `otext*` maps to
  `OCI_ARG_TEXT`, `void*`/`unsigned char*` to `OCI_ARG_RAW`; `OCI_ARG_TEXT_N`
  stays explicit (it needs the buffer capacity argument). Only defined when
  compiling as C11 or later — pre-C11 C callers keep explicit `OCI_ARG_*` tags,
  and the C++ API does not expose the variadic helpers.

---

## 17. Migration checklist (agent-friendly order)

```
1.  [ ] Introduce `OCI_Environment *env` at program start.
2.  [ ] Replace OCI_Initialize(...)      → env = OCI_EnvironmentCreate(err, lib, mode);
3.  [ ] Remove OCI_ENV_CONTEXT from mode flags.
4.  [ ] Replace OCI_Cleanup()            → OCI_EnvironmentFree(env);
5.  [ ] Add `env` as first arg to: OCI_ConnectionCreate, OCI_PoolCreate,
        OCI_PoolGetConnection, OCI_PoolFree, OCI_DateCreate/ArrayCreate,
        OCI_NumberCreate/ArrayCreate, OCI_TimestampCreate/ArrayCreate,
        OCI_IntervalCreate/ArrayCreate, OCI_HashCreate,
        OCI_DatabaseStartup, OCI_DatabaseShutdown, OCI_SetUserPassword,
        OCI_GetOCICompileVersion, OCI_GetOCIRuntimeVersion, OCI_GetImportMode,
        OCI_GetCharset, OCI_GetStats, OCI_GetLocaleString, OCI_SetFormat,
        OCI_GetFormat, OCI_EnableWarnings, OCI_SetErrorHandler,
        OCI_SetHAHandler.
6.  [ ] Rename every `boolean` → `OCI_Boolean` (no compatibility alias exists).
7.  [ ] Rebase every index to 0-based (see §4): resultset column indexes in
        OCI_Get*/OCI_GetColumn, bind indexes/positions, collection indexes,
        JSON item/field indexes, direct-path rows/columns, OCI_FetchSeek
        absolute offsets — and every `for (i = 1; i <= n)` loop.
8.  [ ] Update lookups: OCI_GetColumnIndex / OCI_GetBindIndex /
        OCI_GetCurrentRow / OCI_ErrorGetRow are OCI_Int32 with -1 = miss;
        OCI_GetSqlErrorPos is 0-based.
9.  [ ] Audit silent behaviour changes (see §5): OCI_GetLastError thread
        scope, OCI_CNT_ROWS notification handlers, OCI_GetBindIndex on
        register names.
10. [ ] Error handling (see §6): OCI_ERR_ORACLE/OCI_ERR_OCILIB checks →
        OCI_ErrorGetCategory() == OCI_ERR_CAT_OCI / OCI_ERR_CAT_LIB;
        OCI_ERR_WARNING checks → OCI_ErrorGetType() == OCI_ERR_TYPE_WARNING;
        OCI_ErrorGetOCICode / OCI_ErrorGetInternalCode → OCI_ErrorGetCode.
11. [ ] Rename every `Xxx2` → `XxxByName`; convert OCI_LobRead/Write/Append
        to the char_count/byte_count form; OCI_CollGetElem2 → OCI_CollGetElem.
12. [ ] Check OCI_Bind*/OCI_BindArrayOf* call sites that stored the result
        in a boolean — they now return OCI_Bind*.
13. [ ] Call OCI_SetSubscriptionPort(env, port) before subscriptions;
        drop `port` arg from OCI_SubscriptionRegister;
        convert OCI_DequeueSubscribe to (con, array, count, timeout, cb).
14. [ ] Replace OCI_Mutex*/OCI_Thread*/OCI_ThreadKey* with <threads.h>.
15. [ ] Remove OCI_SESSION_XA.
16. [ ] Remove all OCI_HandleGet* except OCI_HandleGetEnvironment(handle)
        (return type is now OCI_Environment*).
17. [ ] Replace any v3-era compat macros (OCI_ConnPool*, OCI_Schema*,
        OCI_GetColumn*, OCI_SetNull*, mts*/dts* string helpers, MT/DT, …)
        with the modern names — see §14.
18. [ ] OCI_GetAllocatedBytes(mem_type) → OCI_GetStats(env, OCI_UST_BYTES);
        use OCI_UST_* for other counters.
19. [ ] Update POCI_TAF_HANDLER / POCI_HA_HANDLER signatures to typed params.
20. [ ] Recompile; fix remaining errors (all remaining breaks are compile-time).
```

---

## 18. Worked example — before / after

```c
/* --------------------------- v4 --------------------------- */
#include "ocilib.h"

static boolean err_handler(OCI_Error *err) { /* ... */ return TRUE; }

int main(void)
{
    OCI_Connection *cn;
    OCI_Statement  *st;
    OCI_Date       *d;

    if (!OCI_Initialize(err_handler, NULL, OCI_ENV_DEFAULT | OCI_ENV_CONTEXT))
        return 1;

    cn = OCI_ConnectionCreate("db", "usr", "pwd", OCI_SESSION_DEFAULT);
    st = OCI_StatementCreate(cn);
    d  = OCI_DateCreate(cn);

    /* ... work ... */

    OCI_StatementFree(st);
    OCI_DateFree(d);
    OCI_ConnectionFree(cn);
    OCI_Cleanup();
    return 0;
}
```

```c
/* --------------------------- v5 --------------------------- */
#include "ocilib.h"

static void err_handler(OCI_Error *err) { /* ... */ }

int main(void)
{
    OCI_Environment *env;
    OCI_Connection  *cn;
    OCI_Statement   *st;
    OCI_Date        *d;

    env = OCI_EnvironmentCreate(err_handler, NULL, OCI_ENV_DEFAULT);
    if (env == NULL)
        return 1;

    cn = OCI_ConnectionCreate(env, "db", "usr", "pwd", OCI_SESSION_DEFAULT);
    st = OCI_StatementCreate(cn);           /* still parented by con */
    d  = OCI_DateCreate(env);               /* now parented by env  */

    /* ... work ... */

    OCI_StatementFree(st);
    OCI_DateFree(d);
    OCI_ConnectionFree(cn);
    OCI_EnvironmentFree(env);
    return 0;
}
```

---

## Appendix A — Renamed symbols (v4 → v5)

Complete list of the ~430 public symbols renamed to normalize the API on the
`OCI_<Type><Action>` handle model. Every old name is defined in
`include/ocilibc/compat.h` as a macro for its v5 equivalent, so existing code
keeps compiling; define `OCI_NO_COMPAT` to disable the aliases. Tables are
grouped by the family the function belongs to.

### Handle types (typedefs)

| v4 name | v5 name |
|---------|---------|
| `OCI_Coll` | `OCI_Collection` |
| `OCI_DirPath` | `OCI_DirectPath` |
| `OCI_Elem` | `OCI_Element` |
| `OCI_Iter` | `OCI_Iterator` |
| `OCI_Msg` | `OCI_Message` |
| `OCI_Ref` | `OCI_Reference` |
| `OCI_SodaColl` | `OCI_SodaCollection` |
| `OCI_SodaCollCursor` | `OCI_SodaCollectionCursor` |
| `OCI_SodaDb` | `OCI_SodaDatabase` |
| `OCI_SodaDoc` | `OCI_SodaDocument` |
| `OCI_SodaDocCursor` | `OCI_SodaDocumentCursor` |

### Integer-type constants

| v4 name | v5 name |
|---------|---------|
| `OCI_ARG_BIGINT` | `OCI_ARG_INT64` |
| `OCI_ARG_BIGUINT` | `OCI_ARG_UINT64` |
| `OCI_ARG_INT` | `OCI_ARG_INT32` |
| `OCI_ARG_SHORT` | `OCI_ARG_INT16` |
| `OCI_ARG_UINT` | `OCI_ARG_UINT32` |
| `OCI_ARG_USHORT` | `OCI_ARG_UINT16` |
| `OCI_NUM_BIGINT` | `OCI_NUM_INT64` |
| `OCI_NUM_BIGUINT` | `OCI_NUM_UINT64` |
| `OCI_NUM_INT` | `OCI_NUM_INT32` |
| `OCI_NUM_SHORT` | `OCI_NUM_INT16` |
| `OCI_NUM_UINT` | `OCI_NUM_UINT32` |
| `OCI_NUM_USHORT` | `OCI_NUM_UINT16` |

### Collection

| v4 name | v5 name |
|---------|---------|
| `OCI_CollAppend` | `OCI_CollectionAppend` |
| `OCI_CollArrayCreate` | `OCI_CollectionArrayCreate` |
| `OCI_CollArrayFree` | `OCI_CollectionArrayFree` |
| `OCI_CollAssign` | `OCI_CollectionAssign` |
| `OCI_CollClear` | `OCI_CollectionClear` |
| `OCI_CollCreate` | `OCI_CollectionCreate` |
| `OCI_CollDeleteElem` | `OCI_CollectionDeleteElement` |
| `OCI_CollFree` | `OCI_CollectionFree` |
| `OCI_CollGetCount` | `OCI_CollectionGetCount` |
| `OCI_CollGetElem` | `OCI_CollectionGetElement` |
| `OCI_CollGetMax` | `OCI_CollectionGetMax` |
| `OCI_CollGetSize` | `OCI_CollectionGetSize` |
| `OCI_CollGetType` | `OCI_CollectionGetType` |
| `OCI_CollGetTypeInfo` | `OCI_CollectionGetTypeInfo` |
| `OCI_CollSetElem` | `OCI_CollectionSetElement` |
| `OCI_CollToText` | `OCI_CollectionToText` |
| `OCI_CollTrim` | `OCI_CollectionTrim` |

### Column

| v4 name | v5 name |
|---------|---------|
| `OCI_ColumnGetFullSQLType` | `OCI_ColumnGetFullSqlType` |
| `OCI_ColumnGetSQLType` | `OCI_ColumnGetSqlType` |

### Connection

| v4 name | v5 name |
|---------|---------|
| `OCI_Break` | `OCI_ConnectionBreak` |
| `OCI_Commit` | `OCI_ConnectionCommit` |
| `OCI_GetAutoCommit` | `OCI_ConnectionGetAutoCommit` |
| `OCI_GetDBName` | `OCI_ConnectionGetDatabaseName` |
| `OCI_GetDatabase` | `OCI_ConnectionGetConnectionString` |
| `OCI_GetDefaultLobPrefetchSize` | `OCI_ConnectionGetDefaultLobPrefetchSize` |
| `OCI_GetDomainName` | `OCI_ConnectionGetDomainName` |
| `OCI_GetInstanceName` | `OCI_ConnectionGetInstanceName` |
| `OCI_GetInstanceStartTime` | `OCI_ConnectionGetInstanceStartTime` |
| `OCI_GetMaxCursors` | `OCI_ConnectionGetMaxCursors` |
| `OCI_GetPassword` | `OCI_ConnectionGetPassword` |
| `OCI_GetServerMajorVersion` | `OCI_ConnectionGetServerMajorVersion` |
| `OCI_GetServerMinorVersion` | `OCI_ConnectionGetServerMinorVersion` |
| `OCI_GetServerName` | `OCI_ConnectionGetServerName` |
| `OCI_GetServerRevisionVersion` | `OCI_ConnectionGetServerRevisionVersion` |
| `OCI_GetServiceName` | `OCI_ConnectionGetServiceName` |
| `OCI_GetSessionMode` | `OCI_ConnectionGetSessionMode` |
| `OCI_GetSessionTag` | `OCI_ConnectionGetSessionTag` |
| `OCI_GetStatementCacheSize` | `OCI_ConnectionGetStatementCacheSize` |
| `OCI_GetTimeout` | `OCI_ConnectionGetTimeout` |
| `OCI_GetTrace` | `OCI_ConnectionGetTrace` |
| `OCI_GetTransaction` | `OCI_ConnectionGetTransaction` |
| `OCI_GetUserName` | `OCI_ConnectionGetUserName` |
| `OCI_GetVersionConnection` | `OCI_ConnectionGetVersion` |
| `OCI_GetVersionServer` | `OCI_ConnectionGetServerVersionString` |
| `OCI_Immediate` | `OCI_ConnectionImmediate` |
| `OCI_ImmediateFmt` | `OCI_ConnectionImmediateFormat` |
| `OCI_IsConnected` | `OCI_ConnectionIsAlive` |
| `OCI_IsTAFCapable` | `OCI_ConnectionIsTAFCapable` |
| `OCI_Ping` | `OCI_ConnectionPing` |
| `OCI_Rollback` | `OCI_ConnectionRollback` |
| `OCI_ServerDisableOutput` | `OCI_ConnectionDisableServerOutput` |
| `OCI_ServerEnableOutput` | `OCI_ConnectionEnableServerOutput` |
| `OCI_ServerGetOutput` | `OCI_ConnectionGetServerOutput` |
| `OCI_SetAutoCommit` | `OCI_ConnectionSetAutoCommit` |
| `OCI_SetDefaultLobPrefetchSize` | `OCI_ConnectionSetDefaultLobPrefetchSize` |
| `OCI_SetPassword` | `OCI_ConnectionSetPassword` |
| `OCI_SetSessionTag` | `OCI_ConnectionSetSessionTag` |
| `OCI_SetStatementCacheSize` | `OCI_ConnectionSetStatementCacheSize` |
| `OCI_SetTAFHandler` | `OCI_ConnectionSetTAFHandler` |
| `OCI_SetTimeout` | `OCI_ConnectionSetTimeout` |
| `OCI_SetTrace` | `OCI_ConnectionSetTrace` |
| `OCI_SetTransaction` | `OCI_ConnectionSetTransaction` |

### DirectPath

| v4 name | v5 name |
|---------|---------|
| `OCI_DirPathAbort` | `OCI_DirectPathAbort` |
| `OCI_DirPathConvert` | `OCI_DirectPathConvert` |
| `OCI_DirPathCreate` | `OCI_DirectPathCreate` |
| `OCI_DirPathFinish` | `OCI_DirectPathFinish` |
| `OCI_DirPathFlushRow` | `OCI_DirectPathFlushRow` |
| `OCI_DirPathFree` | `OCI_DirectPathFree` |
| `OCI_DirPathGetAffectedRows` | `OCI_DirectPathGetAffectedRows` |
| `OCI_DirPathGetCurrentRows` | `OCI_DirectPathGetCurrentRows` |
| `OCI_DirPathGetErrorColumn` | `OCI_DirectPathGetErrorColumn` |
| `OCI_DirPathGetErrorRow` | `OCI_DirectPathGetErrorRow` |
| `OCI_DirPathGetMaxRows` | `OCI_DirectPathGetMaxRows` |
| `OCI_DirPathGetRowCount` | `OCI_DirectPathGetRowCount` |
| `OCI_DirPathLoad` | `OCI_DirectPathLoad` |
| `OCI_DirPathPrepare` | `OCI_DirectPathPrepare` |
| `OCI_DirPathReset` | `OCI_DirectPathReset` |
| `OCI_DirPathSave` | `OCI_DirectPathSave` |
| `OCI_DirPathSetBufferSize` | `OCI_DirectPathSetBufferSize` |
| `OCI_DirPathSetCacheSize` | `OCI_DirectPathSetCacheSize` |
| `OCI_DirPathSetColumn` | `OCI_DirectPathSetColumn` |
| `OCI_DirPathSetConvertMode` | `OCI_DirectPathSetConvertMode` |
| `OCI_DirPathSetCurrentRows` | `OCI_DirectPathSetCurrentRows` |
| `OCI_DirPathSetDateFormat` | `OCI_DirectPathSetDateFormat` |
| `OCI_DirPathSetEntry` | `OCI_DirectPathSetEntry` |
| `OCI_DirPathSetNoLog` | `OCI_DirectPathSetNoLog` |
| `OCI_DirPathSetParallel` | `OCI_DirectPathSetParallel` |

### Element

| v4 name | v5 name |
|---------|---------|
| `OCI_ElemCreate` | `OCI_ElementCreate` |
| `OCI_ElemFree` | `OCI_ElementFree` |
| `OCI_ElemGetBigInt` | `OCI_ElementGetInt64` |
| `OCI_ElemGetBoolean` | `OCI_ElementGetBoolean` |
| `OCI_ElemGetColl` | `OCI_ElementGetCollection` |
| `OCI_ElemGetDate` | `OCI_ElementGetDate` |
| `OCI_ElemGetDouble` | `OCI_ElementGetDouble` |
| `OCI_ElemGetFile` | `OCI_ElementGetFile` |
| `OCI_ElemGetFloat` | `OCI_ElementGetFloat` |
| `OCI_ElemGetInt` | `OCI_ElementGetInt32` |
| `OCI_ElemGetInterval` | `OCI_ElementGetInterval` |
| `OCI_ElemGetLob` | `OCI_ElementGetLob` |
| `OCI_ElemGetNumber` | `OCI_ElementGetNumber` |
| `OCI_ElemGetObject` | `OCI_ElementGetObject` |
| `OCI_ElemGetRaw` | `OCI_ElementGetRaw` |
| `OCI_ElemGetRawSize` | `OCI_ElementGetRawSize` |
| `OCI_ElemGetRef` | `OCI_ElementGetReference` |
| `OCI_ElemGetShort` | `OCI_ElementGetInt16` |
| `OCI_ElemGetString` | `OCI_ElementGetString` |
| `OCI_ElemGetTimestamp` | `OCI_ElementGetTimestamp` |
| `OCI_ElemGetUnsignedBigInt` | `OCI_ElementGetUInt64` |
| `OCI_ElemGetUnsignedInt` | `OCI_ElementGetUInt32` |
| `OCI_ElemGetUnsignedShort` | `OCI_ElementGetUInt16` |
| `OCI_ElemIsNull` | `OCI_ElementIsNull` |
| `OCI_ElemSetBigInt` | `OCI_ElementSetInt64` |
| `OCI_ElemSetBoolean` | `OCI_ElementSetBoolean` |
| `OCI_ElemSetColl` | `OCI_ElementSetCollection` |
| `OCI_ElemSetDate` | `OCI_ElementSetDate` |
| `OCI_ElemSetDouble` | `OCI_ElementSetDouble` |
| `OCI_ElemSetFile` | `OCI_ElementSetFile` |
| `OCI_ElemSetFloat` | `OCI_ElementSetFloat` |
| `OCI_ElemSetInt` | `OCI_ElementSetInt32` |
| `OCI_ElemSetInterval` | `OCI_ElementSetInterval` |
| `OCI_ElemSetLob` | `OCI_ElementSetLob` |
| `OCI_ElemSetNull` | `OCI_ElementSetNull` |
| `OCI_ElemSetNumber` | `OCI_ElementSetNumber` |
| `OCI_ElemSetObject` | `OCI_ElementSetObject` |
| `OCI_ElemSetRaw` | `OCI_ElementSetRaw` |
| `OCI_ElemSetRef` | `OCI_ElementSetReference` |
| `OCI_ElemSetShort` | `OCI_ElementSetInt16` |
| `OCI_ElemSetString` | `OCI_ElementSetString` |
| `OCI_ElemSetTimestamp` | `OCI_ElementSetTimestamp` |
| `OCI_ElemSetUnsignedBigInt` | `OCI_ElementSetUInt64` |
| `OCI_ElemSetUnsignedInt` | `OCI_ElementSetUInt32` |
| `OCI_ElemSetUnsignedShort` | `OCI_ElementSetUInt16` |

### Environment

| v4 name | v5 name |
|---------|---------|
| `OCI_EnableWarnings` | `OCI_EnvironmentEnableWarnings` |
| `OCI_GetCharset` | `OCI_EnvironmentGetCharset` |
| `OCI_GetFormat` | `OCI_EnvironmentGetFormat` |
| `OCI_GetImportMode` | `OCI_EnvironmentGetImportMode` |
| `OCI_GetLocaleString` | `OCI_EnvironmentGetLocaleString` |
| `OCI_GetOCICompileVersion` | `OCI_EnvironmentGetOCICompileVersion` |
| `OCI_GetOCIRuntimeVersion` | `OCI_EnvironmentGetOCIRuntimeVersion` |
| `OCI_GetStats` | `OCI_EnvironmentGetStats` |
| `OCI_GetSubscriptionPort` | `OCI_EnvironmentGetSubscriptionPort` |
| `OCI_SetErrorHandler` | `OCI_EnvironmentSetErrorHandler` |
| `OCI_SetFormat` | `OCI_EnvironmentSetFormat` |
| `OCI_SetHAHandler` | `OCI_EnvironmentSetHAHandler` |
| `OCI_SetSubscriptionPort` | `OCI_EnvironmentSetSubscriptionPort` |
| `OCI_SetUserPassword` | `OCI_EnvironmentSetUserPassword` |

### Handle

| v4 name | v5 name |
|---------|---------|
| `OCI_GetUserData` | `OCI_HandleGetUserData` |
| `OCI_SetUserData` | `OCI_HandleSetUserData` |

### HashTable

| v4 name | v5 name |
|---------|---------|
| `OCI_HashAddInt` | `OCI_HashTableAddInt32` |
| `OCI_HashAddPointer` | `OCI_HashTableAddPointer` |
| `OCI_HashAddString` | `OCI_HashTableAddString` |
| `OCI_HashCreate` | `OCI_HashTableCreate` |
| `OCI_HashFree` | `OCI_HashTableFree` |
| `OCI_HashGetEntry` | `OCI_HashTableGetEntry` |
| `OCI_HashGetInt` | `OCI_HashTableGetInt32` |
| `OCI_HashGetPointer` | `OCI_HashTableGetPointer` |
| `OCI_HashGetSize` | `OCI_HashTableGetSize` |
| `OCI_HashGetString` | `OCI_HashTableGetString` |
| `OCI_HashGetType` | `OCI_HashTableGetType` |
| `OCI_HashGetValue` | `OCI_HashTableGetValue` |
| `OCI_HashLookup` | `OCI_HashTableLookup` |

### Iterator

| v4 name | v5 name |
|---------|---------|
| `OCI_IterCreate` | `OCI_IteratorCreate` |
| `OCI_IterFree` | `OCI_IteratorFree` |
| `OCI_IterGetCurrent` | `OCI_IteratorGetCurrent` |
| `OCI_IterGetNext` | `OCI_IteratorGetNext` |
| `OCI_IterGetPrev` | `OCI_IteratorGetPrev` |

### Json

| v4 name | v5 name |
|---------|---------|
| `OCI_JsonNodeGetBigInt` | `OCI_JsonNodeGetInt64` |
| `OCI_JsonNodeGetBigUInt` | `OCI_JsonNodeGetUInt64` |
| `OCI_JsonNodeGetInt` | `OCI_JsonNodeGetInt32` |
| `OCI_JsonNodeGetShort` | `OCI_JsonNodeGetInt16` |
| `OCI_JsonNodeGetUInt` | `OCI_JsonNodeGetUInt32` |
| `OCI_JsonNodeGetUShort` | `OCI_JsonNodeGetUInt16` |
| `OCI_JsonNodeSetBigInt` | `OCI_JsonNodeSetInt64` |
| `OCI_JsonNodeSetBigUInt` | `OCI_JsonNodeSetUInt64` |
| `OCI_JsonNodeSetInt` | `OCI_JsonNodeSetInt32` |
| `OCI_JsonNodeSetShort` | `OCI_JsonNodeSetInt16` |
| `OCI_JsonNodeSetUInt` | `OCI_JsonNodeSetUInt32` |
| `OCI_JsonNodeSetUShort` | `OCI_JsonNodeSetUInt16` |

### Message

| v4 name | v5 name |
|---------|---------|
| `OCI_MsgCreate` | `OCI_MessageCreate` |
| `OCI_MsgFree` | `OCI_MessageFree` |
| `OCI_MsgGetAttemptCount` | `OCI_MessageGetAttemptCount` |
| `OCI_MsgGetCorrelation` | `OCI_MessageGetCorrelation` |
| `OCI_MsgGetEnqueueDelay` | `OCI_MessageGetEnqueueDelay` |
| `OCI_MsgGetEnqueueTime` | `OCI_MessageGetEnqueueTime` |
| `OCI_MsgGetExceptionQueue` | `OCI_MessageGetExceptionQueue` |
| `OCI_MsgGetExpiration` | `OCI_MessageGetExpiration` |
| `OCI_MsgGetID` | `OCI_MessageGetID` |
| `OCI_MsgGetObject` | `OCI_MessageGetObject` |
| `OCI_MsgGetOriginalID` | `OCI_MessageGetOriginalID` |
| `OCI_MsgGetPriority` | `OCI_MessageGetPriority` |
| `OCI_MsgGetRaw` | `OCI_MessageGetRaw` |
| `OCI_MsgGetSender` | `OCI_MessageGetSender` |
| `OCI_MsgGetState` | `OCI_MessageGetState` |
| `OCI_MsgReset` | `OCI_MessageReset` |
| `OCI_MsgSetConsumers` | `OCI_MessageSetConsumers` |
| `OCI_MsgSetCorrelation` | `OCI_MessageSetCorrelation` |
| `OCI_MsgSetEnqueueDelay` | `OCI_MessageSetEnqueueDelay` |
| `OCI_MsgSetExceptionQueue` | `OCI_MessageSetExceptionQueue` |
| `OCI_MsgSetExpiration` | `OCI_MessageSetExpiration` |
| `OCI_MsgSetObject` | `OCI_MessageSetObject` |
| `OCI_MsgSetOriginalID` | `OCI_MessageSetOriginalID` |
| `OCI_MsgSetPriority` | `OCI_MessageSetPriority` |
| `OCI_MsgSetRaw` | `OCI_MessageSetRaw` |
| `OCI_MsgSetSender` | `OCI_MessageSetSender` |

### Object

| v4 name | v5 name |
|---------|---------|
| `OCI_ObjectGetBigInt` | `OCI_ObjectGetInt64` |
| `OCI_ObjectGetInt` | `OCI_ObjectGetInt32` |
| `OCI_ObjectGetRef` | `OCI_ObjectGetReference` |
| `OCI_ObjectGetSelfRef` | `OCI_ObjectGetSelfReference` |
| `OCI_ObjectGetShort` | `OCI_ObjectGetInt16` |
| `OCI_ObjectGetUnsignedBigInt` | `OCI_ObjectGetUInt64` |
| `OCI_ObjectGetUnsignedInt` | `OCI_ObjectGetUInt32` |
| `OCI_ObjectGetUnsignedShort` | `OCI_ObjectGetUInt16` |
| `OCI_ObjectSetBigInt` | `OCI_ObjectSetInt64` |
| `OCI_ObjectSetInt` | `OCI_ObjectSetInt32` |
| `OCI_ObjectSetRef` | `OCI_ObjectSetReference` |
| `OCI_ObjectSetShort` | `OCI_ObjectSetInt16` |
| `OCI_ObjectSetUnsignedBigInt` | `OCI_ObjectSetUInt64` |
| `OCI_ObjectSetUnsignedInt` | `OCI_ObjectSetUInt32` |
| `OCI_ObjectSetUnsignedShort` | `OCI_ObjectSetUInt16` |

### Reference

| v4 name | v5 name |
|---------|---------|
| `OCI_RefArrayCreate` | `OCI_ReferenceArrayCreate` |
| `OCI_RefArrayFree` | `OCI_ReferenceArrayFree` |
| `OCI_RefAssign` | `OCI_ReferenceAssign` |
| `OCI_RefCreate` | `OCI_ReferenceCreate` |
| `OCI_RefFree` | `OCI_ReferenceFree` |
| `OCI_RefGetHexSize` | `OCI_ReferenceGetHexSize` |
| `OCI_RefGetObject` | `OCI_ReferenceGetObject` |
| `OCI_RefGetTypeInfo` | `OCI_ReferenceGetTypeInfo` |
| `OCI_RefIsNull` | `OCI_ReferenceIsNull` |
| `OCI_RefSetNull` | `OCI_ReferenceSetNull` |
| `OCI_RefToText` | `OCI_ReferenceToText` |

### Resultset

| v4 name | v5 name |
|---------|---------|
| `OCI_FetchFirst` | `OCI_ResultsetFetchFirst` |
| `OCI_FetchLast` | `OCI_ResultsetFetchLast` |
| `OCI_FetchNext` | `OCI_ResultsetFetchNext` |
| `OCI_FetchPrev` | `OCI_ResultsetFetchPrev` |
| `OCI_FetchSeek` | `OCI_ResultsetFetchSeek` |
| `OCI_GetBigInt` | `OCI_ResultsetGetInt64` |
| `OCI_GetBigIntByName` | `OCI_ResultsetGetInt64ByName` |
| `OCI_GetColl` | `OCI_ResultsetGetColl` |
| `OCI_GetCollByName` | `OCI_ResultsetGetCollByName` |
| `OCI_GetColumn` | `OCI_ResultsetGetColumn` |
| `OCI_GetColumnByName` | `OCI_ResultsetGetColumnByName` |
| `OCI_GetColumnCount` | `OCI_ResultsetGetColumnCount` |
| `OCI_GetColumnIndex` | `OCI_ResultsetGetColumnIndex` |
| `OCI_GetCurrentRow` | `OCI_ResultsetGetCurrentRow` |
| `OCI_GetDataLength` | `OCI_ResultsetGetDataLength` |
| `OCI_GetDataSize` | `OCI_ResultsetGetDataSize` |
| `OCI_GetDataSizeByName` | `OCI_ResultsetGetDataSizeByName` |
| `OCI_GetDate` | `OCI_ResultsetGetDate` |
| `OCI_GetDateByName` | `OCI_ResultsetGetDateByName` |
| `OCI_GetDouble` | `OCI_ResultsetGetDouble` |
| `OCI_GetDoubleByName` | `OCI_ResultsetGetDoubleByName` |
| `OCI_GetFile` | `OCI_ResultsetGetFile` |
| `OCI_GetFileByName` | `OCI_ResultsetGetFileByName` |
| `OCI_GetFloat` | `OCI_ResultsetGetFloat` |
| `OCI_GetFloatByName` | `OCI_ResultsetGetFloatByName` |
| `OCI_GetInt` | `OCI_ResultsetGetInt32` |
| `OCI_GetIntByName` | `OCI_ResultsetGetInt32ByName` |
| `OCI_GetInterval` | `OCI_ResultsetGetInterval` |
| `OCI_GetIntervalByName` | `OCI_ResultsetGetIntervalByName` |
| `OCI_GetJson` | `OCI_ResultsetGetJson` |
| `OCI_GetJsonByName` | `OCI_ResultsetGetJsonByName` |
| `OCI_GetLob` | `OCI_ResultsetGetLob` |
| `OCI_GetLobByName` | `OCI_ResultsetGetLobByName` |
| `OCI_GetLong` | `OCI_ResultsetGetLong` |
| `OCI_GetLongByName` | `OCI_ResultsetGetLongByName` |
| `OCI_GetNumber` | `OCI_ResultsetGetNumber` |
| `OCI_GetNumberByName` | `OCI_ResultsetGetNumberByName` |
| `OCI_GetObject` | `OCI_ResultsetGetObject` |
| `OCI_GetObjectByName` | `OCI_ResultsetGetObjectByName` |
| `OCI_GetRaw` | `OCI_ResultsetGetRaw` |
| `OCI_GetRawByName` | `OCI_ResultsetGetRawByName` |
| `OCI_GetRef` | `OCI_ResultsetGetReference` |
| `OCI_GetRefByName` | `OCI_ResultsetGetReferenceByName` |
| `OCI_GetRowCount` | `OCI_ResultsetGetRowCount` |
| `OCI_GetShort` | `OCI_ResultsetGetInt16` |
| `OCI_GetShortByName` | `OCI_ResultsetGetInt16ByName` |
| `OCI_GetStatement` | `OCI_ResultsetGetCursor` |
| `OCI_GetStatementByName` | `OCI_ResultsetGetCursorByName` |
| `OCI_GetString` | `OCI_ResultsetGetString` |
| `OCI_GetStringByName` | `OCI_ResultsetGetStringByName` |
| `OCI_GetStruct` | `OCI_ResultsetGetStruct` |
| `OCI_GetTimestamp` | `OCI_ResultsetGetTimestamp` |
| `OCI_GetTimestampByName` | `OCI_ResultsetGetTimestampByName` |
| `OCI_GetUnsignedBigInt` | `OCI_ResultsetGetUInt64` |
| `OCI_GetUnsignedBigIntByName` | `OCI_ResultsetGetUInt64ByName` |
| `OCI_GetUnsignedInt` | `OCI_ResultsetGetUInt32` |
| `OCI_GetUnsignedIntByName` | `OCI_ResultsetGetUInt32ByName` |
| `OCI_GetUnsignedShort` | `OCI_ResultsetGetUInt16` |
| `OCI_GetUnsignedShortByName` | `OCI_ResultsetGetUInt16ByName` |
| `OCI_GetVector` | `OCI_ResultsetGetVector` |
| `OCI_GetVectorByName` | `OCI_ResultsetGetVectorByName` |
| `OCI_GetXmlType` | `OCI_ResultsetGetXmlType` |
| `OCI_GetXmlTypeByName` | `OCI_ResultsetGetXmlTypeByName` |
| `OCI_IsNull` | `OCI_ResultsetIsColumnNull` |
| `OCI_IsNullByName` | `OCI_ResultsetIsColumnNullByName` |
| `OCI_SetStructNumericType` | `OCI_ResultsetSetStructNumericType` |
| `OCI_SetStructNumericTypeByName` | `OCI_ResultsetSetStructNumericTypeByName` |


### Statement

| v4 name | v5 name |
|---------|---------|
| `OCI_AllowRebinding` | `OCI_StatementAllowRebinding` |
| `OCI_BindArrayGetSize` | `OCI_StatementBindArrayGetSize` |
| `OCI_BindArrayOfBigInts` | `OCI_StatementBindInt64Array` |
| `OCI_BindArrayOfColls` | `OCI_StatementBindCollectionArray` |
| `OCI_BindArrayOfDates` | `OCI_StatementBindDateArray` |
| `OCI_BindArrayOfDoubles` | `OCI_StatementBindDoubleArray` |
| `OCI_BindArrayOfFiles` | `OCI_StatementBindFileArray` |
| `OCI_BindArrayOfFloats` | `OCI_StatementBindFloatArray` |
| `OCI_BindArrayOfIntervals` | `OCI_StatementBindIntervalArray` |
| `OCI_BindArrayOfInts` | `OCI_StatementBindInt32Array` |
| `OCI_BindArrayOfJsons` | `OCI_StatementBindJsonArray` |
| `OCI_BindArrayOfLobs` | `OCI_StatementBindLobArray` |
| `OCI_BindArrayOfNumbers` | `OCI_StatementBindNumberArray` |
| `OCI_BindArrayOfObjects` | `OCI_StatementBindObjectArray` |
| `OCI_BindArrayOfRaws` | `OCI_StatementBindRawArray` |
| `OCI_BindArrayOfRefs` | `OCI_StatementBindReferenceArray` |
| `OCI_BindArrayOfShorts` | `OCI_StatementBindInt16Array` |
| `OCI_BindArrayOfStrings` | `OCI_StatementBindStringArray` |
| `OCI_BindArrayOfTimestamps` | `OCI_StatementBindTimestampArray` |
| `OCI_BindArrayOfUnsignedBigInts` | `OCI_StatementBindUInt64Array` |
| `OCI_BindArrayOfUnsignedInts` | `OCI_StatementBindUInt32Array` |
| `OCI_BindArrayOfUnsignedShorts` | `OCI_StatementBindUInt16Array` |
| `OCI_BindArrayOfVectors` | `OCI_StatementBindVectorArray` |
| `OCI_BindArraySetSize` | `OCI_StatementBindArraySetSize` |
| `OCI_BindBigInt` | `OCI_StatementBindInt64` |
| `OCI_BindBoolean` | `OCI_StatementBindBoolean` |
| `OCI_BindColl` | `OCI_StatementBindCollection` |
| `OCI_BindDate` | `OCI_StatementBindDate` |
| `OCI_BindDouble` | `OCI_StatementBindDouble` |
| `OCI_BindFile` | `OCI_StatementBindFile` |
| `OCI_BindFloat` | `OCI_StatementBindFloat` |
| `OCI_BindInt` | `OCI_StatementBindInt32` |
| `OCI_BindInterval` | `OCI_StatementBindInterval` |
| `OCI_BindJson` | `OCI_StatementBindJson` |
| `OCI_BindLob` | `OCI_StatementBindLob` |
| `OCI_BindLong` | `OCI_StatementBindLong` |
| `OCI_BindNumber` | `OCI_StatementBindNumber` |
| `OCI_BindObject` | `OCI_StatementBindObject` |
| `OCI_BindRaw` | `OCI_StatementBindRaw` |
| `OCI_BindRef` | `OCI_StatementBindReference` |
| `OCI_BindShort` | `OCI_StatementBindInt16` |
| `OCI_BindStatement` | `OCI_StatementBindCursor` |
| `OCI_BindString` | `OCI_StatementBindString` |
| `OCI_BindTimestamp` | `OCI_StatementBindTimestamp` |
| `OCI_BindUnsignedBigInt` | `OCI_StatementBindUInt64` |
| `OCI_BindUnsignedInt` | `OCI_StatementBindUInt32` |
| `OCI_BindUnsignedShort` | `OCI_StatementBindUInt16` |
| `OCI_BindVector` | `OCI_StatementBindVector` |
| `OCI_Describe` | `OCI_StatementDescribe` |
| `OCI_DescribeFmt` | `OCI_StatementDescribeFormat` |
| `OCI_Execute` | `OCI_StatementExecutePrepared` |
| `OCI_ExecuteAndGetResultset` | `OCI_StatementExecutePreparedAndGetResultset` |
| `OCI_ExecuteStmt` | `OCI_StatementExecute` |
| `OCI_ExecuteStmtAndGetResultset` | `OCI_StatementExecuteAndGetResultset` |
| `OCI_ExecuteStmtFmt` | `OCI_StatementExecuteFormat` |
| `OCI_GetAffectedRows` | `OCI_StatementGetAffectedRows` |
| `OCI_GetBatchError` | `OCI_StatementGetBatchError` |
| `OCI_GetBatchErrorCount` | `OCI_StatementGetBatchErrorCount` |
| `OCI_GetBind` | `OCI_StatementGetBind` |
| `OCI_GetBindAllocation` | `OCI_StatementGetBindAllocation` |
| `OCI_GetBindByName` | `OCI_StatementGetBindByName` |
| `OCI_GetBindCount` | `OCI_StatementGetBindCount` |
| `OCI_GetBindIndex` | `OCI_StatementGetBindIndex` |
| `OCI_GetBindMode` | `OCI_StatementGetBindMode` |
| `OCI_GetFetchMode` | `OCI_StatementGetFetchMode` |
| `OCI_GetFetchSize` | `OCI_StatementGetFetchSize` |
| `OCI_GetLongMode` | `OCI_StatementGetLongMode` |
| `OCI_GetNextResultset` | `OCI_StatementGetNextResultset` |
| `OCI_GetParseBindNames` | `OCI_StatementGetParseBindNames` |
| `OCI_GetPieceSize` | `OCI_StatementGetPieceSize` |
| `OCI_GetPrefetchMemory` | `OCI_StatementGetPrefetchMemory` |
| `OCI_GetPrefetchSize` | `OCI_StatementGetPrefetchSize` |
| `OCI_GetResultset` | `OCI_StatementGetResultset` |
| `OCI_GetSQLCommand` | `OCI_StatementGetSqlCommand` |
| `OCI_GetSQLVerb` | `OCI_StatementGetSqlVerb` |
| `OCI_GetSql` | `OCI_StatementGetSql` |
| `OCI_GetSqlErrorPos` | `OCI_StatementGetSqlErrorPos` |
| `OCI_GetSqlIdentifier` | `OCI_StatementGetSqlIdentifier` |
| `OCI_GetStatementType` | `OCI_StatementGetType` |
| `OCI_IsRebindingAllowed` | `OCI_StatementIsRebindingAllowed` |
| `OCI_Parse` | `OCI_StatementParse` |
| `OCI_ParseFmt` | `OCI_StatementParseFormat` |
| `OCI_Prepare` | `OCI_StatementPrepare` |
| `OCI_PrepareFmt` | `OCI_StatementPrepareFormat` |
| `OCI_RegisterBigInt` | `OCI_StatementRegisterInt64` |
| `OCI_RegisterDate` | `OCI_StatementRegisterDate` |
| `OCI_RegisterDouble` | `OCI_StatementRegisterDouble` |
| `OCI_RegisterFile` | `OCI_StatementRegisterFile` |
| `OCI_RegisterFloat` | `OCI_StatementRegisterFloat` |
| `OCI_RegisterInt` | `OCI_StatementRegisterInt32` |
| `OCI_RegisterInterval` | `OCI_StatementRegisterInterval` |
| `OCI_RegisterJson` | `OCI_StatementRegisterJson` |
| `OCI_RegisterLob` | `OCI_StatementRegisterLob` |
| `OCI_RegisterNumber` | `OCI_StatementRegisterNumber` |
| `OCI_RegisterObject` | `OCI_StatementRegisterObject` |
| `OCI_RegisterRaw` | `OCI_StatementRegisterRaw` |
| `OCI_RegisterRef` | `OCI_StatementRegisterReference` |
| `OCI_RegisterShort` | `OCI_StatementRegisterInt16` |
| `OCI_RegisterString` | `OCI_StatementRegisterString` |
| `OCI_RegisterTimestamp` | `OCI_StatementRegisterTimestamp` |
| `OCI_RegisterUnsignedBigInt` | `OCI_StatementRegisterUInt64` |
| `OCI_RegisterUnsignedInt` | `OCI_StatementRegisterUInt32` |
| `OCI_RegisterUnsignedShort` | `OCI_StatementRegisterUInt16` |
| `OCI_RegisterVector` | `OCI_StatementRegisterVector` |
| `OCI_ReleaseResultsets` | `OCI_StatementReleaseResultsets` |
| `OCI_SetBindAllocation` | `OCI_StatementSetBindAllocation` |
| `OCI_SetBindMode` | `OCI_StatementSetBindMode` |
| `OCI_SetFetchMode` | `OCI_StatementSetFetchMode` |
| `OCI_SetFetchSize` | `OCI_StatementSetFetchSize` |
| `OCI_SetLongMode` | `OCI_StatementSetLongMode` |
| `OCI_SetPieceSize` | `OCI_StatementSetPieceSize` |
| `OCI_SetPrefetchMemory` | `OCI_StatementSetPrefetchMemory` |
| `OCI_SetPrefetchSize` | `OCI_StatementSetPrefetchSize` |

### Subscription

| v4 name | v5 name |
|---------|---------|
| `OCI_SubscriptionRegister` | `OCI_SubscriptionCreate` |

---

*Last reviewed against `develop-v5.0.0`. Update this file whenever the public API changes before release.*
