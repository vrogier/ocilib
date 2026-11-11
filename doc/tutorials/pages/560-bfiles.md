---
title: BFILEs — OCI_File
navtitle: BFILEs
part: 2
level: intermediate
demos: [file.c]
sql: file.sql
---

# BFILEs — `OCI_File`

**Objectives** — by the end of this page you will be able to:

- reference server files through DIRECTORY objects
- open, check, read and close `OCI_File` handles
- bind BFILEs into columns

## In context — the database as a filesystem index

`BFILE` solves a different problem than LOB: the content never enters the
database. The column stores only a *(DIRECTORY object, filename)* pair — a
pointer to a file on the **server's** filesystem — and Oracle opens and reads
the real file each time you access it. Choose BFILE when the data must live as
files (media libraries, document stores shared with other tools, read-only
reference data loaded by `LOAD DATA` style pipelines); choose LOB when you want
transactional, backed-up, database-managed content.

The `DIRECTORY` object is also the security boundary: it maps a database-level
name to an OS path, and access is controlled by `GRANT READ/WRITE ON DIRECTORY`
— the client never sees the server's path. That is why every BFILE workflow
starts on the server side (`CREATE DIRECTORY` + `GRANT`) before any `OCI_File`
call can succeed.

On the OCILIB side `OCI_File` is a connection handle you fill with the
directory+name pair (`SetName`) — DIRECTORY names are stored upper-cased by
Oracle, so the literal you pass should be too. `Exists`/`GetSize`/`Read` then
round-trip to the server — and here the asymmetry bites: paths resolve on the
*database host*, not the client, so `OCI_FileExists` asks "is that file there
for you?" not "is it on my disk?". `OCI_LobCopyFromFile` closes the loop by
pulling BFILE content into a real LOB server-side — the migration path out of
external storage.

## Concepts

A `BFILE` column doesn't store bytes — it stores a **pointer to a file on the
server's filesystem** (`DIRECTORY` object + filename). `OCI_File` is the client-side
handle.

```sql
create directory media_dir as '/u01/media';
grant read on directory media_dir to scott;
```

| C function | C++ method | Purpose |
|------------|------------|---------|
| `OCI_FileCreate` | `File` ctor (`con`, `File::BFile`) | lifecycle |
| `OCI_FileFree` | — (RAII) | release |
| `OCI_FileArrayCreate` | — | bulk arrays |
| `OCI_FileSetName` | `SetInfos` / `SetName` | DIRECTORY + filename |
| `OCI_FileGetName` | `GetName()` | filename |
| `OCI_FileGetDirectory` | `GetDirectory()` | DIRECTORY |
| `OCI_FileExists` | `Exists()` | presence |
| `OCI_FileGetSize` | `GetLength()` | byte length |
| `OCI_FileOpen` / `OCI_FileClose` / `OCI_FileIsOpen` | I/O session |
| `OCI_FileRead(f, buf, len)` → n / `OCI_FileSeek` / `OCI_FileGetOffset` | read bytes |
| `OCI_FileAssign` / `OCI_FileIsEqual` | copy / compare locators |
| `OCI_StatementBindFile` / `OCI_ResultsetGetFile` | bind / fetch |

## Pointing at a file

:::sample
```c
OCI_File *f = OCI_FileCreate(cn, OCI_BFILE);
OCI_FileSetName(f, OTEXT("MEDIA_DIR"), OTEXT("report.pdf"));   /* DIRECTORY is upper-cased */

if (OCI_FileExists(f))
{
    printf("size: %u\n", OCI_FileGetSize(f));
}
```

```cpp
File f(con, File::BFile);
f.SetName("MEDIA_DIR", "report.pdf");          /* DIRECTORY is upper-cased */

if (f.Exists())
{
    std::cout << "size: " << f.GetLength() << "\n";
}
```
:::

## Reading

:::sample
```c
OCI_FileOpen(f);
unsigned char buf[8192];
OCI_UInt32 n;
while ((n = OCI_FileRead(f, buf, sizeof(buf))) > 0)   /* returns bytes */
{
    fwrite(buf, 1, n, stdout);
}
OCI_FileClose(f);
```

```cpp
f.Open();
for (;;)
{
    Raw chunk = f.Read(8192);            /* returns bytes actually read */
    if (chunk.empty()) break;
    fwrite(chunk.data(), 1, chunk.size(), stdout);
}
f.Close();
```
:::

`OCI_FileSeek(f, offset, OCI_SEEK_SET/…)` — random access for big files.

## Storing the pointer

:::sample
```c
OCI_StatementPrepare(st, OTEXT("insert into documents(id, content) values(:i, :f)"));
OCI_StatementBindFile(st, OTEXT(":f"), f);      /* stores dir+name pointer */
OCI_StatementExecutePrepared(st);
```

```cpp
st.Prepare("insert into documents(id, content) values(:i, :f)");
st.Bind(":f", f);                                 /* stores dir+name pointer */
st.ExecutePrepared();
```
:::

A fetched BFILE column gives a borrowed `File` (`OCI_ResultsetGetFile` / `Get<File>`).

## Scenario — fetch, inspect and read a column BFILE

:::sample
```c
OCI_StatementExecute(st, OTEXT("select content from documents where id = :i"));
OCI_ResultsetFetchNext(rs);

OCI_File *f = OCI_ResultsetGetFile(rs, 0);            /* borrowed      */
printf("%s/%s — %u bytes\n",
       OCI_FileGetDirectory(f), OCI_FileGetName(f), OCI_FileGetSize(f));

if (OCI_FileExists(f))
{
    OCI_FileOpen(f);
    unsigned char buf[4096];
    OCI_UInt32 n = OCI_FileRead(f, buf, sizeof(buf));   /* first chunk */
    OCI_FileClose(f);
}
```

```cpp
st.Execute("select content from documents where id = :i");
rs.Next();

File f = rs.Get<File>(0);                              /* borrowed      */
std::cout << f.GetDirectory() << '/' << f.GetName()
          << " — " << f.GetLength() << " bytes\n";

if (f.Exists())
{
    f.Open();
    Raw chunk = f.Read(4096);                          /* first chunk */
    f.Close();
}
```
:::

## Scenario — import a BFILE into a LOB column

"External file → real BLOB" is the classic migration: point the `File`, then copy
server-side via `OCI_LobCopyFromFile` — no client round-trip for the bytes:

:::sample
```c
OCI_File *f = OCI_FileCreate(cn, OCI_BFILE);
OCI_FileSetName(f, OTEXT("MEDIA_DIR"), OTEXT("report.pdf"));

OCI_Lob *blob = OCI_LobCreate(cn, OCI_BLOB);
if (OCI_FileExists(f))
{
    OCI_FileOpen(f);
    OCI_LobCopyFromFile(blob, f, 0, 0,
                        (OCI_UInt64) OCI_FileGetSize(f));
    OCI_FileClose(f);
}
```

```cpp
File f(con, File::BFile);
f.SetName("MEDIA_DIR", "report.pdf");

Lob blob(con, Lob::Blob);
if (f.Exists())
{
    f.Open();
    /* no C++ CopyFromFile — call the C API on the raw handles */
    OCI_LobCopyFromFile(blob, f, 0, 0, (OCI_UInt64) f.GetLength());
    f.Close();
}
```
:::

## Scenario — random access on a big file

`Seek` + `GetOffset` give you random access without reading the whole file:

:::sample
```c
OCI_FileOpen(f);
OCI_FileSeek(f, (OCI_UInt64) 1024*1024, OCI_SEEK_SET);   /* 1 MiB mark  */
unsigned char buf[1024];
OCI_UInt32 n = OCI_FileRead(f, buf, sizeof(buf));        /* 1 KiB there */
OCI_FileClose(f);
```

```cpp
f.Open();
f.Seek(SeekSet, 1024*1024);                              /* 1 MiB mark  */
Raw chunk = f.Read(1024);                                /* 1 KiB there */
f.Close();
```
:::

## Putting it together — the typical sequence

BFILE work always starts on the server — the client cannot fix a missing
DIRECTORY or grant:

1. **Server side first**: `CREATE DIRECTORY x AS '/path'` + `GRANT READ` — both
   require DBA rights; without them every `OCI_File` call fails.
2. **Point at the file**: `FileCreate` + `SetName` with the DIRECTORY name
   *upper-cased* — Oracle stores it that way, and lowercase names silently
   fail `Exists`.
3. **Probe before opening**: `Exists` → `GetSize` — remember both are server
   round-trips asking "is that file there *for you*?", on the database host.
4. **Read**: `Open`, `Read`/`Seek` in the usual chunked loop, `Close`. A
   fetched BFILE column gives the same object, borrowed.
5. **Store or import**: bind the locator into a BFILE column to keep it as an
   external pointer, or `LobCopyFromFile` to pull the content into a real LOB
   server-side — the standard migration path.

Hints: never `Free` a fetched `File`; `GetDirectory`/`GetName` read back what
the locator points at — handy when logging which file a row uses; and a BFILE
read is a filesystem read on the server, not a network file transfer.

## Try it yourself

1. Create a DIRECTORY + grant, point an `OCI_File` at a real file, verify `Exists`.
2. Read a binary file and hash it; bind it into a BFILE column; fetch and re-hash.
3. `Seek` halfway through a large file and dump 1 KB.

## Common pitfalls

- **File exists on client, not server** — BFILE paths resolve on the *database* host.
- **Missing DIRECTORY grant** — `ORA-22285`/permission errors.
- **Lowercase directory name** — Oracle stores DIRECTORY names upper-case.

## Recap

- `OCI_File` = DIRECTORY+name pointer; `SetName` → `Exists` → `Open` → `Read`/`Seek`
  → `Close`; bind into BFILE columns.

**Next:** `SYS.XMLTYPE` values.
