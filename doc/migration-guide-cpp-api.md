# OCILIB v4 → v5 Migration Guide — C++ API

**Audience:** applications using the OCILIB **C++** header-only API (`ocilib.hpp`,
namespace `ocilib`).
For the C layer (`ocilib.h`), see `migration-guide-c-api.md`.

**Status:** draft for v5.0.0 — maintained in `work-v5/` until release; update as the API settles.

> **Tip for automated migration:** this guide is written so it can be fed to an agentic
> coding tool. Every change is stated as a precise find → replace rule with exact
> signatures. Compile after each step; the compiler will surface every call site
> that still needs updating.

---

## 1. Breaking changes at a glance

| # | Change | Effort |
|---|--------|--------|
| 1 | `Environment::Initialize()`/`Cleanup()` statics → `ocilib::Environment` object | Low–Medium |
| 2 | `Environment` static accessors → instance methods | Low (mechanical) |
| 3 | Constructors take `const Environment&` (Connection, Pool, Number, Date, Interval, Timestamp) | Medium |
| 4 | `ocilib::Mutex`, `ocilib::Thread`, `ocilib::ThreadKey` removed | Low–High |
| 5 | `Environment::GetAllocatedBytes(type)` removed; use `GetStats(OCI_UST_BYTES)` | Trivial |
| 6 | Subscription port moved to `Environment::SetSubscriptionPort`; `Dequeue::Subscribe` is now a static batch call | Low–Medium |
| 7 | `Statement::Bind*` returns `BindInfo` instead of `void` (`Register*` unchanged) | None (source-compatible) |
| 8 | C++17 baseline required | None–Low |
| 9 | XA session mode removed | Low |
| 10 | `Exception` model split: `GetType()` = error type, new `GetCategory()`, code getters → `GetCode()` | Low |

**Overall estimate:** a typical app migrates in **under an hour**; the work is
mechanical and compiler-driven. Heavy users of `ocilib::Thread`/`Mutex` or apps that
relied on the implicit singleton need a bit more care (see §4).

---

## 2. Step 1 — `Environment` is now a real object

v4 modelled the library state as a singleton accessed through `static` methods. v5
makes `ocilib::Environment` a **normal RAII object**: constructing it initializes the
library, destroying it frees it.

### v4

```cpp
#include "ocilib.hpp"

using namespace ocilib;

int main()
{
    Environment::Initialize(Environment::Threaded);
    /* ... work ... */
    Environment::Cleanup();
}
```

### v5

```cpp
#include "ocilib.hpp"

using namespace ocilib;

int main()
{
    Environment env(Environment::Threaded);
    /* ... work ... */
}   // env destructor performs the cleanup — no explicit call needed
```

### Exact rules

| v4 (static) | v5 (instance) |
|-------------|---------------|
| `Environment::Initialize(flags, libpath)` | `Environment env(flags, libpath)` — ctor throws `ocilib::Exception` on failure |
| `Environment::Cleanup()` | `env` destructor |
| `Environment::Initialized()` | no longer needed — a constructed `Environment` is always initialized |
| `Environment::GetInstance()` | `Environment::GetInstance(OCI_Environment* env)` — internal-ish; wraps an existing C handle |

The flag enum is unchanged: `Environment::Default`, `Environment::Threaded`,
`Environment::Events`.

> **Note:** v5 supports multiple `Environment` instances (each owns its OCI symbol
> table and handle store). The singleton/global-state assumption is gone — keep a
> reference to `env` and pass it to factories that need it.

---

## 3. Step 2 — `Environment` statics became instance methods

Every former `static` accessor is now called on your `env` object. Mechanical rename:

```cpp
/* v4 */                              /* v5 */
Environment::GetMode();          →    env.GetMode();
Environment::GetImportMode();    →    env.GetImportMode();
Environment::GetCharset();       →    env.GetCharset();
Environment::GetCharMaxSize();   →    env.GetCharMaxSize();
Environment::GetCompileVersion();→    env.GetCompileVersion();
Environment::GetRuntimeVersion();→    env.GetRuntimeVersion();
Environment::EnableWarnings(b);  →    env.EnableWarnings(b);
Environment::SetFormat(t, f);    →    env.SetFormat(t, f);
Environment::GetFormat(t);       →    env.GetFormat(t);
Environment::GetLocaleString(c); →    env.GetLocaleString(c);
Environment::StartDatabase(...); →    env.StartDatabase(...);
Environment::ShutdownDatabase(..);→   env.ShutdownDatabase(...);
Environment::ChangeUserPassword(.);→  env.ChangeUserPassword(...);
Environment::SetHAHandler(h);    →    env.SetHAHandler(h);
```

`GetCompileMajorVersion`/`GetCompileMinorVersion`/`GetCompileRevisionVersion` and the
runtime equivalents follow the same rule.

### Memory / resource statistics

`Environment::GetAllocatedBytes` has been replaced by `Environment::GetStats`.

```cpp
/* v4 */ Environment::GetAllocatedBytes(Environment::AllocatedBytesAll);
/* v5 */ env.GetStats(OCI_UST_BYTES);
```

`Environment::AllocatedBytesFlags` (`AllocatedBytesOracle`, `AllocatedBytesOCILIB`,
`AllocatedBytesAll`) is **removed**. The new method takes an `OCI_Stat` value:

| `stat` value | Meaning |
|--------------|---------|
| `OCI_UST_BYTES`      | Bytes allocated internally by OCILIB for that environment |
| `OCI_UST_HANDLES`    | OCI handles allocated for that environment |
| `OCI_UST_DESCRIPTORS`| OCI descriptors allocated for that environment |
| `OCI_UST_OBJECTS`    | OCILIB object instances allocated for that environment |

```cpp
OCI_UInt64 bytes = env.GetStats(OCI_UST_BYTES);
```

---

## 4. Step 3 — Constructors now take `const Environment&`

Standalone value objects that used to rely on the implicit global environment now take
your `env` explicitly. This is the widest-reaching change.

### Connections and pools

```cpp
/* v4 */
Connection con(db, user, pwd, Environment::SessionDefault);
Pool pool(db, user, pwd, Pool::ConnectionPool, Environment::SessionDefault, 0, 10, 2);
con.Open(db, user, pwd, flags);
pool.Open(db, user, pwd, poolType, flags, min, max, incr);

/* v5 */
Connection con(env, db, user, pwd, Environment::SessionDefault);
Pool pool(env, db, user, pwd, Pool::ConnectionPool, Environment::SessionDefault, 0, 10, 2);
con.Open(env, db, user, pwd, flags);
pool.Open(env, db, user, pwd, poolType, flags, min, max, incr);
```

`Pool::GetConnection(tag)` keeps its signature — the pool carries the environment.

### Value types: `Number`, `Date`, `Interval`, `Timestamp`

Every constructor/allocator that previously defaulted to the global environment now
takes `const Environment&`:

```cpp
/* v4 */                              /* v5 */
Number n(true /*create*/);        →    Number n(env, true);
Number n(value);                  →    Number n(env, value);
Number n(str, format);            →    Number n(env, str, format);

Date d(true);                     →    Date d(env, true);
Date d(str, format);              →    Date d(env, str, format);
Date::SysDate();                  →    Date::SysDate(env);
d.Allocate();                     →    d.Allocate(env.GetHandle());  // see note

Interval i(Interval::DaySecond);  →    Interval i(env, Interval::DaySecond);
Interval i(type, data);           →    Interval i(env, type, data);

Timestamp t(Timestamp::WithTimeZone);→ Timestamp t(env, Timestamp::WithTimeZone);
Timestamp t(type, data);          →    Timestamp t(env, type, data);
Timestamp::SysTimestamp(type);    →    Timestamp::SysTimestamp(env, type);
```

> **Note on `Allocate()`:** the v5 signatures take a raw `OCI_Environment*`
> (`Allocate(OCI_Environment* env)`), e.g. `d.Allocate(env.GetHandle())`. In normal
> code you should prefer the `const Environment&` constructors instead — `Allocate()`
> is the low-level escape hatch.

### Parent-owned objects: unchanged (no `env` needed)

Objects created from a `Connection`, `Statement` or `TypeInfo` do **not** take
`env` — the environment is deduced from the parent handle:

```cpp
Statement stmt(con);                 // con carries the environment
Object obj(typeInfo);                // typeInfo carries it
Reference ref(typeInfo);
Collection<T> coll(typeInfo);
Lob<Clob>/Clob lob(con);
File file(con);                      // and File(con, dir, name)
Long<Clong>/Clong lg(stmt);
Message msg(typeInfo);
Agent agent(con, name, address);
Enqueue eq(typeInfo, queueName);
Dequeue dq(typeInfo, queueName);
DirectPath dp(typeInfo, nbCols, nbRows, partition);
TypeInfo ti(con, name, type);
Transaction tr(con, timeout, flags); // via Connection::StartTransaction(...)
Vector v(con);
```

> **Why:** in v5, every `OCI_*` handle embeds its owning `OCI_Environment*`
> (`OCI_HandleGetEnvironment` on the C side). Internally,
> `HandleHolder::AcquireAllocated/AcquireTransient` no longer take an explicit
> `OCI_Environment*` — the environment is resolved from the parent or the handle
> itself.

`Subscription`, `Resultset`, `XmlType`, `Queue`, `QueueTable`, `Column`,
`Event`, `BindInfo` are unchanged.

---

## 5. Step 4 — Remove `ocilib::Mutex` / `ocilib::Thread` / `ocilib::ThreadKey`

All three classes are deleted along with the underlying C API. Replace with standard
C++ facilities:

| v4 | v5 (standard C++) |
|----|-------------------|
| `ocilib::Mutex m; m.Acquire(); m.Release();` | `std::mutex m; { std::lock_guard<std::mutex> lk(m); ... }` |
| `ocilib::Thread t; t.Run(func, arg); t.Join();` | `std::thread t(func, arg); t.join();` |
| `ocilib::ThreadKey::Create(name, dtor); SetValue/GetValue` | `thread_local` variables, or `std::map<std::thread::id, T>` |

> **Effort:** trivial for a couple of mutexes; more work if you built abstractions on
> `ocilib::Thread`. `ocilib::ThreadKey`'s name-keyed global registry has no standard
> equivalent — most uses translate to `thread_local`.

The library itself is still thread-safe — pass `Environment::Threaded` to the
`Environment` constructor as before.

---

## 6. Step 5 — Notifications and AQ

### 6.1 Port is configured on the environment

```cpp
/* v4 — port passed at each registration */
sub.Register(con, name, changes, handler, port, timeout);

/* v5 — configure once on the environment */
env.SetSubscriptionPort(port);
unsigned int port = env.GetSubscriptionPort();   // read back / auto-assigned
```

### 6.2 `Subscription::Register` lost its `port` parameter, gained `mode`

```cpp
/* v4 */ void Register(const Connection&, const ostring& name, ChangeTypes,
                       NotifyHandlerProc, unsigned int port = 0, unsigned int timeout = 0);
/* v5 */ void Register(const Connection&, const ostring& name, ChangeTypes,
                       NotifyHandlerProc,
                       SubscriptionMode mode = Subscription::ModeDefault,
                       unsigned int timeout = 0);
```

`mode` is a `Subscription::SubscriptionMode` enum (`Subscription::ModeDefault` /
`Subscription::ModeSecureNotification`, i.e. `OCI_SRM_*`). Keep the default to
preserve v4 behavior; `ModeSecureNotification` requires Oracle client 12c+.

### 6.3 `Dequeue::Subscribe` is now a static batch call

```cpp
/* v4 — instance method, one dequeue, explicit port */
dequeue.Subscribe(port, timeout, handler);

/* v5 — static, registers all dequeues at once */
std::vector<Dequeue> queues = { dq1, dq2, dq3 };
Dequeue::Subscribe(con, queues, timeout, handler);
```

`Environment::Events` must still be passed to the `Environment` constructor for
notifications to work.

---

## 6b. Step 5b — `Bind` returns `BindInfo`; `BindInfo::SetDirection` added

All `Statement::Bind<…>(...)` overloads return a `BindInfo` object wrapping the
created `OCI_Bind` handle instead of `void` — **`Statement::Register<…>` is
unchanged** and still returns `void`:

```cpp
/* v4 */ st.Bind<int>(":n", n, BindInfo::In);
/* v5 */ BindInfo b = st.Bind<int>(":n", n, BindInfo::In);
         b.SetDirection(BindInfo::Out);      // new setter — no GetBind() needed
```

Fully source-compatible — ignoring the return value is legal. `Statement::GetBind()`
still exists for retrieval by index/name. On failure an `ocilib::Exception` is thrown
as before. The private `Statement::SetLastBindMode` helper is gone; direction is set
through the returned `BindInfo` (new `BindInfo::SetDirection`).

> **Naming note:** the `OCI_Bind` wrapper class remains `BindInfo`, not `Bind` —
> `Statement` already declares member functions named `Bind(...)`, so inside
> `Statement`'s scope the identifier `Bind` would resolve to the member function and
> collide with the type name. `BindInfo` stays.

## 6c. Step 5c — `Exception`: type, category and unified error code

v4's `Exception::ExceptionType` conflated the **error type** (error vs warning)
with the **origin** (Oracle vs OCILIB) in a single enum, and split the error code
across two getters. v5 separates the three axes:

| Axis | v4 | v5 |
|------|----|----|
| Type | `GetType()` → `Exception::OracleWarning` | `GetType()` → `Exception::Error` / `Exception::Warning` |
| Origin | `GetType()` → `Exception::OracleError` / `Exception::OcilibError` | `GetCategory()` → `Exception::OracleError` / `Exception::OcilibError` / `Exception::JsonError` |
| Code | `GetOracleErrorCode()` / `GetInternalErrorCode()` | `GetCode()` — code in the category's namespace |

### Exact rules

| v4 | v5 |
|----|----|
| `e.GetType() == Exception::OracleError` | `e.GetCategory() == Exception::OracleError` |
| `e.GetType() == Exception::OcilibError` | `e.GetCategory() == Exception::OcilibError` |
| `e.GetType() == Exception::OracleWarning` | `e.GetType() == Exception::Warning` |
| `e.GetOracleErrorCode()` | `e.GetCode()` |
| `e.GetInternalErrorCode()` | `e.GetCode()` |

- `Exception::GetType()` keeps its name but now returns the **error type** only —
  an `ExceptionType` (`Exception::Error` = `OCI_ERR_TYPE_ERROR`,
  `Exception::Warning` = `OCI_ERR_TYPE_WARNING`).
- **New** `Exception::GetCategory()` returns an `ExceptionCategory` — which
  subsystem raised the error: `Exception::OracleError` (`OCI_ERR_CAT_OCI`),
  `Exception::OcilibError` (`OCI_ERR_CAT_LIB`), `Exception::JsonError`
  (`OCI_ERR_CAT_JZN`).
- **Removed** `Exception::GetOracleErrorCode()` and
  `Exception::GetInternalErrorCode()` — use `UInt32 Exception::GetCode()`,
  whose namespace is given by `GetCategory()` (ORA-XXXXX for `OracleError`,
  an `OCI_ERR_*` code for `OcilibError`, a JZN `jsonerr` for `JsonError`).
- `Exception::OracleError` / `Exception::OcilibError` keep their names but moved
  from `ExceptionTypeValues` to `ExceptionCategoryValues`; `OracleWarning` was
  replaced by the type value `Exception::Warning`. Comparing `GetType()`
  against a category value no longer compiles (`core::Enum` only compares
  against its own enum type) — the compiler catches every stale site.

```cpp
/* v4 */
catch (Exception& e)
{
    if (e.GetType() == Exception::OracleError)
        cout << "ORA-" << e.GetOracleErrorCode() << ": " << e.GetMessage();
}

/* v5 */
catch (Exception& e)
{
    if (e.GetCategory() == Exception::OracleError)
        cout << "ORA-" << e.GetCode() << ": " << e.GetMessage();
}
```

## 7. Step 6 — XA removal

`Environment::SessionXA` / `OCI_SESSION_XA` is gone. Remove it from session flags; XA
is no longer supported through OCILIB.

---

## 8. Behavioural changes with unchanged signatures

Do not rely on the compiler alone — these compile fine but behave differently:

1. **Errors are always per-thread.** The C layer no longer needs an opt-in flag for
   thread-local errors; `ocilib::Exception` reflects the calling thread's error. No
   code change needed — just be aware the behavior is uniform now.
2. **`Environment` is not a singleton.** Code that assumed "one global environment"
   (e.g. helper functions creating `Connection`s without parameters) now needs the
   `env` passed in explicitly — this is the main source of call-site churn.
3. **Row-level notifications now fire.** If you subscribe with `ChangeType::Row`
   (`OCI_CNT_ROWS`), you now actually receive per-row events with rowids. Audit
   handlers that assumed rows were never delivered.
4. **AQ dequeue registration is atomic.** `Dequeue::Subscribe` registers all queues in
   one OCI call — fixes `ORA-24915` (listener restart) failures.
5. **New opt-in logging** (process-wide, env-var driven): `OCILIB_LOG`,
   `OCILIB_LOG_CATEGORIES`, `OCILIB_LOG_OUTPUT` — set before constructing
   `Environment`. Useful for debugging a migration.
6. **Copy semantics are now `std::shared_ptr`-backed.** Handle wrappers still behave
   as reference-counted smart handles — semantics are preserved, but the old manual
   reference-list internals are gone. No source change expected.

---

## 9. Non-breaking details

- **`ocilib::Exception`** still derives from `std::exception` and is thrown on C API
  failures — `try`/`catch` structure is unchanged, but the error accessors were
  reworked (`GetType`/`GetCategory`/`GetCode` — see §6c).
- **Stack-object / RAII model unchanged** — objects are still created on the stack,
  handles freed by the last reference going out of scope.
- **`IsNull()` / default-constructed objects** — same contract.
- **New `Statement` helpers** (pure additions):

  ```cpp
  Resultset rs = stmt.ExecuteAndGetResultset();          // ExecutePrepared() + GetResultset()
  Resultset rs = stmt.ExecuteAndGetResultset(sql);       // Execute(sql) + GetResultset()
  ```
- **Lob read/write unchanged at the C++ level** — the underlying C calls
  (`OCI_LobRead`/`Write`/`Append`) now take char/byte count out-params, but the
  `Lob<>::Read`/`Write`/`Append` method signatures are unchanged.
- **`Collection<T>` accessors unchanged** — the C `OCI_CollGetElem2` was removed
  (`OCI_CollGetElem` keeps its v4 form); `Collection<T>::Get/Set/IsElementNull/
  SetElementNull` signatures and semantics are unchanged.
- **Short scalar aliases added** (pure additions) — `ocilib::Int16`, `UInt16`,
  `Int32`, `UInt32`, `Int64`, `UInt64`, `Float`, `Double`, `Size` alias the C
  API types `OCI_Int16` … `OCI_Size`. Both spellings are the same type:
  `rs.Get<ocilib::Int64>` and `rs.Get<OCI_Int64>` are identical.
- **Internal header layout changed** (`detail/core/*`, `detail/support/*` reorganized,
  some files removed: `Mutex.hpp`, `Thread.hpp`, `ThreadKey.hpp`, `ManagedBuffer.hpp`,
  `Synchronizable.hpp`, `ConcurrentList.hpp`, `ConcurrentMap.hpp`,
  `SynchronizationGuard.hpp`, `HandleStoreResolver.hpp`). Only `#include "ocilib.hpp"`
  — never include `detail/` headers directly.

---

## 10. Migration checklist (agent-friendly order)

```
1.  [ ] Create `ocilib::Environment env(flags)` at program start; store it
        where it outlives all OCILIB objects.
2.  [ ] Delete Environment::Initialize / Cleanup / Initialized calls.
3.  [ ] Change Environment::StaticMethod() → env.Method() for all accessors
        (GetMode, GetImportMode, GetCharset, GetCharMaxSize, version getters,
        EnableWarnings, SetFormat, GetFormat, GetLocaleString, StartDatabase,
        ShutdownDatabase, ChangeUserPassword, SetHAHandler).
4.  [ ] GetAllocatedBytes(type) → env.GetStats(OCI_UST_BYTES); drop AllocatedBytesFlags and use OCI_UST_* for other counters.
5.  [ ] Add `env` as first arg to Connection and Pool ctors / Open() calls.
6.  [ ] Add `env` to Number/Date/Interval/Timestamp ctors, SysDate/SysTimestamp,
        and any Allocate() calls (env.GetHandle()).
7.  [ ] Replace ocilib::Mutex/Thread/ThreadKey with <mutex>/<thread>/thread_local.
8.  [ ] Remove Environment::SessionXA / XA usage.
9.  [ ] env.SetSubscriptionPort(port) before subscriptions; drop `port` arg from
        Subscription::Register; convert Dequeue::Subscribe to the static batch form.
10. [ ] Exception handling (see §6c): GetType() comparisons against
        OracleError/OcilibError → GetCategory(); OracleWarning →
        GetType() == Exception::Warning; GetOracleErrorCode() /
        GetInternalErrorCode() → GetCode().
11. [ ] Recompile with C++17+; fix remaining errors.
12. [ ] Audit ChangeType::Row notification handlers for new row events.
```

---

## 11. Worked example — before / after

```cpp
/* --------------------------- v4 --------------------------- */
#include "ocilib.hpp"
using namespace ocilib;

int main()
{
    Environment::Initialize(Environment::Threaded);

    Connection con("db", "usr", "pwd");
    Statement  st(con);
    Date       d = Date::SysDate();

    /* ... work ... */

    Environment::Cleanup();
    return 0;
}
```

```cpp
/* --------------------------- v5 --------------------------- */
#include "ocilib.hpp"
using namespace ocilib;

int main()
{
    Environment env(Environment::Threaded);   // RAII — freed at scope exit

    Connection con(env, "db", "usr", "pwd");
    Statement  st(con);                        // unchanged — parented by con
    Date       d = Date::SysDate(env);

    /* ... work ... */

    return 0;   // no explicit cleanup needed
}
```

---

*Last reviewed against `develop-v5.0.0`. Update this file whenever the public API changes before release.*
