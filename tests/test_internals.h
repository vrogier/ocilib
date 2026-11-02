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

#ifndef OCILIB_TESTS_INTERNALS_H_INCLUDED
#define OCILIB_TESTS_INTERNALS_H_INCLUDED

#include "ocilib.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*
 * White-box helpers for exercising JSON DOM scalar representations that no
 * public API produces (for example Oracle extended OSON scalars). They build
 * real DOM nodes through the same OCI JsonDom calls the library uses; only
 * the scalar type code differs.
 */

OCI_JsonNode * OCI_TestJsonNodeCreateOraBoolean(OCI_Json *json, OCI_Boolean value);

OCI_JsonNode * OCI_TestJsonNodeCreateOpaqueScalar(OCI_Json *json);

/* reset a node's resolved type so OCI_JsonNodeGetType resolves it lazily */
void OCI_TestJsonNodeResetType(OCI_JsonNode *node);

/* node whose DOM handle is NULL (type resolves through the default branch) */
OCI_JsonNode * OCI_TestJsonNodeCreateNullHandle(OCI_Json *json);

/* JZNVAL_NUMBER scalar whose value is stored as text */
OCI_JsonNode * OCI_TestJsonNodeCreateNumberText(OCI_Json *json, const otext *value);

/*
 * White-box helpers for OCI_Column code paths that no public API can reach:
 * legacy/rare Oracle SQL codes (SQLT_CFILE, SQLT_TAB, undocumented PL/SQL
 * codes), unknown vector formats, and out-of-range attribute info calls.
 */

enum OCI_TestOracleCode
{
    OCI_TEST_CODE_CFILE = 0,
    OCI_TEST_CODE_TAB,
    OCI_TEST_CODE_INT,
    OCI_TEST_CODE_UIN,
    OCI_TEST_CODE_FLT,
    OCI_TEST_CODE_CURSOR,
    OCI_TEST_CODE_UNDOC_REC,
    OCI_TEST_CODE_REC,
    OCI_TEST_CODE_PLS_INTEGER,
    OCI_TEST_CODE_BIN_INTEGER,
    OCI_TEST_CODE_NTY,
    OCI_TEST_CODE_OPAQUE,
    OCI_TEST_CODE_UNKNOWN
};

const otext *  OCI_TestColumnVectorFormat(OCI_VectorFormat format);
const otext *  OCI_TestColumnGetSqlType(OCI_Int32 oracle_code);
OCI_UInt32   OCI_TestColumnGetFullSqlType(OCI_Int32 oracle_code, otext *buffer, OCI_UInt32 len);
OCI_Boolean    OCI_TestColumnGetAttributeInfo(OCI_UInt32 count, OCI_UInt32 index, OCI_Int32 datatype, OCI_UInt32 subtype, OCI_Size *size, OCI_Size *align);
OCI_Boolean    OCI_TestColumnMapInfo(OCI_Int32 oracle_code, OCI_Int32 *datatype, OCI_UInt32 *subtype);

/*
 * White-box helper for OcilibBindCreate() data-type arms that no public API
 * can reach: OCI_BindColl/OCI_BindArrayOfColls, OCI_BindArrayOfRefs and
 * OCI_BindArrayOfVectors forbid NULL data and there is no OCI_RegisterColl(),
 * so the internal-allocation paths for collections, REFs, files and vectors
 * cannot be triggered through the public entry points.
 */

enum OCI_TestBindKind
{
    OCI_TEST_BIND_COLL_SCALAR = 0,
    OCI_TEST_BIND_COLL_ARRAY,
    OCI_TEST_BIND_REF_ARRAY,
    OCI_TEST_BIND_FILE_ARRAY,
    OCI_TEST_BIND_VECTOR_ARRAY
};

OCI_Bind * OCI_TestBindCreateNull(OCI_Statement *stmt, const otext *name, OCI_Int32 kind, OCI_TypeInfo *typinf, OCI_UInt32 nbelem);

/* returns the internal bind buffer size (bnd->size) */
OCI_UInt32 OCI_TestBindBufferSize(OCI_Bind *bnd);

/* returns sizeof(dbtext) for the current build */
OCI_UInt32 OCI_TestBindDbTextSize(void);

/*
 * White-box helpers for environment/connection flags and version numbers.
 * They let tests reach legacy code paths gated on old Oracle versions or on
 * OCI features that are always enabled at runtime on modern clients
 * (for example the ub8 LOB calls or scrollable cursors).
 * Each setter returns the previous value so tests can restore it.
 */

OCI_Boolean  OCI_TestEnvSetUseLobUb8(OCI_Connection *con, OCI_Boolean value);
OCI_UInt32 OCI_TestEnvSetVersionRuntime(OCI_Connection *con, OCI_UInt32 value);
OCI_Boolean  OCI_TestEnvSetScrollableCursors(OCI_Connection *con, OCI_Boolean value);
OCI_UInt32 OCI_TestConSetServerVersion(OCI_Connection *con, OCI_UInt32 value);
OCI_UInt32 OCI_TestEnvSetCharset(OCI_Connection *con, OCI_UInt32 value);

/* directly calls the internal OcilibResultsetInitialize() entry point */
OCI_Boolean  OCI_TestResultsetInitialize(OCI_Resultset *rs);

/*
 * White-box helpers for connection internals: reset the cached server
 * version string/number so the version getters fetch it again (through the
 * legacy OCIServerVersion parse path when the runtime version is forced
 * below 18.1), and set the connection state to reach the re-logon arm of
 * OCI_SetPassword().
 */

/* mirrors the internal OCI_CONN_* connection states (src/defs.h) */
enum OCI_TestConnState
{
    OCI_TEST_CONN_ALLOCATED = 1,
    OCI_TEST_CONN_ATTACHED  = 2,
    OCI_TEST_CONN_LOGGED    = 3
};

void          OCI_TestConResetServerVersion(OCI_Connection *con);
OCI_Int32           OCI_TestConSetState(OCI_Connection *con, OCI_Int32 state);
void          OCI_TestConReleaseLogonHandles(OCI_Connection *con);
OCI_UInt32  OCI_TestConSetNbFiles(OCI_Connection *con, OCI_UInt32 value);

/*
 * White-box helpers for OcilibNumberTranslateValue() and
 * OcilibNumberToStringInternal() conversion arms that no public API can
 * produce (for example real->OCINumber or OCI_Int16/OCI_Int32 text rendering).
 */

OCI_Boolean   OCI_TestNumberTranslate(OCI_Number *num, void *in_value, OCI_UInt32 in_type, void *out_value, OCI_UInt32 out_type);
OCI_Boolean   OCI_TestNumberToString(OCI_Number *num, void *value, OCI_UInt32 type, otext *out_value, OCI_Int32 out_value_size);

/*
 * White-box helpers for string utilities (src/stringutils.c): internal
 * conversion, translation and naming functions whose argument combinations
 * cannot be produced through the public API.
 */

OCI_Size        OCI_TestStringLength(const void *ptr, OCI_Size size_elem);
OCI_UInt32  OCI_TestStringBinaryToString(const unsigned char *binary, OCI_UInt32 binary_size, otext *buffer);
OCI_Boolean   OCI_TestStringRequestBuffer(OCI_Connection *con, otext **buffer, OCI_UInt32 *buffer_size, OCI_UInt32 request_size);
void          OCI_TestStringTranslate(void *src, void *dst, OCI_Int32 char_count, OCI_Size size_in, OCI_Size size_out);
void        * OCI_TestStringGetDBStringWide(OCI_Connection *con, const void *src_utf32, OCI_Int32 *size);
void          OCI_TestStringReleaseDBStringWide(OCI_Connection *con, void *str);
char        * OCI_TestStringAnsiDuplicate(OCI_Connection *con, const char *src);
void          OCI_TestStringAnsiFree(OCI_Connection *con, char *str);
OCI_Int32           OCI_TestStringCaseCompare(const otext *str1, const otext *str2);
OCI_Int32           OCI_TestStringAnsiCaseCompare(const char *str1, const char *str2);
OCI_UInt32  OCI_TestStringGetTypeName(const otext *source, otext *dest, OCI_UInt32 length);
OCI_UInt32  OCI_TestStringGetFullTypeName(const otext *schema, const otext *package, const otext *type, const otext *link, otext *name, OCI_UInt32 length);
OCI_UInt32  OCI_TestStringAddToBuffer(otext *buffer, OCI_UInt32 offset, const otext *str, OCI_UInt32 length, OCI_Boolean check_quote);
OCI_UInt32  OCI_TestStringGetFromType(OCI_Connection *con, OCI_Int32 datatype, OCI_UInt32 subtype, void *data, OCI_UInt32 data_size, otext *buffer, OCI_UInt32 buffer_size, OCI_Boolean quote);
OCI_XmlType * OCI_TestXmlTypeCreate(OCI_Statement *stmt);
OCI_Boolean   OCI_TestXmlTypeFree(OCI_XmlType *xmltype);

/*
 * OCI fault injection: arm a fault on an env->api dispatch entry so that the
 * 'countdown'-th subsequent call to it returns OCI_ERROR. The original entry
 * is restored automatically when the fault fires (or explicitly through
 * OCI_TestApiRestore). This lets tests cover the OCI failure arms of the
 * CHECK_OCI / CHECK_ATTRIB_* macros that cannot be triggered through the
 * public API on a healthy connection.
 */

#define OCI_TEST_API_ATTR_GET            1
#define OCI_TEST_API_ATTR_SET            2
#define OCI_TEST_API_RAW_ASSIGN_BYTES    3
#define OCI_TEST_API_SUBSCR_REGISTER     4
#define OCI_TEST_API_SUBSCR_UNREGISTER   5
#define OCI_TEST_API_TRANS_START         6
#define OCI_TEST_API_TRANS_DETACH        7
#define OCI_TEST_API_TRANS_PREPARE       8
#define OCI_TEST_API_TRANS_FORGET        9
#define OCI_TEST_API_DESCRIBE_ANY        10
#define OCI_TEST_API_PARAM_GET           11
#define OCI_TEST_API_TYPE_BY_NAME        12
#define OCI_TEST_API_CONN_POOL_DESTROY   13
#define OCI_TEST_API_SESS_POOL_DESTROY   14
#define OCI_TEST_API_SODA_DROP           15
#define OCI_TEST_API_SODA_TRUNCATE       16
#define OCI_TEST_API_SODA_INSERT         17
#define OCI_TEST_API_SODA_FIND           18
#define OCI_TEST_API_SODA_FIND_ONE       19
#define OCI_TEST_API_SODA_DOC_GET_NEXT   20
#define OCI_TEST_API_SODA_DOC_COUNT      21
#define OCI_TEST_API_SODA_REPL_ONE       22
#define OCI_TEST_API_SODA_REMOVE         23
#define OCI_TEST_API_SODA_INSERT_AND_GET 24
#define OCI_TEST_API_SERVER_DETACH       25

void OCI_TestFailApiCall(OCI_Environment *env, OCI_Int32 api, OCI_Int32 countdown);
void OCI_TestApiRestore(OCI_Environment *env);

/*
 * White-box helpers for internal subsystems that have no public entry point
 * (hash buckets, environment variable lookup, runtime version detection).
 */

void *       OCI_TestHashCreate(OCI_Environment *env, OCI_UInt32 size, OCI_Int32 type);
OCI_Boolean  OCI_TestHashAddString(void *table, const otext *key, const otext *value);
OCI_Int32          OCI_TestHashEntryValueCount(void *table, OCI_UInt32 index);
OCI_Boolean  OCI_TestHashFree(void *table);

OCI_Int32          OCI_TestEnvGetVariable(OCI_Environment *env, const char *name, char *value, OCI_UInt32 size);
OCI_UInt64     OCI_TestEnvGetStats(OCI_Environment *env, OCI_UInt32 stat);
OCI_Boolean  OCI_TestEnvReinitLoaded(OCI_Environment *env);

OCI_Boolean  OCI_TestEnvGetFormat(OCI_UInt32 type);

/* logger internals: OCI_TestLogReset() drains the logger reference count so
   that the next OCI_TestLogInit() re-parses the OCILIB_LOG* environment
   variables. Always pair them and finish a log test with a clean reset. */

void          OCI_TestLogReset(void);
OCI_Boolean   OCI_TestLogInit(void);
OCI_Boolean   OCI_TestLogIsEnabled(OCI_UInt32 category, OCI_UInt32 level);
void          OCI_TestLogSetHandler(POCI_LOG_HANDLER handler);
void          OCI_TestLogWrite(OCI_Environment *env, OCI_UInt32 category, OCI_UInt32 level,
                               const char *function, const otext *message);
const char *  OCI_TestLogLevelName(OCI_UInt32 level);
const char *  OCI_TestLogCategoryName(OCI_UInt32 category);
OCI_Boolean   OCI_TestLogStrEquals(const char *value, const char *name);
OCI_Int32           OCI_TestLogCodepointToUTF8(OCI_UInt32 code, char *dst, OCI_Int32 dst_size);
OCI_Int32           OCI_TestLogNativeToUTF8(const otext *src, char *dst, OCI_Int32 dst_size);

#ifdef __cplusplus
}
#endif

#endif /* OCILIB_TESTS_INTERNALS_H_INCLUDED */
