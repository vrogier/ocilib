/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2023 Vincent ROGIER <vince.rogier@ocilib.net>
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

/* IMPORTANT NOTICE
 *
 * This file contains explanations about Oracle and OCI technologies. 
 * OCILIB is a wrapper around OCI and thus exposes OCI features. 
 * The OCILIB documentation intends to explain Oracle / OCI concepts
 * and is naturally based on the official Oracle OCI documentation. 
 * 
 * Some parts of OCILIB documentation may include some information 
 * taken and adapted from the following Oracle documentations :
 *  - Oracle Call Interface Programmer's Guide
 *  - Oracle Streams - Advanced Queuing User's Guide
 */

#ifndef OCILIB_COMPAT_H_INCLUDED
#define OCILIB_COMPAT_H_INCLUDED

#include "ocilibc/api.h"

/**
 * @addtogroup OcilibCApi
 * @{
 *
 */

/* Compatibility with sources built with older versions of OCILIB */

/* macros added in version 2.3.0 */

#define OCI_CreateConnection                OCI_ConnectionCreate
#define OCI_FreeConnection                  OCI_ConnectionFree
#define OCI_CreateStatement                 OCI_StatementCreate
#define OCI_FreeStatement                   OCI_StatementFree

/* macros added in version 2.4.0 */

#define OCI_CreateTransaction               OCI_TransactionCreate
#define OCI_FreeTransaction                 OCI_TransactionFree
#define OCI_CreateHashTable                 OCI_HashCreate
#define OCI_FreeHashTable                   OCI_HashFree

/* macros added in version 3.0.0 */

#define OCI_GetColumnName                   OCI_ColumnGetName
#define OCI_GetColumnType                   OCI_ColumnGetType
#define OCI_GetColumnCharsetForm            OCI_ColumnGetCharsetForm
#define OCI_GetColumnSQLType                OCI_ColumnGetSQLType
#define OCI_GetColumnFullSQLType            OCI_ColumnGetFullSQLType
#define OCI_GetColumnSize                   OCI_ColumnGetSize
#define OCI_GetColumnScale                  OCI_ColumnGetScale
#define OCI_GetColumnPrecision              OCI_ColumnGetPrecision
#define OCI_GetColumnFractionnalPrecision   OCI_ColumnGetFractionnalPrecision
#define OCI_GetColumnLeadingPrecision       OCI_ColumnGetLeadingPrecision
#define OCI_GetColumnNullable               OCI_ColumnGetNullable
#define OCI_GetColumnCharUsed               OCI_ColumnGetCharUsed

#define OCI_GetFormatDate(s)                OCI_GetDefaultFormatDate(OCI_StatementGetConnection(s))
#define OCI_SetFormatDate(s, f)             OCI_SetDefaultFormatDate(OCI_StatementGetConnection( \
                                                                         s), f)

#define OCI_ERR_API                         OCI_ERR_ORACLE

/* macros added in version 3.2.0 */

#define OCI_ERR_NOT_SUPPORTED               OCI_ERR_DATATYPE_NOT_SUPPORTED
#define OCI_SCHEMA_TABLE                    OCI_TIF_TABLE
#define OCI_SCHEMA_VIEW                     OCI_TIF_VIEW
#define OCI_SCHEMA_TYPE                     OCI_TIF_TYPE

#define OCI_Schema                          OCI_TypeInfo

#define OCI_SchemaGet                       OCI_TypeInfoGet
#define OCI_SchemaFree                      OCI_TypeInfoFree
#define OCI_SchemaGetColumnCount            OCI_TypeInfoGetColumnCount
#define OCI_SchemaGetColumn                 OCI_TypeInfoGetColumn
#define OCI_SchemaGetName                   OCI_TypeInfoGetName

#define OCI_ColumnGetFractionnalPrecision   OCI_ColumnGetFractionalPrecision

/* macro added in version 3.3.0 */

/**
 * @brief
 * [OBSOLETE] Set the bind variable at the given index to null
 *
 * @param stmt  - OCI_Statement handle
 * @param index - Index of the bind variable
 *
 * @warning
 * This call is obsolete ! Use OCI_BindSetNull() instead.
 *
 * @note
 * There is no notion of null value in C.
 * It's necessary to explicitly tell Oracle that the bind has a null value.
 * It must be done before an OCI_Execute() call
 *
 * @warning
 * Index starts with 1
 *
 * @note
 * For handled based data types (non scalar types), OCILIB performs an extra
 * check on handles and set the bind status to null is the handle is null
 *
 * @return
 * TRUE on success otherwise FALSE
 */

#define OCI_SetNull(stmt, index)                                               \
    OCI_BindSetNull(OCI_GetBind(stmt, index))

/**
 * @brief
 * [OBSOLETE] Set the bind variable of the given name to null
 *
 * @param stmt  - OCI_Statement handle
 * @param name  - OCI_Bind variable name
 *
 * @warning
 * This call is obsolete ! Use OCI_BindSetNull() instead.
 *
 * @note
 * There is no notion of null value in C.
 * it's necessary to explicitly tell Oracle that the bind has a null value.
 * It must be done before an OCI_Execute() call
 *
 * @note
 * For handled based data types (non scalar types), OCILIB performs an extra
 * check on handles and set the bind status to null is the handle is null
 *
 * @return
 * TRUE on success otherwise FALSE
 */

#define OCI_SetNull2(stmt, name)                                               \
    OCI_BindSetNull(OCI_GetBind2(stmt, name))

/**
 * @brief
 * [OBSOLETE] Set to null the bind variable at the given position in an input array
 *
 * @param stmt     - OCI_Statement handle
 * @param index    - Index of the bind variable
 * @param position - Position in the array
 *
 * @warning
 * This call is obsolete ! Use OCI_BindSetNullAtPos() instead.
 *
 * @note
 * There is no notion of null value in C.
 * It's necessary to explicitly tell Oracle that the bind has a null value.
 * It must be done before an OCI_Execute() call
 *
 * @warning
 * Index and Position starts with 1
 *
 * @note
 * For handled based data types (non scalar types), OCILIB performs an extra
 * check on handles and set the bind status to null is the handle is null
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

#define OCI_SetNullAtPos(stmt, index, position)                                \
    OCI_BindSetNullAtPos(OCI_GetBind(stmt, index), position)

/**
 * @brief
 * [OBSOLETE] Set to null the bind variable of the given name in an input array
 *
 * @param stmt     - OCI_Statement handle
 * @param name     - OCI_Bind variable name
 * @param position - Position in the array
 *
 * @warning
 * This call is obsolete ! Use OCI_BindSetNullAtPos() instead.
 *
 * @note
 * There is no notion of null value in C.
 * It's necessary to explicitly tell Oracle that the bind has a null value.
 * It must be done before an OCI_Execute() call
 *
 * @warning
 * Position starts with 1
 *
 * @note
 * For handled based data types (non scalar types), OCILIB performs an extra
 * check on handles and set the bind status to null is the handle is null
 *
 * @return
 * TRUE on success otherwise FALSE
 *
 */

#define OCI_SetNullAtPos2(stmt, name, position)                                \
    OCI_BindSetNullAtPos(OCI_GetBind2(stmt, name), position)

/* macro added in version 3.4.0 */

#define OCI_8  OCI_8_1
#define OCI_9  OCI_9_0
#define OCI_10 OCI_10_1
#define OCI_11 OCI_11_1

/* macro added in version 3.6.0 */

#define OCI_CHAR_UNICODE  OCI_CHAR_WIDE
#define OCI_CSF_CHARSET   OCI_CSF_DEFAULT

/* macro added in version 3.7.0 */

#define OCI_ConnPool                OCI_Pool

#define OCI_ConnPoolCreate(db, us, pw, mo, mi, ma, in)                        \
    OCI_PoolCreate(db, us, pw, OCI_POOL_CONNECTION, mo, mi, ma, in)

#define OCI_ConnPoolGetConnection(p)                                          \
    OCI_PoolGetConnection(p, NULL)

#define OCI_ConnPoolFree            OCI_PoolFree
#define OCI_ConnPoolGetTimeout      OCI_PoolGetConnection
#define OCI_ConnPoolSetTimeout      OCI_PoolSetTimeout
#define OCI_ConnPoolGetNoWait       OCI_PoolGetNoWait
#define OCI_ConnPoolSetNoWait       OCI_PoolSetNoWait
#define OCI_ConnPoolGetBusyCount    OCI_PoolGetBusyCount
#define OCI_ConnPoolGetOpenedCount  OCI_PoolGetOpenedCount
#define OCI_ConnPoolGetMin          OCI_PoolGetMin
#define OCI_ConnPoolGetMax          OCI_PoolGetMax
#define OCI_ConnPoolGetIncrement    OCI_PoolGetIncrement

/* macro added in version 3.8.0 */

#define OCI_ObjectGetTimeStamp      OCI_ObjectGetTimestamp
#define OCI_ElemGetTimeStamp        OCI_ElemGetTimestamp
#define OCI_TimestampSysTimeStamp   OCI_TimestampSysTimestamp

/* macro added in version 4.0.0 */

#define OCI_CollSetAt               OCI_CollSetElem
#define OCI_CollGetAt               OCI_CollGetElem
#define OCI_CollGetAt2              OCI_CollGetElem2

#define OCI_GetCharsetMetaData      OCI_GetCharset
#define OCI_GetCharsetUserData      OCI_GetCharset
#define OCI_SIZE_TRACE_INF0         OCI_SIZE_TRACE_INFO

#define MT(x)    OTEXT(x)
#define mtext    otext
#define DT(x)    OTEXT(x)
#define dtext    otext

#define mtsdup          ostrdup
#define mtscpy          ostrcpy
#define mtsncpy         ostrncpy
#define mtscat          ostrcat
#define mtsncat         ostrncat
#define mtslen          ostrlen
#define mtscmp          ostrcmp
#define mtscasecmp      ostrcasecmp
#define mtsprintf       osprintf
#define mtstol          ostrtol
#define mtsscanf        osscanf

#define dtsdup          ostrdup
#define dtscpy          ostrcpy
#define dtsncpy         ostrncpy
#define dtscat          ostrcat
#define dtsncat         ostrncat
#define dtslen          ostrlen
#define dtscmp          ostrcmp
#define dtscasecmp      ostrcasecmp
#define dtsprintf       osprintf
#define dtstol          ostrtol
#define dtsscanf        osscanf

/* macro added in version 4.1.0 */

#define OCI_SetDefaultFormatDate(con, fmt)      OCI_SetFormat(con, OCI_FMT_DATE, fmt)
#define OCI_SetDefaultFormatNumeric(con, fmt)   OCI_SetFormat(con, OCI_FMT_NUMERIC, fmt)

#define OCI_GetDefaultFormatDate(con)           OCI_GetFormat(con, OCI_FMT_DATE)
#define OCI_GetDefaultFormatNumeric(con)        OCI_GetFormat(con, OCI_FMT_NUMERIC)

#define OCI_STRING_FORMAT_NUM_BIN               OCI_STRING_FORMAT_NUM_BDOUBLE

/**
 * @}
 */

#endif  /* OCILIB_COMPAT_H_INCLUDED */
