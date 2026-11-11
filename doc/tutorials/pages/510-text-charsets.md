---
title: Strings, charsets & format helpers
navtitle: Strings & charsets
part: 2
level: beginner
demos: [format.c]
---

# Strings, charsets & format helpers

**Objectives** — by the end of this page you will be able to:

- write source code that compiles on MBCS and wide builds unchanged
- convert dates/numbers to text with `OCI_EnvironmentSetFormat`-style helpers
- use `ocilib::String`/`StringView` correctly in C++

## In context — two charsets, two string types

Oracle keeps two character sets per database: the **database charset** (used by
`CHAR`/`VARCHAR2`/`CLOB`) and the **national charset** (used by
`NCHAR`/`NVARCHAR2`/`NCLOB`, always a Unicode variant). Columns carry a *charset
form* flag saying which world they live in — that is the `OCI_CSF_*` value you
can read off a column or force on a bind.

On the client side OCILIB compiles in two flavors — MBCS (`ocilibm`, `otext` =
`char`) and wide (`ocilibw`, `otext` = wide) — and converts to/from the
server's encoding for you. The `otext`/`OTEXT()` abstraction is what lets the
same source build against both: your literal adapts to whichever `otext` is.
`OCI_EnvironmentGetCharset` reports which mode your build is running, and
`OCI_EnvironmentSetFormat` shapes how the *string getters* render non-text
columns (dates, timestamps, numbers) so `GetString` output can follow your
locale rules instead of the session defaults.

That is also where the classic bugs come from: `sizeof` on wide builds counts
*bytes*, not characters; `strlen` does not exist for `otext`; and passing a raw
`char*` literal where `otext*` is expected compiles silently on MBCS and breaks
on wide builds. The sections below exist to make those mistakes impossible.

## The `otext` contract

OCILIB ships two string personalities — the full story of what each build
does to OCI is on the [charset builds page](charsets.html):

| Build | `otext` | `OTEXT("x")` | `ocilib::String` |
|-------|---------|--------------|------------------|
| MBCS (`OCI_CHARSET_MBCS`) | `char` | `"x"` | `std::string` |
| Wide (`OCI_CHARSET_WIDE`) | `wchar_t` | `L"x"` | `std::wstring` |

Rules for portable code:

- API parameters and returns are `otext*`/`const otext*` — never `char*` in headers.
- Wrap **every** literal in `OTEXT(…)`.
- `sizeof(buf)` is bytes; `sizeof(buf)/sizeof(otext)` is characters — API length
  args are character counts.
- The environment reports its *runtime* charset flavor (page 140).

:::sample
```c
otext name[64];
OCI_ConnectionImmediate(cn, OTEXT("select ename from employees where empno = 7839"),
              OCI_ARG_TEXT, name);
```

```cpp
String name;
st.Execute("select ename from employees where empno = 7839");
if (st.GetResultset().Next())
{
    name = st.GetResultset().Get<String>(0);   /* String = otext-typed */
}
```
:::

## Conversion formats — your to-string rules

When the library converts a DATE or NUMBER to text, it uses per-type formats you can
override on the connection:

| C enum | C++ enum | Controls |
|--------|----------|----------|
| `OCI_FMT_DATE` | `FormatType::FormatDate` | date → text format |
| `OCI_FMT_TIMESTAMP` | `FormatType::FormatTimestamp` | timestamp → text |
| `OCI_FMT_NUMERIC` | `FormatType::FormatNumeric` | number → text |
| `OCI_FMT_BINARY_DOUBLE` | `FormatType::FormatBinaryDouble` | binary_double → text |
| `OCI_FMT_BINARY_FLOAT` | `FormatType::FormatBinaryFloat` | binary_float → text |

:::sample
```c
OCI_EnvironmentSetFormat(env, OCI_FMT_DATE, OTEXT("YYYY-MM-DD"));
/* now string getters on a DATE column return ISO-style text */
```

```cpp
env.SetFormat(FormatType::FormatDate, "YYYY-MM-DD");
```
:::

## In C++ — `String` and `StringView`

- `ocilib::String` = `std::basic_string<otext>` — what every C++ API returns.
- `ocilib::StringView` = `std::basic_string_view<otext>` — zero-copy view where the
  API offers it.
- Plain `const char*` literals work with C++ APIs **on MBCS builds**; write
  `OTEXT(…)`/`String` when you want portability.

## Scenario — charset-form columns (NCHAR/NVARCHAR2)

`NCHAR`/`NVARCHAR2`/`NCLOB` columns carry a **charset form** flag. The getter
still returns `otext*` — OCILIB converts — but binds let you force the form:

:::sample
```c
OCI_StatementPrepare(st,
    OTEXT("insert into labels(id, utf16_name) values(:i, :n)"));
OCI_Bind *b = OCI_StatementBindString(st, OTEXT(":n"), (otext*) name,
                                    (OCI_UInt32) 0);
OCI_BindSetCharsetForm(b, OCI_CSF_NATIONAL);      /* force NCHAR semantics */
```

```cpp
st.Prepare("insert into labels(id, utf16_name) values(:i, :n)");
st.Bind(":n", name);    /* charset form is inferred from the column   */
```
:::

Check what form a fetched column uses:

:::sample
```c
OCI_Column *col = OCI_ResultsetGetColumn(rs, 0);
printf("charset form: %s\n",
       OCI_ColumnGetCharsetForm(col) == OCI_CSF_NATIONAL
           ? "national" : "database");
```

```cpp
Column col = rs.GetColumn(0);
if (col.GetCharsetForm() == CharsetFormNational)
    std::cout << "national charset column\n";
```
:::

## Scenario — reading the *environment's* charset

Your build flavor is fixed at compile time — ask the environment what it is:

:::sample
```c
printf("env charset: %s\n",
       OCI_EnvironmentGetCharset(env) == OCI_CHAR_WIDE ? "wide" : "mbcs");
```

```cpp
std::cout << (env.GetCharset() == Environment::CharsetWide ? "wide" : "mbcs") << '\n';
```
:::

## Scenario — per-type text conversion

`SetFormat` reshapes how **string getters** render non-text columns — handy for
exports and logs without touching SQL `TO_CHAR` calls:

:::sample
```c
OCI_EnvironmentSetFormat(env, OCI_FMT_DATE,    OTEXT("YYYY-MM-DD"));
OCI_EnvironmentSetFormat(env, OCI_FMT_NUMERIC, OTEXT("999G999G990D00"));

/* any OCI_ResultsetGetString on a DATE/NUMBER column now uses these */
const otext *v = OCI_ResultsetGetString(rs, 0);
```

```cpp
env.SetFormat(FormatType::FormatDate,    "YYYY-MM-DD");
env.SetFormat(FormatType::FormatNumeric, "999G999G990D00");

String v = rs.Get<String>(0);
```
:::

## Putting it together — the typical sequence

When you work with text in OCILIB, the order of decisions is usually this:

1. **Pick your build flavor first.** MBCS (`ocilibm`) vs wide (`ocilibw`)
   decides what `otext` is — you cannot mix libraries, so this choice is made
   once, at link time.
2. **Write every SQL literal and string through `OTEXT(...)`** (or `String` in
   C++). That single habit is what makes the same source compile on both builds.
3. **Fetch with the string getters** — they convert to `otext` for you. Remember
   `GetString` converts *any* column, not just `VARCHAR2`: it is your universal
   "give me text" escape hatch.
4. **Check the charset form when binding into NCHAR/NVARCHAR2.** Grab the
   `OCI_Bind*` returned by the bind call and force `OCI_BindSetCharsetForm` if
   the column's form isn't inferred correctly.
5. **Set display formats once, up front.** `OCI_EnvironmentSetFormat` on the
   environment reshapes every later `GetString` on dates and numbers — ideal
   for exports and logs.

Hints worth keeping: API lengths are *characters*, not bytes; `sizeof` betrays
you on wide builds; reach for `StringView` where the API offers it — it's a
zero-copy borrow of text the row still owns.

## Try it yourself

1. Build the same source against `ocilibm` (MBCS) and `ocilibw` (wide) — `OTEXT`
   is what makes it work.
2. Set a custom date format and watch the string getters' output change.
3. Check the environment's charset value on your build.

## Common pitfalls

- **Passing `char*` to `otext*` params** — compiles on MBCS, explodes on wide builds.
- **Using `strlen` on `otext*`** — MBCS-only; keep to API-provided lengths.
- **`sizeof` vs char count** — on wide builds they differ by 2×.

## Recap

- `otext`/`OTEXT`/`ocilib::String` = write-once, run-on-any-build strings.
- `SetFormat` controls OCILIB's date/number↔text conversions.

**Next:** the `OCI_Date` type in full.
