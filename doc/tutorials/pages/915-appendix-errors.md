---
title: Appendix — error model reference
navtitle: Error model
part: 10
---

# Appendix — error model reference

## The `OCI_Error` shape (v5)

| Getter | Type | Values |
|--------|------|--------|
| `OCI_ErrorGetType` | `OCI_ErrorType` | `OCI_ERR_TYPE_ERROR` (1) / `OCI_ERR_TYPE_WARNING` (2) |
| `OCI_ErrorGetCategory` | `OCI_ErrorCategory` | `OCI_ERR_CAT_LIB` (1) / `OCI_ERR_CAT_OCI` (2) / `OCI_ERR_CAT_JZN` (3) |
| `OCI_ErrorGetCode` | `OCI_ErrorCode` | Oracle `ORA-xxxxx` number or OCILIB internal code |
| `OCI_ErrorGetString` | `const otext*` | formatted message |
| `OCI_ErrorGetConnection` / `GetStatement` | handles | source objects (may be NULL) |
| `OCI_ErrorGetRow` | `OCI_UInt32` | batch/array row index when relevant |
| `OCI_ErrorGetLocation` | `const char*` | OCILIB function that raised it |

C++ `ocilib::Exception` mirrors: `GetType()` (`ExceptionType`),
`GetCategory()` (`ExceptionCategory`), `GetCode()`, `GetMessage()`/`what()`,
`GetConnection()`, `GetStatement()`, `GetRow()`.

## Categories decoded

| Category | Origin | Typical examples |
|----------|--------|------------------|
| `OCI_ERR_CAT_LIB` | OCILIB itself | NULL handle, wrong arg, feature not available in this client version, wrong-object-context |
| `OCI_ERR_CAT_OCI` | Oracle client/server | `ORA-00942`, `ORA-01017`, `ORA-00001`, timeouts |
| `OCI_ERR_CAT_JZN` | JSON DOM engine (`JZN-xxxxx`) | malformed JSON, schema-validation errors |

## Delivery paths

1. **Error handler** — your `OCI_EnvironmentCreate`/`OCI_SetErrorHandler` callback;
   fires inside the failing call.
2. **`OCI_GetLastError()`** — thread-local poll anytime after a FALSE/NULL return.
3. **C++ exceptions** — every failed call throws `ocilib::Exception`.

`OCI_EnableWarnings(env, TRUE)` routes `WARNING`-type Oracle notices through the
same channels.

## Return-value conventions

- Most calls: `OCI_Boolean` (`TRUE`/`FALSE`).
- Factories/getters returning handles/values: `NULL`/`0` on failure — check the
  handler/`GetLastError` for details.
- `OCI_Ping` semantics: `TRUE` = alive (not "success" in the error sense).

## Common Oracle codes worth knowing

`ORA-00942` table missing · `ORA-01017` bad credentials · `ORA-00001` unique
constraint · `ORA-01013` operation cancelled (`OCI_Break`) · `ORA-12154` connect
identifier · `ORA-12541`/`12514` listener/service · `ORA-06550`/`06502` PL/SQL
compile/runtime errors.
