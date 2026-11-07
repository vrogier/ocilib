---
title: Strings, charsets & format helpers
navtitle: Strings & charsets
part: 5
level: beginner
demos: [format.c]
---

# Strings, charsets & format helpers

**Objectives** — by the end of this page you will be able to:

- write source code that compiles on ANSI and Unicode builds unchanged
- convert dates/numbers to text with `OCI_SetFormat`-style helpers
- use `ocilib::String`/`StringView` correctly in C++

## The `otext` contract

OCILIB ships two string personalities:

| Build | `otext` | `OTEXT("x")` | `ocilib::String` |
|-------|---------|--------------|------------------|
| ANSI (`OCI_CHARSET_ANSI`) | `char` | `"x"` | `std::string` |
| Unicode (`OCI_CHARSET_WIDE`) | `unsigned short` | `L"x"`-style literal | `std::wstring` |

Rules for portable code:

- API parameters and returns are `otext*`/`const otext*` — never `char*` in headers.
- Wrap **every** literal in `OTEXT(…)`.
- `sizeof(buf)` is bytes; `sizeof(buf)/sizeof(otext)` is characters — API length
  args are character counts.
- The environment reports its *runtime* charset flavor (page 140).

:::sample
```c
otext name[64];
OCI_Immediate(cn, OTEXT("select ename from employees where empno = 7839"),
              OCI_ARG_TEXT, name);
```

```cpp
String name;
st.Execute("select ename from employees where empno = 7839");
if (st.GetResultset().Next())
{
    name = st.GetResultset().Get<String>(1);   /* String = otext-typed */
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
OCI_SetFormat(cn, OCI_FMT_DATE, OTEXT("YYYY-MM-DD"));
/* now string getters on a DATE column return ISO-style text */
```

```cpp
con.SetFormat(FormatType::FormatDate, "YYYY-MM-DD");
```
:::

## In C++ — `String` and `StringView`

- `ocilib::String` = `std::basic_string<otext>` — what every C++ API returns.
- `ocilib::StringView` = `std::basic_string_view<otext>` — zero-copy view where the
  API offers it.
- Plain `const char*` literals work with C++ APIs **on ANSI builds**; write
  `OTEXT(…)`/`String` when you want portability.

## Try it yourself

1. Build the same source against `ociliba` (ANSI) and `ocilibw` (UNICODE) — `OTEXT`
   is what makes it work.
2. Set a custom date format and watch the string getters' output change.
3. Check the environment's charset value on your build.

## Common pitfalls

- **Passing `char*` to `otext*` params** — compiles on ANSI, explodes on wide builds.
- **Using `strlen` on `otext*`** — ANSI-only; keep to API-provided lengths.
- **`sizeof` vs char count** — on wide builds they differ by 2×.

## Recap

- `otext`/`OTEXT`/`ocilib::String` = write-once, run-on-any-build strings.
- `SetFormat` controls OCILIB's date/number↔text conversions.

**Next:** the `OCI_Date` type in full.
