---
title: Checkpoint 7 — the bulk loader
navtitle: Checkpoint 7 bulk loader
part: 8
level: advanced
---

# Checkpoint 7 — the bulk loader

You should now be able to batch DML with array binds, ingest at max speed with
direct path, and administer the instance.

## The exercise — `bulkload`

A CSV importer for `employees` with two engines:

1. `--mode=array` — parse CSV into `OCI_Int32`/otext/`OCI_Double` arrays,
   `OCI_StatementBind*Array`, `BindArraySetSize`, one `Execute`, one `Commit`.
2. `--mode=dirpath` — `TypeInfoGet(employees)` → `DirPathCreate` → `SetColumn` per
   column → `Prepare` → `SetEntry` loop → `Convert`/`Load` → `Finish`.

Add flags: `--rows N`, `--no-log` (DirPathSetNoLog), `--onerror continue`
(report `GetErrorRow`/`GetErrorColumn` and keep going).

## Measure & compare

- Rows/sec for 10k/100k/1M rows in both modes.
- Insert a duplicate `empno` — how does each mode surface the failing row?
- Time `Commit` behavior differences (array = per-execute, dirpath = at `Finish`).

## Acceptance checks

- Row counts match CSV line count in both modes.
- Bad rows are reported with correct index.
- `GetAffectedRows`/`GetRowCount` agree with the file.

## What it exercises

| Skill | Pages |
|-------|-------|
| Array binds | 810 |
| DirectPath cycle + errors | 815 |
| Error handling | 080 |
| Prepared executes | 260 |

Part 8 done. Final stretch: Part 9 — sharding, SODA, capstone.
