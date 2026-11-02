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
 * White-box test helpers. This file is compiled as C and includes the library
 * internal headers because it exercises JSON DOM scalar representations that
 * the public C API cannot produce (JZNVAL_ORA_BOOLEAN, opaque/unknown scalars).
 * Nodes are built through the same OCI JsonDom calls used by the library.
 */

#include "jsonnode.h"
#include "column.h"
#include "bind.h"
#include "hash.h"
#include "helpers.h"
#include "memory.h"
#include "number.h"
#include "resultset.h"
#include "statement.h"
#include "stringutils.h"
#include "xmltype.h"
#include "environment.h"
#include "log.h"
#include "test_internals.h"

OCI_JsonNode * OCI_TestJsonNodeCreateOraBoolean
(
    OCI_Json    *json,
    OCI_Boolean  value
)
{
    jznScalarVal sval;
    jznDomNode  *hnd = NULL;
    ub1          flag;

    if (NULL == json || NULL == json->jdoc)
    {
        return NULL;
    }

    /* JZNVAL_ORA_BOOLEAN is an Oracle extended scalar that only the OSON
       decoder normally produces; JsonDomNewScalar canonicalizes it to
       JZNVAL_TRUE/FALSE. To exercise the library's ORA_BOOLEAN code path,
       create a binary scalar node and patch its type byte in place: the
       jznDomScalar layout observed from JsonDomNewScalar results is
       { links[12], flags, flags, type, pad, inline payload[8], length } */

    flag = (ub1) (0 != value);

    memset(&sval, 0, sizeof(sval));
    sval.type_jznScalarVal      = JZNVAL_ORA_NUMBER;
    sval.oranumval_jznScalarVal = &flag;
    sval.oranumlen_jznScalarVal = (ub4) sizeof(flag);

    hnd = (jznDomNode *) JsonDomNewScalar(json->jdoc, &sval);
    if (NULL == hnd)
    {
        return NULL;
    }

    ((ub1 *) hnd)[14] = (ub1) JZNVAL_ORA_BOOLEAN;

    return OcilibJsonNodeInitialize(json, hnd, OCI_JNT_NULL);
}

void OCI_TestJsonNodeResetType
(
    OCI_JsonNode *node
)
{
    if (NULL != node)
    {
        node->type = OCI_JNT_NULL;
    }
}

OCI_JsonNode * OCI_TestJsonNodeCreateNullHandle
(
    OCI_Json *json
)
{
    if (NULL == json || NULL == json->jdoc)
    {
        return NULL;
    }

    return OcilibJsonNodeInitialize(json, NULL, OCI_JNT_NULL);
}

OCI_JsonNode * OCI_TestJsonNodeCreateNumberText
(
    OCI_Json    *json,
    const otext *value
)
{
    jznScalarVal sval;
    jznDomNode  *hnd = NULL;
    oratext     *str;
    OCI_Size       len;
    OCI_Size       i;

    if (NULL == json || NULL == json->jdoc || NULL == value)
    {
        return NULL;
    }

    /* JZNVAL_NUMBER nodes carry their value as text; the OCI text parser
       canonicalizes them to JZNVAL_ORA_NUMBER, so build a string scalar and
       patch its type byte (see OCI_TestJsonNodeCreateOraBoolean) */

    len = ostrlen(value);
    str = (oratext *) malloc(len + 1);
    if (NULL == str)
    {
        return NULL;
    }
    for (i = 0; i < len; i++)
    {
        str[i] = (oratext) value[i];
    }
    str[len] = 0;

    memset(&sval, 0, sizeof(sval));
    sval.type_jznScalarVal = JZNVAL_STRING;
    sval.val_jznScalarVal  = str;
    sval.len_jznScalarVal  = (ub4) len;

    hnd = (jznDomNode *) JsonDomNewScalar(json->jdoc, &sval);
    free(str);
    if (NULL == hnd)
    {
        return NULL;
    }

    ((ub1 *) hnd)[14] = (ub1) JZNVAL_NUMBER;

    return OcilibJsonNodeInitialize(json, hnd, OCI_JNT_NULL);
}

OCI_JsonNode * OCI_TestJsonNodeCreateOpaqueScalar
(
    OCI_Json *json
)
{
    jznScalarVal sval;
    jznDomNode  *hnd = NULL;

    if (NULL == json || NULL == json->jdoc)
    {
        return NULL;
    }

    memset(&sval, 0, sizeof(sval));
    sval.type_jznScalarVal = JZNVAL_OPAQUE;

    hnd = (jznDomNode *) JsonDomNewScalar(json->jdoc, &sval);
    if (NULL == hnd)
    {
        return NULL;
    }

    return OcilibJsonNodeInitialize(json, hnd, OCI_JNT_NULL);
}

/*
 * Column internals helpers: build a fabricated OCI_Column on the stack and
 * call internal entry points directly. This is only possible because the
 * functions below do not dereference col->con / col->env on the paths used.
 */

static ub2 OCI_TestColumnOracleCode
(
    OCI_Int32 oracle_code
)
{
    switch (oracle_code)
    {
        case OCI_TEST_CODE_CFILE:       return (ub2) SQLT_CFILE;
        case OCI_TEST_CODE_TAB:         return (ub2) SQLT_TAB;
        case OCI_TEST_CODE_INT:         return (ub2) SQLT_INT;
        case OCI_TEST_CODE_UIN:         return (ub2) SQLT_UIN;
        case OCI_TEST_CODE_FLT:         return (ub2) SQLT_FLT;
        case OCI_TEST_CODE_CURSOR:      return (ub2) SQLT_CUR;
        case OCI_TEST_CODE_UNDOC_REC:   return (ub2) SQLT_UNDOCUMENTED_REC;
        case OCI_TEST_CODE_REC:         return (ub2) SQLT_REC;
        case OCI_TEST_CODE_PLS_INTEGER: return (ub2) OCI_TYPECODE_PLS_INTEGER;
        case OCI_TEST_CODE_BIN_INTEGER: return (ub2) SQLT_UNDOCUMENTED_BIN_INTEGER;
        case OCI_TEST_CODE_NTY:         return (ub2) SQLT_NTY;
        case OCI_TEST_CODE_OPAQUE:      return (ub2) SQLT_OPAQUE_TYPE;
        default:                        return (ub2) 0xFFFF;
    }
}

const otext * OCI_TestColumnVectorFormat
(
    OCI_VectorFormat format
)
{
    return OcilibColumnGetVectorFormatString(format);
}

const otext * OCI_TestColumnGetSqlType
(
    OCI_Int32 oracle_code
)
{
    OCI_Column col;

    memset(&col, 0, sizeof(col));
    col.sqlcode = OCI_TestColumnOracleCode(oracle_code);

    return OcilibColumnGetSqlType(&col);
}

OCI_UInt32 OCI_TestColumnGetFullSqlType
(
    OCI_Int32           oracle_code,
    otext        *buffer,
    OCI_UInt32  len
)
{
    OCI_Column col;

    memset(&col, 0, sizeof(col));
    col.sqlcode = OCI_TestColumnOracleCode(oracle_code);

    return OcilibColumnGetFullSqlType(&col, buffer, len);
}

OCI_Boolean OCI_TestColumnGetAttributeInfo
(
    OCI_UInt32 count,
    OCI_UInt32 index,
    OCI_Int32          datatype,
    OCI_UInt32 subtype,
    OCI_Size      *size,
    OCI_Size      *align
)
{
    OCI_Column col;

    memset(&col, 0, sizeof(col));
    col.datatype = (OCI_SqlType) datatype;
    col.subtype  = (OCI_SubType) subtype;

    return OcilibColumnGetAttributeInfo(&col, count, index, size, align);
}

/*
 * OcilibColumnMapInfo() only needs con->env->nls_utf8 on the code paths used
 * here, so zero-initialized stubs are sufficient.
 */

OCI_Boolean OCI_TestColumnMapInfo
(
    OCI_Int32           oracle_code,
    OCI_Int32          *datatype,
    OCI_UInt32 *subtype
)
{
    static OCI_Environment env_stub;
    static OCI_Connection  con_stub;

    OCI_Column col;
    OCI_Boolean res;

    memset(&col, 0, sizeof(col));
    memset(&env_stub, 0, sizeof(env_stub));
    memset(&con_stub, 0, sizeof(con_stub));

    con_stub.env = &env_stub;
    col.sqlcode  = OCI_TestColumnOracleCode(oracle_code);

    res = OcilibColumnMapInfo(&col, &con_stub, NULL);

    if (datatype)
    {
        *datatype = (OCI_Int32) col.datatype;
    }
    if (subtype)
    {
        *subtype = (OCI_UInt32) col.subtype;
    }

    return res;
}

/*
 * OcilibBindCreate() is internal; these calls exercise the internal-allocation
 * data-type arms that the public bind/register entry points cannot produce
 * with NULL data.
 */

OCI_Bind * OCI_TestBindCreateNull
(
    OCI_Statement *stmt,
    const otext   *name,
    OCI_Int32            kind,
    OCI_TypeInfo  *typinf,
    OCI_UInt32   nbelem
)
{
    switch (kind)
    {
        case OCI_TEST_BIND_COLL_SCALAR:
        {
            return OcilibBindCreate(stmt, NULL, name, OCI_BIND_INPUT, sizeof(void *),
                                    OCI_CDT_COLLECTION, SQLT_NTY, 0, typinf, 0);
        }
        case OCI_TEST_BIND_COLL_ARRAY:
        {
            return OcilibBindCreate(stmt, NULL, name, OCI_BIND_INPUT, sizeof(OCIColl *),
                                    OCI_CDT_COLLECTION, SQLT_NTY, 0, typinf, nbelem);
        }
        case OCI_TEST_BIND_REF_ARRAY:
        {
            return OcilibBindCreate(stmt, NULL, name, OCI_BIND_INPUT, sizeof(OCIRef *),
                                    OCI_CDT_REF, SQLT_REF, 0, typinf, nbelem);
        }
        case OCI_TEST_BIND_FILE_ARRAY:
        {
            return OcilibBindCreate(stmt, NULL, name, OCI_BIND_INPUT, sizeof(OCILobLocator *),
                                    OCI_CDT_FILE, (OCI_SqlType) OcilibExternalSubTypeToSQLType(OCI_CDT_FILE, OCI_BFILE),
                                    OCI_BFILE, NULL, nbelem);
        }
#if OCI_VERSION_COMPILE >= OCI_23_4
        case OCI_TEST_BIND_VECTOR_ARRAY:
        {
            return OcilibBindCreate(stmt, NULL, name, OCI_BIND_INPUT, sizeof(OCIVector *),
                                    OCI_CDT_VECTOR, SQLT_VEC, 0, NULL, nbelem);
        }
#endif
        default:
        {
            return NULL;
        }
    }
}

OCI_UInt32 OCI_TestBindBufferSize
(
    OCI_Bind *bnd
)
{
    return bnd ? (OCI_UInt32) bnd->size : 0;
}

OCI_UInt32 OCI_TestBindDbTextSize
(
    void
)
{
    return (OCI_UInt32) sizeof(dbtext);
}

/*
 * Flag / version flippers for reaching legacy code paths. They return the
 * previous value so the test can restore it once the legacy path has run.
 */

OCI_Boolean OCI_TestEnvSetUseLobUb8
(
    OCI_Connection *con,
    OCI_Boolean     value
)
{
    const OCI_Boolean old = con->env->use_lob_ub8;

    con->env->use_lob_ub8 = value;

    return old;
}

OCI_UInt32 OCI_TestEnvSetVersionRuntime
(
    OCI_Connection *con,
    OCI_UInt32    value
)
{
    const OCI_UInt32 old = con->env->version_runtime;

    con->env->version_runtime = value;

    return old;
}

OCI_Boolean OCI_TestEnvSetScrollableCursors
(
    OCI_Connection *con,
    OCI_Boolean     value
)
{
    const OCI_Boolean old = con->env->use_scrollable_cursors;

    con->env->use_scrollable_cursors = value;

    return old;
}

OCI_UInt32 OCI_TestConSetServerVersion
(
    OCI_Connection *con,
    OCI_UInt32    value
)
{
    const OCI_UInt32 old = con->ver_num;

    con->ver_num = value;

    return old;
}

OCI_UInt32 OCI_TestEnvSetCharset
(
    OCI_Connection *con,
    OCI_UInt32    value
)
{
    const OCI_UInt32 old = con->env->charset;

    con->env->charset = value;

    return old;
}

OCI_Boolean OCI_TestResultsetInitialize
(
    OCI_Resultset *rs
)
{
    return OcilibResultsetInitialize(rs);
}

/*
 * Connection internals: reset the cached server version so the version
 * getters fetch it again, and override the connection state.
 */

void OCI_TestConResetServerVersion
(
    OCI_Connection *con
)
{
    OcilibMemoryFree(con->env, con->ver_str);

    con->ver_str = NULL;
    con->ver_num = 0;
}

OCI_Int32 OCI_TestConSetState
(
    OCI_Connection *con,
    OCI_Int32             state
)
{
    const OCI_Int32 old = con->cstate;

    con->cstate = state;

    return old;
}

/* release the session/service-context handles so a second logon through
   OcilibConnectionLogonRegular() does not leak them */

void OCI_TestConReleaseLogonHandles
(
    OCI_Connection *con
)
{
    if (NULL != con->handle)
    {
        OcilibMemoryFreeHandle(con->env, con->handle, OCI_HTYPE_SVCCTX);
        con->handle = NULL;
    }

    if (NULL != con->ses)
    {
        OcilibMemoryFreeHandle(con->env, con->ses, OCI_HTYPE_SESSION);
        con->ses = NULL;
    }
}

OCI_UInt32 OCI_TestConSetNbFiles
(
    OCI_Connection *con,
    OCI_UInt32    value
)
{
    const OCI_UInt32 old = con->nb_files;

    con->nb_files = value;

    return old;
}

/*
 * Number conversion matrix: OcilibNumberTranslateValue() arms that no public
 * API produces (real->OCINumber, integer->real, ...) and binary-type text
 * rendering through OcilibNumberToStringInternal().
 */

OCI_Boolean OCI_TestNumberTranslate
(
    OCI_Number  *num,
    void        *in_value,
    OCI_UInt32 in_type,
    void        *out_value,
    OCI_UInt32 out_type
)
{
    return OcilibNumberTranslateValue(num->env, in_value, in_type, out_value, out_type);
}

OCI_Boolean OCI_TestNumberToString
(
    OCI_Number  *num,
    void        *value,
    OCI_UInt32 type,
    otext       *out_value,
    OCI_Int32          out_value_size
)
{
    return OcilibNumberToStringInternal(num->env, value, type, out_value, out_value_size, NULL);
}

/*
 * String utility internals: expose the pure conversion/translation helpers
 * and the per-datatype conversion switch so tests can reach the arms that no
 * public API call produces (non-buffer sizing calls, NULL payloads, quoting
 * passes, unsupported datatype fallback, ...).
 */

OCI_Size OCI_TestStringLength
(
    const void *ptr,
    OCI_Size      size_elem
)
{
    return OcilibStringLength(ptr, size_elem);
}

OCI_UInt32 OCI_TestStringBinaryToString
(
    const unsigned char *binary,
    OCI_UInt32         binary_size,
    otext               *buffer
)
{
    return OcilibStringBinaryToString(binary, binary_size, buffer);
}

OCI_Boolean OCI_TestStringRequestBuffer
(
    OCI_Connection *con,
    otext         **buffer,
    OCI_UInt32   *buffer_size,
    OCI_UInt32    request_size
)
{
    return OcilibStringRequestBuffer(con ? con->env : NULL, buffer, buffer_size, request_size);
}

void OCI_TestStringTranslate
(
    void  *src,
    void  *dst,
    OCI_Int32    char_count,
    OCI_Size size_in,
    OCI_Size size_out
)
{
    OcilibStringTranslate(src, dst, char_count, size_in, size_out);
}

/* the use_wide_char_conv arms are only compiled-in on platforms with a
   4-byte wchar_t; flipping the flag lets Windows builds exercise the
   UTF32<->UTF16 paths with correctly sized buffers */

void * OCI_TestStringGetDBStringWide
(
    OCI_Connection *con,
    const void     *src_utf32,
    OCI_Int32            *size
)
{
    const OCI_Boolean old = con->env->use_wide_char_conv;

    dbtext *str;

    con->env->use_wide_char_conv = TRUE;
    str = OcilibStringGetDBString(con->env, (const otext *) src_utf32, size);
    con->env->use_wide_char_conv = old;

    return str;
}

void OCI_TestStringReleaseDBStringWide
(
    OCI_Connection *con,
    void           *str
)
{
    const OCI_Boolean old = con->env->use_wide_char_conv;

    con->env->use_wide_char_conv = TRUE;
    OcilibStringReleaseDBString(con->env, (dbtext *) str);
    con->env->use_wide_char_conv = old;
}

char * OCI_TestStringAnsiDuplicate
(
    OCI_Connection *con,
    const char     *src
)
{
    return OcilibStringAnsiDuplicate(con ? con->env : NULL, src);
}

void OCI_TestStringAnsiFree
(
    OCI_Connection *con,
    char           *str
)
{
    OcilibMemoryFree(con ? con->env : NULL, str);
}

OCI_Int32 OCI_TestStringCaseCompare
(
    const otext *str1,
    const otext *str2
)
{
    return OcilibStringCaseCompare(str1, str2);
}

OCI_Int32 OCI_TestStringAnsiCaseCompare
(
    const char *str1,
    const char *str2
)
{
    return OcilibStringAnsiCaseCompare(str1, str2);
}

OCI_UInt32 OCI_TestStringGetTypeName
(
    const otext *source,
    otext       *dest,
    OCI_UInt32 length
)
{
    return OcilibStringGetTypeName(source, dest, length);
}

OCI_UInt32 OCI_TestStringGetFullTypeName
(
    const otext *schema,
    const otext *package,
    const otext *type,
    const otext *link,
    otext       *name,
    OCI_UInt32 length
)
{
    return OcilibStringGetFullTypeName(schema, package, type, link, name, length);
}

OCI_UInt32 OCI_TestStringAddToBuffer
(
    otext          *buffer,
    OCI_UInt32    offset,
    const otext    *str,
    OCI_UInt32    length,
    OCI_Boolean     check_quote
)
{
    return OcilibStringAddToBuffer(buffer, offset, str, length, check_quote);
}

/* fabricates a minimal OCI_Column so tests can drive every datatype arm of
   OcilibStringGetFromType() directly */

OCI_UInt32 OCI_TestStringGetFromType
(
    OCI_Connection *con,
    OCI_Int32             datatype,
    OCI_UInt32    subtype,
    void           *data,
    OCI_UInt32    data_size,
    otext          *buffer,
    OCI_UInt32    buffer_size,
    OCI_Boolean     quote
)
{
    OCI_Column col;

    memset(&col, 0, sizeof(col));
    col.datatype = (OCI_SqlType) datatype;
    col.subtype  = (OCI_SubType) subtype;

    return OcilibStringGetFromType(con, &col, data, data_size, buffer, buffer_size, quote);
}

/* OcilibXmlTypeInitialize() is the real allocation entry point (used by the
   fetch machinery); there is no public create API */
OCI_XmlType * OCI_TestXmlTypeCreate
(
    OCI_Statement *stmt
)
{
    return stmt ? OcilibXmlTypeInitialize(stmt->con, stmt, NULL, NULL) : NULL;
}

OCI_Boolean OCI_TestXmlTypeFree
(
    OCI_XmlType *xmltype
)
{
    return OcilibXmlTypeFree(xmltype);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI fault injection
 *
 * A single generic hook is enough: on x64 every OCI call argument occupies an
 * 8-byte slot, so a wrapper taking ten void* parameters is ABI-compatible with
 * any OCI entry point of at most ten arguments (extra arguments are ignored by
 * the callee). The hook returns OCI_ERROR once its countdown reaches zero and
 * restores the original dispatch entry at the same time.
 * --------------------------------------------------------------------------------------------- */

typedef sword (* POCI_TEST_GENERIC_CALL)
(
    void *a1, void *a2, void *a3, void *a4, void *a5,
    void *a6, void *a7, void *a8, void *a9, void *a10
);

static void * s_oci_hook_slot = NULL;
static void * s_oci_hook_orig = NULL;
static OCI_Int32    s_oci_hook_left = 0;

static sword OciTestGenericHook
(
    void *a1, void *a2, void *a3, void *a4, void *a5,
    void *a6, void *a7, void *a8, void *a9, void *a10
)
{
    if (s_oci_hook_left > 0 && --s_oci_hook_left == 0)
    {
        *(void **) s_oci_hook_slot = s_oci_hook_orig;
        s_oci_hook_slot = NULL;
        return OCI_ERROR;
    }

    return ((POCI_TEST_GENERIC_CALL) s_oci_hook_orig)
           (a1, a2, a3, a4, a5, a6, a7, a8, a9, a10);
}

void OCI_TestFailApiCall
(
    OCI_Environment *env,
    OCI_Int32              api,
    OCI_Int32              countdown
)
{
    void *slot = NULL;

    if (NULL == env || NULL == env->api || countdown < 1)
    {
        return;
    }

    switch (api)
    {
        case OCI_TEST_API_ATTR_GET:           slot = &env->api->OCIAttrGet;                 break;
        case OCI_TEST_API_ATTR_SET:           slot = &env->api->OCIAttrSet;                 break;
        case OCI_TEST_API_RAW_ASSIGN_BYTES:   slot = &env->api->OCIRawAssignBytes;          break;
        case OCI_TEST_API_SUBSCR_REGISTER:    slot = &env->api->OCISubscriptionRegister;    break;
        case OCI_TEST_API_SUBSCR_UNREGISTER:  slot = &env->api->OCISubscriptionUnRegister;  break;
        case OCI_TEST_API_TRANS_START:        slot = &env->api->OCITransStart;              break;
        case OCI_TEST_API_TRANS_DETACH:       slot = &env->api->OCITransDetach;             break;
        case OCI_TEST_API_TRANS_PREPARE:      slot = &env->api->OCITransPrepare;            break;
        case OCI_TEST_API_TRANS_FORGET:       slot = &env->api->OCITransForget;             break;
        case OCI_TEST_API_DESCRIBE_ANY:       slot = &env->api->OCIDescribeAny;             break;
        case OCI_TEST_API_PARAM_GET:          slot = &env->api->OCIParamGet;                break;
        case OCI_TEST_API_TYPE_BY_NAME:       slot = &env->api->OCITypeByName;              break;
        case OCI_TEST_API_CONN_POOL_DESTROY:  slot = &env->api->OCIConnectionPoolDestroy;   break;
        case OCI_TEST_API_SESS_POOL_DESTROY:  slot = &env->api->OCISessionPoolDestroy;      break;
        case OCI_TEST_API_SODA_DROP:          slot = &env->api->OCISodaCollDrop;            break;
        case OCI_TEST_API_SODA_TRUNCATE:      slot = &env->api->OCISodaCollTruncate;        break;
        case OCI_TEST_API_SODA_INSERT:        slot = &env->api->OCISodaInsert;              break;
        case OCI_TEST_API_SODA_FIND:          slot = &env->api->OCISodaFind;                break;
        case OCI_TEST_API_SODA_FIND_ONE:      slot = &env->api->OCISodaFindOne;             break;
        case OCI_TEST_API_SODA_DOC_GET_NEXT:  slot = &env->api->OCISodaDocGetNext;          break;
        case OCI_TEST_API_SODA_DOC_COUNT:     slot = &env->api->OCISodaDocCount;            break;
        case OCI_TEST_API_SODA_REPL_ONE:      slot = &env->api->OCISodaReplOne;             break;
        case OCI_TEST_API_SODA_REMOVE:        slot = &env->api->OCISodaRemove;              break;
        case OCI_TEST_API_SODA_INSERT_AND_GET: slot = &env->api->OCISodaInsertAndGet;       break;
        case OCI_TEST_API_SERVER_DETACH:       slot = &env->api->OCIServerDetach;           break;
        default:                                                                                   break;
    }

    if (NULL != slot)
    {
        s_oci_hook_slot = slot;
        s_oci_hook_orig = *(void **) slot;
        s_oci_hook_left = countdown;

        *(void **) slot = (void *) OciTestGenericHook;
    }
}

void OCI_TestApiRestore
(
    OCI_Environment *env
)
{
    (void) env;

    if (NULL != s_oci_hook_slot)
    {
        *(void **) s_oci_hook_slot = s_oci_hook_orig;
        s_oci_hook_slot = NULL;
    }
}

/* ------------------------------------------------------------------------ *
 * hash table internals
 * ------------------------------------------------------------------------ */

void * OCI_TestHashCreate
(
    OCI_Environment *env,
    OCI_UInt32     size,
    OCI_Int32              type
)
{
    return OcilibHashCreate(env, size, (OCI_HashType) type);
}

OCI_Boolean OCI_TestHashAddString
(
    void        *table,
    const otext *key,
    const otext *value
)
{
    return OcilibHashAddString((OCI_HashTable*) table, key, value);
}

OCI_Int32 OCI_TestHashEntryValueCount
(
    void         *table,
    OCI_UInt32  index
)
{
    OCI_HashEntry *e = OcilibHashGetEntry((OCI_HashTable*) table, index);
    OCI_Int32            n = 0;

    for (OCI_HashValue *v = (e ? e->values : NULL); NULL != v; v = v->next)
    {
        n++;
    }

    return n;
}

OCI_Boolean OCI_TestHashFree
(
    void *table
)
{
    return OcilibHashFree((OCI_HashTable*) table);
}

/* ------------------------------------------------------------------------ *
 * environment internals
 * ------------------------------------------------------------------------ */

OCI_Int32 OCI_TestEnvGetVariable
(
    OCI_Environment *env,
    const char      *name,
    char            *value,
    OCI_UInt32     size
)
{
    char *str = OcilibEnvironmentGetVariable(env, name);

    if (NULL == str)
    {
        return -1;
    }

    const OCI_Int32 len = (OCI_Int32) strlen(str);

    if (NULL != value && size > 0)
    {
        strncpy(value, str, size - 1);
        value[size - 1] = 0;
    }

    OcilibMemoryFree(env, str);

    return len;
}

OCI_UInt64 OCI_TestEnvGetStats
(
    OCI_Environment *env,
    OCI_UInt32     stat
)
{
    return OcilibEnvironmentGetStats(env, (OCI_Stat) stat);
}

OCI_Boolean OCI_TestEnvGetFormat
(
    OCI_UInt32 type
)
{
    OCI_Environment *env = (OCI_Environment *) calloc(1, sizeof(*env));

    if (NULL == env)
    {
        return FALSE;
    }

    env->env   = env;
    env->htype = OCI_IPC_ENVIRONMENT;

    const OCI_Boolean ok = (NULL != OcilibEnvironmentGetFormat(env, type));

    for (OCI_Int32 i = 0; i < OCI_FMT_COUNT; i++)
    {
        OcilibMemoryFree(env, env->formats[i]);
    }

    free(env);

    return ok;
}

/* ------------------------------------------------------------------------ *
 * logger internals
 * ------------------------------------------------------------------------ */

void OCI_TestLogReset
(
    void
)
{
    /* OcilibLogCleanup() decrements the process-wide reference count and is a
       no-op once it reaches zero, so calling it generously drains whatever
       references live environments currently hold. */

    for (OCI_Int32 i = 0; i < 32; i++)
    {
        OcilibLogCleanup();
    }
}

OCI_Boolean OCI_TestLogInit
(
    void
)
{
    return OcilibLogInitialize();
}

OCI_Boolean OCI_TestLogIsEnabled
(
    OCI_UInt32 category,
    OCI_UInt32 level
)
{
    return OcilibLogIsEnabled((OCI_LogCategory) category, (OCI_LogLevel) level);
}

void OCI_TestLogSetHandler
(
    POCI_LOG_HANDLER handler
)
{
    OcilibLogSetHandler(handler);
}

void OCI_TestLogWrite
(
    OCI_Environment *env,
    OCI_UInt32     category,
    OCI_UInt32     level,
    const char      *function,
    const otext     *message
)
{
    OcilibLogWrite(env, (OCI_LogCategory) category, (OCI_LogLevel) level, function, OTEXT("%s"), message);
}

const char * OCI_TestLogLevelName
(
    OCI_UInt32 level
)
{
    return OcilibLogGetLevelName((OCI_LogLevel) level);
}

const char * OCI_TestLogCategoryName
(
    OCI_UInt32 category
)
{
    return OcilibLogGetCategoryName((OCI_LogCategory) category);
}

OCI_Boolean OCI_TestLogStrEquals
(
    const char *value,
    const char *name
)
{
    return OcilibLogStrEquals(value, name);
}

OCI_Int32 OCI_TestLogCodepointToUTF8
(
    OCI_UInt32 code,
    char        *dst,
    OCI_Int32          dst_size
)
{
    return OcilibLogCodepointToUTF8(code, dst, dst_size);
}

OCI_Int32 OCI_TestLogNativeToUTF8
(
    const otext *src,
    char        *dst,
    OCI_Int32          dst_size
)
{
    return OcilibLogNativeToUTF8(src, dst, dst_size);
}
