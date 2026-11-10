# OCILIB v4 → v5 Migration Guide — C++ API

**Audience:** applications using the OCILIB **C++** header-only API (`ocilib.hpp`,
namespace `ocilib`).
For the C layer (`ocilib.h`), see `migration-guide-c-api.md`.

**Status:** draft for v5.0.0 — maintained in `work-v5/` until release; update as the API settles.

> **Tip for automated migration:** this guide is written so it can be fed to an agentic
> coding tool. Every change is stated as a precise find → replace rule with exact
> signatures. Compile after each step; the compiler will surface every call site
> that still needs updating.

> **How to read this guide:** changes are ordered from most impactful to least.
> Steps 1–3 touch every program (compile-time). Step 4 is the dangerous one:
> it compiles silently but changes runtime behaviour. Steps 5–9 are mechanical or
> feature-scoped. Steps 10–11 only affect users of specific features.

---

## 1. Breaking changes at a glance

| # | Change | Effort |
|---|--------|--------|
| 1 | `Environment::Initialize()`/`Cleanup()` statics → `ocilib::Environment` object | Low–Medium |
| 2 | `Environment` static accessors → instance methods | Low (mechanical) |
| 3 | Constructors take `const Environment&` (Connection, Pool, Number, Date, Interval, Timestamp) | Medium |
| 4 | **All** indexes switch from 1-based to **0-based**: `coll[i]`/`node[i]`, iterators, and every index-taking method; index lookups return `Int32` with `-1` = miss | Medium–High (runtime, not compile-time) |
| 5 | `Exception` model split: `GetType()` = error type, new `GetCategory()`, code getters → `GetCode()` | Low |
| 6 | Subscription port moved to `Environment::SetSubscriptionPort`; `Dequeue::Subscribe` is now a static batch call | Low–Medium |
| 7 | `Statement::Bind*` returns `BindInfo` instead of `void` (`Register*` unchanged) | None (source-compatible) |
| 8 | `ocilib::Mutex`, `ocilib::Thread`, `ocilib::ThreadKey` removed | Low–High |
| 9 | `Environment::SessionXA` removed | Low |
| 10 | `Environment::GetAllocatedBytes(type)` removed; use `GetStats(OCI_UST_BYTES)` | Trivial |
| 11 | C++17 baseline required | None–Low |
| 12 | New views `rs.Rows()`, `rs.Columns()`, `ti.Columns()` — `Row`-based access + column descriptors | None (additive) |

**Overall estimate:** a typical app migrates in **under an hour**; the work is
mechanical and compiler-driven. The real risk is **Step 4 (0-based indexes)** —
it is a silent runtime change. Heavy users of `ocilib::Thread`/`Mutex` also need
a bit more care (see §10).

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

Interval i(Interval::DaySecond);  →    Interval i(env, Interval::DaySecond);
Interval i(type, data);           →    Interval i(env, type, data);

Timestamp t(Timestamp::WithTimeZone);→ Timestamp t(env, Timestamp::WithTimeZone);
Timestamp t(type, data);          →    Timestamp t(env, type, data);
Timestamp::SysTimestamp(type);    →    Timestamp::SysTimestamp(env, type);
```

> **Note:** there is no public `Allocate()` escape hatch anymore — the internal
> `Allocate(OCI_Environment*, core::Handle*)` is private. Always construct value
> types through the `const Environment&` constructors (or `SysDate`/`SysTimestamp`).

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

## 5. Step 4 — `[]` and iterators switch to 0-based (the silent trap); new `Rows()`/`Columns()` views

### 5.1 Breaking — all indexes switch to 0-based

In v4, every index in the C++ API was 1-based, like the C API. In v5, **all
indexes are 0-based uniformly** — `operator[]`, iterators, and every
index-taking method (`Get`, `GetItem`, `GetFieldByIndex`, `GetColumn`, `Seek`,
`SetEntry`, …). Name-lookup and position accessors that can miss now return
`Int32` with `-1` meaning "not found / no position" instead of `0`.

> **This is the highest-risk change in v5.** `coll[1]`, `rs.Get<String>(1)`,
> `for (i = 1; i <= n; ++i)` all still compile — they just read the *wrong*
> element. Audit every literal index and every index loop deliberately.

```cpp
/* ---- v4 ---- */
for (OCI_UInt32 i = 1, n = coll.GetSize(); i <= n; ++i)
    process(coll[i]);                     // coll[1] = first element
JsonNode item = node[1];                  // node[1] = first JSON item
auto it = coll.begin();                   // position 1
for (; it != coll.end(); ++it) { /* ... */ }

/* ---- v5 ---- */
for (OCI_UInt32 i = 0, n = coll.GetSize(); i < n; ++i)
    process(coll[i]);                     // coll[0] = first element
JsonNode item = node[0];                  // node[0] = first JSON item
auto it = coll.begin();                   // position 0 — end() is now GetCount()
for (; it != coll.end(); ++it) { /* ... */ }  // unchanged — iterators self-adjust
```

### Exact rules

| v4 | v5 |
|----|----|
| `coll[i]`, `node[i]` — `i` 1-based | same syntax, `i` **0-based** |
| `Collection<T>::begin()`/`end()` — positions 1 / `count+1` | positions **0** / `count` |
| `JsonNode::begin()`/`end()` — positions 1 / `count+1` | positions **0** / `count` |
| `coll[i]`/`node[i]` loops written `i = 1; i <= n` | rewrite `i = 0; i < n` — or better, a range-for |
| `coll.GetElem(i)`, `coll.IsElementNull(i)`, `node.GetItem(i)`, `node.GetFieldByIndex(i)`, `node.GetFieldName(i)`, `node.SetItem(i)`/`InsertItem(i)`/`RemoveItem(i)`, `row.Get(i)`, `rs.GetColumn(i)`, `rs.Seek(SeekAbsolute, i)`, `ti.GetColumn(i)` — `i` 1-based | same methods — `i` now **0-based** (absolute seeks take the row index directly; `InsertItem` accepts `count` to append) |
| `stmt.GetBind(i)`, `stmt.BindToResultSet`, `BindInfo` array positions (`SetNullAtPos`, `SetDataSizeAtPos`, `GetDataSizeAtPos`, `IsNullAtPos`), `dp.SetColumn(i,…)`, `dp.SetEntry(row,col,…)` — 1-based | same — all **0-based** |
| `rs.GetColumnIndex(name)`, `ti.GetColumnIndex(name)`, `stmt.GetBindIndex(name)` — 1-based, `0` = not found | `Int32` result — **0-based** index, **`-1`** = not found |
| `rs.GetCurrentRow()` — 1-based, `0` = not fetched | `Int32` result — **0-based** row, **`-1`** if not fetched/empty |
| `ex.GetRow()` — 1-based array-DML row, `0` = N/A | `Int32` result — **0-based** row, **`-1`** if unrelated |

### 5.2 Additive — `rs.Rows()`, `rs.Columns()`, `ti.Columns()` and `Row`-based access

v4's `while (rs++)`/`rs.Next()`/`rs.Get<T>(i)` still compiles and works —
`Resultset` keeps its navigation methods and direct getters. v5 *adds* a
`Row` view + column views; the new idiom keeps data access on `row`:

```cpp
/* v4 idiom — index updated to 0-based */
while (rs++)
    ocout << rs.Get<String>(0) << oendl;

/* v5 idiom — row-based access */
for (auto&& row : rs.Rows())
    ocout << row.Get<String>(0) << oendl;

auto row = *rs.Rows().begin();            // first row
Row  r   = rs.Rows()[2];                  // 0-based absolute seek — scrollable
                                          // only; throws OCI-10002 forward-only
rows.Size();                              // rows fetched so far

auto cols = rs.Columns();                 // ResultsetColumns — descriptor view
cols.Size();                              // column count
Column c = cols[0];                       // 0-based — first column (== GetColumn(0))
Column n = cols["ENAME"];
cols.At(0); cols.Contains("ENAME"); cols.IndexOf("ENAME"); // 0-based / -1
for (auto&& col : cols) { /* ColumnIterator — random access, 0-based */ }

TypeInfo ti(con, "EMP", TypeInfo::Table);
for (auto&& col : ti.Columns()) { /* TypeInfoColumns — same surface */ }
```

| New member | Purpose |
|------------|---------|
| `rs.Rows()` → `ResultsetRows` | `begin()`/`end()` (`RowIterator`), `Size()`, `rows[i]` (0-based seek) |
| `rs.Columns()` → `ResultsetColumns` | `begin()`/`end()` (`ColumnIterator`), `Size()`, `[i]`/`["name"]` (0-based), `At`, `Contains`, `IndexOf` (−1 miss) |
| `ti.Columns()` → `TypeInfoColumns` | same view on type columns |
| `ti.GetColumnIndex(name)` | 0-based column index, -1 = not found |
| `Row` | proxy for the current row — `Get<T>(i)`/`Get<T>(name)`/`GetOptional<T>` |

#### The `Row` proxy

`Row` is a lightweight proxy bound to the resultset's *current* row — it owns
no data and is cheap to copy. It is produced by dereferencing a `RowIterator`
(`*it`) or by `rows[i]`:

```cpp
for (auto&& row : rs.Rows())
{
    String name       = row.Get<String>(0);            // 0-based column index
    int    salary     = row.Get<int>(OTEXT("SAL"));    // by column name
    auto   comm       = row.GetOptional<int>(OTEXT("COMM")); // std::optional —
                                                  // std::nullopt on SQL NULL
}
```

| `Row` member | Notes |
|--------------|-------|
| `Get<T>(UInt32 index)` | column value by 0-based index — same typed getters as `Resultset::Get<T>` |
| `Get<T>(const String& name)` | column value by name |
| `GetOptional<T>(index)` / `GetOptional<T>(name)` | `std::optional<T>`; `std::nullopt` on SQL NULL |

#### `ResultsetRows` / `RowIterator`

- `rs.Rows()` is a **view**, not a copy — iterating it fetches rows through the
  underlying `Resultset`, which must outlive the view.
- `begin()` performs the first `Next()`; `end()` is the past-the-end sentinel.
  `RowIterator` is an input iterator: `*it` → `Row`, `++` → `Next()`.
- Scrollable resultsets also get `--`, `+=`, `-=` on the iterator and
  `rows[i]` absolute seek (0-based). All seek operations throw `Exception`
  (OCI-10002) on forward-only resultsets.
- `rows.Size()` = number of rows fetched so far (`GetCurrentRow()+1`), i.e.
  the rows already consumed — not a total count for forward-only resultsets.

#### `ColumnView` / `ColumnIterator` — `rs.Columns()` and `ti.Columns()`

`rs.Columns()` (`ResultsetColumns`) and `ti.Columns()` (`TypeInfoColumns`)
share the same `ColumnView<T>` surface over `Column` descriptors:

- `Size()` — column count
- `cols[i]` / `cols.At(i)` — column by 0-based index (`At` bounds-checks)
- `cols["name"]` — column by name (throws if absent — check `Contains` first)
- `Contains(name)` → `bool`; `IndexOf(name)` → 0-based index or `-1`
- `begin()`/`end()` → `ColumnIterator` (random access, 0-based), yielding
  `Column` objects with the usual `GetName`/`GetType`/`GetSQLType`/size/precision/
  scale/nullable accessors

---

## 6. Step 5 — `Exception`: type, category and unified error code

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

---

## 7. Step 6 — Behavioural changes with unchanged signatures

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
5. **`Statement::GetBindIndex` no longer reports registered binds.** In v4, names
   created by `RegisterXxx()` leaked through the shared name map, returning an
   internal position that could not be used with `GetBind()`. In v5 it returns
   **`-1`** for register names — registers are out-params, not binds.
6. **Copy semantics are now `std::shared_ptr`-backed.** Handle wrappers still behave
   as reference-counted smart handles — semantics are preserved, but the old manual
   reference-list internals are gone. No source change expected.
7. **New opt-in logging** (process-wide, env-var driven): `OCILIB_LOG`,
   `OCILIB_LOG_CATEGORIES`, `OCILIB_LOG_OUTPUT` — set before constructing
   `Environment`. Useful for debugging a migration.

---

## 8. Step 7 — Notifications and AQ

### 8.1 Port is configured on the environment

```cpp
/* v4 — port passed at each registration */
sub.Register(con, name, changes, handler, port, timeout);

/* v5 — configure once on the environment */
env.SetSubscriptionPort(port);
unsigned int port = env.GetSubscriptionPort();   // read back / auto-assigned
```

### 8.2 `Subscription::Register` lost its `port` parameter, gained `mode`

```cpp
/* v4 */ void Register(const Connection&, const ostring& name, ChangeTypes,
                       NotifyHandlerProc, unsigned int port = 0, unsigned int timeout = 0);
/* v5 */ void Register(const Connection&, const String& name, ChangeTypes,
                       NotifyHandlerProc,
                       SubscriptionMode mode = Subscription::ModeDefault,
                       unsigned int timeout = 0);
```

`mode` is a `Subscription::SubscriptionMode` enum (`Subscription::ModeDefault` /
`Subscription::ModeSecureNotification`, i.e. `OCI_SRM_*`). Keep the default to
preserve v4 behavior; `ModeSecureNotification` requires Oracle client 12c+.

### 8.3 `Dequeue::Subscribe` is now a static batch call

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

## 9. Step 8 — `Bind` returns `BindInfo`; `BindInfo::SetDirection` added

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

---

## 10. Step 9 — Removed `ocilib::Mutex` / `ocilib::Thread` / `ocilib::ThreadKey`

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

## 11. Step 10 — XA removal

`Environment::SessionXA` / `OCI_SESSION_XA` is gone. Remove it from session flags; XA
is no longer supported through OCILIB.

---

## 12. Non-breaking details

- **`ocilib::Exception`** still derives from `std::exception` and is thrown on C API
  failures — `try`/`catch` structure is unchanged, but the error accessors were
  reworked (`GetType`/`GetCategory`/`GetCode` — see §6).
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
- **`ocilib::String` / `ocilib::StringView` added** — `ocilib::String` is the new
  canonical name for the string type (`ostring` remains as an alias);
  `StringView` is `std::basic_string_view<otext>`.
- **Internal header layout changed** (`detail/core/*`, `detail/support/*` reorganized,
  some files removed: `Mutex.hpp`, `Thread.hpp`, `ThreadKey.hpp`, `ManagedBuffer.hpp`,
  `Synchronizable.hpp`, `ConcurrentList.hpp`, `ConcurrentMap.hpp`,
  `SynchronizationGuard.hpp`, `HandleStoreResolver.hpp`). Only `#include "ocilib.hpp"`
  — never include `detail/` headers directly.

---

## 13. Migration checklist (agent-friendly order)

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
6.  [ ] Add `env` to Number/Date/Interval/Timestamp ctors and to
        Date::SysDate / Timestamp::SysTimestamp calls.
7.  [ ] Rebase all index code to 0-based (see §5): coll[i]/node[i] loops
        (i=1;i<=n → i=0;i<n), plus Get/GetItem/GetElem/GetFieldByIndex/
        GetFieldName/GetColumn/GetBind/Seek/SetItem/InsertItem/RemoveItem/
        SetColumn/SetEntry and BindInfo *AtPos calls; lookups now return
        Int32 with -1 = miss (GetColumnIndex/GetBindIndex/GetCurrentRow/
        ex.GetRow()); optionally move row access to rs.Rows()/row.Get.
8.  [ ] Audit silent behaviour changes (see §7): ChangeType::Row
        notification handlers, Statement::GetBindIndex on register names.
9.  [ ] Exception handling (see §6): GetType() comparisons against
        OracleError/OcilibError → GetCategory(); OracleWarning →
        GetType() == Exception::Warning; GetOracleErrorCode() /
        GetInternalErrorCode() → GetCode().
10. [ ] env.SetSubscriptionPort(port) before subscriptions; drop `port` arg from
        Subscription::Register; convert Dequeue::Subscribe to the static batch form.
11. [ ] Replace ocilib::Mutex/Thread/ThreadKey with <mutex>/<thread>/thread_local.
12. [ ] Remove Environment::SessionXA / XA usage.
13. [ ] Recompile with C++17+; fix remaining errors.
```

---

## 14. Worked example — before / after

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
