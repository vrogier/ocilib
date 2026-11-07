---
title: AI vectors — OCI_Vector (23ai)
navtitle: AI vectors 23ai
part: 5
level: advanced
oracle: "23"
---

# AI vectors — `OCI_Vector` (23ai)

**Objectives** — by the end of this page you will be able to:

- create `VECTOR` values from typed arrays or text
- bind and fetch embeddings for AI Vector Search
- read vector format/dimension metadata

## Meet the Vector object

Oracle 23ai's `VECTOR` type stores embeddings for similarity search.

A `Vector` carries a typed numeric array plus a **format** (element type) and **dimension count** — created on the connection, filled from a C array or text, bound like any other datatype:

| C function | C++ method | Purpose |
|------------|------------|---------|
| `OCI_VectorCreate` | `Vector` ctor (`con`) | lifecycle |
| `OCI_VectorFree` | — (RAII) | release |
| `OCI_VectorArrayCreate` / `OCI_VectorArrayFree` | — | bulk arrays |
| `OCI_VectorSetValues` | `Set<T>` | typed array → vector |
| `OCI_VectorGetValues` | `Get<T>` | vector → array (caller buffer) |
| `OCI_VectorGetInfo` | `GetFormat`/`GetDimensionCount` | format + dimension count |
| `OCI_VectorFromText` | `FromString` | `'[0.1, 0.2]'` → vector |
| `OCI_VectorToText` | `ToString` | vector → text |

Formats select the element type:

| C enum | Element type |
|--------|--------------|
| `OCI_VEC_FLOAT32` | 32-bit float |
| `OCI_VEC_FLOAT64` | 64-bit float |
| `OCI_VEC_INT8` | 8-bit int |
| `OCI_VEC_BINARY` | binary-packed (23.5+) |
| `OCI_VEC_FLEX` | server-decided (describe-only) |

:::sample
```c
float emb[3] = {0.12f, -0.44f, 0.87f};

OCI_Vector *v = OCI_VectorCreate(cn);
OCI_VectorSetValues(v, OCI_VEC_FLOAT32, 3, emb);

/* or from text: */
OCI_VectorFromText(v, OTEXT("[0.1, 0.2, 0.3]"), 12, OCI_VEC_FLOAT32, 3);
```

```cpp
std::vector<float> emb{0.12f, -0.44f, 0.87f};

Vector v(con);
v.Set<float>(emb);                                /* std::vector<T> → vector  */

/* or from text: */
v.FromString("[0.1, 0.2, 0.3]", Vector::Float32, 3);
```
:::

!!! oracle "Version gate"
    `VECTOR` needs Oracle **23ai** client **and** server — check both
    (`OCI_GetOCIRuntimeVersion`, `OCI_GetVersionServer`, page 140).

## Round-trip

:::sample
```c
OCI_Prepare(st, OTEXT("insert into docs(id, embedding) values(1, :v)"));
OCI_BindVector(st, OTEXT(":v"), v);
OCI_Execute(st);

OCI_ExecuteStmt(st, OTEXT("select embedding from docs where id = 1"));
OCI_Resultset *rs = OCI_GetResultset(st);
OCI_FetchNext(rs);
OCI_Vector *got = OCI_GetVector(rs, 1);            /* borrowed */

OCI_VectorFormat fmt; OCI_UInt32 dims;
OCI_VectorGetInfo(got, &fmt, &dims);
float vals[768];
OCI_VectorGetValues(got, vals);                    /* buffer ≥ dims floats */
```

```cpp
st.Prepare("insert into docs(id, embedding) values(1, :v)");
st.Bind(":v", v);
st.ExecutePrepared();

st.Execute("select embedding from docs where id = 1");
if (st.GetResultset().Next())
{
    Vector got = st.GetResultset().Get<Vector>(1); /* borrowed */
    std::vector<float> vals(got.GetDimensionCount());
    got.Get<float>(vals.data());
}
```
:::

Similarity search is just SQL — `vector_distance(embedding, :q)` — bind the query
vector the same way.

## Try it yourself

1. `create table docs(id number, embedding vector(3))`; insert two vectors; run a
   `vector_distance` ranking query.
2. Round-trip a 768-dim FLOAT32 vector via `SetValues`/`GetValues` and diff arrays.
3. Read `OCI_VectorGetInfo` on a `vector(*)` (flex) column — what dims does it report?

## Common pitfalls

- **Dimension mismatch** — `vector(768)` rejects other sizes; `vector(*)` accepts any.
- **Using it on pre-23ai** — feature-gate; see the version matrix appendix.
- **Under-sized `GetValues` buffer** — read `dims` from `GetInfo` first.

## Recap

- `OCI_Vector` = format + dims + typed values; `SetValues`/`FromText` to build,
  `GetValues`/`ToText`/`GetInfo` to read; bind/fetch like any column.

**Next:** LOBs — the streaming workhorse for big text and binary.
