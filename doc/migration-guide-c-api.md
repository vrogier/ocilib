# OCILIB v4 → v5 Migration Guide — C API

**Audience:** applications using the OCILIB **C** library (`ocilib.h`).
For the C++ layer (`ocilib.hpp`), see `migration-guide-cpp-api.md`.

**Status:** draft for v5.0.0 — maintained in `work-v5/` until release; update as the API settles.

> **Tip for automated migration:** this guide is written so it can be fed to an agentic
> coding tool. Every change is stated as a precise find → replace rule with exact
> signatures. Compile after each step; the compiler will surface every call site
> that still needs updating.

---

## 1. Breaking changes at a glance

| # | Change | Effort |
|---|--------|--------|
| 1 | `OCI_Initialize`/`OCI_Cleanup` → `OCI_EnvironmentCreate`/`OCI_EnvironmentFree` | Low |
| 2 | Many top-level calls now take an `OCI_Environment*` first argument | Medium |
| 3 | `boolean` → `OCI_Boolean` | Low (mechanical) |
| 4 | Threading API removed (`OCI_Mutex`/`OCI_Thread`/`OCI_ThreadKey`) | Low–High (depends on usage) |
| 5 | XA / `OCI_SESSION_XA` removed | Low |
| 6 | Raw `OCI_HandleGet*` accessors removed | Low–Medium |
| 7 | Subscription port moved to the environment; `OCI_DequeueSubscribe` is now a batch call | Low–Medium |
| 8 | `OCI_GetAllocatedBytes` removed; use `OCI_GetStats(env, OCI_UST_BYTES)` | Trivial |
| 9 | `compat.h` removed — all v3-era compatibility macros gone | Low–Medium |
| 10 | `OCI_Bind{Type}` / `OCI_BindArrayOf{Type}` return `OCI_Bind*` instead of `OCI_Boolean` | Low |
| 11 | New typed typedefs replace bare `unsigned int` parameters | None (source-compatible) |
| 12 | `big_int`/`big_uint` → `OCI_Int64`/`OCI_UInt64` (aliases kept) | None (source-compatible) |

**Overall estimate:** a typical application (connect → statement → fetch) migrates in
**under an hour** — mostly mechanical edits caught by the compiler. Applications that
use the removed threading API, XA, or raw OCI handles need **real porting work**
(see §4 and §5).

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
unsigned int OCI_GetOCICompileVersion(OCI_Environment *env);
unsigned int OCI_GetOCIRuntimeVersion(OCI_Environment *env);
OCI_ImportMode OCI_GetImportMode(OCI_Environment *env);
OCI_Charset    OCI_GetCharset(OCI_Environment *env);
OCI_UInt64     OCI_GetStats(OCI_Environment *env, OCI_Stat stat); /* see §7 */
OCI_Boolean    OCI_EnableWarnings(OCI_Environment *env, OCI_Boolean value);
OCI_Boolean    OCI_SetErrorHandler(OCI_Environment *env, POCI_ERROR handler);
OCI_Boolean    OCI_SetHAHandler(OCI_Environment *env, POCI_HA_HANDLER handler);
const otext*   OCI_GetLocaleString(OCI_Environment *env, OCI_LocaleCode code);
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
                        unsigned int min_con, unsigned int max_con, unsigned int incr_con);

/* v4 */ OCI_PoolGetConnection(OCI_Pool *pool, const otext *tag);
/* v5 */ OCI_PoolGetConnection(OCI_Environment *env, OCI_Pool *pool, const otext *tag);

/* v4 */ OCI_PoolFree(OCI_Pool *pool);
/* v5 */ OCI_PoolFree(OCI_Environment *env, OCI_Pool *pool);
```

**Standalone datatype objects** (previously took `OCI_Connection *con` — now owned by
the environment, not a connection):

```c
OCI_Date*      OCI_DateCreate(OCI_Environment *env);
OCI_Date**     OCI_DateArrayCreate(OCI_Environment *env, unsigned int nbelem);

OCI_Number*    OCI_NumberCreate(OCI_Environment *env);
OCI_Number**   OCI_NumberArrayCreate(OCI_Environment *env, unsigned int nbelem);

OCI_Timestamp*   OCI_TimestampCreate(OCI_Environment *env, OCI_TimestampType type);
OCI_Timestamp**  OCI_TimestampArrayCreate(OCI_Environment *env, OCI_TimestampType type,
                                          unsigned int nbelem);

OCI_Interval*    OCI_IntervalCreate(OCI_Environment *env, OCI_IntervalType type);
OCI_Interval**   OCI_IntervalArrayCreate(OCI_Environment *env, OCI_IntervalType type,
                                         unsigned int nbelem);
```

**Hash tables, database control:**

```c
OCI_HashTable* OCI_HashCreate(OCI_Environment *env, unsigned int size, OCI_HashType type);

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
```

The following constructors **dropped their `OCI_Connection*` argument** — the owning
connection is now deduced from the `OCI_TypeInfo` handle itself:

```c
OCI_ObjectCreate(OCI_TypeInfo *typinf);
OCI_ObjectArrayCreate(OCI_TypeInfo *typinf, unsigned int nbelem);
OCI_RefCreate(OCI_TypeInfo *typinf);
OCI_RefArrayCreate(OCI_TypeInfo *typinf, unsigned int nbelem);
OCI_CollArrayCreate(OCI_TypeInfo *typinf, unsigned int nbelem);
```

### 3.3 Recovering an environment from a handle

If you need the environment inside a callback or deep in call stacks, use the new helper:

```c
OCI_Environment* OCI_HandleGetEnvironment(void *handle);   /* any OCILIB handle */
```

---

## 4. Step 3 — Replace the removed threading API

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

## 5. Step 4 — Remove XA usage

`OCI_SESSION_XA` and the associated documentation are removed. OCILIB no longer offers
XA / distributed-transaction connections.

**Action:** delete `OCI_SESSION_XA` from session modes. If you need XA, manage the
resource manager integration outside OCILIB (or stay on v4 for that component).

---

## 6. Step 5 — Update notification / AQ code

### 6.1 Subscription port is now per-environment

v4 passed a `port` argument on every registration. v5 configures it once on the
environment, before the first subscription:

```c
OCI_Boolean  OCI_SetSubscriptionPort(OCI_Environment *env, unsigned int port);
unsigned int OCI_GetSubscriptionPort(OCI_Environment *env);
```

### 6.2 `OCI_SubscriptionRegister` lost its `port` parameter

```c
/* v4 */
OCI_SubscriptionRegister(con, name, type, handler, port, timeout);

/* v5 */
OCI_SubscriptionRegister(con, name, type, handler, timeout);
```

### 6.3 `OCI_DequeueSubscribe` is now a batch call

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

## 7. Step 6 — Memory / resource statistics

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

## 8. Step 7 — `boolean` → `OCI_Boolean`

v4 exported `#define boolean int`, which collided with other libraries and polluted
every translation unit. v5 uses a real typedef:

```c
typedef int OCI_Boolean;
```

`TRUE`/`FALSE` are unchanged. Replace `boolean` with `OCI_Boolean` in your code —
a straight textual rename (watch out for `struct boolean` fields if you had any).
A compatibility `typedef OCI_Boolean boolean;` may exist in the headers; do not rely
on it for new code.

---

## 8b. Step 7b — `big_int`/`big_uint` → `OCI_Int64`/`OCI_UInt64`

v5 introduces fixed-width integer typedefs in `platform.h`:

```c
typedef short              OCI_Int16;
typedef unsigned short     OCI_UInt16;
typedef int                OCI_Int32;
typedef unsigned int       OCI_UInt32;
typedef long long          OCI_Int64;
typedef unsigned long long OCI_UInt64;
typedef size_t             OCI_Size;
```

`big_int` and `big_uint` remain as compatibility typedefs, now in
`ocilibc/compat.h` (`typedef OCI_Int64 big_int; typedef OCI_UInt64 big_uint;`).
`ocilib.h` does not include `compat.h`, so code using the old names must add
`#include "ocilibc/compat.h"` explicitly. Rename when convenient; do not use the
old names in new code.

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
- `char` and `unsigned char` remain `char`/`unsigned char` — they denote
  characters and raw bytes, not integer quantities.

---

## 9. Step 8 — Raw OCI handle accessors removed

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

## 10. Step 9 — Removed compatibility macros (`compat.h`)

`include/ocilibc/compat.h` has been **deleted**. It contained backward-compatibility
macros mapping pre-v4 names onto the modern API. All of these aliases are gone —
rename each usage to its modern equivalent:

| Removed alias | Replacement |
|---------------|-------------|
| `OCI_CreateConnection` / `OCI_FreeConnection` | `OCI_ConnectionCreate` / `OCI_ConnectionFree` |
| `OCI_CreateStatement` / `OCI_FreeStatement` | `OCI_StatementCreate` / `OCI_StatementFree` |
| `OCI_CreateTransaction` / `OCI_FreeTransaction` | `OCI_TransactionCreate` / `OCI_TransactionFree` |
| `OCI_CreateHashTable` / `OCI_FreeHashTable` | `OCI_HashCreate` / `OCI_HashFree` |
| `OCI_GetColumn*` (`Name`, `Type`, `CharsetForm`, `SQLType`, `FullSQLType`, `Size`, `Scale`, `Precision`, `FractionnalPrecision`, `LeadingPrecision`, `Nullable`, `CharUsed`) | `OCI_ColumnGet<…>` (same suffix; note `Fractionnal` → `Fractional` spelling fix) |
| `OCI_GetFormatDate(s)` / `OCI_SetFormatDate(s,f)` | `OCI_GetFormat(OCI_HandleGetEnvironment(OCI_StatementGetConnection(s)), OCI_FMT_DATE)` / `OCI_SetFormat(…, OCI_FMT_DATE, f)` |
| `OCI_ERR_API` | `OCI_ERR_ORACLE` |
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

## 10b. Step 10 — Bind/register calls now return `OCI_Bind*`

All input bind-registration functions now return the created `OCI_Bind*` handle
instead of a boolean. `NULL` means failure (as before — an error is also recorded
and retrievable via `OCI_GetLastError()`).

Affected: `OCI_BindBoolean`, `OCI_Bind{Number,Short,UnsignedShort,Int,UnsignedInt,BigInt,UnsignedBigInt,String,Raw,Double,Float,Date,Timestamp,Interval,Lob,File,Object,Vector,Coll,Ref,Statement,Long}` and all `OCI_BindArrayOf{…}` variants.
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

## 10c. Step 11 — `*2` suffixed functions renamed / promoted

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
unsigned int n;                                 unsigned int cc = len, bc = 0;
n = OCI_LobWrite(lob, buf, len);            →   OCI_LobWrite(lob, buf, &cc, &bc);
n = OCI_LobRead(lob, buf, SIZE);            →   OCI_LobRead(lob, buf, &cc, &bc);
```

- Signature: `OCI_Boolean OCI_LobRead(lob, buffer, unsigned int *char_count, unsigned int *byte_count)` — counts are in/out.
- For CLOBs use `char_count`; for BLOBs use `byte_count`. Passing `0` for the unused
  count on input lets OCILIB compute it.
- Return value moved to `OCI_Boolean` — check the out counts for the amount transferred.

### `OCI_CollGetElem2` removed

`OCI_CollGetElem(coll, index)` keeps its v4 signature (`OCI_Elem*` return, shared
collection-owned slot). The redundant `OCI_CollGetElem2` was simply deleted —
if you used it, call `OCI_CollGetElem` instead.

## 11. Unchanged signatures — changed behaviour

> **Warning:** a migration is not only about signatures. Several functions kept
> exactly the same prototype in v5 but behave differently. The compiler will **not**
> flag these — audit them deliberately.

1. **`OCI_GetLastError(void)`** — same prototype, different semantics. In v4 it
   returned the *global* last error unless you opted into thread-local errors with
   `OCI_ENV_CONTEXT`. In v5 it **always** returns the calling thread's `OCI_Error`;
   the flag is gone and there is no global error slot. Code that read "the last error
   produced anywhere" must be reworked to read errors on the thread that produced them.
   New helper: `OCI_Boolean OCI_HasError(void)` — quick check without fetching the
   handle.
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
6. **`OCI_IPC_*` values were renumbered** (and thread/mutex/thread-key IDs removed).
   Never hard-code the numeric values; use the macros.
7. **`OCI_Environment` is public but opaque** — you never dereference it.
8. **One environment per application** is the documented pattern, but multiple
   environments are supported (each loads its own OCI symbol table). Two environments
   can even load *different* OCI client libraries.
9. **New opt-in logging.** Set environment variables before `OCI_EnvironmentCreate`:
   - `OCILIB_LOG` — `NONE` (default), `ERROR`, `WARN`, `INFO`, `TRACE`, `DEBUG`
   - `OCILIB_LOG_CATEGORIES` — `LIB`, `OCI`, `ALL` (default)
   - `OCILIB_LOG_OUTPUT` — `STDERR` (default), `STDOUT`, or a file path

---

## 12. Non-breaking changes you can ignore (or adopt)

- **Typed parameter typedefs** — `OCI_SessionMode`, `OCI_PoolType`, `OCI_LobType`,
  `OCI_FileType`, `OCI_LongType`, `OCI_TimestampType`, `OCI_IntervalType`,
  `OCI_TypeInfoType`, `OCI_SubscriptionType`, `OCI_HashType`, `OCI_EnvMode`,
  `OCI_ImportMode`, `OCI_Charset`, `OCI_LocaleCode`, `OCI_ErrorCode`,
  `OCI_FailoverType/Event/Code`, `OCI_HAEventSource/Type`,
  `OCI_DatabaseStartMode/Flag`, `OCI_DatabaseShutdownMode/Flag`, `OCI_SqlType`, etc.
  All are `unsigned int` (or `int`) aliases — **source-compatible**; passing literals
  or `unsigned int` still compiles. Exception: **callback typedefs must match
  exactly** — update your `POCI_TAF_HANDLER` / `POCI_HA_HANDLER` implementations to
  the typed signatures:

  ```c
  OCI_FailoverCode my_taf(OCI_Connection *con, OCI_FailoverType type, OCI_FailoverEvent event);
  void my_ha(OCI_Connection *con, OCI_HAEventSource src, OCI_HAEventType evt, OCI_Timestamp *t);
  ```

- **`OCI_ErrorCode`** (`typedef int`) for error codes — cosmetic.

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
  stays explicit (it needs the buffer capacity argument). C only — the C++ API
  does not expose the variadic helpers.

---

## 13. Migration checklist (agent-friendly order)

```
1.  [ ] Introduce `OCI_Environment *env` at program start.
2.  [ ] Replace OCI_Initialize(...)      → env = OCI_EnvironmentCreate(err, lib, mode);
3.  [ ] Remove OCI_ENV_CONTEXT from mode flags.
4.  [ ] Replace OCI_Cleanup()            → OCI_EnvironmentFree(env);
5.  [ ] Add `env` as first arg to: OCI_ConnectionCreate, OCI_PoolCreate,
        OCI_PoolGetConnection, OCI_PoolFree, OCI_DateCreate/ArrayCreate,
        OCI_NumberCreate/ArrayCreate, OCI_TimestampCreate/ArrayCreate,
        OCI_IntervalCreate/ArrayCreate, OCI_HashCreate,
        OCI_DatabaseStartup, OCI_DatabaseShutdown,
        OCI_GetOCICompileVersion, OCI_GetOCIRuntimeVersion, OCI_GetImportMode,
        OCI_GetCharset, OCI_GetStats, OCI_GetLocaleString,
        OCI_EnableWarnings, OCI_SetErrorHandler, OCI_SetHAHandler.
6.  [ ] Rename every `boolean` → `OCI_Boolean`.
7.  [ ] Replace OCI_Mutex*/OCI_Thread*/OCI_ThreadKey* with <threads.h>.
8.  [ ] Remove OCI_SESSION_XA.
9.  [ ] Call OCI_SetSubscriptionPort(env, port) before subscriptions;
        drop `port` arg from OCI_SubscriptionRegister;
        convert OCI_DequeueSubscribe to (con, array, count, timeout, cb).
10. [ ] OCI_GetAllocatedBytes(mem_type) → OCI_GetStats(env, OCI_UST_BYTES); use OCI_UST_* for other counters.
11. [ ] Remove all OCI_HandleGet* except OCI_HandleGetEnvironment(handle)
        (return type is now OCI_Environment*).
12. [ ] Update POCI_TAF_HANDLER / POCI_HA_HANDLER signatures to typed params.
13. [ ] Recompile; fix remaining errors (all remaining breaks are compile-time).
14. [ ] Replace any v3-era compat macros (OCI_ConnPool*, OCI_Schema*,
        OCI_GetColumn*, OCI_SetNull*, mts*/dts* string helpers, MT/DT, …)
        with the modern names — see §10.
15. [ ] Audit OCI_CNT_ROWS notification handlers for the new row events.
```

---

## 14. Worked example — before / after

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

*Last reviewed against `develop-v5.0.0`. Update this file whenever the public API changes before release.*
