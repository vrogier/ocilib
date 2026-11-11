---
title: Sharding — OCI_ShardingKey
navtitle: Sharding
part: 9
level: advanced
oracle: "12.2"
demos: [sharding.c, sharding.cpp]
sql: sharding.sql
---

# Sharding — the `ShardingKey`

**Objectives** — by the end of this page you will be able to:

- build sharding keys from typed columns
- connect to a specific shard
- check out sharded connections from a pool

## The ShardingKey

Oracle Sharding splits one logical table across many physical databases (**shards**).

A `ShardingKey` tells the client which shard holds your data — an ordered list of typed column values built on the environment, in the same order as the sharded table's key definition.

A *super-sharding key* adds a second level for composite shards.

:::sample
```c
OCI_ShardingKey *skey = OCI_ShardingKeyCreate(env);
OCI_ShardingKeyAddString(skey, OTEXT("GOLD"));              /* cust class   */
OCI_Number *num = OCI_NumberCreate(env);
OCI_NumberSetValue(num, OCI_NUM_INT32, &(int){42});
OCI_ShardingKeyAddNumber(skey, num);                        /* cust_id      */

/* optional super-sharding key for composite shards */
OCI_ShardingKey *sskey = OCI_ShardingKeyCreate(env);
OCI_ShardingKeyAddString(sskey, OTEXT("EUROPE"));
```

```cpp
ShardingKey skey(env);
skey.AddColumn<String>("GOLD");             /* cust class  */
Number num(env);
num.SetValue<int>(42);
skey.AddColumn<Number>(num);                /* cust_id     */
ShardingKey sskey(env);
sskey.AddColumn<String>("EUROPE");
```
:::

| C function | C++ method | Purpose |
|------------|------------|---------|
| `OCI_ShardingKeyCreate` | `ShardingKey` ctor (`env`) | lifecycle |
| `OCI_ShardingKeyFree` | — (RAII) | release |
| `OCI_ShardingKeyReset` | `Reset()` | clear the key values |
| `OCI_ShardingKeyAdd{String,Number,Date,Timestamp,Raw}` | `AddColumn<T>` | append key columns, in order |
| `OCI_ShardingKeyToText` | `ToText()` | base64 debug rendering |

## Connecting to a shard

The connect goes to the **shard catalog/GSM endpoint** — Oracle routes you to the
right shard based on the key. From then on, it's an ordinary connection:

:::sample
```c
OCI_Connection *cn = OCI_ConnectionCreateSharded(
    env, db, usr, pwd, OCI_SESSION_DEFAULT, skey, sskey);
```

```cpp
Connection cn(env, db, usr, pwd, Environment::SessionDefault, skey, sskey);
```
:::

## Pool variant

Same checkout as a normal pool but routed by key — the pool keeps sessions per
shard transparently:

:::sample
```c
OCI_Connection *cn = OCI_PoolGetConnectionSharded(env, pool, NULL, skey, sskey);
```

```cpp
auto cn = pool.GetConnection("", skey, sskey);
```
:::

## Putting it together — the typical sequence

Sharding adds a routing key to the connect story:

1. **Create the pool or connection** with shard awareness — sharded pools
   route by key.
2. **Get a sharded connection** — `OCI_PoolGetConnectionSharded` with the
   shard key string.
3. **Work normally** — the routed session goes to the right shard
   transparently.

Hints: the shard key is opaque to OCILIB — a string the server interprets;
`Sharded` connect is a pool feature — ad-hoc sharded connects don't exist
the same way; and version-gate this hard — sharding is 12.2+ client and a
sharded topology server-side.

## Try it yourself

1. On a sharded sandbox, connect with a `skey` and verify which shard answered
   (`select sys_context('USERENV','SHARD_ID') from dual`-style check).
2. `ShardingKeyToText` a key and decode it.

## Common pitfalls

- **Column order** — key columns must match the sharded table's key definition,
  in order.
- **Pre-12.2 server** — sharding needs 12cR2+; gate it.

## Recap

- `ShardingKey` = ordered typed key values → `ConnectionCreateSharded` /
  `PoolGetConnectionSharded` route to the right shard.

**Next:** SODA — documents without SQL.
