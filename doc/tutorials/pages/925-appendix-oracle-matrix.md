---
title: Appendix — Oracle version feature matrix
navtitle: Version matrix
part: 10
---

# Appendix — Oracle version feature matrix

Gate version-dependent features with `OCI_VER_MAJ(OCI_EnvironmentGetOCIRuntimeVersion(env))`
**and** the server version — you need *both* client and server ≥ the level.

| Feature | Min Oracle | Pages |
|---------|-----------|-------|
| `TIMESTAMP WITH TIME ZONE`, intervals | 9i | 520–525 |
| Scrollable resultsets | 9i | 275 |
| AQ (queues, agents) | 8i+ | 710–725 |
| Direct path loading | 8i+ | 815 |
| `SYS.XMLTYPE` | 9i+ | 565 |
| Implicit resultsets (`DBMS_SQL.RETURN_RESULT`) | 12c | 325 |
| Sharding (`OCI_ShardingKey`) | 12cR2 | 850 |
| SODA | 18c | 855–860 |
| `OCI_SRM_SECURE_NOTIFICATION` | 19.4 | 730 |
| `JSON` native type + `OCI_Json*` DOM | 21c | 570–575 |
| `VECTOR` type (`OCI_Vector`) | 23ai | 540 |
| `OCI_JsonValidate` schema validation | 23ai | 575 |
| `OCI_ColumnHasJsonSchema` constraint | 23ai | 240 |
| SQL `BOOLEAN` binds/getters | 23ai | 230/245 |
| Admin session modes `SYSRAC` etc. | 11gR1–12cR2 | 110 |
| `OCISoda*` multi-key fast path | 21c+ client | 860 |

## Version helpers

:::sample
```c
OCI_UInt32 rt  = OCI_EnvironmentGetOCIRuntimeVersion(env);        /* client MMmmRR    */
OCI_UInt32 cv  = OCI_ConnectionGetVersion(cn);      /* negotiated       */
const otext *sv = OCI_ConnectionGetServerVersionString(cn);         /* "23.5.0.0"       */

if (OCI_VER_MAJ(rt) >= 23 && OCI_VER_MAJ(cv) >= 23) { /* VECTOR ok */ }
```

```cpp
UInt32 rt = Environment::GetOCIRuntimeVersion();    /* client MMmmRR    */
UInt32 cv = con.GetVersionConnection();             /* negotiated       */
String sv = con.GetServerVersion();                 /* "23.5.0.0"       */
```
:::

Macros: `OCI_VER_MAJ/MIN/REV(v)`, `OCI_VER_MAKE(x,y,z)`, `OCI_12_2`, `OCI_21_1`,
`OCI_23_5`…

## Client-side limitations

- `OCI_CHARSET_WIDE` + `OCISodaIndex*`/`OCI_SRM_SECURE_NOTIFICATION` — known
  Oracle client bug on 23ai; ANSI unaffected.
- Some attributes return defaults on older clients — check `OCIRuntimeVersion`
  before relying on them.
