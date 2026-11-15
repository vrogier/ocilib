---
title: AI vectors — OCI_Vector (23ai)
navtitle: AI vectors 23ai
part: 2
level: advanced
oracle: "23"
---

# AI vectors — `OCI_Vector` (23ai)

**Objectives** — by the end of this page you will be able to:

- create `VECTOR` values from typed arrays or text
- bind and fetch embeddings for AI Vector Search
- read vector format/dimension metadata
- create and read sparse vectors (23ai RU 23.7+)

## In context — why vectors exist in the database

Oracle 23ai added the `VECTOR` type to support **AI Vector Search**: modern
applications store *embeddings* — arrays of floats produced by an embedding
model that encode the *meaning* of a text, image, or product — and then ask the
database "find the 10 rows closest to this query embedding" using a distance
function like `VECTOR_DISTANCE` with `COSINE` or `EUCLIDEAN`. Keeping the
vectors in the same tables as the data they describe means the nearest-neighbour
query is plain SQL with joins, filters, and transactions — no separate vector
store to synchronize.

A `VECTOR` column may declare a fixed dimension (`VECTOR(384)`) or accept
anything (`VECTOR(*)`), and each vector carries a *format* describing its
element type (FLOAT32, FLOAT64, INT8, or the packed BINARY form). `OCI_Vector`
mirrors that pair — format + dimension count + values — and validates them for
you at `SetValues` time: the format selects which C array type the buffer is
interpreted as, and the dimension count must match the column's declaration.

On the OCILIB side, `OCI_Vector` is a connection-scoped handle (created from
`cn`, unlike `Date`/`Interval` which live on the environment). `GetInfo` is your
inspection call — read format + dims *before* choosing a `GetValues` buffer
type, especially on `VECTOR(*)` columns where rows can differ. The C++
`Vector::Set<T>`/`Get<T>` templates collapse the format argument into the C++
type, which removes the classic "wrong format tag" mistake.

## The Vector object

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
| `OCI_VectorIsSparse` | `IsSparse` | sparse storage flag (23.7+) |
| `OCI_VectorSetSparseValues` | `SetSparse` | indices + non-zero values → sparse vector |
| `OCI_VectorGetSparseValues` | `GetSparse` | sparse vector → indices + values |

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
    (`OCI_EnvironmentGetOCIRuntimeVersion`, `OCI_ConnectionGetServerVersionString`, page 140).

## Round-trip

:::sample
```c
OCI_StatementPrepare(st, OTEXT("insert into docs(id, embedding) values(1, :v)"));
OCI_StatementBindVector(st, OTEXT(":v"), v);
OCI_StatementExecutePrepared(st);

OCI_StatementExecute(st, OTEXT("select embedding from docs where id = 1"));
OCI_Resultset *rs = OCI_StatementGetResultset(st);
OCI_ResultsetFetchNext(rs);
OCI_Vector *got = OCI_ResultsetGetVector(rs, 0);            /* borrowed */

OCI_VectorFormat fmt;
OCI_UInt32 dims;
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
    Vector got = st.GetResultset().Get<Vector>(0); /* borrowed */
    std::vector<float> vals(got.GetDimensionCount());
    got.Get<float>(vals.data());
}
```
:::

Similarity search is just SQL — `vector_distance(embedding, :q)` — bind the query
vector the same way.

## Scenario — nearest-neighbour ranking

The canonical AI Vector Search query — embed the question, bind the vector, order
by distance:

:::sample
```c
OCI_Vector *q = OCI_VectorCreate(cn);
OCI_VectorSetValues(q, OCI_VEC_FLOAT32, 384, query_embedding);

OCI_StatementPrepare(st, OTEXT(
    "select id, title, vector_distance(embedding, :q, COSINE) as dist"
    "  from docs order by dist fetch first 10 rows only"));
OCI_StatementBindVector(st, OTEXT(":q"), q);
OCI_StatementExecutePrepared(st);

OCI_Resultset *rs = OCI_StatementGetResultset(st);
while (OCI_ResultsetFetchNext(rs))
{
    printf("%4d  dist=%.4f  %s\n",
           OCI_ResultsetGetInt32 (rs, 0),
           OCI_ResultsetGetDouble(rs, 2),
           OCI_ResultsetGetString(rs, 1));
}
```

```cpp
Vector q(con);
q.Set<float>(query_embedding);

st.Prepare(
    "select id, title, vector_distance(embedding, :q, COSINE) as dist"
    "  from docs order by dist fetch first 10 rows only");
st.Bind(":q", q);
st.ExecutePrepared();

Resultset rs = st.GetResultset();
while (rs.Next())
{
    std::cout << rs.Get<Int32>(0) << "  dist=" << rs.Get<Double>(2)
              << "  " << rs.Get<String>(1) << "\n";
}
```
:::

## Scenario — mixed-format inspection

A `vector(*)` (flex) column can hold heterogeneous vectors — read `GetInfo` first,
then pick the right element type:

:::sample
```c
OCI_Vector *v = OCI_ResultsetGetVector(rs, 0);       /* borrowed       */

OCI_VectorFormat fmt;
OCI_UInt32       dims;
OCI_VectorGetInfo(v, &fmt, &dims);

switch (fmt)
{
    case OCI_VEC_FLOAT32:
        {
            float f[4096];
            OCI_VectorGetValues(v, f);
            break;
        }
    case OCI_VEC_FLOAT64:
        {
            double d[2048];
            OCI_VectorGetValues(v, d);
            break;
        }
    case OCI_VEC_INT8:
        {
            OCI_Int8 i[2048];
            OCI_VectorGetValues(v, i);
            break;
        }
    default:
        /* OCI_VEC_BINARY / OCI_VEC_FLEX — serialize instead */      ;
}
```

```cpp
Vector v = rs.Get<Vector>(0);                        /* borrowed       */

switch (v.GetFormat())
{
    case Vector::Float32:
        {
            std::vector<float> f(v.GetDimensionCount());
            v.Get<float>(f.data());
            break;
        }
    case Vector::Float64:
        {
            std::vector<double> d(v.GetDimensionCount());
            v.Get<double>(d.data());
            break;
        }
    case Vector::Int8:
        {
            std::vector<Int8> i(v.GetDimensionCount());
            v.Get<Int8>(i.data());
            break;
        }
    default:
        /* Binary / Flex — serialize instead */                   ;
}
```
:::

## Scenario — text round-trip for logs/debugging

`ToText`/`FromText` are handy for dumps and test fixtures:

:::sample
```c
otext buf[4096];
OCI_UInt32 n = sizeof(buf) / sizeof(otext);
OCI_VectorToText(v, &n, buf);            /* "[0.12,-0.44,0.87]"       */

OCI_Vector *v2 = OCI_VectorCreate(cn);
OCI_VectorFromText(v2, buf, n, OCI_VEC_FLOAT32, 3);
```

```cpp
String s = v.ToString();                 /* "[0.12,-0.44,0.87]"       */

Vector v2(con);
v2.FromString(s, Vector::Float32, 3);
```
:::

## Scenario — sparse vectors (23ai RU 23.7)

Some embedding models produce very high-dimensional vectors where almost every
dimension is zero — learned sparse lexical representations like SPLADE, or plain
bag-of-words features that can reach tens of thousands of dimensions.

Storing those densely would waste space and bandwidth: Oracle 23ai RU **23.7**
added `VECTOR(n, format, SPARSE)` columns that persist only the non-zero
dimensions, as a list of **1-based** dimension indices plus the values at those
positions.

A sparse vector's textual form spells the storage out:
`'[5,[2,4],[1.0,1.0]]'` means a 5-dimension vector whose only non-zero
components are 1.0 at positions 2 and 4.

Reading a sparse vector is a capacity-based call — you hand over arrays sized
for the worst case (the non-zero count is at most the total dimension count)
and get back how many entries were actually populated; setting one takes the
dimension count and the non-zero list in one shot:

:::sample
```c
OCI_UInt32 idx[2] = { 2, 4 };            /* 1-based positions */
float      val[2] = { 1.5f, 2.5f };

OCI_Vector *v = OCI_VectorCreate(cn);
OCI_VectorSetSparseValues(v, OCI_VEC_FLOAT32, 5, 2, idx, val);

/* read back: count goes in as buffer capacity, out as actual count */

OCI_UInt32 idx_out[8];
float      val_out[8];
OCI_UInt32 count = 8;

OCI_VectorGetSparseValues(v, idx_out, val_out, &count);   /* count == 2 */

if (OCI_VectorIsSparse(v)) { /* stored sparsely */ }
```

```cpp
std::vector<UInt32> idx{ 2, 4 };          /* 1-based positions */
std::vector<float>  val{ 1.5f, 2.5f };

Vector v(con);
v.SetSparse<float>(idx, val, 5);

std::vector<UInt32> idx_out;
std::vector<float>  val_out;
v.GetSparse<float>(idx_out, val_out);     /* resized to actual count */

bool sparse = v.IsSparse();
```
:::

A described `VECTOR(…, SPARSE)` column surfaces the declaration too:
`OCI_ColumnIsVectorSparse` reports it (C++: `Column::IsVectorSparse`), and the
full SQL type name reports `VECTOR(8, FLOAT32, SPARSE)`.

!!! oracle "Version gate"
    Sparse vectors need Oracle **23ai RU 23.7** client **and** server.
    On older versions the sparse calls fail like any unsupported feature —
    gate on `OCI_ConnectionGetVersion` / `GetOCIRuntimeVersion` first.

## Putting it together — the typical sequence

A vector round-trip has four stages, and each has its own pitfall to avoid:

1. **Build the vector**: `Create` on the connection, then `SetValues` with the
   format tag that matches your C array (`OCI_VEC_FLOAT32` for `float[]`) — or
   `FromText` for `'[0.1, 0.2, …]'` strings.
2. **Bind for insert** — the dimension count must match the column declaration;
   `VECTOR(*)` accepts anything, `VECTOR(384)` does not.
3. **Query with it** — `VECTOR_DISTANCE(col, :q, COSINE)` in the ORDER BY, with
   your query vector bound the same way.
4. **Fetch and inspect**: `GetInfo` *first* — format + dims — *then*
   `GetValues` into a buffer of the matching element type.

Hints: under-sizing the `GetValues` buffer is the classic crash — read `dims`
from `GetInfo`, don't guess; on `VECTOR(*)` columns the format can differ row
to row, so switch on `GetInfo` rather than assuming; and the whole feature is
23ai-only — check `ServerVersion` before shipping it.

## Try it yourself

1. `create table docs(id number, embedding vector(3))`; insert two vectors; run a
   `vector_distance` ranking query.
2. Round-trip a 768-dim FLOAT32 vector via `SetValues`/`GetValues` and diff arrays.
3. Read `OCI_VectorGetInfo` on a `vector(*)` (flex) column — what dims does it report?

## Common pitfalls

- **Dimension mismatch** — `vector(768)` rejects other sizes; `vector(*)` accepts any.
- **Using it on pre-23ai** — feature-gate; see the version matrix appendix.
- **Under-sized `GetValues` buffer** — read `dims` from `GetInfo` first.
- **Under-sized sparse arrays** — `GetSparseValues` takes the *capacity* in `count`;
  size it to the dimension count and read the actual non-zero count it returns.
- **0-based indices** — sparse indices are **1-based** dimension positions, not C offsets.

## Recap

- `OCI_Vector` = format + dims + typed values; `SetValues`/`FromText` to build,
  `GetValues`/`ToText`/`GetInfo` to read; bind/fetch like any column.
- Sparse vectors (23.7+) store only non-zero dims — `SetSparseValues` with
  1-based indices, `GetSparseValues` with capacity-in/count-out, `IsSparse` to
  check what you fetched.

**Next:** LOBs — the streaming workhorse for big text and binary.
