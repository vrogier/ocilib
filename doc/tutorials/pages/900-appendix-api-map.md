---
title: Appendix — C ↔ C++ API map
navtitle: API map
part: 10
---

# Appendix — C ↔ C++ API map

One row per public type; the C side shows representative functions, the C++ side the
wrapper class.

| C handle / family | C++ class | Notes |
|-------------------|-----------|-------|
| `OCI_EnvironmentCreate`/`Free`, env flags | `ocilib::Environment` | RAII; `Environment::Default/Threaded/Events` |
| `OCI_Error`, `OCI_GetLastError`, `OCI_EnvironmentSetErrorHandler` | `ocilib::Exception` | thrown by `core::Check`; same fields |
| `OCI_Connection*` create/free/ping/break/attrs | `ocilib::Connection` | `PingServer`, `IsServerAlive`, `SetTrace` |
| `OCI_Statement*`, Execute/Prepare/Parse/Describe, `OCI_StatementExecute*` | `ocilib::Statement` | `Execute`, `Prepare`, `ExecutePrepared`, `ExecuteAndGetResultset`, `GetNextResultset` |
| `OCI_Resultset`, `OCI_Fetch*`, `OCI_Get*` | `ocilib::Resultset` + `Row` | `Get<T>`, `GetOptional<T>`, `begin()`/`end()` iterators |
| `OCI_Column*` | `ocilib::Column` | same attribute getters + `HasJsonSchema` |
| `OCI_StatementBind*`, `OCI_StatementBind*Array`, `OCI_StatementRegister*`, `OCI_StatementAllowRebinding` | `BindInfo`, `Statement::Bind/Register`, `std::vector` binds | `Bind(name, T&, dir)` returns `BindInfo`; vectors + `SetBindArraySize` for arrays |
| `OCI_ConnectionCommit`/`Rollback`/`SetAutoCommit` | `Connection::Commit/Rollback/SetAutoCommit` | |
| `OCI_Transaction*` | `ocilib::Transaction` | `Start/Stop/Prepare/Resume/Forget` |
| `OCI_Pool*` | `ocilib::Pool` | `PoolType` Connection/Session; sharded `GetConnection` overload |
| `OCI_Date*` | `ocilib::Date` | ctor takes `Environment&` |
| `OCI_Timestamp*` | `ocilib::Timestamp` | ctor takes `Environment&` + `TimestampType` |
| `OCI_Interval*` | `ocilib::Interval` | ctor takes `Environment&` + `IntervalType` |
| `OCI_Number*` | `ocilib::Number` | templated numeric conversions |
| `OCI_Vector*` | `ocilib::Vector` | `Get<T>`/`SetValues` typed arrays |
| `OCI_Lob*` | `ocilib::Lob` | `Lob::Blob/Clob/NClob` subtypes |
| `OCI_File*` | `ocilib::File` | `File::BFile/CFile` |
| `OCI_Long*` | `ocilib::Long` | statement-owned like C |
| `OCI_XmlType*` | `ocilib::XmlType` | `GetContent` |
| `OCI_Json*`/`OCI_JsonNode*` | `ocilib::Json`/`JsonNode` | `node["key"]`, `node[i]`, `GetNumeric<T>`, iterators |
| `OCI_TypeInfo*` | `ocilib::TypeInfo` | `TypeInfo::Table/View/Type` |
| `OCI_Object*` | `ocilib::Object` | `Get<T>`/`Set<T>`/`GetOptional<T>` |
| `OCI_Collection*`/`OCI_Element*`/`OCI_Iterator*` | `ocilib::Collection<T>`/`CollectionElement`/`CollectionIterator` | templated element access |
| `OCI_Reference*` | `ocilib::Reference` | `GetObject` pins |
| `OCI_Enqueue`/`OCI_Dequeue`/`OCI_Message`/`OCI_Agent`/`OCI_Queue*` | `Enqueue`, `Dequeue`, `Message`, `Agent`, `Queue`, `QueueTable` | same model as C |
| `OCI_Subscription*`/`OCI_Event*` | `Subscription`, `Event` | `Register`/`Unregister`/`AddStatement` |
| `OCI_DirectPath*` | `DirectPath` | same load cycle |
| `OCI_Database*` | `Database` | `Startup`/`Shutdown` |
| `OCI_ShardingKey*` | `ShardingKey` | `AddColumn<T>` |
| `OCI_SodaDatabase/Coll/Doc/Options` + cursors | `SodaDatabase`, `SodaCollection`, `SodaDocument`, `SodaOptions`, `SodaDocumentCursor`, `SodaCollectionCursor` | cursors are input iterators |
| `OCI_Hash*` | — | use `std::unordered_map` |
| `OCI_Server*Output`, `OCI_ConnectionBreak`, `OCI_ConnectionPing`, `OCI_ConnectionImmediate*`, `OCI_ResultsetGetStruct`, `OCI_EnvironmentSetFormat` | `Connection::EnableServerOutput`/`GetServerOutput`, `Break`, `Ping` — mostly on `Connection`/`Statement` | `Immediate`/`GetStruct`/`SetFormat` stay C-side (no direct C++ twin) |
| `OCI_EnvironmentGetOCIRuntimeVersion`/`GetOCICompileVersion`/`GetImportMode`/`GetCharset`/`OCI_HandleGetEnvironment`/`OCI_EnvironmentEnableWarnings` | `Environment`/`core` helpers | mostly `Environment` methods/statics |
| `OCI_Connection{Enable,Disable,Get}ServerOutput` | `Connection::EnableServerOutput`/`DisableServerOutput`/`GetServerOutput` | DBMS_OUTPUT |

**Rule of thumb:** `OCI_<Type><Verb>` → `Class.Verb()`; `Create`/`Free` → ctor/dtor;
borrowed handles stay borrowed (transient acquires in C++).
