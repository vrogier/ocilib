---
title: Checkpoint 4 — employee profiles
navtitle: Checkpoint 4 profiles
part: 5
level: intermediate
---

# Checkpoint 4 — employee profiles

You should now be able to work with every scalar and large Oracle data type:
strings/charsets, dates, timestamps, intervals, exact numbers, RAW, vectors, LOBs,
BFILEs, XMLType and JSON.

## The exercise — profile service

Extend the employee schema (`tutorial-schema.sql` plus):

```sql
alter table employees add (profile json, photo blob, resume bfile);
create table dept_docs (deptno number, doc xmltype, note varchar2(4000));
create directory docs_dir as '/tmp/docs';
```

Write a program that:

1. **JSON profile** — for each employee, build a mutable `OCI_Json` with
   `{"name":…,"hiredate":…,"skills":[…]}`, `LoadText`/node ops to fill it, then
   `OCI_BindJson` into `profile`.
2. **Photo** — stream a small PNG into `photo` via `OCI_LobCreate`/`LobWrite` in
   32 KB chunks; read it back with `LobRead` and compare checksums.
3. **Resume BFILE** — `OCI_FileSetName(DOCS_DIR, 'smith.txt')`, verify
   `FileExists`, `FileOpen`+`FileRead` it, bind into `resume`.
4. **XML doc** — `xmltype(:x)` insert into `dept_docs` via string bind, fetch with
   `OCI_GetXmlType` and print `GetContent`.
5. **Tenure report** — `OCI_DateDaysBetween(sysdate, hiredate)` per employee;
   format hiredate with `OCI_DateToText`.

## Stretch goals

- Validate `profile` against a JSON schema (`OCI_JsonValidate`, 23ai).
- Store a 3-dim "skills vector" per employee (`OCI_Vector`) and rank similarity.
- Extract `profile.skills` per row via `OCI_JsonNodeGetByPath` and index them in an
  `OCI_HashTable` name→empno map (bridge to page 825).

## What it exercises

| Skill | Pages |
|-------|-------|
| Charset-safe strings & formats | 510 |
| Dates / timestamps / intervals | 515–525 |
| Exact numbers, RAW | 530/535 |
| Vectors (23ai) | 540 |
| LOB chunked I/O | 550/555 |
| BFILE | 560 |
| XMLType | 565 |
| JSON doc + DOM | 570/575 |

Part 5 done — you can now move any Oracle value in and out. Next: the
object-relational API (types, objects, collections, REFs).
