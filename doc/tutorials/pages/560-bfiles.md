---
title: BFILEs — OCI_File
navtitle: BFILEs
part: 5
level: intermediate
demos: [file.c]
sql: file.sql
---

# BFILEs — `OCI_File`

**Objectives** — by the end of this page you will be able to:

- reference server files through DIRECTORY objects
- open, check, read and close `OCI_File` handles
- bind BFILEs into columns

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
| `OCI_FileRead(f, buf, &len)` / `OCI_FileSeek` / `OCI_FileGetOffset` | read bytes |
| `OCI_FileAssign` / `OCI_FileIsEqual` | copy / compare locators |
| `OCI_BindFile` / `OCI_GetFile` | bind / fetch |

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
unsigned char buf[8192]; OCI_UInt32 n = sizeof(buf);
while (OCI_FileRead(f, buf, &n))
{
    fwrite(buf, 1, n, stdout);
    n = sizeof(buf);
}
OCI_FileClose(f);
```

```cpp
f.Open();
unsigned char buf[8192]; UInt32 n = sizeof(buf);
while (f.Read(buf, n))
{
    fwrite(buf, 1, n, stdout);
    n = sizeof(buf);
}
f.Close();
```
:::

`OCI_FileSeek(f, OCI_SEEK_SET/…, offset)` — random access for big files.

## Storing the pointer

:::sample
```c
OCI_Prepare(st, OTEXT("insert into documents(id, content) values(:i, :f)"));
OCI_BindFile(st, OTEXT(":f"), f, OCI_BFILE);      /* stores dir+name pointer */
OCI_Execute(st);
```

```cpp
st.Prepare("insert into documents(id, content) values(:i, :f)");
st.Bind(":f", f);                                 /* stores dir+name pointer */
st.ExecutePrepared();
```
:::

A fetched BFILE column gives a borrowed `File` (`OCI_GetFile` / `Get<File>`).

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
