---
title: Appendix — FAQ & troubleshooting
navtitle: FAQ
part: 10
---

# Appendix — FAQ & troubleshooting

## Startup failures

| Symptom | Cause | Fix |
|---------|-------|-----|
| `OCI_EnvironmentCreate` NULL/throws | `oci.dll`/`libclntsh.so` not found | Instant Client on `PATH`/`LD_LIBRARY_PATH` |
| Loader error before `main` | 32/64-bit mixup | match exe ↔ Instant Client ↔ `ocilib*.dll` bitness |
| `configure: OCI headers not found` | `linkage` mode without SDK | use `--with-oracle-import=runtime` |

## Connection errors

| Code | Meaning | Fix |
|------|---------|-----|
| `ORA-12154` | connect identifier unresolved | use `host:port/service` Easy Connect |
| `ORA-12514` | service unknown | `XEPDB1` vs `XE` on XE; check `lsnrctl status` |
| `ORA-12541` | no listener | DB/container not started |
| `ORA-01017` | bad credentials | re-export `OCILIB_DEMO_*` |

## Runtime errors

| Symptom | Likely issue |
|---------|--------------|
| `ORA-00942` on samples | `tutorial-schema.sql` not run |
| `ORA-06502` PL/SQL buffer | bound buffer too small / wrong `OCI_StatementBindString` size |
| Fetch returns nothing | forgot `OCI_StatementExecutePrepared` after `OCI_StatementPrepare`, or wrong fetch order |
| NULL crashes on get | check `OCI_ResultsetIsColumnNull` / use `GetOptional<T>` |
| `ORA-01013` appears | expected — something called `OCI_ConnectionBreak` |
| "It worked in SQL*Plus but not here" | autocommit off; uncommitted DML invisible elsewhere |

## Link/build errors

| Symptom | Cause |
|---------|-------|
| `LNK2019 OCI_*` | wrong import lib: `ocilib.lib` (MBCS) vs `ocilibw.lib` (wide) |
| `undefined reference to OCI_*` | missing `-locilib` / wrong lib dir |
| `ocilib.h not found` | include path — repo `include/` or install `include/` |
| Garbled strings | MBCS app ↔ wide lib mismatch — rebuild matching |

## Platform notes

- **Windows**: keep `ocilib*.dll` next to the exe or on `PATH`; watch CRT mixing
  (debug exe + release dll is fine, mismatched toolsets less so).
- **Linux**: `sudo ldconfig` after `make install`; `LD_LIBRARY_PATH` must carry
  *both* `libocilib.so` and `libclntsh.so`.
- **WSL**: the WSL checkout builds independently — don't mix Windows-built DLLs
  with WSL `.so`s.

Still stuck? Turn on `OCILIB_LOG=DEBUG` (page 085) and read which call fails.
