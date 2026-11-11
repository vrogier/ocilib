/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2026 Vincent ROGIER <vince.rogier@ocilib.net>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * Source compatibility aliases.
 *
 * OCILIB 5.0.0 normalized the public C API naming around the handle
 * model (see the architecture tutorial). The old pre-rename names
 * are mapped here to their new equivalents so existing sources keep
 * compiling unchanged. Define OCI_NO_COMPAT to disable this header.
 */

#ifndef OCILIB_OCILIBC_COMPAT_H_INCLUDED
#define OCILIB_OCILIBC_COMPAT_H_INCLUDED

#ifndef OCI_NO_COMPAT

/* renamed handle types */

#define OCI_Coll                         OCI_Collection
#define OCI_DirPath                      OCI_DirectPath
#define OCI_Elem                         OCI_Element
#define OCI_Iter                         OCI_Iterator
#define OCI_Msg                          OCI_Message
#define OCI_Ref                          OCI_Reference
#define OCI_SodaColl                     OCI_SodaCollection
#define OCI_SodaCollCursor               OCI_SodaCollectionCursor
#define OCI_SodaDb                       OCI_SodaDatabase
#define OCI_SodaDoc                      OCI_SodaDocument
#define OCI_SodaDocCursor                OCI_SodaDocumentCursor

/* renamed constants - sized integer type scheme */

#define OCI_ARG_BIGINT                   OCI_ARG_INT64
#define OCI_ARG_BIGUINT                  OCI_ARG_UINT64
#define OCI_ARG_INT                      OCI_ARG_INT32
#define OCI_ARG_SHORT                    OCI_ARG_INT16
#define OCI_ARG_UINT                     OCI_ARG_UINT32
#define OCI_ARG_USHORT                   OCI_ARG_UINT16
#define OCI_NUM_BIGINT                   OCI_NUM_INT64
#define OCI_NUM_BIGUINT                  OCI_NUM_UINT64
#define OCI_NUM_INT                      OCI_NUM_INT32
#define OCI_NUM_SHORT                    OCI_NUM_INT16
#define OCI_NUM_UINT                     OCI_NUM_UINT32
#define OCI_NUM_USHORT                   OCI_NUM_UINT16

/* renamed charset constant (the ANSI build was renamed MBCS;
   the build flag itself is aliased earlier in platform.h) */

#define OCI_CHAR_ANSI                    OCI_CHAR_MBCS

/* renamed functions */

/* Collection */

#define OCI_CollAppend                   OCI_CollectionAppend
#define OCI_CollArrayCreate              OCI_CollectionArrayCreate
#define OCI_CollArrayFree                OCI_CollectionArrayFree
#define OCI_CollAssign                   OCI_CollectionAssign
#define OCI_CollClear                    OCI_CollectionClear
#define OCI_CollCreate                   OCI_CollectionCreate
#define OCI_CollDeleteElem               OCI_CollectionDeleteElement
#define OCI_CollFree                     OCI_CollectionFree
#define OCI_CollGetCount                 OCI_CollectionGetCount
#define OCI_CollGetElem                  OCI_CollectionGetElement
#define OCI_CollGetMax                   OCI_CollectionGetMax
#define OCI_CollGetSize                  OCI_CollectionGetSize
#define OCI_CollGetType                  OCI_CollectionGetType
#define OCI_CollGetTypeInfo              OCI_CollectionGetTypeInfo
#define OCI_CollSetElem                  OCI_CollectionSetElement
#define OCI_CollToText                   OCI_CollectionToText
#define OCI_CollTrim                     OCI_CollectionTrim

/* Column */

#define OCI_ColumnGetFullSQLType         OCI_ColumnGetFullSqlType
#define OCI_ColumnGetSQLType             OCI_ColumnGetSqlType

/* Connection */

#define OCI_Break                        OCI_ConnectionBreak
#define OCI_Commit                       OCI_ConnectionCommit
#define OCI_GetAutoCommit                OCI_ConnectionGetAutoCommit
#define OCI_GetDBName                    OCI_ConnectionGetDatabaseName
#define OCI_GetDatabase                  OCI_ConnectionGetConnectionString
#define OCI_GetDefaultLobPrefetchSize    OCI_ConnectionGetDefaultLobPrefetchSize
#define OCI_GetDomainName                OCI_ConnectionGetDomainName
#define OCI_GetInstanceName              OCI_ConnectionGetInstanceName
#define OCI_GetInstanceStartTime         OCI_ConnectionGetInstanceStartTime
#define OCI_GetMaxCursors                OCI_ConnectionGetMaxCursors
#define OCI_GetPassword                  OCI_ConnectionGetPassword
#define OCI_GetServerMajorVersion        OCI_ConnectionGetServerMajorVersion
#define OCI_GetServerMinorVersion        OCI_ConnectionGetServerMinorVersion
#define OCI_GetServerName                OCI_ConnectionGetServerName
#define OCI_GetServerRevisionVersion     OCI_ConnectionGetServerRevisionVersion
#define OCI_GetServiceName               OCI_ConnectionGetServiceName
#define OCI_GetSessionMode               OCI_ConnectionGetSessionMode
#define OCI_GetSessionTag                OCI_ConnectionGetSessionTag
#define OCI_GetStatementCacheSize        OCI_ConnectionGetStatementCacheSize
#define OCI_GetTimeout                   OCI_ConnectionGetTimeout
#define OCI_GetTrace                     OCI_ConnectionGetTrace
#define OCI_GetTransaction               OCI_ConnectionGetTransaction
#define OCI_GetUserName                  OCI_ConnectionGetUserName
#define OCI_GetVersionConnection         OCI_ConnectionGetVersion
#define OCI_GetVersionServer             OCI_ConnectionGetServerVersionString
#define OCI_Immediate                    OCI_ConnectionImmediate
#define OCI_ImmediateFmt                 OCI_ConnectionImmediateFormat
#define OCI_IsConnected                  OCI_ConnectionIsAlive
#define OCI_IsTAFCapable                 OCI_ConnectionIsTAFCapable
#define OCI_Ping                         OCI_ConnectionPing
#define OCI_Rollback                     OCI_ConnectionRollback
#define OCI_ServerDisableOutput          OCI_ConnectionDisableServerOutput
#define OCI_ServerEnableOutput           OCI_ConnectionEnableServerOutput
#define OCI_ServerGetOutput              OCI_ConnectionGetServerOutput
#define OCI_SetAutoCommit                OCI_ConnectionSetAutoCommit
#define OCI_SetDefaultLobPrefetchSize    OCI_ConnectionSetDefaultLobPrefetchSize
#define OCI_SetPassword                  OCI_ConnectionSetPassword
#define OCI_SetSessionTag                OCI_ConnectionSetSessionTag
#define OCI_SetStatementCacheSize        OCI_ConnectionSetStatementCacheSize
#define OCI_SetTAFHandler                OCI_ConnectionSetTAFHandler
#define OCI_SetTimeout                   OCI_ConnectionSetTimeout
#define OCI_SetTrace                     OCI_ConnectionSetTrace
#define OCI_SetTransaction               OCI_ConnectionSetTransaction

/* Direct */

#define OCI_DirPathAbort                 OCI_DirectPathAbort
#define OCI_DirPathConvert               OCI_DirectPathConvert
#define OCI_DirPathCreate                OCI_DirectPathCreate
#define OCI_DirPathFinish                OCI_DirectPathFinish
#define OCI_DirPathFlushRow              OCI_DirectPathFlushRow
#define OCI_DirPathFree                  OCI_DirectPathFree
#define OCI_DirPathGetAffectedRows       OCI_DirectPathGetAffectedRows
#define OCI_DirPathGetCurrentRows        OCI_DirectPathGetCurrentRows
#define OCI_DirPathGetErrorColumn        OCI_DirectPathGetErrorColumn
#define OCI_DirPathGetErrorRow           OCI_DirectPathGetErrorRow
#define OCI_DirPathGetMaxRows            OCI_DirectPathGetMaxRows
#define OCI_DirPathGetRowCount           OCI_DirectPathGetRowCount
#define OCI_DirPathLoad                  OCI_DirectPathLoad
#define OCI_DirPathPrepare               OCI_DirectPathPrepare
#define OCI_DirPathReset                 OCI_DirectPathReset
#define OCI_DirPathSave                  OCI_DirectPathSave
#define OCI_DirPathSetBufferSize         OCI_DirectPathSetBufferSize
#define OCI_DirPathSetCacheSize          OCI_DirectPathSetCacheSize
#define OCI_DirPathSetColumn             OCI_DirectPathSetColumn
#define OCI_DirPathSetConvertMode        OCI_DirectPathSetConvertMode
#define OCI_DirPathSetCurrentRows        OCI_DirectPathSetCurrentRows
#define OCI_DirPathSetDateFormat         OCI_DirectPathSetDateFormat
#define OCI_DirPathSetEntry              OCI_DirectPathSetEntry
#define OCI_DirPathSetNoLog              OCI_DirectPathSetNoLog
#define OCI_DirPathSetParallel           OCI_DirectPathSetParallel

/* Element */

#define OCI_ElemCreate                   OCI_ElementCreate
#define OCI_ElemFree                     OCI_ElementFree
#define OCI_ElemGetBigInt                OCI_ElementGetInt64
#define OCI_ElemGetBoolean               OCI_ElementGetBoolean
#define OCI_ElemGetColl                  OCI_ElementGetCollection
#define OCI_ElemGetDate                  OCI_ElementGetDate
#define OCI_ElemGetDouble                OCI_ElementGetDouble
#define OCI_ElemGetFile                  OCI_ElementGetFile
#define OCI_ElemGetFloat                 OCI_ElementGetFloat
#define OCI_ElemGetInt                   OCI_ElementGetInt32
#define OCI_ElemGetInterval              OCI_ElementGetInterval
#define OCI_ElemGetLob                   OCI_ElementGetLob
#define OCI_ElemGetNumber                OCI_ElementGetNumber
#define OCI_ElemGetObject                OCI_ElementGetObject
#define OCI_ElemGetRaw                   OCI_ElementGetRaw
#define OCI_ElemGetRawSize               OCI_ElementGetRawSize
#define OCI_ElemGetRef                   OCI_ElementGetReference
#define OCI_ElemGetShort                 OCI_ElementGetInt16
#define OCI_ElemGetString                OCI_ElementGetString
#define OCI_ElemGetTimestamp             OCI_ElementGetTimestamp
#define OCI_ElemGetUnsignedBigInt        OCI_ElementGetUInt64
#define OCI_ElemGetUnsignedInt           OCI_ElementGetUInt32
#define OCI_ElemGetUnsignedShort         OCI_ElementGetUInt16
#define OCI_ElemIsNull                   OCI_ElementIsNull
#define OCI_ElemSetBigInt                OCI_ElementSetInt64
#define OCI_ElemSetBoolean               OCI_ElementSetBoolean
#define OCI_ElemSetColl                  OCI_ElementSetCollection
#define OCI_ElemSetDate                  OCI_ElementSetDate
#define OCI_ElemSetDouble                OCI_ElementSetDouble
#define OCI_ElemSetFile                  OCI_ElementSetFile
#define OCI_ElemSetFloat                 OCI_ElementSetFloat
#define OCI_ElemSetInt                   OCI_ElementSetInt32
#define OCI_ElemSetInterval              OCI_ElementSetInterval
#define OCI_ElemSetLob                   OCI_ElementSetLob
#define OCI_ElemSetNull                  OCI_ElementSetNull
#define OCI_ElemSetNumber                OCI_ElementSetNumber
#define OCI_ElemSetObject                OCI_ElementSetObject
#define OCI_ElemSetRaw                   OCI_ElementSetRaw
#define OCI_ElemSetRef                   OCI_ElementSetReference
#define OCI_ElemSetShort                 OCI_ElementSetInt16
#define OCI_ElemSetString                OCI_ElementSetString
#define OCI_ElemSetTimestamp             OCI_ElementSetTimestamp
#define OCI_ElemSetUnsignedBigInt        OCI_ElementSetUInt64
#define OCI_ElemSetUnsignedInt           OCI_ElementSetUInt32
#define OCI_ElemSetUnsignedShort         OCI_ElementSetUInt16

/* Environment */

#define OCI_EnableWarnings               OCI_EnvironmentEnableWarnings
#define OCI_GetCharset                   OCI_EnvironmentGetCharset
#define OCI_GetFormat                    OCI_EnvironmentGetFormat
#define OCI_GetImportMode                OCI_EnvironmentGetImportMode
#define OCI_GetLocaleString              OCI_EnvironmentGetLocaleString
#define OCI_GetOCICompileVersion         OCI_EnvironmentGetOCICompileVersion
#define OCI_GetOCIRuntimeVersion         OCI_EnvironmentGetOCIRuntimeVersion
#define OCI_GetStats                     OCI_EnvironmentGetStats
#define OCI_GetSubscriptionPort          OCI_EnvironmentGetSubscriptionPort
#define OCI_SetErrorHandler              OCI_EnvironmentSetErrorHandler
#define OCI_SetFormat                    OCI_EnvironmentSetFormat
#define OCI_SetHAHandler                 OCI_EnvironmentSetHAHandler
#define OCI_SetSubscriptionPort          OCI_EnvironmentSetSubscriptionPort
#define OCI_SetUserPassword              OCI_EnvironmentSetUserPassword

/* Handle */

#define OCI_GetUserData                  OCI_HandleGetUserData
#define OCI_SetUserData                  OCI_HandleSetUserData

/* Hash */

#define OCI_HashAddInt                   OCI_HashTableAddInt32
#define OCI_HashAddPointer               OCI_HashTableAddPointer
#define OCI_HashAddString                OCI_HashTableAddString
#define OCI_HashCreate                   OCI_HashTableCreate
#define OCI_HashFree                     OCI_HashTableFree
#define OCI_HashGetEntry                 OCI_HashTableGetEntry
#define OCI_HashGetInt                   OCI_HashTableGetInt32
#define OCI_HashGetPointer               OCI_HashTableGetPointer
#define OCI_HashGetSize                  OCI_HashTableGetSize
#define OCI_HashGetString                OCI_HashTableGetString
#define OCI_HashGetType                  OCI_HashTableGetType
#define OCI_HashGetValue                 OCI_HashTableGetValue
#define OCI_HashLookup                   OCI_HashTableLookup

/* Iterator */

#define OCI_IterCreate                   OCI_IteratorCreate
#define OCI_IterFree                     OCI_IteratorFree
#define OCI_IterGetCurrent               OCI_IteratorGetCurrent
#define OCI_IterGetNext                  OCI_IteratorGetNext
#define OCI_IterGetPrev                  OCI_IteratorGetPrev

/* Json */

#define OCI_JsonNodeGetBigInt            OCI_JsonNodeGetInt64
#define OCI_JsonNodeGetBigUInt           OCI_JsonNodeGetUInt64
#define OCI_JsonNodeGetInt               OCI_JsonNodeGetInt32
#define OCI_JsonNodeGetShort             OCI_JsonNodeGetInt16
#define OCI_JsonNodeGetUInt              OCI_JsonNodeGetUInt32
#define OCI_JsonNodeGetUShort            OCI_JsonNodeGetUInt16
#define OCI_JsonNodeSetBigInt            OCI_JsonNodeSetInt64
#define OCI_JsonNodeSetBigUInt           OCI_JsonNodeSetUInt64
#define OCI_JsonNodeSetInt               OCI_JsonNodeSetInt32
#define OCI_JsonNodeSetShort             OCI_JsonNodeSetInt16
#define OCI_JsonNodeSetUInt              OCI_JsonNodeSetUInt32
#define OCI_JsonNodeSetUShort            OCI_JsonNodeSetUInt16

/* Message */

#define OCI_MsgCreate                    OCI_MessageCreate
#define OCI_MsgFree                      OCI_MessageFree
#define OCI_MsgGetAttemptCount           OCI_MessageGetAttemptCount
#define OCI_MsgGetCorrelation            OCI_MessageGetCorrelation
#define OCI_MsgGetEnqueueDelay           OCI_MessageGetEnqueueDelay
#define OCI_MsgGetEnqueueTime            OCI_MessageGetEnqueueTime
#define OCI_MsgGetExceptionQueue         OCI_MessageGetExceptionQueue
#define OCI_MsgGetExpiration             OCI_MessageGetExpiration
#define OCI_MsgGetID                     OCI_MessageGetID
#define OCI_MsgGetObject                 OCI_MessageGetObject
#define OCI_MsgGetOriginalID             OCI_MessageGetOriginalID
#define OCI_MsgGetPriority               OCI_MessageGetPriority
#define OCI_MsgGetRaw                    OCI_MessageGetRaw
#define OCI_MsgGetSender                 OCI_MessageGetSender
#define OCI_MsgGetState                  OCI_MessageGetState
#define OCI_MsgReset                     OCI_MessageReset
#define OCI_MsgSetConsumers              OCI_MessageSetConsumers
#define OCI_MsgSetCorrelation            OCI_MessageSetCorrelation
#define OCI_MsgSetEnqueueDelay           OCI_MessageSetEnqueueDelay
#define OCI_MsgSetExceptionQueue         OCI_MessageSetExceptionQueue
#define OCI_MsgSetExpiration             OCI_MessageSetExpiration
#define OCI_MsgSetObject                 OCI_MessageSetObject
#define OCI_MsgSetOriginalID             OCI_MessageSetOriginalID
#define OCI_MsgSetPriority               OCI_MessageSetPriority
#define OCI_MsgSetRaw                    OCI_MessageSetRaw
#define OCI_MsgSetSender                 OCI_MessageSetSender

/* Object */

#define OCI_ObjectGetBigInt              OCI_ObjectGetInt64
#define OCI_ObjectGetInt                 OCI_ObjectGetInt32
#define OCI_ObjectGetRef                 OCI_ObjectGetReference
#define OCI_ObjectGetSelfRef             OCI_ObjectGetSelfReference
#define OCI_ObjectGetShort               OCI_ObjectGetInt16
#define OCI_ObjectGetUnsignedBigInt      OCI_ObjectGetUInt64
#define OCI_ObjectGetUnsignedInt         OCI_ObjectGetUInt32
#define OCI_ObjectGetUnsignedShort       OCI_ObjectGetUInt16
#define OCI_ObjectSetBigInt              OCI_ObjectSetInt64
#define OCI_ObjectSetInt                 OCI_ObjectSetInt32
#define OCI_ObjectSetRef                 OCI_ObjectSetReference
#define OCI_ObjectSetShort               OCI_ObjectSetInt16
#define OCI_ObjectSetUnsignedBigInt      OCI_ObjectSetUInt64
#define OCI_ObjectSetUnsignedInt         OCI_ObjectSetUInt32
#define OCI_ObjectSetUnsignedShort       OCI_ObjectSetUInt16

/* Reference */

#define OCI_RefArrayCreate               OCI_ReferenceArrayCreate
#define OCI_RefArrayFree                 OCI_ReferenceArrayFree
#define OCI_RefAssign                    OCI_ReferenceAssign
#define OCI_RefCreate                    OCI_ReferenceCreate
#define OCI_RefFree                      OCI_ReferenceFree
#define OCI_RefGetHexSize                OCI_ReferenceGetHexSize
#define OCI_RefGetObject                 OCI_ReferenceGetObject
#define OCI_RefGetTypeInfo               OCI_ReferenceGetTypeInfo
#define OCI_RefIsNull                    OCI_ReferenceIsNull
#define OCI_RefSetNull                   OCI_ReferenceSetNull
#define OCI_RefToText                    OCI_ReferenceToText

/* Resultset */

#define OCI_FetchFirst                   OCI_ResultsetFetchFirst
#define OCI_FetchLast                    OCI_ResultsetFetchLast
#define OCI_FetchNext                    OCI_ResultsetFetchNext
#define OCI_FetchPrev                    OCI_ResultsetFetchPrev
#define OCI_FetchSeek                    OCI_ResultsetFetchSeek
#define OCI_GetBigInt                    OCI_ResultsetGetInt64
#define OCI_GetBigIntByName              OCI_ResultsetGetInt64ByName
#define OCI_GetColl                      OCI_ResultsetGetColl
#define OCI_GetCollByName                OCI_ResultsetGetCollByName
#define OCI_GetColumn                    OCI_ResultsetGetColumn
#define OCI_GetColumnByName              OCI_ResultsetGetColumnByName
#define OCI_GetColumnCount               OCI_ResultsetGetColumnCount
#define OCI_GetColumnIndex               OCI_ResultsetGetColumnIndex
#define OCI_GetCurrentRow                OCI_ResultsetGetCurrentRow
#define OCI_GetDataLength                OCI_ResultsetGetDataLength
#define OCI_GetDataSize                  OCI_ResultsetGetDataSize
#define OCI_GetDataSizeByName            OCI_ResultsetGetDataSizeByName
#define OCI_GetDate                      OCI_ResultsetGetDate
#define OCI_GetDateByName                OCI_ResultsetGetDateByName
#define OCI_GetDouble                    OCI_ResultsetGetDouble
#define OCI_GetDoubleByName              OCI_ResultsetGetDoubleByName
#define OCI_GetFile                      OCI_ResultsetGetFile
#define OCI_GetFileByName                OCI_ResultsetGetFileByName
#define OCI_GetFloat                     OCI_ResultsetGetFloat
#define OCI_GetFloatByName               OCI_ResultsetGetFloatByName
#define OCI_GetInt                       OCI_ResultsetGetInt32
#define OCI_GetIntByName                 OCI_ResultsetGetInt32ByName
#define OCI_GetInterval                  OCI_ResultsetGetInterval
#define OCI_GetIntervalByName            OCI_ResultsetGetIntervalByName
#define OCI_GetJson                      OCI_ResultsetGetJson
#define OCI_GetJsonByName                OCI_ResultsetGetJsonByName
#define OCI_GetLob                       OCI_ResultsetGetLob
#define OCI_GetLobByName                 OCI_ResultsetGetLobByName
#define OCI_GetLong                      OCI_ResultsetGetLong
#define OCI_GetLongByName                OCI_ResultsetGetLongByName
#define OCI_GetNumber                    OCI_ResultsetGetNumber
#define OCI_GetNumberByName              OCI_ResultsetGetNumberByName
#define OCI_GetObject                    OCI_ResultsetGetObject
#define OCI_GetObjectByName              OCI_ResultsetGetObjectByName
#define OCI_GetRaw                       OCI_ResultsetGetRaw
#define OCI_GetRawByName                 OCI_ResultsetGetRawByName
#define OCI_GetRef                       OCI_ResultsetGetReference
#define OCI_GetRefByName                 OCI_ResultsetGetReferenceByName
#define OCI_GetRowCount                  OCI_ResultsetGetRowCount
#define OCI_GetShort                     OCI_ResultsetGetInt16
#define OCI_GetShortByName               OCI_ResultsetGetInt16ByName
#define OCI_GetStatement                 OCI_ResultsetGetCursor
#define OCI_GetStatementByName           OCI_ResultsetGetCursorByName
#define OCI_GetString                    OCI_ResultsetGetString
#define OCI_GetStringByName              OCI_ResultsetGetStringByName
#define OCI_GetStruct                    OCI_ResultsetGetStruct
#define OCI_GetTimestamp                 OCI_ResultsetGetTimestamp
#define OCI_GetTimestampByName           OCI_ResultsetGetTimestampByName
#define OCI_GetUnsignedBigInt            OCI_ResultsetGetUInt64
#define OCI_GetUnsignedBigIntByName      OCI_ResultsetGetUInt64ByName
#define OCI_GetUnsignedInt               OCI_ResultsetGetUInt32
#define OCI_GetUnsignedIntByName         OCI_ResultsetGetUInt32ByName
#define OCI_GetUnsignedShort             OCI_ResultsetGetUInt16
#define OCI_GetUnsignedShortByName       OCI_ResultsetGetUInt16ByName
#define OCI_GetVector                    OCI_ResultsetGetVector
#define OCI_GetVectorByName              OCI_ResultsetGetVectorByName
#define OCI_GetXmlType                   OCI_ResultsetGetXmlType
#define OCI_GetXmlTypeByName             OCI_ResultsetGetXmlTypeByName
#define OCI_IsNull                       OCI_ResultsetIsColumnNull
#define OCI_IsNullByName                 OCI_ResultsetIsColumnNullByName
#define OCI_SetStructNumericType         OCI_ResultsetSetStructNumericType
#define OCI_SetStructNumericTypeByName   OCI_ResultsetSetStructNumericTypeByName

/* Soda */

#define OCI_SodaCollCreateIndex          OCI_SodaCollectionCreateIndex
#define OCI_SodaCollCursorFree           OCI_SodaCollectionCursorFree
#define OCI_SodaCollCursorGetNext        OCI_SodaCollectionCursorGetNext
#define OCI_SodaCollDocCount             OCI_SodaCollectionDocumentCount
#define OCI_SodaCollDocCountWithFilter   OCI_SodaCollectionDocumentCountWithFilter
#define OCI_SodaCollDrop                 OCI_SodaCollectionDrop
#define OCI_SodaCollDropIndex            OCI_SodaCollectionDropIndex
#define OCI_SodaCollFind                 OCI_SodaCollectionFind
#define OCI_SodaCollFindOne              OCI_SodaCollectionFindOne
#define OCI_SodaCollFindOneByKey         OCI_SodaCollectionFindOneByKey
#define OCI_SodaCollFree                 OCI_SodaCollectionFree
#define OCI_SodaCollFreeIndexNames       OCI_SodaCollectionFreeIndexNames
#define OCI_SodaCollGetConnection        OCI_SodaCollectionGetConnection
#define OCI_SodaCollGetDataGuide         OCI_SodaCollectionGetDataGuide
#define OCI_SodaCollGetMetadata          OCI_SodaCollectionGetMetadata
#define OCI_SodaCollGetName              OCI_SodaCollectionGetName
#define OCI_SodaCollInsertDoc            OCI_SodaCollectionInsertDocument
#define OCI_SodaCollInsertMany           OCI_SodaCollectionInsertMany
#define OCI_SodaCollIsMapped             OCI_SodaCollectionIsMapped
#define OCI_SodaCollListIndexes          OCI_SodaCollectionListIndexes
#define OCI_SodaCollRemove               OCI_SodaCollectionRemove
#define OCI_SodaCollRemoveOneByKey       OCI_SodaCollectionRemoveOneByKey
#define OCI_SodaCollReplaceOne           OCI_SodaCollectionReplaceOne
#define OCI_SodaCollReplaceOneByKey      OCI_SodaCollectionReplaceOneByKey
#define OCI_SodaCollSave                 OCI_SodaCollectionSave
#define OCI_SodaCollTruncate             OCI_SodaCollectionTruncate
#define OCI_SodaDbCreate                 OCI_SodaDatabaseCreate
#define OCI_SodaDbCreateColl             OCI_SodaDatabaseCreateCollection
#define OCI_SodaDbFree                   OCI_SodaDatabaseFree
#define OCI_SodaDbFreeCollectionNames    OCI_SodaDatabaseFreeCollectionNames
#define OCI_SodaDbGetCollectionNames     OCI_SodaDatabaseGetCollectionNames
#define OCI_SodaDbGetColls               OCI_SodaDatabaseGetCollections
#define OCI_SodaDbGetConnection          OCI_SodaDatabaseGetConnection
#define OCI_SodaDbOpenColl               OCI_SodaDatabaseOpenCollection
#define OCI_SodaDocCreateFromJson        OCI_SodaDocumentCreateFromJson
#define OCI_SodaDocCreateFromText        OCI_SodaDocumentCreateFromText
#define OCI_SodaDocCursorFree            OCI_SodaDocumentCursorFree
#define OCI_SodaDocCursorGetNext         OCI_SodaDocumentCursorGetNext
#define OCI_SodaDocFree                  OCI_SodaDocumentFree
#define OCI_SodaDocGetCreatedOn          OCI_SodaDocumentGetCreatedOn
#define OCI_SodaDocGetIsJson             OCI_SodaDocumentGetIsJson
#define OCI_SodaDocGetJson               OCI_SodaDocumentGetJson
#define OCI_SodaDocGetKey                OCI_SodaDocumentGetKey
#define OCI_SodaDocGetLastModified       OCI_SodaDocumentGetLastModified
#define OCI_SodaDocGetMediaType          OCI_SodaDocumentGetMediaType
#define OCI_SodaDocGetText               OCI_SodaDocumentGetText
#define OCI_SodaDocGetVersion            OCI_SodaDocumentGetVersion

/* Statement */

#define OCI_AllowRebinding               OCI_StatementAllowRebinding
#define OCI_BindArrayGetSize             OCI_StatementBindArrayGetSize
#define OCI_BindArrayOfBigInts           OCI_StatementBindInt64Array
#define OCI_BindArrayOfColls             OCI_StatementBindCollectionArray
#define OCI_BindArrayOfDates             OCI_StatementBindDateArray
#define OCI_BindArrayOfDoubles           OCI_StatementBindDoubleArray
#define OCI_BindArrayOfFiles             OCI_StatementBindFileArray
#define OCI_BindArrayOfFloats            OCI_StatementBindFloatArray
#define OCI_BindArrayOfIntervals         OCI_StatementBindIntervalArray
#define OCI_BindArrayOfInts              OCI_StatementBindInt32Array
#define OCI_BindArrayOfJsons             OCI_StatementBindJsonArray
#define OCI_BindArrayOfLobs              OCI_StatementBindLobArray
#define OCI_BindArrayOfNumbers           OCI_StatementBindNumberArray
#define OCI_BindArrayOfObjects           OCI_StatementBindObjectArray
#define OCI_BindArrayOfRaws              OCI_StatementBindRawArray
#define OCI_BindArrayOfRefs              OCI_StatementBindReferenceArray
#define OCI_BindArrayOfShorts            OCI_StatementBindInt16Array
#define OCI_BindArrayOfStrings           OCI_StatementBindStringArray
#define OCI_BindArrayOfTimestamps        OCI_StatementBindTimestampArray
#define OCI_BindArrayOfUnsignedBigInts   OCI_StatementBindUInt64Array
#define OCI_BindArrayOfUnsignedInts      OCI_StatementBindUInt32Array
#define OCI_BindArrayOfUnsignedShorts    OCI_StatementBindUInt16Array
#define OCI_BindArrayOfVectors           OCI_StatementBindVectorArray
#define OCI_BindArraySetSize             OCI_StatementBindArraySetSize
#define OCI_BindBigInt                   OCI_StatementBindInt64
#define OCI_BindBoolean                  OCI_StatementBindBoolean
#define OCI_BindColl                     OCI_StatementBindCollection
#define OCI_BindDate                     OCI_StatementBindDate
#define OCI_BindDouble                   OCI_StatementBindDouble
#define OCI_BindFile                     OCI_StatementBindFile
#define OCI_BindFloat                    OCI_StatementBindFloat
#define OCI_BindInt                      OCI_StatementBindInt32
#define OCI_BindInterval                 OCI_StatementBindInterval
#define OCI_BindJson                     OCI_StatementBindJson
#define OCI_BindLob                      OCI_StatementBindLob
#define OCI_BindLong                     OCI_StatementBindLong
#define OCI_BindNumber                   OCI_StatementBindNumber
#define OCI_BindObject                   OCI_StatementBindObject
#define OCI_BindRaw                      OCI_StatementBindRaw
#define OCI_BindRef                      OCI_StatementBindReference
#define OCI_BindShort                    OCI_StatementBindInt16
#define OCI_BindStatement                OCI_StatementBindCursor
#define OCI_BindString                   OCI_StatementBindString
#define OCI_BindTimestamp                OCI_StatementBindTimestamp
#define OCI_BindUnsignedBigInt           OCI_StatementBindUInt64
#define OCI_BindUnsignedInt              OCI_StatementBindUInt32
#define OCI_BindUnsignedShort            OCI_StatementBindUInt16
#define OCI_BindVector                   OCI_StatementBindVector
#define OCI_Describe                     OCI_StatementDescribe
#define OCI_DescribeFmt                  OCI_StatementDescribeFormat
#define OCI_Execute                      OCI_StatementExecutePrepared
#define OCI_ExecuteAndGetResultset       OCI_StatementExecutePreparedAndGetResultset
#define OCI_ExecuteStmt                  OCI_StatementExecute
#define OCI_ExecuteStmtAndGetResultset   OCI_StatementExecuteAndGetResultset
#define OCI_ExecuteStmtFmt               OCI_StatementExecuteFormat
#define OCI_GetAffectedRows              OCI_StatementGetAffectedRows
#define OCI_GetBatchError                OCI_StatementGetBatchError
#define OCI_GetBatchErrorCount           OCI_StatementGetBatchErrorCount
#define OCI_GetBind                      OCI_StatementGetBind
#define OCI_GetBindAllocation            OCI_StatementGetBindAllocation
#define OCI_GetBindByName                OCI_StatementGetBindByName
#define OCI_GetBindCount                 OCI_StatementGetBindCount
#define OCI_GetBindIndex                 OCI_StatementGetBindIndex
#define OCI_GetBindMode                  OCI_StatementGetBindMode
#define OCI_GetFetchMode                 OCI_StatementGetFetchMode
#define OCI_GetFetchSize                 OCI_StatementGetFetchSize
#define OCI_GetLongMode                  OCI_StatementGetLongMode
#define OCI_GetNextResultset             OCI_StatementGetNextResultset
#define OCI_GetParseBindNames            OCI_StatementGetParseBindNames
#define OCI_GetPieceSize                 OCI_StatementGetPieceSize
#define OCI_GetPrefetchMemory            OCI_StatementGetPrefetchMemory
#define OCI_GetPrefetchSize              OCI_StatementGetPrefetchSize
#define OCI_GetResultset                 OCI_StatementGetResultset
#define OCI_GetSQLCommand                OCI_StatementGetSqlCommand
#define OCI_GetSQLVerb                   OCI_StatementGetSqlVerb
#define OCI_GetSql                       OCI_StatementGetSql
#define OCI_GetSqlErrorPos               OCI_StatementGetSqlErrorPos
#define OCI_GetSqlIdentifier             OCI_StatementGetSqlIdentifier
#define OCI_GetStatementType             OCI_StatementGetType
#define OCI_IsRebindingAllowed           OCI_StatementIsRebindingAllowed
#define OCI_Parse                        OCI_StatementParse
#define OCI_ParseFmt                     OCI_StatementParseFormat
#define OCI_Prepare                      OCI_StatementPrepare
#define OCI_PrepareFmt                   OCI_StatementPrepareFormat
#define OCI_RegisterBigInt               OCI_StatementRegisterInt64
#define OCI_RegisterDate                 OCI_StatementRegisterDate
#define OCI_RegisterDouble               OCI_StatementRegisterDouble
#define OCI_RegisterFile                 OCI_StatementRegisterFile
#define OCI_RegisterFloat                OCI_StatementRegisterFloat
#define OCI_RegisterInt                  OCI_StatementRegisterInt32
#define OCI_RegisterInterval             OCI_StatementRegisterInterval
#define OCI_RegisterJson                 OCI_StatementRegisterJson
#define OCI_RegisterLob                  OCI_StatementRegisterLob
#define OCI_RegisterNumber               OCI_StatementRegisterNumber
#define OCI_RegisterObject               OCI_StatementRegisterObject
#define OCI_RegisterRaw                  OCI_StatementRegisterRaw
#define OCI_RegisterRef                  OCI_StatementRegisterReference
#define OCI_RegisterShort                OCI_StatementRegisterInt16
#define OCI_RegisterString               OCI_StatementRegisterString
#define OCI_RegisterTimestamp            OCI_StatementRegisterTimestamp
#define OCI_RegisterUnsignedBigInt       OCI_StatementRegisterUInt64
#define OCI_RegisterUnsignedInt          OCI_StatementRegisterUInt32
#define OCI_RegisterUnsignedShort        OCI_StatementRegisterUInt16
#define OCI_RegisterVector               OCI_StatementRegisterVector
#define OCI_ReleaseResultsets            OCI_StatementReleaseResultsets
#define OCI_SetBindAllocation            OCI_StatementSetBindAllocation
#define OCI_SetBindMode                  OCI_StatementSetBindMode
#define OCI_SetFetchMode                 OCI_StatementSetFetchMode
#define OCI_SetFetchSize                 OCI_StatementSetFetchSize
#define OCI_SetLongMode                  OCI_StatementSetLongMode
#define OCI_SetPieceSize                 OCI_StatementSetPieceSize
#define OCI_SetPrefetchMemory            OCI_StatementSetPrefetchMemory
#define OCI_SetPrefetchSize              OCI_StatementSetPrefetchSize

/* Subscription */

#define OCI_SubscriptionRegister         OCI_SubscriptionCreate

#endif /* OCI_NO_COMPAT */

#endif /* OCILIB_OCILIBC_COMPAT_H_INCLUDED */
