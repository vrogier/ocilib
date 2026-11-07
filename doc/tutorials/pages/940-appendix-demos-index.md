---
title: Appendix — demo programs index
navtitle: Demo index
part: 10
---

# Appendix — demo programs index

Every `demo/*.c` (and matching `.cpp`) is a runnable twin of a tutorial page —
credentials come from `OCILIB_DEMO_*`, SQL setups from the matching `.sql`.

| Demo | Tutorial page |
|------|---------------|
| `conn.c` | 070 first connection |
| `abort.c` | 130 break, 430 threads |
| `err.c`, `err_ctx.c`, `err_warning.c` | 080 error handling |
| `instance.c` | 820 database admin |
| `fetch.c/.cpp`, `fetch_struct.c` | 220–235 fetching, struct map |
| `scroll.c/.cpp` | 275 scrollable |
| `bind.c/.cpp` | 245–255 binds |
| `array.c/.cpp` | 810 array binds |
| `exec.c` | 210 statement lifecycle |
| `returning.c/.cpp`, `rowid.c` | 280 rowid/returning |
| `plsql_block.c` | 310 blocks |
| `cursor.c/.cpp` | 320 ref cursors |
| `implicit_resultset.c/.cpp` | 325 implicit rs |
| `plsql_table.c` (+`.sql`) | 330 plsql tables |
| `output.c` | 335 DBMS_OUTPUT |
| `pool.c/.cpp` | 420–425 pooling |
| `date.c/.cpp`, `timestamp.c/.cpp` | 515–520 |
| `number.c` | 530 |
| `format.c` | 510 formats |
| `lob.c` (+`lob.sql`) | 550–555 lobs |
| `file.c` (+`file.sql`) | 560 BFILE |
| `long.c` (+`long.sql`) | 285 longs |
| `json.c/.cpp` | 570–575 JSON |
| `desc.c`, `meta.c` | 610 TypeInfo |
| `object.c/.cpp` (+`object.sql`) | 615–620 objects |
| `coll.c/.cpp` (+`collections.sql`) | 630–635 collections |
| `ref.c` (+`ref.sql`) | 640 REFs |
| `super_types.c/.cpp` (+`suber_types.sql`) | 625 inheritance |
| `geometry.c/.cpp` (+`geometry.sql`) | 645 case study |
| `queue.c`, `queue_async.c` (+`queue.sql`) | 710–725 AQ |
| `notification.c/.cpp` | 730 CQN |
| `dirpath.c`, `dirpath_complete.c` (+`dirpath.sql`) | 815 direct path |
| `hash.c` | 825 hash tables |
| `sharding.c/.cpp` (+`sharding.sql`) | 850 sharding |
| `soda.c/.cpp` | 855–860 SODA |
| `ocilib_demo.c/.cpp` (+`Makefile_demo`) | whole-API driver |
| `geometry.sql`, `products.sql`, `dates.sql`, `dirpath.sql`, `number.sql` | schema helpers for several pages |

Build them with `demo/Makefile_demo` (Unix) or the Windows test project — see
Part 0 setup pages.
