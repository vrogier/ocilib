---
title: JSON DOM — OCI_JsonNode
navtitle: JSON DOM
part: 5
level: intermediate
oracle: "21"
demos: [json.c, json.cpp]
---

# JSON DOM — the `JsonNode` object

**Objectives** — by the end of this page you will be able to:

- navigate a JSON document node-by-node
- read and write every scalar node type
- manipulate objects and arrays
- query by path, compare and deep-copy nodes

## Meet the JsonNode

A `Json` document is a tree of **nodes** — each node is one value (object, array,
string, number, boolean, binary, date/timestamp/interval, or null) carrying a type
tag (`OCI_JNT_*`). The document owns the root; nodes you *extract* or *create*
(`GetFieldByName`, `GetItem`, `GetByPath`, `Create*`) are yours to free. Ask the
document for its root and walk from there:

:::sample
```c
OCI_JsonNode *root = OCI_JsonGetRoot(json);
OCI_JsonNode *emp  = OCI_JsonNodeGetFieldByName(root, OTEXT("employee"));
OCI_JsonNode *name = OCI_JsonNodeGetFieldByName(emp,  OTEXT("name"));

printf("%s\n", OCI_JsonNodeGetString(name, NULL));
OCI_JsonNodeFree(name); OCI_JsonNodeFree(emp);   /* root belongs to the doc */
```

```cpp
JsonNode root = doc.GetRoot();
JsonNode emp  = root["employee"];                 /* or GetFieldByName        */
JsonNode name = emp["name"];

std::cout << name.GetString() << "\n";            /* RAII — nothing to free   */
```
:::

## Scalars — get & set

| Type | Get | Set |
|------|-----|-----|
| ints | `GetShort/UShort/Int/UInt/BigInt/BigUInt` | `SetShort/…/SetBigUInt` |
| decimals | `GetFloat`/`GetDouble` | `SetFloat`/`SetDouble` |
| exact | `GetNumber` (`OCI_Number*`) | `SetNumber` |
| string | `GetString` | `SetString` |
| bool | `GetBoolean` | `SetBoolean` |
| date/time | `GetDate`/`GetTimestamp`/`GetInterval` | `SetDate`/`…` |
| binary | `GetBinary` | `SetBinary` |
| null | `IsNull` | `SetNull` |

## Objects & arrays

:::sample
```c
/* object ops */
OCI_JsonNodeGetFieldCount(node);
OCI_JsonNodeGetFieldByName(node, OTEXT("key"));
OCI_JsonNodeGetFieldByIndex(node, i);         /* 0-based */
OCI_JsonNodeGetFieldName(node, i);
OCI_JsonNodeHasField(node, OTEXT("key"));
OCI_JsonNodeSetField(node, OTEXT("key"), value_node);
OCI_JsonNodeRemoveField(node, OTEXT("key"));
OCI_JsonNodeRenameField(node, OTEXT("old"), OTEXT("new"));

/* array ops */
OCI_JsonNodeGetItemCount(node);
OCI_JsonNodeGetItem(node, i);                 /* 0-based */
OCI_JsonNodeSetItem(node, i, item);
OCI_JsonNodeAppendItem(node, item);
OCI_JsonNodeInsertItem(node, i, item);
OCI_JsonNodeRemoveItem(node, i);
OCI_JsonNodeRemoveItems(node, start, count);  /* range delete */
```

```cpp
/* object ops */
node.GetFieldCount();
node["key"];                                    /* field access             */
node.GetFieldByIndex(i);                        /* 0-based                  */
node.GetFieldName(i);
node.HasField("key");
node.SetField("key", value_node);
node.RemoveField("key");
node.RenameField("old", "new");

/* array ops */
node.GetItemCount();
node[i];                                        /* 0-based item access      */
node.SetItem(i, item);
node.AppendItem(item);
node.InsertItem(i, item);
node.RemoveItem(i);
node.RemoveItems(start, count);                 /* range delete             */
```
:::

## Building new nodes

:::sample
```c
OCI_JsonNode *s = OCI_JsonNodeCreateString(json, OTEXT("plsql"), 0);
OCI_JsonNodeAppendItem(skills, s);
OCI_JsonNodeFree(s);            /* appended a copy — free your ref */
```

```cpp
JsonNode s = doc.CreateString("plsql");
skills.AppendItem(s);           /* appended a copy — RAII frees your ref    */
```
:::

`OCI_JsonNodeCreate{,Null,Boolean,String,Number,Date,Timestamp,Interval,Binary,
Object,Array}` — build detached nodes then attach them to the tree.

## Navigation & deep ops

| C function | C++ method | Purpose |
|------------|------------|---------|
| `OCI_JsonNodeGetParent` | `GetParent()` | walk up |
| `OCI_JsonNodeGetByPath` | `GetByPath` / `node["key"]`/`node[i]` | `employee.skills[0]`-style lookup |
| `OCI_JsonNodeGetJson` | `GetJson()` | owning document |
| `OCI_JsonNodeAssign` | `Assign` | deep copy node → node |
| `OCI_JsonNodeEquals` | `Equals` | deep compare |
| `OCI_JsonNodeClear` | `Clear()` | empty object/array in place |

## The C++ way — the nice syntax

`Json`/`JsonNode` mirror everything, plus sugar:

```cpp
JsonNode root = doc.GetRoot();
JsonNode emp  = root["employee"];          /* field access, not GetFieldByName */
std::cout << emp["name"].GetString() << "\n";

emp["skills"][0].SetString("C++");         /* array indexing + setters */
for (auto&& n : emp)                       /* iterators walk fields/items */
{
    std::cout << n.GetType() << "\n";
}

auto num = emp["sal"].GetNumeric<Number>();/* covers exact Number too */
```

## Try it yourself

1. Parse the employee doc; print every field name via `GetFieldName`/`GetFieldByIndex`.
2. `GetByPath("employee.skills[1]")` vs manual `GetFieldByName`+`GetItem` — same node?
3. Build a fresh object node, append it into an array, `ToText` the result.

## Common pitfalls

- **Leaking extracted nodes** — extracted/created nodes are owned refs; free them
  (root is doc-owned). C++ RAII covers this.
- **Editing immutable docs** — mark the document mutable first.
- **Indexing** — `node[i]`, iterators and all index-taking methods
  (`GetItem`, `GetFieldByIndex`) are 0-based uniformly.

## Recap

- `GetRoot` → typed node tree; scalar get/set, object field ops, array item ops,
  `GetByPath`, `Assign`/`Equals`.

**Next:** Checkpoint 4 — JSON profiles + LOB photos on the employee schema.
