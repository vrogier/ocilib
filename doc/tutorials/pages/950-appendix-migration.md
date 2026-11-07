---
title: Appendix — coming from OCILIB 4.x
navtitle: From OCILIB 4.x
part: 10
---

# Appendix — coming from OCILIB 4.x

These tutorials target **v5**. If you know OCILIB 4.x, the map is short:

| v4 | v5 | Where it matters |
|----|----|------------------|
| `OCI_Initialize(err, lib, mode)` | `OCI_EnvironmentCreate(err, lib, mode)` → `OCI_Environment*` | pages 060/150 |
| `OCI_Cleanup()` | `OCI_EnvironmentFree(env)` | everywhere |
| global error handler/state | per-`OCI_Environment` state + `OCI_HandleGetEnvironment` | 080/150 |
| `OCI_ErrorGetType` (err vs warning) + `GetOCICode`/`GetInternalCode` | `GetType` (severity), `GetCategory` (`LIB`/`OCI`/`JZN`), unified `GetCode` | 080/915 |
| XA transactions, public `OCI_Mutex`/`OCI_Thread`/`OCI_ThreadKey` | **removed** — C11 threads inside; `OCI_Transaction` covers local/global tx | 415/430 |
| `ocilib::ostring` | `ocilib::String` (`ostring` kept as alias) | 510 |
| JSON/SODA/sharding/vectors | new in v5 | 540/570/850–860 |

**Full guides:** `doc/migration-guide-c-api.md` and `doc/migration-guide-cpp-api.md`
in the repo — read those alongside these tutorials if you're porting.

**Version notes:** `doc/version-5.0.0.md` has the complete change summary —
environment handle, error model split, JSON DOM, SODA, sharding, iterators,
C++17 modernization.
