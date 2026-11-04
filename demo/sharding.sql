/*
 * sharding.sql — DDL fixture for the OCILIB sharding demo
 *
 * WARNING: this script requires a real Oracle Sharding deployment
 * (GDS catalog + registered shards). It cannot be executed against a
 * regular standalone database - sharded table creation is only
 * allowed on a shard catalog database.
 *
 * It demonstrates the DDL users write when declaring sharding keys
 * and super sharding keys.
 */

/* single-column sharding key, consistent hash partitioning */

create sharded table customers (
    cust_id   number       primary key,
    name      varchar2(30),
    region    varchar2(20)
)
partition by consistent hash (cust_id)
tablespace set ts1
duplicates none;

/* composite sharding key + super sharding key (shard group) example:
   rows are first grouped by shard group (region) then spread across
   the group shards by the remaining key columns                   */

create sharded table orders (
    order_id  number,
    region    varchar2(20),
    cust_id   number
)
partition by consistent hash (region, cust_id)
tablespace set ts1;
