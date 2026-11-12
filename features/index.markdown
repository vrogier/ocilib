---
author: admin
comments: true
date: 2026-11-12 22:00:00+01:00
layout: page
slug: features
title: Features
---
OCILIB covers the capabilities used by almost all Oracle applications, while also supporting
specialized Oracle services and advanced data types.

| Feature | Minimum Oracle release |
|---|---|
| Cross-platform C API (ISO C11), with 32-bit and 64-bit support | 8.0 |
| Modern C++ API (C++17), header-only, with iterators usable with standard algorithms | 8.0 |
| MBCS builds on all platforms (UTF-8 or ANSI via `NLS_LANG`); wide UTF-16 builds on Windows | 8.0 |
| Automatic adaptation to the runtime Oracle client version | 8.0 |
| Runtime loading of Oracle client libraries | 8.0 |
| SQL and PL/SQL, including blocks, cursors, index-by tables, and nested tables | 8.0 |
| Connections, sessions, transactions, and connection pooling | 8.0 |
| Statement execution, binding, fetching, bulk operations, and DML returning | 8.0 |
| Oracle types: scalars, dates, timestamps, intervals, LOBs, LONGs, RAWs, ROWIDs, cursors, objects, REFs, and collections | 8.0 |
| Error handling, Oracle warnings, optional logging, and diagnostic statistics | 8.0 |
| SQL and Object APIs, schema description, multiple-client environments, and hash tables | 8.0 |
| Transparent Application Failover (TAF) and high availability | 8.0 |
| Oracle Direct Path loading | 8.0 |
| Scrollable statements and result sets | 9i |
| Advanced Queuing (AQ) | 9i |
| Statement caching | 9.2 |
| Database Change Notification / Continuous Query Notification | 10.2 |
| Remote database startup and shutdown | 10.2 |
| Oracle Sharding, including sharded connections and session pools | 12.2 |
| Oracle SODA (Simple Oracle Document Access; NoSQL API) | 18c |
| Oracle JSON data type support, including DOM and serialized JSON operations | 21c |
| Oracle VECTOR data type support | 23ai |

Feature availability also depends on the Oracle client version used by the application.
