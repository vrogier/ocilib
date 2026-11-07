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

## `OCI_Allow*`

`OCI_AllowRebinding`

## `OCI_Bind* — Bind` → [bind-management](bind-management.html)

`OCI_BindArrayGetSize`, `OCI_BindArrayOfBigInts`, `OCI_BindArrayOfColls`, `OCI_BindArrayOfDates`, `OCI_BindArrayOfDoubles`, `OCI_BindArrayOfFiles`, `OCI_BindArrayOfFloats`, `OCI_BindArrayOfIntervals`, `OCI_BindArrayOfInts`, `OCI_BindArrayOfJsons`, `OCI_BindArrayOfLobs`, `OCI_BindArrayOfNumbers`, `OCI_BindArrayOfObjects`, `OCI_BindArrayOfRaws`, `OCI_BindArrayOfRefs`, `OCI_BindArrayOfShorts`, `OCI_BindArrayOfStrings`, `OCI_BindArrayOfTimestamps`, `OCI_BindArrayOfUnsignedBigInts`, `OCI_BindArrayOfUnsignedInts`, `OCI_BindArrayOfUnsignedShorts`, `OCI_BindArrayOfVectors`, `OCI_BindArraySetSize`, `OCI_BindBigInt`, `OCI_BindBoolean`, `OCI_BindColl`, `OCI_BindDate`, `OCI_BindDouble`, `OCI_BindFile`, `OCI_BindFloat`, `OCI_BindGetAllocationMode`, `OCI_BindGetData`, `OCI_BindGetDataCount`, `OCI_BindGetDataSize`, `OCI_BindGetDataSizeAtPos`, `OCI_BindGetDirection`, `OCI_BindGetName`, `OCI_BindGetStatement`, `OCI_BindGetSubtype`, `OCI_BindGetType`, `OCI_BindInt`, `OCI_BindInterval`, `OCI_BindIsNull`, `OCI_BindIsNullAtPos`, `OCI_BindJson`, `OCI_BindLob`, `OCI_BindLong`, `OCI_BindNumber`, `OCI_BindObject`, `OCI_BindRaw`, `OCI_BindRef`, `OCI_BindSetCharsetForm`, `OCI_BindSetDataSize`, `OCI_BindSetDataSizeAtPos`, `OCI_BindSetDirection`, `OCI_BindSetNotNull`, `OCI_BindSetNotNullAtPos`, `OCI_BindSetNull`, `OCI_BindSetNullAtPos`, `OCI_BindShort`, `OCI_BindStatement`, `OCI_BindString`, `OCI_BindTimestamp`, `OCI_BindUnsignedBigInt`, `OCI_BindUnsignedInt`, `OCI_BindUnsignedShort`, `OCI_BindVector`

## `OCI_Break*`

`OCI_Break`

## `OCI_Coll* — Coll` → [collections-advanced](collections-advanced.html)

`OCI_CollAppend`, `OCI_CollArrayCreate`, `OCI_CollArrayFree`, `OCI_CollAssign`, `OCI_CollClear`, `OCI_CollCreate`, `OCI_CollDeleteElem`, `OCI_CollFree`, `OCI_CollGetCount`, `OCI_CollGetElem`, `OCI_CollGetMax`, `OCI_CollGetSize`, `OCI_CollGetType`, `OCI_CollGetTypeInfo`, `OCI_CollSetElem`, `OCI_CollToText`, `OCI_CollTrim`

## `OCI_Column* — Column` → [column-metadata](column-metadata.html)

`OCI_ColumnGetCharUsed`, `OCI_ColumnGetCharsetForm`, `OCI_ColumnGetCollationID`, `OCI_ColumnGetDimension`, `OCI_ColumnGetFractionalPrecision`, `OCI_ColumnGetFullSQLType`, `OCI_ColumnGetLeadingPrecision`, `OCI_ColumnGetName`, `OCI_ColumnGetNullable`, `OCI_ColumnGetPrecision`, `OCI_ColumnGetPropertyFlags`, `OCI_ColumnGetSQLType`, `OCI_ColumnGetScale`, `OCI_ColumnGetSize`, `OCI_ColumnGetSubType`, `OCI_ColumnGetType`, `OCI_ColumnGetTypeInfo`, `OCI_ColumnHasJsonSchema`

## `OCI_Commit*`

`OCI_Commit`

## `OCI_Connection* — Connection` → [connection-strings](connection-strings.html)

`OCI_ConnectionCreate`, `OCI_ConnectionCreateSharded`, `OCI_ConnectionFree`

## `OCI_Database* — Database` → [database-admin](database-admin.html)

`OCI_DatabaseShutdown`, `OCI_DatabaseStartup`

## `OCI_Date* — Date` → [dates](dates.html)

`OCI_DateAddDays`, `OCI_DateAddMonths`, `OCI_DateArrayCreate`, `OCI_DateArrayFree`, `OCI_DateAssign`, `OCI_DateCheck`, `OCI_DateCompare`, `OCI_DateCreate`, `OCI_DateDaysBetween`, `OCI_DateFree`, `OCI_DateFromCTime`, `OCI_DateFromText`, `OCI_DateGetDate`, `OCI_DateGetDateTime`, `OCI_DateGetTime`, `OCI_DateLastDay`, `OCI_DateNextDay`, `OCI_DateSetDate`, `OCI_DateSetDateTime`, `OCI_DateSetTime`, `OCI_DateSysDate`, `OCI_DateToCTime`, `OCI_DateToText`, `OCI_DateZoneToZone`

## `OCI_Dequeue* — Dequeue` → [aq-dequeue](aq-dequeue.html)

`OCI_DequeueCreate`, `OCI_DequeueFree`, `OCI_DequeueGet`, `OCI_DequeueGetConsumer`, `OCI_DequeueGetCorrelation`, `OCI_DequeueGetMode`, `OCI_DequeueGetNavigation`, `OCI_DequeueGetRelativeMsgID`, `OCI_DequeueGetVisibility`, `OCI_DequeueGetWaitTime`, `OCI_DequeueListen`, `OCI_DequeueSetAgentList`, `OCI_DequeueSetConsumer`, `OCI_DequeueSetCorrelation`, `OCI_DequeueSetMode`, `OCI_DequeueSetNavigation`, `OCI_DequeueSetRelativeMsgID`, `OCI_DequeueSetVisibility`, `OCI_DequeueSetWaitTime`, `OCI_DequeueSubscribe`, `OCI_DequeueUnsubscribe`

## `OCI_Describe*`

`OCI_Describe`, `OCI_DescribeFmt`

## `OCI_DirPath* — DirPath` → [direct-path](direct-path.html)

`OCI_DirPathAbort`, `OCI_DirPathConvert`, `OCI_DirPathCreate`, `OCI_DirPathFinish`, `OCI_DirPathFlushRow`, `OCI_DirPathFree`, `OCI_DirPathGetAffectedRows`, `OCI_DirPathGetCurrentRows`, `OCI_DirPathGetErrorColumn`, `OCI_DirPathGetErrorRow`, `OCI_DirPathGetMaxRows`, `OCI_DirPathGetRowCount`, `OCI_DirPathLoad`, `OCI_DirPathPrepare`, `OCI_DirPathReset`, `OCI_DirPathSave`, `OCI_DirPathSetBufferSize`, `OCI_DirPathSetCacheSize`, `OCI_DirPathSetColumn`, `OCI_DirPathSetConvertMode`, `OCI_DirPathSetCurrentRows`, `OCI_DirPathSetDateFormat`, `OCI_DirPathSetEntry`, `OCI_DirPathSetNoLog`, `OCI_DirPathSetParallel`

## `OCI_Elem* — Elem` → [collections-advanced](collections-advanced.html)

`OCI_ElemCreate`, `OCI_ElemFree`, `OCI_ElemGetBigInt`, `OCI_ElemGetBoolean`, `OCI_ElemGetColl`, `OCI_ElemGetDate`, `OCI_ElemGetDouble`, `OCI_ElemGetFile`, `OCI_ElemGetFloat`, `OCI_ElemGetInt`, `OCI_ElemGetInterval`, `OCI_ElemGetLob`, `OCI_ElemGetNumber`, `OCI_ElemGetObject`, `OCI_ElemGetRaw`, `OCI_ElemGetRawSize`, `OCI_ElemGetRef`, `OCI_ElemGetShort`, `OCI_ElemGetString`, `OCI_ElemGetTimestamp`, `OCI_ElemGetUnsignedBigInt`, `OCI_ElemGetUnsignedInt`, `OCI_ElemGetUnsignedShort`, `OCI_ElemIsNull`, `OCI_ElemSetBigInt`, `OCI_ElemSetBoolean`, `OCI_ElemSetColl`, `OCI_ElemSetDate`, `OCI_ElemSetDouble`, `OCI_ElemSetFile`, `OCI_ElemSetFloat`, `OCI_ElemSetInt`, `OCI_ElemSetInterval`, `OCI_ElemSetLob`, `OCI_ElemSetNull`, `OCI_ElemSetNumber`, `OCI_ElemSetObject`, `OCI_ElemSetRaw`, `OCI_ElemSetRef`, `OCI_ElemSetShort`, `OCI_ElemSetString`, `OCI_ElemSetTimestamp`, `OCI_ElemSetUnsignedBigInt`, `OCI_ElemSetUnsignedInt`, `OCI_ElemSetUnsignedShort`

## `OCI_Enable*`

`OCI_EnableWarnings`

## `OCI_Enqueue* — Enqueue` → [aq-enqueue](aq-enqueue.html)

`OCI_EnqueueCreate`, `OCI_EnqueueFree`, `OCI_EnqueueGetRelativeMsgID`, `OCI_EnqueueGetSequenceDeviation`, `OCI_EnqueueGetVisibility`, `OCI_EnqueuePut`, `OCI_EnqueueSetRelativeMsgID`, `OCI_EnqueueSetSequenceDeviation`, `OCI_EnqueueSetVisibility`

## `OCI_Environment* — Environment` → [environments](environments.html)

`OCI_EnvironmentCreate`, `OCI_EnvironmentFree`

## `OCI_Error* — Error` → [error-handling](error-handling.html)

`OCI_ErrorGetCategory`, `OCI_ErrorGetCode`, `OCI_ErrorGetConnection`, `OCI_ErrorGetLocation`, `OCI_ErrorGetRow`, `OCI_ErrorGetStatement`, `OCI_ErrorGetString`, `OCI_ErrorGetType`

## `OCI_Event* — Event` → [notifications](notifications.html)

`OCI_EventGetDatabase`, `OCI_EventGetObject`, `OCI_EventGetOperation`, `OCI_EventGetRowid`, `OCI_EventGetSubscription`, `OCI_EventGetType`

## `OCI_Execute*`

`OCI_Execute`, `OCI_ExecuteAndGetResultset`, `OCI_ExecuteStmt`, `OCI_ExecuteStmtAndGetResultset`, `OCI_ExecuteStmtFmt`

## `OCI_Fetch*`

`OCI_FetchFirst`, `OCI_FetchLast`, `OCI_FetchNext`, `OCI_FetchPrev`, `OCI_FetchSeek`

## `OCI_File* — File` → [bfiles](bfiles.html)

`OCI_FileArrayCreate`, `OCI_FileArrayFree`, `OCI_FileAssign`, `OCI_FileClose`, `OCI_FileCreate`, `OCI_FileExists`, `OCI_FileFree`, `OCI_FileGetConnection`, `OCI_FileGetDirectory`, `OCI_FileGetName`, `OCI_FileGetOffset`, `OCI_FileGetSize`, `OCI_FileGetType`, `OCI_FileIsEqual`, `OCI_FileIsOpen`, `OCI_FileOpen`, `OCI_FileRead`, `OCI_FileSeek`, `OCI_FileSetName`

## `OCI_Get*`

`OCI_GetAffectedRows`, `OCI_GetAutoCommit`, `OCI_GetBatchError`, `OCI_GetBatchErrorCount`, `OCI_GetBigInt`, `OCI_GetBigIntByName`, `OCI_GetBind`, `OCI_GetBindAllocation`, `OCI_GetBindByName`, `OCI_GetBindCount`, `OCI_GetBindIndex`, `OCI_GetBindMode`, `OCI_GetCharset`, `OCI_GetColl`, `OCI_GetCollByName`, `OCI_GetColumn`, `OCI_GetColumnByName`, `OCI_GetColumnCount`, `OCI_GetColumnIndex`, `OCI_GetCurrentRow`, `OCI_GetDBName`, `OCI_GetDataLength`, `OCI_GetDataSize`, `OCI_GetDataSizeByName`, `OCI_GetDatabase`, `OCI_GetDate`, `OCI_GetDateByName`, `OCI_GetDefaultLobPrefetchSize`, `OCI_GetDomainName`, `OCI_GetDouble`, `OCI_GetDoubleByName`, `OCI_GetFetchMode`, `OCI_GetFetchSize`, `OCI_GetFile`, `OCI_GetFileByName`, `OCI_GetFloat`, `OCI_GetFloatByName`, `OCI_GetFormat`, `OCI_GetImportMode`, `OCI_GetInstanceName`, `OCI_GetInstanceStartTime`, `OCI_GetInt`, `OCI_GetIntByName`, `OCI_GetInterval`, `OCI_GetIntervalByName`, `OCI_GetJson`, `OCI_GetJsonByName`, `OCI_GetLastError`, `OCI_GetLob`, `OCI_GetLobByName`, `OCI_GetLocaleString`, `OCI_GetLong`, `OCI_GetLongByName`, `OCI_GetLongMode`, `OCI_GetMaxCursors`, `OCI_GetNextResultset`, `OCI_GetNumber`, `OCI_GetNumberByName`, `OCI_GetOCICompileVersion`, `OCI_GetOCIRuntimeVersion`, `OCI_GetObject`, `OCI_GetObjectByName`, `OCI_GetParseBindNames`, `OCI_GetPassword`, `OCI_GetPieceSize`, `OCI_GetPrefetchMemory`, `OCI_GetPrefetchSize`, `OCI_GetRaw`, `OCI_GetRawByName`, `OCI_GetRef`, `OCI_GetRefByName`, `OCI_GetResultset`, `OCI_GetRowCount`, `OCI_GetSQLCommand`, `OCI_GetSQLVerb`, `OCI_GetServerMajorVersion`, `OCI_GetServerMinorVersion`, `OCI_GetServerName`, `OCI_GetServerRevisionVersion`, `OCI_GetServiceName`, `OCI_GetSessionMode`, `OCI_GetSessionTag`, `OCI_GetShort`, `OCI_GetShortByName`, `OCI_GetSql`, `OCI_GetSqlErrorPos`, `OCI_GetSqlIdentifier`, `OCI_GetStatement`, `OCI_GetStatementByName`, `OCI_GetStatementCacheSize`, `OCI_GetStatementType`, `OCI_GetStats`, `OCI_GetString`, `OCI_GetStringByName`, `OCI_GetStruct`, `OCI_GetSubscriptionPort`, `OCI_GetTimeout`, `OCI_GetTimestamp`, `OCI_GetTimestampByName`, `OCI_GetTrace`, `OCI_GetTransaction`, `OCI_GetUnsignedBigInt`, `OCI_GetUnsignedBigIntByName`, `OCI_GetUnsignedInt`, `OCI_GetUnsignedIntByName`, `OCI_GetUnsignedShort`, `OCI_GetUnsignedShortByName`, `OCI_GetUserData`, `OCI_GetUserName`, `OCI_GetVector`, `OCI_GetVectorByName`, `OCI_GetVersionConnection`, `OCI_GetVersionServer`, `OCI_GetXmlType`, `OCI_GetXmlTypeByName`

## `OCI_Handle*`

`OCI_HandleGetEnvironment`

## `OCI_Has*`

`OCI_HasError`

## `OCI_Hash* — Hash` → [hash-tables](hash-tables.html)

`OCI_HashAddInt`, `OCI_HashAddPointer`, `OCI_HashAddString`, `OCI_HashCreate`, `OCI_HashFree`, `OCI_HashGetEntry`, `OCI_HashGetInt`, `OCI_HashGetPointer`, `OCI_HashGetSize`, `OCI_HashGetString`, `OCI_HashGetType`, `OCI_HashGetValue`, `OCI_HashLookup`

## `OCI_Immediate* — Immediate` → [statement-lifecycle](statement-lifecycle.html)

`OCI_Immediate`, `OCI_ImmediateFmt`

## `OCI_Interval* — Interval` → [intervals](intervals.html)

`OCI_IntervalAdd`, `OCI_IntervalArrayCreate`, `OCI_IntervalArrayFree`, `OCI_IntervalAssign`, `OCI_IntervalCheck`, `OCI_IntervalCompare`, `OCI_IntervalCreate`, `OCI_IntervalFree`, `OCI_IntervalFromText`, `OCI_IntervalFromTimeZone`, `OCI_IntervalGetDaySecond`, `OCI_IntervalGetType`, `OCI_IntervalGetYearMonth`, `OCI_IntervalSetDaySecond`, `OCI_IntervalSetYearMonth`, `OCI_IntervalSubtract`, `OCI_IntervalToText`

## `OCI_Is*`

`OCI_IsConnected`, `OCI_IsNull`, `OCI_IsNullByName`, `OCI_IsRebindingAllowed`, `OCI_IsTAFCapable`

## `OCI_Iter* — Iter` → [collections-advanced](collections-advanced.html)

`OCI_IterCreate`, `OCI_IterFree`, `OCI_IterGetCurrent`, `OCI_IterGetNext`, `OCI_IterGetPrev`

## `OCI_Json* — Json` → [json-columns](json-columns.html)

`OCI_JsonArrayCreate`, `OCI_JsonArrayFree`, `OCI_JsonClone`, `OCI_JsonCreate`, `OCI_JsonFree`, `OCI_JsonGetConnection`, `OCI_JsonGetRoot`, `OCI_JsonIsMutable`, `OCI_JsonLoadBinary`, `OCI_JsonLoadText`, `OCI_JsonSetMutable`, `OCI_JsonSetRoot`, `OCI_JsonToBinary`, `OCI_JsonToText`, `OCI_JsonValidate`

## `OCI_JsonNode* — JsonNode` → [json-dom](json-dom.html)

`OCI_JsonNodeAppendItem`, `OCI_JsonNodeAssign`, `OCI_JsonNodeClear`, `OCI_JsonNodeCreate`, `OCI_JsonNodeCreateArray`, `OCI_JsonNodeCreateBinary`, `OCI_JsonNodeCreateBoolean`, `OCI_JsonNodeCreateDate`, `OCI_JsonNodeCreateInterval`, `OCI_JsonNodeCreateNull`, `OCI_JsonNodeCreateNumber`, `OCI_JsonNodeCreateObject`, `OCI_JsonNodeCreateString`, `OCI_JsonNodeCreateTimestamp`, `OCI_JsonNodeEquals`, `OCI_JsonNodeFree`, `OCI_JsonNodeGetBigInt`, `OCI_JsonNodeGetBigUInt`, `OCI_JsonNodeGetBinary`, `OCI_JsonNodeGetBoolean`, `OCI_JsonNodeGetByPath`, `OCI_JsonNodeGetDate`, `OCI_JsonNodeGetDouble`, `OCI_JsonNodeGetFieldByIndex`, `OCI_JsonNodeGetFieldByName`, `OCI_JsonNodeGetFieldCount`, `OCI_JsonNodeGetFieldName`, `OCI_JsonNodeGetFloat`, `OCI_JsonNodeGetInt`, `OCI_JsonNodeGetInterval`, `OCI_JsonNodeGetItem`, `OCI_JsonNodeGetItemCount`, `OCI_JsonNodeGetJson`, `OCI_JsonNodeGetNumber`, `OCI_JsonNodeGetParent`, `OCI_JsonNodeGetShort`, `OCI_JsonNodeGetString`, `OCI_JsonNodeGetTimestamp`, `OCI_JsonNodeGetType`, `OCI_JsonNodeGetUInt`, `OCI_JsonNodeGetUShort`, `OCI_JsonNodeHasField`, `OCI_JsonNodeInsertItem`, `OCI_JsonNodeIsNull`, `OCI_JsonNodeRemoveField`, `OCI_JsonNodeRemoveItem`, `OCI_JsonNodeRemoveItems`, `OCI_JsonNodeRenameField`, `OCI_JsonNodeSetBigInt`, `OCI_JsonNodeSetBigUInt`, `OCI_JsonNodeSetBinary`, `OCI_JsonNodeSetBoolean`, `OCI_JsonNodeSetDate`, `OCI_JsonNodeSetDouble`, `OCI_JsonNodeSetField`, `OCI_JsonNodeSetFloat`, `OCI_JsonNodeSetInt`, `OCI_JsonNodeSetInterval`, `OCI_JsonNodeSetItem`, `OCI_JsonNodeSetNull`, `OCI_JsonNodeSetNumber`, `OCI_JsonNodeSetShort`, `OCI_JsonNodeSetString`, `OCI_JsonNodeSetTimestamp`, `OCI_JsonNodeSetUInt`, `OCI_JsonNodeSetUShort`

## `OCI_Lob* — Lob` → [lobs-basics](lobs-basics.html)

`OCI_LobAppend`, `OCI_LobAppendLob`, `OCI_LobArrayCreate`, `OCI_LobArrayFree`, `OCI_LobAssign`, `OCI_LobClose`, `OCI_LobCopy`, `OCI_LobCopyFromFile`, `OCI_LobCreate`, `OCI_LobEnableBuffering`, `OCI_LobErase`, `OCI_LobFlush`, `OCI_LobFree`, `OCI_LobGetChunkSize`, `OCI_LobGetConnection`, `OCI_LobGetLength`, `OCI_LobGetMaxSize`, `OCI_LobGetOffset`, `OCI_LobGetType`, `OCI_LobIsEqual`, `OCI_LobIsRemote`, `OCI_LobIsTemporary`, `OCI_LobOpen`, `OCI_LobRead`, `OCI_LobSeek`, `OCI_LobTruncate`, `OCI_LobWrite`

## `OCI_Long* — Long` → [longs](longs.html)

`OCI_LongCreate`, `OCI_LongFree`, `OCI_LongGetBuffer`, `OCI_LongGetSize`, `OCI_LongGetType`, `OCI_LongRead`, `OCI_LongWrite`

## `OCI_Msg* — Msg` → [aq-enqueue](aq-enqueue.html)

`OCI_MsgCreate`, `OCI_MsgFree`, `OCI_MsgGetAttemptCount`, `OCI_MsgGetCorrelation`, `OCI_MsgGetEnqueueDelay`, `OCI_MsgGetEnqueueTime`, `OCI_MsgGetExceptionQueue`, `OCI_MsgGetExpiration`, `OCI_MsgGetID`, `OCI_MsgGetObject`, `OCI_MsgGetOriginalID`, `OCI_MsgGetPriority`, `OCI_MsgGetRaw`, `OCI_MsgGetSender`, `OCI_MsgGetState`, `OCI_MsgReset`, `OCI_MsgSetConsumers`, `OCI_MsgSetCorrelation`, `OCI_MsgSetEnqueueDelay`, `OCI_MsgSetExceptionQueue`, `OCI_MsgSetExpiration`, `OCI_MsgSetObject`, `OCI_MsgSetOriginalID`, `OCI_MsgSetPriority`, `OCI_MsgSetRaw`, `OCI_MsgSetSender`

## `OCI_Number* — Number` → [numbers](numbers.html)

`OCI_NumberAdd`, `OCI_NumberArrayCreate`, `OCI_NumberArrayFree`, `OCI_NumberAssign`, `OCI_NumberCompare`, `OCI_NumberCreate`, `OCI_NumberDivide`, `OCI_NumberFree`, `OCI_NumberFromText`, `OCI_NumberGetContent`, `OCI_NumberGetValue`, `OCI_NumberMultiply`, `OCI_NumberSetContent`, `OCI_NumberSetValue`, `OCI_NumberSub`, `OCI_NumberToText`

## `OCI_Object* — Object` → [objects-attrs-complete](objects-attrs-complete.html)

`OCI_ObjectArrayCreate`, `OCI_ObjectArrayFree`, `OCI_ObjectAssign`, `OCI_ObjectCreate`, `OCI_ObjectFree`, `OCI_ObjectGetBigInt`, `OCI_ObjectGetBoolean`, `OCI_ObjectGetColl`, `OCI_ObjectGetDate`, `OCI_ObjectGetDouble`, `OCI_ObjectGetFile`, `OCI_ObjectGetFloat`, `OCI_ObjectGetInt`, `OCI_ObjectGetInterval`, `OCI_ObjectGetLob`, `OCI_ObjectGetNumber`, `OCI_ObjectGetObject`, `OCI_ObjectGetRaw`, `OCI_ObjectGetRawSize`, `OCI_ObjectGetRef`, `OCI_ObjectGetSelfRef`, `OCI_ObjectGetShort`, `OCI_ObjectGetString`, `OCI_ObjectGetStruct`, `OCI_ObjectGetTimestamp`, `OCI_ObjectGetType`, `OCI_ObjectGetTypeInfo`, `OCI_ObjectGetUnsignedBigInt`, `OCI_ObjectGetUnsignedInt`, `OCI_ObjectGetUnsignedShort`, `OCI_ObjectIsNull`, `OCI_ObjectSetBigInt`, `OCI_ObjectSetBoolean`, `OCI_ObjectSetColl`, `OCI_ObjectSetDate`, `OCI_ObjectSetDouble`, `OCI_ObjectSetFile`, `OCI_ObjectSetFloat`, `OCI_ObjectSetInt`, `OCI_ObjectSetInterval`, `OCI_ObjectSetLob`, `OCI_ObjectSetNull`, `OCI_ObjectSetNumber`, `OCI_ObjectSetObject`, `OCI_ObjectSetRaw`, `OCI_ObjectSetRef`, `OCI_ObjectSetShort`, `OCI_ObjectSetString`, `OCI_ObjectSetTimestamp`, `OCI_ObjectSetUnsignedBigInt`, `OCI_ObjectSetUnsignedInt`, `OCI_ObjectSetUnsignedShort`, `OCI_ObjectToText`

## `OCI_Parse*`

`OCI_Parse`, `OCI_ParseFmt`

## `OCI_Ping*`

`OCI_Ping`

## `OCI_Pool* — Pool` → [pooling-basics](pooling-basics.html)

`OCI_PoolCreate`, `OCI_PoolFree`, `OCI_PoolGetBusyCount`, `OCI_PoolGetConnection`, `OCI_PoolGetConnectionSharded`, `OCI_PoolGetIncrement`, `OCI_PoolGetMax`, `OCI_PoolGetMin`, `OCI_PoolGetNoWait`, `OCI_PoolGetOpenedCount`, `OCI_PoolGetStatementCacheSize`, `OCI_PoolGetTimeout`, `OCI_PoolSetNoWait`, `OCI_PoolSetStatementCacheSize`, `OCI_PoolSetTimeout`

## `OCI_Prepare*`

`OCI_Prepare`, `OCI_PrepareFmt`

## `OCI_Queue* — Queue` → [aq-setup](aq-setup.html)

`OCI_QueueAlter`, `OCI_QueueCreate`, `OCI_QueueDrop`, `OCI_QueueStart`, `OCI_QueueStop`

## `OCI_QueueTable* — QueueTable` → [aq-setup](aq-setup.html)

`OCI_QueueTableAlter`, `OCI_QueueTableCreate`, `OCI_QueueTableDrop`, `OCI_QueueTableMigrate`, `OCI_QueueTablePurge`

## `OCI_Ref* — Ref` → [refs](refs.html)

`OCI_RefArrayCreate`, `OCI_RefArrayFree`, `OCI_RefAssign`, `OCI_RefCreate`, `OCI_RefFree`, `OCI_RefGetHexSize`, `OCI_RefGetObject`, `OCI_RefGetTypeInfo`, `OCI_RefIsNull`, `OCI_RefSetNull`, `OCI_RefToText`

## `OCI_Register* — Register` → [rowid-returning](rowid-returning.html)

`OCI_RegisterBigInt`, `OCI_RegisterDate`, `OCI_RegisterDouble`, `OCI_RegisterFile`, `OCI_RegisterFloat`, `OCI_RegisterInt`, `OCI_RegisterInterval`, `OCI_RegisterJson`, `OCI_RegisterLob`, `OCI_RegisterNumber`, `OCI_RegisterObject`, `OCI_RegisterRaw`, `OCI_RegisterRef`, `OCI_RegisterShort`, `OCI_RegisterString`, `OCI_RegisterTimestamp`, `OCI_RegisterUnsignedBigInt`, `OCI_RegisterUnsignedInt`, `OCI_RegisterUnsignedShort`, `OCI_RegisterVector`

## `OCI_Release*`

`OCI_ReleaseResultsets`

## `OCI_Resultset* — Resultset` → [fetch-getters](fetch-getters.html)

`OCI_ResultsetGetStatement`

## `OCI_Rollback*`

`OCI_Rollback`

## `OCI_Server* — Server` → [server-output](server-output.html)

`OCI_ServerDisableOutput`, `OCI_ServerEnableOutput`, `OCI_ServerGetOutput`

## `OCI_Set*`

`OCI_SetAutoCommit`, `OCI_SetBindAllocation`, `OCI_SetBindMode`, `OCI_SetDefaultLobPrefetchSize`, `OCI_SetErrorHandler`, `OCI_SetFetchMode`, `OCI_SetFetchSize`, `OCI_SetFormat`, `OCI_SetHAHandler`, `OCI_SetLongMode`, `OCI_SetPassword`, `OCI_SetPieceSize`, `OCI_SetPrefetchMemory`, `OCI_SetPrefetchSize`, `OCI_SetSessionTag`, `OCI_SetStatementCacheSize`, `OCI_SetStructNumericType`, `OCI_SetStructNumericTypeByName`, `OCI_SetSubscriptionPort`, `OCI_SetTAFHandler`, `OCI_SetTimeout`, `OCI_SetTrace`, `OCI_SetTransaction`, `OCI_SetUserData`, `OCI_SetUserPassword`

## `OCI_ShardingKey* — ShardingKey` → [sharding](sharding.html)

`OCI_ShardingKeyAddDate`, `OCI_ShardingKeyAddNumber`, `OCI_ShardingKeyAddRaw`, `OCI_ShardingKeyAddString`, `OCI_ShardingKeyAddTimestamp`, `OCI_ShardingKeyCreate`, `OCI_ShardingKeyFree`, `OCI_ShardingKeyReset`, `OCI_ShardingKeyToText`

## `OCI_SodaColl* — SodaColl` → [soda-basics](soda-basics.html)

`OCI_SodaCollCreateIndex`, `OCI_SodaCollDocCount`, `OCI_SodaCollDocCountWithFilter`, `OCI_SodaCollDrop`, `OCI_SodaCollDropIndex`, `OCI_SodaCollFind`, `OCI_SodaCollFindOne`, `OCI_SodaCollFindOneByKey`, `OCI_SodaCollFree`, `OCI_SodaCollFreeIndexNames`, `OCI_SodaCollGetConnection`, `OCI_SodaCollGetDataGuide`, `OCI_SodaCollGetMetadata`, `OCI_SodaCollGetName`, `OCI_SodaCollInsertDoc`, `OCI_SodaCollInsertMany`, `OCI_SodaCollIsMapped`, `OCI_SodaCollListIndexes`, `OCI_SodaCollRemove`, `OCI_SodaCollRemoveOneByKey`, `OCI_SodaCollReplaceOne`, `OCI_SodaCollReplaceOneByKey`, `OCI_SodaCollSave`, `OCI_SodaCollTruncate`

## `OCI_SodaCollCursor* — SodaCollCursor` → [soda-advanced](soda-advanced.html)

`OCI_SodaCollCursorFree`, `OCI_SodaCollCursorGetNext`

## `OCI_SodaDb* — SodaDb` → [soda-basics](soda-basics.html)

`OCI_SodaDbCreate`, `OCI_SodaDbCreateColl`, `OCI_SodaDbFree`, `OCI_SodaDbFreeCollectionNames`, `OCI_SodaDbGetCollectionNames`, `OCI_SodaDbGetColls`, `OCI_SodaDbGetConnection`, `OCI_SodaDbOpenColl`

## `OCI_SodaDoc* — SodaDoc` → [soda-basics](soda-basics.html)

`OCI_SodaDocCreateFromJson`, `OCI_SodaDocCreateFromText`, `OCI_SodaDocFree`, `OCI_SodaDocGetCreatedOn`, `OCI_SodaDocGetIsJson`, `OCI_SodaDocGetJson`, `OCI_SodaDocGetKey`, `OCI_SodaDocGetLastModified`, `OCI_SodaDocGetMediaType`, `OCI_SodaDocGetText`, `OCI_SodaDocGetVersion`

## `OCI_SodaDocCursor* — SodaDocCursor` → [soda-basics](soda-basics.html)

`OCI_SodaDocCursorFree`, `OCI_SodaDocCursorGetNext`

## `OCI_SodaOptions* — SodaOptions` → [soda-advanced](soda-advanced.html)

`OCI_SodaOptionsCreate`, `OCI_SodaOptionsFree`, `OCI_SodaOptionsSetFetchArraySize`, `OCI_SodaOptionsSetFilter`, `OCI_SodaOptionsSetHint`, `OCI_SodaOptionsSetKey`, `OCI_SodaOptionsSetKeys`, `OCI_SodaOptionsSetLimit`, `OCI_SodaOptionsSetLock`, `OCI_SodaOptionsSetSkip`, `OCI_SodaOptionsSetVersion`

## `OCI_Statement* — Statement` → [statement-lifecycle](statement-lifecycle.html)

`OCI_StatementCreate`, `OCI_StatementFree`, `OCI_StatementGetConnection`

## `OCI_Subscription* — Subscription` → [notifications](notifications.html)

`OCI_SubscriptionAddStatement`, `OCI_SubscriptionGetConnection`, `OCI_SubscriptionGetName`, `OCI_SubscriptionGetTimeout`, `OCI_SubscriptionRegister`, `OCI_SubscriptionUnregister`

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
