---
title: Appendix — full API index
navtitle: API index
part: 10
---

# Appendix — full API index

All 877 public C functions, grouped by family.

Each family links to the tutorial page that teaches it.

`Free`, `ArrayCreate`/`ArrayFree`, and `*ByName` twins follow the same rules as the base function in their family.

## `OCI_Agent* — Agent` → [aq-async](aq-async.html)

`OCI_AgentCreate`, `OCI_AgentFree`, `OCI_AgentGetAddress`, `OCI_AgentGetName`, `OCI_AgentSetAddress`, `OCI_AgentSetName`

## `OCI_StatementAllow*`

`OCI_StatementAllowRebinding`

## `OCI_Bind* — Bind` → [bind-management](bind-management.html)

`OCI_StatementBindArrayGetSize`, `OCI_StatementBindInt64Array`, `OCI_StatementBindCollectionArray`, `OCI_StatementBindDateArray`, `OCI_StatementBindDoubleArray`, `OCI_StatementBindFileArray`, `OCI_StatementBindFloatArray`, `OCI_StatementBindIntervalArray`, `OCI_StatementBindInt32Array`, `OCI_StatementBindJsonArray`, `OCI_StatementBindLobArray`, `OCI_StatementBindNumberArray`, `OCI_StatementBindObjectArray`, `OCI_StatementBindRawArray`, `OCI_StatementBindReferenceArray`, `OCI_StatementBindInt16Array`, `OCI_StatementBindStringArray`, `OCI_StatementBindTimestampArray`, `OCI_StatementBindUInt64Array`, `OCI_StatementBindUInt32Array`, `OCI_StatementBindUInt16Array`, `OCI_StatementBindVectorArray`, `OCI_StatementBindArraySetSize`, `OCI_StatementBindInt64`, `OCI_StatementBindBoolean`, `OCI_StatementBindCollection`, `OCI_StatementBindDate`, `OCI_StatementBindDouble`, `OCI_StatementBindFile`, `OCI_StatementBindFloat`, `OCI_BindGetAllocationMode`, `OCI_BindGetData`, `OCI_BindGetDataCount`, `OCI_BindGetDataSize`, `OCI_BindGetDataSizeAtPos`, `OCI_BindGetDirection`, `OCI_BindGetName`, `OCI_BindGetStatement`, `OCI_BindGetSubtype`, `OCI_BindGetType`, `OCI_StatementBindInt32`, `OCI_StatementBindInterval`, `OCI_BindIsNull`, `OCI_BindIsNullAtPos`, `OCI_StatementBindJson`, `OCI_StatementBindLob`, `OCI_StatementBindLong`, `OCI_StatementBindNumber`, `OCI_StatementBindObject`, `OCI_StatementBindRaw`, `OCI_StatementBindReference`, `OCI_BindSetCharsetForm`, `OCI_BindSetDataSize`, `OCI_BindSetDataSizeAtPos`, `OCI_BindSetDirection`, `OCI_BindSetNotNull`, `OCI_BindSetNotNullAtPos`, `OCI_BindSetNull`, `OCI_BindSetNullAtPos`, `OCI_StatementBindInt16`, `OCI_StatementBindCursor`, `OCI_StatementBindString`, `OCI_StatementBindTimestamp`, `OCI_StatementBindUInt64`, `OCI_StatementBindUInt32`, `OCI_StatementBindUInt16`, `OCI_StatementBindVector`

## `OCI_ConnectionBreak*`

`OCI_ConnectionBreak`

## `OCI_Collection* — Collection` → [collections-advanced](collections-advanced.html)

`OCI_CollectionAppend`, `OCI_CollectionArrayCreate`, `OCI_CollectionArrayFree`, `OCI_CollectionAssign`, `OCI_CollectionClear`, `OCI_CollectionCreate`, `OCI_CollectionDeleteElement`, `OCI_CollectionFree`, `OCI_CollectionGetCount`, `OCI_CollectionGetElement`, `OCI_CollectionGetMax`, `OCI_CollectionGetSize`, `OCI_CollectionGetType`, `OCI_CollectionGetTypeInfo`, `OCI_CollectionSetElement`, `OCI_CollectionToText`, `OCI_CollectionTrim`

## `OCI_Column* — Column` → [column-metadata](column-metadata.html)

`OCI_ColumnGetCharUsed`, `OCI_ColumnGetCharsetForm`, `OCI_ColumnGetCollationID`, `OCI_ColumnGetDimension`, `OCI_ColumnGetFractionalPrecision`, `OCI_ColumnGetFullSqlType`, `OCI_ColumnGetLeadingPrecision`, `OCI_ColumnGetName`, `OCI_ColumnGetNullable`, `OCI_ColumnGetPrecision`, `OCI_ColumnGetPropertyFlags`, `OCI_ColumnGetSqlType`, `OCI_ColumnGetScale`, `OCI_ColumnGetSize`, `OCI_ColumnGetSubType`, `OCI_ColumnGetType`, `OCI_ColumnGetTypeInfo`, `OCI_ColumnHasJsonSchema`

## `OCI_ConnectionCommit*`

`OCI_ConnectionCommit`

## `OCI_Connection* — Connection` → [connection-strings](connection-strings.html)

`OCI_ConnectionCreate`, `OCI_ConnectionCreateSharded`, `OCI_ConnectionFree`

## `OCI_Database* — Database` → [database-admin](database-admin.html)

`OCI_DatabaseShutdown`, `OCI_DatabaseStartup`

## `OCI_Date* — Date` → [dates](dates.html)

`OCI_DateAddDays`, `OCI_DateAddMonths`, `OCI_DateArrayCreate`, `OCI_DateArrayFree`, `OCI_DateAssign`, `OCI_DateCheck`, `OCI_DateCompare`, `OCI_DateCreate`, `OCI_DateDaysBetween`, `OCI_DateFree`, `OCI_DateFromCTime`, `OCI_DateFromText`, `OCI_DateGetDate`, `OCI_DateGetDateTime`, `OCI_DateGetTime`, `OCI_DateLastDay`, `OCI_DateNextDay`, `OCI_DateSetDate`, `OCI_DateSetDateTime`, `OCI_DateSetTime`, `OCI_DateSysDate`, `OCI_DateToCTime`, `OCI_DateToText`, `OCI_DateZoneToZone`

## `OCI_Dequeue* — Dequeue` → [aq-dequeue](aq-dequeue.html)

`OCI_DequeueCreate`, `OCI_DequeueFree`, `OCI_DequeueGet`, `OCI_DequeueGetConsumer`, `OCI_DequeueGetCorrelation`, `OCI_DequeueGetMode`, `OCI_DequeueGetNavigation`, `OCI_DequeueGetRelativeMsgID`, `OCI_DequeueGetVisibility`, `OCI_DequeueGetWaitTime`, `OCI_DequeueListen`, `OCI_DequeueSetAgentList`, `OCI_DequeueSetConsumer`, `OCI_DequeueSetCorrelation`, `OCI_DequeueSetMode`, `OCI_DequeueSetNavigation`, `OCI_DequeueSetRelativeMsgID`, `OCI_DequeueSetVisibility`, `OCI_DequeueSetWaitTime`, `OCI_DequeueSubscribe`, `OCI_DequeueUnsubscribe`

## `OCI_StatementDescribe*`

`OCI_StatementDescribe`, `OCI_StatementDescribeFormat`

## `OCI_DirectPath* — DirectPath` → [direct-path](direct-path.html)

`OCI_DirectPathAbort`, `OCI_DirectPathConvert`, `OCI_DirectPathCreate`, `OCI_DirectPathFinish`, `OCI_DirectPathFlushRow`, `OCI_DirectPathFree`, `OCI_DirectPathGetAffectedRows`, `OCI_DirectPathGetCurrentRows`, `OCI_DirectPathGetErrorColumn`, `OCI_DirectPathGetErrorRow`, `OCI_DirectPathGetMaxRows`, `OCI_DirectPathGetRowCount`, `OCI_DirectPathLoad`, `OCI_DirectPathPrepare`, `OCI_DirectPathReset`, `OCI_DirectPathSave`, `OCI_DirectPathSetBufferSize`, `OCI_DirectPathSetCacheSize`, `OCI_DirectPathSetColumn`, `OCI_DirectPathSetConvertMode`, `OCI_DirectPathSetCurrentRows`, `OCI_DirectPathSetDateFormat`, `OCI_DirectPathSetEntry`, `OCI_DirectPathSetNoLog`, `OCI_DirectPathSetParallel`

## `OCI_Element* — Element` → [collections-advanced](collections-advanced.html)

`OCI_ElementCreate`, `OCI_ElementFree`, `OCI_ElementGetInt64`, `OCI_ElementGetBoolean`, `OCI_ElementGetCollection`, `OCI_ElementGetDate`, `OCI_ElementGetDouble`, `OCI_ElementGetFile`, `OCI_ElementGetFloat`, `OCI_ElementGetInt32`, `OCI_ElementGetInterval`, `OCI_ElementGetLob`, `OCI_ElementGetNumber`, `OCI_ElementGetObject`, `OCI_ElementGetRaw`, `OCI_ElementGetRawSize`, `OCI_ElementGetReference`, `OCI_ElementGetInt16`, `OCI_ElementGetString`, `OCI_ElementGetTimestamp`, `OCI_ElementGetUInt64`, `OCI_ElementGetUInt32`, `OCI_ElementGetUInt16`, `OCI_ElementIsNull`, `OCI_ElementSetInt64`, `OCI_ElementSetBoolean`, `OCI_ElementSetCollection`, `OCI_ElementSetDate`, `OCI_ElementSetDouble`, `OCI_ElementSetFile`, `OCI_ElementSetFloat`, `OCI_ElementSetInt32`, `OCI_ElementSetInterval`, `OCI_ElementSetLob`, `OCI_ElementSetNull`, `OCI_ElementSetNumber`, `OCI_ElementSetObject`, `OCI_ElementSetRaw`, `OCI_ElementSetReference`, `OCI_ElementSetInt16`, `OCI_ElementSetString`, `OCI_ElementSetTimestamp`, `OCI_ElementSetUInt64`, `OCI_ElementSetUInt32`, `OCI_ElementSetUInt16`

## `OCI_EnvironmentEnable*`

`OCI_EnvironmentEnableWarnings`

## `OCI_Enqueue* — Enqueue` → [aq-enqueue](aq-enqueue.html)

`OCI_EnqueueCreate`, `OCI_EnqueueFree`, `OCI_EnqueueGetRelativeMsgID`, `OCI_EnqueueGetSequenceDeviation`, `OCI_EnqueueGetVisibility`, `OCI_EnqueuePut`, `OCI_EnqueueSetRelativeMsgID`, `OCI_EnqueueSetSequenceDeviation`, `OCI_EnqueueSetVisibility`

## `OCI_Environment* — Environment` → [environments](environments.html)

`OCI_EnvironmentCreate`, `OCI_EnvironmentFree`

## `OCI_Error* — Error` → [error-handling](error-handling.html)

`OCI_ErrorGetCategory`, `OCI_ErrorGetCode`, `OCI_ErrorGetConnection`, `OCI_ErrorGetLocation`, `OCI_ErrorGetRow`, `OCI_ErrorGetStatement`, `OCI_ErrorGetString`, `OCI_ErrorGetType`

## `OCI_Event* — Event` → [notifications](notifications.html)

`OCI_EventGetDatabase`, `OCI_EventGetObject`, `OCI_EventGetOperation`, `OCI_EventGetRowid`, `OCI_EventGetSubscription`, `OCI_EventGetType`

## `OCI_StatementExecute*`

`OCI_StatementExecutePrepared`, `OCI_StatementExecutePreparedAndGetResultset`, `OCI_StatementExecute`, `OCI_StatementExecuteAndGetResultset`, `OCI_StatementExecuteFormat`

## `OCI_ResultsetFetch*`

`OCI_ResultsetFetchFirst`, `OCI_ResultsetFetchLast`, `OCI_ResultsetFetchNext`, `OCI_ResultsetFetchPrev`, `OCI_ResultsetFetchSeek`

## `OCI_File* — File` → [bfiles](bfiles.html)

`OCI_FileArrayCreate`, `OCI_FileArrayFree`, `OCI_FileAssign`, `OCI_FileClose`, `OCI_FileCreate`, `OCI_FileExists`, `OCI_FileFree`, `OCI_FileGetConnection`, `OCI_FileGetDirectory`, `OCI_FileGetName`, `OCI_FileGetOffset`, `OCI_FileGetSize`, `OCI_FileGetType`, `OCI_FileIsEqual`, `OCI_FileIsOpen`, `OCI_FileOpen`, `OCI_FileRead`, `OCI_FileSeek`, `OCI_FileSetName`

## `OCI_*Get*`

`OCI_StatementGetAffectedRows`, `OCI_ConnectionGetAutoCommit`, `OCI_StatementGetBatchError`, `OCI_StatementGetBatchErrorCount`, `OCI_ResultsetGetInt64`, `OCI_ResultsetGetInt64ByName`, `OCI_StatementGetBind`, `OCI_StatementGetBindAllocation`, `OCI_StatementGetBindByName`, `OCI_StatementGetBindCount`, `OCI_StatementGetBindIndex`, `OCI_StatementGetBindMode`, `OCI_EnvironmentGetCharset`, `OCI_ResultsetGetColl`, `OCI_ResultsetGetCollByName`, `OCI_ResultsetGetColumn`, `OCI_ResultsetGetColumnByName`, `OCI_ResultsetGetColumnCount`, `OCI_ResultsetGetColumnIndex`, `OCI_ResultsetGetCurrentRow`, `OCI_ConnectionGetDatabaseName`, `OCI_ResultsetGetDataLength`, `OCI_ResultsetGetDataSize`, `OCI_ResultsetGetDataSizeByName`, `OCI_ConnectionGetConnectionString`, `OCI_ResultsetGetDate`, `OCI_ResultsetGetDateByName`, `OCI_ConnectionGetDefaultLobPrefetchSize`, `OCI_ConnectionGetDomainName`, `OCI_ResultsetGetDouble`, `OCI_ResultsetGetDoubleByName`, `OCI_StatementGetFetchMode`, `OCI_StatementGetFetchSize`, `OCI_ResultsetGetFile`, `OCI_ResultsetGetFileByName`, `OCI_ResultsetGetFloat`, `OCI_ResultsetGetFloatByName`, `OCI_EnvironmentGetFormat`, `OCI_EnvironmentGetImportMode`, `OCI_ConnectionGetInstanceName`, `OCI_ConnectionGetInstanceStartTime`, `OCI_ResultsetGetInt32`, `OCI_ResultsetGetInt32ByName`, `OCI_ResultsetGetInterval`, `OCI_ResultsetGetIntervalByName`, `OCI_ResultsetGetJson`, `OCI_ResultsetGetJsonByName`, `OCI_GetLastError`, `OCI_ResultsetGetLob`, `OCI_ResultsetGetLobByName`, `OCI_EnvironmentGetLocaleString`, `OCI_ResultsetGetLong`, `OCI_ResultsetGetLongByName`, `OCI_StatementGetLongMode`, `OCI_ConnectionGetMaxCursors`, `OCI_StatementGetNextResultset`, `OCI_ResultsetGetNumber`, `OCI_ResultsetGetNumberByName`, `OCI_EnvironmentGetOCICompileVersion`, `OCI_EnvironmentGetOCIRuntimeVersion`, `OCI_ResultsetGetObject`, `OCI_ResultsetGetObjectByName`, `OCI_StatementGetParseBindNames`, `OCI_ConnectionGetPassword`, `OCI_StatementGetPieceSize`, `OCI_StatementGetPrefetchMemory`, `OCI_StatementGetPrefetchSize`, `OCI_ResultsetGetRaw`, `OCI_ResultsetGetRawByName`, `OCI_ResultsetGetReference`, `OCI_ResultsetGetReferenceByName`, `OCI_StatementGetResultset`, `OCI_ResultsetGetRowCount`, `OCI_StatementGetSqlCommand`, `OCI_StatementGetSqlVerb`, `OCI_ConnectionGetServerMajorVersion`, `OCI_ConnectionGetServerMinorVersion`, `OCI_ConnectionGetServerName`, `OCI_ConnectionGetServerRevisionVersion`, `OCI_ConnectionGetServiceName`, `OCI_ConnectionGetSessionMode`, `OCI_ConnectionGetSessionTag`, `OCI_ResultsetGetInt16`, `OCI_ResultsetGetInt16ByName`, `OCI_StatementGetSql`, `OCI_StatementGetSqlErrorPos`, `OCI_StatementGetSqlIdentifier`, `OCI_ResultsetGetCursor`, `OCI_ResultsetGetCursorByName`, `OCI_ConnectionGetStatementCacheSize`, `OCI_StatementGetType`, `OCI_EnvironmentGetStats`, `OCI_ResultsetGetString`, `OCI_ResultsetGetStringByName`, `OCI_ResultsetGetStruct`, `OCI_EnvironmentGetSubscriptionPort`, `OCI_ConnectionGetTimeout`, `OCI_ResultsetGetTimestamp`, `OCI_ResultsetGetTimestampByName`, `OCI_ConnectionGetTrace`, `OCI_ConnectionGetTransaction`, `OCI_ResultsetGetUInt64`, `OCI_ResultsetGetUInt64ByName`, `OCI_ResultsetGetUInt32`, `OCI_ResultsetGetUInt32ByName`, `OCI_ResultsetGetUInt16`, `OCI_ResultsetGetUInt16ByName`, `OCI_HandleGetUserData`, `OCI_ConnectionGetUserName`, `OCI_ResultsetGetVector`, `OCI_ResultsetGetVectorByName`, `OCI_ConnectionGetVersion`, `OCI_ConnectionGetServerVersionString`, `OCI_ResultsetGetXmlType`, `OCI_ResultsetGetXmlTypeByName`

## `OCI_Handle*`

`OCI_HandleGetEnvironment`

## `OCI_HashTable* — HashTable` → [hash-tables](hash-tables.html)

`OCI_HashTableAddInt32`, `OCI_HashTableAddPointer`, `OCI_HashTableAddString`, `OCI_HashTableCreate`, `OCI_HashTableFree`, `OCI_HashTableGetEntry`, `OCI_HashTableGetInt32`, `OCI_HashTableGetPointer`, `OCI_HashTableGetSize`, `OCI_HashTableGetString`, `OCI_HashTableGetType`, `OCI_HashTableGetValue`, `OCI_HashTableLookup`

## `OCI_ConnectionImmediate* — Immediate` → [statement-lifecycle](statement-lifecycle.html)

`OCI_ConnectionImmediate`, `OCI_ConnectionImmediateFormat`

## `OCI_Interval* — Interval` → [intervals](intervals.html)

`OCI_IntervalAdd`, `OCI_IntervalArrayCreate`, `OCI_IntervalArrayFree`, `OCI_IntervalAssign`, `OCI_IntervalCheck`, `OCI_IntervalCompare`, `OCI_IntervalCreate`, `OCI_IntervalFree`, `OCI_IntervalFromText`, `OCI_IntervalFromTimeZone`, `OCI_IntervalGetDaySecond`, `OCI_IntervalGetType`, `OCI_IntervalGetYearMonth`, `OCI_IntervalSetDaySecond`, `OCI_IntervalSetYearMonth`, `OCI_IntervalSubtract`, `OCI_IntervalToText`

## `OCI_*Is*`

`OCI_ConnectionIsAlive`, `OCI_ResultsetIsColumnNull`, `OCI_ResultsetIsColumnNullByName`, `OCI_StatementIsRebindingAllowed`, `OCI_ConnectionIsTAFCapable`

## `OCI_Iterator* — Iterator` → [collections-advanced](collections-advanced.html)

`OCI_IteratorCreate`, `OCI_IteratorFree`, `OCI_IteratorGetCurrent`, `OCI_IteratorGetNext`, `OCI_IteratorGetPrev`

## `OCI_Json* — Json` → [json-columns](json-columns.html)

`OCI_JsonArrayCreate`, `OCI_JsonArrayFree`, `OCI_JsonClone`, `OCI_JsonCreate`, `OCI_JsonFree`, `OCI_JsonGetConnection`, `OCI_JsonGetRoot`, `OCI_JsonIsMutable`, `OCI_JsonLoadBinary`, `OCI_JsonLoadText`, `OCI_JsonSetMutable`, `OCI_JsonSetRoot`, `OCI_JsonToBinary`, `OCI_JsonToText`, `OCI_JsonValidate`

## `OCI_JsonNode* — JsonNode` → [json-dom](json-dom.html)

`OCI_JsonNodeAppendItem`, `OCI_JsonNodeAssign`, `OCI_JsonNodeClear`, `OCI_JsonNodeCreate`, `OCI_JsonNodeCreateArray`, `OCI_JsonNodeCreateBinary`, `OCI_JsonNodeCreateBoolean`, `OCI_JsonNodeCreateDate`, `OCI_JsonNodeCreateInterval`, `OCI_JsonNodeCreateNull`, `OCI_JsonNodeCreateNumber`, `OCI_JsonNodeCreateObject`, `OCI_JsonNodeCreateString`, `OCI_JsonNodeCreateTimestamp`, `OCI_JsonNodeEquals`, `OCI_JsonNodeFree`, `OCI_JsonNodeGetInt64`, `OCI_JsonNodeGetUInt64`, `OCI_JsonNodeGetBinary`, `OCI_JsonNodeGetBoolean`, `OCI_JsonNodeGetByPath`, `OCI_JsonNodeGetDate`, `OCI_JsonNodeGetDouble`, `OCI_JsonNodeGetFieldByIndex`, `OCI_JsonNodeGetFieldByName`, `OCI_JsonNodeGetFieldCount`, `OCI_JsonNodeGetFieldName`, `OCI_JsonNodeGetFloat`, `OCI_JsonNodeGetInt32`, `OCI_JsonNodeGetInterval`, `OCI_JsonNodeGetItem`, `OCI_JsonNodeGetItemCount`, `OCI_JsonNodeGetJson`, `OCI_JsonNodeGetNumber`, `OCI_JsonNodeGetParent`, `OCI_JsonNodeGetInt16`, `OCI_JsonNodeGetString`, `OCI_JsonNodeGetTimestamp`, `OCI_JsonNodeGetType`, `OCI_JsonNodeGetUInt32`, `OCI_JsonNodeGetUInt16`, `OCI_JsonNodeHasField`, `OCI_JsonNodeInsertItem`, `OCI_JsonNodeIsNull`, `OCI_JsonNodeRemoveField`, `OCI_JsonNodeRemoveItem`, `OCI_JsonNodeRemoveItems`, `OCI_JsonNodeRenameField`, `OCI_JsonNodeSetInt64`, `OCI_JsonNodeSetUInt64`, `OCI_JsonNodeSetBinary`, `OCI_JsonNodeSetBoolean`, `OCI_JsonNodeSetDate`, `OCI_JsonNodeSetDouble`, `OCI_JsonNodeSetField`, `OCI_JsonNodeSetFloat`, `OCI_JsonNodeSetInt32`, `OCI_JsonNodeSetInterval`, `OCI_JsonNodeSetItem`, `OCI_JsonNodeSetNull`, `OCI_JsonNodeSetNumber`, `OCI_JsonNodeSetInt16`, `OCI_JsonNodeSetString`, `OCI_JsonNodeSetTimestamp`, `OCI_JsonNodeSetUInt32`, `OCI_JsonNodeSetUInt16`

## `OCI_Lob* — Lob` → [lobs-basics](lobs-basics.html)

`OCI_LobAppend`, `OCI_LobAppendLob`, `OCI_LobArrayCreate`, `OCI_LobArrayFree`, `OCI_LobAssign`, `OCI_LobClose`, `OCI_LobCopy`, `OCI_LobCopyFromFile`, `OCI_LobCreate`, `OCI_LobEnableBuffering`, `OCI_LobErase`, `OCI_LobFlush`, `OCI_LobFree`, `OCI_LobGetChunkSize`, `OCI_LobGetConnection`, `OCI_LobGetLength`, `OCI_LobGetMaxSize`, `OCI_LobGetOffset`, `OCI_LobGetType`, `OCI_LobIsEqual`, `OCI_LobIsRemote`, `OCI_LobIsTemporary`, `OCI_LobOpen`, `OCI_LobRead`, `OCI_LobSeek`, `OCI_LobTruncate`, `OCI_LobWrite`

## `OCI_Long* — Long` → [longs](longs.html)

`OCI_LongCreate`, `OCI_LongFree`, `OCI_LongGetBuffer`, `OCI_LongGetSize`, `OCI_LongGetType`, `OCI_LongRead`, `OCI_LongWrite`

## `OCI_Message* — Message` → [aq-enqueue](aq-enqueue.html)

`OCI_MessageCreate`, `OCI_MessageFree`, `OCI_MessageGetAttemptCount`, `OCI_MessageGetCorrelation`, `OCI_MessageGetEnqueueDelay`, `OCI_MessageGetEnqueueTime`, `OCI_MessageGetExceptionQueue`, `OCI_MessageGetExpiration`, `OCI_MessageGetID`, `OCI_MessageGetObject`, `OCI_MessageGetOriginalID`, `OCI_MessageGetPriority`, `OCI_MessageGetRaw`, `OCI_MessageGetSender`, `OCI_MessageGetState`, `OCI_MessageReset`, `OCI_MessageSetConsumers`, `OCI_MessageSetCorrelation`, `OCI_MessageSetEnqueueDelay`, `OCI_MessageSetExceptionQueue`, `OCI_MessageSetExpiration`, `OCI_MessageSetObject`, `OCI_MessageSetOriginalID`, `OCI_MessageSetPriority`, `OCI_MessageSetRaw`, `OCI_MessageSetSender`

## `OCI_Number* — Number` → [numbers](numbers.html)

`OCI_NumberAdd`, `OCI_NumberArrayCreate`, `OCI_NumberArrayFree`, `OCI_NumberAssign`, `OCI_NumberCompare`, `OCI_NumberCreate`, `OCI_NumberDivide`, `OCI_NumberFree`, `OCI_NumberFromText`, `OCI_NumberGetContent`, `OCI_NumberGetValue`, `OCI_NumberMultiply`, `OCI_NumberSetContent`, `OCI_NumberSetValue`, `OCI_NumberSub`, `OCI_NumberToText`

## `OCI_Object* — Object` → [objects-attrs-complete](objects-attrs-complete.html)

`OCI_ObjectArrayCreate`, `OCI_ObjectArrayFree`, `OCI_ObjectAssign`, `OCI_ObjectCreate`, `OCI_ObjectFree`, `OCI_ObjectGetInt64`, `OCI_ObjectGetBoolean`, `OCI_ObjectGetColl`, `OCI_ObjectGetDate`, `OCI_ObjectGetDouble`, `OCI_ObjectGetFile`, `OCI_ObjectGetFloat`, `OCI_ObjectGetInt32`, `OCI_ObjectGetInterval`, `OCI_ObjectGetLob`, `OCI_ObjectGetNumber`, `OCI_ObjectGetObject`, `OCI_ObjectGetRaw`, `OCI_ObjectGetRawSize`, `OCI_ObjectGetReference`, `OCI_ObjectGetSelfReference`, `OCI_ObjectGetInt16`, `OCI_ObjectGetString`, `OCI_ObjectGetStruct`, `OCI_ObjectGetTimestamp`, `OCI_ObjectGetType`, `OCI_ObjectGetTypeInfo`, `OCI_ObjectGetUInt64`, `OCI_ObjectGetUInt32`, `OCI_ObjectGetUInt16`, `OCI_ObjectIsNull`, `OCI_ObjectSetInt64`, `OCI_ObjectSetBoolean`, `OCI_ObjectSetColl`, `OCI_ObjectSetDate`, `OCI_ObjectSetDouble`, `OCI_ObjectSetFile`, `OCI_ObjectSetFloat`, `OCI_ObjectSetInt32`, `OCI_ObjectSetInterval`, `OCI_ObjectSetLob`, `OCI_ObjectSetNull`, `OCI_ObjectSetNumber`, `OCI_ObjectSetObject`, `OCI_ObjectSetRaw`, `OCI_ObjectSetReference`, `OCI_ObjectSetInt16`, `OCI_ObjectSetString`, `OCI_ObjectSetTimestamp`, `OCI_ObjectSetUInt64`, `OCI_ObjectSetUInt32`, `OCI_ObjectSetUInt16`, `OCI_ObjectToText`

## `OCI_StatementParse*`

`OCI_StatementParse`, `OCI_StatementParseFormat`

## `OCI_ConnectionPing*`

`OCI_ConnectionPing`

## `OCI_Pool* — Pool` → [pooling-basics](pooling-basics.html)

`OCI_PoolCreate`, `OCI_PoolFree`, `OCI_PoolGetBusyCount`, `OCI_PoolGetConnection`, `OCI_PoolGetConnectionSharded`, `OCI_PoolGetIncrement`, `OCI_PoolGetMax`, `OCI_PoolGetMin`, `OCI_PoolGetNoWait`, `OCI_PoolGetOpenedCount`, `OCI_PoolGetStatementCacheSize`, `OCI_PoolGetTimeout`, `OCI_PoolSetNoWait`, `OCI_PoolSetStatementCacheSize`, `OCI_PoolSetTimeout`

## `OCI_StatementPrepare*`

`OCI_StatementPrepare`, `OCI_StatementPrepareFormat`

## `OCI_Queue* — Queue` → [aq-setup](aq-setup.html)

`OCI_QueueAlter`, `OCI_QueueCreate`, `OCI_QueueDrop`, `OCI_QueueStart`, `OCI_QueueStop`

## `OCI_QueueTable* — QueueTable` → [aq-setup](aq-setup.html)

`OCI_QueueTableAlter`, `OCI_QueueTableCreate`, `OCI_QueueTableDrop`, `OCI_QueueTableMigrate`, `OCI_QueueTablePurge`

## `OCI_Reference* — Reference` → [refs](refs.html)

`OCI_ReferenceArrayCreate`, `OCI_ReferenceArrayFree`, `OCI_ReferenceAssign`, `OCI_ReferenceCreate`, `OCI_ReferenceFree`, `OCI_ReferenceGetHexSize`, `OCI_ReferenceGetObject`, `OCI_ReferenceGetTypeInfo`, `OCI_ReferenceIsNull`, `OCI_ReferenceSetNull`, `OCI_ReferenceToText`

## `OCI_StatementRegister* — Register` → [rowid-returning](rowid-returning.html)

`OCI_StatementRegisterInt64`, `OCI_StatementRegisterDate`, `OCI_StatementRegisterDouble`, `OCI_StatementRegisterFile`, `OCI_StatementRegisterFloat`, `OCI_StatementRegisterInt32`, `OCI_StatementRegisterInterval`, `OCI_StatementRegisterJson`, `OCI_StatementRegisterLob`, `OCI_StatementRegisterNumber`, `OCI_StatementRegisterObject`, `OCI_StatementRegisterRaw`, `OCI_StatementRegisterReference`, `OCI_StatementRegisterInt16`, `OCI_StatementRegisterString`, `OCI_StatementRegisterTimestamp`, `OCI_StatementRegisterUInt64`, `OCI_StatementRegisterUInt32`, `OCI_StatementRegisterUInt16`, `OCI_StatementRegisterVector`

## `OCI_StatementRelease*`

`OCI_StatementReleaseResultsets`

## `OCI_Resultset* — Resultset` → [fetch-getters](fetch-getters.html)

`OCI_ResultsetGetStatement`

## `OCI_ConnectionRollback*`

`OCI_ConnectionRollback`

## `OCI_Connection*ServerOutput — Server output` → [server-output](server-output.html)

`OCI_ConnectionDisableServerOutput`, `OCI_ConnectionEnableServerOutput`, `OCI_ConnectionGetServerOutput`

## `OCI_*Set*`

`OCI_ConnectionSetAutoCommit`, `OCI_StatementSetBindAllocation`, `OCI_StatementSetBindMode`, `OCI_ConnectionSetDefaultLobPrefetchSize`, `OCI_EnvironmentSetErrorHandler`, `OCI_StatementSetFetchMode`, `OCI_StatementSetFetchSize`, `OCI_EnvironmentSetFormat`, `OCI_EnvironmentSetHAHandler`, `OCI_StatementSetLongMode`, `OCI_ConnectionSetPassword`, `OCI_StatementSetPieceSize`, `OCI_StatementSetPrefetchMemory`, `OCI_StatementSetPrefetchSize`, `OCI_ConnectionSetSessionTag`, `OCI_ConnectionSetStatementCacheSize`, `OCI_ResultsetSetStructNumericType`, `OCI_ResultsetSetStructNumericTypeByName`, `OCI_EnvironmentSetSubscriptionPort`, `OCI_ConnectionSetTAFHandler`, `OCI_ConnectionSetTimeout`, `OCI_ConnectionSetTrace`, `OCI_ConnectionSetTransaction`, `OCI_HandleSetUserData`, `OCI_EnvironmentSetUserPassword`

## `OCI_ShardingKey* — ShardingKey` → [sharding](sharding.html)

`OCI_ShardingKeyAddDate`, `OCI_ShardingKeyAddNumber`, `OCI_ShardingKeyAddRaw`, `OCI_ShardingKeyAddString`, `OCI_ShardingKeyAddTimestamp`, `OCI_ShardingKeyCreate`, `OCI_ShardingKeyFree`, `OCI_ShardingKeyReset`, `OCI_ShardingKeyToText`

## `OCI_SodaCollection* — SodaCollection` → [soda-basics](soda-basics.html)

`OCI_SodaCollectionCreateIndex`, `OCI_SodaCollectionDocumentCount`, `OCI_SodaCollectionDocumentCountWithFilter`, `OCI_SodaCollectionDrop`, `OCI_SodaCollectionDropIndex`, `OCI_SodaCollectionFind`, `OCI_SodaCollectionFindOne`, `OCI_SodaCollectionFindOneByKey`, `OCI_SodaCollectionFree`, `OCI_SodaCollectionFreeIndexNames`, `OCI_SodaCollectionGetConnection`, `OCI_SodaCollectionGetDataGuide`, `OCI_SodaCollectionGetMetadata`, `OCI_SodaCollectionGetName`, `OCI_SodaCollectionInsertDocument`, `OCI_SodaCollectionInsertMany`, `OCI_SodaCollectionIsMapped`, `OCI_SodaCollectionListIndexes`, `OCI_SodaCollectionRemove`, `OCI_SodaCollectionRemoveOneByKey`, `OCI_SodaCollectionReplaceOne`, `OCI_SodaCollectionReplaceOneByKey`, `OCI_SodaCollectionSave`, `OCI_SodaCollectionTruncate`

## `OCI_SodaCollectionCursor* — SodaCollectionCursor` → [soda-advanced](soda-advanced.html)

`OCI_SodaCollectionCursorFree`, `OCI_SodaCollectionCursorGetNext`

## `OCI_SodaDatabase* — SodaDatabase` → [soda-basics](soda-basics.html)

`OCI_SodaDatabaseCreate`, `OCI_SodaDatabaseCreateCollection`, `OCI_SodaDatabaseFree`, `OCI_SodaDatabaseFreeCollectionNames`, `OCI_SodaDatabaseGetCollectionNames`, `OCI_SodaDatabaseGetCollections`, `OCI_SodaDatabaseGetConnection`, `OCI_SodaDatabaseOpenCollection`

## `OCI_SodaDocument* — SodaDocument` → [soda-basics](soda-basics.html)

`OCI_SodaDocumentCreateFromJson`, `OCI_SodaDocumentCreateFromText`, `OCI_SodaDocumentFree`, `OCI_SodaDocumentGetCreatedOn`, `OCI_SodaDocumentGetIsJson`, `OCI_SodaDocumentGetJson`, `OCI_SodaDocumentGetKey`, `OCI_SodaDocumentGetLastModified`, `OCI_SodaDocumentGetMediaType`, `OCI_SodaDocumentGetText`, `OCI_SodaDocumentGetVersion`

## `OCI_SodaDocumentCursor* — SodaDocumentCursor` → [soda-basics](soda-basics.html)

`OCI_SodaDocumentCursorFree`, `OCI_SodaDocumentCursorGetNext`

## `OCI_SodaOptions* — SodaOptions` → [soda-advanced](soda-advanced.html)

`OCI_SodaOptionsCreate`, `OCI_SodaOptionsFree`, `OCI_SodaOptionsSetFetchArraySize`, `OCI_SodaOptionsSetFilter`, `OCI_SodaOptionsSetHint`, `OCI_SodaOptionsSetKey`, `OCI_SodaOptionsSetKeys`, `OCI_SodaOptionsSetLimit`, `OCI_SodaOptionsSetLock`, `OCI_SodaOptionsSetSkip`, `OCI_SodaOptionsSetVersion`

## `OCI_Statement* — Statement` → [statement-lifecycle](statement-lifecycle.html)

`OCI_StatementCreate`, `OCI_StatementFree`, `OCI_StatementGetConnection`

## `OCI_Subscription* — Subscription` → [notifications](notifications.html)

`OCI_SubscriptionAddStatement`, `OCI_SubscriptionGetConnection`, `OCI_SubscriptionGetName`, `OCI_SubscriptionGetTimeout`, `OCI_SubscriptionCreate`, `OCI_SubscriptionUnregister`

## `OCI_Timestamp* — Timestamp` → [timestamps](timestamps.html)

`OCI_TimestampArrayCreate`, `OCI_TimestampArrayFree`, `OCI_TimestampAssign`, `OCI_TimestampCheck`, `OCI_TimestampCompare`, `OCI_TimestampConstruct`, `OCI_TimestampConvert`, `OCI_TimestampCreate`, `OCI_TimestampFree`, `OCI_TimestampFromCTime`, `OCI_TimestampFromText`, `OCI_TimestampGetDate`, `OCI_TimestampGetDateTime`, `OCI_TimestampGetTime`, `OCI_TimestampGetTimeZoneName`, `OCI_TimestampGetTimeZoneOffset`, `OCI_TimestampGetType`, `OCI_TimestampIntervalAdd`, `OCI_TimestampIntervalSub`, `OCI_TimestampSubtract`, `OCI_TimestampSysTimestamp`, `OCI_TimestampToCTime`, `OCI_TimestampToText`

## `OCI_Transaction* — Transaction` → [transaction-objects](transaction-objects.html)

`OCI_TransactionCreate`, `OCI_TransactionForget`, `OCI_TransactionFree`, `OCI_TransactionGetMode`, `OCI_TransactionGetTimeout`, `OCI_TransactionPrepare`, `OCI_TransactionResume`, `OCI_TransactionStart`, `OCI_TransactionStop`

## `OCI_TypeInfo* — TypeInfo` → [describe-schema](describe-schema.html)

`OCI_TypeInfoFree`, `OCI_TypeInfoGet`, `OCI_TypeInfoGetColumn`, `OCI_TypeInfoGetColumnCount`, `OCI_TypeInfoGetConnection`, `OCI_TypeInfoGetName`, `OCI_TypeInfoGetSuperType`, `OCI_TypeInfoGetType`, `OCI_TypeInfoIsFinalType`

## `OCI_Vector* — Vector` → [vectors](vectors.html)

`OCI_VectorArrayCreate`, `OCI_VectorArrayFree`, `OCI_VectorCreate`, `OCI_VectorFree`, `OCI_VectorFromText`, `OCI_VectorGetInfo`, `OCI_VectorGetValues`, `OCI_VectorSetValues`, `OCI_VectorToText`

## `OCI_XmlType* — XmlType` → [xmltype](xmltype.html)

`OCI_XmlTypeGetContent`, `OCI_XmlTypeGetContentSize`
