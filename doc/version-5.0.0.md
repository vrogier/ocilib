# OCILIB v5.0.0 — Change Summary

This document is a high-level synthesis of the v5.0.0 development work. It is based on the `*-PLAN.md` files at the repository root and a `git diff` against `master` (excluding the plan documents themselves and the unit-test tree).

---

## 1. Scope at a glance

| Area | Files touched | Insertions | Deletions |
|------|---------------|-----------:|----------:|
| Library + demos + build (excl. tests/plans) | ~208 | ~7,100 | ~9,300 |
| Unit tests | 60 | ~8,150 | ~1,450 |

The release touches the C API, the C++ API, the internal handle/symbol architecture, the build system, the demos, and the test suite.

---

## 2. Major highlights

- **`OCI_Environment` becomes a public, first-class handle.** `OCI_Initialize()` / `OCI_Cleanup()` are replaced by `OCI_EnvironmentCreate()` / `OCI_EnvironmentFree()`. Most top-level constructors now take an explicit `OCI_Environment*` parameter.
- **C11 for the C layer and C++17 for the C++ layer.** The C implementation now uses ISO C11 `<threads.h>` for internal synchronization. The C++ header-only API now requires C++17 and uses `std::shared_ptr`, `std::mutex`, `std::unordered_map`, and `std::function` internally.
- **Per-environment OCI symbol table.** All OCI function pointers are loaded into an `OCI_Symbols` table attached to each `OCI_Environment`, so every OCI call is dispatched through `env->api-><func>`.
- **Unified handle polymorphism.** All internal handle structures now share a common base layout (`OCI_HANDLE` / `OCI_DATATYPE` macros in `src/types.h`). A new public helper, `OCI_HandleGetEnvironment()`, can retrieve the owning environment from any handle.
- **Optional runtime logging.** A new logging subsystem in `src/log.c` is controlled by environment variables (`OCILIB_LOG`, `OCILIB_LOG_CATEGORIES`, `OCILIB_LOG_OUTPUT`) and instruments public API entry/exit, OCI calls, and error reporting.
- **Removal of legacy public APIs.** XA/distributed transactions, the public `OCI_Mutex` / `OCI_Thread` / `OCI_ThreadKey` C APIs, and the matching C++ wrappers are gone. Internal synchronization has been replaced with native C11 primitives.
- **Bug fixes in notification and AQ dequeue registration.** Row-level change notifications now work correctly, and AQ dequeue registration now registers all dequeues in a single call.
- **Large test-suite expansion.** The gtest suite grew from **246 to 420 test cases** (`TEST`/`TEST_P`), with many new test modules for previously uncovered areas.

---

## 3. Public `OCI_Environment`

The biggest user-visible change is the end of the global, static environment.

- New lifecycle:
  - `OCI_Environment* OCI_EnvironmentCreate(POCI_ERROR err_handler, const otext* lib_path, unsigned int mode)`
  - `OCI_Boolean OCI_EnvironmentFree(OCI_Environment* env)`
- `OCI_Initialize()` / `OCI_Cleanup()` have been removed.
- All top-level constructors that previously relied on an implicit global environment now take an explicit `OCI_Environment*`: `OCI_ConnectionCreate`, `OCI_PoolCreate`, `OCI_NumberCreate`, `OCI_DateCreate`, `OCI_TimestampCreate`, etc.
- `OCI_GetLastError()` no longer takes a parameter and returns the per-thread `OCI_Error` stored in C11 `tss_t`, in all modes. The `OCI_ENV_CONTEXT` flag has been removed: thread-local error storage is always enabled.
- New `OCI_HasError()` returns TRUE when an error (or enabled warning) was recorded on the calling thread.
- A new public function `OCI_HandleGetEnvironment()` can recover the owning `OCI_Environment*` from any OCILIB handle.

---

## 4. C11 / C++17 baseline

### C layer
- The C implementation now targets **ISO C11**.
- Internal synchronization uses `<threads.h>` (`mtx_t`, `tss_t`, `once_flag`, `call_once`):
  - `OCI_List` and `OCI_Environment` embed `mtx_t` directly.
  - Per-thread `OCI_Error` storage uses `tss_t`.
- The old `src/mutex.c`, `src/thread.c`, and `src/threadkey.c` implementation files have been deleted.

### C++ layer
- The header-only C++ API now requires **C++17 or later** (`#error` in `include/ocilibcpp/config.hpp`).
- Old C++03 shims (`core::EnableIf`, `core::IsSame`, manual `SmartHandle` reference lists, `CallbackPointer`, `ManagedBuffer`, `Synchronizable`, `ConcurrentList`, `ConcurrentMap`) have been replaced by standard C++17 facilities:
  - `std::enable_if` / `std::is_same`
  - `std::shared_ptr` / `std::weak_ptr` handle control blocks
  - `std::mutex` + `std::lock_guard`
  - `std::unordered_map` for the handle store and callback maps
  - `std::function` for user callbacks
- C++ wrappers `ocilib::Mutex`, `ocilib::Thread`, and `ocilib::ThreadKey` have been removed.

---

## 5. Logging support

A new, opt-in logging subsystem is introduced in `src/log.c` / `src/log.h`.

- **Activation via environment variables** (read during `OCI_EnvironmentCreate`):
  - `OCILIB_LOG` — level: `NONE` (default), `ERROR`, `WARN`, `INFO`, `TRACE`, `DEBUG`
  - `OCILIB_LOG_CATEGORIES` — `LIB`, `OCI`, or `ALL` (default)
  - `OCILIB_LOG_OUTPUT` — `STDERR` (default), `STDOUT`, or a file path
- A single global `OCI_Logger` instance is initialized before any OCI call; it is reference-counted across environments.
- The log sink uses a platform-native lock (`CRITICAL_SECTION` on Windows, `pthread_mutex_t` elsewhere) so it can safely trace the OCI calls that create OCI mutexes.
- `CHECK_OCI` automatically emits `[TRACE] [OCI]` lines for every OCI call.
- `ENTER_FUNC` / `EXIT_FUNC` macros emit `[TRACE] [LIB]` entry/exit lines for internal implementations.
- `OcilibExceptionCallHandler()` emits `[ERROR]`/`[WARN]` lines for Oracle and OCILIB errors.
- Log files are plain text; `otext` messages are converted to UTF-8 before writing.
- The public programmatic control API (`OCI_GetLogLevel` / `OCI_SetLogLevel` / etc.) is documented as a future Phase-6 addition; the initial release is environment-variable driven.

---

## 6. Removed APIs and features

### XA / distributed transactions
- The `OCI_SESSION_XA` session mode and all XA/distributed-transaction documentation have been removed from the public API.

### Public C threading / mutex / TLS APIs
- Removed types: `OCI_Mutex`, `OCI_Thread`, `OCI_ThreadKey`
- Removed callbacks: `POCI_THREAD`, `POCI_THREADKEYDEST`
- Removed functions:
  - `OCI_MutexCreate/Free/Acquire/Release`
  - `OCI_ThreadCreate/Free/Run/Join`
  - `OCI_ThreadKeyCreate/SetValue/GetValue`
- Removed C++ classes: `ocilib::Mutex`, `ocilib::Thread`, `ocilib::ThreadKey`
- Removed test files: `TestThread.cpp`, `TestThreadKey.cpp`

### OCI environment statistics
- The `mem_bytes_oci` counter and the `AllocatedBytesFlags` enumeration have been removed.
- `OCI_GetAllocatedBytes()` and `Environment::GetAllocatedBytes()` have been removed.
- Use the new generic diagnostic API:
  - C: `OCI_UInt64 OCI_GetStats(OCI_Environment* env, OCI_Stat stat);`
  - C++: `OCI_UInt64 Environment::GetStats(OCI_Stat stat) const;`
- Supported `OCI_Stat` / `stat` values:
  - `OCI_UST_BYTES` — bytes allocated internally by OCILIB.
  - `OCI_UST_HANDLES` — OCI handles allocated for the environment.
  - `OCI_UST_DESCRIPTORS` — OCI descriptors allocated for the environment.
  - `OCI_UST_OBJECTS` — OCILIB object instances allocated for the environment.
- The OCI client runtime byte counter is no longer exposed.

### C++ internal statics
- The global/static `HandleStore` default instance is gone; each `ocilib::Environment` owns its own `HandleStore`.
- `Environment` is no longer a process-wide singleton in the C++ layer; users create `ocilib::Environment` instances explicitly.

---

## 7. Notable bug fixes

### Change-notification rows (`OCI_CNT_ROWS`)
In `src/callback.c`, the `OcilibCallbackNotifyChanges` callback now treats `OCI_EVENT_OBJCHANGE` for both `OCI_CNT_OBJECTS` and `OCI_CNT_ROWS`. Previously row-level notifications were ignored, so subscribers requesting `OCI_CNT_ROWS` did not receive per-row events and rowids.

### AQ dequeue registration
`OCI_DequeueSubscribe()` has been restructured:
- Old signature took a single `OCI_Dequeue*` and a `port` argument.
- New signature takes a connection, an array of `OCI_Dequeue*`, a count, a timeout, and a callback: `OCI_DequeueSubscribe(OCI_Connection*, OCI_Dequeue**, unsigned int, unsigned int, POCI_NOTIFY_AQ)`.
- The notification port is now configured once per environment with `OCI_SetSubscriptionPort()`.
- All dequeues are registered with a single `OCISubscriptionRegister()` call, eliminating the `ORA-24915` caused by repeated listener starts.

### Other fixes
- Per-environment symbol loading fixes the runtime-import path when multiple environments or different library paths are used.
- Handle environment retrieval is now uniform through `OCI_HandleGetEnvironment()`, removing the need for the old `OcilibEnvironmentGetFromObject()` switch.
- Unicode / `OCI_CHARSET_WIDE` paths in notification unregistration are preserved.

---

## 8. Breaking changes for users

The following items require code changes when upgrading from v4.x to v5.0.0:

1. **Initialization / cleanup**
   - Replace `OCI_Initialize(...)` / `OCI_Cleanup()` with `OCI_EnvironmentCreate(...)` / `OCI_EnvironmentFree(env)`.

2. **Environment parameter**
   - Pass `OCI_Environment*` to constructors such as `OCI_ConnectionCreate`, `OCI_PoolCreate`, `OCI_DateCreate`, `OCI_NumberCreate`, `OCI_TimestampCreate`, etc.

3. **Threading / mutex / TLS**
   - Replace `OCI_Mutex*`, `OCI_Thread*`, and `OCI_ThreadKey*` usage with C11 `<threads.h>`, C++ `<thread>`/`<mutex>`, or platform APIs.

4. **XA**
   - Remove any `OCI_SESSION_XA` usage; XA connectivity is no longer supported.

5. **Notifications / AQ**
   - Call `OCI_SetSubscriptionPort(env, port)` before registering subscriptions or dequeue listeners.
   - Update `OCI_DequeueSubscribe()` calls to the new connection + array signature.

6. **Memory / resource statistics**
   - Replace `OCI_GetAllocatedBytes()` calls with `OCI_GetStats(env, OCI_UST_BYTES)`.
   - The separate OCI-client byte counter is no longer available.
   - Other counters are available through `OCI_UST_HANDLES`, `OCI_UST_DESCRIPTORS`, and `OCI_UST_OBJECTS`.

7. **C++ API**
   - A C++17 compiler is now required.
   - `ocilib::Mutex`, `ocilib::Thread`, and `ocilib::ThreadKey` are removed.
   - `ocilib::Environment` is constructed explicitly and is not a singleton.
   - Replace `Environment::GetAllocatedBytes()` with `Environment::GetStats(OCI_UST_BYTES)`.

8. **Error retrieval**
   - `OCI_GetLastError()` now requires an `OCI_Environment*` argument.

9. **Boolean type**
   - The public boolean type is now `OCI_Boolean` (`typedef int OCI_Boolean`), replacing the `#define boolean int` macro that polluted every consumer translation unit.
   - `boolean` remains available as a compatibility typedef (`typedef OCI_Boolean boolean`) so existing code still compiles; new code should use `OCI_Boolean`.

10. **Fixed-width integer types**
   - New public typedefs: `OCI_Int16`, `OCI_UInt16`, `OCI_Int32`, `OCI_UInt32`, `OCI_Int64`, `OCI_UInt64`, `OCI_Float`, `OCI_Double`, plus `OCI_Size` for byte sizes and buffer lengths.
   - The C++ API additionally exposes short aliases in `namespace ocilib`: `Int16`, `UInt16`, `Int32`, `UInt32`, `Int64`, `UInt64`, `Float`, `Double`, `Size` (same types as the `OCI_` names).
   - The C API (headers, `src/`, demos, tests) uses these types in place of bare `int`, `unsigned int`, `short`, `unsigned short` and `size_t`.
   - `OCI_Int64`/`OCI_UInt64` are `long long`/`unsigned long long` — identical to the former `big_int`/`big_uint` on every platform — and now always exist (the 32-bit fallback branch is gone).
   - `big_int`/`big_uint` remain available as compatibility typedefs; new code should use `OCI_Int64`/`OCI_UInt64`.

---

## 9. Unit tests

The gtest-based suite was heavily expanded and updated for the v5.0 APIs.

- **Test case count:** from **246** in `master` to **420** in v5.0.0 (`TEST` + `TEST_P` macros).
- **New test modules** were added for previously untested or lightly tested areas such as Agent, Bind, Callback, Column, Dequeue, Element, Enqueue, Error, Event, Exception, File, Format, Handle, Hash, Helpers, Iterator, List, Log, Memory, Message, ReportedIssues, Resultset, Statement, Stringutils, Transaction, and TypeInfo.
- **Removed test modules:** `TestThread.cpp` and `TestThreadKey.cpp` (APIs removed).
- `TestConnection`, `TestPool`, `TestQueue`, `TestSubscription`, `TestNotifications`, and other existing modules were updated to use `OCI_EnvironmentCreate`, `std::thread`, the new subscription port API, and the new dequeue registration signature.
- The Visual Studio test project (`tests/ocilib_tests.vcxproj`) and filters were updated to reflect the new file set.

---

## 10. Build system notes

- `proj/dll/ocilib_dll.vcxproj` now builds the C sources with `stdc11`.
- `src/Makefile.am` removes `mutex.c/thread.c/threadkey.c` and adds `log.c`.
- `tests/ocilib_tests.vcxproj` removes the thread/thread-key test files and adds the new test modules.
- Demos were updated to call `OCI_EnvironmentCreate`; samples that previously used `OCI_Thread`/`OCI_Mutex` still need to be ported to standard threading primitives.

---

## 11. Source plans consulted

This summary was assembled from the following design documents at the repository root:

- `C11-PLAN.md`
- `CPP-MIGRATION-PLAN.md`
- `LOGGING-PLAN.md`
- `OCI-SYMBOL-PLAN.md`
- `POLYMORPHISM-PLAN.md`
- `SUB-PORT-PLAN.md`
